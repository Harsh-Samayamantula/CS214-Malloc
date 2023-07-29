#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mymalloc.h"

int main()
{
    myinit(1);

    void *p1 = mymalloc(500);
    *(int *)p1 = 999;
    printf("the value at %p is %d\n", p1, *(int *)p1);

    printHeap(0);

    void *p2 = mymalloc(10);
    printHeap(0);

    void *p3 = mymalloc(10000);
    printHeap(0);

    myfree(p1);
    printHeap(0);
    p2 = myrealloc(p2, 100);
    // printHeap(0);

    myfree(p2);

    printHeap(0);
    p3 = myrealloc(p3, 10700);
    printHeap(0);

    void *p4 = mymalloc(70);
    printHeap(0);
    myfree(p3);
    printHeap(0);

    myfree(p4);
    printHeap(0);

    mycleanup();
    return EXIT_SUCCESS;
}