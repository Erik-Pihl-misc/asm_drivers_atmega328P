;********************************************************************************
;* @brief Driver for buttons via a structure, implemented as shown below:
;*
;*        struct button {
;*            volatile uint8_t* port_reg;
;*            volatile uint8_t* pin_reg;
;*            volatile uint8_t* dir_reg;
;*            volatile uint8_t* pcmsk_reg;
;*            uint8_t pcie_bit;
;*            uint8_t pin;
;*        };
;********************************************************************************
.ifndef button_inc_
.equ button_inc_ = 0

;********************************************************************************
;* @brief Includes utility subroutines for bit shifting.
;********************************************************************************
.include "utils.inc"

;********************************************************************************
;* @brief Parameters associated with the button driver.
;********************************************************************************
.equ button_pin_reg_ptr_offset   = 0  ; Offset for pin register pointer.
.equ button_dir_reg_ptr_offset   = 2  ; Offset for data direction register pointer.
.equ button_port_reg_ptr_offset  = 4  ; Offset for port register pointer.
.equ button_pcmsk_reg_ptr_offset = 6  ; Offset for pin change mask register pointer.
.equ button_pcie_bit_offset      = 8  ; Offset for pin change interrupt enable bit.
.equ button_pin_offset           = 9  ; Offset for pin number.
.equ button_size                 = 10 ; Size of button structure in bytes.

;********************************************************************************
;* @brief Initializes button on specified I/O port B or D.
;*
;* @param R25:R24 Reference to the button
;* @param R22     The button's pin number on Arduino Uno (0 - 13).
;*
;* @return 0 upon successful initialization, error code 1 if the specified pin
;*         number is invalid (via R24).
;********************************************************************************
button_init:
    movw z, r24
    cpi r22, 8
    brlo button_init_portd
    cpi r22, 14
    brlo button_init_portb
    button_init_error:
        ldi r24, 0x01
        ret
    button_init_portb:
        subi r22, 8
        ldi r24, low(pinb + 0x20)
        ldi r25, high(pinb + 0x20)
        ldi r20, low(pcmsk0)
        ldi r21, high(pcmsk0)
        ldi r18, pcie0
        rjmp button_init_structure
    button_init_portd:
        ldi r24, low(pind + 0x20)
        ldi r25, high(pind + 0x20)
        ldi r20, low(pcmsk2)
        ldi r21, high(pcmsk2)
        ldi r18, pcie2
        rjmp button_init_structure
    button_init_structure:
        std z + button_pin_reg_ptr_offset, r24
        std z + button_pin_reg_ptr_offset + 1, r25
        adiw r24, 1
        std z + button_dir_reg_ptr_offset, r24
        std z + button_dir_reg_ptr_offset + 1, r25
        adiw r24, 1
        std z + button_port_reg_ptr_offset, r24
        std z + button_port_reg_ptr_offset + 1, r25
        std z + button_pcmsk_reg_ptr_offset, r20
        std z + button_pcmsk_reg_ptr_offset + 1, r21
        std z + button_pcie_bit_offset, r18
        std z + button_pin_offset, r22
    button_init_hardware:
        mov r24, r22
        rcall shift_bits_inverted
        ldd xl, z + button_dir_reg_ptr_offset
        ldd xh, z + button_dir_reg_ptr_offset + 1
        ld r16, x
        and r16, r24
        st x, r16
        ldd r24, z + button_pin_offset
        rcall shift_bits
        ldd xl, z + button_port_reg_ptr_offset
        ldd xh, z + button_port_reg_ptr_offset + 1
        ld r16, x
        or r16, r24
        st x, r16
    button_init_end:
        ldi r24, 0x00
        ret

;********************************************************************************
;* @brief Indicates if referenced button is pressed.
;*
;* @param R25:R24 Reference to the button
;*
;* @return 1 if the button is pressed, else 0.
;********************************************************************************
button_pressed:
    movw z, r24
    ldd r24, z + button_pin_offset
    rcall shift_bits
    ldd xl, z + button_pin_reg_ptr_offset
    ldd xh, z + button_pin_reg_ptr_offset + 1
    ld r16, x
    and r16, r24
    brne button_is_pressed_true
    button_is_pressed_false:
        ldi r24, 0x00
        ret 
    button_is_pressed_true:
        ldi r24, 0x01
        ret

;********************************************************************************
;* @brief Enables pin change interrupt on referenced button.
;*
;* @param R25:R24 Reference to the button.
;********************************************************************************
button_enable_interrupt:
    movw z, r24
    ldd r24, z + button_pcie_bit_offset
    rcall shift_bits
    lds r16, pcicr
    or r16, r24
    sts pcicr, r16
    ldd r24, z + button_pin_offset
    rcall shift_bits
    ldd xl, z + button_pcmsk_reg_ptr_offset
    ldd xh, z + button_pcmsk_reg_ptr_offset + 1
    ld r16, x
    or r16, r24
    st x, r16
    sei
    ret

;********************************************************************************
;* @brief Disables pin change interrupt on referenced button.
;*
;* @param R25:R24 Reference to the button.
;********************************************************************************
button_disable_interrupt:
    movw z, r24
    ldd r24, z + button_pin_reg_ptr_offset
    rcall shift_bits_inverted
    ldd xl, z + button_pcmsk_reg_ptr_offset
    ldd xh, z + button_pcmsk_reg_ptr_offset + 1
    ld r16, x
    and r16, r24
    st x, r16
    ret

;********************************************************************************
;* @brief Indicates if pin change interrupt is enabled on referenced button.
;*
;* @param R25:R24 Reference to the button.
;*
;* @return 1 if pin change interrupt is enabled, else 0.
;********************************************************************************
button_interrupt_enabled:
    movw z, r24
    ldd r24, z + button_pin_offset
    rcall shift_bits
    ldd xl, z + button_pcmsk_reg_ptr_offset
    ldd xh, z + button_pcmsk_reg_ptr_offset + 1
    ld r16, x
    and r16, r24
    brne button_interrupt_enabled_true
    button_interrupt_enabled_false:
        ldi r24, 0x01
        ret
    button_interrupt_enabled_true:
        ldi r24, 0x00
        ret

;********************************************************************************
;* @brief Toggles pin change interrupt on referenced button.
;*
;* @param R25:R24 Reference to the button.
;********************************************************************************
button_toggle_interrupt:
    movw z, r24
    rcall button_interrupt_enabled
    mov r16, r24
    movw r24, z
    cpi r16, 0x00
    brne button_toggle_interrupt_enable
    button_toggle_interrupt_disable:
        rcall button_disable_interrupt
        ret
    button_toggle_interrupt_enable:
        rcall button_enable_interrupt
        ret
  
.endif ; button_inc_

