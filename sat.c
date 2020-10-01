#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "sat.h"







B256 constrain(uint8_t pos, uint8_t neg){
	static const uint64_t ks[6] =  {0xAAAAAAAAAAAAAAAA,
									0x6666666666666666,
									0xf0f0f0f0f0f0f0f0,
									0xff00ff00ff00ff00,
									0xffff0000ffff0000,
									0xffffffff00000000};

	B256 ret;

	uint64_t p = 0;
	p |= (pos &  1)?  ks[0] : 0;
	p |= (pos &  2)?  ks[1] : 0;
	p |= (pos &  4)?  ks[2] : 0;
	p |= (pos &  8)?  ks[3] : 0;
	p |= (pos & 16)?  ks[4] : 0;
	p |= (pos & 32)?  ks[5] : 0;

	uint64_t n = 0;
	n |= (neg &  1)? ~ks[0] : 0;
	n |= (neg &  2)? ~ks[1] : 0;
	n |= (neg &  4)? ~ks[2] : 0;
	n |= (neg &  8)? ~ks[3] : 0;
	n |= (neg & 16)? ~ks[4] : 0;
	n |= (neg & 32)? ~ks[5] : 0;

	uint64_t bits = p | n;
	ret.bits[0] = ((neg & 64) | (neg & 128))? 0xffffffffffffffff : bits;
	ret.bits[1] = ((pos & 64) | (neg & 128))? 0xffffffffffffffff : bits;
	ret.bits[2] = ((neg & 64) | (pos & 128))? 0xffffffffffffffff : bits;
	ret.bits[3] = ((pos & 64) | (pos & 128))? 0xffffffffffffffff : bits;

	return ret;
}


int void256(B256 a){
	uint64_t x  = b.bits[0];
			 x &= b.bits[1];
			 x &= b.bits[2];
			 x &= b.bits[3];
	return x != 0;
}



B256 and256(B256 a, B256 b){
	B256 ret;
	for(int i = 0; i < 4; i++)
		ret.bits[i] = a.bits[i] & b.bits[i];
	return ret;
}


B256 orr256(B256 a, B256 b){
	B256 ret;
	for(int i = 0; i < 4; i++)
		ret.bits[i] = a.bits[i] | b.bits[i];
	return ret;
}


B256 xor256(B256 a, B256 b){
	B256 ret;
	for(int i = 0; i < 4; i++)
		ret.bits[i] = a.bits[i] ^ b.bits[i];
	return ret;
}


int pct256(B256 a){
 	int sum = 0;
	for(int i = 0; i < 4; i++)
		sum += __builtin_popcountl(a.bits[i]);
	return sum;
}


B256 intersect(B256* ret, int ct){
	B256 ret;
	for(int i = 0; i <  4; i++) ret.bits[i] = -1;
	
	for(int i = 0; i < ct; i++)
		for(int j = 0; j < 4; j++) ret.bits[j] &= ret[i].bits[j];
	
	return ret;
}


void printB256(B256 x){
	printf("X: %lx %lx %lx %lx\n\n", x.bits[0], x.bits[1], x.bits[2], x.bits[3]);
}




Instance newInstance(int cap){
	Instance ret;
	ret.clauses   = malloc(sizeof(Clause) * cap);
	ret.clausecap = cap;
	ret.clausect  = 0;
	ret.varct     = 0;
	return ret;
}


void resizeInstance(Instance* i, int cap){
	if(cap <= i->clausecap) return;
	Clause* cs   = i->clauses;
	i->clauses   = malloc(sizeof(Clause) * cap);
	i->clausecap = cap;
	for(int ix = 0; ix < i->clausect; ix++) i->clauses[ix] = cs[ix];
	free(cs);
}


void addClause(Instance* i, Clause c){
	if(i->clausect+1 >= i->clausecap) resizeInstance(i, i->clausecap * 2);
	i->clauses[i->clausect] = c;
	i->clausect++;
	int32_t maxvar = 0;
	maxvar = (abs(c.a) > abs(c.b))? abs(c.a) : abs(c.b);
	maxvar = (abs(c.b) > abs(c.c))? abs(c.b) : abs(c.c);
	i->varct = (i->varct > maxvar)? i->varct : maxvar;
}


