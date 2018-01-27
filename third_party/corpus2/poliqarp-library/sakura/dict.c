/*
 * This file is part of the Poliqarp suite.
 * 
 * Copyright (C) 2009 by Instytut Podstaw Informatyki Polskiej
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

#include <foostring/foostring.h>

#include <sakura/dict.h>
#include <sakura/exception.h>

int poliqarp_newdict_open(struct newdict *dict,
   const char *base_name, bool want_index,
   const char *image_path_format, const char *offset_path_format,
   const char *error_message, struct poliqarp_error *error)
{
   char *image_path = NULL, *offset_path = NULL;
   int rc;
   image_path = string_aformat(image_path_format, base_name);
   if (image_path == NULL)
      goto error;
   offset_path = string_aformat(offset_path_format, base_name);
   if (offset_path == NULL)
      goto error;
   rc = newdict_open(dict, image_path, offset_path, want_index);
   if (rc == 0)
      goto done;
error:
   if (image_path == NULL || offset_path == NULL) {
      poliqarp_error_from_system(error, "%s", error_message);
   } else {
      poliqarp_error_from_system(error, _("%s (%s or %s)"), error_message,
         image_path, offset_path);
   }
   rc = -1;
done:
   free(image_path);
   free(offset_path);
   return rc;
}

