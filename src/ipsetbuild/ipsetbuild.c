/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */


#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>
#include <gio/gio.h>
#include <gio/gunixinputstream.h>
#include <gio/gunixoutputstream.h>

#include <ipset/ipset.h>


static gchar  *output_filename = NULL;


static GOptionEntry entries[] =
{
    { "output", 'o', 0, G_OPTION_ARG_FILENAME, &output_filename,
      "output file (\"-\" for stdout)", "FILE" },
    { NULL }
};


/**
 * A log handler that ignores the logging messages.
 */

static void
ignore_log_message(const gchar *log_domain, GLogLevelFlags log_level,
                   const gchar *message, gpointer user_data)
{
}


int
main(int argc, char **argv)
{
    g_type_init();
    ipset_init_library();

    /*
     * Parse the command-line options.
     */

    GError  *error = NULL;
    GOptionContext  *context;

    context = g_option_context_new("INPUT FILES");
    g_option_context_add_main_entries(context, entries, NULL);

    if (!g_option_context_parse(context, &argc, &argv, &error))
    {
        fprintf(stderr, "Error parsing command-line options: %s\n",
                error->message);
        exit(1);
    }

    /*
     * Verify that the user specified at least one SiLK file to read.
     */

    if (argc <= 1)
    {
        fprintf(stderr, "ERROR: You need to specify at "
                "least one input file.\n\n%s",
                g_option_context_get_help(context, TRUE, NULL));
        exit(1);
    }

    /*
     * Set up logging.
     */

    g_log_set_handler("ipset", G_LOG_LEVEL_DEBUG,
                      ignore_log_message, NULL);

    /*
     * Read in the IP set files specified on the command line.
     */

    ip_set_t  set;
    ipset_init(&set);

    int  i;
    for (i = 1; i < argc; i++)
    {
        const char  *filename = argv[i];
        GInputStream  *stream;

        /*
         * Create a raw GInputStream for the file.
         */

        if (strcmp(filename, "-") == 0)
        {
            fprintf(stderr, "Opening stdin...\n");
            filename = "stdin";

            stream = g_unix_input_stream_new(STDIN_FILENO, FALSE);
        } else {
            fprintf(stderr, "Opening file %s...\n", filename);

            GFile  *file = g_file_new_for_commandline_arg(filename);
            GFileInputStream  *fstream =
                g_file_read(file, NULL, &error);
            stream = G_INPUT_STREAM(fstream);

            if (fstream == NULL)
            {
                fprintf(stderr, "Cannot open file %s:\n  %s\n",
                        filename, error->message);
                exit(1);
            }
        }

        /*
         * Wrap the GInputStream in a GDataInputStream so that it's
         * easy to read individual lines.
         */

        GDataInputStream  *distream =
            g_data_input_stream_new(stream);

        /*
         * Read in one IP address per line in the file.
         */

        gsize  ip_count = 0;

        gsize  line_length;
        gchar  *line;


        while ((line = g_data_input_stream_read_line
                (distream, &line_length, NULL, &error))
               != NULL)
        {
            /*
             * Reserve enough space for an IPv6 address.
             */

            guint32  addr[4];
            int  rc;

            /*
             * Try to parse the line as an IPv4 address.  If that
             * works, add it to the set.
             */

            rc = inet_pton(AF_INET, line, addr);
            if (rc == 1)
            {
                ipset_ipv4_add(&set, addr);
                g_free(line);
                continue;
            }

            /*
             * If that didn't work, try IPv6.
             */

            rc = inet_pton(AF_INET6, line, addr);
            if (rc == 1)
            {
                ipset_ipv6_add(&set, addr);
                g_free(line);
                continue;
            }

            /*
             * Otherwise, we've got an error.
             */

            fprintf(stderr, "\"%s\" is not a valid IP address.\n", line);
            exit(1);
        }

        if (error != NULL)
        {
            /*
             * There was an error reading from the stream.
             */

            fprintf(stderr, "Error reading from %s:\n  %s\n",
                    filename, error->message);
            exit(1);
        }

        fprintf(stderr, "Read %" G_GSIZE_FORMAT " IP addresses from %s.\n",
                ip_count, filename);

        /*
         * Free the streams before opening the next file.
         */

        g_object_unref(distream);
        g_object_unref(stream);
    }

    fprintf(stderr, "Set uses %" G_GSIZE_FORMAT " bytes of memory.\n",
            ipset_memory_size(&set));

    /*
     * Serialize the IP set to the desired output file.
     */

    GOutputStream  *ostream;

    if ((output_filename == NULL) ||
        (strcmp(output_filename, "-") == 0))
    {
        fprintf(stderr, "Writing to stdout...\n");

        ostream = g_unix_output_stream_new(STDOUT_FILENO, FALSE);
        output_filename = "stdout";
    } else {
        fprintf(stderr, "Writing to file %s...\n", output_filename);

        GFile  *file = g_file_new_for_commandline_arg(output_filename);
        GFileOutputStream  *fstream =
            g_file_replace(file, NULL, FALSE, G_FILE_CREATE_NONE,
                           NULL, &error);
        ostream = G_OUTPUT_STREAM(fstream);

        if (fstream == NULL)
        {
            fprintf(stderr, "Cannot open file %s:\n  %s\n",
                    output_filename, error->message);
            exit(1);
        }
    }

    if (!ipset_save(ostream, &set, &error))
    {
        fprintf(stderr, "Error saving IP set:\n  %s\n",
                error->message);
        exit(1);
    }

    /*
     * Close the output stream for exiting.
     */

    g_object_unref(ostream);

    return 0;
}
