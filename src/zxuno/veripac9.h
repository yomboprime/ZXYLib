/*
 * @author yomboprime
 *
 * ZX-Uno UART Functions
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

extern void veripac9_readAllMemory( uint8_t *buffer );

extern void veripac9_writeMemory( uint8_t address, uint8_t value );

#endif /* ZXUNO_VERIPAC_H */
