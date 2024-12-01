/*
 * CHcon.cpp
 *
 *  Created on: 22 Dec 2020
 *      Author: zhangmengxuan
 */
#include "head.h"

int Graph::StartCHPOrder(string indexfile, string orderfile){
	ifstream IF(orderfile);
	if(!IF){
		cout<<"Cannot open Map "<<orderfile<<endl;
	}
	NodeOrder.assign(nodenum, -1);
	vNodeOrder.assign(nodenum, -1);
	int num, nodeID, nodeorder;
	IF>>num;
	for(int i=0;i<num;i++){
		IF>>nodeID>>nodeorder;
		NodeOrder[nodeID]=nodeorder;
		if(nodeorder!=-1){
			vNodeOrder[nodeorder]=nodeID;
		}
	}

	vector<pair<int,int>> vecp;
	AdjaShort.assign(nodenum, vecp);
	AdjaShortR.assign(nodenum, vecp);
	vvNode.assign(Neighbor.begin(), Neighbor.end());
	map<int, vector<int>> vecm;
	SupportNodes.assign(nodenum, vecm);

	fstream file;
	file.open(indexfile);
	if(!file){
		CHPconsorder();
		writeShortCutorder(indexfile);
		ReadShortCut(indexfile);
	}
	else
	{
		ReadShortCut(indexfile);
	}

	return 0;
}

//known the order beforehand
int Graph::StartCHOrder(string indexfile, string orderfile){
	ifstream IF(orderfile);
	if(!IF){
		cout<<"Cannot open Map "<<orderfile<<endl;
	}
	NodeOrder.assign(nodenum, -1);
	vNodeOrder.assign(nodenum, -1);
	int num, nodeID, nodeorder;
	IF>>num;
	for(int i=0;i<num;i++){
		IF>>nodeID>>nodeorder;
		NodeOrder[nodeID]=nodeorder;
		if(nodeorder!=-1){
			vNodeOrder[nodeorder]=nodeID;
		}
	}

	vector<pair<int,pair<int,int>>> vect;
	NeighborCon.assign(nodenum,vect);

	std::chrono::high_resolution_clock::time_point t1, t2;
	std::chrono::duration<double> time_span;
	double runT;

	fstream file;
	file.open(indexfile);
	if(!file){
		t1=std::chrono::high_resolution_clock::now();
		CHconsorder();
		t2=std::chrono::high_resolution_clock::now();
		time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2-t1);
		runT= time_span.count();
		cout<<"CH MT "<<runT<<endl;
		writeShortCutCH(indexfile);
		ReadShortCutCH(indexfile);
	}
	else
	{
		ReadShortCutCH(indexfile);
	}

	return 0;
}

void Graph::CHPconsorder(){
	vector<pair<int,int>> nodeinfor;
	vvpShortCut.assign(nodenum, nodeinfor);
	//vnContractedNeighborsOld.assign(nodenum, 0);
	//vnContractedNeighbors.assign(nodenum, 0);
	vector<bool> vbVisited(nodenum, false);
	map<int, int> mPosition;
	mPosition[0]=nodenum-1;
	bool bUpdated;
	vector<pair<int,int>> vU,vW;

	//cout<<"Building static CH "<<endl;

	int v;// current contracting vertex
	int NOwCount=0;
	for(int i=0;i<vNodeOrder.size()-1;i++){
		//if(i%100000==0) cout<<"i=/////////////////////// "<<i<<endl;
		v=vNodeOrder[i];
		if(v!=-1){
			vU=vvNode[v];
			//cout<<"contracting node "<<v<<endl;
			for(auto ivpr=vU.begin();ivpr!=vU.end();ivpr++){
				CHcontractionorder((*ivpr).first, v, vbVisited, (*ivpr).second, vU);
			}

			//deleting v from G
			for(auto ivp = vvNode[v].begin(); ivp != vvNode[v].end(); ivp++)
			{
				for(auto ivpr = vvNode[(*ivp).first].begin(); ivpr != vvNode[(*ivp).first].end(); ivpr++)
					if((*ivpr).first == v)
					{
						vvNode[(*ivp).first].erase(ivpr);
						break;
					}
			}
			vbVisited[v]=true;
		}
	}

	/*for(int i=0;i<vNodeOrder.size();i++){
		NodeOrder[vNodeOrder[i]]=i;
	}*/
}

