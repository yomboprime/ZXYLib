/*
 * @author yomboprime
 *
 * Media player
*/

#include <stdio.h>
#include <input.h>
#include <stdlib.h>
#include <string.h>
#include <spectrum.h>

#include <time.h>

#include "../../src/zxuno/zxuno.h"
#include "../../src/zxuno/wavunolite.h"
#include "../../src/zxuno/turbo.h"
#include "../../src/zxuno/radas.h"
#include "../../src/esxdos.h"
#include "../../src/textUtils.h"
#include "../../src/fileDialog.h"

// Function prototypes

void playWavFileContinuously( uint16_t bufferSize );

bool openMediaFile();
bool closeMediaFile();

bool readStringFromFile( uint8_t *string, uint8_t size );
bool skipBytesFile( uint16_t numBytes );
uint16_t readByteFile();


// Global variables

#define PATH_SIZE ( 200 )
uint8_t filePath[ PATH_SIZE ];

#define BUFFER_SIZE 882
uint8_t buffer[ BUFFER_SIZE ];

int16_t drive;
int16_t mediaFileHandle;
uint8_t *screenPtr;
uint8_t errorString[ 200 ];

uint32_t numBytesWAV;
uint32_t beginPos, endPos;
bool sampleIsStereo = false;
uint32_t sampleFrequency = 0;

