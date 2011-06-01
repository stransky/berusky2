#include <stdio.h>
#include "Apak.h"
#include "font3d.h"
//#include "2d_graphic.h"
//#include "2ddx.h"

#include "3d_all.h"
#include "Berusky3d_kofola_interface.h"
#include "Berusky_universal.h"
#include "Berusky3d_kofola2d.h"

//#pragma comment(lib,"Msimg32.lib")

static B2_FONT b2_font;

extern _3D_DATA		_3dd;

void fn2_Release_Font(void);
void Uni2Char(WCHAR *cUni, char *cText, int ctsize);


int fn2_Find_Char(GAME_TRIGER *gt, TRIGER_STRUCTURE *ts, int *top, int *left, int *bottom, int *right, int *ycor, WCHAR cWChar,
				 float xbmp, float ybmp)
{
	float *l, *t, *r, *b, *y;
	int i;

/*	for(i=0;i<gt->lastcommand;i++)
		switch(gt->command[i].iCommand)
		{
			case 1:
				{
					if(gt->command[i].LastParam > 5 &&
					   gt->command[i].Parametr[0].Type == 3)
					{
						if((WCHAR)gt->command[i].Parametr[0].Value == cWChar)
						{
							l = (float *)&gt->command[i].Parametr[1].Value;
							t = (float *)&gt->command[i].Parametr[2].Value;
							r = (float *)&gt->command[i].Parametr[3].Value;
							b = (float *)&gt->command[i].Parametr[4].Value;
							y = (float *)&gt->command[i].Parametr[5].Value;

							*left = ftoi((*l) * xbmp);
							*top = ftoi((*t) * ybmp);
							*right = ftoi((*r) * xbmp);
							*bottom = ftoi((*b) * ybmp);
							*ycor = ftoi((*y) * ybmp);

							return 1;
						}
					}
				}
				break;
		}*/

	if(cWChar > b2_font.iMaxCharValue)
		return 0;

	i = b2_font.pTTable[cWChar];

	if(i < 0)
		return 0;
	else
	{
		l = (float *)&gt->command[i].Parametr[1].Value;
		t = (float *)&gt->command[i].Parametr[2].Value;
		r = (float *)&gt->command[i].Parametr[3].Value;
		b = (float *)&gt->command[i].Parametr[4].Value;
		y = (float *)&gt->command[i].Parametr[5].Value;

		*left = ftoi((*l) * xbmp);
		*top = ftoi((*t) * ybmp);
		*right = ftoi((*r) * xbmp);
		*bottom = ftoi((*b) * ybmp);
		*ycor = ftoi((*y) * ybmp);		
	}

	return 1;
}

int fn2_Create_TTable(int iMaxCharValue, GAME_TRIGER *gt, TRIGER_STRUCTURE *ts)
{
	int i;

	b2_font.pTTable = (int *) malloc((iMaxCharValue + 1)* sizeof(int));

	if(!b2_font.pTTable)
		return 0;

	for(i=0;i<iMaxCharValue + 1;i++)
		b2_font.pTTable[i] = -1;

	for(i=0;i<gt->lastcommand;i++)
		switch(gt->command[i].iCommand)
		{
			case 1:
				if(gt->command[i].Parametr[0].Value < (unsigned) iMaxCharValue + 1)
						b2_font.pTTable[(int)gt->command[i].Parametr[0].Value] = i;
				break;
		}

	b2_font.iMaxCharValue = iMaxCharValue;
	return 1;
}

int fn2_Set_Font_Params(GAME_TRIGER *gt, TRIGER_STRUCTURE *ts)
{
	int iMaxCharValue = 0;
	int i;

	for(i=0;i<gt->lastcommand;i++)
		switch(gt->command[i].iCommand)
		{
			case 1:
				if((int)gt->command[i].Parametr[0].Value > iMaxCharValue)
					iMaxCharValue = (int)gt->command[i].Parametr[0].Value;
				break;
			case 2:
				{
					if(gt->command[i].LastParam > 2 &&
					   gt->command[i].Parametr[0].Type == 0)
						b2_font.tcolor = RGB(gt->command[i].Parametr[0].Value, 
											 gt->command[i].Parametr[1].Value,
											 gt->command[i].Parametr[2].Value);
				}
				break;
			case 3:
				{
					if(gt->command[i].LastParam > 0 &&
					   gt->command[i].Parametr[0].Type == 0)
						b2_font.iYPlus = gt->command[i].Parametr[0].Value;
				}
				break;
			case 6:
				{
					if(gt->command[i].LastParam > 0 &&
					   gt->command[i].Parametr[0].Type == 0)
						b2_font.iXPlus = gt->command[i].Parametr[0].Value;
				}
				break;
		}

	if(!fn2_Create_TTable(iMaxCharValue, gt, ts))
		return 0;

	return 1;
}

