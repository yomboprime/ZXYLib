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

#ifndef COPROCESSOR_H
#define COPROCESSOR_H

#include "../integerTypes.h"

// Coprocessor read/write register in the ZX-Uno register space
#define COPROCESSOR_REG (0xC0)

// Selects the coprocessor ZX-Uno register
extern void coprocessorMultAddOptimizedInit();

// Performs (a * b + c) in 16 bit 2's complement 9.7 fixed point arithmetic
extern int16_t coprocessorMultAdd( int16_t a, int16_t b,  int16_t c );

// Performs (a * b + c) in 16 bit 2's complement 9.7 fixed point arithmetic. Optimized in assembler.
extern int16_t coprocessorMultAddOptimized( int16_t cc, int16_t bb,  int16_t aa );

/*
// Performs a * b + c in 8.8 fixed notation
extern int16_t coprocessorMultAdd88( int16_t a, int16_t b,  int16_t c );

// Performs a * b + c in 8.8 fixed notation. Optimized in assembler.
// Call coprocessorMultAddOptimizedInit once, then lots of calls to this function without changing ZX-Uno registers
extern int16_t coprocessorMultAdd88Optimized( int16_t cc, int16_t bb,  int16_t aa );
*/

#endif // COPROCESSOR_H
