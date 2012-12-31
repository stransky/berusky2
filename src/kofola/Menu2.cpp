//------------------------------------------------------------------------------------------------
// 0.0.1
//------------------------------------------------------------------------------------------------
#include <stdio.h>
#include "3d_all.h"
#include "Berusky3d_kofola_interface.h"
#include "Berusky3d_kofola2d.h"
#include "game_logic.h"
#include "menu_script.h"
#include "Command.h"
#include "3D_graphic.h"
#include "Menu.h"
#include "Menu2.h"
#include "Comics.h"
#include "credits.h"
#include "font3d.h"
#include "controls3d.h"
#include "menu_def.h"
#include "Setup.h"
#include "profiles.h"
#include "Apak.h"
#include "Object.h"
#include "animationk.h"
#include "Demo.h"
#include "load_level.h"
#include "ambient_sounds.h"
#include "Tools.h"

#define RES_NUM 60

#define HDC2DD		 -1

extern SETUP setup;
extern _3D_CURSOR _3dCur;
extern APAK_HANDLE *pBmpArchive;
extern PLAYER_PROFILE pPlayerProfile;
extern APAK_HANDLE *p3DMArchive;
extern int iActualLevel;
extern int iActualScene;
extern B2_FONT b2_3d_font;
extern char cFontFile[5][64];
extern AUDIO_DATA ad;
extern int iLanguageVersion;

typedef struct
{
  CMD_LINE *cmd;
  int iWave;
  char bStop;
} ANIMATION;

extern _2D_DATA _2dd;
extern HW_KONFIG hwconf;

static ANIMATION anm[32];
static DWORD timercnt = 0;
static DWORD dwLTime;
static int CompositDC;
static int FontDC;
static int BackDC;

static int iCompositDC;
static int iFontDC;
//static int iBackDC;

static char bBackDC = 0;

static DeviceHandle iLoadScreenDevice = 0;
static int iLoadScreenBitmap = K_CHYBA;
static RECT iLoadScreenRect = { 0, 0, 1024, 768 };

RECT_LINE rline;

//extern int gi_EnumDisplaySettings(DEVMODE *pdevmode);
int check_Save_Owner(char *cDir, WCHAR * wFileName);
int LoadHelp(char *c_file);

CMD_LINE *GetCMD2(CMD_LINE * cmp, CMD_LINE * pres)
{
  int i;

  for (i = 0; i < RES_NUM; i++)
    if (!strcmp(cmp->cParam[1], pres[i].cParam[0]))
      return &pres[i];

  return NULL;
}

//------------------------------------------------------------------------------------------------
// zastavi vsechny animace
//------------------------------------------------------------------------------------------------
void StopAll2(void)
{
  int i;

  for (i = 0; i < 32; i++)
    if (anm[i].cmd)
      anm[i].bStop = 1;
}

//------------------------------------------------------------------------------------------------
// zastavi animaci
//------------------------------------------------------------------------------------------------
void Stop2(CMD_LINE * cmd)
{
  int i;

  for (i = 0; i < 32; i++)
    if (cmd == anm[i].cmd) {
      if (cmd->bEndActivate[0] && cmd->pCmdLine)
        cmd->pCmdLine[(int)cmd->bEndActivate[0]].bActive = 1;

      if (cmd->bEndActivate[1] && cmd->pCmdLine)
        cmd->pCmdLine[(int)cmd->bEndActivate[1]].bActive = 1;

      cmd->bActive = 0;

      anm[i].bStop = 1;
      return;
    }
}

int NextStep2(CMD_LINE * cmd)
{
  if ((rand() % 100) < 60)
    return cmd->iAnim[cmd->iLastfrm][6];
  else
    return cmd->iAnim[cmd->iLastfrm][6 +
      rand() % (cmd->iAnim[cmd->iLastfrm][12])];
}

void DrawFrame2(CMD_LINE * cmd)
{
  if (!cmd->iLayer) {
    if (cmd->iAnim[cmd->iLastfrm][6] >= 0) {
      RECT r;

      ddx2BitBlt(CompositDC,
        cmd->iAnim[cmd->iLastfrm][2] + cmd->iAnim[cmd->iLastfrm][6],
        cmd->iAnim[cmd->iLastfrm][3] + cmd->iAnim[cmd->iLastfrm][7],
        cmd->iAnim[cmd->iLastfrm][8], cmd->iAnim[cmd->iLastfrm][9],
        cmd->iAnim[cmd->iLastfrm][1],
        cmd->iAnim[cmd->iLastfrm][6], cmd->iAnim[cmd->iLastfrm][7]);

      r.left = cmd->iAnim[cmd->iLastfrm][2] + cmd->iAnim[cmd->iLastfrm][6];
      r.top = cmd->iAnim[cmd->iLastfrm][3] + cmd->iAnim[cmd->iLastfrm][7];
      r.right = cmd->iAnim[cmd->iLastfrm][8];
      r.bottom = cmd->iAnim[cmd->iLastfrm][9];

      ddx2AddRectItem(&rline, r, 1);
    }
    else
      ddx2DrawSurface(CompositDC, cmd->iAnim[cmd->iLastfrm], 1);
  }
  else
    ddx2DrawSurface(FontDC, cmd->iAnim[cmd->iLastfrm], 3);
}

//------------------------------------------------------------------------------------------------
// provede animaci
//------------------------------------------------------------------------------------------------
void Animation2(CMD_LINE * cmd, int time)
{
ANIMATION_DRAW:

  while (time > 0) {
    if (cmd->iAnim[cmd->iLastfrm][5] == -1) {
      Stop2(cmd);
      break;
    }

    time -= cmd->iAnim[cmd->iLastfrm][4];
    if (time > (cmd->iAnim[cmd->iAnim[cmd->iLastfrm][5]][4] / 2.0f)) {
/*			if(cmd->iAnim[cmd->iLastfrm][6] >= 0)
				cmd->iAnim[cmd->iLastfrm][5] = NextStep(cmd);*/

      if (cmd->iAnim[cmd->iLastfrm][10] > 0)
        DrawFrame2(cmd);

      cmd->iLastfrm = cmd->iAnim[cmd->iLastfrm][5];
    }
  }

  //----------------------------------------------------------------------
  DrawFrame2(cmd);

  if (!cmd->iAnim[cmd->iLastfrm][4]) {
/*		if(cmd->iAnim[cmd->iLastfrm][6] >= 0)
			cmd->iAnim[cmd->iLastfrm][5] = NextStep(cmd);*/

    cmd->iLastfrm = cmd->iAnim[cmd->iLastfrm][5];
    goto ANIMATION_DRAW;
  }

  if (cmd->iAnim[cmd->iLastfrm][5] == -1) {
    Stop2(cmd);
    return;
  }

/*	if(cmd->iAnim[cmd->iLastfrm][6] >= 0)
		cmd->iAnim[cmd->iLastfrm][5] = NextStep(cmd);*/

  cmd->iLastfrm = cmd->iAnim[cmd->iLastfrm][5];
}

int RectCross2(RECT_LINE * p_rl, int idx)
{
  int c = 0;
  int i;
  RECT *dr = &p_rl->rect[idx].rect;

  for (i = 0; i < rline.rlast; i++)
    if (i != idx)
      if (co2_Rect_Hit(p_rl->rect[i].rect, dr->left, dr->top) ||
        co2_Rect_Hit(p_rl->rect[i].rect, dr->right, dr->top) ||
        co2_Rect_Hit(p_rl->rect[i].rect, dr->left, dr->bottom) ||
        co2_Rect_Hit(p_rl->rect[i].rect, dr->right, dr->bottom))
        c++;

  return 0;
}

//------------------------------------------------------------------------------------------------
// provede animace
//------------------------------------------------------------------------------------------------
void AnimationEvent2(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
  DRAW_RECT *dr;
  int i;
  char bAnim = 0;
  DWORD e;

  //priprava 
  //BitBltU(CompositDC,0,0,1024,768,NULL,0,0,WHITENESS);

  e = abs(dwLTime - dwTime);
  for (i = 0; i < 32; i++)
    if (anm[i].cmd) {
      if (anm[i].bStop) {
        anm[i].cmd->iLastfrm = 1;
        anm[i].cmd->iCounter = 0;

				if(anm[i].iWave != -1)
					adas_Release_Source(PARTICULAR_SOUND_SOURCE, UNDEFINED_VALUE, anm[i].iWave);

        anm[i].iWave = -1;
        anm[i].cmd = NULL;
      }
      else {
        anm[i].cmd->iCounter += e;
        if (anm[i].cmd->iCounter >=
          anm[i].cmd->iAnim[anm[i].cmd->iLastfrm][4]) {
          Animation2(anm[i].cmd, anm[i].cmd->iCounter);
          anm[i].cmd->iCounter = 0;
          bAnim = 1;
        }
      }
    }

  //prenos kreslenych dat;
  if (bAnim) {
    if (bBackDC) {
      for (i = 0; i < rline.rlast; i++) {
        dr = &rline.rect[i];

        ddx2TransparentBlt(BackDC, dr->rect.left, dr->rect.top,
          dr->rect.right, dr->rect.bottom, FontDC, dr->rect.left,
          dr->rect.top, dr->rect.right, dr->rect.bottom, TRANSCOLOR);
      }

      for (i = 0; i < rline.rlast; i++) {
        dr = &rline.rect[i];

        ddx2TransparentBlt(CompositDC, dr->rect.left, dr->rect.top,
          dr->rect.right, dr->rect.bottom, BackDC, dr->rect.left,
          dr->rect.top, dr->rect.right, dr->rect.bottom, TRANSCOLOR);

      }
    }
    else
      for (i = 0; i < rline.rlast; i++) {
        dr = &rline.rect[i];

        ddx2TransparentBlt(CompositDC, dr->rect.left, dr->rect.top,
          dr->rect.right, dr->rect.bottom, FontDC, dr->rect.left,
          dr->rect.top, dr->rect.right, dr->rect.bottom, TRANSCOLOR);
      }

    for (i = 0; i < rline.rlast; i++) {
      dr = &rline.rect[i];

//                              ddx2UpdateMouse();

      //if(dr->iLayer >= 1 || RectCross(&rline, i))
      ddx2TransparentBltDisplay(dr->rect.left, dr->rect.top, dr->rect.right,
        dr->rect.bottom, CompositDC, dr->rect.left, dr->rect.top,
        dr->rect.right, dr->rect.bottom, TRANSCOLOR);
    }
  }

//      if(!bAnim)
//              DisplayFrame();

  dwLTime = dwTime;
  timercnt += e;

  //_2d_Clear_RectLine(&rline);
}

void InitRandomJumps2(CMD_LINE * cmd)
{
  int i;

  for (i = 0; i < 200; i++)
    if (cmd->iAnim[i][6] >= 0)
      cmd->iAnim[i][5] = cmd->iAnim[i][6 + rand() % (cmd->iAnim[i][12])];
}

int FindAnimation2(CMD_LINE * cmd)
{
  int i;

  for (i = 0; i < 32; i++)
    if (anm[i].cmd == cmd)
      return 1;

  return 0;
}

//------------------------------------------------------------------------------------------------
// prida animaci
//------------------------------------------------------------------------------------------------
int AddAnimation2(CMD_LINE * cmd, AUDIO_DATA * p_ad, char bOnlyOnes)
{
  int i, r;
  float pos[3] = { 0.0f, 0.0f, 1.0f };

  for (i = 0; i < 32; i++)
    if (!anm[i].cmd) {
      if (bOnlyOnes && FindAnimation2(cmd))
        return -1;

//                      InitRandomJumps(cmd);

      anm[i].cmd = cmd;
      anm[i].bStop = 0;
      anm[i].iWave = -1;
      Animation2(anm[i].cmd, 0);

      if (cmd->iParam[0] == COM_RANDOMANIMATION) {
        r = rand() % 3;

        if (cmd->iParam[r + 2] < 0) {
          if (cmd->iParam[2] >= 0) {
            adas_Get_Listener_Position(pos);
            ap_Play_Sound(0,1,0,pos,cmd->iParam[2], NULL, &ad);
          }
        }
        else {
          adas_Get_Listener_Position(pos);
          ap_Play_Sound(0,1,0,pos,cmd->iParam[r+2], NULL, &ad);
        }
      }

      return i;
    }
    else if (!strcmp(cmd->cParam[1], anm[i].cmd->cParam[0]) && !anm[i].bStop)
      return -1;

  return -1;
}

int ChooseBidedExitAnimation2(CMD_LINE * cmd, int Index, AUDIO_DATA * p_ad)
{
  int id[32];
  int i, c = 0;

  for (i = Index; i < 32; i++)
    if (cmd[i].iParam[0] == COM_BINDEXITANIMATION) {
      id[c] = i;
      c++;

      if (c > 31)
        break;
    }
    else if (cmd[i].iParam[0] != COM_BINDANIMATION)
      break;

  if (c)
    return id[rand() % c];
  else
    return -1;
}

int ChooseBidedAnimation2(CMD_LINE * cmd, int Index, AUDIO_DATA * p_ad)
{
  int id[32];
  int i, c = 0;

  for (i = Index; i < 32; i++)
    if (cmd[i].iParam[0] == COM_BINDANIMATION) {
      id[c] = i;
      c++;

      if (c > 31)
        break;
    }
    else if (cmd[i].iParam[0] != COM_BINDEXITANIMATION)
      break;

  if (c)
    return id[rand() % c];
  else
    return -1;
}

void FreeAnimations2(CMD_LINE * cmd, int csize)
{
  int i, j;

  for (i = 0; i < csize; i++)
    for (j = 0; j < 32; j++)
      if (anm[j].cmd)
        if (anm[j].cmd == &cmd[i]) {
          anm[j].cmd->iLastfrm = 1;
          anm[j].cmd->iCounter = 0;
          anm[j].iWave = -1;
          anm[j].cmd = NULL;
        }

}

//------------------------------------------------------------------------------------------------
// prida animaci
//------------------------------------------------------------------------------------------------
void CheckAnimation2(CMD_LINE * cmd, AUDIO_DATA * p_ad)
{
  int i;
//  float pos[3] = { 0.0f, 0.0f, 1.0f };

  for (i = 0; i < 32; i++)
    if (anm[i].cmd)
      if (!strcmp(cmd->cParam[1], anm[i].cmd->cParam[0])) {
        Stop2(anm[i].cmd);

        if (!cmd->iLayer) {
          //menucommand_Draw(_2dd.hDC, anm[i].cmd->iAnim[0] , 0);
          ddx2DrawDisplay(anm[i].cmd->iAnim[0], 0);
        }
        else {
          //menucommand_DrawT(_2dd.hDC, anm[i].cmd->iAnim[0]);
          ddx2DrawDisplayColorKey(anm[i].cmd->iAnim[0], 0, TRANSCOLOR);
          //menucommand_Draw(FontDC, anm[i].cmd->iAnim[0], 3);
          ddx2DrawSurface(FontDC, anm[i].cmd->iAnim[0], 3);
        }
        return;
      }
}

//------------------------------------------------------------------------------------------------
// spusti nahodny zvuk podle zadani
//------------------------------------------------------------------------------------------------
int mPlaySound2(CMD_LINE * cmd, AUDIO_DATA * p_ad, int type)
{
  float pos[3] = { 0.0f, 0.0f, 1.0f };
  int r = rand() % 4, ret = 0;

  if (cmd->iParam[r + 1] < 0) {
    if (cmd->iParam[1] >= 0) {
       adas_Get_Listener_Position(pos);
       ret = ap_Play_Sound(type,1,0,pos,cmd->iParam[1], NULL, &ad);
    }
  }
  else {
    adas_Get_Listener_Position(pos);
    ret = ap_Play_Sound(type,1,0,pos,cmd->iParam[r+1], NULL, &ad);
  }

  return ret;
}

int CreateFontAnimations2(CMD_LINE * res, int *lastcmd, int ycor, char bcor)
{
  int lcmd = *lastcmd;

  char text[256];
  RECT r = { 0, 0, 0, 0 };
  int i;
  int sidx1, sidx2;
  int y;

  for (i = 0; i < lcmd; i++)
    if (res[i].iParam[0] == COM_SETRECT) {
      r.left = res[i].iParam[1];
      r.top = res[i].iParam[2];
      r.right = res[i].iParam[3];
      r.bottom = res[i].iParam[4];

      if (bcor) {
        r.top += ycor;
        r.bottom += ycor;
      }
    }

  y = r.top;

  for (i = 0; i < lcmd; i++)
    if (res[i].iParam[0] == COM_CREATEBUTTON)
      if (fn2_Gen_Menu_Text(0, HDC2DD, res[i].cParam[0], &sidx1,
          &sidx2) != -1)
        if (sidx1 != -1 && sidx2 != -1) {
          int ii;
          int oy = res[i].iParam[2];

          //int x = r.left + ftoi((r.right - r.left - _2dd.bitmap[sidx1].bitmap.bmWidth) / (float)2.0f);
          int x =
            r.left + ftoi((r.right - r.left -
              ddx2GetWidth(sidx1)) / (float) 2.0f);

          if (bcor)
            oy += ycor;

          if (res[i].iParam[1] != -1) {
            oy = res[i].iParam[2];

            if (bcor)
              oy += ycor;

            y = oy;
            x = res[i].iParam[1];
          }

          y += ddx2GetHeight(sidx1);

          //OnAbove(16,661,100,748, quit_game.txt, NO_EXEPTION)
          sprintf(text, "OnAbove(%d,%d,%d,%d, NO_EXEPTION, NO_EXEPTION)", x,
            oy, x + ddx2GetWidth(sidx1), y);
          Parse_LineT(text, res[*lastcmd].iParam, 6, res[*lastcmd].cParam[0],
            res[*lastcmd].cParam[1]);
          res[*lastcmd].uiTimerID = 0;
          res[*lastcmd].iLastfrm = 1;
          res[*lastcmd].iCounter = 0;

          /*
             (0, 123, 0, 640, 0, 1)
             (1, 124, 0, 640, 25, 2)
             (2, 124, 0, 640, 25, -1)
           */
          res[*lastcmd].iAnim[0][0] = 0;
          res[*lastcmd].iAnim[0][1] = sidx1;
          res[*lastcmd].iAnim[0][2] = x;
          res[*lastcmd].iAnim[0][3] = oy;
          res[*lastcmd].iAnim[0][4] = 0;
          res[*lastcmd].iAnim[0][5] = 1;

          for (ii = 6; ii < 13; ii++)
            res[*lastcmd].iAnim[0][ii] = -1;

          x =
            r.left + ftoi((r.right - r.left -
              ddx2GetWidth(sidx2)) / (float) 2.0f);

          if (res[i].iParam[1] != -1)
            x = res[i].iParam[1];

          res[*lastcmd].iAnim[1][0] = 1;
          res[*lastcmd].iAnim[1][1] = sidx2;
          res[*lastcmd].iAnim[1][2] = x;
          res[*lastcmd].iAnim[1][3] = oy;
          res[*lastcmd].iAnim[1][4] = 50;
          res[*lastcmd].iAnim[1][5] = 2;

          for (ii = 6; ii < 13; ii++)
            res[*lastcmd].iAnim[1][ii] = -1;

          res[*lastcmd].iAnim[2][0] = 2;
          res[*lastcmd].iAnim[2][1] = sidx2;
          res[*lastcmd].iAnim[2][2] = x;
          res[*lastcmd].iAnim[2][3] = oy;
          res[*lastcmd].iAnim[2][4] = 50;
          res[*lastcmd].iAnim[2][5] = -1;

          for (ii = 6; ii < 13; ii++)
            res[*lastcmd].iAnim[2][ii] = -1;

          res[*lastcmd].iLayer = 1;
          (*lastcmd)++;

          //Draw(1,0,0)
          sprintf(text, "Draw(%d,%d,%d)", sidx1, x, oy);
          Parse_LineT(text, res[*lastcmd].iParam, 6, res[*lastcmd].cParam[0],
            res[*lastcmd].cParam[1]);
          res[*lastcmd].iLayer = 1;
          (*lastcmd)++;

          //OnClick(16,661,100,748, quit_gamec.txt, EXIT)
          sprintf(text, "OnClick(%d,%d,%d,%d, NO_EXEPTION, %s)", x, oy,
            x + ddx2GetWidth(sidx1), y, res[i].cParam[1]);

          Parse_LineT(text, res[*lastcmd].iParam, 6, res[*lastcmd].cParam[0],
            res[*lastcmd].cParam[1]);
          res[*lastcmd].iLayer = 1;
          (*lastcmd)++;

          //BindSound(54,55,56,-1)
          strcpy(text, "BindSound(54,55,56,-1)");
          Parse_LineT(text, res[*lastcmd].iParam, 6, res[*lastcmd].cParam[0],
            res[*lastcmd].cParam[1]);
          res[*lastcmd].iLayer = 0;
          (*lastcmd)++;
        }

  return y;
}

