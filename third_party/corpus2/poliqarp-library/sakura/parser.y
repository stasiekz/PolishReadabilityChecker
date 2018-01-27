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

#include <errno.h>
#include <math.h>

#include <sakura/lexer.h>
#include <sakura/corpus.h>
#include <sakura/query.h>
#include <sakura/expression.h>
#include <sakura/value.h>
#include <sakura/value-orth.h>
#include <sakura/value-space.h>
#include <sakura/value-base.h>
#include <sakura/value-tag.h>
#include <sakura/value-pos.h>
#include <sakura/value-attr.h>
#include <sakura/value-interp.h>
#include <sakura/value-type.h>
#include <sakura/meta-value.h>
#include <sakura/common/graph.h>
#include <sakura/common/hash-table.h>

/* Called by yyparse on error */
void yyerror(yyscan_t scanner, struct poliqarp_query *query, const char *message)
{
   poliqarp_error_message_set(query->error, _(message));
}

int yylex(void *, void *);

static size_t yytnamerr(char *yyres, const char *yystr)
{
   const char *translation = _(yystr);
   size_t length = strlen(translation);
   if (yyres != NULL)
      strcpy(yyres, translation);
   return length;
}
#if 0
   M_("$undefined");
   M_("$end");
#endif
#define yytnamerr yytnamerr

#define YY_ _

struct poliqarp_value *poliqarp_to_interp_value(const struct poliqarp_corpus *corpus,
   struct poliqarp_value *value)
{
   struct poliqarp_value *result;
   result = poliqarp_value_interp_create(corpus, value);
   poliqarp_value_destroy(value);
   return result;
}

static bool validate_expression(void *expression, struct poliqarp_error *error)
{
   if (expression == NULL) {
      if (errno == EINVAL)
         poliqarp_error_message_set(error, _("Phrasal and segment constraints cannot be combined"));
      else
         poliqarp_error_from_system(error, NULL);
      return false;
   }
   return true;
}

static bool require_syntax(struct poliqarp_corpus *corpus, struct poliqarp_error *error)
{
   if (corpus->syntax.syntax)
      return true;
   else {
      poliqarp_error_message_set(error, _("Phrasal queries are not allowed for this corpus"));
      return false;
   }
}

%}

%pure_parser
%lex-param { yyscan_t yyscanner }
%parse-param { void *yyscanner }
%parse-param { struct poliqarp_query *query }
%expect 30
%error-verbose

%union {
   char *as_text;
   struct {
      enum poliqarp_match_strategy strategy;
      bool use_disamb;
      bool negate;
   } as_op;
   struct {
      enum poliqarp_meta_match_strategy strategy;
      bool negate;
   } as_mop;
   struct {
      unsigned int flags;
      unsigned int xflags;
      char *text;
   } as_flagged_regexp;
   int as_int;
   double as_double;
   struct nfs_graph as_query_part;
   struct poliqarp_expression *as_expression;
   struct poliqarp_query *as_query;
   struct poliqarp_within *as_within;
   struct {
      int min;
      int max;
   } as_quantifier;
}

%token <as_text> IDENT
%token <as_text> STRING
%token <as_text> STRING_OPEN
%token <as_int> VARIABLE
%token <as_int> INTEGER
%token <as_int> SIGNED_INTEGER
%token <as_double> REAL_NUMBER
%token <as_op> OP
%token <as_mop> MOP
%token <void> META
%token <void> WITHIN
%token <void> HEAD
%token <void> SYNH
%token <void> SEMH
%token <void> GROUP_BY
%token <void> STAT_INTERP
%token <void> STAT_INTERP_RANDOM
%token <void> STAT_INTERP_COMBINE
%token <void> STAT_SORT_AFRONTE
%token <void> STAT_SORT_BY
%token <void> STAT_SORT_BY_FREQ
%token <void> STAT_SORT_BY_CP
%token <void> STAT_SORT_BY_SCP
%token <void> STAT_SORT_BY_MAXCP
%token <void> STAT_SORT_BY_DICE
%token <void> STAT_BIAS
%token <void> STAT_MIN
%token <void> STAT_COUNT

