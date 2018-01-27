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

#include <poliqarp-config.h>

#include <errno.h>

#include <foostring/foostring.h>

#include <sakura/common/args.h>

enum STATE {
   S_error,
   S_start,
   S_numeric,
   S_bareword,
   S_punct,
   S_string
};

enum TOKEN {
   TOK_error = -1,
   TOK_end,
   TOK_numeric,
   TOK_bareword,
   TOK_string,
   TOK_string_error,
   TOK_punct
};

struct lex_context {
   size_t offset;
   size_t size;
   const char *buffer;
   const char *token;
   size_t token_size;
};

static void set_parse_buffer(struct lex_context *this, const char *buffer, 
   size_t size)
{
   this->offset = 0;
   this->size = size;
   this->buffer = buffer;
   this->token = NULL;
   this->token_size = 0;
}

#define set_state(what)                       \
   do {                                       \
      state = what;                           \
      goto loop;                              \
   } while (0)

#define token(what)                           \
   do {                                       \
      this->token_size = buf - this->token;   \
      this->offset = buf - this->buffer;      \
      return what;                            \
   } while (0)

static char *duplicate_token(const struct lex_context *this)
{
   char *result;
   result = malloc(this->token_size + 1);
   if (result == NULL)
      return NULL;
   memcpy(result, this->token, this->token_size);
   result[this->token_size] = 0;
   return result;
}

static enum TOKEN get_token(struct lex_context *this)
{
   const char *buf = this->buffer + this->offset;
   const char *buf_end = this->buffer + this->size;
   enum STATE state = S_start;
   int c;
   int quote_char = 0;

loop:
   switch (state) {
      case S_error: /* ERRORS */
         token(TOK_error);

      case S_start: /* START */
         if (buf == buf_end)
            token(TOK_end);
         c = *buf;

         /* eat white space */
         if (ascii_isspace(c)) {
            for (; buf < buf_end && ascii_isspace(*buf); ++buf) {}
            set_state(S_start);
         }

         /* detect... */

         /* numbers */
         if (ascii_isdigit(c) || c == '-' || c == '+') {
            this->token = buf;
            set_state(S_numeric);
         }

         /* quoted content */
         if (strchr("\"'`", c)) {
            quote_char = c;
            this->token = buf++;
            set_state(S_string);
         }

         /* words */
         if (c == '.' || ascii_isalpha(c)) {
            this->token = buf;
            set_state(S_bareword);
         }

         /* almost everything else */
         if (ascii_isgraph(c)) {
            this->token = buf;
            set_state(S_punct);
         }

         /* @FIXME can we ever get here? */

         this->token = buf;
         set_state(S_error);

      case S_numeric: /* NUMERIC */
         while (buf < buf_end && ascii_isdigit(*buf))
            ++buf;
         if (ascii_isalpha(*buf) || strchr("_-+/.", *buf))
            set_state(S_bareword);
         token(TOK_numeric);

      case S_bareword: /* WORDS */
         while (buf < buf_end && (ascii_isalnum(*buf) || strchr("_-=/.", *buf)))
            ++buf;
         token(TOK_bareword);

      case S_punct: /* PUNCT */
         while (buf < buf_end && (ascii_isgraph(*buf) && !ascii_isalnum(*buf)))
            ++buf;
         token(TOK_punct);

      case S_string: /* QUOTED CONTENT */
         c = -1;
         for (; buf < buf_end && (c = *buf) != quote_char; ++buf)
            if (c == '\\')
               buf++;
         if (c == quote_char) {
            buf++;
            token(TOK_string);
         } else
            token(TOK_string_error);
      default:
         abort(); /* Should not happen. */
   }
}