void SetTab2(int iTab, int iLTab, CONTROL_LIST_ITEM2 * p_list, int lsize,
  int *hdcTab)
{
  int i;

  if (iLTab > -1) {
    //BitBlt(hdcTab[iLTab], 0, 0, TAB_XRES, TAB_YRES, _2dd.hDC, TAB_X, TAB_Y, SRCCOPY);
    ddx2BitBlt(hdcTab[iLTab], 0, 0, TAB_XRES, TAB_YRES, HDC2DD, TAB_X, TAB_Y);
  }

  for (i = 0; i < lsize; i++)
    if ((p_list + i)->iTab == iTab && !(p_list + i)->bDisabled)
      (p_list + i)->bActive = 1;
    else if ((p_list + i)->iTab != -1)
      (p_list + i)->bActive = 0;

  //BitBlt(_2dd.hDC, TAB_X, TAB_Y, TAB_XRES, TAB_YRES, hdcTab[iTab], 0, 0, SRCCOPY);
  ddx2BitBltDisplay(TAB_X, TAB_Y, TAB_XRES, TAB_YRES, hdcTab[iTab], 0, 0);
//      DisplayFrame();
}

void CharMenuCheckMultyKyes2(LIST_VIEW_CONTROL2 * p_li, int iKey)
{
  int y;
  int i;
  RECT r;

  for (i = 0; i < p_li->listnum; i++)
    if (i != p_li->cSelected && p_li->piValue[i] == iKey) {
      y = i * 30;

      r.left = 550;
      r.top = y + 2;
      r.right = r.left + (ddx2GetWidth(p_li->bDCn) - 550);
      r.bottom = r.top + 28;

      ddx2FillRect(p_li->bDCn, &r, 0);

      ddx2FillRect(p_li->bDCs, &r, 0);

      char tmp[200];
      co2_List_Add_String(p_li, i, 550, Key2String(255, tmp), 255, 0);
    }
}

void SetCharMenu2(LIST_VIEW_CONTROL2 * p_li, LEVELINFO * p_Level,
  ANIMATION_MODULE * p_am, int mix, int miy)
{
  char text[128];
  int y = p_li->cSelected * 30;
//  int xs = p_li->rectList.top + TAB_Y + y - p_li->dx;
  int xp = 0;
  int xt = 0;
  int i;
  RECT r;
  RECT rtmp;

  int iCursor = 0;

  mi.x = mix;
  mi.y = miy;

  if (!p_li->cSelected || p_li->cSelected == 11 || p_li->cSelected == 19)
    return;

  r.left = 550;
  r.top = y + 2;
  r.right = r.left + (ddx2GetWidth(p_li->bDCn) - 550);
  r.bottom = r.top + 28;

  ddx2FillRect(p_li->bDCn, &r, 0);
  ddx2FillRect(p_li->bDCs, &r, 0);

  if (y - p_li->dx < 0)
    xp = (y - p_li->dx) * -1;

  if ((p_li->rectList.top + TAB_Y + y - p_li->dx + 30) >
    p_li->rectList.bottom + TAB_Y)
    xt =
      (p_li->rectList.top + TAB_Y + y - p_li->dx + 30) -
      (p_li->rectList.bottom + TAB_Y);

  ddx2BitBlt(HDC2DD, p_li->rectList.left + TAB_X,
    p_li->rectList.top + TAB_Y + y - p_li->dx + xp, ddx2GetWidth(p_li->bDCs),
    30 - xp - xt, p_li->bDCs, 0, y + xp);

  //spracuj_spravy(0);
  am_Flip(p_Level, p_am, 1, &rline, CLIST_ITEMC, 0, 0, 0);

  r.top = p_li->rectList.top + TAB_Y + y - p_li->dx + xp - 2;
  r.left = 665;
  r.right = r.left + 220;
  r.bottom = r.top + 27;

  rtmp.left = r.left;
  rtmp.top = r.top;
  rtmp.right = 220;
  rtmp.bottom = 27;

  while (!key[0]) {
    if (iCursor < 100)
      ddx2FillRect(HDC2DD, &r, RGB(63, 122, 163));
    else if (iCursor > 99 && iCursor < 200)
      ddx2FillRect(HDC2DD, &r, 0);
    else {
      iCursor = 0;
      ddx2FillRect(HDC2DD, &r, 0);
    }

    ddx2AddRectItem(&rline, rtmp, 0);

    iCursor += 4;

    Sleep(10);
    am_Flip(p_Level, p_am, 1, &rline, CLIST_ITEMC, 0, 0, 0);

    if (key[K_F1] || key[K_F2] || key[K_F3] || key[K_F10] || key[K_PLUS]
      || key[K_MINUS]) {
      key[0] = 0;
      key[K_F1] = 0;
      key[K_F2] = 0;
      key[K_F3] = 0;
      key[K_F10] = 0;
      key[K_PLUS] = 0;
      key[K_MINUS] = 0;
    }
  }

  key[0] = 0;

  for (i = 0; i < POCET_KLAVES; i++)
    if (key[i]) {
      Key2String(i, text);      
      key[i] = 0;
      break;
    }

  co2_List_Add_String(p_li, p_li->cSelected, 550, text, i, 0);
  CharMenuCheckMultyKyes2(p_li, i);

  ddx2BitBlt(HDC2DD, p_li->rectList.left + TAB_X, p_li->rectList.top + TAB_Y,
    ddx2GetWidth(p_li->bDCn), p_li->rectList.bottom - p_li->rectList.top,
    p_li->bDCn, 0, p_li->dx);

  ddx2BitBlt(HDC2DD, p_li->rectList.left + TAB_X,
    p_li->rectList.top + TAB_Y + y - p_li->dx + xp, ddx2GetWidth(p_li->bDCs),
    30 - xp - xt, p_li->bDCs, 0, y + xp);

}

void SetMenuSettings2(CONTROL_LIST_ITEM2 * citem, int *hdcTabUse)
{
  int i;

  if (hdcTabUse[0]) {
    //setup.pohled_berusky = co2_Check_Get_State(citem, CLIST_ITEMC, 0);
    setup.posouvat_kameru = co2_Check_Get_State(citem, CLIST_ITEMC, 1);
    setup.ovladani = co2_Check_Get_State(citem, CLIST_ITEMC, 2);
    //setup.menu_vram_load = co_Check_Get_State(citem, CLIST_ITEMC, 3);
    setup.start_zpruhlednovani = co2_Check_Get_State(citem, CLIST_ITEMC, 10);
    setup.start_zvyraznovani = co2_Check_Get_State(citem, CLIST_ITEMC, 11);
    setup.ovladani_rohy = co2_Check_Get_State(citem, CLIST_ITEMC, 12);
    setup.camera_intro = co2_Check_Get_State(citem, CLIST_ITEMC, 16);

    i = co2_Check_Get_State(citem, CLIST_ITEMC, 13);

    if (i)
      setup.ovladani_rohy_default = 0;
    else
      setup.ovladani_rohy_default = 1;

    setup.ovladani_rohy_smer = co2_Check_Get_State(citem, CLIST_ITEMC, 15);
    //setup.ovladani_pr_posun = co2_Check_Get_State(citem, CLIST_ITEMC, 16);

    setup.ovladani_rohy_rychlost =
      co2_Progres_Get(citem, CLIST_ITEMC, 0) / 10.0f;
    setup.p_kamera_radius = co2_Progres_Get(citem, CLIST_ITEMC, 1) / 2.0f;
  }

  if (hdcTabUse[1]) {

    i = co2_Combo_Get_Sel(citem, CLIST_ITEMC, 6);

    if (i == 3)
      setup.light_dyn = 0;
    else {
      setup.light_dyn = 1;
      setup.light_rychlost = i + 1;
    }

    //setup.light_rychlost = co2_Combo_Get_Sel(citem, CLIST_ITEMC, 6) + 1;

    co2_Combo_Drop_Get_Sel(citem, CLIST_ITEMC, 10, &setup.text_ostrost);


    setup.ditering = co2_Check_Get_State(citem, CLIST_ITEMC, 7);
    setup.animace_okoli = co2_Check_Get_State(citem, CLIST_ITEMC, 8);
  }

  if (hdcTabUse[2]) {
    setup.soundvolume = co2_Progres_Get(citem, CLIST_ITEMC, 4);
    setup.ambientvolume = co2_Progres_Get(citem, CLIST_ITEMC, 5);
    setup.musicvolume = co2_Progres_Get(citem, CLIST_ITEMC, 6);
  }

  if (hdcTabUse[3]) {
    setup.key[1] = co2_List_Get_Value(citem, CLIST_ITEMC, 0, 1);
    setup.key[2] = co2_List_Get_Value(citem, CLIST_ITEMC, 0, 2);
    setup.key[3] = co2_List_Get_Value(citem, CLIST_ITEMC, 0, 3);
    setup.key[4] = co2_List_Get_Value(citem, CLIST_ITEMC, 0, 4);
    setup.key[5] = co2_List_Get_Value(citem, CLIST_ITEMC, 0, 5);
    setup.key[6] = co2_List_Get_Value(citem, CLIST_ITEMC, 0, 6);
    setup.key[7] = co2_List_Get_Value(citem, CLIST_ITEMC, 0, 7);
    setup.key[8] = co2_List_Get_Value(citem, CLIST_ITEMC, 0, 8);
    setup.key[9] = co2_List_Get_Value(citem, CLIST_ITEMC, 0, 9);
    setup.key[10] = co2_List_Get_Value(citem, CLIST_ITEMC, 0, 10);

    setup.key[12] = co2_List_Get_Value(citem, CLIST_ITEMC, 0, 12);
    setup.key[13] = co2_List_Get_Value(citem, CLIST_ITEMC, 0, 13);
    setup.key[14] = co2_List_Get_Value(citem, CLIST_ITEMC, 0, 14);
    setup.key[15] = co2_List_Get_Value(citem, CLIST_ITEMC, 0, 15);
    setup.key[16] = co2_List_Get_Value(citem, CLIST_ITEMC, 0, 16);
    setup.key[17] = co2_List_Get_Value(citem, CLIST_ITEMC, 0, 17);
    setup.key[18] = co2_List_Get_Value(citem, CLIST_ITEMC, 0, 18);

    setup.key[20] = co2_List_Get_Value(citem, CLIST_ITEMC, 0, 20);
    setup.key[21] = co2_List_Get_Value(citem, CLIST_ITEMC, 0, 21);
    setup.key[22] = co2_List_Get_Value(citem, CLIST_ITEMC, 0, 22);
    setup.key[23] = co2_List_Get_Value(citem, CLIST_ITEMC, 0, 23);
    setup.key[24] = co2_List_Get_Value(citem, CLIST_ITEMC, 0, 24);
    setup.key[25] = co2_List_Get_Value(citem, CLIST_ITEMC, 0, 25);
    setup.key[26] = co2_List_Get_Value(citem, CLIST_ITEMC, 0, 26);
    setup.key[27] = co2_List_Get_Value(citem, CLIST_ITEMC, 0, 27);
    setup.key[28] = co2_List_Get_Value(citem, CLIST_ITEMC, 0, 28);
    setup.key[29] = co2_List_Get_Value(citem, CLIST_ITEMC, 0, 29);
    setup.key[30] = co2_List_Get_Value(citem, CLIST_ITEMC, 0, 30);
  }
}

void SetMenuSettingsS2(CONTROL_LIST_ITEM2 * citem, int *hdcTabUse)
{
  if (hdcTabUse[2]) {
    setup.soundvolume = co2_Progres_Get(citem, CLIST_ITEMC, 4);
    setup.ambientvolume = co2_Progres_Get(citem, CLIST_ITEMC, 5);
    setup.musicvolume = co2_Progres_Get(citem, CLIST_ITEMC, 6);
  }
}

void InitTab3d2(CONTROL_LIST_ITEM2 * citem, int *hdcTab)
{
  int i;

  //kprintf(1, "#settings_lights");

  co2_Set_Text_Right(hdcTab[1], "#settings_lights", 0, 285, 57);
  citem[32].p_combo = co2_Create_Combo(hdcTab[1], 300, 50, 100, 6);
  citem[32].iTab = 1;

  co2_Combo_Add_StringWC(citem[32].p_combo, "##settings_gemeral_3d_high");
  co2_Combo_Add_StringWC(citem[32].p_combo, "##settings_gemeral_3d_normal");
  co2_Combo_Add_StringWC(citem[32].p_combo, "##settings_gemeral_3d_low");
  //co2_Combo_Add_StringWC(citem[32].p_combo, "##settings_gemeral_3d_elow");
  co2_Combo_Add_StringWC(citem[32].p_combo, "##settings_lights_off");
  co2_Combo_Set_Params(citem[32].p_combo, 4);

  if (setup.light_dyn) {
    co2_Combo_Set_Sel(hdcTab[1], citem[32].p_combo, setup.light_rychlost - 1);
  }
  else {
    co2_Combo_Set_Sel(hdcTab[1], citem[32].p_combo, 3);
    //co2_Combo_Set_Sel(hdcTab[1], citem[32].p_combo, setup.light_rychlost - 1);
    /*co2_Draw_Disable(hdcTab[1], 0, 0, 0, citem[32].p_combo->y, 500, 30);
       citem[32].bActive = 0;
       citem[32].bDisabled = 1; */
  }


  //kprintf(1, "##settings_sharpness");

  co2_Set_Text_Right(hdcTab[1], "##settings_sharpness", 0, 285, 97);
  citem[42].p_combod = co2_Create_Combo_Drop(hdcTab[1], 300, 90, 10);
  citem[42].iTab = 1;

  co2_Combo_Drop_Add_String(citem[42].p_combod, "-3.0", -3.0f);
  co2_Combo_Drop_Add_String(citem[42].p_combod, "-2.75", -2.75f);
  co2_Combo_Drop_Add_String(citem[42].p_combod, "-2.5", -2.5f);
  co2_Combo_Drop_Add_String(citem[42].p_combod, "-2.25", -2.25f);
  co2_Combo_Drop_Add_String(citem[42].p_combod, "-2.0", -2.0f);
  co2_Combo_Drop_Add_String(citem[42].p_combod, "-1.75", -1.75f);
  co2_Combo_Drop_Add_String(citem[42].p_combod, "-1.5", -1.5f);
  co2_Combo_Drop_Add_String(citem[42].p_combod, "-1.25", -1.25f);
  co2_Combo_Drop_Add_String(citem[42].p_combod, "-1.0", -1.0f);
  co2_Combo_Drop_Add_String(citem[42].p_combod, "-0.75", -0.75f);
  co2_Combo_Drop_Add_String(citem[42].p_combod, "-0.5", -0.5f);
  co2_Combo_Drop_Add_String(citem[42].p_combod, "-0.25", -0.25f);
  co2_Combo_Drop_Add_String(citem[42].p_combod, "0.0", 0.0f);
  co2_Combo_Drop_Add_String(citem[42].p_combod, "+0.25", 0.25f);
  co2_Combo_Drop_Add_String(citem[42].p_combod, "+0.5", 0.5f);
  co2_Combo_Drop_Add_String(citem[42].p_combod, "+0.75", 0.75f);
  co2_Combo_Drop_Add_String(citem[42].p_combod, "+1.0", 1.0f);
  co2_Combo_Drop_Add_String(citem[42].p_combod, "+1.25", 1.25f);
  co2_Combo_Drop_Add_String(citem[42].p_combod, "+1.5", 1.5f);
  co2_Combo_Drop_Add_String(citem[42].p_combod, "+1.75", 1.75f);
  co2_Combo_Drop_Add_String(citem[42].p_combod, "+2.0", 2.0f);
  co2_Combo_Drop_Add_String(citem[42].p_combod, "+2.25", 2.25f);
  co2_Combo_Drop_Add_String(citem[42].p_combod, "+2.5", 2.5f);
  co2_Combo_Drop_Add_String(citem[42].p_combod, "+2.75", 2.75f);
  co2_Combo_Drop_Add_String(citem[42].p_combod, "+3.0", 3.0f);

  i = ftoi(((setup.text_ostrost + 3) * 100) / 25.0f);

  if (i > 24)
    i = 24;

  co2_Combo_Drop_Set_Sel(hdcTab[1], citem[42].p_combod, i);

  //kprintf(1, "##settings_dithering");

  citem[29].p_check =
    co2_Create_CheckBox(hdcTab[1], 300, 150, "##settings_dithering", 0, 7);
  citem[29].iTab = 1;
  co2_Check_Set_State(citem[29].p_check, hdcTab[1], setup.ditering, 1);

  //kprintf(1, "##settings_animations");

  citem[30].p_check =
    co2_Create_CheckBox(hdcTab[1], 300, 180, "##settings_animations", 0, 8);
  citem[30].iTab = 1;
  co2_Check_Set_State(citem[30].p_check, hdcTab[1], setup.animace_okoli, 1);
}

void InitTabSound2(CONTROL_LIST_ITEM2 * citem, int *hdcTab)
{
  co2_Set_Text_Right(hdcTab[2], "##settings_sound", 0, 260, 50);
  citem[21].p_prog = co2_Create_Progres(hdcTab[2], 310, 50, 0, 100, 4);
  citem[21].iTab = 2;
  co2_Progres_Set(citem[21].p_prog, hdcTab[2], setup.soundvolume);

  co2_Set_Text_Right(hdcTab[2], "##settings_ambient", 0, 260, 85);
  citem[22].p_prog = co2_Create_Progres(hdcTab[2], 310, 85, 0, 100, 5);
  citem[22].iTab = 2;
  co2_Progres_Set(citem[22].p_prog, hdcTab[2], setup.ambientvolume);

  co2_Set_Text_Right(hdcTab[2], "##settings_music", 0, 260, 120);
  citem[23].p_prog = co2_Create_Progres(hdcTab[2], 310, 120, 0, 100, 6);
  citem[23].iTab = 2;
  co2_Progres_Set(citem[23].p_prog, hdcTab[2], setup.musicvolume);

  /*co2_Set_Text(hdcTab[2], 260 - TAB_X, 350 - TAB_Y, "##settings_sound", 0);
     citem[21].p_prog = co2_Create_Progres(hdcTab[2], 345 - TAB_X, 352 - TAB_Y, 0, 100, 4);
     citem[21].iTab = 2;
     co2_Progres_Set(citem[21].p_prog, hdcTab[2], setup.soundvolume);

     co2_Set_Text(hdcTab[2], 150 - TAB_X, 385 - TAB_Y, "##settings_ambient", 0);
     citem[22].p_prog = co2_Create_Progres(hdcTab[2], 345 - TAB_X, 387 - TAB_Y, 0, 100, 5);
     citem[22].iTab = 2;
     co2_Progres_Set(citem[22].p_prog, hdcTab[2], setup.ambientvolume);

     co2_Set_Text(hdcTab[2], 242 - TAB_X, 420 - TAB_Y, "##settings_music", 0);
     citem[23].p_prog = co2_Create_Progres(hdcTab[2], 345 - TAB_X, 422 - TAB_Y, 0, 100, 6);
     citem[23].iTab = 2;
     co2_Progres_Set(citem[23].p_prog, hdcTab[2], setup.musicvolume); */
}

