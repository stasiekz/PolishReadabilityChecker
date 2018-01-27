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

#ifndef POLIQARP_BACKEND_INDEX_H
#define POLIQARP_BACKEND_INDEX_H

#include <sakura/backend-config.h>

#include <sakura/common/bitstream.h>
#include <sakura/common/file-map.h>

#define POLIQARP_CORPUS_ORTH_INDEX_FORMAT          "%s.poliqarp.rindex.orth"
#define POLIQARP_CORPUS_DISAMB_INDEX_FORMAT        "%s.poliqarp.rindex.disamb"
#define POLIQARP_CORPUS_AMB_INDEX_FORMAT           "%s.poliqarp.rindex.amb"

#define POLIQARP_INDEX_ORTH            1  /**< Index of orths. */
#define POLIQARP_INDEX_DISAMB          2  /**< Index of disambiguated interpretations. */
#define POLIQARP_INDEX_AMB             4  /**< Index of ambiguous interpretations. */

#define POLIQARP_INDEX_DEFAULT_GRANULARITY 1024
#define POLIQARP_INDEX_MIN_GRANULARITY 100
#define POLIQARP_INDEX_MAX_GRANULARITY 1000000

struct poliqarp_ibs_mem_backend {
   uint8_t *data;
   size_t num_bit;
};

struct poliqarp_rindex {
   struct file_map image;
   struct file_map offset;
   struct ibs ibs;
   struct poliqarp_ibs_mem_backend backend;
   size_t num_lists;
   size_t granularity;
};

struct poliqarp_backend_index {
   struct poliqarp_rindex *orth_index;
   struct poliqarp_rindex *disamb_index;
   struct poliqarp_rindex *amb_index;
};

int poliqarp_index_calculate(const char *desc);

char *poliqarp_index_stringify(int indices);

int poliqarp_backend_index_open(struct poliqarp_backend_index *this,
   const struct poliqarp_backend_config *config, const char *base_name,
   struct poliqarp_error *error);

void poliqarp_backend_index_close(struct poliqarp_backend_index *this);

void poliqarp_rindex_set(struct poliqarp_rindex *this, size_t which);

#endif /* POLIQARP_BACKEND_INDEX_H */
