#include "../h/Scheduler.hpp"

List<PCB> Scheduler::ready;

PCB* Scheduler::get() {                                                     // Obzirom da se koristi first-fit
    PCB *pcb = ready.removeFirst();                                         // algoritam, dohvatanje niti se svodi na
    return pcb;                                                             // uzimanje prvog elementa iz ulancane
}                                                                           // liste spremnih niti.

void Scheduler::put(PCB *pcb) {                                             // Obzirom da se koristi first-fit
    ready.addLast(pcb);                                                // algoritam, stavljanje niti u Scheduler se
}                                                                           // svodi na dodavanje nje na kraj ul. liste.

void* Scheduler::operator new(size_t size) {                                // Preklopljeni operator za alokaciju.
    size_t adjSize = 0;
    if (size % MEM_BLOCK_SIZE == 0) adjSize = size / MEM_BLOCK_SIZE;
    else adjSize = (size / MEM_BLOCK_SIZE) + 1;
    return MemoryAllocator::get()->mem_alloc(adjSize);
}

void Scheduler::operator delete(void* ptr) {                                // Preklopljeni operator za dealokaciju.
    MemoryAllocator::get()->mem_free(ptr);
}
