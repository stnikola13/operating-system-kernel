#ifndef _MemoryAllocator_hpp_
#define _MemoryAllocator_hpp_

#include "../lib/hw.h"

class MemoryAllocator {
private:
    struct BlockHeader {                                                    // Struktura koja se koristi za obelezavanje
        BlockHeader* next;                                                  // slobodnog prostora (jedan slob. segment
        size_t size;                                                        // cini ovo zaglavlje i iza njega pravi
    };                                                                      // slobodan prostor i tako u krug).

    static bool initialized;
    static MemoryAllocator* singleton;
    static BlockHeader* freeMemHead;

    // Svi konstruktori i operatori dodele su u privatnoj sekciji jer je u pitanju Singleton kl. sa samo jednim objektom.
    MemoryAllocator() = default;
    MemoryAllocator(const MemoryAllocator&) = default;
    MemoryAllocator(MemoryAllocator&&) = default;
    ~MemoryAllocator() = default;
    MemoryAllocator& operator=(const MemoryAllocator&) = default;
    MemoryAllocator& operator=(MemoryAllocator&&) = default;

public:
    // Napomena: Funkcija je velikim delom prekopirana iz 1. zadatka na kolokvijumu iz avgusta 2021.
    void* mem_alloc (size_t size);

    // Napomena: Funkcija je velikim delom prekopirana iz 2. zadatka na kolokvijumu iz septembra 2015.
    int mem_free (void* ptr);

    // Na osnovu definicije Singleton klase.
    static MemoryAllocator* get();
};

#endif