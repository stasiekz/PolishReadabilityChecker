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

#include <locale.h>
#include <stdio.h>

#include <foostring/foostring.h>
#include <sakura/poliqarp.h>

#include <unibits/strcoll.h>

#define POLIQARP_MAJOR_VERSION 1
#define POLIQARP_MINOR_VERSION 3
#define POLIQARP_REVISION_NUMBER 12
#define POLIQARP_LIBRARY_NAME    "sakura"

const int poliqarp_major_version = POLIQARP_MAJOR_VERSION;
const int poliqarp_minor_version = POLIQARP_MINOR_VERSION;
const int poliqarp_revision_number = POLIQARP_REVISION_NUMBER;
const char poliqarp_library_name[] = POLIQARP_LIBRARY_NAME;

int poliqarp_create(const char *locale, struct poliqarp_error *error)
{
   int rc;
   setlocale(LC_ALL, locale);
   rc = poliqarp_regexp_validate_utf8() || unibits_validate_utf8();
   if (rc < 0)
      goto error;
   if (rc > 0) {
      /* Maybe UTF-8 variant of LC_CTYPE is fine? */
      const char *locale = setlocale(LC_CTYPE, NULL);
      if (locale != NULL) {
         const char *locale_end = locale;
         while (*locale_end != '\0' && *locale_end != '.')
            locale_end++;
         size_t length = locale_end - locale;
         char *new_locale = malloc(length + 7);
         if (new_locale == NULL)
            goto error;
         new_locale[0] = '\0';
         strncat(new_locale, locale, length);
         strcat(new_locale + length, ".UTF-8");
         setlocale(LC_CTYPE, new_locale);
         free(new_locale);
         rc = poliqarp_regexp_validate_utf8() || unibits_validate_utf8();
         if (rc < 0)
            goto error;
      }
   }
   if (rc > 0) {
      /* Maybe LC_COLLATE is fine? */
      const char *locale = setlocale(LC_COLLATE, NULL);
      if (locale != NULL) {
         char *new_locale = malloc(strlen(locale) + 7);
         if (new_locale == NULL)
            goto error;
         strcpy(new_locale, locale);
         setlocale(LC_CTYPE, new_locale);
         rc = poliqarp_regexp_validate_utf8() || unibits_validate_utf8();
         if (rc < 0)
            goto error;
         if (rc > 0) {
            /* Maybe UTF-8 variant of LC_COLLATE is fine? */
            char *new_locale_end = new_locale;
            while (*new_locale_end != '\0' && *new_locale_end != '.')
               new_locale_end++;
            strcpy(new_locale_end, ".UTF-8");
            setlocale(LC_CTYPE, new_locale);
            rc = poliqarp_regexp_validate_utf8() || unibits_validate_utf8();
            if (rc < 0)
               goto error;
         }
         free(new_locale);
      }
   }
   if (rc > 0) {
      /* Maybe en_US.UTF-8 is available? */
      setlocale(LC_CTYPE, "en_US.UTF-8");
      rc = poliqarp_regexp_validate_utf8() || unibits_validate_utf8();
   }
   if (rc != 0) {
      poliqarp_error_message_set(error, _("Unable to set a UTF-8 locale"));
      return -1;
   }
   return 0;
error:
   poliqarp_error_from_system(error, _("Unable to initialize the Poliqarp library"));
   return -1;
}

int poliqarp_destroy(void)
{
   return 0;
}

int poliqarp_get_corpus_info(const struct poliqarp_corpus *corpus,
   struct poliqarp_corpus_info *info)
{
   info->num_segments = poliqarp_backend_corpus_size(&corpus->corpus);
   info->num_types = poliqarp_backend_orth_num_items(
      poliqarp_get_const_backend(corpus, orth));
   info->num_lemmata = poliqarp_backend_base_num_items__disamb(
      poliqarp_get_const_backend(corpus, base));
   info->num_tags = poliqarp_backend_tag_num_items(
      poliqarp_get_const_backend(corpus, tag));
   return 0;
}

int poliqarp_get_segment(struct poliqarp_segment *segment, 
   struct poliqarp_corpus *corpus, size_t index)
{
#ifndef NDEBUG
   if (index >= poliqarp_backend_corpus_size(&corpus->corpus))
      return -1;
#endif  

   segment->corpus = corpus;
   segment->segment = poliqarp_backend_corpus_get(&corpus->corpus, index);

   return 0;
}

int poliqarp_get_segment_info(const struct poliqarp_segment *segment,
   struct poliqarp_segment_info *info)
{
   info->space_before = segment->segment.orth_space_id & 1;
   info->text = poliqarp_backend_orth_fetch(
      poliqarp_get_const_backend(segment->corpus, orth), 
      segment->segment.orth_space_id >> 1);
   return 0;
}

