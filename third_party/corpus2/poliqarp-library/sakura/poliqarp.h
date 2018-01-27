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
 * @file poliqarp.h
 * @brief Public API of libpoliqarp.
 *
 * This file defines a set of low-level corpus structures and functions.
 * Any library that implements them can be linked with the Poliqarp daemon and
 * used with Poliqarp clients. The default implementation is codenamed
 * `sakura'.
 */

#ifndef POLIQARP_H
#define POLIQARP_H

#include <poliqarp-config.h>

#include <stdlib.h>
#include <progress/progress.h>

/** Version number and name of the library. */
extern const int poliqarp_major_version;   
extern const int poliqarp_minor_version;
extern const int poliqarp_revision_number;
extern const char poliqarp_library_name[];

/* Declaration of private structures. */

/**
 * Logically, a corpus is just a sequence of segments. However, this file does
 * not tell how it is precisely defined -- that is the job of 
 * poliqarp-private.h, included below. Instead, it declares functions that
 * return a particular segment in the corpus, or number of segments contained
 * therein, etc.
 */
struct poliqarp_corpus;

/**
 * A parsed query can have just about any internal structure.
 */
struct poliqarp_query;


/**
 * An error message wrapper.
 */
struct poliqarp_error;

/**
 * A match buffer is an array holding results of a query. It generally
 * has a fixed size, but can be enlarged or shrunk as needed. 
 */
struct poliqarp_match_buffer;

/**
 * A segment is a triple <O, D, A>, where O is a string (denoting orthographic
 * form of the segment -- the form as it appears in the text) and D and A are
 * sets of interpretations (see below), corresponding to disambiguated and
 * ambiguous interpretations, respectively. 
 */
struct poliqarp_segment;

/**
 * Straightforwardly enough, an interpretation set is a collection of
 * interpretations (which see below).
 */
struct poliqarp_interpretation_set;

/**
 * An interpretation is a pair <b, t> of strings, representing the base form
 * and tag of a segment, respectively. As far as this API is concerned, tags
 * do not have internal structure; they are just strings. 
 */
struct poliqarp_interpretation;

/**
 * This is a collection of pieces of metadata that can be specified for a 
 * document.
 */
struct poliqarp_metadata_set;

/** 
 * A single element of the metadata set.
 */
struct poliqarp_metadata;

/**
 * State of a pseudo-random number generator.
 */
struct poliqarp_random_state;

/* General library routines. */

/**
 * Initializes the library. Must be used once before any other library
 * functions are called.
 * @param locale name to be passed to setlocale()
 * @return 0 if the initialization was successful, -1 otherwise.
 */
int poliqarp_create(const char *locale, struct poliqarp_error *error);

/**
 * Frees all resources used internally by the library. 
 * @return 0 if the cleanup was successful, -1 otherwise.
 */
int poliqarp_destroy(void);

/**
 * Initializes per-thread private data of the library. Must be called once
 * before any other library functions are called by every thread that uses
 * them.
 * @return 0 if the initialization was successful, -1 otherwise.
 */
int poliqarp_thread_init(void **data);

/**
 * Destructive counterpart of poliqarp_thread_init().
 * @return 0 if the cleanup was successful, -1 otherwise.
 */
int poliqarp_thread_done(void *data);

/* Corpus-related routines. */

/** 
 * Public corpus information. 
 */
struct poliqarp_corpus_info {
   size_t num_segments; /**< Number of segments in this corpus. */
   size_t num_types;    /**< Number of types of segments in this corpus. */
   size_t num_lemmata;  /**< Number of types of lemmata in this corpus. */
   size_t num_tags;     /**< Number of types of tags in this corpus. */
};

/**
 * Public tagset information.
 */
struct poliqarp_tagset_info {
   size_t num_categories; /**< Number of grammatical categories. */
   char **categories;     /**< Array of category descriptions. */
   size_t num_classes;    /**< Number of grammatical classes. */
   char **classes;        /**< Array of class descriptions. */
};

/** 
 * Opens a corpus.
 * @param corpus The structure to be initialized.
 * @param name Some kind of name for the corpus. It could be a file name, a
 * common prefix of several file names (as is the case with sakura), etc.
 * @param progress Indicator of the progress of this operation.
 * @return 0 if the corpus was opened successfully, -1 in case of an error.
 * @note It is safe to call this function in a thread that can be cancelled
 * with a deferred cancellation request.
 */
int poliqarp_open_corpus(struct poliqarp_corpus *corpus, const char *name, 
   progress_t *progress, struct poliqarp_error *error);

/**
 * Closes a corpus and frees all resources associated with it.
 * @param corpus The corpus to be closed.
 * @return 0 if the corpus was closed successfully, -1 in case of an error.
 * @note Think of this function as a hint to the library that a particular
 * corpus should be closed. In particular, the library might as well leave
 * it open if it so desires, or close it after a certain amount of time
 * (for instance if many users seem to be using this corpus intensively).
 */
int poliqarp_close_corpus(struct poliqarp_corpus *corpus);

/** 
 * Retrieves information about the corpus.
 * @param corpus The corpus to extract information from.
 * @param info The destination structure.
 * @return 0 upon successful completion, -1 upon error.
 */
int poliqarp_get_corpus_info(const struct poliqarp_corpus *corpus,
   struct poliqarp_corpus_info *info);

/**
 * Retrieves the tagset used by the corpus.
 * @param corpus The corpus to extract tagset information from.
 * @param info The destination structure.
 * @return 0 upon successful completion, -1 on error.
 */
int poliqarp_get_tagset_info(const struct poliqarp_corpus *corpus,
   struct poliqarp_tagset_info *info);

/**
 * Frees all resources allocated by poliqarp_get_tagset_info().
 */
void poliqarp_free_tagset_info(struct poliqarp_tagset_info *info);

/* Query-related routines. */

/**
 * Query flags.
 */
#define POLIQARP_QFLAG_QUERY_I 1 /**< Query case-insensitive */
#define POLIQARP_QFLAG_QUERY_X 2 /**< Query: not whole words */
#define POLIQARP_QFLAG_META_I 4 /**< Metadata case-insensitive */
#define POLIQARP_QFLAG_META_X 8 /**< Metadata: not whole words */

/** 
 * Analyzes query text and initializes a query structure.
 * @param query The query structure to be initialized.
 * @param text Text of the query.
 * @param corpus The corpus that this query will be run on.
 * @param flags Query flags, a combination of QFLAG_*.
 * @param rewrite Name of query rewriting rules or NULL.
 * @return 0 upon successful completion, -1 otherwise (e.g.
 *    in case of parse error).
 */
int poliqarp_create_query(struct poliqarp_query *query, const char *text,
   struct poliqarp_corpus *corpus, int flags, const char *rewrite,
   struct poliqarp_random_state *random_state,
   struct poliqarp_error *error);

/** 
 * Destroys a query object.
 * @param query The query to be destroyed.
 * @return 0 upon successful destroy, -1 if an error occurred. 
 */
int poliqarp_destroy_query(struct poliqarp_query *query);

/** 
 * Sends a message to the client, notifying it that several new
 * results of a query have been found. This function does not belong to
 * the corpus library, but is implemented in the daemon and declared here
 * as a means for the search routine to communicate with the outside world.
 * @param session The session parameter passed to poliqarp_produce.
 */
extern void async_notify_new_results(void *session);

/**
 * Executes a query on the corpus, producing at most a given number of
 * results. This function can be called multiple times to increasingly
 * produce more results.
 * @param buffer The match buffer to store results in.
 * @param count Maximum number of results to be produced.
 * @param query The query to be executed.
 * @param progress Structure indicating progress of the operation.
 * @param session The session to which send messages about new results.
 * @param notify_step If this is non-zero, asynchronous messages are sent
 * to the session specified by the former parameter each time this many
 * new results are found. Otherwise, no messages are sent.
 * @param max_match_length Maximum permissible match length, in segments.
 * @return 0 upon successful completion, -1 on error.
 * @note It is safe to call this function in a thread that can be cancelled
 * with a deferred cancellation request.
 */
int poliqarp_produce(struct poliqarp_match_buffer *buffer, size_t count, 
   struct poliqarp_query *query, progress_t *progress, void *session,
   size_t notify_step, size_t max_match_length);

/* Match buffer operations. */

/**
 * Match column. Designates one of the columns available in the match for
 * sorting.
 */
enum poliqarp_column {
   POLIQARP_COLUMN_LEFT_CONTEXT, /**< Selects left context for sorting. */
   POLIQARP_COLUMN_LEFT_MATCH,   /**< Selects left match for sorting. */
   POLIQARP_COLUMN_MATCH,        /**< Selects entire match for sorting. */
   POLIQARP_COLUMN_RIGHT_MATCH,  /**< Selects right match for sorting. */
   POLIQARP_COLUMN_RIGHT_CONTEXT /**< Selects right context for sorting. */
};

/**
 * Sorting criteria. Required by sorting routine.
 */
struct poliqarp_sort_info {
   enum poliqarp_column column; /**< What to sort by. */
   bool ascending;              /**< True iff sorting in ascending order. */
   bool atergo;                 /**< True iff sorting a tergo. */
   size_t context;              /**< Width of match context, in segments.
                                     Used when sorting by context. */
};

/**
 * Public information about the match buffer.
 */
struct poliqarp_match_buffer_info {
   size_t capacity;  /**< Size of buffer. */
   size_t used;      /**< Number of stored results. */
   size_t num_results;
   /**< Number of results spotted during query execution. */
};

/**
 * Creates a match buffer.
 * @param buffer The buffer structure to be initialized.
 * @param size Size of buffer.
 * @return 0 upon successful creation, -1 otherwise.
 */
int poliqarp_create_match_buffer(struct poliqarp_match_buffer *buffer, 
   size_t size);

/**
 * Destroys a match buffer.
 * @param buffer The buffer to be destroyed.
 * @return 0 upon successful destruction, -1 otherwise.
 */
int poliqarp_destroy_match_buffer(struct poliqarp_match_buffer *buffer);

/**
 * Retrieves information about match buffer.
 * @param buffer The buffer to extract information from.
 * @param info Structure that will hold the result.
 */
int poliqarp_get_match_buffer_info(struct poliqarp_match_buffer *buffer,
   struct poliqarp_match_buffer_info *info);

/**
 * Sorts match buffer according to the given criteria.
 * @param buffer The buffer to sort results in.
 * @param criteria Criteria of sorting.
 * @param progress Structure indicating progress of the operation.
 * @return 0 if sorting succeeded, -1 if it failed.
 * @note This routine performs a stable sort, which makes it possible to sort
 * the buffer using multiple criteria. To do that, it suffices to call this
 * function several times, starting with least significant criteria and
 * finishing with most significant ones.
 * @note It is safe to call this function in a thread that can be cancelled
 * with a deferred cancellation request.
 */
int poliqarp_sort_match_buffer(struct poliqarp_match_buffer *buffer, 
   const struct poliqarp_sort_info *criteria, progress_t *progress);

/**
 * Removes all matches from a match buffer. The size of the buffer remains
 * unchanged.
 * @param buffer The buffer to be cleared.
 * @return 0 upon successful completion, -1 on error.
 */
int poliqarp_forget(struct poliqarp_match_buffer *buffer);

/** 
 * Resizes a match buffer, possibly dropping several matches.
 * When the buffer gets enlarged, empty slots are added at the end.
 * When the buffer gets shrunk, the effect is twofold: first, empty
 * elements (if any) are removed from the end of the buffer; if that is not
 * sufficient, oldest matches are dropped to match the new size.
 * @param buffer The buffer to be resized.
 * @param size New size of the buffer.
 * @return 0 if the buffer got successfully resized, -1 on failure.
 */
int poliqarp_resize_match_buffer(struct poliqarp_match_buffer *buffer, 
   size_t size);

/* Match operations. */

/** 
 * Structure of a match is invariant to Poliqarp's design, so here it is --
 * basically a range with a point between its borders.
 */
struct poliqarp_match {
   size_t start; /** Offset of the first segment that belongs to this 
                     match. */
   size_t withinEnd;
   size_t withinStart;
   size_t end;   /** Offset of one-past-end segment in this match. */
   size_t focus; /** Offset (relative to start of corpus) of focus point. */
   size_t document; /** Document identifier associated with this match. */
};

/**
 * Retrieves a match from a match buffer.
 * @param buffer The buffer to retrieve the match from.
 * @param match The match to be retrieved.
 * @param index Index of the match in buffer.
 * @return 0 upon successful retrieval, -1 on error.
 * @note Does not do range checking if NDEBUG is defined.
 */
int poliqarp_get_match(const struct poliqarp_match_buffer *buffer, 
   struct poliqarp_match *match, size_t index);

/**
 * Creates a match for the whole document.
 * @param corpus Corpus that contains the document.
 * @param match The match to be created.
 * @param document Index of the document (valid values for this argument are
 *    values of 'document' field from struct poliqarp_match).
 * @return 0 upon successful completion, -1 on error.
 */
int poliqarp_get_match_for_document(const struct poliqarp_corpus *corpus,
   size_t document, struct poliqarp_match *match);

/* Segment operations. */

/**
 * Public segment information. 
 */
struct poliqarp_segment_info {
   const char *text;  /**< Orthographic form of a segment.       */
   bool space_before; /**< Is there a space before this segment? */
};

/**
 * Retrieves segment by index from a corpus.
 * @param segment Where to store the retrieved segment.
 * @param corpus The corpus to retrieve the segment from.
 * @param index Index of the segment to be retrieved.
 * @return 0 upon successful retrieval, -1 on error.
 * @note Does not do range checking if NDEBUG is defined.
 */
int poliqarp_get_segment(struct poliqarp_segment *segment,
   struct poliqarp_corpus *corpus, size_t index);

/**
 * Retrieves information about a segment.
 * @param segment Segment to extract information from.
 * @param info The destination structure.
 * @return 0 upon successful retrieval, -1 on error.
 */
int poliqarp_get_segment_info(const struct poliqarp_segment *segment,
   struct poliqarp_segment_info *info);

/* Interpretation set operations. */

/** 
 * Public information about interpretation set.
 */
struct poliqarp_interpretation_set_info {
   size_t size; /**< Number of interpretations. */
};

/**
 * Retrieves set of disambiguated interpretations of a segment.
 * @param segment The segment to extract the set from.
 * @param set The destination structure.
 * @return 0 upon successful retrieval, -1 on error.
 */
int poliqarp_get_disambiguated_interpretations(
   const struct poliqarp_segment *segment, 
   struct poliqarp_interpretation_set *set);

/**
 * Retrieves set of ambiguous interpretations of a segment.
 * @param segment The segment to extract the set from.
 * @param set The destination structure.
 * @return 0 upon successful retrieval, -1 on error.
 */
int poliqarp_get_ambiguous_interpretations(
   const struct poliqarp_segment *segment,
   struct poliqarp_interpretation_set *set);

/**
 * Retrieves information about a set of interpretations.
 * @param set The set to extract the information from.
 * @param info The destination structure.
 * @return 0 upon successful retrieval, -1 on error.
 */
int poliqarp_get_interpretation_set_info(
   const struct poliqarp_interpretation_set *set,
   struct poliqarp_interpretation_set_info *info);

/* Interpretation operations. */

/** 
 * Public information about an interpretation.
 */
struct poliqarp_interpretation_info {
   const char *base; /**< Base form of the segment. */
   const char *tag;  /**< Unparsed tag. */
};

/**
 * Retrieves an interpretation from a set.
 * @param set The set to extract the interpretation from.
 * @param interp The destination structure.
 * @param index Index of the interpretation in a set.
 * @return 0 upon successful retrieval, -1 on error.
 */
int poliqarp_get_interpretation(const struct poliqarp_interpretation_set *set,
   struct poliqarp_interpretation *interp, size_t index);

/**
 * Retrieves information about an interpretation.
 * @param interp The interpretation to extract information from.
 * @param info The destination structure.
 * @return 0 upon successful retrieval, -1 on error.
 */
int poliqarp_get_interpretation_info(
   const struct poliqarp_interpretation *interp, 
   struct poliqarp_interpretation_info *info);

/* Alias operations. */

/**
 * Public alias information.
 */
struct poliqarp_alias {
   const char *name;
   const char *value;
};

/**
 * Public alias list information.
 */
struct poliqarp_alias_list {
   struct poliqarp_alias *aliases;
   size_t num_aliases;
};

/**
 * Defines an alias for an open corpus. From now on, all occurrences of 
 * `name' when querying this corpus will be replaced by `value'.
 * @param corpus Corpus to define alias for.
 * @param name Name of the alias.
 * @param value Value of the alias.
 * @return 0 upon successful completion, -1 on error.
 */
