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

#include <sakura/random.h>
#include <sakura/query.h>
#include <sakura/lexer.h>
#include <sakura/common/bitstream.h>

#define POLIQARP_UNINDEX_THRESHOLD 200

int yyparse(yyscan_t scanner, struct poliqarp_query *query);

struct poliqarp_within *poliqarp_within_create_subdocument(
   struct poliqarp_subdocument_set *set)
{
   struct poliqarp_within *result = malloc(sizeof(*result));
   result->type = WITHIN_SUBDOCUMENT;
   result->as.subdocument = set;
   return result;
}

struct poliqarp_within *poliqarp_within_create_phrase(
   struct poliqarp_expression *phrase)
{
   struct poliqarp_within *result = malloc(sizeof(*result));
   result->type = WITHIN_PHRASE;
   result->as.phrase = phrase;
   return result;
}

int poliqarp_create_query(struct poliqarp_query *this, 
   const char *query_text, struct poliqarp_corpus *corpus, int flags,
   const char *rewrite, struct poliqarp_random_state *random_state,
   struct poliqarp_error *error)
{
   yyscan_t scanner;
   size_t i;
   int rc;

   assert(this != NULL);
   assert(corpus != NULL);
   assert(query_text != NULL);

   this->aliases = &(poliqarp_get_const_backend(corpus, config)->aliases);
   bitset_arena_create_dummy(&this->area.arena);
   graph_create(&this->graph, (set_compare_fn)poliqarp_expression_compare,
      (set_free_fn)poliqarp_expression_destroy);
   this->corpus = corpus;
   this->error = error;
   this->random_state = random_state;

   yylex_init(&scanner);
   this->query_text = strdup(query_text);
   yy_scan_string(this->query_text, scanner);
   this->meta_expression = NULL;
   this->eflags = 0;
   this->flags = flags;
   memset(this->variable_ranges, 0, sizeof this->variable_ranges);
   for (i = 0; i < POLIQARP_MAX_VARIABLES; i++)
      this->variable_types[i] = NULL;
   this->rewrite = poliqarp_get_query_rewrite(
      &corpus->config.query_rewrite_table,
      rewrite, false);
   this->rewrite_in_progress = false;
   switch (yyparse(scanner, this)) {
      case 0:
         break;
      case 2:
         errno = ENOMEM;
         poliqarp_error_from_system(this->error, NULL);
         /* fall through */
      case 1:
         rc = -1;
         poliqarp_destroy_query(this);
         goto cleanup;
      default:
         abort(); /* should not happen */
   }
   this->have_last_context = false;
   this->max_segment = 0;
   this->progress = 0;
   this->progress_helper = 0;
   poliqarp_create_search_area(this);

   /* reset corpus backends */
   corpus->document.current = 0;
   if (this->within && this->within->type == WITHIN_SUBDOCUMENT)
      this->within->as.subdocument->current = 0;
   if (corpus->syntax.syntax)
      poliqarp_backend_syntax_reset(&corpus->syntax);
   rc = 0;
cleanup:
   yylex_destroy(scanner);
   return rc;
}

void poliqarp_query_set_meta_expression(struct poliqarp_query *this, 
   struct poliqarp_expression *exp)
{
   assert(this != NULL);

   if (this->meta_expression)
      poliqarp_expression_destroy(this->meta_expression);
   this->meta_expression = exp;
}

void poliqarp_query_set_within(struct poliqarp_query *this, 
   struct poliqarp_within *within)
{
   assert(this != NULL);

   this->within = within;
}

int poliqarp_destroy_query(struct poliqarp_query *this)
{
   assert(this != NULL);

   if (this->corpus) {
      free(this->query_text);
      graph_destroy(&this->graph);
      if (this->meta_expression)
         poliqarp_expression_destroy(this->meta_expression);
      this->corpus = NULL;
      bitset_arena_destroy(&this->area.arena);
   }
   return 0;
}

enum xmode { cleanup, look, found, phrase_found, not_found, add, boundary, finish };

struct produce_helper {
   size_t last_segment;
   const struct poliqarp_searchable_area *area;
   const struct dfs_node *root;
   void *session;
   int notify_step;
   progress_t *progress;
   size_t progress_helper;
   bool document_fixup;
};

static inline uint64_t update_progress(struct poliqarp_query *this,
   progress_t *progress, size_t pos, uint64_t helper)
{
   if (pos > this->max_segment) {
      uint32_t num_segments =
         poliqarp_backend_corpus_size(&this->corpus->corpus);
      helper += (uint64_t)100 * (pos - this->max_segment);
      this->max_segment = pos;
      while (helper >= num_segments) {
         helper -= num_segments;
         progress_advance(progress, 1);
      }
   }
   return helper;
}

struct return_stack {
   size_t size;
   size_t pointer;
   struct poliqarp_search_context *data;
};

static void return_stack_create(struct return_stack *stack)
{
   stack->size = 16;
   stack->pointer = 0;
   stack->data = malloc(stack->size * sizeof(stack->data[0]));
}

static void return_stack_destroy(struct return_stack *stack)
{
   free(stack->data);
}

static void return_stack_push(struct return_stack *stack, 
   const struct poliqarp_search_context *p)
{
   if (stack->pointer == stack->size) {
      stack->size *= 2;
      stack->data = realloc(stack->data, stack->size * sizeof(stack->data[0]));
   }
   stack->data[stack->pointer++] = *p;
}

static void return_stack_push_link(struct return_stack *stack,
   const struct poliqarp_search_context *p)
{
   struct poliqarp_search_context q = *p;
   q.link = q.link->next;
   return_stack_push(stack, &q);
}

static void return_stack_push_phrase(struct return_stack *stack,
   const struct poliqarp_search_context *p)
{
   struct poliqarp_search_context q = *p;
   q.phrase++;
   return_stack_push(stack, &q);
}