int poliqarp_get_disambiguated_interpretations(
   const struct poliqarp_segment *segment,
   struct poliqarp_interpretation_set *set)
{
   set->corpus = segment->corpus;
   set->set = segment->segment.interp_disamb_id;
   set->disamb = true;
   return 0;
}

int poliqarp_get_ambiguous_interpretations(
   const struct poliqarp_segment *segment,
   struct poliqarp_interpretation_set *set)
{
   set->corpus = segment->corpus;
   set->set = segment->segment.interp_amb_id;
   set->disamb = false;
   return 0;
}

int poliqarp_get_interpretation_set_info(
   const struct poliqarp_interpretation_set *set,
   struct poliqarp_interpretation_set_info *info)
{
   info->size = set->disamb ? 
      poliqarp_backend_interp_length__disamb(
         poliqarp_get_const_backend(set->corpus, interp), set->set) :
      poliqarp_backend_interp_length__amb(
         poliqarp_get_const_backend(set->corpus, interp), set->set);
   return 0;
}

int poliqarp_get_interpretation(const struct poliqarp_interpretation_set *set,
   struct poliqarp_interpretation *interp, size_t index)
{
   const struct poliqarp_binary_interp *binterp;
   binterp = set->disamb ?
      poliqarp_backend_interp_fetch__disamb(
         poliqarp_get_const_backend(set->corpus, interp), set->set) :
      poliqarp_backend_interp_fetch__amb(
         poliqarp_get_const_backend(set->corpus, interp), set->set);
   interp->corpus = set->corpus;
   interp->disamb = set->disamb;
   interp->interp = binterp[index];
   POLIQARP_INTERP_LE_TO_HE(interp->interp);
   return 0;
}

int poliqarp_get_interpretation_info(
   const struct poliqarp_interpretation *interp,
   struct poliqarp_interpretation_info *info)
{
   info->base = interp->disamb ?
      poliqarp_backend_base_fetch__disamb(
         poliqarp_get_const_backend(interp->corpus, base), 
         interp->interp.base_id) :
      poliqarp_backend_base_fetch__amb(
         poliqarp_get_const_backend(interp->corpus, base), 
         interp->interp.base_id);
   info->tag = poliqarp_backend_tag_fetch(
      poliqarp_get_const_backend(interp->corpus, tag), interp->interp.tag_id);
   return 0;
}

int poliqarp_define_alias(struct poliqarp_corpus *corpus, const char *name,
   const char *value)
{
   if (hash_table_set(&(poliqarp_get_backend(corpus, config)->aliases), name, 
      strdup(value)))
   {
      return -1;
   }
   return 0;
}

int poliqarp_delete_alias(struct poliqarp_corpus *corpus, const char *name)
{
   if (hash_table_unset(&(poliqarp_get_backend(corpus, config)->aliases), name))
      return -1;
   return 0;
}

static void get_aliases_iterator(const char *key, const void *value,
   void *env)
{
   struct poliqarp_alias **alias = (struct poliqarp_alias **)env;
   (*alias)->name = key;
   (*alias)->value = (const char *)value;
   (*alias)++;
}

int poliqarp_get_aliases(const struct poliqarp_corpus *corpus,
   struct poliqarp_alias_list *aliases)
{
   const struct hash_table *table = 
      &(poliqarp_get_const_backend(corpus, config)->aliases);
   struct poliqarp_alias *tmp;
   aliases->num_aliases = hash_table_num_items(table);
   tmp = aliases->aliases = malloc(aliases->num_aliases * 
      sizeof(struct poliqarp_alias));
   hash_table_iterate(table, &tmp, get_aliases_iterator);
   return 0;
}

int poliqarp_free_aliases(struct poliqarp_alias_list *aliases)
{
   free(aliases->aliases);
   return 0;
}

int poliqarp_get_metadata_set(const struct poliqarp_corpus *corpus,
   size_t id, struct poliqarp_metadata_set *meta)
{
   struct poliqarp_document document;
   if (poliqarp_backend_document_fetch(&corpus->document, id, &document) == -1)
      return -1;
   meta->corpus = corpus;
   meta->low = document.meta_low;
   meta->high = document.meta_high;
   return 0;
}

size_t poliqarp_metadata_count(const struct poliqarp_metadata_set *meta)
{
   return meta->high - meta->low;
}

int poliqarp_get_metadata(const struct poliqarp_metadata_set *set, 
   size_t index, struct poliqarp_metadata *meta)
{
   index += set->low;
   meta->corpus = set->corpus;   
   meta->meta = poliqarp_backend_meta_fetch(poliqarp_get_const_backend(
      set->corpus, meta), index);
   return 0;
}

