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

void playWavFileContinuously( uint16_t bufferSize );

void playWAVFile( uint32_t start, uint32_t end );

void playRadastanVideoFile();

bool openMediaFile();
bool closeMediaFile();

bool readVideoFrame( uint8_t *screen );

void resetWavunoIntUserPointer();
void setWavunoIntBeginAndEndAddress( uint32_t beginAddress, uint32_t endAddress );

void setWavunoExtUserPointer( uint32_t value );
void setWavunoExtBeginAndEndAddress( uint32_t beginAddress, uint32_t endAddress );

// Global variables

#define PATH_SIZE ( 200 )
uint8_t filePath[ PATH_SIZE ];

//#define BUFFER_SIZE 7810
#define BUFFER_SIZE 882
uint8_t buffer[ BUFFER_SIZE ];
//uint8_t *buffer = RADAS_SCREEN_ADDR_1;

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


#define WAVUNO_DATA_REG 250
#define WAVUNO_STAT_REG 251

#define WAVUNO_REG_INT_SAMPLE_RESET 0
#define WAVUNO_REG_INT_SAMPLE_WRITE 1
#define WAVUNO_REG_INT_SAMPLE_READ 2

#define WAVUNO_REG_INT_CONTROL_FORMAT 3
#define WAVUNO_REG_INT_CONTROL_BEGIN_REPROD 4
#define WAVUNO_REG_INT_CONTROL_END_REPROD 5

#define WAVUNO_REG_INT_FREQ_DIVIDER0 6
#define WAVUNO_REG_INT_FREQ_DIVIDER1 7

#define WAVUNO_REG_INT_START_LOOP0 8
#define WAVUNO_REG_INT_START_LOOP1 9

#define WAVUNO_REG_INT_END_LOOP0 10
#define WAVUNO_REG_INT_END_LOOP1 11

#define WAVUNO_REG_EXT_USER_POINTER0 12
#define WAVUNO_REG_EXT_USER_POINTER1 13
#define WAVUNO_REG_EXT_USER_POINTER2 14

#define WAVUNO_REG_EXT_SAMPLE_WRITE 15
#define WAVUNO_REG_EXT_SAMPLE_READ 16

#define WAVUNO_REG_EXT_CONTROL_FORMAT 17
#define WAVUNO_REG_EXT_CONTROL_BEGIN_REPROD 18
#define WAVUNO_REG_EXT_CONTROL_END_REPROD 19

#define WAVUNO_REG_EXT_FREQ_DIVIDER0 20
#define WAVUNO_REG_EXT_FREQ_DIVIDER1 21

#define WAVUNO_REG_EXT_START_LOOP0 22
#define WAVUNO_REG_EXT_START_LOOP1 23
#define WAVUNO_REG_EXT_START_LOOP2 24

