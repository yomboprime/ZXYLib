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

// ZX-Uno UART functions

#include "uart.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "zxuno.h"
#include "../textUtils.h"


// If true, then there is a byte already read in memory
bool UART_pokeByte;

// The already read byte
uint8_t UART_byteBuffer;

// If true, then there is a byte already received in the uart, waiting to be read
bool UART_bitReceived;

void UART_begin() {

    // Cleans the uart flags by reading the registers

    outp( ZXUNO_ADDR, UART_STAT_REG );
    inp( ZXUNO_REG );

    outp( ZXUNO_ADDR, UART_DATA_REG );
    inp( ZXUNO_REG );

    // Clears the "byte received in the memory buffer" flag
    UART_pokeByte = false;

    // Clears the bit "byte received in the UART"
    UART_bitReceived = false;

}

void UART_writeByte( uint8_t value ) {

    uint8_t stat = 0;

    outp( ZXUNO_ADDR, UART_STAT_REG );
    while ( ( stat & UART_BYTE_TRANSMITTING_BIT ) == UART_BYTE_TRANSMITTING_BIT ) {

        // Wait transmission of previous byte

        stat = inp( ZXUNO_REG );

        if ( stat & UART_BYTE_RECEIVED_BIT == UART_BYTE_RECEIVED_BIT ) {

            // A byte has been received in the uart. Store that info because that bit
            // is reset when reading the register that contains it.
            UART_bitReceived = true;

        }

    }

    // Write the byte to the uart
    outp( ZXUNO_ADDR, UART_DATA_REG );
    outp( ZXUNO_REG, value );

}

uint16_t UART_write( uint8_t* buf, uint16_t len ) {

    // Caution: exactly 64 KB (65536 bytes) is not supported

    uint16_t i;

    for ( i = 1; i <= len && i > 0; i++ ) {

        UART_writeByte( *buf++ );

    }

    return len;

}

void UART_print( uint8_t *s ) {

    while ( *s != 0 ) {

        UART_writeByte( *s++ );

    }

}

void UART_println( uint8_t *s ) {

    while ( *s != 0 ) {

        UART_writeByte( *s++ );

    }

    UART_writeByte( 13 );
    UART_writeByte( 10 );

}

int UART_available() {

    // Returns number of available bytes to read instantly (0 or 1)

    int c;

    if ( UART_pokeByte  == true || UART_bitReceived == true ) {
        // There is one byte already read in memory or in the UART
        return 1;
    }

    c = UART_read();

    if ( c < 0 ) {

        // No byte available
        return 0;

    }

    // Store the read byte and return 1 byte available
    UART_pokeByte = true;
    UART_byteBuffer = (uint8_t)c;

    return 1;

}

uint8_t UART_readBlocking() {

    // This function is blocking, it waits for a byte to be received if there is no byte in the buffer

    uint8_t value;

    if ( UART_pokeByte  == true ) {

        UART_pokeByte = false;
        return UART_byteBuffer;

    }

    if ( UART_bitReceived == true ) {

        outp( ZXUNO_ADDR, UART_DATA_REG );
        value = inp( ZXUNO_REG );
        UART_bitReceived = false;

    }
    else {

        outp( ZXUNO_ADDR, UART_STAT_REG );
        while ( ( inp( ZXUNO_REG ) & UART_BYTE_RECEIVED_BIT ) == 0 ) {
            // Wait until byte is received
        }

        outp( ZXUNO_ADDR, UART_DATA_REG );
        value = inp( ZXUNO_REG );

    }

    return value;

}

int UART_read() {

    // This function looks without blocking to see if a byte has been received.
    // In affirmative case it returns the value
    // In case there is no received byte it returns -1

    int value;

    if ( UART_pokeByte  == true ) {

        UART_pokeByte = false;
        return (int)UART_byteBuffer;

    }

    if ( UART_bitReceived == true ) {

        outp( ZXUNO_ADDR, UART_DATA_REG );
        value = inp( ZXUNO_REG );
        UART_bitReceived = false;

    }
    else {
        outp( ZXUNO_ADDR, UART_STAT_REG );
        if ( ( inp( ZXUNO_REG ) & UART_BYTE_RECEIVED_BIT ) == UART_BYTE_RECEIVED_BIT ) {

            outp( ZXUNO_ADDR, UART_DATA_REG );
            value = inp( ZXUNO_REG );

        }
        else {

            value = -1;

        }
    }

    return value;

}

int UART_read_timeout( long timeout_ms ) {

    // Wait for a byte a specified time in ms
    // If no byte was available in the given time, returns -1

    int c;
    long t0;

	//timeout_ms += timeout_ms >> 2;
	t0 = millis();

    while ( millis() - t0 < timeout_ms ) {
        c = UART_read();
        if ( c >= 0 ) {
            return c;
        }
    }

    return -1;

}

int UART_peek() {

    // Read an available byte without consuming it.
    // Returns -1 if no byte was available.

    int c;

    if ( UART_pokeByte  == true ) {

        // There is a byte already read in memory
        return UART_byteBuffer;

    }

    c = UART_read();

    if ( c < 0 ) {

        // No byte available
        return -1;

    }

    // Store the read byte and return it
    UART_pokeByte = 1;
    UART_byteBuffer = (uint8_t)c;

    return UART_byteBuffer;


}

int32_t UART_parseInt( long timeout_ms ) {

    // Parses a string representing an integer.
    // Returns -1 if timeoud passed.

    int32_t value = 0;
    int c_int;
    uint8_t c;
    uint8_t numChars = 0;
    uint8_t minusSign = false;
    long t0;

	//timeout_ms += timeout_ms >> 2;
	t0 = millis();

    while ( 1 ) {

        if ( millis() - t0 > timeout_ms ) {
            return -1;
        }

        c_int = UART_peek();
        if ( c_int >= 0 ) {

            c = (uint8_t)c_int;

            if ( textUtils_isDigit( c ) ) {

                c -= '0';

                value = value * 10 + c;

                c_int = UART_read();
                numChars++;

            }
            else if ( c == '-' && numChars == 0 ) {

                minusSign = true;
                c_int = UART_read();
                numChars++;

            }
            else {

                if ( numChars > 0 ) {

                    // Correct number entered.
                    if ( minusSign == true ) {
                        value = - value;
                    }

                    return value;

                }
                else {

                    // Skip non numeric char at beginning
                    c_int = UART_read();

                }
            }
        }
    }
}

bool UART_find( uint8_t *s, long timeout_ms ) {

    // Reads

    int c_int;
    uint8_t c;
    uint8_t *ps = s;
    uint8_t l = strlen( s );
    uint8_t numChars = 0;
    long t;

    if ( l == 0 ) {
        // Umm should it return true here instead?
        return false;
    }

    //timeout_ms += timeout_ms >> 2;
    t = millis();

    while ( millis() - t < timeout_ms ) {

        c_int = UART_read();

        if ( c_int >= 0 ) {

            c = (uint8_t)c_int;
			//fputc_cons( c );

            if ( *ps == c ) {

                numChars++;

                if ( numChars == l ) {

                    return true;

                }

                ps++;

            }
            else {

                ps = s;
                numChars = 0;

            }

        }

    }

    return false;
}