void InitTabControls2(CONTROL_LIST_ITEM2 * citem, int *hdcTab)
{
  char ctext[256];

  citem[24].p_list = co2_Create_List(hdcTab[3], 25, 50, 800, 580, 0, 31, 0);
  citem[24].iTab = 3;

  co2_List_Add_String(citem[24].p_list, 0, 2, "##control_beatle", 0, 1);
  co2_List_Add_String(citem[24].p_list, 1, 2, "##control_forward", 0, 0);
  co2_List_Add_String(citem[24].p_list, 2, 2, "##control_back", 0, 0);
  co2_List_Add_String(citem[24].p_list, 3, 2, "##control_left", 0, 0);
  co2_List_Add_String(citem[24].p_list, 4, 2, "##control_right", 0, 0);
  co2_List_Add_String(citem[24].p_list, 5, 2, "##control_nextb", 0, 0);
  co2_List_Add_String(citem[24].p_list, 6, 2, "##control_b1", 0, 0);
  co2_List_Add_String(citem[24].p_list, 7, 2, "##control_b2", 0, 0);
  co2_List_Add_String(citem[24].p_list, 8, 2, "##control_b3", 0, 0);
  co2_List_Add_String(citem[24].p_list, 9, 2, "##control_b4", 0, 0);
  co2_List_Add_String(citem[24].p_list, 10, 2, "##control_b5", 0, 0);
  co2_List_Add_String(citem[24].p_list, 11, 2, "##control_camera", 0, 1);
  co2_List_Add_String(citem[24].p_list, 12, 2, "##control_crotation", 0, 0);
  co2_List_Add_String(citem[24].p_list, 13, 2, "##control_cmove", 0, 0);
  co2_List_Add_String(citem[24].p_list, 14, 2, "##control_czoomin", 0, 0);
  co2_List_Add_String(citem[24].p_list, 15, 2, "##control_czoomout", 0, 0);
  co2_List_Add_String(citem[24].p_list, 16, 2, "##control_cfastleft", 0, 0);
  co2_List_Add_String(citem[24].p_list, 17, 2, "##control_cfastright", 0, 0);
  co2_List_Add_String(citem[24].p_list, 18, 2, "##control_ccamera", 0, 0);
  co2_List_Add_String(citem[24].p_list, 19, 2, "##control_game", 0, 1);
  co2_List_Add_String(citem[24].p_list, 20, 2, "##control_gmenu", 0, 0);
  co2_List_Add_String(citem[24].p_list, 21, 2, "##control_grestart", 0, 0);
  co2_List_Add_String(citem[24].p_list, 22, 2, "##control_ginventory", 0, 0);
  //co2_List_Add_String(citem[24].p_list, 23, 2, "##control_gusepack", 0, 0);
  co2_List_Add_String(citem[24].p_list, 23, 2, "##control_gnexttrack", 0, 0);
  co2_List_Add_String(citem[24].p_list, 24, 2, "##control_gturnoffi", 0, 0);
  co2_List_Add_String(citem[24].p_list, 25, 2, "##control_gturnofft", 0, 0);
  co2_List_Add_String(citem[24].p_list, 26, 2, "##control_gscreenshot", 0, 0);
  co2_List_Add_String(citem[24].p_list, 27, 2, "##control_gtransparent", 0, 0);
  co2_List_Add_String(citem[24].p_list, 28, 2, "##control_ghighlight", 0, 0);
  co2_List_Add_String(citem[24].p_list, 29, 2, "##control_gdrawconn", 0, 0);
  co2_List_Add_String(citem[24].p_list, 30, 2, "##control_pause", 0, 0);

  Key2String(setup.key[1], ctext);
  co2_List_Add_String(citem[24].p_list, 1, 550, ctext, setup.key[1], 0);
  Key2String(setup.key[2], ctext);
  co2_List_Add_String(citem[24].p_list, 2, 550, ctext, setup.key[2], 0);
  Key2String(setup.key[3], ctext);
  co2_List_Add_String(citem[24].p_list, 3, 550, ctext, setup.key[3], 0);
  Key2String(setup.key[4], ctext);
  co2_List_Add_String(citem[24].p_list, 4, 550, ctext, setup.key[4], 0);
  Key2String(setup.key[5], ctext);
  co2_List_Add_String(citem[24].p_list, 5, 550, ctext, setup.key[5], 0);
  Key2String(setup.key[6], ctext);
  co2_List_Add_String(citem[24].p_list, 6, 550, ctext, setup.key[6], 0);
  Key2String(setup.key[7], ctext);
  co2_List_Add_String(citem[24].p_list, 7, 550, ctext, setup.key[7], 0);
  Key2String(setup.key[8], ctext);
  co2_List_Add_String(citem[24].p_list, 8, 550, ctext, setup.key[8], 0);
  Key2String(setup.key[9], ctext);
  co2_List_Add_String(citem[24].p_list, 9, 550, ctext, setup.key[9], 0);
  Key2String(setup.key[10], ctext);
  co2_List_Add_String(citem[24].p_list, 10, 550, ctext, setup.key[10], 0);

  Key2String(setup.key[12], ctext);
  co2_List_Add_String(citem[24].p_list, 12, 550, ctext, setup.key[12], 0);
  Key2String(setup.key[13], ctext);
  co2_List_Add_String(citem[24].p_list, 13, 550, ctext, setup.key[13], 0);
  Key2String(setup.key[14], ctext);
  co2_List_Add_String(citem[24].p_list, 14, 550, ctext, setup.key[14], 0);
  Key2String(setup.key[15], ctext);
  co2_List_Add_String(citem[24].p_list, 15, 550, ctext, setup.key[15], 0);
  Key2String(setup.key[16], ctext);
  co2_List_Add_String(citem[24].p_list, 16, 550, ctext, setup.key[16], 0);
  Key2String(setup.key[17], ctext);
  co2_List_Add_String(citem[24].p_list, 17, 550, ctext, setup.key[17], 0);
  Key2String(setup.key[18], ctext);
  co2_List_Add_String(citem[24].p_list, 18, 550, ctext, setup.key[18], 0);

  Key2String(setup.key[20], ctext);
  co2_List_Add_String(citem[24].p_list, 20, 550, ctext, setup.key[20], 0);
  Key2String(setup.key[21], ctext);
  co2_List_Add_String(citem[24].p_list, 21, 550, ctext, setup.key[21], 0);
  Key2String(setup.key[22], ctext);
  co2_List_Add_String(citem[24].p_list, 22, 550, ctext, setup.key[22], 0);
  /*Key2String(setup.key[23], ctext);
     co2_List_Add_String(citem[24].p_list, 23, 550, ctext, setup.key[23], 0); */
  Key2String(setup.key[23], ctext);
  co2_List_Add_String(citem[24].p_list, 23, 550, ctext, setup.key[23], 0);
  Key2String(setup.key[24], ctext);
  co2_List_Add_String(citem[24].p_list, 24, 550, ctext, setup.key[24], 0);
  Key2String(setup.key[25], ctext);
  co2_List_Add_String(citem[24].p_list, 25, 550, ctext, setup.key[25], 0);
  Key2String(setup.key[26], ctext);
  co2_List_Add_String(citem[24].p_list, 26, 550, ctext, setup.key[26], 0);
  Key2String(setup.key[27], ctext);
  co2_List_Add_String(citem[24].p_list, 27, 550, ctext, setup.key[27], 0);
  Key2String(setup.key[28], ctext);
  co2_List_Add_String(citem[24].p_list, 28, 550, ctext, setup.key[28], 0);
  Key2String(setup.key[29], ctext);
  co2_List_Add_String(citem[24].p_list, 29, 550, ctext, setup.key[29], 0);
  Key2String(setup.key[30], ctext);
  co2_List_Add_String(citem[24].p_list, 30, 550, ctext, setup.key[30], 0);

  co2_List_Redraw(hdcTab[3], citem[24].p_list, 0);
}

