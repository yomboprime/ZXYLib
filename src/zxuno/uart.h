/*

ZXLib

MIT License

Copyright (c) 2021 Juan Jose Luna Espinosa

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

// ZX-Uno UART Functions

#ifndef ZXUNO_UART_H
#define ZXUNO_UART_H

#include "../integerTypes.h"

#define UART_DATA_REG 0xc6
#define UART_STAT_REG 0xc7
#define UART_BYTE_RECEIVED_BIT 0x80
#define UART_BYTE_TRANSMITTING_BIT 0x40

extern void UART_begin();

extern void UART_writeByte( uint8_t value );
extern uint16_t UART_write( uint8_t* buf, uint16_t len );

extern void UART_print( uint8_t *s );
extern void UART_println( uint8_t *s );

extern int UART_available();
extern uint8_t UART_readBlocking();
extern int UART_read();
extern int UART_read_timeout( long timeout_ms );
extern int UART_peek();

extern int32_t UART_parseInt( long timeout_ms );
extern bool UART_find( uint8_t *s, long timeout_ms );

extern void delay( long ms );

#endif /* ZXUNO_UART_H */

