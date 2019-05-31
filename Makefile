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

srcFilesExampleUARTTerminal = src/textUtils.c src/zxuno/uart.c src/zxuno/zxuno.c examples/exampleUARTTerminal.c

srcFilesExampleVeripac = src/textUtils.c src/esxdos.c src/zxuno/zxuno.c src/zxuno/turbo.c src/fileDialog.c src/graphics.c src/zxuno/veripac9.c examples/veripac/exampleVeripac9.c

srcFilesExampleVideoPlayer = src/textUtils.c src/esxdos.c src/zxuno/zxuno.c src/zxuno/turbo.c src/fileDialog.c src/zxuno/radas.c examples/exampleVideoPlayer.c

srcFilesExampleSRAMTest = src/textUtils.c src/zxuno/zxuno.c src/zxuno/turbo.c examples/exampleSRAMTest.c

srcFilesExampleWavBackground = src/textUtils.c src/esxdos.c src/zxuno/zxuno.c src/zxuno/turbo.c src/fileDialog.c src/zxuno/radas.c examples/exampleWavBackground.c

srcFilesExampleWavman = src/textUtils.c src/esxdos.c src/zxuno/zxuno.c src/zxuno/turbo.c src/fileDialog.c src/zxuno/wavunolite.c examples/wavman/exampleWavman.c

srcFilesExampleWiFiConfig = src/textUtils.c src/zxuno/uart.c src/zxuno/zxuno.c examples/exampleWiFiConfig.c

srcFilesExampleDrawTriangles = src/zxthree/screen/screen.c src/zxthree/raster/triangle/triangle.c src/zxthree/impl/zxradas/screen/scrradas.c src/zxuno/radas.c src/zxuno/turbo.c src/zxuno/zxuno.c examples/exampleDrawTriangles.c

srcFilesExampleSpeederGame = src/zxthree/screen/screen.c src/zxthree/raster/triangle/triangle.c src/zxthree/impl/zxradas/screen/scrradas.c src/zxuno/radas.c src/zxuno/turbo.c src/zxuno/zxuno.c examples/exampleSpeederGame.c

srcFilesExampleFelicesFiestas = src/zxthree/screen/screen.c src/zxthree/raster/triangle/triangle.c src/zxthree/impl/zxradas/screen/scrradas.c src/zxuno/radas.c src/zxuno/turbo.c src/zxuno/zxuno.c examples/exampleFelicesFiestas.c

srcFilesTestCoprocesador = src/textUtils.c src/zxuno/zxuno.c src/zxuno/coprocessor.c src/zxuno/turbo.c examples/testCoprocesador.c

srcFilesTestCoprocesador2 = src/zxthree/screen/screen.c src/zxthree/raster/triangle/triangle.c src/zxthree/impl/zxradas/screen/scrradas.c src/zxuno/radas.c src/zxuno/zxuno.c src/zxuno/coprocessor.c src/zxuno/turbo.c examples/testCoprocesador2.c

srcFilesTestMouse = src/zxthree/screen/screen.c src/zxthree/raster/triangle/triangle.c src/zxthree/impl/zxradas/screen/scrradas.c src/zxuno/radas.c src/zxuno/turbo.c src/zxuno/zxuno.c examples/exampleMouse.c

# All the targets:
all: generateBASICLoader createExample1 createExample2 createExample3 createExample4 createExample5 createExample6 createExample7 createExample8 createExample9 createExample10 createExample11 createExample12 createExample13 createExample14 createExample15 createExample16 createExample17 createExample18

# Targets:

generateBASICLoader:
	./bas2tap/bas2tap -q -a10 -sprogram ./cargadorBASIC/cargador.bas

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

createExample9: compile9 createTAP9 concatenateTAPs9 generateWavLeches9

compile9:
	zcc +zx -o f9.bin -lndos -lmzx $(srcFilesExampleSRAMTest) > ultimolog.txt

createTAP9:
	$(node) ./bin2tap-js/bin2tap.js ../f9.bin > ultimolog.txt

concatenateTAPs9:
	cat ./cargadorBASIC/cargador.tap f9.tap > SRAMTEST.tap

generateWav9:
	tape2wav ./SRAMTEST.tap ./SRAMTEST.wav > ultimolog.txt

generateWavLeches9:
	./CgLeches SRAMTEST.tap SRAMTEST.wav 3 > ultimolog.txt

#------------------------------------------------------------------------------

createExample10: compile10 createTAP10 concatenateTAPs10 generateWavLeches10

compile10:
	zcc +zx -o f10.bin -lndos -lmzx $(srcFilesExampleWavBackground) > ultimolog.txt

createTAP10:
	$(node) ./bin2tap-js/bin2tap.js ../f10.bin > ultimolog.txt

concatenateTAPs10:
	cat ./cargadorBASIC/cargador.tap f10.tap > WAVBACKG.tap

generateWav10:
	tape2wav ./WAVBACKG.tap ./WAVBACKG.wav > ultimolog.txt

generateWavLeches10:
	./CgLeches WAVBACKG.tap WAVBACKG.wav 3 > ultimolog.txt

#------------------------------------------------------------------------------

createExample11: compile11 createTAP11 concatenateTAPs11 generateWavLeches11

compile11:
	zcc +zx -o f11.bin -lndos -lmzx $(srcFilesExampleWavman) > ultimolog.txt

createTAP11:
	$(node) ./bin2tap-js/bin2tap.js ../f11.bin > ultimolog.txt

