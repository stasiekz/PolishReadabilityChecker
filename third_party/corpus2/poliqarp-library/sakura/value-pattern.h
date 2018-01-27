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

#ifndef POLIQARP_VALUE_PATTERN_H
#define POLIQARP_VALUE_PATTERN_H

#include <sakura/corpus.h>
#include <sakura/exception.h>
#include <sakura/regexp.h>
#include <sakura/value.h>

#include <sakura/common/newdict.h>

/** @addtogroup poliqarp_value */
/** @{ */
/** @file value-pattern.h Create a value that matches any pattern */

/** 
 * Create a value that matches the pattern in the given dictionary. 
 * @param dict The dictionary.
 * @param regexp The pattern.
 */
struct poliqarp_value *poliqarp_value_pattern_create(const struct newdict *dict,
   const struct poliqarp_regexp *regexp);

/** @} */

#endif
