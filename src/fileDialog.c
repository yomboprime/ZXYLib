/*
 * @author yomboprime
 *
 * File dialog using ESXDOS
*/

#include "fileDialog.h"

#include <math.h>
#include <string.h>

#include "esxdos.h"
#include "textUtils.h"

// Function prototypes
void filedialogCallBackPrintEntry( uint16_t numEntry, uint8_t *entry, uint8_t *userData );
void fileDialogCallBackSelectEntry( uint16_t numEntry, uint8_t *entryPtr, uint8_t *userData );
void fileDialogBrightSelection( uint16_t selectedEntry, bool bright );
void updateVerticalBar( uint16_t xPos, uint16_t pos, uint16_t total, uint8_t attrs1, uint8_t attrs2 );

#define OPENDIALOG_MAX_DIR_ENTRIES 20
#define OPENDIALOG_MAX_DIR_ENTRIES_2 400

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
 * attrs1, attrs2: Different colour attributes for the dialog.
 */
bool openFileDialog( uint8_t *message, uint8_t *path, int16_t maxPathLength, uint8_t attrs1, uint8_t attrs2 ) {

    uint8_t fileName[ 13 + 1 ];

    uint8_t key;

    uint16_t i;

    uint16_t firstEntry = 0;
    uint16_t numEntries = 0;
    uint16_t selectedEntry = 0;

    uint16_t totalEntries = 0;

    uint16_t titleX = 16 - ( strlen( message ) >> 1 );

    bool refreshDirectory = true;

    bool moreEntries = false;

    textUtils_setAttributes( attrs2 );
    textUtils_cls();

    // Print title
    textUtils_printAt( titleX, 1 );
    textUtils_print( message );

    while ( 1 ) {

        if ( refreshDirectory == true ) {

            textUtils_setAttributes( attrs1 );
            numEntries = fileDialogIterateSDDirectory( path, firstEntry, OPENDIALOG_MAX_DIR_ENTRIES, filedialogCallBackPrintEntry, &totalEntries, &moreEntries, NULL );
            if ( numEntries > 0 ) {

                textUtils_setAttributes( attrs2 );
                for ( i = OPENDIALOG_MAX_DIR_ENTRIES - numEntries; i > 0; i-- ) {
                    textUtils_print("\n                               " );
                }

                fileDialogBrightSelection( selectedEntry, true );
            }

            updateVerticalBar( 30, firstEntry, totalEntries, attrs1, attrs2 );

            refreshDirectory = false;

        }
        
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
                if ( selectedEntry == numEntries - 1 ) {
                    if ( moreEntries == true ) {
                        firstEntry += OPENDIALOG_MAX_DIR_ENTRIES;
                        fileDialogBrightSelection( selectedEntry, false );
                        selectedEntry = 0;
                        refreshDirectory = true;
                    }
                }
                else {
                    fileDialogBrightSelection( selectedEntry, false );
                    selectedEntry++;
                    fileDialogBrightSelection( selectedEntry, true );
                }
                break;

            // Up
            case 11:
                if ( selectedEntry == 0 ) {
                    if ( firstEntry > 0 ) {
                        firstEntry -= OPENDIALOG_MAX_DIR_ENTRIES;
                        fileDialogBrightSelection( selectedEntry, false );
                        selectedEntry = OPENDIALOG_MAX_DIR_ENTRIES - 1;
                        refreshDirectory = true;
                    }
                }
                else {
                    fileDialogBrightSelection( selectedEntry, false );
                    selectedEntry--;
                    fileDialogBrightSelection( selectedEntry, true );
                }
                break;

            // Space, Escape
            case 32:
                return false;

            // Enter
            case 13:

                numEntries = fileDialogIterateSDDirectory( path, firstEntry + selectedEntry, 1, fileDialogCallBackSelectEntry, NULL, NULL, fileName );
                if ( numEntries > 0 ) {
                    if ( strcmp( fileName + 1, ".." ) == 0 ) {
                        // Selected ..
                        fileDialogpathUpOneDir( path );
                        fileDialogBrightSelection( selectedEntry, false );
                        firstEntry = 0;
                        selectedEntry = 0;
                        refreshDirectory = true;
                    }
                    else if ( strcmp( fileName + 1, "." ) == 0 ) {
                        fileDialogBrightSelection( selectedEntry, false );
                        refreshDirectory = true;
                    }
                    else if ( fileDialogConcatPath( path, fileName + 1, maxPathLength, *fileName == 1 ? true : false ) == true ) {

                        if ( *fileName == 1 ) {
                            // The user selected a directory
                            fileDialogBrightSelection( selectedEntry, false );
                            firstEntry = 0;
                            selectedEntry = 0;
                            refreshDirectory = true;
                        }
                        else {
                            // The user selected a file and the path is already updated
                            return true;
                        }
                    }
                }
                
                break;
        }

        //textUtils_print( "Key: " );
        //textUtils_println_l( key );
    }

    return false;

}

