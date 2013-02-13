#include "atoc.h"

#include <string.h>
#include <stdio.h>

// returns less than zero on failure
int load_file(char *filename) {
    word next[2];
    FILE *file = fopen(filename, "rb");

    if (!file)
        return -1;

    if (fread(next, 1, 1, file) != 1) 
        return -2;

    if (next[0] != 'a')
        return -2;


    while (1) {
        if (fread(next, 1, 2, file) != 2) {
            if (ferror(file)) 
                return -2;
            else
                return 0;
        }

        if (next[0]+next[1] < next[0])
            return -3;

        if (fread(&mem[next[0]], 1, next[1], file) != next[1])
            return -2;
    }
}   
