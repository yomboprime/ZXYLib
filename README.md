# ZXYLib
C Library for ZX Spectrum with ZX-Uno special functions.

Contents in the src folder:

- textUtils.h, .c: Utils for displaying text and setting colors.
- esxdos.h, .c: Functions for accesing the SD card: get drive, open, stat, sync, delete, write or read to/from file, open and read directory, get/set CWD, get/set current file pos (seek).
- plus3dos.h, .c: Functions for accesing the SD card, for +3DOS/3e
- zxuno.h, .c: Register definitions, millis() time function
- uart.h, .c: Read/write to the ZX-Uno UART configured to 115200 bauds
- fileDialog.h, .c: Open a file dialog (normal zx text screen/menu) to select a file from the SD card.
- graphics.h, .c: ZX Spectrum normal graphic mode functions.
- radas.h, .c: ZX-Uno Radastan graphic mode functions.
- turbo.h, .c: ZX-Uno Turbo modes management.
- retroPro/*: Retroprot game and file protocol, see: https://github.com/yomboprime/Retroprot
