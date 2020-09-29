#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "sat.h"





int main(){
	Instance sat = newInstance(32);
	
	int32_t as  = addVars(&sat, 32);
	int32_t bs  = addVars(&sat, 32);
	int32_t qs  = addVars(&sat, 32);
	int32_t cry = addVar (&sat);
	int32_t ovf = addVar (&sat);
	
	adder(&sat, 32, as-31, bs-31, qs-31, cry, ovf);
	//printSAT(&sat);
	
	Connectome  c = buildConnectome(&sat);
	printConnectome(&c);
	
	SolverState s = newSolverState (&sat);
}
