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

#ifndef POLIQARP_EXCEPTION_H
#define POLIQARP_EXCEPTION_H

#include <poliqarp-config.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

struct poliqarp_error {
   char *message;
   bool malloced;
};

static const struct poliqarp_error poliqarp_error_none = {
   NULL, false
};

const char *poliqarp_error_message_get(const struct poliqarp_error *);
void poliqarp_error_set(struct poliqarp_error *, struct poliqarp_error *);
void poliqarp_error_message_set(struct poliqarp_error *, const char *, ...);
void poliqarp_error_from_system_n(int, struct poliqarp_error *,
   const char *, ...);

#define poliqarp_error_from_system(...) \
   do { \
      int __errnum = errno; \
      poliqarp_error_from_system_n(__errnum, __VA_ARGS__); \
   } while (0)

#endif