static struct poliqarp_search_context return_stack_pop(struct return_stack *stack)
{
   return stack->data[--stack->pointer];
}

static bool return_stack_empty(const struct return_stack *stack)
{
   return (stack->pointer == 0);
}

static void return_stack_wipe(struct return_stack *stack)
{
   stack->pointer = 0;
}

static inline struct poliqarp_binary_segment get_segment(
   struct poliqarp_backend_corpus *this, size_t index)
{
   if (index != file_reader_current(&this->corpus))
      poliqarp_backend_corpus_seek(this, index);
   return poliqarp_backend_corpus_next(this);
}

static bool phrase_expression_eval_single(const struct poliqarp_expression *expression,
   struct poliqarp_corpus *corpus, const struct poliqarp_syntax_group *phrase)
{
   bool res = true;
   struct poliqarp_binary_segment pos;
   if (expression->as.phrase.synh) {
      if (phrase->u.noncoord.synh != POLIQARP_SYNTAX_GROUP_UNKNOWN) {
         pos = get_segment(&corpus->corpus, phrase->u.noncoord.synh);
         res = poliqarp_expression_eval(expression->as.phrase.synh, corpus, &pos, NULL);
      } else {
         res = false;
      }
   }
   if (!res)
      return false;
   if (expression->as.phrase.same) 
      return phrase->u.noncoord.synh == phrase->u.noncoord.semh;
   if (expression->as.phrase.semh) {
      if (phrase->u.noncoord.semh != POLIQARP_SYNTAX_GROUP_UNKNOWN) {
         pos = get_segment(&corpus->corpus, phrase->u.noncoord.semh);
         return poliqarp_expression_eval(expression->as.phrase.semh, corpus, &pos, NULL);
      } else {
         return false;
      }
   } else
      return true;
}

static bool phrase_expression_eval(const struct poliqarp_expression *expression,
   struct poliqarp_corpus *corpus, size_t i)
{
   bool result = false;
   struct poliqarp_syntax_group *groups = corpus->syntax.groups;
   switch (expression->type) {
      case POLIQARP_EXPRESSION_CONSTANT:
         return expression->as.constant;
      case POLIQARP_EXPRESSION_VALUE:
         return BIT_ARRAY_GET(((struct poliqarp_value *)(expression->as.value.value))->bits,
            groups[i].type);
      case POLIQARP_EXPRESSION_AND:
         return expression->as.expression.negate ^ 
            (phrase_expression_eval(expression->as.expression.left, corpus, i) && 
             phrase_expression_eval(expression->as.expression.right, corpus, i));
      case POLIQARP_EXPRESSION_OR:
         return expression->as.expression.negate ^ 
            (phrase_expression_eval(expression->as.expression.left, corpus, i) ||
             phrase_expression_eval(expression->as.expression.right, corpus, i));         
      case POLIQARP_EXPRESSION_PHRASE:
         if (groups[i].type == POLIQARP_SYNTAX_GROUP_COORD) {
            int len = groups[i].u.coord.length;
            result = expression->as.phrase.negate ^ expression->as.phrase.all;
            while (--len) {
               if (++i == corpus->syntax.size)
                  i = 0;
               if (groups[i].type == POLIQARP_SYNTAX_GROUP_COORD || groups[i].type == POLIQARP_SYNTAX_GROUP_CONJUNCTION)
                  continue;
               if (expression->as.phrase.all) {
                  if (!phrase_expression_eval_single(expression, corpus, groups + i)) {
                     result = !result;
                     break;
                  }
               } else {
                  if (phrase_expression_eval_single(expression, corpus, groups + i)) {
                     result = !result;
                     break;
                  }
               }
            }
         } else {
            result = expression->as.phrase.negate ^ 
               phrase_expression_eval_single(expression, corpus, groups + i);
         }
         return result;
      case POLIQARP_EXPRESSION_VARIABLE:
         return phrase_expression_eval(expression->as.variable.children[0],
            corpus, i);
         /* FIXME */
      case POLIQARP_EXPRESSION_INVALID:
         abort(); /* Should not happen. */
   }
   return false; /* muffle potential warnings */
}

static size_t find_phrase_satisfying_expression(struct poliqarp_corpus *corpus,
   uint32_t offset, size_t phrase, const struct poliqarp_expression *expr)
{
   struct poliqarp_backend_syntax *this = &corpus->syntax;
   if (phrase == (size_t)-1 || offset / 1024 > this->groups[phrase].to / 1024) {
      this->start = this->end = phrase = 0;
      this->pos = en4(*((uint32_t *)tinydb_fetch_item(&this->offsets, offset / 1024)));
   }
   while (phrase == this->end || this->groups[phrase].to < offset ||
      !phrase_expression_eval(expr, corpus, phrase)) 
   {
      if (phrase == this->end) {
         if (poliqarp_backend_syntax_next(this) == -1)
            return -1;
         this->start = phrase;
      } else {
         if (++phrase == this->size)
            phrase = 0;
      }
   }
   return phrase;
}