%left '|'
%left '&'
%left '!'
%left CONCAT
%nonassoc OPERATOR

%type <as_text> expanded_alias
%type <as_text> ident_string
%type <as_int> signed_integer
%type <as_int> unsigned_integer
%type <as_double> real_number
%type <as_flagged_regexp> flagged_regexp
%type <as_flagged_regexp> flagged_regexp_m
%type <as_expression> simple_exp
%type <as_expression> exp
%type <as_expression> exp_m
%type <as_expression> exp_phrase
%type <as_query_part> query_part
%type <as_query> query
%type <as_quantifier> quantifier
%type <as_expression> meta
%type <as_within> within

%destructor
{
   free($$);
} IDENT STRING expanded_alias ident_string

%destructor
{
   free($$.text);
} flagged_regexp flagged_regexp_m

%destructor
{
   poliqarp_expression_destroy($$);
} simple_exp exp exp_m exp_phrase meta

%destructor
{
   free($$);
} within

%%

query: 
   query_part within meta group_by
      {
         if (query->rewrite_in_progress)
         {
            query->rewrite_graph = $1;
         } else {
            poliqarp_query_set_within(query, $2);
            poliqarp_query_set_meta_expression(query, $3);
            graph_nfs_to_dfs(&query->graph, $1);
         }
      }
;

meta:
      { $$ = NULL; }
 | META exp_m
      { 
         if (query->rewrite_in_progress) {
            poliqarp_error_message_set(query->error, _("Metadata part is not allowed in a rewritten query"));
            poliqarp_expression_destroy($2);
            YYABORT;
         }
         $$ = $2;
      }
;

within:
      { $$ = NULL; }
 | WITHIN expanded_alias
      {
         if (query->rewrite_in_progress) {
            poliqarp_error_message_set(query->error, _("Within part is not allowed in a rewritten query"));
            free($2);
            YYABORT;
         }
         struct poliqarp_subdocument_set *set =
            poliqarp_backend_subdocument_lookup_set(&query->corpus->subdocument, $2);
         if (set == NULL) {
            poliqarp_error_message_set(query->error, _("There is no such structure %s"), $2);
            free($2); 
            YYABORT;
         }
         $$ = poliqarp_within_create_subdocument(set);
      }
  | WITHIN '[' exp ']'
      {
         if (poliqarp_expression_type($3) != POLIQARP_EXPRESSION_PHRASE) {
            poliqarp_error_message_set(query->error, _("Within expression does not denote a phrase"));
            poliqarp_expression_destroy($3);
            YYABORT;
         }
         $$ = poliqarp_within_create_phrase($3);
      }
;

group_by:
      { }
 | GROUP_BY stat_attr_list stat_aux_attr_list stat_interp stat_sort stat_min stat_count
      {
         poliqarp_error_message_set(query->error, _("Statistical queries are not supported"));
         YYABORT;
      }
;

stat_attr:
   ident_string
      { }
 | signed_integer '.' ident_string
      { }
;

stat_attr_list:
   stat_attr
      { }
 | stat_attr ',' stat_attr_list
      { }
;

stat_aux_attr_list:
      { }
 | ';' stat_attr_list
      { }
;

stat_interp:
      { }
 | STAT_INTERP STAT_INTERP_RANDOM
      { }
 | STAT_INTERP STAT_INTERP_COMBINE
      { }
;

stat_sort:
      { }
 | STAT_SORT_AFRONTE
      { }
 | STAT_SORT_BY STAT_SORT_BY_FREQ
      { }
 | STAT_SORT_BY stat_collating_function stat_bias
      { }
;

stat_collating_function:
   STAT_SORT_BY_CP
       { }
 | STAT_SORT_BY_SCP
       { }
 | STAT_SORT_BY_MAXCP
       { }
 | STAT_SORT_BY_DICE
       { }
;

stat_bias:
      { }
 | STAT_BIAS real_number
      { }
;

stat_min:
      { }
 | STAT_MIN unsigned_integer
      { }
