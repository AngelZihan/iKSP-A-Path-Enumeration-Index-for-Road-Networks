#include "graph.h"

int Graph::readBeijingMapDirected(string filename)
{
	ifstream inGraph(filename.c_str());
	if(!inGraph)
	{
		cout << "Cannot open Beijing Map " << filename << endl;
		return -1;
	}

	int nodeNum, i;
	inGraph >> nodeNum >> minX >> maxX >> minY >> maxY;
	cout << "Beijing Node number: " << nodeNum << endl;
	this->nodeNum = nodeNum;

	vector<Edge> vEdges; 
	vCoor.reserve(nodeNum);
	

	double x, y;
	int nodeID, type, j, k;
	int ID2, length;
	for(i = 0; i < nodeNum; i++)
	{
		inGraph >> nodeID >> type >> x >> y >> j >> j; 
		vCoor.push_back(make_pair(x, y));

		for(k = 0; k < j; k++)
		{
			inGraph >> ID2 >> length; 
			struct Edge e, eR;
			e.ID1 = nodeID;
			e.ID2 = ID2;
			e.length = length;
			vEdges.push_back(e);

			eR.ID1 = ID2;
			eR.ID2 = nodeID;
			eR.length = length;
			vEdgeR.push_back(eR);
		}

		inGraph >> j;
		for(k = 0; k < j; k++)
			inGraph >> ID2 >> length;

		inGraph >> j;
		for(k = 0; k < j; k++)
			inGraph >> ID2 >> length;
	}

	cout << "Finish Reading " << filename << endl;
	inGraph.close();

	adjList.resize(nodeNum);
	adjListR.resize(nodeNum);
	adjListEdge.resize(nodeNum);
	adjListEdgeR.resize(nodeNum);
	int edgeCount = 0;
	for(auto &edge: vEdges)
	{
		int ID1 = edge.ID1;
		int ID2 = edge.ID2;
		int length = edge.length;

		bool b = false;
		for(auto ivp = adjList[ID1].begin(); ivp != adjList[ID1].end(); ivp++)
		{
			if((*ivp).first == ID2) 
			{
				b = true;
				break;
			}
		}

		if(!b) 
		{
			adjList[ID1].push_back(make_pair(ID2, length)); 
			adjListR[ID2].push_back(make_pair(ID1, length));

			edge.edgeID = edgeCount; 
			cout << ID2 << "\t" << ID1 << "\t" << edgeCount << endl;
			adjListEdge[ID1].push_back(make_pair(ID2, edgeCount)); 
			adjListEdgeR[ID2].push_back(make_pair(ID1, edgeCount));
			vEdge.push_back(edge);
			edgeCount++;
		}
	}

	cout << "Beijing Road number: " << edgeCount << endl;

	return nodeNum;
}

