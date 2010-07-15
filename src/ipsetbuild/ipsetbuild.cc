/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */


#include <fstream>
#include <iostream>
#include <string>

#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>
#include <glog/logging.h>

#include <ip/ip.hh>
#include <ip/set.hh>


namespace po = boost::program_options;
using namespace ip;


void
noop_deallocator(void *)
{
}


void
show_usage(std::ostream &stream, po::options_description &opts)
{
    std::cout << "Usage: ipsetbuild [options] [input files...]"
              << std::endl << std::endl
              << opts;
}


int
main(int argc, char **argv)
{
    // Define the command-line options.

    typedef std::vector<std::string>  string_vec_t;

    po::options_description  opts("Options");
    opts.add_options()
        ("help", "show help message")
        ("output", po::value<std::string>()->default_value("-"),
         "output file (\"-\" for stdout)")
        ("input", po::value<string_vec_t>(),
         "input files (\"-\" for stdin)")
        ;

    po::positional_options_description  pos;
    pos.add("input", -1);

    po::variables_map  vm;
    try
    {
        po::store(po::command_line_parser(argc, argv).
                  options(opts).positional(pos).run(), vm);
        po::notify(vm);
    } catch (po::error &e) {
        std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
        show_usage(std::cerr, opts);
        return 1;
    }

    // Output a help message if requested.

    if (vm.count("help"))
    {
        show_usage(std::cout, opts);
        return 1;
    }

    // Verify that the user specified at least one SiLK file to read.

    if (vm.count("input") == 0)
    {
        std::cerr << "ERROR: You need to specify at least one input file."
                  << std::endl << std::endl;
        show_usage(std::cerr, opts);
        return 1;
    }

    // Set up logging.

    FLAGS_logtostderr = true;
    google::InitGoogleLogging(argv[0]);

    // Read in the IP set files specified on the command line.

    set_t  set;
    string_vec_t  filenames = vm["input"].as<string_vec_t>();

    for (string_vec_t::iterator it = filenames.begin();
         it != filenames.end();
         ++it)
    {
        std::string  &filename = *it;
        boost::shared_ptr<std::istream>  stream;

        if (filename == "-")
        {
            std::cerr << "Opening stdin" << std::endl;
            stream.reset(&std::cin, noop_deallocator);
            filename = "stdin";
        } else {
            std::cerr << "Opening file " << filename << std::endl;

            stream.reset(new std::ifstream
                         (filename.c_str(),
                          std::ios_base::in | std::ios_base::binary));

            if (stream->fail())
            {
                std::cerr << "Cannot open file " << filename << std::endl;
                continue;
            }
        }

        size_t  ip_count = 0;
        std::string  ip;

        while (*stream)
        {
            if (std::getline(*stream, ip))
            {
                try
                {
                    ip_count++;
                    ip_addr_t  addr(ip);
                    set.add(addr);
                } catch (bad_ip_format &) {
                    std::cerr << '"' << ip << '"'
                              << " is not a valid IP address"
                              << std::endl;
                    return 1;
                }
            }
        }

        if (!stream->eof())
        {
            std::cerr << "Error reading from " << filename
                      << std::endl;
            return 1;
        }

        std::cerr << "Read " << ip_count << " IP addresses from "
                  << filename << std::endl;
    }

    std::cerr << "Set uses " << set.memory_size()
              << " bytes of memory." << std::endl;

    // Serialize the IP set to the desired output file.

    std::string  output_filename = vm["output"].as<std::string>();
    boost::shared_ptr<std::ostream>  stream;

    if (output_filename == "-")
    {
        std::cerr << "Writing to stdout" << std::endl;
        stream.reset(&std::cout, noop_deallocator);
        output_filename = "stdout";
    } else {
        std::cerr << "Writing to file " << output_filename
                  << std::endl;

        stream.reset(new std::ofstream
                     (output_filename.c_str(),
                      std::ios_base::out | std::ios_base::binary));

        if (stream->fail())
        {
            std::cerr << "Cannot open file " << output_filename
                      << std::endl;
            return 1;
        }
    }

    set.save(*stream);

    return 0;
}