;

stat_count:
      { }
 | STAT_COUNT unsigned_integer
      { }
;

simple_exp:
   '[' ']'
      { $$ = NULL; }
 | '[' exp ']'
      { $$ = $2; }
;

exp_phrase:
   HEAD OP simple_exp simple_exp
      { 
         if (!require_syntax(query->corpus, query->error)) {
            poliqarp_expression_destroy($3);
            poliqarp_expression_destroy($4);
            YYABORT;
         }
         $$ = poliqarp_phrase_expression($3, $4, false, ($2.strategy == POLIQARP_STRATEGY_ALL)); 
         if ($2.negate) 
            $$ = poliqarp_not_expression($$); 
      }
 | HEAD OP simple_exp
      { 
         if (!require_syntax(query->corpus, query->error)) {
            poliqarp_expression_destroy($3);
            YYABORT;
         }
         $$ = poliqarp_phrase_expression($3, $3, true, ($2.strategy == POLIQARP_STRATEGY_ALL)); 
         if ($2.negate)
            $$ = poliqarp_not_expression($$);
      }
 | SYNH OP simple_exp 
      { 
         if (!require_syntax(query->corpus, query->error)) {
            poliqarp_expression_destroy($3);
            YYABORT;
         }
         $$ = poliqarp_phrase_expression($3, NULL, false, ($2.strategy == POLIQARP_STRATEGY_ALL)); 
         if ($2.negate)
            $$ = poliqarp_not_expression($$);
      }
 | SEMH OP simple_exp 
      { 
         if (!require_syntax(query->corpus, query->error)) {
            poliqarp_expression_destroy($3);
            YYABORT;
         }
         $$ = poliqarp_phrase_expression(NULL, $3, false, ($2.strategy == POLIQARP_STRATEGY_ALL)); 
         if ($2.negate)
            $$ = poliqarp_not_expression($$);
      }
;

