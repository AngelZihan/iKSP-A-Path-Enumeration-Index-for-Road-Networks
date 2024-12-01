#include "graph.h"

int main()
{
    string filename = "./USA-road-d.NY.gr";

	Graph g = Graph(filename);
	cout << "Graph loading finish" << endl;
	int ID1, ID2;

	g.loadTD("./NY.TD");
    g.vvSEInit();
    g.vvSEQuery = g.vvSE;
    g.vmEFinsihedQuery = g.vmEFinsihed;
    g.vvLEQuery = g.vvLE;
    g.vmLFinsihedQuery = g.vmLFinsihed;
    cout << "read File: " << endl;
    vector<int> kSet;
    kSet.push_back(10);
    for(int mm = 1; mm < 2; mm++){
        for(int nn = 0; nn < kSet.size(); nn++){
            int k = kSet[nn];
            ofstream outFile("./NY_result");
            outFile << filename << endl;
            vector<int> ID1List, ID2List;
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
            inGraph.close();
            vector<int> skipID1;
            vector<int> skipID2;
            int count = 0;
            for (int kk = 0; kk < ID1List.size(); kk++) {
                count ++;
                if(count == 80){
                    g.vvSEQuery = g.vvSE;
                    g.vmEFinsihedQuery = g.vmEFinsihed;
                    g.vvLEQuery = g.vvLE;
                    g.vmLFinsihedQuery = g.vmLFinsihed;
                    count = 0;
                }
                ID1 = ID1List[kk];
                ID2 = ID2List[kk];
                outFile << "ID1: " << ID1 << " ID2: " << ID2 << endl;
                outFile << "k: " << k << endl;
                int i = 0;
                vector<int> vR;
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
                for (int j = 0; j < vR.size(); ++j) {
                    outFile << vR[j] << " ";
                }
                outFile << endl;
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
                        std::cout << *it << " ";
                    }
                    cout << endl;
                    cout << j << ":" << vR[j] << endl << endl;
                }
                cout << endl;
            }
            outFile << endl;
        }
    }
	return 0;
}

