
#include <dos.h>
#include <screen/screen.h>
#include <impl/pcdos/screen/scrdos.h>

#define VIDEO_MODE_TEXT_DOS ( 0x03 )
#define VIDEO_MODE_EGA320x200_DOS ( 0x0D )
#define VIDEO_MODE_MCGA320x200_DOS ( 0x13 )

#define SCREEN_X_EGA_MCGA ( 320 )
#define SCREEN_Y_EGA_MCGA ( 200 )
#define SCREEN_MCGA_LINE_NUM_BYTES ( SCREEN_X_EGA_MCGA )
#define SCREEN_MCGA_NUM_BYTES ( SCREEN_X_EGA_MCGA * SCREEN_Y_EGA_MCGA )

uint8_t *VRAM_POINTER = (uint8_t *)0xA0000000L;

uint8_t *backbufferDOS = NULL;

bool setPaletteDOS( uint8_t *palette ) {
    
    int b = 0;
    int i;

    if ( currentVideoMode != VIDEO_MODE_MCGA320x200 ) {
        return false;
    }

    for ( i = 0; i < 256; i++ ) {
        outp( 0x3c8, i);
        outp( 0x3c9, palette[ b++ ] );
        outp( 0x3c9, palette[ b++ ] );
        outp( 0x3c9, palette[ b++ ] );
    }
    
    return true;
    
}

uint8_t *getVRAMPointerDOS() {
    return VRAM_POINTER;
}

void setBackBufPointerDOS( uint8_t *buffer ) {
    backbufferDOS = buffer;
}

uint8_t *getBackBufPointerDOS() {
    return backbufferDOS;
}

void blitVRAMDOS() {

    memcpy( VRAM_POINTER, backbufferDOS, SCREEN_MCGA_NUM_BYTES );

}

void screenScanDOSMCGA( uint8_t *pointer, int16_t startx, int16_t endx, uint8_t color ) {

    // TODO test memset()
    
    if ( startx > endx ) {
        return;
    }
    
    memset( pointer + startx, color, endx - startx + 1 );

    /*
    pointer += startx;    
    while ( startx <= endx ) {
        *pointer++ = color;
        startx++;
    }
    */

}


void screenScanTrimmedDOSMCGA( uint8_t *pointer, int16_t startx, int16_t endx, uint8_t color ) {

    if ( startx >= screenSizeX ) {
        return;
    }
    if ( endx < 0 ) {
        return;
    }
    if ( startx < 0 ) {
        startx = 0;
    }
    if ( endx >= screenSizeX ) {
        endx = screenSizeX - 1;
    }

    // TODO test memset()
    pointer += startx;
    while ( startx <= endx ) {
        *pointer++ = color;
        startx++;
    }
}

bool setVideoMode( uint8_t mode ) {

    uint8_t modeDOS = 0;
    union REGS regs;
    
    switch ( mode ) {
        case VIDEO_MODE_TEXT:
            modeDOS = VIDEO_MODE_TEXT_DOS;
            screenSizeX = 0;
            screenSizeY = 0;
            screenLineNumBytes = 0;
            screenNumBytes = 0;
            setPalette = NULL;
            getVRAMPointer = NULL;
            setBackBufPointer = NULL;
            getVRAMBackBufPointer = NULL;
            blitVRAM = NULL;
            screenScan = NULL;
            screenScanTrimmed = NULL;
            break;
        
        case VIDEO_MODE_EGA320x200:
            // TODO not complete.
            modeDOS = VIDEO_MODE_EGA320x200_DOS;
            screenSizeX = SCREEN_X_EGA_MCGA;
            screenSizeY = SCREEN_Y_EGA_MCGA;
            screenLineNumBytes = 0;
            screenNumBytes = 0;
            setPalette = NULL;
            getVRAMPointer = getVRAMPointerDOS;
            setBackBufPointer = setBackBufPointerDOS;
            getVRAMBackBufPointer = getBackBufPointerDOS;
            blitVRAM = blitVRAMDOS;
            screenScan = NULL;
            screenScanTrimmed = NULL;
            break;
            
        case VIDEO_MODE_MCGA320x200:
            modeDOS = VIDEO_MODE_MCGA320x200_DOS;
            screenSizeX = SCREEN_X_EGA_MCGA;
            screenSizeY = SCREEN_Y_EGA_MCGA;
            screenLineNumBytes = SCREEN_MCGA_LINE_NUM_BYTES;
            screenNumBytes = SCREEN_MCGA_NUM_BYTES;
            setPalette = setPaletteDOS;
            getVRAMPointer = getVRAMPointerDOS;
            setBackBufPointer = setBackBufPointerDOS;
            getVRAMBackBufPointer = getBackBufPointerDOS;
            blitVRAM = blitVRAMDOS;
            screenScan = screenScanDOSMCGA;
            screenScanTrimmed = screenScanTrimmedDOSMCGA;
            break;
            
        default:
            return false;
    }
    
    regs.h.ah = 0x00;
    regs.h.al = modeDOS;

    int86( 0x10, &regs, &regs );
    
    currentVideoMode = mode;

    return true;

}
