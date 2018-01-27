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

/**
 * @file file-reader.h
 * @brief Lightweight mmap() workalike.
 *
 * The memory view of files provided by mmap() and lookalikes (see file-map.h)
 * is appropriate when one needs random access to files. However, there are
 * files that Poliqarp processes mostly sequentially, occassionally skipping
 * some parts of the file and only rarely needing a random access. Moreover,
 * the two kinds of access are performed simultaneously by different threads.
 * Hence the need for a structure that provides easy access to file elements
 * while at the same time allowing random access. Such a structure, called
 * a file reader, and operations on it are what this header defines.
 */

#ifndef FILE_READER_H
#define FILE_READER_H

#include <poliqarp-config.h>

#include <stdio.h>
#include <sys/types.h>
#ifndef POLIQARP_SINGLE_THREADED
#include <pthread.h>
#endif

#ifdef USE_EXTENSIVE_MMAP
#include <sakura/common/file-map.h>
#endif

/**
 * A lightweight memory mapping object.
 */
struct file_reader {
#ifdef USE_EXTENSIVE_MMAP
   struct file_map map;      /**< The underlying file map. */
#else
   FILE *file;               /**< The underlying file object. */
   size_t buffer_size;       /**< Size of the caches, as number of items. */
   void *buffer_seq;         /**< Cache for sequential access. */
   void *buffer_rnd;         /**< Cache for random access. */
   void *last_cache;         /**< Cache of recently fetched elements. */
   off64_t rnd_start;        /**< Likewise, for random cache. */
   bool need_seek;           /**< Do we need to do a fseeko()? */
#ifndef POLIQARP_SINGLE_THREADED
   pthread_mutex_t ns_mutex; /**< Thread synchronizer. */
#endif
#endif
   size_t item_size;         /**< Size of a single item, in bytes. */
   off64_t num_items;        /**< Number of items in this file. */
   off64_t seq_start;        /**< Offset of the element corresponding to
                                  current start of sequential cache. */
   off64_t seq_offset;       /**< Offset of the 'current' position in the
                                  sequential cache. */
};

/** 
 * Creates a file reader for the specified file. 
 * @param filename Path of the file to be mapped.
 * @param item_size Size of a single item in the file, in bytes.
 * @return 0 upon successful creation, a nonzero value on failure.
 * @note The specified file must exist, be readable; moreover, its size
 * should be divisible by item_size.
 */
int file_reader_create(struct file_reader *reader, const char *filename, 
   size_t item_size);

/**
 * Frees resources assigned to a file reader, closing the file.
 */
int file_reader_destroy(struct file_reader *reader);

/**
 * Returns next element in the file.
 * @note This routine pertains to sequential access.
 */
const void *file_reader_next(struct file_reader *reader);

/**
 * Causes the specified element to be returned by next call of 
 * file_reader_next(). Sequential reading of elements will continue from that
 * element. 
 * @param pos Offset of the element in the file.
 * @note This routine pertains to sequential access.
 */
void file_reader_seek(struct file_reader *reader, off64_t pos);

/**
 * Returns the specified element in the file. Doesn't influence the
 * 'current' position of sequential access.
 * @param i Offset of the element in the file.
 * @note This routine pertains to random access.
 */
const void *file_reader_get(struct file_reader *reader, off64_t i);

/**
 * Returns the current position of sequential access.  Analogous
 * to ftello().
 */
static inline size_t file_reader_current(const struct file_reader *reader)
{
   return reader->seq_start + reader->seq_offset;
}

#endif /* FILE_READER_H */
