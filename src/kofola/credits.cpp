#include <stdio.h>
#include "3d_all.h"
#include "Berusky3d_kofola_interface.h"
#include "menu_script.h"
#include "2D_graphic.h"
//#include "2ddx.h"
#include "font.h"

#define CREDIT_SURFACES 150

typedef struct 
{
	int	iSurface;
	int	x;
	int	y;
} CREDIT_SURFACE;

extern B2_FONT	b2_font;
extern APAK_HANDLE		*pBmpArchive;
extern APAK_HANDLE		*pDataArchive;

extern	   char cBrutalRestart;
extern	   int CompositDC;
extern	   int FontDC;
extern	   int BackDC;

extern	int		iCompositDC,
				iFontDC,
				iBackDC;

extern char cFontFile[5][64];

int LoadClock(int *iClock);
void DrawClock(int *iClock, int i);

int cr_Set_Text_Center(int hdc, char *text, int isection, RECT r)
{
	int xp, yp;
	int tx, ty;
	WCHAR wc[128];
	int h;

	h = ddxCreateSurface(1024, 110, ddxFindFreeSurface());

	MultiByteToWideChar( CP_ACP, 0, text, strlen(text)+1, wc, sizeof(wc)/sizeof(wc[0]) );

	fn_Draw_MessageA(h, 0, 0, &b2_font.gt, &b2_font.ts, wc, isection, &tx, &ty);

	xp = ftoi(((r.right - r.left) - tx) / 2.0f);
	//yp = ftoi(((r.bottom - r.top) - ty) / 2.0f);
	yp = 0;

	ddxTransparentBlt(hdc, r.left + xp, r.top + yp, tx, ty, h, 0, 0, tx, 90, TRANSCOLOR);
	
	ddxReleaseBitmap(h);

	return 1;
}

int cr_Set_Text_CenterW(int hdc, WCHAR *text, int isection, RECT r)
{
	int xp, yp;
	int tx, ty;
	WCHAR wc[128];
	int h;

	h = ddxCreateSurface(1024, 110, ddxFindFreeSurface());

	MultiByteToWideChar( CP_ACP, 0, (char *)text, strlen((char *)text)+1, wc, sizeof(wc)/sizeof(wc[0]) );

	fn_Draw_MessageA(h, 0, 0, &b2_font.gt, &b2_font.ts, text, isection, &tx, &ty);

	xp = ftoi(((r.right - r.left) - tx) / 2.0f);
	yp = 0;

	ddxTransparentBlt(hdc, r.left + xp, r.top + yp, tx, ty, h, 0, 0, tx, 90, TRANSCOLOR);
	
	ddxReleaseBitmap(h);

	return 1;
}

void cr_DrawSurface(int iDest, int iSour, int *y)
{
	int xp = ftoi((1024 - ddxGetWidth(iSour)) / 2.0f);

	ddxBitBlt(iDest, xp, *y, ddxGetWidth(iSour), ddxGetHight(iSour), iSour, 0, 0);

	
	(*y) += ddxGetHight(iSour);
}

void cr_Cleare(int iSurface)
{
	int idx = ddxCreateSurface(1024, 100, ddxFindFreeSurface());
	int i,y=0;

	//ddxCleareSurfaceColor(iSurface, RGB(255, 0, 255));
	if(idx == -1)
	{
		ddxCleareSurfaceColor(iSurface, 0);
		kprintf(1, "cr_Cleare probehl neuspesne!!!");
		return;
	}

	ddxCleareSurfaceColor(idx, 0);
	
	for(i=0;i<ddxGetHight(iSurface);i+=100)
	{
		ddxBitBlt(iSurface, 0, i, 1024, 100, idx, 0, 0);
	}

	ddxBitBlt(iSurface, 0, ddxGetHight(iSurface)-100, 1024, 100, idx, 0, 0);
	ddxReleaseBitmap(idx);
}

