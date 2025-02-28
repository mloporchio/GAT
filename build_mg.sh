#!/bin/bash
#
#   Given a list of contracts, this script reads the ERC-20 transfer list of each contract and builds 
#   the corresponding multigraph. 
#
#   INPUT:
#   No input required from the user.
#
#   OUTPUT:
#   -   For each contract, the script produces two TSV files describing the multigraph: 
#           - the weighted edge list (the weight of an edge is the amount transferred);
#           - the node map (i.e., a mapping between address identifiers used in the original transfer list 
#             and those used for nodes);
#           
#   -   The script outputs a TSV file describing the main characteristics of the multigraph for each contract.
#       The file contains one row per contract with the following fields:  
#           - name: name of the contract;  
#           - num_nodes: number of nodes in the multigraph;  
#           - num_edges: number of edges in the multigraph;  
#           - elapsed_time: time taken for construction (in nanoseconds).  
#
#   Author: Matteo Loporchio
#

NAMES=("frax" "esd" "fei" "ampl" "ust")
BUILDER="MultigraphBuilder"
INPUT_PATH="./data"
OUTPUT_PATH="./results/mg"
OUTPUT_FILE="${OUTPUT_PATH}/mg_build_stats.tsv"

printf "name\tnum_nodes\tnum_edges\telapsed_time\n" > ${OUTPUT_FILE}
for NAME in ${NAMES[@]}; do
    INPUT_FILE="${INPUT_PATH}/${NAME}.csv"
    EDGE_LIST_FILE="${OUTPUT_PATH}/${NAME}_mg_el.tsv"
    NODE_MAP_FILE="${OUTPUT_PATH}/${NAME}_mg_nm.tsv"
    printf "%s\t" ${NAME} >> ${OUTPUT_FILE}
    java -Xmx128g ${BUILDER} ${INPUT_FILE} ${EDGE_LIST_FILE} ${NODE_MAP_FILE} >> ${OUTPUT_FILE}
done