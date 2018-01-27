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

#include <poliqarp-config.h>

#include <sakura/poliqarp.h>
#include <stdio.h>

#define CONFIG_CLEANUP_FLAG     1
#define INTERP_CLEANUP_FLAG     2
#define CORPUS_CLEANUP_FLAG     4
#define DOCUMENT_CLEANUP_FLAG 8
#define SUBDOCUMENT_CLEANUP_FLAG 16
#define META_CLEANUP_FLAG       32
#define ORTH_CLEANUP_FLAG       64
#define BASE_CLEANUP_FLAG       128
#define TAG_CLEANUP_FLAG        256
#define INDEX_CLEANUP_FLAG      512
#define SYNTAX_CLEANUP_FLAG     1024

#ifdef POLIQARP_SINGLE_THREADED

/* disable progress */
#define progress_reset(x) do {} while (0)
#define progress_advance(x, y) do {} while (0)
/* disable cancellation testing */
#define pthread_allow_cancel() do {} while (0)
#undef pthread_setcancelstate
#define pthread_setcancelstate(x, y) do {} while (0)
#undef pthread_cleanup_push
#define pthread_cleanup_push(x, y) do {} while (0)
#undef pthread_cleanup_pop
#define pthread_cleanup_pop(x) do {} while (0)

#else

#define pthread_allow_cancel() do { \
   pthread_setcancelstate(cancel_state, NULL); \
   pthread_testcancel(); \
   pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL); \
} while (0)

static void poliqarp_open_corpus_cleanup(void *data)
{
   struct poliqarp_corpus *corpus = data; 
   poliqarp_close_corpus(corpus);
}

#endif

int poliqarp_open_corpus(struct poliqarp_corpus *corpus, const char *name,
   progress_t *progress, struct poliqarp_error *error)
{
   int cancel_state;
   int rc = 0;
   pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cancel_state); 
   corpus->cleanup_flags = 0;
   corpus->base_name = strdup(name);
   if (corpus->base_name == NULL) {
      poliqarp_error_from_system(error, "Unable to open corpus");
      rc = -1;
   }
   progress_reset(progress);
   pthread_cleanup_push(poliqarp_open_corpus_cleanup, corpus);
   while (rc == 0) {
      /* config */
      rc = poliqarp_backend_config_open(&corpus->config, name, error);
      if (rc != 0)
         break;
      corpus->cleanup_flags |= CONFIG_CLEANUP_FLAG;
      progress_advance(progress, 9); /* 9 */
      pthread_allow_cancel();
      /* interp */
      rc = poliqarp_backend_interp_open(&corpus->interp, name, error);
      if (rc != 0)
         break;
      corpus->cleanup_flags |= INTERP_CLEANUP_FLAG;
      progress_advance(progress, 9); /* 18 */
      pthread_allow_cancel();
      /* corpus image */
      rc = poliqarp_backend_corpus_open(&corpus->corpus, name, error);
      if (rc != 0)
         break;
      corpus->cleanup_flags |= CORPUS_CLEANUP_FLAG;
      progress_advance(progress, 9); /* 27 */
      pthread_allow_cancel();
      /* corpus document */
      rc = poliqarp_backend_document_open(&corpus->document, name, error);
      if (rc != 0)
         break;
      corpus->cleanup_flags |= DOCUMENT_CLEANUP_FLAG;
      progress_advance(progress, 9); /* 36 */
      pthread_allow_cancel();
      /* subdocument */
      rc = poliqarp_backend_subdocument_open(&corpus->subdocument, name, error);
      if (rc != 0)
         break;
      corpus->cleanup_flags |= SUBDOCUMENT_CLEANUP_FLAG;
      progress_advance(progress, 9); /* 45 */
      pthread_allow_cancel();
      /* meta document */
      rc = poliqarp_backend_meta_open(&corpus->meta, name, error);
      if (rc != 0)
         break;
      corpus->cleanup_flags |= META_CLEANUP_FLAG;
      progress_advance(progress, 10); /* 55 */
      pthread_allow_cancel();
      /* orth */
      rc = poliqarp_backend_orth_open(&corpus->orth, name, error);
      if (rc != 0)
         break;
      corpus->cleanup_flags |= ORTH_CLEANUP_FLAG;
      progress_advance(progress, 9); /* 64 */
      pthread_allow_cancel();
      /* base */
      rc = poliqarp_backend_base_open(&corpus->base, name, error);
      if (rc != 0)
         break;
      corpus->cleanup_flags |= BASE_CLEANUP_FLAG;
      progress_advance(progress, 9); /* 73 */
      pthread_allow_cancel();
      /* tag */
      rc = poliqarp_backend_tag_open(&corpus->tag, &corpus->config, name, error);
      if (rc != 0)
         break;
      corpus->cleanup_flags |= TAG_CLEANUP_FLAG;
      progress_advance(progress, 9); /* 82 */
      pthread_allow_cancel();
      /* index */
      rc = poliqarp_backend_index_open(&corpus->index, &corpus->config, name, error);
      if (rc != 0)
         break;
      corpus->cleanup_flags |= INDEX_CLEANUP_FLAG;
      progress_advance(progress, 9); /* 91 */
      pthread_allow_cancel();
      /* syntax */
      rc = poliqarp_backend_syntax_open(&corpus->syntax, name, error);
      if (rc != 0)
         break;
      corpus->cleanup_flags |= SYNTAX_CLEANUP_FLAG;
      progress_advance(progress, 9); /* 100 */
      break;
   };
   if (rc != 0)
      poliqarp_close_corpus(corpus);
   pthread_cleanup_pop(0);
   pthread_setcancelstate(cancel_state, NULL); 
   return rc;
}

int poliqarp_close_corpus(struct poliqarp_corpus *corpus)
{
   assert(corpus != NULL);

   if (corpus->cleanup_flags & INDEX_CLEANUP_FLAG)
      poliqarp_backend_index_close(&corpus->index);

   if (corpus->cleanup_flags & INTERP_CLEANUP_FLAG)
      poliqarp_backend_interp_close(&corpus->interp);

   if (corpus->cleanup_flags & CORPUS_CLEANUP_FLAG)
      poliqarp_backend_corpus_close(&corpus->corpus);

   if (corpus->cleanup_flags & DOCUMENT_CLEANUP_FLAG)
      poliqarp_backend_document_close(&corpus->document);

   if (corpus->cleanup_flags & SUBDOCUMENT_CLEANUP_FLAG)
      poliqarp_backend_subdocument_close(&corpus->subdocument);

   if (corpus->cleanup_flags & META_CLEANUP_FLAG)
      poliqarp_backend_meta_close(&corpus->meta);

   if (corpus->cleanup_flags & ORTH_CLEANUP_FLAG)
      poliqarp_backend_orth_close(&corpus->orth);

   if (corpus->cleanup_flags & BASE_CLEANUP_FLAG)
      poliqarp_backend_base_close(&corpus->base);

   if (corpus->cleanup_flags & CONFIG_CLEANUP_FLAG)
      poliqarp_backend_config_close(&corpus->config);

   if (corpus->cleanup_flags & TAG_CLEANUP_FLAG)
      poliqarp_backend_tag_close(&corpus->tag);

   if (corpus->cleanup_flags & SYNTAX_CLEANUP_FLAG)
      poliqarp_backend_syntax_close(&corpus->syntax);

   corpus->cleanup_flags = 0;
   free(corpus->base_name);
   corpus->base_name = NULL;

   return 0;
}
