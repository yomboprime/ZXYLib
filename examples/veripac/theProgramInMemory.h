
#ifndef THEPROGRAMINMEMORY_H
#define THEPROGRAMINMEMORY_H

static uint8_t *theProgramInMemory = "\
#inicializamos registros\r\
6f0c\r\
6e30\r\
6c19\r\
6d16\r\
6a2d\r\
6917\r\
6403\r\
\r\
#RND 1\r\
08\r\
b0\r\
9e\r\
e0\r\
a0\r\
\r\
#RND 2\r\
08\r\
b1 #línea 20\r\
9e\r\
e1\r\
a1\r\
\r\
#RND 3\r\
08\r\
b2\r\
9e\r\
e2\r\
a2\r\
\r\
#RND 4\r\
08\r\
b3 #línea 30\r\
9e\r\
e3\r\
a3\r\
\r\
#Pintamos los 4 símbolos\r\
c0\r\
c1\r\
c2\r\
c3\r\
#Pausa sonora de 1 sg.\r\
05\r\
\r\
1451 #GoSub BorrarPant;81d\r\
\r\
#Tecla 1º símbolo\r\
75 #Línea 41\r\
84\r\
a5\r\
c5\r\
\r\
#Tecla 2º símbolo\r\
75\r\
84\r\
a6\r\
c6\r\
\r\
#Tecla 3º símbolo\r\
75\r\
84 #Línea 50\r\
a7\r\
c7\r\
\r\
#Tecla 4º símbolo\r\
75\r\
84\r\
a8\r\
c8\r\
\r\
#Comparo símbolos user > veripac\r\
cf #Print \"espacio separador\"\r\
90\r\
244a #NO ;74d\r\
91 #Línea 61\r\
254a #NO ;74d\r\
92\r\
264a #NO ;74d\r\
93\r\
274a #NO ;74d\r\
\r\
#Print \"SI\"\r\
cc\r\
cd\r\
124c #PausaTecla ;76d\r\
\r\
#Print \"NO\"\r\
ca\r\
c9\r\
#PausaTecla\r\
04 #Línea  76\r\
1451 #Borrar Pantalla; 81d\r\
\r\
1200 #Goto 00\r\
\r\
#Borrar Pantalla\r\
6b00 #81d\r\
cf\r\
cf\r\
cf\r\
cf\r\
cf\r\
cf\r\
cf\r\
6b00\r\
07";

#endif /* THEPROGRAMINMEMORY_H */

