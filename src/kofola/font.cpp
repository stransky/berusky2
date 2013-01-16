#include <stdio.h>
#include "font.h"
#include "2D_graphic.h"
#include "menu_def.h"

#include "3d_all.h"
#include "Berusky3d_kofola_interface.h"
#include "Berusky_universal.h"

B2_FONT b2_2d_font;

extern _3D_DATA _3dd;

void fn_Release_Font(int bTextures);
void Uni2Char(WCHAR * cUni, char *cText, int ctsize);

/*HDC fn_CreateDC(HDC hdc, int x, int y, int _2dd_idx)
{
	HDC				hdcBack = NULL;
	HBITMAP			hback_bitmap = NULL;
//	BITMAP			back_bitmap;

	if(!hdc)
		return NULL;
	
	hdcBack = CreateCompatibleDC(hdc);
	if(!hdcBack)
		return 0;
	
	hback_bitmap = CreateCompatibleBitmap(hdc, x, y);
	if(!hback_bitmap)
		return 0;

	//GetObject(hback_bitmap, sizeof(back_bitmap), &back_bitmap);
	_2dd.bitmap[_2dd_idx].old_hbitmap = SelectObject(hdcBack, hback_bitmap);

	//DeleteObject(hback_bitmap);

	SetStretchBltMode(hdcBack, HALFTONE);
	SetBrushOrgEx(hdcBack,0,0,NULL);

	_2dd.bitmap[_2dd_idx].bitmapDC = hdcBack;

	_2dd.bitmap[_2dd_idx].bitmap.bmType = 0;
	_2dd.bitmap[_2dd_idx].bitmap.bmWidth = x;
	_2dd.bitmap[_2dd_idx].bitmap.bmHeight = y;
	_2dd.bitmap[_2dd_idx].bLoad = 1;
	_2dd.bitmap[_2dd_idx].hbitmap = hback_bitmap;

	BitBltU(hdcBack,0,0,x,y,NULL,0,0,WHITENESS);

	return hdcBack;
}*/


int fn_Find_Char(GAME_TRIGER * gt, TRIGER_STRUCTURE * ts, int *top, int *left,
  int *bottom, int *right, int *ycor, WCHAR cWChar, float xbmp, float ybmp)
{
  float *l, *t, *r, *b, *y;
  int i;

  if (cWChar > b2_2d_font.iMaxCharValue)
    return 0;

  i = b2_2d_font.pTTable[cWChar];

  if (i < 0)
    return 0;
  else {
    l = (float *) &gt->command[i].Parametr[1].Value;
    t = (float *) &gt->command[i].Parametr[2].Value;
    r = (float *) &gt->command[i].Parametr[3].Value;
    b = (float *) &gt->command[i].Parametr[4].Value;
    y = (float *) &gt->command[i].Parametr[5].Value;

    *left = ftoi((*l) * xbmp);
    *top = ftoi((*t) * ybmp);
    *right = ftoi((*r) * xbmp);
    *bottom = ftoi((*b) * ybmp);
    *ycor = ftoi((*y) * ybmp);
  }

  return 1;
}

int fn_Create_TTable(int iMaxCharValue, GAME_TRIGER * gt,
  TRIGER_STRUCTURE * ts)
{
  int i;

  b2_2d_font.pTTable = (int *) malloc((iMaxCharValue + 1) * sizeof(int));

  b2_2d_font.iMaxCharValue = iMaxCharValue;

  kprintf(1, "b2_2d_font.pTTable = %p", b2_2d_font.pTTable);

  if (!b2_2d_font.pTTable)
    return 0;

  for (i = 0; i < iMaxCharValue + 1; i++)
    b2_2d_font.pTTable[i] = -1;

  for (i = 0; i < gt->lastcommand; i++)
    switch (gt->command[i].iCommand) {
      case 1:
        if (gt->command[i].Parametr[0].Value < (unsigned) iMaxCharValue + 1)
          b2_2d_font.pTTable[(int) gt->command[i].Parametr[0].Value] = i;
        break;
    }

  return 1;
}

