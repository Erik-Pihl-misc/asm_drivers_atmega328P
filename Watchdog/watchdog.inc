;/********************************************************************************
;* @brief Driver for the ATmega328P watchdog timer.
;********************************************************************************/
.ifndef watchdog_inc_
.equ watchdog_inc_ = 0

;/********************************************************************************
;* @brief Watchdog timeout parameters.
;********************************************************************************/
.equ watchdog_timeout_16ms   = 0                                       ; 16 ms
.equ watchdog_timeout_32ms   = (1 << WDP0)                             ; 32 ms
.equ watchdog_timeout_64ms   = (1 << WDP1)                             ; 64 ms
.equ watchdog_timeout_128ms  = (1 << WDP1) | (1 << WDP0)               ; 128 ms
.equ watchdog_timeout_256ms  = (1 << WDP2)                             ; 256 ms
.equ watchdot_timeout_512ms  = (1 << WDP2) | (1 << WDP0)               ; 512 ms
.equ watchdog_timeout_1024ms = (1 << WDP2) | (1 << WDP1)               ; 1024 ms
.equ watchdog_timeout_2048ms = (1 << WDP2) | (1 << WDP1) | (1 << WDP0) ; 2048 ms
.equ watchdog_timeout_4096ms = (1 << WDP3)                             ; 4096 ms 
.equ watchdog_timeout_8192ms = (1 << WDP3) | (1 << WDP0)               ; 8192 ms

;/********************************************************************************
;* @brief Initializes the watchdog timer and sets the selected timeout.
;*
;* @param R24 The timeout of the watchdog timer.
;********************************************************************************/
watchdog_init:
    cli
    ldi r16, (1 << wdce) | (1 << wde)
    sts wdtcsr, r16
    sts wdtcsr, r24
    sei
    ret

;/********************************************************************************
;* @brief Resets the watchdog timer. This subroutine must be called before the
;*        watchdog timer elapses, else the system will be reset (or a watchdog
;*        interrupt will be generated, depending on the selected mode of the
;*        watchdog timer).
;********************************************************************************/
watchdog_reset:
    cli
    wdr
    lds r24, mcusr
    andi r24, ~(1 << wdrf)
    sts mcusr, r24
    sei
    ret

;/********************************************************************************
;* @brief Enables system reset so that the system will be reset if the watchdog
;*        timer elapses, which occurs if the watchdog timer isn't reset in time.
;********************************************************************************/
watchdog_enable_system_reset:
    rcall watchdog_reset
    cli
    lds r24, wdtcsr
    ori r24, (1 << wdce) | (1 << wde)
    sts wdtcsr, r24
    lds r24, wdtcsr
    ori r24, (1 << wde)
    sts wdtcsr, r24
    sei
    ret

;/********************************************************************************
;* @brief Disables system reset so that the system won't be reset even if the
;*        watchdog timer elapses.
;********************************************************************************/
watchdog_disable_system_reset:
    rcall watchdog_reset
    cli
    lds r24, wdtcsr
    ori r24, (1 << wdce) | (1 << wde)
    sts wdtcsr, r24
    lds r24, wdtcsr
    andi r24, ~(1 << wde)
    sts wdtcsr, r24
    sei
    ret

;/********************************************************************************
;* @brief Enables watchdog timer interrupt so that an interrupt will occur if
;*        the watchdog timer elapses, which occurs if it isn't reset in time.
;********************************************************************************/
watchdog_enable_interrupt:
    rcall watchdog_reset
    cli
    lds r24, wdtcsr
    ori r24, (1 << wdce) | (1 << wde)
    sts wdtcsr, r24
    lds r24, wdtcsr
    ori r24, (1 << wdie)
    sts wdtcsr, r24
    sei
    ret

;/********************************************************************************
;* @brief Disables watchdog timer interrupt so that no interrupt will occur
;*        if the watchdog timer elapses.
;********************************************************************************/
watchdog_disable_interrupt:
    rcall watchdog_reset
    cli
    lds r24, wdtcsr
    ori r24, (1 << wdce) | (1 << wde)
    sts wdtcsr, r24
    lds r24, wdtcsr
    andi r24, ~(1 << wdie)
    sts wdtcsr, r24
    sei
    ret

.endif ; watchdog_inc_