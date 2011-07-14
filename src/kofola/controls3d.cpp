#include <wchar.h>
#include <stdio.h>
#include "3d_all.h"
#include "Berusky3d_kofola_interface.h"
#include "Berusky3d_kofola2d.h"
#include "controls3d.h"
#include "Apak.h"
#include "2D_graphic.h"
#include "font3d.h"
#include "menu_def.h"
//#include "2ddx.h"


extern B2_FONT b2_3d_font;
extern APAK_HANDLE *pControlsArchive;
extern HDC BackDC;
static char bBlockList = 0;
static char bExclusive = 0;
extern RECT_LINE rline;

#define HDC2DD		-1

static HDC_EDIT_CONTROL2 hdcED;
static HDC_COMBO_CONTROL2 hdcCO;
static HDC_LIST_CONTROL2 hdcLI;
static HDC_BUTTON_CONTROL2 hdcBU;
static HDC_CHECKBOX_CONTROL2 hdcCH;
static HDC_PROGRES_CONTROL2 hdcPR;
static HDC_FRAME_CONTROL2 hdcFR;

void co2_Del_Combo_List(COMBO_CONTROL2 * p_co, int hdc, int xcor, int ycor);
int co_Handle_Edit_Key_Filter(void);

//WCHAR co_ToUnicode(WPARAM wScanCode);
WCHAR co_ToUnicode(int wScanCode);

void co2_Draw_Disable(int hdc, int xcor, int ycor, int x, int y, int dx,
  int dy)
{
  ddx2TransparentBlt(hdc, x + xcor, y + ycor, dx, dy, hdcCH.hdcGray, 0, 0, dx,
    dy, TRANSCOLOR);
}

HDC co2_CreateDC(HDC hdc, int x, int y, HDC_INFO2 * pdcinfo)
{
/*
	HDC				hdcBack = NULL;
	HBITMAP			hback_bitmap = NULL;
	BITMAP			back_bitmap;

	if(!hdc)
		return NULL;
	
	hdcBack = CreateCompatibleDC(hdc);
	if(!hdcBack)
		return 0;

	hback_bitmap = CreateCompatibleBitmap(hdc, x, y);
	if(!hback_bitmap)
		return 0;

	GetObject(hback_bitmap, sizeof(back_bitmap), &back_bitmap);
	pdcinfo->hbitmapold = SelectObject(hdcBack, hback_bitmap);

	DeleteObject(hback_bitmap);

	//SetStretchBltMode(hdcBack, HALFTONE);
	//SetBrushOrgEx(hdcBack,0,0,NULL);

	pdcinfo->hdc = hdcBack;

	pdcinfo->x = x;
	pdcinfo->y = y;
	pdcinfo->hbitmap = hback_bitmap;

	BitBlt(hdcBack,0,0,x,y,NULL,0,0,BLACKNESS);

	return hdcBack;
*/
}

int co2_Release_Bitmap(HDC_INFO2 * pdcinfo)
{
  if (!pdcinfo)
    return 1;
/*
	SelectObject(pdcinfo->hdc,pdcinfo->hbitmapold);	
    DeleteObject(pdcinfo->hbitmap); 
	DeleteDC(pdcinfo->hdc);
*/
  return 1;
}

int co2_Load_Graphic(int combo_var)
{
  pControlsArchive->pActualNode = pControlsArchive->pRootNode->pNextNode;

  if (!combo_var) {
    hdcCO.hdcCombo = ddx2LoadBitmap("combo.bmp", pControlsArchive);
    hdcCO.hdcComboEnd = ddx2LoadBitmap("combo_end.bmp", pControlsArchive);
    hdcCO.hdcComboMid = ddx2LoadBitmap("combo_mid.bmp", pControlsArchive);
  }
  else {
    hdcCO.hdcCombo = ddx2LoadBitmap("combo_l.bmp", pControlsArchive);
    hdcCO.hdcComboEnd = ddx2LoadBitmap("combo_end_l.bmp", pControlsArchive);
    hdcCO.hdcComboMid = ddx2LoadBitmap("combo_mid_l.bmp", pControlsArchive);
  }

  hdcCO.hdcComboUp = ddx2LoadBitmap("combo_up.bmp", pControlsArchive);
  hdcCO.hdcComboDown = ddx2LoadBitmap("combo_down.bmp", pControlsArchive);
  hdcCO.hdcComboDrop = ddx2LoadBitmap("combo_drop.bmp", pControlsArchive);
  hdcCO.hdcComboMover = ddx2LoadBitmap("combo_mover.bmp", pControlsArchive);
  hdcCO.hdcComboMoverb = ddx2LoadBitmap("combo_moverb.bmp", pControlsArchive);

  hdcED.hdcEdit = ddx2LoadBitmap("edit.bmp", pControlsArchive);

  hdcLI.hdcListDown = ddx2LoadBitmap("list_down.bmp", pControlsArchive);
  hdcLI.hdcListMover = ddx2LoadBitmap("list_mover.bmp", pControlsArchive);
  hdcLI.hdcListUp = ddx2LoadBitmap("list_up.bmp", pControlsArchive);
  hdcLI.hdcListTop = ddx2LoadBitmap("list_top.bmp", pControlsArchive);
  hdcLI.hdcListBottom = ddx2LoadBitmap("list_bottom.bmp", pControlsArchive);
  hdcLI.hdcListLeft = ddx2LoadBitmap("list_left.bmp", pControlsArchive);
  hdcLI.hdcListRight = ddx2LoadBitmap("list_right.bmp", pControlsArchive);
  hdcLI.hdcListTL = ddx2LoadBitmap("list_tl.bmp", pControlsArchive);
  hdcLI.hdcListTR = ddx2LoadBitmap("list_tr.bmp", pControlsArchive);
  hdcLI.hdcListBL = ddx2LoadBitmap("list_bl.bmp", pControlsArchive);
  hdcLI.hdcListBR = ddx2LoadBitmap("list_br.bmp", pControlsArchive);

  hdcBU.hdcButtonL = ddx2LoadBitmap("button_l.bmp", pControlsArchive);
  hdcBU.hdcButtonS = ddx2LoadBitmap("button_s.bmp", pControlsArchive);
  hdcCH.hdcCheck = ddx2LoadBitmap("checkbox.bmp", pControlsArchive);
  hdcCH.hdcGray = ddx2LoadBitmap("gray.bmp", pControlsArchive);
  hdcPR.hdcLine = ddx2LoadBitmap("progres_line.bmp", pControlsArchive);
  hdcPR.hdcMover = ddx2LoadBitmap("progres_mover.bmp", pControlsArchive);
  hdcFR.hdcFrame = ddx2LoadBitmap("frame.bmp", pControlsArchive);

  return 1;
}

int co2_Release_Graphic(void)
{
  ddx2ReleaseBitmap(hdcCO.hdcCombo);
  ddx2ReleaseBitmap(hdcCO.hdcComboEnd);
  ddx2ReleaseBitmap(hdcCO.hdcComboMid);
  ddx2ReleaseBitmap(hdcCO.hdcComboDown);
  ddx2ReleaseBitmap(hdcCO.hdcComboUp);
  ddx2ReleaseBitmap(hdcCO.hdcComboDrop);
  ddx2ReleaseBitmap(hdcCO.hdcComboMover);
  ddx2ReleaseBitmap(hdcCO.hdcComboMoverb);
  ddx2ReleaseBitmap(hdcED.hdcEdit);
  ddx2ReleaseBitmap(hdcLI.hdcListDown);
  ddx2ReleaseBitmap(hdcLI.hdcListMover);
  ddx2ReleaseBitmap(hdcLI.hdcListUp);
  ddx2ReleaseBitmap(hdcLI.hdcListTop);
  ddx2ReleaseBitmap(hdcLI.hdcListBottom);
  ddx2ReleaseBitmap(hdcLI.hdcListLeft);
  ddx2ReleaseBitmap(hdcLI.hdcListRight);
  ddx2ReleaseBitmap(hdcLI.hdcListTL);
  ddx2ReleaseBitmap(hdcLI.hdcListTR);
  ddx2ReleaseBitmap(hdcLI.hdcListBL);
  ddx2ReleaseBitmap(hdcLI.hdcListBR);
  ddx2ReleaseBitmap(hdcBU.hdcButtonL);
  ddx2ReleaseBitmap(hdcBU.hdcButtonS);
  ddx2ReleaseBitmap(hdcCH.hdcCheck);
  ddx2ReleaseBitmap(hdcPR.hdcLine);
  ddx2ReleaseBitmap(hdcPR.hdcMover);
  ddx2ReleaseBitmap(hdcFR.hdcFrame);
  ddx2ReleaseBitmap(hdcCH.hdcGray);

  return 1;
}

void co2_Combo_Draw(int hdc, COMBO_CONTROL2 * p_co, int xcor, int ycor)
{
  int c = 0;
  int x = p_co->x;
  int y = p_co->y;
  int width = p_co->Width;
  int hight = p_co->Hight;
  int maxlisthight = p_co->ListMaxHight;

  int ax = p_co->x, ay = p_co->y, ly;

  //------------------------------------------------------- EDIT BOX ----------------------------------------------
  if (p_co->bEdit) {
    /*BitBlt(hdc, ax, ay, _2dd.bitmap[hdcCO.hdcCombo].bitmap.bmWidth, _2dd.bitmap[hdcCO.hdcCombo].bitmap.bmHeight,
       _2dd.bitmap[hdcCO.hdcCombo].bitmapDC, 0, 0, SRCCOPY); */

    ddx2BitBlt(hdc, ax, ay, ddx2GetWidth(hdcCO.hdcCombo),
      ddx2GetHeight(hdcCO.hdcCombo), hdcCO.hdcCombo, 0, 0);
  }

  p_co->coEditRect.top = p_co->y;
  p_co->coEditRect.bottom = p_co->y + ddx2GetHeight(hdcCO.hdcCombo);
  p_co->coEditRect.left = p_co->x;
  p_co->coEditRect.right = p_co->x + ddx2GetWidth(hdcCO.hdcCombo);

  p_co->WidthR = ddx2GetWidth(hdcCO.hdcCombo);

  //------------------------------------------------------- EDIT BOX ----------------------------------------------

  //------------------------------------------------------- EDIT BOX DOWN BUTTON ----------------------------------
  /*p_co->coDownRect.top = p_co->coEditRect.top+5;
     p_co->coDownRect.bottom = p_co->coEditRect.bottom - 5;
     p_co->coDownRect.left = p_co->coEditRect.right - 28;
     p_co->coDownRect.right = p_co->coEditRect.right - 5; */

  p_co->coDownRect.top = p_co->y;
  p_co->coDownRect.bottom = p_co->y + ddx2GetHeight(hdcCO.hdcCombo);
  p_co->coDownRect.left = p_co->x;
  p_co->coDownRect.right = p_co->x + ddx2GetWidth(hdcCO.hdcCombo);
  //------------------------------------------------------- EDIT BOX DOWN BUTTON ----------------------------------

  ay += ddx2GetHeight(hdcCO.hdcCombo);
  ax = p_co->x;
  ly = ay;

  //------------------------------------------------------- EDIT BOX LIST BOX -------------------------------------
  while (ay < ly + maxlisthight) {
    if (p_co->bList) {
      /*BitBlt(hdc, ax + xcor, ay + ycor, _2dd.bitmap[hdcCO.hdcComboMid].bitmap.bmWidth, _2dd.bitmap[hdcCO.hdcComboMid].bitmap.bmHeight,
         _2dd.bitmap[hdcCO.hdcComboMid].bitmapDC, 0, 0, SRCCOPY); */
      ddx2BitBlt(hdc, ax + xcor, ay + ycor, ddx2GetWidth(hdcCO.hdcComboMid),
        ddx2GetHeight(hdcCO.hdcComboMid), hdcCO.hdcComboMid, 0, 0);

      if (p_co->pItem) {
        /*TransparentBltU(hdc, ax + xcor + 5, ay + ycor, p_co->pItem[c].Norm.x, p_co->pItem[c].Norm.y,
           p_co->pItem[c].Norm.hdc, 0, 0, p_co->pItem[c].Norm.x, p_co->pItem[c].Norm.y, RGB(255, 0, 255)); */
        ddx2TransparentBlt(hdc, ax + xcor + 5, ay + ycor + 3,
          ddx2GetWidth(p_co->pItem[c].Norm),
          ddx2GetHeight(p_co->pItem[c].Norm), p_co->pItem[c].Norm, 0, 0,
          ddx2GetWidth(p_co->pItem[c].Norm),
          ddx2GetHeight(p_co->pItem[c].Norm), TRANSCOLOR);
      }
    }

    c++;

    ay += ddx2GetHeight(hdcCO.hdcComboMid);
  }

  if (p_co->bList) {
    /*BitBlt(hdc, ax + xcor, ay + ycor, _2dd.bitmap[hdcCO.hdcComboEnd].bitmap.bmWidth, _2dd.bitmap[hdcCO.hdcComboEnd].bitmap.bmHeight,
       _2dd.bitmap[hdcCO.hdcComboEnd].bitmapDC, 0, 0, SRCCOPY); */
    ddx2BitBlt(hdc, ax + xcor, ay + ycor, ddx2GetWidth(hdcCO.hdcComboEnd),
      ddx2GetHeight(hdcCO.hdcComboEnd), hdcCO.hdcComboEnd, 0, 0);
  }

  p_co->ListMaxHightR = ay + ddx2GetHeight(hdcCO.hdcComboEnd) - ly;

  if (p_co->CounfOfItems > p_co->CounfOfItemsL) {
    /*BitBlt(hdc, x + _2dd.bitmap[hdcCO.hdcCombo].bitmap.bmWidth + xcor - 4 - 
       _2dd.bitmap[hdcCO.hdcComboUp].bitmap.bmWidth, 
       y + ycor + _2dd.bitmap[hdcCO.hdcCombo].bitmap.bmHeight, 
       _2dd.bitmap[hdcCO.hdcComboUp].bitmap.bmWidth, 
       _2dd.bitmap[hdcCO.hdcComboUp].bitmap.bmHeight,
       _2dd.bitmap[hdcCO.hdcComboUp].bitmapDC, 0, 0, SRCCOPY); */

    /*BitBlt(hdc, x + _2dd.bitmap[hdcCO.hdcCombo].bitmap.bmWidth + xcor - 4 - 
       _2dd.bitmap[hdcCO.hdcComboDown].bitmap.bmWidth, 
       ay + _2dd.bitmap[hdcCO.hdcComboEnd].bitmap.bmHeight + ycor - 
       _2dd.bitmap[hdcCO.hdcComboDown].bitmap.bmHeight - 4, 
       _2dd.bitmap[hdcCO.hdcComboDown].bitmap.bmWidth, 
       _2dd.bitmap[hdcCO.hdcComboDown].bitmap.bmHeight,
       _2dd.bitmap[hdcCO.hdcComboDown].bitmapDC, 0, 0, SRCCOPY); */

    ddx2BitBlt(hdc, x + ddx2GetWidth(hdcCO.hdcCombo) + xcor - 4 -
      ddx2GetWidth(hdcCO.hdcComboUp),
      y + ycor + ddx2GetHeight(hdcCO.hdcCombo),
      ddx2GetWidth(hdcCO.hdcComboUp),
      ddx2GetHeight(hdcCO.hdcComboUp), hdcCO.hdcComboUp, 0, 0);

    ddx2BitBlt(hdc, x + ddx2GetWidth(hdcCO.hdcCombo) + xcor - 4 -
      ddx2GetWidth(hdcCO.hdcComboDown),
      ay + ddx2GetHeight(hdcCO.hdcComboEnd) + ycor -
      ddx2GetHeight(hdcCO.hdcComboDown) - 4,
      ddx2GetWidth(hdcCO.hdcComboDown),
      ddx2GetHeight(hdcCO.hdcComboDown), hdcCO.hdcComboDown, 0, 0);

    p_co->coLDown.top = ay + ddx2GetHeight(hdcCO.hdcComboEnd) + ycor -
      ddx2GetHeight(hdcCO.hdcComboDown) - 4;
    p_co->coLDown.bottom =
      p_co->coLDown.top + ddx2GetHeight(hdcCO.hdcComboDown);
    p_co->coLDown.left =
      x + ddx2GetWidth(hdcCO.hdcCombo) + xcor - 4 -
      ddx2GetWidth(hdcCO.hdcComboDown);
    p_co->coLDown.right =
      p_co->coLDown.left + ddx2GetWidth(hdcCO.hdcComboDown);

    p_co->coLUp.top = y + ycor + ddx2GetHeight(hdcCO.hdcCombo);
    p_co->coLUp.bottom = p_co->coLUp.top + ddx2GetHeight(hdcCO.hdcComboUp);
    p_co->coLUp.left = x + ddx2GetWidth(hdcCO.hdcCombo) + xcor - 4 -
      ddx2GetWidth(hdcCO.hdcComboDown);
    p_co->coLUp.right = p_co->coLUp.left + ddx2GetWidth(hdcCO.hdcComboUp);

    p_co->coLMov.top = p_co->coLUp.bottom + 1;
    p_co->coLMov.bottom = p_co->coLDown.top + 1;
    p_co->coLMov.left = p_co->coLUp.left;
    p_co->coLMov.right =
      p_co->coLMov.left + ddx2GetWidth(hdcCO.hdcComboMover);

    ddx2BitBlt(hdc, p_co->coLMov.left, p_co->coLMov.top,
      ddx2GetWidth(hdcCO.hdcComboMover), ddx2GetHeight(hdcCO.hdcComboMover),
      hdcCO.hdcComboMover, 0, 0);

    p_co->xmstart = p_co->coLMov.top;
    p_co->xm =
      p_co->coLDown.top - p_co->coLMov.top -
      ddx2GetHeight(hdcCO.hdcComboMover);
    p_co->xmp =
      p_co->coLMov.top +
      (int) floor(ddx2GetHeight(hdcCO.hdcComboMover) / 2.0f);
    p_co->xmstart += (int) floor(ddx2GetHeight(hdcCO.hdcComboMover) / 2.0f);

    p_co->coLMovA.top = p_co->coLUp.bottom + 1;
    p_co->coLMovA.bottom = p_co->coLDown.top - 1;
    p_co->coLMovA.left = p_co->coLMov.left;
    p_co->coLMovA.right = p_co->coLMov.right;
  }
  else {
    p_co->coLDown.top = -1;
    p_co->coLDown.bottom = -1;
    p_co->coLDown.left = -1;
    p_co->coLDown.right = -1;

    p_co->coLUp.top = -1;
    p_co->coLUp.bottom = -1;
    p_co->coLUp.left = -1;
    p_co->coLUp.right = -1;

    p_co->coLMov.top = -1;
    p_co->coLMov.bottom = -1;
    p_co->coLMov.left = -1;
    p_co->coLMov.right = -1;

    p_co->coLMovA.top = -1;
    p_co->coLMovA.bottom = -1;
    p_co->coLMovA.left = -1;
    p_co->coLMovA.right = -1;
  }

  //------------------------------------------------------- EDIT BOX LIST BOX -------------------------------------
}

void co2_Combo_Draw_List(int hdc, COMBO_CONTROL2 * p_co, int xcor, int ycor,
  char bDel)
{
  int c = 0 + p_co->SSelected;
  int maxlisthight = p_co->ListMaxHight;

  int ax = p_co->x, ay = p_co->y, ly;

  if (p_co->CSelected == p_co->Selected)
    return;

  ay += ddx2GetHeight(hdcCO.hdcCombo);
  ly = ay;

  if (bDel)
    co2_Del_Combo_List(p_co, hdc, xcor, ycor);

  while (ay < ly + maxlisthight) {
    if (p_co->bList) {
      if (c == p_co->CSelected && c != p_co->Selected) {
        if (p_co->pItem) {
          /*TransparentBltU(hdc, ax + xcor + 5, ay + ycor, p_co->pItem[c].Sel.x, p_co->pItem[c].Sel.y,
             p_co->pItem[c].Sel.hdc, 0, 0, p_co->pItem[c].Sel.x, p_co->pItem[c].Sel.y, RGB(255, 0, 255)); */

          ddx2TransparentBlt(hdc, ax + xcor + 5, ay + ycor + 3,
            ddx2GetWidth(p_co->pItem[c].Sel),
            ddx2GetHeight(p_co->pItem[c].Sel), p_co->pItem[c].Sel, 0, 0,
            ddx2GetWidth(p_co->pItem[c].Sel),
            ddx2GetHeight(p_co->pItem[c].Sel), TRANSCOLOR);
        }
      }
      else {
        if (p_co->pItem) {
          /*TransparentBltU(hdc, ax + xcor + 5, ay + ycor, p_co->pItem[c].Norm.x, p_co->pItem[c].Norm.y,
             p_co->pItem[c].Norm.hdc, 0, 0, p_co->pItem[c].Norm.x, p_co->pItem[c].Norm.y, RGB(255, 0, 255)); */

          ddx2TransparentBlt(hdc, ax + xcor + 5, ay + ycor + 3,
            ddx2GetWidth(p_co->pItem[c].Norm),
            ddx2GetHeight(p_co->pItem[c].Norm), p_co->pItem[c].Norm, 0, 0,
            ddx2GetWidth(p_co->pItem[c].Norm),
            ddx2GetHeight(p_co->pItem[c].Norm), TRANSCOLOR);

        }
      }
    }

    c++;

    ay += ddx2GetHeight(hdcCO.hdcComboMid);
  }
}

