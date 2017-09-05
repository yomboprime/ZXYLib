
#include "../../zxthree/screen/screen.h"

uint16_t screenSizeX = 0;
uint16_t screenSizeY = 0;
uint16_t screenLineNumBytes = 0;
uint16_t screenNumBytes = 0;
uint8_t currentVideoMode = 0;

SetPaletteFunc( setPalette ) = NULL;

SetPixelPFunction( setBackBufPointer ) = NULL;

GetPixelPFunction( getVRAMPointer ) = NULL;

GetPixelPFunction( getVRAMBackBufPointer ) = NULL;

BlitVRAMFunc( blitVRAM ) = NULL;

ScreenScanFunc( screenScan ) = NULL;

ScreenScanFunc( screenScanTrimmed ) = NULL;
