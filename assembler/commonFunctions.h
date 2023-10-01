#ifndef _COMMONFNC_H
#define _COMMONFNC_H
#include "stdio.h"
#include "../data_structure/list.h"
#include "../structs/external.h"

void addExtern(List externsVec, const char *externName, const unsigned int callAddress);
void *wordConstructor(const void *copy);
void wordDestructor(void *item);

#endif