query_part: 
   flagged_regexp
      {
         struct poliqarp_value *value;
         struct poliqarp_expression *expr;
         char *rewritten_query_text = NULL;
         if (!query->rewrite_in_progress)
            rewritten_query_text = poliqarp_rewrite_query(query->rewrite, $1.text, $1.flags, $1.xflags);
         if (rewritten_query_text == NULL) {
            struct poliqarp_regexp regexp; 
            int rc = poliqarp_regexp_create(&regexp, $1.text, $1.flags, $1.xflags);
            if (rc != 0) {
               poliqarp_error_from_regexp(query->error, &regexp,
                  _("Error in regular expression '%s'"), $1.text);
               poliqarp_regexp_destroy(&regexp);
               free($1.text);
               YYABORT;
            }
            value = poliqarp_value_create_orth(query->corpus, &regexp);
            poliqarp_regexp_destroy(&regexp);
            poliqarp_value_use_strategy(value, POLIQARP_STRATEGY_ANY);
            poliqarp_value_use_disamb(value, true);
         }
         else
            value = NULL;
         free($1.text);
         if (value != NULL) {
            expr = poliqarp_expression_create_value(value, 
               (poliqarp_value_eval_fn)poliqarp_value_eval, 
               (poliqarp_value_compare_fn)poliqarp_value_compare,
               (poliqarp_value_destroy_fn)poliqarp_value_destroy);
            graph_nfs_atomic(&$$, &query->graph,
               graph_store_symbol(&query->graph, expr));
         } else {
            int rc;
            yyscan_t scanner;
            yylex_init(&scanner);
            yy_scan_string(rewritten_query_text, scanner);
            query->rewrite_in_progress = true;
            rc = yyparse(scanner, query);
            query->rewrite_in_progress = false;
            yylex_destroy(scanner);
            free(rewritten_query_text);
            switch (rc) {
               case 0:
                  $$ = query->rewrite_graph;
                  break;
               case 2:
                  errno = ENOMEM;
                  poliqarp_error_from_system(query->error, NULL);
                  /* fall through */
               case 1:
                  YYABORT;
               default:
                  abort(); /* should not happen */
            }
         }
      }
 | '(' query_part ')'
      { $$ = $2; }
 | simple_exp
      { 
         void *symbol;
         if ($1 != NULL) {
            symbol = graph_store_symbol(&query->graph, $1);
            if (symbol == NULL) {
               poliqarp_error_from_system(query->error, NULL);
               poliqarp_expression_destroy($1);
               YYABORT;
            }
         }
         else
            symbol = SYMBOL_DOT;
         int rc = graph_nfs_atomic(&$$, &query->graph, symbol);
         if (rc != 0) {
            poliqarp_error_from_system(query->error, NULL);
            poliqarp_expression_destroy($1);
            YYABORT;
         }
      }
 | '[' exp error
      {
         poliqarp_expression_destroy($2);
         poliqarp_error_message_set(query->error, _("Missing ]"));
         YYABORT;
      }
 | query_part '|' query_part 
      {
         int rc = graph_nfs_union(&$$, &query->graph, $1, $3, 0);
         if (rc != 0) {
            poliqarp_error_from_system(query->error, NULL);
            YYABORT;
         }
      }
 | query_part '|' error
      {
         poliqarp_error_message_set(query->error, _("Parse error after |"));
         YYABORT;
      }
 | query_part '^' query_part %prec CONCAT
      {
         query->eflags |= POLIQARP_QEFLAG_HAS_ALIGN;
         int rc = graph_nfs_concat(&$$, &query->graph, $1, $3, 1);
         if (rc != 0) {
            poliqarp_error_from_system(query->error, NULL);
            YYABORT;
         }
      }
 | query_part query_part %prec CONCAT
      {
         int rc = graph_nfs_concat(&$$, &query->graph, $1, $2, 0);
         if (rc != 0) {
            poliqarp_error_from_system(query->error, NULL);
            YYABORT;
         }
      }
 | query_part quantifier 
      {
         int rc = graph_nfs_quantify(&$$, &query->graph, $1, $2.min, $2.max);
         if (rc != 0) {
            if (errno == EINVAL) 
               poliqarp_error_message_set(query->error, _("Incorrect quantification"));
            else
               poliqarp_error_from_system(query->error, NULL);
            YYABORT;
         }
      }
;

