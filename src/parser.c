#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "cmdc.h"

void heading(void);
void headingTrailingHashes(char *spaceChar, int *pos);

void tabs(int *blankLines, int *lineStart, int *lastIndent);
void newLine(int *blankLines, int *lineStart);
void paragraph(int *blankLines, int *lineStart);
void printEscapedChar(int c);
void backslash(void);
int bufferInlineSpaces(int c, char *spaceChar, int *pos);
void flushBufferInlineSpaces(char *spaceChar, int *pos);
void thematicBreak(int *blankLines, int *lineStart, int lastIndent);
void popAndClose(int *blankLines, int *lineStart);

int isCodeBlockInline(void);
int isCodeBlockFenced(void);
int isCloseCodeBlockFenced(void);
void codeBlockInline(void);
void codeBlockFenced(int *blankLines, int *lineStart, int lastIndent);
void openCodeBlockFenced(int *blankLines, int *lineStart);
void printBackTicks(void);

void blockquote(int *blankLines, int *lineStart);

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
    lastIndent = 0; /*кол-во отступов высчитаное функцией tabs, используется в thematicBreak*/

    while((c = getch()) != EOF) {
        ++lineStart;

        if(bufferInlineSpaces(c, spaceChar, &pos))
            continue;

        /*обработка отступов и пустых строк*/
        if(lineStart == 1 && isspace(c)) {
            ungetch(c);
            tabs(&blankLines, &lineStart, &lastIndent);
        }
        /*завершение строки*/
        else if(c == '\n') {
            lastIndent = 0; /*перенос строки сбрасывает отступы*/
            newLine(&blankLines, &lineStart);
        }
        /*заголовки*/
        else if(c == '#' && peek() == NULL) {
            ungetch(c);
            heading();
        }
        else if(c == '#' && lineStart == 1 && peek() != NULL && peek()->type == PARAGRAPH) {
            ungetch(c);

            if(lastIndent < 4)
                popAndClose(&blankLines, &lineStart);

            printf("\n");
        }
        else if(c == '#' && lineStart == 1 && peek() != NULL && peek()->type == BLOCKQUOTE) {
            ungetch(c);
            heading();
        }
        /*любой не пробельный символ в начале строки*/
        else if(lineStart == 1) {
            ungetch(c);

            if(c == '`' || c == '~')
                if(isCodeBlockFenced())
                    codeBlockFenced(&blankLines, &lineStart, lastIndent);
                else
                    paragraph(&blankLines, &lineStart);

            else if(c == '*' || c == '-' || c == '_') /*тематический разрыв*/
                thematicBreak(&blankLines, &lineStart, lastIndent);
            else if(c == '>') /*цитата*/
                blockquote(&blankLines, &lineStart);
            else
                paragraph(&blankLines, &lineStart);
        }
        /*экранирование*/
        else if(c == '\\') {
            backslash();
        }
        /*инлайн блок кода*/
        else if(c == '`' && peek() != NULL && peek()->type == PARAGRAPH) {
            ungetch(c);

            if(isCodeBlockInline())
                codeBlockInline();
            else 
                printBackTicks();
        }
        else {
            printHTMLEntities(c);
            // printf("%c", c);
        }
    }

    /*финальное закрытие всех блоков*/
    if(peek() != NULL) {
        while((pt = pop())) {
            if(pt->type == CODE_BLOCK && pt->kind == INDENTED) {
                flushBufferInlineSpaces(spaceChar, &pos);
                /*если не было переноса строки перед закрытием блока вывести его*/
                if(blankLines == 0)
                    printf("\n");
            }
            else if(pt->type == BLOCKQUOTE) {
                printf("\n");
            }
            pt->close(pt);
        }
    }
}


