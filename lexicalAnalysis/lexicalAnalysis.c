#include "lexicalAnalysis.h"
#include "limits.h"
#include "stdio.h"
#include "ctype.h"
#include "string.h"
#include "../data_structure/tree.h"
#include "errno.h"
#include "stdlib.h"

#define MAXNUM 511
#define MINNUM -512
#define MAXREG 7
#define MINREG 0
#define SPACECHARS " \f\n\t\r\v"

static void skipSpaces(char **s)
{
    while (**s && isspace(**s))
    {
        (*s)++;
    }
}
typedef struct
{
    char *string;
    char *label;
    int data[MAXDATA];
    int count;
} DirectiveOperands;

typedef struct
{
    int num;
    int reg;
    char *labelName;
} InstructionOperands;

typedef struct
{
    DirectiveOperands operands;
    enum
    {
        directiveExtern,
        directiveEntry,
        directiveString,
        directiveData
    } type;
} Directive;

typedef struct
{
    InstructionOperands operands[2];
    enum
    {
        typeMov,
        typeCmp,
        typeAdd,
        typeSub,
        typeLea,
        typeNot,
        typeClr,
        typeInc,
        typeDec,
        typeJmp,
        typeBne,
        typeRed,
        typePrn,
        typeJsr,
        typeRts,
        typeStop
    } type;
    enum
    {
        OperandOptions
    } operandTypes[2];
} Instruction;

struct TokenTree
{
    char errorMessage[150];
    char label[MAXLABEL + 1];
    enum
    {
        TOKEN_INSTRUCTION,
        TOKEN_DIRECTIVE
    } tokenType;
    union
    {
        Directive directive;
        Instruction instruction;
    } tokenData;
};

static int isTree = 0;
WordTree searchForInstruction = NULL;
WordTree searchForDirective = NULL;

enum OperandOptions
{
    OP_OPTION_NULL = 0,
    OP_OPTION_IMMEDIATE = 1,
    OP_OPTION_LABEL = 3,
    OP_OPTION_REG_NUMBER = 5,
    OP_OPTION_OVERFLOW = 8,
    OP_OPTION_MISSING = 16
};

static struct instruction_mapping
{
    const char *instructionName; /* The name of the instruction */
    int key;                     /* A unique key to identify the instruction */
    enum OperandOptions sourceOpOptions;
    enum OperandOptions destOpOptions;

} instruction_mapping[16] = {
    {"mov", typeMov, OP_OPTION_IMMEDIATE | OP_OPTION_LABEL | OP_OPTION_REG_NUMBER, OP_OPTION_LABEL | OP_OPTION_REG_NUMBER},
    {"cmp", typeCmp, OP_OPTION_IMMEDIATE | OP_OPTION_LABEL | OP_OPTION_REG_NUMBER, OP_OPTION_IMMEDIATE | OP_OPTION_LABEL | OP_OPTION_REG_NUMBER},
    {"add", typeAdd, OP_OPTION_IMMEDIATE | OP_OPTION_LABEL | OP_OPTION_REG_NUMBER, OP_OPTION_LABEL | OP_OPTION_REG_NUMBER},
    {"sub", typeSub, OP_OPTION_IMMEDIATE | OP_OPTION_LABEL | OP_OPTION_REG_NUMBER, OP_OPTION_LABEL | OP_OPTION_REG_NUMBER},
    {"lea", typeLea, OP_OPTION_LABEL, OP_OPTION_LABEL | OP_OPTION_REG_NUMBER},

    {"not", typeNot, OP_OPTION_NULL, OP_OPTION_LABEL | OP_OPTION_REG_NUMBER},
    {"clr", typeClr, OP_OPTION_NULL, OP_OPTION_LABEL | OP_OPTION_REG_NUMBER},
    {"inc", typeInc, OP_OPTION_NULL, OP_OPTION_LABEL | OP_OPTION_REG_NUMBER},
    {"dec", typeDec, OP_OPTION_NULL, OP_OPTION_LABEL | OP_OPTION_REG_NUMBER},
    {"jmp", typeJmp, OP_OPTION_NULL, OP_OPTION_LABEL | OP_OPTION_REG_NUMBER},
    {"bne", typeBne, OP_OPTION_NULL, OP_OPTION_LABEL | OP_OPTION_REG_NUMBER},
    {"red", typeRed, OP_OPTION_NULL, OP_OPTION_LABEL | OP_OPTION_REG_NUMBER},
    {"prn", typePrn, OP_OPTION_NULL, OP_OPTION_IMMEDIATE | OP_OPTION_LABEL | OP_OPTION_REG_NUMBER},
    {"jsr", typeJsr, OP_OPTION_NULL, OP_OPTION_LABEL | OP_OPTION_REG_NUMBER},
    {"rts", typeRts, OP_OPTION_NULL, OP_OPTION_NULL},
    {"stop", typeStop, OP_OPTION_NULL, OP_OPTION_NULL},
};

