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

#include <sakura/value-interp.h>

static void poliqarp_value_interp_create__disamb(struct poliqarp_value *this,
   const struct poliqarp_corpus *corpus, const struct poliqarp_value *value)
{
   size_t i;
   unsigned *bits = value->bits;
   const struct poliqarp_binary_interp *interp;
   const struct poliqarp_binary_interp *interp_end;

   switch (value->domain) {
      case POLIQARP_DOMAIN_BASE:
         switch (value->strategy) {
            case POLIQARP_STRATEGY_ANY:
               for (i = 0; i < this->num_items; ++i) {
                  interp = poliqarp_backend_interp_fetch__disamb(&corpus->interp, 
                     i);
                  interp_end = interp + 
                     poliqarp_backend_interp_length__disamb(&corpus->interp, i);
                  if (poliqarp_value_micro_eval_base_any(bits, interp, 
                     interp_end)) 
                  {
                     BIT_ARRAY_SET(this->bits, i);
                     ++this->num_hits;
                  }
               }
               break;
            case POLIQARP_STRATEGY_ALL:
               for (i = 0; i < this->num_items; ++i) {
                  interp = poliqarp_backend_interp_fetch__disamb(&corpus->interp, 
                     i);
                  interp_end = interp +
                     poliqarp_backend_interp_length__disamb(&corpus->interp, i);
                  if (poliqarp_value_micro_eval_base_all(bits, interp, 
                     interp_end)) 
                  {
                     BIT_ARRAY_SET(this->bits, i);
                     ++this->num_hits;
                  }
               }
               break;
            }
         break;
      case POLIQARP_DOMAIN_TAG:
         switch (value->strategy) {
            case POLIQARP_STRATEGY_ANY:
               for (i = 0; i < this->num_items; ++i) {
                  interp = poliqarp_backend_interp_fetch__disamb(&corpus->interp, 
                     i);
                  interp_end = interp +
                     poliqarp_backend_interp_length__disamb(&corpus->interp, i);
                  if (poliqarp_value_micro_eval_tag_any(bits, interp, interp_end)) 
                  {
                     BIT_ARRAY_SET(this->bits, i);
                     ++this->num_hits;
                  }
               }
               break;
            case POLIQARP_STRATEGY_ALL:
               for (i = 0; i < this->num_items; ++i) {
                  interp = poliqarp_backend_interp_fetch__disamb(&corpus->interp, 
                     i);
                  interp_end = interp +
                     poliqarp_backend_interp_length__disamb(&corpus->interp, i);
                  if (poliqarp_value_micro_eval_tag_all(bits, interp, interp_end)) 
                  {
                     BIT_ARRAY_SET(this->bits, i);
                     ++this->num_hits;
                  }
               }
               break;
            }
         break;
      default:
         abort(); /* Should not happen. */
   }
}

static void poliqarp_value_interp_create__amb(struct poliqarp_value *this,
   const struct poliqarp_corpus *corpus, const struct poliqarp_value *value)
{
   size_t i;
   unsigned *bits = value->bits;
   const struct poliqarp_binary_interp *interp;
   const struct poliqarp_binary_interp *interp_end;

   switch (value->domain) {
      case POLIQARP_DOMAIN_BASE:
         switch (value->strategy) {
            case POLIQARP_STRATEGY_ANY:
               for (i = 0; i < this->num_items; ++i) {
                  interp = poliqarp_backend_interp_fetch__amb(&corpus->interp, i);
                  interp_end = interp + 
                     poliqarp_backend_interp_length__amb(&corpus->interp, i);
                  if (poliqarp_value_micro_eval_base_any(bits, interp, 
                     interp_end)) 
                  {
                     BIT_ARRAY_SET(this->bits, i);
                     ++this->num_hits;
                  }
               }
               break;
            case POLIQARP_STRATEGY_ALL:
               for (i = 0; i < this->num_items; ++i) {
                  interp = poliqarp_backend_interp_fetch__amb(&corpus->interp, i);
                  interp_end = interp + 
                     poliqarp_backend_interp_length__amb(&corpus->interp, i);
                  if (poliqarp_value_micro_eval_base_all(bits, interp, 
                     interp_end)) 
                  {
                     BIT_ARRAY_SET(this->bits, i);
                     ++this->num_hits;
                  }
               }
               break;
         }
         break;
      case POLIQARP_DOMAIN_TAG:
         switch (value->strategy) {
            case POLIQARP_STRATEGY_ANY:
               for (i = 0; i < this->num_items; ++i) {
                  interp = poliqarp_backend_interp_fetch__amb(&corpus->interp, i);
                  interp_end = interp + 
                     poliqarp_backend_interp_length__amb(&corpus->interp, i);
                  if (poliqarp_value_micro_eval_tag_any(bits, interp, interp_end)) 
                  {
                     BIT_ARRAY_SET(this->bits, i);
                     ++this->num_hits;
                  }
               }
               break;
            case POLIQARP_STRATEGY_ALL:
               for (i = 0; i < this->num_items; ++i) {
                  interp = poliqarp_backend_interp_fetch__amb(&corpus->interp, i);
                  interp_end = interp + 
                     poliqarp_backend_interp_length__amb(&corpus->interp, i);
                  if (poliqarp_value_micro_eval_tag_all(bits, interp, interp_end)) 
                  {
                     BIT_ARRAY_SET(this->bits, i);
                     ++this->num_hits;
                  }
               }
               break;
         }
         break;
      default:
         abort(); /* Should not happen. */
   }
}

struct poliqarp_value *poliqarp_value_interp_create(const struct poliqarp_corpus
    *corpus, const struct poliqarp_value *value)
{
   struct poliqarp_value *this;

   /* do the allocation */
   this = malloc(sizeof *this);
   this->num_items = value->use_disamb ?
      poliqarp_backend_interp_num_items__disamb(&corpus->interp) : 
      poliqarp_backend_interp_num_items__amb(&corpus->interp);
   this->num_bytes = BIT_ARRAY_LENGTH_BYTES(this->bits, this->num_items);
   this->num_hits = 0;
   this->bits = malloc(this->num_bytes);
   this->strategy = value->strategy;
   this->domain = value-> use_disamb ? POLIQARP_DOMAIN_INTERP__DISAMB : 
      POLIQARP_DOMAIN_INTERP__AMB;

   /* clear the bit field */
   memset(this->bits, 0, this->num_bytes);

   if (value->use_disamb)
      poliqarp_value_interp_create__disamb(this, corpus, value);
   else
      poliqarp_value_interp_create__amb(this, corpus, value);
   return this;
}