void RunMenuSettings2(char *p_File_Name, AUDIO_DATA * p_ad,
                      LEVELINFO * p_Level, ANIMATION_MODULE * p_am)
{
  int mix, miy;
  float s_factor[2];
  RECT rTMP = { 0, 0, 1024, 768 };

  DWORD dwEplased = 0, dwStart, dwStop;

//      ROZLISENI       roz[POCETROZLICENI];
  int ActiveTab = 0;
  int hdcTab[TAB_NUM];
  int hdcTabUse[TAB_NUM];

  CONTROL_LIST_ITEM2 citem[CLIST_ITEMC];

//      FILE    *file;
  char dir[256];
  int lastcmd, lastanm, i;
  CMD_LINE *res = NULL;
  int lastabv = -1;
  char in, click = 0;
  int anmid = -1, resid = -1, anbind = -1;
  int bind;
  DeviceHandle dh = 0;
  int sh = K_CHYBA;
//  int iBmp;

  _3d_End_Draw();

  p_Level->bResetini = 0;

  p_Level->iCursor = 133;
  am_FlipA(p_Level, p_am, 1, &rline, CLIST_ITEMC, 0, 0, 0);


  _2d_Clear_RectLine(&rline);

  s_factor[0] = (float) 1024 / (float) hwconf.xres;
  s_factor[1] = (float) 768 / (float) hwconf.yres;

  ddx2Init(200, RGB(255, 0, 255));
  dh = ddx2DeviceCreate(TRUE, 32);

  if (!dh)
    return;

  //fn2_Release_Font();
  ddx2DeviceSetActive(dh);

  ddx2DeviceSetBackBufferSize(1024, 768);
  ddx2DeviceSetBackBufferRect(0, 0, 1024, 768);
  ddx2DeviceSetTextRenderRec(0, 0, 1024, 768);
  ddx2DeviceSetScreenRec(0, 0, hwconf.xres, hwconf.yres);

  //ddx2DeviceSetRender(TRUE);

  ddx2CleareSurface(DDX2_BACK_BUFFER);
  sh = ddx2LoadBitmap("settings.bmp", pBmpArchive);
  LoadHelp("3dsettings_load.txt");

  ddx2FillRect(DDX2_BACK_BUFFER, &rTMP, RGB(255, 0, 255));
  ddx2SetRect(&rTMP, 1);

  am_FlipA(p_Level, p_am, 1, &rline, CLIST_ITEMC, 0, 0, 0);

  // Stop renderu hry
  //ddx2GameRender(FALSE);

  // - volat jako soucast hry (po vykreslni listi)
  //void ddx2RenderujMenu(G_KONFIG *p_ber)


  // - volat pokud jede menu bez hry - maze obrazovku a kresli menu
  //void ddx2RenderujVse(G_KONFIG *p_ber)

  // flipovat musis sam vzdycky
  // volej ale v kazdem frame jen jednu tuto funkci (podle toho jesli
  // 2D grafika je soucasti hry nebo jede samostatne)

  _2d_Clear_RectLine(&rline);

  res = (CMD_LINE *) malloc(RES_NUM * sizeof(CMD_LINE));

  if (!res) {
    kprintf(1, "RunMenuSettings2: Out of Memory");
    return;
  }
  else
    ZeroMemory(res, RES_NUM * sizeof(CMD_LINE));

//      ZeroMemory(roz, POCETROZLICENI * sizeof(ROZLISENI));

  Load_ini();

  iCompositDC = ddx2FindFreeSurface();
  CompositDC = ddx2CreateSurface(1024, 768, iCompositDC);
  iFontDC = ddx2FindFreeSurface();
  FontDC = ddx2CreateSurface(1024, 768, iFontDC);

  ZeroMemory(citem, CLIST_ITEMC * sizeof(CONTROL_LIST_ITEM2));

  am_FlipA(p_Level, p_am, 1, &rline, CLIST_ITEMC, 0, 0, 0);

  fn2_Set_Font(cFontFile[2]);
  fn2_Load_Bitmaps();

  am_FlipA(p_Level, p_am, 1, &rline, CLIST_ITEMC, 0, 0, 0);

  for (bind = 0; bind < RES_NUM; bind++) {
    for (lastcmd = 0; lastcmd < 200; lastcmd++) {
      res[bind].iAnim[lastcmd][11] = -1;
      res[bind].iAnim[lastcmd][0] = -1;
    }

    for (in = 0; in < 6; in++)
      res[bind].iParam[(int)in] = -1;

    res[bind].iLayer = 0;
  }

  lastcmd = 0;
  timercnt = 0;

  GetPrivateProfileString("game", "data_dir", "c:\\", dir, 256, ini_file);
  working_file_translate(dir, 256);
  chdir((dir));

  //natadhe skript menu
  LoadMenuScript(p_File_Name, res, &lastcmd);

  in = 0;

  CreateFontAnimations2(res, &lastcmd, 0, 0);

  am_FlipA(p_Level, p_am, 1, &rline, CLIST_ITEMC, 0, 0, 0);

  // privede prikazy, ketere se maji provest na zacatku a, kresleni, flip,
  // animace na OnAbove
  for (i = 0; i < lastcmd; i++) {
    lastanm = 0;

    switch (res[i].iParam[0]) {
      case COM_DRAW:
        if (!res[i].iLayer) {
          //ddx2DrawSurface(BackDC, res[i].iParam, 2);
          ddx2DrawDisplay(res[i].iParam, 0);
        }
        else {
          //ddx2DrawSurfaceColorKey(BackDC, res[i].iParam, 2, TRANSCOLOR );
          //ddx2DrawSurface(FontDC, res[i].iParam, 3);
          ddx2DrawDisplayColorKey(res[i].iParam, 0, TRANSCOLOR);
          ddx2DrawSurface(FontDC, res[i].iParam, 3);
        }
        break;
      case COM_RANDOMANIMATION:
      case COM_ONCLICK:
      case COM_ONABOVE:
      case COM_RUNANIMATION:
      case COM_BINDEXITANIMATION:
      case COM_BINDANIMATION:
        //nahrati animace k udalosti OnAbove
        LoadAnimationMenuScript(res, i, &lastanm);
        break;
    }
  }

  //DisplayFrame();

  am_FlipA(p_Level, p_am, 1, &rline, CLIST_ITEMC, 0, 0, 0);

  for (i = 0; i < TAB_NUM; i++) {
    hdcTab[i] = ddx2CreateSurface(TAB_XRES, TAB_YRES, ddx2FindFreeSurface());

    if (hdcTab[i])
      ddx2BitBlt(hdcTab[i], 0, 0, TAB_XRES, TAB_YRES, HDC2DD, TAB_X, TAB_Y);

    hdcTabUse[i] = 0;
  }

  am_FlipA(p_Level, p_am, 1, &rline, CLIST_ITEMC, 0, 0, 0);

  if (!co2_Load_Graphic(0)) {
    kprintf(1, "co2_Load_Graphic ... can't load controls!");
    //MessageBox(NULL,"co2_Load_Graphic","co2_Load_Graphic", MB_OK);
  }
  else {
//    int count = 0;

    citem[0].bActive = 0;
    citem[1].bActive = 0;

    /*citem[2].p_check = co2_Create_CheckBox(hdcTab[0], 25, 50, "##settings_camera_rot", 0, 0);
       co2_Check_Set_State(citem[2].p_check, hdcTab[0], setup.pohled_berusky, 1);
       citem[2].bActive = 1; */

    citem[3].p_check =
      co2_Create_CheckBox(hdcTab[0], 25, 50, "##settings_camera_mov", 0, 1);
    co2_Check_Set_State(citem[3].p_check, hdcTab[0], setup.posouvat_kameru,
      1);
    citem[3].bActive = 1;

    citem[4].p_check =
      co2_Create_CheckBox(hdcTab[0], 25, 80, "##settings_b1_control", 0, 2);
    co2_Check_Set_State(citem[4].p_check, hdcTab[0], setup.ovladani, 1);
    citem[4].bActive = 1;

    citem[35].p_check =
      co2_Create_CheckBox(hdcTab[0], 25, 110,
      "##settings_beathe_vis_at_start", 0, 10);
    co2_Check_Set_State(citem[35].p_check, hdcTab[0],
      setup.start_zpruhlednovani, 1);
    citem[35].bActive = 1;

    citem[36].p_check =
      co2_Create_CheckBox(hdcTab[0], 25, 140, "##settings_items_vis_at_start",
      0, 11);
    co2_Check_Set_State(citem[36].p_check, hdcTab[0],
      setup.start_zvyraznovani, 1);
    citem[36].bActive = 1;

    citem[41].p_check =
      co2_Create_CheckBox(hdcTab[0], 25, 200, "##settings_camera_intro", 0,
      16);
    co2_Check_Set_State(citem[41].p_check, hdcTab[0], setup.camera_intro, 1);
    citem[41].bActive = 1;

    citem[37].p_check =
      co2_Create_CheckBox(hdcTab[0], 25, 230, "##settings_camera_rect", 0,
      12);
    co2_Check_Set_State(citem[37].p_check, hdcTab[0], setup.ovladani_rohy, 1);
    citem[37].bActive = 1;

    citem[38].p_check =
      co2_Create_CheckBox(hdcTab[0], 25, 260, "##settings_implicit_move", 0,
      13);

    if (setup.ovladani_rohy_default)
      co2_Check_Set_State(citem[38].p_check, hdcTab[0], 0, 1);
    else
      co2_Check_Set_State(citem[38].p_check, hdcTab[0], 1, 1);

    citem[38].bActive = 1;

    citem[39].p_check =
      co2_Create_CheckBox(hdcTab[0], 25, 290, "##settings_imlicit_rot", 0,
      14);

    if (setup.ovladani_rohy_default)
      co2_Check_Set_State(citem[39].p_check, hdcTab[0], 1, 1);
    else
      co2_Check_Set_State(citem[39].p_check, hdcTab[0], 0, 1);

    citem[39].bActive = 1;

    citem[40].p_check =
      co2_Create_CheckBox(hdcTab[0], 25, 320, "##settings_camera_swap", 0,
      15);
    co2_Check_Set_State(citem[40].p_check, hdcTab[0],
      setup.ovladani_rohy_smer, 1);
    citem[40].bActive = 1;

    /*citem[41].p_check = co2_Create_CheckBox(hdcTab[0], 25, 350, "##setings_camera_move_cur", 0, 16);
       co2_Check_Set_State(citem[41].p_check, hdcTab[0], setup.ovladani_pr_posun, 1);
       citem[41].bActive = 1; */

    co2_Set_Text_Right(hdcTab[0], "##setings_camera_speed", 0, 450, 390);
    citem[42].p_prog = co2_Create_Progres(hdcTab[0], 500, 387, 0, 10, 0);
    co2_Progres_Set(citem[42].p_prog, hdcTab[0],
      ftoi(setup.ovladani_rohy_rychlost * 10));
    citem[42].bActive = 1;

    co2_Set_Text_Right(hdcTab[0], "##settings_trans_radius", 0, 450, 425);
    citem[7].p_prog = co2_Create_Progres(hdcTab[0], 500, 412, 1, 20, 1);
    co2_Progres_Set(citem[7].p_prog, hdcTab[0],
      ftoi(setup.p_kamera_radius * 2));
    citem[7].bActive = 1;

    /*if(!setup.posouvat_kameru)
       co2_Check_Disable(hdcTab[0], 0, 0, citem, CLIST_ITEMC, 0); */

    if (!setup.ovladani_rohy) {
      co2_Check_Disable(hdcTab[0], 0, 0, citem, CLIST_ITEMC, 13);
      co2_Check_Disable(hdcTab[0], 0, 0, citem, CLIST_ITEMC, 14);
      co2_Check_Disable(hdcTab[0], 0, 0, citem, CLIST_ITEMC, 15);
      //co2_Check_Disable(hdcTab[0], 0, 0, citem, CLIST_ITEMC, 16);
      co2_Progres_Disable(hdcTab[0], 0, 0, citem, CLIST_ITEMC, 0, 1,
        hdcTab[0]);
    }

    SetTab2(0, -1, citem, CLIST_ITEMC, hdcTab);
    hdcTabUse[0] = 1;

    /*if(!setup.posouvat_kameru)
       co2_Check_Disable(HDC2DD, TAB_X, TAB_Y, citem, CLIST_ITEMC, 0); */

    if (!setup.ovladani_rohy) {
      co2_Check_Disable(HDC2DD, TAB_X, TAB_Y, citem, CLIST_ITEMC, 13);
      co2_Check_Disable(HDC2DD, TAB_X, TAB_Y, citem, CLIST_ITEMC, 14);
      co2_Check_Disable(HDC2DD, TAB_X, TAB_Y, citem, CLIST_ITEMC, 15);
      //co2_Check_Disable(HDC2DD, TAB_X, TAB_Y, citem, CLIST_ITEMC, 16);
      co2_Progres_Disable(HDC2DD, TAB_X, TAB_Y, citem, CLIST_ITEMC, 0, 0,
        HDC2DD);
    }

  }


  am_FlipA(p_Level, p_am, 1, &rline, CLIST_ITEMC, 0, 0, 0);

//BEGIN_MENU:

  for (i = 0; i < lastcmd; i++)
    if (res[i].iParam[0] == COM_RUNANIMATION) {
      int iWave = AddAnimation2(&res[i], &ad, 0);

      if (iWave != -1) {
        if (res[i + 1].iParam[0] == COM_BINDSOUND)
          anm[iWave].iWave = res[i + 1].iParam[5] =
            mPlaySound2(&res[i + 1], &ad, 2);
      }
    }

  mi.t1 = 0;
  mi.t2 = 0;
  mi.dx = 0;
  mi.dy = 0;
  anmid = -1;
  resid = -1;
  anbind = -1;
  bind = -1;
  lastabv = -1;
  in = 0;

  p_Level->iCursor = 0;
  ddx2GameRender(FALSE);
  ddx2DeviceSetRender(TRUE);
  spracuj_spravy(0);

  while (!key[K_ESC]) {
    mix = mi.x;
    miy = mi.y;

    mi.x = (int) ceil(mi.x * s_factor[0]);
    mi.y = (int) ceil(mi.y * s_factor[1]);

    dwStart = timeGetTime();

    //pohnul mysi
    if (mi.dx || mi.dy) {
      //dostala se mys do akcni oblasti (OnAbove)?
      if (!click)
        for (i = 0; i < lastcmd; i++)
          if (res[i].iParam[0] == COM_ONABOVE) {
            if ((mi.x >= res[i].iParam[1]) &&
              (mi.x <= res[i].iParam[3]) &&
              (mi.y >= res[i].iParam[2]) && (mi.y <= res[i].iParam[4])) {
              //spusteni animace v OnAbove
              if (i != lastabv) {
                if (in) {
                  Stop2(&res[lastabv]);

                  if (!res[lastabv].iLayer) {
                    ddx2DrawDisplay(res[lastabv].iAnim[0], 0);
                    ddx2DrawSurface(CompositDC, res[lastabv].iAnim[0], 3);
                  }
                  else {
                    ddx2DrawDisplayColorKey(res[lastabv].iAnim[0], 3,
                      TRANSCOLOR);
                    ddx2DrawSurface(FontDC, res[lastabv].iAnim[0], 3);
                  }
                }

                CheckAnimation2(&res[i], &ad);

                lastabv = i;
                AddAnimation2(&res[i], &ad, 0);
                in = 1;

                bind = ChooseBidedAnimation2(res, i + 1, &ad);

                if (bind != -1) {
                  CheckAnimation2(&res[bind], &ad);
                  AddAnimation2(&res[bind], &ad, 1);
                  anbind = bind;

                  mPlaySound2(&res[bind], &ad, 0);
                }

                strcpy(dir, res[i].cParam[1]);
              }
            }
            else if (lastabv == i) {
              // odesel z oblasti, ktera byla aktivni -> stop animace                                 
              // a odznaceni oblasti
              Stop2(&res[i]);

              if (!res[i].iLayer) {
                ddx2DrawDisplay(res[i].iAnim[0], 0);
                ddx2DrawSurface(CompositDC, res[i].iAnim[0], 3);
              }
              else {
                ddx2DrawDisplayColorKey(res[i].iAnim[0], 0, TRANSCOLOR);
                ddx2DrawSurface(FontDC, res[i].iAnim[0], 3);
              }

              bind = ChooseBidedExitAnimation2(res, i + 1, &ad);

              if (bind != -1) {
                int iAnim;

                if (anbind != -1) {
                  Stop2(&res[anbind]);

                  if (!res[i].iLayer) {
                    ddx2DrawDisplay(res[anbind].iAnim[0], 0);
                    ddx2DrawSurface(CompositDC, res[anbind].iAnim[0], 3);
                  }
                  else {
                    ddx2DrawDisplayColorKey(res[anbind].iAnim[0], 0,
                      TRANSCOLOR);
                    ddx2DrawSurface(FontDC, res[anbind].iAnim[0], 3);
                  }
                }

                iAnim = AddAnimation2(&res[bind], &ad, 1);

                if (iAnim != -1)
                  anm[iAnim].iWave = mPlaySound2(&res[bind], &ad, 2);
              }

              lastabv = -1;
              anbind = -1;
              in = 0;

              strcpy(dir, "");
            }
          }

      mi.dx = 0;
      mi.dy = 0;
    }

    co2_Handle_Controls(citem, CLIST_ITEMC, mi.x - TAB_X, mi.y - TAB_Y,
      HDC2DD, TAB_X, TAB_Y);


		if(co2_Progres_Changed(citem, CLIST_ITEMC, 4))
			ad.Sound_Gain = co2_Progres_Get(citem, CLIST_ITEMC, 4) / 100.0f;

    if (co2_Progres_Changed(citem, CLIST_ITEMC, 5)) {

			float laf = ad.Ambient_Gain;

			ad.Ambient_Gain = co2_Progres_Get(citem, CLIST_ITEMC, 5) / 100.0f;
			adas_Set_Source_Gain(-1, LOOPING_TYPE, UNDEFINED_VALUE, ad.Ambient_Gain);
			
			if(laf < 0.05f && ad.Ambient_Gain >= 0.05f)
			{
				adas_Release_Source(-1, LOOPING_TYPE, UNDEFINED_VALUE);
				as_Start(p_Level->Environment.id, &ad, p_Level);
			}
    }


    if (co2_Progres_Changed(citem, CLIST_ITEMC, 6)) {
    
			float f = co2_Progres_Get(citem, CLIST_ITEMC, 6) / 100.0f;

			ogg_gain(f);

			ad.Music_Gain = f;

			if(f >= 0.05f && !ogg_playing())
			{
				ad.Music_Gain = f;

				ap_Stop_Song(&ad);
				ap_Play_Song(0,1,&ad);
				//adas_OGG_Set_Priority(p_Level->Base_Priority);
				//ap_Play_Song(0,0,&ad);
			}
    }

    //stlacil leve tlacitko
    if (mi.t1 && !click) {
      int iCheck = -1;
      LIST_VIEW_CONTROL2 *p_li;

      iCheck = co2_Check_Get_State_Change(citem, CLIST_ITEMC, 12);

      if (iCheck != -1) {
        if (!iCheck) {
          co2_Check_Disable(HDC2DD, TAB_X, TAB_Y, citem, CLIST_ITEMC, 13);
          co2_Check_Disable(HDC2DD, TAB_X, TAB_Y, citem, CLIST_ITEMC, 14);
          co2_Check_Disable(HDC2DD, TAB_X, TAB_Y, citem, CLIST_ITEMC, 15);
          //co2_Check_Disable(HDC2DD, TAB_X, TAB_Y, citem, CLIST_ITEMC, 16);
          co2_Progres_Disable(HDC2DD, TAB_X, TAB_Y, citem, CLIST_ITEMC, 0, 1,
            HDC2DD);
        }
        else {
          co2_Check_Enable(HDC2DD, TAB_X, TAB_Y, citem, CLIST_ITEMC, 13);
          co2_Check_Enable(HDC2DD, TAB_X, TAB_Y, citem, CLIST_ITEMC, 14);
          co2_Check_Enable(HDC2DD, TAB_X, TAB_Y, citem, CLIST_ITEMC, 15);
          //co2_Check_Enable(HDC2DD, TAB_X, TAB_Y, citem, CLIST_ITEMC, 16);
          co2_Progres_Enable(HDC2DD, TAB_X, TAB_Y, citem, CLIST_ITEMC, 0);
        }
      }

      if (ActiveTab == 0 && !citem[39].bDisabled) {
        iCheck = co2_Check_Get_State_Change(citem, CLIST_ITEMC, 13);

        if (iCheck != -1 && iCheck == 1)
          co2_Check_Set_State(citem[39].p_check, HDC2DD, 0, 1);
        else if (iCheck != -1 && !iCheck)
          co2_Check_Set_State(citem[39].p_check, HDC2DD, 1, 1);

        iCheck = co2_Check_Get_State_Change(citem, CLIST_ITEMC, 14);

        if (iCheck != -1 && iCheck == 1)
          co2_Check_Set_State(citem[38].p_check, HDC2DD, 0, 1);
        else if (iCheck != -1 && !iCheck)
          co2_Check_Set_State(citem[38].p_check, HDC2DD, 1, 1);
      }

      if (co2_List_Get_Clck(citem, CLIST_ITEMC, 0, &p_li) == 1) {
        float pos[3] = { 0, 0, 0 };
        ap_Play_Sound(0,1,0,pos,54 + (rand()%3), NULL, &ad);
        SetCharMenu2(p_li, p_Level, p_am, mix, miy);
      }

      //dostala se mys do akcni oblasti (OnClick)?
      for (i = 0; i < lastcmd; i++)
        if (res[i].iParam[0] == COM_ONCLICK)
          if ((mi.x >= res[i].iParam[1]) &&
            (mi.x <= res[i].iParam[3]) &&
            (mi.y >= res[i].iParam[2]) && (mi.y <= res[i].iParam[4])) {
            if (res[i].iAnim[0][0] >= 0) {
              //pokud je animace, tak ji spust
              anmid = AddAnimation2(&res[i], &ad, 0);

              if (res[i + 1].iParam[0] == COM_BINDSOUND)
                mPlaySound2(&res[i + 1], &ad, 1);

              resid = i;
              click = 1;
            }
            else {
              if (res[i + 1].iParam[0] == COM_BINDSOUND)
                mPlaySound2(&res[i + 1], &ad, 1);

              resid = i;
              click = 1;
              anmid = 31;
            }
          }

      mi.t1 = 0;
    }

    //provedeni akce po animaci menu
    if (click)
      if (!anm[anmid].cmd) {
        click = 0;

        //StopAll();

        if (!strcmp(res[resid].cParam[1], "EXIT")) {
          float pos[3] = { 0, 0, 0 };
          ap_Play_Sound(0,1,0,pos,54 + (rand()%3), NULL, &ad);
          key[K_ESC] = 1;
        }

        if (!strcmp(res[resid].cParam[1], "OK")) {
          float pos[3] = { 0, 0, 0 };
          ap_Play_Sound(0,1,0,pos,54 + (rand()%3), NULL, &ad);
          SetMenuSettings2(citem, hdcTabUse);
          Save_ini();
          key[K_ESC] = 1;
          p_Level->bResetini = 1;
        }

        if (!strcmp(res[resid].cParam[1], "GAME") && ActiveTab) {
          SetTab2(0, ActiveTab, citem, CLIST_ITEMC, hdcTab);
          ActiveTab = 0;
          hdcTabUse[0]++;
        }

        if (!strcmp(res[resid].cParam[1], "3D") && ActiveTab != 1) {
          if (!hdcTabUse[1])
            InitTab3d2(citem, hdcTab);

          SetTab2(1, ActiveTab, citem, CLIST_ITEMC, hdcTab);
          ActiveTab = 1;

          hdcTabUse[1]++;
        }


        if (!strcmp(res[resid].cParam[1], "SOUND") && ActiveTab != 2) {
          if (!hdcTabUse[2])
            InitTabSound2(citem, hdcTab);

          SetTab2(2, ActiveTab, citem, CLIST_ITEMC, hdcTab);
          ActiveTab = 2;
          hdcTabUse[2]++;
        }

        if (!strcmp(res[resid].cParam[1], "CONTROLS") && ActiveTab != 3) {
          if (!hdcTabUse[3])
            InitTabControls2(citem, hdcTab);

          SetTab2(3, ActiveTab, citem, CLIST_ITEMC, hdcTab);
          ActiveTab = 3;
          hdcTabUse[3]++;
        }

        resid = -1;

        if (key[K_ESC]) {          
          for(i=0;i<lastcmd;i++) {
            if(res[i].iParam[0] == COM_BINDSOUND && res[i].iParam[5] != -1) {
              adas_Release_Source(PARTICULAR_SOUND_SOURCE, UNDEFINED_VALUE, res[i].iParam[5]);
              res[i].iParam[5] = -1;
            }
          }
          goto __QUIT;
        }
      }

    //pokud prisel cas, tak provedu nahodne animace (podle jejich pravdepodobnosti)
    if (timercnt > 500) {
      timercnt = 0;

      for (i = 0; i < lastcmd; i++)
        if (res[i].iParam[0] == COM_RANDOMANIMATION)
          if (rand() % 200 <= res[i].iParam[1] &&
            strcmp(dir, res[i].cParam[0])) {
            CheckAnimation2(&res[i], &ad);
            AddAnimation2(&res[i], &ad, 0);
          }
    }

    dwStop = timeGetTime();

    dwEplased += dwStop - dwStart;

    AnimationEvent2(NULL, 0, 0, dwStop);

    mi.x = mix;
    mi.y = miy;

    am_Flip(p_Level, p_am, 1, &rline, CLIST_ITEMC, 0, 0, 0);
  }

__QUIT:
  ddx2ReleaseBitmap(iCompositDC);
  ddx2ReleaseBitmap(iFontDC);
//      ddx2ReleaseBitmap(iBackDC);

  SetMenuSettingsS2(citem, hdcTabUse);
  Save_ini();

  fn2_Release_Font();
  StopAll2();
  co2_Handle_Release(citem, CLIST_ITEMC);
  co2_Release_Graphic();
  key[K_ESC] = 0;

  if (dh) {
    ddx2ReleaseBitmap(sh);
    ddx2DeviceDeaktivuj();
    ddx2DeviceRemove(dh);
    ddx2Release();
    dh = 0;
  }

  FreeAnimations2(res, RES_NUM);
  free((void *) res);

//      fn2_Set_Font("font3d.pak");
//      fn2_Load_Bitmaps();

  // Start renderu hry
  ddx2GameRender(TRUE);
}

int FillListLoad2(LIST_VIEW_CONTROL2 * p_li, char *mask, char bAdd,
  char bLoad)
{
//      char text[128];
//      char *t;
  char dir[256];
  WCHAR wFile[128];
  int x = 0;
  int c = 0;

  //long Done, error;
  //struct _finddata_t    Data;

  LIST_ITEM_ *list;
  int isize;

  GetPrivateProfileString("game", "save_dir", "c:\\", dir, 256, ini_file);
  working_file_translate(dir, 256);
  chdir((dir));

  FillStringList(mask, &list, &isize);

  if (!isize)
    return 0;

  for (c = 0; c < isize; c++) {
    ZeroMemory(wFile, 128 * sizeof(WCHAR));

    if (bLoad == 2) {
      if (!demo_Check_Owner(pPlayerProfile.cName, list[c].text, wFile))
        continue;
    }
    else if (!check_Save_Owner(list[c].text, wFile))
      continue;

    if (bAdd)
      co2_List_Add_StringWC2(p_li, x, 2, wFile, list[c].text, x, 0);

    x++;
  }

/*	Done = _findfirst(mask,&Data);
	error = Done;
			
	while(error != -1)
	{
		ZeroMemory(wFile, 128 * sizeof(WCHAR));

		if(error != -1)
		{
			if(bLoad == 2)
			{
				if(!demo_Check_Owner(pPlayerProfile.cName, Data.name, wFile))
				{
					error = _findnext(Done,&Data);
					continue;
				}
			}
			else
				if(!check_Save_Owner(Data.name, wFile))
				{
					error = _findnext(Done,&Data);
					continue;
				}

				if(bAdd)
					co2_List_Add_StringWC2(p_li, c, 2, wFile, Data.name, c, 0);

			error = _findnext(Done,&Data);
			c++;
		}
	}
	_findclose(Done); */

  free((void *) list);
  return x;
}

