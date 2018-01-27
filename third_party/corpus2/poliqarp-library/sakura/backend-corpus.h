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

/**
 * @file backend-corpus.h
 * @brief The corpus backend -- loading and manipulations.
 *
 * This backend handles the corpus proper (*.poliqarp.corpus.image).
 * The backend is currently implemented as a file-reader of fixed-size 
 * structures of type struct binary_corpus and provides a random-access
 * API.
 * 
 * TODO: The format will change in the near future to support Huffman-encoded
 * corpus images; the random-access API will be retained, but a notion of
 * an `access context' will be introduced, one context for searching thread
 * and one for result retrieval (so as not to mix cache buffers).
 */

#ifndef POLIQARP_BACKEND_CORPUS_H
#define POLIQARP_BACKEND_CORPUS_H

#include <sakura/common/file-reader.h>
#include <sakura/exception.h>
#include <sakura/abi.h>

/* This is here because indexer needs it. */
#define POLIQARP_CORPUS_IMAGE_FORMAT "%s.poliqarp.corpus.image"

struct poliqarp_backend_corpus {
   struct file_reader corpus;
};

int poliqarp_backend_corpus_open(struct poliqarp_backend_corpus *this,
   const char *base_name, struct poliqarp_error *error);

void poliqarp_backend_corpus_close(struct poliqarp_backend_corpus *this);

static inline struct poliqarp_binary_segment poliqarp_backend_corpus_get(
   struct poliqarp_backend_corpus *this, size_t index)
{
   struct poliqarp_binary_segment res = *(
      (struct poliqarp_binary_segment *)file_reader_get(&this->corpus, index));
   POLIQARP_SEGMENT_LE_TO_HE(res);
   return res;
}

static inline uint32_t poliqarp_backend_corpus_size(
   const struct poliqarp_backend_corpus *this)
{
   return this->corpus.num_items;
}

static inline void poliqarp_backend_corpus_seek(struct poliqarp_backend_corpus *this,
   size_t index)
{
   file_reader_seek(&this->corpus, index);
}

static inline struct poliqarp_binary_segment poliqarp_backend_corpus_next(
   struct poliqarp_backend_corpus *this)
{
   struct poliqarp_binary_segment res = *(
      (struct poliqarp_binary_segment *)file_reader_next(&this->corpus));
   POLIQARP_SEGMENT_LE_TO_HE(res);
   return res;
}

#endif /* POLIQARP_BACKEND_CORPUS_H */
