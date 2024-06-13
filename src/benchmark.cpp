#include <chrono>
#include <iostream>
#include <stddef.h>
#include <thread>
#include <vector>

#include "alloc.h"
#include "block.h"
 
void benchmark_standard_alloc(size_t size, int16_t allocations) {
    const auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < allocations; ++i) {
        void* mem = malloc(size);
        free(mem);
    }
    const auto end = std::chrono::steady_clock::now();
    std::cout << "[STANDARD] Time elapsed for " << allocations << " allocations: " <<  (end-start).count() << "\n";
}

void benchmark_alloc(Allocator& allocator, size_t size, int16_t allocations) {
    const auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < allocations; ++i) {
        void* mem = allocator.alloc(size);
        allocator.dealloc(mem);
    }
    const auto end = std::chrono::steady_clock::now();
    std::cout << "[MYALLOC] Time elapsed for " << allocations << " allocations: " <<  (end-start).count() << "\n";
}

void benchmark(Allocator& allocator, size_t size, int16_t allocations) {
    std::thread standard_thread(benchmark_standard_alloc, size, allocations);
    std::thread custom_thread(benchmark_alloc, std::ref(allocator), size, allocations);

    standard_thread.join();
    custom_thread.join();

    std::cout << "Benchmark Complete\n";
}