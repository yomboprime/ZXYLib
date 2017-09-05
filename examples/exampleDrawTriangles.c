/*
 * @author yomboprime
 *
 * Simple UART Terminal
*/
#include <stdio.h>
#include <input.h>

#include "../src/textUtils.h"
#include "../src/zxuno/uart.h"

#define getKey() in_Inkey()

void main(void) {

    int c;
    uint8_t key;

    textUtils_32ColumnsMode();
    textUtils_cls();

    UART_begin();

    textUtils_println( "Opened the UART. Following is the input. You can type in the keyboard to send bytes." );

    while ( 1 ) {

        c = UART_read();
        if ( c >= 0 ) {

            if ( ( c >= 32 && c < 127 ) || c == '\n' || c == '\r' ) {
                fputc_cons( c );
            }
            else {
                fputc_cons( '-' );
            }

        }

        key = getKey();
        if ( key > 0 ) {
            while ( getKey() > 0 ) {
                // Wait key release
            }
            if ( key == 13 ) {
                UART_writeByte( 13 );
                UART_writeByte( 10 );
            }
            else {
                UART_writeByte( key );
            }
        }

    }

    textUtils_println( "End. Press a key." );
    waitKey();
}
