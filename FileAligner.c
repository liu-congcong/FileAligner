#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "hash.h"
#include "line.h"
#define MAX_TARGETS 1024
#define MAX_LENGTH 1024*1024*1024
#define HASHSIZE 100000

typedef struct Node
{
    char *key;
    char **values;
    uint8_t *flags;
    struct Node *next;
} Node;

int insterHash(Node **hashTable, char *key, char *value, int n, int i)
{
    int hash = elfHash(key) % HASHSIZE;
    Node *node = hashTable[hash];
    bool flag = true;
    if (node)
    {
        while (true)
        {
            if (!strcmp(node->key, key))
            {
                flag = false;
                break;
            }
            else if (!node->next)
                break;
            node = node->next;
        }
        if (flag)
        {
            Node *newNode = malloc(sizeof(Node));
            newNode->key = key;
            newNode->values = malloc(n * sizeof(char *));
            memset(newNode->values, 0, n * sizeof(char *));
            newNode->values[i] = value;
            newNode->flags = malloc(n * sizeof(uint8_t));
            memset(newNode->flags, 0, n * sizeof(uint8_t));
            newNode->flags[i] = 1;
            newNode->next = NULL;
            node->next = newNode;
        }
        else
        {
            assert(!node->flags[i]);
            node->values[i] = value;
        }
    }
    else
    {
        Node *node = malloc(sizeof(Node));
        node->key = key;
        node->values = malloc(n * sizeof(char *));
        memset(node->values, 0, n * sizeof(char *));
        node->values[i] = value;
        node->flags = malloc(n * sizeof(uint8_t));
        memset(node->flags, 0, n * sizeof(uint8_t));
        node->flags[i] = 1;
        node->next = NULL;
        hashTable[hash] = node;
    }
    return 0;
}

int readFiles(Node **hashTable, char **headerLines, char **blankLines, char **fileList, char separator, int **targets, int targetColumns, int files)
{
    char *buffer = malloc(MAX_LENGTH * sizeof(char));
    int lineLength;
    for (int i = 0; i < files; i++)
    {
        FILE *openFile = fopen(fileList[i], "r");
        assert(openFile); /* open file failed */

        /* header line */
        fgets(buffer, MAX_LENGTH, openFile);
        lineLength = strlen(buffer);
        if (lineLength && buffer[lineLength - 1] == '\n')
            buffer[--lineLength] = 0;

        /* columns of file */
        int columns = getColumns(buffer, separator);

        /* targets */
        int *targetColumnList = targets[i];
        for (int j = 0; j < targetColumns; j++)
            assert(targetColumnList[j] < columns); /* target col <= max cols */

        /* targets removed columns */
        int *nonTargetColumnList = NULL;
        int notSelectedTargets;
        if (targetColumns < columns)
        {
            notSelectedTargets = columns - targetColumns;
            nonTargetColumnList = getComplementaryColumns(columns, targetColumns, targetColumnList);
        }
        else
        {
            notSelectedTargets = targetColumns;
            nonTargetColumnList = malloc(targetColumns * sizeof(int));
            memcpy(nonTargetColumnList, targetColumnList, targetColumns * sizeof(int));
        }

        /* line struct */
        lineColumn *lineColumns = malloc(columns * sizeof(lineColumn));

        /* split line */
        splitLine(lineColumns, buffer, separator);

        /* rebuild header */
        headerLines[i] = buildLine(lineColumns, nonTargetColumnList, notSelectedTargets);

        /* blank line */
        char *blankLine = malloc(notSelectedTargets * sizeof(char));
        memset(blankLine, SEP, notSelectedTargets);
        blankLine[notSelectedTargets - 1] = 0;
        blankLines[i] = blankLine;

        while (fgets(buffer, MAX_LENGTH, openFile))
        {
            lineLength = strlen(buffer);
            if (lineLength && buffer[lineLength - 1] == '\n')
                buffer[--lineLength] = 0;
            splitLine(lineColumns, buffer, separator);
            char *key = buildLine(lineColumns, targetColumnList, targetColumns);
            char *value = buildLine(lineColumns, nonTargetColumnList, notSelectedTargets);
            // printf("file: %s, key: %s, value: %s\n", fileList[i], key, value);
            insterHash(hashTable, key, value, files, i);
        }
        fclose(openFile);
        free(nonTargetColumnList);
        free(lineColumns);
    }
    free(buffer);
    return 0;
}

int *getTargets(char *string, int *columns)
{
    int *columnList = malloc(MAX_TARGETS * sizeof(int));
    int column;
    int i = 0;
    char *token = strtok(string, ",");
    while (token)
    {
        column = atoi(token);
        assert(column > 0);
        columnList[i++] = column - 1;
        token = strtok(NULL, ",");
    }
    if (*columns)
        assert(*columns == i);
    else
        *columns = i;
    return columnList;
}

