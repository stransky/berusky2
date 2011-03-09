#ifndef __APAK_TYPES___
#define __APAK_TYPES___

#include <stdio.h>
#include <io.h>
#include "zlib.h"

typedef unsigned char	apBite;
typedef unsigned int	apuInt;
typedef unsigned long	apuLong;

typedef struct _APAKFILEHEADER
{
	char		cFileID[4];
	apBite		apBFileLoVersion;
	apBite		apBFileHiVersion;
	apuInt		apuISizeofFAT;
	apuInt		apuICountofFiles;
	apuInt		apuICountofDirectiories;
	apuInt		apuICountofNodes;
	apuLong		apuLSizeofPAK;
	int			iReserved[100];
} APAKFILEHEADER;

typedef struct _APAKFILE
{
	char				cName[256];
	time_t				ttCreateTime;
	apuInt				uattFileAttribute;
	apuLong				apuLFileLocation;
	apuLong				apuLSizeofFile;
	apuLong				apuLRealSizeofFile;
	char				bNotCompressed;
	struct _APAKFILE	*pNext;
} APAKFILE;

typedef struct _APAKDIRECTORY
{
	char					cName[256];
	time_t					ttCreateTime;
	apuInt					uattDirAttribute;
	void					*papakNode;
	struct _APAKDIRECTORY	*pNext;
} APAKDIRECTORY;

typedef struct _APAKNODE
{
	char				cWDir[256];
	apuLong				apuLSizeofFile;
	APAKFILE			*apakFile;
	apuLong				apuLSizeofDirectory;
	APAKDIRECTORY		*apakDirectory;
	struct _APAKNODE	*pPreviousNode;
	struct _APAKNODE	*pNextNode;
} APAKNODE;

typedef struct _APAKFILENODE
{
	int		iNextItem;
	apuLong	apuLfSeek;
} APAKFILENODE;

typedef struct _APAK_HANDLE
{
	FILE				*pFILE;
	APAKNODE			*pActualNode;
	APAKFILEHEADER		FileHeader;
	char				bError;
	char				cError[256];
	APAKNODE			*pRootNode;
} APAK_HANDLE;

typedef struct _APAK_FILE_HANDLE
{
	apuLong		apuLfStartofFile;
	apuLong		apuLfEndofFile;
	apuLong		apuLfCursor;
	char		bUni;
	char		bEof;
	char		*pBuffer;
	APAKFILE	*pFileInfo;
	APAK_HANDLE	*pArchive;
} APAK_FILE_HANDLE;

typedef struct _APAK_STREAM_TYPE
{
	APAK_FILE_HANDLE	*pAHandle;
	FILE				*pFile;
} APAK_STREAM_TYPE;

typedef struct _APAK_FIND
{
	APAK_HANDLE	*pArchive;
	APAKNODE	*pSearchNode;
	int			iLastFile;
	int			iLastDir;
	char		cFileSpec[256];
} APAK_FIND;

typedef struct _APAK_FIND_SWITCH
{
	APAK_FIND	*pAFind;
	long		find;
} APAK_FIND_SWITCH;

#define APAK_ENDOFFAT	0
#define APAK_ENDOFNODE	1
#define APAK_NODE		2
#define APAK_DIRECTORY	3
#define APAK_FILE		4
#define APAK_ALLOCNODE	5

#define APAK_FILE_NOT_FOUND		-1
#define APAK_UNABLE_TO_READ		-2
#define APAK_VERSION_MISMATCH	-3
#define APAK_OUT_OF_MEMORY		-4

#endif