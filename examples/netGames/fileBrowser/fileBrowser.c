/*

FILEBROW: TCP/IP file browser for ESXDOS

MIT License

Copyright (c) 2021 Juan Jose Luna Espinosa

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

/*
* Includes
*/
#include <input.h>
#include <stdlib.h>
#include <string.h>
#include <spectrum.h>
#include <math.h>

#include "../../../src/network.h"
#include "../../../src/textUtils.h"
#include "../../../src/zxuno/turbo.h"
#include "../../../src/esxdos.h"
#include "../../../src/config.h"
#include "../../../src/retroProt/retroProtFile.h"

/*
* Macros / prototypes
*/

#define getKey() in_Inkey()

void loadConfig();
void configCallback( uint8_t *param, uint8_t *value );

void defineGraphics();
void brightSelection( bool bright );

void connect();
bool refreshTCPListing( uint8_t showRefreshingMessage );
void showTCPFileInfo();

void initDrive();
void refreshSDListing();
void showSDFileInfo();
bool getSDFileNameAndSize( uint8_t *path, uint32_t *fileSize, uint8_t *fileOrDirectory );

void downloadProgressCallback( uint32_t totalBytesRead );
void uploadProgressCallback( uint32_t totalBytesRead );

void updateVerticalBar( uint16_t xPos, uint16_t pos, uint16_t total );

void printToStatusBox( uint8_t *line1, uint8_t *line2 );
void printToTitleBox( bool secondColumn, uint8_t *line1, uint8_t *line2 );
void printToBox( uint8_t attributes, uint16_t x, uint16_t y, uint16_t length, uint8_t *line1, uint8_t *line2 );

void activateTurbo();
void deactivateTurbo();

uint8_t *shortString( uint8_t *string, int maxSize );
bool concatPath( uint8_t *string1, uint8_t *string2, uint16_t maxSize, bool appendSlash );

void infiniteLoop();

/*
* UDG Graphics
*/
#define UDG_DIRECTORY 0
#define UDG_BAR0 1
#define UDG_BAR1 2
#define UDG_BAR2 3
#define UDG_BARBACKGROUND0 4
#define UDG_BARBACKGROUND1 5
#define UDG_BARBACKGROUND2 6
#define UDG_STRIKE 7

static uint8_t directoryUDG [] = {
	0, 126, 130, 159, 161, 161, 194, 254
};

static uint8_t bar0UDG [] = {
	255, 129, 129, 129, 129, 129, 129, 129
};

static uint8_t bar1UDG [] = {
	129, 129, 129, 129, 129, 129, 129, 129
};

static uint8_t bar2UDG [] = {
	129, 129, 129, 129, 129, 129, 129, 255
};

static uint8_t barBackground0UDG [] = {
	170, 0, 129, 0, 129, 0, 129, 0
};

static uint8_t barBackground1UDG [] = {
	129, 0, 129, 0, 129, 0, 129, 0
};

static uint8_t barBackground2UDG [] = {
	129, 0, 129, 0, 129, 0, 129, 84
};

static uint8_t strikeUDG [] = {
	0x3c, 0x3c, 0x78, 0x7f, 0x0e, 0x1c, 0x18, 0x30
};

/*
* GUI elements
*/
// Coordinates
#define X_COORD_TITLE_BOX 0
#define X_SIZE_TITLE_BOX 32
#define X_SIZE_TITLE_BOX1 17
#define X_SIZE_TITLE_BOX2 13
#define Y_COORD_TITLE_BOX 0
#define Y_SIZE_TITLE_BOX 2

#define X_COORD_TCP_LIST 0
#define Y_COORD_TCP_LIST 2
#define X_SIZE_TCP_LIST 16

#define X_COORD_SD_LIST 18
#define Y_COORD_SD_LIST 2
#define X_SIZE_SD_LIST 13

#define X_COORD_STATUS_BOX 0
#define Y_COORD_STATUS_BOX 22
#define X_SIZE_STATUS_BOX 32
#define Y_SIZE_STATUS_BOX 2

#define X_COORD_TCP_VERT_BAR 16
#define X_COORD_SD_VERT_BAR 31
#define Y_COORD_START_VERT_BAR 2
#define Y_COORD_END_VERT_BAR 21

// attributes
#define ATTRIBUTES_TITLE_BOX ( PAPER_BLUE | INK_WHITE | BRIGHT )
#define ATTRIBUTES_LISTING ( PAPER_CYAN | INK_BLACK )
#define ATTRIBUTES_LISTING_SELECTED ( PAPER_CYAN | INK_BLACK | BRIGHT )
#define ATTRIBUTES_STATUS_BOX ( PAPER_BLACK | INK_WHITE | BRIGHT )
#define ATTRIBUTES_VERT_BAR ( PAPER_WHITE | INK_BLACK )
#define ATTRIBUTES_VERT_BAR_BACKGROUND ( PAPER_WHITE | INK_BLACK | BRIGHT )
#define ATTRIBUTES_DIRECTORY_ICON ( PAPER_CYAN | INK_YELLOW )
#define ATTRIBUTES_LIGHTNING ( FLASH | BRIGHT | PAPER_BLUE | INK_YELLOW )

/*
* Global variables / other definitions
* Use of global variables is maximized due to performance.
*/

uint16_t key, key2, i, j, k;
uint32_t longTemp1;
uint16_t diff;
uint32_t fileSize1;
uint8_t fileOrDirectory1;
uint16_t pathLength1;

uint8_t *pbuffer;
uint8_t *pbuffer2;
uint8_t *pbuffer3;

uint8_t tempChar;

