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

#ifndef POLIQARP_BACKEND_DOCUMENT_H
#define POLIQARP_BACKEND_DOCUMENT_H

#include <sakura/common/tinydb.h>
#include <sakura/exception.h>
#include <sakura/abi.h>

#define POLIQARP_CORPUS_DOCUMENT_FORMAT "%s.poliqarp.chunk.image"

struct poliqarp_backend_document {
   struct tinydb document;
   uint32_t current;
};

int poliqarp_backend_document_open(struct poliqarp_backend_document *this, 
   const char *basename, struct poliqarp_error *error);
void poliqarp_backend_document_close(struct poliqarp_backend_document *this);
int poliqarp_backend_document_fetch(const struct poliqarp_backend_document *this,
   size_t n, struct poliqarp_document *document);
int poliqarp_backend_document_next(struct poliqarp_backend_document *this,
   struct poliqarp_document *document);
void poliqarp_backend_document_search(struct poliqarp_backend_document *this,
   uint32_t pos);

#endif /* POLIQARP_BACKEND_DOCUMENT_H */
