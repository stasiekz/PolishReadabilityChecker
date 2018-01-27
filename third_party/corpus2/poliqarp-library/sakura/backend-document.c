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

#include <stdlib.h>

#include <foostring/foostring.h>
#include <sakura/backend-document.h>
#include <sakura/exception.h>


int poliqarp_backend_document_open(struct poliqarp_backend_document *this, 
   const char *basename, struct poliqarp_error *error)
{
   int rc;
   char *path = string_aformat(POLIQARP_CORPUS_DOCUMENT_FORMAT, basename);
   if (path == NULL) {
      poliqarp_error_from_system(error, _("Unable to open corpus document"));
      return -1;
   }
   rc = tinydb_open(&this->document, path, sizeof(struct poliqarp_document));
   if (rc != 0) {
      poliqarp_error_from_system(error, _("Unable to open corpus document (%s)"), path);
   }
   free(path);
   if (rc != 0)
      return rc;
   this->current = 0;
   return 0;
}

void poliqarp_backend_document_close(struct poliqarp_backend_document *this)
{
   tinydb_close(&this->document);
}

int poliqarp_backend_document_next(struct poliqarp_backend_document *this,
   struct poliqarp_document *document)
{
   int res = poliqarp_backend_document_fetch(this, this->current, document);
   this->current++;
   return res;
}

int poliqarp_backend_document_fetch(const struct poliqarp_backend_document *this,
   size_t n, struct poliqarp_document *document)
{
   if (n >= this->document.num_items)
      return -1;

   *document = *(struct poliqarp_document *)tinydb_fetch_item(&this->document, n);
   /* endian-neutralize */
   document->corpus_low = en4(document->corpus_low);
   document->corpus_high = en4(document->corpus_high);
   document->meta_low = en4(document->meta_low);
   document->meta_high = en4(document->meta_high);  
   return 0;
}

/* FIXME: This is code duplication (see poliqarp_subdocument_search), to be fixed
 * someday... */
void poliqarp_backend_document_search(struct poliqarp_backend_document *this,
   uint32_t pos)
{
   struct poliqarp_document *arr = (struct poliqarp_document *)this->document.image;
   size_t l = this->current, r = this->document.num_items - 1, m;
   while (l < r) {
      m = (l + r) / 2;
      if (pos >= en4(arr[m].corpus_low) && pos < en4(arr[m].corpus_high)) {
         this->current = m;
         return;
      } else if (pos < en4(arr[m].corpus_low)) {
         r = m - 1;
      } else {
         l = m + 1;
      }
   }
   if (pos >= en4(arr[r].corpus_high))
      this->current = r + 1;
   else
      this->current = r;
}