void cr_Release_Bitmaps(CREDIT_SURFACE *p_cs, int *iClock)
{
	int i;

	for(i=0;i<CREDIT_SURFACES;i++)
		if(p_cs->iSurface != -1)
			ddxReleaseBitmap(p_cs->iSurface);

	ddxResizeCursorBack(0);
	ddxSetCursorSurface(0);

	for(i=0;i<12;i++)
		if(iClock[i] != -1)
		{
			ddxReleaseBitmap(iClock[i]);
			iClock[i] = -1;
		}

}

void cr_Set_Surface(CREDIT_SURFACE *p_cs, int y)
{
	if(p_cs->iSurface == -1)
		return;
	
	p_cs->y = y;
	p_cs->x = ftoi((1024 - ddxGetWidth(p_cs->iSurface)) / 2.0f);
}

void cr_Draw_Creadits(CREDIT_SURFACE *cs, int y)
{
	int	dy;
	int i;

	ddxCleareSurfaceColorDisplay(0);

	for(i=0;i<CREDIT_SURFACES;i++)
		if(cs[i].iSurface != -1)
			if((cs[i].y + ddxGetHight(cs[i].iSurface)) > y && (cs[i].y < y + 768))
			{
				dy = cs[i].y - y;

				if(dy < 0)	//obrazek vyjizdi nahore ven
					ddxBitBltDisplay(cs[i].x, 0, ddxGetWidth(cs[i].iSurface), ddxGetHight(cs[i].iSurface) + dy,
									 cs[i].iSurface, 0, dy * -1);
				else
					if(cs[i].y + ddxGetHight(cs[i].iSurface) > y + 768)	//obrazek vjizni zespodu
					{
						int dm = (cs[i].y + ddxGetHight(cs[i].iSurface)) - (y + 768);

						ddxBitBltDisplay(cs[i].x, dy, ddxGetWidth(cs[i].iSurface), ddxGetHight(cs[i].iSurface) - dm,
										 cs[i].iSurface, 0, 0);
					}
					else
						ddxBitBltDisplay(cs[i].x, dy, ddxGetWidth(cs[i].iSurface), ddxGetHight(cs[i].iSurface),
										 cs[i].iSurface, 0, 0);

			}

	DisplayFrame();
	spracuj_spravy(0);
}

