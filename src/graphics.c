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

// ZX Spectrum normal graphic mode functions

#include "graphics.h"
#include <stdlib.h>
#include <spectrum.h>

void paintGraphic8x8BlockPosition( uint16_t blockPosX, uint16_t blockPosY, uint8_t *graphic ) {

    uint8_t *addr = zx_cyx2saddr( blockPosY, blockPosX );
    uint8_t i;

    for ( i = 0; i < 8; i++ ) {

        *addr = *graphic++;

        addr = zx_saddrpdown( addr );

    }

}

void paintGraphicBlockPosition( uint16_t blockPosX, uint16_t blockPosY, uint16_t numBlocksX, uint16_t numBlocksY, uint8_t *graphic ) {

    uint16_t i;
    uint16_t maxi = blockPosY + numBlocksY;

    while ( numBlocksX > 0 ) {

        for ( i = blockPosY; i < maxi; i++ ) {

            paintGraphic8x8BlockPosition( blockPosX, i, graphic );

            graphic += 8;

        }

        numBlocksX--;
        blockPosX++;
    }

}

void paintGraphicBlockPositionReadOrder( uint16_t blockPosX, uint16_t blockPosY, uint16_t numBlocksX, uint16_t numBlocksY, uint8_t *graphic ) {

    uint16_t i;
    uint16_t maxi = blockPosX + numBlocksX;

    while ( numBlocksY > 0 ) {

        for ( i = blockPosX; i < maxi; i++ ) {

            paintGraphic8x8BlockPosition( i, blockPosY, graphic );

            graphic += 8;

        }

        numBlocksY--;
        blockPosY++;
    }

}

void paintGraphic8x8PixelPosition( uint16_t pixelPosX, uint16_t pixelPosY, uint16_t numLines, uint8_t *graphic ) {

    uint8_t mask;
    uint8_t *addr = zx_pxy2saddr( pixelPosX, pixelPosY, &mask );
    uint8_t *addr2;
    uint8_t i;
    uint8_t pixelsGraphic;
    uint8_t pixelsScreen;
    uint16_t pixelPosXBlock = pixelPosX % 8;
    uint16_t pixelPosXBlockM8 = 8 - pixelPosXBlock;

    if ( mask == 128 ) {
        mask = 255;
    }
    else {
        mask = ( mask << 1 ) - 1;
    }

    for ( i = 0; i < numLines; i++ ) {

        pixelsScreen = *addr & ( ~ mask );

        pixelsGraphic = *graphic++;

        *addr = pixelsScreen | ( pixelsGraphic >> pixelPosXBlock );

        if ( mask != 128 ) {

            addr2 = addr + 1;

            pixelsScreen = *addr2 & mask;

            *addr2 = pixelsScreen | ( pixelsGraphic << pixelPosXBlockM8 );

        }

        addr = zx_saddrpdown( addr );

    }


}

void paintGraphicPixelPosition( uint16_t pixelPosX, uint16_t pixelPosY, uint16_t numBlocksX, uint16_t numPixelsY, uint8_t *graphic ) {

    uint16_t i, y;
    uint16_t linesLeft;

    while ( numBlocksX > 0 ) {

        y = pixelPosY;
        linesLeft = numPixelsY;

        while ( linesLeft >= 8 ) {

            paintGraphic8x8PixelPosition( pixelPosX, y, 8, graphic );

            graphic += 8;

            y+= 8;
            linesLeft -= 8;

        }

        if ( linesLeft > 0 ) {

            paintGraphic8x8PixelPosition( pixelPosX, y, linesLeft, graphic );

            graphic += 8;

        }

        numBlocksX--;
        pixelPosX+= 8;
    }

}