int Graph::CHcontractionorder(int ID1, int ID2, vector<bool>& vbVisited, int dUV, vector<pair<int, int> >& vW){
	benchmark::heap<2,int,int> Heap(nodenum);
	vector<int> vDistance(nodenum, INF);
	int topNodeID, topDistance, neighborNodeID, neighborLength;
	int w;
	vDistance[ID1]=0;
	Heap.update(ID1,0);
	int count=0;
	map<int,int> mWDistance;
	vector<pair<int,int>> vpWDistance;
	map<int,int> mDistance;
	int maxWDistance=-1;

	for(auto ivp=vW.begin();ivp!=vW.end();ivp++){
		w=(*ivp).first;
		if(NodeOrder[w]>NodeOrder[ID1]){//to get rid of redundant computation
			if(vbVisited[w])
				continue;
			int d=(*ivp).second+dUV;
			mWDistance[w]=d;
			if(d>maxWDistance)
				maxWDistance=d;
			mDistance[w]=INF;
			//maintain the support node information
			if(w<ID1)
				SupportNodes[w][ID1].push_back(ID2);
			else
				SupportNodes[ID1][w].push_back(ID2);
		}
	}
	if(mDistance.empty()){
		//NOwCount+=1;
		return 0;
	}
	for(auto imWDistance=mWDistance.begin();imWDistance!=mWDistance.end();imWDistance++)
		vpWDistance.push_back(make_pair((*imWDistance).first, (*imWDistance).second));

	int dThreshold=maxWDistance;

	while(!Heap.empty()){
		Heap.extract_min(topNodeID, topDistance);
		if(vbVisited[topNodeID])
			continue;
		if(topDistance>dThreshold)
			break;
		for(auto ivp=vvNode[topNodeID].begin();ivp!=vvNode[topNodeID].end();ivp++){
			neighborNodeID = (*ivp).first;
			neighborLength = (*ivp).second;
			if(vbVisited[neighborNodeID] || neighborNodeID==ID2)
				continue;
			int d = vDistance[topNodeID] + neighborLength;
			if(vDistance[neighborNodeID] ==INF && neighborNodeID != ID1)
			{
				vDistance[neighborNodeID] = d;
				Heap.update(neighborNodeID, d);
				if(mWDistance.find(neighborNodeID) != mWDistance.end())
					mDistance[neighborNodeID] = d;
			}else if(vDistance[neighborNodeID]>d){
				vDistance[neighborNodeID] = d;
				Heap.update(neighborNodeID, d);
				if(mWDistance.find(neighborNodeID) != mWDistance.end())
					mDistance[neighborNodeID] = d;
			}
			else if(vDistance[neighborNodeID] == d)
			{
				if(mWDistance.find(neighborNodeID) != mWDistance.end())
					mDistance[neighborNodeID] = d;
			}
		}
	}

	for(auto imDistance = mDistance.begin(); imDistance != mDistance.end(); imDistance++)
	{
		if((*imDistance).second > mWDistance[(*imDistance).first])//add shortcuts
		{
			int w = (*imDistance).first;
			int distance = mWDistance[(*imDistance).first];

			vvpShortCut[ID1].push_back(make_pair(w, distance));
			vvNode[ID1].push_back(make_pair(w, distance));
			vvNode[w].push_back(make_pair(ID1, distance));
		}
		/*else{//not add shortcut
			int w = (*imDistance).first;
			int dis= (*imDistance).second;

			int dDijk=Dij(ID1, w);
			if(dDijk!=dis)
				cout<<"distance inequivalent "<<dDijk<<" "<<dis<<endl;
		}*/
	}

	//cout<<"shortcut number "<<count<<endl;
	return 0;
}