int Graph::readUSMap(string filename)
{ 
	ifstream inGraph(filename);
	if(!inGraph)
		cout << "Cannot open Map " << filename << endl; 
	cout << "Reading " << filename << endl;

	string line;
	do
	{
		getline(inGraph,line);
		if(line[0]=='p')
		{ 
			vector<string> vs;
			boost::split(vs,line,boost::is_any_of(" "),boost::token_compress_on);
			nodeNum = stoi(vs[2]); //edgenum=stoi(vs[3]);
			cout << "Nodenum " << nodeNum<<endl;
			edgeNum = 0;
		}
	}while(line[0]=='c'|| line[0]=='p');

	int ID1, ID2, length;
	adjList.resize(nodeNum);
	adjListR.resize(nodeNum);
	adjListEdge.resize(nodeNum);
	adjListEdgeR.resize(nodeNum);   
	unordered_map<int, int> um;
	vmEdge.assign(nodeNum, um); 
	unordered_map<int, bool> ub;
	vmEFinsihed.assign(nodeNum, ub);
    vmLFinsihed.assign(nodeNum, ub);
	int edgeCount = 0; 
	string a;
	while(!inGraph.eof())
	{
		vector<string> vs;
		boost::split(vs,line,boost::is_any_of(" "),boost::token_compress_on);
		ID1 = stoi(vs[1]) - 1;
		ID2 = stoi(vs[2]) - 1;
        //change when use rank graph
        /*ID1 = stoi(vs[1]);
        ID2 = stoi(vs[2]);*/
		length = stoi(vs[3]);
		
//		inGraph >> a >> ID1 >> ID2 >> length;
//		ID1 -= 1;
//		ID2 -= 1;
		
		struct Edge e; 
		e.ID1 = ID1;
		e.ID2 = ID2;
		e.length = length;
		e.edgeID = edgeCount; 
			
		bool bExisit = false;
		for(int i = 0; i < (int)adjList[ID1].size(); i++) 
		{
			if(adjList[ID1][i].first == ID2)
			{
				bExisit = true;
				break;
			}
		}

		if(!bExisit)
		{
			vEdge.push_back(e);
			adjList[ID1].push_back(make_pair(ID2, length));
			adjListR[ID2].push_back(make_pair(ID1, length));
			adjListEdge[ID1].push_back(make_pair(ID2, edgeCount));
			adjListEdgeR[ID2].push_back(make_pair(ID1, edgeCount));  
			vmEdge[ID1][ID2] = length; 
			vmEFinsihed[ID1][ID2] = true;
            vmLFinsihed[ID1][ID2] = true;
	//		umEdge.insert(make_pair(make_pair(ID1, ID2),length)); 
			edgeCount++;
		}
		getline(inGraph,line);
	}

    //For TJ
/*
    unordered_set<int> usedNodes;
    vector<bool> visited(nodeNum, false);
    vector<int> componentSize;
    vector<vector<int>> components;

    // Helper function for DFS
    std::function<void(int, vector<int>&)> dfs = [&](int node, vector<int>& component) {
        visited[node] = true;
        component.push_back(node);
        for(const auto& neighbor : adjList[node]) {
            if(!visited[neighbor.first]) {
                dfs(neighbor.first, component);
            }
        }
    };

    // Find all connected components
    for(int i = 0; i < nodeNum; i++) {
        if(!visited[i]) {
            vector<int> component;
            dfs(i, component);
            components.push_back(component);
            componentSize.push_back(component.size());
        }
    }

    // Find the largest component
    int largestComponentIndex = max_element(componentSize.begin(), componentSize.end()) - componentSize.begin();
    const vector<int>& largestComponent = components[largestComponentIndex];

    // Mark nodes that are part of the largest component
    unordered_set<int> largestComponentSet(largestComponent.begin(), largestComponent.end());

    nodeNum = largestComponent.size();
    adjList.clear();
    adjListR.clear();
    adjListEdge.clear();
    adjListEdgeR.clear();

    adjList.resize(nodeNum);
    adjListR.resize(nodeNum);
    adjListEdge.resize(nodeNum);
    adjListEdgeR.resize(nodeNum);

    // Rebuild the graph with only the largest connected component
    int newID = 0;
    for (int i = 0; i < nodeNum; i++) {
        newNodeMap[largestComponent[i]] = newID++;
//        cout << "old: " << largestComponent[i] << " new: " << newNodeMap[largestComponent[i]] << endl;
    }

    for (const auto& e : vEdge) {
        int newID1 = newNodeMap[e.ID1];
        int newID2 = newNodeMap[e.ID2];

        adjList[newID1].push_back(make_pair(newID2, e.length));
        adjListR[newID2].push_back(make_pair(newID1, e.length));
        adjListEdge[newID1].push_back(make_pair(newID2, e.edgeID));
        adjListEdgeR[newID2].push_back(make_pair(newID1, e.edgeID));
    }

    cout << "Graph reduced to largest component with " << nodeNum << " nodes and " << edgeCount << " edges." << endl;

*/






/*    for (int i = 0; i < nodeNum; ++i)
    {
        if (adjList[i].empty()) continue; // Skip already processed nodes

        if (adjList[i].size() == 1) // Degree 1: Directly delete the node
        {
            int neighbor = adjList[i][0].first;
            int edgeID = adjListEdge[i][0].second;

            // Remove the reference to this node from its neighbor
            adjList[neighbor].erase(
                    remove_if(adjList[neighbor].begin(), adjList[neighbor].end(),
                              [i](const pair<int, int>& p) { return p.first == i; }),
                    adjList[neighbor].end()
            );

            adjListEdge[neighbor].erase(
                    remove_if(adjListEdge[neighbor].begin(), adjListEdge[neighbor].end(),
                              [edgeID](const pair<int, int>& p) { return p.second == edgeID; }),
                    adjListEdge[neighbor].end()
            );

            adjListR[neighbor].erase(
                    remove_if(adjListR[neighbor].begin(), adjListR[neighbor].end(),
                              [i](const pair<int, int>& p) { return p.first == i; }),
                    adjListR[neighbor].end()
            );

            adjListEdgeR[neighbor].erase(
                    remove_if(adjListEdgeR[neighbor].begin(), adjListEdgeR[neighbor].end(),
                              [edgeID](const pair<int, int>& p) { return p.second == edgeID; }),
                    adjListEdgeR[neighbor].end()
            );

            vmEdge[i].clear();
            vmEFinsihed[i].clear();
            vmLFinsihed[i].clear();
            adjList[i].clear();
            adjListEdge[i].clear();
        }
        else if (adjList[i].size() == 2) // Degree 2: Update neighbors
        {
            int neighbor1 = adjList[i][0].first;
            int neighbor2 = adjList[i][1].first;
            int length1 = adjList[i][0].second;
            int length2 = adjList[i][1].second;

            int edgeID1 = adjListEdge[i][0].second;
            int edgeID2 = adjListEdge[i][1].second;

            // Add a new edge between neighbor1 and neighbor2
            adjList[neighbor1].push_back(make_pair(neighbor2, length1 + length2));
            adjList[neighbor2].push_back(make_pair(neighbor1, length1 + length2));

            adjListEdge[neighbor1].push_back(make_pair(neighbor2, edgeCount));
            adjListEdge[neighbor2].push_back(make_pair(neighbor1, edgeCount));
            edgeCount++;

            adjListR[neighbor1].push_back(make_pair(neighbor2, length1 + length2));
            adjListR[neighbor2].push_back(make_pair(neighbor1, length1 + length2));

            adjListEdgeR[neighbor1].push_back(make_pair(neighbor2, edgeCount));
            adjListEdgeR[neighbor2].push_back(make_pair(neighbor1, edgeCount));

            // Remove connections to the current node from the neighbors
            adjList[neighbor1].erase(
                    remove_if(adjList[neighbor1].begin(), adjList[neighbor1].end(),
                              [i](const pair<int, int>& p) { return p.first == i; }),
                    adjList[neighbor1].end()
            );

            adjListEdge[neighbor1].erase(
                    remove_if(adjListEdge[neighbor1].begin(), adjListEdge[neighbor1].end(),
                              [edgeID1](const pair<int, int>& p) { return p.second == edgeID1; }),
                    adjListEdge[neighbor1].end()
            );

            adjListR[neighbor1].erase(
                    remove_if(adjListR[neighbor1].begin(), adjListR[neighbor1].end(),
                              [i](const pair<int, int>& p) { return p.first == i; }),
                    adjListR[neighbor1].end()
            );

            adjListEdgeR[neighbor1].erase(
                    remove_if(adjListEdgeR[neighbor1].begin(), adjListEdgeR[neighbor1].end(),
                              [edgeID1](const pair<int, int>& p) { return p.second == edgeID1; }),
                    adjListEdgeR[neighbor1].end()
            );

            adjList[neighbor2].erase(
                    remove_if(adjList[neighbor2].begin(), adjList[neighbor2].end(),
                              [i](const pair<int, int>& p) { return p.first == i; }),
                    adjList[neighbor2].end()
            );

            adjListEdge[neighbor2].erase(
                    remove_if(adjListEdge[neighbor2].begin(), adjListEdge[neighbor2].end(),
                              [edgeID2](const pair<int, int>& p) { return p.second == edgeID2; }),
                    adjListEdge[neighbor2].end()
            );

            adjListR[neighbor2].erase(
                    remove_if(adjListR[neighbor2].begin(), adjListR[neighbor2].end(),
                              [i](const pair<int, int>& p) { return p.first == i; }),
                    adjListR[neighbor2].end()
            );

            adjListEdgeR[neighbor2].erase(
                    remove_if(adjListEdgeR[neighbor2].begin(), adjListEdgeR[neighbor2].end(),
                              [edgeID2](const pair<int, int>& p) { return p.second == edgeID2; }),
                    adjListEdgeR[neighbor2].end()
            );

            vmEdge[i].clear();
            vmEFinsihed[i].clear();
            vmLFinsihed[i].clear();
            adjList[i].clear();
            adjListEdge[i].clear();
        }
    }*/


    vbISO.assign(nodeNum, false);
	inGraph.close();
	return nodeNum;
}

