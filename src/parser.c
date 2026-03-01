#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "cmdc.h"

void heading(void);
void tabs(int *blankLines);

void parser(void) {
    int c, lineStart, blankLines;
    struct tag *pt;

    lineStart = 0;
    blankLines = 0;
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
            ungetch(c);
            tabs(&blankLines);
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

void tabs(int *blankLines) {
    struct tag *pt;
    int indent, c;

    char spaceChar[100] = {0};
    int pos = 0;

    indent = 0;

    while(isspace(c = getch()) && c != '\n') {
        indent += (c == '\t') ? 4 : 1;
        if(pos < 100)
            spaceChar[pos++] = c;
        else
            fprintf(stderr, "error: many indent symbols\n");
    }

    /*это пустая строка*/
    if(c == '\n') {
        ++blankLines;
        return;
    }
    else
        ungetch(c);

    /*внутри какого-то блока*/
    if((pt = peek()) != NULL) {
        
        if(strcmp(pt->type, "codeBlock") == 0) {
            if(indent < 4) {
                pt = pop();
                pt->close(pt);
                push(getTag("paragraph"));
                printf("\n<p>");
                indent = 0;
            }
            /*здесь indent не сбрасывается*/
        }
        else if(strcmp(pt->type, "paragraph") == 0) {
            indent = 0;
        }
    }
    /*вне блока*/
    else {
        if(indent < 4)
            indent = 0;
        else {
            push(getTag("codeBlock"));
            printf("<pre><code>");
        }
    }

    if(indent) {
        for(int i = 0, p = 0; i < pos; ++i) {
            if(p >= 4)
                printf("%c", spaceChar[i]);
            else
                p += spaceChar[i] == '\t' ? 4 : 1;
        }
    }
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