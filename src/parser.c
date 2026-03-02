#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "cmdc.h"

void heading(void);
void tabs(int *blankLines, int *lineStart);
void newLine(int *blankLines, int *lineStart);

void parser(void) {
    int c, lineStart, blankLines;
    struct tag *pt;

    lineStart = 0;
    blankLines = 0;
    while((c = getch()) != EOF) {
        ++lineStart;

        /*обработка отступов и пустых строк*/
        if(lineStart == 1 && isspace(c)) {
            ungetch(c);
            tabs(&blankLines, &lineStart);
        }
        /*завершение строки*/
        else if(c == '\n') {
            newLine(&blankLines, &lineStart);
        }
        // else if(c == '\n' && peek() != NULL) {
        //     while((pt = peek()) && pt->singleLine == 1) {
        //         pt->close(pt);
        //         pop();
        //     }
        //     printf("\n");
        //     lineStart = 0;
        // }
        /*заголовки*/
        else if(c == '#' && peek() == NULL) {
            heading();
        }
        else {
            if((pt = peek()) != NULL && strcmp(pt->type, "codeBlock") == 0)
                while(blankLines-- > 0)
                    printf("\n");
            printf("%c", c);
        }
    }

    if(peek() != NULL) {
        while((pt = pop())) {
            if(strcmp(pt->type, "codeBlock") == 0) {
                // while(blankLines-- > 0)
                //     printf("\n");
                /*перед закрытием блока должен быть перенос строки */
                printf("\n");
            }
                 
            pt->close(pt);
        }
    }
}

void newLine(int *blankLines, int *lineStart) {
    struct tag *pt;

    if(peek() != NULL) {
        /*закрыть инлайн теги*/
        while((pt = peek()) && pt->singleLine == 1) {
            pt->close(pt);
            pop();
        }
    }        
    /*многострочный тег*/
    if((pt = peek())) {

        if(strcmp(pt->type, "codeBlock") == 0) {
            while((*blankLines)-- > 0)
                printf("\n");
        }
        else {
            pt->close(pt);
            pop();
        }
    }
    printf("\n");
            
    *lineStart = 0;
    *blankLines = 0;
}

void tabs(int *blankLines, int *lineStart) {
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
        ++*blankLines;
        *lineStart = 0;
        return;
    }
    else
        ungetch(c);

    /*внутри какого-то блока*/
    // printf("DEBUG: peek() = %p\n", (void*)peek());

    if((pt = peek()) != NULL) {
        if(strcmp(pt->type, "codeBlock") == 0) {
            if(indent < 4) {
                pt = pop();
                pt->close(pt);
                printf("\n");
                // push(getTag("paragraph"));
                // printf("\n<p>");
                indent = 0;
                *blankLines = 0;
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

    if((c == ' ' || c == '\t') && t.level <= 6) {
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