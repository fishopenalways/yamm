#include <stdio.h>
#include "dll.h"

/* 循环双向链表初始化，即链表头的prev和next指向链表头本身 */
void dllistInit(dllist *dllistHead)
{
    dllistHead->prev = dllistHead;
    dllistHead->next = dllistHead;

}

/* 给定要添加的节点以及前一个节点，直接进行插入 */
void dllistInsert(dllist *dllistPrevNode, dllist *dllistNodeToAdd)
{
    if (NULL == dllistPrevNode || NULL == dllistNodeToAdd)
    {
        printf("Input is error!\n");
        return;
    }

    dllistNodeToAdd->next = dllistPrevNode->next;
    dllistNodeToAdd->prev = dllistPrevNode;
    dllistPrevNode->next = dllistNodeToAdd;
    dllistNodeToAdd->next->prev = dllistNodeToAdd;

    return;
}

/* 给定一个循环双向链表节点，在该节点所在的链表中删除该节 */
void dllistDelete(dllist *dllistNodeToDelete)
{
    if (!dllistNodeToDelete)
    {
        printf("The node to be deleted is NULL\n");
        return;
    }
    
    dllistNodeToDelete->prev->next = dllistNodeToDelete->next;
    dllistNodeToDelete->next->prev = dllistNodeToDelete->prev;
    dllistNodeToDelete->prev = NULL;
    dllistNodeToDelete->next = NULL;
    dllistNodeToDelete = NULL;
    
    return;
}

/* 销毁循环双向链表 */
void dllistDestroy(dllist *dll)
{
    dllist *currentDll = dll->next;
    dllist *nextDll = currentDll->next;
    while (currentDll != dll)
    {
        dllistDelete(currentDll);
        currentDll = nextDll;
        nextDll = nextDll->next;
    }
    currentDll = NULL;
    nextDll = NULL;
}