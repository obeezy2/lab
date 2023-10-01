#ifndef MISSINGSYM_H
#define MISSINGSYM_H
#include <stdio.h>

#include <stdint.h>

typedef struct missingSym missingSym;

struct missingSym *missingSymCreate();
void missingSymSetSymbolName(struct missingSym *missingSymbol, const char *symbolName);
void missingSymSetWord(struct missingSym *missingSymbol, unsigned int *word);
void missingSymSetCurrLine(struct missingSym *missingSymbol, int currLine);
void missingSymSetCallAddressess(struct missingSym *missingSymbol, unsigned int callAddressess);
const char *missingSymGetSymbolName(const struct missingSym *missingSymbol);
unsigned int *missingSymGetWord(const struct missingSym *missingSymbol);
int missingSymGetCurrLine(const struct missingSym *missingSymbol);
unsigned int missingSymGetCallAddressess(const struct missingSym *missingSymbol);
void missingSymDestroy(struct missingSym *missingSymbol);
void *missingSymbolConstructor(const void *copy);
void missingSymbolDestructor(void *item);
#endif