#define MAX_DISPLAY_DIR_ENTRIES 20
#define MAX_DISPLAY_DIR_ENTRIES_2 400
#define MAX_BYTES_TCP_FILENAME 20
#define DISPLAY_BYTES_TCP_FILENAME 11

// 1 attr + 8 name + 1 dot + 3 ext + 1 terminating 0 + 4 date + 4 filesize
#define MAX_SD_ENTRY_SIZE 22

#define TCP_FILE_ENTRY_SIZE ( MAX_BYTES_TCP_FILENAME + TCP_DIR_ENTRY_SIZE )

// Disk buffer // TODO 4096
#define DISK_BUFFER_SIZE ( 1024 )
uint8_t *diskBuffer = (uint8_t *)0x6000;

// Current TCP path
#define TCP_PATH_SIZE 512
uint8_t *tcpPath = (uint8_t *)0x7000;

// Current SD path
#define SD_PATH_SIZE 512
uint8_t *sdPath = (uint8_t *)0x7200;

// Filename scratch buffer
#define FILE_PATH_SIZE 512
uint8_t *filePath = (uint8_t *)0x7400;

// TCP List buffer
#define TCP_LIST_SIZE 582
uint8_t *tcpList = (uint8_t *)0x7600;

// SD List buffer
#define SD_LIST_SIZE 442
uint8_t *sdList = (uint8_t *)0x7846;

// WiFi connection parameters
uint8_t currentWiFiSSID[ NETWORK_SSID_LENGTH + 1 ];
#define MAX_PASSWORD_LENGTH 63
uint8_t currentWiFiPassword[ MAX_PASSWORD_LENGTH + 1 ];
#define MAX_HOST_LENGTH 80
uint8_t currentHost[ MAX_HOST_LENGTH + 1 ];
uint8_t currentPort[ 6 ];

// Current display status variables
uint16_t firstDisplayedTCPEntry = 0;
uint16_t numDisplayedTCPEntries = 0;
uint16_t numTotalTCPEntries = 0;

uint16_t firstDisplayedSDEntry = 0;
uint16_t numDisplayedSDEntries = 0;
uint16_t numTotalSDEntries = 0;

uint8_t originalTurbo;
#define FP_TURBO ZXUNO_TURBO_X2

// Selection
bool selectedTCP_NO_SD = false;
uint16_t selectedDisplayedEntry = 0;

// Connection status
bool connected = false;

// SD card variables
bool SD_initiated = false;
int16_t SD_drive;

// Infinite loop variables
bool showFileInfo;

uint8_t numOrderingMethods = 6;
uint8_t *orderingMethods[] = {
	"A-Z",
	"Z-A",
	"Size desc.",
	"Size asc.",
	"Date desc.",
	"Date asc."
};
uint8_t currentOrderingMethod = 0;


//*****************************************
//*****************************************
//***************** MAIN ******************
//*****************************************
//*****************************************
void main(void) {

	originalTurbo = TURBO_get();
	TURBO_set( FP_TURBO );

	textUtils_32ColumnsMode();
	textUtils_cls();

	defineGraphics();

	zx_border( INK_BLUE );

	TURBO_set( originalTurbo );
	
	loadConfig();
	TURBO_set( FP_TURBO );

	textUtils_paintRectangleWithAttributes( X_COORD_TITLE_BOX, X_SIZE_TITLE_BOX - 1, Y_COORD_TITLE_BOX, Y_SIZE_TITLE_BOX - 1, ATTRIBUTES_TITLE_BOX );

	textUtils_paintRectangleWithAttributes( X_COORD_TCP_LIST,
			X_COORD_TCP_LIST + X_SIZE_TCP_LIST - 1,
			Y_COORD_TCP_LIST,
			Y_COORD_TCP_LIST + MAX_DISPLAY_DIR_ENTRIES - 1,
			ATTRIBUTES_LISTING );

	textUtils_paintRectangleWithAttributes( X_COORD_SD_LIST,
			X_COORD_SD_LIST + X_SIZE_SD_LIST - 1,
			Y_COORD_SD_LIST,
			Y_COORD_SD_LIST + MAX_DISPLAY_DIR_ENTRIES - 1,
			ATTRIBUTES_LISTING );

	textUtils_paintRectangleWithAttributes( X_COORD_SD_LIST - 1,
			X_COORD_SD_LIST - 1,
			Y_COORD_SD_LIST,
			Y_COORD_SD_LIST + MAX_DISPLAY_DIR_ENTRIES - 1,
			PAPER_BLUE | INK_BLACK );

	textUtils_paintRectangleWithAttributes( X_COORD_STATUS_BOX,
			X_COORD_STATUS_BOX + X_SIZE_STATUS_BOX - 1,
			Y_COORD_STATUS_BOX,
			Y_COORD_STATUS_BOX + Y_SIZE_STATUS_BOX - 1,
			ATTRIBUTES_STATUS_BOX );

	updateVerticalBar( X_COORD_TCP_VERT_BAR, 0, 20 );
	updateVerticalBar( X_COORD_SD_VERT_BAR, 0, 20 );

	initDrive();

	if ( SD_initiated ) {

		refreshSDListing();

	}

	connect();

	if ( connected ) {

		if ( refreshTCPListing( true ) == true ) {

			printToStatusBox( "FILEBROW ready.", "yombo'2021" );

		}

	}

	brightSelection( true );
	
	TURBO_set( originalTurbo );

	// The loop
	infiniteLoop();

}
//*****************************************
//*****************************************
//*****************************************
//*****************************************
//*****************************************

