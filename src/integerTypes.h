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

// Standard types definition

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