static int check_boundaries(struct poliqarp_search_context *ctx,
   struct poliqarp_query *this)
{
   int check_index = (ctx->index % this->area.granularity == 0);
   for (;;) {
      if (check_index) {
         size_t i = ctx->index / this->area.granularity;
         while (i < this->area.num_bits && 
            !bitset_arena_get(&this->area.arena, this->area.area, i)) 
         {
            i++;
         }
         if (ctx->index < i * this->area.granularity)
            ctx->index = i * this->area.granularity;
         if (ctx->index >= poliqarp_backend_corpus_size(&this->corpus->corpus))
            return -1;
      }
      if (ctx->index >= ctx->document.corpus_high) {
         /* try to read next document */
         if (poliqarp_backend_document_next(&this->corpus->document, &ctx->document) == -1)
            return -1;
         if (ctx->index < ctx->document.corpus_low || ctx->index >= ctx->document.corpus_high) {
            poliqarp_backend_document_search(&this->corpus->document, ctx->index);
            if (poliqarp_backend_document_next(&this->corpus->document, &ctx->document) == -1)
               return -1;
         }
         while (this->meta_expression && 
            poliqarp_expression_eval(this->meta_expression, this->corpus, &ctx->document, NULL) == false)
         {
            if (poliqarp_backend_document_next(&this->corpus->document, &ctx->document) == -1)
               return -1;
         }
         if (ctx->index < ctx->document.corpus_low)
            ctx->index = ctx->document.corpus_low;
         check_index = 1;
         continue;
      }
      if (this->within && this->within->type == WITHIN_SUBDOCUMENT &&
         ctx->index >= ctx->subdocument.corpus_high) 
      {
         /* try to read next subdocument */
         if (poliqarp_subdocument_next(this->within->as.subdocument, &ctx->subdocument) == -1)
            return -1;
         if (ctx->index >= ctx->subdocument.corpus_high) {
            poliqarp_subdocument_search(this->within->as.subdocument, ctx->index);
            if (poliqarp_subdocument_next(this->within->as.subdocument, &ctx->subdocument) == -1)
               return -1;
         }
         if (ctx->index < ctx->subdocument.corpus_low)
            ctx->index = ctx->subdocument.corpus_low;
         check_index = 1;
         continue;
      }
      if (this->within && this->within->type == WITHIN_PHRASE) {
         /* This function is called in the circumstances where we can safely
            reset the phrase list.  We want to scroll to the next phrase that
            satisfies the current `within phrase' expression. */
         struct poliqarp_backend_syntax *syntax = &this->corpus->syntax;
         ctx->within_phrase = find_phrase_satisfying_expression(this->corpus, ctx->index,
            ctx->within_phrase, this->within->as.phrase);
         ctx->phrase = syntax->start;
         if (ctx->within_phrase == (size_t)-1)
            return -1;
         if (ctx->index < syntax->groups[ctx->within_phrase].from) {
            ctx->index = syntax->groups[ctx->within_phrase].from;
            check_index = 1;
            continue;
         }
      }
      return 0;
   }
}

static bool end_of_within(struct poliqarp_search_context *ctx,
   struct poliqarp_query *this)
{
   return 
      ((ctx->index == ctx->document.corpus_high) ||
       (this->within && this->within->type == WITHIN_SUBDOCUMENT &&
          ctx->index == ctx->subdocument.corpus_high) ||
       (this->within && this->within->type == WITHIN_PHRASE && 
          ctx->index == this->corpus->syntax.groups[ctx->within_phrase].to + 1));
}

static void poliqarp_produce_cleanup(void *data)
{
   struct return_stack *stack = data;
   return_stack_destroy(stack);
}

static bool poliqarp_reset_variable_bindings(struct poliqarp_query *this,
   size_t *variable_bindings)
{
   size_t i;
   for (i = 0; i < POLIQARP_MAX_VARIABLES; i++)
      variable_bindings[i] = 0;
   return true;
}

static bool poliqarp_next_variable_bindings(struct poliqarp_query *this,
   size_t *variable_bindings)
{
   size_t i;
   for (i = 0; i < POLIQARP_MAX_VARIABLES; i++)
   {
      if (variable_bindings[i] > 0)
      {
         variable_bindings[i]--;
         break;
      }
      else
      {
         size_t r = this->variable_ranges[i];
         variable_bindings[i] = (r > 0) ? (r - 1) : 0; 
      }
   }
   return i < POLIQARP_MAX_VARIABLES;
}

static inline void poliqarp_swap_query_result(
   struct poliqarp_match_buffer *this, size_t i)
{
   pthread_mutex_lock(&this->mutex);
   assert(i < this->used);
   struct poliqarp_match temp = this->match[i];
   this->match[i] = this->match[this->used - 1];
   this->match[this->used - 1] = temp;
   pthread_mutex_unlock(&this->mutex);
}

static inline void poliqarp_replace_query_result(
   struct poliqarp_match_buffer *this, size_t i,
   const struct poliqarp_match *match)
{
   pthread_mutex_lock(&this->mutex);
   assert(i < this->used);
   this->match[i] = *match;
   pthread_mutex_unlock(&this->mutex);
}

int poliqarp_produce(struct poliqarp_match_buffer *result, size_t max,
   struct poliqarp_query *this, progress_t *progress, void *session,
   size_t notify_step, size_t max_match_length)
{
   struct poliqarp_corpus *corpus = this->corpus;
   size_t corpus_size;
   enum xmode next_mode, mode;
   struct poliqarp_search_context ctx;
   struct return_stack stack;
   struct poliqarp_binary_segment pos;
   struct poliqarp_match match;
   uint64_t progress_helper = 0;
#ifndef POLIQARP_SINGLE_THREADED
   int cancel_state;
#endif
   size_t variable_bindings[POLIQARP_MAX_VARIABLES];

#ifndef POLIQARP_SINGLE_THREADED
   pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cancel_state); 
#endif
   corpus_size = poliqarp_backend_corpus_size(&corpus->corpus);
   pthread_mutex_lock(&progress->mutex);
   result->corpus = corpus;
   pthread_mutex_unlock(&progress->mutex);
   return_stack_create(&stack);
   if (this->have_last_context)
      ctx = this->last_context;
   else {
      progress_reset(progress);
      ctx.m_start = (size_t)-1;
      ctx.phrase = 0;
      ctx.document.corpus_high = ctx.subdocument.corpus_high = 0;
      ctx.within_phrase = (size_t)-1;
   }

   if (this->eflags & POLIQARP_QEFLAG_HAS_VARIABLES)
   {
      poliqarp_reset_variable_bindings(this, variable_bindings);
      ctx.index = -1;
   }

