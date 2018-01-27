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
 * @file abi.h
 * This file describes binary corpus format.
 */

#ifndef POLIQARP_ABI_H
#define POLIQARP_ABI_H

#include <poliqarp-config.h>
#include <stddef.h>

#include <stddef.h>

#define POLIQARP_ABI_VERSION 2

/**
 * Interpretation item ABI.
 */
struct poliqarp_binary_interp {
   uint32_t base_id; /**< Identifier of string associated with this 
                          base form. */
   uint32_t tag_id;  /**< Identifier of string associated with this tag. */
} __attribute__((packed)); /* -64- */

union poliqarp_legacy_binary_interp {
   struct {
      unsigned base_id:20; /**< Identifier of string associated with this 
                                base form. */
      unsigned tag_id:12;  /**< Identifier of string associated with this tag. */
   } __attribute__((packed)) attributes; /* -32- */
   uint32_t data;
};

static inline struct poliqarp_binary_interp
poliqarp_convert_legacy_binary_interp(
   const union poliqarp_legacy_binary_interp legacy_interpretation)
{
   struct poliqarp_binary_interp res;
   uint32_t data = en4(legacy_interpretation.data);
   res.base_id = data & 0xfffff;
   res.tag_id = data >> 20;
   return res;
}

/**
 * Segment item ABI.
 */
struct poliqarp_binary_segment {
   uint32_t orth_space_id;    /**< Identifier of string associated with 
                                   this segment; lowest bit set to 1
                                   if a space occurs before this segment. */
   uint32_t interp_disamb_id; /**< Identifier of the interpretation set 
                                   (disambiguated). */
   uint32_t interp_amb_id;    /**< Identifier of the interpretation set 
                                   (ambiguous). */
} __attribute__((packed)); /* -96- */

union poliqarp_legacy_binary_segment {
   struct {
      unsigned space:1;             /**< Set to one if a space occurs before 
                                         this segment. */
      unsigned orth_id:21;          /**< Identifier of string associated with 
                                         this segment. */
      unsigned interp_disamb_id:21; /**< Identifier of the interpretation set 
                                         (disambiguated). */
      unsigned interp_amb_id:21;    /**< Identifier of the interpretation set 
                                         (ambiguous). */
   } __attribute__((packed)) attributes;
   uint64_t data;
} __attribute__((packed)); /* -64- */

static inline struct poliqarp_binary_segment
poliqarp_convert_legacy_binary_segment(
   const union poliqarp_legacy_binary_segment legacy_segment)
{
   struct poliqarp_binary_segment res;
   uint64_t data = en8(legacy_segment.data);
   res.orth_space_id = data & 0x3fffff;
   res.interp_disamb_id = (data >> 22) & 0x1fffff;
   res.interp_amb_id = (data >> 43) & 0x1fffff;
   return res;
}

#ifdef WORDS_BIGENDIAN

#define POLIQARP_INTERP_LE_TO_HE(x) do { \
   (x).base_id = en4((x).base_id); \
   (x).tag_id = en4((x).tag_id); \
} while (0)

#define POLIQARP_SEGMENT_LE_TO_HE(x) do { \
   (x).orth_space_id = en4((x).orth_space_id); \
   (x).interp_disamb_id = en4((x).interp_disamb_id); \
   (x).interp_amb_id = en4((x).interp_amb_id); \
} while (0)

#else

#define POLIQARP_INTERP_LE_TO_HE(x) do { } while (0)
#define POLIQARP_SEGMENT_LE_TO_HE(x) do { } while (0)

#endif

struct poliqarp_interpretation {
   const struct poliqarp_corpus *corpus;
   struct poliqarp_binary_interp interp;
   bool disamb;
};

struct poliqarp_segment {
   const struct poliqarp_corpus *corpus; /**< Corpus that this segment
                                              belongs to. */
   struct poliqarp_binary_segment segment; /**< Information about the 
                                                segment. */
};

struct poliqarp_interpretation_set {
   const struct poliqarp_corpus *corpus; 
                 /**< Corpus that this set belongs to. */
   uint32_t set; /**< Identifier of the set. */
   bool disamb;  /**< True iff it's a disambiguated set. */
};

/** Date metadata, all fields are self-explanatory. */
struct poliqarp_meta_date {
   uint16_t year;       /**< Year.  */
   uint8_t month;       /**< Month. */
   uint8_t day;         /**< Day.   */
};

enum poliqarp_binary_metadata_type {
   POLIQARP_METADATA_SINGLE,    /**< Item has a single value.              */
   POLIQARP_METADATA_MULTI,     /**< Item has multiple values.             */
   POLIQARP_METADATA_DATE,      /**< Item has a single date value.         */
   POLIQARP_METADATA_UNDEFINED  /**< This value is undefined for the item. */
};

/** Type + value union. */
struct poliqarp_binary_metadata {
   uint32_t type; /**< Type of meta item. */
   uint32_t key;                 /**< Key to key dictionary in backend meta. */
   union {
      uint32_t text;                  /**< Key to value dictionary. */
      struct poliqarp_meta_date date; /**< Packed date.             */
   } value_as;                        /**< Union of values.         */
};

struct poliqarp_metadata {
   const struct poliqarp_corpus *corpus; 
      /**< Corpus that this metadata belongs to.    */
   struct poliqarp_binary_metadata meta; /**< The metadata proper. */
};

struct poliqarp_metadata_set {
   const struct poliqarp_corpus *corpus; 
      /**< Corpus that this set belongs to.    */
   size_t low;  /**< Lower range of metadata.  */
   size_t high; /**< Higher range of metadata. */
};

/** Document information. */
struct poliqarp_document {
   uint32_t corpus_low;    /**< Lower bound of corpus.     */
   uint32_t corpus_high;   /**< Higher bound of corpus.    */
   uint32_t meta_low;      /**< Lower bound of meta data.  */
   uint32_t meta_high;     /**< Higher bound of meta data. */
};

/** Subdocument information. */
struct poliqarp_subdocument {
   uint32_t corpus_low;    /**< Lower bound of corpus.   */
   uint32_t corpus_high;   /**< Higher bound of corpus.  */
};

#endif /* ABI_H */