int fn2_Set_Font_Bmps(GAME_TRIGER *gt, TRIGER_STRUCTURE *ts)
{
	char text[256];
	int i;

	txt_trida(TEXT_MENU);

	for(i=0;i<gt->lastcommand;i++)
		switch(gt->command[i].iCommand)
		{
			case 4:
				{
					if(gt->command[i].LastParam > 1 &&
					   gt->command[i].Parametr[1].Type == 2)
					{
					   memset(text, 0, 256);

					   WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK | WC_DEFAULTCHAR, (const unsigned short *)ts->StrTable[gt->command[i].Parametr[1].Value], 
											wcslen((const unsigned short *)ts->StrTable[gt->command[i].Parametr[1].Value]),
											text, 256, NULL, NULL);

					   b2_font.iBitmap[gt->command[i].Parametr[0].Value] = ddx2LoadBitmap(text, b2_font.pArchive);
					   //_2d_APAK_Load_Bitmap(text, b2_font.pArchive);
					}  
				}
				break;
			case 5:
				{
					if(gt->command[i].LastParam > 1 &&
					   gt->command[i].Parametr[1].Type == 2)
					{
					
					   memset(text, 0, 256);

					   WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK | WC_DEFAULTCHAR, (const unsigned short *)ts->StrTable[gt->command[i].Parametr[1].Value], 
											wcslen((const unsigned short *)ts->StrTable[gt->command[i].Parametr[1].Value]),
											text, 256, NULL, NULL);


						txt_nahraj_texturu_z_func(b2_font.pArchive, text, 
							&b2_font.tex[gt->command[i].Parametr[0].Value], 1, 0, 
							&b2_font.konf[gt->command[i].Parametr[0].Value],nahraj_aux);
					}
				}
				break;
		}

	return 0;
}

void fn2_Draw_Message(int iSurface, int iXpos, int iYpos, GAME_TRIGER *gt, TRIGER_STRUCTURE *ts, WCHAR *cFile, 
					 WCHAR *cStop, int iSection, int *iXmax, int *iYmax)
{
	WCHAR	wtext[1024];
	int		i;
	int		top, bottom, left, right, ycor;

	int		x = iXpos, y = iYpos - b2_font.iYPlus;

	*iXmax = x;
	*iYmax = y;

	i = wcslen(cFile);

	cFile = wcsstr((WCHAR *) b2_font.pTBuffer, cFile);
	
	if(!cFile)
		return;

	cStop = wcsstr(cFile, cStop);

	if(!cStop)
		return;

	ZeroMemory(wtext, 1024 * sizeof(WCHAR));
	wcsncpy(wtext, cFile + i,  cStop - cFile - i);

		for(i=0;i<(int)wcslen(wtext);i++)
		{
			if(wtext[i] == 32)
			{
				//x+= 50;
				x+=b2_font.iXPlus;
				continue;
			}
			else
				if(wtext[i] == 10)
				{
					//y+= 17;
					y+= b2_font.iYPlus;
					x = iXpos;

					*iYmax = y;
					continue;
				}
				else
					if(fn2_Find_Char(gt, ts, &top, &left, &bottom, &right, &ycor, wtext[i], 
									(float)ddx2GetWidth(b2_font.iBitmap[iSection]),
									(float)ddx2GetHeight(b2_font.iBitmap[iSection])))
					{		
						/*TransparentBltU(hdc, x, y + ycor, right - left + 1, bottom - top + 1, _2dd.bitmap[b2_font.iBitmap[iSection]].bitmapDC, 
										left, top, right - left + 1, bottom - top + 1, b2_font.tcolor);*/

						ddx2TransparentBlt(iSurface, x, y + ycor, right - left + 1, bottom - top + 1, 
										   b2_font.iBitmap[iSection],left, top, right - left +1, 
										   bottom - top +1, b2_font.tcolor);

						x += right - left + 2;

						if(x > *iXmax)
							*iXmax = x;
					}
		}
}

typedef struct
{
	int		xStart;
	int		xEnd;
	WCHAR	*wcWord;
	int		wLen;
} TEXT_WORLD;

int end_char(WCHAR wChar)
{
	if(wChar == 10 || wChar == 13 || wChar == 32)
		return 1;
	else
		return 0;
}

void fn2_Draw_Format_Line(GAME_TRIGER *gt, TRIGER_STRUCTURE *ts, TEXT_WORLD *tWord, int iWordl, 
						  int x, int y, int xmax, int iSection, int iSurface, char bFormat)
{
	int	top, bottom, left, right, ycor;
	int i, j;
	int sx;

	float fp = 0;
	float f = 0;

	if(bFormat)
		fp = (xmax - tWord[iWordl-1].xEnd) / (float)(iWordl - 1);

	if(!iWordl)
		return;

	for(i=0;i<iWordl;i++)
	{
		sx = tWord[i].xStart + ftoi(f);

		for(j=0;j<128;j++)
		{
			if(!tWord[i].wcWord)
				return;

			if(!end_char(tWord[i].wcWord[j]))
			{
				if(fn2_Find_Char(gt, ts, &top, &left, &bottom, &right, &ycor, tWord[i].wcWord[j], 
								(float)ddx2GetWidth(b2_font.iBitmap[iSection]),
								(float)ddx2GetHeight(b2_font.iBitmap[iSection])))
				{
					ddx2TransparentBlt(iSurface, sx, y + ycor, right - left + 1, bottom - top + 1, 
									   b2_font.iBitmap[iSection],left, top, right - left +1, 
									   bottom - top +1, b2_font.tcolor);

					sx += right - left + 2;
				}
			}
			else
				break;
		}

		f += fp;
	}
}

