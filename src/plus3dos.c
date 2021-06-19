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

// PLUS3DOS API wrapper functions

#include "plus3dos.h"


#include "textUtils.h"


/*
 * Initializes access to Plus3DOS (does not initialize the system)
 * Returns 0 if no error, else error code in lower byte
 */
uint16_t plus3dos_init() {

	#asm

		// Init +3DOS
		//ld ix, DOS_INIT
		//call plus3dosc
		//jp nc, errorc

		// Disable RAM DISK
/*
		ld ix, DOS_SET_1346
		ld hl,0
		ld d,h
		ld e,d
		call plus3dosc
		jp nc, errorc
*/

		// Returns hl = 0 (no error)
		ld hl, 0
		jp endmainc

	.errorc
		// Returns h = 0xFF, error code in l
		ld h, 255
		ld l, a
		jp endmainc

	.setDOSc
		di
		push	bc
		push	af
		ld	bc, CMR0
		ld	a,(BANKM)
		res	4,a
		or	7
		ld	(BANKM),a
		out	(c),a
		pop	af
		pop	bc
		ret

	.plus3dosc
		call	setDOSc
		ld	(addsc+1),ix
	.addsc
		call	0
	.setBASICc
		di
		push af
		push bc
		ld	bc, CMR0
		ld	a,(BANKM)
		set	4,a
		and	248
		ld	(BANKM),a
		out	(c),a
		pop	bc
		pop	af
		ei
		ret

	.endmainc

	#endasm

}

/*
 * Gets PPlus3DOS version in HL: h = issue, l = version
*/
uint16_t plus3dos_getVersion() {

	#asm

		// Get version
		ld ix, DOS_VERSION
		call plus3dosc

		// Returns h = issue, l = version
		ld l, e
		ld h, d

	#endasm

}

/*
 * Returns size in KBytes or 0xFF in high byte and error code in low byte
*/
uint16_t plus3dos_getFreeSpace( uint8_t driveLetter ) {

	#asm

		// Get sp and skip return address in hl
		ld hl, 2
		add hl, sp

		// First param from the end (driveLetter)
		// de <- (hl)
		ld a, (hl)

		// Get free disk size
		ld ix, DOS_FREE_SPACE
		call plus3dosc

		// Returns size in KBytes in hl or 0xFF in h and error code in l
		jp c, endFreeSpace
		ld h, 255
		ld l, a
	.endFreeSpace

	#endasm

}

/*
 * Open a file.
 * 
 * fileName String terminated in 0xFF
 * fileNumber: Number of file to open
 * accessMode: See ACCESS_MODE_*
 * createAction: See CREATE_ACTION_*
 * openAction: See OPEN_ACTION_*
 * 
 * Returns 0 (no error) or 0xFF in high byte and error code in low byte
 * 
 * Always close opened files when done with them.
 */
uint16_t plus3dos_open( uint8_t *fileName, uint8_t fileNumber, uint8_t accessMode, uint8_t createAction, uint8_t openAction ) {

	// fileName is terminated with 0xFF, not 0x00
	// 0 <= fileNumber <= 15
	// accessMode is one of ACCESS_MODE_*
	// createAction is one of CREATE_ACTION_*
	// openAction is one of OPEN_ACTION_*

	#asm

	.openstart

		// Open file

		// Get sp and skip return address in hl
		ld hl, 2
		add hl, sp

		// First param from the end (openAction)
		ld e, (hl)
		inc hl
		inc hl

		// Second param from the end (createAction)
		ld d, (hl)
		inc hl
		inc hl

		// Third param from the end (accessMode)
		ld c, (hl)
		inc hl
		inc hl

		// Fourth param from the end (fileNumber)
		ld b, (hl)
		inc hl
		inc hl

		// Save b and c
		push bc

		// Last param from the end (fileName)
		ld c, (hl)
		inc hl
		ld b, (hl)
		ld h, b
		ld l, c

		// Restore b and c
		pop bc

		ld ix, DOS_OPEN
		call plus3dosc

		// Returns hl = 0 (no error), or h = 255 (error code in l)

		jp nc, errordosopen
		ld hl, 0
		jp enddosopen

	.errordosopen
		ld h, 255
		ld l, a

	.enddosopen

	#endasm

}

/*
 * Closes a file.
 * 
 * fileNumber: Number of file to close
 * 
 * Returns 0 (no error) or 0xFF in high byte and error code in low byte
*/

uint16_t plus3dos_close( uint8_t fileNumber ) {

	// 0 <= fileNumber <= 15

	#asm

		// Close file

		// Get sp and skip return address in hl
		ld hl, 2
		add hl, sp

		// First param from the end (fileNumber)
		ld b, (hl)

		ld ix, DOS_CLOSE
		call plus3dosc

		// Returns hl = 0 (no error), or h = 255 (error code in l)
		jp nc, errordosclose

		// Stop the motor
		ld ix, DOS_MOTOR_OFF
		call plus3dosc

		ld hl, 0
		jp enddosclose

	.errordosclose
		ld h, 255
		ld l, a

	.enddosclose

	#endasm

}

/*
 * Read a number of bytes from a file
 * buffer: The buffer to store the bytes read.
 * fileNumber: The open file number to read from
 * numBytes: Number of bytes to read.
 * Returns number of unread bytes
 */
