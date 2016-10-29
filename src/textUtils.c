/*
 * @author yomboprime
 *
 * ZX Spectrum text functions
*/

#include "textUtils.h"
#include <stdio.h>

void textUtils_cls() {

    fputc_cons( 12 );

}

void textUtils_32ColumnsMode() {

    fputc_cons( 1 );
    fputc_cons( 32 );

}

void textUtils_64ColumnsMode() {

    fputc_cons( 1 );
    fputc_cons( 64 );

}

void textUtils_print( unsigned char *text ) {

    fputs( text, stdout );

}

void textUtils_print_l( long n ) {

    unsigned char s[ 12 ];

    sprintf( s, "%ld", n );

    fputs( s, stdout );

}

void textUtils_printAt32( int x, int y, unsigned char *text ) {

    fputc_cons( 22 );
    fputc_cons( (char)( y + 0x20 ) );
    fputc_cons( (char)( x + 0x20 ) );
    fputs( text, stdout );

}

void textUtils_printAt64( int x, int y, unsigned char *text ) {

    fputc_cons( 22 );
    fputc_cons( (char)y );
    fputc_cons( (char)x );
    fputs( text, stdout );

}

void textUtils_paintSegmentWithAttributes( unsigned char y, unsigned char x0, unsigned char x1, unsigned char attributes ) {

    unsigned char x;
    unsigned char *ptrAttr = (unsigned char *)COLOR_ATTRIBUTE_START_ADDRESS;
    for ( x = 0; x < y; x++ ) {
        ptrAttr += 32;
    }
    ptrAttr += x0;

    for ( x = x0; x <= x1; x++ ) {
        *ptrAttr++ = attributes;
    }

}

bool isDigit( uint8_t c ) {
    return c >= '0' && c <= '9';
}
