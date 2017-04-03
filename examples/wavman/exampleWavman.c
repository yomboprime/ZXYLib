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
#include <sound.h>

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

void resetKeyboard();
unsigned char readJoystickAndKeyboard();

#define playBeep() bit_frequency( 0.05, 130.812 )

uint16_t countSongs();
void getSongFileName();

void callBackDummy( uint16_t numEntry, uint8_t *entryPtr, uint8_t *userData );
void callBackGetFileName( uint16_t numEntry, uint8_t *entryPtr, uint8_t *userData );

uint16_t getRandomSongIndex();

// Global variables

#define PATH_SIZE ( 200 )
uint8_t filePath[ PATH_SIZE ];

#define BUFFER_SIZE 882
uint8_t buffer[ BUFFER_SIZE ];

int16_t drive;
int16_t mediaFileHandle;
uint8_t errorString[ 200 ];

bool sampleIsStereo = false;
uint32_t sampleFrequency = 0;

struct in_UDK keyboardMap;
uint8_t joystickValue;
uint8_t joystickValuePrev;



// Play variables

uint32_t bytesLeftToLoad;
uint16_t numBytesToRead, numBytesRead;
uint8_t *p;
bool doEnd;
bool finishedPlaying;
uint32_t bufferSizeMinusOne;
uint32_t twoBufferSizeMinusOne;
uint8_t bufferTurn;
bool flagStart;
uint8_t key;
uint16_t tempValue;
uint32_t freqValue;

uint8_t pressedCounter;
bool pressedLeft;
bool pressedRight;
bool pressedUp;
bool pressedFire;

// Play state

#define STATE_PAUSE 0
#define STATE_PLAYING 1
#define STATE_FASTFORWARD 2
#define STATE_FASTBACKWARD 3
unsigned char playingState = STATE_PAUSE;

#define SONG_LOOPALL 0
#define SONG_LOOPONE 1
#define SONG_RAND 2
unsigned char songState = SONG_LOOPALL;

uint16_t numTotalSongs = 0;
uint16_t currentSongIndex = 0;


void main(void) {
    
    resetKeyboard();
    
    sprintf( filePath, "/music" );
    sprintf( errorString, "No error." );

    textUtils_32ColumnsMode();
    textUtils_cls();
    zx_border( INK_GREEN );

    TURBO_set( ZXUNO_TURBO_X1 );

    numTotalSongs = countSongs();
    textUtils_print( "Number of songs: " );
    textUtils_println_l( numTotalSongs );
//    waitKeyPress();
    
    playBeep();

    playingState = STATE_PAUSE;
    joystickValuePrev = 0;
    pressedCounter = 0;
    pressedLeft = false;
    pressedRight = false;
    pressedUp = false;
    pressedFire = false;

    while ( 1 ) {

        TURBO_set( ZXUNO_TURBO_X4 );

        if ( ! getSongFileName() ) {
            textUtils_print( "Error getting song name." );
            waitKeyPress();
            continue;
        }
        
        textUtils_print( "Selected file: " );
        textUtils_print_l( currentSongIndex );
        textUtils_print( ", " );
        textUtils_println( filePath );

        textUtils_println( "\nPlaying WAV file... " );

        playWavFileContinuously( BUFFER_SIZE );

        textUtils_print( "\nMessage: " );
        textUtils_println( errorString );
        textUtils_println( "Press any key..." );
        
        if ( finishedPlaying ) {
            switch ( songState ) {
                case SONG_LOOPALL:
                    currentSongIndex++;
                    if ( currentSongIndex >= numTotalSongs ) {
                        currentSongIndex = 0;
                    }
                    break;
                case SONG_RAND:
                    currentSongIndex = getRandomSongIndex();
                    break;
            }
        }

        TURBO_set( ZXUNO_TURBO_X1 );

//        waitKeyPress();

    }

}

