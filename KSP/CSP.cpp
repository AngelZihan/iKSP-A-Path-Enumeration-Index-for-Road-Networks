#include "graph.h"
	
pair<int, int> Graph::fKSPCSP(int ID1, int ID2, int C)
{
	//Preprocessing
	//Build reverse SP tree
	vector<int> vSPTDistance(nodeNum, INF);
	vector<int> vSPTCost(nodeNum, INF);
	vector<int> vSPTCostLB(nodeNum, INF);
	vector<int> vCostUB(nodeNum, INF);
	vector<int> vSPTParent(nodeNum, -1); 
	vector<int> vSPTParentEdge(nodeNum, -1); 
	vector<int> vTmp;
	vector<vector<int> > vSPT(nodeNum, vTmp);    
	
	boost::thread_group threads; 
	std::chrono::high_resolution_clock::time_point t1;
	std::chrono::high_resolution_clock::time_point t2;
	std::chrono::duration<double> time_span;
//	t1 = std::chrono::high_resolution_clock::now();
	threads.add_thread(new boost::thread(&Graph::rCSPT, this, ID2, boost::ref(vSPTDistance), boost::ref(vSPTCost), boost::ref(vSPTParent), boost::ref(vSPTParentEdge), boost::ref(vSPT)));
	threads.add_thread(new boost::thread(&Graph::rCostLB, this, ID2, boost::ref(vSPTCostLB), boost::ref(vCostUB))); 
	threads.join_all();
//	t2 = std::chrono::high_resolution_clock::now();
//	time_span = std::chrono::duration_cast<std::chrono::duration<double> >(t2 - t1);
//	cout << "Thraed Time:" << time_span.count() << endl;
	
	//Generate Side Distance
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
	
	benchmark::heap<2, int, int> qPath(nodeNum); 
//	vector<vector<int> > vvResult;	//Exact Path
//	*/vvResult.reserve(k);
//	vector<int> vDistance;
	vector<int> vCost;
	vector<vector<int> > vvPathCandidate;	//nodes
	vector<vector<int> > vvPathCandidateEdge;//edges
	vector<int> vPathParent;				//Deviated from
	vector<int> vPathParentPos;				//Deviated Pos from Parent
	vector<vector<int> > vvPathRemovedEdge; //Removed Edges when compute 
	vvPathCandidate.push_back(vPath);
//	vDistance.push_back(d);
	vvPathCandidateEdge.push_back(vPathEdge);
	vPathParent.push_back(-1);
	vPathParentPos.push_back(0);
	vvPathRemovedEdge.push_back(vTmp);
	
	unordered_set<int> s;
	vector<unordered_set<int> > vPTreeEdge(nodeNum, s); //All the edges in the pseudo tree, used for removeing edges. 

	qPath.update(vvPathCandidate.size()-1, d);  
	vCost.push_back(cTmp); 
	int topPathID, topPathDistance; 
	int pcount = 0;
	while(!qPath.empty())
	{
		pcount++;
		qPath.extract_min(topPathID, topPathDistance); 
		cout << pcount << "\t" << topPathDistance << "\t" << vCost[topPathID] << endl; 
		int topCost = vCost[topPathID];
		if(topCost <= C)
			return make_pair(topPathDistance, topCost); 
		
		//Update pseudo tree
		for(auto& e : vvPathCandidateEdge[topPathID])
		{
			int eID1 = vEdge[e].ID1;
			vPTreeEdge[eID1].insert(e);
		}

		//Generating new side graph
		vector<vector<pair<int, int> > > adjSideDistTmp = adjSideDist;     
		vector<vector<pair<int, int> > > adjSideRoadTmp = adjSideRoad;    
		
		//Store the Ignored Edge
		vector<int> vRemovedEdgeTmp; 
		vector<int> vIgnoredNode;
		vIgnoredNode.reserve(nodeNum);
/*		if(vvPathRemovedEdge[topPathID].size() > 0)
			vIgnoredNode.push_back(vEdge[*(vvPathRemovedEdge[topPathID].begin())].ID1); 
		
		for(auto& ie : vvPathRemovedEdge[topPathID])
		{
			int eID1 = vEdge[ie].ID1;
			int eID2 = vEdge[ie].ID2;
			vIgnoredNode.push_back(eID2);
			for(int i = 0; i < (int)adjSideDistTmp[eID1].size(); i++)
			{
				if(adjSideDistTmp[eID1][i].first == eID2)
				{
					adjSideDistTmp[eID1].erase(adjSideDistTmp[eID1].begin() + i);
					adjSideRoadTmp[eID1].erase(adjSideRoadTmp[eID1].begin() + i);  
					break;
				}
			}
		//	vRemovedEdgeTmp.push_back(ie); 
		}
*/		
		//Sub-path, find the start of the current deviation
		vector<int> vPreviousNode;
		vector<int> vPreviousEdge;
		int subLength = 0; 
		int subCost = 0;
		int i = 0; 
		bool bDominated = false;
		for(; i < vPathParentPos[topPathID]; i++)
		{
			int nodeID = vvPathCandidate[topPathID][i];
			int edgeID = vvPathCandidateEdge[topPathID][i];
			vPreviousNode.push_back(nodeID);
			vPreviousEdge.push_back(edgeID); 

			int eID1 = vEdge[edgeID].ID1;
			int eID2 = vEdge[edgeID].ID2;
			vIgnoredNode.push_back(eID2);
			for(int i = 0; i < (int)adjSideDistTmp[eID1].size(); i++)
			{
				if(adjSideDistTmp[eID1][i].first == eID2)
				{
					adjSideDistTmp[eID1].erase(adjSideDistTmp[eID1].begin() + i);
					adjSideRoadTmp[eID1].erase(adjSideRoadTmp[eID1].begin() + i);  
					break;
				}
			}
		
			subLength += vEdge[edgeID].length;
			subCost += vEdge[edgeID].cost; 


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

	//		if(bDominated) 
	//			continue;
			//	break;
			if(!bDominated)
				vmSkyline[nodeID][subLength] = subCost;
		}

//		if(bDominated) 
//			continue;
		if(subCost > C)
			continue;

//			break;

//		cout << "Parent pos:" << i << "\tsubcost:" << subCost << endl;
		//Remove the edges incrementally for each new path
		vIgnoredNode.push_back(vEdge[*(vvPathCandidateEdge[topPathID].begin()+i)].ID1);  
		for(auto ie = vvPathCandidateEdge[topPathID].begin() + i; ie != vvPathCandidateEdge[topPathID].end(); ie++)
		{
			int eID1 = vEdge[*ie].ID1;
			int eID2 = vEdge[*ie].ID2;
			vIgnoredNode.push_back(eID2);
			for(int j = 0; j < (int)adjSideDistTmp[eID1].size(); j++)
			{
				if(adjSideDistTmp[eID1][j].first == eID2)
				{
					adjSideDistTmp[eID1].erase(adjSideDistTmp[eID1].begin() + j);
					adjSideRoadTmp[eID1].erase(adjSideRoadTmp[eID1].begin() + j);  
					break;
				}
			}
			vRemovedEdgeTmp.push_back(*ie);
			
			subLength += vEdge[*ie].length;
			subCost += vEdge[*ie].cost; 

			if(subCost > C)
				break;
			
			map<int, int> m = vmSkyline[eID2]; 
			if(m.empty())
				vmSkyline[eID2][subLength] = subCost;
			
			bDominated = false;
			for(auto& it: m)
			{
				if(it.first < subLength && it.second < subCost) 
				{
					bDominated = true; 
					break;
				}
				else if(it.first > subLength && it.second > subCost)  
				{
					vmSkyline[eID2].erase(it.first);
				}
			}
				
			if(bDominated)
				continue;
			vmSkyline[eID2][subLength] = subCost;
			
			int d2 = subLength; 
			int c2 = subCost; 
			
			vector<vector<pair<int, int> > > adjSideDistTmp2 = adjSideDistTmp;
			vector<vector<pair<int, int> > > adjSideRoadTmp2 = adjSideRoadTmp;
	
			for(int j = 0; j < (int)adjSideRoadTmp2[eID1].size();) 
			{
				int rID = adjSideRoadTmp2[eID1][j].second;
				if(vPTreeEdge[eID1].find(rID) != vPTreeEdge[eID1].end())
				{
					adjSideDistTmp2[eID1].erase(adjSideDistTmp2[eID1].begin()+j);
					adjSideRoadTmp2[eID1].erase(adjSideRoadTmp2[eID1].begin()+j);
				}
				else
					j++;
			}

			if(adjSideDistTmp2[eID1].size() == 0)
				continue;
			
			bool bE = false; 
			int dTmp, cTmp;
//			int dTmp = DijkstraSideCostPrune(eID1, ID2, adjSideDistTmp, adjSideRoadTmp, vSPTParent, vSPTParentEdge, vSPTDistance, vLabel, vIgnoredNode, vPath, vPathEdge, bE);     
//			cout << "current pos " << i + distance(vvPathCandidateEdge[topPathID].begin(), ie) << "\tcurrent subcost:" << subCost << endl; 
			DijkstraSideCostPruneCSP(eID1, ID2, adjSideDistTmp2, adjSideRoadTmp2, vSPTParent, vSPTParentEdge, vSPTDistance, vSPTCost, vLabel, vIgnoredNode, vPath, vPathEdge, bE, subLength, subCost, vSPTCostLB, vmSkyline, dTmp, cTmp);     
//			cout << cTmp << endl;
			if(bE)
			{
				d2 += dTmp;   
				c2 += cTmp;
			}
			else
			{
//				cout << "Normal:" << d2 << "\t" << c2 ;   
//				cout << "\tvPathEdge size" <<  vPathEdge.size() << endl;
				if(vPathEdge.size() == 0)
					continue;
				for(auto& edgeID : vPathEdge) 
				{
					d2 += vEdge[edgeID].length; 
					c2 += vEdge[edgeID].cost; 
				}
//				cout << "\t" << d2 << "\t" << c2 << endl;
			}

			if(dTmp == INF)
			{
				d2 = INF; 
				c2 = INF; 
				continue;
			}
		
			if(d2 < topPathDistance)
			{
				cout << "Error!" << endl;
				cout << d2 << "\t" << topPathDistance << endl;
				cout << subLength << "\t" << dTmp << endl;
				if(bE)
					cout << "bE True" << endl; 
				else
					cout << "bE False" << endl;
			}

			//vDistance.push_back(d2); 
			vector<int> vPreviousNodeTmp = vPreviousNode;
			vector<int> vPreviousEdgeTmp = vPreviousEdge;
			vPreviousNodeTmp.insert(vPreviousNodeTmp.end(), vPath.begin()+1, vPath.end());
			vPreviousEdgeTmp.insert(vPreviousEdgeTmp.end(), vPathEdge.begin(), vPathEdge.end());
	
			vvPathCandidate.push_back(vPreviousNodeTmp);
			vvPathCandidateEdge.push_back(vPreviousEdgeTmp);
			
			vPathParent.push_back(topPathID);
			vPathParentPos.push_back(ie - vvPathCandidateEdge[topPathID].begin());
		
			vCost.push_back(c2);

			vvPathRemovedEdge.push_back(vRemovedEdgeTmp);
			qPath.update(vvPathCandidate.size()-1, d2);
			
			vPreviousNode.push_back(eID2);
			vPreviousEdge.push_back(*ie);
		}	
		

	}
	
	return make_pair(-1,-1);
}

