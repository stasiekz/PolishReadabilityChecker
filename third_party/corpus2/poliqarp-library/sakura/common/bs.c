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

#include <sakura/common/bs.h>

void bitset_arena_create_dummy(bitset_arena *this)
{
   assert(this != NULL);
   this->arena = NULL;
   this->private_arena = NULL;
}

int bitset_arena_create(bitset_arena *this, size_t num_bits, 
   struct marena *arena)
{
   if (arena) {
      this->arena = arena;
      this->private_arena = false;
   } else {
      this->arena = malloc(sizeof *this->arena);
      if (this->arena == NULL)
         return -1;
      this->private_arena = true;
      marena_create(this->arena);
   }
   this->num_bits = num_bits;
   this->num_units = (num_bits + BS_BITS_PER_UNIT - 1) / BS_BITS_PER_UNIT;
   this->num_bytes = this->num_units * sizeof(bitset_unit);
   return 0;
}

void bitset_arena_destroy(bitset_arena *this)
{
   assert(this != NULL);
   /* detect if we've been constructed in the first place */
   if (this->arena && this->private_arena) {
      marena_destroy(this->arena);
      free(this->arena);
   }
}

bitset bitset_arena_alloc(bitset_arena *this)
{
   bitset bs;
   assert(this != NULL);
   bs = marena_alloc(this->arena, this->num_bytes);
   if (bs == NULL)
      return NULL;
   memset(bs, 0, this->num_bytes);
   return bs;
}

bitset bitset_arena_alloc_ones(bitset_arena *this)
{
   bitset bs;
   assert(this != NULL);
   bs = marena_alloc(this->arena, this->num_bytes);
   if (bs == NULL)
      return NULL;
   memset(bs, 255, this->num_bytes);
   return bs;
}

/* results are placed in bs1 */
bitset bitset_arena_union(struct bitset_arena *this, bitset bs1, 
   const_bitset bs2)
{
   bitset_unit *dest;
   const bitset_unit *src;
   const bitset_unit *end;

   assert(this != NULL);
   assert(bs1 != NULL);
   assert(bs2 != NULL);
   dest = bs1;
   src = bs2;
   end = bs2 + this->num_units;
   while (src < end) {
      *dest |= *src;
      ++dest;
      ++src;
   }
   return bs1;
}

/* results are placed in bs1 */
bitset bitset_arena_intersect(struct bitset_arena *this, bitset bs1,
   const_bitset bs2)
{
   bitset_unit *dest;
   const bitset_unit *src;
   const bitset_unit *end;

   assert(this != NULL);
   assert(bs1 != NULL);
   assert(bs2 != NULL);
   dest = bs1;
   src = bs2;
   end = bs2 + this->num_units;
   while (src < end) {
      *dest &= *src;
      ++dest;
      ++src;
   }
   return bs1;
}

bitset bitset_arena_copy(bitset_arena *this, const_bitset bs)
{
   bitset result;
   assert(this != NULL);
   assert(bs != NULL);
   result = bitset_arena_alloc(this);
   if (result == NULL)
      return NULL;
   memcpy(result, bs, this->num_bytes);
   return result;
}

bitset bitset_arena_copy_to(bitset_arena *this, const_bitset from, bitset to)
{
   assert(this != NULL);
   assert(from != NULL);
   assert(to != NULL);
   memcpy(to, from, this->num_bytes);
   return to;
}

void bitset_arena_print(const bitset_arena *this, const_bitset bs, 
   FILE *stream)
{
   assert(this != NULL);
   if (bs != NULL) {
      size_t i;
      int items = 0;
      fprintf(stream, "{ ");
      for (i = 0; i < this->num_bits; ++i)
         if (bitset_arena_get(this, bs, i)) {
            fprintf(stream, "%" PRIuSIZE " ", i);
            ++items;
         }
      fprintf(stream, __("} (%d item)\n", "} (%d items)\n", items), items);
   } else
      fputs("(null)\n", stream);
}

size_t bitset_count_ones(const bitset_arena *this, const_bitset bs)
{
   size_t i, items = 0;
   assert(this != NULL);
   for (i = 0; i < this->num_bits; ++i)
      if (bitset_arena_get(this, bs, i))
         items++;
   return items;
}

int bitset_arena_get(const bitset_arena *this, const_bitset bs, size_t i)
{
   assert(this != NULL);
   assert(bs != NULL);
   assert(i < this->num_bits);
   return bs[i / BS_BITS_PER_UNIT] & ((bitset_unit)1 << (i % BS_BITS_PER_UNIT)) ? 1 : 0;
}

void bitset_arena_set(const bitset_arena *this, bitset bs, size_t i)
{
   assert(this != NULL);
   assert(bs != NULL);
   assert(i < this->num_bits);
   bs[i / BS_BITS_PER_UNIT] |= ((bitset_unit)1 << (i % BS_BITS_PER_UNIT));
}

void bitset_arena_clear(const bitset_arena *this, bitset bs, size_t i)
{
   assert(this != NULL);
   assert(bs != NULL);
   assert(i < this->num_bits);
   bs[i / BS_BITS_PER_UNIT] &= ~((bitset_unit)1 << (i % BS_BITS_PER_UNIT));
}

void bitset_arena_fill(struct bitset_arena *this, bitset bs, int value)
{
   assert(this != NULL);
   assert(bs != NULL);
   memset(bs, value ? 255 : 0, this->num_bytes);
}

void bitset_arena_flip(const bitset_arena *this, bitset bs, size_t i)
{
   assert(this != NULL);
   assert(bs != NULL);
   assert(i < this->num_bits);
   bs[i / BS_BITS_PER_UNIT] ^= ~((bitset_unit)1 << (i % BS_BITS_PER_UNIT));
}

int bitset_arena_compare(const bitset_arena *this, const_bitset bs1, 
   const_bitset bs2)
{
   assert(this != NULL);
   assert(bs1 != NULL);
   assert(bs2 != NULL);
   return memcmp(bs1, bs2, this->num_bytes);
}
