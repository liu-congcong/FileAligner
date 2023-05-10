#ifndef __LINE_H__
#define __LINE_H__

#include <string.h>
#include <stdlib.h>
#define SEP '\t'

/* lineColumn *lineColumns = malloc(columns * sizeof(lineColumn)) */
typedef struct lineColumn
{
    char *column;
    int length;
} lineColumn;

int getColumns(char *string, char separator)
{
    int columns = 1;
    while (*string)
    {
        if (*string++ == separator)
            columns++;
    }
    return columns;
}

int getComplementaryColumns(int **complementaryColumnList, int *columnList, int columns, int totalColumns)
{
    int *totalColumnList = malloc(totalColumns * sizeof(int));
    *complementaryColumnList = malloc((totalColumns - columns) * sizeof(int));
    int complementaryColumns = 0;
    for (int i = 0; i < totalColumns; i++)
        totalColumnList[i] = 1;
    for (int i = 0; i < columns; i++)
        totalColumnList[columnList[i]] = 0;
    for (int i = 0; i < totalColumns; i++)
    {
        if (totalColumnList[i])
            (*complementaryColumnList)[complementaryColumns++] = i;
    }
    free(totalColumnList);
    return complementaryColumns;
}

int splitLine(lineColumn *columnList, char *string, char separator)
{
    int left = 0;
    int right = 0;
    int column = 0;
    while (string[right])
    {
        if (string[right] == separator)
        {
            columnList[column].column = string + left;
            columnList[column].length = right - left;
            left = right + 1;
            column++;
        }
        right++;
    }
    columnList[column].column = string + left;
    columnList[column].length = right - left;
    return 0;
}

char *buildLine(lineColumn *lineColumns, int *columnList, int columns)
{
    int length = columns; /* columns + separator + 0 = strlen + 1*/
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
