/**
 * @file cg_hits.cpp
 * @author Matteo Loporchio
 * @date 2025-04-15
 * 
 * This program reads the collapsed graph from a file and computes the Hub and Authority scores
 * for all nodes in the graph. 
 * The HITS algorithm is used to compute the Hub and Authority scores for all nodes. 
 * The algorithm is run in three different cases (all directed):
 * 
 * 1) Unweighted graph;
 * 2) Weighted graph, where the weight of each edge is the total number of transfers;
 * 3) Weighted graph, where the weight of each edge is the total amount transferred.
 * 
 * The output is written to a TSV file.
 *
 *  INPUT:
 *  The weighted edge list for the collapsed graph.
 *
 *  OUTPUT:
 *  A TSV file summarizing the Hub and Authority scores for each node.
 *  The output file contains one line for each node and each line includes the following fields:
 *      - numeric identifier of the node;
 *      - Hub score of the node (unweighted);
 *      - Hub score of the node (weighted by total number of transfers);
 *      - Hub score of the node (weighted by total amount transferred);
 *      - Authority score of the node (unweighted);
 *      - Authority score of the node (weighted by total number of transfers);
 *      - Authority score of the node (weighted by total amount transferred).
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

    // Compute HITS for all three cases (unweighted, weighted by number of transfers, weighted by amount).
    igraph_vector_t hs, hs_ntr, hs_amount, as, as_ntr, as_amount;
    igraph_vector_init(&hs, num_nodes);
    igraph_vector_init(&hs_ntr, num_nodes);
    igraph_vector_init(&hs_amount, num_nodes);
    igraph_vector_init(&as, num_nodes);
    igraph_vector_init(&as_ntr, num_nodes);
    igraph_vector_init(&as_amount, num_nodes);
    igraph_hub_and_authority_scores(&graph, &hs, &as, NULL, 0, NULL, NULL);
    igraph_hub_and_authority_scores(&graph, &hs_ntr, &as_ntr, NULL, 0, &w_ntr, NULL);
    igraph_hub_and_authority_scores(&graph, &hs_amount, &as_amount, NULL, 0, &w_amount, NULL);
 
    // Write the results to the output TSV file.
    FILE *output_file = fopen(argv[2], "w");
    if (!output_file) {
        cerr << "Error: could not open output file!\n";
        return 1;
    }
    fprintf(output_file, "node_id\thub\thub_ntr\thub_amount\tauth\tauth_ntr\tauth_amount\n");
    for (int i = 0; i < num_nodes; i++) {
        double h = VECTOR(hs)[i];
        double h_ntr = VECTOR(hs_ntr)[i];
        double h_amount = VECTOR(hs_amount)[i];
        double a = VECTOR(as)[i];
        double a_ntr = VECTOR(as_ntr)[i];
        double a_amount = VECTOR(as_amount)[i];
        fprintf(output_file, "%d\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\n", i, h, h_ntr, h_amount, a, a_ntr, a_amount);
    }
    // Close the output file.
    fclose(output_file);

    // Free the memory occupied by the graph.
    igraph_destroy(&graph);
    igraph_vector_destroy(&w_ntr);
    igraph_vector_destroy(&w_amount);
    igraph_vector_destroy(&hs);
    igraph_vector_destroy(&hs_ntr);
    igraph_vector_destroy(&hs_amount);
    igraph_vector_destroy(&as);
    igraph_vector_destroy(&as_ntr);
    igraph_vector_destroy(&as_amount);
    
    auto end = high_resolution_clock::now();
    auto elapsed = duration_cast<nanoseconds>(end - start);

    // Print information about the program execution. 
    cout << num_nodes << '\t' << num_edges << '\t' << elapsed.count() << '\n';
    return 0;
}