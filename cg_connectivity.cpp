/**
 * @file cg_connectivity.cpp
 * @author Matteo Loporchio
 * @date 2025-02-28
 * 
 * 
 */

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <igraph.h>

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
    igraph_integer_t total_nodes = igraph_vcount(&graph);

    // Compute the weakly connected components of the graph.
    igraph_integer_t num_wcc;
    igraph_vector_int_t wcc_map;
    igraph_vector_int_t wcc_sizes;
    igraph_vector_int_init(&wcc_map, total_nodes);
    igraph_vector_int_init(&wcc_sizes, total_nodes);
    igraph_connected_components(&graph, &wcc_map, &wcc_sizes, &num_wcc, IGRAPH_WEAK);

    // // Extract the subgraph corresponding to the largest connected component.
    // igraph_t comp;
    // igraph_integer_t largest_comp_id = igraph_vector_int_which_max(&wcc_sizes);
    // igraph_integer_t largest_comp_size = VECTOR(wcc_sizes)[largest_comp_id];
    // igraph_vector_int_t comp_vertices;
    // igraph_vector_int_init(&comp_vertices, largest_comp_size);
    // int j = 0;
    // for (int i = 0; i < total_nodes; i++) {
    //     if (VECTOR(wcc_map)[i] == largest_comp_id) {
    //         VECTOR(comp_vertices)[j] = i;
    //         j++;
    //     }
    // }   
    // igraph_vs_t comp_vids;
    // igraph_vs_vector(&comp_vids, &comp_vertices);
    // igraph_induced_subgraph(&graph, &comp, comp_vids, IGRAPH_SUBGRAPH_AUTO);

    // // Compute the number of nodes and edges in the component.
    // igraph_integer_t comp_nodes = igraph_vcount(&comp);
    // igraph_integer_t comp_edges = igraph_ecount(&comp);

    // // Compute the clustering coefficient for all nodes in the component.
    // igraph_real_t transitivity;
    // igraph_transitivity_undirected(&comp, &transitivity, IGRAPH_TRANSITIVITY_NAN);

    // // Compute the density of the component.
    // igraph_real_t density;
    // igraph_density(&comp, &density, 0);

    //
    double coverage = (double) largest_comp_size / (double) total_nodes;

    // Free the memory occupied by the graph.
    igraph_destroy(&graph);
    igraph_vector_destroy(&w_ntr);
    igraph_vector_destroy(&w_amount);
    igraph_destroy(&comp);
    igraph_vector_int_destroy(&wcc_map);
    igraph_vector_int_destroy(&wcc_sizes);
    igraph_vector_int_destroy(&comp_vertices);
    
    auto end = high_resolution_clock::now();
    auto elapsed = duration_cast<nanoseconds>(end - start);
    cout << comp_nodes << '\t' 
        << comp_edges << '\t' 
        << transitivity << '\t' 
        << density << '\t' 
        << coverage << '\t' 
        << elapsed.count() << '\n';
    return 0;
}


