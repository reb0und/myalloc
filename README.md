# myalloc

A memory allocator using best fit alllocation implemented in C++.

```sh
sh run.sh
```

### Allocating memory

```cpp
void* allocated_memory = allocator.alloc(1024);
```

### Deallocating memory

```cpp
allocator.deallocate(allocated_memory);
```