int co2_Combo_Hit2Sel(COMBO_CONTROL2 * p_co, int x, int y)
{
  int ay = p_co->y + ddx2GetHeight(hdcCO.hdcCombo);

  return (int) floor((y - ay) / (float) ddx2GetHeight(hdcCO.hdcComboMid));
}

void co2_Release_Edit(CONTROL_EDIT2 * p_ed)
{
  ddx2ReleaseBitmap(p_ed->bDC);

  free((void *) p_ed);
}

void co2_Release_Combo(COMBO_CONTROL2 * p_co)
{
  int i;

  bBlockList = 0;

  if (p_co->pItem) {
    for (i = 0; i < p_co->CounfOfItems; i++) {
      //co2_Release_Bitmap(&p_co->pItem[i].Norm);
      ddx2ReleaseBitmap(p_co->pItem[i].Norm);
      //co2_Release_Bitmap(&p_co->pItem[i].Sel);
      ddx2ReleaseBitmap(p_co->pItem[i].Sel);
    }

    free((void *) p_co->pItem);
  }

  if (p_co->pBDC) {
    //co2_Release_Bitmap(p_co->pBDC);
    ddx2ReleaseBitmap(*p_co->pBDC);
    free((void *) p_co->pBDC);
  }

  free((void *) p_co);
}

COMBO_CONTROL2 *co2_Create_Combo(int hdc, int x, int y, int maxlisthight,
  int id)
{
  COMBO_CONTROL2 *p_co = NULL;

  p_co = (COMBO_CONTROL2 *) malloc(sizeof(COMBO_CONTROL2));

  if (!p_co)
    return 0;

  ZeroMemory(p_co, sizeof(COMBO_CONTROL2));

  p_co->comboID = id;
  p_co->x = x;
  p_co->y = y;
  p_co->Hight = ddx2GetWidth(hdcCO.hdcCombo);
  p_co->Width = ddx2GetHeight(hdcCO.hdcCombo);
  p_co->ListMaxHight = maxlisthight;
  p_co->bEdit = 1;
  p_co->bList = 0;
  p_co->pBDC = NULL;
  p_co->Selected = -1;
  p_co->CSelected = -1;
  p_co->SSelected = 0;
  p_co->pItem = NULL;
  p_co->CounfOfItems = 0;
  p_co->xm = 0;
  p_co->xmstart = 0;
  p_co->bIn = 0;
  p_co->OSelected = -1;

  co2_Combo_Draw(hdc, p_co, 0, 0);

  return p_co;
}

void co2_Release_Combo_Drop(COMBO_DROP_CONTROL2 * p_co)
{
  int i;

  bBlockList = 0;

  if (p_co->pItem) {
    for (i = 0; i < p_co->CounfOfItems; i++) {
      //co_Release_Bitmap(&p_co->pItem[i].Norm);
      ddx2ReleaseBitmap(p_co->pItem[i].Norm);
      //co_Release_Bitmap(&p_co->pItem[i].Sel);
      ddx2ReleaseBitmap(p_co->pItem[i].Sel);
    }

    free((void *) p_co->pItem);
  }

  free((void *) p_co);
}

COMBO_DROP_CONTROL2 *co2_Create_Combo_Drop(int hdc, int x, int y, int id)
{
  int ax, ay;

  COMBO_DROP_CONTROL2 *p_co = NULL;

  p_co = (COMBO_DROP_CONTROL2 *) malloc(sizeof(COMBO_DROP_CONTROL2));

  if (!p_co)
    return 0;

  ZeroMemory(p_co, sizeof(COMBO_DROP_CONTROL2));

  p_co->CounfOfItems = 0;
  p_co->dropID = id;
  p_co->pItem = NULL;
  p_co->Selected = -1;
  p_co->x = x;
  p_co->y = y;

/*	BitBlt(hdc, x, y, _2dd.bitmap[hdcCO.hdcComboDrop].bitmap.bmWidth, _2dd.bitmap[hdcCO.hdcComboDrop].bitmap.bmHeight, 
		   _2dd.bitmap[hdcCO.hdcComboDrop].bitmapDC, 0, 0, SRCCOPY);*/

  ddx2BitBlt(hdc, x, y, ddx2GetWidth(hdcCO.hdcComboDrop),
    ddx2GetHeight(hdcCO.hdcComboDrop), hdcCO.hdcComboDrop, 0, 0);

  ax = x + ddx2GetWidth(hdcCO.hdcComboDrop);
  ay = y + ddx2GetHeight(hdcCO.hdcComboDrop);

  p_co->coLUp.top = y + 5;
  p_co->coLUp.bottom = ay - 5;
  p_co->coLUp.left = ax - 20;
  p_co->coLUp.right = ax - 2;

  p_co->coLDown.top = y + 5;
  p_co->coLDown.bottom = ay - 5;
  p_co->coLDown.left = ax - 39;
  p_co->coLDown.right = ax - 21;

  return p_co;
}

int co2_Combo_Drop_Add_String(COMBO_DROP_CONTROL2 * p_co, char *text,
  float fValue)
{
  int tx, ty;
  WCHAR wc[128];

  p_co->CounfOfItems++;

  p_co->pItem =
    (COMBO_ITEM2 *) realloc(p_co->pItem,
    p_co->CounfOfItems * sizeof(COMBO_ITEM2));

  if (!p_co->pItem)
    return 0;

  /*co2_CreateDC(_2dd.hDC, 
     _2dd.bitmap[hdcCO.hdcComboDrop].bitmap.bmWidth - 20, _2dd.bitmap[hdcCO.hdcComboMid].bitmap.bmHeight,
     &p_co->pItem[p_co->CounfOfItems-1].Norm); */

  p_co->pItem[p_co->CounfOfItems - 1].Norm =
    ddx2CreateSurface(ddx2GetWidth(hdcCO.hdcComboDrop) - 20,
    ddx2GetHeight(hdcCO.hdcComboMid), ddx2FindFreeSurface());

  /*BitBltU(p_co->pItem[p_co->CounfOfItems-1].Norm.hdc,
     0,0,p_co->pItem[p_co->CounfOfItems-1].Norm.x,p_co->pItem[p_co->CounfOfItems-1].Norm.y,NULL,0,0,WHITENESS); */

  /*co2_CreateDC(_2dd.hDC, 
     _2dd.bitmap[hdcCO.hdcComboDrop].bitmap.bmWidth - 20, _2dd.bitmap[hdcCO.hdcComboMid].bitmap.bmHeight,
     &p_co->pItem[p_co->CounfOfItems-1].Sel); */

  p_co->pItem[p_co->CounfOfItems - 1].Sel =
    ddx2CreateSurface(ddx2GetWidth(hdcCO.hdcComboDrop) - 20,
    ddx2GetHeight(hdcCO.hdcComboMid), ddx2FindFreeSurface());

  /*BitBltU(p_co->pItem[p_co->CounfOfItems-1].Sel.hdc,
     0,0,p_co->pItem[p_co->CounfOfItems-1].Sel.x,p_co->pItem[p_co->CounfOfItems-1].Sel.y,NULL,0,0,WHITENESS); */

  strcpy(p_co->pItem[p_co->CounfOfItems - 1].text, text);

  p_co->pItem[p_co->CounfOfItems - 1].fValue = fValue;

  MultiByteToWideChar(CP_ACP, 0, text, strlen(text) + 1, wc,
    sizeof(wc) / sizeof(wc[0]));
  fn2_Draw_MessageA(p_co->pItem[p_co->CounfOfItems - 1].Norm, 2, 0,
    &b2_3d_font.gt, &b2_3d_font.ts, wc, 0, &tx, &ty);
  fn2_Draw_MessageA(p_co->pItem[p_co->CounfOfItems - 1].Sel, 2, 0,
    &b2_3d_font.gt, &b2_3d_font.ts, wc, 1, &tx, &ty);

  return 1;
}

int co2_Combo_Drop_Set_Sel(int hdc, COMBO_DROP_CONTROL2 * p_co, int i)
{
  p_co->Selected = i;

  /*
     BitBlt(hdc, p_co->x, p_co->y, _2dd.bitmap[hdcCO.hdcComboDrop].bitmap.bmWidth, 
     _2dd.bitmap[hdcCO.hdcComboDrop].bitmap.bmHeight,
     _2dd.bitmap[hdcCO.hdcComboDrop].bitmapDC, 0, 0, SRCCOPY); */

  ddx2BitBlt(hdc, p_co->x, p_co->y, ddx2GetWidth(hdcCO.hdcComboDrop),
    ddx2GetHeight(hdcCO.hdcComboDrop), hdcCO.hdcComboDrop, 0, 0);

  if (p_co->pItem) {
    /*TransparentBltU(hdc, p_co->x + 5, p_co->y + 7, p_co->pItem[p_co->Selected].Norm.x, p_co->pItem[p_co->Selected].Norm.y,
       p_co->pItem[p_co->Selected].Norm.hdc, 0, 0, p_co->pItem[p_co->Selected].Norm.x, p_co->pItem[p_co->Selected].Norm.y, TRANSCOLOR); */

    ddx2TransparentBlt(hdc, p_co->x + 5, p_co->y + 7,
      ddx2GetWidth(p_co->pItem[p_co->Selected].Norm),
      ddx2GetHeight(p_co->pItem[p_co->Selected].Norm),
      p_co->pItem[p_co->Selected].Norm, 0, 0,
      ddx2GetWidth(p_co->pItem[p_co->Selected].Norm),
      ddx2GetHeight(p_co->pItem[p_co->Selected].Norm), TRANSCOLOR);
  }

  return 1;
}

int co2_Combo_Drop_Set_String(int hdc, COMBO_DROP_CONTROL2 * p_co, int xcor,
  int ycor)
{
  if (p_co->Selected < 0)
    return 0;

  /*BitBlt(hdc, p_co->x + xcor, p_co->y + ycor, _2dd.bitmap[hdcCO.hdcComboDrop].bitmap.bmWidth, 
     _2dd.bitmap[hdcCO.hdcComboDrop].bitmap.bmHeight,
     _2dd.bitmap[hdcCO.hdcComboDrop].bitmapDC, 0, 0, SRCCOPY); */

  ddx2BitBlt(hdc, p_co->x + xcor, p_co->y + ycor,
    ddx2GetWidth(hdcCO.hdcComboDrop), ddx2GetHeight(hdcCO.hdcComboDrop),
    hdcCO.hdcComboDrop, 0, 0);

  if (p_co->pItem) {
    /*TransparentBltU(hdc, p_co->x + xcor + 5, p_co->y + ycor + 7, p_co->pItem[p_co->Selected].Norm.x, p_co->pItem[p_co->Selected].Norm.y,
       p_co->pItem[p_co->Selected].Norm.hdc, 0, 0, p_co->pItem[p_co->Selected].Norm.x, p_co->pItem[p_co->Selected].Norm.y, TRANSCOLOR); */

    ddx2TransparentBlt(hdc, p_co->x + xcor + 5, p_co->y + ycor + 7,
      ddx2GetWidth(p_co->pItem[p_co->Selected].Norm),
      ddx2GetHeight(p_co->pItem[p_co->Selected].Norm),
      p_co->pItem[p_co->Selected].Norm, 0, 0,
      ddx2GetWidth(p_co->pItem[p_co->Selected].Norm),
      ddx2GetHeight(p_co->pItem[p_co->Selected].Norm), TRANSCOLOR);
  }

  return 1;
}

int co2_Combo_Drop_Get_Sel(CONTROL_LIST_ITEM2 * p_list, int lsize, int id,
  float *pf)
{
  int i;

  for (i = 0; i < lsize; i++)
    if ((p_list + i)->p_combod)
      if ((p_list + i)->p_combod->dropID == id) {
        if ((p_list + i)->p_combod->pItem
          && (p_list + i)->p_combod->Selected > -1)
          *pf =
            (p_list + i)->p_combod->pItem[(p_list +
              i)->p_combod->Selected].fValue;

        return (p_list + i)->p_combod->Selected;
      }

  return -1;
}

int co2_Combo_Add_String(COMBO_CONTROL2 * p_co, char *text)
{
  int tx, ty;
  WCHAR wc[128];

  p_co->CounfOfItems++;

  p_co->pItem =
    (COMBO_ITEM2 *) realloc(p_co->pItem,
    p_co->CounfOfItems * sizeof(COMBO_ITEM2));

  if (!p_co->pItem)
    return 0;

  /*co2_CreateDC(_2dd.hDC, 
     _2dd.bitmap[hdcCO.hdcCombo].bitmap.bmWidth - 10, _2dd.bitmap[hdcCO.hdcComboMid].bitmap.bmHeight,
     &p_co->pItem[p_co->CounfOfItems-1].Norm); */


  p_co->pItem[p_co->CounfOfItems - 1].Norm =
    ddx2CreateSurface(ddx2GetWidth(hdcCO.hdcCombo),
    ddx2GetHeight(hdcCO.hdcComboMid), ddx2FindFreeSurface());

  /*BitBltU(p_co->pItem[p_co->CounfOfItems-1].Norm.hdc,
     0,0,p_co->pItem[p_co->CounfOfItems-1].Norm.x,p_co->pItem[p_co->CounfOfItems-1].Norm.y,NULL,0,0,WHITENESS); */

  /*co2_CreateDC(_2dd.hDC, 
     _2dd.bitmap[hdcCO.hdcCombo].bitmap.bmWidth - 10, _2dd.bitmap[hdcCO.hdcComboMid].bitmap.bmHeight,
     &p_co->pItem[p_co->CounfOfItems-1].Sel);

     BitBltU(p_co->pItem[p_co->CounfOfItems-1].Sel.hdc,
     0,0,p_co->pItem[p_co->CounfOfItems-1].Sel.x,p_co->pItem[p_co->CounfOfItems-1].Sel.y,NULL,0,0,WHITENESS); */

  p_co->pItem[p_co->CounfOfItems - 1].Sel =
    ddx2CreateSurface(ddx2GetWidth(hdcCO.hdcCombo),
    ddx2GetHeight(hdcCO.hdcComboMid), ddx2FindFreeSurface());

  strcpy(p_co->pItem[p_co->CounfOfItems - 1].text, text);

  MultiByteToWideChar(CP_ACP, 0, text, strlen(text) + 1, wc,
    sizeof(wc) / sizeof(wc[0]));
  fn2_Draw_MessageA(p_co->pItem[p_co->CounfOfItems - 1].Norm, 2, 0,
    &b2_3d_font.gt, &b2_3d_font.ts, wc, 0, &tx, &ty);
  fn2_Draw_MessageA(p_co->pItem[p_co->CounfOfItems - 1].Sel, 2, 0,
    &b2_3d_font.gt, &b2_3d_font.ts, wc, 1, &tx, &ty);

  return 1;
}

int co2_Combo_Add_StringWC(COMBO_CONTROL2 * p_co, char *text)
{
  int tx, ty;
  WCHAR wc[128];
  WCHAR ws[128];

  p_co->CounfOfItems++;

  p_co->pItem =
    (COMBO_ITEM2 *) realloc(p_co->pItem,
    p_co->CounfOfItems * sizeof(COMBO_ITEM2));

  if (!p_co->pItem)
    return 0;

  /*co2_CreateDC(_2dd.hDC, 
     _2dd.bitmap[hdcCO.hdcCombo].bitmap.bmWidth - 10, _2dd.bitmap[hdcCO.hdcComboMid].bitmap.bmHeight,
     &p_co->pItem[p_co->CounfOfItems-1].Norm);

     BitBltU(p_co->pItem[p_co->CounfOfItems-1].Norm.hdc,
     0,0,p_co->pItem[p_co->CounfOfItems-1].Norm.x,p_co->pItem[p_co->CounfOfItems-1].Norm.y,NULL,0,0,WHITENESS); */

  p_co->pItem[p_co->CounfOfItems - 1].Norm =
    ddx2CreateSurface(ddx2GetWidth(hdcCO.hdcCombo),
    ddx2GetHeight(hdcCO.hdcComboMid), ddx2FindFreeSurface());

  /*co2_CreateDC(_2dd.hDC, 
     _2dd.bitmap[hdcCO.hdcCombo].bitmap.bmWidth - 10, _2dd.bitmap[hdcCO.hdcComboMid].bitmap.bmHeight,
     &p_co->pItem[p_co->CounfOfItems-1].Sel);

     BitBltU(p_co->pItem[p_co->CounfOfItems-1].Sel.hdc,
     0,0,p_co->pItem[p_co->CounfOfItems-1].Sel.x,p_co->pItem[p_co->CounfOfItems-1].Sel.y,NULL,0,0,WHITENESS); */

  p_co->pItem[p_co->CounfOfItems - 1].Sel =
    ddx2CreateSurface(ddx2GetWidth(hdcCO.hdcCombo),
    ddx2GetHeight(hdcCO.hdcComboMid), ddx2FindFreeSurface());

  strcpy(p_co->pItem[p_co->CounfOfItems - 1].text, text);

  MultiByteToWideChar(CP_ACP, 0, text, strlen(text) + 1, wc,
    sizeof(wc) / sizeof(wc[0]));
  MultiByteToWideChar(CP_ACP, 0, "##endofmessage",
    strlen("##endofmessage") + 1, ws, sizeof(ws) / sizeof(ws[0]));

  fn2_Draw_Message(p_co->pItem[p_co->CounfOfItems - 1].Norm, 2, 0,
    &b2_3d_font.gt, &b2_3d_font.ts, wc, ws, 0, &tx, &ty);
  fn2_Draw_Message(p_co->pItem[p_co->CounfOfItems - 1].Sel, 2, 0,
    &b2_3d_font.gt, &b2_3d_font.ts, wc, ws, 1, &tx, &ty);

  return 1;
}

int co2_Combo_Set_String(int hdc, COMBO_CONTROL2 * p_co, int xcor, int ycor)
{
  if (p_co->Selected < 0)
    return 0;

  /*BitBlt(hdc, p_co->x + xcor, p_co->y + ycor, _2dd.bitmap[hdcCO.hdcCombo].bitmap.bmWidth, 
     _2dd.bitmap[hdcCO.hdcCombo].bitmap.bmHeight,
     _2dd.bitmap[hdcCO.hdcCombo].bitmapDC, 0, 0, SRCCOPY); */

  ddx2BitBlt(hdc, p_co->x + xcor, p_co->y + ycor,
    ddx2GetWidth(hdcCO.hdcCombo), ddx2GetHeight(hdcCO.hdcCombo),
    hdcCO.hdcCombo, 0, 0);

  if (p_co->pItem) {
    /*TransparentBltU(hdc, p_co->x + xcor + 5, p_co->y + ycor + 7, p_co->pItem[p_co->Selected].Norm.x, p_co->pItem[p_co->Selected].Norm.y,
       p_co->pItem[p_co->Selected].Norm.hdc, 0, 0, p_co->pItem[p_co->Selected].Norm.x, p_co->pItem[p_co->Selected].Norm.y, TRANSCOLOR); */

    ddx2TransparentBlt(hdc, p_co->x + xcor + 5, p_co->y + ycor + 7,
      ddx2GetWidth(p_co->pItem[p_co->Selected].Norm),
      ddx2GetHeight(p_co->pItem[p_co->Selected].Norm),
      p_co->pItem[p_co->Selected].Norm, 0, 0,
      ddx2GetWidth(p_co->pItem[p_co->Selected].Norm),
      ddx2GetHeight(p_co->pItem[p_co->Selected].Norm), TRANSCOLOR);
  }

  return 1;
}

int co2_Combo_Set_Sel(int hdc, COMBO_CONTROL2 * p_co, int i)
{
  p_co->Selected = i;
  p_co->OSelected = i;

  /*BitBlt(hdc, p_co->x, p_co->y, _2dd.bitmap[hdcCO.hdcCombo].bitmap.bmWidth, 
     _2dd.bitmap[hdcCO.hdcCombo].bitmap.bmHeight,
     _2dd.bitmap[hdcCO.hdcCombo].bitmapDC, 0, 0, SRCCOPY); */

  ddx2BitBlt(hdc, p_co->x, p_co->y, ddx2GetWidth(hdcCO.hdcCombo),
    ddx2GetHeight(hdcCO.hdcCombo), hdcCO.hdcCombo, 0, 0);

  if (p_co->pItem) {
    /*TransparentBltU(hdc, p_co->x + 5, p_co->y + 7, p_co->pItem[p_co->Selected].Norm.x, p_co->pItem[p_co->Selected].Norm.y,
       p_co->pItem[p_co->Selected].Norm.hdc, 0, 0, p_co->pItem[p_co->Selected].Norm.x, p_co->pItem[p_co->Selected].Norm.y, TRANSCOLOR); */

    ddx2TransparentBlt(hdc, p_co->x + 5, p_co->y + 7,
      ddx2GetWidth(p_co->pItem[p_co->Selected].Norm),
      ddx2GetHeight(p_co->pItem[p_co->Selected].Norm),
      p_co->pItem[p_co->Selected].Norm, 0, 0,
      ddx2GetWidth(p_co->pItem[p_co->Selected].Norm),
      ddx2GetHeight(p_co->pItem[p_co->Selected].Norm), TRANSCOLOR);

  }

  return 1;
}