void fn2_Draw_MessageRECT(int iSurface, int iXpos, int iYpos, GAME_TRIGER *gt, TRIGER_STRUCTURE *ts, WCHAR *cFile, 
					      WCHAR *cStop, int iSection, int *iXmax, int *iYmax, RECT *pR)
{
	//rovnatko...................
	TEXT_WORLD	tWord[64];
	int			iWordl = 0;
	//rovnatko...................

	WCHAR	wtext[2024];
	int		i;
	int		top, bottom, left, right, ycor;
	
	int		x = iXpos, y = iYpos - b2_font.iYPlus;
	int		lx;

	ZeroMemory(tWord, 64 * sizeof(TEXT_WORLD));
	*iXmax = x;
	*iYmax = y;

	i = wcslen(cFile);

	cFile = wcsstr((WCHAR *) b2_font.pTBuffer, cFile);
	
	if(!cFile)
		return;

	cStop = wcsstr(cFile, cStop);

	if(!cStop)
		return;

	ZeroMemory(wtext, 2024 * sizeof(WCHAR));
	wcsncpy(wtext, cFile + i,  cStop - cFile - i);

		for(i=0;i<(int)wcslen(wtext);i++)
		{
			if(wtext[i] == 32)
			{
				x+=b2_font.iXPlus;

				//pokud posledni pismeno nebyla mezera ani enter, tak konec slova
				if(!end_char(wtext[i-1]))
				{
					tWord[iWordl].xEnd = x - b2_font.iXPlus;
					tWord[iWordl].wLen = &wtext[i] - tWord[iWordl].wcWord;
					iWordl++;
				}

				continue;
			}
			else
				if(wtext[i] == 10)
				{
					//y+= 17;
					y+= b2_font.iYPlus;
					x = iXpos;

					*iYmax = y;

					if(!end_char(wtext[i-1]))
					{
						tWord[iWordl].xEnd = x - b2_font.iXPlus;
						tWord[iWordl].wLen = &wtext[i] - tWord[iWordl].wcWord;
						iWordl++;
					}

					fn2_Draw_Format_Line(gt, ts, tWord, iWordl+1, pR->left, y - b2_font.iYPlus, pR->right, iSection, iSurface, 0);

					ZeroMemory(tWord, 64 * sizeof(TEXT_WORLD));
					iWordl = 0;
					continue;
				}
				else
					if(fn2_Find_Char(gt, ts, &top, &left, &bottom, &right, &ycor, wtext[i], 
									(float)ddx2GetWidth(b2_font.iBitmap[iSection]),
									(float)ddx2GetHeight(b2_font.iBitmap[iSection])))
					{
						//pretekl sem?
						if(x + right - left + 2 > pR->right)
						{
							if(tWord[iWordl].xStart)
							{
								lx = x - tWord[iWordl].xStart;
								x = pR->left + (x - tWord[iWordl].xStart);
							}
							else
								x = pR->left;

							if(iWordl > 0)
							{
								if(tWord[iWordl-1].wLen == 1)
								{
									if(!tWord[iWordl].xStart)
									{
										fn2_Draw_Format_Line(gt, ts, tWord, iWordl - 1, pR->left, y, pR->right, iSection, iSurface, 1);
	
										y+= b2_font.iYPlus;
										ZeroMemory(&tWord[0], sizeof(TEXT_WORLD));

										tWord[0].wcWord = tWord[iWordl - 1].wcWord;
										tWord[0].xEnd = pR->left + (tWord[iWordl - 1].xEnd - tWord[iWordl - 1].xStart);
										tWord[0].xStart = pR->left;
										tWord[0].wLen = tWord[iWordl - 1].wLen;

										ZeroMemory(&tWord[1], 63 * sizeof(TEXT_WORLD));

										x = tWord[0].xEnd + b2_font.iXPlus;

										iWordl = 1;
									}
									else
									{
										fn2_Draw_Format_Line(gt, ts, tWord, iWordl - 1, pR->left, y, pR->right, iSection, iSurface, 1);
	
										y+= b2_font.iYPlus;
										ZeroMemory(&tWord[0], sizeof(TEXT_WORLD));

										tWord[0].wcWord = tWord[iWordl - 1].wcWord;
										tWord[0].xEnd = pR->left + (tWord[iWordl - 1].xEnd - tWord[iWordl - 1].xStart);
										tWord[0].xStart = pR->left;
										tWord[0].wLen = tWord[iWordl - 1].wLen;

										ZeroMemory(&tWord[1], sizeof(TEXT_WORLD));

										tWord[1].wcWord = tWord[iWordl].wcWord;
										tWord[1].xStart = tWord[0].xEnd + (tWord[iWordl].xStart - tWord[iWordl - 1].xEnd);

										ZeroMemory(&tWord[2], 62 * sizeof(TEXT_WORLD));

										x = tWord[1].xStart + lx;

										iWordl = 1;
									}
								}
								else
								{
									FN2_DRAW_MESSAGERECT_NORMAL_FORMAT_LINE:

									fn2_Draw_Format_Line(gt, ts, tWord, iWordl, pR->left, y, pR->right, iSection, iSurface, 1);

									y+= b2_font.iYPlus;
									ZeroMemory(&tWord[0], sizeof(TEXT_WORLD));

									tWord[0].wcWord = tWord[iWordl].wcWord;
									tWord[0].xStart = pR->left;

									ZeroMemory(&tWord[1], 63 * sizeof(TEXT_WORLD));

									iWordl = 0;
								}
							}
							else
								goto FN2_DRAW_MESSAGERECT_NORMAL_FORMAT_LINE;

							*iYmax = y;
						}			

						/*ddx2TransparentBlt(iSurface, x, y + ycor, right - left + 1, bottom - top + 1, 
										   b2_font.iBitmap[iSection],left, top, right - left +1, 
										   bottom - top +1, b2_font.tcolor);*/

						//pokud posledni pismeno byla mezera nebo enter, tak zacatek slova
						if(end_char(wtext[i-1]))
						{
							tWord[iWordl].xStart = x;
							tWord[iWordl].wcWord = &wtext[i];
						}

						x += right - left + 2;

						if(x > *iXmax)
							*iXmax = x;
					}
		}
}

