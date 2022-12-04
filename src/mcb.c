#include "mcb.h"

int idleInit(dllist *idle)
{
    dllistInit(idle);
    
    return 0;
}

//给定idle链表头，使用头插法插入MCB，并将该MCB的类型置为挂起
//Attetion：起连接作用的是mcbToAdd（类型为MCB）中的dllnode（类型为dllist）
int idleInsert(dllist *idle, MCB *mcbToAdd)
{
    if (mcbToAdd->memAddr != NULL)
    {
        /* 说明该mcb还没有从mem链表中删除，因此idle插入失败 */
        printf("The mcb to be added has not been deleted from mem list.\n");

        return -1;
    }

    dllistInsert(idle, &(mcbToAdd->dllNode));

    return 0;
}

//给定mcb节点，将该节点从dile链表中删除
int idleDelete(MCB *mcbToDelete)
{
    if (mcbToDelete->memType != IDLE)
    {
        /* 说明要删除的mcb不是idle链表中的mcb，删除失败 */
        printf("The mcb to be deleted is not in the idle list.\n");

        return -1;
    }

    dllistDelete(&(mcbToDelete->dllNode));

    return 0;
}

//初始化mem链表，肯定会有一个mcb控制整块用户内存空间，因为刚开始没人使用，所以type肯定为FREE
int memInit(dllist *mem, MCB *firstMcb)
{
    dllistInit(mem);//必须进行初始化，否则下一步会对空指针操作造成段错误
    dllistInsert(mem, &(firstMcb->dllNode));

    firstMcb->memType = FREE;

    return 0;
}

//在mem链表中间插入mcb
int memInsert(dllist *prevMcb_dllNode, dllist *mcbToAdd_dllNode)
{
    dllistInsert(prevMcb_dllNode, mcbToAdd_dllNode);

    return 0;
}


int memDelete(MCB *mcbToDelete)
{
    if (mcbToDelete->memType == IDLE)
    {
        /* 说明要删除的mcb是idle链表中的节点，删除失败 */
        printf("The mcb to be deleted is not in the mem list.\n");

        return -1;
    }

    dllistDelete(&(mcbToDelete->dllNode));

    //当从mem链表中删除mcb时，该mcb肯定要归还到idle链表中，因此该mcb的type肯定为IDLE
    mcbToDelete->memType = IDLE;

    return 0;
}

//销毁idle或mem链表
int idleOrMemDestroy(dllist *idleOrMem)
{
    dllist *currentDll = idleOrMem->next;
    MCB *currentMCB = NULL;
    while (currentDll != idleOrMem)
    {
        currentMCB = container_of(currentDll, MCB, dllNode);
        currentMCB->memSize = 0;
        currentMCB->memAddr = NULL;
        currentMCB->memType = IDLE;
        currentDll = currentDll->next;
    }
    dllistDestroy(idleOrMem);
    return 0;
}

void mcbMerge(MCB *prevMcb, MCB *nextMcb)
{
    prevMcb->memSize += nextMcb->memSize;
    prevMcb->memType = FREE;
    memDelete(nextMcb);
    nextMcb->memSize = 0;
    nextMcb->memAddr = NULL;
    nextMcb->memType = IDLE;
}

//打印idle链表或者mem链表信息
void mcbListDump(dllist *head)
{
    int usedNum = 0;
    int freeNum = 0;
    int idleNum = 0;
    MCB *mcbProbe = NULL;
    dllist *probe = head->next;

    while (probe != head)
    {
        mcbProbe = container_of(probe, MCB, dllNode);
        if (mcbProbe->memType == USED)
        {
            usedNum++;
        }
        if (mcbProbe->memType == FREE)
        {
            freeNum++;
        }
        if (mcbProbe->memType == IDLE)
        {
            idleNum++;
        }
        probe = probe->next;
    }
    
    if (idleNum)
    {
        printf("idle dllist\n");
        printf("IDLE mcbNum: %d\n", idleNum);
    }
    else
    {
        printf("mem dllist\n");
        printf("USED mcbNum: %d\t\tFREE mcbNum: %d\n", usedNum, freeNum);
    }
}