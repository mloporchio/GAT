#!/bin/bash
#
#
#
#

NAMES=("usdt" "weth" "usdc" "dai" "frax" "esd" "fei" "ampl" "ust")
CLASS_NAME="MultigraphBuilder"
INPUT_PATH="./data"
OUTPUT_PATH="./results"
OUTPUT_FILE="${OUTPUT_PATH}/multigraph_build_stats.tsv"

printf "name\tnum_nodes\tnum_edges\telapsed_time\n" > ${OUTPUT_FILE}
for NAME in ${NAMES[@]}; do
    INPUT_FILE="${INPUT_PATH}/${NAME}.csv"
    EDGE_LIST_FILE="${OUTPUT_PATH}/${NAME}_multigraph_edges.tsv"
    NODE_MAP_FILE="${OUTPUT_PATH}/${NAME}_multigraph_nodes.tsv"
    printf "%s\t" ${NAME} >> ${OUTPUT_FILE}
    java -Xmx200g ${CLASS_NAME} ${INPUT_FILE} ${EDGE_LIST_FILE} ${NODE_MAP_FILE} >> ${OUTPUT_FILE}
done