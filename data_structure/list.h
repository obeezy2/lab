#ifndef LIST_H
#define LIST_H
#include "stdio.h"
#include "stddef.h"
#include "stdint.h"

typedef struct ListData *List;

List createDynamicList(void *(*itemCtor)(const void *copy), void (*itemDtor)(void *item));
void *listInsertItem(List vec, const void *copy);
void *const *listGetBegin(const List vec);
void *const *listGetEnd(const List vec);
size_t listGetItemCount(const List vec);
void listDeallocItems(List vec);
void listDealloc(List *vec);

#endif
