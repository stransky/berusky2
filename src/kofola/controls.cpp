#include <wchar.h>
#include <stdio.h>

#include "3d_all.h"
#include "Berusky3d_kofola_interface.h"

#include "controls.h"
#include "Apak.h"
#include "2D_graphic.h"
#include "font.h"
#include "menu_def.h"
//#include "2ddx.h"

#define HDC2DD -1

extern B2_FONT b2_2d_font;
extern APAK_HANDLE *pControlsArchive;
extern HDC BackDC;
static char bBlockList = 0;
static char bExclusive = 0;

extern RECT_LINE rline;
extern MOUSE_INFO dim;

HDC_EDIT_CONTROL hdcED;
HDC_COMBO_CONTROL hdcCO;
HDC_LIST_CONTROL hdcLI;
HDC_BUTTON_CONTROL hdcBU;
HDC_CHECKBOX_CONTROL hdcCH;
HDC_PROGRES_CONTROL hdcPR;
HDC_FRAME_CONTROL hdcFR;

void co_Del_Combo_List(COMBO_CONTROL * p_co, int hdc, int xcor, int ycor);

int co_Load_Graphic(int combo_var)
{
  pControlsArchive->pActualNode = pControlsArchive->pRootNode->pNextNode;

  if (!combo_var) {
    hdcCO.hdcCombo = ddxLoadBitmap("combo.bmp", pControlsArchive);
    hdcCO.hdcComboEnd = ddxLoadBitmap("combo_end.bmp", pControlsArchive);
    hdcCO.hdcComboMid = ddxLoadBitmap("combo_mid.bmp", pControlsArchive);
  }
  else {
    hdcCO.hdcCombo = ddxLoadBitmap("combo_l.bmp", pControlsArchive);
    hdcCO.hdcComboEnd = ddxLoadBitmap("combo_end_l.bmp", pControlsArchive);
    hdcCO.hdcComboMid = ddxLoadBitmap("combo_mid_l.bmp", pControlsArchive);
  }

  hdcCO.hdcComboUp = ddxLoadBitmap("combo_up.bmp", pControlsArchive);
  hdcCO.hdcComboDown = ddxLoadBitmap("combo_down.bmp", pControlsArchive);
  hdcCO.hdcComboDrop = ddxLoadBitmap("combo_drop.bmp", pControlsArchive);
  hdcCO.hdcComboMover = ddxLoadBitmap("combo_mover.bmp", pControlsArchive);
  hdcCO.hdcComboMoverb = ddxLoadBitmap("combo_moverb.bmp", pControlsArchive);

  if (!combo_var)
    hdcED.hdcEdit = ddxLoadBitmap("edit.bmp", pControlsArchive);
  else
    hdcED.hdcEdit = ddxLoadBitmap("edit_l.bmp", pControlsArchive);

  hdcLI.hdcListDown = ddxLoadBitmap("list_down.bmp", pControlsArchive);
  hdcLI.hdcListMover = ddxLoadBitmap("list_mover.bmp", pControlsArchive);
  hdcLI.hdcListUp = ddxLoadBitmap("list_up.bmp", pControlsArchive);
  hdcLI.hdcListTop = ddxLoadBitmap("list_top.bmp", pControlsArchive);
  hdcLI.hdcListBottom = ddxLoadBitmap("list_bottom.bmp", pControlsArchive);
  hdcLI.hdcListLeft = ddxLoadBitmap("list_left.bmp", pControlsArchive);
  hdcLI.hdcListRight = ddxLoadBitmap("list_right.bmp", pControlsArchive);
  hdcLI.hdcListTL = ddxLoadBitmap("list_tl.bmp", pControlsArchive);
  hdcLI.hdcListTR = ddxLoadBitmap("list_tr.bmp", pControlsArchive);
  hdcLI.hdcListBL = ddxLoadBitmap("list_bl.bmp", pControlsArchive);
  hdcLI.hdcListBR = ddxLoadBitmap("list_br.bmp", pControlsArchive);

  hdcBU.hdcButtonL = ddxLoadBitmap("button_l.bmp", pControlsArchive);
  hdcBU.hdcButtonS = ddxLoadBitmap("button_s.bmp", pControlsArchive);
  hdcCH.hdcCheck = ddxLoadBitmap("checkbox.bmp", pControlsArchive);
  hdcCH.hdcGray = ddxLoadBitmap("gray.bmp", pControlsArchive);
  hdcPR.hdcLine = ddxLoadBitmap("progres_line.bmp", pControlsArchive);
  hdcPR.hdcMover = ddxLoadBitmap("progres_mover.bmp", pControlsArchive);
  hdcFR.hdcFrame = ddxLoadBitmap("frame.bmp", pControlsArchive);

  bBlockList = 0;
  bExclusive = 0;

  return 1;
}

int co_Release_Graphic(void)
{
  ddxReleaseBitmap(hdcCO.hdcCombo);
  ddxReleaseBitmap(hdcCO.hdcComboEnd);
  ddxReleaseBitmap(hdcCO.hdcComboMid);
  ddxReleaseBitmap(hdcCO.hdcComboDown);
  ddxReleaseBitmap(hdcCO.hdcComboUp);
  ddxReleaseBitmap(hdcCO.hdcComboDrop);
  ddxReleaseBitmap(hdcCO.hdcComboMover);
  ddxReleaseBitmap(hdcCO.hdcComboMoverb);
  zero_memory(&hdcCO,sizeof(hdcCO));

  ddxReleaseBitmap(hdcED.hdcEdit);
  zero_memory(&hdcED,sizeof(hdcED));

  ddxReleaseBitmap(hdcLI.hdcListDown);
  ddxReleaseBitmap(hdcLI.hdcListMover);
  ddxReleaseBitmap(hdcLI.hdcListUp);
  ddxReleaseBitmap(hdcLI.hdcListTop);
  ddxReleaseBitmap(hdcLI.hdcListBottom);
  ddxReleaseBitmap(hdcLI.hdcListLeft);
  ddxReleaseBitmap(hdcLI.hdcListRight);
  ddxReleaseBitmap(hdcLI.hdcListTL);
  ddxReleaseBitmap(hdcLI.hdcListTR);
  ddxReleaseBitmap(hdcLI.hdcListBL);
  ddxReleaseBitmap(hdcLI.hdcListBR);
  zero_memory(&hdcLI,sizeof(hdcLI));
  
  ddxReleaseBitmap(hdcBU.hdcButtonL);
  ddxReleaseBitmap(hdcBU.hdcButtonS);
  zero_memory(&hdcBU, sizeof(hdcBU));
  
  ddxReleaseBitmap(hdcCH.hdcCheck);
  ddxReleaseBitmap(hdcCH.hdcGray);
  zero_memory(&hdcCH,sizeof(hdcCH));
  
  ddxReleaseBitmap(hdcPR.hdcLine);
  ddxReleaseBitmap(hdcPR.hdcMover);
  zero_memory(&hdcPR,sizeof(hdcPR));
  
  ddxReleaseBitmap(hdcFR.hdcFrame);
  zero_memory(&hdcFR,sizeof(hdcFR));
  
  return 1;
}

