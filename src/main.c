#include <stdio.h>
#include <stdlib.h>

#include "yamm.h"

int main(void)
{
    yammInit(0x100);
    yammDump();
    void *p1 = yammAlloc(0x1);
    void *p2 = yammAlloc(0x1);
    void *p3 = yammAlloc(0x1);
    // printf("%p\n", p1);
    // printf("%p\n", p2);
    yammFree(p1);
    yammFree(p3);
    // yammFree(p2);
    printf("\n");
    yammDump();
    // int res = yammDestroy();
    int res = yammForceDestroy();
    printf("%d\n", res);
    
    return 0;
}