int32_t addVar(Instance* i){
	i->varct++;
	return i->varct;
}


int32_t addVars(Instance* i, int ct){
	i->varct  += ct;
	return i->varct;
}


void printSAT(Instance* i){
	printf("SAT : %i / %i\n", i->clausect, i->varct);
	for(int ix = 0; ix < i->clausect; ix++){
		Clause c = i->clauses[ix];
		printf("  %i %i %i\n", c.a, c.b, c.c);
	}
}



Connectome buildConnectome(Instance* i){
	Connectome ret;
	ret.varct    = i->varct;
	ret.clausect = i->clausect;
	
	ret.clausects = malloc(sizeof(int) * i->varct);
	for(int ix = 0; ix < i->varct;    ix++) ret.clausects[ix] = 0;
	for(int ix = 0; ix < i->clausect; ix++){
		Clause cl = i->clauses[ix];
		int32_t a = abs(cl.a), b = abs(cl.b), c = abs(cl.c);
		
		if(a != 0) ret.clausects[a-1]++;
		if(b != 0) ret.clausects[b-1]++;
		if(c != 0) ret.clausects[c-1]++;
	}
	
	int* refFill  = malloc(sizeof(int ) * ret.varct);
	ret.clauseref = malloc(sizeof(int*) * ret.varct);
	for(int ix = 0; ix < i->varct;    ix++) refFill[ix] = 0;
	for(int ix = 0; ix < i->varct;    ix++) ret.clauseref[ix] = malloc(sizeof(int) * ret.clausects[ix]);
	for(int ix = 0; ix < i->clausect; ix++){
		Clause cl = i->clauses[ix];
		int32_t a = abs(cl.a), b = abs(cl.b), c = abs(cl.c);
		
		if(a != 0){
			ret.clauseref[a-1][refFill[a-1]] = ix;
			refFill[a-1]++;
		}
		
		if(b != 0){
			ret.clauseref[b-1][refFill[b-1]] = ix;
			refFill[b-1]++;
		}
		
		if(c != 0){
			ret.clauseref[c-1][refFill[c-1]] = ix;
			refFill[c-1]++;
		}
	}
	
	free(refFill);
	return ret;
}


void printConnectome(Connectome* c){
	printf("Connectome:\n");
	for(int i = 0; i < c->varct; i++){
		printf("%i (%i) : ", i+1, c->clausects[i]);
		for(int j = 0; j < c->clausects[i]; j++) printf(" [%i]", c->clauseref[i][j]);
		printf("\n");
	}
}


SolverState newSolverState(Instance* i){
	SolverState ret;
	ret.varct      = i->varct;
	ret.clausect   = i->clausect;
	int varsize    = (ret.varct    / 64) + ((ret.varct    % 64) != 0);
	int clssize    = (ret.clausect / 64) + ((ret.clausect % 64) != 0);
	ret.knownVars  = malloc(sizeof(uint64_t) * varsize);
	ret.stateVars  = malloc(sizeof(uint64_t) * varsize);
	ret.satClauses = malloc(sizeof(uint64_t) * clssize);
	for(int ix = 0; ix < varsize; ix++){
		ret.knownVars[ix] = 0;
		ret.stateVars[ix] = 0;
	}
	for(int ix = 0; ix < clssize; ix++) ret.satClauses[ix] = 0;
	
	return ret;
}


void constantPropagation(Connectome* c, SolverState* s){
	for(int i = 0; i < c->clausect; i++){
	
	}
}










int solve(Instance* i, uint64_t* bits, int size){
	int ix = 0;
	int words = (size / 64) + 1;
	uint64_t* progress = malloc(sizeof(uint64_t) * words);
	for(int i = 0; i < words; i++) progress[i] = 0;
	
	int cont = 1;
	while(cont){
		// Recursively search the SAT Tree
	}
	
	free(progress);
	return 1;
}



