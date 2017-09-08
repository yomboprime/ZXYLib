/*
 * @author yomboprime
 *
 * Speeder Bike Game
 * 
 * 
*/
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <input.h>
#include <sound.h>

#include "../src/zxthree/screen/screen.h"
#include "../src/zxthree/impl/zxradas/screen/scrradas.h"
#include "../src/zxthree/raster/triangle/triangle.h"
#include "../src/zxuno/turbo.h"

#define getKey() in_Inkey()

// Constants

#define BLACK_COLOR 0
#define GROUND_COLOR 1
#define SPEEDER_COLOR1 2
#define SPEEDER_COLOR2 3
#define BROWN_COLOR 4
#define SKY_COLOR 6
#define BORDER_COLOR 7
#define SKY_COLOR_DEG0 9
#define SKY_COLOR_DEG1 10
#define SKY_COLOR_DEG2 11

#define MIN_GREEN_COLOR 12
#define NUM_GREEN_COLORS 4

#define SKY_COLOR_DEG_SIZE 3


#define NUM_DECIMAL_PLACES 6

#define TO_FIXED( x ) ( ( x ) << NUM_DECIMAL_PLACES )
#define FROM_FIXED( x ) ( ( x ) >> NUM_DECIMAL_PLACES )

#define MAX_Z TO_FIXED( 63 )
#define MIN_Z TO_FIXED( -63 )
#define MAX_X TO_FIXED( 255 )
#define MIN_X TO_FIXED( -255 )

#define NUM_TREES 16

#define NUM_POINTS_TREE 4

#define Z_PROJECT_MULT_BITS 2
#define Z_LOOKUP_NUM_Z ( 64 << Z_PROJECT_MULT_BITS )
#define Z_LOOKUP_SIZE ( 2 * NUM_POINTS_TREE * Z_LOOKUP_NUM_Z )

#define Z_MAX_CULLING TO_FIXED( 70 )
#define X_MAX_CULLING TO_FIXED( 32 + 15 )

#define DISTANCE_COLLIDE_PLAYER 10

#define MAX_PLAYER_VEL_X 387
#define MAX_PLAYER_VEL_Z 128
#define MIN_PLAYER_VEL_Z -5
#define ACCEL_PLAYER_Z 6

#define DEFAULT_GROUND_LEVEL 48
#define MAX_GROUND_LEVEL 56
#define MIN_GROUND_LEVEL 30

#define MOUNTAIN_Z_CYCLE 115

#define imin( a, b ) ( a > b ? b : a )
#define imax( a, b ) ( a < b ? b : a )
#define iabs( a ) ( a < 0 ? -a : a )


// Global variables

struct in_UDK keyboardMapping;

uint8_t groundLevel;
uint16_t skyBytes;
uint16_t groundBytes;

int16_t treeX[ NUM_TREES ];
int16_t treeZ[ NUM_TREES ];

int16_t originalTreePointsX[ NUM_POINTS_TREE ];
int16_t originalTreePointsY[ NUM_POINTS_TREE ];

int16_t originalSpeederPoints[ 6 << 2 ];

int16_t zLookup[ Z_LOOKUP_SIZE ];
int16_t *tempZlookup;

int16_t triCoords[ 6 ];

int16_t playerPosX = 0;
int16_t playerVelX = 0;
int16_t playerVelZ = 0;
uint8_t playerStop = 0;

int16_t temp;
uint8_t palette1[ 16 ];    
int16_t b;
int16_t seed;
uint8_t color;
uint8_t *vram;
int16_t i, j, n;

uint8_t turbo;
uint8_t key = 0;

int16_t x0p;
int16_t x0;
int16_t y0;

int16_t firstTree;

int16_t zMountain;

#define bip() bit_frequency( 0.1, 400.0 )

// Function prototypes
uint8_t readKeyboardAndMouse();
void downGroundLevel();
void upGroundLevel();
void resetGroundLevel();
void drawMountain( int z );

