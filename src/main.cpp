#include "../h/syscall_cpp.hpp"
#include "../h/helpFunctions.h"
#include "../h/PCB.hpp"

//#include "../h/tests/Threads_C_API_test.hpp"
//#include "../h/tests//Threads_CPP_API_test.hpp"
//#include "../h/tests/Threads_Semaphore_C_test.hpp"
//#include "../h/tests/Threads_Semaphore_CPP_test.hpp"
//#include "../h/tests/ConsumerProducer_C_API_test.h"
//#include "../h/tests/ConsumerProducer_CPP_Sync_API_test.hpp"

bool done = false;                                              // Boolean flag da li je userMain funkcija zavrsena.

extern "C" void interruptvec();                                 // Eksterna funkcija (asm) koja se pokrece pri prekidu.
uint64 basePointer;                                             // Base pointer koji sluzi za backup sp u prekidu.

void idleFunction(void *args) {                                 // Idle funkcija koja samo prepusta procesor drugima.
    while(true) PCB::yield();
}

void userMain(void* args) {                                     // Korisnicka funkcija.
    //Threads_C_API_test();
    //Threads_CPP_API_test();
    //Threads_Semaphore_C_test();
    //Threads_Semaphore_CPP_test();
    //producerConsumer_C_API();
    //producerConsumer_CPP_Sync_API();

    done = true;
}

int main() {
    asm volatile("csrw stvec, %0" : : "r" (interruptvec));          // Inicijalizacija registra adrese prekidne rutine.

    thread_t *t1 = (thread_t*)mem_alloc(sizeof(thread_t));     // Main postaje nit, kreira se nit sa
    thread_create(t1, nullptr, nullptr);      // nullptr telom i argumentima i ona je running.
    PCB::running = *t1;

    Thread* idle = new Thread(idleFunction, nullptr);     // Kreira se nit za idle funkciju.
    idle->start();

    userMain(nullptr);                                         // Poziva se userMain funkcija.
    //Thread *user = new Thread(userMain, nullptr);
    //user->start();
    //while (!done) PCB::yield();
    // delete user;

    delete idle;                                                    // Brisu se idle i main nit.
    delete t1;

    printStringT("\n*\n");                                      // Oznaka da je program zavrsen.

    return 0;
}