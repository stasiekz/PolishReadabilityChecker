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

#include <errno.h>

#include <sakura/common/newdict.h>

static void skip(void *foo) 
{
}

int newdict_open(struct newdict *this, const char *image_name, 
   const char *offset_name, bool want_index)
{
   int my_errno;

   init_file_map(&this->freq_map);
   this->freq = NULL;

   if (create_file_map(&this->offset_map, offset_name) != 0)
   {
      my_errno = errno;
      goto err;
   }

   this->num_items = get_file_size(&this->offset_map) / 
      sizeof(newdict_offset_t);
   this->item = malloc(sizeof *this->item * this->num_items);
   if (this->item == NULL)
   {
      my_errno = errno;
      goto err_at_malloc;
   }

   if (create_file_map(&this->dict_map, image_name) != 0)
   {
      my_errno = errno;
      goto err_at_image;
   }

   {
      const void **item = this->item;
      const char *image = get_file_image(&this->dict_map);
      const newdict_offset_t *offset = get_file_image(&this->offset_map);

      size_t num_items = this->num_items;
      size_t i;
      for (i = 0; i < num_items; ++i)
         item[i] = image + en4(offset[i]);
      if (want_index) {
         this->index = malloc(sizeof *(this->index));
         if (this->index == NULL)
         {
            my_errno = errno;
            goto err_at_image;
         }
         if (create_hash_table(this->index, num_items, 0, NULL))
         {
            my_errno = errno;
            goto err_at_index_create;
         }
         for (i = 0; i < num_items; ++i)
            if (hash_table_set(this->index, (const char *)item[i], 
               (void *)((intptr_t)(i + 1))))
            {
               my_errno = errno;
               goto err_at_index;
            }
      } else
         this->index = NULL;
   }
   return 0;

   /* cleanup code */

err_at_index:
   destroy_hash_table(this->index, skip);
err_at_index_create:
   free(this->index);
err_at_image:
   free(this->item);
err_at_malloc:
   destroy_file_map(&this->offset_map);
err:
   init_file_map(&this->dict_map);
   init_file_map(&this->offset_map);
   this->item = NULL;
   errno = my_errno;
   return -1;
}

int newdict_open2(struct newdict *this, const char *image_name, 
   const char *offset_name, const char *freq_name)
{
   int code;

   code = create_file_map(&this->freq_map, freq_name);
   if (code != 0)
      return code;

   code = newdict_open(this, image_name, offset_name, false);
   if (code != 0) {
      int my_errno = errno;
      destroy_file_map(&this->freq_map);
      errno = my_errno;
      return code;
   }
   this->freq = get_file_image(&this->freq_map);
   return 0;
}

void newdict_close(struct newdict *this)
{
   destroy_file_map(&this->dict_map);
   destroy_file_map(&this->offset_map);
   destroy_file_map(&this->freq_map);
   if (this->index) {
      destroy_hash_table(this->index, skip);
      free(this->index);
   }
   free(this->item);
}

int newindex_open(struct newindex *this, struct newdict *dict, 
   const char *index_name)
{
   int code;
   code = create_file_map(&this->index_map, index_name);
   if (code != 0)
      return code;

   this->num_items = get_file_size(&this->index_map) / sizeof(newdict_offset_t);
   if (this->num_items != dict->num_items) {
      destroy_file_map(&this->index_map);
      errno = EINVAL;
      return -1;
   }
   this->index = get_file_image(&this->index_map);
   return 0;
}

void newindex_close(struct newindex *this)
{
   destroy_file_map(&this->index_map);
}