void blockquote(int *blankLines, int *lineStart) {
    struct tag t, *pt, *blockquote;
    int c, level, indent;

    char spaceChar[100];
    int pos = 0;

    level = 0;
    indent = 0;
    while((c = getch()) == '>' || c == ' ' || c == '\t') {
        if(c == '>') {
            ++level;
            indent = 0;
            pos = 0;
        }
        else {
            indent += (c == '\t') ? TAB_STEP : 1;
            if(pos < 100)
                spaceChar[pos++] = c;
            else
                fprintf(stderr, "error: many indent symbols\n");
        }
    }
    ungetch(c);
    if(indent > 4) {
        /*возврат символов в поток, кроме первого
        если символ таб, то такая ситуация явно не описывается в спецификации.
        Из экспериментов с примерами стало ясно, что 
        в блоке цитат формируется кодовый блок с отступами в случае
        если идет последовательность:
        > таб пробел пробел пробел
        это странно*/
        while(pos > 1)
            ungetch(spaceChar[--pos]);
    }

    blockquote = findByBlockType(BLOCKQUOTE);
    if(blockquote) {
        if(blockquote->level < level) {
            int i = level - blockquote->level;

            if(i > 0 && peek()) {
                popAndClose(blankLines, lineStart);
                printf("\n");
            }

            while(i-- > 0) {
                t = getTag(BLOCKQUOTE);
                t.level = level - i;
                push(t);
                printf("<blockquote>");
                printf("\n");
            }
        }
    }
    else {
        if((pt = peek()) != NULL) {
            popAndClose(blankLines, lineStart);
            printf("\n");
        }

        int i = level;
        while(i-- > 0) {
            t = getTag(BLOCKQUOTE);
            t.level = level - i;
            push(t);
            printf("<blockquote>");
            printf("\n");
        }
     }

    *blankLines = 0;
    *lineStart = 0;
}


/*
Ищет пару для открывающей последовательности inline кодового блока.
Прерывает поиск если:
    - Пустая строка
    - Начало fenced-блока
    - Конец заголовка (перевод строки)
    - EOF
*/
int isCodeBlockInline(void) {
    struct tag t;
    int c, initChar, s, match;

    char buf[500];
    int pos = 0;

    initChar = getch();
    buf[pos++] = initChar;

    /*создать открывающий тег*/
    t = getTag(CODE_INLINE);
    t.level = 1;

    while((c = getch()) == initChar) {
        if(pos < 500) {
            buf[pos++] = c;
            t.level++;
        }
        else fprintf(stderr, "error: many indent symbols\n");
    }
    ungetch(c);

    s = 1, match = 0;
    while((c = getch()) != EOF) {
        if(pos < 500)
            buf[pos++] = c;
        else fprintf(stderr, "error: many indent symbols\n");

        /*поиск пары*/
        if(c == initChar) {
            int l = 1;
            while((c = getch()) == initChar) {
                if(pos < 500)
                    buf[pos++] = c;
                else fprintf(stderr, "error: many indent symbols\n");
                ++l;
            }
            if(l == t.level) {
                match = 1;
                break;
            }
            else {
                ungetch(c);
                ++s; /*увеличить счётчик, т.к. была проверка на закрывающий тег*/
                continue;
            }
        }

        if (c == '\n') {
            /*инлайн код в заголовке, читать до первого переноса строки*/
            if(peek()->type == HEADING) {
                match = 0;
                break;
            }
            /*пустая строка, конец inline блока*/
            else if(!s) {
                match = 0;
                break;
            }
            s = 0;
        }
        else {
            if(!s && (c == ' ' || c == '\t')) {
                continue;
            }
            /*в начале могут быть отступы И символ открытия кодового блока*/
            /*возможно это начало fenced блока кода*/
            else if(!s && (c == '`' || c == '~')) {
                ungetch(c); // вернут в поток перед isCodeBlockFenced
                if(isCodeBlockFenced()) {
                    /*это fenced блок, конец inline блока*/
                    match = 0;
                    break;
                }
                getch(); // вывести символ из потока
            }
            ++s;
        }
    }
    ungetch(c);

    /*возврат символов в поток*/
    while(pos > 0)
        ungetch(buf[--pos]);

    return match;
}

