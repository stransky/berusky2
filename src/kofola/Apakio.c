#include <stdio.h>
#include <string.h>
#include "Apak.h"

#define APAKMAKEWORD(a, b)      ((unsigned short)(((unsigned char)(a)) | ((unsigned short)((unsigned char)(b))) << 8))

extern void apakError(APAK_HANDLE *pHandle, char *cError);
extern int  apakReadError(int iError, APAK_HANDLE	*pHandle);

int apakDir(APAK_HANDLE *pHandle, char *cFileName)
{
	char	buffer[256];
	int		j, i = strlen(cFileName);

	for(j = i; j >= 0; j--)
		if(cFileName[j] == '\\')
			break;

	if(j <= 0)
		return 1;

	strncpy(buffer, cFileName, j);
	buffer[j] = '\0';

	if(achdir(pHandle, buffer) == -1)
		return 0;

	strncpy(buffer, &cFileName[j+1], strlen(cFileName) - j);

	buffer[strlen(cFileName) - j] = '\0';

	strcpy(cFileName, buffer);

	return 1;
}

APAKFILE *apakFile(APAK_HANDLE *pHandle, char *cFileName)
{
	int i;

	if(!pHandle->pActualNode->apuLSizeofFile)
		return NULL;

	for(i=0;(unsigned)i<pHandle->pActualNode->apuLSizeofFile;i++)
		if(!strcasecmp(cFileName, pHandle->pActualNode->apakFile[i].cName))
			return &pHandle->pActualNode->apakFile[i];

	return NULL;
}

int fillbuffer(APAK_FILE_HANDLE *pFHandle)
{
	int err = 0;
	char *pTmp;
	unsigned int rsize, size;

	rsize = pFHandle->pFileInfo->apuLRealSizeofFile;
	size = pFHandle->pFileInfo->apuLSizeofFile;

	pTmp = (char *) malloc(size);
	
	if(!pTmp)
		return 0;

	fseek(pFHandle->pArchive->pFILE, pFHandle->apuLfStartofFile, SEEK_SET);
	fread(pTmp, size, 1, pFHandle->pArchive->pFILE);

/*
ZEXTERN int ZEXPORT compress OF((Bytef *dest,   uLongf *destLen,
                                 const Bytef *source, uLong sourceLen));
*/
	if(pFHandle->pFileInfo->bNotCompressed)
		memcpy(pFHandle->pBuffer, pTmp, size);
	else
		err = uncompress(pFHandle->pBuffer, &rsize, pTmp, size);
    
	free((void *) pTmp);

	if(err == Z_MEM_ERROR ||
	   err == Z_BUF_ERROR ||
	   err == Z_DATA_ERROR)
		return 0;   

	return 1;
}

FILE *aopen(APAK_HANDLE *pHandle, const char *filename, const char *mode )
{
	APAKNODE		 *pNode;
	APAK_FILE_HANDLE *pFHandle = NULL;
	APAK_STREAM_TYPE *pAStream = NULL;
	
	pAStream = (APAK_STREAM_TYPE *)malloc(sizeof(APAK_STREAM_TYPE));

	if(!pAStream)
		return 0;

	memset(pAStream, 0, sizeof(APAK_STREAM_TYPE));

	// strandard i/o stream (fopen, fclose, ....)
	if(!pHandle)
	{
		pAStream->pFile = fopen(filename, mode);

		if(!pAStream->pFile)
		{
			free((void *) pAStream);
			pAStream = NULL;
		}

		return (FILE *) pAStream;
	}

	// APAK stream (aread, agets, aclose, ....)
	if(mode[0] == 'w')
	{
		apakError(pHandle, "It's impossible to write into archive");
		free((void *) pAStream);
		return 0;
	}

	if(!filename)
	{
		apakError(pHandle, "filename = NULL");
		free((void *) pAStream);
		return 0;
	}

	if(!strlen(filename))
	{
		apakError(pHandle, "filename length = 0");
		free((void *) pAStream);
		return 0;
	}

	pNode = pHandle->pActualNode;

	if(!apakDir(pHandle, (char *)filename))
	{
		apakError(pHandle, "File not found");
		free((void *) pAStream);
		return 0;
	}

	pFHandle = (APAK_FILE_HANDLE *)malloc(sizeof(APAK_FILE_HANDLE));

	if(!pFHandle)
	{
		apakError(pHandle, "Unable to allocate memory for file handle");
		return 0;
		free((void *) pAStream);
	}

	memset(pFHandle, 0, sizeof(APAK_FILE_HANDLE));

	pFHandle->pFileInfo = apakFile(pHandle, (char *)filename);

	if(!pFHandle->pFileInfo)
	{
		free((void *) pFHandle);
		free((void *) pAStream);
		apakError(pHandle, "File not found");
		pHandle->pActualNode = pNode;
		return 0;
	}

	pFHandle->apuLfCursor = 0;
	pFHandle->apuLfStartofFile = pFHandle->pFileInfo->apuLFileLocation;
	pFHandle->apuLfEndofFile = pFHandle->pFileInfo->apuLFileLocation + pFHandle->pFileInfo->apuLSizeofFile;
	pFHandle->pArchive = pHandle;
	pFHandle->bEof = 0;

	pHandle->pActualNode = pNode;

	pFHandle->pBuffer = malloc(pFHandle->pFileInfo->apuLRealSizeofFile);

	if(!pFHandle->pBuffer)
	{
		free((void *) pFHandle);
		free((void *) pAStream);
		apakError(pHandle, "Can't allocate memory for internal buffer");
		pHandle->pActualNode = pNode;
		return 0;
	}

	memset(pFHandle->pBuffer, 0, pFHandle->pFileInfo->apuLSizeofFile);

	if(!fillbuffer(pFHandle))
	{
		free((void *) pFHandle);
		free((void *) pFHandle->pBuffer);
		free((void *) pAStream);
		apakError(pHandle, "Can't fill internal buffer");
		pHandle->pActualNode = pNode;
		return 0;
	}

	pAStream->pAHandle = pFHandle;

	return (FILE *) pAStream;
}

