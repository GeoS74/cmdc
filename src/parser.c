#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "cmdc.h"

void heading(void);
void tabs(int *blankLines, int *lineStart, int *lastIndent);
void newLine(int *blankLines, int *lineStart);
void paragraph(int *blankLines, int *lineStart);
void printEscapedChar(int c);
void backslash(void);
int bufferInlineSpaces(int c, char *spaceChar, int *pos);
void flushBufferInlineSpaces(char *spaceChar, int *pos);
void thematicBreak(int *blankLines, int *lineStart, int lastIndent);
void codeBlock(int *blankLines, int *lineStart);


void parser(void) {
    /*
    переменная hasContent перенесена в функцию bufferInlineSpaces
    и объявлена как статичная, это может стать проблемой если использовать парсер в потоках.
    Чтобы это исправить надо объявить её в основной функции парсера и
    передавать по ссылке в bufferInlineSpaces.
    В основной функции парсера обнулять её при получении символа '\n'
    int hasContent = 0;
    */

    /*переменные для буферизации пробелов между символами и в конце строки*/
    char spaceChar[100] = {0};
    int pos = 0;

    int c, lineStart, blankLines, lastIndent;
    struct tag *pt;

    lineStart = 0;
    blankLines = 0;
    lastIndent = 0; /*кол-во отступов высчитаное функцией tabs*/

    while((c = getch()) != EOF) {
        ++lineStart;
// printf("debug 1\n");
        if(bufferInlineSpaces(c, spaceChar, &pos))
            continue;
// printf("debug 2\n");
        /*обработка отступов и пустых строк*/
        if(lineStart == 1 && isspace(c)) {
// printf("debug 3\n");
            ungetch(c);
            tabs(&blankLines, &lineStart, &lastIndent);
        }
        /*завершение строки*/
        else if(c == '\n') {
// printf("debug 4\n");
            lastIndent = 0; /*перенос строки сбрасывает отступы*/
            newLine(&blankLines, &lineStart);
        }
        /*заголовки*/
        else if(c == '#' && peek() == NULL) {
// printf("debug 5\n");
            ungetch(c);
            heading();
        }
        /*любой не пробельный символ в начале строки*/
        else if(lineStart == 1) {
// printf("debug 6 c=%c\n", c);
            ungetch(c);

            if(c == '`' || c == '~')
                codeBlock(&blankLines, &lineStart);
            else if(c == '*' || c == '-' || c == '_') { /*тематический разрыв*/
// printf("debug 7\n");
                thematicBreak(&blankLines, &lineStart, lastIndent);
            }
            else {
// printf("debug 8\n");
                paragraph(&blankLines, &lineStart);
            }
                 
        }
        /*экранирование*/
        else if(c == '\\') {
// printf("debug 9\n");
            backslash();
        }
        else {
// printf("debug 10: c=%c\n", c);
            printf("%c", c);
        }
    }

    /*финальное закрытие всех блоков*/
    if(peek() != NULL) {
        while((pt = pop())) {
            if(pt->type == CODE_BLOCK) {
                flushBufferInlineSpaces(spaceChar, &pos);
                /*если не было переноса строки перед закрытием блока вывести его*/
                if(blankLines == 0)
                    printf("\n");
            }
            pt->close(pt);
        }
    }
}

void codeBlock(int *blankLines, int *lineStart) {
    struct tag *pt;
    int c, initChar, count, isBreaks;
}

