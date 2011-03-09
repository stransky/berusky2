//------------------------------------------------------------------------------------------------
// 1.0.0
//------------------------------------------------------------------------------------------------

#ifndef __FONT_
#define __FONT_

#include <windows.h>
#include <stdio.h>

#include "trigers.h"
#include "apak.h"
#include "3d_graphic.h"

#include "..\komat\3d_all.h"
#include "..\Komat\Berusky_universal.h"

#define	FONT_MAX_BMP	16

typedef struct _B2_FONT
{
	int					iBitmap[FONT_MAX_BMP];
	APAK_HANDLE			*pArchive;
	GRAMMAR				gr;
	TRIGER_STRUCTURE	ts;
	GAME_TRIGER			gt;
	EDIT_TEXT			tex[FONT_MAX_BMP];
	EDIT_TEXT_KONFIG	konf[FONT_MAX_BMP];
	WCHAR				*pTBuffer;
	int					iTSize;
	FILE				*file;
	COLORREF			tcolor;
	int					iYPlus;
	int					iXPlus;
	int					*pTTable;
	int					iMaxCharValue;
} B2_FONT;

	int fn2_Set_Font(char *cPAK);
	void fn2_Release_Font(void);
	void fn2_Release_3d_Textures_Full(void);
	void fn2_Release_3d_Textures(void);
	void fn2_Release_3d_Textures_RAM(void);
	void fn2_Load_Textures_From_RAM(void);
	int fn2_Load_Bitmaps(void);
	void fn2_Test(int hdc);
	void fn2_Test2(int hdc);
	void fn2_Texture(int iSection);
	HDC fn2_CreateDC(HDC hdc, int x, int y, int _2dd_idx);
	int fn2_Gen_Menu(void);
	int fn2_Gen_Menu_Text(int iSection, int hdc, char *cText, int *i1, int *i2);

	void fn2_Convert_Rect(char *cFile, int xmax, int ymax);
	void fn2_Draw_Draw_Frames(void);
	void fn2_Draw_Message(int iSurface, int iXpos, int iYpos, GAME_TRIGER *gt, TRIGER_STRUCTURE *ts, WCHAR *cFile, 
						WCHAR *cStop, int iSection, int *iXmax, int *iYmax);

	void fn2_Draw_MessageA(int iSurface, int iXpos, int iYpos, GAME_TRIGER *gt, TRIGER_STRUCTURE *ts, WCHAR *cText, 
					 int iSection, int *iXmax, int *iYmax);

	void fn2_Draw_MessageRECT(int iSurface, int iXpos, int iYpos, GAME_TRIGER *gt, TRIGER_STRUCTURE *ts, WCHAR *cFile, 
					      WCHAR *cStop, int iSection, int *iXmax, int *iYmax, RECT *pR);

	int fn2_DC2Tex(HDC hdc, int xr, int yr, int turn, int texture);

#endif