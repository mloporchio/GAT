#!/bin/bash
#
#   
#   Author: Matteo Loporchio
#

NAMES=("frax" "esd" "fei" "ampl" "ust")
MEASURES=("harmonic" "pagerank" "hits")
GRAPH_PATH="./results/cg"
OUTPUT_PATH="."
STATS_FILE="./results/cg/cg_centrality_stats.tsv"

printf "name\tmeasure\tnum_nodes\tnum_edges\telapsed_time\n" > ${STATS_FILE}
for NAME in ${NAMES[@]}; do
    printf "%s\t" ${NAME} >> ${STATS_FILE}
    EDGE_LIST_FILE="${GRAPH_PATH}/${NAME}_cg_el.tsv"
    for MEASURE in ${MEASURES[@]}; do
        printf "%s\t" ${MEASURE} >> ${STATS_FILE}
        OUTPUT_FILE="${OUTPUT_PATH}/${NAME}_cg_${MEASURE}.tsv"
        python3 "cg_centrality_${MEASURE}.py" ${EDGE_LIST_FILE} ${OUTPUT_FILE} > ${STATS_FILE}
    done
done