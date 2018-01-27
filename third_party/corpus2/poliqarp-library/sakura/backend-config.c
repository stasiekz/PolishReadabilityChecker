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

#include <foostring/foostring.h>

#include <sakura/backend-config.h>
#include <sakura/backend-index.h>
#include <sakura/abi.h>
#include <sakura/cdf.h>
#include <sakura/query-rewrite.h>
#include <sakura/exception.h>
#include <sakura/config.h>

enum poliqarp_section {
   SECTION_NONE = -1,
   SECTION_STARTUP,
   SECTION_ALIASES,
   SECTION_ATTR,
   SECTION_POS,
   SECTION_NAMED_ENTITY,
   SECTION_QUERY_REWRITE_RULES,
};

/* attribute construction */

struct poliqarp_attr *poliqarp_new_attr(struct poliqarp_backend_config *this,
   const char *name)
{
   static enum poliqarp_entity_type tag = POLIQARP_ENTITY_ATTR;
   struct entity *self;
   struct poliqarp_attr *result;

   /* create entity */
   self = new_entity(&this->named_items, name, &tag, NULL);

   if (self == NULL)
      return NULL;

   /* create attr */
   result = marena_alloc(&this->arena, sizeof *result);
   if (result == NULL)
      return NULL;
   result->self = self;
   result->num_values = 0;
   result->first_value = NULL;
   result->id = this->num_attr++;

   /* update pointer in entity */
   self->data = result;

   return result;
}

struct poliqarp_attr_value *poliqarp_new_attr_value(
   struct poliqarp_backend_config *this, struct poliqarp_attr *attr, const char *name)
{
   static enum poliqarp_entity_type tag = POLIQARP_ENTITY_ATTR_VALUE;
   struct entity *self;
   struct poliqarp_attr_value *result;

   /* create entity NOTE: it uses different entity bag */
   self = new_entity(&this->attr_values, name, &tag, NULL);

   if (self == NULL)
      return NULL;

   /* create attr value */
   result = marena_alloc(&this->arena, sizeof *result);
   if (result == NULL)
      return NULL;
   result->self = self;
   result->next_value = attr->first_value;
   result->attr = attr;
   result->id = attr->num_values++;

   /* update pointer in entity */
   self->data = result;

   /* update attr's value list */
   attr->first_value = result;

   return result;
}

/* part of speech construction */

struct poliqarp_part_of_speech *poliqarp_new_part_of_speech(
   struct poliqarp_backend_config *this, const char *name)
{
   static enum poliqarp_entity_type tag = POLIQARP_ENTITY_POS;
   struct entity *self;
   struct poliqarp_part_of_speech *result;

   /* create entity */
   self = new_entity(&this->named_items, name, &tag, NULL);

   if (self == NULL)
      return NULL;

   /* create part of speech */
   result = marena_alloc(&this->arena, sizeof *result);
   if (result == NULL)
      return NULL;
   result->self = self;
   result->num_instances = 0;
   result->first_instance = NULL;
   result->id = this->num_pos++;

   /* update pointer in entity */
   self->data = result;

   return result;
}

/* attribute instance construction */

struct poliqarp_attr_instance *poliqarp_new_attr_instance(
   struct poliqarp_backend_config *this, struct poliqarp_part_of_speech *pos,
   struct poliqarp_attr *attr, bool is_optional)
{
   struct poliqarp_attr_instance *result = marena_alloc(&this->arena, 
      sizeof *result);
   if (result == NULL)
      return NULL;

   /* create instance */
   result->next_instance = pos->first_instance;
   result->attr = attr;
   result->pos = pos;
   result->is_optional = is_optional;

   /* update attr instance list in part of speech */
   pos->first_instance = result;

   return result;
}

/** Configuration parser handler. */
static int poliqarp_backend_config_handler(void *exta, int section,
   char *text, struct poliqarp_error *error);