// MAIN
void main() {
    
    // Save turbo and set turbo X8
    turbo = TURBO_get();
    TURBO_set( ZXUNO_TURBO_X8 );

    // Set text mode and show initial message
    setVideoMode( VIDEO_MODE_ZX_DEFAULT );
    
    printf( "\nSpeeder Demo" );
    printf( "\n\nInitializing..." );

    seed = clock();
    //seed = 1234;
    srand( seed );
    
    resetGroundLevel();
    zMountain = 0;

    // This are the objects vector definitions

    // Tree points.
    originalTreePointsX[ 0 ] = 6;
    originalTreePointsY[ 0 ] = -25;
    originalTreePointsX[ 1 ] = 4;
    originalTreePointsY[ 1 ] = 12;
    originalTreePointsX[ 2 ] = 27;
    originalTreePointsY[ 2 ] = 12;
    originalTreePointsX[ 3 ] = 0;
    originalTreePointsY[ 3 ] = 77;
    
    // Speeder points
    b = 0;
    originalSpeederPoints[ b++ ] = 5 + 64;
    originalSpeederPoints[ b++ ] = -26 + 96;
    originalSpeederPoints[ b++ ] = 14 + 64;
    originalSpeederPoints[ b++ ] = -15 + 96;
    originalSpeederPoints[ b++ ] = -4 + 64;
    originalSpeederPoints[ b++ ] = -18 + 96;

    originalSpeederPoints[ b++ ] = 2 + 64;
    originalSpeederPoints[ b++ ] = -20 + 96;
    originalSpeederPoints[ b++ ] = 24 + 64;
    originalSpeederPoints[ b++ ] = 95;
    originalSpeederPoints[ b++ ] = 10 + 64;
    originalSpeederPoints[ b++ ] = 95;

    originalSpeederPoints[ b++ ] = -5 + 64;
    originalSpeederPoints[ b++ ] = -26 + 96;
    originalSpeederPoints[ b++ ] = 4 + 64;
    originalSpeederPoints[ b++ ] = -18 + 96;
    originalSpeederPoints[ b++ ] = -14 + 64;
    originalSpeederPoints[ b++ ] = -15 + 96;

    originalSpeederPoints[ b++ ] = -2 + 64;
    originalSpeederPoints[ b++ ] = -20 + 96;
    originalSpeederPoints[ b++ ] = -10 + 64;
    originalSpeederPoints[ b++ ] = 95;
    originalSpeederPoints[ b++ ] = -24 + 64;
    originalSpeederPoints[ b++ ] = 95;

    // Precompute tree's projected points lookup table
    printf( "\n\nPrecomputing tables..." );
    b = 0;
    for ( i = 0; i < Z_LOOKUP_NUM_Z; i++ ) {
        for ( j = 0; j < NUM_POINTS_TREE; j++ ) {
            zLookup[ b++ ] = ((int16_t)( ( originalTreePointsX[ j ] << Z_PROJECT_MULT_BITS ) / ( i + 1 ) ) );
            zLookup[ b++ ] = ((int16_t)( ( originalTreePointsY[ j ] << Z_PROJECT_MULT_BITS ) / ( i + 1 ) ) );
        }
    }

    // Init trees
    printf( "\n\nIniting trees..." );
    b = 1;
    for ( i = 0; i < NUM_TREES; i++ ) {

        treeX[ i ] = 0;
        while ( treeX[ i ] < DISTANCE_COLLIDE_PLAYER ) {
            treeX[ i ] = ( rand() & 0x3FFF );
        }
        if ( rand() & 1 ) {
            treeX[ i ] = - treeX[ i ];
        }
        treeZ[ i ] = 0;
        while ( treeZ[ i ] < DISTANCE_COLLIDE_PLAYER ) {
            treeZ[ i ] = rand() & 0x0FFF;
            //treeZ[ i ] = ( rand() & MAX_Z );
        }
        if ( rand() & 1 ) {
            treeZ[ i ] = - treeZ[ i ];
        }

    }

    // Sort tree array
    printf( "\n\nSorting trees..." );
    b = 1;
    while ( b ) {
        b = 0;
        for ( i = 0; i < NUM_TREES - 1; i++ ) {
            if ( treeZ[ i ] < treeZ[ i + 1 ] ) {
                temp = treeZ[ i ];
                treeZ[ i ] = treeZ[ i + 1 ];
                treeZ[ i + 1 ] = temp;
                temp = treeX[ i ];
                treeX[ i ] = treeX[ i + 1 ];
                treeX[ i + 1 ] = temp;
                b = 1;
            }
        }
    }
    firstTree = 0;

    
    // Set Radastan video mode
    setVideoMode( VIDEO_MODE_RADASTAN );

    // Set color palette
    palette1[ BLACK_COLOR ] = 0;
    palette1[ GROUND_COLOR ] = 0x91;
    palette1[ SPEEDER_COLOR1 ] = 0x25;
    palette1[ SPEEDER_COLOR2 ] = 0x4A;
    palette1[ BROWN_COLOR ] = 0x10;
    palette1[ SKY_COLOR ] = 0xA2;
    palette1[ SKY_COLOR_DEG0 ] = 0xC6;
    palette1[ SKY_COLOR_DEG1 ] = 0xCB;
    palette1[ SKY_COLOR_DEG2 ] = 0xEF;

    b = 3;
    for ( i = MIN_GREEN_COLOR; i < MIN_GREEN_COLOR + NUM_GREEN_COLORS; i++ ) {
        palette1[ i ] = ( b++ ) << 5;
    }

    setPalette( palette1 );

    // Setup keyboard & joystick
    keyboardMapping.fire = in_LookupKey( ' ' );
    keyboardMapping.left = in_LookupKey( 'o' );
    keyboardMapping.right = in_LookupKey( 'p' );
    keyboardMapping.down = in_LookupKey( 'a' );
    keyboardMapping.up = in_LookupKey( 'q' );


    // MAIN LOOP
    while ( key != in_FIRE ) {

        // Player control
        key = readKeyboardAndMouse();
        if ( key & in_UP ) {
            if ( playerStop == 0 ) {
                temp = playerVelZ < 0;
                playerVelZ += ACCEL_PLAYER_Z;
                if ( playerVelZ > 0 ) {
                    downGroundLevel();
                    if ( temp ) {
                        playerVelZ = 0;
                        playerStop = 1;
                        resetGroundLevel();
                    }
                    else if ( playerVelZ > MAX_PLAYER_VEL_Z ) {
                        playerVelZ = MAX_PLAYER_VEL_Z;
                    }
                }
            }
        }
        else if ( key & in_DOWN ) {
            if ( playerStop == 0 ) {
                temp = playerVelZ > 0;
                playerVelZ -= ACCEL_PLAYER_Z;
                if ( playerVelZ >= 0 ) {
                    upGroundLevel();
                }
                if ( playerVelZ <= 0 ) {
                    if ( temp ) {
                        playerVelZ = 0;
                        playerStop = 1;
                        resetGroundLevel();
                    }
                    else if ( playerVelZ < MIN_PLAYER_VEL_Z ) {
                        playerVelZ = MIN_PLAYER_VEL_Z;
                    }
                }
            }
        }
        else {
            playerStop = 0;
        }

        if ( key & in_LEFT ) {
            playerVelX = - MAX_PLAYER_VEL_X;
        }
        else if ( key & in_RIGHT ) {
            playerVelX = MAX_PLAYER_VEL_X;
        }
        else {
            playerVelX = 0;
        }

        // Player movement
        playerPosX += playerVelX;
        if ( playerPosX > MAX_X ) {
            playerPosX = MIN_X;
        }
        else if ( playerPosX < MIN_X ) {
            playerPosX = MAX_X;
        }

        // Here starts the drawing of the frame

        // Get pointer to current drawing screen
        vram = getVRAMPointer();

        // Draw sky and ground, covering all the screen
        // Sky
        memset( vram, SKY_COLOR | ( SKY_COLOR << 4 ), skyBytes );
        // Sky degradation
        temp = skyBytes;
        b = 192;
        for ( i = 0; i < SKY_COLOR_DEG_SIZE; i++ ) {
            color = SKY_COLOR_DEG0 + i;
            color = color | ( color << 4 );
            memset( vram + temp, color, b );
            temp+= b;
            b -= 64;
        }
        // Ground
        memset( vram + temp, GROUND_COLOR | ( GROUND_COLOR << 4 ), groundBytes );
        
        // Mountains movement
        if ( playerVelZ > 0 ) {
            zMountain += FROM_FIXED( playerVelZ );
            if ( zMountain >= MOUNTAIN_Z_CYCLE ) {
                zMountain -= MOUNTAIN_Z_CYCLE;
            }
            else if ( zMountain < 0 ) {
                zMountain += MOUNTAIN_Z_CYCLE;
            }
        }

        // Draw mountains
        drawMountain( zMountain );
        temp = zMountain + ( MOUNTAIN_Z_CYCLE >> 1 );
        if ( temp >= MOUNTAIN_Z_CYCLE ) {
            temp -= MOUNTAIN_Z_CYCLE;
        }
        drawMountain( temp );
        
        temp = zMountain + 70;
        if ( temp >= MOUNTAIN_Z_CYCLE ) {
            temp -= MOUNTAIN_Z_CYCLE;
        }
        drawMountain( - temp );
        temp = zMountain + 70 + ( MOUNTAIN_Z_CYCLE >> 1 );
        if ( temp >= MOUNTAIN_Z_CYCLE ) {
            temp -= MOUNTAIN_Z_CYCLE;
        }
        drawMountain( - temp );

        // Process trees in descending z order
        for ( i = firstTree, n = NUM_TREES; n > 0; i = ( i + 1 ) % NUM_TREES, n-- ) {

            treeZ[ i ] -= playerVelZ;
            
            if ( treeZ[ i ] > MAX_Z ) {
                treeZ[ i ] -= MAX_Z;
                treeZ[ i ] += MIN_Z;
                firstTree++;
                if ( firstTree >= NUM_TREES ) {
                    firstTree -= NUM_TREES;
                }
                treeX[ i ] = ( rand() & 0x3FFF );
                if ( rand() & 1 ) {
                    treeX[ i ] = - treeX[ i ];
                }
            }
            else if ( treeZ[ i ] < MIN_Z ) {
                treeZ[ i ] += MAX_Z;
                treeZ[ i ] -= MIN_Z;
                firstTree--;
                if ( firstTree < 0 ) {
                    firstTree += NUM_TREES;
                }
                treeX[ i ] = ( rand() & 0x3FFF );
                if ( rand() & 1 ) {
                    treeX[ i ] = - treeX[ i ];
                }
            }

            if ( treeZ[ i ] > ( 1 << Z_PROJECT_MULT_BITS ) && treeZ[ i ] < Z_MAX_CULLING) {

                // Draw the tree

                // Draw tree trunk

                b = FROM_FIXED( treeZ[ i ] << Z_PROJECT_MULT_BITS );
                tempZlookup = zLookup + ( b << 3 );
                
                // Project tree
                temp = treeX[ i ] - playerPosX;
                if ( temp > MAX_X ) {
                    temp-= MAX_X;
                    temp+= MIN_X;
                }
                else if ( temp < MIN_X ) {
                    temp-= MIN_X;
                    temp+= MAX_X;
                }
                x0 = 64 + ( temp / ( treeZ[ i ] > 0 ? treeZ[ i ] : TO_FIXED( 1 ) ) );
                
                // Cull horizontally
                if ( x0 > X_MAX_CULLING || x0 < - X_MAX_CULLING ) {
                    continue;
                }

                y0 = groundLevel;

                triCoords[ 0 ] = ( tempZlookup[ 0 ] ) + x0;
                triCoords[ 1 ] = - ( tempZlookup[ 1 ] ) + y0;
                triCoords[ 2 ] = - ( tempZlookup[ 0 ] ) + x0;
                triCoords[ 3 ] = - ( tempZlookup[ 1 ] ) + y0;
                triCoords[ 4 ] = ( tempZlookup[ 2 ] ) + x0;
                triCoords[ 5 ] = - ( tempZlookup[ 3 ] ) + y0;

                // Trunk color
                color = BROWN_COLOR | ( BROWN_COLOR << 4 );

                drawTriangle( triCoords, color, screenScanTrimmed, vram );

                triCoords[ 0 ] = - ( tempZlookup[ 0 ] ) + x0;
                triCoords[ 1 ] = - ( tempZlookup[ 1 ] ) + y0;
                triCoords[ 2 ] = - ( tempZlookup[ 2 ] ) + x0;
                triCoords[ 3 ] = - ( tempZlookup[ 3 ] ) + y0;
                triCoords[ 4 ] = ( tempZlookup[ 2 ] ) + x0;
                triCoords[ 5 ] = - ( tempZlookup[ 3 ] ) + y0;

                drawTriangle( triCoords, color, screenScanTrimmed, vram );

                // Draw tree leaves

                triCoords[ 0 ] = ( tempZlookup[ 4 ]  ) + x0;
                triCoords[ 1 ] = - ( tempZlookup[ 5 ]  ) + y0;
                triCoords[ 2 ] = - ( tempZlookup[ 4 ]  ) + x0;
                triCoords[ 3 ] = - ( tempZlookup[ 5 ]  ) + y0;
                triCoords[ 4 ] = ( tempZlookup[ 6 ]  ) + x0;
                triCoords[ 5 ] = - ( tempZlookup[ 7 ] ) + y0;

                // leaves color
                //color = MIN_GREEN_COLOR + NUM_GREEN_COLORS - 1 - ( FROM_FIXED( treeZ[ i ] ) >> 3 );
                
                color = treeZ[ i ] >> 8;
                if ( color >= NUM_GREEN_COLORS ) {
                    color = NUM_GREEN_COLORS - 1;
                }
                color = MIN_GREEN_COLOR + NUM_GREEN_COLORS - 1 - color;
                color = color | ( color << 4 );

                drawTriangle( triCoords, color, screenScanTrimmed, vram );
            }
        }
        
        // Draw the speeder
        drawTriangle( originalSpeederPoints + 6, SPEEDER_COLOR2 | ( SPEEDER_COLOR2 << 4 ), screenScanTrimmed, vram );
        drawTriangle( originalSpeederPoints + 18, SPEEDER_COLOR2 | ( SPEEDER_COLOR2 << 4 ), screenScanTrimmed, vram );
        drawTriangle( originalSpeederPoints, SPEEDER_COLOR1 | ( SPEEDER_COLOR1 << 4 ), screenScanTrimmed, vram );
        drawTriangle( originalSpeederPoints + 12, SPEEDER_COLOR1 | ( SPEEDER_COLOR1 << 4 ), screenScanTrimmed, vram );

        // Blit the screen
        blitVRAM();

    }

    setVideoMode( VIDEO_MODE_ZX_DEFAULT );

    printf( "Game end. Have a nice day.\n" );

    TURBO_set( turbo );

    while ( 1 ) {}
}

