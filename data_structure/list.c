
#include <stdlib.h>
#include "list.h"

#define LISTSIZE 12

struct ListData
{
    void **items;
    size_t pointers;
    size_t itemCount;
    void *(*itemCtor)(const void *copy);
    void (*itemDtor)(void *item);
};
void *const *listGetBegin(const List vec)
{
    return vec->items;
}
void *const *listGetEnd(const List vec)
{
    return &vec->items[vec->pointers - 1];
}

size_t listGetItemCount(const List vec)
{
    return vec->itemCount;
}

void listDeallocItems(List vec)
{
    size_t it;

    if (vec->itemDtor != NULL)
    {
        for (it = 0; it < vec->pointers; it++)
        {
            if (vec->items[it] != NULL)
                vec->itemDtor(vec->items[it]);
        }
    }
}

void listDealloc(List *vec)
{
    if (*vec != NULL)
    {
        listDeallocItems(*vec);

        free((*vec)->items);
        free(*vec);
        *vec = NULL;
    }
}

List createDynamicList(void *(*itemCtor)(const void *copy), void (*itemDtor)(void *item))
{
    List newVec = calloc(1, sizeof(struct ListData));
    if (newVec == NULL)
        return NULL;
    newVec->pointers = LISTSIZE;
    newVec->items = calloc(LISTSIZE, sizeof(void *));
    if (newVec->items == NULL)
    {
        free(newVec);
        return NULL;
    }
    newVec->itemCtor = itemCtor;
    newVec->itemDtor = itemDtor;
    return newVec;
}

void *listInsertItem(List vec, const void *copy)
{
    size_t it;
    void **temp;
    if (vec->itemCount == vec->pointers)
    {
        vec->pointers *= 2;
        temp = realloc(vec->items, vec->pointers * sizeof(void *));
        if (temp == NULL)
        {
            vec->pointers /= 2;
            return NULL;
        }
        vec->items = temp;
        vec->items[vec->itemCount] = vec->itemCtor(copy);
        if (vec->items[vec->itemCount] == NULL)
        {
            return NULL;
        }
        vec->itemCount++;

        for (it = vec->itemCount; it < vec->pointers; it++)
        {
            vec->items[it] = NULL;
        }
    }
    else
    {
        for (it = 0; it < vec->pointers; it++)
        {
            if (vec->items[it] == NULL)
            {
                vec->items[it] = vec->itemCtor(copy);
                if (vec->items[it] != NULL)
                {
                    vec->itemCount++;
                    break;
                }
                return NULL;
            }
        }
    }
    return vec->items[vec->itemCount - 1];
}
