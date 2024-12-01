#include "graph.h"

int main()
{
    string filename = "./map/USA-road-d.FLA.gr";
//    string filename = "./map/MH.txt";
//    string filename = "./map/Manhattan.csv";
	Graph g = Graph(filename);
	cout << "Graph loading finish" << endl;
	srand (time(NULL));

	std::chrono::high_resolution_clock::time_point t1;
	std::chrono::high_resolution_clock::time_point t2;
    std::chrono::high_resolution_clock::time_point t3;
    std::chrono::high_resolution_clock::time_point t4;
    std::chrono::high_resolution_clock::time_point tm;
    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point end;
	std::chrono::duration<double> time_span;
    std::chrono::duration<double> time_span2;
    std::chrono::duration<double> time_span3;
    std::chrono::duration<double> time_span4;
	
	int ID1, ID2;

    t1 = std::chrono::high_resolution_clock::now();
	g.loadTD("./NY.TD");
    g.vvSEInit();
    g.vvLEInit();
    g.vvSEQuery = g.vvSE;
    g.vmEFinsihedQuery = g.vmEFinsihed;
    g.vvLEQuery = g.vvLE;
    g.vmLFinsihedQuery = g.vmLFinsihed;
    t2 = std::chrono::high_resolution_clock::now();
    time_span = std::chrono::duration_cast<std::chrono::duration<double> >(t2 - t1);
    cout << "read File: " << endl;
    vector<int> kSet;
//    kSet.push_back(100);
//    kSet.push_back(1000);
    kSet.push_back(10000);
//    kSet.push_back(100000);
//    kSet.push_back(200000);
//    kSet.push_back(500000);
//    kSet.push_back(1000000);
    for(int mm = 1; mm < 2; mm++){
        for(int nn = 0; nn < kSet.size(); nn++){
            int k = kSet[nn];
//            ofstream outFile("/export/project/zihan/E1/iKSP/TJ_Q"+ to_string(mm) +"_"+to_string(k));
//            ofstream outFile("/data/zluoat/E1/iKSP/COL_Q"+ to_string(mm) +"_"+to_string(k));
            ofstream outFile("/data/zluoat/E5/FLA_Q"+ to_string(mm) +"_"+to_string(k));
            outFile << filename << endl;
            vector<int> ID1List, ID2List;
            string queryFilename = "./E-query/FLA/USA-FLA-Q" +to_string(mm) + ".txt";
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
//TJ
//                pID1 =  g.newNodeMap[stoi(vs[0])];
//                pID2 =  g.newNodeMap[stoi(vs[1])];
                ID1List.push_back(pID1);
                ID2List.push_back(pID2);
                getline(inGraph, line);
            }
            inGraph.close();
            double aveEnumTime = 0;
            double aveGeneTime = 0;
            int avePairNumber = 0;
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
                t3 = std::chrono::high_resolution_clock::now();
                while(i < k)
                {
                    start = std::chrono::high_resolution_clock::now();
//                    outFile << "start : " << endl;
                    int r;
                    if(i == 0){
                        r = g.peakKSP(ID1, ID2, i, true);
                    }
                    else{
                        r = g.peakKSP(ID1, ID2, i, false);
                    }
//                    outFile << "start1 : " << endl;
                    end = std::chrono::high_resolution_clock::now();
//                    outFile << "start2 : " << endl;
                    time_span4 = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
//                    outFile << "start3 : " << endl;
                    vR.push_back(r);
                    if(r == -1)
                        break;
//                    outFile << "start4 : " << endl;
                    tm = std::chrono::high_resolution_clock::now();
                    time_span3 = std::chrono::duration_cast<std::chrono::duration<double>>(tm - t3);
//                    outFile << "start5 : " << endl;
                    if (time_span3.count() > 50 || time_span4.count() > 50) {
//                        std::cout << "Time exceeded 5 seconds for the function or total loop, exiting loop." << std::endl;
                        break;
                    }
                    i++;
                }
                t4 = std::chrono::high_resolution_clock::now();
                time_span2 = std::chrono::duration_cast<std::chrono::duration<double> >(t4 - t3);
                if(time_span2.count() > 50){
                    skipID1.push_back(ID1);
                    skipID2.push_back(ID2);
                    continue;
                }
                aveEnumTime += time_span2.count();
                avePairNumber += 1;
                for (int j = 0; j < vR.size(); ++j) {
                    outFile << vR[j] << " ";
                }
                outFile << endl;
                outFile << "TD Time: " << time_span.count() << " Path Enum Time: "<< time_span2.count()  <<  endl;
            }
            outFile << endl;
            outFile << "aveEnumTime: " << aveEnumTime/avePairNumber << " aveGeneTime: " << aveGeneTime/ID2List.size() << endl;
            outFile << "size of vvLEQuery: " << g.vvLEQuery.size() << " size of vvSEQuery: " << g.vvSEQuery.size() << endl;
            outFile << "Skip Pair: " << endl;
            for(int i = 0; i < skipID1.size(); i++){
                outFile << skipID1[i] << " " << skipID2[i] << endl;
            }

/*            size_t totalMemory = 0;
            for (const auto& vecLE : g.vvLEQuery) {
                for (const auto& le : vecLE) {
                    size_t structMemory = sizeof(le.aNum) + sizeof(le.ID1) + sizeof(le.ID2) + sizeof(le.KSPNumber) + sizeof(le.bFinished);

                    structMemory += le.vDistance.capacity() * sizeof(int);
                    structMemory += le.vA.capacity() * sizeof(int);
                    structMemory += sizeof(std::unordered_map<int, int>) + le.umAPos.size() * (sizeof(int) + sizeof(int));

                    for (const auto& vec : le.vNode) {
                        structMemory += vec.capacity() * sizeof(int);
                    }
                    for (const auto& vec : le.rankSet1) {
                        structMemory += vec.capacity() * sizeof(int);
                    }
                    for (const auto& vec : le.rankSet2) {
                        structMemory += vec.capacity() * sizeof(int);
                    }
                    for (const auto& vec : le.vRank) {
                        structMemory += vec.capacity() * sizeof(int);
                    }

                    structMemory += le.vK.capacity() * sizeof(int);
                    structMemory += le.vKW.capacity() * sizeof(int);
                    structMemory += le.pathTotal.capacity() * sizeof(int);
                    structMemory += le.vSubA.capacity() * sizeof(pair<int, int>);

                    for (const auto& s : le.visited) {
                        structMemory += sizeof(s) + s.size() * sizeof(pair<int, int>);
                    }

                    structMemory += le.vbFinished.capacity() * sizeof(bool);
                    structMemory += le.vpbFinished.capacity() * sizeof(pair<bool, bool>);

                    for (const auto& p : le.vpvADistance) {
                        structMemory += sizeof(p);
                        structMemory += p.first.capacity() * sizeof(int);
                        structMemory += p.second.capacity() * sizeof(int);
                    }

                    structMemory += le.vR.capacity() * sizeof(pair<std::pair<int, int>, pair<int, int>>);

                    structMemory += sizeof(std::unordered_set<string>) + le.hashSet.size() * sizeof(string);

                    totalMemory += structMemory;
                }
            }

            outFile << "Total memory overhead of vvLEQuery: " << totalMemory/1073741824 << " GB" << endl;

            size_t totalMemory1 = 0;

            for (const auto& vecSE : g.vvSEQuery) {
                for (const auto& se : vecSE) {
                    size_t structMemory = sizeof(se.wNum) + sizeof(se.ID1) + sizeof(se.ID2) + sizeof(se.bFinished);

                    structMemory += se.vDistance.capacity() * sizeof(int);
                    structMemory += se.vW.capacity() * sizeof(int);
                    structMemory += sizeof(std::unordered_map<int, int>) + se.umWPos.size() * (sizeof(int) + sizeof(int));

                    for (const auto& vec : se.vNode) {
                        structMemory += vec.capacity() * sizeof(int);
                    }
                    for (const auto& vec : se.rankSet1) {
                        structMemory += vec.capacity() * sizeof(int);
                    }
                    for (const auto& vec : se.rankSet2) {
                        structMemory += vec.capacity() * sizeof(int);
                    }
                    for (const auto& vec : se.vRank) {
                        structMemory += vec.capacity() * sizeof(int);
                    }

                    for (const auto& s : se.visited) {
                        structMemory += sizeof(s) + s.size() * sizeof(pair<int, int>);
                    }

                    structMemory += se.vK.capacity() * sizeof(int);
                    structMemory += se.vKW.capacity() * sizeof(int);
                    structMemory += se.vSubK.capacity() * sizeof(pair<int, int>);
                    structMemory += se.vbFinished.capacity() * sizeof(bool);
                    structMemory += se.sebLoop.capacity() * sizeof(bool);
                    structMemory += se.vpbFinished.capacity() * sizeof(pair<bool, bool>);

                    for (const auto& p : se.vpvWDistance) {
                        structMemory += sizeof(p);
                        structMemory += p.first.capacity() * sizeof(int);
                        structMemory += p.second.capacity() * sizeof(int);
                    }

                    structMemory += se.vR.capacity() * sizeof(pair<std::pair<int, int>, pair<int, int>>);

                    totalMemory1 += structMemory;
                }
            }

            outFile << "Total memory cost of vvSEQuery: " << totalMemory1/1073741824 << " GB" << std::endl;


            size_t totalMemory2 = 0;
            size_t structMemory = sizeof(g.bPrint) + sizeof(g.KSPNumber) + sizeof(g.bFinished);
            for (const auto& vec : g.vNode) {
                structMemory += vec.capacity() * sizeof(int);
            }
            for (const auto& vec : g.rankSet1) {
                structMemory += vec.capacity() * sizeof(int);
            }
            for (const auto& vec : g.rankSet2) {
                structMemory += vec.capacity() * sizeof(int);
            }
            for (const auto& vec : g.vRank) {
                structMemory += vec.capacity() * sizeof(int);
            }

            structMemory += g.vK.capacity() * sizeof(int);
            structMemory += g.vKW.capacity() * sizeof(int);
            structMemory += g.pathTotal.capacity() * sizeof(int);
            structMemory += g.vSubA.capacity() * sizeof(pair<int, int>);

            for (const auto& s : g.visited) {
                structMemory += sizeof(s) + s.size() * sizeof(pair<int, int>);
            }

            structMemory += g.vbFinished.capacity() * sizeof(bool);
            structMemory += g.vpbFinished.capacity() * sizeof(pair<bool, bool>);

            for (const auto& p : g.vpvADistance) {
                structMemory += sizeof(p);
                structMemory += p.first.capacity() * sizeof(int);
                structMemory += p.second.capacity() * sizeof(int);
            }

            structMemory += g.vR.capacity() * sizeof(pair<std::pair<int, int>, pair<int, int>>);

            structMemory += sizeof(std::unordered_set<string>) + g.hashSet.size() * sizeof(string);

            totalMemory2 += structMemory;

            outFile << "Total memory cost of peakLabel: " << totalMemory2 << std::endl;
            outFile.close();*/

        }
    }
	return 0;
}