int RunMenuLoadGameLoad2(char *p_File_Name, AUDIO_DATA * p_ad,
  LEVELINFO * p_Level, ANIMATION_MODULE * p_am, char bLoad, WCHAR * cText)
{
  int iReturn = 0;
  int mix, miy;
  float s_factor[2], s_bfactor[2];
  RECT rTMP = { 299, 209, 444, 415 };
  //RECT  rTMP = {0,0,1024,768};

  DWORD dwEplased = 0, dwStart, dwStop;

  CONTROL_LIST_ITEM2 citem[CLIST_ITEMC];

//      FILE    *file;
  char dir[256];
  int lastcmd, lastanm, i;
  CMD_LINE *res = NULL;
  int lastabv = -1;
  char in, click = 0;
  int anmid = -1, resid = -1, anbind = -1;
  int bind;
  DeviceHandle dh = 0;
  int sh = K_CHYBA;

  _3d_End_Draw();
  //kprintf(1, "----------------- RunMenuLoadGameLoad2 ---------------------");

  p_Level->bResetini = 0;

  _2d_Clear_RectLine(&rline);

  s_factor[0] = (float) 1024 / (float) hwconf.xres;
  s_factor[1] = (float) 768 / (float) hwconf.yres;

  s_bfactor[0] = (float) hwconf.xres / 1024.0f;
  s_bfactor[1] = (float) hwconf.yres / 768.0f;

  p_Level->iCursor = 133;
  am_FlipA(p_Level, p_am, 1, &rline, CLIST_ITEMC, 0, 0, 0);
  p_Level->iCursor = 0;

  ddx2Init(200, RGB(255, 0, 255));
  dh = ddx2DeviceCreate(TRUE, 32);

  if (!dh)
    return 0;

  //fn2_Release_Font();
  ddx2DeviceSetActive(dh);

  ddx2DeviceSetBackBufferSize(1024, 768);
  ddx2DeviceSetBackBufferRect(299, 209, 444, 415);
  ddx2DeviceSetTextRenderRec(0, 0, 444, 415);
  ddx2DeviceSetScreenRec((int) ceil(s_bfactor[0] * 299),
    (int) ceil(s_bfactor[1] * 209), (int) ceil(s_bfactor[0] * 444),
    (int) ceil(s_bfactor[1] * 415));

  ddx2DeviceSetRender(TRUE);

  ddx2CleareSurface(DDX2_BACK_BUFFER);
  sh = ddx2LoadBitmap("smallmenu.bmp", pBmpArchive);

  ddx2SetRect(&rTMP, 1);

  ddx2GameRender(TRUE);

  res = (CMD_LINE *) malloc(RES_NUM * sizeof(CMD_LINE));

  if (!res) {
    kprintf(1, "RunMenuLoadGameLoad2: Out of Memory");
    return 0;
  }
  else
    ZeroMemory(res, RES_NUM * sizeof(CMD_LINE));

  iCompositDC = ddx2FindFreeSurface();
  CompositDC = ddx2CreateSurface(1024, 768, iCompositDC);
  iFontDC = ddx2FindFreeSurface();
  FontDC = ddx2CreateSurface(1024, 768, iFontDC);

  ZeroMemory(citem, CLIST_ITEMC * sizeof(CONTROL_LIST_ITEM2));

  fn2_Set_Font(cFontFile[0]);
  fn2_Load_Bitmaps();

  for (bind = 0; bind < RES_NUM; bind++) {
    for (lastcmd = 0; lastcmd < 200; lastcmd++) {
      res[bind].iAnim[lastcmd][11] = -1;
      res[bind].iAnim[lastcmd][0] = -1;
    }

    for (in = 0; in < 6; in++)
      res[bind].iParam[(int)in] = -1;

    res[bind].iLayer = 0;
  }

  lastcmd = 0;
  timercnt = 0;

  GetPrivateProfileString("game", "data_dir", "c:\\", dir, 256, ini_file);
  working_file_translate(dir, 256);
  chdir((dir));

  //natadhe skript menu
  LoadMenuScript(p_File_Name, res, &lastcmd);

  in = 0;

  CreateFontAnimations2(res, &lastcmd, 0, 0);

  // privede prikazy, ketere se maji provest na zacatku a, kresleni, flip,
  // animace na OnAbove
  for (i = 0; i < lastcmd; i++) {
    lastanm = 0;

    switch (res[i].iParam[0]) {
      case COM_DRAW:
        if (!res[i].iLayer)
          ddx2DrawDisplayColorKey(res[i].iParam, 0, TRANSCOLOR);
        else {
          ddx2DrawDisplayColorKey(res[i].iParam, 0, TRANSCOLOR);
          ddx2DrawSurface(FontDC, res[i].iParam, 3);
        }
        break;
      case COM_RANDOMANIMATION:
      case COM_ONCLICK:
      case COM_ONABOVE:
      case COM_RUNANIMATION:
      case COM_BINDEXITANIMATION:
      case COM_BINDANIMATION:
        //nahrati animace k udalosti OnAbove
        LoadAnimationMenuScript(res, i, &lastanm);
        break;
    }
  }

  if (!co2_Load_Graphic(0))
    kprintf(1, "co2_Load_Graphic ... con't load controls!");
  else {
    RECT r;
    int xx;

    r.left = 299;
    r.top = 209;
    r.right = 743;
    r.bottom = 359;

    if (!bLoad)                 //save
      co2_Set_Text_Center(HDC2DD, "##menu_savegame", 0, r);
    else if (bLoad == 1)        // load
      co2_Set_Text_Center(HDC2DD, "##menu_loadgame", 0, r);
    else                        //save demo
      co2_Set_Text_Center(HDC2DD, "##menu_savedemo", 0, r);

    fn2_Release_Font();

    fn2_Set_Font(cFontFile[2]);
    fn2_Load_Bitmaps();

    xx = FillListLoad2(NULL, "*", 0, bLoad);

    if (bLoad == 1) {
      if (xx < 6)
        xx = 6;

      citem[1].p_list = co2_Create_List(HDC2DD, 360, 320, 320, 185, 0, xx, 1);
      citem[1].bActive = 1;
      FillListLoad2(citem[1].p_list, "*", 1, bLoad);
    }
    else if (!bLoad) {
      if (xx < 5)
        xx = 5;

      citem[0].p_edit = co2_Create_Edit(HDC2DD, 373, 485, 0);
      citem[0].bActive = 1;

      citem[1].p_list = co2_Create_List(HDC2DD, 360, 320, 320, 150, 0, xx, 1);
      citem[1].bActive = 1;
      FillListLoad2(citem[1].p_list, "*", 1, bLoad);
    }
    else {
      xx = FillListLoad2(NULL, "*.dem", 0, bLoad);

      if (xx < 5)
        xx = 5;

      citem[0].p_edit = co2_Create_Edit(HDC2DD, 373, 485, 0);
      citem[0].bActive = 1;

      citem[1].p_list = co2_Create_List(HDC2DD, 360, 320, 320, 150, 0, xx, 1);
      citem[1].bActive = 1;
      FillListLoad2(citem[1].p_list, "*.dem", 1, bLoad);
    }


    co2_List_Redraw(HDC2DD, citem[1].p_list, 0);
  }


//BEGIN_MENU:

  for (i = 0; i < lastcmd; i++)
    if (res[i].iParam[0] == COM_RUNANIMATION) {
      int iWave = AddAnimation2(&res[i], &ad, 0);

      if (iWave != -1) {
        if (res[i + 1].iParam[0] == COM_BINDSOUND)
          anm[iWave].iWave = res[i + 1].iParam[5] =
            mPlaySound2(&res[i + 1], &ad, 2);
      }
    }

  mi.t1 = 0;
  mi.t2 = 0;
  mi.dx = 0;
  mi.dy = 0;
  anmid = -1;
  resid = -1;
  anbind = -1;
  bind = -1;
  lastabv = -1;
  in = 0;

  spracuj_spravy(0);

  while (!key[K_ESC]) {
    mix = mi.x;
    miy = mi.y;

    mi.x = (int) ceil(mi.x * s_factor[0]);
    mi.y = (int) ceil(mi.y * s_factor[1]);

    dwStart = timeGetTime();

    //pohnul mysi
    if ((mi.dx || mi.dy) && !p_Level->bRestart && !p_Level->bReturnToMenu) {
      //dostala se mys do akcni oblasti (OnAbove)?
      if (!click)
        for (i = 0; i < lastcmd; i++)
          if (res[i].iParam[0] == COM_ONABOVE) {
            if ((mi.x >= res[i].iParam[1]) &&
              (mi.x <= res[i].iParam[3]) &&
              (mi.y >= res[i].iParam[2]) && (mi.y <= res[i].iParam[4])) {
              //spusteni animace v OnAbove
              if (i != lastabv) {
                if (in) {
                  Stop2(&res[lastabv]);

                  if (!res[lastabv].iLayer) {
                    ddx2DrawDisplay(res[lastabv].iAnim[0], 0);
                    ddx2DrawSurface(CompositDC, res[lastabv].iAnim[0], 3);
                  }
                  else {
                    ddx2DrawDisplayColorKey(res[lastabv].iAnim[0], 3,
                      TRANSCOLOR);
                    ddx2DrawSurface(FontDC, res[lastabv].iAnim[0], 3);
                  }
                }

                CheckAnimation2(&res[i], &ad);

                lastabv = i;
                AddAnimation2(&res[i], &ad, 0);
                in = 1;

                bind = ChooseBidedAnimation2(res, i + 1, &ad);

                if (bind != -1) {
                  CheckAnimation2(&res[bind], &ad);
                  AddAnimation2(&res[bind], &ad, 1);
                  anbind = bind;

                  mPlaySound2(&res[bind], &ad, 0);
                }

                strcpy(dir, res[i].cParam[1]);
              }
            }
            else if (lastabv == i) {
              // odesel z oblasti, ktera byla aktivni -> stop animace                                 
              // a odznaceni oblasti
              Stop2(&res[i]);

              if (!res[i].iLayer) {
                ddx2DrawDisplay(res[i].iAnim[0], 0);
                ddx2DrawSurface(CompositDC, res[i].iAnim[0], 3);
              }
              else {
                ddx2DrawDisplayColorKey(res[i].iAnim[0], 0, TRANSCOLOR);
                ddx2DrawSurface(FontDC, res[i].iAnim[0], 3);
              }

              bind = ChooseBidedExitAnimation2(res, i + 1, &ad);

              if (bind != -1) {
                int iAnim;

                if (anbind != -1) {
                  Stop2(&res[anbind]);

                  if (!res[i].iLayer) {
                    ddx2DrawDisplay(res[anbind].iAnim[0], 0);
                    ddx2DrawSurface(CompositDC, res[anbind].iAnim[0], 3);
                  }
                  else {
                    ddx2DrawDisplayColorKey(res[anbind].iAnim[0], 0,
                      TRANSCOLOR);
                    ddx2DrawSurface(FontDC, res[anbind].iAnim[0], 3);
                  }
                }

                iAnim = AddAnimation2(&res[bind], &ad, 1);

                if (iAnim != -1)
                  anm[iAnim].iWave = mPlaySound2(&res[bind], &ad, 2);
              }

              lastabv = -1;
              anbind = -1;
              in = 0;

              strcpy(dir, "");
            }
          }

      mi.dx = 0;
      mi.dy = 0;
    }

    if (!p_Level->bRestart && !p_Level->bReturnToMenu)
      co2_Handle_Controls(citem, CLIST_ITEMC, mi.x, mi.y, HDC2DD, 0, 0);

    //stlacil leve tlacitko
    if (mi.t1 && !click && !p_Level->bRestart && !p_Level->bReturnToMenu) {
      LIST_VIEW_CONTROL2 *p_li;

      if (co2_List_Get_Clck(citem, CLIST_ITEMC, 0, &p_li) == 1) {
        if (p_li->cSelected != -1) {
          float pos[3] = { 0, 0, 0 };
          ap_Play_Sound(0,1,0,pos,54 + (rand()%3), NULL, &ad);

          if (!bLoad || bLoad == 2)
            co2_Edit_Set_Text(citem[0].p_edit, p_li->pwText[p_li->cSelected],
              1, HDC2DD, 0, 0);
        }
      }

      if (co2_List_Get_Dbclck(citem, CLIST_ITEMC, 0, &p_li) == 1) {
        float pos[3] = { 0, 0, 0 };
        ap_Play_Sound(0,1,0,pos,54 + (rand()%3), NULL, &ad);

        if (p_li->cSelected != -1)
          if (p_li->pwText[p_li->cSelected]) {
            if (!bLoad || bLoad == 2)
              wcscpy(cText, citem[0].p_edit->wtext);
            else
              wcscpy(cText, p_li->pwText[p_li->cSelected]);

            if (!bLoad || bLoad == 2)
              iReturn++;

            key[K_ESC] = 1;
          }
      }

      //dostala se mys do akcni oblasti (OnClick)?
      if (!p_Level->bRestart && !p_Level->bReturnToMenu)
        for (i = 0; i < lastcmd; i++)
          if (res[i].iParam[0] == COM_ONCLICK)
            if ((mi.x >= res[i].iParam[1]) &&
              (mi.x <= res[i].iParam[3]) &&
              (mi.y >= res[i].iParam[2]) && (mi.y <= res[i].iParam[4])) {
              if (res[i].iAnim[0][0] >= 0) {
                //pokud je animace, tak ji spust
                anmid = AddAnimation2(&res[i], &ad, 0);

                if (res[i + 1].iParam[0] == COM_BINDSOUND)
                  mPlaySound2(&res[i + 1], &ad, 1);

                resid = i;
                click = 1;
              }
              else {
                if (res[i + 1].iParam[0] == COM_BINDSOUND)
                  mPlaySound2(&res[i + 1], &ad, 1);

                resid = i;
                click = 1;
                anmid = 31;
              }
            }

      mi.t1 = 0;
    }

    //provedeni akce po animaci menu
    if (click)
      if (!anm[anmid].cmd) {
        click = 0;

        if (!strcmp(res[resid].cParam[1], "OK")) {
          if (!bLoad || bLoad == 2)
            wcscpy(cText, citem[0].p_edit->wtext);
          else if (citem[1].p_list->cClckSel != -1)
            if (citem[1].p_list->pwText[citem[1].p_list->cClckSel])
              wcscpy(cText,
                citem[1].p_list->pwText[citem[1].p_list->cClckSel]);

          if (wcslen(cText)) {
            char ccheck[] = "profiles";
            WCHAR wcheck[64];

            MultiByteToWideChar(CP_ACP, 0, ccheck, strlen(ccheck) + 1, wcheck,
              sizeof(wcheck) / sizeof(wcheck[0]));

            if (wcscasecmp(cText, wcheck)) {
              if (!bLoad || bLoad == 2)
                iReturn++;

              key[K_ESC] = 1;
            }
          }
        }

        if (!strcmp(res[resid].cParam[1], "EXIT") ||
          !strcmp(res[resid].cParam[1], "CANCEL")) {
          ZeroMemory(cText, 128 * sizeof(WCHAR));
          key[K_ESC] = 1;
          //break;
        }

        resid = -1;

        if (key[K_ESC]) {          
          for(i=0;i<lastcmd;i++)
            if(res[i].iParam[0] == COM_BINDSOUND && res[i].iParam[5] != -1)
             {
               adas_Release_Source(PARTICULAR_SOUND_SOURCE, UNDEFINED_VALUE, res[i].iParam[5]);
               res[i].iParam[5] = -1;
             }           
          //goto __QUIT;
        }
      }

    //pokud prisel cas, tak provedu nahodne animace (podle jejich pravdepodobnosti)
    if (timercnt > 500) {
      timercnt = 0;

      for (i = 0; i < lastcmd; i++)
        if (res[i].iParam[0] == COM_RANDOMANIMATION)
          if (rand() % 200 <= res[i].iParam[1] &&
            strcmp(dir, res[i].cParam[0])) {
            CheckAnimation2(&res[i], &ad);
            AddAnimation2(&res[i], &ad, 0);
          }
    }

    dwStop = timeGetTime();

    dwEplased += dwStop - dwStart;

    AnimationEvent2(NULL, 0, 0, dwStop);

    mi.x = mix;
    mi.y = miy;

    if (iReturn) {
      char ccheck[256];

      ZeroMemory(ccheck, 256);

      if (!bLoad)
        lsi_Save_Exist(cText, ccheck);
      else
        demo_Exist(cText, ccheck);

      if (strlen(ccheck)) {
        if (!bLoad)
          p_Level->bRestart = 1;
        else
          p_Level->bReturnToMenu = 1;

        p_Level->iMessageBoxReturn = -1;
        key[K_ESC] = 0;
        iReturn = 0;
      }
    }

    if ((p_Level->bRestart || p_Level->bReturnToMenu) && mi.t1) {
      if (!p_Level->iMessageBoxReturn) {
        iReturn = 0;
        p_Level->bRestart = 0;
        p_Level->bReturnToMenu = 0;
        p_Level->iMessageBoxReturn = -1;
      }
      else if (p_Level->iMessageBoxReturn == 1) {
        iReturn = 1;
        key[K_ESC] = 1;
        p_Level->bRestart = 0;
        p_Level->bReturnToMenu = 0;
        p_Level->iMessageBoxReturn = -1;
      }

      mi.t1 = 0;
    }

    am_Flip(p_Level, p_am, 0, &rline, CLIST_ITEMC, 0, 0, 0);
  }

//__QUIT:
  ddx2ReleaseBitmap(iCompositDC);
  ddx2ReleaseBitmap(iFontDC);

  fn2_Release_Font();
  StopAll2();
  co2_Handle_Release(citem, CLIST_ITEMC);
  co2_Release_Graphic();
  key[K_ESC] = 0;

  if (dh) {
    ddx2ReleaseBitmap(sh);
    ddx2DeviceDeaktivuj();
    ddx2DeviceRemove(dh);
    ddx2Release();
    dh = 0;
  }

  FreeAnimations2(res, RES_NUM);
  free((void *) res);

//      fn2_Set_Font("font3d.pak");
//      fn2_Load_Bitmaps();

  ddx2GameRender(TRUE);

  p_Level->bRestart = 0;
  return iReturn;
}

int LoadHelp(char *c_file)
{
  int c = 0, i = -1;
  char text[256];
  FILE *file;

  file = aopen(pBmpArchive, c_file, "r");

  if (!file)
    return -1;

  while (!aeof(file)) {
    memset(text, 0, 256);

    agets(text, 256, file);

    if (text[0]) {
      newline_cut(text);

      if (!c)
        i = ddx2LoadBitmap(text, pBmpArchive);
      else
        ddx2LoadBitmap(text, pBmpArchive);

      //kprintf(1, "%s - %d", text, t);

      c++;
    }
  }

  aclose(file);

  pBmpArchive->bError = 0;

  return i;
}

