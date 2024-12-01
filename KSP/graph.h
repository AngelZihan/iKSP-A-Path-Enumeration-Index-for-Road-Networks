#ifndef GRAPH_H
#define	GRAPH_H
#include <vector>
#include <map>
#include <list>
#include <iterator> // For std::next and std::prev
#include <iostream>
#include <fstream>
#include <chrono>
#include "tools.h"
#include "heap.h"  
#include <list>
#include <string>
#include <stdlib.h>
#include <unordered_map>
#include <unordered_set> 
#include <algorithm> 
#include <stack>
#include <queue>
#include <utility>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/unordered_set.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/utility.hpp>
//#include <boost/heap/fibonacci_heap.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <memory>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/unique_ptr.hpp>
#include "xxhash.h"

using namespace std;
using namespace benchmark;

typedef struct COMPARENODE
{
	pair<int, int> pif;//ID, minCost
	bool operator() (const struct COMPARENODE& a, const struct COMPARENODE& b) const  
	{  
		return a.pif.second > b.pif.second; 
	} 
}compareNode;

struct pair_hash 
{ 
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1,T2> &p) const 
	{ 
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ h2;  
    }
};

struct Edge
{
	int ID1, ID2, length, edgeID, cost;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & ID1, ID2, length, edgeID, cost;
    }
};

struct PTNode
{
	int nodeID;
	unordered_map<int, int>	PTRoad;	//childrenID, roadID
	unordered_map<int, int> PTChildren; //childrenID, node Pos
};

struct NeighborInfo{
	int nid;
	int w;
	int c;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & nid;
        ar & w;
        ar & c;
    }
};

static vector<int> GlobalNodeOrder;  

struct TreeNode
{
	vector<pair<int,pair<int,int>>> vert;//neighID/weight/count
	vector<pair<int,NeighborInfo>> vNeighborInfo;//posID,neighID,weight,count(for shortcut information maintenance)
	vector<int> pos, pos2; //Neighbour pos
	vector<int> dis, cnt;//the distance value and corresponding count number
	//vector<set<int>> FromNode;
	set<int> changedPos;
	vector<bool> FN;//another succint way of FromNode
	set<int> DisRe;
	vector<int> vChildren;
	int height, hdepth;//hdepty is the deepest node that a vertex still exists
	int parentNodeID; //parent
	int uniqueVertex;
	vector<int> vSupport;

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & vert;
        ar & vNeighborInfo;
        ar & pos, pos2;
        ar & dis, cnt;
        ar & changedPos;
        ar & FN;
        ar & DisRe;
        ar & vChildren;
        ar & height, hdepth;
        ar & parentNodeID;
        ar & uniqueVertex;
        ar & vSupport;
    }
	
	TreeNode()
	{ 
		vert.clear();
		vNeighborInfo.clear();
		pos.clear();
		dis.clear();
		cnt.clear();
		vChildren.clear();
		parentNodeID = -1;
		uniqueVertex = -1;
		height = 0;
		hdepth = 0;
		changedPos.clear();
		FN.clear();
		DisRe.clear();
		vSupport.clear();
	}
};

/*static bool wCompare(pair<int, int>& w1, pair<int, int>& w2)
{
	return w1.second <= w2.second;
}*/

struct subW
{
	int d;
	int w;
    int d1;
    int d2;
	int k1;
	int k2;
	bool bW;//True for superedge, false for label

    subW() : d(0), w(0), k1(0), k2(0), bW(false) {}


    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & d;
        ar & w;
        ar & k1;
        ar & k2;
        ar & bW;
    }
};

struct CompareW
{
	bool operator()(subW& w1, subW& w2)
	{
		if(w1.d < w2.d)
			return false;
		else if(w1.d == w2.d)
			return w1.w > w2.w; 
		return true;
	}
};

