/*
 * This file is part of the Poliqarp suite.
 *
 * Copyright (C) 2008-2009 by Instytut Podstaw Informatyki Polskiej
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
 * Check if the currect locale settings are appropriate for
 * unibits_afronte_strcoll and its counterparts.
 *
 * @note In case of an unsupported locale, <code>errno</code> is set to
 * <code>EILSEQ</code>.
 *
 * @return 0 for a supported locale, 1 for an unsupported one, -1 on error.
 *
 * @see unibits_afronte_strcoll
 * @see unibits_afronte_stricoll
 * @see unibits_atergo_strcoll
 * @see unibits_atergo_stricoll
 */
int unibits_validate_utf8(void);


/**
 * Compare two UTF-8 strings. Return an integer less than, equal to, or greater
 * than zero if s1 is found, respectively, to be less than, to match, or be
 * greater than s2.
 *
 * @note The comparison is based on strings interpreted as appropriate for the
 * current locale for category LC_COLLATE.
 *
 * @note For Unix systems, the character encoding used in the current locale is
 * expected to be UTF-8. For Windows system, this function is not dependent
 * upon the locale character set.
 */
int unibits_afronte_strcoll(const char *s1, const char *s2);

/**
 * Compare two UTF-8 encoded strings, ignoring the case of the characters.
 * Return an integer less than, equal to, or greater than zero if s1 is found,
 * respectively, to be less than, to match, or be greater than s2.
 *
 * @note The comparison is based on strings interpreted as appropriate for the
 * current locale for category LC_COLLATE.
 *
 * @note For Unix systems, the character encoding used in the current locale is
 * expected to be UTF-8. For Windows systems, this function is not dependent
 * upon the locale character set.
 */
int unibits_afronte_stricoll(const char *s1, const char *s2);

/**
 * Compare two UTF-8 encoded strings, using reverse order of characters.
 * Return an integer less than, equal to, or greater than zero if s1 is found,
 * respectively, to be less than, to match, or be greater than s2.
 *
 * @note The comparison is based on strings interpreted as appropriate for the
 * current locale for category LC_COLLATE.
 *
 * @note For Unix systems, the character encoding used in the current locale is
 * expected to be UTF-8. For Windows systems, this function is not dependent
 * upon the locale character set.
 */
int unibits_atergo_strcoll(const char *s1, const char *s2);

/**
 * Compare two UTF-8 encoded strings, using reverse order of characters,
 * ignoring the case of the characters. Return an integer less than, equal to,
 * or greater than zero if s1 is found, respectively, to be less than, to
 * match, or be greater than s2.
 *
 * @note The comparison is based on strings interpreted as appropriate for the
 * current locale for category LC_COLLATE.
 *
 * @note For Unix systems, the character encoding used in the current locale is
 * expected to be UTF-8. For Windows systems, this function is not dependent
 * upon the locale character set.
 */
int unibits_atergo_stricoll(const char *s1, const char *s2);
