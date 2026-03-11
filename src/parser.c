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
void popAndClose(int *blankLines, int *lineStart);

int isCodeBlockFenced(void);
void codeBlockInline(int *blankLines, int *lineStart);
void codeBlockFenced(void);

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
        else if(peek() != NULL && peek()->type == PARAGRAPH && c == '`') {
            ungetch(c);
            codeBlockInline(&blankLines, &lineStart);
        }
        /*любой не пробельный символ в начале строки*/
        else if(lineStart == 1) {
            ungetch(c);

            if(c == '`' || c == '~')
                // codeBlock(&blankLines, &lineStart);
                if(isCodeBlockFenced())
                    codeBlockFenced();
                else
                    paragraph(&blankLines, &lineStart);

            else if(c == '*' || c == '-' || c == '_') /*тематический разрыв*/
                thematicBreak(&blankLines, &lineStart, lastIndent);
            else
                paragraph(&blankLines, &lineStart);
                 
        }
        /*экранирование*/
        else if(c == '\\') {
            backslash();
        }
        else {
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


// void flushBufferCodeBlockInline() {

// }

/*
*/
void printCodeBlockInline(char *buf, int max) {
    // printf("\n~~~~buf~~~\n");
    // for(int i = 0; i < max; ++i)
    //     printf("%c", buf[i]);
    // printf("\n~~~~~~~~\n");
    // return;

    struct tag t;
    int i, f, start;

    start = 0;
    i = 0;

    /*выводить символы до первого `*/
    for(; buf[i] != '`' && i < max; ++i)
        printf("%c", buf[i]);
    
    start = i;
    
    /*буфер пустой, завершить обработку*/
    if(i == max) 
        return;

    /*создать открывающий тег*/
    t = getTag(CODE_INLINE);
    for(; buf[i] == '`'; ++i)
        t.level++;
    
    /*найти закрывающий тег*/
    for(f = 0; i < max; ++i) {
        if(buf[i] == '`') {
            ++f;
            continue;
        }

        if(f == t.level) {
            printf("<code>");
            for(int w = start + f; w < i - f; ++w)
                printf("%c", buf[w]);
            t.close(&t);
            printCodeBlockInline(buf + i, max - i);
            f = 0;
            break;
        }
        f = 0;
    }

    if(f && f == t.level) {
        printf("<code>");
        for(int w = start + f; w < i - f; ++w)
            printf("%c", buf[w]);
        t.close(&t);
        // printf("para f = %d i = %d max = %d\n", f, i, max);
    }
    
    // for(int i = 0; i < pos; ++i)
    //     printf("%c", buf[i]);
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

    s = 0, match = 0;
    while((c = getch()) != EOF) {
        if(pos < 500)
            buf[pos++] = c;
        else fprintf(stderr, "error: many indent symbols\n");

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
        }

        if (c == '\n') {
            /*пустая строка, конец inline блока*/
            if(!s) {
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

/*
всегда вызывается внутри какого-то блока

Прерывает буферизацию если:
    - Пустая строка
    - Начало fenced-блока
    - Конец заголовка (перевод строки)
    - EOF
*/
void codeBlockInline(int *blankLines, int *lineStart){
    int c, s;
    
    char buf[500];
    int pos = 0;

    s = 0;
    while((c = getch()) != EOF) {
        if(pos < 500) 
            buf[pos++] = c;
        else fprintf(stderr, "error: many indent symbols\n");

        if (c == '\n') {
            /*инлайн код в заголовке, читать до первого переноса строки*/
            if(peek()->type == HEADING) {
                printCodeBlockInline(buf, pos);
                popAndClose(blankLines, lineStart);
                printf("\n");
                return;
            }
            /*пустая строка, конец inline блока*/
            else if(!s) {
                printCodeBlockInline(buf, pos);
                popAndClose(blankLines, lineStart);
                printf("\n");
                return;
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
                    --pos;
                    printCodeBlockInline(buf, pos);
                    popAndClose(blankLines, lineStart);
                    printf("\n");
                    return;
                }
                getch(); // вывести символ из потока
            }
            ++s;
        }
    }
    ungetch(c); // вернуть EOF в поток
    printCodeBlockInline(buf, pos);
    return;
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
    
    while((c = getch()) != '\n' && c != EOF) {
        if(c == initChar)
            break;

        if(pos < 500)
            buf[pos++] = c;
        else fprintf(stderr, "error: many indent symbols\n");
    }
    ungetch(c);

    /*возврат символов в поток*/
    while(pos > 0)
        ungetch(buf[--pos]);

    return c == initChar ? 0 : 1;
}


// void codeBlock(int *blankLines, int *lineStart) {
//     int c, initChar, count;
//     enum blockType type;

//     initChar = getch();
//     count = 1;

//     while((c = getch()) == initChar)
//         if(c == initChar) 
//             ++count;

//     ungetch(c);

//     /*определить тип кодового блока и передать управление*/
//     if(count < 3) {
//         while(count-- > 0)
//             ungetch(initChar);
//         paragraph(blankLines, lineStart);
//     }
//     else {
//         type = initChar == '`' ? FENCED_BACKTICK : FENCED_TILDE;
//         codeBlockFenced(type, count);
//     }
// }

void codeBlockFenced(){
    printf("codeBlockFenced\n");
    // struct tag *pt, t;

    // t = getTag(CODE_BLOCK);
    // t.kind = initChar == '`' ? FENCED_BACKTICK : FENCED_TILDE;
    // t.level = count;
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
            popAndClose(blankLines, lineStart);
        }
        else if(pt->type == PARAGRAPH) {
            if(*blankLines > 0 || isBreaks) {
                popAndClose(blankLines, lineStart);
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
            popAndClose(blankLines, lineStart);
        }
        else if(pt->type == PARAGRAPH) {
            if(*blankLines > 0) {
                popAndClose(blankLines, lineStart);
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
буферезирует пробелы между символами и в конце строки
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

/*закрывает блок и сбрасывает отступ с пустыми строками*/
void popAndClose(int *blankLines, int *lineStart) {
    struct tag *pt = pop();
    if(pt) {
        pt->close(pt);
    }
    *blankLines = 0;
    *lineStart = 0;
}
