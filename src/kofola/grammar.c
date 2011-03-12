#include "..\\komat\\mss_on.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "..\komat\3d_all.h"
#include "..\Komat\berusky3d_kofola_interface.h"
#include "grammar.h"
#include "menu_script.h"

extern APAK_HANDLE		*pDataArchive;

int StrtoType(char *pStr)
{
	if(!strcmp(pStr,"iV"))
		return 0;

	if(!strcmp(pStr,"fV"))
		return 1;

	if(!strcmp(pStr,"sV"))
		return 2;

	if(!strcmp(pStr,"uV"))
		return 3;

	return -1;
}

void gr_Add_Mask(char *pMask, GRAMMAR *pGr)
{
	char Expression[64];
	int cursor = 0, lm = pGr->LastMask;

	if(!strlen(pMask))
		return;

	cursor = Find_Next_Expresion(pMask, cursor, pGr->Mask[lm].cCommand);
	cursor = Find_Next_Expresion(pMask, cursor, Expression);

	pGr->Mask[lm].iCommand = atoi(Expression);
	pGr->Mask[lm].LastParam = 0;
	while(cursor != -1)
	{
		cursor = Find_Next_Expresion(pMask, cursor, Expression);

		if(cursor != -1)
		{
			pGr->Mask[lm].Parametr[pGr->Mask[lm].LastParam].Type = StrtoType(Expression);
			pGr->Mask[lm].LastParam++;
		}
	}

	pGr->LastMask++;
}

char gr_Load_Grammar(char *pFile, GRAMMAR *pGr)
{
	FILE	*file;
	char	text[256];
	
	GetPrivateProfileString("game","data_dir","c:\\",text,256,ini_file);
	chdir(text);
	
	file = aopen(pDataArchive, pFile, "r");

	if(!file)
		return 0;

	pGr->LastMask = 0;

	while(!aeof(file))
	{
		agets(text,256,file);
		gr_Add_Mask(text, pGr);
		strcpy(text,"");
	}

	aclose(file);
	return 1;
}