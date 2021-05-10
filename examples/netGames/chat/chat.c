/*
 * @author yomboprime
 *
 * Simple chat
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <input.h>

#include "../../../src/textUtils.h"
#include "../../../src/zxuno/turbo.h"
#include "../../../src/retroProt.h"

#define WIFI_SSID "wipi4"
#define WIFI_PASSWD "12341234"
#define SERVER_NAME "192.168.1.100"
#define SERVER_PORT "8092"

#define NICKNAME_LENGTH 20
#define MAX_MESSAGE_LENGTH 234

#define MAX_ROOMS_LISTED 10
#define SERVER_DATA_TIMEOUT 1000

#define getKey() in_Inkey()

uint8_t appId[ APP_ID_LENGTH ];
uint8_t roomName[ ROOM_NAME_LENGTH ];
uint8_t returnedRoomName[ ROOM_NAME_LENGTH ];

uint8_t myAttributes, myId;
uint8_t myNickname[ NICKNAME_LENGTH ];
uint8_t messageBuffer[ ROOM_NAME_LENGTH * MAX_ROOMS_LISTED /*MAX_MESSAGE_LENGTH*/ ];
uint8_t numCharactersInBuffer = 0;

uint8_t whiteColorBlackBackground;
uint8_t whiteColorBlueBackground;

long numRooms = 0;
long roomIndex;

long selectedRoom;
bool roomExists = false;
long longValue;
int c, c2, c3;
uint8_t b0, b1, b2, b3, b4, ib;
uint8_t prevChar = 0;

int numClientsInRoom = 0;

bool doExit;

void listRoomsCallback( uint8_t numClients, uint8_t maxNumClients, uint8_t numBytesPerTransfer, uint8_t timeout, uint8_t flags );
void terminate();

