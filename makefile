#
#	File:	makefile
#	Author:	Matteo Loporchio
#

CXX=g++
CXX_FLAGS=-O3 --std=c++11 -I /data/matteoL/igraph/include/igraph
LD_FLAGS=-L /data/matteoL/igraph/lib -ligraph -fopenmp

.PHONY: clean

%.o: %.cpp
	$(CXX) $(CXX_FLAGS) -c $^ 

collapsed_graph_degree: graph.o collapsed_graph_degree.o
	$(CXX) $(CXX_FLAGS) $^ -o $@ $(LD_FLAGS)

multigraph_degree: graph.o multigraph_degree.o
	$(CXX) $(CXX_FLAGS) $^ -o $@ $(LD_FLAGS)

# analyzer_gcc: analyzer_gcc.o
# 	$(CXX) $(CXX_FLAGS) $^ -o $@ $(LD_FLAGS)

# builder: builder.o
# 	$(CXX) $(CXX_FLAGS) $^ -o $@

# pl_fit: pl_fit.o
# 	$(CXX) $(CXX_FLAGS) $^ -o $@ $(LD_FLAGS)

all: collapsed_graph_degree multigraph_degree

clean:
	rm -f *.o collapsed_graph_degree multigraph_degree