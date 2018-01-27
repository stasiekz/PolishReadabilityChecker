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
 * @file bs.h
 * @brief Bitsets and bitset arenas.
 *
 * A bitset is a packed (memory-efficient) array of bits. Since Poliqarp
 * excessively allocates, drops and reuses large amounts of bitsets
 * of the same size, a specialized memory source called a 'bitset arena'
 * is provided. It allows for quick allocations of bitsets of the same size.
 */

#ifndef BS_H
#define BS_H

#include <poliqarp-config.h>

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <limits.h>

#include <sakura/common/memory-arena.h>

typedef uint_fast32_t bitset_unit;   /**< The type of memory unit for bitsets. */
typedef bitset_unit *bitset;    /**< A bitset is an array of memory units. */
typedef const bitset_unit *const_bitset; /**< A constant bitset. */

#define BS_BITS_PER_UNIT            (sizeof (bitset_unit) * CHAR_BIT)
#define BS_NONE                     -1

/** Bitset arena structure. */
typedef struct bitset_arena {
   size_t num_bits;      /**< Size of each block (in bits actually used).   */
   size_t num_bytes;     /**< Size of each block (in bytes allocated).      */
   size_t num_units;     /**< Size of each block (in units).                */
   struct marena *arena; /**< Memory source.                                */
   bool private_arena;   /**< True if arena is private and should be freed. */
} bitset_arena;

/**
 * Create a dummy bitset arena object. The only legal operation on such an area
 * is to destroy it.
 */
void bitset_arena_create_dummy(bitset_arena *this);

/**
 * Creates the bitset arena object.
 * From now on we can call bitset_arena_alloc() to obtain
 * a new bit-set. Each bit set is of the same size.
 * The memory for each subsequent bitset_arena_alloc() call is obtained
 * from the specified arena object.
 * 
 * @return 0 on success, -1 on error.
 * 
 * @param num_bits Size of each set in bits.
 * @param this Use this arena as a backing storage. If this is NULL, this
 *             bitset will allocate a private arena for its own use.
 */
int bitset_arena_create(bitset_arena *this, size_t num_bits, 
   struct marena *arena);

/**
 * Destructor.
 */
void bitset_arena_destroy(bitset_arena *this);

/**
 * Allocate new, empty bit-set of the size specified when creating this arena.
 *
 * @return The allocated bitset or NULL.
 */
bitset bitset_arena_alloc(bitset_arena *this);

/**
 * Allocate new, full bit-set of the size specified when creating this arena.
 *
 * @return The allocated bitset or NULL.
 */
bitset bitset_arena_alloc_ones(bitset_arena *this);

/**
 * Create an union of two bit-sets of the same size.
 * The union is performed by bitwise or-ing the memory area; results
 * are placed in the first bitset.
 *
 * @return The first bitset.
 */
bitset bitset_arena_union(bitset_arena *this, bitset bs1, const_bitset bs2);

/**
 * Create an intersection of two bit-sets of the same size.
 * The intersection is performed by bitwise and-ing the memory area; results
 * are placed in the first bitset.
 *
 * @return The first bitset.
 */
bitset bitset_arena_intersect(bitset_arena *this, bitset bs1, const_bitset bs2);

/**
 * Allocate a new bit-set and copy all contents from the specified bit-set.
 *
 * @return The allocated bitset or NULL.
 */
bitset bitset_arena_copy(bitset_arena *this, const_bitset bs);

/**
 * Copy contents of a bitset to another already allocated bitset.
 */
bitset bitset_arena_copy_to(bitset_arena *this, const_bitset from, bitset to);

/**
 * Print the indices of all existing objects.
 * The printout looks like this:
 * { 1 3 4 6 } (4 items)
 */
void bitset_arena_print(const bitset_arena *this, const_bitset bs, 
   FILE *stream);

/**
 * Calculate number of bits set to one in the given bitset.
 */
size_t bitset_count_ones(const bitset_arena *this, const_bitset bs);

/**
 * Get bit.
 *
 * @return 0 or 1.
 */
int bitset_arena_get(const bitset_arena *arena, const_bitset bs, size_t i);

/**
 * Set bit (set to 1).
 */
void bitset_arena_set(const bitset_arena *arena, bitset bs, size_t i);

/**
 * Clear bit (set to 0).
 */
void bitset_arena_clear(const bitset_arena *arena, bitset bs, size_t i);

/**
 * Set all bits to 0 or 1.
 */
void bitset_arena_fill(struct bitset_arena *arena, bitset bs, int value);

/**
 * Flip bit (set to opposite value).
 */
void bitset_arena_flip(const bitset_arena *arena, bitset bs, size_t i);

/**
 * Returns a nonzero value iff the two given bitsets are equal (that is,
 * contain exactly the same bits).
 */
int bitset_arena_compare(const bitset_arena *arena, const_bitset bs1, 
   const_bitset bs2);

/**
 * Iterate over the given index.
 * The iterator works as follows:
 *
 * @code
 * for (index=0; (index=bitset_arena_iterate(this,bs,index)) != (size_t)-1; ++index)
 *    printf ("object at index %d exists\n", index)
 * @endcode
 */
static inline size_t bitset_arena_iterate(const bitset_arena *this, 
   const_bitset bs, size_t index)
{
   while (index < this->num_bits && bitset_arena_get(this, bs, index) == 0)
      ++index;
   return index < this->num_bits ? index : (size_t)-1;
}

#endif /* BS_H */
