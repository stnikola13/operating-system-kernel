# 1 "src/registers.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/riscv64-linux-gnu/include/stdc-predef.h" 1 3
# 32 "<command-line>" 2
# 1 "src/registers.S"
.global pushRegisters
.type pushRegisters, @function

.global popRegisters
.type popRegisters, @function

pushRegisters: addi sp, sp, -256

                # Cuvanje registara na steku (izostavljamo registre ra, sp (koji se cuvaju u Context) i zero).
                sd gp, 24(sp)

                sd tp, 32(sp)
                sd s0, 40(sp)
                sd s1, 48(sp)
                sd s2, 56(sp)

                sd s3, 64(sp)
                sd s4, 72(sp)
                sd s5, 80(sp)
                sd s6, 88(sp)

                sd s7, 96(sp)
                sd s8, 104(sp)
                sd s9, 112(sp)
                sd s10, 120(sp)

                sd s11, 128(sp)
                sd a0, 136(sp)
                sd a1, 144(sp)
                sd a2, 152(sp)

                sd a3, 160(sp)
                sd a4, 168(sp)
                sd a5, 176(sp)
                sd a6, 184(sp)

                sd a7, 192(sp)
                sd t0, 200(sp)
                sd t1, 208(sp)
                sd t2, 216(sp)

                sd t3, 224(sp)
                sd t4, 232(sp)
                sd t5, 240(sp)
                sd t6, 248(sp)

                ret


popRegisters: # Restauracija registara sa steka (izostavljamo registre ra, sp (koji se cuvaju u Context) i zero).
                ld gp, 24(sp)

                ld tp, 32(sp)
                ld s0, 40(sp)
                ld s1, 48(sp)
                ld s2, 56(sp)

                ld s3, 64(sp)
                ld s4, 72(sp)
                ld s5, 80(sp)
                ld s6, 88(sp)

                ld s7, 96(sp)
                ld s8, 104(sp)
                ld s9, 112(sp)
                ld s10, 120(sp)

                ld s11, 128(sp)
                ld a0, 136(sp)
                ld a1, 144(sp)
                ld a2, 152(sp)

                ld a3, 160(sp)
                ld a4, 168(sp)
                ld a5, 176(sp)
                ld a6, 184(sp)

                ld a7, 192(sp)
                ld t0, 200(sp)
                ld t1, 208(sp)
                ld t2, 216(sp)

                ld t3, 224(sp)
                ld t4, 232(sp)
                ld t5, 240(sp)
                ld t6, 248(sp)

                addi sp, sp, 256

                ret