void Graph::rCSPT(int root, vector<int>& vSPTDistance, vector<int>& vSPTCost, vector<int>& vSPTParent, vector<int>& vSPTParentEdge, vector<vector<int> >& vSPT)   
{
	benchmark::heap<2, int, int> queue(nodeNum); 
	queue.update(root, 0);

	vector<bool> vbVisited(nodeNum, false);
	int topNodeID, neighborNodeID, neighborLength, neighborRoadID, neighborCost; 

	vSPTDistance[root] = 0;
	vSPTCost[root] = 0;

	compareNode cnTop;
	while(!queue.empty())
	{
		int topDistance; 
		queue.extract_min(topNodeID, topDistance); 
		vbVisited[topNodeID] = true; 
		for(int i = 0; i < (int)adjListR[topNodeID].size(); i++)
		{
			neighborNodeID = adjListR[topNodeID][i].first;
			neighborLength = adjListR[topNodeID][i].second;
			neighborRoadID = adjListEdgeR[topNodeID][i].second;
			neighborCost = adjListCostR[topNodeID][i].second;
			int d = vSPTDistance[topNodeID] + neighborLength; 
			if(!vbVisited[neighborNodeID])
			{
				if(vSPTDistance[neighborNodeID] > d)
				{
					vSPTDistance[neighborNodeID] = d;
					queue.update(neighborNodeID, d); 
					vSPTParent[neighborNodeID] = topNodeID;
					vSPTParentEdge[neighborNodeID] = neighborRoadID;
					vSPTCost[neighborNodeID] = vSPTCost[topNodeID] + neighborCost;
//					cout << vSPTCost[neighborNodeID] << endl; 
				}
			}
		}
	}
	
	//Construct SPT
	for(int i = 0; i < nodeNum; i++)
		if(vSPTParent[i] != -1)
			vSPT[vSPTParent[i]].push_back(i);
}

