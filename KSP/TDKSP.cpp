#include "graph.h"

/*priority_queue<subH, vector<subH>, CompareH> hQ;
vector<int> vK;
//vector<int> vchaPath;
vector<int> vKW;
int KSPNumber = 0;
vector<vector<int>> v1;
vector<vector<int>> v2;
vector<int> tmpv;
vector<vector<int>> vNode;
//vector<string> vPathstr;
vector<vector<int>> vRank;
vector<int> pathTotal;
vector<vector<int>> rankSet1;
vector<vector<int>> rankSet2;
vector<pair<int, int> > vSubA;
//unordered_set<string> uniquePaths;
unordered_set<uint64_t> hashSet;
//vector<string> uniquePaths;
bool bPrint = false;
vector<pair<vector<int>, vector<int> > >vpvADistance;
//    vector<int> vA;
pair<vector<int>, vector<int> > tmpvpvADistance;

vector<pair<pair<int, int>, pair<int, int > > > vR;
vector<set<pair<int, int>>> visited;
set<pair<int, int>> tmpvisited;
pair<pair<int, int>, pair<int, int > > tmpvR;
unordered_map<int, int> umAPos;
vector<bool> vbFinished; //Finish Enumeration of A_i
vector<pair<bool, bool> > vpbFinished;
bool bFinished;*/


vector<int> _DD,_DD2;
struct DegComp
{
    int x;
    DegComp(int _x)
    {
        x = _x;
    }

    bool operator< (const DegComp d) const
    {
        if(_DD[x] != _DD[d.x])
            return _DD[x] < _DD[d.x];
        if(_DD2[x] != _DD2[d.x])
            return _DD2[x] < _DD2[d.x];
        return x < d.x;
    }
};

void printDeg(const std::set<DegComp>& Deg) {
    for (const auto& elem : Deg) {
        std::cout << "Node index: " << elem.x;
        std::cout << ", Degree (_DD): " << _DD[elem.x];
        std::cout << ", Degree (_DD2): " << _DD2[elem.x] << std::endl;
    }
}

static bool seCompare(seEnum se1, seEnum se2)
{
    if(GlobalNodeOrder[se1.ID1] < GlobalNodeOrder[se2.ID1])
        return true;
    else if(GlobalNodeOrder[se1.ID1] == GlobalNodeOrder[se2.ID1])
    {
        if(GlobalNodeOrder[se1.ID2] < GlobalNodeOrder[se2.ID2])
            return true;
        else
            return false;
    }
    else
        return false;
}

void Graph::loadTD(string filename)
{
    ifstream infile(filename);
    if(!infile)
        TDConstruction(filename);
    else
        readTD(filename);
}

void Graph::TDConstruction(string filename)
{
    CHConstruction();
    makeTree();
    cout << "Finish Make Tree" << endl;
    makeIndex();
    cout << "Finish Make Index" << endl;

   /* rankCHConstruction();
    makeTree();
    cout << "Finish Make Tree" << endl;
    makeIndex();
    cout << "Finish Make Index" << endl;*/
}

void Graph::CHConstruction()
{
    //Contracted Graph E
    //Neighbor, <Distance, |Same Min Value|>
    map<int, pair<int,int> > m;
    E.assign(nodeNum,m);
    for(int i = 0; i < (int)adjList.size(); i++)
        for(int j = 0; j < (int)adjList[i].size(); j++)
            E[i].insert(make_pair(adjList[i][j].first,make_pair(adjList[i][j].second, 1)));
    _DD.assign(nodeNum, 0);
    _DD2.assign(nodeNum, 0);
    DD.assign(nodeNum, 0);
    DD2.assign(nodeNum, 0);


    set<DegComp> Deg;
    int degree;
    for(int i = 0; i < nodeNum; i++)
    {
        degree = adjList[i].size();
        /*if(i ==10165){
            cout << i << " " << degree << endl;
        }*/
        if(degree != 0)
        {
            _DD[i] = degree;
            _DD2[i] = degree;
            DD[i] = degree;
            DD2[i] = degree;
            Deg.insert(DegComp(i));
        }
    }
//    cout << DD[10165] << endl;
//    printDeg(Deg);

    vector<bool> exist(nodeNum, true);
    //If Degree Changed by Contraction
    vector<bool> change(nodeNum, false);

    //Contracted Neighbors
    //Neighbor, <Distance, Supportive Node Number>
    vector<pair<int,pair<int,int> > > vect;
    NeighborCon.assign(nodeNum, vect);
    SCconNodes.clear();

    //Vector-based SE for KSP
    vector<seEnum> vSETmp;
    vector<vector<seEnum> > vvSETmp;
    vvSE.assign(nodeNum, vSETmp);
    vvSETmp.assign(nodeNum, vSETmp);
    unordered_map<int, int> um;
    vector<unordered_map<int, int> > vmSEPosTmp;
    vmSEPos.assign(nodeNum, um);
    vmSEPosTmp.assign(nodeNum, um);


    cout << "Begin to contract" << endl;
    int count = 0;
    while(!Deg.empty())
    {
        count++;
        int x = (*Deg.begin()).x;

        while(true)
        {
            if(change[x])
            {
//                cout << "change x: " << x << endl;
                Deg.erase(DegComp(x));
                _DD[x] = DD[x];
                _DD2[x] = DD2[x];
                Deg.insert(DegComp(x));
                change[x] = false;
                x = (*Deg.begin()).x;
            }
            else
                break;
        }
//        cout << "x: " << x << " degree: " << DD[x] << endl;
        vNodeOrder.push_back(x);
        Deg.erase(Deg.begin());
        exist[x] = false;

        //Tmp neighbors of the contracted graph
        vector<pair<int, pair<int, int> > > Neigh;
        for(auto& it : E[x]) {
//            cout << "neighbour first: " << it.first << " neighbour second: " << it.second.first << endl;
            if(exist[it.first])
                Neigh.push_back(it);
        }

//        cout << "Neigh size: " << Neigh.size() << endl;
        NeighborCon[x].assign(Neigh.begin(), Neigh.end());
        //if(x == 1)
        //			cout << x << "\t" << Neigh.size() << endl;

        //Maintain E
        for(int i=0; i < (int)Neigh.size(); i++)
        {
            int y = Neigh[i].first;
            deleteE(x,y);
            change[y] = true;
        }

        int	ID1, ID2, dTmp;
        for(int i = 0; i<(int)Neigh.size(); i++)
        {
            for(int j=i+1; j<(int)Neigh.size(); j++)
            {
                ID1 = Neigh[i].first;
                ID2 = Neigh[j].first;
                dTmp =  Neigh[i].second.first+Neigh[j].second.first;
                insertE(ID1, ID2, dTmp);

                //Degree Changed
                change[ID1] = true;
                change[ID2] = true;

                //Temporally Store once with smaller ID
                if(ID1 < ID2)
                {
                    //		SCconNodes[make_pair(ID1, ID2)].push_back(x);
                    if(vmSEPosTmp[ID1].find(ID2) == vmSEPosTmp[ID1].end())
                    {
                        seEnum se;
                        se.ID1 = ID1;
                        se.ID2 = ID2;
                        se.wNum = 0;
                        //se.vK.push_back(dTmp);
                        vvSETmp[ID1].push_back(se);
                        vmSEPosTmp[ID1].insert(make_pair(ID2, vvSETmp[ID1].size()-1));
                    }
                    int pos = vmSEPosTmp[ID1][ID2];
//                    cout << "position: " << pos << endl;
                    vvSETmp[ID1][pos].wNum++;
                    vvSETmp[ID1][pos].vW.push_back(x);
                    vvSETmp[ID1][pos].umWPos[x] = vvSETmp[ID1][pos].wNum-1;
                    vvSETmp[ID1][pos].vDistance.push_back(dTmp);
                    vvSETmp[ID1][pos].vbFinished.push_back(false);
                    vvSETmp[ID1][pos].vpbFinished.push_back(make_pair(false, false));
                    vector<int> v1, v2;
                    v1.push_back(Neigh[i].second.first);
                    v2.push_back(Neigh[j].second.first);
                    vvSETmp[ID1][pos].vpvWDistance.push_back(make_pair(v1, v2));
                    set<pair<int, int>> initialSet;
                    initialSet.insert({0, 0});
                    vvSETmp[ID1][pos].visited.push_back(initialSet);
//                    cout << "ID1: " << ID1 << " ID2: " << ID2 << " wNum: " << vvSETmp[ID1][pos].wNum << " vW: "<< vvSETmp[ID1][pos].vW[vvSETmp[ID1][pos].vW.size()-1] << " vvSETmp[ID1][pos].umWPos[x]: " << vvSETmp[ID1][pos].umWPos[x] << "  dTmp: " << dTmp << endl;
                }
                else
                {
                    //		SCconNodes[make_pair(ID2, ID1)].push_back(x);
                    //		SCconNodes[make_pair(ID1, ID2)].push_back(x);
                    if(vmSEPosTmp[ID2].find(ID1) == vmSEPosTmp[ID2].end())
                    {
                        seEnum se;
                        se.ID1 = ID2;
                        se.ID2 = ID1;
                        se.wNum = 0;
                        //se.vK.push_back(dTmp);
                        vvSETmp[ID2].push_back(se);
                        vmSEPosTmp[ID2].insert(make_pair(ID1, vvSETmp[ID2].size()-1));
                    }
                    int pos = vmSEPosTmp[ID2][ID1];
//                    cout << "position: " << pos << endl;
                    vvSETmp[ID2][pos].wNum++;
                    vvSETmp[ID2][pos].vW.push_back(x);
                    vvSETmp[ID2][pos].umWPos[x] = vvSETmp[ID2][pos].wNum-1;
                    vvSETmp[ID2][pos].vDistance.push_back(dTmp);
                    vvSETmp[ID2][pos].vbFinished.push_back(false);
                    vvSETmp[ID2][pos].vpbFinished.push_back(make_pair(false, false));
                    vector<int> v1, v2;
                    v1.push_back(Neigh[i].second.first);
                    v2.push_back(Neigh[j].second.first);
                    vvSETmp[ID2][pos].vpvWDistance.push_back(make_pair(v2, v1));
                    set<pair<int, int>> initialSet;
                    initialSet.insert({0, 0});
                    vvSETmp[ID2][pos].visited.push_back(initialSet);
//                    cout << "ID2: " << ID2 << " ID1: " << ID1 << " wNum: " << vvSETmp[ID2][pos].wNum << endl;
//                    cout << " vvSETmp[ID2][pos].visited: " << vvSETmp[ID2][pos].visited.size() << endl;
                }
            }
        }
    }
    NodeOrder.assign(nodeNum,-1);
    for(int k=0; k < (int)vNodeOrder.size();k++)
        NodeOrder[vNodeOrder[k]] = k;

    //Reorganize the superedge orders
    GlobalNodeOrder = NodeOrder;
    cout << vNodeOrder.size() << endl;
    for(int k=0; k < (int)vNodeOrder.size();k++)
    {
        int ID = vNodeOrder[k];
//        cout << "k: " << k << " ID: " << ID << endl;
        int idTmp;
        //Send the se to the higher v
        for(auto& ise: vvSETmp[ID])
        {
//            cout << ise.ID1 << " " << ise.ID2 << endl;
            if(NodeOrder[ise.ID1] < NodeOrder[ise.ID2])
            {
                idTmp = ise.ID1;
                ise.ID1 = ise.ID2;
                ise.ID2 = idTmp;
                vector<int> vTmp;
                for(auto& ip : ise.vpvWDistance)
                {
                    vTmp = ip.first;
                    ip.first = ip.second;
                    ip.second = vTmp;
                }

            }
            //When a SE is also an edge
            //The supportive node is ID2
            auto it = vmEdge[ise.ID1].find(ise.ID2);
            if(it != vmEdge[ise.ID1].end())
            {
                ise.vW.push_back(ise.ID2);
                ise.wNum++;
                ise.vDistance.push_back((*it).second);
                ise.umWPos[ise.ID2] = ise.vW.size()-1;
                ise.vbFinished.push_back(true);
                ise.vpbFinished.push_back(make_pair(true, true));
                vector<int> v1, v2;
                v1.push_back((*it).second);
                v2.push_back(0);
                ise.vpvWDistance.push_back(make_pair(v1, v2));
            }
            vvSE[ise.ID1].push_back(ise);
        }

        //Organize all the se with ID as the highest v
        sort(vvSE[ID].begin(), vvSE[ID].end(), seCompare);
        for(int i = 0; i<(int)vvSE[ID].size(); i++){
//            cout << "ID1: " << vvSE[ID][i].ID1 << " ID2: " << vvSE[ID][i].ID2 << endl;
            vmSEPos[ID].insert(make_pair(vvSE[ID][i].ID2, i));
        }
    }

    cout<<"Finish Contraction"<<endl;
}
void Graph::rankCHConstruction(){
    map<int, pair<int,int> > m;
    E.assign(nodeNum,m);
    for(int i = 0; i < (int)adjList.size(); i++)
        for(int j = 0; j < (int)adjList[i].size(); j++)
            E[i].insert(make_pair(adjList[i][j].first,make_pair(adjList[i][j].second, 1)));

    vector<bool> exist(nodeNum, true);
    //If Degree Changed by Contraction
    vector<bool> change(nodeNum, false);

    //Contracted Neighbors
    //Neighbor, <Distance, Supportive Node Number>
    vector<pair<int,pair<int,int> > > vect;
    NeighborCon.assign(nodeNum, vect);

    //Vector-based SE for KSP
    vector<seEnum> vSETmp;
    vector<vector<seEnum> > vvSETmp;
    vvSE.assign(nodeNum, vSETmp);
    vvSETmp.assign(nodeNum, vSETmp);
    unordered_map<int, int> um;
    vector<unordered_map<int, int> > vmSEPosTmp;
    vmSEPos.assign(nodeNum, um);
    vmSEPosTmp.assign(nodeNum, um);


    cout << "Begin to contract" << endl;
    int count = 0;
    for(int i = nodeNum-1; i >-1; i--){
        int x = i;
        vNodeOrder.push_back(x);
        exist[x] = false;

        vector<pair<int, pair<int, int> > > Neigh;
        for(auto& it : E[x]) {
            if(exist[it.first])
                Neigh.push_back(it);
        }
        NeighborCon[x].assign(Neigh.begin(), Neigh.end());
        for(int i=0; i < (int)Neigh.size(); i++)
        {
            int y = Neigh[i].first;
            deleteE(x,y);
            change[y] = true;
        }
        int	ID1, ID2, dTmp;
        for(int i = 0; i<(int)Neigh.size(); i++)
        {
            for(int j=i+1; j<(int)Neigh.size(); j++)
            {
                ID1 = Neigh[i].first;
                ID2 = Neigh[j].first;
                dTmp =  Neigh[i].second.first+Neigh[j].second.first;
                insertE(ID1, ID2, dTmp);

                //Degree Changed
                change[ID1] = true;
                change[ID2] = true;

                //Temporally Store once with smaller ID
                if(ID1 < ID2)
                {
                    //		SCconNodes[make_pair(ID1, ID2)].push_back(x);
                    if(vmSEPosTmp[ID1].find(ID2) == vmSEPosTmp[ID1].end())
                    {
                        seEnum se;
                        se.ID1 = ID1;
                        se.ID2 = ID2;
                        se.wNum = 0;
                        //se.vK.push_back(dTmp);
                        vvSETmp[ID1].push_back(se);
                        vmSEPosTmp[ID1].insert(make_pair(ID2, vvSETmp[ID1].size()-1));
                    }
                    int pos = vmSEPosTmp[ID1][ID2];
//                    cout << "position: " << pos << endl;
                    vvSETmp[ID1][pos].wNum++;
                    vvSETmp[ID1][pos].vW.push_back(x);
                    vvSETmp[ID1][pos].umWPos[x] = vvSETmp[ID1][pos].wNum-1;
                    vvSETmp[ID1][pos].vDistance.push_back(dTmp);
                    vvSETmp[ID1][pos].vbFinished.push_back(false);
                    vvSETmp[ID1][pos].vpbFinished.push_back(make_pair(false, false));
                    vector<int> v1, v2;
                    v1.push_back(Neigh[i].second.first);
                    v2.push_back(Neigh[j].second.first);
                    vvSETmp[ID1][pos].vpvWDistance.push_back(make_pair(v1, v2));
                    set<pair<int, int>> initialSet;
                    initialSet.insert({0, 0});
                    vvSETmp[ID1][pos].visited.push_back(initialSet);
//                    cout << "ID1: " << ID1 << " ID2: " << ID2 << " wNum: " << vvSETmp[ID1][pos].wNum << " vW: "<< vvSETmp[ID1][pos].vW[vvSETmp[ID1][pos].vW.size()-1] << " vvSETmp[ID1][pos].umWPos[x]: " << vvSETmp[ID1][pos].umWPos[x] << "  dTmp: " << dTmp << endl;
                }
                else
                {
                    if(vmSEPosTmp[ID2].find(ID1) == vmSEPosTmp[ID2].end())
                    {
                        seEnum se;
                        se.ID1 = ID2;
                        se.ID2 = ID1;
                        se.wNum = 0;
                        vvSETmp[ID2].push_back(se);
                        vmSEPosTmp[ID2].insert(make_pair(ID1, vvSETmp[ID2].size()-1));
                    }
                    int pos = vmSEPosTmp[ID2][ID1];
                    vvSETmp[ID2][pos].wNum++;
                    vvSETmp[ID2][pos].vW.push_back(x);
                    vvSETmp[ID2][pos].umWPos[x] = vvSETmp[ID2][pos].wNum-1;
                    vvSETmp[ID2][pos].vDistance.push_back(dTmp);
                    vvSETmp[ID2][pos].vbFinished.push_back(false);
                    vvSETmp[ID2][pos].vpbFinished.push_back(make_pair(false, false));
                    vector<int> v1, v2;
                    v1.push_back(Neigh[i].second.first);
                    v2.push_back(Neigh[j].second.first);
                    vvSETmp[ID2][pos].vpvWDistance.push_back(make_pair(v2, v1));
                    set<pair<int, int>> initialSet;
                    initialSet.insert({0, 0});
                    vvSETmp[ID2][pos].visited.push_back(initialSet);
                }
            }
        }

    }

    NodeOrder.assign(nodeNum,-1);
    for(int k=0; k < (int)vNodeOrder.size();k++)
        NodeOrder[vNodeOrder[k]] = k;

    //Reorganize the superedge orders
    GlobalNodeOrder = NodeOrder;
    cout << vNodeOrder.size() << endl;
    for(int k=0; k < (int)vNodeOrder.size();k++)
    {
        int ID = vNodeOrder[k];
        int idTmp;
        //Send the se to the higher v
        for(auto& ise: vvSETmp[ID])
        {
            if(NodeOrder[ise.ID1] < NodeOrder[ise.ID2])
            {
                idTmp = ise.ID1;
                ise.ID1 = ise.ID2;
                ise.ID2 = idTmp;
                vector<int> vTmp;
                for(auto& ip : ise.vpvWDistance)
                {
                    vTmp = ip.first;
                    ip.first = ip.second;
                    ip.second = vTmp;
                }

            }
            //When a SE is also an edge
            //The supportive node is ID2
            auto it = vmEdge[ise.ID1].find(ise.ID2);
            if(it != vmEdge[ise.ID1].end())
            {
                ise.vW.push_back(ise.ID2);
                ise.wNum++;
                ise.vDistance.push_back((*it).second);
                ise.umWPos[ise.ID2] = ise.vW.size()-1;
                ise.vbFinished.push_back(true);
                ise.vpbFinished.push_back(make_pair(true, true));
                vector<int> v1, v2;
                v1.push_back((*it).second);
                v2.push_back(0);
                ise.vpvWDistance.push_back(make_pair(v1, v2));
            }
            vvSE[ise.ID1].push_back(ise);
        }

        //Organize all the se with ID as the highest v
        sort(vvSE[ID].begin(), vvSE[ID].end(), seCompare);
        for(int i = 0; i<(int)vvSE[ID].size(); i++){
//            cout << "ID1: " << vvSE[ID][i].ID1 << " ID2: " << vvSE[ID][i].ID2 << endl;
            vmSEPos[ID].insert(make_pair(vvSE[ID][i].ID2, i));
        }
    }

    cout<<"Finish Contraction"<<endl;

}
void Graph::makeTree()
{
    vector<int> vecemp;
    VidtoTNid.assign(nodeNum,vecemp);

    rank.assign(nodeNum,0);
    int len = vNodeOrder.size()-1;
    heightMax = 0;

    TreeNode rootNode;
    int x = vNodeOrder[len];
//	cout<<"len "<<len<<" , ID "<<x<<endl;
    while(x == -1)//to skip those vertices whose ID is -1
    {
        len--;
        x = vNodeOrder[len];
//		cout<<"-1len "<<len<<" , ID "<<x<<endl;
    }
    rootNode.vert = NeighborCon[x];
    rootNode.uniqueVertex=x;
    rootNode.parentNodeID = -1;
    rootNode.height = 1;
    rank[x] = 0;
    Tree.push_back(rootNode);
//    cout << "Tree size(): " << Tree.size()-1 << " uniqueVertex: " << x << endl;
    len--;

    int nn;
    for(;len >= 0; len--)
    {
        int x = vNodeOrder[len];
//        cout<<"len "<<len<<" , ID "<<x<<endl;
        TreeNode nodeTmp;
        nodeTmp.vert = NeighborCon[x];

        nodeTmp.uniqueVertex = x;
        if(NeighborCon[x].size() == 0)
            cout << x << endl;
//        cout << NeighborCon[x].size() << endl;
        int parentNodeID = match(x, NeighborCon[x]);
//        cout << parentNodeID << endl;
//        cout << Tree.size() << endl;
        Tree[parentNodeID].vChildren.push_back(Tree.size());
        nodeTmp.parentNodeID= parentNodeID;
        nodeTmp.height = Tree[parentNodeID].height + 1;
        nodeTmp.hdepth = Tree[parentNodeID].height + 1;

        for(int i=0; i < (int)NeighborCon[x].size(); i++)
        {
            nn = NeighborCon[x][i].first;
            VidtoTNid[nn].push_back(Tree.size());
            if(Tree[rank[nn]].hdepth < Tree[parentNodeID].height + 1)
                Tree[rank[nn]].hdepth = Tree[parentNodeID].height + 1;
        }

        if(nodeTmp.height > heightMax)
            heightMax = nodeTmp.height;
        rank[x] = Tree.size();
        Tree.push_back(nodeTmp);
//        cout << "Tree size(): " << Tree.size()-1 << " uniqueVertex: " << x << endl;
    }
}
void Graph::makeIndex()
{
    cout << "makeIndex" << endl;
    makeRMQ();
    //initialize
    vector<int> list; //list.clear();
    list.push_back(Tree[0].uniqueVertex);
//    cout << "uniqueVertex: " << Tree[0].uniqueVertex << endl;
    Tree[0].pos.clear();
    Tree[0].pos.push_back(0);

    //Vector-based LE for KSP
    vector<labelEnum> vLETmp;
    vvLE.assign(nodeNum, vLETmp);
    unordered_map<int, int> um;
    vmLEPos.assign(nodeNum, um);

//    cout << "children size: " << Tree[0].vChildren.size() << endl;
    for(int i = 0;i < (int)Tree[0].vChildren.size(); i++)
        makeIndexDFS(Tree[0].vChildren[i], list);


    for(int k=0; k < (int)vNodeOrder.size();k++) {
        int ID = vNodeOrder[k];
        for (int i = 0; i < (int) vvLE[ID].size(); i++) {
//            cout << "ID1: " << vvLE[ID][i].ID1 << " ID2: " << vvLE[ID][i].ID2 << endl;
            vmLEPos[ID].insert(make_pair(vvLE[ID][i].ID2, i));
        }
    }

}

