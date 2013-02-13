#include "atoc.h"
#include "obj.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

int timeval = 1000;


void handle_kill(int err) {
    printf("\033[?25h\n");
    exit(0);
}

void init_display(void) {
    printf("\033[?25l");
}

void simple_display(void) {
    printf("\rc[%2x] ir[%2x] r[%2x %2x %2x %2x]", cond, ir, reg[0], reg[1], reg[2], reg[3]);
    fflush(stdout);
    usleep(timeval);
}

int main(int argc, char **argv) {
    int i;
    errno = 0;

    for (i=1; i<argc; i++) {
        if (!strcmp(argv[i], "-f")) {
            i++;

            if (strcmp(argv[i],"0"))
                timeval = (int)(1000.0 / strtod(argv[i], 0));
            else
                timeval = 0;
        } else if (!strcmp(argv[i]+strlen(argv[i])-3, ".ao")) {
            load_file(argv[i]);
        }        
    }

    init_display();

    signal(SIGINT, handle_kill);

    run(simple_display);
    exit(0);
}
