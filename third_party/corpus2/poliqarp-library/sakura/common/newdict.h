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
 * @file newdict.h
 * @brief On-disk dictionaries and dictionary indices.
 *
 * Dictionaries are sequences of data of variable size (such as strings)
 * stored on disk in two files: one (the image file) contains actual data
 * as one block, the other (the offset file) contains offsets of those
 * data stored as unsigned 32-bit little-endian integers.
 *
 * Dictionary indices are integer-to-integer on-disk maps which contain
 * indices to dictionaries and allow to iterate over elements of a dictionary
 * in a certain order.
 */

#ifndef HAVE_NEWDICT_H
#define HAVE_NEWDICT_H

#include <poliqarp-config.h>

#include <stdlib.h>

#include <sakura/common/file-map.h>
#include <sakura/common/hash-table.h>

/**
 * Offset used inside dictionary file.
 */
typedef uint32_t newdict_offset_t;

/**
 * Dictionary structure. Some fields are optional (can be NULL).
 */
struct newdict {
   const void **item;          /**< Array containing the data. */
   const uint32_t *freq;       /**< Array containing (optionally) frequencies
                                    of items. */
   size_t num_items;           /**< Number of items in this dictionary. */
   struct hash_table *index;   /**< Optional inverted index mapping strings
                                    to their positions. */
   struct file_map dict_map;   /**< Image file map object. */
   struct file_map offset_map; /**< Offset file map object. */
   struct file_map freq_map;   /**< Frequency file map object. */
};

/**
 * Open dictionary.
 * @param this Uninitialized dictionary object.
 * @param image_name File name of the dictionary image.
 * @param offset_name File name of the dictionary offset table.
 * @param want_index Whether to create an inverted index for this dictionary.
 */
int newdict_open(struct newdict *this, const char *image_name, 
   const char *offset_name, bool want_index);

/**
 * Open dictionary with frequency table.
 * @param this Uninitialized dictionary object.
 * @param image_name File name of the dictionary image.
 * @param offset_name File name of the dictionary offset table.
 * @param freq_name File name of the dictionary frequency table.
 */
int newdict_open2(struct newdict *this, const char *image_name,
   const char *offset_name, const char *freq_name);

/**
 * Close dictionary.
 */
void newdict_close(struct newdict *this);

/**
 * Dictionary index.
 * Allows to iterate over elements in specified order
 */
struct newindex {
   const uint32_t *index;     /**< Array of the indices. */
   struct file_map index_map; /**< Index file map object. */
   size_t num_items;          /**< Number of items. */
};

/**
 * Open dictionary index.
 */
int newindex_open(struct newindex *this, struct newdict *dict,
   const char *index_name);

/**
 * Close dictionary index.
 */
void newindex_close(struct newindex *this);

/**
 * Get item (pointer to data) from dictionary.
 */
#define GET_ITEM(dict, key)            ((dict)->item[key])

/**
 * Get frequency of item from dictionary.
 */
#define GET_FREQ(dict, key)            ((dict)->freq[key])

/**
 * Get content length of item.
 */
#define GET_LENGTH(dict, key)                                 \
   (en4(*(newdict_offset_t *)(((char *)GET_ITEM(dict,key)) -  \
      sizeof (newdict_offset_t))))

#define GET_INDEX_ITEM(INDEX,KEY)      ((INDEX)->index[KEY])

/**
 * Get number of items in dictionary/index.
 */
#define GET_NUM_ITEMS(DICT_OR_INDEX)   ((DICT_OR_INDEX)->num_items)

#endif                        /* NEWDICT_H */
