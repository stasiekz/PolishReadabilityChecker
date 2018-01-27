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
 * @file memory-arena.h
 * @brief Quick allocator for small objects.
 *
 * A common case when allocating objects is that we want to allocate many
 * objects, then free them all at once. It could be done with malloc() and
 * free(), but that would be costly, especially in case of a very large 
 * amount of tiny objects. To resolve this difficulty, Poliqarp provides a
 * memory allocator for such objects, called a memory arena.
 *
 * Memory arenas somewhat resemble 'obstacks' defined by GNU glibc: they
 * pre-allocate chunks of memory of fixed size and place objects on them,
 * so that allocation is fast. When one doesn't need the memory allocated
 * on the arena anymore, one can free it all at once, or release it 
 * (the memory then won't be freed, but made available for allocation
 * again).
 */

#ifndef MEMORY_ARENA_H
#define MEMORY_ARENA_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

/* Memory areas are tunable via these parameters. */

#define MARENA_ALIGNMENT      4    /**< Align requests to this boundary. */
#define MARENA_DEF_BLOCK_SIZE 4096 /**< Default block allocation size.   */
#define MARENA_TRACK_ALLOC    0    /**< Track number of allocations.     */
#define MARENA_DEBUG          0    /**< Display debugging information.   */ 

/** Memory arena node. */
struct marena_node {
   struct marena_node *next; /**< Next such node.                 */
   size_t used;              /**< Number of bytes used.           */
   size_t capacity;          /**< Capacity of this node.          */
   void *memory;             /**< Memory allocated for this node. */
};

/** Memory arena big node. */
struct marena_bignode {
   struct marena_bignode *next; /**< Next such node.                 */
   size_t size;                 /**< Capacity (all used).            */
   void *memory;                /**< Memory allocated for this node. */
};

/** Memory arena. */
struct marena {
   struct marena_node *first;        /**< List of nodes.         */
   struct marena_node *current;      /**< Current (last) node.   */
   struct marena_bignode *first_big; /**< List of big nodes.     */
#if MARENA_TRACK_ALLOC
   size_t n_alloc;                   /**< Number of allocations. */
#endif
   size_t block_size;                /**< Block size.            */
};

/** 
 * Creates a memory arena. 
 * @param this Pointer to uninitialized object. 
 */
void marena_create(struct marena *this);

/** 
 * Allocates memory from arena. 
 */
void *marena_alloc(struct marena *this, size_t size);

/** 
 * Releases all the memory allocated by this arena.
 * When the memory allocated from an arena is no longer used, this function
 * can be called. It doesn't free the memory, but releases it so that it 
 * can be reused.
 */
void marena_release(struct marena *this);

/** 
 * Creates a duplicate of a string getting necessary memory from arena. 
 */
char *marena_strdup(struct marena *this, const char *string);

/**  
 * Same as marena_strdup, except that it copies at most 'length' characters.
 * When the string is longer than 'length', only 'length' characters are
 * copied, and a null byte is added.
 */
char *marena_strndup(struct marena *this, const char *string, size_t length);

/** 
 * Destroys this arena and frees all allocated memory. 
 */
void marena_destroy(struct marena *this);

/* Options used to tune the arena at run-time. */

#define MARENA_OPT_BLOCKSIZE   1 /**< Block allocation size. */
#define MARENA_OPT_NALLOC      2 /**< Get number of allocations for 
                                      this arena. Available only when 
                                      support for this option is enabled. */

/** 
 * Sets a run-time option for an area.
 * @param opt Option number, one of MARENA_OPT_*.
 * @param value Value of the option.
 */
int marena_setopt(struct marena *this, int opt, size_t value);

/** 
 * Gets the value of a run-time option.
 * @param opt Option number, one of MARENA_OPT_*.
 */
size_t marena_getopt(struct marena *this, int opt);

#endif /* MEMORY_ARENA_H */