/* Structure that holds the mapping between an assembly directive and its key */

static struct directive_mapping
{
    const char *directiveName; /* The name of the directive */
    int key;                   /* A unique key to identify the directive */

} directive_mapping[4] = {
    {"data", directiveData},
    {"string", directiveString},
    {"extern", directiveExtern},
    {"entry", directiveEntry}};

/* Initializes the trees used for searching for instructions and directives */

static void lexer_wordT_init()
{
    int i;
    searchForInstruction = wordT();
    searchForDirective = wordT();
    for (i = 0; i < 16; i++)
    {
        insertWord(searchForInstruction, instruction_mapping[i].instructionName, &instruction_mapping[i]);
    }
    for (i = 0; i < 4; i++)
    {
        insertWord(searchForDirective, directive_mapping[i].directiveName, &directive_mapping[i]);
    }
    isTree = 1;
}

/* Enum representing the type of label */

enum labelType
{
    correctLabel,
    labelStartsWithoutChar,
    labelContainsNumbers,
    labelTooLong
};
/* Reports errors related to directives by formatting the error message and storing it in the token tree */

static void reportDirError(TokenTree *myTree, const char *format, const char *arg1, const char *arg2)
{
    sprintf(myTree->errorMessage, format, arg1, arg2);
}

/* Reports errors related to instructions by formatting the error message and storing it in the token tree */

static void reportInstError(TokenTree *myTree, const char *format, const char *arg)
{
    sprintf(myTree->errorMessage, format, arg);
}
static enum labelType checkLabel(const char *label)
{

    int characterCount = 0;
    if (!isalpha(*label))
    {
        return labelStartsWithoutChar;
    }
    label++;
    while (*label && isalnum(*label))
    {
        characterCount++;
        label++;
    };
    if (*label != '\0')
    {
        return labelContainsNumbers;
    }
    if (characterCount > MAXLABEL)
    {
        return labelTooLong;
    }
    return correctLabel;
}

/*
 * This function parses a string that represents a number, converts it to a long integer,
 * and performs several validation checks to ensure that the number is valid.
 */

static int parseNumber(const char *numbersString, char **endPointer, long *num, long min, long max)
{
    char *end;
    *num = strtol(numbersString, &end, 10);
    errno = 0;

    while (isspace(*end))
        end++;
    if (*end != '\0')
    {
        return -1;
    }
    if (errno == ERANGE)
    {
        return -2;
    }
    if (*num > max || *num < min)
    {
        return -3;
    }
    if (endPointer)
        *endPointer = end;
    return 0;
}

/* Parses a string representing an operand and determines its type.
 */

enum OperandOptions parseOperands(char *operandString, char **label, int *constNum, int *reg)
{
    char *temp;
    long num;
    int ret;
    enum OperandOptions operandOption = OP_OPTION_NULL;

    if (operandString == NULL)
    {
        return OP_OPTION_NULL;
    }

    skipSpaces(&operandString);

    if (*operandString == '\0')
    {
        return OP_OPTION_NULL;
    }

