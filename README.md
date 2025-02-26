# GAT: a Graph Analysis Toolkit for ERC-20 token transfers

This repository contains a set of software tools for analyzing ERC-20 token transfer networks.

## Technologies 

- C/C++
- Java
- Bash
- Python
- ...

## Graph models

The currently supported graph models are:

1) Multigraph: a weighted directed multigraph where:
    - each node represents an Ethereum address;
    - each edge (u, v) represents a token transfer from address u to v;
    - each edge is labelled with the amount of tokens transferred.

2) Collapsed graph: a weighted directed graph where:
    - each node represents an Ethereum address;
    - each edge (u, v) summarizes all transfers from address u to v.
    - each edge is labelled with the total number of transfers and the total amount of tokens exchanged.