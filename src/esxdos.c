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

    ESXDOS_FSTAT_Struct stat;

    ESXDOS_fstat( (uint8_t *)&stat, fhandle );
    iferror {
        return 0;
    }

    return stat.fileSize;

}

/*
 * Given a file handle, returns true if it corresponds to a directory else returns false.
 */
bool ESXDOS_isDirectory( int16_t fhandle ) {

    ESXDOS_FSTAT_Struct stat;

    ESXDOS_fstat( (uint8_t *)&stat, fhandle );
    iferror {
        return false;
    }

    return ( stat.attributes & ESXDOS_FILE_ATTRIBUTE_DIR_BIT ) == ESXDOS_FILE_ATTRIBUTE_DIR_BIT ? true : false;

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
 * drive: The drive number
 * Returns file handle if success
 * Always close opened files when done with them.
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

    #asm

        ; get param in a
        ld hl, 2
        add hl, sp
        ld a, (hl)

        rst 0x08
        defb ESXDOS_F_SYNC

    #endasm

}


extern uint32_t ESXDOS_fseek_returnValue;

#asm
._ESXDOS_fseek_returnValue
 defb	0
 defb	0
 defb	0
 defb	0
#endasm

/*
 * Seek the current file cursor (position, or pointer)
 * n The number of bytes to seek
 * mode: See the ESXDOS_SEEK_* constants in esxdos.h
 * fhandle: The file handle
 *
 * Returns current value of cursor/pointer position
 */
uint32_t ESXDOS_fseek( uint32_t n, int16_t mode, int16_t fhandle ) {

    #asm

        ; get sp and skip return address
        ld hl, 2
        add hl, sp

        ; first param from the end
        ld a, (hl)
        inc hl
        inc hl

        ; second param from the end
        ; ixl <- (hl)
        ld e, (hl)
        inc hl
        inc hl
        ld d, 0
        ld ixl, e
        ld ixh, d

        ; last param from the end
        ; bcde <- (hl)
        ld e, (hl)
        inc hl
        ld d, (hl)
        inc hl
        ld c, (hl)
        inc hl
        ld b, (hl)

        rst 0x08
        defb ESXDOS_F_SEEK

        ; Return value
        ld hl, _ESXDOS_fseek_returnValue
        ; (hl) <- bcde
        ld (hl), e
        inc hl
        ld (hl), d
        inc hl
        ld (hl), c
        inc hl
        ld (hl), b


    #endasm

    return ESXDOS_fseek_returnValue;
}

/*
 * Given a file handle, return current value of cursor/pointer position
 */
uint32_t ESXDOS_fgetPos( int16_t fhandle ) {

    #asm

        ; get sp and skip return address
        ld hl, 2
        add hl, sp

        ; first param from the end
        ld a, (hl)
        inc hl
        inc hl

        rst 0x08
        defb ESXDOS_F_GETPOS

        ; Return value
        ld hl, _ESXDOS_fseek_returnValue
        ; (hl) <- bcde
        ld (hl), e
        inc hl
        ld (hl), d
        inc hl
        ld (hl), c
        inc hl
        ld (hl), b


    #endasm

    return ESXDOS_fseek_returnValue;

}

/*
 * Given a file handle, fill a ESXDOS_FSTAT_Struct with info/status of the file.
 * infoStruct: A ESXDOS_FSTAT_Struct struct (See esxdos.h)
 * fhandle: The file handle
 * Return error code if there was error.
 */
int16_t ESXDOS_fstat( ESXDOS_FSTAT_Struct *infoStruct, int16_t fhandle ) {

    #asm

        ; get sp and skip return address
        ld hl, 2
        add hl, sp

        ; first param from the end
        ld a, (hl)
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
        defb ESXDOS_F_FSTAT

        ld h, 0
        ld l, a     ; Return value

    #endasm

}

/*
 * Open a directory for listing its files
 * pathDirName can be absolute or relative to CWD
 * drive: The drive number
 * Returns directory handle if success
 * Always close opened directories when done with them.
 */
int16_t ESXDOS_openDirectory( uint8_t *pathDirName, int16_t drive ) {

    #asm

        ; get sp and skip return address
        ld hl, 2
        add hl, sp

        ; first param from the end
        ld a, (hl)
        inc hl
        inc hl

        ; last param from the end
        ; ix <- (hl)
        ld e, (hl)
        inc hl
        ld d, (hl)
        ld ixl, e
        ld ixh, d

        ; No attributes
        ld b, 0

        rst 0x08
        defb ESXDOS_F_OPENDIR

        ld h, 0
        ld l, a     ; Returns file handle if no error else error code

    #endasm

}

/*
 * Read a directory entry in a buffer
 * buffer: The buffer to store the entry bytes
 * dhandle: The directory handle
 * Returns 1 if an entry was read (and possibly there are more after this one, which you can read by making more calls). Else returns 0.
 *
 * Entry Structure:
 * - 1 byte MSDOS-like attributes.
 * - asciiz File or directory name.
 * - 4 bytes Date
 * - 4 bytes File Size
 */
int16_t ESXDOS_readDirectory( uint8_t *buffer, int16_t dhandle ) {

    #asm

        ; get sp and skip return address
        ld hl, 2
        add hl, sp

        ; first param from the end
        ld a, (hl)
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
        defb ESXDOS_F_READDIR

        ld h, 0
        ld l, a     ; Return value

    #endasm


}

/*
 * Get the current working directory (cwd) path in a buffer
 * buffer:  The buffer to fill in the cwd, will be null terminated
 * drive: The drive number
 */
void ESXDOS_getCWD( uint8_t *buffer, int16_t drive ) {

    #asm

        ; get sp and skip return address
        ld hl, 2
        add hl, sp

        ; first param from the end
        ld a, (hl)
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
        defb ESXDOS_F_GETCWD

    #endasm

}

/*
 * Set the current working directory (cwd) path
 * pathDirName:  The string with the cwd to set, null terminated.
 * drive: The drive number
 */
void ESXDOS_changeDirectory( uint8_t *pathDirName, int16_t drive ) {

    #asm

        ; get sp and skip return address
        ld hl, 2
        add hl, sp

        ; first param from the end
        ld a, (hl)
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
        defb ESXDOS_F_CHDIR

    #endasm

}

/*
 * Deletes a file
 * pathFileName: path including the file name
 * drive: The drive number
 */
void ESXDOS_delete( uint8_t *pathFileName, int16_t drive ) {

    #asm

        ; get sp and skip return address
        ld hl, 2
        add hl, sp

        ; first param from the end
        ld a, (hl)
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
        defb ESXDOS_F_UNLINK

    #endasm

}
