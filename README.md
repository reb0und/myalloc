# myalloc

A memory allocator using best fit alllocation implemented in C++.

```sh
sh run.sh
```

### Allocating memory

```cpp
void* allocated_memory = allocator.alloc(sizeof(std::string));
```

### Deallocating memory

```cpp
allocator.deallocate(allocated_memory);
```

### Process

- Create memory pool of 1 MB
- Initialize free list (list of all free blocks)
- Return best fit block based on size
- If block is greater than or equal to the requested size + the size of the block's metadata, split the block into two parts (one to satisfy allocation request and other remains in free list)
- alloc() returns pointer to block + size of block's metadata
- dealloc() takes pointer to an allocated block, converts it back to a pointer to a block, checks if the next block is free using pointer arithmetic (pointer to block + adding size of Block + the block's metadata = address of next block), check if the next block is within the memory pools bounds, check if the next block is free, coalesce the blocks if so, check and coalesce previous block if free
- Test cases

# Notes

Apologies as this fairly poorly documented, my intention was mainly to share my work. Additionally, this allocator is significantly slower than malloc.
