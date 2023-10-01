#ifndef _SECONDPASS_H
#define _SECONDPASS_H
#include "stdio.h"
#include "../data_structure/list.h"
#include "../structs/code.h"
int secondPass(FILE *file, struct CodeFile *o, const char *amName, List missingSymbolTable);

#endif