#define WAVUNO_REG_EXT_END_LOOP0 25
#define WAVUNO_REG_EXT_END_LOOP1 26
#define WAVUNO_REG_EXT_END_LOOP2 27


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

    sprintf( filePath, "/sonidos/probando.wav" );
    sprintf( errorString, "No error." );

    textUtils_32ColumnsMode();
    textUtils_cls();
    zx_border( INK_BLUE );

    TURBO_set( ZXUNO_TURBO_X4 );
    textUtils_println( "\nLoading /sonidos/probando.wav... " );
    numBytesWAV = loadWAVFile();
    TURBO_set( ZXUNO_TURBO_X1 );

    while ( 1 ) {

        if ( numBytesWAV <= 0 ) {
            textUtils_println( "Error loading WAV file." );
            waitKeyPress();
            continue;
        }

        fileDialogpathUpOneDir( filePath );
        while ( openFileDialog( "Open media file", filePath, PATH_SIZE, INK_BLUE | PAPER_WHITE, INK_WHITE | PAPER_CYAN ) == false ) {

            textUtils_cls();
            textUtils_println( "Didn't select a file." );
            waitKeyPress();

        }

        TURBO_set( ZXUNO_TURBO_X4 );

        textUtils_cls();

        textUtils_print( "Selected file: " );
        textUtils_println( filePath );
        waitKeyPress();
        while ( in_Inkey() != 0 ) {
            // Wait for user to release key
        }

/*
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

        playWAVFile( 0, numBytesWAV - 1 );

        //playRadastanVideoFile();
*/


        textUtils_println( "\nPlaying WAV file... " );
        playWavFileContinuously( BUFFER_SIZE );


        textUtils_print( "\nMessage: " );
        textUtils_println( errorString );

        TURBO_set( ZXUNO_TURBO_X1 );

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

void playWAVFile( uint32_t start, uint32_t end ) {

    bool doEnd = false;

    setWavunoExtBeginAndEndAddress( start, end );

    // Set looping
    outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
    outp( ZXUNO_REG, WAVUNO_REG_EXT_CONTROL_FORMAT );
    outp( ZXUNO_ADDR, WAVUNO_DATA_REG );
    outp( ZXUNO_REG, 1 );

    // Play the wav file
    outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
    outp( ZXUNO_REG, WAVUNO_REG_EXT_CONTROL_BEGIN_REPROD );
    outp( ZXUNO_ADDR, WAVUNO_DATA_REG );
    outp( ZXUNO_REG, 1 );

    while ( doEnd == false ) {
        if ( in_Inkey() != 0 ) {

            // Stop playing
            outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
            outp( ZXUNO_REG, WAVUNO_REG_EXT_CONTROL_BEGIN_REPROD );
            outp( ZXUNO_ADDR, WAVUNO_DATA_REG );
            outp( ZXUNO_REG, 0 );


            sprintf( errorString, "Interrupted by user." );
            doEnd = true;
            break;
        }

        outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
        outp( ZXUNO_REG, WAVUNO_REG_EXT_CONTROL_BEGIN_REPROD );
        outp( ZXUNO_ADDR, WAVUNO_DATA_REG );
        if ( inp( ZXUNO_REG ) == 0 ) {
            sprintf( errorString, "Finished playing." );
            doEnd = true;
            break;
        }

        outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
        outp( ZXUNO_REG, WAVUNO_REG_EXT_CONTROL_END_REPROD );
        outp( ZXUNO_ADDR, WAVUNO_DATA_REG );
        if ( inp( ZXUNO_REG ) == 1 ) {
            outp( ZXUNO_REG, 0 );
            textUtils_print( "Looped." );
        }

    }

}

uint32_t loadWAVFile() {

    // Loads a PCM 8 bit mono sample to wavuno external sram
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
            closeMediaFile();
            return 0;
        }
        numBytesToLoad -= 44;

        if ( numBytesToLoad > SRAM_LENGTH ) {
            numBytesToLoad = SRAM_LENGTH;
        }

        bytesLeftToLoad = numBytesToLoad;

        ESXDOS_fseek( 44, ESXDOS_SEEK_FROM_START, mediaFileHandle );

        setWavunoExtUserPointer( 0 );

        // Write to sram
        outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
        outp( ZXUNO_REG, WAVUNO_REG_EXT_SAMPLE_WRITE );
        outp( ZXUNO_ADDR, WAVUNO_DATA_REG );

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

void playWavFileContinuously( uint16_t bufferSize ) {

    // Plays a PCM 8 bit sample using ping pong buffers in wavuno internal ram

    uint32_t bytesLeftToLoad;
    uint16_t numBytesToRead, numBytesRead;
    uint8_t *p;
    bool doEnd = false;
    uint32_t bufferSizeMinusOne = bufferSize - 1;
    uint32_t twoBufferSizeMinusOne = ( bufferSize << 1 ) - 1;
    uint8_t bufferTurn = 0;
    bool flagStart = true;

    if ( openMediaFile() == false ) {
        sprintf( errorString, "Couldn't open WAV file." );
        return;
    }

    bytesLeftToLoad = ESXDOS_fsize( mediaFileHandle );
    if ( bytesLeftToLoad < 44 ) {
        sprintf( errorString, "File is too short to be a WAV file." );
        closeMediaFile();
        return;
    }
    bytesLeftToLoad -= 44;

    ESXDOS_fseek( 44, ESXDOS_SEEK_FROM_START, mediaFileHandle );

    sprintf( errorString, "Finished playing WAV file." );

    // Set looping and
    // TODO stereo
    outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
    outp( ZXUNO_REG, WAVUNO_REG_INT_CONTROL_FORMAT );
    outp( ZXUNO_ADDR, WAVUNO_DATA_REG );
    outp( ZXUNO_REG, 1 );

    setWavunoIntBeginAndEndAddress( 0, bufferSizeMinusOne );

    while ( doEnd == false && bytesLeftToLoad > 0 ) {

        if ( in_Inkey() != 0 ) {
//            sprintf( errorString, "Interrupted by user." );
//            doEnd = true;
//            break;

            setWavunoExtBeginAndEndAddress( 0, numBytesWAV - 1 );

            // Set looping
            outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
            outp( ZXUNO_REG, WAVUNO_REG_EXT_CONTROL_FORMAT );
            outp( ZXUNO_ADDR, WAVUNO_DATA_REG );
            outp( ZXUNO_REG, 0 );

            // Play the wav file
            outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
            outp( ZXUNO_REG, WAVUNO_REG_EXT_CONTROL_BEGIN_REPROD );
            outp( ZXUNO_ADDR, WAVUNO_DATA_REG );
            outp( ZXUNO_REG, 1 );

        }

        numBytesToRead = bufferSize;
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
            sprintf( errorString, "Finished playing WAV file." );
            doEnd = true;
        }

        bytesLeftToLoad -= numBytesRead;

        // Write to Wavuno ram
        if ( bufferTurn == 0 ) {
            resetWavunoIntUserPointer();
        }

        outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
        outp( ZXUNO_REG, WAVUNO_REG_INT_SAMPLE_WRITE );
        outp( ZXUNO_ADDR, WAVUNO_DATA_REG );
        p = buffer;
        while ( numBytesRead > 0 ) {

            outp( ZXUNO_REG, *p++ );

            numBytesRead--;
        }

        if ( flagStart ) {

            // Begin to play the wav file
            outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
            outp( ZXUNO_REG, WAVUNO_REG_INT_CONTROL_BEGIN_REPROD );
            outp( ZXUNO_ADDR, WAVUNO_DATA_REG );
            outp( ZXUNO_REG, 1 );

            flagStart = false;

        }
        else {

            // Wait for loop to end
            outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
            outp( ZXUNO_REG, WAVUNO_REG_INT_CONTROL_END_REPROD );
            outp( ZXUNO_ADDR, WAVUNO_DATA_REG );
            while ( inp( ZXUNO_REG ) == 0 ) {
                // Keep waiting
            }
            // Clear loop completed flag
            outp( ZXUNO_REG, 0 );

        }

        // Stop playing
//        outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
//        outp( ZXUNO_REG, WAVUNO_REG_CONTROL_BEGIN_REPROD );
//        outp( ZXUNO_ADDR, WAVUNO_DATA_REG );
//        outp( ZXUNO_REG, 0 );

        // Switch begin and end addresses of next buffer to play
        if ( bufferTurn == 0 ) {
            setWavunoIntBeginAndEndAddress( bufferSize , twoBufferSizeMinusOne );
            bufferTurn = 1;
        }
        else {
            setWavunoIntBeginAndEndAddress( 0, bufferSizeMinusOne );
            bufferTurn = 0;
        }

    }

    // Stop playing
    outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
    outp( ZXUNO_REG, WAVUNO_REG_INT_CONTROL_BEGIN_REPROD );
    outp( ZXUNO_ADDR, WAVUNO_DATA_REG );
    outp( ZXUNO_REG, 0 );

    closeMediaFile();

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

void resetWavunoIntUserPointer() {

    outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
    outp( ZXUNO_REG, WAVUNO_REG_INT_SAMPLE_RESET );
    outp( ZXUNO_ADDR, WAVUNO_DATA_REG );
    outp( ZXUNO_REG, 0 );

}

void setWavunoIntBeginAndEndAddress( uint32_t beginAddress, uint32_t endAddress ) {

    // TODO remove "& 0xFF"

    outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
    outp( ZXUNO_REG, WAVUNO_REG_INT_START_LOOP1 );
    outp( ZXUNO_ADDR, WAVUNO_DATA_REG );
    outp( ZXUNO_REG, ( beginAddress >> 8 ) & 0x0FF );
    outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
    outp( ZXUNO_REG, WAVUNO_REG_INT_START_LOOP0 );
    outp( ZXUNO_ADDR, WAVUNO_DATA_REG );
    outp( ZXUNO_REG, beginAddress & 0x0FF );

    outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
    outp( ZXUNO_REG, WAVUNO_REG_INT_END_LOOP1 );
    outp( ZXUNO_ADDR, WAVUNO_DATA_REG );
    outp( ZXUNO_REG, ( endAddress >> 8 ) & 0x0FF );
    outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
    outp( ZXUNO_REG, WAVUNO_REG_INT_END_LOOP0 );
    outp( ZXUNO_ADDR, WAVUNO_DATA_REG );
    outp( ZXUNO_REG, endAddress & 0x0FF );

}

void setWavunoExtUserPointer( uint32_t value ) {

    // TODO remove "& 0xFF"

    outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
    outp( ZXUNO_REG, WAVUNO_REG_EXT_USER_POINTER2 );
    outp( ZXUNO_ADDR, WAVUNO_DATA_REG );
    outp( ZXUNO_REG, ( value >> 16 ) & 0x0FF );
    outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
    outp( ZXUNO_REG, WAVUNO_REG_EXT_USER_POINTER1 );
    outp( ZXUNO_ADDR, WAVUNO_DATA_REG );
    outp( ZXUNO_REG, ( value >> 8 ) & 0x0FF );
    outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
    outp( ZXUNO_REG, WAVUNO_REG_EXT_USER_POINTER0 );
    outp( ZXUNO_ADDR, WAVUNO_DATA_REG );
    outp( ZXUNO_REG, value & 0x0FF );

}

void setWavunoExtBeginAndEndAddress( uint32_t beginAddress, uint32_t endAddress ) {

    // TODO remove "& 0xFF"

    outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
    outp( ZXUNO_REG, WAVUNO_REG_EXT_START_LOOP2 );
    outp( ZXUNO_ADDR, WAVUNO_DATA_REG );
    outp( ZXUNO_REG, ( beginAddress >> 16 ) & 0x0FF );
    outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
    outp( ZXUNO_REG, WAVUNO_REG_EXT_START_LOOP1 );
    outp( ZXUNO_ADDR, WAVUNO_DATA_REG );
    outp( ZXUNO_REG, ( beginAddress >> 8 ) & 0x0FF );
    outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
    outp( ZXUNO_REG, WAVUNO_REG_EXT_START_LOOP0 );
    outp( ZXUNO_ADDR, WAVUNO_DATA_REG );
    outp( ZXUNO_REG, beginAddress & 0x0FF );

    outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
    outp( ZXUNO_REG, WAVUNO_REG_EXT_END_LOOP2 );
    outp( ZXUNO_ADDR, WAVUNO_DATA_REG );
    outp( ZXUNO_REG, ( endAddress >> 16 ) & 0x0FF );
    outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
    outp( ZXUNO_REG, WAVUNO_REG_EXT_END_LOOP1 );
    outp( ZXUNO_ADDR, WAVUNO_DATA_REG );
    outp( ZXUNO_REG, ( endAddress >> 8 ) & 0x0FF );
    outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
    outp( ZXUNO_REG, WAVUNO_REG_EXT_END_LOOP0 );
    outp( ZXUNO_ADDR, WAVUNO_DATA_REG );
    outp( ZXUNO_REG, endAddress & 0x0FF );

}
