#include "../h/Sem.hpp"             // Sem zove PCB koji zove Scheduler koji zove List koji zove MemoryAllocator.
#include "../h/helpFunctions.h"
#include "../h/syscall_c.hpp"
//#include "../lib/mem.h"

extern uint64 basePointer;

extern "C" void interrupt() {                                       // Format ove funkcije je uzet sa prez. za 2. vezbe.
    console_handler();
    uint64 scause;
    asm volatile("csrr %0, scause" : "=r" (scause));

    if (scause == 0x8000000000000001UL) {                           // timerInterrupt
        uint64 sip;
        asm volatile("csrr %0, sip" : "=r" (sip));
        sip &= ~2;                                                          // Oznaka da se obradio prekid.
        asm volatile("csrw sip, %0" : : "r" (sip));
        //console_handler();
    }
    else if (scause == 0x8000000000000009UL) {                      // hardwareInterrupt
        //printStringT("hardwareInterrupt\n");
        //console_handler();
    }
    else if (scause == 0x0000000000000002UL) {                      // illegalInstruction
        //printStringT("illegalInstruction\n");
        //console_handler();
    }
    else if (scause == 0x0000000000000005UL) {                      // forbiddenRead
        //printStringT("forbiddenRead\n");
        //console_handler();
    }
    else if (scause == 0x0000000000000007UL) {                      // forbiddenWrite
        //printStringT("forbiddenWrite\n");
        //console_handler();
    }                                                               // ecallSystem + ecallUser
    else if (scause == 0x0000000000000008UL || scause == 0x0000000000000009UL) {
        uint64 code;
        asm volatile("ld a0, 128(%0)" : : "r" (basePointer));           // Napomena za dalje: uvek se prvo sa steka skine
        asm volatile("mv %0, a0" : "=r" (code));                        // registar u slucaju da se promenio usput.

        if (code == 0x01) {                                         // (TODO) 0x01: mem_alloc
            uint64 sizeInBlocks;
            void *ptr = nullptr;
            asm volatile("ld a1, 136(%0)" : : "r" (basePointer));       // U a1 ponovo upisujemo sacuvanu vrednost sa
            asm volatile("mv %0, a1" : "=r" (sizeInBlocks));            // steka za slucaj da je bilo promene.

            MemoryAllocator *mem = MemoryAllocator::get();              // Preko MemoryAllocatora se alocira memorija.
            ptr = mem->mem_alloc(sizeInBlocks);

            asm volatile("mv a0, %0" : : "r" (ptr));
            asm volatile("sd a0, 128(%0)" : : "r" (basePointer));       // Iz a0 rucno upisujemo na stek zbog override.

            uint64 sepc;                                                    // (Sve instrukcije su velicine 4 bajta.)
            asm volatile("csrr %0, sepc" : "=r" (sepc));                    // Prelazak na sledecu instrukciju, posto
            sepc = sepc + 4;                                                // sepc pokazuje tacno tamo gde je sacuvan
            asm volatile("csrw sepc, %0" : : "r" (sepc));                   // inicijalno, a ne na sledecu instrukciju.
        }

        else if (code == 0x02) {                                    // (TODO) 0x02: mem_free
            void* ptr = nullptr;
            int status = 0;
            asm volatile("ld a1, 136(%0)" : : "r" (basePointer));       // U a1 ponovo upisujemo sacuvanu vrednost sa
            asm volatile("mv %0, a1" : "=r" (ptr));                     // steka za slucaj da je bilo promene.

            MemoryAllocator *mem = MemoryAllocator::get();              // Preko MemoryAllocatora se dealocira memorija.
            status = mem->mem_free(ptr);

            asm volatile("mv a0, %0" : : "r" (status));
            asm volatile("sd a0, 128(%0)" : : "r" (basePointer));       // Iz a0 rucno upisujemo na stek zbog override.

            uint64 sepc;                                                    // (Sve instrukcije su velicine 4 bajta.)
            asm volatile("csrr %0, sepc" : "=r" (sepc));                    // Prelazak na sledecu instrukciju, posto
            sepc = sepc + 4;                                                // sepc pokazuje tacno tamo gde je sacuvan
            asm volatile("csrw sepc, %0" : : "r" (sepc));                   // inicijalno, a ne na sledecu instrukciju.
        }

        else if (code == 0x11) {                                    // (TODO) 0x11: thread_create
            int status = 0;
            thread_t* handle = nullptr;
            void(*start_routine)(void*) = nullptr;
            void* arg = nullptr;
            uint64 *stack = nullptr;

            asm volatile("ld a1, 136(%0)" : : "r" (basePointer));       // U a1 ponovo upisujemo sacuvanu vrednost sa
            asm volatile("mv %0, a1" : "=r" (handle));                  // steka za slucaj da je bilo promene.
            asm volatile("ld a2, 144(%0)" : : "r" (basePointer));       // U a2 ponovo upisujemo sacuvanu vrednost sa
            asm volatile("mv %0, a2" : "=r" (start_routine));           // steka za slucaj da je bilo promene.
            asm volatile("ld a3, 152(%0)" : : "r" (basePointer));       // U a3 ponovo upisujemo sacuvanu vrednost sa
            asm volatile("mv %0, a3" : "=r" (arg));                     // steka za slucaj da je bilo promene.
            asm volatile("ld a4, 160(%0)" : : "r" (basePointer));       // U a4 ponovo upisujemo sacuvanu vrednost sa
            asm volatile("mv %0, a4" : "=r" (stack));                   // steka za slucaj da je bilo promene.

            PCB *pcb = PCB::createActiveThread(start_routine, arg, stack);
            if (handle != nullptr) *handle = pcb;                       // Kreira se aktivna nit za dobijene argumente
            if (pcb == nullptr) status = -1;                            // i stavlja u handle.

            asm volatile("mv a0, %0" : : "r" (status));
            asm volatile("sd a0, 128(%0)" : : "r" (basePointer));       // Iz a0 rucno upisujemo na stek zbog override.

            uint64 sepc;                                                    // (Sve instrukcije su velicine 4 bajta.)
            asm volatile("csrr %0, sepc" : "=r" (sepc));                    // Prelazak na sledecu instrukciju, posto
            sepc = sepc + 4;                                                // sepc pokazuje tacno tamo gde je sacuvan
            asm volatile("csrw sepc, %0" : : "r" (sepc));                   // inicijalno, a ne na sledecu instrukciju.
        }

        else if (code == 0x12) {                                    // (TODO) 0x12: thread_exit
            uint64 sepc;                                                    // Moramo da sacuvamo povratnu adresu zbog
            asm volatile("csrr %0, sepc" : "=r" (sepc));                    // dispatch-a (to ce biti prva instrukcija)
                                                                            // nakon dispatch-a pa ovde pamtimo sepc.
            uint64 sstatus;                                                 // Posto moze da dodje do promene sstatus
            asm volatile("csrr %0, sstatus" : "=r" (sstatus));              // pamtimo i njegovu vrednost.

            int status = 0;
            PCB::running->kill();                                       // Preko staticke metode se tekuca nit ubija.

            asm volatile("mv a0, %0" : : "r" (status));
            asm volatile("sd a0, 128(%0)" : : "r" (basePointer));       // Iz a0 rucno upisujemo na stek zbog override.

            sepc = sepc + 4;
            asm volatile("csrw sepc, %0" : : "r" (sepc));                   // Restauriramo sacuvane vrednosti, povratnu
            asm volatile("csrw sstatus, %0" : : "r" (sstatus));             // adresu i statusni registar.
        }

        else if (code == 0x13) {                                    // (TODO) 0x13: thread_dispatch
            uint64 sepc;                                                    // Moramo da sacuvamo povratnu adresu zbog
            asm volatile("csrr %0, sepc" : "=r" (sepc));                    // yield-a (to ce biti prva instrukcija)
                                                                            // nakon yield-a pa ovde pamtimo sepc.
            uint64 sstatus;                                                 // Posto moze da dodje do promene sstatus
            asm volatile("csrr %0, sstatus" : "=r" (sstatus));              // pamtimo i njegovu vrednost.

            PCB::yield();                                                   // Menja se kontekst.

            sepc = sepc + 4;
            asm volatile("csrw sepc, %0" : : "r" (sepc));                   // Restauriramo sacuvane vrednosti, povratnu
            asm volatile("csrw sstatus, %0" : : "r" (sstatus));             // adresu i statusni registar.
        }

        else if (code == 0x21) {                                    // (TODO) 0x21: sem_open
            sem_t* sem = nullptr;
            unsigned init = 0;
            int status = 0;

            asm volatile("ld a1, 136(%0)" : : "r" (basePointer));       // U a1 ponovo upisujemo sacuvanu vrednost sa
            asm volatile("mv %0, a1" : "=r" (sem));                     // steka za slucaj da je bilo promene.
            asm volatile("ld a2, 144(%0)" : : "r" (basePointer));       // U a2 ponovo upisujemo sacuvanu vrednost sa
            asm volatile("mv %0, a2" : "=r" (init));                    // steka za slucaj da je bilo promene.

            if (sem == nullptr || init < 0) status = -1;
            else {                                                      // Kreira se semafor za dobijene argumente
                Sem *semaphore = Sem::createSemaphore(init); // i stavlja se u handle.
                *sem = semaphore;
            }

            asm volatile("mv a0, %0" : : "r" (status));                 // Status upisujemo u registar a0.
            asm volatile("sd a0, 128(%0)" : : "r" (basePointer));       // Iz a0 rucno upisujemo na stek zbog override.

            uint64 sepc;                                                    // (Sve instrukcije su velicine 4 bajta.)
            asm volatile("csrr %0, sepc" : "=r" (sepc));                    // Prelazak na sledecu instrukciju, posto
            sepc = sepc + 4;                                                // sepc pokazuje tacno tamo gde je sacuvan
            asm volatile("csrw sepc, %0" : : "r" (sepc));                   // inicijalno, a ne na sledecu instrukciju.
        }

        else if (code == 0x22) {                                    // (TODO) 0x22: sem_close
            sem_t sem = nullptr;
            int status = 0;

            asm volatile("ld a1, 136(%0)" : : "r" (basePointer));       // U a1 ponovo upisujemo sacuvanu vrednost sa
            asm volatile("mv %0, a1" : "=r" (sem));                     // steka za slucaj da je bilo promene.

            if (sem == nullptr) status = -1;
            else {
                delete sem;                                             // Eksplicitno brise semafor (destruktor).
            }

            asm volatile("mv a0, %0" : : "r" (status));                 // Status upisujemo u registar a0.
            asm volatile("sd a0, 128(%0)" : : "r" (basePointer));       // Iz a0 rucno upisujemo na stek zbog override.

            uint64 sepc;                                                    // (Sve instrukcije su velicine 4 bajta.)
            asm volatile("csrr %0, sepc" : "=r" (sepc));                    // Prelazak na sledecu instrukciju, posto
            sepc = sepc + 4;                                                // sepc pokazuje tacno tamo gde je sacuvan
            asm volatile("csrw sepc, %0" : : "r" (sepc));                   // inicijalno, a ne na sledecu instrukciju.
        }

        else if (code == 0x23) {                                    // (TODO) 0x23: sem_wait
            uint64 sepc;                                                    // Moramo da sacuvamo povratnu adresu zbog
            asm volatile("csrr %0, sepc" : "=r" (sepc));                    // yield-a (to ce biti prva instrukcija)
                                                                            // nakon yield-a pa ovde pamtimo sepc.
            uint64 sstatus;                                                 // Posto moze da dodje do promene sstatus
            asm volatile("csrr %0, sstatus" : "=r" (sstatus));              // pamtimo i njegovu vrednost.

            sem_t sem = nullptr;
            int status = 0;

            asm volatile("ld a1, 136(%0)" : : "r" (basePointer));       // U a1 ponovo upisujemo sacuvanu vrednost sa
            asm volatile("mv %0, a1" : "=r" (sem));                     // steka za slucaj da je bilo promene.

            if (sem == nullptr) status = -1;                            // Ukoliko prosledjeni semafor postoji
            else {                                                      // poziva se operacija wait nad njim.
                Sem* semaphore = sem;
                semaphore->wait();
                PCB* cur = PCB::running;                                // Ukoliko je za nit za koju se pozvao wait
                if (cur->isError()) {                                   // obrisan semafor, status je negativan.
                    status = -1;                                        // Nakon toga se uklanja greska.
                    cur->setError(false);
                }
            }

            asm volatile("mv a0, %0" : : "r" (status));                 // Status upisujemo u registar a0.
            asm volatile("sd a0, 128(%0)" : : "r" (basePointer));       // Iz a0 rucno upisujemo na stek zbog override.

            sepc = sepc + 4;
            asm volatile("csrw sepc, %0" : : "r" (sepc));                   // Restauriramo sacuvane vrednosti, povratnu
            asm volatile("csrw sstatus, %0" : : "r" (sstatus));             // adresu i statusni registar.
        }

        else if (code == 0x24) {                                    // (TODO) 0x24: sem_signal
            sem_t sem = nullptr;
            int status = 0;

            asm volatile("ld a1, 136(%0)" : : "r" (basePointer));       // U a1 ponovo upisujemo sacuvanu vrednost sa
            asm volatile("mv %0, a1" : "=r" (sem));                     // steka za slucaj da je bilo promene.

            if (sem == nullptr) status = -1;                            // Ukoliko prosledjeni semafor postoji
            else {                                                      // poziva se operacija signal nad njim.
                Sem* semaphore = sem;
                semaphore->signal();
            }

            asm volatile("mv a0, %0" : : "r" (status));                 // Status upisujemo u registar a0.
            asm volatile("sd a0, 128(%0)" : : "r" (basePointer));       // Iz a0 rucno upisujemo na stek zbog override.

            uint64 sepc;                                                    // (Sve instrukcije su velicine 4 bajta.)
            asm volatile("csrr %0, sepc" : "=r" (sepc));                    // Prelazak na sledecu instrukciju, posto
            sepc = sepc + 4;                                                // sepc pokazuje tacno tamo gde je sacuvan
            asm volatile("csrw sepc, %0" : : "r" (sepc));                   // inicijalno, a ne na sledecu instrukciju.
        }

        else if (code == 0x31) {                                    // (TODO) 0x31: time_sleep
            time_t time = 0;
            int status = 0;

            asm volatile("ld a1, 136(%0)" : : "r" (basePointer));       // U a1 ponovo upisujemo sacuvanu vrednost sa
            asm volatile("mv %0, a1" : "=r" (time));                    // steka za slucaj da je bilo promene.

            // Poziv metode za uspavljivanje (nemam)

            asm volatile("mv a0, %0" : : "r" (status));                 // Status upisujemo u registar a0.
            asm volatile("sd a0, 128(%0)" : : "r" (basePointer));       // Iz a0 rucno upisujemo na stek zbog override.

            uint64 sepc;                                                    // (Sve instrukcije su velicine 4 bajta.)
            asm volatile("csrr %0, sepc" : "=r" (sepc));                    // Prelazak na sledecu instrukciju, posto
            sepc = sepc + 4;                                                // sepc pokazuje tacno tamo gde je sacuvan
            asm volatile("csrw sepc, %0" : : "r" (sepc));                   // inicijalno, a ne na sledecu instrukciju.
        }

        else if (code == 0x41) {                                    // (TODO) 0x41: getc
            char c = ' ';

            //c = __getc();                                               // Poziva ugradjenu funkciju za unos karaktera.

            asm volatile("mv a0, %0" : : "r" (c));                      // Procitani karakter upisujemo u registar a0.
            asm volatile("sd a0, 128(%0)" : : "r" (basePointer));       // Iz a0 rucno upisujemo na stek zbog override.

            uint64 sepc;                                                    // (Sve instrukcije su velicine 4 bajta.)
            asm volatile("csrr %0, sepc" : "=r" (sepc));                    // Prelazak na sledecu instrukciju, posto
            sepc = sepc + 4;                                                // sepc pokazuje tacno tamo gde je sacuvan
            asm volatile("csrw sepc, %0" : : "r" (sepc));                   // inicijalno, a ne na sledecu instrukciju.
        }

        else if (code == 0x42) {                                    // (TODO) 0x42: putc
            char c = ' ';

            asm volatile("ld a1, 136(%0)" : : "r" (basePointer));       // U a1 ponovo upisujemo sacuvanu vrednost sa
            asm volatile("mv %0, a1" : "=r" (c));                       // steka za slucaj da je bilo promene.

            //__putc(c);                                              // Poziva ugradjenu funkciju za ispis karaktera.

            uint64 sepc;                                                    // (Sve instrukcije su velicine 4 bajta.)
            asm volatile("csrr %0, sepc" : "=r" (sepc));                    // Prelazak na sledecu instrukciju, posto
            sepc = sepc + 4;                                                // sepc pokazuje tacno tamo gde je sacuvan
            asm volatile("csrw sepc, %0" : : "r" (sepc));                   // inicijalno, a ne na sledecu instrukciju.
        }
        //console_handler();
    }
    else {                                                          // Other
        printStringT("*Unknown cause*\n");                           // Ispisuju se scause, sepc i stval.
        uint64 sepc;
        asm volatile("csrr %0, sepc" : "=r" (sepc));
        uint64 stval;
        asm volatile("csrr %0, stval" : "=r" (stval));

        printStringT("scause: ");
        printInt(scause);
        __putc('\n');

        printStringT("sepc: ");
        printInt(sepc);
        __putc('\n');

        printStringT("stval: ");
        printInt(stval);
        __putc('\n');
    }
    //console_handler();                                                      // TODO: Nisam siguran da li ovo treba ovde ili samo kod tajmera
    return;
}