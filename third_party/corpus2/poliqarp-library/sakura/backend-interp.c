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

#include <sakura/backend-interp.h>
#include <sakura/dict.h>
#include <sakura/exception.h>

#include <foostring/foostring.h>

int poliqarp_backend_interp_open(struct poliqarp_backend_interp *this, 
   const char *base_name, struct poliqarp_error *error)
{
   int rc;
   rc = poliqarp_newdict_open(&this->dict__disamb, base_name, false,
      POLIQARP_INTERP1_IMAGE_FORMAT, POLIQARP_INTERP1_OFFSET_FORMAT,
      "Unable to open interp-1 dictionary", error);
   if (rc != 0)
      return rc;
   rc = poliqarp_newdict_open(&this->dict__amb, base_name, false,
      POLIQARP_INTERP2_IMAGE_FORMAT, POLIQARP_INTERP2_OFFSET_FORMAT,
      "Unable to open interp-2 dictionary", error);
   if (rc != 0) {
      newdict_close(&this->dict__disamb);
      return rc;      
   }
   return 0;
}

void poliqarp_backend_interp_close(struct poliqarp_backend_interp *this)
{
   newdict_close(&this->dict__disamb);
   newdict_close(&this->dict__amb);
}

