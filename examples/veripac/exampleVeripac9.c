/*
 * @author yomboprime
 *
 * Example of use of Veripac9
*/
#include <stdio.h>
#include <input.h>
#include <stdlib.h>
#include <spectrum.h>
#include <sound.h>

#include "../../src/zxuno/zxuno.h"
#include "../../src/zxuno/turbo.h"
#include "../../src/zxuno/veripac9.h"
#include "../../src/esxdos.h"
#include "../../src/textUtils.h"
#include "../../src/fileDialog.h"
#include "../../src/graphics.h"
#include "veripacCharsetGraphics.h"

// Function prototypes
void refreshDisplay( bool onlyRegisters, bool drawDisplay );
uint8_t veripac9Color( uint8_t color );
uint16_t mapASCII2Veripac( uint16_t asciiKey );
void beep( uint8_t freq );
void drawAskForKeyboard( bool drawNoErase );
void printGraphicDisplay( uint8_t *memory );
void printDebug( uint8_t *memory, bool onlyRegisters );
void clearState();
bool loadFile();
uint16_t loadProgram();
void initTurbo();
void setTurbo();
void quitTurbo();

// Global variables
#define PATH_LENGTH 200
uint8_t filePath[ PATH_LENGTH ];
uint8_t dirPath[ PATH_LENGTH ];

#define STATE_STOP 0
#define STATE_RUNNING 1
uint8_t state = STATE_STOP;

#define DISPLAY_NORMAL 0
#define DISPLAY_DEBUG 1
uint8_t display = DISPLAY_NORMAL;

uint8_t memoryBuffer[ 256 ];

#define FILE_BUFFER_SIZE 512
uint8_t fileBuffer[ FILE_BUFFER_SIZE ];

uint32_t filePointer = 0;
uint32_t currentLine = 1;

bool flagResetFilePointer = false;
bool flagClearScreen = false;

#define PARSE_STATE_NIBBLE0 0
#define PARSE_STATE_NIBBLE1 1
#define PARSE_STATE_COMMENT 2

uint8_t previousTurbo;
uint8_t currentTurbo;

#define BEEP_DURATION_MS 0.5