int co2_Combo_Get_Sel(CONTROL_LIST_ITEM2 * p_list, int lsize, int id)
{
  int i;

  for (i = 0; i < lsize; i++)
    if ((p_list + i)->p_combo)
      if ((p_list + i)->p_combo->comboID == id)
        return (p_list + i)->p_combo->Selected;

  return -1;
}

int co2_Combo_Get_Sel_Not_Opend(CONTROL_LIST_ITEM2 * p_list, int lsize,
  int id)
{
  int i;

  for (i = 0; i < lsize; i++)
    if ((p_list + i)->p_combo)
      if ((p_list + i)->p_combo->comboID == id
        && !(p_list + i)->p_combo->bList)
        return (p_list + i)->p_combo->Selected;

  return -1;
}

int co2_Combo_Set_Params(COMBO_CONTROL2 * p_co, int itemnum)
{
  p_co->ListMaxHight = itemnum * ddx2GetHeight(hdcCO.hdcComboMid);
  p_co->CounfOfItemsL = itemnum;

  return 1;
}

int co2_Combo_Open(int hdc, COMBO_CONTROL2 * p_co, int xcor, int ycor)
{
  int *pDCI;

  if (p_co->bList)
    return 1;

  bBlockList = 1;

  p_co->pBDC = (int *) malloc(sizeof(int));

  if (!p_co->pBDC)
    return 0;

  //p_co->ListMaxHightR = (p_co->CounfOfItems * 35) + _2dd.bitmap[hdcCO.hdcComboEnd].bitmap.bmHeight;
  //p_co->ListMaxHightR = (p_co->CounfOfItemsL * 25) + ddx2GetHeight(hdcCO.hdcComboEnd);

  if (p_co->CounfOfItems > p_co->CounfOfItemsL)
    p_co->ListMaxHightR =
      (p_co->CounfOfItemsL * 25) + ddx2GetHeight(hdcCO.hdcComboEnd);
  else
    p_co->ListMaxHightR =
      (p_co->CounfOfItems * 25) + ddx2GetHeight(hdcCO.hdcComboEnd);

  //co2_CreateDC(hdc, p_co->WidthR, p_co->ListMaxHightR, p_co->pBDC);
  *p_co->pBDC =
    ddx2CreateSurface(p_co->WidthR, p_co->ListMaxHightR,
    ddx2FindFreeSurface());

  pDCI = p_co->pBDC;

  if (!pDCI) {
    free((void *) p_co->pBDC);
    return 0;
  }

  /*BitBlt(pDCI->hdc, 0, 0, pDCI->x, pDCI->y, hdc, p_co->x + xcor, 
     p_co->y + _2dd.bitmap[hdcCO.hdcCombo].bitmap.bmHeight + ycor, SRCCOPY); */

  ddx2BitBlt(*pDCI, 0, 0, ddx2GetWidth(*pDCI), ddx2GetHeight(*pDCI), hdc,
    p_co->x + xcor, p_co->y + ddx2GetHeight(hdcCO.hdcCombo) + ycor);


  p_co->CSelected = -1;
  p_co->Selected = -1;

  p_co->bList = 1;
  p_co->bEdit = 0;
  co2_Combo_Draw(hdc, p_co, xcor, ycor);

  p_co->coListRect.top = p_co->y + ddx2GetHeight(hdcCO.hdcCombo);
  p_co->coListRect.bottom =
    p_co->y + ddx2GetHeight(hdcCO.hdcCombo) + p_co->ListMaxHightR;
  p_co->coListRect.left = p_co->x;

  if (p_co->CounfOfItems > p_co->CounfOfItemsL)
    p_co->coListRect.right = p_co->x + ddx2GetWidth(hdcCO.hdcComboMid) - 2 -
      ddx2GetWidth(hdcCO.hdcComboUp);
  else
    p_co->coListRect.right = p_co->x + ddx2GetWidth(hdcCO.hdcComboMid) - 2;

  p_co->bEdit = 1;
  return 1;
}

int co2_Combo_Close(int hdc, COMBO_CONTROL2 * p_co, int xcor, int ycor)
{
  if (!p_co->bList)
    return 1;

  bBlockList = 0;

  p_co->SSelected = 0;
  p_co->bList = 0;

/*	BitBlt(hdc, p_co->x + xcor, ycor + p_co->y + _2dd.bitmap[hdcCO.hdcCombo].bitmap.bmHeight, p_co->pBDC->x, p_co->pBDC->y,
		   p_co->pBDC->hdc, 0, 0, SRCCOPY);*/

  ddx2BitBlt(hdc, p_co->x + xcor,
    ycor + p_co->y + ddx2GetHeight(hdcCO.hdcCombo), ddx2GetWidth(*p_co->pBDC),
    ddx2GetHeight(*p_co->pBDC), *p_co->pBDC, 0, 0);

  //co2_Release_Bitmap(p_co->pBDC);
  ddx2ReleaseBitmap(*p_co->pBDC);

  free((void *) p_co->pBDC);
  p_co->pBDC = NULL;

  mi.t1 = 0;
  mi.dt1 = 0;

  return 1;
}

WCHAR *co2_Edit_Get_Text(CONTROL_EDIT2 * p_ed)
{
  if (!p_ed)
    return NULL;
  else
    return p_ed->wtext;
}

void co2_Edit_Set_Text(CONTROL_EDIT2 * p_ed, WCHAR * cText, char bRedraw,
  int hdc, int xcor, int ycor)
{
  int xt, yt;

  if (!p_ed)
    return;

  if (!cText)
    return;

  p_ed->pTime = timeGetTime();
  wcscpy(p_ed->wtext, cText);

  if (bRedraw) {
    ddx2BitBlt(hdc, p_ed->rect.left + xcor, p_ed->rect.top + ycor,
      ddx2GetWidth(hdcED.hdcEdit), ddx2GetHeight(hdcED.hdcEdit),
      hdcED.hdcEdit, 0, 0);

    ddx2CleareSurface(p_ed->bDC);

    fn2_Draw_MessageA(p_ed->bDC, 0, 3, &b2_3d_font.gt, &b2_3d_font.ts,
      p_ed->wtext, 0, &xt, &yt);

    ddx2TransparentBlt(hdc, p_ed->rect.left + 5 + xcor,
      p_ed->rect.top + 4 + xcor, p_ed->rect.right - p_ed->rect.left - 10,
      p_ed->rect.bottom - p_ed->rect.top - 8, p_ed->bDC, 0, 0,
      p_ed->rect.right - p_ed->rect.left - 10,
      p_ed->rect.bottom - p_ed->rect.top - 8, TRANSCOLOR);
  }
}

CONTROL_EDIT2 *co2_Create_Edit(int hdc, int x, int y, int editID)
{
  CONTROL_EDIT2 *p_ed = NULL;

  p_ed = (CONTROL_EDIT2 *) malloc(sizeof(CONTROL_EDIT2));

  if (!p_ed)
    return 0;

  ZeroMemory(p_ed, sizeof(CONTROL_EDIT2));

  p_ed->x = x;
  p_ed->y = y;
  p_ed->tx = 0;
  p_ed->bcActive = 0;
  p_ed->bActive = 0;
  p_ed->rect.left = x;
  p_ed->rect.top = y;
  p_ed->rect.right = x + ddx2GetWidth(hdcED.hdcEdit);
  p_ed->rect.bottom = y + ddx2GetHeight(hdcED.hdcEdit);

  /*BitBlt(hdc, x,  y, _2dd.bitmap[hdcED.hdcEdit].bitmap.bmWidth, _2dd.bitmap[hdcED.hdcEdit].bitmap.bmHeight, 
     _2dd.bitmap[hdcED.hdcEdit].bitmapDC, 0, 0, SRCCOPY); */

  ddx2BitBlt(hdc, x, y, ddx2GetWidth(hdcED.hdcEdit),
    ddx2GetHeight(hdcED.hdcEdit), hdcED.hdcEdit, 0, 0);

  /*BitBlt(_2dd.hDC, x,  y, _2dd.bitmap[hdcED.hdcEdit].bitmap.bmWidth, _2dd.bitmap[hdcED.hdcEdit].bitmap.bmHeight, 
     _2dd.bitmap[hdcED.hdcEdit].bitmapDC, 0, 0, SRCCOPY); */

  ddx2BitBlt(HDC2DD, x, y, ddx2GetWidth(hdcED.hdcEdit),
    ddx2GetHeight(hdcED.hdcEdit), hdcED.hdcEdit, 0, 0);

  /*co2_CreateDC(_2dd.hDC, _2dd.bitmap[hdcED.hdcEdit].bitmap.bmWidth + 25, _2dd.bitmap[hdcED.hdcEdit].bitmap.bmHeight, &p_ed->bDC);
     BitBltU(p_ed->bDC.hdc,0,0,p_ed->bDC.x,p_ed->bDC.y,NULL,0,0,WHITENESS); */

  p_ed->bDC =
    ddx2CreateSurface(ddx2GetWidth(hdcED.hdcEdit) + 25,
    ddx2GetHeight(hdcED.hdcEdit), ddx2FindFreeSurface());
  return p_ed;
}

void co2_Release_Button(BUTTON_CONTROL2 * p_bu)
{
  //co2_Release_Bitmap(&p_bu->dc);
  ddx2ReleaseBitmap(p_bu->dc);
  free((void *) p_bu);
}

BUTTON_CONTROL2 *co2_Create_Button(int hdc, int x, int y, int type,
  char *text, int isection, int buttonID)
{
  WCHAR wc[128];
  WCHAR ws[128];

  int bmpx, bmpy;
  int bmpDC;
  int cx, cy;
  int tx, ty;
  int tmpDC;
  BUTTON_CONTROL2 *p_bu = NULL;

  p_bu = (BUTTON_CONTROL2 *) malloc(sizeof(BUTTON_CONTROL2));

  if (!p_bu)
    return 0;

  ZeroMemory(p_bu, sizeof(BUTTON_CONTROL2));

  p_bu->ButtonID = buttonID;
  p_bu->bActivated = 0;
  p_bu->x = x;
  p_bu->y = y;

  switch (type) {
    case BUTTON_LARGE:
      bmpx = ddx2GetWidth(hdcBU.hdcButtonL);
      bmpy = ddx2GetHeight(hdcBU.hdcButtonL);
      bmpDC = hdcBU.hdcButtonL;
      //co2_CreateDC(hdc, bmpx, bmpy, &p_bu->dc);
      p_bu->dc = ddx2CreateSurface(bmpx, bmpy, ddx2FindFreeSurface());

      p_bu->Rect.bottom = y + bmpy;
      p_bu->Rect.right = x + bmpx;
      break;
    case BUTTON_SMALL:
      bmpx = ddx2GetWidth(hdcBU.hdcButtonS);
      bmpy = ddx2GetHeight(hdcBU.hdcButtonS);
      bmpDC = hdcBU.hdcButtonS;
      //co2_CreateDC(hdc, bmpx, bmpy, &p_bu->dc);
      p_bu->dc = ddx2CreateSurface(bmpx, bmpy, ddx2FindFreeSurface());

      p_bu->Rect.bottom = y + bmpy;
      p_bu->Rect.right = x + bmpx;
      break;
  }

  p_bu->type = type;
  p_bu->Rect.top = y;
  p_bu->Rect.left = x;

  //BitBlt(p_bu->dc.hdc, 0, 0, bmpx, bmpy, bmpDC, 0, 0, SRCCOPY);
  ddx2BitBlt(p_bu->dc, 0, 0, bmpx, bmpy, bmpDC, 0, 0);

  //co2_CreateDC(hdc, bmpx, bmpy, &tmpDC);
  //BitBltU(tmpDC.hdc,0,0,bmpx,bmpy,NULL,0,0,WHITENESS);
  tmpDC = ddx2CreateSurface(bmpx, bmpy, ddx2FindFreeSurface());

  MultiByteToWideChar(CP_ACP, 0, text, strlen(text) + 1, wc,
    sizeof(wc) / sizeof(wc[0]));
  MultiByteToWideChar(CP_ACP, 0, "##endofmessage",
    strlen("##endofmessage") + 1, ws, sizeof(ws) / sizeof(ws[0]));

  //fn2_Draw_Message(tmpDC.hdc, 0, 0, &b2_3d_font.gt, &b2_3d_font.ts, wc, ws, isection, &tx, &ty);
  fn2_Draw_Message(tmpDC, 0, 0, &b2_3d_font.gt, &b2_3d_font.ts, wc, ws,
    isection, &tx, &ty);

  cx = ftoi(((p_bu->Rect.right - p_bu->Rect.left) - tx) / 2.0f);
  cy = ftoi(((p_bu->Rect.bottom - p_bu->Rect.top) - ty) / 2.0f);

  //TransparentBltU(p_bu->dc.hdc, cx, cy, tx, ty,  tmpDC.hdc, 0, 0, tx, ty, TRANSCOLOR);
  ddx2TransparentBlt(p_bu->dc, cx, cy, tx, ty, tmpDC, 0, 0, tx, ty,
    TRANSCOLOR);

  //co2_Release_Bitmap(&tmpDC);
  ddx2ReleaseBitmap(tmpDC);

  //BitBlt(hdc, p_bu->Rect.left, p_bu->Rect.top, p_bu->dc.x, p_bu->dc.y, p_bu->dc.hdc, 0, 0, SRCCOPY);
  ddx2BitBlt(hdc, p_bu->Rect.left, p_bu->Rect.top, ddx2GetWidth(p_bu->dc),
    ddx2GetHeight(p_bu->dc), p_bu->dc, 0, 0);

  return p_bu;
}

int co2_Check_Set_State(CHECKBOX_CONTROL2 * p_ch, int hdc, int state,
  char bDraw)
{
  int x = 0, y = 0;
  int bmpx, bmpy;
  int bmpDC;

  bmpx = ddx2GetWidth(hdcCH.hdcCheck);
  bmpy = ddx2GetHeight(hdcCH.hdcCheck);
  bmpDC = hdcCH.hdcCheck;

  if (hdc == HDC2DD) {
    x = TAB_X;
    y = TAB_Y;
  }

  p_ch->bChecked = state;

  if (p_ch->bChecked) {
    if (bDraw) {
      //TransparentBltU(hdc, p_ch->Rect.left, p_ch->Rect.top, bmpx, 24, bmpDC, 0, 54, bmpx, 24, RGB(237, 77, 0));
      ddx2TransparentBlt(hdc, p_ch->Rect.left + x, p_ch->Rect.top + y, bmpx,
        20, bmpDC, 0, 25, bmpx, 20, RGB(237, 77, 0));
    }

    p_ch->bChange = 1;
    //return 1;
  }
  else {
    if (bDraw) {
      //TransparentBltU(hdc, p_ch->Rect.left, p_ch->Rect.top, bmpx, 24, bmpDC, 0, 80, bmpx, 24, RGB(237, 77, 0));
      ddx2TransparentBlt(hdc, p_ch->Rect.left + x, p_ch->Rect.top + y, bmpx,
        20, bmpDC, 0, 1, bmpx, 20, RGB(237, 77, 0));
    }

    p_ch->bChange = 1;
    //return 1;
  }

  if (p_ch->iDC != -1)
    ddx2BitBlt(p_ch->iDC, 0, 0, p_ch->RectFull.right, p_ch->RectFull.bottom,
      hdc, p_ch->x + x, p_ch->y + y);

  return 1;
}

int co2_Check_Get_State_Change(CONTROL_LIST_ITEM2 * p_list, int lsize, int id)
{
  int i;

  for (i = 0; i < lsize; i++)
    if ((p_list + i)->p_check)
      if ((p_list + i)->p_check->checkID == id
        && (p_list + i)->p_check->bChange) {
        (p_list + i)->p_check->bChange = 0;
        return (p_list + i)->p_check->bChecked;
      }

  return -1;
}

void co2_Check_Disable(int hdc, int xcor, int ycor,
  CONTROL_LIST_ITEM2 * p_list, int lsize, int id)
{
  CHECKBOX_CONTROL2 *p_ch;
  int i;

  for (i = 0; i < lsize; i++)
    if ((p_list + i)->p_check)
      if ((p_list + i)->p_check->checkID == id) {
        p_ch = (p_list + i)->p_check;
        (p_list + i)->bActive = 0;
        (p_list + i)->bDisabled = 1;

        ddx2TransparentBlt(hdc, p_ch->RectFull.left + xcor,
          p_ch->RectFull.top + ycor,
          p_ch->RectFull.right,
          p_ch->RectFull.bottom,
          hdcCH.hdcGray,
          0, 0, p_ch->RectFull.right, p_ch->RectFull.bottom, TRANSCOLOR);
        return;
      }

  return;
}

void co2_Check_Enable(int hdc, int xcor, int ycor,
  CONTROL_LIST_ITEM2 * p_list, int lsize, int id)
{
  CHECKBOX_CONTROL2 *p_ch;
  int i;

  for (i = 0; i < lsize; i++)
    if ((p_list + i)->p_check)
      if ((p_list + i)->p_check->checkID == id) {
        p_ch = (p_list + i)->p_check;
        (p_list + i)->bActive = 1;
        (p_list + i)->bDisabled = 0;

        if (p_ch->iDC != -1)
          ddx2BitBlt(hdc, p_ch->RectFull.left + xcor,
            p_ch->RectFull.top + ycor,
            p_ch->RectFull.right, p_ch->RectFull.bottom, p_ch->iDC, 0, 0);
        return;
      }

  return;
}

void co2_Release_CheckBox(CHECKBOX_CONTROL2 * p_ch)
{
  if (p_ch->iDC != -1)
    ddx2ReleaseBitmap(p_ch->iDC);

  free((void *) p_ch);
}

CHECKBOX_CONTROL2 *co2_Create_CheckBox(int hdc, int x, int y, char *text,
  int isection, int checkID)
{
  WCHAR wc[128];
  WCHAR ws[128];

  int bmpx, bmpy;
  int bmpDC;
  int tx, ty;
  CHECKBOX_CONTROL2 *p_ch = NULL;

  p_ch = (CHECKBOX_CONTROL2 *) malloc(sizeof(CHECKBOX_CONTROL2));

  if (!p_ch)
    return 0;

  ZeroMemory(p_ch, sizeof(CHECKBOX_CONTROL2));

  p_ch->checkID = checkID;

  bmpx = ddx2GetWidth(hdcCH.hdcCheck);
  bmpy = ddx2GetHeight(hdcCH.hdcCheck);
  bmpDC = hdcCH.hdcCheck;
  //co2_CreateDC(hdc, bmpx, bmpy, &p_bu->dc);

  p_ch->Rect.bottom = y + 24;
  p_ch->Rect.right = x + bmpx;
  p_ch->Rect.top = y;
  p_ch->Rect.left = x;
  p_ch->x = x;
  p_ch->y = y;

  //TransparentBltU(hdc, x, y, bmpx, 24, bmpDC, 0, 80, bmpx, 24, RGB(237, 77, 0));
  ddx2TransparentBlt(hdc, x, y, bmpx, 20, bmpDC, 0, 1, bmpx, 20, RGB(237, 77,
      0));

  MultiByteToWideChar(CP_ACP, 0, text, strlen(text) + 1, wc,
    sizeof(wc) / sizeof(wc[0]));
  MultiByteToWideChar(CP_ACP, 0, "##endofmessage",
    strlen("##endofmessage") + 1, ws, sizeof(ws) / sizeof(ws[0]));

  fn2_Draw_Message(hdc, x + 30, y, &b2_3d_font.gt, &b2_3d_font.ts, wc, ws,
    isection, &tx, &ty);

  p_ch->RectFull.left = x;
  p_ch->RectFull.top = y;
  p_ch->RectFull.right = 30 + tx - p_ch->RectFull.left;
  p_ch->RectFull.bottom = ty - p_ch->RectFull.top;

  p_ch->iDC =
    ddx2CreateSurface(p_ch->RectFull.right, p_ch->RectFull.bottom,
    ddx2FindFreeSurface());

  return p_ch;
}

