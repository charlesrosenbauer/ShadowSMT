#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "sat.h"




void addAND(Instance* i, int a, int b, int q){
	addClause(i, (Clause){-a, -b,  q});
	addClause(i, (Clause){ a, -q, -q});
	addClause(i, (Clause){ b, -q, -q});
}


void addORR(Instance* i, int a, int b, int q){
	addClause(i, (Clause){ a,  b, -q});
	addClause(i, (Clause){-a,  q,  q});
	addClause(i, (Clause){-b,  q,  q});
}


void addNOR(Instance* i, int a, int b, int q){
	addClause(i, (Clause){ a,  b,  q});
	addClause(i, (Clause){-a, -q, -q});
	addClause(i, (Clause){-b, -q, -q});
}


void addNOT(Instance* i, int a, int q){
	addClause(i, (Clause){ a,  q,  q});
	addClause(i, (Clause){-a, -q, -q});
}


void addXOR(Instance* i, int a, int b, int q){
	addClause(i, (Clause){-a, -b, -q});
	addClause(i, (Clause){ a,  b, -q});
	addClause(i, (Clause){ a, -b,  q});
	addClause(i, (Clause){-a,  b,  q});
}


void addAdder(Instance* i, int a, int b, int cin, int sum, int cex){
	int32_t x = addVars(i, 3)-2;
	int32_t y = x+1;
	int32_t z = x+2;
	
	addAND(i,   a,   b,   x);
	addXOR(i,   a,   b,   y);
	addAND(i,   y, cin,   z);
	addXOR(i,   y, cin, sum);
	addORR(i,   x,   z, cex);
}




void     adder   (Instance* i, int bits, int startA, int startB, int startQ, int cin, int ovf){
	int a  = startA;
	int b  = startB;
	int q  = startQ;
	int c  = cin;
	int o  = addVar(i);
	for(int ix = 0; ix < bits; ix++){
		addAdder(i, a, b, c, q, o);
		a++;
		b++;
		q++;
		c = o;
		o = (ix == bits-1)? ovf : addVar(i);
	}
}
