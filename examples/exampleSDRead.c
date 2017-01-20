/*
 * @author yomboprime
 *
 * Test read from SD card file
*/

#include <input.h>
#include <stdlib.h>
#include <string.h>

#include "../src/textUtils.h"
#include "../src/esxdos.h"

#define getKey() in_Inkey()

#define BUFFER_SIZE ( 4096 )
uint8_t buffer[ BUFFER_SIZE + 1 ];

void main(void) {

    int16_t drive;

    int16_t f;

    uint8_t *filePath = "/hola.txt";

    uint8_t buffer[258];

    uint16_t numBytes;



    textUtils_64ColumnsMode();
    textUtils_cls();

    textUtils_println( "This example reads a file from SD card. Press a key to start..." );

    waitKey();

    drive = ESXDOS_getDefaultDrive();

    iferror {
        textUtils_println( "SD card not inserted." );
        return;
    }

    textUtils_println( "SD card detected." );

    textUtils_print( "Drive: " );
    textUtils_println_l( drive );

    textUtils_print( "Opening file: " );
    textUtils_println( filePath );

    f = ESXDOS_fopen( filePath, ESXDOS_FILEMODE_READ, drive );
    iferror {
        textUtils_print( "Could not open file." );
        textUtils_print( "Error code= " );
        textUtils_println_l( f );
        return;
    }

    numBytes = BUFFER_SIZE;
    numBytes = ESXDOS_fread( buffer, numBytes, f );
    iferror {
        textUtils_println( "Error reading the file." );
    }
    else {

        buffer[ numBytes ] = 0;

        textUtils_print( "Read bytes: " );
        textUtils_println_l( numBytes );

        textUtils_print( "Content: " );
        textUtils_println( buffer );

    }

    ESXDOS_fclose( f );
    iferror {
        textUtils_println( "Error closing the file." );
        return;
    }
    textUtils_println( "File closed OK" );

    textUtils_println( "End. Press a key." );
    waitKey();

}
