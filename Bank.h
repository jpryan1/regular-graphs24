
#ifndef  _BANK_H_    /* only process this file once */
#define  _BANK_H_

#define MAXN WORDSIZE
#define ONE_WORD_SETS 1
#include "nauty.h"

#define VERTICES 10

struct BankNode{
	graph g[VERTICES];
	BankNode* left;
	BankNode* right;
};


class Bank{
public:
	Bank(){
		root = NULL;
		size=0;
	}
	
	~Bank(){
		//This should traverse and delete all the configurations. To be honest,
		// we don't ever actually delete the bank though, so this doesn't need
		// to be implemented
		if(root){
			clean(root);
			delete root;
		}
	}
	
	int add(graph* g);//RETURNS 1 IF ADDED, 0 IF ALREADY IN BANK
	int size;
	void clean(BankNode* node);
private:

	BankNode* root;
	int recursiveAdd(graph* g, BankNode* node);
};



#endif