int Graph::StartCHP(string filename){
	NodeOrder.assign(nodenum, -1);
	for(int nodeID=0;nodeID<nodenum;nodeID++)
		vNodeOrder.push_back(nodeID);

	AdjaShort.assign(Neighbor.begin(), Neighbor.end());
	vvNode.assign(Neighbor.begin(), Neighbor.end());

	fstream file;
	file.open(filename);
	if(!file){
		CHPcons();
		writeShortCut(filename);
		ReadShortCut(filename);
	}
	else
	{
		ReadShortCut(filename);
	}

	return 0;
}

void Graph::CHPcons(){
	vector<pair<int,int>> nodeinfor;
	vvpShortCut.assign(nodenum, nodeinfor);
	vnContractedNeighborsOld.assign(nodenum, 0);
	vnContractedNeighbors.assign(nodenum, 0);
	vector<bool> vbVisited(nodenum, false);
	map<int, int> mPosition;
	mPosition[0]=nodenum-1;
	bool bUpdated;
	vector<pair<int,int>> vU,vW;

	//cout<<"Building static CH "<<endl;

	int v;// current contracting vertex
	int NOwCount=0;
	for(int i=0;i<vNodeOrder.size()-1;i++){
		if(i%10000==0) cout<<"i=/////////////////////// "<<i<<endl;
		bUpdated=true;
		while(bUpdated){
			if(vnContractedNeighbors[vNodeOrder[i]]>0){
				if(vnContractedNeighbors[vNodeOrder[i]]>vnContractedNeighborsOld[vNodeOrder[i]])
					bUpdated=NodeOrderUpdate(vNodeOrder[i],i,mPosition);
				else
					bUpdated=false;
			}else
				bUpdated=false;
		}
		v=vNodeOrder[i];
		vU=vvNode[v];
		//cout<<"contracting node "<<v<<endl;
		for(auto ivpr=vU.begin();ivpr!=vU.end();ivpr++){
			CHcontraction(NOwCount, (*ivpr).first, v, vbVisited, (*ivpr).second, vU);
		}

		//deleting v from G
		for(auto ivp = vvNode[v].begin(); ivp != vvNode[v].end(); ivp++)
		{
			for(auto ivpr = vvNode[(*ivp).first].begin(); ivpr != vvNode[(*ivp).first].end(); ivpr++)
				if((*ivpr).first == v)
				{
					vvNode[(*ivp).first].erase(ivpr);
					break;
				}
		}
		vbVisited[v]=true;
	}

	for(int i=0;i<vNodeOrder.size();i++){
		NodeOrder[vNodeOrder[i]]=i;
	}
}

bool Graph::NodeOrderUpdate(int nodeID, int index, map<int, int>& mPosition)
{
	map<int, int>::iterator imp1, imp2, imp3;
	vector<int>::iterator iIndex;

	for(imp1 = mPosition.begin(); (*imp1).first <= vnContractedNeighborsOld[nodeID] && imp1 != mPosition.end(); imp1++);
	imp1--;

	for(imp2 = mPosition.begin(); (*imp2).first <= vnContractedNeighbors[nodeID] && imp2 != mPosition.end(); imp2++);
	imp2--;

	if(mPosition.find(vnContractedNeighbors[nodeID]) == mPosition.end())
	{
		mPosition[vnContractedNeighbors[nodeID]] = (*imp2).second;
	}

	for(imp2 = mPosition.begin(); (*imp2).first <= vnContractedNeighbors[nodeID] && imp2 != mPosition.end(); imp2++);
	imp2--;

	for(; (*imp1).first < (*imp2).first; imp1++)
	{
		(*imp1).second--;
	}
	iIndex = vNodeOrder.begin() + index;
	vNodeOrder.erase(iIndex);
	iIndex = vNodeOrder.begin() + (*imp2).second;
	vNodeOrder.insert(iIndex, nodeID);

	vnContractedNeighborsOld[nodeID] = vnContractedNeighbors[nodeID];

	return true;
}

