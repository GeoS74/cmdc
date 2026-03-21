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

struct tag *findByBlockType(enum blockType type) {
    int i = pos;
    while(--i >= 0) {
        if(buf[i].type == type)
            return &buf[i];
    }
    return NULL;
}

static void printCloseTag(struct tag *self) {
    if(self->htmlTag != NULL) {
        if(self->level > 0 && self->type == HEADING)
            printf("</%s%d>", self->htmlTag, self->level);
        else
            printf("</%s>", self->htmlTag);
    }
    else
        fprintf(stderr, "error: htmlTag is NULL\n");
}

struct tag getTag(enum blockType type) {
    struct tag t;
    t.type = NONE_TYPE;
    t.htmlTag = NULL;
    t.className = NULL;
    t.level = 0;
    t.singleLine = 0;
    t.close = printCloseTag;
    t.kind = NONE_KIND;

    switch(type) {
        case HEADING:
            t.type = HEADING;
            t.htmlTag = "h";
            t.singleLine = 1;
            break;
        case PARAGRAPH:
            t.type = PARAGRAPH;
            t.htmlTag = "p";
            break;
        case CODE_BLOCK:
            t.type = CODE_BLOCK;
            t.htmlTag = "code></pre";
            break;
        case CODE_INLINE:
            t.type = CODE_INLINE;
            t.htmlTag = "code";
            break;
        default:
            fprintf(stderr, "error: unknown tag name\n");
            break;
    }
    return t;
}
