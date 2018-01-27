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

#include <sakura/backend-syntax.h>

#include <sakura/dict.h>
#include <foostring/foostring.h>

static int syntax_next_bit(void *data)
{
   struct poliqarp_backend_syntax *syntax = 
      (struct poliqarp_backend_syntax *)data;
   uint8_t *buffer = (uint8_t *)syntax->data.image;
   int res = buffer[syntax->pos / 8] & (1 << (syntax->pos % 8));
   syntax->pos++;
   return res;
}

int poliqarp_backend_syntax_open(struct poliqarp_backend_syntax *this,
   const char *base_name, struct poliqarp_error *error)
{
   bool have_dict, have_image, have_offsets, have_groups;
   char *path;
   int rc;
   have_dict = have_image = have_offsets = have_groups = false;
   rc = poliqarp_newdict_open(&this->dict_types, base_name, true,
      "%s.poliqarp.grtype.image", "%s.poliqarp.grtype.offset",
      _("Unable to open group types dictionary"), error);
   if (rc != 0) {
      if (errno == ENOENT) {
         /* Presumably there's no syntax data. */
         poliqarp_error_message_set(error, NULL);
         this->syntax = false;
         return 0;
      }
      goto error;
   }
   have_dict = true;

   path = string_aformat("%s.poliqarp.syntax.image", base_name);
   if (path == NULL)
      goto error_message;
   rc = create_file_map(&this->data, path);
   if (rc != 0) {
      poliqarp_error_from_system(error, _("Unable to open syntax image"
         " (%s)"), path);
   }
   free(path);
   if (rc != 0)
      goto error;
   have_image = true;

   this->numbits = 8 * this->data.size - 8 + 
      ((uint8_t *)this->data.image)[this->data.size - 1];
  
   path = string_aformat("%s.poliqarp.syntax.offset", base_name);
   if (path == NULL)
      goto error_message;
   rc = tinydb_open(&this->offsets, path, sizeof(uint32_t));
   if (rc != 0) {

   }
   free(path);
   if (rc != 0)
      goto error;
   have_offsets = true;
   
   this->size = 4096;
   this->groups = malloc(this->size * sizeof(struct poliqarp_syntax_group));
   if (this->groups == NULL)
      goto error_message;
   have_groups = true;

   this->bitstream.next_bit = syntax_next_bit;
   this->bitstream.data = this;
   this->pos = this->start = this->end = 0;
   this->syntax = true;
   return 0;

error_message:
   poliqarp_error_from_system(error, _("Unable initialize syntax backend"));
error:
   if (have_dict)
      newdict_close(&this->dict_types);
   if (have_image)
      destroy_file_map(&this->data);
   if (have_offsets)
      tinydb_close(&this->offsets);
   if (have_groups)
      free(this->groups);
   return -1;
}

void poliqarp_backend_syntax_close(struct poliqarp_backend_syntax *this)
{
   if (this->syntax) {
      free(this->groups);
      tinydb_close(&this->offsets);
      destroy_file_map(&this->data);
      newdict_close(&this->dict_types);
      this->syntax = false;
   }
}

static int poliqarp_backend_syntax_next_internal(struct poliqarp_backend_syntax *this,
   int recursive)
{
   int synchro;
   size_t old_end = this->end;
   struct poliqarp_syntax_group *group = this->groups + this->end;
   if (this->pos >= this->numbits)
      return -1;
   synchro = this->bitstream.next_bit(this);
   if (synchro)
      group->from = decode_binary(&this->bitstream, 32);
   else
      group->from = this->lastfrom + decode_gamma(&this->bitstream) - 1;
   this->lastfrom = group->from;
   group->to = group->from + decode_gamma(&this->bitstream) - 1;
   group->type = decode_gamma(&this->bitstream) - 1;
   if (group->type == POLIQARP_SYNTAX_GROUP_COORD)
      group->u.coord.length = decode_gamma(&this->bitstream);
   else {
      int len = clog2(group->to - group->from + 1);
      int havesynh = this->bitstream.next_bit(this);
      int havesemh = this->bitstream.next_bit(this);
      group->u.noncoord.synh = havesynh 
         ? group->from + decode_binary(&this->bitstream, len)
         : POLIQARP_SYNTAX_GROUP_UNKNOWN;
      group->u.noncoord.semh = havesemh
         ? group->from + decode_binary(&this->bitstream, len)
         : POLIQARP_SYNTAX_GROUP_UNKNOWN;
   }
   this->end++;
   if (this->end == this->size)
      this->end = 0;
   if (this->end == this->start) {
      size_t oldsize = this->size;
      this->size *= 2;
      this->groups = realloc(this->groups, this->size * sizeof(struct poliqarp_syntax_group));
      memcpy(this->groups + oldsize, this->groups, this->start * sizeof(struct poliqarp_syntax_group));
      this->end += oldsize;
      group = this->groups + old_end;
   }
   if (group->type == POLIQARP_SYNTAX_GROUP_COORD && recursive) {
      size_t i;
      for (i = 0; i < group->u.coord.length; i++) 
         if (poliqarp_backend_syntax_next_internal(this, 0) == -1)
            return -1;
   }
   return 0;
}

int poliqarp_backend_syntax_next(struct poliqarp_backend_syntax *this)
{
   return poliqarp_backend_syntax_next_internal(this, 1);
}

void poliqarp_backend_syntax_reset(struct poliqarp_backend_syntax *this)
{
   this->start = this->end = this->pos = 0;
}
