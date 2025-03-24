/**
 * @file cg_distance.cpp
 * @author Matteo Loporchio
 * @date 2025-03-06
 * 
 *  This program reads the collapsed graph from a file and computes 
 *  the average shortest path length between all pairs of nodes.
 *
 *  INPUT:
 *  The weighted edge list for the collapsed graph.
 *
 *  PRINT:
 *  The program prints the following information to stdout:
 *      - number of graph nodes;
 *      - number of graph edges;
 *      - average shortest path length of the graph;
 *      - elapsed time (in nanoseconds).
 */

#include <chrono>
#include <iostream>
#include "graph.hpp"

using namespace std;
using namespace std::chrono;

int main(int argc, char **argv) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <input_file>\n";
        return 1;
    }
    auto start = high_resolution_clock::now();
    
    // Load the graph from the corresponding file.
    FILE *input_file = fopen(argv[1], "r");
    if (!input_file) {
        cerr << "Error: could not open input file!\n";
        return 1;
    }
    igraph_t graph;
    igraph_vector_t w_ntr; // stores weights (total number of transfers)
    igraph_vector_t w_amount; // stores weights (total value transferred)
    igraph_vector_init(&w_ntr, 0);
    igraph_vector_init(&w_amount, 0);
    read_collapsed_graph(&graph, &w_ntr, &w_amount, input_file);
    fclose(input_file);

    // Obtain the number of nodes and edges.
    igraph_integer_t num_nodes = igraph_vcount(&graph);
    igraph_integer_t num_edges = igraph_ecount(&graph);

    // Compute the average shortest path length of the graph.
    igraph_real_t avg_distance;
    igraph_average_path_length(&graph, &avg_distance, NULL, IGRAPH_DIRECTED, 1);

    // Free the memory occupied by the graph.
    igraph_destroy(&graph);
    igraph_vector_destroy(&w_ntr);
    igraph_vector_destroy(&w_amount);

    auto end = high_resolution_clock::now();
    auto elapsed = duration_cast<nanoseconds>(end - start);

    // Print information about the program execution. 
    cout << num_nodes << '\t' 
        << num_edges << '\t' 
        << avg_distance << '\t' 
        << elapsed.count() << '\n';
    return 0;
}

