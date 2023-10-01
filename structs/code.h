#ifndef CODE_FILE_H
#define CODE_FILE_H
#include "../data_structure/list.h"
#include "../data_structure/tree.h"

typedef struct CodeFile CodeFile;

List getCodeFileCode(const struct CodeFile *codeFile);
List getCodeFileData(const struct CodeFile *codeFile);
List getCodeFileSymbolTable(const struct CodeFile *codeFile);
WordTree getCodeFileSymbolCheck(const struct CodeFile *codeFile);
List getCodeFileExternsVec(const struct CodeFile *codeFile);
int getCodeFileEntriesNumber(const struct CodeFile *codeFile);

void setCodeFileCode(struct CodeFile *codeFile, List code);
void setCodeFileData(struct CodeFile *codeFile, List data);
void setCodeFileSymbolTable(struct CodeFile *codeFile, List symbolTable);
void setCodeFileSymbolCheck(struct CodeFile *codeFile, WordTree symbolCheck);
void setCodeFileExternsVec(struct CodeFile *codeFile, List externsVec);
void setCodeFileEntriesNumber(struct CodeFile *codeFile, int entriesNumber);
CodeFile *newCodeFile();
void deallocCodeFile(CodeFile *codeFile);

#endif
