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

#include <sakura/backend-index.h>
#include <sakura/exception.h>
#include <foostring/foostring.h>

int poliqarp_index_calculate(const char *desc)
{
   int res = 0;
   while (*desc) {
      switch (*desc) {
         case 'o':
            res |= POLIQARP_INDEX_ORTH;
            break;
         case 'd':
            res |= POLIQARP_INDEX_DISAMB;
            break;
         case 'a':
            res |= POLIQARP_INDEX_AMB;
            break;
         default:
            return -1;
      }
      desc++;
   }
   return res;
}

char *poliqarp_index_stringify(int indices)
{
   static char res[4];
   char *tmp = res;

   if (indices & POLIQARP_INDEX_ORTH) 
      *tmp++ = 'o';
   if (indices & POLIQARP_INDEX_DISAMB) 
      *tmp++ = 'd';
   if (indices & POLIQARP_INDEX_AMB)
      *tmp++ = 'a';
   *tmp++ = 0;
   return res;
}

static int ibs_mem_backend_next_bit(void *bs)
{
   struct poliqarp_ibs_mem_backend *ibs =
      (struct poliqarp_ibs_mem_backend *)bs;
   int x = ibs->data[ibs->num_bit / 8] & (1 << (ibs->num_bit % 8));
   ibs->num_bit++;
   return x;
}

static struct poliqarp_rindex *open_rindex(const char *base_name,
   size_t granularity, const char *format, struct poliqarp_error *error)
{
   char *path, *format2;
   struct poliqarp_rindex *index;
   bool have_image_map = false, have_offset_map = false;
   int rc;

   index = malloc(sizeof *index);
   if (index == NULL)
      goto error_message;

   index->granularity = granularity;

   init_file_map(&index->image);
   init_file_map(&index->offset);

   path = string_aformat(format, base_name);
   if (path == NULL)
      goto error_message;

   rc = create_file_map(&index->image, path);
   if (rc != 0) {
      poliqarp_error_from_system(error, _("Unable to open rindex image (%s)"),
         path);
   }
   free(path);
   if (rc != 0)
      goto error;
   have_image_map = true;

   format2 = string_aformat("%s.offset", format);
   if (format2 == NULL)
      goto error_message;
   path = string_aformat(format2, base_name);
   free(format2);
   if (path == NULL)
      goto error_message;

   rc = create_file_map(&index->offset, path);
   if (rc != 0)
      poliqarp_error_from_system(error, _("Unable to open rindex offset (%s)"),
         path);
   free(path);
   if (rc != 0)
      goto error;
   have_offset_map = true;

   index->backend.data = (uint8_t *)get_file_image(&index->image);
   index->ibs.data = &index->backend;
   index->ibs.next_bit = ibs_mem_backend_next_bit;
   index->num_lists = get_file_size(&index->offset) / sizeof(uint32_t);
   return index;
error_message:
   poliqarp_error_from_system(error, _("Unable to initialize rindex backend"));
error:
   if (have_image_map)
      destroy_file_map(&index->image);
   if (have_offset_map)
      destroy_file_map(&index->offset);
   free(index);
   return NULL;
}

void poliqarp_rindex_set(struct poliqarp_rindex *this, size_t which)
{
   uint32_t *offsets = (uint32_t *)get_file_image(&this->offset);
   this->backend.num_bit = en4(offsets[which]);
}

static void close_rindex(struct poliqarp_rindex *index)
{
   destroy_file_map(&index->offset);
   destroy_file_map(&index->image);
   free(index);
}

int poliqarp_backend_index_open(struct poliqarp_backend_index *this,
   const struct poliqarp_backend_config *config, const char *base_name,
   struct poliqarp_error *error)
{
   this->orth_index = this->disamb_index = this->amb_index = NULL;
   if (config->cdf.indices & POLIQARP_INDEX_ORTH) {
      this->orth_index = open_rindex(base_name, config->cdf.granularity,
         POLIQARP_CORPUS_ORTH_INDEX_FORMAT, error);
      if (this->orth_index == NULL)
         goto error;
   }
   if (config->cdf.indices & POLIQARP_INDEX_DISAMB) {
      this->disamb_index = open_rindex(base_name, config->cdf.granularity,
         POLIQARP_CORPUS_DISAMB_INDEX_FORMAT, error);
      if (this->disamb_index == NULL)
         goto error;
   }
   if (config->cdf.indices & POLIQARP_INDEX_AMB) {
      this->amb_index = open_rindex(base_name, config->cdf.granularity,
         POLIQARP_CORPUS_AMB_INDEX_FORMAT, error);
      if (this->amb_index == NULL)
         goto error;
   }
   return 0;
error:
   poliqarp_backend_index_close(this);
   return -1;
}

void poliqarp_backend_index_close(struct poliqarp_backend_index *this)
{
   if (this->orth_index) {
      close_rindex(this->orth_index);
      this->orth_index = NULL;
   }
   if (this->disamb_index) {
      close_rindex(this->disamb_index);
      this->disamb_index = NULL;
   }
   if (this->amb_index) {
      close_rindex(this->amb_index);
      this->amb_index = NULL;
   }
}
