#ifndef _Scheduler_hpp_
#define _Scheduler_hpp_

#include "list.hpp"

// Napomena: Klasa Scheduler je u potpunosti preuzeta sa 7. vezbi - sinhrona promena konteksta.

class PCB;

class Scheduler {
private:
    static List<PCB> ready;                                                 // Red spremnih niti.

public:
    static PCB* get();                                                      // Dohvatanje jedne spremne niti.
    static void put(PCB *pcb);                                              // Stavljanje jedne niti u Scheduler.

    void* operator new(size_t size);                                        // Preklopljeni operator za alokaciju.
    void operator delete(void* ptr);                                        // Preklopljeni operator za dealokaciju.
};

#endif
