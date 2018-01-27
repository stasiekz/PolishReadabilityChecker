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

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#include <foostring/foostring.h>

size_t strnlen(const char *str, size_t maxlen);

static void default_oom_callback(string_t s)
{
   return;
}

void (*string_oom_callback)(string_t) = default_oom_callback;

/**
 * A helper function determining the buffer size.
 */
static inline size_t string_capacity_advice(size_t request) 
{
   size_t size = 16;
   if (request > SIZE_MAX / 2)
      return SIZE_MAX;
   while (size < request)
      size *= 2;
   return size;
}

/**
 * Enlarges the buffer to new_size bytes.
 *
 * @return 0 on success, -1 on error
 */
static inline int string_enlarge(string_t s, size_t new_size)
{
   assert(new_size >= s->size);
   assert(new_size != 0);
   if (new_size > s->size) {
      char *new_cstring;
      new_cstring = realloc(s->str, new_size);
      if (new_cstring == NULL) {
         string_oom_callback(s);
         return -1;
      }
      s->str = new_cstring;
      s->size = new_size;
   }
   return 0;
}

/**
 * Ensures that a buffer will be able to contain at least count
 * additional bytes.
 *
 * @return 0 on success, -1 on error
 */
static inline int string_ensure_extra_capacity(string_t s, size_t count)
{
   if (count >= SIZE_MAX - s->len)
      return -1;
   if (s->size <= s->len + count + 1)
      return string_enlarge(s, string_capacity_advice(s->len + count + 1));
   return 0;
}

/**
 * Ensures that a buffer will be able to contain at least count bytes
 * (not counting the terminal null byte).
 *
 * @return 0 on success, -1 on error
 */
static inline int string_ensure_capacity(string_t s, size_t count)
{
   if (count == SIZE_MAX)
      return -1;
   if (s->size <= count + 1)
      return string_enlarge(s, string_capacity_advice(count + 1));
   return 0;
}


/**
 * Setting this to non-zero will cause the library to collect
 * statistics regarding string usage in the program.  This can be
 * useful for profiling purposes.
 *
 * Unfortunately, turning this settings on makes foostring routines
 * non-reentrant.
 */
#define STRING_STATS 0

/**
 * Setting this to a non-zero value will cause the library to store
 * at most STRING_CACHE_SIZE string_t structures in the allocator's
 * cache.  This will limit the number of allocations and theoretically
 * improve the program's efficiency.
 *
 * Unfortunately, turning this settings on makes foostring routines
 * non-reentrant, so it's turned off by default.
 **/
#define STRING_CACHE_SIZE 0

#if defined(STRING_CACHE_SIZE) && STRING_CACHE_SIZE > 0

/**
 * List of free string structures.  Pointer to the character
 * buffer is treated as pointer to the next element.
 **/
static string_t free_string_list = NULL;

/**
 * Number of elements in the above list.
 **/
static size_t free_string_count = 0;

#endif

#if defined(STRING_STATS) && STRING_STATS > 0

/**
 * Maximum number of allocated strings observed.
 **/
static size_t peek_string_count = 0;

/**
 * Current number of allocated strings.
 **/
static size_t curr_string_count = 0;

#endif

/**
 * Allocates a string_t structure.  If string cache is enabled, tries
 * to use free strings from the cache.
 **/
static string_t string_alloc() 
{
   string_t this;
#if defined (STRING_CACHE_SIZE) && STRING_CACHE_SIZE > 0
   if (free_string_list) {
      this = free_string_list;
      free_string_count -= 1;
      free_string_list = (void *)this->str;
   } else
      this = malloc(sizeof *this);
#else
   this = malloc(sizeof *this);
#endif
   if (this == NULL) {
      string_oom_callback(NULL);
      return NULL;
   }
#if defined (STRING_STATS) && STRING_STATS > 0
   ++curr_string_count;
   if (curr_string_count > peek_string_count)
      peek_string_count = curr_string_count;
#endif
   return this;
}

/**
 * Frees a string.  If the program is compiled with string cache
 * enabled, tries to store the string on the list of free elements.
 **/
static void string_free_internal(string_t this)
{
#if defined (STRING_CACHE_SIZE) && STRING_CACHE_SIZE > 0
   if (free_string_count < STRING_CACHE_SIZE) {
      this->str = (void *)free_string_list;
      free_string_list = this;
      free_string_count += 1;
   } else
      free (this);
#else
   free (this);
#endif
#if defined (STRING_STATS) && STRING_STATS > 0
   --curr_string_count;
#endif
}