void fn2_Draw_MessageA(int iSurface, int iXpos, int iYpos, GAME_TRIGER *gt, TRIGER_STRUCTURE *ts, WCHAR *cText, 
					 int iSection, int *iXmax, int *iYmax)
{
	WCHAR	wtext[1024];
	int		i;
	int		top, bottom, left, right, ycor;

	int		x = iXpos, y = iYpos;

	*iXmax = x;
	*iYmax = y;

	ZeroMemory(wtext, 1024 * sizeof(WCHAR));
	wcsncpy(wtext, cText,  wcslen(cText));

		for(i=0;i<(int)wcslen(wtext);i++)
		{
			if(wtext[i] == 32)
			{
				//x+= 50;
				x+=b2_font.iXPlus;
				continue;
			}
			else
				if(wtext[i] == 10)
				{
					//y+= 17;
					y+= b2_font.iYPlus;
					x = iXpos;

					*iYmax = y;
					continue;
				}
				else
					if(fn2_Find_Char(gt, ts, &top, &left, &bottom, &right, &ycor, wtext[i], 
									(float)ddx2GetWidth(b2_font.iBitmap[iSection]),
									(float)ddx2GetHeight(b2_font.iBitmap[iSection])))
					{		
						/*TransparentBltU(hdc, x, y + ycor, right - left + 1, bottom - top + 1, _2dd.bitmap[b2_font.iBitmap[iSection]].bitmapDC, 
										left, top, right - left + 1, bottom - top + 1, b2_font.tcolor);*/

						ddx2TransparentBlt(iSurface, x, y + ycor, right - left + 1, bottom - top + 1, 
										  b2_font.iBitmap[iSection],left, top, right - left +1, 
										  bottom - top +1, b2_font.tcolor);

						x += right - left + 2;

						if(x > *iXmax)
							*iXmax = x;
					}
		}
}

void fn2_Set_Char(unsigned int *pTexture, int iXSize, int iYSize, int iXpos, int iYpos, 
				 unsigned int *pSource, int iXSSize, int iYSSize,
				 int iCXSize, int iCYSize, int iXCpos, int iYCpos)
{
	unsigned int *pT = &pTexture[(iXSize * (iYSize - iYpos)) - (iXSize - iXpos)];
	unsigned int *pS = &pSource[(iXSSize * (iYSSize - iYCpos)) - (iXSSize - iXCpos)];

	int y;

	for(y = iYCpos; y < iYCpos + iCYSize;y++)
	{
		memcpy((void *) pT, (void *) pS, iCXSize * sizeof(unsigned int));

		pT -= iXSize;
		pS -= iXSSize;
	}
}

void fn2_Gen_Texture(byte **lpTexture, int iXSize, int iYSize, int iXpos, int iYpos, GAME_TRIGER *gt, 
					TRIGER_STRUCTURE *ts, WCHAR *cFile, WCHAR *cStop, int iSection, int *iXres,
					int *iYres)
{
	WCHAR	wtext[1024];
	int		i;
	int		top, bottom, left, right, ycor;
	byte	*pT = NULL;

	int		x = iXpos, y = iYpos - b2_font.iYPlus;

	*iXres = iXpos;
	*iYres = iYpos;

	i = wcslen(cFile);

	cFile = wcsstr((WCHAR *) b2_font.pTBuffer, cFile);
	
	if(!cFile)
		return;

	cStop = wcsstr(cFile, cStop);

	if(!cStop)
		return;

	ZeroMemory(wtext, 1024 * sizeof(WCHAR));
	wcsncpy(wtext, cFile + i,  cStop - cFile - i);

	pT = (byte	*) malloc(iXSize * iYSize * sizeof(unsigned int));

	if(!pT)
		return;
	
	*lpTexture = pT;

	ZeroMemory(pT, iXSize * iYSize * sizeof(unsigned int));

		for(i=0;i<(int)wcslen(wtext);i++)
		{
			if(wtext[i] == 32)
			{
				x+= b2_font.iXPlus;
				continue;
			}
			else
				if(wtext[i] == 10)
				{
					//y+= 17;
					y+= b2_font.iYPlus;
					// *iYres = y;
					x = iXpos;
					continue;
				}          
				else if(fn2_Find_Char(gt, ts, &top, &left, &bottom, &right, &ycor, wtext[i],
									(float)b2_font.tex[iSection].p_bmp->x,
									(float)b2_font.tex[iSection].p_bmp->y))
					{          
						if(b2_font.tex[iSection].p_bmp)
						{
							fn2_Set_Char((unsigned int *)pT, iXSize, iYSize, x, y + ycor, 
									    (unsigned int *)b2_font.tex[iSection].p_bmp->data, 
										b2_font.tex[iSection].p_bmp->x, b2_font.tex[iSection].p_bmp->y,
										right - left, bottom - top, left, top);
						}

						x += right - left + 2;

						if(x > *iXres)
							*iXres = x;

						if(bottom - top > *iYres)
							*iYres = bottom - top;
					}          
		}
}

