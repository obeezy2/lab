#include "preAssembler.h"
#include "../data_structure/list.h"
#include "../data_structure/tree.h"

#include "stddef.h"
#include "string.h"
#include "stdio.h"
#include "ctype.h"
#include "stdlib.h"

/* Define colors for text output */

#define RED "\x1B[31m"
#define RESET "\x1B[0m"
#define MAG "\x1B[35m"

/* Define file extensions */

#define asFile ".as"
#define amFile ".am"
/* Define maximum lengths */

#define MAX_MACRO_NAME_LEN 31
#define MAX_LINE_LENGTH 81

/* Define whitespace characters */

#define WHITESPACECHARS " \t\n\f\r\v"

static void skipSpaces(char **s)
{
    while (**s && isspace(**s))
    {
        (*s)++;
    }
}

static void skipSpacesReverse(char **s, char *base)
{
    while (**s && isspace(**s) && base != *s)
    {
        (*s)--;
    }
}
/* Enum defining different types of lines */

enum LineType
{
    emptyLine,
    defineMacro,
    endDefineMacro,
    otherLine,
    marcoAlreadyExists,
    invalidMacroDefinition,
    invalidEndMacroDefinition,
    macroCall,
    invalidMacroCall

};

/* Struct to represent a macro definition */

struct MacroDef
{
    char name[MAX_MACRO_NAME_LEN + 1];
    List lines;
};

/* Function prototypes */

static void *createLine(const void *copy);
static void destroyLine(void *item);
static void *createMacro(const void *copy);
static void destroyMacro(void *item);
enum LineType handleEndDefineMacro(char *token, char *line, struct MacroDef **macro);
enum LineType handleDefineMacro(char *token, char *line, struct MacroDef **macro, const WordTree macroLookup, List macroTable, struct MacroDef *newMacro);
void handleWhitespaceChars(char *token);

/* Function to create a line from a copy */

static void *createLine(const void *copy)
{
    const char *line = copy;
    return strcpy(malloc(strlen(line) + 1), line);
}

/* Function to destroy a line */

static void destroyLine(void *item)
{
    free(item);
}

/* Function to create a macro from a copy */

static void *createMacro(const void *copy)
{
    const struct MacroDef *copy1 = copy;
    struct MacroDef *newMacro = malloc(sizeof(struct MacroDef));
    strcpy(newMacro->name, copy1->name);
    newMacro->lines = createDynamicList(createLine, destroyLine);
    return newMacro;
}

/* Function to destroy a macro */

static void destroyMacro(void *item)
{
    const struct MacroDef *macro = item;
    listDealloc((List *)&macro->lines);
    free((void *)macro);
}
/* Function to check the type of line based on the contents, if its macro or not */

enum LineType checkLine(char *line, struct MacroDef **macro, const WordTree macroLookup, List macroTable)
{
    struct MacroDef newMacro = {0};
    struct MacroDef *local;
    char *token;
    token = strchr(line, ';');
    if (token)
        *token = '\0';
    skipSpaces(&line);
    if (*line == '\0')
        return emptyLine;
    token = strstr(line, "endmcro");
    if (token)
        return handleEndDefineMacro(token, line, macro);
    token = strstr(line, "mcro");
    if (token)
        return handleDefineMacro(token, line, macro, macroLookup, macroTable, &newMacro);

    token = strpbrk(line, WHITESPACECHARS);
    if (token)
        *token = '\0';
    local = checkIfExists(macroLookup, line);
    if (local == NULL)
    {
        *token = ' ';
        return otherLine;
    }
    token++;
    skipSpaces(&token);
    if (*token != '\0')
        return invalidMacroCall;
    *macro = local;
    return macroCall;
}

/* Function to handle "endmcro" (end macro definition) token in line */

enum LineType handleEndDefineMacro(char *token, char *line, struct MacroDef **macro)
{
    char *temp;
    temp = token;
    skipSpacesReverse(&temp, line);
    if (temp != line)
        return invalidEndMacroDefinition;
    token += 7;
    skipSpaces(&token);
    if (*token != '\0')
        return invalidEndMacroDefinition;
    *macro = NULL;
    return endDefineMacro;
}

/* Function to handle "mcro" (macro definition) token in line */

enum LineType handleDefineMacro(char *token, char *line, struct MacroDef **macro, const WordTree macroLookup, List macroTable, struct MacroDef *newMacro)
{
    char *temp;
    temp = token;
    skipSpacesReverse(&temp, line);
    if (temp != line)
        return invalidMacroDefinition;

