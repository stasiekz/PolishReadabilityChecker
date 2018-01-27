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

#include <errno.h>

#include <sakura/expression.h>

struct poliqarp_expression *poliqarp_and_expression(struct poliqarp_expression *left,
   struct poliqarp_expression *right)
{
   if (left->type == POLIQARP_EXPRESSION_CONSTANT) {
      /* left side is constant, return either constant false or 
       * right expression */
      if (left->as.constant == false) {
         poliqarp_expression_destroy(right);
         return left;
      } else {
         poliqarp_expression_destroy(left);
         return right;
      }
   } else if (right->type == POLIQARP_EXPRESSION_CONSTANT) {
      /* right side is constant, return either constant false or 
       * left expression */
      if (right->as.constant == false) {
         poliqarp_expression_destroy(left);
         return right;
      } else {
         poliqarp_expression_destroy(right);
         return left;
      }
   } else {
      /* both are ordinary values, create extra logical node */
      struct poliqarp_expression *this = malloc(sizeof *this);
      if (this == NULL)
         return NULL;
      this->type = POLIQARP_EXPRESSION_AND;
      this->as.expression.negate = false;
      this->as.expression.left = left;
      this->as.expression.right = right;
      if (poliqarp_expression_type(this) == POLIQARP_EXPRESSION_INVALID) {
         free(this);
         return NULL;
      }
      return this;
   }
}

struct poliqarp_expression *poliqarp_or_expression(struct poliqarp_expression *left,
   struct poliqarp_expression *right)
{
   if (left->type == POLIQARP_EXPRESSION_CONSTANT) {
      /* left side is constant, return either constant true or 
       * right expression */
      if (left->as.constant == true) {
         poliqarp_expression_destroy(right);
         return left;
      } else {
         poliqarp_expression_destroy(left);
         return right;
      }
   } else if (right->type == POLIQARP_EXPRESSION_CONSTANT) {
      /* right side is constant, return either constant false or 
       * left expression */
      if (right->as.constant == true) {
         poliqarp_expression_destroy(left);
         return right;
      } else {
         poliqarp_expression_destroy(right);
         return left;
      }
   } else {
      /* both are ordinary values, create extra logical node */
      struct poliqarp_expression *this = malloc(sizeof *this);
      if (this == NULL)
         return NULL;
      this->type = POLIQARP_EXPRESSION_OR;
      this->as.expression.negate = false;
      this->as.expression.left = left;
      this->as.expression.right = right;
      if (poliqarp_expression_type(this) == POLIQARP_EXPRESSION_INVALID) {
         free(this);
         return NULL;
      }
      return this;
   }
}

struct poliqarp_expression *poliqarp_phrase_expression(struct poliqarp_expression *synh,
   struct poliqarp_expression *semh, bool same, bool all)
{
   struct poliqarp_expression *this = malloc(sizeof *this);
   if (this == NULL)
      return NULL;
   this->type = POLIQARP_EXPRESSION_PHRASE;
   this->as.phrase.same = same;
   this->as.phrase.all = all;
   this->as.phrase.synh = synh;
   this->as.phrase.semh = semh;
   this->as.phrase.negate = false;
   return this;
}

struct poliqarp_expression *poliqarp_variable_expression(size_t n_children,
   struct poliqarp_expression **children, int id)
{
   struct poliqarp_expression *this = malloc(sizeof *this);
   if (this == NULL)
      return NULL;
   this->type = POLIQARP_EXPRESSION_VARIABLE;
   assert(n_children > 0);
   this->as.variable.n_children = n_children;
   this->as.variable.children = children;
   this->as.variable.id = id;
   return this;
}

struct poliqarp_expression *poliqarp_not_expression(struct poliqarp_expression *this)
{
   switch (this->type) {
      case POLIQARP_EXPRESSION_AND:
      case POLIQARP_EXPRESSION_OR:
         this->as.expression.negate ^= true;
         break;
      case POLIQARP_EXPRESSION_VALUE:
         this->as.value.negate ^= true;
         break;
      case POLIQARP_EXPRESSION_CONSTANT:
         this->as.constant ^= true;
         break;
      case POLIQARP_EXPRESSION_PHRASE:
         this->as.phrase.negate ^= true;
         break;
      case POLIQARP_EXPRESSION_VARIABLE:
         {
            size_t i;
            for (i = 0; i < this->as.variable.n_children; i++)
               poliqarp_not_expression(this->as.variable.children[i]);
         }
         break;
      case POLIQARP_EXPRESSION_INVALID:
         abort(); /* shoult not happen */
   }
   return this;
}


struct poliqarp_expression *poliqarp_expression_create_constant(bool value)
{
   struct poliqarp_expression *this = malloc(sizeof *this);
   if (this == NULL)
      return NULL;
   this->type = POLIQARP_EXPRESSION_CONSTANT;
   this->as.constant = value;
   return this;
}

