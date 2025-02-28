#
#	File:	makefile
#	Author:	Matteo Loporchio
#

CXX=g++
CXX_FLAGS=-O3 --std=c++11 -I /data/matteoL/igraph/include/igraph
LD_FLAGS=-L /data/matteoL/igraph/lib -ligraph -fopenmp
JC=javac
JFLAGS=-cp ".:lib/*"

.SUFFIXES: .java .class
.PHONY: clean

classes:
	$(JC) $(JFLAGS) *.java

%.o: %.cpp
	$(CXX) $(CXX_FLAGS) -c $^ 

cg_degree: graph.o cg_degree.o
	$(CXX) $(CXX_FLAGS) $^ -o $@ $(LD_FLAGS)

mg_degree: graph.o mg_degree.o
	$(CXX) $(CXX_FLAGS) $^ -o $@ $(LD_FLAGS)

all: classes cg_degree mg_degree

clean:
	$(RM) *.class *.o cg_degree mg_degree