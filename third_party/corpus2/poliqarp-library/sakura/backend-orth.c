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

#include <sakura/backend-orth.h>
#include <sakura/dict.h>
#include <sakura/exception.h>
#include <foostring/foostring.h>

/*==========[ ORTH BACKEND ]==============================================*/

int poliqarp_backend_orth_open(struct poliqarp_backend_orth *this, 
   const char *base_name, struct poliqarp_error *error)
{
   char *path;
   bool have_dict, have_afronte_index, have_atergo_index;
   int rc;

   have_dict = have_afronte_index = have_atergo_index = false;

   rc = poliqarp_newdict_open(&this->dict, base_name, true,
      POLIQARP_ORTH_IMAGE_FORMAT, POLIQARP_ORTH_OFFSET_FORMAT,
      _("Unable to open text dictionary"), error);
   if (rc != 0)
      goto error;
   have_dict = true;

   path = string_aformat(POLIQARP_ORTH_INDEX_FORMAT, base_name, "afronte");
   if (path == NULL)
      goto error_message;
   rc = newindex_open(&this->afronte_index, &this->dict, path);
   if (rc != 0) {
      /* try deprecated name */
      free(path);
      path = string_aformat(POLIQARP_ORTH_INDEX_FORMAT, base_name, "alpha");
      if (path == NULL)
         goto error_message;
      rc = newindex_open(&this->afronte_index, &this->dict, path);
      if (rc != 0) {
         poliqarp_error_from_system(error, _("Unable to open afronte index"
            "(%s)"), path);
      }
      free(path);
      if (rc != 0)
         goto error;
   }
   have_afronte_index = true;

   path = string_aformat(POLIQARP_ORTH_INDEX_FORMAT, base_name, "atergo");
   if (path == NULL)
      goto error_message;

   rc = newindex_open(&this->atergo_index, &this->dict, path);
   if (rc != 0) {
      poliqarp_error_from_system(error, _("Unable to open atergo index (%s)"),
         path);
   }
   free(path);
   if (rc != 0)
      goto error;
   have_atergo_index = true;
   return 0;
error_message:
   poliqarp_error_from_system(error, _("Unable to initialize orth backend"));
error:
   if (have_dict)
      newdict_close(&this->dict);
   if (have_afronte_index)
      newindex_close(&this->afronte_index);
   if (have_atergo_index)
      newindex_close(&this->atergo_index);
   return -1;
}

void poliqarp_backend_orth_close(struct poliqarp_backend_orth *this)
{
   newdict_close(&this->dict);
   newindex_close(&this->afronte_index);
   newindex_close(&this->atergo_index);
}

