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

#ifndef UNIBITS_UNIBITS_EXTRA_H
#define UNIBITS_UNIBITS_EXTRA_H

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ckalloc malloc
#define ckfree free
#define ckrealloc realloc

#define EXTERN extern
#define CONST const
#define VOID void
#define INLINE inline
#define UCHAR(c) ((unsigned char) c)

#define TCL_UTF_MAX 4
typedef unsigned int Tcl_UniChar;

extern const unsigned char Tcl_Utf8TotalBytes[256];

#endif
