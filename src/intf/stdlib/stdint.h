#pragma once
#include <stdint-gcc.h>
/*typedef signed char int8;
typedef unsigned char uint8;

typedef signed short int16;
typedef unsigned short uint16;

typedef signed int int32;
typedef unsigned int uint32;

typedef signed long long int int64;
typedef unsigned long long int uint64;*/


typedef uint8_t bool;
#define false 0
#define true 1

#define NULL ((void*)0)

#define min(a,b) ((a) < (b) ? (a) : (b))
#define min(a,b) ((a) > (b) ? (a) : (b))