#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned char table1[][4]=
  { {1, 2, 2, 3}, {2, 2, 3, 3}, {2, 3, 3, 4}, {3, 3, 4, 4},
    {1, 2, 3, 4}, {2, 3, 4, 5}, {2, 3, 4, 5}, {3, 4, 5, 6}};
unsigned char table2[][4]=
  { {1, 1, 2, 2}, {1, 2, 2, 3}, {2, 2, 3, 3}, {2, 3, 3, 4},
    {1, 2, 3, 4}, {1, 2, 3, 4}, {2, 3, 4, 5}, {2, 3, 4, 5}};
unsigned char byvel[][8]=
  { { 0xed, 0xde, 0xd2, 0xc3, 0x00, 0x71, 0x62, 0x53 },
    { 0xf1, 0xe5, 0xd6, 0xc7, 0x04, 0x78, 0x69, 0x5d }};
unsigned char termin[][8]=
  { { 21, 22, 23, 24, 23, 24, 25, 26 },
    { 13, 14, 15, 16, 15, 16, 17, 18 }};
unsigned char *mem, *precalc;
unsigned char inibit= 0, tzx= 0, pzx = 0, channel_type= 1, checksum, mlow, velo, refconf;
FILE *fi, *fo;
int i, j, k, flag, ind= 0;
unsigned short length, outbyte= 1, frequency, pilotts, pilotpulses;

int strcasecmp(const char *a, const char *b){
  int ca, cb;
  do{
    ca= *a++ & 0xff;
    cb= *b++ & 0xff;
    if (ca >= 'A' && ca <= 'Z')
      ca+= 'a' - 'A';
    if (cb >= 'A' && cb <= 'Z')
      cb+= 'a' - 'A';
  } while ( ca == cb && ca != '\0' );
  return ca - cb;
}

void outbits( short val ){
  if( tzx )
    for ( i= 0; i<val; i++ )
      if( outbyte>0xff )
        precalc[ind++]= outbyte&0xff,
        outbyte= 2 | inibit;
      else
        outbyte<<= 1,
        outbyte|= inibit;
  else if (pzx)
  {
    fprintf (fo, "PULSE %d\n", val*((mlow)?73:79));
  }
  else
    for ( i= 0; i<val; i++ ){
      precalc[ind++]= inibit ? 0xc0 : 0x40;
      if( channel_type==2 )
        precalc[ind++]= inibit ? 0xc0 : 0x40;
      else if( channel_type==6 )
        precalc[ind++]= inibit ? 0x40 : 0xc0;
    }
  if( ind>0xff000 )
    fwrite( precalc, 1, ind, fo ),
    ind= 0;
  inibit^= 1;
}

char char2hex(char value, char * name){
  if( value<'0' || value>'f' || value<'A' && value>'9' || value<'a' && value>'F' )
    printf("\nInvalid character %c or '%s' not exists\n", value, name),
    exit(-1);
  return value>'9' ? 9+(value&7) : value-'0';
}

int parseHex(char * name, int index){
  int flen= strlen(name);
  if( name[0]=='\'' )
    for ( i= 1; i<11 && name[i]!='\''; ++i )
      mem[i+6]= name[i];
  else if( ~flen & 1 ){
    flen>>= 1;
    flen>10 && index==7 && (flen= 10);
    for ( i= 0; i < flen; i++ )
      mem[i+index]= char2hex(name[i<<1|1], name) | char2hex(name[i<<1], name) << 4;
    ++i;
  }
  while( ++i<12 )
    mem[i+5]= ' ';
  return flen;
}

