#ifndef __READLINE_H__
#define __READLINE_H__

#include <stdio.h>

typedef struct
{
    FILE *fp;
    char *buffer;
    size_t size;
    size_t n;
} LineNode;

char *readLine(LineNode *);

#endif

/*
#include <stdio.h>
#include <stdlib.h>
#include "readLine.h"

int main(int argc, char *argv[])
{
    LineNode *lineNode = malloc(sizeof(LineNode));
    FILE *fp = fopen(argv[1], "r");
    if (!fp) exit(EXIT_FAILURE);
    lineNode->fp = fp;
    lineNode->size = 1024;
    lineNode->buffer = malloc(lineNode->size);
    while (readLine(lineNode))
    {
        printf("%llu: %s\n", lineNode->n, lineNode->buffer);
    }
    fclose(fp);
    free(lineNode->buffer);
    free(lineNode);
    return 0;
}
*/