    if (*operandString == '@')
    {
        if (*(operandString + 1) == 'r')
        {
            if (*(operandString + 2) == '+' || *(operandString + 2) == '-')
            {
                operandOption = OP_OPTION_NULL;
            }
            else if (parseNumber(operandString + 2, NULL, &num, MINREG, MAXREG) != 0)
            {
                operandOption = OP_OPTION_NULL;
            }
            else
            {
                if (reg)
                {
                    *reg = (int)num;
                }
                operandOption = OP_OPTION_REG_NUMBER;
            }
        }
        else
        {
            operandOption = OP_OPTION_NULL;
        }
    }
    else if (isalpha(*operandString))
    {
        temp = strpbrk(operandString, SPACECHARS);
        if (temp)
        {
            *temp = '\0';
            temp++;
            skipSpaces(&temp);
            if (*temp != '\0')
            {
                *operandString = ' ';
                operandOption = OP_OPTION_NULL;
            }
        }
        if (checkLabel(operandString) != correctLabel)
        {
            operandOption = OP_OPTION_NULL;
        }
        else
        {
            if (label)
            {
                *label = operandString;
            }
            operandOption = OP_OPTION_LABEL;
        }
    }
    else
    {
        ret = parseNumber(operandString, NULL, &num, MINNUM, MAXNUM);
        if (ret < -2)
        {
            operandOption = OP_OPTION_NULL;
        }
        else if (ret == 0)
        {
            if (constNum)
            {
                *constNum = num;
            }
            operandOption = OP_OPTION_IMMEDIATE;
        }
    }

    return operandOption;
}
/*
 * This function handles instructions with two operands.
 * It parses and validates the source and destination operands and stores them
 * in the `TokenTree` structure. It also reports errors in case of invalid operands.
 */

static void handleTwoOperands(TokenTree *myTree, char *operandString, struct instruction_mapping *instMap, char *seperator)
{
    char operandOption;
    char *extra = strchr(seperator + 1, ',');
    if (extra)
    {
        reportInstError(myTree, "extra comma in operands", NULL);
        return;
    }
    if (instMap->sourceOpOptions == OP_OPTION_NULL)
    {
        reportInstError(myTree, "two operands for instruction '%s'", instMap->instructionName);
        return;
    }
    *seperator = '\0';
    operandOption = parseOperands(operandString, &myTree->tokenData.instruction.operands[0].labelName, &myTree->tokenData.instruction.operands[0].num, &myTree->tokenData.instruction.operands[0].reg);
    if (operandOption == OP_OPTION_NULL)
    {
        reportInstError(myTree, "Invalid operand: '%s'", operandString);
        return;
    }
    if (operandOption == OP_OPTION_OVERFLOW)
    {
        reportInstError(myTree, "operand overflow: '%s' source", operandString);
        return;
    }
    if (operandOption == OP_OPTION_MISSING)
    {
        reportInstError(myTree, " no operand for source", NULL);
        return;
    }

    if (instMap->sourceOpOptions == OP_OPTION_NULL)
    {
        reportInstError(myTree, "Unsupported source operand: '%s'", operandString);
        return;
    }
    switch (operandOption)
    {
    case OP_OPTION_IMMEDIATE:
        myTree->tokenData.instruction.operandTypes[0] = OP_OPTION_IMMEDIATE;
        break;
    case OP_OPTION_REG_NUMBER:
        myTree->tokenData.instruction.operandTypes[0] = OP_OPTION_REG_NUMBER;
        break;
    default:
        myTree->tokenData.instruction.operandTypes[0] = OP_OPTION_LABEL;
        break;
    }

    operandString = seperator + 1;
    operandOption = parseOperands(operandString, &myTree->tokenData.instruction.operands[1].labelName, &myTree->tokenData.instruction.operands[1].num, &myTree->tokenData.instruction.operands[1].reg);
    if (operandOption == OP_OPTION_NULL)
    {
        reportInstError(myTree, "bad operand for destination: '%s'", operandString);
        return;
    }
    if (operandOption == OP_OPTION_OVERFLOW)
    {
        reportInstError(myTree, "operand overflow for destination: '%s'", operandString);
        return;
    }
    if (operandOption == OP_OPTION_MISSING)
    {
        reportInstError(myTree, "no operand for destination", NULL);
        return;
    }

    if (instMap->destOpOptions == OP_OPTION_NULL)
    {
        reportInstError(myTree, "null destination operand: '%s'", operandString);
        return;
    }
    switch (operandOption)
    {
    case OP_OPTION_IMMEDIATE:
        myTree->tokenData.instruction.operandTypes[1] = OP_OPTION_IMMEDIATE;
        break;
    case OP_OPTION_REG_NUMBER:
        myTree->tokenData.instruction.operandTypes[1] = OP_OPTION_REG_NUMBER;
        break;
    default:
        myTree->tokenData.instruction.operandTypes[1] = OP_OPTION_LABEL;
        break;
    }
}