exp_m: 
   IDENT MOP flagged_regexp_m
      {
         const struct poliqarp_corpus *corpus = query->corpus;
         struct poliqarp_meta_value *value;
         char *value_string = $3.text;
         unsigned flags = $3.flags;
         unsigned xflags = $3.xflags;

         enum poliqarp_meta_type mtype = poliqarp_get_metadata_type(
            poliqarp_get_const_backend(corpus, meta), $1);
         if (mtype == POLIQARP_META_TYPE_UNKNOWN) {
            poliqarp_error_message_set(query->error, _("Unknown metadata type"));
            free($1); free(value_string);
            YYABORT;
         }
         if (strlen(value_string) == 0)
            value = poliqarp_meta_value_create_undefined(corpus, $1);
         else if (mtype == POLIQARP_META_TYPE_STRING) {
            if ($2.strategy == POLIQARP_STRATEGY_EQUAL) {
               struct poliqarp_regexp regexp; 
               int rc = poliqarp_regexp_create(&regexp, value_string, flags, xflags);
               if (rc != 0) {
                  poliqarp_error_from_regexp(query->error, &regexp,
                     _("Error in regular expression '%s'"), value_string);
                  poliqarp_regexp_destroy(&regexp);
                  free($1); free(value_string);
                  YYABORT;
               }
               value = poliqarp_meta_value_create_text(corpus, $1, &regexp);
            } else {
               poliqarp_error_message_set(query->error, _("Unordered metadata type"));
               free($1); free(value_string);
               YYABORT;
            }
         } else { /* mtype == META_TYPE_DATE */
            struct poliqarp_meta_date date;
            if (poliqarp_meta_date_parse(value_string, &date) == -1) {
               poliqarp_error_message_set(query->error, _("Malformed date"));
               free($1); free(value_string);
               YYABORT;
            }
            if ($2.strategy == POLIQARP_STRATEGY_GREATER || 
                $2.strategy == POLIQARP_STRATEGY_SMALLER_EQUAL) {
               if (date.month == 0) date.month = 99;
               if (date.day == 0) date.day = 99;
            }
            if ($2.strategy == POLIQARP_STRATEGY_EQUAL) {
               if (date.year && date.month && date.day) 
                  value = poliqarp_meta_value_create_date(corpus, $1,
                     date, $2.strategy);
               else {
                  struct poliqarp_meta_value *vlow, *vhigh;
                  vlow = poliqarp_meta_value_create_date(corpus, $1,
                     date, POLIQARP_STRATEGY_GREATER_EQUAL);
                  if (date.month == 0) date.month = 99;
                  if (date.day == 0) date.day = 99;
                  vhigh = poliqarp_meta_value_create_date(corpus, $1,
                     date, POLIQARP_STRATEGY_SMALLER_EQUAL);
                  $$ = poliqarp_and_expression(
                     poliqarp_expression_create_value(vlow,
                        (poliqarp_value_eval_fn)poliqarp_meta_value_eval,
                        (poliqarp_value_compare_fn)poliqarp_meta_value_compare,
                        (poliqarp_value_destroy_fn)poliqarp_meta_value_destroy),
                     poliqarp_expression_create_value(vhigh,
                        (poliqarp_value_eval_fn)poliqarp_meta_value_eval,
                        (poliqarp_value_compare_fn)poliqarp_meta_value_compare,
                        (poliqarp_value_destroy_fn)poliqarp_meta_value_destroy));
                  goto expression_created;
               }
            } else {
               value = poliqarp_meta_value_create_date(corpus, $1,
                  date, $2.strategy);
            }
         }

         /* create expression */
         $$ = poliqarp_expression_create_value(value,
            (poliqarp_value_eval_fn)poliqarp_meta_value_eval,
            (poliqarp_value_compare_fn)poliqarp_meta_value_compare,
            (poliqarp_value_destroy_fn)poliqarp_meta_value_destroy);
expression_created:
         /* negate if neccesary */
         if ($2.negate)
            $$ = poliqarp_not_expression($$);
         /* cleanup */
         free($1);
         free($3.text);
      }
 | exp_m '|' exp_m   
      {
         $$ = poliqarp_or_expression($1, $3);
         if (!validate_expression($$, query->error))
            YYABORT;
      }
 | exp_m '&' exp_m
      {
         $$ = poliqarp_and_expression($1, $3);
         if (!validate_expression($$, query->error))
            YYABORT;
      }
 | '(' exp_m ')' 
      { $$ = $2; }
 | '!' exp_m
      { $$ = poliqarp_not_expression ($2); }
;

