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

#include <stdio.h>
#include <time.h>

#include "getpid.h"
#include "log.h"
#include "utils.h"

int init_log()
{
   if (cfg.logging_on) {
      cfg.log = fopen(string_str(cfg.logfile), "a");
      if (cfg.log == NULL)
         return -1;
   }
   return 0;
}

void done_log()
{
   if (cfg.logging_on) {
      if (cfg.log)
         fclose(cfg.log);
   }
}

void log_entry(const char *fmt, ...)
{
   va_list l;
   string_t s;
   struct tm *tm;
   time_t t;
   char timebuf[30];

   if (!cfg.logging_on)
      return;
   s = string_create();
   time(&t);
   lock(&cfg.logmutex);
   tm = localtime(&t);
   unlock(&cfg.logmutex);
   strftime(timebuf, sizeof timebuf, "%Y-%m-%d %H:%M:%S%z", tm);
   string_append_char(s, '[');
   string_append_str(s, timebuf);
   string_format_append(s, "] poliqarpd[%d]: ", getpid());
   va_start(l, fmt);
   string_vformat_append(s, fmt, l);
   va_end(l);   
   string_append_char(s, '\n');
   fputs(string_str(s), cfg.log);
   fflush(cfg.log);
   string_free(s);
}
