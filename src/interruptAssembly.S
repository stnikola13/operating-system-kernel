# 1 "src/interruptAssembly.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/riscv64-linux-gnu/include/stdc-predef.h" 1 3
# 32 "<command-line>" 2
# 1 "src/interruptAssembly.S"
# Ideja asemblerskog koda prekidne rutine je preuzeta sa prezentacije za 2. vezbe.
.global interrupt
.type interrupt, @function

.global interruptvec
.type interruptvec, @function

.extern basePointer
.align 4

interruptvec: addi sp, sp, -256

                # Cuvanje registara na steku.
                sd ra, 0(sp)
                sd sp, 8(sp)
                sd gp, 16(sp)
                sd tp, 24(sp)

                sd s0, 32(sp)
                sd s1, 40(sp)
                sd s2, 48(sp)
                sd s3, 56(sp)

                sd s4, 64(sp)
                sd s5, 72(sp)
                sd s6, 80(sp)
                sd s7, 88(sp)

                sd s8, 96(sp)
                sd s9, 104(sp)
                sd s10, 112(sp)
                sd s11, 120(sp)

                sd a0, 128(sp)
                sd a1, 136(sp)
                sd a2, 144(sp)
                sd a3, 152(sp)

                sd a4, 160(sp)
                sd a5, 168(sp)
                sd a6, 176(sp)
                sd a7, 184(sp)

                sd t0, 192(sp)
                sd t1, 200(sp)
                sd t2, 208(sp)
                sd t3, 216(sp)

                sd t4, 224(sp)
                sd t5, 232(sp)
                sd t6, 240(sp)
                sd zero, 248(sp)

                # Cuvanje trenutne vrednosti stack pointera kao basePointera zbog eventualnih promena registara.
                sd sp, (basePointer), t0

                # Poziv potprograma za prekidnu rutinu.
                call interrupt

                # Restauracija sacuvanih registara.
                ld ra, 0(sp)
                ld sp, 8(sp)
                ld gp, 16(sp)
                ld tp, 24(sp)

                ld s0, 32(sp)
                ld s1, 40(sp)
                ld s2, 48(sp)
                ld s3, 56(sp)

                ld s4, 64(sp)
                ld s5, 72(sp)
                ld s6, 80(sp)
                ld s7, 88(sp)

                ld s8, 96(sp)
                ld s9, 104(sp)
                ld s10, 112(sp)
                ld s11, 120(sp)

                ld a0, 128(sp)
                ld a1, 136(sp)
                ld a2, 144(sp)
                ld a3, 152(sp)

                ld a4, 160(sp)
                ld a5, 168(sp)
                ld a6, 176(sp)
                ld a7, 184(sp)

                ld t0, 192(sp)
                ld t1, 200(sp)
                ld t2, 208(sp)
                ld t3, 216(sp)

                ld t4, 224(sp)
                ld t5, 232(sp)
                ld t6, 240(sp)
                ld zero, 248(sp)

                addi sp, sp, 256

                sret