int co2_Progres_Set(PROGRES_CONTROL2 * p_pr, int hdc, int i)
{
  int dd = p_pr->max - p_pr->min;
  int md = p_pr->rectProgres.right - p_pr->rectProgres.left;
  float in = md / (float) dd;
  int x = p_pr->rectProgres.left + ftoi((i - p_pr->min) * in);

  int yp;

  //BitBlt(hdc, p_pr->rectMover.left, p_pr->rectMover.top, p_pr->bDC.x, p_pr->bDC.y, p_pr->bDC.hdc, 0, 0, SRCCOPY);
  ddx2BitBlt(hdc, p_pr->rectMover.left, p_pr->rectMover.top,
    ddx2GetWidth(p_pr->bDC), ddx2GetHeight(p_pr->bDC), p_pr->bDC, 0, 0);

  p_pr->pos = x;
  p_pr->rectMover.left = p_pr->pos - p_pr->cor;
  p_pr->rectMover.right = p_pr->rectMover.left + ddx2GetWidth(hdcPR.hdcMover);

  yp = ftoi(ddx2GetHeight(hdcPR.hdcMover) / 2.0f);

  /*BitBlt(p_pr->bDC.hdc, 0, 0, _2dd.bitmap[hdcPR.hdcMover].bitmap.bmWidth, 
     _2dd.bitmap[hdcPR.hdcMover].bitmap.bmHeight, hdc, 
     p_pr->rectMover.left, p_pr->rectMover.top, SRCCOPY); */

  ddx2BitBlt(p_pr->bDC, 0, 0, ddx2GetWidth(hdcPR.hdcMover),
    ddx2GetHeight(hdcPR.hdcMover), hdc,
    p_pr->rectMover.left, p_pr->rectMover.top);

  /*BitBlt(hdc, p_pr->rectMover.left, p_pr->rectMover.top, 
     _2dd.bitmap[hdcPR.hdcMover].bitmap.bmWidth, _2dd.bitmap[hdcPR.hdcMover].bitmap.bmHeight, 
     _2dd.bitmap[hdcPR.hdcMover].bitmapDC, 0, 0, SRCCOPY); */

  /*ddx2BitBlt(hdc, p_pr->rectMover.left, p_pr->rectMover.top, 
     ddx2GetWidth(hdcPR.hdcMover), ddx2GetHeight(hdcPR.hdcMover), 
     hdcPR.hdcMover, 0, 0); */

  ddx2TransparentBlt(hdc, p_pr->rectMover.left, p_pr->rectMover.top,
    ddx2GetWidth(hdcPR.hdcMover), ddx2GetHeight(hdcPR.hdcMover),
    hdcPR.hdcMover, 0, 0, ddx2GetWidth(hdcPR.hdcMover),
    ddx2GetHeight(hdcPR.hdcMover), RGB(237, 77, 0));
  return 1;
}

void co2_Progres_Disable(int hdc, int xcor, int ycor,
  CONTROL_LIST_ITEM2 * p_list, int lsize, int id, char bSave, int shdc)
{
  PROGRES_CONTROL2 *p_pr;
  int i;

  for (i = 0; i < lsize; i++)
    if ((p_list + i)->p_prog)
      if ((p_list + i)->p_prog->progID == id) {
        p_pr = (p_list + i)->p_prog;
        (p_list + i)->bActive = 0;
        (p_list + i)->bDisabled = 1;

        if (bSave)
          ddx2BitBlt(p_pr->iDC, 0, 0, p_pr->RectFull.right,
            p_pr->RectFull.bottom, shdc, p_pr->RectFull.left + xcor,
            p_pr->RectFull.top + ycor);

        ddx2TransparentBlt(hdc, p_pr->RectFull.left + xcor,
          p_pr->RectFull.top + ycor,
          p_pr->RectFull.right,
          p_pr->RectFull.bottom,
          hdcCH.hdcGray,
          0, 0, p_pr->RectFull.right, p_pr->RectFull.bottom, TRANSCOLOR);
        return;
      }

  return;
}

void co2_Progres_Enable(int hdc, int xcor, int ycor,
  CONTROL_LIST_ITEM2 * p_list, int lsize, int id)
{
  PROGRES_CONTROL2 *p_pr;
  int i;

  for (i = 0; i < lsize; i++)
    if ((p_list + i)->p_prog)
      if ((p_list + i)->p_prog->progID == id) {
        p_pr = (p_list + i)->p_prog;
        (p_list + i)->bActive = 1;
        (p_list + i)->bDisabled = 0;

        if (p_pr->iDC != -1)
          ddx2BitBlt(hdc, p_pr->RectFull.left + xcor,
            p_pr->RectFull.top + ycor,
            p_pr->RectFull.right, p_pr->RectFull.bottom, p_pr->iDC, 0, 0);
        return;
      }

  return;
}


int co2_Progres_Get(CONTROL_LIST_ITEM2 * p_list, int lsize, int id)
{
  int i;

  for (i = 0; i < lsize; i++)
    if ((p_list + i)->p_prog)
      if ((p_list + i)->p_prog->progID == id) {
        PROGRES_CONTROL2 *p_pr = (p_list + i)->p_prog;
        int dd = p_pr->max - p_pr->min;
        int md = p_pr->rectProgres.right - p_pr->rectProgres.left;
        float out = dd / (float) md;
        int x = p_pr->min + ftoi((p_pr->pos - p_pr->rectProgres.left) * out);

        return x;
      }

  return 0;
}

int co2_Progres_Changed(CONTROL_LIST_ITEM2 * p_list, int lsize, int id)
{
  int i;

  for (i = 0; i < lsize; i++)
    if ((p_list + i)->p_prog)
      if ((p_list + i)->p_prog->progID == id)
        if ((p_list + i)->p_prog->bChange)
          return 1;
        else
          return 0;

  return 0;
}

void co2_Release_Progres(PROGRES_CONTROL2 * p_pr)
{
  //co2_Release_Bitmap(&p_pr->bDC);
  ddx2ReleaseBitmap(p_pr->bDC);

  free((void *) p_pr);
}

PROGRES_CONTROL2 *co2_Create_Progres(int hdc, int x, int y, int min, int max,
  int progID)
{
  int bmpx, bmpy, yp;
  int bmpDC;
  PROGRES_CONTROL2 *p_pr = NULL;

  p_pr = (PROGRES_CONTROL2 *) malloc(sizeof(PROGRES_CONTROL2));

  if (!p_pr)
    return 0;

  ZeroMemory(p_pr, sizeof(PROGRES_CONTROL2));

  p_pr->bChange = 0;
  p_pr->bIn = 0;
  p_pr->progID = progID;
  p_pr->min = min;
  p_pr->max = max;
  p_pr->x = x;
  p_pr->y = y;
  p_pr->cor = ftoi(ddx2GetWidth(hdcPR.hdcMover) / 2.0f);
  p_pr->pos = x;
  p_pr->bExclusive = 0;

  bmpx = ddx2GetWidth(hdcPR.hdcLine);
  bmpy = ddx2GetHeight(hdcPR.hdcMover);
  bmpDC = hdcPR.hdcLine;
  yp = ftoi(bmpy / 2.0f);

  p_pr->rectMover.top = y;
  p_pr->rectMover.left = x;
  p_pr->rectMover.right = x + ddx2GetWidth(hdcPR.hdcMover);
  p_pr->rectMover.bottom = y + bmpy;

  p_pr->rectProgres.top = y;
  p_pr->rectProgres.left = x;
  p_pr->rectProgres.bottom = p_pr->rectMover.bottom;
  p_pr->rectProgres.right = x + bmpx;

  //BitBlt(hdc, x, y+yp, bmpx, _2dd.bitmap[hdcPR.hdcLine].bitmap.bmHeight, bmpDC, 0, 0, SRCCOPY);
  ddx2BitBlt(hdc, x, y + yp, bmpx, ddx2GetHeight(hdcPR.hdcLine), bmpDC, 0, 0);
  //co2_CreateDC(hdc, _2dd.bitmap[hdcPR.hdcMover].bitmap.bmWidth, _2dd.bitmap[hdcPR.hdcMover].bitmap.bmHeight, &p_pr->bDC);
  p_pr->bDC =
    ddx2CreateSurface(ddx2GetWidth(hdcPR.hdcMover),
    ddx2GetHeight(hdcPR.hdcMover), ddx2FindFreeSurface());

  //BitBlt(p_pr->bDC.hdc, 0, 0, _2dd.bitmap[hdcPR.hdcMover].bitmap.bmWidth, bmpy, hdc, x, y, SRCCOPY);
  ddx2BitBlt(p_pr->bDC, 0, 0, ddx2GetWidth(hdcPR.hdcMover), bmpy, hdc, x, y);

  //BitBlt(hdc, x, y, _2dd.bitmap[hdcPR.hdcMover].bitmap.bmWidth, bmpy, _2dd.bitmap[hdcPR.hdcMover].bitmapDC, 0, 0, SRCCOPY);
  //ddx2BitBlt(hdc, x, y, ddx2GetWidth(hdcPR.hdcMover), bmpy, hdcPR.hdcMover, 0, 0);
  ddx2TransparentBlt(hdc, x, y, ddx2GetWidth(hdcPR.hdcMover), bmpy,
    hdcPR.hdcMover, 0, 0, ddx2GetWidth(hdcPR.hdcMover), bmpy, RGB(237, 77,
      0));

  p_pr->RectFull.left = x - yp - 300;
  p_pr->RectFull.top = y;
  p_pr->RectFull.right = bmpx + yp + 300;
  p_pr->RectFull.bottom = bmpy;

  p_pr->iDC =
    ddx2CreateSurface(p_pr->RectFull.right, p_pr->RectFull.bottom,
    ddx2FindFreeSurface());

  return p_pr;
}

int co2_Set_Text(int hdc, int x, int y, char *text, int isection)
{
  int tx, ty;
  WCHAR wc[128];
  WCHAR ws[128];

  MultiByteToWideChar(CP_ACP, 0, text, strlen(text) + 1, wc,
    sizeof(wc) / sizeof(wc[0]));
  MultiByteToWideChar(CP_ACP, 0, "##endofmessage",
    strlen("##endofmessage") + 1, ws, sizeof(ws) / sizeof(ws[0]));

  fn2_Draw_Message(hdc, x, y, &b2_3d_font.gt, &b2_3d_font.ts, wc, ws,
    isection, &tx, &ty);

  return 1;
}

int co2_Set_TextWC(int hdc, int x, int y, char *text, int isection)
{
  int tx, ty;
  WCHAR wc[128];

  MultiByteToWideChar(CP_ACP, 0, text, strlen(text) + 1, wc,
    sizeof(wc) / sizeof(wc[0]));

  fn2_Draw_MessageA(hdc, x, y, &b2_3d_font.gt, &b2_3d_font.ts, wc, isection,
    &tx, &ty);

  return 1;
}

int co2_Set_Text_Formated_Rect(int hdc, int x, int y, char *text,
  int isection, RECT * pR)
{
  int tx, ty;
  WCHAR wc[128];
  WCHAR ws[128];

  MultiByteToWideChar(CP_ACP, 0, text, strlen(text) + 1, wc,
    sizeof(wc) / sizeof(wc[0]));
  MultiByteToWideChar(CP_ACP, 0, "##endofmessage",
    strlen("##endofmessage") + 1, ws, sizeof(ws) / sizeof(ws[0]));

  fn2_Draw_MessageRECT(hdc, x, y, &b2_3d_font.gt, &b2_3d_font.ts, wc, ws,
    isection, &tx, &ty, pR);

  return ty;
}

int co2_Set_Text_Center(int hdc, char *text, int isection, RECT r)
{
  int xp, yp;
  int tx, ty;
  WCHAR wc[128];
  WCHAR ws[128];
  int h;

  //co2_CreateDC(hdc, 600, 100, &h);

  h = ddx2CreateSurface(600, 100, ddx2FindFreeSurface());

  //BitBltU(h.hdc, 0, 0, 600, 100, NULL, 0, 0, WHITENESS);

  MultiByteToWideChar(CP_ACP, 0, text, strlen(text) + 1, wc,
    sizeof(wc) / sizeof(wc[0]));
  MultiByteToWideChar(CP_ACP, 0, "##endofmessage",
    strlen("##endofmessage") + 1, ws, sizeof(ws) / sizeof(ws[0]));

  fn2_Draw_Message(h, 0, 0, &b2_3d_font.gt, &b2_3d_font.ts, wc, ws, isection,
    &tx, &ty);

  xp = ftoi(((r.right - r.left) - tx) / 2.0f);
  yp = ftoi(((r.bottom - r.top) - ty) / 2.0f);

  //TransparentBltU(hdc, r.left + xp, r.top + yp, tx, ty, h.hdc, 0, 0, tx, ty, TRANSCOLOR);
  ddx2TransparentBlt(hdc, r.left + xp, r.top + yp, tx, ty, h, 0, 0, tx, ty,
    TRANSCOLOR);

  //TransparentBltU(_2dd.hDC, r.left + xp, r.top + yp, tx, ty, h.hdc, 0, 0, tx, ty, TRANSCOLOR);  
  ddx2TransparentBltDisplay(r.left + xp, r.top + yp, tx, ty, h, 0, 0, tx, ty,
    TRANSCOLOR);

  //co2_Release_Bitmap(&h);
  ddx2ReleaseBitmap(h);

  return 1;
}

int co2_Set_Text_Right(int hdc, char *text, int isection, int x, int y)
{
  int xp;
  int tx, ty;
  WCHAR wc[128];
  WCHAR ws[128];
  int h;

  h = ddx2CreateSurface(600, 100, ddx2FindFreeSurface());

  MultiByteToWideChar(CP_ACP, 0, text, strlen(text) + 1, wc,
    sizeof(wc) / sizeof(wc[0]));
  MultiByteToWideChar(CP_ACP, 0, "##endofmessage",
    strlen("##endofmessage") + 1, ws, sizeof(ws) / sizeof(ws[0]));

  fn2_Draw_Message(h, 0, 0, &b2_3d_font.gt, &b2_3d_font.ts, wc, ws, isection,
    &tx, &ty);

  xp = x - tx;

  ddx2TransparentBlt(hdc, xp, y, tx, ty, h, 0, 0, tx, ty, TRANSCOLOR);

  //ddx2TransparentBltDisplay(xp, y, tx, ty, h, 0, 0, tx, ty, TRANSCOLOR);        

  ddx2ReleaseBitmap(h);

  return 1;
}

void co2_Draw_Line(int x1, int y1, int x2, int y2, COLORREF color, HDC hdc)
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

void co2_Draw_Lines(HDC hdc, int xr, int yr, COLORREF color)
{
  int i;

  for (i = 35; i < yr; i += 35)
    co2_Draw_Line(0, i, xr, i, color, hdc);
}

int co2_List_Delete_Item(LIST_VIEW_CONTROL2 * p_li, char **cValue)
{
  int i;
  RECT r;

  if (!p_li->bSelection)
    return 0;

  if (p_li->cClckSel == -1)
    return 0;

  if (p_li->cClckSel >= p_li->listnum)
    return 0;

  if (!p_li->cText[p_li->cClckSel])
    return 0;

  if (cValue)
    (*cValue) = strdup(p_li->cText[p_li->cClckSel]);

  free((void *) p_li->cText[p_li->cClckSel]);
  free((void *) p_li->pwText[p_li->cClckSel]);

  for (i = p_li->cClckSel + 1; i < p_li->listnum; i++) {
    p_li->piValue[i - 1] = p_li->piValue[i];
    p_li->cText[i - 1] = p_li->cText[i];
    p_li->pwText[i - 1] = p_li->pwText[i];
  }

  p_li->pwText[p_li->listnum - 1] = NULL;
  p_li->cText[p_li->listnum - 1] = NULL;
  p_li->piValue[p_li->listnum - 1] = 0;

  i = p_li->cClckSel * 30;

  ddx2BitBlt(p_li->bDCn, 0, i, ddx2GetWidth(p_li->bDCn),
    ddx2GetHeight(p_li->bDCn) - (i + 30), p_li->bDCn, 0, i + 30);

  ddx2BitBlt(p_li->bDCs, 0, i, ddx2GetWidth(p_li->bDCn),
    ddx2GetHeight(p_li->bDCn) - (i + 30), p_li->bDCs, 0, i + 30);

  r.left = 0;
  r.right = ddx2GetWidth(p_li->bDCn);
  r.bottom = ddx2GetHeight(p_li->bDCn);
  r.top = r.bottom - 30;

  ddx2FillRect(p_li->bDCn, &r, 0);
  ddx2FillRect(p_li->bDCs, &r, 0);

  if (ddx2GetHeight(p_li->bDCn) - 30 >
    p_li->rectList.bottom - p_li->rectList.top) {
    int ds, dn;

    ds = p_li->bDCs;
    dn = p_li->bDCn;

    p_li->bDCn =
      ddx2CreateSurface(ddx2GetWidth(dn), ddx2GetHeight(dn) - 30,
      ddx2FindFreeSurface());
    p_li->bDCs =
      ddx2CreateSurface(ddx2GetWidth(ds), ddx2GetHeight(ds) - 30,
      ddx2FindFreeSurface());

    ddx2BitBlt(p_li->bDCn, 0, 0, ddx2GetWidth(p_li->bDCn),
      ddx2GetHeight(p_li->bDCn), dn, 0, 0);
    ddx2BitBlt(p_li->bDCs, 0, 0, ddx2GetWidth(p_li->bDCs),
      ddx2GetHeight(p_li->bDCs), ds, 0, 0);

    ddx2ReleaseBitmap(dn);
    ddx2ReleaseBitmap(ds);

    p_li->listnum--;
  }

  p_li->cClckSel = -1;

  return 1;
}

int co2_List_Add_String(LIST_VIEW_CONTROL2 * p_li, int index, int x,
  char *text, int iValue, char bSelected)
{
  int tx, ty;
  WCHAR wc[128];
  WCHAR ws[128];

  if (p_li->piValue)
    p_li->piValue[index] = iValue;

  MultiByteToWideChar(CP_ACP, 0, "##endofmessage",
    strlen("##endofmessage") + 1, ws, sizeof(ws) / sizeof(ws[0]));

  MultiByteToWideChar(CP_ACP, 0, text, strlen(text) + 1, wc,
    sizeof(wc) / sizeof(wc[0]));

  if (!bSelected)
    fn2_Draw_Message(p_li->bDCn, x, (index * 30) + 2, &b2_3d_font.gt,
      &b2_3d_font.ts, wc, ws, 0, &tx, &ty);
  else
    fn2_Draw_Message(p_li->bDCn, x, (index * 30) + 2, &b2_3d_font.gt,
      &b2_3d_font.ts, wc, ws, 1, &tx, &ty);

  fn2_Draw_Message(p_li->bDCs, x, (index * 30) + 2, &b2_3d_font.gt,
    &b2_3d_font.ts, wc, ws, 1, &tx, &ty);

  if (p_li->pwText[index]) {
    free((void *) p_li->pwText[index]);
    p_li->pwText[index] = NULL;
  }

  p_li->pwText[index] = wcsdup(wc);

  if (p_li->cText[index]) {
    free((void *) p_li->cText[index]);
    p_li->cText[index] = NULL;
  }

  p_li->cText[index] = strdup(text);

  return 1;
}

int co2_List_Add_StringWC(LIST_VIEW_CONTROL2 * p_li, int index, int x,
  char *text, int iValue, char bSelected)
{
  int tx, ty;
  WCHAR wc[128];

  if (p_li->piValue)
    p_li->piValue[index] = iValue;

  MultiByteToWideChar(CP_ACP, 0, text, strlen(text) + 1, wc,
    sizeof(wc) / sizeof(wc[0]));

  if (!bSelected)
    fn2_Draw_MessageA(p_li->bDCn, x, (index * 30) + 2, &b2_3d_font.gt,
      &b2_3d_font.ts, wc, 0, &tx, &ty);
  else
    fn2_Draw_MessageA(p_li->bDCn, x, (index * 30) + 2, &b2_3d_font.gt,
      &b2_3d_font.ts, wc, 1, &tx, &ty);

  fn2_Draw_MessageA(p_li->bDCs, x, (index * 30) + 2, &b2_3d_font.gt,
    &b2_3d_font.ts, wc, 1, &tx, &ty);

  if (p_li->pwText[index]) {
    free((void *) p_li->pwText[index]);
    p_li->pwText[index] = NULL;
  }

  p_li->pwText[index] = wcsdup(wc);

  if (p_li->cText[index]) {
    free((void *) p_li->cText[index]);
    p_li->cText[index] = NULL;
  }

  p_li->cText[index] = strdup(text);

  return 1;
}

int co2_List_Add_StringWC2(LIST_VIEW_CONTROL2 * p_li, int index, int x,
  WCHAR * wc, char *text, int iValue, char bSelected)
{
  int tx, ty;

  if (p_li->piValue)
    p_li->piValue[index] = iValue;

  if (!bSelected)
    fn2_Draw_MessageA(p_li->bDCn, x, (index * 30) + 2, &b2_3d_font.gt,
      &b2_3d_font.ts, wc, 0, &tx, &ty);
  else
    fn2_Draw_MessageA(p_li->bDCn, x, (index * 30) + 2, &b2_3d_font.gt,
      &b2_3d_font.ts, wc, 1, &tx, &ty);

  fn2_Draw_MessageA(p_li->bDCs, x, (index * 30) + 2, &b2_3d_font.gt,
    &b2_3d_font.ts, wc, 1, &tx, &ty);

  if (p_li->pwText[index]) {
    free((void *) p_li->pwText[index]);
    p_li->pwText[index] = NULL;
  }

  p_li->pwText[index] = wcsdup(wc);

  if (p_li->cText[index]) {
    free((void *) p_li->cText[index]);
    p_li->cText[index] = NULL;
  }

  p_li->cText[index] = strdup(text);

  return 1;
}