struct seEnum
{
	int wNum;	//supportive node number
	int ID1, ID2;	//ID1 has higher order
	//benchmark::heap<2, int, int> Q; 
	priority_queue<subW, vector<subW >, CompareW> Q;//distance, wID
	vector<int> vDistance;  //Distances of w
    vector<int> vDistance1;
    vector<int> vDistance2;
	vector<int> vW;//Support nodes
	unordered_map<int, int> umWPos;
	vector<int> vK;//Top-K distance 
	vector<int> vKW;//w that creates k
    vector<pair<int, int>> subKNumber;
    vector<int> pathTotal;
    vector<vector<int>> vNode;
//    vector<string> vPathstr;
    vector<vector<int>> rankSet1;
    vector<vector<int>> rankSet2;
    vector<vector<int>> vRank;
    vector<bool> sebLoop; //true for path is with loop, false for path is without loop
    vector<set<pair<int,int>>> visited;
	vector<pair<int, int> > vSubK;//For path retrieval
	vector<bool> vbFinished;//Finish Enumeration of w_i
	vector<pair<bool, bool> > vpbFinished;//Of two sub path
	vector<pair<vector<int>, vector<int> > > vpvWDistance; //For kSum
//    vector<pair<vector<vector<int> >, vector<vector<int> > > > vpWNode;
	vector<pair<pair<int, int>, pair<int, int> > > vR; 
	bool bFinished;


    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & wNum;
        ar & ID1;
        ar & ID2;

        priority_queue<subW, std::vector<subW>, CompareW> tempQ = Q;

        // Serialize the priority queue by converting it to a vector
        vector<subW> Q_container;
        while (!tempQ.empty()) {
            Q_container.push_back(tempQ.top());
            tempQ.pop();
        }
        ar & Q_container;

        // Reconstruct the priority queue after deserialization
        if (Archive::is_loading::value) {
            for (auto& elem : Q_container) {
                Q.push(elem);
            }
        }


        ar & vDistance;
        ar & vW;
        ar & umWPos;
        ar & vK;
        ar & vKW;
        ar & vNode;
        ar & visited;
        ar & vSubK;
        ar & vbFinished;
        ar & vpbFinished;
        ar & vpvWDistance;
        ar & vR;
        ar & bFinished;
    }

};


struct sePATH
{
	seEnum* se;
	int k;
	list<int>::iterator it1, it2;  
	bool bForward;
};

struct subA
{
	int d;
    int d1;
    int d2;
	int a;
	int k1;
	int k2;
	bool bSE;
	seEnum *se;
//    unique_ptr<seEnum> se;

    // Serialization function
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & d;
        ar & a;
        ar & k1;
        ar & k2;
        ar & bSE;
        ar & *se;
    }
};

struct CompareA
{
	bool operator()(subA& a1, subA& a2)
	{
		if(a1.d < a2.d)
			return false;
		else if(a1.d == a2.d)
			return a1.a > a2.a; 
		return true;
	}
};

template<class Archive, class T, class Container, class Compare>
void save(Archive& ar, const std::priority_queue<T, Container, Compare>& pq, const unsigned int version) {
    Container c;
    // Copy pq into a non-const priority_queue to access its underlying container
    std::priority_queue<T, Container, Compare> temp = pq;
    while (!temp.empty()) {
        c.push_back(temp.top());
        temp.pop();
    }
    ar & c;
}

template<class Archive, class T, class Container, class Compare>
void load(Archive& ar, std::priority_queue<T, Container, Compare>& pq, const unsigned int version) {
    Container c;
    ar & c;
    pq = std::priority_queue<T, Container, Compare>(Compare(), std::move(c)); // Reconstruct the priority_queue
}

template<class Archive, class T, class Container, class Compare>
void serialize(Archive& ar, std::priority_queue<T, Container, Compare>& pq, const unsigned int version) {
    boost::serialization::split_free(ar, pq, version);
}


