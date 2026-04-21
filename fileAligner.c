/* gcc -static -o fileAligner fileAligner.c hash.c line.c readLine.c */
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hash.h"
#include "line.h"
#include "readLine.h"

#define MAX_TARGETS 1024

typedef struct Node
{
    char *key;
    char **values;
    uint8_t *flags;
    struct Node *next;
} Node;

Node *initializeNode(int n)
{
    Node *node = malloc(sizeof(Node));
    node->values = malloc(n * sizeof(char *));
    memset(node->values, 0, n * sizeof(char *));
    node->flags = malloc(n * sizeof(uint8_t));
    memset(node->flags, 0, n * sizeof(uint8_t));
    node->next = NULL;
    return node;
}

int insertHash(Node **hashTable, unsigned int hashTableSize, char *key, char *value, int n, int i)
{
    unsigned int hash = elfHash(key) % hashTableSize;
    Node *node = hashTable[hash];
    if (node)
    {
        while (strcmp(node->key, key))
        {
            if (node->next) node = node->next;
            else
            {
                node->next = initializeNode(n);
                node = node->next;
                node->key = key;
                break;
            }
        }
    }
    else
    {
        hashTable[hash] = initializeNode(n);
        node = hashTable[hash];
        node->key = key;
    }
    node->values[i] = value;
    node->flags[i] = 1;
    return 0;
}

int readFile(Node **hashTable, unsigned int hashTableSize, int i, char **header, char **blank, char *emptyColumn, char *file, char separator, int *targetColumns, int nTargetColumns, int n)
{
    LineColumn *lineColumns = NULL;
    int *nonTargetColumns = NULL;
    int nNonTargetColumns = 0;
    FILE *fp = fopen(file, "r");
    if (!fp) exit(EXIT_FAILURE);

    LineNode *node = malloc(sizeof(LineNode));
    node->fp = fp;
    node->size = 1024;
    node->buffer = malloc(node->size);
    while (readLine(node))
    {
        if (lineColumns) /* body */
        {
            splitLine(lineColumns, node->buffer, separator);
            char *key = buildLine(lineColumns, targetColumns, nTargetColumns);
            char *value = buildLine(lineColumns, nonTargetColumns, nNonTargetColumns);
            insertHash(hashTable, hashTableSize, key, value, n, i);
        }
        else /* header */
        {
            int nColumns = getColumns(node->buffer, separator); /* number of columns of file */
            for (int i = 0; i < nTargetColumns; i++) assert(targetColumns[i] < nColumns); /* target col <= max cols */
            if (nTargetColumns < nColumns)
            {
                nNonTargetColumns = nColumns - nTargetColumns;
                nonTargetColumns = getComplementaryColumns(nColumns, nTargetColumns, targetColumns);
            }
            else
            {
                nNonTargetColumns = nTargetColumns;
                nonTargetColumns = malloc(nTargetColumns * sizeof(int));
                memcpy(nonTargetColumns, targetColumns, nTargetColumns * sizeof(int));
            }
            lineColumns = malloc(nColumns * sizeof(LineColumn)); /* line struct */
            splitLine(lineColumns, node->buffer, separator); /* split line */
            *header = buildLine(lineColumns, nonTargetColumns, nNonTargetColumns); /* rebuild header */
            size_t emptyColumnSize = strlen(emptyColumn);
            *blank = malloc(nNonTargetColumns * (emptyColumnSize + 1) * sizeof(char)); /* blank line */
            memset(*blank, SEP, nNonTargetColumns * (emptyColumnSize + 1) * sizeof(char));
            (*blank)[nNonTargetColumns * (emptyColumnSize + 1) - 1] = 0;
            for (int i = 0; i < nNonTargetColumns; i++) memcpy(*blank + i * (emptyColumnSize + 1), emptyColumn, emptyColumnSize);
        }
    }
    fclose(fp);
    free(nonTargetColumns);
    free(lineColumns);
    free(node->buffer);
    free(node);
    return 0;
}

int *getTargets(char *string, int *nColumns)
{
    int *columns = malloc(MAX_TARGETS * sizeof(int));
    int i = 0;
    char *token = strtok(string, ",");
    while (token)
    {
        columns[i] = atoi(token) - 1;
        if (columns[i] < 0) exit(EXIT_FAILURE);
        i++;
        token = strtok(NULL, ",");
    }
    if (*nColumns)
    {
        if (*nColumns != i) exit(EXIT_FAILURE);
    }
    else *nColumns = i;
    return columns;
}