void Graph::makeRMQ()
{
    //EulerSeq.clear();
    toRMQ.assign(nodeNum,0);
    //RMQIndex.clear();
    makeRMQDFS(0, 1);
    RMQIndex.push_back(EulerSeq);

    int m = EulerSeq.size();
    for (int i = 2, k = 1; i < m; i = i * 2, k++)
    {
        vector<int> tmp;
        tmp.assign(m, 0);
        for (int j = 0; j < m - i; j++)
        {
            int x = RMQIndex[k-1][j];
            int y = RMQIndex[k-1][j + i/2];
            if (Tree[x].height < Tree[y].height)
                tmp[j] = x;
            else
                tmp[j] = y;
        }
        RMQIndex.push_back(tmp);
    }
}

void Graph::makeRMQDFS(int p, int height)
{
    toRMQ[p] = EulerSeq.size();
    EulerSeq.push_back(p);
    for (int i = 0; i < (int)Tree[p].vChildren.size(); i++)
    {
        makeRMQDFS(Tree[p].vChildren[i], height+1);
        EulerSeq.push_back(p);
    }
}

void Graph::makeIndexDFS(int p, vector<int>& list)
{
    //initialize
//    cout << "p: " << p << endl;
    int NeiNum = Tree[p].vert.size();
//    cout << "NeiNum size: " << NeiNum << endl;
    Tree[p].pos.assign(NeiNum+1, 0);
    Tree[p].pos2.assign(NeiNum+1, 0);
    Tree[p].dis.assign(list.size(), INF);
//    Tree[p].dis.assign(nodeNum, INF);
    Tree[p].cnt.assign(list.size(), 0);
//    Tree[p].cnt.assign(nodeNum, INF);
    Tree[p].FN.assign(list.size(), true);

    int nodeID = Tree[p].uniqueVertex;
//    cout << "nodeID: " << nodeID << endl;
    labelEnum le;
    vector<labelEnum> vLE(list.size(), le);
    for(int j=0; j < (int)list.size(); j++)
    {
        vLE[j].aNum = (int)list.size();
//        cout << "list size: " << list.size() << endl;
        vLE[j].ID1 = nodeID;
        vLE[j].ID2 = list[j];
//        cout << "ID1: " << vLE[j].ID1 << " ID2: " << vLE[j].ID2 << endl;
    }

    //pos
    //map<int,Nei> Nmap; Nmap.clear();//shortcut infor ordered by the pos ID
    for(int i=0; i < NeiNum; i++)
    {
//        cout << "Nei: " << Tree[p].vert[i].first << endl;
        for(int j=0; j < (int)list.size(); j++)
        {
//            cout << "list[j]: " << list[j] << endl;
            if(Tree[p].vert[i].first==list[j])
            {
                int p1 = rank[list[j]];
                Tree[p].pos[i]= j;
                Tree[p].pos2[i] = p1;
//                cout << "Tree[p]: " << p << " i: " << i << " p1: " << p1 << " list[j]: " << list[j] << endl;
                Tree[p].dis[j]=Tree[p].vert[i].second.first;
                Tree[p].cnt[j]=1;
                break;
            }
        }
    }
    Tree[p].pos[NeiNum] = list.size();
    Tree[p].pos2[NeiNum] = p;
//    cout << "NeiNum: " << NeiNum << " list size:" << Tree[p].pos[NeiNum] << endl;


    //dis
    for(int i=0; i < NeiNum; i++)
    {
        int x = Tree[p].vert[i].first;
        int disvb = Tree[p].vert[i].second.first;
        int k = Tree[p].pos[i];//the kth ancestor is x
//        cout << "x: " << x << " k: " << k << endl;

        for(int j=0; j < (int)list.size(); j++)
        {
            int y = list[j];//the jth ancestor is y
//            cout << "y: " << y << " j: " << j << endl;
            int z;//the distance from x to y
            if(k != j)
            {
                if(k < j)
                    z = Tree[rank[y]].dis[k];
                else if(k > j)
                    z = Tree[rank[x]].dis[j];

                if(Tree[p].dis[j] > z + disvb)
                {
                    Tree[p].dis[j] = z + disvb;
                    Tree[p].FN[j] = false;
                    Tree[p].cnt[j] = 1;
                }
                else if(Tree[p].dis[j]==z+disvb)
                    Tree[p].cnt[j] += 1;
            }

            //Init subA
            subA sa;
            if(k != j)
            {
                sa.d = z + disvb;
                sa.d1 = z;
                sa.d2 = disvb;
                sa.a = x;
                sa.bSE = false;
            }
            else
            {
                //tree ID to nodeID
                sa.se = &vvSE[p][i];
//                sa.se = make_unique<seEnum>(vvSE[p][i]);
//                sa.se = boost::shared_ptr<seEnum>(&vvSE[p][i]);
//				sa.se = &vvSE[Tree[p].uniqueVertex][i];
//                cout << "se ID1: " << sa.se->ID1 << " se ID2: " << sa.se->ID2 << endl;
                sa.a = x;
                sa.bSE = true;
                sa.d = disvb;
                z = 0;
            }
            sa.k1 = 0;
            sa.k2 = 0;
            vLE[j].Q.push(sa);
            vLE[j].ID1 = Tree[p].uniqueVertex;
            vLE[j].ID2 = y;
            vLE[j].vDistance.push_back(sa.d);
            vLE[j].vA.push_back(x);
            vLE[j].umAPos.insert(make_pair(x, i));
            vLE[j].vbFinished.push_back(false);
            vLE[j].vpbFinished.push_back(make_pair(false,false));
            vector<int> v1, v2;
            v1.push_back(disvb);
            v2.push_back(z);
            vLE[j].vpvADistance.push_back(make_pair(v1, v2));
//            vLE[j].vpNode.push_back(make_pair(x1, x2));
            set<pair<int, int>> initialSet;
            initialSet.insert({0, 0});
            vLE[j].visited.push_back(initialSet);
            vLE[j].vR.push_back(make_pair(make_pair(0,0), make_pair(0,0)));
            vLE[j].bFinished = false;
            vmLFinsihed[Tree[p].uniqueVertex][y] = false;

//            cout << "i:" << i << " x: " << x << " j: " << j << " y: " << y << " ID1: " << vLE[j].ID1 << " ID2: " << vLE[j].ID2 << " vA: " << x << " distance: " << sa.d << " Q size: " << vLE[j].Q.size() << endl;
        }
    }
//    cout << "p end: " << p << endl;
    //changed
//    vvLE[p] = vLE;
    vvLE[Tree[p].uniqueVertex] = vLE;

    //nested loop
    list.push_back(Tree[p].uniqueVertex);
    /* cout << "List!!!" << endl;
     for(int m = 0; m < list.size(); m++){
         cout << list[m] << " " ;
     }
     cout << endl;*/
    for(int i=0;i < (int)Tree[p].vChildren.size(); i++)
        makeIndexDFS(Tree[p].vChildren[i], list);

    list.pop_back();
}

void Graph::searchChild(int p, int a, int b, int& mind, int& minc, int miniH)
{
    int ssw,wtt; int ca=0;
    for(int j=0; j < (int)Tree[p].vert.size(); j++)
    {
        if(Tree[p].vert[j].first == a)
        {
            ssw = Tree[p].vert[j].second.first;
            ca += 1;
        }
        if(Tree[p].vert[j].first == b)
        {
            wtt = Tree[p].vert[j].second.first;
            ca += 1;
        }
    }

    if(ca == 2)
    {
        if(ssw + wtt < mind)
        {
            mind = ssw + wtt;
            minc = 1;
        }
        else if(ssw + wtt == mind)
            minc += 1;

        if(Tree[p].height < miniH)
            for(int i=0; i < (int)Tree[p].vChildren.size(); i++)
                searchChild(Tree[p].vChildren[i], a, b, mind, minc, miniH);
    }
}

void Graph::deleteE(int u, int v)
{
    if(E[u].find(v) != E[u].end())
    {
        E[u].erase(E[u].find(v));
        //use rank close
        DD[u]--;
    }

    if(E[v].find(u) != E[v].end())
    {
        E[v].erase(E[v].find(u));
        //use rank close
        DD[v]--;
    }
}

void Graph::deleteEorder(int u, int v)
{
    if(E[u].find(v) != E[u].end())
        E[u].erase(E[u].find(v));

    if(E[v].find(u)!=E[v].end())
        E[v].erase(E[v].find(u));
}

void Graph::insertE(int u, int v, int w)
{
    if(E[u].find(v) == E[u].end())
    {
        E[u].insert(make_pair(v,make_pair(w,1)));
        //use rank close
        DD[u]++;
        DD2[u]++;
    }
    else
    {
        if(E[u][v].first > w)
            E[u][v] = make_pair(w, 1);
        else if(E[u][v].first == w)
            E[u][v].second++;
    }

    if(E[v].find(u) == E[v].end())
    {
        E[v].insert(make_pair(u,make_pair(w,1)));
        //use rank close
        DD[v]++;
        DD2[v]++;
    }
    else
    {
        if(E[v][u].first>w)
            E[v][u] = make_pair(w, 1);
        else if(E[v][u].first == w)
            E[v][u].second++;
    }
}

void Graph::insertEorder(int u, int v, int w)
{
    if(E[u].find(v) == E[u].end())
        E[u].insert(make_pair(v,make_pair(w,1)));
    else
    {
        if(E[u][v].first > w)
            E[u][v] = make_pair(w, 1);
        else if(E[u][v].first == w)
            E[u][v].second++;
    }

    if(E[v].find(u) == E[v].end())
        E[v].insert(make_pair(u,make_pair(w,1)));
    else
    {
        if(E[v][u].first > w)
            E[v][u] = make_pair(w, 1);
        else if(E[v][u].first == w)
            E[v][u].second++;
    }
}

void Graph::readTD(string filename)
{
}

void Graph::writeTD(string filename)
{
    //Write nodeOrder


    //Write NeighborCon

    //Write SCconNodes

    //Write Tree

    //Write Label

}

//Find the lowest TreeNode
int Graph::match(int x, vector<pair<int,pair<int,int>>> &vert)
{
//    cout << "x: " << x << endl;
    int nearest = vert[0].first;
    for(int i=1; i< (int)vert.size(); i++)
    {
        if(rank[vert[i].first] > rank[nearest])
            nearest = vert[i].first;
    }
//    cout << "nearest: " << nearest << endl;
    int p = rank[nearest];
    return p;
}

int Graph::QueryH2H(int ID1,int ID2)
{
    if(ID1 == ID2)
        return 0;

    if(NodeOrder[ID1]==-1 || NodeOrder[ID2]==-1)
        return INF;

    int r1 = rank[ID1];
    int r2 = rank[ID2];
    int LCA = LCAQuery(r1,r2);

    if(LCA == r1)
        return Tree[r2].dis[Tree[r1].pos.back()];
    else if(LCA == r2)
        return Tree[r1].dis[Tree[r2].pos.back()];
    else
    {
        int tmp = INF;
        for(int i=0; i < Tree[LCA].pos.size(); i++)
        {
            if(tmp>Tree[r1].dis[Tree[LCA].pos[i]] + Tree[r2].dis[Tree[LCA].pos[i]])
                tmp=Tree[r1].dis[Tree[LCA].pos[i]]+Tree[r2].dis[Tree[LCA].pos[i]];
        }
        return tmp;
    }
}

int Graph::LCAQuery(int _p, int _q){
    int p = toRMQ[_p], q = toRMQ[_q];
    if (p > q)
    {
        int x = p;
        p = q;
        q = x;
    }
    int len = q - p + 1;
    int i = 1, k = 0;
    while (i * 2 < len)
    {
        i *= 2;
        k++;
    }
    q = q - i + 1;
    if (Tree[RMQIndex[k][p]].height < Tree[RMQIndex[k][q]].height)
        return RMQIndex[k][p];
    else
        return RMQIndex[k][q];
}

void Graph::findLeaves(vector<int>& vLeaves)
{
    for(auto& tn : Tree)
    {
        if(tn.vChildren.size()== 0)
        {
            if(adjList[tn.uniqueVertex].size() == 2)
            {
                cout << tn.uniqueVertex << "\t" << tn.height << endl;
                vLeaves.push_back(tn.uniqueVertex);
            }
        }
    }
}

int Graph::TDKSP(int ID1, int ID2, int k)
{
    //MultiHop S
    if(ID1 == ID2)
        return 0;

    if(NodeOrder[ID1]==-1 || NodeOrder[ID2]==-1)
        return INF;

    int r1 = rank[ID1];
    int r2 = rank[ID2];
    cout << "r1: " << r1 << " r2: " << r2 << endl;
    int LCA = LCAQuery(r1,r2);
    cout << "LCA: " << LCA << endl;
    LCA = r1;

    cout << "Vertex of " << ID1 << ":" << Tree[LCA].uniqueVertex << endl;
    //	for(auto &v : Tree[LCA].vert)
    /*for(auto &v : Tree[LCA].vert)
        cout << v.first << "\t";
    cout << endl;*/

    vvSEQuery = vvSE;

    //key: distance
    //value: Hop pos
    benchmark::heap<2, int, int> qHopPath(Tree[LCA].vert.size() + 1);
    for(int i = 0; i < Tree[LCA].vert.size(); i++)
    {
        int d = QueryH2H(ID2, Tree[LCA].vert[i].first) + Tree[r1].dis[Tree[LCA].pos[i]];
        cout << "ID2 to vert i: " << QueryH2H(ID2, Tree[LCA].vert[i].first)  << " Tree[r1].dis[Tree[LCA].pos[i]]: " << Tree[r1].dis[Tree[LCA].pos[i]] << endl;
        cout << Tree[LCA].vert[i].first << "\t" << d << endl;
        //		int d = Tree[r1].dis[Tree[LCA].pos[i]] + Tree[r2].dis[Tree[LCA].pos[i]];
        //		in1 d = Tree[r1].dis[Tree[LCA].pos[i]] + Tree[r2].dis[Tree[LCA].pos[i]];
        qHopPath.update(i, d);
    }

    int topDistance;
    int topHop;
    while(!qHopPath.empty())
    {
        qHopPath.extract_min(topHop, topDistance);
        cout << topDistance << "\t" << topHop << endl;
    }

    return k;
}

void Graph::vvSEInit()
{
    cout << "vvSEInit" << endl;
    for(int i = 0; i < vNodeOrder.size(); i++)
    {
        for(auto& se: vvSE[vNodeOrder[i]])
        {
            bool bFinished = true;
            for(int k = 0; k < se.wNum; k++)
            {
                int w = se.vW[k];
                //	se.Q.push(make_pair(w, se.vDistance[k]));
                subW sw;
                sw.d = se.vDistance[k];
                /*sw.d1 = se.vpvWDistance[se.umWPos[w]].first[0];
                sw.d2 = se.vpvWDistance[se.umWPos[w]].second[0];*/
                sw.w = w;
                sw.bW = true;
                sw.k1 = 0;
                sw.k2 = 0;
                se.Q.push(sw);
//				se.Q.push(make_pair(se.vDistance[k], w));
                if(w == se.ID2)
                    se.vR.push_back(make_pair(make_pair(-1,-1), make_pair(-1,-1)));
                else
                    se.vR.push_back(make_pair(make_pair(0,0), make_pair(0,0)));
                bool b1 = false;
                bool b2 = false;

                //cout << se.ID1 << "\t" << se.ID2 << "\t" << w << endl;//
                //If this se is an edge, then is won't appera in vvSE
                //Uncomment for faster enum.
                //Comment for complete path retrieval
/*
				if(vmSEPos[se.ID1].find(w) == vmSEPos[se.ID1].end())
					b1 = true;
				else if(vvSE[se.ID1][vmSEPos[se.ID1][w]].wNum == 1 && vvSE[se.ID1][vmSEPos[se.ID1][w]].vpbFinished[0].first && vvSE[se.ID1][vmSEPos[se.ID1][w]].vpbFinished[0].second)
					b1 = true;
				else
					bFinished = false;

				if(vmSEPos[se.ID2].find(w) == vmSEPos[se.ID2].end())
					b2 = true;
				else if(vvSE[se.ID2][vmSEPos[se.ID2][w]].wNum == 1 && vvSE[se.ID2][vmSEPos[se.ID2][w]].vpbFinished[0].first && vvSE[se.ID2][vmSEPos[se.ID2][w]].vpbFinished[0].second)
					b2 = true;
				else
					bFinished = false;

				se.vpbFinished.push_back(make_pair(b1, b2));
				*/
                se.vpbFinished.push_back(make_pair(false, false));
            }
/*			se.bFinished = bFinished;
			vmEFinsihed[se.ID1][se.ID2] = bFinished;
			if(se.wNum > 1)
			{
				vmEFinsihed[se.ID1][se.ID2] = false;
				se.bFinished = false;
			}
			*/
            se.bFinished = false;
            vmEFinsihed[se.ID1][se.ID2] = false;
        }
    }

}

void Graph::vvLEInit()
{
    cout << "vvLEInit" << endl;
    for(int i = 0; i < vNodeOrder.size(); i++)
    {
        for(auto& le: vvLE[vNodeOrder[i]]){
//            cout << "ID1: " << le.ID1 << " ID2: " << le.ID2 << endl;
        }

    }

}