int fn_Set_Font_Params(GAME_TRIGER * gt, TRIGER_STRUCTURE * ts)
{
  int iMaxCharValue = 0;
  int i;

  for (i = 0; i < gt->lastcommand; i++)
    switch (gt->command[i].iCommand) {
      case 1:
        if ((int) gt->command[i].Parametr[0].Value > iMaxCharValue)
          iMaxCharValue = (int) gt->command[i].Parametr[0].Value;
        break;
      case 2:
        {
          if (gt->command[i].LastParam > 2 &&
            gt->command[i].Parametr[0].Type == 0)
            b2_2d_font.tcolor = RGB(gt->command[i].Parametr[0].Value,
              gt->command[i].Parametr[1].Value,
              gt->command[i].Parametr[2].Value);
        }
        break;
      case 3:
        {
          if (gt->command[i].LastParam > 0 &&
            gt->command[i].Parametr[0].Type == 0)
            b2_2d_font.iYPlus = gt->command[i].Parametr[0].Value;
        }
        break;
      case 6:
        {
          if (gt->command[i].LastParam > 0 &&
            gt->command[i].Parametr[0].Type == 0)
            b2_2d_font.iXPlus = gt->command[i].Parametr[0].Value;
        }
        break;
    }

  if (!fn_Create_TTable(iMaxCharValue, gt, ts))
    return 0;

  return 1;
}

int fn_Set_Font_Bmps(GAME_TRIGER * gt, TRIGER_STRUCTURE * ts)
{
  char text[256];
  int i;

  txt_trida(TEXT_MENU);

  for (i = 0; i < gt->lastcommand; i++)
    switch (gt->command[i].iCommand) {
      case 4:
        {
          if (gt->command[i].LastParam > 1 &&
            gt->command[i].Parametr[1].Type == 2) {
            memset(text, 0, 256);

            WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK | WC_DEFAULTCHAR,
              (wchar_t *) ts->StrTable[gt->command[i].Parametr[1].Value],
              wcslen((wchar_t *) ts->StrTable[gt->command[i].Parametr[1].
                  Value]), text, 256, NULL, NULL);

            b2_2d_font.iBitmap[gt->command[i].Parametr[0].Value] =
              ddxLoadBitmap(text, b2_2d_font.pArchive);
          }
        }
        break;
      case 5:
        {
          if (gt->command[i].LastParam > 1 &&
            gt->command[i].Parametr[1].Type == 2) {

            memset(text, 0, 256);

            WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK | WC_DEFAULTCHAR,
              (wchar_t *) ts->StrTable[gt->command[i].Parametr[1].Value],
              wcslen((wchar_t *) ts->StrTable[gt->command[i].Parametr[1].
                  Value]), text, 256, NULL, NULL);
            txt_nahraj_texturu_z_func(b2_2d_font.pArchive, text,
              &b2_2d_font.tex[gt->command[i].Parametr[0].Value], 1, 0,
              &b2_2d_font.konf[gt->command[i].Parametr[0].Value], bmp_nahraj);
          }
        }
        break;
    }

  return 0;
}

void fn_Draw_Message(int iSurface, int iXpos, int iYpos, GAME_TRIGER * gt,
  TRIGER_STRUCTURE * ts, WCHAR * cFile, WCHAR * cStop, int iSection,
  int *iXmax, int *iYmax)
{
  WCHAR wtext[1024];
  int i;
  int top, bottom, left, right, ycor;

  int x = iXpos, y = iYpos - b2_2d_font.iYPlus;

  *iXmax = x;
  *iYmax = y;

  i = wcslen(cFile);

  cFile = wcsstr((WCHAR *) b2_2d_font.pTBuffer, cFile);

  if (!cFile)
    return;

  cStop = wcsstr(cFile, cStop);

  if (!cStop)
    return;

  ZeroMemory(wtext, 1024 * sizeof(WCHAR));
  wcsncpy(wtext, cFile + i, cStop - cFile - i);

  for (i = 0; i < (int) wcslen(wtext); i++) {
    if (wtext[i] == 32) {
      //x+= 50;
      x += b2_2d_font.iXPlus;
      continue;
    }
    else if (wtext[i] == 10) {
      //y+= 17;
      y += b2_2d_font.iYPlus;
      x = iXpos;

      *iYmax += b2_2d_font.iYPlus;
      continue;
    }
    else
      if (fn_Find_Char(gt, ts, &top, &left, &bottom, &right, &ycor, wtext[i],
        (float) ddxGetWidth(b2_2d_font.iBitmap[iSection]),
        (float) ddxGetHight(b2_2d_font.iBitmap[iSection]))) {
      ddxTransparentBlt(iSurface, x, y + ycor, right - left + 1,
        bottom - top + 1, b2_2d_font.iBitmap[iSection], left, top,
        right - left + 1, bottom - top + 1, b2_2d_font.tcolor);

      x += right - left + 2;

      if (x > *iXmax)
        *iXmax = x;

      if (bottom - top + 1 > b2_2d_font.iYPlus)
        *iYmax = (*iYmax) - b2_2d_font.iYPlus + (bottom - top + 1);
    }
  }
}

