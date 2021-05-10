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

// File dialog using ESXDOS


#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include "integerTypes.h"

extern bool openFileDialog( uint8_t *message, uint8_t *path, int16_t maxPathLength, uint8_t attrs1, uint8_t attrs2 );

extern bool fileDialogConcatPath( uint8_t *string1, uint8_t *string2, uint16_t maxSize, bool appendBar );
extern void fileDialogpathUpOneDir( uint8_t *path );

extern uint16_t fileDialogIterateSDDirectory( uint8_t *dirPath, uint16_t firstEntry, uint16_t maxEntries, void *entryCallBack, uint16_t *totalEntries, bool *moreEntries, uint8_t *userData );

#endif /* FILEDIALOG_H */

