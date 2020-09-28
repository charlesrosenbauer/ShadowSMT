#ifndef __SAT_HEADER__
#define __SAT_HEADER__


#include "stdint.h"


/*
	Bit Vector Manipulation.
	
	The idea here is that at small scales, brute force can often outperform
	smarter approaches thanks to low constant factors associated with 
	simplicity.
	
	Additionally, as software can be thought of as walking a tree (that's why
	we use call stacks), and because the vast majority of nodes on a tree are
	close to the leaves, it should follow that using a sufficiently fast brute-
	force strategy on small scales should be very beneficial to certain
	problems.
	
	In the particular case of SAT problems, the problem is often phrased as one
	of searching a tree, and so this analogy is even more fitting. Add to this
	that SAT can be formulated as just a very large set intersection problem,
	and the ability to run through hundreds of iterations of a brute force
	algorithm using just a couple simple vectorized bitwise operations is a
	very promising option available for this task.
*/
typedef struct{
	uint64_t bits[4];
}B256;


B256 constrain  (uint8_t,   uint8_t);
B256 and256		(B256, 		B256);
B256 orr256		(B256, 		B256);
B256 xor256		(B256, 		B256);
int  pct256		(B256);

void printB256  (B256);



/*
	Clauses and Instances
*/

typedef struct{
	int32_t a, b, c;
}Clause;

typedef struct{
	Clause*  clauses;
	int      clausect, clausecap, varct;
}Instance;

typedef struct{
	int** clauseref;
	int*  clausects;
	int   varct, clausect;
	
	uint64_t* satClauses;
}Connectome;


Instance newInstance   (int);
void     resizeInstance(Instance*, int);
void     addClause     (Instance*, Clause);
int32_t  addVar        (Instance*);
int32_t  addVars       (Instance*, int);
void     printSAT      (Instance*);

Connectome buildConnectome(Instance*);
void       printConnectome(Connectome*);

/*
	Circuit construction
*/
void     addAND  (Instance*, int, int, int);
void     addANDN (Instance*, int, int, int);
void     addORR  (Instance*, int, int, int);
void     addNOR  (Instance*, int, int, int);
void     addNOT  (Instance*, int, int);
void     addXOR  (Instance*, int, int, int);
void     addAdder(Instance*, int, int, int, int, int);

void     adder   (Instance*, int, int, int, int, int, int);
void     mux     (Instance*, int, int, int, int, int);



/*

*/



#endif
