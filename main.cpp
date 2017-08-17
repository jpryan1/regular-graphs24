



#include <iostream>
#include <fstream>
#include <unistd.h>
#include <vector>
#include "Bank.h"

void canonize(graph* g){
	//This function converts the graph g into its canonical form
	int lab[VERTICES];
	int ptn[VERTICES];
	int orbits[VERTICES];
	DEFAULTOPTIONS_GRAPH(options);
	options.getcanon = true;
	statsblk stats;
	graph canonized[VERTICES];
	densenauty(g, lab, ptn, orbits, &options, &stats, 1, VERTICES, canonized);
	memcpy(g, canonized, sizeof(graph)*VERTICES);
}

int hasEdge(graph* g, int i, int j){
	return ((g[i] & bit[j])>0 || (g[j] & bit[i])>0);
}
void addEdge(graph* g, int a, int b){
	ADDONEEDGE(g, a, b, 1);
}
void deleteEdge(graph*g, int a, int b){
	DELELEMENT(g+a, b);
	DELELEMENT(g+b, a);
}

void setup(graph* g, int setting){
	//This function sets up the vertices w,x,y,z based on which of 11 possible configurations they could have
	if(setting!=11) addEdge(g, 2, 3);
	if(setting < 9)	addEdge(g, 2, 4);
	if(setting==1||
	   setting==2||
	   setting==5 )	addEdge(g, 2, 5);
	if(setting==1||
	   setting==4||
	   setting==6)	addEdge(g, 3, 4);
	if(setting==1||
	   setting==2||
	   setting==3)	addEdge(g, 3, 5);
	if(setting<5 ||
	   setting==7||
	   setting==9)	addEdge(g, 4, 5);
}

void connectUV(graph* g, int setting){
	//This function sets up the adjacencies between u,v and w,x,y,z based on which of 81 possible matchings they could have
	//Each of w,x,y,z must be connected to one or both of u,v. Thus, each vertex has 3 choices, hence 3^4 = 81 possible matchings
	int temp;
	for(int i=2; i<6; i++){
		temp = setting % 3;
		setting = setting / 3;
		switch(temp){
			case 0:
				addEdge(g, 0, i);
				break;
			case 1:
				addEdge(g, 1, i);
				break;
			case 2:
				addEdge(g, 0, i);
				addEdge(g, 1, i);
				break;
			default:
				break;
		}
	}
}
void extraSetup(graph* g, int extra, int setting){
	//this function configures the adjacencies for w,x,y,z, etc with the extra vertices
	//example: if extra == 2, then we are attaching beta to w,x,y,z, and alpha.
	
	for(int i=2; i<5+extra; i++){
		int temp = setting%2;
		setting = setting /2;
		if(temp){
			addEdge(g, i, 5+extra);
		}
	}
	
}


graph* makeCopy(graph* g){
	//This function makes a copy which can be added to the bank.
	graph* copy = new graph[VERTICES];
	memcpy(copy, g, sizeof(graph)*VERTICES);
	return copy;
}

int alreadyChecked(graph* g, Bank& b){
	
	graph* copy = makeCopy(g);
	canonize(copy);
	int added = b.add(copy);
	delete [] copy;
	return !added;
}


int check24regular(graph* g, int extra, int* m){
	//extra is number of extra vertices, after first 6
	
	//This may be slow, but hopefully 10 vertices is manageable
	//returns 0 if a pair w/ >5 has been found,
	//returns 1 if no pair w/ >5 has been found, and
	//returns 2 if graph is (2,4) regular
	bool isReg = true;
	int min = 4;
	for(int i=0; i < 5 + extra; i++){
		for(int j=i+1; j < 6 + extra; j++){
			if(hasEdge(g, i, j)) continue;
			//For each pair of nonadjacent vertices
			int counter=0;
			for(int k=0; k<6+extra; k++){
				//how many of the other vertices are connected to at least one in the pair
				if(k!=i && k!=j
				   && ( hasEdge(g,i,k)
					   || hasEdge(g,j,k))){
					   counter++;
				   }
			}
			if(counter > 4) return 0;
			if(counter < 4){
			
				isReg = false;
				if( counter < min ){
					min = counter;
				}
			}
			
		}
	}
	*m = min;
	if(isReg) return 2;
	return 1;
	
}

