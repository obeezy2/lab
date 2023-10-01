#include "output.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "../structs/code.h"
#include "../structs/external.h"
#include "../structs/symbol.h"
#define BASE64 "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
#define EXTEXT ".ext"
#define ENTEXT ".ent"
#define OBEXT ".ob"

/**
 * Gets the length of a string.
 * @param name1 Pointer to the string.
 * @return Length of the string.
 */

static size_t getLength(const char *name1)
{
    return strlen(name1);
}

/**
 * Constructs a full file name by concatenating a base name and an extension.
 * @param name1 Base name.
 * @param extension File extension.
 * @return Full file name or NULL if memory allocation fails.
 */

static char *getFileName(const char *name1, const char *extension)
{
    size_t length = getLength(name1);
    char *fullName = (char *)malloc(length + strlen(extension) + 1);
    if (fullName)
    {
        strcpy(fullName, name1);
        strcat(fullName, extension);
    }
    return fullName;
}

/**
 * Outputs entry symbols and their addresses to a file.
 * @param file Output file.
 * @param symbolTable List of symbols.
 */

static void outputEntry(FILE *file, List symbolTable)
{
    void *const *begin;
    void *const *end;

    for (begin = listGetBegin(symbolTable), end = listGetEnd(symbolTable); begin <= end; begin++)
    {
        if (*begin)
        {
            struct symbol *symVar = (struct symbol *)(*begin);
            if (symVar != NULL)
            {
                int symType = getSymbolType(symVar);
                if (symType >= getSymEntryCodeType())
                {
                    const char *symName = getSymbolName(symVar);
                    unsigned int symAdr = getSymbolAdr(symVar);
                    fprintf(file, "%s\t%u\n", symName, symAdr);
                }
            }
        }
    }
}

/**
 * Outputs entry symbols and their addresses to an entry file.
 * @param entryFileName Name of the entry file.
 * @param symbolTable List of symbols.
 */
static void outputEntryFile(const char *entryFileName, List symbolTable)
{
    FILE *entryFile = fopen(entryFileName, "w");
    if (entryFile)
    {
        outputEntry(entryFile, symbolTable);
        fclose(entryFile);
    }
}

/**
 * Processes external symbols and outputs their names and call addresses to a file.
 * @param file Output file.
 * @param externName Name of the external symbol.
 * @param callAddressesses List of call addresses.
 */

static void processExtern(FILE *file, const char *externName, List callAddressesses)
{
    void *const *start;
    void *const *end;

    for (start = listGetBegin(callAddressesses), end = listGetEnd(callAddressesses); start <= end; start++)
    {
        if (*start)
        {
            fprintf(file, "%s\t%u\n", externName, *((unsigned int *)(*start)));
        }
    }
}

/**
 * Outputs external symbols and their call addresses to an extern file.
 * @param externFileName Name of the extern file.
 * @param external_call_list List of external symbols.
 */

static void outputExtern(const char *externFileName, List external_call_list)
{
    FILE *externFile = fopen(externFileName, "w");
    if (externFile)
    {
        void *const *start;
        void *const *end;

        for (start = listGetBegin(external_call_list), end = listGetEnd(external_call_list); start <= end; start++)
        {
            if (*start)
            {
                const char *externName = getExternName((const struct ExternalInvocation *)(*start));
                List callAddressesses = getCallAddressesses((const struct ExternalInvocation *)(*start));
                processExtern(externFile, externName, callAddressesses);
            }
        }
        fclose(externFile);
    }
}

/**
 * Prints memory data in base64 format.
 * @param outputFile Output file.
 * @param value Memory value.
 */

static void printCharsMemory(FILE *outputFile, unsigned int value)
{
    const char *const base64Chars = BASE64;
    unsigned int mostBits = (value >> 6) & 0x3F;
    unsigned int lessBits = (value & 0x3F);
    fprintf(outputFile, "%c%c\n", base64Chars[mostBits], base64Chars[lessBits]);
}

/**
 * Outputs memory data in base64 format to a file.
 * @param outputFile Output file.
 * @param data List of memory data.
 */

static void outputMemoryData(FILE *outputFile, const List data)
{
    void *const *dataStart;
    void *const *dataEnd;

    for (dataStart = listGetBegin(data), dataEnd = listGetEnd(data); dataStart <= dataEnd; dataStart++)
    {
        if (*dataStart)
        {
            printCharsMemory(outputFile, *(unsigned int *)(*dataStart));
        }
    }
}

/**
 * Generates output files for the assembler: entry file, extern file, and object file.
 * @param name1 Base file name.
 * @param obj Code file object containing code and data, symbols, and external references.
 */

void output(const char *name1, struct CodeFile *obj)
{
    char *entryFilename = NULL;
    char *ext_filename = NULL;
    char *obFileName = NULL;
    FILE *obFile = NULL;

    if (!obj)
    {
        return;
    }

    if (getCodeFileEntriesNumber(obj) >= 1)
    {
        entryFilename = getFileName(name1, ENTEXT);
        if (entryFilename)
        {
            outputEntryFile(entryFilename, getCodeFileSymbolTable(obj));
            free(entryFilename);
        }
    }

    if (listGetItemCount(getCodeFileExternsVec(obj)) >= 1)
    {
        ext_filename = getFileName(name1, EXTEXT);
        if (ext_filename)
        {
            outputExtern(ext_filename, getCodeFileExternsVec(obj));
            free(ext_filename);
        }
    }

    obFileName = getFileName(name1, OBEXT);
    if (obFileName)
    {
        obFile = fopen(obFileName, "w");
        if (obFile)
        {
            fprintf(obFile, "%lu %lu\n", listGetItemCount(getCodeFileCode(obj)), listGetItemCount(getCodeFileData(obj)));
            outputMemoryData(obFile, getCodeFileCode(obj));
            outputMemoryData(obFile, getCodeFileData(obj));
            fclose(obFile);
        }
        free(obFileName);
    }
}