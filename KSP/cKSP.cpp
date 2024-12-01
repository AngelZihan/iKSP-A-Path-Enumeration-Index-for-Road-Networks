#include "graph.h"

int Graph::cKSP(int ID1, int ID2, int k, vector<int>& kResults, vector<vector<int> >& vkPath)
{
	//Preprocessing
	//Build reverse SP tree
	vector<int> vSPTDistance(nodeNum, INF);
	vector<int> vSPTParent(nodeNum, -1); 
	vector<int> vSPTParentEdge(nodeNum, -1); 
	vector<int> vTmp;
	vector<vector<int> > vSPT(nodeNum, vTmp);  
	rSPT(ID2, vSPTDistance, vSPTParent, vSPTParentEdge, vSPT);  
	
	//Generating Side Cost
	vector<vector<pair<int, int> > > adjSideCost;  
	vector<vector<pair<int, int> > > adjSideRoad;   
//	Edge e;
	vector<Edge> vSideEdge;
	vSideEdge.resize(vEdge.size());
	adjSideCost.reserve(nodeNum); 
	for(int i = 0; i < nodeNum; i++)
	{
		vector<pair<int, int> > vPair, vPairRoad;  
		if(vSPTDistance[i] == INF)
		{
			adjSideCost.push_back(vPair); 
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
		adjSideCost.push_back(vPair);
		adjSideRoad.push_back(vPairRoad);
	}
/*	for(int i = 0; i < nodeNum; i++) 
	{
		cout << i << "\t";
		for(int j = 0; j < (int)adjSideCost[i].size(); j++)  
			cout << adjSideCost[i][j].first << "," << adjSideCost[i][j].second << "\t";
		cout << endl;
	}*/
	
	//Structure labels, for fast loop detection 
	label lTmp;
	lTmp.pre = -1;
	lTmp.post = -1;
	lTmp.parent = -1;
	vector<label> vLabel(nodeNum, lTmp); 
	intervalLabel(ID2, vSPT, vSPTParent, vLabel);
/*	queue<int> q;
	q.push(ID2);
	while(!q.empty())
	{
		int id = q.front();
		q.pop(); 
		cout << id << "," << vLabel[id].pre << "," << vLabel[id].post << "," << vLabel[id].parent << endl;
		for(auto& v : vSPT[id])
			q.push(v);
	}*/

	
	vector<vector<int> > vvResult;	//Exact Path
	vvResult.reserve(k);
	vector<int> vDistance;
	vector<vector<int> > vvPathCandidate;	//nodes
	vector<vector<int> > vvPathCandidateEdge;//edges
	vector<int> vPathParent;				//Deviated from
	vector<int> vPathParentPos;				//Deviated Pos from Parent
	vector<vector<int> > vvPathRemovedEdge; //Removed Edges when computed
		
	vector<int> vPath, vPathEdge;
	benchmark::heap<2, int, int> qPath(nodeNum*100); 
	int d = DijkstraPath(ID1, ID2, vPath, vPathEdge); 
	
	cout << d << endl;
	vvPathCandidate.push_back(vPath); 
	vDistance.push_back(d);
	vvPathCandidateEdge.push_back(vPathEdge);
	vPathParent.push_back(-1);
	vPathParentPos.push_back(0);
	vvPathRemovedEdge.push_back(vTmp);

	cout << vvPathCandidate.size() << endl;
	qPath.update(vvPathCandidate.size()-1, d); 
	int topPathID, topPathDistance;

	int oldD = -1;
	while((int)vvResult.size() < k)
	{
		qPath.extract_min(topPathID, topPathDistance);
		kResults.push_back(topPathDistance);
		vkPath.push_back(vvPathCandidate[topPathID]);
		vvResult.push_back(vvPathCandidate[topPathID]); 
	
		if(topPathDistance > 120000)
			break;
		cout <<  vvResult.size() << "\t" << topPathDistance << endl; 
		if(topPathDistance < oldD)
			cout << "Error!" << topPathDistance << "\t" << oldD << endl;
		oldD = topPathDistance;

		vector<vector<pair<int, int> > > adjSideCostTmp = adjSideCost;    
		vector<vector<pair<int, int> > > adjSideRoadTmp = adjSideRoad;   
		
		//Store the Ignored Edge
		vector<int> vRemovedEdgeTmp; 
		vector<int> vIgnoredNode;
		vIgnoredNode.reserve(nodeNum);
		if(vvPathRemovedEdge[topPathID].size() > 0)
			vIgnoredNode.push_back(vEdge[*(vvPathRemovedEdge[topPathID].begin())].ID1); 
		for(auto& ie : vvPathRemovedEdge[topPathID])
		{
			int eID1 = vEdge[ie].ID1;
			int eID2 = vEdge[ie].ID2;
			vIgnoredNode.push_back(eID2);
			for(int i = 0; i < (int)adjSideCostTmp[eID1].size(); i++)
			{
				if(adjSideCostTmp[eID1][i].first == eID2)
				{
					adjSideCostTmp[eID1].erase(adjSideCostTmp[eID1].begin() + i);
					adjSideRoadTmp[eID1].erase(adjSideRoadTmp[eID1].begin()+i); 
					break;
				}
			}
			vRemovedEdgeTmp.push_back(ie); 
		}

		//Sub-path, find the start of the current deviation
		vector<int> vPreviousNode;
		vector<int> vPreviousEdge;
		int subLength = 0;
		int i = 0; 
//		vPreviousNode.push_back(ID1);
		for(; i < vPathParentPos[topPathID]; i++)
		{
			vPreviousNode.push_back(vvPathCandidate[topPathID][i]);
			vPreviousEdge.push_back(vvPathCandidateEdge[topPathID][i]);
			subLength += vEdge[vvPathCandidateEdge[topPathID][i]].length;
		}
		
		//Remove the edges incrementally for each new path
		vIgnoredNode.push_back(vEdge[*(vvPathCandidateEdge[topPathID].begin()+i)].ID1);  
		for(auto ie = vvPathCandidateEdge[topPathID].begin() + i; ie != vvPathCandidateEdge[topPathID].end(); ie++)
		{
			int eID1 = vEdge[*ie].ID1;
			int eID2 = vEdge[*ie].ID2;
			vIgnoredNode.push_back(eID2);
			for(int j = 0; j < (int)adjSideCostTmp[eID1].size(); j++)
			{
				if(adjSideCostTmp[eID1][j].first == eID2)
				{
					adjSideCostTmp[eID1].erase(adjSideCostTmp[eID1].begin() + j);
					adjSideRoadTmp[eID1].erase(adjSideRoadTmp[eID1].begin() + j);  
					break;
				}
			}
			vRemovedEdgeTmp.push_back(*ie);
			
			bool bE = false;
			int dTmp = DijkstraSideCostPrune(eID1, ID2, adjSideCostTmp, adjSideRoadTmp, vSPTParent, vSPTParentEdge, vSPTDistance, vLabel, vIgnoredNode, vPath, vPathEdge, bE);     
		//	cout << vPath.size() << "\t" << vPathEdge.size() << endl; 
		//	bE = false;
		//	dTmp = DijkstraSideCost(eID1, ID2, adjSideCostTmp, adjSideRoadTmp, vPath, vPathEdge);    
		//	cout << vPath.size() << "\t" << vPathEdge.size() << endl; 
	
			int d2 = subLength; 
			subLength += vEdge[*ie].length; 
			if(bE)
			{
				d2 += dTmp;  
//				cout<< d2 << endl;
//				cout << vPath.size() << "\t" << vPathEdge.size() << endl;
			}
			else
				for(auto& edgeID : vPathEdge)
					d2 += vEdge[edgeID].length;

			if(dTmp == INF)
				d2 = INF;

			vDistance.push_back(d2); 
			vector<int> vPreviousNodeTmp = vPreviousNode;
			vector<int> vPreviousEdgeTmp = vPreviousEdge;
			vPreviousNodeTmp.insert(vPreviousNodeTmp.end(), vPath.begin()+1, vPath.end());
			vPreviousEdgeTmp.insert(vPreviousEdgeTmp.end(), vPathEdge.begin(), vPathEdge.end());
	
			vvPathCandidate.push_back(vPreviousNodeTmp);
			vvPathCandidateEdge.push_back(vPreviousEdgeTmp);
			
			vPathParent.push_back(topPathID);
			vPathParentPos.push_back(ie - vvPathCandidateEdge[topPathID].begin());
		
			vvPathRemovedEdge.push_back(vRemovedEdgeTmp);
			qPath.update(vvPathCandidate.size()-1, d2);
			
			vPreviousNode.push_back(eID2);
			vPreviousEdge.push_back(*ie);
		}	
		
		//Recover the removed Edges
/*		for(auto ie = vRemovedEdgeTmp.begin(); ie != vRemovedEdgeTmp.end(); ie++)
		{
			int eID1 = vEdge[*ie].ID1;
			int eID2 = vEdge[*ie].ID2;
			adjSideCost[eID1].push_back(make_pair(eID2, vSideEdge[*ie].length));
			adjSideRoad[eID1].push_back(make_pair(eID2, *ie));
		}*/
	}

	return kResults.size();
}

void Graph::rSPT(int root, vector<int>& vSPTDistance, vector<int>& vSPTParent, vector<int>& vSPTParentEdge, vector<vector<int> >& vSPT)  
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
		for(int i = 0; i < (int)adjListR[topNodeID].size(); i++)
		{
			neighborNodeID = adjListR[topNodeID][i].first;
			neighborLength = adjListR[topNodeID][i].second;
			neighborRoadID = adjListEdgeR[topNodeID][i].second;
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

void Graph::intervalLabel(int root, vector<vector<int> >& vSPT, vector<int>& vSPTParent, vector<label>& vLabel) 
{
	stack<int> sDFS;
	sDFS.push(root); 
	int count = 0;
	vector<bool> vbVisited(nodeNum, false);
	while(!sDFS.empty()) 
	{
		int u = sDFS.top();
		if(vbVisited[u] == false) 
		{
			vLabel[u].pre = count;
			count++;
			vbVisited[u] = true;
			for(auto& v : vSPT[u])
				sDFS.push(v);
		}
		else
		{
			vLabel[u].post = count;
			count++;
			sDFS.pop(); 
			if(vSPTParent[u] != -1)
				vLabel[u].parent = vLabel[vSPTParent[u]].pre;
		}
	}
}
	
int Graph::DijkstraSideCost(int ID1, int ID2, vector<vector<pair<int, int> > >& adjSideCost, vector<vector<pair<int, int> > >& adjSideRoad, vector<int>& vPath, vector<int>& vPathEdge) 
{
	benchmark::heap<2, int, int> queue(nodeNum);
	queue.update(ID1, 0);

	vector<int> vDistance(nodeNum, INF);
	vector<int> vPrevious(nodeNum, -1);
	vector<int> vPreviousEdge(nodeNum, -1);
	vector<bool> vbVisited(nodeNum, false);
	int topNodeID, neighborNodeID, neighborLength, neighborRoadID;
	vector<pair<int, int> >::iterator ivp;

	vDistance[ID1] = 0;

	while(!queue.empty())
	{
		int topDistance; 
		queue.extract_min(topNodeID, topDistance); 
		vbVisited[topNodeID] = true;
		if(topNodeID == ID2)
			break;

		for(int i = 0; i < (int)adjSideCost[topNodeID].size(); i++) 
		{
			neighborNodeID = adjSideCost[topNodeID][i].first;
			neighborLength = adjSideCost[topNodeID][i].second; 
			neighborRoadID = adjSideRoad[topNodeID][i].second;
			int d = vDistance[topNodeID] + neighborLength;
			if(!vbVisited[neighborNodeID])
			{
				if(vDistance[neighborNodeID] > d)
				{
					vDistance[neighborNodeID] = d;
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
	while(p != -1)
	{
		vPath.push_back(p);
		vPathEdge.push_back(e);
		e = vPreviousEdge[p];
		p = vPrevious[p];
	}

	reverse(vPath.begin(), vPath.end());
	reverse(vPathEdge.begin(), vPathEdge.end());

	return vDistance[ID2];
}

int Graph::DijkstraSideCostPrune(int ID1, int ID2, vector<vector<pair<int, int> > >& adjSideCost, vector<vector<pair<int, int> > >& adjSideRoad, vector<int>& vSPTParent, vector<int>& vSPTParentEdge, vector<int>& vSPTDistance, vector<label>& vLabel, vector<int>& vIgnoredNode, vector<int>& vPath, vector<int>& vPathEdge, bool& bE)   
{
	benchmark::heap<2, int, int> queue(nodeNum);
	queue.update(ID1, 0);

	vector<int> vDistance(nodeNum, INF);
	vector<int> vPrevious(nodeNum, -1);
	vector<int> vPreviousEdge(nodeNum, -1);
	vector<bool> vbVisited(nodeNum, false);
	int topNodeID, neighborNodeID, neighborLength, neighborRoadID;
	vector<pair<int, int> >::iterator ivp; 

//	cout << "Ignored:";
	for(auto& v : vIgnoredNode) 
	{
//		cout << v << "\t";
		vbVisited[v] = true;
	}
//	cout << endl;

	vDistance[ID1] = 0;

	while(!queue.empty())
	{
		int topDistance; 
		queue.extract_min(topNodeID, topDistance); 
		vbVisited[topNodeID] = true;
		if(topNodeID == ID2)
			break;
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

//			cout << "top:" << topNodeID << endl; 
			
			while(p != -1)
			{
				vPath.push_back(p);
				vPathEdge.push_back(e);
				d += vEdge[e].length;
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
//			if(d2 != vSPTDistance[topNodeID])
//				cout << "Error:" << d2 << "\t" << vSPTDistance[topNodeID] << endl;
//			cout << "dist:" <<  topDistance  << "\t" << vSPTDistance[topNodeID] << endl;
			
			return  d + vSPTDistance[topNodeID];
		}

		for(int i = 0; i < (int)adjSideCost[topNodeID].size(); i++) 
		{
			neighborNodeID = adjSideCost[topNodeID][i].first;
			neighborLength = adjSideCost[topNodeID][i].second; 
			neighborRoadID = adjSideRoad[topNodeID][i].second;
			int d = vDistance[topNodeID] + neighborLength;
			if(!vbVisited[neighborNodeID])
			{
				if(vDistance[neighborNodeID] > d)
				{
					vDistance[neighborNodeID] = d;
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
	while(p != -1)
	{
		vPath.push_back(p);
		vPathEdge.push_back(e);
		e = vPreviousEdge[p];
		p = vPrevious[p];
	}

	reverse(vPath.begin(), vPath.end());
	reverse(vPathEdge.begin(), vPathEdge.end());

	bE = false;
	return vDistance[ID2];
}
