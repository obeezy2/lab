#include "assembler/assembler.h"
#include "lexicalAnalysis/lexicalAnalysis.h"

int main(int argc, char **argv)
{
    return assembler(argc - 1, argv + 1);

}