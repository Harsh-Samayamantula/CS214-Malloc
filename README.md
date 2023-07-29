# Custom Memory Allocation (Malloc) in C
CS214 Malloc Implementation

## Description
This project involves the implementation of a custom version of `malloc`, `free`, and `realloc` functions in C. The library, `mymalloc`, allows users to manage dynamic memory allocation in a custom heap. The main objective is to provide memory management functionality similar to the standard `malloc` library functions.

## Features
- `myinit`: Initializes the custom heap and performs any necessary initializations. Able to provide memory allocation based on the following allocaiton algorithms:
    - 0: First Fit
    - 1: Next Fit
    - 2: Best Fit
- `mymalloc`: Allocates a contiguous region of memory of at least the requested size (if possible) from the custom heap and is guaranteed to be 8-byte aligned.  The returned address is guaranteed to be 8-byte aligned.
- `myfree`: Frees the memory region previously allocated by mymalloc. The freed region becomes available for future allocations and should be coalesced with adjacent free regions.
- `myrealloc`: Resizes the memory region pointed to by ptr to be at least the new given size. If it is not possible to resize in place, a new region is allocated, and the data from the original region is copied over. The old region is then freed.
- `mycleanup`: Frees the entire custom heap and performs any necessary cleanup.

## Authors
Harshith Samayamantula  
Adam Trabelsi