static void handleOneOperand(TokenTree *myTree, char *operandString, struct instruction_mapping *instMap)
{
    char operandOption;
    if (instMap->sourceOpOptions != OP_OPTION_NULL)
    {
        reportInstError(myTree, "Expected a comma separator for instruction '%s'", instMap->instructionName);
        return;
    }
    operandOption = parseOperands(operandString, &myTree->tokenData.instruction.operands[1].labelName, &myTree->tokenData.instruction.operands[1].num, &myTree->tokenData.instruction.operands[1].reg);
    if (operandOption != OP_OPTION_MISSING && instMap->destOpOptions == OP_OPTION_NULL)
    {
        reportInstError(myTree, "Instruction '%s' should not have operands", instMap->instructionName);
        return;
    }
    if (operandOption == OP_OPTION_MISSING)
    {
        reportInstError(myTree, "Expected operand missing for instruction '%s'", instMap->instructionName);
        return;
    }
    if (operandOption == OP_OPTION_OVERFLOW)
    {
        reportInstError(myTree, "Operand overflow in destination: '%s'", operandString);
        return;
    }
    if (operandOption == OP_OPTION_NULL)
    {
        reportInstError(myTree, "Invalid destination operand: '%s'", operandString);
        return;
    }
    if (instMap->destOpOptions == OP_OPTION_NULL)
    {
        reportInstError(myTree, "Unsupported destination operand: '%s'", operandString);
        return;
    }
    switch (operandOption)
    {
    case OP_OPTION_IMMEDIATE:
    case OP_OPTION_REG_NUMBER:
        myTree->tokenData.instruction.operandTypes[1] = operandOption;
        break;
    default:
        myTree->tokenData.instruction.operandTypes[1] = OP_OPTION_LABEL;
        break;
    }
}

/*
 * This function parses the instructions, identifies the number of operands, and
 * calls the appropriate function to handle them.
 */
static void parseInstructions(TokenTree *myTree, char *operandString, struct instruction_mapping *instMap)
{
    char *seperator = NULL;

    if (operandString)
        seperator = strchr(operandString, ',');
    else
    {
        if (instMap->destOpOptions != OP_OPTION_NULL)
        {
            reportInstError(myTree, "INSTRUCTION '%s' expected one operand", instMap->instructionName);
            return;
        }
        return;
    }

    if (seperator)
    {
        handleTwoOperands(myTree, operandString, instMap, seperator);
    }
    else
    {
        handleOneOperand(myTree, operandString, instMap);
    }
} /*
   * This function handles the entry directive.
   * It parses and validates the operand of the directive and reports errors in case of invalid operand.
   */

static void handleEntryDirective(TokenTree *myTree, char *operandString, struct directive_mapping *directiveMap)
{
    if (parseOperands(operandString, &myTree->tokenData.directive.operands.label, NULL, NULL) != OP_OPTION_LABEL)
    {
        reportDirError(myTree, "DIRECTIVE '%s' has an invalid operand '%s'", directiveMap->directiveName, operandString);
    }
} /*
   * This function handles the string directive.
   * It parses and validates the string operand of the directive and reports errors in case of invalid operand.
   */

