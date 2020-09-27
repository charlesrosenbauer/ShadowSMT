#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "sat.h"





int main(){
	Instance sat = newInstance(32);
	int buffsize;
	
	buffsize = addVars(&sat, 5);
	addAdder(&sat, buffsize+1, buffsize+2, buffsize+3, buffsize+4, buffsize+5);
	
	buffsize = addVars(&sat, 5);
	addAdder(&sat, buffsize+1, buffsize+2, buffsize+3, buffsize+4, buffsize+5);
	
	printSAT(&sat);
}
