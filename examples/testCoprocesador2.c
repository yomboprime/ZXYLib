/*
 * @author yomboprime
 *
 * 
 * Test coprocesador
*/
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <input.h>
#include <math.h>

#include "../src/zxthree/screen/screen.h"
#include "../src/zxthree/impl/zxradas/screen/scrradas.h"
#include "../src/zxthree/raster/triangle/triangle.h"

#include "../src/zxuno/coprocessor.h"
#include "../src/zxuno/turbo.h"

#define getKey() in_Inkey()

// Global variables

struct in_UDK keyboardMapping;

uint8_t palette[ 16 ];    
int16_t seed;
uint8_t *vram;
int16_t i, j;

int16_t triCoords[ 6 ];

uint8_t turbo;

uint8_t key;

int16_t result1 = 0;
int16_t result2 = 0;
int16_t a, b, c;

uint32_t t, t2;

float tf;

// Function prototypes
uint8_t readKeyboardAndJoy();

void main(void) {

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

    
    triCoords[ 0 ] = 100;
    triCoords[ 1 ] = 20;
    triCoords[ 2 ] = 110;
    triCoords[ 3 ] = 60;
    triCoords[ 4 ] = 15;
    triCoords[ 5 ] = 35;
    
    
    // Get pointer to current drawing screen
    vram = getVRAMPointer();

    // Clear background
    memset( vram, 0x00, screenNumBytes );
    


    drawTriangle( triCoords, 0x55, screenScanTrimmed, vram );



    // Blit the screen
    blitVRAM();
    
/*
    #define NUM_OPS 2000
    t = getFrames();
    for ( i = 0; i < NUM_OPS; i ++ ) {

        result2 = coprocessorMultAdd88Optimized( c, b, a );

    }
    t2 = getFrames();
    
    tf = t2 - t;
    tf = ((float)NUM_OPS) * 50.0 / tf;
*/

    TURBO_set( turbo );

    while ( getKey() == 0 );

    setVideoMode( VIDEO_MODE_ZX_DEFAULT );

}

uint8_t readKeyboardAndJoy() {
    return ((void*)in_JoyKeyboard)( &keyboardMapping ) | ((void*)in_JoySinclair1)( &keyboardMapping );
}