int fn2_Open_Archive(char *cFile, APAK_HANDLE **pAHandle, char *cAppName, char *cKeyName)
{
	int	e;
	char text[256];

	GetPrivateProfileString(cAppName,cKeyName,"c:\\",text,256,ini_file);
	chdir(text);

  apak_dir_correction(text);
	(*pAHandle) = apakopen(cFile, text, &e);

	if(!(*pAHandle))
	{
		kprintf(1, "Unable to open archive %s", cFile);

		switch(e)
		{
		case APAK_FILE_NOT_FOUND:
			kprintf(1, "Reason: File not found");
			break;
		case APAK_UNABLE_TO_READ:
			kprintf(1, "Reason: Unable to read from file");
			break;
		case APAK_VERSION_MISMATCH:
			kprintf(1, "Reason: Version mismatch");
			break;
		case APAK_OUT_OF_MEMORY:
			kprintf(1, "Reason: Out of memory");
			break;
		}

		sprintf(text, "Unable to open archive %s", cFile);
		//MessageBox(NULL,text,"Error",MB_OK);

		return 0;
	}

	achdir((*pAHandle), text);

	kprintf(1, "APAK: %s", cFile);
	kprintf(1, "Velikost AFAT: %.1fKB", (*pAHandle)->FileHeader.apuISizeofFAT / 1000.0f);
	kprintf(1, "Velikost Archivu: %.1fMB", (*pAHandle)->FileHeader.apuLSizeofPAK / 1000000.0f);
	kprintf(1, "Souboru: %d", (*pAHandle)->FileHeader.apuICountofFiles);
	kprintf(1, "Adresaru: %d", (*pAHandle)->FileHeader.apuICountofDirectiories);
	kprintf(1, "Uzlu: %d", (*pAHandle)->FileHeader.apuICountofNodes);

	return 1;
}

