 
#include "triangle.h"

void paintPixel( int16_t x, int16_t y, uint8_t color, uint8_t *buffer ) {

    if ( x < 0 || x >= screenSizeX || y < 0 || y >= screenSizeY ) {
        return;
    }
    
    buffer[ screenSizeX * y + x ] = color;
    
}

bool isCWTriangle( int16_t *coordinates ) {
    
    int32_t v1x = coordinates[ 2 ] - coordinates[ 0 ];
    int32_t v1y = coordinates[ 3 ] - coordinates[ 1 ];
    
    int32_t v2x = coordinates[ 4 ] - coordinates[ 0 ];
    int32_t v2y = coordinates[ 5 ] - coordinates[ 1 ];

    int32_t crossProduct = v1x * v2y - v1y * v2x;
    
    return crossProduct > 0;

}

void drawTriangle( int16_t *coordinates, uint8_t color, ScreenScanFunc( screenScan ), uint8_t *screenBuf ) {

    uint8_t index0 = 0;
    uint8_t index1 = 1;
    uint8_t index2 = 2;
    
    uint8_t straightSideIsLeft = false;
    
    int16_t x0;
    int16_t y0;
    int16_t x1;
    int16_t y1;
    int16_t x2;
    int16_t y2;
    
    int16_t y;
    int16_t xLeft;
    int16_t xRight;
    int16_t dxLeft;
    int16_t dyLeft;
    int16_t dxRight;
    int16_t dyRight;
    int16_t errorLeft;
    int16_t errorRight;
    int16_t dirLeft;
    int16_t dirRight;
    int16_t pixelsLeft;
    int16_t pixelsRight;
    
    bool linePointerValid = false;
    uint8_t *linePointer;
    
    bool firstHalf = true;
    bool secondHalf = true;
    
    // Order the three vertices in ascending Y
    
    if ( coordinates[ 1 ] < coordinates[ 3 ] ) {
        if ( coordinates[ 3 ] >= coordinates[ 5 ] ) {
            if ( coordinates[ 1 ] < coordinates[ 5 ] ) {
                index1 = 2;
                index2 = 1;
            }
            else {
                index0 = 2;
                index1 = 0;
                index2 = 1;
                straightSideIsLeft = true;
            }
        }
        else {
            straightSideIsLeft = true;
        }
    }
    else {
        if ( coordinates[ 1 ] < coordinates[ 5 ] ) {
            index0 = 1;
            index1 = 0;
            index2 = 2;
        }
        else {
            if ( coordinates[ 3 ] < coordinates[ 5 ] ) {
                index0 = 1;
                index1 = 2;
                index2 = 0;
                straightSideIsLeft = true;
            }
            else {
                index0 = 2;
                index1 = 1;
                index2 = 0;
            }
        }
    }
    
    // Get ordered coordinates

    x0 = coordinates[ ( index0 << 1 ) ];
    y0 = coordinates[ ( index0 << 1 ) + 1 ];
    
    x1 = coordinates[ ( index1 << 1 ) ];
    y1 = coordinates[ ( index1 << 1 ) + 1 ];
    
    x2 = coordinates[ ( index2 << 1 ) ];
    y2 = coordinates[ ( index2 << 1 ) + 1 ];

    xLeft = x0;
    xRight = x0;
    y = y0;
    
    if ( y >= 0 ) {
        linePointerValid = true;
        linePointer = screenBuf + screenLineNumBytes * y;
    }

    // Detect horizontal edge cases,
    // order vertices in X, and
    // compute coordinate deltas
    if ( y0 == y1 ) {
        
        firstHalf = false;
        
        if ( x0 > x1 ) {

            xLeft = x1;
            xRight = x0;
            dxLeft = x2 - x1;
            dxRight = x2 - x0;
            dyLeft = y2 - y1;
            dyRight = y2 - y0;

        }
        else {

            xLeft = x0;
            xRight = x1;
            dxLeft = x2 - x0;
            dxRight = x2 - x1;
            dyLeft = y2 - y0;
            dyRight = y2 - y1;

        }

    }
    else {
        
        if ( y1 == y2 ) {
        
            secondHalf = false;
            
        }

        if ( straightSideIsLeft ) {

            dxLeft = x2 - x0;
            dxRight = x1 - x0;
            dyLeft = y2 - y0;
            dyRight = y1 - y0;
        }
        else {

            dxLeft = x1 - x0;
            dxRight = x2 - x0;
            dyLeft = y1 - y0;
            dyRight = y2 - y0;

        }

    }

    // Direction = false -> left; = true -> right
    if ( dxLeft < 0 ) {
        dirLeft = -1;
        dxLeft = -dxLeft;
    }
    else {
        dirLeft = 1;
    }
    if ( dxRight < 0 ) {
        dirRight = -1;
        dxRight = -dxRight;
    }
    else {
        dirRight = 1;
    }
    
    pixelsLeft = dxLeft;
    pixelsRight = dxRight;
    errorLeft = dxLeft >> 1;
    errorRight = dxRight >> 1;

    // Draw the first half of the triangle
    if ( firstHalf ) {
        
        while ( y <= y1 ) {

            errorLeft += dxLeft;
            while ( errorLeft >= dyLeft && pixelsLeft > 0 ) {
                errorLeft -= dyLeft;
                xLeft += dirLeft;
                pixelsLeft--;
            }

            errorRight += dxRight;
            while ( errorRight >= dyRight && pixelsRight > 0 ) {
                errorRight -= dyRight;
                xRight += dirRight;
                pixelsRight--;
            }

            if ( ! linePointerValid ) {
                // Compute linePointer when y > 0 to prevent pointer overflow
                if ( y >= 0 ) {
                    linePointer = screenBuf + screenLineNumBytes * y;
                    linePointerValid = true;
                }
            }

            if ( linePointerValid ) {
                screenScan( linePointer, xLeft, xRight, color );
            }

            y++;
            linePointer += screenLineNumBytes;
            
        }
        
    }
    
    // Draw the second half of the triangle
    if ( secondHalf && y < screenSizeY ) {
        
        if ( straightSideIsLeft ) {
            
            dxRight = x2 - x1;
            dyRight = y2 - y1;

            if ( dxRight < 0 ) {
                dirRight = -1;
                dxRight = -dxRight;
            }
            else {
                dirRight = 1;
            }

            pixelsRight = dxRight;
            errorRight = dxRight >> 1;

        }
        else {
            
            dxLeft = x2 - x1;
            dyLeft = y2 - y1;

            if ( dxLeft < 0 ) {
                dirLeft = -1;
                dxLeft = -dxLeft;
            }
            else {
                dirLeft = 1;
            }
            
            pixelsLeft = dxLeft;
            errorLeft = dxLeft >> 1;

        }
        
        if ( y2 >= screenSizeY ) {
            y2 = screenSizeY - 1;
        }
        
        while ( y <= y2 ) {
            
            errorLeft += dxLeft;
            while ( errorLeft >= dyLeft && pixelsLeft > 0 ) {
                errorLeft -= dyLeft;
                xLeft += dirLeft;
                pixelsLeft--;
            }

            errorRight += dxRight;
            while ( errorRight >= dyRight && pixelsRight > 0 ) {
                errorRight -= dyRight;
                xRight += dirRight;
                pixelsRight--;
            }
            
            if ( ! linePointerValid ) {
                // Compute linePointer when y > 0 to prevent pointer overflow
                if ( y >= 0 ) {
                    linePointer = screenBuf + screenLineNumBytes * y;
                    linePointerValid = true;
                }
            }

            if ( linePointerValid ) {
                screenScan( linePointer, xLeft, xRight, color );
            }

            y++;
            linePointer += screenLineNumBytes;

        }

    }

    //paintPixel( x0, y0, 2, screenBuf );
    //paintPixel( x1, y1, 3, screenBuf );
    //paintPixel( x2, y2, 6, screenBuf );

}
