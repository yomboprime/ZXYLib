/*
 * @author yomboprime
 *
 * ZX Spectrum text functions
*/

#ifndef TEXTUTILS_H
#define TEXTUTILS_H

#include <stdio.h>
#include "integerTypes.h"

#define COLOR_ATTRIBUTE_START_ADDRESS 22528

extern void textUtils_cls();

extern void textUtils_32ColumnsMode();

extern void textUtils_64ColumnsMode();

extern void textUtils_print( unsigned char *text );
extern void textUtils_print_l( long n );

#define textUtils_println( x ) textUtils_print( x ); fputc_cons( '\n' )
#define textUtils_println_l( x ) textUtils_print_l( x ); fputc_cons( '\n' )

extern void textUtils_printAt32( int x, int y, unsigned char *text );

extern void textUtils_printAt64( int x, int y, unsigned char *text );

extern void textUtils_paintSegmentWithAttributes( unsigned char y, unsigned char x0, unsigned char x1, unsigned char attributes );

extern bool isDigit( uint8_t c );

#endif /* TEXTUTILS_H */