int co2_List_Redraw(int hdc, LIST_VIEW_CONTROL2 * p_li, int y)
{
  /*BitBlt(hdc, p_li->rectList.left, p_li->rectList.top, p_li->bDCn.x, 
     p_li->rectList.bottom - p_li->rectList.top, p_li->bDCn.hdc, 0, y, SRCCOPY); */

  if (y + (p_li->rectList.bottom - p_li->rectList.top) >
    ddx2GetHeight(p_li->bDCn))
    y =
      ddx2GetHeight(p_li->bDCn) - (p_li->rectList.bottom -
      p_li->rectList.top);

  if (y < 0)
    y = 0;

  ddx2BitBlt(hdc, p_li->rectList.left, p_li->rectList.top,
    ddx2GetWidth(p_li->bDCn), p_li->rectList.bottom - p_li->rectList.top,
    p_li->bDCn, 0, y);

  return y;
}

int co2_List_View_Redraw(int hdc, LIST_VIEW2_CONTROL2 * p_li, int y)
{
  ddx2BitBlt(hdc, p_li->rectList.left, p_li->rectList.top,
    ddx2GetWidth(p_li->bDC), p_li->rectList.bottom - p_li->rectList.top,
    p_li->bDC, 0, y);

  return 1;
}

void co2_Release_List(LIST_VIEW_CONTROL2 * p_li)
{
  int i;

  //co2_Release_Bitmap(&p_li->bDCm);
  ddx2ReleaseBitmap(p_li->bDCm);
  //co2_Release_Bitmap(&p_li->bDCn);
  ddx2ReleaseBitmap(p_li->bDCn);
  //co2_Release_Bitmap(&p_li->bDCs);
  ddx2ReleaseBitmap(p_li->bDCs);

  if (p_li->piValue);
  free((void *) p_li->piValue);

  if (p_li->pwText)
    for (i = 0; i < p_li->listnum; i++)
      if (p_li->pwText[i])
        free((void *) p_li->pwText[i]);

  if (p_li->cText)
    for (i = 0; i < p_li->listnum; i++)
      if (p_li->cText[i])
        free((void *) p_li->cText[i]);

  free((void *) p_li->pwText);
  free((void *) p_li->cText);

  free((void *) p_li);
}

void co2_Release_List_View(LIST_VIEW2_CONTROL2 * p_li)
{
  ddx2ReleaseBitmap(p_li->bDCm);
  ddx2ReleaseBitmap(p_li->bDC);

  free((void *) p_li);
}

int co2_Get_List_View_DC(LIST_VIEW2_CONTROL2 * p_li)
{
  if (!p_li)
    return -1;
  else
    return p_li->bDC;
}

LIST_VIEW_CONTROL2 *co2_Create_List(int hdc, int x, int y, int width,
  int hight, int id, int numofitems, char bSelection)
{
  int ax, ay, lx, ly;
  LIST_VIEW_CONTROL2 *p_li = NULL;

  p_li = (LIST_VIEW_CONTROL2 *) malloc(sizeof(LIST_VIEW_CONTROL2));

  if (!p_li)
    return 0;

  p_li->piValue = (int *) malloc(numofitems * sizeof(int));
  p_li->pwText = (WCHAR **) malloc(numofitems * sizeof(WCHAR *));
  p_li->cText = (char **) malloc(numofitems * sizeof(char *));
  p_li->bClck = 0;
  p_li->bDblClck = 0;
  p_li->pTime = timeGetTime();
  p_li->Pushc = 0;
  p_li->listnum = numofitems;
  p_li->dx = 0;
  p_li->bIn = 0;
  p_li->bInE = 0;
  p_li->x = x;
  p_li->y = y;
  p_li->mpos = 0;
  p_li->listID = id;
  p_li->rectList.top = y + ddx2GetHeight(hdcLI.hdcListTop);
  p_li->rectList.left = x + ddx2GetWidth(hdcLI.hdcListLeft);
  p_li->rectList.bottom = y + hight - ddx2GetHeight(hdcLI.hdcListBottom);
  p_li->rectList.right =
    x + width - ddx2GetWidth(hdcLI.hdcListRight) -
    ddx2GetWidth(hdcLI.hdcListUp);
  p_li->cClckSel = -1;
  p_li->bSelection = bSelection;

  if (p_li->pwText)
    ZeroMemory(p_li->pwText, numofitems * sizeof(WCHAR *));

  if (p_li->cText)
    ZeroMemory(p_li->cText, numofitems * sizeof(char *));

  ax = x;
  ay = y;

  /*BitBlt(hdc, x, y, _2dd.bitmap[hdcLI.hdcListTL].bitmap.bmWidth, 
     _2dd.bitmap[hdcLI.hdcListTL].bitmap.bmHeight, _2dd.bitmap[hdcLI.hdcListTL].bitmapDC, 0, 0, SRCCOPY); */

  ddx2BitBlt(hdc, x, y, ddx2GetWidth(hdcLI.hdcListTL),
    ddx2GetHeight(hdcLI.hdcListTL), hdcLI.hdcListTL, 0, 0);

  ax += ddx2GetWidth(hdcLI.hdcListTL);

  lx = ax;

  while (ax - lx < width - ddx2GetWidth(hdcLI.hdcListTop)) {
    /*BitBlt(hdc, ax, ay, _2dd.bitmap[hdcLI.hdcListTop].bitmap.bmWidth, 
       _2dd.bitmap[hdcLI.hdcListTop].bitmap.bmHeight, _2dd.bitmap[hdcLI.hdcListTop].bitmapDC, 0, 0, SRCCOPY); */

    ddx2BitBlt(hdc, ax, ay, ddx2GetWidth(hdcLI.hdcListTop),
      ddx2GetHeight(hdcLI.hdcListTop), hdcLI.hdcListTop, 0, 0);

    ax += ddx2GetWidth(hdcLI.hdcListTop);
  }

  /*BitBlt(hdc, x + width - _2dd.bitmap[hdcLI.hdcListTop].bitmap.bmWidth, ay, _2dd.bitmap[hdcLI.hdcListTop].bitmap.bmWidth, 
     _2dd.bitmap[hdcLI.hdcListTop].bitmap.bmHeight, _2dd.bitmap[hdcLI.hdcListTop].bitmapDC, 0, 0, SRCCOPY); */

  ddx2BitBlt(hdc, x + width - ddx2GetWidth(hdcLI.hdcListTop), ay,
    ddx2GetWidth(hdcLI.hdcListTop), ddx2GetHeight(hdcLI.hdcListTop),
    hdcLI.hdcListTop, 0, 0);

  /*BitBlt(hdc, x + width - _2dd.bitmap[hdcLI.hdcListTR].bitmap.bmWidth, ay, _2dd.bitmap[hdcLI.hdcListTR].bitmap.bmWidth, 
     _2dd.bitmap[hdcLI.hdcListTR].bitmap.bmHeight, _2dd.bitmap[hdcLI.hdcListTR].bitmapDC, 0, 0, SRCCOPY); */

  ddx2BitBlt(hdc, x + width - ddx2GetWidth(hdcLI.hdcListTR), ay,
    ddx2GetWidth(hdcLI.hdcListTR), ddx2GetHeight(hdcLI.hdcListTR),
    hdcLI.hdcListTR, 0, 0);

  ay += ddx2GetHeight(hdcLI.hdcListTop);

  ly = ay;
  ax = x;

  while (ay - ly < hight - ddx2GetHeight(hdcLI.hdcListLeft)) {
    /*BitBlt(hdc, ax, ay, _2dd.bitmap[hdcLI.hdcListLeft].bitmap.bmWidth, 
       _2dd.bitmap[hdcLI.hdcListLeft].bitmap.bmHeight, _2dd.bitmap[hdcLI.hdcListLeft].bitmapDC, 0, 0, SRCCOPY); */

    ddx2BitBlt(hdc, ax, ay, ddx2GetWidth(hdcLI.hdcListLeft),
      ddx2GetHeight(hdcLI.hdcListLeft), hdcLI.hdcListLeft, 0, 0);

    /*BitBlt(hdc, ax + width - _2dd.bitmap[hdcLI.hdcListRight].bitmap.bmWidth, ay, _2dd.bitmap[hdcLI.hdcListRight].bitmap.bmWidth, 
       _2dd.bitmap[hdcLI.hdcListRight].bitmap.bmHeight, _2dd.bitmap[hdcLI.hdcListRight].bitmapDC, 0, 0, SRCCOPY); */

    ddx2BitBlt(hdc, ax + width - ddx2GetWidth(hdcLI.hdcListRight), ay,
      ddx2GetWidth(hdcLI.hdcListRight), ddx2GetHeight(hdcLI.hdcListRight),
      hdcLI.hdcListRight, 0, 0);

    ay += ddx2GetHeight(hdcLI.hdcListLeft);
  }


  /*BitBlt(hdc, ax, y + hight - _2dd.bitmap[hdcLI.hdcListLeft].bitmap.bmHeight, _2dd.bitmap[hdcLI.hdcListLeft].bitmap.bmWidth, 
     _2dd.bitmap[hdcLI.hdcListLeft].bitmap.bmHeight, _2dd.bitmap[hdcLI.hdcListLeft].bitmapDC, 0, 0, SRCCOPY); */

  ddx2BitBlt(hdc, ax, y + hight - ddx2GetHeight(hdcLI.hdcListLeft),
    ddx2GetWidth(hdcLI.hdcListLeft), ddx2GetHeight(hdcLI.hdcListLeft),
    hdcLI.hdcListLeft, 0, 0);

  /*BitBlt(hdc, ax + width - _2dd.bitmap[hdcLI.hdcListRight].bitmap.bmWidth, y + hight - _2dd.bitmap[hdcLI.hdcListRight].bitmap.bmHeight, 
     _2dd.bitmap[hdcLI.hdcListRight].bitmap.bmWidth, _2dd.bitmap[hdcLI.hdcListRight].bitmap.bmHeight, 
     _2dd.bitmap[hdcLI.hdcListRight].bitmapDC, 0, 0, SRCCOPY); */

  ddx2BitBlt(hdc, ax + width - ddx2GetWidth(hdcLI.hdcListRight),
    y + hight - ddx2GetHeight(hdcLI.hdcListRight),
    ddx2GetWidth(hdcLI.hdcListRight), ddx2GetHeight(hdcLI.hdcListRight),
    hdcLI.hdcListRight, 0, 0);

  ay = y + hight - ddx2GetHeight(hdcLI.hdcListBottom);
  ax = x;

  /*BitBlt(hdc, x, ay, _2dd.bitmap[hdcLI.hdcListBL].bitmap.bmWidth, 
     _2dd.bitmap[hdcLI.hdcListBL].bitmap.bmHeight, _2dd.bitmap[hdcLI.hdcListBL].bitmapDC, 0, 0, SRCCOPY); */

  ddx2BitBlt(hdc, x, ay, ddx2GetWidth(hdcLI.hdcListBL),
    ddx2GetHeight(hdcLI.hdcListBL), hdcLI.hdcListBL, 0, 0);

  ax += ddx2GetWidth(hdcLI.hdcListBL);

  lx = ax;

  while (ax - lx < width - ddx2GetWidth(hdcLI.hdcListBottom)) {
    /*BitBlt(hdc, ax, ay, _2dd.bitmap[hdcLI.hdcListBottom].bitmap.bmWidth, 
       _2dd.bitmap[hdcLI.hdcListBottom].bitmap.bmHeight, _2dd.bitmap[hdcLI.hdcListBottom].bitmapDC, 0, 0, SRCCOPY); */

    ddx2BitBlt(hdc, ax, ay, ddx2GetWidth(hdcLI.hdcListBottom),
      ddx2GetHeight(hdcLI.hdcListBottom), hdcLI.hdcListBottom, 0, 0);

    ax += ddx2GetWidth(hdcLI.hdcListBottom);
  }

  /*BitBlt(hdc, x + width - _2dd.bitmap[hdcLI.hdcListBottom].bitmap.bmWidth, ay, _2dd.bitmap[hdcLI.hdcListBottom].bitmap.bmWidth, 
     _2dd.bitmap[hdcLI.hdcListBottom].bitmap.bmHeight, _2dd.bitmap[hdcLI.hdcListBottom].bitmapDC, 0, 0, SRCCOPY); */

  ddx2BitBlt(hdc, x + width - ddx2GetWidth(hdcLI.hdcListBottom), ay,
    ddx2GetWidth(hdcLI.hdcListBottom), ddx2GetHeight(hdcLI.hdcListBottom),
    hdcLI.hdcListBottom, 0, 0);

  /*BitBlt(hdc, x + width - _2dd.bitmap[hdcLI.hdcListBR].bitmap.bmWidth, ay, _2dd.bitmap[hdcLI.hdcListBR].bitmap.bmWidth, 
     _2dd.bitmap[hdcLI.hdcListBR].bitmap.bmHeight, _2dd.bitmap[hdcLI.hdcListBR].bitmapDC, 0, 0, SRCCOPY); */

  ddx2BitBlt(hdc, x + width - ddx2GetWidth(hdcLI.hdcListBR), ay,
    ddx2GetWidth(hdcLI.hdcListBR), ddx2GetHeight(hdcLI.hdcListBR),
    hdcLI.hdcListBR, 0, 0);

  p_li->rectUp.top = y + ddx2GetHeight(hdcLI.hdcListTop);
  p_li->rectUp.bottom = p_li->rectUp.top + ddx2GetHeight(hdcLI.hdcListUp);
  p_li->rectUp.left =
    x + width - ddx2GetWidth(hdcLI.hdcListBR) - ddx2GetWidth(hdcLI.hdcListUp);
  p_li->rectUp.right = p_li->rectUp.left + ddx2GetWidth(hdcLI.hdcListUp);

  /*TransparentBltU(hdc, x + width - _2dd.bitmap[hdcLI.hdcListBR].bitmap.bmWidth - _2dd.bitmap[hdcLI.hdcListUp].bitmap.bmWidth, 
     y + _2dd.bitmap[hdcLI.hdcListTop].bitmap.bmHeight, 
     _2dd.bitmap[hdcLI.hdcListUp].bitmap.bmWidth, 
     _2dd.bitmap[hdcLI.hdcListUp].bitmap.bmHeight, 
     _2dd.bitmap[hdcLI.hdcListUp].bitmapDC, 
     0, 0, 
     _2dd.bitmap[hdcLI.hdcListUp].bitmap.bmWidth, 
     _2dd.bitmap[hdcLI.hdcListUp].bitmap.bmHeight, RGB(238, 77, 0)); */

  ddx2TransparentBlt(hdc,
    x + width - ddx2GetWidth(hdcLI.hdcListBR) - ddx2GetWidth(hdcLI.hdcListUp),
    y + ddx2GetHeight(hdcLI.hdcListTop), ddx2GetWidth(hdcLI.hdcListUp),
    ddx2GetHeight(hdcLI.hdcListUp), hdcLI.hdcListUp, 0, 0,
    ddx2GetWidth(hdcLI.hdcListUp), ddx2GetHeight(hdcLI.hdcListUp), RGB(238,
      77, 0));

  p_li->rectDown.top =
    y + hight - ddx2GetHeight(hdcLI.hdcListBR) -
    ddx2GetHeight(hdcLI.hdcListDown);
  p_li->rectDown.bottom = p_li->rectDown.top + ddx2GetHeight(hdcLI.hdcListUp);
  p_li->rectDown.left =
    x + width - ddx2GetWidth(hdcLI.hdcListTR) - ddx2GetWidth(hdcLI.hdcListUp);
  p_li->rectDown.right = p_li->rectDown.left + ddx2GetWidth(hdcLI.hdcListUp);

  /*TransparentBltU(hdc, x + width - _2dd.bitmap[hdcLI.hdcListTR].bitmap.bmWidth - _2dd.bitmap[hdcLI.hdcListUp].bitmap.bmWidth, 
     y + hight - _2dd.bitmap[hdcLI.hdcListBR].bitmap.bmWidth - _2dd.bitmap[hdcLI.hdcListDown].bitmap.bmHeight, 
     _2dd.bitmap[hdcLI.hdcListDown].bitmap.bmWidth, 
     _2dd.bitmap[hdcLI.hdcListDown].bitmap.bmHeight, 
     _2dd.bitmap[hdcLI.hdcListDown].bitmapDC, 
     0, 0, 
     _2dd.bitmap[hdcLI.hdcListDown].bitmap.bmWidth, 
     _2dd.bitmap[hdcLI.hdcListDown].bitmap.bmHeight, RGB(238, 77, 0)); */

  ddx2TransparentBlt(hdc,
    x + width - ddx2GetWidth(hdcLI.hdcListTR) - ddx2GetWidth(hdcLI.hdcListUp),
    y + hight - ddx2GetHeight(hdcLI.hdcListBR) -
    ddx2GetHeight(hdcLI.hdcListDown), ddx2GetWidth(hdcLI.hdcListDown),
    ddx2GetHeight(hdcLI.hdcListDown), hdcLI.hdcListDown, 0, 0,
    ddx2GetWidth(hdcLI.hdcListDown), ddx2GetHeight(hdcLI.hdcListDown),
    RGB(238, 77, 0));

  //co2_CreateDC(hdc, _2dd.bitmap[hdcCH.hdcCheck].bitmap.bmWidth, 24, &p_li->bDCm);

  p_li->bDCm =
    ddx2CreateSurface(ddx2GetWidth(hdcCH.hdcCheck), 24,
    ddx2FindFreeSurface());

  /*BitBlt(p_li->bDCm.hdc, 0, 0, p_li->bDCm.x, p_li->bDCm.y, hdc, 
     x + width - _2dd.bitmap[hdcLI.hdcListBR].bitmap.bmWidth - _2dd.bitmap[hdcCH.hdcCheck].bitmap.bmWidth - 2, 
     y + _2dd.bitmap[hdcLI.hdcListTop].bitmap.bmHeight + _2dd.bitmap[hdcLI.hdcListUp].bitmap.bmHeight, 
     SRCCOPY); */

  ddx2BitBlt(p_li->bDCm, 0, 0, ddx2GetWidth(p_li->bDCm),
    ddx2GetHeight(p_li->bDCm), hdc,
    x + width - ddx2GetWidth(hdcLI.hdcListBR) - ddx2GetWidth(hdcCH.hdcCheck) -
    2, y + ddx2GetHeight(hdcLI.hdcListTop) + ddx2GetHeight(hdcLI.hdcListUp));

  /*TransparentBltU(hdc, x + width - _2dd.bitmap[hdcLI.hdcListBR].bitmap.bmWidth - _2dd.bitmap[hdcCH.hdcCheck].bitmap.bmWidth - 2, 
     y + _2dd.bitmap[hdcLI.hdcListTop].bitmap.bmHeight + _2dd.bitmap[hdcLI.hdcListUp].bitmap.bmHeight, 
     _2dd.bitmap[hdcCH.hdcCheck].bitmap.bmWidth, 
     24, 
     _2dd.bitmap[hdcCH.hdcCheck].bitmapDC, 
     0, 0, 
     _2dd.bitmap[hdcCH.hdcCheck].bitmap.bmWidth, 
     24, RGB(237, 77, 0)); */

  ddx2TransparentBlt(hdc,
    x + width - ddx2GetWidth(hdcLI.hdcListBR) - ddx2GetWidth(hdcCH.hdcCheck) -
    2, y + ddx2GetHeight(hdcLI.hdcListTop) + ddx2GetHeight(hdcLI.hdcListUp),
    ddx2GetWidth(hdcCH.hdcCheck), 20, hdcCH.hdcCheck, 0, 49,
    ddx2GetWidth(hdcCH.hdcCheck), 20, RGB(237, 77, 0));

  p_li->rectMover.top =
    y + ddx2GetHeight(hdcLI.hdcListTop) + ddx2GetHeight(hdcLI.hdcListUp);
  p_li->rectMover.bottom = p_li->rectMover.top + 24;
  p_li->rectMover.left =
    x + width - ddx2GetWidth(hdcLI.hdcListBR) - ddx2GetWidth(hdcCH.hdcCheck) -
    2;
  p_li->rectMover.right = p_li->rectMover.left + ddx2GetWidth(hdcCH.hdcCheck);

  //co2_CreateDC(hdc, p_li->rectList.right - p_li->rectList.left, numofitems * 35, &p_li->bDCn);
  p_li->bDCn =
    ddx2CreateSurface(p_li->rectList.right - p_li->rectList.left,
    numofitems * 30, ddx2FindFreeSurface());
  ddx2CleareSurfaceColor(p_li->bDCn, 0);

  //co2_CreateDC(hdc, p_li->rectList.right - p_li->rectList.left, numofitems * 35, &p_li->bDCs);
  p_li->bDCs =
    ddx2CreateSurface(p_li->rectList.right - p_li->rectList.left,
    numofitems * 30, ddx2FindFreeSurface());
  ddx2CleareSurfaceColor(p_li->bDCs, 0);

  //co2_Draw_Lines(p_li->bDCn.hdc, p_li->bDCn.x, p_li->bDCn.y, RGB(101, 98, 93));
  //co2_Draw_Lines(p_li->bDCs.hdc, p_li->bDCs.x, p_li->bDCs.y, RGB(101, 98, 93));

  /*BitBlt(hdc, x + _2dd.bitmap[hdcLI.hdcListLeft].bitmap.bmWidth,
     y + _2dd.bitmap[hdcLI.hdcListTop].bitmap.bmHeight,
     p_li->bDCn.x,
     p_li->rectList.bottom - p_li->rectList.top,
     p_li->bDCn.hdc,
     0, 0, SRCCOPY); */

  ddx2BitBlt(hdc, x + ddx2GetWidth(hdcLI.hdcListLeft),
    y + ddx2GetHeight(hdcLI.hdcListTop),
    ddx2GetWidth(p_li->bDCn),
    p_li->rectList.bottom - p_li->rectList.top, p_li->bDCn, 0, 0);

  p_li->mpos = p_li->mpmin =
    y + ddx2GetHeight(hdcLI.hdcListTop) + ddx2GetHeight(hdcLI.hdcListUp) + 12;
  p_li->mpmax =
    y + hight - ddx2GetHeight(hdcLI.hdcListBottom) -
    ddx2GetHeight(hdcLI.hdcListDown) - 12;

  p_li->rectMoverA.left = p_li->rectMover.left;
  p_li->rectMoverA.right = p_li->rectMover.right;
  p_li->rectMoverA.top =
    y + ddx2GetHeight(hdcLI.hdcListTop) + ddx2GetHeight(hdcLI.hdcListUp) + 12;
  p_li->rectMoverA.bottom =
    y + hight - ddx2GetHeight(hdcLI.hdcListBottom) -
    ddx2GetHeight(hdcLI.hdcListDown) - 12;


  return p_li;
}