void Graph::rCostLB(int root, vector<int>& vSPTCostLB, vector<int>& vCostUB) 
{
	benchmark::heap<2, int, int> queue(nodeNum); 
	queue.update(root, 0);

	vector<bool> vbVisited(nodeNum, false);
	int topNodeID, neighborNodeID, neighborCost; 

	vSPTCostLB[root] = 0;
	compareNode cnTop;
	while(!queue.empty())
	{
		int topDistance; 
		queue.extract_min(topNodeID, topDistance); 
		vbVisited[topNodeID] = true; 
		for(int i = 0; i < (int)adjListR[topNodeID].size(); i++)
		{
			neighborNodeID = adjListCostR[topNodeID][i].first;
			neighborCost = adjListCostR[topNodeID][i].second;
			int c = vSPTCostLB[topNodeID] + neighborCost; 
			if(!vbVisited[neighborNodeID])
			{
				if(vSPTCostLB[neighborNodeID] > c)
				{
					vSPTCostLB[neighborNodeID] = c; 
//					vCostUB[neighborNodeID] = C - c;
					queue.update(neighborNodeID, c); 
				}
			}
		}
	}
}

void Graph::sideDist(vector<int>& vSPTDistance, vector<vector<pair<int, int> > >& adjSideDist, vector<vector<pair<int, int> > >& adjSideRoad, vector<Edge>& vSideEdge)
{
	for(int i = 0; i < nodeNum; i++)
	{
		vector<pair<int, int> > vPair, vPairRoad;  
		if(vSPTDistance[i] == INF)
		{
			adjSideDist.push_back(vPair); 
			adjSideRoad.push_back(vPairRoad); 
			continue;
		}

		for(int j = 0; j < (int)adjList[i].size(); j++) 
		{
			int v = adjList[i][j].first;
			int d = vSPTDistance[v]; 
			int eID = adjListEdge[i][j].second;
			if(d == INF)
				continue;
			int c = d + adjList[i][j].second - vSPTDistance[i]; 
			vPair.push_back(make_pair(v, c));
			vPairRoad.push_back(make_pair(v, eID));
			vSideEdge[eID] = vEdge[eID]; 
			vSideEdge[eID].length = c;
		}
		adjSideDist.push_back(vPair);
		adjSideRoad.push_back(vPairRoad);
	}
}
	
pair<int, int> Graph::cKSPCSP(int ID1, int ID2, int C)
{
}
	
pair<int, int> Graph::Pulse(int ID1, int ID2, int C)
{
}
	
pair<int, int> Graph::skylineCSP(int ID1, int ID2, int C)
{
}

