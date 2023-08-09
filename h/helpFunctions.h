#ifndef _helpFunctions_h_
#define _helpFunctions_h_

#include "../lib/hw.h"
#include "../lib/console.h"

// Napomena: Neke od sledecih funkcija su preuzete sa 7. vezbi - sinhrona promena konteksta.

void print(char* arr, size_t size);                        // Ispisuje karaktere iz zadatog niza sa zadatom duzinom.
void printStringT(char const *arr);                        // Ispisuje karaktere zadatog stringa.
void printIntBinary(uint64 num);                          // Ispisuje najnizih 8 bitova prosledjenog 64-bitnog broja.
void printInt(uint64 num);                                // Funkcija je u potpunosti preuzeta sa 7. vezbi i sluzi
                                                          // za ispis integera u dekadnom obliku.

#endif
