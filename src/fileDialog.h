/*
 * @author yomboprime
 *
 * File dialog using ESXDOS
*/


#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include "integerTypes.h"

extern bool openFileDialog( uint8_t *message, uint8_t *path, int16_t maxPathLength, uint8_t attrs1, uint8_t attrs2 );

extern bool fileDialogConcatPath( uint8_t *string1, uint8_t *string2, uint16_t maxSize, bool appendBar );
extern void fileDialogpathUpOneDir( uint8_t *path );

extern uint16_t fileDialogIterateSDDirectory( uint8_t *dirPath, uint16_t firstEntry, uint16_t maxEntries, void *entryCallBack, uint16_t *totalEntries, bool *moreEntries, uint8_t *userData );

#endif /* FILEDIALOG_H */

