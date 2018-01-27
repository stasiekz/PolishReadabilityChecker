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

#include <sakura/common/graph.h>

void graph_nfs_create(struct nfs_env *this, struct marena *arena)
{
   this->first_node = NULL;
   this->node_array = NULL;
   this->num_nodes = 0;
   this->arena = arena;
}

int graph_nfs_to_dfs(struct graph_env *this, struct nfs_graph root)
{
   struct nfs_node *node;
   struct nfs_node **node_array;
   int rc;

   /* create node array */
   this->nfs.root = root;
   node_array = this->nfs.node_array = malloc(sizeof *this->nfs.node_array * 
      this->nfs.num_nodes);
   if (node_array == NULL)
      return -1;
   node_array += this->nfs.num_nodes - 1; /* write backward! */
   for (node = this->nfs.first_node; node != NULL; node = node->next, 
      --node_array)
   {
      *node_array = node;
   }

   /* initialize bitset */
   bitset_arena_create(&this->bitset_arena, this->nfs.num_nodes, &this->arena);

   /* transform graphs */
   {
      bitset closure;
      rc = graph_nfs_get_closure(&closure, this, this->nfs.root.start);
      if (rc != 0)
         return rc;
      this->dfs.root = graph_dfs_alloc(this, closure);
      if (this->dfs.root == NULL)
         return -1;
   }

   rc = graph_dfs_build(this, this->dfs.root);
   if (rc != 0)
      return rc;
   graph_dfs_calc_dist(this->dfs.root, 0);
   return 0;
}

void graph_nfs_destroy(struct nfs_env *this)
{
   free(this->node_array);
}

void graph_dfs_create(struct dfs_env *this, struct marena *arena)
{
   this->first_node = NULL;
   this->num_nodes = 0;
   this->arena = arena;
}

void graph_dfs_destroy(struct dfs_env *this)
{
   /* nothing is needed here */
}

int graph_create(struct graph_env *this, set_compare_fn symbol_compare, 
   set_free_fn symbol_free)
{
   int rc;
   marena_create(&this->arena);
   rc = set_create(&this->symbol_set, symbol_compare, symbol_free, &this->arena);
   if (rc != 0) {
      marena_destroy(&this->arena);
      return rc;
   }
   bitset_arena_create_dummy(&this->bitset_arena);
   graph_nfs_create(&this->nfs, &this->arena);
   graph_dfs_create(&this->dfs, &this->arena);
   return 0;
}

void graph_destroy(struct graph_env *this)
{
   set_destroy(&this->symbol_set);
   bitset_arena_destroy(&this->bitset_arena);
   graph_nfs_destroy(&this->nfs);
   graph_dfs_destroy(&this->dfs);
   marena_destroy(&this->arena);
}

void *graph_store_symbol(struct graph_env *this, void *symbol)
{
   struct set_node *node = set_put(&this->symbol_set, symbol);
   if (node == NULL)
      return NULL;
   return node->item;
}

struct nfs_node *graph_nfs_alloc(struct graph_env *this)
{
   struct nfs_node *result;

   result = marena_alloc(this->nfs.arena, sizeof *result);
   if (result == NULL)
      return NULL;
   result->next = this->nfs.first_node;
   result->first_link = NULL;
   result->id = this->nfs.num_nodes++;
   result->closure = NULL;
   result->clone = NULL;
   result->flags.is_marked = 0;

   this->nfs.first_node = result;
   return result;
}

int graph_nfs_link(struct graph_env *this, struct nfs_node *from, 
   struct nfs_node *to, void *symbol, unsigned flags)
{
   struct nfs_link *link;

   assert(from != NULL);
   assert(to != NULL);

   for (link = from->first_link; link; link = link->next)
      if (link->to == to && link->symbol == symbol) {
         link->flags |= flags;
         return 0;
      }

   link = marena_alloc(this->nfs.arena, sizeof *link);
   if (link == NULL)
      return -1;
   link->next = from->first_link;
   link->to = to;
   link->symbol = symbol;
   link->flags = flags;
   from->first_link = link;
   return 0;
}

void graph_nfs_closure_partial(struct graph_env *env, struct nfs_node *node, 
   struct nfs_node *orig)
{
   struct nfs_link *link;

   assert(node != NULL);

   /* mark this node as set */
   bitset_arena_set(&env->bitset_arena, orig->closure, node->id);

