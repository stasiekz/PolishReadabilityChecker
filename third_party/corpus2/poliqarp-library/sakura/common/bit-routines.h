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
 * @file bit-routines.h
 * @brief Operations on bit sequences.
 *
 * The macros defined in this header allow to treat arrays of integer values
 * as packed arrays of bits.
 */

#ifndef BIT_ROUTINES_H
#define BIT_ROUTINES_H

#include <limits.h>

/**
 * Returns the size of the given bit array in bytes.
 * @param base Pointer to the beginning of the array.
 * @param size Number of bits in the array.
 */ 
#define BIT_ARRAY_LENGTH_BYTES(base, size) \
   (((size) + sizeof *(base) * CHAR_BIT - 1) / CHAR_BIT)

/**
 * Sets the value of a particular bit in the array to 1.
 * @param base Pointer to the beginning of the array.
 * @param index Zero-based index of the bit.
 */
#define BIT_ARRAY_SET(base, index) \
   ((base)[(index) / (sizeof *(base) * CHAR_BIT)] |= \
    1 << (index) % (sizeof *(base) * CHAR_BIT))

/**
 * Returns true iff a particular bit in the array is set to 1.
 * @param base Pointer to the beginning of the array.
 * @param index Zero-based index of the bit.
 */
#define BIT_ARRAY_GET(base, index) \
   ((base)[(index) / (sizeof *(base) * CHAR_BIT)] & \
    (1 << (index) % (sizeof *(base) * CHAR_BIT)) ? true : false)

#endif /* BIT_ROUTINES_H */
