#ifndef BLOCK_H
#define BLOCK_H

#include <new>
#include <stddef.h>

#include "linked_list.h"


class Block {
    private:
        size_t size;
        bool status;
        Block* next;
    public:
        Block(size_t size) : size(size), status(false), next(0) {};
        ~Block() {};

        size_t get_size() const {
            if (!size) {
                std::cerr << "size is null\n";
                exit(-1);
            }
            return size;
        }

        void set_size(size_t newSize) {
            size = newSize;
        }

        bool get_status() {
            return status;
        }

        void set_status(bool status) {
            status = status;
        }

        Block* get_next() const {
            return next;
        }

        void set_next(Block* b) {
            next = b;
        }

        // check if block is large enough to be split into two
        Block* split(LinkedList<Block*>* free_list, size_t req_size) {
            // block size is large enough to accomodate requested size + block metadata
            if (size >= req_size + sizeof(Block)) {
                // calculate addr of new block within current block
                void* new_block_addr = reinterpret_cast<char*>(this) + sizeof(Block) + req_size;
                size_t new_block_size = size - req_size - sizeof(Block);
                size = req_size;
                Block* new_block = new (new_block_addr) Block(new_block_size);

                new_block->set_next(next);
                next = new_block;
                
                free_list->insertAtBeginning(new_block);

                return new_block;
            }

            return 0;
        }
};

#endif