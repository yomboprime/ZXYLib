/*
 * @author yomboprime
 *
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
