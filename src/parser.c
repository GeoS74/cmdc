#include <stdio.h>
#include <ctype.h>
#include "cmdc.h"

void heading(void);

void parser(void) {
    int c;
    struct tag *pt;

    while((c = getch()) != EOF) {

        if(c == '\n' && peek() != NULL) {
            while((pt = pop()) && pt->singleLine == 1)
                pt->close(pt);
            printf("\n");
            continue;
        }

        if(c == '#' && peek() == NULL) {
            heading();
            continue;
        }
        printf("%c", c);
    }

    if(peek() != NULL) {
        while((pt = pop()))
            pt->close(pt);
    }
}

void heading(void) {
    int c;
    struct tag t;

    t = getTag("heading");
    while((c = getch()) == '#')
        t.level++;

    if(isspace(c) && t.level <= 6) {
        while((c = getch()) == ' ')
            ;
        push(t);
        printf("<h%d>", t.level);
        ungetch(c);
    }
    else {
        push(getTag("paragraph"));
        printf("<p>");
        while(t.level-- > 0)
            printf("#");
        ungetch(c);
    }
}