int Graph::DijkstraSideCostPruneCSP(int ID1, int ID2, vector<vector<pair<int, int> > >& adjSideDist, vector<vector<pair<int, int> > >& adjSideRoad, vector<int>& vSPTParent, vector<int>& vSPTParentEdge, vector<int>& vSPTDistance,  vector<int>& vSPTCost, vector<label>& vLabel, vector<int>& vIgnoredNode, vector<int>& vPath, vector<int>& vPathEdge, bool& bE, int& subLength, int& subCost, vector<int>& vSPTCostLB, vector<map<int, int> >& vmSkyline, int& dTmp, int& cTmp)      
{
	benchmark::heap<2, int, int> queue(nodeNum);
	queue.update(ID1, 0);

	vector<int> vDistance(nodeNum, INF);
	vector<int> vRealDist(nodeNum, INF);
	vector<int> vRealCost(nodeNum, INF);
	vector<int> vPrevious(nodeNum, -1);
	vector<int> vPreviousEdge(nodeNum, -1);
	vector<bool> vbVisited(nodeNum, false);
	int topNodeID, neighborNodeID, neighborLength, neighborRoadID;
	vector<pair<int, int> >::iterator ivp;
	
	for(auto& v : vIgnoredNode) 
	{
		vbVisited[v] = true;
	}

	vDistance[ID1] = 0;
	vRealDist[ID1] = subLength;
	vRealCost[ID1] = subCost;

	while(!queue.empty())
	{
		int topDistance; 
		queue.extract_min(topNodeID, topDistance); 
		vbVisited[topNodeID] = true;

		//skyline Pruning  
		int topRealDist = vRealDist[topNodeID];  
		int topRealCost = vRealCost[topNodeID];

		if(topNodeID == ID2)
			break;
/*			
		map<int, int> m = vmSkyline[topNodeID]; 
		if(m.empty())
			vmSkyline[topNodeID][topRealDist] = topRealCost;
		
		bool bDominated = false;
		for(auto& it: m)
		{
			if(it.first < topRealDist && it.second < topRealCost) 
			{
				bDominated = true; 
				break;
			}
			else if(it.first > topRealDist && it.second > topRealCost)  
			{
				vmSkyline[topNodeID].erase(it.first);
			}
		}
			
		if(bDominated)
			continue;
		
		vmSkyline[topNodeID][topRealDist] = topRealCost;
		cout << "Not Dominateed" << endl;
*/
		//Early Termination
		bool b = true;
		if(topNodeID != ID1)
		{
			for(auto& iNode: vIgnoredNode)
			{
				if(!(vLabel[topNodeID].pre < vLabel[iNode].pre || vLabel[topNodeID].post > vLabel[iNode].post)) 
				{
					b = false;
					break;
				}
			}
		}
		else
			b = false;

		if(b)
		{
			bE = true;
			vPath.clear();
			vPathEdge.clear();
			vPath.push_back(topNodeID);
			int p = vPrevious[topNodeID];
			int e = vPreviousEdge[topNodeID];
			int d = 0;
			int c = 0;

			while(p != -1)
			{
				vPath.push_back(p);
				vPathEdge.push_back(e);
				d += vEdge[e].length; 
				c += vEdge[e].cost;
				e = vPreviousEdge[p];
				p = vPrevious[p];
			}

			reverse(vPath.begin(), vPath.end());
			reverse(vPathEdge.begin(), vPathEdge.end());
				
			p = topNodeID;
			while(vSPTParent[p] != -1)
			{
				vPathEdge.push_back(vSPTParentEdge[p]);
				p = vSPTParent[p];
				vPath.push_back(p);
			}
//			cout << "Early termination dist:" <<  topDistance  << "\t" << vSPTDistance[topNodeID] << endl; 
//			cout << "dist:" << d << "\t" << vSPTDistance[topNodeID] << "\tcost:" << c << "\t" << vSPTCost[topNodeID] << endl;
			dTmp = d + vSPTDistance[topNodeID];
//			cout <<  "A:" <<c << "\t" << vSPTCost[topNodeID] << "\t" << vSPTDistance[topNodeID] << endl;
			cTmp = c + vSPTCost[topNodeID];
			return  d + vSPTDistance[topNodeID];
		}

		for(int i = 0; i < (int)adjSideDist[topNodeID].size(); i++) 
		{
			neighborNodeID = adjSideDist[topNodeID][i].first;
			neighborLength = adjSideDist[topNodeID][i].second; 
			neighborRoadID = adjSideRoad[topNodeID][i].second;
			int d = vDistance[topNodeID] + neighborLength; 
			if(!vbVisited[neighborNodeID])
			{
				if(vDistance[neighborNodeID] > d)
				{
					vDistance[neighborNodeID] = d; 
					vRealDist[neighborNodeID] = vRealDist[topNodeID] + vEdge[neighborRoadID].length;
					vRealCost[neighborNodeID] = vRealCost[topNodeID] + vEdge[neighborRoadID].cost;
					queue.update(neighborNodeID, d);
					vPrevious[neighborNodeID] = topNodeID;
					vPreviousEdge[neighborNodeID] = neighborRoadID;
				}
			}
		}
	}

	vPath.clear();
	vPathEdge.clear();
	vPath.push_back(ID2);
	int p = vPrevious[ID2];
	int e = vPreviousEdge[ID2]; 
	int c = 0; 
//	cout << "Traverse back" << endl; 
//	cout << ID1 << "\t" << ID2 << endl;
//	cout << p << "\t" << e << endl;
	while(p != -1)
	{
//		cout << p << "\t" << e << endl;
		vPath.push_back(p);
		vPathEdge.push_back(e);
		c += vEdge[e].cost;
		e = vPreviousEdge[p];
		p = vPrevious[p];
	}

	reverse(vPath.begin(), vPath.end());
	reverse(vPathEdge.begin(), vPathEdge.end());

//	cout << "dist:" << vDistance[ID2] << "\tcost:" << c  << endl;
	bE = false; 
	dTmp = vDistance[ID2];
	cTmp = c; 
	return vDistance[ID2];
}

