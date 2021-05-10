/*
 * @author yomboprime
 *
 * Video player
*/
#include <stdio.h>
#include <input.h>
#include <stdlib.h>
#include <spectrum.h>

#include <time.h>

#include "../src/zxuno/zxuno.h"
#include "../src/zxuno/radas.h"
#include "../src/esxdos.h"
#include "../src/textUtils.h"
#include "../src/fileDialog.h"

// Function prototypes

bool openVideoFile();
bool readVideoFrame( uint8_t *screen );
bool closeVideoFile();

// Global variables

#define PATH_SIZE ( 200 )
uint8_t filePath[ PATH_SIZE ];

#define ONLY_VIDEO_FRAME_SIZE ( RADAS_SCREEN_NUM_BYTES + 16 )
#define AUDIO_FRAME_SIZE 882
#define VIDEO_AND_AUDIO_FRAME_SIZE ( AUDIO_FRAME_SIZE + ONLY_VIDEO_FRAME_SIZE )

int16_t drive;
int16_t videoFileHandle;

uint8_t *screenPtr;

uint8_t errorString[ 200 ];

static uint8_t paleta[] = {

    // Pixels en formato gggrrrbb

    0x0,
    0xc3,
    0xfa,
    0x6f,
    0x6d,
    0x86,
    0xd0,
    0xfc,
    0xff,
    0xdb,
    0x92,
    0x1c,
    0xc0,
    0x23,
    0xab,
    0x9c
};

uint8_t c, c2;
bool errorFlag = false;
int32_t ci;
int32_t errors, matches, total, totalErrors;
uint16_t seed = 1234;

uint8_t colorFunc1( uint32_t i ) {

    return i & 1 != 0 ? 0xE0 : 0x1C;

}

