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
#include "../src/textUtils.h"
#include "../src/fileDialog.h"

// Function prototypes
void refreshDisplay();
void printMemory( uint8_t *memory );

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

uint8_t buffer[256];

void main(void) {

    uint16_t i;
    uint8_t key;
    uint8_t *ptr;
    uint8_t v;

    sprintf( dirPath, "/zxart" );

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
                textUtils_32ColumnsMode();
                textUtils_cls();
                sprintf( filePath, "%s", dirPath );
                v = openFileDialog( "Open program file", filePath, PATH_LENGTH, INK_BLACK | PAPER_WHITE, INK_BLACK | PAPER_WHITE, INK_BLACK | PAPER_WHITE | BRIGHT );
                textUtils_64ColumnsMode();
                textUtils_cls();
                if ( v == true ) {
                    // Load the file
                    textUtils_printAt64( 10, 10 );
                    textUtils_print( "Selected file: " );
                    textUtils_print( filePath );
                }
                else {
                    textUtils_printAt64( 10, 10 );
                    textUtils_print( "Didn't select a file. " );
                }
                waitKeyPress();
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
        veripac9_readAllMemory( buffer );
        printMemory( buffer );
    }

}

void printMemory( uint8_t *memory ) {

    int v, i, j;
    uint8_t str[3], p;

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
