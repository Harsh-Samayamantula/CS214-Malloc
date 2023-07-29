#include <sys/types.h>
#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef struct HeapCell
{
    size_t size;
    int free;
    struct HeapCell *next;
    struct HeapCell *nextFree;
    struct HeapCell *prev;
} HeapCell;

HeapCell *heap;
HeapCell *lastAllocated;
int allocStrategy = -1;
int HEAP_SIZE = 1024 * 1024;

/**
 * @brief
 *
 * @param printMode 0 to print all heap cells, 1 to print only free cells
 */
void printHeap(int printMode)
{

    int totalMemSize = 0;
    HeapCell *curr = heap;
    printf("\n");
    while (curr != NULL)
    {
        totalMemSize += curr->size;
        printf("(%ld %s)", curr->size, curr->free ? "free" : "in-use");

        if (printMode == 0)
            curr = curr->next;
        else if (printMode == 1)
            curr = curr->nextFree;

        if (curr != NULL)
            printf(" => ");
    }
}

void myinit(int allocAlg)
{

    if (allocAlg < 0 || allocAlg > 2)
    {
        printf("Provide a valid allocation strategy. 0 = first fit, 1 = next fit, 2 = best fit");
        return;
    }
    allocStrategy = allocAlg;
    heap = malloc(HEAP_SIZE);

    // heap = aligned_alloc(8, HEAP_SIZE);
    printf("%d\n", HEAP_SIZE);
    heap->next = NULL;
    heap->nextFree = NULL;
    heap->prev = NULL;
    heap->free = 1;
    heap->size = HEAP_SIZE - sizeof(HeapCell);
    lastAllocated = NULL;
}

void *mymalloc(size_t size)
{
    if (size == 0)
        return NULL;

    if (allocStrategy < 0 || allocStrategy > 2)
    {
        printf("Provide a valid allocation strategy. 0 = first fit, 1 = next fit, 2 = best fit");
        return NULL;
    }

    // header + memory + padding if needed
    size = size + sizeof(HeapCell);
    size = size % 8 == 0 ? size : size + 8 - size % 8;

    if (allocStrategy == 2)
    {
        // best fit
        int bestFitSize = __INT_MAX__;
        HeapCell *bestFit = NULL;
        HeapCell *prev = NULL;
        HeapCell *bestFitPrev = NULL;
        HeapCell *curr = heap;
        while (curr != NULL)
        {
            if (curr->free == 1 && curr->size > size && (bestFit == NULL || curr->size < bestFitSize))
            {
                bestFit = curr;
                bestFitSize = curr->size;
                if (prev != NULL)
                    bestFitPrev = prev;
            }
            else if (curr->free == 1 && curr->size == size)
            {
                curr->free = 0;
                return curr + sizeof(HeapCell);
            }
            prev = curr;
            curr = curr->next;
        }

        if (bestFit == NULL)
        {
            return NULL;
        }

        // allocated cell
        int originalSize = bestFit->size;
        bestFit->size = size;
        bestFit->free = 0;

        // left over cell
        HeapCell *nextCell = bestFit + bestFit->size;
        nextCell->size = originalSize - bestFit->size;
        nextCell->next = bestFit->next;
        nextCell->nextFree = bestFit->nextFree;
        nextCell->prev = bestFit;
        nextCell->free = 1;

        bestFit->next = nextCell;
        bestFit->nextFree = nextCell;

        // last allocation
        lastAllocated = bestFit;

        if (bestFitPrev != NULL)
            bestFitPrev->nextFree = nextCell;

        return bestFit + sizeof(HeapCell);
    }
    else
    {

        // first fit
        HeapCell *curr = heap;
        HeapCell *prev = NULL;

        if (allocStrategy == 1 && lastAllocated != NULL)
        {
            // next fit
            while (curr != NULL)
            {
                if (curr == lastAllocated)
                {
                    break;
                }
                curr = curr->next;
            }
            if (curr != lastAllocated)
            {
                curr = heap;
            }
        }
        while (curr != NULL)
        {
            if (curr->free == 1 && curr->size > size)
            {

                // allocated cell
                int originalSize = curr->size;
                curr->size = size;
                curr->free = 0;

                // left over cell
                HeapCell *nextCell = curr + curr->size;
                nextCell->size = originalSize - curr->size;
                nextCell->next = curr->next;
                nextCell->nextFree = curr->nextFree;
                nextCell->prev = curr;
                nextCell->free = 1;

                curr->next = nextCell;

                lastAllocated = nextCell;

                if (prev != NULL)
                    prev->nextFree = nextCell;

                return curr + sizeof(HeapCell);
            }
            else if (curr->free == 1 && curr->size == size)
            {
                // curr->free = 1;
                curr->free = 0;
                return curr + sizeof(HeapCell);
            }

            prev = curr;
            curr = curr->next;
        }

        if (allocStrategy == 1)
        {
            HeapCell *curr = heap;
            while (curr != NULL)
            {
                if (curr->free == 1 && curr->size > size)
                {

                    // allocated cell
                    int originalSize = curr->size;
                    curr->size = size;
                    curr->free = 0;

                    // left over cell
                    HeapCell *nextCell = curr + curr->size;
                    nextCell->size = originalSize - curr->size;
                    nextCell->next = curr->next;
                    nextCell->nextFree = curr->nextFree;
                    nextCell->prev = curr;
                    nextCell->free = 1;

                    curr->next = nextCell;

                    lastAllocated = nextCell;

                    if (prev != NULL)
                        prev->nextFree = nextCell;

                    return curr + sizeof(HeapCell);
                }
                else if (curr->free == 1 && curr->size == size)
                {
                    // curr->free = 1;
                    curr->free = 0;
                    return curr + sizeof(HeapCell);
                }

                prev = curr;
                curr = curr->next;
            }
        }
    }

    return NULL;
}

