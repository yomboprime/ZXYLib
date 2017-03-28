/*
 * @author yomboprime
 *
 * Wavuno lite functions
*/

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

