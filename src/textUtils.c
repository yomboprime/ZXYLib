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

#include "textUtils.h"
#include <stdio.h>
#include <input.h>
#include <spectrum.h>

/*
 * Clears the screen
 */
void textUtils_cls() {

    fputc_cons( 12 );

}

/*
 * Sets 32 column mode
 */
void textUtils_32ColumnsMode() {

    fputc_cons( 1 );
    fputc_cons( 32 );

}

/*
 * Sets 64 column mode
 */
void textUtils_64ColumnsMode() {

    fputc_cons( 1 );
    fputc_cons( 64 );

}

/*
 * Prints asciiz string
 */
void textUtils_print( uint8_t *text ) {

    fputs( (const char *)text, stdout );

}

/*
 * Prints integer in decimal notation
 */
void textUtils_print_l( long n ) {

    uint8_t s[ 12 ];

    sprintf( s, "%ld", n );

    fputs( s, stdout );

}

/*
 * Locates the cursor in 32 column mode
 *
 * In 32 columns mode:
 * 0 <= x <= 31
 * 0 <= y <= 23
 *
 * In 64 columns mode:
 * 0 <= x <= 63
 * 0 <= y <= 23
 *
 */
void textUtils_printAt( int x, int y ) {

    fputc_cons( 22 );
    fputc_cons( (char)( y + 0x20 ) );
    fputc_cons( (char)( x + 0x20 ) );

}

/*
 * Sets current ink, paper, bright and flash for painting.
 */
void textUtils_setAttributes( uint8_t attributes ) {

    uint8_t paper = ( attributes & 0x38 ) >> 3;
    uint8_t ink = attributes & 0x07;
    uint8_t flash = (attributes & 0x80) != 0 ? 1 : 0;
    uint8_t bright = (attributes & 0x40) != 0 ? 1 : 0;

    fputc_cons( 16 ); fputc_cons( ink + 48 );
    fputc_cons( 17 ); fputc_cons( paper + 48 );
/*    fputc_cons( 18 ); fputc_cons( flash + 48 );
    fputc_cons( 19 ); fputc_cons( bright + 48 );
*/  
}

uint8_t textUtils_createAttributes( bool flash, bool bright, uint8_t paper, uint8_t ink ) {

	uint8_t attr = 0;
	attr = ink & 7;
	attr |= ( paper & 7 ) << 3;
	attr |= bright ? 0x40 : 0x00;
	attr |= flash ? 0x80 : 0x00;

	return attr;

}

/*
 * Sets attributes of character x,y at screen
 */
void textUtils_paintCharWithAttributes( uint8_t x, uint8_t y, uint8_t attributes ) {

    uint8_t *ptrAttr = ( (uint8_t *)COLOR_ATTRIBUTE_START_ADDRESS );
    ptrAttr += 32 * y + x;

    *ptrAttr = attributes;

}

/*
 * Sets attributes of character segment x0->x1, y at screen
 */
void textUtils_paintSegmentWithAttributes( uint8_t x0, uint8_t x1, uint8_t y, uint8_t attributes ) {

    uint8_t x;
    uint8_t *ptrAttr = ( (uint8_t *)COLOR_ATTRIBUTE_START_ADDRESS );
    ptrAttr += 32 * y + x0;

    for ( x = x0; x <= x1; x++ ) {
        *ptrAttr++ = attributes;
    }

}

/*
 * Sets attributes of rectangle x0,y0 - x1,y1 at screen
 */
void textUtils_paintRectangleWithAttributes( uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1, uint8_t attributes ) {

    uint8_t y;

    for ( y = y0; y <= y1; y++ ) {

        textUtils_paintSegmentWithAttributes( x0, x1, y, attributes );

    }

}

/*
 * Sets or clears bright of character segment x0->x1, y at screen
 */
void textUtils_paintSegmentWithBright( uint8_t x0, uint8_t x1, uint8_t y, bool bright ) {

    uint8_t x;
    uint8_t attr;
    uint8_t *ptrAttr = ( (uint8_t *)COLOR_ATTRIBUTE_START_ADDRESS );
    ptrAttr += 32 * y + x0;

    for ( x = x0; x <= x1; x++ ) {
        attr= *ptrAttr;
        if ( bright == true ) {
            attr |= BRIGHT;
        }
        else {
            attr &= ~BRIGHT;
        }

        *ptrAttr++ = attr;
    }

}

/*
 * Define a UDG 8x8 pixels graphic in memory. You can use it by "print( UDG_GRAPHICS_START + graphicIndex )"
 * graphic: pointer to 8 bytes defining the graphic
 * graphicIndex: 0 to 15
 */
void textUtils_defineUDGGraphic( uint8_t *graphic, uint16_t graphicIndex ) {

    uint8_t i;

    uint8_t *p = 65368;

    p += ( graphicIndex << 3 );

    for ( i = 0; i < 8; i++ ) {
        *p++ = *graphic++;
    }

}

bool textUtils_isDigit( uint8_t c ) {
    return c >= '0' && c <= '9';
}

bool textUtils_isReadable( uint8_t c ) {

	return c >= ' ' && c <= '~';

}

/*
 * Waits for a key press with repetition
 */
uint16_t waitKeyPress(  uint16_t count ) {

    uint16_t k = in_Inkey();
    while ( k > 0 && count > 0 ) {
        k = in_Inkey();
        count--;
    }

    while ( k == 0 ) {
        k = in_Inkey();
    };

    return k;
}

/*
 * Waits for a key press
 */
uint16_t waitKey() {

    uint16_t k = 0;

    while ( in_Inkey() > 0 );

    while ( k == 0 ) {
        k = in_Inkey();
    };

    return k;
}
