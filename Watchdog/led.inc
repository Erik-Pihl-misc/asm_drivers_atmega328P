;********************************************************************************
;* @brief Driver for LEDs via a structure, implemented as shown below:
;*
;*        struct led {
;*            volatile uint8_t* port_reg;
;*            volatile uint8_t* pin_reg;
;*            volatile uint8_t* dir_reg;
;*            uint8_t pin;
;*        };
;********************************************************************************
.ifndef led_inc_
.equ led_inc_ = 0

;********************************************************************************
;* @brief Includes utility subroutines for bit shifting.
;********************************************************************************
.include "utils.inc"

;********************************************************************************
;* @brief Parameters associated with the LED driver.
;********************************************************************************
.equ led_port_reg_ptr_offset = 0 ; Offset for port register pointer.
.equ led_pin_reg_ptr_offset  = 2 ; Offset for pin register pointer.
.equ led_dir_reg_ptr_offset  = 4 ; Offset for data direction register pointer.
.equ led_pin_offset          = 6 ; Offset for pin number.
.equ led_size                = 7 ; Size of LED structure in bytes.

;********************************************************************************
;* @brief Initializes LED on specified I/O port B or D.
;*
;* @param R25:R24 Reference to the LED.
;* @param R22     The LED's pin number on Arduino Uno (0 - 13).
;*
;* @return 0 upon successful initialization, error code 1 if the specified pin
;*         number is invalid (via R24).
;********************************************************************************
led_init:
    movw z, r24
    cpi r22, 8
    brlo led_init_portd
    cpi r22, 14
    brlo led_init_portb
    led_init_error:
        ldi r24, 0x01
        ret
    led_init_portb:
        subi r22, 8
        ldi xl, low(pinb + 0x20)
        ldi xh, high(pinb + 0x20)
        rjmp led_init_structure
    led_init_portd:
        ldi xl, low(pind + 0x20)
        ldi xh, high(pind + 0x20)
    led_init_structure:
       std z + led_pin_offset, r22
       std z + led_pin_reg_ptr_offset, xl
       std z + led_pin_reg_ptr_offset + 1, xh
       adiw x, 1
       std z + led_dir_reg_ptr_offset, xl
       std z + led_dir_reg_ptr_offset + 1, xh
       adiw x, 1
       std z + led_port_reg_ptr_offset, xl
       std z + led_port_reg_ptr_offset + 1, xh
    led_init_hardware:
        mov r24, r22
        rcall shift_bits
        ldd xl, z + led_dir_reg_ptr_offset
        ldd xh, z + led_dir_reg_ptr_offset + 1
        ld r16, x
        or r16, r24
        st x, r16
        ldd r24, z + led_pin_offset
        rcall shift_bits_inverted
        ldd xl, z + led_port_reg_ptr_offset
        ldd xh, z + led_port_reg_ptr_offset + 1
        ld r16, x
        and r16, r24
        st x, r16
    led_init_end:
        ldi r24, 0x00
        ret
        
;********************************************************************************
;* @brief Clears referenced LED and disables connected port.
;*
;* @param R25:R24 Reference to the LED.
;********************************************************************************
led_clear:
    movw z, r24
    ldd xl, z + led_port_reg_ptr_offset
    ldd xh, z + led_port_reg_ptr_offset + 1
    ldd r24, z + led_pin_offset
    rcall shift_bits_inverted
    ld r16, x
    and r16, r24
    st x, r16
    ldd xl, z + led_pin_reg_ptr_offset
    ldd xh, z + led_pin_reg_ptr_offset + 1
    ld r16, x
    and r16, r24
    st x, r16
    led_clear_structure:
        ldi r24, 0x00
        ldi r16, 0x00
        led_clear_structure_loop:
            cpi r16, led_size
            breq led_clear_end
            st z+, r24
            inc r16
            rjmp led_clear_structure_loop
        led_clear_end:
            ret

;********************************************************************************
;* @brief Enables referenced LED.
;*
;* @param R25:R24 Reference to the LED.
;********************************************************************************
led_on:
    movw z, r24
    ldd xl, z + led_port_reg_ptr_offset
    ldd xh, z + led_port_reg_ptr_offset + 1
    ldd r24, z + led_pin_offset
    rcall shift_bits
    ld r16, x
    or r16, r24
    st x, r16
    ret

;********************************************************************************
;* @brief Disables referenced LED.
;*
;* @param R25:R24 Reference to the LED.
;********************************************************************************
led_off:
    movw z, r24
    ldd xl, z + led_port_reg_ptr_offset
    ldd xh, z + led_port_reg_ptr_offset + 1
    ldd r24, z + led_pin_offset
    rcall shift_bits_inverted
    ld r16, x
    and r16, r24
    st x, r16
    ret

;********************************************************************************
;* @brief Toggles referenced LED.
;*
;* @param R25:R24 Reference to the LED.
;********************************************************************************
led_toggle:
    movw z, r24
    ldd r24, z + led_pin_offset
    rcall shift_bits
    ldd xl, z + led_pin_reg_ptr_offset
    ldd xh, z + led_pin_reg_ptr_offset + 1
    st x, r24
    ret

;********************************************************************************
;* @brief Indicates if referenced LED is enabled.
;*
;* @param R25:R24 Reference to the LED.
;*
;* @return 1 if the LED is enabled, else 0 (via R24).
;********************************************************************************
led_enabled:
    movw z, r24
    ldd xl, z + led_pin_reg_ptr_offset
    ldd xh, z + led_pin_reg_ptr_offset + 1
    ld r16, x
    ldd r24, z + led_pin_offset
    rcall shift_bits
    and r16, r24
    brne led_enabled_true
    led_enabled_false:
        ldi r24, 0x00
        ret
    led_enabled_true:
       ldi r24, 0x01
       ret

;********************************************************************************
;* @brief Provides the pin number on the associated port of referenced LED.
;*
;* @param R25:R24 Reference to the LED.
;*
;* @return The pin number 0 - 7 of the LED (via R24).
;********************************************************************************
led_get_pin:
    movw z, r24
    ldd r24, z + led_pin_offset
    ret

;********************************************************************************
;* @brief Provides the port register of referenced LED.
;*
;* @param R25:R24 Reference to the LED.
;*
;* @return Reference to the port register of the LED (via R25:R24).
;********************************************************************************
led_get_port_reg:
    movw z, r24
    ldd r24, z + led_port_reg_ptr_offset
    ldd r25, z + led_port_reg_ptr_offset + 1
    ret

;********************************************************************************
;* @brief Provides the pin register of referenced LED
;*
;* @param R25:R24 Reference to the LED.
;*
;* @return Reference to the pin register of the LED (via R25:R24).
;********************************************************************************
led_get_pin_reg:
    movw z, r24
    ldd r24, z + led_pin_reg_ptr_offset
    ldd r25, z + led_pin_reg_ptr_offset + 1
    ret

;********************************************************************************
;* @brief Provides the data direction register of referenced LED.
;*
;* @param R25:R24 Reference to the LED.
;*
;* @return Reference to the data direction register of the LED (via R25:R24).
;********************************************************************************
led_get_dir_reg:
    movw z, r24
    ldd r24, z + led_dir_reg_ptr_offset
    ldd r25, z + led_dir_reg_ptr_offset + 1
    ret

.endif ; led_inc_