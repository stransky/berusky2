#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "Apak.h"

extern void apakError(APAK_HANDLE *pHandle, char *cError);
extern int  apakReadError(int iError, APAK_HANDLE	*pHandle);

void apakchdir(APAK_HANDLE *pHandle, char *cDir, APAKNODE **pNode)
{
	int i;

	if(cDir[1] == ':')
	{
		*pNode = pHandle->pRootNode->pNextNode;
		return;
	}
	
	if(!(*pNode))
		return;

	if(!((*pNode)->apuLSizeofDirectory))
		if(strcmp("..", cDir))
		{
			if(!(strstr((*pNode)->cWDir, cDir)))
			{
				*pNode = NULL;
				return;
			}
			else
				return; 
		}

	for(i=0;(unsigned)i<(*pNode)->apuLSizeofDirectory;i++)
		if(!strcmp((*pNode)->apakDirectory[i].cName, cDir))
		{
			*pNode = (*pNode)->apakDirectory[i].papakNode;
			return;
		}

	if((*pNode)->pPreviousNode)
		if(!strcmp((*pNode)->pPreviousNode->cWDir, "ROOT:"))
		{
			if(strstr((*pNode)->cWDir, cDir))
				return;
		}

	if((*pNode)->pPreviousNode)
		if(!strcmp("..", cDir))
		{
			if(strcmp((*pNode)->pPreviousNode->cWDir, "ROOT:"))
			{
				(*pNode) = (*pNode)->pPreviousNode;
				return;
			}
			else
				return;
		}

	*pNode = NULL;
	return;
}

int apakParsechdir(unsigned int iIndex, char *cDir, char *cExpression)
{
	int i;

	if( iIndex >= strlen(cDir))
		return -1;

	for(i=iIndex;(unsigned)i<strlen(cDir);i++)
		if(cDir[i] == '\\' || cDir[i] == '/')
			break;

	strncpy(cExpression, &cDir[iIndex], i-iIndex);
	cExpression[i-iIndex] = '\0';

	i++;

	return i;
}

int achdir(APAK_HANDLE *pHandle, const char *dirname)
{
	char cExpression[256];
	int i = 0;
	APAKNODE *pNode;

	if(!pHandle)
		return chdir((dirname));

	pNode = pHandle->pActualNode;

	while(i != -1)
	{
		i = apakParsechdir(i, (char *)dirname, cExpression);
		if(i == -1)
			break;

		apakchdir(pHandle, cExpression, &pNode);

		if(!pNode)
		{
			apakError(pHandle, "Specified path could not be found");
			return -1;
		}
	}

	pHandle->pActualNode = pNode;

	return 0;
}

char *agetcwd(APAK_HANDLE *pHandle, char *buffer, int maxlen )
{
	if(!pHandle)
		getcwd(buffer, maxlen);

	if(maxlen > 256)
		maxlen = 256;

	if(!pHandle->pActualNode)
		return NULL;

	strncpy(buffer, pHandle->pActualNode->cWDir, maxlen);

	return buffer;
}
