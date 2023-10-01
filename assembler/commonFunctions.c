#include "assembler.h"
#include "../lexicalAnalysis/lexicalAnalysis.h"
#include "stdio.h"
#include "../data_structure/tree.h"
#include "../data_structure/list.h"
#include "../preAssembly/preAssembler.h"
#include "stdlib.h"
#include "string.h"
#include "../structs/external.h"
#include "../output/output.h"
#include "err.h"
#include "firstPass.h"
#include "secondPass.h"
void *wordConstructor(const void *copy)
{

    return memcpy(malloc(sizeof(unsigned int)), copy, sizeof(unsigned int));
}

void wordDestructor(void *item)
{
    free(item);
}

/**
 * Adds an external reference to the list of external references.
 * If the external reference with the same name already exists, the new call
 * address is added to the existing list of call addresses for that reference.
 * If the external reference does not exist, a new external reference is created,
 * and the call address is added to it.
 *
 * @param externsVec The list of external references.
 * @param externName The name of the external reference.
 * @param callAddress The address where the external reference is called.
 */
void addExtern(List externsVec, const char *externName, const unsigned int callAddress)
{
    void *const *externStart;
    void *const *externEnd;
    struct ExternalInvocation *extern1 = (struct ExternalInvocation *)malloc(sizeOfExtern());
    List callAddressList = createDynamicList(wordConstructor, wordDestructor);

    if (!extern1)
    {
        fprintf(stderr, "Memory allocation error\n");
        return;
    }

    setCallAddressesses(extern1, NULL);
    setExternName(extern1, externName);

    for (externStart = listGetBegin(externsVec), externEnd = listGetEnd(externsVec); externStart <= externEnd; externStart++)
    {
        if (*externStart)
        {
            struct ExternalInvocation *eInvocation = (struct ExternalInvocation *)(*externStart);
            if (strcmp(externName, getExternName(eInvocation)) == 0)
            {
                List callAddresses = getCallAddressesses(eInvocation);
                listInsertItem(callAddresses, &callAddress);
                free(extern1);
                return;
            }
        }
    }

    if (!callAddressList)
    {
        fprintf(stderr, "Memory allocation error\n");
        free(extern1);
        return;
    }
    listInsertItem(callAddressList, &callAddress);
    setCallAddressesses(extern1, callAddressList);

    listInsertItem(externsVec, extern1);
}