//Derived from Pascoal
int Graph::eKSP(int ID1, int ID2, int C)
{
	vector<int> vSPTDistance(nodeNum, INF); 
	vector<int> vSPTCost(nodeNum, INF); 
	vector<int> vCurrentCost(nodeNum, INF);//Store the current best, used for pruning (from i to t)
	vector<int> vSPTParent(nodeNum, -1);
	vector<int> vSPTParentEdge(nodeNum, -1);
	vector<int> vTmp;
	vector<vector<int> > vSPT(nodeNum, vTmp);   

	vector<int> vrSPTDistance(nodeNum, INF); 
	vector<int> vrSPTCost(nodeNum, INF);
	rCPT(ID2, ID1, C, vrSPTCost, vrSPTDistance); 
	cout << "min cost:" << vrSPTCost[ID1] << "\tdistance:" << vrSPTDistance[ID1] << endl; 
	if(vrSPTCost[ID1] > C) 
	{
		cout << "Min cost is larger than " << C << endl;
		return -1;
	}
	
	SCPT(ID1, vSPTDistance, vSPTCost, vSPTParent, vSPTParentEdge, vSPT, C);  
	if(vSPTCost[ID2] <= C)
		return vSPTDistance[ID2];

	vector<int> vFCost;
	forwardCost(ID1, C, vFCost);

	vector<int> vEdgeReducedLength(vEdge.size(), INF);
	for(int i = 0; i < (int)vEdge.size(); i++)
	{
		if(vrSPTCost[vEdge[i].ID1] > C || vrSPTCost[vEdge[i].ID2] > C)
			vEdgeReducedLength[i] = INF;
		else
			vEdgeReducedLength[i] = vEdge[i].length + vSPTDistance[vEdge[i].ID1] - vSPTDistance[vEdge[i].ID2];  
	}
	
/*	label lTmp;
	lTmp.pre = -1;
	lTmp.post = -1;
	lTmp.parent = -1;
	vector<label> vLabel(nodeNum, lTmp); 
	intervalLabel(ID1, vSPT, vSPTParent, vLabel);
*/	
//	cD(ID1, ID2);
	vector<vector<int> > vvResult;	//Exact Path
	vector<int> vDistance;
	vector<int> vCost; 
	vector<int> vPathParent;	//Deviated from
	vector<int> vPathParentPos;	//Deviated Pos from Parent
	
	vector<vector<int> > vvPathCandidate;	 //nodes
	vector<vector<int> > vvPathCandidateEdge;//edges 
	vector<vector<int> > vvPathCandidateCost;//Cost from i to t
	vector<unordered_map<int, int> > vmPathNodePos;	//Position of the non-fixed vertices
	//The size of mvPathNodePos indicates the fixed pos
	vector<vector<pair<int, int> > > vvPathNonTree; 
	vector<benchmark::heap<2,int,int> > vqArc;  
	vector<multimap<int, int> > vmArc;
	vector<int> vFather; 
	vFather.push_back(-1);
	
	map<int, int> m;
	vector<map<int, int> > vmSkyline(nodeNum, m); 
	
	vector<int> vPath; 
	vector<int> vPathEdge;
	vPath.push_back(ID2); 
	vector<pair<int, int> > vPathNonTree; 
	int oldP = ID2;
	int p = vSPTParent[ID2]; 
	int e = vSPTParentEdge[ID2];
//	benchmark::heap<2, int, int> qArc(vEdge.size());   
	multimap<int, int> mArc; 
	while(p != -1)
	{
		vPath.push_back(p); 
		for(int i = 0; i < (int)adjListEdgeR[oldP].size(); i++) 
		{
			int eID = adjListEdgeR[oldP][i].second;  
			if(eID != e && vEdgeReducedLength[eID] < INF)  
				mArc.insert(make_pair(vEdgeReducedLength[eID], eID));
			//	qArc.update(eID, vEdgeReducedLength[eID]);
		}
		oldP = p;
		vPathEdge.push_back(e);
		e = vSPTParentEdge[p];
		p = vSPTParent[p];
	}
//	vqArc.push_back(qArc);
	vmArc.push_back(mArc);

	reverse(vPath.begin(), vPath.end()); 
	reverse(vPathEdge.begin(), vPathEdge.end()); 
	unordered_map<int, int> mPos;
	for(int i = 0; i < (int)vPath.size(); i++)
		mPos[vPath[i]] = i;
	vmPathNodePos.push_back(mPos);

	//cout << vPath.size() << "\t" << mPos.size() << endl;

	benchmark::heap<2, int, int> qPath(nodeNum);  
	vvPathCandidate.push_back(vPath); 
	vvPathCandidateEdge.push_back(vPathEdge);
	vDistance.push_back(vSPTDistance[ID2]);  
	vCost.push_back(vSPTCost[ID2]);
	vPathParent.push_back(-1);
	vPathParentPos.push_back(0);
	qPath.update(vvPathCandidate.size()-1, vSPTDistance[ID2]); 
	
	int costTmp = 0;
//	vCurrentCost[ID2] = 0;
	vector<int> vPathCost;
	vPathCost.push_back(costTmp); 
	int dTmp = 0;
	for(auto it = vPathEdge.rbegin(); it != vPathEdge.rend(); it++)
	{	
		int eNodeID1 = vEdge[*it].ID1;
		costTmp += vEdge[*it].cost;
		dTmp += vEdge[*it].length;
		vPathCost.push_back(costTmp);  
//		vCurrentCost[eNodeID1] = costTmp;
		vmSkyline[eNodeID1].insert(make_pair(dTmp, costTmp));
	}
	reverse(vPathCost.begin(), vPathCost.end());
	vvPathCandidateCost.push_back(vPathCost);

	vector<int> vResultID;
	int topPathID, topPathDistance; 
	int pcount = 0;
	int old = -1;
	while(true)
	{
		pcount++;
		qPath.extract_min(topPathID, topPathDistance); 
		if(topPathDistance < old)
			cout<< "Error" <<endl;
		old = topPathDistance;
//		cout << pcount << "\t" << topPathDistance << "\t" << vCost[topPathID] << "\t" << qPath.size() << "\t" << vvPathCandidateCost.size() << "\t" << mArc[topPathID].size() << "\t" << vvPathCandidate[topPathID].size() << endl;     
		
//		cout << pcount << "\t" << topPathDistance << "\t" << vvPathCandidateCost[topPathID][0] << endl;

		if(vCost[topPathID] <= C) 
		{
			cout << "Cost:" << vCost[topPathID] << endl;
			return topPathDistance;
		}

		vector<int> vTwo;
		vTwo.push_back(topPathID);
//		if(vFather[topPathID] != -1 &&  !vqArc[vFather[topPathID]].empty())  
		if(vFather[topPathID] != -1 &&  !vmArc[vFather[topPathID]].empty())  
			vTwo.push_back(vFather[topPathID]);

		int count = 0;
		int index= 0;
		for(auto& pID : vTwo)
		{
			count++;
			bool bLoop = true;  
			if(index == 0)
			{
		/*		for(int i = 0; i < vmPathNodePos[pID].size(); i++)
				{
					int nodeID = vvPathCandidate[pID][i];
					if(vvPathCandidateCost[pID][i] < vCurrentCost[nodeID])
						vCurrentCost[nodeID] = vvPathCandidateCost[pID][i];
				}
		*/		index++; 
//				vvPathCandidate[pID].clear();
			}

			while(bLoop) 
			{
				int mineID;
				int eReducedLen; 
//				if(vqArc[pID].empty())  
				if(vmArc[pID].empty())   
				{
					vvPathCandidate[pID].clear();
	//				vvPathCandidateEdge[pID].clear();
					vvPathCandidateCost[pID].clear();  
					vmPathNodePos[pID].clear();
					break;
				}
//				vqArc[pID].extract_min(mineID, eReducedLen);  
				auto it = vmArc[pID].begin();
				mineID = (*it).second;
				eReducedLen = (*it).first;
				vmArc[pID].erase(it);
				int eNodeID1 = vEdge[mineID].ID1;
				int eNodeID2 = vEdge[mineID].ID2;

//				if(mineID == 69235)
//				cout << "Top MineID:" << mineID << "\tdistance:" << (*it).first << endl;
				
				bool bpLoop = false; 
				unordered_set<int> sE;
				for(int i = vmPathNodePos[pID][eNodeID2]; i < (int)vvPathCandidate[pID].size(); i++)  
				{
					if(sE.find(vvPathCandidate[pID][i]) == sE.end())
						sE.insert(vvPathCandidate[pID][i]);	 
					else
					{
						bpLoop = true;
						continue;
					}
				}
					
				//Cannot prune like this 
				//Current larger cost does not necessarily lead to a future larger cost
				//The cost could be smaller
				int costTmp = vvPathCandidateCost[pID][vmPathNodePos[pID][eNodeID2]] + vEdge[mineID].cost;  
		//		if(mineID == 69235)
		//			cout << "costTmp:" << costTmp << endl;
				int distTmp = vDistance[pID] - vSPTDistance[eNodeID2] + vEdge[mineID].length;     
				
	/*			int d2 = vDistance[pID] - vSPTDistance[eNodeID2];
				for(int i = vmPathNodePos[pID][eNodeID2]+1; i < (int)vvPathCandidate[pID].size(); i++)   
				{
					d2 -= vEdge[vvPathCandidateEdge[pID][i-1]].length;
				}
				if(d2 !=0)
					cout << "Error d2 " << d2 << endl; */
				//if vFCost[eNodeID1] + costTmp > C, prune
				if(costTmp >= vCurrentCost[eNodeID1] || costTmp > C - vFCost[eNodeID1] || vFCost[eNodeID1] > C)    
				{
		/*			if(eNodeID1 == 27174) 
					{
						if(costTmp >= vCurrentCost[eNodeID1])
							cout << "A " << costTmp << "\t" << vCurrentCost[eNodeID1] << endl;
						else if(costTmp > C - vFCost[eNodeID1])
							cout << "B " << costTmp << "\t" << C-vFCost[eNodeID1] << endl;
						else if(vFCost[eNodeID1] > C)     
							cout << "C " << vFCost[eNodeID1] << C << endl;
						cout << "Prune:" << costTmp << "\t" << vCurrentCost[eNodeID1] << "\t" << vFCost[eNodeID1] << endl;     
					}*/
//					cout << "mineID:" << mineID << endl;
					bLoop = true; 
					continue;  
				} 

//				cout << "dist tmp:" << distTmp << endl;
				bool bDominated = false;
				map<int, int> m = vmSkyline[eNodeID1]; 
				if(m.empty()) 
				{
			//		if(eNodeID1 == 27174)
			//			cout << "Insert0:" << eNodeID1 << "\t" << distTmp << endl;
					vmSkyline[eNodeID1][distTmp] = costTmp;
				}
				else
				{
					for(auto& it: m)
					{
			//			if(eNodeID1 == 27174)
			//				cout <<  "c:" << it.second << "\t" << costTmp << endl << "d:" << it.first << "\t" << distTmp << endl; 
						if(it.first < distTmp && it.second < costTmp) 
						{ 
//							cout << "Skyline Prune" << endl;
							bDominated = true; 
							break;
						}
						else if(it.first > distTmp && it.second > costTmp)  
							vmSkyline[eNodeID1].erase(it.first);
					}

					if(!bDominated)
					{
			//			if(eNodeID1 == 27174)
			//				cout << "Insert1:" << eNodeID1 << "\t" << distTmp << endl;
						vmSkyline[eNodeID1][distTmp] = costTmp; 
					}
					else
					{
						bpLoop = true;
						continue;
					}
				}
				
				if(!bpLoop)
				{
					bLoop = false;
					//Traverse the non-fixed part: the tree
					benchmark::heap<2, int, int> qArcTmp(vEdge.size());    
//					benchmark::heap<2, int, int> qArcTmp(1000);     
					multimap<int, int> mArcTmp;
					int p = eNodeID1;
					int e = vSPTParentEdge[p];

					vPath.clear();
					vPathEdge.clear();
					vector<int> vPathCost; 
			//		int cCheck = 0; 

					while(p != -1)
					{
						vPath.push_back(p);
						vPathCost.push_back(costTmp); 

//						cout << "p:" << p << endl;
//						if(vrSPTCost[p] < C && vFCost[p]+vrSPTCost[p] <= C && costTmp <= vCurrentCost[p])
						if(vrSPTCost[p] <= C && vFCost[p]+vrSPTCost[p] <= C)
						{
							for(int i = 0; i < (int)adjListEdgeR[p].size(); i++) 
							{
								int reID = adjListEdgeR[p][i].second;
								int eID1 = vEdge[reID].ID1;
								bool b2 = false;
								map<int, int> m2 = vmSkyline[eID1]; 
								int dTmp = distTmp + vEdge[reID].length;
								int cTmp = costTmp + vEdge[reID].cost;

			/*					if(reID == 69235)
								{ 
									cout << "reID:" << reID << endl; 
									cout << m2.size() << endl;
								}
			*/
								for(auto& it: m2)
								{
						//			if(reID == 69235)
						//				cout << it.second << "\t" << cTmp << endl;
									if(it.first < dTmp && it.second < cTmp) 
									{
										b2 = true;
										break;
									}
								}

								if(b2)
									continue;
		//						if(reID == 69235)
		//						cout << "remain reID:" << reID << endl;
								if(reID != e && reID != mineID && vrSPTCost[eID1] <= C)	  
								{
									mArcTmp.insert(make_pair(vEdgeReducedLength[reID], reID)); 
						//			if(reID == 27174)
						//				cout << "Insert2:" << reID << "\t" << vEdgeReducedLength[reID] << endl;
									//qArcTmp.update(reID, vEdgeReducedLength[reID]);
								}
							}
						}

						vPathEdge.push_back(e); 
						costTmp += vEdge[e].cost;
						distTmp += vEdge[e].length;
							
//						cCheck += vEdge[e].cost;
						p = vSPTParent[p];
						if(vSPTParent[p] == -1) 
						{
							vPath.push_back(p);
							vPathCost.push_back(costTmp);
							break;
						}
						e = vSPTParentEdge[p];
					
						bDominated = false;
						map<int, int> m = vmSkyline[p]; 
						if(m.empty()) 
						{
				//			if(p == 27174)
				//				cout << "Insert3:" << eNodeID1 << "\t" << distTmp << endl;
							vmSkyline[p][distTmp] = costTmp;
						}
						else
						{
							for(auto& it: m)
							{
								if(it.first < distTmp && it.second < costTmp) 
								{
									bDominated = true; 
									break;
								}
								else if(it.first > distTmp && it.second > costTmp)  
									vmSkyline[p].erase(it.first);
							}
						}

						if(!bDominated) 
						{
				//			if(p == 27174)
				//				cout << "Insert4:" << eNodeID1 << "\t" << distTmp << endl;
							vmSkyline[p][distTmp] = costTmp;  
						}
	/*					else
						{
							bpLoop = true;
							break;
						}
	*/				}

	/*				if(bpLoop)
					{
						bLoop = true;
						continue;
					}
	*/	
					int dist = vDistance[pID] - vSPTDistance[eNodeID2] + vSPTDistance[eNodeID1] + vEdge[mineID].length;    
			//		Cannot use topPathDistance as the second one is the parent
			//		cout << dist << "\t" << topPathDistance + eReducedLen << endl;
			//		int dist = topPathDistance + eReducedLen;
			//		if(dist != d2)
			//			cout << "Error 4:" << dist << "\t" << d2 << endl;
					int cost = vCost[pID] - vSPTCost[eNodeID2] + vSPTCost[eNodeID1] + vEdge[mineID].cost;
					vDistance.push_back(dist); 
					vCost.push_back(cost);
					vFather.push_back(pID); 
					qPath.update(vDistance.size()-1, dist); 

					reverse(vPath.begin(), vPath.end());
					reverse(vPathEdge.begin(), vPathEdge.end()); 
					reverse(vPathCost.begin(), vPathCost.end());
					
					unordered_map<int, int> mE; 
					int i;
					//Pos stop at eNodeID1 as it is boundary of fixed
					for(i = 0; i < (int)vPath.size(); i++)
						mE[vPath[i]] = i; 
					vmPathNodePos.push_back(mE); 
					
					vPath.push_back(eNodeID2);
					vPathEdge.push_back(mineID);
					costTmp = vvPathCandidateCost[pID][vmPathNodePos[pID][eNodeID2]];    
					vPathCost.push_back(costTmp);
					
//					if(cost != vPathCost[0])
//						cout << "Error First Cost!" << cost <<"\t" << vPathCost[0] << endl;
					
					costTmp = vvPathCandidateCost[pID][vmPathNodePos[pID][eNodeID2]];
					for(int j = vmPathNodePos[pID][eNodeID2]; j+1 < (int)vvPathCandidate[pID].size(); j++)
					{
						int nodeID = vvPathCandidate[pID][j+1]; 
						vPath.push_back(nodeID);
						int edgeID = vvPathCandidateEdge[pID][j];
						vPathEdge.push_back(edgeID); 
						vPathCost.push_back(vvPathCandidateCost[pID][j+1]);
						costTmp -= vEdge[edgeID].cost;
						vPathCost.push_back(costTmp);
					}
					
					if(vvPathCandidateCost[pID][vmPathNodePos[pID][eNodeID2]]+vEdge[mineID].cost < vCurrentCost[eNodeID1])
						vCurrentCost[eNodeID1] = vvPathCandidateCost[pID][vmPathNodePos[pID][eNodeID2]]+vEdge[mineID].cost;

					//vqArc.push_back(qArcTmp);
					vmArc.push_back(mArcTmp); 
					vvPathCandidate.push_back(vPath);
					vvPathCandidateEdge.push_back(vPathEdge);
					vvPathCandidateCost.push_back(vPathCost);
				}
			}
		}
	} 

	return -1;
}


