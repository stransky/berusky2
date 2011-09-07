#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void convert(char *p_file)
{
  char file_in[256];
  char file_out[256];

  printf("Converting %s...\n", p_file);

  strcpy(file_in, p_file);
  strcpy(file_out, p_file);
  strcat(file_out, ".out");

  FILE *f_in = fopen(file_in,"rb");
  FILE *f_out = fopen(file_out,"wb");

  int c;
  while((c = fgetc(f_in)) && c != EOF) {
    if(c != 0xd)
      fputc(c, f_out);
  }
  
  fclose(f_in);
  fclose(f_out);

  rename(file_out, file_in);
}

int main(int argc, char **argv)
{
  int i;
  for(i = 1; i < argc; i++) {
    convert(argv[i]);
  }
}
