#include <stdio.h>
#include <string.h>
#include "Apak.h"
#include "3d_all.h"

extern void apakError(APAK_HANDLE *pHandle, char *cError);
extern int  apakReadError(int iError, APAK_HANDLE	*pHandle);
extern int  apakDir(APAK_HANDLE *pHandle, char *cFileName);

int apakFindNextPoint(char *cFile, int i)
{
	int j;

	for(j = i; (unsigned)j < strlen(cFile); j++)
		if(cFile[j] == '.')
			return j-1;

	return -2;
}

int apakCheckFile(char *cFile, char *cMask)
{
	int	i, j = 0;

	if(!cFile || !cMask || !strlen(cMask))
		return 0;

	strlwr(cFile);
	strlwr(cMask);

	for(i=0;(unsigned)i<strlen(cFile);i++)
	{
		if(cMask[j] == '?')
			continue;
		else
			if(cMask[j] == '*')
			{
				i = apakFindNextPoint(cFile, i);

				if(i == -2)
					return 1;
			}
			else
				if(cFile[i] != cMask[j])
					return 0;
		j++;
	}

	return 1;
}
/*
int apakFindFile(APAK_FIND *pFind, struct _finddata_t *fileinfo)
{
	int iRet = 0;

	if(pFind->iLastFile == -1 && pFind->iLastDir == -1)
		return -1;

	if(pFind->iLastFile != -1)
	{
		if(apakCheckFile(pFind->pSearchNode->apakFile[pFind->iLastFile].cName, pFind->cFileSpec))
		{
			ZeroMemory(fileinfo, sizeof(struct _finddata_t));

			fileinfo->attrib = pFind->pSearchNode->apakFile[pFind->iLastFile].uattFileAttribute;
			strcpy(fileinfo->name, pFind->pSearchNode->apakFile[pFind->iLastFile].cName);
			fileinfo->size = pFind->pSearchNode->apakFile[pFind->iLastFile].apuLSizeofFile;
			fileinfo->time_create = pFind->pSearchNode->apakFile[pFind->iLastFile].ttCreateTime;

			iRet = 1;
		}

		pFind->iLastFile++;
		if((unsigned)pFind->iLastFile >= pFind->pSearchNode->apuLSizeofFile)
			pFind->iLastFile = -1;

		return iRet;
	}

	if(pFind->iLastDir != -1)
	{
		if(apakCheckFile(pFind->pSearchNode->apakDirectory[pFind->iLastDir].cName, pFind->cFileSpec))
		{
			ZeroMemory(fileinfo, sizeof(struct _finddata_t));

			fileinfo->attrib = pFind->pSearchNode->apakDirectory[pFind->iLastDir].uattDirAttribute;
			strcpy(fileinfo->name, pFind->pSearchNode->apakDirectory[pFind->iLastDir].cName);
			fileinfo->time_create = pFind->pSearchNode->apakDirectory[pFind->iLastDir].ttCreateTime;

			iRet = 1;
		}

		pFind->iLastDir++;
		if((unsigned)pFind->iLastDir >= pFind->pSearchNode->apuLSizeofDirectory)
			pFind->iLastDir = -1;

		return iRet;
	}

	return 0;
}
*/
/*
long afindfirst(APAK_HANDLE *pHandle, char *filespec, struct _finddata_t *fileinfo)
{
	int				 iRet;
	APAKNODE		 *pNode;
	APAK_FILE_HANDLE *pFHandle = NULL;
	APAK_FIND		 *pFind;
	APAK_FIND_SWITCH *pFSwitch;

	pFSwitch = (APAK_FIND_SWITCH *) malloc(sizeof(APAK_FIND_SWITCH));

	if(!pFSwitch)
		return -1;

	memset(pFSwitch, 0, sizeof(APAK_FIND_SWITCH));

	if(!pHandle)
	{
		 pFSwitch->find = _findfirst(filespec, fileinfo);

		 if(pFSwitch->find == -1)
		 {
			 free((void *) pFSwitch);
			 return -1;
		 }

		 return (long) pFSwitch;
	}

	if(!filespec)
	{
		apakError(pHandle, "filespec = NULL");
		free((void *) pFSwitch);
		return -1;
	}

	if(!strlen(filespec))
	{
		apakError(pHandle, "filespec length = 0");
		free((void *) pFSwitch);
		return -1;
	}
		
	if(!fileinfo)
	{
		apakError(pHandle, "fileinfo = NULL");
		free((void *) pFSwitch);
		return -1;
	}
	
	pNode = pHandle->pActualNode;

	if(!apakDir(pHandle, (char *)filespec))
	{
		apakError(pHandle, "Directory not found");
		free((void *) pFSwitch);
		return -1;
	}

	pFind = (APAK_FIND *) malloc(sizeof(APAK_FIND));

	if(!pFind)
	{
		apakError(pHandle, "Unable allocate memory for find structure");
		free((void *) pFSwitch);
		return -1;
	}

	pFSwitch->pAFind = pFind;

	pFind->iLastDir = 0;
	pFind->iLastFile = 0;
	pFind->pArchive = pHandle;
	pFind->pSearchNode = pHandle->pActualNode;
	strcpy(pFind->cFileSpec, filespec);
	pHandle->pActualNode = pNode;

	do
	{
		iRet = apakFindFile(pFind, fileinfo);
	}
	while(!iRet);

	if(iRet == -1)
	{
		free((void *) pFind);
		free((void *) pFSwitch);
		return -1;
	}

	return (long) pFSwitch;
}

int afindnext(long handle, struct _finddata_t *fileinfo)
{
	APAK_FIND		 *pFind;
	APAK_FIND_SWITCH *pFSwitch = (APAK_FIND_SWITCH *)handle;
	int iRet;

	if(!pFSwitch)
		return -1;

	if(pFSwitch->find)
		return _findnext(pFSwitch->find, fileinfo);

	pFind = pFSwitch->pAFind;

	if(!pFind)
		return -1;

	do
	{
		iRet = apakFindFile(pFind, fileinfo);
	}
	while(!iRet);

	return iRet;
}
*/

int afindclose(long handle)
{
/*
	APAK_FIND		 *pFind;
	APAK_FIND_SWITCH *pFSwitch = (APAK_FIND_SWITCH *)handle;

	if(!pFSwitch)
		return -1;

	if(pFSwitch->find < 0)
		return 0;

	if(pFSwitch->find)
		return _findclose(pFSwitch->find);

	pFind = pFSwitch->pAFind;

	if(!pFind)
		return -1;

	free((void *) pFind);
	free((void *) pFSwitch);
*/
	return 0;
}
