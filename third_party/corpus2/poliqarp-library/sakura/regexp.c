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
#include <string.h>

#include <sakura/regexp.h>

#define C1 "a"
#define C2 "\xce\xb1" /* GREEK SMALL LETTER ALPHA */
#define C3 "\xe0\xa4\x85" /* DEVANAGARI LETTER A */
#define C4 "\xf4\x80\x80\x80" /* Plane 15 Private Use, First */

int poliqarp_regexp_validate_utf8()
{
   int rc;
   regex_t preg;
   const char *string = C1 C2 C3 C4;
   const char *regex = "^" C1 "[" C2 "][" C3 "][" C4 "]$";
   rc = regcomp(&preg, regex, REG_EXTENDED | REG_NOSUB);
   /* A match is expected for UTF-8 encoding *only*. */
   if (rc != 0) {
      errno = (rc == REG_ESPACE) ? ENOMEM : EINVAL;
      return -1;
   }
   rc = regexec(&preg, string, 0, NULL, 0);
   regfree(&preg);
   if (rc != 0) {
      errno = EILSEQ;
      return 1;
   }
   return 0;
}

#undef C1
#undef C2
#undef C3
#undef C4

/* create a regular expression */
int poliqarp_regexp_create(struct poliqarp_regexp *this, const char *pattern, 
   unsigned int flags, unsigned int xflags)
{
   char *use_pattern;
   bool remade_pattern = false;

   if (xflags & POLIQARP_REG_NO_ANCHORS) {
      use_pattern = (char *)pattern;
      this->simple = false;
   } else {
      use_pattern = malloc(strlen(pattern) + 5);
      sprintf(use_pattern, "^(%s)$", pattern);
      remade_pattern = true;
      this->simple = (flags & REG_ICASE) ? false :
         (strpbrk(pattern, "|*+?{,}()^$.\\-[]=:\"") == NULL);
   }
   this->pattern = strdup(pattern);
   this->status = regcomp(&this->preg, use_pattern, flags | REG_EXTENDED);
   if (remade_pattern)
      free(use_pattern);
   if (this->status) {
      size_t size;
      size = regerror(this->status, &this->preg, 0, 0);
      this->message = malloc(size);
      if (this->message != NULL)
         regerror(this->status, &this->preg, this->message, size);
   } else
      this->message = NULL;
   return this->status;
}

/* free the regular expression */
void poliqarp_regexp_destroy(struct poliqarp_regexp *this)
{
   if (this->status == 0)
      regfree(&this->preg);
   free(this->pattern);
   free(this->message);
}

void poliqarp_parse_regexp_flags(const char *text, unsigned int *flags, 
   unsigned int *xflags)
{
   unsigned result = *flags;    /* this provides continuity but requires 
                                   initialization by the caller */
   unsigned xresult = *xflags;
   int c;

   while ((c = *text++))
      switch (c) {
         case 'i':
            result |= REG_ICASE;
            break;
         case 'I':
            result &= ~REG_ICASE;
            break;
         case 'x':
            xresult |= POLIQARP_REG_NO_ANCHORS;
            break;
         case 'X':
            xresult &= ~POLIQARP_REG_NO_ANCHORS;
            break;
         default:
            break;
      }
   *flags = result;
   *xflags = xresult;
}
