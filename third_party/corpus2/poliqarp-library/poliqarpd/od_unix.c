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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include <foostring/foostring.h>

#include "osdep.h"

static char *config_dir = NULL;

const char *get_config_dir() 
{
   if (config_dir == NULL)
   {
      char* home = getenv("HOME");
      if (home == NULL)
         return "";
      config_dir = string_aformat("%s/.poliqarp/", home);
   }
   return config_dir;
}

void free_config_dir() {
   free(config_dir);
}

void poliqarp_sleep(int seconds)
{
   sleep(seconds);
}

void handle_signals()
{
   signal(SIGPIPE, SIG_IGN);
   signal(SIGINT, SIG_IGN);
}

int console_detach()
{
   switch (fork()) {
      case -1:
         return -1;
      case 0:
         break;
      default:
         _exit(0);
   }
   if (setsid() == -1)
      return -1;
   return 0;
}

void notify_readiness(unsigned long thread_id)
{
   /* Deliberately not implemented. */
}

void print_socket_error(const char *s)
{
   perror(s);
}
