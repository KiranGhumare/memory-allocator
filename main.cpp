// create a class/struct to store the data that we get from OS

#include "pool.h"
#include <cstddef>
#include <algorithm>
#include <sys/mman.h>
#include <iostream>

struct MemoryPool {
    void* buffer;
    BlockHeader* head;
};

constexpr size_t BLOCK_HEADER_SIZE = sizeof(BlockHeader);

size_t calculateMemoryPoolSize(int numBlocks, size_t blockSize) {
    size_t requestedSize = std::max(blockSize, BLOCK_HEADER_SIZE);
    return requestedSize*numBlocks;
}

MemoryPool* initializeMemoryPool(size_t blockSize, int numBlocks) {
    MemoryPool* pool = new MemoryPool();
    size_t totalMemorySize = calculateMemoryPoolSize(numBlocks, blockSize);
    // call OS to get memory
    void* buffer = mmap(nullptr, totalMemorySize, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if (buffer == MAP_FAILED) {
        delete pool;
        return nullptr;
    }
    pool->buffer = buffer;
    // divide memory block into size of blockSize
    // create free list
    // point head at the first block
    size_t requestedSize = std::max(blockSize, BLOCK_HEADER_SIZE);
    pool->head = static_cast<BlockHeader*>(buffer);
    BlockHeader* temp = pool->head;
    char* current = static_cast<char*>(buffer);
    for (int i=1;i<numBlocks;i++) {
        temp->next = static_cast<BlockHeader*>(static_cast<void*>(current + requestedSize));        
        current+=requestedSize;
        temp = temp->next;
    }
    temp->next = nullptr;
    return pool;
}

BlockHeader* allocateMemory(MemoryPool* pool) {
    if (pool->head==nullptr) {
        std::cout<<"Memory exhausted!"<<std::endl;
        return nullptr;
    }
    BlockHeader* temp = pool->head;
    pool->head = pool->head->next;
    return temp;
}

void freeMemory(MemoryPool* pool, BlockHeader* obj) {
    BlockHeader* temp = pool->head;
    pool->head = obj;
    obj->next = temp;
    return;
}

int main() {
    int numBlocks = 10;
    size_t blockSize = 8;
    MemoryPool* pool = initializeMemoryPool(blockSize, numBlocks);
    std::cout<<pool->head<<std::endl;
    BlockHeader* obj = allocateMemory(pool);
    BlockHeader* obj1 = allocateMemory(pool);
    BlockHeader* obj2 = allocateMemory(pool);
    std::cout<<obj<<std::endl;
    BlockHeader* temp1 = pool->head;
    int count = 0;
    while (temp1!=nullptr) {
        count++;
        temp1 = temp1->next;
    }
    std::cout<<count<<std::endl;
    count = 0;
    // freeMemory(pool, obj, blockSize, 2);
    temp1 = pool->head;
    while (temp1!=nullptr) {
        count++;
        temp1 = temp1->next;
    }
    std::cout<<count<<std::endl;
    return 0;
} 