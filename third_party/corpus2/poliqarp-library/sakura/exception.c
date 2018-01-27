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

#include <assert.h>
#include <string.h>

#include <foostring/foostring.h>

#include <sakura/exception.h>
#include <sakura/regexp.h>

static char *surrogate_message = "Possibly cannot allocate memory";

void poliqarp_error_set(struct poliqarp_error *this, struct poliqarp_error *that)
{
   *this = *that;
   *that = poliqarp_error_none;
}

void poliqarp_error_message_set(struct poliqarp_error *this, const char *fmt, ...)
{
   int my_errno = errno;
   if (this->malloced)
      free(this->message);
   if (fmt == NULL) {
      *this = poliqarp_error_none;
      return;
   }
   va_list ap;
   va_start(ap, fmt);
   this->message = string_avformat(fmt, ap);
   va_end(ap);
   if (this->message == NULL) {
      this->message = surrogate_message;
      this->malloced = false;
   } else
      this->malloced = true;
   errno = my_errno;
}

const char *poliqarp_error_message_get(const struct poliqarp_error *this)
{
   return this->message;
}

void poliqarp_error_from_system_n(int errnum, struct poliqarp_error *this, const char *fmt, ...)
{
   int my_errno = errno;
   if (this->malloced)
      free(this->message);
   const char *error_message;
#ifdef HAVE_STRERROR_R
   char error_message_buf[BUFSIZ];
#ifdef STRERROR_R_CHAR_P
   error_message = strerror_r(errnum, error_message_buf,
      sizeof error_message_buf);
#else
   strerror_r(errno, error_message_buf, sizeof error_message_buf);
   error_message = error_message_buf;
#endif
#else
   error_message = strerror(errno);
   /* FIXME: strerror() call is *might* not be thread-safe. */
#endif
   string_t message = string_create();
   if (fmt != NULL) {
      va_list ap;
      va_start(ap, fmt);
      string_vformat(message, fmt, ap);
      va_end(ap);
      string_append_str(message, ": "); 
   }
   string_append_str(message, error_message);
   this->message = string_free_and_get_buffer(message);
   if (this->message == NULL) {
      this->message = surrogate_message;
      this->malloced = false;
   } else
      this->malloced = true;
   errno = my_errno;
}

void poliqarp_error_from_regexp(struct poliqarp_error *this,
   const struct poliqarp_regexp *regexp, const char *fmt, ...)
{
   int my_errno = errno;
   assert(regexp->status != 0);
   if (regexp->message == NULL) {
      if (this->malloced)
         free(this->message);
      this->message = surrogate_message;
      this->malloced = false;
      return;
   }
   string_t message = string_create();
   if (fmt != NULL) {
      va_list ap;
      va_start(ap, fmt);
      string_vformat(message, fmt, ap);
      va_end(ap);
      string_append_str(message, ": "); 
   }
   string_append_str(message, regexp->message);
   this->message = string_free_and_get_buffer(message);
   if (this->message == NULL) {
      this->message = surrogate_message;
      this->malloced = false;
   } else
      this->malloced = true;
   my_errno = errno;
}