static void handleStringDirective(TokenTree *myTree, char *operandString, struct directive_mapping *directiveMap)
{
    char *seperator;
    char *seperator2;

    seperator = strchr(operandString, '"');
    if (!seperator)
    {
        reportDirError(myTree, "DIRECTIVE: '%s' has no opening '\"': '%s'.", directiveMap->directiveName, operandString);
    }
    seperator++;
    seperator2 = strrchr(seperator, '"');
    if (!seperator2)
    {
        reportDirError(myTree, "DIRECTIVE: '%s' has no closing '\"': '%s", directiveMap->directiveName, operandString);
    }
    *seperator2 = '\0';
    seperator2++;
    skipSpaces(&seperator2);
    if (*seperator2 != '\0')
    {
        reportDirError(myTree, "DIRECTIVE: '%s' has extra text after its string: '%s", directiveMap->directiveName, seperator2);
    }
    myTree->tokenData.directive.operands.string = seperator;
}
/*
 * This function handles the data directive.
 * It parses and validates the numeric operands of the directive and reports errors in case of invalid operands.
 */

static void handleDataDirective(TokenTree *myTree, char *operandString, struct directive_mapping *directiveMap)
{
    char *seperator;
    int num_count = 0;
    int curr_num;
    do
    {
        seperator = strchr(operandString, ',');
        if (seperator)
            *seperator = '\0';
        switch (parseOperands(operandString, NULL, &curr_num, NULL))
        {
        case OP_OPTION_IMMEDIATE:
            myTree->tokenData.directive.operands.data[num_count] = curr_num;
            num_count++;
            myTree->tokenData.directive.operands.count = num_count;
            break;
        case OP_OPTION_OVERFLOW:
            reportDirError(myTree, "DIRECTIVE :'%s overflowed number :'%s'", directiveMap->directiveName, operandString);
            return;
        case OP_OPTION_MISSING:
            reportDirError(myTree, "DIRECTIVE :'%s expected a number", directiveMap->directiveName, "");
            break;
        default:
            reportDirError(myTree, "DIRECTIVE :'%s got no number :'%s'", directiveMap->directiveName, operandString);
            break;
        }
        if (seperator)
        {
            operandString = seperator + 1;
        }
        else
            break;
    } while (1);
}
/*
 * This function parses the directives, identifies the type of directive, and
 * calls the appropriate function to handle them
 */

static void parseDirective(TokenTree *myTree, char *operandString, struct directive_mapping *directiveMap)
{
    if (directiveMap->key <= directiveEntry)
    {
        handleEntryDirective(myTree, operandString, directiveMap);
    }
    else if (directiveMap->key == directiveString)
    {
        handleStringDirective(myTree, operandString, directiveMap);
    }
    else if (directiveMap->key <= directiveData)
    {
        handleDataDirective(myTree, operandString, directiveMap);
    }
}
/*
 * This function creates a `TokenTree` structure, identifies the type of token
 * (directive or instruction), parses it, and returns the `TokenTree` structure.
 */

