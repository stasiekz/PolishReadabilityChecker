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

#ifndef POLIQARP_DATE_SPAN_H
#define POLIQARP_DATE_SPAN_H

#include <stdlib.h>
#include <sakura/abi.h>

/** @todo */
struct poliqarp_date_span {
   struct poliqarp_date_span *next;
   struct poliqarp_meta_date start;
   struct poliqarp_meta_date end;
};

/** @todo */
struct poliqarp_date_span *poliqarp_date_span_create(struct poliqarp_meta_date date);

/** @todo */
void poliqarp_date_span_destroy(struct poliqarp_date_span *this);

#endif
