# Kod za promenu konteksta u asembleru je u potpunosti preuzet sa 7. vezbi - sinhrona promena konteksta.

.global _ZN3PCB13contextSwitchEPNS_7ContextES1_
.type _ZN3PCB13contextSwitchEPNS_7ContextES1_, @function

_ZN3PCB13contextSwitchEPNS_7ContextES1_:    sd ra, 0(a0)                # Tekuca povratna adresa i stack pointer se
                                            sd sp, 8(a0)                # cuvaju u strukturi Context (na osnovu adrese
                                                                        # strukture iz registra a0).
                                            ld ra, 0(a1)                # Slicno se iz nove strukture restaurira
                                            ld sp, 8(a1)                # povratna adresa i novi stack pointer.

                                            ret