int poliqarp_backend_config_open(struct poliqarp_backend_config *this, 
   const char *base_name, struct poliqarp_error *error)
{
   int rc;
   bool have_marena = false, have_named_items = false,
      have_attr_values = false, have_aliases = false,
      have_query_rewrite_table = false;
   static enum poliqarp_entity_type tag_pos = POLIQARP_ENTITY_ITEM_POS;
   static enum poliqarp_entity_type tag_orth = POLIQARP_ENTITY_ITEM_ORTH;
   static enum poliqarp_entity_type tag_base = POLIQARP_ENTITY_ITEM_BASE;
   static enum poliqarp_entity_type tag_tag = POLIQARP_ENTITY_ITEM_TAG;
   static enum poliqarp_entity_type tag_space = POLIQARP_ENTITY_ITEM_SPACE;
   static enum poliqarp_entity_type tag_type = POLIQARP_ENTITY_ITEM_TYPE;

   /* create arena */
   marena_create(&this->arena);
   have_marena = true;

   /* create two entity bags */
   create_entity_bag(&this->named_items, &this->arena);
   have_named_items = true;
   create_entity_bag(&this->attr_values, &this->arena);
   have_attr_values = true;

   /* create alias manager */
   create_hash_table(&this->aliases, 25, HASHTABLE_DUPLICATE_KEYS, &this->arena);
   have_aliases = true;

   /* create query rewrite table */
   poliqarp_create_query_rewrite_table(&this->query_rewrite_table);
   have_query_rewrite_table = true;

   /* reset counters */
   this->num_attr = 0;
   this->num_pos = 0;

   /* create default entities */
   new_entity(&this->named_items, "entity-pos", &tag_pos, NULL);
   new_entity(&this->named_items, "entity-orth", &tag_orth, NULL);
   new_entity(&this->named_items, "entity-base", &tag_base, NULL);
   new_entity(&this->named_items, "entity-tag", &tag_tag, NULL);
   new_entity(&this->named_items, "space", &tag_space, NULL);
   new_entity(&this->named_items, "type", &tag_type, NULL);

   static const struct poliqarp_config_section sections[] = {
      { "STARTUP", SECTION_STARTUP }, /* obsolete */
      { "ALIASES", SECTION_ALIASES },
      { "ATTR", SECTION_ATTR },
      { "POS", SECTION_POS },
      { "NAMED-ENTITY", SECTION_NAMED_ENTITY },
      { "QUERY-REWRITE-RULES", SECTION_QUERY_REWRITE_RULES },
      { NULL, 0 }
   };

   /* parse the config file */
   char *conf_path = string_aformat("%s.cfg", base_name); 
   if (conf_path == NULL) {
      poliqarp_error_from_system(error, "Unable to read corpus configuration file");
      goto error;
   }
   rc = poliqarp_parse_config_file(this, conf_path,
      sections, poliqarp_backend_config_handler, error);
   if (rc != 0)
      goto error;
   free(conf_path);

   /* read CDF */
   rc = poliqarp_cdf_read(base_name, &this->cdf);
   if (rc != 0 && errno != ENOENT) {
      poliqarp_error_from_system(error, _("Unable to read CDF file"));
      goto error;
   }

   /* validate corpus format */
   if (this->cdf.version != POLIQARP_ABI_VERSION) {
      poliqarp_error_message_set(error,
         _("Version %d of binary format is not supported"), this->cdf.version);
      goto error;
   }
   else if (this->cdf.endianness != ENDIAN_LE) {
      poliqarp_error_message_set(error,
         _("Big-endian binary format is not supported"), this->cdf.version);
      goto error;
   }
   return 0;
error:
   if (have_named_items)
      destroy_entity_bag(&this->named_items);
   if (have_attr_values)
      destroy_entity_bag(&this->attr_values);
   if (have_aliases)
      destroy_hash_table(&this->aliases, free);
   if (have_query_rewrite_table)
      poliqarp_destroy_query_rewrite_table(&this->query_rewrite_table);
   if (have_marena)
      marena_destroy(&this->arena);
   return -1;
}