void loadConfig() {

	int error;
	uint8_t *errorTxt;
	uint8_t configFilename[ 10 ];

	*currentWiFiSSID = 0;
	*currentWiFiPassword = 0;

	*currentHost = 0;
	*currentPort = 0;
	sprintf( tcpPath, "/" );
	sprintf( sdPath, "/" );

	error = loadConfigFile( "/SYS/CONFIG/RPROT.CFG", configCallback, diskBuffer, DISK_BUFFER_SIZE );

	// Validate parameters

	if ( error == CONFIG_OK ) {

		if ( *currentWiFiSSID == 0 ) {

			error = CONFIG_ERROR_VALIDATION;
			errorTxt = "SSID not set.";

		}
		else if ( *currentWiFiPassword == 0 ) {

			error = CONFIG_ERROR_VALIDATION;
			errorTxt = "WiFi password not set.";

		}

		if ( *currentHost == 0 ) {

			error = CONFIG_ERROR_VALIDATION;
			errorTxt = "Server not set.";

		}
		else if ( *currentPort == 0 ) {

			error = CONFIG_ERROR_VALIDATION;
			errorTxt = "Port not set.";

		}

	}

	if ( error != CONFIG_OK ) {

		textUtils_println( "Error loading RPROT.CFG: " );
		textUtils_println( configError( error ) );
		if ( error = CONFIG_ERROR_VALIDATION ) textUtils_println( errorTxt );

		waitKey();

	}

}

void configCallback( uint8_t *param, uint8_t *value ) {

	if ( strcmp( "ssid", param ) == 0 ) {
		strcpy( currentWiFiSSID, value );
	}
	else if ( strcmp( "password", param ) == 0 ) {
		strcpy( currentWiFiPassword, value );
	}
	else if ( strcmp( "server", param ) == 0 ) {
		strcpy( currentHost, value );
	}
	else if ( strcmp( "file_port", param ) == 0 ) {
		strcpy( currentPort, value );
	}
	else if ( strcmp( "tcp_initial_dir", param ) == 0 ) {
		strcpy( tcpPath, value );
	}
	else if ( strcmp( "sd_initial_dir", param ) == 0 ) {
		strcpy( sdPath, value );
	}

}

void defineGraphics() {

	textUtils_defineUDGGraphic( directoryUDG, UDG_DIRECTORY );

	textUtils_defineUDGGraphic( bar0UDG, UDG_BAR0 );
	textUtils_defineUDGGraphic( bar1UDG, UDG_BAR1 );
	textUtils_defineUDGGraphic( bar2UDG, UDG_BAR2 );

	textUtils_defineUDGGraphic( barBackground0UDG, UDG_BARBACKGROUND0 );
	textUtils_defineUDGGraphic( barBackground1UDG, UDG_BARBACKGROUND1 );
	textUtils_defineUDGGraphic( barBackground2UDG, UDG_BARBACKGROUND2 );
	
	textUtils_defineUDGGraphic( strikeUDG, UDG_STRIKE );

}

void brightSelection( bool bright ) {

	uint8_t x0 = selectedTCP_NO_SD == true ? X_COORD_TCP_LIST : X_COORD_SD_LIST;
	uint8_t s0 = selectedTCP_NO_SD == true ? X_COORD_TCP_VERT_BAR : X_SIZE_TITLE_BOX2;

	textUtils_paintSegmentWithBright( x0, x0 + s0 - 1, Y_COORD_TCP_LIST + selectedDisplayedEntry, bright );

}

bool refreshTCPListing( uint8_t showRefreshingMessage ) {

	bool flagMoreChars;
	uint8_t currentFileNameSize;
	uint8_t result;

	if ( showRefreshingMessage ) printToStatusBox( "Reading list...", NULL );

	result = rpFile_listFiles(
		tcpPath,
		"",
		currentOrderingMethod,
		tcpList,
		firstDisplayedTCPEntry,
		MAX_DISPLAY_DIR_ENTRIES,
		&numDisplayedTCPEntries,
		&numTotalTCPEntries,
		MAX_BYTES_TCP_FILENAME
	);

	if ( result != RPF_ERROR_OK ) {
		sprintf( tcpPath, "Code: %d", result );
		printToStatusBox( "Error reading list:", tcpPath );
		return false;
	}

	pbuffer = tcpList;

	textUtils_setAttributes( ATTRIBUTES_LISTING );
	for ( i = 0; i < numDisplayedTCPEntries; i++ ) {

		pbuffer2 = pbuffer;

		textUtils_printAt( X_COORD_TCP_LIST, Y_COORD_TCP_LIST + i );

		// Print directory icon or space
		if ( *pbuffer2++ == '>' ) {
			textUtils_setAttributes( ATTRIBUTES_DIRECTORY_ICON );
			fputc_cons( UDG_GRAPHICS_START + UDG_DIRECTORY );
			textUtils_setAttributes( ATTRIBUTES_LISTING );
		}
		else {
			fputc_cons( ' ' );
		}

		currentFileNameSize = DISPLAY_BYTES_TCP_FILENAME;
		flagMoreChars = pbuffer2[ MAX_BYTES_TCP_FILENAME ] == '.' ? true : false;
		if ( flagMoreChars == true ) {
			currentFileNameSize -= 2;
		}

		for ( j = 0; j < currentFileNameSize; j++ ) {

			tempChar = *pbuffer2++;
			if ( textUtils_isReadable( tempChar ) ) fputc_cons( tempChar );
			else fputc_cons( ' ' );

		}

		for ( ; j < MAX_BYTES_TCP_FILENAME; j++ ) pbuffer2++;

		// Skip separation char
		pbuffer2++;

		// Print separation
		if ( flagMoreChars == true ) {

			fputc_cons( '.' );
			fputc_cons( '.' );
			fputc_cons( '.' );

		}
		else {

			fputc_cons( ' ' );

		}

		// Extension
		for ( j = 0; j < 3; j++ ) {

			fputc_cons( *pbuffer2++ );

		}

		pbuffer += TCP_FILE_ENTRY_SIZE;

	}

	// Erase rest of lines
	k = X_SIZE_TCP_LIST;
	for ( ; i < MAX_DISPLAY_DIR_ENTRIES; i++ ) {
		textUtils_printAt( X_COORD_TCP_LIST, Y_COORD_TCP_LIST + i );
		for ( j = 0; j < k; j++ ) {
			fputc_cons( ' ' );
		}
	}

	updateVerticalBar( X_COORD_TCP_VERT_BAR, firstDisplayedTCPEntry, numTotalTCPEntries );

	// Don't show host name or IP on screen (security risk)
	//printToTitleBox( false, shortString( currentHost, X_SIZE_TITLE_BOX1 ), shortString( tcpPath, X_SIZE_TITLE_BOX1 ) );
	printToTitleBox( false, shortString( "RetroProt server:", X_SIZE_TITLE_BOX1 ), shortString( tcpPath, X_SIZE_TITLE_BOX1 ) );

	return true;

}

