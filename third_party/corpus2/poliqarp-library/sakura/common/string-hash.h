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

#ifndef STRING_HASH_H
#define STRING_HASH_H

#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

typedef uint32_t string_hash_t;

#if defined(__GNUC__) && defined(__i386__)
/* x86 architecture supports unaligned 16-bit word access. */
static inline uint16_t hash_string_16(const char *string)
{
   return ((const uint16_t*) string)[0];
}
#else
/* More portable but slower. */
static inline uint16_t hash_string_16(const char *string)
{
   return (uint8_t) string[1] | ((uint8_t) string[0] << 8);
}
#endif

/** 
 * Paul Hsieh hash function.
 * @param string string to hash
 * @param length length of the string
 * See http://www.azillionmonkeys.com/qed/hash.html for details.
 */
static inline string_hash_t hash_string_n(const char *string, size_t length)
{
   assert(string != NULL);
   size_t reminding_length = length % 4;
   length /= 4;
   string_hash_t hash = length, temp;

   while (length > 0) {
      hash += hash_string_16(string);
      temp = (hash_string_16(string + 2) << 11) ^ hash;
      hash = (hash << 16) ^ temp;
      string += 4;
      hash += hash >> 11;
      length--;
   }
   switch (reminding_length) {
      case 3:
         hash += hash_string_16(string);
         hash ^= hash << 16;
         hash ^= (uint8_t) string[2] << 18;
         hash += hash >> 11;
         break;
      case 2:
         hash += hash_string_16(string);
         hash ^= hash << 11;
         hash += hash >> 17;
         break;
      case 1:
         hash += (uint8_t) string[0];
         hash ^= hash << 10;
         hash += hash >> 1;
         break;
      default:
         break;
   }
   hash ^= hash << 3;
   hash += hash >> 5;
   hash ^= hash << 4;
   hash += hash >> 17;
   hash ^= hash << 25;
   hash += hash >> 6;
   return hash;
}

/**
 * Paul Hsieh hash function.
 * @param string null-terminated string to hash
 * @see hash_string_n
 * See http://www.azillionmonkeys.com/qed/hash.html for details.
 */
static inline string_hash_t hash_string(const char *string)
{
   return hash_string_n(string, strlen(string));
}

#endif
