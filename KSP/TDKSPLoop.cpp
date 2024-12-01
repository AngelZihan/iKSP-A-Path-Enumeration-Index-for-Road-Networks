#include "graph.h"

int Graph::caveKSPLoop(int ID1, int ID2, int k, bool bForward)
{
	if(!bForward)
	{
		int IDTmp;
		IDTmp = ID1;
		ID1 = ID2;
		ID2 = IDTmp;
	}

	auto it = vmSEPos[ID1].find(ID2);
	if(it == vmSEPos[ID1].end()) 
		return -1;

	if(vmEFinsihedQuery[ID1][ID2]) 
	{
		if(k < vvSEQuery[ID1][(*it).second].vK.size()) 
			return vvSEQuery[ID1][(*it).second].vK[k];  
		else
			return -1;
	}
		
	if(k < vvSEQuery[ID1][(*it).second].vK.size())
		return vvSEQuery[ID1][(*it).second].vK[k]; 

	int dNext;
	while(k >= vvSEQuery[ID1][(*it).second].vK.size())
	{
		seEnum& se = vvSEQuery[ID1][(*it).second];
		if(se.Q.empty()) 
		{
			se.bFinished = true;
			vmEFinsihedQuery[ID1][ID2] = true; 
		}

		//w has finished Update
		if(vmEFinsihedQuery[ID1][ID2]) 
		{
			//When the last w has finished enumeration
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
		se.Q.pop();
		int w = pTop.w; 
		int wPos = se.umWPos[w];
		dNext = pTop.d; 
		se. vK.push_back(dNext); 
		se.vKW.push_back(w);
		se.vSubK.push_back(make_pair(pTop.k1, pTop.k2));
	
		if(w == ID2)
		{
			if(se.Q.empty()) 
			{
				se.bFinished = true;
				vmEFinsihedQuery[ID1][ID2] = true;
			}
			continue;
		}

		//w has finished Update
		if(vmEFinsihedQuery[ID1][w]) 
		{
			//When the last w has finished enumeration
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
			continue;

		//Update w1 when b2 is an edge 
		//b1 and b2 are both edges are covered by vbFinished
		if(b1 && !b2)
		{
			d1Next = caveKSPLoop(ID1, w, se.vR[wPos].first.first+1, true);    

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
			se.vR[wPos].first.first++; 
			subW sw;
			sw.d = d1Next + d2Next;
			sw.w = w; 
			sw.bW = true;
			sw.k1 = se.vR[wPos].first.first; 
			sw.k2 = -1;
			se.Q.push(sw);
		}
		else if(!b1 && b2) //b1 is an edge
		{
			d2Next = caveKSPLoop(ID2, w, se.vR[wPos].first.second+1, false); 
			
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
			se.vR[wPos].first.second++;
			subW sw;
			sw.d = d1Next + d2Next;
			sw.w = w;
			sw.bW = true;
			sw.k1 = -1;
			sw.k2 = se.vR[wPos].first.second;
			se.Q.push(sw);
		}
		else if(b1 && b2) //b1 and b2 are all se
		{
			auto& p11 = se.vR[wPos].first.first;
			auto& p12 = se.vR[wPos].first.second; 
			auto& p21 = se.vR[wPos].second.first;
			auto& p22 = se.vR[wPos].second.second; 
			auto& pvDistance = se.vpvWDistance[wPos]; 

			//w finished enum
			if(p11 == p22 && p12 == p21 && p11 == pvDistance.first.size()-1 && p12 == pvDistance.second.size()-1 && vmEFinsihedQuery[ID1][w] && vmEFinsihedQuery[ID2][w]) 
			{
				se.vbFinished[wPos] = true;  
				if(p11 == 0 && p12 == 0)
					return -1;
				if(k < vvSEQuery[ID1][(*it).second].vK.size()) 
					return vvSEQuery[ID1][(*it).second].vK[k]; 
				else 
					continue;
			}

			//Skip the first (0,0)(0,0) case
			if(p11 == 0 && p12 == 0 && p21 == 0 && p22 == 0)
			{
				if(pvDistance.first[p11] < pvDistance.second[p12])
				{
					if(p12 == pvDistance.second.size()-1 && !vmEFinsihedQuery[ID2][w])  
					{
						d2Next = caveKSPLoop(ID2, w, p12+1, false);   

						if(d2Next == -1)
						{
							se.vpbFinished[wPos].second = true;   
							vmEFinsihedQuery[ID2][w] = true;
						}
						else
							pvDistance.second.push_back(d2Next);
					}
					p12 = (p12+1) % pvDistance.second.size();  
					if(p12 == 0)
					{
						if(p11 == pvDistance.first.size()-1 && !vmEFinsihedQuery[ID1][w])    
						{
							d1Next = caveKSPLoop(ID1, w, p11+1, true);    

							if(d1Next == -1)
							{
								se.vpbFinished[wPos].first = true;  
								vmEFinsihedQuery[ID1][w] = true;  
							}
							else
								pvDistance.first.push_back(d1Next);
						}
						p11 = (p11+1) % pvDistance.first.size();
					}

					if(p22 == pvDistance.first.size()-1 && !vmEFinsihedQuery[ID1][w])   
					{
						d1Next = caveKSPLoop(ID1, w, p22+1, true);  
						if(d1Next == -1)
						{
							se.vpbFinished[wPos].first = true;  
							vmEFinsihedQuery[ID1][w] = true;  
						}
						else
							pvDistance.first.push_back(d1Next);
					}
					p22 = (p22+1) % pvDistance.first.size();
					if(p22 == 0)
					{
						if(p21 == pvDistance.second.size()-1 && !vmEFinsihedQuery[ID2][w])   
						{
							d2Next = caveKSPLoop(ID2, w, p21+1, false);    
							if(d2Next == -1)
							{
								se.vpbFinished[wPos].second = true;  
								vmEFinsihedQuery[ID2][w] = true;  
							}
							else
								pvDistance.second.push_back(d2Next);
						}
						p21 = (p21+1) % pvDistance.second.size();
					}
				}
				else
				{
					if(p11 == pvDistance.first.size()-1 && !vmEFinsihedQuery[ID1][w])   
					{
						d1Next = caveKSPLoop(ID1, w, p11+1, true); 
						if(d1Next == -1)
						{
							se.vpbFinished[wPos].first = true;   
							vmEFinsihedQuery[ID1][w] = true;
						}
						else
							pvDistance.first.push_back(d1Next);
					}
					p11 = (p11+1) % pvDistance.first.size();  
					if(p11 == 0)
					{
						if(p12 == pvDistance.second.size()-1 && !vmEFinsihedQuery[ID2][w])   
						{
							d2Next = caveKSPLoop(ID2, w, p12+1, false);   
							if(d2Next == -1)
							{
								se.vpbFinished[wPos].second = true;   
								vmEFinsihedQuery[ID2][w] = true;
							}
							else
								pvDistance.second.push_back(d2Next);
						}
						p12 = (p12+1) % pvDistance.second.size();
					}

					if(p21 == pvDistance.second.size()-1 && !vmEFinsihedQuery[ID2][w])   
					{
						d2Next = caveKSPLoop(ID2, w, p21+1, false);    
						if(d2Next == -1)
						{
							se.vpbFinished[wPos].second = true;   
							vmEFinsihedQuery[ID2][w] = true;
						}
						else
							pvDistance.second.push_back(d2Next);
					}
					p21 = (p21+1) % pvDistance.second.size();
					if(p21 == 0)
					{
						if(p22 == pvDistance.first.size()-1 && !vmEFinsihedQuery[ID1][w])   
						{
							d1Next = caveKSPLoop(ID1, w, p22+1, true);    

							if(d1Next == -1)
							{
								se.vpbFinished[wPos].first = true;   
								vmEFinsihedQuery[ID1][w] = true;
							}
							else
								pvDistance.first.push_back(d1Next);
						}
						p22 = (p22+1) % pvDistance.first.size();
					}
				}
			}
			
			if(p11 == p22 && p12 == p21 && p11 == pvDistance.first.size()-1 && p12 == pvDistance.second.size() -1 && vmEFinsihedQuery[ID1][w] && vmEFinsihedQuery[ID2][w])  
			{
				se.vbFinished[wPos] = true;  
				if(p11 == 0 && p12 == 0) 
					continue;
				
				d1Next = pvDistance.first[p11];
				d2Next = pvDistance.second[p12];
				subW sw;
				sw.d = d1Next + d2Next;
				sw.w = w; 
				sw.bW = true;
				sw.k1 = p11;
				sw.k2 = p12;
				se.Q.push(sw);
				if(k < vvSEQuery[ID1][(*it).second].vK.size()) 
					return vvSEQuery[ID1][(*it).second].vK[k]; 
				else 
					continue;
			}

			//Point to the same element
			//Apply Rule 3
			if(p11 == p22 && p12 == p21) 
			{
				if(pvDistance.first[p11] < pvDistance.second[p12])
				{
					subW sw;
					sw.d = pvDistance.first[p11]+pvDistance.second[p12];  
					sw.w = w; 
					sw.bW = true;
					sw.k1 = p11;
					sw.k2 = p12;
					se.Q.push(sw);
					if(p12 == pvDistance.second.size()-1 && !vmEFinsihedQuery[ID2][w])   
					{
						d2Next = caveKSPLoop(ID2, w, p12+1, false);   

						if(d2Next == -1)
						{
							se.vpbFinished[wPos].second = true;   
							vmEFinsihedQuery[ID2][w] = true;
						}
						else
							pvDistance.second.push_back(d2Next);
					}
					p12 = (p12+1) % pvDistance.second.size();  
					if(p12 == 0)
					{
						if(p11 == pvDistance.first.size()-1 && !vmEFinsihedQuery[ID1][w])   
						{
							d1Next = caveKSPLoop(ID1, w, p11+1, true);  
							if(d1Next == -1)
							{
								se.vpbFinished[wPos].first = true;   
								vmEFinsihedQuery[ID1][w] = true;
							}
							else
								pvDistance.first.push_back(d1Next);
						}
						p11 = (p11+1) % pvDistance.first.size();
					}

					if(p22 == pvDistance.first.size()-1 && !vmEFinsihedQuery[ID1][w])   
					{
						d1Next = caveKSPLoop(ID1, w, p22+1, true);   
						if(d1Next == -1)
						{
							se.vpbFinished[wPos].first = true;   
							vmEFinsihedQuery[ID1][w] = true;
						}
						else
							pvDistance.first.push_back(d1Next);
					}
					p22 = (p22+1) % pvDistance.first.size();
					if(p22 == 0)
					{
						if(p21 == pvDistance.second.size()-1 && !vmEFinsihedQuery[ID2][w])   
						{
							d2Next = caveKSPLoop(ID2, w, p21+1, false);  

							if(d2Next == -1)
							{
								se.vpbFinished[wPos].second = true;   
								vmEFinsihedQuery[ID2][w] = true;
							}
							else
								pvDistance.second.push_back(d2Next);
						}
						p21 = (p21+1) % pvDistance.second.size();
					}
				}
				else
				{
					subW sw;
					sw.d = pvDistance.second[p12]+pvDistance.first[p11];  
					sw.w = w;
					sw.bW = true;
					sw.k1 = p11;
					sw.k2 = p12;
					se.Q.push(sw);
					if(p11 == pvDistance.first.size()-1 && !vmEFinsihedQuery[ID1][w])   
					{
						d1Next = caveKSPLoop(ID1, w, p11+1, true);   
						if(d1Next == -1)
						{
							se.vpbFinished[wPos].first = true;   
							vmEFinsihedQuery[ID1][w] = true;
						}
						else
							pvDistance.first.push_back(d1Next);
					}
					p11 = (p11+1) % pvDistance.first.size();  
					if(p11 == 0)
					{
						if(p12 == pvDistance.second.size()-1 && !vmEFinsihedQuery[ID2][w])   
						{
							d2Next = caveKSPLoop(ID2, w, p12+1, false);   
							if(d2Next == -1) 
							{
								se.vpbFinished[wPos].second = true;   
								vmEFinsihedQuery[ID2][w] = true;
							}
							else
								pvDistance.second.push_back(d2Next);
						}
						p12 = (p12+1) % pvDistance.second.size();
					}

					if(p21 == pvDistance.second.size()-1 && !vmEFinsihedQuery[ID2][w])   
					{
						d2Next = caveKSPLoop(ID2, w, p21+1, false);   
						if(d2Next == -1)
						{
							se.vpbFinished[wPos].second = true;   
							vmEFinsihedQuery[ID2][w] = true;
						}
						else
							pvDistance.second.push_back(d2Next);
					}
					p21 = (p21+1) % pvDistance.second.size();
					if(p21 == 0)
					{
						if(p22 == pvDistance.first.size()-1 && !vmEFinsihedQuery[ID1][w])   
						{
							d1Next = caveKSPLoop(ID1, w, p22+1, true);   
							if(d1Next == -1)
							{
								se.vpbFinished[wPos].first = true;   
								vmEFinsihedQuery[ID1][w] = true;
							}
							else
								pvDistance.first.push_back(d1Next);
						}
						p22 = (p22+1) % pvDistance.first.size();
					}
				}
			}
			else if(pvDistance.first[p11] + pvDistance.second[p12] <= pvDistance.second[p21] + pvDistance.first[p22])
			{
				if(pvDistance.first[p11] < pvDistance.second[p12])
				{
					subW sw;
					sw.d = pvDistance.first[p11]+pvDistance.second[p12];  
					sw.w = w;
					sw.bW = true;
					sw.k1 = p11;
					sw.k2 = p12;
					se.Q.push(sw);
					if(p12 == pvDistance.second.size()-1 && !vmEFinsihedQuery[ID2][w])   
					{
						d2Next = caveKSPLoop(ID2, w, p12+1, false);   
						if(d2Next == -1)
						{
							se.vpbFinished[wPos].second = true;   
							vmEFinsihedQuery[ID2][w] = true;
						}
						else
							pvDistance.second.push_back(d2Next);
					}
					p12 = (p12+1) % pvDistance.second.size();  
					if(p12 == 0)
					{
						if(p11 == pvDistance.first.size()-1 && !vmEFinsihedQuery[ID1][w])   
						{
							d1Next = caveKSPLoop(ID1, w, p11+1, true);   
							if(d1Next == -1)
							{
								se.vpbFinished[wPos].first = true;   
								vmEFinsihedQuery[ID1][w] = true;
							}
							else
								pvDistance.first.push_back(d1Next);
						}
						p11 = (p11+1) % pvDistance.first.size();
					}
				}
				else
				{
					subW sw;
					sw.d = pvDistance.second[p12]+pvDistance.first[p11];  
					sw.w = w;
					sw.bW = true;
					sw.k1 = p11;
					sw.k2 = p12;
					se.Q.push(sw);
					if(p11 == pvDistance.first.size()-1 && !vmEFinsihedQuery[ID1][w])   
					{ 
						d1Next = caveKSPLoop(ID1, w, p11+1, true);   
						if(d1Next == -1)
						{
							se.vpbFinished[wPos].first = true;   
							vmEFinsihedQuery[ID1][w] = true;
						}
						else
							pvDistance.first.push_back(d1Next);
					}
					p11 = (p11+1) % pvDistance.first.size();  
					if(p11 == 0)
					{
						if(p12 == pvDistance.second.size()-1 && !vmEFinsihedQuery[ID2][w])    
						{
							d2Next = caveKSPLoop(ID2, w, p12+1, false);
							if(d2Next == -1)
							{
								se.vpbFinished[wPos].second = true;   
								vmEFinsihedQuery[ID2][w] = true;
							}
							else
								pvDistance.second.push_back(d2Next);
						}
						p12 = (p12+1) % pvDistance.second.size();
					}
				}
			}
			else if(pvDistance.first[p11] + pvDistance.second[p12] > pvDistance.second[p21] + pvDistance.first[p22])
			{
				if(pvDistance.second[p21] < pvDistance.first[p22])
				{
					subW sw;
					sw.d = pvDistance.second[p21]+pvDistance.first[p22];  
					sw.w = w; 
					sw.bW = true;
					sw.k1 = p22;
					sw.k2 = p21;
					se.Q.push(sw);
					if(p21 == pvDistance.second.size()-1 && !vmEFinsihedQuery[ID2][w])   
					{
						d2Next = caveKSPLoop(ID2, w, p21+1, false);   
						if(d2Next == -1)
						{
							se.vpbFinished[wPos].second = true;   
							vmEFinsihedQuery[ID2][w] = true;
						}
						else
							pvDistance.second.push_back(d2Next);
					}
					p21 = (p21+1) % pvDistance.second.size();
					if(p21 == 0)
					{
						if(p22 == pvDistance.first.size()-1 && !vmEFinsihedQuery[ID1][w])   
						{
							d1Next = caveKSPLoop(ID1, w, p22+1, true);   
							if(d1Next == -1)
							{
								se.vpbFinished[wPos].first = true;   
								vmEFinsihedQuery[ID1][w] = true;
							}
							else
								pvDistance.first.push_back(d1Next);
						}
						p22 = (p22+1) % pvDistance.first.size();
					}
				}
				else
				{
					subW sw;
					sw.d = pvDistance.first[p22]+pvDistance.second[p21];  
					sw.w = w;
					sw.bW = true;
					sw.k1 = p22;
					sw.k2 = p21;
					se.Q.push(sw);
					if(p22 == pvDistance.first.size()-1 && !vmEFinsihedQuery[ID1][w])   
					{
						d1Next = caveKSPLoop(ID1, w, p22+1, true);   
						if(d1Next == -1)
						{
							se.vpbFinished[wPos].first = true;  
							vmEFinsihedQuery[ID1][w] = true;
						}
						else
							pvDistance.first.push_back(d1Next);
					}
					p22 = (p22+1) % pvDistance.first.size();
					if(p22 == 0)
					{
						if(p21 == pvDistance.second.size()-1 && !vmEFinsihedQuery[ID2][w])   
						{
							d2Next = caveKSPLoop(ID2, w, p21+1, false);   
							if(d2Next == -1)
							{
								se.vpbFinished[wPos].second = true;   
								vmEFinsihedQuery[ID2][w] = true;
							}
							else
								pvDistance.second.push_back(d2Next);
						}
						p21 = (p21+1) % pvDistance.second.size();
					}
				}
			}

		}
	}
	
	return vvSEQuery[ID1][(*it).second].vK[k]; 
}

