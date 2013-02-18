#ifndef ATOC_DISPLAY
#define ATOC_DISPLAY

#include "atoc.h"


void (*init_display(int fancy, int delay))(void);
void deinit_display(void);


#endif