#ifndef POLIQARP_SINGLE_THREADED
   pthread_cleanup_push(poliqarp_produce_cleanup, &stack);
#endif
   next_mode = cleanup; /* just to shut up compilers */
   for (mode = cleanup; ; mode = next_mode) {
      switch (mode) {
         case cleanup:
            if (return_stack_empty(&stack)) {
               ctx.node = this->graph.dfs.root;
               ctx.link = NULL;
               if (this->eflags & POLIQARP_QEFLAG_HAS_VARIABLES)
               {
                  if (ctx.m_start != (size_t)-1)
                     ctx.index = ctx.m_start;
                  if (!poliqarp_next_variable_bindings(this, variable_bindings))
                     ctx.index++;
               }
               else
                  ctx.index = ctx.m_start + 1;
               ctx.m_start = ctx.m_end = ctx.m_focus = ctx.c_focus = (size_t)-1;
            } else {
               ctx = return_stack_pop(&stack);
               next_mode = look; /* skip checking boundaries if backtracking */
               break;
            }
            /* fall through */

         case boundary:
            if (check_boundaries(&ctx, this) == -1 || ctx.index >= corpus_size) {
               next_mode = finish;
               break;
            }
            /* fall through */

         case look:
#ifndef POLIQARP_SINGLE_THREADED
            pthread_setcancelstate(cancel_state, NULL);
            pthread_testcancel(); /* cancel opportunity */
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
#endif
            next_mode = not_found;
            if (ctx.link == NULL)
               ctx.link = ctx.node->first_link;
            pos = get_segment(&corpus->corpus, ctx.index);
            while (ctx.link) {
               if (ctx.m_start != (size_t)-1 && ctx.index - ctx.m_start >= max_match_length)
               {
                  /* skip */
               } else if (ctx.link->symbol != SYMBOL_DOT && poliqarp_expression_type(
                     (const struct poliqarp_expression *)ctx.link->symbol) == POLIQARP_EXPRESSION_PHRASE)
               {
                  if (!corpus->syntax.syntax)
                     break;
                  for (;;) {
                     if (ctx.phrase == corpus->syntax.end) {
                        if (poliqarp_backend_syntax_next(&corpus->syntax) == -1)
                           goto look_breakout;
                     }
                     if (corpus->syntax.groups[ctx.phrase].from < ctx.index) {
                        ctx.phrase++;
                        continue;
                     } else if (corpus->syntax.groups[ctx.phrase].from == ctx.index) {
                        if (phrase_expression_eval((const struct poliqarp_expression *)ctx.link->symbol,
                               corpus, ctx.phrase))
                        {
                           next_mode = phrase_found;
                           goto look_breakout;
                        } else
                           ctx.phrase++;
                     } else 
                        break; 
                  }
               } else if (ctx.link->symbol == SYMBOL_DOT || 
                  (ctx.index < corpus_size && 
                   poliqarp_expression_eval(ctx.link->symbol, corpus, &pos, variable_bindings)))
               {
                  if (ctx.link->next)
                     return_stack_push_link(&stack, &ctx);
                  next_mode = found;
                  if (ctx.link->flags)
                     ctx.c_focus = ctx.index;
                  break;
               }
               ctx.link = ctx.link->next;
            }
look_breakout:
            if (ctx.node->flags.is_final) {
               ctx.m_focus = ctx.c_focus; /* promote focus candidate */
               ctx.m_end = ctx.index;
            }
            break;

         case found:
            ctx.node = ctx.link->to; /* follow link */
            ctx.link = NULL; 
            if (ctx.m_start == (size_t)-1) /* remember first item */
               ctx.m_start = ctx.index;
            ctx.index++;
            if (end_of_within(&ctx, this)) {
               if (ctx.node->flags.is_final)
                  ctx.m_end = ctx.index;
               next_mode = ctx.m_end != (size_t)-1 ? add : cleanup;
            } else
               next_mode = look;
            progress_helper = update_progress(this, progress, ctx.index,
               progress_helper);
            break;

         case phrase_found:
            /* FIXME: check whether this is not the last phrase */
            return_stack_push_phrase(&stack, &ctx);
            ctx.node = ctx.link->to; /* follow link */
            ctx.link = NULL;
            if (ctx.m_start == (size_t)-1) /* remember first item */
               ctx.m_start = ctx.index;
            ctx.index = corpus->syntax.groups[ctx.phrase].to + 1;
            if (end_of_within(&ctx, this)) {
               if (ctx.node->flags.is_final)
                  ctx.m_end = ctx.index;
               next_mode = ctx.m_end != (size_t)-1 ? add : cleanup;
            } else
               next_mode = look;
            progress_helper = update_progress(this, progress, ctx.index,
               progress_helper);
            break;
            
         case not_found:
            if (ctx.m_start != (size_t)-1) {
               if (ctx.m_end == (size_t)-1 || ctx.m_start == ctx.m_end)
                  next_mode = cleanup;
               else
                  next_mode = add;
               /* note: we don't advance here */
            } else if (this->eflags & POLIQARP_QEFLAG_HAS_VARIABLES) {
               next_mode = cleanup;
            } else {
               ctx.index++;
               if ((ctx.index & 0xffff) == 0) {
                  /* From time to time, update the progress even if there are
                   * no matches. */
                  progress_helper = update_progress(this, progress, ctx.index,
                     progress_helper);
               }
               next_mode = boundary;
            }
            break;

         case add:
            match.start = ctx.m_start;
            assert(ctx.m_end != (size_t)-1);
            match.end = ctx.m_end;
            match.withinStart = ctx.subdocument.corpus_low;
            match.withinEnd = ctx.subdocument.corpus_high;
            match.focus = ctx.m_focus == (size_t)-1 ? ctx.m_start : ctx.m_focus;
            match.document = corpus->document.current - 1;
            pthread_mutex_lock(&result->mutex);
            result->num_results++;
            pthread_mutex_unlock(&result->mutex);
            if (result->used < result->capacity) {
               poliqarp_append_query_result(result, &match);
               if (this->random_state) {
                  size_t i = poliqarp_random(this->random_state) % result->num_results;
                  poliqarp_swap_query_result(result, i);
               } else {
                  if (notify_step > 0 && result->num_results % notify_step == 0)
                     async_notify_new_results(session);
                  if (result->num_results == max)
                     goto out;
               }
            } else {
               if (this->random_state) {
                  size_t i = poliqarp_random(this->random_state) % result->num_results;
                  if (i < result->used)
                     poliqarp_replace_query_result(result, i, &match);
               } else {
                  goto out;
               }
            }
               
            return_stack_wipe(&stack);
            /* Comment out the following line for (slower) support of subresults,
               i.e. results that are part of other results. */
            ctx.m_start = ctx.m_end - 1;
            next_mode = cleanup;
            break;
            
         case finish:
            goto out;
      }
   }

