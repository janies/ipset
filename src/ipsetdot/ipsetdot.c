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


static gchar  *input_filename = "-";
static gchar  *output_filename = "-";


static GOptionEntry entries[] =
{
    { "input", 'i', 0,
      G_OPTION_ARG_FILENAME, &input_filename,
      "input file (\"-\" for stdin)", "FILE" },
    { "output", 'o', 0,
      G_OPTION_ARG_FILENAME, &output_filename,
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

    context = g_option_context_new("");
    g_option_context_add_main_entries(context, entries, NULL);

    if (!g_option_context_parse(context, &argc, &argv, &error))
    {
        fprintf(stderr, "Error parsing command-line options: %s\n",
                error->message);
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

    ip_set_t  *set = NULL;

    {
        GInputStream  *stream;

        /*
         * Create a raw GInputStream for the file.
         */

        if (strcmp(input_filename, "-") == 0)
        {
            fprintf(stderr, "Opening stdin...\n");
            input_filename = "stdin";

            stream = g_unix_input_stream_new(STDIN_FILENO, FALSE);
        } else {
            fprintf(stderr, "Opening file %s...\n", input_filename);

            GFile  *file =
                g_file_new_for_commandline_arg(input_filename);
            GFileInputStream  *fstream =
                g_file_read(file, NULL, &error);
            stream = G_INPUT_STREAM(fstream);

            if (fstream == NULL)
            {
                fprintf(stderr, "Cannot open file %s:\n  %s\n",
                        input_filename, error->message);
                exit(1);
            }
        }

        /*
         * Read in the IP set from the specified file.
         */

        set = ipset_load(stream, &error);

        if (set == NULL)
        {
            fprintf(stderr, "Error reading %s:\n  %s\n",
                    input_filename, error->message);
            exit(1);
        }
    }

    /*
     * Generate a GraphViz dot file for the set.
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

    if (!ipset_save_dot(ostream, set, &error))
    {
        fprintf(stderr, "Error saving IP set:\n  %s\n",
                error->message);
        exit(1);
    }

    ipset_free(set);

    /*
     * Close the output stream for exiting.
     */

    g_object_unref(ostream);

    return 0;
}
