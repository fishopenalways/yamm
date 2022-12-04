#ifndef _MCB_H_
#define _MCB_H_

#include <stdio.h>
#include "dll.h"

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE*)0)->MEMBER)
#define container_of(ptr, type, member) ({				\
     void *__mptr = (void *)(ptr);                      \
     ((type *)(__mptr - offsetof(type, member))); })

#define MCB_SIZE sizeof(MCB)

typedef struct _MCB
{
    dllist dllNode;
    void *memAddr;   //记录内存控制块管理的内存起始地址
    int memSize;            //记录内存控制块管理的内存大小
    enum type
    {
        IDLE = 0,
        FREE = 1,
        USED = 2,
    }memType;               //记录内存控制块管理的内存的当前状态
}MCB;

int idleInit(dllist *idle);
int idleInsert(dllist *idle, MCB *mcbToAdd);
int idleDelete(MCB *mcbToDelete);
int idleOrMemDestroy(dllist *idleOrMem);
int memInit(dllist *mem, MCB *firstMcb);
int memInsert(dllist *prevMcb_dllNode, dllist *mcbToAdd_dllNode);
int memDelete(MCB *mcbToDelete);
void mcbMerge(MCB *prevMcb, MCB *nextMcb);
void mcbListDump(dllist *head);

#endif