/*
 * @author yomboprime
 *
 * File dialog using ESXDOS
*/

#include "fileDialog.h"

#include <string.h>

#include "esxdos.h"
#include "textUtils.h"

// Function prototypes
uint16_t fileDialogIterateSDDirectory( uint8_t *dirPath, uint16_t firstEntry, uint16_t maxEntries, void *entryCallBack );
void callBackPrintEntry( uint16_t numEntry, uint8_t *entry );
extern bool fileDialogConcatPath( uint8_t *string1, uint8_t *string2, uint16_t maxSize );
extern void fileDialogpathUpOneDir( uint8_t *path );


#define OPENDIALOG_MAX_DISPLAY_DIR_ENTRIES 20

/*
 * This function opens a file dialog in normal 32x24 chars screen mode.
 * This function must be called with the normal spectrum graphics mode set.
 * When the function exits, it is left to the function caller to change again the graphics mode and redraw the screen as appropriate.
 * The selected file path will be overwritten in the "path" string parameter.
 * It returns true on success; false if the user cancelled the selection or path name was greater than maxPathLength parameter.
 * Parameters:
 * message: Title of the dialog.
 * path: Initial dir path to open, started and terminated in "/". It is also the output value, as the selected file name will be
 *  appended to this string.
 * maxPathLength: If the total path exceeds this length, the function will return false (error)
 * attrs1, attrs2, attrsSel: Different colour attributes for the dialog.
 */
bool openFileDialog( uint8_t *message, uint8_t *path, int16_t maxPathLength, uint8_t attrs1, uint8_t attrs2, uint8_t attrsSel ) {

    uint8_t fileName[ 13 ];

    uint8_t key;

    uint16_t firstEntry = 0;
    uint16_t numEntries = 0;
    uint16_t selectedEntry = 0;

    uint16_t titleX = 16 - ( strlen( message ) >> 1 );

    textUtils_cls();

    // Print title
    textUtils_printAt32( titleX, 1 );
    textUtils_print( message );
    
    numEntries = fileDialogIterateSDDirectory( path, firstEntry, OPENDIALOG_MAX_DISPLAY_DIR_ENTRIES, callBackPrintEntry );

    while ( 1 ) {
        
        key = waitKeyPress();

        switch ( key ) {
            // Left
            case 8:
                break;
            // Right
            case 9:
                break;
            // Down
            case 10:
                break;
            // Up
            case 11:
                break;
        }

        if ( key == 32 ) {
            return false;
        }
        else {
            textUtils_print( "Key: " );
            textUtils_println_l( key );
        }

    }

    return false;

}

uint16_t fileDialogIterateSDDirectory( uint8_t *dirPath, uint16_t firstEntry, uint16_t maxEntries, void *entryCallBack ) {

    // Calls entryCallBack( numEntry, entry ) for each directory entry and returns number of iterated entries

    int16_t drive;
    int16_t dirHandle;
    uint16_t numEntries = 0;
    uint16_t readResult;
    uint8_t entry[22];

    drive = ESXDOS_getDefaultDrive();
    dirHandle = ESXDOS_openDirectory( dirPath, drive );
    iferror {
        return 0;
    }

    readResult = 1;
    while ( readResult > 0 ) {

        readResult = ESXDOS_readDirectory( entry, dirHandle );
        iferror {
            break;
        }

        if ( readResult == 0 ) {
            break;
        }

        if ( numEntries >= maxEntries ) {
            break;
        }

        if ( firstEntry == 0 ) {

            entryCallBack( numEntries, entry );

            numEntries++;

        }
        else {
            firstEntry--;
        }

    }

    ESXDOS_fclose( dirHandle );

    return numEntries;

}

void callBackPrintEntry( uint16_t numEntry, uint8_t *entryPtr ) {

    uint32_t number;
    uint8_t strNumber[ 11 ];
    uint8_t isDir;
    uint8_t nChars;

    textUtils_printAt32( 2, numEntry + 3 );

    if ( *entryPtr++ == ESXDOS_FILE_ATTRIBUTE_DIR_BIT ) {
        textUtils_print( "DIR " );
        isDir = true;
    }
    else {
        textUtils_print( "    " );
        isDir = false;
    }

    textUtils_print( entryPtr );
    nChars = 0;
    while ( *entryPtr != 0 ) {
        entryPtr++;
        nChars++;
    }
    while ( nChars < 12 ) {
        textUtils_print( " " );
        nChars++;
    }
    entryPtr++;

    // Skip date
    entryPtr+= 4;

    if ( isDir == false ) {
        // File size
        number = *entryPtr++;
        number += ( (uint32_t)( *entryPtr++ ) ) << 8;
        number += ( (uint32_t)( *entryPtr++ ) ) << 16;
        number += ( (uint32_t)( *entryPtr++ ) ) << 24;

        sprintf( strNumber, " %10lu", number );
        textUtils_print( strNumber );

    }

}

bool fileDialogConcatPath( uint8_t *string1, uint8_t *string2, uint16_t maxSize ) {

    uint16_t l1 = strlen( string1 );
    uint16_t l2 = strlen( string2 );

    if ( l1 + l2 + 1 > maxSize ) {
        return false;
    }

    string1 += l1;

    while ( l2 > 0 ) {
        *string1++ = *string2++;
        l2--;
    }
    *string1++='/';
    *string1 = 0;

    return true;

}

void fileDialogpathUpOneDir( uint8_t *path ) {

    uint8_t *pbuffer = path;
    while ( *pbuffer > 0 ) {
        pbuffer++;
    }
    pbuffer--;
    if ( *pbuffer == '/' ) {
        pbuffer--;
    }

    while( pbuffer > path && *pbuffer != '/' ) {
        pbuffer--;
    }

    if ( *pbuffer == '/' ) {
        pbuffer++;
        *pbuffer = 0;
    }

}