/*применять эту функцию только в паре с isCodeBlockInline*/
void codeBlockInline(void) {
    struct tag t;
    int c, initChar, emptyString;

    /*создать открывающий тег*/
    t = getTag(CODE_INLINE);
    t.level = 1;
    initChar = getch();

    while((c = getch()) == initChar)
        t.level++;
    ungetch(c);

    /*записать все символы в буфер*/
    char buf[500];
    int pos = 0;

    emptyString = 1; /*флаг пустой строки*/
    while((c = getch()) != EOF) {
        if(pos < 500) 
            buf[pos++] = c;
        else fprintf(stderr, "error: many indent symbols\n");

        /*поиск пары*/
        if(c == initChar) {
            int l = 1;
            while((c = getch()) == initChar) {
                ++l;
                if(pos < 500) 
                    buf[pos++] = c;
                else fprintf(stderr, "error: many indent symbols\n");
            }
            ungetch(c);
                 
            if(l == t.level) {
                pos -= t.level;
                break; /*должен обязательно сработать*/
            }

            emptyString = 0;
        }

        if(!isspace(c))
            emptyString = 0;
    }

    /*поиск пробелов по краям*/
    if(!emptyString && (buf[0] == ' ' || buf[0] == '\n') && (buf[pos - 1] == ' ' || buf[pos - 1] == '\n')) {
        // сдвигаем начало на 1 символ
        for(int i = 0; i < pos - 1; i++)
            buf[i] = buf[i + 1];
        pos -= 2;  // убираем два пробела
    }

    /*печать блока*/
    printf("<code>");
    int nl = 0;
    for(int i = 0; i < pos; ++i) {
        if(buf[i] == '\n') {
            ++nl;
            continue;
        }
        
        if(nl && isspace(buf[i]))
            continue;

        /*если был символ переноса и печатаем символ не пробел, то вывести пробел*/
        if(nl && !isspace(buf[i]))
            printf(" ");
        
        nl = 0;
        printHTMLEntities(buf[i]);
             
    }
    printf("</code>");
}

void printBackTicks(void) {
    int c;
    while((c = getch()) == '`')
        printf("%c", c);
    ungetch(c);
}

/*
проверяет строку на начало fenced блока
первый символ или ` или ~
возвращает 0 или 1
все считанные символы возвращает в поток
*/
int isCodeBlockFenced(void) {
    int c, initChar;

    char buf[500];
    int pos = 0;

    initChar = getch();
    buf[pos++] = initChar;

    while((c = getch()) == initChar) {
        if(pos < 500)
            buf[pos++] = c;
        else fprintf(stderr, "error: many indent symbols\n");
    }
    ungetch(c);

    if(pos < 3) {
        /*возврат символов в поток*/
        while(pos > 0)
            ungetch(buf[--pos]);
        return 0;
    }
    
    /*если блок кода открывается символом (`), проверить оставшиеся символы на наличие (`)*/
    /*прервать если (`) есть, т.к. возможно это инлайн блок кода*/
    if(initChar == '`') {
        while((c = getch()) != '\n' && c != EOF) {
            if(c == initChar)
                break;

            if(pos < 500)
                buf[pos++] = c;
            else fprintf(stderr, "error: many indent symbols\n");
        }
        ungetch(c);
    }

    /*возврат символов в поток*/
    while(pos > 0)
        ungetch(buf[--pos]);

    return c == initChar ? 0 : 1;
}

