#include "graph.h"

int Graph::Yen(int ID1, int ID2, int k, vector<int>& kResults)
{
	vector<int> vPath, vPathEdge;
	vector<vector<int> > vvResult;
	vvResult.reserve(k);
	
	vector<int> vDistance;
	vector<vector<int> > vvPathCandidate;	//nodes
	vector<vector<int> > vvPathCandidateEdge;//edges
	vector<int> vPathParent;				//Deviated from
	vector<int> vPathParentPos;				//Deviated Pos from Parent
	vector<vector<int> > vvPathRemovedEdge; //Removed Edges when computed
	vector<unordered_set<int> > vvPathRemovedNode; //Removed Edges when computed

	benchmark::heap<2, int, int> qPath(adjList.size()*100);
	
	int d = DijkstraPath(ID1, ID2, vPath, vPathEdge);
	vDistance.push_back(d);
	vvPathCandidate.push_back(vPath);
	vvPathCandidateEdge.push_back(vPathEdge);
	vPathParent.push_back(-1);
	vPathParentPos.push_back(0);
	vector<int> vTmp; 
	unordered_set<int>  sB;
	vvPathRemovedEdge.push_back(vTmp);
	vvPathRemovedNode.push_back(sB);

	//vvPathCandidate.size()-1 is the current Path ID
	qPath.update(vvPathCandidate.size()-1, d);
	
	int topPathID, topPathDistance;

	while((int)vvResult.size() < k)
	{
		qPath.extract_min(topPathID, topPathDistance); 
		kResults.push_back(topPathDistance);  
		vvResult.push_back(vvPathCandidate[topPathID]);
			
		vector<int> vRemovedEdgeTmp;
		unordered_set<int> sRemovedNodeTmp; 
		for(auto ie = vvPathRemovedEdge[topPathID].begin(); ie != vvPathRemovedEdge[topPathID].end(); ie++)
		{
			int eID1 = vEdge[*ie].ID1;
			int eID2 = vEdge[*ie].ID2; 
			sRemovedNodeTmp.insert(eID1);
			for(int i = 0; i < (int)adjList[eID1].size(); i++)
			{
				if(adjList[eID1][i].first == eID2)
				{
					adjList[eID1].erase(adjList[eID1].begin() + i);
					adjListEdge[eID1].erase(adjListEdge[eID1].begin()+i);
				}
			}
			vRemovedEdgeTmp.push_back(*ie);
		}

		//sub-path
		vector<int> vPreviousNode;
		vector<int> vPreviousEdge;
		int subLength = 0;
		int i;
		for(i = 0; i < vPathParentPos[topPathID]; i++)
		{
			vPreviousNode.push_back(vvPathCandidate[topPathID][i]);
			vPreviousEdge.push_back(vvPathCandidateEdge[topPathID][i]);
			subLength += vEdge[vvPathCandidateEdge[topPathID][i]].length;
		}

		//Remove the edges incrementally for each new path
		for(auto ie = vvPathCandidateEdge[topPathID].begin() + i; ie != vvPathCandidateEdge[topPathID].end(); ie++)
		{
			int eID1 = vEdge[*ie].ID1;
			int eID2 = vEdge[*ie].ID2;
			for(int j = 0; j < (int)adjList[eID1].size(); j++)
			{
				if(adjList[eID1][j].first == eID2)
				{
					adjList[eID1].erase(adjList[eID1].begin() + j);
					adjListEdge[eID1].erase(adjListEdge[eID1].begin()+j);
				}
			}
			vRemovedEdgeTmp.push_back(*ie); 
			sRemovedNodeTmp.insert(eID1);  

			int dTmp = DijkstraPath2(eID1, ID2, sRemovedNodeTmp, vPath, vPathEdge) + subLength;
			subLength += vEdge[*ie].length;
			vector<int> vPreviousNodeTmp = vPreviousNode;
			vector<int> vPreviousEdgeTmp = vPreviousEdge;
			vPreviousNodeTmp.insert(vPreviousNodeTmp.end(), vPath.begin() + 1, vPath.end());
			vPreviousEdgeTmp.insert(vPreviousEdgeTmp.end(), vPathEdge.begin(), vPathEdge.end()); 
		    
			vPreviousNode.push_back(eID2);
			vPreviousEdge.push_back(*ie);

			vDistance.push_back(dTmp);
			vvPathCandidate.push_back(vPreviousNodeTmp);
			vvPathCandidateEdge.push_back(vPreviousEdgeTmp);
			vPathParent.push_back(topPathID);
			vPathParentPos.push_back(ie - vvPathCandidateEdge[topPathID].begin());
			vvPathRemovedEdge.push_back(vRemovedEdgeTmp);
			vvPathRemovedNode.push_back(sRemovedNodeTmp); 
			qPath.update(vvPathCandidate.size()-1, dTmp);
			
		}

		//Recover the removed edges
		for(auto ie = vRemovedEdgeTmp.begin(); ie != vRemovedEdgeTmp.end(); ie++)
		{
			int eID1 = vEdge[*ie].ID1;
			int eID2 = vEdge[*ie].ID2;
			adjList[eID1].push_back(make_pair(eID2, vEdge[*ie].length));
			adjListEdge[eID1].push_back(make_pair(eID2, *ie));
		}
	}

	return vvResult.size();
}
	