void playWavFileContinuously( uint16_t bufferSize ) {

    // Plays a PCM 8 bit sample using ping pong buffers in wavuno internal ram
    // Returns true if the song finished playing, false if terminated for another cause

    doEnd = false;
    bufferSizeMinusOne = bufferSize - 1;
    twoBufferSizeMinusOne = ( bufferSize << 1 ) - 1;
    bufferTurn = 0;
    flagStart = true;

    freqValue = 3500;
    
    finishedPlaying = true;

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

        // Keyboard input
        
        joystickValue = readJoystickAndKeyboard();
        
        if ( joystickValuePrev == 0 && joystickValue != 0 ) {
            playBeep();
        }
        
        if ( joystickValue & in_LEFT ) {
            pressedLeft = true;
            if ( joystickValuePrev != 0 ) {
                if ( pressedCounter > 10 ) {
                    playingState = STATE_FASTBACKWARD;
                    pressedCounter = 11;
                }
                else {
                    pressedCounter++;
                }
            }
            else {
                pressedCounter = 0;
                playingState = STATE_PLAYING;
            }
        }
        else if ( joystickValue & in_RIGHT ) {
            pressedRight = true;
            if ( joystickValuePrev != 0 ) {
                if ( pressedCounter > 10 ) {
                    playingState = STATE_FASTFORWARD;
                    pressedCounter = 11;
                }
                else {
                    pressedCounter++;
                }
            }
            else {
                pressedCounter = 0;
                playingState = STATE_PLAYING;
            }
        }
        else {
            if ( pressedLeft ) {
                if ( pressedCounter <= 10 ) {
                    sprintf( errorString, "Prev" );
                    if ( currentSongIndex > 0 ) {
                        currentSongIndex--;
                    }
                    else {
                        currentSongIndex = numTotalSongs - 1;
                    }
                    doEnd = true;
                    finishedPlaying = false;
                }
                else {
                    playingState = STATE_PLAYING;
                }
            }
            else if ( pressedRight ) {
                if ( pressedCounter <= 10 ) {
                    sprintf( errorString, "Next" );
                    currentSongIndex++;
                    if ( currentSongIndex >= numTotalSongs ) {
                        currentSongIndex = 0;
                    }
                    doEnd = true;
                    finishedPlaying = false;
                }
                else {
                    playingState = STATE_PLAYING;
                }
            }
            pressedLeft = false;
            pressedRight = false;
        }
        
        if ( joystickValue & in_UP ) {
            pressedUp = true;
            if ( joystickValuePrev != 0 ) {
                if ( pressedCounter > 10 ) {
                    playingState = STATE_PLAYING;
                    songState = SONG_LOOPALL;
                    currentSongIndex = 0;
                    doEnd = true;
                    finishedPlaying = false;
                    pressedCounter = 11;
                }
                else {
                    pressedCounter++;
                }
            }
            else {
                pressedCounter = 0;
                playingState = STATE_PLAYING;
            }
        }
        else {
            if ( pressedUp ) {
                if ( pressedCounter <= 10 ) {
                    playingState = STATE_PLAYING;
                    songState = SONG_RAND;
                    currentSongIndex = getRandomSongIndex();
                    doEnd = true;
                    finishedPlaying = false;
                }
            }
            pressedUp = false;
        }

        if ( ( joystickValue & in_DOWN ) && ( joystickValuePrev == 0 ) ) {
            if ( playingState == STATE_PAUSE ) {
                playingState = STATE_PLAYING;
                flagStart = true;
                bufferTurn = 0;
            }
            else {
                playingState = STATE_PAUSE;
                // Stop playing
                outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
                outp( ZXUNO_REG, WAVUNO_REG_INT_CONTROL_BEGIN_REPROD );
                outp( ZXUNO_ADDR, WAVUNO_DATA_REG );
                outp( ZXUNO_REG, 0 );
            }
            srand( time(NULL) );
        }
        
        if ( joystickValue & in_FIRE ) {
            pressedFire = true;
            if ( joystickValuePrev != 0 ) {
                if ( pressedCounter > 10 ) {
                    playingState = STATE_PLAYING;
                    songState = SONG_LOOPALL;
                    pressedCounter = 11;
                }
                else {
                    pressedCounter++;
                }
            }
            else {
                pressedCounter = 0;
                playingState = STATE_PLAYING;
            }
        }
        else {
            if ( pressedFire ) {
                if ( pressedCounter <= 10 ) {
                    playingState = STATE_PLAYING;
                    songState = SONG_LOOPONE;
                }
            }
            pressedFire = false;
        }

        joystickValuePrev = joystickValue;
        
        switch ( playingState ) {
            case STATE_PAUSE:
                continue;
                
            case STATE_FASTFORWARD:
                numBytesToRead = bufferSize;
                if ( numBytesToRead > bytesLeftToLoad ) {
                    numBytesToRead = bytesLeftToLoad;
                }
                ESXDOS_fseek( numBytesToRead, ESXDOS_SEEK_FORWARD_FROM_CURRENT, mediaFileHandle );
                bytesLeftToLoad -= numBytesRead;
                break;
                
            case STATE_FASTBACKWARD:
                numBytesToRead = bufferSize << 2;
                if ( numBytesToRead > bytesLeftToLoad ) {
                    numBytesToRead = bytesLeftToLoad;
                }
                if ( ESXDOS_fseek( numBytesToRead, ESXDOS_SEEK_BACKWARDS_FROM_CURRENT, mediaFileHandle ) == 0 ) {
                    bytesLeftToLoad = ESXDOS_fsize( mediaFileHandle ) - 44;
                }
                else {
                    bytesLeftToLoad += numBytesRead;
                }
                break;
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

void resetKeyboard() {
    keyboardMap.fire = in_LookupKey( ' ' );
    keyboardMap.left = in_LookupKey( 'o' );
    keyboardMap.right = in_LookupKey( 'p' );
    keyboardMap.down = in_LookupKey( 'a' );
    keyboardMap.up = in_LookupKey( 'q' );
}

unsigned char readJoystickAndKeyboard() {
    return ((void*)in_JoyKeyboard)( & keyboardMap ) | ((void*)in_JoyKempston)( & keyboardMap );
}

uint16_t countSongs() {
    
    // (-2 for ./ and ../)
    return fileDialogIterateSDDirectory( "/music", 0, 65535, callBackDummy, NULL, NULL, NULL ) - 2;

}

void getSongFileName() {

    bool found = false;
    
    fileDialogIterateSDDirectory( "/music", 0, 65535, callBackGetFileName, NULL, NULL, &found );
    
    return found;

}

void callBackDummy( uint16_t numEntry, uint8_t *entryPtr, uint8_t *userData ) {
    
    // Nothing to do here
    
}

void callBackGetFileName( uint16_t numEntry, uint8_t *entryPtr, uint8_t *userData ) {
    
    if ( numEntry == ( currentSongIndex + 2 ) && ( *entryPtr++ != ESXDOS_FILE_ATTRIBUTE_DIR_BIT ) ) {

        strcpy( filePath, "/music/" );
        p = filePath + 7;
        while ( *entryPtr ) {
            *p++ = *entryPtr++;
        }
        *p = *entryPtr;

        *((bool*)(userData)) = true;
        
    }
    
}

uint16_t getRandomSongIndex() {
    
    return ((uint16_t)( ( numTotalSongs - 1 ) * ( ((float)rand()) / 32767.0 ) ) );
    
}