int fn2_Load_Grammar(char *pFile, GRAMMAR *pGr)
{
	FILE	*file;
	char	text[256];
	
	file = aopen(b2_font.pArchive, pFile, "rb");

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

char fn2_Load_Triger(char *pFile, GAME_TRIGER *pTriger, GRAMMAR *pGr, TRIGER_STRUCTURE *pTStruct)
{
	FILE	*file;
	char	text[256];
	WCHAR	wtext[128];
  word  wotext[128];

	
	pTriger->lastcommand = 0;

	file = aopen(b2_font.pArchive, pFile, "rb");

	aunicode(file);

	if(!file)
		return 0;

	aseek(file, 2, SEEK_SET);

	while(!aeof(file))
	{
		memset(wtext, 0, 128 * sizeof(WCHAR));
		agets((char *) wotext,256,file);
		wchar_windows_to_linux(wotext, 128, wtext);
		trig_Parse_LineU(wtext, &pTriger->command[pTriger->lastcommand], pTriger, pGr, pTStruct);		
	}

	aclose(file);
	return 1;
}

int fn2_Set_Font(char *cPAK)
{
	int i;

	memset(&b2_font, 0, sizeof(B2_FONT));

	if(!fn2_Open_Archive(cPAK, &b2_font.pArchive,"game","bitmap_dir"))
		return 0;

	b2_font.file = aopen(b2_font.pArchive, "texts.txt", "rb");

	if(!b2_font.file)
	{
		apakclose(b2_font.pArchive);
		return 0;
	}
	else
	{
    int iTSize;
		agetbuffer(b2_font.file, (char **)&b2_font.pTBuffer, &iTSize);
    b2_font.pTBuffer = wchar_windows_to_linux((word *)b2_font.pTBuffer, iTSize);

		if(!b2_font.pTBuffer)
		{
			apakclose(b2_font.pArchive);
			return 0;
		}
	}

	if(!fn2_Load_Grammar("font_grammar.txt", &b2_font.gr))
		return 0;

	b2_font.ts.LastStr = 0;
	b2_font.ts.sizeofT = 0;
	b2_font.ts.pTriger = NULL;
	
	if(!fn2_Load_Triger("font_def.txt", &b2_font.gt, &b2_font.gr, &b2_font.ts))
		return 0;

	for(i=0;i<FONT_MAX_BMP;i++)
		b2_font.iBitmap[i] = -1;

	b2_font.pTTable = NULL;
	
	if(!fn2_Set_Font_Params(&b2_font.gt, &b2_font.ts))
		return 0;

	return 1;
}

int fn2_Load_Bitmaps(void)
{
	if(!b2_font.pArchive)
		return 0;

	kom_set_default_text_config(0,0,1,0,0,1);
	fn2_Set_Font_Bmps(&b2_font.gt, &b2_font.ts);
	kom_ret_default_text_config();

	return 1;
}

void fn2_Release_Font(void)
{
	int i;

	aclose(b2_font.file);
	apakclose(b2_font.pArchive);

//	if(_2dd.bitmap)
		for(i=0;i<FONT_MAX_BMP;i++)
			if(b2_font.iBitmap[i] != -1)
			{
				ddx2ReleaseBitmap(b2_font.iBitmap[i]);
				//_2d_Release_Bitmap(b2_font.iBitmap[i]);
			}

	for(i=0;i<FONT_MAX_BMP;i++)
		if(b2_font.tex[i].load)
		{
			txt_zrus_texturu_ram(&b2_font.tex[i]);
			b2_font.tex[i].load = 0;
		}

	//b2_font.iTSize = 0;

	if(b2_font.pTTable)
		free((void *) b2_font.pTTable);

	b2_font.pTTable = NULL;
}

void fn2_Release_3d_Textures_Full(void)
{
	int i;

	for(i=133;i<_3dd.last;i++)
	{
		txt_zrus_texturu(&_3dd.p_texture[i]);
		_3dd.p_texture[i].text = -1;
		free((void *) _3dd.p_sysramtexture[i].data);
	}

	_3dd.last = 133;
}

void fn2_Release_3d_Textures_RAM(void)
{
	int i;

	for(i=133;i<_3dd.last;i++)
	{
		free((void *) _3dd.p_sysramtexture[i].data);
		_3dd.p_sysramtexture[i].data = NULL;
	}

	_3dd.last = 133;
}

void fn2_Release_3d_Textures(void)
{
	int i;

	for(i=133;i<_3dd.last;i++)
	{
		txt_zrus_texturu(&_3dd.p_texture[i]);
		_3dd.p_texture[i].text = -1;
	}
}

void fn2_Test(int hdc)
{
	int x, y;

	WCHAR wc[64];
	WCHAR ws[64];

	MultiByteToWideChar( CP_ACP, 0, "##message1", strlen("##message1")+1, wc, sizeof(wc)/sizeof(wc[0]));
	MultiByteToWideChar( CP_ACP, 0, "##endofmessage", strlen("##endofmessage")+1, ws, sizeof(ws)/sizeof(ws[0]));

	fn2_Draw_Message(hdc, 360, 230, &b2_font.gt, &b2_font.ts, wc, ws, 0, &x, &y);
}

void fn2_Test2(int hdc)
{
	int x, y;

	WCHAR wc[64];
	WCHAR ws[64];

	MultiByteToWideChar( CP_ACP, 0, "##settings", strlen("##settings")+1, wc, sizeof(wc)/sizeof(wc[0]));
	MultiByteToWideChar( CP_ACP, 0, "##endofmessage", strlen("##endofmessage")+1, ws, sizeof(ws)/sizeof(ws[0]));

	fn2_Draw_Message(hdc, 800, 600, &b2_font.gt, &b2_font.ts, wc, ws, 0, &x, &y);
}

int fn2_Text_Blt(int hdc, WCHAR *ws, WCHAR *wc, int iSurface, int iSection, int *i)
{
	int x, y;
	int tmpDC;

	//BitBlt(fDC,0,0,1024,200,NULL,0,0,WHITENESS);

/*	{
		RECT r = {-1, -1, 1025, 201};
		_2d_Fill_Rect(fDC, r, RGB(255, 0, 255));
	}*/

	fn2_Draw_Message(iSurface, 0, 0, &b2_font.gt, &b2_font.ts, wc, ws, iSection, &x, &y);	
	
	//*i = _2d_Find_Free_Surface();
	*i = ddx2FindFreeSurface();

	//tmpDC = fn2_CreateDC(hdc, x, y, *i);
	tmpDC = ddx2CreateSurface(x, y, *i);

	if(!tmpDC)
		return 0;
	else
		ddx2TransparentBlt(tmpDC, 0, 0, x, y, iSurface, 0, 0, x, y, TRANSCOLOR);

		//TransparentBltU(tmpDC, 0, 0, x, y, fDC, 0, 0, x, y, RGB(255, 0, 255));

	return 1;
}

int fn2_Gen_Menu_Text(int iSection, int hdc, char *cText, int *i1, int *i2)
{
	WCHAR wc[64];
	WCHAR ws[64];
	//int sidx = _2d_Find_Free_Surface();
	int sidx = ddx2FindFreeSurface();
	//HDC	fDC = fn2_CreateDC(hdc, 1024, 200, sidx);
	int	fDC = ddx2CreateSurface(1024, 200, sidx);

	if(!fDC)
		return -1;

	MultiByteToWideChar( CP_ACP, 0, cText, strlen(cText)+1, wc, sizeof(wc)/sizeof(wc[0]) );
	MultiByteToWideChar( CP_ACP, 0, "##endofmessage", strlen("##endofmessage")+1, ws, sizeof(ws)/sizeof(ws[0]) );

	if(!fn2_Text_Blt(hdc, ws, wc, fDC, iSection, i1))
	{
		ddx2ReleaseBitmap(sidx);
		return -1;
	}

	if(!fn2_Text_Blt(hdc, ws, wc, fDC, iSection+1, i2))
	{
		ddx2ReleaseBitmap(sidx);
		return -1;
	}

	ddx2ReleaseBitmap(sidx);

	return sidx;
}

int fn2_Gen_Menu_Texts(int iSection, int hdc)
{
/*	int x;
	WCHAR wc[64];
	WCHAR ws[64];
	//int sidx = _2d_Find_Free_Surface();
	int sidx = ddx2FindFreeSurface();
	int	fDC = ddx2CreateSurface(1024, 200, sidx);

	if(!fDC)
		return 0;

	MultiByteToWideChar( CP_ACP, 0, "##menu_newgame", strlen("##menu_newgame")+1, wc, sizeof(wc)/sizeof(wc[0]) );
	MultiByteToWideChar( CP_ACP, 0, "##endofmessage", strlen("##endofmessage")+1, ws, sizeof(ws)/sizeof(ws[0]) );

	if(!fn2_Text_Blt(hdc, ws, wc, fDC, 0, &x))
		return 0;

	if(!fn2_Text_Blt(hdc, ws, wc, fDC, 1, &x))
		return 0;

	MultiByteToWideChar( CP_ACP, 0, "##menu_loadgame", strlen("##menu_loadgame")+1, wc, sizeof(wc)/sizeof(wc[0]) );
	if(!fn2_Text_Blt(hdc, ws, wc, fDC, 0, &x))
		return 0;

	if(!fn2_Text_Blt(hdc, ws, wc, fDC, 1, &x))
		return 0;

	MultiByteToWideChar( CP_ACP, 0, "##menu_settings", strlen("##menu_settings")+1, wc, sizeof(wc)/sizeof(wc[0]) );
	if(!fn2_Text_Blt(hdc, ws, wc, fDC, 0, &x))
		return 0;

	if(!fn2_Text_Blt(hdc, ws, wc, fDC, 1, &x))
		return 0;

	MultiByteToWideChar( CP_ACP, 0, "##menu_credits", strlen("##menu_credits")+1, wc, sizeof(wc)/sizeof(wc[0]) );
	if(!fn2_Text_Blt(hdc, ws, wc, fDC, 0, &x))
		return 0;

	if(!fn2_Text_Blt(hdc, ws, wc, fDC, 1, &x))
		return 0;

	ddx2_Release_Bitmap(sidx);*/

	return 1;
}

int fn2_Gen_Menu(void)
{
	return fn2_Gen_Menu_Texts(0, HDC2DD);
}

#define FONT_X_MAX 1024
#define FONT_Y_MAX 1024

int fn2_Up(int iValue)
{
	int i, x = 4;

	for(i=3;i<12;i++)
	{
		x = x * 2;

		if(x > iValue)
			return x;
	}

	return FONT_X_MAX;
}

int fn2_Blt(byte *pT, byte **pD, int ix, int iy)
{
	int y;
	unsigned int *puT = (unsigned int *)pT+(FONT_X_MAX * (FONT_Y_MAX - 1));
	unsigned int *puD;

	*pD = (byte *) malloc(ix * iy * sizeof(unsigned int));

	if(!(*pD))
		return 0;

	memset((*pD), 0, ix * iy * sizeof(unsigned int));

	puD = (unsigned int *)(*pD)+(ix * (iy - 1));

	for(y = 0; y<iy;y++)
	{
		
		memcpy((void *) puD, (void *) puT, ix * sizeof(unsigned int));

		puT -= FONT_X_MAX;
		puD -= ix;
	}

	for(y=0;y<FONT_X_MAX * FONT_Y_MAX;y++)
		if(pT[y])
			pT[y] = pT[y];

	free((void *) pT);

	return 1;
}

int fn2_Put_in_3d_List(int text, EDIT_TEXT *p_tex, EDIT_TEXT_KONFIG *p_konf, int x, int y, char *pMem, int ox, int oy)
{
	memcpy(&_3dd.p_texture[_3dd.last], p_tex, sizeof(EDIT_TEXT));
	memcpy(&_3dd.p_sysramtexture[_3dd.last].konf, p_konf, sizeof(EDIT_TEXT_KONFIG));

	_3dd.p_texture[_3dd.last].text = text;
	
	_3dd.p_sysramtexture[_3dd.last].data = pMem;
	_3dd.p_sysramtexture[_3dd.last].x = x;
	_3dd.p_sysramtexture[_3dd.last].y = y;

	_3dd.p_sysramtexture[_3dd.last].tx = ox;
	_3dd.p_sysramtexture[_3dd.last].ty = oy;

	_3dd.p_sysramtexture[_3dd.last].bSLoaded = 0;
	_3dd.p_sysramtexture[_3dd.last].bVLoaded = 0;

	_3dd.last++;

	return _3dd.last-1;
}

int fn2_Get_Font_Texture(int iSection, char *cText)
{
	int	tx, ty;
	int Xmax, Ymax;

	byte *pT = NULL;
	byte *pnT = NULL;

	WCHAR wc[64];
	WCHAR ws[64];

	MultiByteToWideChar( CP_ACP, 0, cText, strlen(cText)+1, wc, sizeof(wc)/sizeof(wc[0]) );
	MultiByteToWideChar( CP_ACP, 0, "##endofmessage", strlen("##endofmessage")+1, ws, sizeof(ws)/sizeof(ws[0]) );

	fn2_Gen_Texture(&pT, FONT_X_MAX, FONT_Y_MAX, 0, 0, &b2_font.gt, &b2_font.ts, wc, ws, iSection, &Xmax, &Ymax);

	tx = Xmax;
	ty = Ymax;
	Xmax = fn2_Up(Xmax);
	Ymax = fn2_Up(Ymax);

	fn2_Blt(pT, &pnT, Xmax, Ymax);

	/*glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, b2_font.tex.p_bmp->x, b2_font.tex.p_bmp->y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
				 b2_font.tex.p_bmp->data);*/

	return fn2_Put_in_3d_List(b2_font.tex[iSection].text, &b2_font.tex[iSection], &b2_font.konf[iSection], Xmax, Ymax, pnT, tx, ty);
}

void fn2_Load_Textures_From_RAM(void)
{
	int i;

	for(i=133;i<_3dd.last;i++)
	{
		glGenTextures(1, &_3dd.p_texture[i].text);
		glBindTexture(GL_TEXTURE_2D, _3dd.p_texture[i].text);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _3dd.p_sysramtexture[i].x, _3dd.p_sysramtexture[i].y, 
					 0, GL_RGBA, GL_UNSIGNED_BYTE, _3dd.p_sysramtexture[i].data);

	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); 
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	}
}