int Graph::CHcontraction(int &NOwCount, int ID1, int ID2, vector<bool>& vbVisited, int dUV, vector<pair<int, int> >& vW){
	benchmark::heap<2,int,int> Heap(nodenum);
	vector<int> vDistance(nodenum, INF);
	int topNodeID, topDistance, neighborNodeID, neighborLength;
	int w;
	vDistance[ID1]=0;
	Heap.update(ID1,0);
	int count=0;
	map<int,int> mWDistance;
	vector<pair<int,int>> vpWDistance;
	map<int,int> mDistance;
	int maxWDistance=-1;

	for(auto ivp=vW.begin();ivp!=vW.end();ivp++){
		w=(*ivp).first;
		if(w>ID1){//to get rid of redundant computation
			if(vbVisited[w] || ID1==w)
				continue;
			int d=(*ivp).second+dUV;
			mWDistance[w]=d;
			if(d>maxWDistance)
				maxWDistance=d;
			mDistance[w]=INF;
		}
	}
	if(mDistance.empty()){
		NOwCount+=1;
		return 0;
	}
	for(auto imWDistance=mWDistance.begin();imWDistance!=mWDistance.end();imWDistance++)
		vpWDistance.push_back(make_pair((*imWDistance).first, (*imWDistance).second));

	int dThreshold=maxWDistance;

	while(!Heap.empty()){
		Heap.extract_min(topNodeID, topDistance);
		if(vbVisited[topNodeID])
			continue;
		if(topDistance>dThreshold)
			break;
		for(auto ivp=vvNode[topNodeID].begin();ivp!=vvNode[topNodeID].end();ivp++){
			neighborNodeID = (*ivp).first;
			neighborLength = (*ivp).second;
			if(vbVisited[neighborNodeID] || neighborNodeID==ID2)
				continue;
			int d = vDistance[topNodeID] + neighborLength;
			if(vDistance[neighborNodeID] ==INF && neighborNodeID != ID1)
			{
				vDistance[neighborNodeID] = d;
				Heap.update(neighborNodeID, d);
				if(mWDistance.find(neighborNodeID) != mWDistance.end())
					mDistance[neighborNodeID] = d;
			}else if(vDistance[neighborNodeID]>d){
				vDistance[neighborNodeID] = d;
				Heap.update(neighborNodeID, d);
				if(mWDistance.find(neighborNodeID) != mWDistance.end())
					mDistance[neighborNodeID] = d;
			}
			else if(vDistance[neighborNodeID] == d)
			{
				if(mWDistance.find(neighborNodeID) != mWDistance.end())
					mDistance[neighborNodeID] = d;
			}
		}
	}

	for(auto imDistance = mDistance.begin(); imDistance != mDistance.end(); imDistance++)
	{
		if((*imDistance).second > mWDistance[(*imDistance).first])
		{
			int w = (*imDistance).first;
			int distance = mWDistance[(*imDistance).first];

			vvpShortCut[ID1].push_back(make_pair(w, distance));
			vvNode[ID1].push_back(make_pair(w, distance));

			vvpShortCut[w].push_back(make_pair(ID1, distance));
			vvNode[w].push_back(make_pair(ID1, distance));
			//OutEdgesV[ID1].push_back(make_pair(w, make_pair(distance,ID2)));

			vnContractedNeighbors[ID1]++;
			vnContractedNeighbors[w]++;
			count++;
		}
	}

	//cout<<"shortcut number "<<count<<endl;
	return 0;
}

