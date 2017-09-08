/*
 * @author yomboprime
 *
 * Draw triangles example
*/
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <input.h>
#include <time.h>

#include "../src/zxthree/screen/screen.h"
//#include "../src/zxthree/impl/pcdos/screen/scrdos.h"
#include "../src/zxthree/impl/zxradas/screen/scrradas.h"
#include "../src/zxthree/raster/triangle/triangle.h"
#include "../src/zxuno/turbo.h"

main() {
    
    int seed;
    
    uint8_t palette1[ 16 ];    
    int b = 0;
    int c;
    

    //clock_t t0, t1;    
    long t0, t1;
    
    int16_t coords[ 6 ];
    
    uint8_t color = 4;

    uint8_t *vram;
    
    int16_t temp;
    
    int numCopies;
    
    int i;
    
    float s;
    
    char tempString[ 12 ];
    
    uint8_t turbo;

    turbo = TURBO_get();
    TURBO_set( ZXUNO_TURBO_X8 );

    setVideoMode( VIDEO_MODE_ZX_DEFAULT );
    
    printf( "\nPress a key to start..." );
    getch();

    seed = clock();
    
    srand( seed );

    setVideoMode( VIDEO_MODE_RADASTAN );

    //setBackBufPointer( backBuffer );

    for ( c = 0; c < 16; c++ ) {
            palette1[ b++ ] = c << 4;
            //palette1[ b++ ] = c;
            //palette1[ b++ ] = 0;
    }
    setPalette( palette1 );
    
    numCopies = 1000;

    coords[ 0 ] = 145;
    coords[ 1 ] = 43;
    coords[ 2 ] = 215;
    coords[ 3 ] = 176;
    coords[ 4 ] = 67;
    coords[ 5 ] = 112;

    //vram = getVRAMPointer();
    vram = getVRAMBackBufPointer();
    
    //t0 = clock();
    t0 = time( NULL );

    for ( i = 0; i < numCopies; i++ ) {

        //clearBuffer( backBuffer );
//        memset( backBuffer, 0, screenNumBytes );

        /*
        // Gradient over all the screen:
        uint16_t offset = 0 * 320 + 0;
        for ( int j = 0; j < 200; j++ ) {
        for ( int k = 0; k < 320; k++ ) {
                backBuffer[ offset++ ] = (uint8_t)k;
            }
            //offset += 320 - 80;
        }
        */
        
        /*
        // Rectangle:
        int l = 50;
        uint8_t color = 0;
        uint8_t *p = backBuffer + l * screenSizeX;
        for ( ; l < 150; l++ ) {
            
            screenScan( p, 100, 210, color++ );
            
            p+= screenSizeX;
            
        }
        */
        

        // Random triangle coords and color
/*
        coords[ 0 ] = rand() / 100;
        coords[ 1 ] = rand() / 163;
        coords[ 2 ] = rand() / 100;
        coords[ 3 ] = rand() / 163;
        coords[ 4 ] = rand() / 100;
        coords[ 5 ] = rand() / 163;
*/
        
        coords[ 0 ] = rand() & 127;
        coords[ 1 ] = rand() & 127;
        coords[ 2 ] = rand() & 127;
        coords[ 3 ] = rand() & 127;
        coords[ 4 ] = rand() & 127;
        coords[ 5 ] = rand() & 127;

        color = (uint8_t)rand();
        
        color = ( color & 0x0F ) | ( ( color & 0x0F ) << 4 );

        // Check if CW, if not then invert

        if ( ! isCWTriangle( coords ) ) {

            temp = coords[ 0 ];
            coords[ 0 ] = coords[ 2 ];
            coords[ 2 ] = temp;
            
            temp = coords[ 1 ];
            coords[ 1 ] = coords[ 3 ];
            coords[ 3 ] = temp;
            
        }
        
        // Draw triangle
        //drawTriangle( coords, color, screenScanTrimmed, backBuffer );
        drawTriangle( coords, color, screenScanTrimmed, vram );

//        blitVRAM();
        
        /*
        if ( getch() == ' ' ) {
            setVideoMode( VIDEO_MODE_ZX_DEFAULT );
            getch();
            TURBO_set( ZXUNO_TURBO_X1 );
            exit( 0 );
        }*/

        
        //getch();
        
        
//        seed++;
//        srand( seed );

    }

    t1 = clock();

    setVideoMode( VIDEO_MODE_ZX_DEFAULT );

    temp = t1 - t0;
    
    printf( "End.\n" );

    /*
    printf( "CLOCKS_PER_SEC = %d\n", CLOCKS_PER_SEC );
    
    printf( "t0: %ld\n", t0 );
    printf( "t1: %ld\n", t1 );

    printf( "Total clocks: %d\n", temp );
    printf( "Total tris: %d\n", numCopies );
    */


//    s = ( (float)temp ) / ( (float)CLOCKS_PER_SEC );
//    printf( "Time to make %d triangles (in seconds): %f\n", numCopies, s );
//    printf( "Triangles per second: %f\n", numCopies / s );
  
    getch();

    TURBO_set( turbo );

    while ( 1 ) {}
}
