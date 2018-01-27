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

#ifndef POLIQARP_BACKEND_CONFIG_H
#define POLIQARP_BACKEND_CONFIG_H

#include <poliqarp-config.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sakura/cdf.h>
#include <sakura/common/hash-table.h>
#include <sakura/common/entity.h>
#include <sakura/common/args.h>
#include <sakura/exception.h>
#include <sakura/query-rewrite.h>

/** @defgroup poliqarp_backend_config Configuration Backend */
/** @{ */
/** @file backend-config.h Configuration Backend */

/* -------------------------------------------------------------- */

struct poliqarp_attr;

/** Single attribute value. */
struct poliqarp_attr_value {
   struct poliqarp_attr_value *next_value;  /**< List stuff.         */
   struct poliqarp_attr *attr;              /**< Attribute pointer. */
   size_t id;                               /**< Identifier. */
   const struct entity *self;           /**< Represented entity. */
};

/** Single attribute name. */
struct poliqarp_attr {
   size_t num_values;                   /**< Number of values associated with 
                                             attribute.              */
   struct poliqarp_attr_value *first_value; /**< Pointer to first value. */
   size_t id;                           /**< Identifier.             */
   const struct entity *self;           /**< Represented entity.     */
};

/** Instance of an attribute (for assembling parts of speech). */
struct poliqarp_attr_instance {
   struct poliqarp_attr_instance *next_instance;
   const struct poliqarp_attr *attr;
   struct poliqarp_part_of_speech *pos;
   bool is_optional;
};

/** Part of speech. */
struct poliqarp_part_of_speech {
   size_t num_instances;
   struct poliqarp_attr_instance *first_instance;
   size_t id;
   const struct entity *self;
};

/** Entity type.
 *
 * Entities are used to get a name-to-object association.
 * The type is used to describe the content of the object
 */
enum poliqarp_entity_type {
   POLIQARP_ENTITY_ATTR,        /**< Attribute name.       */
   POLIQARP_ENTITY_ATTR_VALUE,  /**< Part of speech name?  */
   POLIQARP_ENTITY_POS,         /**< Part of speech name.  */
   POLIQARP_ENTITY_ITEM_POS,    /**< This is the representation of 'pos' 
                                 in expressions.       */
   POLIQARP_ENTITY_ITEM_ORTH,   /**< Likewise 'orth'.      */
   POLIQARP_ENTITY_ITEM_SPACE,  /**< Likewise 'space'.     */
   POLIQARP_ENTITY_ITEM_BASE,   /**< Likewise 'base name'. */
   POLIQARP_ENTITY_ITEM_TAG,    /**< Likewise 'tag'.       */
   POLIQARP_ENTITY_ITEM_TYPE    /**< Likewise 'type'.      */
};

/** Configuration backend structure. */
struct poliqarp_backend_config {
   struct marena arena;           /**< Memory arena used by entities.  */
   struct entity_bag named_items; /**< Entity list.                    */
   struct entity_bag attr_values; /**< Attribute value list.           */
   size_t num_attr;               /**< Number of all attributes.       */
   size_t num_pos;                /**< Number of all parts of speech.  */
   struct hash_table aliases;     /**< Alias manager.                  */
   struct poliqarp_query_rewrite_table query_rewrite_table;
                                  /**< Rules of query rewriting.       */
   struct poliqarp_cdf_info cdf;  /**< Machine-specific configuration. */
};

/* -------------------------------------------------------------- */

/** Attribute constructor. */
struct poliqarp_attr *poliqarp_new_attr(struct poliqarp_backend_config *this,
   const char *name);

/** Attribute value constructor. */
struct poliqarp_attr_value *poliqarp_new_attr_value(
   struct poliqarp_backend_config *this, struct poliqarp_attr *attr, const char *name);

/** Part of speech constructor. */
struct poliqarp_part_of_speech *poliqarp_new_part_of_speech(
   struct poliqarp_backend_config *this, const char *name);

/** Attribute instance constructor. */
struct poliqarp_attr_instance *poliqarp_new_attr_instance(
   struct poliqarp_backend_config *this, struct poliqarp_part_of_speech *pos,
   struct poliqarp_attr *attr, bool is_optional);

/** Open the attribute backend by the given base name. */
int poliqarp_backend_config_open(struct poliqarp_backend_config *this,
   const char *base_name, struct poliqarp_error *error);

/** Close the attribute backend. */
void poliqarp_backend_config_close(struct poliqarp_backend_config *this);

/** Parsed tag representation. */
struct poliqarp_parsed_tag {
   struct poliqarp_part_of_speech *pos;     /**< Part of speech. */
   struct poliqarp_attr_value **attr_value; /**< Array of attributes. 
                                             The size of the array can be 
                                             retrieved from the part of speech 
                                             object.         */
};

/**
 * Parse the tag on the fly.
 * @note the tag is destroyed
 * @return 0 on success, -1 otherwise.
 */
int poliqarp_backend_config_parse(const struct poliqarp_backend_config *this,
   struct poliqarp_parsed_tag *result, char *tag);

/**
 * Parse the tag on the fly.
 * @return 0 on success, -1 otherwise.
 */
int poliqarp_backend_config_parse_copy(const struct poliqarp_backend_config *this,
   struct poliqarp_parsed_tag *result, const char *tag);

/** @} */

#endif /* POLIQARP_BACKEND_CONFIG_H */

