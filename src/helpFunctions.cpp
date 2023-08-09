#include "../h/helpFunctions.h"

void print(char* arr, size_t size) {                        // Ispisuje karaktere iz zadatog niza sa zadatom duzinom.
    for (size_t i = 0; i < size; i++) {
        __putc(arr[i]);
    }
    __putc('\n');
}

void printStringT(char const *arr) {                         // Ispisuje karaktere zadatog stringa.
    int i = 0;
    while (arr[i] != '\0') {
        __putc(arr[i++]);
    }
}

void printIntBinary(uint64 num) {                           // Ispisuje najnizih 8 bitova prosledjenog 64-bitnog broja.
    for (int i = 7; i >= 0; i--) {
        if (num & (1 << i)) __putc('1');
        else __putc('0');
    }
    __putc('\n');
}

void printInt(uint64 num) {                                 // Funkcija je u potpunosti preuzeta sa 7. vezbi i sluzi
    static char digits[] = "0123456789";                    // za ispis integera u dekadnom obliku.
    char buf[16]; int i = 0, neg = 0; uint64 x;

    if (num < 0) {
        neg = 1;
        x = -num;
    }
    else x = num;

    do buf[i++] = digits[x % 10];
    while ((x /= 10) != 0);

    if (neg) buf[i++] = '-';
    while (--i >= 0) __putc(buf[i]);
}
