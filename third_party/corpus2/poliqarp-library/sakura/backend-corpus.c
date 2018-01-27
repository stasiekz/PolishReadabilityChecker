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

#include <sakura/abi.h>
#include <sakura/backend-corpus.h>
#include <sakura/exception.h>
#include <sakura/common/file-reader.h>

int poliqarp_backend_corpus_open(struct poliqarp_backend_corpus *this,
   const char *base_name, struct poliqarp_error *error)
{
   int rc;
   char *path = string_aformat(POLIQARP_CORPUS_IMAGE_FORMAT, base_name);
   if (path == NULL) {
      poliqarp_error_from_system(error, _("Unable to open corpus image"));
      return -1;
   }
   rc = file_reader_create(&this->corpus, path,
      sizeof(struct poliqarp_binary_segment));
   free(path);
   if (rc != 0) {
      poliqarp_error_from_system(error, _("Unable to open corpus image (%s)"),
         path);
      return -1;
   }
   return 0;
}

void poliqarp_backend_corpus_close(struct poliqarp_backend_corpus *this)
{
   file_reader_destroy(&this->corpus);
}
