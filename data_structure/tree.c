#include "tree.h"
#include <stdlib.h>

#define BASECHAR ' '
struct wordElement
{
    void *endString;
    struct wordElement *next[95];
};

struct wordT
{
    struct wordElement *next[95];
};
static struct wordElement *traverseTree(struct wordElement *element, const char *string)
{
    while (element)
    {
        if (*string == '\0' && element->endString != NULL)
        {
            return element;
        }
        element = element->next[(*string) - BASECHAR];
        string++;
    }
    return NULL;
}

WordTree wordT()
{
    return calloc(1, sizeof(struct wordT));
}

const char *insertWord(WordTree wordT, const char *string, void *endString)
{
    struct wordElement **iterator = &wordT->next[(*string) - BASECHAR];
    while (1)
    {
        if (*iterator == NULL)
        {
            (*iterator) = calloc(1, sizeof(struct wordElement));
            if (*iterator == NULL)
                return NULL;
        }
        string++;
        if (*string != '\0')
            iterator = &(*iterator)->next[(*string) - BASECHAR];
        else
            break;
    }
    (*iterator)->endString = endString;
    return string;
}

void *checkIfExists(WordTree wordT, const char *string)
{
    struct wordElement *find_node;
    if (string == NULL)
        return NULL;
    find_node = traverseTree(wordT->next[(*string) - BASECHAR], string + 1);
    return find_node == NULL ? NULL : find_node->endString;
}

static void deallocSubTree(struct wordElement *element)
{
    int i;
    for (i = 0; i < 95; i++)
    {
        if (element->next[i] != NULL)
        {
            deallocSubTree(element->next[i]);
            element->next[i] = NULL;
        }
    }
    free(element);
}
void treeDealloc(WordTree *wordT)
{
    int i;
    if (*wordT != NULL)
    {
        WordTree t = *wordT;
        for (i = 0; i < 95; i++)
        {
            if (t->next[i] != NULL)
                deallocSubTree(t->next[i]);
        }
        free(*wordT);
        (*wordT) = NULL;
    }
}