struct labelEnum
{
	int aNum; //access node number
	int ID1, ID2; //ID2 has higher order
	priority_queue<subA, vector<subA>, CompareA> Q;//d, aID
	vector<int> vDistance; //d of a
	vector<int> vA; //pivot nodes
	unordered_map<int, int> umAPos;
    vector<vector<int>> vNode;
//    vector<string> vPathstr;
    vector<vector<int>> rankSet1;
    vector<vector<int>> rankSet2;
    vector<vector<int>> vRank;
    int KSPNumber;
	vector<int> vK; //Top-K Distance
	vector<int> vKW; //a that creates k
    vector<pair<int, int>> subKNumber;
    vector<int> pathTotal;
	vector<pair<int, int> > vSubA; //For path retrieval
    vector<set<pair<int,int>>> visited;
	vector<bool> vbFinished; //Finish Enumeration of A_i
	vector<pair<bool, bool> > vpbFinished;
	vector<pair<vector<int>, vector<int> > > vpvADistance;
//    vector<pair<vector<vector<int> >, vector<vector<int> > > > vpNode;
	vector<pair<pair<int, int>, pair<int, int > > > vR;
//    unordered_set<string> uniquePath;
    unordered_set<uint64_t> hashSet;
	bool bFinished;

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & aNum;
        ar & ID1;
        ar & ID2;

        priority_queue<subA, std::vector<subA>, CompareA> tempQ = Q;

        // Serialize the priority queue by converting it to a vector
        vector<subA> Q_container;
        while (!tempQ.empty()) {
            Q_container.push_back(tempQ.top());
            tempQ.pop();
        }
        ar & Q_container;

        // Reconstruct the priority queue after deserialization
        if (Archive::is_loading::value) {
            for (auto& elem : Q_container) {
                Q.push(elem);
            }
        }
        ar & vDistance;
        ar & vA;
        ar & umAPos;
        ar & vNode;
        ar & KSPNumber;
        ar & vK;
        ar & vKW;
        ar & visited;
        ar & vSubA;
        ar & vbFinished;
        ar & vpbFinished;
        ar & vpvADistance;
        ar & vR;
//        ar & uniquePath;
        ar & bFinished;
        // Serialize other members
    }

};

struct subH
{
    int d;
    int h;
    int d1;
    int d2;
    int k1;
    int k2;
    bool bSE1;
    bool bSE2;
    seEnum* se;
};

struct CompareH
{
    bool operator()(subH& h1, subH& h2)
    {
        if(h1.d < h2.d)
            return false;
        else if(h1.d == h2.d)
            return h1.h > h2.h;
        return true;
    }
};

//priority_queue<subH, vector<subH>, CompareH> hQ;



struct labelPath
{
	labelEnum* vex;
	int k;
	list<int>::iterator it1, it2;
	bool bForward;
    bool bContinue;
};

class Graph
{
public:
//    friend class boost::serialization::access;

	Graph(){}
	Graph(string filename)
	{
		if(filename == "./beijingNodeNew") 
		{
			readBeijingMapDirected(filename);
			ISONodes();
		}
		else if(filename == "./ExampleGraph2" )  
		{
			readExampleMap(filename);
		}
//		else if(filename == "/Users/angel/CLionProjects/KSP/simple")
        else if(filename == "./simple")
			readSimpleGraph(filename);
		else
		{
			readUSMap(filename);
			//readUSMapCost(filename);
		}
	}

	//True: Only adjList and adjListMap are used
	//False: Also use R versions
	int nodeNum;
	int edgeNum;
    unordered_map<int, int> newNodeMap;
	vector<vector<pair<int, int> > > adjList;		//neighborID, Distance
	vector<vector<pair<int, int> > > adjListR;
	vector<vector<pair<int, int> > > adjListEdge;	//neighbor,edgeID
	vector<vector<pair<int, int> > > adjListEdgeR;

	vector<vector<pair<int, int> > > adjListCost;		//neighborID, cost
	vector<vector<pair<int, int> > > adjListCostR;

	vector<unordered_map<int, int> > vmEdge;
//	unordered_map<pair<int, int>, int, pair_hash> umEdge;

	vector<Edge> vEdge;
	vector<Edge> vEdgeR; 

