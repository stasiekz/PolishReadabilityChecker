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

/** 
 * @file graph.h 
 * @brief Finite-state automata represented as graphs.
 *
 * This file declares structures representing finite-state automata in
 * two flavours: non-deterministic (called NFS throughout the rest of these
 * comments) and deterministic (DFS). The automata are used to represent
 * compiled Poliqarp queries.
 */

#ifndef HAVE_GRAPH_H
#define HAVE_GRAPH_H

#include <sakura/common/set.h>
#include <sakura/common/bs.h>

/** Symbol used to represent epsilon. */
#define SYMBOL_EPSILON        NULL

/** Symbol used to represent anything. */
#define SYMBOL_DOT            (void *)(-1)

/** Link between two NFS nodes. */
struct nfs_link {
   struct nfs_link *next;  /**< Next link in the originating node. */
   struct nfs_node *to;    /**< Destination node.                  */
   void *symbol;           /**< Symbol.                            */
   unsigned flags;         /**< Flags associated with this link.   */
};

/** Link between two DFS nodes. */
struct dfs_link {
   struct dfs_link *next;  /**< Next link in the originating node. */
   struct dfs_node *to;    /**< Destination node.                  */
   void *symbol;           /**< Symbol.                            */
   bitset path_id;         /**< Bitset describing which nfs-nodes
                                are reachable via this symbol.     */
   unsigned flags;         /**< Flags associated with this link.   */
};

/** Node in an NFS graph. */
struct nfs_node {
   struct nfs_node *next;       /**< Next node in this graph environment. */
   struct nfs_link *first_link; /**< List of links.                       */
   int id;                      /**< My unique id.                        */
   struct nfs_node *clone;      /**< My clone. This is used to create 
                                     copies of graphs.                    */
   bitset closure;              /**< Closure of this node. Everything 
                                     reachable via SYMBOL_EPSILON.        */
   struct {
      unsigned is_marked:1;     /**< Used to create copies of graphs.     */
   } flags;                     /**< Flags associated with this node.     */
};

/** Node in an DFS graph. */
struct dfs_node {
   struct dfs_node *next;       /**< Next node in this graph environment. */
   struct dfs_link *first_link; /**< Normal links.                        */
   struct dfs_link *dot_link;   /**< Special dot link.                    */
   int num_links;               /**< Number of links.                     */
   const_bitset env_id;         /**< Bitset describing nfs nodes 
                                     composing this node.                 */
   int id;                      /**< My unique id.                        */
   struct {
      unsigned is_final:1;      /**< This is a final node (one that can 
                                     successfully end the graph).         */
      unsigned is_complete:1;   /**< This node has been built.            */
   } flags;                     /**< Flags associated with this node.     */
   size_t distance;             /**< Length of path from root to this node. 
                                     (size_t)-1 if cycle                  */
   int visited;                 /**< Needed when calculating the above.   */
};

/** NFS graph structure. */
struct nfs_graph {
   struct nfs_node *start;      /**< Initial node.     */
   struct nfs_node *end;        /**< Final node.       */
};

/** DFS graph structure. */
struct dfs_graph {
   struct dfs_node *start;      /**< Initial node.     */
   struct dfs_node *end;        /**< Final node.       */
};

/** NFS graph environment. Keeps graph data together. */
struct nfs_env {
   struct nfs_node *first_node;  /**< List of nodes.                     */ 
   struct nfs_node **node_array; /**< Array of nodes (when nfs->dfs 
                                      transformation begins).            */
   size_t num_nodes;             /**< Number of nodes.                   */
   struct nfs_graph root;        /**< The graph that contains all nodes. */
   struct marena *arena;         /**< Memory arena that supplies memory 
                                      for all internal allocations.      */
};

/** DFS graph environment. Keeps graph data together. */
struct dfs_env {
   struct dfs_node *first_node; /**< List of nodes.                     */
   size_t num_nodes;            /**< Number of nodes.                   */
   struct dfs_node *root;       /**< The graph that contains all nodes. */
   struct marena *arena;        /**< Memory arena that supplies memory 
                                     for all internal allocations.      */
};

