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
#include "../structs/external.h"
#include "../structs/missingSymbol.h"
#include "../structs/symbol.h"
#include "../structs/code.h"
#include "firstPass.h"
#include "commonFunctions.h"
#include "../output/output.h"
#define RED "\x1B[31m"
#define RESET "\x1B[0m"
#define MAG "\x1B[35m"

/* handleInstructionLabel:
 * Handles a label associated with an instruction.
 * Checks if the label already exists and ensures correct behavior based on the symbol type.
 * Args:
 * - label: The label string associated with an instruction.
 * - scopeSym: Symbol struct for the current scope.
 * - find: Symbol struct for the found symbol in the symbol table.
 * - o: CodeFile struct containing the generated code, data, and symbol tables.
 * - lineCounter: The current line number in the input file.
 * - errorCode: Pointer to an integer to update if an error occurs.
 */

static void handleInstructionLabel(const char *label, struct symbol *scopeSym, struct symbol *find, struct CodeFile *o, unsigned int lineCounter, int *errorCode)
{
    if (find)
    {
        if (getSymbolType(find) != getSymEntryType())
        {
            fprintf(stderr, RED "ERROR: label '%s' was already defined in line '%d'\n" RESET, getSymbolName(find), lineCounter);
            *errorCode = 0;
        }
        else
        {
            setSymbolType(find, getSymEntryCodeType());
            setSymbolAdr(find, (unsigned int)listGetItemCount(getCodeFileCode(o)) + baseAddress);
        }
    }
    else
    {
        setSymbolType(scopeSym, getSymCodeType());
        setSymbolAdr(scopeSym, (unsigned int)listGetItemCount(getCodeFileCode(o)) + baseAddress);
        setSymbolDeclaredLine(scopeSym, lineCounter);
        insertWord(getCodeFileSymbolCheck(o), getSymbolName(scopeSym), listInsertItem(getCodeFileSymbolTable(o), scopeSym));
    }
}
/* handleDirectiveLabel:
 * Handles a label associated with a directive.
 * Checks if the label already exists and ensures correct behavior based on the symbol type.
 * Args:
 * - label: The label string associated with a directive.
 * - myTree: The token tree representing the parsed line of assembly code.
 * - scopeSym: Symbol struct for the current scope.
 * - find: Symbol struct for the found symbol in the symbol table.
 * - o: CodeFile struct containing the generated code, data, and symbol tables.
 * - lineCounter: The current line number in the input file.
 * - errorCode: Pointer to an integer to update if an error occurs.
 */

static void handleDirectiveLabel(const char *label, TokenTree *myTree, struct symbol *scopeSym, struct symbol *find, struct CodeFile *o, unsigned int lineCounter, int *errorCode)
{
    if (getTokenTreeDirectiveOptions(myTree) <= getDirectiveEntry())
    {
        fprintf(stderr, MAG "WARNING: neglecting label for line '%d' " RESET, lineCounter);
    }
    else
    {
        if (find)
        {
            if (getTokenTreeDirectiveOptions(myTree) >= getDirectiveString())
            {
                if (getSymbolType(find) != getSymEntryType())
                {
                    fprintf(stderr, RED "ERROR: label: '%s' - was already defined in line '%d'" RESET, getSymbolName(find), lineCounter);
                    *errorCode = 0;
                }
                else
                {
                    setSymbolType(find, getSymEntryDataType());
                    setSymbolAdr(find, listGetItemCount(getCodeFileData(o)));
                    setSymbolDeclaredLine(find, lineCounter);
                }
            }
        }
        else
        {
            if (getTokenTreeDirectiveOptions(myTree) >= getDirectiveString())
            {
                setSymbolType(scopeSym, getSymDataType());
                setSymbolAdr(scopeSym, (unsigned int)listGetItemCount(getCodeFileData(o)));
                setSymbolDeclaredLine(scopeSym, lineCounter);

                insertWord(getCodeFileSymbolCheck(o), getSymbolName(scopeSym), listInsertItem(getCodeFileSymbolTable(o), scopeSym));
            }
            else
            {
                fprintf(stderr, MAG "WARNING: unused label at line '%d' " RESET, lineCounter);
            }
        }
    }
}

/* handleLabelProcessing:
 * Processes a label found in a line of assembly code.
 * Calls the appropriate label handler based on the type of the line (instruction or directive).
 * Args:
 * - label: The label string found in the line of assembly code.
 * - myTree: The token tree representing the parsed line of assembly code.
 * - lineCounter: The current line number in the input file.
 * - scopeSym: Symbol struct for the current scope.
 * - find: Symbol struct for the found symbol in the symbol table.
 * - o: CodeFile struct containing the generated code, data, and symbol tables.
 * - errorCode: Pointer to an integer to update if an error occurs.
 */