TokenTree *getTree(char *sentenceLine)
{
    enum labelType labelType = 0;
    TokenTree *myTree = (TokenTree *)malloc(sizeof(TokenTree));
    struct instruction_mapping *instMap = NULL;
    struct directive_mapping *directiveMap = NULL;

    char *extra, *extra2;
    if (!isTree)
    {
        lexer_wordT_init();
    }
    sentenceLine[strcspn(sentenceLine, "\r\n")] = 0;
    skipSpaces(&sentenceLine);
    extra = strchr(sentenceLine, ':');
    if (extra)
    {
        extra2 = strchr(extra + 1, ':');
        if (extra2)
        {
            strcpy(myTree->errorMessage, "the token ':' appears twice in this line");
            return myTree;
        }
        (*extra) = '\0';
        switch (checkLabel(sentenceLine))
        {
        case labelStartsWithoutChar:
            sprintf(myTree->errorMessage, "label:'%s' missing alpha", sentenceLine);
            break;
        case labelContainsNumbers:
            sprintf(myTree->errorMessage, "label:'%s' contains numbers", sentenceLine);
            break;
        case labelTooLong:
            sprintf(myTree->errorMessage, "label:'%s' is too long : %d", sentenceLine, MAXLABEL);
            break;
        case correctLabel:
            strcpy(myTree->label, sentenceLine);
            break;
        }
        if (labelType != correctLabel)
        {
            return myTree;
        }
        sentenceLine = extra + 1;
        skipSpaces(&sentenceLine);
    }
    if (*sentenceLine == '\0' && myTree->label[0] != '\0')
    {
        sprintf(myTree->errorMessage, "empty line: '%s'", myTree->label);
        return myTree;
    }
    extra = strpbrk(sentenceLine, SPACECHARS);
    if (extra)
    {
        *extra = '\0';
        extra++;
        skipSpaces(&extra);
    }

    if (*sentenceLine == '.')
    {
        directiveMap = checkIfExists(searchForDirective, sentenceLine + 1);
        if (!directiveMap)
        {
            sprintf(myTree->errorMessage, "directive is unknown :'%s'", sentenceLine + 1);
            return myTree;
        }
        myTree->tokenType = TOKEN_DIRECTIVE;
        myTree->tokenData.directive.type = directiveMap->key;
        parseDirective(myTree, extra, directiveMap);
        return myTree;
    }
    instMap = checkIfExists(searchForInstruction, sentenceLine);
    if (!instMap)
    {
        sprintf(myTree->errorMessage, "keyword is unknown '%s'", sentenceLine);
        return myTree;
    }
    myTree->tokenType = TOKEN_INSTRUCTION;
    myTree->tokenData.instruction.type = instMap->key;
    parseInstructions(myTree, extra, instMap);
    return myTree;
}

/*getters and setters*/
int getTokenTreeOptions(const TokenTree *tree)
{
    if (tree == NULL)
    {
        fprintf(stderr, "Error: NULL pointer passed to getTokenTreeOptions\n");
        return -1;
    }
    return tree->tokenType;
}

void setTokenTreeOptions(TokenTree *tree, int options)
{
    if (tree == NULL)
    {
        fprintf(stderr, "Error: NULL pointer passed to setTokenTreeOptions\n");
        return;
    }
    tree->tokenType = options;
}

int getTokenTreeDirectiveOptions(const TokenTree *tree)
{
    return tree->tokenData.directive.type;
}

void setTokenTreeDirectiveOptions(TokenTree *tree, int directiveOptions)
{
    tree->tokenData.directive.type = directiveOptions;
}

const char *getTokenTreeDirectiveOperandsString(const TokenTree *tree)
{
    return tree->tokenData.directive.operands.string;
}

const char *getTokenTreeDirectiveOperandsLabel(const TokenTree *tree)
{
    return tree->tokenData.directive.operands.label;
}

int getTokenTreeDirectiveOperandsDataData(const TokenTree *tree, size_t index)
{
    return tree->tokenData.directive.operands.data[index];
}

void setTokenTreeDirectiveOperandsDataData(TokenTree *tree, size_t index, int value)
{
    tree->tokenData.directive.operands.data[index] = value;
}

int getTokenTreeDirectiveOperandsDataCount(const TokenTree *tree)
{
    return tree->tokenData.directive.operands.count;
}

void setTokenTreeDirectiveOperandsDataCount(TokenTree *tree, int count)
{
    tree->tokenData.directive.operands.count = count;
}

int getTokenTreeInstructionType(const TokenTree *tree)
{
    return tree->tokenData.instruction.type;
}

void setTokenTreeInstructionType(TokenTree *tree, int instructionType)
{
    tree->tokenData.instruction.type = instructionType;
}

