# ZXYLib
C Library for ZX Spectrum with ZX-Uno special functions.

Contents in the src folder:

- textUtils.h, .c: Utils for displaying text and setting colors.
- esxdos.h, .c: Functions for accesing the SD card: get drive, open, stat, sync, delete, write or read to/from file, open and read directory, get/set CWD, get/set current file pos (seek).
- zxuno.h, .c: Register definitions, millis() time function
- uart.h, .c: Read/write to the ZX-Uno UART configured to 115200 bauds
- fileDialog.h, .c: Open a file dialog (normal zx text screen/menu) to select a file from the SD card.
- radas.h, .c: Basic Radastan graphic mode functions.