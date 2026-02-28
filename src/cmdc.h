#ifndef CMDC_H
#define CMDC_H

void parser(void);

int getch(void);
void ungetch(int c);

struct tag {
    char *type; /*тип html тега*/
    char *htmlTag; /*html тег (h, a, em и т.д.)*/
    char *className; /*html class*/
    int level; /*уровень (для заголовков)*/
    int singleLine; /*если 1 - однострочный тег, 0 - многострочный*/
    void (*close)(struct tag *self); /*функция генерирует закрывающий тег и сразу выводит в поток*/
};

/*генерирует структуры тегов*/
struct tag getTag(char *type);
void push(struct tag t);
struct tag *pop(void);
struct tag *peek(void);

#endif