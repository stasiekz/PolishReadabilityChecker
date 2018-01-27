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

#ifndef POLIQARP_VALUE_H
#define POLIQARP_VALUE_H

#include <stdlib.h>

#include <sakura/common/bit-routines.h>
#include <sakura/corpus.h>
#include <sakura/abi.h>

/** @defgroup poliqarp_value Expression Value Representation */
/** @{ */
/** @file value.h Expression value representation */

/** 
 * Match domain.
 *
 * Some values match only in a specific domain.
 * For example the expression:
 *    base='foo'
 * will create an expression that will match in the POLIQARP_DOMAIN_BASE domain
 * to all instances of the word 'foo'.
 * This is used to know which dictionary to use
 **/
enum poliqarp_match_domain {
   POLIQARP_DOMAIN_ORTH,            /**< Orth domain.                         */
   POLIQARP_DOMAIN_SPACE,           /**< Space domain.                        */
   POLIQARP_DOMAIN_BASE,            /**< Base domain.                         */
   POLIQARP_DOMAIN_TAG,             /**< Tag domain.                          */
   POLIQARP_DOMAIN_INTERP__DISAMB,  /**< Disambiguated interpretation domain. */
   POLIQARP_DOMAIN_INTERP__AMB,     /**< Ambiguous interpretation domain.     */
   POLIQARP_DOMAIN_TYPE             /**< Phrasal type domain.                 */
};

/** 
 * Match strategy. Determines how the value behaves when attempting to match
 * given position. Directly resembles differences between the '=' and '~' 
 * operators 
 */
enum poliqarp_match_strategy {
   POLIQARP_STRATEGY_ANY, /**< Match succeeds when any interpretation is 
                           matched. */
   POLIQARP_STRATEGY_ALL  /**< Match succeeds when all interpretations are 
                           matched. */
};

/** Logic operator, used to create complex expressions. */
enum poliqarp_logic_operator {
   POLIQARP_OPERATOR_AND,  /**< And operator.       */
   POLIQARP_OPERATOR_OR    /**< Or operator.        */
};

/**
 * Value structure.
 *
 * Values can be evaluated on given corpus and segment.
 * The result of such evaluation can be one of: 'matches' or 'does not match'
 * Values can be combined to build expressions
 */
struct poliqarp_value {
   unsigned *bits;   /**< Bit field indicating 'match' or 'no match'.      */
   size_t num_bytes; /**< Length of that field in bytes.                   */
   size_t num_items; /**< Number of bits used in that field.               */
   size_t num_hits;  /**< Number of one's in the used part of that field.  */
   enum poliqarp_match_domain domain;  /**< Operator domain.               */
   enum poliqarp_match_strategy strategy;  
                     /**< Operator match strategy.                         */
   bool use_disamb;  /**< Process only disambiguated interpretations.      */
};

/** 
 * Check if an expression made of two values and operator can be optimized. 
 * @param this Left value.
 * @param that Right value.
 * @param oper Operator.
 */
bool poliqarp_value_can_be_optimized(const struct poliqarp_value *this,
   const struct poliqarp_value *that, enum poliqarp_logic_operator oper);

/** 
 * Optimize an expression made of two values and an opeator.
 * This function makes sure that this and that are taken care of.
 * Caller is responsible for freeing the result.
 * @param this Left value.
 * @param that Right value.
 * @param oper Operator.
 */
struct poliqarp_value *poliqarp_value_optimize(const struct poliqarp_value *this,
   const struct poliqarp_value *that, enum poliqarp_logic_operator oper);

/**
 * Comparison operator.
 * This is used to enforce some ordering of values so that they can be 
 * sorted and looked up quickly
 **/
int poliqarp_value_compare(const struct poliqarp_value *this, 
   const struct poliqarp_value *that);

/** Value destructor. */
void poliqarp_value_destroy(struct poliqarp_value *this);

/** Modify the value to use given match strategy. */
void poliqarp_value_use_strategy(struct poliqarp_value *this,
   enum poliqarp_match_strategy strategy);

/** Modify the value to use given disambiguation settings. */
void poliqarp_value_use_disamb(struct poliqarp_value *this, bool use_disamb);

/**
 * Micro-evaluate the value for this interpretation.
 * Use the MATCH_ANY strategy for TAG domain
 */
