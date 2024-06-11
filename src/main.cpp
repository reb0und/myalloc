#include <cstdlib>
#include <iostream>
#include <stddef.h>
#include <vector>

#include "alloc.h"
#include "linked_list.h"

void test_basic_alloc_dealloc(Allocator& a) {
    std::cout << "Testing basic allocation and deallocation..." << std::endl;
    void* mem1 = a.alloc(256 * 1024);
    if (!mem1) {
        throw std::bad_alloc();
    }
    
    // fill allocated mem with data
    std::fill_n(static_cast<char*>(mem1), 256 * 1024, 0xAA);

    void* mem2 = a.alloc(128 * 1024);
    if (!mem2) {
        throw std::bad_alloc();
    }
    std::fill_n(static_cast<char*>(mem2), 128 * 1024, 0xBB);

    std::cout << "Allocated 256 KB and 128 KB blocks." << std::endl;

    a.dealloc(mem1);
    a.dealloc(mem2);

    std::cout << "Deallocated 256 KB and 128 KB blocks." << std::endl;

    Node<Block*>* current = a.get_free_list().get_head();
    while (current) {
        Block* block = current->data;
        std::cout << "Block in free list - Size: " << block->get_size() << ", Free: " << block->get_status() << std::endl;
        current = current->next;
    }
}

void test_multi_alloc_dealloc(Allocator& allocator) {
    std::cout << "Testing multiple allocations and deallocations..." << std::endl;
    void* mem1 = allocator.alloc(64 * 1024);
    void* mem2 = allocator.alloc(32 * 1024);
    void* mem3 = allocator.alloc(16 * 1024);
    void* mem4 = allocator.alloc(8 * 1024);

    if (!mem1 || !mem2 || !mem3 || !mem4) {
        throw std::bad_alloc();
    }

    std::fill_n(static_cast<char*>(mem1), 64 * 1024, 0xAA);
    std::fill_n(static_cast<char*>(mem2), 32 * 1024, 0xBB);
    std::fill_n(static_cast<char*>(mem3), 16 * 1024, 0xCC);
    std::fill_n(static_cast<char*>(mem4), 8 * 1024, 0xDD);

    std::cout << "Allocated 64 KB, 32 KB, 16 KB, and 8 KB blocks." << std::endl;

    allocator.dealloc(mem1);
    allocator.dealloc(mem3);

    std::cout << "Deallocated 64 KB and 16 KB blocks." << std::endl;

    Node<Block*>* current = allocator.get_free_list().get_head();
    while (current) {
        Block* block = current->data;
        std::cout << "Block in free list - Size: " << block->get_size() << ", Free: " << block->get_status() << std::endl;
        current = current->next;
    }

    allocator.dealloc(mem2);
    allocator.dealloc(mem4);

    std::cout << "Deallocated 32 KB and 8 KB blocks." << std::endl;

    while (current) {
        Block* block = current->data;
        std::cout << "Block in free list - Size: " << block->get_size() << ", Free: " << block->get_status() << std::endl;
        current = current->next;
    }
}

void stress_test(Allocator& allocator, size_t num_operations, size_t max_block_size) {
    std::cout << "Starting stress test..." << std::endl;
    std::srand(std::time(nullptr));

    std::vector<void*> allocated_blocks;

    for (size_t i = 0; i < num_operations; ++i) {
        if (std::rand() % 2 == 0) {
            size_t size = (std::rand() % max_block_size) + 1;
            void* mem = allocator.alloc(size);
            if (mem) {
                allocated_blocks.push_back(mem);
                std::fill_n(static_cast<char*>(mem), size, 0xEE);
                std::cout << "Allocated block of size " << size << " bytes." << std::endl;
            }
        } else if (!allocated_blocks.empty()) {
            size_t index = std::rand() % allocated_blocks.size();
            allocator.dealloc(allocated_blocks[index]);
            std::cout << "Deallocated block of size " << index << " bytes." << std::endl;
            allocated_blocks.erase(allocated_blocks.begin() + index);
        }
    }

    for (void* mem : allocated_blocks) {
        allocator.dealloc(mem);
    }

    std::cout << "Stress test completed." << std::endl;

    Node<Block*>* current = allocator.get_free_list().get_head();
    while (current) {
        Block* block = current->data;
        std::cout << "Block in free list - Size: " << block->get_size() << ", Free: " << block->get_status() << std::endl;
        current = current->next;
    }
}

int main() {
    try {
        Allocator allocator;

        test_basic_alloc_dealloc(allocator);
        test_multi_alloc_dealloc(allocator);
        stress_test(allocator, 1000, 1024 * 10);

    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