   /* create the closure recursively */
   for (link = node->first_link; link; link = link->next)
      if (link->symbol == SYMBOL_EPSILON && 
          bitset_arena_get(&env->bitset_arena, orig->closure, 
             link->to->id) == 0)
      {
         graph_nfs_closure_partial(env, link->to, orig);
      }
}

int graph_nfs_get_closure(bitset *result, struct graph_env *env, struct nfs_node *node)
{
   assert(node != NULL);

   if (node->closure == NULL) {
      node->closure = bitset_arena_alloc(&env->bitset_arena);
      if (node->closure == NULL)
         return -1;
      graph_nfs_closure_partial(env, node, node);
   }
   *result = node->closure;
   return 0;
}

int graph_nfs_atomic(struct nfs_graph *result, struct graph_env *this, void *symbol)
{
   result->start = graph_nfs_alloc(this);
   if (result->start == NULL)
      return -1;
   result->end = graph_nfs_alloc(this);
   if (result->end == NULL)
      return -1;
   return graph_nfs_link(this, result->start, result->end, symbol, 0);
}

int graph_nfs_concat(struct nfs_graph *result, struct graph_env *this, struct
   nfs_graph a, struct nfs_graph b, unsigned flags)
{
   result->start = a.start;
   result->end = b.end;
   return graph_nfs_link(this, a.end, b.start, SYMBOL_EPSILON, flags);
}

int graph_nfs_union(struct nfs_graph *result, struct graph_env *this, struct
   nfs_graph a, struct nfs_graph b, unsigned flags)
{
   result->start = graph_nfs_alloc(this);
   if (result->start == NULL)
      return -1;
   result->end = graph_nfs_alloc(this);
   if (result->end == NULL)
      return -1;
   return
      graph_nfs_link(this, result->start, a.start, SYMBOL_EPSILON, 0) ||
      graph_nfs_link(this, result->start, b.start, SYMBOL_EPSILON, 0) ||
      graph_nfs_link(this, a.end, result->end, SYMBOL_EPSILON, 0) ||
      graph_nfs_link(this, b.end, result->end, SYMBOL_EPSILON, 0);
}

int graph_nfs_copy_partial(struct graph_env *this, struct nfs_node *node)
{
   int rc;
   struct nfs_link *link;

   /* been here */
   node->flags.is_marked = 1;

   for (link = node->first_link; link; link = link->next) {
      if (link->to->clone == NULL) {
         link->to->clone = graph_nfs_alloc(this);  /* duplicate destination */
         if (link->to->clone == NULL)
            return -1;
      }

      /* duplicate the link */
      rc = graph_nfs_link(this, node->clone, link->to->clone, link->symbol,
         link->flags);
      if (rc != 0)
         return rc;

      if (link->to->flags.is_marked == 0) {
         rc = graph_nfs_copy_partial(this, link->to);
         if (rc != 0)
            return rc;
      }
   }
   return 0;
}

int graph_nfs_copy(struct nfs_graph *result, struct graph_env *this, struct nfs_graph g)
{
   struct nfs_node *node;
   struct nfs_node *clone;
   int rc;

   for (node = this->nfs.first_node; node; node = node->next) {
      node->flags.is_marked = 0;
      node->clone = NULL;
   }

   clone = graph_nfs_alloc(this);
   if (clone == NULL)
      return -1;

   g.start->clone = clone;
   g.end->flags.is_marked = 1;

   /* mumbo jumbo */
   rc = graph_nfs_copy_partial(this, g.start);
   if (rc != 0)
      return rc;

   result->start = g.start->clone;
   result->end = g.end->clone;

   return 0;
}

int graph_nfs_quantify(struct nfs_graph *result, struct graph_env *this,
   struct nfs_graph g, int n, int m)
{
   int rc;

