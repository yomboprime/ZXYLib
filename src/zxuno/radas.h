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


#ifndef RADASTAN_H
#define RADASTAN_H

#include "../integerTypes.h"

#define RADAS_SCREEN_ADDR_0 ( ( uint8_t * ) 0x4000U )
#define RADAS_SCREEN_ADDR_1 ( ( uint8_t * ) 0x6000U )
#define RADAS_SIZE_X 128
#define RADAS_SIZE_Y 96
#define RADAS_BITS_SCREEN_WIDTH 6
#define RADAS_SIZE_X2 ( RADAS_SIZE_X >> 1 )
#define RADAS_SCREEN_NUM_BYTES 6144

typedef struct {
    
    uint8_t sizeX2;
    uint8_t sizeY;
    uint8_t *pixels;
    
} RadasImage;

extern void radasSetRadastanMode( uint8_t attributes, uint8_t border, uint8_t *palette );
extern void radasSetNormalMode( uint8_t attributes, uint8_t border );

extern void radasSetPalette( uint8_t *palette );

extern uint8_t *radasGetCurrentScreen();
extern uint8_t *radasGetTheOtherScreen();

extern void radasChangeScreens();

extern void radasChangeScreensVariables();

extern void radasCls( uint8_t *screen, uint8_t value );
extern void radasClsScreen0( uint8_t value );
extern void radasClsScreen1( uint8_t value );

extern void radasCopyScreen( uint8_t *origin, uint8_t *destination );


extern void radasDrawPixel( uint8_t x, uint8_t y, uint8_t color );
extern void radasDraw2Pixels( uint8_t x2, uint8_t y, uint8_t color );

extern void radasDrawRectangle( uint8_t x2, uint8_t y, uint8_t width2, uint8_t height, uint8_t value );
extern void radasEraseRectangle( uint8_t x2, uint8_t y, uint8_t width2, uint8_t height );

extern void radasDrawImage( uint8_t x2, uint8_t y, RadasImage *image );

#endif //RADASTAN_H

