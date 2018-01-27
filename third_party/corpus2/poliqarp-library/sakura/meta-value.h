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

#ifndef POLIQARP_META_VALUE_H
#define POLIQARP_META_VALUE_H

#include <stdlib.h>
#include <sakura/corpus.h>
#include <sakura/abi.h>
#include <sakura/value.h>
#include <sakura/regexp.h>
#include <sakura/date-span.h>

/** @todo */
enum poliqarp_meta_match_domain {
   POLIQARP_DOMAIN_DATE,
   POLIQARP_DOMAIN_TEXT,
   POLIQARP_DOMAIN_UNDEFINED
};

/** @todo */
enum poliqarp_meta_match_strategy {
   POLIQARP_STRATEGY_EQUAL,
   POLIQARP_STRATEGY_GREATER,
   POLIQARP_STRATEGY_GREATER_EQUAL,
   POLIQARP_STRATEGY_SMALLER,
   POLIQARP_STRATEGY_SMALLER_EQUAL
};

/** @todo */
struct poliqarp_meta_value {
   enum poliqarp_meta_match_domain domain;     /**< operator domain.         */
   enum poliqarp_meta_match_strategy strategy; /**< operator match strategy. */
   size_t key;
   struct {
      struct poliqarp_regexp value_pattern;
      struct poliqarp_meta_date date;
      struct poliqarp_date_span date_span;
   } value_as;
};

/** @todo */
struct poliqarp_meta_value *poliqarp_meta_value_create_date(
   const struct poliqarp_corpus *corpus, const char *key_text, 
   struct poliqarp_meta_date date, enum poliqarp_meta_match_strategy strategy);

/** @todo */
struct poliqarp_meta_value *poliqarp_meta_value_create_undefined(
   const struct poliqarp_corpus *corpus, const char *key_text);

/** @todo */
struct poliqarp_meta_value *poliqarp_meta_value_create_text(
   const struct poliqarp_corpus *corpus, const char *key_text, 
   const struct poliqarp_regexp *pattern);

/** @todo */
void poliqarp_meta_value_destroy(struct poliqarp_meta_value *this);

/** @todo */
bool poliqarp_meta_value_can_be_optimized(const struct poliqarp_meta_value *this,
   const struct poliqarp_meta_value *that, enum poliqarp_logic_operator oper);

/** @todo */
struct poliqarp_value *poliqarp_meta_value_optimize(
   const struct poliqarp_meta_value *this, const struct poliqarp_meta_value *that,
   enum poliqarp_logic_operator oper);

/** @todo */
int poliqarp_meta_value_compare(const struct poliqarp_meta_value *this,
   const struct poliqarp_meta_value *that);

/** @todo */
bool poliqarp_meta_value_eval(const struct poliqarp_meta_value *this,
   const struct poliqarp_corpus *corpus, const struct poliqarp_document *document);

/** @todo */
void poliqarp_meta_value_modify(struct poliqarp_meta_value *this,
   enum poliqarp_meta_match_strategy strategy);

int poliqarp_meta_date_parse(const char *text, struct poliqarp_meta_date *date);


#endif
