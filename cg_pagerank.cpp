/**
 * @file cg_pagerank.cpp
 * @author Matteo Loporchio
 * @date 2025-03-06
 * 
 * This program reads the collapsed graph from a file and computes the PageRank
 * for all nodes. The PageRank is computed in three different cases (all directed):
 * 
 * 1) Unweighted graph;
 * 2) Weighted graph, where the weight of each edge is the total number of transfers;
 * 3) Weighted graph, where the weight of each edge is the total amount transferred.
 * 
 * The PageRank is computed with a default damping factor of 0.85.
 * The output is written to a TSV file.
 *
 *  INPUT:
 *  The weighted edge list for the collapsed graph.
 *
 *  OUTPUT:
 *  A TSV file summarizing the PageRank for each node.
 *  The output file contains one line for each node and each line includes the following fields:
 *      - numeric identifier of the node;
 *      - PageRank of the node (unweighted);
 *      - PageRank of the node (weighted by total number of transfers);
 *      - PageRank of the node (weighted by total amount transferred).
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

#define DAMPING_FACTOR 0.85 // default damping factor for PageRank

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

    // Compute PageRank for all three cases (unweighted, weighted by number of transfers, weighted by amount).
    igraph_vector_t pagerank, pagerank_ntr, pagerank_amount;
    igraph_vector_init(&pagerank, num_nodes);
    igraph_vector_init(&pagerank_ntr, num_nodes);
    igraph_vector_init(&pagerank_amount, num_nodes);
    igraph_pagerank(&graph, IGRAPH_PAGERANK_ALGO_PRPACK, &pagerank, NULL, igraph_vss_all(), IGRAPH_DIRECTED, DAMPING_FACTOR, NULL, NULL);
    igraph_pagerank(&graph, IGRAPH_PAGERANK_ALGO_PRPACK, &pagerank_ntr, NULL, igraph_vss_all(), IGRAPH_DIRECTED, DAMPING_FACTOR, &w_ntr, NULL);
    igraph_pagerank(&graph, IGRAPH_PAGERANK_ALGO_PRPACK, &pagerank_amount, NULL, igraph_vss_all(), IGRAPH_DIRECTED, DAMPING_FACTOR, &w_amount, NULL);
 
    // Write the results to the output TSV file.
    FILE *output_file = fopen(argv[2], "w");
    if (!output_file) {
        cerr << "Error: could not open output file!\n";
        return 1;
    }
    fprintf(output_file, "node_id\tpagerank\tpagerank_ntr\tpagerank_amount\n");
    for (int i = 0; i < num_nodes; i++) {
        double p = VECTOR(pagerank)[i];
        double p_ntr = VECTOR(pagerank_ntr)[i];
        double p_amount = VECTOR(pagerank_amount)[i];
        fprintf(output_file, "%d\t%lf\t%lf\t%lf\n", i, p, p_ntr, p_amount); 
    }
    fclose(output_file);

    // Free the memory occupied by the graph.
    igraph_destroy(&graph);
    igraph_vector_destroy(&w_ntr);
    igraph_vector_destroy(&w_amount);
    igraph_vector_destroy(&pagerank);
    igraph_vector_destroy(&pagerank_ntr);
    igraph_vector_destroy(&pagerank_amount);
    
    auto end = high_resolution_clock::now();
    auto elapsed = duration_cast<nanoseconds>(end - start);

    // Print information about the program execution. 
    cout << num_nodes << '\t' << num_edges << '\t' << elapsed.count() << '\n';
    return 0;
}

