#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "Apak.h"
#include "compat_mini.h"

#define APAK_VERSION_HI		1
#define APAK_VERSION_LOW	1

void apakCheck(void)
{
  assert(sizeof(APAKFILEHEADER) == 428);
  assert(sizeof(APAKFILE_DISK) == 284);
  assert(sizeof(APAKDIRECTORY_DISK) == 272);
  assert(sizeof(APAKNODE_DISK) == 280);
  assert(sizeof(APAKFILENODE) == 8);
}

void apakfile_from_disk(APAKFILE_DISK *src, APAKFILE *desc)
{
  memset(desc,0,sizeof(APAKFILE));
  memcpy(desc->cName, src->cName, sizeof(desc->cName));
  desc->uattFileAttribute = src->uattFileAttribute;
  desc->apuLFileLocation = src->apuLFileLocation;
  desc->apuLSizeofFile = src->apuLSizeofFile;
  desc->apuLRealSizeofFile = src->apuLRealSizeofFile;
  desc->bNotCompressed = src->bNotCompressed;
}

void apakdirectory_from_disk(APAKDIRECTORY_DISK *src, APAKDIRECTORY *desc)
{
  memset(desc,0,sizeof(APAKDIRECTORY));
  memcpy(desc->cName, src->cName, sizeof(desc->cName));
  desc->uattDirAttribute = src->uattDirAttribute;
} 

void apaknode_from_disk(APAKNODE_DISK *src, APAKNODE *desc)
{
  memset(desc,0,sizeof(APAKNODE));
  memcpy(desc->cWDir, src->cWDir, sizeof(desc->cWDir));
  desc->apuLSizeofFile = src->apuLSizeofFile;
  desc->apuLSizeofDirectory = src->apuLSizeofDirectory;
}

void apakError(APAK_HANDLE * pHandle, char *cError)
{
  pHandle->bError = 1;
  strcpy(pHandle->cError, cError);
  //fprintf(stderr,"apakError: %s\n", cError);  
}

int apakReadError(int iError, APAK_HANDLE * pHandle)
{
  if (!iError) {
    apakError(pHandle, "Read error");
    return 1;
  }

  if (iError == -1) {
    apakError(pHandle, "Unknown Error");
    return 1;
  }

  return 0;
}

void apakGetVersion(int *iLow, int *iHi)
{
  *iLow = APAK_VERSION_LOW;
  *iHi = APAK_VERSION_HI;
}

