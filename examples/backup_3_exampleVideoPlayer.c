/*
 * @author yomboprime
 *
 * Media player
*/

#include <stdio.h>
#include <input.h>
#include <stdlib.h>
#include <spectrum.h>

#include <time.h>

#include "../src/zxuno/zxuno.h"
#include "../src/zxuno/turbo.h"
#include "../src/zxuno/radas.h"
#include "../src/esxdos.h"
#include "../src/textUtils.h"
#include "../src/fileDialog.h"

// Function prototypes

uint32_t loadWAVFile();
void playWAVFile();
void playRadastanVideoFile();
bool openMediaFile();
bool closeMediaFile();
bool readVideoFrame( uint8_t *screen );

// Global variables

#define PATH_SIZE ( 200 )
uint8_t filePath[ PATH_SIZE ];

#define BUFFER_SIZE 7810
//uint8_t buffer[ BUFFER_SIZE ];
uint8_t *buffer = RADAS_SCREEN_ADDR_1;

#define ONLY_VIDEO_FRAME_SIZE ( RADAS_SCREEN_NUM_BYTES + 16 )
#define AUDIO_FRAME_SIZE 882
#define VIDEO_AND_AUDIO_FRAME_SIZE ( AUDIO_FRAME_SIZE + ONLY_VIDEO_FRAME_SIZE )

// WAV format definitions
#define WAV_RIFF_OFFSET 0
#define WAV_WAVEFMT_OFFSET 8
#define WAV_FORMAT1_OFFSET 16
#define WAV_FORMAT2_OFFSET 20
#define WAV_NUM_CHANNELS 22
#define WAV_SAMPLE_FREQUENCY 24
#define WAV_DATA 36
#define WAV_SAMPLE_DATA 44

#define SRAM_LENGTH 524288
#define RAM_DATA_REG 250
#define RAM_STAT_REG 251

int16_t drive;
int16_t mediaFileHandle;
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

uint32_t numBytesWAV;

void main(void) {

    sprintf( filePath, "/dummy/" );
    sprintf( errorString, "No error." );

    textUtils_32ColumnsMode();
    textUtils_cls();
    zx_border( INK_BLUE );

    while ( 1 ) {

        fileDialogpathUpOneDir( filePath );
        while ( openFileDialog( "Open media file", filePath, PATH_SIZE, INK_BLUE | PAPER_WHITE, INK_WHITE | PAPER_CYAN ) == false ) {

            textUtils_cls();
            textUtils_println( "Didn't select a file." );
            waitKeyPress();

        }

//        TURBO_set( ZXUNO_TURBO_X4 );

        textUtils_cls();

        textUtils_print( "Selected file: " );
        textUtils_println( filePath );
        waitKeyPress();
        while ( in_Inkey() != 0 ) {
            // Wait for user to release key
        }

        textUtils_println( "\nLoading WAV file... " );

        numBytesWAV = loadWAVFile();
        //numBytesWAV = 1;

        textUtils_print( "Loaded " );
        textUtils_print_l( numBytesWAV );
        textUtils_println( " bytes." );

        if ( numBytesWAV <= 0 ) {
            textUtils_println( "Error loading WAV file." );
            waitKeyPress();
            continue;
        }

        textUtils_println( "Press a key to play..." );
        waitKeyPress();
        while ( in_Inkey() != 0 ) {
            // Wait for user to release key
        }
        textUtils_println( "Playing WAV file... " );

        playWAVFile();

        //playRadastanVideoFile();

        textUtils_print( "Message: " );
        textUtils_println( errorString );

//        TURBO_set( ZXUNO_TURBO_X1 );

        waitKeyPress();

    }

}



void playRadastanVideoFile() {

    radasSetRadastanMode( PAPER_WHITE | INK_BLACK, PAPER_BLUE, paleta );

    if ( openMediaFile() == true ) {

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

        closeMediaFile();

    }
    else {
        sprintf( errorString, "Couldn't open video file." );
    }

    radasSetNormalMode( PAPER_WHITE | INK_BLACK, PAPER_BLUE );

}

