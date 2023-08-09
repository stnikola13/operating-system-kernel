#ifndef _syscall_c_hpp_
#define _syscall_c_hpp_

#include "../lib/hw.h"
#include "../lib/console.h"

void* mem_alloc(size_t size);
int mem_free(void* ptr);

class PCB;
typedef PCB* thread_t;

int thread_create(thread_t* handle, void(*start_routine)(void*), void* arg);
int thread_exit();
void thread_dispatch();

class Sem;
typedef Sem* sem_t;

int sem_open(sem_t* handle, unsigned init);
int sem_close(sem_t handle);
int sem_wait(sem_t id);
int sem_signal(sem_t id);

typedef unsigned long time_t;
int time_sleep(time_t time);

const int EOF = -1;
char getc();
void putc(char c);

#endif