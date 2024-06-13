#include <cstdlib>
#include <iostream>
#include <stddef.h>
#include <thread>
#include <vector>

#include "alloc.h"
#include "benchmark.h"
#include "linked_list.h"

void test_basic_alloc_dealloc(Allocator& a) {
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

    a.dealloc(mem1);
    a.dealloc(mem2);


    Node<Block*>* current = a.get_free_list().get_head();
    while (current) {
        Block* block = current->data;
        current = current->next;
    }
}

void test_multi_alloc_dealloc(Allocator& allocator) {
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

    allocator.dealloc(mem1);
    allocator.dealloc(mem3);

    Node<Block*>* current = allocator.get_free_list().get_head();
    while (current) {
        Block* block = current->data;
        current = current->next;
    }

    allocator.dealloc(mem2);
    allocator.dealloc(mem4);

    while (current) {
        Block* block = current->data;
        current = current->next;
    }
}

void stress_test(Allocator& allocator, size_t num_operations, size_t max_block_size) {
    std::cout << "Starting stress test...\n";
    std::srand(std::time(nullptr));

    std::vector<void*> allocated_blocks;

    for (size_t i = 0; i < num_operations; ++i) {
        if (std::rand() % 2 == 0) {
            size_t size = (std::rand() % max_block_size) + 1;
            void* mem = allocator.alloc(size);
            if (mem) {
                allocated_blocks.push_back(mem);
                std::fill_n(static_cast<char*>(mem), size, 0xEE);
            }
        } else if (!allocated_blocks.empty()) {
            size_t index = std::rand() % allocated_blocks.size();
            allocator.dealloc(allocated_blocks[index]);
            allocated_blocks.erase(allocated_blocks.begin() + index);
        }
    }

    for (void* mem : allocated_blocks) {
        allocator.dealloc(mem);
    }

    Node<Block*>* current = allocator.get_free_list().get_head();
    while (current) {
        Block* block = current->data;
        current = current->next;
    }
}

void alloc_wrap(Allocator& allocator, size_t size, int8_t allocations) {
    for (int i = 0; i < allocations; ++i) {
        void* mem = allocator.alloc(size);
        allocator.dealloc(mem);
    }
}

void test_multithreaded(Allocator& allocator, size_t size, int8_t allocations) {
    std::vector<std::thread> threads;

    for (int i = 0; i < allocations; ++i) {
        threads.emplace_back(alloc_wrap, std::ref(allocator), size, allocations);
    }

    for (auto& t : threads) {
        t.join();
    }
}

int main() {
    try {
        Allocator allocator;

        test_basic_alloc_dealloc(allocator);
        test_multi_alloc_dealloc(allocator);
        stress_test(allocator, 1000, 1024 * 10);
        test_multithreaded(allocator, 16, 100);

        benchmark(allocator, 8, 100);
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