void Graph::readSimpleGraph(string filename)
{
	ifstream inGraph(filename);
	if(!inGraph)
		cout << "Cannot open Map " << filename << endl; 
	cout << "Reading " << filename << endl;

	inGraph >> nodeNum >> edgeNum;

	int ID1, ID2, length;
	adjList.resize(nodeNum);
	adjListR.resize(nodeNum);
	adjListEdge.resize(nodeNum);
	adjListEdgeR.resize(nodeNum);
    unordered_map<int, int> um;
    vmEdge.assign(nodeNum, um);
    unordered_map<int, bool> ub;
    vmEFinsihed.assign(nodeNum, ub);
    vmLFinsihed.assign(nodeNum, ub);
	int edgeCount = 0; 
	string a;

	while(inGraph >> ID1 >> ID2 >> length)
	{
		struct Edge e; 
		e.ID1 = ID1;
		e.ID2 = ID2;
		e.length = length;
		e.edgeID = edgeCount; 
			
		vEdge.push_back(e);
		adjList[ID1].push_back(make_pair(ID2, length));
		adjListR[ID2].push_back(make_pair(ID1, length));
		adjListEdge[ID1].push_back(make_pair(ID2, edgeCount)); 
		adjListEdgeR[ID2].push_back(make_pair(ID1, edgeCount));
        vmEdge[ID1][ID2] = length;
        vmEFinsihed[ID1][ID2] = true;
        vmLFinsihed[ID1][ID2] = true;
        //		umEdge.insert(make_pair(make_pair(ID1, ID2),length));
		edgeCount++;
	}

	inGraph.close();
}


