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

%{

#include <poliqarp-config.h>

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include <sakura/corpus.h>
#include <sakura/expression.h>
#include <sakura/value.h>
#include <sakura/meta-value.h>
#include <sakura/common/graph.h>
#include <sakura/parser.h>
#include <foostring/foostring.h>
#include <unibits/unibits.h>

static int string_to_int(const char *s)
{
   long result;
   char *end_ptr;
   errno = 0;
   result = strtol(s, &end_ptr, 10);
   if (result < INT_MIN)
      result = INT_MIN;
   if (result > INT_MAX || *end_ptr != '\0')
      result = INT_MAX;
   return result;
}

%}

%x META_MODE
%x STAT_MODE
%x STAT_SORT_MODE
%x STAT_INTERP_MODE
%x DQUOTE
%x SQUOTE
%option noyywrap
%option reentrant bison-bridge
%option 8bit
%option nounput
%option stack

DIGIT [[:digit:]]
SPACE [[:space:]]+

/* Note: flex (not to mention the other lexes) is brain damaged. It doesn't
 * use the regex library (or regex facilities of libc), thus the regular
 * expression below does not support multibyte characters. Because we expect
 * UTF-8 input, and because the characters 128-255 can only be parts of
 * multibyte characters above U+0080, we explicitly allow them all to be
 * parts of identifiers. */
ID [:_.a-zA-Z0-9\x80-\xff-]
ID_EXT [*?+]

%%

   string_t string_buf;
   string_buf = string_create(); /* just to shut up compilers */
   string_free(string_buf);

<INITIAL>(?i:meta) {
   BEGIN(META_MODE);
   return META;
}

<INITIAL,META_MODE>(?i:within) {
   return WITHIN;
}

<INITIAL,META_MODE>(?i:group{SPACE}by) {
   BEGIN(STAT_MODE);
   return GROUP_BY;
}

<INITIAL>(?i:head) {
   return HEAD;
}

<INITIAL>(?i:synh) {
   return SYNH;
}

<INITIAL>(?i:semh) {
   return SEMH;
}

<STAT_MODE>(i?:interp) {
   BEGIN(STAT_INTERP_MODE);
   return STAT_INTERP;
}

<STAT_INTERP_MODE>(i?:random) {
   BEGIN(STAT_MODE);
   return STAT_INTERP_RANDOM;
}

<STAT_INTERP_MODE>(i?:combine) {
   BEGIN(STAT_MODE);
   return STAT_INTERP_COMBINE;
}

<STAT_MODE>(i?:sort{SPACE}a{SPACE}fronte) {
   return STAT_SORT_AFRONTE;
}

<STAT_MODE>(?i:sort{SPACE}by) {
   BEGIN(STAT_SORT_MODE);
   return STAT_SORT_BY;
}

<STAT_SORT_MODE>(?i:freq) {
   BEGIN(STAT_MODE);
   return STAT_SORT_BY_FREQ;
}

<STAT_SORT_MODE>(?i:cp) {
   BEGIN(STAT_MODE);
   return STAT_SORT_BY_CP;
}

<STAT_SORT_MODE>(?i:scp) {
   BEGIN(STAT_MODE);
   return STAT_SORT_BY_SCP;
}

<STAT_SORT_MODE>(?i:maxcp) {
   BEGIN(STAT_MODE);
   return STAT_SORT_BY_MAXCP;
}

<STAT_SORT_MODE>(?i:dice) {
   BEGIN(STAT_MODE);
   return STAT_SORT_BY_DICE;
}

<STAT_MODE>(?i:bias) {
   return STAT_BIAS;
}

<STAT_MODE>(?i:min) {
   return STAT_MIN;
}

<STAT_MODE>(?i:count) {
   return STAT_COUNT;
}

<META_MODE>"<" {
   yylval->as_mop.strategy = POLIQARP_STRATEGY_SMALLER;
   yylval->as_mop.negate = false;
   return MOP;
}

<META_MODE>"<=" {
   yylval->as_mop.strategy = POLIQARP_STRATEGY_SMALLER_EQUAL;
   yylval->as_mop.negate = false;
   return MOP;
}

<META_MODE>">" {
   yylval->as_mop.strategy = POLIQARP_STRATEGY_GREATER;
   yylval->as_mop.negate = false;
   return MOP;
}

<META_MODE>">=" {
   yylval->as_mop.strategy = POLIQARP_STRATEGY_GREATER_EQUAL;
   yylval->as_mop.negate = false;
   return MOP;
}

<META_MODE>"=" {
   yylval->as_mop.strategy = POLIQARP_STRATEGY_EQUAL;
   yylval->as_mop.negate = false;
   return MOP;
}

<META_MODE>"!=" {
   yylval->as_mop.strategy = POLIQARP_STRATEGY_EQUAL;
   yylval->as_mop.negate = true;
   return MOP;
}

<INITIAL>[$]{DIGIT}+ {
   yylval->as_int = string_to_int(yytext + 1);
   return VARIABLE;
}

