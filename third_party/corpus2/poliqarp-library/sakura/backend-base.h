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

#ifndef POLIQARP_BACKEND_BASE_H
#define POLIQARP_BACKEND_BASE_H

#include <poliqarp-config.h>

#include <sakura/exception.h>
#include <sakura/common/newdict.h>

/** @todo */
#define POLIQARP_BASE1_IMAGE_FORMAT "%s.poliqarp.base1.image"
/** @todo */
#define POLIQARP_BASE1_OFFSET_FORMAT "%s.poliqarp.base1.offset"

/** @todo */
#define POLIQARP_BASE2_IMAGE_FORMAT "%s.poliqarp.base2.image"
/** @todo */
#define POLIQARP_BASE2_OFFSET_FORMAT "%s.poliqarp.base2.offset"


/** @todo */
struct poliqarp_backend_base {
   struct newdict dict_disamb; /**< Dictionary with disambiguated base 
                                    strings. */
   struct newdict dict_amb;    /**< Dictionary with ambiguous base strings. */
};

/** @todo */
int poliqarp_backend_base_open(struct poliqarp_backend_base *this, 
   const char *base_name, struct poliqarp_error *error);

/** @todo */
void poliqarp_backend_base_close(struct poliqarp_backend_base *this);

static inline size_t poliqarp_backend_base_num_items__disamb(
   const struct poliqarp_backend_base *this)
{
   return GET_NUM_ITEMS(&this->dict_disamb);
}

static inline size_t poliqarp_backend_base_num_items_amb(
   const struct poliqarp_backend_base *this)
{
   return GET_NUM_ITEMS(&this->dict_amb);
}

static inline const char *poliqarp_backend_base_fetch__disamb(
   const struct poliqarp_backend_base *this, size_t key)
{
   assert(key < this->dict_disamb.num_items);
   return (const char*) GET_ITEM(&this->dict_disamb, key);
}

static inline const char *poliqarp_backend_base_fetch__amb(
   const struct poliqarp_backend_base *this, size_t key)
{
   assert(key < this->dict_amb.num_items);
   return (const char*) GET_ITEM(&this->dict_amb, key);
}

static inline size_t poliqarp_backend_base_length__disamb(
   const struct poliqarp_backend_base *this, size_t key)
{
   assert(key < this->dict_disamb.num_items);
   return GET_LENGTH(&this->dict_disamb, key);
}

static inline size_t poliqarp_backend_base_length__amb(
   const struct poliqarp_backend_base *this, size_t key)
{
   assert(key < this->dict_amb.num_items);
   return GET_LENGTH(&this->dict_amb, key);
}

#endif /* POLIQARP_BACKEND_BASE_H */
