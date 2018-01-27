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

#ifndef POLIQARP_BACKEND_SUBDOCUMENT_H
#define POLIQARP_BACKEND_SUBDOCUMENT_H

#include <sakura/common/file-map.h>
#include <sakura/common/newdict.h>
#include <sakura/exception.h>
#include <sakura/abi.h>

#define POLIQARP_SUBDOCUMENT_ITEM_FORMAT "%s.poliqarp.subchunk.item.%s"
#define POLIQARP_SUBDOCUMENT_IMAGE_FORMAT  "%s.poliqarp.subchunk.image"
#define POLIQARP_SUBDOCUMENT_OFFSET_FORMAT "%s.poliqarp.subchunk.offset"

struct poliqarp_subdocument_set {
   struct file_map map;
   uint32_t current;
};

struct poliqarp_backend_subdocument {
   struct newdict name_dict;
   struct poliqarp_subdocument_set *sets;
};

int poliqarp_subdocument_next(struct poliqarp_subdocument_set *set,
   struct poliqarp_subdocument *subdocument);
void poliqarp_subdocument_search(struct poliqarp_subdocument_set *set,
   uint32_t pos);
int poliqarp_backend_subdocument_open(struct poliqarp_backend_subdocument *this,
   const char *base_name, struct poliqarp_error *error);
void poliqarp_backend_subdocument_close(struct poliqarp_backend_subdocument *this);
struct poliqarp_subdocument_set *poliqarp_backend_subdocument_lookup_set(
   struct poliqarp_backend_subdocument *this, const char *set_name);

#endif /* POLIQARP_BACKEND_SUBDOCUMENT_H */
