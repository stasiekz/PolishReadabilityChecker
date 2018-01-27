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

#ifndef POLIQARP_BACKEND_ORTH_H
#define POLIQARP_BACKEND_ORTH_H

#include <sakura/common/newdict.h>
#include <sakura/exception.h>

/** @todo */
#define POLIQARP_ORTH_IMAGE_FORMAT "%s.poliqarp.orth.image"
/** @todo */
#define POLIQARP_ORTH_OFFSET_FORMAT "%s.poliqarp.orth.offset"
/** @todo */
#define POLIQARP_ORTH_INDEX_FORMAT "%s.poliqarp.orth.index.%s"

/** @todo */
struct poliqarp_backend_orth {
   struct newdict dict;
   struct newindex afronte_index;
   struct newindex atergo_index;
};

/** @todo */
int poliqarp_backend_orth_open(struct poliqarp_backend_orth *this, 
   const char *base_name, struct poliqarp_error *error);

/** @todo */
void poliqarp_backend_orth_close(struct poliqarp_backend_orth *this);

static inline size_t poliqarp_backend_orth_num_items(
   const struct poliqarp_backend_orth *this)
{
   return GET_NUM_ITEMS(&this->dict);
}

static inline const char *poliqarp_backend_orth_fetch(
   const struct poliqarp_backend_orth *this, size_t key)
{
   assert(key < this->dict.num_items);
   return (const char*) GET_ITEM(&this->dict, key);
}

static inline size_t poliqarp_backend_orth_length(
   const struct poliqarp_backend_orth *this, size_t key)
{
   assert(key < this->dict.num_items);
   return GET_LENGTH(&this->dict, key);
}

static inline size_t poliqarp_backend_orth_afronte_fetch(
   const struct poliqarp_backend_orth *this, size_t key)
{
   assert(key < this->dict.num_items);
   return GET_INDEX_ITEM(&this->afronte_index, key);
}

static inline size_t poliqarp_backend_orth_atergo_fetch(
   const struct poliqarp_backend_orth *this, size_t key)
{
   assert(key < this->dict.num_items);
   return GET_INDEX_ITEM(&this->atergo_index, key);
}

#endif /* POLIQARP_BACKEND_ORTH_H */