void main(void) {

    uint8_t key;
    bool doStep;
    bool drawScreen;
    uint8_t controlReg;
    uint8_t instructionReg;
    uint8_t regB;
    uint16_t i;

    initTurbo();

    sprintf( filePath, "/" );

    //textUtils_32ColumnsMode();
    textUtils_64ColumnsMode();
    textUtils_setAttributes( PAPER_BLACK | INK_WHITE );
    textUtils_cls();
    zx_border( INK_BLUE );

    veripac9_readAllMemory( memoryBuffer );
    refreshDisplay( false, true );

    while ( 1 ) {

        //key = waitKeyPress();
        key = in_Inkey();

        doStep = state == STATE_RUNNING ? true : false;

        switch ( key ) {

            case 's':
            case 'S':
                state = STATE_STOP;
                doStep = doStep == true ? false : true;
                quitTurbo();
                while ( in_Inkey() > 0 ) {
                    // Wait key release
                }
                break;

            case 'r':
            case 'R':
                state = STATE_RUNNING;
                doStep = true;
                setTurbo();
                break;

            case 'd':
            case 'D':
                while ( in_Inkey() > 0 ) {
                    // Wait key release
                }
                if ( display == DISPLAY_NORMAL ) {

                    display = DISPLAY_DEBUG;

                    veripac9_readRegistersAndScreen( memoryBuffer );

                    state = STATE_STOP;
                    doStep = false;
                    quitTurbo();

                    textUtils_64ColumnsMode();

                }
                else {

                    display = DISPLAY_NORMAL;

                    textUtils_32ColumnsMode();

                }

                textUtils_cls();
                refreshDisplay( false, true );
                
                break;

            case 'l':
            case 'L':

                quitTurbo();

                if ( loadFile() == true ) {
                    veripac9_readAllMemory( memoryBuffer );
                }

                textUtils_64ColumnsMode();
                textUtils_setAttributes( PAPER_BLACK | INK_WHITE );
                textUtils_cls();
                zx_border( INK_BLUE );

                refreshDisplay( false, true );

                state = STATE_STOP;
                doStep = false;

                break;

            default:
                // Nothing to do here
                break;
        }

        if ( doStep == true ) {

            zx_border( INK_MAGENTA );

            controlReg = 0xFF;
            drawScreen = false;
            while ( ( controlReg & 0x03 ) != 0 ) {

                // Step
                veripac9_step();

                controlReg = veripac9_readMemory( VERIPAC_CONTROL_REG );

                switch ( controlReg ) {
                    case 0:
                        // Fetch 1
                        if ( display == DISPLAY_DEBUG ) {
                            veripac9_readRegistersAndScreen( memoryBuffer );
                        }
                        else if ( drawScreen ) {
                            veripac9_readScreen( memoryBuffer );
                        }
                        refreshDisplay( true, drawScreen );
                        break;
                    //case 1:
                        // Fetch 2
                        //break;
                    case 2:
                        // Execution
                        instructionReg = veripac9_readMemory( VERIPAC_INSTRUCTION_REG );
                        if ( instructionReg == 0x0B || ( instructionReg & 0xF0 ) == 0xC0 ) {
                            drawScreen = true;
                        }
                        else if ( ( veripac9_readMemory( VERIPAC_KEYBOARD_REG ) & 1 ) != 0 ) {
                            
                            // Veripac is asking for a keypress
                            drawAskForKeyboard( true );

                            key = 0xFF;
                            while ( key == 0xFF ) {
                                key = mapASCII2Veripac( waitKey() );
                                if ( key == 0 ) {
                                    // Small beep
                                    bit_frequency( 0.1, 400.0 );
                                }
                            }

                            while ( in_Inkey() > 0 ) {
                                // Wait key release
                            }

                            veripac9_writeMemory( VERIPAC_KEYBOARD_REG, key );

                            drawAskForKeyboard( false );

                        }
                        break;
                    case 3:
                        // Halt: restart and load next program in the file
                        veripac9_reset();
                        controlReg = 0;
                        if ( flagResetFilePointer == true ) {
                            if ( veripac9_readMemory( VERIPAC_ACCUMULATOR ) != 0 ) {
                                filePointer = 0;
                            }
                            flagResetFilePointer = false;
                        }
                        else if ( flagClearScreen == true ) {
                            for ( i = VERIPAC_SCREEN_START; i < VERIPAC_SCREEN_START + VERIPAC_SCREEN_LENGTH; i++ ) {
                                veripac9_writeMemory( (uint8_t)i, 0x0C );
                            }
                            flagClearScreen = false;
                        }
                        if ( loadProgram() > 0 ) {
                            veripac9_readAllMemory( memoryBuffer );
                        }
                        else {
                            state = STATE_STOP;
                            quitTurbo();
                        }
                        refreshDisplay( false, true );
                        break;
                    case 6:
                        // Execution, buzzer enabled
                        zx_border( INK_YELLOW );
                        regB = veripac9_readMemory( VERIPAC_REGS_START + 0x0B ) & 0xE0;
                        if ( regB != 0xE0 ) {
                            beep( regB );
                        }
                        zx_border( INK_MAGENTA );
                        break;
                }

            }

            zx_border( INK_BLUE );

        }

    }

}

void refreshDisplay( bool onlyRegisters, bool drawDisplay ) {

    if ( display == DISPLAY_NORMAL ) {
        if ( drawDisplay == true ) {
            printGraphicDisplay( memoryBuffer );
        }
    }
    else {
        printDebug( memoryBuffer, onlyRegisters );
    }

}

uint8_t veripac9Color( uint8_t color ) {

    switch ( color ) {
        case 0x00:
            return INK_GREEN;
        case 0x40:
            return INK_RED;
        case 0x80:
            return INK_BLUE;
        case 0xC0:
            return INK_YELLOW;
        default:
            return INK_WHITE;
    }

}

