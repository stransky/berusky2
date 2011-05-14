/*
  Stdio->zlib interface
*/
/*
#define  ZLIB_FILES

#ifdef  ZLIB_FILES
*/
#define FFILE gzFile

#define ffopen                  gzopen
#define ffread(buffer,s1,s2,f)  gzread(f,buffer,(s1)*(s2))
#define ffwrite(buffer,s1,s2,f) gzwrite(f,buffer,(s1)*(s2))
#define ffseek                  gzseek
#define fftell                  gztell
#define ffclose                 gzclose
#define ffputs(string,f)        gzputs(f,string)
#define ffgets(string,n,f)      gzgets(f,string,n)
#define ffputc(c,f)             gzputc(f,c)
#define ffgetc                  gzgetc
#define ffeof                   gzeof
/*
#else

#define FFILE                   FILE *

#define ffopen                  fopen
#define ffread(buffer,s1,s2,f)  fread(buffer,s1,s2,f)
#define ffwrite(buffer,s1,s2,f) fwrite(buffer,s1,s2,f)
#define ffseek                  fseek
#define fftell                  ftell
#define ffclose                 fclose
#define ffputs(string,f)        fputs(string,f)
#define ffgets(string,n,f)      fgets(string,n,f)
#define ffputc(c,f)             fputc(c,f)
#define ffgetc                  fgetc
#define ffeof                   feof

#endif
*/
