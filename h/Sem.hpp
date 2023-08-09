#ifndef _Sem_hpp_
#define _Sem_hpp_

#include "../h/PCB.hpp"

// Napomena: Skelet klase i ideje za implementaciju svih metoda su preuzeti iz skripte za PRV (str. 122 i 123).

class Sem {
private:
    List<PCB>* blocked;                                                     // Red blokiranih niti.
    int val;                                                                // Vrednost semafora.

protected:
    void block();                                                           // Pomocna metoda za blokiranje niti.
    void deblock();                                                         // Pomocna metoda za deblokiranje niti.
    explicit Sem(int value);                                                // Eksplicitni konstruktor, zasticeni.

public:
    void* operator new(size_t size);                                        // Preklopljeni operator za alokaciju.
    void operator delete(void* ptr);                                        // Preklopljeni operator za dealokaciju.

    static Sem* createSemaphore(int initSemValue = 1);                      // Staticka metoda za kreiranje semafora.
    ~Sem();                                                                 // Javni destruktor.
    void wait();                                                            // Klasicna metoda wait.
    void signal();                                                          // Klasicna metoda signal.
    int getValue() const { return val; };                                   // Metoda za vracanje vrednosti semafora.
};

#endif