int DrawHelp(int iBmp, int iSurface, int *iHelpIndex)
{
  int y, ty;
  RECT rect = { 10, 0, 776, 1000 };
  int lid = GetPrivateProfileInt("game", "languageid", 0, ini_file);

  if (iSurface == -1)
    return 0;

  iHelpIndex[0] = 0;
  co2_Set_Text(iSurface, 10, 10, "##help_introduction_main", 1);
  y =
    co2_Set_Text_Formated_Rect(iSurface, 10, 60, "##help_introduction_h", 0,
    &rect) + 50;

  //BERUSKA/////////////////////////////////////////////////////////////////////////////////////////////////
  iHelpIndex[1] = y;
  co2_Set_Text(iSurface, 10, y, "##help_beatle_main", 1);
  y += 50;

  ddx2BitBlt(iSurface, 0, y, ddx2GetWidth(iBmp + 3), ddx2GetHeight(iBmp + 3),
    iBmp + 3, 0, 0);
  rect.left = 10 + ddx2GetWidth(iBmp + 3);

  y =
    co2_Set_Text_Formated_Rect(iSurface, rect.left, y, "##help_beatle_h", 0,
    &rect) + 50;

  //EXIT/////////////////////////////////////////////////////////////////////////////////////////////////
  iHelpIndex[2] = y;
  co2_Set_Text(iSurface, 10, y, "##help_exit_main", 1);
  y += 50;

  ddx2BitBlt(iSurface, 0, y, ddx2GetWidth(iBmp + 4), ddx2GetHeight(iBmp + 4),
    iBmp + 4, 0, 0);
  rect.left = 10 + ddx2GetWidth(iBmp + 4);

  co2_Set_Text_Formated_Rect(iSurface, rect.left, y, "##help_exit_h", 0,
    &rect);
  y += ddx2GetHeight(iBmp + 4) + 50;

  //ZED/////////////////////////////////////////////////////////////////////////////////////////////////
  iHelpIndex[3] = y;
  co2_Set_Text(iSurface, 10, y, "##help_wall_main", 1);
  y += 50;

  ddx2BitBlt(iSurface, 0, y, ddx2GetWidth(iBmp + 15),
    ddx2GetHeight(iBmp + 15), iBmp + 15, 0, 0);
  rect.left = 10 + ddx2GetWidth(iBmp + 15);

  co2_Set_Text_Formated_Rect(iSurface, rect.left, y, "##help_wall_h", 0,
    &rect);
  y += ddx2GetHeight(iBmp + 15) + 50;

  //SIKMINA/////////////////////////////////////////////////////////////////////////////////////////////////
  iHelpIndex[4] = y;
  co2_Set_Text(iSurface, 10, y, "##help_up_main", 1);
  y += 50;

  ddx2BitBlt(iSurface, 0, y, ddx2GetWidth(iBmp + 8), ddx2GetHeight(iBmp + 8),
    iBmp + 8, 0, 0);
  rect.left = 10 + ddx2GetWidth(iBmp + 8);

  co2_Set_Text_Formated_Rect(iSurface, rect.left, y, "##help_up_h", 0, &rect);
  y += ddx2GetHeight(iBmp + 8) + 50;

  //BEDNA LEHKA /////////////////////////////////////////////////////////////////////////////////////////////////
  iHelpIndex[5] = y;
  co2_Set_Text(iSurface, 10, y, "##help_empty_box_main", 1);
  y += 50;

  ddx2BitBlt(iSurface, 0, y, ddx2GetWidth(iBmp + 1), ddx2GetHeight(iBmp + 1),
    iBmp + 1, 0, 0);
  rect.left = 10 + ddx2GetWidth(iBmp + 1);

  co2_Set_Text_Formated_Rect(iSurface, rect.left, y, "##help_empty_box_h", 0,
    &rect);
  y += ddx2GetHeight(iBmp + 1) + 50;

  //BEDNA TEZKA /////////////////////////////////////////////////////////////////////////////////////////////////
  iHelpIndex[6] = y;
  co2_Set_Text(iSurface, 10, y, "##help_full_box_main", 1);
  y += 50;

  ddx2BitBlt(iSurface, 0, y, ddx2GetWidth(iBmp + 2), ddx2GetHeight(iBmp + 2),
    iBmp + 2, 0, 0);
  rect.left = 10 + ddx2GetWidth(iBmp + 2);

  co2_Set_Text_Formated_Rect(iSurface, rect.left, y, "##help_full_box_h", 0,
    &rect);
  y += ddx2GetHeight(iBmp + 2) + 50;

  //BEDNA TNT /////////////////////////////////////////////////////////////////////////////////////////////////
  iHelpIndex[7] = y;
  co2_Set_Text(iSurface, 10, y, "##help_tnt_box_main", 1);
  y += 50;

  ddx2BitBlt(iSurface, 0, y, ddx2GetWidth(iBmp + 11),
    ddx2GetHeight(iBmp + 11), iBmp + 11, 0, 0);
  rect.left = 10 + ddx2GetWidth(iBmp + 11);

  co2_Set_Text_Formated_Rect(iSurface, rect.left, y, "##help_tnt_box_h", 0,
    &rect);
  y += ddx2GetHeight(iBmp + 11) + 50;

  //KAMEN /////////////////////////////////////////////////////////////////////////////////////////////////
  iHelpIndex[8] = y;
  co2_Set_Text(iSurface, 10, y, "##help_rock_main", 1);
  y += 50;

  ddx2BitBlt(iSurface, 0, y, ddx2GetWidth(iBmp + 5), ddx2GetHeight(iBmp + 5),
    iBmp + 5, 0, 0);
  rect.left = 10 + ddx2GetWidth(iBmp + 5);

  co2_Set_Text_Formated_Rect(iSurface, rect.left, y, "##help_rock_h", 0,
    &rect);
  y += ddx2GetHeight(iBmp + 5) + 50;

  //Tlacitko /////////////////////////////////////////////////////////////////////////////////////////////////
  iHelpIndex[9] = y;
  co2_Set_Text(iSurface, 10, y, "##help_button_main", 1);
  y += 50;

  ddx2BitBlt(iSurface, 0, y, ddx2GetWidth(iBmp + 10),
    ddx2GetHeight(iBmp + 10), iBmp + 10, 0, 0);
  rect.left = 10 + ddx2GetWidth(iBmp + 10);

  co2_Set_Text_Formated_Rect(iSurface, rect.left, y, "##help_button_h", 0,
    &rect);
  y += ddx2GetHeight(iBmp + 10) + 50;

  //TELEPORT /////////////////////////////////////////////////////////////////////////////////////////////////
  iHelpIndex[10] = y;
  co2_Set_Text(iSurface, 10, y, "##help_teleport_main", 1);
  y += 50;

  ddx2BitBlt(iSurface, 0, y, ddx2GetWidth(iBmp + 9), ddx2GetHeight(iBmp + 9),
    iBmp + 9, 0, 0);

  rect.left = 10 + ddx2GetWidth(iBmp + 9);

  ty = y + ddx2GetHeight(iBmp + 9) + 50;

  y =
    co2_Set_Text_Formated_Rect(iSurface, rect.left, y, "##help_teleport_h", 0,
    &rect) + 50;

  //TELEPORT NA TLACITKO/////////////////////////////////////////////////////////////////////////////////////////////////
  iHelpIndex[11] = y;
  co2_Set_Text(iSurface, rect.left, y, "##help_teleport_button_main", 1);
  y += 50;

  y =
    co2_Set_Text_Formated_Rect(iSurface, rect.left, y,
    "##help_teleport_button_h", 0, &rect);

  y = ty;

  //VYTAH /////////////////////////////////////////////////////////////////////////////////////////////////
  iHelpIndex[12] = y;
  co2_Set_Text(iSurface, 10, y, "##help_lift_main", 1);
  y += 50;

  ddx2BitBlt(iSurface, 0, y, ddx2GetWidth(iBmp + 12),
    ddx2GetHeight(iBmp + 12), iBmp + 12, 0, 0);
  rect.left = 10 + ddx2GetWidth(iBmp + 12);

  co2_Set_Text_Formated_Rect(iSurface, rect.left, y, "##help_lift_h", 0,
    &rect);
  y += ddx2GetHeight(iBmp + 12) + 50;

  //VYTAH NA VAHU /////////////////////////////////////////////////////////////////////////////////////////////////
  iHelpIndex[13] = y;
  co2_Set_Text(iSurface, 10, y, "##help_lift_weight_main", 1);
  y += 50;

  ddx2BitBlt(iSurface, 0, y, ddx2GetWidth(iBmp + 17),
    ddx2GetHeight(iBmp + 17), iBmp + 17, 0, 0);
  rect.left = 10 + ddx2GetWidth(iBmp + 17);
  ty = y + ddx2GetHeight(iBmp + 17) + 50;

  y =
    co2_Set_Text_Formated_Rect(iSurface, rect.left, y, "##help_lift_weight_h",
    0, &rect);

  if (lid == 2 || lid == 4)
    y = ty;
  else
    y += 50;

  //VYTAH NA TLACITKO /////////////////////////////////////////////////////////////////////////////////////////////////
  iHelpIndex[14] = y;
  co2_Set_Text(iSurface, 10, y, "##help_lift_button_main", 1);
  y += 50;

  ddx2BitBlt(iSurface, 0, y, ddx2GetWidth(iBmp + 18),
    ddx2GetHeight(iBmp + 18), iBmp + 18, 0, 0);
  rect.left = 10 + ddx2GetWidth(iBmp + 18);

  ty = y + ddx2GetHeight(iBmp + 18) + 50;

  y =
    co2_Set_Text_Formated_Rect(iSurface, rect.left, y, "##help_lift_button_h",
    0, &rect);
  y = ty;

  //VODA /////////////////////////////////////////////////////////////////////////////////////////////////
  iHelpIndex[15] = y;
  co2_Set_Text(iSurface, 10, y, "##help_water_main", 1);
  y += 50;

  rect.left = 10;

  y =
    co2_Set_Text_Formated_Rect(iSurface, rect.left, y, "##help_water_h", 0,
    &rect);
  y += 50;

  //PROPADLO /////////////////////////////////////////////////////////////////////////////////////////////////
  iHelpIndex[16] = y;
  co2_Set_Text(iSurface, 10, y, "##help_trap_door_main", 1);
  y += 50;

  ddx2BitBlt(iSurface, 0, y, ddx2GetWidth(iBmp + 7), ddx2GetHeight(iBmp + 7),
    iBmp + 7, 0, 0);
  rect.left = 10 + ddx2GetWidth(iBmp + 7);

  co2_Set_Text_Formated_Rect(iSurface, rect.left, y, "##help_trap_door_h", 0,
    &rect);
  y += ddx2GetHeight(iBmp + 7) + 50;

  //SNORCHL /////////////////////////////////////////////////////////////////////////////////////////////////
  iHelpIndex[17] = y;
  co2_Set_Text(iSurface, 10, y, "##help_water_pipe_main", 1);
  y += 50;

  ddx2BitBlt(iSurface, 0, y, ddx2GetWidth(iBmp + 16),
    ddx2GetHeight(iBmp + 16), iBmp + 16, 0, 0);
  rect.left = 10 + ddx2GetWidth(iBmp + 16);

  co2_Set_Text_Formated_Rect(iSurface, rect.left, y, "##help_water_pipe_h", 0,
    &rect);
  y += ddx2GetHeight(iBmp + 16) + 50;

  //JIDLO /////////////////////////////////////////////////////////////////////////////////////////////////
  iHelpIndex[18] = y;
  co2_Set_Text(iSurface, 10, y, "##help_hormonal_vitamine_main", 1);
  y += 50;

  ddx2BitBlt(iSurface, 0, y, ddx2GetWidth(iBmp + 13),
    ddx2GetHeight(iBmp + 13), iBmp + 13, 0, 0);
  rect.left = 10 + ddx2GetWidth(iBmp + 13);

  co2_Set_Text_Formated_Rect(iSurface, rect.left, y,
    "##help_hormonal_vitamine_h", 0, &rect);
  y += ddx2GetHeight(iBmp + 13) + 50;

  //ZAVAZI /////////////////////////////////////////////////////////////////////////////////////////////////
  iHelpIndex[19] = y;
  co2_Set_Text(iSurface, 10, y, "##help_iron_main", 1);
  y += 50;

  ddx2BitBlt(iSurface, 0, y, ddx2GetWidth(iBmp + 14),
    ddx2GetHeight(iBmp + 14), iBmp + 14, 0, 0);
  rect.left = 10 + ddx2GetWidth(iBmp + 14);

  co2_Set_Text_Formated_Rect(iSurface, rect.left, y, "##help_iron_h", 0,
    &rect);
  y += ddx2GetHeight(iBmp + 14) + 50;

  //KROMPAC /////////////////////////////////////////////////////////////////////////////////////////////////
  iHelpIndex[20] = y;
  co2_Set_Text(iSurface, 10, y, "##help_rock_axe_main", 1);
  y += 50;

  ddx2BitBlt(iSurface, 0, y, ddx2GetWidth(iBmp + 6), ddx2GetHeight(iBmp + 6),
    iBmp + 6, 0, 0);
  rect.left = 10 + ddx2GetWidth(iBmp + 6);

  co2_Set_Text_Formated_Rect(iSurface, rect.left, y, "##help_rock_axe_h", 0,
    &rect);
  y += ddx2GetHeight(iBmp + 6) + 50;

  return 1;
}

void RunMenuHelp2(char *p_File_Name, AUDIO_DATA * p_ad, LEVELINFO * p_Level,
  ANIMATION_MODULE * p_am)
{
  int mix, miy;
  float s_factor[2];
  RECT rTMP = { 0, 0, 1024, 768 };

  DWORD dwEplased = 0, dwStart, dwStop;

  CONTROL_LIST_ITEM2 citem[CLIST_ITEMC];

//      FILE    *file;
  char dir[256];
  int lastcmd, lastanm, i;
  CMD_LINE *res = NULL;
  int lastabv = -1;
  char in, click = 0;
  int anmid = -1, resid = -1, anbind = -1;
  int bind;
  DeviceHandle dh = 0;
  int sh = K_CHYBA;
  int sh1 = K_CHYBA;
  int sh2 = K_CHYBA;
  int iBmp;
//  int iBook;
  int iHelpIndex[23];
  int iComboSel = 0;
  int iComboActSel = 0;

  _3d_End_Draw();

  ZeroMemory(iHelpIndex, 23 * sizeof(int));
  _2d_Clear_RectLine(&rline);

  p_Level->iCursor = 133;
  am_FlipA(p_Level, p_am, 1, &rline, CLIST_ITEMC, 0, 0, 0);
  //p_Level->iCursor = 0;

  s_factor[0] = (float) 1024 / (float) hwconf.xres;
  s_factor[1] = (float) 768 / (float) hwconf.yres;

  ddx2Init(200, RGB(255, 0, 255));
  dh = ddx2DeviceCreate(TRUE, 32);

  if (!dh)
    return;

  //fn2_Release_Font();
  ddx2DeviceSetActive(dh);

//      ddx2DeviceScreenAktivuj(0,0,hwconf.xres,hwconf.yres,1024,768);
//      ddx2DeviceBackBufferMapRec(0,0,1024,768);

  ddx2DeviceSetBackBufferSize(1024, 768);
  ddx2DeviceSetBackBufferRect(0, 0, 1024, 768);
  ddx2DeviceSetTextRenderRec(0, 0, 1024, 768);
  ddx2DeviceSetScreenRec(0, 0, hwconf.xres, hwconf.yres);

  //ddx2DeviceSetRender(TRUE);

  am_FlipA(p_Level, p_am, 1, &rline, CLIST_ITEMC, 0, 0, 0);

  ddx2CleareSurface(DDX2_BACK_BUFFER);
  sh = ddx2LoadBitmap("help_frame.bmp", pBmpArchive);
  sh1 = ddx2LoadBitmap("set_back2-1.bmp", pBmpArchive);
  sh2 = ddx2LoadBitmap("set_back1-1.bmp", pBmpArchive);

  ddx2FillRect(DDX2_BACK_BUFFER, &rTMP, RGB(255, 0, 255));
  ddx2SetRect(&rTMP, 1);

  // Stop renderu hry
  //ddx2GameRender(FALSE);

  am_FlipA(p_Level, p_am, 1, &rline, CLIST_ITEMC, 0, 0, 0);

  res = (CMD_LINE *) malloc(RES_NUM * sizeof(CMD_LINE));

  if (!res) {
    kprintf(1, "RunHelp2: Out of Memory");
    return;
  }
  else
    ZeroMemory(res, RES_NUM * sizeof(CMD_LINE));


  LoadHelp("helpbook.txt");
  iBmp = LoadHelp("helpload.txt");

  iCompositDC = ddx2FindFreeSurface();
  CompositDC = ddx2CreateSurface(1024, 768, iCompositDC);
  iFontDC = ddx2FindFreeSurface();
  FontDC = ddx2CreateSurface(1024, 768, iFontDC);

  am_FlipA(p_Level, p_am, 1, &rline, CLIST_ITEMC, 0, 0, 0);

  ZeroMemory(citem, CLIST_ITEMC * sizeof(CONTROL_LIST_ITEM2));

  fn2_Set_Font(cFontFile[2]);
  fn2_Load_Bitmaps();

  am_FlipA(p_Level, p_am, 1, &rline, CLIST_ITEMC, 0, 0, 0);

  for (bind = 0; bind < RES_NUM; bind++) {
    for (lastcmd = 0; lastcmd < 200; lastcmd++) {
      res[bind].iAnim[lastcmd][11] = -1;
      res[bind].iAnim[lastcmd][0] = -1;
    }

    for (in = 0; in < 6; in++)
      res[bind].iParam[(int)in] = -1;

    res[bind].iLayer = 0;
  }

  lastcmd = 0;
  timercnt = 0;

  GetPrivateProfileString("game", "data_dir", "c:\\", dir, 256, ini_file);
  working_file_translate(dir, 256);
  chdir((dir));

  //natadhe skript menu
  LoadMenuScript(p_File_Name, res, &lastcmd);

  am_FlipA(p_Level, p_am, 1, &rline, CLIST_ITEMC, 0, 0, 0);

  in = 0;

  //CreateFontAnimations2(res, &lastcmd);

  // privede prikazy, ketere se maji provest na zacatku a, kresleni, flip,
  // animace na OnAbove
  for (i = 0; i < lastcmd; i++) {
    lastanm = 0;

    switch (res[i].iParam[0]) {
      case COM_DRAW:
//                              if(!res[i].iLayer)
        ddx2DrawDisplay(res[i].iParam, 0);
/*				else
					ddx2DrawDisplayColorKey(res[i].iParam, 0, TRANSCOLOR );
					ddx2DrawSurface(FontDC, res[i].iParam, 3);*/
        break;
      case COM_RANDOMANIMATION:
      case COM_ONCLICK:
      case COM_ONABOVE:
      case COM_RUNANIMATION:
      case COM_BINDEXITANIMATION:
      case COM_BINDANIMATION:
        //nahrati animace k udalosti OnAbove
        LoadAnimationMenuScript(res, i, &lastanm);
        break;
    }
  }

  am_FlipA(p_Level, p_am, 1, &rline, CLIST_ITEMC, 0, 0, 0);

  if (!co2_Load_Graphic(1)) {
    kprintf(1, "co2_Load_Graphic ... con't load controls!");
    assert(0);
  }
  else {
    citem[0].p_combo = co2_Create_Combo(HDC2DD, 100, 67, 500, 0);
    citem[0].bActive = 1;

    co2_Combo_Add_StringWC(citem[0].p_combo, "##help_introduction_mainc");
    co2_Combo_Add_StringWC(citem[0].p_combo, "##help_beatle_mainc");
    co2_Combo_Add_StringWC(citem[0].p_combo, "##help_exit_mainc");
    co2_Combo_Add_StringWC(citem[0].p_combo, "##help_wall_mainc");
    co2_Combo_Add_StringWC(citem[0].p_combo, "##help_up_mainc");
    co2_Combo_Add_StringWC(citem[0].p_combo, "##help_empty_box_mainc");
    co2_Combo_Add_StringWC(citem[0].p_combo, "##help_full_box_mainc");
    co2_Combo_Add_StringWC(citem[0].p_combo, "##help_tnt_box_mainc");
    co2_Combo_Add_StringWC(citem[0].p_combo, "##help_rock_mainc");
    co2_Combo_Add_StringWC(citem[0].p_combo, "##help_button_mainc");
    co2_Combo_Add_StringWC(citem[0].p_combo, "##help_teleport_mainc");
    co2_Combo_Add_StringWC(citem[0].p_combo, "##help_teleport_button_mainc");
    co2_Combo_Add_StringWC(citem[0].p_combo, "##help_lift_mainc");
    co2_Combo_Add_StringWC(citem[0].p_combo, "##help_lift_weight_mainc");
    co2_Combo_Add_StringWC(citem[0].p_combo, "##help_lift_button_mainc");
    co2_Combo_Add_StringWC(citem[0].p_combo, "##help_water_mainc");
    co2_Combo_Add_StringWC(citem[0].p_combo, "##help_trap_door_mainc");
    co2_Combo_Add_StringWC(citem[0].p_combo, "##help_water_pipe_mainc");
    co2_Combo_Add_StringWC(citem[0].p_combo,
      "##help_hormonal_vitamine_mainc");
    co2_Combo_Add_StringWC(citem[0].p_combo, "##help_iron_mainc");
    co2_Combo_Add_StringWC(citem[0].p_combo, "##help_rock_axe_mainc");


    co2_Combo_Set_Params(citem[0].p_combo, 19);
    co2_Combo_Set_Sel(HDC2DD, citem[0].p_combo, 0);

    citem[1].p_list_v =
      co2_Create_List_View(HDC2DD, 100, 107, 828, 622, 0, 6800);
    citem[1].bActive = 1;

    am_FlipA(p_Level, p_am, 1, &rline, CLIST_ITEMC, 0, 0, 0);

    DrawHelp(iBmp, co2_Get_List_View_DC(citem[1].p_list_v), iHelpIndex);

    am_FlipA(p_Level, p_am, 1, &rline, CLIST_ITEMC, 0, 0, 0);

    co2_List_View_Redraw(HDC2DD, citem[1].p_list_v, 0);
  }

  for (i = 0; i < lastcmd; i++)
    if (res[i].iParam[0] == COM_RUNANIMATION) {
      int iWave = AddAnimation2(&res[i], &ad, 0);

      if (iWave != -1) {
        if (res[i + 1].iParam[0] == COM_BINDSOUND)
          anm[iWave].iWave = res[i + 1].iParam[5] =
            mPlaySound2(&res[i + 1], &ad, 2);
      }
    }

  mi.t1 = 0;
  mi.t2 = 0;
  mi.dx = 0;
  mi.dy = 0;
  anmid = -1;
  resid = -1;
  anbind = -1;
  bind = -1;
  lastabv = -1;
  in = 0;

  p_Level->iCursor = 0;
  ddx2GameRender(FALSE);
  ddx2DeviceSetRender(TRUE);

  spracuj_spravy(0);

  while (!key[K_ESC]) {
    mix = mi.x;
    miy = mi.y;

    mi.x = (int) ceil(mi.x * s_factor[0]);
    mi.y = (int) ceil(mi.y * s_factor[1]);

    dwStart = timeGetTime();

    //pohnul mysi
    if (mi.dx || mi.dy) {
      //dostala se mys do akcni oblasti (OnAbove)?
      if (!click)
        for (i = 0; i < lastcmd; i++)
          if (res[i].iParam[0] == COM_ONABOVE) {
            if ((mi.x >= res[i].iParam[1]) &&
              (mi.x <= res[i].iParam[3]) &&
              (mi.y >= res[i].iParam[2]) && (mi.y <= res[i].iParam[4])) {
              //spusteni animace v OnAbove
              if (i != lastabv) {
                if (in) {
                  Stop2(&res[lastabv]);

                  if (!res[lastabv].iLayer) {
                    ddx2DrawDisplay(res[lastabv].iAnim[0], 0);
                    ddx2DrawSurface(CompositDC, res[lastabv].iAnim[0], 3);
                  }
                  else {
                    ddx2DrawDisplayColorKey(res[lastabv].iAnim[0], 3,
                      TRANSCOLOR);
                    ddx2DrawSurface(FontDC, res[lastabv].iAnim[0], 3);
                  }
                }

                CheckAnimation2(&res[i], &ad);

                lastabv = i;
                AddAnimation2(&res[i], &ad, 0);
                in = 1;

                bind = ChooseBidedAnimation2(res, i + 1, &ad);

                if (bind != -1) {
                  CheckAnimation2(&res[bind], &ad);
                  AddAnimation2(&res[bind], &ad, 1);
                  anbind = bind;

                  mPlaySound2(&res[bind], &ad, 0);
                }

                strcpy(dir, res[i].cParam[1]);
              }
            }
            else if (lastabv == i) {
              // odesel z oblasti, ktera byla aktivni -> stop animace                                 
              // a odznaceni oblasti
              Stop2(&res[i]);

              if (!res[i].iLayer) {
                ddx2DrawDisplay(res[i].iAnim[0], 0);
                ddx2DrawSurface(CompositDC, res[i].iAnim[0], 3);
              }
              else {
                ddx2DrawDisplayColorKey(res[i].iAnim[0], 0, TRANSCOLOR);
                ddx2DrawSurface(FontDC, res[i].iAnim[0], 3);
              }

              bind = ChooseBidedExitAnimation2(res, i + 1, &ad);

              if (bind != -1) {
                int iAnim;

                if (anbind != -1) {
                  Stop2(&res[anbind]);

                  if (!res[i].iLayer) {
                    ddx2DrawDisplay(res[anbind].iAnim[0], 0);
                    ddx2DrawSurface(CompositDC, res[anbind].iAnim[0], 3);
                  }
                  else {
                    ddx2DrawDisplayColorKey(res[anbind].iAnim[0], 0,
                      TRANSCOLOR);
                    ddx2DrawSurface(FontDC, res[anbind].iAnim[0], 3);
                  }
                }

                iAnim = AddAnimation2(&res[bind], &ad, 1);

                if (iAnim != -1)
                  anm[iAnim].iWave = mPlaySound2(&res[bind], &ad, 2);
              }

              lastabv = -1;
              anbind = -1;
              in = 0;

              strcpy(dir, "");
            }
          }

      mi.dx = 0;
      mi.dy = 0;
    }

    co2_Handle_Controls(citem, CLIST_ITEMC, mi.x, mi.y, HDC2DD, 0, 0);

    iComboActSel = co2_Combo_Get_Sel_Not_Opend(citem, CLIST_ITEMC, 0);

    if (iComboActSel != iComboSel && iComboActSel != -1) {
      co2_Set_List_View_List_Pos(citem[1].p_list_v, iHelpIndex[iComboActSel],
        HDC2DD, 0, 0);
      iComboSel = iComboActSel;
    }

    //stlacil leve tlacitko
    if (mi.t1 && !click) {
      if (co2_Is_Button_Activated(citem, CLIST_ITEMC, 0)) {
        float pos[3] = { 0, 0, 0 };
        ap_Play_Sound(0,1,0,pos,54 + (rand()%3), NULL, &ad);
        key[K_ESC] = 1;
        p_Level->bResetini = 1;
      }

      //dostala se mys do akcni oblasti (OnClick)?
      for (i = 0; i < lastcmd; i++)
        if (res[i].iParam[0] == COM_ONCLICK)
          if ((mi.x >= res[i].iParam[1]) &&
            (mi.x <= res[i].iParam[3]) &&
            (mi.y >= res[i].iParam[2]) && (mi.y <= res[i].iParam[4])) {
            if (res[i].iAnim[0][0] >= 0) {
              //pokud je animace, tak ji spust
              anmid = AddAnimation2(&res[i], &ad, 0);

              if (res[i + 1].iParam[0] == COM_BINDSOUND)
                mPlaySound2(&res[i + 1], &ad, 1);

              resid = i;
              click = 1;
            }
            else {
              if (res[i + 1].iParam[0] == COM_BINDSOUND)
                mPlaySound2(&res[i + 1], &ad, 1);

              resid = i;
              click = 1;
              anmid = 31;
            }
          }

      mi.t1 = 0;
    }

    //provedeni akce po animaci menu
    if (click)
      if (!anm[anmid].cmd) {
        click = 0;

        //StopAll();

        if (!strcmp(res[resid].cParam[1], "EXIT")) {
          key[K_ESC] = 1;
          //break;
        }

        resid = -1;

        if (key[K_ESC]) {
          for(i=0;i<lastcmd;i++)
            if(res[i].iParam[0] == COM_BINDSOUND && res[i].iParam[5] != -1) {
              adas_Release_Source(PARTICULAR_SOUND_SOURCE, UNDEFINED_VALUE, res[i].iParam[5]);
              res[i].iParam[5] = -1;
           }
           goto __QUIT;
        }
      }

    //pokud prisel cas, tak provedu nahodne animace (podle jejich pravdepodobnosti)
    if (timercnt > 500) {
      timercnt = 0;

      for (i = 0; i < lastcmd; i++)
        if (res[i].iParam[0] == COM_RANDOMANIMATION)
          if (rand() % 200 <= res[i].iParam[1] &&
            strcmp(dir, res[i].cParam[0])) {
            CheckAnimation2(&res[i], &ad);
            AddAnimation2(&res[i], &ad, 0);
          }
    }

    dwStop = timeGetTime();

    dwEplased += dwStop - dwStart;

    AnimationEvent2(NULL, 0, 0, dwStop);

    mi.x = mix;
    mi.y = miy;

    am_Flip(p_Level, p_am, 1, &rline, CLIST_ITEMC, 0, 0, 0);
  }

__QUIT:
  ddx2ReleaseBitmap(iCompositDC);
  ddx2ReleaseBitmap(iFontDC);

  fn2_Release_Font();
  StopAll2();
  co2_Handle_Release(citem, CLIST_ITEMC);
  co2_Release_Graphic();
  key[K_ESC] = 0;

  if (dh) {
    ddx2ReleaseBitmap(sh);
    ddx2ReleaseBitmap(sh1);
    ddx2ReleaseBitmap(sh2);
    ddx2DeviceDeaktivuj();
    ddx2DeviceRemove(dh);
    ddx2Release();
    dh = 0;
  }

  FreeAnimations2(res, RES_NUM);
  free((void *) res);

//      fn2_Set_Font("font3d.pak");
//      fn2_Load_Bitmaps();

  // Start renderu hry
  ddx2GameRender(TRUE);
}

