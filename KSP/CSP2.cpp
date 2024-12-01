#include "graph.h"

int Graph::kspCSP(int ID1, int ID2, int C) 
{
	//Preprocessing
	//Build reverse SP tree
	vector<int> vSPTDistance(nodeNum, INF);
	vector<int> vSPTParent(nodeNum, -1); 
	vector<int> vSPTParentEdge(nodeNum, -1); 
	vector<int> vTmp;
	vector<vector<int> > vSPT(nodeNum, vTmp);  
	vector<int> vSPTCost(nodeNum, INF);
	vector<int> vSPTCostLB(nodeNum, INF);
	vector<int> vCostUB(nodeNum, INF);
	
	boost::thread_group threads; 
//	rSPT(ID2, vSPTDistance, vSPTParent, vSPTParentEdge, vSPT);  
	threads.add_thread(new boost::thread(&Graph::rCSPT, this, ID2, boost::ref(vSPTDistance), boost::ref(vSPTCost), boost::ref(vSPTParent), boost::ref(vSPTParentEdge), boost::ref(vSPT)));
	threads.add_thread(new boost::thread(&Graph::rCostLB, this, ID2, boost::ref(vSPTCostLB), boost::ref(vCostUB))); 
	threads.join_all();

	if(vSPTCostLB[ID1] > C) 
	{
		cout << "Min cost " << vSPTCostLB[ID1] <<  " is larger than " << C << endl;
		return -1;
	}
	
	//Generating Side Cost
	vector<vector<pair<int, int> > > adjSideDist;  
	vector<vector<pair<int, int> > > adjSideRoad;   
	vector<Edge> vSideEdge;
	vSideEdge.resize(vEdge.size());
	adjSideDist.reserve(nodeNum); 
	threads.add_thread(new boost::thread(&Graph::sideDist, this, boost::ref(vSPTDistance), boost::ref(adjSideDist), boost::ref(adjSideRoad), boost::ref(vSideEdge)));
	label lTmp;
	lTmp.pre = -1;
	lTmp.post = -1;
	lTmp.parent = -1;
	vector<label> vLabel(nodeNum, lTmp); 
	threads.add_thread(new boost::thread(&Graph::intervalLabel, this, ID2, boost::ref(vSPT), boost::ref(vSPTParent), boost::ref(vLabel)));
	threads.join_all();
	
	vector<int> vCost;
	vector<int> vDistance;
	vector<vector<int> > vvPathCandidate;	//nodes
	vector<vector<int> > vvPathCandidateEdge;//edges
	vector<int> vPathParent;				//Deviated from
	vector<int> vPathParentPos;				//Deviated Pos from Parent
		
	benchmark::heap<2, int, int> qPath(nodeNum*100); 
	/*vector<int> vPath, vPathEdge;
	int d = DijkstraPath(ID1, ID2, vPath, vPathEdge);  */
	
	//Store the forward skyline results
	//dist, cost
	//max cost = C - vSPTCostLB(u) 
	//If the expansion is dominated by any result in it, prune
	map<int, int> m;
	vector<map<int, int> > vmSkyline(nodeNum, m); 
	
	
	//Recover the first path
	vector<int> vPath, vPathEdge; 
	int p = ID1;
	vPath.push_back(p); 
	int d = 0; 
	int cTmp = 0;
	while(vSPTParent[p] != -1)
	{
		vPathEdge.push_back(vSPTParentEdge[p]);
		d += vEdge[vSPTParentEdge[p]].length;
		cTmp += vEdge[vSPTParentEdge[p]].cost;
		vmSkyline[vSPTParent[p]][d] = cTmp;
		p = vSPTParent[p];
		vPath.push_back(p);
	}
	
//	cout << d << endl;
	vvPathCandidate.push_back(vPath); 
	vDistance.push_back(d);
	vvPathCandidateEdge.push_back(vPathEdge);
	vPathParent.push_back(-1);
	vPathParentPos.push_back(0);
	
	unordered_set<int> s;
	vector<unordered_set<int> > vPTreeEdge(nodeNum, s); //All the edges in the pseudo tree, used for removeing edges. 

//	cout << vvPathCandidate.size() << endl;
	qPath.update(vvPathCandidate.size()-1, d); 
	vCost.push_back(cTmp); 
	int topPathID, topPathDistance;

/*	vector<int> vCorrect;
	ifstream ic("./c"); 
	int icc;
	while(ic >> icc)
		vCorrect.push_back(icc);
*/
	vector<PTNode> vPTreeNode; 
	PTNode PTree; 
	unordered_map<int, int> PTRoad;
	unordered_map<int, int> PTChildren;
	PTree.nodeID = ID1; 
	PTree.PTRoad = PTRoad;
	PTree.PTChildren = PTChildren; 
	vPTreeNode.push_back(PTree);


	int oldD = -1;
	int count = 0;
	while(!qPath.empty())
	{
		count++;
		qPath.extract_min(topPathID, topPathDistance);
	
//		cout << count << "\t" << topPathDistance << "\t" << vCost[topPathID] << endl;  
//		cout << "top:" << topPathDistance << endl;
		if(topPathDistance < oldD)
			cout << "Error!" << topPathDistance << "\t" << oldD << endl;
		oldD = topPathDistance;
		
		int topCost = vCost[topPathID];
		if(topCost <= C)
			return topPathDistance;
		
		int currentTreeNodeID = 0;
		for(auto& e : vvPathCandidateEdge[topPathID])
		{
			int eID2 = vEdge[e].ID2;
			if(vPTreeNode[currentTreeNodeID].PTChildren.find(eID2) == vPTreeNode[currentTreeNodeID].PTChildren.end())
			{
				PTNode PNodeNew;
				unordered_map<int, int> PTRoadTmp;
				unordered_map<int, int> PTChildrenTmp; 
				PNodeNew.nodeID = eID2;
				PNodeNew.PTRoad = PTRoadTmp;
				PNodeNew.PTChildren = PTChildrenTmp;
				vPTreeNode.push_back(PNodeNew);
				vPTreeNode[currentTreeNodeID].PTRoad[eID2] = e;
				vPTreeNode[currentTreeNodeID].PTChildren[eID2] = vPTreeNode.size()-1; 
			}

			currentTreeNodeID = vPTreeNode[currentTreeNodeID].PTChildren[eID2];  
		}
/*a		cout << "Tree Test:" << endl;
		for(auto& i : vPTreeNode)
		{
			cout << i.nodeID << "\t" << endl; 
			for(auto& j : i.PTChildren)
				cout << j.first << "," << j.second << endl;
		}
*/

		vector<vector<pair<int, int> > > adjSideDistTmp = adjSideDist;    
		vector<vector<pair<int, int> > > adjSideRoadTmp = adjSideRoad;    

/*		cout << "Top!" << endl;
		unordered_set<int> us;
		int oldv = -1; 
		int dist = 0; 
		int c = 0;
		for(auto& v : vvPathCandidate[topPathID])
		{
			if(c == vPathParentPos[topPathID])
				cout << "Deviation" << endl;
			c++;
			cout << v << "\t";
			bool b = false;
			if(oldv > -1)
			{
				for(auto v2 : adjList[oldv])
				{
					if(v2.first == v)
					{
						dist += v2.second;
						b = true;
						break;
					}
				}
				if(!b)
				{
					cout << endl << "Disconnected!" << endl;
				}
			}

			oldv = v;
			if(us.find(v) != us.end())
				cout << endl << "Loop!" << endl;
			else
				us.insert(v);
		}
		cout << endl;
		cout << "Dist compare:" << dist << "\t" << topPathDistance << endl;
*/
		
		//Store the Ignored Edge
		vector<int> vIgnoredNode;
		vIgnoredNode.reserve(nodeNum);
		
		//Sub-path, find the start of the current deviation
		vector<int> vPreviousNode;
		vector<int> vPreviousEdge;

		int i = 0;  
/*		bool bC = true;
		vPreviousNode.push_back(ID1);
		for(; i < vPathParentPos[topPathID]; i++)
		{
			int nodeID = vvPathCandidate[topPathID][i];
			vPreviousNode.push_back(vvPathCandidate[topPathID][i+1]); 
		}
		for(int iv = 0; iv < (int)vPreviousNode.size(); iv++) 
		{
			if(vPreviousNode[iv] != vCorrect[iv])
			{
				bC = false;
				break;
			}
		}
*/
		int subLength = 0; 
		int subCost = 0;
		vPreviousNode.clear();
		vPreviousNode.push_back(ID1);
		vIgnoredNode.push_back(ID1); 
//		adjSideDistTmp[ID1].clear();
//		adjSideRoadTmp[ID1].clear(); 
		currentTreeNodeID = 0;  
		for(i = 0; i < vPathParentPos[topPathID]; i++)
		{
			int nodeID = vvPathCandidate[topPathID][i];
	//		cout << "nodeID:" << nodeID << endl;
			vPreviousNode.push_back(vvPathCandidate[topPathID][i+1]);
			vPreviousEdge.push_back(vvPathCandidateEdge[topPathID][i]);
			subLength += vEdge[vvPathCandidateEdge[topPathID][i]].length; 
			subCost += vEdge[vvPathCandidateEdge[topPathID][i]].cost;  
			
//			cout << "Current Tree Node:" << vPTreeNode[currentTreeNodeID].nodeID << endl;
			currentTreeNodeID = vPTreeNode[currentTreeNodeID].PTChildren[vvPathCandidate[topPathID][i+1]];
//			CurrentTreeNode = &vPTreeNode[CurrentTreeNode->PTChildren[vvPathCandidate[topPathID][i+1]]];  
			vIgnoredNode.push_back(vvPathCandidate[topPathID][i]);
//			adjSideDistTmp[vvPathCandidate[topPathID][i]].clear();
//			adjSideRoadTmp[vvPathCandidate[topPathID][i]].clear();
			
			bool bDominated = false;
			map<int, int> m = vmSkyline[nodeID]; 
			if(m.empty())
			{
				vmSkyline[nodeID][subLength] = subCost;
				continue;
			}

			for(auto& it: m)
			{
				if(it.first < subLength && it.second < subCost) 
				{
					bDominated = true; 
					break;
				}
				else if(it.first > subLength && it.second > subCost)  
				{
					vmSkyline[nodeID].erase(it.first);
				}
			}

			if(!bDominated)
				vmSkyline[nodeID][subLength] = subCost;

		}
	

		if(subCost > C)
			continue;
		
//			cout << "Current Tree Node:" << vPTreeNode[currentTreeNodeID].nodeID << endl;
		//Remove the edges incrementally for each new path
		vIgnoredNode.push_back(vEdge[*(vvPathCandidateEdge[topPathID].begin()+i)].ID1);  
		for(auto ie = vvPathCandidateEdge[topPathID].begin() + i; ie != vvPathCandidateEdge[topPathID].end(); ie++)
		{
			int eID1 = vEdge[*ie].ID1;
			int eID2 = vEdge[*ie].ID2;
//			cout << "eID1: " << eID1 << "\tcurrentTreeNode:" << vPTreeNode[currentTreeNodeID].nodeID << endl;
		
			vIgnoredNode.push_back(eID1);
			for(int j = 0; j < (int)adjSideDistTmp[eID1].size();) 
			{
				int eID2Tmp = adjSideDistTmp[eID1][j].first;
				int eRoadID = adjSideRoadTmp[eID1][j].second;
				if(eID2Tmp == eID2)
				{
		/*			if(eID1 == 25720)  
					{
						cout << "1 Remove " << eID2Tmp << endl; 
					}
		*/			adjSideDistTmp[eID1].erase(adjSideDistTmp[eID1].begin() + j);
					adjSideRoadTmp[eID1].erase(adjSideRoadTmp[eID1].begin() + j);  
					//break;
				}
//				else if(vPTreeEdge[eID1].find(eRoadID) != vPTreeEdge[eID1].end())
//				else if(CurrentTreeNode->PTRoad.find(eID1) != CurrentTreeNode->PTRoad.end())
				else if(vPTreeNode[currentTreeNodeID].PTRoad.find(eID2Tmp) != vPTreeNode[currentTreeNodeID].PTRoad.end())
				{
		//			if(eID1 == 25720)  
		//				cout << "2 Remove " << eID2Tmp << "\t" << eRoadID << endl;  
					adjSideDistTmp[eID1].erase(adjSideDistTmp[eID1].begin() + j);
					adjSideRoadTmp[eID1].erase(adjSideRoadTmp[eID1].begin() + j);  
				}
				else
					j++;
			}
			
//			cout << "eID1:" << eID1 << "\teID2:" << eID2 << endl;
			map<int, int> m = vmSkyline[eID1]; 
			if(m.empty())
				vmSkyline[eID1][subLength] = subCost;
			
			bool bDominated = false;
			for(auto& it: m)
			{
				if(it.first < subLength && it.second < subCost) 
				{
					bDominated = true; 
					break;
				}
				else if(it.first > subLength && it.second > subCost)  
				{
					vmSkyline[eID1].erase(it.first);
				}
			}
				
			if(bDominated) 
			{
		//		if(bC)
	//				cout << "Dominated!" << eID1 << endl;
				subLength += vEdge[*ie].length; 
				subCost += vEdge[*ie].cost; 
				vPreviousNode.push_back(eID2);
				vPreviousEdge.push_back(*ie);  
				continue;
			}
			
			vmSkyline[eID1][subLength] = subCost; 

			if(subCost + vSPTCostLB[eID1] > C)
			{
				subLength += vEdge[*ie].length; 
				subCost += vEdge[*ie].cost; 
				vPreviousNode.push_back(eID2);
				vPreviousEdge.push_back(*ie);  
				continue;
			}

//			vRemovedEdgeTmp.push_back(*ie);
/*			if(eID1 == 25720) 
			{
				cout << "Remaining 1:" << endl;
				for(int j = 0; j < (int)adjSideDistTmp[eID1].size();j++)   
					cout <<  adjSideDistTmp[eID1][j].first << endl;
			}
*/
//			cout << "eID1:" << eID1 << endl;

			bool bE = false;
			int dTmp, cTmp;
			//int dTmp = DijkstraSideCostPrune(eID1, ID2, adjSideDistTmp, adjSideRoadTmp, vSPTParent, vSPTParentEdge, vSPTDistance, vLabel, vIgnoredNode, vPath, vPathEdge, bE);			
//			cout << "eID1:" << eID1 << "\tSubLength:" << subLength << endl;
			DijkstraSideCostPruneCSP(eID1, ID2, adjSideDistTmp, adjSideRoadTmp, vSPTParent, vSPTParentEdge, vSPTDistance, vSPTCost, vLabel, vIgnoredNode, vPath, vPathEdge, bE, subLength, subCost, vSPTCostLB, vmSkyline, dTmp, cTmp);     
	

		//	CurrentTreeNode = &vPTreeNode[CurrentTreeNode->PTChildren[eID2]]; 
			currentTreeNodeID = vPTreeNode[currentTreeNodeID].PTChildren[eID2]; 

			int d2 = subLength; 
			int c2 = subCost; 
			subLength += vEdge[*ie].length; 
			subCost += vEdge[*ie].cost;  

			if(bE)
			{
				d2 += dTmp; 
				c2 += cTmp; 
/*				if(eID1 == 17743)   
	//			if(bC)  
				{
					cout << "Top Path:" << endl;
					for(auto&v : vvPathCandidate[topPathID]) 
						cout << v << "\t";
					cout << endl;

					cout << "Pos:" << vPathParentPos[topPathID] << "\t" << vvPathCandidate[topPathID][vPathParentPos[topPathID]] << endl;

					cout << "Early Termination" << endl;
					cout << "eID1:" << eID1 << endl;
					cout << "Ingored Node:"; 
					for(auto& ig:vIgnoredNode)
					{
						cout << ig << "\t";
					}
					cout << endl;
					cout << d2 << "\t" << c2 << "\t" << cTmp << endl;
				}
	*/
			}
			else
			{
				if(dTmp != INF)
				{
//					cout << "Normal Termination" << endl;
					for(auto& edgeID : vPathEdge) 
					{
						d2 += vEdge[edgeID].length;
						c2 += vEdge[edgeID].cost;
					}
			/*		if(d2 == 4082)
					{
						cout << "Normal Termination" << endl; 
						cout << d2 << "\t" << c2 << "\t" << dTmp << endl; 
					} 
			*/	}
			}
			/*	if(eID1 == 17743) 
//				if(bC)  
				{
					cout << "Top Path:" << endl;
					for(auto&v : vvPathCandidate[topPathID]) 
						cout << v << "\t";
					cout << endl;

					cout << "Pos:" << vPathParentPos[topPathID] << "\t" << vvPathCandidate[topPathID][vPathParentPos[topPathID]] << endl;

					cout << "Early Termination" << endl;
					cout << "eID1:" << eID1 << endl;
					cout << "Ingored Node:"; 
					for(auto& ig:vIgnoredNode)
					{
						cout << ig << "\t";
					}
					cout << endl;
					cout << "Remaining:" << endl;
					for(int j = 0; j < (int)adjSideDistTmp[eID1].size();j++)   
						cout <<  adjSideDistTmp[eID1][j].first << endl;

					cout << "Original:" << endl;
					for(int j = 0; j < (int)adjSideDist[eID1].size();j++)   
						cout <<  adjSideDist[eID1][j].first << endl;
				
					cout << "d2:" << d2 << "\t" << c2 << "\t" << "\t" << dTmp << "\t"<< cTmp << endl;
				}
			//	d2 = INF;
*/
	
			vector<int> vPreviousNodeTmp = vPreviousNode;
			vector<int> vPreviousEdgeTmp = vPreviousEdge;
			
			vPreviousNode.push_back(eID2);
			vPreviousEdge.push_back(*ie);  

			
			if(dTmp == INF)
			{
//				cout << "Skip" << endl << endl;
				continue;
			}
			
			vPreviousNodeTmp.insert(vPreviousNodeTmp.end(), vPath.begin()+1, vPath.end());
			vPreviousEdgeTmp.insert(vPreviousEdgeTmp.end(), vPathEdge.begin(), vPathEdge.end());
	
			vvPathCandidate.push_back(vPreviousNodeTmp);
			vvPathCandidateEdge.push_back(vPreviousEdgeTmp);
			
			vPathParent.push_back(topPathID);
			vPathParentPos.push_back(ie - vvPathCandidateEdge[topPathID].begin());
		
	//		cout << "d2:" << d2 << "\t" << dTmp << endl;
			qPath.update(vvPathCandidate.size()-1, d2);
			vDistance.push_back(d2);
			vCost.push_back(c2);
			
			unordered_set<int> us;
			int oldv = -1; 
			int dist = 0; 
			int c = 0;
		//	if(d2 == 71907)
		
/*			if(eID1 == 17743 || d2 == 95635) 
//			if(bC) 
//			if(d2 == 93277)  
			{
				cout << "eID1:" << eID1 << endl;
				for(auto& v : vPreviousNodeTmp)  
				{
					bool b = false;
					cout << v << "\t";
					if(oldv > -1)
					{	
						for(int j = 0 ; j < (int)adjList[oldv].size(); j++)
						{
							if(adjList[oldv][j].first == v)
							{
								dist += adjList[oldv][j].second;
								c += adjListCost[oldv][j].second;
								b = true;
								break;
							}
						}
						if(!b)
						{
							cout << endl << "Disconnected!" << oldv << "\t" << v << endl;
						}
					}
					oldv = v;
					if(us.find(v) != us.end())
						cout << endl << "Loop!" << endl;
					else
						us.insert(v);
				}
				cout << endl;
				cout << "Cost compare:" << c << "\t" << c2 << endl; 
				cout << "Dist compare:" << dist << "\t" << d2 << endl << endl;
			}
		*/
/*			
			unordered_set<int> us;
			int oldv = -1; 
			int dist = 0; 
			int c = 0;
			for(auto& v : vPreviousNodeTmp) 
			{
				if(c == ie - vvPathCandidateEdge[topPathID].begin()) 
					cout << "Deviation" << endl;
				c++;
				cout << v << "\t";
				bool b = false;
				if(oldv > -1)
				{
					for(auto v2 : adjList[oldv])
					{
						if(v2.first == v)
						{
							dist += v2.second;
							b = true;
							break;
						}
					}
					if(!b)
					{
						cout << endl << "Disconnected!" << endl;
					}
				}
	
				oldv = v;
				if(us.find(v) != us.end())
					cout << endl << "Loop!" << endl;
				else
				us.insert(v);
			}
			cout << endl;
			cout << "Dist compare:" << dist << "\t" << d2 << endl << endl;
		
			if(dist < topPathDistance)
				cout << "Error!" << dist << "\t" << topPathDistance << endl;

				*/
		}	

		
	}

	return topPathDistance;
}
