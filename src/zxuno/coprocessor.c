
#include "coprocessor.h"
#include "zxuno.h"
#include <stdlib.h>

void coprocessorMultAddInit() {
    outp( ZXUNO_ADDR, COPROCESSOR_REG );
}

// Performs (a * b + c) in 16 bit 2's complement 9.7 fixed point arithmetic
int16_t coprocessorMultAdd( int16_t a, int16_t b,  int16_t c ) {
    
    int16_t result = 0;
    uint8_t r0 = 0;
    uint8_t r1 = 0;

    outp( ZXUNO_ADDR, COPROCESSOR_REG );
    
    // Write lo A
    outp( ZXUNO_REG, (uint8_t)( a ) );
    
    // Write hi A
    outp( ZXUNO_REG, (uint8_t)( ( a & 0xFF00 ) >> 8 ) );

    // Write lo B
    outp( ZXUNO_REG, (uint8_t)( b ) );
    
    // Write hi B
    outp( ZXUNO_REG, (uint8_t)( ( b & 0xFF00 ) >> 8 ) );
    
    // Write lo C
    outp( ZXUNO_REG, (uint8_t)( c ) );
    
    // Write hi C
    outp( ZXUNO_REG, (uint8_t)( ( c & 0xFF00 ) >> 8 ) );
    
    
    // Read result
    r0 = inp( ZXUNO_REG );
    r1 = inp( ZXUNO_REG );

    result = ( r1 << 8 ) | r0;

    return result;
    
}

// Performs (a * b + c) in 16 bit 2's complement 9.7 fixed point arithmetic
// Warning: Call coprocessorMultAddInit first.
// Optimized in embedded assembler
int16_t coprocessorMultAddOptimized( int16_t cc, int16_t bb,  int16_t aa ) {

    #asm
    
        ; zxuno register port
        ld bc, ZXUNO_REG

        ; get sp and skip return address
        ld hl, 2
        add hl, sp

        ; first param from the end ( a )
        
        ; low byte
        ld a, (hl)
        inc hl
        out (c), a
        
        ; high byte
        ld a, (hl)
        inc hl
        out (c), a

        ; second param from the end ( b )
        
        ; low byte
        ld a, (hl)
        inc hl
        out (c), a
        
        ; high byte
        ld a, (hl)
        inc hl
        out (c), a

        ; third param from the end ( c )
        
        ; low byte
        ld a, (hl)
        inc hl
        out (c), a
        
        ; high byte
        ld a, (hl)
        inc hl
        out (c), a

        ; Return 16 bit number in hl
        in l, (c)
        in h, (c)

    #endasm

}

/*
// Performs a * b + c in 8.8 fixed point 2's complement arithmetic
int16_t coprocessorMultAdd88( int16_t a, int16_t b,  int16_t c ) {
    
    int32_t result = 0;
    uint8_t r0 = 0;
    uint8_t r1 = 0;

    outp( ZXUNO_ADDR, COPROCESSOR_REG );
    
    // Write lo A
    outp( ZXUNO_REG, (uint8_t)( a ) );
    
    // Write hi A
    outp( ZXUNO_REG, (uint8_t)( ( a & 0xFF00 ) >> 8 ) );

    // Write lo B
    outp( ZXUNO_REG, (uint8_t)( b ) );
    
    // Write hi B
    outp( ZXUNO_REG, (uint8_t)( ( b & 0xFF00 ) >> 8 ) );
    
    // Write lo C
    outp( ZXUNO_REG, (uint8_t)( c ) );
    
    // Write hi C
    outp( ZXUNO_REG, (uint8_t)( ( c & 0xFF00 ) >> 8 ) );
    
    // Read result, ignoring the outer bytes
    // Ignore first byte
    inp( ZXUNO_REG );
    
    r0 = inp( ZXUNO_REG );
    r1 = inp( ZXUNO_REG );
    
    // Ignore last byte
    inp( ZXUNO_REG );

    
    result = ( ( (uint16_t) r1 ) << 8 ) | r0;
    
    return result;
    
}

// Performs a * b + c in 8.8 fixed point 2's complement arithmetic
// Warning: it doesn't select the coprocessor register: outp( ZXUNO_ADDR, COPROCESSOR_REG );
// Optimized in embedded assembler
int16_t coprocessorMultAdd88Optimized( int16_t cc, int16_t bb,  int16_t aa ) {

    #asm
    
        ; zxuno register port
        ld bc, ZXUNO_REG

        ; get sp and skip return address
        ld hl, 2
        add hl, sp

        ; first param from the end ( a )
        ; low byte
        ld a, (hl)
        inc hl
        out (c), a
        ; high byte
        ld a, (hl)
        inc hl
        out (c), a

        ; second param from the end ( b )
        ; low byte
        ld a, (hl)
        inc hl
        out (c), a
        ; high byte
        ld a, (hl)
        inc hl
        out (c), a

        ; third param from the end ( c )
        ; low byte
        ld a, (hl)
        inc hl
        out (c), a
        ; high byte
        ld a, (hl)
        inc hl
        out (c), a

        ; Return 16 bit number in hl
        in a, (c) ; Ignore first byte
        in l, (c)
        in h, (c)
        in a, (c) ; Ignore last byte


    #endasm

}
*/
