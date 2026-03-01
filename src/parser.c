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

    char spaceChar[100] = {0};
    int pos = 0;

    int indent = c == '\t' ? 4 : 1;
    spaceChar[pos++] = c;
    while(isspace(c = getch()) && c != '\n') {
        indent += (c == '\t') ? 4 : 1;
        if(pos < 100)
            spaceChar[pos++] = c;
        else
            fprintf(stderr, "error: many indent symbols\n");
    }

    if(c == '\n') {
        /*это пустая строка*/
    }

    /*внутри какого-то блока*/
    if((pt = peek()) != NULL) {
        
        if(strcmp(pt->type, "codeBlock") == 0) {
            if(indent >= 4)
                indent -= 4;
            else {
                pt = pop();
                pt->close(pt);
                push(getTag("paragraph"));
                printf("\n<p>");
                indent = 0;
            }
        }
        else if(strcmp(pt->type, "paragraph") == 0) {
            indent = 0;
        }
    }
    /*вне блока*/
    else {
        if(indent >= 4) {
            push(getTag("codeBlock"));
            printf("<pre><code>");
            indent -= 4;
        }
        else
            indent = 0;
    }

    while(indent-- > 0)
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