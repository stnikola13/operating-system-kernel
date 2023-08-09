#include "../h/syscall_c.hpp"

void* mem_alloc(size_t size) {                              // Prosledjuje se najmanji broj bajtova koji treba alocirati.
    size_t adjSize = 0;
    uint64 code = 0x01;
    void* ptr = nullptr;

    if (size % MEM_BLOCK_SIZE == 0) adjSize = size / MEM_BLOCK_SIZE;    // Poravnanje bajtova na blokove.
    else adjSize = (size / MEM_BLOCK_SIZE) + 1;                         // adjSize je broj blokova koji se alocira.

    asm volatile("mv a1, %0" : : "r" (adjSize));            // U reg. a1 se stavlja adjSize (param. sistemskog poziva).
    asm volatile("mv a0, %0" : : "r" (code));               // Broj sistemskog poziva se stavlja u registar a0.
    asm volatile("ecall");                                  // Sistemski poziv.
    asm volatile("mv %0, a0" : "=r" (ptr));                 // Povratnu vrednost poziva stavljamo u pokazivac ptr.

    return ptr;                                             // Vracamo pokazivac koji je vracen iz sistemskog poziva.
}

int mem_free(void* ptr) {
    uint64 code = 0x02;
    int status = 0;

    asm volatile("mv a1, %0" : : "r" (ptr));                // U registar a1 se stavlja ptr (param. sistemskog poziva).
    asm volatile("mv a0, %0" : : "r" (code));               // Broj sistemskog poziva se stavlja u registar a0.
    asm volatile("ecall");                                  // Sistemski poziv.
    asm volatile("mv %0, a0" : "=r" (status));              // Povratnu vrednost poziva stavljamo u promenljivu status.

    return status;                                          // Vracamo status koji je vracen iz sistemskog poziva.
}

int thread_create(thread_t* handle, void(*start_routine)(void*), void* arg) {
    uint64 code = 0x11;
    int status = 0;
    uint64* stack = nullptr;
    if (start_routine != nullptr) stack = (uint64*)mem_alloc(DEFAULT_STACK_SIZE * sizeof(uint64));
    if (start_routine != nullptr && stack == nullptr) return -1;

    asm volatile("mv a4, %0" : : "r" (stack));              // Argumente pakujemo od visih ka nizim registrima, a
    asm volatile("mv a3, %0" : : "r" (arg));                // posle u prekidnoj rutini raspakujemo od nizih ka
    asm volatile("mv a2, %0" : : "r" (start_routine));      // visim registrima (C-ovska konvencija desno-levo).
    asm volatile("mv a1, %0" : : "r" (handle));
    asm volatile("mv a0, %0" : : "r" (code));               // Broj sistemskog poziva se stavlja u registar a0.
    asm volatile("ecall");                                  // Sistemski poziv.
    asm volatile("mv %0, a0" : "=r" (status));              // Povratnu vrednost poziva stavljamo u promenljivu status.

    return status;
}

int thread_exit() {
    uint64 code = 0x12;
    int status = 0;

    asm volatile("mv a0, %0" : : "r" (code));               // Broj sistemskog poziva se stavlja u registar a0.
    asm volatile("ecall");                                  // Sistemski poziv.
    asm volatile("mv %0, a0" : "=r" (status));              // Povratnu vrednost poziva stavljamo u promenljivu status.

    return status;
}

void thread_dispatch() {
    uint64 code = 0x13;

    asm volatile("mv a0, %0" : : "r" (code));               // Broj sistemskog poziva se stavlja u registar a0.
    asm volatile("ecall");                                  // Sistemski poziv.

    return;
}

int sem_open(sem_t* handle, unsigned init) {
    uint64 code = 0x21;
    int status = 0;

    asm volatile("mv a2, %0" : : "r" (init));               // Pakuju se pocetna vrednost i rucka semafora.
    asm volatile("mv a1, %0" : : "r" (handle));
    asm volatile("mv a0, %0" : : "r" (code));               // Broj sistemskog poziva se stavlja u registar a0.
    asm volatile("ecall");                                  // Sistemski poziv.
    asm volatile("mv %0, a0" : "=r" (status));              // Povratnu vrednost poziva stavljamo u promenljivu status.

    return status;
}

int sem_close(sem_t handle) {
    uint64 code = 0x22;
    int status = 0;

    asm volatile("mv a1, %0" : : "r" (handle));             // Pakuje se rucka semafora kao argument.
    asm volatile("mv a0, %0" : : "r" (code));               // Broj sistemskog poziva se stavlja u registar a0.
    asm volatile("ecall");                                  // Sistemski poziv.
    asm volatile("mv %0, a0" : "=r" (status));              // Povratnu vrednost poziva stavljamo u promenljivu status.

    return status;
}

int sem_wait(sem_t id) {
    uint64 code = 0x23;
    int status = 0;

    asm volatile("mv a1, %0" : : "r" (id));                 // Pakuje se identifikator semafora kao argument.
    asm volatile("mv a0, %0" : : "r" (code));               // Broj sistemskog poziva se stavlja u registar a0.
    asm volatile("ecall");                                  // Sistemski poziv.
    asm volatile("mv %0, a0" : "=r" (status));              // Povratnu vrednost poziva stavljamo u promenljivu status.

    return status;
}

int sem_signal(sem_t id) {
    uint64 code = 0x24;
    int status = 0;

    asm volatile("mv a1, %0" : : "r" (id));                 // Pakuje se identifikator semafora kao argument.
    asm volatile("mv a0, %0" : : "r" (code));               // Broj sistemskog poziva se stavlja u registar a0.
    asm volatile("ecall");                                  // Sistemski poziv.
    asm volatile("mv %0, a0" : "=r" (status));              // Povratnu vrednost poziva stavljamo u promenljivu status.

    return status;
}

int time_sleep(time_t time) {
    uint64 code = 0x31;
    int status = 0;

    asm volatile("mv a1, %0" : : "r" (time));               // Pakuje se vreme za koje treba uspavati nit.
    asm volatile("mv a0, %0" : : "r" (code));               // Broj sistemskog poziva se stavlja u registar a0.
    asm volatile("ecall");                                  // Sistemski poziv.
    asm volatile("mv %0, a0" : "=r" (status));              // Povratnu vrednost poziva stavljamo u promenljivu status.

    return status;
}

char getc() {
    /*uint64 code = 0x41;
    char c = ' ';

    asm volatile("mv a0, %0" : : "r" (code));               // Broj sistemskog poziva se stavlja u registar a0.
    asm volatile("ecall");                                  // Sistemski poziv.
    asm volatile("mv %0, a0" : "=r" (c));                   // Povratnu vrednost poziva stavljamo u promenljivu status.

    return c;*/
    return __getc();
}

void putc(char c) {
    /*uint64 code = 0x42;

    asm volatile("mv a1, %0" : : "r" (c));               // Pakuje se vreme za koje treba uspavati nit.
    asm volatile("mv a0, %0" : : "r" (code));               // Broj sistemskog poziva se stavlja u registar a0.
    asm volatile("ecall");                                  // Sistemski poziv.

    return;*/
    __putc(c);
}