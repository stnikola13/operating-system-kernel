#ifndef _PCB_hpp_
#define _PCB_hpp_

#include "Scheduler.hpp"

// Napomena: Kod ove klase je vecinskim delom preuzet sa 7. vezbi - sinhrona (i delom asinhrona) promena konteksta.

using Body = void (*)(void*);

extern "C" void pushRegisters();                                            // Funkcije za cuvanje i restauraciju
extern "C" void popRegisters();                                             // registara (osim ra i sp) na steku.

class PCB {
private:
    struct Context {                                                        // Struktura za cuvanje konteksta niti
        uint64 ra;                                                          // (povratne adrese i stack pointer-a).
        uint64 sp;
    };

    Body body;                                                              // Telo funkcije koju nit treba da izvrsi.
    void* args;                                                             // Argumenti za koje se poziva funkc. body.
    uint64 *stack;                                                          // Pokazivac(niz) na stek dodeljen niti.
    Context context;                                                        // Kontekst niti (stack p., return address).
    bool finished;                                                          // Da li je nit zavrsila svoju funkciju?
    bool started;                                                           // Da li je nit bila pokretana/startovana?
    bool blocked;                                                           // Da li je nit blokirana na nekom semaforu?
    bool error;                                                             // Univerzalna greska (obrisan semafor...).
    uint64 timeSlice;                                                       // Vremenski odsecak dodeljen niti (neimpl.).

    static void dispatch();                                                 // Metoda za promenu konteksta.
    static void contextSwitch(Context *oldContext, Context *newContext);    // Asemblerska metoda za promenu konteksta.
    static void wrapperFunction();                                          // Wrapper funkcija za poziv sa argumentima.

    explicit PCB(Body body, void* args);                                    // Koristi se za nit koja se ne pokrece.
    explicit PCB(Body body, void* args, uint64* stack);                     // Koristi se za nit koja se odmah pokrece.

public:
    static uint64 timeSliceCounter;                                         // Brojac proteklog vremena (neimplement.).

    void* operator new(size_t size);                                        // Preklopljeni operatori za alokaciju i
    void operator delete(void* ptr);                                        // dealokaciju memorije.

    static PCB* running;                                                    // Pokazivac na tekucu (aktivnu) nit (PCB).
    static void yield();                                                    // Prepustanje procesora (sebi/drugoj niti).
    void kill();                                                            // Ubijanje niti (zavrsavanje).

    static PCB* createInactiveThread(Body body, void* args);                // Stat. metoda za kreiranje PCB-a (neakt.).
    static PCB* createActiveThread(Body body, void* args, uint64* stack);   // Stat. metoda za kreiranje PCB-a (aktiv.).

    bool isFinished() const { return finished; }                            // Provera da li je nit zavrsila svoju funk.
    void setFinished(bool isFinished) { finished = isFinished; }            // Postavljanje statusa zavrsetka niti.

    Body getBody() const { return body; }                                   // Dohvatanje tela funkcije te niti.
    void* getArgs() const { return args; }                                  // Dohvatanje argumenata za funkciju te niti.

    bool wasStarted() const { return started; }                             // Da li je nit startovana (zbog Thread).
    void setStarted(bool wasStarted) { started = wasStarted; }              // Postavljanje startovanosti (zbog Thread).

    bool isBlocked() const { return blocked; }                             // Da li je nit blokirana (zbog Sem).
    void setBlocked(bool isBlocked) { blocked = isBlocked; }               // Postavljanje statusa blokade (zbog Sem).

    bool isError() const { return error; }                                  // Da li je doslo do greska (zbog Semaphore).
    void setError(bool isError) { error = isError; }                        // Postalvjanje greske (zbog Semaphore).

    uint64 getTimeSlice() const { return timeSlice; }                       // Dohvatanje vremenskog odescka niti.
    void setTimeSlice(uint64 time) { timeSlice = time; }                    // Postavljanje vremenskog odescka niti.

    ~PCB() { delete[] stack; }                                              // Javni destruktor za dealokaciju steka.
};

#endif