int cr_Credits(HWND hWnd, AUDIO_DATA *p_ad)
{
/*
	DWORD	dwStart, dwStop, dwEplased = 0;
	int		y = 868;
	int		dy = 868*2 + 2000;
	char	text[256];
	char	cbmp[256];
	FILE	*file;
	int		c = 0;
	int		bmp = 0;
	RECT	r;
	float	f = p_ad->Music_Gain;
	int		i;
	CREDIT_SURFACE	cs[CREDIT_SURFACES];
	int		iActual = 0;
	int		iClock[12];
	APAK_HANDLE	*hArchive = NULL;
	int		error;

	for(i=0;i<12;i++)
		iClock[i] = -1;

	LoadClock(iClock);

	if(iClock[0] != -1)
	{
		ddxResizeCursorBack(iClock[0]);
		DrawClock(iClock, 0);
	}
	
	if(ogg_playing())
	{
		while(f >= 0.05f)
		{
			f -= 0.05f;
			ogg_gain(f);
			Sleep(25);
			DrawClock(iClock, 0);
		}

		ap_Stop_Song(p_ad);
	}

	for(i=0;i<CREDIT_SURFACES;i++)
	{
		cs[i].iSurface = -1;
		cs[i].x = 0;
		cs[i].y = 0;
	}

	GetPrivateProfileString("game","bitmap_dir","c:\\",text,255,ini_file);
  working_file_translate(text,256);
	chdir((text));

  apak_dir_correction(text);
	hArchive = apakopen(cFontFile[2], text, &error);

	if(!hArchive)
		return 0;
	else
		hArchive->pActualNode = hArchive->pRootNode->pNextNode;

	file = aopen(hArchive, "credits.txt", "r");

	if(!file)
		return 0;

	while(!aeof(file))
	{
		agets(text, 256, file);
		c++;
	}

	DrawClock(iClock, 1);
	aseek(file, 0, SEEK_SET);

	dy += c * 75;
	
	if(c > CREDIT_SURFACES-1)
	{
		kprintf(1, "Kredity: radku je vic jak surfacu!");
		apakclose(hArchive);
		return 0;
	}

	ddxSetFlip(0);
	fn_Set_Font(cFontFile[0]);
	DrawClock(iClock, 2);
	fn_Load_Bitmaps();
	DrawClock(iClock, 3);
	
	cs[iActual].iSurface = ddxLoadBitmap("anakreon_small.bmp", pBmpArchive);
	
	if(cs[iActual].iSurface == -1)
	{
		cr_Release_Bitmaps(cs, iClock);
		apakclose(hArchive);
		return 0;
	}
	else
		cr_Set_Surface(&cs[iActual], y);

	DrawClock(iClock, 4);
	y+= 250;
	iActual++;
	
	cs[iActual].iSurface = ddxLoadBitmap("cinemax_small.bmp", pBmpArchive);

	if(cs[iActual].iSurface == -1)
	{
		cr_Release_Bitmaps(cs, iClock);
		apakclose(hArchive);
		return 0;
	}
	else
		cr_Set_Surface(&cs[iActual], y);

	iActual++;
	y+= 768;

	DrawClock(iClock, 5);

	r.left = 0;
	r.right = 1024;
	r.top = 0;
	r.bottom = 90;

	while(!aeof(file))
	{
		ZeroMemory(text, 256);
		agets(text, 256, file);

		if(text[0] == '$')
		{
			if(bmp < 5)
			{
				if(bmp)
				{
					sprintf(cbmp, "brouk%d.bmp", bmp);
					cs[iActual].iSurface = ddxLoadBitmap(cbmp, pBmpArchive);

					if(cs[iActual].iSurface == -1)
					{
						cr_Release_Bitmaps(cs, iClock);
						apakclose(hArchive);
						return 0;
					}
					else
					{
						cr_Set_Surface(&cs[iActual], y);
						y += ddxGetHight(cs[iActual].iSurface);
						iActual++;
					}
				}

				bmp++;
			}

			cs[iActual].iSurface = ddxCreateSurface(1024, 90, ddxFindFreeSurface());

			if(cs[iActual].iSurface == -1)
			{
				cr_Release_Bitmaps(cs, iClock);
				apakclose(hArchive);
				return 0;
			}
			else
			{
				ddxCleareSurfaceColor(cs[iActual].iSurface, 0);
				cr_Set_Text_Center(cs[iActual].iSurface, text, 1, r);
				cs[iActual].y = y;
				iActual++;
			}
		}
		else
			if(strcmp(text,"\n") && strlen(text))
			{
				cs[iActual].iSurface = ddxCreateSurface(1024, 90, ddxFindFreeSurface());

				if(cs[iActual].iSurface == -1)
				{
					cr_Release_Bitmaps(cs, iClock);
					apakclose(hArchive);
					return 0;
				}
				else
				{
					ddxCleareSurfaceColor(cs[iActual].iSurface, 0);
					cr_Set_Text_Center(cs[iActual].iSurface, text, 0, r);
					cs[iActual].y = y;
					iActual++;
				}
			}

		y += 80;

		if(iActual >= CREDIT_SURFACES)
		{
			kprintf(1, "iActual >= CREDIT_SURFACES !!!!!!!!!!!");
			break;
		}

		DrawClock(iClock, 6);
	}

	aclose(file);

	y = 0;

	ap_Play_Song(11,0, p_ad);

	ddxResizeCursorBack(0);
	ddxSetCursorSurface(0);

	for(i=0;i<12;i++)
		if(iClock[i] != -1)
		{
			ddxReleaseBitmap(iClock[i]);
			iClock[i] = -1;
		}

	ddxSetCursor(0);
	dwStart = timeGetTime();

	while(!key[K_ESC])
	{
		y = (int)ftoi((dwEplased * dy) / (float) 195000);
	
		if(y >=  dy)
			key[K_ESC] = 1;

		cr_Draw_Creadits(cs, y);

		dwStop = timeGetTime();
		dwEplased = dwStop - dwStart;

		if(ddxRestore(&CompositDC, &FontDC, &BackDC, &iCompositDC, &iFontDC, &iBackDC, &cBrutalRestart, p_ad))
		{
			ddxSetFlip(1);
			ddxSetCursor(1);
			apakclose(hArchive);
			return 1;
		}
	}

	cr_Release_Bitmaps(cs, iClock);
	fn_Release_Font(1);

	if(ogg_playing())
		ap_Stop_Song(p_ad);

	Sleep(1000);
	ap_Play_Song(0, 0, p_ad);

	ddxSetCursor(1);
	ddxSetFlip(1);
	key[K_ESC] = 0;
	apakclose(hArchive);
*/
	return 0;
}

