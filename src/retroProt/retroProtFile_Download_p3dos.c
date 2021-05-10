/*

ZXLib

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

#include "retroProtFile.h"
#include "../plus3dos.h"

#include <string.h>
#include <stdlib.h>
#include <spectrum.h>

#include "../textUtils.h"

// Implementation for Plus3e

uint8_t rpFile_downloadFile( uint8_t *tcpPath, uint8_t *searchString, uint8_t ordering, uint16_t entry, uint8_t *sdPath, uint8_t *diskBuffer, uint16_t diskBufferSize, void (*progressCallback)() ) {

	// Send command: "downloadFile"
	//
	// Parameters:
	//
	// tcpPath: Path of the directory to download the file from.
	// searchString: Search terms, space-separated.
	// ordering: Order of the file list. See FILE_ORDERING_* in retroProtFile.h
	// entry: The entry index to download.
	// sdPath: Path in the local file system. Terminated with 0xFF.
	// diskBuffer: Temporary buffer to do the disk writing.
	// diskBufferSize: Size of the temporary buffer. For SD writing minimum of 512 bytes is best.
	// callback: will be called periodically with ( uint32_t bytesRead )
	//
	// Return value:
	//
	// RPF_ERROR_OK if all went well.
	// RPF_ERROR_TIMEOUT if timeout occurred waiting for server response.
	// RPF_ERROR_INVALID_PARAMETER if ordering is not a FILE_ORDERING_*
	// RPF_ERROR_BAD_SERVER_RESPONSE if server response was not correct.

	uint8_t b0, b1, b2, b3;
	int c;
	uint8_t *p;

	uint32_t bytesLeft;
	uint32_t bytesWritten;
	uint32_t bytesInBuffer;
	uint8_t drive;
	uint16_t result;
	uint8_t *bufPointer;

	if ( ordering > FILE_ORDERING_MAX ) return RPF_ERROR_INVALID_PARAMETER;

	// Send command byte
	UART_writeByte( 0x02 );

	// Send path
	for ( p = tcpPath; *p != 0; p++ ) UART_writeByte( *p );
	UART_writeByte( 0 );

	// Send search string
	for ( p = searchString; *p != 0; p++ ) UART_writeByte( *p );
	UART_writeByte( 0 );

	// Send ordering type
	UART_writeByte( ordering );

	// Send entry index
	UART_writeByte( entry & 0xFF );
	UART_writeByte( ( entry & 0xFF00 ) >> 8 );

	// Get server response

	c = UART_read_timeout( 1000 );

	if ( c < 0 ) {
		return RPF_ERROR_TIMEOUT;
	}
	else if ( c != 0 ) {

		switch ( c ) {
			case 0x01: return RPF_ERROR_PATH_NOT_FOUND;
			case 0x02: return RPF_ERROR_INDEX_OUT_OF_BOUNDS;
			case 0x03: return RPF_ERROR_CANT_DOWNLOAD_DIRECTORY;
			// Should not happen. Different protocol version?
			default: return RPF_ERROR_BAD_SERVER_RESPONSE;
		}

	}

	b0 = UART_readBlocking();
	b1 = UART_readBlocking();
	b2 = UART_readBlocking();
	b3 = UART_readBlocking();
	bytesLeft = b0 | ( ((uint32_t)b1) << 8 ) | ( ((uint32_t)b2) << 16 ) | ( ((uint32_t)b3) << 24 );
	

	// Open file for writing
	
	result = plus3dos_open( sdPath, 1, ACCESS_MODE_EXCLUSIVE_WRITE, CREATE_ACTION_POINT_TO_HEADER, OPEN_ACTION_OVERWRITE );
	if ( result != 0 ) {

		return RPF_ERROR_WRITING_FILE;

	}

	bytesInBuffer = 0;
	bytesWritten = 0;
	bufPointer = diskBuffer;

	if ( progressCallback != NULL ) {
		progressCallback( 0L );
	}

	while ( bytesLeft > 0 ) {

		*bufPointer++ = UART_readBlocking();
		bytesInBuffer ++;
		bytesLeft --;

		if ( bytesInBuffer == ((uint32_t)diskBufferSize) ) {

			result = plus3dos_writeBytes( diskBuffer, 1, diskBufferSize );
			if ( result > 0 ) {
				
				plus3dos_close( 1 );
				return RPF_ERROR_WRITING_FILE;

			}
			

			bufPointer = diskBuffer;
			bytesInBuffer = 0;
			bytesWritten += diskBufferSize;

			if ( progressCallback != NULL ) progressCallback( bytesWritten );

		}

	}

	if ( bytesInBuffer > 0 ) {

		result = plus3dos_writeBytes( diskBuffer, 1, (uint16_t)bytesInBuffer );
		if ( result > 0 ) {
				
			plus3dos_close( 1 );
			return RPF_ERROR_WRITING_FILE;

		}

		bytesWritten += bytesInBuffer;

		if ( progressCallback != NULL ) progressCallback( bytesWritten );

	}

	result = plus3dos_close( 1 );
	if ( result != 0 ) {

		return RPF_ERROR_WRITING_FILE;

	}

	return RPF_ERROR_OK;

}

uint8_t rpFile_uploadFile( uint8_t *tcpPath, uint8_t *sdFilename, uint8_t *diskBuffer, uint16_t diskBufferSize, uint32_t fileSize, void (*progressCallback)() ) {

	// Send command: "uploadFile"
	//
	// Parameters:
	//
	// tcpPath: Path of the remote directory to upload the file to. Terminated with 0
	// sdFilename: Path in the local file system, terminatetd with 0xFF
	// diskBuffer: Temporary buffer to do the disk writing.
	// diskBufferSize: Size of the temporary buffer. For SD writing minimum of 512 bytes is best.
	// callback: will be called periodically with ( uint32_t bytesWritten )
	//
	// Return value:
	//
	// RPF_ERROR_OK if all went well.
	// RPF_ERROR_TIMEOUT if timeout occurred waiting for server response.
	// RPF_ERROR_BAD_SERVER_RESPONSE if server response was not correct.

	int c;
	uint8_t *p;

	uint32_t bytesLeft;
	uint32_t bytesRead;
	uint16_t bytesToRead;
	uint16_t result;

	// Add 128 bytes for header
	bytesLeft = fileSize;

	// Send command byte
	UART_writeByte( 0x03 );

	// Send path
	for ( p = tcpPath; *p != 0; p++ ) UART_writeByte( *p );
	
	UART_writeByte( '/' );

	// Send fileName
	for ( p = sdFilename; *p != 0xFF; p++ ) UART_writeByte( *p );
	UART_writeByte( 0 );

	// Send file size
	UART_writeByte( bytesLeft & 0xFF );
	UART_writeByte( ( bytesLeft & 0x0000FF00 ) >> 8 );
	UART_writeByte( ( bytesLeft & 0x00FF0000 ) >> 16 );
	UART_writeByte( ( bytesLeft & 0xFF000000 ) >> 24 );

	// Get server response

	c = UART_read_timeout( 1000 );

	if ( c < 0 ) {
		return RPF_ERROR_TIMEOUT;
	}
	else if ( c != 0 ) {

		switch ( c ) {
			case 0x01: return RPF_ERROR_FILE_ALREADY_EXISTS;
			case 0x02: return RPF_ERROR_FILE_SIZE_IS_0;
			case 0x03: return RPF_ERROR_FILE_SIZE_TOO_LARGE;
			case 0x04: return RPF_ERROR_SERVER_CANT_CREATE_FILE;
			// Should not happen. Different protocol version?
			default: return RPF_ERROR_BAD_SERVER_RESPONSE;
		}

	}

	// Open file for reading
	
	result = plus3dos_open( sdFilename, 1, ACCESS_MODE_EXCLUSIVE_READ, CREATE_ACTION_DONTCREATE, OPEN_ACTION_POSITION_TO_HEADER );

	if ( result != 0 ) {

		return RPF_ERROR_READING_FILE;

	}

	if ( progressCallback != NULL ) {
		progressCallback( 0L );
	}

	bytesRead = 0;

	while ( bytesLeft > 0 ) {

		bytesToRead = diskBufferSize;
		if ( bytesToRead > bytesLeft )  bytesToRead = bytesLeft;
		
		result = plus3dos_readBytes( diskBuffer, 1, bytesToRead );
		if ( result > 0 ) {
				
			plus3dos_close( 1 );
			return RPF_ERROR_READING_FILE;

		}

		UART_write( diskBuffer, bytesToRead );

		bytesLeft -= (uint32_t)bytesToRead;
		bytesRead += (uint32_t)bytesToRead;

		if ( progressCallback != NULL ) progressCallback( bytesRead );

	}

	result = plus3dos_close( 1 );
	if ( result != 0 ) {

		return RPF_ERROR_WRITING_FILE;

	}

	// Wait final command end confirmation from server
	c = UART_read_timeout( 2000 );

	if ( c < 0 ) {
		return RPF_ERROR_TIMEOUT;
	}
	else if ( c != 0 ) {

		return RPF_ERROR_BAD_SERVER_RESPONSE;

	}

	return RPF_ERROR_OK;

}
