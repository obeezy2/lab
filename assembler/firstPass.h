#ifndef _FIRSTPASS_H
#define _FIRSTPASS_H
#include "../lexicalAnalysis/lexicalAnalysis.h"
#include "stdio.h"
#include "../data_structure/list.h"
#include "../structs/code.h"
#include "../structs/external.h"
#include "../structs/missingSymbol.h"
#include "../structs/symbol.h"
#include "../structs/code.h"
#include "firstPass.h"
#include "commonFunctions.h"
#include "../output/output.h"
int firstPass(FILE *file, struct CodeFile *o, const char *amName, List missingSymbolTable);

#endif