uint16_t fileDialogIterateSDDirectory( uint8_t *dirPath, uint16_t firstEntry, uint16_t maxEntries, void *entryCallBack, uint16_t *totalEntries, bool *moreEntries, uint8_t *userData ) {

    // Calls entryCallBack( numEntry, entry, userData ) for each directory entry and returns number of iterated entries
    // moreEntries is modified and tells if there are more entries after the last iterated.

    int16_t drive;
    int16_t dirHandle;
    uint16_t numEntries = 0;
    uint16_t readResult;
    uint8_t entry[22];

    if ( moreEntries != NULL ) {
        *moreEntries = false;
    }

    if ( totalEntries != NULL ) {
        *totalEntries = 0;
    }

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

        if ( totalEntries != NULL ) {
            (*totalEntries)++;
        }

        if ( numEntries >= maxEntries ) {
            if ( moreEntries != NULL ) {
                *moreEntries = true;
            }
            if ( totalEntries == NULL ) {
                break;
            }
        }
        else {
            if ( firstEntry == 0 ) {

                entryCallBack( numEntries, entry, userData );

                numEntries++;

            }
            else {
                firstEntry--;
            }
        }

    }

    ESXDOS_fclose( dirHandle );

    return numEntries;

}

void fileDialogBrightSelection( uint16_t selectedEntry, bool bright ) {

    textUtils_paintSegmentWithBright( 2, 29, 3 + selectedEntry, bright );

}

void filedialogCallBackPrintEntry( uint16_t numEntry, uint8_t *entryPtr, uint8_t *userData ) {

    uint8_t b1, b2, b3, b4;
    uint32_t fileSize;
    
    uint8_t strNumber[ 11 ];
    uint8_t isDir;
    uint8_t nChars;
    uint8_t * ptr;

    textUtils_printAt( 2, numEntry + 3 );

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
    while ( nChars < 24 ) {
        textUtils_print( " " );
        nChars++;
    }

    /*
    entryPtr++;

    // Skip date
    entryPtr+= 4;

    if ( isDir == false ) {

        // File size

        fileSize = *entryPtr++;
        fileSize += ( (uint32_t)( *entryPtr++ ) ) << 8;
        fileSize += ( (uint32_t)( *entryPtr++ ) ) << 16;
        fileSize += ( (uint32_t)( *entryPtr++ ) ) << 24;

        sprintf( strNumber, "10%lu", fileSize );
        textUtils_print( strNumber );

    }
    */
}

void fileDialogCallBackSelectEntry( uint16_t numEntry, uint8_t *entryPtr, uint8_t *userData ) {

    uint32_t number;
    uint8_t strNumber[ 11 ];
    uint8_t isDir;
    uint8_t nChars;

    // First byte of user data is 1 = directory, 0 = file
    *userData++ = *entryPtr++ == ESXDOS_FILE_ATTRIBUTE_DIR_BIT ? 1: 0;

    // Next is the asciiz file name.
    nChars = 0;
    while ( *entryPtr != 0 ) {
        *userData++ = *entryPtr++;
        nChars++;
    }
    *userData = 0;

}

bool fileDialogConcatPath( uint8_t *string1, uint8_t *string2, uint16_t maxSize, bool appendBar ) {

    // Concats string2 to string1

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
    if ( appendBar == true ) {
        *string1++='/';
    }
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


void updateVerticalBar( uint16_t xPos, uint16_t pos, uint16_t total, uint8_t attrs1, uint8_t attrs2 ) {

    uint16_t posChars = 0;
    uint16_t sizeChars = OPENDIALOG_MAX_DIR_ENTRIES;
    uint16_t end;
    uint16_t i;
    float totalInverse;
    float fraction;

    if ( total > OPENDIALOG_MAX_DIR_ENTRIES ) {

        totalInverse = (int32_t)total;
        totalInverse = 1.0 / totalInverse;
        fraction = ((int32_t)pos);
        fraction *= totalInverse;

        posChars = (uint16_t)( fraction * ((float)OPENDIALOG_MAX_DIR_ENTRIES) );
        sizeChars = (uint16_t)( ((float)OPENDIALOG_MAX_DIR_ENTRIES_2 ) * totalInverse );
        if ( sizeChars == 0 ) {
            sizeChars = 1;
        }

    }

    // Adjust to the end
    end = posChars + sizeChars;
    if ( end > OPENDIALOG_MAX_DIR_ENTRIES ) {
        posChars -= end - OPENDIALOG_MAX_DIR_ENTRIES;
    }

    // Print bar background
    textUtils_setAttributes( attrs1 );
    for ( i = 3; i < 23; i++ ) {
        textUtils_printAt( xPos, i );
        fputc_cons( '|' );
    }


    // Print bar
    textUtils_setAttributes( attrs2 );
    for ( i = 3 + posChars; i < 3 + posChars + sizeChars; i++ ) {
        textUtils_printAt( xPos, i );
        fputc_cons( '=' );
    }

}