void Graph::SCPT(int root, vector<int>& vSPTDistance, vector<int>& vSPTCost, vector<int>& vSPTParent, vector<int>& vSPTParentEdge, vector<vector<int> >& vSPT, int C)     
{
	benchmark::heap<2, int, int> queue(nodeNum); 
	queue.update(root, 0);

	vector<bool> vbVisited(nodeNum, false);
	int topNodeID, neighborNodeID, neighborLength, neighborRoadID, neighborCost; 

	vSPTDistance[root] = 0; 
	vSPTCost[root] = 0;

	compareNode cnTop;
	while(!queue.empty())
	{
		int topDistance; 
		queue.extract_min(topNodeID, topDistance); 
		vbVisited[topNodeID] = true; 
		for(int i = 0; i < (int)adjList[topNodeID].size(); i++)
		{
			neighborNodeID = adjList[topNodeID][i].first;
			neighborLength = adjList[topNodeID][i].second;
			neighborRoadID = adjListEdge[topNodeID][i].second; 
			neighborCost = adjListCost[topNodeID][i].second;
			int d = vSPTDistance[topNodeID] + neighborLength;
			int c = vSPTCost[topNodeID] + neighborCost; 
		//	if(c > C)
		//		continue;
			if(!vbVisited[neighborNodeID] && vSPTDistance[neighborNodeID] > d)
			{
				vSPTDistance[neighborNodeID] = d;
				vSPTCost[neighborNodeID] = c; 
				queue.update(neighborNodeID, d); 
				vSPTParent[neighborNodeID] = topNodeID;
				vSPTParentEdge[neighborNodeID] = neighborRoadID; 
			}
		}
	}
	
	//Construct SPT
	for(int i = 0; i < nodeNum; i++)
		if(vSPTParent[i] != -1)
			vSPT[vSPTParent[i]].push_back(i);
}

