#include "atoc.h"
#include "ops.h"


word reg[4];
word ir = 0x0;
word cond = 0x0;

word mem[256];

void (*op_m[4])(word op) = {mov, alu, cld, cda};


void run(void (*disp)()) {
    cond = 0x80;

    while (cond & 0x80) {
        if (cond & 0x40)
            op_m[ir>>6](ir);
        else
            fetch();

        disp();

        cond ^= 0x40;
    }
}
