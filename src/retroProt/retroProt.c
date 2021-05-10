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

#include "retroProt.h"
#include <stdlib.h>
#include <spectrum.h>

long *retroProt_roomsCounter;
bool retroProt_countFullRooms;
uint8_t *retroProt_retRoomName_Dest;
uint8_t retroProt_retRoomName_Src[ ROOM_NAME_LENGTH ];

void retroProt_countRoomsCallback( uint8_t numClients, uint8_t maxNumClients, uint8_t numBytesPerTransfer, uint8_t timeout, uint8_t flags );
void retroProt_searchRandomRoomCallback( uint8_t numClients, uint8_t maxNumClients, uint8_t numBytesPerTransfer, uint8_t timeout, uint8_t flags );

uint8_t retroProt_listRooms( uint8_t *appId, void (*callback)(), uint8_t *returnedRoomName, long *numberOfRooms ) {

	// Send command: "listRooms"
	//
	// Parameters:
	//
	// appId: uint8_t appId[ APP_ID_LENGTH ] with the app Id to filter with.
	// callback: Optional function (can be null): void callback( uint8_t numClients, uint8_t maxNumClients, uint8_t numBytesPerTransfer, uint8_t timeout, uint8_t flags ) { ... }
	// The callback will be called once for each existing room, with the parameters:
	// 	- numClients: Number of current clients in the room.
	// 	- maxNumClients: Maximum number of current clients in the room.
	// 	- numBytesPerTransfer: Number of bytes to transfer from client to server in a single loop.
	// 	- timeout: Room timeout in 1/50s of a second.
	// 	- flags: See RetroProt documentation for room flags meaning.
	// returnedRoomName: Optional user provided buffer of uint8_t[ ROOM_NAME_LENGTH ] that will be filled with the room name just before each call to the callback.
	// numberOfRooms: Optional pointer, returned number of rooms counted. If it is not NULL it will be filled with the number of times the callback has been called.
	//
	// Return value:
	//
	// RP_ERROR_OK if all went well.
	// RP_ERROR_TIMEOUT if timeout occurred waiting for server response.

	int c;
	uint8_t b0, b1, b2, b3, ib;
	long longValue;
	long il;

	uint8_t numClients;
	uint8_t maxNumClients;
	uint8_t numBytesPerTransfer;
	uint8_t timeout;
	uint8_t flags;

	UART_writeByte( 0x00 );
	UART_write( appId, APP_ID_LENGTH );

	c = UART_read_timeout( 1000 );
	if ( c < 0 ) return RP_ERROR_TIMEOUT;

	b0 = c;
	b1 = UART_readBlocking();
	b2 = UART_readBlocking();
	b3 = UART_readBlocking();
	longValue = b0 | ( ((long)b1) << 8 ) | ( ((long)b2) << 16 ) | ( ((long)b3) << 24 );
	if ( numberOfRooms ) *numberOfRooms = longValue;

	for ( il = 0; il < longValue; il ++ ) {

		for ( ib = 0; ib < ROOM_NAME_LENGTH; ib ++ ) {

			b0 = UART_readBlocking();
			if ( returnedRoomName ) returnedRoomName[ ib ] = b0;

		}

		numClients = UART_readBlocking();
		maxNumClients = UART_readBlocking();
		numBytesPerTransfer = UART_readBlocking();
		timeout = UART_readBlocking();
		flags = UART_readBlocking();

		callback( numClients, maxNumClients, numBytesPerTransfer, timeout, flags );

	}

	return RP_ERROR_OK;

}

