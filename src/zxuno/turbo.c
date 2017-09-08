/*
 * @author yomboprime
 *
 * ZX-Uno Turbo functions
*/

#include "turbo.h"

#include <stdlib.h>

#include "zxuno.h"


uint8_t TURBO_get() {

    // Gets current turbo mode

    outp( ZXUNO_ADDR, SCANDBLCTRL_REG );
    return inp( ZXUNO_REG ) & ZXUNO_TURBO_MASK;

}

void TURBO_set( uint8_t turboMode ) {

    // Sets a turbo mode.

    outp( ZXUNO_ADDR, SCANDBLCTRL_REG );
    outp( ZXUNO_REG, ( inp( ZXUNO_REG ) & ( ~ ZXUNO_TURBO_MASK ) ) | turboMode );

}
