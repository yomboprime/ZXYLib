/*
 * @author yomboprime
 *
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
