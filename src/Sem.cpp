#include "../h/Sem.hpp"

void* Sem::operator new(size_t size) {                                      // Preklopljeni operator za alokaciju.
    size_t adjSize = 0;
    if (size % MEM_BLOCK_SIZE == 0) adjSize = size / MEM_BLOCK_SIZE;        // Zaokruzivanje bajtova na blokove.
    else adjSize = (size / MEM_BLOCK_SIZE) + 1;
    return MemoryAllocator::get()->mem_alloc(adjSize);
}

void Sem::operator delete(void* ptr) {                                      // Preklopljeni operator za dealokaciju.
    MemoryAllocator::get()->mem_free(ptr);
}

Sem* Sem::createSemaphore(int initSemValue) {                               // Staticka metoda za eksplicitno
    return new Sem(initSemValue);                                     // kreiranje semafora.
}

void Sem::block() {                                                         // Metoda za blokiranje koja tekucu nit
    PCB* pcb = PCB::running;                                                // blokira (postalvja blocked flag), dodaje
    pcb->setBlocked(true);                                         // je u listu blokiranih i poziva yield
    blocked->addLast(pcb);                                             // tako da druga nit dobije procesor.
    PCB::yield();
}

void Sem::deblock() {                                                       // Metoda za deblokiranje koja uzima prvu
    PCB* pcb = blocked->removeFirst();                                      // nit iz reda blokiranih, deblokira je
    pcb->setBlocked(false);                                        // (uklanja joj blocked flag) i stavlja je
    Scheduler::put(pcb);                                                   // u Scheduler (nece odmah biti pokrenuta
}                                                                          // vec mora da saceka svoj red).

Sem::Sem(int value) {                                                       // Konstruktor koji inicijalizuje value
    val = value;                                                            // i alocira prostor za red blokiranih niti.
    blocked = new List<PCB>();
}

Sem::~Sem() {                                                               // Destruktor koji uklanja niti iz reda
    PCB* pcb = blocked->removeFirst();                                      // blokiranih, jednu po jednu, uklanjajuci
    while (pcb != nullptr) {                                                // im status blokiranosti, i stavlja ih
        pcb->setBlocked(false);                                     // u red spremnih.
        pcb->setError(true);
        Scheduler::put(pcb);
        pcb = blocked->removeFirst();
    }
    delete blocked;                                                         // Na kraju dealocira memoriju za red.
}

void Sem::wait() {                                                          // Prema definiciji operacije wait().
    val--;
    if (val < 0) block();
}

void Sem::signal() {                                                        // Prema definiciji operacije signal().
    if (val < 0) deblock();
    val++;
}