void updateVerticalBar( uint16_t xPos, uint16_t pos, uint16_t total ) {

	// 0 <= pos <= 1

	uint16_t posChars = 0;
	uint16_t sizeChars = MAX_DISPLAY_DIR_ENTRIES;
	uint16_t end;
	uint32_t n1, n2;

	if ( total > MAX_DISPLAY_DIR_ENTRIES ) {

		n1 = MAX_DISPLAY_DIR_ENTRIES;
		n1 *= pos;
		n1 /= total;
		posChars = (uint16_t)( n1 );

		n2 = total;
		n2 /= MAX_DISPLAY_DIR_ENTRIES;
		if ( ( total % MAX_DISPLAY_DIR_ENTRIES ) != 0 ) n2++;

		n1 = MAX_DISPLAY_DIR_ENTRIES;
		n1 /= n2;

		sizeChars = (uint16_t)( n1 );
		if ( sizeChars == 0 ) {
			sizeChars = 1;
		}

	}

	// Adjust to the end
	end = posChars + sizeChars;
	if ( end > MAX_DISPLAY_DIR_ENTRIES ) {
		posChars -= end - MAX_DISPLAY_DIR_ENTRIES;
	}

	// Print bar background
	textUtils_setAttributes( ATTRIBUTES_VERT_BAR_BACKGROUND );
	textUtils_printAt( xPos, Y_COORD_START_VERT_BAR );
	fputc_cons( UDG_GRAPHICS_START + UDG_BARBACKGROUND0 );
	for ( i = Y_COORD_START_VERT_BAR + 1; i < Y_COORD_END_VERT_BAR; i++ ) {
		textUtils_printAt( xPos, i );
		fputc_cons( UDG_GRAPHICS_START + UDG_BARBACKGROUND1 );
	}
	textUtils_printAt( xPos, Y_COORD_END_VERT_BAR );
	fputc_cons( UDG_GRAPHICS_START + UDG_BARBACKGROUND2 );

	// Print bar
	textUtils_setAttributes( ATTRIBUTES_VERT_BAR );
	textUtils_printAt( xPos, Y_COORD_START_VERT_BAR + posChars );
	fputc_cons( UDG_GRAPHICS_START + UDG_BAR0 );
	for ( i = Y_COORD_START_VERT_BAR + posChars + 1; i < Y_COORD_START_VERT_BAR + posChars + sizeChars - 1; i++ ) {
		textUtils_printAt( xPos, i );
		fputc_cons( UDG_GRAPHICS_START + UDG_BAR1 );
	}
	textUtils_printAt( xPos, Y_COORD_START_VERT_BAR + posChars + sizeChars - 1 );
	fputc_cons( UDG_GRAPHICS_START + UDG_BAR2 );

}

void printToStatusBox( uint8_t *line1, uint8_t *line2 ) {
	printToBox( ATTRIBUTES_STATUS_BOX, X_COORD_STATUS_BOX, Y_COORD_STATUS_BOX, X_SIZE_STATUS_BOX, line1, line2 );
}

void printToTitleBox( bool secondColumn, uint8_t *line1, uint8_t *line2 ) {
	printToBox( ATTRIBUTES_TITLE_BOX,
			secondColumn == false ? X_COORD_TITLE_BOX : X_COORD_SD_LIST,
			Y_COORD_TITLE_BOX,
			secondColumn == false ? X_SIZE_TITLE_BOX1 : X_SIZE_TITLE_BOX2,
			line1, line2 );
            
	textUtils_paintRectangleWithAttributes( X_COORD_TITLE_BOX, X_SIZE_TITLE_BOX - 1, Y_COORD_TITLE_BOX, Y_SIZE_TITLE_BOX - 1, ATTRIBUTES_TITLE_BOX );
}

void printToBox( uint8_t attributes, uint16_t x, uint16_t y, uint16_t length, uint8_t *line1, uint8_t *line2 ) {

	textUtils_setAttributes( attributes );

	textUtils_printAt( x, y );
	i = 0;
	if ( line1 != NULL ) {
		while ( i < length && *line1 > 0 ) {
			fputc_cons( *line1++ );
			i++;
		}
	}
	for ( ; i < length; i++ ) {
		fputc_cons( ' ' );
	}

	textUtils_printAt( x, y + 1 );
	i = 0;
	if ( line2 != NULL ) {
		while ( i < length && *line2 > 0 ) {
			fputc_cons( *line2++ );
			i++;
		}
	}
	for ( ; i < length; i++ ) {
		fputc_cons( ' ' );
	}

}