string_t string_create()
{
   string_t this = string_alloc();
   if (this == NULL)
      return NULL;
   this->str = malloc(1);
   if (this->str == NULL) {
      free(this);
      string_oom_callback(NULL);
      return NULL;
   }
   this->str[0] = 0;
   this->len = 0;
   this->size = 1;
   return this;
}

string_t string_init(const char *c_string)
{
   string_t this;
   size_t length;

   assert(c_string != NULL);
   
   this = string_alloc();
   if (this == NULL)
      return NULL;
   length = strlen(c_string);
   this->str = malloc(length + 1);
   if (this->str == NULL) {
      free(this);
      string_oom_callback(NULL);
      return NULL;
   }
   this->len = length;
   this->size = length + 1;
   memcpy(this->str, c_string, length);
   this->str[this->len] = 0;
   return this;
}

string_t string_init_n(const char *c_string, size_t n)
{
   string_t this;
   size_t length;
   
   assert(c_string != NULL);
   
   this = string_alloc();
   if (this == NULL)
      return NULL;
   length = strnlen(c_string, n);
   this->str = malloc(length + 1);
   if (this->str == NULL) {
      free(this);
      string_oom_callback(NULL);
      return NULL;
   }
   this->len = length;
   this->size = length + 1;
   memcpy(this->str, c_string, length);
   this->str[this->len] = 0;
   return this;
}

void string_free(string_t s)
{
   assert(s != NULL);
   free(s->str);
   s->len = -1; 
   s->size = 0;
   string_free_internal(s);
}

char *string_free_and_get_buffer(string_t s)
{
   char *buffer;
   assert (s != NULL);

   buffer = s->str;
   string_free_internal(s);
   return buffer;
}

int string_append(string_t s, const string_t s2)
{
   assert(s != NULL);
   assert(s2 != NULL);

   return string_append_buf(s, s2->str, s2->len);
}

int string_append_buf(string_t s, const char *buffer, size_t count)
{
   assert(s != NULL);
   assert(buffer != NULL);
   
   if (string_ensure_extra_capacity(s, count) != 0)
     return -1; 
   s->str[s->len + count] = 0;
   memcpy(s->str + s->len, buffer, count);
   s->len += count;
   return 0;
}

int string_append_str(string_t s, const char *c_string)
{
   assert(s != NULL);
   assert(c_string != NULL);

   return string_append_buf(s, c_string, strlen(c_string));
}

int string_append_strn(string_t s, const char *c_string, size_t n)
{
   assert(s != NULL);
   assert(c_string != NULL);

   return string_append_buf(s, c_string, strnlen(c_string, n));
}

int string_append_char(string_t s, char c)
{
   assert (s != NULL);

   if (string_ensure_extra_capacity(s, 1) != 0)
      return -1;
   s->str[s->len + 1] = 0;
   s->str[s->len++] = c;
   return 0;
}

int string_append_long(string_t s, long value)
{
   char buffer[CHAR_BIT * sizeof (long) / 3 + 2];
   sprintf(buffer, "%ld", value);
   return string_append_str(s, buffer);
}

int string_append_unsigned_long(string_t s, unsigned long value)
{
   char buffer[CHAR_BIT * sizeof (unsigned long) / 3 + 2];
   sprintf(buffer, "%lu", value);
   return string_append_str(s, buffer);
}

int string_insert_strn(string_t s, size_t index, const char *c_string,
   size_t count)
{
   assert(s != NULL);
   assert(c_string != NULL);
   
   count = strnlen(c_string, count);
   if (string_ensure_extra_capacity(s, count) != 0)
      return -1;
   memmove(s->str + index + count, s->str + index, s->len + 1 - index);
   memmove(s->str + index, c_string, count);
   s->len += count;
   return 0;
}

int string_insert_str(string_t s, size_t index, const char *c_string)
{
   assert(s != NULL);
   assert(c_string != NULL);

   return string_insert_strn(s, index, c_string, -1);
}

void string_clear(string_t s)
{
   assert(s != NULL);
   
   if (s->size > string_capacity_advice(0)) {
      char *new_cstring;
      int my_errno = errno;
      s->size = string_capacity_advice(0);
      new_cstring = realloc(s->str, s->size);
      if (new_cstring != NULL)
         s->str = new_cstring;
      else {
         errno = my_errno;
         /* Swallow possible errors, as nothing terrible happened. */
      }
   }
   s->str[0] = 0;
   s->len = 0;
}

int string_format(string_t s, const char *format, ...)
{
   int rc;
   va_list ap;

   va_start(ap, format);
   string_clear(s);
   rc = string_vformat_append(s, format, ap);
   va_end(ap);
   return rc;
}