uint16_t mapASCII2Veripac( uint16_t asciiKey ) {

    if ( asciiKey >= '0' && asciiKey <= '9' ) {
        return asciiKey - '0';
    }

    switch ( asciiKey ) {

        case ':':
            return 0x0A;
        case '$':
            return 0x0B;
        case ' ':
            return 0x0C;
        case '?':
            return 0x0D;
        case 'l':
        case 'L':
            return 0x0E;
        case 'p':
        case 'P':
            return 0x0F;
        case '+':
            return 0x10;
        case 'w':
        case 'W':
            return 0x11;
        case 'e':
        case 'E':
            return 0x12;
        case 'r':
        case 'R':
            return 0x13;
        case 't':
        case 'T':
            return 0x14;
        case 'u':
        case 'U':
            return 0x15;
        case 'i':
        case 'I':
            return 0x16;
        case 'o':
        case 'O':
            return 0x17;
        case 'q':
        case 'Q':
            return 0x18;
        case 's':
        case 'S':
            return 0x19;
        case 'd':
        case 'D':
            return 0x1A;
        case 'f':
        case 'F':
            return 0x1B;
        case 'g':
        case 'G':
            return 0x1C;
        case 'h':
        case 'H':
            return 0x1D;
        case 'j':
        case 'J':
            return 0x1E;
        case 'k':
        case 'K':
            return 0x1F;
        case 'a':
        case 'A':
            return 0x20;
        case 'z':
        case 'Z':
            return 0x21;
        case 'x':
        case 'X':
            return 0x22;
        case 'c':
        case 'C':
            return 0x23;
        case 'v':
        case 'V':
            return 0x24;
        case 'b':
        case 'B':
            return 0x25;
        case 'm':
        case 'M':
            return 0x26;
        case '.':
            return 0x27;
        case '-':
            return 0x28;
        case '*':
            return 0x29;
        case '/':
            return 0x2A;
        case '=':
            return 0x2B;
        case 'y':
        case 'Y':
            return 0x2C;
        case 'n':
        case 'N':
            return 0x2D;

        default:
            return 0xFF;
    }

}

void beep( uint8_t freq ) {

    switch ( freq ) {
        case 0x00:
            bit_frequency( BEEP_DURATION_MS, 523.251 );
            break;
        case 0x20:
            bit_frequency( BEEP_DURATION_MS, 587.330 );
            break;
        case 0x40:
            bit_frequency( BEEP_DURATION_MS, 659.255 );
            break;
        case 0x60:
            bit_frequency( BEEP_DURATION_MS, 698.456 );
            break;
        case 0x80:
            bit_frequency( BEEP_DURATION_MS, 783.991 );
            break;
        case 0xA0:
            bit_frequency( BEEP_DURATION_MS, 880.000 );
            break;
        case 0xC0:
            bit_frequency( BEEP_DURATION_MS, 987.767 );
            break;
        default:
            bit_frequency( BEEP_DURATION_MS, 523.251 );
            break;
    }

}

void printGraphicDisplay( uint8_t *memory ) {

    uint16_t i, j, c, p;
    uint8_t attrs;

    // Print graphic screen
    p = 0;
    for ( j = 10; j < 14; j+= 2 ) {
        for ( i = 0; i < 32; i+= 2 ) {
            c = memory[ VERIPAC_SCREEN_START + p ];
            
            attrs = veripac9Color( c & 0xC0 );
            c &= 0x3F;

            paintGraphicBlockPositionReadOrder( i, j, 2, 2, veripacCharsetGraphics + ( c << 5 ) );

            textUtils_paintRectangleWithAttributes( i, i + 1, j, j + 1, attrs );

            p++;
        }
    }

}

