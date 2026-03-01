#include <stdio.h>
#include <string.h>
#include "cmdc.h"

#define BUFSIZE 50
static struct tag buf[BUFSIZE];
static int pos = 0;

void push(struct tag t) {
    if(pos < BUFSIZE)
        buf[pos++] = t;
    else
        fprintf(stderr, "error: overflow tag buffer\n");
}

struct tag *pop(void) {
    if(pos > 0)
        return &buf[--pos];
    else {
        // fprintf(stderr, "error: tag buffer is empty\n");
        return NULL;
    }
}

struct tag *peek(void) {
    if(pos > 0)
        return &buf[pos - 1];
    else {
        // fprintf(stderr, "error: tag buffer is empty\n");
        return NULL;
    }
}

static void printCloseTag(struct tag *self) {
    if(self->htmlTag != NULL) {
        if(self->level > 0)
            printf("</%s%d>", self->htmlTag, self->level);
        else
            printf("</%s>", self->htmlTag);
    }
    else
        fprintf(stderr, "error: htmlTag is NULL\n");
}

struct tag getTag(char *type) {
    struct tag t;
    t.type = NULL;
    t.htmlTag = NULL;
    t.className = NULL;
    t.level = 0;
    t.singleLine = 0;
    t.close = printCloseTag;

    if(strcmp(type, "heading") == 0) {
        t.type = "heading";
        t.htmlTag = "h";
        t.level = 1;
        t.singleLine = 1;
    }
    else if(strcmp(type, "paragraph") == 0) {
        t.type = "paragraph";
        t.htmlTag = "p";
    }
    else if(strcmp(type, "codeBlock") == 0) {
        t.type = "codeBlock";
        t.htmlTag = "code></pre";
    }
    else 
        fprintf(stderr, "error: unknown tag name\n");
    return t;
}

 