void GetSceneString(char *cscene)
{
  if (iActualScene < 0 || iActualScene > 12)
    strcpy(cscene, "##scene_name_unknown");
  else
    sprintf(cscene, "##scene_name_%d", iActualScene);

  return;
}

void GetLevelTime(char *ctime, LEVELINFO * p_Level)
{
  long iTime = (int) floor(p_Level->dwPlayTime / 1000.0f);
  int sec = iTime % 60;
  int min;
  int hour;
  char cmin[12];
  char csec[12];

  iTime = (int) floor(iTime / 60.0f);
  min = iTime % 60;

  iTime = (int) floor(iTime / 60.0f);
  hour = iTime % 60;

  if (sec < 10)
    sprintf(csec, "0%d", sec);
  else
    sprintf(csec, "%d", sec);

  if (min < 10)
    sprintf(cmin, "0%d", min);
  else
    sprintf(cmin, "%d", min);

  if (!min && !hour) {

    sprintf(ctime, "00:%d", sec);
    /*sprintf(ctime, "%d sekund", sec);

       if(sec < 2)
       strcat(ctime, "a");

       if(sec > 1 && sec < 5)
       strcat(ctime, "y"); */
  }
  else if (!hour)
    sprintf(ctime, "%s:%s", cmin, csec);
  else
    sprintf(ctime, "%d:%s:%s", hour, cmin, csec);
}

int LoadCList2(char *cFile)
{
  int c = 0, i = -1;
  char text[256];
  FILE *file;

  file = aopen(pBmpArchive, cFile, "r");

  if (!file)
    return -1;

  while (!aeof(file)) {
    memset(text, 0, 256);

    agets(text, 256, file);

    if (text[0]) {
      newline_cut(text);

      if (!c)
        i = ddx2LoadBitmap(text, pBmpArchive);
      else
        ddx2LoadBitmap(text, pBmpArchive);

      c++;
    }
  }

  aclose(file);

  pBmpArchive->bError = 0;

  return i;
}

void RunMenuLevelStats2(char *p_File_Name, AUDIO_DATA * p_ad,
  LEVELINFO * p_Level, ANIMATION_MODULE * p_am)
{
  int mix, miy;
  float s_factor[2];
  RECT rTMP = { 0, 0, 1024, 768 };

  DWORD dwEplased = 0, dwStart, dwStop;

  CONTROL_LIST_ITEM2 citem[CLIST_ITEMC];

  char dir[256];
  int lastcmd, lastanm, i;
  CMD_LINE *res = NULL;
  int lastabv = -1;
  char in, click = 0;
  int anmid = -1, resid = -1, anbind = -1;
  int bind;
  DeviceHandle dh = 0;
  int sh = K_CHYBA;
  int ifdx = 0;
//  int iBack;

  _2d_Clear_RectLine(&rline);

  _2d_Add_RectItem(&rline, rTMP, 0);

  s_factor[0] = (float) 1024 / (float) hwconf.xres;
  s_factor[1] = (float) 768 / (float) hwconf.yres;

  ddx2Init(200, RGB(255, 0, 255));
  dh = ddx2DeviceCreate(TRUE, 32);

  if (!dh)
    return;

  ddx2DeviceSetActive(dh);

  ddx2DeviceSetBackBufferSize(1024, 768);
  ddx2DeviceSetBackBufferRect(0, 0, 1024, 768);
  ddx2DeviceSetTextRenderRec(0, 0, 1024, 768);
  ddx2DeviceSetScreenRec(0, 0, hwconf.xres, hwconf.yres);

  ddx2DeviceSetRender(TRUE);

  ddx2CleareSurface(DDX2_BACK_BUFFER);


  switch (iActualScene) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 7:
      sh = ddx2LoadBitmap("end_screen3.bmp", pBmpArchive);
      ifdx = 2;
      break;
    case 5:
    case 8:
      sh = ddx2LoadBitmap("end_screen1.bmp", pBmpArchive);
      ifdx = 0;
      break;
    case 0:
    case 6:
    case 9:
    case 10:
    case 11:
      sh = ddx2LoadBitmap("end_screen2.bmp", pBmpArchive);
      ifdx = 1;
      break;
    case 12:
      sh = ddx2LoadBitmap("end_screen4.bmp", pBmpArchive);
      ifdx = 3;
      break;
    default:
      sh = ddx2LoadBitmap("end_screen4.bmp", pBmpArchive);
      ifdx = 3;
      break;
  }

  if (sh == -1) {
    kprintf(1, "RunMenuLevelStats2 ... sh == -1 !!!");
    ddx2DeviceDeaktivuj();
    ddx2DeviceRemove(dh);
    ddx2Release();
    ddx2GameRender(TRUE);
    return;
  }

  //ddx2FillRect(DDX2_BACK_BUFFER, &rTMP, RGB(54, 47, 45));
  ddx2SetRect(&rTMP, 1);

  // Stop renderu hry
  ddx2GameRender(FALSE);

  res = (CMD_LINE *) malloc(RES_NUM * sizeof(CMD_LINE));

  if (!res) {
    kprintf(1, "RunMenuLevelStats2: Out of Memory");
    return;
  }
  else
    ZeroMemory(res, RES_NUM * sizeof(CMD_LINE));

  iCompositDC = ddx2FindFreeSurface();
  CompositDC = ddx2CreateSurface(1024, 768, iCompositDC);
  iFontDC = ddx2FindFreeSurface();
  FontDC = ddx2CreateSurface(1024, 768, iFontDC);

  LoadCList2("load_bs.txt");

  ZeroMemory(citem, CLIST_ITEMC * sizeof(CONTROL_LIST_ITEM2));

  fn2_Set_Font(cFontFile[3]);
  fn2_Load_Bitmaps();

  for (bind = 0; bind < RES_NUM; bind++) {
    for (lastcmd = 0; lastcmd < 200; lastcmd++) {
      res[bind].iAnim[lastcmd][11] = -1;
      res[bind].iAnim[lastcmd][0] = -1;
    }

    for (in = 0; in < 6; in++)
      res[bind].iParam[(int)in] = -1;

    res[bind].iLayer = 0;
  }

  lastcmd = 0;
  timercnt = 0;

  GetPrivateProfileString("game", "data_dir", "c:\\", dir, 256, ini_file);
  working_file_translate(dir, 256);
  chdir((dir));

  //natadhe skript menu
  LoadMenuScript(p_File_Name, res, &lastcmd);

  in = 0;

  //CreateFontAnimations2(res, &lastcmd, 0, 0);

  // privede prikazy, ketere se maji provest na zacatku a, kresleni, flip,
  // animace na OnAbove
  for (i = 0; i < lastcmd; i++) {
    lastanm = 0;

    switch (res[i].iParam[0]) {
      case COM_DRAW:
        //if(!res[i].iLayer)
        ddx2DrawDisplay(res[i].iParam, 0);
        break;
      case COM_RANDOMANIMATION:
      case COM_ONCLICK:
      case COM_ONABOVE:
      case COM_RUNANIMATION:
      case COM_BINDEXITANIMATION:
      case COM_BINDANIMATION:
        //nahrati animace k udalosti OnAbove
        LoadAnimationMenuScript(res, i, &lastanm);
        break;
    }
  }

  co2_Set_Text(HDC2DD, 50, 50, "##stats_scene", ifdx);
  co2_Set_Text(HDC2DD, 50, 80, "##stats_time", ifdx);
  co2_Set_Text(HDC2DD, 50, 110, "##stats_steps", ifdx);

  GetSceneString(dir);
  co2_Set_Text(HDC2DD, 250, 50, dir, ifdx);
  GetLevelTime(dir, p_Level);
  co2_Set_TextWC(HDC2DD, 250, 80, dir, ifdx);

  co2_Set_TextWC(HDC2DD, 250, 110, itoa(p_Level->iNumOfSteps, dir, 10), ifdx);

  for (i = 0; i < lastcmd; i++)
    if (res[i].iParam[0] == COM_RUNANIMATION) {
      int iWave = AddAnimation2(&res[i], &ad, 0);

      if (iWave != -1) {
        if (res[i + 1].iParam[0] == COM_BINDSOUND)
          anm[iWave].iWave = res[i + 1].iParam[5] =
            mPlaySound2(&res[i + 1], &ad, 2);
      }
    }

  mi.t1 = 0;
  mi.t2 = 0;
  mi.dx = 0;
  mi.dy = 0;
  anmid = -1;
  resid = -1;
  anbind = -1;
  bind = -1;
  lastabv = -1;
  in = 0;

  spracuj_spravy(0);

  while (!key[K_ESC] && !key[K_ENTER]) {
    mix = mi.x;
    miy = mi.y;

    mi.x = (int) ceil(mi.x * s_factor[0]);
    mi.y = (int) ceil(mi.y * s_factor[1]);

    dwStart = timeGetTime();

    //pohnul mysi
    if (mi.dx || mi.dy) {
      //dostala se mys do akcni oblasti (OnAbove)?
      if (!click)
        for (i = 0; i < lastcmd; i++)
          if (res[i].iParam[0] == COM_ONABOVE) {
            if ((mi.x >= res[i].iParam[1]) &&
              (mi.x <= res[i].iParam[3]) &&
              (mi.y >= res[i].iParam[2]) && (mi.y <= res[i].iParam[4])) {
              //kprintf(1, "i = %d, lastabv = %d", i, lastabv);

              //spusteni animace v OnAbove
              if (i != lastabv) {
                //kprintf(1, "in = %d, %d, %d, %d = mi = %d, %d", res[i].iParam[1], res[i].iParam[2], res[i].iParam[3], res[i].iParam[4], mi.x, mi.y);
                if (in) {
                  Stop2(&res[lastabv]);

                  if (!res[lastabv].iLayer) {
                    ddx2DrawDisplay(res[lastabv].iAnim[0], 0);
                    ddx2DrawSurface(CompositDC, res[lastabv].iAnim[0], 3);
                  }
                  else {
                    ddx2DrawDisplayColorKey(res[lastabv].iAnim[0], 3,
                      TRANSCOLOR);
                    ddx2DrawSurface(FontDC, res[lastabv].iAnim[0], 3);
                  }
                }

                CheckAnimation2(&res[i], &ad);

                lastabv = i;
                AddAnimation2(&res[i], &ad, 0);
                in = 1;

                bind = ChooseBidedAnimation2(res, i + 1, &ad);

                if (bind != -1) {
                  CheckAnimation2(&res[bind], &ad);
                  AddAnimation2(&res[bind], &ad, 1);
                  anbind = bind;

                  mPlaySound2(&res[bind], &ad, 0);
                }

                strcpy(dir, res[i].cParam[1]);
              }
            }
            else if (lastabv == i) {
              //kprintf(1, "out  mi = %d, %d", mi.x, mi.y);

              // odesel z oblasti, ktera byla aktivni -> stop animace                                 
              // a odznaceni oblasti
              Stop2(&res[i]);

              if (!res[i].iLayer) {
                ddx2DrawDisplay(res[i].iAnim[0], 0);
                ddx2DrawSurface(CompositDC, res[i].iAnim[0], 3);
              }
              else {
                ddx2DrawDisplayColorKey(res[i].iAnim[0], 0, TRANSCOLOR);
                ddx2DrawSurface(FontDC, res[i].iAnim[0], 3);
              }

              bind = ChooseBidedExitAnimation2(res, i + 1, &ad);

              if (bind != -1) {
                int iAnim;

                if (anbind != -1) {
                  Stop2(&res[anbind]);

                  if (!res[i].iLayer) {
                    ddx2DrawDisplay(res[anbind].iAnim[0], 0);
                    ddx2DrawSurface(CompositDC, res[anbind].iAnim[0], 3);
                  }
                  else {
                    ddx2DrawDisplayColorKey(res[anbind].iAnim[0], 0,
                      TRANSCOLOR);
                    ddx2DrawSurface(FontDC, res[anbind].iAnim[0], 3);
                  }
                }

                iAnim = AddAnimation2(&res[bind], &ad, 1);

                if (iAnim != -1)
                  anm[iAnim].iWave = mPlaySound2(&res[bind], &ad, 2);
              }

              lastabv = -1;
              anbind = -1;
              in = 0;

              strcpy(dir, "");
            }
          }

      mi.dx = 0;
      mi.dy = 0;
    }

    //stlacil leve tlacitko
    if (mi.t1 && !click) {
      //dostala se mys do akcni oblasti (OnClick)?
      for (i = 0; i < lastcmd; i++)
        if (res[i].iParam[0] == COM_ONCLICK)
          if ((mi.x >= res[i].iParam[1]) &&
            (mi.x <= res[i].iParam[3]) &&
            (mi.y >= res[i].iParam[2]) && (mi.y <= res[i].iParam[4])) {
            if (res[i].iAnim[0][0] >= 0) {
              //pokud je animace, tak ji spust
              anmid = AddAnimation2(&res[i], &ad, 0);

              if (res[i + 1].iParam[0] == COM_BINDSOUND)
                mPlaySound2(&res[i + 1], &ad, 1);

              resid = i;
              click = 1;
            }
            else {
              if (res[i + 1].iParam[0] == COM_BINDSOUND)
                mPlaySound2(&res[i + 1], &ad, 1);

              resid = i;
              click = 1;
              anmid = 31;
            }
          }

      mi.t1 = 0;
    }

    //provedeni akce po animaci menu
    if (click)
      if (!anm[anmid].cmd) {
        click = 0;

        //StopAll();

        if (!strcmp(res[resid].cParam[1], "EXIT")) {
          key[K_ESC] = 1;
          //break;
        }

        resid = -1;

        if (key[K_ESC]) {
          goto __QUIT;
        }
      }

    //pokud prisel cas, tak provedu nahodne animace (podle jejich pravdepodobnosti)
    if (timercnt > 500) {
      timercnt = 0;

      for (i = 0; i < lastcmd; i++)
        if (res[i].iParam[0] == COM_RANDOMANIMATION)
          if (rand() % 200 <= res[i].iParam[1] &&
            strcmp(dir, res[i].cParam[0])) {
            CheckAnimation2(&res[i], &ad);
            AddAnimation2(&res[i], &ad, 0);
          }
    }

    dwStop = timeGetTime();

    dwEplased += dwStop - dwStart;

    AnimationEvent2(NULL, 0, 0, dwStop);

    mi.x = mix;
    mi.y = miy;

    am_Flip(p_Level, p_am, 1, &rline, CLIST_ITEMC, 0, 0, 0);
  }