uint8_t retroProt_createRoom( uint8_t *appId, uint8_t *roomName, uint8_t maxClients, uint8_t numBytesPerTransfer, uint8_t timeout, uint8_t flags ) {

	// Send command: "createRoom"
	//
	// Parameters:
	//
	// appId: uint8_t appId[ APP_ID_LENGTH ] with the app Id that the clients will be running on this room.
	// roomName: Buffer of uint8_t[ ROOM_NAME_LENGTH ] with the name of the room, 0-terminated.
	// maxClients: Maximum number of clients the room will support, from 2 to 255 included.
	// numBytesPerTransfer: Number of bytes to be transferred to the server in each "applicationData" command transfer.
	// timeout: Room timeout in 1/50s of a second.
	// flags: Room flags, See RetroProt documentation for room flags meaning.
	//
	// Return value:
	//
	// RP_ERROR_OK if the room was created OK and the client entered the room.
	// RP_ERROR_TIMEOUT if timeout occurred waiting for server response.
	// RP_ERROR_INVALID_PARAMETER if maxClient is < 2, or numBytesPerTransfer is 0, or if timeout is 0, or given flags value is not supported.
	// RP_ERROR_MAX_ROOMS_REACHED if room could not be created becuse the server maximum number of rooms was reached.
	// RP_ERROR_BAD_SERVER_RESPONSE if server response was not correct.

	int c;

	if ( maxClients < 2 ) return RP_ERROR_INVALID_PARAMETER;
	if ( numBytesPerTransfer == 0 ) return RP_ERROR_INVALID_PARAMETER;
	if ( timeout == 0 ) return RP_ERROR_INVALID_PARAMETER;

	UART_writeByte( 0x01 );

	UART_write( appId, APP_ID_LENGTH );
	UART_write( roomName, ROOM_NAME_LENGTH );

	// Max clients in room
	UART_writeByte( maxClients );

	// Num bytes per transfer
	UART_writeByte( numBytesPerTransfer );

	// Let time to the ESP8266 to consume its internal 64 byte UART buffer.
	delay( 10 );

	// Timeout in 1/50s of second
	UART_writeByte( timeout );

	// Flags
	UART_writeByte( flags );

	c = UART_read_timeout( 2500 );

	if ( c < 0 ) {
		return RP_ERROR_TIMEOUT;
	}
	else if ( c != 0 ) {

		switch ( c ) {
			case 0x01: return RP_ERROR_MAX_ROOMS_REACHED;
			case 0x02: return RP_ERROR_ROOM_NAME_IN_USE;
			case 0x03: return RP_ERROR_INVALID_PARAMETER;
			case 0x04: return RP_ERROR_INVALID_PARAMETER;
			case 0x05: return RP_ERROR_INVALID_PARAMETER;
			case 0x06: return RP_ERROR_INVALID_PARAMETER;
			// Should not happen. Different protocol version?
			default: return RP_ERROR_BAD_SERVER_RESPONSE;
		}

	}

	return RP_ERROR_OK;

}

bool retroProt_roomIdEquals( uint8_t *roomName1, uint8_t *roomName2 ) {

	// Check if two room names are equal.
	//
	// Parameters:
	//
	// room1, room2: The two room names to check.
	//
	// Return value:
	//
	// true if and only if the names are equal.

	uint8_t i, c1, c2;

	for ( i = 0; i < ROOM_NAME_LENGTH; i ++ ) {

		c1 = *roomName1;
		c2 = *roomName2;

		if ( c1 != c2 ) return false;

		if ( ! c1 ) return true;

	}

	return true;

}

uint8_t retroProt_enterRoom( uint8_t * roomName ) {

	// Send command: "enterRoom"
	//
	// Parameters:
	//
	// roomName: Buffer of uint8_t[ ROOM_NAME_LENGTH ] with the name of the room, 0-terminated.
	//
	// Return value:
	//
	// RP_ERROR_OK if the room was created OK and the client entered the room.
	// RP_ERROR_TIMEOUT if timeout occurred waiting for server response.
	// RP_ERROR_BAD_SERVER_RESPONSE if server response was not correct.

	int c;

	UART_writeByte( 0x02 );
	UART_write( roomName, ROOM_NAME_LENGTH );

	c = UART_read_timeout( 2500 );

	if ( c < 0 ) {
		return RP_ERROR_TIMEOUT;
	}
	else if ( c != 0 ) {

		switch ( c ) {
			case 0x01: return RP_ERROR_ROOM_NOT_FOUND;
			case 0x02: return RP_ERROR_MAX_CLIENTS_REACHED;
			// Should not happen. Different protocol version?
			default: return RP_ERROR_BAD_SERVER_RESPONSE;
		}

	}

	return RP_ERROR_OK;

}