void co_Combo_Draw(int hdc, COMBO_CONTROL * p_co, int xcor, int ycor)
{
  int i;
  int c = 0;
  int x = p_co->x;
  int y = p_co->y;
//  int width = p_co->Width;
//  int hight = p_co->Hight;
  int maxlisthight = p_co->ListMaxHight;

  int ax = p_co->x, ay = p_co->y, ly;

  //------------------------------------------------------- EDIT BOX ----------------------------------------------
  if (p_co->bEdit) {
    ddxBitBlt(hdc, ax, ay, ddxGetWidth(hdcCO.hdcCombo),
      ddxGetHight(hdcCO.hdcCombo), hdcCO.hdcCombo, 0, 0);
  }

  p_co->coEditRect.top = p_co->y;
  p_co->coEditRect.bottom = p_co->y + ddxGetHight(hdcCO.hdcCombo);
  p_co->coEditRect.left = p_co->x;
  p_co->coEditRect.right = p_co->x + ddxGetWidth(hdcCO.hdcCombo);

  p_co->WidthR = ddxGetWidth(hdcCO.hdcCombo);

  //------------------------------------------------------- EDIT BOX ----------------------------------------------

  //------------------------------------------------------- EDIT BOX DOWN BUTTON ----------------------------------
  /*p_co->coDownRect.top = p_co->coEditRect.top+5;
     p_co->coDownRect.bottom = p_co->coEditRect.bottom - 5;
     p_co->coDownRect.left = p_co->coEditRect.right - 28;
     p_co->coDownRect.right = p_co->coEditRect.right - 5; */

  p_co->coDownRect.top = p_co->y;
  p_co->coDownRect.bottom = p_co->y + ddxGetHight(hdcCO.hdcCombo);
  p_co->coDownRect.left = p_co->x;
  p_co->coDownRect.right = p_co->x + ddxGetWidth(hdcCO.hdcCombo);
  //------------------------------------------------------- EDIT BOX DOWN BUTTON ----------------------------------

  ay += ddxGetHight(hdcCO.hdcCombo);
  ax = p_co->x;
  ly = ay;

  //------------------------------------------------------- EDIT BOX LIST BOX -------------------------------------
  ddxSetFlip(0);

  for (i = 0; i < 2; i++) {
    ay = ly;
    c = 0;

    while (ay < ly + maxlisthight) {
      if (p_co->bList) {
        ddxBitBlt(hdc, ax + xcor, ay + ycor, ddxGetWidth(hdcCO.hdcComboMid),
          ddxGetHight(hdcCO.hdcComboMid), hdcCO.hdcComboMid, 0, 0);

			if(p_co->pItem)
				ddxTransparentBlt(hdc, ax + xcor + 5, ay + ycor + 3, ddxGetWidth(p_co->pItem[c].Norm), ddxGetHight(p_co->pItem[c].Norm),
								  p_co->pItem[c].Norm, 0, 0, ddxGetWidth(p_co->pItem[c].Norm), ddxGetHight(p_co->pItem[c].Norm), TRANSCOLOR);
      }

      c++;

      ay += ddxGetHight(hdcCO.hdcComboMid);
    }

    if (!i)
      DisplayFrame();
  }

  ddxSetFlip(1);

  if (p_co->bList) {
    ddxBitBlt(hdc, ax + xcor, ay + ycor, ddxGetWidth(hdcCO.hdcComboEnd),
      ddxGetHight(hdcCO.hdcComboEnd), hdcCO.hdcComboEnd, 0, 0);
  }

  p_co->ListMaxHightR = ay + ddxGetHight(hdcCO.hdcComboEnd) - ly;

  if (p_co->CounfOfItems > p_co->CounfOfItemsL) {
    ddxBitBlt(hdc, x + ddxGetWidth(hdcCO.hdcCombo) + xcor - 4 -
      ddxGetWidth(hdcCO.hdcComboUp),
      y + ycor + ddxGetHight(hdcCO.hdcCombo),
      ddxGetWidth(hdcCO.hdcComboUp),
      ddxGetHight(hdcCO.hdcComboUp), hdcCO.hdcComboUp, 0, 0);

    ddxBitBlt(hdc, x + ddxGetWidth(hdcCO.hdcCombo) + xcor - 4 -
      ddxGetWidth(hdcCO.hdcComboDown),
      ay + ddxGetHight(hdcCO.hdcComboEnd) + ycor -
      ddxGetHight(hdcCO.hdcComboDown) - 4,
      ddxGetWidth(hdcCO.hdcComboDown),
      ddxGetHight(hdcCO.hdcComboDown), hdcCO.hdcComboDown, 0, 0);

    p_co->coLDown.top = ay + ddxGetHight(hdcCO.hdcComboEnd) + ycor -
      ddxGetHight(hdcCO.hdcComboDown) - 4;
    p_co->coLDown.bottom =
      p_co->coLDown.top + ddxGetHight(hdcCO.hdcComboDown);
    p_co->coLDown.left =
      x + ddxGetWidth(hdcCO.hdcCombo) + xcor - 4 -
      ddxGetWidth(hdcCO.hdcComboDown);
    p_co->coLDown.right =
      p_co->coLDown.left + ddxGetWidth(hdcCO.hdcComboDown);

    p_co->coLUp.top = y + ycor + ddxGetHight(hdcCO.hdcCombo);
    p_co->coLUp.bottom = p_co->coLUp.top + ddxGetHight(hdcCO.hdcComboUp);
    p_co->coLUp.left = x + ddxGetWidth(hdcCO.hdcCombo) + xcor - 4 -
      ddxGetWidth(hdcCO.hdcComboDown);
    p_co->coLUp.right = p_co->coLUp.left + ddxGetWidth(hdcCO.hdcComboUp);

    p_co->coLMov.top = p_co->coLUp.bottom + 1;
    p_co->coLMov.bottom = p_co->coLDown.top + 1;
    p_co->coLMov.left = p_co->coLUp.left;
    p_co->coLMov.right = p_co->coLMov.left + ddxGetWidth(hdcCO.hdcComboMover);

    ddxBitBlt(hdc, p_co->coLMov.left, p_co->coLMov.top,
      ddxGetWidth(hdcCO.hdcComboMover), ddxGetHight(hdcCO.hdcComboMover),
      hdcCO.hdcComboMover, 0, 0);

    p_co->xmstart = p_co->coLMov.top;
    p_co->xm =
      p_co->coLDown.top - p_co->coLMov.top - ddxGetHight(hdcCO.hdcComboMover);
    p_co->xmp =
      p_co->coLMov.top + (int) floor(ddxGetHight(hdcCO.hdcComboMover) / 2.0f);
    p_co->xmstart += (int) floor(ddxGetHight(hdcCO.hdcComboMover) / 2.0f);

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

void co_Combo_Draw_List(int hdc, COMBO_CONTROL * p_co, int xcor, int ycor,
  char bDel)
{
  int i;
  int c = 0 + p_co->SSelected;
  int maxlisthight = p_co->ListMaxHight;

  int ax = p_co->x, ay = p_co->y, ly;

  if (p_co->CSelected == p_co->Selected)
    return;

  ay += ddxGetHight(hdcCO.hdcCombo);
  ly = ay;

  ddxSetFlip(0);
  for (i = 0; i < 2; i++) {
    c = p_co->SSelected;
    ay = ly;

    if (bDel)
      co_Del_Combo_List(p_co, hdc, xcor, ycor);

    while (ay < ly + maxlisthight) {
      if (p_co->bList) {
        if (c == p_co->CSelected && c != p_co->Selected) {
          if (p_co->pItem) {            
             ddxTransparentBlt(hdc, ax + xcor + 5, ay + ycor + 3, ddxGetWidth(p_co->pItem[c].Sel), ddxGetHight(p_co->pItem[c].Sel),
             p_co->pItem[c].Sel, 0, 0, ddxGetWidth(p_co->pItem[c].Sel), ddxGetHight(p_co->pItem[c].Sel), TRANSCOLOR);
          }
        }
        else {
          if (p_co->pItem) {
             ddxTransparentBlt(hdc, ax + xcor + 5, ay + ycor + 3, ddxGetWidth(p_co->pItem[c].Norm), ddxGetHight(p_co->pItem[c].Norm),
             p_co->pItem[c].Norm, 0, 0, ddxGetWidth(p_co->pItem[c].Norm), ddxGetHight(p_co->pItem[c].Norm), TRANSCOLOR);
          }
        }
      }

      c++;

      ay += ddxGetHight(hdcCO.hdcComboMid);
    }

    if (!i)
      DisplayFrame();
  }

  ddxSetFlip(1);
}

int co_Combo_Hit2Sel(COMBO_CONTROL * p_co, int x, int y)
{
  int ay = p_co->y + ddxGetHight(hdcCO.hdcCombo);

  return (int) floor((y - ay) / (float) ddxGetHight(hdcCO.hdcComboMid));
}

void co_Release_Edit(CONTROL_EDIT * p_ed)
{
  ddxReleaseBitmap(p_ed->bDC);

  free((void *) p_ed);
}

void co_Release_Combo(COMBO_CONTROL * p_co)
{
  int i;

  bBlockList = 0;

  if (p_co->pItem) {
    for (i = 0; i < p_co->CounfOfItems; i++) {
      //co_Release_Bitmap(&p_co->pItem[i].Norm);
      ddxReleaseBitmap(p_co->pItem[i].Norm);
      //co_Release_Bitmap(&p_co->pItem[i].Sel);
      ddxReleaseBitmap(p_co->pItem[i].Sel);
    }

    free((void *) p_co->pItem);
  }

  if (p_co->pBDC) {
    //co_Release_Bitmap(p_co->pBDC);
    ddxReleaseBitmap(*p_co->pBDC);
    free((void *) p_co->pBDC);
  }

  free((void *) p_co);
}

COMBO_CONTROL *co_Create_Combo(int hdc, int x, int y, int maxlisthight, int id)
{
  COMBO_CONTROL *p_co = (COMBO_CONTROL *) mmalloc(sizeof(COMBO_CONTROL));
  p_co->comboID = id;
  p_co->x = x;
  p_co->y = y;
  p_co->Hight = ddxGetWidth(hdcCO.hdcCombo);
  p_co->Width = ddxGetHight(hdcCO.hdcCombo);
  p_co->ListMaxHight = maxlisthight;
  p_co->bEdit = 1;
  p_co->bList = 0;
  p_co->pBDC = NULL;
  p_co->Selected = -1;
  p_co->CSelected = -1;
  p_co->SSelected = 0;
  p_co->OSelected = -1;
  p_co->pItem = NULL;
  p_co->CounfOfItems = 0;
  p_co->xm = 0;
  p_co->xmstart = 0;
  p_co->bIn = 0;

  co_Combo_Draw(hdc, p_co, 0, 0);

  return p_co;
}

void co_Release_Combo_Drop(COMBO_DROP_CONTROL * p_co)
{
  int i;

  bBlockList = 0;

  if (p_co->pItem) {
    for (i = 0; i < p_co->CounfOfItems; i++) {
      //co_Release_Bitmap(&p_co->pItem[i].Norm);
      ddxReleaseBitmap(p_co->pItem[i].Norm);
      //co_Release_Bitmap(&p_co->pItem[i].Sel);
      ddxReleaseBitmap(p_co->pItem[i].Sel);
    }

    free((void *) p_co->pItem);
  }

  free((void *) p_co);
}

COMBO_DROP_CONTROL *co_Create_Combo_Drop(int hdc, int x, int y, int id)
{
  int ax, ay;

  COMBO_DROP_CONTROL *p_co = NULL;

  p_co = (COMBO_DROP_CONTROL *) malloc(sizeof(COMBO_DROP_CONTROL));

  if (!p_co)
    return 0;

  ZeroMemory(p_co, sizeof(COMBO_DROP_CONTROL));

  p_co->CounfOfItems = 0;
  p_co->dropID = id;
  p_co->pItem = NULL;
  p_co->Selected = -1;
  p_co->x = x;
  p_co->y = y;

/*	BitBlt(hdc, x, y, _2dd.bitmap[hdcCO.hdcComboDrop].bitmap.bmWidth, _2dd.bitmap[hdcCO.hdcComboDrop].bitmap.bmHeight, 
		   _2dd.bitmap[hdcCO.hdcComboDrop].bitmapDC, 0, 0, SRCCOPY);*/

  ddxBitBlt(hdc, x, y, ddxGetWidth(hdcCO.hdcComboDrop),
    ddxGetHight(hdcCO.hdcComboDrop), hdcCO.hdcComboDrop, 0, 0);

  ax = x + ddxGetWidth(hdcCO.hdcComboDrop);
  ay = y + ddxGetHight(hdcCO.hdcComboDrop);

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

int co_Combo_Drop_Add_String(COMBO_DROP_CONTROL * p_co, char *text,
  float fValue)
{
  int tx, ty;
  WCHAR wc[128];

  p_co->CounfOfItems++;

  p_co->pItem =
    (COMBO_ITEM *) realloc(p_co->pItem,
    p_co->CounfOfItems * sizeof(COMBO_ITEM));

  if (!p_co->pItem)
    return 0;

  p_co->pItem[p_co->CounfOfItems - 1].Norm =
    ddxCreateSurface(ddxGetWidth(hdcCO.hdcComboDrop) - 20,
    ddxGetHight(hdcCO.hdcComboMid), ddxFindFreeSurface());

  p_co->pItem[p_co->CounfOfItems - 1].Sel =
    ddxCreateSurface(ddxGetWidth(hdcCO.hdcComboDrop) - 20,
    ddxGetHight(hdcCO.hdcComboMid), ddxFindFreeSurface());

  strcpy(p_co->pItem[p_co->CounfOfItems - 1].text, text);

  p_co->pItem[p_co->CounfOfItems - 1].fValue = fValue;

  MultiByteToWideChar(CP_ACP, 0, text, strlen(text) + 1, wc,
    sizeof(wc) / sizeof(wc[0]));
  fn_Draw_MessageA(p_co->pItem[p_co->CounfOfItems - 1].Norm, 2, 0,
    &b2_2d_font.gt, &b2_2d_font.ts, wc, 0, &tx, &ty);
  fn_Draw_MessageA(p_co->pItem[p_co->CounfOfItems - 1].Sel, 2, 0,
    &b2_2d_font.gt, &b2_2d_font.ts, wc, 1, &tx, &ty);

  return 1;
}

int co_Combo_Drop_Add_StringWC(COMBO_DROP_CONTROL * p_co, char *text,
  float fValue)
{
  int tx, ty;
  WCHAR wc[128];
  WCHAR ws[128];

  p_co->CounfOfItems++;

  p_co->pItem =
    (COMBO_ITEM *) realloc(p_co->pItem,
    p_co->CounfOfItems * sizeof(COMBO_ITEM));

  if (!p_co->pItem)
    return 0;

  p_co->pItem[p_co->CounfOfItems - 1].Norm =
    ddxCreateSurface(ddxGetWidth(hdcCO.hdcComboDrop) - 20,
    ddxGetHight(hdcCO.hdcComboMid), ddxFindFreeSurface());

  p_co->pItem[p_co->CounfOfItems - 1].Sel =
    ddxCreateSurface(ddxGetWidth(hdcCO.hdcComboDrop) - 20,
    ddxGetHight(hdcCO.hdcComboMid), ddxFindFreeSurface());

  strcpy(p_co->pItem[p_co->CounfOfItems - 1].text, text);

  p_co->pItem[p_co->CounfOfItems - 1].fValue = fValue;

  MultiByteToWideChar(CP_ACP, 0, text, strlen(text) + 1, wc,
    sizeof(wc) / sizeof(wc[0]));
  MultiByteToWideChar(CP_ACP, 0, "##endofmessage",
    strlen("##endofmessage") + 1, ws, sizeof(ws) / sizeof(ws[0]));

  fn_Draw_Message(p_co->pItem[p_co->CounfOfItems - 1].Norm, 2, 0,
    &b2_2d_font.gt, &b2_2d_font.ts, wc, ws, 0, &tx, &ty);
  fn_Draw_Message(p_co->pItem[p_co->CounfOfItems - 1].Sel, 2, 0,
    &b2_2d_font.gt, &b2_2d_font.ts, wc, ws, 1, &tx, &ty);

  return 1;
}

int co_Combo_Drop_Set_Sel(int hdc, COMBO_DROP_CONTROL * p_co, int i)
{
  p_co->Selected = i;

  /*
     BitBlt(hdc, p_co->x, p_co->y, _2dd.bitmap[hdcCO.hdcComboDrop].bitmap.bmWidth, 
     _2dd.bitmap[hdcCO.hdcComboDrop].bitmap.bmHeight,
     _2dd.bitmap[hdcCO.hdcComboDrop].bitmapDC, 0, 0, SRCCOPY); */

  ddxBitBlt(hdc, p_co->x, p_co->y, ddxGetWidth(hdcCO.hdcComboDrop),
    ddxGetHight(hdcCO.hdcComboDrop), hdcCO.hdcComboDrop, 0, 0);

  if (p_co->pItem) {
    /*TransparentBltU(hdc, p_co->x + 5, p_co->y + 7, p_co->pItem[p_co->Selected].Norm.x, p_co->pItem[p_co->Selected].Norm.y,
       p_co->pItem[p_co->Selected].Norm.hdc, 0, 0, p_co->pItem[p_co->Selected].Norm.x, p_co->pItem[p_co->Selected].Norm.y, TRANSCOLOR); */
  
		ddxTransparentBlt(hdc, p_co->x + 5, p_co->y + 7, ddxGetWidth(p_co->pItem[p_co->Selected].Norm), ddxGetHight(p_co->pItem[p_co->Selected].Norm),
					      p_co->pItem[p_co->Selected].Norm, 0, 0, ddxGetWidth(p_co->pItem[p_co->Selected].Norm), ddxGetHight(p_co->pItem[p_co->Selected].Norm), TRANSCOLOR);
  }

  return 1;
}

int co_Combo_Drop_Set_String(int hdc, COMBO_DROP_CONTROL * p_co, int xcor,
  int ycor)
{
  int i;

  if (p_co->Selected < 0)
    return 0;


  ddxSetFlip(0);

  for (i = 0; i < 2; i++) {
    ddxBitBlt(hdc, p_co->x + xcor, p_co->y + ycor,
      ddxGetWidth(hdcCO.hdcComboDrop), ddxGetHight(hdcCO.hdcComboDrop),
      hdcCO.hdcComboDrop, 0, 0);

    if (p_co->pItem)
      ddxTransparentBlt(hdc, p_co->x + xcor + 5, p_co->y + ycor + 7,
        ddxGetWidth(p_co->pItem[p_co->Selected].Norm),
        ddxGetHight(p_co->pItem[p_co->Selected].Norm),
        p_co->pItem[p_co->Selected].Norm, 0, 0,
        ddxGetWidth(p_co->pItem[p_co->Selected].Norm),
        ddxGetHight(p_co->pItem[p_co->Selected].Norm), TRANSCOLOR);

    if (!i)
      DisplayFrame();
  }

  ddxSetFlip(1);

  return 1;
}

int co_Combo_Drop_Get_Sel(CONTROL_LIST_ITEM * p_list, int lsize, int id,
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

int co_Combo_Add_String(COMBO_CONTROL * p_co, char *text)
{
  int tx, ty;
  WCHAR wc[128];

  p_co->CounfOfItems++;

  p_co->pItem =
    (COMBO_ITEM *) realloc(p_co->pItem,
    p_co->CounfOfItems * sizeof(COMBO_ITEM));

  if (!p_co->pItem)
    return 0;

  p_co->pItem[p_co->CounfOfItems - 1].Norm =
    ddxCreateSurface(ddxGetWidth(hdcCO.hdcCombo),
    ddxGetHight(hdcCO.hdcComboMid), ddxFindFreeSurface());

  p_co->pItem[p_co->CounfOfItems - 1].Sel =
    ddxCreateSurface(ddxGetWidth(hdcCO.hdcCombo),
    ddxGetHight(hdcCO.hdcComboMid), ddxFindFreeSurface());

  strcpy(p_co->pItem[p_co->CounfOfItems - 1].text, text);

  MultiByteToWideChar(CP_ACP, 0, text, strlen(text) + 1, wc,
    sizeof(wc) / sizeof(wc[0]));
  fn_Draw_MessageA(p_co->pItem[p_co->CounfOfItems - 1].Norm, 2, 0,
    &b2_2d_font.gt, &b2_2d_font.ts, wc, 0, &tx, &ty);
  fn_Draw_MessageA(p_co->pItem[p_co->CounfOfItems - 1].Sel, 2, 0,
    &b2_2d_font.gt, &b2_2d_font.ts, wc, 1, &tx, &ty);

  return 1;
}

int co_Combo_Add_StringWC2(COMBO_CONTROL * p_co, WCHAR * wc, char *text)
{
  int tx, ty;

  p_co->CounfOfItems++;

  p_co->pItem =
    (COMBO_ITEM *) realloc(p_co->pItem,
    p_co->CounfOfItems * sizeof(COMBO_ITEM));

  if (!p_co->pItem)
    return 0;

  p_co->pItem[p_co->CounfOfItems - 1].Norm =
    ddxCreateSurface(ddxGetWidth(hdcCO.hdcCombo),
    ddxGetHight(hdcCO.hdcComboMid), ddxFindFreeSurface());

  p_co->pItem[p_co->CounfOfItems - 1].Sel =
    ddxCreateSurface(ddxGetWidth(hdcCO.hdcCombo),
    ddxGetHight(hdcCO.hdcComboMid), ddxFindFreeSurface());

  strcpy(p_co->pItem[p_co->CounfOfItems - 1].text, text);

  fn_Draw_MessageA(p_co->pItem[p_co->CounfOfItems - 1].Norm, 2, 0,
    &b2_2d_font.gt, &b2_2d_font.ts, wc, 0, &tx, &ty);
  fn_Draw_MessageA(p_co->pItem[p_co->CounfOfItems - 1].Sel, 2, 0,
    &b2_2d_font.gt, &b2_2d_font.ts, wc, 1, &tx, &ty);

  return 1;
}

int co_Combo_Add_StringWC(COMBO_CONTROL * p_co, char *text)
{
  int tx, ty;
  WCHAR wc[128];
  WCHAR ws[128];

  p_co->CounfOfItems++;

  p_co->pItem =
    (COMBO_ITEM *) realloc(p_co->pItem,
    p_co->CounfOfItems * sizeof(COMBO_ITEM));

  if (!p_co->pItem)
    return 0;

  p_co->pItem[p_co->CounfOfItems - 1].Norm =
    ddxCreateSurface(ddxGetWidth(hdcCO.hdcCombo),
    ddxGetHight(hdcCO.hdcComboMid), ddxFindFreeSurface());
  p_co->pItem[p_co->CounfOfItems - 1].Sel =
    ddxCreateSurface(ddxGetWidth(hdcCO.hdcCombo),
    ddxGetHight(hdcCO.hdcComboMid), ddxFindFreeSurface());

  strcpy(p_co->pItem[p_co->CounfOfItems - 1].text, text);

  MultiByteToWideChar(CP_ACP, 0, text, strlen(text) + 1, wc,
    sizeof(wc) / sizeof(wc[0]));
  MultiByteToWideChar(CP_ACP, 0, "##endofmessage",
    strlen("##endofmessage") + 1, ws, sizeof(ws) / sizeof(ws[0]));

  fn_Draw_Message(p_co->pItem[p_co->CounfOfItems - 1].Norm, 2, 0,
    &b2_2d_font.gt, &b2_2d_font.ts, wc, ws, 0, &tx, &ty);
  fn_Draw_Message(p_co->pItem[p_co->CounfOfItems - 1].Sel, 2, 0,
    &b2_2d_font.gt, &b2_2d_font.ts, wc, ws, 1, &tx, &ty);
  return 1;
}

int co_Combo_Set_String(int hdc, COMBO_CONTROL * p_co, int xcor, int ycor)
{
  if (p_co->Selected < 0)
    return 0;

  /*BitBlt(hdc, p_co->x + xcor, p_co->y + ycor, _2dd.bitmap[hdcCO.hdcCombo].bitmap.bmWidth, 
     _2dd.bitmap[hdcCO.hdcCombo].bitmap.bmHeight,
     _2dd.bitmap[hdcCO.hdcCombo].bitmapDC, 0, 0, SRCCOPY); */

  ddxBitBlt(hdc, p_co->x + xcor, p_co->y + ycor, ddxGetWidth(hdcCO.hdcCombo),
    ddxGetHight(hdcCO.hdcCombo), hdcCO.hdcCombo, 0, 0);

  if (p_co->pItem) {
    /*TransparentBltU(hdc, p_co->x + xcor + 5, p_co->y + ycor + 7, p_co->pItem[p_co->Selected].Norm.x, p_co->pItem[p_co->Selected].Norm.y,
       p_co->pItem[p_co->Selected].Norm.hdc, 0, 0, p_co->pItem[p_co->Selected].Norm.x, p_co->pItem[p_co->Selected].Norm.y, TRANSCOLOR); */

    ddxTransparentBlt(hdc, p_co->x + xcor + 5, p_co->y + ycor + 7,
      ddxGetWidth(p_co->pItem[p_co->Selected].Norm),
      ddxGetHight(p_co->pItem[p_co->Selected].Norm),
      p_co->pItem[p_co->Selected].Norm, 0, 0,
      ddxGetWidth(p_co->pItem[p_co->Selected].Norm),
      ddxGetHight(p_co->pItem[p_co->Selected].Norm), TRANSCOLOR);
  }

  return 1;
}

int co_Combo_Set_Sel(int hdc, COMBO_CONTROL * p_co, int i)
{
  int xp = 0;
  int yp = 0;

  p_co->Selected = i;
  p_co->OSelected = i;

  if (hdc == HDC2DD) {
    xp = TAB_X;
    yp = TAB_Y;
  }

  ddxBitBlt(hdc, p_co->x + xp, p_co->y + yp, ddxGetWidth(hdcCO.hdcCombo),
    ddxGetHight(hdcCO.hdcCombo), hdcCO.hdcCombo, 0, 0);

  if (p_co->pItem)
    ddxTransparentBlt(hdc, p_co->x + 5 + xp, p_co->y + 7 + yp,
      ddxGetWidth(p_co->pItem[p_co->Selected].Norm),
      ddxGetHight(p_co->pItem[p_co->Selected].Norm),
      p_co->pItem[p_co->Selected].Norm, 0, 0,
      ddxGetWidth(p_co->pItem[p_co->Selected].Norm),
      ddxGetHight(p_co->pItem[p_co->Selected].Norm), TRANSCOLOR);

  return 1;
}

int co_Combo_Get_Sel(CONTROL_LIST_ITEM * p_list, int lsize, int id)
{
  int i;

  for (i = 0; i < lsize; i++)
    if ((p_list + i)->p_combo)
      if ((p_list + i)->p_combo->comboID == id)
        return (p_list + i)->p_combo->Selected;

  return -1;
}

int co_Combo_Get_Sel_Not_Opend(CONTROL_LIST_ITEM * p_list, int lsize, int id)
{
  int i;

  for (i = 0; i < lsize; i++)
    if ((p_list + i)->p_combo)
      if ((p_list + i)->p_combo->comboID == id
        && !(p_list + i)->p_combo->bList)
        return (p_list + i)->p_combo->Selected;

  return -1;
}

int co_Combo_Set_Params(COMBO_CONTROL * p_co, int itemnum)
{
  p_co->ListMaxHight = itemnum * ddxGetHight(hdcCO.hdcComboMid);
  p_co->CounfOfItemsL = itemnum;

  return 1;
}

int co_Combo_Open(int hdc, COMBO_CONTROL * p_co, int xcor, int ycor)
{
  int *pDCI;

  if (p_co->bList)
    return 1;

  bBlockList = 1;

  p_co->pBDC = (int *) malloc(sizeof(int));
  if (!p_co->pBDC)
    return 0;

  //p_co->ListMaxHightR = (p_co->CounfOfItems * 35) + _2dd.bitmap[hdcCO.hdcComboEnd].bitmap.bmHeight;
  if (p_co->CounfOfItems > p_co->CounfOfItemsL)
    p_co->ListMaxHightR =
      (p_co->CounfOfItemsL * 25) + ddxGetHight(hdcCO.hdcComboEnd);
  else
    p_co->ListMaxHightR =
      (p_co->CounfOfItems * 25) + ddxGetHight(hdcCO.hdcComboEnd);

  // kprintf(1, "p_co->CounfOfItems = %d, p_co->ListMaxHightR = %d", p_co->CounfOfItems, p_co->ListMaxHightR);

  //co_CreateDC(hdc, p_co->WidthR, p_co->ListMaxHightR, p_co->pBDC);
  *p_co->pBDC =
    ddxCreateSurface(p_co->WidthR, p_co->ListMaxHightR, ddxFindFreeSurface());

  pDCI = p_co->pBDC;

  if (!pDCI) {
    free((void *) p_co->pBDC);
    return 0;
  }

  /*BitBlt(pDCI->hdc, 0, 0, pDCI->x, pDCI->y, hdc, p_co->x + xcor, 
     p_co->y + _2dd.bitmap[hdcCO.hdcCombo].bitmap.bmHeight + ycor, SRCCOPY); */

  ddxBitBlt(*pDCI, 0, 0, ddxGetWidth(*pDCI), ddxGetHight(*pDCI), hdc,
    p_co->x + xcor, p_co->y + ddxGetHight(hdcCO.hdcCombo) + ycor);


  p_co->CSelected = -1;
  p_co->Selected = -1;

  p_co->bList = 1;
  p_co->bEdit = 0;
  co_Combo_Draw(hdc, p_co, xcor, ycor);

  p_co->coListRect.top = p_co->y + ddxGetHight(hdcCO.hdcCombo);
  p_co->coListRect.bottom =
    p_co->y + ddxGetHight(hdcCO.hdcCombo) + p_co->ListMaxHightR;
  p_co->coListRect.left = p_co->x;

  if (p_co->CounfOfItems > p_co->CounfOfItemsL)
    p_co->coListRect.right = p_co->x + ddxGetWidth(hdcCO.hdcComboMid) - 2 -
      ddxGetWidth(hdcCO.hdcComboUp);
  else
    p_co->coListRect.right = p_co->x + ddxGetWidth(hdcCO.hdcComboMid) - 2;

  p_co->bEdit = 1;
  return 1;
}

int co_Combo_Close(int hdc, COMBO_CONTROL * p_co, int xcor, int ycor)
{
  if (!p_co->bList)
    return 1;

  bBlockList = 0;

  p_co->SSelected = 0;
  p_co->bList = 0;

  ddxBitBlt(hdc, p_co->x + xcor, ycor + p_co->y + ddxGetHight(hdcCO.hdcCombo),
    ddxGetWidth(*p_co->pBDC), ddxGetHight(*p_co->pBDC), *p_co->pBDC, 0, 0);

  //co_Release_Bitmap(p_co->pBDC);
  ddxReleaseBitmap(*p_co->pBDC);

  free((void *) p_co->pBDC);
  p_co->pBDC = NULL;

  dim.t1 = 0;
  dim.dt1 = 0;

  return 1;
}

WCHAR *co_Edit_Get_Text(CONTROL_EDIT * p_ed)
{
  if (!p_ed)
    return NULL;
  else
    return p_ed->wtext;
}

CONTROL_EDIT *co_Create_Edit(int hdc, int x, int y, int editID)
{
  CONTROL_EDIT *p_ed = NULL;

  p_ed = (CONTROL_EDIT *) malloc(sizeof(CONTROL_EDIT));

  if (!p_ed)
    return 0;

  ZeroMemory(p_ed, sizeof(CONTROL_EDIT));

  p_ed->x = x;
  p_ed->y = y;
  p_ed->tx = 0;
  p_ed->bcActive = 0;
  p_ed->bActive = 0;
  p_ed->rect.left = x;
  p_ed->rect.top = y;
  p_ed->rect.right = x + ddxGetWidth(hdcED.hdcEdit);
  p_ed->rect.bottom = y + ddxGetHight(hdcED.hdcEdit);

  ddxBitBlt(hdc, x, y, ddxGetWidth(hdcED.hdcEdit), ddxGetHight(hdcED.hdcEdit),
    hdcED.hdcEdit, 0, 0);
  p_ed->bDC =
    ddxCreateSurface(ddxGetWidth(hdcED.hdcEdit) + 25,
    ddxGetHight(hdcED.hdcEdit), ddxFindFreeSurface());

  return p_ed;
}

void co_Release_Button(BUTTON_CONTROL * p_bu)
{
  //co_Release_Bitmap(&p_bu->dc);
  ddxReleaseBitmap(p_bu->dc);
  free((void *) p_bu);
}

BUTTON_CONTROL *co_Create_Button(int hdc, int x, int y, int type, char *text,
  int isection, int buttonID)
{
  WCHAR wc[128];
  WCHAR ws[128];

  int bmpx = 0, 
      bmpy = 0;
  int bmpDC = 0;
  int cx, cy;
  int tx, ty;
  int tmpDC;
  BUTTON_CONTROL *p_bu = NULL;

  p_bu = (BUTTON_CONTROL *) malloc(sizeof(BUTTON_CONTROL));

  if (!p_bu)
    return 0;

  ZeroMemory(p_bu, sizeof(BUTTON_CONTROL));

  p_bu->ButtonID = buttonID;
  p_bu->bActivated = 0;
  p_bu->x = x;
  p_bu->y = y;

  switch (type) {
    case BUTTON_LARGE:
      bmpx = ddxGetWidth(hdcBU.hdcButtonL);
      bmpy = ddxGetHight(hdcBU.hdcButtonL);
      bmpDC = hdcBU.hdcButtonL;
      p_bu->dc = ddxCreateSurface(bmpx, bmpy, ddxFindFreeSurface());

      p_bu->Rect.bottom = y + bmpy;
      p_bu->Rect.right = x + bmpx;
      break;
    case BUTTON_SMALL:
      bmpx = ddxGetWidth(hdcBU.hdcButtonS);
      bmpy = ddxGetHight(hdcBU.hdcButtonS);
      bmpDC = hdcBU.hdcButtonS;
      p_bu->dc = ddxCreateSurface(bmpx, bmpy, ddxFindFreeSurface());

      p_bu->Rect.bottom = y + bmpy;
      p_bu->Rect.right = x + bmpx;
      break;
    default:
      assert(0);
  }

  p_bu->type = type;
  p_bu->Rect.top = y;
  p_bu->Rect.left = x;

  ddxBitBlt(p_bu->dc, 0, 0, bmpx, bmpy, bmpDC, 0, 0);

  tmpDC = ddxCreateSurface(bmpx, bmpy, ddxFindFreeSurface());

  MultiByteToWideChar(CP_ACP, 0, text, strlen(text) + 1, wc,
    sizeof(wc) / sizeof(wc[0]));
  MultiByteToWideChar(CP_ACP, 0, "##endofmessage",
    strlen("##endofmessage") + 1, ws, sizeof(ws) / sizeof(ws[0]));
  
  fn_Draw_Message(tmpDC, 0, 0, &b2_2d_font.gt, &b2_2d_font.ts, wc, ws, isection, &tx, &ty);

  cx = ftoi(((p_bu->Rect.right - p_bu->Rect.left) - tx) / 2.0f);
  cy = ftoi(((p_bu->Rect.bottom - p_bu->Rect.top) - ty) / 2.0f);

  ddxTransparentBlt(p_bu->dc, cx, cy, tx, ty, tmpDC, 0, 0, tx, ty, TRANSCOLOR);

  ddxReleaseBitmap(tmpDC);

  ddxBitBlt(hdc, p_bu->Rect.left, p_bu->Rect.top, ddxGetWidth(p_bu->dc), ddxGetHight(p_bu->dc), p_bu->dc, 0, 0);

  return p_bu;
}

int co_Check_Set_State(CHECKBOX_CONTROL * p_ch, int hdc, int state,
  char bDraw)
{
  int x = 0, y = 0;
  int bmpx = ddxGetWidth(hdcCH.hdcCheck);
  int bmpDC;
    
  bmpDC = hdcCH.hdcCheck;

  p_ch->bChecked = state;

  if (hdc == HDC2DD) {
    x = TAB_X;
    y = TAB_Y;
  }

  if (p_ch->bChecked) {
    if (bDraw) {
      ddxTransparentBlt(hdc, p_ch->Rect.left + x, p_ch->Rect.top + y, bmpx,
                        20, bmpDC, 0, 25, bmpx, 20, RGB(237, 77, 0));
    }

    p_ch->bChange = 1;
  }
  else {
    if (bDraw) {
      ddxTransparentBlt(hdc, p_ch->Rect.left + x, p_ch->Rect.top + y, bmpx,
                        20, bmpDC, 0, 1, bmpx, 20, RGB(237, 77, 0));
    }

    p_ch->bChange = 1;
  }

  if (p_ch->iDC != -1)
    ddxBitBlt(p_ch->iDC, 0, 0, p_ch->RectFull.right, p_ch->RectFull.bottom,
              hdc, p_ch->x + x, p_ch->y + y);

  return 1;
}

void co_Release_CheckBox(CHECKBOX_CONTROL * p_ch)
{
  if (p_ch->iDC != -1)
    ddxReleaseBitmap(p_ch->iDC);

  free((void *) p_ch);
}

CHECKBOX_CONTROL *co_Create_CheckBox(int hdc, int x, int y, char *text, 
                                     int isection, int checkID, 
                                     CONTROL_CALLBACK p_callback, 
                                     void *p_callback_data)
{
  WCHAR wc[128];
  WCHAR ws[128];

  int bmpx;
  int bmpDC;
  int tx, ty;
  CHECKBOX_CONTROL *p_ch = NULL;

  p_ch = (CHECKBOX_CONTROL *) mmalloc(sizeof(CHECKBOX_CONTROL));
  p_ch->checkID = checkID;

  bmpx = ddxGetWidth(hdcCH.hdcCheck);  
  bmpDC = hdcCH.hdcCheck;

  p_ch->Rect.bottom = y + 20;
  p_ch->Rect.right = x + bmpx;
  p_ch->Rect.top = y;
  p_ch->Rect.left = x;
  p_ch->x = x;
  p_ch->y = y;
  p_ch->bChecked = 0;
  p_ch->iDC = -1;
  p_ch->p_callback = p_callback;
  p_ch->p_callback_data = p_callback_data;

  ddxTransparentBlt(hdc, x, y, bmpx, 20, bmpDC, 0, 1, bmpx, 20, RGB(237, 77, 0));

  MultiByteToWideChar(CP_ACP, 0, text, strlen(text) + 1, wc, sizeof(wc) / sizeof(wc[0]));
  MultiByteToWideChar(CP_ACP, 0, "##endofmessage", strlen("##endofmessage") + 1, ws, sizeof(ws) / sizeof(ws[0]));

  if(fn_Draw_Message(hdc, x + 30, y + 3, &b2_2d_font.gt, &b2_2d_font.ts, wc, ws, isection, &tx, &ty)) {
    p_ch->RectFull.left = x;
    p_ch->RectFull.top = y;
    p_ch->RectFull.right = 30 + tx - p_ch->RectFull.left;
    p_ch->RectFull.bottom = ty - p_ch->RectFull.top;
    p_ch->iDC = ddxCreateSurface(p_ch->RectFull.right, p_ch->RectFull.bottom, ddxFindFreeSurface());
  }

  return p_ch;
}

int co_Progres_Set(PROGRES_CONTROL * p_pr, int hdc, int i)
{
  int dd = p_pr->max - p_pr->min;
  int md = p_pr->rectProgres.right - p_pr->rectProgres.left;
  float in = md / (float) dd;
  int x = p_pr->rectProgres.left + ftoi((i - p_pr->min) * in);

  //int yp;

  //BitBlt(hdc, p_pr->rectMover.left, p_pr->rectMover.top, p_pr->bDC.x, p_pr->bDC.y, p_pr->bDC.hdc, 0, 0, SRCCOPY);
  ddxBitBlt(hdc, p_pr->rectMover.left, p_pr->rectMover.top,
    ddxGetWidth(p_pr->bDC), ddxGetHight(p_pr->bDC), p_pr->bDC, 0, 0);

  p_pr->pos = x;
  p_pr->rectMover.left = p_pr->pos - p_pr->cor;
  p_pr->rectMover.right = p_pr->rectMover.left + ddxGetWidth(hdcPR.hdcMover);

  // yp = ftoi(ddxGetHight(hdcPR.hdcMover) / 2.0f);

  /*BitBlt(p_pr->bDC.hdc, 0, 0, _2dd.bitmap[hdcPR.hdcMover].bitmap.bmWidth, 
     _2dd.bitmap[hdcPR.hdcMover].bitmap.bmHeight, hdc, 
     p_pr->rectMover.left, p_pr->rectMover.top, SRCCOPY); */

  ddxBitBlt(p_pr->bDC, 0, 0, ddxGetWidth(hdcPR.hdcMover),
    ddxGetHight(hdcPR.hdcMover), hdc,
    p_pr->rectMover.left, p_pr->rectMover.top);

  /*BitBlt(hdc, p_pr->rectMover.left, p_pr->rectMover.top, 
     _2dd.bitmap[hdcPR.hdcMover].bitmap.bmWidth, _2dd.bitmap[hdcPR.hdcMover].bitmap.bmHeight, 
     _2dd.bitmap[hdcPR.hdcMover].bitmapDC, 0, 0, SRCCOPY); */

  ddxTransparentBlt(hdc, p_pr->rectMover.left, p_pr->rectMover.top,
    ddxGetWidth(hdcPR.hdcMover), ddxGetHight(hdcPR.hdcMover),
    hdcPR.hdcMover, 0, 0, ddxGetWidth(hdcPR.hdcMover),
    ddxGetHight(hdcPR.hdcMover), RGB(237, 77, 0));

  return 1;
}

int co_Progres_Get(CONTROL_LIST_ITEM * p_list, int lsize, int id)
{
  int i;

  for (i = 0; i < lsize; i++)
    if ((p_list + i)->p_prog)
      if ((p_list + i)->p_prog->progID == id) {
        PROGRES_CONTROL *p_pr = (p_list + i)->p_prog;
        int dd = p_pr->max - p_pr->min;
        int md = p_pr->rectProgres.right - p_pr->rectProgres.left;
        float out = dd / (float) md;
        int x = p_pr->min + ftoi((p_pr->pos - p_pr->rectProgres.left) * out);

        return x;
      }

  return 0;
}

int co_Progres_Changed(CONTROL_LIST_ITEM * p_list, int lsize, int id)
{
  int i;

  for (i = 0; i < lsize; i++) {
    if ((p_list + i)->p_prog) {
      if ((p_list + i)->p_prog->progID == id) {
        if ((p_list + i)->p_prog->bChange)
          return 1;
        else
          return 0;
      }
    }
  }

  return 0;
}

void co_Progres_Disable(int hdc, int xcor, int ycor,
                        CONTROL_LIST_ITEM * p_list, int lsize, int id, 
                        char bSave, int shdc)
{
  PROGRES_CONTROL *p_pr;
  int i;

  for (i = 0; i < lsize; i++) {
    if ((p_list + i)->p_prog) {
      if ((p_list + i)->p_prog->progID == id) {
        p_pr = (p_list + i)->p_prog;
        (p_list + i)->bActive = 0;
        (p_list + i)->bDisabled = 1;

        if (bSave)
          ddxBitBlt(p_pr->iDC, 0, 0, p_pr->RectFull.right,
            p_pr->RectFull.bottom, shdc, p_pr->RectFull.left + xcor,
            p_pr->RectFull.top + ycor);

        ddxTransparentBlt(hdc, p_pr->RectFull.left + xcor,
          p_pr->RectFull.top + ycor,
          p_pr->RectFull.right,
          p_pr->RectFull.bottom,
          hdcCH.hdcGray,
          0, 0, p_pr->RectFull.right, p_pr->RectFull.bottom, TRANSCOLOR);
        return;
      }
    }
  }

  return;
}

void co_Progres_Enable(int hdc, int xcor, int ycor,
  CONTROL_LIST_ITEM * p_list, int lsize, int id)
{
  PROGRES_CONTROL *p_pr;
  int i;

  for (i = 0; i < lsize; i++)
    if ((p_list + i)->p_prog)
      if ((p_list + i)->p_prog->progID == id) {
        p_pr = (p_list + i)->p_prog;
        (p_list + i)->bActive = 1;
        (p_list + i)->bDisabled = 0;

        if (p_pr->iDC != -1)
          ddxBitBlt(hdc, p_pr->RectFull.left + xcor,
            p_pr->RectFull.top + ycor,
            p_pr->RectFull.right, p_pr->RectFull.bottom, p_pr->iDC, 0, 0);
        return;
      }

  return;
}

void co_Release_Progres(PROGRES_CONTROL * p_pr)
{
  //co_Release_Bitmap(&p_pr->bDC);
  ddxReleaseBitmap(p_pr->iDC);
  ddxReleaseBitmap(p_pr->bDC);

  free((void *) p_pr);
}

PROGRES_CONTROL *co_Create_Progres(int hdc, int x, int y, int min, int max,
  int progID)
{
  int bmpx, bmpy, yp;
  int bmpDC;
  PROGRES_CONTROL *p_pr = NULL;

  p_pr = (PROGRES_CONTROL *) malloc(sizeof(PROGRES_CONTROL));

  if (!p_pr)
    return 0;

  ZeroMemory(p_pr, sizeof(PROGRES_CONTROL));

  p_pr->bChange = 0;
  p_pr->bIn = 0;
  p_pr->progID = progID;
  p_pr->min = min;
  p_pr->max = max;
  p_pr->x = x;
  p_pr->y = y;
  p_pr->cor = ftoi(ddxGetWidth(hdcPR.hdcMover) / 2.0f);
  p_pr->pos = x;
  p_pr->bExclusive = 0;

  bmpx = ddxGetWidth(hdcPR.hdcLine);
  bmpy = ddxGetHight(hdcPR.hdcMover);
  bmpDC = hdcPR.hdcLine;
  yp = ftoi(bmpy / 2.0f);

  p_pr->rectMover.top = y;
  p_pr->rectMover.left = x;
  p_pr->rectMover.right = x + ddxGetWidth(hdcPR.hdcMover);
  p_pr->rectMover.bottom = y + bmpy;

  p_pr->rectProgres.top = y;
  p_pr->rectProgres.left = x;
  p_pr->rectProgres.bottom = p_pr->rectMover.bottom;
  p_pr->rectProgres.right = x + bmpx;

  //BitBlt(hdc, x, y+yp, bmpx, _2dd.bitmap[hdcPR.hdcLine].bitmap.bmHeight, bmpDC, 0, 0, SRCCOPY);
  ddxBitBlt(hdc, x, y + yp, bmpx, ddxGetHight(hdcPR.hdcLine), bmpDC, 0, 0);
  //co_CreateDC(hdc, _2dd.bitmap[hdcPR.hdcMover].bitmap.bmWidth, _2dd.bitmap[hdcPR.hdcMover].bitmap.bmHeight, &p_pr->bDC);
  p_pr->bDC =
    ddxCreateSurface(ddxGetWidth(hdcPR.hdcMover), ddxGetHight(hdcPR.hdcMover),
    ddxFindFreeSurface());

  //BitBlt(p_pr->bDC.hdc, 0, 0, _2dd.bitmap[hdcPR.hdcMover].bitmap.bmWidth, bmpy, hdc, x, y, SRCCOPY);
  ddxBitBlt(p_pr->bDC, 0, 0, ddxGetWidth(hdcPR.hdcMover),
    ddxGetHight(hdcPR.hdcMover), hdc, x, y);

  //BitBlt(hdc, x, y, _2dd.bitmap[hdcPR.hdcMover].bitmap.bmWidth, bmpy, _2dd.bitmap[hdcPR.hdcMover].bitmapDC, 0, 0, SRCCOPY);

  ddxTransparentBlt(hdc, x, y, ddxGetWidth(hdcPR.hdcMover), bmpy,
    hdcPR.hdcMover, 0, 0, ddxGetWidth(hdcPR.hdcMover), bmpy, RGB(237, 77, 0));

  p_pr->RectFull.left = x - yp - 300;
  p_pr->RectFull.top = y;
  p_pr->RectFull.right = bmpx + yp + 300;
  p_pr->RectFull.bottom = bmpy;

  p_pr->iDC =
    ddxCreateSurface(p_pr->RectFull.right, p_pr->RectFull.bottom,
    ddxFindFreeSurface());

  return p_pr;
}

int co_Set_Text(int hdc, int x, int y, char *text, int isection)
{
  int tx, ty;
  WCHAR wc[128];
  WCHAR ws[128];

  MultiByteToWideChar(CP_ACP, 0, text, strlen(text) + 1, wc,
    sizeof(wc) / sizeof(wc[0]));
  MultiByteToWideChar(CP_ACP, 0, "##endofmessage",
    strlen("##endofmessage") + 1, ws, sizeof(ws) / sizeof(ws[0]));

  fn_Draw_Message(hdc, x, y, &b2_2d_font.gt, &b2_2d_font.ts, wc, ws, isection,
    &tx, &ty);

  return 1;
}

int co_Set_Text_Center(int hdc, char *text, int isection, RECT r)
{
  int xp, yp;
  int tx, ty;
  WCHAR wc[128];
  WCHAR ws[128];
  int h;

  h = ddxCreateSurface(600, 100, ddxFindFreeSurface());

  MultiByteToWideChar(CP_ACP, 0, text, strlen(text) + 1, wc, sizeof(wc) / sizeof(wc[0]));
  MultiByteToWideChar(CP_ACP, 0, "##endofmessage", strlen("##endofmessage") + 1, ws, sizeof(ws) / sizeof(ws[0]));

  fn_Draw_Message(h, 0, 0, &b2_2d_font.gt, &b2_2d_font.ts, wc, ws, isection, &tx, &ty);

  xp = ftoi(((r.right - r.left) - tx) / 2.0f);
  yp = ftoi(((r.bottom - r.top) - ty) / 2.0f);
  
  ddxTransparentBlt(hdc, r.left + xp, r.top + yp, tx, ty, h, 0, 0, tx, ty, TRANSCOLOR);

  ddxReleaseBitmap(h);

  return 1;
}

int co_Set_Text_Right(int hdc, char *text, int isection, int x, int y)
{
  int xp;
  int tx, ty;
  WCHAR wc[128];
  WCHAR ws[128];
  int h;

  h = ddxCreateSurface(600, 100, ddxFindFreeSurface());

  MultiByteToWideChar(CP_ACP, 0, text, strlen(text) + 1, wc,
    sizeof(wc) / sizeof(wc[0]));
  MultiByteToWideChar(CP_ACP, 0, "##endofmessage",
    strlen("##endofmessage") + 1, ws, sizeof(ws) / sizeof(ws[0]));

  fn_Draw_Message(h, 0, 0, &b2_2d_font.gt, &b2_2d_font.ts, wc, ws, isection,
    &tx, &ty);

  xp = x - tx;

  ddxTransparentBlt(hdc, xp, y, tx, ty, h, 0, 0, tx, ty, TRANSCOLOR);
  ddxReleaseBitmap(h);

  return 1;
}

int co_Set_Text_RightWC(int hdc, char *text, int isection, int x, int y)
{
  int xp;
  int tx, ty;
  WCHAR wc[128];
  int h;

  h = ddxCreateSurface(600, 100, ddxFindFreeSurface());

  MultiByteToWideChar(CP_ACP, 0, text, strlen(text) + 1, wc,
    sizeof(wc) / sizeof(wc[0]));

  fn_Draw_MessageA(h, 0, 0, &b2_2d_font.gt, &b2_2d_font.ts, wc, isection, &tx, &ty);

  xp = x - tx;
  ddxTransparentBlt(hdc, xp, y, tx, ty, h, 0, 0, tx, ty, TRANSCOLOR);
  ddxReleaseBitmap(h);

  return 1;
}

int co_List_Delete_Item(LIST_VIEW_CONTROL * p_li, char **cValue)
{
  int i;
  RECT r;

  if (p_li->cClckSel == -1)
    return 0;

  if (p_li->cClckSel >= p_li->listnum)
    return 0;

  if (!p_li->piValue[p_li->cClckSel].cValue
    && !p_li->piValue[p_li->cClckSel].wcValue)
    return 0;

  if (cValue)
    (*cValue) = strdup(p_li->piValue[p_li->cClckSel].cValue);

  free((void *) p_li->piValue[p_li->cClckSel].cValue);
  free((void *) p_li->piValue[p_li->cClckSel].wcValue);

  for (i = p_li->cClckSel + 1; i < p_li->listnum; i++) {
    p_li->piValue[i - 1].cValue = p_li->piValue[i].cValue;
    p_li->piValue[i - 1].wcValue = p_li->piValue[i].wcValue;
    p_li->piValue[i - 1].iValue = p_li->piValue[i].iValue;
  }

  p_li->piValue[p_li->listnum - 1].cValue = NULL;
  p_li->piValue[p_li->listnum - 1].wcValue = NULL;
  p_li->piValue[p_li->listnum - 1].iValue = 0;

  i = p_li->cClckSel * 30;

  ddxBitBlt(p_li->bDCn, 0, i, ddxGetWidth(p_li->bDCn),
    ddxGetHight(p_li->bDCn) - (i + 30), p_li->bDCn, 0, i + 30);

  ddxBitBlt(p_li->bDCs, 0, i, ddxGetWidth(p_li->bDCn),
    ddxGetHight(p_li->bDCn) - (i + 30), p_li->bDCs, 0, i + 30);

  r.left = 0;
  r.right = ddxGetWidth(p_li->bDCn);
  r.bottom = ddxGetHight(p_li->bDCn);
  r.top = r.bottom - 30;

  ddxFillRect(p_li->bDCn, &r, 0);
  ddxFillRect(p_li->bDCs, &r, 0);

  if (ddxGetHight(p_li->bDCn) - 30 >
    p_li->rectList.bottom - p_li->rectList.top) {
    int ds, dn;

    ds = p_li->bDCs;
    dn = p_li->bDCn;

    p_li->bDCn =
      ddxCreateSurface(ddxGetWidth(dn), ddxGetHight(dn) - 30,
      ddxFindFreeSurface());
    p_li->bDCs =
      ddxCreateSurface(ddxGetWidth(ds), ddxGetHight(ds) - 30,
      ddxFindFreeSurface());

    ddxBitBlt(p_li->bDCn, 0, 0, ddxGetWidth(p_li->bDCn),
      ddxGetHight(p_li->bDCn), dn, 0, 0);
    ddxBitBlt(p_li->bDCs, 0, 0, ddxGetWidth(p_li->bDCs),
      ddxGetHight(p_li->bDCs), ds, 0, 0);

    ddxReleaseBitmap(dn);
    ddxReleaseBitmap(ds);

    p_li->listnum--;
  }

  p_li->cClckSel = -1;

  return 1;
}

int co_List_Add_String(LIST_VIEW_CONTROL * p_li, int index, int x, char *text,
  int iValue, char bSelected)
{
  int tx, ty;
  WCHAR wc[128];
  WCHAR ws[128];

  if (p_li->piValue) {
    p_li->piValue[index].iValue = iValue;

    if (p_li->piValue[index].cValue) {
      free((void *) p_li->piValue[index].cValue);
      p_li->piValue[index].cValue = NULL;
    }

    p_li->piValue[index].cValue = strdup(text);
    p_li->piValue[index].wcValue = NULL;
  }

  MultiByteToWideChar(CP_ACP, 0, "##endofmessage",
    strlen("##endofmessage") + 1, ws, sizeof(ws) / sizeof(ws[0]));

  MultiByteToWideChar(CP_ACP, 0, text, strlen(text) + 1, wc,
    sizeof(wc) / sizeof(wc[0]));

  if (!bSelected)
    fn_Draw_Message(p_li->bDCn, x, (index * 30) + 2, &b2_2d_font.gt,
      &b2_2d_font.ts, wc, ws, 0, &tx, &ty);
  else
    fn_Draw_Message(p_li->bDCn, x, (index * 30) + 2, &b2_2d_font.gt,
      &b2_2d_font.ts, wc, ws, 1, &tx, &ty);

  fn_Draw_Message(p_li->bDCs, x, (index * 30) + 2, &b2_2d_font.gt,
    &b2_2d_font.ts, wc, ws, 1, &tx, &ty);

  return 1;
}

int co_List_Add_StringWC(LIST_VIEW_CONTROL * p_li, int index, int x,
  char *text, int iValue, char bSelected)
{
  int tx, ty;
  WCHAR wc[128];

  if (p_li->piValue) {
    p_li->piValue[index].iValue = iValue;

    if (p_li->piValue[index].cValue) {
      free((void *) p_li->piValue[index].cValue);
      p_li->piValue[index].cValue = NULL;
    }

    p_li->piValue[index].cValue = strdup(text);
    p_li->piValue[index].wcValue = NULL;
  }

  MultiByteToWideChar(CP_ACP, 0, text, strlen(text) + 1, wc,
    sizeof(wc) / sizeof(wc[0]));

  if (!bSelected)
    fn_Draw_MessageA(p_li->bDCn, x, (index * 30) + 2, &b2_2d_font.gt,
      &b2_2d_font.ts, wc, 0, &tx, &ty);
  else
    fn_Draw_MessageA(p_li->bDCn, x, (index * 30) + 2, &b2_2d_font.gt,
      &b2_2d_font.ts, wc, 1, &tx, &ty);

  fn_Draw_MessageA(p_li->bDCs, x, (index * 30) + 2, &b2_2d_font.gt,
    &b2_2d_font.ts, wc, 1, &tx, &ty);

  return 1;
}

int co_List_Add_StringWC2(LIST_VIEW_CONTROL * p_li, int index, int x,
  WCHAR * wc, char *text, int iValue, char bSelected)
{
  int tx, ty;

  if (p_li->piValue) {
    p_li->piValue[index].iValue = iValue;

    if (p_li->piValue[index].cValue) {
      free((void *) p_li->piValue[index].cValue);
      p_li->piValue[index].cValue = NULL;
    }

    p_li->piValue[index].cValue = strdup(text);
    p_li->piValue[index].wcValue = NULL;
  }

  if (!bSelected)
    fn_Draw_MessageA(p_li->bDCn, x, (index * 30) + 2, &b2_2d_font.gt,
      &b2_2d_font.ts, wc, 0, &tx, &ty);
  else
    fn_Draw_MessageA(p_li->bDCn, x, (index * 30) + 2, &b2_2d_font.gt,
      &b2_2d_font.ts, wc, 1, &tx, &ty);

  fn_Draw_MessageA(p_li->bDCs, x, (index * 30) + 2, &b2_2d_font.gt,
    &b2_2d_font.ts, wc, 1, &tx, &ty);

  return 1;
}

int co_List_Redraw(int hdc, LIST_VIEW_CONTROL * p_li, int y)
{
  /*BitBlt(hdc, p_li->rectList.left, p_li->rectList.top, p_li->bDCn.x, 
     p_li->rectList.bottom - p_li->rectList.top, p_li->bDCn.hdc, 0, y, SRCCOPY); */

  if (y + (p_li->rectList.bottom - p_li->rectList.top) > ddxGetHight(p_li->bDCn))
    y = ddxGetHight(p_li->bDCn) - (p_li->rectList.bottom - p_li->rectList.top);

  if (y < 0)
    y = 0;

  ddxBitBlt(hdc, p_li->rectList.left, p_li->rectList.top,
    ddxGetWidth(p_li->bDCn), p_li->rectList.bottom - p_li->rectList.top,
    p_li->bDCn, 0, y);

  return y;
}

void co_Cleare_List(LIST_VIEW_CONTROL * p_li, int numofitems, int hdc,
  int xcor, int ycor)
{
  int i;

  ddxReleaseBitmap(p_li->bDCn);
  ddxReleaseBitmap(p_li->bDCs);

  if (p_li->piValue) {
    for (i = 0; i < p_li->listnum; i++) {
      if (p_li->piValue[i].cValue)
        free((void *) p_li->piValue[i].cValue);

      if (p_li->piValue[i].wcValue)
        free((void *) p_li->piValue[i].wcValue);
    }

    free((void *) p_li->piValue);
    p_li->piValue = NULL;

    p_li->piValue =
      (LIST_VIEW_ITEM *) malloc(numofitems * sizeof(LIST_VIEW_ITEM));
    ZeroMemory(p_li->piValue, numofitems * sizeof(LIST_VIEW_ITEM));
  }

  p_li->bClck = 0;
  p_li->bDblClck = 0;
  p_li->bClck = 0;
  p_li->Pushc = 0;
  p_li->listnum = numofitems;
  p_li->dx = 0;
  p_li->bIn = 0;
  p_li->bInE = 0;
  p_li->mpos = p_li->mpmin;

  p_li->bDCn =
    ddxCreateSurface(p_li->rectList.right - p_li->rectList.left,
    numofitems * 30, ddxFindFreeSurface());
  ddxCleareSurfaceColor(p_li->bDCn, 0);

  p_li->bDCs =
    ddxCreateSurface(p_li->rectList.right - p_li->rectList.left,
    numofitems * 30, ddxFindFreeSurface());
  ddxCleareSurfaceColor(p_li->bDCs, 0);

  ddxBitBlt(hdc, p_li->rectMover.left + xcor, p_li->rectMover.top + ycor,
    ddxGetWidth(p_li->bDCm), ddxGetHight(p_li->bDCm), p_li->bDCm, 0, 0);

  p_li->rectMover.top = p_li->mpos - 12;
  p_li->rectMover.bottom = p_li->mpos + 12;

  ddxBitBlt(p_li->bDCm, 0, 0, ddxGetWidth(p_li->bDCm),
    ddxGetHight(p_li->bDCm), hdc, p_li->rectMover.left + xcor,
    p_li->rectMover.top + ycor);

  ddxTransparentBlt(hdc, p_li->rectMover.left + xcor,
    p_li->rectMover.top + ycor,
    ddxGetWidth(hdcCH.hdcCheck),
    24,
    hdcCH.hdcCheck, 0, 0, ddxGetWidth(hdcCH.hdcCheck), 24, RGB(237, 77, 0));
}

void co_Release_List(LIST_VIEW_CONTROL * p_li)
{
  int i;

  //co_Release_Bitmap(&p_li->bDCm);
  ddxReleaseBitmap(p_li->bDCm);
  //co_Release_Bitmap(&p_li->bDCn);
  ddxReleaseBitmap(p_li->bDCn);
  //co_Release_Bitmap(&p_li->bDCs);
  ddxReleaseBitmap(p_li->bDCs);

  if (p_li->piValue) {
    for (i = 0; i < p_li->listnum; i++) {
      if (p_li->piValue[i].cValue)
        free((void *) p_li->piValue[i].cValue);

      if (p_li->piValue[i].wcValue)
        free((void *) p_li->piValue[i].wcValue);
    }

    free((void *) p_li->piValue);
  }

  free((void *) p_li);
}

LIST_VIEW_CONTROL *co_Create_List(int hdc, int x, int y, int width, int hight,
  int id, int numofitems, int bSelection)
{
  int ax, ay, lx, ly;
  LIST_VIEW_CONTROL *p_li = NULL;

  p_li = (LIST_VIEW_CONTROL *) malloc(sizeof(LIST_VIEW_CONTROL));

  //kprintf(1, "alokuji %d bytu", sizeof(LIST_VIEW_CONTROL));

  if (!p_li)
    return 0;

  p_li->piValue =
    (LIST_VIEW_ITEM *) malloc(numofitems * sizeof(LIST_VIEW_ITEM));

  ZeroMemory(p_li->piValue, numofitems * sizeof(LIST_VIEW_ITEM));

  //kprintf(1, "alokuji %d bytu", numofitems * sizeof(LIST_VIEW_ITEM));

  p_li->bClck = 0;
  p_li->bDblClck = 0;
  p_li->bClck = 0;
  p_li->pTime = timeGetTime();
  p_li->cClckSel = -1;
  p_li->Pushc = 0;
  p_li->listnum = numofitems;
  p_li->dx = 0;
  p_li->bIn = 0;
  p_li->bInE = 0;
  p_li->x = x;
  p_li->y = y;
  p_li->mpos = 0;
  p_li->listID = id;
  p_li->rectList.top = y + ddxGetHight(hdcLI.hdcListTop);
  p_li->rectList.left = x + ddxGetWidth(hdcLI.hdcListLeft);
  p_li->rectList.bottom = y + hight - ddxGetHight(hdcLI.hdcListBottom);
  p_li->rectList.right =
    x + width - ddxGetWidth(hdcLI.hdcListRight) -
    ddxGetWidth(hdcLI.hdcListUp);
  p_li->bSelection = bSelection;

  ax = x;
  ay = y;

  /*BitBlt(hdc, x, y, _2dd.bitmap[hdcLI.hdcListTL].bitmap.bmWidth, 
     _2dd.bitmap[hdcLI.hdcListTL].bitmap.bmHeight, _2dd.bitmap[hdcLI.hdcListTL].bitmapDC, 0, 0, SRCCOPY); */

  ddxBitBlt(hdc, x, y, ddxGetWidth(hdcLI.hdcListTL),
    ddxGetHight(hdcLI.hdcListTL), hdcLI.hdcListTL, 0, 0);

  ax += ddxGetWidth(hdcLI.hdcListTL);

  lx = ax;

  while (ax - lx < width - ddxGetWidth(hdcLI.hdcListTop)) {
    /*BitBlt(hdc, ax, ay, _2dd.bitmap[hdcLI.hdcListTop].bitmap.bmWidth, 
       _2dd.bitmap[hdcLI.hdcListTop].bitmap.bmHeight, _2dd.bitmap[hdcLI.hdcListTop].bitmapDC, 0, 0, SRCCOPY); */

    ddxBitBlt(hdc, ax, ay, ddxGetWidth(hdcLI.hdcListTop),
      ddxGetHight(hdcLI.hdcListTop), hdcLI.hdcListTop, 0, 0);

    ax += ddxGetWidth(hdcLI.hdcListTop);
  }

  /*BitBlt(hdc, x + width - _2dd.bitmap[hdcLI.hdcListTop].bitmap.bmWidth, ay, _2dd.bitmap[hdcLI.hdcListTop].bitmap.bmWidth, 
     _2dd.bitmap[hdcLI.hdcListTop].bitmap.bmHeight, _2dd.bitmap[hdcLI.hdcListTop].bitmapDC, 0, 0, SRCCOPY); */

  ddxBitBlt(hdc, x + width - ddxGetWidth(hdcLI.hdcListTop), ay,
    ddxGetWidth(hdcLI.hdcListTop), ddxGetHight(hdcLI.hdcListTop),
    hdcLI.hdcListTop, 0, 0);

  /*BitBlt(hdc, x + width - _2dd.bitmap[hdcLI.hdcListTR].bitmap.bmWidth, ay, _2dd.bitmap[hdcLI.hdcListTR].bitmap.bmWidth, 
     _2dd.bitmap[hdcLI.hdcListTR].bitmap.bmHeight, _2dd.bitmap[hdcLI.hdcListTR].bitmapDC, 0, 0, SRCCOPY); */

  ddxBitBlt(hdc, x + width - ddxGetWidth(hdcLI.hdcListTR), ay,
    ddxGetWidth(hdcLI.hdcListTR), ddxGetHight(hdcLI.hdcListTR),
    hdcLI.hdcListTR, 0, 0);

  ay += ddxGetHight(hdcLI.hdcListTop);

  ly = ay;
  ax = x;

  while (ay - ly < hight - ddxGetHight(hdcLI.hdcListLeft)) {
    /*BitBlt(hdc, ax, ay, _2dd.bitmap[hdcLI.hdcListLeft].bitmap.bmWidth, 
       _2dd.bitmap[hdcLI.hdcListLeft].bitmap.bmHeight, _2dd.bitmap[hdcLI.hdcListLeft].bitmapDC, 0, 0, SRCCOPY); */

    ddxBitBlt(hdc, ax, ay, ddxGetWidth(hdcLI.hdcListLeft),
      ddxGetHight(hdcLI.hdcListLeft), hdcLI.hdcListLeft, 0, 0);

    /*BitBlt(hdc, ax + width - _2dd.bitmap[hdcLI.hdcListRight].bitmap.bmWidth, ay, _2dd.bitmap[hdcLI.hdcListRight].bitmap.bmWidth, 
       _2dd.bitmap[hdcLI.hdcListRight].bitmap.bmHeight, _2dd.bitmap[hdcLI.hdcListRight].bitmapDC, 0, 0, SRCCOPY); */

    ddxBitBlt(hdc, ax + width - ddxGetWidth(hdcLI.hdcListRight), ay,
      ddxGetWidth(hdcLI.hdcListRight), ddxGetHight(hdcLI.hdcListRight),
      hdcLI.hdcListRight, 0, 0);

    ay += ddxGetHight(hdcLI.hdcListLeft);
  }


  /*BitBlt(hdc, ax, y + hight - _2dd.bitmap[hdcLI.hdcListLeft].bitmap.bmHeight, _2dd.bitmap[hdcLI.hdcListLeft].bitmap.bmWidth, 
     _2dd.bitmap[hdcLI.hdcListLeft].bitmap.bmHeight, _2dd.bitmap[hdcLI.hdcListLeft].bitmapDC, 0, 0, SRCCOPY); */

  ddxBitBlt(hdc, ax, y + hight - ddxGetHight(hdcLI.hdcListLeft),
    ddxGetWidth(hdcLI.hdcListLeft), ddxGetHight(hdcLI.hdcListLeft),
    hdcLI.hdcListLeft, 0, 0);

  /*BitBlt(hdc, ax + width - _2dd.bitmap[hdcLI.hdcListRight].bitmap.bmWidth, y + hight - _2dd.bitmap[hdcLI.hdcListRight].bitmap.bmHeight, 
     _2dd.bitmap[hdcLI.hdcListRight].bitmap.bmWidth, _2dd.bitmap[hdcLI.hdcListRight].bitmap.bmHeight, 
     _2dd.bitmap[hdcLI.hdcListRight].bitmapDC, 0, 0, SRCCOPY); */

  ddxBitBlt(hdc, ax + width - ddxGetWidth(hdcLI.hdcListRight),
    y + hight - ddxGetHight(hdcLI.hdcListRight),
    ddxGetWidth(hdcLI.hdcListRight), ddxGetHight(hdcLI.hdcListRight),
    hdcLI.hdcListRight, 0, 0);

  ay = y + hight - ddxGetHight(hdcLI.hdcListBottom);
  ax = x;

  /*BitBlt(hdc, x, ay, _2dd.bitmap[hdcLI.hdcListBL].bitmap.bmWidth, 
     _2dd.bitmap[hdcLI.hdcListBL].bitmap.bmHeight, _2dd.bitmap[hdcLI.hdcListBL].bitmapDC, 0, 0, SRCCOPY); */

  ddxBitBlt(hdc, x, ay, ddxGetWidth(hdcLI.hdcListBL),
    ddxGetHight(hdcLI.hdcListBL), hdcLI.hdcListBL, 0, 0);

  ax += ddxGetWidth(hdcLI.hdcListBL);

  lx = ax;

  while (ax - lx < width - ddxGetWidth(hdcLI.hdcListBottom)) {
    /*BitBlt(hdc, ax, ay, _2dd.bitmap[hdcLI.hdcListBottom].bitmap.bmWidth, 
       _2dd.bitmap[hdcLI.hdcListBottom].bitmap.bmHeight, _2dd.bitmap[hdcLI.hdcListBottom].bitmapDC, 0, 0, SRCCOPY); */

    ddxBitBlt(hdc, ax, ay, ddxGetWidth(hdcLI.hdcListBottom),
      ddxGetHight(hdcLI.hdcListBottom), hdcLI.hdcListBottom, 0, 0);

    ax += ddxGetWidth(hdcLI.hdcListBottom);
  }

  /*BitBlt(hdc, x + width - _2dd.bitmap[hdcLI.hdcListBottom].bitmap.bmWidth, ay, _2dd.bitmap[hdcLI.hdcListBottom].bitmap.bmWidth, 
     _2dd.bitmap[hdcLI.hdcListBottom].bitmap.bmHeight, _2dd.bitmap[hdcLI.hdcListBottom].bitmapDC, 0, 0, SRCCOPY); */

  ddxBitBlt(hdc, x + width - ddxGetWidth(hdcLI.hdcListBottom), ay,
    ddxGetWidth(hdcLI.hdcListBottom), ddxGetHight(hdcLI.hdcListBottom),
    hdcLI.hdcListBottom, 0, 0);

  /*BitBlt(hdc, x + width - _2dd.bitmap[hdcLI.hdcListBR].bitmap.bmWidth, ay, _2dd.bitmap[hdcLI.hdcListBR].bitmap.bmWidth, 
     _2dd.bitmap[hdcLI.hdcListBR].bitmap.bmHeight, _2dd.bitmap[hdcLI.hdcListBR].bitmapDC, 0, 0, SRCCOPY); */

  ddxBitBlt(hdc, x + width - ddxGetWidth(hdcLI.hdcListBR), ay,
    ddxGetWidth(hdcLI.hdcListBR), ddxGetHight(hdcLI.hdcListBR),
    hdcLI.hdcListBR, 0, 0);

  p_li->rectUp.top = y + ddxGetHight(hdcLI.hdcListTop);
  p_li->rectUp.bottom = p_li->rectUp.top + ddxGetHight(hdcLI.hdcListUp);
  p_li->rectUp.left =
    x + width - ddxGetWidth(hdcLI.hdcListBR) - ddxGetWidth(hdcLI.hdcListUp);
  p_li->rectUp.right = p_li->rectUp.left + ddxGetWidth(hdcLI.hdcListUp);

  /*TransparentBltU(hdc, x + width - _2dd.bitmap[hdcLI.hdcListBR].bitmap.bmWidth - _2dd.bitmap[hdcLI.hdcListUp].bitmap.bmWidth, 
     y + _2dd.bitmap[hdcLI.hdcListTop].bitmap.bmHeight, 
     _2dd.bitmap[hdcLI.hdcListUp].bitmap.bmWidth, 
     _2dd.bitmap[hdcLI.hdcListUp].bitmap.bmHeight, 
     _2dd.bitmap[hdcLI.hdcListUp].bitmapDC, 
     0, 0, 
     _2dd.bitmap[hdcLI.hdcListUp].bitmap.bmWidth, 
     _2dd.bitmap[hdcLI.hdcListUp].bitmap.bmHeight, RGB(238, 77, 0)); */

  ddxTransparentBlt(hdc,
    x + width - ddxGetWidth(hdcLI.hdcListBR) - ddxGetWidth(hdcLI.hdcListUp),
    y + ddxGetHight(hdcLI.hdcListTop), ddxGetWidth(hdcLI.hdcListUp),
    ddxGetHight(hdcLI.hdcListUp), hdcLI.hdcListUp, 0, 0,
    ddxGetWidth(hdcLI.hdcListUp), ddxGetHight(hdcLI.hdcListUp), RGB(238, 77,
      0));

  p_li->rectDown.top =
    y + hight - ddxGetHight(hdcLI.hdcListBR) - ddxGetHight(hdcLI.hdcListDown);
  p_li->rectDown.bottom = p_li->rectDown.top + ddxGetHight(hdcLI.hdcListUp);
  p_li->rectDown.left =
    x + width - ddxGetWidth(hdcLI.hdcListTR) - ddxGetWidth(hdcLI.hdcListUp);
  p_li->rectDown.right = p_li->rectDown.left + ddxGetWidth(hdcLI.hdcListUp);

  /*TransparentBltU(hdc, x + width - _2dd.bitmap[hdcLI.hdcListTR].bitmap.bmWidth - _2dd.bitmap[hdcLI.hdcListUp].bitmap.bmWidth, 
     y + hight - _2dd.bitmap[hdcLI.hdcListBR].bitmap.bmWidth - _2dd.bitmap[hdcLI.hdcListDown].bitmap.bmHeight, 
     _2dd.bitmap[hdcLI.hdcListDown].bitmap.bmWidth, 
     _2dd.bitmap[hdcLI.hdcListDown].bitmap.bmHeight, 
     _2dd.bitmap[hdcLI.hdcListDown].bitmapDC, 
     0, 0, 
     _2dd.bitmap[hdcLI.hdcListDown].bitmap.bmWidth, 
     _2dd.bitmap[hdcLI.hdcListDown].bitmap.bmHeight, RGB(238, 77, 0)); */

  ddxTransparentBlt(hdc,
    x + width - ddxGetWidth(hdcLI.hdcListTR) - ddxGetWidth(hdcLI.hdcListUp),
    y + hight - ddxGetHight(hdcLI.hdcListBR) - ddxGetHight(hdcLI.hdcListDown),
    ddxGetWidth(hdcLI.hdcListDown), ddxGetHight(hdcLI.hdcListDown),
    hdcLI.hdcListDown, 0, 0, ddxGetWidth(hdcLI.hdcListDown),
    ddxGetHight(hdcLI.hdcListDown), RGB(238, 77, 0));

  //co_CreateDC(hdc, _2dd.bitmap[hdcCH.hdcCheck].bitmap.bmWidth, 24, &p_li->bDCm);

  p_li->bDCm =
    ddxCreateSurface(ddxGetWidth(hdcCH.hdcCheck), 20, ddxFindFreeSurface());

  /*BitBlt(p_li->bDCm.hdc, 0, 0, p_li->bDCm.x, p_li->bDCm.y, hdc, 
     x + width - _2dd.bitmap[hdcLI.hdcListBR].bitmap.bmWidth - _2dd.bitmap[hdcCH.hdcCheck].bitmap.bmWidth - 2, 
     y + _2dd.bitmap[hdcLI.hdcListTop].bitmap.bmHeight + _2dd.bitmap[hdcLI.hdcListUp].bitmap.bmHeight, 
     SRCCOPY); */

  ddxBitBlt(p_li->bDCm, 0, 0, ddxGetWidth(p_li->bDCm),
    ddxGetHight(p_li->bDCm), hdc,
    x + width - ddxGetWidth(hdcLI.hdcListBR) - ddxGetWidth(hdcCH.hdcCheck) -
    2, y + ddxGetHight(hdcLI.hdcListTop) + ddxGetHight(hdcLI.hdcListUp));

  /*TransparentBltU(hdc, x + width - _2dd.bitmap[hdcLI.hdcListBR].bitmap.bmWidth - _2dd.bitmap[hdcCH.hdcCheck].bitmap.bmWidth - 2, 
     y + _2dd.bitmap[hdcLI.hdcListTop].bitmap.bmHeight + _2dd.bitmap[hdcLI.hdcListUp].bitmap.bmHeight, 
     _2dd.bitmap[hdcCH.hdcCheck].bitmap.bmWidth, 
     24, 
     _2dd.bitmap[hdcCH.hdcCheck].bitmapDC, 
     0, 0, 
     _2dd.bitmap[hdcCH.hdcCheck].bitmap.bmWidth, 
     24, RGB(237, 77, 0)); */

  ddxTransparentBlt(hdc,
    x + width - ddxGetWidth(hdcLI.hdcListBR) - ddxGetWidth(hdcCH.hdcCheck),
    y + ddxGetHight(hdcLI.hdcListTop) + ddxGetHight(hdcLI.hdcListUp),
    ddxGetWidth(hdcCH.hdcCheck), 20, hdcCH.hdcCheck, 0, 49,
    ddxGetWidth(hdcCH.hdcCheck), 20, RGB(237, 77, 0));

  p_li->rectMover.top =
    y + ddxGetHight(hdcLI.hdcListTop) + ddxGetHight(hdcLI.hdcListUp);
  p_li->rectMover.bottom = p_li->rectMover.top + 24;
  p_li->rectMover.left =
    x + width - ddxGetWidth(hdcLI.hdcListBR) - ddxGetWidth(hdcCH.hdcCheck);
  p_li->rectMover.right = p_li->rectMover.left + ddxGetWidth(hdcCH.hdcCheck);

  //co_CreateDC(hdc, p_li->rectList.right - p_li->rectList.left, numofitems * 30, &p_li->bDCn);
  p_li->bDCn =
    ddxCreateSurface(p_li->rectList.right - p_li->rectList.left,
    numofitems * 30, ddxFindFreeSurface());
  ddxCleareSurfaceColor(p_li->bDCn, 0);

  //co_CreateDC(hdc, p_li->rectList.right - p_li->rectList.left, numofitems * 35, &p_li->bDCs);
  p_li->bDCs =
    ddxCreateSurface(p_li->rectList.right - p_li->rectList.left,
    numofitems * 30, ddxFindFreeSurface());
  ddxCleareSurfaceColor(p_li->bDCs, 0);

  /*BitBlt(hdc, x + _2dd.bitmap[hdcLI.hdcListLeft].bitmap.bmWidth,
     y + _2dd.bitmap[hdcLI.hdcListTop].bitmap.bmHeight,
     p_li->bDCn.x,
     p_li->rectList.bottom - p_li->rectList.top,
     p_li->bDCn.hdc,
     0, 0, SRCCOPY); */

  ddxBitBlt(hdc, x + ddxGetWidth(hdcLI.hdcListLeft),
    y + ddxGetHight(hdcLI.hdcListTop),
    ddxGetWidth(p_li->bDCn),
    p_li->rectList.bottom - p_li->rectList.top, p_li->bDCn, 0, 0);

  p_li->mpos = p_li->mpmin =
    y + ddxGetHight(hdcLI.hdcListTop) + ddxGetHight(hdcLI.hdcListUp) + 12;
  p_li->mpmax =
    y + hight - ddxGetHight(hdcLI.hdcListBottom) -
    ddxGetHight(hdcLI.hdcListDown) - 12;

  p_li->rectMoverA.left = p_li->rectMover.left;
  p_li->rectMoverA.right = p_li->rectMover.right;
  p_li->rectMoverA.top =
    y + ddxGetHight(hdcLI.hdcListTop) + ddxGetHight(hdcLI.hdcListUp) + 12;
  p_li->rectMoverA.bottom =
    y + hight - ddxGetHight(hdcLI.hdcListBottom) -
    ddxGetHight(hdcLI.hdcListDown) - 12;


  return p_li;
}

int co_Rect_Hit(RECT rect, int x, int y)
{
  if (x >= rect.left && x <= rect.right && y >= rect.top && y <= rect.bottom)
    return 1;
  else
    return 0;
}

void co_Del_Combo_List(COMBO_CONTROL * p_co, int hdc, int xcor, int ycor)
{
  int y = ycor + p_co->y + ddxGetHight(hdcCO.hdcCombo);
  int i;

  for (i = 0; i < p_co->CounfOfItemsL; i++) {
    /*BitBlt(hdc, p_co->x + xcor, y, 
       p_co->coListRect.right - p_co->coListRect.left - 2, _2dd.bitmap[hdcCO.hdcComboMid].bitmap.bmHeight,
       _2dd.bitmap[hdcCO.hdcComboMid].bitmapDC, 0, 0, SRCCOPY); */

    ddxBitBlt(hdc, p_co->x + xcor, y,
      p_co->coListRect.right - p_co->coListRect.left - 2,
      ddxGetHight(hdcCO.hdcComboMid), hdcCO.hdcComboMid, 0, 0);

    y += ddxGetHight(hdcCO.hdcComboMid);
  }
}

int co_Handle_Combo_Drop(COMBO_DROP_CONTROL * p_co, char bFocus, int x, int y,
  int hdc, int xcor, int ycor)
{
  int c = 0;

  if (dim.t1)
    if (co_Rect_Hit(p_co->coLUp, x, y)) {
      p_co->Selected--;

      if (p_co->Selected < 0)
        p_co->Selected = 0;

      co_Combo_Drop_Set_String(hdc, p_co, xcor, ycor);

      c++;
    }

  if (dim.t1)
    if (co_Rect_Hit(p_co->coLDown, x, y)) {
      p_co->Selected++;

      if (p_co->Selected >= p_co->CounfOfItems)
        p_co->Selected = p_co->CounfOfItems - 1;

      co_Combo_Drop_Set_String(hdc, p_co, xcor, ycor);

      c++;
    }

  return c;
}

int co_Handle_Combo(COMBO_CONTROL * p_co, char bFocus, int x, int y, int hdc,
  int xcor, int ycor, char b_list)
{
	int c = 0;
	RECT r;

	if(dim.t1 && b_list && !p_co->bList)
		return 0;

	if(!dim.dt1)
		p_co->bIn = 0;

	if(dim.t1 || dim.dt1) {
    if(co_Rect_Hit(p_co->coLMovA, x+xcor, y+ycor) && p_co->bList)
    {
      int ym;
      int	ymcor;
      int	ymx;
      int	i;
  
      COMBO_HADLE_MOVER:
  
      ym = (y + ycor - p_co->xmstart);
      ymcor = (int)floor(ddxGetHight(hdcCO.hdcComboMover) / 2.0f);
      ymx = p_co->xm;
  
      if(ym < 0)
        ym = 0;
  
      if(ym > ymx)
        ym = ymx;
  
      i = ftoi(((p_co->CounfOfItems - p_co->CounfOfItemsL) * ym) / (float)ymx);
  
      p_co->SSelected = i;
  
      p_co->CSelected = p_co->Selected - 1;
  
      co_Combo_Draw_List(hdc, p_co, xcor, ycor, 1);
  
      r.left = p_co->x + xcor;
      r.top = p_co->y + ddxGetHight(hdcCO.hdcCombo) + ycor;
      r.right = p_co->WidthR;
      r.bottom = p_co->ListMaxHight + ddxGetHight(hdcCO.hdcComboEnd);
  
      _2d_Add_RectItem(&rline, r, 1);	
      
      if(ym + p_co->xmstart - ymcor != p_co->coLMov.top)
      {
        int t,b;
  
        t = ym + p_co->xmstart - ymcor;
        b = p_co->coLMov.top + ddxGetHight(hdcCO.hdcComboMover);
  
        ddxSetFlip(0);
  
        for(i=0;i<2;i++)
        {
          ddxBitBlt(hdc, p_co->coLMov.left, p_co->coLMov.top, ddxGetWidth(hdcCO.hdcComboMoverb), 
                ddxGetHight(hdcCO.hdcComboMoverb), hdcCO.hdcComboMoverb, 0, 0);
  
          ddxBitBlt(hdc, p_co->coLMov.left, t, ddxGetWidth(hdcCO.hdcComboMover), 
                ddxGetHight(hdcCO.hdcComboMover), hdcCO.hdcComboMover, 0, 0);
  
          if(!i)
            DisplayFrame();
        }
  
        p_co->coLMov.top = t;
        p_co->coLMov.bottom = b;
  
        ddxSetFlip(1);
      }
  
      p_co->bIn = 1;
      c++;
      return c;
    }
    else {
      if(dim.dt1 && p_co->bIn)
        goto COMBO_HADLE_MOVER;
      else
        p_co->bIn = 0;
    }
  }

	if(dim.t1) {
    if(co_Rect_Hit(p_co->coLUp, x+xcor, y+ycor) && p_co->bList)
    {
      int	ymcor = (int)floor(ddxGetHight(hdcCO.hdcComboMover) / 2.0f);
      int t,b;
      int ny, i;
  
      p_co->SSelected--;
  
      if(p_co->SSelected < 0)
        p_co->SSelected = 0;
  
      p_co->CSelected = p_co->Selected - 1;
  
      //co_Del_Combo_List(p_co, hdc, xcor, ycor);
      co_Combo_Draw_List(hdc, p_co, xcor, ycor, 1);
  
      r.left = p_co->x + xcor;
      r.top = p_co->y + ddxGetHight(hdcCO.hdcCombo) + ycor;
      r.right = p_co->WidthR;
      r.bottom = p_co->ListMaxHight + ddxGetHight(hdcCO.hdcComboEnd);
  
      _2d_Add_RectItem(&rline, r, 1);	
  
      ny = ftoi((p_co->xm / (float)(p_co->CounfOfItems - p_co->CounfOfItemsL)) * p_co->SSelected);
  
      t = ny + p_co->xmstart - ymcor;
      b = p_co->coLMov.top + ddxGetHight(hdcCO.hdcComboMover);
  
      ddxSetFlip(0);
  
      for(i=0;i<2;i++)
      {
        ddxBitBlt(hdc, p_co->coLMov.left, p_co->coLMov.top, ddxGetWidth(hdcCO.hdcComboMoverb), 
              ddxGetHight(hdcCO.hdcComboMoverb), hdcCO.hdcComboMoverb, 0, 0);
  
        ddxBitBlt(hdc, p_co->coLMov.left, t, ddxGetWidth(hdcCO.hdcComboMover), 
              ddxGetHight(hdcCO.hdcComboMover), hdcCO.hdcComboMover, 0, 0);
  
        if(!i)
          DisplayFrame();
      }
  
      p_co->coLMov.top = t;
      p_co->coLMov.bottom = b;
  
      ddxSetFlip(1);
  
      c++;
      return c;
    }
  }

	if(dim.t1) {
    if(co_Rect_Hit(p_co->coLDown, x+xcor, y+ycor) && p_co->bList)
    {
      int	ymcor = (int)floor(ddxGetHight(hdcCO.hdcComboMover) / 2.0f);
      int t,b;
      int ny, i;
      p_co->SSelected++;
  
      if(p_co->SSelected > p_co->CounfOfItems - p_co->CounfOfItemsL)
        p_co->SSelected = p_co->CounfOfItems - p_co->CounfOfItemsL;
  
      p_co->CSelected = p_co->Selected - 1;
      
      //co_Del_Combo_List(p_co, hdc,xcor, ycor);
      co_Combo_Draw_List(hdc, p_co, xcor, ycor, 1);
      
      r.left = p_co->x + xcor;
      r.top = p_co->y + ddxGetHight(hdcCO.hdcCombo) + ycor;
      r.right = p_co->WidthR;
      r.bottom = p_co->ListMaxHight + ddxGetHight(hdcCO.hdcComboEnd);
  
      _2d_Add_RectItem(&rline, r, 1);
  
      ny = ftoi((p_co->xm / (float)(p_co->CounfOfItems - p_co->CounfOfItemsL)) * p_co->SSelected);
  
      t = ny + p_co->xmstart - ymcor;
      b = p_co->coLMov.top + ddxGetHight(hdcCO.hdcComboMover);
  
      ddxSetFlip(0);
  
      for(i=0;i<2;i++)
      {
        ddxBitBlt(hdc, p_co->coLMov.left, p_co->coLMov.top, ddxGetWidth(hdcCO.hdcComboMoverb), 
              ddxGetHight(hdcCO.hdcComboMoverb), hdcCO.hdcComboMoverb, 0, 0);
  
        ddxBitBlt(hdc, p_co->coLMov.left, t, ddxGetWidth(hdcCO.hdcComboMover), 
              ddxGetHight(hdcCO.hdcComboMover), hdcCO.hdcComboMover, 0, 0);
  
        if(!i)
          DisplayFrame();
      }
  
      p_co->coLMov.top = t;
      p_co->coLMov.bottom = b;
  
      ddxSetFlip(1);
  
      c++;
      return c;
    }
  }

	if(dim.t1) {
    if(co_Rect_Hit(p_co->coDownRect, x, y))
    {
    /*r.left = p_co->x + xcor;
       r.top = p_co->y + ddxGetHight(hdcCO.hdcCombo) + ycor;
       r.right = p_co->WidthR;
       r.bottom = p_co->ListMaxHightR;
  
       _2d_Add_RectItem(&rline, r, 1); */
  
      if(p_co->bList)
      {
        p_co->Selected = p_co->OSelected;
        co_Combo_Close(hdc, p_co, xcor, ycor);
      }
      else
      {
        p_co->OSelected = p_co->Selected;
        co_Combo_Open(hdc, p_co, xcor, ycor);
      }
  
      r.left = p_co->x + xcor;
      r.top = p_co->y + ddxGetHight(hdcCO.hdcCombo) + ycor;
      r.right = p_co->WidthR;
      r.bottom = p_co->ListMaxHight + ddxGetHight(hdcCO.hdcComboEnd);
  
      _2d_Add_RectItem(&rline, r, 1);
  
      c++;
    }
  }

	if(co_Rect_Hit(p_co->coListRect, x, y) && p_co->bList)
	{
		c++;

		p_co->CSelected = p_co->SSelected + co_Combo_Hit2Sel(p_co, x, y);

		if(!dim.t1 && p_co->bList)
		{
			co_Combo_Draw_List(hdc, p_co, xcor, ycor, 0);
			p_co->Selected = p_co->CSelected;
		}
		else
			if(dim.t1 && p_co->bList)
			{
				p_co->OSelected = p_co->Selected = p_co->CSelected;
				co_Combo_Set_String(hdc, p_co, xcor, ycor);
				co_Combo_Close(hdc, p_co, xcor, ycor);

				r.left = p_co->x + xcor;
				r.top = p_co->y + ddxGetHight(hdcCO.hdcCombo) + ycor;
				r.right = p_co->WidthR;
				r.bottom = p_co->ListMaxHight + ddxGetHight(hdcCO.hdcComboEnd);

				_2d_Add_RectItem(&rline, r, 1);
			}
	}
	else
		if(!dim.t1 && p_co->bList)
		{
			c++;
			p_co->CSelected = -1;
			co_Combo_Draw_List(hdc, p_co, xcor, ycor, 0);
			p_co->Selected = -1;

			r.left = p_co->x + xcor;
			r.top = p_co->y + ddxGetHight(hdcCO.hdcCombo) + ycor;
			r.right = p_co->WidthR;
			r.bottom = p_co->ListMaxHight + ddxGetHight(hdcCO.hdcComboEnd);

			_2d_Add_RectItem(&rline, r, 1);
		}


	if(dim.t1)
	if(!c && bFocus)
	{
		co_Combo_Close(hdc, p_co, xcor, ycor);

		p_co->Selected = p_co->OSelected;

		r.left = p_co->x + xcor;
		r.top = p_co->y + ddxGetHight(hdcCO.hdcCombo) + ycor;
		r.right = p_co->WidthR;
		r.bottom = p_co->ListMaxHight + ddxGetHight(hdcCO.hdcComboEnd);

		_2d_Add_RectItem(&rline, r, 1);
	}

	if(bFocus)
	{
		if(p_co->bList)
		{
			r.left = p_co->x + xcor;
			r.top = p_co->y + ddxGetHight(hdcCO.hdcCombo) + ycor;
			r.right = p_co->WidthR;
			r.bottom = p_co->ListMaxHight + ddxGetHight(hdcCO.hdcComboEnd);

			_2d_Add_RectItem_IfNPresent(&rline, r, 1);
		}
	
		r.left = p_co->x + xcor;
		r.top = p_co->y + ycor;
		r.right = ddxGetWidth(hdcCO.hdcCombo);
		r.bottom = ddxGetHight(hdcCO.hdcCombo);

		_2d_Add_RectItem_IfNPresent(&rline, r, 1);
	}

	return c;
}

int co_Handle_Button(BUTTON_CONTROL * p_bu, int x, int y)
{
  if (co_Rect_Hit(p_bu->Rect, x, y)) {
    p_bu->bActivated = 1;
    return 1;
  }

  return 0;
}

int co_Handle_Checkbox(CHECKBOX_CONTROL * p_ch, int x, int y)
{
	RECT r;
	int bmpx;
	int	bmpDC;

	if(co_Rect_Hit(p_ch->Rect, x, y))
	{
		bmpx = ddxGetWidth(hdcCH.hdcCheck);
		bmpDC = hdcCH.hdcCheck;

		if(!p_ch->bChecked)
		{
			p_ch->bChecked = 1;
			ddxTransparentBltDisplay(p_ch->Rect.left + TAB_X, 
                               p_ch->Rect.top + TAB_Y, 
                               bmpx, 20, bmpDC, 0, 25, 
                               bmpx, 20, RGB(237, 77, 0));
			p_ch->bChange = 1;

			r.left = p_ch->Rect.left + TAB_X;
			r.top = p_ch->Rect.top + TAB_Y;
			r.right = bmpx;
			r.bottom = 20;

			_2d_Add_RectItem(&rline, r, 1);

			if(p_ch->iDC != -1) {
				ddxBitBlt(p_ch->iDC, 0, 0, p_ch->RectFull.right, 
                  p_ch->RectFull.bottom,
                  HDC2DD, p_ch->x + TAB_X, p_ch->y + TAB_Y);
      }
		}
		else
		{
			p_ch->bChecked = 0;
			ddxTransparentBltDisplay(p_ch->Rect.left + TAB_X, 
                               p_ch->Rect.top + TAB_Y, 
                               bmpx, 20, bmpDC, 0, 1, 
                               bmpx, 20, RGB(237, 77, 0));
			p_ch->bChange = 1;

			r.left = p_ch->Rect.left + TAB_X;
			r.top = p_ch->Rect.top + TAB_Y;
			r.right = bmpx;
			r.bottom = 20;

			_2d_Add_RectItem(&rline, r, 1);

			if(p_ch->iDC != -1)
				ddxBitBlt(p_ch->iDC, 0, 0, p_ch->RectFull.right, 
                  p_ch->RectFull.bottom,
                  HDC2DD, p_ch->x + TAB_X, p_ch->y + TAB_Y);
		}

    if(p_ch->p_callback)
      p_ch->p_callback(p_ch);
    
  	return 1;
	}

	p_ch->bChange = 0;
  return 0;
}

int co_Handle_Progres(PROGRES_CONTROL * p_pr, int x, int y)
{
  int i;

  if (bExclusive && !p_pr->bExclusive)
    return 0;

  if (dim.t1 || dim.dt1)
    if (co_Rect_Hit(p_pr->rectProgres, x, y)) {
      int rl;
      //int rr;
      int c;

      if (p_pr->pos == x)
        return 1;

      rl = p_pr->rectMover.left;
      //rr = p_pr->rectMover.right;

      p_pr->rectMover.left = x - p_pr->cor;
      p_pr->rectMover.right = (x - p_pr->cor) + ddxGetWidth(hdcPR.hdcMover);

      ddxSetFlip(0);

      if (!ddxGetMode())
        c = 2;
      else
        c = 1;

      for (i = 0; i < c; i++) {
        ddxBitBltDisplay(rl + TAB_X, p_pr->rectMover.top + TAB_Y,
          ddxGetWidth(p_pr->bDC), ddxGetHight(p_pr->bDC), p_pr->bDC, 0, 0);

        if (i || c == 1)
          ddxBitBlt(p_pr->bDC, 0, 0, ddxGetWidth(p_pr->bDC),
            ddxGetHight(p_pr->bDC), HDC2DD, p_pr->rectMover.left + TAB_X,
            p_pr->rectMover.top + TAB_Y);

        ddxTransparentBltDisplay(p_pr->rectMover.left + TAB_X,
          p_pr->rectMover.top + TAB_Y, ddxGetWidth(hdcPR.hdcMover),
          ddxGetHight(hdcPR.hdcMover), hdcPR.hdcMover, 0, 0,
          ddxGetWidth(hdcPR.hdcMover), ddxGetHight(hdcPR.hdcMover), RGB(237,
            77, 0));
        if (!i)
          DisplayFrame();
      }

      ddxSetFlip(1);

      p_pr->pos = x;
      p_pr->bIn = 1;
      p_pr->bChange = 1;
      p_pr->bExclusive = 1;
      bExclusive = 1;
      return 1;
    }

  if (p_pr->bIn && dim.dt1) {
    int rl;
    //int rr;
    int c;

    /*if(y > p_pr->rectProgres.bottom || y < p_pr->rectProgres.top)
       {
       p_pr->bIn = 0;
       return 0;
       } */

    if (x >= p_pr->rectProgres.right)
      x = p_pr->rectProgres.right;

    if (x <= p_pr->rectProgres.left)
      x = p_pr->rectProgres.left;

    if (p_pr->pos == x)
      return 1;

    p_pr->pos = x;

    rl = p_pr->rectMover.left;
    //rr = p_pr->rectMover.right;

    p_pr->rectMover.left = x - p_pr->cor;
    p_pr->rectMover.right = (x - p_pr->cor) + ddxGetWidth(hdcPR.hdcMover);


    ddxSetFlip(0);

    if (!ddxGetMode())
      c = 2;
    else
      c = 1;

    for (i = 0; i < c; i++) {

      ddxBitBltDisplay(rl + TAB_X, p_pr->rectMover.top + TAB_Y,
        ddxGetWidth(p_pr->bDC), ddxGetHight(p_pr->bDC), p_pr->bDC, 0, 0);

      if (i || c == 1)
        ddxBitBlt(p_pr->bDC, 0, 0, ddxGetWidth(p_pr->bDC),
          ddxGetHight(p_pr->bDC), HDC2DD, p_pr->rectMover.left + TAB_X,
          p_pr->rectMover.top + TAB_Y);

      ddxTransparentBltDisplay(p_pr->rectMover.left + TAB_X,
        p_pr->rectMover.top + TAB_Y, ddxGetWidth(hdcPR.hdcMover),
        ddxGetHight(hdcPR.hdcMover), hdcPR.hdcMover, 0, 0,
        ddxGetWidth(hdcPR.hdcMover), ddxGetHight(hdcPR.hdcMover), RGB(237, 77,
          0));
      if (!i)
        DisplayFrame();
    }

    ddxSetFlip(1);

    p_pr->bIn = 1;
    p_pr->bChange = 1;
    p_pr->bExclusive = 1;
    bExclusive = 1;

    return 1;
  }

  if (!dim.dt1) {
    p_pr->bExclusive = 0;
    bExclusive = 0;
    p_pr->bIn = 0;
  }

  p_pr->bChange = 0;

  return 0;
}

int co_List_Get_Dbclck(CONTROL_LIST_ITEM * p_list, int lsize, int id,
  LIST_VIEW_CONTROL ** p_li)
{
  int i;

  for (i = 0; i < lsize; i++)
    if ((p_list + i)->p_list && (p_list + i)->bActive) {
      *p_li = (p_list + i)->p_list;
      return (p_list + i)->p_list->bDblClck;
    }

  return -1;
}

int co_List_Get_Clck(CONTROL_LIST_ITEM * p_list, int lsize, int id,
  LIST_VIEW_CONTROL ** p_li)
{
  int i;

  for (i = 0; i < lsize; i++)
    if ((p_list + i)->p_list && (p_list + i)->bActive) {
      *p_li = (p_list + i)->p_list;
      return (p_list + i)->p_list->bClck;
    }

  return -1;
}

int co_List_Get_Value(CONTROL_LIST_ITEM * p_list, int lsize, int id,
  int index)
{
  int i;

  for (i = 0; i < lsize; i++)
    if ((p_list + i)->p_list)
      return (p_list + i)->p_list->piValue[index].iValue;

  return -1;
}

void co_get_XP_XT(LIST_VIEW_CONTROL * p_li, int i, int *p_xp, int *p_xt,
  int ycor)
{
//  int xs = p_li->rectList.top + ycor + (i * 30) - p_li->dx;
  int xp = 0;
  int xt = 0;

  if (dim.t1)
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

void co_delete(char *cfile)
{
/*
	char olddir[MAX_PATH+1];
	char dir[MAX_PATH+1];
	char *c;
	long Done, error;
	struct _finddata_t	Data;

	if(!cfile)
		return;

	c =strstr(cfile, ".");

	_getcwd(olddir, MAX_PATH);

	GetPrivateProfileString("game","save_dir","c:\\",dir,MAX_PATH,ini_file);
  working_file_translate(dir,256);

	chdir((dir));

	if(c)	// ma to tecku v filu, tak je to demo, jinak je to save (dir)
		remove(cfile);
	else
	{
		chdir((cfile));

		Done = _findfirst("*.*",&Data);
		error = Done;
			
		while(error != -1)
		{
			if(error != -1)
				if(!(Data.attrib & _A_SUBDIR))
					remove(Data.name);

			error = _findnext(Done,&Data);
		}

		_findclose(Done);

		chdir((dir));
		rmdir(cfile);
	}

	chdir((olddir));
*/
}

int co_Handle_List(LIST_VIEW_CONTROL * p_li, int x, int y, int hdc, int xcor,
  int ycor)
{
	int xp = 0;
	int	xt = 0;

	int c = 0;

	RECT r;
	DWORD	t = timeGetTime();

	if(bBlockList)
		return 0;

	r.left = p_li->rectList.left + xcor;
	r.top = p_li->rectList.top + ycor;
	r.right = ddxGetWidth(p_li->bDCn);
	r.bottom = p_li->rectList.bottom - p_li->rectList.top;

	_2d_Add_RectItem_IfNPresent(&rline, r, 1);

	r.left = p_li->rectMoverA.left + xcor;
	r.top = p_li->rectMoverA.top + ycor - 12;
	r.right = p_li->rectMoverA.right - p_li->rectMoverA.left;
	r.bottom = p_li->rectMoverA.bottom - p_li->rectMoverA.top + 24;

	_2d_Add_RectItem_IfNPresent(&rline, r, 1);
	
	if(dim.dt1 && p_li->bIn > 0)
	{
		if(y < p_li->mpmin)
			y = p_li->mpmin;

		if(y > p_li->mpmax)
			y = p_li->mpmax;

		p_li->bIn = 0;
		goto HANDLE_LISTVIEW;
	}

	if(!dim.dt1)
		p_li->bIn = 0;

	if(p_li->Pushc < 2)
		if(t - p_li->pTime > 1000)
		{
			p_li->pTime = t;
			p_li->Pushc = 0;
			p_li->cSelected = -1;
			p_li->bClck = 0;
			p_li->bDblClck = 0;
		}

	if(co_Rect_Hit(p_li->rectList, x, y))
	{
		int i = (int)floor((p_li->dx + (y - p_li->rectList.top)) / 30.0f);

		if(dim.t1)
		{
			p_li->bClck++;
			p_li->Pushc++;
		}

		if(i < 0 || i > p_li->listnum)
		{
			ddxBitBlt(hdc, p_li->rectList.left + xcor, p_li->rectList.top + ycor, ddxGetWidth(p_li->bDCn), 
			          p_li->rectList.bottom - p_li->rectList.top, p_li->bDCn, 0, p_li->dx);

			p_li->Selected = -1;
			p_li->Pushc = 0;
			p_li->pTime = t;

			if(dim.t1)
			{
				p_li->cSelected = -1;
				p_li->cClckSel = -1;
			}

			c++;
		}
		else
			if(i != p_li->Selected)
			{
				co_get_XP_XT(p_li, i, &xp, &xt, ycor);

				ddxBitBlt(hdc, p_li->rectList.left + xcor, p_li->rectList.top + ycor, ddxGetWidth(p_li->bDCn), 
					      p_li->rectList.bottom - p_li->rectList.top, p_li->bDCn, 0, p_li->dx);

				p_li->Selected = i;

				ddxBitBlt(hdc, p_li->rectList.left + xcor, p_li->rectList.top + ycor + (i * 30) - p_li->dx + xp, 
					      ddxGetWidth(p_li->bDCs), 30 - xp - xt, p_li->bDCs, 0, (i * 30) + xp);

				if(p_li->cClckSel != -1 && p_li->bSelection)
				{
					co_get_XP_XT(p_li, p_li->cClckSel, &xp, &xt, ycor);

					ddxBitBlt(hdc, p_li->rectList.left + xcor, p_li->rectList.top + ycor + (p_li->cClckSel * 30) - p_li->dx + xp, 
						      ddxGetWidth(p_li->bDCs), 30 - xp - xt, p_li->bDCs, 0, (p_li->cClckSel * 30) + xp);
				}

				c++;

			}
			else
			 if(dim.t1)
			 {
				if(p_li->Pushc > 1)
				{
					if(p_li->cSelected == i)
						p_li->bDblClck = 1;

					p_li->Pushc = 0;
					p_li->pTime = t;
				}
				else
					p_li->cSelected = i;

				if(p_li->Pushc == 1)
					p_li->bClck = 1;

				p_li->cClckSel = i;

				if(p_li->bSelection)
				{
					ddxBitBlt(hdc, p_li->rectList.left + xcor, p_li->rectList.top + ycor, ddxGetWidth(p_li->bDCn), 
						      p_li->rectList.bottom - p_li->rectList.top, p_li->bDCn, 0, p_li->dx);

					co_get_XP_XT(p_li, p_li->cClckSel, &xp, &xt, ycor);

					ddxBitBlt(hdc, p_li->rectList.left + xcor, p_li->rectList.top + ycor + (p_li->cClckSel * 30) - p_li->dx + xp, 
						      ddxGetWidth(p_li->bDCs), 30 - xp - xt, p_li->bDCs, 0, (p_li->cClckSel * 30) + xp);
				}
			 }
	}
	else
		if(p_li->Selected != -1)
		{
			p_li->Selected = -1;

			ddxBitBlt(hdc, p_li->rectList.left + xcor, p_li->rectList.top + ycor, ddxGetWidth(p_li->bDCn), 
			          p_li->rectList.bottom - p_li->rectList.top, p_li->bDCn, 0, p_li->dx);	
			
			if(p_li->cClckSel != -1 && p_li->bSelection)
			{
				co_get_XP_XT(p_li, p_li->cClckSel, &xp, &xt, ycor);

				ddxBitBlt(hdc, p_li->rectList.left + xcor, p_li->rectList.top + ycor + (p_li->cClckSel * 30) - p_li->dx + xp, 
						  ddxGetWidth(p_li->bDCs), 30 - xp - xt, p_li->bDCs, 0, (p_li->cClckSel * 30) + xp);
			}

			c++;
		}

	if(dim.t1)
	if(co_Rect_Hit(p_li->rectUp, x, y))
	{
		float dcm = (float)ddxGetHight(p_li->bDCn) - (p_li->rectList.bottom - p_li->rectList.top);
		float c = (p_li->rectMoverA.bottom - p_li->rectMoverA.top) / dcm;

		y = p_li->mpos - ftoi(30 * c);

		p_li->bInE = -1;
		p_li->bIn = -1;

		goto HANDLE_LISTVIEW;
	}

	if(dim.t1)
	if(co_Rect_Hit(p_li->rectDown, x, y))
	{
		float dcm = (float)ddxGetHight(p_li->bDCn) - (p_li->rectList.bottom - p_li->rectList.top);
		float c = (p_li->rectMoverA.bottom - p_li->rectMoverA.top) / dcm;
		
		y = p_li->mpos + ftoi(30 * c);

		p_li->bInE = -1;
		p_li->bIn = -1;

		goto HANDLE_LISTVIEW;
	}

	if(dim.t1 || dim.dt1)
	if(co_Rect_Hit(p_li->rectMoverA, x, y))
	{
		int dcm, mm, pm, pos;
		int rt, j;
    //int rb;

HANDLE_LISTVIEW:

		if(y > p_li->mpmax)
			y = p_li->mpmax;
		else
			if(y < p_li->mpmin)
				y = p_li->mpmin;

		if(p_li->mpos == y)
		{
			p_li->bIn++;
			return 1;
		}

		p_li->mpos = y;

		rt = p_li->rectMover.top;
		//rb = p_li->rectMover.bottom;

		p_li->rectMover.top = p_li->mpos - 12;
		p_li->rectMover.bottom = p_li->mpos + 12;

		if(hdc == HDC2DD)
		{
			int c;

			ddxSetFlip(0);

			if(!ddxGetMode())
				c = 2;
			else
				c = 1;

			for(j=0;j<c;j++)
			{
				ddxBitBlt(hdc, p_li->rectMover.left + xcor, rt + ycor, ddxGetWidth(p_li->bDCm), 
						  ddxGetHight(p_li->bDCm), p_li->bDCm, 0, 0);

	
				if(j)
				ddxBitBlt(p_li->bDCm, 0, 0, ddxGetWidth(p_li->bDCm), ddxGetHight(p_li->bDCm), hdc, 
						  p_li->rectMover.left + xcor, p_li->rectMover.top + ycor);

				ddxTransparentBlt(hdc, p_li->rectMover.left + xcor, 
								  p_li->rectMover.top + ycor, 
								  ddxGetWidth(hdcCH.hdcCheck), 
								  20, 
								  hdcCH.hdcCheck, 
								  0, 49, 
							      ddxGetWidth(hdcCH.hdcCheck), 
				                  20, RGB(237, 77, 0));

				if(!j)
					DisplayFrame();
			}

			ddxSetFlip(1);
		}
		else
		{
				ddxBitBlt(hdc, p_li->rectMover.left + xcor, rt + ycor, ddxGetWidth(p_li->bDCm), 
						  ddxGetHight(p_li->bDCm), p_li->bDCm, 0, 0);

	
				ddxBitBlt(p_li->bDCm, 0, 0, ddxGetWidth(p_li->bDCm), ddxGetHight(p_li->bDCm), hdc, 
						  p_li->rectMover.left + xcor, p_li->rectMover.top + ycor);

				ddxTransparentBlt(hdc, p_li->rectMover.left + xcor, 
								  p_li->rectMover.top + ycor, 
								  ddxGetWidth(hdcCH.hdcCheck), 
								  20, 
								  hdcCH.hdcCheck, 
								  0, 49, 
							      ddxGetWidth(hdcCH.hdcCheck), 
				                  20, RGB(237, 77, 0));
		}

		dcm = ddxGetHight(p_li->bDCn) - (p_li->rectList.bottom - p_li->rectList.top);
		mm = p_li->mpmax - p_li->mpmin;
		pm = p_li->mpos - p_li->mpmin;

		pos = ftoi((pm * dcm) / (float)mm);

		if(p_li->mpos == p_li->mpmax)
			pos = ddxGetHight(p_li->bDCn) - (p_li->rectList.bottom - p_li->rectList.top);

		if(p_li->mpos == p_li->mpmin)
			pos = 0;

		p_li->dx = pos;

		ddxBitBlt(hdc, p_li->rectList.left + xcor, p_li->rectList.top + ycor, ddxGetWidth(p_li->bDCn), 
			      p_li->rectList.bottom - p_li->rectList.top, p_li->bDCn, 0, pos);

		if(p_li->cClckSel != -1 && p_li->bSelection)
		{
			co_get_XP_XT(p_li, p_li->cClckSel, &xp, &xt, ycor);

			ddxBitBlt(hdc, p_li->rectList.left + xcor, p_li->rectList.top + ycor + (p_li->cClckSel * 30) - p_li->dx + xp, 
			      ddxGetWidth(p_li->bDCs), 30 - xp - xt, p_li->bDCs, 0, (p_li->cClckSel * 30) + xp);
		}

		p_li->bIn++;
		p_li->bInE++;
		return 1;
	}

	if(key[K_DEL])
	{
		char *cfile = NULL;

		key[K_DEL] = 0;

		if(co_List_Delete_Item(p_li, &cfile))
		{
			p_li->dx = co_List_Redraw(hdc, p_li, p_li->dx);
			co_delete(cfile);
		}

		if(cfile)
			free((void *) cfile);
	}

	return c;  
}

int co_Is_Button_Activated(CONTROL_LIST_ITEM * p_list, int lsize, int id)
{
  int i;

  for (i = 0; i < lsize; i++)
    if ((p_list + i)->p_button)
      if ((p_list + i)->p_button->ButtonID == id
        && (p_list + i)->p_button->bActivated)
        return 1;

  return 0;
}

int co_Check_Get_State(CONTROL_LIST_ITEM * p_list, int lsize, int id)
{
  int i;

  for (i = 0; i < lsize; i++)
    if ((p_list + i)->p_check)
      if ((p_list + i)->p_check->checkID == id)
        return (p_list + i)->p_check->bChecked;

  return 0;
}

int co_Check_Get_State_Change(CONTROL_LIST_ITEM * p_list, int lsize, int id)
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

void co_Check_Disable(int hdc, int xcor, int ycor, CONTROL_LIST_ITEM * p_list,
  int lsize, int id)
{
  CHECKBOX_CONTROL *p_ch;
  int i;

  for (i = 0; i < lsize; i++)
    if ((p_list + i)->p_check)
      if ((p_list + i)->p_check->checkID == id) {
        p_ch = (p_list + i)->p_check;
        (p_list + i)->bActive = 0;
        (p_list + i)->bDisabled = 1;

        ddxTransparentBlt(hdc, p_ch->RectFull.left + xcor,
          p_ch->RectFull.top + ycor,
          p_ch->RectFull.right,
          p_ch->RectFull.bottom,
          hdcCH.hdcGray,
          0, 0, p_ch->RectFull.right, p_ch->RectFull.bottom, TRANSCOLOR);
        return;
      }

  return;
}

void co_Check_Enable(int hdc, int xcor, int ycor, CONTROL_LIST_ITEM * p_list,
  int lsize, int id)
{
  CHECKBOX_CONTROL *p_ch;
  int i;

  for (i = 0; i < lsize; i++)
    if ((p_list + i)->p_check)
      if ((p_list + i)->p_check->checkID == id) {
        p_ch = (p_list + i)->p_check;
        (p_list + i)->bActive = 1;
        (p_list + i)->bDisabled = 0;

        if (p_ch->iDC != -1)
          ddxBitBlt(hdc, p_ch->RectFull.left + xcor,
            p_ch->RectFull.top + ycor,
            p_ch->RectFull.right, p_ch->RectFull.bottom, p_ch->iDC, 0, 0);
        return;
      }

  return;
}

int co_Handle_Edit_Key_Filter(void)
{
//  char k[256];
  int c = 0, i;

  //GetKeyboardState(k);

  if (key[K_ESC])
    c++;

  if (key[K_PTRSCR])
    c++;

  if (key[K_DEL])
    c++;

  if (key[K_INS])
    c++;

  if (key[K_PGDN])
    c++;

  if (key[K_PGUP])
    c++;

  if (key[K_TAB])
    c++;
/*
	if(k[VK_LCONTROL]&0x80)
		c++;

	if(k[VK_RCONTROL]&0x80)
		c++;

	if(k[VK_LMENU]&0x80)
		c++;

	if(k[VK_RMENU]&0x80)
		c++;
*/
  if (key[K_TAB])
    c++;

  if (key[K_CTRL])
    c++;

  if (key[K_PTRSCR])
    c++;

  if (key[K_ALT])
    c++;

  for (i = 59; i < 84; i++)
    if (key[i])
      c++;

  if (key[K_F11])
    c++;

  if (key[K_F12])
    c++;

  return c;
}

int co_Handle_wsclen(CONTROL_EDIT * p_ed)
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

int draw_edit(CONTROL_EDIT * p_ed, int x, int y, int hdc, int xcor, int ycor)
{
  RECT r;
  int xt, yt;

  ddxBitBlt(hdc, p_ed->rect.left + xcor, p_ed->rect.top + ycor,
    ddxGetWidth(hdcED.hdcEdit), ddxGetHight(hdcED.hdcEdit),
    hdcED.hdcEdit, 0, 0);

  ddxCleareSurface(p_ed->bDC);

  fn_Draw_MessageA(p_ed->bDC, 0, 3, &b2_2d_font.gt, &b2_2d_font.ts,
    p_ed->wtext, 0, &xt, &yt);

  ddxTransparentBlt(hdc, p_ed->rect.left + 5 + xcor,
    p_ed->rect.top + 4 + xcor, p_ed->rect.right - p_ed->rect.left - 10,
    p_ed->rect.bottom - p_ed->rect.top - 8, p_ed->bDC, 0, 0,
    p_ed->rect.right - p_ed->rect.left - 10,
    p_ed->rect.bottom - p_ed->rect.top - 8, TRANSCOLOR);


  r.left = p_ed->x;
  r.top = p_ed->y;
  r.right = ddxGetWidth(hdcED.hdcEdit);
  r.bottom = ddxGetHight(hdcED.hdcEdit);

  _2d_Add_RectItem(&rline, r, 0);

  return xt;
}

// It's actually ascii character
WCHAR co_ToUnicode(int wScanCode)
{
  WCHAR res;
  char c = wScanCode;
  mbrtowc(&res, &c, 1, NULL);
  key_pressed = 0;
  return res;
}

int co_Handle_Edit(CONTROL_EDIT * p_ed, int x, int y, int hdc, int xcor, int ycor)
{
  DWORD t = timeGetTime();

  if (bBlockList)
    return 0;

  if (dim.t1) {
    if (co_Rect_Hit(p_ed->rect, x, y)) {
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
      goto CO_HANDLE_DRAW;
    }
  }

  if (p_ed->bActive) {
    spracuj_spravy(0);

    if (key[0] && key_pressed) {
      WCHAR wt[3];
      int   size;

      if (co_Handle_Edit_Key_Filter() || (co_Handle_wsclen(p_ed) && !key[K_BKSP])) {
        memset(key, 0, POCET_KLAVES * sizeof(char));
        return 1;
      }

      wt[0] = wt[1] = wt[2] = 0;
      size = MultiByteToWideChar(CP_ACP, 0, (char *) &key_pressed, 1, wt, sizeof(wt)/sizeof(wt[0]));
      if(size > 1) {
        // we have got some unsupported character - ignore it
        memset(key, 0, POCET_KLAVES * sizeof(char));
        return 1;
      }
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

        goto CO_HANDLE_DRAW;
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

    CO_HANDLE_DRAW:

      p_ed->tx = draw_edit(p_ed, x, y, hdc, xcor, ycor);

      memset(key, 0, POCET_KLAVES * sizeof(char));
    }
  }

  if (t - p_ed->pTime > 500 && p_ed->bActive) {
    p_ed->pTime = t;

    if (!p_ed->bcActive) {
      char t[32];
      WCHAR wt[32];

      p_ed->bcActive = 1;
      strcpy(t, "_");
      MultiByteToWideChar(CP_ACP, 0, t, strlen(t) + 1, wt,
        sizeof(wt) / sizeof(wt[0]));

      if (!wcslen(p_ed->wtext))
        wcscat(p_ed->wtext, wt);
      else if (p_ed->wtext[wcslen(p_ed->wtext) - 1] != 95)
        wcscat(p_ed->wtext, wt);

      p_ed->tx = draw_edit(p_ed, x, y, hdc, xcor, ycor);
    }
    else {
      p_ed->bcActive = 0;
      p_ed->wtext[wcslen(p_ed->wtext) - 1] = 0;
      p_ed->tx = draw_edit(p_ed, x, y, hdc, xcor, ycor);
    }

  }

  return 1;
}

char co_Check_Open_Combo(CONTROL_LIST_ITEM * p_list, int lsize)
{
  int i;

  for (i = 0; i < lsize; i++)
    if ((p_list + i)->p_combo && (p_list + i)->bActive)
      if ((p_list + i)->p_combo->bList)
        return 1;

  return 0;
}

void co_Handle_Controls(CONTROL_LIST_ITEM * p_list, int lsize, int x, int y,
  int hdc, int xcor, int ycor)
{
  char c = co_Check_Open_Combo(p_list, lsize);
  int i;

  for (i = 0; i < lsize; i++) {
    if ((p_list + i)->p_combo && (p_list + i)->bActive
      && !(p_list + i)->bDisabled) {
      if (!co_Handle_Combo((p_list + i)->p_combo, (p_list + i)->bFocus, x, y,
          hdc, xcor, ycor, c) && (p_list + i)->bFocus)
        (p_list + i)->bFocus = 0;
      else
        (p_list + i)->bFocus = 1;
    }

    if (dim.t1)
      if ((p_list + i)->p_combod && (p_list + i)->bActive
        && !(p_list + i)->bDisabled) {
        if (!co_Handle_Combo_Drop((p_list + i)->p_combod,
            (p_list + i)->bFocus, x, y, hdc, xcor, ycor))
          (p_list + i)->bFocus = 0;
        else
          (p_list + i)->bFocus = 1;
      }

    if (dim.t1)
      if ((p_list + i)->p_button && (p_list + i)->bActive
        && !(p_list + i)->bDisabled) {
        int xx = x, yy = y;

        if ((p_list + i)->iTab == -1) {
          xx += TAB_X;
          yy += TAB_Y;
        }

        if (!co_Handle_Button((p_list + i)->p_button, xx, yy))
          (p_list + i)->bFocus = 0;
        else {
          (p_list + i)->bFocus = 1;
        }
      }

    if (dim.t1)
      if ((p_list + i)->p_check && (p_list + i)->bActive
        && !(p_list + i)->bDisabled) {
        if (!co_Handle_Checkbox((p_list + i)->p_check, x, y))
          (p_list + i)->bFocus = 0;
        else {
          (p_list + i)->bFocus = 1;
        }
      }

    if ((p_list + i)->p_prog && (p_list + i)->bActive
      && !(p_list + i)->bDisabled) {
      if (!co_Handle_Progres((p_list + i)->p_prog, x, y))
        (p_list + i)->bFocus = 0;
      else {
        (p_list + i)->bFocus = 1;
      }
    }

    if ((p_list + i)->p_list && (p_list + i)->bActive
      && !(p_list + i)->bDisabled) {
      if (!co_Handle_List((p_list + i)->p_list, x, y, hdc, xcor, ycor))
        (p_list + i)->bFocus = 0;
      else {
        (p_list + i)->bFocus = 1;
      }
    }

    if ((p_list + i)->p_edit && (p_list + i)->bActive
      && !(p_list + i)->bDisabled) {
      if (!co_Handle_Edit((p_list + i)->p_edit, x, y, hdc, xcor, ycor))
        (p_list + i)->bFocus = 0;
      else {
        (p_list + i)->bFocus = 1;
      }
    }
  }
}

void co_Handle_Release(CONTROL_LIST_ITEM * p_list, int lsize)
{
  int i;

  for (i = 0; i < lsize; i++) {
    if ((p_list + i)->p_combo)
      co_Release_Combo((p_list + i)->p_combo);

    if ((p_list + i)->p_button)
      co_Release_Button((p_list + i)->p_button);

    if ((p_list + i)->p_check)
      co_Release_CheckBox((p_list + i)->p_check);

    if ((p_list + i)->p_prog)
      co_Release_Progres((p_list + i)->p_prog);

    if ((p_list + i)->p_list)
      co_Release_List((p_list + i)->p_list);

    if ((p_list + i)->p_edit)
      co_Release_Edit((p_list + i)->p_edit);

    if ((p_list + i)->p_combod)
      co_Release_Combo_Drop((p_list + i)->p_combod);

  }
}