out:
   if (result->used > max)
      result->used = max;
   this->have_last_context = true;
   this->last_context = ctx;
#ifndef POLIQARP_SINGLE_THREADED
   pthread_cleanup_pop(1);
   pthread_setcancelstate(cancel_state, NULL);
#else
   poliqarp_produce_cleanup(&stack);
#endif
   return 0;
}

int poliqarp_create_match_buffer(struct poliqarp_match_buffer *this, 
   size_t capacity)
{
   assert(this != NULL);

   this->match = malloc(sizeof *this->match * capacity);
   this->used = 0;
   this->num_results = 0;
   this->capacity = capacity;
   this->sort_arena = NULL;
   pthread_mutex_init(&this->mutex, NULL);
   return 0;
}

int poliqarp_destroy_match_buffer(struct poliqarp_match_buffer *this)
{
   assert(this != NULL);
   free(this->match);
   if (this->sort_arena)
      marena_destroy(this->sort_arena);
   pthread_mutex_destroy(&this->mutex);
   return 0;
}

int poliqarp_get_match_buffer_info(struct poliqarp_match_buffer *buffer,
   struct poliqarp_match_buffer_info *info)
{
   pthread_mutex_lock(&buffer->mutex);
   info->capacity = buffer->capacity;
   info->used = buffer->used;
   info->num_results = buffer->num_results;
   pthread_mutex_unlock(&buffer->mutex);
   return 0;
}

int poliqarp_forget(struct poliqarp_match_buffer *buffer)
{
   buffer->used = 0;
   buffer->num_results = 0;
   return 0;
}

int poliqarp_resize_match_buffer(struct poliqarp_match_buffer *buffer,
   size_t size)
{
   if (size < buffer->used) {
      memmove(buffer->match, buffer->match + buffer->used - size, 
         size * sizeof *buffer->match);
      buffer->used = size;
   }
   buffer->match = realloc(buffer->match, size * sizeof *buffer->match);
   buffer->capacity = size;
   return 0;
}

struct sort_helper_descriptor {
   uint32_t *segments;
   size_t length;
};

static inline int sort_cmp(struct sort_helper_descriptor *hdesc, uint32_t x,
   uint32_t y, const struct poliqarp_sort_info *criteria, 
   struct poliqarp_corpus *corpus)
{
   int res = 0;
   size_t xl = 0, yl = 0;
   if (criteria->atergo) {
      if (hdesc[x].length && hdesc[y].length) { 
         xl = hdesc[x].length - 1;
         yl = hdesc[y].length - 1;
         while (xl && yl) {
            uint32_t xo = hdesc[x].segments[xl], yo = hdesc[y].segments[yl];
            size_t i1, i2;
            i1 = poliqarp_backend_orth_atergo_fetch(poliqarp_get_const_backend(
               corpus, orth), xo);
            i2 = poliqarp_backend_orth_atergo_fetch(poliqarp_get_const_backend(
               corpus, orth), yo); 
            res = i1 == i2 ? 0 : (i1 < i2 ? -1 : +1);
            if (res)
               break;
            xl--; yl--;
         }
      } else {
         xl = hdesc[x].length;
         yl = hdesc[y].length;
      }
   } else {
      while (xl < hdesc[x].length && yl < hdesc[y].length) {
         uint32_t xo = hdesc[x].segments[xl], yo = hdesc[y].segments[yl];
         size_t i1, i2;
         i1 = poliqarp_backend_orth_afronte_fetch(poliqarp_get_const_backend(
            corpus, orth), xo);
         i2 = poliqarp_backend_orth_afronte_fetch(poliqarp_get_const_backend(
            corpus, orth), yo); 
         res = i1 == i2 ? 0 : (i1 < i2 ? -1 : +1);
         if (res)
            break;
         xl++; yl++;
      }
      xl = hdesc[x].length - xl;
      yl = hdesc[y].length - yl;
   }
   res = res ? res : xl ? 1 : yl ? -1 : 0;
   if (!criteria->ascending)
      res = -res;
   return (int) res;
} 

static void sort_helper(struct sort_helper_descriptor *hdesc, uint32_t *arr, 
   size_t start, size_t end, uint32_t *scratch, 
   const struct poliqarp_sort_info *criteria, struct poliqarp_corpus *corpus, 
   progress_t *progress)
{
   size_t i = 0, length = end - start, mid = length / 2, l = start, r = l + mid;
   if (start + 1 >= end)
      return;
   sort_helper(hdesc, arr, start, start + mid, scratch, criteria, corpus, 
      progress);
   sort_helper(hdesc, arr, start + mid, end, scratch, criteria, corpus, 
      progress);
   for (i = 0; i < length; i++) {
      if (l < start + mid && 
         (r == end || sort_cmp(hdesc, arr[l], arr[r], criteria, corpus) <= 0))
      {
         scratch[i] = arr[l++];
      } else {
         scratch[i] = arr[r++];
      }  
   }
   memcpy(arr + start, scratch, length * sizeof(*arr));
#ifndef POLIQARP_SINGLE_THREADED
   pthread_testcancel();
#endif
}

