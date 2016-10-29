/*
 * @author yomboprime
 *
 * ZX Spectrum Divmmc functions
*/

#include "esxdos.h"

/*
 *
 * Generic functions
 *
 */

/*
 * Returns file size given a file handle
 */
uint32_t ESXDOS_fsize( int16_t fhandle ) {

    uint8_t buffer[ ESXDOS_FILE_STAT_STRUCT_SIZE ];
    uint32_t *pointer;

    ESXDOS_fstat( buffer, fhandle );
    iferror {
        return 0;
    }

    pointer = ( (uint32_t*)( buffer + ESXDOS_FILE_STAT_SIZE_OFFSET ) );

    return *pointer;


}

/*
 * Returns if a file handle corresponds to a directory (true)
 */
bool ESXDOS_isDirectory( int16_t fhandle ) {

    uint8_t buffer[ ESXDOS_FILE_STAT_STRUCT_SIZE ];

    ESXDOS_fstat( buffer, fhandle );
    iferror {
        return false;
    }

    return ( buffer[ ESXDOS_FILE_STAT_ATTRIBUTES_OFFSET ] & ESXDOS_FILE_ATTRIBUTE_DIR_BIT ) == ESXDOS_FILE_ATTRIBUTE_DIR_BIT ? true : false;

}

/*
 *
 * ESXDOS API wrapper functions
 *
 * Note that carry is preserved in all functions. If set, it means that some error occurred.
 * Check with "iferror {...} else {...}" after every call.
 *
 * For example:
 *
 * uint16_t drive = ESXDOS_getDefaultDrive();
 * iferror {
 *     printf( "Error reading device." );
 * }
 * else {
 *     printf( "Default drive number is: %u", drive );
 * }
 *
 */

/*
 * Gets the default drive number. Error if no drive present. If error, returned value is ESXDOS error code
 */
int16_t ESXDOS_getDefaultDrive() {

    #asm

        ld a, 0     ; Drive number (0 = get default) in a

        rst 0x08
        defb ESXDOS_M_GETSETDRV

        ld h, 0
        ld l, a     ; Returns default drive if no error

    #endasm

}

/*
 * Open a file.
 * pathFileName can be absolute or relative to CWD
 * mode: one of ESXDOS_FILEMODE_* values
 * Returns file handle if success
 */
int16_t ESXDOS_fopen( uint8_t *pathFileName, int16_t mode, int16_t drive ) {

    #asm

        ; get sp and skip return address
        ld hl, 2
        add hl, sp

        ; first param from the end
        ld a, (hl)
        inc hl
        inc hl

        ; second param from the end
        ld b, (hl)
        inc hl
        inc hl

        ; last param from the end
        ; ix <- (hl)
        ld e, (hl)
        inc hl
        ld d, (hl)
        ld ixl, e
        ld ixh, d

        rst 0x08
        defb ESXDOS_F_OPEN

        ld h, 0
        ld l, a     ; Returns file handle if no error else error code

    #endasm

}

/*
 * Close a file or directory handle.
 * TODO Make it FASTCALL
 */
void ESXDOS_fclose( uint16_t fhandle ) {

    #asm

        ; get param in a
        ld hl, 2
        add hl, sp
        ld a, (hl)

        rst 0x08
        defb ESXDOS_F_CLOSE

    #endasm

}

/*
 * Read a number of bytes from a file
 * buffer: The buffer to store the bytes read.
 * lenght: Number of bytes to read.
 * fhandle: The file handle
 * Returns the number of bytes actually read.
 */
uint16_t ESXDOS_fread( uint8_t *buffer, uint16_t length, int16_t fhandle ) {

    #asm

        ; get sp and skip return address
        ld hl, 2
        add hl, sp

        ; first param from the end
        ld a, (hl)
        inc hl
        inc hl

        ; second param from the end
        ld c, (hl)
        inc hl
        ld b, (hl)
        inc hl
        
        ; last param from the end
        ; ix <- (hl)
        ld e, (hl)
        inc hl
        ld d, (hl)
        ld ixl, e
        ld ixh, d

        rst 0x08
        defb ESXDOS_F_READ

        ld l, c
        ld h, b     ; Returns num. of bytes read if no error

    #endasm

}

/*
 * Write a number of bytes to a file
 * buffer: The buffer of bytes to write.
 * lenght: Number of bytes to write.
 * fhandle: The file handle
 * Returns the number of bytes actually written.
 */
uint16_t ESXDOS_fwrite( uint8_t *buffer, uint16_t length, int16_t fhandle ) {

    #asm

        ; get sp and skip return address
        ld hl, 2
        add hl, sp

        ; first param from the end
        ld a, (hl)
        inc hl
        inc hl

        ; second param from the end
        ld c, (hl)
        inc hl
        ld b, (hl)
        inc hl

        ; last param from the end
        ; ix <- (hl)
        ld e, (hl)
        inc hl
        ld d, (hl)
        ld ixl, e
        ld ixh, d

        rst 0x08
        defb ESXDOS_F_WRITE

        ld l, c
        ld h, b     ; Returns num. of bytes written if no error

    #endasm

}

/*
 * Given a file handle, updates file info / flushes write cache
 */
void ESXDOS_fsync( uint16_t fhandle ) {
    // TODO implement
}


uint32_t ESXDOS_fseek( uint32_t n, int16_t mode, int16_t fhandle ) {
    // TODO implement
}

uint32_t ESXDOS_fgetPos( int16_t fhandle ) {
    // TODO implement
}

void ESXDOS_fstat( uint8_t *buffer, int16_t fhandle ) {
    // TODO implement
}

int16_t ESXDOS_openDirectory( uint8_t *pathDirName, int16_t drive ) {
    // TODO implement
}

int16_t ESXDOS_readDirectory( uint8_t *buffer, int16_t fhandle ) {
    // TODO implement
}

void ESXDOS_getCWD( uint8_t *path, int16_t drive ) {
    // TODO implement
}

void ESXDOS_changeDirectory( uint8_t *pathDirName, int16_t drive ) {
    // TODO implement
}

void ESXDOS_delete( uint8_t *pathFileName, int16_t drive ) {
    // TODO implement
}
