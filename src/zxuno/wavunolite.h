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

// Wavuno lite functions

#ifndef ZXUNO_WAVUNO_LITE_H
#define ZXUNO_WAVUNO_LITE_H

#include "../integerTypes.h"

#define UART_DATA_REG 250
#define UART_STAT_REG 251

#define ONLY_VIDEO_FRAME_SIZE ( RADAS_SCREEN_NUM_BYTES + 16 )
#define AUDIO_FRAME_SIZE 882
#define VIDEO_AND_AUDIO_FRAME_SIZE ( AUDIO_FRAME_SIZE + ONLY_VIDEO_FRAME_SIZE )

// WAV format definitions
#define WAV_RIFF_OFFSET 0
#define WAV_WAVEFMT_OFFSET 8
#define WAV_FORMAT1_OFFSET 16
#define WAV_FORMAT2_OFFSET 20
#define WAV_NUM_CHANNELS 22
#define WAV_SAMPLE_FREQUENCY 24
#define WAV_DATA 36
#define WAV_SAMPLE_DATA 44

// Wavuno interface

#define WAVUNO_DATA_REG 250
#define WAVUNO_STAT_REG 251

// Wavuno lite registers

#define WAVUNO_REG_INT_SAMPLE_RESET 0
#define WAVUNO_REG_INT_SAMPLE_WRITE 1

#define WAVUNO_REG_INT_CONTROL_FORMAT 3
#define WAVUNO_REG_INT_CONTROL_BEGIN_REPROD 4
#define WAVUNO_REG_INT_CONTROL_END_REPROD 5

#define WAVUNO_REG_INT_FREQ_DIVIDER0 6
#define WAVUNO_REG_INT_FREQ_DIVIDER1 7

#define WAVUNO_REG_INT_START_LOOP0 8
#define WAVUNO_REG_INT_START_LOOP1 9

#define WAVUNO_REG_INT_END_LOOP0 10
#define WAVUNO_REG_INT_END_LOOP1 11

extern void resetWavunoIntUserPointer();
extern void setWavunoIntBeginEnd( uint32_t beginAddress, uint32_t endAddress );

#endif /* ZXUNO_WAVUNO_LITE_H */