void fn_Draw_MessageA(int iSurface, int iXpos, int iYpos, GAME_TRIGER * gt,
  TRIGER_STRUCTURE * ts, WCHAR * cText, int iSection, int *iXmax, int *iYmax)
{
  WCHAR wtext[1024];
  int i;
  int top, bottom, left, right, ycor;

  int x = iXpos, y = iYpos;

  *iXmax = x;
  *iYmax = y + b2_2d_font.iYPlus;

  ZeroMemory(wtext, 1024 * sizeof(WCHAR));
  wcsncpy(wtext, cText, wcslen(cText));

  for (i = 0; i < (int) wcslen(wtext); i++) {
    if (wtext[i] == 32) {
      //x+= 50;
      x += b2_2d_font.iXPlus;
      continue;
    }
    else if (wtext[i] == 10) {
      //y+= 17;
      y += b2_2d_font.iYPlus;
      x = iXpos;

      *iYmax += b2_2d_font.iYPlus;
      continue;
    }
    else
      if (fn_Find_Char(gt, ts, &top, &left, &bottom, &right, &ycor, wtext[i],
        (float) ddxGetWidth(b2_2d_font.iBitmap[iSection]),
        (float) ddxGetHight(b2_2d_font.iBitmap[iSection]))) {
      /*TransparentBltU(hdc, x, y + ycor, right - left + 1, bottom - top + 1, _2dd.bitmap[b2_2d_font.iBitmap[iSection]].bitmapDC, 
         left, top, right - left + 1, bottom - top + 1, b2_2d_font.tcolor); */

      ddxTransparentBlt(iSurface, x, y + ycor, right - left + 1,
        bottom - top + 1, b2_2d_font.iBitmap[iSection], left, top,
        right - left + 1, bottom - top + 1, b2_2d_font.tcolor);

      x += right - left + 2;

      if (x > *iXmax)
        *iXmax = x;

      if (bottom - top + 1 > b2_2d_font.iYPlus)
        *iYmax = (*iYmax) - b2_2d_font.iYPlus + (bottom - top + 1);
    }
  }
}

void fn_Set_Char(unsigned int *pTexture, int iXSize, int iYSize, int iXpos,
  int iYpos, unsigned int *pSource, int iXSSize, int iYSSize, int iCXSize,
  int iCYSize, int iXCpos, int iYCpos)
{
  unsigned int *pT =
    &pTexture[(iXSize * (iYSize - iYpos)) - (iXSize - iXpos)];
  unsigned int *pS =
    &pSource[(iXSSize * (iYSSize - iYCpos)) - (iXSSize - iXCpos)];

  int y;

  for (y = iYCpos; y <= iYCpos + iCYSize; y++) {
    memcpy((void *) pT, (void *) pS, iCXSize * sizeof(unsigned int));

    pT -= iXSize;
    pS -= iXSSize;
  }
}

