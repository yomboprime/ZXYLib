/*
 * @author yomboprime
 *
 * File dialog using ESXDOS
*/


#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include "integerTypes.h"

extern bool openFileDialog( uint8_t *message, uint8_t *path, int16_t maxPathLength, uint8_t attrs1, uint8_t attrs2 );


#endif /* FILEDIALOG_H */