void fn2_Texture(int iSection)
{
	int i;

	for(i=0;i<=iSection;i++)
		fn2_Get_Font_Texture(i, "##restartlevel");

	for(i=0;i<=iSection;i++)
		fn2_Get_Font_Texture(i, "##mainmenu");

	for(i=0;i<=iSection;i++)
		fn2_Get_Font_Texture(i, "##settings");

	for(i=0;i<=iSection;i++)
		fn2_Get_Font_Texture(i, "##recorddemo");

	for(i=0;i<=iSection;i++)
		fn2_Get_Font_Texture(i, "##back");
}

//int fn2_Find_Char(GAME_TRIGER *gt, TRIGER_STRUCTURE *ts, int *top, int *left, int *bottom, int *right, int *ycor, WCHAR cWChar)
void fn2_Convert_Rect(char *cFile, int xmax, int ymax)
{
	FILE *f;
	FILE *fi;
	GAME_TRIGER *gt = &b2_font.gt;
	TRIGER_STRUCTURE *ts = &b2_font.ts;
	int i;
	char text[256];
	char textt[256];
	char t[32];
	float *fl1, *fl2, *fl3, *fl4, *fl5;

	f = fopen("fontout.txt","w");
	fi = fopen("font_def.txt","r");

	for(i=0;i<gt->lastcommand;i++)
		switch(gt->command[i].iCommand)
		{
			case 1:
				{
					if(gt->command[i].LastParam > 5 &&
					   gt->command[i].Parametr[0].Type == 3)
					{
						fgets(textt, 256, fi);
						strncpy(t, textt, 10);
						t[10] = '\0';
						strcpy(text,"");

						fl1 = (float *)&gt->command[i].Parametr[1].Value;
						fl2 = (float *)&gt->command[i].Parametr[2].Value;
						fl3 = (float *)&gt->command[i].Parametr[3].Value;
						fl4 = (float *)&gt->command[i].Parametr[4].Value;
						fl5 = (float *)&gt->command[i].Parametr[5].Value;

						sprintf(text, "%s %f, %f, %f, %f, %f);\n", t,
														    (*fl1) * 0.75f,
															(*fl2) * 0.75f,
															(*fl3) * 0.75f,
															(*fl4) * 0.75f,
															(*fl5) * 0.75f);

						fputs(text, f);
					}
				}
				break;
		}

	fclose(fi);
	fclose(f);
	return;
}