concatenateTAPs11:
	cat ./cargadorBASIC/cargador.tap f11.tap > WAVMAN.tap

generateWav11:
	tape2wav ./WAVMAN.tap ./WAVMAN.wav > ultimolog.txt

generateWavLeches11:
	./CgLeches WAVMAN.tap WAVMAN.wav 3 > ultimolog.txt

#------------------------------------------------------------------------------

createExample12: compile12 createTAP12 concatenateTAPs12 generateWavLeches12

compile12:
	zcc +zx -o f12.bin -lndos -lmzx $(srcFilesExampleWiFiConfig) > ultimolog.txt

createTAP12:
	$(node) ./bin2tap-js/bin2tap.js ../f12.bin > ultimolog.txt

concatenateTAPs12:
	cat ./cargadorBASIC/cargador.tap f12.tap > WIFICONF.tap

generateWav12:
	tape2wav ./WIFICONF.tap ./WIFICONF.wav > ultimolog.txt

generateWavLeches12:
	./CgLeches WIFICONF.tap WIFICONF.wav 3 > ultimolog.txt

#------------------------------------------------------------------------------

createExample13: compile13 createTAP13 concatenateTAPs13 generateWavLeches13

compile13:
	zcc +zx -o f13.bin -lndos -lmzx $(srcFilesExampleDrawTriangles) > ultimolog.txt

createTAP13:
	$(node) ./bin2tap-js/bin2tap.js ../f13.bin > ultimolog.txt

concatenateTAPs13:
	cat ./cargadorBASIC/cargador.tap f13.tap > DRAWTRIS.tap

generateWav13:
	tape2wav ./DRAWTRIS.tap ./DRAWTRIS.wav > ultimolog.txt

generateWavLeches13:
	./CgLeches DRAWTRIS.tap DRAWTRIS.wav 3 > ultimolog.txt

#------------------------------------------------------------------------------

createExample14: compile14 concatenateTAPs14 createTAP14 generateWavLeches14

compile14:
	zcc +zx -o f14.bin -lndos -lmzx $(srcFilesExampleSpeederGame) > ultimolog.txt

concatenateTAPs14:
	cat ./cargadorBinario/loader.bin f14.bin > SPEEDER.bin

#createTAP14:
#	$(node) ./bin2tap-js/bin2tap.js ../SPEEDER.bin > ultimolog.txt

createTAP14:
	./gentape/GenTape SPEEDER.tap basic 'SPEEDER' 0 SPEEDER.bin > ultimolog.txt

generateWavLeches14:
	./CgLeches SPEEDER.tap SPEEDER.wav 3 > ultimolog.txt

#------------------------------------------------------------------------------

createExample15: compile15 concatenateTAPs15 createTAP15 generateWavLeches15

compile15:
	zcc +zx -o f15.bin -lndos -lmzx $(srcFilesExampleFelicesFiestas) > ultimolog.txt

concatenateTAPs15:
	cat ./cargadorBinario/loader.bin f15.bin > FELICESF.bin

createTAP15:
	./gentape/GenTape FELICESF.tap basic 'FELICESF' 0 FELICESF.bin > ultimolog.txt

generateWavLeches15:
	./CgLeches FELICESF.tap FELICESF.wav 3 > ultimolog.txt

#------------------------------------------------------------------------------

createExample16: compile16 concatenateTAPs16 createTAP16 generateWavLeches16

compile16:
	zcc +zx -o f16.bin -lndos -lmzx $(srcFilesTestCoprocesador) > ultimolog.txt

concatenateTAPs16:
	cat ./cargadorBinario/loader.bin f16.bin > TESTCOPR.bin

createTAP16:
	./gentape/GenTape TESTCOPR.tap basic 'TESTCOPR' 0 TESTCOPR.bin > ultimolog.txt

generateWavLeches16:
	./CgLeches TESTCOPR.tap TESTCOPR.wav 3 > ultimolog.txt

#------------------------------------------------------------------------------

createExample17: compile17 concatenateTAPs17 createTAP17 generateWavLeches17

compile17:
	zcc +zx -o f17.bin -lndos -lmzx $(srcFilesTestCoprocesador2) > ultimolog.txt

concatenateTAPs17:
	cat ./cargadorBinario/loader.bin f17.bin > TESTCOP2.bin

createTAP17:
	./gentape/GenTape TESTCOP2.tap basic 'TESTCOP2' 0 TESTCOP2.bin > ultimolog.txt

generateWavLeches17:
	./CgLeches TESTCOP2.tap TESTCOP2.wav 3 > ultimolog.txt

#------------------------------------------------------------------------------

createExample18: compile18 concatenateTAPs18 createTAP18 generateWavLeches18

compile18:
	zcc +zx -o f18.bin -lndos -lmzx $(srcFilesTestMouse) > ultimolog.txt

concatenateTAPs18:
	cat ./cargadorBinario/loader.bin f18.bin > TESTMOUS.bin

createTAP18:
	./gentape/GenTape TESTMOUS.tap basic 'TESTMOUSE' 0 TESTMOUS.bin > ultimolog.txt

generateWavLeches18:
	./CgLeches TESTMOUS.tap TESTMOUS.wav 3 > ultimolog.txt

#------------------------------------------------------------------------------

clean:
	$(RM) *.bin *.i *.tap *.op* *.o *.reloc *~ zcc_opt.def *.wav ./cargadorBASIC/cargador.tap
