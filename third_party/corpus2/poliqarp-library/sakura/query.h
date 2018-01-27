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

#ifndef POLIQARP_QUERY_H
#define POLIQARP_QUERY_H

#include <pthread.h>

#include <time.h>

#include <sakura/poliqarp.h>
#include <sakura/corpus.h>
#include <sakura/expression.h>
#include <sakura/meta-value.h>

#include <sakura/common/bs.h>
#include <sakura/common/graph.h>
#include <sakura/common/hash-table.h>

/** @defgroup poliqarp_query Query module */
/** @{ */
/** @file query.h Query things */

/* TODO: Add value-related-headers */

struct poliqarp_query;
struct poliqarp_match_buffer;
struct poliqarp_match;

struct poliqarp_searchable_area {
   size_t num_bits;
   size_t num_segments;
   size_t granularity;
   bitset area;
   bitset_arena arena;
};

#define POLIQARP_QEFLAG_HAS_ALIGN 1
#define POLIQARP_QEFLAG_HAS_VARIABLES 2

struct poliqarp_within {
   enum poliqarp_within_type {
      WITHIN_SUBDOCUMENT,
      WITHIN_PHRASE
   } type;
   union {
      struct poliqarp_subdocument_set *subdocument;
      struct poliqarp_expression *phrase;
   } as;
};

struct poliqarp_within *poliqarp_within_create_subdocument(
   struct poliqarp_subdocument_set *set);
struct poliqarp_within *poliqarp_within_create_phrase(
   struct poliqarp_expression *phrase);

struct poliqarp_search_context {
   const struct dfs_node *node;
   struct dfs_link *link;
   size_t index;
   size_t phrase;
   size_t within_phrase;
   size_t m_start;
   size_t m_end;
   size_t m_focus;
   size_t c_focus;
   struct poliqarp_document document;
   struct poliqarp_subdocument subdocument;
};


/**
 * User makes a query, here it is.
 */
struct poliqarp_query {
   struct poliqarp_corpus *corpus;          /**< Corpus.                  */
   char *query_text;                        /**< Original query text.     */
   struct graph_env graph;                  /**< Match graph.             */
   struct poliqarp_expression *meta_expression; /**< Meta expression.         */
   struct poliqarp_search_context last_context; /**< Last search context.     */
   bool have_last_context;                  /**< Whether we have executed
                                                 this query before.       */
   struct poliqarp_within *within;          /**< Within object.           */
   const struct hash_table *aliases;        /**< Hash table with aliases. */
   struct poliqarp_searchable_area area;        /**< Area of search.          */
   int progress;                            /**< Progress of the query at
                                                 the start of searching.  */
   size_t max_segment;                      /**< Offset of the rightmost
                                                 segment we have seen.    */
   int flags;                               /**< Flags of query.          */
   int eflags;                              /**< Extended (internal) flags
                                                 of query.                */
   size_t variable_ranges[POLIQARP_MAX_VARIABLES]; /**< Ranges of variables. */
   void *variable_types[POLIQARP_MAX_VARIABLES];   /**< Types of variables. */
   size_t progress_helper;                  /**< Field used to keep track
                                                 of progress.             */
   struct poliqarp_query_rewrite *rewrite;  /**< Rules of query rewriting. */
   bool rewrite_in_progress;                /**< Query rewrite in progress? */
   struct nfs_graph rewrite_graph;          /**< Temporary graph for query rewriting. */
   struct poliqarp_error *error;            /**< Compile error. */
   struct poliqarp_random_state *random_state; /**< State of a pseudo-random number generator. */
};

/**
 * Set constraining meta-data expression.
 * @param this Query object.
 * @param exp Expression object.
 */
void poliqarp_query_set_meta_expression(struct poliqarp_query *this,
   struct poliqarp_expression *exp);

/**
 * Set within constraint.
 * @param this Query object.
 * @param within Within constraint.
 **/
void poliqarp_query_set_within(struct poliqarp_query *this, 
   struct poliqarp_within *within);

/** Query result structure. */
struct poliqarp_match_buffer {
   struct poliqarp_corpus *corpus;  /**< Corpus that the matches are valid 
                                         on.                            */
   struct poliqarp_match *match;    /**< Array of matches.              */
   size_t capacity;                 /**< Capacity of the match array.   */
   size_t used;                     /**< Number of used items in the 
                                         match array.                   */
   size_t num_results;
   /**< Number of results spotted during query execution. */
   struct marena *sort_arena;       /**< Sort scratchpad memory source. */
   pthread_mutex_t mutex;
};

/**
 * Append match to result buffer.
 * It should be EXPLICITLY verified that enough space exists.
 * @param this Result object.
 * @param match Match object.
 */
void poliqarp_append_query_result(struct poliqarp_match_buffer *this, 
   const struct poliqarp_match *match);

void poliqarp_create_search_area(struct poliqarp_query *this);

/** @} */

#endif
