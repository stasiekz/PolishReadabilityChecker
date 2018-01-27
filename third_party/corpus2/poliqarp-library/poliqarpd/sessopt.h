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

#ifndef POLIQARPD_SESSOPT_H
#define POLIQARPD_SESSOPT_H

#include <poliqarp-config.h>

#include <foostring/foostring.h>
#include "errors.h"

/** 
 * Structure representing width of a context (which can be given either in 
 * segments or characters).
 * FIXME: shouldn't this be moved to libpoliqarp.h?
 */
struct context_width {
   size_t width;  /**< how wide this segment is. */
   bool charspec; /**< if this is true, the `width' field specifies the width
                       in characters, else in segments. */
};

/**
 * Structure representing options that can be set for a session.
 */
struct sessopt {
   struct context_width left_context_width;
   struct context_width right_context_width;
   struct context_width wide_context_width;

   /** This is an integer that specifies what should be displayed in various 
    * parts of the query result. It consists of several bitfields
    * that can be set or not. */
   int retrieve;

   /** This value specifies the intervals between result portions. */
   size_t interval;

   /** Specifies whether to retrieve disambiguated interpretations. */
   bool disamb;

   /** Specifies the default flags for various parts of the query. This is a
    * bitwise OR of QFLAG_*. */
   int qflags;

   /** Specifies the name for query rewriting rules.
    */
   string_t rewrite;

   /** Specifies whether a random sample is requested.
    */
   bool random_sample;
};

/** 
 * Bitmasks for use with `retrieve' field of struct sessopt. 
 */
#define SHOW_LEMMATA_LC 0x0001
#define SHOW_LEMMATA_LM 0x0002
#define SHOW_LEMMATA_RM 0x0004
#define SHOW_LEMMATA_RC 0x0008
#define SHOW_TAGS_LC 0x0010
#define SHOW_TAGS_LM 0x0020
#define SHOW_TAGS_RM 0x0040
#define SHOW_TAGS_RC 0x0080
#define SHOW_IDS_LC 0x0100
#define SHOW_IDS_LM 0x0200
#define SHOW_IDS_RM 0x0400
#define SHOW_IDS_RC 0x0800

/**
 * Initializes the given options structure with default values.
 */
void sessopt_init(struct sessopt *options);

/**
 * Frees the given options structure.
 */
void sessopt_free(struct sessopt *options);

/**
 * Set the value of an option.
 * @return 0 upon successful setting.
 * @return PE_INVOPT if the option name was incorrect.
 * @return PE_INVVAL if the option value was incorrect.
 */
int sessopt_set(struct sessopt *options, const char *name, const char *value);

#endif /* POLIQARPD_SESSION_H */