vector<int> _DD,_DD2;
struct DegComp{
	int x;
	DegComp(int _x){
		x=_x;
	}
	bool operator< (const DegComp d) const{
		if(_DD[x]!=_DD[d.x])
			return _DD[x]<_DD[d.x];
		if(_DD2[x]!=_DD2[x])
			return _DD2[x]<_DD2[d.x];
		return x<d.x;
	}
};

void Graph::CHcons(){
	//initialize E
	map<int,pair<int,int>> m;
	E.assign(nodenum,m);
	for(int i=0;i<Neighbor.size();i++){
		for(int j=0;j<Neighbor[i].size();j++)
			E[i].insert(make_pair(Neighbor[i][j].first,make_pair(Neighbor[i][j].second,1)));
	}

	_DD.assign(nodenum,0);_DD2.assign(nodenum,0);
	DD.assign(nodenum,0); DD2.assign(nodenum,0);

	set<DegComp> Deg;
	int degree;
	for(int i=0;i<nodenum;i++){
		degree=Neighbor[i].size();
		if(degree!=0){
			_DD[i]=degree;
			_DD2[i]=degree;
			DD[i]=degree;
			DD2[i]=degree;
			Deg.insert(DegComp(i));
		}
	}

	vector<bool> exist; exist.assign(nodenum,true);
	vector<bool> change; change.assign(nodenum,false);

	vector<pair<int,pair<int,int>>> vect;
	NeighborCon.assign(nodenum,vect); //NeighborCon.clear();
	//SCconNodes.clear();

	//cout<<"Begin to contract"<<endl;
	int count=0;

	while(!Deg.empty()){
		//if(count%10==0) cout<<"count "<<count<<endl;
		count+=1;
		int x=(*Deg.begin()).x;

		while(true){
			if(change[x])
				Deg.erase(DegComp(x));
				_DD[x]=DD[x];
				_DD2[x]=DD2[x];
				Deg.insert(DegComp(x));
				change[x]=false;
				x=(*Deg.begin()).x;
			}else
				break;
		}

		vNodeOrder.push_back(x);
		Deg.erase(Deg.begin());
		exist[x]=false;

		vector<pair<int,pair<int,int>>> Neigh; //Neigh.clear();

		for(auto it=E[x].begin();it!=E[x].end();it++){
			if(exist[(*it).first]){
				Neigh.push_back(*it);
			}
		}
		NeighborCon[x].assign(Neigh.begin(),Neigh.end());

		for(int i=0;i<Neigh.size();i++){
			int y=Neigh[i].first;
			deleteE(x,y);
			change[y]=true;
		}

		for(int i=0;i<Neigh.size();i++){
			for(int j=i+1;j<Neigh.size();j++){
				insertE(Neigh[i].first,Neigh[j].first,Neigh[i].second.first+Neigh[j].second.first);
				change[Neigh[i].first]=true;
				change[Neigh[j].first]=true;
				if(Neigh[i].first<Neigh[j].first)
					SCconNodes[make_pair(Neigh[i].first,Neigh[j].first)].push_back(x);//no direction
				else if(Neigh[j].first<Neigh[i].first)
					SCconNodes[make_pair(Neigh[j].first,Neigh[i].first)].push_back(x);
			}
		}
	}

	NodeOrder.assign(nodenum,-1);
	for(int k=0;k<vNodeOrder.size();k++){
		NodeOrder[vNodeOrder[k]]=k;
	}
	//cout<<"Finish Contract"<<endl;
}

