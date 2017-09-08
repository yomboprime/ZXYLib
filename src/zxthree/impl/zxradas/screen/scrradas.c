
#include <spectrum.h>
#include <string.h>
#include <stdlib.h>

#include "../../../screen/screen.h"
#include "scrradas.h"
#include "../../../../zxuno/zxuno.h"
#include "../../../../zxuno/radas.h"

#define SCREEN_X_ZX_DEFAULT ( 256 )
#define SCREEN_Y_ZX_DEFAULT ( 192 )
#define SCREEN_ZX_DEFAULT_LINE_NUM_BYTES ( 32 )
#define SCREEN_ZX_DEFAULT_NUM_BYTES ( SCREEN_X_EGA_MCGA * SCREEN_Y_EGA_MCGA )

#define SCREEN_X_RADASTAN ( 128 )
#define SCREEN_Y_RADASTAN ( 96 )
#define SCREEN_RADASTAN_LINE_NUM_BYTES ( 64 )
#define SCREEN_RADASTAN_NUM_BYTES ( 6144 )

#define DMASRC 0xa1
#define DMADST 0xa2
#define DMALEN 0xa4
#define DMACTR 0xa0

void memsetDMA( uint8_t *mem, uint8_t val, uint16_t len ) {

    *mem = val;
    len--;
    if ( ! len ) {
        return;
    }

    outp( ZXUNO_ADDR, DMASRC );
    outp( ZXUNO_REG, mem & 0xff );
    outp( ZXUNO_REG, ( mem >> 8 ) & 0xff );

    mem++;
    outp( ZXUNO_ADDR, DMADST );
    outp( ZXUNO_REG, mem&0xff );
    outp( ZXUNO_REG, ( mem >> 8 ) & 0xff );

    outp( ZXUNO_ADDR, DMALEN );
    outp( ZXUNO_REG, len & 0xff );
    outp( ZXUNO_REG, ( len >> 8 ) & 0xff );

    outp( ZXUNO_ADDR, DMACTR );
    outp( ZXUNO_REG, 1 ); /* DMA mem->mem, burst */

}

bool setPaletteRadas( uint8_t *palette ) {

    if ( currentVideoMode != VIDEO_MODE_RADASTAN ) {
        return false;
    }

    radasSetPalette( palette );
    
    return true;
    
}

uint8_t *getVRAMPointerRadas() {
    return radasGetCurrentScreen();
}

void setBackBufPointerRadas( uint8_t *buffer ) {
    // Nothing to do here
}

uint8_t *getBackBufPointerRadas() {
    return radasGetTheOtherScreen();
}

void blitVRAMRadas() {

    radasChangeScreens();

}

void screenScanRadas( uint8_t *pointer, int16_t startx, int16_t endx, uint8_t color ) {

    if ( startx > endx ) {
        return;
    }

    memset( pointer + ( startx >> 1 ), color, ( endx - startx + 1 ) >> 1 );
    
    //memsetDMA( pointer + ( startx >> 1 ), color, ( endx - startx + 1 ) >> 1 );

}


void screenScanTrimmedRadas( uint8_t *pointer, int16_t startx, int16_t endx, uint8_t color ) {

    uint8_t temp;
    uint8_t *tempPointer;
    
    if ( startx > endx ) {
        return;
    }

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

    if ( startx & 1 ) {
        tempPointer = pointer + ( startx >> 1 );
        temp = *tempPointer;
        temp = ( temp & 0xF0 ) | ( color & 0x0F );
        *tempPointer = temp;
        startx++;
    }

    if ( startx >= screenSizeX ) {
        return;
    }
    if ( startx > endx ) {
        return;
    }

    if ( ! ( endx & 1 ) ) {
        tempPointer = pointer + ( endx >> 1 );
        temp = *tempPointer;
        temp = ( temp & 0x0F ) | ( color & 0xF0 );
        *tempPointer = temp;
        endx--;
    }
    if ( endx < 0 ) {
        return;
    }
    if ( startx > endx ) {
        return;
    }

    memset( pointer + ( startx >> 1 ), color, ( endx - startx + 1 ) >> 1 );

    //memsetDMA( pointer + ( startx >> 1 ), color, ( endx - startx + 1 ) >> 1 );
}

bool setVideoMode( uint8_t mode ) {

    uint8_t blackPalette[ 16 ];
    memset( blackPalette, 0, 16 );
    
    switch ( mode ) {
        case VIDEO_MODE_ZX_DEFAULT:
            // TODO
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
            
            radasSetNormalMode( PAPER_WHITE | INK_BLACK, INK_WHITE );
            
            break;
        
        case VIDEO_MODE_RADASTAN:
            screenSizeX = SCREEN_X_RADASTAN;
            screenSizeY = SCREEN_Y_RADASTAN;
            screenLineNumBytes = SCREEN_RADASTAN_LINE_NUM_BYTES;
            screenNumBytes = SCREEN_RADASTAN_NUM_BYTES;
            setPalette = setPaletteRadas;
            getVRAMPointer = getVRAMPointerRadas;
            setBackBufPointer = setBackBufPointerRadas;
            getVRAMBackBufPointer = getBackBufPointerRadas;
            blitVRAM = blitVRAMRadas;
            screenScan = screenScanRadas;
            screenScanTrimmed = screenScanTrimmedRadas;
            
            radasSetRadastanMode( PAPER_WHITE | INK_BLACK, INK_WHITE, blackPalette );
            
            break;
            
        default:
            return false;
    }
    
    currentVideoMode = mode;

    return true;

}
