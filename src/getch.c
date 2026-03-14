#include <stdio.h>

#define BUFSIZE 100
static char buf[BUFSIZE];
static int pos = 0;

static char history[2] = {0};
static int buffChar(int c) {
    history[1] = history[0];
    history[0] = c;
    return c;
}

int prevch(void) {
    return history[1];
}

int getch(void) {
    return pos > 0 ? buf[--pos] : buffChar(getchar());
    // return pos > 0 ? buf[--pos] : getchar();
}

void ungetch(int c) {
    if(pos < BUFSIZE)
        buf[pos++] = c;
    else
        fprintf(stderr, "error: overflow char buffer\n");
}

