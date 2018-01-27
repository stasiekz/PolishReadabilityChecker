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
#include <sys/types.h>

#include <sakura/backend-meta.h>
#include <sakura/exception.h>
#include <sakura/common/getline.h>
#include <sakura/dict.h>
#include <foostring/foostring.h>

static void free_type_list(struct poliqarp_meta_type_list *head)
{
   while (head) {
      struct poliqarp_meta_type_list *elem = head;
      head = head->next;
      free(elem->key);
      free(elem);
   }
}

static int read_type_list(struct poliqarp_meta_type_list **head, 
   const char *file_name)
{
   FILE *fp = fopen(file_name, "rt");
   char *buf = NULL;
   size_t buflen = 0;
  
   *head = NULL;
   if (fp == NULL) 
      return -1;
   for (;;) {
      struct poliqarp_meta_type_list *elem;
      enum poliqarp_meta_type type = POLIQARP_META_TYPE_STRING;
      bool have_type = false;
      size_t len;
      
      if (getline(&buf, &buflen, fp) == -1)
         break;
      len = strlen(buf);
      if (len < 3)
         continue;
      if (buf[len - 1] == '\n')
         buf[len - 1] = '\0';
      switch (buf[0]) {
         case 'S': 
            type = POLIQARP_META_TYPE_STRING; 
            have_type = true;
            break;
         case 'D':
            type = POLIQARP_META_TYPE_DATE;
            have_type = true;
            break;            
      }
      if (!have_type)
         continue;
      elem = malloc(sizeof *elem);
      if (elem == NULL)
         break;
      elem->next = *head;
      elem->type = type;
      elem->key = strdup(buf + 2);
      if (elem->key == NULL) {
         free(elem);
         break;
      }
      *head = elem;
   }
   free(buf);
   if (!feof(fp)) {
      int my_errno = errno;
      fclose(fp); /* It's safe to ignore errors. */
      *head = NULL;
      errno = my_errno;
      return -1;
   }
   else {
      fclose(fp); /* It's safe to ignore errors. */
      return 0;
   }
}

int poliqarp_backend_meta_open(struct poliqarp_backend_meta *this, 
   const char *base_name, struct poliqarp_error *error)
{
   char *path;
   bool have_key, have_value, have_meta, have_types;
   int rc;

   have_key = have_value = have_meta = have_types = false;

   rc = poliqarp_newdict_open(&this->key, base_name, false,
      POLIQARP_META_KEY_IMAGE_FORMAT, POLIQARP_META_KEY_OFFSET_FORMAT,
      _("Unable to open meta-key dictionary"), error);
   if (rc != 0)
      goto error;
   have_key = true;

   rc = poliqarp_newdict_open(&this->value, base_name, false,
      POLIQARP_META_VALUE_IMAGE_FORMAT, POLIQARP_META_VALUE_OFFSET_FORMAT,
      _("Unable to open meta-value dictionary"), error);
   if (rc != 0)
      goto error;
   have_value = true;

   path = string_aformat(POLIQARP_META_IMAGE_FORMAT, base_name);
   if (path == NULL)
      goto error_message;

   rc = tinydb_open(&this->meta, path,
      sizeof(struct poliqarp_binary_metadata));
   if (rc != 0) {
      poliqarp_error_from_system(error, _("Unable to open meta image (%s)"),
         path);
   }
   free(path);
   if (rc != 0)
      goto error;
   have_meta = true;

   path = string_aformat(POLIQARP_META_CONFIG_FORMAT, base_name);
   if (path == NULL)
      goto error_message;

   rc = read_type_list(&this->types, path);
   if (rc != 0) {
      poliqarp_error_from_system(error, _("Unable to open meta configuration"
         " (%s)"), path);
   }
   free(path);
   if (rc != 0)
      goto error;
   have_types = true;

   return 0;

error_message:
   poliqarp_error_from_system(error, _("Unable to initialize meta backend"));
error:
   if (have_key)
      newdict_close(&this->key);
   if (have_value)
      newdict_close(&this->value);
   if (have_meta)
      tinydb_close(&this->meta);
   if (have_types)
      free_type_list(this->types);
   return -1;
}

void poliqarp_backend_meta_close(struct poliqarp_backend_meta *this)
{
   newdict_close(&this->key);
   newdict_close(&this->value);
   tinydb_close(&this->meta);
   free_type_list(this->types);
   this->types = NULL;
}

enum poliqarp_meta_type poliqarp_get_metadata_type(
   const struct poliqarp_backend_meta *this, const char *key)
{
   struct poliqarp_meta_type_list *l;
   for (l = this->types; l; l = l->next) 
      if (strcmp(l->key, key) == 0) 
         return l->type;
   return POLIQARP_META_TYPE_UNKNOWN;
}
