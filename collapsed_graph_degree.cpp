/**
 * @file collapsed_graph_degree.cpp
 * @author Matteo Loporchio
 * @date 2025-02-16
 * 
 *  This program reads the collapsed graph from a file and computes information 
 *  related to the degree and strength of each node. The strength is calculated 
 *  based on the two weights associated with each edge, namely the total number 
 *  of transfers and the total value transferred.
 *
 *  INPUT:
 *  The weighted edge list for the collapsed graph.
 *
 *  OUTPUT:
 *  A TSV file summarizing degree and strength properties for each node.
 *  The output file contains one line for each node and each line includes 7 fields:
 *      - numeric identifier of the node;
 *      - in-degree of the node;
 *      - out-degree of the node;
 *      - in-strength of the node (computed according to total number of transfers);
 *      - out-strength of the node (computed according to total number of transfers);
 *      - in-strength of the node (computed according to total amount transferred);
 *      - out-strength of the node (computed according to total amount transferred);
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

    // Compute the degree and strength for each vertex.
    igraph_vector_int_t in_deg_v, out_deg_v;
    igraph_vector_t in_str_ntr_v, out_str_ntr_v;
    igraph_vector_t in_str_amount_v, out_str_amount_v;
    igraph_vector_int_init(&in_deg_v, num_nodes);
    igraph_vector_int_init(&out_deg_v, num_nodes);
    igraph_vector_init(&in_str_ntr_v, num_nodes);
    igraph_vector_init(&out_str_ntr_v, num_nodes);
    igraph_vector_init(&in_str_amount_v, num_nodes);
    igraph_vector_init(&out_str_amount_v, num_nodes);
    igraph_degree(&graph, &in_deg_v, igraph_vss_all(), IGRAPH_IN, 1);
    igraph_degree(&graph, &out_deg_v, igraph_vss_all(), IGRAPH_OUT, 1);
    igraph_strength(&graph, &in_str_ntr_v, igraph_vss_all(), IGRAPH_IN, 1, &w_ntr);
    igraph_strength(&graph, &out_str_ntr_v, igraph_vss_all(), IGRAPH_OUT, 1, &w_ntr);
    igraph_strength(&graph, &in_str_amount_v, igraph_vss_all(), IGRAPH_IN, 1, &w_amount);
    igraph_strength(&graph, &out_str_amount_v, igraph_vss_all(), IGRAPH_OUT, 1, &w_amount);
 
    // Write the results to the output TSV file.
    FILE *output_file = fopen(argv[2], "w");
    if (!output_file) {
        cerr << "Error: could not open output file!\n";
        return 1;
    }
    fprintf(output_file, "node_id\tin_deg\tout_deg\tin_str_ntr\tout_str_ntr\tin_str_amount\tout_str_amount\n");
    for (int i = 0; i < num_nodes; i++) {
        int in_deg = VECTOR(in_deg_v)[i];
        int out_deg = VECTOR(out_deg_v)[i];
        double in_str_ntr = VECTOR(in_str_ntr_v)[i];
        double out_str_ntr = VECTOR(out_str_ntr_v)[i];
        double in_str_amount = VECTOR(in_str_amount_v)[i];
        double out_str_amount = VECTOR(out_str_amount_v)[i];
        fprintf(output_file, "%d\t%d\t%d\t%lf\t%lf\t%lf\t%lf\n", i, in_deg, out_deg, 
        in_str_ntr, out_str_ntr, in_str_amount, out_str_amount);
    }
    fclose(output_file);

    // Free the memory occupied by the graph.
    igraph_destroy(&graph);
    igraph_vector_destroy(&w_ntr);
    igraph_vector_destroy(&w_amount);
    igraph_vector_int_destroy(&in_deg_v);
    igraph_vector_int_destroy(&out_deg_v);
    igraph_vector_destroy(&in_str_ntr_v);
    igraph_vector_destroy(&out_str_ntr_v);
    igraph_vector_destroy(&in_str_amount_v);
    igraph_vector_destroy(&out_str_amount_v);
    
    auto end = high_resolution_clock::now();
    auto elapsed = duration_cast<nanoseconds>(end - start);

    // Print information about the program execution. 
    cout << num_nodes << '\t' << num_edges << '\t' << elapsed.count() << '\n';
    return 0;
}