void printDebug( uint8_t *memory, bool onlyRegisters ) {

    int v, i, j, controlReg;
    uint8_t str[3], p;

    textUtils_setAttributes( PAPER_BLACK | INK_WHITE );
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

void drawAskForKeyboard( bool drawNoErase ) {

    if ( display == DISPLAY_NORMAL ) {

        if ( drawNoErase == true ) {
            paintGraphicBlockPositionReadOrder( 4, 4, 2, 2, veripacCharsetGraphics + ( 0x0D << 5 ) );
            textUtils_paintRectangleWithAttributes( 4, 5, 4, 5, PAPER_BLACK | INK_YELLOW );
        }
        else {
            paintGraphicBlockPositionReadOrder( 4, 4, 2, 2, veripacCharsetGraphics + ( 0x0C << 5 ) );
            textUtils_paintRectangleWithAttributes( 4, 5, 4, 5, PAPER_BLACK | INK_YELLOW );
        }

    }
    else {

        textUtils_printAt( 16, 1 );

        if ( drawNoErase == true ) {

            textUtils_print( "?" );
            textUtils_paintRectangleWithAttributes( 8, 8, 1, 1, PAPER_BLACK | INK_YELLOW );
            
        }
        else {

            textUtils_print( " " );
            textUtils_paintRectangleWithAttributes( 8, 8, 1, 1, PAPER_BLACK | INK_YELLOW );

        }

    }

}

void clearState() {

    uint16_t i;

    // Clear all memory behind the program, resetting Veripac state
    for ( i = VERIPAC_RAM_LENGTH; i < VERIPAC_SCREEN_START; i++ ) {
        veripac9_writeMemory( (uint8_t)i, 0 );
    }

    // Clear screen
    for ( ; i < VERIPAC_REGS_START; i++ ) {
        veripac9_writeMemory( (uint8_t)i, 0x0C );
    }

    // Clear registers
    for ( ; i < 256; i++ ) {
        veripac9_writeMemory( (uint8_t)i, 0 );
    }

}

void initTurbo() {

    previousTurbo = TURBO_get();
    currentTurbo = previousTurbo;

}

void setTurbo() {

    if ( currentTurbo != ZXUNO_TURBO_X4 ) {
        
        TURBO_set( ZXUNO_TURBO_X4 );
        currentTurbo = ZXUNO_TURBO_X4;
        
    }

}

void quitTurbo() {

    if ( currentTurbo != previousTurbo ) {

        TURBO_set( previousTurbo );
        currentTurbo = previousTurbo;

    }

}

bool loadFile() {

    // Opens file dialog to select a file to load.

    bool selectedAFile;

    uint16_t i, programNumBytes = 0;

    textUtils_32ColumnsMode();
    textUtils_setAttributes( INK_BLUE | PAPER_WHITE );
    textUtils_cls();

    fileDialogpathUpOneDir( filePath );
    sprintf( dirPath, "%s", filePath );

    selectedAFile = openFileDialog( "Open program file", filePath, PATH_LENGTH, INK_BLUE | PAPER_WHITE, INK_WHITE | PAPER_CYAN );

    textUtils_64ColumnsMode();
    textUtils_setAttributes( PAPER_BLACK | INK_WHITE );
    textUtils_cls();

    if ( selectedAFile == true ) {

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

            clearState();

        }

        textUtils_printAt( 10, 7 );
        textUtils_print( "Press any key to continue..." );
        waitKeyPress();

        return true;

    }
    else {

        sprintf( filePath, "%s", dirPath );

        textUtils_printAt( 10, 7 );
        textUtils_print( "Didn't select a file. " );
        waitKeyPress();
    }

    return false;

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
                    if ( c == 10 ) {
                        parseState = PARSE_STATE_NIBBLE0;
                        currentLine++;
                    }
                }
                else {

                    if ( parseState == PARSE_STATE_NIBBLE0 ) {
                        if ( c == '#' ) {
                            // Comment
                            parseState = PARSE_STATE_COMMENT;
                            continue;
                        }
                        else if ( c == '!' ) {
                            // Program separation
                            doEnd = true;
                            continue;
                        }
                        else if ( c == '$' ) {
                            // End of file, restart from beggining of file
                            flagResetFilePointer = true;
                            doEnd = true;
                            continue;
                        }
                        else if ( c == 10 ) {
                            // new line
                            currentLine++;
                            continue;
                        }
                        else if ( c == 13 ) {
                            // Ignore line feed
                            continue;
                        }
                        else if ( c == 32 ) {
                            // Ignore space
                            continue;
                        }
                        else if ( c == 9 ) {
                            // Ignore tab
                            continue;
                        }
                        else if ( c == '@' ) {
                            // Program separation & Clear screen
                            flagClearScreen = true;
                            doEnd = true;
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

    if ( programNumBytes > 0 ) {

        // Load the memory in the veripac9 subprocessor
        for ( i = 0; i < programNumBytes; i++ ) {
            veripac9_writeMemory( (uint8_t)i, memoryBuffer[ i ] );
        }

    }

    // Clear all memory behind the program to the end of ram
    for ( ; i < VERIPAC_RAM_LENGTH; i++ ) {
        veripac9_writeMemory( (uint8_t)i, 0 );
    }

    return programNumBytes;
}