//With Loop
int Graph::caveKSP(int ID1, int ID2, int k, int d1, int d2, int dTop, bool bLeft, bool bForward)
{
    std::chrono::high_resolution_clock::time_point t1;
    std::chrono::high_resolution_clock::time_point t2;
    std::chrono::duration<double> time_span;
    t1 = std::chrono::high_resolution_clock::now();
    /*if(bForward){
        if(bPrint == true){
            cout << endl << "Valley " << ID1 <<"\t" << ID2 << "\ttrue\t" << k << endl;
        }
    }
    else
    {
        int IDTmp;
        IDTmp = ID1;
        ID1 = ID2;
        ID2 = IDTmp;
        if(bPrint == true){
            cout << endl << "Valley " << ID1 <<"\t" << ID2 << "\tfalse\t" << k << endl;
        }
    }*/
//    cout << endl << "Valley " << ID1 <<"\t" << ID2 << "\tfalse\t" << k << endl;
    if(bPrint == true){
        cout << endl << "Valley " << ID1 <<"\t" << ID2 << "\tfalse\t" << k << endl;
    }
    int o1 = NodeOrder[ID1];
    int o2 = NodeOrder[ID2];

    if(o1 < o2){
        int tmp = ID1;
        ID1 = ID2;
        ID2 = tmp;
    }
//    cout << endl << "Valley " << ID1 <<" " << ID2 << "false " << k << endl;
    auto it = vmSEPos[ID1].find(ID2); // check if it is a super edge
    if(it == vmSEPos[ID1].end())
    {
        if(bPrint == true){
            cout << "No SE. Edge Distance is " << vmEdge[ID1][ID2] << endl;
        }
        return -1;
    }

    if(bPrint == true){
        cout << "wNum:" << vvSEQuery[ID1][(*it).second].wNum << endl; // super edge numbers
        for(auto& id:vvSEQuery[ID1][(*it).second].vDistance)
            cout << id <<"\t";
        cout << endl;

        cout << "vk Size:" << vvSEQuery[ID1][(*it).second].vK.size() << endl;//For path retrievalvK.size() << endl;
        for(auto& id:vvSEQuery[ID1][(*it).second].vK)
            cout << id <<"\t";
        cout << endl;
    }

    if(vmEFinsihedQuery[ID1][ID2])
    {
        if(bPrint == true){
            cout << "Finished Enumeration!" << endl;
        }
        if(k < vvSEQuery[ID1][(*it).second].vK.size())
            return vvSEQuery[ID1][(*it).second].vK[k];
        else
            return -1;
    }

    if(k < vvSEQuery[ID1][(*it).second].vK.size())
    {
        if(bPrint == true){
            cout << "A:" <<  k << "\t" <<  vvSEQuery[ID1][(*it).second].vK.size() << "\t" << vvSEQuery[ID1][(*it).second].vKW.size() << endl;
        }
        return vvSEQuery[ID1][(*it).second].vK[k];
    }
    int dNext;
    while(k >= vvSEQuery[ID1][(*it).second].vK.size())
    {
        t2 = std::chrono::high_resolution_clock::now();
        time_span = std::chrono::duration_cast<std::chrono::duration<double> >(t2 - t1);
        if(time_span.count() > 200){
            return -1;
        }
        /*cout << "k: " << k << endl;
        cout << "vK size:" << vvSEQuery[ID1][(*it).second].vK.size() << endl;
        cout << "new k: " << " ID1: " << ID1 << " ID2: " << ID2 << endl;*/
        if(bPrint == true){
            cout << "k: " << k << endl;
            cout << "vK size:" << vvSEQuery[ID1][(*it).second].vK.size() << endl;
        }
        seEnum& se = vvSEQuery[ID1][(*it).second];
        if(se.Q.empty())
        {
            se.bFinished = true;
            vmEFinsihedQuery[ID1][ID2] = true;
        }
        if(vmEFinsihedQuery[ID1][ID2])
        {
            if(bPrint == true){
                cout << "se Has finished Update" << endl;
            }
            if(se.Q.empty())
            {
                se.bFinished = true;
                vmEFinsihedQuery[ID1][ID2] = true;
            }
            if(k < vvSEQuery[ID1][(*it).second].vK.size())
                return vvSEQuery[ID1][(*it).second].vK[k];
            else
                return -1;
        }

        subW pTop = se.Q.top();
        if(bPrint == true){
            cout << "Generate New dNext" << endl;
            for(auto& id:se.vDistance)
                cout << id <<"\t";
            cout << endl;
            cout << "Qsize:" << se.Q.size() << endl;
        }

        se.Q.pop();
        int w = pTop.w;
        int wPos = se.umWPos[w];
        dNext = pTop.d;
        if(bPrint == true){
            cout << "Top Dist:" << dNext << " via Node: " << w << endl;
            cout << "Qsize:" << se.Q.size() << endl;
        }
        /* cout << "Top Dist:" << dNext << " via Node: " << w << endl;
         cout << "Qsize:" << se.Q.size() << endl;*/
        subW pTop2 = se.Q.top();
        int dTop2 = pTop2.d;
//        cout << "Next Top Dist:" << dNext22 << " Next via Node: " << w2 << endl;

        se.vK.push_back(dNext);
        se.vKW.push_back(w);
//        cout << "pTop.k1: " << pTop.k1 << " pTop.k2: " << pTop2.k2 << endl;
        se.sebLoop.push_back(false);

        vector<int> x1;
        if(w != ID2 ){
            x1.push_back(w);
        }
        se.vNode.push_back(x1);

       /* string s1;
        if(w != ID2 ){
            s1 = to_string(w)+"-";
        }
        se.vPathstr.push_back(s1);*/

        vector<int> x2;
        x2.push_back(rank[ID1]);
        if(w != ID2 ){
            x2.push_back(rank[w]);
        }
        sort(x2.begin(), x2.end());
        se.rankSet1.push_back(x2);

        vector<int> x3;
        x3.push_back(rank[ID2]);
        if(w != ID2 ){
            x3.push_back(rank[w]);
        }
        sort(x3.begin(), x3.end());
        se.rankSet2.push_back(x3);



        vector<int> x4;
        x4.push_back(rank[ID1]);
        x4.push_back(rank[ID2]);
        if(w != ID2 ){
            x4.push_back(rank[w]);
        }
        sort(x4.begin(), x4.end());
        se.vRank.push_back(x4);
        se.pathTotal.push_back(rank[ID1]+rank[ID2]+rank[w]);

//        bool bLoop = false;

        if(w == 0){
            if(bPrint == true){
                cout << "VKW0!!" << endl;
            }
        }
        se.vSubK.push_back(make_pair(pTop.k1, pTop.k2));

        if(bPrint == true){
            cout << "k1: " << pTop.k1 << " k2: " << pTop.k2 << endl;
            cout << "new vk Size:" << vvSEQuery[ID1][(*it).second].vK.size() << endl;
            cout << "vKW size: " << vvSEQuery[ID1][(*it).second].vKW.size() << endl;
            for(auto& id:vvSEQuery[ID1][(*it).second].vK)
                cout << id <<"\t";
            cout << endl;
        }

        if(w == ID2)
        {
            if(bPrint == true){
                cout << " w is ID2" << endl;
            }
            if(se.Q.empty())
            {
                se.bFinished = true;
                vmEFinsihedQuery[ID1][ID2] = true;
            }
            continue;
        }


        auto& pvDistance = se.vpvWDistance[wPos];
        int d11 = pvDistance.first[pTop.k1];
        int d22 = pvDistance.second[pTop.k2];
        if(bPrint == true){
            cout << "valley pvDistance first size: " << pvDistance.first.size() << endl;
            for (int i = 0; i < pvDistance.first.size(); ++i) {
                cout << pvDistance.first[i] << " ";
            }
            cout << endl;

            cout << "valley pvDistance second size: " << pvDistance.second.size() << endl;
            for (int i = 0; i < pvDistance.second.size(); ++i) {
                cout << pvDistance.second[i] << " ";
            }
            cout << endl;
            for(int i = 0; i < se.vNode.size(); i++){
                cout << "ii: " << i << endl;
                for(const int& element : se.vNode[i]){
                    cout << element << " ";
                }
                cout << endl;
            }
            cout << "vmEFinsihedQuery: " << vmEFinsihedQuery[ID1][w] << endl;
            cout << "vmEFinishedQuery2: " << vmEFinsihedQuery[ID2][w] << endl;
        }



        //If is ID1,w is a se
        bool b1 = true;
        bool b2 = true;
        auto it1 = vmSEPos[ID1].find(w);
        auto it2 = vmSEPos[ID2].find(w);
        int d1Next, d2Next;
        if(it1 == vmSEPos[ID1].end())
        {
            b1 = false;
            d1Next = vmEdge[ID1][w];
        }
        if(it2 == vmSEPos[ID2].end())
        {
            b2 = false;
            d2Next = vmEdge[ID2][w];
        }
        if(!b1 && !b2)
        {
            if(bPrint == true){
                cout << "e1 and e2 are edges" << endl;
            }
            continue;
        }
//        se.vR.push_back(make_pair(make_pair(vvSEQuery[ID1][(*it1).second].vSubK[pTop.k1].first,vvSEQuery[ID1][(*it1).second].vSubK[pTop.k1].second), make_pair(vvSEQuery[ID2][(*it2).second].vSubK[pTop.k2].first,vvSEQuery[ID2][(*it2).second].vSubK[pTop.k2].first)));
        //Update w1 when b2 is an edge
        //b1 and b2 are both edges are covered by vbFinished
        if(b1 && !b2)
        {
            if(bPrint == true){
                cout << "AA" << endl;
            }
            int p11 = pTop.k1;
            if(p11 == 0){
                caveKSP(ID1, w, 0, INF, INF, INF, true, true);
            }
            if(it1 != vmSEPos[ID1].end()){
                se.vNode[k].insert(se.vNode[k].begin(),vvSEQuery[ID1][(*it1).second].vNode[p11].begin(),vvSEQuery[ID1][(*it1).second].vNode[p11].end());
                /*for(int i = 0; i < vvSEQuery[ID1][(*it1).second].vNode[p11].size(); i++){
                    se.rankSet1[k].push_back(rank[vvSEQuery[ID1][(*it1).second].vNode[p11][i]]);
                }*/
//                se.vPathstr[k].insert(0,vvSEQuery[ID1][(*it1).second].vPathstr[p11]);
                se.rankSet1[k] = vvSEQuery[ID1][(*it1).second].vRank[p11];
                if(vvSEQuery[ID1][(*it1).second].sebLoop[p11] == true){
                    se.sebLoop[k] = true;
                }
            }



            auto it111 = se.rankSet1[k].begin();
            auto it222 = se.rankSet2[k].begin();
            bool wAdded = false;
            se.vRank[k].clear();
            se.pathTotal[k] = 0;
            while (it111 != se.rankSet1[k].end() && it222 != se.rankSet2[k].end()) {
                if (*it111 == *it222) {
                    if(*it111 == rank[w] && !wAdded){
                        wAdded = true;
                        se.vRank[k].push_back(*it111);
                        se.pathTotal[k] += *it111;
                        ++it111;
                        ++it222;
                    }
                    else{
                        se.sebLoop[k] = true;
                        break;
                    }
                } else if (*it111 < *it222) {
                    se.vRank[k].push_back(*it111);
                    se.pathTotal[k] += *it111;
                    ++it111;
                } else {
                    se.vRank[k].push_back(*it222);
                    se.pathTotal[k] += *it222;
                    ++it222;
                }
            }

            while (it111 != se.rankSet1[k].end()) {
                se.vRank[k].push_back(*it111);
                se.pathTotal[k] += *it111;
                ++it111;
            }

            while (it222 != se.rankSet2[k].end()) {
                se.vRank[k].push_back(*it222);
                se.pathTotal[k] += *it222;
                ++it222;
            }



            if(se.sebLoop[k] == true){
//                cout << "se1 bLoop is true" << endl;
                if(bLeft == true){
                    if(dNext + d2 <= dTop2){
                        se.vK.pop_back();
                        se.vKW.pop_back();
                        se.vSubK.pop_back();
                        se.vNode.pop_back();
                        se.sebLoop.pop_back();
                        se.vRank.pop_back();
                        se.pathTotal.pop_back();
                        se.rankSet1.pop_back();
                        se.rankSet2.pop_back();
                    }
                    else{
//                        cout << "True!!!" << endl;
                    }
                }
                else{
                    if( d1 +dNext <= dTop2){
                        se.vK.pop_back();
                        se.vKW.pop_back();
                        se.vSubK.pop_back();
                        se.vNode.pop_back();
                        se.sebLoop.pop_back();
                        se.vRank.pop_back();
                        se.pathTotal.pop_back();
                        se.rankSet1.pop_back();
                        se.rankSet2.pop_back();
                    }
                    else{
//                        cout << "True!!!" << endl;
                    }

                }
            }

            d1Next = caveKSP(ID1, w, p11+1, d11, d22, dTop2, true, true);

            if(d1Next == -1)
            {
                vmEFinsihedQuery[ID1][w] = true;
                se.vpbFinished[wPos].first = true;
                se.vbFinished[wPos] = true;
                if(se.Q.empty())
                {
                    se.bFinished = true;
                    vmEFinsihedQuery[ID1][ID2] = true;
                }
                if(k < vvSEQuery[ID1][(*it).second].vK.size())
                    return vvSEQuery[ID1][(*it).second].vK[k];
                else
                    continue;
            }
            if(bPrint == true){
                cout << "New Dist:" << ID1 << "\t" << w << "\t" << k << "\tDist:" << d1Next << "\t" << d2Next  << "\t" << d1Next + d2Next << endl;
            }

            if(d1Next != -1){
//                cout << " ID1: " << ID1 << " w: " << w << " ID2: " << ID2 << " vvSEQuery[ID1][(*it1).second].vK.size(): " << vvSEQuery[ID1][(*it1).second].vK.size() << " se.vK.size(): " << se.vK.size() << endl;
//                cout << "Q size: " << se.Q.size() << endl;
                if(it1 != vmSEPos[ID1].end())
                {
                    if(vvSEQuery[ID1][(*it1).second].vK.size() > se.vK.size()*100){
//                    cout << "Here!" << endl;
                        continue;
                    }
                }
            }

//			se.vR[wPos].first.first++;
            subW sw;
            sw.d = d1Next + d2Next;
            sw.d1 = d1Next;
            sw.d2 = d2Next;
            sw.w = w;
            sw.bW = true;
//			sw.k1 = se.vR[wPos].first.first;
            sw.k1 = p11+1;
            sw.k2 = -1;
            se.Q.push(sw);
            if(sw.d < se.vK[se.vK.size()-1]){
                cout << "11: se!" << endl;
                for(int i = 0; i < pvDistance.first.size(); i++){
                    cout << pvDistance.first[i] << " ";
                }
                cout << endl;
            }
        }
        else if(!b1 && b2) //b1 is an edge
        {
            if(bPrint == true){
                cout << "BB" << endl;
            }
            int p12 = pTop.k2;
            if(p12 == 0){
                caveKSP(ID2, w, 0, INF, INF, INF, false, true);
            }
            if(bPrint == true){
                cout << "p12: " << p12 << endl;
                cout << "ID2: "<< ID2 << " (*it2).second: " << (*it2).second << endl;
                cout << vvSEQuery[ID2][(*it2).second].sebLoop[p12] << endl;
            }
            if(it2 != vmSEPos[ID2].end()){
                for(int i = vvSEQuery[ID2][(*it2).second].vNode[p12].size()-1; i >= 0; i--){
                    se.vNode[k].push_back(vvSEQuery[ID2][(*it2).second].vNode[p12][i]);
//                    se.vPathstr[k] += to_string(vvSEQuery[ID2][(*it2).second].vNode[p12][i]) + "-";
//                    se.rankSet2[k].push_back(rank[vvSEQuery[ID2][(*it2).second].vNode[p12][i]]);
                    if(bPrint == true){
                        cout << vvSEQuery[ID2][(*it2).second].vNode[p12][i] << " ";
                    }
                }
                se.rankSet2[k] = vvSEQuery[ID2][(*it2).second].vRank[p12];
                if(vvSEQuery[ID2][(*it2).second].sebLoop[p12] == true){
                    /*cout << "Bse is true" << endl;
                    for(int i = 0; i <  se.vNode[k].size(); i++){
                        cout << se.vNode[k][i] << " ";
                    }
                    cout << endl;*/
                    se.sebLoop[k] = true;
                }
                if(bPrint == true){
                    cout << endl;
                }
            }



//            bLoop = false;

            auto it111 = se.rankSet1[k].begin();
            auto it222 = se.rankSet2[k].begin();
            bool wAdded = false;
            se.vRank[k].clear();
            se.pathTotal[k] = 0;
            while (it111 != se.rankSet1[k].end() && it222 != se.rankSet2[k].end()) {
                if (*it111 == *it222) {
                    if(*it111 == rank[w] && !wAdded){
                        wAdded = true;
                        se.vRank[k].push_back(*it111);
                        se.pathTotal[k] += *it111;
                        ++it111;
                        ++it222;
                    }
                    else{
                        se.sebLoop[k] = true;
                        break;
                    }
                } else if (*it111 < *it222) {
                    se.vRank[k].push_back(*it111);
                    se.pathTotal[k] += *it111;
                    ++it111;
                } else {
                    se.vRank[k].push_back(*it222);
                    se.pathTotal[k] += *it222;
                    ++it222;
                }
            }
            while (it111 != se.rankSet1[k].end()) {
                se.vRank[k].push_back(*it111);
                ++it111;
            }

            while (it222 != se.rankSet2[k].end()) {
                se.vRank[k].push_back(*it222);
                ++it222;
            }




            if(se.sebLoop[k] == true){
                if(bLeft = true){
                    if(dNext + d2 <= dTop2){
                        se.vK.pop_back();
                        se.vKW.pop_back();
                        se.vSubK.pop_back();
                        se.vNode.pop_back();
                        se.sebLoop.pop_back();
                        se.vRank.pop_back();
                        se.pathTotal.pop_back();
                        se.rankSet1.pop_back();
                        se.rankSet2.pop_back();
                    }
                    else{
//                        cout << "True!!!" << endl;
                    }
                }
                else{
                    if( d1 +dNext <= dTop2){
                        se.vK.pop_back();
                        se.vKW.pop_back();
                        se.vSubK.pop_back();
                        se.vNode.pop_back();
                        se.sebLoop.pop_back();
                        se.vRank.pop_back();
                        se.pathTotal.pop_back();
                        se.rankSet1.pop_back();
                        se.rankSet2.pop_back();
                    }
                    else{
//                        cout << "True!!!" << endl;
                    }
                }
            }

            d2Next = caveKSP(ID2, w, p12+1, d11, d22, dTop2, false, true);

            if(d2Next == -1)
            {
                vmEFinsihedQuery[ID2][w] = true;
                se.vpbFinished[wPos].second = true;
                se.vbFinished[wPos] = true;
                if(se.Q.empty())
                {
                    se.bFinished = true;
                    vmEFinsihedQuery[ID1][ID2] = true;
                }
                if(k < vvSEQuery[ID1][(*it).second].vK.size())
                    return vvSEQuery[ID1][(*it).second].vK[k];
                else
                    continue;
            }
            if(bPrint == true){
                cout << "New Dist:" << ID1 << "\t" << w << "\t" << k << "\tDist:" << d1Next << "\t" << d2Next  << "\t" << d1Next + d2Next << endl;
            }

            if(d2Next != -1){
                if(it2 != vmSEPos[ID2].end())
                {
                    if(vvSEQuery[ID2][(*it2).second].vK.size() > se.vK.size()*100){
//                    cout << "Here!" << endl;
                        continue;
                    }
                }
//                cout << " ID1: " << ID1 << " w: " << w << " ID2: " << ID2 << " vvSEQuery[ID2][(*it2).second].vK.size(): " << vvSEQuery[ID2][(*it2).second].vK.size() << " se.vK.size(): " << se.vK.size() << endl;
//                cout << "Q size: " << se.Q.size() << endl;
            }

//			se.vR[wPos].first.second++;
            /*se.vR[wPos].second.first = vvSEQuery[ID1][(*it).second].vSubK[p12].first;
            se.vR[wPos].second.second = vvSEQuery[ID1][(*it).second].vSubK[p12].second;*/
            subW sw;
            sw.d = d1Next + d2Next;
            sw.d1 = d1Next;
            sw.d2 = d2Next;
            sw.w = w;
            sw.bW = true;
            sw.k1 = -1;
//			sw.k2 = se.vR[wPos].first.second;
            sw.k2 = p12 + 1;
            se.Q.push(sw);
            if(sw.d < se.vK[se.vK.size()-1]){
                cout << "22: se!" << endl;
                for(int i = 0; i < pvDistance.second.size(); i++){
                    cout << pvDistance.second[i] << " ";
                }
                cout << endl;
            }
        }
        else if(b1 && b2) //b1 and b2 are all se
        {
            if(bPrint == true){
                cout << "CC" << endl;
            }
            int p11 = pTop.k1;
            int p12 = pTop.k2;
            if(bPrint == true){
                cout << "(" << p11 <<"," << p12 <<")" << endl;
            }
            if(p11 == pvDistance.first.size()-1)
            {
                if(bPrint == true){
                    cout << "p12 is 0, p11 at the end" << endl;
                }
                if(p11 == 0){
                    caveKSP(ID1, w, 0, INF, INF, INF, true, true);
                }
                d1Next = caveKSP(ID1, w, p11+1, d11, d22, dTop2, true, true);
                if(bPrint == true){
                    cout << "cave p11 d1Next: " << d1Next << endl;
                }
//                if(d1Next == -1)
                if(d1Next < 0)
                {
                    se.vpbFinished[wPos].first = true;
                    vmEFinsihedQuery[ID1][w] = true;
                }
                else{
                    if(d1Next < pvDistance.first[pvDistance.first.size()-1]){
                        cout << "d1Next: " << d1Next << endl;
                        for(int i = 0; i < pvDistance.first.size(); i++){
                            cout << pvDistance.first[i] << " ";
                        }
                        cout << endl;
                        cout << "p11+1: " << p11+1 << endl;
                    }
                    pvDistance.first.push_back(d1Next);
                }
                if(bPrint == true){
                    cout << "size: " << pvDistance.first.size() << endl;
                }
            }
            se.vNode[k].insert(se.vNode[k].begin(),vvSEQuery[ID1][(*it1).second].vNode[p11].begin(),vvSEQuery[ID1][(*it1).second].vNode[p11].end());
//            se.vPathstr[k].insert(0,vvSEQuery[ID1][(*it1).second].vPathstr[p11]);
            /*for(int i = 0; i < vvSEQuery[ID1][(*it1).second].vNode[p11].size(); i++){
                se.rankSet1[k].push_back(rank[vvSEQuery[ID1][(*it1).second].vNode[p11][i]]);
            }*/
            se.rankSet1[k] = vvSEQuery[ID1][(*it1).second].vRank[p11];
            if(vvSEQuery[ID1][(*it1).second].sebLoop[p11] == true){
                se.sebLoop[k] = true;
            }

            if(p12 == pvDistance.second.size()-1)
            {
                if(bPrint == true){
                    cout << "p12 at the end" << endl;
                }
                if(p12 == 0){
                    caveKSP(ID2, w, 0, INF, INF, INF, false, true);
                }

                d2Next = caveKSP(ID2, w, p12+1, d11, d22, dTop2, false, true);
                if(bPrint == true){
                    cout << "cave p12 dNext22: " << d2Next << endl;
                }
//                if(d2Next == -1)
                if(d2Next < 0)
                {
                    se.vpbFinished[wPos].second = true;
                    vmEFinsihedQuery[ID2][w] = true;
                }
                else{
                    if(d2Next < pvDistance.second[pvDistance.second.size()-1]){
                        cout << "d2Next: " << d2Next << endl;
                        for(int i = 0; i < pvDistance.second.size(); i++){
                            cout << pvDistance.second[i] << " ";
                        }
                        cout << endl;
                        cout << "p12+1: " << p12+1 << endl;
                    }
                    pvDistance.second.push_back(d2Next);
                }
                if(bPrint == true){
                    cout << "size: " << pvDistance.second.size() << endl;
                }
            }

            for(int i = vvSEQuery[ID2][(*it2).second].vNode[p12].size()-1; i >= 0; i--){
                se.vNode[k].push_back(vvSEQuery[ID2][(*it2).second].vNode[p12][i]);
//                se.vPathstr[k].append(to_string(vvSEQuery[ID2][(*it2).second].vNode[p12][i]) + "-");
//                se.rankSet2[k].push_back(rank[vvSEQuery[ID2][(*it2).second].vNode[p12][i]]);
//                    se.vNode[k].insert(se.vNode[k].end()-1,vvSEQuery[ID2][(*it2).second].vNode[p12][i]);
                if(bPrint == true){
                    cout << vvSEQuery[ID2][(*it2).second].vNode[p12][i] << " ";
                }
//                cout << vvSEQuery[ID2][(*it2).second].vNode[p12][i] << " ";
            }
            se.rankSet2[k] = vvSEQuery[ID2][(*it2).second].vRank[p12];
            if(vvSEQuery[ID2][(*it2).second].sebLoop[p12] == true){
                se.sebLoop[k] = true;
            }

            if(bPrint == true){
                cout << endl;
            }

            if(bPrint == true){
                cout << "valley 1" << endl;
            }
            if(vmEFinsihedQuery[ID1][w]){
                if(bPrint == true){
                    cout << "First is true" << endl;
                }
            }
//                cout << "First is true" << endl;
            else{
                if(bPrint == true){
                    cout << "First is false" << endl;
                }
            }

            if(vmEFinsihedQuery[ID2][w]){
                if(bPrint == true){
                    cout << "Second is true" << endl;
                }
            }
            else{
                if(bPrint == true){
                    cout << "Second is false" << endl;
                }
            }
            if(bPrint == true){
                cout << "size: " << endl;
            }
            if(bPrint == true){
                if (se.visited.size() > wPos) {
                    cout << se.visited[wPos].size() << std::endl;
                } else {
                    cout << "Position wPos out of range" << std::endl;
                }
            }


//            bLoop = false;
            /*sort(se.rankSet1[k].begin(),se.rankSet1[k].end());
            sort(se.rankSet2[k].begin(),se.rankSet2[k].end());*/


            auto it111 = se.rankSet1[k].begin();
            auto it222 = se.rankSet2[k].begin();
            bool wAdded = false;
            se.vRank[k].clear();
            se.pathTotal[k] = 0;
            while (it111 != se.rankSet1[k].end() && it222 != se.rankSet2[k].end()) {
                if (*it111 == *it222) {
                    if(*it111 == rank[w] && !wAdded){
                        wAdded = true;
                        se.vRank[k].push_back(*it111);
                        se.pathTotal[k] += *it111;
                        ++it111;
                        ++it222;
                    }
                    else{
                        se.sebLoop[k] = true;
                        break;
                    }
                } else if (*it111 < *it222) {
                    se.vRank[k].push_back(*it111);
                    se.pathTotal[k] += *it111;
                    ++it111;
                } else {
                    se.vRank[k].push_back(*it222);
                    se.pathTotal[k] += *it222;
                    ++it222;
                }
            }

            while (it111 != se.rankSet1[k].end()) {
                se.vRank[k].push_back(*it111);
                se.pathTotal[k] += *it111;
                ++it111;
            }

            while (it222 != se.rankSet2[k].end()) {
                se.vRank[k].push_back(*it222);
                se.pathTotal[k] += *it222;
                ++it222;
            }

            /*if(vvSEQuery[ID1][(*it1).second].sebLoop[p11] == true || vvSEQuery[ID2][(*it2).second].sebLoop[p12] == true){
                se.sebLoop[k] = true;
            }*/
//            cout << "se.sebLoop[k]: " << se.sebLoop[k] << endl;

            if(bPrint ==true){
                cout << "ID1: " << ID1 << " ID2: " << ID2 << endl;
                for(int i = 0; i < se.vNode[k].size(); i++){
                    cout << se.vNode[k][i] << " ";
                }
                cout << endl;
            }

            if(se.sebLoop[k] == true){
                if(bLeft = true){
                    if(dNext + d2 <= dTop2){
                        se.vK.pop_back();
                        se.vKW.pop_back();
                        se.vSubK.pop_back();
                        se.vNode.pop_back();
                        se.sebLoop.pop_back();
                        se.vRank.pop_back();
                        se.pathTotal.pop_back();
                        se.rankSet1.pop_back();
                        se.rankSet2.pop_back();
                    }
                    else{
//                        cout << "True!!!" << endl;
                    }
                }
                else{
                    if(d1 +dNext <= dTop2){
                        se.vK.pop_back();
                        se.vKW.pop_back();
                        se.vSubK.pop_back();
                        se.vNode.pop_back();
                        se.sebLoop.pop_back();
                        se.vRank.pop_back();
                        se.pathTotal.pop_back();
                        se.rankSet1.pop_back();
                        se.rankSet2.pop_back();
                    }
                    else{
//                        cout << "True!!!" << endl;
                    }

                }
            }

            //change position
            if(d1Next != -1){
//                if(vvSEQuery[caveID1][(*it12).second].sebLoop[p11+1] == false && (p11 <= le.vK.size()*10)){
                if(p11+1 <= se.vK.size()*100){
                    if (p11 + 1 < pvDistance.first.size() && se.visited[wPos].find({p11 + 1, p12}) == se.visited[wPos].end()) {
                        subW sw;
//                sw.d = pvDistance.second[p11 + 1]+pvDistance.first[p12];
                        sw.d = pvDistance.first[p11 + 1]+pvDistance.second[p12];
                        sw.d1 = pvDistance.first[p11 + 1];
                        sw.d2 = pvDistance.second[p12];
                        sw.w = w;
                        sw.bW = true;
                        sw.k1 = p11+1;
                        sw.k2 = p12;
                        if(bPrint == true){
                            cout <<"1: " << " d1: " << pvDistance.first[p11 + 1] << " d2: " << pvDistance.second[p12] << endl;
                            cout << "k1: " << p11+1 << " k2: " << p12 << " sw.d: " << sw.d << endl;
                        }
                        se.Q.push(sw);
                        se.visited[wPos].insert({p11 + 1, p12});
                        if(sw.d < se.vK[se.vK.size()-1]){
                            cout << "p11: " << p11 << " p12: " << p12 << " ID1: " << ID1 << " w: " << w << " ID2: " << ID2 << endl;
                            cout << "33: se!" << " d: " << sw.d << " pvDistance.first[p11+1]: " << pvDistance.first[p11+1] << " pvDistance.second[p12]: " << pvDistance.second[p12] << endl;
                            for(int i = 0; i < pvDistance.first.size(); i++){
                                cout << pvDistance.first[i] << " ";
                            }
                            cout << endl;
                            for(int i = 0; i < pvDistance.second.size(); i++){
                                cout << pvDistance.second[i] << " ";
                            }
                            cout << endl;
                        }
                    }
                    bool result = se.visited[wPos].find({p11, p12 + 1}) == se.visited[wPos].end();
                    if(bPrint == true){
                        cout << "ID1: " << ID1 << " w: " << w << " ID2: " << ID2 << endl;
                        cout << "p12+1: " << p12 + 1 << " pvDistance.second.size(): " << pvDistance.second.size() << " se.visited[wPos].find({p11, p12 + 1}): " << (result ? "true" : "false") << endl;
                    }
                    auto it = se.visited[wPos].find({p11, p12 + 1});
                    if(bPrint == true){
                        if (it != se.visited[wPos].end()) {
                            std::cout << "Found: (" << it->first << ", " << it->second << ")" << std::endl;
                        } else {
                            std::cout << "Not found" << std::endl;
                        }
                    }
                }
            }
            /*if (p11 + 1 < pvDistance.first.size() && se.visited[wPos].find({p11 + 1, p12}) == se.visited[wPos].end()) {
                subW sw;
//                sw.d = pvDistance.second[p11 + 1]+pvDistance.first[p12];
                sw.d = pvDistance.first[p11 + 1]+pvDistance.second[p12];
                sw.d1 = pvDistance.first[p11 + 1];
                sw.d2 = pvDistance.second[p12];
                sw.w = w;
                sw.bW = true;
                sw.k1 = p11+1;
                sw.k2 = p12;
                if(bPrint == true){
                    cout <<"1: " << " d1: " << pvDistance.first[p11 + 1] << " d2: " << pvDistance.second[p12] << endl;
                    cout << "k1: " << p11+1 << " k2: " << p12 << " sw.d: " << sw.d << endl;
                }
                se.Q.push(sw);
                se.visited[wPos].insert({p11 + 1, p12});
                if(sw.d < se.vK[se.vK.size()-1]){
                    cout << "p11: " << p11 << " p12: " << p12 << " ID1: " << ID1 << " w: " << w << " ID2: " << ID2 << endl;
                    cout << "33: se!" << " d: " << sw.d << " pvDistance.first[p11+1]: " << pvDistance.first[p11+1] << " pvDistance.second[p12]: " << pvDistance.second[p12] << endl;
                    for(int i = 0; i < pvDistance.first.size(); i++){
                        cout << pvDistance.first[i] << " ";
                    }
                    cout << endl;
                    for(int i = 0; i < pvDistance.second.size(); i++){
                        cout << pvDistance.second[i] << " ";
                    }
                    cout << endl;
                }
            }
            bool result = se.visited[wPos].find({p11, p12 + 1}) == se.visited[wPos].end();
            if(bPrint == true){
                cout << "ID1: " << ID1 << " w: " << w << " ID2: " << ID2 << endl;
                cout << "p12+1: " << p12 + 1 << " pvDistance.second.size(): " << pvDistance.second.size() << " se.visited[wPos].find({p11, p12 + 1}): " << (result ? "true" : "false") << endl;
            }
            auto it = se.visited[wPos].find({p11, p12 + 1});
            if(bPrint == true){
                if (it != se.visited[wPos].end()) {
                    std::cout << "Found: (" << it->first << ", " << it->second << ")" << std::endl;
                } else {
                    std::cout << "Not found" << std::endl;
                }
            }*/

            if(d2Next != -1){
//                cout << " ID1: " << ID1 << " w: " << w << " ID2: " << ID2 << " vvSEQuery[ID2][(*it2).second].vK.size(): " << vvSEQuery[ID2][(*it2).second].vK.size() << " se.vK.size(): " << se.vK.size() << endl;
//                cout << "Q size: " << se.Q.size() << endl;
                if(it2 != vmSEPos[ID2].end())
                {
                    if(vvSEQuery[ID2][(*it2).second].vK.size() > se.vK.size()*100){
//                    cout << "Here!" << endl;
                        continue;
                    }
                }
            }


            if (p12 + 1 < pvDistance.second.size() && se.visited[wPos].find({p11, p12 + 1}) == se.visited[wPos].end()) {
                subW sw;
//                sw.d = pvDistance.second[p11]+pvDistance.first[p12 + 1];
                sw.d = pvDistance.first[p11]+pvDistance.second[p12 + 1];
                sw.w = w;
                sw.d1 = pvDistance.first[p11];
                sw.d2 = pvDistance.second[p12 + 1];
                sw.bW = true;
                sw.k1 = p11;
                sw.k2 = p12+1;
                if(bPrint == true){
                    cout <<"2: " << " d1: " << pvDistance.first[p11] << " d2: " << pvDistance.second[p12 + 1] << endl;
                    cout << "k1: " << sw.k1 << " k2: " << sw.k2 << " sw.d: " << sw.d << endl;
                }
                se.Q.push(sw);
                se.visited[wPos].insert({p11, p12+1});
                if(sw.d < se.vK[se.vK.size()-1]){
                    cout << "p11: " << p11 << " p12: " << p12 << " ID1: " << ID1 << " w: " << w << " ID2: " << ID2 << endl;
                    cout << "44: se!" << " d: " << sw.d << " pvDistance.first[p11]: " << pvDistance.first[p11] << " pvDistance.second[p12 + 1]: " << pvDistance.second[p12 + 1] << endl;
                    for(int i = 0; i < pvDistance.first.size(); i++){
                        cout << pvDistance.first[i] << " ";
                    }
                    cout << endl;
                    for(int i = 0; i < pvDistance.second.size(); i++){
                        cout << pvDistance.second[i] << " ";
                    }
                    cout << endl;
                }
            }
        }
    }

    return vvSEQuery[ID1][(*it).second].vK[k];
}

