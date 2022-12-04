#include "yamm.h"

YAMM yamm = {NULL, NULL, NULL, NULL, NULL, 0};

enum RET_VAL {
    ERROR,
    OK,
};

int yammInit(int size)
{
    size = ALIGN(size);
    int mcbNum = size / 2;
    yamm.newMem = malloc(2 * DLLIST_SIZE + mcbNum * MCB_SIZE + size);
    MCB *idleMcb = NULL;

    if (!yamm.newMem)
    {
        printf("init error\n");
        return ERROR;
    }

    memset(yamm.newMem, 0, 2 * DLLIST_SIZE + mcbNum * MCB_SIZE + size);

    yamm.mem = (dllist *)yamm.newMem;
    yamm.idle = (dllist *)(yamm.newMem + DLLIST_SIZE);
    yamm.mcb = (MCB *)(yamm.newMem + (2 * DLLIST_SIZE));
    yamm.userMem = yamm.newMem + (2 * DLLIST_SIZE + mcbNum * MCB_SIZE);
    yamm.userMemSize = size;

    //初始化mem链表
    memInit(yamm.mem, yamm.mcb);
    yamm.mcb->memSize = yamm.userMemSize;
    yamm.mcb->memAddr = yamm.userMem;

    //初始化idle链表
    idleInit(yamm.idle);
    idleMcb = yamm.mcb + 1;
    for (int i = 0; i < (mcbNum - 1); i++)
    {
        idleMcb->memType = IDLE;//此句其实可以省略，memset之后所有mcb的type都为0，对应IDLE
        idleInsert(yamm.idle, idleMcb);
        idleMcb++;
    }
    
    return OK;
}

int yammForceDestroy()
{
    idleOrMemDestroy(yamm.idle);    //销毁idle链表
    idleOrMemDestroy(yamm.mem);     //销毁mem链表
    
    yamm.mem = NULL;
    yamm.idle = NULL;
    yamm.mcb = NULL;
    yamm.userMem = NULL;

    free(yamm.newMem);
    yamm.newMem = NULL;
    
    printf("Successfully Destroy!\n");

    return OK;
}

void *yammAlloc(int size)
{
    size = ALIGN(size);
    
    if (size <= 0 || (yamm.idle->next == yamm.idle) || size > yamm.userMemSize)
    {
        /* 如果size小于0或者idle链表为空或者申请的内存大于yamm管理的总内存，则分配失败 */
        printf("Alloc failed!\n");
        return NULL;
    }
    
    dllist *currentDll = yamm.mem->next;
    MCB *currentMCB = NULL;
    while (currentDll != yamm.mem)
    {
        currentMCB = container_of(currentDll, MCB, dllNode);
        if (currentMCB->memType == FREE)
        {
            if (currentMCB->memSize == size)
            {
                currentMCB->memType = USED;
                printf("Alloc success!\n");
                return currentMCB->memAddr;
            }
            if (currentMCB->memSize > size)
            {
                MCB *mcb = container_of(yamm.idle->next, MCB, dllNode);
                idleDelete(mcb);
                mcb->memType = FREE;
                memInsert(&(currentMCB->dllNode), &(mcb->dllNode));
                mcb->memSize = currentMCB->memSize - size;
                currentMCB->memSize = size;
                currentMCB->memType = USED;
                mcb->memAddr = currentMCB->memAddr + size;
                printf("Alloc success!\n");
                return currentMCB->memAddr;
            }   
        }
        currentDll = currentDll->next;
    }

    return NULL;
}