exp: 
   exp_phrase
      { $$ = $1; }
 | IDENT OP INTEGER
      {
         const struct poliqarp_corpus *corpus = query->corpus;
         const struct entity *entity;
         struct poliqarp_value *value;

         /* lookup name */
         entity = lookup_const_entity(&corpus->config.named_items, $1);
         if (entity == NULL || entity->tag == NULL) {
            poliqarp_error_message_set(query->error, _("There is no such attribute: %s"), $1);
            free($1);
            YYABORT;
         }

         /* create things based on entity type */
         switch (*(enum poliqarp_entity_type *)entity->tag) {
            case POLIQARP_ENTITY_ITEM_SPACE:
               value = poliqarp_value_create_space(corpus, ($3 != 0) != $2.negate);
               break;
            case POLIQARP_ENTITY_ITEM_POS:
            case POLIQARP_ENTITY_ITEM_BASE:
            case POLIQARP_ENTITY_ITEM_TAG:
            case POLIQARP_ENTITY_ITEM_TYPE:
            case POLIQARP_ENTITY_ATTR:
               poliqarp_error_message_set(query->error, _("Integers are not valid values for the attribute: %s"), $1);
               free($1);
               YYABORT;
               break;
            default:
               poliqarp_error_message_set(query->error, _("There is no such attribute: %s"), $1);
               free($1);
               YYABORT;
               break;
         }

         /* create expression */
         $$ = poliqarp_expression_create_value(value,
            (poliqarp_value_eval_fn)poliqarp_value_eval, 
            (poliqarp_value_compare_fn)poliqarp_value_compare,
            (poliqarp_value_destroy_fn)poliqarp_value_destroy);
         /* cleanup */
         free($1);
      }
 | IDENT OP flagged_regexp
      {
         const struct poliqarp_corpus *corpus = query->corpus;
         const struct entity *entity;
         struct poliqarp_value *value;
         char *value_string = $3.text;
         unsigned flags = $3.flags;
         unsigned xflags = $3.xflags;

         /* lookup name */
         entity = lookup_const_entity(&corpus->config.named_items, $1);
         if (entity == NULL || entity->tag == NULL) {
            poliqarp_error_message_set(query->error, _("There is no such attribute: %s"), $1);
            free($1); free(value_string);
            YYABORT;
         }

         struct poliqarp_regexp regexp; 
         int rc = poliqarp_regexp_create(&regexp, value_string, flags, xflags);
         if (rc != 0) {
            poliqarp_error_from_regexp(query->error, &regexp,
               _("Error in regular expression '%s'"), value_string);
            poliqarp_regexp_destroy(&regexp);
            free($1); free(value_string);
            YYABORT;
         }

         /* create things based on entity type */
         switch (*(enum poliqarp_entity_type *)entity->tag) {
            case POLIQARP_ENTITY_ITEM_ORTH:
               value = poliqarp_value_create_orth(corpus, &regexp);
               break;
            case POLIQARP_ENTITY_ITEM_POS:
               value = poliqarp_value_create_pos(corpus, &regexp);
               break;
            case POLIQARP_ENTITY_ITEM_BASE:
               value = $2.use_disamb ? 
                  poliqarp_value_create_base__disambiguated(corpus, &regexp) :
                  poliqarp_value_create_base__ambiguous(corpus, &regexp);
               break;
            case POLIQARP_ENTITY_ITEM_TAG:
               value = poliqarp_value_create_tag(corpus, &regexp);
               break;
            case POLIQARP_ENTITY_ITEM_TYPE:
               if (!require_syntax(query->corpus, query->error)) {
                  poliqarp_regexp_destroy(&regexp);
                  free($1); free(value_string);
                  YYABORT;
               }
               value = poliqarp_value_create_type(corpus, &regexp);
               break;
            case POLIQARP_ENTITY_ATTR:
               value = poliqarp_value_create_attr(corpus, $1, &regexp);
               break;
            default:
               poliqarp_error_message_set(query->error, _("There is no such attribute: %s"), $1);
               poliqarp_regexp_destroy(&regexp);
               free($1); free(value_string);
               YYABORT;
               break;
         }

         /* apply operator effects */
         poliqarp_value_use_strategy(value, $2.strategy);
         poliqarp_value_use_disamb(value, $2.use_disamb);

         switch (*(enum poliqarp_entity_type *)entity->tag) {
            case POLIQARP_ENTITY_ITEM_ORTH:
            case POLIQARP_ENTITY_ITEM_TYPE:
               break;
            case POLIQARP_ENTITY_ITEM_POS:
            case POLIQARP_ENTITY_ITEM_BASE:
            case POLIQARP_ENTITY_ITEM_TAG:
            case POLIQARP_ENTITY_ATTR:
               value = poliqarp_to_interp_value(corpus, value);
               break;
            default:
               abort(); /* Should not happen. */
         }

         /* create expression */
         $$ = poliqarp_expression_create_value(value,
            (poliqarp_value_eval_fn)poliqarp_value_eval, 
            (poliqarp_value_compare_fn)poliqarp_value_compare,
            (poliqarp_value_destroy_fn)poliqarp_value_destroy);

         /* negate if neccesary */
         if ($2.negate)
            $$ = poliqarp_not_expression($$);

         /* cleanup */
         poliqarp_regexp_destroy(&regexp);
         free($1);
         free(value_string);
      }
 | IDENT OP VARIABLE
      {
         const struct poliqarp_corpus *corpus = query->corpus;
         const struct entity *entity;
         struct poliqarp_value **values;
         struct poliqarp_expression **children = NULL;
         size_t i, j, n_values;	

         if ($3 >= POLIQARP_MAX_VARIABLES || $3 < 0) {
            poliqarp_error_message_set(query->error, _("Variable number is too large"));
            free($1);
            YYABORT;
         }

         /* lookup name */
         entity = lookup_const_entity(&corpus->config.named_items, $1);
         if (entity == NULL || entity->tag == NULL) {
            poliqarp_error_message_set(query->error, _("There is no such attribute: %s"), $1);
            free($1);
            YYABORT;
         }

         /* create things based on entity type */
         switch (*(enum poliqarp_entity_type *)entity->tag) {
            case POLIQARP_ENTITY_ITEM_POS:
               values = poliqarp_value_create_all_pos(corpus, &n_values);
               break;
            case POLIQARP_ENTITY_ATTR:
               values = poliqarp_value_create_all_attr(corpus, entity->name, &n_values);
               break;
            default:
               poliqarp_error_message_set(query->error, _("There is no such attribute: %s"), $1);
               free($1);
               YYABORT;
               break;
         }
         free($1);
         if (values == NULL) {
            poliqarp_error_from_system(query->error, NULL);
            YYABORT;
         }
         for (i = 0; i < n_values; i++) {
            /* apply operator effects */
            poliqarp_value_use_strategy(values[i], $2.strategy);
            poliqarp_value_use_disamb(values[i], $2.use_disamb);
            values[i] = poliqarp_to_interp_value(corpus, values[i]);
            if (values[i] == NULL)
               break;
         }
         if (i != n_values) {
            poliqarp_error_from_system(query->error, NULL);
            for (j = 0; j < i; j++)
               poliqarp_value_destroy(values[j]);
            free(values);
            YYABORT;
         }
         children = malloc(n_values * sizeof(*children));
         if (children == NULL) {
            poliqarp_error_from_system(query->error, NULL);
            YYABORT;
         }

         for (i = 0; i < n_values; i++)
            children[i] = NULL;
         for (i = 0; i < n_values; i++) {
            children[i] = poliqarp_expression_create_value(values[i],
               (poliqarp_value_eval_fn)poliqarp_value_eval, 
               (poliqarp_value_compare_fn)poliqarp_value_compare,
               (poliqarp_value_destroy_fn)poliqarp_value_destroy);
            if (children[i] == NULL)
               break;
            values[i] = NULL;
         }
         if (i != n_values) {
            poliqarp_error_from_system(query->error, NULL);
            for (j = 0; j < i; j++)
               poliqarp_expression_destroy(children[j]);
            for (j = i; j < n_values; j++)
               poliqarp_value_destroy(values[j]);
            free(values);
            YYABORT;
         }

         free(values);

         /* create expression */
         $$ = poliqarp_variable_expression(n_values, children, $3);
         if ($$ == NULL) {
            poliqarp_error_from_system(query->error, NULL);
            for (i = 0; i < n_values; i++)
               poliqarp_expression_destroy(children[i]);
            YYABORT;
         }
         /* negate if neccesary */
         if ($2.negate)
            $$ = poliqarp_not_expression($$);
         /* check variable types, ranges etc. */
         query->eflags |= POLIQARP_QEFLAG_HAS_VARIABLES;
         if (query->variable_types[$3] == NULL)
            query->variable_types[$3] = entity->name; 
         else if (query->variable_types[$3] != entity->name) {
            poliqarp_error_message_set(query->error, _("Incompatible variable types"));
            poliqarp_expression_destroy($$);
            YYABORT;
         }
         poliqarp_expression_variable_ranges($$, query->variable_ranges);
         /* cleanup */
      }
 | exp '|' exp 
      {
         $$ = poliqarp_or_expression($1, $3);
         if (!validate_expression($$, query->error))
            YYABORT;
      }
 | exp '&' exp 
      {
         $$ = poliqarp_and_expression($1, $3);
         if (!validate_expression($$, query->error))
            YYABORT;
      }
 | '(' exp ')' 
      { $$ = $2; }
 | '!' exp 
      { $$ = poliqarp_not_expression ($2); }
