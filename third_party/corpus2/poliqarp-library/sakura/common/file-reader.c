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
#include <stdlib.h>
#include <string.h>
#include <sakura/common/file-reader.h>

#define SHIFT(ptr, x, y) ((void *)(((char *)(ptr)) + (x) * (y)))

typedef void (*cleanup_t)(void *);

#ifndef USE_EXTENSIVE_MMAP
static void file_reader_rewind(struct file_reader *reader, off64_t pos)
{
#ifndef POLIQARP_SINGLE_THREADED
   pthread_cleanup_push((cleanup_t)pthread_mutex_unlock, 
      (void *)&reader->ns_mutex);
   pthread_mutex_lock(&reader->ns_mutex);
#endif
   assert(pos >= 0);
   fseeko(reader->file, pos * reader->item_size, SEEK_SET);
   reader->seq_start = pos;
   reader->seq_offset = 0;
   fread(reader->buffer_seq, reader->item_size, reader->buffer_size, 
         reader->file);
#ifndef POLIQARP_SINGLE_THREADED
   pthread_cleanup_pop(1);
#endif
}

static inline off64_t *last_cache_offset(struct file_reader *reader, size_t i)
{
   return (off64_t *)SHIFT(reader->last_cache, reader->item_size + sizeof(off64_t), i);
}

static inline void *last_cache_data(struct file_reader *reader, size_t i)
{
   return (void *)((char *)(last_cache_offset(reader, i)) + sizeof(off64_t));
}
#endif

int file_reader_create(struct file_reader *reader, const char *filename,
   size_t item_size)
{
#ifdef USE_EXTENSIVE_MMAP
   struct file_map map;
   if (create_file_map(&map, filename) != 0)
      return -1;
   reader->map = map;
#else
   size_t i;
   FILE *file = fopen(filename, "rb");
   if (file == NULL)
      return -1;
   reader->file = file;
#endif
   reader->item_size = item_size;
#ifdef USE_EXTENSIVE_MMAP
   reader->num_items = get_file_size(&map) / item_size;
   reader->seq_start = reader->seq_offset = 0;
   return 0;
#else
#ifndef POLIQARP_SINGLE_THREADED
   pthread_mutex_init(&reader->ns_mutex, NULL);
#endif
   reader->buffer_size = 1024;
   reader->buffer_seq = (void *)malloc(item_size * reader->buffer_size);
   if (reader->buffer_seq == NULL)
      goto err_at_seq;
   reader->buffer_rnd = (void *)malloc(item_size * reader->buffer_size);
   if (reader->buffer_rnd == NULL)
      goto err_at_rnd;
   reader->last_cache = (void *)malloc((item_size + sizeof(off64_t)) *
      reader->buffer_size);
   if (reader->last_cache == NULL)
      goto err_at_lc;
   for (i = 0; i < reader->buffer_size; i++) 
      *last_cache_offset(reader, i) = -1;
   fseeko(file, 0, SEEK_END);
   reader->num_items = ftello(file) / item_size;
   file_reader_rewind(reader, 0);
   reader->rnd_start = -1;
   reader->need_seek = false;
   return 0;
err_at_lc:
   free(reader->buffer_rnd);
err_at_rnd:
   free(reader->buffer_seq);
err_at_seq:
   return -1;
#endif
}

int file_reader_destroy(struct file_reader *reader)
{
#ifdef USE_EXTENSIVE_MMAP
   destroy_file_map(&reader->map); 
#else
   free(reader->buffer_seq);
   free(reader->buffer_rnd);
   free(reader->last_cache);
   fclose(reader->file);
#ifndef POLIQARP_SINGLE_THREADED
   pthread_mutex_destroy(&reader->ns_mutex);
#endif
#endif
   return 0;
}

void file_reader_seek(struct file_reader *reader, off64_t pos)
{
   assert(pos >= 0);
#ifdef USE_EXTENSIVE_MMAP
   reader->seq_start = pos;
#else
   if (pos >= reader->seq_start && 
       pos < reader->seq_start + reader->buffer_size)
   {
      reader->seq_offset = pos - reader->seq_start;
   } else {
      file_reader_rewind(reader, pos);
   }
#endif
}

const void *file_reader_next(struct file_reader *reader)
{
#ifdef USE_EXTENSIVE_MMAP
   return get_file_image(&reader->map) + reader->seq_start * reader->item_size; 
#else
   void *result;
   if (reader->seq_start + reader->seq_offset >= reader->num_items)
      return NULL;
   if (reader->seq_offset == reader->buffer_size) {
      reader->seq_start += reader->seq_offset;
      reader->seq_offset = 0;
#ifndef POLIQARP_SINGLE_THREADED
      pthread_cleanup_push((cleanup_t)pthread_mutex_unlock, 
         (void *)&reader->ns_mutex);
      pthread_mutex_lock(&reader->ns_mutex);
#endif
      if (reader->need_seek) {
         reader->need_seek = false;
         assert(reader->seq_start >= 0);
         fseeko(reader->file, reader->seq_start * reader->item_size, SEEK_SET);
      }
      fread(reader->buffer_seq, reader->item_size, reader->buffer_size,
         reader->file);
#ifndef POLIQARP_SINGLE_THREADED
      pthread_cleanup_pop(1);
#endif
   }
   result = SHIFT(reader->buffer_seq, reader->item_size, reader->seq_offset++);
   return result;
#endif
}

const void *file_reader_get(struct file_reader *reader, off64_t i)
{
#ifdef USE_EXTENSIVE_MMAP
   return get_file_image(&reader->map) + i * reader->item_size; 
#else
   off64_t delta = i - reader->rnd_start;
   size_t cacheofs = i % reader->buffer_size;
   void *retval;

   assert(i >= 0);

   if (*last_cache_offset(reader, cacheofs) == i)
      return last_cache_data(reader, cacheofs);
   if (reader->rnd_start == -1 || delta < 0 || delta >= reader->buffer_size)
   {
#ifndef POLIQARP_SINGLE_THREADED
      pthread_cleanup_push((cleanup_t)pthread_mutex_unlock, 
         (void *)&reader->ns_mutex);
      pthread_mutex_lock(&reader->ns_mutex);
#endif
      assert(i >= 0);
      fseeko(reader->file, i * reader->item_size, SEEK_SET);
      reader->rnd_start = i;
      delta = 0;
      fread(reader->buffer_rnd, reader->item_size, reader->buffer_size,
         reader->file);
      reader->need_seek = true;
#ifndef POLIQARP_SINGLE_THREADED
      pthread_cleanup_pop(1);
#endif
   }
   retval = SHIFT(reader->buffer_rnd, reader->item_size, delta);
   *last_cache_offset(reader, cacheofs) = i;
   memcpy(last_cache_data(reader, cacheofs), retval, reader->item_size);
   return retval;
#endif
}
