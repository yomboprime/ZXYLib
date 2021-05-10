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

#ifndef ZXUNO_VERIPAC_H
#define ZXUNO_VERIPAC_H

#include "../integerTypes.h"

#define VERIPAC_DATA_REG 250
#define VERIPAC_STAT_REG 251

#define VERIPAC_RAM_LENGTH 0xCA

#define VERIPAC_CONTROL_REG VERIPAC_RAM_LENGTH
#define VERIPAC_KEYBOARD_REG ( VERIPAC_CONTROL_REG + 1 )
#define VERIPAC_ACCUMULATOR ( VERIPAC_CONTROL_REG + 2 )
#define VERIPAC_PROGRAM_COUNTER ( VERIPAC_CONTROL_REG + 3 )
#define VERIPAC_INSTRUCTION_REG ( VERIPAC_CONTROL_REG + 4 )
#define VERIPAC_DATA_COUNTER ( VERIPAC_CONTROL_REG + 5 )

#define VERIPAC_SCREEN_START 0xD0
#define VERIPAC_SCREEN_LENGTH 32
#define VERIPAC_NUM_LINES 2
#define VERIPAC_LINE_LENGTH ( VERIPAC_SCREEN_LENGTH / VERIPAC_NUM_LINES )

#define VERIPAC_NUM_REGS 16
#define VERIPAC_REGS_START 0xF0

extern void veripac9_reset();
extern void veripac9_step();

extern uint8_t veripac9_readMemory( uint8_t address );
extern void veripac9_writeMemory( uint8_t address, uint8_t value );

extern void veripac9_readAllMemory( uint8_t *buffer );
extern void veripac9_readRegistersAndScreen( uint8_t *buffer );
extern void veripac9_readScreen( uint8_t *buffer );


#endif /* ZXUNO_VERIPAC_H */