struct read_serializer {
   size_t left;
   size_t right; 
   size_t pos;
};

static int compare_serializers(const void *x, const void *y)
{
   const struct read_serializer *xx = (const struct read_serializer *)x,
                                *yy = (const struct read_serializer *)y;
   return (long)xx->left - (long)yy->left;
}

static struct sort_helper_descriptor *sort_prepare_helper(
   struct poliqarp_match_buffer *buffer, 
   const struct poliqarp_sort_info *criteria)
{
   struct sort_helper_descriptor *result;
   size_t i;
   struct read_serializer *ser;
   
   if (buffer->sort_arena == NULL) {
      buffer->sort_arena = malloc(sizeof(*(buffer->sort_arena)));
      if (buffer->sort_arena == NULL)
         return NULL;
      marena_create(buffer->sort_arena);
   }
   result = malloc(buffer->used * sizeof(*result));
   if (result == NULL)
      return NULL;
   ser = malloc(buffer->used * sizeof(*ser));
   if (ser == NULL) {
      free(result);
      return NULL;
   }
   for (i = 0; i < buffer->used; i++) {
      size_t left = 0, right = 0;
      size_t segnum;
      switch (criteria->column) {
         case POLIQARP_COLUMN_LEFT_CONTEXT:
            left = buffer->match[i].start < criteria->context ? 0 :
               buffer->match[i].start - criteria->context;
            right = buffer->match[i].start;
            break;
         case POLIQARP_COLUMN_LEFT_MATCH:
            left = buffer->match[i].start;
            right = buffer->match[i].focus;
            break;
         case POLIQARP_COLUMN_MATCH:
            left = buffer->match[i].start;
            right = buffer->match[i].end;
            break;
         case POLIQARP_COLUMN_RIGHT_MATCH:
            left = buffer->match[i].focus;
            right = buffer->match[i].end;
            break;
         case POLIQARP_COLUMN_RIGHT_CONTEXT:
            left = buffer->match[i].end;
            right = buffer->match[i].end + criteria->context;
            segnum = poliqarp_backend_corpus_size(&buffer->corpus->corpus);
            if (right > segnum)
               right = segnum;
            break;
         default:
            abort(); /* Should not happen. */
      }
      ser[i].left = left;
      ser[i].right = right;
      ser[i].pos = i;
   }
   qsort(ser, buffer->used, sizeof(*ser), compare_serializers);
   for (i = 0; i < buffer->used; i++) 
   {
      size_t k = ser[i].pos, left = ser[i].left, right = ser[i].right;
      result[k].length = right - left;
      if (result[k].length > 0) {
         size_t j;
         result[k].segments = marena_alloc(buffer->sort_arena, 
            sizeof(uint32_t) * result[k].length);
         if (result[k].segments == NULL) {
            free(result);
            return NULL;
         }
         for (j = 0; j < result[k].length; j++) {
            struct poliqarp_binary_segment s = poliqarp_backend_corpus_get(
               &buffer->corpus->corpus, left + j);
            result[k].segments[j] = s.orth_space_id >> 1;
         }
      } else {
         result[k].segments = NULL;
      }
   }
   free(ser);
   return result;
}

struct poliqarp_sort_sandbox {
   uint32_t *scratch;
   uint32_t *array;
   struct sort_helper_descriptor *helper_descriptor;
   struct poliqarp_match_buffer *buffer;
};

static void poliqarp_sort_match_buffer_cleanup(void *data)
{
   struct poliqarp_sort_sandbox *cleanup_data = data;
   free(cleanup_data->scratch);
   free(cleanup_data->array);
   free(cleanup_data->helper_descriptor);
   if (cleanup_data->buffer->sort_arena != NULL)
      marena_release(cleanup_data->buffer->sort_arena);
}

/* 
 * Main sorting routine -- implements mergesort.
 *
 * Note: DO NOT use qsort(), because:
 * (a) this might not be portable; on some systems we might have to provide
 *     a replacement anyway
 * (b) it imposes a performance impact because of calls to comparator
 * (c) it is not stable by default -- it CAN be made stable, but that means
 *     performance costs
 * (d) it does not support progress reporting
 */
int poliqarp_sort_match_buffer(struct poliqarp_match_buffer *buffer,
   const struct poliqarp_sort_info *criteria, progress_t *progress)
{   
   struct poliqarp_sort_sandbox sandbox = { NULL, NULL, NULL, NULL };
   uint32_t i;
   int res = 0;
#ifndef POLIQARP_SINGLE_THREADED
   int cancel_state;
#endif

   if (buffer->used < 2)
      return 0;
   
#ifndef POLIQARP_SINGLE_THREADED
   pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cancel_state); 
   pthread_cleanup_push(poliqarp_sort_match_buffer_cleanup, &sandbox);
#endif
   sandbox.buffer = buffer;
   if ((sandbox.helper_descriptor = sort_prepare_helper(buffer, criteria)) == NULL) 
      goto err;
   sandbox.scratch = malloc(buffer->used * sizeof(*sandbox.scratch));
   if (sandbox.scratch == NULL)
   {
      res = -1;
      goto err;
   }
   sandbox.array = malloc(buffer->used * sizeof(*sandbox.array));
   if (sandbox.array == NULL)
   {
      res = -1;
      goto err;
   }
   for (i = 0; i < buffer->used; i++)
      sandbox.array[i] = i;
#ifndef POLIQARP_SINGLE_THREADED
   pthread_setcancelstate(cancel_state, NULL);
#endif   
   sort_helper(sandbox.helper_descriptor, sandbox.array, 0, buffer->used,
      sandbox.scratch, criteria, buffer->corpus, progress);
#ifndef POLIQARP_SINGLE_THREADED
   pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
