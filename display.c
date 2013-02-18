#include "atoc.h"
#include "display.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>

static int delay;

typedef struct {
    const char x; 
    const char y; 
    const char *val;
} fragment; 

typedef struct { 
    const int color; 
    const int size[3];
    const fragment *cycle[3];
} frame;


static const fragment cldf1[15] = {
    {17,21,"--------->"}, 
    {29, 20, "->"}, 
    {20, 13, "-"},
    {22, 13, "-+----+"},
    {28, 14, "|"},
    {28, 15, "|"},
    {28, 16, "+->"},
    {23, 12, "|"},
    {36, 12, "-"},
    {23, 11, "|    +---->"},
    {23, 10, "|    |"},
    {23, 9, "x"},
    {28, 9, "x"},
    {23, 8, "|    |"},
    {23, 7, "v    |"}
};

static const fragment cldf2[26] = {
    {17,21,"--------->"}, 
    {29, 20, "->"}, 
    {22, 13, "-+----+"},
    {28, 14, "|"},
    {28, 15, "|"},
    {28, 16, "+->"},
    {23, 12, "|"},
    {36, 12, "-"},
    {23, 11, "|    +---->"},
    {23, 10, "|    |"},
    {23, 8, "|    |"},
    {23, 7, "v    |"},
    {38, 18, "->|"}, 
    {40, 17, "|"}, 
    {40, 16, "|"}, 
    {40, 15, "|"}, 
    {40, 14, "|"}, 
    {40, 13, "|"}, 
    {40, 12, "|"}, 
    {40, 11, "|"}, 
    {40, 10, "|"}, 
    {1, 9, "\\---------------------x----x-----------/"}, 
    {1, 10, "|"},
    {1, 11, "|"},
    {1, 12, "|"},
    {1, 13, "+-->"}
};

static const fragment cldtf3[8] = {
    {38, 12, "->|"}, 
    {40, 11, "|"}, 
    {40, 10, "|"}, 
    {1, 9, "\\--------------------------------------/"}, 
    {1, 10, "|"},
    {1, 11, "|"},
    {1, 12, "|"},
    {1, 13, "+-->"}
};

static const fragment cldff3[8] = {
    {38, 12, "->|"}, 
    {40, 11, "|"}, 
    {40, 10, "|"}, 
    {1, 9, "\\--------------------------------------/"}, 
    {1, 10, "|"},
    {1, 11, "|"},
    {1, 12, "|"},
    {1, 13, "+-->"}
};

static const fragment fetf3[17] = {
    {38, 12, "->|"}, 
    {40, 11, "|"}, 
    {40, 10, "|"}, 
    {1, 9, "\\--------------------------------------/"}, 
    {1, 10, "|"},
    {1, 11, "|"},
    {1, 12, "|"},
    {1, 13, "|"},
    {1, 14, "|"},
    {1, 15, "|"},
    {1, 16, "|"},
    {1, 17, "|"},
    {1, 18, "|"},
    {1, 19, "|"},
    {1, 20, "|"},
    {1, 21, "|"},
    {1, 22, "+-->"}
};

static const fragment cdaf1[7] = {
    {17,21,"--------->"}, 
    {29, 20, "->"}, 
    {20, 13, "-"},
    {22, 13, "------+"},
    {28, 14, "|"},
    {28, 15, "|"},
    {28, 16, "+->"},
};

static const fragment cdaf2[27] = {
    {17,21,"--------->"}, 
    {29, 20, "->"}, 
    {22, 13, "------+"},
    {28, 14, "|"},
    {28, 15, "|"},
    {28, 16, "+->"},
    {38, 18, "->|"}, 
    {40, 17, "|"}, 
    {40, 16, "|"}, 
    {40, 15, "|"}, 
    {40, 14, "|"}, 
    {40, 13, "|"}, 
    {40, 12, "|"}, 
    {40, 11, "|"}, 
    {40, 10, "|"}, 
    {1, 9, "\\--------------------------------------/"}, 
    {1, 10, "|"},
    {1, 11, "|"},
    {1, 12, "|"},
    {1, 13, "+-->"},
    {1, 14, "|"},
    {1, 15, "|"},
    {1, 16, "|"},
    {1, 17, "|"},
    {1, 18, "|"},
    {1, 19, "|"},
    {1, 20, "+-->"}
};


static const frame fetch_f = {3, {15, 26, 17}, {cldf1, cldf2, fetf3}};
static const frame cldt_f =  {4, {15, 26,  8}, {cldf1, cldf2, cldtf3}};
static const frame cldf_f =  {4, {15, 26,  8}, {cldf1, cldf2, cldff3}};
static const frame cda_f =   {5, { 7, 27,  0}, {cdaf1, cdaf2, 0}};
    /*{1, {0, 0, 0}, {0, 0, 0}},
    {4, {0, 0, 0}, {0, 0, 0}},
    {5, {0, 0, 0}, {0, 0, 0}}
};*/