//CH contraction without pruning; order known beforehand
void Graph::CHconsorder(){
	//initialize E
	map<int,pair<int,int>> m;
	E.assign(nodenum,m);
	for(int i=0;i<Neighbor.size();i++){
		for(int j=0;j<Neighbor[i].size();j++)
			E[i].insert(make_pair(Neighbor[i][j].first,make_pair(Neighbor[i][j].second,1)));
	}

	vector<bool> exist; exist.assign(nodenum,true);
	vector<bool> change; change.assign(nodenum,false);

	//cout<<"Begin to contract"<<endl;

	for(int nodeorder=0;nodeorder<nodenum;nodeorder++){
		int x=vNodeOrder[nodeorder];
		if(x!=-1){//to identify and exclude the isolated vertices
			exist[x]=false;

			vector<pair<int,pair<int,int>>> Neigh; //Neigh.clear();

			for(auto it=E[x].begin();it!=E[x].end();it++){
				if(exist[(*it).first]){
					Neigh.push_back(*it);
				}
			}
			NeighborCon[x].assign(Neigh.begin(),Neigh.end());

			for(int i=0;i<Neigh.size();i++){
				int y=Neigh[i].first;
				deleteEorder(x,y);
			}

			for(int i=0;i<Neigh.size();i++){
				for(int j=i+1;j<Neigh.size();j++){
					insertEorder(Neigh[i].first,Neigh[j].first,Neigh[i].second.first+Neigh[j].second.first);
					if(Neigh[i].first<Neigh[j].first)
						SCconNodes[make_pair(Neigh[i].first,Neigh[j].first)].push_back(x);//no direction
					else if(Neigh[j].first<Neigh[i].first)
						SCconNodes[make_pair(Neigh[j].first,Neigh[i].first)].push_back(x);
				}
			}
		}
	}
}

void Graph::insertE(int u,int v,int w){
	if(E[u].find(v)==E[u].end()){
		E[u].insert(make_pair(v,make_pair(w,1)));
		DD[u]++;
		DD2[u]++;
	}
	else{
		if(E[u][v].first>w)
			E[u][v]=make_pair(w,1);
		else if(E[u][v].first==w)
			E[u][v].second+=1;
	}

	if(E[v].find(u)==E[v].end()){
		E[v].insert(make_pair(u,make_pair(w,1)));
		DD[v]++;
		DD2[v]++;
	}
	else{
		if(E[v][u].first>w)
			E[v][u]=make_pair(w,1);
		else if(E[v][u].first==w)
			E[v][u].second+=1;
	}
}

void Graph::insertEorder(int u,int v,int w){
	if(E[u].find(v)==E[u].end()){
		E[u].insert(make_pair(v,make_pair(w,1)));
		//DD[u]++;
		//DD2[u]++;
	}
	else{
		if(E[u][v].first>w)
			E[u][v]=make_pair(w,1);
		else if(E[u][v].first==w)
			E[u][v].second+=1;
	}

	if(E[v].find(u)==E[v].end()){
		E[v].insert(make_pair(u,make_pair(w,1)));
		//DD[v]++;
		//DD2[v]++;
	}
	else{
		if(E[v][u].first>w)
			E[v][u]=make_pair(w,1);
		else if(E[v][u].first==w)
			E[v][u].second+=1;
	}
}

void Graph::deleteE(int u,int v){
	if(E[u].find(v)!=E[u].end()){
		E[u].erase(E[u].find(v));
		DD[u]--;
	}

	if(E[v].find(u)!=E[v].end()){
		E[v].erase(E[v].find(u));
		DD[v]--;
	}
}

void Graph::deleteEorder(int u,int v){
	if(E[u].find(v)!=E[u].end()){
		E[u].erase(E[u].find(v));
		//DD[u]--;
	}

	if(E[v].find(u)!=E[v].end()){
		E[v].erase(E[v].find(u));
		//DD[v]--;
	}
}

