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

#include "network.h"
#include <stdlib.h>
#include <spectrum.h>

uint8_t initNetwork() {

	// Returns error code (see network.h)

	UART_begin();

	// End possible previous connection
	UART_print( (uint8_t*)"+++" );

	delay( 1500 );

	UART_println( (uint8_t*)"AT+CIPMODE=0" );

	if ( UART_find( (uint8_t*)"OK", 500 ) ) {

		UART_println( (uint8_t*)"AT+CIPCLOSE" );

		UART_find( (uint8_t*)"OK", 500 );

	}

	// Detect WiFi module
    UART_println( (uint8_t*)"AT" );

	if ( ! UART_find( (uint8_t*)"OK", 1500 ) ) return NETWORK_ERROR_MODEM_NOT_DETECTED;

	return NETWORK_ERROR_OK;

}

uint8_t connectToWiFi( char *ssid, char *password, long timeout ) {

	// timeout in ms

	UART_println( (uint8_t*)"AT+CWMODE=3" );
	if ( ! UART_find( (uint8_t*)"OK", 1500 ) ) return NETWORK_ERROR_SET_WIFI_MODE;

	UART_print( (uint8_t*)"AT+CWJAP=\"" );
	UART_print( (uint8_t*)ssid );
	UART_print( (uint8_t*)"\",\"" );
	UART_print( (uint8_t*)password );
	UART_println( (uint8_t*)"\"" );
	if ( ! UART_find( (uint8_t*)"OK", timeout ) ) return NETWORK_ERROR_CONNECT_TO_WIFI;

	return NETWORK_ERROR_OK;

}

uint8_t connectToServer( char *hostName, char *hostPort ) {

	UART_println( (uint8_t*)"AT+CIPMUX=0" );
	if ( ! UART_find( (uint8_t*)"OK", 2500 ) ) return NETWORK_ERROR_CONFIG_MODEM_MULTI;

	UART_print( (uint8_t*)"AT+CIPSTART=\"TCP\",\"" );
	UART_print( (uint8_t*)hostName );
	UART_print( (uint8_t*)"\"," );
	UART_println( (uint8_t*)hostPort );
	if ( ! UART_find( (uint8_t*)"OK", 2500 ) ) return NETWORK_ERROR_CONNECT_SERVER;

	return NETWORK_ERROR_OK;

}

uint8_t openServerStream() {

	// After success, uart functions can be used to read and write data, and closeServerStream to finish.

	UART_println( (uint8_t*)"AT+CIPMODE=1" );
	if ( ! UART_find( (uint8_t*)"OK", 2000 ) ) return NETWORK_ERROR_OPEN_SERVER_STREAM;

	UART_println( (uint8_t*)"AT+CIPSEND" );
	if ( ! UART_find( (uint8_t*)"OK", 2000 ) ) return NETWORK_ERROR_WRITE_SERVER_DATA;
	if ( ! UART_find( (uint8_t*)">", 2000 ) ) return NETWORK_ERROR_READ_SERVER_DATA;

	return NETWORK_ERROR_OK;

}

uint8_t closeServerStream() {

	delay( 1500 );
	UART_print( (uint8_t*)"+++" );
	delay( 1500 );
	UART_println( (uint8_t*)"AT+CIPMODE=0" );
	if ( ! UART_find( (uint8_t*)"OK", 2000 ) ) return NETWORK_ERROR_CLOSE_STREAM;

	return NETWORK_ERROR_OK;

}

uint8_t closeServerConnection() {

	UART_println( (uint8_t*)"AT+CIPCLOSE" );
	if ( ! UART_find( (uint8_t*)"OK", 2000 ) ) return NETWORK_ERROR_CLOSE_CONNECTION;

	return NETWORK_ERROR_OK;

}
