#include <stdio.h>

#define BUFSIZE 100
static char buf[BUFSIZE];
static int pos = 0;

int getch(void) {
    return pos > 0 ? buf[--pos] : getchar();
}

void ungetch(int c) {
    if(pos < BUFSIZE)
        buf[pos++] = c;
    else
        fprintf(stderr, "error: overflow char buffer\n");
}
