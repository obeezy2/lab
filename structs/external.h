#ifndef EXTERNALINVOCATION_H
#define EXTERNALINVOCATION_H

#include "../data_structure/list.h"
#define MAXLABEL 31

typedef struct ExternalInvocation ExternalInvocation;

List getCallAddressesses(const struct ExternalInvocation *ei);
void setCallAddressesses(struct ExternalInvocation *ei, List callAddressesses);

const char *getExternName(const struct ExternalInvocation *ei);
void setExternName(struct ExternalInvocation *ei, const char *externName);
void *externConstructor(const void *copy);
void externDestructor(void *item);
size_t sizeOfExtern();
#endif
