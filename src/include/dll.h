#ifndef _DLL_H_
#define _DLL_H_

#define DLLIST_SIZE sizeof(dllist)

typedef struct dllist
{
    struct dllist* prev;
    struct dllist* next;
}dllist;

void dllistInit(dllist *dllistHead);
void dllistInsert(dllist *dllistPrevNode, dllist *dllistNodeToAdd);
void dllistDelete(dllist *dllistNodeToDelete);
void dllistDestroy(dllist *dll);

#endif