<INITIAL,STAT_MODE>{DIGIT}+ {
   yylval->as_int = string_to_int(yytext);
   return INTEGER;
}

<STAT_MODE>[+-]{DIGIT}+ {
   yylval->as_int = string_to_int(yytext);
   return SIGNED_INTEGER;
}

<STAT_MODE>[+-]?{DIGIT}+[.]{DIGIT}+ {
   char *end_ptr;
   errno = 0;
   yylval->as_double = strtod(yytext, &end_ptr);
   if (*end_ptr != '\0' || errno == ERANGE)
      yylval->as_double = NAN;
   return REAL_NUMBER;
}

<INITIAL,META_MODE,STAT_MODE>{ID}+({ID}|{ID_EXT})* {
   yylval->as_text = strdup(yytext);
   return IDENT;
}

<INITIAL>~ {
   yylval->as_op.strategy = POLIQARP_STRATEGY_ANY;
   yylval->as_op.use_disamb = false;
   yylval->as_op.negate = false;
   return OP;
}

<INITIAL>!~ {
   yylval->as_op.strategy = POLIQARP_STRATEGY_ANY;
   yylval->as_op.use_disamb = false;
   yylval->as_op.negate = true;
   return OP;
}

<INITIAL>~~ {
   yylval->as_op.strategy = POLIQARP_STRATEGY_ALL;
   yylval->as_op.use_disamb = false;
   yylval->as_op.negate = false;
   return OP;
}

<INITIAL>!~~ {
   yylval->as_op.strategy = POLIQARP_STRATEGY_ALL;
   yylval->as_op.use_disamb = false;
   yylval->as_op.negate = true;
   return OP;
}

<INITIAL>= {
   yylval->as_op.strategy = POLIQARP_STRATEGY_ANY;
   yylval->as_op.use_disamb = true;
   yylval->as_op.negate = false;
   return OP;
}

<INITIAL>!= {
   yylval->as_op.strategy = POLIQARP_STRATEGY_ANY;
   yylval->as_op.use_disamb = true;
   yylval->as_op.negate = true;
   return OP;
}

<INITIAL>== {
   yylval->as_op.strategy = POLIQARP_STRATEGY_ALL;
   yylval->as_op.use_disamb = true;
   yylval->as_op.negate = false;
   return OP;
}

<INITIAL>!== {
   yylval->as_op.strategy = POLIQARP_STRATEGY_ALL;
   yylval->as_op.use_disamb = true;
   yylval->as_op.negate = true;
   return OP;
}

<INITIAL,META_MODE,STAT_MODE>"\'" {
   yy_push_state(SQUOTE, yyscanner);
   string_buf = string_create();
}

<INITIAL,META_MODE,STAT_MODE>"\"" {
   yy_push_state(DQUOTE, yyscanner);
   string_buf = string_create();
}

<DQUOTE>\" |
<SQUOTE>\' {
   yy_pop_state(yyscanner);
   yylval->as_text = string_free_and_get_buffer(string_buf);
   return STRING;
}

<SQUOTE,DQUOTE>{
   "\\n" string_append_char(string_buf, '\n');
   "\\r" string_append_char(string_buf, '\r');
   "\\t" string_append_char(string_buf, '\t');
   "\\v" string_append_char(string_buf, '\v');
   "\\b" string_append_char(string_buf, '\b');
   "\\f" string_append_char(string_buf, '\f');
   "\\\\" string_append_char(string_buf, '\\');
   "\\\"" string_append_char(string_buf, '\"');
   "\\\'" string_append_char(string_buf, '\'');
   "\\\n" string_append_char(string_buf, yytext[1]);
   \\(x[0-9a-fA-F]{2}|u[0-9a-fA-F]{4}|U00[0-9a-fA-F]{6}) {
      unsigned long code = strtoul(yytext + 2, NULL, 16);
      if (code == 0 || code > 0x10ffff)
         code = 0xfffd;
      Tcl_UniChar ch = code; /* no overflow expected */
      Tcl_DString dstring;
      Tcl_DStringInit(&dstring);
      char * string = Tcl_UniCharToUtfDString(&ch, 1, &dstring);
      string_append_str(string_buf, string);
      Tcl_DStringFree(&dstring);
   }
   \\. string_append_strn(string_buf, yytext, yyleng);
}

<DQUOTE>[^\\\n\"]+ |
<SQUOTE>[^\\\n\']+ {
   string_append_strn(string_buf, yytext, yyleng);
}

<SQUOTE,DQUOTE>{
   \n |
   <<EOF>> {
      yy_pop_state(yyscanner);
      yylval->as_text = string_free_and_get_buffer(string_buf);
      return STRING_OPEN;
   }
}

<INITIAL,META_MODE,STAT_MODE>{SPACE}

<<EOF>> {
   BEGIN(INITIAL);
   yyterminate();
}

<INITIAL,META_MODE,STAT_MODE>. {
   return *yytext;
}

%%
