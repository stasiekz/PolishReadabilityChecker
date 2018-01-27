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

/** @defgroup poliqarp_corpus Corpus Module */
/** @{ */
/** @file corpus.h Corpus Module */

#ifndef POLIQARP_CORPUS_H
#define POLIQARP_CORPUS_H

#include <progress/progress.h>
#include <sakura/backend-corpus.h>
#include <sakura/backend-document.h>
#include <sakura/backend-subdocument.h>
#include <sakura/backend-meta.h>
#include <sakura/backend-orth.h>
#include <sakura/backend-base.h>
#include <sakura/backend-tag.h>
#include <sakura/backend-interp.h>
#include <sakura/backend-config.h>
#include <sakura/backend-index.h>
#include <sakura/backend-syntax.h>

#include <sakura/common/hash-table.h>

/** 
 * Corpus structure.
 * Holds all backends together
 */
struct poliqarp_corpus {
   struct poliqarp_backend_document document;       /**< Document backend.           */
   struct poliqarp_backend_subdocument subdocument; /**< Subdocument backend.        */
   struct poliqarp_backend_meta meta;         /**< Meta backend.            */
   struct poliqarp_backend_corpus corpus;     /**< Corpus backend.          */
   struct poliqarp_backend_orth orth;         /**< Orth backend.            */
   struct poliqarp_backend_base base;         /**< Base backend.            */
   struct poliqarp_backend_config config;     /**< Configuration backend.   */
   struct poliqarp_backend_tag tag;           /**< Tag backend.             */
   struct poliqarp_backend_interp interp;     /**< Interpretation backend.  */
   struct poliqarp_backend_index index;       /**< Index backend.           */
   struct poliqarp_backend_syntax syntax;     /**< Syntax backend.          */
   char *base_name;                           /**< Copy of the base name.   */
   unsigned cleanup_flags; /**< Cleanup flags. Each backend gets its own flag
                                here if it opened correctly. */
};

#define poliqarp_get_backend(corpus, type) (&(corpus)->type)
#define poliqarp_get_const_backend(corpus, type) \
   ((const struct poliqarp_backend_##type *) poliqarp_get_backend(corpus, type))

#endif                          /* POLIQARP_CORPUS_H */

/** @} */