/** Graph environment. Keeps graph data together. */
struct graph_env {
   struct marena arena;              /**< Graph wide memory arena. */
   struct set symbol_set;            /**< Set of symbols.          */
   struct bitset_arena bitset_arena; /**< Bitset arena.            */
   struct nfs_env nfs;               /**< NFS graph subsystem.     */
   struct dfs_env dfs;               /**< DFS graph subsystem.     */
};

/** 
 * Create a graph with the given symbol comparator and destructor. 
 * @param this Uninitialized graph object.
 * @param symbol_compare Symbol comparator.
 * @param symbol_free Symbol destructor. 
 */
int graph_create(struct graph_env *this, set_compare_fn symbol_compare,
   set_free_fn symbol_free);

/** Graph destructor. */
void graph_destroy(struct graph_env *this);

/** Store symbol in the graph environment. */
void *graph_store_symbol(struct graph_env *this, void *symbol);

/** Allocate an NFS node. */
struct nfs_node *graph_nfs_alloc(struct graph_env *this);

/** Link two NFS nodes. */
int graph_nfs_link(struct graph_env *this, struct nfs_node *from,
   struct nfs_node *to, void *symbol, unsigned flags);

/** Calculate bitset of all nodes reachable via epsilon from the given node. */
int graph_nfs_get_closure(bitset *result, struct graph_env *this,
   struct nfs_node *node);

/**
 * Create atomic NFS graph of two nodes. The nodes are connected with 
 * a single link that has the given symbol associated with it
 */
int graph_nfs_atomic(struct nfs_graph *result, struct graph_env *this,
   void *symbol);

/**
 * Concatenate two graphs together. The concatenation is achieved by adding 
 * a link from the final node of graph 'a' to the initial node of the 
 * graph 'b'. The link has the epsilon symbol associated with it and is 
 * given 'flags'.
 *
 * @return the graph containing a's initial node and b's final node
 */
int graph_nfs_concat(struct nfs_graph *result, struct graph_env *this,
   struct nfs_graph a, struct nfs_graph b, unsigned flags);

/**
 * Create a union of two graphs. The union is performed by creating two nodes
 * (that later become the resulting graph) and linking them in the following 
 * fashion:
 *
 * <ul>
 *   <li>new initial node is linked with a'a initial node</li>
 *   <li>new initial node is linked with b'a initial node</li>
 *   <li>a's final node is linked with new final node</li>
 *   <li>b's final node is linked with new final node</li>
 * </ul>
 *
 * All links are associated with the epsilon symbol and the given flags
 */
int graph_nfs_union(struct nfs_graph *result, struct graph_env *this,
   struct nfs_graph a, struct nfs_graph b, unsigned flags);

/** 
 * Return a graph containing copy of all the nodes and links
 * of the given graph.
 *
 * @bug This function is rather messy and inefficient
 */
int graph_nfs_copy(struct nfs_graph *result, struct graph_env *this,
   struct nfs_graph g);

/** 
 * Quantify the given graph.
 *
 * @bug Quantification is messy and inefficient.
 * @param n Minimum number of occurrences.
 * @param m Maximum number of occurrences.
 */
int graph_nfs_quantify(struct nfs_graph *result, struct graph_env *this,
   struct nfs_graph g, int n, int m);

/** 
 * Get node by id.
 *
 * Get a node with this description if it exists or
 * create a new node and assign it this id
 *
 * @return the node with matching id
 */
struct dfs_node *graph_dfs_fetch(struct graph_env *this, bitset env_id);

/** Allocate a node with given description. */
struct dfs_node *graph_dfs_alloc(struct graph_env *this, bitset env_id);

/** Recursive friend of nfs_to_dfs. */
int graph_dfs_build(struct graph_env *this, struct dfs_node *node);

/** Convert from nfs to dfs. */
int graph_nfs_to_dfs(struct graph_env *this, struct nfs_graph root);

/** Calculate path lengths. */
void graph_dfs_calc_dist(struct dfs_node *node, size_t distance);

#endif

/** @} */
