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

// Network functions (AT commands with ESP8266 through UART)

#ifndef NETWORK_H
#define NETWORK_H

#include "integerTypes.h"
#include "zxuno/uart.h"

// Error codes
#define NETWORK_ERROR_OK 0
#define NETWORK_ERROR_MODEM_NOT_DETECTED 1
#define NETWORK_ERROR_SET_WIFI_MODE 2
#define NETWORK_ERROR_CONNECT_TO_WIFI 3
#define NETWORK_ERROR_CONFIG_MODEM_MULTI 4
#define NETWORK_ERROR_CONNECT_SERVER 5
#define NETWORK_ERROR_OPEN_SERVER_STREAM 6
#define NETWORK_ERROR_READ_SERVER_DATA 7
#define NETWORK_ERROR_WRITE_SERVER_DATA 8
#define NETWORK_ERROR_CLOSE_STREAM 9
#define NETWORK_ERROR_CLOSE_CONNECTION 10

#define NETWORK_SSID_LENGTH 32

extern uint8_t initNetwork();

extern uint8_t connectToWiFi( char *ssid, char *password, long timeout );
// TODO: disconnectFromWiFi()

extern uint8_t connectToServer( char *hostName, char *hostPort );
extern uint8_t openServerStream();
extern uint8_t closeServerStream();
extern uint8_t closeServerConnection();

#endif /* NETWORK_H */