void Graph::CHconsMT(){
	//initialize SCconNodesMT
	map<int, vector<int>> mi;
	SCconNodesMT.assign(nodenum, mi);

	//initialize E
	map<int,pair<int,int>> m;
	E.assign(nodenum,m);
	for(int i=0;i<Neighbor.size();i++){
		for(int j=0;j<Neighbor[i].size();j++)
			E[i].insert(make_pair(Neighbor[i][j].first,make_pair(Neighbor[i][j].second,1)));
	}

	_DD.assign(nodenum,0);_DD2.assign(nodenum,0);
	DD.assign(nodenum,0); DD2.assign(nodenum,0);

	set<DegComp> Deg;
	int degree;
	for(int i=0;i<nodenum;i++){
		degree=Neighbor[i].size();
		if(degree!=0){
			_DD[i]=degree;
			_DD2[i]=degree;
			DD[i]=degree;
			DD2[i]=degree;
			Deg.insert(DegComp(i));
		}
	}

	vector<bool> exist; exist.assign(nodenum,true);
	vector<bool> change; change.assign(nodenum,false);

	vector<pair<int,pair<int,int>>> vect;
	NeighborCon.assign(nodenum,vect); //NeighborCon.clear();
	//SCconNodes.clear();

	//cout<<"Begin to contract"<<endl;
	int count=0;

	while(!Deg.empty()){
		if(count%10000==0)
			cout<<"count "<<count<<endl;
		count+=1;
		int x=(*Deg.begin()).x;

		while(true){
			if(change[x]){
				Deg.erase(DegComp(x));
				_DD[x]=DD[x];
				_DD2[x]=DD2[x];
				Deg.insert(DegComp(x));
				change[x]=false;
				x=(*Deg.begin()).x;
			}else
				break;
		}

		vNodeOrder.push_back(x);
		Deg.erase(Deg.begin());
		exist[x]=false;

		vector<pair<int,pair<int,int>>> Neigh; //Neigh.clear();

		for(auto it=E[x].begin();it!=E[x].end();it++){
			if(exist[(*it).first]){
				Neigh.push_back(*it);
			}
		}
		NeighborCon[x].assign(Neigh.begin(),Neigh.end());

		//multi threads for n^2 combination
		//map<int,int> Neighmap;
		for(int i=0;i<Neigh.size();i++){
			int y=Neigh[i].first;
			deleteE(x,y);
			change[y]=true;
			//Neighmap[y]=Neigh[i].second.first;
		}

		int stepf=Neigh.size()/threadnum;
		boost::thread_group threadf;
		for(int i=0;i<threadnum;i++){
			pair<int,int> p;
			p.first=i*stepf;
			if(i==threadnum-1)
				p.second=Neigh.size();
			else
				p.second=(i+1)*stepf;
			threadf.add_thread(new boost::thread(&Graph::NeighborCom, this, boost::ref(Neigh), p, x));
		}
		threadf.join_all();
	}

	NodeOrder.assign(nodenum,-1);
	for(int k=0;k<vNodeOrder.size();k++){
		NodeOrder[vNodeOrder[k]]=k;
	}
	//cout<<"Finish Contract"<<endl;
}

void Graph::NeighborCom(vector<pair<int,pair<int,int>>> &Neighvec, pair<int,int> p, int x){
	sm->wait();
	int ID1, w1;
	int ID2, w2;
	for(int k=p.first;k<p.second;k++){
		ID1=Neighvec[k].first;
		w1=Neighvec[k].second.first;
		for(int h=0;h<Neighvec.size();h++){
			ID2=Neighvec[h].first;
			w2=Neighvec[h].second.first;
			insertEMT(ID1, ID2, w1+w2);
			if(ID1<ID2)
				SCconNodesMT[ID1][ID2].push_back(x);
		}
	}
	sm->notify();
}

void Graph::insertEMT(int u,int v,int w){
	if(E[u].find(v)==E[u].end()){
		E[u].insert(make_pair(v,make_pair(w,1)));
		DD[u]++;
		DD2[u]++;
	}
	else{
		if(E[u][v].first>w)
			E[u][v]=make_pair(w,1);
		else if(E[u][v].first==w)
			E[u][v].second+=1;
	}
}