uint16_t plus3dos_readBytes( uint8_t *buffer, uint8_t fileNumber, uint16_t numBytes ) {

	// 0 <= fileNumber <= 15

	#asm

		// Read bytes from file

		// Get sp and skip return address in hl
		ld hl, 2
		add hl, sp

		// First param from the end (numBytes)
		// de <- (hl)
		ld e, (hl)
		inc hl
		ld d, (hl)
		inc hl

		// Second param from the end (fileNumber)
		ld b, (hl)
		inc hl
		inc hl
		push bc

		// Third param from the end (buffer)
		ld c, (hl)
		inc hl
		ld b, (hl)
		ld h, b
		ld l, c

		// Restore b
		pop bc

		// Set page
		ld c, 0

		ld ix, DOS_READ
		call plus3dosc

		// Returns hl = 0 (no error), or number of bytes unread

		jp nc, errordosread
		ld hl, 0
		jp enddosread

	.errordosread
		ld h, d
		ld l, e

	.enddosread

	#endasm

}

/*
 * Write a number of bytes to a file
 * buffer: The buffer of bytes to write.
 * fileNumber: The open file number to read from
 * numBytes: Number of bytes to write.
 * Returns the number of bytes remaining unwritten.
 */
uint16_t plus3dos_writeBytes( uint8_t *buffer, uint8_t fileNumber, uint16_t numBytes ) {

	// 0 <= fileNumber <= 15

	#asm

		// Read bytes from file

		// Get sp and skip return address in hl
		ld hl, 2
		add hl, sp

		// First param from the end (numBytes)
		// de <- (hl)
		ld e, (hl)
		inc hl
		ld d, (hl)
		inc hl

		// Second param from the end (fileNumber)
		ld b, (hl)
		inc hl
		inc hl
		push bc

		// Third param from the end (buffer)
		ld c, (hl)
		inc hl
		ld b, (hl)
		ld h, b
		ld l, c

		// Restore b
		pop bc

		// Set page
		ld c, 0

		ld ix, DOS_WRITE
		call plus3dosc

		// Returns hl = 0 (no error), or number of bytes unwritten

		jp nc, errordoswrite
		ld hl, 0
		jp enddoswrite

	.errordoswrite
		ld h, d
		ld l, e

	.enddoswrite

	#endasm

}

/*
 * Deletes a file
 * pathFileName: path including the file name
 * drive: The drive number
 */
/*
void _delete( uint8_t *pathFileName, int16_t drive ) {
*/

/*
 * Read directory entries.
 * fileName File name filter or empty string (terminated in 0xFF)
 * buffer Buffer to be filled with entries. It must be  255 * 13 bytes. The first 13 bytes must be filled with 0s
 * Each entry is 8 bytes name, 3 bytes extension, 2 bytes size (in KB)
 * Returns:
 *   0x00 in high byte and number of entries + 1 (starting in the second entry of the buffer) in low byte, or
 *   0xFF in high byte and error code in low byte.
 * 
 * At most 254 (255 - 1) entries (the maximum) are listed.
 * Always close opened directories when done with them.
 */
uint16_t plus3dos_readDir( uint8_t *fileName, uint8_t *buffer ) {

	// Buffer must be >= 64 * 13 bytes and filled with 0s

	#asm

		// Read catalog (CAT)

		// Get sp and skip return address in hl
		ld hl, 2
		add hl, sp

		// First param from the end (buffer)
		// de <- (hl)
		ld e, (hl)
		inc hl
		ld d, (hl)
		inc hl

		// Second param from the end (fileName)
		ld c, (hl)
		inc hl
		ld b, (hl)
		ld h, b
		ld l, c

		// Ask for 255 entries (Max 254 entries + 1 initial)
		ld b, 255
		// Don't show system files
		ld c, 0

		ld ix, DOS_CAT
		call plus3dosc

		// Returns h = 0 (no error, number of entries in l), or 0xFF (error code in l)

		jp nc, errorb
		ld h, 0
		ld l, b
		jp endmainb

	.errorb
		ld h, 255
		ld l, a

	.endmainb

	#endasm

}

uint32_t plus3dos_getFileSizeBytes( uint8_t *fileName, uint8_t fileNumber, uint8_t *buffer, uint16_t bufferSize, uint8_t openAction ) {

	uint16_t result;
	uint32_t totalBytes = 0;
	
	//result = plus3dos_open( fileName, fileNumber, ACCESS_MODE_EXCLUSIVE_READ, CREATE_ACTION_DONTCREATE, OPEN_ACTION_POSITION_TO_DATA );
	result = plus3dos_open( fileName, fileNumber, ACCESS_MODE_EXCLUSIVE_READ, CREATE_ACTION_DONTCREATE, openAction );
	if ( result != 0 ) return 34;

	while ( result == 0 ) {

		result = plus3dos_readBytes( buffer, fileNumber, bufferSize );
		
		totalBytes += bufferSize;

		if ( result > 0 ) {

			totalBytes -= (uint32_t)result;

		}
		
	}
	
	plus3dos_close( fileNumber );
	
	// Add 128 bytes of file header
//	totalBytes += 128;
	
	return totalBytes;
	
}
