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

// RetroProt Game Engine Functions


#ifndef RETROPROT_H
#define RETROPROT_H

#include "./network.h"

#define APP_ID_LENGTH 32
#define ROOM_NAME_LENGTH 32

// Error codes

// No error, operation successful.
#define RP_ERROR_OK 0
// Server timed out.
#define RP_ERROR_TIMEOUT 1
// Server response was not correct.
#define RP_ERROR_BAD_SERVER_RESPONSE 2
// Some invalid parameter was provided, see function documentation in code for possible causes.
#define RP_ERROR_INVALID_PARAMETER 3
// Can't create room because the maximum number of rooms in the server was reached.
#define RP_ERROR_MAX_ROOMS_REACHED 4
// Can't create room because provided room name is already in use by an existing room.
#define RP_ERROR_ROOM_NAME_IN_USE 5
// Can't enter in the room because the room name was not found in the server.
#define RP_ERROR_ROOM_NOT_FOUND 6
// Can't enter in the room because the maximum number of clients in the room was reached.
#define RP_ERROR_MAX_CLIENTS_REACHED 7
// Error, client is not in a room
#define RP_ERROR_CLIENT_HAS_NO_ROOM 8
// Room was removed before client was able to enter it
#define RP_ERROR_ROOM_WAS_REMOVED 9

extern uint8_t retroProt_listRooms( uint8_t *appId, void (*callback)(), uint8_t *returnedRoomName, long *numberOfRooms );

extern uint8_t retroProt_countRooms( uint8_t *appId, long *numberOfRooms, bool countFullRooms );

extern uint8_t retroProt_createRoom( uint8_t *appId, uint8_t *roomName, uint8_t maxClients, uint8_t numBytesPerTransfer, uint8_t timeout, uint8_t flags );

extern bool retroProt_roomIdEquals( uint8_t *roomName1, uint8_t *roomName2 );

extern uint8_t retroProt_enterRoom( uint8_t *appId );
extern uint8_t retroProt_enterRandomRoom( uint8_t *appId, uint8_t *returnedRoomName );

extern uint8_t retroProt_leaveRoom();

extern void retroProt_disconnect();

extern uint8_t retroProt_resetTimer();

extern uint8_t retroProt_getUniqueId( uint8_t *id );

extern uint8_t retroProt_ping( long maxTimeout, long *pingTime );

extern uint8_t retroProt_getRandomNumber( uint8_t *number );

#endif /* RETROPROT_H */