;

/* quantifiers */
quantifier: 
   '{' '}' 
      { $$.min = 1; $$.max = 1; }
 | '{' INTEGER ',' INTEGER '}'
      { $$.min = $2; $$.max = $4; }
 | '{' INTEGER '}' 
      { $$.min = $2; $$.max = $2; }
 | '{' ',' INTEGER '}' 
      { $$.min = 0; $$.max = $3; }
 | '{' INTEGER ',' '}' 
      { $$.min = $2; $$.max = -1; }
 | '+' 
      { $$.min = 1; $$.max = -1; }
 | '?'
      { $$.min = 0; $$.max = 1; }
 | '*'
      { $$.min = 0; $$.max = -1; }
 | '{' error 
      {
         poliqarp_error_message_set(query->error, _("Parse error after {"));
         YYABORT;
      }
;


flagged_regexp_m: 
   expanded_alias
      {
         $$.flags = (query->flags & POLIQARP_QFLAG_META_I) ? REG_ICASE : 0;
         $$.flags |= REG_NOSUB;
         $$.xflags = (query->flags & POLIQARP_QFLAG_META_X) ? POLIQARP_REG_NO_ANCHORS : 0;
         $$.text = $1;
      }
 | expanded_alias '/' IDENT
      {
         $$.flags = (query->flags & POLIQARP_QFLAG_META_I) ? REG_ICASE : 0;
         $$.flags |= REG_NOSUB;
         $$.xflags = (query->flags & POLIQARP_QFLAG_META_X) ? POLIQARP_REG_NO_ANCHORS : 0;
         poliqarp_parse_regexp_flags($3, &$$.flags, &$$.xflags); /* extra flags */
         free($3);
         $$.text = $1;
      }
 | expanded_alias '/' error
      {
         poliqarp_error_message_set(query->error, _("Missing flags"));
         free($1);
         YYABORT;
      }
