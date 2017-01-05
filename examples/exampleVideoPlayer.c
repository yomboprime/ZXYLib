/*
 * @author yomboprime
 *
 * Video player
*/
#include <stdio.h>
#include <input.h>
#include <stdlib.h>
#include <spectrum.h>

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

void main(void) {

    uint16_t i;
    uint8_t key;

    sprintf( filePath, "/dummy/" );
    sprintf( errorString, "No error.")

    textUtils_32ColumnsMode();
    textUtils_cls();
    zx_border( INK_BLUE );

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
