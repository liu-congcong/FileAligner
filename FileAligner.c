#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include "hash.h"
#include "line.h"
#define MAX_TARGETS 1024
#define MAX_LENGTH 1024*1024*1024
#define HASHSIZE 10000000

typedef struct hashNode
{
    char *key;
    char **values;
    struct hashNode *next;
} hashNode;

int insterHash(hashNode **hashTable, char *key, char *value, int n, int i)
{
    int hash = elfHash(key) % HASHSIZE;
    hashNode *node = hashTable[hash];
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
            hashNode *newNode = malloc(sizeof(hashNode));
            newNode->key = key;
            newNode->values = malloc(n * sizeof(char *));
            newNode->values[i] = value;
            newNode->next = NULL;
            node->next = newNode;
        }
        else
        {
            node->values[i] = value;
        }
    }
    else
    {
        hashNode *node = malloc(sizeof(hashNode));
        node->key = key;
        node->values = malloc(n * sizeof(char *));
        node->values[i] = value;
        node->next = NULL;
        hashTable[hash] = node;
    }
    return 0;
}

int readFiles(hashNode **hashTable, char **headerLines, char **blankLines, char **fileList, char seperator, int **targets, int files)
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
        int columns = getColumns(buffer, seperator);

        /* all columns */
        int *allTargetList = malloc(columns * sizeof(int));
        for (int column = 0; column < columns; column++)
            allTargetList[column] = column;

        /* line struct */
        lineColumn *lineColumns = malloc(columns * sizeof(lineColumn));

        /* split line */
        splitLine(lineColumns, buffer, lineLength, seperator);
        
        /* rebuild header */
        headerLines[i] = buildLine(lineColumns, allTargetList, columns);

        /* blank line */
        char *blankLine = malloc(columns * sizeof(char));
        memset(blankLine, SEP, columns);
        blankLine[columns - 1] = 0;
        blankLines[i] = blankLine;

        int *selectedTargetList = targets[i];
        int selectedTargets = 0;
        while (selectedTargetList[selectedTargets] >= 0)
            selectedTargets++;

        while (fgets(buffer, MAX_LENGTH, openFile))
        {
            lineLength = strlen(buffer);
            if (lineLength && buffer[lineLength - 1] == '\n')
                buffer[--lineLength] = 0;
            splitLine(lineColumns, buffer, lineLength, seperator);
            char *key = buildLine(lineColumns, selectedTargetList, selectedTargets);
            char *value = buildLine(lineColumns, allTargetList, columns);
            /* printf("key: %s, value: %s\n", key, value) */
            insterHash(hashTable, key, value, files, i);
        }
        fclose(openFile);
        free(allTargetList);
        free(lineColumns);
    }
    return 0;
}

int *getTargets(char *columns_string)
{
    int *columns = malloc(MAX_TARGETS * sizeof(int));
    int column_index = 0;
    char *token = strtok(columns_string, ",");
    while (token)
    {
        columns[column_index++] = atoi(token) - 1;
        token = strtok(NULL, ",");
    }
    columns[column_index] = -1;
    return columns;
}

int output(hashNode **hashTable, int n, char **headerLines, char **blankLines, char *output)
{
    FILE *openFile = fopen(output, "w");
    assert(openFile);

    /* write header */
    for (int i = 0; i < n; i++)
    {
        fputs(headerLines[i], openFile);
        fputc(i == n - 1 ? '\n' : SEP, openFile);
    }

    for (int i = 0; i < HASHSIZE; i++)
    {
        hashNode *node = hashTable[i];
        while (node)
        {
            for (int j = 0; j < n; j++)
            {
                /* printf("key: %s, value: %s\n", node->key, (node->values)[j] ? (node->values)[j] : blankLines[j]) */
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

int main(int argc, char *argv[])
{
    char **fileList = malloc(1024 * sizeof(char *));
    memset(fileList, 0, 1024);
    int files = 1;
    int **targetList = malloc(1023 * sizeof(int *));
    memset(targetList, 0, 1023);
    int targets = 0;
    char seperator = '\t';
    int flag = 0;

    for (int i = 1; i < argc; i++)
    {
        if (!strncasecmp("-i", argv[i], 2) || !strncasecmp("--i", argv[i], 3))
            flag = 1;
        else if (!strncasecmp("-o", argv[i], 2) || !strncasecmp("--o", argv[i], 3))
            flag = 2;
        else if (!strncasecmp("-t", argv[i], 2) || !strncasecmp("--t", argv[i], 3))
            flag = 3;
        else if (!strncasecmp("-s", argv[i], 2) || !strncasecmp("--s", argv[i], 3))
            flag = 4;
        else if (flag == 1)
            fileList[files++] = argv[i];
        else if (flag == 2)
            fileList[0] = argv[i];
        else if (flag == 3)
            targetList[targets++] = getTargets(argv[i]);
        else if (flag == 4)
        {
            if (!strncasecmp(argv[i], "c", 1))
                seperator = ',';
            else if (!strncasecmp(argv[i], "s", 1))
                seperator = ' ';
            else if (!strncasecmp(argv[i], "t", 1))
                seperator = '\t';
            else
            {
                puts("-s/--seperator: <table|comma|space>");
                exit(EXIT_SUCCESS);
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
        puts("\nAlign files according to the selected columns (https://github.com/liu-congcong/FileAligner)");
        puts("Usage:");
        printf("    %s -i <files> -t <cols> -o <file> [-s <sep>]\n", argv[0]);
        printf("    e.g.: %s -i input1 input2 -t 1,2,8 1,6,8 -o output -s t\n", argv[0]);
        printf("          %s -i input1 input2 input3 -t 10,1 -o output -s c\n", argv[0]);
        puts("Options:");
        puts("    -i/--inputs: <input1> ... <inputN> files with a header line");
        puts("    -t/--targets: <1col1,...,1colM> ... <Ncol1,...,NcolM>");
        puts("                  <col1,...,colM> for all files");
        puts("    -o/--output: <output>");
        puts("    -s/--seperator: <table|comma|space>\n");
        exit(EXIT_SUCCESS);
    }
    ioTest(fileList, files);

    hashNode **hashTable = malloc(HASHSIZE * sizeof(hashNode *));
    memset(hashTable, 0, HASHSIZE * sizeof(hashNode *));

    char **headerLines = malloc(targets * sizeof(char *));
    char **blankLines = malloc(targets * sizeof(char *));

    readFiles(hashTable, headerLines, blankLines, fileList + 1, seperator, targetList, targets);
    output(hashTable, targets, headerLines, blankLines, fileList[0]);
    /* free */
    return 0;
}
