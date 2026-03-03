#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "cmdc.h"

void heading(void);
void tabs(int *blankLines, int *lineStart);
void newLine(int *blankLines, int *lineStart);
void paragraph(int *blankLines, int *lineStart);

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
            ungetch(c);
            heading();
        }
        /*любой не пробельный символ вне блоков*/
        else if(lineStart == 1) {
            ungetch(c);
            paragraph(&blankLines, &lineStart);
        }
        else {
            // if((pt = peek()) != NULL && strcmp(pt->type, "codeBlock") == 0)
            //     while(blankLines-- > 0)
            //         printf("\n");
            printf("%c", c);
        }
    }

    /*финальное закрытие всех блоков*/
    if(peek() != NULL) {
        while((pt = pop())) {
            if(pt->type == CODE_BLOCK) {
                // while(blankLines-- > 0)
                //     printf("\n");
                /*перед закрытием блока должен быть перенос строки */
                printf("\n");
            }
                 
            pt->close(pt);
        }
    }
}

void paragraph(int *blankLines, int *lineStart) {
    struct tag *pt;

    if((pt = peek()) != NULL) {
        if(pt->type == CODE_BLOCK && pt->kind == INDENTED) {
            pt = pop();
            pt->close(pt);
            *blankLines = 0;
            *lineStart = 0;
        }
        else if(pt->type == PARAGRAPH) {
            if(*blankLines > 0) {
                pt = pop();
                pt->close(pt);
                *blankLines = 0;
                *lineStart = 0;
            }
        }
        printf("\n");
    }
    else {
        push(getTag(PARAGRAPH));
        printf("<p>");
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

        if(pt->type == CODE_BLOCK) {
            while((*blankLines)-- > 0)
                printf("\n");
        }
        else if(pt->type == PARAGRAPH) {
            *lineStart = 0;
            *blankLines = 0;
            return;
        }
        // else {
        //     pt->close(pt);
        //     pop();
        // }
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
    if((pt = peek()) != NULL) {
        if(pt->type == CODE_BLOCK && pt->kind == INDENTED) {
            if(indent < 4) {
                pt = pop();
                pt->close(pt);
                printf("\n");
                indent = 0;
                *blankLines = 0;
            }
            /*здесь indent не сбрасывается*/
            /*отступы внутри кодового блока должны быть выведены*/
        }
        else if(pt->type == PARAGRAPH) {
            indent = 0;
        }
    }
    /*вне блока*/
    else {
        if(indent < 4)
            indent = 0;
        else {
            struct tag t = getTag(CODE_BLOCK);
            t.kind = INDENTED;
            push(t);
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
    else
        *lineStart = 0;
}

/*
первый считанный символ гарантировано будет #
может открыть paragraph
*/
void heading(void) {
    int c;
    struct tag t;

    t = getTag(HEADING);
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
        push(getTag(PARAGRAPH));
        printf("<p>");
        while(t.level-- > 0)
            printf("#");
        ungetch(c);
    }
}