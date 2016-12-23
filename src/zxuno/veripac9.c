/*
 * @author yomboprime
 *
*/

#include "veripac9.h"

#include <stdlib.h>

#include "zxuno.h"
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
 * Write one memory position
 */
void veripac9_writeMemory( uint8_t address, uint8_t value ) {

    outp( ZXUNO_ADDR, VERIPAC_STAT_REG );
    outp( ZXUNO_REG, address );

    outp( ZXUNO_ADDR, VERIPAC_DATA_REG );
    outp( ZXUNO_REG, value );

}
