/*
 * @author yomboprime
 *
 * Mouse example
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


int seed;
    
uint8_t palette1[ 16 ];    

int16_t b = 0;
int16_t c;
int16_t i, j, p;

int16_t xa, ya;
int16_t x1, y1;
int16_t xd, yd;
uint16_t buttons;
int16_t xPointer, yPointer;

int16_t coords[ 6 ];
int16_t coordsTri[ 6 ];
int16_t coordsPointer[ 6 * 3 ];

uint8_t color = 7;

uint8_t *vram;
uint8_t *backBuffer;

uint8_t tempBuffer[ 64 * 96 ];

int16_t temp;

char tempString[ 12 ];

uint8_t turbo;

main() {

    turbo = TURBO_get();
    TURBO_set( ZXUNO_TURBO_X8 );

    setVideoMode( VIDEO_MODE_ZX_DEFAULT );
    
//    printf( "\nPress a key to start..." );
//    getch();

/*
	while ( 1 ) {	
		x1 = inp( (uint16_t)64479 );
		y1 = inp( (uint16_t)65503 );
		buttons = inp( (uint16_t)64223 );
		
		printf( "x: %d, y: %d, b: %d\n", x1, y1, buttons );
		
		if ( ( buttons & 1 ) == 0 ) {
			break;
		}
		
	}
*/

    seed = clock();
    srand( seed );

    setVideoMode( VIDEO_MODE_RADASTAN );

    for ( c = 0; c < 16; c++ ) {
            palette1[ b++ ] = c << 4;
            //palette1[ b++ ] = c;
            //palette1[ b++ ] = 0;
    }
    setPalette( palette1 );

	
	memset( tempBuffer, 0, screenNumBytes );
	
    coords[ 0 ] = 0;
    coords[ 1 ] = 0;
    coords[ 2 ] = 0;
    coords[ 3 ] = 0;
    coords[ 4 ] = 0;
    coords[ 5 ] = 0;

    coordsTri[ 0 ] = 0;
    coordsTri[ 1 ] = -3;
    coordsTri[ 2 ] = 4;
    coordsTri[ 3 ] = 3;
    coordsTri[ 4 ] = -4;
    coordsTri[ 5 ] = 3;

	coordsPointer[ 0 ] = 0;
    coordsPointer[ 1 ] = 0;
    coordsPointer[ 2 ] = 5;
    coordsPointer[ 3 ] = 0;
    coordsPointer[ 4 ] = 0;
    coordsPointer[ 5 ] = 5;
	
	coordsPointer[ 0 + 6 ] = 2;
    coordsPointer[ 1 + 6 ] = 0;
    coordsPointer[ 2 + 6 ] = 7;
    coordsPointer[ 3 + 6 ] = 6;
    coordsPointer[ 4 + 6 ] = 6;
    coordsPointer[ 5 + 6 ] = 7;
	
	coordsPointer[ 0 + 12 ] = 0;
    coordsPointer[ 1 + 12 ] = 2;
    coordsPointer[ 2 + 12 ] = 2;
    coordsPointer[ 3 + 12 ] = 0;
    coordsPointer[ 4 + 12 ] = 6;
    coordsPointer[ 5 + 12 ] = 7;
	
	xPointer = 0;
	yPointer = 0;
	xa = inp( (uint16_t)64479 );
	ya = inp( (uint16_t)65503 );

    //vram = getVRAMPointer();
	// vram = getVRAMBackBufPointer();

    while ( 1 ) {
		
		vram = getVRAMPointer();
		backBuffer = getVRAMBackBufPointer();

        //clearBuffer( vram );
        //memset( vram, 0, screenNumBytes );

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

/*
        coords[ 0 ] = rand() & 127;
        coords[ 1 ] = rand() & 127;
        coords[ 2 ] = rand() & 127;
        coords[ 3 ] = rand() & 127;
        coords[ 4 ] = rand() & 127;
        coords[ 5 ] = rand() & 127;
*/

		x1 = inp( (uint16_t)64479 );
		y1 = inp( (uint16_t)65503 );
		buttons = inp( (uint16_t)64223 );

		xd = x1 - xa;
		yd = y1 - ya;

		if ( xd < 127 && xd > -127 ) {
			xPointer += xd;
		}
		if ( yd < 127 && yd > -127 ) {
			yPointer -= yd;
		}

		if ( xPointer < 0 ) {
			xPointer = 0;
		}
		else if ( xPointer > 127 ) {
			xPointer = 127;
		}

		if ( yPointer < 0 ) {
			yPointer = 0;
		}
		else if ( yPointer > 95 ) {
			yPointer = 95;
		}

		xa = x1;
		ya = y1;

        //color = (uint8_t)rand();
        
        color = ( color & 0x0F ) | ( ( color & 0x0F ) << 4 );

        // Check if CW, if not then invert
/*
        if ( ! isCWTriangle( coords ) ) {

            temp = coords[ 0 ];
            coords[ 0 ] = coords[ 2 ];
            coords[ 2 ] = temp;
            
            temp = coords[ 1 ];
            coords[ 1 ] = coords[ 3 ];
            coords[ 3 ] = temp;
            
        }
*/
        // Draw triangle

        if ( ( buttons & 1 ) == 0 || ( buttons & 2 ) == 0 ) {
			p = 0;
			for ( j = 0; j < 6; j+= 2 ) {
				coords[ j ] = coordsTri[ p++ ] + xPointer;
				coords[ j + 1 ] = coordsTri[ p++ ] + yPointer;
			}

			drawTriangle( coords, ( buttons & 2 ) == 0 ? 0x44 : 0, screenScanTrimmed, tempBuffer );
			
		}

		memcpy( vram, tempBuffer, screenNumBytes );

		// Draw pointer

		p = 0;
		for ( i = 0; i < 3; i++ ) {

			for ( j = 0; j < 6; j+= 2 ) {
				coords[ j ] = coordsPointer[ p++ ] + xPointer;
				coords[ j + 1 ] = coordsPointer[ p++ ] + yPointer;
			}

			drawTriangle( coords, color, screenScanTrimmed, vram );

		}

//		printf(" xP: %d, yP: %d\n", xPointer, yPointer );

        blitVRAM();
        
//        seed++;
//        srand( seed );

    }

    setVideoMode( VIDEO_MODE_ZX_DEFAULT );
    
    printf( "End.\n" );
    getch();

    TURBO_set( turbo );

    //while ( 1 ) {}
	
	exit( 0 );

}