void main(void) {

    sprintf( filePath, "/" );
    sprintf( errorString, "No error." );

    textUtils_32ColumnsMode();
    textUtils_cls();
    zx_border( INK_GREEN );

    beginPos = 0;

    TURBO_set( ZXUNO_TURBO_X1 );

    while ( 1 ) {

        fileDialogpathUpOneDir( filePath );
        while ( openFileDialog( "Open media file", filePath, PATH_SIZE, INK_GREEN | PAPER_WHITE, INK_WHITE | PAPER_GREEN ) == false ) {

            textUtils_cls();
            textUtils_println( "Didn't select a file." );
            textUtils_println( "Press any key..." );
            waitKeyPress();

        }

        TURBO_set( ZXUNO_TURBO_X4 );

        textUtils_cls();

        textUtils_print( "Selected file: " );
        textUtils_println( filePath );

        textUtils_println( "\nPlaying WAV file... " );
        playWavFileContinuously( BUFFER_SIZE );

        textUtils_print( "\nMessage: " );
        textUtils_println( errorString );
        textUtils_println( "Press any key..." );

        TURBO_set( ZXUNO_TURBO_X1 );

        waitKeyPress();

    }

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

    uint8_t key;

    uint16_t tempValue;

    uint32_t freqValue = 3500;

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

    sprintf( errorString, "Selected file is not WAV uncompressed unsigned 8 bit." );
    if ( readStringFromFile( "RIFF", 4 ) == false ) {
        closeMediaFile();
        return;
    }

    if ( skipBytesFile( 4 ) == false ) {
        closeMediaFile();
        return;
    }

    if ( readStringFromFile( "WAVEfmt ", 8 ) == false ) {
        closeMediaFile();
        return;
    }

    if ( readByteFile() != 16 ) {
        closeMediaFile();
        return;
    }

    if ( readByteFile() != 0 ) {
        closeMediaFile();
        return;
    }

    if ( readByteFile() != 0 ) {
        closeMediaFile();
        return;
    }

    if ( readByteFile() != 0 ) {
        closeMediaFile();
        return;
    }

    if ( readByteFile() != 1 ) {
        closeMediaFile();
        return;
    }

    if ( readByteFile() != 0 ) {
        closeMediaFile();
        return;
    }

    // Num channels
    sprintf( errorString, "Unsupported number of channels. Must be 1 or 2" );
    tempValue = readByteFile();
    if ( tempValue != 1 && tempValue != 2 ) {
        closeMediaFile();
        return;
    }

    if ( readByteFile() != 0 ) {
        closeMediaFile();
        return;
    }

    sampleIsStereo = tempValue == 2;

    if ( sampleIsStereo == true ) {
        sprintf( errorString, "Unsupported number of channels. Internal channel only suppports mono samples for now." );
        closeMediaFile();
        return;
    }

    sprintf( errorString, "Invalid sample frequency." );
    tempValue = readByteFile();
    if ( tempValue == 0xFFFF ) {
        closeMediaFile();
        return;
    }
    sampleFrequency = tempValue;

    tempValue = readByteFile();
    if ( tempValue == 0xFFFF ) {
        closeMediaFile();
        return;
    }
    sampleFrequency = sampleFrequency  | ( ((uint32_t)tempValue) << 8 );

    tempValue = readByteFile();
    if ( tempValue == 0xFFFF ) {
        closeMediaFile();
        return;
    }
    sampleFrequency = sampleFrequency  | ( ((uint32_t)tempValue) << 16 );

    tempValue = readByteFile();
    if ( tempValue == 0xFFFF ) {
        closeMediaFile();
        return;
    }
    sampleFrequency = sampleFrequency  | ( ((uint32_t)tempValue) << 24 );

    sprintf( errorString, "Selected file is not WAV uncompressed unsigned 8 bit." );
    if ( skipBytesFile( 6 ) == false ) {
        closeMediaFile();
        return;
    }

    if ( readByteFile() != 8 ) {
        closeMediaFile();
        return;
    }

    if ( readByteFile() != 0 ) {
        closeMediaFile();
        return;
    }

    if ( readStringFromFile( "data", 4 ) == false ) {
        closeMediaFile();
        return;
    }

    if ( skipBytesFile( 4 ) == false ) {
        closeMediaFile();
        return;
    }

    sprintf( errorString, "Finished playing WAV file." );

    // Set internal ram frequency.
    freqValue *= 1000;
    freqValue /= sampleFrequency;
    outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
    outp( ZXUNO_REG, WAVUNO_REG_INT_FREQ_DIVIDER0 );
    outp( ZXUNO_ADDR, WAVUNO_DATA_REG );
    outp( ZXUNO_REG, (uint8_t)freqValue );

    // Set looping and
    // TODO stereo
    outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
    outp( ZXUNO_REG, WAVUNO_REG_INT_CONTROL_FORMAT );
    outp( ZXUNO_ADDR, WAVUNO_DATA_REG );
    outp( ZXUNO_REG, 1 );

    setWavunoIntBeginEnd( 0, bufferSizeMinusOne );

    while ( doEnd == false && bytesLeftToLoad > 0 ) {

        key = in_Inkey();
        if ( key >0 ) {
            sprintf( errorString, "Interrupted by user." );
            doEnd = true;
            break;
        }
/*
        if ( key >= '1' && key <= '4' ) {
            // Play the wav file
            outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
            outp( ZXUNO_REG, WAVUNO_REG_EXT_CONTROL_BEGIN_REPROD );
            outp( ZXUNO_ADDR, WAVUNO_DATA_REG );
            //outp( ZXUNO_REG, ( 1 << ( key - '1' ) ) | inp( ZXUNO_REG ) );
            //outp( ZXUNO_REG, 2 );
            outp( ZXUNO_REG, 1 );

        }
*/
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
            setWavunoIntBeginEnd( bufferSize , twoBufferSizeMinusOne );
            bufferTurn = 1;
        }
        else {
            setWavunoIntBeginEnd( 0, bufferSizeMinusOne );
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

bool readStringFromFile( uint8_t *string, uint8_t size ) {

    uint16_t numBytesRead;

    numBytesRead = ESXDOS_fread( buffer, size, mediaFileHandle );

    iferror {
        return false;
    }

    if ( numBytesRead < size ) {
        return false;
    }

    buffer[ size ] = 0;

    return strcmp( string, buffer ) == 0 ? true : false;

}

bool skipBytesFile( uint16_t numBytes ) {
    ESXDOS_fseek( numBytes, ESXDOS_SEEK_FORWARD_FROM_CURRENT, mediaFileHandle );
    iferror {
        return false;
    }
    return true;
}

uint16_t readByteFile() {

    // Returns byte read or 0xFFFF if error

    uint16_t numBytesRead;

    numBytesRead = ESXDOS_fread( buffer, 1, mediaFileHandle );

    iferror {
        return 0xFFFF;
    }

    if ( numBytesRead < 1 ) {
        return 0xFFFF;
    }

    return buffer[ 0 ];

}