struct poliqarp_expression *poliqarp_expression_create_value(void *value,
   poliqarp_value_eval_fn eval, poliqarp_value_compare_fn compare, 
   poliqarp_value_destroy_fn destroy)
{
   struct poliqarp_expression *this = malloc(sizeof *this);
   if (this == NULL)
      return NULL;
   this->type = POLIQARP_EXPRESSION_VALUE;
   this->as.value.value = value;
   this->as.value.eval = eval;
   this->as.value.compare = compare;
   this->as.value.destroy = destroy;
   this->as.value.negate = false;

   return this;
}

static bool poliqarp_expression_eval__and(const struct poliqarp_expression *this,
   const struct poliqarp_corpus *corpus,
   const void *argument, const void *bindings)
{
   assert(this != NULL);
   return this->as.expression.negate ^ 
      (poliqarp_expression_eval(this->as.expression.left, corpus, argument, bindings) && 
       poliqarp_expression_eval(this->as.expression.right, corpus, argument, bindings));
}

static bool poliqarp_expression_eval__or(const struct poliqarp_expression *this,
   const struct poliqarp_corpus *corpus,
   const void *argument, const void *bindings)
{
   assert(this != NULL);
   return this->as.expression.negate ^ 
      (poliqarp_expression_eval(this->as.expression.left, corpus, argument, bindings) || 
       poliqarp_expression_eval(this->as.expression.right, corpus, argument, bindings));
}

static bool poliqarp_expression_eval__value(const struct poliqarp_expression *this,
   const struct poliqarp_corpus *corpus,
   const void *argument, const void *bindings)
{
   assert(this != NULL);
   return this->as.value.negate ^ this->as.value.eval(this->as.value.value,
      corpus, argument);
}

static bool poliqarp_expression_eval__const(const struct poliqarp_expression *this,
   const struct poliqarp_corpus *corpus,
   const void *argument, const void *bindings)
{
   assert(this != NULL);
   return this->as.constant;
}

static bool poliqarp_expression_eval__variable(const struct poliqarp_expression *this,
   const struct poliqarp_corpus *corpus,
   const void *argument, const void *bindings)
{
   assert(bindings != NULL);
   assert(this != NULL);
   const size_t *variable_bindings = bindings;
   size_t nth = variable_bindings[this->as.variable.id];
   return poliqarp_expression_eval(this->as.variable.children[nth], corpus,
      argument, bindings);
}

bool poliqarp_expression_eval(const struct poliqarp_expression *this,
   const struct poliqarp_corpus *corpus,
   const void *argument, const void *bindings)
{
   assert(this != NULL);
   assert(corpus != NULL);
   assert(argument != NULL);
   switch (this->type) {
      case POLIQARP_EXPRESSION_AND:
         return poliqarp_expression_eval__and(this, corpus, argument, bindings);
      case POLIQARP_EXPRESSION_OR:
         return poliqarp_expression_eval__or(this, corpus, argument, bindings);
      case POLIQARP_EXPRESSION_VALUE:
         return poliqarp_expression_eval__value(this, corpus, argument, bindings);
      case POLIQARP_EXPRESSION_CONSTANT:
         return poliqarp_expression_eval__const(this, corpus, argument, bindings);
      case POLIQARP_EXPRESSION_PHRASE:
         return true;
      case POLIQARP_EXPRESSION_VARIABLE:
         return poliqarp_expression_eval__variable(this, corpus, argument, bindings);
      case POLIQARP_EXPRESSION_INVALID:
         abort(); /* Should not happen. */
   }
   abort(); /* Should not happen. */
}

int poliqarp_expression_compare(const struct poliqarp_expression *this,
   const struct poliqarp_expression *that)
{
   size_t i;
   int res;
   if (this == that)
      return 0;
   if (this == NULL)
      return 1;
   if (that == NULL)
      return -1;
   if (this->type != that->type)
      return this->type - that->type;

   switch (this->type) {
      case POLIQARP_EXPRESSION_OR:
      case POLIQARP_EXPRESSION_AND:
         if (this->as.expression.negate != that->as.expression.negate)
            return this->as.expression.negate - that->as.expression.negate;
         res = poliqarp_expression_compare(this->as.expression.left,
             that->as.expression.left);
         if (res)
            return res;
         return poliqarp_expression_compare(this->as.expression.right,
             that->as.expression.right);
      case POLIQARP_EXPRESSION_VALUE:
         if (this->as.value.negate != that->as.value.negate)
            return this->as.value.negate - that->as.value.negate;
         return this->as.value.compare(this->as.value.value,
             that->as.value.value);
      case POLIQARP_EXPRESSION_CONSTANT:
         return this->as.constant - that->as.constant;
      case POLIQARP_EXPRESSION_PHRASE:
         if (this->as.phrase.same != that->as.phrase.same)
            return this->as.phrase.same - that->as.phrase.same;
         if (this->as.phrase.all != that->as.phrase.all)
            return this->as.phrase.all - that->as.phrase.all;
         if (this->as.phrase.negate != that->as.phrase.negate)
            return this->as.phrase.negate - that->as.phrase.negate;
         res = poliqarp_expression_compare(this->as.phrase.synh, that->as.phrase.synh);
         if (res)
            return res;
         return poliqarp_expression_compare(this->as.phrase.semh, that->as.phrase.semh);
      case POLIQARP_EXPRESSION_VARIABLE:
         if (this->as.variable.id != that->as.variable.id)
            return this->as.variable.id - that->as.variable.id;
         assert(this->as.variable.n_children == that->as.variable.n_children);
         for (i = 0; i < this->as.variable.n_children; i++)
         {
            res = poliqarp_expression_compare(
               this->as.variable.children[i],
               that->as.variable.children[i]);
            if (res)
               return res;
         }
         return 0;
      case POLIQARP_EXPRESSION_INVALID:
         abort(); /* Should not happen. */
   }
   abort(); /* Should not happen. */
}