char *createHeaderLine(int separators, char **fieldList, int fields)
{
    char *buffer = malloc(MAX_LENGTH * sizeof(char));
    int i = 0;
    while (i < separators)
        buffer[i++] = SEP;
    for (int j = 0; j < fields; j++)
    {
        int length = strlen(fieldList[j]);
        assert(i + length + 2 <= MAX_LENGTH); /* prefix + header + SEP|\n + 0 */
        strcpy(buffer + i, fieldList[j]);
        i += length;
        buffer[i++] = SEP;
    }
    buffer[i - 1] = '\n';
    buffer[i] = 0;
    return buffer;
}

int output(Node **hashTable, int n, char *headerLine, char **blankLines, char *output)
{
    FILE *openFile = fopen(output, "w");
    assert(openFile);

    /* write header */
    fputs(headerLine, openFile);

    for (int i = 0; i < HASHSIZE; i++)
    {
        Node *node = hashTable[i];
        while (node)
        {
            fputs(node->key, openFile);
            fputc(SEP, openFile);
            for (int j = 0; j < n; j++)
            {
                /* printf("key: %s, value: %s\n", node->key, (node->values)[j] ? (node->values)[j] : blankLines[j]); */
                fputs((node->values)[j] ? (node->values)[j] : blankLines[j], openFile);
                fputc(j == n - 1 ? '\n' : SEP, openFile);
            }
            node = node->next;
        }
    }
    fclose(openFile);
    return 0;
}

int ioTest(char **fileList, int files)
{
    FILE *openFile = fopen(fileList[0], "wb");
    assert (openFile);
    fclose(openFile);
    for (int i = 1; i < files; i++)
        assert (!access(fileList[i], R_OK));
    return 0;
}

int printHelp()
{
    puts("filenAligner v1.0.0");
    puts("Align multiple files based on selected columns.");
    puts("https://github.com/liu-congcong/fileAligner");
    puts("\nUsage:");
    puts("  filenAligner [options]");
    puts("\nOptions:");
    puts("  -i    Path to the input files");
    puts("        A header line for each file should be existed");
    puts("  -t    Target columns for alignment");
    puts("        Examples:");
    puts("          '1,2 2,3' aligns file1 on columns 1, 2 and file2 on columns 2, 3");
    puts("          '1,2' aligns all files on columns 1, 2");
    puts("  -o    Path to the output file");
    puts("  -s    Column separator in input files: table (default) | comma | space");
    return 0;
}

int main(int argc, char *argv[])
{
    char **fileList = malloc(1024 * sizeof(char *));
    memset(fileList, 0, 1024);
    int files = 1;
    int **targetList = malloc(1023 * sizeof(int *));
    int targets = 0;
    int targetColumns = 0;
    char separator = '\t';
    int flag = 0;

    for (int i = 1; i < argc; i++)
    {
        if (!strncasecmp("-i", argv[i], 2))
            flag = 1;
        else if (!strncasecmp("-o", argv[i], 2))
            flag = 2;
        else if (!strncasecmp("-t", argv[i], 2))
            flag = 3;
        else if (!strncasecmp("-s", argv[i], 2))
            flag = 4;
        else if (flag == 1)
            fileList[files++] = argv[i];
        else if (flag == 2)
            fileList[0] = argv[i];
        else if (flag == 3)
            targetList[targets++] = getTargets(argv[i], &targetColumns);
        else if (flag == 4)
        {
            if (!strncasecmp(argv[i], "c", 1))
                separator = ',';
            else if (!strncasecmp(argv[i], "s", 1))
                separator = ' ';
            else if (!strncasecmp(argv[i], "t", 1))
                separator = '\t';
            else
            {
                printHelp();
                exit(EXIT_FAILURE);
            }
        }
    }

    if (targets == 1)
    {
        targets = files - 1;
        for (int i = 1; i < targets; i++)
            targetList[i] = targetList[0];
    }

    if (files - 1 != targets || !fileList[0] || !targets)
    {
        printHelp();
        exit(EXIT_FAILURE);
    }
    ioTest(fileList, files);

    Node **hashTable = malloc(HASHSIZE * sizeof(Node *));
    memset(hashTable, 0, HASHSIZE * sizeof(Node *));

    char **headerLines = malloc(targets * sizeof(char *));
    char **blankLines = malloc(targets * sizeof(char *));
    readFiles(hashTable, headerLines, blankLines, fileList + 1, separator, targetList, targetColumns, targets);
    char *headerLine = createHeaderLine(targetColumns, headerLines, targets);
    output(hashTable, targets, headerLine, blankLines, fileList[0]);
    /* free */
    return 0;
}