static char *interpolate(const char *buf, size_t size)
{
   int c;
   char *result_buf;
   char *result;
   const char *buf_end;

   result_buf = result = malloc(size + 1);
   if (result == NULL)
      return NULL;
   buf_end = buf[size - 1] == buf[0] ? buf + size - 1 : buf + size;
   buf += 1;
   switch (buf[-1]) {
      case '/': case '"':
         while (buf < buf_end) {
            c = *buf++;
            if (c == '\\') {
               c = *buf++;
               switch (c) {
                  case 'n':
                     *result++ = '\n';
                     break;
                  case 'r':
                     *result++ = '\r';
                     break;
                  case 't':
                     *result++ = '\t';
                     break;
                  case 'v':
                     *result++ = '\v';
                     break;
                  case 'b':
                     *result++ = '\b';
                     break;
                  case 'a':
                     *result++ = '\a';
                     break;
                  case 'f':
                     *result++ = '\f';
                     break;
                  case 'e':
                     *result++ = 27;
                     break;
                  case 'x':
                     {
                        int n;
                        int old_c;

                        old_c = c = *buf++;
                        if (c >= '0' && c <= '9')
                           c -= '0';
                        else if (c >= 'a' && c <= 'z')
                           c -= 'a';
                        else if (c >= 'A' && c <= 'Z')
                           c -= 'A';
                        else {
                           *result++ = 'x';
                           *result++ = old_c;
                           break;
                        }

                        n = c * 16;

                        c = *buf++;
                        if (c >= '0' && c <= '9')
                           c -= '0';
                        else if (c >= 'a' && c <= 'z')
                           c -= 'a';
                        else if (c >= 'A' && c <= 'Z')
                           c -= 'A';
                        else {
                           *result++ = 'x';
                           *result++ = old_c;
                           *result++ = c;
                           break;
                        }

                        n += c;

                        *result++ = n;
                     }
                  default:
                     *result++ = c;
               }
            } else
               *result++ = c;
         }
         break;
      case '\'':
         while (buf < buf_end) {
            c = *buf++;
            if (c == '\\')
               *result++ = *buf++;
            else
               *result++ = c;
         }
         break;
      case '`':
         while (buf < buf_end) {
            c = *buf++;
            if (c == '\\') {
               c = *buf++;
               switch (c) {
               case 'n':
                  *result++ = '\n';
                  break;
               case 'r':
                  *result++ = '\r';
                  break;
               case 't':
                  *result++ = '\t';
                  break;
               case 'v':
                  *result++ = '\v';
                  break;
               case 'b':
                  *result++ = '\b';
                  break;
               case 'a':
                  *result++ = '\a';
                  break;
               case 'f':
                  *result++ = '\f';
                  break;
               case 'e':
                  *result++ = 27;
                  break;
               case 'x':
                  {
                     int n;
                     int old_c;

                     old_c = c = *buf++;
                     if (c >= '0' && c <= '9')
                        c -= '0';
                     else if (c >= 'a' && c <= 'z')
                        c -= 'a';
                     else if (c >= 'A' && c <= 'Z')
                        c -= 'A';
                     else {
                        *result++ = 'x';
                        *result++ = old_c;
                        break;
                     }

                     n = c * 16;

                     c = *buf++;
                     if (c >= '0' && c <= '9')
                        c -= '0';
                     else if (c >= 'a' && c <= 'z')
                        c -= 'a';
                     else if (c >= 'A' && c <= 'Z')
                        c -= 'A';
                     else {
                        *result++ = 'x';
                        *result++ = old_c;
                        *result++ = c;
                        break;
                     }

                     n += c;

                     *result++ = n;
                  }
               default:
                  *result++ = c;
               }
            } else
               *result++ = c;
            /* eat whitespace */
            if (ascii_isspace(result[-1])) {
               if (ascii_isspace(*result_buf))
                  --result;
               else if (result > result_buf + 1 && ascii_isspace(result[-1])
                      && ascii_isspace(result[-2])) {
                  --result;
                  *result = ' ';
               }
            }
         }
         if (result > result_buf && ascii_isspace(result[-1]))
            --result;
         break;
   }
   *result++ = 0;
   return result_buf;
}


void args_init(struct text_args *this)
{
   this->num_items = 0;
   this->capacity = 0;
   this->item = NULL;
}

int args_parse(struct text_args *this, const char *text)
{
   struct lex_context context;
   size_t i;

   for (i = 0; i < this->num_items; ++i)
      free(this->item[i].value);

   set_parse_buffer(&context, text, strlen(text));

   for (this->num_items = 0;; this->num_items++) {
      struct text_value *this_item;

      if (this->num_items >= this->capacity) {
         struct text_value *new_items;
         this->capacity = this->capacity * 2 + 1;
         if (this->capacity > SIZE_MAX / sizeof *this->item) {
            errno = ENOMEM;
            return -1;
         }
         new_items = realloc(this->item, this->capacity * sizeof *this->item);
         if (new_items == NULL)
            return -1;
         else
            this->item = new_items;
      }
      this_item = this->item + this->num_items;
      switch (get_token(&context)) {
         case 0:
            return 0;
         case TOK_bareword:
         case TOK_punct:
            this_item->value = duplicate_token(&context);
            this_item->flags = 0;
            break;
         case TOK_numeric:
            this_item->value = duplicate_token(&context);
            this_item->flags = TV_NUMERIC;
            break;
         case TOK_string:
            this_item->value = interpolate(context.token, context.token_size);
            this_item->flags = 0;
            break;
         case TOK_string_error:
            this_item->value = interpolate(context.token, context.token_size);
            this_item->flags = TV_ERROR;
            break;
         case TOK_error:
            return 0;
      }
      if (this_item->value == NULL)
         return -1;
   }
}

void args_free(struct text_args *this)
{
   size_t i;
   for (i = 0; i < this->num_items; ++i)
      free(this->item[i].value);
   free(this->item);
}
