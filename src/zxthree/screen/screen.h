
#ifndef __SCREEN__
#define __SCREEN__

#include "../../zxthree/bits.h"

#ifdef __cplusplus
    extern "C" {
#endif

// Sets display video mode.
// This function must be always implemented in machine specific .c file
// 'mode' parameter is machine dependent. mode = 0 must be the default mode in the machine.
// Returns true on success, false otherwise
extern bool setVideoMode( uint8_t mode );

// The user SHOULD NOT WRITE to these variables, ONLY READ them.
// These variables contain valid values only after a call to setVideoMode() which returned true.
extern uint16_t screenSizeX;
extern uint16_t screenSizeY;
extern uint16_t screenLineNumBytes;
extern uint16_t screenNumBytes;
extern uint8_t currentVideoMode;


#define SCREEN_ASPECT ( ( (float)( screenSizeY ) ) / ( (float)( screenSizeX ) ) )

// Function pointers, dependent on machine and current graphic mode

// Note that in some machines and graphic modes, some functions have
// no meaning and thus its pointers will be NULL.

// Sets a display color palette.
// 'palette' parameter is machine and display mode dependent.
// Returns true on success, false otherwise
//bool setPalette( uint8_t *palette );
//typedef bool(*SetPaletteFunc)( uint8_t * );
#define SetPaletteFunc( x ) bool(*x)()
extern SetPaletteFunc( setPalette );

// Sets pointer to start of backbuffer, if this option is available.
// Backbuffer pixel data distribution is machine and display mode dependent.
//bool setBackBufPointer( uint8_t *buffer );
//typedef void(*SetPixelPFunction)( uint8_t * );
#define SetPixelPFunction( x ) void(*x)()
extern SetPixelPFunction( setBackBufPointer );

// Returns pointer to start of VRAM.
// VRAM pixel data distribution is machine and display mode dependent.
// uint8_t *getVRAMPointer( void );
//typedef uint8_t *(*GetPixelPFunction)( void );
#define GetPixelPFunction( x ) uint8_t *(*x)()
extern GetPixelPFunction( getVRAMPointer );

// Returns pointer to VRAM's backbuffer.
// VRAM pixel data distribution is machine and display mode dependent.
// uint8_t *getVRAMBackBufPointer( void );
extern GetPixelPFunction( getVRAMBackBufPointer );

// Performs VRAM double buffer blit if available for machine and current display mode and configuration.
//void blitVRAM( void )
//typedef void(*BlitVRAMFunc)( void );
#define BlitVRAMFunc( x ) void(*x)()
extern BlitVRAMFunc( blitVRAM );

// Draws a "scan" (set of contiguous pixels in the same row)
// void screenScan( uint8_t *pointer, int16_t startx, int16_t endx, uint8_t color )
//typedef void(*ScreenScanFunc)( uint8_t *, int16_t, int16_t, uint8_t );
#define ScreenScanFunc( x ) void(*x)()
extern ScreenScanFunc( screenScan );

// Same as screenScan, but performing clipping check against screen limits
// void screenScanTrimmed( uint8_t *pointer, int16_t startx, int16_t endx, uint8_t color )
extern ScreenScanFunc( screenScanTrimmed );

#ifdef __cplusplus
    }
#endif

#endif //__SCREEN__