	int readCost(string filename);
	
	
	//Identify the ISO nodes
	vector<bool> vbISOF;	//forward
	vector<bool> vbISOB;	//backward
	vector<bool> vbISOU;	//F & B
	vector<bool> vbISO;		//F | B
	int ISONodes();
	int BFS(int nodeID, bool bF, vector<bool>& vbVisited);

	vector<pair<double, double> > vCoor; 
	unordered_map<string, pair<int, int> > mCoor;
	double minX, minY, maxX, maxY;

	//lon, lat
	int readBeijingMapDirected(string filename); 
	int readUSMap(string filename);
	int readUSCost(string filename);
	int readUSMapCost(string filename);
	int readExampleMap(string filename);  
	void readSimpleGraph(string filename);
	int readUSCoor(string filename);

	//test.cpp
	void testCSP(string filename);
	void SCPT(int root, vector<int>& vSPTDistance, vector<int>& vSPTCost, vector<int>& vSPTParent, vector<int>& vSPTParentEdge, vector<vector<int> >& vSPT, int C);
	void rCPT(int root, int ID1, int C, vector<int>& vrSPTCost, vector<int>& vrSPTDistance);

	//	void contractNode(int threshold);

	int Dijkstra(int ID1, int ID2);
	int DijkstraPath(int ID1, int ID2, vector<int>& vPath, vector<int>& vPathEdge);
	int DijkstraPath2(int ID1, int ID2, unordered_set<int>& sRemovedNode, vector<int>& vPath, vector<int>& vPathEdge);
	int AStar(int ID1, int ID2);
	int AStarPath(int ID1, int ID2, vector<int>& vPath, vector<int>& vPathEdge, string& city); 
	int EuclideanDistance(int ID1, int ID2);
	int EuclideanDistanceAdaptive(int ID1, int ID2, int latU, int lonU);

	int Yen(int ID1, int ID2, int k, vector<int>& kResults);
	int Yen2003(int ID1, int ID2, int k, vector<int>& kResults);  
	int Pascoal(int ID1, int ID2, int k, vector<int>& kResults, vector<vector<int> >& vkPath);
	void SPT(int root, vector<int>& vSPTDistance, vector<int>& vSPTParent, vector<int>& vSPTParentEdge, vector<vector<int> >& vSPT);

	//CIKM2010 in cKSP
	typedef struct LABEL
	{
		int pre;
		int post;
		int parent;
	}label;
	int cKSP(int ID1, int ID2, int k, vector<int>& kResults, vector<vector<int> >& vkPath);  
	void rSPT(int root, vector<int>& vSPTDistance, vector<int>& vSPTParent, vector<int>& vSPTParentEdge, vector<vector<int> >& vSPT);//reverse SP Tree
	void intervalLabel(int root, vector<vector<int> >& vSPT, vector<int>& vSPTParent, vector<label>& vLabel); 
	int DijkstraSideCost(int ID1, int ID2, vector<vector<pair<int, int> > >& adjSideCost, vector<vector<pair<int, int> > >& adjSideRoad, vector<int>& vPath, vector<int>& vPathEdge);
	int DijkstraSideCostPrune(int ID1, int ID2, vector<vector<pair<int, int> > >& adjSideCost, vector<vector<pair<int, int> > >& adjSideRoad, vector<int>& vSPTParent, vector<int>& vSPTParentEdge, vector<int>& vSPTDistance, vector<label>& vLabel, vector<int>& vIgnoredNode, vector<int>& vPath, vector<int>& vPathEdge, bool& bE);

