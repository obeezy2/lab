#include "assembler.h"
#include "../lexicalAnalysis/lexicalAnalysis.h"
#include "stdio.h"
#include "../data_structure/tree.h"
#include "../data_structure/list.h"
#include "../preAssembly/preAssembler.h"
#include "stdlib.h"
#include "string.h"
#include "../output/output.h"
#include "firstPass.h"
#include "secondPass.h"
#include "commonFunctions.h"
#include "../structs/code.h"
#include "../structs/missingSymbol.h"

/**
 * This function processes an assembly file by performing a two-pass assembly.
 * It preprocesses the file, runs the first and second passes, and then generates
 * output files if the assembly was successful.
 *
 * @param filename The name of the input assembly file.
 * @param missingSymbolTable List for storing symbols referenced in the
 *   assembly code that haven't been resolved during the first pass.
 *
 * @return Returns 0 if the file was successfully handled, and -1 otherwise.
 */

int handleFile(const char *filename, List missingSymbolTable)
{
    const char *amName;
    FILE *amFile;
    CodeFile *currObj;
    int firstPassResult, secondPassResult;

    amName = preprocess(filename);
    if (!amName)
    {
        return -1;
    }

    amFile = fopen(amName, "r");
    if (!amFile)
    {
        free((void *)amName);
        return -1;
    }

    currObj = newCodeFile();
    firstPassResult = firstPass(amFile, currObj, amName, missingSymbolTable);
    if (firstPassResult == 1)
    {
        fseek(amFile, 0, SEEK_SET);
        secondPassResult = secondPass(amFile, currObj, amName, missingSymbolTable);
        if (secondPassResult == 1)
        {
            output(filename, currObj);
        }
    }

    deallocCodeFile(currObj);

    fclose(amFile);
    free((void *)amName);

    return 0;
}
/**
 * The main function of the assembler. It initializes the missingSymbolTable list,
 * and iterates through all input files, processing each with handleFile.
 *
 * @param fileCount The number of input files.
 * @param fileName An array of strings containing the names of the input files.
 *
 * @return Returns 0 upon successful completion.
 */

int assembler(int fileCount, char **fileName)
{
    List missingSymbolTable;
    int i;

    missingSymbolTable = createDynamicList(missingSymbolConstructor, missingSymbolDestructor);

    for (i = 0; i < fileCount; i++)
    {
        handleFile(fileName[i], missingSymbolTable);
    }

    return 0;
}