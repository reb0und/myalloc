#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <stddef.h>

#include "alloc.h"

void benchmark(Allocator& allocator, size_t size, int16_t allocations);
void benchmark_standard_alloc(size_t size, int16_t allocations);
void benchmark_alloc(Allocator& allocator, size_t size, int16_t allocations);

#endif