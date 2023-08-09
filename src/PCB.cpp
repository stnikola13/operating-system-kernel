#include "../h/PCB.hpp"

// Napomena: Kod ove klase je vecinskim delom preuzet sa 7. vezbi - sinhrona promena konteksta.

PCB* PCB::running = nullptr;
uint64 PCB::timeSliceCounter = 0;

void* PCB::operator new(size_t size) {                                      // Preklopljeni operator za alokaciju.
    size_t adjSize = 0;
    if (size % MEM_BLOCK_SIZE == 0) adjSize = size / MEM_BLOCK_SIZE;        // Zaokruzivanje bajtova na blokove.
    else adjSize = (size / MEM_BLOCK_SIZE) + 1;
    return MemoryAllocator::get()->mem_alloc(adjSize);
}

void PCB::operator delete(void* ptr) {                                      // Preklopljeni operator za dealokaciju.
    MemoryAllocator::get()->mem_free(ptr);
}

PCB::PCB(Body body, void* args, uint64* stack) : context({0, 0}) {
    this->body = body;                                                      // Koristi se za kreiranje aktivnih niti
    this->stack = stack;                                                    // koje se pokrecu odmah po kreiranju.
    this->args = args;                                                      // Cuvaju se telo f-je, argumenti i stek.
    this->finished = false;                                                 // Flag oznacava da nije zavrsila sa radom.
    this->started = false;                                                  // Nit nije startovana dok se ne uradi start.
    this->blocked = false;                                                  // Nit nije blokirana na pocetku.
    this->error = false;                                                    // Nit nema nikakvu gresku na pocetku.
    this->timeSlice = DEFAULT_TIME_SLICE;                                   // Nit dobija podrazumevani odsecak vremena.

    uint64 sp = 0;
    uint64 ra = 0;                                                          // Povratna adresa je adresa wrapperFunc
    if (body != nullptr) ra = (uint64)wrapperFunction;                      // funkcije omotaca koja poziva body funkc.
    if (stack != nullptr) sp = (uint64)&stack[DEFAULT_STACK_SIZE];          // Stack pointer je pokazivac na prvu
    context = {ra, sp};                                                     // lokaciju iza kraja steka.

    if (body != nullptr) Scheduler::put(this);                          // Posto je aktivna, stavlja se u Scheduler.
}

PCB::PCB(Body body, void* args) : context({0, 0}) {                  // Koristi se za kreiranje neaktivne niti
    this->body = body;                                                      // koja je samo napravljena, ne i pokrenuta.
    this->stack = nullptr;                                                  // Samo se cuvaju telo f-je i argumenti,
    this->args = args;                                                      // ostale vrednosti su 0/null. Postavljen
    this->finished = true;                                                  // je flag kao da je zavrsila sa radom.
    this->started = false;                                                  // Nit nije startovana dok se ne uradi start.
    this->blocked = false;                                                  // Nit nije blokirana na pocetku.
    this->error = false;                                                    // Nit nema nikakvu gresku na pocetku.
    this->timeSlice = DEFAULT_TIME_SLICE;                                   // Nit dobija podrazumevani odsecak vremena.
}

void PCB::yield() {
    pushRegisters();                                                        // Cuva kontekst u vidu registara.
    dispatch();                                                             // Menja kontekst.
    popRegisters();                                                         // Restaurira nov kontekst u vidu registara.
}

void PCB::kill() {                                                          // Kada nit treba da se unisti, samo joj
    this->finished = true;                                                  // se postavlja finished flag i automatski
    PCB::yield();                                                           // kad se pozove yield nece biti stavljena
}                                                                           // u Scheduler i nece dobiti procesor vise.

void PCB::dispatch() {
    PCB *oldPCB = running;                                                  // Tekuca nit postaje oldPCB.

    //if (!(oldPCB->finished || oldPCB->blocked || oldPCB->error))
    if (!(oldPCB->finished || oldPCB->blocked))                             // Ukoliko ona nije zavrsila svoj posao,
        Scheduler::put(oldPCB);                                         // stavlja se na cekanje u Scheduler.
    running = Scheduler::get();                                             // Iz Schedulera se dohvata spremna nit.

    PCB::contextSwitch(&oldPCB->context,                          // Menja se kontekst sa stare na novu nit.
                       &running->context);
}

PCB* PCB::createInactiveThread(Body body, void *args) {                     // Kreira neaktivnu nit (bez steka).
    PCB *pcb = new PCB(body, args);
    return pcb;
}

PCB* PCB::createActiveThread(Body body, void* args, uint64* stack) {        // Kreira aktivnu nit (sa stekom).
    PCB *pcb = new PCB(body, args, stack);
    return pcb;
}

void PCB::wrapperFunction() {                                               // Staticka funkcija omotac koja omogucava
    PCB* thread = running;                                                  // da se dodeljena funckija pozove sa
    Body func = thread->body;                                               // argumentima tako sto za tekucu nit poziva
    void* callArgs = thread->args;                                          // funkciju sa njenim argumentima. Nakon
    func(callArgs);                                                         // povratka iz pozvane funkcije postavlja se
    running->finished = true;                                               // finished flag i prebacuje se kontekst
    PCB::yield();                                                           // na drugu nit.
}