   if (n == 0 && m == -1) {
      /* '*' */
      *result = g;
      rc =
         graph_nfs_link(this, result->start, result->end, SYMBOL_EPSILON, 0) ||
         graph_nfs_link(this, result->end, result->start, SYMBOL_EPSILON, 0);
      return rc;
   } else if (n == 0 && m == 0) {
      /* {0} is erroneous */
      result->start = result->end = NULL;
      errno = EINVAL;
      return -1;
   } else if (n == 0 && m == 1) {
      /* '?' */
      *result = g;
      rc = graph_nfs_link(this, result->start, result->end, SYMBOL_EPSILON, 0);
      return rc;
   } else if (n == 1 && m == -1) {
      /* '+' */
      *result = g;
      rc = graph_nfs_link(this, result->end, result->start, SYMBOL_EPSILON, 0);
      return rc;
   } else if (m == -1) {
      /* {n,} */
      int i;
      struct nfs_graph *copy = malloc(sizeof *copy * (n + 1));
      if (copy == NULL)
         return -1;

      copy[0] = g;

      for (i = 1; i <= n; ++i) {
         rc = graph_nfs_copy(copy + i, this, g);
         if (rc != 0) {
            free(copy);
            return rc;
         }
      }
      for (i = 0; i < n; ++i) {
         rc = graph_nfs_link(this, copy[i].end, copy[i + 1].start,
            SYMBOL_EPSILON, 0);
         if (rc != 0) {
            free(copy);
            return rc;
         }
      }

      rc = graph_nfs_link(this, copy[n].end, copy[n].start, SYMBOL_EPSILON, 0);
      if (rc != 0) {
         free(copy);
         return rc;
      }

      result->start = copy[0].start;
      result->end = copy[n].start;

      free(copy);
      return 0;
   } else if (n <= m) {
      /* {n,m} */
      int i;
      struct nfs_graph *copy = malloc(sizeof *copy * m);
      if (copy == NULL)
         return -1;

      copy[0] = g;
      for (i = 1; i < m; ++i) {
         rc = graph_nfs_copy(copy + i, this, g);
         if (rc != 0) {
            free(copy);
            return rc;
         }
      }

      result->start = graph_nfs_alloc(this);
      if (result->start == NULL) {
         free(copy);
         return -1;
      }
      result->end = graph_nfs_alloc(this);
      if (result->end == NULL) {
         free(copy);
         return -1;
      }

      /* link start -> first */
      rc = graph_nfs_link(this, result->start, copy[0].start, SYMBOL_EPSILON, 0);
      if (rc != 0) {
         free(copy);
         return rc;
      }

      /* link last -> end */
      rc = graph_nfs_link(this, copy[m - 1].end, result->end, SYMBOL_EPSILON, 0);
      if (rc != 0) {
         free(copy);
         return rc;
      }

      for (i = 0; i < m; ++i) {
         if (i > 0) {
            rc = graph_nfs_link(this, copy[i - 1].end, copy[i].start, 
               SYMBOL_EPSILON, 0);
            if (rc != 0) {
               free(copy);
               return rc;
            }
         }
         if (i >= n) {
            rc = graph_nfs_link(this, copy[i].start, result->end, SYMBOL_EPSILON, 0);
            if (rc != 0) {
               free(copy);
               return rc;
            }
         }
      }
      free(copy);
      return 0;
   } else {
      result->start = result->end = NULL;
      errno = EINVAL;
      return -1;
   }
}

/**
 * Get an existing node with this description or allocate it.
 */
struct dfs_node *graph_dfs_fetch(struct graph_env *this, bitset env_id)
{
   struct dfs_node *node;

   for (node = this->dfs.first_node; node; node = node->next)
      if (bitset_arena_compare(&this->bitset_arena, env_id, node->env_id) == 0)
         return node;

   return graph_dfs_alloc(this, env_id);
}

/**
 * Allocate a node with given description.
 */
struct dfs_node *graph_dfs_alloc(struct graph_env *env, bitset env_id)
{
   struct dfs_node *result = marena_alloc(env->dfs.arena, sizeof *result);
   if (result == NULL)
      return NULL;

   assert(env_id != NULL);

   result->next = env->dfs.first_node;
   result->first_link = NULL;
   result->dot_link = NULL;
   result->num_links = 0;
   result->env_id = env_id;
   result->id = env->dfs.num_nodes++;
   result->flags.is_final = bitset_arena_get(&env->bitset_arena, env_id,
      env->nfs.root.end->id) ? 1 : 0;
   result->flags.is_complete = 0;
   result->visited = 0;

   env->dfs.first_node = result;

   return result;
}


/**
 * Recursive friend of graph_nfs_2_dfs.
 */