void co2_Set_List_View_List_Pos(LIST_VIEW2_CONTROL2 * p_li, int y, int hdc,
  int xcor, int ycor)
{
  int dcm, mm, pm, pos;

  dcm =
    ddx2GetHeight(p_li->bDC) - (p_li->rectList.bottom - p_li->rectList.top);
  mm = p_li->mpmax - p_li->mpmin;

  if (y > dcm)
    y = dcm;

  pm = y;

  pos = p_li->mpmin + ftoi((mm * y) / (float) dcm);

  if (pos > p_li->mpmax)
    pos = p_li->mpmax;
  else if (pos < p_li->mpmin)
    pos = p_li->mpmin;

  p_li->mpos = pos;
  p_li->dx = y;

  ddx2BitBlt(hdc, p_li->rectMover.left + xcor, p_li->rectMover.top + ycor,
    ddx2GetWidth(p_li->bDCm), ddx2GetHeight(p_li->bDCm), p_li->bDCm, 0, 0);

  p_li->rectMover.top = p_li->mpos - 12;
  p_li->rectMover.bottom = p_li->mpos + 12;

  ddx2BitBlt(p_li->bDCm, 0, 0, ddx2GetWidth(p_li->bDCm),
    ddx2GetHeight(p_li->bDCm), hdc, p_li->rectMover.left + xcor,
    p_li->rectMover.top + ycor);

  ddx2TransparentBlt(hdc, p_li->rectMover.left + xcor,
    p_li->rectMover.top + ycor,
    ddx2GetWidth(hdcCH.hdcCheck),
    20,
    hdcCH.hdcCheck, 0, 49, ddx2GetWidth(hdcCH.hdcCheck), 20, RGB(237, 77, 0));

  ddx2BitBlt(hdc, p_li->rectList.left + xcor, p_li->rectList.top + ycor,
    ddx2GetWidth(p_li->bDC), p_li->rectList.bottom - p_li->rectList.top,
    p_li->bDC, 0, y);

/*HANDLE_LISTVIEW:

		if(y > p_li->mpmax)
			y = p_li->mpmax;
		else
			if(y < p_li->mpmin)
				y = p_li->mpmin;

		if(p_li->mpos == y)
			return 1;

		p_li->mpos = y;

		ddx2BitBlt(hdc, p_li->rectMover.left + xcor, p_li->rectMover.top + ycor, ddx2GetWidth(p_li->bDCm), 
				  ddx2GetHeight(p_li->bDCm), p_li->bDCm, 0, 0);

		p_li->rectMover.top = p_li->mpos - 12;
		p_li->rectMover.bottom = p_li->mpos + 12;

		ddx2BitBlt(p_li->bDCm, 0, 0, ddx2GetWidth(p_li->bDCm), ddx2GetHeight(p_li->bDCm), hdc, 
			      p_li->rectMover.left + xcor, p_li->rectMover.top + ycor);

		ddx2TransparentBlt(hdc, p_li->rectMover.left + xcor, 
					      p_li->rectMover.top + ycor, 
					      ddx2GetWidth(hdcCH.hdcCheck), 
				          24, 
				          hdcCH.hdcCheck, 
				          0, 0, 
				          ddx2GetWidth(hdcCH.hdcCheck), 
				          24, RGB(237, 77, 0));

		dcm = ddx2GetHeight(p_li->bDCn) - (p_li->rectList.bottom - p_li->rectList.top);
		mm = p_li->mpmax - p_li->mpmin;
		pm = p_li->mpos - p_li->mpmin;

		pos = ftoi((pm * dcm) / (float)mm);

		if(p_li->mpos == p_li->mpmax)
			pos = ddx2GetHeight(p_li->bDCn) - (p_li->rectList.bottom - p_li->rectList.top);

		if(p_li->mpos == p_li->mpmin)
			pos = 0;

		p_li->dx = pos;

		ddx2BitBlt(hdc, p_li->rectList.left + xcor, p_li->rectList.top + ycor, ddx2GetWidth(p_li->bDCn), 
			      p_li->rectList.bottom - p_li->rectList.top, p_li->bDCn, 0, pos);*/
}

LIST_VIEW2_CONTROL2 *co2_Create_List_View(int hdc, int x, int y, int width,
  int hight, int id, int iViewHeight)
{
  int ax, ay, lx, ly;
  LIST_VIEW2_CONTROL2 *p_li = NULL;

  p_li = (LIST_VIEW2_CONTROL2 *) malloc(sizeof(LIST_VIEW2_CONTROL2));

  if (!p_li)
    return 0;

  p_li->bIn = 0;
  p_li->bInE = 0;
  p_li->iViewHeight = iViewHeight;
  p_li->dx = 0;
  p_li->x = x;
  p_li->y = y;
  p_li->mpos = 0;
  p_li->listID = id;
  p_li->rectList.top = y + ddx2GetHeight(hdcLI.hdcListTop);
  p_li->rectList.left = x + ddx2GetWidth(hdcLI.hdcListLeft);
  p_li->rectList.bottom = y + hight - ddx2GetHeight(hdcLI.hdcListBottom);
  p_li->rectList.right =
    x + width - ddx2GetWidth(hdcLI.hdcListRight) -
    ddx2GetWidth(hdcLI.hdcListUp);

  ax = x;
  ay = y;

  ddx2BitBlt(hdc, x, y, ddx2GetWidth(hdcLI.hdcListTL),
    ddx2GetHeight(hdcLI.hdcListTL), hdcLI.hdcListTL, 0, 0);

  ax += ddx2GetWidth(hdcLI.hdcListTL);

  lx = ax;

  while (ax - lx < width - ddx2GetWidth(hdcLI.hdcListTop)) {
    ddx2BitBlt(hdc, ax, ay, ddx2GetWidth(hdcLI.hdcListTop),
      ddx2GetHeight(hdcLI.hdcListTop), hdcLI.hdcListTop, 0, 0);

    ax += ddx2GetWidth(hdcLI.hdcListTop);
  }

  ddx2BitBlt(hdc, x + width - ddx2GetWidth(hdcLI.hdcListTop), ay,
    ddx2GetWidth(hdcLI.hdcListTop), ddx2GetHeight(hdcLI.hdcListTop),
    hdcLI.hdcListTop, 0, 0);

  ddx2BitBlt(hdc, x + width - ddx2GetWidth(hdcLI.hdcListTR), ay,
    ddx2GetWidth(hdcLI.hdcListTR), ddx2GetHeight(hdcLI.hdcListTR),
    hdcLI.hdcListTR, 0, 0);

  ay += ddx2GetHeight(hdcLI.hdcListTop);

  ly = ay;
  ax = x;

  while (ay - ly < hight - ddx2GetHeight(hdcLI.hdcListLeft)) {
    ddx2BitBlt(hdc, ax, ay, ddx2GetWidth(hdcLI.hdcListLeft),
      ddx2GetHeight(hdcLI.hdcListLeft), hdcLI.hdcListLeft, 0, 0);

    ddx2BitBlt(hdc, ax + width - ddx2GetWidth(hdcLI.hdcListRight), ay,
      ddx2GetWidth(hdcLI.hdcListRight), ddx2GetHeight(hdcLI.hdcListRight),
      hdcLI.hdcListRight, 0, 0);

    ay += ddx2GetHeight(hdcLI.hdcListLeft);
  }

  ddx2BitBlt(hdc, ax, y + hight - ddx2GetHeight(hdcLI.hdcListLeft),
    ddx2GetWidth(hdcLI.hdcListLeft), ddx2GetHeight(hdcLI.hdcListLeft),
    hdcLI.hdcListLeft, 0, 0);

  ddx2BitBlt(hdc, ax + width - ddx2GetWidth(hdcLI.hdcListRight),
    y + hight - ddx2GetHeight(hdcLI.hdcListRight),
    ddx2GetWidth(hdcLI.hdcListRight), ddx2GetHeight(hdcLI.hdcListRight),
    hdcLI.hdcListRight, 0, 0);

  ay = y + hight - ddx2GetHeight(hdcLI.hdcListBottom);
  ax = x;

  ddx2BitBlt(hdc, x, ay, ddx2GetWidth(hdcLI.hdcListBL),
    ddx2GetHeight(hdcLI.hdcListBL), hdcLI.hdcListBL, 0, 0);

  ax += ddx2GetWidth(hdcLI.hdcListBL);

  lx = ax;

  while (ax - lx < width - ddx2GetWidth(hdcLI.hdcListBottom)) {
    ddx2BitBlt(hdc, ax, ay, ddx2GetWidth(hdcLI.hdcListBottom),
      ddx2GetHeight(hdcLI.hdcListBottom), hdcLI.hdcListBottom, 0, 0);

    ax += ddx2GetWidth(hdcLI.hdcListBottom);
  }

  ddx2BitBlt(hdc, x + width - ddx2GetWidth(hdcLI.hdcListBottom), ay,
    ddx2GetWidth(hdcLI.hdcListBottom), ddx2GetHeight(hdcLI.hdcListBottom),
    hdcLI.hdcListBottom, 0, 0);

  ddx2BitBlt(hdc, x + width - ddx2GetWidth(hdcLI.hdcListBR), ay,
    ddx2GetWidth(hdcLI.hdcListBR), ddx2GetHeight(hdcLI.hdcListBR),
    hdcLI.hdcListBR, 0, 0);

  p_li->rectUp.top = y + ddx2GetHeight(hdcLI.hdcListTop);
  p_li->rectUp.bottom = p_li->rectUp.top + ddx2GetHeight(hdcLI.hdcListUp);
  p_li->rectUp.left =
    x + width - ddx2GetWidth(hdcLI.hdcListBR) - ddx2GetWidth(hdcLI.hdcListUp);
  p_li->rectUp.right = p_li->rectUp.left + ddx2GetWidth(hdcLI.hdcListUp);

  ddx2TransparentBlt(hdc,
    x + width - ddx2GetWidth(hdcLI.hdcListBR) - ddx2GetWidth(hdcLI.hdcListUp),
    y + ddx2GetHeight(hdcLI.hdcListTop), ddx2GetWidth(hdcLI.hdcListUp),
    ddx2GetHeight(hdcLI.hdcListUp), hdcLI.hdcListUp, 0, 0,
    ddx2GetWidth(hdcLI.hdcListUp), ddx2GetHeight(hdcLI.hdcListUp), RGB(238,
      77, 0));

  p_li->rectDown.top =
    y + hight - ddx2GetHeight(hdcLI.hdcListBR) -
    ddx2GetHeight(hdcLI.hdcListDown);
  p_li->rectDown.bottom = p_li->rectDown.top + ddx2GetHeight(hdcLI.hdcListUp);
  p_li->rectDown.left =
    x + width - ddx2GetWidth(hdcLI.hdcListTR) - ddx2GetWidth(hdcLI.hdcListUp);
  p_li->rectDown.right = p_li->rectDown.left + ddx2GetWidth(hdcLI.hdcListUp);

  ddx2TransparentBlt(hdc,
    x + width - ddx2GetWidth(hdcLI.hdcListTR) - ddx2GetWidth(hdcLI.hdcListUp),
    y + hight - ddx2GetHeight(hdcLI.hdcListBR) -
    ddx2GetHeight(hdcLI.hdcListDown), ddx2GetWidth(hdcLI.hdcListDown),
    ddx2GetHeight(hdcLI.hdcListDown), hdcLI.hdcListDown, 0, 0,
    ddx2GetWidth(hdcLI.hdcListDown), ddx2GetHeight(hdcLI.hdcListDown),
    RGB(238, 77, 0));

  p_li->bDCm =
    ddx2CreateSurface(ddx2GetWidth(hdcCH.hdcCheck), 24,
    ddx2FindFreeSurface());

  ddx2BitBlt(p_li->bDCm, 0, 0, ddx2GetWidth(p_li->bDCm),
    ddx2GetHeight(p_li->bDCm), hdc,
    x + width - ddx2GetWidth(hdcLI.hdcListBR) - ddx2GetWidth(hdcCH.hdcCheck) -
    2, y + ddx2GetHeight(hdcLI.hdcListTop) + ddx2GetHeight(hdcLI.hdcListUp));

  ddx2TransparentBlt(hdc,
    x + width - ddx2GetWidth(hdcLI.hdcListBR) - ddx2GetWidth(hdcCH.hdcCheck),
    y + ddx2GetHeight(hdcLI.hdcListTop) + ddx2GetHeight(hdcLI.hdcListUp),
    ddx2GetWidth(hdcCH.hdcCheck), 20, hdcCH.hdcCheck, 0, 49,
    ddx2GetWidth(hdcCH.hdcCheck), 20, RGB(237, 77, 0));

  p_li->rectMover.top =
    y + ddx2GetHeight(hdcLI.hdcListTop) + ddx2GetHeight(hdcLI.hdcListUp);
  p_li->rectMover.bottom = p_li->rectMover.top + 24;
  p_li->rectMover.left =
    x + width - ddx2GetWidth(hdcLI.hdcListBR) - ddx2GetWidth(hdcCH.hdcCheck);
  p_li->rectMover.right = p_li->rectMover.left + ddx2GetWidth(hdcCH.hdcCheck);

  p_li->bDC =
    ddx2CreateSurface(p_li->rectList.right - p_li->rectList.left, iViewHeight,
    ddx2FindFreeSurface());
  ddx2CleareSurfaceColor(p_li->bDC, 0);

  ddx2BitBlt(hdc, x + ddx2GetWidth(hdcLI.hdcListLeft),
    y + ddx2GetHeight(hdcLI.hdcListTop),
    ddx2GetWidth(p_li->bDC),
    p_li->rectList.bottom - p_li->rectList.top, p_li->bDC, 0, 0);

  p_li->mpos = p_li->mpmin =
    y + ddx2GetHeight(hdcLI.hdcListTop) + ddx2GetHeight(hdcLI.hdcListUp) + 12;
  p_li->mpmax =
    y + hight - ddx2GetHeight(hdcLI.hdcListBottom) -
    ddx2GetHeight(hdcLI.hdcListDown) - 12;

  p_li->rectMoverA.left = p_li->rectMover.left;
  p_li->rectMoverA.right = p_li->rectMover.right;
  p_li->rectMoverA.top =
    y + ddx2GetHeight(hdcLI.hdcListTop) + ddx2GetHeight(hdcLI.hdcListUp) + 12;
  p_li->rectMoverA.bottom =
    y + hight - ddx2GetHeight(hdcLI.hdcListBottom) -
    ddx2GetHeight(hdcLI.hdcListDown) - 12;


  return p_li;
}

int co2_Rect_Hit(RECT rect, int x, int y)
{
  if (x >= rect.left && x <= rect.right && y >= rect.top && y <= rect.bottom)
    return 1;
  else
    return 0;
}

void co2_Del_Combo_List(COMBO_CONTROL2 * p_co, int hdc, int xcor, int ycor)
{
  int y = ycor + p_co->y + ddx2GetHeight(hdcCO.hdcCombo);
  int i;

  for (i = 0; i < p_co->CounfOfItemsL; i++) {
    /*BitBlt(hdc, p_co->x + xcor, y, 
       p_co->coListRect.right - p_co->coListRect.left - 2, _2dd.bitmap[hdcCO.hdcComboMid].bitmap.bmHeight,
       _2dd.bitmap[hdcCO.hdcComboMid].bitmapDC, 0, 0, SRCCOPY); */

    ddx2BitBlt(hdc, p_co->x + xcor, y,
      p_co->coListRect.right - p_co->coListRect.left - 2,
      ddx2GetHeight(hdcCO.hdcComboMid), hdcCO.hdcComboMid, 0, 0);

    y += ddx2GetHeight(hdcCO.hdcComboMid);
  }
}

int co2_Handle_Combo_Drop(COMBO_DROP_CONTROL2 * p_co, char bFocus, int x,
  int y, int hdc, int xcor, int ycor)
{
  int c = 0;

  if (mi.t1)
    if (co2_Rect_Hit(p_co->coLUp, x, y)) {
      p_co->Selected--;

      if (p_co->Selected < 0)
        p_co->Selected = 0;

      co2_Combo_Drop_Set_String(hdc, p_co, xcor, ycor);

      c++;
    }

  if (mi.t1)
    if (co2_Rect_Hit(p_co->coLDown, x, y)) {
      p_co->Selected++;

      if (p_co->Selected >= p_co->CounfOfItems)
        p_co->Selected = p_co->CounfOfItems - 1;

      co2_Combo_Drop_Set_String(hdc, p_co, xcor, ycor);

      c++;
    }

  return c;
}

