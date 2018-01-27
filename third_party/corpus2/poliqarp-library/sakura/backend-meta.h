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

#ifndef POLIQARP_BACKEND_META_H
#define POLIQARP_BACKEND_META_H

#include <sakura/common/newdict.h>
#include <sakura/common/tinydb.h>
#include <sakura/exception.h>
#include <sakura/abi.h>

#define POLIQARP_META_KEY_IMAGE_FORMAT "%s.poliqarp.meta-key.image"
#define POLIQARP_META_KEY_OFFSET_FORMAT "%s.poliqarp.meta-key.offset"
#define POLIQARP_META_VALUE_IMAGE_FORMAT "%s.poliqarp.meta-value.image"
#define POLIQARP_META_VALUE_OFFSET_FORMAT "%s.poliqarp.meta-value.offset"
#define POLIQARP_META_IMAGE_FORMAT "%s.poliqarp.meta.image"
#define POLIQARP_META_CONFIG_FORMAT "%s.meta.cfg"

enum poliqarp_meta_type {
   POLIQARP_META_TYPE_STRING,
   POLIQARP_META_TYPE_DATE,
   POLIQARP_META_TYPE_UNKNOWN
};

struct poliqarp_meta_type_list {
   char *key;
   enum poliqarp_meta_type type;
   struct poliqarp_meta_type_list *next;
};

/** @todo */
struct poliqarp_backend_meta {
   struct newdict key;
   struct newdict value;
   struct tinydb meta;
   struct poliqarp_meta_type_list *types;   
};

/** @todo */
int poliqarp_backend_meta_open(struct poliqarp_backend_meta *this, 
   const char *base_name, struct poliqarp_error *error);

/** @todo */
void poliqarp_backend_meta_close(struct poliqarp_backend_meta *this);

static inline size_t poliqarp_backend_meta_num_keys(
   const struct poliqarp_backend_meta *this)
{
   return GET_NUM_ITEMS(&this->key);
}

static inline size_t poliqarp_backend_meta_num_values(
   const struct poliqarp_backend_meta *this)
{
   return GET_NUM_ITEMS(&this->value);
}

static inline size_t poliqarp_backend_meta_num_items(
   const struct poliqarp_backend_meta *this)
{
   return this->meta.num_items;
}

static inline const char *poliqarp_backend_meta_key_fetch(
   const struct poliqarp_backend_meta *this, size_t index)
{
   assert(index < this->key.num_items);
   return (const char*) GET_ITEM(&this->key, index);
}

static inline const char *poliqarp_backend_meta_value_fetch(
   const struct poliqarp_backend_meta *this, size_t index)
{
   assert(index < this->value.num_items);
   return (const char*) GET_ITEM(&this->value, index);
}

static inline struct poliqarp_binary_metadata poliqarp_backend_meta_fetch(
   const struct poliqarp_backend_meta *this, size_t index)
{
   struct poliqarp_binary_metadata res;
   assert(index < this->meta.num_items);
   res = 
      *(struct poliqarp_binary_metadata *)
      (tinydb_fetch_item(&this->meta, index));
   res.type = en4(res.type);
   res.key = en4(res.key);
   switch (res.type) {
      case POLIQARP_METADATA_SINGLE:
      case POLIQARP_METADATA_MULTI:
         res.value_as.text = en4(res.value_as.text);
         break;
      case POLIQARP_METADATA_DATE:
         res.value_as.date.year = en2(res.value_as.date.year);
         break;
      case POLIQARP_METADATA_UNDEFINED:
         break;
      default:
         abort(); /* Should not happen for a valid corpus. */
   }
   return res;
}

/** 
 * Returns the type of given key of metadata.
 */
enum poliqarp_meta_type poliqarp_get_metadata_type(
   const struct poliqarp_backend_meta *this, const char *key);

#endif