void fn_Gen_Texture(char ** lpTexture, int iXSize, int iYSize, int iXpos,
  int iYpos, GAME_TRIGER * gt, TRIGER_STRUCTURE * ts, WCHAR * cFile,
  WCHAR * cStop, int iSection, int *iXres, int *iYres)
{
  WCHAR wtext[1024];
  int i;
  int top, bottom, left, right, ycor;
  char *pT = NULL;

  int x = iXpos, y = iYpos - b2_2d_font.iYPlus;

  *iXres = iXpos;
  *iYres = iYpos;

  i = wcslen(cFile);

  cFile = wcsstr((WCHAR *) b2_2d_font.pTBuffer, cFile);

  if (!cFile)
    return;

  cStop = wcsstr(cFile, cStop);

  if (!cStop)
    return;

  ZeroMemory(wtext, 1024 * sizeof(WCHAR));
  wcsncpy(wtext, cFile + i, cStop - cFile - i);

  pT = (char *) malloc(iXSize * iYSize * sizeof(unsigned int));

  if (!pT)
    return;

  *lpTexture = pT;

  ZeroMemory(pT, iXSize * iYSize * sizeof(unsigned int));

  for (i = 0; i < (int) wcslen(wtext); i++) {
    if (wtext[i] == 32) {
      x += b2_2d_font.iXPlus;
      continue;
    }
    else if (wtext[i] == 13) {
      //y+= 17;
      y += b2_2d_font.iYPlus;
      // *iYres = y;
      x = iXpos;
      continue;
    }
    else
      if (fn_Find_Char(gt, ts, &top, &left, &bottom, &right, &ycor, wtext[i],
        (float) b2_2d_font.tex[iSection].p_bmp->x,
        (float) b2_2d_font.tex[iSection].p_bmp->y)) {
      if (b2_2d_font.tex[iSection].p_bmp) {
        fn_Set_Char((unsigned int *) pT, iXSize, iYSize, x, y + ycor,
          (unsigned int *) b2_2d_font.tex[iSection].p_bmp->data,
          b2_2d_font.tex[iSection].p_bmp->x,
          b2_2d_font.tex[iSection].p_bmp->y, right - left, bottom - top, left,
          top);
      }

      x += right - left + 2;

      if (x > *iXres)
        *iXres = x;

      if (y + ycor + (bottom - top) > *iYres)
        *iYres = y + ycor + (bottom - top);
    }
  }
}


