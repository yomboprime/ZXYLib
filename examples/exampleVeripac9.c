/*
 * @author yomboprime
 *
 * Example of use of Veripac9
*/
#include <stdio.h>
#include <input.h>
#include <stdlib.h>
#include <spectrum.h>

#include "../src/zxuno/zxuno.h"
#include "../src/zxuno/veripac9.h"
#include "../src/esxdos.h"
#include "../src/textUtils.h"
#include "../src/fileDialog.h"

// Function prototypes
void refreshDisplay();
void printMemory( uint8_t *memory );
void loadFile();
uint16_t loadProgram();

// Global variables
#define PATH_LENGTH 200
uint8_t filePath[ PATH_LENGTH ];
uint8_t dirPath[ PATH_LENGTH ];

#define STATE_STOP 0
#define STATE_RUNNING 1
uint8_t state = STATE_RUNNING;

#define DISPLAY_NORMAL 0
#define DISPLAY_DEBUG 1
uint8_t display = DISPLAY_DEBUG;

uint8_t memoryBuffer[ 256 ];

#define FILE_BUFFER_SIZE 512
uint8_t fileBuffer[ FILE_BUFFER_SIZE ];

uint32_t filePointer = 0;
uint32_t currentLine = 1;

#define PARSE_STATE_NIBBLE0 0
#define PARSE_STATE_NIBBLE1 1
#define PARSE_STATE_COMMENT 2

void main(void) {

    uint16_t i;
    uint8_t key;
    uint8_t *ptr;

    sprintf( dirPath, "/" );

    textUtils_64ColumnsMode();
    textUtils_cls();

/*
    for ( i = 0; i < 256; i++ ) {
        if ( i != VERIPAC_CONTROL_REG ) {
            veripac9_writeMemory( (uint8_t)i, i != VERIPAC_PROGRAM_COUNTER ? (uint8_t)i : 85 );
        }
    }
*/

    refreshDisplay();

    while ( 1 ) {

        key = waitKeyPress();

        switch ( key ) {
            case 'l':
            case 'L':
                loadFile();
                break;
        }

        if ( state == STATE_RUNNING ) {

            // Step
            veripac9_writeMemory( 0xCA, 1 );
            veripac9_writeMemory( 0xCA, 0 );

            refreshDisplay();

        }

    }

    textUtils_println( "End. Press a key." );
    waitKeyPress();
}

void refreshDisplay() {

    if ( display == DISPLAY_DEBUG ) {
        veripac9_readAllMemory( memoryBuffer );
        printMemory( memoryBuffer );
    }

}

void printMemory( uint8_t *memory ) {

    int v, i, j;
    uint8_t str[3], p;

    textUtils_setAttributes( PAPER_WHITE | INK_BLACK );

    textUtils_cls();

    // Program Counter
    v = memory[ VERIPAC_PROGRAM_COUNTER ];
    sprintf( str, "%02x", v );
    textUtils_print( "PC: " ); textUtils_print( str );
    
    // Control Unit State
    v = memory[ VERIPAC_CONTROL_REG ];
    sprintf( str, "%02x", v );
    textUtils_print( "  UC: " ); textUtils_print( str );

    // Instruction Register
    v = memory[ VERIPAC_INSTRUCTION_REG ];
    sprintf( str, "%02x", v );
    textUtils_print( "  IC: " ); textUtils_print( str );

    // Data Counter
    v = memory[ VERIPAC_DATA_COUNTER ];
    sprintf( str, "%02x", v );
    textUtils_print( "  DC: " ); textUtils_println( str );

    // Accumulator
    v = memory[ VERIPAC_ACCUMULATOR ];
    sprintf( str, "%02x", v );
    textUtils_print( "AC: " ); textUtils_print( str );

    // Keyboard register
    v = memory[ VERIPAC_KEYBOARD_REG ];
    sprintf( str, "%02x", v );
    textUtils_print( "  KB: " ); textUtils_println( str );

    // Print registers
    textUtils_println( "Registers:" );
    textUtils_println( " 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F" );
    for ( i = 0; i < VERIPAC_NUM_REGS; i++ ) {
        v = memory[ VERIPAC_REGS_START + i ];
        sprintf( str, "%02x", v );
        textUtils_print( str ); textUtils_print( "  " );
    }

    // Print screen
    textUtils_println( "Screen:" );
    textUtils_println( " 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F" );
    p = 0;
    for ( j = 0; j < VERIPAC_NUM_LINES; j++ ) {
        for ( i = 0; i < VERIPAC_LINE_LENGTH; i++ ) {
            v = memory[ VERIPAC_SCREEN_START + p++ ];
            sprintf( str, "%02x", v );
            textUtils_print( str ); textUtils_print( "  " );
        }
    }

    // Print memory
    textUtils_println( "Memory:" );
    textUtils_println( " 0   2   4   6   8   A   C   E   0   2   4   6   8   A   C   E" );
    for ( i = 0; i < VERIPAC_RAM_LENGTH; i++ ) {
        v = memory[ i ];
        sprintf( str, "%02x", v );
        textUtils_print( str ); textUtils_print( "  " );
    }

}

