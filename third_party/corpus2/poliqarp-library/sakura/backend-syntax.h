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

#ifndef POLIQARP_BACKEND_SYNTAX_H
#define POLIQARP_BACKEND_SYNTAX_H

#include <poliqarp-config.h>

#include <sakura/common/bitstream.h>
#include <sakura/common/newdict.h>
#include <sakura/common/tinydb.h>
#include <sakura/common/file-map.h>
#include <sakura/exception.h>

struct poliqarp_syntax_group {
   uint32_t type;
   uint32_t from, to;
   union {
      struct {
         uint32_t length;
      } coord;
      struct {
         uint32_t synh;
         uint32_t semh;
      } noncoord;
   } u;
};

struct poliqarp_backend_syntax {
   bool syntax;               /**< Whether we have syntax backend at all. */
   struct ibs bitstream;      /**< Bitstream for input syntax file. */
   struct newdict dict_types; /**< Dictionary of group types. */
   struct file_map data;      /**< Memory view of the syntax image file. */
   struct tinydb offsets;     /**< Array of offsets providing random access. */
   struct poliqarp_syntax_group *groups; /**< Array of decoded groups. */
   uint32_t type;
   uint32_t lastfrom;         /**< The 'from' attribute of last read group. */
   size_t numbits;            /**< Total number of bits in the syntax image. */
   size_t pos;                /**< Offset of the bit we're currently on. */
   size_t size;               /**< Number of groups in the `groups' array. */
   size_t start;              /**< Where the decoded groups start. */
   size_t end;                /**< Where the decoded groups end. */
}; 

#define POLIQARP_SYNTAX_GROUP_UNKNOWN     ((uint32_t)(-1))
#define POLIQARP_SYNTAX_GROUP_COORD       0
#define POLIQARP_SYNTAX_GROUP_CONJUNCTION 1

int poliqarp_backend_syntax_open(struct poliqarp_backend_syntax *this,
   const char *base_name, struct poliqarp_error *error);
void poliqarp_backend_syntax_close(struct poliqarp_backend_syntax *this);
int poliqarp_backend_syntax_next(struct poliqarp_backend_syntax *this);
void poliqarp_backend_syntax_reset(struct poliqarp_backend_syntax *this);

#endif /* POLIQARP_BACKEND_SYNTAX_H */
