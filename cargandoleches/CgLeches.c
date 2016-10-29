#include <stdlib.h>
#include <stdio.h>
#include <string.h>

FILE *fi, *fo, *ft;
unsigned char in[0x10000], mem[0x20000];
int i, j, k, tlength;
unsigned short length, param, frequency= 44100;
char tzx= 0, pzx = 0, wav= 0, channel_type= 1, velo= 3, offset= 0, command[0x100], *ext;

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

int main(int argc, char* argv[]){
  if( argc==1 )
    printf("\n"
    "CgLeches v1.00, an ultra load block generator by Antonio Villena, 17 Sep 2014\n\n"
    "  CgLeches <ifile> <ofile> [speed] [channel_type] [srate] [offset]\n\n"
    "  <ifile>        TAP input file, mandatory\n"
    "  <ofile>        Output file, between TZX, PZX or WAV file, mandatory\n"
    "  [speed]        A number between 0 and 7. Lower is faster. Default 3\n"
    "  [channel_type] Possible values are: mono (default), stereo or stereoinv\n"
    "  [srate]        Sample rate, 44100 (default) or 48000\n"
    "  [offset]       -2,-1,0 (default),1 or 2. Fine grain adjust for symbol offset\n\n"),
    exit(0);
  if( argc<3 || argc>7 )
    printf("\nInvalid number of parameters\n"),
    exit(-1);
  fi= fopen(argv[1], "rb");
  if( !fi )
    printf("\nInput file not found: %s\n", argv[1]),
    exit(-1);
  fseek(fi, 0, SEEK_END);
  tlength= ftell(fi);
  fseek(fi, 0, SEEK_SET);
  if( !(ext= strchr(argv[2], '.')) )
    printf("\nInvalid argument name: %s\n", argv[2]),
    exit(-1);
  fo= fopen(argv[2], "wb+");
  if( !fo )
    printf("\nCannot create output file: %s\n", argv[2]),
    exit(-1);
  argc>3 && (velo= atoi(argv[3]));
  if( velo>7 )
    printf("\nInvalid speed: %s\n", argv[3]),
    exit(-1);
  if( argc>4 ){
    if( !strcasecmp(argv[4], "mono") )
      channel_type= 1;
    else if( !strcasecmp(argv[4], "stereo") )
      channel_type= 2;
    else if( !strcasecmp(argv[4], "stereoinv") )
      channel_type= 6;
    else
      printf("\nInvalid argument name: %s\n", argv[4]),
      exit(-1);
  }
  argc>5 && (frequency= atoi(argv[5]));
  if( frequency!=44100 && frequency!=48000 )
    printf("\nInvalid sample rate: %s\n", argv[5]),
    exit(-1);
  argc>6 && (offset= atoi(argv[6]));
  if( offset<-2 || offset>2 )
    printf("\nInvalid offset: %s\n", argv[6]),
    exit(-1);
  if( !strcasecmp((char *)strchr(argv[2], '.'), ".tzx" ) )
    fprintf( fo, "ZXTape!" ),
    *(int*)mem= 0xa011a,
    fwrite(mem, ++tzx, 3, fo);
  else if ( !strcasecmp((char *)strchr(argv[2], '.'), ".pzx" ) ){
    fprintf (fo, "PZX 1.0\n\n");
    pzx=1;
  }
  else if( !strcasecmp((char *)strchr(argv[2], '.'), ".wav" ) ){
    memset(mem, wav++, 44);
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
    printf("\nInvalid file extension: %s\n", argv[2]),
    exit(-1);

  ext++;
  while ( tlength ){
    fread(&length, 1, 2, fi);
    fread(in, 1, length, fi);
    ft= fopen("_tmp.tap", "wb+");
    fwrite(in+1, 1, length-2, ft);
    fclose(ft);
#ifdef __linux__
    sprintf(command, "./leches %d %s tmp.%s %02x %d %d 100 %d _tmp.tap > nul\n", frequency,
            channel_type-1 ? (channel_type-2?"stereoinv":"stereo") : "mono",
            ext, in[0], velo, offset, in[0] ? 2000 : 200);
    if( system(command) )
      printf("\nError: leches not found \n"),
      exit(-1);
#else
    sprintf(command, "leches %d %s tmp.%s %02x %d %d 100 %d _tmp.tap > nul\n", frequency,
            channel_type-1 ? (channel_type-2?"stereoinv":"stereo") : "mono",
            ext, in[0], velo, offset, in[0] ? 2000 : 200);
    if( system(command) )
      printf("\nError: leches.exe not found \n"),
      exit(-1);
#endif
    else{
      sprintf(command, "tmp.%s", ext);
      ft= fopen(command, "rb");
      if( ft ){
        if( tzx )
        {
          fseek(ft, 0, SEEK_END);
          i= ftell(ft)-10;
          fseek(ft, 10, SEEK_SET);
        }
        else if (pzx)
        {
          fseek(ft, 0, SEEK_END);
          i= ftell(ft);
          fseek(ft, 0, SEEK_SET);
        }
        else
        {
          fread(mem, 1, 44, ft);
          i= *(int*)(mem+40);
        }
        j= i>>16;
        k= i&0xffff;
        for ( i= 0; i<j; i++ )
        {
          fread(in, 1, 0x10000, ft);
          fwrite(in, 1, 0x10000, fo);
        }
        fread(in, 1, k, ft);
        fwrite(in, 1, k, fo);
        fclose(ft);
      }
      else
        printf("\nError: plug doesn't generate valid file\n"),
        exit(-1);
    }
    tlength-= 2+length;
  }
  if( remove(command) || remove("_tmp.tap") )
    printf("\nError: deleting \n"),
    exit(-1);
  if( wav )
    i= ftell(fo)-8,
    fseek(fo, 4, SEEK_SET),
    fwrite(&i, 4, 1, fo),
    i-= 36,
    fseek(fo, 40, SEEK_SET),
    fwrite(&i, 4, 1, fo);
  fclose(fo);
}