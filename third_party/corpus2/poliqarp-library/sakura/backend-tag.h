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

#ifndef POLIQARP_BACKEND_TAG_H
#define POLIQARP_BACKEND_TAG_H

#include <sakura/backend-config.h>

#include <sakura/common/newdict.h>

/* the %s is for encoding selected */
#define POLIQARP_TAG_IMAGE_FORMAT "%s.poliqarp.tag.image"
#define POLIQARP_TAG_OFFSET_FORMAT "%s.poliqarp.tag.offset"

/**
 * Tag backend structure.
 * Used to retrieve tags from storage
 */
struct poliqarp_backend_tag {
   struct newdict dict;                /**< Dictionary object that contains 
                                            all tags.                       */
   void *parsed_tag_memory;            /**< Memory used by all parsed tags. */
   struct poliqarp_parsed_tag *parsed_tag; /**< Parsed tags.                    */
};

/** Open the tag backend for the given base name.
 *
 * The attribute backend is needed to parse the tags into a more meaningful 
 * structure
 *
 * @param this Uninitialized backend object.
 * @param backend_config Initialized configuration backend.
 * @param base_name Base name.
 */
int poliqarp_backend_tag_open(struct poliqarp_backend_tag *this,
   struct poliqarp_backend_config *backend_config, const char *base_name,
   struct poliqarp_error *error);

/**
 * Close the tag backend and release resources.
 */
void poliqarp_backend_tag_close(struct poliqarp_backend_tag *this);

static inline size_t poliqarp_backend_tag_num_items(
   const struct poliqarp_backend_tag *this)
{
   return GET_NUM_ITEMS(&this->dict);
}

static inline const char *poliqarp_backend_tag_fetch(
   const struct poliqarp_backend_tag *this, size_t key)
{
   assert(key < this->dict.num_items);
   return (const char*) GET_ITEM(&this->dict, key);
}

static inline const struct poliqarp_parsed_tag *poliqarp_backend_parsed_tag_fetch(
   const struct poliqarp_backend_tag *this, size_t key)
{
   assert(key < this->dict.num_items);
   return this->parsed_tag + key;
}

static inline size_t poliqarp_backend_tag_length(
   const struct poliqarp_backend_tag *this, size_t key)
{
   return GET_LENGTH(&this->dict, key);
}

#endif /* POLIQARP_BACKEND_TAG_H */
