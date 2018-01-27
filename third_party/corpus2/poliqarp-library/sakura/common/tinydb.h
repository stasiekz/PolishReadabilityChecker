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

#ifndef TINYDB_H
#define TINYDB_H

#include <poliqarp-config.h>

#include <stdlib.h>
#include <assert.h>

#include <sakura/common/file-map.h>

struct tinydb {
   struct file_map file_map;
   const void *image;
   size_t item_size;
   size_t num_items;
   size_t num_bytes;
};

int tinydb_open(struct tinydb *this, const char *image_pathname, 
   size_t item_size);

void tinydb_close(struct tinydb *this);

static inline const void *tinydb_fetch_item(const struct tinydb *this, 
   size_t index)
{
   assert(index < this->num_items);
   return ((char *)this->image) + this->item_size * index;
}

static inline const void *tinydb_fetch_image(const struct tinydb *this)
{
   return this->image;
}

#endif /* TINYDB_H */