int poliqarp_get_metadata_types(struct poliqarp_metadata_types *types,
   const struct poliqarp_corpus *corpus)
{
   int num = 0;
   struct poliqarp_meta_type_list *mtypes = corpus->meta.types;
   
   while (mtypes) {
      ++num;
      mtypes = mtypes->next;
   }
   types->types = malloc(num * sizeof *(types->types));
   if (types->types == NULL)
      return -1;
   types->num_types = num;
   
   mtypes = corpus->meta.types;
   while (num) {
      num--;
      types->types[num].key = mtypes->key;
      types->types[num].type = 
         (mtypes->type == POLIQARP_META_TYPE_STRING) ? POLIQARP_META_TEXT :
         POLIQARP_META_DATE;
      mtypes = mtypes->next;
   }
   return 0;
}

int poliqarp_free_metadata_types(struct poliqarp_metadata_types *types)
{
   free(types->types);
   return 0;
}

int poliqarp_get_metadata_info(const struct poliqarp_metadata *meta,
   struct poliqarp_metadata_info *info)
{
   info->key = poliqarp_backend_meta_key_fetch(poliqarp_get_const_backend(
      meta->corpus, meta), meta->meta.key);
   switch (meta->meta.type) {
      case POLIQARP_METADATA_SINGLE:
      case POLIQARP_METADATA_MULTI:
         info->type = POLIQARP_META_TEXT;
         info->value.text = poliqarp_backend_meta_value_fetch(
            poliqarp_get_const_backend(meta->corpus, meta), 
            meta->meta.value_as.text);
         break;
      case POLIQARP_METADATA_DATE:
         info->type = POLIQARP_META_DATE;
         info->value.date.year = meta->meta.value_as.date.year;
         info->value.date.month = meta->meta.value_as.date.month;
         info->value.date.day = meta->meta.value_as.date.day;
         break;
      case POLIQARP_METADATA_UNDEFINED:
         info->type = POLIQARP_META_UNDEFINED;
         break;
      default:
         abort(); /* Should not happen. */
   }
   return 0;
}

int poliqarp_get_tagset_info(const struct poliqarp_corpus *corpus,
   struct poliqarp_tagset_info *info)
{
   const struct poliqarp_backend_config *cfg = 
      poliqarp_get_const_backend(corpus, config);
   struct entity *entity;
   struct poliqarp_attr *attr;
   struct poliqarp_attr_value *aval;
   struct poliqarp_part_of_speech *pos;
   struct poliqarp_attr_instance *ainst;
   string_t s;

   /* first pass: gather number of classes and categories */
   info->num_categories = info->num_classes = 0;
   for (entity = cfg->named_items.first_entity; entity; 
      entity = entity->next_entity) 
   {
      switch (*(enum poliqarp_entity_type *)entity->tag) {
         case POLIQARP_ENTITY_POS: info->num_classes++; break;
         case POLIQARP_ENTITY_ATTR: info->num_categories++; break;
         default: break;
      }
   }

   /* allocate memory */
   info->classes = malloc(info->num_classes * sizeof(*(info->classes)));
   info->categories = malloc(info->num_categories * sizeof(*(info->categories)));

   /* second pass: retrieve the info */
   info->num_categories = info->num_classes = 0;
   for (entity = cfg->named_items.first_entity; entity;
      entity = entity->next_entity)
   {
      switch (*(enum poliqarp_entity_type *)entity->tag) {
         case POLIQARP_ENTITY_POS:
            s = string_create();
            string_append_str(s, entity->name);
            pos = (struct poliqarp_part_of_speech *)entity->data;
            for (ainst = pos->first_instance; ainst; 
               ainst = ainst->next_instance)
            {
               string_append_str(s, " ");
               if (ainst->is_optional)
                  string_append_str(s, "[");
               string_append_str(s, ainst->attr->self->name);
               if (ainst->is_optional)
                  string_append_str(s, "]");
            }
            info->classes[info->num_classes++] = string_free_and_get_buffer(s);
            break;
         case POLIQARP_ENTITY_ATTR: 
            s = string_create();
            string_append_str(s, entity->name);
            attr = (struct poliqarp_attr *)entity->data;
            for (aval = attr->first_value; aval; aval = aval->next_value) {
               string_append_str(s, " ");
               string_append_str(s, aval->self->name);
            }
            info->categories[info->num_categories++] = string_free_and_get_buffer(s);
            break;
         default:
            break;
      }
   }
   return 0;
}

void poliqarp_free_tagset_info(struct poliqarp_tagset_info *info)
{
   size_t i;
   for (i = 0; i < info->num_classes; i++)
      free(info->classes[i]);
   for (i = 0; i < info->num_categories; i++)
      free(info->categories[i]);
}