__QUIT:
  ddx2ReleaseBitmap(iCompositDC);
  ddx2ReleaseBitmap(iFontDC);

  fn2_Release_Font();
  StopAll2();
  key[K_ESC] = 0;
  key[K_ENTER] = 0;

  if (dh) {
    ddx2ReleaseBitmap(sh);
    ddx2DeviceDeaktivuj();
    ddx2DeviceRemove(dh);
    ddx2Release();
    dh = 0;
  }

  FreeAnimations2(res, RES_NUM);
  free((void *) res);

//      fn2_Set_Font("font3d.pak");
//      fn2_Load_Bitmaps();

  // Start renderu hry
  ddx2GameRender(TRUE);
}

void RunMenuTutorial2(char *p_File_Name, AUDIO_DATA * p_ad,
  LEVELINFO * p_Level, ANIMATION_MODULE * p_am)
{
  int mix, miy;
  float s_factor[2], s_bfactor[2];
  RECT rTMP = { 0, 0, 1024, 768 };

  DWORD dwEplased = 0, dwStart, dwStop;

  CONTROL_LIST_ITEM2 citem[CLIST_ITEMC];

//      FILE    *file;
  char dir[256];
  int lastcmd, lastanm, i;
  CMD_LINE *res = NULL;
  int lastabv = -1;
  char in, click = 0;
  int anmid = -1, resid = -1, anbind = -1;
  int bind;
  DeviceHandle dh = 0;
  int ycorrection = 0;
  EDIT_TEXT ttext;
  //int sh = K_CHYBA;

  ZeroMemory(&ttext, sizeof(EDIT_TEXT));

  p_Level->iCursor = 133;
  am_FlipA(p_Level, p_am, 1, &rline, CLIST_ITEMC, 0, 0, 0);
  p_Level->iCursor = 0;

  txt_trida(TEXT_MENU);
  kom_set_default_text_config(0, 0, 1, 0, 0, 1);
  txt_nahraj_texturu_z_func(p3DMArchive, "tutor_frame.bmp", &ttext, 0, 1,
    NULL, bmp_nahraj);
  //txt_nahraj_texturu_z_func(p3DMArchive,"Hneda_a.bmp", &ttext, 0, 1, NULL,bmp_nahraj);
  kom_ret_default_text_config();

  _2d_Clear_RectLine(&rline);

  s_factor[0] = (float) 1024 / (float) hwconf.xres;
  s_factor[1] = (float) 768 / (float) hwconf.yres;

  s_bfactor[0] = (float) hwconf.xres / 1024.0f;
  s_bfactor[1] = (float) hwconf.yres / 768.0f;

  ddx2Init(200, RGB(255, 0, 255));
  dh = ddx2DeviceCreate(FALSE, 32);

  if (!dh)
    return;

  //fn2_Release_Font();
  ddx2DeviceSetActive(dh);

  ddx2DeviceSetBackBufferSize(1024, 768);
  ddx2DeviceSetBackBufferRect(10, 200, 900, 450);
  ddx2DeviceSetTextRenderRec(0, 0, 900, 450);
  ddx2DeviceSetScreenRec(ftoi(10 * s_bfactor[0]), ftoi(200 * s_bfactor[1]),
    ftoi(900 * s_bfactor[0]), ftoi(450 * s_bfactor[1]));
  //ddx2DeviceSetScreenRec(0, 0, hwconf.xres,hwconf.yres);

  ddx2DeviceSetRender(TRUE);

  ddx2CleareSurface(DDX2_BACK_BUFFER);
//      sh = ddx2LoadBitmap("brouk.bmp", pBmpArchive);

  //ddx2FillRect(DDX2_BACK_BUFFER, &rTMP, RGB(255, 0, 255));
  ddx2SetRect(&rTMP, 1);

  // Stop renderu hry
  ddx2GameRender(TRUE);

  res = (CMD_LINE *) malloc(RES_NUM * sizeof(CMD_LINE));

  if (!res) {
    kprintf(1, "RunMenuLevelStats2: Out of Memory");
    return;
  }
  else
    ZeroMemory(res, RES_NUM * sizeof(CMD_LINE));

  iCompositDC = ddx2FindFreeSurface();
  CompositDC = ddx2CreateSurface(1024, 768, iCompositDC);
  iFontDC = ddx2FindFreeSurface();
  FontDC = ddx2CreateSurface(1024, 768, iFontDC);

  ZeroMemory(citem, CLIST_ITEMC * sizeof(CONTROL_LIST_ITEM2));

  fn2_Set_Font(cFontFile[2]);
  fn2_Load_Bitmaps();

  for (bind = 0; bind < RES_NUM; bind++) {
    for (lastcmd = 0; lastcmd < 200; lastcmd++) {
      res[bind].iAnim[lastcmd][11] = -1;
      res[bind].iAnim[lastcmd][0] = -1;
    }

    for (in = 0; in < 6; in++)
      res[bind].iParam[(int)in] = -1;

    res[bind].iLayer = 0;
  }

  lastcmd = 0;
  timercnt = 0;

  GetPrivateProfileString("game", "data_dir", "c:\\", dir, 256, ini_file);
  working_file_translate(dir, 256);
  chdir((dir));

  //natadhe skript menu
  LoadMenuScript(p_File_Name, res, &lastcmd);

  //lastcmd--;
  in = 0;

  {
    int ycor = 0;
    char tt[256];
    RECT rr = { 20, 210, 890, 758 };

    sprintf(tt, "##tutorial_%d", iActualLevel - 200);

    ycor = co2_Set_Text_Formated_Rect(HDC2DD, 20, 210, tt, 0, &rr);
    //ycor = (200 + ycor + 50) - 600;
    ycor = (ycor + 50) - 600;
    _2d_Clear_RectLine(&rline);

    rr.left = 10;
    rr.top = 200;
    rr.right = 910;
    rr.bottom = 460;

    ddx2AddRectItem(&rline, rr, 0);

    ycorrection = CreateFontAnimations2(res, &lastcmd, ycor, 1);

    //ycor = ftoi((ycor - 25) * s_factor[1]);
    //ycor = ycor - 50;
    //ycorrection = 650 + ycor;
  }

  // privede prikazy, ketere se maji provest na zacatku a, kresleni, flip,
  // animace na OnAbove
  for (i = 0; i < lastcmd; i++) {
    lastanm = 0;

    switch (res[i].iParam[0]) {
      case COM_DRAW:
        if (!res[i].iLayer)
          ddx2DrawDisplay(res[i].iParam, 0);
        else
          ddx2DrawDisplayColorKey(res[i].iParam, 0, TRANSCOLOR);
        ddx2DrawSurface(FontDC, res[i].iParam, 3);
        break;
      case COM_RANDOMANIMATION:
      case COM_ONCLICK:
      case COM_ONABOVE:
      case COM_RUNANIMATION:
      case COM_BINDEXITANIMATION:
      case COM_BINDANIMATION:
        //nahrati animace k udalosti OnAbove
        LoadAnimationMenuScript(res, i, &lastanm);
        break;
    }
  }

  for (i = 0; i < lastcmd; i++)
    if (res[i].iParam[0] == COM_RUNANIMATION) {
      int iWave = AddAnimation2(&res[i], &ad, 0);

      if (iWave != -1) {
        if (res[i + 1].iParam[0] == COM_BINDSOUND)
          anm[iWave].iWave = res[i + 1].iParam[5] =
            mPlaySound2(&res[i + 1], &ad, 2);
      }
    }

  mi.t1 = 0;
  mi.t2 = 0;
  mi.dx = 0;
  mi.dy = 0;
  anmid = -1;
  resid = -1;
  anbind = -1;
  bind = -1;
  lastabv = -1;
  in = 0;

  spracuj_spravy(0);

  while (!key[K_ESC] && !key[K_ENTER]) {
    mix = mi.x;
    miy = mi.y;

    mi.x = (int) ceil(mi.x * s_factor[0]);
    mi.y = (int) ceil(mi.y * s_factor[1]);

    dwStart = timeGetTime();

    //pohnul mysi
    if (mi.dx || mi.dy) {
      //dostala se mys do akcni oblasti (OnAbove)?
      if (!click)
        for (i = 0; i < lastcmd; i++)
          if (res[i].iParam[0] == COM_ONABOVE) {
            if ((mi.x >= res[i].iParam[1]) &&
              (mi.x <= res[i].iParam[3]) &&
              (mi.y >= res[i].iParam[2]) && (mi.y <= res[i].iParam[4])) {
              //kprintf(1, "i = %d, lastabv = %d", i, lastabv);

              //spusteni animace v OnAbove
              if (i != lastabv) {
                //kprintf(1, "in = %d, %d, %d, %d = mi = %d, %d", res[i].iParam[1], res[i].iParam[2], res[i].iParam[3], res[i].iParam[4], mi.x, mi.y);
                if (in) {
                  Stop2(&res[lastabv]);

                  if (!res[lastabv].iLayer) {
                    ddx2DrawDisplay(res[lastabv].iAnim[0], 0);
                    ddx2DrawSurface(CompositDC, res[lastabv].iAnim[0], 3);
                  }
                  else {
                    ddx2DrawDisplayColorKey(res[lastabv].iAnim[0], 3,
                      TRANSCOLOR);
                    ddx2DrawSurface(FontDC, res[lastabv].iAnim[0], 3);
                  }
                }

                CheckAnimation2(&res[i], &ad);

                lastabv = i;
                AddAnimation2(&res[i], &ad, 0);
                in = 1;

                bind = ChooseBidedAnimation2(res, i + 1, &ad);

                if (bind != -1) {
                  CheckAnimation2(&res[bind], &ad);
                  AddAnimation2(&res[bind], &ad, 1);
                  anbind = bind;

                  mPlaySound2(&res[bind], &ad, 0);
                }

                strcpy(dir, res[i].cParam[1]);
              }
            }
            else if (lastabv == i) {
              //kprintf(1, "out  mi = %d, %d", mi.x, mi.y);

              // odesel z oblasti, ktera byla aktivni -> stop animace                                 
              // a odznaceni oblasti
              Stop2(&res[i]);

              if (!res[i].iLayer) {
                ddx2DrawDisplay(res[i].iAnim[0], 0);
                ddx2DrawSurface(CompositDC, res[i].iAnim[0], 3);
              }
              else {
                ddx2DrawDisplayColorKey(res[i].iAnim[0], 0, TRANSCOLOR);
                ddx2DrawSurface(FontDC, res[i].iAnim[0], 3);
              }

              bind = ChooseBidedExitAnimation2(res, i + 1, &ad);

              if (bind != -1) {
                int iAnim;

                if (anbind != -1) {
                  Stop2(&res[anbind]);

                  if (!res[i].iLayer) {
                    ddx2DrawDisplay(res[anbind].iAnim[0], 0);
                    ddx2DrawSurface(CompositDC, res[anbind].iAnim[0], 3);
                  }
                  else {
                    ddx2DrawDisplayColorKey(res[anbind].iAnim[0], 0,
                      TRANSCOLOR);
                    ddx2DrawSurface(FontDC, res[anbind].iAnim[0], 3);
                  }
                }

                iAnim = AddAnimation2(&res[bind], &ad, 1);

                if (iAnim != -1)
                  anm[iAnim].iWave = mPlaySound2(&res[bind], &ad, 2);
              }

              lastabv = -1;
              anbind = -1;
              in = 0;

              strcpy(dir, "");
            }
          }

      mi.dx = 0;
      mi.dy = 0;
    }

    //stlacil leve tlacitko
    if (mi.t1 && !click) {
      //dostala se mys do akcni oblasti (OnClick)?
      for (i = 0; i < lastcmd; i++)
        if (res[i].iParam[0] == COM_ONCLICK)
          if ((mi.x >= res[i].iParam[1]) &&
            (mi.x <= res[i].iParam[3]) &&
            (mi.y >= res[i].iParam[2]) && (mi.y <= res[i].iParam[4])) {
            if (res[i].iAnim[0][0] >= 0) {
              //pokud je animace, tak ji spust
              anmid = AddAnimation2(&res[i], &ad, 0);

              if (res[i + 1].iParam[0] == COM_BINDSOUND)
                mPlaySound2(&res[i + 1], &ad, 1);

              resid = i;
              click = 1;
            }
            else {
              if (res[i + 1].iParam[0] == COM_BINDSOUND)
                mPlaySound2(&res[i + 1], &ad, 1);

              resid = i;
              click = 1;
              anmid = 31;
            }
          }

      mi.t1 = 0;
    }

    //provedeni akce po animaci menu
    if (click)
      if (!anm[anmid].cmd) {
        click = 0;

        //StopAll();

        if (!strcmp(res[resid].cParam[1], "EXIT")) {
          key[K_ESC] = 1;
          //break;
        }

        resid = -1;

        if (key[K_ESC]) {
          for (i = 0; i < lastcmd; i++)
            if (res[i].iParam[0] == COM_BINDSOUND && res[i].iParam[5] != -1) {
              adas_Release_Source(PARTICULAR_SOUND_SOURCE, UNDEFINED_VALUE, res[i].iParam[5]);
              res[i].iParam[5] = -1;
            }
          goto __QUIT;
        }
      }

    //pokud prisel cas, tak provedu nahodne animace (podle jejich pravdepodobnosti)
    if (timercnt > 500) {
      timercnt = 0;

      for (i = 0; i < lastcmd; i++)
        if (res[i].iParam[0] == COM_RANDOMANIMATION)
          if (rand() % 200 <= res[i].iParam[1] &&
            strcmp(dir, res[i].cParam[0])) {
            CheckAnimation2(&res[i], &ad);
            AddAnimation2(&res[i], &ad, 0);
          }
    }

    dwStop = timeGetTime();

    dwEplased += dwStop - dwStart;

    AnimationEvent2(NULL, 0, 0, dwStop);

    mi.x = mix;
    mi.y = miy;

    am_Flip(p_Level, p_am, 0, &rline, CLIST_ITEMC, 1, ttext.text,
      ycorrection);
  }

__QUIT:
  ddx2ReleaseBitmap(iCompositDC);
  ddx2ReleaseBitmap(iFontDC);

  fn2_Release_Font();
  StopAll2();
  key[K_ESC] = 0;
  key[K_ENTER] = 0;

  if (dh) {
//              ddx2ReleaseBitmap(sh);    
    ddx2DeviceDeaktivuj();
    ddx2DeviceRemove(dh);
    ddx2Release();
    dh = 0;
  }

  FreeAnimations2(res, RES_NUM);
  free((void *) res);

//      fn2_Set_Font("font3d.pak");
//      fn2_Load_Bitmaps();

  // Start renderu hry
  ddx2GameRender(TRUE);

  txt_zrus_texturu(&ttext);
}

void RunMenuLoadScreen2(void)
{
  RECT r = { 0, 0, 891, 149 };
  int i = rand() % 49;
  char text[256];
  int sh, ty;

  sprintf(text, "##load_hint_%d", i);

  kprintf(1, "ddx2Init");

  ddx2Init(10, RGB(255, 0, 255));
  iLoadScreenDevice = ddx2DeviceCreate(TRUE, 32);

  if (!iLoadScreenDevice)
    return;

  ddx2DeviceSetActive(iLoadScreenDevice);

  ddx2DeviceSetBackBufferSize(1024, 768);
  ddx2DeviceSetBackBufferRect(0, 0, 1024, 768);
  ddx2DeviceSetTextRenderRec(0, 0, 1024, 768);
  ddx2DeviceSetScreenRec(0, 0, hwconf.xres, hwconf.yres);

  ddx2DeviceSetRender(TRUE);

  ddx2CleareSurface(DDX2_BACK_BUFFER);

  pBmpArchive->pActualNode = pBmpArchive->pRootNode->pNextNode;

  if (iActualScene < 13) {
    char text[256];
    APAK_HANDLE *hArchive = NULL;
    char cFile[MAX_PATH];
    char cDir[MAX_PATH];

    sprintf(text, "scene%d.bmp", iActualScene);

    if (iLanguageVersion == 4) {
      GetPrivateProfileString("game", "bitmap_dir", "c:\\", cDir, MAX_PATH,
        ini_file);
      working_file_translate(cDir, MAX_PATH);
      sprintf(cFile, "%s%cscene%d.pak", cDir, DIR_SLASH, iActualScene);
      hArchive = apakopen(cFile, cDir, &sh);

      if (!hArchive) {
        kprintf(1, "Nepodarilo se otevrit archiv [%s]\n", cFile);
        return;
      }
      else
        hArchive->pActualNode = hArchive->pRootNode->pNextNode;

      iLoadScreenBitmap = ddx2LoadBitmap(text, hArchive);
      apakclose(hArchive);
    }
    else
      iLoadScreenBitmap = ddx2LoadBitmap(text, pBmpArchive);
  }
  else
    iLoadScreenBitmap = ddx2LoadBitmap("LoadScreen.bmp", pBmpArchive);

  ddx2SetRect(&iLoadScreenRect, 1);

  ddx2GameRender(FALSE);

  ddx2BitBltDisplay(0, 0, 1024, 768, iLoadScreenBitmap, 0, 0);

  if (iActualScene < 13) {
    fn2_Set_Font(cFontFile[2]);
    fn2_Load_Bitmaps();

    sh = ddx2CreateSurface(891, 149, ddx2FindFreeSurface());

    if (sh != K_CHYBA) {
      ty = co2_Set_Text_Formated_Rect(sh, 0, 0, text, 0, &r);

      ddx2TransparentBltDisplay(83,
        33 + ftoi(((6 * b2_3d_font.iYPlus) - ty) / 2.0f), 891, 149, sh, 0, 0,
        891, 149, TRANSCOLOR);
      ddx2ReleaseBitmap(sh);
    }

    fn2_Release_Font();
  }

  ddx2RenderujVse(p_ber);
  flip();
}

void RunMenuLoadScreenRelease(int iCount)
{
  int i = GetPrivateProfileInt("hra", "numofbackbuffers", 2, ini_file);

  if (iLoadScreenDevice) {
    ddx2CleareSurfaceColor(DDX2_BACK_BUFFER, 0);

    for (i = 0; i < iCount; i++) {
      ddx2RenderujVse(p_ber);
      flip();
    }

    ddx2ReleaseBitmap(iLoadScreenBitmap);
    ddx2DeviceDeaktivuj();
    ddx2DeviceRemove(iLoadScreenDevice);
    ddx2Release();
    iLoadScreenDevice = 0;
  }

  ddx2GameRender(TRUE);
}

void RunMenuLoadScreenInitBar(int iNumOfItems)
{
  _2dd.ProgressStatus = 0;
  _2dd.ProgressPlus = 100.0f / (float) iNumOfItems;
}

void RunMenuLoadScreenAddProgress(float fPercent)
{
  if (fPercent >= 0)
    _2dd.ProgressStatus += fPercent;
  else
    _2dd.ProgressStatus += _2dd.ProgressPlus;
}

void RunMenuLoadScreenDrawProgress(int x, int y)
{
  RECT r = { 0, 753, 0, 768 };
  int Width;

  Width = (int) ftoi((1024 * _2dd.ProgressStatus) / 100.0f);

  r.right = Width;

  switch (iActualScene) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 7:
      ddx2FillRect(DDX2_BACK_BUFFER, &r, RGB(166, 200, 145));
      break;
    case 5:
    case 8:
      ddx2FillRect(DDX2_BACK_BUFFER, &r, RGB(224, 211, 188));
      break;
    case 0:
    case 10:
    case 11:
      ddx2FillRect(DDX2_BACK_BUFFER, &r, RGB(2, 25, 77));
      break;
    case 6:
    case 9:
      ddx2FillRect(DDX2_BACK_BUFFER, &r, RGB(170, 193, 245));
      break;
    case 12:
      ddx2FillRect(DDX2_BACK_BUFFER, &r, RGB(255, 196, 129));
      break;
    default:
      ddx2FillRect(DDX2_BACK_BUFFER, &r, RGB(255, 0, 0));
      break;
  }



  ddx2RenderujVse(p_ber);
  flip();
}
