;********************************************************************************
;* @brief Definitions of miscellaneous parameters.
;********************************************************************************
.ifndef def_inc_
.equ def_inc_ = 0

.equ reset_vect  = 0x00 ; Reset vector, starting address of the program.
.equ pcint0_vect = 0x06 ; Interrupt vector for PCINT0 (PCI on I/O port B).
.equ pcint1_vect = 0x08 ; Interrupt vector for PCINT1 (PCI on I/O port C).
.equ pcint2_vect = 0x0A ; Interrupt vector for PCINT2 (PCI on I/O port D).

.equ external_ram_start = ramend + 1 ; Starting address for the external RAM memory.

.endif ; def_inc_ 