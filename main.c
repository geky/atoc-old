#include "atoc.h"
#include "obj.h"
#include "display.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>


void handle_kill(int err) {
    deinit_display();
    exit(0);
}    

int main(int argc, char **argv) {
    int i, type = 0, delay = 1000;
    void (*display)(void);
    errno = 0;

    for (i=1; i<argc; i++) {
        if (!strcmp(argv[i], "-f")) {
            i++;

            if (strcmp(argv[i],"0"))
                delay = (int)(1000.0 / strtod(argv[i], 0));
            else
                delay = 0;
        } else if (!strcmp(argv[i], "-c")) {
            type = 1;
        } else if (!strcmp(argv[i]+strlen(argv[i])-3, ".ao")) {
            load_file(argv[i]);
        }        
    }

    signal(SIGINT, handle_kill);

    display = init_display(type, delay);
    run(display);

    exit(0);
}
