#include <windows.h>
#include <stdio.h>
#include "..\komat\3d_all.h"
#include "..\Komat\berusky3d_kofola_interface.h"
#include "..\Komat\Berusky_universal.h"
#include "ArmAccess.h"

int MyTouch(char *cFile)
{
	FILE *file=fopen(cFile,"r");

	if(file)
		fclose(file);

	return (int)file;
}

void GetB2Versnions(char *pInstalled)
{
	int i;
	char cwd[MAX_PATH];
	char dir[MAX_PATH];
	char cFile[256];

	for(i=0;i<11;i++)
		pInstalled[i] = '0';

	pInstalled[12] = '\0';
	
	getcwd(cwd, MAX_PATH);
	GetPrivateProfileString("game","bitmap_dir","c:\\",dir,MAX_PATH,ini_file);
	chdir(dir);

	for(i=0;i<11;i++)
	{
		sprintf(cFile,"scene%d.pak",i);
		if(MyTouch(cFile))
			pInstalled[i]='1';
	}

	chdir(cwd);
}

void AddOne(char *pInstalled1, char *pInstalled2)
{
	int i;

	for(i=0;i<(signed)strlen(pInstalled1);i++)
		if(pInstalled1[i] == '1' && pInstalled2[i] == '0')
		{
			pInstalled2[i]='1';
			break;
		}
}

int Check(char *pInstalled1, char *pInstalled2)
{
	int i;

	for(i=0;i<(signed)strlen(pInstalled1);i++)
		if(pInstalled1[i] == '1' && pInstalled2[i] == '0')
			return 0;

	return 1;
}

int Expire(HWND hWnd)
{
	int i;
	int iEnterKey = 0;
	int iRet;
	char cInstalled1[12];
	char cInstalled2[12];
	ShowEnterKeyDialogFn ShowEnterKeyDialog;
	SetUserStringFn SetUserString;
	GetUserStringFn GetUserString;
	ExpireCurrentKeyFn ExpireCurrentKey;
	DWORD dError;
	HINSTANCE libInst=LoadLibrary("ArmAccess.DLL");
	char cText[256];

	if(!libInst) exit(0); // Couldn't load library
	
	ExpireCurrentKey=(ExpireCurrentKeyFn) GetProcAddress(libInst, "ExpireCurrentKey");
	if(ExpireCurrentKey==0) exit(0); // Couldn't find function

	ShowEnterKeyDialog=(ShowEnterKeyDialogFn) GetProcAddress(libInst, "ShowEnterKeyDialog");
	if(ShowEnterKeyDialog==0) exit(0); // Couldn't find function

	SetUserString=(SetUserStringFn) GetProcAddress(libInst, "SetUserString");
	if(SetUserString==0) exit(0); // Couldn't find function

	GetUserString=(GetUserStringFn) GetProcAddress(libInst, "GetUserString");
	if(GetUserString==0) exit(0); // Couldn't find function

	memset(cText, 0, 256);
    if (GetEnvironmentVariable("FIRSTRUN", cText, 255))
	{
		//MessageBox(hWnd, "FIRSTRUN","Info",MB_OK);
		
		memset(cText, 0, 256);
		for(i=0;i<2;i++)
			cText[i] = '1';

		for(i=2;i<11;i++)
			cText[i] = '0';

		SetUserString(0, cText);
	}

	memset(cInstalled1, 0, 12);
	memset(cInstalled2, 0, 12);
	GetB2Versnions(cInstalled1);

	for(i=0;i<2;i++)
		cInstalled2[i] = '1';

	for(i=2;i<11;i++)
		cInstalled2[i] = '0';
	
	GetUserString(0, cInstalled2, 256);

	sprintf(cText,"[%s]x[%s]", cInstalled1, cInstalled2);
	kprintf(1,"Instalacni maska je %s",cText);
	//MessageBox(hWnd, cText,"cInstalled", MB_OK);

	if(!Check(cInstalled1,cInstalled2))
	{
		iEnterKey=1;
		//MessageBox(hWnd, "Detekovan Patch","Info",MB_OK);
		kprintf(1,"Detekovan Patch");
		ExpireCurrentKey();
	}

	memset(cText, 0, 256);
    if (GetEnvironmentVariable("EXPIRED", cText, 255))
		iEnterKey=1;

	if(iEnterKey)
	{
		ShowWindow(hWnd, SW_MINIMIZE);
		iRet = ShowEnterKeyDialog(hWnd);

		dError = GetLastError();
		if(dError)
			exit(0);
	}

	memset(cText, 0, 256);
    if (!GetEnvironmentVariable("EXPIRED", cText, 255))
	{
		//MessageBox(hWnd, "Klic neni vyexpirovanej", "Info", MB_OK);
		kprintf(1,"Klic NENI vyexpirovanej");

		AddOne(cInstalled1, cInstalled2);
		SetUserString(0, cInstalled2);
    }
	else
	{
		//MessageBox(hWnd, "Expired", "Error!", MB_OK|MB_ICONERROR);
		kprintf(1,"Klic JE vyexpirovanej");
		exit(0);
	}

	if(iEnterKey)
		ShowWindow(hWnd, SW_RESTORE);

	FreeLibrary(libInst);

	return 1;
}

int DefCert(HWND hWnd)
{
	HINSTANCE libInst=LoadLibrary("ArmAccess.DLL");
	char cText[256];

	if(!libInst) exit(0); // Couldn't load library
	
	memset(cText, 0, 256);
    if (GetEnvironmentVariable("ALTUSERNAME", cText, 255))
	{
		if(!strcmp(cText, "DEFAULT"))
		{
			FreeLibrary(libInst);
			return 1;
		}
		else
		{
			FreeLibrary(libInst);
			return 0;
		}
	}
	else
	{
		FreeLibrary(libInst);
		return 0;
	}

	return 0;
}

