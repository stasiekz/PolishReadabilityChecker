/*
 * This file is part of the Poliqarp suite.
 * 
 * Copyright (C) 2008-2009 by Instytut Podstaw Informatyki Polskiej
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


#ifndef POLIQARP_QUERY_REWRITE_H
#define POLIQARP_QUERY_REWRITE_H

#include <sakura/common/hash-table.h>

struct poliqarp_query_rewrite_table {
   struct hash_table hash_table;
};

struct poliqarp_query_rewrite;

struct poliqarp_query_rewrite_rule;

int poliqarp_create_query_rewrite_table(struct poliqarp_query_rewrite_table *);

void poliqarp_destroy_query_rewrite_table(
  struct poliqarp_query_rewrite_table *);

struct poliqarp_query_rewrite *poliqarp_get_query_rewrite(
   struct poliqarp_query_rewrite_table *, const char *name, bool create);

int poliqarp_add_query_rewrite_rule(struct poliqarp_query_rewrite *,
   const char *pattern, const char *replacement);

char *poliqarp_rewrite_query(struct poliqarp_query_rewrite *this,
   const char *query, unsigned int flags, unsigned int xflags);

#endif
