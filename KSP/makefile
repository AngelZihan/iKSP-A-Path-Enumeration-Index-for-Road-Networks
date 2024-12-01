CXX=g++ -std=c++17 
OPT=-O3

KSP: main.o graph.o tools.o Yen.o CSP.o CSP2.o cKSP.o test.o coorTest.o KSP.o eKSP.o TDKSP.o TDKSPLoop.o xxhash.o
	$(CXX) -g -o KSP main.o graph.o tools.o Yen.o CSP.o CSP2.o cKSP.o test.o KSP.o  eKSP.o TDKSP.o TDKSPLoop.o xxhash.o -lpthread -lboost_system   -lboost_thread -lboost_serialization
	$(CXX) -g -o coorTest coorTest.o graph.o tools.o xxhash.o -lpthread -lboost_system  -lboost_thread -lboost_serialization

graph.o:graph.cpp
	$(CXX) -g -c $(OPT) graph.cpp
main.o:main.cpp
	$(CXX) -g -c $(OPT) main.cpp
tools.o:tools.cpp
	$(CXX) -g -c $(OPT) tools.cpp
Yen.o:Yen.cpp
	$(CXX) -g -c $(OPT) Yen.cpp
CSP.o:CSP.cpp
	$(CXX) -g -c $(OPT) CSP.cpp
CSP2.o:CSP2.cpp
	$(CXX) -g -c $(OPT) CSP2.cpp
cKSP.o:cKSP.cpp
	$(CXX) -g -c $(OPT) cKSP.cpp
test.o:test.cpp
	$(CXX) -g -c $(OPT) test.cpp
coorTest.o:coorTest.cpp
	$(CXX) -g -c $(OPT) coorTest.cpp 
KSP.o:KSP.cpp
	$(CXX) -g -c $(OPT) KSP.cpp	
eKSP.o:eKSP.cpp
	$(CXX) -g -c $(OPT) eKSP.cpp	 
TDKSP.o:TDKSP.cpp
	$(CXX) -g -c $(OPT) TDKSP.cpp	  
TDKSPLoop.o:TDKSPLoop.cpp
	$(CXX) -g -c $(OPT) TDKSPLoop.cpp	  
xxhash.o: xxhash.c
	$(CXX) -g -c $(OPT) xxhash.c

clean:
	rm *.o
	rm KSP