void poliqarp_backend_config_close(struct poliqarp_backend_config *this)
{
   /* get rid of everything */
   destroy_entity_bag(&this->named_items);
   destroy_entity_bag(&this->attr_values);
   destroy_hash_table(&this->aliases, free);
   poliqarp_destroy_query_rewrite_table(&this->query_rewrite_table);

   /* free everything we have, quick and painless */
   marena_destroy(&this->arena);
}

/* tag parsers */

/**
 * Parse the tag on the fly, the tag is destroyed.
 */ 
int poliqarp_backend_config_parse(const struct poliqarp_backend_config *this,
   struct poliqarp_parsed_tag *result, char *tag)
{
   const struct entity *entity;
   char *item;
   size_t i;

   /* reset attributes */
   for (i = 0; i < this->num_attr; ++i)
      result->attr_value[i] = NULL;

   /* parse part of speech */
   item = strtok(tag, ":");
   entity = lookup_const_entity(&this->named_items, item);
   if (entity == NULL)
      return -1;
   if (entity->tag == NULL || 
       *(enum poliqarp_entity_type *)entity->tag != POLIQARP_ENTITY_POS)
   {
      errno = EINVAL;
      return -1;
   }
   result->pos = entity->data;

   /* parse attributes */
   while ((item = strtok(NULL, ":"))) {
      entity = lookup_const_entity(&this->attr_values, item);
      if (entity == NULL)
         return -1;
      if (entity == NULL || entity->tag == NULL || 
          *(enum poliqarp_entity_type *)entity->tag != POLIQARP_ENTITY_ATTR_VALUE)
      {
         errno = EINVAL;
         return -1;
      }
      result->attr_value[((struct poliqarp_attr_value *) entity->data)->attr->id] =
         entity->data;
   }
   return 0;
}

/**
 * Non-destructive version of poliqarp_backend_config_parse_copy(). */
int poliqarp_backend_config_parse_copy(const struct poliqarp_backend_config *this,
   struct poliqarp_parsed_tag *result, const char *tag)
{
   char *copy = strdup(tag);
   if (copy == NULL)
      return -1;
   int rc = poliqarp_backend_config_parse(this, result, copy);
   free(copy);
   return rc;
}

