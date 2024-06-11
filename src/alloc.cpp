#include <iostream>
#include <stddef.h>
#include <stdexcept>

#include "alloc.h"
#include "block.h"
#include "linked_list.h"

const size_t POOL_SIZE = 1024 * 1024;

Allocator::Allocator() {
    mem_pool = malloc(POOL_SIZE);
    if (!mem_pool) {
        throw std::runtime_error("Memory pool initial allocation failed");
    }
    Block* initial_block = new (mem_pool) Block(POOL_SIZE-sizeof(Block));
    free_list.insertAtBeginning(initial_block);
}


Allocator::~Allocator() {
    if(mem_pool) {
        free(mem_pool);
        mem_pool = 0;
    }
}

// returns address to usable memory after allocated block after block's metadat
void* Allocator::alloc(size_t size) {
    Node<Block*>* current_node = free_list.get_head();
    Node<Block*>* prev_node = 0;
    Node<Block*>* best_node = 0;
    Node<Block*>* prev_best_node = 0;
    
    // traverse free_list for best fit block
    while (current_node) {
        Block* b = current_node->data;
        if (b == 0) {
            std::cerr << "Error: Current block is null\n";
            break;
        }
        
        if(b->get_size() >= size && (!best_node || b->get_size() < best_node->data->get_size())) {
            prev_node = best_node;
            best_node = current_node;
        }
        prev_node = current_node;
        current_node = current_node->next;
    }

    if (!best_node) {
        return 0;
    }

    Block* best_block = best_node->data;

    // check if splitting block is necessary
    if (best_block->get_size() > size + sizeof(Block)) {
        best_block->split(&free_list, size);
    } else {
        if (prev_best_node) {
            prev_node->data->set_next(best_block->get_next());
        } else {
            free_list.set_head(best_node);
        }
    }
    
    best_block->set_status(false);

    // convert pointer to block to pointer of type void and pad addr to account for block's metadata
    return reinterpret_cast<void*>(reinterpret_cast<char*>(best_block) + sizeof(Block));
}

void Allocator::dealloc(void *ptr) {
    if (!ptr) return;

    Block* block = reinterpret_cast<Block*>(reinterpret_cast<char*>(ptr) - sizeof(Block));
    block->set_status(true);

    // check if the next block is free and coalesce
    Block* next_block = reinterpret_cast<Block*>(reinterpret_cast<char*>(block) + sizeof(Block) + block->get_size());
    if (reinterpret_cast<char*>(next_block) < reinterpret_cast<char*>(mem_pool) + POOL_SIZE && next_block->get_status()) {
        block->set_size(block->get_size() + sizeof(Block) + next_block->get_size());
        block->set_next(next_block->get_next());
    }

    Node<Block*>* current_node = free_list.get_head();
    Block* prev_block = nullptr;

    while (current_node) {
        Block* b = current_node->data;
        if (reinterpret_cast<char*>(b) + sizeof(Block) + b->get_size() == reinterpret_cast<char*>(block)) {
            prev_block = b;
            break;
        }
        current_node = current_node->next;
    }

    if (prev_block && prev_block->get_status()) {
        prev_block->set_size(prev_block->get_size() + sizeof(Block) + block->get_size());
        prev_block->set_next(block->get_next());
    } else {
        free_list.insertAtBeginning(block);
    }
}

LinkedList<Block*>& Allocator::get_free_list() {
    return free_list;
}