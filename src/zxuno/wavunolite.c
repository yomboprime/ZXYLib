/*
 * @author yomboprime
 *
 * ZX-Uno UART functions
*/

#include "wavunolite.h"

#include <stdlib.h>

#include "zxuno.h"

void resetWavunoIntUserPointer() {

    outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
    outp( ZXUNO_REG, WAVUNO_REG_INT_SAMPLE_RESET );
    outp( ZXUNO_ADDR, WAVUNO_DATA_REG );
    outp( ZXUNO_REG, 0 );

}

void setWavunoIntBeginEnd( uint32_t beginAddress, uint32_t endAddress ) {

    // TODO remove "& 0xFF"

    outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
    outp( ZXUNO_REG, WAVUNO_REG_INT_START_LOOP1 );
    outp( ZXUNO_ADDR, WAVUNO_DATA_REG );
    outp( ZXUNO_REG, ( beginAddress >> 8 ) & 0x0FF );
    outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
    outp( ZXUNO_REG, WAVUNO_REG_INT_START_LOOP0 );
    outp( ZXUNO_ADDR, WAVUNO_DATA_REG );
    outp( ZXUNO_REG, beginAddress & 0x0FF );

    outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
    outp( ZXUNO_REG, WAVUNO_REG_INT_END_LOOP1 );
    outp( ZXUNO_ADDR, WAVUNO_DATA_REG );
    outp( ZXUNO_REG, ( endAddress >> 8 ) & 0x0FF );
    outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
    outp( ZXUNO_REG, WAVUNO_REG_INT_END_LOOP0 );
    outp( ZXUNO_ADDR, WAVUNO_DATA_REG );
    outp( ZXUNO_REG, endAddress & 0x0FF );

}
