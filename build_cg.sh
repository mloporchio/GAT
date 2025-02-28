#!/bin/bash
#
#   Given a list of contracts, this script reads the ERC-20 transfer list of each contract and builds 
#   the corresponding collapsed graph. 
#   
#   More precisely, for each contract in the list, this script:
#   1) constructs the collapsed graph to be used with the igraph library;
#   2) constructs the unweighted version of the collapsed graph to be used with the WebGraph library;
#
#   Graph (2) is obtained from graph (1) by removing all weights associated with the edges.
#   This step is necessary because WebGraph does not support operations on weighted graphs.
#
#   INPUT:
#   No input required from the user.
#
#   OUTPUT:
#   -   For each contract, the script produces two TSV files describing the collapsed graph: 
#           - the weighted edge list (weights are the number of transfers between two nodes and 
#             total amount transferred);
#           - the node map (i.e., a mapping between address identifiers used in the original transfer list 
#             and those used for nodes);
#           
#   -   The script outputs a TSV file describing the main characteristics of the collapsed graph for each contract.
#       The file contains one row per contract with the following fields:  
#           - name: name of the contract;  
#           - num_nodes: number of nodes in the collapsed graph;  
#           - num_edges: number of edges in the collapsed graph;  
#           - elapsed_time: time taken for construction (in nanoseconds).  
#
#   Author: Matteo Loporchio
#

NAMES=("frax" "esd" "fei" "ampl" "ust")
COLLAPSED_BUILDER="CollapsedGraphBuilder"
WEBGRAPH_BUILDER="WebGraphBuilder"
INPUT_PATH="./data"
COLLAPSED_OUTPUT_PATH="./results/cg"
WEBGRAPH_OUTPUT_PATH="./results/webgraph"
OUTPUT_FILE="${COLLAPSED_OUTPUT_PATH}/cg_build_stats.tsv"
JAVA_OPTIONS=""

printf "name\tnum_nodes\tnum_edges\telapsed_time\n" > ${OUTPUT_FILE}
for NAME in ${NAMES[@]}; do
    INPUT_FILE="${INPUT_PATH}/${NAME}.csv"
    EDGE_LIST_FILE="${COLLAPSED_OUTPUT_PATH}/${NAME}_cg_el.tsv"
    NODE_MAP_FILE="${COLLAPSED_OUTPUT_PATH}/${NAME}_cg_nm.tsv"
    printf "%s\t" ${NAME} >> ${OUTPUT_FILE}
    java -Xmx128g ${COLLAPSED_BUILDER} ${INPUT_FILE} ${EDGE_LIST_FILE} ${NODE_MAP_FILE} >> ${OUTPUT_FILE}
    # Then, transform each edge list into the WebGraph BVGraph format.
    WEBGRAPH_TEMP_EL="${WEBGRAPH_OUTPUT_PATH}/${NAME}_temp_el.tsv"
    cut -d$'\t' -f1,2 "${EDGE_LIST_FILE}" > "${WEBGRAPH_TEMP_EL}"
    java -Xmx128g -cp ".:./lib/*" ${WEBGRAPH_BUILDER} "${WEBGRAPH_TEMP_EL}" "${WEBGRAPH_OUTPUT_PATH}/${NAME}"
    rm "${WEBGRAPH_TEMP_EL}" # Delete temporary edge list
done