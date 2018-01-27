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

#include <sakura/cdf.h>
#include <sakura/config.h>
#include <sakura/abi.h>
#include <sakura/backend-index.h>
#include <sakura/exception.h>
#include <foostring/foostring.h>

static const char filespec[] = "%s.cdf";

static const char spec_version[] = "version";
static const char spec_endianness[] = "endianness";
static const char spec_indices[] = "indices";
static const char spec_granularity[] = "index-granularity";
static const char spec_le[] = "little-endian";
static const char spec_be[] = "big-endian";

static int handle_var(const struct poliqarp_variable *var,
   struct poliqarp_cdf_info *cdf)
{
   if (strcmp(var->name, spec_version) == 0) {
      cdf->version = atoi(var->value);
      if (cdf->version < 1)
      {
         cdf->version = 1;
         return -1;
      }	 
      return 0;
   }
   else if (strcmp(var->name, spec_endianness) == 0) {
      if (strcmp(var->value, spec_le) == 0)
         cdf->endianness = ENDIAN_LE;
      else if (strcmp(var->value, spec_be) == 0)
         cdf->endianness = ENDIAN_BE;
      else
         return -1;
   } else if (strcmp(var->name, spec_indices) == 0) {
      int indices = poliqarp_index_calculate(var->value);
      if (indices == -1)
         return -1;
      else
         cdf->indices = indices;
   } else if (strcmp(var->name, spec_granularity) == 0) {
      int gran = atoi(var->value);
      if (gran < POLIQARP_INDEX_MIN_GRANULARITY)
         return -1;  
      if (gran > POLIQARP_INDEX_MAX_GRANULARITY)
         return -1;
      cdf->granularity = gran;
   } else 
      return -1;
   return 0;
}

void poliqarp_cdf_create(struct poliqarp_cdf_info *cdf)
{
   cdf->version = POLIQARP_ABI_VERSION;
   cdf->indices = 0;
   cdf->granularity = POLIQARP_INDEX_DEFAULT_GRANULARITY;
   cdf->endianness = ENDIAN_LE;
}

int poliqarp_cdf_read(const char *name, struct poliqarp_cdf_info *cdf)
{
   char *pathname, *line;
   FILE *f;
   struct poliqarp_variable var;
   int rc;

   poliqarp_cdf_create(cdf);
   cdf->version = 1;
   pathname = string_aformat(filespec, name);
   if (pathname == NULL)
      return -1;
   f = fopen(pathname, "rt");
   free(pathname);
   if (f == NULL)
      return -1;
   while ((line = poliqarp_fetch_line(f)) != NULL) {
      rc = poliqarp_parse_variable(&var, line);
      free(line);
      if (rc != 0)
         break;
      rc = handle_var(&var, cdf);
      poliqarp_free_variable(&var);
      if (rc != 0)
         break;
   }
   if (rc == 0 && !feof(f)) /* poliqarp_fetch_line() failed */ {
      /* Don't allow fclose() to overwrite errno. */
      int my_errno = errno;
      fclose(f);
      errno = my_errno;
      return -1;
   }
   fclose(f); /* Safe to ignore errors. */
   if (rc != 0)
      errno = EINVAL;
   return rc;
}

int poliqarp_cdf_write(const char *name, const struct poliqarp_cdf_info *cdf)
{
   char *pathname;
   FILE *f;
   
   pathname = string_aformat(filespec, name);
   if (pathname == NULL)
      return -1;
   f = fopen(pathname, "wt");
   free(pathname);
   if (f == NULL)
      return -1;
   fprintf(f, "%s = %d\n", spec_version, POLIQARP_ABI_VERSION);
   fprintf(f, "%s = %s\n", spec_endianness, 
      cdf->endianness == ENDIAN_LE ? spec_le : spec_be); 
   fprintf(f, "%s = %s\n", spec_indices, poliqarp_index_stringify(cdf->indices));
   fprintf(f, "%s = %" PRIuSIZE "\n", spec_granularity, cdf->granularity);
   if (ferror(f)) {
      /* Don't allow fclose() to overwrite errno. */
      int my_errno = errno;
      fclose(f);
      errno = my_errno;
      return -1;
   }
   if (fclose(f) != 0)
      return -1;
   return 0;
}