void apakCreateNode(APAK_HANDLE * pHandle, char bFirst, APAKNODE * pPrevNode,
  APAKNODE ** pCpyNode, char bConnect, char *cDir, char bSetcwDir)
{
  char buffer[256];
  int iRet, iLastFile, iLastDir, i;
  APAKFILENODE FileNode;
  APAKNODE *pNode;

  if (!bFirst) {
    iRet = fread(&FileNode, sizeof(APAKFILENODE), 1, pHandle->pFILE);
    if (apakReadError(iRet, pHandle))
      return;

    if (FileNode.iNextItem != APAK_NODE) {
      apakError(pHandle, "AFAT corrupted");
      return;
    }
  }

  pNode = (APAKNODE *) malloc(sizeof(APAKNODE));

  if (!pNode) {
    apakError(pHandle, "Unable allocate memory for node");
    return;
  }

  if (pCpyNode)
    *pCpyNode = pNode;

  APAKNODE_DISK tmp;
  iRet = fread(&tmp, sizeof(APAKNODE_DISK), 1, pHandle->pFILE);
  apaknode_from_disk(&tmp, pNode);

  if (apakReadError(iRet, pHandle))
    return;

  if (bFirst) {
    pHandle->pRootNode = pNode;
    pHandle->pActualNode = pNode;
  }

  if (bConnect) {
    pPrevNode->pNextNode = pNode;

    if (bSetcwDir && !strcmp(pNode->cWDir, "{APPDIR}"))
      strcpy(pNode->cWDir, cDir);
  }
  else if (!bFirst) {
    strcpy(buffer, pNode->cWDir);
    strcpy(pNode->cWDir, cDir);
    strcat(pNode->cWDir, DIR_SLASH_STRING);
    strcat(pNode->cWDir, buffer);
  }

  pNode->apakDirectory = NULL;
  pNode->apakFile = NULL;
  pNode->pPreviousNode = pPrevNode;
  pNode->pNextNode = NULL;

  iLastFile = 0;
  iLastDir = 0;

  if (pNode->apuLSizeofFile) {
    pNode->apakFile =
      (APAKFILE *) malloc(pNode->apuLSizeofFile * sizeof(APAKFILE));

    if (!pNode->apakFile) {
      apakError(pHandle, "Unable allocate memory for file table");
      return;
    }
  }

  for (i = 0; (unsigned int) i < pNode->apuLSizeofFile; i++)
    ZeroMemory(&pNode->apakFile[i], sizeof(APAKFILE));

  if (pNode->apuLSizeofDirectory) {
    pNode->apakDirectory =
      (APAKDIRECTORY *) malloc(pNode->apuLSizeofDirectory *
      sizeof(APAKDIRECTORY));

    if (!pNode->apakDirectory) {
      apakError(pHandle, "Unable allocate memory for directory table");
      return;
    }
  }

  for (i = 0; (unsigned int) i < pNode->apuLSizeofDirectory; i++)
    ZeroMemory(&pNode->apakDirectory[i], sizeof(APAKDIRECTORY));

  for (i = 0; (unsigned int) i < pNode->apuLSizeofFile; i++) {
    iRet = fread(&FileNode, sizeof(APAKFILENODE), 1, pHandle->pFILE);

    if (apakReadError(iRet, pHandle))
      return;

    if (FileNode.iNextItem != APAK_FILE) {
      apakError(pHandle, "AFAT corrupted");
      return;
    }
    
    APAKFILE_DISK tmp;
    iRet = fread(&tmp, sizeof(APAKFILE_DISK), 1, pHandle->pFILE);
    apakfile_from_disk(&tmp, &pNode->apakFile[iLastFile]);

    if (apakReadError(iRet, pHandle))
      return;

    pNode->apakFile[iLastFile].pNext = NULL;

    iLastFile++;
  }

  for (i = 0; (unsigned int) i < pNode->apuLSizeofDirectory; i++) {
    iRet = fread(&FileNode, sizeof(APAKFILENODE), 1, pHandle->pFILE);

    if (apakReadError(iRet, pHandle))
      return;

    if (FileNode.iNextItem != APAK_DIRECTORY) {
      apakError(pHandle, "AFAT corrupted");
      return;
    }

    APAKDIRECTORY_DISK tmp;
    iRet = fread(&tmp, sizeof(APAKDIRECTORY_DISK), 1, pHandle->pFILE);
    apakdirectory_from_disk(&tmp, &pNode->apakDirectory[iLastDir]);

    if (apakReadError(iRet, pHandle))
      return;

    pNode->apakDirectory[iLastDir].papakNode = NULL;

    apakCreateNode(pHandle, 0, pNode,
      (APAKNODE **) & pNode->apakDirectory[iLastDir].papakNode, 0,
      pNode->cWDir, bSetcwDir);

    pNode->apakDirectory[iLastDir].pNext = NULL;

    iLastDir++;
  }

  for (i = 1; (unsigned int) i < pNode->apuLSizeofFile; i++)
    pNode->apakFile[i - 1].pNext = &pNode->apakFile[i];

  for (i = 1; (unsigned int) i < pNode->apuLSizeofDirectory; i++)
    pNode->apakDirectory[i - 1].pNext = &pNode->apakDirectory[i];

  iRet = fread(&FileNode, sizeof(APAKFILENODE), 1, pHandle->pFILE);

  if (apakReadError(iRet, pHandle))
    return;

  if (FileNode.iNextItem == APAK_ALLOCNODE)
    apakCreateNode(pHandle, 0, pNode, NULL, 1, cDir, bSetcwDir);
}

