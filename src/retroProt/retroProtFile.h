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

// RetroProt File Transfer Functions

#ifndef RETROPROT_FILE_H
#define RETROPROT_FILE_H

#include "../network.h"

// Size of an entry in the rpFile_listFiles buffer result, minus the file name bytes
// It is equal to 1 byte type + 1 byte separation + 3 bytes extension + 4 bytes file size
#define TCP_DIR_ENTRY_SIZE ( 9 )

// Error codes

#define RPF_ERROR_OK ( 0 )
#define RPF_ERROR_TIMEOUT ( 1 )
#define RPF_ERROR_INVALID_PARAMETER ( 2 )
#define RPF_ERROR_BAD_SERVER_RESPONSE ( 3 )
#define RPF_ERROR_PATH_NOT_FOUND ( 4 )
#define RPF_ERROR_INDEX_OUT_OF_BOUNDS ( 5 )
#define RPF_ERROR_CANT_DOWNLOAD_DIRECTORY ( 6 )
#define RPF_ERROR_DOWNLOADING ( 7 )
#define RPF_ERROR_SD_NOT_INSERTED ( 8 )
#define RPF_ERROR_WRITING_FILE ( 9 )
#define RPF_ERROR_FILE_ALREADY_EXISTS ( 10 )
#define RPF_ERROR_FILE_SIZE_IS_0 ( 11 )
#define RPF_ERROR_FILE_SIZE_TOO_LARGE ( 12 )
#define RPF_ERROR_READING_FILE ( 13 )
#define RPF_ERROR_SERVER_CANT_CREATE_FILE ( 14 )

// File list ordering types

#define FILE_ORDERING_ALPHABETICAL ( 0x00 )
#define FILE_ORDERING_ALPHABETICAL_INVERTED ( 0x01 )
#define FILE_ORDERING_BY_SIZE_DECREASING ( 0x02 )
#define FILE_ORDERING_BY_SIZE_INCREASING ( 0x03 )
#define FILE_ORDERING_BY_DATE_DECREASING ( 0x04 )
#define FILE_ORDERING_BY_DATE_INCREASING ( 0x05 )
#define FILE_ORDERING_MAX FILE_ORDERING_BY_DATE_INCREASING

extern uint8_t rpFile_listFiles( uint8_t *tcpPath, uint8_t *searchString, uint8_t ordering, uint8_t *buffer, uint16_t firstEntry, uint16_t maxEntries, uint16_t *numEntries, uint16_t *numTotalEntries, uint8_t maxBytesFileName );

extern uint8_t rpFile_getFileNameAndSize( uint8_t *tcpPath, uint8_t *searchString, uint8_t ordering, uint16_t entry, uint8_t *buffer, uint16_t bufferSize, uint32_t *fileSize, uint8_t *fileOrDirectory );

extern uint8_t rpFile_downloadFile( uint8_t *tcpPath, uint8_t *searchString, uint8_t ordering, uint16_t entry, uint8_t *sdPath, uint8_t *diskBuffer, uint16_t diskBufferSize, void (*progressCallback)() );

extern uint8_t rpFile_uploadFile( uint8_t *tcpPath, uint8_t *sdFilename, uint8_t *diskBuffer, uint16_t diskBufferSize, uint32_t fileSize, void (*progressCallback)() );

#endif /* RETROPROT_FILE_H */
