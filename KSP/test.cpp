#include "graph.h"
	
void Graph::testCSP(string filename)
{
	int num = 500; 
	ifstream inTest(filename.c_str()); 
	vector<int> vID1(num, -1);
	vector<int> vID2(num, -1);
	vector<int> vC(num, -1);
	int ID1, ID2, C;
	for(int i = 0; i < num; i++)
	{
		inTest >> ID1 >> ID2 >> C;
		vID1[i] = ID1-1;
		vID2[i] = ID2-1;
		vC[i] = C;
	}
	
	vector<int> vDeKSP(num, -1);  
	vector<int> vDcKSP(num, -1);  
	
	std::chrono::high_resolution_clock::time_point t1;
	std::chrono::high_resolution_clock::time_point t2;
	std::chrono::duration<double> time_span; 
	int i;
	double te = 0;
	double tc = 0; 
	int errorCount = 0;
//	for(i = 176; i < num; i++) 
	for(i = 7; i < num; i++) 
	{
		cout << i << ":" << vID1[i] << "\t" << vID2[i] << "\t" << vC[i] << endl; 

		t1 = std::chrono::high_resolution_clock::now(); 
		vDeKSP[i] = eKSP(vID1[i], vID2[i], vC[i]); 
		t2 = std::chrono::high_resolution_clock::now(); 
		cout << vDeKSP[i] << endl;
		time_span = std::chrono::duration_cast<std::chrono::duration<double> >(t2 - t1); 
		cout << "eKSP Time:" <<time_span.count() << endl; 
		te += time_span.count();	

				
		t1 = std::chrono::high_resolution_clock::now(); 
//		pair<int, int> p = fKSPCSP(vID1[i], vID2[i], vC[i]); 
//		vDcKSP[i] = p.first;  
		vDcKSP[i] = kspCSP(vID1[i], vID2[i], vC[i]); 
		t2 = std::chrono::high_resolution_clock::now(); 
		time_span = std::chrono::duration_cast<std::chrono::duration<double> >(t2 - t1); 
		cout << vDcKSP[i] << endl;
		cout << "cKSP Time:" <<time_span.count() << endl; 
		tc += time_span.count();
//		break
		cout << endl;

		if(vDeKSP[i] != vDcKSP[i]) 
		{
			errorCount++;
			cout << "Error! " << vDeKSP[i] << "\t" <<vDcKSP[i] << endl;
		}
	
//		break;
	}

	cout << "eKSP averge: " << te / num << endl;
	cout << "cKSP averge: " << tc / num << endl; 
	cout << "Error Count:" << errorCount << endl;
/*
	t1 = std::chrono::high_resolution_clock::now(); 
	for(i = 0; i < num; i++) 
	{
		cout << i << ":" << vID1[i] << "\t" << vID2[i] << "\t" << vC[i] << endl;
		vDeKSP[i] = eKSP(vID1[i], vID2[i], vC[i]); 
		cout << vDeKSP[i] << endl;
//		break;
	}
	t2 = std::chrono::high_resolution_clock::now();
	time_span = std::chrono::duration_cast<std::chrono::duration<double> >(t2 - t1); 
	cout << "eKSP Time:" <<time_span.count() / (i+1) << endl;
	cout << vDeKSP[0] << endl;
*/	
}	

