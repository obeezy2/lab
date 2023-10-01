#include "assembler.h"
#include "../lexicalAnalysis/lexicalAnalysis.h"
#include "stdio.h"
#include "../data_structure/tree.h"
#include "../data_structure/list.h"
#include "../preAssembly/preAssembler.h"
#include "stdlib.h"
#define maxLineCapacity 81
#include "string.h"
#define baseAddress 100
#include "../output/output.h"
#include "commonFunctions.h"
#include "../structs/code.h"
#include "../structs/missingSymbol.h"
#include "../structs/symbol.h"

#define RED "\x1B[31m"
#define RESET "\x1B[0m"
#define MAG "\x1B[35m"

/**
 * This function performs a second pass over the assembly source file to
 * resolve missing symbols and finalize the code and data address values.
 *
 * @param file Pointer to the input assembly file being processed.
 * @param o Pointer to a structure holding the generated code and data,
 *   as well as symbol and extern tables.
 * @param amName Assembly file name (not used in the function).
 * @param missingSymbolTable List of symbols referenced in the assembly code
 *   that haven't been resolved in the first pass.
 *
 * @return Returns 1 if the second pass was successful, and 0 if errors were found.
 */
int secondPass(FILE *file, struct CodeFile *o, const char *amName, List missingSymbolTable)
{
    void *const *begin;
    void *const *end;
    struct symbol *find = NULL;
    int errorCode = 1;

    for (begin = listGetBegin(getCodeFileSymbolTable(o)), end = listGetEnd(getCodeFileSymbolTable(o)); begin <= end; begin++)
    {
        if (*begin)
        {
            struct symbol *symVar = (struct symbol *)(*begin);
            if (getSymbolType(symVar) == getSymEntryType())
            {
                errorCode = 0;
            }
            else if (getSymbolType(symVar) >= getSymEntryCodeType())
            {
                int currentEntriesNumber = getCodeFileEntriesNumber(o);
                setCodeFileEntriesNumber(o, currentEntriesNumber + 1);
            }
            if (getSymbolType(symVar) == getSymDataType() || getSymbolType(symVar) == getSymEntryDataType())
            {
                unsigned int newAdr = getSymbolAdr(symVar) + listGetItemCount(getCodeFileCode(o)) + baseAddress;
                setSymbolAdr(symVar, newAdr);
            }
        }
    }

    for (begin = listGetBegin(missingSymbolTable), end = listGetEnd(missingSymbolTable); begin <= end; begin++)
    {
        if (*begin)
        {
            struct missingSym *missingSymVar = (struct missingSym *)(*begin);
            find = checkIfExists(getCodeFileSymbolCheck(o), missingSymGetSymbolName(missingSymVar));
            if (find && getSymbolType(find) != getSymEntryType())
            {
                unsigned int *wordPtr = missingSymGetWord(missingSymVar);
                *wordPtr = getSymbolAdr(find) << 2;
                if (getSymbolType(find) == getSymExternType())
                {
                    *wordPtr |= 1;
                    addExtern(getCodeFileExternsVec(o), getSymbolName(find), missingSymGetCallAddressess(missingSymVar));
                }
                else
                {
                    *wordPtr |= 2;
                }
            }
            else
            {
                errorCode = 0;
            }
        }
    }

    listDealloc(&missingSymbolTable);
    return errorCode;
}
