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

#ifndef __TRIANGLE__
#define __TRIANGLE__

#include "../../bits.h"

#include "../../screen/screen.h"

#ifdef __cplusplus
    extern "C" {
#endif

/*
 * Draws a filled triangle in the screen.
 * 
 * Given the triangle ABC,
 * coordinates[ 0 ] = A.x
 * coordinates[ 1 ] = A.y
 * coordinates[ 2 ] = B.x
 * coordinates[ 3 ] = B.y
 * coordinates[ 4 ] = C.x
 * coordinates[ 5 ] = C.y
 * 
 * The vertices must be in clockwise rotation order (CW)
 * 
 * color: color to paint the triangle with. Its meaning is machine and current graphics mode dependent.
 * 
 * screenScan: Function to paint scanlines of pixels
 * 
 * screenBuf: VRAM or buffer pointer to draw into.
 * 
 */
extern void drawTriangle( int16_t *coordinates, uint8_t color, ScreenScanFunc( screenScan ), uint8_t *screenBuf );

/*
 * Tells if given vertices are in clockwise rotation order (CW)
 * 
 * coordinates: As specified in drawTriangle()
 *
 */
extern bool isCWTriangle( int16_t *coordinates );

void paintPixel( int16_t x, int16_t y, uint8_t color, uint8_t *buffer );

#ifdef __cplusplus
    }
#endif

#endif //__TRIANGLE__ 
