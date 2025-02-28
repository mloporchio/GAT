#!/bin/bash
#
#   This script reads the list of ERC-1155 transfers associated with each contract (in CSV format)
#   and constructs the multigraph of the contract. 
#
#   INPUT:
#   - Path of the directory containing all contract transfer lists in CSV format.
#
#   OUTPUT:
#   The script outputs a TSV file that describes the main characteristics of the multigraphs. 
#   The file contains one row per contract with the following fields:  
#   - name: name of the contract;  
#   - num_nodes: number of nodes in the multigraph;  
#   - num_edges: number of edges in the multigraph;  
#   - elapsed_time: time taken for construction (in nanoseconds).  
#
#   Author: Matteo Loporchio
#

NAMES=("frax" "esd" "fei" "ampl" "ust")
BUILDER_NAME="MultigraphBuilder"
INPUT_PATH="./data"
OUTPUT_PATH="./results/mg"
OUTPUT_FILE="${OUTPUT_PATH}/mg_build_stats.tsv"

printf "name\tnum_nodes\tnum_edges\telapsed_time\n" > ${OUTPUT_FILE}
for NAME in ${NAMES[@]}; do
    INPUT_FILE="${INPUT_PATH}/${NAME}.csv"
    EDGE_LIST_FILE="${OUTPUT_PATH}/${NAME}_mg_el.tsv"
    NODE_MAP_FILE="${OUTPUT_PATH}/${NAME}_mg_nm.tsv"
    printf "%s\t" ${NAME} >> ${OUTPUT_FILE}
    java -Xmx128g ${BUILDER_NAME} ${INPUT_FILE} ${EDGE_LIST_FILE} ${NODE_MAP_FILE} >> ${OUTPUT_FILE}
done