// TODO refactor to use the freeNext.
void myfree(void *inputPointer)
{
    HeapCell *ptr = (HeapCell *)inputPointer - sizeof(HeapCell);
    HeapCell *curr = heap;
    HeapCell *next = heap->next;
    HeapCell *prev = NULL;

    while (curr != NULL)
    {
        if (curr == ptr)
        {

            if (next != NULL && next->free == 1)
            {
                curr->size = curr->size + next->size;
                curr->next = next->next;
                next = NULL;
            }
            if (prev != NULL && prev->free == 1)
            {
                prev->size = prev->size + curr->size;
                prev->next = next;
                curr = prev;
            }

            curr->free = 1;
            if (prev != NULL)
                prev->nextFree = curr;

            return;
        }
        next = next->next;
        prev = curr;
        curr = curr->next;
    }

    // clear
}

void *myrealloc(void *ptr, size_t size)
{

    if (ptr == NULL && size == 0)
    {
        return NULL;
    }
    if (ptr == NULL)
    {
        return mymalloc(size);
    }
    if (size == 0)
    {
        myfree(ptr);
        return NULL;
    }

    // header + memory + padding if needed
    size = size + sizeof(HeapCell);
    size = size % 8 == 0 ? size : size + 8 - size % 8;

    HeapCell *alloc = (HeapCell *)ptr - sizeof(HeapCell);

    if (ptr != NULL && alloc->size > size)
    {
        return alloc + sizeof(HeapCell);
    }
    // GET PREVIOUS FREE
    HeapCell *prevFree = NULL;
    HeapCell *iter = heap;
    if (alloc->next != NULL && alloc->next->free == 1)
    {
        while (iter != NULL)
        {
            if (iter == alloc->next)
            {
                break;
            }
            prevFree = iter;
            iter = iter->nextFree;
        }
    }

    if (alloc->next != NULL && alloc->next->free == 1 && (alloc->size + alloc->next->size) > size)
    {
        alloc->next->size = alloc->next->size - (size - alloc->size);
        // Increasing the size of the current allocated block
        alloc->size = size;

        memcpy(alloc + alloc->size, alloc->next, sizeof(HeapCell));
        // alloc->next = alloc + alloc->size;

        if (prevFree != NULL)
        {
            prevFree->nextFree = alloc->next;
        }

        return alloc + sizeof(HeapCell);
    }
    else if (alloc->next != NULL && alloc->next->free == 1 && alloc->size + alloc->next->size == size)
    {
        alloc->size = size;
        if (prevFree != NULL)
        {
            prevFree = alloc->next->nextFree;
        }
        alloc->next = alloc->next->next;
        alloc->free = 0;
        return alloc + sizeof(HeapCell);
    }

    HeapCell *previous = alloc->prev;
    HeapCell *prevPrev = NULL;
    if (previous != NULL)
    {

        prevPrev = previous->prev;
    }
    if (previous != NULL && previous->free == 1 && previous->size + alloc->size > size)
    {
        // COPY DATA to the beginning of the previous free block
        memcpy(previous + sizeof(HeapCell), alloc + sizeof(HeapCell), alloc->size - sizeof(HeapCell));

        alloc->size = previous->size + alloc->size - size;
        // printf("Newly modified size of alloc: %ld \n\n", alloc->size);
        previous->size = size;
        previous->free = 0;

        memcpy(previous + previous->size, alloc, sizeof(HeapCell));
        // printf("Newly modified size of alloc: %ld \n\n", alloc->size);

        alloc->free = 1;

        // Setting the reallocated block's next to the newly free block
        previous->next = alloc;
        // Since prevprev's nextfree prev was previous, setting it to alloc will ensure that the nextfree is preserved
        previous->nextFree = alloc;

        if (prevPrev != NULL)
        {

            prevPrev->nextFree = alloc;
        }

        // returning the reallocated pointer
        return previous + sizeof(HeapCell);
    }
    else if (previous != NULL && previous->free == 1 && previous->size + alloc->size == size)
    {
        memcpy(previous + sizeof(HeapCell), alloc + sizeof(HeapCell), alloc->size - sizeof(HeapCell));

        previous->size = size;
        previous->free = 0;
        previous->next = alloc->next;
        previous->next->prev = previous;
        return previous + sizeof(HeapCell);
    }

    // Allocate new block of memory
    HeapCell *newAlloc = mymalloc(size);
    if (newAlloc == NULL)
    {
        return NULL;
    }
    memcpy(newAlloc + sizeof(HeapCell), alloc + sizeof(HeapCell), alloc->size - sizeof(HeapCell));
    myfree(alloc);
    return newAlloc;
}

void mycleanup()
{
    free(heap);
};