static int poliqarp_backend_config_handler(void *extra, int section,
   char *text, struct poliqarp_error *error)
{
   struct poliqarp_backend_config *this = extra;
   bool have_args = false, have_variable = true;
   struct poliqarp_variable variable;
   struct text_args args;

   int rc;
   bool is_optional = false;
   size_t i;

   struct entity *entity;
   struct entity *result;
   struct poliqarp_part_of_speech *pos;
   struct poliqarp_attr *attr;

   args_init(&args);

   switch (section) {
      case SECTION_ATTR:
      case SECTION_POS:
      case SECTION_NAMED_ENTITY:
      case SECTION_QUERY_REWRITE_RULES:
         rc = poliqarp_parse_variable(&variable, text);
         if (rc != 0) {
            if (errno == EINVAL) {
               poliqarp_error_message_set(error, _("a line is not a key=value pair"));
               goto error;
            } else
               goto system_error;
         }
         have_variable = true;
         rc = args_parse(&args, variable.value);
         if (rc != 0)
            goto system_error;
         have_args = true;
         break;
      case SECTION_STARTUP: /* support for old syntax: /alias foo = bar baz */
         rc = poliqarp_parse_variable(&variable, text);
         if (rc != 0)
            goto system_error;
         have_variable = true;
         if (strncmp(variable.name, "/alias ", 7) == 0) {
            char *tmp = variable.value;
            for (; *tmp; tmp++)
               if (*tmp == ' ')
                  *tmp = '|';
            rc = hash_table_set(&this->aliases, variable.name + 7, strdup(variable.value));
            if (rc != 0)
               goto system_error;
         }
         poliqarp_free_variable(&variable);
         have_variable = false;
         break;
      case SECTION_ALIASES:
         rc = poliqarp_parse_variable(&variable, text);
         if (rc != 0)
            goto system_error;
         have_variable = true;
         rc = hash_table_set(&this->aliases, variable.name, strdup(variable.value));
         if (rc != 0)
            goto system_error;
         poliqarp_free_variable(&variable);
         have_variable = false;
         break;
      default:
         break;
   }
   switch (section) {
      case SECTION_POS:
         /* create new part of speech */
         pos = poliqarp_new_part_of_speech(this, variable.name);
         if (pos == NULL) {
            if (errno == EEXIST) {
               poliqarp_error_message_set(error,
                  _("unable to redefine part-of-speech '%s'"), variable.name);
               goto error;
            }
            else
               goto system_error;
         }
         for (i = 0; i < args.num_items; ++i) {
            char *item = args.item[i].value;
            /* detect optional markers */
            if (strcmp(item, "[") == 0) {
               is_optional = true;
               continue;
            }
            /* lookup name */
            entity = lookup_entity(&this->named_items, item);
            if (entity == NULL) {
               if (errno == ENOENT) {
                  poliqarp_error_message_set(error, _("entity '%s' is undefined"), item);
                  goto error;
               } else
                  goto system_error;
            }
            if (entity->tag == NULL || 
                *(enum poliqarp_entity_type *)entity->tag != POLIQARP_ENTITY_ATTR)
            {
               poliqarp_error_message_set(error, _("entity '%s' is not an attribute"), item);
               goto error;
            }
            attr = entity->data;
            /* create new attr instance */
            if (poliqarp_new_attr_instance(this, pos, attr, is_optional) == NULL)
               goto system_error;
            if (is_optional) {
               ++i;
               is_optional = false;
            }
         }
         break;

      case SECTION_ATTR:
         /* create new attribute */
         attr = poliqarp_new_attr(this, variable.name);
         if (attr == NULL) { 
            if (errno == EEXIST) {
               poliqarp_error_message_set(error, _("unable to redefine attribute '%s'"),
                  variable.name);
               goto error;
            }
            else
               goto system_error;
         }

         /* add values */
         for (i = 0; i < args.num_items; ++i)
            if (poliqarp_new_attr_value(this, attr, args.item[i].value) == NULL) {
               if (errno == EEXIST) {
                  poliqarp_error_message_set(error, _("duplicate attribute value '%s'"), args.item[i].value);
                  goto error;
               }
               else
                  goto system_error;
            }
         break;
      case SECTION_NAMED_ENTITY:
         entity = lookup_entity(&this->named_items, variable.name);

         if (entity == NULL) {
            if (errno == ENOENT) {
               poliqarp_error_message_set(error, _("entity '%s' is undefined"), variable.name);
               goto error;
            }
            else
               goto system_error;
         }
         /* make aliases */
         for (i = 0; i < args.num_items; ++i) {
            result = new_entity_alias(&this->named_items, entity, 
               args.item[i].value);
            if (result != NULL)
               continue;
            if (errno == EEXIST) {
               poliqarp_error_message_set(error,
                  _("unable to redefine '%s' as '%s'"), entity->name,
                  args.item[i].value);
               goto error;
            }
            else
               goto system_error;
         }
         break;
      case SECTION_QUERY_REWRITE_RULES:
         if (args.num_items != 2) {
            poliqarp_error_message_set(error, _("unable to create rule set %s; "
               "each rewrite rule should consist of exactly 2 strings"), variable.name);
            goto error;
         }
         {
            struct poliqarp_query_rewrite *rewrite =
               poliqarp_get_query_rewrite(&this->query_rewrite_table, variable.name, true);
            if (rewrite == NULL)
               goto system_error;
            rc = poliqarp_add_query_rewrite_rule(rewrite, args.item[0].value, args.item[1].value);
            if (rc != 0)
               goto system_error;
         }
      default:
         break;
   } /* switch (section) */
   rc = 0;
   goto done;
system_error:
   poliqarp_error_from_system(error, NULL);
error:
   rc = -1;
done:
   if (have_variable)
      poliqarp_free_variable(&variable);
   if (have_args)
      args_free(&args);
   return rc;
}