void main(void) {

    sprintf( filePath, "/dummy/" );
    sprintf( errorString, "No error." );

    textUtils_32ColumnsMode();
    textUtils_cls();
    zx_border( INK_BLUE );




totalErrors = 0;

//for ( seed = 1234; seed < 1235; seed++ ) {

//    textUtils_println( "Press a key..." );
//    waitKeyPress( 350 );

    #define RAM_DATA_REG 250
    #define RAM_STAT_REG 251

    //#define RAM_LENGTH 16384
    //#define RAM_LENGTH 32768
    //#define RAM_LENGTH 65536
    //#define RAM_LENGTH 131072
    //#define RAM_LENGTH 262144

    #define RAM_LENGTH 524288

    //#define RAM_LENGTH 262145

    // Reset ram pointer
    outp( ZXUNO_ADDR, RAM_STAT_REG );
    outp( ZXUNO_REG, 4 );

    outp( ZXUNO_ADDR, RAM_DATA_REG );
    outp( ZXUNO_REG, 85 );

    srand( seed );

    for ( ci = 0; ci < RAM_LENGTH; ci++ ) {

        //c = colorFunc1( ci );

        //c = 85;

        //c = 16;
        //c = ci >> 1;//      PROBAR ESTOS DOS o con mas incermetno
        //c = ci & 0x01C;
        c = ci;
        //c = ( ci >> 10 ) & 0x0FF;
        //c = 0;
        //c = ( ci & 0x07F800 ) >> 11;
        //c = (uint8_t)( rand() );

        // Write to sram
        outp( ZXUNO_ADDR, RAM_STAT_REG );
        outp( ZXUNO_REG, 0 );

        // Write value
        outp( ZXUNO_ADDR, RAM_DATA_REG );
        outp( ZXUNO_REG, c );
/*
        // Increment pointer
        outp( ZXUNO_ADDR, RAM_STAT_REG );
        outp( ZXUNO_REG, 2 );
        outp( ZXUNO_ADDR, RAM_DATA_REG );
        outp( ZXUNO_REG, 85 );
*/
    }

    textUtils_println( "Finished writing, verifying..." );

    // Reset ram pointer
    outp( ZXUNO_ADDR, RAM_STAT_REG );
    outp( ZXUNO_REG, 4 );

    outp( ZXUNO_ADDR, RAM_DATA_REG );
    outp( ZXUNO_REG, 85 );


    srand( seed );

    errors = 0;
    matches = 0;
    total = 0;

    errorFlag = false;

    for ( ci = 0; ci < RAM_LENGTH; ci++ ) {

        //c = colorFunc1( ci );

        //c = 85;

        //c = 16;
        //c = ci >> 1;
        //c = ci & 0x01C;
        c = ci;
        //c = ( ci >> 10 ) & 0x0FF;
        //c = ( ci & 0x07F800 ) >> 11;
        //c = (uint8_t)( rand() );

        // Read sram
        outp( ZXUNO_ADDR, RAM_STAT_REG );
        outp( ZXUNO_REG, 1 );

        // Read value
        outp( ZXUNO_ADDR, RAM_DATA_REG );
        outp( ZXUNO_REG, 85 );
        c2 = inp( ZXUNO_REG );
/*
        // Increment pointer
        outp( ZXUNO_ADDR, RAM_STAT_REG );
        outp( ZXUNO_REG, 2 );
        outp( ZXUNO_ADDR, RAM_DATA_REG );
        outp( ZXUNO_REG, 85 );
*/
        if ( c != c2 ) {
/*
            if ( errorFlag == false ) {
                textUtils_print( "Bad byte in pos: " );
                textUtils_println_l( ci );
                textUtils_print( "Read byte: " );
                textUtils_println_l( c2 );
                textUtils_print( "Rand byte: " );
                textUtils_println_l( c );
                //waitKey();
            }

            errorFlag = true;
*/
            errors++;
            totalErrors++;

        }
        else {
/*
            if ( errorFlag == true ) {
                textUtils_print( "Good byte in pos: " );
                textUtils_println_l( ci );
                textUtils_print( "byte: " );
                textUtils_println_l( c2 );
                //waitKey();
            }

            errorFlag = false;
*/
            matches++;
        }

        total++;
    }

    textUtils_print( "err: " );
    textUtils_print_l( errors );
    textUtils_print( ", mat: " );
    textUtils_print_l( matches );
    textUtils_print( ", tot: " );
    textUtils_println_l( total );

    textUtils_print( "Total errors: " );
    textUtils_println_l( totalErrors );

//}

textUtils_print( "\nFinished test." );

waitKeyPress( 350 );



























/*
1
    textUtils_println( "Press a key..." );

    waitKeyPress();

    #define RAM_DATA_REG 250
    #define RAM_STAT_REG 251
    #define RAM_LENGTH 32

    // Reset ram pointer
    outp( ZXUNO_ADDR, RAM_STAT_REG );
    outp( ZXUNO_REG, 0 );

    srand( 1234 );

    outp( ZXUNO_ADDR, RAM_DATA_REG );
    for ( ci = 0; ci < RAM_LENGTH; ci++ ) {

        c = ci;// - 16;//rand();

        outp( ZXUNO_REG, c );

    }

    textUtils_println( "Finished writing, verifying..." );

    // Reset ram pointer
    outp( ZXUNO_ADDR, RAM_STAT_REG );
    outp( ZXUNO_REG, 0 );

    srand( 1234 );

    for ( ci = 0; ci < RAM_LENGTH; ci++ ) {

        c = ci;//rand();

        outp( ZXUNO_ADDR, RAM_STAT_REG );
        outp( ZXUNO_REG, 1 );

        outp( ZXUNO_ADDR, RAM_DATA_REG );
        c2 = inp( ZXUNO_REG );

        if ( c != c2 ) {

            textUtils_print( "Bad byte in pos: " );
            textUtils_println_l( ci );
            textUtils_print( "Read byte: " );
            textUtils_println_l( c2 );
            textUtils_print( "Rand byte: " );
            textUtils_println_l( c );

            waitKey();

        }

    }
    textUtils_print( "\nFinished test." );

    waitKeyPress();
1
*/




    while ( 1 ) {

        textUtils_println( "Starting..." );
        waitKeyPress();

        fileDialogpathUpOneDir( filePath );
        while ( openFileDialog( "Open .rdm or .rva video file", filePath, PATH_SIZE, INK_BLUE | PAPER_WHITE, INK_WHITE | PAPER_CYAN ) == false ) {

            textUtils_println( "Didn't select a file." );
            waitKeyPress();

        }

        textUtils_print( "Selected file: " );
        textUtils_println( filePath );
        textUtils_println( "\nPress a key to play the video... " );
        waitKeyPress();

        radasSetRadastanMode( PAPER_WHITE | INK_BLACK, PAPER_BLUE, paleta );

        while ( in_Inkey() != 0 ) {
            // Wait for user to release key
        }

        if ( openVideoFile() == true ) {

            screenPtr = radasGetCurrentScreen();

            sprintf( errorString, "Reached end of file or error." );
            while ( readVideoFrame( screenPtr ) == true ) {

                radasSetPalette( screenPtr + RADAS_SCREEN_NUM_BYTES );

                radasChangeScreens();

                screenPtr = radasGetCurrentScreen();

                if ( in_Inkey() != 0 ) {
                    sprintf( errorString, "Interrupted by user." );
                    break;
                }

    //            delay( 50 );

            }

            closeVideoFile();

        }
        else {
            sprintf( errorString, "Couldn't open video file." );
        }

    /*
        radasDrawRectangle( 5, 10, 20, 12, 0x18 );
        for ( i = 0; i < 16; i++ ) {
            radasDrawRectangle( i * 4, 48, 4, 1, i & 1 ? 0x11: 0x22 );
            radasDrawRectangle( i * 4, 50, 4, 8, i | ( i << 4 ) );
        }
    */

        /*key = 0;
        while ( in_Inkey() != 0 ) {
            // Wait for the user to release key
        }
        while ( key == 0 ) {



            key = in_Inkey();

        }*/

        radasSetNormalMode( PAPER_WHITE | INK_BLACK, PAPER_BLUE );

        textUtils_print( "Message: " );
        textUtils_println( errorString );

        waitKeyPress();

    }

}

bool openVideoFile() {

    drive = ESXDOS_getDefaultDrive();

    iferror {
        return false;
    }

    videoFileHandle = ESXDOS_fopen( filePath, ESXDOS_FILEMODE_READ, drive );
    iferror {
        return false;
    }

    return true;
}

bool readVideoFrame( uint8_t *screen ) {

    uint16_t i, numBytes;
    uint8_t c;
    bool doEnd = false;

    numBytes = ESXDOS_fread( screen, ONLY_VIDEO_FRAME_SIZE, videoFileHandle );

    iferror {
        return false;
    }

    if ( numBytes < ONLY_VIDEO_FRAME_SIZE ) {
        // Reached end of file
        return false;
    }

    return true;
}

bool closeVideoFile() {

    ESXDOS_fclose( videoFileHandle );
    iferror {
        return false;
    }

    return true;

}
