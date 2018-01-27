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

#ifndef POLIQARP_BACKEND_INTERP_H
#define POLIQARP_BACKEND_INTERP_H

#include <sakura/backend-config.h>
#include <sakura/abi.h>

#include <sakura/common/newdict.h>

/** @todo */
#define POLIQARP_INTERP1_IMAGE_FORMAT "%s.poliqarp.interp1.image"
/** @todo */
#define POLIQARP_INTERP1_OFFSET_FORMAT "%s.poliqarp.interp1.offset"
/** @todo */
#define POLIQARP_INTERP2_IMAGE_FORMAT "%s.poliqarp.interp2.image"
/** @todo */
#define POLIQARP_INTERP2_OFFSET_FORMAT "%s.poliqarp.interp2.offset"

/** @todo */
#define POLIQARP_OLD_INTERP1_IMAGE_FORMAT "%s.poliqarp.subpos1.image"
/** @todo */
#define POLIQARP_OLD_INTERP1_OFFSET_FORMAT "%s.poliqarp.subpos1.offset"
/** @todo */
#define POLIQARP_OLD_INTERP2_IMAGE_FORMAT "%s.poliqarp.subpos2.image"
/** @todo */
#define POLIQARP_OLD_INTERP2_OFFSET_FORMAT "%s.poliqarp.subpos2.offset"

/** @todo */
struct poliqarp_backend_interp {
   struct newdict dict__disamb;
   struct newdict dict__amb;
};

/** @todo */
int poliqarp_backend_interp_open(struct poliqarp_backend_interp *this, 
   const char *base_name, struct poliqarp_error *error);

/** @todo */
void poliqarp_backend_interp_close(struct poliqarp_backend_interp *this);

/** 
 * Get the number of disambiguated interpretation sets.
 */
static inline size_t poliqarp_backend_interp_num_items__disamb(
   const struct poliqarp_backend_interp *this)
{
   return GET_NUM_ITEMS(&this->dict__disamb);
}

/**
 * Get given disambiguated interpretation set.
 */
static inline const struct poliqarp_binary_interp *
poliqarp_backend_interp_fetch__disamb(
   const struct poliqarp_backend_interp *this, size_t key)
{
   assert(key < this->dict__disamb.num_items);
   return (struct poliqarp_binary_interp*) GET_ITEM(&this->dict__disamb, key);
}

/**
 * Get the length of given disambiguated interpretation.
 * Length is equal to number of interpretations in given set.
 */
static inline size_t poliqarp_backend_interp_length__disamb(
   const struct poliqarp_backend_interp *this, size_t key)
{
   assert(key < this->dict__disamb.num_items);
   return GET_LENGTH(&this->dict__disamb, key) / 
      sizeof(struct poliqarp_binary_interp);
}

/**
 * Get the number of ambiguous interpretation sets.
 */
static inline size_t poliqarp_backend_interp_num_items__amb(
   const struct poliqarp_backend_interp *this)
{
   return GET_NUM_ITEMS(&this->dict__amb);
}

/**
 * Get given ambiguous interpretation set.
 */
static inline const struct poliqarp_binary_interp *
poliqarp_backend_interp_fetch__amb(
   const struct poliqarp_backend_interp *this, size_t key)
{
   assert(key < this->dict__amb.num_items);
   return (struct poliqarp_binary_interp*) GET_ITEM(&this->dict__amb, key);
}

/**
 * Get the length of given ambiguous interpretation.
 * Length is equal to number of interpretations in given set.
 */
static inline size_t poliqarp_backend_interp_length__amb(
   const struct poliqarp_backend_interp *this, size_t key)
{
   assert(key < this->dict__amb.num_items);
   return GET_LENGTH(&this->dict__amb, key) / 
      sizeof(struct poliqarp_binary_interp);
}

#endif /* POLIQARP_BACKEND_INTERP_H */
