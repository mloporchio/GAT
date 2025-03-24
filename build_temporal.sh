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
#   -   A set of CSV files, each containing a chunk of the original contract transfer list.
#   -   A TSV file containing the mapping between the numeric chunk identifiers and 
#       the inital timestamp of the chunk time range.
#   
#   Author: Matteo Loporchio

NAMES=("frax" "esd" "fei" "ampl" "ust")
INPUT_PATH="./data"
OUTPUT_PATH="./results/temporal"
TIMESTAMP_FILE="data/block_timestamps_0-14999999.csv"
CHUNK_BUILDER="temporal_builder.py"
CHUNK_SIZE="1m"

mkdir -p "${OUTPUT_PATH}"

for NAME in ${NAMES[@]}; do
    INPUT_FILE="${INPUT_PATH}/${NAME}.csv"
    CURRENT_OUTPUT_PATH="${OUTPUT_PATH}/${NAME}"
    mkdir -p "${CURRENT_OUTPUT_PATH}" # This creates the contract directory, if needed.
    CHUNK_BASE_NAME="${CURRENT_OUTPUT_PATH}/${NAME}_chunk"
    CHUNK_MAP_FILE="${CURRENT_OUTPUT_PATH}/${NAME}_chunk_map.tsv"
    python3 ${CHUNK_BUILDER} ${INPUT_FILE} ${TIMESTAMP_FILE} ${CHUNK_BASE_NAME} ${CHUNK_MAP_FILE} ${CHUNK_SIZE}
done