int string_format_append(string_t s, const char *format, ...)
{
   int rc;
   va_list ap;

   va_start(ap, format);
   rc = string_vformat_append(s, format, ap);
   va_end(ap);
   return rc;
}

int string_vformat(string_t s, const char *format, va_list ap)
{
   string_clear(s);
   return string_vformat_append(s, format, ap);
}

int string_vformat_append(string_t s, const char *format, va_list ap)
{
   int rc;
   const char *f = format;

again:
   /* copy as many characters as we can */
   while (*f != 0 && *f != '%') {
      rc = string_append_char(s, *f++);
      if (rc != 0)
         return -1;
   }
   
   /* if we have to take care of formatting, go ahead :-) */
   if (*f++ == '%') {
      char c;
      struct {
         bool f_long;
      } flags;
      
      flags.f_long = false;
format_again:
      rc = 0;
      switch (c = *f++) {
         case 0:
            break;
         case '%':
            rc = string_append_char(s, '%');
            break;
         case 'c':
            {
               char ch = (char)va_arg(ap, int);
               rc = string_append_char(s, ch);
            }
            break;
         case 's':
            {
               const char *text = va_arg(ap, const char *);
               rc = string_append_str(s, text);
            }
            break;
         case 'S':
            {
               string_t text = va_arg(ap, string_t);
               rc = string_append(s, text);
            }
            break;
         case 'd':
            if (flags.f_long) {
               long d = va_arg(ap, long);
               rc = string_append_long(s, d);
            } else {
               int d = va_arg(ap, int);
               rc = string_append_long(s, d);
            }
            break;
         case 'u':
            if (flags.f_long) {
               unsigned long d = va_arg(ap, unsigned long);
               rc = string_append_unsigned_long(s, d);
            } else {
               unsigned int d = va_arg(ap, unsigned int);
               rc = string_append_unsigned_long(s, d);
            }
            break;
         case 'l':
            flags.f_long = true;
            goto format_again;
         default:
            rc = string_append_char(s, c);
            break;
      }
      if (rc != 0)
         return -1;
      /* formatting complete, let's see whether we have some more
         characters to handle */
      goto again;
   } /* if (*f == '%') */
   return 0;
}

char *string_avformat(const char *format, va_list ap)
{
   int rc;
   string_t s = string_create();
   char *cstring;
   
   rc = string_vformat_append(s, format, ap);
   cstring = string_free_and_get_buffer(s);
   if (rc == 0)
      return cstring;
   else {
      free(cstring);
      return NULL;
   }
}

char *string_aformat(const char *format, ...)
{
   int rc;
   va_list ap;
   string_t s = string_create();
   char *cstring;
   
   va_start(ap, format);
   rc = string_vformat_append(s, format, ap);
   va_end(ap);
   cstring = string_free_and_get_buffer(s);
   if (rc == 0)
      return cstring;
   else {
      free(cstring);
      return NULL;
   }
}

string_t string_fgets(FILE *f)
{
   char buf[BUFSIZ];
   string_t res = string_create();
   if (res == NULL)
      return NULL;
   
   while (fgets(buf, sizeof buf, f)) {
      int rc = string_append_str(res, buf);
      if (rc != 0)
         return NULL;
      if (strchr(buf, '\n'))
         break;
   }
   if (string_len(res) == 0) {
      string_free(res);
      return NULL;
   }
   return res;
}

int string_cmp(const string_t s1, const string_t s2)
{
   return strcmp(string_str(s1), string_str(s2));
}

int string_ccmp(const string_t s1, const char *s2)
{
   return strcmp(string_str(s1), s2);
}

int string_printf(const char *format, ...)
{
   int result;
   va_list ap;
   va_start(ap, format);
   result = string_vfprintf(stdout, format, ap);
   va_end(ap);
   return result;
}

int string_fprintf(FILE *stream, const char *format, ...)
{
   int result;
   va_list ap;
   va_start(ap, format);
   result = string_vfprintf(stream, format, ap);
   va_end(ap);
   return result;
}

int string_vprintf(const char *format, va_list ap)
{
   return string_vfprintf(stdout, format, ap);
}

int string_vfprintf(FILE *stream, const char *format, va_list ap)
{
   string_t text;
   int rc;

   text = string_create();
   rc = string_vformat(text, format, ap);
   if (rc != 0)
      return -1;
   rc = fputs(text->str, stream);
   string_free(text);
   return -(rc > 0);
}

void string_purge(string_t str)
{
   str->str[0] = '\0';
   str->len = 0;
}