int yammFree(void *ptr)
{
    if(!ptr)
    {
        return ERROR;
    }

    dllist *currDll = yamm.mem->next;
    dllist *prevDll;
    dllist *nextDll;
    MCB *prevMcb;
    MCB *currMcb;
    MCB *nextMcb;
    while(currDll != yamm.mem)
    {
        currMcb = container_of(currDll, MCB, dllNode);
        
        if(currMcb->memAddr == ptr)
        {
            prevDll = currDll->prev;
            nextDll = currDll->next;
            if(prevDll == yamm.mem)
            {
                nextMcb = container_of(nextDll, MCB, dllNode);
                if(nextMcb->memType == USED)
                {
                    currMcb->memType = FREE;
                    printf("yamm.mem   USED\n");
                    return OK;
                }
                if(nextMcb->memType == FREE)
                {
                    mcbMerge(currMcb, nextMcb);
                    idleInsert(yamm.idle, nextMcb);
                    printf("yamm.mem   FREE\n");
                    return OK;
                }
            }
            if(nextDll == yamm.mem)
            {
                prevMcb = container_of(prevDll, MCB, dllNode);
                if(prevMcb->memType == USED)
                {
                    currMcb->memType = FREE;
                    printf("USED    yamm.mem\n");
                    return OK;
                }
                if(prevMcb->memType == FREE)
                {
                    mcbMerge(prevMcb, currMcb);
                    idleInsert(yamm.idle, currMcb);
                    printf("FREE    yamm.mem\n");
                    return OK;
                }
            }
            prevMcb = container_of(prevDll, MCB, dllNode);
            nextMcb = container_of(nextDll, MCB, dllNode);
            if(prevMcb->memType == USED && nextMcb->memType == USED)
            {
                currMcb->memType = FREE;
                printf("USED    USED\n");
                return OK;
            }
            if(prevMcb->memType == FREE && nextMcb->memType == USED)
            {
                mcbMerge(prevMcb, currMcb);
                idleInsert(yamm.idle, currMcb);
                printf("FREE    USED\n");
                return OK;
            }
            if(prevMcb->memType == USED && nextMcb->memType == FREE)
            {
                mcbMerge(currMcb, nextMcb);
                idleInsert(yamm.idle, nextMcb);
                printf("USED    FREE\n");
                return OK;
            }
            if(prevMcb->memType == FREE && nextMcb->memType == FREE)
            {
                mcbMerge(prevMcb, currMcb);
                mcbMerge(prevMcb, nextMcb);
                idleInsert(yamm.idle, currMcb);
                idleInsert(yamm.idle, nextMcb);
                printf("FREE    FREE\n");
                return OK;
            }
            return OK;
        }
        currDll = currDll->next;
    }
    return ERROR;
}

int yammDestroy()
{
    dllist *currDll = yamm.mem->next;
    MCB *currMcb;
    while(currDll != yamm.mem)
    {
        currMcb = container_of(currDll, MCB, dllNode);
        if(currMcb->memType != FREE)
        {
            printf("destroy fail.\n");
            return ERROR;
        }
        currDll = currDll->next;
    }
    idleOrMemDestroy(yamm.idle);
    idleOrMemDestroy(yamm.mem);
    yamm.idle = NULL;
    yamm.mem = NULL;
    yamm.mcb = NULL;
    yamm.userMem = NULL;
    free(yamm.newMem);
    yamm.newMem = NULL;
    return OK;
}

int yammDump()
{
    if (!yamm.newMem)
    {
        printf("No yamm exists\n");

        return ERROR;
    }
    
    MCB *mcb = yamm.mcb;
    int totalMcbCount = ((char*)yamm.userMem - (char*)yamm.mcb) / MCB_SIZE;

    //使用表驱动的思想，增强可维护性，当mcb状态种类增加，只需修改mcbCount和mcbType数组
    int mcbCount[] = {0, 0, 0};      //第一个是idle，第二个是free，第三个是used
    int mcbType[] = {0, 1, 2};       //第一个是idle，第二个是free，第三个是used
    int arrSize = sizeof(mcbCount) / sizeof(mcbCount[0]);
    
    for (int i = 0; i < totalMcbCount; i++)
    {
        for (int j = 0; j < arrSize; j++)
        {
            if (!(mcb->memType - mcbType[j]))
            {
                mcbCount[j]++;
            }
        }
        mcb++;
    }

    printf("total MCB: %d\n", totalMcbCount);
    printf("idle MCB: %d\n", mcbCount[0]);
    printf("free MCB: %d\n", mcbCount[1]);
    printf("used MCB: %d\n", mcbCount[2]);
    return OK;
}