void Graph::caveRetriveal(int ID1, int ID2, int k, list<int>& lPath)
{
    if(bPrint == true){
        cout << "caveRetriveal: " << endl;
    }
    lPath.push_back(ID1);
    lPath.push_back(ID2);
    auto it1 = lPath.begin();
    auto it2 = it1;
    it2++;
    if(bPrint == true){
        cout << "it1: " << *it1 << " it2: " << *it2 << endl;
    }
    auto it = vmSEPos[ID1].find(ID2);
    queue<sePATH> qSE;
    sePATH seP;
    //Add to avoid the situation it == vmSEPos[ID1].end()
    if(it == vmSEPos[ID1].end())
    {
        return;
    }
    else{
        seP.se = &vvSEQuery[ID1][(*it).second];
        seP.k = k;
        seP.it1 = it1;
        seP.it2 = it2;
        seP.bForward = true;
    }
    qSE.push(seP);
//	cout << "R:" << ID1 << "\t" << ID2 << "\t" << k << endl;
    while(!qSE.empty())
    {
        sePATH seTop = qSE.front();
        qSE.pop();
        int topID1 = seTop.se->ID1;
        int topID2 = seTop.se->ID2;
//		cout << endl << "RTop:" << topID1 << "\t" << topID2 << "\t" << seTop.k << "\t" << *(seTop.it1) << "\t" << *(seTop.it2) << endl;

        /*for(auto& il: lPath)
            cout << il << "\t";
        cout << endl;*/
        sePATH seLeft, seRight;
        if(seTop.bForward)
        {
            if(bPrint == true){
                cout << "Forward" << endl;
                cout << seTop.se->vKW.size() << endl;
                cout << "k: " << seTop.k << endl;
            }
            int w;
            if ((*(seTop.se)).vKW.size() == 0)
            {
                subW pTop = seTop.se->Q.top();
                w = pTop.w;
                if(bPrint == true){
                    cout << "seTop ID1:" << topID1 << "\tseTop ID2:" << topID2 << "\tw:" << w << endl;
                }
                if(w == topID1 || w == topID2)
                    continue;
                seTop.it1++;
                if(bPrint == true){
                    cout << "seTop.it1: " << *seTop.it1 << endl;
                }
                seTop.it1 = lPath.insert(seTop.it1, w);
                seTop.it1--;
            }
            else
            {
                w = seTop.se->vKW[seTop.k];
                if(bPrint == true){
                    cout << "seTop ID1:" << topID1 << "\tseTop ID2:" << topID2 << "\tw:" << w << endl;
                }
                if(w == topID1 || w == topID2)
                    continue;
                seTop.it1++;
//                cout << "seTop.it1: " << *seTop.it1 << endl;
                seTop.it1 = lPath.insert(seTop.it1,w);
                seTop.it1--;
//                cout << "seTop.it2: " << *seTop.it1 << endl;
            }

            auto itLeft = vmSEPos[topID1].find(w);
            if(itLeft != vmSEPos[topID1].end())
            {
                seLeft.se = &vvSEQuery[topID1][(*itLeft).second];
                if(bPrint == true){
                    cout << (*(seTop.se)).vKW.size() << endl;
                }
                if ((*(seTop.se)).vKW.size() == 0){
                    seLeft.k = 0;
//                    cout << "0!!!" << endl;
                }
                else
                    seLeft.k = (*(seTop.se)).vSubK[seTop.k].first;
//                seLeft.k = (*(seTop.se)).vSubK[seTop.k].first;
                if(bPrint == true){
                    cout << "Left k:" << seLeft.k << endl;
                    cout << "Left k distance: " << seLeft.se->vK[seLeft.k] << endl;
                    cout << "Left ID1:" << seLeft.se->ID1 << "\tLeft ID2:" << seLeft.se->ID2 << endl;
                }
                if(seLeft.k != -1)
                {
                    seLeft.it1 = seTop.it1;
                    seLeft.it2 = seLeft.it1;
                    seLeft.it2++;
                    seLeft.bForward = true;
                    qSE.push(seLeft);
                }
//				else
//					cout << "Left Original Edge " << topID1 << "\t" << w << "\t" << vmEdge[topID1][w] << endl;
            }
//			else
//				cout << "Left Edge " << topID1 << "\t" << w << "\t" << vmEdge[topID1][w] << endl;

            auto itRight = vmSEPos[topID2].find(w);
            if(itRight != vmSEPos[topID2].end())
            {
                seRight.se = &vvSEQuery[topID2][(*itRight).second];
                if(bPrint == true){
                    cout << "k: " << k << " vKW size(): " << (*(seTop.se)).vKW.size() << endl;
                }
//                cout << "k: " << k << " vKW size(): " << (*(seTop.se)).vKW.size() << endl;
//				if ((*(seTop.se)).vKW.size() <= k){
                if ((*(seTop.se)).vKW.size() == 0){
                    seRight.k = 0;
//                    cout << "0!!!" << endl;
                }
                else
                    seRight.k = (*(seTop.se)).vSubK[seTop.k].second;
//                seRight.k = (*(seTop.se)).vSubK[seTop.k].second;
                if(bPrint == true){
                    cout << "Right k:" << seRight.k << endl;
                    cout << "Right k distance: " << seRight.se->vK[seRight.k] << endl;
                    cout << "Right ID1:" << seRight.se->ID1 << "\tRight ID2:" << seRight.se->ID2 << endl;
                }
                if(seRight.k != -1)
                {
                    seRight.it1 = seTop.it2;
                    seRight.it2 = seRight.it1;
                    seRight.it2--;
                    seRight.bForward = false;
                    qSE.push(seRight);
                }
//				else
//					cout << "Right Original Edge " << topID2 << "\t" << w << "\t" << vmEdge[topID2][w] << endl;
            }
//			else
//				cout << "Right Edge " << topID2 << "\t" << w << "\t" << vmEdge[topID2][w] << endl;
        }
        else
        {
            if(bPrint == true){
                cout << "Backward" << endl;
                cout << seTop.se->vKW.size() << endl;
                cout << "k: " << seTop.k << endl;
            }
            int w;
            if ((*(seTop.se)).vKW.size() == 0)
            {
                subW pTop = seTop.se->Q.top();
                w = pTop.w;
                if(bPrint == true){
                    cout << "seTop ID1:" << topID1 << "\tseTop ID2:" << topID2 << "\tw:" << w << endl;
                }
                if(w == topID1 || w == topID2)
                    continue;
/*				if((topID2 == 113195 && w == 113179) || (topID1 == 113179 && topID2==113194) )
				{
					for(int j = 0; j < seTop.se->vDistance.size(); j++)
					{
						cout << seTop.se->vW[j] << ":" << seTop.se->vDistance[j] << endl;
					}
				}
*/
                seTop.it2++;
                seTop.it2 = lPath.insert(seTop.it2, w);
                seTop.it2--;

            }
            else
            {
                w = seTop.se->vKW[seTop.k];
                if(bPrint == true){
                    cout << "seTop ID1:" << topID1 << "\tseTop ID2:" << topID2 << "\tw:" << w << endl;
                }
                if(w == topID1 || w == topID2)
                    continue;
                seTop.it2++;
                seTop.it2 = lPath.insert(seTop.it2, w);
                seTop.it2--;
            }

            auto itLeft = vmSEPos[topID2].find(w);
            if(itLeft != vmSEPos[topID2].end())
            {
                seLeft.se = &vvSEQuery[topID2][(*itLeft).second];
                if ((*(seTop.se)).vKW.size() == 0){
                    seLeft.k = 0;
//                    cout << "0!!!" << endl;
                }
                else
//					seLeft.k = (*(seTop.se)).vSubK[seTop.k].first;
                    seLeft.k = (*(seTop.se)).vSubK[seTop.k].second;
//                !!!!!!!!!!!!!!!!!!!!!!!!!
                if(bPrint == true){
                    cout << "Left k:" << seLeft.k << endl;
                    cout << "Left ID1:" << seLeft.se->ID1 << "\tLeft ID2:" << seLeft.se->ID2 << endl;
                }
                if(seLeft.k != -1)
                {
                    seLeft.it1 = seTop.it2;
                    seLeft.it2 = seLeft.it1;
                    seLeft.it2++;
                    seLeft.bForward = true;
                    qSE.push(seLeft);
                }
//				else
//					cout << "Left Original Edge " << topID2 << "\t" << w << "\t" << vmEdge[topID2][w] << endl;
            }
//			else
//				cout << "Left Edge " << topID2 << "\t" << w << "\t" << vmEdge[topID2][w] << endl;

            auto itRight = vmSEPos[topID1].find(w);
            if(itRight != vmSEPos[topID1].end())
            {
                seRight.se = &vvSEQuery[topID1][(*itRight).second];
                if ((*(seTop.se)).vKW.size() == 0){
                    seRight.k = 0;
//                    cout << "0!!!" << endl;
                }
                    //QSWL!!!!!!!
                else
//					seRight.k = (*(seTop.se)).vSubK[seTop.k].second;
                    seRight.k = (*(seTop.se)).vSubK[seTop.k].first;
                if(bPrint == true){
                    cout << "Right k:" << seRight.k << endl;
                    cout << "Right ID1:" << seRight.se->ID1 << "\tRight ID2:" << seRight.se->ID2 << endl;
                }
                if(seRight.k != -1)
                {
                    seRight.it1 = seTop.it1;
                    seRight.it2 = seRight.it1;
                    seRight.it2--;
                    seRight.bForward = false;
                    qSE.push(seRight);
                }
//				else
//					cout << "Right Original Edge " << topID1 << "\t" << w << "\t" << vmEdge[topID1][w] << endl;
            }
//			else
//				cout << "Right Edge " << topID1 << "\t" << w << "\t" << vmEdge[topID1][w] << endl;
        }

    }
}


