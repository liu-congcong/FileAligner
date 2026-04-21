#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "line.h"
#define SEP '\t'

int getColumns(char *string, char separator)
{
    int n = 1;
    while (*string)
    {
        if (*string == separator) n++;
        string++;
    }
    return n;
}

int *getComplementaryColumns(int N, int n, int *columns)
{
    /*
    N: total number of columns
    n: number of selected columns
    columns: int, ..., int
    return: complementary columns
    */
    assert(N > n);
    int *totalColumns = malloc(N * sizeof(int));
    memset(totalColumns, 1, N * sizeof(int));
    int *complementaryColumns = malloc((N - n) * sizeof(int));
    for (int i = 0; i < n; i++) totalColumns[columns[i]] = 0;
    int m = 0; /* number of complementary columns */
    for (int i = 0; i < N; i++)
    {
        if (totalColumns[i]) complementaryColumns[m++] = i;
    }
    free(totalColumns);
    return complementaryColumns;
}

int splitLine(LineColumn *columns, char *string, char separator)
{
    int i = 0;
    int flag = 1;
    char *x = string;
    while (*string)
    {
        if (*string == separator && flag)
        {
            columns[i].column = x;
            columns[i].length = string - x;
            x = string + 1;
            i++;
        }
        else if (*string == '"') flag = !flag;
        string++;
    }
    columns[i].column = x;
    columns[i].length = string - x;
    return 0;
}

char *buildLine(LineColumn *lineColumns, int *columns, int nColumns)
{
    size_t offset = nColumns; /* columns + separator + 0 = strlen + 1 */
    for (int i = 0; i < nColumns; i++) offset += lineColumns[columns[i]].length;
    char *line = malloc(offset);
    offset = 0;
    for (int i = 0; i < nColumns; i++)
    {
        LineColumn column = lineColumns[columns[i]];
        memcpy(line + offset, column.column, column.length);
        offset += column.length;
        line[offset++] = SEP;
    }
    line[offset - 1] = 0;
    return line;
}
