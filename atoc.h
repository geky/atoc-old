#ifndef ATOC
#define ATOC

#include <stdlib.h>
#include <stdint.h>


typedef uint8_t word;

extern word reg[4];
extern word ir;
extern word cond;

extern word mem[256];


void run(void (*)());

#endif