int Graph::readUSMapCost(string filename)
{ 
	ifstream inGraph(filename);
	if(!inGraph)
		cout << "Cannot open Map " << filename << endl; 
	cout << "Reading " << filename << endl;

	string line; 
//	int eNum;
	inGraph >> nodeNum;  
//	inGraph >> nodeNum >> eNum; 

	int ID1, ID2, length, cost;
	adjList.resize(nodeNum);
	adjListR.resize(nodeNum);
	adjListEdge.resize(nodeNum);
	adjListEdgeR.resize(nodeNum); 
	adjListCost.resize(nodeNum);
	adjListCostR.resize(nodeNum);
	int edgeCount = 0;
//	cost = 0;
	while(!inGraph.eof())
	{
		inGraph >> ID1 >> ID2 >> length >> cost;
//		inGraph >> ID1 >> ID2 >> length; 
		ID1 -= 1; 
		ID2 -= 1;  

		struct Edge e; 
		e.ID1 = ID1;
		e.ID2 = ID2;
		e.length = length;
		e.edgeID = edgeCount;  
		e.cost = cost;

		bool bExisit = false;
		for(int i = 0; i < (int)adjListCost[ID1].size(); i++) 
		{
			if(adjListCost[ID1][i].first == ID2)
			{
				bExisit = true;
				if(cost < adjListCost[ID1][i].second)
				{
//					cout << "Cost update:" << adjListCost[ID1][i].second << "\t" << cost << endl; 
					adjListCost[ID1][i].second = cost; 
					for(int j = 0; j < (int)adjListCostR[ID2].size(); j++)
					{
						if(adjListCostR[ID2][j].first == ID1)
						{
							adjListCostR[ID2][j].second = cost;    
							break;
						}
					}

					int eID = adjListEdge[ID1][i].second;
					vEdge[eID].cost = cost;
				}
				break;
			}
		}

		if(!bExisit)
		{
			vEdge.push_back(e);
			adjList[ID1].push_back(make_pair(ID2, length));
			adjListR[ID2].push_back(make_pair(ID1, length));
			adjListEdge[ID1].push_back(make_pair(ID2, edgeCount)); 
			adjListEdgeR[ID2].push_back(make_pair(ID1, edgeCount));
			adjListCost[ID1].push_back(make_pair(ID2, cost));
			adjListCostR[ID2].push_back(make_pair(ID1, cost)); 
			edgeCount++;
		}
	//	getline(inGraph,line);
	}

/*	for(int i = 0; i < (int)adjListCost.size(); i++)
	{
		unordered_map<int, int> us;
		for(auto& c: adjListCost[i])
		{
			if(us.find(c.first) != us.end())
			{
				cout << "Repeated! " << i << "\t" << c.first << "\t" << us[c.first] << "\t" << c.second << endl;
			}
			else
				us[c.first] = c.second;
		}
	}
*/
	vbISO.assign(nodeNum, false); 
	inGraph.close();
/*	
	vector<int> vCorrect;
	ifstream ic("./c"); 
	int icc;
	while(ic >> icc)
		vCorrect.push_back(icc); 
	int c= 0;
	int d = 0;
	int oldv = -1; 
	for(auto&v : vCorrect)
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
		cout << endl << d << "\t" << c << "\t" << endl;
	}
	*/

	return nodeNum;
}