void poliqarp_expression_destroy(struct poliqarp_expression *this)
{
   size_t i;
   if (this == NULL)
      return;
   switch (this->type) {
      case POLIQARP_EXPRESSION_AND:
      case POLIQARP_EXPRESSION_OR:
         poliqarp_expression_destroy(this->as.expression.left);
         poliqarp_expression_destroy(this->as.expression.right);
         break;
      case POLIQARP_EXPRESSION_VALUE:
         this->as.value.destroy(this->as.value.value);
         break;
      case POLIQARP_EXPRESSION_CONSTANT:
         break;
      case POLIQARP_EXPRESSION_PHRASE:
         if (this->as.phrase.synh != NULL)
            poliqarp_expression_destroy(this->as.phrase.synh);
         if (this->as.phrase.semh != NULL && !this->as.phrase.same)
            poliqarp_expression_destroy(this->as.phrase.semh);
         break;
      case POLIQARP_EXPRESSION_VARIABLE:
         for (i = 0; i < this->as.variable.n_children; i++)
            poliqarp_expression_destroy(this->as.variable.children[i]);
         free(this->as.variable.children);
         break;
      case POLIQARP_EXPRESSION_INVALID:
         abort(); /* Should not happen. */
   }
   free(this);
}

size_t poliqarp_expression_variable_ranges(
   const struct poliqarp_expression *this, size_t *ranges)
{
   assert(this != NULL);
   switch (this->type) {
      case POLIQARP_EXPRESSION_CONSTANT:
      case POLIQARP_EXPRESSION_PHRASE:
      case POLIQARP_EXPRESSION_VALUE:
         return 0;
      case POLIQARP_EXPRESSION_OR:
      case POLIQARP_EXPRESSION_AND:
         {
            size_t i;
            size_t j;
            i = poliqarp_expression_variable_ranges(this->as.expression.left, ranges);
            j = poliqarp_expression_variable_ranges(this->as.expression.right, ranges);
            return (i > j) ? i : j;
         }
      case POLIQARP_EXPRESSION_VARIABLE:
         ranges[this->as.variable.id] = this->as.variable.n_children;
         return this->as.variable.id;
      case POLIQARP_EXPRESSION_INVALID:
         abort(); /* Should not happen. */
   }
   abort(); /* Should not happen. */
}

enum poliqarp_expression_type poliqarp_expression_type(
   const struct poliqarp_expression *this)
{
   assert(this != NULL);
   switch (this->type) {
      case POLIQARP_EXPRESSION_CONSTANT:
      case POLIQARP_EXPRESSION_PHRASE:
         return this->type;
      case POLIQARP_EXPRESSION_VALUE:
         /* this is ugly (relies on poliqarp_value_compare not being inline function).
          * FIXME: Change at some time */
         if (this->as.value.compare == (poliqarp_value_compare_fn)poliqarp_value_compare &&
            ((struct poliqarp_value *)this->as.value.value)->domain == 
               POLIQARP_DOMAIN_TYPE)
         {
            return POLIQARP_EXPRESSION_PHRASE;
         } else
            return POLIQARP_EXPRESSION_VALUE;
      case POLIQARP_EXPRESSION_OR:
      case POLIQARP_EXPRESSION_AND:
         {
            enum poliqarp_expression_type tleft, tright;
            tleft = poliqarp_expression_type(this->as.expression.left);
            tright = poliqarp_expression_type(this->as.expression.right);
            if (tleft == POLIQARP_EXPRESSION_CONSTANT)
               return tright;
            else if (tright == POLIQARP_EXPRESSION_CONSTANT)
               return tleft;
            else if (tleft == tright)
               return tleft;
            else {
               errno = EINVAL;
               return POLIQARP_EXPRESSION_INVALID;
            }
         }
      case POLIQARP_EXPRESSION_VARIABLE:
         return poliqarp_expression_type(this->as.variable.children[0]);
      case POLIQARP_EXPRESSION_INVALID:
         abort(); /* Should not happen. */
   }
   abort(); /* Should not happen. */
}
