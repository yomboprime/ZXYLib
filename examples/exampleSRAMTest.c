/*
 * @author yomboprime
 *
 * External SRAM Test
*/

#include <stdio.h>
#include <input.h>
#include <stdlib.h>
#include <spectrum.h>

#include <time.h>

#include "../src/zxuno/zxuno.h"
#include "../src/textUtils.h"

// Function prototypes

void setWavunoExtUserPointer( uint32_t value );
void setWavunoExtBeginAndEndAddress( uint32_t beginAddress, uint32_t endAddress );

// Global variables

#define SRAM_LENGTH 524288
//#define SRAM_LENGTH 2097152


#define WAVUNO_DATA_REG 250
#define WAVUNO_STAT_REG 251

#define WAVUNO_REG_EXT_USER_POINTER0 12
#define WAVUNO_REG_EXT_USER_POINTER1 13
#define WAVUNO_REG_EXT_USER_POINTER2 14

#define WAVUNO_REG_EXT_SAMPLE_WRITE 15
#define WAVUNO_REG_EXT_SAMPLE_READ 16

uint8_t c, c2;
bool errorFlag = false;
int32_t ci;
int32_t errors, matches, total, firstError, lastError;
uint16_t seed = 1234;

void main(void) {

    textUtils_32ColumnsMode();
    textUtils_cls();
    zx_border( INK_BLUE );


    //TURBO_set( ZXUNO_TURBO_X1 );

    while ( 1 ) {

        errors = 0;
        matches = 0;
        total = 0;
        firstError = SRAM_LENGTH;
        lastError = 0;
        errorFlag = false;

        textUtils_println( "Starting test, writing SRAM..." );
        textUtils_print( "(" );
        textUtils_print_l( SRAM_LENGTH );
        textUtils_println( " bytes)" );

        setWavunoExtUserPointer( 0 );

        // Write to sram
        outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
        outp( ZXUNO_REG, WAVUNO_REG_EXT_SAMPLE_WRITE );
        outp( ZXUNO_ADDR, WAVUNO_DATA_REG );

        srand( seed );

        for ( ci = 0; ci < SRAM_LENGTH; ci++ ) {

            //c = ci;
            c = (uint8_t)( rand() );

            // Write value to sram
            outp( ZXUNO_REG, c );

        }
        textUtils_println( "SRAM written. Verifying..." );


        setWavunoExtUserPointer( 0 );

        srand( seed );

        for ( ci = 0; ci < SRAM_LENGTH; ci++ ) {

            //c = ci;
            c = (uint8_t)( rand() );

            // Read from sram
            outp( ZXUNO_ADDR, WAVUNO_STAT_REG );
            outp( ZXUNO_REG, WAVUNO_REG_EXT_SAMPLE_READ );
            outp( ZXUNO_ADDR, WAVUNO_DATA_REG );
            outp( ZXUNO_REG, 0 );
            c2 = inp( ZXUNO_REG );

            if ( c != c2 ) {
                errors++;
                if ( errorFlag == false ) {
                    firstError = ci;
                    errorFlag = true;
                }
                lastError = ci;
            }
            else {
                matches++;
            }

            total++;

        }
        textUtils_println( "SRAM read. Results:" );
        textUtils_print( "Errors: " ); textUtils_println_l( errors );
        textUtils_print( "Matches: " ); textUtils_println_l( matches );
        textUtils_print( "Total: " ); textUtils_println_l( total );
        textUtils_print( "First error at: " );
        if ( errorFlag == true ) {
             textUtils_println_l( firstError );
        }
        else {
            textUtils_println( "(No errors)" );
        }
        textUtils_print( "Last error at: " );
        if ( errorFlag == true ) {
             textUtils_println_l( lastError );
        }
        else {
            textUtils_println( "(No errors)" );
        }

        textUtils_println( "\nPress a key to start again..." );

        waitKeyPress();

    }

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
