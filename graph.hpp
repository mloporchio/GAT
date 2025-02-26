/**
 * @file graph.hpp
 * @author Matteo Loporchio
 * @date 2025-02-16
 * 
 *  This file contains the definitions of functions for reading graphs from text files.
 *  Graphs are imported and managed using the igraph library.
 *  The currently supported graph models are:
 *
 *  1) Multigraph: a weighted directed multigraph where:
 *      - each node represents an address;
 *      - each edge (u, v) represents a token transfer from address u to v;
 *      - each edge is labelled with the amount of tokens transferred.
 *
 *  2) Collapsed graph: a weighted directed graph where:
 *      - each node represents an address;
 *      - each edge (u, v) summarizes all transfers from address u to v.
 *      - each edge is labelled with the total number of transfers and the total amount of tokens exchanged.
 */

#ifndef GRAPH_H
#define GRAPH_H

#include <cstdio>
#include <igraph.h>

/**
 * @brief Reads the multigraph edge list from a file and builds the corresponding graph.
 * 
 * @param graph stores the final graph
 * @param w_amount stores the final weight vector (with the amount of tokens transferred for each edge)
 * @param input_file text file containing the list of weighted edges
 */
void read_multigraph(igraph_t *graph, igraph_vector_t *w_amount, FILE *input_file);

/**
 * @brief Reads the collapsed graph edge list from a file and builds the corresponding graph.
 * 
 * @param graph stores the final graph
 * @param w_ntr stores the final weight vector (with total number of transfers for each edge)
 * @param w_amount stores the final weight vector (with total amount transferred for each edge)
 * @param input_file text file containing the list of weighted edges
 */
void read_collapsed_graph(igraph_t *graph, igraph_vector_t *w_ntr, igraph_vector_t *w_amount, FILE *input_file);

#endif