int graph_dfs_build(struct graph_env *this, struct dfs_node *d_node)
{
   size_t index;
   struct nfs_link *n_link;
   struct dfs_link *d_link = NULL;
   struct nfs_node *n_node;
   unsigned flags = 0;

   assert(this != NULL);
   assert(d_node != NULL);

   /* been here */
   d_node->flags.is_complete = 1;

   /* for every nfs-node in the bitset */
   for (index = 0; 
        (index = bitset_arena_iterate(&this->bitset_arena, d_node->env_id, 
            index)) != (size_t)-1; 
        ++index) 
   {
      assert(index < this->nfs.num_nodes);
      n_node = this->nfs.node_array[index];
      /* for every link in that nfs-node */
      for (n_link = n_node->first_link; n_link != NULL; n_link = n_link->next) 
      {
         /* skip EPSILONs */
         if (n_link->symbol == SYMBOL_EPSILON) {
            flags |= n_link->flags; /* but collect their flags */
            continue;
         }
         /* treat dot symbol specially */
         if (n_link->symbol == SYMBOL_DOT) {
            if (d_node->dot_link != NULL) {
               d_link = d_node->dot_link;
               goto skip_create_d_link;
            }

            d_link = d_node->dot_link = marena_alloc(this->dfs.arena, 
               sizeof *d_node->dot_link);
            if (d_link == NULL)
               return -1;
            d_link->next = NULL;
            d_link->to = NULL;
            d_link->symbol = SYMBOL_DOT;
            d_link->flags = flags;
            d_link->path_id = bitset_arena_alloc(&this->bitset_arena);
            if (d_link->path_id == NULL)
               return -1;
            d_node->num_links++;
         } else {
            /* find the corresponding link in dfs-node */
            for (d_link = d_node->first_link; d_link != NULL; 
               d_link = d_link->next)
            {
               if (d_link->symbol == n_link->symbol)
                  goto skip_create_d_link;
            }
            /* didn't find link with this symbol? create it! */
            d_link = marena_alloc(this->dfs.arena, sizeof *d_link);
            if (d_link == NULL)
               return -1;
            d_link->next = d_node->first_link;
            d_link->to = NULL;
            d_link->symbol = n_link->symbol;
            d_link->flags = flags;
            d_link->path_id = bitset_arena_alloc(&this->bitset_arena);
            if (d_link->path_id == NULL)
               return -1;
            d_node->first_link = d_link;
            d_node->num_links++;
         }
skip_create_d_link:
         /* AND FINALLY: mark nfs-node in the path-id */
         assert(d_link != NULL);
         assert(d_link->path_id != NULL);
         {
            bitset closure;
            int rc = graph_nfs_get_closure(&closure, this, n_link->to);
            if (rc != 0)
               return rc;
            bitset_arena_union(&this->bitset_arena, d_link->path_id, closure);
         }
         d_link->flags |= n_link->flags;
      }
   }

   /* make dot link the very last link */
   if (d_node->dot_link) {
      d_link = d_node->first_link;
      if (d_link == NULL)
         d_node->first_link = d_node->dot_link;
      else {
         while (d_link->next != NULL)
            d_link = d_link->next;
         d_link->next = d_node->dot_link;
      }
   }

   /* create all targets */
   for (d_link = d_node->first_link; d_link != NULL; d_link = d_link->next) {
      d_link->to = graph_dfs_fetch(this, d_link->path_id);
      if (d_link->to == NULL)
         return -1;
   }

   /* build all unbuilt targets */
   for (d_link = d_node->first_link; d_link != NULL; d_link = d_link->next)
      if (d_link->to->flags.is_complete == 0) {
         int rc = graph_dfs_build(this, d_link->to);
         if (rc != 0)
            return rc;
      }

   return 0;
}

void graph_dfs_calc_dist(struct dfs_node *node, size_t distance)
{
   struct dfs_link *link;
   if (distance == (size_t)-1) {
      if (node->distance == (size_t)-1)
         return;
      else {
         node->distance = (size_t)-1;
         for (link = node->first_link; link != NULL; link = link->next)
            graph_dfs_calc_dist(link->to, (size_t)-1);
      }
   } else {
      if (node->visited)
         graph_dfs_calc_dist(node, (size_t)-1);
      else {
         node->visited = 1;
         node->distance = distance;
         for (link = node->first_link; link != NULL; link = link->next)
            graph_dfs_calc_dist(link->to, distance + 1);
      }
   }
}
