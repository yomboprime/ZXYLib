/*
 * @author yomboprime
 *
 * ZX-Uno Specific definitions and functions
*/

#include "zxuno.h"

#include <time.h>

long ZX_Uno_t = 0UL;

long millis() {

    // Note: this is a not exact simplification
    // Should be ticks(50 Hz) * 20 ms = 1000 ms,
    // But it is simplified to ticks(50 Hz) * 16 = 800 ms
    // 50 Hz * 16 = 800 ~ 1000 ms / s
    return time( &ZX_Uno_t ) << 4;

}