int main(int argc, char* argv[]){
  mem= (unsigned char *) malloc (0x20000);
  if( argc==1 )
    printf("\n"
    "leches v0.03, an ultra load block generator by Antonio Villena, 17 Sep 2014\n\n"
    "  leches <srate> <channel_type> <ofile> <flag> <pilot_ms> <pause_ms> <ifile>\n\n"
    "  <srate>         Sample rate, 44100 or 48000. Default is 44100\n"
    "  <channel_type>  Possible values are: mono (default), stereo or stereoinv\n"
    "  <ofile>         Output file, between TZX, PZX or WAV file\n"
    "  <flag>          Flag byte, 00 for header, ff or another for data blocks\n"
    "  <speed>         Between 0 and 7. [0..3] for Safer and [4..7] for Reckless\n"
    "  <offset>        -2,-1,0,1 or 2. Fine grain adjust for symbol offset\n"
    "  <pilot_ms>      Duration of pilot in milliseconds\n"
    "  <pause_ms>      Duration of pause after block in milliseconds\n"
    "  <ifile>         Hexadecimal string or filename as data origin of that block\n\n"),
    exit(0);
  if( argc!=10 )
    printf("\nInvalid number of parameters\n"),
    exit(-1);
  frequency= atoi(argv[1]);
  if( frequency!=44100 && frequency!=48000 )
    printf("\nInvalid sample rate: %d\n", frequency),
    exit(-1);
  if( !strcasecmp(argv[2], "mono") )
    channel_type= 1;
  else if( !strcasecmp(argv[2], "stereo") )
    channel_type= 2;
  else if( !strcasecmp(argv[2], "stereoinv") )
    channel_type= 6;
  else
    printf("\nInvalid argument name: %s\n", argv[2]),
    exit(-1);
  fo= fopen(argv[3], "wb+");
  if( !fo )
    printf("\nCannot create output file: %s\n", argv[3]),
    exit(-1);
  precalc= (unsigned char *) malloc (0x200000);
  if( !strcasecmp((char *)strchr(argv[3], '.'), ".tzx" ) )
    ++tzx;
  else if ( !strcasecmp((char *)strchr(argv[3], '.'), ".pzx" ) )
    ++pzx;
  else if( !strcasecmp((char *)strchr(argv[3], '.'), ".wav" ) ){
    memset(mem, 0, 44);
    memset(precalc, 128, 0x200000);
    *(int*)mem= 0x46464952;
    *(int*)(mem+8)= 0x45564157;
    *(int*)(mem+12)= 0x20746d66;
    *(char*)(mem+16)= 0x10;
    *(char*)(mem+20)= 0x01;
    *(char*)(mem+22)= *(char*)(mem+32)= channel_type&3;
    *(short*)(mem+24)= frequency;
    *(int*)(mem+28)= frequency*(channel_type&3);
    *(char*)(mem+34)= 8;
    *(int*)(mem+36)= 0x61746164;
    fwrite(mem, 1, 44, fo);
  }
  else
    printf("Output format not allowed, use only TZX, PZX or WAV\n"),
    exit(-1);
  mlow= frequency==48000 ? 1 : 0;
  pilotts= mlow ? 876 : 948;  // era 875 : 952
  pilotpulses= atof(argv[7])*3500/pilotts+0.5;
  pilotpulses&1 || ++pilotpulses;  // queremos que pilotpulses siempre sea impar
  fi= fopen(argv[9], "rb");
  if( fi )
    length= fread(mem, 1, 0x20000, fi);
  else
    length= parseHex(argv[9], 0);
  velo= atoi(argv[5]);
  refconf= (byvel[mlow][velo]                &128)
         + (byvel[mlow][velo]+3*atoi(argv[6])&127);
  for ( checksum= i= 0; i<length; i++ )
    checksum^= mem[i];
  if( tzx )
    fprintf( fo, "ZXTape!" ),
    *(int*)precalc= 0xa011a,
    precalc[3]= 0x12,
    *(short*)(precalc+4)= pilotts,
    *(short*)(precalc+6)= pilotpulses,
    precalc[8]= 0x15,
    *(short*)(precalc+9)= mlow ? 73 : 79,
    *(short*)(precalc+11)= atof(argv[8]),
    ind= 17;
  else if (pzx)
  {
    fprintf (fo, "PULSES\n"
                 "PULSE %d %d\n", pilotts, pilotpulses);
  }
  else
    while( pilotpulses-- )
      outbits( 12 );
  outbits( 28 );
  pilotpulses= 6;
  while( pilotpulses-- )
    outbits( 12 );
  outbits( 2 );
  outbits( mlow ? 4 : 8 );
  flag= refconf | strtol(argv[4], NULL, 16)<<8 | checksum<<16;
  for ( j= 0; j<24; j++, flag<<= 1 )
    outbits( k= flag&0x800000 ? 4 : 8 ),
    outbits( k );
  outbits( 2 );
  outbits( 3 );
  --mem;
  while( length-- )
    outbits( table1[velo][*++mem>>6  ] ),
    outbits( table2[velo][  *mem>>6  ] ),
    outbits( table1[velo][*mem>>4 & 3] ),
    outbits( table2[velo][*mem>>4 & 3] ),
    outbits( table1[velo][*mem>>2 & 3] ),
    outbits( table2[velo][*mem>>2 & 3] ),
    outbits( table1[velo][*mem    & 3] ),
    outbits( table2[velo][*mem    & 3] );
  outbits( termin[mlow][velo]>>1 );
  outbits( termin[mlow][velo]-(termin[mlow][velo]>>1) );
  outbits( 1 );
  outbits( 1 );
  if( tzx ){
    for ( j= 8; outbyte<0x100; outbyte<<= 1, --j );
    precalc[ind++]= outbyte;
    fwrite( precalc, 1, ind, fo );
    i= j | (ftell(fo)-24)<<8;
    fseek(fo, 20, SEEK_SET);
    fwrite(&i, 4, 1, fo);
  }
  else if (pzx)
    outbits (20000);  // pausa?
  else
    fwrite( precalc, 1, ind, fo ),
    fwrite( precalc+0x100000, 1, frequency*(channel_type&3)*atof(argv[8])/1000, fo),
    i= ftell(fo)-8,
    fseek(fo, 4, SEEK_SET),
    fwrite(&i, 4, 1, fo),
    i-= 36,
    fseek(fo, 40, SEEK_SET),
    fwrite(&i, 4, 1, fo);
  fclose(fi);
  fclose(fo);
  printf("\nFile generated successfully\n");
  exit(0);
}