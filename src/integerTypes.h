/*
 * @author yomboprime
 *
 * Standard types definition
*/

#ifndef INTEGERTYPES_H
#define INTEGERTYPES_H

#include <stdint.h>

typedef u8_t uint8_t;       /* 8 bit type */
typedef u16_t uint16_t;     /* 16 bit type */
typedef u32_t uint32_t;     /* 32 bit type */

typedef i8_t int8_t;        /* 8 bit signed type */
typedef i16_t int16_t;      /* 16 bit signed type */
typedef i32_t int32_t;      /* 32 bit signed type */

typedef uint8_t bool;

#define true (1)
#define false (0)

#ifndef NULL
#define NULL ((void*)0)
#endif

#endif /* INTEGERTYPES_H */