static void handleLabelProcessing(const char *label, TokenTree *myTree, unsigned int lineCounter, struct symbol *scopeSym, struct symbol *find, struct CodeFile *o, int *errorCode)
{
    if (label[0] != '\0')
    {
        setSymbolName(scopeSym, label);
        find = checkIfExists(getCodeFileSymbolCheck(o), label);

        if (getTokenTreeOptions(myTree) == getInstruction())
        {
            handleInstructionLabel(label, scopeSym, find, o, lineCounter, errorCode);
        }
        else
        {
            handleDirectiveLabel(label, myTree, scopeSym, find, o, lineCounter, errorCode);
        }
    }
}
/* handleInstructionProcessing:
 * Processes an instruction found in a line of assembly code.
 * Handles the operands and adds the generated code to the code table.
 * Args:
 * - myTree: The token tree representing the parsed line of assembly code.
 * - o: CodeFile struct containing the generated code, data, and symbol tables.
 * - lineCounter: The current line number in the input file.
 * - missingSymbol: Struct for missing symbols that cannot be resolved in the first pass.
 * - missingSymbolTable: List of missing symbols.
 * - word: Pointer to the generated code for the instruction.
 * - find: Symbol struct for the found symbol in the symbol table.
 * - externAddress: Pointer to the address for the extern symbol.
 */

static void handleInstructionProcessing(TokenTree *myTree, struct CodeFile *o, unsigned int lineCounter, missingSym *missingSymbol, List missingSymbolTable, unsigned int *word, struct symbol *find, unsigned int *externAddress)
{
    int i;
    unsigned int *insertedWord;

    *word = getTokenTreeInstructionsOperandsOptions(myTree, 1) << 2;
    *word |= getTokenTreeInstructionsOperandsOptions(myTree, 0) << 9;
    *word |= getTokenTreeInstructionType(myTree) << 5;
    listInsertItem(getCodeFileCode(o), word);

    if (getTokenTreeInstructionType(myTree) >= getTypeRts())
    {
    }
    else
    {
        if (getTokenTreeInstructionsOperandsOptions(myTree, 1) == getOperandRegisterNumber() &&
            getTokenTreeInstructionsOperandsOptions(myTree, 0) == getOperandRegisterNumber())
        {
            *word = getTokenTreeInstructionsOperandsReg(myTree, 1) << 2;
            *word |= getTokenTreeInstructionsOperandsReg(myTree, 0) << 7;
            listInsertItem(getCodeFileCode(o), word);
        }
        else
        {
            for (i = 0; i < 2; i++)
            {
                int operandOptions = getTokenTreeInstructionsOperandsOptions(myTree, i);
                if (operandOptions == getOperandRegisterNumber())
                {
                    *word = getTokenTreeInstructionsOperandsReg(myTree, i) << (7 - (i * 5));
                    listInsertItem(getCodeFileCode(o), word);
                }
                else if (operandOptions == getOperandLabel())
                {
                    find = checkIfExists(getCodeFileSymbolCheck(o), getTokenTreeInstructionsOperandsLabelName(myTree, i));
                    if (find && getSymbolType(find) != getSymEntryType())
                    {
                        *word = getSymbolAdr(find) << 2;
                        if (getSymbolType(find) == getSymExternType())
                        {
                            *word |= 1;
                            *externAddress = listGetItemCount(getCodeFileCode(o)) + baseAddress;
                            addExtern(getCodeFileExternsVec(o), getSymbolName(find), *externAddress);
                        }
                        else
                        {
                            *word |= 2;
                        }
                    }
                    insertedWord = listInsertItem(getCodeFileCode(o), word);
                    if (!find || (find && getSymbolType(find) == getSymEntryType()))
                    {
                        missingSymSetSymbolName(missingSymbol, getTokenTreeInstructionsOperandsLabelName(myTree, i));
                        missingSymSetWord(missingSymbol, insertedWord);
                        missingSymSetCurrLine(missingSymbol, lineCounter);
                        missingSymSetCallAddressess(missingSymbol, *externAddress = listGetItemCount(getCodeFileCode(o)) + baseAddress - 1);
                        listInsertItem(missingSymbolTable, missingSymbol);
                    }
                }
                else if (operandOptions == getOperandNumber())
                {
                    *word = getTokenTreeInstructionsOperandsNum(myTree, i) << 2;
                    listInsertItem(getCodeFileCode(o), word);
                }
                else if (operandOptions == getOperandNull())
                {
                }
            }
        }
    }
}

/* handleDirective:
 * Processes a directive found in a line of assembly code.
 * Handles the data and adds it to the data table.
 * Args:
 * - myTree: The token tree representing the parsed line of assembly code.
 * - o: CodeFile struct containing the generated code, data, and symbol tables.
 * - lineCounter: The current line number in the input file.
 * - scopeSym: Symbol struct for the current scope.
 * - find: Symbol struct for the found symbol in the symbol table.
 * - errorCode: Pointer to an integer to update if an error occurs.
 */