int poliqarp_define_alias(struct poliqarp_corpus *corpus, const char *name,
   const char *value);

/**
 * Deletes an alias that is currently defined for this corpus. 
 * @param corpus Corpus to define alias for.
 * @param name Name of the alias that is being deleted.
 * @return 0 upon successful completion, -1 on error (e.g. no such alias
 *    exists).
 */
int poliqarp_delete_alias(struct poliqarp_corpus *corpus, const char *name);

/**
 * Retrieves the list of aliases available for an open corpus. 
 * @param corpus Corpus to retrieve aliases for.
 * @param aliases Pointer to a structure that will contain the aliases.
 * @return 0 on successful completion, -1 on error.
 */
int poliqarp_get_aliases(const struct poliqarp_corpus *corpus, 
   struct poliqarp_alias_list *aliases);

/**
 * Frees the memory allocated for the alias list by poliqarp_get_aliases().
 * @param aliases Structure to be released.
 * @return 0 on successful completion, -1 on error.
 */
int poliqarp_free_aliases(struct poliqarp_alias_list *aliases);

/* Metadata operations. */

/**
 * A piece of metadata is a 'key-value' pair. The key can be an arbitrary
 * string, whereas values come in two flavours: textual and date. This
 * enum defines the type of metadata. 
 */
