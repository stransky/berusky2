#ifndef _GAMEINIT_
#define _GAMEINIT_

#include <stdio.h>
#include "Apak.h"

//void gi_Init_Sound_Engine(HWND hWnd, AUDIO_DATA *p_ad);
void gi_Release_Sound_Engine(void);
int gi_Get_Max_Resolution_Bpp(int xPels, int yPels);

//int gi_Change_Grafic_Mode(int xPels, int yPels, int Bits, int freq, DWORD dwflags);
//int gi_EnumDisplaySettings(DEVMODE *pdevmode);
int gi_Open_Archive(char *cFile, APAK_HANDLE ** pAHandle, char *cAppName,
  char *cKeyName);
int gi_Get_Max_Freq(int xPels, int yPels, int bpp);
void gi_Set_Win_Version(void);

#endif
