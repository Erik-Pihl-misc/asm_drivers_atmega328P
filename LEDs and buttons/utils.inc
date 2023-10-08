;********************************************************************************
;* @brief Contains utility subroutines.
;********************************************************************************
.ifndef utils_inc_
.equ utils_inc_ = 0

;********************************************************************************
;* @brief Provides value bit shifted specified number of times via R24.
;*        For instance, if specified number of shifts is 5, the value
;*        (1 << 5) = 0010 0000 is returned.
;*
;* @param R24 The number of bit shifts to perform.
;*
;* @return The bit shifted value (via R24).
;********************************************************************************
shift_bits:
    ldi r16, 0x01
    ldi r17, 0x00
    shift_bits_loop:
        cp r17, r24
        breq shift_bits_end
        lsl r16
        inc r17
        rjmp shift_bits_loop
    shift_bits_end:
        mov r24, r16
        ret

;********************************************************************************
;* @brief Provides value bit shifted specified number of times and inverted 
;*        via R24. For instance, if specified number of shifts is 5, the value 
;*        ~(1 << 5) = 1101 1111 is returned.
;*
;* @param R24 The number of bit shifts to perform.
;*
;* @return The bit shifted value (via R24).
;********************************************************************************
shift_bits_inverted:
    ldi r16, 0xFE
    ldi r17, 0x00
    shift_bits_inverted_loop:
        cp r17, r24
        breq shift_bits_inverted_end
        lsl r16
        inc r16
        inc r17
        rjmp shift_bits_inverted_loop
    shift_bits_inverted_end:
        mov r24, r16
        ret

.endif ; utils_inc_