"""
This script reads the ERC-20 transfer list of a contract and splits the transfers 
in contiguous chunks based on their timestamp. 
The script saves each chunk of transfers to a separate CSV file and creates 
a mapping file that associates each numeric chunk identifier with the timestamp.

INPUT:
The script requires the following arguments:
    1. The path to the ERC-20 transfer list.
    2. The path to the block timestamp list.
    3. The base name for each chunk file.
    4. The path to the chunk mapping file.
    5. The width of each chunk (e.g., '1D' for daily chunks).

OUTPUT:
The script produces:
    -   One transfer list for each chunk (in CSV format). 
        Each list has the same columns as the input transfer list.
    -   A TSV file containing the mapping between chunks and their time ranges. 
        The file has two columns representing:
        -   the numeric identifier of the chunk;
        -   the initial timestamp of the chunk time range 
            (i.e., the timestamp representing the leftmost endpoint of the range).

Author: Matteo Loporchio
"""

import pandas as pd
import sys

TRANSFER_FILE = sys.argv[1]
TIMESTAMP_FILE = sys.argv[2]
CHUNK_BASE_NAME = sys.argv[3]
CHUNK_MAP_FILE = sys.argv[4]
CHUNK_SIZE = sys.argv[5]

# Load the transfer list for a given contract and import the block timestamps.
df = pd.read_csv(TRANSFER_FILE, header=None, names=['block_id', 'contract_id', 'from_id', 'to_id', 'amount'])
ts_df = pd.read_csv(TIMESTAMP_FILE, header=None, names=['block_id', 'timestamp'])

# Join the two dataframes based on block identifier.
df = df.merge(ts_df, on='block_id', how='left')
# Convert the timestamp to a datetime object.
df['timestamp'] = pd.to_datetime(df['timestamp'], unit='s')
# Set the timestamp as the index.
df.set_index('timestamp', inplace=True)

# Resample the dataframe using chunks with a size equal to CHUNK_SIZE.
rdf = df.resample(CHUNK_SIZE)
# Save each chunk to a separate CSV file.
with open(CHUNK_MAP_FILE, 'w') as f:
    f.write('chunk_id\ttimestamp\n')
    for i, (timestamp, data) in enumerate(rdf):
        data.to_csv(f'{CHUNK_BASE_NAME}_{i}.csv', index=False, header=False)
        f.write(f'{i}\t{timestamp}\n')
        
# Print the number of chunks created.
print(len(rdf))