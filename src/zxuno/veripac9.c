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

#include "veripac9.h"

#include <stdlib.h>

#include "zxuno.h"

void veripac9_reset() {

    veripac9_writeMemory( VERIPAC_CONTROL_REG, 3 );
    veripac9_writeMemory( VERIPAC_CONTROL_REG, 0 );

}

void veripac9_step() {

    veripac9_writeMemory( VERIPAC_CONTROL_REG, 1 );
    veripac9_writeMemory( VERIPAC_CONTROL_REG, 0 );

}

/*
 * Read one memory position
 */
uint8_t veripac9_readMemory( uint8_t address ) {

    outp( ZXUNO_ADDR, VERIPAC_STAT_REG );
    outp( ZXUNO_REG, address );

    outp( ZXUNO_ADDR, VERIPAC_DATA_REG );
    return inp( ZXUNO_REG );

}

/*
 * Write one memory position
 */
void veripac9_writeMemory( uint8_t address, uint8_t value ) {

    outp( ZXUNO_ADDR, VERIPAC_STAT_REG );
    outp( ZXUNO_REG, address );

    outp( ZXUNO_ADDR, VERIPAC_DATA_REG );
    outp( ZXUNO_REG, value );

}

/*
 * Reads all memory positions (256)
 */
void veripac9_readAllMemory( uint8_t *buffer ) {

    uint16_t i;

    for ( i = 0; i < 256; i++ ) {

        outp( ZXUNO_ADDR, VERIPAC_STAT_REG );
        outp( ZXUNO_REG, (uint8_t) i );

        outp( ZXUNO_ADDR, VERIPAC_DATA_REG );
        *buffer++ = inp( ZXUNO_REG );

    }

}

/*
 * Reads only registers and screen buffer
 */
extern void veripac9_readRegistersAndScreen( uint8_t *buffer ) {

    uint16_t i;

    buffer+= VERIPAC_RAM_LENGTH;

    for ( i = VERIPAC_RAM_LENGTH; i < 256; i++ ) {

        outp( ZXUNO_ADDR, VERIPAC_STAT_REG );
        outp( ZXUNO_REG, (uint8_t) i );

        outp( ZXUNO_ADDR, VERIPAC_DATA_REG );
        *buffer++ = inp( ZXUNO_REG );

    }

}

/*
 * Reads only screen buffer
 */
extern void veripac9_readScreen( uint8_t *buffer ) {

    uint16_t i;

    buffer+= VERIPAC_SCREEN_START;

    for ( i = VERIPAC_SCREEN_START; i < VERIPAC_SCREEN_START + VERIPAC_SCREEN_LENGTH; i++ ) {

        outp( ZXUNO_ADDR, VERIPAC_STAT_REG );
        outp( ZXUNO_REG, (uint8_t) i );

        outp( ZXUNO_ADDR, VERIPAC_DATA_REG );
        *buffer++ = inp( ZXUNO_REG );

    }

}
