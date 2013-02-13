#include "atoc.h"

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>


void handle_kill(int err) {
    printf("\033[?25h\n");
    exit(0);
}

void init_display(void) {
    printf("\033[?25l");
}

void display(void) {
    printf("\rc[%2x] ir[%2x] r[%2x %2x %2x %2x]", cond, ir, reg[0], reg[1], reg[2], reg[3]);
    usleep(1000);
}

int main() {
    int i;

    init_display();

    signal(SIGINT, handle_kill);

    srand(time(0));
    for (i=0; i<0x100; i++) mem[i] = rand() % 0x100;

    run(display);
    exit(0);
}