static const char circuit_base[] =
 "                                  .--.    \n"
 "                                  |  |  | \n"
 "     .----------.                 |SH|  | \n"
 " |   |-----. CU |                 |  |  | \n"
 " |   | IR  |    |                  '.|  | \n"
 " |   |-----. -1 |          |.  |-.      | \n"
 " |   | cz  |    |          ||  |  '-.   | \n"
 " |   '----------'          |'  |     |  | \n"
 " |                              > ALU|  | \n"
 " |   .-------------.           |     |  | \n"
 " |   | Reg   .-----|           |  .-'   | \n"
 " |   |       | Rd  | &         |-'      | \n"
 " |   |-----. .-----|                    | \n"
 " |   | Rn  | | Ro  | &            |.    | \n"
 " |   '-------------'              || &  | \n"
 " |                                |'    | \n"
 " |                                      | \n"
" \\--------------------------------------/ \n"
 "                                          \n"
 "                                          \n"
 "     .----------------------------.       \n"
 "     | Mem          | MAR |  MDR  |       \n"
 "     |              '-----'-------|       \n"
 "     |                            |       \n"
 "     |                            |       \n"
 "     '----------------------------'       \n";



static void put_all(int size, const fragment *frags) {
    int i;
    for (i=0; i < size; i++) {
        printf("\033[%dC\033[%dA%s\033[%dB\033[%dD",
               frags[i].x, frags[i].y, 
               frags[i].val, frags[i].y, 
               frags[i].x + strlen(frags[i].val)
        );
    }
}

static void rm_all(int size, const fragment *frags) {
    int i;
    for (i=0; i < size; i++) {
        int len = strlen(frags[i].val);

        printf("\033[%dC\033[%dA%.*s\033[%dB\033[%dD",
               frags[i].x, frags[i].y, 
               len, circuit_base + (26 - frags[i].y)*43 + frags[i].x,
               frags[i].y, frags[i].x + len
        );
    }
}


const frame *previous = &fetch_f; //TODO
word prev_rd = 0;

void circuit_display(void) {
    const frame *current;
    word ro, rd, cu;


    if (cond & 0x40) {
        ro = rd = 3;
        cu = -1;

        current = &fetch_f;
    } else {
        ro = (ir&0xc) >> 2;
        rd = ir &0x3;

        switch (ir&0xc0) {
            case 0x00: 
            case 0x40:
            case 0x80: 
                switch (ir&0x30) {
                    case 0x00: current = !(cond&0x1) ? &cldt_f : &cldf_f; break;
                    case 0x10: current =  (cond&0x1) ? &cldt_f : &cldf_f; break;
                    case 0x20: current =  (cond&0x2) ? &cldt_f : &cldf_f; break;
                    case 0x30: current = &cldt_f; break;
                }
                break;
            case 0xc0: current = &cda_f; break;
        }
    }

    
    printf("\033[0m"
           "\033[16C\033[13A%d\033[13B\033[17D"
           "\033[16C\033[15A%d\033[15B\033[17D"
           "\033[8C\033[13A%d\033[13B\033[9D"
           "\033[13C\033[21A%02x\033[21B\033[15D",
           ro, rd, prev_rd, cu
    );

    rm_all(previous->size[1], previous->cycle[1]);

    printf("\033[3%dm", previous->color);
    put_all(previous->size[2], previous->cycle[2]);

    printf("\033[3%dm", current->color);
    put_all(current->size[0], current->cycle[0]);


    fflush(stdout);
    usleep(delay);


    printf("\033[0m"
           "\033[8C\033[13A%d\033[13B\033[9D", 
           ro
    );

    rm_all(current->size[0], current->cycle[0]);
    rm_all(previous->size[2], previous->cycle[2]);

    printf("\033[3%dm", current->color);
    put_all(current->size[1], current->cycle[1]);


    fflush(stdout);
    usleep(delay);

    previous = current;
    prev_rd = rd;
}

//////////////////////////////

void simple_display(void) {
    printf("\rc[%2x] ir[%2x] r[%2x %2x %2x %2x]", cond, ir, reg[0], reg[1], reg[2], reg[3]);
    fflush(stdout);
    usleep(delay);
}

//////////////////////////////

void (*init_display(int fancy, int delay_time))(void) {
    delay = delay_time;
    printf("\033[?25l");

    switch (fancy) {
        case 0:
            return simple_display;
        case 1:
            delay /= 2;
            printf("\033[0m");
            printf(circuit_base);
            return circuit_display;
    }

    return 0;
}

void deinit_display(void) {
    printf("\033[?25h\033[0m\n");
}    
