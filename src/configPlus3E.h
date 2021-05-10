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

#ifndef CONFIG_PLUS3E_H
#define CONFIG_PLUS3E_H

#include <stdio.h>
#include "integerTypes.h"

#define PARAM_NAME_LENGTH 32
#define PARAM_VALUE_LENGTH 128

#define CONFIG_OK				( - 1 )
#define CONFIG_ERROR_NOTFOUND	( - 2 )
#define CONFIG_ERROR_READING	( - 3 )
#define CONFIG_ERROR_CANTCLOSE	( - 4 )
#define CONFIG_ERROR_VALIDATION	( - 5 )

extern int loadConfigFile( uint8_t *configFilePath, void *paramCallback, uint8_t *fileBuffer, uint16_t bufferSize );
extern uint8_t *configError( int error );

#endif /* CONFIG_PLUS3E_H */