    token += 4;
    skipSpaces(&token);
    line = token;

    token = strpbrk(line, WHITESPACECHARS);

    if (token)
        handleWhitespaceChars(token);

    *macro = checkIfExists(macroLookup, line);

    if (*macro)
        return marcoAlreadyExists;

    strcpy(newMacro->name, line);

    *macro = listInsertItem(macroTable, newMacro);

    insertWord(macroLookup, line, *macro);

    return defineMacro;
}

/* Function to handle whitespace characters in token */

void handleWhitespaceChars(char *token)
{
    *token = '\0';
    token++;
    skipSpaces(&token);
}

/* Function to open input and output files */

int openInputOutputFiles(const char *fileBaseName, FILE **inputFile, FILE **outputFile)
{
    size_t fileBaseNameLen;
    char *inputFileName;
    char *outputFileName;

    fileBaseNameLen = strlen(fileBaseName);
    inputFileName = strcat(strcpy(malloc(fileBaseNameLen + strlen(asFile) + 1), fileBaseName), asFile);
    outputFileName = strcat(strcpy(malloc(fileBaseNameLen + strlen(amFile) + 1), fileBaseName), amFile);

    *inputFile = fopen(inputFileName, "r");
    *outputFile = fopen(outputFileName, "w");

    free(inputFileName);
    free(outputFileName);

    if (*inputFile == NULL || *outputFile == NULL)
    {
        return -1;
    }

    return 0;
}

/* Function to close input and output files */

void closeInputOutputFiles(FILE *inputFile, FILE *outputFile)
{
    fclose(inputFile);
    fclose(outputFile);
}
/* Function to create a macro table and its lookup tree */

void createMacroTable(List *macroTable, WordTree *macroTableLookup)
{
    *macroTable = createDynamicList(createMacro, destroyMacro);
    *macroTableLookup = wordT();
}

/* Function to destroy a macro table and its lookup tree */

void destroyMacroTable(List *macroTable, WordTree *macroTableLookup)
{
    listDealloc(macroTable);
    treeDealloc(macroTableLookup);
}

/* Function to process a line of input, including handling of macro definitions and macro calls */

void processLine(char *lineBuff, struct MacroDef **macro, WordTree macroTableLookup, List macroTable, FILE *outputFile)
{
    void *const *begin;
    void *const *end;

    switch (checkLine(lineBuff, macro, macroTableLookup, macroTable))
    {
    case emptyLine:
        break;
    case defineMacro:
        break;

    case endDefineMacro:
        break;

    case macroCall:
        for (begin = listGetBegin((*macro)->lines), end = listGetEnd((*macro)->lines); begin <= end; begin++)
        {
            if (*begin)
            {
                fputs((const char *)(*begin), outputFile);
            }
        }
        *macro = NULL;
        break;
    case otherLine:
        if (*macro)
        {
            listInsertItem((*macro)->lines, &lineBuff[0]);
        }
        else
        {
            fputs(lineBuff, outputFile);
        }
        break;
    case marcoAlreadyExists:
        fprintf(stderr, MAG "macro already exists" RESET);
        break;
    case invalidEndMacroDefinition:
        fprintf(stderr, MAG "ERROR: bad end macro definition" RESET);

        break;
    case invalidMacroDefinition:
        fprintf(stderr, MAG "ERROR: bad  macro definition" RESET);

        break;
    case invalidMacroCall:
        fprintf(stderr, MAG "ERROR: bad  macro call" RESET);
        break;
    }
}

/* Main preprocessing function */

const char *preprocess(const char *fileBaseName)
{
    char lineBuff[MAX_LINE_LENGTH] = {0};
    size_t fileBaseNameLen;
    char *outputFileName;
    FILE *outputFile;
    FILE *inputFile;
    List macroTable = NULL;
    WordTree macroTableLookup = NULL;
    struct MacroDef *macro = NULL;

    if (openInputOutputFiles(fileBaseName, &inputFile, &outputFile) == -1)
    {
        return NULL;
    }

    createMacroTable(&macroTable, &macroTableLookup);

    while (fgets(lineBuff, sizeof(lineBuff), inputFile))
    {
        processLine(lineBuff, &macro, macroTableLookup, macroTable, outputFile);
    }

    destroyMacroTable(&macroTable, &macroTableLookup);

    closeInputOutputFiles(inputFile, outputFile);

    fileBaseNameLen = strlen(fileBaseName);
    outputFileName = strcat(strcpy(malloc(fileBaseNameLen + strlen(amFile) + 1), fileBaseName), amFile);

    return outputFileName;
}
