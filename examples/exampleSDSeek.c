/*
 * @author yomboprime
 *
 * Test write to SD card file, then seek file
*/

#include <input.h>
#include <stdlib.h>
#include <string.h>

#include "../src/textUtils.h"
#include "../src/esxdos.h"

#define getKey() in_Inkey()

#define BUFFER_SIZE ( 1024 )
uint8_t buffer[ BUFFER_SIZE ];


void main(void) {

    int16_t drive;

    int16_t f;

    uint8_t *filePath = "/rndtest.txt";

    uint8_t *pbuffer;

    uint16_t numBytes;

    uint16_t i, j;
    uint8_t c;

    uint32_t pos1, pos2;

    bool someError = false;


    textUtils_64ColumnsMode();
    textUtils_cls();

    textUtils_println( "This example writes a file in SD card and perform seek tests.\nThis will overwrite the file /rndtest.txt if it exists.\nPress a key to proceed." );

    waitKey();

    drive = ESXDOS_getDefaultDrive();

    iferror {
        textUtils_println( "SD card not inserted." );
        return;
    }

    textUtils_println( "SD card detected." );

    textUtils_print( "Drive: " );
    textUtils_println_l( drive );

    textUtils_print( "Opening file for writing: " );
    textUtils_println( filePath );

    f = ESXDOS_fopen( filePath, ESXDOS_FILEMODE_WRITE_CREATE, drive );
    iferror {
        textUtils_print( "Could not open file for writing." );
        textUtils_print( "Error code= " );
        textUtils_println_l( f );
        return;
    }


    for ( i = 0; i < 10; i++ ) {

        c = '0' + i;

        pbuffer = buffer;
        for ( j = 0; j < 100; j++ ) {
            *pbuffer++ = c;
        }
        numBytes = ESXDOS_fwrite( buffer, 100, f );
        iferror {
            textUtils_println( "Error writing to the file." );
            j = 100;
            i = 10;
        }
    }

    textUtils_println( "File written. " );

    textUtils_println( "Waiting 5 seconds..." );
    delay( 5000 );

    textUtils_println( "Starting random file access tests." );

    ESXDOS_fsync( f );
    iferror {
        textUtils_println( "Error doing sync to the file." );
        return;
    }
    textUtils_println( "File sync'ed OK" );

    ESXDOS_fclose( f );
    iferror {
        textUtils_println( "Error closing the file." );
        return;
    }
    textUtils_println( "File closed OK" );

    //-----

    textUtils_print( "Opening file for reading: " );
    textUtils_println( filePath );

    f = ESXDOS_fopen( filePath, ESXDOS_FILEMODE_READ, drive );
    iferror {
        textUtils_print( "Could not open file." );
        textUtils_print( "Error code= " );
        textUtils_println_l( f );
        return;
    }


    // Seek tests
    for ( i = 0; i < 10; i++ ) {

        textUtils_println( "Seeking file..." );

        pos1 = ESXDOS_fseek( 100 * i, ESXDOS_SEEK_FROM_START, f );

        iferror {
            textUtils_println( "Error while seeking." );
            break;
        }

        pos2 = ESXDOS_fgetPos( f );

        textUtils_print( "Seek ok. Pos = " );
        textUtils_print_l( pos1 );
        if ( pos1 == pos2 ) {
            textUtils_print( " Ok " );
        }
        else {
            textUtils_print( " KO. " );
            someError = true;
        }

        numBytes = ESXDOS_fread( &c, 1, f );
        iferror {
            textUtils_println( "Error reading the file." );
            break;
        }
        else {

            textUtils_print( " Read: " );
            fputc_cons( c );

            if ( c == '0' + i ) {
                textUtils_print( " Ok " );
            }
            else {
                textUtils_print( " KO. " );
                someError = true;
            }

            textUtils_print( "\n" );
        }


    }

    ESXDOS_fclose( f );
    iferror {
        textUtils_println( "Error closing the file." );
        return;
    }
    textUtils_println( "File closed OK" );

    if ( someError == true ) {
        textUtils_println( "Some test did not pass." );
    }
    else {
        textUtils_println( "All tests passed." );
    }

    textUtils_println( "End. Press a key." );
    waitKey();


}
