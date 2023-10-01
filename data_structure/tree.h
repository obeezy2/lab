
#ifndef __TRIE_H_
#define __TRIE_H_

typedef struct wordT *WordTree;

WordTree wordT();

const char *insertWord(WordTree wordT, const char *string, void *endString);

void *checkIfExists(WordTree wordT, const char *string);

void treeDealloc(WordTree *wordT);

#endif