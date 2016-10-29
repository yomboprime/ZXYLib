/*
 * @author yomboprime
 *
 * ZX Spectrum Divmmc functions
*/

#ifndef ESXDOS_H
#define ESXDOS_H

#include "integerTypes.h"


// File access modes

// Open for read
#define ESXDOS_FILEMODE_READ ( 0x01 )

// Open for write, fail if exists
#define ESXDOS_FILEMODE_WRITE ( 0x06 )

// Open for write, create if not exists, truncate if exists
#define ESXDOS_FILEMODE_WRITE_CREATE ( 0x0E )


// ESXDOS calls

#define ESXDOS_M_GETSETDRV ( 0x89 )

#define ESXDOS_F_OPEN ( 0x9A )
#define ESXDOS_F_CLOSE ( 0x9B )

#define ESXDOS_F_READ ( 0x9D )
#define ESXDOS_F_WRITE ( 0x9E )

// Other definitions

#define ESXDOS_FILE_ATTRIBUTE_DIR_BIT ( 0x10 )

#define ESXDOS_FILE_STAT_STRUCT_SIZE ( 11 )
#define ESXDOS_FILE_STAT_ATTRIBUTES_OFFSET ( 2 )
#define ESXDOS_FILE_STAT_SIZE_OFFSET ( 7 )

/*
 *
 *  Generic functions
 *
 *
*/
extern uint32_t ESXDOS_fsize( int16_t fhandle );
bool ESXDOS_isDirectory( int16_t fhandle );

/*
 * ESXDOS API wrapper functions
 *
 * Note: Check with "iferror {...} else {...}" after every call (Carry flag)
 *
*/
extern int16_t ESXDOS_getDefaultDrive();
extern int16_t ESXDOS_fopen( uint8_t *pathFileName, int16_t mode, int16_t drive );
extern void ESXDOS_fclose( uint16_t fhandle );
extern uint16_t ESXDOS_fread( uint8_t *buffer, uint16_t length, int16_t fhandle );
extern uint16_t ESXDOS_fwrite( uint8_t *buffer, uint16_t length, int16_t fhandle );
extern void ESXDOS_fsync( uint16_t fhandle );
extern uint32_t ESXDOS_fseek( uint32_t n, int16_t mode, int16_t fhandle );
extern uint32_t ESXDOS_fgetPos( int16_t fhandle );
extern void ESXDOS_fstat( uint8_t *buffer, int16_t fhandle );
extern int16_t ESXDOS_openDirectory( uint8_t *pathDirName, int16_t drive );
extern int16_t ESXDOS_readDirectory( uint8_t *buffer, int16_t fhandle );
extern void ESXDOS_getCWD( uint8_t *path, int16_t drive );
extern void ESXDOS_changeDirectory( uint8_t *pathDirName, int16_t drive );
extern void ESXDOS_delete( uint8_t *pathFileName, int16_t drive );

#endif /* ESXDOS_H */

