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

#include <stdlib.h>
#include <string.h>

#include <sakura/common/bs-file-backend.h>

int file_obs_create(struct file_obs *bs, size_t block_size, FILE *f)
{
   bs->block_size = block_size;
   bs->bits_used = 0;
   bs->blocks_written = 0;
   bs->data = malloc(block_size);
   if (bs->data == NULL)
      return -1;
   bs->file = f;
   return 0;
}

void file_obs_set_backend(struct obs *obs, struct file_obs *backend)
{
   obs->data = backend;
   obs->add_bit = file_obs_add_bit;
}

void file_obs_destroy(struct file_obs *bs)
{
   free(bs->data);
}

int file_obs_flush(void *d)
{
   struct file_obs *bs = (struct file_obs *)d;
   if (bs->bits_used == 0)
      return 0;
   if (fwrite(bs->data, bs->block_size, 1, bs->file) != 1)
      return -1;
   bs->blocks_written++;
   bs->bits_used = 0;
   return 0;
}

int file_obs_flush_partial(void *d)
{
   struct file_obs *bs = (struct file_obs *)d;
   if (bs->bits_used == 0)
      return 0;
   if (fwrite(bs->data, (bs->bits_used + 7) / 8, 1, bs->file) != 1)
      return -1;
   bs->blocks_written++;
   bs->bits_used = 0;
   return 0;
}

int file_obs_halve_block(struct file_obs *bs)
{
   if (bs->block_size == 1)
      return 0;
   bs->block_size /= 2;
   bs->blocks_written *= 2;
   if (bs->bits_used >= 8 * bs->block_size) {
      if (fwrite(bs->data, bs->block_size, 1, bs->file) != 1)
         return -1;
      bs->bits_used -= 8 * bs->block_size;
      bs->blocks_written++;
      memcpy(bs->data, bs->data + bs->block_size, bs->block_size);
   }
   return 0;
}

void file_obs_add_bit_noflush(void *d, int x)
{
   struct file_obs *bs = (struct file_obs *)d;
   size_t bit = bs->bits_used++;
   if (x)
      bs->data[bit / 8] |= (1 << (bit % 8));
   else
      bs->data[bit / 8] &= ~(1 << (bit % 8));
}

int file_obs_add_bit(void *d, int x)
{
   struct file_obs *bs = (struct file_obs *)d;
   file_obs_add_bit_noflush(d, x);
   if (bs->bits_used == 8 * bs->block_size)
      return file_obs_flush(d);
   return 0;
}