enum poliqarp_metadata_type {
   POLIQARP_META_TEXT, /**< Textual piece of metadata. */
   POLIQARP_META_DATE, /**< Date-holding piece of metadata. */
   POLIQARP_META_UNDEFINED /**< This information has not been defined 
                                for this corpus. */
};

/**
 * One possible value type for a metadata can be a date. This is useful when
 * specifying information such as date of creation, date of first publication,
 * etc.
 */
struct poliqarp_date {
   int year;   /**< Year.  */
   int month;  /**< Month. */
   int day;    /**< Day.   */
};

/**
 * A binding of key name to type of metadata.
 */
struct poliqarp_metadata_type_binding {
   char *key;                            /**< Name of the key. */
   enum poliqarp_metadata_type type;     /**< Type of values for this key. */
};

/**
 * The set of metadata types: an array of bindings of metadata keys to
 * values.
 */
struct poliqarp_metadata_types {
   struct poliqarp_metadata_type_binding *types; /**< The set proper. */
   size_t num_types;                             /**< Number of types. */
};

/**
 * Public information about a piece of metadata. 
 */
struct poliqarp_metadata_info {
   enum poliqarp_metadata_type type;  /**< Type of this metadata. */
   const char *key;                   /**< Name of key. */
   union {
      const char *text;               /**< Textual value. */
      struct poliqarp_date date;      /**< Date value. */
   } value;                           /**< Value union. */
};

