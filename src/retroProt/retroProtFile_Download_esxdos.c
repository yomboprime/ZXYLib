#include "retroProtFile.h"
#include "../esxdos.h"

#include <string.h>
#include <stdlib.h>
#include <spectrum.h>

uint8_t rpFile_downloadFile( uint8_t *tcpPath, uint8_t *searchString, uint8_t ordering, uint16_t entry, uint8_t *sdPath, uint8_t processing, uint8_t *diskBuffer, uint16_t diskBufferSize, void (*progressCallback)() ) {

	// Send command: "downloadFile"
	//
	// Parameters:
	//
	// tcpPath: Path of the directory to show its contents.
	// searchString: Search terms, space-separated.
	// ordering: Order of the file list. See FILE_ORDERING_* in retroProtFile.h
	// entry: The entry index to download.
	// sdPath: Path in the local file system.
	// processing: Type of processing to the file that will apply the server
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
	uint16_t bytesInBuffer;
	uint8_t drive;
	uint8_t fileHandle;
	uint8_t *bufPointer;


/*
	uint16_t bytesAvailable;
	int32_t totalBytesRead;

	int16_t bytesRead;
	uint16_t numBytesReadInBuffer;
	uint16_t bytesAvailableBuffer;
	uint8_t *bufPointer;

	uint8_t returnCode;

	uint8_t c;
*/

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
	
	// Send processing type
	UART_writeByte( processing );

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
	drive = ESXDOS_getDefaultDrive();

	iferror {
		return RPF_ERROR_SD_NOT_INSERTED;
	}

	fileHandle = ESXDOS_fopen( sdPath, ESXDOS_FILEMODE_WRITE_CREATE, drive );
	iferror {
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

		if ( bytesInBuffer == diskBufferSize ) {

			ESXDOS_fwrite( diskBuffer, diskBufferSize, fileHandle );
			iferror {
				ESXDOS_fclose( fileHandle );
				return RPF_ERROR_WRITING_FILE;
			}

			bufPointer = diskBuffer;
			bytesInBuffer = 0;
			bytesWritten += diskBufferSize;

			if ( progressCallback != NULL ) progressCallback( bytesWritten );

		}

	}

	if ( bytesInBuffer > 0 ) {

		ESXDOS_fwrite( diskBuffer, bytesInBuffer, fileHandle );
		iferror {
			ESXDOS_fclose( fileHandle );
			return RPF_ERROR_WRITING_FILE;
		}

		bytesWritten += bytesInBuffer;

		if ( progressCallback != NULL ) progressCallback( bytesWritten );

	}

	ESXDOS_fsync( fileHandle );
	ESXDOS_fclose( fileHandle );

	iferror {
		return RPF_ERROR_WRITING_FILE;
	}

	return RPF_ERROR_OK;

}

uint8_t rpFile_uploadFile( uint8_t *tcpPath, uint8_t *sdFullPath, uint8_t *sdFileName, uint8_t processing, uint8_t *diskBuffer, uint16_t diskBufferSize, uint32_t fileSize, void (*progressCallback)() ) {

	// Send command: "uploadFile"
	//
	// Parameters:
	//
	// tcpPath: Path of the remote directory to upload the file to.
	// sdFullPath: Full path with filename in the local file system.
	// processing: Type of processing to the file that will apply the server.
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

	uint8_t drive;
	uint8_t fileHandle;
	
	uint32_t bytesLeft;
	uint32_t bytesRead;
	uint16_t bytesToRead;
	uint16_t result;

	bytesLeft = fileSize;

	// Send command byte
	UART_writeByte( 0x03 );

	// Send path
	for ( p = tcpPath; *p != 0; p++ ) UART_writeByte( *p );
	
	UART_writeByte( '/' );

	// Send fileName
	for ( p = sdFileName; *p != 0; p++ ) UART_writeByte( *p );
	UART_writeByte( 0 );

	// Send file size
	UART_writeByte( (uint8_t)( bytesLeft & 0xFF ) );
	UART_writeByte( (uint8_t)( ( bytesLeft & 0x0000FF00 ) >> 8 ) );
	UART_writeByte( (uint8_t)( ( bytesLeft & 0x00FF0000 ) >> 16 ) );
	UART_writeByte( (uint8_t)( ( bytesLeft & 0xFF000000 ) >> 24 ) );
	
	// Send processing type
	UART_writeByte( processing );

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

	drive = ESXDOS_getDefaultDrive();

	iferror {
		return RPF_ERROR_SD_NOT_INSERTED;
	}

	fileHandle = ESXDOS_fopen( sdFullPath, ESXDOS_FILEMODE_READ, drive );
	iferror {

		return RPF_ERROR_READING_FILE;

	}

	if ( progressCallback != NULL ) {
		progressCallback( 0L );
	}

	bytesRead = 0;

	while ( bytesLeft > 0 ) {

		bytesToRead = diskBufferSize;
		if ( bytesToRead > bytesLeft ) bytesToRead = bytesLeft;
		
		
		result = ESXDOS_fread( diskBuffer, bytesToRead, fileHandle );

		if ( result != bytesToRead ) {

			ESXDOS_fclose( fileHandle );

			return RPF_ERROR_READING_FILE;

		}

		UART_write( diskBuffer, bytesToRead );

		bytesLeft -= (uint32_t)bytesToRead;
		bytesRead += (uint32_t)bytesToRead;

		if ( progressCallback != NULL ) progressCallback( bytesRead );

	}

	ESXDOS_fclose( fileHandle );

	iferror {
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
