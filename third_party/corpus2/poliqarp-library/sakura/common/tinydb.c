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

#include <sakura/common/tinydb.h>

int tinydb_open(struct tinydb *this, const char *image_pathname, 
   size_t item_size)
{
   int code;
   code = create_file_map(&this->file_map, image_pathname);
   if (code != 0)
      return code;
   this->num_bytes = get_file_size(&this->file_map);
   if (this->num_bytes % item_size != 0) {
      destroy_file_map(&this->file_map);
      errno = EINVAL;
      return -1;
   }
   this->num_items = this->num_bytes / item_size;
   this->item_size = item_size;
   this->image = get_file_image(&this->file_map);
   return 0;
}

void tinydb_close(struct tinydb *this)
{
   destroy_file_map(&this->file_map);
}
