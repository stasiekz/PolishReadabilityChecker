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

#include <sakura/backend-tag.h>
#include <sakura/dict.h>
#include <sakura/exception.h>
#include <foostring/foostring.h>
#include <sys/types.h>

int poliqarp_backend_tag_open(struct poliqarp_backend_tag *this,
   struct poliqarp_backend_config *backend_config, const char *base_name,
   struct poliqarp_error *error)
{
   size_t i;
   size_t num_tags;
   size_t num_attr;
   int rc;

   rc = poliqarp_newdict_open(&this->dict, base_name, false,
      POLIQARP_TAG_IMAGE_FORMAT, POLIQARP_TAG_OFFSET_FORMAT,
      _("Unable to open tag dictionary"), error);
   if (rc != 0)
      return rc;

   this->parsed_tag = NULL;
   this->parsed_tag_memory = NULL;

   num_tags = poliqarp_backend_tag_num_items(this);
   num_attr = backend_config->num_attr;

   this->parsed_tag = malloc(sizeof *this->parsed_tag * num_tags);
   if (this->parsed_tag != NULL)
      this->parsed_tag_memory = 
         malloc(sizeof *this->parsed_tag[i].attr_value * num_attr * num_tags);
   if (this->parsed_tag == NULL || this->parsed_tag_memory == NULL) {
      poliqarp_error_from_system(error, _("Unable to parse tags"));
      goto error;
   }
   for (i = 0; i < num_tags; ++i) {
      const char *tag_name;
      this->parsed_tag[i].attr_value = (struct poliqarp_attr_value **)
         (this->parsed_tag_memory) + i * num_attr;
      tag_name = poliqarp_backend_tag_fetch(this, i);
      rc = poliqarp_backend_config_parse_copy(backend_config,
         this->parsed_tag + i, tag_name);
      if (rc != 0) {
         if (errno == EINVAL || errno == ENOENT)
            poliqarp_error_message_set(error, _("%s is not a valid tag"), tag_name);
         else
            poliqarp_error_from_system(error, _("Unable to parse tags"));
         goto error;
      }
   }
   return 0;
error:
   newdict_close(&this->dict);
   free(this->parsed_tag);
   free(this->parsed_tag_memory);
   return -1;
}

void poliqarp_backend_tag_close(struct poliqarp_backend_tag *this)
{
   newdict_close(&this->dict);
   free(this->parsed_tag);
   free(this->parsed_tag_memory);
}