void codeBlockFenced(int *blankLines, int *lineStart, int lastIndent){
    openCodeBlockFenced(blankLines, lineStart);

    int c, indent, hasContent;
    indent = 0;
    hasContent = 0;

    while((c = getch()) != EOF) {
        if(c == ' ' && !hasContent) {
            ++indent;
            continue;
        }

        if(c == '\n') {
            while(indent-- > lastIndent)
                printf(" ");

            printf("\n");
            hasContent = 0;
            indent = 0;
            continue;
        }

        if(indent < 4) {
            if((c == '`' && peek()->kind == FENCED_BACKTICK) || 
               (c == '~' && peek()->kind == FENCED_TILDE)) {

                ungetch(c);
                if(isCloseCodeBlockFenced()) {
                    popAndClose(blankLines, lineStart);

                    if((c = getch()) == '\n')
                        printf("\n");
                    else {
                        hasContent = 0; // не выводить финальный символ переноса (см. ниже)
                        ungetch(c);
                    }
                        
                    break;
                }
                while(indent-- > lastIndent)
                    printf(" ");

                while((c = getch()) == '`' || c == '~')
                    printf("%c", c);
                ungetch(c);
                hasContent = 1;
                continue;
            }
        }

        while(indent-- > lastIndent)
            printf(" ");

        hasContent = 1;
        printHTMLEntities(c);
    }

    if(hasContent)
        printf("\n");
}

void openCodeBlockFenced(int *blankLines, int *lineStart) {
    int c, initChar;
    struct tag *pt, t;

    if(peek() != NULL) {
        /*закрыть инлайн теги*/
        while((pt = peek()) && pt->singleLine == 1) {
            pt->close(pt);
            pop();
        }

        /*закрыть многострочный тег тег*/
        popAndClose(blankLines, lineStart);
        printf("\n");
    }

    /*создать открывающий тег*/
    t = getTag(CODE_BLOCK);
    initChar = getch();
    t.kind = initChar == '`' ? FENCED_BACKTICK : FENCED_TILDE;
    t.level = 1;

    while((c = getch()) == initChar)
        t.level++;
    ungetch(c);

    push(t);

    /*пропустить пробелы и табуляции*/
    while((c = getch()) == ' ' || c == '\t')
        ;

    printf("<pre><code");
    /*получить название класса*/
    if(c != '\n' && c != EOF) {
        ungetch(c);
        printf(" class=\"language-");
        while((c = getch()) != '\n' && c != EOF && c != ' ')
            printf("%c", c);
        printf("\"");
    }
    printf(">");

    /*отбросить всё остальное*/
    if(c == ' ') {
        while((c = getch()) != '\n' && c != EOF)
            ;
    }
}