;

flagged_regexp: 
   expanded_alias
      {
         $$.flags = (query->flags & POLIQARP_QFLAG_QUERY_I) ? REG_ICASE : 0;
         $$.xflags = (query->flags & POLIQARP_QFLAG_QUERY_X) ? POLIQARP_REG_NO_ANCHORS : 0;
         $$.text = $1;
      }
 | expanded_alias '/' IDENT
      {
         $$.flags = (query->flags & POLIQARP_QFLAG_QUERY_I) ? REG_ICASE : 0;
         $$.xflags = (query->flags & POLIQARP_QFLAG_QUERY_X) ? POLIQARP_REG_NO_ANCHORS : 0;
         poliqarp_parse_regexp_flags($3, &$$.flags, &$$.xflags); /* extra flags */
         free($3);
         $$.text = $1;
      }
;

expanded_alias:
   ident_string 
      {
         if (query->aliases) {
            const char *text = hash_table_const_get(query->aliases, $1);
            $$ = text ? strdup (text) : $1;
         } else
            $$ = $1;
      }
;

ident_string:
   IDENT
 | STRING
      { $$ = $1; }
 | STRING_OPEN error
      {
         poliqarp_error_message_set(query->error, _("No closing quote after string: %s"), $1);
         free($1);
         YYABORT;
      }
;

unsigned_integer:
   INTEGER
      {
         $$ = $1;
      }
;

signed_integer:
   unsigned_integer
      {
         $$ = $1;
      }
 | SIGNED_INTEGER
      {
         $$ = $1;
      }
;

real_number:
   signed_integer
      {
         $$ = $1;
      }
 | REAL_NUMBER
      {
         if (isnan($1)) {
            poliqarp_error_message_set(query->error, _("Invalid floating-point number"));
            YYABORT;
         }
         $$ = $1;
      }
;

%%