int Graph::peakKSP(int ID1, int ID2, int k, bool brnvK)
{
    std::chrono::high_resolution_clock::time_point t1;
    std::chrono::high_resolution_clock::time_point t2;
    t1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span;
    int o1 = NodeOrder[ID1];
    int o2 = NodeOrder[ID2];

    if(o1 > o2){
        int tmp = ID1;
        ID1 = ID2;
        ID2 = tmp;
    }
//    cout << endl << "Peak " << ID1 <<" " << ID2 << " true " << k << endl;
    if(bPrint == true){
        cout << endl << "Peak " << ID1 <<"\t" << ID2 << "\ttrue\t" << k << endl;
    }
//    cout << endl << "Peak " << ID1 <<"\t" << ID2 << "\t" << brnvK << k << endl;

    int r1 = rank[ID1];
    int r2 = rank[ID2];
    int LCA = LCAQuery(r1,r2);
    if(bPrint == true){
        cout << "r1: " << r1 << " r2: " << r2 << " LCA: " <<  LCA << endl;
        cout << "unique Node: " << Tree[LCA].uniqueVertex << endl;
        cout << "r1 Node: " << Tree[r1].uniqueVertex << " r2 Node: " << Tree[r2].uniqueVertex << endl;
        cout << "k: " << k << endl;
    }

    if (!(LCA == r1 || LCA == r2)){
        bool bAdd = false;
        if(brnvK == true){
//            cout<< "need to be clear" << endl;
            vK.clear();
            vKW.clear();
            vRank.clear();
            pathTotal.clear();
            rankSet1.clear();
            rankSet2.clear();
//            vPathstr.clear();
//            vchaPath.clear();
            vNode.clear();
            hQ = priority_queue<subH, vector<subH>, CompareH>();
            vNode = vector<vector<int>>();
            vSubA.clear();
//            uniquePaths.clear();
            hashSet.clear();
            umAPos.clear();
//            cout <<"clear" << endl;
        }
        while(k >= vK.size()){
            subH sTop;
            int aPos;
            if(k == 0 && bAdd == false){
                if(bPrint == true){
                    cout << " LCA: " << LCA << " r1: " << r1 << " r2: " << r2 << " k: " << k << endl;
                }
//                cout << " LCA: " << LCA << " r1: " << r1 << " r2: " << r2 << " k: " << k << endl;
                vbFinished.assign(nodeNum,false);
                vpbFinished.assign(nodeNum, make_pair(false,false));
                v1.assign(nodeNum, tmpv);
                v2.assign(nodeNum, tmpv);
                vpvADistance.assign(nodeNum,tmpvpvADistance);
                visited.assign(nodeNum,tmpvisited);
                vR.assign(nodeNum, tmpvR);
                if(bPrint == true) {
                    cout << "LCA Node: " << Tree[LCA].uniqueVertex << endl;
                }
                int i = 0;
                subH sh;
                int hID = Tree[LCA].uniqueVertex;
                if(bPrint == true) {
                    cout << "hID: " << hID << endl;
                }
//                cout << "hID: " << hID << endl;
                auto it = vmLEPos[ID1].find(hID);
                if(it == vmLEPos[ID1].end())
                {
//                    cout << "No Label" << endl;
                    return -1;
                }
                auto it2 = vmLEPos[ID2].find(hID);
                if(it == vmLEPos[ID2].end())
                {
//                    cout << "No Label" << endl;
                    return -1;
                }

                int dNext1 = peakKSP(ID1, hID, k, false);
                vpvADistance[hID].first.push_back(dNext1);
//                cout << "dNext1: " << dNext1 << endl;
                if(bPrint == true) {
                    cout << "dNext1: " << dNext1 << endl;
                }

                int dNext2 = peakKSP(ID2, hID, k, false);
                vpvADistance[hID].second.push_back(dNext2);
//                cout << "dNext2:" << dNext2 << endl;
                if(bPrint == true) {
                    cout << "dNext2:" << dNext2 << endl;
                }

                sh.d1 = dNext1;
                sh.d2 = dNext2;
                sh.k1 = 0;
                sh.k2 = 0;
                sh.d = dNext1 + dNext2;
                sh.h = hID;
                vR[hID] = make_pair(make_pair(0,0), make_pair(0,0));
                hQ.push(sh);
                umAPos.insert(make_pair(hID, i));
                i++;
//                for(auto& m : vvLEQuery[Tree[LCA].uniqueVertex])
                for(int m = 0; m < Tree[LCA].vert.size(); m++)
                {
                    subH sh;
//                    int hID = m.ID2;
                    int hID = Tree[LCA].vert[m].first;
                    if(bPrint == true) {
                        cout << "hID: " << hID << endl;
                    }
//                    cout << "hID: " << hID << endl;
                    auto it = vmLEPos[ID1].find(hID);
                    if(it == vmLEPos[ID1].end())
                    {
//                        cout << "No Label" << endl;
                        return -1;
                    }
                    int dNext1 = peakKSP(ID1, hID, k, false);
                    vpvADistance[hID].first.push_back(dNext1);
                    if(bPrint == true) {
                        cout << "dNext1: " << dNext1 << endl;
                    }
//                    cout << "dNext1: " << dNext1 << endl;
                    int dNext2 = peakKSP(ID2, hID, k, false);
                    vpvADistance[hID].second.push_back(dNext2);
                    if(bPrint == true) {
                        cout << "dNext2:" << dNext2 << endl;
                    }
//                    cout << "dNext2:" << dNext2 << endl;
                    sh.d1 = dNext1;
                    sh.d2 = dNext2;
                    sh.k1 = 0;
                    sh.k2 = 0;
                    sh.d = dNext1 + dNext2;
                    sh.h = hID;
                    vR[hID] = make_pair(make_pair(0,0), make_pair(0,0));
                    hQ.push(sh);
                    umAPos.insert(make_pair(hID, i));
                    i++;
                }
                cout << "Finish ancestor" << endl;


                sTop = hQ.top();
                hQ.pop();
//                vchaPath.push_back(1);
                vK.push_back(sTop.d);
//                KSPNumber++;
                vKW.push_back(sTop.h);
                visited[sTop.h].insert({0, 0});
                aPos = umAPos[sTop.h];
                vSubA.push_back(make_pair(sTop.k1, sTop.k2));
                if(bPrint == true) {
                    cout << "k1: " << sTop.k1 << " k2: " << sTop.k2 << endl;
                }
                auto it3 = vmLEPos[ID1].find(sTop.h);
                auto it4 = vmLEPos[ID2].find(sTop.h);


                vector<int> x1;
                x1.push_back(ID1);
                x1.push_back(ID2);
                if(sTop.h != ID2 ){
                    x1.insert(x1.begin()+1,sTop.h);
                }
                vNode.push_back(x1);


                vector<int> x2;
                x2.push_back(rank[ID1]);
                if(sTop.h != ID2 ){
                    x2.push_back(rank[sTop.h]);
                }
                sort(x2.begin(), x2.end());
                rankSet1.push_back(x2);

                vector<int> x3;
                x3.push_back(rank[ID2]);
                if(sTop.h != ID2 ){
                    x3.push_back(rank[sTop.h]);
                }
                sort(x3.begin(), x3.end());
                rankSet2.push_back(x3);

                vector<int> x4;
                x4.push_back(rank[ID1]);
                x4.push_back(rank[ID2]);
                if(sTop.h != ID2 ){
                    x4.push_back(rank[sTop.h]);
                }
                sort(x4.begin(), x4.end());
                vRank.push_back(x4);
                pathTotal.push_back(rank[ID1]+rank[ID2]+rank[sTop.h]);

                int p11 = sTop.k1;
                int p12 = sTop.k2;

                auto ithTop = find(vNode[k].begin(), vNode[k].end(), sTop.h);
                /*auto strTop = vPathstr[k].find("-"+to_string(sTop.h)+"-")+1;
                auto firstDash = vvLEQuery[ID1][(*it).second].vPathstr[p11].find("-");
                auto finalItem = vvLEQuery[ID1][(*it).second].vPathstr[p11].find("-"+to_string(sTop.h)+"-");*/

                if (ithTop != vNode[k].end()) {
                    vNode[k].insert(ithTop, vvLEQuery[ID1][(*it3).second].vNode[p11].begin()+1, vvLEQuery[ID1][(*it3).second].vNode[p11].end()-1);
//                    string extracted = vvLEQuery[ID1][(*it3).second].vPathstr[p11].substr(2, vvLEQuery[ID1][(*it3).second].vPathstr[p11].size()-4);
                    /*string extracted = vvLEQuery[ID1][(*it3).second].vPathstr[p11].substr(firstDash+1, finalItem-firstDash);
                    vPathstr[k].insert(strTop, extracted);*/
                }
                /*for(int i = 1; i < vvLEQuery[ID1][(*it3).second].vNode[p11].size()-1; i++){
                    rankSet1[k].push_back(rank[vvLEQuery[ID1][(*it3).second].vNode[p11][i]]);
                }*/
                rankSet1[k] = vvLEQuery[ID1][(*it3).second].vRank[p11];
                for(int m = vvLEQuery[ID2][(*it4).second].vNode[p12].size()-2; m >= 1; m--){
                    if(bPrint == true) {
                        cout << "vvLEQuery[ID2][(*it4).second].vNode[p12][i]: " << vvLEQuery[ID2][(*it4).second].vNode[p12][m] << " ";
                    }
                    vNode[k].insert(vNode[k].end()-1, vvLEQuery[ID2][(*it4).second].vNode[p12][m]);
                    /*auto ID2Pos = vPathstr[k].find("-"+to_string(ID2)+"-")+1;
                    vPathstr[k].insert(ID2Pos, to_string(vvLEQuery[ID2][(*it4).second].vNode[p12][m])+"-");*/
//                    rankSet2[k].push_back(rank[vvLEQuery[ID2][(*it4).second].vNode[p12][m]]);
                }
                rankSet2[k] = vvLEQuery[ID2][(*it4).second].vRank[p12];
                if(bPrint == true) {
                    cout << endl;
                }
                //If loop?
                if(bPrint == true) {
                    for(int m = 0; m < vNode[k].size(); m++){
                        cout << vNode[k][m] << " ";
                    }
                    cout << endl;
                }
                unordered_set<int> elementSet;
                bool bLoop = false;
                bool bRepeat = false;

                /* sort(rankSet1[k].begin(),rankSet1[k].end());
                 sort(rankSet2[k].begin(),rankSet2[k].end());*/

                auto it111 = rankSet1[k].begin();
                auto it222 = rankSet2[k].begin();


                bool hAdded = false;
                vRank[k].clear();
                pathTotal[k]= 0;
                while (it111 != rankSet1[k].end() && it222 != rankSet2[k].end()) {
                    if (*it111 == *it222) {
                        if(*it111 == rank[sTop.h] && !hAdded){
                            hAdded = true;
                            vRank[k].push_back(*it111);
                            pathTotal[k] += *it111;
                            ++it111;
                            ++it222;
                        }
                        else{
                            bLoop = true;
                            break;
                        }
                    } else if (*it111 < *it222) {
                        vRank[k].push_back(*it111);
                        pathTotal[k] += *it111;
                        ++it111;
                    } else {
                        vRank[k].push_back(*it222);
                        pathTotal[k] += *it222;
                        ++it222;
                    }
                }

                while (it111 != rankSet1[k].end()) {
                    vRank[k].push_back(*it111);
                    pathTotal[k] += *it111;
                    ++it111;
                }

                while (it222 != rankSet2[k].end()) {
                    vRank[k].push_back(*it222);
                    pathTotal[k] += *it222;
                    ++it222;
                }

                uint64_t vPathHash = XXH64(vNode[k].data(), vNode[k].size() * sizeof(int), 0);
                if(k > 0){
                    if(vK[k] != vK[k-1]){
                        hashSet.clear();
                    }
                    else{
                        for(int i = 1; i < hashSet.size()+1; i++){
                            if(vNode[k-i].size() == vNode[k].size() && pathTotal[k-i] == pathTotal[k] && pathTotal[k-i] == pathTotal[k]){
                                if (hashSet.find(vPathHash) != hashSet.end()) {
                                    bRepeat = true;
                                    break;
                                }
                            }
                        }
                    }
                }


                if(bLoop == true || bRepeat == true){
//                    cout << "ID1: " << ID1 << " ID2: " << ID2 << " a: " << sTop.h << " peak1 true" << " bLoop: " << bLoop << " bRepeat: " << bRepeat << " length: " << vK[vK.size()-1] << endl;
                    /*for(int i = 0; i < vNode[vNode.size()-1].size(); i++){
                        cout << vNode[vNode.size()-1][i] << " ";
                    }
                    cout << endl;*/
//                    cout << "le1 true" << endl;
                    vK.pop_back();
                    KSPNumber++;
//                    vPathstr.pop_back();
                    vKW.pop_back();
                    vRank.pop_back();
                    pathTotal.pop_back();
                    vSubA.pop_back();
                    vNode.pop_back();
                    rankSet1.pop_back();
                    rankSet2.pop_back();
                }
                else{
//                    uniquePaths.insert(vPathstr[k]);
                    hashSet.insert(vPathHash);
                }

               /* if(bPrint == true) {
                    for(const auto& path :  uniquePaths){
                        cout << path << endl;
                    }
                    cout << "uniquePaths size: " << uniquePaths.size() << " k: " << k << endl;
                }*/

//                uniquePaths.insert(pathStr);

                if(bPrint == true) {
                    cout << "first end" << endl;
                }

                if(bPrint == true) {
                    /*for(const auto& path :  uniquePaths){
                        cout << path << endl;
                    }

                    cout << endl;
                    cout << endl;
                    cout << endl;
                    cout << endl;*/
//                cout << "hID: " << sTop.h << endl;
                    cout << "d: " << sTop.d << " d1: " << sTop.d1 << " d2: " << sTop.d2 << " h: " << sTop.h << endl;
                    cout << "k: " << k << " vK size: " << vK.size() << endl;
                }
                bAdd = true;

            }
            else{
                sTop = hQ.top();
                hQ.pop();
//                vchaPath.push_back(1);
                vK.push_back(sTop.d);
                vKW.push_back(sTop.h);
                aPos = umAPos[sTop.h];
                vSubA.push_back(make_pair(sTop.k1, sTop.k2));
                if(bPrint == true) {
                    cout << "ID1: " << ID1 << " ID2: " << ID2 << " sTop.h: " << sTop.h << endl;
                    cout << "hQ size: " << hQ.size() << endl;
                    cout << "k1: " << sTop.k1 << " k2: " << sTop.k2 << endl;
                    cout << "d: " << sTop.d << " d1: " << sTop.d1 << " d2: " << sTop.d2 << " h: " << sTop.h << endl;
                    cout << "k: " << k << " vK size: " << vK.size() << endl;
                }


                auto it = vmLEPos[ID1].find(sTop.h);
                if(it == vmLEPos[ID1].end())
                {
//                    cout << "No Label" << endl;
                    return -1;
                }
                auto it2 = vmLEPos[ID2].find(sTop.h);
                if(it == vmLEPos[ID2].end())
                {
//                    cout << "No Label" << endl;
                    return -1;
                }
                if(bPrint == true) {
                    cout << "k size():" << endl;
                    cout << "k: " << k << endl;
                    cout << "new add test" << endl;
                }

                vector<int> x1;
                x1.push_back(ID1);
                x1.push_back(ID2);
                if(sTop.h != ID2 ){
                    x1.insert(x1.begin()+1,sTop.h);
//                    x1.push_back(sTop.h);
                }
                vNode.push_back(x1);

                /*string s1;
                s1 = to_string(ID1)+"-";
                s1 += to_string(ID2)+"-";
                if(sTop.h != ID2 ){
                    auto ID2pos = s1.find("-"+to_string(ID2)+"-")+1;
                    s1.insert(ID2pos, to_string(sTop.h)+"-");
                }
                vPathstr.push_back(s1);*/
//                cout << "second vPathstr[k]: " << vPathstr[k] << endl;

                vector<int> x2;
                x2.push_back(rank[ID1]);
                if(sTop.h != ID2 ){
                    x2.push_back(rank[sTop.h]);
                }
                sort(x2.begin(), x2.end());
                rankSet1.push_back(x2);

                vector<int> x3;
                x3.push_back(rank[ID2]);
                if(sTop.h != ID2 ){
                    x3.push_back(rank[sTop.h]);
                }
                sort(x3.begin(), x3.end());
                rankSet2.push_back(x3);

                vector<int> x4;
                x4.push_back(rank[ID1]);
                x4.push_back(rank[ID2]);
                if(sTop.h != ID2 ){
                    x4.push_back(rank[sTop.h]);
                }
                sort(x4.begin(), x4.end());
                vRank.push_back(x4);
                pathTotal.push_back(rank[ID1]+rank[ID2]+rank[sTop.h]);

                int p11 = sTop.k1;
                int p12 = sTop.k2;
                auto ithTop = find(vNode[k].begin(), vNode[k].end(), sTop.h);
                /*auto strTop = vPathstr[k].find("-"+to_string(sTop.h)+"-")+1;
//                cout << "sTop: " << sTop.h << " pos: " << strTop << endl;
                auto firstDash = vvLEQuery[ID1][(*it).second].vPathstr[p11].find("-");
                auto finalItem = vvLEQuery[ID1][(*it).second].vPathstr[p11].find("-"+to_string(sTop.h)+"-");*/

                if (ithTop != vNode[k].end()) {
                    vNode[k].insert(ithTop, vvLEQuery[ID1][(*it).second].vNode[p11].begin()+1, vvLEQuery[ID1][(*it).second].vNode[p11].end()-1);
//                    cout << vvLEQuery[ID1][(*it).second].vPathstr[p11] << endl;
//                    rankSet1[k].insert(rankSet1[k].begin(), vvLEQuery[ID1][(*it).second].vNode[p11].begin()+1, vvLEQuery[ID1][(*it).second].vNode[p11].end()-1);
                    /*string extracted = vvLEQuery[ID1][(*it).second].vPathstr[p11].substr(firstDash +1, finalItem-firstDash);
//                    cout << "second extracted: "<< extracted << endl;
                    vPathstr[k].insert(strTop, extracted);*/
                }
                /* for(int i = 1; i < vvLEQuery[ID1][(*it).second].vNode[p11].size()-1; i++){
                     rankSet1[k].push_back(rank[vvLEQuery[ID1][(*it).second].vNode[p11][i]]);
                 }*/
                rankSet1[k] = vvLEQuery[ID1][(*it).second].vRank[p11];
//                rankSet2[k].push_back(rank[ID2]);
                for(int m = vvLEQuery[ID2][(*it2).second].vNode[p12].size()-2; m >= 1; m--){
                    if(bPrint == true) {
                        cout << "vvLEQuery[ID2][(*it2).second].vNode[p12]: " << vvLEQuery[ID2][(*it2).second].vNode[p12][m] << " ";
                    }
                    vNode[k].insert(vNode[k].end()-1, vvLEQuery[ID2][(*it2).second].vNode[p12][m]);
//                    rankSet2[k].push_back(rank[vvLEQuery[ID2][(*it2).second].vNode[p12][m]]);
                    /*auto ID2Pos = vPathstr[k].find("-"+ to_string(ID2)+"-")+1;
                    vPathstr[k].insert(ID2Pos, to_string(vvLEQuery[ID2][(*it2).second].vNode[p12][m])+"-");*/
                }
                rankSet2[k] = vvLEQuery[ID2][(*it2).second].vRank[p12];
                if(bPrint == true) {
                    cout << endl;
                }
                /*cout << "vPathstr[k]: ";
                cout << vPathstr[k];
                cout << endl;
                cout << "vNode: ";
                for(int i = 0; i< vNode[k].size(); i++){
                    cout << vNode[k][i] << " ";
                }
                cout << endl;
                cout << endl;*/

                //If loop?
                if(bPrint == true) {
                    for(int m = 0; m < vNode[k].size(); m++){
                        cout << vNode[k][m] << " ";
                    }
                    cout << endl;
                }
                unordered_set<int> elementSet;
                bool bLoop = false;
                bool bRepeat = false;

                /*sort(rankSet1[k].begin(),rankSet1[k].end());
                sort(rankSet2[k].begin(),rankSet2[k].end());*/

                auto it111 = rankSet1[k].begin();
                auto it222 = rankSet2[k].begin();


                bool hAdded = false;
                vRank[k].clear();
                pathTotal[k] = 0;
                while (it111 != rankSet1[k].end() && it222 != rankSet2[k].end()) {
                    if (*it111 == *it222) {
                        if(*it111 == rank[sTop.h] && !hAdded){
                            hAdded = true;
                            vRank[k].push_back(*it111);
                            pathTotal[k] += *it111;
                            ++it111;
                            ++it222;
                        }
                        else{
                            bLoop = true;
                            break;
                        }
                    } else if (*it111 < *it222) {
                        vRank[k].push_back(*it111);
                        pathTotal[k] += *it111;
                        ++it111;
                    } else {
                        vRank[k].push_back(*it222);
                        pathTotal[k] += *it222;
                        ++it222;
                    }
                }

                while (it111 != rankSet1[k].end()) {
                    vRank[k].push_back(*it111);
                    pathTotal[k] += *it111;
                    ++it111;
                }

                while (it222 != rankSet2[k].end()) {
                    vRank[k].push_back(*it222);
                    pathTotal[k] += *it222;
                    ++it222;
                }
                /*if(bLoop== false){
                    cout << "vNode: ";
                    for(int i = 0; i < vNode[k].size(); i++){
                        cout << rank[vNode[k][i]] << " ";
                    }
                    cout << endl;
                    cout << "rankSet1: ";
                    for(int i = 0; i < rankSet1[k].size(); i++){
                        cout << rankSet1[k][i] << " ";
                    }
                    cout << endl;
                    cout << "rankSet2: ";
                    for(int i = 0; i < rankSet2[k].size(); i++){
                        cout << rankSet2[k][i] << " ";
                    }
                    cout << endl;
                    cout <<endl;
                }*/


                /*string pathStr;
                vector<int> tmpvNode;
                tmpvNode = vNode[k];
//                sort(tmpvNode.begin(), tmpvNode.end());
                if(bPrint == true) {
                    cout << "First repeat test: " << endl;
                }
//                cout << "First repeat test: " << endl;
                for(int node : tmpvNode) {
                    if(bPrint == true) {
                        cout << node << " ";
                    }
                    pathStr += to_string(node) + "-";
                }
                if(bPrint == true) {
                    cout << endl;
                }
                *//*if (uniquePaths.find(pathStr) != uniquePaths.end()) {
                    bRepeat = true;
                }*//*
//                cout << "vK[k]: " << vK[k] << " vK[k-1]: " << vK[k-1] << endl;
                if(k > 0 && vK[k] != vK[k-1]){
                    uniquePaths.clear();
                }
                else{
                    if(k > 0){
                        for(int i = 1; i < uniquePaths.size()+1; i++){
                            if(vNode[k-i].size() == vNode[k].size()){
                                if (uniquePaths.find(pathStr) != uniquePaths.end()) {
                                    bRepeat = true;
                                    break;
                                }
                            }
                        }
                    }
                }*/
                /*if(k > 0 && vK[k] != vK[k-1]){
                    uniquePaths.clear();
                    bRepeat = false;
                }
                else{
                    if (uniquePaths.find(vPathstr[k]) != uniquePaths.end()) {
                        bRepeat = true;
                    }
                    else{
                        bRepeat = false;
                    }
                }*/


                uint64_t vPathHash = XXH64(vNode[k].data(), vNode[k].size() * sizeof(int), 0);
                if(k > 0){
                    if(vK[k] != vK[k-1]){
                        hashSet.clear();
                    }
                    else{
                        for(int i = 1; i < hashSet.size()+1; i++){
                            if(vNode[k-i].size() == vNode[k].size() && pathTotal[k-i] == pathTotal[k]){
                                if (hashSet.find(vPathHash) != hashSet.end()) {
                                    bRepeat = true;
                                    break;
                                }
                            }
                        }
                    }
                }


                if(bLoop == true || bRepeat == true){
//                    cout << "le2 true" << endl;
                    /*for(const auto& path :  uniquePaths){
                        cout << path << endl;
                    }*/
//                    cout << "ID1: " << ID1 << " ID2: " << ID2 << " a: " << sTop.h << " peak2 true" << " bLoop: " << bLoop << " bRepeat: " << bRepeat << " length: " << vK[vK.size()-1] << endl;
                    /*for(int i = 0; i < vNode[vNode.size()-1].size(); i++){
                        cout << vNode[vNode.size()-1][i] << " ";
                    }
                    cout << endl;*/
                    vK.pop_back();
                    KSPNumber++;
//                    vPathstr.pop_back();
                    vRank.pop_back();
                    pathTotal.pop_back();
                    vKW.pop_back();
                    vSubA.pop_back();
                    vNode.pop_back();
                    rankSet1.pop_back();
                    rankSet2.pop_back();
                }
                else{
//                    uniquePaths.insert(vPathstr[k]);
//                    uniquePaths.insert(pathStr);
                    hashSet.insert(vPathHash);
                }

                /*if(bPrint == true) {
                    for(const auto& path :  uniquePaths){
                        cout << path << endl;
                    }
                    cout << "uniquePaths size: " << uniquePaths.size() << " k: " << k << endl;
                }*/

//                uniquePaths.insert(pathStr);

            }

            int p11 = sTop.k1;
            int p12 = sTop.k2;
            auto& pvDistance = vpvADistance[sTop.h];
            int dNext11,dNext22;

            if(bPrint == true) {
                cout << "pvDistance first size: " << pvDistance.first.size() << endl;
                for (int i = 0; i < pvDistance.first.size(); ++i) {
                    cout << pvDistance.first[i] << " ";
                }
                cout << endl;
                cout << "pvDistance second size: " << pvDistance.second.size() << endl;
                for (int i = 0; i < pvDistance.second.size(); ++i) {
                    cout << pvDistance.second[i] << " ";
                }
                cout << endl;
                cout << "(" << p11 <<"," << p12 <<")" << endl;
            }
            auto it = vmLEPos[ID1].find(sTop.h);
            if(it == vmLEPos[ID1].end())
            {
                vmLFinsihedQuery[ID1][sTop.h] = true;
            }
            auto it2 = vmLEPos[ID2].find(sTop.h);
            if(it == vmLEPos[ID2].end())
            {
                vmLFinsihedQuery[ID2][sTop.h] = true;
            }

            if(vvLEQuery[ID1][sTop.h].vK.size() <= pvDistance.first.size() || vvLEQuery[ID2][sTop.h].vK.size() <= pvDistance.second.size()){
                if(p11 == pvDistance.first.size()-1 && p12 == pvDistance.second.size()-1 && vmLFinsihedQuery[ID1][sTop.h] && vmLFinsihedQuery[ID2][sTop.h])
                {
                    vbFinished[aPos] = true;
                    /* if(p11 == 0 && p12 == 0)
                         return -1;*/
                    if(k < vK.size())
                        return vK[k];
                    else
                        return -1;
                }
            }

            if(bPrint == true) {
                cout << pvDistance.first[p11] << "\t" << pvDistance.second[p12] << endl;
                cout << "pvDistance first: " << pvDistance.first[p11] << " pvDistance second: " << pvDistance.second[p12] << endl;
            }


            if(p11 == pvDistance.first.size()-1) {
                dNext11 = peakKSP(ID1, sTop.h, p11 + 1, false);
                if (dNext11 == -1) {
                    vpbFinished[aPos].first = true;
                    vmLFinsihedQuery[ID1][sTop.h] = true;
                } else
                    pvDistance.first.push_back(dNext11);
                if(bPrint == true) {
                    cout << "size: " << pvDistance.first.size() << endl;
                }
            }


            if(p12 == pvDistance.second.size()-1){
                dNext22 = peakKSP(ID2, sTop.h, p12 + 1, false);
                if (dNext22 == -1) {
                    vpbFinished[aPos].second = true;
                    vmLFinsihedQuery[ID2][sTop.h] = true;
                } else
                    pvDistance.second.push_back(dNext22);
                if(bPrint == true) {
                    cout << "size: " << pvDistance.second.size() << endl;
                }
            }
            if(bPrint == true) {
                cout << "peak 1" << endl;
            }

            if(bPrint == true) {
                if(vmLFinsihedQuery[ID1][sTop.h])
                    cout << "First is true" << endl;
                else
                    cout << "First is false" << endl;

                if(vmLFinsihedQuery[ID2][sTop.h])
                    cout << "Second is true" << endl;
                else
                    cout << "Second is false" << endl;

                cout << "p11 + 1: " << p11 + 1 << " pvDistance.first.size(): " << pvDistance.first.size() << endl;
                cout << "visited: " << visited[sTop.h].size() << endl;
            }



            if (p11 + 1 < pvDistance.first.size() && visited[sTop.h].find({p11 + 1, p12}) == visited[sTop.h].end()) {
                subH sh1;
                sh1.d = pvDistance.first[p11 + 1]+pvDistance.second[p12];
                sh1.d1 = pvDistance.first[p11+1];
                sh1.d2 = pvDistance.second[p12];
                if(bPrint == true) {
                    cout << "sh1.d: " << sh1.d << endl;
                }
//                cout << "sh1.d: " << sh1.d << endl;
                sh1.h = sTop.h;
                sh1.k1 = p11+1;
                sh1.k2 = p12;
                hQ.push(sh1);
                visited[sTop.h].insert({p11 + 1, p12});
            }
            if (p12 + 1 < pvDistance.second.size() && visited[sTop.h].find({p11, p12 + 1}) == visited[sTop.h].end()) {
                subH sh1;
                sh1.d = pvDistance.first[p11]+pvDistance.second[p12+1];
                sh1.d1 = pvDistance.first[p11];
                sh1.d2 = pvDistance.second[p12+1];
                if(bPrint == true) {
                    cout << "sh1.d: " << sh1.d << endl;
                }
//                cout << "sh1.d: " << sh1.d << endl;
                sh1.h = sTop.h;
                sh1.k1 = p11;
                sh1.k2 = p12+1;
                hQ.push(sh1);
                visited[sTop.h].insert({p11, p12 + 1});
            }
        }
        return vK[k];
    }
    else{

        auto it = vmLEPos[ID1].find(Tree[LCA].uniqueVertex);
        if(it == vmLEPos[ID1].end())
        {
//            cout << "No label " << endl;
            return -1;
        }

        if(bPrint == true) {
            for(auto& id:vvLEQuery[ID1][(*it).second].vA)
                cout << id <<"\t";
            cout << endl;
            for(auto& id:vvLEQuery[ID1][(*it).second].vDistance)
                cout << id <<"\t";
            cout << endl;

            cout << "vk Size:" << vvLEQuery[ID1][(*it).second].vK.size() << endl;//For path retrievalvK.size() << endl;
            for(auto& id:vvLEQuery[ID1][(*it).second].vK)
                cout << id <<"\t";
            cout << endl;
        }
        if(vmLFinsihedQuery[ID1][ID2])
        {
            if(bPrint == true) {
                cout << "Finished Enumeration!" << endl;
            }
            if(k < vvLEQuery[ID1][(*it).second].vK.size())
                return vvLEQuery[ID1][(*it).second].vK[k];
            else
                return -1;
        }
        if(k == 0){
            vvLEQuery[ID1][(*it).second].KSPNumber = 0;
        }
        if(k < vvLEQuery[ID1][(*it).second].vK.size())
        {
            if(bPrint == true) {
                cout << "A: " <<  k << "\t" <<  vvLEQuery[ID1][(*it).second].vK.size() << "\t" << vvLEQuery[ID1][(*it).second].vKW.size() << endl;
                cout << "return" << endl;
            }
            return vvLEQuery[ID1][(*it).second].vK[k];
        }
        int dNext;
        while(k >= vvLEQuery[ID1][(*it).second].vK.size())
        {
            t2 = std::chrono::high_resolution_clock::now();
            time_span = std::chrono::duration_cast<std::chrono::duration<double> >(t2 - t1);
            if(time_span.count() > 200){
                return -1;
            }
            if(bPrint == true) {
                cout << "k: " << k << endl;
                cout << "vK size:" << vvLEQuery[ID1][(*it).second].vK.size() << endl;
            }

            labelEnum& le = vvLEQuery[ID1][(*it).second];
            if(le.Q.empty())
            {
                le.bFinished = true;
                vmLFinsihedQuery[ID1][ID2] = true;
            }

            if(vmLFinsihedQuery[ID1][ID2])
            {
//                cout << "label Has finished Update" << endl;
                //When the last w has finished enumeration
                if(le.Q.empty())
                {
                    le.bFinished = true;
                    vmLFinsihedQuery[ID1][ID2] = true;
                }
                if(k < vvLEQuery[ID1][(*it).second].vK.size())
                    return vvLEQuery[ID1][(*it).second].vK[k];
                else
                    return -1;
            }

            subA sTop = le.Q.top();
            if(bPrint == true) {
                cout << "Generate New dNext" << endl;
                cout << "Qsize:" << le.Q.size() << endl;
            }
            le.Q.pop();
//            vvLEQuery[ID1][(*it).second].Q.pop();
            int a = sTop.a;
            int aPos = le.umAPos[a];
            dNext = sTop.d;
//            cout << "peak: " << "ID1: " << ID1 << " a: " << a << " ID2: " << ID2 << endl;
//            cout << "sTop.k1: " << sTop.k1 << " sTop.k2: " << sTop.k2 << endl;
            if(bPrint == true) {
                cout << "Top Dist:" << dNext << " via Node: " << a << endl;
            }
            le.vK.push_back(dNext);
            le.vKW.push_back(a);

            subA pTop2 = le.Q.top();
            int dTop2 = pTop2.d;

            if(bPrint == true) {
                cout << "dNext: " << dNext << " a: " << a << " sTop.k1: " << sTop.k1 << " sTop.k2: " << sTop.k2 << endl;
            }
            le.vSubA.push_back(make_pair(sTop.k1, sTop.k2));

            vector<int> x1;
            x1.push_back(ID1);
            x1.push_back(ID2);
            if(a != ID2 ){
                x1.insert(x1.begin()+1,a);
            }
            le.vNode.push_back(x1);

            /*string s1;
            s1 = to_string(ID1)+"-";
            s1 = s1 + to_string(ID2)+"-";
            if(a != ID2 ){
                auto ID2pos = s1.find("-"+to_string(ID2)+"-")+1;
                s1.insert(ID2pos, to_string(a)+"-");
            }
            le.vPathstr.push_back(s1);*/
//            cout << "Begin le.vPathstr: " << le.vPathstr[k] << endl;

            vector<int> x2;
            x2.push_back(rank[ID1]);
            if(a != ID2 ){
                x2.push_back(rank[a]);
            }
            sort(x2.begin(), x2.end());

            le.rankSet1.push_back(x2);

            vector<int> x3;
            if(a != ID2 ){
                x3.push_back(rank[a]);
            }
            x3.push_back(rank[ID2]);
            le.rankSet2.push_back(x3);

            vector<int> x4;
            x4.push_back(rank[ID1]);
            x4.push_back(rank[ID2]);
            if(a != ID2 ){
                x4.push_back(rank[a]);
            }
            sort(x4.begin(), x4.end());
            le.vRank.push_back(x4);
            le.pathTotal.push_back(rank[ID1]+rank[ID2]+rank[a]);

            bool bLoop = false;
            bool bRepeat = false;
//            le.bLoop.push_back(bLoop);
            if(bPrint == true) {
//                cout << "k: " << k << " loop size: " << le.bLoop.size() << endl;

                cout << "new vk Size:" << le.vK.size() << endl;
                for(auto& id:vvLEQuery[ID1][(*it).second].vK)
                    cout << id <<"\t";
                cout << endl;
                cout << "a: " << a << endl;
                cout << "bSE: " << sTop.bSE << endl;
            }
            auto& pvDistance = le.vpvADistance[aPos];
            if(bPrint == true) {
                cout << "peak pvDistance first size: " << pvDistance.first.size() << endl;
                for (int i = 0; i < pvDistance.first.size(); ++i) {
                    cout << pvDistance.first[i] << " ";
                }
                cout << endl;
                cout << "peak pvDistance second size: " << pvDistance.second.size() << endl;
                for (int i = 0; i < pvDistance.second.size(); ++i) {
                    cout << pvDistance.second[i] << " ";
                }
                cout << endl;

                for(int i = 0; i < le.vNode.size(); i++){
                    cout << "peak vNode ii: " << i << endl;
                    for(int j = 0; j < le.vNode[i].size(); j++){
                        cout << le.vNode[i][j] << " ";
                    }
                    cout << endl;
                }
                cout << endl;
            }




            if(a == ID2)
            {
//                cout << "a == ID2" << " ID1: " << ID1 << " ID2: " << ID2 << endl;
//                le.vchaPath.push_back(0);
                if(bPrint == true) {
                    cout << " a is ID2" << endl;
                }
                if(sTop.k1 == 0){
                    int d11Next = caveKSP(a, ID1, 0, INF, INF, INF, true, true);
                }

                /*bRepeat = false;
                bLoop = false;*/

                int d11 = INF;
                int d12 = INF;
                auto it11 = vmSEPos[a].find(ID1);
//                cout << "a: " << a << " ID1: " << ID1 << endl;
                if(it11 != vmSEPos[a].end())
                {
                    if(bPrint == true) {
                        cout << "sTop.k1: " << sTop.k1  << endl;
                        cout << "vvSEQuery[a][(*it).second].vNode: " << vvSEQuery[a][(*it11).second].vNode.size() << endl;
                    }
//                    cout << "vvSEQuery[a][(*it11).second].vNode[sTop.k1] size: " << vvSEQuery[a][(*it11).second].vNode[sTop.k1].size() << " vvSEQuery[a][(*it11).second].vPathstr[sTop.k1].size(): " << vvSEQuery[a][(*it11).second].vPathstr[sTop.k1].size() << endl;
                    for(int i = 0; i < vvSEQuery[a][(*it11).second].vNode[sTop.k1].size(); i++){
                        le.vNode[k].insert(le.vNode[k].begin()+1,vvSEQuery[a][(*it11).second].vNode[sTop.k1][i]);
//                        cout << vvSEQuery[a][(*it11).second].vNode[sTop.k1][i] << " ";
                        /*auto firstDash = le.vPathstr[k].find("-");
                        le.vPathstr[k].insert(firstDash+1, to_string(vvSEQuery[a][(*it11).second].vNode[sTop.k1][i])+"-");*/
                        if(bPrint == true) {
                            cout << vvSEQuery[a][(*it11).second].vNode[sTop.k1][i] << " ";
                        }
                    }
                    le.vRank[k] = vvSEQuery[a][(*it11).second].vRank[sTop.k1];
                    le.pathTotal[k] = vvSEQuery[a][(*it11).second].pathTotal[sTop.k1];
                    if(bPrint == true) {
                        cout << endl;
                    }
                    int w = vvSEQuery[a][(*it11).second].vKW[sTop.k1];
                    int wPos = vvSEQuery[a][(*it11).second].umWPos[w];

//                    cout << vvSEQuery[a][(*it11).second].vpvWDistance[wPos].first.size() << endl;
                    auto& sepvDistance = vvSEQuery[a][(*it11).second].vpvWDistance[wPos];
                    int d11pos = vvSEQuery[a][(*it11).second].vSubK[sTop.k1].first;
                    int d12pos = vvSEQuery[a][(*it11).second].vSubK[sTop.k1].second;
//                    cout << sepvDistance.first.size() << endl;
                    d11 = sepvDistance.first[d11pos];
                    d12 = sepvDistance.second[d12pos];
                    if(vvSEQuery[a][(*it11).second].sebLoop[sTop.k1] == true){
                        bLoop = true;
                    }
//                    cout << vvSEQuery[a][(*it11).second].sebLoop[sTop.k1] << endl;
                }

                uint64_t vPathHash = XXH64(le.vNode[k].data(), le.vNode[k].size() * sizeof(int), 0);
                if(k > 0){
                    if(le.vK[k] != le.vK[k-1]){
                        le.hashSet.clear();
                    }
                    else{
                        for(int i = 1; i < le.hashSet.size()+1; i++){
                            if(le.vNode[k-i].size() == le.vNode[k].size() && le.pathTotal[k-i] == le.pathTotal[k]){
                                if (le.hashSet.find(vPathHash) != le.hashSet.end()) {
                                    bRepeat = true;
                                    break;
                                }
                            }
                        }
                    }
                }
                /*for(int i = 0; i < le.vNode[k].size(); i++){
                    cout << le.vNode[k][i] << " ";
                }
                cout << endl;*/

                if(bLoop == true || bRepeat == true){
//                    cout << "ID1: " << ID1 << " ID2: " << ID2 << " a: " << a << " a==ID2 true" << " bLoop: " << bLoop << " bRepeat: " << bRepeat << " length: " << le.vK[le.vK.size()-1] << endl;
                    le.vK.pop_back();
                    KSPNumber++;
                    le.vKW.pop_back();
//                    le.vPathstr.pop_back();
                    le.vSubA.pop_back();
                    le.vNode.pop_back();
                    le.rankSet1.pop_back();
                    le.rankSet2.pop_back();
                    le.vRank.pop_back();
                    le.pathTotal.pop_back();
                }
                else{
//                    le.uniquePath.insert(le.vPathstr[k]);
//                    le.uniquePath.insert(pathStr);
                    le.hashSet.insert(vPathHash);
                }
                /*if(ID1 == 25063 && ID2 == 187636 && k == 100){
                    cout << "Q test!" << endl;
                    cout << "vK size: " << le.vK.size() << " vK[vK.size()-1]: " << le.vK[le.vK.size()-1] << endl;
                    while(!le.Q.empty()){
                        subA sTop1 = le.Q.top();
                        cout << "a: " << sTop1.a << " d: " << sTop1.d << " sTop1.k1: " << sTop1.k1 << " sTop1.k2: " << sTop1.k2 << " sTop1.d1: " << sTop1.d1 << " sTop1.d2: " << sTop1.d2 << endl;
                        le.Q.pop();
                    }
                    return -1;
                }*/


                int d11Next = caveKSP(a, ID1, sTop.k1+1, d11, d12, dTop2, true, true);
//                cout << "pesk d11Next caveKSP a == ID2: " << d11Next << " bLoop: " << vvSEQuery[a][(*it11).second].sebLoop[sTop.k1+1] << endl;
//                cout << "valley a : " << vvSEQuery[a][(*it11).second].vKW[sTop.k1+1] << endl;
                if(bPrint == true) {
                    cout << "pesk d11Next caveKSP a == ID2: " << d11Next << endl;
                }


                if(d11Next == -1)
                {
                    vmEFinsihedQuery[a][ID1] = true;
                    le.vpbFinished[aPos].first = true;
                }
                else{
                    if(sTop.k1+1 <= le.vK.size()*100) {
                        subA sa;
                        sa.d = d11Next;
                        sa.a = a;
                        sa.k1 = sTop.k1 + 1;
//                    sa.k2 = sTop.k2;
                        sa.k2 = -1;
                        if (sa.d < le.vK[le.vK.size() - 1]) {
                            cout << "1: sa.d < le.vK[le.vK.size()-1]: " << sa.d << " ID1: " << ID1 << " ID2: " << ID2
                                 << endl;
                            for (int mm = 0; mm < vvSEQuery[a][(*it11).second].vK.size(); mm++) {
                                cout << vvSEQuery[a][(*it11).second].vK[mm] << " ";
                            }
                            cout << endl;
                            cout << "p11: " << sTop.k1 << " p12: " << sTop.k2 << endl;
                            for (int i = 0; i < pvDistance.first.size(); i++) {
                                cout << pvDistance.first[i] << " ";
                            }
                            cout << endl;
                            for (int j = 0; j < pvDistance.second.size(); j++) {
                                cout << pvDistance.first[j] << " ";
                            }
                            cout << endl;
                        }
                        le.Q.push(sa);
                        if (bPrint == true) {
                            cout << "insert into peak Q" << "d: " << sa.d << " sa.k1: " << sa.k1 << " sa.k2: " << sa.k2
                                 << endl;
                        }
                        pvDistance.first.push_back(d11Next);
                    }
                }
                //change
                if(le.Q.empty())
                {
                    le.bFinished = true;
                    vmLFinsihedQuery[ID1][ID2] = true;
                    if(bPrint == true) {
                        cout << "le.Q empty!" << endl;
                    }
                }

                continue;
            }

            /*le.rankSet1[k].push_back(rank[ID1]);
            le.rankSet2[k].push_back(rank[ID2]);*/

            int r2 = NodeOrder[ID1];
            int r3 = NodeOrder[a];
            int r4 = NodeOrder[ID2];
           /* cout << "ID1: " << ID1 << " NodeOrder ID1: " << NodeOrder[ID1] << " rank ID1: " << rank[ID1]  << " ID2: " << ID2 << " NodeOrder ID2: " <<  NodeOrder[ID2] << " rank ID2: " << rank[ID2] << endl;
            cout << "a: " << a << " NodeOrder a: " << NodeOrder[a] <<" rank a: " << rank[a] << endl;*/
            if(r3 > r4){
                auto it1 = vmLEPos[ID2].find(a);
                if(it1 == vmLEPos[ID2].end())
                {
//                    cout << "No Label" << endl;
                    return -1;
                }
            }
            else{
                auto it1 = vmLEPos[a].find(ID2);
                if(it1 == vmLEPos[a].end())
                {
                    return -1;
                }
            }

            int p11 = sTop.k1;
            int p12 = sTop.k2;
            if(bPrint == true) {
                cout << "(" << p11 <<"," << p12 <<")" << endl;
            }
            int d1Next, d2Next, d2CaveNext;



            int caveID1 = ID1;
            int caveID2 = a;
            bool bcaveForward = true;
            if(NodeOrder[ID1] < NodeOrder[a])
            {
                bcaveForward = false;
                int IDTmp = ID1;
                caveID1 = a;
                caveID2 = IDTmp;
            }

            if(bPrint == true) {
                cout << pvDistance.first[p11] << "\t" << pvDistance.second[p12] << endl;
                cout << "pvDistance first: " << pvDistance.first[p11] << " pvDistance second: " << pvDistance.second[p12] << endl;
            }

            auto it12 = vmSEPos[caveID1].find(caveID2);
            if(p11 == pvDistance.first.size()-1)
            {
                if(bPrint == true) {
                    cout << "p12 is 0, p11 at the end" << endl;
                }
                if(p11 == 0)
                {
                    caveKSP(caveID1, caveID2, 0, INF, INF, INF, true, true);
                }
                int d11 = INF;
                int d12 = INF;
                if(it12 != vmSEPos[caveID1].end())
                {
                    int w = vvSEQuery[caveID1][(*it12).second].vKW[p11];
                    int wPos = vvSEQuery[caveID1][(*it12).second].umWPos[w];
                    auto& sepvDistance = vvSEQuery[caveID1][(*it12).second].vpvWDistance[wPos];
                    int d11pos = vvSEQuery[caveID1][(*it12).second].vSubK[p11].first;
                    int d12pos = vvSEQuery[caveID1][(*it12).second].vSubK[p11].second;
                    d11 = sepvDistance.first[d11pos];
                    d12 = sepvDistance.second[d12pos];
                    /*if(vvSEQuery[caveID1][(*it12).second].sebLoop[p11] == true){
                        bLoop = true;
                    }*/
                }
//                cout << "caveID1:" << caveID1 << " caveID2: " << caveID2 << " k: " << p11 << " bLoop: " << bLoop << endl;

                d1Next = caveKSP(caveID1, caveID2, p11+1, d11, d12, dTop2, true, true);
                if(bPrint == true) {
                    cout << "peak cave d1Next: " << d1Next << endl;
                }
//                cout << "peak cave" << " caveID1: " << caveID1 << " caveID2: " << caveID2 << " d1Next: " << d1Next << endl;
                if(d1Next == -1)
                {
                    if(r2 < r3){
                        vmEFinsihedQuery[a][ID1] = true;
                    }
                    else{
                        vmEFinsihedQuery[ID1][a] = true;
                    }
                    le.vpbFinished[aPos].first = true;
                }
                else
                    pvDistance.first.push_back(d1Next);

                if(bPrint == true) {
                    cout << "size: " << pvDistance.first.size() << endl;
                }
            }


            if(it12 != vmSEPos[caveID1].end())
            {
                if(bPrint == true) {
                    cout << "p11: " << p11 << " vvSEQuery[caveID1][(*it12).second].vNode: " << vvSEQuery[caveID1][(*it12).second].vNode.size() << endl;
                }
                if(caveID1 == a){
                    for(int i = 0; i < vvSEQuery[caveID1][(*it12).second].vNode[p11].size(); i++){
                        le.vNode[k].insert(le.vNode[k].begin()+1, vvSEQuery[caveID1][(*it12).second].vNode[p11][i]);
                        /*auto firstDash = le.vPathstr[k].find("-");
                        le.vPathstr[k].insert(firstDash+1, to_string(vvSEQuery[caveID1][(*it12).second].vNode[p11][i])+"-");*/
                        if(bPrint == true) {
                            cout << vvSEQuery[caveID1][(*it12).second].vNode[p11][i] << " ";
                        }
//                        le.rankSet1[k].push_back(rank[vvSEQuery[caveID1][(*it12).second].vNode[p11][i]]);
                    }
                    le.rankSet1[k] = vvSEQuery[caveID1][(*it12).second].vRank[p11];
                    if(bPrint == true) {
                        cout << endl;
                    }
                }
                else{
                    for(int i = 0; i < vvSEQuery[caveID1][(*it12).second].vNode[p11].size(); i++){
                        le.vNode[k].insert(le.vNode[k].end()-2, vvSEQuery[caveID1][(*it12).second].vNode[p11][i]);
                        /*auto astrPos = le.vPathstr[k].find("-"+to_string(a)+"-")+1;
                        le.vPathstr[k].insert(astrPos, to_string(vvSEQuery[caveID1][(*it12).second].vNode[p11][i])+"-");*/
                        if(bPrint == true) {
                            cout << vvSEQuery[caveID1][(*it12).second].vNode[p11][i] << " ";
                        }
//                        le.rankSet1[k].push_back(rank[vvSEQuery[caveID1][(*it12).second].vNode[p11][i]]);
                    }
                    le.rankSet1[k] = vvSEQuery[caveID1][(*it12).second].vRank[p11];
                    if(bPrint == true) {
                        cout << endl;
                    }
                }
                if(vvSEQuery[caveID1][(*it12).second].sebLoop[p11] == true){
                    bLoop = true;
                }
            }


            if(p12 == pvDistance.second.size()-1)
            {
//                cout << "p12 at the end" << endl;
                if(bPrint == true) {
                    cout << "p12 at the end" << endl;
                }
                if(p12 == 0)
                {
                    peakKSP(a,ID2,0,false);
                }

                d2Next = peakKSP(a,ID2,p12+1,false);
//                cout << "peak peak" << " a: " << a << " ID2: " << ID2 << " d2Next: " << d2Next << endl;
                if(bPrint == true) {
                    cout << "peak peak dNext22: " << d2Next << endl;
                }

                if(d2Next == -1)
                {
                    if(r3 > r4){
                        vmLFinsihedQuery[ID2][a] = true;
                    }
                    else{
                        vmLFinsihedQuery[a][ID2] = true;
                    }
                    le.vpbFinished[aPos].second = true;
                }
                else{
                    pvDistance.second.push_back(d2Next);
                }
                if(bPrint == true) {
                    cout << "size: " << pvDistance.second.size() << endl;
                }

            }
            if(bPrint == true) {
                cout << "peak add" << endl;
                cout << "a: " << a << " ID2: " << ID2 << endl;
            }
            if(r3 > r4){
                auto it13 = vmLEPos[ID2].find(a);
                if(it13 != vmLEPos[ID2].end())
                {
                    for(int i = vvLEQuery[ID2][(*it13).second].vNode[p12].size()-2; i >=1; i--){
                        le.vNode[k].insert(le.vNode[k].end()-1, vvLEQuery[ID2][(*it13).second].vNode[p12][i]);
//                        cout << k << " " << le.rankSet2.size() << " " << le.vNode.size()<< endl;
//                        le.rankSet2[k].push_back(rank[vvLEQuery[ID2][(*it13).second].vNode[p12][i]]);
//                        cout << "test4" << endl;
                        /*auto finalItem = le.vPathstr[k].find("-"+to_string(ID2)+"-")+1;
//                        cout << "final:"<< finalItem << endl;
                        le.vPathstr[k].insert(finalItem, to_string(vvLEQuery[ID2][(*it13).second].vNode[p12][i])+"-");*/
//                        cout << "le.vPathstr:" <<  le.vPathstr[k] << endl;
                    }
                    le.rankSet2[k] = vvLEQuery[ID2][(*it13).second].vRank[p12];
                    if(bPrint == true){
                        cout << endl;
                    }
                }
            }
            else{
                auto it13 = vmLEPos[a].find(ID2);
                if(it13 != vmLEPos[a].end())
                {
                    if(bPrint == true) {
                        cout << "1 True" << endl;
                    }
                    le.vNode[k].insert(le.vNode[k].end()-1, vvLEQuery[a][(*it13).second].vNode[p12].begin()+1, vvLEQuery[a][(*it13).second].vNode[p12].end()-1);
                    le.rankSet2[k] = vvLEQuery[a][(*it13).second].vRank[p12];
                }
            }
            //If loop?
            if(bPrint == true) {
                for(int i = 0; i < le.vNode[k].size(); i++){
                    cout << le.vNode[k][i] << " ";
                }
                cout << endl;
            }

            auto it111 = le.rankSet1[k].begin();
            auto it222 = le.rankSet2[k].begin();

            if(bLoop == false){
                bool aAdded = false;
                le.vRank[k].clear();
                le.pathTotal[k] = 0;
                while (it111 != le.rankSet1[k].end() && it222 != le.rankSet2[k].end()) {
                    if (*it111 == *it222) {
                        if(*it111 == rank[a] && !aAdded){
                            aAdded = true;
                            le.vRank[k].push_back(*it111);
                            le.pathTotal[k] += *it111;
                            ++it111;
                            ++it222;
                        }
                        else{
                            bLoop = true;
                            break;
                        }
                    } else if (*it111 < *it222) {
                        le.vRank[k].push_back(*it111);
                        le.pathTotal[k] += *it111;
                        ++it111;
                    } else {
                        le.vRank[k].push_back(*it222);
                        le.pathTotal[k] += *it222;
                        ++it222;
                    }
                }

                while (it111 != le.rankSet1[k].end()) {
                    le.vRank[k].push_back(*it111);
                    le.pathTotal[k] += *it111;
                    ++it111;
                }

                while (it222 != le.rankSet2[k].end()) {
                    le.vRank[k].push_back(*it222);
                    le.pathTotal[k] += *it222;
                    ++it222;
                }
            }

            uint64_t vPathHash = XXH64(le.vNode[k].data(), le.vNode[k].size() * sizeof(int), 0);
            if(k > 0){
                if(le.vK[k] != le.vK[k-1]){
                    le.hashSet.clear();
                }
                else{
                    for(int i = 1; i < le.hashSet.size()+1; i++){
                        if(le.vNode[k-i].size() == le.vNode[k].size() && le.pathTotal[k-i] == le.pathTotal[k]){
                            if (le.hashSet.find(vPathHash) != le.hashSet.end()) {
                                bRepeat = true;
                                break;
                            }
                        }
                    }
                }
            }
            /*for(int i = 0; i < le.vNode[k].size(); i++){
                cout << le.vNode[k][i] << " ";
            }
            cout << endl;*/
            /*if(ID1 == 25089 && ID2 == 187636 && a == 25063){
                for(int i = 0; i < le.vNode[k].size(); i++){
                    cout << le.vNode[k][i] << " ";
                }
                cout << endl;
            }
*/
            if(bLoop == true || bRepeat == true){
//                cout << "ID1: " << ID1 << " ID2: " << ID2 << " a: " << a << " le true" << " bLoop: " << bLoop << " bRepeat: " << bRepeat << " length: " << le.vK[le.vK.size()-1] << endl;
                /*for(int i = 0; i < le.vNode[le.vNode.size()-1].size(); i++){
                    cout << le.vNode[le.vNode.size()-1][i] << " ";
                }
                cout << endl;*/
                le.vK.pop_back();
                KSPNumber++;
//                le.vPathstr.pop_back();
                le.vKW.pop_back();
                le.vRank.pop_back();
                le.pathTotal.pop_back();
                le.vSubA.pop_back();
                le.vNode.pop_back();
                le.rankSet1.pop_back();
                le.rankSet2.pop_back();
            }
            else{
                le.hashSet.insert(vPathHash);
            }

           /* if(ID1 == 25089 && ID2 == 187636){
                cout << "Q test!" << endl;
                cout << "vK size: " << le.vK.size() << " vK[vK.size()-1]: " << le.vK[le.vK.size()-1] << endl;
                while(!le.Q.empty()){
                    subA sTop1 = le.Q.top();
                    cout << "a: " << sTop1.a << " d: " << sTop1.d << " sTop1.k1: " << sTop1.k1 << " sTop1.k2: " << sTop1.k2 << " sTop1.d1: " << sTop1.d1 << " sTop1.d2: " << sTop1.d2 << endl;
                    le.Q.pop();
                }
                return -1;
            }*/



            /*if(r3 > r4){
                vmLFinsihedQueryTmp = vmLFinsihedQuery[ID2][a];
            }
            else{
                vmLFinsihedQueryTmp = vmLFinsihedQuery[a][ID2];
            }

            if(r2 < r3){
                vmEFinsihedQueryTmp = vmEFinsihedQuery[a][ID1];
            }
            else{
                vmEFinsihedQueryTmp = vmEFinsihedQuery[ID1][a];
            }

            if(bPrint == true) {
                cout << "peak 2" << endl;
                if(vmEFinsihedQueryTmp)
                    cout << "First is true" << endl;
                else
                    cout << "First is false" << endl;

                if(vmLFinsihedQueryTmp)
                    cout << "Second is true" << endl;
                else
                    cout << "Second is false" << endl;

                cout << "le.visited[aPos]: " << endl;
                cout  << le.visited[aPos].size() << endl;
            }*/
            if(d1Next != -1){
//                if(vvSEQuery[caveID1][(*it12).second].sebLoop[p11+1] == false && (p11 <= le.vK.size()*10)){
                if(p11+1 <= le.vK.size()*100){
                    if (p11 + 1 < pvDistance.first.size() && le.visited[aPos].find({p11 + 1, p12}) == le.visited[aPos].end()) {

                        subA sa;
                        sa.d = pvDistance.first[p11 + 1]+pvDistance.second[p12];
                        sa.a = a;
                        sa.k1 = p11+1;
                        sa.k2 = p12;
                        if(bPrint == true) {
                            cout << "sa.k1: " << sa.k1 << " sa.k2: " << sa.k2 << " sa.d: " << sa.d << " pvDistance.first[p11 + 1]: " << pvDistance.first[p11 + 1] << " pvDistance.second[p12]: " << pvDistance.second[p12] << endl;
                        }
                        if(sa.d < le.vK[le.vK.size()-1]){
                            cout << "2: sa.d < le.vK[le.vK.size()-1]: " << sa.d << " ID1: " << ID1 << " ID2: " << ID2 << endl;
                            cout << "p11: " << p11 << " p12: " << p12 << endl;
                            for(int i = 0; i < pvDistance.first.size(); i++){
                                cout << pvDistance.first[i] << " ";
                            }
                            cout << endl;
                            for(int j = 0; j < pvDistance.second.size(); j++){
                                cout << pvDistance.first[j] << " ";
                            }
                            cout << endl;
                        }
                        le.Q.push(sa);
                        le.visited[aPos].insert({p11 + 1, p12});
                    }
                }
            }

            /*if (p11 + 1 < pvDistance.first.size() && le.visited[aPos].find({p11 + 1, p12}) == le.visited[aPos].end()) {

                subA sa;
                sa.d = pvDistance.first[p11 + 1]+pvDistance.second[p12];
                sa.a = a;
                sa.k1 = p11+1;
                sa.k2 = p12;
                if(bPrint == true) {
                    cout << "sa.k1: " << sa.k1 << " sa.k2: " << sa.k2 << " sa.d: " << sa.d << " pvDistance.first[p11 + 1]: " << pvDistance.first[p11 + 1] << " pvDistance.second[p12]: " << pvDistance.second[p12] << endl;
                }
                if(sa.d < le.vK[le.vK.size()-1]){
                    cout << "2: sa.d < le.vK[le.vK.size()-1]: " << sa.d << " ID1: " << ID1 << " ID2: " << ID2 << endl;
                    cout << "p11: " << p11 << " p12: " << p12 << endl;
                    for(int i = 0; i < pvDistance.first.size(); i++){
                        cout << pvDistance.first[i] << " ";
                    }
                    cout << endl;
                    for(int j = 0; j < pvDistance.second.size(); j++){
                        cout << pvDistance.first[j] << " ";
                    }
                    cout << endl;
                }
                le.Q.push(sa);
                le.visited[aPos].insert({p11 + 1, p12});
            }*/

           /* if(r3 > r4){
                auto itNext11 = vmLEPos[ID2].find(a);
                if(d2Next != -1){
                    if(vvLEQuery[ID2][(*itNext11).second].vKW[p12+1] == a){
                        auto itNext2 = vmSEPos[a].find(ID2);
                        cout << " ID1: " << ID1 << " a: " << a << " ID2: " << ID2 << " vvLEQuery[ID2][(*itNext11).second].vK.size(): " << vvLEQuery[ID2][(*itNext11).second].vK.size() << " le.vK.size(): " << le.vK.size() << endl;
                        cout << "Q size: " << le.Q.size() << endl;
//                        if(vvSEQuery[a][(*itNext2).second].sebLoop[vvLEQuery[ID2][(*itNext11).second].vSubA[p12+1].first] == false && vvSEQuery[a][(*itNext2).second].vK.size() > 100000 && le.vK.size() < 10){
//                        if(vvSEQuery[a][(*itNext2).second].vK.size() > 10000 && le.vK.size() < 10000){
                        if(vvLEQuery[ID2][(*itNext11).second].vK.size() > le.vK.size()*10 || vvSEQuery[a][(*itNext2).second].vK.size() > vvLEQuery[ID2][(*itNext11).second].vK.size()*100){
                            cout << "Here!" << endl;
                            if(bLoop == true){
                                continue;
                            }
                        }
                    }
                }
            }
            else{
                auto itNext11 = vmLEPos[a].find(ID2);
                if(d2Next != -1){
                    if(vvLEQuery[a][(*itNext11).second].vKW[p12+1] == ID2){
                        auto itNext2 = vmSEPos[ID2].find(a);
                        cout << " ID1: " << ID1 << " a: " << a << " ID2: " << ID2 << " vvLEQuery[a][(*itNext11).second].vK.size(): " << vvLEQuery[a][(*itNext11).second].vK.size() << " le.vK.size(): " << le.vK.size() << endl;
                        cout << "Q size: " << le.Q.size() << endl;
//                        if(vvSEQuery[ID2][(*itNext2).second].sebLoop[vvLEQuery[a][(*itNext11).second].vSubA[p12+1].first] == false && vvSEQuery[ID2][(*itNext2).second].vK.size() > 100000 && le.vK.size() < 10){
                        if(vvLEQuery[a][(*itNext11).second].vK.size() > le.vK.size()*10 || vvSEQuery[ID2][(*itNext2).second].vK.size() > vvLEQuery[a][(*itNext11).second].vK.size()*100){
                            cout << "Here!" << endl;
                            if(bLoop == true){
                                continue;
                            }
                        }
                    }
                }
            }*/

           if(d2Next != -1){
               if(r3 > r4){
                   auto itNext11 = vmLEPos[ID2].find(a);
                   auto itNext2 = vmSEPos[a].find(ID2);
//                   cout << " ID1: " << ID1 << " a: " << a << " ID2: " << ID2 << " vvLEQuery[ID2][(*itNext11).second].vK.size(): " << vvLEQuery[ID2][(*itNext11).second].vK.size() << " le.vK.size(): " << le.vK.size() << endl;
//                   cout << "Q size: " << le.Q.size() << endl;
//                   cout << " ID1: " << ID1 << " a: " << a << " ID2: " << ID2 << " vvLEQuery[ID2][(*itNext11).second].vK.size(): " << vvLEQuery[ID2][(*itNext11).second].vK.size() << " le.vK.size(): " << le.vK.size() << endl;
//                   cout << "Q size: " << le.Q.size() << endl;
                   if(vvLEQuery[ID2][(*itNext11).second].vKW[p12+1] == a){
                       if(itNext2 != vmLEPos[a].end()) {
                           if (vvSEQuery[a][(*itNext2).second].vK.size() >
                               vvLEQuery[ID2][(*itNext11).second].vK.size() * 10) {
                               if (bLoop == true) {
//                               cout << "Here!" << endl;
                                   continue;
                               }
                           }
                       }
                   }
                   if(vvLEQuery[ID2][(*itNext11).second].vK.size() > le.vK.size()*100){
                       if(bLoop == true){
//                           cout << "Here!" << endl;
                           continue;
                       }
                   }
               }
               else{
                   auto itNext11 = vmLEPos[a].find(ID2);
                   auto itNext2 = vmSEPos[ID2].find(a);
//                   cout << " ID1: " << ID1 << " a: " << a << " ID2: " << ID2 << " vvLEQuery[a][(*itNext11).second].vK.size(): " << vvLEQuery[a][(*itNext11).second].vK.size() << " le.vK.size(): " << le.vK.size() << endl;
//                   cout << "Q size: " << le.Q.size() << endl;
//                   cout << " ID1: " << ID1 << " a: " << a << " ID2: " << ID2 << " vvLEQuery[a][(*itNext11).second].vK.size(): " << vvLEQuery[a][(*itNext11).second].vK.size() << " le.vK.size(): " << le.vK.size() << endl;
//                   cout << "Q size: " << le.Q.size() << endl;
                   if(vvLEQuery[a][(*itNext11).second].vKW[p12+1] == ID2){
                       if(itNext2 != vmLEPos[a].end()){
                           if(vvSEQuery[ID2][(*itNext2).second].vK.size() > vvLEQuery[a][(*itNext11).second].vK.size()*100){
                               if(bLoop == true){
                                   //                               cout << "Here!" << endl;
                                   continue;
                               }
                           }
                       }
                   }
                   if(vvLEQuery[a][(*itNext11).second].vK.size() > le.vK.size()*100){
                       if(bLoop == true){
//                           cout << "Here!" << endl;
                           continue;
                       }
                   }
               }

           }

//            cout << "add p12+1 to heap" << endl;


            if (p12 + 1 < pvDistance.second.size() && le.visited[aPos].find({p11, p12 + 1}) == le.visited[aPos].end()) {
                subA sa;
                sa.d = pvDistance.first[p11]+pvDistance.second[p12+1];
                sa.a = a;
                sa.k1 = p11;
                sa.k2 = p12+1;
                if(bPrint == true) {
                    cout << "sa.k1: " << sa.k1 << " sa.k2: " << sa.k2 << " sa.d: " << sa.d << " pvDistance.first[p11]: " << pvDistance.first[p11] << " pvDistance.second[p12 + 1]: " << pvDistance.second[p12 + 1] << endl;
                }
                if(sa.d < le.vK[le.vK.size()-1]){
                    cout << "3: sa.d < le.vK[le.vK.size()-1]: " << sa.d << " ID1: " << ID1 << " ID2: " << ID2 << endl;
                    cout << "p11: " << p11 << " p12: " << p12 << endl;
                    for(int i = 0; i < pvDistance.first.size(); i++){
                        cout << pvDistance.first[i] << " ";
                    }
                    cout << endl;
                    for(int j = 0; j < pvDistance.second.size(); j++){
                        cout << pvDistance.second[j] << " ";
                    }
                    cout << endl;
                }
                le.Q.push(sa);
                le.visited[aPos].insert({p11, p12+1});
            }


        }
        return vvLEQuery[ID1][(*it).second].vK[k];
    }
}