void Graph::rCPT(int root, int ID1, int C, vector<int>& vrSPTCost, vector<int>& vrSPTDistance)      
{
	benchmark::heap<2, int, int> queue(nodeNum); 
	queue.update(root, 0);

	vector<bool> vbVisited(nodeNum, false);
	int topNodeID, neighborNodeID, neighborRoadID, neighborLength, neighborCost, topCost;  

	vrSPTCost[root] = 0;
	vrSPTDistance[root] = 0;

	compareNode cnTop;
	vector<int> vParent(nodeNum, -1);
	vector<int> vParentEdge(vEdge.size(), -1);
	while(!queue.empty())
	{
		int topDistance; 
		queue.extract_min(topNodeID, topCost); 
		vbVisited[topNodeID] = true; 
		if(topCost > C)
			break;

		for(int i = 0; i < (int)adjListR[topNodeID].size(); i++)
		{
			neighborNodeID = adjListR[topNodeID][i].first;
			neighborLength = adjListR[topNodeID][i].second; 
			neighborRoadID = adjListEdgeR[topNodeID][i].second; 
			neighborCost = adjListCostR[topNodeID][i].second;
			int c = vrSPTCost[topNodeID] + neighborCost; 
			int d = vrSPTDistance[topNodeID] + neighborLength;
			if(!vbVisited[neighborNodeID] && vrSPTCost[neighborNodeID] > c)
			{
				vrSPTCost[neighborNodeID] = c;
/*				if(vEdge[neighborRoadID].cost != neighborCost)
				{
					cout << "Edge Error!" << neighborCost << "\t" << vEdge[neighborRoadID].cost << endl; 
					cout << topNodeID << endl;
					for(int j = 0; j < (int)adjListR[topNodeID].size(); j++)
					{
						cout << adjListR[topNodeID][j].first << "\t" << adjListR[topNodeID][j].second << endl;
						cout << adjListEdgeR[topNodeID][j].first << "\t" << adjListEdgeR[topNodeID][j].second << endl;
						cout << adjListCostR[topNodeID][j].first << "\t" << adjListCostR[topNodeID][j].second << endl;

					}
				}
*/
				vrSPTDistance[neighborNodeID] = d;
				queue.update(neighborNodeID, c);  
				vParent[neighborNodeID] = topNodeID;
				vParentEdge[neighborNodeID] = neighborRoadID;
			}
		}
	}

/*	vector<int> vPath, vPathEdge; 
	int p = ID1;
	int e = vParentEdge[p];
	while(p != root)
	{
		vPath.push_back(p);
		vPathEdge.push_back(e);
		p = vParent[p];
		e = vParentEdge[p]; 
	}
	vPath.push_back(p); 

	for(auto& v: vPath)
		cout << v <<"\t";
	cout << endl;
	
	int c = 0;
	int d = 0;
	for(auto& ip : vPathEdge)
	{
		c += vEdge[ip].cost; 
		d += vEdge[ip].length;
	}
	cout << "Cost:" << c << "\tDist:" << d << endl; 
	
	d = 0;
	c = 0;
	int oldv = -1;
	for(auto&v : vPath) 
	{
		cout << v << "\t";
		bool b = false;
		if(oldv > -1)
		{
			for(int j= 0 ; j < (int)adjList[oldv].size(); j++)
			{
				if(adjList[oldv][j].first == v)
				{
					d += adjList[oldv][j].second;
					c += adjListCost[oldv][j].second;
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
		cout << endl << d << "\t" << c << "\t" << endl;
	}
	*/
}

