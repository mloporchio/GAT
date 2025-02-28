#
#	File:	makefile
#	Author:	Matteo Loporchio
#

CXX=g++
CXX_FLAGS=-O3 --std=c++11 -I /data/matteoL/igraph/include/igraph
LD_FLAGS=-L /data/matteoL/igraph/lib -ligraph -fopenmp
JC=javac
JFLAGS=-cp ".:lib/*"

.PHONY: clean

classes:
	$(JC) $(JFLAGS) *.java

%.o: %.cpp
	$(CXX) $(CXX_FLAGS) -c $^ 

cg_degree: graph.o cg_degree.o
	$(CXX) $(CXX_FLAGS) $^ -o $@ $(LD_FLAGS)

cg_connectivity: graph.o cg_connectivity.o
	$(CXX) $(CXX_FLAGS) $^ -o $@ $(LD_FLAGS)

mg_degree: graph.o mg_degree.o
	$(CXX) $(CXX_FLAGS) $^ -o $@ $(LD_FLAGS)

all: classes cg_degree cg_connectivity mg_degree

clean:
	$(RM) *.class *.o cg_degree cg_connectivity mg_degree

cleanall: clean
	$(RM) results/cg/* results/mg/* results/webgraph/*