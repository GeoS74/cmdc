#ifndef CMDC_H
#define CMDC_H

void parser(void);

int getch(void);
void ungetch(int c);

/* Типы блоков */
enum blockType {
    NONE_TYPE,
    PARAGRAPH,
    HEADING,
    CODE_BLOCK
};

/* Роды блоков (вариации) */
enum blockKind {
    NONE_KIND,      /* для блоков без вариаций */
    INDENTED,       /* для кода с отступами */
    FENCED          /* для кода с ``` */
};

struct tag {
    enum blockType type;            /*тип html тега*/
    char *htmlTag;                  /*html тег (h, a, em и т.д.)*/
    char *className;                /*html class*/
    int level;                      /*уровень (для заголовков)*/
    int singleLine;                 /*если 1 - однострочный тег, 0 - многострочный*/
    void (*close)(struct tag *self); /*функция генерирует закрывающий тег и сразу выводит в поток*/
    enum blockKind kind;            /*род блока (например для блока кода: indented/fenced)*/
};

/*генерирует структуры тегов*/
struct tag getTag(enum blockType);
void push(struct tag t);
struct tag *pop(void);
struct tag *peek(void);

/*печатает экранируемый символ*/
void printEscapedChar(int c);

#endif