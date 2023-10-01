
#include "symbol.h"
#include "stdio.h"
#include "string.h"
#include <stdlib.h>

#define MAXLABEL 31
struct symbol
{
    enum
    {
        symExtern,
        symEntry,
        symCode,
        symData,
        symEntryCode,
        symEntryData
    } type;
    unsigned int adr;
    char name[MAXLABEL + 1];
    unsigned int declaredLine;
};
/*New symbol intitialization and destroyer*/
struct symbol *symbolCreate()
{
    return calloc(1, sizeof(struct symbol));
}
void symbolDestroy(struct symbol *symbol)
{
    free(symbol);
}
/*<---------------Getters and setters----------------->*/

void setSymbolType(struct symbol *symbolVar, int type)
{
    if (symbolVar != NULL)
    {
        symbolVar->type = type;
    }
}

void setSymbolAdr(struct symbol *symbolVar, unsigned int adr)
{
    if (symbolVar != NULL)
    {
        symbolVar->adr = adr;
    }
}

void setSymbolName(struct symbol *symbolVar, const char *name)
{
    if (symbolVar != NULL && name != NULL)
    {
        strncpy(symbolVar->name, name, MAXLABEL);
        symbolVar->name[MAXLABEL] = '\0';
    }
}

void setSymbolDeclaredLine(struct symbol *symbolVar, unsigned int declaredLine)
{
    if (symbolVar != NULL)
    {
        symbolVar->declaredLine = declaredLine;
    }
}

int getSymbolType(const struct symbol *symbolVar)
{
    return (symbolVar != NULL) ? symbolVar->type : -1;
}

unsigned int getSymbolAdr(const struct symbol *symbolVar)
{
    return (symbolVar != NULL) ? symbolVar->adr : 0;
}

const char *getSymbolName(const struct symbol *symbolVar)
{
    return (symbolVar != NULL) ? symbolVar->name : NULL;
}

unsigned int getSymbolDeclaredLine(const struct symbol *symbolVar)
{
    return (symbolVar != NULL) ? symbolVar->declaredLine : 0;
}
int getSymExternType(void)
{
    return symExtern;
}

int getSymEntryType(void)
{
    return symEntry;
}

int getSymCodeType(void)
{
    return symCode;
}

int getSymDataType(void)
{
    return symData;
}

int getSymEntryCodeType(void)
{
    return symEntryCode;
}

int getSymEntryDataType(void)
{
    return symEntryData;
}

size_t sizeOfSymbol(void)
{
    return sizeof(struct symbol);
}
/*----------------------------------------------------------------------------------*/
