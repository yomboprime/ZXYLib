
#ifndef __BITS__
#define __BITS__

#include "retrosys.h"

#ifdef RETRO_SYSTEM_PC_DOS

#include <mem.h>

typedef unsigned char uint8_t;
typedef char int8_t;

typedef unsigned int uint16_t;
typedef int int16_t;

typedef unsigned long uint32_t;
typedef long int32_t;

typedef unsigned char bool;

enum { false, true };

#endif

#ifdef RETRO_SYSTEM_ZX_UNO

#include "../integerTypes.h"

#endif

#endif //__BITS__