static void handleDirective(TokenTree *myTree, struct CodeFile *o, int lineCounter, struct symbol *scopeSym, struct symbol *find, int *errorCode)
{
    int i;
    const char *datmyTreering;
    unsigned int word = 0;

    int directiveOptions = getTokenTreeDirectiveOptions(myTree);
    const char *label = getTokenTreeLabel(myTree);
    if (directiveOptions <= getDirectiveData() && directiveOptions >= getDirectiveString() && label[0] == '\0')
    {
        fprintf(stderr, MAG "WARNING: neglecting %s because it has no label\n" RESET, directiveOptions == getDirectiveData() ? ".data" : ".string");
    }
    else
    {
        if (directiveOptions == getDirectiveString())
        {
            for (i = 0, datmyTreering = getTokenTreeDirectiveOperandsString(myTree); *datmyTreering; datmyTreering++)
            {
                word = *datmyTreering;
                listInsertItem(getCodeFileData(o), &word);
            }
            word = 0;
            listInsertItem(getCodeFileData(o), &word);
        }
        else if (directiveOptions == getDirectiveData())
        {
            for (i = 0; i < getTokenTreeDirectiveOperandsDataCount(myTree); i++)
            {
                unsigned int dataItem = getTokenTreeDirectiveOperandsDataData(myTree, i);
                listInsertItem(getCodeFileData(o), &dataItem);
            }
        }
        else if (directiveOptions == getDirectiveExtern() || directiveOptions == getDirectiveEntry())
        {
            find = checkIfExists(getCodeFileSymbolCheck(o), getTokenTreeDirectiveOperandsLabel(myTree));
            if (find)
            {
                if (directiveOptions == getDirectiveEntry())
                {
                    if (getSymbolType(find) == getSymEntryType() || getSymbolType(find) >= getSymEntryCodeType() || getSymbolType(find) >= getSymEntryDataType())
                    {
                        fprintf(stderr, MAG "WARNING: label '%s' was already defined in another line\n" RESET, getSymbolName(find));
                    }
                    else if (getSymbolType(find) == getSymExternType())
                    {
                        fprintf(stderr, RED "ERROR: label '%s' was already defined in another line\n" RESET, getSymbolName(find));
                        errorCode = 0;
                    }
                    else
                    {
                        setSymbolType(find, getSymbolType(find) == getSymCodeType() ? getSymEntryCodeType() : getSymEntryDataType());
                    }
                }
                else
                {
                    if (getSymbolType(find) == getSymExternType())
                    {
                        fprintf(stderr, MAG "WARNING: label '%s' was already defined in another line\n" RESET, getSymbolName(find));
                    }
                    else
                    {
                        fprintf(stderr, RED "ERROR: label '%s' was already defined in another line\n" RESET, getSymbolName(find));
                        errorCode = 0;
                    }
                }
            }
            else
            {
                setSymbolName(scopeSym, getTokenTreeDirectiveOperandsLabel(myTree));
                setSymbolType(scopeSym, directiveOptions);
                setSymbolAdr(scopeSym, 0);
                setSymbolDeclaredLine(scopeSym, lineCounter);
                insertWord(getCodeFileSymbolCheck(o), getSymbolName(scopeSym), listInsertItem(getCodeFileSymbolTable(o), scopeSym));
            }
        }
    }
}

/**
 * This function performs the first pass over the assembly source file to
 * tokenize the input, create an initial symbol table, and generate
 * code and data segments. It also identifies and stores unresolved symbols
 * for processing in the second pass.
 *
 * @param file Pointer to the input assembly file being processed.
 * @param o Pointer to a structure holding the generated code and data,
 *   as well as symbol and extern tables.
 * @param amName Assembly file name (not used in the function).
 * @param missingSymbolTable List for storing symbols referenced in the
 *   assembly code that haven't been resolved during the first pass.
 *
 * @return Returns 1 if the first pass was successful, and 0 if errors were found.
 */

int firstPass(FILE *file, struct CodeFile *o, const char *amName, List missingSymbolTable)
{
    char lineContainer[maxLineCapacity] = {0};
    TokenTree *myTree = NULL;
    struct symbol *scopeSym = symbolCreate();
    struct symbol *find = symbolCreate();
    unsigned int externAddress = 0;
    missingSym *missingSymbol = missingSymCreate();
    unsigned int lineCounter = 1;
    unsigned int word = 0;
    int errorCode = 1;
    const char *label;
    const char *errorMessage;
    int options;
    while (fgets(lineContainer, sizeof(lineContainer), file))
    {
        myTree = getTree(lineContainer);
        errorMessage = getTokenTreeErrorMessage(myTree);
        if (errorMessage[0] != '\0')
        {
            fprintf(stderr, RED "ERROR : %s\n" RESET, errorMessage);

            errorCode = 1;
            lineCounter++;
            continue;
        }
        label = getTokenTreeLabel(myTree);
        handleLabelProcessing(label, myTree, lineCounter, scopeSym, find, o, &errorCode);

        options = getTokenTreeOptions(myTree);

        if (options == getInstruction())
        {
            handleInstructionProcessing(myTree, o, lineCounter, missingSymbol, missingSymbolTable, &word, find, &externAddress);
        }
        else if (options == getDirective())
        {
            handleDirective(myTree, o, lineCounter, scopeSym, find, &errorCode);
        }
    }
    missingSymDestroy(missingSymbol);
    symbolDestroy(find);
    symbolDestroy(scopeSym);
    treeDestroy(myTree);
    myTree = NULL;
    lineCounter++;

    return errorCode;
}