int Graph::readExampleMap(string filename)
{ 
	ifstream inGraph(filename);
	if(!inGraph)
		cout << "Cannot open Map " << filename << endl; 
	cout << "Reading " << filename << endl;

	string line;
	do
	{
		getline(inGraph,line);
		if(line[0]=='p')
		{ 
			vector<string> vs;
			boost::split(vs,line,boost::is_any_of(" "),boost::token_compress_on);
			nodeNum = stoi(vs[2]); //edgenum=stoi(vs[3]);
			cout << "Nodenum " << nodeNum<<endl;
			edgeNum = 0;
		}
	}while(line[0]=='c'|| line[0]=='p');

	int ID1, ID2, length;
	adjList.resize(nodeNum);
	adjListR.resize(nodeNum);
	adjListEdge.resize(nodeNum);
	adjListEdgeR.resize(nodeNum); 
	int edgeCount = 0;
	while(!inGraph.eof())
	{
		vector<string> vs;
		boost::split(vs,line,boost::is_any_of(" "),boost::token_compress_on);
		ID1 = stoi(vs[1]); 
		ID2 = stoi(vs[2]); 
		length = stoi(vs[3]);
		
		struct Edge e; 
		e.ID1 = ID1;
		e.ID2 = ID2;
		e.length = length;
		e.edgeID = edgeCount; 
		vEdge.push_back(e);

		cout << ID1 << "\t" << ID2 << "\t" << length << endl;
		adjList[ID1].push_back(make_pair(ID2, length));
		adjListR[ID2].push_back(make_pair(ID1, length));
		adjListEdge[ID1].push_back(make_pair(ID2, edgeCount)); 
		adjListEdgeR[ID2].push_back(make_pair(ID1, edgeCount));
		edgeCount++;
		getline(inGraph,line);
	}

	vbISO.assign(nodeNum, false); 
	inGraph.close();

//	mCoor["NY"] = make_pair(84000, 69000);

	return nodeNum;
}
	
int Graph::readUSCost(string filename)
{
	ifstream inGraph(filename);
	if(!inGraph)
		cout << "Cannot open Map " << filename << endl; 
	cout << "Reading " << filename << endl;

	string line;
	do
	{
		getline(inGraph,line);
	}while(line[0]=='c'||line[0]=='p');

	int ID1, ID2, cost; 
	adjListCost.resize(nodeNum);
	adjListCostR.resize(nodeNum);
	int edgeCount = 0;
	while(!inGraph.eof())
	{
		vector<string> vs;
		boost::split(vs,line,boost::is_any_of(" "),boost::token_compress_on);
		ID1 = stoi(vs[1]) - 1;  
		ID2 = stoi(vs[2]) - 1; 
		cost = stoi(vs[3]);
		
		adjListCost[ID1].push_back(make_pair(ID2, cost));
		adjListCostR[ID2].push_back(make_pair(ID1, cost)); 
		vEdge[edgeCount].cost = cost;
		edgeCount++;
		getline(inGraph,line);
	}
	
	cout << vEdge.size() << endl;
	return nodeNum;
}