int Graph::Yen2003(int ID1, int ID2, int k, vector<int>& kResults)
{
	return 0;
}

int Graph::Pascoal(int ID1, int ID2, int k, vector<int>& kResults, vector<vector<int> >& vkPath) 
{
	vector<int> vSPTDistance(nodeNum, INF); 
	vector<int> vSPTParent(nodeNum, -1);
	vector<int> vSPTParentEdge(nodeNum, -1);
	vector<int> vTmp;
	vector<vector<int> > vSPT(nodeNum, vTmp);  
	
	SPT(ID1, vSPTDistance, vSPTParent, vSPTParentEdge, vSPT); 
	vector<int> vEdgeReducedLength(vEdge.size(), INF);

	for(int i = 0; i < (int)vEdge.size(); i++)
		vEdgeReducedLength[i] = vEdge[i].length + vSPTDistance[vEdge[i].ID1] - vSPTDistance[vEdge[i].ID2];  
//	for(int i = 0; i < (int)vEdge.size(); i++)
//		cout << vEdgeReducedLength[i] << endl;
	
	vector<vector<int> > vvResult;	//Exact Path
	vvResult.reserve(k);
	vector<int> vDistance;
	vector<int> vPathParent;				//Deviated from
	vector<int> vPathParentPos;				//Deviated Pos from Parent
	
	vector<vector<int> > vvPathCandidate;	//nodes
	vector<vector<int> > vvPathCandidateEdge;//edges 
	vector<unordered_map<int, int> > vmPathNodePos;
	vector<vector<pair<int, int> > > vvPathNonTree; 
	vector<benchmark::heap<2,int,int> > vqArc; 
	vector<int> vFather; 
	vFather.push_back(-1);
	
	vector<int> vPath; 
	vector<int> vPathEdge;
	vPath.push_back(ID2); 
	vector<pair<int, int> > vPathNonTree; 
	int oldP = ID2;
	int p = vSPTParent[ID2]; 
	int e = vSPTParentEdge[ID2];
	benchmark::heap<2, int, int> qArc(vEdge.size());     
	while(p != -1)
	{
		vPath.push_back(p); 
		for(int i = 0; i < (int)adjListEdgeR[oldP].size(); i++) 
		{
			int eID = adjListEdgeR[oldP][i].second;  
			if(eID != e) 
				qArc.update(eID, vEdgeReducedLength[eID]);
		}
		oldP = p;
		vPathEdge.push_back(e);
		e = vSPTParentEdge[p];
		p = vSPTParent[p];
	}
	vqArc.push_back(qArc);

	reverse(vPath.begin(), vPath.end()); 
	reverse(vPathEdge.begin(), vPathEdge.end()); 
	unordered_map<int, int> mPos;
	for(int i = 0; i < (int)vPath.size(); i++)
		mPos[vPath[i]] = i;
	vmPathNodePos.push_back(mPos);

	//cout << vPath.size() << "\t" << mPos.size() << endl;

	benchmark::heap<2, int, int> qPath(nodeNum);  
	vvPathCandidate.push_back(vPath); 
	vDistance.push_back(vSPTDistance[ID2]); 
	vvPathCandidateEdge.push_back(vPathEdge);
	vPathParent.push_back(-1);
	vPathParentPos.push_back(0);
	qPath.update(vvPathCandidate.size()-1, vSPTDistance[ID2]); 
	
	vector<int> vResultID;
	int topPathID, topPathDistance;
	while((int)kResults.size() < k)
	{
//		cout << endl << "k:" << kResults.size() << endl;
		qPath.extract_min(topPathID, topPathDistance); 

		unordered_set<int> us; 
		bool bNew = true;
		for(auto& v : vvPathCandidate[topPathID]) 
		{
			if(us.find(v) == us.end())
				us.insert(v);	  
			else
			{
				bNew = false;
				break;
			}
		}

		if(bNew)
		{
			kResults.push_back(topPathDistance);  
			vkPath.push_back(vvPathCandidate[topPathID]);
			vResultID.push_back(topPathID);
		}
//		else
//			continue;

//		cout << "topPathID:" << topPathID << "\t" << topPathDistance <<endl;
//		vvResult.push_back(vvPathCandidate[topPathID]); 


		vector<int> vTwo;
		vTwo.push_back(topPathID);
		if(vFather[topPathID] != -1)
			vTwo.push_back(vFather[topPathID]);

		for(auto& pID : vTwo)
		{
//			if(pID != topPathID && pID != vFather[topPathID]) 
//				continue;
//			cout << "pID:" << pID << endl; 
			bool bLoop = true;  
			vector<int> vDeleteTmp;
			while(bLoop)
			{
				int mineID;
				int eReducedLen; 
				if(vqArc[pID].empty()) 
					break;
				vqArc[pID].extract_min(mineID, eReducedLen); 
				vDeleteTmp.push_back(mineID);
//				cout << eReducedLen << endl;
				int eNodeID1 = vEdge[mineID].ID1;
				int eNodeID2 = vEdge[mineID].ID2;
				int p = eNodeID1; 
				bool bpLoop = false;
				vector<int> vFixedSubpath; //Start from eNodeID1
				unordered_set<int> sE;
				for(int i = vmPathNodePos[pID][eNodeID2]; i < (int)vvPathCandidate[pID].size(); i++)  
				{
					if(sE.find(vvPathCandidate[pID][i]) == sE.end())
						sE.insert(vvPathCandidate[pID][i]);	 
					else
					{
						bpLoop = true;
						break;
					}
				}
			/*	unordered_set<int> sE;
				for(int i = vmPathNodePos[pID][eNodeID2]; i < (int)vvPathCandidate[pID].size(); i++)
					sE.insert(vvPathCandidate[pID][i]);	
				
				while(p != -1)
				{
					if(sE.find(p) != sE.end())
					{
						bpLoop = true; 
//						cout << "Loop!" << endl;
						break;
					}
					p = vSPTParent[p];
				}
			*/	
				if(!bpLoop)
				{
//					cout << "No loop" << endl;
					bLoop = false;
					benchmark::heap<2, int, int> qArcTmp(vEdge.size());    
					int p = eNodeID1;
					int e = vSPTParentEdge[p];
					
					vPath.clear();
					vPathEdge.clear();
					while(p != -1)
					{
						vPath.push_back(p);
						for(int i = 0; i < (int)adjListEdgeR[p].size(); i++) 
						{
							int reID = adjListEdgeR[p][i].second;
							if(reID != e && reID != mineID)	 
								qArcTmp.update(reID, vEdgeReducedLength[reID]);
						}
						vPathEdge.push_back(e);
						p = vSPTParent[p];
						if(vSPTParent[p] == -1) 
						{
							vPath.push_back(p);
							break;
						}
						e = vSPTParentEdge[p];
					}
				
					int dist = vDistance[pID] - vSPTDistance[eNodeID2] + vSPTDistance[eNodeID1] + vEdge[mineID].length;  
					vDistance.push_back(dist); 
					vFather.push_back(pID); 
//					cout << vDistance.size()-1 << ":" << dist << endl;
					qPath.update(vDistance.size()-1, dist); 

					reverse(vPath.begin(), vPath.end());
					reverse(vPathEdge.begin(), vPathEdge.end()); 
					unordered_map<int, int> mE; 
					int i;
					for(i = 0; i < (int)vPath.size(); i++)
						mE[vPath[i]] = i; 
					vPath.push_back(eNodeID2);
					vPathEdge.push_back(mineID);
				//	mE[eNodeID2] = i;
				//	i++;
					for(int j = vmPathNodePos[pID][eNodeID2]; j+1 < (int)vvPathCandidate[pID].size(); j++)
					{
//						vPath.push_back(vvPathCandidate[pID][j+1]);
						int nodeID = vvPathCandidate[pID][j+1]; 
						vPath.push_back(nodeID);
						int edgeID = vvPathCandidateEdge[pID][j];
						vPathEdge.push_back(edgeID);  
				//		mE[vvPathCandidate[pID][j+1]] = i;
				//		i++;
					}
					vqArc.push_back(qArcTmp);
					vvPathCandidate.push_back(vPath);
					vvPathCandidateEdge.push_back(vPathEdge);
					vmPathNodePos.push_back(mE); 
				}
			}
		}

	}
	
	return 0;
}

void Graph::SPT(int root, vector<int>& vSPTDistance, vector<int>& vSPTParent, vector<int>& vSPTParentEdge, vector<vector<int> >& vSPT)   
{
	benchmark::heap<2, int, int> queue(nodeNum); 
	queue.update(root, 0);

	vector<bool> vbVisited(nodeNum, false);
	int topNodeID, neighborNodeID, neighborLength, neighborRoadID; 
	vector<pair<int, int> >::iterator ivp;

	vSPTDistance[root] = 0;
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
			int d = vSPTDistance[topNodeID] + neighborLength;
			if(!vbVisited[neighborNodeID])
			{
				if(vSPTDistance[neighborNodeID] > d)
				{
					vSPTDistance[neighborNodeID] = d;
					queue.update(neighborNodeID, d); 
					vSPTParent[neighborNodeID] = topNodeID;
					vSPTParentEdge[neighborNodeID] = neighborRoadID; 
				}
			}
		}
	}
	
	//Construct SPT
	for(int i = 0; i < nodeNum; i++)
		if(vSPTParent[i] != -1)
			vSPT[vSPTParent[i]].push_back(i);
}

