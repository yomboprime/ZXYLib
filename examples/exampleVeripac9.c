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
void refreshDisplay( bool onlyRegisters );
void printMemory( uint8_t *memory, bool onlyRegisters );
void loadFile();
uint16_t loadProgram();

// Global variables
#define PATH_LENGTH 200
uint8_t filePath[ PATH_LENGTH ];
uint8_t dirPath[ PATH_LENGTH ];

#define STATE_STOP 0
#define STATE_RUNNING 1
uint8_t state = STATE_STOP;

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
    bool doStep;
    uint8_t controlReg;
    uint8_t *ptr;

    sprintf( dirPath, "/" );

    textUtils_64ColumnsMode();
    textUtils_cls();

    zx_border( INK_BLUE );

    veripac9_readAllMemory( memoryBuffer );
    refreshDisplay( false );

    doStep = false;

    while ( 1 ) {

        //key = waitKeyPress();
        key = in_Inkey();

        switch ( key ) {
            case 'l':
            case 'L':
                loadFile();
                break;

            case 's':
            case 'S':
                state = STATE_STOP;
                doStep = true;
                break;

            case 'r':
            case 'R':
                state = STATE_RUNNING;
                doStep = true;
                break;

            default:
                if ( state == STATE_RUNNING ) {

                    doStep = true;

                }
                break;
        }

        if ( doStep == true ) {

            zx_border( INK_MAGENTA );

            controlReg = 0xFF;
            while ( ( controlReg & 0x03 ) != 0 ) {

                // Step
                veripac9_writeMemory( 0xCA, 1 );
                veripac9_writeMemory( 0xCA, 0 );

                veripac9_readAllMemory( memoryBuffer );
                controlReg = memoryBuffer[ VERIPAC_CONTROL_REG ];

                switch ( controlReg & 0x03 ) {
                    case 0:
                        // Fetch 1
                        refreshDisplay( true );
                        break;
                    //case 1:
                        // Fetch 2
                        //break;
                    case 2:
                        // Execution
                        if ( controlReg & 8 ) {
                            // Buzzer
                            zx_border( INK_YELLOW );
                            delay( 500 );
                            zx_border( INK_MAGENTA );
                        }
                        break;
                    case 3:
                        // Halt
                        break;
                }

            }

            zx_border( INK_BLUE );

            doStep = false;

        }

    }

}

void refreshDisplay( bool onlyRegisters ) {

    if ( display == DISPLAY_DEBUG ) {
        printMemory( memoryBuffer, onlyRegisters );
    }

}

void printMemory( uint8_t *memory, bool onlyRegisters ) {

    int v, i, j, controlReg;
    uint8_t str[3], p;

    textUtils_setAttributes( PAPER_WHITE | INK_BLACK );
    textUtils_printAt( 0, 0 );

    
    // Program Counter
    v = memory[ VERIPAC_PROGRAM_COUNTER ];
    sprintf( str, "%02x", v );
    textUtils_print( "PC: " ); textUtils_print( str );
    
    // Control Unit State
    controlReg = memory[ VERIPAC_CONTROL_REG ];
    sprintf( str, "%02x", controlReg );
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
    if ( onlyRegisters == false || controlReg == 0 ) {
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
    }

    if ( onlyRegisters == false ) {
        // Print memory
        textUtils_println( "Memory:" );
        textUtils_println( " 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F" );
        for ( i = 0; i < VERIPAC_RAM_LENGTH; i++ ) {
            v = memory[ i ];
            sprintf( str, "%02x", v );
            textUtils_print( str ); textUtils_print( "  " );
        }
    }

}

void loadFile() {

    // Opens file dialog to select a file to load.

    uint8_t v;

    uint16_t i, programNumBytes = 0;

//    state = STATE_STOP;

    textUtils_32ColumnsMode();
    textUtils_cls();

    sprintf( filePath, "%s", dirPath );
    v = openFileDialog( "Open program file", filePath, PATH_LENGTH, INK_BLUE | PAPER_WHITE, INK_WHITE | PAPER_CYAN );

    textUtils_64ColumnsMode();
    textUtils_setAttributes( PAPER_WHITE | INK_BLACK );
    textUtils_cls();

    if ( v == true ) {
        textUtils_printAt( 10, 1 );
        textUtils_print( "Selected file: " );
        textUtils_print( filePath );
        textUtils_printAt( 10, 3 );
        textUtils_print( "Loading the file... " );

        filePointer = 0;
        currentLine = 1;
        
        programNumBytes = loadProgram();
        
        if ( programNumBytes > 0 ) {

            textUtils_printAt( 10, 5 );
            textUtils_print( "Loaded program of " );
            textUtils_print_l( programNumBytes );
            textUtils_print( " bytes." );

            // Loads the memory in the veripac9 subprocessor
            for ( i = 0; i < programNumBytes; i++ ) {
                veripac9_writeMemory( (uint8_t)i, memoryBuffer[ i ] );
            }

        }

        // Clear all memory behind the program
        for ( i = programNumBytes; i < VERIPAC_RAM_LENGTH; i++ ) {
            veripac9_writeMemory( (uint8_t)i, 0 );
        }

        textUtils_printAt( 10, 7 );
        textUtils_print( "Press any key to continue..." );
        waitKeyPress();

        veripac9_readAllMemory( memoryBuffer );
        refreshDisplay( false );
    }
    else {
        textUtils_printAt( 10, 7 );
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

                    if ( parseState == PARSE_STATE_NIBBLE0 ) {
                        if ( c == '#' ) {
                            parseState = PARSE_STATE_COMMENT;
                            continue;
                        }
                        else if ( c == '!' ) {
                            doEnd = true;
                            continue;
                        }
                        else if ( c == '$' ) {
                            filePointer = 0;
                            doEnd = true;
                            continue;
                        }
                        else if ( c == 13 ) {
                            currentLine++;
                            continue;
                        }
                        else if ( c == 10 ) {
                            continue;
                        }
                    }

                    if ( c >= '0' && c <= '9' ) {
                        nibble = c - '0';
                    }
                    else if ( c >= 'a' && c <= 'f' ) {
                        nibble = c - 'a' + 0x0A;
                    }
                    else if ( c >= 'A' && c <= 'F' ) {
                        nibble = c - 'A' + 0x0A;
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