void playWAVFile() {

    bool doEnd = false;

    // Reset ram pointer
    outp( ZXUNO_ADDR, RAM_STAT_REG );
    outp( ZXUNO_REG, 4 );
    outp( ZXUNO_ADDR, RAM_DATA_REG );
    outp( ZXUNO_REG, 85 );

    // Play the wav file
    outp( ZXUNO_ADDR, RAM_STAT_REG );
    outp( ZXUNO_REG, 8 );
    outp( ZXUNO_ADDR, RAM_DATA_REG );
    outp( ZXUNO_REG, 85 );

    while ( doEnd == false ) {
        if ( in_Inkey() != 0 ) {
            sprintf( errorString, "Interrupted by user." );
            doEnd = true;
            break;
        }

        outp( ZXUNO_ADDR, RAM_STAT_REG );
        if ( inp( ZXUNO_REG ) == 0 ) {
            sprintf( errorString, "Finished playing." );
            doEnd = true;
            break;
        }

    }

}

uint32_t loadWAVFile() {

    // Loads a PCM 8 bit mono sample to sram
    // Returns num bytes loaded

    uint32_t numBytesToLoad;
    uint32_t bytesLeftToLoad;
    uint16_t i, numBytesToRead, numBytesRead;
    uint8_t c;
    uint8_t * p;
    bool doEnd = false;

    if ( openMediaFile() == true ) {

        sprintf( errorString, "" );

        numBytesToLoad = ESXDOS_fsize( mediaFileHandle );
        if ( numBytesToLoad < 44 ) {
            sprintf( errorString, "File is too short to be a WAV file." );
            return 0;
        }
        numBytesToLoad -= 44;

        if ( numBytesToLoad > SRAM_LENGTH ) {
            numBytesToLoad = SRAM_LENGTH;
        }

        bytesLeftToLoad = numBytesToLoad;

        ESXDOS_fseek( 44, ESXDOS_SEEK_FROM_START, mediaFileHandle );

        // Reset ram pointer
        outp( ZXUNO_ADDR, RAM_STAT_REG );
        outp( ZXUNO_REG, 4 );
        outp( ZXUNO_ADDR, RAM_DATA_REG );
        outp( ZXUNO_REG, 85 );

        // Write to sram
        outp( ZXUNO_ADDR, RAM_STAT_REG );
        outp( ZXUNO_REG, 0 );
        outp( ZXUNO_ADDR, RAM_DATA_REG );

        while ( doEnd == false && bytesLeftToLoad > 0 ) {

            numBytesToRead = BUFFER_SIZE;
            if ( numBytesToRead > bytesLeftToLoad ) {
                numBytesToRead = bytesLeftToLoad;
            }

            numBytesRead = ESXDOS_fread( buffer, numBytesToRead, mediaFileHandle );

            iferror {
                sprintf( errorString, "Error reading the file" );
                doEnd = true;
                break;
            }

            if ( numBytesRead < numBytesToRead ) {
                // Reached end of file
                doEnd = true;
                break;
            }

            bytesLeftToLoad -= numBytesRead;

            p = buffer;
            while ( numBytesRead > 0 ) {

                // Write value to sram
                outp( ZXUNO_REG, *p++ );

                numBytesRead--;
            }

        }

        closeMediaFile();

    }
    else {
        sprintf( errorString, "Couldn't open WAV file." );
        return 0;
    }

    return numBytesToLoad;

}

bool readVideoFrame( uint8_t *screen ) {

    uint16_t i, numBytes;
    uint8_t c;
    bool doEnd = false;

    numBytes = ESXDOS_fread( screen, ONLY_VIDEO_FRAME_SIZE, mediaFileHandle );

    iferror {
        return false;
    }

    if ( numBytes < ONLY_VIDEO_FRAME_SIZE ) {
        // Reached end of file
        return false;
    }

    return true;
}

bool openMediaFile() {

    drive = ESXDOS_getDefaultDrive();

    iferror {
        return false;
    }

    mediaFileHandle = ESXDOS_fopen( filePath, ESXDOS_FILEMODE_READ, drive );
    iferror {
        return false;
    }

    return true;
}

bool closeMediaFile() {

    ESXDOS_fclose( mediaFileHandle );
    iferror {
        return false;
    }

    return true;

}
