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

#ifndef POLIQARP_COMMONDEF_H
#define POLIQARP_COMMONDEF_H

#ifdef HAVE_INTTYPES_H
#  include <inttypes.h>
#endif

#ifdef HAVE_GETTEXT
#  include <libintl.h>
#  define _(x) gettext(x)
#else
#  define _(x) (x)
#  define gettext(x) (x)
#  define ngettext(x,y,n) (((n) == 1) ? (x) : (y))
#endif
#define __(x,y,n) ngettext(x,y,n)
#define _M(x) (x) /* xgettext --keyword=_M ... */

#ifdef HAVE_STDBOOL_H
#  include <stdbool.h>
#else
#  undef bool
#  define bool unsigned char
#  define true 1
#  define false 0
#endif

#ifndef __GNUC__
#  define __attribute__(x)
#endif

#include <limits.h>
#ifndef PATH_MAX
#  define PATH_MAX 4096
#endif

#if !HAVE_FSEEKO && HAVE_FSEEKO64
#  undef fseeko
#  define fseeko fseeko64
#  undef ftello
#  define ftello ftello64
#endif

#ifdef WORDS_BIGENDIAN

static inline uint16_t en2(uint16_t x)
{
   return ((x & 0xFF) << 8) + (x >> 8);
}

static inline uint32_t en4(uint32_t x) 
{
   return ((x & 0xFF) << 24) +
          (((x >> 8) & 0xFF) << 16) +
          (((x >> 16) & 0xFF) << 8) +
          ((x >> 24) & 0xFF);
}

static inline uint64_t en8(uint64_t x)
{
   return ((x & 0xFF) << 56) +
          (((x >> 8) & 0xFF) << 48) +
          (((x >> 16) & 0xFF) << 40) +
          (((x >> 24) & 0xFF) << 32) +
          (((x >> 32) & 0xFF) << 24) +
          (((x >> 40) & 0xFF) << 16) +
          (((x >> 48) & 0xFF) << 8) +
          ((x >> 56) & 0xFF);
}

#else

static inline uint16_t en2(uint16_t x) { return x; }
static inline uint32_t en4(uint32_t x) { return x; }
static inline uint64_t en8(uint64_t x) { return x; }

#endif

#endif /* POLIQARP_COMMONDEF_H */
