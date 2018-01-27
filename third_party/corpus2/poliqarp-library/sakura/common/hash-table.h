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
 * @file hash-table.h
 * @brief Hash tables with textual keys.
 *
 * This file defines hash tables (with standard operations) that map strings
 * to arbitrary values. Hash conflicts are resolved by means of linked lists.
 */ 

#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <poliqarp-config.h>

#include <stdlib.h>

#include <sakura/common/memory-arena.h>
#include <sakura/common/string-hash.h>

/* Hashtable creation flags. */
#define HASHTABLE_DUPLICATE_KEYS  1 /**< Shall the keys be duplicated? */

/**
 * A single node in the table. 
 */
struct hash_table_node {
   char *key;                    /**< Key for this mapping. */
   void *value;                  /**< Value for this mapping. */
   struct hash_table_node *next; /**< Next node with the same hash value. */
   string_hash_t hash;           /**< Hash value of the key. */
};

/**
 * The hash table.
 */
struct hash_table {
   size_t size;      /**< Number of nodes. */
   size_t num_items; /**< Number of items currently stores in the table. */
   int flags;        /**< Table flags, bitwise OR of HASHTABLE_*. */
   struct hash_table_node **node_array; /**< Array of nodes. */
   struct marena *arena; /**< Memory source. */
   bool private_arena; /**< True iff arena is private for this hash table. */
};

/**
 * Constructs an empty hash table with the given size.
 * 
 * @param table the structure to be initialized.
 * @param size size of the table (number of distinct hash values possible).
 * @param flags creation flags of the table, with the following meanings:
 * <dl>
 *   <dt><tt>HASHTABLE_DUPLICATE_KEYS</tt></dt>
 *   <dd>If this is set, the hash_table_set() function will make a private 
 *       copy of the key parameter passed to it, so that the original can
 *       be freed immediately. Otherwise, a pointer to the original key
 *       is stored.
 *    </dd>
 * </dl>
 * @param arena the memory arena to be used as the source of memory. If
 *     this is NULL, a private arena will be allocated and freed upon 
 *     destruction of this hash table.
 * @return 0 upon successful creation, a non-zero value on error.
 */
int create_hash_table(struct hash_table *table, size_t size, int flags,
   struct marena *arena);

/**
 * Destroys the given hash table.
 * 
 * @param table the structure to be destroyed.
 * @param free_fn pointer to a function that frees a single value.
 * @return 0 upon successful completion, a non-zero value on error.
 */
int destroy_hash_table(struct hash_table *table, void (*free_fn)(void *));

/**
 * Creates a key-to-value mapping in the table. If a mapping for this key
 * already existed, it is replaced by the new one (the old mapping is lost).
 * 
 * @return 0 upon success, a non-zero value on error.
 */
int hash_table_set(struct hash_table *table, const char *key, void *value);

/**
 * Removes a mapping for the given key from the table.
 * 
 * @return 0 upon success, a non-zero value on error or if there were no such 
 * mapping.
 */
int hash_table_unset(struct hash_table *table, const char *key);

/**
 * Returns the value associated with the given key, or NULL if there is no
 * such value.
 */
void *hash_table_get(struct hash_table *table, const char *key);

/** 
 * Same as hash_table_get(), except that it operates on a constant hash table.
 *
 * @see hash_table_get
 */
const void *hash_table_const_get(const struct hash_table *table,
   const char *key);

/**
 * Invokes the given function (the iterator) on all mappings contained 
 * in the hash table.
 *
 * @param env The environment, an arbitrary value provided for the 
 *    key-value mappings to communicate with outside world.
 * @param iterator The iterator function. It must accept three arguments:
 *    key, value, and environment (env is passed as a third argument to
 *    the iterator).
 */
void hash_table_iterate(const struct hash_table *table, void *env,
   void (*iterator)(const char *, const void *, void *));

/**
 * Returns number of items in the given hash table.
 */
size_t hash_table_num_items(const struct hash_table *table);

#endif