	//CSP
	pair<int, int> fKSPCSP(int ID1, int ID2, int C); 
	void rCSPT(int root, vector<int>& vSPTDistance, vector<int>& vSPTCost, vector<int>& vSPTParent, vector<int>& vSPTParentEdge, vector<vector<int> >& vSPT); 
	void rCostLB(int root, vector<int>& vSPTCostLB, vector<int>& vCostUB);
	void sideDist(vector<int>& vSPTDistance, vector<vector<pair<int, int> > >& adjSideDist, vector<vector<pair<int, int> > >& adjSideRoad, vector<Edge>& vSideEdge);
//	int DijkstraSideCostPruneCSP(int ID1, int ID2, vector<vector<pair<int, int> > >& adjSideDist, vector<vector<pair<int, int> > >& adjSideRoad, vector<int>& vSPTParent, vector<int>& vSPTParentEdge, vector<int>& vSPTDistance, vector<label>& vLabel, vector<int>& vIgnoredNode, vector<int>& vPath, vector<int>& vPathEdge, bool& bE);
	int DijkstraSideCostPruneCSP(int ID1, int ID2, vector<vector<pair<int, int> > >& adjSideDist, vector<vector<pair<int, int> > >& adjSideRoad, vector<int>& vSPTParent, vector<int>& vSPTParentEdge, vector<int>& vSPTDistance, vector<int>& vSPTCost, vector<label>& vLabel, vector<int>& vIgnoredNode, vector<int>& vPath, vector<int>& vPathEdge, bool& bE, int& subLength, int& subCost, vector<int>& vSPTCostLB, vector<map<int, int> >& vmSkyline, int& dTmp, int& cTmp);  

	pair<int, int> cKSPCSP(int ID1, int ID2, int C); 
	pair<int, int> Pulse(int ID1, int ID2, int C);
	int eKSP(int ID1, int ID2, int C);
	pair<int, int> skylineCSP(int ID1, int ID2, int C);
	void cD(int ID1, int ID2);
	void forwardCost(int ID1, int C, vector<int>& vFCost); 


	int kspCSP(int ID1, int ID2, int C); 

	void cKSPNew(int ID1, int ID2, int k, vector<int>& kResults, vector<vector<int> >& vkPath);
	int DijkstraSideCostPruneNew(int ID1, int ID2, vector<vector<pair<int, int> > >& adjSideDist, vector<vector<pair<int, int> > >& adjSideRoad, vector<int>& vSPTParent, vector<int>& vSPTParentEdge, vector<int>& vSPTDistance, vector<label>& vLabel, vector<int>& vIgnoredNode, vector<int>& vPath, vector<int>& vPathEdge, bool& bE, int& subLength,  int& dTmp);

	int eKSPNew(int ID1, int ID2, int k, vector<int>& kResults, vector<vector<int> >& vkPath);   
	
	void FindRepeatedPath(vector<vector<int> >& vvPath); 

	//TDKSP
	vector<map<int,pair<int,int>>> E;
	vector<int> DD,DD2;
	vector<int> vNodeOrder;
	vector<int> NodeOrder;
	vector<vector<pair<int,pair<int,int>>>> NeighborCon;
	//<nodeID1, nodeID2>, <support nodes>
	//nodeID1 < nodeID2
	map<pair<int,int>,vector<int>> SCconNodes;



    priority_queue<subH, vector<subH>, CompareH> hQ;
    vector<int> vK;
    vector<int> vKW;
    int KSPNumber = 0;
    vector<vector<int>> v1;
    vector<vector<int>> v2;
    vector<int> tmpv;
    vector<vector<int>> vNode;
    vector<vector<int>> vRank;
    vector<int> pathTotal;
    vector<vector<int>> rankSet1;
    vector<vector<int>> rankSet2;
    vector<pair<int, int> > vSubA;
    unordered_set<uint64_t> hashSet;
    bool bPrint = false;
    vector<pair<vector<int>, vector<int> > >vpvADistance;
    pair<vector<int>, vector<int> > tmpvpvADistance;

    vector<pair<pair<int, int>, pair<int, int > > > vR;
    vector<set<pair<int, int>>> visited;
    set<pair<int, int>> tmpvisited;
    pair<pair<int, int>, pair<int, int > > tmpvR;
    unordered_map<int, int> umAPos;
    vector<bool> vbFinished; //Finish Enumeration of A_i
    vector<pair<bool, bool> > vpbFinished;
    bool bFinished;
	
