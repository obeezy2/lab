
#include "../data_structure/list.h"
#include "../data_structure/tree.h"
#include "code.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "external.h"
#include "symbol.h"
struct CodeFile
{
    List code;
    List data;
    List symbolTable;
    WordTree symbolCheck;
    List externsVec;
    int entriesNumber;
};

/*<------Getters and setters for the CodeFile Struct* ----->*/
List getCodeFileCode(const struct CodeFile *codeFile)
{
    return codeFile->code;
}

List getCodeFileData(const struct CodeFile *codeFile)
{
    return codeFile->data;
}

List getCodeFileSymbolTable(const struct CodeFile *codeFile)
{
    return codeFile->symbolTable;
}

WordTree getCodeFileSymbolCheck(const struct CodeFile *codeFile)
{
    return codeFile->symbolCheck;
}

List getCodeFileExternsVec(const struct CodeFile *codeFile)
{
    return codeFile->externsVec;
}

int getCodeFileEntriesNumber(const struct CodeFile *codeFile)
{
    return codeFile->entriesNumber;
}

void setCodeFileCode(struct CodeFile *codeFile, List code)
{
    codeFile->code = code;
}

void setCodeFileData(struct CodeFile *codeFile, List data)
{
    codeFile->data = data;
}

void setCodeFileSymbolTable(struct CodeFile *codeFile, List symbolTable)
{
    codeFile->symbolTable = symbolTable;
}

void setCodeFileSymbolCheck(struct CodeFile *codeFile, WordTree symbolCheck)
{
    codeFile->symbolCheck = symbolCheck;
}

void setCodeFileExternsVec(struct CodeFile *codeFile, List externsVec)
{
    codeFile->externsVec = externsVec;
}

void setCodeFileEntriesNumber(struct CodeFile *codeFile, int entriesNumber)
{
    codeFile->entriesNumber = entriesNumber;
}
/*-------------------------------------------------------------------*/

/*Constructors/Destructors*/
static void *symbolConstructor(const void *copy)
{
    return memcpy(malloc(sizeOfSymbol()), copy, sizeOfSymbol());
}
static void symbolDestructor(void *item)
{
    free(item);
}

static void *wordConstructor(const void *copy)
{

    return memcpy(malloc(sizeof(unsigned int)), copy, sizeof(unsigned int));
}

static void wordDestructor(void *item)
{
    free(item);
}

/*Creating a new CodeFile obj*/
static struct CodeFile newObj()
{
    struct CodeFile assembledFile = {0};
    assembledFile.code = createDynamicList(wordConstructor, wordDestructor);
    assembledFile.data = createDynamicList(wordConstructor, wordDestructor);
    assembledFile.symbolTable = createDynamicList(symbolConstructor, symbolDestructor);
    assembledFile.externsVec = createDynamicList(externConstructor, externDestructor);
    assembledFile.symbolCheck = wordT();
    return assembledFile;
}
/*Destroying the object sections*/
static void objectDealloc(struct CodeFile *obj)
{
    listDealloc(&obj->code);
    listDealloc(&obj->data);
    listDealloc(&obj->symbolTable);
    listDealloc(&obj->externsVec);
    treeDealloc(&obj->symbolCheck);
}

/*Codefile initializer*/
CodeFile *newCodeFile()
{
    CodeFile *codeFile = malloc(sizeof(CodeFile));
    *codeFile = newObj();
    return codeFile;
}

/*Deallocating the CodeFile object*/
void deallocCodeFile(CodeFile *codeFile)
{
    if (codeFile)
    {
        objectDealloc(codeFile);
        free(codeFile);
    }
}