/**
 * @file cg_connectivity.cpp
 * @author Matteo Loporchio
 * @date 2025-02-28
 * 
 *  This program reads the collapsed graph from a file and computes information 
 *  about the connected components of the graph. In particular, it computes the
 *  weakly and strongly connected components of the graph and writes the results
 *  to an output file, associated the component identifiers to each node.
 *
 *  INPUT:
 *  The weighted edge list for the collapsed graph.
 *
 *  OUTPUT:
 *  A TSV file summarizing the connectivity properties of each node.
 *  The output file contains one line for each node.
 *  Each line includes the following fields:
 *      - identifier of the node;
 *      - identifier of the weakly connected component to which the node belongs;
 *      - identifier of the strongly connected component to which the node belongs;
 *
 *  PRINT:
 *  The program prints the following information to stdout:
 *      - number of graph nodes;
 *      - number of graph edges;
 *      - number of weakly connected components;
 *      - number of strongly connected components;
 *      - elapsed time (in nanoseconds).
 */

#include <chrono>
#include <iostream>
#include "graph.hpp"

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

    // Compute the weakly and strongly connected components of the graph.
    igraph_integer_t num_wcc, num_scc;
    igraph_vector_int_t wcc_map, scc_map;
    igraph_vector_int_init(&wcc_map, num_nodes);
    igraph_vector_int_init(&scc_map, num_nodes);
    igraph_connected_components(&graph, &wcc_map, NULL, &num_wcc, IGRAPH_WEAK);
    igraph_connected_components(&graph, &scc_map, NULL, &num_scc, IGRAPH_STRONG);

    // Write the results to the output TSV file.
    FILE *output_file = fopen(argv[2], "w");
    if (!output_file) {
        cerr << "Error: could not open output file!\n";
        return 1;
    }
    fprintf(output_file, "node_id\twcc_id\tscc_id\n");
    for (int i = 0; i < num_nodes; i++) {
        int wcc_id = VECTOR(wcc_map)[i];
        int scc_id = VECTOR(scc_map)[i];
        fprintf(output_file, "%d\t%d\t%d\n", i, wcc_id, scc_id);
    }
    fclose(output_file);

    // Free the memory occupied by the graph.
    igraph_destroy(&graph);
    igraph_vector_destroy(&w_ntr);
    igraph_vector_destroy(&w_amount);
    igraph_vector_int_destroy(&wcc_map);
    igraph_vector_int_destroy(&scc_map);
    
    // Print information to stdout.
    auto end = high_resolution_clock::now();
    auto elapsed = duration_cast<nanoseconds>(end - start);
    cout << num_nodes << '\t' 
        << num_edges << '\t' 
        << num_wcc << '\t' 
        << num_scc << '\t' 
        << elapsed.count() << '\n';
    return 0;
}