void main( void ) {

	whiteColorBlackBackground = textUtils_createAttributes( false, false, 0, 7 );
	whiteColorBlueBackground = textUtils_createAttributes( false, true, 1, 7 );


	textUtils_64ColumnsMode();
	textUtils_setAttributes( whiteColorBlackBackground );
    textUtils_cls();
	textUtils_printAt( 0, 0 );

	TURBO_set( ZXUNO_TURBO_X2 );

    textUtils_println( "\n\n\nChat Application for RetroProt server." );
    textUtils_printEOL();

	textUtils_print( "Initing WiFi module..." );

	if ( initNetwork() != NETWORK_ERROR_OK ) {

		textUtils_println( "\nCould not communicate with WiFi module." );
		terminate();

	}

	textUtils_println( "OK" );
	textUtils_print( "Connecting to WiFi..." );

	// Connect to WiFi network
	if ( connectToWiFi( WIFI_SSID, WIFI_PASSWD, 25000 ) != NETWORK_ERROR_OK ) {

		textUtils_println( "\nError connecting to specified WiFi." );
		terminate();

	}

	textUtils_println( "OK" );

	// Connect to the server

	textUtils_print( "Connecting to server..." );

	if ( connectToServer( SERVER_NAME, SERVER_PORT ) != NETWORK_ERROR_OK ) {
		textUtils_println( "Error connecting to server." );
		terminate();
	}

	if ( openServerStream() != NETWORK_ERROR_OK ) {
		textUtils_println( "Error opening data stream to server." );
		terminate();
	}

	textUtils_println( "OK\n" );

	// Get a random color for the nickname
	b0 = retroProt_getRandomNumber( &b1 );
	if ( b0 != RP_ERROR_OK ) {

		textUtils_println( "Timeout obtaining random number from server." );
		terminate();

	}
	// Paper and ink are inverted for complementary colors
	myAttributes = textUtils_createAttributes( false, true, b1, ~ b1 );

	// Ask user for a nickname
	b1 = 0;
	while ( b1 == 0 ) {

		textUtils_print( "\nPlease enter your nickname (max 20 chars): " );

		b2 = 0;
		ib = 0;
		while ( b2 == 0 ) {

			b0 = 0;
			while ( b0 == 0 ) b0 = getKey();
			while ( getKey() );

			if ( textUtils_isReadable( b0 ) ) {

				if ( ib < NICKNAME_LENGTH ) {

					myNickname[ ib ++ ] = b0;
					fputc_cons( b0 );

				}

			}
			else if ( b0 == 13 ) b2 = 1;

		}

		if ( ib > 0 ) b1 = 1;

	}

	for ( ; ib < NICKNAME_LENGTH; ib ++ ) myNickname[ ib ] = 0;
	textUtils_printEOL();


	// Set App Id
	c = 0;
	appId[ c ++ ] = 'C';
	appId[ c ++ ] = 'h';
	appId[ c ++ ] = 'a';
	appId[ c ++ ] = 't';
	appId[ c ++ ] = '_';
	appId[ c ++ ] = 'v';
	appId[ c ++ ] = '1';
	appId[ c ++ ] = '.';
	appId[ c ++ ] = '0';
	while ( c < APP_ID_LENGTH ) appId[ c ++ ] = 0;

	while ( 1 ) {

		// Room name
		c = 0;
		while ( c < ROOM_NAME_LENGTH ) roomName[ c ++ ] = 0;


		// List rooms. Get their names in messageBuffer temporarily.

		textUtils_println( "Listing first 10 chat rooms:" );

		roomExists = false;
		roomIndex = 0;
		if ( retroProt_listRooms( appId, listRoomsCallback, returnedRoomName, &numRooms ) == RP_ERROR_TIMEOUT ) {
			textUtils_println( "Timeout reading server room list." );
			terminate();
		}

		// TODO remove?
		delay( 50 );

		textUtils_print( "\nTotal rooms: " );
		textUtils_println_l( numRooms );

		if ( numRooms > MAX_ROOMS_LISTED ) numRooms = MAX_ROOMS_LISTED;

		selectedRoom = -1;
		if ( numRooms > 0 ) {

			// Ask the user for a room to enter, or to create one

			b1 = 0;
			while ( b1 == 0 ) {

				textUtils_print( "\nEnter room number (0-" );
				textUtils_print_l( numRooms - 1 );
				textUtils_print( ") or 'n' to create new room: " );

				b2 = 0;
				while ( b2 == 0 ) {

					b0 = 0;
					while ( b0 == 0 ) b0 = getKey();
					if ( textUtils_isDigit( b0 ) ) fputc_cons( b0 );
					while ( getKey() );

					if ( textUtils_isDigit( b0 ) || b0 == 'n' ) b2 = 1;

				}

				if ( textUtils_isDigit( b0 ) && b0 <= '0' + numRooms - 1 ) {

					selectedRoom = b0 - '0';
					b1 = 1;

				}
				else if ( b0 == 'n' ) b1 = 1;

			}

			textUtils_printEOL();

		}

		if ( selectedRoom >= 0 ) {

			// Get selected room name

			longValue = ROOM_NAME_LENGTH * selectedRoom;

			for ( ib = 0; ib < ROOM_NAME_LENGTH; ib ++ ) {

				roomName[ ib ] = messageBuffer[ longValue ++ ];

			}

			roomExists = true;

		}
		else {

			// Ask user for name of new room
			b1 = 0;
			while ( b1 == 0 ) {

				textUtils_print( "\nEnter new room name (max 32 chars): " );

				b2 = 0;
				ib = 0;
				while ( b2 == 0 ) {

					b0 = 0;
					while ( b0 == 0 ) b0 = getKey();
					while ( getKey() );

					if ( textUtils_isReadable( b0 ) ) {

						if ( ib < ROOM_NAME_LENGTH ) {

							roomName[ ib ++ ] = b0;
							fputc_cons( b0 );

						}

					}
					else if ( b0 == 13 ) b2 = 1;

				}

				if ( ib > 0 ) b1 = 1;

			}

			for ( ; ib < ROOM_NAME_LENGTH; ib ++ ) roomName[ ib ] = 0;
			roomExists = false;
			textUtils_printEOL();

		}

		if ( ! roomExists ) {

			// Required room does not exist, create room

			b0 = retroProt_createRoom( appId, roomName, 50, 1 + NICKNAME_LENGTH + MAX_MESSAGE_LENGTH, 50, 2 );

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

			textUtils_print( "Created room: " );
			for ( ib = 0; ib < ROOM_NAME_LENGTH; ib ++ ) {

				b0 = roomName[ ib ];
				if ( b0 ) fputc_cons( b0 );
				else break;

			}
			textUtils_printEOL();

			textUtils_println( "You are the owner and moderator of the chat room. If things go  out of control, disconnect. The room will be removed." );

		}
		else {

			// Enter into room

			if ( retroProt_enterRoom( roomName ) == RP_ERROR_TIMEOUT ) {

				textUtils_print( "Error entering room: Server timed out." );
				terminate();

			}

			textUtils_print( "Entered in the room: " );
			for ( ib = 0; ib < ROOM_NAME_LENGTH; ib ++ ) {

				b0 = roomName[ ib ];
				if ( b0 ) fputc_cons( b0 );
				else break;

			}
			textUtils_printEOL();

		}

		textUtils_println( "You can now type messages. To exit type 'exit'." );

		// Get my client id

		b0 = retroProt_getUniqueId( &myId );

		if ( b0 == RP_ERROR_TIMEOUT ) {
			textUtils_println( "Timeout obtaining client id." );
			terminate();
		}
		else if ( b0 != 0 ) {

			textUtils_println( "Error obtaining client id." );
			terminate();

		}

		while ( 1 ) {

			numClientsInRoom = -1;

			// Read keyboard
			c = getKey();

			if ( prevChar == 0 && c == 13 && numCharactersInBuffer > 0 ) {

				// A message is in messageBuffer and <enter> was just pressed

				if ( strncmp( messageBuffer, "exit", 4 ) == 0 ) break;

				// Send data packet command
				UART_writeByte( 0x05 );

				// Send attributes byte
				UART_writeByte( myAttributes );

				// Send nickname
				b0 = 0;
				for ( ib = 0; ib < NICKNAME_LENGTH; ib ++ ) {

					if ( b0 ) UART_writeByte( 0 );
					else {

						b1 = myNickname[ ib ];

						UART_writeByte( b1 );

						if ( ! b1 ) b0 = 1;

					}


				}

				// Send text message
				b0 = 0;
				for ( ib = 0; ib < MAX_MESSAGE_LENGTH; ib ++ ) {

					if ( b0 ) UART_writeByte( 0 );
					else {

						if ( ib >= numCharactersInBuffer ) b1 = 0;
						else b1 = messageBuffer[ ib ];

						UART_writeByte( b1 );

						if ( ! b1 ) b0 = 1;

					}

				}

				// Erase user message from screen
				textUtils_printAt( 0, 0 );
				textUtils_setAttributes( whiteColorBlackBackground );
				for ( b0 = 0; b0 < numCharactersInBuffer; b0 ++ ) fputc_cons( ' ' );
				numCharactersInBuffer = 0;

			}
			else {

				// Send empty data packet command
				UART_writeByte( 0x06 );

			}

			if ( textUtils_isReadable( c ) && prevChar == 0  ) {

				if ( numCharactersInBuffer < MAX_MESSAGE_LENGTH ) {

					messageBuffer[ numCharactersInBuffer ++ ] = c;

				}

			}
			else if ( c == 12 ) {

				// Pressed backspace, delete character

				// Paint black rest of line
				c3 = numCharactersInBuffer + 2;
				textUtils_setAttributes( whiteColorBlackBackground );
				textUtils_printAt( 0, 0 );
				for ( b0 = 0; b0 < c3; b0 ++ ) fputc_cons( ' ' );

				if ( numCharactersInBuffer > 0 ) numCharactersInBuffer --;

			}

			prevChar = c;

			// Wait all clients data packet
			numClientsInRoom = 0;
			c2 = UART_read_timeout( SERVER_DATA_TIMEOUT );
			if ( c2 < 0 ) break;

			// Read extra byte
			//b1 = UART_readBlocking();
			c = UART_read_timeout( SERVER_DATA_TIMEOUT );
			if ( c < 0 ) break;
			numClientsInRoom = c2 + c;
			if ( numClientsInRoom == 0 ) break;

			// Read data from all clients that sent data
			for ( ib = 0; ib < c2; ib ++ ) {

				// Attributes byte
				//b0 = UART_readBlocking();
				c = UART_read_timeout( SERVER_DATA_TIMEOUT );
				if ( c < 0 ) { doExit = true; break; }

				// Scroll screen
				textUtils_setAttributes( whiteColorBlackBackground );
				textUtils_printAt( 63, 23 );
				fputc_cons( ' ' );

				// Print nickname
				textUtils_setAttributes( c );

				b2 = 0;
				b3 = 0;
				for ( b0 = 0; b0 < NICKNAME_LENGTH; b0 ++ ) {

					//b1 = UART_readBlocking();
					c = UART_read_timeout( SERVER_DATA_TIMEOUT );
					if ( c < 0 ) { doExit = true; break; }

					if ( ( c != 0 ) && b2 == 0 ) {
						fputc_cons( c );
						b3 ++;
					}
					else b2 = 1;

				}
				if ( doExit ) break;

				fputc_cons( ':' );
				textUtils_setAttributes( whiteColorBlackBackground );
				fputc_cons( ' ' );

				// Print message
				b2 = 0;
				b4 = 0;
				for ( b0 = 0; b0 < MAX_MESSAGE_LENGTH; b0 ++ ) {

					//b1 = UART_readBlocking();
					c = UART_read_timeout( SERVER_DATA_TIMEOUT );
					if ( c < 0 ) { doExit = true; break; }

					if ( ( c != 0 ) && b2 == 0 ) {
						fputc_cons( c );
						b4 ++;
					}
					else b2 = 1;

				}
				if ( doExit ) break;

				// Paint black rest of line
				c = b3 + b4 + 2;
				c = c % 64;
				textUtils_setAttributes( whiteColorBlackBackground );
				textUtils_printAt( c, 23 );
				for ( b0 = c; b0 < 64; b0 ++ ) fputc_cons( ' ' );

			}
			if ( doExit ) break;

			// Print at the top of the screen the message the user is typing

			textUtils_printAt( 0, 0 );
			textUtils_setAttributes( whiteColorBlueBackground );
			fputc_cons( '>' );
			fputc_cons( ' ' );

			for ( b0 = 0; b0 < numCharactersInBuffer; b0 ++ ) fputc_cons( messageBuffer[ b0 ] );
			fputc_cons( ' ' );

			textUtils_setAttributes( whiteColorBlackBackground );
			fputc_cons( ' ' );

		}

		numCharactersInBuffer = 0;

		if ( numClientsInRoom == 0 ) {

			textUtils_setAttributes( textUtils_createAttributes( true, true, 2, 0 ) );
			textUtils_printAt( 14, 12 );
			textUtils_println( "The room was removed. Press a key." );
			while ( getKey() );
			while ( ! getKey() );
			while ( getKey() );

		}
		else {

			// Leave room.
			retroProt_leaveRoom();

		}

		textUtils_setAttributes( whiteColorBlackBackground );
		textUtils_cls();
		textUtils_printAt( 0, 0 );

	}

	// Exit server
	closeServerStream();
	closeServerConnection();

	textUtils_println( "\nProgram terminated." );

	terminate();

}

void listRoomsCallback( uint8_t numClients, uint8_t maxNumClients, uint8_t numBytesPerTransfer, uint8_t timeout, uint8_t flags ) {

	textUtils_printEOL();
	textUtils_print( "Room " );
	textUtils_print_l( roomIndex );
	textUtils_print( ": " );

	b1 = 0;
	longValue = ((long)ROOM_NAME_LENGTH) * ((long)roomIndex);
	for ( ib = 0; ib < ROOM_NAME_LENGTH; ib ++ ) {

		b0 = returnedRoomName[ ib ];

		messageBuffer[ longValue ++ ] = b0;

		if ( b1 == 0 ) {

			if ( b0 ) fputc_cons( b0 );
			else b1 = 1;

		}

	}

	textUtils_print( ", People: " );
	textUtils_print_l( numClients );


	textUtils_print( ", Max: " );
	textUtils_print_l( maxNumClients );

	roomIndex ++;

}

void terminate() {

	while ( 1 ) {}

}
