/*
 * @author yomboprime
 *
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <input.h>

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
long numRooms = 0;
long roomIndex;
bool roomExists = false;
long longValue;
int c, c2;
uint8_t b0, b1, b2, b3, ib;

void listRoomsCallback( uint8_t numClients, uint8_t maxNumClients, uint8_t numBytesPerTransfer, uint8_t timeout, uint8_t flags );
void terminate();

void main( void ) {

	textUtils_64ColumnsMode();
    textUtils_cls();

	TURBO_set( ZXUNO_TURBO_X2 );

    textUtils_println( "Network test" );
    textUtils_println( "" );

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

	// TODO remove?
	delay( 50 );

	textUtils_print( "\nNumber of rooms: " );
	textUtils_println_l( numRooms );

	if ( ! roomExists ) {

		// Required room does not exist, create room

		b0 = retroProt_createRoom( appId, roomName, 2, 2, 15, 3 );

		if ( b0 != RP_ERROR_OK ) {

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

		textUtils_println( "Created room." );

	}
	else {

		// Found required room, enter into it

		if ( retroProt_enterRoom( roomName ) == RP_ERROR_TIMEOUT ) {

			textUtils_print( "Error entering room: Server timed out." );
			terminate();

		}

		textUtils_println( "Entered in the room." );

	}

	delay( 100 );

	// Get my client id

	b0 = retroProt_getUniqueId( &myId );

	if ( b0 == RP_ERROR_TIMEOUT ) {
		textUtils_println( "Timeout obtaining client id." );
		terminate();
	}
	else if ( b0 != 0 ) {

		switch ( b0 ) {
			case 0x01: textUtils_println( "Error obtaining client id: Client is not in room." ); break;
			// Should not happen. Different protocol version?
			default: textUtils_println( "Error obtaining client id: Bad server response." ); break;
		}

		terminate();

	}

	while ( 1 ) {

		// Read keyboard
		c = getKey();

		if ( c ) {

			// Send data packet
			UART_writeByte( 0x05 );
			UART_writeByte( myId );
			UART_writeByte( c );

		}
		else {

			// Send empty data packet
			UART_writeByte( 0x06 );

		}

		// Wait data packet
		c2 = UART_read_timeout( 1500 );
		if ( c2 < 0 ) {
			textUtils_println( "Timeout while reading data packet from server." );
			terminate();
		}

		// Read timestamp
		b0 = UART_readBlocking();
		b1 = UART_readBlocking();
		b2 = UART_readBlocking();
		b3 = UART_readBlocking();
		if ( c2 > 0 ) {

			longValue = b0 | ( ((long)b1) << 8 ) | ( ((long)b2) << 16 ) | ( ((long)b3) << 24 );
			textUtils_print( "Time: " );
			textUtils_println_l( longValue );

		}

		// Read extra byte
		b1 = UART_readBlocking();

		if ( c2 > 0 ) {

			// Read all clients data
			for ( ib = 0; ib < c2; ib ++ ) {

				b1 = UART_readBlocking();
				b2 = UART_readBlocking();

				textUtils_print( "Client " );
				textUtils_print_l( b1 );
				textUtils_print( " key: " );
				textUtils_println_l( b2 );

			}

		}

		if ( c == 32 ) break;

		//delay( 100 );

	}

/*
	while ( 1 ) {

		UART_print( "a" );
		UART_read_timeout( 8000 );

// 		c = 0;
// 		while ( c != -1 ) {
//
// 			c = UART_read_timeout( 8000 );
//
// 			if ( c != -1 ) fputc_cons( c );
//
// 		}

	}
*/

	// Leave room. Also can be disconnect command.
	retroProt_leaveRoom();

	// Exit server
	closeServerStream();
	closeServerConnection();

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
