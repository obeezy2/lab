#include "missingSymbol.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAXLABEL 30
struct missingSym
{
    char symbolName[MAXLABEL + 1];
    unsigned int *word;
    int currLine;
    unsigned int callAddressess;
};
/*Missing symbol struct initialization*/
struct missingSym *missingSymCreate()
{
    return calloc(1, sizeof(struct missingSym));
}
/*Destroyers and constructors/destructors*/
void missingSymDestroy(struct missingSym *missingSymbol)
{
    free(missingSymbol);
}
void *missingSymbolConstructor(const void *copy)
{

    return memcpy(malloc(sizeof(struct missingSym)), copy, sizeof(struct missingSym));
}
void missingSymbolDestructor(void *item)
{
    free(item);
}

/*<-------------------------Getters and setters------------------------------------->*/
void missingSymSetSymbolName(struct missingSym *missingSymbol, const char *symbolName)
{
    strncpy(missingSymbol->symbolName, symbolName, MAXLABEL);
    missingSymbol->symbolName[MAXLABEL] = '\0';
}

void missingSymSetWord(struct missingSym *missingSymbol, unsigned int *word)
{
    missingSymbol->word = word;
}

void missingSymSetCurrLine(struct missingSym *missingSymbol, int currLine)
{
    missingSymbol->currLine = currLine;
}

void missingSymSetCallAddressess(struct missingSym *missingSymbol, unsigned int callAddressess)
{
    missingSymbol->callAddressess = callAddressess;
}

const char *missingSymGetSymbolName(const struct missingSym *missingSymbol)
{
    return missingSymbol->symbolName;
}

unsigned int *missingSymGetWord(const struct missingSym *missingSymbol)
{
    return missingSymbol->word;
}

int missingSymGetCurrLine(const struct missingSym *missingSymbol)
{
    return missingSymbol->currLine;
}

unsigned int missingSymGetCallAddressess(const struct missingSym *missingSymbol)
{
    return missingSymbol->callAddressess;
}
/*----------------------------------------------------------------------------------*/