/**
 * Retrieves the set of metadata for a given document.
 * @param corpus Corpus that contains the metadata.
 * @param document Index of the document (valid values for this argument are
 *    values of 'document' field from struct poliqarp_match).
 * @param set The destination structure.
 * @return 0 upon successful completion, -1 on error.
 */
int poliqarp_get_metadata_set(const struct poliqarp_corpus *corpus,
   size_t document, struct poliqarp_metadata_set *set);

/**
 * Retrieves the set of metadata types defined for a given corpus.
 * @param corpus Corpus that contains the metadata.
 * @param types The structure to contain the set.
 * @return 0 upon successful completion, -1 on error.
 */
int poliqarp_get_metadata_types(struct poliqarp_metadata_types *types,
   const struct poliqarp_corpus *corpus);

/**
 * Frees the resources allocated by poliqarp_get_metadata_types.
 * @param types The structure to be freed.
 * @return 0 upon successful completion, -1 on error.
 */
int poliqarp_free_metadata_types(struct poliqarp_metadata_types *types);

/**
 * Returns number of pieces of metadata in a set.
 * @param set The set of metadata.
 */
size_t poliqarp_metadata_count(const struct poliqarp_metadata_set *set);

/**
 * Returns a single piece of metadata from a set.
 * @param set The set of metadata.
 * @param index Index of the piece of metadata to retrieve.
 * @param meta The destination structure.
 */
int poliqarp_get_metadata(const struct poliqarp_metadata_set *set, 
   size_t index, struct poliqarp_metadata *meta);

/**
 * Retrieves information about a single piece of metadata. 
 * @param meta The metadata to query about.
 * @param info The public information structure.
 * @return 0 on successful retrieval, -1 on failure.
 */
int poliqarp_get_metadata_info(const struct poliqarp_metadata *meta,
   struct poliqarp_metadata_info *info);

/* 
 * Finally, include the definitions of private structures so that their sizes
 * are known to the user.
 */
#include <sakura/poliqarp-private.h>

#endif /* POLIQARP_H */