int cr_CreditsUNI(HWND hWnd, AUDIO_DATA *p_ad)
{
/*
	DWORD	dwStart, dwStop, dwEplased = 0;
	int		y = 868;
	int		dy = 868*2 + 2000;
	char	text[256];
	char	cbmp[256];
	FILE	*file;
	int		c = 0;
	int		bmp = 0;
	RECT	r;
	float	f = p_ad->Music_Gain;
	int		i;
	CREDIT_SURFACE	cs[CREDIT_SURFACES];
	int		iActual = 0;
	int		iClock[12];
	APAK_HANDLE	*hArchive = NULL;
	int		error;

	for(i=0;i<12;i++)
		iClock[i] = -1;

	LoadClock(iClock);

	if(iClock[0] != -1)
	{
		ddxResizeCursorBack(iClock[0]);
		DrawClock(iClock, 0);
	}
	
	if(ogg_playing())
	{
		while(f >= 0.05f)
		{
			f -= 0.05f;
			ogg_gain(f);
			Sleep(25);
			DrawClock(iClock, 0);
		}

		ap_Stop_Song(p_ad);
	}

	for(i=0;i<CREDIT_SURFACES;i++)
	{
		cs[i].iSurface = -1;
		cs[i].x = 0;
		cs[i].y = 0;
	}

	GetPrivateProfileString("game","bitmap_dir","c:\\",text,255,ini_file);
  working_file_translate(text,255);
	chdir((text));

  apak_dir_correction(text);
	hArchive = apakopen(cFontFile[2], text, &error);
	
	if(!hArchive)
		return 0;
	else
		hArchive->pActualNode = hArchive->pRootNode->pNextNode;

	file = aopen(hArchive, "credits.txt", "r");

	if(!file)
		return 0;

	aunicode(file);

	while(!aeof(file))
	{
		agets(text, 256, file);
		c++;
	}

	DrawClock(iClock, 1);
	aseek(file, 2, SEEK_SET);

	dy += c * 75;
	
	if(c > CREDIT_SURFACES-1)
	{
		kprintf(1, "Kredity: radku je vic jak surfacu!");
		apakclose(hArchive);
		return 0;
	}

	ddxSetFlip(0);
	fn_Set_Font(cFontFile[0]);
	DrawClock(iClock, 2);
	fn_Load_Bitmaps();
	DrawClock(iClock, 3);
	
	cs[iActual].iSurface = ddxLoadBitmap("anakreon_small.bmp", pBmpArchive);
	
	if(cs[iActual].iSurface == -1)
	{
		cr_Release_Bitmaps(cs, iClock);
		apakclose(hArchive);
		return 0;
	}
	else
		cr_Set_Surface(&cs[iActual], y);

	DrawClock(iClock, 4);
	y+= 250;
	iActual++;
	
	cs[iActual].iSurface = ddxLoadBitmap("cinemax_small.bmp", pBmpArchive);

	if(cs[iActual].iSurface == -1)
	{
		cr_Release_Bitmaps(cs, iClock);
		apakclose(hArchive);
		return 0;
	}
	else
		cr_Set_Surface(&cs[iActual], y);

	iActual++;
	y+= 768;

	DrawClock(iClock, 5);

	r.left = 0;
	r.right = 1024;
	r.top = 0;
	r.bottom = 90;

	while(!aeof(file))
	{
		ZeroMemory(text, 256);
		agets(text, 256, file);

		if(text[0] == '$')
		{
			if(bmp < 5)
			{
				if(bmp)
				{
					sprintf(cbmp, "brouk%d.bmp", bmp);
					cs[iActual].iSurface = ddxLoadBitmap(cbmp, pBmpArchive);

					if(cs[iActual].iSurface == -1)
					{
						cr_Release_Bitmaps(cs, iClock);
						apakclose(hArchive);
						return 0;
					}
					else
					{
						cr_Set_Surface(&cs[iActual], y);
						y += ddxGetHight(cs[iActual].iSurface);
						iActual++;
					}
				}

				bmp++;
			}

			cs[iActual].iSurface = ddxCreateSurface(1024, 90, ddxFindFreeSurface());

			if(cs[iActual].iSurface == -1)
			{
				cr_Release_Bitmaps(cs, iClock);
				apakclose(hArchive);
				return 0;
			}
			else
			{
				ddxCleareSurfaceColor(cs[iActual].iSurface, 0);
				cr_Set_Text_CenterW(cs[iActual].iSurface, (WCHAR *)text, 1, r);
				cs[iActual].y = y;
				iActual++;
			}
		}
		else
			if(strcmp(text,"\n") && strlen(text))
			{
				cs[iActual].iSurface = ddxCreateSurface(1024, 90, ddxFindFreeSurface());

				if(cs[iActual].iSurface == -1)
				{
					cr_Release_Bitmaps(cs, iClock);
					apakclose(hArchive);
					return 0;
				}
				else
				{
					ddxCleareSurfaceColor(cs[iActual].iSurface, 0);
					cr_Set_Text_CenterW(cs[iActual].iSurface, (WCHAR *)text, 0, r);
					cs[iActual].y = y;
					iActual++;
				}
			}

		y += 80;

		if(iActual >= CREDIT_SURFACES)
		{
			kprintf(1, "iActual >= CREDIT_SURFACES !!!!!!!!!!!");
			break;
		}

		DrawClock(iClock, 6);
	}

	aclose(file);

	y = 0;

	ap_Play_Song(11,0, p_ad);

	ddxResizeCursorBack(0);
	ddxSetCursorSurface(0);

	for(i=0;i<12;i++)
		if(iClock[i] != -1)
		{
			ddxReleaseBitmap(iClock[i]);
			iClock[i] = -1;
		}

	ddxSetCursor(0);
	dwStart = timeGetTime();

	while(!key[K_ESC])
	{
		y = (int)ftoi((dwEplased * dy) / (float) 195000);
	
		if(y >=  dy)
			key[K_ESC] = 1;

		cr_Draw_Creadits(cs, y);

		dwStop = timeGetTime();
		dwEplased = dwStop - dwStart;

		if(ddxRestore(&CompositDC, &FontDC, &BackDC, &iCompositDC, &iFontDC, &iBackDC, &cBrutalRestart, p_ad))
		{
			ddxSetFlip(1);
			ddxSetCursor(1);
			apakclose(hArchive);
			return 1;
		}
	}

	cr_Release_Bitmaps(cs, iClock);
	fn_Release_Font(1);

	if(ogg_playing())
		ap_Stop_Song(p_ad);

	Sleep(1000);
	ap_Play_Song(0, 0, p_ad);

	ddxSetCursor(1);
	ddxSetFlip(1);
	key[K_ESC] = 0;
	apakclose(hArchive);
*/
	return 0;
}
