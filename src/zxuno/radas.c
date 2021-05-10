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

#include <stdlib.h>
#include <spectrum.h>
#include "radas.h"

// Current drawing screen. Initial value is 1 (the one which is not shown))
uint8_t iCurrentScreen = 1;

uint8_t *currentScreen = RADAS_SCREEN_ADDR_1;
uint8_t *theOtherScreen = RADAS_SCREEN_ADDR_0;

void radasSetRadastanMode( uint8_t attributes, uint8_t border, uint8_t *palette ) {
    
    // The attributes and border are set before clearing the two radastan shadow screens

    zx_colour( attributes );
    zx_border( border );

    radasClsScreen0( 0 );
    radasClsScreen1( 0 );

    radasSetPalette( palette );

    outp( 0xFC3B, 64 );
    outp( 0xFD3B, 3 );

    iCurrentScreen = 1;
    currentScreen = RADAS_SCREEN_ADDR_1;
    theOtherScreen = RADAS_SCREEN_ADDR_0;

}

void radasSetNormalMode( uint8_t attributes, uint8_t border ) {

    radasClsScreen0( 0 );
    
    if ( ! iCurrentScreen ) {
         radasChangeScreens();
    }
    
    outp( 0xFC3B, 64 );
    outp( 0xFD3B, 0 );

    zx_colour( attributes );
    zx_border( border );

}

void radasSetPalette( uint8_t *palette ) {
    
    uint8_t i;
    for ( i = 0; i < 16; i++ ) {
        ulaplus_set( i, palette[ i ] );
    }

}

uint8_t *radasGetCurrentScreen() {

    return currentScreen;

}

uint8_t *radasGetTheOtherScreen() {
    
    return theOtherScreen;
    
}

uint8_t obtenerIndicePantalla() {
    return iCurrentScreen;
}

void radasChangeScreens() {
    
    if ( iCurrentScreen ) {
        outp( 0x00FF, 1 );
        currentScreen = RADAS_SCREEN_ADDR_0;
        theOtherScreen = RADAS_SCREEN_ADDR_1;
        iCurrentScreen = 0;
    }
    else {
        outp( 0x00FF, 0 );
        currentScreen = RADAS_SCREEN_ADDR_1;
        theOtherScreen = RADAS_SCREEN_ADDR_0;
        iCurrentScreen = 1;
    }

}

void radasChangeScreensVariabless() {
    
    if ( iCurrentScreen ) {
        currentScreen = RADAS_SCREEN_ADDR_0;
        theOtherScreen = RADAS_SCREEN_ADDR_1;
        iCurrentScreen = 0;
    }
    else {
        currentScreen = RADAS_SCREEN_ADDR_1;
        theOtherScreen = RADAS_SCREEN_ADDR_0;
        iCurrentScreen = 1;
    }

}

void radasCls( uint8_t *screen, uint8_t value ) {
    int i;
    for ( i = 0; i < RADAS_SCREEN_NUM_BYTES; i++ ) {
        *screen++ = value;
    }
}

void radasClsScreen0( uint8_t value ) {
    uint8_t *p;
    for ( p = 0x4000U; p < 22528; p++ ) {
        *p = value;
    }
}

void radasClsScreen1( uint8_t value ) {
    uint8_t *p;
    for ( p = 0x6000U; p < 30720; p++ ) {
        *p = value;
    }
}

void radasCopyScreen( uint8_t *origin, uint8_t *destination ) {
    uint16_t i;
    for ( i = 0; i < RADAS_SCREEN_NUM_BYTES; i++ ) {
        *destination++ = *origin++;
    }
}

void radasDrawPixel( uint8_t x, uint8_t y, uint8_t color ) {

    // 0 <= x < 128
    // 0 <= y < 96
    // Only low nibble of color is used
    
    uint8_t pixelValues;
    
    uint8_t *ptrPixel = ( uint8_t * ) ( currentScreen + ( ( ( uint16_t ) x ) >> 1 ) + ( ( (  uint16_t ) y ) << RADAS_BITS_SCREEN_WIDTH ) );

    pixelValues = *ptrPixel;

    if ( ! ( x & 1 ) ) {
        color <<= 4;
        pixelValues &= 0xF0;
    }
    else {
        pixelValues &= 0x0F;
    }

    pixelValues |= color;

    *ptrPixel = pixelValues;

}

void radasDraw2Pixels( uint8_t x2, uint8_t y, uint8_t color ) {

    // x2 is in pairs of pixels, i.e. x/2
    // 0 <= x2 < 64
    // 0 <= y < 96
    // color has the two nibbles of both pixels
    

    *( ( uint8_t * ) ( currentScreen + ( (  uint16_t ) x2 ) + ( ( (  uint16_t ) y ) << 6 ) ) ) = color;

}

void radasDrawRectangle( uint8_t x2, uint8_t y, uint8_t width2, uint8_t height, uint8_t value ) {

    // x2 is in pairs of pixels, i.e. x/2, the same as width2
    // 0 <= x2 < 64
    // 0 <= y < 96
    // 1 <= width2 < 64
    // 1 <= height < 96
    // color has the two nibbles of both pixels
    
    uint8_t i, j;
    uint8_t *p = currentScreen + ( (  uint16_t ) x2 ) + ( ( (  uint16_t ) y ) << RADAS_BITS_SCREEN_WIDTH );
    uint16_t dx = RADAS_SIZE_X2 - width2;
    for ( j = 0; j < height; j++ ) {
        for ( i = 0; i < width2; i++ ) {
            *p++ = value;
        }
        p += dx;
    }
}

void radasEraseRectangle( uint8_t x2, uint8_t y, uint8_t width2, uint8_t height ) {

    // x2 is in pairs of pixels, i.e. x/2, the same as width2
    // 0 <= x2 < 64
    // 0 <= y < 96
    // 1 <= width2 < 64
    // 1 <= height < 96
    
    uint8_t i, j;
    uint8_t *p = currentScreen + ( (  uint16_t ) x2 ) + ( ( (  uint16_t ) y ) << RADAS_BITS_SCREEN_WIDTH );
    uint16_t dx = RADAS_SIZE_X2 - width2;
    for ( j = 0; j < height; j++ ) {
        for ( i = 0; i < width2; i++ ) {
            *p++ = 0;
        }
        p += dx;
    }
}

void radasDrawImage( uint8_t x2, uint8_t y, RadasImage *image ) {

    // x2 is in pairs of pixels, i.e. x/2, the same as width2
    // The image is clipped against screen limits

    uint8_t i, j;
    uint8_t *p = currentScreen + ( (  uint16_t ) x2 ) + ( ( (  uint16_t ) y ) << RADAS_BITS_SCREEN_WIDTH );
    uint16_t dx = RADAS_SIZE_X2 - image->sizeX2;
    uint8_t sizeY = image->sizeY;
    uint8_t sizeX2 = image->sizeX2;
    uint8_t *pImg = image->pixels;
    for ( j = 0; j < sizeY; j++ ) {
        for ( i = 0; i < sizeX2; i++ ) {
            *p++ = *pImg++;
        }
        p += dx;
    }

}