void loadFile() {

    // Opens file dialog to select a file to load.

    uint8_t v;

    uint16_t i, programNumBytes = 0;

    state = STATE_STOP;

    textUtils_32ColumnsMode();
    textUtils_cls();

    sprintf( filePath, "%s", dirPath );
    v = openFileDialog( "Open program file", filePath, PATH_LENGTH, INK_BLUE | PAPER_WHITE, INK_WHITE | PAPER_CYAN );

    textUtils_64ColumnsMode();
    textUtils_setAttributes( PAPER_WHITE | INK_BLACK );
    textUtils_cls();

    if ( v == true ) {
        // Load the file
        textUtils_printAt64( 10, 10 );
        textUtils_print( "Selected file: " );
        textUtils_print( filePath );
        textUtils_printAt64( 10, 11 );
        textUtils_print( "Loading the file... " );
        textUtils_printAt64( 10, 12 );
        filePointer = 0;
        currentLine = 1;
        
        programNumBytes = loadProgram();
        
        if ( programNumBytes > 0 ) {

            textUtils_print( "\n  Loaded program of " );
            textUtils_print_l( programNumBytes );
            textUtils_println( " bytes." );

            // Loads the memory in the veripac9 subprocessor
            for ( i = 0; i < programNumBytes; i++ ) {
                veripac9_writeMemory( (uint8_t)i, memoryBuffer[ i ] );
            }

        }

        // Clear all memory behind the program
        for ( i = programNumBytes; i < VERIPAC_RAM_LENGTH; i++ ) {
            veripac9_writeMemory( (uint8_t)i, 0 );
        }

        textUtils_print( "\n  Press any key to continue..." );
        waitKeyPress();

        refreshDisplay();
    }
    else {
        textUtils_printAt64( 10, 10 );
        textUtils_print( "Didn't select a file. " );
        waitKeyPress();
    }

}

uint16_t loadProgram() {

    // Loads the next program in the current file at current position.
    // It is loaded in the memoryBuffer array.
    // Returns number of bytes of the loaded program, 0 if error or last program.

    int16_t drive;

    int16_t fileHandle;

    uint16_t i, numBytes;
    uint16_t programNumBytes = 0;

    uint8_t parseState = PARSE_STATE_NIBBLE0;

    uint8_t c;
    uint8_t nibble, nibble0;

    bool doEnd = false;

    drive = ESXDOS_getDefaultDrive();

    iferror {
        textUtils_print( "SD card not inserted." );
        return 0;
    }

    fileHandle = ESXDOS_fopen( filePath, ESXDOS_FILEMODE_READ, drive );
    iferror {
        textUtils_print( "Could not open file." );
        textUtils_print( "\n  Error code= " );
        textUtils_println_l( fileHandle );
        return 0;
    }

    ESXDOS_fseek( filePointer, ESXDOS_SEEK_FROM_START, fileHandle );

    while ( doEnd == false ) {

        numBytes = ESXDOS_fread( fileBuffer, FILE_BUFFER_SIZE, fileHandle );

        iferror {
            textUtils_println( "Error reading the file." );
            programNumBytes = 0;
            doEnd = true;
            break;
        }

        if ( numBytes == 0 ) {
            // Reached end of file
            if ( parseState == PARSE_STATE_NIBBLE1 ) {
                textUtils_println( "Unexpected end of file in line " );
                textUtils_println_l( currentLine );
                programNumBytes = 0;
            }
            doEnd = true;
            break;
        }
        else {

            for ( i = 0; i < numBytes && doEnd == false; i++ ) {

                c = fileBuffer[ i ];
                filePointer++;

                if ( parseState == PARSE_STATE_COMMENT ) {
                    if ( c == 13 ) {
                        parseState = PARSE_STATE_NIBBLE0;
                    }
                }
                else {
                    switch ( c ) {
                        case '#':
                            parseState = PARSE_STATE_COMMENT;
                            break;
                        case '!':
                            doEnd = true;
                            break;
                        case '$':
                            filePointer = 0;
                            doEnd = true;
                            break;
                        case 13:
                            currentLine++;
                            break;
                        case 10:
                            break;
                        default:
                            if ( c >= '0' && c <= '9' ) {
                                nibble = c - '0';
                            }
                            else if ( c >= 'a' && c <= 'f' ) {
                                nibble = c - 'a';
                            }
                            else if ( c >= 'A' && c <= 'F' ) {
                                nibble = c - 'A';
                            }
                            else {
                                textUtils_print( "Syntax error in line " );
                                textUtils_println_l( currentLine );
                                programNumBytes = 0;
                                doEnd = true;
                                break;
                            }
                            if ( parseState == PARSE_STATE_NIBBLE0 ) {
                                nibble0 = ( nibble & 0x0F ) << 4;
                                parseState = PARSE_STATE_NIBBLE1;
                            }
                            else {
                                nibble0 = nibble0 | ( nibble & 0x0F );
                                if ( programNumBytes < VERIPAC_RAM_LENGTH ) {
                                    memoryBuffer[ programNumBytes++ ] = nibble0;
                                }
                                parseState = PARSE_STATE_NIBBLE0;
                            }
                            break;
                    }
                }

            }
        }

    }

    ESXDOS_fclose( fileHandle );
    iferror {
        textUtils_println( "Error closing the file." );
        programNumBytes = 0;
    }

    return programNumBytes;
}
