/*
 * @author yomboprime
 *
 * FelicesFiestas demo 2017
 * 
 * 
*/
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <input.h>

#include "../src/zxthree/screen/screen.h"
#include "../src/zxthree/impl/zxradas/screen/scrradas.h"
#include "../src/zxthree/raster/triangle/triangle.h"
#include "../src/zxuno/turbo.h"

#define getKey() in_Inkey()

// Constants

#define imin( a, b ) ( a > b ? b : a )
#define imax( a, b ) ( a < b ? b : a )
#define iabs( a ) ( a < 0 ? -a : a )


// Global variables

struct in_UDK keyboardMapping;

uint8_t palette[ 16 ];    
int16_t b;
int16_t seed;
uint8_t *vram;
int16_t i, j;

int16_t triCoords[ 6 ];

uint8_t turbo;

uint8_t colorOffset = 0;
uint8_t c;

// Estrella
typedef struct {
    int16_t x;
    int16_t y;
    int16_t tam;
    uint8_t color;
    uint8_t crecimiento;
    uint8_t decrecimiento;
} Estrella;

#define NUM_ESTRELLAS 9
Estrella estrellas[ NUM_ESTRELLAS ];

Estrella *e;

#define bip() bit_frequency( 0.1, 400.0 )


// Function prototypes
uint8_t readKeyboardAndJoy();
void setupEstrella( Estrella *e);
void pintaEstrella( Estrella *estrella );

// MAIN
void main() {
    
    #asm
        ei
    #endasm
    
    // Save turbo and set turbo X8
    turbo = TURBO_get();
    TURBO_set( ZXUNO_TURBO_X8 );

    seed = 1234;
    srand( seed );

    // Initialize
    
    // Set Radastan video mode
    setVideoMode( VIDEO_MODE_RADASTAN );

    // Set color palette
    palette[ 0 ] = 0x00;
    for ( i = 1; i < 16; i++ ) {
        palette[ i ] = (uint8_t)( rand() & 0x0FF );
    }
    setPalette( palette );

    // Setup keyboard & joystick
    keyboardMapping.fire = in_LookupKey( ' ' );
    keyboardMapping.left = in_LookupKey( 'o' );
    keyboardMapping.right = in_LookupKey( 'p' );
    keyboardMapping.down = in_LookupKey( 'a' );
    keyboardMapping.up = in_LookupKey( 'q' );

    seed = clock();
    
    for ( i = 0; i < NUM_ESTRELLAS; i++ ) {    
        setupEstrella( &estrellas[ i ] );
    }

    // MAIN LOOP
    while ( readKeyboardAndJoy() != in_FIRE ) {

        // Get pointer to current drawing screen
        vram = getVRAMPointer();

        // Clear background
        memset( vram, 0x00, screenNumBytes );

        
        if ( colorOffset == 14 ) {
            colorOffset = 0;
        }
        else {
            colorOffset++;
        }

        for ( i = 0; i < NUM_ESTRELLAS; i++ ) {
            
            e = &estrellas[ i ];
            
            // Procesa estrella
            if ( e->crecimiento > 0 ) {
                e->tam+= 2;
                e->crecimiento--;
            }
            else if ( e->decrecimiento > 0 ) {
                e->tam-= 2;
                e->decrecimiento--;
            }
            else {
                setupEstrella( e );
            }
            
            c = ( colorOffset + i ) & 0x0F;
            if ( ! c ) {
                c = rand() & 0x0F;
            }
            c = c | ( c << 4 );
            e->color = c;
            
            // Pinta estrella
            
            pintaEstrella( e );
        }

       
        // Blit the screen
        blitVRAM();

    }

    setVideoMode( VIDEO_MODE_ZX_DEFAULT );

    TURBO_set( turbo );

}

uint8_t readKeyboardAndJoy() {
    return ((void*)in_JoyKeyboard)( &keyboardMapping ) | ((void*)in_JoySinclair1)( &keyboardMapping );
}

void pintaEstrella( Estrella *estrella ) {

    int16_t t = estrella->tam;
    int16_t t2 = t >> 1;
    int16_t x = estrella->x;
    int16_t y = estrella->y;
    int16_t xp = x + t2;
    int16_t xm = x - t2;
    int16_t yp = y + t2;
    int16_t ym = y - t2;
    uint8_t c = estrella->color;

    b = 0;
    triCoords[ b++ ] = x;
    triCoords[ b++ ] = y - t;
    triCoords[ b++ ] = xp;
    triCoords[ b++ ] = y;
    triCoords[ b++ ] = xm;
    triCoords[ b++ ] = y;
    drawTriangle( triCoords, c, screenScanTrimmed, vram );


    b = 0;
    triCoords[ b++ ] = x + t;
    triCoords[ b++ ] = y;
    triCoords[ b++ ] = x;
    triCoords[ b++ ] = yp;
    triCoords[ b++ ] = x;
    triCoords[ b++ ] = ym;
    drawTriangle( triCoords, c, screenScanTrimmed, vram );



    b = 0;
    triCoords[ b++ ] = x;
    triCoords[ b++ ] = y + t;
    triCoords[ b++ ] = xm;
    triCoords[ b++ ] = y;
    triCoords[ b++ ] = xp;
    triCoords[ b++ ] = y;
    drawTriangle( triCoords, c, screenScanTrimmed, vram );


    b = 0;
    triCoords[ b++ ] = x - t;
    triCoords[ b++ ] = y;
    triCoords[ b++ ] = x;
    triCoords[ b++ ] = ym;
    triCoords[ b++ ] = x;
    triCoords[ b++ ] = yp;
    drawTriangle( triCoords, c, screenScanTrimmed, vram );

}

void setupEstrella( Estrella * estrella ) {

    estrella->x = rand() & 0x7F;
    estrella->y = rand() & 0x3F;
    
    if ( ( rand() & 0x07 ) < 2 ) {
        // Estrella grande
        estrella->tam = ( 6 + ( rand() & 0x03 ) ) << 1;
    }
    else {
        // Estrella pequeña
        estrella->tam = ( 2 + ( rand() & 0x03 ) ) << 1;
    }

    estrella->crecimiento = 1 + ( rand() & 0x03 );
    estrella->decrecimiento = 4;
    
    if ( estrella->tam < 8 ) {
        estrella->crecimiento = estrella->tam >> 1;
        estrella->decrecimiento = estrella->crecimiento;
    }

}