	void loadTD(string filename);
	void readTD(string filename);
	void writeTD(string filename);
	void TDConstruction(string filename);
	void CHConstruction();
    void rankCHConstruction();
	void makeTree();
	void makeIndex();
	void makeIndexDFS(int p, vector<int>& list);
	void searchChild(int p, int a, int b, int& mind, int& minc, int miniH);
	int match(int x, vector<pair<int,pair<int,int>>> &vert);
	void insertE(int u, int v, int w);
	void insertEorder(int u, int v, int w);
	void deleteE(int u, int v);
	void deleteEorder(int u, int v);
	
	vector<int> rank;
	vector<TreeNode> Tree;
	int heightMax;
	vector<vector<int>> VidtoTNid;//one vertex exist in those tree nodes (nodeID--->tree node rank)
	
	vector<int> EulerSeq;
	vector<int> toRMQ;
	vector<vector<int>> RMQIndex;
	void makeRMQ();
	void makeRMQDFS(int p, int height);
	int QueryH2H(int ID1,int ID2);
	int LCAQuery(int _p, int _q);
	void findLeaves(vector<int>& vLeaves);

	vector<vector<seEnum> > vvSE;
	vector<vector<seEnum> > vvSEQuery;//Copy from vvSE for each query 
	vector<unordered_map<int, int> > vmSEPos;
	vector<unordered_map<int, bool> > vmEFinsihed;//SE+E
	vector<unordered_map<int, bool> > vmEFinsihedQuery;//SE+E 
	//bool seCompare(seEnum se1, seEnum se2);
	
	vector<vector<labelEnum> > vvLE;
	vector<vector<labelEnum> > vvLEQuery;
	vector<unordered_map<int, int> > vmLEPos; 
	vector<unordered_map<int, bool> > vmLFinsihed;//SE+E
	vector<unordered_map<int, bool> > vmLFinsihedQuery;//SE+E 

	int TDKSP(int ID1, int ID2, int k);  
	void vvSEInit();
	void vvLEInit();
	int caveKSP(int ID1, int ID2, int k, int d1, int d2, int dTop, bool bLeft, bool bForward); // true for left, false for right
//    int caveKSP(int ID1, int ID2, int k, bool bForward);
	void caveRetriveal(int ID1, int ID2, int k, list<int>& lPath);
    int peakKSP(int ID1, int ID2, int k, bool brnvK);
    void peakRetriveal(int ID1, int ID2, int k, list<int>& lPath);
    double jaccard_similarity(const set<pair<int, int>>& path1, const set<pair<int, int>>& path2);
    pair<int, int> normalize_edge(int node1, int node2);
    int calculate_path_length(const set<pair<int, int>>& path);

	int caveKSPLoop(int ID1, int ID2, int k, bool bForward); 
	int hopKSPLoop(int ID1, int ID2, int k, bool bForward);
    bool loadGraphData(const string& filename);
    void saveGraphData(const string& filename);

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & nodeNum;
        ar & edgeNum;
        ar & adjList;
        ar & adjListR;
        ar & adjListEdge;
        ar & adjListEdgeR;
        ar & adjListCost;
        ar & adjListCostR;
        ar & vmEdge;
        ar & vEdge;
        ar & vEdgeR;
        ar & vvSE;
        ar & vvLE;
        /*ar & vmEFinsihed;
        ar & vmLFinsihed;
        ar & E;
        ar & DD,DD2;
        ar & vNodeOrder;
        ar & NodeOrder;
        ar & NeighborCon;
        ar & SCconNodes;
        ar & rank;
        ar & Tree;
        ar & heightMax;
        ar & VidtoTNid;//one vertex exist in those tree nodes (nodeID--->tree node rank)
        ar & EulerSeq;
        ar & toRMQ;
        ar & RMQIndex;
        ar & vmSEPos;
        ar & vmLEPos;*/
    }

};



#endif