int co2_Handle_Combo(COMBO_CONTROL2 * p_co, char bFocus, int x, int y,
  int hdc, int xcor, int ycor, char b_list)
{
  int c = 0;
  RECT r;

  if (mi.t1 && b_list && !p_co->bList)
    return 0;

  if (!mi.dt1)
    p_co->bIn = 0;

  if (mi.t1 || mi.dt1)
    if (co2_Rect_Hit(p_co->coLMovA, x + xcor, y + ycor) && p_co->bList) {
      int ym;
      int ymcor;
      int ymx;
      int i;

    COMBO2_HADLE_MOVER:

      ym = (y + ycor - p_co->xmstart);
      ymcor = (int) floor(ddx2GetHeight(hdcCO.hdcComboMover) / 2.0f);
      ymx = p_co->xm;

      if (ym < 0)
        ym = 0;

      if (ym > ymx)
        ym = ymx;

      i =
        ftoi(((p_co->CounfOfItems - p_co->CounfOfItemsL) * ym) / (float) ymx);

      p_co->SSelected = i;

      p_co->CSelected = p_co->Selected - 1;

      co2_Combo_Draw_List(hdc, p_co, xcor, ycor, 1);

      r.left = p_co->x + xcor;
      r.top = p_co->y + ddx2GetHeight(hdcCO.hdcCombo) + ycor;
      r.right = p_co->WidthR;
      r.bottom = p_co->ListMaxHight + ddx2GetHeight(hdcCO.hdcComboEnd);

      _2d_Add_RectItem(&rline, r, 1);

      if (ym + p_co->xmstart - ymcor != p_co->coLMov.top) {
        int t, b;

        t = ym + p_co->xmstart - ymcor;
        b = p_co->coLMov.top + ddx2GetHeight(hdcCO.hdcComboMover);

        ddx2BitBlt(hdc, p_co->coLMov.left, p_co->coLMov.top,
          ddx2GetWidth(hdcCO.hdcComboMoverb),
          ddx2GetHeight(hdcCO.hdcComboMoverb), hdcCO.hdcComboMoverb, 0, 0);

        ddx2BitBlt(hdc, p_co->coLMov.left, t,
          ddx2GetWidth(hdcCO.hdcComboMover),
          ddx2GetHeight(hdcCO.hdcComboMover), hdcCO.hdcComboMover, 0, 0);

        p_co->coLMov.top = t;
        p_co->coLMov.bottom = b;

      }

      p_co->bIn = 1;
      c++;
      return c;
    }
    else if (mi.dt1 && p_co->bIn)
      goto COMBO2_HADLE_MOVER;

  if (mi.t1)
    if (co2_Rect_Hit(p_co->coLUp, x + xcor, y + ycor) && p_co->bList) {
      int ymcor = (int) floor(ddx2GetHeight(hdcCO.hdcComboMover) / 2.0f);
      int t, b;
      int ny;

      p_co->SSelected--;

      if (p_co->SSelected < 0)
        p_co->SSelected = 0;

      p_co->CSelected = p_co->Selected - 1;

      co2_Del_Combo_List(p_co, hdc, xcor, ycor);
      co2_Combo_Draw_List(hdc, p_co, xcor, ycor, 0);

      ny =
        ftoi((p_co->xm / (float) (p_co->CounfOfItems -
            p_co->CounfOfItemsL)) * p_co->SSelected);

      t = ny + p_co->xmstart - ymcor;
      b = p_co->coLMov.top + ddx2GetHeight(hdcCO.hdcComboMover);

      ddx2BitBlt(hdc, p_co->coLMov.left, p_co->coLMov.top,
        ddx2GetWidth(hdcCO.hdcComboMoverb),
        ddx2GetHeight(hdcCO.hdcComboMoverb), hdcCO.hdcComboMoverb, 0, 0);

      ddx2BitBlt(hdc, p_co->coLMov.left, t, ddx2GetWidth(hdcCO.hdcComboMover),
        ddx2GetHeight(hdcCO.hdcComboMover), hdcCO.hdcComboMover, 0, 0);

      p_co->coLMov.top = t;
      p_co->coLMov.bottom = b;

      c++;
    }

  if (mi.t1)
    if (co2_Rect_Hit(p_co->coLDown, x + xcor, y + ycor) && p_co->bList) {
      int ymcor = (int) floor(ddx2GetHeight(hdcCO.hdcComboMover) / 2.0f);
      int t, b;
      int ny;

      p_co->SSelected++;

      if (p_co->SSelected > p_co->CounfOfItems - p_co->CounfOfItemsL)
        p_co->SSelected = p_co->CounfOfItems - p_co->CounfOfItemsL;

      p_co->CSelected = p_co->Selected - 1;

      co2_Del_Combo_List(p_co, hdc, xcor, ycor);
      co2_Combo_Draw_List(hdc, p_co, xcor, ycor, 0);

      ny =
        ftoi((p_co->xm / (float) (p_co->CounfOfItems -
            p_co->CounfOfItemsL)) * p_co->SSelected);

      t = ny + p_co->xmstart - ymcor;
      b = p_co->coLMov.top + ddx2GetHeight(hdcCO.hdcComboMover);

      ddx2BitBlt(hdc, p_co->coLMov.left, p_co->coLMov.top,
        ddx2GetWidth(hdcCO.hdcComboMoverb),
        ddx2GetHeight(hdcCO.hdcComboMoverb), hdcCO.hdcComboMoverb, 0, 0);

      ddx2BitBlt(hdc, p_co->coLMov.left, t, ddx2GetWidth(hdcCO.hdcComboMover),
        ddx2GetHeight(hdcCO.hdcComboMover), hdcCO.hdcComboMover, 0, 0);

      p_co->coLMov.top = t;
      p_co->coLMov.bottom = b;

      c++;
    }

  if (mi.t1)
    if (co2_Rect_Hit(p_co->coDownRect, x, y)) {
      r.left = p_co->x + xcor;
      r.top = p_co->y + ddx2GetHeight(hdcCO.hdcCombo) + ycor;
      r.right = p_co->WidthR;
      r.bottom = p_co->ListMaxHight + ddx2GetHeight(hdcCO.hdcComboEnd);

      _2d_Add_RectItem(&rline, r, 1);

      if (p_co->bList) {
        co2_Combo_Close(hdc, p_co, xcor, ycor);
        p_co->Selected = p_co->OSelected;
      }
      else {
        p_co->OSelected = p_co->Selected;
        co2_Combo_Open(hdc, p_co, xcor, ycor);
      }

      r.left = p_co->x + xcor;
      r.top = p_co->y + ddx2GetHeight(hdcCO.hdcCombo) + ycor;
      r.right = p_co->WidthR;
      r.bottom = p_co->ListMaxHight + ddx2GetHeight(hdcCO.hdcComboEnd);

      _2d_Add_RectItem(&rline, r, 1);

      c++;
    }

  if (co2_Rect_Hit(p_co->coListRect, x, y) && p_co->bList) {
    c++;

    p_co->CSelected = p_co->SSelected + co2_Combo_Hit2Sel(p_co, x, y);

    if (!mi.t1 && p_co->bList) {
      co2_Combo_Draw_List(hdc, p_co, xcor, ycor, 0);
      p_co->Selected = p_co->CSelected;
    }
    else if (mi.t1 && p_co->bList) {
      p_co->OSelected = p_co->Selected = p_co->CSelected;
      co2_Combo_Set_String(hdc, p_co, xcor, ycor);
      co2_Combo_Close(hdc, p_co, xcor, ycor);
    }
  }
  else if (!mi.t1 && p_co->bList) {
    c++;
    p_co->CSelected = -1;
    co2_Combo_Draw_List(hdc, p_co, xcor, ycor, 0);
    p_co->Selected = -1;
  }


  if (mi.t1)
    if (!c && bFocus) {
      r.left = p_co->x + xcor;
      r.top = p_co->y + ddx2GetHeight(hdcCO.hdcCombo) + ycor;
      r.right = p_co->WidthR;
      r.bottom = p_co->ListMaxHight + ddx2GetHeight(hdcCO.hdcComboEnd);

      //_2d_Add_RectItem(&rline, r, 1);

      co2_Combo_Close(hdc, p_co, xcor, ycor);
      p_co->Selected = p_co->OSelected;

      r.left = p_co->x + xcor;
      r.top = p_co->y + ddx2GetHeight(hdcCO.hdcCombo) + ycor;
      r.right = p_co->WidthR;
      r.bottom = p_co->ListMaxHight + ddx2GetHeight(hdcCO.hdcComboEnd);

      //_2d_Add_RectItem(&rline, r, 1);
    }

  r.left = p_co->x + xcor;
  r.top = p_co->y + ddx2GetHeight(hdcCO.hdcCombo) + ycor;
  r.right = p_co->WidthR;
  r.bottom = p_co->ListMaxHight + ddx2GetHeight(hdcCO.hdcComboEnd);

  //_2d_Add_RectItem_IfNPresent(&rline, r, 1);

  r.left = p_co->x + xcor;
  r.top = p_co->y + ycor;
  r.right = ddx2GetWidth(hdcCO.hdcCombo);
  r.bottom = ddx2GetHeight(hdcCO.hdcCombo);

  //_2d_Add_RectItem_IfNPresent(&rline, r, 1);


  return c;
}

int co2_Handle_Button(BUTTON_CONTROL2 * p_bu, int x, int y)
{
  if (co2_Rect_Hit(p_bu->Rect, x, y)) {
    p_bu->bActivated = 1;
    return 1;
  }

  return 0;
}

int co2_Handle_Checkbox(CHECKBOX_CONTROL2 * p_ch, int x, int y)
{
  int bmpx, bmpy;
  int bmpDC;

  if (co2_Rect_Hit(p_ch->Rect, x, y)) {
    bmpx = ddx2GetWidth(hdcCH.hdcCheck);
    bmpy = ddx2GetHeight(hdcCH.hdcCheck);
    bmpDC = hdcCH.hdcCheck;

    if (!p_ch->bChecked) {
      p_ch->bChecked = 1;
      //TransparentBltU(_2dd.hDC, p_ch->Rect.left + TAB_X, p_ch->Rect.top + TAB_Y, bmpx, 24, bmpDC, 0, 54, bmpx, 24, RGB(237, 77, 0));
      ddx2TransparentBltDisplay(p_ch->Rect.left + TAB_X,
        p_ch->Rect.top + TAB_Y, bmpx, 20, bmpDC, 0, 25, bmpx, 20, RGB(237, 77,
          0));
      p_ch->bChange = 1;

      if (p_ch->iDC != -1)
        ddx2BitBlt(p_ch->iDC, 0, 0, p_ch->RectFull.right,
          p_ch->RectFull.bottom, HDC2DD, p_ch->x + TAB_X, p_ch->y + TAB_Y);

      return 1;
    }
    else {
      p_ch->bChecked = 0;
      //TransparentBltU(_2dd.hDC, p_ch->Rect.left + TAB_X, p_ch->Rect.top + TAB_Y, bmpx, 24, bmpDC, 0, 80, bmpx, 24, RGB(237, 77, 0));
      ddx2TransparentBltDisplay(p_ch->Rect.left + TAB_X,
        p_ch->Rect.top + TAB_Y, bmpx, 20, bmpDC, 0, 1, bmpx, 20, RGB(237, 77,
          0));
      p_ch->bChange = 1;

      if (p_ch->iDC != -1)
        ddx2BitBlt(p_ch->iDC, 0, 0, p_ch->RectFull.right,
          p_ch->RectFull.bottom, HDC2DD, p_ch->x + TAB_X, p_ch->y + TAB_Y);

      return 1;
    }
  }

  p_ch->bChange = 0;
  return 0;
}

int co2_Handle_Progres(PROGRES_CONTROL2 * p_pr, int x, int y)
{
  if (bExclusive && !p_pr->bExclusive)
    return 0;

  if (mi.t1 || mi.dt1)
    if (co2_Rect_Hit(p_pr->rectProgres, x, y)) {
      if (p_pr->pos == x)
        return 1;

      ddx2BitBltDisplay(p_pr->rectMover.left + TAB_X,
        p_pr->rectMover.top + TAB_Y, ddx2GetWidth(p_pr->bDC),
        ddx2GetHeight(p_pr->bDC), p_pr->bDC, 0, 0);

      p_pr->rectMover.left = x - p_pr->cor;
      p_pr->rectMover.right = (x - p_pr->cor) + ddx2GetWidth(hdcPR.hdcMover);

      ddx2BitBlt(p_pr->bDC, 0, 0, ddx2GetWidth(p_pr->bDC),
        ddx2GetHeight(p_pr->bDC), HDC2DD, p_pr->rectMover.left + TAB_X,
        p_pr->rectMover.top + TAB_Y);

      /*ddx2BitBltDisplay(p_pr->rectMover.left + TAB_X, p_pr->rectMover.top + TAB_Y, 
         ddx2GetWidth(hdcPR.hdcMover),  ddx2GetHeight(hdcPR.hdcMover), 
         hdcPR.hdcMover, 0, 0); */

      ddx2TransparentBltDisplay(p_pr->rectMover.left + TAB_X,
        p_pr->rectMover.top + TAB_Y, ddx2GetWidth(hdcPR.hdcMover),
        ddx2GetHeight(hdcPR.hdcMover), hdcPR.hdcMover, 0, 0,
        ddx2GetWidth(hdcPR.hdcMover), ddx2GetHeight(hdcPR.hdcMover), RGB(237,
          77, 0));

      p_pr->pos = x;
      p_pr->bIn = 1;
      p_pr->bChange = 1;
      p_pr->bExclusive = 1;
      bExclusive = 1;
      return 1;
    }

  if (p_pr->bIn && mi.dt1) {
/*		if(y > p_pr->rectProgres.bottom || y < p_pr->rectProgres.top)
		{
			p_pr->bIn = 0;
			return 0;
		}*/

    if (x >= p_pr->rectProgres.right)
      x = p_pr->rectProgres.right;

    if (x <= p_pr->rectProgres.left)
      x = p_pr->rectProgres.left;

    if (p_pr->pos == x)
      return 1;

    p_pr->pos = x;

    ddx2BitBltDisplay(p_pr->rectMover.left + TAB_X,
      p_pr->rectMover.top + TAB_Y, ddx2GetWidth(p_pr->bDC),
      ddx2GetHeight(p_pr->bDC), p_pr->bDC, 0, 0);

    p_pr->rectMover.left = x - p_pr->cor;
    p_pr->rectMover.right = (x - p_pr->cor) + ddx2GetWidth(hdcPR.hdcMover);

    ddx2BitBlt(p_pr->bDC, 0, 0, ddx2GetWidth(p_pr->bDC),
      ddx2GetHeight(p_pr->bDC), HDC2DD, p_pr->rectMover.left + TAB_X,
      p_pr->rectMover.top + TAB_Y);

    /*ddx2BitBltDisplay(p_pr->rectMover.left + TAB_X, p_pr->rectMover.top + TAB_Y, 
       ddx2GetWidth(hdcPR.hdcMover),  ddx2GetHeight(hdcPR.hdcMover), 
       hdcPR.hdcMover, 0, 0); */

    ddx2TransparentBltDisplay(p_pr->rectMover.left + TAB_X,
      p_pr->rectMover.top + TAB_Y, ddx2GetWidth(hdcPR.hdcMover),
      ddx2GetHeight(hdcPR.hdcMover), hdcPR.hdcMover, 0, 0,
      ddx2GetWidth(hdcPR.hdcMover), ddx2GetHeight(hdcPR.hdcMover), RGB(237,
        77, 0));

    //p_pr->bIn = 0;
    p_pr->bChange = 1;
    p_pr->bExclusive = 1;
    bExclusive = 1;
    return 1;
  }

  if (!mi.dt1) {
    p_pr->bIn = 0;
    p_pr->bExclusive = 0;
    bExclusive = 0;
  }

  p_pr->bChange = 0;

  return 0;
}

int co2_List_Get_Dbclck(CONTROL_LIST_ITEM2 * p_list, int lsize, int id,
  LIST_VIEW_CONTROL2 ** p_li)
{
  int i;

  for (i = 0; i < lsize; i++)
    if ((p_list + i)->p_list && (p_list + i)->bActive) {
      *p_li = (p_list + i)->p_list;
      return (p_list + i)->p_list->bDblClck;
    }

  return -1;
}

int co2_List_Get_Clck(CONTROL_LIST_ITEM2 * p_list, int lsize, int id,
  LIST_VIEW_CONTROL2 ** p_li)
{
  int i;

  for (i = 0; i < lsize; i++)
    if ((p_list + i)->p_list && (p_list + i)->bActive) {
      *p_li = (p_list + i)->p_list;
      return (p_list + i)->p_list->bClck;
    }

  return -1;
}

int co2_List_Get_Value(CONTROL_LIST_ITEM2 * p_list, int lsize, int id,
  int index)
{
  int i;

  for (i = 0; i < lsize; i++)
    if ((p_list + i)->p_list)
      return (p_list + i)->p_list->piValue[index];

  return -1;
}

void co2_get_XP_XT(LIST_VIEW_CONTROL2 * p_li, int i, int *p_xp, int *p_xt,
  int ycor)
{
  int xs = p_li->rectList.top + ycor + (i * 30) - p_li->dx;
  int xp = 0;
  int xt = 0;

  if (mi.t1)
    p_li->cSelected = i;

  if ((i * 30) - p_li->dx < 0)
    xp = ((i * 30) - p_li->dx) * -1;

  if ((p_li->rectList.top + ycor + (i * 30) - p_li->dx + 30) >
    p_li->rectList.bottom + ycor)
    xt =
      (p_li->rectList.top + ycor + (i * 30) - p_li->dx + 30) -
      (p_li->rectList.bottom + ycor);

  *p_xp = xp;
  *p_xt = xt;

  return;
}

int co2_Handle_List(LIST_VIEW_CONTROL2 * p_li, int x, int y, int hdc,
  int xcor, int ycor)
{
  int xp = 0;
  int xt = 0;

  RECT r;
  DWORD t = timeGetTime();

  if (bBlockList)
    return 0;

  r.left = p_li->rectList.left + xcor;
  r.top = p_li->rectList.top + ycor;
  r.right = ddx2GetWidth(p_li->bDCn);
  r.bottom = p_li->rectList.bottom - p_li->rectList.top;

  //_2d_Add_RectItem_IfNPresent(&rline, r, 1);

  if (mi.dt1 && p_li->bIn > 0) {
    if (y < p_li->mpmin)
      y = p_li->mpmin;

    if (y > p_li->mpmax)
      y = p_li->mpmax;

    p_li->bIn = 0;
    goto HANDLE_LISTVIEW;
  }

  if (!mi.dt1)
    p_li->bIn = 0;

  if (p_li->Pushc < 2)
    if (t - p_li->pTime > 1000) {
      p_li->pTime = t;
      p_li->Pushc = 0;
      p_li->cSelected = -1;
      p_li->bClck = 0;
      p_li->bDblClck = 0;
    }

  if (co2_Rect_Hit(p_li->rectList, x, y)) {
    int i = (int) floor((p_li->dx + (y - p_li->rectList.top)) / 30.0f);

    if (mi.t1) {
      p_li->bClck++;
      p_li->Pushc++;
    }

    if (i < 0 || i > p_li->listnum) {
      ddx2BitBlt(hdc, p_li->rectList.left + xcor, p_li->rectList.top + ycor,
        ddx2GetWidth(p_li->bDCn), p_li->rectList.bottom - p_li->rectList.top,
        p_li->bDCn, 0, p_li->dx);

      p_li->Selected = -1;
      p_li->Pushc = 0;
      p_li->pTime = t;

      if (mi.t1) {
        p_li->cSelected = -1;
        p_li->cClckSel = -1;
      }
    }
    else if (i != p_li->Selected) {
      if (mi.t1)
        p_li->cSelected = i;

      co2_get_XP_XT(p_li, i, &xp, &xt, ycor);

      ddx2BitBlt(hdc, p_li->rectList.left + xcor, p_li->rectList.top + ycor,
        ddx2GetWidth(p_li->bDCn), p_li->rectList.bottom - p_li->rectList.top,
        p_li->bDCn, 0, p_li->dx);

      p_li->Selected = i;

      ddx2BitBlt(hdc, p_li->rectList.left + xcor,
        p_li->rectList.top + ycor + (i * 30) - p_li->dx + xp,
        ddx2GetWidth(p_li->bDCs), 30 - xp - xt, p_li->bDCs, 0, (i * 30) + xp);

      if (p_li->cClckSel != -1 && p_li->bSelection) {
        co2_get_XP_XT(p_li, p_li->cClckSel, &xp, &xt, ycor);

        ddx2BitBlt(hdc, p_li->rectList.left + xcor,
          p_li->rectList.top + ycor + (p_li->cClckSel * 30) - p_li->dx + xp,
          ddx2GetWidth(p_li->bDCs), 30 - xp - xt, p_li->bDCs, 0,
          (p_li->cClckSel * 30) + xp);
      }
    }
    else if (mi.t1) {
      if (p_li->Pushc > 1) {
        if (p_li->cSelected == i)
          p_li->bDblClck = 1;

        p_li->Pushc = 0;
        p_li->pTime = t;
      }
      else
        p_li->cSelected = i;

      if (p_li->Pushc == 1)
        p_li->bClck = 1;

      p_li->cClckSel = i;

      if (p_li->bSelection) {
        ddx2BitBlt(hdc, p_li->rectList.left + xcor, p_li->rectList.top + ycor,
          ddx2GetWidth(p_li->bDCn),
          p_li->rectList.bottom - p_li->rectList.top, p_li->bDCn, 0,
          p_li->dx);

        co2_get_XP_XT(p_li, p_li->cClckSel, &xp, &xt, ycor);

        ddx2BitBlt(hdc, p_li->rectList.left + xcor,
          p_li->rectList.top + ycor + (p_li->cClckSel * 30) - p_li->dx + xp,
          ddx2GetWidth(p_li->bDCs), 30 - xp - xt, p_li->bDCs, 0,
          (p_li->cClckSel * 30) + xp);
      }

    }
  }
  else if (p_li->Selected != -1) {
    p_li->Selected = -1;

    ddx2BitBlt(hdc, p_li->rectList.left + xcor, p_li->rectList.top + ycor,
      ddx2GetWidth(p_li->bDCn), p_li->rectList.bottom - p_li->rectList.top,
      p_li->bDCn, 0, p_li->dx);

    if (p_li->cClckSel != -1 && p_li->bSelection) {
      co2_get_XP_XT(p_li, p_li->cClckSel, &xp, &xt, ycor);

      ddx2BitBlt(hdc, p_li->rectList.left + xcor,
        p_li->rectList.top + ycor + (p_li->cClckSel * 30) - p_li->dx + xp,
        ddx2GetWidth(p_li->bDCs), 30 - xp - xt, p_li->bDCs, 0,
        (p_li->cClckSel * 30) + xp);
    }
  }

  if (mi.t1)
    if (co2_Rect_Hit(p_li->rectUp, x, y)) {
      float dcm =
        (float) ddx2GetHeight(p_li->bDCn) - (p_li->rectList.bottom -
        p_li->rectList.top);
      float c = (p_li->rectMoverA.bottom - p_li->rectMoverA.top) / dcm;

      y = p_li->mpos - ftoi(30 * c);

      p_li->bInE = -1;
      p_li->bIn = -1;

      goto HANDLE_LISTVIEW;
    }

  if (mi.t1)
    if (co2_Rect_Hit(p_li->rectDown, x, y)) {
      float dcm =
        (float) ddx2GetHeight(p_li->bDCn) - (p_li->rectList.bottom -
        p_li->rectList.top);
      float c = (p_li->rectMoverA.bottom - p_li->rectMoverA.top) / dcm;

      y = p_li->mpos + ftoi(30 * c);

      p_li->bInE = -1;
      p_li->bIn = -1;
      goto HANDLE_LISTVIEW;
    }

  if (mi.t1 || mi.dt1)
    if (co2_Rect_Hit(p_li->rectMoverA, x, y)) {
      int dcm, mm, pm, pos;

    HANDLE_LISTVIEW:

      if (y > p_li->mpmax)
        y = p_li->mpmax;
      else if (y < p_li->mpmin)
        y = p_li->mpmin;

      if (p_li->mpos == y) {
        p_li->bIn++;
        return 1;
      }

      p_li->mpos = y;

      ddx2BitBlt(hdc, p_li->rectMover.left + xcor, p_li->rectMover.top + ycor,
        ddx2GetWidth(p_li->bDCm), ddx2GetHeight(p_li->bDCm), p_li->bDCm, 0,
        0);

      p_li->rectMover.top = p_li->mpos - 12;
      p_li->rectMover.bottom = p_li->mpos + 12;

      ddx2BitBlt(p_li->bDCm, 0, 0, ddx2GetWidth(p_li->bDCm),
        ddx2GetHeight(p_li->bDCm), hdc, p_li->rectMover.left + xcor,
        p_li->rectMover.top + ycor);

      ddx2TransparentBlt(hdc, p_li->rectMover.left + xcor,
        p_li->rectMover.top + ycor,
        ddx2GetWidth(hdcCH.hdcCheck),
        20,
        hdcCH.hdcCheck,
        0, 49, ddx2GetWidth(hdcCH.hdcCheck), 20, RGB(237, 77, 0));

      dcm =
        ddx2GetHeight(p_li->bDCn) - (p_li->rectList.bottom -
        p_li->rectList.top);
      mm = p_li->mpmax - p_li->mpmin;
      pm = p_li->mpos - p_li->mpmin;

      pos = ftoi((pm * dcm) / (float) mm);

      if (p_li->mpos == p_li->mpmax)
        pos =
          ddx2GetHeight(p_li->bDCn) - (p_li->rectList.bottom -
          p_li->rectList.top);

      if (p_li->mpos == p_li->mpmin)
        pos = 0;

      p_li->dx = pos;

      ddx2BitBlt(hdc, p_li->rectList.left + xcor, p_li->rectList.top + ycor,
        ddx2GetWidth(p_li->bDCn), p_li->rectList.bottom - p_li->rectList.top,
        p_li->bDCn, 0, pos);

      if (p_li->cClckSel != -1 && p_li->bSelection) {
        co2_get_XP_XT(p_li, p_li->cClckSel, &xp, &xt, ycor);

        ddx2BitBlt(hdc, p_li->rectList.left + xcor,
          p_li->rectList.top + ycor + (p_li->cClckSel * 30) - p_li->dx + xp,
          ddx2GetWidth(p_li->bDCs), 30 - xp - xt, p_li->bDCs, 0,
          (p_li->cClckSel * 30) + xp);
      }

      p_li->bIn++;
      p_li->bInE++;
      return 1;
    }


  r.left = p_li->rectList.left + xcor;
  r.top = p_li->rectList.top + ycor;
  r.right = ddx2GetWidth(p_li->bDCn);
  r.bottom = p_li->rectList.bottom - p_li->rectList.top;

  //_2d_Add_RectItem_IfNPresent(&rline, r, 1);

  if (key[K_DEL]) {
    char *cfile = NULL;

    key[K_DEL] = 0;

    if (co2_List_Delete_Item(p_li, &cfile)) {
      p_li->dx = co2_List_Redraw(hdc, p_li, p_li->dx);
      co_delete(cfile);
    }

    if (cfile)
      free((void *) cfile);
  }

  return 0;
}

