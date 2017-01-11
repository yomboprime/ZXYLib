/*
 * @author yomboprime
 *
 * ZX Spectrum normal graphic mode functions
*/

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "integerTypes.h"

extern void paintGraphic8x8BlockPosition( uint16_t blockPosX, uint16_t blockPosY, uint8_t *graphic );
extern void paintGraphicBlockPosition( uint16_t blockPosX, uint16_t blockPosY, uint16_t numBlocksX, uint16_t numBlocksY, uint8_t *graphic );
extern void paintGraphicBlockPositionReadOrder( uint16_t blockPosX, uint16_t blockPosY, uint16_t numBlocksX, uint16_t numBlocksY, uint8_t *graphic );

extern void paintGraphic8x8PixelPosition( uint16_t pixelPosX, uint16_t pixelPosY, uint16_t numLines, uint8_t *graphic );
extern void paintGraphicPixelPosition( uint16_t pixelPosX, uint16_t pixelPosY, uint16_t numBlocksX, uint16_t numPixelsY, uint8_t *graphic );

#endif /* GRAPHICS_H */

