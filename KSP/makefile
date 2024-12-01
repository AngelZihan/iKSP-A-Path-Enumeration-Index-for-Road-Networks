CXX=g++ -std=c++17 
OPT=-O3

KSP: main.o graph.o tools.o TDKSP.o TDKSPLoop.o xxhash.o
	$(CXX) -g -o KSP main.o tools.o graph.o TDKSP.o TDKSPLoop.o xxhash.o -lpthread -lboost_system   -lboost_thread -lboost_serialization

graph.o:graph.cpp
	$(CXX) -g -c $(OPT) graph.cpp
main.o:main.cpp
	$(CXX) -g -c $(OPT) main.cpp
tools.o:tools.cpp
	$(CXX) -g -c $(OPT) tools.cpp
TDKSP.o:TDKSP.cpp
	$(CXX) -g -c $(OPT) TDKSP.cpp
TDKSPLoop.o:TDKSPLoop.cpp
	$(CXX) -g -c $(OPT) TDKSPLoop.cpp
xxhash.o: xxhash.c
	$(CXX) -g -c $(OPT) xxhash.c

clean:
	rm *.o
	rm KSP