int apakLoadFAT(APAK_HANDLE * pHandle, char *cDir)
{
  int iRet;
  APAKFILENODE FileNode;

  FileNode.iNextItem = -1;

  iRet = fread(&FileNode, sizeof(APAKFILENODE), 1, pHandle->pFILE);

  if (apakReadError(iRet, pHandle))
    return 0;

  if (FileNode.iNextItem == APAK_ENDOFFAT)
    return 1;
  else if (FileNode.iNextItem == APAK_NODE)
    if (cDir)
      if (cDir[1] == ':')
        apakCreateNode(pHandle, 1, NULL, NULL, 0, &cDir[3], 1);
      else
        apakCreateNode(pHandle, 1, NULL, NULL, 0, cDir, 1);
    else
      apakCreateNode(pHandle, 1, NULL, NULL, 0, NULL, 0);
  else
    return 0;

  return 1;
}

APAK_HANDLE *apakopen(char *cArchive, char *cDir, int *pError)
{
  int iRet;
  APAK_HANDLE *pHandle;
  FILE *pFile;

  apakCheck();

  pFile = fopen(cArchive, "rb");

  if (!pFile) {
    *pError = APAK_FILE_NOT_FOUND;
    return NULL;
  }

  pHandle = (APAK_HANDLE *) malloc(sizeof(APAK_HANDLE));

  if (!pHandle) {
    *pError = APAK_OUT_OF_MEMORY;
    return NULL;
  }

  ZeroMemory(pHandle, sizeof(APAK_HANDLE));

  pHandle->pFILE = pFile;

  iRet = fread(&pHandle->FileHeader, sizeof(APAKFILEHEADER), 1, pFile);

  if (apakReadError(iRet, pHandle)) {
    fclose(pHandle->pFILE);
    free((void *) pHandle);
    *pError = APAK_UNABLE_TO_READ;
    return NULL;
  }

  if (pHandle->FileHeader.apBFileHiVersion != APAK_VERSION_HI ||
    pHandle->FileHeader.apBFileLoVersion != APAK_VERSION_LOW) {
    fclose(pHandle->pFILE);
    free((void *) pHandle);
    *pError = APAK_VERSION_MISMATCH;
    return NULL;
  }

  if (!apakLoadFAT(pHandle, apak_dir_correction(cDir)))
    apakError(pHandle, "AFAT corrupted");

  return pHandle;
}

void apakReleaseNode(APAKNODE * pNode)
{
  int i;

  if (!pNode)
    return;

  if (pNode->apakFile)
    free((void *) pNode->apakFile);


  if (pNode->apuLSizeofDirectory) {
    for (i = 0; (unsigned) i < pNode->apuLSizeofDirectory; i++)
      apakReleaseNode(pNode->apakDirectory[i].papakNode);

    free((void *) pNode->apakDirectory);
  }


  if (pNode->pNextNode)
    apakReleaseNode(pNode->pNextNode);

  free((void *) pNode);
}

void apakclose(APAK_HANDLE * pHandle)
{
  if (!pHandle)
    return;

  apakReleaseNode(pHandle->pRootNode);

  fclose(pHandle->pFILE);
  free((void *) pHandle);
}

char * apak_dir_correction(char *dir)
{
  static char tmp[MAX_PATH] = "c:";
  if(dir || (tolower(dir[0]) == 'c' && dir[1] == ':'))
    return(dir);
  
  assert(strlen(dir) < 254);
  strcat(tmp+2, dir);
  return(tmp);
}

void apak_dir_correction(char *dir, char *out)
{
  if(dir && tolower(dir[0]) != 'c' && dir[1] != ':') {
    out[0] = 'c';
    out[1] = ':';
    strcpy(out+2, dir);
  }
}
