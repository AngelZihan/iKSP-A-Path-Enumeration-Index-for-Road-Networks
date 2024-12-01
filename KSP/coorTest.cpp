#include "graph.h"

int main()
{
	//string filename = "./beijingNodeNew";
	//string filename = "./ExampleGraph2";  
	//string filename = "./map/USA-road-d.NY.gr";
	//string costname = "./map/USA-road-t.NY.gr";
	double coorThreshold = 0.000001;
//	string filename = "./USA-Road-Networks/USA-distance-time/USA-CTR.txt";  
	string filename = "./map/E-USA-d.gr";   
	Graph g = Graph(filename); 
	g.readUSCoor("./map/USA-road-d.E.co");   
	vector<double> vLat, vLon;
	for(int i = 0; i < g.nodeNum; i++)
	{
		for(auto & neighborNode : g.adjList[i])
		{
			double lonDif = abs(g.vCoor[i].first - g.vCoor[neighborNode.first].first); 
			double latDif = abs(g.vCoor[i].second - g.vCoor[neighborNode.first].second); 
			if(lonDif < coorThreshold)
			{
				double latTmp = neighborNode.second / latDif;
				if(latTmp < 99999999)
					vLat.push_back(neighborNode.second / latDif);
			}
			else if(latDif < coorThreshold) 
			{
				double lonTmp = neighborNode.second / lonDif;
				if(lonTmp < 99999999) 
					vLon.push_back(neighborNode.second / lonDif);
			}
		}
	}

	double lonSum = 0;
	cout << "CTR Lon:";
	for(auto& lon: vLon)
	{
		lonSum += lon;
		cout << lon << "\t";
	}
	cout << endl << lonSum / vLon.size() << endl;
	
	double latSum = 0;
	cout << "CTR Lat:";
	for(auto& lat: vLat)
	{
		latSum += lat;
		cout << lat << "\t";
	}
	cout << endl << latSum / vLat.size() << endl;
}