void Graph::cD(int ID1, int ID2)       
{
	benchmark::heap<2, int, int> queue(nodeNum); 
	queue.update(ID1, 0);

	vector<bool> vbVisited(nodeNum, false);
	int topNodeID, neighborNodeID, neighborRoadID, neighborLength, neighborCost, topCost;  
	vector<pair<int, int> >::iterator ivp; 
	vector<int> vrSPTCost(nodeNum,INF); 
	vector<int> vParent(nodeNum, -1);

	vrSPTCost[ID1] = 0;

	compareNode cnTop;
	while(!queue.empty())
	{
		int topDistance; 
		queue.extract_min(topNodeID, topCost);  
		if(topNodeID == ID2) 
		{
			cout << "MinCost:" << topCost << endl;  
			break;
		}
		vbVisited[topNodeID] = true; 
		for(int i = 0; i < (int)adjListR[topNodeID].size(); i++)
		{
			neighborNodeID = adjList[topNodeID][i].first;
			neighborRoadID = adjListEdge[topNodeID][i].second; 
			neighborLength = adjList[topNodeID][i].second; 
			neighborCost = adjListCost[topNodeID][i].second;
			int c = vrSPTCost[topNodeID] + neighborCost; 
			if(!vbVisited[neighborNodeID])
			{
				if(vrSPTCost[neighborNodeID] > c)
				{
					vrSPTCost[neighborNodeID] = c;
					queue.update(neighborNodeID, c);  
					vParent[neighborNodeID] = topNodeID;
			;	}
			}
		}
	}
/*	int p = ID2;
	while(p != -1)
	{
		cout << p << endl;
		p = vParent[p];
	}*/
}

void Graph::forwardCost(int ID1, int C, vector<int>& vFCost)
{
	benchmark::heap<2, int, int> queue(nodeNum); 
	queue.update(ID1, 0);

	vector<bool> vbVisited(nodeNum, false);
	int topNodeID, neighborNodeID, neighborRoadID, neighborLength, neighborCost, topCost;  
	vector<pair<int, int> >::iterator ivp; 
	vFCost.assign(nodeNum,INF); 

	vFCost[ID1] = 0;

	compareNode cnTop;
	while(!queue.empty())
	{
		queue.extract_min(topNodeID, topCost);  
		if(topCost > C) 
			break;
		
		vbVisited[topNodeID] = true; 
		for(int i = 0; i < (int)adjList[topNodeID].size(); i++)
		{
			neighborNodeID = adjList[topNodeID][i].first;
			neighborCost = adjListCost[topNodeID][i].second;
			int c = vFCost[topNodeID] + neighborCost; 
			if(!vbVisited[neighborNodeID])
			{
				if(vFCost[neighborNodeID] > c)
				{
					vFCost[neighborNodeID] = c;
					queue.update(neighborNodeID, c);  
				}
			}
		}
	}
}

