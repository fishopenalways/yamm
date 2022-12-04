# ifndef _YAMM_H_
# define _YAMM_H_

#define ALIGN(n) ((n+7) & ~7)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dll.h"
#include "mcb.h"

typedef struct _YAMM
{
    void *newMem;
    dllist *mem;
    dllist *idle;
    MCB *mcb;
    void *userMem;
    int userMemSize;
}YAMM;

int yammInit(int size);
void *yammAlloc(int size);
int yammFree(void *ptr);
int yammDestroy();
int yammDump();

int yammForceDestroy();

#endif