void Graph::CHconsMTOrderGenerate(){
	int Twidth=0;//tree width
	//initialize SCconNodesMT
	map<int, vector<int>> mi;
	SCconNodesMT.assign(nodenum, mi);

	//initialize E
	map<int,pair<int,int>> m;
	E.assign(nodenum,m);
	for(int i=0;i<Neighbor.size();i++){
		for(int j=0;j<Neighbor[i].size();j++)
			E[i].insert(make_pair(Neighbor[i][j].first,make_pair(0,1)));
	}

	_DD.assign(nodenum,0);_DD2.assign(nodenum,0);
	DD.assign(nodenum,0); DD2.assign(nodenum,0);

	set<DegComp> Deg;
	int degree;
	for(int i=0;i<nodenum;i++){
		degree=Neighbor[i].size();
		if(degree!=0){
			_DD[i]=degree;
			_DD2[i]=degree;
			DD[i]=degree;
			DD2[i]=degree;
			Deg.insert(DegComp(i));
		}
	}

	vector<bool> exist; exist.assign(nodenum,true);
	vector<bool> change; change.assign(nodenum,false);

	vector<pair<int,pair<int,int>>> vect;
	NeighborCon.assign(nodenum,vect); //NeighborCon.clear();
	//SCconNodes.clear();

	//cout<<"Begin to contract"<<endl;
	int count=0;

	while(!Deg.empty()){
		if(count%10000==0)
			cout<<"count "<<count<<" , treewidth "<<Twidth<<endl;
		count+=1;
		int x=(*Deg.begin()).x;

		while(true){
			if(change[x]){
				Deg.erase(DegComp(x));
				_DD[x]=DD[x];
				_DD2[x]=DD2[x];
				Deg.insert(DegComp(x));
				change[x]=false;
				x=(*Deg.begin()).x;
			}else
				break;
		}

		vNodeOrder.push_back(x);
		Deg.erase(Deg.begin());
		exist[x]=false;

		vector<pair<int,pair<int,int>>> Neigh; //Neigh.clear();

		for(auto it=E[x].begin();it!=E[x].end();it++){
			if(exist[(*it).first]){
				Neigh.push_back(*it);
			}
		}

		if(Neigh.size()>Twidth)
			Twidth=Neigh.size();

		NeighborCon[x].assign(Neigh.begin(),Neigh.end());

		//multi threads for n^2 combination
		//map<int,int> Neighmap;
		for(int i=0;i<Neigh.size();i++){
			int y=Neigh[i].first;
			deleteE(x,y);
			change[y]=true;
			//Neighmap[y]=Neigh[i].second.first;
		}

		int stepf=Neigh.size()/threadnum;
		boost::thread_group threadf;
		for(int i=0;i<threadnum;i++){
			pair<int,int> p;
			p.first=i*stepf;
			if(i==threadnum-1)
				p.second=Neigh.size();
			else
				p.second=(i+1)*stepf;
			threadf.add_thread(new boost::thread(&Graph::NeighborComOrderGenerate, this, boost::ref(Neigh), p, x));
		}
		threadf.join_all();
	}

	NodeOrder.assign(nodenum,-1);
	for(int k=0;k<vNodeOrder.size();k++){
		NodeOrder[vNodeOrder[k]]=k;
	}
	cout<<"Finish Contract"<<" , treewidth "<<Twidth<<endl;
}

void Graph::NeighborComOrderGenerate(vector<pair<int,pair<int,int>>> &Neighvec, pair<int,int> p, int x){
	sm->wait();
	int ID1, w1;
	int ID2, w2;
	for(int k=p.first;k<p.second;k++){
		ID1=Neighvec[k].first;
		for(int h=0;h<Neighvec.size();h++){
			ID2=Neighvec[h].first;
			insertEMTOrderGenerate(ID1, ID2, 1);
		}
	}
	sm->notify();
}

void Graph::insertEMTOrderGenerate(int u,int v,int w){
	if(E[u].find(v)==E[u].end()){
		E[u].insert(make_pair(v,make_pair(w,1)));
		DD[u]++;
		DD2[u]++;
	}
}