#endif

   /* rearrange buffer */
   for (i = 0; i < buffer->used; i++)
      sandbox.scratch[sandbox.array[i]] = i;
   for (i = 0; i < buffer->used; i++) {
      struct poliqarp_match m = buffer->match[i];
      buffer->match[i] = buffer->match[sandbox.array[i]];
      buffer->match[sandbox.array[i]] = m;
      sandbox.array[sandbox.scratch[i]] = sandbox.array[i];
      sandbox.scratch[sandbox.array[i]] = sandbox.scratch[i];
   };
err:
#ifndef POLIQARP_SINGLE_THREADED
   ; /* just to shut up compilers */
   pthread_cleanup_pop(1);
   pthread_setcancelstate(cancel_state, NULL);
#else
   poliqarp_sort_match_buffer_cleanup(&sandbox);
#endif   
   return res;
}

int poliqarp_get_match(const struct poliqarp_match_buffer *buffer,
   struct poliqarp_match *match, size_t index)
{
#ifndef NDEBUG
   if (index >= buffer->used)
      return -1;
#endif
   *match = buffer->match[index];
   return 0;
}

int poliqarp_get_match_for_document(const struct poliqarp_corpus *corpus,
   size_t id, struct poliqarp_match *match)
{
   struct poliqarp_document document;
   if (poliqarp_backend_document_fetch(&corpus->document, id, &document) == -1)
      return -1;
   match->start = match->focus = document.corpus_low;
   match->end = document.corpus_high;
   match->document = id;
   return 0;
}

void poliqarp_append_query_result(struct poliqarp_match_buffer *this,
   const struct poliqarp_match *match)
{
   pthread_mutex_lock(&this->mutex);
   assert(this->used < this->capacity);
   this->match[this->used++] = *match;
   pthread_mutex_unlock(&this->mutex);
}


static void poliqarp_unindex_item(bitset bs, struct poliqarp_searchable_area *area,
   size_t distance, size_t what, struct poliqarp_rindex *index)
{
   uint32_t freq, i, b, pos = 0;
   size_t dmin = distance / area->granularity, 
          dmax = (distance + area->granularity - 1) / area->granularity;
   poliqarp_rindex_set(index, what);
   freq = decode_gamma(&index->ibs);
   b = get_golomb_parameter(area->num_bits, freq);
   for (i = 0; i < freq; i++) {
      uint32_t x = decode_golomb(&index->ibs, b);
      if (i == 0)
         pos = x - 1;
      else
         pos += x;
      if (pos >= dmin)
         bitset_arena_set(&area->arena, bs, pos - dmin);
      if (pos >= dmax)
         bitset_arena_set(&area->arena, bs, pos - dmax);
   }
}

static bitset poliqarp_unindex_value(struct poliqarp_value *value, 
   struct poliqarp_searchable_area *area, size_t distance, int indices, 
   const struct poliqarp_backend_index *index)
{
   struct poliqarp_rindex *my_index;
   size_t i, pos;
   bitset res;

   if (value->num_hits > POLIQARP_UNINDEX_THRESHOLD)
      return NULL;
   switch (value->domain) {
      case POLIQARP_DOMAIN_ORTH:
         my_index = index->orth_index;
         break;
      case POLIQARP_DOMAIN_SPACE:
         my_index = NULL;
         break;
      case POLIQARP_DOMAIN_INTERP__DISAMB:
         my_index = index->disamb_index;
         break;
      case POLIQARP_DOMAIN_INTERP__AMB:
         my_index = index->amb_index;
         break;
      default:
         abort(); /* Should not happen. */
   }
   if (my_index == NULL)
      return NULL;
   res = bitset_arena_alloc(&area->arena);
   i = value->num_hits;
   pos = 0;
   while (i) {
      if (BIT_ARRAY_GET(value->bits, pos)) {
         poliqarp_unindex_item(res, area, distance, pos, my_index);
         i--;
      }
      pos++;
   }
   return res;
}

static bitset poliqarp_unindex_expression(struct poliqarp_expression *expr, 
   struct poliqarp_searchable_area *area, size_t distance, int indices, 
   const struct poliqarp_backend_index *index)
{
   bitset bs_left, bs_right;
   if (expr == SYMBOL_DOT)
      return NULL;
   switch (expr->type) {
      case POLIQARP_EXPRESSION_CONSTANT:
         return NULL;
      case POLIQARP_EXPRESSION_AND:
         if (expr->as.expression.negate)
            return NULL;
         bs_left = poliqarp_unindex_expression(expr->as.expression.left, area, 
            distance, indices, index);
         bs_right = poliqarp_unindex_expression(expr->as.expression.right, area,
            distance, indices, index);
         if (bs_left == NULL)
            return bs_right;
         if (bs_right == NULL)
            return bs_left;
         bitset_arena_intersect(&area->arena, bs_left, bs_right);
         return bs_left;
      case POLIQARP_EXPRESSION_OR:
         if (expr->as.expression.negate)
            return NULL;
         bs_left = poliqarp_unindex_expression(expr->as.expression.left, area, 
            distance, indices, index);
         bs_right = poliqarp_unindex_expression(expr->as.expression.right, area,
            distance, indices, index);
         if (bs_left == NULL || bs_right == NULL)
            return NULL;
         bitset_arena_union(&area->arena, bs_left, bs_right);
         return bs_left;
      case POLIQARP_EXPRESSION_VALUE:
         if (expr->as.value.negate)
            return NULL;
         return poliqarp_unindex_value(expr->as.value.value, area, distance,
            indices, index);
      default:
         return NULL;
   };
}

