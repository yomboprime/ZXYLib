/*
 * @author yomboprime
 *
 * ZX-Uno Turbo Functions
*/

#ifndef ZXUNO_TURBO_H
#define ZXUNO_TURBO_H

#include "../integerTypes.h"

#define ZXUNO_TURBO_X1 0x00
#define ZXUNO_TURBO_X2 0x40
#define ZXUNO_TURBO_X4 0x80
#define ZXUNO_TURBO_X8 0xC0

#define ZXUNO_TURBO_MASK 0xC0

extern uint8_t TURBO_get();
extern void TURBO_set( uint8_t turboMode );

#endif /* ZXUNO_TURBO_H */

