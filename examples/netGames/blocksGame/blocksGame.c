/*
 * @author yomboprime
 *
 *
 * Simple game in which each client moves a colored character around the screen.
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <input.h>
#include <spectrum.h>

#include "../../../src/textUtils.h"
#include "../../../src/zxuno/turbo.h"
#include "../../../src/retroProt.h"

#define WIFI_SSID "wipi4"
#define WIFI_PASSWD "12341234"
#define SERVER_NAME "192.168.1.100"
#define SERVER_PORT "8092"

#define getKey() in_Inkey()

uint8_t appId[ APP_ID_LENGTH ];
uint8_t roomName[ ROOM_NAME_LENGTH ];
uint8_t returnedRoomName[ ROOM_NAME_LENGTH ];

uint8_t myId;
uint8_t myPosX, myPosY, myAttributes, myCharacter;
uint8_t numClients = 0;
long numRooms = 0;
long roomIndex;
bool roomExists = false;
long longValue;
int c, c2;
uint8_t b0, b1, b2, b3, b4, b5, ib;

void listRoomsCallback( uint8_t numClients, uint8_t maxNumClients, uint8_t numBytesPerTransfer, uint8_t timeout, uint8_t flags );
void terminate();

void main( void ) {

	textUtils_64ColumnsMode();
    textUtils_cls();

	TURBO_set( ZXUNO_TURBO_X2 );

    textUtils_println( "Blocks Game" );
    textUtils_println( "" );
	textUtils_println( "Connecting to WiFi..." );

	if ( initNetwork() != NETWORK_ERROR_OK ) {

		textUtils_println( "Could not communicate with WiFi module." );
		terminate();

	}

	// Connect to WiFi network
	if ( connectToWiFi( WIFI_SSID, WIFI_PASSWD, 25000 ) != NETWORK_ERROR_OK ) {

		textUtils_println( "Error connecting to WiFi." );
		terminate();

	}

	// Connect to the server

	textUtils_println( "Connecting to server..." );

	if ( connectToServer( SERVER_NAME, SERVER_PORT ) != NETWORK_ERROR_OK ) {
		textUtils_println( "Error connecting to server." );
		terminate();
	}

	if ( openServerStream() != NETWORK_ERROR_OK ) {
		textUtils_println( "Error opening data stream to server." );
		terminate();
	}

	textUtils_println( "Connected to server." );
	textUtils_printEOL();

	for ( b0 = 0; b0 < 3; b0 ++ ) {

		textUtils_print( "Pinging server..." );
		b1 = retroProt_ping( 10000, &longValue );
		if ( b1 != RP_ERROR_OK ) {

			textUtils_println( "Error pinging server." );

		}
		else {

			textUtils_print( " Time(ms): " );
			textUtils_println_l( longValue );

		}

	}

	// Set App Id
	c = 0;
	appId[ c ++ ] = 'T';
	appId[ c ++ ] = 'e';
	appId[ c ++ ] = 's';
	appId[ c ++ ] = 't';
	appId[ c ++ ] = ' ';
	appId[ c ++ ] = 'a';
 	appId[ c ++ ] = 'p';
 	appId[ c ++ ] = 'p';
 	appId[ c ++ ] = ' ';
 	appId[ c ++ ] = '1';
	while ( c < APP_ID_LENGTH ) appId[ c ++ ] = 0;

	// Set room name
	c = 0;
	roomName[ c ++ ] = 'R';
	roomName[ c ++ ] = 'o';
	roomName[ c ++ ] = 'o';
	roomName[ c ++ ] = 'm';
	roomName[ c ++ ] = ' ';
	roomName[ c ++ ] = '#';
	roomName[ c ++ ] = '1';
	while ( c < ROOM_NAME_LENGTH ) roomName[ c ++ ] = 0;

	roomExists = false;
	roomIndex = 0;
	if ( retroProt_listRooms( appId, listRoomsCallback, returnedRoomName, &numRooms ) == RP_ERROR_TIMEOUT ) {
		textUtils_println( "Timeout reading server room list." );
		terminate();
	}

	textUtils_print( "\nNumber of rooms: " );
	textUtils_println_l( numRooms );


	// Set my state

	b0 = retroProt_getRandomNumber( &myPosX );
	if ( b0 != RP_ERROR_OK ) terminate();
	myPosX = myPosX % 32;

	b0 = retroProt_getRandomNumber( &myPosY );
	if ( b0 != RP_ERROR_OK ) terminate();
	myPosY = myPosY % 24;

	b0 = retroProt_getRandomNumber( &b1 );
	if ( b0 != RP_ERROR_OK ) terminate();

	b0 = retroProt_getRandomNumber( &b2 );
	if ( b0 != RP_ERROR_OK ) terminate();

	myAttributes = textUtils_createAttributes( false, true, b1, b2 );

	b1 = 0;
	while ( b1 == 0 ) {

		b0 = retroProt_getRandomNumber( &myCharacter );
		if ( b0 != RP_ERROR_OK ) terminate();

		if ( ! ( ( myCharacter >= 'a' && myCharacter <= 'z' ) || ( myCharacter >= 'A' && myCharacter <= 'Z' ) || ( myCharacter >= '0' && myCharacter <= '9' ) ) ) b1 = 0;
		else b1 = 1;

	}

	textUtils_setAttributes( 0 );
	zx_border( 0 );
	textUtils_cls();
	textUtils_32ColumnsMode();

	// Paint avatar
	textUtils_setAttributes( myAttributes );
	textUtils_printAt( myPosX, myPosY );
	fputc_cons( myCharacter );


	if ( ! roomExists ) {

		// Required room does not exist, create room

		b0 = retroProt_createRoom( appId, roomName, 20, 6, 15, 0 );

		if ( b0 != RP_ERROR_OK ) {

			textUtils_printAt( 0, 0 );
			textUtils_setAttributes( textUtils_createAttributes( false, false, 7, 0 ) );
			textUtils_print( "Error creating room: " );
			switch ( b0 )  {
				case RP_ERROR_TIMEOUT: textUtils_println( "Server timed out." ); break;
				case RP_ERROR_MAX_ROOMS_REACHED: textUtils_println( "Maximum number of rooms reached." ); break;
				case RP_ERROR_ROOM_NAME_IN_USE: textUtils_println( "Room name already exists." ); break;
				case RP_ERROR_INVALID_PARAMETER: textUtils_println( "Invalid parameter." ); break;
				default: break;
			}

			terminate();

		}

	}
	else {

		// Found required room, enter into it

		if ( retroProt_enterRoom( roomName ) == RP_ERROR_TIMEOUT ) {

			textUtils_printAt( 0, 0 );
			textUtils_setAttributes( textUtils_createAttributes( false, false, 7, 0 ) );
			textUtils_print( "Error entering room: Server timed out." );
			terminate();

		}

	}

	// Get my client id

	b0 = retroProt_getUniqueId( &myId );

	if ( b0 == RP_ERROR_TIMEOUT ) {
		textUtils_printAt( 0, 0 );
		textUtils_setAttributes( textUtils_createAttributes( false, false, 7, 0 ) );
		textUtils_println( "Timeout obtaining client id." );
		terminate();
	}
	else if ( b0 != 0 ) {

		textUtils_setAttributes( textUtils_createAttributes( false, false, 7, 0 ) );
		switch ( b0 ) {
			case 0x01: textUtils_println( "Error obtaining client id: Client is not in room." ); break;
			// Should not happen. Different protocol version?
			default: textUtils_println( "Error obtaining client id: Bad server response." ); break;
		}

		terminate();

	}

	c = 0;
	while ( c != ' ' ) {

		// Read keyboard
		c = getKey();

		// Send data packet
		UART_writeByte( 0x05 );
		UART_writeByte( myId );
		UART_writeByte( myAttributes );
		UART_writeByte( myCharacter );
		UART_writeByte( myPosX );
		UART_writeByte( myPosY );
		UART_writeByte( c );

		// Wait all clients data packet
		c2 = UART_read_timeout( 1500 );
		if ( c2 < 0 ) {
			textUtils_printAt( 0, 0 );
			textUtils_setAttributes( textUtils_createAttributes( false, false, 7, 0 ) );
			textUtils_println( "Timeout while reading data packet from server." );
			terminate();
		}
		else {

			// Clear screen if num clients changed
			if ( numClients != c2 ) {

				textUtils_setAttributes( 0 );
				textUtils_cls();
				numClients = c2;

			}

			// Read all clients data
			for ( ib = 0; ib < c2; ib ++ ) {

				// Id
				b0 = UART_readBlocking();
				// Attributes
				b1 = UART_readBlocking();
				// Character
				b2 = UART_readBlocking();
				// Pos X
				b3 = UART_readBlocking();
				// Pos Y
				b4 = UART_readBlocking();
				// Pressed key
				b5 = UART_readBlocking();

				// Erase avatar in previous position
				textUtils_setAttributes( 0 );
				textUtils_printAt( b3, b4 );
				fputc_cons( ' ' );

				switch ( b5 ) {

					// Left
					case 8: b3 --; if ( b3 == 255 ) b3 = 31; break;

					// Right
					case 9: b3 ++; if ( b3 == 32 ) b3 = 0; break;

					// Down
					case 10: b4 ++; if ( b4 == 24 ) b4 = 0; break;

					// Up
					case 11: b4 --; if ( b4 == 255 ) b4 = 23; break;

				}

				// If this client is in flash and touches me, interchange flash
				if ( ( b1 & 128 ) && ( myPosX == b3 ) && ( myPosY == b4 ) ) {

					myAttributes |= 128;
					b1 &= 127;

				}

				// If this client is me, update my state
				if ( b0 == myId ) {

					myPosX = b3;
					myPosY = b4;
					myAttributes = b1;

				}

				// Paint avatar in new position
				textUtils_setAttributes( b1 );
				textUtils_printAt( b3, b4 );
				fputc_cons( b2 );

			}

		}

		switch ( c ) {

			default:
				break;

		}

		//delay( 100 );

	}

	// Leave room. Also can be disconnect command.
	retroProt_leaveRoom();

	// Exit server
	closeServerStream();
	closeServerConnection();

	textUtils_printAt( 0, 0 );
	textUtils_setAttributes( textUtils_createAttributes( false, false, 7, 0 ) );
	textUtils_println( "\nProgram terminated." );

	terminate();

}

void listRoomsCallback( uint8_t numClients, uint8_t maxNumClients, uint8_t numBytesPerTransfer, uint8_t timeout, uint8_t flags ) {

	textUtils_print( "\nRoom " );
	textUtils_print_l( ++ roomIndex );
	textUtils_println( ":" );

	textUtils_print( "Room name: " );
	for ( ib = 0; ib < ROOM_NAME_LENGTH; ib ++ ) {

		b0 = returnedRoomName[ ib ];
		if ( ! b0 ) break;

		fputc_cons( b0 );

	}

	textUtils_println( "" );

	if ( retroProt_roomIdEquals( roomName, returnedRoomName ) ) {

		textUtils_println( "Found required room!" );
		roomExists = true;

	}

	textUtils_print( "Number of clients: " );
	textUtils_println_l( numClients );

	textUtils_print( "Max number of clients: " );
	textUtils_println_l( maxNumClients );

	textUtils_print( "Number of bytes per client transmission: " );
	textUtils_println_l( numBytesPerTransfer );

	textUtils_print( "Room timeout in 1/50s of second: " );
	textUtils_println_l( timeout );

	textUtils_print( "Room flags: " );
	textUtils_println_l( flags );

}

void terminate() {

	while ( 1 ) {}

}