void poliqarp_recursive_create_area(struct poliqarp_searchable_area *area, 
   bitset work, struct dfs_node *node, int indices, 
   const struct poliqarp_backend_index *index)
{
   bitset bs;
   struct dfs_link *link;

   if (node->distance == (size_t)-1 || node->flags.is_final) {
      bitset_arena_union(&area->arena, area->area, work);
      return;
   }
   bs = bitset_arena_copy(&area->arena, work);
   for (link = node->first_link; link != NULL; link = link->next) {
      bitset link_bs;

      bitset_arena_copy_to(&area->arena, bs, work);
      link_bs = poliqarp_unindex_expression(link->symbol, area, node->distance,
         indices, index);
      if (link_bs)
         bitset_arena_intersect(&area->arena, work, link_bs);
      poliqarp_recursive_create_area(area, work, link->to, indices, index);
   }
}

struct dfs_queue_item {
   void* symbol;
   const struct dfs_node *node;
   size_t distance;
};

struct dfs_queue {
   struct dfs_queue_item *items;
   size_t start;
   size_t length;
   size_t max_length;
   size_t max_distance;
   bool overflown;
};

static void dfs_queue_create(struct dfs_queue *queue, size_t max_length, size_t max_distance)
{
   queue->items = calloc(max_length, sizeof(struct dfs_queue_item));
   if (queue->items == NULL)
      abort();
   queue->start = 0;
   queue->length = 0;
   queue->max_length = max_length;
   queue->max_distance = max_distance;
   queue->overflown = false;
}

static void dfs_queue_destroy(struct dfs_queue *queue)
{
   free(queue->items);
   queue->items = NULL;
}

static void dfs_queue_push(struct dfs_queue *queue, void *symbol,
   const struct dfs_node *node, size_t distance)
{
   size_t i;
   if (queue->overflown)
      return;
   if (node->flags.is_final && distance < queue->max_distance)
      queue->max_distance = distance;
   if (distance > queue->max_distance)
   {
      queue->overflown = true;
      return;
   }
   if (queue->length >= queue->max_length)
   {
      queue->overflown = true;
      while (queue->length > 0)
      {
         i = (queue->start + queue->length - 1) % queue->max_length;
         if (queue->items[i].distance >= distance)
            queue->length--;
         else
            break;
      }
      return;
   }
   i = (queue->start + queue->length) % queue->max_length;
   queue->items[i].symbol = symbol;
   queue->items[i].node = node;
   queue->items[i].distance = distance;
   queue->length++;
}

static const struct dfs_queue_item* dfs_queue_pop(struct dfs_queue *queue)
{
   size_t i;
   if (queue->length == 0)
      return NULL;
   i = queue->start;
   queue->start = (queue->start + 1) % queue->max_length;
   queue->length--;
   return queue->items + i;
}

static void poliqarp_bfs_create_area(struct poliqarp_searchable_area *area, 
   struct dfs_node *start_node, size_t num_nodes,
   int indices, const struct poliqarp_backend_index *index)
{
   bitset work_bs;
   size_t current_distance = 0;
   const struct dfs_queue_item *queue_item;
   const struct dfs_link *link;
   struct dfs_queue queue;
   dfs_queue_create(&queue, 3 * num_nodes, num_nodes);
   work_bs = bitset_arena_alloc(&area->arena);
   for (link = start_node->first_link; link != NULL; link = link->next)
      dfs_queue_push(&queue, link->symbol, link->to, 0);
   while (1)
   {
      queue_item = dfs_queue_pop(&queue);
      if (queue_item == NULL)
         break;
      if (queue_item->distance > current_distance)
      {
         if (work_bs != NULL)
            bitset_arena_intersect(&area->arena, area->area, work_bs);
         work_bs = poliqarp_unindex_expression(queue_item->symbol, area,
            queue_item->distance, indices, index);
         current_distance++;
      }
      else if (work_bs)
      {
         bitset tmp_bs = poliqarp_unindex_expression(queue_item->symbol, area,
            queue_item->distance, indices, index);
         if (tmp_bs)
            bitset_arena_union(&area->arena, work_bs, tmp_bs);
         else
            work_bs = NULL;
      }
      for (link = queue_item->node->first_link; link != NULL; link = link->next)
         dfs_queue_push(&queue, link->symbol, link->to, current_distance + 1);
   }
   if (work_bs != NULL)
      bitset_arena_intersect(&area->arena, area->area, work_bs);
   dfs_queue_destroy(&queue);
}

void poliqarp_create_search_area(struct poliqarp_query *this)
{
   struct poliqarp_searchable_area *area = &this->area;
   size_t num_segments = poliqarp_backend_corpus_size(&this->corpus->corpus);

   if (this->corpus->config.cdf.indices == 0) {
      area->num_bits = 1;
      area->granularity = num_segments;
   } else {
      area->granularity = this->corpus->config.cdf.granularity;
      area->num_bits = num_segments / area->granularity;
      if (area->num_bits * area->granularity < num_segments)
         area->num_bits++;
   }

   bitset_arena_create(&area->arena, area->num_bits, NULL);
   area->area = bitset_arena_alloc(&area->arena);
   if (this->corpus->config.cdf.indices == 0) {
      bitset_arena_set(&area->arena, area->area, 0);
   } else {
      /* Try depth-first search to restrict area: */
      bitset work = bitset_arena_alloc_ones(&area->arena);
      poliqarp_recursive_create_area(area, work, this->graph.dfs.root,
         this->corpus->config.cdf.indices, poliqarp_get_const_backend(this->corpus,
            index));
      /* Try breadth-first search to restrict area: */
      poliqarp_bfs_create_area(area,
         this->graph.dfs.root, this->graph.dfs.num_nodes,
         this->corpus->config.cdf.indices,
         poliqarp_get_const_backend(this->corpus, index));
   }
   area->num_segments = area->granularity * bitset_count_ones(&area->arena, 
      area->area);
   if (bitset_arena_get(&area->arena, area->area, area->num_bits - 1)) 
      area->num_segments = area->num_segments - area->granularity + 
         (num_segments - 1) % area->granularity + 1;
}
