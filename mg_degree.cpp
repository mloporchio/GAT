/**
 * @file mg_degree.cpp
 * @author Matteo Loporchio
 * @date 2025-02-28
 * 
 *  This program reads the multigraph from a file and computes information 
 *  related to the degree and strength of each node. 
 *  The strength is calculated based on the amount of tokens transferred.
 *
 *  INPUT:
 *  The weighted edge list for the multigraph.
 *
 *  OUTPUT:
 *  A TSV file summarizing degree and strength properties for each node.
 *  The output file contains one line for each node. 
 *  Each line includes the following fields:
 *      - numeric identifier of the node;
 *      - in-degree of the node;
 *      - out-degree of the node;
 *      - in-strength of the node (computed according to incoming amounts);
 *      - out-strength of the node (computed according to outgoing amounts);
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
    igraph_vector_t weights;
    igraph_vector_init(&weights, 0);
    read_multigraph(&graph, &weights, input_file);
    fclose(input_file);

    // Obtain the number of nodes and edges.
    igraph_integer_t num_nodes = igraph_vcount(&graph);
    igraph_integer_t num_edges = igraph_ecount(&graph);
    
    // Compute the degree and strength for each vertex.
    igraph_vector_int_t indeg_v, outdeg_v;
    igraph_vector_t instr_v, outstr_v;
    igraph_vector_int_init(&indeg_v, num_nodes);
    igraph_vector_int_init(&outdeg_v, num_nodes);
    igraph_vector_init(&instr_v, num_nodes);
    igraph_vector_init(&outstr_v, num_nodes);
    igraph_degree(&graph, &indeg_v, igraph_vss_all(), IGRAPH_IN, 1);
    igraph_degree(&graph, &outdeg_v, igraph_vss_all(), IGRAPH_OUT, 1);
    igraph_strength(&graph, &instr_v, igraph_vss_all(), IGRAPH_IN, 1, &weights);
    igraph_strength(&graph, &outstr_v, igraph_vss_all(), IGRAPH_OUT, 1, &weights);
 
    // Write the results to the output TSV file.
    FILE *output_file = fopen(argv[2], "w");
    if (!output_file) {
        cerr << "Error: could not open output file!\n";
        return 1;
    }
    fprintf(output_file, "node_id\tin_degree\tout_degree\tin_strength\tout_strength\n");
    for (int i = 0; i < num_nodes; i++) {
        int indeg = VECTOR(indeg_v)[i];
        int outdeg = VECTOR(outdeg_v)[i];
        double instr = VECTOR(instr_v)[i];
        double outstr = VECTOR(outstr_v)[i];
        fprintf(output_file, "%d\t%d\t%d\t%lf\t%lf\n", i, indeg, outdeg, instr, outstr);
    }
    fclose(output_file);

    // Free the memory occupied by the graph.
    igraph_destroy(&graph);
    igraph_vector_destroy(&weights);
    igraph_vector_int_destroy(&indeg_v);
    igraph_vector_int_destroy(&outdeg_v);
    igraph_vector_destroy(&instr_v);
    igraph_vector_destroy(&outstr_v);
    
    auto end = high_resolution_clock::now();
    auto elapsed = duration_cast<nanoseconds>(end - start);

    // Print information about the program execution. 
    cout << num_nodes << '\t' << num_edges << '\t' << elapsed.count() << '\n';
    return 0;
}