
#include "Bank.h"

int Bank::add(graph* g){ //RETURNS 1 IF ADDED, 0 IF ALREADY IN BANK
	
	if(!root){
		root = new BankNode();
		memcpy(root->g, g, sizeof(graph)*VERTICES);
		size++;
		return 1;
	}
	int temp = recursiveAdd(g, root);
	if(temp) size++;
	return temp;
	
}

void Bank::clean(BankNode* node){
	if(node->left){
		clean(node->left);
		delete node->left;
	}
	if(node->right){
		clean(node->right);
		delete node->right;
	}
}


int Bank::recursiveAdd(graph* g, BankNode* node){
	for(int i=0; i<VERTICES; i++){
		
		if(g[i]<node->g[i]){
			//we consider g to be "less than" node->g in this BST
			if(!node->left){
				node->left = new BankNode();
				memcpy(node->left->g, g, sizeof(graph)*VERTICES);
				return 1;
				
			}else{
				return recursiveAdd(g, node->left);
				
			}

		}
		
		else if(g[i]>node->g[i]){
			//we consider g to be "greater than" node->g in this BST
			if(!node->right){
				node->right = new BankNode();
				memcpy(node->right->g, g, sizeof(graph)*VERTICES);
				return 1;
			}else{
				return recursiveAdd(g,  node->right);
			}
		}
		
	}
	//the for loop terminates only if g has the same adj matrix as node->g, hence the tree already contains g
	return 0;
	
}