void connect() {

	uint8_t flag = 0;
	uint8_t i;
	connected = false;

	printToStatusBox( "Initing network...", NULL );

	if ( initNetwork() != NETWORK_ERROR_OK ) {

		printToStatusBox( "Can't talk with the WiFi module.", NULL );

		return;

	}

	// Try connecting to server without connecting WiFi. If it fails, retry first connecting to WiFi
	for ( i = 0; i < 2; i ++ ) {

		if ( i == 1 ) {

			printToStatusBox( "Connecting to net... ", currentWiFiSSID );

			if ( connectToWiFi( currentWiFiSSID, currentWiFiPassword, 25000 ) != NETWORK_ERROR_OK ) {

				printToStatusBox( "Error connecting to net ", currentWiFiSSID );
				return;

			}

		}

		printToStatusBox( "Connecting to server...", NULL );

		if ( connectToServer( currentHost, currentPort ) != NETWORK_ERROR_OK ) {

			if ( i == 1 ) {

				printToStatusBox( "Error connecting to server.", NULL );
				return;

			}

		}
		else break;

	}

	if ( openServerStream() != NETWORK_ERROR_OK ) {

		printToStatusBox( "Error opening data stream", "to server." );
		return;

	}

	connected = true;

}

void initDrive() {

	printToStatusBox( "Initing SD...", NULL );

	SD_initiated = false;

	SD_drive = ESXDOS_getDefaultDrive();
	iferror {
		printToStatusBox( "SD not inserted.", NULL );
		return;
	}

	printToStatusBox( "SD detected.", NULL );

	ESXDOS_getCWD( sdPath, SD_drive );
	iferror {
		printToStatusBox( "Error getting SD CWD.", NULL );
		return;
	}

	SD_initiated = true;

}

void refreshSDListing() {

	int16_t dhandle;

	uint16_t readResult;
	uint16_t firstEntryCount;

	uint8_t attributes;

	numDisplayedSDEntries = 0;
	numTotalSDEntries = 0;

	dhandle = ESXDOS_openDirectory( sdPath, SD_drive );
	iferror {
		sprintf( filePath, "Error code= %d", dhandle );
		printToStatusBox( "Could not open directory.", filePath );
		return;
	}

	pbuffer = sdList;
	firstEntryCount = firstDisplayedSDEntry;

	readResult = 1;
	while ( readResult > 0 ) {

		readResult = ESXDOS_readDirectory( pbuffer, dhandle );
		iferror {
			sprintf( filePath, "Error code= %d", readResult );
			printToStatusBox( "Error reading the directory." , filePath );
			readResult = 0;
		}
		else {

			if ( readResult == 0 ) {
				break;
			}

			if ( *( pbuffer + 1 ) == '.' && *( pbuffer + 2 ) == 0 ) {

				continue;

			}

			if ( numDisplayedSDEntries < MAX_DISPLAY_DIR_ENTRIES ) {

				if ( firstEntryCount == 0 ) {
					pbuffer += MAX_SD_ENTRY_SIZE;
					numDisplayedSDEntries++;
				}
				else {
					firstEntryCount--;
				}

			}

			numTotalSDEntries++;

		}

	}

	pbuffer = sdList;
	textUtils_setAttributes( ATTRIBUTES_LISTING );
	for ( i = 0; i < numDisplayedSDEntries; i++ ) {

		pbuffer2 = pbuffer;

		textUtils_printAt( X_COORD_SD_LIST, Y_COORD_SD_LIST + i );

		// Read attributes byte
		attributes = *pbuffer2++;

		if ( attributes == ESXDOS_FILE_ATTRIBUTE_DIR_BIT ) {

			textUtils_setAttributes( ATTRIBUTES_DIRECTORY_ICON );
			fputc_cons( UDG_GRAPHICS_START + UDG_DIRECTORY );
			textUtils_setAttributes( ATTRIBUTES_LISTING );
		}
		else {
			fputc_cons( ' ' );
		}

		// Print file name
		j = 0;
		while( *pbuffer2 != 0 ) {

			tempChar = *pbuffer2;
			if ( textUtils_isReadable( tempChar ) ) fputc_cons( tempChar );
			else fputc_cons( '?' );

			pbuffer2++;
			j++;

		}

		for ( ; j < 12; j++ ) {

			fputc_cons( ' ' );

		}

		pbuffer += MAX_SD_ENTRY_SIZE;

	}

	// Erase rest of lines
	k = X_SIZE_SD_LIST;
	for ( ; i < MAX_DISPLAY_DIR_ENTRIES; i++ ) {
		textUtils_printAt( X_COORD_SD_LIST, Y_COORD_SD_LIST + i );
		for ( j = 0; j < k; j++ ) {
			fputc_cons( ' ' );
		}
	}

	ESXDOS_fclose( dhandle );

	updateVerticalBar( X_COORD_SD_VERT_BAR, firstDisplayedSDEntry, numTotalSDEntries );

	printToTitleBox( true, "ESXDOS Drive:", shortString( sdPath, X_SIZE_TITLE_BOX2 ) );

}