void Graph::peakRetriveal(int ID1, int ID2, int k, list<int>& lPath)
{
//    cout << "ID1: " << ID1 << " ID2: " << ID2 << " k: " << k << endl;
    int o1 = NodeOrder[ID1];
    int o2 = NodeOrder[ID2];
    bool bReverse = false;
    if(o1 > o2){
        bReverse = true;
        int tmp = ID1;
        ID1 = ID2;
        ID2 = tmp;
    }
    lPath.push_back(ID1);
    lPath.push_back(ID2);
    auto it1 = lPath.begin();
    auto it2 = it1;
    it2++;
    int r1 = rank[ID1];
    int r2 = rank[ID2];
    int LCA = LCAQuery(r1,r2);
    if(bPrint == true) {
        cout << "ID1: " << ID1 << " ID2: " << ID2 << " k: " << k << "r1: " << r1 << " r2: " << r2 << " LCA: " << LCA << endl;
    }
//    cout << "ID1: " << ID1 << " ID2: " << ID2 << " k: " << k << "r1: " << r1 << " r2: " << r2 << " LCA: " << LCA << endl;
    queue<labelPath> qLE;
    labelPath leP,lePLeft,lePRight;
    if (!(LCA == r1 || LCA == r2)){
        int a;
        a = vKW[k];
        if(bPrint == true) {
            cout << "vKW a: " << a << endl;
        }
//        cout << "vKW a: " << a << endl;
        it1++;
        it1 = lPath.insert(it1,a);
        it1--;
        auto itLeft = vmLEPos[ID1].find(a);
        lePLeft.vex = &vvLEQuery[ID1][(*itLeft).second];
        if (vKW.size() == 0)
            lePLeft.k = 0;
        else
            lePLeft.k = vSubA[k].first;
        if(bPrint == true) {
            cout << "k: " << k << " vKW size: " << vKW.size() << " lePLeft.k: " << lePLeft.k << endl;
        }
//        cout << "k: " << k << " vKW size: " << vKW.size() << " lePLeft.k: " << lePLeft.k << endl;
//        cout << "lePLeft.k: " << lePLeft.k << endl;
        if(lePLeft.k != -1)
        {
            lePLeft.bContinue = false;
            lePLeft.it1 = it1;
            lePLeft.it2 = lePLeft.it1;
            lePLeft.it2++;
//            cout << "lePLeft.it1: " << *(lePLeft.it1) << " lePLeft.it2: " << *(lePLeft.it2) << endl;
            lePLeft.bForward = true;
            qLE.push(lePLeft);
        }
        auto itRight = vmLEPos[ID2].find(a);
        lePRight.vex = &vvLEQuery[ID2][(*itRight).second];
        if (vKW.size() == 0)
            lePRight.k = 0;
        else
            lePRight.k = vSubA[k].second;
//        cout << "lePRight.k: " << lePRight.k << endl;
        if(lePRight.k != -1)
        {
            lePRight.bContinue = false;
            lePRight.it1 = it2;
            lePRight.it2 = lePRight.it1;
            lePRight.it2--;
//            cout << "lePRight.it1: " << *(lePRight.it1) << " lePRight.it2: " << *(lePRight.it2) << endl;
            lePRight.bForward = false;
            qLE.push(lePRight);
        }
    }
    else{
        auto it = vmLEPos[ID1].find(ID2);
        leP.bContinue = false;
        leP.vex = &vvLEQuery[ID1][(*it).second];
        leP.k = k;
        leP.it1 = it1;
        leP.it2 = it2;
        leP.bForward = true;
        qLE.push(leP);
    }
    while(!qLE.empty())
    {
        labelPath leTop = qLE.front();
        qLE.pop();
        int topID1 = leTop.vex->ID1;
        int topID2 = leTop.vex->ID2;
        int it1 = *leTop.it1;
        int it2 = *leTop.it2;
        if(bPrint == true) {
            cout << "topID1: " << topID1 << " topID2: " << topID2 << " it1: " << it1 << " it2: " << it2 << endl;
        }
//        cout << "topID1: " << topID1 << " topID2: " << topID2 << " it1: " << it1 << " it2: " << it2 << endl;
        labelPath leLeft, leRight;
        sePATH seLeft, seRight;
        if(leTop.bForward)
        {
            if(bPrint == true) {
                cout << "case 1" << endl;
                cout << "vKW size: " << leTop.vex->vKW.size() << endl;
            }
//            cout << "case 1" << endl;
//            cout << "vKW size: " << leTop.vex->vKW.size() << endl;
//            for (int i = 0; i < leTop.vex->vKW.size(); ++i) {
//                cout << "vKW i: " << leTop.vex->vKW[i] << endl;
//            }
            int a;
            if ((*(leTop.vex)).vKW.size() == 0)
            {
                subA pTop = leTop.vex->Q.top();
                a = pTop.a;
                if(a == topID1 || a== topID2){

                    seLeft.k = (*(leTop.vex)).vSubA[leTop.k].first;
                    if(bPrint == true) {
                        cout << "a = topID seLeft.k: " << seLeft.k << endl;
                    }
                    cout << "a = topID seLeft.k: " << seLeft.k << endl;
                    int caveID1 = topID1;
                    int caveID2 = topID2;
                    bool bcaveForward = true;
                    if(NodeOrder[topID1] < NodeOrder[topID2])
                    {
                        bcaveForward = false;
                        int IDTmp = topID1;
                        caveID1 = a;
                        caveID2 = IDTmp;
                    }
                    list<int> cavePath;
                    caveRetriveal(caveID1, caveID2, seLeft.k, cavePath);
                    if(bcaveForward == false){
                        cavePath.reverse();
                    }
                    if (cavePath.size() > 2) {
                        auto startIt = std::next(cavePath.begin());
                        auto endIt = std::prev(cavePath.end());
                        lPath.insert(leTop.it2, startIt, endIt);
                    }
                    if(bPrint == true) {
                        for (list<int>::iterator it = cavePath.begin(); it != cavePath.end(); ++it) {
                            std::cout << *it << std::endl;
                        }
                    }
                    continue;
                }
                leTop.it1++;
                leTop.it1 = lPath.insert(leTop.it1, a);
                leTop.it1--;
            }
            else
            {
                a = leTop.vex->vKW[leTop.k];
                if(bPrint == true) {
                    cout << "leTop.k: " << leTop.k << " a: " << a << endl;
                }
//                cout << "leTop.k: " << leTop.k << " a: " << a << endl;
                if(a == topID1 || a == topID2){

                    seLeft.k = (*(leTop.vex)).vSubA[leTop.k].first;
                    if(bPrint == true) {
                        cout << "a = topID seLeft.k: " << seLeft.k << endl;
                    }
//                    cout << "a = topID seLeft.k: " << seLeft.k << endl;
                    int caveID1 = topID1;
                    int caveID2 = topID2;
                    bool bcaveForward = true;
                    if(NodeOrder[topID1] < NodeOrder[topID2])
                    {
                        bcaveForward = false;
                        int IDTmp = topID1;
                        caveID1 = a;
                        caveID2 = IDTmp;
                    }
                    list<int> cavePath;
                    caveRetriveal(caveID1, caveID2, seLeft.k, cavePath);
                    if(bcaveForward == false){
                        cavePath.reverse();
                    }
                    if (cavePath.size() > 2) {
                        auto startIt = std::next(cavePath.begin());
                        auto endIt = std::prev(cavePath.end());
                        lPath.insert(leTop.it2, startIt, endIt);
                    }
                    if(bPrint == true) {
                        for (list<int>::iterator it = cavePath.begin(); it != cavePath.end(); ++it) {
                            std::cout << *it << std::endl;
                        }
                    }
                    continue;
                }
                leTop.it1++;
                leTop.it1 = lPath.insert(leTop.it1,a);
                leTop.it1--;
//                cout << "leTop.it1: " << *(leTop.it1) << " leTop.it2: " << *(leTop.it2) << endl;
            }
            int o1 = NodeOrder[topID1];
            int o2 = NodeOrder[a];
            int o3 = NodeOrder[topID2];

            if(o3 > o2){
                auto itRight = vmLEPos[a].find(topID2);
                if(itRight != vmLEPos[a].end())
                {
                    if(bPrint == true) {
                        cout << "o3 > o2" << endl;
                        cout << "Right" << endl;
                        cout << "topID2: " << topID2 << " a: " << a << endl;
                    }
                    leRight.vex = &vvLEQuery[a][(*itRight).second];
                    /*if ((*(leTop.vex)).vKW.size() <= k)
                        leRight.k = 0;
                    else
                        leRight.k = (*(leTop.vex)).vSubA[leTop.k].second;*/
                    leRight.k = (*(leTop.vex)).vSubA[leTop.k].second;
                    if(bPrint == true) {
                        cout <<  "leRight.k: " <<  leRight.k << endl;
                    }
                    if(leRight.k != -1)
                    {
                        leRight.it1 = leTop.it2;
                        leRight.it2 = leRight.it1;
                        leRight.it1--;
                        leRight.bForward = true;
                        if(bPrint == true) {
                            cout << "it1: " << *leRight.it1 << " it2: " << *leRight.it2 << endl;
                        }
//                    leRight.bForward = true;
                        leRight.bContinue = false;
                        qLE.push(leRight);
                    }
                }
            }
            else{
                if(bPrint == true) {
                    cout << "o3 < o2" << endl;
                }
                auto itRight = vmLEPos[topID2].find(a);
                if(itRight != vmLEPos[topID2].end())
                {
                    if(bPrint == true) {
                        cout << "Right" << endl;
                        cout << "topID2: " << topID2 << " a: " << a << endl;
                    }
                    leRight.vex = &vvLEQuery[topID2][(*itRight).second];
                    if(bPrint == true) {
                        cout << "vSubA.size(): " << (*(leTop.vex)).vSubA.size() << endl;
                    }
                    leRight.k = (*(leTop.vex)).vSubA[leTop.k].second;
                    /* if ((*(leTop.vex)).vKW.size() <= k)
                         leRight.k = 0;
                     else
                         leRight.k = (*(leTop.vex)).vSubA[leTop.k].second;*/
                    if(bPrint == true) {
                        cout << (*(leTop.vex)).vSubA[leTop.k].first << " " << (*(leTop.vex)).vSubA[leTop.k].second << endl;
                        cout <<  "leTop.k: " << leTop.k << " leRight.k: " <<  leRight.k << endl;
                    }
                    if(leRight.k != -1)
                    {
                        leRight.it2 = leTop.it2;
                        leRight.it1 = leRight.it2;
                        leRight.it2--;
                        leRight.bForward = false;
                        if(bPrint == true) {
                            cout << "it1: " << *leRight.it1 << " it2: " << *leRight.it2 << endl;
                        }
//                    leRight.bForward = true;
                        leRight.bContinue = false;
                        qLE.push(leRight);
                    }

                }
            }

            seLeft.k = (*(leTop.vex)).vSubA[leTop.k].first;
//            cout << "seLeft.k: " << seLeft.k << endl;
            int caveID1 = topID1;
            int caveID2 = a;
            bool bcaveForward = true;
            if(NodeOrder[topID1] < NodeOrder[a])
            {
                bcaveForward = false;
                int IDTmp = topID1;
                caveID1 = a;
                caveID2 = IDTmp;
            }
            list<int> cavePath;
            if(bPrint == true) {
                cout << "caveID1: " << caveID1 << " cave ID2: " << caveID2 << "seLeft.k: " << seLeft.k << endl;
            }
            caveRetriveal(caveID1, caveID2, seLeft.k, cavePath);
            if(bcaveForward == false){
                cavePath.reverse();
            }
            auto caveitt = leRight.it2;
            if(bPrint == true) {
                cout << "leRight.it1: " << *leRight.it1 << " a: " << a << " leRight.it2: " << *leRight.it2 << endl;
            }
            if(*leRight.it1 == a){
                if(bPrint == true) {
                    cout << "true" << endl;
                }
                caveitt = leRight.it1;
            }

            if (cavePath.size() > 2) {
                auto startIt = std::next(cavePath.begin());
                auto endIt = std::prev(cavePath.end());
                if(bPrint == true) {
                    cout << "caveitt: " << *caveitt << endl;
                }
                // Insert the nodes into lPath, excluding the first and last nodes of cavePath
                lPath.insert(caveitt, startIt, endIt);
            }
            if(bPrint == true) {
                for (list<int>::iterator it = cavePath.begin(); it != cavePath.end(); ++it) {
                    std::cout << *it << std::endl;
                }
            }
        }

        else
        {
            if(bPrint == true) {
                cout << "case 2" << endl;
            }
            int a;
            if(bPrint == true) {
                cout << "vKW size: " << leTop.vex->vKW.size() << endl;
            }
//            for (int i = 0; i < leTop.vex->vKW.size(); ++i) {
//                cout << "vKW i: " << leTop.vex->vKW[i] << endl;
//            }
            if ((*(leTop.vex)).vKW.size() == 0)
            {
                subA pTop = leTop.vex->Q.top();
                a = pTop.a;
                if(a == topID1 || a == topID2){

                    seRight.k = (*(leTop.vex)).vSubA[leTop.k].first;
                    if(bPrint == true) {
                        cout << "seRight.k: " << seRight.k << endl;
                    }
                    int caveID1 = topID1;
                    int caveID2 = topID2;
                    bool bcaveForward = true;
                    if(NodeOrder[topID1] < NodeOrder[topID2])
                    {
                        bcaveForward = false;
                        int IDTmp = topID1;
                        caveID1 = a;
                        caveID2 = IDTmp;
                    }
                    list<int> cavePath;
                    caveRetriveal(caveID1, caveID2, seRight.k, cavePath);
                    if(bcaveForward == true){
                        cavePath.reverse();
                    }
                    if(bPrint == true) {
                        cout << "leTop.it1: " << *leTop.it1 << endl;
                    }
                    auto caveit = leTop.it1;
                    if(bPrint == true) {
                        cout << "caveit: " << *caveit << endl;
                        cout << "leLeft.it1: " << *leTop.it1 << endl;
                    }
                    if (cavePath.size() > 2) {
                        auto startIt = std::next(cavePath.begin());
                        auto endIt = std::prev(cavePath.end());

                        // Insert the nodes into lPath, excluding the first and last nodes of cavePath
                        lPath.insert(caveit, startIt, endIt);
                    }
                    continue;
                }
                leTop.it2++;
                leTop.it2 = lPath.insert(leTop.it2, a);
                leTop.it2--;

            }
            else
            {
                a = leTop.vex->vKW[leTop.k];
                if(bPrint == true) {
                    cout << "leTop.k: " << leTop.k << " a: " << a << endl;
                }
                if(a == topID1 || a == topID2)
                {
                    seRight.k = (*(leTop.vex)).vSubA[leTop.k].first;
                    if(bPrint == true) {
                        cout << "seRight.k: " << seRight.k << endl;
                    }
                    int caveID1 = topID1;
                    int caveID2 = topID2;
                    bool bcaveForward = true;
                    if(NodeOrder[topID1] < NodeOrder[topID2])
                    {
                        bcaveForward = false;
                        int IDTmp = topID1;
                        caveID1 = a;
                        caveID2 = IDTmp;
                    }
                    list<int> cavePath;
                    caveRetriveal(caveID1, caveID2, seRight.k, cavePath);
                    if(bcaveForward == true){
                        cavePath.reverse();
                    }
                    if(bPrint == true) {
                        cout << "leTop.it1: " << *leTop.it1 << endl;
                    }
//                    cout << "leTop.it1: " << *leTop.it1 << endl;
                    auto caveit = leTop.it1;
                    if(bPrint == true) {
                        cout << "caveit: " << *caveit << endl;
                        cout << "leLeft.it1: " << *leTop.it1 << endl;
                    }
                    if (cavePath.size() > 2) {
                        auto startIt = std::next(cavePath.begin());
                        auto endIt = std::prev(cavePath.end());

                        // Insert the nodes into lPath, excluding the first and last nodes of cavePath
                        lPath.insert(caveit, startIt, endIt);
                    }
                    continue;
                }
                leTop.it2++;
                leTop.it2 = lPath.insert(leTop.it2, a);
                leTop.it2--;
            }
            int o1 = NodeOrder[topID1];
            int o2 = NodeOrder[a];
            int o3 = NodeOrder[topID2];
            if(bPrint == true) {
                cout << "leTop.bContinue: " << leTop.bContinue << endl;
            }
            if(o3 > o2){
                if(bPrint == true) {
                    cout << "o3 > o2" << endl;
                }
                auto itLeft = vmLEPos[a].find(topID2);
                if (itLeft != vmLEPos[a].end()) {
                    if(bPrint == true) {
                        cout << "Left" << endl;
                        cout << "topID2: " << topID2 << " a: " << a << endl;
                    }
                    leLeft.vex = &vvLEQuery[a][(*itLeft).second];
                    leLeft.k = (*(leTop.vex)).vSubA[leTop.k].second;
                    if(bPrint == true) {
                        cout << (*(leTop.vex)).vSubA[leTop.k].first << " " << (*(leTop.vex)).vSubA[leTop.k].second << endl;
                        cout << "leLeft.k: " << leLeft.k << endl;
                    }
                    if (leLeft.k != -1) {
                        leLeft.it1 = leTop.it2;
                        leLeft.it2 = leLeft.it1;
                        leLeft.it1++;
                        if(bPrint == true) {
                            cout << "it1: " << *leLeft.it1 << " it2: " << *leLeft.it2 << endl;
                        }
                        leLeft.bForward = false;
                        leLeft.bContinue = false;
                        qLE.push(leLeft);
                    }
                }
                /*else{
                    cout << "no more" << endl;
                }*/
            }
            else{
                if(bPrint == true) {
                    cout << "o3 < o2" << endl;
                    cout << "topID2: " << topID2 << " a: " << a << endl;
                }
                auto itLeft = vmLEPos[topID2].find(a);
                if (itLeft != vmLEPos[topID2].end()) {
                    if(bPrint == true) {
                        cout << "Left" << endl;
                        cout << "topID2: " << topID2 << " a: " << a << endl;
                    }
                    leLeft.vex = &vvLEQuery[topID2][(*itLeft).second];
                    leLeft.k = (*(leTop.vex)).vSubA[leTop.k].second;
                    if(bPrint == true) {
                        cout << (*(leTop.vex)).vSubA[leTop.k].first << " " << (*(leTop.vex)).vSubA[leTop.k].second << endl;
                        cout << "leLeft.k: " << leLeft.k << endl;
                    }
                    if (leLeft.k != -1) {
//                        leLeft.it1 = leTop.it2;
//                        leLeft.it2 = leLeft.it1;
//                        leLeft.it1++;
                        leLeft.it2 = leTop.it2;
                        leLeft.it1 = leLeft.it2;
                        leLeft.it2++;
                        if(bPrint == true) {
                            cout << "it1: " << *leLeft.it1 << " it2: " << *leLeft.it2 << endl;
                        }
                        leLeft.bForward = true;
                        leLeft.bContinue = false;
                        qLE.push(leLeft);
                    }
                }
                /* else{
                     cout << "no more" << endl;
                 }*/
            }


            seRight.k = (*(leTop.vex)).vSubA[leTop.k].first;
            if(bPrint == true) {
                cout << "seRight.k: " << seRight.k << endl;
            }
//            cout << "seRight.k: " << seRight.k << endl;
            int caveID1 = topID1;
            int caveID2 = a;
            bool bcaveForward = true;
            if(NodeOrder[topID1] < NodeOrder[a])
            {
                bcaveForward = false;
                int IDTmp = topID1;
                caveID1 = a;
                caveID2 = IDTmp;
            }
            list<int> cavePath;
            caveRetriveal(caveID1, caveID2, seRight.k, cavePath);
            if(bcaveForward == true){
                cavePath.reverse();
            }
            if(bPrint == true) {
                cout << "leLeft.it1: " << *leLeft.it1 << endl;
            }
//            cout << "leLeft.it1: " << *leLeft.it1 << endl;
            leLeft.it1++;
            auto caveit = leLeft.it1;
            leLeft.it1--;
            leLeft.it2++;
            if(leLeft.it2 == leTop.it1){
                caveit = leLeft.it2;
            }
            leLeft.it2--;
            if(bPrint == true) {
                cout << "caveit: " << *caveit << endl;
                cout << "leLeft.it1: " << *leLeft.it1 << endl;
            }
            if (cavePath.size() > 2) {
                auto startIt = std::next(cavePath.begin());
                auto endIt = std::prev(cavePath.end());

                // Insert the nodes into lPath, excluding the first and last nodes of cavePath
                lPath.insert(caveit, startIt, endIt);
            }
        }
    }
    if(bReverse == true){
        reverse(lPath.begin(),lPath.end());
    }
}












