;********************************************************************************
;* @brief Assembly program for toggling a LED by pressing a button. Pin change
;*        interrupt is enabled on the button pin.
;********************************************************************************
.include "def.inc"
.include "dseg.inc"

;********************************************************************************
;* @brief Code segment, the location of the assembly code.
;********************************************************************************
.cseg

;********************************************************************************
;* @brief Reset vector, the start address of the program.
;********************************************************************************
.org reset_vect
    rjmp init_stack_pointer

;********************************************************************************
;* @brief Interrupt vector for PCINT0, i.e. pin change interrupts on I/O port B. 
;********************************************************************************
.org pcint0_vect
    rjmp isr_pcint0

;********************************************************************************
;* @brief Including drivers.
;********************************************************************************
.include "led.inc"
.include "button.inc"

;********************************************************************************
;* @brief Addresses for hardware structures.
;********************************************************************************
.equ led1_addr = ramend + 1              ; Starting address for led1.
.equ button1_addr = led1_addr + led_size ; Staring address for button1.

;********************************************************************************
;* @brief Toggles the LED if the button is pressed.
;********************************************************************************
isr_pcint0:
    ldi r24, low(button1_addr)
    ldi r25, high(button1_addr)
    rcall button_pressed
    cpi r24, 0x00
    breq isr_pcint0_end
    ldi r24, low(led1_addr)
    ldi r25, high(led1_addr)
    rcall led_toggle
    isr_pcint0_end:
        reti
 
;********************************************************************************
;* @brief Initializes stack pointer before starting the program. This is done
;*        automatically for all newer AVR devices, but it's still recommended 
;*        to initialize manually to ensure that the stack pointer is correctly 
;*        initalized after software resets.
;********************************************************************************
init_stack_pointer:
    ldi r16, low(ramend)
    ldi r17, high(ramend)
    out spl, r16
    out sph, r17
    rjmp main

;********************************************************************************
;* @brief Initializes the hardware, then keeps the program running as long as
;*        voltage is supplied. The rest of the program is interrupt driven.
;********************************************************************************
main:
    rcall setup
    main_loop:
            rjmp main_loop

;********************************************************************************
;* @brief Initializes the LED and button.
;********************************************************************************
setup:
    ldi r24, low(led1_addr)
    ldi r25, high(led1_addr)
    ldi r22, 8
    rcall led_init
    ldi r24, low(button1_addr)
    ldi r25, high(button1_addr)
    ldi r22, 13
    rcall button_init
    ldi r24, low(button1_addr)
    ldi r25, high(button1_addr)
    rcall button_enable_interrupt
    ret
