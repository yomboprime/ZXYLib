/*
 * @author yomboprime
 *
 * Configuration program for the WiFi addon.
 * 
 * It enables the hardware handshaking of the WiFi module so it listens the
 * ZX-Uno RTS signal and waits for it to be low before transmitting more data.
 * 
 * It does so with the following AT command:
 * AT+UART_DEF=115200,8,1,0,2
 * 
*/
#include <stdio.h>
#include <stdlib.h>
#include <input.h>

#include "../src/textUtils.h"
#include "../src/zxuno/uart.h"

#define getKey() in_Inkey()

void main(void) {

    int c0 = -1, c1 = -1;
    bool end = false;
    bool success = false;

    textUtils_32ColumnsMode();
    textUtils_cls();

    textUtils_print( "WiFi addon configuration utility" );
    textUtils_println( "" );
    textUtils_println( "This program sends a command to" );
    textUtils_println( "the WiFi module to configure it" );
    textUtils_print( "so it listens to ZX-Uno RTS sig-" );
    textUtils_println( "nal." );
    textUtils_println( "" );
    textUtils_println( "It is harmless to execute this" );
    textUtils_println( "program more than once." );
    textUtils_println( "" );
    textUtils_println( "Press 'Y' key to start..." );

    while ( getKey() != 'Y' && getKey() != 'y' );
    
    textUtils_cls();
            
    textUtils_println( "Starting..." );

    
    // Initialize UART
    UART_begin();
    
    // Send the command
    UART_println( "AT+UART_DEF=115200,8,1,0,2" );
    
    delay( 1000 );
    
    // Send AT command to check if bidirectional communication is now possible
    UART_println( "AT" );
    
    // Listen for "OK"
    while ( ! end ) {
        if ( c0 == -1 ) {
            c0 = UART_read_timeout( 5000 );
            if ( c0 == -1 ) {
                end = true;
            }
        }
        else {
            c1 = UART_read_timeout( 5000 );
            if ( c1 == -1 ) {
                end = true;
            }
            else {
                if ( c0 == 'O' && c1 == 'K' ) {
                    success = true;
                    end = true;
                }
                else {
                    c0 = c1;
                }
            }
        }
    }
    
    if ( success == true ) {
        
        // Clear queue
        while ( UART_read_timeout( 1000 ) >= 0 );
        
        textUtils_println( "" );
        textUtils_println( "Command successful." );
        textUtils_println( "WiFi module firmware version:" );
        textUtils_println( "" );

        // Send firmware information retrieval command
        UART_println( "AT+GMR" );

        c0 = 0;
        while ( c0 >= 0 ) {
            c0 = UART_read_timeout( 2000 );
            if ( c0 >= 0 ) {
                fputc_cons( c0 );
            }
        }
        
        textUtils_println( "" );
        textUtils_println( "WiFi module was configured OK." );
        
    }
    else {
        textUtils_println( "" );
        textUtils_println( "Some error happened: Couldn't" );
        textUtils_println( "get response from WiFi module." );
        textUtils_println( "" );
        textUtils_println( "Make sure you have the WiFi" );
        textUtils_println( "addon module properly installed" );
        textUtils_print( "and you have selected the ZX-Uno" );
        textUtils_println( "core with UART support." );
    }
    
    textUtils_println( "End of program." );
    while ( 1 );

}
