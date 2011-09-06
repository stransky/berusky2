#ifndef __APAK_TYPES___
#define __APAK_TYPES___

#include <time.h>
#include <stdio.h>
#include "zlib.h"

typedef unsigned char apBite;
typedef unsigned int apuInt;

typedef struct _APAKFILEHEADER
{
  char cFileID[4];
  apBite apBFileLoVersion;
  apBite apBFileHiVersion;
  apuInt apuISizeofFAT;
  apuInt apuICountofFiles;
  apuInt apuICountofDirectiories;
  apuInt apuICountofNodes;
  apuInt apuLSizeofPAK;
  int iReserved[100];
} APAKFILEHEADER;

typedef struct _APAKFILE
{
  char cName[256];
  time_t ttCreateTime;
  apuInt uattFileAttribute;
  apuInt apuLFileLocation;
  apuInt apuLSizeofFile;
  apuInt apuLRealSizeofFile;
  char bNotCompressed;
  struct _APAKFILE *pNext;
} APAKFILE;

typedef struct _APAKFILE_DISK
{
  char cName[256];
  apuInt tmp;
  apuInt uattFileAttribute;
  apuInt apuLFileLocation;
  apuInt apuLSizeofFile;
  apuInt apuLRealSizeofFile;
  char bNotCompressed;
  apuInt tmp1;
} APAKFILE_DISK;

struct apaknode;
typedef struct apaknode APAKNODE;

typedef struct _APAKDIRECTORY
{
  char cName[256];
  time_t ttCreateTime;
  apuInt uattDirAttribute;
  APAKNODE *papakNode;
  struct _APAKDIRECTORY *pNext;
} APAKDIRECTORY;

typedef struct _APAKDIRECTORY_DISK
{
  char cName[256];
  apuInt tmp;
  apuInt uattDirAttribute;
  apuInt tmp1;
  apuInt tmp2;
} APAKDIRECTORY_DISK;

typedef struct apaknode
{
  char cWDir[256];
  apuInt apuLSizeofFile;
  APAKFILE *apakFile;
  apuInt apuLSizeofDirectory;
  APAKDIRECTORY *apakDirectory;
  APAKNODE *pPreviousNode;
  APAKNODE *pNextNode;
} APAKNODE;

typedef struct apaknode_disk
{
  char cWDir[256];
  apuInt apuLSizeofFile;
  apuInt tmp;
  apuInt apuLSizeofDirectory;
  apuInt tmp1;
  apuInt tmp2;
  apuInt tmp3;
} APAKNODE_DISK;

typedef struct _APAKFILENODE
{
  int iNextItem;
  apuInt apuLfSeek;
} APAKFILENODE;

typedef struct _APAK_HANDLE
{
  FILE *pFILE;
  APAKNODE *pActualNode;
  APAKFILEHEADER FileHeader;
  char bError;
  char cError[256];
  APAKNODE *pRootNode;
} APAK_HANDLE;

typedef struct _APAK_FILE_HANDLE
{
  apuInt apuLfStartofFile;
  apuInt apuLfEndofFile;
  apuInt apuLfCursor;
  char bUni;
  char bEof;
  char *pBuffer;
  APAKFILE *pFileInfo;
  APAK_HANDLE *pArchive;
} APAK_FILE_HANDLE;

typedef struct _APAK_STREAM_TYPE
{
  APAK_FILE_HANDLE *pAHandle;
  FILE *pFile;
} APAK_STREAM_TYPE;

typedef struct _APAK_FIND
{
  APAK_HANDLE *pArchive;
  APAKNODE *pSearchNode;
  int iLastFile;
  int iLastDir;
  char cFileSpec[256];
} APAK_FIND;

typedef struct _APAK_FIND_SWITCH
{
  APAK_FIND *pAFind;
  long find;
} APAK_FIND_SWITCH;

struct _finddata_t
{
  unsigned attrib;
  time_t time_create;      /*-1forFATfilesystems*/
  time_t time_access;      /*-1forFATfilesystems */
  time_t time_write;
  int size;
  char name[260];
};

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