void showTCPFileInfo() {

	uint8_t b0;

	if ( selectedDisplayedEntry >= numDisplayedTCPEntries ) {
		return;
	}

	pbuffer = tcpList;
	pbuffer += selectedDisplayedEntry * TCP_FILE_ENTRY_SIZE;

	// Type byte
	i = *pbuffer++;

	// Store name
	pbuffer2 = filePath;


	j = 0;
	while ( j < MAX_BYTES_TCP_FILENAME ) {
		b0 = *pbuffer;
		if ( ! textUtils_isReadable( b0 ) ) break;
		*pbuffer2++ = b0;
		pbuffer++;
		j++;
	}

	while ( j < MAX_BYTES_TCP_FILENAME ) {
		pbuffer++;
		j++;
	}

	// Separation char
	if ( *pbuffer++ == '.' ) {
		*pbuffer2++ = '.';
		*pbuffer2++ = '.';
		*pbuffer2++ = '.';
	}
	else if ( *pbuffer != ' ' ) {
		// Dot, if there's extension
		*pbuffer2++ = '.';
	}

	// Store extension
	*pbuffer2++ = *pbuffer++;
	*pbuffer2++ = *pbuffer++;
	*pbuffer2++ = *pbuffer++;

	*pbuffer2++ = 0;

	if ( i == '>' ) {
		sprintf( pbuffer2, "<DIR>" );
	}
	else {
		// Store file size
		longTemp1 = *pbuffer++;
		longTemp1 += ( (uint32_t)( *pbuffer++ ) ) << 8;
		longTemp1 += ( (uint32_t)( *pbuffer++ ) ) << 16;
		longTemp1 += ( (uint32_t)( *pbuffer++ ) ) << 24;
		sprintf( pbuffer2, "%ld bytes.", longTemp1 );
	}

	printToStatusBox( filePath, pbuffer2 );

}

void showSDFileInfo() {

	if ( selectedDisplayedEntry >= numDisplayedSDEntries ) {
		return;
	}

	pbuffer = sdList;
	pbuffer += selectedDisplayedEntry * MAX_SD_ENTRY_SIZE;

	// Attributes byte
	i = *pbuffer++;

	// Store name
	pbuffer2 = filePath;
	while( *pbuffer > 0 ) {
		*pbuffer2++ = *pbuffer++;
	}
	*pbuffer2++ = 0;
	pbuffer++;

	// Skip date
	pbuffer+= 4;

	if ( i == ESXDOS_FILE_ATTRIBUTE_DIR_BIT ) {
		sprintf( pbuffer2, "<DIR>" );
	}
	else {
		// Store file size
		longTemp1 = *pbuffer++;
		longTemp1 += ( (uint32_t)( *pbuffer++ ) ) << 8;
		longTemp1 += ( (uint32_t)( *pbuffer++ ) ) << 16;
		longTemp1 += ( (uint32_t)( *pbuffer++ ) ) << 24;
		sprintf( pbuffer2, "%ld bytes.", longTemp1 );
	}

	printToStatusBox( filePath, pbuffer2 );

}

bool getSDFileNameAndSize( uint8_t *path, uint32_t *fileSize, uint8_t *fileOrDirectory ) {

	if ( selectedDisplayedEntry >= numDisplayedSDEntries ) {
		return false;
	}

	pbuffer = sdList;
	pbuffer += selectedDisplayedEntry * MAX_SD_ENTRY_SIZE;

	// Attributes byte
	*fileOrDirectory = *pbuffer++;

	// Store name
	while( *pbuffer > 0 ) {
		*path++ = *pbuffer++;
	}
	*path = 0;
	pbuffer++;

	// Skip date
	pbuffer+= 4;

	if ( *fileOrDirectory != ESXDOS_FILE_ATTRIBUTE_DIR_BIT ) {
		// Store file size
		*fileSize = *pbuffer++;
		*fileSize += ( (uint32_t)( *pbuffer++ ) ) << 8;
		*fileSize += ( (uint32_t)( *pbuffer++ ) ) << 16;
		*fileSize += ( (uint32_t)( *pbuffer++ ) ) << 24;
	}

	return true;
}

bool concatPath( uint8_t *string1, uint8_t *string2, uint16_t maxSize, bool appendSlash ) {

	uint16_t l1 = strlen( string1 );
	uint16_t l2 = strlen( string2 );

	if ( l1 + l2 + ( appendSlash ? 1 : 0 ) > maxSize ) {
		return false;
	}

	string1 += l1;

	while ( l2 > 0 ) {
		*string1++ = *string2++;
		l2--;
	}
	if ( appendSlash ) {
		*string1++='/';
	}
	*string1 = 0;

	return true;

}

