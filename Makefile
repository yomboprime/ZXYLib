#    This file is part of ftpUno
# 
#    ftpUno  is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.



# Configure here your path to Node js:
node = /media/datos/soft/nodejs/node-v4.4.5-linux-x64/bin/node

# All the source files:
srcFilesWriteSDTest = src/textUtils.c src/zxuno/zxuno.c src/zxuno/uart.c src/esxdos.c examples/writeSDTest.c

srcFilesReadSDTest = src/textUtils.c src/zxuno/zxuno.c src/zxuno/uart.c src/esxdos.c examples/readSDTest.c

# All the targets:
all: generateBASICLoader createExample1 createExample2


# Targets:

generateBASICLoader:
	./bas2tap/bas2tap -q -a10 -sftpUno ./cargadorBASIC/cargador.bas

createExample1: compile1 createTAP1 concatenateTAPs1 generateWavLeches1

createExample2: compile2 createTAP2 concatenateTAPs2 generateWavLeches2

compile1:
	zcc +zx -o f1.bin -lndos $(srcFilesWriteSDTest) > ultimolog.txt

compile2:
	zcc +zx -o f2.bin -lndos $(srcFilesReadSDTest) > ultimolog.txt

createTAP1:
	$(node) ./bin2tap-js/bin2tap.js ../f1.bin > ultimolog.txt

createTAP2:
	$(node) ./bin2tap-js/bin2tap.js ../f2.bin > ultimolog.txt

concatenateTAPs1:
	cat ./cargadorBASIC/cargador.tap f1.tap > ExampleWriteSDTest.tap

concatenateTAPs2:
	cat ./cargadorBASIC/cargador.tap f2.tap > ExampleReadSDTest.tap

generateWav1:
	tape2wav ./ExampleWriteSDTest.tap ./ExampleWriteSDTest.wav > ultimolog.txt

generateWavLeches1:
	./CgLeches ExampleWriteSDTest.tap ExampleWriteSDTest.wav 3 > ultimolog.txt

generateWav2:
	tape2wav ./ExampleReadSDTest.tap ./ExampleReadSDTest.wav > ultimolog.txt

generateWavLeches2:
	./CgLeches ExampleReadSDTest.tap ExampleReadSDTest.wav 3 > ultimolog.txt

clean:
	$(RM) *.bin *.i *.tap *.op* *.o *.reloc *~ zcc_opt.def *.wav ./cargadorBASIC/cargador.TAP
