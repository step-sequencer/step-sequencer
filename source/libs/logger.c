#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

#ifndef BAUD
#define BAUD 9600
#endif

#include "logger.h"
#include <avr/io.h>
#include <util/setbaud.h>

uint8_t loglibIsInitialized = 0;

void init()
{
    /* 
     UBRR(USART Baud Rate Register)
     Sets upper and lower bytes of the calculated prescaler value for baud with setbaud.h library
     This header file requires that on entry values are already defined for F_CPU and BAUD.
     http://www.nongnu.org/avr-libc//user-manual/setbaud_8h_source.html
    */
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

    /* 
     UCSZ20 (part of UCSR0B), UCSZ01 and UCSZ00 (both part of UCSR0C) control the data size.
     Possible sizes: 5-bit (000), 6-bit (001), 7-bit (010), 8-bit (011) and 9-bit (111).
     Configure data frame size to 8-bits -> UCSZ20 = 0, UCSZ01 = 1 and UCSZ00 = 1
    */
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    // Enable transmitter with TXEN0 (part of UCSR0B)
    UCSR0B = (1 << TXEN0);

    loglibIsInitialized = 1;
}

void uart_putchar(char c)
{
    /* 
     We need to check if register is ready to be written.
     This is the case if UDRE0 (called UART Data Register Emty, part of UCSR0A) is set to 1
     Alternativly one could also wait for TXC0 (Set when all data has transmitted, part of UCSR0A)
    */
    loop_until_bit_is_set(UCSR0A, UDRE0);

    // Data can be transmitted by writing a byte to UDR0 (UART Data Register)
    UDR0 = c;
}

void usb_log(char *string)
{
    if (!loglibIsInitialized)
    {
        init();
    }
    // Loop until end of string writing char by char.
    while (*string)
    {
        uart_putchar(*string++);
    }
}
