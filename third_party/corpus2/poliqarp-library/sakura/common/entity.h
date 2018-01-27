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
 * @file entity.h
 * @brief Named entities and entity bags.
 *
 * An entity can be just about anything that has a name. Entities can be
 * perceived as mappings from names to arbitrary data. They can also have
 * tags (metadata) associated with them, which also can be just about anything.
 * It is possible to create aliases (alternate names) for already existing 
 * entities.
 *
 * Entities are grouped in collections called 'entity bags'. It is possible
 * to look up entities by name in a bag.
 */

#ifndef ENTITY_H
#define ENTITY_H

#include <poliqarp-config.h>

#include <stdlib.h>
#include <string.h>

#include <sakura/common/memory-arena.h>

/** 
 * A named entity.
 * @note When this->tag == this, entity is an alias and this->data is the 
 *    aliased entity.
 */
struct entity {
   struct entity *next_entity;  /**< Entities are assembled together into a
                                     singly-linked list. */
   char *name;                  /**< Name of the entity. */
   void *tag;                   /**< Metadata mechanism. */
   void *data;                  /**< Data pointer.       */
};

/**
 * Bag of entities.
 */
struct entity_bag {
   struct entity *first_entity; /**< First element of the list of entities in
                                     this bag. */
   size_t num_items;            /**< Number of items in this bag. */
   struct marena *arena;        /**< Memory arena used as a backing storage. */
   bool local_arena;            /**< If true, the arena is local for this bag
                                     and should be destroyed when the bag
                                     gets freed. */
};

/**
 * Creates an empty entity bag.
 * @param this The structure to be initialized.
 * @param arena Memory arena used as a backing storage. If this is NULL,
 *    the bag will allocate an arena for its own use. 
 * @return 0 on success, -1 on error.
 */
int create_entity_bag(struct entity_bag *this, struct marena *arena);

/**
 * Destroys an entity bag and all entities contained in it. 
 * @param this The bag to be destroyed.
 */
void destroy_entity_bag(struct entity_bag *this);

/**
 * Constructs and returns a new entity.
 * @param this The bag that will contain the entity.
 * @param name Name of the entity. 
 * @param tag Metadata for the entity.
 * @param tag Data for the entity.
 * @return The newly-created entity.
 */
struct entity *new_entity(struct entity_bag *this, const char *name, void *tag,
   void *data);

/**
 * Searches for an entity in the entity bag.
 * @param this The bag to search in.
 * @param name Name of the entity to search for.
 * @return The first matching entity if found, or NULL if there is no such
 *    entity.
 */
struct entity *lookup_entity(struct entity_bag *this, const char *name);

/**
 * Same as lookup_entity, except that the parameters must be constant. 
 */
const struct entity *lookup_const_entity(const struct entity_bag *this, 
   const char *name);

/**
 * Creates an alias for the existing entity.
 * @param this The bag that will contain the alias.
 * @param entity The entity that the alias will point to.
 * @param name Name of the new alias.
 * @return The resulting alias.
 */
struct entity *new_entity_alias(struct entity_bag *this, struct entity *entity,
   const char *name);

#endif /* ENTITY_H */
