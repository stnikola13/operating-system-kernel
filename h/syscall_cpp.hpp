#ifndef _syscall_cpp_hpp_
#define _syscall_cpp_hpp_

#include "syscall_c.hpp"

// Operatori za alokaciju i dealokaciju memorije
void* operator new(size_t size);                    // :: su izostavljene ispred operator, posto proizvode gresku.
void* operator new[](size_t size);                  // :: su izostavljene ispred operator, posto proizvode gresku.
void operator delete(void* ptr) noexcept;           // :: su izostavljene ispred operator, posto proizvode gresku.
void operator delete[](void* ptr) noexcept;         // :: su izostavljene ispred operator, posto proizvode gresku.


// Klasa koja apstrahuje niti - Thread
class Thread {
public:
    Thread(void (*body)(void*), void* arg);
    virtual ~Thread();
    int start();
    static void dispatch();
    static int sleep(time_t);
    static void wrapperFunction(void* args);
protected:
    Thread();
    virtual void run() {}
private:
    thread_t myHandle;
};


// Klasa koja apstrahuje semafore - Semaphore
class Semaphore {
public:
    Semaphore(unsigned init = 1);
    virtual ~Semaphore();
    int wait();
    int signal();
private:
    sem_t myHandle;
};


// Apstrakcija periodicnih niti - PeriodicThread
class PeriodicThread : public Thread {
protected:
    PeriodicThread(time_t period);
    virtual void periodicActivation() {}
};


// Klasa koja apstrahuje konzolu - Console
class Console {
public:
    static char getc();
    static void putc(char c);
};

#endif