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

#include <string.h>
#include <stdlib.h>
#include <spectrum.h>

uint8_t rpFile_listFiles( uint8_t *tcpPath, uint8_t *searchString, uint8_t ordering, uint8_t *buffer, uint16_t firstEntry, uint16_t maxEntries, uint16_t *numEntries, uint16_t *numTotalEntries, uint8_t maxBytesFileName ) {

	// Send command: "listFiles"
	//
	// Parameters:
	//
	// tcpPath: Path of the directory to show its contents.
	// searchString: Search terms, space-separated.
	// ordering: Order of the file list. See FILE_ORDERING_* in retroProtFile.h
	// buffer: Buffer that will be filled with the file entries. See the Retroprot protocol specification for entry format definition.
	// firstEntry: Entry offset in the directory listing.
	// maxEntries: Maximum number of entries to be shown, starting from offset.
	// numEntries: Returned number of entries.
	// numTotalEntries: Returned total number of entries of the directory.
	// maxBytesFileName: Maximum length in characters of the file name in each entry.
	//
	// Return value:
	//
	// RPF_ERROR_OK if all went well.
	// RPF_ERROR_TIMEOUT if timeout occurred waiting for server response.
	// RPF_ERROR_INVALID_PARAMETER if ordering is not a FILE_ORDERING_*, maxEntries is 0 or maxBytesFileName is 0.
	// RPF_ERROR_BAD_SERVER_RESPONSE if server response was not correct.

	uint8_t b0, b1;
	uint16_t serverNumEntries, serverTotalEntries;
	int c;
	uint8_t *p;
	uint16_t i, j, l;

	if ( ordering > FILE_ORDERING_MAX ) return RPF_ERROR_INVALID_PARAMETER;
	if ( maxEntries == 0 ) return RPF_ERROR_INVALID_PARAMETER;
	if ( maxBytesFileName == 0 ) return RPF_ERROR_INVALID_PARAMETER;

	// Send command byte
	UART_writeByte( 0x00 );

	// Send path
	for ( p = tcpPath; *p != 0; p++ ) UART_writeByte( *p );
	UART_writeByte( 0 );

	// Send search string
	for ( p = searchString; *p != 0; p++ ) UART_writeByte( *p );
	UART_writeByte( 0 );

	// Send ordering type
	UART_writeByte( ordering );

	// Send first entry index
	UART_writeByte( firstEntry & 0xFF );
	UART_writeByte( ( firstEntry & 0xFF00 ) >> 8 );

	// Send max entries
	UART_writeByte( maxEntries & 0xFF );
	UART_writeByte( ( maxEntries & 0xFF00 ) >> 8 );

	// Send maxBytesFileName
	UART_writeByte( maxBytesFileName );

	// Get server response

	c = UART_read_timeout( 1000 );

	if ( c < 0 ) {
		return RPF_ERROR_TIMEOUT;
	}
	else if ( c != 0 ) {

		switch ( c ) {
			case 0x01: return RPF_ERROR_PATH_NOT_FOUND;
			// Should not happen. Different protocol version?
			default: return RPF_ERROR_BAD_SERVER_RESPONSE;
		}

	}

	b0 = UART_readBlocking();
	b1 = UART_readBlocking();
	serverTotalEntries = ((uint16_t)b0) | ( ((uint16_t)b1) << 8 );

	b0 = UART_readBlocking();
	b1 = UART_readBlocking();
	serverNumEntries = ((uint16_t)b0) | ( ((uint16_t)b1) << 8 );

	*numEntries = serverNumEntries;
	*numTotalEntries = serverTotalEntries;

	if ( serverNumEntries == 0 ) {
		return RPF_ERROR_OK;
	}

	p = buffer;

	l = maxBytesFileName + 9;

	for ( i = 0; i < serverNumEntries; i ++ ) {

		for ( j = 0; j < l; j ++ ) {

			*p++ = UART_readBlocking();

		}

	}

	return RPF_ERROR_OK;

}

uint8_t rpFile_getFileNameAndSize( uint8_t *tcpPath, uint8_t *searchString, uint8_t ordering, uint16_t entry, uint8_t *buffer, uint16_t bufferSize, uint32_t *fileSize, uint8_t *fileOrDirectory ) {

	// Send command: "getFileNameAndSize"
	//
	// Parameters:
	//
	// tcpPath: Path of the directory to show its contents.
	// searchString: Search terms, space-separated.
	// ordering: Order of the file list. See FILE_ORDERING_* in retroProtFile.h
	// entry: The entry index to get information of.
	// buffer: Buffer that will be filled with the file name. It will be at most 255 bytes.
	// bufferSize: The provided buffer size in bytes.
	// fileSize: Returned file size (0 for a directory)
	// fileOrDirectory: '>' for a directory, ' ' for a regular file.
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
	uint16_t i, l;

	if ( ordering > FILE_ORDERING_MAX ) return RPF_ERROR_INVALID_PARAMETER;

	// Send command byte
	UART_writeByte( 0x01 );

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
			// Should not happen. Different protocol version?
			default: return RPF_ERROR_BAD_SERVER_RESPONSE;
		}

	}

	*fileOrDirectory = UART_readBlocking();

	p = buffer;
	l = bufferSize;
	if ( l > 255 ) l = 255;
	for( i = 0; i < l && b0 != 0; i ++, p++ ) {
		b0 = UART_readBlocking();
		*p = b0;
	}
	*p = 0;

	b0 = UART_readBlocking();
	b1 = UART_readBlocking();
	b2 = UART_readBlocking();
	b3 = UART_readBlocking();
	*fileSize = b0 | ( ((uint32_t)b1) << 8 ) | ( ((uint32_t)b2) << 16 ) | ( ((uint32_t)b3) << 24 );

	return RPF_ERROR_OK;

}
