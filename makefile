#
#	File:	makefile
#	Author:	Matteo Loporchio
#

CXX=g++
CXX_FLAGS=-O3 --std=c++11 -I /data/matteoL/igraph/include/igraph
LD_FLAGS=-L /data/matteoL/igraph/lib -ligraph -fopenmp
JC=javac
JC_FLAGS=-cp ".:lib/*"

.PHONY: clean

classes:
	$(JC) $(JC_FLAGS) *.java

%.o: %.cpp
	$(CXX) $(CXX_FLAGS) -c $^ 

cg_connectivity: graph.o cg_connectivity.o
	$(CXX) $(CXX_FLAGS) $^ -o $@ $(LD_FLAGS)

cg_degree: graph.o cg_degree.o
	$(CXX) $(CXX_FLAGS) $^ -o $@ $(LD_FLAGS)

cg_distance: graph.o cg_distance.o
	$(CXX) $(CXX_FLAGS) $^ -o $@ $(LD_FLAGS)

cg_harmonic: graph.o cg_harmonic.o
	$(CXX) $(CXX_FLAGS) $^ -o $@ $(LD_FLAGS)

cg_hits: graph.o cg_hits.o
	$(CXX) $(CXX_FLAGS) $^ -o $@ $(LD_FLAGS)

cg_pagerank: graph.o cg_pagerank.o
	$(CXX) $(CXX_FLAGS) $^ -o $@ $(LD_FLAGS)

mg_degree: graph.o mg_degree.o
	$(CXX) $(CXX_FLAGS) $^ -o $@ $(LD_FLAGS)

all: classes cg_connectivity cg_degree cg_distance cg_harmonic cg_hits cg_pagerank mg_degree

clean:
	$(RM) *.class *.o cg_connectivity cg_degree cg_distance cg_harmonic cg_hits cg_pagerank mg_degree

cleanall: clean
	$(RM) results/cg/* results/mg/* results/webgraph/*