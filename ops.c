#include "ops.h"


static inline word ro(word op) {
    return (op & 0xc) >> 2;
}

static inline word rd(word op) {
    return op & 0x3;
}

static inline word check(unsigned int arg) {
    cond &= 0xfc;
    cond |= (arg >> 7) & 0x2;
    cond |= (!arg) & 0x1;

    return (word)(arg & 0xff);
}

static inline word test(word op) {
    word cases[4] = {!(cond&0x1), cond&0x1, cond&0x2, 1};
    return cases[op];
} 


static word and(word a, word b) { return check(a&b); }
static word xor(word a, word b) { return check(a^b); }
static word add(word a, word b) { return check(a+b); }
static word sub(word a, word b) { return check(a-b); }

static word (*alu_m[4])(word, word) = {and, xor, add, sub};


static void movr(word ro, word rd) { reg[ro] = reg[rd]; }
static void ashr(word ro, word rd) { reg[ro] = check(((int8_t)reg[rd]) >> 1); }
static void exch(word ro, word rd) { word temp = reg[ro];
                                     reg[ro] = reg[rd];
                                     reg[rd] = temp; }
static void stor(word ro, word rd) { mem[reg[ro]] = reg[rd]; }

static void (*mov_m[4])(word, word) = {movr, ashr, exch, stor};



void fetch(void) {
    ir = mem[reg[0x3]++];
}

void alu(word op) {
    reg[ro(op)] = alu_m[(op&0x30) >> 4](reg[ro(op)], reg[rd(op)]);
}

void mov(word op) {
    mov_m[(op&0x30) >> 4](ro(op), rd(op));
}

void cld(word op) {
    word state = test((op&0x30) >> 4);
    word addr = reg[ro(op)];
    reg[ro(op)]++;
    if (state) reg[rd(op)] = check(mem[addr]);
}

void cda(word op) {
    reg[ro(op)] = alu_m[(op&0x30) >> 4](reg[ro(op)], test(op&0x3) ? 0xff : 0x00);
}