int aclose(FILE *stream)
{
	APAK_STREAM_TYPE *pAStream = (APAK_STREAM_TYPE *) stream;
	APAK_FILE_HANDLE *pFHandle;

	if(!pAStream)
		return 0;

	if(pAStream->pFile)
	{
		free((void *) pAStream);
		return fclose(pAStream->pFile);
	}

	pFHandle = (APAK_FILE_HANDLE *) pAStream->pAHandle;

	if(!pFHandle)
		return EOF;

	if(pFHandle->pBuffer)
		free((void *) pFHandle->pBuffer);

	free((void *) pFHandle);
	free((void *) pAStream);
	return 0;
}

size_t aread(void *buffer, size_t size, size_t count, FILE *stream)
{
	char				*cbuffer = buffer;
	char				*cbCursor = NULL;
	int					iAddSize = 0;
	unsigned int		readsize, 
						iSize = 0;
	APAK_STREAM_TYPE	*pAStream = (APAK_STREAM_TYPE *) stream;
	APAK_FILE_HANDLE	*pFHandle;

	if(!pAStream)
		return 0;

	if(pAStream->pFile)
		return fread(buffer, size, count, pAStream->pFile);

	pFHandle = (APAK_FILE_HANDLE *) pAStream->pAHandle;

	if(!pFHandle)
		return 0;

	iSize = pFHandle->pFileInfo->apuLRealSizeofFile;

	if(pFHandle->apuLfCursor >= iSize)
	{
		pFHandle->bEof = 1;
		return 0;
	}

	cbCursor = &pFHandle->pBuffer[pFHandle->apuLfCursor];

	readsize = size * count;

	if(pFHandle->apuLfCursor + readsize > iSize)
	{
		readsize = iSize - pFHandle->apuLfCursor;
		pFHandle->bEof = 1;
	}

	memcpy(buffer, cbCursor, readsize);
	
	pFHandle->apuLfCursor += readsize;

	return readsize;
}

size_t awrite(const void *buffer, size_t size, size_t count, FILE *stream)
{
	APAK_STREAM_TYPE	*pAStream = (APAK_STREAM_TYPE *) stream;

	if(!pAStream)
		return 0;

	if(pAStream->pFile)
		return fwrite(buffer, size, count, pAStream->pFile);

	return 0;
}

int aeof(FILE *stream )
{
	APAK_STREAM_TYPE	*pAStream = (APAK_STREAM_TYPE *) stream;
	APAK_FILE_HANDLE	*pFHandle;

	if(!pAStream)
		return 0;

	if(pAStream->pFile)
		return feof(pAStream->pFile);

	pFHandle = (APAK_FILE_HANDLE *) pAStream->pAHandle;

	if(!pFHandle)
		return 0;

	return pFHandle->bEof;
}

