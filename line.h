#ifndef __LINE_H__
#define __LINE_H__

#define SEP '\t'

typedef struct
{
    char *column;
    size_t length;
} LineColumn;


int getColumns(char *string, char separator);

int *getComplementaryColumns(int N, int n, int *columns);

int splitLine(LineColumn *columns, char *string, char separator);

char *buildLine(LineColumn *lineColumns, int *columns, int nColumns);

#endif
