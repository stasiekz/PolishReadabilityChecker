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

#ifndef FOOSTRING_H
#define FOOSTRING_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/** 
 * A simple textual type enabling the user to create arbitrarily long
 * strings without having to care for the length of the buffer.  The
 * string is available via the field `str'.  One should not access
 * this field directly.  Instead, the macro string_str() should be
 * used.
 *
 * @par Example code:
 * @code
 *     string_t s;
 *
 *     s = string_init("ala");
 *     string_append_char(s, ' ');
 *     string_append_str(s, "ma kota");
 *     printf("%s\n", string_str(s));
 *     string_free(s);
 * @endcode
 */
struct string_t {
   char *str;   /**< Buffer of characters */
   size_t len;  /**< Number of characters used */
   size_t size; /**< Size of the buffer in bytes */
};

/**
 * A typedef to the string_t structure.
 * @note For the programmer's convenience, this type is an alias for a
 * pointer to struct string_t.
 */
typedef struct string_t *string_t;

/**
 * This function will be called in an out-of-memory situation. By default it
 * does nothing.
 */
extern void (*string_oom_callback)(string_t);

/**
 * Creates and returns a new, empty string.
 *
 * @return a newly created string, or NULL in case of error
 */
string_t string_create(void);

/**
 * Creates and returns a string containing a copy of the given text.
 *
 * @note The initial text should be non-null.  Use string_create() to
 * create empty strings.
 *
 * @return a newly created string, or NULL in case of error
 */
string_t string_init(const char *c_string);

/**
 * Creates and returns a string containing a copy of at most n initial
 * characters of the given text. 
 *
 * @note The initial text should be non-null.  Use string_create() to
 * create empty strings.
 *
 * @return a newly created string, or NULL in case of error
 */
string_t string_init_n(const char *c_string, size_t n);

/**
 * Frees a string and deallocates its buffer.
 */
void string_free(string_t s);

/**
 * Frees a string structure, but returns its text buffer which should
 * be freed separately via free().
 */
char *string_free_and_get_buffer(string_t s);

/**
 * Affixes s2 at the end of s.
 *
 * @return 0 on success, -1 on error
 */
int string_append(string_t s, const string_t s2);

/**
 * Affixes the given buffer at the end of a string.
 *
 * @return 0 on success, -1 on error
 */
int string_append_buf(string_t s, const char *buffer, size_t count);

/**
 * Affixes the given C string at the end of a string.
 *
 * @return 0 on success, -1 on error
 */
int string_append_str(string_t s, const char *c_string);

/**
 * Appends at most n bytes of a given C string at the end of a string.
 *
 * @return 0 on success, -1 on error
 */
int string_append_strn(string_t s, const char *c_string, size_t n);

/**
 * Appends a given character at the end of a string.
 *
 * @return 0 on success, -1 on error
 */
int string_append_char(string_t s, char c);

/**
 * Appends a decimal value at the end of a string.
 *
 * @return 0 on success, -1 on error
 */
int string_append_long(string_t s, long value);

/**
 * Appends an unsigned decimal value at the end of a string.
 *
 * @return 0 on success, -1 on error
 */
int string_append_unsigned_long(string_t s, unsigned long value);

/**
 * Reads a line from the file.
 *
 * @return a newly created string, or NULL in case of error or if were no data
 *         to read
 */
string_t string_fgets(FILE *f);

/** 
 * Compares two strings (with semantic as in strcmp()). 
 */
int string_cmp(const string_t s1, const string_t s2);

/** 
 * Compares a string with a null-terminated string (with semantic as in strcmp()). 
 */
int string_ccmp(const string_t s1, const char *s2);

/* -------------------------------------------------- */

/**
 * Inserts at most count characters of a given C string at the given
 * place in the string buffer.
 *
 * @note When count == (size_t) -1, the entire C string is inserted.
 *
 * @return 0 on success, -1 on error
 */
int string_insert_strn(string_t s, size_t index, const char *c_string,
   size_t count);

/**
 * Insert a given C string at the given place in the string buffer.
 *
 * @return 0 on success, -1 on error
 */
int string_insert_str(string_t s, size_t index, const char *c_string);

/**
 * Clears the text buffer without freeing the structure's memory.
 */
void string_clear(string_t s);


/**
 * Gets the string buffer.
 */
static inline char* string_str(string_t s)
{
   return s->str;
}

/**
 * Gets the string length.
 */
static inline size_t string_len(string_t s)
{
   return s->len;
}

/**
 * Formats a text buffer according to the given pattern.  The format
 * is similar to printf() and friends, but it handles only the
 * following specifiers:
 * <dl>
 * <dt>%c</dt>
 * <dd>Interprets the argument as a single character</dd>
 * <dt>%s</dt>
 * <dd>Interprets the argument as a C string (char *)</dd>
 * <dt>%S</dt>
 * <dd>Interprets the argument as a string_t object</dd>
 * <dt>%d</dt>
 * <dd>Interprets the argument as an <code>int</code></dd>
 * <dt>%u</dt>
 * <dd>Interprets the argument as an <code>unsigned int</code></dd>
 * <dt>%ld</dt>
 * <dd>Interprets the argument as an <code>long</code></dd>
 * <dt>%lu</dt>
 * <dd>Interprets the argument as an <code>unsigned long</code></dd>
 * <dt>%%</dt>
 * <dd>Inserts a literal percent character
 * </dl>
 *
 * @return 0 on success, -1 on error
 */
int string_format(string_t s, const char *format, ...);

/**
 * Formats text and appends it at the end of the given string.
 *
 * @return 0 on success, -1 on error
 */
int string_format_append(string_t s, const char *format, ...);

/**
 * Similar to string_format_append(), but the argument list is passed
 * as a va_list.
 *
 * @return 0 on success, -1 on error
 */
int string_vformat_append(string_t s, const char *format, va_list ap);

/**
 * Similar to string_format(), but the argument list is passed as a va_list.
 *
 * @return 0 on success, -1 on error
 */
int string_vformat(string_t s, const char *format, va_list ap);

/* -------------------------------------------------- */

/**
 * Uses string_format() to create a C string that should be freed
 * using free().
 *
 * @return 0 on success, -1 on error
 */
char *string_aformat(const char *format, ...);

/**
 * Similar to string_aformat(), but the argument list is passed as a
 * va_list.
 *
 * @return 0 on success, -1 on error
 */
char *string_avformat(const char *format, va_list ap);

/* -------------------------------------------------- */

/**
 * Works like printf(), but interprets its arguments in the same way
 * string_format() does.
 *
 * @return 0 on success, -1 on error
 */
int string_printf(const char *format, ...);

/**
 * Works like fprintf(), but interprets its arguments in the same way
 * string_format() does.
 *
 * @return 0 on success, -1 on error
 */
int string_fprintf(FILE *stream, const char *format, ...);

/**
 * Works like vprintf(), but interprets its arguments in the same way
 * string_format() does.
 *
 * @return 0 on success, -1 on error
 */
int string_vprintf(const char *format, va_list ap);

/**
 * Works like vfprintf(), but interprets its arguments in the same way
 * string_format() does.
 *
 * @return 0 on success, -1 on error
 */
int string_vfprintf(FILE *stream, const char *format, va_list ap);

/**
 * Clears the string without strinking the string buffer.
 */
void string_purge(string_t str);


/**
 * An ASCII variant of isspace().
 */
static inline int ascii_isspace(char c)
{
   return
      c == ' '  || c == '\f' || c == '\n' ||
      c == '\r' || c == '\t' || c == '\v';
}

/**
 * An ASCII variant of isalpha().
 */
static inline int ascii_isalpha(char c)
{
   return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

/**
 * An ASCII variant of isdigit().
 */
static inline int ascii_isdigit(char c)
{
   return c >= '0' && c <= '9';
}

/**
 * An ASCII variant of isalnum().
 */
static inline int ascii_isalnum(char c)
{
   return ascii_isalpha(c) || ascii_isdigit(c);
}

/**
 * An ASCII variant of isgraph().
 */
static inline int ascii_isgraph(char c)
{
   return c > ' ' && c < '\x7f';
}

/**
 * An ASCII variant of toupper()
 */
static inline char ascii_toupper(char c)
{
   if (c >= 'a' && c <= 'z')
      c += 'A' - 'a';
   return c;
}

/**
 * An ASCII variant of tolower()
 */
static inline char ascii_tolower(char c)
{
   if (c >= 'A' && c <= 'Z')
      c += 'a' - 'A';
   return c;
}

#endif /* FOOSTRING_H */