void fn2_Draw_Line(int x1, int y1, int x2, int y2, COLORREF color, HDC hdc)
{
/* TODO
	HPEN	LastPen;
	HPEN	MePen;

	MePen = CreatePen(PS_SOLID,1,color);
	LastPen = (HPEN)SelectObject(hdc,MePen);

	MoveToEx(hdc,x1, y1, NULL);
	LineTo(hdc,x2, y2);

	SelectObject(hdc,LastPen);
	DeleteObject(MePen);
*/
}

int fn2_DC2Tex(HDC hdc, int xr, int yr, int turn, int texture)
{
/*
	int					tex;
	int					i, c = 0;
//	HDC					hdcMem;
	BITMAPINFO			*pbmiRGB = (BITMAPINFO *) NULL; 
	HBITMAP				hbmRGB  = (HBITMAP) NULL;
	PBYTE				pjBitsRGB;
	PBYTE				pjBitsTEX;

//	hdcMem  = CreateCompatibleDC(hdc);

	pbmiRGB = (BITMAPINFO *)LocalAlloc(LMEM_FIXED|LMEM_ZEROINIT, sizeof(BITMAPINFO) );
	
	if (!pbmiRGB)
		return 0;
	
	pbmiRGB->bmiHeader.biSize	       = sizeof(BITMAPINFOHEADER);     
	pbmiRGB->bmiHeader.biWidth		   = xr;     
	pbmiRGB->bmiHeader.biHeight        = yr;     
	pbmiRGB->bmiHeader.biPlanes        = 1;     
	pbmiRGB->bmiHeader.biBitCount      = 24;     
	pbmiRGB->bmiHeader.biCompression   = BI_RGB;     
	pbmiRGB->bmiHeader.biSizeImage     = pbmiRGB->bmiHeader.biWidth
										 * abs(pbmiRGB->bmiHeader.biHeight) * 3; 
*/
/*	hbmRGB = CreateDIBSection(hdcMem, pbmiRGB, DIB_RGB_COLORS,
							  (PVOID *) &pjBitsRGB, NULL, 0);
	
	if (!hbmRGB) 
		return 0;

	if (!SelectObject(hdcMem, hbmRGB))
		return 0;
 
	if (!BitBlt(hdcMem, 0,0, xr, yr, hdc, 0,0,SRCCOPY)) 
		return 0;*/
/*
	pjBitsRGB = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbmiRGB->bmiHeader.biSizeImage);

	if (!pjBitsRGB) 
		return 0;

	if (!GetDIBits(hdc, hbmRGB, 0, (UINT)pbmiRGB->bmiHeader.biHeight, pjBitsRGB, 
				   (LPBITMAPINFO)pbmiRGB, DIB_RGB_COLORS))
		return 0;

	pjBitsTEX = (PBYTE) malloc(pbmiRGB->bmiHeader.biWidth * abs(pbmiRGB->bmiHeader.biHeight) * 4);

	memset(pjBitsTEX, 0, pbmiRGB->bmiHeader.biWidth * abs(pbmiRGB->bmiHeader.biHeight) * 4);

	for(i=0;i<(signed)pbmiRGB->bmiHeader.biSizeImage;i+=3)
	{
		pjBitsTEX[c] = pjBitsRGB[i+2];
		pjBitsTEX[c+1] = pjBitsRGB[i+1];
		pjBitsTEX[c+2] = pjBitsRGB[i];
		pjBitsTEX[c+3] = 0xff;

		c+=4;
	}

	if(!turn)
	{
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, xr, yr, 0, GL_RGBA, GL_UNSIGNED_BYTE, pjBitsTEX);

		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); 
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	}
	else
	{
		//void glTexSubImage2D  ( GLenum target , GLint level , GLint xoffset , GLint yoffset , GLsizei width , 
		//						GLsizei height , GLenum format , GLenum type , const GLvoid *pixels ); 
		glTexSubImage2D(texture, 0, 0, 0, 1024, 768, GL_RGBA, GL_TEXTURE_2D, pjBitsTEX);
	}

	//DeleteDC(hdcMem);          
	DeleteObject(hbmRGB);
	LocalFree(pbmiRGB);
	GlobalFree((HGLOBAL)pjBitsRGB);
	free((void *) pjBitsTEX);

	return tex;
*/
}
