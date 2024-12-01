#include "graph.h"

int main()
{
    string filename = "./simple";

	Graph g = Graph(filename);
	cout << "Graph loading finish" << endl;
    srand (time(NULL));

    std::chrono::high_resolution_clock::time_point t1;
    std::chrono::high_resolution_clock::time_point t2;
    std::chrono::high_resolution_clock::time_point t3;
    std::chrono::high_resolution_clock::time_point t4;
    std::chrono::duration<double> time_span;
    std::chrono::duration<double> time_span2;
	int ID1, ID2;

    t1 = std::chrono::high_resolution_clock::now();
	g.loadTD("./NY.TD");
    g.vvSEInit();
    g.vvSEQuery = g.vvSE;
    g.vmEFinsihedQuery = g.vmEFinsihed;
    g.vvLEQuery = g.vvLE;
    g.vmLFinsihedQuery = g.vmLFinsihed;
    t2 = std::chrono::high_resolution_clock::now();
    time_span = std::chrono::duration_cast<std::chrono::duration<double> >(t2 - t1);
    cout << "read File: " << endl;
    vector<int> kSet;
    kSet.push_back(10);
    for(int mm = 1; mm < 2; mm++){
        for(int nn = 0; nn < kSet.size(); nn++){
            int k = kSet[nn];
            ofstream outFile("./Simple_result");
            outFile << filename << endl;
            /*vector<int> ID1List, ID2List;
            string queryFilename = "./USA-NY-Q2.txt";
            ifstream inGraph(queryFilename);
            if(!inGraph)
                outFile << "Cannot open Map " << queryFilename << endl;
            int pID1, pID2;
            string line;
            getline(inGraph,line);
            while(!inGraph.eof())
            {
                vector<string> vs;
                boost::split(vs,line,boost::is_any_of(" "),boost::token_compress_on);
                pID1 = stoi(vs[0]);
                pID2 = stoi(vs[1]);
                ID1List.push_back(pID1);
                ID2List.push_back(pID2);
                getline(inGraph, line);
            }
            inGraph.close();*/
            double aveEnumTime = 0;
            double aveGeneTime = 0;
            int avePairNumber = 0;
            int count = 0;
            for (int kk = 0; kk < 1; kk++) {
                /*count ++;
                if(count == 80){
                    g.vvSEQuery = g.vvSE;
                    g.vmEFinsihedQuery = g.vmEFinsihed;
                    g.vvLEQuery = g.vvLE;
                    g.vmLFinsihedQuery = g.vmLFinsihed;
                    count = 0;
                }*/
                ID1 = 0;
                ID2 = 5;
                outFile << "ID1: " << ID1 << " ID2: " << ID2 << endl;
                outFile << "k: " << k << endl;
                int i = 0;
                vector<int> vR;
                t3 = std::chrono::high_resolution_clock::now();
                while(i < k)
                {
                    int r;
                    if(i == 0){
                        r = g.peakKSP(ID1, ID2, i, true);
                    }
                    else{
                        r = g.peakKSP(ID1, ID2, i, false);
                    }

                    vR.push_back(r);
                    if(r == -1)
                        break;
                    i++;
                }
                t4 = std::chrono::high_resolution_clock::now();
                time_span2 = std::chrono::duration_cast<std::chrono::duration<double> >(t2 - t1);
                aveEnumTime += time_span2.count();
                avePairNumber += 1;
                for (int j = 0; j < vR.size(); ++j) {
                    outFile << vR[j] << " ";
                }
                outFile << endl;
                outFile << "TD Time: " << time_span.count() << " Path Enum Time: "<< time_span2.count()  <<  endl;
                vector<list<int> > vlPath;
                i = 0;
                while(i < k)
                {
                    if(vR[i] == -1)
                        break;
                    list<int> lPath;
                    g.peakRetriveal(ID1, ID2, i, lPath);
                    vlPath.push_back(lPath);
                    i++;
                }
                for (int j = 0; j < vlPath.size(); ++j) {
                    for (list<int>::iterator it = vlPath[j].begin(); it != vlPath[j].end(); ++it) {
                        outFile << *it << " ";
                    }
                    outFile << endl;
                    outFile << j << ":" << vR[j] << endl << endl;
                }
                outFile << endl;

            }
            outFile << endl;
            outFile << endl;
//            outFile << "aveEnumTime: " << aveEnumTime/avePairNumber << " aveGeneTime: " << aveGeneTime/ID2List.size() << endl;
        }
    }
	return 0;
}

