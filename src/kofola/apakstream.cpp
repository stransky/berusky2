#include <stdio.h>
#include <string.h>
#include "Apak.h"

extern void apakError(APAK_HANDLE * pHandle, char *cError);

int acheckstream(FILE * stream)
{
  APAK_STREAM_TYPE *pAStream = (APAK_STREAM_TYPE *) stream;

  if (!pAStream)
    return -1;

  if (pAStream->pFile)
    return 0;
  else if (!pAStream->pAHandle)
    return -1;
  else
    return 1;
}

void agetbuffer(FILE * stream, char **pBuffer, apuInt * psize)
{
  APAK_STREAM_TYPE *pAStream = (APAK_STREAM_TYPE *) stream;
  APAK_FILE_HANDLE *pFHandle;

  if (acheckstream(stream) < 1)
    return;

  pFHandle = (APAK_FILE_HANDLE *) pAStream->pAHandle;

  if (!pBuffer || !psize) {
    apakError(pFHandle->pArchive, "pBuffer = NULL and/or psize = NULL");
    return;
  }

  *pBuffer = pFHandle->pBuffer;
  *psize = pFHandle->pFileInfo->apuLRealSizeofFile;
}

FILE *agetfile(FILE * stream)
{
  APAK_STREAM_TYPE *pAStream = (APAK_STREAM_TYPE *) stream;

  if (acheckstream(stream) != 0)
    return NULL;

  return pAStream->pFile;
}

FILE *afiletoswitch(FILE * stream)
{
  APAK_STREAM_TYPE *pAStream =
    (APAK_STREAM_TYPE *) malloc(sizeof(APAK_STREAM_TYPE));

  if (!pAStream)
    return NULL;

  memset(pAStream, 0, sizeof(APAK_STREAM_TYPE));

  pAStream->pFile = stream;

  return (FILE *) pAStream;
}