char *createHeaderLine(int offset, char **headers, int n)
{
    size_t *sizes = malloc(n * sizeof(size_t));
    size_t size = offset + n;
    for (int i = 0; i < n; i++)
    {
        sizes[i] = strlen(headers[i]);
        size += sizes[i];
    }
    char *buffer = malloc(size * sizeof(char));
    memset(buffer, SEP, offset * sizeof(char));
    for (int i = 0; i < n; i++)
    {
        memcpy(buffer + offset, headers[i], sizes[i]);
        offset += sizes[i];
        buffer[offset++] = SEP;
    }
    buffer[size - 1] = 0;
    free(sizes);
    return buffer;
}

int outputData(Node **hashTable, unsigned int hashTableSize, int n, char *header, char **blanks)
{
    Node *node = NULL;
    Node *temp = NULL;

    /* output header */
    puts(header);

    for (unsigned int i = 0; i < hashTableSize; i++)
    {
        node = hashTable[i];
        while (node)
        {
            printf("%s%c", node->key, SEP);
            for (int j = 0; j < n; j++)
            {
                if ((node->values)[j])
                {
                    printf("%s%c", (node->values)[j], j == n - 1 ? '\n' : SEP);
                    free((node->values)[j]);
                }
                else printf("%s%c", blanks[j], j == n - 1 ? '\n' : SEP);
            }
            temp = node;
            node = node->next;
            free(temp->key);
            free(temp->values);
            free(temp->flags);
            free(temp);
        }
    }
    return 0;
}

int ioTest(char **files, int n)
{
    for (int i = 0; i < n; i++)
    {
        if (access(files[i], R_OK)) exit(EXIT_FAILURE);
    }
    return 0;
}

int printHelp()
{
    puts("filenAligner v1.1.4");
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
    puts("  -h    Size of the hash table (1-2147483647, default: 1000000)");
    puts("  -s    Column separator in input files: table|comma|space (default: table)");
    puts("  -v    Value of the empty column (default: \"\")");
    return 0;
}

int main(int argc, char *argv[])
{
    char **files = malloc(argc * sizeof(char *));
    memset(files, 0, argc * sizeof(char *));
    int nFiles = 0;
    int **targets = malloc(MAX_TARGETS * sizeof(int *));
    memset(targets, 0, MAX_TARGETS * sizeof(int *));
    int nTargets = 0;
    int nTargetColumns = 0;
    unsigned int hashTableSize = 1000000;
    char separator = '\t';
    char *emptyColumn = "";
    int option;
    while ((option = getopt(argc, argv, "i:t:h:s:v:")) != -1)
    {
        switch (option)
        {
            case 'i':
                for (int i = optind - 1; i < argc && argv[i][0] != '-'; i++) files[nFiles++] = argv[i];
                break;
            case 't':
                for (int i = optind - 1; i < argc && argv[i][0] != '-'; i++) targets[nTargets++] = getTargets(argv[i], &nTargetColumns);
                break;
            case 'h':
                hashTableSize = (unsigned int)atoi(optarg);
                break;
            case 's':
                if (!strncmp(optarg, "c", 1)) separator = ',';
                else if (!strncmp(optarg, "s", 1)) separator = ' ';
                else if (!strncmp(optarg, "t", 1)) separator = '\t';
                else
                {
                    printHelp();
                    exit(EXIT_FAILURE);
                }
                break;
            case 'v':
                emptyColumn = optarg;
                break;
            default:
                printHelp();
                exit(EXIT_FAILURE);
        }
    }

    if (!nTargets || (nTargets > 1 && nFiles != nTargets) || nFiles < 2 || hashTableSize <= 0)
    {
        printHelp();
        exit(EXIT_FAILURE);
    }
    ioTest(files, nFiles);

    Node **hashTable = malloc(hashTableSize * sizeof(Node *));
    memset(hashTable, 0, hashTableSize * sizeof(Node *));
    char **headers = malloc(nFiles * sizeof(char *));
    char **blanks = malloc(nFiles * sizeof(char *));
    for (int i = 0; i < nFiles; i++)
    {
        readFile(hashTable, hashTableSize, i, headers + i, blanks + i, emptyColumn, files[i], separator, nTargets == 1 ? targets[0] : targets[i], nTargetColumns, nFiles);
    }
    char *header = createHeaderLine(nTargetColumns, headers, nFiles);
    outputData(hashTable, hashTableSize, nFiles, header, blanks);

    free(files);
    for (int i = 0; i < nFiles; i++)
    {
        if ((nTargets == 1 && i == 0) || nTargets > 1) free(targets[i]);
        free(headers[i]);
        free(blanks[i]);
    }
    free(targets);
    free(headers);
    free(blanks);
    free(hashTable);
    free(header);
    return 0;
}