static inline bool poliqarp_value_micro_eval_tag_any(unsigned *bits,
   const struct poliqarp_binary_interp *interp,
   const struct poliqarp_binary_interp *interp_end)
{
   for (; interp < interp_end; ++interp) {
      struct poliqarp_binary_interp terp = *interp;
      POLIQARP_INTERP_LE_TO_HE(terp);
      if (BIT_ARRAY_GET(bits, terp.tag_id))
         return true;
   }
   return false;
}

/**
 * Micro-evaluate the value for this interpretation.
 * Use the MATCH_ALL strategy for TAG domain
 */
static inline bool poliqarp_value_micro_eval_tag_all(unsigned *bits,
   const struct poliqarp_binary_interp *interp,
   const struct poliqarp_binary_interp *interp_end)
{
   /* try to find a true value, exit on first false found */
   for (; interp < interp_end; ++interp) {
      struct poliqarp_binary_interp terp = *interp;
      POLIQARP_INTERP_LE_TO_HE(terp);
      if (BIT_ARRAY_GET(bits, terp.tag_id) == false)
         return false;
      else
         goto found_true;
   }
   return false;
found_true:
   /* now that we've already seen one true value we can just exit on any 
    * false value we encounter */
   for (; interp < interp_end; ++interp) {
      struct poliqarp_binary_interp terp = *interp;
      POLIQARP_INTERP_LE_TO_HE(terp);
      if (BIT_ARRAY_GET(bits, terp.tag_id) == false)
         return false;
   }
   return true;
}

/**
 * Micro-evaluate the value for this interpretation.
 * Use the MATCH_ANY strategy for BASE domain
 */
static inline bool poliqarp_value_micro_eval_base_any(unsigned *bits,
   const struct poliqarp_binary_interp *interp,
   const struct poliqarp_binary_interp *interp_end)
{
   for (; interp < interp_end; ++interp) {
      struct poliqarp_binary_interp terp = *interp;
      POLIQARP_INTERP_LE_TO_HE(terp);
      if (BIT_ARRAY_GET(bits, terp.base_id))
         return true;
   }
   return false;
}

/**
 * Micro-evaluate the value for this interpretation.
 * Use the MATCH_ALL strategy for BASE domain
 */
static inline bool poliqarp_value_micro_eval_base_all(unsigned *bits,
   const struct poliqarp_binary_interp *interp,
   const struct poliqarp_binary_interp *interp_end)
{
   /* try to find a true value, exit on first false found */
   for (; interp < interp_end; ++interp) {
      struct poliqarp_binary_interp terp = *interp;
      POLIQARP_INTERP_LE_TO_HE(terp);
      if (BIT_ARRAY_GET(bits, terp.base_id) == false)
         return false;
      else
         goto found_true;
   }
   return false;
found_true:
   /* now that we've already seen one true value we can just exit on any 
    * false value we encounter */
   for (; interp < interp_end; ++interp) {
      struct poliqarp_binary_interp terp = *interp;
      POLIQARP_INTERP_LE_TO_HE(terp);
      if (BIT_ARRAY_GET(bits, terp.base_id) == false)
         return false;
   }
   return true;
}

/** 
 * Evaluate value and return the result. 
 * @param this Value object.
 * @param corpus Corpus object.
 * @param pos Position for which the value is calculated.
 */
static inline bool poliqarp_value_eval(const struct poliqarp_value *this,
   const struct poliqarp_corpus * corpus,
   const struct poliqarp_binary_segment *pos)
{
   switch (this->domain) {
      case POLIQARP_DOMAIN_ORTH:
         return BIT_ARRAY_GET(this->bits, pos->orth_space_id >> 1);
      case POLIQARP_DOMAIN_SPACE:
         return BIT_ARRAY_GET(this->bits, 0) == (pos->orth_space_id & 1);
      case POLIQARP_DOMAIN_INTERP__DISAMB:
         return BIT_ARRAY_GET(this->bits, pos->interp_disamb_id);
      case POLIQARP_DOMAIN_INTERP__AMB:
         return BIT_ARRAY_GET(this->bits, pos->interp_amb_id);
      case POLIQARP_DOMAIN_TYPE:
         return false;
      default:
         abort(); /* Should not happen. */
   }
}

/** @} */

#endif /* VALUE_H */