int fn_Open_Archive(char *cFile, APAK_HANDLE ** pAHandle, char *cAppName,
  char *cKeyName)
{
  int e;
  char text[256];

  GetPrivateProfileString(cAppName, cKeyName, "c:\\", text, 256, ini_file);
  working_file_translate(text, 256);
  chdir((text));

  (*pAHandle) = apakopen(cFile, text, &e);

  if (!(*pAHandle)) {
    kprintf(1, "Unable to open archive %s", cFile);

    switch (e) {
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

    assert(0);
    abort();
  }

  achdir((*pAHandle), text);

  kprintf(1, "APAK: %s", cFile);
  kprintf(1, "Velikost AFAT: %.1fKB",
    (*pAHandle)->FileHeader.apuISizeofFAT / 1000.0f);
  kprintf(1, "Velikost Archivu: %.1fMB",
    (*pAHandle)->FileHeader.apuLSizeofPAK / 1000000.0f);
  kprintf(1, "Souboru: %d", (*pAHandle)->FileHeader.apuICountofFiles);
  kprintf(1, "Adresaru: %d", (*pAHandle)->FileHeader.apuICountofDirectiories);
  kprintf(1, "Uzlu: %d", (*pAHandle)->FileHeader.apuICountofNodes);

  return 1;
}

int fn_Load_Grammar(char *pFile, GRAMMAR * pGr)
{
  FILE *file;
  char text[256];

  file = aopen(b2_2d_font.pArchive, pFile, "rb");

  if (!file)
    return 0;

  pGr->LastMask = 0;

  while (!aeof(file)) {
    agets(text, 256, file);
    gr_Add_Mask(text, pGr);
    strcpy(text, "");
  }

  aclose(file);
  return 1;
}

char fn_Load_Triger(char *pFile, GAME_TRIGER * pTriger, GRAMMAR * pGr,
  TRIGER_STRUCTURE * pTStruct)
{
  FILE *file;
  WCHAR wtext[128];
  word wotext[128];

  pTriger->lastcommand = 0;

  file = aopen(b2_2d_font.pArchive, pFile, "rb");

  aunicode(file);

  if (!file)
    return 0;

  aseek(file, 2, SEEK_SET);

  while (!aeof(file)) {
    memset(wtext, 0, 128 * sizeof(WCHAR));
    if(agets((char *) wotext, 256, file)) {
      wchar_windows_to_linux(wotext, 128, wtext);
      trig_Parse_LineU(wtext, &pTriger->command[pTriger->lastcommand], pTriger,
        pGr, pTStruct);
    }
  }

  aclose(file);
  return 1;
}

int fn_Set_Font(char *cPAK)
{
  int i;

  memset(&b2_2d_font, 0, sizeof(B2_FONT));

  if (!fn_Open_Archive(cPAK, &b2_2d_font.pArchive, "game", "bitmap_dir"))
    return 0;

  b2_2d_font.file = aopen(b2_2d_font.pArchive, "texts.txt", "rb");

  if (!b2_2d_font.file) {
    apakclose(&b2_2d_font.pArchive);
    return 0;
  }
  else {
    agetbuffer(b2_2d_font.file, (char **) &b2_2d_font.pTBuffer, &b2_2d_font.iTSize);
    b2_2d_font.pTBuffer = wchar_windows_to_linux((word *) b2_2d_font.pTBuffer, b2_2d_font.iTSize);

    if (!b2_2d_font.pTBuffer) {
      apakclose(&b2_2d_font.pArchive);
      return 0;
    }
  }

  if (!fn_Load_Grammar("font_grammar.txt", &b2_2d_font.gr))
    return 0;

  b2_2d_font.ts.LastStr = 0;
  b2_2d_font.ts.sizeofT = 0;
  b2_2d_font.ts.pTriger = NULL;

  if (!fn_Load_Triger("font_def.txt", &b2_2d_font.gt, &b2_2d_font.gr,
      &b2_2d_font.ts))
    return 0;

  for (i = 0; i < FONT_MAX_BMP; i++)
    b2_2d_font.iBitmap[i] = -1;

  b2_2d_font.pTTable = NULL;

  if (!fn_Set_Font_Params(&b2_2d_font.gt, &b2_2d_font.ts))
    return 0;

  //kprintf(1, "set font = %s", cPAK);

  return 1;
}

int fn_Load_Bitmaps(void)
{
  if (!b2_2d_font.pArchive)
    return 0;

  kom_set_default_text_config(0, 0, 1, 0, 0, 1);
  fn_Set_Font_Bmps(&b2_2d_font.gt, &b2_2d_font.ts);
  kom_ret_default_text_config();

  return 1;
}

void fn_Release_Font(int bTextures)
{
  int i;

  aclose(b2_2d_font.file);
  apakclose(&b2_2d_font.pArchive);

  //if(_2dd.bitmap)
  if (bTextures)
    for (i = 0; i < FONT_MAX_BMP; i++)
      if (b2_2d_font.iBitmap[i] != -1) {
        ddxReleaseBitmap(b2_2d_font.iBitmap[i]);
      }

  if (bTextures)
    for (i = 0; i < FONT_MAX_BMP; i++)
      if (b2_2d_font.tex[i].load) {
        txt_zrus_texturu_ram(&b2_2d_font.tex[i]);
        b2_2d_font.tex[i].load = 0;
      }

  b2_2d_font.iTSize = 0;

  if (b2_2d_font.pTTable)
    free((void *) b2_2d_font.pTTable);

  b2_2d_font.pTTable = NULL;
}

void fn_Release_3d_Textures_Full(void)
{
  int i;

  for (i = 133 + MENU_ANIMATION_CORECTION; i < _3dd.last; i++) {
    txt_zrus_texturu(&_3dd.p_texture[i]);
    _3dd.p_texture[i].text = -1;
    free((void *) _3dd.p_sysramtexture[i].data);
  }

  _3dd.last = 133 + MENU_ANIMATION_CORECTION;
}

void fn_Release_3d_Textures(void)
{
  int i;

  for (i = 133 + MENU_ANIMATION_CORECTION; i < _3dd.last; i++) {
    txt_zrus_texturu(&_3dd.p_texture[i]);
    _3dd.p_texture[i].text = -1;
  }
}

void fn_Test(int hdc)
{
  int x, y;

  WCHAR wc[64];
  WCHAR ws[64];

  MultiByteToWideChar(CP_ACP, 0, "##message1", strlen("##message1") + 1, wc,
    sizeof(wc) / sizeof(wc[0]));
  MultiByteToWideChar(CP_ACP, 0, "##endofmessage",
    strlen("##endofmessage") + 1, ws, sizeof(ws) / sizeof(ws[0]));

  fn_Draw_Message(hdc, 360, 230, &b2_2d_font.gt, &b2_2d_font.ts, wc, ws, 0,
    &x, &y);
}

void fn_Test2(int hdc)
{
  int x, y;

  WCHAR wc[64];
  WCHAR ws[64];

  MultiByteToWideChar(CP_ACP, 0, "##settings", strlen("##settings") + 1, wc,
    sizeof(wc) / sizeof(wc[0]));
  MultiByteToWideChar(CP_ACP, 0, "##endofmessage",
    strlen("##endofmessage") + 1, ws, sizeof(ws) / sizeof(ws[0]));

  fn_Draw_Message(hdc, 800, 600, &b2_2d_font.gt, &b2_2d_font.ts, wc, ws, 0,
    &x, &y);
}

int fn_Text_Blt(int hdc, WCHAR * ws, WCHAR * wc, int iSurface, int iSection,
  int *i)
{
  int x, y;
  int tmpDC;

  fn_Draw_Message(iSurface, 0, 0, &b2_2d_font.gt, &b2_2d_font.ts, wc, ws,
    iSection, &x, &y);

  *i = ddxFindFreeSurface();

  tmpDC = ddxCreateSurface(x, y, *i);

  if (!tmpDC)
    return 0;
  else
    ddxTransparentBlt(tmpDC, 0, 0, x, y, iSurface, 0, 0, x, y, TRANSCOLOR);

  return 1;
}

int fn_Gen_Menu_Text(int iSection, int hdc, char *cText, int *i1, int *i2)
{
  WCHAR wc[64];
  WCHAR ws[64];

  int sidx = ddxFindFreeSurface();
  int fDC = ddxCreateSurface(1024, 200, sidx);

  if (!fDC)
    return -1;

  MultiByteToWideChar(CP_ACP, 0, cText, strlen(cText) + 1, wc,
    sizeof(wc) / sizeof(wc[0]));
  MultiByteToWideChar(CP_ACP, 0, "##endofmessage",
    strlen("##endofmessage") + 1, ws, sizeof(ws) / sizeof(ws[0]));

  if (!fn_Text_Blt(hdc, ws, wc, fDC, iSection, i1)) {
    ddxReleaseBitmap(sidx);
    return -1;
  }

  if (!fn_Text_Blt(hdc, ws, wc, fDC, iSection + 1, i2)) {
    ddxReleaseBitmap(sidx);
    return -1;
  }

  ddxReleaseBitmap(sidx);

  return sidx;
}

int fn_Gen_Menu_Texts(int iSection, int hdc)
{
  int x;
  WCHAR wc[64];
  WCHAR ws[64];
  
  int sidx = ddxFindFreeSurface();
  int fDC = ddxCreateSurface(1024, 200, sidx);

  if (!fDC)
    return 0;

  MultiByteToWideChar(CP_ACP, 0, "##menu_newgame",
    strlen("##menu_newgame") + 1, wc, sizeof(wc) / sizeof(wc[0]));
  MultiByteToWideChar(CP_ACP, 0, "##endofmessage",
    strlen("##endofmessage") + 1, ws, sizeof(ws) / sizeof(ws[0]));

  if (!fn_Text_Blt(hdc, ws, wc, fDC, 0, &x))
    return 0;

  if (!fn_Text_Blt(hdc, ws, wc, fDC, 1, &x))
    return 0;

  MultiByteToWideChar(CP_ACP, 0, "##menu_loadgame",
    strlen("##menu_loadgame") + 1, wc, sizeof(wc) / sizeof(wc[0]));
  if (!fn_Text_Blt(hdc, ws, wc, fDC, 0, &x))
    return 0;

  if (!fn_Text_Blt(hdc, ws, wc, fDC, 1, &x))
    return 0;

  MultiByteToWideChar(CP_ACP, 0, "##menu_settings",
    strlen("##menu_settings") + 1, wc, sizeof(wc) / sizeof(wc[0]));
  if (!fn_Text_Blt(hdc, ws, wc, fDC, 0, &x))
    return 0;

  if (!fn_Text_Blt(hdc, ws, wc, fDC, 1, &x))
    return 0;

  MultiByteToWideChar(CP_ACP, 0, "##menu_credits",
    strlen("##menu_credits") + 1, wc, sizeof(wc) / sizeof(wc[0]));
  if (!fn_Text_Blt(hdc, ws, wc, fDC, 0, &x))
    return 0;

  if (!fn_Text_Blt(hdc, ws, wc, fDC, 1, &x))
    return 0;
  
  ddxReleaseBitmap(sidx);
  return 1;
}

int fn_Gen_Menu(void)
{
  return fn_Gen_Menu_Texts(0, HDC2DD);
}

#define FONT_X_MAX 1024
#define FONT_Y_MAX 1024

int fn_Up(int iValue)
{
  int i, x = 4;

  for (i = 3; i < 12; i++) {
    x = x * 2;

    if (x > iValue)
      return x;
  }

  return FONT_X_MAX;
}

int fn_Blt(char *pT, char **pD, int ix, int iy)
{
  int y;
  unsigned int *puT = (unsigned int *) pT + (FONT_X_MAX * (FONT_Y_MAX - 1));
  unsigned int *puD;

  *pD = (char *) malloc(ix * iy * sizeof(unsigned int));

  if (!(*pD))
    return 0;

  memset((*pD), 0, ix * iy * sizeof(unsigned int));

  puD = (unsigned int *) (*pD) + (ix * (iy - 1));

  for (y = 0; y < iy; y++) {

    memcpy((void *) puD, (void *) puT, ix * sizeof(unsigned int));

    puT -= FONT_X_MAX;
    puD -= ix;
  }

  for (y = 0; y < FONT_X_MAX * FONT_Y_MAX; y++)
    if (pT[y])
      pT[y] = pT[y];

  free((void *) pT);

  return 1;
}

int fn_Put_in_3d_List(int text, EDIT_TEXT * p_tex, EDIT_TEXT_KONFIG * p_konf,
  int x, int y, char *pMem, int ox, int oy)
{
  memcpy(&_3dd.p_texture[_3dd.last], p_tex, sizeof(EDIT_TEXT));
  memcpy(&_3dd.p_sysramtexture[_3dd.last].konf, p_konf,
    sizeof(EDIT_TEXT_KONFIG));

  _3dd.p_texture[_3dd.last].text = text;

  _3dd.p_sysramtexture[_3dd.last].data = pMem;
  _3dd.p_sysramtexture[_3dd.last].x = x;
  _3dd.p_sysramtexture[_3dd.last].y = y;

  _3dd.p_sysramtexture[_3dd.last].tx = ox;
  _3dd.p_sysramtexture[_3dd.last].ty = oy;

  _3dd.p_sysramtexture[_3dd.last].bSLoaded = 0;
  _3dd.p_sysramtexture[_3dd.last].bVLoaded = 0;

  _3dd.last++;

  return _3dd.last - 1;
}

int fn_Get_Font_Texture(int iSection, char *cText)
{
  int tx, ty;
  int Xmax, Ymax;
  char *pT = NULL;
  char *pnT = NULL;

  WCHAR wc[64];
  WCHAR ws[64];

  MultiByteToWideChar(CP_ACP, 0, cText, strlen(cText) + 1, wc,
    sizeof(wc) / sizeof(wc[0]));
  MultiByteToWideChar(CP_ACP, 0, "##endofmessage",
    strlen("##endofmessage") + 1, ws, sizeof(ws) / sizeof(ws[0]));

  fn_Gen_Texture(&pT, FONT_X_MAX, FONT_Y_MAX, 0, 0, &b2_2d_font.gt,
                 &b2_2d_font.ts, wc, ws, iSection, &Xmax, &Ymax);

  tx = Xmax;
  ty = Ymax;
  Xmax = fn_Up(Xmax);
  Ymax = fn_Up(Ymax);

  fn_Blt(pT, &pnT, Xmax, Ymax);

  /*glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, b2_2d_font.tex.p_bmp->x, b2_2d_font.tex.p_bmp->y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
     b2_2d_font.tex.p_bmp->data); */

  return fn_Put_in_3d_List(b2_2d_font.tex[iSection].text,
    &b2_2d_font.tex[iSection], &b2_2d_font.konf[iSection], Xmax, Ymax, pnT,
    tx, ty);
}

void fn_Load_Textures_From_RAM(void)
{
  int i;

  for (i = 133 + MENU_ANIMATION_CORECTION; i < _3dd.last; i++) {
    glGenTextures(1, &_3dd.p_texture[i].text);
    glBindTexture(GL_TEXTURE_2D, _3dd.p_texture[i].text);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _3dd.p_sysramtexture[i].x,
      _3dd.p_sysramtexture[i].y, 0, GL_RGBA, GL_UNSIGNED_BYTE,
      _3dd.p_sysramtexture[i].data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }
}

void fn_Texture(int iSection)
{
  int i;

  for (i = 0; i <= iSection; i++)
    fn_Get_Font_Texture(i, "##restartlevel");

  for (i = 0; i <= iSection; i++)
    fn_Get_Font_Texture(i, "##mainmenu");

  for (i = 0; i <= iSection; i++)
    fn_Get_Font_Texture(i, "##settings");

  for (i = 0; i <= iSection; i++)
    fn_Get_Font_Texture(i, "##recorddemo");

  for (i = 0; i <= iSection; i++)
    fn_Get_Font_Texture(i, "##back");

  for (i = 0; i <= iSection; i++)
    fn_Get_Font_Texture(i, "##menu_loadgame");

  for (i = 0; i <= iSection; i++)
    fn_Get_Font_Texture(i, "##menu_savegame");
}

//int fn_Find_Char(GAME_TRIGER *gt, TRIGER_STRUCTURE *ts, int *top, int *left, int *bottom, int *right, int *ycor, WCHAR cWChar)
void fn_Convert_Rect(char *cFile, int xmax, int ymax)
{
  FILE *f;
  FILE *fi;
  GAME_TRIGER *gt = &b2_2d_font.gt;
  //TRIGER_STRUCTURE *ts = &b2_2d_font.ts;
  int i;
  char text[256];
  char textt[256];
  char t[32];
  float *fl1, *fl2, *fl3, *fl4, *fl5;

  getcwd(text, 255);

  f = fopen("fontout.txt", "w");
  fi = fopen("font_def.txt", "r");

  for (i = 0; i < gt->lastcommand; i++)
    switch (gt->command[i].iCommand) {
      case 1:
        {
          if (gt->command[i].LastParam > 5 &&
            gt->command[i].Parametr[0].Type == 3) {
            fgets(textt, 256, fi);
            strncpy(t, textt, 10);
            t[10] = '\0';
            strcpy(text, "");

            fl1 = (float *) &gt->command[i].Parametr[1].Value;
            fl2 = (float *) &gt->command[i].Parametr[2].Value;
            fl3 = (float *) &gt->command[i].Parametr[3].Value;
            fl4 = (float *) &gt->command[i].Parametr[4].Value;
            fl5 = (float *) &gt->command[i].Parametr[5].Value;

            *fl2 = ((*fl2) * 576) / 900.0f;
            *fl4 = ((*fl4) * 576) / 900.0f;
            *fl5 = ((*fl5) * 576) / 900.0f;


            sprintf(text, "%s %f, %f, %f, %f, %f);\n", t,
              (*fl1), (*fl2), (*fl3), (*fl4), (*fl5));

            fputs(text, f);
          }
        }
        break;
    }

  fclose(fi);
  fclose(f);
  return;
}

void fn_Draw_Line(int x1, int y1, int x2, int y2, COLORREF color, HDC hdc)
{
/*
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

void fn_Draw_Draw_Frames(void)
{
/*
	float xbmp = 1024.0f;
	float ybmp = 768.0f;
	GAME_TRIGER *gt = &b2_2d_font.gt;
	TRIGER_STRUCTURE *ts = &b2_2d_font.ts;
	int left, top, bottom, right, ycor;
	float *l, *t, *r, *b, *y;
	int i;
	
	BitBlt(_2dd.hDC,0,0,1024,768,_2dd.bitmap[b2_2d_font.iBitmap[0]].bitmapDC,0,0,SRCCOPY);

	for(i=0;i<gt->lastcommand;i++)
		switch(gt->command[i].iCommand)
		{
			case 1:
				{
					if(gt->command[i].LastParam > 5 &&
					   gt->command[i].Parametr[0].Type == 3)
					{
							l = (float *)&gt->command[i].Parametr[1].Value;
							t = (float *)&gt->command[i].Parametr[2].Value;
							r = (float *)&gt->command[i].Parametr[3].Value;
							b = (float *)&gt->command[i].Parametr[4].Value;
							y = (float *)&gt->command[i].Parametr[5].Value;

							left = ftoi((*l) * xbmp);
							top = ftoi((*t) * ybmp);
							right = ftoi((*r) * xbmp);
							bottom = ftoi((*b) * ybmp);
							ycor = ftoi((*y) * ybmp);

						fn_Draw_Line(left-1, top-1, right+1, top-1, RGB(255, 0, 0), _2dd.hDC);
						fn_Draw_Line(right+1, top-1, right+1, bottom+1, RGB(255, 0, 0), _2dd.hDC);
						fn_Draw_Line(right+1, bottom+1, left-1, bottom+1, RGB(255, 0, 0), _2dd.hDC);
						fn_Draw_Line(left-1, bottom+1, left-1, top-1, RGB(255, 0, 0), _2dd.hDC);
					}
				}
				break;
		}
*/
  return;
}
/*
int fn_DC2Tex(HDC hdc, int xr, int yr, int turn, int texture)
{

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
/*  
}
*/