void print(graph* g){
	//This function gives output which the python file can convert to images
	for(int i=0; i<VERTICES-1; i++){
		for(int j=i+1; j<VERTICES; j++){
			if(hasEdge(g, i, j)){
				std::cout<<i<<","<<j<<std::endl;
			}
		}
	}std::cout<<"end graph"<<std::endl;
}
int main(int argc, char** argv){
	//CONVENTION:
	//Vertex 0 is u, vertex 1 is v, and vertices 2-5 are w,x,y,z
	
	
	clock_t begin = clock();
	
	
	graph g[VERTICES];
	graph g_preUV[VERTICES];
	graph g_preExtra[VERTICES];
	//The array g_preUV will save the w,x,y,z configuration so the function doesn't need to be called many times
	graph* copy;
	Bank bank;
	int counter=0;
	Bank winners;
	//For each of 11 setups of the w,x,y,z:
	for(int i=1; i<=11; i++){
		//zero out the graph
		memset(g, 0, VERTICES*sizeof(graph));
		//Set up the ith graph
		setup(g, i);
		
		//save the setup
		memcpy(g_preUV, g, sizeof(graph)*VERTICES);

		//For each of the 3^4 = 81 setups with u,v
		for(int j=0; j<81; j++){
			
			//setup uv edges
			connectUV(g, j);

			//if already checked, continue
			if(alreadyChecked(g, bank)){
				//delete uv edges
				memcpy(g, g_preUV, sizeof(graph)*VERTICES);
				continue;
			}
			
			
			//check if (2,4) reg
			int min = 0;
			int reg = check24regular(g, 0, &min);
			//If a pair has neighbor set > 4, skip
			if(reg == 0){
				//delete uv edges
				memcpy(g, g_preUV, sizeof(graph)*VERTICES);
				continue;
			}
			if(reg == 2){
				counter++;
				//Print number of vertices before graph
				std::cout<<6<<std::endl;
				print(g);
				
			}
			
			
			int extra_settings = 16; //for alpha, there are 16 edge addition combinations =2^4
			std::vector<graph*> current_graphs;
			std::vector<graph*> next_graphs;
			current_graphs.push_back(makeCopy(g));
			
			
			for(int extra = 1; extra <= 4; extra++){
				for(int k = 0; k<current_graphs.size(); k++){
					//for each graph to add extra vertices to
					
					graph* currentGraph = current_graphs[k];
					memcpy(g_preExtra, currentGraph, VERTICES*sizeof(graph));
					//save configuration without alpha, beta etc edges
					
					for(int m = 0; m < extra_settings; m++){
						//for every adjacency set of the new vertex
						extraSetup(currentGraph, extra, m);//now currentGraph has alpha, beta edges
						if(alreadyChecked(currentGraph, bank)){
							memcpy(currentGraph, g_preExtra, VERTICES*sizeof(graph));
							//TODO are we not zeroing out beta edges somewhere?
							continue;
						}
						
						reg = check24regular(currentGraph, extra, &min);
						if(reg == 0){
							memcpy(currentGraph, g_preExtra, VERTICES*sizeof(graph));
							continue;
						}
						
						if(reg == 2){
							counter++;
							std::cout<<(extra+6)<<std::endl;
							print(currentGraph);

						}
						if( min >= extra ){ // this is a HUGE shortcut - no need to consider further if min vertex
							//can't have 4 by adding more extras
							next_graphs.push_back(makeCopy(currentGraph));
						}
						memcpy(currentGraph, g_preExtra, VERTICES*sizeof(graph));
						
					}
				}
				
				for(int k=0; k<current_graphs.size();k++) delete[] current_graphs[k]; //delete the arrays
				current_graphs.clear(); //get rid of the pointers
				for(int k=0; k<next_graphs.size(); k++) current_graphs.push_back(next_graphs[k]);
				next_graphs.clear();
				extra_settings *= 2;
				
			}
			for(int k=0; k<current_graphs.size();k++) delete[] current_graphs[k]; //delete the arrays
			
			
			memcpy(g, g_preUV, sizeof(graph)*VERTICES);
		}
		
		
		
	}
	std::cout<<"Found "<<counter<<std::endl;	
	clock_t end = clock();
	std::cout<<((end-begin+0.0)/CLOCKS_PER_SEC)<<"seconds elapsed"<<std::endl;
	return 0;
}
















