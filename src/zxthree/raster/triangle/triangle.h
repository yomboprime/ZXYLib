
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