char *agets(char *string, int n, FILE *stream)
{
	int					iRet = 0, readsize,i;
	unsigned int		iSize;
	char				*cbCursor, cLast = 0, *Tmp;
	APAK_FILE_HANDLE	*pFHandle;
	APAK_STREAM_TYPE	*pAStream = (APAK_STREAM_TYPE *) stream;

	if(!pAStream)
		return 0;

	if(pAStream->pFile)
		return fgets(string, n, pAStream->pFile);

	pFHandle = (APAK_FILE_HANDLE *) pAStream->pAHandle;

	if(!pFHandle)
		return NULL;

	cbCursor = &pFHandle->pBuffer[pFHandle->apuLfCursor];

	iSize = pFHandle->pFileInfo->apuLRealSizeofFile;
	
	if(pFHandle->apuLfCursor >= iSize)
	{
		pFHandle->bEof = 1;
		return 0;
	}

//	memset(string, 0, n);

	n--;

	if(n < 1)
		return 0;

	if(iSize - pFHandle->apuLfCursor > (unsigned)n)
		readsize = n;
	else
		readsize = iSize - pFHandle->apuLfCursor;

	Tmp = string;

	for(i=0;i<readsize;i++)
	{
		if((*cbCursor == 13 && !(*(cbCursor+1)) && pFHandle->bUni) ||
		   (*cbCursor == 13 && !pFHandle->bUni))
		{
			if(pFHandle->bUni)
			{
				cbCursor+=2;
				i+=2;
			}
			else
			{
				cbCursor++;
				i++;
			}
		}

		if(*cbCursor == 10)
			cLast = 1;

		*string = *cbCursor;

		cbCursor++;
		string++;

		if(cLast)
			break;
	}

	if(pFHandle->bUni)
	{
		*string = 0;
		string++;
	}

	*string = '\0';

	if(pFHandle->bUni)
		pFHandle->apuLfCursor += i + 2;
	else
		pFHandle->apuLfCursor += i + 1;

/*	if(pFHandle->apuLfCursor >= iSize)
		pFHandle->bEof = 1;*/

	return Tmp;
}

int aputs(const char *string, FILE *stream)
{
	APAK_STREAM_TYPE	*pAStream = (APAK_STREAM_TYPE *) stream;

	if(!pAStream)
		return 0;

	if(pAStream->pFile)
		return fputs(string, pAStream->pFile);

	return EOF;
}

long atell(FILE *stream )
{
	APAK_STREAM_TYPE	*pAStream = (APAK_STREAM_TYPE *) stream;
	APAK_FILE_HANDLE    *pFHandle;

	if(!pAStream)
		return 0;

	if(pAStream->pFile)
		return ftell(pAStream->pFile);

	pFHandle = (APAK_FILE_HANDLE *) pAStream->pAHandle;

	if(!pFHandle)
		return 0;

	return pFHandle->apuLfCursor;
}

int aseek(FILE *stream, long offset, int origin)
{
	APAK_STREAM_TYPE	*pAStream = (APAK_STREAM_TYPE *) stream;
	APAK_FILE_HANDLE    *pFHandle;

	if(!pAStream)
		return 0;

	if(pAStream->pFile)
		return fseek(pAStream->pFile, offset, origin);

	pFHandle = (APAK_FILE_HANDLE *) pAStream->pAHandle;

	if(!pFHandle)
		return 1;

	switch(origin)
	{
	case SEEK_CUR:
			pFHandle->apuLfCursor = pFHandle->apuLfCursor + offset;
		break;
	case SEEK_END:
			pFHandle->apuLfCursor = pFHandle->pFileInfo->apuLRealSizeofFile + offset;
		break;
	case SEEK_SET:
			pFHandle->apuLfCursor = offset;
		break;
	}

	if(pFHandle->apuLfCursor < 0)
		pFHandle->apuLfCursor = 0;

	if(pFHandle->apuLfCursor > pFHandle->pFileInfo->apuLRealSizeofFile)
		pFHandle->apuLfCursor = pFHandle->pFileInfo->apuLRealSizeofFile;

	pFHandle->bEof = 0;

	return 0;
}

int aunicode(FILE *stream)
{
	APAK_STREAM_TYPE	*pAStream = (APAK_STREAM_TYPE *) stream;
	APAK_FILE_HANDLE	*pFHandle;

	if(!pAStream)
		return 0;

	if(pAStream->pFile)
		return 0;

	pFHandle = (APAK_FILE_HANDLE *) pAStream->pAHandle;

	if(!pFHandle)
		return 0;

	if(APAKMAKEWORD(pFHandle->pBuffer[0], pFHandle->pBuffer[1]) == 0xFEFF)
	{
		pFHandle->bUni = 1;
		return 1;
	}

	return 0;
}
