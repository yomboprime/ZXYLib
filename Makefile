# @author yomboprime
#
#
#
#


# Configure here your path to Node js:
node = node

# All the source files:
srcFilesExampleSDWrite = src/textUtils.c src/esxdos.c examples/exampleSDWrite.c

srcFilesExampleSDRead = src/textUtils.c src/esxdos.c examples/exampleSDRead.c

srcFilesExampleSDList = src/textUtils.c src/esxdos.c examples/exampleSDList.c

srcFilesExampleSDStat = src/textUtils.c src/esxdos.c examples/exampleSDStat.c

srcFilesExampleSDSeek = src/textUtils.c src/esxdos.c examples/exampleSDSeek.c

srcFilesExampleUARTTerminal = src/textUtils.c src/esxdos.c src/zxuno/uart.c src/zxuno/zxuno.c examples/exampleUARTTerminal.c

srcFilesExampleVeripac = src/textUtils.c src/esxdos.c src/zxuno/zxuno.c src/fileDialog.c src/graphics.c src/zxuno/veripac9.c examples/veripac/exampleVeripac9.c

srcFilesExampleVideoPlayer = src/textUtils.c src/esxdos.c src/zxuno/zxuno.c src/fileDialog.c src/zxuno/radas.c examples/exampleVideoPlayer.c

# All the targets:
all: generateBASICLoader createExample1 createExample2 createExample3 createExample4 createExample5 createExample6 createExample7 createExample8


# Targets:

generateBASICLoader:
	./bas2tap/bas2tap -q -a10 -sftpUno ./cargadorBASIC/cargador.bas

#------------------------------------------------------------------------------
createExample1: compile1 createTAP1 concatenateTAPs1 generateWavLeches1

compile1:
	zcc +zx -o f1.bin -lndos $(srcFilesExampleSDWrite) > ultimolog.txt

createTAP1:
	$(node) ./bin2tap-js/bin2tap.js ../f1.bin > ultimolog.txt

concatenateTAPs1:
	cat ./cargadorBASIC/cargador.tap f1.tap > ExampleSDWrite.tap

generateWav1:
	tape2wav ./ExampleWriteSDTest.tap ./ExampleSDWrite.wav > ultimolog.txt

generateWavLeches1:
	./CgLeches ExampleSDWrite.tap ExampleSDWrite.wav 3 > ultimolog.txt

#------------------------------------------------------------------------------
createExample2: compile2 createTAP2 concatenateTAPs2 generateWavLeches2

compile2:
	zcc +zx -o f2.bin -lndos $(srcFilesExampleSDRead) > ultimolog.txt

createTAP2:
	$(node) ./bin2tap-js/bin2tap.js ../f2.bin > ultimolog.txt

concatenateTAPs2:
	cat ./cargadorBASIC/cargador.tap f2.tap > ExampleSDRead.tap

generateWav2:
	tape2wav ./ExampleSDRead.tap ./ExampleSDRead.wav > ultimolog.txt

generateWavLeches2:
	./CgLeches ExampleSDRead.tap ExampleSDRead.wav 3 > ultimolog.txt

#------------------------------------------------------------------------------
createExample3: compile3 createTAP3 concatenateTAPs3 generateWavLeches3

compile3:
	zcc +zx -o f3.bin -lndos $(srcFilesExampleSDList) > ultimolog.txt

createTAP3:
	$(node) ./bin2tap-js/bin2tap.js ../f3.bin > ultimolog.txt

concatenateTAPs3:
	cat ./cargadorBASIC/cargador.tap f3.tap > ExampleSDList.tap

generateWav3:
	tape2wav ./ExampleSDList.tap ./ExampleSDList.wav > ultimolog.txt

generateWavLeches3:
	./CgLeches ExampleSDList.tap ExampleSDList.wav 3 > ultimolog.txt

#------------------------------------------------------------------------------
createExample4: compile4 createTAP4 concatenateTAPs4 generateWavLeches4

compile4:
	zcc +zx -o f4.bin -lndos $(srcFilesExampleSDStat) > ultimolog.txt

createTAP4:
	$(node) ./bin2tap-js/bin2tap.js ../f4.bin > ultimolog.txt

concatenateTAPs4:
	cat ./cargadorBASIC/cargador.tap f4.tap > ExampleSDStat.tap

generateWav4:
	tape2wav ./ExampleSDStat.tap ./ExampleSDStat.wav > ultimolog.txt

generateWavLeches4:
	./CgLeches ExampleSDStat.tap ExampleSDStat.wav 3 > ultimolog.txt

#------------------------------------------------------------------------------
createExample5: compile5 createTAP5 concatenateTAPs5 generateWavLeches5

compile5:
	zcc +zx -o f5.bin -lndos $(srcFilesExampleSDSeek) > ultimolog.txt

createTAP5:
	$(node) ./bin2tap-js/bin2tap.js ../f5.bin > ultimolog.txt

concatenateTAPs5:
	cat ./cargadorBASIC/cargador.tap f5.tap > ExampleSDSeek.tap

generateWav5:
	tape2wav ./ExampleSDSeek.tap ./ExampleSDSeek.wav > ultimolog.txt

generateWavLeches5:
	./CgLeches ExampleSDSeek.tap ExampleSDSeek.wav 3 > ultimolog.txt


#------------------------------------------------------------------------------

createExample6: compile6 createTAP6 concatenateTAPs6 generateWavLeches6

compile6:
	zcc +zx -o f6.bin -lndos $(srcFilesExampleUARTTerminal) > ultimolog.txt

createTAP6:
	$(node) ./bin2tap-js/bin2tap.js ../f6.bin > ultimolog.txt

concatenateTAPs6:
	cat ./cargadorBASIC/cargador.tap f6.tap > UARTTERM.tap

generateWav6:
	tape2wav ./UARTTERM.tap ./UARTTERM.wav > ultimolog.txt

generateWavLeches6:
	./CgLeches UARTTERM.tap UARTTERM.wav 3 > ultimolog.txt

#------------------------------------------------------------------------------

createExample7: compile7 createTAP7 concatenateTAPs7 generateWavLeches7

compile7:
	zcc +zx -o f7.bin -lndos -lmzx $(srcFilesExampleVeripac) > ultimolog.txt

createTAP7:
	$(node) ./bin2tap-js/bin2tap.js ../f7.bin > ultimolog.txt

concatenateTAPs7:
	cat ./cargadorBASIC/cargador.tap f7.tap > VERIPAC9.tap

generateWav7:
	tape2wav ./VERIPAC9.tap ./VERIPAC9.wav > ultimolog.txt

generateWavLeches7:
	./CgLeches VERIPAC9.tap VERIPAC9.wav 3 > ultimolog.txt

#------------------------------------------------------------------------------

createExample8: compile8 createTAP8 concatenateTAPs8 generateWavLeches8

compile8:
	zcc +zx -o f8.bin -lndos -lmzx $(srcFilesExampleVideoPlayer) > ultimolog.txt

createTAP8:
	$(node) ./bin2tap-js/bin2tap.js ../f8.bin > ultimolog.txt

concatenateTAPs8:
	cat ./cargadorBASIC/cargador.tap f8.tap > PLAYVID.tap

generateWav8:
	tape2wav ./PLAYVID.tap ./PLAYVID.wav > ultimolog.txt

generateWavLeches8:
	./CgLeches PLAYVID.tap PLAYVID.wav 3 > ultimolog.txt

#------------------------------------------------------------------------------

clean:
	$(RM) *.bin *.i *.tap *.op* *.o *.reloc *~ zcc_opt.def *.wav ./cargadorBASIC/cargador.tap
