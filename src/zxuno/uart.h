/*
 * @author yomboprime
 *
 * ZX-Uno UART Functions
*/

#ifndef ZXUNO_UART_H
#define ZXUNO_UART_H

#include "../integerTypes.h"

#define UART_DATA_REG 250
#define UART_STAT_REG 251
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
extern int UART_peek();

extern int32_t UART_parseInt();
extern bool UART_find( uint8_t *s );

#endif /* ZXUNO_UART_H */

