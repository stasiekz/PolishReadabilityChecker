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

#include <assert.h>
#include <errno.h>
#include <string.h>

#include <sakura/config.h>
#include <sakura/common/getline.h>
#include <foostring/foostring.h>

/**
 * Trim the string.
 * @param s the string to trim
 * @param start pointer to the start of the trimmed part of string
 * @param end pointer to the end of (i.e. null byte) of the trimmed part of string
 */
static inline void poliqarp_trim_helper(const char *s, const char **start, const char **end)
{
   assert(s != NULL);
   *start = s;
   *end = strchr(s, '\0');
   /* Skip leading white space. */
   while (*start < *end && ascii_isspace(**start))
      (*start)++;
   /* Skip trailing white space. */
   while (*start < *end && ascii_isspace((*end)[-1]))
      (*end)--;
   assert(*start <= *end);
}

/**
 * Trim the string in-place. 
 * @param s the string to trim
 */
static void poliqarp_trim_inplace(char *s)
{
   const char *start, *end;
   poliqarp_trim_helper(s, &start, &end);
   s[end - s] = '\0';
   if (start != s)
      memmove(s, start, end - start + 1);
}

/**
 * Trim the string.
 * @param s the string to trim
 * @return the trimmed string, allocated by malloc(), or NULL in case of error
 */
static char *poliqarp_trim(char *s)
{
   char *result;
   const char *start, *end;
   poliqarp_trim_helper(s, &start, &end);
   result = malloc(end - start + 1);
   if (result == NULL)
      return NULL;
   memcpy(result, start, end - start);
   result[end - start] = '\0';
   return result;
}


char *poliqarp_fetch_line(FILE *stream)
{
   size_t n = 0;
   char *line = NULL;
   ssize_t read;

   do {
      if ((read = getline(&line, &n, stream)) == -1) {
         free(line);
         return NULL;
      }
      /* Cut comments, if any: */
      char *ptr = strchr(line, '#');
      if (ptr != NULL)
         *ptr = '\0';
      poliqarp_trim_inplace(line);
   } while (*line == '\0');
   return line;
}

int poliqarp_parse_variable(struct poliqarp_variable *this, char *text)
{
   char *ptr = strchr(text, '=');
   this->name = this->value = NULL;
   if (ptr == NULL) {
      errno = EINVAL;
      return -1;
   }
   *ptr = '\0';
   this->name = poliqarp_trim(text);
   if (this->name == NULL)
      return -1;
   this->value = poliqarp_trim(ptr + 1);
   if (this->value == NULL) {
      free(this->name);
      this->name = NULL;
      return -1;
   }
   return 0;
}

void poliqarp_free_variable(struct poliqarp_variable *this)
{
   free(this->name);
   free(this->value);
}

static int poliqarp_get_section_id(const char *name,
   const struct poliqarp_config_section *sections)
{
   const struct poliqarp_config_section *section = sections;
   for (; section->name; section++)
      if (strcasecmp(section->name, name) == 0)
         return section->id;
   return -1;
}

int poliqarp_parse_config_file(void *extra, const char *path,
   const struct poliqarp_config_section *sections,
   poliqarp_config_handler handler, struct poliqarp_error *error)
{
   FILE *fp = fopen(path, "rt");
   if (fp == NULL) {
      poliqarp_error_from_system(error, _("Unable to open configuration file (%s)"), path);
      return -1;
   }

   int rc = 0;
   int section = -1;
   char *line;
   while ((line = poliqarp_fetch_line(fp)) != NULL) {
      if (line[0] == '[') {  /* section declaration */
         const char *section_name;
         char *ptr = strrchr(line, ']');
         if (ptr != NULL)
            *ptr = 0;
         else {
            poliqarp_error_message_set(error,
               _("Malformed configuration file (%s):"
               " no ']' after section name"), path);
            rc = -1;
            break;
         }
         section_name = line + 1;
         section = poliqarp_get_section_id(section_name, sections);
         if (section < 0) {
            poliqarp_error_message_set(error,
               _("Malformed configuration file (%s):"
               " unknown section %s"), path, section_name);
            rc = -1;
            break;
         }
      } else if (section < 0) { 
         poliqarp_error_message_set(error,
               _("Malformed configuration file (%s):"
               " statement outside of any section"), path);
         break;
      } else {
         rc = handler(extra, section, line, error);
         if (rc != 0) {
            char *old_message = strdup(poliqarp_error_message_get(error));
            poliqarp_error_message_set(error,
               _("Unable to process configuration file (%s): %s"),
               path, old_message == NULL ? 
               _("possibly cannot allocate memory") : old_message);
               break;
            free(old_message);
         }
      }
      free(line);
      line = NULL;
   }
   if (rc == 0 && !feof(fp)) /* poliqarp_fetch_line() failed */ {
      poliqarp_error_from_system(error,
         "Unable to read configuration file (%s)", path);
      rc = -1;
   }
   free(line);
   fclose(fp); /* Safe to ignore errors. */
   return rc;
}
