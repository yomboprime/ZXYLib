#
# ZXLib
#
# MIT License
#
# Copyright (c) 2021 Juan Jose Luna Espinosa
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.


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

srcFilesFileBrowser = src/textUtils.c src/zxuno/zxuno.c src/esxdos.c src/zxuno/uart.c src/zxuno/turbo.c src/network.c src/config.c src/retroProt/retroProtFile.c src/retroProt/retroProtFile_Download_esxdos.c examples/netGames/fileBrowser/fileBrowser.c

srcFilesFileBrowserPlus3DOS = src/textUtils.c src/zxuno/zxuno.c src/plus3dos.c src/zxuno/uart.c src/zxuno/turbo.c src/network.c src/retroProt/retroProtFile.c src/retroProt/retroProtFile_Download_p3dos.c src/configPlus3E.c examples/netGames/filePlus3EBrowser/filePlus3EBrowser.c

srcFilesWiFi_Test = src/textUtils.c src/zxuno/zxuno.c src/zxuno/uart.c src/zxuno/turbo.c src/network.c src/retroProt.c examples/netGames/WiFi_Test/WiFi_Test.c

srcFilesBlocksGame = src/textUtils.c src/zxuno/zxuno.c src/zxuno/uart.c src/zxuno/turbo.c src/network.c src/retroProt.c examples/netGames/blocksGame/blocksGame.c

srcFilesChessGame = src/textUtils.c src/zxuno/zxuno.c src/zxuno/uart.c src/zxuno/turbo.c src/network.c src/retroProt.c examples/netGames/chessGame/chessGame.c

srcFilesChat = src/textUtils.c src/zxuno/zxuno.c src/zxuno/uart.c src/zxuno/turbo.c src/network.c src/retroProt.c examples/netGames/chat/chat.c


# All the targets:
#all: generateBASICLoader createExample1 createExample2 createExample3 createExample4 createExample5 createExample6 createExample7 createExample8 createExample9 createExample10 createExample11 createExample12 createExample13 createExample14 createExample15 createExample16 createExample17 createExample18

all: createExample19 createExample19b

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

createExample19: compile19 concatenateTAPs19 createTAP19 copyTAP19

compile19:
	zcc +zx -o f19.bin -lndos -lmzx $(srcFilesFileBrowser) > ultimolog.txt

concatenateTAPs19:
	cat ./cargadorBinario/loader.bin f19.bin > fileBrowser.bin

createTAP19:
	./gentape/GenTape fileBrow.tap basic 'FILEBROW' 0 fileBrowser.bin > ultimolog.txt

copyTAP19:
	cp fileBrow.tap /home/yombo/devel/RetroProtServer/public/fileBrow.tap

#------------------------------------------------------------------------------

createExample19b: compile19b createTAP19b concatenateTAPs19b header19b copy19b
#generateWav19b playWav19b

compile19b:
	zcc +zx -o f19b.bin -lndos -lmzx $(srcFilesFileBrowserPlus3DOS) > ultimolog.txt

createTAP19b:
	$(node) ./bin2tap-js/bin2tap.js ../f19b.bin > ultimolog.txt

concatenateTAPs19b:
	cat ./cargadorBASIC/cargador.tap f19b.tap > filePlus.tap

generateWav19b:
	tape2wav filePlus.tap filePlus.wav > ultimolog.txt

header19b:
	cat cabeceraESXDOS/header f19b.bin > filePlus.bin

playWav19b:
#	/home/yombo/scriptsescritorio/zesarux.sh
	ffplay -nodisp -ss 3 -autoexit filePlus.wav

copy19b:
	cp filePlus.bin /home/yombo/devel/RetroProtServer/public/

#------------------------------------------------------------------------------


createExample20: compile20 concatenateTAPs20 createTAP20

compile20:
	zcc +zx -o f20.bin -lndos -lmzx $(srcFilesWiFi_Test) > ultimolog.txt

concatenateTAPs20:
	cat ./cargadorBinario/loader.bin f20.bin > testWiFi.bin

createTAP20:
	./gentape/GenTape testWiFi.tap basic 'WIFITEST' 0 testWiFi.bin > ultimolog.txt

copyTAP20:
	cp testWiFi.tap /home/yombo/devel/RetroProtServer/public/test.tap

#------------------------------------------------------------------------------

createExample21: compile21 concatenateTAPs21 createTAP21

compile21:
	zcc +zx -o f21.bin -lndos -lmzx $(srcFilesBlocksGame) > ultimolog.txt

concatenateTAPs21:
	cat ./cargadorBinario/loader.bin f21.bin > bloksGam.bin

createTAP21:
	./gentape/GenTape bloksGam.tap basic 'BLOKSGAM' 0 bloksGam.bin > ultimolog.txt

copyTAP21:
	cp bloksGam.tap /home/yombo/devel/RetroProtServer/public/test.tap



#------------------------------------------------------------------------------

createExample22: compile22 concatenateTAPs22 createTAP22

compile22:
	zcc +zx -o f22.bin -lndos -lmzx $(srcFilesChessGame) > ultimolog.txt

concatenateTAPs22:
	cat ./cargadorBinario/loader.bin f22.bin > cheSpeak.bin

createTAP22:
	./gentape/GenTape cheSpeak.tap basic 'CHESPEAK' 0 cheSpeak.bin > ultimolog.txt

copyTAP22:
	cp cheSpeak.tap /home/yombo/devel/RetroProtServer/public/test.tap

#------------------------------------------------------------------------------

createExample23: compile23 concatenateTAPs23 createTAP23

compile23:
	zcc +zx -o f23.bin -lndos -lmzx $(srcFilesChat) > ultimolog.txt

concatenateTAPs23:
	cat ./cargadorBinario/loader.bin f23.bin > chat.bin

createTAP23:
	./gentape/GenTape chat.tap basic 'CHAT' 0 chat.bin > ultimolog.txt

copyTAP23:
	cp chat.tap /home/yombo/devel/RetroProtServer/public/test.tap

#------------------------------------------------------------------------------

clean:
	$(RM) *.bin *.i *.tap *.op* *.o *.reloc *~ zcc_opt.def *.wav ./cargadorBASIC/cargador.tap
