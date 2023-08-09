#include "../h/MemoryAllocator.hpp"

bool MemoryAllocator::initialized = false;
MemoryAllocator* MemoryAllocator::singleton;                                // Potencijalno = nullptr;
MemoryAllocator::BlockHeader* MemoryAllocator::freeMemHead;

// Napomena: Funkcija je velikim delom prekopirana iz 1. zadatka na kolokvijumu iz avgusta 2021.

void* MemoryAllocator::mem_alloc (size_t size) {                            // Prosledjuje se broj blokova koji se
    size_t adjSize = size * MEM_BLOCK_SIZE;                                 // alocira (size), pa se ovde to pretvara u
    //adjSize = adjSize + sizeof(BlockHeader);                              // broj bajtova (adjSize).
    BlockHeader* blk = freeMemHead;
    BlockHeader* prev = nullptr;

    while (blk != nullptr) {                                                // Po first-fit algoritmu se trazi prvi
        if (blk->size >= adjSize) break;                                    // element liste (blok) koji je slobodan, a
        prev = blk;                                                         // dovoljno velik.
        blk = blk->next;
    }

    if (blk == nullptr) return nullptr;                                     // Ako takav blok ne postoji, vraca se null
                                                                            // posto nema dovoljno slobodnog mesta.
    size_t remainingSize = blk->size - adjSize;
    if (remainingSize >= sizeof(BlockHeader) + MEM_BLOCK_SIZE) {            // U suprotnom se alocira blok.
        blk->size = adjSize;                                                // Ako preostala velicina ima prostora
        size_t offset = sizeof(BlockHeader) + adjSize;                      // za bar jos jedan blok, odvaja se taj deo.
        BlockHeader* newBlk = (BlockHeader*)((char*)blk + offset);
        if (prev) prev->next = newBlk;
        else freeMemHead = newBlk;
        newBlk->next = blk->next;
        newBlk->size = remainingSize - sizeof(BlockHeader);
    }
    else {                                                                  // Ako nema mesta ni za jedan blok preostalo,
        if (prev) prev->next = blk->next;                                   // alocira se ceo deo, da bi se smanjila
        else freeMemHead = blk->next;                                       // interna fragmentacija.
    }
    blk->next = nullptr;
    return (void*)((char*)blk + sizeof(BlockHeader));
    //return (void*)blk;                                                    // Potencijalno ako ovo gore ne bude radilo.
}


// Napomena: Funkcija je velikim delom prekopirana iz 2. zadatka na kolokvijumu iz septembra 2015.

int MemoryAllocator::mem_free (void* ptr) {                                 // U sustini svugde se dodaje sada sizeof
    if (ptr == nullptr) return -1;                                          // (BlockHeader) posto je inicijalno pokaz.
                                                                            // ptr pomeren unazad za sizeof(BlockHeader)
                                                                            // kako bi pokazivao na pocetak zaglavlja
    ptr = (void*)((char*)ptr - sizeof(BlockHeader));                        // starog slobodnog segmenta. Pozicioniramo
    BlockHeader *cur = nullptr;                                             // pokazivac da pokazuje na pocetak hedera.
    if (freeMemHead == nullptr || ptr < (void*)freeMemHead) cur = nullptr;  // TODO: Ovo mora (pogledaj beleske).
    else {
        for (cur = freeMemHead; cur->next != nullptr &&                     // Trazimo najblizi slobodni
        ptr>(void*)(cur->next); cur = cur->next);                           // segment pokazivacu.
    }

    if (cur && ((char*)cur + sizeof(BlockHeader) + cur->size) == ptr) {     // Pokusaj nadovezivanja na prethodni
        cur->size += ((BlockHeader*)ptr)->size + sizeof(BlockHeader);       // slobodni segment. Ranije bilo
        if (cur->next && ((char*)cur + sizeof(BlockHeader)                  // if (cur && ((char*)cur+cur->size)==ptr).
        + cur->size) == (char*)cur->next) {                                 // Potencijalno ukloniti svuda
            cur->size += cur->next->size + sizeof(BlockHeader);             // BlockHeader ako bude pravilo probleme.
            cur->next = cur->next->next;                                    // Onda se pokusava spajanje sa sledecim.
        }
        //ptr = nullptr;
        return 0;
    }
    else {                                                                  // Pokusaj nadovezivanja na sledeci segment.
        BlockHeader* nextSeg = nullptr;
        if (cur) nextSeg = cur->next;
        else nextSeg = freeMemHead;

        if (nextSeg && ((char*)ptr + sizeof(BlockHeader) +
        ((BlockHeader*)ptr)->size) == (char*)nextSeg) {                      // Potencijalno ukloniti svuda
            BlockHeader* newSeg = (BlockHeader*)ptr;                         // BlockHeader ako bude pravilo probleme.
            newSeg->size = nextSeg->size +
                    ((BlockHeader*)ptr)->size + sizeof(BlockHeader);
            newSeg->next = nextSeg->next;
            //ptr = nullptr;
            return 0;
        }
        else {                                                               // Iza trenutnog (cur) ubacujemo
            BlockHeader* newSeg = (BlockHeader*)ptr;                         // ovaj slobodni segment.
            newSeg->size = ((BlockHeader*)ptr)->size;
            if (cur) newSeg->next = cur->next;
            else newSeg->next = freeMemHead;
            if (cur) cur->next = newSeg;
            else freeMemHead = newSeg;
            //ptr = nullptr;
            return 0;
        }
    }
}


// Na osnovu definicije Singleton klase.

MemoryAllocator* MemoryAllocator::get() {
    if (!initialized) {                                                      // Prvi put prilikom dohvatanja se
        initialized = true;                                                  // vrsi inicijalizacija. Prvo mesto
        MemoryAllocator::freeMemHead = (BlockHeader*)HEAP_START_ADDR;        // na hipu ce zauzimati zaglavlje
        MemoryAllocator::freeMemHead->size = size_t((char*)(HEAP_END_ADDR) - // freeMemHead. Odmah nakon njega
                (char*)(HEAP_START_ADDR) - sizeof(BlockHeader));             // krece slobodan prostor velicine
        MemoryAllocator::freeMemHead->next = nullptr;                        // ukupnog prostora na hipu od kog
    }                                                                        // je oduzeta velicina zaglavlja.
    return singleton;
}