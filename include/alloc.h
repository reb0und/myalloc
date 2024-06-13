#ifndef ALLOC_H
#define ALLOC_H

#include <mutex>
#include <stddef.h>

#include "block.h"
#include "linked_list.h"

class Allocator {
    private:
        void* mem_pool;
        LinkedList<Block*> free_list;
        std::mutex mtx;
    public:
        Allocator();
        ~Allocator();
        
        void* alloc(size_t size);
        void dealloc(void* ptr);
        LinkedList<Block*>& get_free_list();
};

#endif