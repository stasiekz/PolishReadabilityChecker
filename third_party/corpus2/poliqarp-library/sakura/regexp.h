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

#ifndef POLIQARP_REGEXP_H
#define POLIQARP_REGEXP_H

#include <sakura/exception.h>

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

/** Don't transform regular expression to "^(%s)$". */
#define POLIQARP_REG_NO_ANCHORS 1

/* If we are using UTF-8 internally, include the headers of TCL regexp library
 * and add some wrappers, otherwise fall back to standard regexp library. */

#ifdef USE_TCL_REGEX

#include <win32/regex/regguts.h>

extern int regcomp(regex_t * preg, const char *regex, int flags);
extern int regexec(const regex_t * preg, const char *string,
   size_t nmatch, regmatch_t pmatch[], int eflags);
#else
#include <regex.h>
#endif /* USE_TCL_REGEX */

/** Nifty Poliqarp regular expressions wrapper. */
struct poliqarp_regexp {
   char *pattern; /**< Text of pattern.           */
   char *message; /**< Error message, if any.     */
   int status;    /**< Status code.               */
   bool simple;   /**< Is strcmp() enough to execute this RE? */
   regex_t preg;  /**< Regular expression object. */
};

/**
 * Check if current locale settings allow match agains UTF-8 strings.
 *
 * @note In case of a non-UTF-8 locale <code>errno</code> is set to
 * <code>EILSEQ</code>.
 *
 * @return 0 for UTF-8 locale, 1 for non-UTF-8 locale, -1 on error.
 */
int poliqarp_regexp_validate_utf8(void);

/**
 * Create a regular expression.
 *
 * @param this pointer to the structure to be initialized.
 * @param pattern NULL-terminated string that follows the POSIX extended
 *        regular expressions syntax
 * @param flags POSIX-specific flags: bitwise-inclusive OR of zero or more of
 *        the following flags: REG_ICASE, REG_NOSUB
 * @param xflags Poliqarp-specific flags: bitwise-inclusive OR of zero or more
 *        of the following flags: POLIQARP_REG_NO_ANCHORS
 *
 * @return 0 on success, a non-zero code otherwise.
 *
 * @see poliqarp_regexp_destroy
 * */
int poliqarp_regexp_create(struct poliqarp_regexp *this, const char *pattern, 
   unsigned int flags, unsigned int xflags);

/** 
 * Free the regular expression.
 *
 * @param this pointer to the structure to be destroyed
 *
 * @see poliqarp_regexp_create
 */
void poliqarp_regexp_destroy(struct poliqarp_regexp *this);

/** Parse regular expression flags. */
void poliqarp_parse_regexp_flags(const char *text, unsigned int *flags, 
   unsigned int *xflags);

/** 
 * Try to match the given string to the pattern and obtain substring match
 * addressing information.
 *
 * Unless <code>REG_NOSUB</code> was set for the compilation of the regular
 * expression, it is possible to obtain substring match addressing information.
 *
 * @param this compiled regular expression
 * @param string string to be matched against the regular expression
 * @param nmatch number of elements of <code>pmatch</code>
 * @param pmatch buffer to store substring match addressing information
 *
 * @see poliqarp_regexp_match
 */
static inline bool poliqarp_regexp_match_ex(const struct poliqarp_regexp *this, 
   const char *string, size_t nmatch, regmatch_t pmatch[])
{
   return regexec(&this->preg, string, nmatch, pmatch, 0) != REG_NOMATCH;
}

/**
 * Try to match the given string to the pattern. 
 *
 * @param this compiled regular expression
 * @param string string to be matched against the regular expression
 *
 * @see poliqarp_regexp_match_ex
 */
static inline bool poliqarp_regexp_match(const struct poliqarp_regexp *this, 
   const char *string)
{
   return poliqarp_regexp_match_ex(this, string, 0, NULL);
}

/**
 * Extract error message from the unsuccessfully compiled regular expression.
 */
void poliqarp_error_from_regexp(struct poliqarp_error *error,
   const struct poliqarp_regexp *this,
   const char *fmt, ...);

#endif
