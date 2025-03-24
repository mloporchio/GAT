/**
 * @file graph.cpp
 * @author Matteo Loporchio
 * @date 2025-02-16
 * 
 *  This file contains the implementation of functions for reading graphs from text files.
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

#include "graph.hpp"
#include <algorithm>
#include <cstdlib>
#include <cstring>

/**
 * @brief Reads the multigraph edge list from a file and builds the corresponding graph.
 * 
 * @param graph stores the final graph
 * @param w_amount stores the final weight vector (with the amount of tokens transferred for each edge)
 * @param input_file text file containing the list of weighted edges
 */
void read_multigraph(igraph_t *graph, igraph_vector_t *weights, FILE *input_file) {
    igraph_vector_int_t edges;
    igraph_vector_int_init(&edges, 0);
    //igraph_vector_int_reserve(&edges, 1000000*2);
    int max_node_id = 0;
    char *line_buf = NULL;
    size_t line_size = 0;
    while (getline(&line_buf, &line_size, input_file) > 0) {
        char *token = NULL;
        int token_count = 0;
        int from, to;
        double value;
        while ((token = strsep(&line_buf, "\t"))) {
            // Sender address
            if (token_count == 0) from = atoi(token);
            // Receiver address
            if (token_count == 1) to = atoi(token);
            // Amount of tokens transferred
            if (token_count == 2) value = atof(token);
            token_count++;
        }
        igraph_vector_int_push_back(&edges, from);
        igraph_vector_int_push_back(&edges, to);
        igraph_vector_push_back(weights, value);
        max_node_id = std::max({max_node_id, from, to});
    }
    int num_nodes = max_node_id + 1;
    igraph_empty(graph, num_nodes, IGRAPH_DIRECTED);
    igraph_add_edges(graph, &edges, NULL);
    igraph_vector_int_destroy(&edges);
}

/**
 * @brief Reads the collapsed graph edge list from a file and builds the corresponding graph.
 * 
 * @param graph stores the final graph
 * @param w_ntr stores the final weight vector (with total number of transfers for each edge)
 * @param w_amount stores the final weight vector (with total amount transferred for each edge)
 * @param input_file text file containing the list of weighted edges
 */
void read_collapsed_graph(igraph_t *graph, igraph_vector_t *w_ntr, igraph_vector_t *w_amount, FILE *input_file) {
    igraph_vector_int_t edges;
    igraph_vector_int_init(&edges, 0);
    //igraph_vector_int_reserve(&edges, 1000000*2);
    int max_node_id = 0;
    char *line_buf = NULL;
    size_t line_size = 0;
    while (getline(&line_buf, &line_size, input_file) > 0) {
        char *token = NULL;
        int token_count = 0;
        int from, to, total_transfers;
        double total_value;
        while ((token = strsep(&line_buf, "\t"))) {
            // Field 0: sender address
            if (token_count == 0) from = atoi(token);
            // Field 1: receiver address
            if (token_count == 1) to = atoi(token);
            // Field 2: total number of transfers
            if (token_count == 2) total_transfers = atof(token);
            // Field 3: total amount transferred
            if (token_count == 3) total_value = atof(token);
            token_count++;
        }
        igraph_vector_int_push_back(&edges, from);
        igraph_vector_int_push_back(&edges, to);
        igraph_vector_push_back(w_ntr, total_transfers);
        igraph_vector_push_back(w_amount, total_value);
        max_node_id = std::max({max_node_id, from, to});
    }
    int num_nodes = max_node_id + 1;
    igraph_empty(graph, num_nodes, IGRAPH_DIRECTED);
    igraph_add_edges(graph, &edges, NULL);
    igraph_vector_int_destroy(&edges);
}

/**
 * @brief Extracts the subgraph induced by the largest weakly connected component.
 * @param graph the original graph
 * @param comp the graph representing the largest weakly connected component of the input graph
 */
void get_largest_wcc(igraph_t *graph, igraph_t *comp) {
    if (!graph || !comp) return;
    // Compute the weakly connected components of the graph.
    igraph_integer_t num_nodes = igraph_vcount(graph);
    igraph_integer_t num_wcc;
    igraph_vector_int_t wcc_map;
    igraph_vector_int_t wcc_sizes;
    igraph_vector_int_init(&wcc_map, num_nodes);
    igraph_vector_int_init(&wcc_sizes, num_nodes);
    igraph_connected_components(graph, &wcc_map, &wcc_sizes, &num_wcc, IGRAPH_WEAK);
    // Extract the subgraph corresponding to the largest connected component.
    igraph_integer_t largest_comp_id = igraph_vector_int_which_max(&wcc_sizes);
    igraph_integer_t largest_comp_size = VECTOR(wcc_sizes)[largest_comp_id];
    igraph_vector_int_t comp_vertices;
    igraph_vector_int_init(&comp_vertices, largest_comp_size);
    int j = 0;
    for (int i = 0; i < num_nodes; i++) {
        if (VECTOR(wcc_map)[i] == largest_comp_id) {
            VECTOR(comp_vertices)[j] = i;
            j++;
        }
    }   
    igraph_vs_t comp_vids;
    igraph_vs_vector(&comp_vids, &comp_vertices);
    igraph_induced_subgraph(graph, comp, comp_vids, IGRAPH_SUBGRAPH_AUTO);
}
