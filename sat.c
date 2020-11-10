#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "sat.h"
#include "array.h"







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
	uint64_t x  = a.bits[0];
			 x &= a.bits[1];
			 x &= a.bits[2];
			 x &= a.bits[3];
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


B256 intersect(B256* xs, int ct){
	B256 ret;
	for(int i = 0; i <  4; i++) ret.bits[i] = -1;
	
	for(int i = 0; i < ct; i++)
		for(int j = 0; j < 4; j++) ret.bits[j] &= xs[i].bits[j];
	
	return ret;
}


int isNull(B256 x){
	return 0 != (x.bits[0] | x.bits[1] | x.bits[2] | x.bits[3]);
}

int nextSolution(B256 x, uint8_t min){
	int sols = pct256(x);
	if(sols == 0) return -1;
	if(sols == 1) return min;
	for(int i = 0; i < 5; i++){
		int ix = ((min / 64) + i) % 4;
		uint64_t mask = (i == 0)? (1l << (min % 64))-1 : 0xffffffffffffffff;
		if(x.bits[ix] & mask) return __builtin_ctzl(x.bits[ix] & mask);
	}
	return min;
}

int firstSolution(B256 x){
	if(x.bits[0] != 0) return __builtin_ctzl(x.bits[0]);
	if(x.bits[1] != 0) return __builtin_ctzl(x.bits[1]);
	if(x.bits[2] != 0) return __builtin_ctzl(x.bits[2]);
	if(x.bits[3] != 0) return __builtin_ctzl(x.bits[3]);
	
	return -1;
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
	
	ret.localVars = malloc(sizeof(Set32) * i->varct);
	for(int ix = 0; ix < i->varct; ix++){
		ret.localVars[ix] = initSet32(32);
		for(int j = 0; j < ret.clausects[ix]; j++){
			Clause cl = i->clauses[ret.clauseref[ix][j]];
			if(cl.a != 0) insertSet32(&ret.localVars[ix], cl.a);
			if(cl.b != 0) insertSet32(&ret.localVars[ix], cl.b);
			if(cl.c != 0) insertSet32(&ret.localVars[ix], cl.c);
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
		printSet32(c->localVars[i]);
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



B256 makeConstraint(Clause c, FilterMetadata fm){
	uint16_t mask = 0;
	
	for(int v = 0; v < 3; v++){
		int x, sign;
		if(v == 0){
			// Set A
			x    = (c.a < 0)? -c.a : c.a;
			sign = c.a < 0;
		}else if(v == 1){
			// Set B
			x    = (c.b < 0)? -c.b : c.b;
			sign = c.b < 0;
		}else{
			// Set C
			x    = (c.c < 0)? -c.c : c.c;
			sign = c.c < 0;
		}
		if(x != 0){
			for(int i = 0; i < 8; i++){
				int ix = (i+x+1)%8;
				if(x == fm.vals[ix]){
					mask |= (1 << (ix + (8 * sign)));
					break;
				}
			}
		}
	}
	
	return constrain(mask & 0xff, mask >> 8);
}






int solve(Instance* inst, uint64_t* bits){

	if(inst->varct <= 8){
		FilterMetadata fm;
		B256* consts = malloc(sizeof(B256) * inst->clausect);
		
		for(int i = 0; i < 8; i++) fm.vals[i] = i+1;
		for(int i = 0; i < inst->clausect; i++) consts[i] = makeConstraint(inst->clauses[i], fm);
		
		B256 sat = intersect(consts, inst->clausect);
		free(consts);
		
		return firstSolution(sat);
	}
	
	return 4701;
}



