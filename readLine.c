#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "readLine.h"

char *readLine(LineNode *node)
{
    char *flag = fgets(node->buffer, node->size, node->fp);
    if (flag)
    {
        node->n = strlen(node->buffer);
        while (node->n + 1 == node->size && node->buffer[node->size - 2] != '\n')
        {
            node->buffer = realloc(node->buffer, node->size * 2);
            assert(node->buffer);
            size_t newline = node->buffer[node->size - 2] == '\r' ? 2 : 1;
            if (fgets(node->buffer + node->size - newline, node->size + newline, node->fp))
            {
                node->n = node->size - newline + strlen(node->buffer + node->size - newline);
                node->size *= 2;
            }
            else assert(feof(node->fp));
        }
        while (node->n > 0 && (node->buffer[node->n - 1] == '\n' || node->buffer[node->n - 1] == '\r')) node->buffer[--node->n] = 0;
    }
    else assert(feof(node->fp));
    return flag;
}