int isCloseCodeBlockFenced(void) {
    int c, initChar;

    char buf[500];
    int pos = 0;

    initChar = getch();
    buf[pos++] = initChar;

    while((c = getch()) == initChar) {
        if(pos < 500)
            buf[pos++] = c;
        else fprintf(stderr, "error: many indent symbols\n");
    }
    ungetch(c);

    if(peek()->level > pos) {
        /*возврат символов в поток*/
        while(pos > 0)
            ungetch(buf[--pos]);
        return 0;
    }

    while((c = getch()) == ' ' || c == '\t') {
        if(pos < 500)
            buf[pos++] = c;
        else fprintf(stderr, "error: many indent symbols\n");
    }
    ungetch(c);

    if(c == '\n' || c == EOF) {
        ungetch(c);
        return 1;
    }

    /*возврат символов в поток*/
    while(pos > 0)
        ungetch(buf[--pos]);
    return 0;
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

    while((c = getch()) == initChar || c == ' ' || c == '\t') {
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
            popAndClose(blankLines, lineStart);
        }
        else if(pt->type == PARAGRAPH) {
            if(*blankLines > 0 || isBreaks) {
                popAndClose(blankLines, lineStart);
            }
        }
        printf("\n");

        /*закрыть все уровни blockquote*/
        if((pt = peek()) != NULL && pt->type == BLOCKQUOTE) {
            while((pt = pop()) != NULL) {
                pt->close(pt);
                printf("\n");
            }
        }

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
            popAndClose(blankLines, lineStart);
            printf("\n");
        }
        else if(pt->type == PARAGRAPH) {
            if(*blankLines > 0) {
                popAndClose(blankLines, lineStart);
                printf("\n");

                if(peek() && peek()->type == BLOCKQUOTE) {
                    /*закрыть все уровни blockquote*/
                    while((pt = pop()) != NULL) {
                        pt->close(pt);
                        printf("\n");
                    }
                }
            }
            else
                printf("\n");
        }
        else if(pt->type == BLOCKQUOTE) {
            push(getTag(PARAGRAPH));
            printf("<p>");
        }
        // printf("\n");
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

    /*если предыдущий символ это пробел, то строка входит в блок цитат*/
    // int isBlockquoteLine = prevch() == ' ';
    // if(!isBlockquoteLine && findByBlockType(BLOCKQUOTE)) {
    //     /*закрыть все уровни blockquote*/
    //     while((pt = pop()) != NULL) {
    //         pt->close(pt);
    //         printf("\n");
    //     }
    // }

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
                popAndClose(blankLines, lineStart);
                printf("\n");
                indent = 0;
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
            if(*blankLines > 0) {
                popAndClose(blankLines, lineStart); /*сбросит lineStart*/
                *lineStart = indent;                /*есть отсупы, lineStart не равен 0*/
                printf("\n");
                struct tag t = getTag(CODE_BLOCK);
                t.kind = INDENTED;
                push(t);
                printf("<pre><code>");
            }
            else
                indent = 0;
        }
        else if(pt->type == BLOCKQUOTE) {
            if(indent >= TAB_STEP) {
                struct tag t = getTag(CODE_BLOCK);
                t.kind = INDENTED;
                push(t);
                printf("<pre><code>");
                *blankLines = 0;
            }
            else 
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

void headingTrailingHashes(char *spaceChar, int *pos) {
    int c;
    
    char buf[500];
    int p = 0;

    while((c = getch()) == '#') {
        if(p < 500)
            buf[p++] = c;
        else fprintf(stderr, "error: many indent symbols\n");
    }
    ungetch(c);

    while((c = getch()) == ' ' || c == '\t') {
        if(p < 500)
            buf[p++] = c;
        else fprintf(stderr, "error: many indent symbols\n");
    }
    ungetch(c);

    if(c != '\n' && c != EOF) {
        /*вывод буфера*/
        for(int i = 0; i < *pos; ++i)
                printf("%c", spaceChar[i]);
        *pos = 0;
        /*вывод буфера*/  
        for(int i = 0; i < p; ++i)
            printf("%c", buf[i]);
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

void printHTMLEntities(int c) {
    switch(c) {
        case '&': printf("&amp;"); break;
        case '<': printf("&lt;"); break;
        case '>': printf("&gt;"); break;
        case '"': printf("&quot;"); break;
        default: printf("%c", c); break;
    }
}

/*
буферезирует пробелы между символами и в конце строки
возврат:
    1 - символ был пробелом и обработан (нужен continue)
    0 - символ требует дальнейшей обработки
*/
int bufferInlineSpaces(int c, char *spaceChar, int *pos) {
    struct tag *pt;
    static int hasContent = 0;

    if(!hasContent)
        hasContent = (c != ' ' && c != '\t' && c != '>');
        
    if(hasContent) {
        if(c == ' ' || c == '\t') {
            spaceChar[(*pos)++] = c;
            return 1;
        }
        else if(prevch() == ' ' && c == '#' && peek() != NULL && peek()->type == HEADING) {
            /*обработка внутри заголовка*/
            ungetch(c);
            headingTrailingHashes(spaceChar, pos);
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

/*закрывает блок и сбрасывает отступ с пустыми строками*/
void popAndClose(int *blankLines, int *lineStart) {
    struct tag *pt = pop();
    if(pt) {
        pt->close(pt);
    }
    *blankLines = 0;
    *lineStart = 0;
}
