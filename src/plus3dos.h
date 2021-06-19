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

// ZX Spectrum Divmmc functions

#ifndef PLUS3DOS_H
#define PLUS3DOS_H

#include "integerTypes.h"

// +3DOS system function calls

#define DOS_INIT 256
#define DOS_VERSION 259
#define DOS_OPEN 262
#define DOS_CLOSE 265
#define DOS_READ 274
#define DOS_WRITE 277
#define DOS_CAT 286
#define DOS_FREE_SPACE 289
#define DOS_SET_1346 319
// TODO make this call in plus3dos_deinit()
#define DOS_MOTOR_OFF 412

#define CMR0 32765

// +3DOS constants

#define OPEN_ACTION_ERROR_EXISTS 0
#define OPEN_ACTION_POSITION_TO_DATA 1
#define OPEN_ACTION_POSITION_TO_HEADER 2
#define OPEN_ACTION_MAKE_BACKUP 3
#define OPEN_ACTION_OVERWRITE 4

#define CREATE_ACTION_DONTCREATE 0
#define CREATE_ACTION_POINT_TO_DATA 1
#define CREATE_ACTION_POINT_TO_HEADER 2

#define FILE_TYPE_BASIC 0
#define FILE_TYPE_NUM_ARRAY 1
#define FILE_TYPE_CHR_ARRAY 2
#define FILE_TYPE_BYTES 3

#define ACCESS_MODE_EXCLUSIVE_READ 1
#define ACCESS_MODE_EXCLUSIVE_WRITE 2
#define ACCESS_MODE_EXCLUSIVE_READ_WRITE 3
#define ACCESS_MODE_SHARED 5

#define FMODE_CREATE 0

#define BANKM 23388

/*
 * PLUS3DOS API wrapper functions
 *
*/

extern uint16_t plus3dos_init();
extern uint16_t plus3dos_getVersion();
extern uint16_t plus3dos_getFreeSpace( uint8_t driveLetter );
extern uint16_t plus3dos_open( uint8_t *fileName, uint8_t fileNumber, uint8_t accessMode, uint8_t createAction, uint8_t openAction );
extern uint16_t plus3dos_close( uint8_t fileNumber );
extern uint16_t plus3dos_readBytes( uint8_t *buffer, uint8_t fileNumber, uint16_t numBytes );
extern uint16_t plus3dos_writeBytes( uint8_t *buffer, uint8_t fileNumber, uint16_t numBytes );
extern uint16_t plus3dos_readDir( uint8_t *fileName, uint8_t *buffer );
extern uint32_t plus3dos_getFileSizeBytes( uint8_t *fileName, uint8_t fileNumber, uint8_t *buffer, uint16_t bufferSize, uint8_t openAction );

//extern void plus3dos_delete( uint8_t *fileName );

#endif /* PLUS3DOS_H */