uint8_t retroProt_leaveRoom() {

	// Send command: "leaveRoom"
	//
	// Parameters:
	//
	// roomName: Buffer of uint8_t[ ROOM_NAME_LENGTH ] with the name of the room, 0-terminated.
	//
	// Return value:
	//
	// RP_ERROR_OK if the room was created OK and the client entered the room.
	// RP_ERROR_TIMEOUT if timeout occurred waiting for server response.
	// RP_ERROR_BAD_SERVER_RESPONSE if server response was not correct.

	int c;

	UART_writeByte( 0x03 );

	c = UART_read_timeout( 2500 );

	if ( c < 0 ) {
		return RP_ERROR_TIMEOUT;
	}
	else if ( c != 0 ) {

		switch ( c ) {
			case 0x01: return RP_ERROR_CLIENT_HAS_NO_ROOM;
			// Should not happen. Different protocol version?
			default: return RP_ERROR_BAD_SERVER_RESPONSE;
		}

	}

	return RP_ERROR_OK;

}

void retroProt_disconnect() {

	// Send command: "disconnect"
	//
	// The server closes the connection.

	UART_writeByte( 0x04 );

}

uint8_t retroProt_resetTimer() {

	// Send command: "resetTimer"
	//
	// Parameters: None
	//
	// Return value:
	//
	// RP_ERROR_OK if the timer was reset successfully.
	// RP_ERROR_CLIENT_HAS_NO_ROOM if no timer was reset because the client is not in a room.
	// RP_ERROR_TIMEOUT if timeout occurred waiting for server response.
	// RP_ERROR_BAD_SERVER_RESPONSE if server response was not correct.

	int c;

	UART_writeByte( 0x07 );

	c = UART_read_timeout( 1000 );

	if ( c < 0 ) {
		return RP_ERROR_TIMEOUT;
	}
	else if ( c != 0 ) {

		switch ( c ) {
			case 0x01: return RP_ERROR_CLIENT_HAS_NO_ROOM;
			// Should not happen. Different protocol version?
			default: return RP_ERROR_BAD_SERVER_RESPONSE;
		}

	}

	return RP_ERROR_OK;

}

uint8_t retroProt_getUniqueId( uint8_t *id ) {

	// Send command: "getUniqueId"
	//
	// Parameters:
	//
	// id: The returned unique id from 0 to 255
	//
	// Return value:
	//
	// RP_ERROR_OK if the id obtained correctly.
	// RP_ERROR_TIMEOUT if timeout occurred waiting for server response.
	// RP_ERROR_BAD_SERVER_RESPONSE if server response was not correct.

	int c;

	UART_writeByte( 0x08 );

	c = UART_read_timeout( 1000 );

	if ( c < 0 ) {
		return RP_ERROR_TIMEOUT;
	}
	else if ( c != 0 ) {

		switch ( c ) {
			case 0x01: return RP_ERROR_CLIENT_HAS_NO_ROOM;
			// Should not happen. Different protocol version?
			default: return RP_ERROR_BAD_SERVER_RESPONSE;
		}

	}

	*id = UART_readBlocking();

	return RP_ERROR_OK;

}

uint8_t retroProt_ping( long maxTimeout, long *pingTime ) {

	// Send command: "ping"
	//
	// Parameters:
	//
	// maxTimeout: Maximum time in milliseconds to wait for the ping
	// pingTime: Returned ping time in milliseconds.
	//
	// Return value:
	//
	// RP_ERROR_OK if the ping was successful and pingTime contains the ping time in milliseconds.
	// RP_ERROR_TIMEOUT if timeout occurred waiting for server response.
	// RP_ERROR_BAD_SERVER_RESPONSE if server response was not correct.

	long t0, t1;
	int c;

	t0 = millis();

	UART_writeByte( 0x09 );
	c = UART_read_timeout( maxTimeout );

	t1 = millis();

	if ( c < 0 ) return RP_ERROR_TIMEOUT;

	if ( c != 0 ) return RP_ERROR_BAD_SERVER_RESPONSE;

	*pingTime = t1 - t0;

	return RP_ERROR_OK;

}

uint8_t retroProt_getRandomNumber( uint8_t *number ) {

	// Send command: "rand"
	//
	// Parameters:
	//
	// number: The returned random number from 0 to 255
	//
	// Return value:
	//
	// RP_ERROR_OK if the random number was obtained correctly.
	// RP_ERROR_TIMEOUT if timeout occurred waiting for server response.

	int c;

	UART_writeByte( 0x0A );

	c = UART_read_timeout( 2500 );

	if ( c < 0 ) {
		return RP_ERROR_TIMEOUT;
	}

	*number = c;

	return RP_ERROR_OK;

}