void pathUpOneDir( uint8_t *path ) {

	pbuffer = path;
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

uint8_t *shortString( uint8_t *string, int maxSize ) {

	int s = strlen( string );
	if ( s > maxSize ) {
		s -= maxSize;
		string += s;
	}

	return string;

}

void downloadProgressCallback( uint32_t totalBytesRead ) {

	sprintf( diskBuffer, "%lu of %lu bytes.", totalBytesRead, fileSize1 );
	printToStatusBox( "Downloading...", diskBuffer );

}

void uploadProgressCallback( uint32_t totalBytesRead ) {

	sprintf( (char *)diskBuffer, "%lu of %lu bytes.", totalBytesRead, fileSize1 );
	printToStatusBox( "Uploading...", diskBuffer );

}

void activateTurbo() {
	
	TURBO_set( FP_TURBO );

	textUtils_setAttributes( ATTRIBUTES_LIGHTNING );
	textUtils_printAt( X_COORD_SD_LIST - 1, Y_COORD_SD_LIST - 1 );
	fputc_cons( UDG_GRAPHICS_START + UDG_STRIKE );

}

void deactivateTurbo() {


	textUtils_setAttributes( ATTRIBUTES_TITLE_BOX );
	textUtils_printAt( X_COORD_SD_LIST - 1, Y_COORD_SD_LIST - 1 );
	fputc_cons( ' ' );

	TURBO_set( originalTurbo );

}

void infiniteLoop() {

	while ( true ) {

		// Keyboard input
		key = waitKeyPress( 200 );

		showFileInfo = false;

		if ( ( ( key >= 8 ) && ( key <= 13 ) ) || ( key == 'o' ) )  activateTurbo();

		switch ( key ) {
			// Up
			case 11:

				if ( selectedDisplayedEntry == 0 ) {

					if ( selectedTCP_NO_SD == true ) {

						if ( firstDisplayedTCPEntry > 0 ) {

							diff = MAX_DISPLAY_DIR_ENTRIES;
							if ( diff > firstDisplayedTCPEntry ) {
								diff = firstDisplayedTCPEntry;
							}
							firstDisplayedTCPEntry -= diff;

							brightSelection( false );

							refreshTCPListing( true );

							selectedDisplayedEntry = numDisplayedTCPEntries - 1;

							brightSelection( true );

						}

					}
					else {

						if ( firstDisplayedSDEntry > 0 ) {

							diff = MAX_DISPLAY_DIR_ENTRIES;
							if ( diff > firstDisplayedSDEntry ) {
								diff = firstDisplayedSDEntry;
							}
							firstDisplayedSDEntry -= diff;

							brightSelection( false );

							refreshSDListing();

							selectedDisplayedEntry = numDisplayedSDEntries - 1;

							brightSelection( true );

						}

					}
				}
				else {
					brightSelection( false );
					selectedDisplayedEntry--;
					diff = selectedTCP_NO_SD == true ? numDisplayedTCPEntries : numDisplayedSDEntries;
					if ( diff > 0 && selectedDisplayedEntry > diff - 1 ) {
						selectedDisplayedEntry = diff - 1;
					}
					brightSelection( true );
				}
				showFileInfo = true;
				
				break;

			// Down
			case 10:

				if ( selectedDisplayedEntry == MAX_DISPLAY_DIR_ENTRIES - 1 ) {

					if ( selectedTCP_NO_SD == true ) {

						if ( firstDisplayedTCPEntry + MAX_DISPLAY_DIR_ENTRIES < numTotalTCPEntries ) {

							firstDisplayedTCPEntry += MAX_DISPLAY_DIR_ENTRIES;

							refreshTCPListing( true );

							brightSelection( false );

							selectedDisplayedEntry = 0;

							brightSelection( true );

						}

					}
					else {

						if ( firstDisplayedSDEntry + MAX_DISPLAY_DIR_ENTRIES < numTotalSDEntries ) {

							firstDisplayedSDEntry += MAX_DISPLAY_DIR_ENTRIES;

							brightSelection( false );

							refreshSDListing();

							selectedDisplayedEntry = 0;

							brightSelection( true );

						}

					}
				}
				else {
					brightSelection( false );
					selectedDisplayedEntry++;
					diff = selectedTCP_NO_SD == true ? numDisplayedTCPEntries : numDisplayedSDEntries;
					if ( diff > 0 && selectedDisplayedEntry > diff - 1 ) {
						selectedDisplayedEntry = diff - 1;
					}
					brightSelection( true );
				}
				showFileInfo = true;
				
				break;

			// Left and right
			case 8:
			case 9:

				brightSelection( false );
				selectedTCP_NO_SD = selectedTCP_NO_SD == true ? false : true;
				diff = selectedTCP_NO_SD == true ? numDisplayedTCPEntries : numDisplayedSDEntries;
				if ( diff > 0 && selectedDisplayedEntry > diff - 1 ) {
					selectedDisplayedEntry = diff - 1;
				}
				brightSelection( true );
				showFileInfo = true;
				
				break;

			// Intro pressed
			case 13:

				if ( selectedTCP_NO_SD == true ) {

					printToStatusBox( "Reading file info...", NULL );
					diff = rpFile_getFileNameAndSize(
						tcpPath,
						"",
						currentOrderingMethod,
						firstDisplayedTCPEntry + selectedDisplayedEntry,
						diskBuffer,
						FILE_PATH_SIZE,
						&fileSize1,
						&fileOrDirectory1
					);
					if ( diff != RPF_ERROR_OK ) {
						sprintf( diskBuffer, "Code= %d", diff );
						printToStatusBox( "Error reading file info.", diskBuffer );
					}
					else {
						if ( fileOrDirectory1 == '>' ) {
							// Navigate down through directory
							if ( *diskBuffer == '.' && *( diskBuffer + 1 ) == 0 ) {
								// '.' is the same directory, just do nothing
								diff = true;
							}
							else if ( *diskBuffer == '.' && *( diskBuffer + 1 ) == '.' && *( diskBuffer + 2 ) == 0 ) {
								// ".." is up one directory
								pathUpOneDir( tcpPath );
								diff = true;
							}
							else {
								// Regular directory
								diff = concatPath( tcpPath, diskBuffer, TCP_PATH_SIZE, true );
								if ( diff == false ) {
									printToStatusBox( "File path too long.", NULL );
								}
							}
							if ( diff == true ) {
								brightSelection( false );
								firstDisplayedTCPEntry = 0;
								refreshTCPListing( true );
								selectedDisplayedEntry = 0;
								brightSelection( true );
								showFileInfo = true;
							}
						}
						else {

							// Copy SD file path and filename shortened to 8.3 name
							strcpy( filePath, sdPath );
							pbuffer2 = filePath;
							while ( *pbuffer2 > 0 ) pbuffer2++;

							pbuffer3 = diskBuffer;
							j = 0;
							for( i = 0; i < 8 && *pbuffer3 > 0; i++ ) {
								if ( *pbuffer3 == '.' ) {
									j = 1;
									*pbuffer2 = '.';
								}
								else if ( *pbuffer3 == ' ' ) {
									*pbuffer2 = '_';
								}
								else {
									*pbuffer2 = *pbuffer3;
								}
								pbuffer2++;
								pbuffer3++;
							}
							if ( *pbuffer3 > 0 ) {
								i = strlen( pbuffer3 );
								k = 3;
								if ( i < 3 ) {
									k = i;
								}
								pbuffer3 += i - k;
								if ( j == 0 && *( pbuffer3 - 1 ) == '.' ) {
									*pbuffer2++ = '.';
								}
								while ( k > 0 ) {
									*pbuffer2++ = *pbuffer3++;
									k--;
								}
							}
							*pbuffer2 = 0;

							// Download the file with confirmation

							sprintf( diskBuffer, "Confirm DOWNLOAD %lu bytes?", fileSize1 );
							printToStatusBox( diskBuffer, shortString( filePath, 27 ) );
							textUtils_printAt( 27, 23 );
							textUtils_print( "(Y/N)" );

							key2 = waitKey();

							if ( key2 == 'y' || key2 == 'Y' ) {

								printToStatusBox( "Downloading...", NULL );

								diff = rpFile_downloadFile(
									tcpPath,
									"",
									currentOrderingMethod,
									firstDisplayedTCPEntry + selectedDisplayedEntry,
									filePath,
                                    0, // TODO H+ H-
									diskBuffer,
									DISK_BUFFER_SIZE,
									downloadProgressCallback
								);

								if ( diff == RPF_ERROR_OK ) {

									brightSelection( false );
									refreshSDListing();
									brightSelection( true );
									
									printToStatusBox( "File downloaded succesfully.", NULL );

								}
								else {

									sprintf( diskBuffer, "Code: %d", diff );
									printToStatusBox( "Error downloading.", diskBuffer );

								}

							}
							else {

								brightSelection( false );
								refreshTCPListing( false );
								refreshSDListing();
								brightSelection( true );
								
								printToStatusBox( "Aborted by user.", NULL );

							}
						}
					}
				}
				else {
					if ( getSDFileNameAndSize( filePath, &fileSize1, &fileOrDirectory1 ) == true ) {
						if ( fileOrDirectory1 == ESXDOS_FILE_ATTRIBUTE_DIR_BIT ) {
							// Navigate down through directory
							if ( *filePath == '.' && *( filePath + 1 ) == 0 ) {
								// '.' is the same directory, just do nothing
								diff = true;
							}
							else if ( *filePath == '.' && *( filePath + 1 ) == '.' && *( filePath + 2 ) == 0 ) {
								// ".." is up one directory
								pathUpOneDir( sdPath );
								diff = true;
							}
							else {
								// Regular directory
								diff = concatPath( sdPath, filePath, SD_PATH_SIZE, true );
								if ( diff == false ) {
									printToStatusBox( "File path too long.", NULL );
								}
							}
							if ( diff == true ) {
								brightSelection( false );
								firstDisplayedSDEntry = 0;
								refreshSDListing();
								selectedDisplayedEntry = 0;
								brightSelection( true );
								showFileInfo = true;
							}
						}
						else {

							pathLength1 = strlen( sdPath );

							if ( concatPath( sdPath, filePath, SD_PATH_SIZE, false ) == false ) {
								printToStatusBox( "File path too long.", NULL );
								sdPath[ pathLength1 ] = 0;
							}
							else {

								// Upload the file with confirmation

								sprintf( diskBuffer, "Confirm UPLOAD %lu bytes?", fileSize1 );
								printToStatusBox( diskBuffer, shortString( filePath, 27 ) );
								textUtils_printAt( 27, 23 );
								textUtils_print( "(Y/N)" );

								key2 = waitKey();

								if ( key2 == 'y' || key2 == 'Y' ) {

									printToStatusBox( "Uploading...", NULL );

									diff = rpFile_uploadFile(
										tcpPath,
										sdPath,
										filePath,
										0, // TODO H+ H-
										diskBuffer,
										DISK_BUFFER_SIZE,
										fileSize1,
										uploadProgressCallback
									);
									
									sdPath[ pathLength1 ] = 0;

									if ( diff == RPF_ERROR_OK ) {

										brightSelection( false );
										refreshTCPListing( false );
										brightSelection( true );
										
										printToStatusBox( "File uploaded succesfully.", NULL );

									}
									else {

										sprintf( diskBuffer, "Code: %d", diff );
										printToStatusBox( "Error uploading.", diskBuffer );

									}

								}
								else {
									
									sdPath[ pathLength1 ] = 0;

									brightSelection( false );
									refreshTCPListing( false );
									refreshSDListing();
									brightSelection( true );
									
									printToStatusBox( "Aborted by user.", NULL );
									
								}
							}
						}
					}
					else {
						printToStatusBox( "Error obtaining SD file info.", NULL );
					}
				}
				break;

			// Backspace pressed
			case 12:
				brightSelection( false );
				if ( selectedTCP_NO_SD == true ) {
					pathUpOneDir( tcpPath );
					firstDisplayedTCPEntry = 0;
					refreshTCPListing( true );
				}
				else {
					pathUpOneDir( sdPath );
					firstDisplayedSDEntry = 0;
					refreshSDListing();
				}
				selectedDisplayedEntry = 0;
				brightSelection( true );
				showFileInfo = true;
				break;

			case 'o':

				// Change ordering

				currentOrderingMethod ++;
				if ( currentOrderingMethod >= numOrderingMethods ) currentOrderingMethod = 0;
				refreshTCPListing( true );
				brightSelection( true );
				printToStatusBox( "File ordering set to:", orderingMethods[ currentOrderingMethod ] );
				
				break;

		}

		if ( showFileInfo == true ) {
			if ( selectedTCP_NO_SD == true ) {
				showTCPFileInfo();
			}
			else {
				showSDFileInfo();
			}
		}
		
		if ( ( ( key >= 8 ) && ( key <= 13 ) ) || ( key == 'o' ) ) deactivateTurbo();

	}
}
