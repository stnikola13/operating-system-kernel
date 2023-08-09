#include "../h/syscall_cpp.hpp"
#include "../h/PCB.hpp"
//#include "../lib/mem.h"

// Operatori za alokaciju i dealokaciju memorije
void* operator new(size_t size) {                                                  // Poziv mem_alloc iz C-API-ja.
    return mem_alloc(size);
}

void* operator new[](size_t size) {                                                // Poziv mem_free iz C-API-ja.
    return mem_alloc(size);
}

void operator delete(void* ptr) noexcept {                                         // Poziv mem_alloc iz C-API-ja.
    mem_free(ptr);
}

void operator delete[](void* ptr) noexcept {                                       // Poziv mem_free iz C-API-ja.
    mem_free(ptr);
}


// Klasa Thread
Thread::Thread(void (*body)(void*), void* arg) {                                        // Konstruktor samo kreira
    PCB *pcb = PCB::createInactiveThread(body, arg);                               // neaktivnu nit koja ce se
    myHandle = pcb;                                                                     // pokrenuti tek kad se pozove
}                                                                                       // start.

Thread::~Thread() {                                                                     // Destruktor samo oznacava
    myHandle->setFinished(true);                                               // da je tekuca nit zavrsena,
    dispatch();                                                                         // menja kontekst i potencijalno
    delete myHandle;                        // TODO: nisam siguran da li ovo treba      // brise PCB iz sebe.
}

int Thread::start() {                                                           // Ako nit nije do sada bila pokretana
    if (!myHandle->wasStarted()) {                                              // (started flag) pokrece se tako sto se
        myHandle->setStarted(true);                                   // dohvataju telo i args iz postojece
        void (*body)(void *) = myHandle->getBody();                             // niti, nakon cega se stara neaktivna
        void *args = myHandle->getArgs();                                       // nit brise i kreira se nova aktivna
        delete myHandle;                                                        // nit sa procitanim telom i argumentima
        PCB *pcb = PCB::createActiveThread(body,args,                           // kao i alociranim stekom.
         (uint64 *) mem_alloc(DEFAULT_STACK_SIZE * sizeof(uint64)));  // Kreirana nit se stavlja u myHandle
        myHandle = pcb;                                                         // i vraca se 0 kao validna vrednost.
        return 0;
    }                                                                           // Ukoliko je nit prethodno startovana
    return -1;                                                                  // poziv nema efekta i vraca 0.
}

void Thread::dispatch() {                                                       // Samo poziva thread_dispatch iz
    thread_dispatch();                                                          // C-API-ja.
}

int Thread::sleep(time_t) {                                                     // Poziva thread_sleep iz C-API-ja.
    return 0;
}

Thread::Thread() {                                                              // U argument pakujemo zapravo objekat
    PCB *pcb = PCB::createInactiveThread(wrapperFunction, this);      // koji poziva funkciju, kako bismo
    myHandle = pcb;                                                             // u funkciji omotacu raspakovali obj.
}                                                                               // i polimorfno preko njega pozvali run.

void Thread::wrapperFunction(void* args) {                                      // Wrapper funkcija koja se koristi
    if (args != nullptr) {                                                      // za polimorfno pozivanje run metode
        Thread* thread = (Thread*)args;                                         // preko objekta. Nakon povratka iz
        thread->run();                                                          // poziva, postavlja se finished flag
        thread->myHandle->setFinished(true);                           // i pozivom dispatch kontekst prelazi
        Thread::dispatch();                                                     // na drugu nit trajno (ova je do kraja
    }                                                                           // zavrsila svoj posao).
}


// Klasa Semaphore
Semaphore::Semaphore (unsigned init) {
    sem_open(&myHandle, init);                                          // Samo poziva sem_open iz C-API-ja.
}

Semaphore::~Semaphore () {
    sem_close(myHandle);                                                // Samo poziva sem_close iz C-API-ja.
}

int Semaphore::wait () {
    return sem_wait(myHandle);                                              // Samo poziva sem_wait iz C-API-ja.
}

int Semaphore::signal () {
    return sem_signal(myHandle);                                            // Samo poziva sem_signal iz C-API-ja.
}


// Klasa PeriodicThread
PeriodicThread::PeriodicThread(time_t period) {                                 // Periodicne niti nisu implementirane.
    // TODO: dodati implementaciju
}


// Klasa Console
char Console::getc() {
    char c = ::getc();                                                          // Poziva se C-API funkcija getc.
    return c;
}

void Console::putc(char c) {                                                    // Poziva se C-API funkcija putc.
    ::putc(c);
}