#ifndef __ARRAY_HEADER__
#define __ARRAY_HEADER__


#include "stdint.h"


typedef struct{
	int32_t* arr;
	uint64_t bloom[2];
	int size, cap;
}Array;


Array makeArray  (int);
void  appendArray(Array*, int32_t);
Array unionArrays(Array, Array);
Array interArrays(Array, Array);





#endif
