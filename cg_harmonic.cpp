/**
 * @file cg_harmonic.cpp
 * @author Matteo Loporchio
 * @date 2025-03-06
 * 
 *  This program reads the collapsed graph from a file and computes the harmonic centrality
 *  for all nodes.
 *
 *  INPUT:
 *  The weighted edge list for the collapsed graph.
 *
 *  OUTPUT:
 *  A TSV file summarizing the harmonic centrality for each node.
 *  The file contains one line for each node. Each line includes the following fields:
 *      - numeric identifier of the node;
 *      - harmonic centrality of the node.
 * 
 *  PRINT:
 *  The program prints the following information to stdout:
 *      - number of graph nodes;
 *      - number of graph edges;
 *      - elapsed time (in nanoseconds).
 */

#include <chrono>
#include <iostream>
#include "graph.hpp"

#define DAMPING_FACTOR 0.85 // This is the default damping factor for PageRank

using namespace std;
using namespace std::chrono;

int main(int argc, char **argv) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <input_file> <output_file>\n";
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

    // Compute the harmonic centrality.
    igraph_vector_t harmonic;
    igraph_vector_init(&harmonic, num_nodes);
    igraph_harmonic_centrality(&graph, &harmonic, igraph_vss_all(), IGRAPH_IN, NULL, 0);

    // Write the results to the output TSV file.
    FILE *output_file = fopen(argv[2], "w");
    if (!output_file) {
        cerr << "Error: could not open output file!\n";
        return 1;
    }
    fprintf(output_file, "node_id\tharmonic\n");
    for (int i = 0; i < num_nodes; i++) {
        double h = VECTOR(harmonic)[i];
        fprintf(output_file, "%d\t%lf\n", i, h); 
    }
    fclose(output_file);

    // Free the memory occupied by the graph.
    igraph_destroy(&graph);
    igraph_vector_destroy(&w_ntr);
    igraph_vector_destroy(&w_amount);
    igraph_vector_destroy(&harmonic);
    //igraph_vector_destroy(&harmonic_ntr);
    //igraph_vector_destroy(&pagerank_amount);
    
    auto end = high_resolution_clock::now();
    auto elapsed = duration_cast<nanoseconds>(end - start);

    // Print information about the program execution. 
    cout << num_nodes << '\t' << num_edges << '\t' << elapsed.count() << '\n';
    return 0;
}

