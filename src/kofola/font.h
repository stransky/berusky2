//------------------------------------------------------------------------------------------------
// 1.0.0
//------------------------------------------------------------------------------------------------

#ifndef __FONT_
#define __FONT_

#include <stdio.h>

#include "trigers.h"
#include "Apak.h"
#include "3D_graphic.h"

#include "3d_all.h"
#include "Berusky_universal.h"

#define	FONT_MAX_BMP	16

typedef struct _B2_FONT
{
  int iBitmap[FONT_MAX_BMP];
  APAK_HANDLE *pArchive;
  GRAMMAR gr;
  TRIGER_STRUCTURE ts;
  GAME_TRIGER gt;
  EDIT_TEXT tex[FONT_MAX_BMP];
  EDIT_TEXT_KONFIG konf[FONT_MAX_BMP];
  WCHAR *pTBuffer;
  apuInt iTSize;
  FILE *file;
  COLORREF tcolor;
  int iYPlus;
  int iXPlus;
  int *pTTable;
  int iMaxCharValue;
} B2_FONT;

int fn_Set_Font(char *cPAK);
void fn_Release_Font(int bTextures);
void fn_Release_3d_Textures_Full(void);
void fn_Release_3d_Textures(void);
void fn_Load_Textures_From_RAM(void);
int fn_Load_Bitmaps(void);
void fn_Test(int hdc);
void fn_Test2(int hdc);
void fn_Texture(int iSection);

        //HDC fn_CreateDC(HDC hdc, int x, int y, int _2dd_idx);
int fn_Gen_Menu(void);
int fn_Gen_Menu_Text(int iSection, int hdc, char *cText, int *i1, int *i2);

void fn_Convert_Rect(char *cFile, int xmax, int ymax);
void fn_Draw_Draw_Frames(void);
void fn_Draw_Message(int iSurface, int iXpos, int iYpos, GAME_TRIGER * gt,
  TRIGER_STRUCTURE * ts, WCHAR * cFile, WCHAR * cStop, int iSection,
  int *iXmax, int *iYmax);

void fn_Draw_MessageA(int iSurface, int iXpos, int iYpos, GAME_TRIGER * gt,
  TRIGER_STRUCTURE * ts, WCHAR * cText, int iSection, int *iXmax, int *iYmax);

        //int fn_DC2Tex(HDC hdc, int xr, int yr, int turn, int texture);

int fn_Get_Font_Texture(int iSection, char *cText);

#endif
