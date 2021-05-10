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
// ZX Spectrum text functions

#ifndef TEXTUTILS_H
#define TEXTUTILS_H

#include <stdio.h>
#include "integerTypes.h"

#define COLOR_ATTRIBUTE_START_ADDRESS 22528

#define UDG_GRAPHICS_START 128

extern void textUtils_cls();

extern void textUtils_32ColumnsMode();

extern void textUtils_64ColumnsMode();

extern void textUtils_print( uint8_t *text );
extern void textUtils_print_l( long n );

#define textUtils_printEOL() fputc_cons( '\n' )
#define textUtils_println( x ) textUtils_print( x ); fputc_cons( '\n' )
#define textUtils_println_l( x ) textUtils_print_l( x ); fputc_cons( '\n' )

extern void textUtils_printAt( int x, int y );

extern void textUtils_setAttributes( uint8_t attributes );
extern uint8_t textUtils_createAttributes( bool flash, bool bright, uint8_t paper, uint8_t ink );

extern void textUtils_paintCharWithAttributes( uint8_t x, uint8_t y, uint8_t attributes );
extern void textUtils_paintSegmentWithAttributes( uint8_t x0, uint8_t x1, uint8_t y, uint8_t attributes );
extern void textUtils_paintRectangleWithAttributes( uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1, uint8_t attributes );

extern void textUtils_paintSegmentWithBright( uint8_t x0, uint8_t x1, uint8_t y, bool bright );

extern void textUtils_defineUDGGraphic( uint8_t *graphic, uint16_t graphicIndex );

extern bool textUtils_isDigit( uint8_t c );
extern bool textUtils_isReadable( uint8_t c );

extern uint16_t waitKey();
extern uint16_t waitKeyPress( uint16_t count );

#endif /* TEXTUTILS_H */
