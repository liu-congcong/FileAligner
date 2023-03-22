#ifndef __LINE_H__
#define __LINE_H__

#include <string.h>
#include <stdlib.h>
#define SEP '\t'

typedef struct lineColumn
{
    char *column;
    int length;
} lineColumn;

int getColumns(char *string, char seperator)
{
    int columns = 1;
    while (*string)
    {
        if (*string++ == seperator)
            columns++;
    }
    return columns;
}

int splitLine(lineColumn *lineColumns, char *line, int length, char seperator)
{
    int linePosition = 0;
    int linePosition_ = 0;
    for (; linePosition < length; linePosition++)
    {
        /* 012#45#789# */
        if (line[linePosition] == seperator)
        {
            lineColumns->column = line + linePosition_;
            lineColumns->length = linePosition - linePosition_;
            linePosition_ = linePosition + 1;
            lineColumns++;
        }
    }
    lineColumns->column = line + linePosition_;
    lineColumns->length = linePosition - linePosition_;
    return 0;
}

char *buildLine(lineColumn *lineColumns, int *columnList, int columns)
{
    int length = columns; /* strlen + 1*/
    for (int i = 0; i < columns; i++)
        length += lineColumns[columnList[i]].length;

    char *line = malloc(length * sizeof(char));
    length = 0;
    for (int i = 0; i < columns; i++)
    {
        lineColumn column = lineColumns[columnList[i]];
        strncpy(line + length, column.column, column.length);
        length += column.length;
        line[length++] = SEP;
    }
    line[length - 1] = 0;
    return line;
}

#endif
