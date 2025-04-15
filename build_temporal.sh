#!/bin/bash
#   
#   Given a list of contracts, this script reads the ERC-20 transfer list of each contract 
#   and splits it into contiguous temporal chunks based on the timestamps of the transfers.
#
#   INPUT:
#   No input required from the user.
#
#   OUTPUT:
#   For each contract, the script produces:
#   -   A set of CSV files, each containing a contiguous chunk of the original contract transfer list.
#   -   A TSV file containing the mapping between the numeric chunk identifiers and 
#       the inital timestamp of the chunk time range.
#   
#   Author: Matteo Loporchio
#

NAMES=("frax" "esd" "fei" "ampl" "ust")
INPUT_PATH="./data"
OUTPUT_PATH="./results/temporal"
TIMESTAMP_FILE="data/block_timestamps_0-14999999.csv"
CHUNK_BUILDER="temporal_builder.py"
CHUNK_SIZE="1m"
COLLAPSED_BUILDER="CollapsedGraphBuilder"

mkdir -p "${OUTPUT_PATH}"

for NAME in ${NAMES[@]}; do
    INPUT_FILE="${INPUT_PATH}/${NAME}.csv"
    CHUNK_OUTPUT_PATH="${OUTPUT_PATH}/${NAME}"
    mkdir -p "${CHUNK_OUTPUT_PATH}" # This creates the contract directory, if needed.

    # Create the chunks.
    CHUNK_BASE_NAME="${CHUNK_OUTPUT_PATH}/${NAME}_chunk"
    CHUNK_MAP_FILE="${CHUNK_OUTPUT_PATH}/${NAME}_chunk_map.tsv"
    NUM_CHUNKS=$(python3 ${CHUNK_BUILDER} ${INPUT_FILE} ${TIMESTAMP_FILE} ${CHUNK_BASE_NAME} ${CHUNK_MAP_FILE} ${CHUNK_SIZE})

    # For each chunk, build the corresponding collapsed graph.
    STATS_FILE="${CHUNK_OUTPUT_PATH}/${NAME}_cg_build_stats.tsv"
    printf "chunk_id\tnum_nodes\tnum_edges\telapsed_time\n" > ${STATS_FILE}
    for ((i=0; i<${NUM_CHUNKS}; i++)); do
        CHUNK_FILE="${CHUNK_BASE_NAME}_${i}.csv"
        EDGE_LIST_FILE="${CHUNK_OUTPUT_PATH}/${NAME}_chunk_${i}_cg_el.tsv"
        NODE_MAP_FILE="${CHUNK_OUTPUT_PATH}/${NAME}_chunk_${i}_cg_nm.tsv"
        printf "%s\t" ${i} >> ${STATS_FILE}
        java -Xmx128g ${COLLAPSED_BUILDER} ${CHUNK_FILE} ${EDGE_LIST_FILE} ${NODE_MAP_FILE} >> ${STATS_FILE}
    done
done