#include "external.h"
#include <string.h>
#include "stdlib.h"
struct ExternalInvocation
{
    List callAddressesses;
    char externName[MAXLABEL + 1];
};

/*<-------------------Getters and setters---------------->*/

List getCallAddressesses(const struct ExternalInvocation *ei)
{
    return ei->callAddressesses;
}

void setCallAddressesses(struct ExternalInvocation *ei, List callAddressesses)
{
    ei->callAddressesses = callAddressesses;
}

const char *getExternName(const struct ExternalInvocation *ei)
{
    return ei->externName;
}

void setExternName(struct ExternalInvocation *ei, const char *externName)
{
    strncpy(ei->externName, externName, MAXLABEL);
    ei->externName[MAXLABEL] = '\0';
}

/*-------------------------------------------------------------------*/

/*Returns the sizeof the struct*/

size_t
sizeOfExtern()
{
    return sizeof(struct ExternalInvocation);
}

/*Extern constructor and destroctur*/
void *externConstructor(const void *copy)
{
    return memcpy(malloc(sizeof(struct ExternalInvocation)), copy, sizeof(struct ExternalInvocation));
}
void externDestructor(void *item)
{
    struct ExternalInvocation *external = item;
    listDealloc(&external->callAddressesses);
    free(item);
}