int Graph::DijkstraPath(int ID1, int ID2, vector<int>& vPath, vector<int>& vPathEdge)
{
	benchmark::heap<2, int, int> queue(adjList.size());
	queue.update(ID1, 0);

	vector<int> vDistance(adjList.size(), INF);
	vector<int> vPrevious(adjList.size(), -1);
	vector<int> vPreviousEdge(adjList.size(), -1);
	vector<bool> vbVisited(adjList.size(), false);
	int topNodeID, neighborNodeID, neighborLength, neighborRoadID;

	vDistance[ID1] = 0;

	while(!queue.empty())
	{
		int topDistance; 
		queue.extract_min(topNodeID, topDistance); 
		vbVisited[topNodeID] = true;
		if(topNodeID == ID2)
			break;

		for(int i = 0; i < (int)adjList[topNodeID].size(); i++)
		{
			neighborNodeID = adjList[topNodeID][i].first;
			neighborLength = adjList[topNodeID][i].second; 
			neighborRoadID = adjListEdge[topNodeID][i].second;
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

//	if(vPathEdge.size() > 0)
//		vPathEdge.erase(vPathEdge.end()-1);

	reverse(vPath.begin(), vPath.end());
	reverse(vPathEdge.begin(), vPathEdge.end());

	return vDistance[ID2];
}

int Graph::DijkstraPath2(int ID1, int ID2, unordered_set<int>& sRemovedNode, vector<int>& vPath, vector<int>& vPathEdge)
{
	benchmark::heap<2, int, int> queue(adjList.size());
	queue.update(ID1, 0);

	vector<int> vDistance(adjList.size(), INF);
	vector<int> vPrevious(adjList.size(), -1);
	vector<int> vPreviousEdge(adjList.size(), -1);
	vector<bool> vbVisited(adjList.size(), false);
	int topNodeID, neighborNodeID, neighborLength, neighborRoadID;

	vDistance[ID1] = 0;

	while(!queue.empty())
	{
		int topDistance; 
		queue.extract_min(topNodeID, topDistance); 
		vbVisited[topNodeID] = true;
		if(topNodeID == ID2)
			break;

		for(int i = 0; i < (int)adjList[topNodeID].size(); i++)
		{
			neighborNodeID = adjList[topNodeID][i].first; 
			if(sRemovedNode.find(neighborNodeID) != sRemovedNode.end())
				continue;
			neighborLength = adjList[topNodeID][i].second; 
			neighborRoadID = adjListEdge[topNodeID][i].second;
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

//	if(vPathEdge.size() > 0)
//		vPathEdge.erase(vPathEdge.end()-1);

	reverse(vPath.begin(), vPath.end());
	reverse(vPathEdge.begin(), vPathEdge.end());

	return vDistance[ID2];
}

int Graph::Dijkstra(int ID1, int ID2)
{
	benchmark::heap<2, int, int> queue(adjList.size());
	queue.update(ID1, 0);

	vector<int> vDistance(nodeNum, INF);
	vector<bool> vbVisited(nodeNum, false);
	int topNodeID, neighborNodeID, neighborLength;
	vector<pair<int, int> >::iterator ivp;

	vDistance[ID1] = 0;

	compareNode cnTop;
	while(!queue.empty())
	{
		int topDistance; 
		queue.extract_min(topNodeID, topDistance); 
		vbVisited[topNodeID] = true; 
//		cout << topNodeID << "\t" << vDistance[topNodeID] << endl;
		if(topNodeID == ID2)
			break;

		for(ivp = adjList[topNodeID].begin(); ivp != adjList[topNodeID].end(); ivp++)
		{
			neighborNodeID = (*ivp).first;
			neighborLength = (*ivp).second; 
			int d = vDistance[topNodeID] + neighborLength;
			if(!vbVisited[neighborNodeID])
			{
				if(vDistance[neighborNodeID] > d)
				{
					vDistance[neighborNodeID] = d;
					queue.update(neighborNodeID, d);
				}
			}
		}
	}

	return vDistance[ID2];
}

int Graph::AStar(int ID1, int ID2)
{
	benchmark::heap<2, int, int> queue(adjList.size());
	vector<int> vDistance(adjList.size(), INF);
	vector<bool> vbVisited(adjList.size(), false);
	int topNodeID, neighborNodeID, neighborLength;
	vector<pair<int, int> >::iterator ivp;

	queue.update(ID1, EuclideanDistance(ID1, ID2)); 
	vDistance[ID1] = 0;

	compareNode cnTop;
	while(!queue.empty())
	{
		int topDistance;
		queue.extract_min(topNodeID, topDistance);
		vbVisited[topNodeID] = true;

		if(topNodeID == ID2)
			break;

		for(ivp = adjList[topNodeID].begin(); ivp != adjList[topNodeID].end(); ivp++)

		{
			neighborNodeID = (*ivp).first;
			neighborLength = (*ivp).second; 
			int d = vDistance[topNodeID] + neighborLength;
			if(!vbVisited[neighborNodeID])
			{
				if(vDistance[neighborNodeID] > d)
				{
					vDistance[neighborNodeID] = d;
					queue.update(neighborNodeID, d+EuclideanDistance(neighborNodeID, ID2));
				}
			}
		}
	}

	return vDistance[ID2];
}

int Graph::AStarPath(int ID1, int ID2, vector<int>& vPath, vector<int>& vPathEdge, string& city)
{
	benchmark::heap<2, int, int> queue(adjList.size());
	vector<int> vDistance(adjList.size(), INF);
	vector<int> vPrevious(adjList.size(), -1);
	vector<int> vPreviousEdge(adjList.size(), -1);
	vector<bool> vbVisited(adjList.size(), false);
	int topNodeID, neighborNodeID, neighborLength, neighborRoadID;

	int latU, lonU;
	if(city == "US")
	{
		lonU = 84000;
		latU = 69000;
	}
	else
	{
		lonU = 83907;
		latU = 111319; 
	}

	queue.update(ID1, EuclideanDistance(ID1, ID2)); 
	vDistance[ID1] = 0;

	compareNode cnTop;
	while(!queue.empty())
	{
		int topDistance;
		queue.extract_min(topNodeID, topDistance);
		vbVisited[topNodeID] = true;

		if(topNodeID == ID2)
			break;

		for(int i = 0; i < (int)adjList[topNodeID].size(); i++)
		{
			neighborNodeID = adjList[topNodeID][i].first;
			neighborLength = adjList[topNodeID][i].second; 
			neighborRoadID = adjListEdge[topNodeID][i].second;
			int d = vDistance[topNodeID] + neighborLength;
			if(!vbVisited[neighborNodeID])
			{
				if(vDistance[neighborNodeID] > d)
				{
					vDistance[neighborNodeID] = d;
					queue.update(neighborNodeID, d+EuclideanDistance(neighborNodeID, ID2));
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

inline int Graph::EuclideanDistance(int ID1, int ID2)
{
	int lat=(int)(abs(vCoor[ID1].first - vCoor[ID2].first)*111319);
	int lon=(int)(abs(vCoor[ID1].second - vCoor[ID2].second)*83907);
	int min, max;
	min = (lat > lon) ? lon : lat;
	max = (lat > lon) ? lat : lon;
	int approx = max*1007 + min*441;
	if(max < (min << 4))
		approx -= max * 40;
	return (approx + 512) >> 10;
}

inline int Graph::EuclideanDistanceAdaptive(int ID1, int ID2, int latU, int lonU)
{
	int lat=(int)(abs(vCoor[ID1].first - vCoor[ID2].first)*latU);
	int lon=(int)(abs(vCoor[ID1].second - vCoor[ID2].second)*lonU);
	int min, max;
	min = (lat > lon) ? lon : lat;
	max = (lat > lon) ? lat : lon;
	int approx = max*1007 + min*441;
	if(max < (min << 4))
		approx -= max * 40;
	return (approx + 512) >> 10;
}

int Graph::ISONodes()
{
	srand (time(NULL));
	vbISOF.assign((int)adjList.size(), false);	//forward
	vbISOB.assign((int)adjList.size(), false);	//backward
	vbISOU.assign( (int)adjList.size(), false);	//F & B
	vbISO.assign((int)adjList.size(), false);	//F | B

	ifstream ifISOF("./beijingISOF");
	ifstream ifISOB("./beijingISOB");
	ifstream ifISOU("./beijingISOU");
	ifstream ifISO("./beijingISO");

	if(!ifISOF || !ifISOB || !ifISOU || !ifISO)
	{
		//ISO files do not exist
		//Create new ones
		srand(time(NULL));
		cout << "Identifying ISO Nodes" << endl;
		for(int i = 0; i < 10; i++)
		{
			int nodeID = rand() % adjList.size();
			cout <<nodeID <<endl;
			vector<bool> vbVisitedF;
			vector<bool> vbVisitedB;
			int vnF = BFS(nodeID, true, vbVisitedF);
			int vnB = BFS(nodeID, false, vbVisitedB);
			cout <<vnF <<"\t" << vnB <<endl;

			if(vnF < 1000 || vnB < 1000)
				continue;

			for(int j = 0; j < (int)adjList.size(); j++)
			{
				if(!vbVisitedF[j])
					vbISOF[j] = true;

				if(!vbVisitedB[j])
					vbISOB[j] = true;

				if(!vbVisitedF[j] || !vbVisitedB[j])
					vbISOU[j] = true;

				if(!vbVisitedF[j] && !vbVisitedB[j])
					vbISO[j] = true;
			}
		}

		ofstream ofF("./beijingISOF");
		ofstream ofB("./beijingISOB");
		ofstream ofU("./beijingISOU");
		ofstream of("./beijingISO");

		for(int i = 0; i < (int)adjList.size(); i++)
		{
			if(vbISOF[i])
				ofF << i << endl;

			if(vbISOB[i])
				ofB << i << endl;

			if(vbISOU[i])
				ofU << i << endl;

			if(vbISO[i])
				of << i << endl;
		}

		ofF.close();
		ofB.close();
		ofU.close();
		of.close();

		return 0;
	}
	else
	{
		int nodeID;
		cout << "Loading ISO Nodes" << endl;
		while(ifISOF >> nodeID)
			vbISOF[nodeID] = true;

		while(ifISOB >> nodeID)
			vbISOB[nodeID] = true;

		while(ifISOU >> nodeID)
			vbISOU[nodeID] = true;

		while(ifISO >> nodeID)
			vbISO[nodeID] = true;

		return 0;
	}
}

int Graph::BFS(int nodeID, bool bF, vector<bool>& vbVisited)
{
	vbVisited.assign(adjList.size()+1, false);
	queue<int> Q;
	Q.push(nodeID);
	vbVisited[nodeID] = true;
	int count = 0;
	while(!Q.empty())
	{
		int topNodeID = Q.front();
		Q.pop();
		count++;
		if(bF)
		{
			for(auto& it : adjList[topNodeID])
				if(!vbVisited[it.first])
				{
					vbVisited[it.first] = true;
					Q.push(it.first);
				}
		}
		else
		{
			for(auto& it : adjListR[topNodeID])
				if(!vbVisited[it.first])
				{
					vbVisited[it.first] = true;
					Q.push(it.first);
				}
		}
	}

	return count;
}
	
int Graph::readUSCoor(string filename) 
{
	vCoor.reserve(nodeNum);
	ifstream inFile(filename.c_str()); 
	if(!inFile)
	{
		cout << "Cannot open " << filename << endl;
		return -1;
	}

	string s;
	int nodeID;
	double lon, lat;
	while(inFile >> s)
	{
		if(s == "v") 
		{
			inFile >> nodeID >> lon >> lat;
			lon = -lon / 1000000;
			lat = lat / 1000000;
			vCoor.push_back(make_pair(lon, lat));
		}
	}

	return 0;
}

bool Graph::loadGraphData(const std::string& filename) {
    try {
        std::ifstream ifs(filename);
        if (!ifs.is_open()) {
            std::cerr << "Error: Cannot open file " << filename << std::endl;
            return false;
        }
        /*if (!ifs.is_open()) {
            std::cerr << "Error: Cannot open file ./NY.TD.data" << std::endl;
        } else {
            std::cout << "File ./NY.TD.data opened successfully." << std::endl;
        }*/
        boost::archive::text_iarchive ia(ifs);
        cout << "run *this " << endl;
        ia >> *this;  // Deserialize the Graph object
        cout << "finish run *this" << endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading graph data: " << e.what() << std::endl;
        return false;
    }
}

void Graph::saveGraphData(const std::string& filename) {
    std::ofstream ofs(filename);
    if (!ofs.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return;
    }

    // Debugging: print out some key members
    std::cout << "nodeNum: " << nodeNum << std::endl;
    std::cout << "edgeNum: " << edgeNum << std::endl;
    std::cout << "SCconNodes size: " << SCconNodes.size() << std::endl;
    // Add more debugging information if necessary

    try {
        boost::archive::text_oarchive oa(ofs);
        oa << *this;  // Serialize the Graph object to the archive
    } catch (const std::exception& e) {
        std::cerr << "Error saving graph data: " << e.what() << std::endl;
    }
}
