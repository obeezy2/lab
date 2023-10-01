#ifndef _lexicalAnalysis_H_
#define _lexicalAnalysis_H_
#define MAXDATA 90
#define MAXLABEL 31

#include "stddef.h"
typedef struct TokenTree TokenTree;
struct TokenTree *createTokenTree();

const char *getTokenTreeLabel(const TokenTree *tree);
void setTokenTreeLabel(TokenTree *tree, const char *label);
const char *getTokenTreeErrorMessage(const TokenTree *tree);
void setTokenTreeErrorMessage(TokenTree *tree, const char *errorMessage);
int getTokenTreeOptions(const TokenTree *tree);
void setTokenTreeOptions(TokenTree *tree, int options);
int getTokenTreeDirectiveOptions(const TokenTree *tree);
void setTokenTreeDirectiveOptions(TokenTree *tree, int directiveOptions);
void destoryTokenTree(struct TokenTree *TokenTree);

const char *getTokenTreeDirectiveOperandsString(const TokenTree *tree);
void setTokenTreeDirectiveOperandsString(TokenTree *tree, const char *str);
const char *getTokenTreeDirectiveOperandsLabel(const TokenTree *tree);
void setTokenTreeDirectiveOperandsLabel(TokenTree *tree, const char *label);

int getTokenTreeDirectiveOperandsDataData(const TokenTree *tree, size_t index);
void setTokenTreeDirectiveOperandsDataData(TokenTree *tree, size_t index, int value);

int getTokenTreeDirectiveOperandsDataCount(const TokenTree *tree);
void setTokenTreeDirectiveOperandsDataCount(TokenTree *tree, int count);

int getTokenTreeInstructionType(const TokenTree *tree);
void setTokenTreeInstructionType(TokenTree *tree, int instructionType);

int getTokenTreeInstructionsOperandsOptions(const TokenTree *tree, size_t index);
void setTokenTreeInstructionsOperandsOptions(TokenTree *tree, size_t index, int option);

int getTokenTreeInstructionsOperandsNum(const TokenTree *tree, size_t index);
void setTokenTreeInstructionsOperandsNum(TokenTree *tree, size_t index, int num);

int getTokenTreeInstructionsOperandsReg(const TokenTree *tree, size_t index);
void setTokenTreeInstructionsOperandsReg(TokenTree *tree, size_t index, int reg);

const char *getTokenTreeInstructionsOperandsLabelName(const TokenTree *tree, size_t index);
void setTokenTreeInstructionsOperandsLabelName(TokenTree *tree, size_t index, const char *labelName);

int getTypeMov(void);
int getTypeCmp(void);
int getTypeAdd(void);
int getTypeSub(void);
int getTypeLea(void);
int getTypeNot(void);
int getTypeClr(void);
int getTypeInc(void);
int getTypeDec(void);
int getTypeJmp(void);
int getTypeBne(void);
int getTypeRed(void);
int getTypePrn(void);
int getTypeJsr(void);
int getTypeRts(void);
int getTypeStop(void);
int getDirectiveData(void);
int getDirectiveExtern(void);
int getDirectiveEntry(void);
int getDirectiveString(void);
int getInstruction(void);
int getDirective(void);
int getOperandRegisterNumber(void);
int getOperandLabel(void);
int getOperandNumber(void);
int getOperandNull(void);
TokenTree *getTree(char *line);
void treeDestroy(TokenTree *myTree);

#endif