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

#include <sakura/value.h>

bool poliqarp_value_can_be_optimized(const struct poliqarp_value *this,
   const struct poliqarp_value *that, enum poliqarp_logic_operator oper)
{
   /* TODO */
   return false;
}

struct poliqarp_value *poliqarp_value_optimize(const struct poliqarp_value *this,
   const struct poliqarp_value *that, enum poliqarp_logic_operator oper)
{
   abort(); /* See the TODO above. */
}

int poliqarp_value_compare(const struct poliqarp_value *this, 
   const struct poliqarp_value *that)
{
   return this->domain == that->domain && this->strategy == that->strategy &&
      this->num_hits == that->num_hits ? 
      memcmp(this->bits, that->bits, this->num_bytes) : -1;
}

void poliqarp_value_destroy(struct poliqarp_value *this)
{
   free(this->bits);
   free(this);
}

void poliqarp_value_use_strategy(struct poliqarp_value *this, 
   enum poliqarp_match_strategy strategy)
{
   this->strategy = strategy;
}

void poliqarp_value_use_disamb(struct poliqarp_value *this, bool use_disamb)
{
   this->use_disamb = use_disamb;
}
