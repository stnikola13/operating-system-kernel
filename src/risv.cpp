#include "../h/riscv.hpp"

// Napomena: Funkcija je preuzeta sa 7. vezbi - asinhrona promena konteksta.

void Riscv::popSppSpie() {
    asm volatile("csrw sepc, ra");
    asm volatile("sret");
}
