#include "array.h"

#include "stdint.h"
#include "stdlib.h"


Array makeArray  (int cap){
	Array ret;
	ret.arr  = malloc(sizeof(int32_t) * cap);
	ret.cap  = cap;
	ret.size = 0;
	ret.bloom[0] = 0;
	ret.bloom[1] = 0;
	return ret;
}


void  appendArray(Array* a, int32_t x){
	if(a->size + 1 >= a->cap){
		int32_t* tmp = a->arr;
		a->arr = malloc(sizeof(int32_t) * a->cap * 2);
		a->cap *= 2;
		for(int i = 0; i < a->size; i++) a->arr[i] = tmp[i];
		free(tmp);
	}
	uint64_t h0 = 1l << (  x            % 64);
	uint64_t h1 = 1l << (((x / 64) + x) % 64);
	if(((a->bloom[0] & h0) == 0) | ((a->bloom[1] & h1) == 0)){
		a->bloom[0] |= h0;
		a->bloom[1] |= h1;
		a->arr[a->size] = x;
		a->size++;
		return;
	}
	for(int i = 0; i < a->size; i++)
		if(a->arr[i] == x) return;
	
	a->arr[a->size] = x;
	a->size++;
}


Array unionArrays(Array a, Array b){
	Array ret = makeArray((a.size + b.size) * 2);
	
	// do stuff
	
	return ret;
}


Array interArrays(Array a, Array b){
	int minsize = (a.size > b.size)? b.size : a.size;
	Array ret = makeArray(minsize * 2);
	
	// do stuff
	
	return ret;
}
