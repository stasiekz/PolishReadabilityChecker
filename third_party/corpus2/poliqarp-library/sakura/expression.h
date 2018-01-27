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

#ifndef POLIQARP_EXPRESSION_H
#define POLIQARP_EXPRESSION_H

#include <sakura/corpus.h>
#include <sakura/value.h>

/** Expression node type. */
enum poliqarp_expression_type {
   POLIQARP_EXPRESSION_VALUE,    /**< Expression contains some (calulatable) 
                                  logic value.            */
   POLIQARP_EXPRESSION_AND,      /**< Logical AND expression. */
   POLIQARP_EXPRESSION_OR,       /**< Logical OR expression.  */
   POLIQARP_EXPRESSION_CONSTANT, /**< Expression contains some constant 
                                  logic value.            */
   POLIQARP_EXPRESSION_PHRASE,   /**< Expression contains a phrase
                                  heads description. */
   POLIQARP_EXPRESSION_VARIABLE, /**< Expression contains a variable */
   POLIQARP_EXPRESSION_INVALID = -1 /**< It is not possible to determine
                                         expression type */
};

/**
 * Evaluation function type.
 */
typedef bool (*poliqarp_value_eval_fn)(void *this, 
   const struct poliqarp_corpus *corpus, const void *argument);

/**
 * Comparator function type.
 */
typedef int (*poliqarp_value_compare_fn)(const void *this, const void *that);

/**
 * Destructor function type.
 */
typedef void (*poliqarp_value_destroy_fn)(void *this);

/** Expression node structure. */
struct poliqarp_expression {
   enum poliqarp_expression_type type;        /**< Type of the expression. */
   union {
      struct {
         bool negate;                     /**< Whether to negate calculated 
                                               value.                  */
         void *value;                     /**< Value pointer.          */
         poliqarp_value_eval_fn eval;         /**< Evaluator function.     */
         poliqarp_value_compare_fn compare;   /**< Comparator function.    */
         poliqarp_value_destroy_fn destroy;   /**< Destructor function.    */
      } value;                            /**< Value stuff.            */
      struct {
         bool negate;                     /**< Whether to negate calculated 
                                               value.                  */
         struct poliqarp_expression *left;    /**< Left hand expression.   */
         struct poliqarp_expression *right;   /**< Right hand expression.  */
      } expression;                       /**< Expression stuff.       */
      struct {
         bool same;                       /**< Whether syntactic and semantic
                                               heads must be the same 
                                               segment.                */
         bool all;                        /**< Whether all members of a
                                               coordination phrase should
                                               match.                  */
         bool negate;                     /**< Whether to negate calculated
                                               value.                  */
         struct poliqarp_expression *synh;    /**< Syntactic head.     */
         struct poliqarp_expression *semh;    /**< Semantic head.      */
      } phrase;
      struct {
         const void* type;
         struct poliqarp_expression **children;
         size_t n_children;
         int id;
      } variable;                         /**< Variable wrapper.       */
      bool constant;                      /**< Constant value.         */
   } as;                                  /**< Data union.             */
};

#define POLIQARP_MAX_VARIABLES 16

/** Create logical 'and' expression. */
struct poliqarp_expression *poliqarp_and_expression(struct poliqarp_expression *left,
   struct poliqarp_expression *right);

/** Create logical 'or' expression. */
struct poliqarp_expression *poliqarp_or_expression(struct poliqarp_expression *left,
   struct poliqarp_expression *right);

/** Create logical 'not' expression. */
struct poliqarp_expression *poliqarp_not_expression(struct poliqarp_expression *this);

/** Create a phrasal expression with given head specifications. */
struct poliqarp_expression *poliqarp_phrase_expression(struct poliqarp_expression *synh,
   struct poliqarp_expression *semh, bool same, bool all);

/** Create an expression with a variable. */
struct poliqarp_expression *poliqarp_variable_expression(size_t n_children,
   struct poliqarp_expression **children, int id);

/** Create constant expression. */
struct poliqarp_expression *poliqarp_expression_create_constant(bool value);

/** Create value expression. */
struct poliqarp_expression *poliqarp_expression_create_value(void *value,
   poliqarp_value_eval_fn eval, poliqarp_value_compare_fn compare, 
   poliqarp_value_destroy_fn destroy);

/** Evaluate expression. */
bool poliqarp_expression_eval(const struct poliqarp_expression *this,
   const struct poliqarp_corpus *corpus, 
   const void *argument, const void *bindings);

/** Compare two expressions. */
int poliqarp_expression_compare(const struct poliqarp_expression *this,
   const struct poliqarp_expression *that);

/** Free expression. */
void poliqarp_expression_destroy(struct poliqarp_expression *this);

/** Checks the type of an expression. Return POLIQARP_EXPRESSION_INVALID if the
 *  types don't match. */
enum poliqarp_expression_type poliqarp_expression_type(
   const struct poliqarp_expression *this);

/**
 * FIXME: write a proper comment for this function
 */
size_t poliqarp_expression_variable_ranges(
   const struct poliqarp_expression *this, size_t *ranges);

#endif
