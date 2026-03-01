#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "cmdc.h"

void heading(void);
void tabs(int c);

void parser(void) {
    int c, lineStart;
    struct tag *pt;

    lineStart = 0;
    while((c = getch()) != EOF) {
        ++lineStart;

        if(c == '\n' && peek() != NULL) {
            while((pt = peek()) && pt->singleLine == 1) {
                pt->close(pt);
                pop();
            }
            printf("\n");
            lineStart = 0;
            continue;
        }

        if(lineStart == 1 && isspace(c)) { /*первый символ пробел или таб*/
            tabs(c);
            continue;
        }
        else if(c == '#' && peek() == NULL) {
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

void tabs(int c) {
    struct tag *pt;

    int spaces = c == '\t' ? 4 : 1;
    while((c = getch()) != '\n' && isspace(c))
        spaces += (c == '\t') ? 4 : 1;

    if((pt = peek()) != NULL) { /*внутри какого-то блока*/
        
        if(strcmp(pt->type, "codeBlock") == 0) {
            if(spaces >= 4)
                spaces -= 4;
            else {
                pt = pop();
                pt->close(pt);
                push(getTag("paragraph"));
                printf("\n<p>");
                spaces = 0;
            }
        }
        else if(strcmp(pt->type, "paragraph") == 0) {
            spaces = 0;
        }
    }
    else { /*вне блока*/
        if(spaces >= 4) {
            push(getTag("codeBlock"));
            printf("<pre><code>");
        }
        else {
            push(getTag("paragraph"));
            printf("<p>");
        }
        spaces = 0;
    }

    while(spaces-- > 0)
        printf(" ");
    ungetch(c);
}

void heading(void) {
    int c;
    struct tag t;

    t = getTag("heading");
    while((c = getch()) == '#')
        t.level++;

    if(c == ' ' && t.level <= 6) {
        while((c = getch()) == ' ')
            ;
        push(t);
        printf("<h%d>", t.level);
        ungetch(c);
    }
    else if((c == '\n' || c == EOF) && t.level <= 6) {
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