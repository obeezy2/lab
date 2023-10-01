#ifndef SYMBOL_H

#define SYMBOL_H
#include "../data_structure/list.h"
#include "../data_structure/tree.h"
#include <stdio.h>

typedef struct symbol symbol;

void setSymbolType(struct symbol *symbolVar, int type);
void setSymbolAdr(struct symbol *symbolVar, unsigned int adr);
void setSymbolName(struct symbol *symbolVar, const char *name);
void setSymbolDeclaredLine(struct symbol *symbolVar, unsigned int declaredLine);

int getSymbolType(const struct symbol *symbolVar);
unsigned int getSymbolAdr(const struct symbol *symbolVar);
const char *getSymbolName(const struct symbol *symbolVar);
unsigned int getSymbolDeclaredLine(const struct symbol *symbolVar);
int getSymExternType(void);
int getSymEntryType(void);
int getSymCodeType(void);
int getSymDataType(void);
int getSymEntryCodeType(void);
int getSymEntryDataType(void);
size_t sizeOfSymbol(void);
struct symbol *symbolCreate();
void symbolDestroy(struct symbol *symbol);

#endif