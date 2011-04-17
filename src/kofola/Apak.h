#ifndef __APAK___
#define __APAK___

#include "apak_types.h"
#include <stdlib.h>
#include <string.h>

APAK_HANDLE *apakopen(char *cArchive, char *cDir, int *pError);

void apakclose(APAK_HANDLE *pHandle);

void apakGetVersion(int *iLow, int *iHi);

//----------------------------------------------------------------------------------
// Tyto I/O funkce funguji stejne jako jejich standardni vzory
//----------------------------------------------------------------------------------
int achdir(APAK_HANDLE *pHandle, const char *dirname);

char *agetcwd(APAK_HANDLE *pHandle, char *buffer, int maxlen);

long afindfirst(APAK_HANDLE *pHandle, char *filespec, struct _finddata_t *fileinfo);

int afindnext(long handle, struct _finddata_t *fileinfo);

int afindclose(long handle);

FILE *aopen(APAK_HANDLE *pHandle, const char *filename, const char *mode);

int aclose(FILE *stream);

size_t aread(void *buffer, size_t size, size_t count, FILE *stream);

// works only with mapped stdin/out
size_t awrite(const void *buffer, size_t size, size_t count, FILE *stream);

char *agets(char *string, int n, FILE *stream);

int aputs(const char *string, FILE *stream);

int aeof( FILE *stream);

long atell(FILE *stream );

int aseek(FILE *stream, long offset, int origin);

//----------------------------------------------------------------------------------
// nasledujici funkce funguji pouze na astream
//----------------------------------------------------------------------------------
// vrati pointer na file buffer a jeho velikost
void agetbuffer(FILE *stream, char **pBuffer, apuLong *psize);

// vraci to stejne co _find_first, krome lastwrite a last acces
// void agetfiledata(FILE *stream, struct _finddata_t *fileinfo);

//----------------------------------------------------------------------------------
// vraci druh astreamu
// -1 ... error
//  0 ... premapovanej stand soubor
//  1 ... bufferovany soubor z PAKu (mozno napr. pouzit agetbuffer, getfiledata aj.)
int acheckstream(FILE *stream);

// vraci standardni FILE *, pokud je mapovan stdin/out. jinak vraci NULL
FILE *agetfile(FILE *stream);

// prevede std FILE na switch. Na navratovou hodnotu uz se nesmy volat fclose, fread,...
// ale aclose, aread, awrite, ....
FILE *afiletoswitch(FILE *stream);

// testuje zda je textovy soubor v uni code
int aunicode(FILE *stream);

#endif
