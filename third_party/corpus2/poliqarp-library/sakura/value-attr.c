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

#include <errno.h>

#include <sakura/value-attr.h>
#include <sakura/regexp.h>

struct poliqarp_value *poliqarp_value_create_attr(const struct poliqarp_corpus *corpus,
   const char *name, const struct poliqarp_regexp *pattern)
{
   struct poliqarp_value *this;

   struct poliqarp_attr *attr;

   const struct poliqarp_backend_tag *backend_tag = &corpus->tag;
   const struct poliqarp_backend_config *backend_config = &corpus->config;
   const struct poliqarp_attr_value *value;
   const struct entity *entity;

   size_t i;

   unsigned int *bitmap;

   /* find our attribute */
   entity = lookup_const_entity(&backend_config->named_items, name);
   if (entity == NULL || 
      *(enum poliqarp_entity_type *) entity->tag != POLIQARP_ENTITY_ATTR)
   {
      errno = ENOENT;
      return NULL;
   }
   attr = entity->data;

   /* create bitmap big enough to hold each part of speech */
   bitmap = malloc(BIT_ARRAY_LENGTH_BYTES(bitmap, attr->num_values));
   memset(bitmap, 0, BIT_ARRAY_LENGTH_BYTES(bitmap, attr->num_values));

   for (value = attr->first_value; value; value = value->next_value) {
      if (poliqarp_regexp_match(pattern, value->self->name))
         BIT_ARRAY_SET(bitmap, value->id);
   }

   /* now allocate value */
   this = malloc(sizeof *this);
   this->num_items = poliqarp_backend_tag_num_items(backend_tag);
   this->num_bytes = BIT_ARRAY_LENGTH_BYTES(this->bits, this->num_items);
   this->num_hits = 0;
   this->bits = malloc(this->num_bytes);
   this->domain = POLIQARP_DOMAIN_TAG;

   /* clear the bit field */
   memset(this->bits, 0, this->num_bytes);

   {
      size_t attr_id = attr->id;
      const struct poliqarp_attr_value *attr_value;
      /* iterate over all tags and find those that match */
      for (i = 0; i < this->num_items; ++i) {
         attr_value = poliqarp_backend_parsed_tag_fetch(backend_tag, i)->
            attr_value[attr_id];
         if (attr_value && BIT_ARRAY_GET(bitmap, attr_value->id)) {
            BIT_ARRAY_SET(this->bits, i);
            ++this->num_hits;
         }
      }
   }

   free(bitmap);
   return this;
}

struct poliqarp_value **poliqarp_value_create_all_attr(const struct poliqarp_corpus *corpus,
   const char *name, size_t *n)
{
   struct poliqarp_value **values, *this;

   const struct poliqarp_backend_tag *backend_tag = &corpus->tag;
   const struct poliqarp_backend_config *backend_config = &corpus->config;
   const struct poliqarp_attr_value *avalue1, *avalue2;
   const struct entity *entity;
   struct poliqarp_attr *attr;
   size_t n_tags, i;

   entity = lookup_const_entity(&backend_config->named_items, name);
   if (entity == NULL || 
      *(enum poliqarp_entity_type *) entity->tag != POLIQARP_ENTITY_ATTR)
   {
      errno = ENOENT;
      return NULL;
   }
   attr = entity->data;

   *n = attr->num_values;
   values = malloc(*n * sizeof(*this));

   n_tags = poliqarp_backend_tag_num_items(backend_tag);
   for (avalue1 = attr->first_value; avalue1; avalue1 = avalue1->next_value) {
      values[avalue1->id] = this = malloc(sizeof(*this));
      this->num_items = n_tags;
      this->num_bytes = BIT_ARRAY_LENGTH_BYTES(this->bits, n_tags);
      this->bits = malloc(this->num_bytes);
      this->domain = POLIQARP_DOMAIN_TAG;
      this->num_hits = 0;
      memset(this->bits, 0, this->num_bytes);
      for (i = 0; i < n_tags; i++) {
         const struct poliqarp_parsed_tag *parsed_tag =
            poliqarp_backend_parsed_tag_fetch(backend_tag, i);
         avalue2 = parsed_tag->attr_value[attr->id];
         if (avalue2 && (avalue1->id == avalue2->id)) {
            BIT_ARRAY_SET(this->bits, i);
            this->num_hits++;
         }
      }
   }
   return values;
}
