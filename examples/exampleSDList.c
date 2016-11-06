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

void waitKey() {
    while (getKey());
    while (!getKey());
}

#define BUFFER_SIZE ( 4096 )
uint8_t buffer[ BUFFER_SIZE + 1 ];

void main(void) {

    int16_t drive;

    int16_t dhandle;

    uint8_t currentDirectory[ 1024 ];

    uint16_t readResult;

    uint8_t * pbuffer;

    uint8_t c;

    uint8_t attributes;

    uint32_t fileSize;



    
    textUtils_64ColumnsMode();
    textUtils_cls();

    textUtils_println( "This example reads a directory listing from SD card. Press a key to start..." );

    waitKey();

    drive = ESXDOS_getDefaultDrive();
    iferror {
        textUtils_println( "SD card not inserted." );
        return;
    }

    textUtils_println( "SD card detected." );

    textUtils_print( "Drive: " );
    textUtils_println_l( drive );

    ESXDOS_getCWD( currentDirectory, drive );
    iferror {
        textUtils_print( "Error obtaining current directory." );
        return;
    }

    textUtils_print( "Current directory: " );
    textUtils_println( currentDirectory );

    dhandle = ESXDOS_openDirectory( currentDirectory, drive );
    iferror {
        textUtils_print( "Could not open directory." );
        textUtils_print( "Error code= " );
        textUtils_println_l( dhandle );
        return;
    }

    textUtils_print( "Directory handle: " );
    textUtils_println_l( dhandle );

    textUtils_println( "Directory listing:" );

    readResult = 1;
    while ( readResult > 0 ) {

        readResult = ESXDOS_readDirectory( buffer, dhandle );
        iferror {
            textUtils_println( "Error reading the directory." );
            textUtils_print( "Error code= " );
            textUtils_println_l( readResult );
            readResult = 0;
        }
        else {

            if ( readResult == 0 ) {
                break;
            }

            pbuffer = buffer;

            // Read attributes byte
            attributes = *pbuffer++;

            // Print file name
            textUtils_print( pbuffer );

            // Skip asciiz name
            while( *pbuffer > 0 ) {
                pbuffer++;
            }

            // Skip 1 terminating null byte and 4 date bytes
            pbuffer += 5;

            textUtils_print( "    " );

            if ( attributes & ESXDOS_FILE_ATTRIBUTE_DIR_BIT ) {

                textUtils_println( "<DIR>");

            }
            else {
                // Print size
                fileSize = *pbuffer++;
                fileSize += ( (uint32_t)( *pbuffer++ ) ) << 8;
                fileSize += ( (uint32_t)( *pbuffer++ ) ) << 16;
                fileSize += ( (uint32_t)( *pbuffer++ ) ) << 24;

                textUtils_println_l( fileSize );
            }

        }

    }

    ESXDOS_fclose( dhandle );
    iferror {
        textUtils_println( "Error closing the directory." );
        return;
    }
    textUtils_println( "Directory closed OK" );

    textUtils_println( "End. Press a key." );
    waitKey();
}
