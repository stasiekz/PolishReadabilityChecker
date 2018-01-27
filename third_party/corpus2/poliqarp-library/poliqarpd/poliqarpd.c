/*
 * This file is part of the Poliqarp suite.
 * 
 * Copyright (C) 2004-2009 by Instytut Podstaw Informatyki Polskiej
 * Akademii Nauk (IPI PAN; Institute of Computer Science, Polish
 * Academy of Sciences; cf. www.ipipan.waw.pl).  All rights reserved.
 * 
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file gpl.txt included in the packaging
 * of this file.  (See http://www.gnu.org/licenses/translations.html for
 * unofficial translations.)
 * 
 * A commercial license is available from IPI PAN (contact
 * Michal.Ciesiolka@ipipan.waw.pl or ipi@ipipan.waw.pl for more
 * information).  Licensees holding a valid commercial license from IPI
 * PAN may use this file in accordance with that license.
 * 
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
 * THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.
 */

#include <poliqarp-config.h>

#include <getopt.h>
#include <sakura/poliqarp.h>
#include <stdio.h>
#include <string.h>

#include "configuration.h" 
#include "log.h"
#include "osdep.h"
#include "server.h"

void display_help(const char *progname) 
{
   printf(_("Usage: %s OPTIONS\n\n"
      "  -h, --help              display this help and exit\n"
      "  -v, --version           display version information and exit\n"
      "  -g, --gui               GUI mode: cause the server to terminate when\n"
      "                          the session counter drops down to zero for\n"
      "                          the first time\n"
      "  -c, --config-file=FILE  use the given FILE as a configuration file\n"
      "                          instead of $HOME/.poliqarp/poliqarpd.conf\n"
      "  -d, --detach            detach from the controlling terminal and\n"
      "                          run in the background\n"
#if 0
      /* -n is intended for internal use only; keep it undocumented */
      "  -n, --notify=TID        notify thread TID that poliqarpd is ready\n"
      "                          to accept connections\n"
#endif
      "\n"
      "Report bugs to <" PACKAGE_BUGREPORT ">.\n"), 
      progname);
}

void display_version() 
{
   printf(_("poliqarpd version %s (compiled with %s library, "
      "version %d.%d.%d)\n"), POLIQARPD_VERSION, poliqarp_library_name,
      poliqarp_major_version, poliqarp_minor_version, 
      poliqarp_revision_number);
}

void init_subsystems()
{
   struct poliqarp_error error = poliqarp_error_none;
   if (poliqarp_create("" /* use the system locale */, &error) != 0) {
      fprintf(stderr, "%s.\n", poliqarp_error_message_get(&error));
      poliqarp_error_message_set(&error, NULL);
      exit(1);
   }
   if (init_log() == -1) 
      fprintf(stderr, _("Warning: could not initialize log"));
   handle_signals();
#ifdef HAVE_GETTEXT
   bindtextdomain("poliqarp", LOCALEDIR);
   textdomain("poliqarp");
#endif
}

void done_subsystems()
{
   done_log();
   poliqarp_destroy();
}

int main(int argc, char *argv[]) 
{
   struct option longopts[] = {
      { "help", 0, NULL, 'h' },
      { "version", 0, NULL, 'v' },
      { "gui", 0, NULL, 'g' },
      { "config-file", 1, NULL, 'c' },
      { "detach", 0, NULL, 'd' },
      { "notify", 1, NULL, 'n' },
      { NULL, 0, NULL, 0 }
   };
   int opt;
   string_t config_file = string_create();

   init_default_cfg();
   string_format(config_file, "%spoliqarpd.conf", get_config_dir());
   while ((opt = getopt_long(argc, argv, "hvgc:dn:", longopts, NULL)) != -1) {
      switch (opt) {
         case 'h': 
            display_help(*argv);
            return 0;
         case 'v':
            display_version();
            return 0;
         case 'g':
            cfg.gui_mode = true;
            break;
         case 'c':
            string_free(config_file);
            config_file = string_init(optarg);
            break;
         case 'd':
            cfg.detach = true;
            break;
         case 'n':
            cfg.notify_thread_id = strtoul(optarg, NULL, 10);
            /* deliberately ignore errors */
            break;
         default:
            exit(EXIT_FAILURE);
      }
   }

   if (read_cfg(string_str(config_file)) == -1)
      fprintf(stderr, _("Warning: could not access configuration file\n")); 
   string_free(config_file);

   init_subsystems();

   log_entry(_("server starting up"));
   server_loop();
   log_entry(_("server shutting down"));

   done_subsystems();
   done_cfg();
   free_config_dir();
   return 0;
}