uint8_t readKeyboardAndMouse() {
    return ((void*)in_JoyKeyboard)( &keyboardMapping ) | ((void*)in_JoySinclair1)( &keyboardMapping );
}

void resetGroundLevel() {
    groundLevel = 48;
    skyBytes = ( 6144 >> 1 ) - ( ( 1 + 2 + 3 ) << 6 );
    groundBytes = 6144 >> 1;
}

void downGroundLevel() {

    if ( groundLevel >= MAX_GROUND_LEVEL - 1 ) {
        return;
    }
    
    groundLevel++;

    skyBytes+= screenLineNumBytes;
    groundBytes-= screenLineNumBytes;
}

void upGroundLevel() {

    if ( groundLevel <= MIN_GROUND_LEVEL ) {
        return;
    }

    groundLevel--;

    skyBytes-= screenLineNumBytes;
    groundBytes+= screenLineNumBytes;
}

void drawMountain( int z ) {
    
    int16_t x = abs( z );
    triCoords[ 0 ] = 64 + z;
    triCoords[ 1 ] = groundLevel - ( x >> 2 );
    triCoords[ 2 ] = triCoords[ 0 ] + ( x >> 1 );
    triCoords[ 3 ] = groundLevel;
    triCoords[ 4 ] = triCoords[ 0 ] - ( x >> 1 );
    triCoords[ 5 ] = groundLevel;

    drawTriangle( triCoords, GROUND_COLOR | ( GROUND_COLOR << 4 ), screenScanTrimmed, vram );
    
}