int getTokenTreeInstructionsOperandsOptions(const TokenTree *tree, size_t index)
{
    return tree->tokenData.instruction.operandTypes[index];
}

void setTokenTreeInstructionsOperandsOptions(TokenTree *tree, size_t index, int option)
{
    tree->tokenData.instruction.operandTypes[index] = option;
}

int getTokenTreeInstructionsOperandsNum(const TokenTree *tree, size_t index)
{
    return tree->tokenData.instruction.operands[index].num;
}

void setTokenTreeInstructionsOperandsNum(TokenTree *tree, size_t index, int num)
{
    tree->tokenData.instruction.operands[index].num = num;
}

int getTokenTreeInstructionsOperandsReg(const TokenTree *tree, size_t index)
{
    return tree->tokenData.instruction.operands[index].reg;
}

void setTokenTreeInstructionsOperandsReg(TokenTree *tree, size_t index, int reg)
{
    tree->tokenData.instruction.operands[index].reg = reg;
}

const char *getTokenTreeInstructionsOperandsLabelName(const TokenTree *tree, size_t index)
{
    return tree->tokenData.instruction.operands[index].labelName;
}

TokenTree *createTokenTree()
{
    return calloc(1, sizeof(TokenTree));
}
void destoryTokenTree(struct TokenTree *TokenTree)
{
    free(TokenTree);
}

int getTypeMov(void) { return typeMov; }
int getTypeCmp(void) { return typeCmp; }
int getTypeAdd(void) { return typeAdd; }
int getTypeSub(void) { return typeSub; }
int getTypeLea(void) { return typeLea; }
int getTypeNot(void) { return typeNot; }
int getTypeClr(void) { return typeClr; }
int getTypeInc(void) { return typeInc; }
int getTypeDec(void) { return typeDec; }
int getTypeJmp(void) { return typeJmp; }
int getTypeBne(void) { return typeBne; }
int getTypeRed(void) { return typeRed; }
int getTypePrn(void) { return typePrn; }
int getTypeJsr(void) { return typeJsr; }
int getTypeRts(void) { return typeRts; }
int getTypeStop(void) { return typeStop; }
int getInstruction(void)
{
    return TOKEN_INSTRUCTION;
}
const char *getTokenTreeLabel(const TokenTree *tree)
{
    if (tree == NULL)
    {
        return NULL;
    }
    return tree->label;
}

void setTokenTreeLabel(TokenTree *tree, const char *label)
{
    if (tree == NULL || label == NULL)
    {
        return;
    }
    strncpy(tree->label, label, MAXLABEL);
    tree->label[MAXLABEL] = '\0';
}

const char *getTokenTreeErrorMessage(const TokenTree *tree)
{
    if (tree == NULL)
    {
        return NULL;
    }
    return tree->errorMessage;
}

void setTokenTreeErrorMessage(TokenTree *tree, const char *errorMessage)
{
    if (tree == NULL || errorMessage == NULL)
    {
        return;
    }
    strncpy(tree->errorMessage, errorMessage, sizeof(tree->errorMessage) - 1);
    tree->errorMessage[sizeof(tree->errorMessage) - 1] = '\0';
}

int getDirective(void)
{
    return TOKEN_DIRECTIVE;
}
int getDirectiveData(void) { return directiveData; }
int getDirectiveExtern(void) { return directiveExtern; }
int getDirectiveEntry(void) { return directiveEntry; }
int getDirectiveString(void) { return directiveString; }
int getOperandRegisterNumber(void)
{
    return OP_OPTION_REG_NUMBER;
}

int getOperandLabel(void)
{
    return OP_OPTION_LABEL;
}
int getOperandNumber(void)
{
    return OP_OPTION_IMMEDIATE;
}
int getOperandNull(void)
{
    return OP_OPTION_NULL;
}
void treeDestroy(TokenTree *myTree)
{
    if (myTree == NULL)
    {
        return;
        free(myTree);
    }
}