int co2_Handle_List_View(LIST_VIEW2_CONTROL2 * p_li, int x, int y, int hdc,
  int xcor, int ycor)
{
  RECT r;

  if (bBlockList)
    return 0;

  r.left = p_li->rectList.left + xcor;
  r.top = p_li->rectList.top + ycor;
  r.right = ddx2GetWidth(p_li->bDC);
  r.bottom = p_li->rectList.bottom - p_li->rectList.top;

  //_2d_Add_RectItem_IfNPresent(&rline, r, 1);

  if ((mi.t1 && co2_Rect_Hit(p_li->rectUp, x, y)) || key[K_PLUS]
    || key[K_NAHORU]) {
    //float dcm = (float)ddx2GetHeight(p_li->bDC) - (p_li->rectList.bottom - p_li->rectList.top);
    //float c = (p_li->rectMoverA.bottom - p_li->rectMoverA.top) / dcm;

    p_li->bIn = -1;
    p_li->bInE = -1;

    key[K_PLUS] = 0;
    key[K_NAHORU] = 0;

    p_li->dx -= 30;

    if (p_li->dx < 0)
      p_li->dx = 0;

    //goto HANDLE_LISTVIEW;
    co2_Set_List_View_List_Pos(p_li, p_li->dx, hdc, xcor, ycor);
    return 1;
  }

  if ((mi.t1 && co2_Rect_Hit(p_li->rectDown, x, y)) || key[K_MINUS]
    || key[K_DOLU]) {
    /*float dcm = (float)ddx2GetHeight(p_li->bDC) - (p_li->rectList.bottom - p_li->rectList.top);
       float c = (p_li->rectMoverA.bottom - p_li->rectMoverA.top) / dcm; */

    p_li->bIn = -1;
    p_li->bInE = -1;

    key[K_MINUS] = 0;
    key[K_DOLU] = 0;

    p_li->dx += 30;

    if (p_li->dx >
      ddx2GetHeight(p_li->bDC) - (p_li->rectList.bottom - p_li->rectList.top))
      p_li->dx =
        ddx2GetHeight(p_li->bDC) - (p_li->rectList.bottom -
        p_li->rectList.top);

    co2_Set_List_View_List_Pos(p_li, p_li->dx, hdc, xcor, ycor);

    //goto HANDLE_LISTVIEW;
    return 1;
  }

  if (mi.t1 || mi.dt1)
    if (co2_Rect_Hit(p_li->rectMoverA, x, y)) {
      int dcm, mm, pm, pos;

    HANDLE_LISTVIEW:

      if (y > p_li->mpmax)
        y = p_li->mpmax;
      else if (y < p_li->mpmin)
        y = p_li->mpmin;

      if (p_li->mpos == y) {
        p_li->bIn++;
        return 1;
      }

      p_li->mpos = y;

      ddx2BitBlt(hdc, p_li->rectMover.left + xcor, p_li->rectMover.top + ycor,
        ddx2GetWidth(p_li->bDCm), ddx2GetHeight(p_li->bDCm), p_li->bDCm, 0,
        0);

      p_li->rectMover.top = p_li->mpos - 12;
      p_li->rectMover.bottom = p_li->mpos + 12;

      ddx2BitBlt(p_li->bDCm, 0, 0, ddx2GetWidth(p_li->bDCm),
        ddx2GetHeight(p_li->bDCm), hdc, p_li->rectMover.left + xcor,
        p_li->rectMover.top + ycor);

      ddx2TransparentBlt(hdc, p_li->rectMover.left + xcor,
        p_li->rectMover.top + ycor,
        ddx2GetWidth(hdcCH.hdcCheck),
        20,
        hdcCH.hdcCheck,
        0, 49, ddx2GetWidth(hdcCH.hdcCheck), 20, RGB(237, 77, 0));

      dcm =
        ddx2GetHeight(p_li->bDC) - (p_li->rectList.bottom -
        p_li->rectList.top);
      mm = p_li->mpmax - p_li->mpmin;
      pm = p_li->mpos - p_li->mpmin;

      pos = ftoi((pm * dcm) / (float) mm);

      if (p_li->mpos == p_li->mpmax)
        pos =
          ddx2GetHeight(p_li->bDC) - (p_li->rectList.bottom -
          p_li->rectList.top);

      if (p_li->mpos == p_li->mpmin)
        pos = 0;

      p_li->dx = pos;

      ddx2BitBlt(hdc, p_li->rectList.left + xcor, p_li->rectList.top + ycor,
        ddx2GetWidth(p_li->bDC), p_li->rectList.bottom - p_li->rectList.top,
        p_li->bDC, 0, pos);

      p_li->bIn++;
      p_li->bInE++;
      return 1;
    }

  if (mi.dt1 && p_li->bIn > 0) {
    if (y < p_li->mpmin)
      y = p_li->mpmin;

    if (y > p_li->mpmax)
      y = p_li->mpmax;

    p_li->bIn = 0;
    goto HANDLE_LISTVIEW;
  }

  if (!mi.dt1)
    p_li->bIn = 0;

  r.left = p_li->rectList.left + xcor;
  r.top = p_li->rectList.top + ycor;
  r.right = ddx2GetWidth(p_li->bDC);
  r.bottom = p_li->rectList.bottom - p_li->rectList.top;

  //_2d_Add_RectItem_IfNPresent(&rline, r, 1);

  return 0;
}

int co2_Is_Button_Activated(CONTROL_LIST_ITEM2 * p_list, int lsize, int id)
{
  int i;

  for (i = 0; i < lsize; i++)
    if ((p_list + i)->p_button)
      if ((p_list + i)->p_button->ButtonID == id
        && (p_list + i)->p_button->bActivated)
        return 1;

  return 0;
}

int co2_Check_Get_State(CONTROL_LIST_ITEM2 * p_list, int lsize, int id)
{
  int i;

  for (i = 0; i < lsize; i++)
    if ((p_list + i)->p_check)
      if ((p_list + i)->p_check->checkID == id)
        return (p_list + i)->p_check->bChecked;

  return 0;
}


int co2_Handle_wsclen(CONTROL_EDIT2 * p_ed)
{
  if (p_ed->wtext[wcslen(p_ed->wtext) - 1] == 95) {
    if (wcslen(p_ed->wtext) > 29)
      return 1;

    if (p_ed->tx > p_ed->rect.right - p_ed->rect.left - 30)
      return 1;
  }
  else {
    if (wcslen(p_ed->wtext) > 30)
      return 1;

    if (p_ed->tx > p_ed->rect.right - p_ed->rect.left - 30)
      return 1;
  }


  return 0;
}

int co2_Handle_Edit(CONTROL_EDIT2 * p_ed, int x, int y, int hdc, int xcor,
  int ycor)
{
//      RECT r;
  DWORD t = timeGetTime();

  if (mi.t1)
    if (co2_Rect_Hit(p_ed->rect, x, y)) {
      if (!p_ed->bActive) {
        p_ed->pTime = t;
        p_ed->bcActive = 0;
      }

      p_ed->bActive = 1;
      return 1;
    }
    else {
      if (p_ed->wtext[wcslen(p_ed->wtext) - 1] == 95)
        p_ed->wtext[wcslen(p_ed->wtext) - 1] = 0;

      p_ed->bActive = 0;
      goto co2_HANDLE_DRAW;
      return 0;
    }

  if (p_ed->bActive) {
    if (key[0] && key_pressed) {
      int xt, yt;
      WCHAR wt[2];

      if (co_Handle_Edit_Key_Filter() || (co2_Handle_wsclen(p_ed)
          && !key[K_BKSP])) {
        memset(key, 0, POCET_KLAVES * sizeof(char));
        return 1;
      }

      memset(wt, 0, 2 * sizeof(WCHAR));

      MultiByteToWideChar(CP_ACP, 0, (char *) &key_pressed, 1, wt,
        sizeof(wt) / sizeof(wt[0]));

      //wt[0] = key_pressed;
      wt[0] = co_ToUnicode(key_pressed);

      if (!wt[0]) {
        memset(key, 0, POCET_KLAVES * sizeof(char));
        return 1;
      }

      if (wt[0] == '_') {
        memset(key, 0, POCET_KLAVES * sizeof(char));
        return 1;
      }

      if (key[K_ENTER]) {
        if (p_ed->wtext[wcslen(p_ed->wtext) - 1] == 95)
          p_ed->wtext[wcslen(p_ed->wtext) - 1] = 0;

        p_ed->bActive = 0;

        goto co2_HANDLE_DRAW;
      }

      if (key[K_BKSP]) {
        if (wcslen(p_ed->wtext)) {
          if (p_ed->wtext[wcslen(p_ed->wtext) - 1] != 95)
            p_ed->wtext[wcslen(p_ed->wtext) - 1] = 0;
          else if (wcslen(p_ed->wtext) > 1) {
            p_ed->wtext[wcslen(p_ed->wtext) - 1] = 0;
            p_ed->wtext[wcslen(p_ed->wtext) - 1] = 95;
          }
          else {
            p_ed->wtext[0] = 95;
            p_ed->wtext[1] = 0;
          }
        }
      }
      else {
        if (!wcslen(p_ed->wtext))
          wcscat(p_ed->wtext, wt);
        else if (p_ed->wtext[wcslen(p_ed->wtext) - 1] != 95)
          wcscat(p_ed->wtext, wt);
        else {
          p_ed->wtext[wcslen(p_ed->wtext) - 1] = wt[0];
          p_ed->wtext[wcslen(p_ed->wtext)] = 95;
          p_ed->wtext[wcslen(p_ed->wtext)] = 0;
        }
      }


    co2_HANDLE_DRAW:
      /*BitBlt(hdc, p_ed->rect.left + xcor,  p_ed->rect.top + ycor, 
         _2dd.bitmap[hdcED.hdcEdit].bitmap.bmWidth, _2dd.bitmap[hdcED.hdcEdit].bitmap.bmHeight, 
         _2dd.bitmap[hdcED.hdcEdit].bitmapDC, 0, 0, SRCCOPY); */

      ddx2BitBlt(hdc, p_ed->rect.left + xcor, p_ed->rect.top + ycor,
        ddx2GetWidth(hdcED.hdcEdit), ddx2GetHeight(hdcED.hdcEdit),
        hdcED.hdcEdit, 0, 0);

      ddx2CleareSurface(p_ed->bDC);

      fn2_Draw_MessageA(p_ed->bDC, 0, 3, &b2_3d_font.gt, &b2_3d_font.ts,
        p_ed->wtext, 0, &xt, &yt);

      ddx2TransparentBlt(hdc, p_ed->rect.left + 5 + xcor,
        p_ed->rect.top + 4 + xcor, p_ed->rect.right - p_ed->rect.left - 10,
        p_ed->rect.bottom - p_ed->rect.top - 8, p_ed->bDC, 0, 0,
        p_ed->rect.right - p_ed->rect.left - 10,
        p_ed->rect.bottom - p_ed->rect.top - 8, TRANSCOLOR);

      p_ed->tx = xt;
      memset(key, 0, POCET_KLAVES * sizeof(char));
    }
  }

  if (t - p_ed->pTime > 500 && p_ed->bActive) {
    p_ed->pTime = t;

    if (!p_ed->bcActive) {
      char t[32];
      WCHAR wt[32];
      int xt, yt;

      p_ed->bcActive = 1;
      strcpy(t, "_");
      MultiByteToWideChar(CP_ACP, 0, t, strlen(t) + 1, wt,
        sizeof(wt) / sizeof(wt[0]));

      if (!wcslen(p_ed->wtext))
        wcscat(p_ed->wtext, wt);
      else if (p_ed->wtext[wcslen(p_ed->wtext) - 1] != 95)
        wcscat(p_ed->wtext, wt);

      fn2_Draw_MessageA(p_ed->bDC, 0, 3, &b2_3d_font.gt, &b2_3d_font.ts,
        p_ed->wtext, 0, &xt, &yt);

      ddx2TransparentBlt(hdc, p_ed->rect.left + 5 + xcor,
        p_ed->rect.top + 4 + ycor, p_ed->rect.right - p_ed->rect.left - 10,
        p_ed->rect.bottom - p_ed->rect.top - 8, p_ed->bDC, 0, 0,
        p_ed->rect.right - p_ed->rect.left - 10,
        p_ed->rect.bottom - p_ed->rect.top - 8, TRANSCOLOR);

      p_ed->tx = xt;
    }
    else {
      int xt, yt;

      p_ed->bcActive = 0;

      p_ed->wtext[wcslen(p_ed->wtext) - 1] = 0;


      ddx2BitBlt(hdc, p_ed->rect.left + xcor, p_ed->rect.top + ycor,
        ddx2GetWidth(hdcED.hdcEdit), ddx2GetHeight(hdcED.hdcEdit),
        hdcED.hdcEdit, 0, 0);

      ddx2CleareSurface(p_ed->bDC);

      fn2_Draw_MessageA(p_ed->bDC, 0, 3, &b2_3d_font.gt, &b2_3d_font.ts,
        p_ed->wtext, 0, &xt, &yt);

      ddx2TransparentBlt(hdc, p_ed->rect.left + 5 + xcor,
        p_ed->rect.top + 4 + xcor, p_ed->rect.right - p_ed->rect.left - 10,
        p_ed->rect.bottom - p_ed->rect.top - 8, p_ed->bDC, 0, 0,
        p_ed->rect.right - p_ed->rect.left - 10,
        p_ed->rect.bottom - p_ed->rect.top - 8, TRANSCOLOR);

      p_ed->tx = xt;
    }

  }

/*	r.left = p_ed->x;
	r.top = p_ed->y;
	r.right = ddx2GetWidth(hdcED.hdcEdit);
	r.bottom = ddx2GetHeight(hdcED.hdcEdit);

	_2d_Add_RectItem_IfNPresent(&rline, r, 1);*/

  return 1;
}

char co2_Check_Open_Combo(CONTROL_LIST_ITEM2 * p_list, int lsize)
{
  int i;

  for (i = 0; i < lsize; i++)
    if ((p_list + i)->p_combo && (p_list + i)->bActive)
      if ((p_list + i)->p_combo->bList)
        return 1;

  return 0;
}

void co2_Handle_Controls(CONTROL_LIST_ITEM2 * p_list, int lsize, int x, int y,
  int hdc, int xcor, int ycor)
{
  char c = co2_Check_Open_Combo(p_list, lsize);
  int i;

  for (i = 0; i < lsize; i++) {
    if ((p_list + i)->p_list_v && (p_list + i)->bActive) {
      if (!co2_Handle_List_View((p_list + i)->p_list_v, x, y, hdc, xcor,
          ycor))
        (p_list + i)->bFocus = 0;
      else {
        (p_list + i)->bFocus = 1;
      }
    }

    if ((p_list + i)->p_combo && (p_list + i)->bActive) {
      if (!co2_Handle_Combo((p_list + i)->p_combo, (p_list + i)->bFocus, x, y,
          hdc, xcor, ycor, c) && (p_list + i)->bFocus)
        (p_list + i)->bFocus = 0;
      else
        (p_list + i)->bFocus = 1;
    }

    if (mi.t1)
      if ((p_list + i)->p_combod && (p_list + i)->bActive) {
        if (!co2_Handle_Combo_Drop((p_list + i)->p_combod,
            (p_list + i)->bFocus, x, y, hdc, xcor, ycor))
          (p_list + i)->bFocus = 0;
        else
          (p_list + i)->bFocus = 1;
      }

    if (mi.t1)
      if ((p_list + i)->p_button && (p_list + i)->bActive) {
        int xx = x, yy = y;

        if ((p_list + i)->iTab == -1) {
          xx += TAB_X;
          yy += TAB_Y;
        }

        if (!co2_Handle_Button((p_list + i)->p_button, xx, yy))
          (p_list + i)->bFocus = 0;
        else {
          (p_list + i)->bFocus = 1;
        }
      }

    if (mi.t1)
      if ((p_list + i)->p_check && (p_list + i)->bActive) {
        if (!co2_Handle_Checkbox((p_list + i)->p_check, x, y))
          (p_list + i)->bFocus = 0;
        else {
          (p_list + i)->bFocus = 1;
        }
      }

    //if(mi.t1 || mi.dt1)
    if ((p_list + i)->p_prog && (p_list + i)->bActive) {
      if (!co2_Handle_Progres((p_list + i)->p_prog, x, y))
        (p_list + i)->bFocus = 0;
      else {
        (p_list + i)->bFocus = 1;
      }
    }

    if ((p_list + i)->p_list && (p_list + i)->bActive) {
      if (!co2_Handle_List((p_list + i)->p_list, x, y, hdc, xcor, ycor))
        (p_list + i)->bFocus = 0;
      else {
        (p_list + i)->bFocus = 1;
      }
    }

    if ((p_list + i)->p_edit && (p_list + i)->bActive) {
      if (!co2_Handle_Edit((p_list + i)->p_edit, x, y, hdc, xcor, ycor))
        (p_list + i)->bFocus = 0;
      else {
        (p_list + i)->bFocus = 1;
      }
    }
  }
}

void co2_Handle_Release(CONTROL_LIST_ITEM2 * p_list, int lsize)
{
  int i;

  for (i = 0; i < lsize; i++) {
    if ((p_list + i)->p_combo)
      co2_Release_Combo((p_list + i)->p_combo);

    if ((p_list + i)->p_button)
      co2_Release_Button((p_list + i)->p_button);

    if ((p_list + i)->p_check)
      co2_Release_CheckBox((p_list + i)->p_check);

    if ((p_list + i)->p_prog)
      co2_Release_Progres((p_list + i)->p_prog);

    if ((p_list + i)->p_list)
      co2_Release_List((p_list + i)->p_list);

    if ((p_list + i)->p_edit)
      co2_Release_Edit((p_list + i)->p_edit);

    if ((p_list + i)->p_combod)
      co2_Release_Combo_Drop((p_list + i)->p_combod);

    if ((p_list + i)->p_list_v)
      co2_Release_List_View((p_list + i)->p_list_v);
  }
}