/*
использует буфер
*/
void thematicBreak(int *blankLines, int *lineStart, int lastIndent) {
    struct tag *pt;
    int c, initChar, count, isBreaks;

    char buf[100] = {0};
    int pos = 0;

    initChar = getch();
    buf[pos++] = initChar;
    count = 1;

    while((c = getch()) == initChar || c == ' ') {
        if(c == initChar) 
            ++count;

        if(pos < 100)
            buf[pos++] = c;
        else
            fprintf(stderr, "error: many indent symbols\n");
    }

    isBreaks = (count >= 3 && lastIndent < TAB_STEP && (c == '\n' || c == EOF));

    /*вернуть все считанные символы обратно в поток*/
    ungetch(c);
    if(!isBreaks)
        while(pos > 0)
            ungetch(buf[--pos]);

    if((pt = peek()) != NULL) {
        if(pt->type == CODE_BLOCK && pt->kind == INDENTED) {
            pt = pop();
            pt->close(pt);
            *blankLines = 0;
            *lineStart = 0;
        }
        else if(pt->type == PARAGRAPH) {
            if(*blankLines > 0 || isBreaks) {
                pt = pop();
                pt->close(pt);
                *blankLines = 0;
                *lineStart = 0;
            }
        }
        printf("\n");

        if(isBreaks) {
            printf("<hr />");
            if(c == '\n') {
            //     *blankLines = 0;
            //     *lineStart = 0;
                printf("\n");
            }
        }
             
    }
    else {
        if(isBreaks) {
            printf("<hr />");
            // if(c == '\n') {
            //     *blankLines = 0;
            //     *lineStart = 0;
            //     printf("\n");
            // }
        }
        else {
            push(getTag(PARAGRAPH));
            printf("<p>");
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

/*
для параграфа \n не печатается
*/
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
            for(; *blankLines > 0; --*blankLines)
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

void tabs(int *blankLines, int *lineStart, int *lastIndent) {
    struct tag *pt;
    int indent, c;

    char spaceChar[100] = {0};
    int pos = 0;

    indent = 0;

    while(isspace(c = getch()) && c != '\n') {
        indent += (c == '\t') ? TAB_STEP : 1;
        if(pos < 100)
            spaceChar[pos++] = c;
        else
            fprintf(stderr, "error: many indent symbols\n");
    }
    *lastIndent = indent;

    /*это пустая строка*/
    if(c == '\n' || c == EOF) {
        ++*blankLines;
        *lineStart = 0;
        return;
    }
    else
        ungetch(c);

    /*внутри какого-то блока*/
    if((pt = peek()) != NULL) {
        if(pt->type == CODE_BLOCK && pt->kind == INDENTED) {
            if(indent < TAB_STEP) {
                pt = pop();
                pt->close(pt);
                printf("\n");
                indent = 0;
                *blankLines = 0;
            }
            else {
                /*вывести пустые строки внутри блока кода*/
                for(; *blankLines > 0; --*blankLines)
                    printf("\n");
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
        if(indent < TAB_STEP)
            indent = 0;
        else {
            struct tag t = getTag(CODE_BLOCK);
            t.kind = INDENTED;
            push(t);
            printf("<pre><code>");
            *blankLines = 0;
        }
    }

    if(indent) {
        for(int i = 0, p = 0; i < pos; ++i) {
            if(p >= TAB_STEP)
                printf("%c", spaceChar[i]);
            else
                p += spaceChar[i] == '\t' ? TAB_STEP : 1;
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

void backslash(void) {
    struct tag *pt;
    int c;

    c = getch();
    if((pt = peek()) != NULL && pt->type == CODE_BLOCK)
        printf("\\%c", c);
    else if(c == '\n') {
        printf("<br />");
        ungetch(c);
    }
    else if(c == EOF)
        printf("\\");
    else
        printEscapedChar(c);
}

void printEscapedChar(int c) {
    switch(c) {
        case '!': printf("!"); break;
        case '#': printf("#"); break;
        case '$': printf("$"); break;
        case '%': printf("%%"); break;
        case '\'': printf("'"); break;
        case '(': printf("("); break;
        case ')': printf(")"); break;
        case '*': printf("*"); break;
        case '+': printf("+"); break;
        case ',': printf(","); break;
        case '-': printf("-"); break;
        case '.': printf("."); break;
        case '/': printf("/"); break;
        case ':': printf(":"); break;
        case ';': printf(";"); break;
        case '=': printf("="); break;
        case '?': printf("?"); break;
        case '@': printf("@"); break;
        case '[': printf("["); break;
        case ']': printf("]"); break;
        case '\\': printf("\\"); break;
        case '^': printf("^"); break;
        case '_': printf("_"); break;
        case '`': printf("`"); break;
        case '{': printf("{"); break;
        case '}': printf("}"); break;
        case '|': printf("|"); break;
        case '~': printf("~"); break;
        case '&': printf("&amp;"); break;
        case '<': printf("&lt;"); break;
        case '>': printf("&gt;"); break;
        case '"': printf("&quot;"); break;
        default: printf("\\%c", c); break;
    }
}

/*
буферезирует пробелы между символами и в коне строки
возврат:
    1 - символ был пробелом и обработан (нужен continue)
    0 - символ требует дальнейшей обработки
*/
int bufferInlineSpaces(int c, char *spaceChar, int *pos) {
    struct tag *pt;
    static int hasContent = 0;

    if(!hasContent)
        hasContent = (c != ' ' && c != '\t');
        
    if(hasContent) {
        if(c == ' ' || c == '\t') {
            spaceChar[(*pos)++] = c;
            return 1;
        }
        else if(c == '\n') {
            if((pt = peek()) != NULL) {
                /*внутри кодового блока оставлять пробелы в конце строк*/
                if(pt->type == CODE_BLOCK) {
                    for(int i = 0; i < *pos; ++i)
                        printf("%c", spaceChar[i]);
                }
                /*внутри параграфа пробелы в конце строки заменить на <br />*/
                else if(pt->type == PARAGRAPH) {
                    if(*pos >= 2)
                        printf("<br />");
                }
            }
            *pos = 0;
            hasContent = 0;
        }
        else {
            for(int i = 0; i < *pos; ++i)
                printf("%c", spaceChar[i]);
            *pos = 0;
        }
    }
    return 0;
}
/*сбрасывает буфер пробелов между символами*/
void flushBufferInlineSpaces(char *spaceChar, int *pos) {
    for(int i = 0; i < *pos; ++i)
        printf("%c", spaceChar[i]);
    *pos = 0;
}