uint8_t retroProt_countRooms( uint8_t *appId, long *numberOfRooms, bool countFullRooms ) {

	// Get count of number of rooms in the server with the given application id.
	//
	// Parameters:
	//
	// appId: appId to filter by.
	// numberOfRooms: Returned number of rooms.
	// countFullRooms: If set to true, rooms which are full (maximum number of clients was reached) will also be counted.
	//
	// Return value:
	// The same as the retroProt_listRooms function.

	retroProt_roomsCounter = numberOfRooms;
	retroProt_countFullRooms = countFullRooms;
	return retroProt_listRooms( appId, retroProt_countRoomsCallback, NULL, NULL );

}

void retroProt_countRoomsCallback( uint8_t numClients, uint8_t maxNumClients, uint8_t numBytesPerTransfer, uint8_t timeout, uint8_t flags ) {

	if ( retroProt_countFullRooms || numClients < maxNumClients ) ( *retroProt_roomsCounter ) ++;

}

uint8_t retroProt_enterRandomRoom( uint8_t *appId, uint8_t *returnedRoomName ) {

	// Enter a random room among the server current rooms with the given application id.
	//
	// Parameters:
	//
	// appId: uint8_t appId[ APP_ID_LENGTH ] with the app Id to filter with the possible target room to enter.
	// returnedRoomName: User provided buffer of uint8_t[ ROOM_NAME_LENGTH ] that will be filled with the room name just entered.
	//
	// Return value:
	//
	// RP_ERROR_OK if all went well, the client entered a room.
	// RP_ERROR_TIMEOUT if timeout occurred waiting for server response.
	// RP_ERROR_ROOM_NOT_FOUND If no room was found with given appId.
	// RP_ERROR_ROOM_WAS_REMOVED If the selected random room was removed before the client could enter it (returnedRoomName is valid though)
	// RP_ERROR_BAD_SERVER_RESPONSE if server response was not correct while trying to enter the room (returnedRoomName is valid though)

	int c;
	uint8_t b0, b1, b2, b3, br;
	long *numberOfRooms = 0;
	long randomNumber;

	b0 = retroProt_countRooms( appId, numberOfRooms, false );
	if ( b0 != RP_ERROR_OK ) return b0;

	if ( numberOfRooms == 0 ) return RP_ERROR_ROOM_NOT_FOUND;

	// Get a 4 byte random number

	br = retroProt_getRandomNumber( &b0 );
	if ( b1 != RP_ERROR_OK ) return br;
	br = retroProt_getRandomNumber( &b1 );
	if ( b1 != RP_ERROR_OK ) return br;
	br = retroProt_getRandomNumber( &b2 );
	if ( b1 != RP_ERROR_OK ) return br;
	br = retroProt_getRandomNumber( &b3 );
	if ( b1 != RP_ERROR_OK ) return br;

	randomNumber = b0 | ( ((long)b1) << 8 ) | ( ((long)b2) << 16 ) | ( ((long)b3) << 24 );

	*retroProt_roomsCounter = randomNumber % ( *numberOfRooms );

	retroProt_retRoomName_Dest = returnedRoomName;

	br = retroProt_listRooms( appId, retroProt_searchRandomRoomCallback, retroProt_retRoomName_Src, NULL );
	if ( br != RP_ERROR_OK ) return br;

	if ( ( *retroProt_roomsCounter ) != -1 ) return RP_ERROR_ROOM_WAS_REMOVED;

	return retroProt_enterRoom( returnedRoomName );

}

void retroProt_searchRandomRoomCallback( uint8_t numClients, uint8_t maxNumClients, uint8_t numBytesPerTransfer, uint8_t timeout, uint8_t flags ) {

	uint8_t ib;

	if ( numClients < maxNumClients ) {

		if ( retroProt_roomsCounter == 0 ) {

			for ( ib = 0; ib < ROOM_NAME_LENGTH; ib ++ ) retroProt_retRoomName_Dest[ ib ] = retroProt_retRoomName_Src[ ib ];

			retroProt_roomsCounter = -1;

		}
		else if ( retroProt_roomsCounter > 0 ) retroProt_roomsCounter --;

	}

}
