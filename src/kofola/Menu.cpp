//------------------------------------------------------------------------------------------------
// 0.0.1
//------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "3d_all.h"
#include "Berusky3d_kofola_interface.h"
#include "game_logic.h"
#include "menu_script.h"
#include "Command.h"
#include "3D_graphic.h"
#include "2D_graphic.h"
#include "Menu.h"
#include "Comics.h"
#include "credits.h"
#include "font.h"
#include "controls.h"
#include "menu_def.h"
#include "Setup.h"
#include "profiles.h"
#include "Apak.h"
#include "3D_menus.h"
#include "Demo.h"
#include "load_level.h"
#include "Menu.h"
#include "Menu2.h"
#include "menu_def.h"
#include "Tools.h"

#ifdef LINUX
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <fnmatch.h>
#endif

#define HDC2DD		-1
#define RES_NUM 200

extern SETUP setup;
extern _3D_CURSOR _3dCur;
extern APAK_HANDLE *pBmpArchive;
extern APAK_HANDLE *pDataArchive;
extern PLAYER_PROFILE pPlayerProfile;
extern HINT_TEXTURE pHintTexture[26];
extern char cFontFile[5][64];
extern int bWindowMenu;
extern int iLanguageVersion;

typedef struct __2D_HINT
{
  int x;
  int y;
  int iSurface;
  int iBSurface;
  char bUsed;
} _2D_HINT;

typedef struct
{
  CMD_LINE *cmd;
  int iWave;
  char bStop;
} ANIMATION;

extern HW_KONFIG hwconf;

MOUSE_INFO dim;

static ANIMATION anm[32];
static DWORD timercnt = 0;
static DWORD timercntframe = 0;
static DWORD dwLTime;
static char cRestartMainMenu;
char cBrutalRestart = 0;
int CompositDC;
int FontDC;
int BackDC;

int iCompositDC, iFontDC, iBackDC;

static char bBackDC = 0;
int iActualScene = 0;
int iActualLevel = 0;
static char bNewScene = 0;
static _2D_HINT _2d_hint;

extern RECT_LINE rline;

//extern int gi_EnumDisplaySettings(DEVMODE *pdevmode);

void RunMenuSceneMap(char *p_File_Name, HWND hWnd, AUDIO_DATA * p_ad, int cpu,
  char *cSceneBmp, char *cSceneAnim, int iScene, int iLevelStart,
  int iNumOfLevels, char *cLevelList, int xTV, int yTV, char bLoadGame,
  char *cSaveFile, char bTutorial, int xBack, int yBack);

void RunMenuStartGame(char *p_File_Name, HWND hWnd, AUDIO_DATA * p_ad,
  int cpu);
int RunMenuComix(char *p_File_Name, HWND hWnd, AUDIO_DATA * p_ad, int iScene);
void DrawClock(int *iClock, int i);
int FillComboProfiles(COMBO_CONTROL * p_co, int *iSel);
int LoadClock(int *iClock);
void RunMenuCibron(char *cBmp);

CMD_LINE *GetCMD(CMD_LINE * cmp, CMD_LINE * pres)
{
  int i;

  for (i = 0; i < RES_NUM; i++)
    if (!strcmp(cmp->cParam[1], pres[i].cParam[0]))
      return &pres[i];

  return NULL;
}

int CheckScenePresence(int iScene)
{
  char t[256];

  sprintf(t, "scene%d", iScene);
  return GetPrivateProfileInt("Internet", t, 0, ini_file);
}

//------------------------------------------------------------------------------------------------
// zastavi vsechny animace
//------------------------------------------------------------------------------------------------
void StopAll(void)
{
  int i;

  for (i = 0; i < 32; i++)
    if (anm[i].cmd)
      anm[i].bStop = 1;
}

//------------------------------------------------------------------------------------------------
// zastavi animaci
//------------------------------------------------------------------------------------------------
void Stop(CMD_LINE * cmd)
{
  int i;

  if (cmd->iAnim[0][0] == -1)
    return;

  for (i = 0; i < 32; i++)
    if (cmd == anm[i].cmd) {
      if (cmd->bEndActivate[0] && cmd->pCmdLine) {
        cmd->pCmdLine[(int)cmd->bEndActivate[0]].bActive = 1;
        cmd->bActive = 0;
      }

      if (cmd->bEndActivate[1] && cmd->pCmdLine) {
        cmd->pCmdLine[(int)cmd->bEndActivate[1]].bActive = 1;
        cmd->bActive = 0;
      }

      anm[i].bStop = 1;
      return;
    }
}

int NextStep(CMD_LINE * cmd)
{
  if ((rand() % 100) < 60)
    return cmd->iAnim[cmd->iLastfrm][11];
  else
    return cmd->iAnim[cmd->iLastfrm][11 +
      rand() % (cmd->iAnim[cmd->iLastfrm][17])];
}

void DrawFrame(CMD_LINE * cmd, AUDIO_DATA * p_ad)
{
  if (cmd->iAnim[cmd->iLastfrm][10] == 2) {
    float p[3] = { 0, 0, 0 };

    //kprintf(1, "Play");
    if(karmin_aktivni)
      ap_Play_Sound(0, 1, 0, p, 147 + (rand()%2), NULL, p_ad);
  }

  if (!cmd->iLayer) {
    if (cmd->iAnim[cmd->iLastfrm][6] >= 0) {
      RECT r;

      ddxBitBlt(CompositDC,
        cmd->iAnim[cmd->iLastfrm][2] + cmd->iAnim[cmd->iLastfrm][6],
        cmd->iAnim[cmd->iLastfrm][3] + cmd->iAnim[cmd->iLastfrm][7],
        cmd->iAnim[cmd->iLastfrm][8], cmd->iAnim[cmd->iLastfrm][9],
        cmd->iAnim[cmd->iLastfrm][1],
        cmd->iAnim[cmd->iLastfrm][6], cmd->iAnim[cmd->iLastfrm][7]);

      r.left = cmd->iAnim[cmd->iLastfrm][2] + cmd->iAnim[cmd->iLastfrm][6];
      r.top = cmd->iAnim[cmd->iLastfrm][3] + cmd->iAnim[cmd->iLastfrm][7];
      r.right = cmd->iAnim[cmd->iLastfrm][8];
      r.bottom = cmd->iAnim[cmd->iLastfrm][9];

      ddxAddRectItem(&rline, r, 1);
    }
    else
      ddxDrawSurface(CompositDC, cmd->iAnim[cmd->iLastfrm], 1);

  }
  else if (cmd->iLayer == 10)
    ddxDrawSurfaceColorKey(CompositDC, cmd->iAnim[cmd->iLastfrm], 1,
      TRANSCOLOR);
  else
    ddxDrawSurface(FontDC, cmd->iAnim[cmd->iLastfrm], 3);
}

//------------------------------------------------------------------------------------------------
// provede animaci
//------------------------------------------------------------------------------------------------
void Animation(CMD_LINE * cmd, int time, AUDIO_DATA * p_ad)
{
ANIMATION_DRAW:

  while (time > 0) {
    if (cmd->iAnim[cmd->iLastfrm][5] == -1) {
      Stop(cmd);
      break;
    }

    time -= cmd->iAnim[cmd->iLastfrm][4];
    if (time > (cmd->iAnim[cmd->iAnim[cmd->iLastfrm][5]][4] / 2.0f)) {
      if (cmd->iAnim[cmd->iLastfrm][11] >= 0)
        cmd->iAnim[cmd->iLastfrm][5] = NextStep(cmd);

      if (cmd->iAnim[cmd->iLastfrm][10] > 0)
        DrawFrame(cmd, p_ad);

      cmd->iLastfrm = cmd->iAnim[cmd->iLastfrm][5];
    }
  }

  //----------------------------------------------------------------------
  DrawFrame(cmd, p_ad);

  if (!cmd->iAnim[cmd->iLastfrm][4]) {
    if (cmd->iAnim[cmd->iLastfrm][11] >= 0)
      cmd->iAnim[cmd->iLastfrm][5] = NextStep(cmd);

    cmd->iLastfrm = cmd->iAnim[cmd->iLastfrm][5];
    goto ANIMATION_DRAW;
  }

  if (cmd->iAnim[cmd->iLastfrm][5] == -1) {
    Stop(cmd);
    return;
  }

  if (cmd->iAnim[cmd->iLastfrm][11] >= 0)
    cmd->iAnim[cmd->iLastfrm][5] = NextStep(cmd);

  cmd->iLastfrm = cmd->iAnim[cmd->iLastfrm][5];
}

int RectCross(RECT_LINE * p_rl, int idx)
{
  int c = 0;
  int i;
  RECT *dr = &p_rl->rect[idx].rect;

  for (i = 0; i < rline.rlast; i++)
    if (i != idx)
      if (co_Rect_Hit(p_rl->rect[i].rect, dr->left, dr->top) ||
        co_Rect_Hit(p_rl->rect[i].rect, dr->right, dr->top) ||
        co_Rect_Hit(p_rl->rect[i].rect, dr->left, dr->bottom) ||
        co_Rect_Hit(p_rl->rect[i].rect, dr->right, dr->bottom))
        c++;

  return 0;
}

//------------------------------------------------------------------------------------------------
// provede animace
//------------------------------------------------------------------------------------------------
void AnimationEvent(DWORD dwTime, AUDIO_DATA * p_ad)
{
  DRAW_RECT *dr;
  int i;
  char bAnim = 0;
  DWORD e;

  e = abs(dwLTime - dwTime);

  for (i = 0; i < 32; i++)
    if (anm[i].cmd) {
      if (anm[i].bStop) {
        anm[i].cmd->iLastfrm = 1;
        anm[i].cmd->iCounter = 0;

        if (anm[i].iWave > -1) {
          //kprintf(1, "%d", anm[i].iWave);
          adas_Release_Source(PARTICULAR_SOUND_SOURCE, UNDEFINED_VALUE, anm[i].iWave);
        }

        anm[i].iWave = -1;
        anm[i].cmd = NULL;
      }
      else {
        anm[i].cmd->iCounter += e;
        if (anm[i].cmd->iCounter >=
          anm[i].cmd->iAnim[anm[i].cmd->iLastfrm][4]) {
          Animation(anm[i].cmd, anm[i].cmd->iCounter, p_ad);
          anm[i].cmd->iCounter = 0;
          bAnim = 1;
        }
      }
    }

  //prenos kreslenych dat;
  if (bAnim || rline.rlast) {
    if (bBackDC) {
      for (i = 0; i < rline.rlast; i++) {
        dr = &rline.rect[i];
        ddxTransparentBlt(BackDC, dr->rect.left, dr->rect.top, dr->rect.right,
          dr->rect.bottom, FontDC, dr->rect.left, dr->rect.top,
          dr->rect.right, dr->rect.bottom, TRANSCOLOR);
      }

      for (i = 0; i < rline.rlast; i++) {
        dr = &rline.rect[i];
        ddxTransparentBlt(CompositDC, dr->rect.left, dr->rect.top,
          dr->rect.right, dr->rect.bottom, BackDC, dr->rect.left,
          dr->rect.top, dr->rect.right, dr->rect.bottom, TRANSCOLOR);
      }
    }
    else {
      for (i = 0; i < rline.rlast; i++) {
        dr = &rline.rect[i];
        ddxTransparentBlt(CompositDC, dr->rect.left, dr->rect.top,
          dr->rect.right, dr->rect.bottom, FontDC, dr->rect.left,
          dr->rect.top, dr->rect.right, dr->rect.bottom, TRANSCOLOR);
      }
    }

    for (i = 0; i < rline.rlast; i++) {
      dr = &rline.rect[i];
      ddxTransparentBltDisplay(dr->rect.left, dr->rect.top, dr->rect.right,
        dr->rect.bottom, CompositDC, dr->rect.left, dr->rect.top,
        dr->rect.right, dr->rect.bottom, TRANSCOLOR);

    }

    ddxUpdateMouse();

    DisplayFrame();

    bAnim = 1;
  }

  timercntframe += e;

	if(!bAnim && (dim.dx || dim.dy) && timercntframe > 9)
	{
		timercntframe = 0;
		DisplayFrame();
	}

  dwLTime = dwTime;
  timercnt += e;

  _2d_Clear_RectLine(&rline);
}

void InitRandomJumps(CMD_LINE * cmd)
{
  int i;

  for (i = 0; i < 200; i++)
    if (cmd->iAnim[i][11] >= 0)
      cmd->iAnim[i][5] = cmd->iAnim[i][11 + rand() % (cmd->iAnim[i][17])];
}

int FindAnimation(CMD_LINE * cmd)
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
int AddAnimation(CMD_LINE * cmd, AUDIO_DATA * p_ad, char bOnlyOnes,
  char bButton)
{
  int i, r;
  float pos[3] = { 0.0f, 0.0f, 1.0f };

  if (!bButton && bWindowMenu)
    return -1;

  if (cmd->iAnim[0][0] == -1)
    return -1;

  for (i = 0; i < 32; i++)
    if (!anm[i].cmd) {
      if (bOnlyOnes && FindAnimation(cmd))
        return -1;

      InitRandomJumps(cmd);

      anm[i].cmd = cmd;
      anm[i].bStop = 0;
      anm[i].iWave = -1;
      Animation(anm[i].cmd, 0, p_ad);

      if (cmd->iParam[0] == COM_RANDOMANIMATION) {
        r = rand() % 3;

        if (cmd->iParam[r + 2] < 0) {
          if (cmd->iParam[2] >= 0) {
            adas_Get_Listener_Position(pos);  
            if(karmin_aktivni)
              ap_Play_Sound(0,1,0,pos,cmd->iParam[2], NULL, p_ad);
          }
        }
        else {
          adas_Get_Listener_Position(pos);
          if(karmin_aktivni)
            ap_Play_Sound(0,1,0,pos,cmd->iParam[r+2], NULL, p_ad);
        }
      }

      return i;
    }
    else if (!strcmp(cmd->cParam[1], anm[i].cmd->cParam[0]) && !anm[i].bStop)
      return -1;

  return -1;
}

int ChooseBidedExitAnimation(CMD_LINE * cmd, int Index, AUDIO_DATA * p_ad)
{
  int id[RES_NUM];
  int i, c = 0;

  for (i = Index; i < RES_NUM; i++)
    if (cmd[i].iParam[0] == COM_BINDEXITANIMATION) {
      id[c] = i;
      c++;

      if (c > RES_NUM - 1)
        break;
    }
    else if (cmd[i].iParam[0] != COM_BINDANIMATION)
      break;

  if (c)
    return id[rand() % c];
  else
    return -1;
}

int ChooseBidedAnimation(CMD_LINE * cmd, int Index, AUDIO_DATA * p_ad)
{
  int id[RES_NUM];
  int i, c = 0;

  for (i = Index; i < RES_NUM; i++)
    if (cmd[i].iParam[0] == COM_BINDANIMATION) {
      id[c] = i;
      c++;

      if (c > RES_NUM - 1)
        break;
    }
    else if (cmd[i].iParam[0] != COM_BINDEXITANIMATION)
      break;

  if (c)
    return id[rand() % c];
  else
    return -1;
}

void ZeroAnimations(void)
{
  int i;

  for (i = 0; i < 32; i++) {
    anm[i].bStop = 0;
    anm[i].iWave = -1;
    anm[i].cmd = NULL;
  }
}

void FreeAnimations(CMD_LINE * cmd, int csize)
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
void CheckAnimation(CMD_LINE * cmd, AUDIO_DATA * p_ad)
{
  int i;
//  float pos[3] = { 0.0f, 0.0f, 1.0f };

  for (i = 0; i < 32; i++)
    if (anm[i].cmd)
      if (!strcmp(cmd->cParam[1], anm[i].cmd->cParam[0])) {
        Stop(anm[i].cmd);

        /*if(!cmd->iLayer)
           {
           //menucommand_Draw(_2dd.hDC, anm[i].cmd->iAnim[0] , 0);
           ddxDrawDisplay(anm[i].cmd->iAnim[0] , 0);
           }
           else
           {
           //menucommand_DrawT(_2dd.hDC, anm[i].cmd->iAnim[0]);
           ddxDrawDisplayColorKey(anm[i].cmd->iAnim[0], 0, TRANSCOLOR);
           //menucommand_Draw(FontDC, anm[i].cmd->iAnim[0], 3);
           ddxDrawSurface(FontDC, anm[i].cmd->iAnim[0], 3);
           } */
        return;
      }
}

void StretchAnimation(RECT * rStart, RECT * rFinish, int iSurface, int iSpeed,
  AUDIO_DATA * p_ad)
{
  DWORD dwS, dwF, dwE;
  char done = 0;
  int max = 0;
  int i;
  int p[4];                     //{left, top, right, bottom}
  float s[4];

  RECT rBmp;
  RECT rDraw;

  p[0] = rFinish->left - rStart->left;
  p[1] = rFinish->top - rStart->top;
  p[2] = rFinish->right - rStart->right;
  p[3] = rFinish->bottom - rStart->bottom;

  for (i = 0; i < 4; i++)
    if (abs(p[i]) > max)
      max = abs(p[i]);

  for (i = 0; i < 4; i++)
    s[i] = p[i] / (float) max;

  StopAll();

  rBmp.left = 0;
  rBmp.top = 0;
  rBmp.right = ddxGetWidth(iSurface);
  rBmp.bottom = ddxGetHight(iSurface);

  memcpy(&rDraw, rStart, sizeof(RECT));

  dwS = timeGetTime();

  while (!done) {
    ddxStretchBltDisplay(&rDraw, iSurface, &rBmp);
    DisplayFrame();

    dwF = timeGetTime();

    dwE = dwF - dwS;

    rDraw.left = rStart->left + ftoi((s[0] * dwE) / (float) iSpeed);
    rDraw.top = rStart->top + ftoi((s[1] * dwE) / (float) iSpeed);
    rDraw.right = rStart->right + ftoi((s[2] * dwE) / (float) iSpeed);
    rDraw.bottom = rStart->bottom + ftoi((s[3] * dwE) / (float) iSpeed);

    //kprintf(1, "rDraw.left, rFinish->left: [%d, %d]", rDraw.left, rFinish->left);
    if (rDraw.left <= rFinish->left && p[0]) {
      rDraw.left = rFinish->left;
      done = 1;
    }

    //kprintf(1, "rDraw.top, rFinish->top: [%d, %d]", rDraw.top, rFinish->top);
    if (rDraw.top <= rFinish->top && p[1]) {
      rDraw.top = rFinish->top;
      done = 1;
    }

    //kprintf(1, "rDraw.right, rFinish->right: [%d, %d]", rDraw.right, rFinish->right);
    if (rDraw.right >= rFinish->right && p[2]) {
      rDraw.right = rFinish->right;
      done = 1;
    }

    //kprintf(1, "rDraw.bottom, rFinish->bottom: [%d, %d]", rDraw.bottom, rFinish->bottom);
    if (rDraw.bottom >= rFinish->bottom && p[3]) {
      rDraw.bottom = rFinish->bottom;
      done = 1;
    }

    spracuj_spravy(0);
    ddxUpdateMouse();

    /*if(rDraw.left <= rFinish->left &&
       rDraw.top  <= rFinish->top &&
       rDraw.bottom >= rFinish->bottom &&
       rDraw.right >= rFinish->right)
       done = 1; */

    if (ddxRestore(p_ad))
      return;
  }

  ddxStretchBltDisplay(rFinish, iSurface, &rBmp);
  DisplayFrame();
  ddxStretchBltDisplay(rFinish, iSurface, &rBmp);
}

//------------------------------------------------------------------------------------------------
// spusti nahodny zvuk podle zadani
//------------------------------------------------------------------------------------------------
int mPlaySound(CMD_LINE * cmd, AUDIO_DATA * p_ad, int type)
{
  float pos[3] = { 0.0f, 0.0f, 1.0f };
  int r = rand() % 4, ret = -1;

  if (cmd->iParam[r + 1] < 0) {
    if (cmd->iParam[1] >= 0) {
      adas_Get_Listener_Position(pos);
      if(karmin_aktivni)
        ret = ap_Play_Sound(type,1,0,pos,cmd->iParam[1], NULL, p_ad);
    }
  }
  else {
    adas_Get_Listener_Position(pos);
    if(karmin_aktivni)
      ret = ap_Play_Sound(type,1,0,pos,cmd->iParam[r+1], NULL, p_ad);
  }

  return ret;
}

void GetSceneParam(int iScene, int *iStartLevel, int *iNumOfLevels)
{
  switch (iScene) {
    case 1:
      *iStartLevel = 0;
      *iNumOfLevels = 11;
      return;
    case 2:
      *iStartLevel = 11;
      *iNumOfLevels = 10;
      return;
    case 3:
      *iStartLevel = 21;
      *iNumOfLevels = 12;
      return;
    case 4:
      *iStartLevel = 33;
      *iNumOfLevels = 10;
      return;
    case 5:
      *iStartLevel = 43;
      *iNumOfLevels = 10;
      return;
    case 6:
      *iStartLevel = 53;
      *iNumOfLevels = 10;
      return;
    case 7:
      *iStartLevel = 63;
      *iNumOfLevels = 10;
      return;
    case 8:
      *iStartLevel = 73;
      *iNumOfLevels = 12;
      return;
    case 9:
      *iStartLevel = 85;
      *iNumOfLevels = 10;
      return;
  }
}

int SceneDone(void)
{
  int i;
  int iStartLevel = 0;
  int iNumOfLevels = 0;

  GetSceneParam(iActualScene, &iStartLevel, &iNumOfLevels);

  for (i = iStartLevel; i < iStartLevel + iNumOfLevels; i++)
    if (!pPlayerProfile.cLevel[i])
      return 0;

  return 1;
}

//------------------------------------------------------------------------------------------------
// spusti level
//------------------------------------------------------------------------------------------------
int RunLevel(HWND hWnd, AUDIO_DATA * p_ad, int cpu, char *lvl, char *env)
{
  int ret;
  TIMER_ID Timer_ID;
  char cenv[64];
  float f = p_ad->Music_Gain;

  strcpy(cenv, env);

	if(ogg_playing())
	{
		while(f >= 0.05f)
		{
			f -= 0.05f;
			ogg_gain(f);
			Sleep(25);
		}

		ap_Stop_Song(p_ad);
	}

  //Sleep(1000);

  ddxRelease();
  FreeDirectDraw();

  ShowCursor(FALSE);

  {
    RunMenuLoadScreen2();
    RunMenuLoadScreenInitBar(15);
    RunMenuLoadScreenAddProgress(-1);
    RunMenuLoadScreenDrawProgress(-1, -1);

    _3d_Init();
    _3d_Load_List("3D_load.dat");

    _3d_Gen_Hints(pHintTexture, 26);

    RunMenuLoadScreenAddProgress(-1);

    _3d_Load_Indikace();

    Timer_ID = SetTimer(NULL, 0, 250, (TIMERPROC) gl_Set_Frame_Rate);

		adas_Release_Source(-1, ALL_TYPES, UNDEFINED_VALUE);
		adas_Release_Source(ALL_SOUND_SOURCES, ALL_TYPES,UNDEFINED_VALUE); 

    ret = gl_Run_Level(lvl, cenv, p_ad, cpu);

    if (ret == 1)
      pPlayerProfile.cLevel[iActualLevel] = 1;
    else if (ret == -1)
      RunMenuLoadScreenRelease(3);

    kprintf(1, "KillTimer");
    KillTimer(NULL, Timer_ID);

    kprintf(1, "_3d_Release_Hints");
    _3d_Release_Hints(pHintTexture, 26);

    kprintf(1, "_3d_Release");
    _3d_Release();
    spracuj_spravy(0);
    //maximalizuj_okno(NULL);
    ShowCursor(TRUE);
    spracuj_spravy(0);
  }
  
  ddxInit();
  spracuj_spravy(0);
  InitDirectDraw();
  spracuj_spravy(0);  
  ddxLoadList("2d_load.dat", 0);
  spracuj_spravy(0);
  iCompositDC = ddxFindFreeSurface();
  CompositDC = ddxCreateSurface(1024, 768, iCompositDC);
  iFontDC = ddxFindFreeSurface();
  FontDC = ddxCreateSurface(1024, 768, iFontDC);
  iBackDC = ddxFindFreeSurface();
  BackDC = ddxCreateSurface(1024, 768, iBackDC);
  spracuj_spravy(0);

  ZeroAnimations();

  bNewScene = 0;

#ifndef __DEMO
  //pokud se jedna o herni scenu 
  if (iActualScene > 0 && iActualScene < 10)
    if (SceneDone() && !pPlayerProfile.cMovie[iActualScene]) {
      PLAYER_PROFILE pProfile;

      pPlayerProfile.cMovie[iActualScene] = 1;
      pr_SaveProfile(&pPlayerProfile);

      if (iActualScene < 9) {
        memcpy(&pProfile, &pPlayerProfile, sizeof(PLAYER_PROFILE));
        pProfile.cScene[iActualScene + 1] = 1;
        pr_SaveProfile(&pProfile);
      }

      ogg_gain(p_ad->Music_Gain);
      RunMenuComix("menu_comix.txt", NULL, p_ad, iActualScene);

      if (iActualScene < 9)
        bNewScene = 1;

      if (iActualScene == 9) {
        cr_Credits(NULL, p_ad);
        cRestartMainMenu = 1;
        bNewScene = 1;
        iActualScene = 0;
      }
    }
    else {
      ap_Play_Song(0,0,p_ad);
      //adas_OGG_Set_Priority(cpu);
    }
  else {
    ap_Play_Song(0,0,p_ad);
    //adas_OGG_Set_Priority(cpu);
  }
#endif

#ifdef __DEMO
  ap_Play_Song(0,0,p_ad);
#endif

  cBrutalRestart = 1;
  return ret;
}

//------------------------------------------------------------------------------------------------
// spusti credity
//------------------------------------------------------------------------------------------------
void Credits(HWND hWnd, AUDIO_DATA * p_ad, int cpu)
{
/*
  char dir[256];

  StopAll();

	if(ogg_playing())
		ap_Stop_Song(p_ad);

  SetCursor(NULL);
  _2d_Release();

  GetPrivateProfileString("game", "data_dir", "c:\\", dir, 256, ini_file);
  working_file_translate(dir, 256);
  chdir((dir));


  cr_Credits(NULL, p_ad);

  _2d_Init();
  _2d_APAK_Load_List("2d_load.dat");

  ap_Play_Song(0,0,p_ad);
  //adas_OGG_Set_Priority(cpu);

  GetPrivateProfileString("game", "data_dir", "c:\\", dir, 256, ini_file);
  working_file_translate(dir, 256);
  chdir((dir));

  //SetCursor(LoadCursor(NULL, IDC_ARROW));
  spracuj_spravy(0);
  ShowCursor(TRUE);
  spracuj_spravy(0);
  */
}

void CreateFontAnimations(CMD_LINE * res, int *lastcmd)
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
    }

  y = r.top;

  for (i = 0; i < lcmd; i++)
    if (res[i].iParam[0] == COM_CREATEBUTTON)
      if (fn_Gen_Menu_Text(0, HDC2DD, res[i].cParam[0], &sidx1, &sidx2) != -1)
        if (sidx1 != -1 && sidx2 != -1) {
          int ii;
          int oy = res[i].iParam[2];

          //int x = r.left + ftoi((r.right - r.left - _2dd.bitmap[sidx1].bitmap.bmWidth) / (float)2.0f);
          int x =
            r.left + ftoi((r.right - r.left -
              ddxGetWidth(sidx1)) / (float) 2.0f);

          if (res[i].iParam[1] != -1) {
            oy = res[i].iParam[2];
            y = oy;
            x = res[i].iParam[1];
          }
          else {
            oy = res[i].iParam[2];
            y = oy;
          }

          y += ddxGetHight(sidx1);

          //OnAbove(16,661,100,748, quit_game.txt, NO_EXEPTION)
          sprintf(text, "OnAbove(%d,%d,%d,%d, NO_EXEPTION, NO_EXEPTION)", x,
            oy, x + ddxGetWidth(sidx1), y);
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
              ddxGetWidth(sidx2)) / (float) 2.0f);

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
            x + ddxGetWidth(sidx1), y, res[i].cParam[1]);

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
}

void SetTab(int iTab, int iLTab, CONTROL_LIST_ITEM * p_list, int lsize,
  int *hdcTab)
{
  int i;

  if (iLTab > -1) {
    //BitBlt(hdcTab[iLTab], 0, 0, TAB_XRES, TAB_YRES, _2dd.hDC, TAB_X, TAB_Y, SRCCOPY);
    ddxBitBlt(hdcTab[iLTab], 0, 0, TAB_XRES, TAB_YRES, HDC2DD, TAB_X, TAB_Y);
  }

  for (i = 0; i < lsize; i++)
    if ((p_list + i)->iTab == iTab && !(p_list + i)->bDisabled)
      (p_list + i)->bActive = 1;
    else if ((p_list + i)->iTab != -1)
      (p_list + i)->bActive = 0;

  //BitBlt(_2dd.hDC, TAB_X, TAB_Y, TAB_XRES, TAB_YRES, hdcTab[iTab], 0, 0, SRCCOPY);
  ddxBitBltDisplay(TAB_X, TAB_Y, TAB_XRES, TAB_YRES, hdcTab[iTab], 0, 0);
  DisplayFrame();
}

void EnumRozliseni(ROZLISENI ** res, int *roz_size)
{
  SDL_Rect** modes;
  int i;

  kprintf(1, "Screen resolutions:");

  modes = SDL_ListModes(NULL,SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_OPENGL|SDL_FULLSCREEN);

  if(!modes) {
    // No available resolutions?
    res = NULL;
    roz_size = 0;
    return;
  }

  if (modes == (SDL_Rect**)-1) {
    // Actually not very useful 
    res = NULL;
    roz_size = 0;
  }

  for(i = 0; modes[i]; i++)
	{
		kprintf(1, "%d x %d", modes[i]->w, modes[i]->h);
	}

	(*roz_size) = i;

	kprintf(1, "Pocet rozliseni = %d", i);

	(*res) = (ROZLISENI *)mmalloc((*roz_size) * sizeof(ROZLISENI));

	memset((*res), 0, (*roz_size) * sizeof(ROZLISENI));

  const SDL_VideoInfo *info = SDL_GetVideoInfo();
  int bpp = info->vfmt->BitsPerPixel;

  int j = 0;
  for(i = 0; modes[i]; i++)
	{
		if(modes[i]->w > 600)
		{
			(*res)[j].bpp = bpp;
			(*res)[j].x = modes[i]->w;
			(*res)[j].y = modes[i]->h;
			(*res)[j].freq = 0;
			j++;

			kprintf(1, "ComboAddItem (%d x %d x %d)", modes[i]->w, modes[i]->h, bpp);
		}
  }
}

void CharMenuCheckMultyKyes(LIST_VIEW_CONTROL * p_li, int iKey)
{
  int y;
  int i;
  RECT r;

  for (i = 0; i < p_li->listnum; i++)
    if (i != p_li->cSelected && p_li->piValue[i].iValue == iKey) {
      y = i * 30;

      r.left = 550;
      r.top = y + 2;
      r.right = r.left + (ddxGetWidth(p_li->bDCn) - 550);
      r.bottom = r.top + 28;

      ddxFillRect(p_li->bDCn, &r, 0);

      ddxFillRect(p_li->bDCs, &r, 0);

      char tmp[200];
      co_List_Add_String(p_li, i, 550, Key2String(255, tmp), 255, 0);
    }
}

void SetCharMenu(LIST_VIEW_CONTROL * p_li)
{
  char text[128];
  int y = p_li->cSelected * 30;
//  int xs = p_li->rectList.top + TAB_Y + y - p_li->dx;
  int xp = 0;
  int xt = 0;
  int i;
  int iCursor = 0;

  RECT r;

  if (!p_li->cSelected || p_li->cSelected == 11 || p_li->cSelected == 19)
    return;

  r.left = 550;
  r.top = y + 2;
  r.right = r.left + (ddxGetWidth(p_li->bDCn) - 550);
  r.bottom = r.top + 28;

  ddxFillRect(p_li->bDCn, &r, 0);

  ddxFillRect(p_li->bDCs, &r, 0);

  if (y - p_li->dx < 0)
    xp = (y - p_li->dx) * -1;

  if ((p_li->rectList.top + TAB_Y + y - p_li->dx + 30) >
    p_li->rectList.bottom + TAB_Y)
    xt =
      (p_li->rectList.top + TAB_Y + y - p_li->dx + 30) -
      (p_li->rectList.bottom + TAB_Y);

  ddxBitBlt(HDC2DD, p_li->rectList.left + TAB_X,
    p_li->rectList.top + TAB_Y + y - p_li->dx + xp, ddxGetWidth(p_li->bDCs),
    30 - xp - xt, p_li->bDCs, 0, y + xp);

  r.top = p_li->rectList.top + TAB_Y + y - p_li->dx + xp - 2;
  r.left = 665;
  r.right = r.left + 220;
  r.bottom = r.top + 27;

  DisplayFrame();

  while (!key[0]) {
    Sleep(10);
    spracuj_spravy(0);

    ddxUpdateMouse();

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

    if (iCursor < 100)
      ddxFillRectDisplay(&r, RGB(63, 122, 163));
    else if (iCursor > 99 && iCursor < 200)
      ddxFillRectDisplay(&r, 0);
    else {
      iCursor = 0;
      ddxFillRectDisplay(&r, 0);
    }

    iCursor += 4;

    //if(dim.dx || dim.dy)
    DisplayFrame();
  }

  key[0] = 0;

  for (i = 0; i < POCET_KLAVES; i++)
    if (key[i]) {
      Key2String(i,text);      
      key[i] = 0;
      break;
    }

  co_List_Add_String(p_li, p_li->cSelected, 550, text, i, 0);
  CharMenuCheckMultyKyes(p_li, i);

  ddxBitBlt(HDC2DD, p_li->rectList.left + TAB_X, p_li->rectList.top + TAB_Y,
    ddxGetWidth(p_li->bDCn), p_li->rectList.bottom - p_li->rectList.top,
    p_li->bDCn, 0, p_li->dx);

  ddxBitBlt(HDC2DD, p_li->rectList.left + TAB_X,
    p_li->rectList.top + TAB_Y + y - p_li->dx + xp, ddxGetWidth(p_li->bDCs),
    30 - xp - xt, p_li->bDCs, 0, y + xp);

  DisplayFrame();
}

void SetMenuSettings(ROZLISENI * roz, CONTROL_LIST_ITEM * citem,
  int *hdcTabUse)
{
  float f;
  int i;

  if (hdcTabUse[0]) {
    //setup.pohled_berusky = co_Check_Get_State(citem, CLIST_ITEMC, 0);
    setup.posouvat_kameru = co_Check_Get_State(citem, CLIST_ITEMC, 1);
    setup.ovladani = co_Check_Get_State(citem, CLIST_ITEMC, 2);
    //setup.menu_vram_load = co_Check_Get_State(citem, CLIST_ITEMC, 3);
    setup.start_zpruhlednovani = co_Check_Get_State(citem, CLIST_ITEMC, 10);
    setup.start_zvyraznovani = co_Check_Get_State(citem, CLIST_ITEMC, 11);
    setup.ovladani_rohy = co_Check_Get_State(citem, CLIST_ITEMC, 12);
    setup.camera_intro = co_Check_Get_State(citem, CLIST_ITEMC, 16);

    i = co_Check_Get_State(citem, CLIST_ITEMC, 13);

    if (i)
      setup.ovladani_rohy_default = 0;
    else
      setup.ovladani_rohy_default = 1;

    setup.ovladani_rohy_smer = co_Check_Get_State(citem, CLIST_ITEMC, 15);
    //setup.ovladani_pr_posun = co_Check_Get_State(citem, CLIST_ITEMC, 16);

    setup.ovladani_rohy_rychlost =
      co_Progres_Get(citem, CLIST_ITEMC, 0) / 10.0f;
    setup.p_kamera_radius = co_Progres_Get(citem, CLIST_ITEMC, 1) / 2.0f;
  }

  if (hdcTabUse[1]) {

    i = co_Combo_Get_Sel(citem, CLIST_ITEMC, 0);

    setup.xres = roz[i].x;
    setup.yres = roz[i].y;
    setup.bpp = (char) roz[i].bpp;

    setup.general_3d = co_Combo_Get_Sel(citem, CLIST_ITEMC, 1);

    i = co_Combo_Get_Sel(citem, CLIST_ITEMC, 2);

    if (!i)
      setup.extra_light_vertex = 1;
    else
      setup.extra_light_vertex = 0;

    setup.kvalita_casticv = co_Combo_Get_Sel(citem, CLIST_ITEMC, 5);

    i = co_Combo_Get_Sel(citem, CLIST_ITEMC, 6);

    if (i == 3)
      setup.light_dyn = 0;
    else {
      setup.light_dyn = 1;
      setup.light_rychlost = i + 1;
    }

    setup.text_detail2 = co_Combo_Get_Sel(citem, CLIST_ITEMC, 3);
    setup.text_detail3 = co_Combo_Get_Sel(citem, CLIST_ITEMC, 4);

    i = co_Combo_Get_Sel(citem, CLIST_ITEMC, 7);

    switch (i) {
      case 2:
        setup.bump_mapping = 0;
        break;
      case 1:
        setup.bump_mapping = 1;
        setup.text_bump2 = 1;
        setup.text_bump3 = 0;
        break;
      case 0:
        setup.bump_mapping = 1;
        setup.text_bump2 = 1;
        setup.text_bump3 = 1;
        break;
    }

    setup.text_mip_mapping = co_Combo_Get_Sel(citem, CLIST_ITEMC, 8);

    i = co_Combo_Get_Sel(citem, CLIST_ITEMC, 9);

    if (!i)
      setup.text_bpp = 32;
    else
      setup.text_bpp = 16;

    i = co_Combo_Drop_Get_Sel(citem, CLIST_ITEMC, 0, &f);

    if (!i)
      setup.text_ans = 0;
    else {
      setup.text_ans = 1;
      setup.text_ans_stupen = i;
    }

    setup.text_kvalita = co_Combo_Get_Sel(citem, CLIST_ITEMC, 10);

    setup.text_komprese = co_Check_Get_State(citem, CLIST_ITEMC, 6);
    setup.ditering = co_Check_Get_State(citem, CLIST_ITEMC, 7);
    setup.zrcado_aktivni = co_Check_Get_State(citem, CLIST_ITEMC, 4);
    setup.scene_materialy = co_Check_Get_State(citem, CLIST_ITEMC, 8);
    setup.vertex_arrays = co_Check_Get_State(citem, CLIST_ITEMC, 9);
    setup.menu_vram_load = co_Check_Get_State(citem, CLIST_ITEMC, 10);

    co_Combo_Drop_Get_Sel(citem, CLIST_ITEMC, 10, &setup.text_ostrost);
  }

  if (hdcTabUse[2]) {
    setup.soundvolume = co_Progres_Get(citem, CLIST_ITEMC, 4);
    setup.ambientvolume = co_Progres_Get(citem, CLIST_ITEMC, 5);
    setup.musicvolume = co_Progres_Get(citem, CLIST_ITEMC, 6);
  }

  if (hdcTabUse[3]) {
    setup.key[1] = co_List_Get_Value(citem, CLIST_ITEMC, 0, 1);
    setup.key[2] = co_List_Get_Value(citem, CLIST_ITEMC, 0, 2);
    setup.key[3] = co_List_Get_Value(citem, CLIST_ITEMC, 0, 3);
    setup.key[4] = co_List_Get_Value(citem, CLIST_ITEMC, 0, 4);
    setup.key[5] = co_List_Get_Value(citem, CLIST_ITEMC, 0, 5);
    setup.key[6] = co_List_Get_Value(citem, CLIST_ITEMC, 0, 6);
    setup.key[7] = co_List_Get_Value(citem, CLIST_ITEMC, 0, 7);
    setup.key[8] = co_List_Get_Value(citem, CLIST_ITEMC, 0, 8);
    setup.key[9] = co_List_Get_Value(citem, CLIST_ITEMC, 0, 9);
    setup.key[10] = co_List_Get_Value(citem, CLIST_ITEMC, 0, 10);

    setup.key[12] = co_List_Get_Value(citem, CLIST_ITEMC, 0, 12);
    setup.key[13] = co_List_Get_Value(citem, CLIST_ITEMC, 0, 13);
    setup.key[14] = co_List_Get_Value(citem, CLIST_ITEMC, 0, 14);
    setup.key[15] = co_List_Get_Value(citem, CLIST_ITEMC, 0, 15);
    setup.key[16] = co_List_Get_Value(citem, CLIST_ITEMC, 0, 16);
    setup.key[17] = co_List_Get_Value(citem, CLIST_ITEMC, 0, 17);
    setup.key[18] = co_List_Get_Value(citem, CLIST_ITEMC, 0, 18);

    setup.key[20] = co_List_Get_Value(citem, CLIST_ITEMC, 0, 20);
    setup.key[21] = co_List_Get_Value(citem, CLIST_ITEMC, 0, 21);
    setup.key[22] = co_List_Get_Value(citem, CLIST_ITEMC, 0, 22);
    setup.key[23] = co_List_Get_Value(citem, CLIST_ITEMC, 0, 23);
    setup.key[24] = co_List_Get_Value(citem, CLIST_ITEMC, 0, 24);
    setup.key[25] = co_List_Get_Value(citem, CLIST_ITEMC, 0, 25);
    setup.key[26] = co_List_Get_Value(citem, CLIST_ITEMC, 0, 26);
    setup.key[27] = co_List_Get_Value(citem, CLIST_ITEMC, 0, 27);
    setup.key[28] = co_List_Get_Value(citem, CLIST_ITEMC, 0, 28);
    setup.key[29] = co_List_Get_Value(citem, CLIST_ITEMC, 0, 29);
    setup.key[30] = co_List_Get_Value(citem, CLIST_ITEMC, 0, 30);
  }
}

void SetMenuSettingsS(ROZLISENI * roz, CONTROL_LIST_ITEM * citem,
  int *hdcTabUse)
{
  if (hdcTabUse[2]) {
    setup.soundvolume = co_Progres_Get(citem, CLIST_ITEMC, 4);
    setup.ambientvolume = co_Progres_Get(citem, CLIST_ITEMC, 5);
    setup.musicvolume = co_Progres_Get(citem, CLIST_ITEMC, 6);
  }
}

void InitTab3d(CONTROL_LIST_ITEM * citem, int *hdcTab, ROZLISENI * roz,
  int roz_size)
{
  int i;
  int count = 0;
  int iClock;

  iClock = ddxLoadBitmap("clock1-1.bmp", pBmpArchive);
  ddxResizeCursorBack(iClock);
  DrawClock(&iClock, 0);

  co_Set_Text_Right(hdcTab[1], "##settings_resolution", 0, 285, 57);

  citem[8].p_combo = co_Create_Combo(hdcTab[1], 300, 50, 450, 0);
  citem[8].iTab = 1;

  for (i = 0; i < roz_size; i++) {
    if (roz[i].bpp > 0) {
      char text[256];

      sprintf(text, "%dx%dx%d", roz[i].x, roz[i].y, roz[i].bpp);

      co_Combo_Add_String(citem[8].p_combo, text);

      count++;
    }
  }

  if (count > 10)
    co_Combo_Set_Params(citem[8].p_combo, 10);
  else
    co_Combo_Set_Params(citem[8].p_combo, count);

  co_Combo_Set_Sel(hdcTab[1], citem[8].p_combo, GetResCombobox(roz,
      roz_size));

  //////////////////////////////##settings_gemeral_3d
  co_Set_Text_Right(hdcTab[1], "##settings_gemeral_3d", 0, 285, 97);
  citem[9].p_combo = co_Create_Combo(hdcTab[1], 300, 90, 100, 1);
  citem[9].iTab = 1;

  co_Combo_Add_StringWC(citem[9].p_combo, "##settings_gemeral_3d_high");
  co_Combo_Add_StringWC(citem[9].p_combo, "##settings_gemeral_3d_normal");
  co_Combo_Add_StringWC(citem[9].p_combo, "##settings_gemeral_3d_low");
  co_Combo_Add_StringWC(citem[9].p_combo, "##settings_gemeral_3d_elow");
  co_Combo_Set_Params(citem[9].p_combo, 4);

  co_Combo_Set_Sel(hdcTab[1], citem[9].p_combo, setup.general_3d);

  /////////////////////////////////##settings_model
  co_Set_Text_Right(hdcTab[1], "##settings_model", 0, 285, 137);
  citem[33].p_combo = co_Create_Combo(hdcTab[1], 300, 130, 100, 2);
  citem[33].iTab = 1;
  co_Combo_Add_StringWC(citem[33].p_combo, "##settings_gemeral_3d_high");
  co_Combo_Add_StringWC(citem[33].p_combo, "##settings_gemeral_3d_normal");
  co_Combo_Set_Params(citem[33].p_combo, 2);

  if (!setup.extra_light_vertex)
    co_Combo_Set_Sel(hdcTab[1], citem[33].p_combo, 1);
  else
    co_Combo_Set_Sel(hdcTab[1], citem[33].p_combo, 0);

  co_Set_Text_Right(hdcTab[1], "##settings_partices_weather", 0, 285, 177);
  citem[10].p_combo = co_Create_Combo(hdcTab[1], 300, 170, 100, 5);
  citem[10].iTab = 1;

  co_Combo_Add_StringWC(citem[10].p_combo, "##settings_gemeral_3d_high");
  co_Combo_Add_StringWC(citem[10].p_combo, "##settings_gemeral_3d_normal");
  co_Combo_Add_StringWC(citem[10].p_combo, "##settings_gemeral_3d_low");
  co_Combo_Add_StringWC(citem[10].p_combo, "##settings_gemeral_3d_elow");
  co_Combo_Set_Params(citem[10].p_combo, 4);
  co_Combo_Set_Sel(hdcTab[1], citem[10].p_combo, setup.kvalita_casticv);

  co_Set_Text_Right(hdcTab[1], "#settings_lights", 0, 285, 217);
  citem[32].p_combo = co_Create_Combo(hdcTab[1], 300, 210, 130, 6);
  citem[32].iTab = 1;

  co_Combo_Add_StringWC(citem[32].p_combo, "##settings_gemeral_3d_high");
  co_Combo_Add_StringWC(citem[32].p_combo, "##settings_gemeral_3d_normal");
  co_Combo_Add_StringWC(citem[32].p_combo, "##settings_gemeral_3d_low");
  //co_Combo_Add_StringWC(citem[32].p_combo, "##settings_gemeral_3d_elow");
  co_Combo_Add_StringWC(citem[32].p_combo, "##settings_lights_off");
  co_Combo_Set_Params(citem[32].p_combo, 4);

  if (!setup.light_dyn)
    co_Combo_Set_Sel(hdcTab[1], citem[32].p_combo, 3);
  else
    co_Combo_Set_Sel(hdcTab[1], citem[32].p_combo, setup.light_rychlost - 1);


  /////////////////////////////////##settings_item_textures
  co_Set_Text_Right(hdcTab[1], "##settings_item_textures", 0, 285, 257);
  citem[34].p_combo = co_Create_Combo(hdcTab[1], 300, 250, 100, 3);
  citem[34].iTab = 1;

  co_Combo_Add_StringWC(citem[34].p_combo, "##settings_gemeral_3d_high");
  co_Combo_Add_StringWC(citem[34].p_combo, "##settings_gemeral_3d_normal");
  co_Combo_Add_StringWC(citem[34].p_combo, "##settings_gemeral_3d_low");
  co_Combo_Set_Params(citem[34].p_combo, 3);
  co_Combo_Set_Sel(hdcTab[1], citem[34].p_combo, setup.text_detail2);

  /////////////////////////////////##settings_scene_textures
  co_Set_Text_Right(hdcTab[1], "##settings_scene_textures", 0, 285, 297);
  citem[35].p_combo = co_Create_Combo(hdcTab[1], 300, 290, 100, 4);
  citem[35].iTab = 1;

  co_Combo_Add_StringWC(citem[35].p_combo, "##settings_gemeral_3d_high");
  co_Combo_Add_StringWC(citem[35].p_combo, "##settings_gemeral_3d_normal");
  co_Combo_Add_StringWC(citem[35].p_combo, "##settings_gemeral_3d_low");
  co_Combo_Set_Params(citem[35].p_combo, 3);
  co_Combo_Set_Sel(hdcTab[1], citem[35].p_combo, setup.text_detail3);

  co_Set_Text_Right(hdcTab[1], "##settings_bump", 0, 285, 337);
  citem[25].p_combo = co_Create_Combo(hdcTab[1], 300, 330, 100, 7);
  citem[25].iTab = 1;

  co_Combo_Add_StringWC(citem[25].p_combo, "##settings_combo_all");
  co_Combo_Add_StringWC(citem[25].p_combo, "##settings_combo_items");
  co_Combo_Add_StringWC(citem[25].p_combo, "##settings_combo_turnoff");
  co_Combo_Set_Params(citem[25].p_combo, 3);

  if (!setup.bump_mapping)
    co_Combo_Set_Sel(hdcTab[1], citem[25].p_combo, 2);
  else if (setup.bump_mapping && setup.text_bump2 && !setup.text_bump3)
    co_Combo_Set_Sel(hdcTab[1], citem[25].p_combo, 1);
  else
    co_Combo_Set_Sel(hdcTab[1], citem[25].p_combo, 0);

  co_Set_Text_Right(hdcTab[1], "##settings_texture_filterig", 0, 285, 377);
  citem[18].p_combo = co_Create_Combo(hdcTab[1], 300, 370, 100, 8);
  citem[18].iTab = 1;

  co_Combo_Add_StringWC(citem[18].p_combo,
    "##settings_texture_filterig_linear");
  co_Combo_Add_StringWC(citem[18].p_combo, "##settings_texture_mip_mapping");
  co_Combo_Set_Params(citem[18].p_combo, 2);

  co_Combo_Set_Sel(hdcTab[1], citem[18].p_combo, setup.text_mip_mapping);

  co_Set_Text_Right(hdcTab[1], "##settings_texture_depth", 0, 285, 417);
  citem[26].p_combo = co_Create_Combo(hdcTab[1], 300, 410, 100, 9);
  citem[26].iTab = 1;

  co_Combo_Add_StringWC(citem[26].p_combo, "##settings_texture_depth_32");
  co_Combo_Add_StringWC(citem[26].p_combo, "##settings_texture_depth_16");
  co_Combo_Set_Params(citem[26].p_combo, 2);

  if (setup.text_bpp == 32)
    co_Combo_Set_Sel(hdcTab[1], citem[26].p_combo, 0);
  else
    co_Combo_Set_Sel(hdcTab[1], citem[26].p_combo, 1);

  co_Set_Text_Right(hdcTab[1], "##settings_anisotropic_filtering", 0, 285,
    457);
  citem[27].p_combod = co_Create_Combo_Drop(hdcTab[1], 300, 450, 0);
  citem[27].iTab = 1;

  co_Combo_Drop_Add_StringWC(citem[27].p_combod, "##settings_lights_turnoff",
    0.0f);
  co_Combo_Drop_Add_String(citem[27].p_combod, "2", 2.0f);
  co_Combo_Drop_Add_String(citem[27].p_combod, "4", 4.0f);
  co_Combo_Drop_Add_String(citem[27].p_combod, "8", 8.0f);
  co_Combo_Drop_Add_String(citem[27].p_combod, "16", 16.0f);
  co_Combo_Drop_Add_String(citem[27].p_combod, "32", 32.0f);

  if (!setup.text_ans)
    co_Combo_Drop_Set_Sel(hdcTab[1], citem[27].p_combod, 0);
  else
    co_Combo_Drop_Set_Sel(hdcTab[1], citem[27].p_combod,
      setup.text_ans_stupen);


  co_Set_Text_Right(hdcTab[1], "##settings_sharpness", 0, 285, 497);
  citem[42].p_combod = co_Create_Combo_Drop(hdcTab[1], 300, 490, 10);
  citem[42].iTab = 1;

  co_Combo_Drop_Add_String(citem[42].p_combod, "-3.0", -3.0f);
  co_Combo_Drop_Add_String(citem[42].p_combod, "-2.75", -2.75f);
  co_Combo_Drop_Add_String(citem[42].p_combod, "-2.5", -2.5f);
  co_Combo_Drop_Add_String(citem[42].p_combod, "-2.25", -2.25f);
  co_Combo_Drop_Add_String(citem[42].p_combod, "-2.0", -2.0f);
  co_Combo_Drop_Add_String(citem[42].p_combod, "-1.75", -1.75f);
  co_Combo_Drop_Add_String(citem[42].p_combod, "-1.5", -1.5f);
  co_Combo_Drop_Add_String(citem[42].p_combod, "-1.25", -1.25f);
  co_Combo_Drop_Add_String(citem[42].p_combod, "-1.0", -1.0f);
  co_Combo_Drop_Add_String(citem[42].p_combod, "-0.75", -0.75f);
  co_Combo_Drop_Add_String(citem[42].p_combod, "-0.5", -0.5f);
  co_Combo_Drop_Add_String(citem[42].p_combod, "-0.25", -0.25f);
  co_Combo_Drop_Add_String(citem[42].p_combod, "0.0", 0.0f);
  co_Combo_Drop_Add_String(citem[42].p_combod, "+0.25", 0.25f);
  co_Combo_Drop_Add_String(citem[42].p_combod, "+0.5", 0.5f);
  co_Combo_Drop_Add_String(citem[42].p_combod, "+0.75", 0.75f);
  co_Combo_Drop_Add_String(citem[42].p_combod, "+1.0", 1.0f);
  co_Combo_Drop_Add_String(citem[42].p_combod, "+1.25", 1.25f);
  co_Combo_Drop_Add_String(citem[42].p_combod, "+1.5", 1.5f);
  co_Combo_Drop_Add_String(citem[42].p_combod, "+1.75", 1.75f);
  co_Combo_Drop_Add_String(citem[42].p_combod, "+2.0", 2.0f);
  co_Combo_Drop_Add_String(citem[42].p_combod, "+2.25", 2.25f);
  co_Combo_Drop_Add_String(citem[42].p_combod, "+2.5", 2.5f);
  co_Combo_Drop_Add_String(citem[42].p_combod, "+2.75", 2.75f);
  co_Combo_Drop_Add_String(citem[42].p_combod, "+3.0", 3.0f);

  i = ftoi(((setup.text_ostrost + 3) * 100) / 25.0f);

  if (i > 24)
    i = 24;

  co_Combo_Drop_Set_Sel(hdcTab[1], citem[42].p_combod, i);

  co_Set_Text_Right(hdcTab[1], "##settings_opengl", 0, 285, 537);
  citem[28].p_combo = co_Create_Combo(hdcTab[1], 300, 530, 100, 10);
  citem[28].iTab = 1;

  co_Combo_Add_StringWC(citem[28].p_combo, "##settings_opengl_rychlost");
  co_Combo_Add_StringWC(citem[28].p_combo, "##settings_opengl_kvalita");
  co_Combo_Set_Params(citem[28].p_combo, 2);
  co_Combo_Set_Sel(hdcTab[1], citem[28].p_combo, setup.text_kvalita);

  citem[16].p_check =
    co_Create_CheckBox(hdcTab[1], 525, 255, "##settings_texture_commpression",
    0, 6);
  citem[16].iTab = 1;
  co_Check_Set_State(citem[16].p_check, hdcTab[1], setup.text_komprese, 1);

  citem[29].p_check =
    co_Create_CheckBox(hdcTab[1], 525, 415, "##settings_dithering", 0, 7);
  citem[29].iTab = 1;
  co_Check_Set_State(citem[29].p_check, hdcTab[1], setup.ditering, 1);

  citem[11].p_check =
    co_Create_CheckBox(hdcTab[1], 525, 135, "##settings_draw_mirror", 0, 4);
  citem[11].iTab = 1;
  co_Check_Set_State(citem[11].p_check, hdcTab[1], setup.zrcado_aktivni, 1);

  citem[30].p_check =
    co_Create_CheckBox(hdcTab[1], 525, 535, "##settings_caustic", 0, 8);
  citem[30].iTab = 1;
  co_Check_Set_State(citem[30].p_check, hdcTab[1], setup.scene_materialy, 1);

  citem[31].p_check =
    co_Create_CheckBox(hdcTab[1], 300, 580, "##settings_vertex", 0, 9);
  citem[31].iTab = 1;
  co_Check_Set_State(citem[31].p_check, hdcTab[1], setup.vertex_arrays, 1);

  citem[44].p_check =
    co_Create_CheckBox(hdcTab[1], 300, 610, "##settings_load_menu_vram", 0,
    10);
  citem[44].iTab = 1;
  co_Check_Set_State(citem[44].p_check, hdcTab[1], setup.menu_vram_load, 1);

  ddxSetCursor(0);
  DisplayFrame();
  DisplayFrame();
  ddxResizeCursorBack(0);
  ddxSetCursorSurface(0);
  ddxSetCursor(1);
  DisplayFrame();
  DisplayFrame();
  ddxReleaseBitmap(iClock);
}

void InitTabSound(CONTROL_LIST_ITEM * citem, int *hdcTab)
{
  co_Set_Text_Right(hdcTab[2], "##settings_sound", 0, 260, 50);
  citem[21].p_prog = co_Create_Progres(hdcTab[2], 310, 50, 0, 100, 4);
  citem[21].iTab = 2;
  co_Progres_Set(citem[21].p_prog, hdcTab[2], setup.soundvolume);

  co_Set_Text_Right(hdcTab[2], "##settings_ambient", 0, 260, 85);
  citem[22].p_prog = co_Create_Progres(hdcTab[2], 310, 85, 0, 100, 5);
  citem[22].iTab = 2;
  co_Progres_Set(citem[22].p_prog, hdcTab[2], setup.ambientvolume);

  co_Set_Text_Right(hdcTab[2], "##settings_music", 0, 260, 120);
  citem[23].p_prog = co_Create_Progres(hdcTab[2], 310, 120, 0, 100, 6);
  citem[23].iTab = 2;
  co_Progres_Set(citem[23].p_prog, hdcTab[2], setup.musicvolume);
}

void InitTabControls(CONTROL_LIST_ITEM * citem, int *hdcTab)
{
  char ctext[256];
  int iClock;

  iClock = ddxLoadBitmap("clock1-1.bmp", pBmpArchive);
  ddxResizeCursorBack(iClock);
  DrawClock(&iClock, 0);

  citem[24].p_list = co_Create_List(hdcTab[3], 25, 50, 800, 580, 0, 31, 0);
  citem[24].iTab = 3;

  co_List_Add_String(citem[24].p_list, 0, 2, "##control_beatle", 0, 1);
  co_List_Add_String(citem[24].p_list, 1, 2, "##control_forward", 0, 0);
  co_List_Add_String(citem[24].p_list, 2, 2, "##control_back", 0, 0);
  co_List_Add_String(citem[24].p_list, 3, 2, "##control_left", 0, 0);
  co_List_Add_String(citem[24].p_list, 4, 2, "##control_right", 0, 0);
  co_List_Add_String(citem[24].p_list, 5, 2, "##control_nextb", 0, 0);
  co_List_Add_String(citem[24].p_list, 6, 2, "##control_b1", 0, 0);
  co_List_Add_String(citem[24].p_list, 7, 2, "##control_b2", 0, 0);
  co_List_Add_String(citem[24].p_list, 8, 2, "##control_b3", 0, 0);
  co_List_Add_String(citem[24].p_list, 9, 2, "##control_b4", 0, 0);
  co_List_Add_String(citem[24].p_list, 10, 2, "##control_b5", 0, 0);
  co_List_Add_String(citem[24].p_list, 11, 2, "##control_camera", 0, 1);
  co_List_Add_String(citem[24].p_list, 12, 2, "##control_crotation", 0, 0);
  co_List_Add_String(citem[24].p_list, 13, 2, "##control_cmove", 0, 0);
  co_List_Add_String(citem[24].p_list, 14, 2, "##control_czoomin", 0, 0);
  co_List_Add_String(citem[24].p_list, 15, 2, "##control_czoomout", 0, 0);
  co_List_Add_String(citem[24].p_list, 16, 2, "##control_cfastleft", 0, 0);
  co_List_Add_String(citem[24].p_list, 17, 2, "##control_cfastright", 0, 0);
  co_List_Add_String(citem[24].p_list, 18, 2, "##control_ccamera", 0, 0);
  co_List_Add_String(citem[24].p_list, 19, 2, "##control_game", 0, 1);
  co_List_Add_String(citem[24].p_list, 20, 2, "##control_gmenu", 0, 0);
  co_List_Add_String(citem[24].p_list, 21, 2, "##control_grestart", 0, 0);
  co_List_Add_String(citem[24].p_list, 22, 2, "##control_ginventory", 0, 0);
  //co_List_Add_String(citem[24].p_list, 23, 2, "##control_gusepack", 0, 0);
  co_List_Add_String(citem[24].p_list, 23, 2, "##control_gnexttrack", 0, 0);
  co_List_Add_String(citem[24].p_list, 24, 2, "##control_gturnoffi", 0, 0);
  co_List_Add_String(citem[24].p_list, 25, 2, "##control_gturnofft", 0, 0);
  co_List_Add_String(citem[24].p_list, 26, 2, "##control_gscreenshot", 0, 0);
  co_List_Add_String(citem[24].p_list, 27, 2, "##control_gtransparent", 0, 0);
  co_List_Add_String(citem[24].p_list, 28, 2, "##control_ghighlight", 0, 0);
  co_List_Add_String(citem[24].p_list, 29, 2, "##control_gdrawconn", 0, 0);
  co_List_Add_String(citem[24].p_list, 30, 2, "##control_pause", 0, 0);

  Key2String(setup.key[1], ctext);
  co_List_Add_String(citem[24].p_list, 1, 550, ctext, setup.key[1], 0);
  Key2String(setup.key[2], ctext);
  co_List_Add_String(citem[24].p_list, 2, 550, ctext, setup.key[2], 0);
  Key2String(setup.key[3], ctext);
  co_List_Add_String(citem[24].p_list, 3, 550, ctext, setup.key[3], 0);
  Key2String(setup.key[4], ctext);
  co_List_Add_String(citem[24].p_list, 4, 550, ctext, setup.key[4], 0);
  Key2String(setup.key[5], ctext);
  co_List_Add_String(citem[24].p_list, 5, 550, ctext, setup.key[5], 0);
  Key2String(setup.key[6], ctext);
  co_List_Add_String(citem[24].p_list, 6, 550, ctext, setup.key[6], 0);
  Key2String(setup.key[7], ctext);
  co_List_Add_String(citem[24].p_list, 7, 550, ctext, setup.key[7], 0);
  Key2String(setup.key[8], ctext);
  co_List_Add_String(citem[24].p_list, 8, 550, ctext, setup.key[8], 0);
  Key2String(setup.key[9], ctext);
  co_List_Add_String(citem[24].p_list, 9, 550, ctext, setup.key[9], 0);
  Key2String(setup.key[10], ctext);
  co_List_Add_String(citem[24].p_list, 10, 550, ctext, setup.key[10], 0);

  Key2String(setup.key[12], ctext);
  co_List_Add_String(citem[24].p_list, 12, 550, ctext, setup.key[12], 0);
  Key2String(setup.key[13], ctext);
  co_List_Add_String(citem[24].p_list, 13, 550, ctext, setup.key[13], 0);
  Key2String(setup.key[14], ctext);
  co_List_Add_String(citem[24].p_list, 14, 550, ctext, setup.key[14], 0);
  Key2String(setup.key[15], ctext);
  co_List_Add_String(citem[24].p_list, 15, 550, ctext, setup.key[15], 0);
  Key2String(setup.key[16], ctext);
  co_List_Add_String(citem[24].p_list, 16, 550, ctext, setup.key[16], 0);
  Key2String(setup.key[17], ctext);
  co_List_Add_String(citem[24].p_list, 17, 550, ctext, setup.key[17], 0);
  Key2String(setup.key[18], ctext);
  co_List_Add_String(citem[24].p_list, 18, 550, ctext, setup.key[18], 0);

  Key2String(setup.key[20], ctext);
  co_List_Add_String(citem[24].p_list, 20, 550, ctext, setup.key[20], 0);
  Key2String(setup.key[21], ctext);
  co_List_Add_String(citem[24].p_list, 21, 550, ctext, setup.key[21], 0);
  Key2String(setup.key[22], ctext);
  co_List_Add_String(citem[24].p_list, 22, 550, ctext, setup.key[22], 0);
  //Key2String(setup.key[23], ctext);
  //co_List_Add_String(citem[24].p_list, 23, 550, ctext, setup.key[23], 0);
  Key2String(setup.key[23], ctext);
  co_List_Add_String(citem[24].p_list, 23, 550, ctext, setup.key[23], 0);
  Key2String(setup.key[24], ctext);
  co_List_Add_String(citem[24].p_list, 24, 550, ctext, setup.key[24], 0);
  Key2String(setup.key[25], ctext);
  co_List_Add_String(citem[24].p_list, 25, 550, ctext, setup.key[25], 0);
  Key2String(setup.key[26], ctext);
  co_List_Add_String(citem[24].p_list, 26, 550, ctext, setup.key[26], 0);
  Key2String(setup.key[27], ctext);
  co_List_Add_String(citem[24].p_list, 27, 550, ctext, setup.key[27], 0);
  Key2String(setup.key[28], ctext);
  co_List_Add_String(citem[24].p_list, 28, 550, ctext, setup.key[28], 0);
  Key2String(setup.key[29], ctext);
  co_List_Add_String(citem[24].p_list, 29, 550, ctext, setup.key[29], 0);
  Key2String(setup.key[30], ctext);
  co_List_Add_String(citem[24].p_list, 30, 550, ctext, setup.key[30], 0);

  co_List_Redraw(hdcTab[3], citem[24].p_list, 0);

  ddxSetCursor(0);
  DisplayFrame();
  DisplayFrame();
  ddxResizeCursorBack(0);
  ddxSetCursorSurface(0);
  ddxSetCursor(1);
  DisplayFrame();
  DisplayFrame();
  ddxReleaseBitmap(iClock);
}

void RunMenuSetGeneralSettings(CONTROL_LIST_ITEM * citem, int iset)
{

  int i;

  ddxSetFlip(0);

  for (i = 0; i < 2; i++) {
    switch (iset) {
      case 0:

        //modely
        co_Combo_Set_Sel(HDC2DD, citem[33].p_combo, 0);

        //castice
        co_Combo_Set_Sel(HDC2DD, citem[10].p_combo, 0);

        //svetla
        setup.light_dyn = 1;
        co_Combo_Set_Sel(HDC2DD, citem[32].p_combo, 0);

        //textury predmetu
        co_Combo_Set_Sel(HDC2DD, citem[34].p_combo, 0);

        //textury okoli
        co_Combo_Set_Sel(HDC2DD, citem[35].p_combo, 0);

        //zrcadlo
        co_Check_Set_State(citem[11].p_check, HDC2DD, 1, 1);

        //caustic
        co_Check_Set_State(citem[30].p_check, HDC2DD, 1, 1);

        break;
      case 1:

        //modely
        co_Combo_Set_Sel(HDC2DD, citem[33].p_combo, 1);

        //castice
        co_Combo_Set_Sel(HDC2DD, citem[10].p_combo, 1);

        //svetla
        setup.light_dyn = 1;
        co_Combo_Set_Sel(HDC2DD, citem[32].p_combo, 1);

        //textury predmetu
        co_Combo_Set_Sel(HDC2DD, citem[34].p_combo, 1);

        //textury okoli
        co_Combo_Set_Sel(HDC2DD, citem[35].p_combo, 1);

        //zrcadlo
        co_Check_Set_State(citem[11].p_check, HDC2DD, 0, 1);

        //caustic
        co_Check_Set_State(citem[30].p_check, HDC2DD, 0, 1);

        break;
      case 2:

        //modely
        co_Combo_Set_Sel(HDC2DD, citem[33].p_combo, 1);

        //castice
        co_Combo_Set_Sel(HDC2DD, citem[10].p_combo, 2);

        //svetla
        setup.light_dyn = 1;
        co_Combo_Set_Sel(HDC2DD, citem[32].p_combo, 2);

        //textury predmetu
        co_Combo_Set_Sel(HDC2DD, citem[34].p_combo, 1);

        //textury okoli
        co_Combo_Set_Sel(HDC2DD, citem[35].p_combo, 2);

        //zrcadlo
        co_Check_Set_State(citem[11].p_check, HDC2DD, 0, 1);

        //caustic
        co_Check_Set_State(citem[30].p_check, HDC2DD, 0, 1);

        break;
      case 3:

        //modely
        co_Combo_Set_Sel(HDC2DD, citem[33].p_combo, 1);

        //castice
        co_Combo_Set_Sel(HDC2DD, citem[10].p_combo, 2);

        //svetla
        setup.light_dyn = 0;
        co_Combo_Set_Sel(HDC2DD, citem[32].p_combo, 3);

        //textury predmetu
        co_Combo_Set_Sel(HDC2DD, citem[34].p_combo, 2);

        //textury okoli
        co_Combo_Set_Sel(HDC2DD, citem[35].p_combo, 2);

        //zrcadlo
        co_Check_Set_State(citem[11].p_check, HDC2DD, 0, 1);

        //caustic
        co_Check_Set_State(citem[30].p_check, HDC2DD, 0, 1);

        break;
    }

    if (!i)
      DisplayFrame();
  }

  ddxSetFlip(1);

}

void RunMenuSettings(char *p_File_Name, HWND hWnd, AUDIO_DATA * p_ad, int cpu)
{
  DWORD dwEplased = 0, dwStart, dwStop;

  ROZLISENI *roz = NULL;
  int roz_size = 0;
  int ActiveTab = 0;
  int hdcTab[TAB_NUM];
  int hdcTabUse[TAB_NUM];

  CONTROL_LIST_ITEM citem[CLIST_ITEMC];

  int iClock[12];

//      FILE    *file;
  char dir[256];
  int lastcmd, lastanm, i;

  //CMD_LINE      res[RES_NUM];
  CMD_LINE *res = NULL;
  int lastabv = -1;
  char in, click = 0;
  int anmid = -1, resid = -1, anbind = -1;
  int bind;
  int igeneral_3d;
  int iComboActSel = -1;

  for (i = 0; i < 12; i++)
    iClock[i] = -1;

  res = (CMD_LINE *) malloc(RES_NUM * sizeof(CMD_LINE));

  if (!res) {
    kprintf(1, "RunMenuSettings: Out of Memory");
    return;
  }
  else
    ZeroMemory(res, RES_NUM * sizeof(CMD_LINE));

  EnumRozliseni(&roz, &roz_size);
  Load_ini();
  igeneral_3d = setup.general_3d;

  //BitBltU(FontDC, 0, 0, 1024, 768, NULL, 0, 0, WHITENESS);
  //BitBltU(BackDC, 0, 0, 1024, 768, NULL, 0, 0, WHITENESS);
  ddxCleareSurface(FontDC);
  ddxCleareSurface(BackDC);
  ddxCleareSurface(CompositDC);

  LoadClock(iClock);

  if (iClock[0] != -1) {
    ddxResizeCursorBack(iClock[0]);
    DrawClock(iClock, 0);
  }

MENU_SETTING_BRUTAL_RESTART:

  ActiveTab = 0;

  ZeroMemory(citem, CLIST_ITEMC * sizeof(CONTROL_LIST_ITEM));

  fn_Set_Font(cFontFile[2]);
  fn_Load_Bitmaps();

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

  //CreateFontAnimations(res, &lastcmd);

  DrawClock(iClock, 1);
  // privede prikazy, ketere se maji provest na zacatku a, kresleni, flip,
  // animace na OnAbove
  for (i = 0; i < lastcmd; i++) {
    lastanm = 0;

    switch (res[i].iParam[0]) {
      case COM_DRAW:
        if (!res[i].iLayer) {
          ddxDrawSurface(BackDC, res[i].iParam, 2);
          //menucommand_Draw(BackDC, res[i].iParam, 2);
        }
        else {
          ddxDrawSurfaceColorKey(BackDC, res[i].iParam, 2, TRANSCOLOR);
          //ddxDrawSurface(BackDC, res[i].iParam, 2);
          ddxDrawSurface(FontDC, res[i].iParam, 3);
          //menucommand_DrawT(BackDC, res[i].iParam);
          //menucommand_Draw(FontDC, res[i].iParam, 3);
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

  DrawClock(iClock, 2);
  for (i = 0; i < TAB_NUM; i++) {
    hdcTab[i] = ddxCreateSurface(TAB_XRES, TAB_YRES, ddxFindFreeSurface());

    if (hdcTab[i]) {
      ddxBitBlt(hdcTab[i], 0, 0, TAB_XRES, TAB_YRES, BackDC, TAB_X, TAB_Y);
    }

    hdcTabUse[i] = 0;
  }

  DrawClock(iClock, 3);
  if (co_Load_Graphic(0)) {
    //int count = 0;

    DrawClock(iClock, 4);
    citem[0].bActive = 0;
    citem[1].bActive = 0;

    //citem[2].p_check = co_Create_CheckBox(hdcTab[0], 25, 50, "##settings_camera_rot", 0, 0);
    //co_Check_Set_State(citem[2].p_check, hdcTab[0], setup.pohled_berusky, 1);
    citem[2].bActive = 1;

    citem[3].p_check =
      co_Create_CheckBox(hdcTab[0], 25, 50, "##settings_camera_mov", 0, 1);
    co_Check_Set_State(citem[3].p_check, hdcTab[0], setup.posouvat_kameru, 1);
    citem[3].bActive = 1;

    citem[4].p_check =
      co_Create_CheckBox(hdcTab[0], 25, 80, "##settings_b1_control", 0, 2);
    co_Check_Set_State(citem[4].p_check, hdcTab[0], setup.ovladani, 1);
    citem[4].bActive = 1;

    citem[45].p_check =
      co_Create_CheckBox(hdcTab[0], 25, 110, "##settings_beathe_vis_at_start",
      0, 10);
    co_Check_Set_State(citem[45].p_check, hdcTab[0],
      setup.start_zpruhlednovani, 1);
    citem[45].bActive = 1;

    citem[46].p_check =
      co_Create_CheckBox(hdcTab[0], 25, 140, "##settings_items_vis_at_start",
      0, 11);
    co_Check_Set_State(citem[46].p_check, hdcTab[0], setup.start_zvyraznovani,
      1);
    citem[46].bActive = 1;

    citem[51].p_check =
      co_Create_CheckBox(hdcTab[0], 25, 200, "##settings_camera_intro", 0,
      16);
    co_Check_Set_State(citem[51].p_check, hdcTab[0], setup.camera_intro, 1);
    citem[51].bActive = 1;

    citem[47].p_check =
      co_Create_CheckBox(hdcTab[0], 25, 230, "##settings_camera_rect", 0, 12);
    co_Check_Set_State(citem[47].p_check, hdcTab[0], setup.ovladani_rohy, 1);
    citem[47].bActive = 1;

    citem[48].p_check =
      co_Create_CheckBox(hdcTab[0], 25, 260, "##settings_implicit_move", 0,
      13);

    if (setup.ovladani_rohy_default)
      co_Check_Set_State(citem[48].p_check, hdcTab[0], 0, 1);
    else
      co_Check_Set_State(citem[48].p_check, hdcTab[0], 1, 1);

    DrawClock(iClock, 5);

    citem[48].bActive = 1;

    citem[49].p_check =
      co_Create_CheckBox(hdcTab[0], 25, 290, "##settings_imlicit_rot", 0, 14);

    if (setup.ovladani_rohy_default)
      co_Check_Set_State(citem[49].p_check, hdcTab[0], 1, 1);
    else
      co_Check_Set_State(citem[49].p_check, hdcTab[0], 0, 1);

    citem[49].bActive = 1;

    citem[50].p_check =
      co_Create_CheckBox(hdcTab[0], 25, 320, "##settings_camera_swap", 0, 15);
    co_Check_Set_State(citem[50].p_check, hdcTab[0], setup.ovladani_rohy_smer,
      1);
    citem[50].bActive = 1;

    //citem[51].p_check = co_Create_CheckBox(hdcTab[0], 25, 350, "##setings_camera_move_cur", 0, 16);
    //co_Check_Set_State(citem[51].p_check, hdcTab[0], setup.ovladani_pr_posun, 1);
    //citem[51].bActive = 1;

    //co_Set_Text_Right(hdcTab[0], "##settings_transparency", 0, 450, 300);
    //citem[6].p_prog = co_Create_Progres(hdcTab[0], 500, 300, 2, 18, 0);
    //co_Progres_Set(citem[6].p_prog, hdcTab[0], ftoi(setup.p_kamera_alfa * 20));
    //citem[6].bActive = 1;

    co_Set_Text_Right(hdcTab[0], "##setings_camera_speed", 0, 450, 390);
    citem[52].p_prog = co_Create_Progres(hdcTab[0], 500, 387, 0, 10, 0);
    co_Progres_Set(citem[52].p_prog, hdcTab[0],
      ftoi(setup.ovladani_rohy_rychlost * 10));
    citem[52].bActive = 1;

    co_Set_Text_Right(hdcTab[0], "##settings_trans_radius", 0, 450, 425);
    citem[7].p_prog = co_Create_Progres(hdcTab[0], 500, 412, 1, 20, 1);
    co_Progres_Set(citem[7].p_prog, hdcTab[0],
      ftoi(setup.p_kamera_radius * 2));
    citem[7].bActive = 1;

    //if(!setup.posouvat_kameru)
    //co_Check_Disable(hdcTab[0], 0, 0, citem, CLIST_ITEMC, 0);

    if (!setup.ovladani_rohy) {
      co_Check_Disable(hdcTab[0], 0, 0, citem, CLIST_ITEMC, 13);
      co_Check_Disable(hdcTab[0], 0, 0, citem, CLIST_ITEMC, 14);
      co_Check_Disable(hdcTab[0], 0, 0, citem, CLIST_ITEMC, 15);
      //co_Check_Disable(hdcTab[0], 0, 0, citem, CLIST_ITEMC, 16);
      co_Progres_Disable(hdcTab[0], 0, 0, citem, CLIST_ITEMC, 0, 1,
        hdcTab[0]);
    }

    DrawClock(iClock, 6);
    ddxBitBltDisplay(0, 0, 1024, 768, BackDC, 0, 0);
    SetTab(0, -1, citem, CLIST_ITEMC, hdcTab);

    //if(!setup.posouvat_kameru)
    //      co_Check_Disable(HDC2DD, TAB_X, TAB_Y, citem, CLIST_ITEMC, 0);

    if (!setup.ovladani_rohy) {
      co_Check_Disable(HDC2DD, TAB_X, TAB_Y, citem, CLIST_ITEMC, 13);
      co_Check_Disable(HDC2DD, TAB_X, TAB_Y, citem, CLIST_ITEMC, 14);
      co_Check_Disable(HDC2DD, TAB_X, TAB_Y, citem, CLIST_ITEMC, 15);
      //co_Check_Disable(HDC2DD, TAB_X, TAB_Y, citem, CLIST_ITEMC, 16);
      co_Progres_Disable(HDC2DD, TAB_X, TAB_Y, citem, CLIST_ITEMC, 0, 0,
        HDC2DD);
    }

    DrawClock(iClock, 7);

    ddxCleareSurface(BackDC);
    hdcTabUse[0] = 1;

    ddxSetCursor(0);
    DisplayFrame();
    DisplayFrame();
    ddxResizeCursorBack(0);
    ddxSetCursorSurface(0);
    ddxSetCursor(1);
    DisplayFrame();
    DisplayFrame();

    for (i = 0; i < 12; i++)
      if (iClock[i] != -1) {
        ddxReleaseBitmap(iClock[i]);
        iClock[i] = -1;
      }

  }

//BEGIN_MENU:

  for (i = 0; i < lastcmd; i++)
    if (res[i].iParam[0] == COM_RUNANIMATION) {
      int iWave = AddAnimation(&res[i], p_ad, 0, 0);

      if (iWave != -1) {
        if (res[i + 1].iParam[0] == COM_BINDSOUND)
          anm[iWave].iWave = res[i + 1].iParam[5] =
            mPlaySound(&res[i + 1], p_ad, 2);
      }
    }

  dim.t1 = 0;
  dim.t2 = 0;
  dim.dx = 0;
  dim.dy = 0;
  anmid = -1;
  resid = -1;
  anbind = -1;
  bind = -1;
  lastabv = -1;
  in = 0;

  spracuj_spravy(0);

  while (!key[K_ESC]) {
    dwStart = timeGetTime();

    //pohnul mysi
    if (dim.dx || dim.dy) {
      //dostala se mys do akcni oblasti (OnAbove)?
      if (!click) {
        for (i = 0; i < lastcmd; i++) {
          if (res[i].iParam[0] == COM_ONABOVE) {
            if ((dim.x >= res[i].iParam[1]) && (dim.x <= res[i].iParam[3]) &&
                (dim.y >= res[i].iParam[2]) && (dim.y <= res[i].iParam[4])) 
            {
              //spusteni animace v OnAbove
              if (i != lastabv) {
                if (in) {
                  Stop(&res[lastabv]);

                  if (!res[lastabv].iLayer) {
                    //menucommand_Draw(_2dd.hDC, res[lastabv].iAnim[0], 0);
                    ddxDrawDisplay(res[lastabv].iAnim[0], 0);
                    ddxDrawSurface(CompositDC, res[lastabv].iAnim[0], 3);
                  }
                  else {
                    //menucommand_DrawT(_2dd.hDC, res[lastabv].iAnim[0]);
                    ddxDrawDisplayColorKey(res[lastabv].iAnim[0], 3,
                      TRANSCOLOR);
                    //menucommand_Draw(FontDC, res[lastabv].iAnim[0], 3);
                    ddxDrawSurface(FontDC, res[lastabv].iAnim[0], 3);
                  }
                }

                CheckAnimation(&res[i], p_ad);

                lastabv = i;
                AddAnimation(&res[i], p_ad, 0, 1);
                in = 1;

                bind = ChooseBidedAnimation(res, i + 1, p_ad);

                if (bind != -1) {
                  CheckAnimation(&res[bind], p_ad);
                  AddAnimation(&res[bind], p_ad, 1, 1);
                  anbind = bind;

                  mPlaySound(&res[bind], p_ad, 0);
                }

                strcpy(dir, res[i].cParam[1]);
              }
            }
            else if (lastabv == i) {
              // odesel z oblasti, ktera byla aktivni -> stop animace                                 
              // a odznaceni oblasti
              Stop(&res[i]);

              if (!res[i].iLayer) {
                //menucommand_Draw(_2dd.hDC, res[i].iAnim[0], 0);
                ddxDrawDisplay(res[i].iAnim[0], 0);
                ddxDrawSurface(CompositDC, res[i].iAnim[0], 3);
              }
              else {
                //menucommand_DrawT(_2dd.hDC, res[i].iAnim[0]);
                ddxDrawDisplayColorKey(res[i].iAnim[0], 0, TRANSCOLOR);
                //menucommand_Draw(FontDC, res[i].iAnim[0], 3);
                ddxDrawSurface(FontDC, res[i].iAnim[0], 3);
              }

              bind = ChooseBidedExitAnimation(res, i + 1, p_ad);

              if (bind != -1) {
                int iAnim;

                if (anbind != -1) {
                  Stop(&res[anbind]);

                  if (!res[i].iLayer) {
                    //menucommand_Draw(_2dd.hDC, res[anbind].iAnim[0], 0);
                    ddxDrawDisplay(res[anbind].iAnim[0], 0);
                    ddxDrawSurface(CompositDC, res[anbind].iAnim[0], 3);
                  }
                  else {
                    //menucommand_DrawT(_2dd.hDC, res[anbind].iAnim[0]);
                    ddxDrawDisplayColorKey(res[anbind].iAnim[0], 0,
                      TRANSCOLOR);
                    //menucommand_Draw(FontDC, res[anbind].iAnim[0], 3);
                    ddxDrawSurface(FontDC, res[anbind].iAnim[0], 3);
                  }
                }

                iAnim = AddAnimation(&res[bind], p_ad, 1, 1);

                if (iAnim != -1)
                  anm[iAnim].iWave = mPlaySound(&res[bind], p_ad, 2);
              }

              lastabv = -1;
              anbind = -1;
              in = 0;

              strcpy(dir, "");
            }
          }
        }
      }
      dim.dx = 0;
      dim.dy = 0;
    }

    co_Handle_Controls(citem, CLIST_ITEMC, dim.x - TAB_X, dim.y - TAB_Y,
      HDC2DD, TAB_X, TAB_Y);


		if(co_Progres_Changed(citem, CLIST_ITEMC, 4))
			p_ad->Sound_Gain = co_Progres_Get(citem, CLIST_ITEMC, 4) / 100.0f;


    if (co_Progres_Changed(citem, CLIST_ITEMC, 6)) {
      float f = co_Progres_Get(citem, CLIST_ITEMC, 6) / 100.0f;

      ogg_gain(f);
      p_ad->Music_Gain = f;

      if (f >= 0.05f && !ogg_playing()) {
        p_ad->Music_Gain = f;
        ap_Play_Song(0,0,p_ad);
      }

    }

    if (ActiveTab == 1) {
      iComboActSel = co_Combo_Get_Sel_Not_Opend(citem, CLIST_ITEMC, 1);

      if (iComboActSel != -1 && iComboActSel != igeneral_3d) {
        igeneral_3d = iComboActSel;
        RunMenuSetGeneralSettings(citem, igeneral_3d);
      }
    }

    //stlacil leve tlacitko
    if (dim.t1 && !click) {
      int iCheck = -1;
      LIST_VIEW_CONTROL *p_li;

      if (co_List_Get_Clck(citem, CLIST_ITEMC, 0, &p_li) == 1) {
        float pos[3] = { 0, 0, 0 };

        if(karmin_aktivni)
          ap_Play_Sound(0,1,0,pos,54 + (rand()%3), NULL, p_ad);

        SetCharMenu(p_li);
      }

      iCheck = co_Check_Get_State_Change(citem, CLIST_ITEMC, 12);

      if (iCheck != -1) {
        if (!iCheck) {
          co_Check_Disable(HDC2DD, TAB_X, TAB_Y, citem, CLIST_ITEMC, 13);
          co_Check_Disable(HDC2DD, TAB_X, TAB_Y, citem, CLIST_ITEMC, 14);
          co_Check_Disable(HDC2DD, TAB_X, TAB_Y, citem, CLIST_ITEMC, 15);
          //co_Check_Disable(HDC2DD, TAB_X, TAB_Y, citem, CLIST_ITEMC, 16);
          co_Progres_Disable(HDC2DD, TAB_X, TAB_Y, citem, CLIST_ITEMC, 0, 1,
            HDC2DD);
        }
        else {
          co_Check_Enable(HDC2DD, TAB_X, TAB_Y, citem, CLIST_ITEMC, 13);
          co_Check_Enable(HDC2DD, TAB_X, TAB_Y, citem, CLIST_ITEMC, 14);
          co_Check_Enable(HDC2DD, TAB_X, TAB_Y, citem, CLIST_ITEMC, 15);
          //co_Check_Enable(HDC2DD, TAB_X, TAB_Y, citem, CLIST_ITEMC, 16);
          co_Progres_Enable(HDC2DD, TAB_X, TAB_Y, citem, CLIST_ITEMC, 0);
        }
      }

      if (ActiveTab == 0 && !citem[49].bDisabled) {
        iCheck = co_Check_Get_State_Change(citem, CLIST_ITEMC, 13);

        if (iCheck != -1 && iCheck == 1)
          co_Check_Set_State(citem[49].p_check, HDC2DD, 0, 1);
        else if (iCheck != -1 && !iCheck)
          co_Check_Set_State(citem[49].p_check, HDC2DD, 1, 1);

        iCheck = co_Check_Get_State_Change(citem, CLIST_ITEMC, 14);

        if (iCheck != -1 && iCheck == 1)
          co_Check_Set_State(citem[48].p_check, HDC2DD, 0, 1);
        else if (iCheck != -1 && !iCheck)
          co_Check_Set_State(citem[48].p_check, HDC2DD, 1, 1);
      }

      //dostala se mys do akcni oblasti (OnClick)?
      for (i = 0; i < lastcmd; i++)
        if (res[i].iParam[0] == COM_ONCLICK)
          if ((dim.x >= res[i].iParam[1]) &&
            (dim.x <= res[i].iParam[3]) &&
            (dim.y >= res[i].iParam[2]) && (dim.y <= res[i].iParam[4])) {
            if (res[i].iAnim[0][0] >= 0) {
              //pokud je animace, tak ji spust
              anmid = AddAnimation(&res[i], p_ad, 0, 1);

              if (res[i + 1].iParam[0] == COM_BINDSOUND)
                mPlaySound(&res[i + 1], p_ad, 1);

              resid = i;
              click = 1;
            }
            else {
              if (res[i + 1].iParam[0] == COM_BINDSOUND)
                mPlaySound(&res[i + 1], p_ad, 1);

              resid = i;
              click = 1;
              anmid = 31;
            }
          }

      dim.t1 = 0;
    }

    //provedeni akce po animaci menu
    if (click)
      if (!anm[anmid].cmd) {
        click = 0;

        //StopAll();

        if (!strcmp(res[resid].cParam[1], "OK")) {
          float pos[3] = { 0, 0, 0 };

          if(karmin_aktivni)
            ap_Play_Sound(0,1,0,pos,54 + (rand()%3), NULL, p_ad);

          SetMenuSettings(roz, citem, hdcTabUse);
          Save_ini();
          key[K_ESC] = 1;
        }

        if (!strcmp(res[resid].cParam[1], "EXIT")) {
          float pos[3] = { 0, 0, 0 };

          if(karmin_aktivni)
            ap_Play_Sound(0,1,0,pos,54 + (rand()%3), NULL, p_ad);

          key[K_ESC] = 1;
        }

        if (!strcmp(res[resid].cParam[1], "GAME") && ActiveTab) {
          SetTab(0, ActiveTab, citem, CLIST_ITEMC, hdcTab);
          ActiveTab = 0;
          hdcTabUse[0]++;
        }

        if (!strcmp(res[resid].cParam[1], "3D") && ActiveTab != 1) {
          if (!hdcTabUse[1])
            InitTab3d(citem, hdcTab, roz, roz_size);

          SetTab(1, ActiveTab, citem, CLIST_ITEMC, hdcTab);
          ActiveTab = 1;

          hdcTabUse[1]++;
        }


        if (!strcmp(res[resid].cParam[1], "SOUND") && ActiveTab != 2) {
          if (!hdcTabUse[2])
            InitTabSound(citem, hdcTab);

          SetTab(2, ActiveTab, citem, CLIST_ITEMC, hdcTab);
          ActiveTab = 2;
          hdcTabUse[2]++;
        }

        if (!strcmp(res[resid].cParam[1], "CONTROLS") && ActiveTab != 3) {
          if (!hdcTabUse[3])
            InitTabControls(citem, hdcTab);

          SetTab(3, ActiveTab, citem, CLIST_ITEMC, hdcTab);
          ActiveTab = 3;
          hdcTabUse[3]++;
        }

        resid = -1;
        
        if (key[K_ESC]) {
          for(i=0;i<lastcmd;i++) {
            if(res[i].iParam[0] == COM_BINDSOUND && res[i].iParam[5] != -1)
            {
              adas_Release_Source(PARTICULAR_SOUND_SOURCE, UNDEFINED_VALUE, res[i].iParam[5]);
              res[i].iParam[5] = -1;
            }
          }
          goto __QUIT;
        }

        /*else
           goto BEGIN_MENU; */
      }

    //pokud prisel cas, tak provedu nahodne animace (podle jejich pravdepodobnosti)
    if (timercnt > 500) {
      timercnt = 0;

      for (i = 0; i < lastcmd; i++)
        if (res[i].iParam[0] == COM_RANDOMANIMATION)
          if (rand() % 200 <= res[i].iParam[1] &&
            strcmp(dir, res[i].cParam[0])) {
            CheckAnimation(&res[i], p_ad);
            AddAnimation(&res[i], p_ad, 0, 0);
          }
    }

/*		spracuj_spravy(0);
		ddxUpdateMouse();

		if(dim.dx || dim.dy)
			DisplayFrame();*/

    dwStop = timeGetTime();

    dwEplased += dwStop - dwStart;

    spracuj_spravy(0);
    ddxUpdateMouse();
    AnimationEvent(dwStop, p_ad);

    if (dim.tf1) {
      dim.t1 = 1;
      dim.tf1 = 0;
    }

    if (dim.tf2) {
      dim.t2 = 1;
      dim.tf2 = 0;
    }

    if (ddxRestore(p_ad))
      goto MENU_SETTING_BRUTAL_RESTART;
  }

__QUIT:
  //BitBltU(FontDC, 0, 0, 1024, 768, NULL, 0, 0, WHITENESS);
  //BitBltU(BackDC, 0, 0, 1024, 768, NULL, 0, 0, WHITENESS);
  ddxCleareSurface(FontDC);
  ddxCleareSurface(BackDC);
  ddxCleareSurface(CompositDC);

  SetMenuSettingsS(roz, citem, hdcTabUse);
  Save_ini();

  fn_Release_Font(1);
  StopAll();
  co_Handle_Release(citem, CLIST_ITEMC);
  co_Release_Graphic();
  key[K_ESC] = 0;

  FreeAnimations(res, RES_NUM);
  free((void *) res);
  free((void *) roz);
}

void RunStretchAnimation(char *cScene, int x, int y, AUDIO_DATA * p_ad)
{
  int idx;
  RECT r;
  RECT s = { 0, 0, 1024, 768 };
  APAK_HANDLE *hArchive = NULL;
  char cFile[MAX_PATH];
  char cDir[MAX_PATH];

  if (iLanguageVersion == 4) {
    GetPrivateProfileString("game", "bitmap_dir", "c:\\", cDir, MAX_PATH, ini_file);
    working_file_translate(cDir, MAX_PATH);
    sprintf(cFile, "%s%cscene%d.pak", cDir, DIR_SLASH, iActualScene);    
    hArchive = apakopen(cFile, cDir, &idx);

    if (!hArchive) {
      kprintf(1, "Nepodarilo se otevrit archiv [%s]\n", cFile);
      return;
    }
    else
      hArchive->pActualNode = hArchive->pRootNode->pNextNode;

    idx = ddxLoadBitmap(cScene, hArchive);
  }
  else
    idx = ddxLoadBitmap(cScene, pBmpArchive);

  if (idx < 0) {
    if (iLanguageVersion == 4)
      apakclose(&hArchive);
    return;
  }

  r.left = x;
  r.top = y;
  r.right = r.left + ddxGetWidth(180);
  r.bottom = r.top + ddxGetHight(180);

  StretchAnimation(&r, &s, idx, 1, p_ad);
  ddxReleaseBitmap(idx);

  if (iLanguageVersion == 4)
    apakclose(&hArchive);
}

void RunMenuNewGameSceneActivate(CMD_LINE * res)
{
#ifndef __DEMO
  int i;

  for (i = 1; i < 10; i++)
    if (pPlayerProfile.cScene[i]) {
      res[i + 8].bActive = 1;
      res[i + 17].bActive = 1;
      ddxDrawSurface(BackDC, res[i + 8].iAnim[0], 2);

      if (i > 1)
        ddxDrawSurface(BackDC, res[i - 1].iAnim[6], 2);
    }
#endif

#ifdef __DEMO

  res[6 + 8].bActive = 1;
  res[6 + 17].bActive = 1;
  ddxDrawSurface(BackDC, res[6 + 8].iAnim[0], 2);

#endif
}

void GetRunMenuNewGameSceneLoadGame(char *cscene, char *cscenemap,
  char *csceneanim, char *cscenelevel, int *iLevelStart, int *iNumOfLevels,
  int *xTV, int *yTV, char *bTutorial, int *xBack, int *yBack)
{
  *bTutorial = 0;

  switch (iActualScene) {
    case 0:
      *iLevelStart = 200;
      *iNumOfLevels = 20;
      *xTV = 0;
      *yTV = 0;
      *bTutorial = 1;
      *xBack = 975;
      *yBack = 7;
      break;
    case 1:
      *iLevelStart = 0;
      *iNumOfLevels = 11;
      *xTV = 919;
      *yTV = 677;
      *xBack = 713;
      *yBack = 679;
      break;
    case 2:
      *iLevelStart = 11;
      *iNumOfLevels = 10;
      *xTV = 916;
      *yTV = 8;
      *xBack = 971;
      *yBack = 703;
      break;
    case 3:
      *iLevelStart = 21;
      *iNumOfLevels = 12;
      *xTV = 918;
      *yTV = 7;
      *xBack = 973;
      *yBack = 413;
      break;
    case 4:
      *iLevelStart = 33;
      *iNumOfLevels = 10;
      *xTV = 197;
      *yTV = 9;
      *xBack = 8;
      *yBack = 16;
      break;
    case 5:
      *iLevelStart = 43;
      *iNumOfLevels = 10;
      *xTV = 918;
      *yTV = 9;
      *xBack = 976;
      *yBack = 309;
      break;
    case 6:
      *iLevelStart = 53;

#ifndef __DEMO
      *iNumOfLevels = 10;
#endif

#ifdef __DEMO
      *iNumOfLevels = 6;
#endif

      *xTV = 920;
      *yTV = 8;
      *xBack = 967;
      *yBack = 211;
      break;
    case 7:
      *iLevelStart = 63;
      *iNumOfLevels = 10;
      *xTV = 11;
      *yTV = 6;
      *xBack = 8;
      *yBack = 279;
      break;
    case 8:
      *iLevelStart = 73;
      *iNumOfLevels = 12;
      *xTV = 15;
      *yTV = 677;
      *xBack = 12;
      *yBack = 490;
      break;
    case 9:
      *iLevelStart = 85;
      *iNumOfLevels = 10;
      *xTV = 918;
      *yTV = 7;
      *xBack = 971;
      *yBack = 338;
      break;
    case 10:
      *iLevelStart = 300;
      *iNumOfLevels = 15;
      *xTV = 920;
      *yTV = 5;
      *xBack = 706;
      *yBack = 6;
      break;
    case 11:
      *iLevelStart = 315;
      *iNumOfLevels = 15;
      *xTV = 920;
      *yTV = 5;
      *xBack = 750;
      *yBack = 5;
      break;
    case 12:
      *iLevelStart = 330;
      *iNumOfLevels = 15;
      *xTV = 920;
      *yTV = 7;
      *xBack = 973;
      *yBack = 229;
      break;
  }

  sprintf(cscenemap, "scene%d_map.bmp", iActualScene);
  sprintf(csceneanim, "scene%d_anim", iActualScene);
  sprintf(cscenelevel, "scene%d_levels.txt", iActualScene);
  sprintf(cscene, "Mmnew_game_scene%d_map.txt", iActualScene);
}

void RunMenuNewGameSceneLoadGame(HWND hWnd, AUDIO_DATA * p_ad, int cpu,
  char bLoadGame, char *cSaveFile)
{
  char cscene[64] = "";
  char cscenemap[64] = "";
  char csceneanim[64] = "";
  char cscenelevel[64] = "";
  int iLevelStart = 0;
  int iNumOfLevels = 0;
  int xTV = 0;
  int yTV = 0;
  int xBack = 0;
  int yBack = 0;
  char bTutorial = 0;

  GetRunMenuNewGameSceneLoadGame(cscene, cscenemap, csceneanim, cscenelevel,
    &iLevelStart, &iNumOfLevels, &xTV, &yTV, &bTutorial, &xBack, &yBack);

  RunMenuSceneMap(cscene, NULL, p_ad, cpu, cscenemap,
    csceneanim, iActualScene, iLevelStart,
    iNumOfLevels, cscenelevel, xTV, yTV, 1, cSaveFile, bTutorial, 0, 0);
}

void RunMenuNewGameScene(char *p_File_Name, HWND hWnd, AUDIO_DATA * p_ad,
  int cpu, char bLoadGame, char *cSaveFile, char bNewGame)
{
  DWORD dwEplased = 0, dwStart, dwStop;

//      FILE    *file;
  char dir[256];
  int lastcmd, lastanm, i;

  //CMD_LINE      res[RES_NUM];
  CMD_LINE *res = NULL;
  int lastabv = -1;
  char in, click = 0;
  int anmid = -1, resid = -1, anbind = -1;
  int bind;

  bBackDC = 0;

  res = (CMD_LINE *) malloc(RES_NUM * sizeof(CMD_LINE));

  if (!res) {
    kprintf(1, "RunMenuNewGameScene: Out of Memory");
    return;
  }
  else
    ZeroMemory(res, RES_NUM * sizeof(CMD_LINE));

#ifndef __DEMO
  if (!pPlayerProfile.cMovie[0] && bNewGame) {
    pPlayerProfile.cMovie[0] = 1;
    pr_SaveProfile(&pPlayerProfile);
    RunMenuComix("menu_comix.txt", NULL, p_ad, 0);
  }
#endif

  ////////////////////////////LOAD GAME
  if (bLoadGame) {
    if (lsi_Get_Save_Info(cSaveFile, &iActualLevel, &iActualScene)) {
      RunMenuNewGameSceneLoadGame(NULL, p_ad, cpu, bLoadGame, cSaveFile);

      bLoadGame = 0;

      if (!iActualScene) {
        free((void *) res);
        return;
      }
    }                           ////////////////////////////LOAD GAME
  }
  else if (!iActualScene) {
    RunMenuSceneMap("Mmnew_game_scene0_map.txt", NULL, p_ad, cpu,
      "scene0_map.bmp", "scene0_anim", 0, 200, 20, "scene0_levels.txt", 0, 0,
      0, NULL, 1, 975, 7);

    bLoadGame = 0;

    if (!iActualScene) {
      free((void *) res);
      return;
    }
  }
  else if (iActualScene == 10) {
    RunMenuSceneMap("Mmnew_game_scene10_map.txt", NULL, p_ad, cpu,
      "scene10_map.bmp", "scene10_anim", 10, 300, 15, "scene10_levels.txt",
      920, 2, 0, NULL, 0, 706, 6);

    bLoadGame = 0;

    if (iActualScene == 10) {
      free((void *) res);
      return;
    }
  }
  else if (iActualScene == 11) {
    RunMenuSceneMap("Mmnew_game_scene11_map.txt", NULL, p_ad, cpu,
      "scene11_map.bmp", "scene11_anim", 11, 315, 15, "scene11_levels.txt",
      920, 2, 0, NULL, 0, 750, 5);

    bLoadGame = 0;

    if (iActualScene == 11) {
      free((void *) res);
      return;
    }
  }
  else if (iActualScene == 12) {
    RunMenuSceneMap("Mmnew_game_scene12_map.txt", NULL, p_ad, cpu,
      "scene12_map.bmp", "scene12_anim", 12, 330, 15, "scene12_levels.txt",
      920, 7, 0, NULL, 0, 973, 229);

    bLoadGame = 0;

    if (iActualScene == 12) {
      free((void *) res);
      return;
    }
  }

  ddxCleareSurface(CompositDC);
  ddxCleareSurface(FontDC);
  ddxCleareSurface(BackDC);

BEGIN_MENU_NEWGAMESCENE_BRUTAL:

  for (bind = 0; bind < RES_NUM; bind++) {
    for (lastcmd = 0; lastcmd < 200; lastcmd++) {
      res[bind].iAnim[lastcmd][0] = -1;
      res[bind].iAnim[lastcmd][11] = -1;
    }

    for (in = 0; in < 6; in++)
      res[bind].iParam[(int)in] = -1;

    res[bind].pCmdLine = res;
  }

  lastcmd = 0;
  timercnt = 0;

  GetPrivateProfileString("game", "data_dir", "c:\\", dir, 256, ini_file);
  working_file_translate(dir, 256);
  chdir((dir));

  //natadhe skript menu
  LoadMenuScript(p_File_Name, res, &lastcmd);

  in = 0;

  // privede prikazy, ketere se maji provest na zacatku a, kresleni, flip,
  // animace na OnAbove
  for (i = 0; i < lastcmd; i++) {
    lastanm = 0;

    switch (res[i].iParam[0]) {
      case COM_DRAW:
        //ddxDrawDisplay(res[i].iParam, 0);
        ddxDrawSurface(BackDC, res[i].iParam, 0);
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

  RunMenuNewGameSceneActivate(res);
  ddxBitBltDisplay(0, 0, 1024, 768, BackDC, 0, 0);
  ddxCleareSurface(BackDC);
  //DisplayFrame();

BEGIN_MENU_NEWGAMESCENE:

  //NOVA CESTA ////////////////////////////////////////////////////////////////////////////////////
  if (bNewScene) {
    bNewScene = 0;

    //      1       =       prvni animace   way1_2
    /*res[1].bEndActivate[0] = 10;  //prvni OnAbove
       res[1].bEndActivate[1] = 19; //prvni OnClick
       res[1].bActive = 1; *///aktivace

    //      1       =       prvni animace   way1_2
    res[iActualScene].bEndActivate[0] = 10 + (iActualScene - 1);        //prvni OnAbove
    res[iActualScene].bEndActivate[1] = 19 + (iActualScene - 1);        //prvni OnClick
    res[iActualScene].bActive = 1;      //aktivace

    pPlayerProfile.cScene[iActualScene + 1] = 1;
    pr_SaveProfile(&pPlayerProfile);
  }

  //aktivace tlacitka zpet
  res[27].bActive = 1;
  res[28].bActive = 1;

  for (i = 0; i < lastcmd; i++)
    if (res[i].iParam[0] == COM_RUNANIMATION && res[i].bActive) {
      int iWave = AddAnimation(&res[i], p_ad, 0, 1);

      if (iWave != -1) {
        if (res[i + 1].iParam[0] == COM_BINDSOUND)
          anm[iWave].iWave = res[i + 1].iParam[5] =
            mPlaySound(&res[i + 1], p_ad, 2);
      }
    }

  dim.t1 = 0;
  dim.t2 = 0;
  dim.dx = 0;
  dim.dy = 0;
  anmid = -1;
  resid = -1;
  anbind = -1;
  bind = -1;
  lastabv = -1;
  in = 0;

  spracuj_spravy(0);

  dwLTime = timeGetTime();

  while (!key[K_ESC]) {
    dwStart = timeGetTime();

    //pohnul mysi
    if (dim.dx || dim.dy) {
      //dostala se mys do akcni oblasti (OnAbove)?
      if (!click)
        for (i = 0; i < lastcmd; i++)
          if (res[i].iParam[0] == COM_ONABOVE && res[i].bActive) {
            if ((dim.x >= res[i].iParam[1]) &&
              (dim.x <= res[i].iParam[3]) &&
              (dim.y >= res[i].iParam[2]) && (dim.y <= res[i].iParam[4])) {
              //spusteni animace v OnAbove
              if (i != lastabv) {
                if (in) {
                  Stop(&res[lastabv]);

                  if (!res[lastabv].iLayer) {
                    ddxDrawDisplay(res[lastabv].iAnim[0], 0);
                    ddxDrawSurface(CompositDC, res[lastabv].iAnim[0], 3);
                  }
                  else {
                    ddxDrawDisplayColorKey(res[lastabv].iAnim[0], 0,
                      TRANSCOLOR);
                    ddxDrawSurface(FontDC, res[lastabv].iAnim[0], 3);
                  }
                }

                CheckAnimation(&res[i], p_ad);

                lastabv = i;
                AddAnimation(&res[i], p_ad, 0, 1);
                in = 1;

                bind = ChooseBidedAnimation(res, i + 1, p_ad);

                if (bind != -1) {
                  CheckAnimation(&res[bind], p_ad);
                  AddAnimation(&res[bind], p_ad, 1, 1);
                  anbind = bind;

                  mPlaySound(&res[bind], p_ad, 0);
                }

                strcpy(dir, res[i].cParam[1]);
              }
            }
            else if (lastabv == i) {
            LEAVE_ANIMATION:

              // odesel z oblasti, ktera byla aktivni -> stop animace                                 
              // a odznaceni oblasti
              Stop(&res[i]);

              if (!res[i].iLayer) {
                ddxDrawDisplay(res[i].iAnim[0], 0);
                ddxDrawSurface(CompositDC, res[i].iAnim[0], 3);
              }
              else {
                ddxDrawDisplayColorKey(res[i].iAnim[0], 0, TRANSCOLOR);
                ddxDrawSurface(FontDC, res[i].iAnim[0], 3);
              }

              bind = ChooseBidedExitAnimation(res, i + 1, p_ad);

              if (bind != -1) {
                int iAnim;

                if (anbind != -1) {
                  Stop(&res[anbind]);

                  if (!res[anbind].iLayer) {
                    ddxDrawDisplay(res[anbind].iAnim[0], 0);
                    ddxDrawSurface(CompositDC, res[anbind].iAnim[0], 3);
                  }
                  else {
                    ddxDrawDisplayColorKey(res[anbind].iAnim[0], 0,
                      TRANSCOLOR);
                    ddxDrawSurface(FontDC, res[anbind].iAnim[0], 3);
                  }
                }

                iAnim = AddAnimation(&res[bind], p_ad, 1, 1);

                if (iAnim != -1)
                  anm[iAnim].iWave = mPlaySound(&res[bind], p_ad, 2);
              }

              lastabv = -1;
              anbind = -1;
              in = 0;

              strcpy(dir, "");
            }

            dim.dx = 0;
            dim.dy = 0;
          }
    }

    //stlacil leve tlacitko
    if (dim.t1 && !click) {
      //dostala se mys do akcni oblasti (OnClick)?
      for (i = 0; i < lastcmd; i++)
        if (res[i].iParam[0] == COM_ONCLICK && res[i].bActive)
          if ((dim.x >= res[i].iParam[1]) &&
            (dim.x <= res[i].iParam[3]) &&
            (dim.y >= res[i].iParam[2]) && (dim.y <= res[i].iParam[4])) {
            if (res[i].iAnim[0][0] >= 0) {
              //pokud je animace, tak ji spust
              anmid = AddAnimation(&res[i], p_ad, 0, 1);

              if (res[i + 1].iParam[0] == COM_BINDSOUND)
                mPlaySound(&res[i + 1], p_ad, 1);

              resid = i;
              click = 1;
            }
            else {
              if (res[i + 1].iParam[0] == COM_BINDSOUND)
                mPlaySound(&res[i + 1], p_ad, 1);

              resid = i;
              click = 1;
              anmid = 31;
            }
          }

      dim.t1 = 0;
    }

    //provedeni akce po animaci menu
    if (click)
      if (!anm[anmid].cmd) {
        click = 0;

        //StopAll();

        if (!strcmp(res[resid].cParam[1], "EXIT") ||
          !strcmp(res[resid].cParam[1], "CANCEL")) {
          key[K_ESC] = 1;
          cRestartMainMenu = 1;
          //break;
        }

        if (!strcmp(res[resid].cParam[1], "SCENE1")) {
          if (iLanguageVersion == 4 && !CheckScenePresence(1)) {
            RunMenuCibron("buy.bmp");
            click = 0;
            i = lastabv;
            goto LEAVE_ANIMATION;
          }
          else {
            iActualScene = 1;
            RunStretchAnimation("scene1_map.bmp", 0, 129, p_ad);
            RunMenuSceneMap("Mmnew_game_scene1_map.txt", NULL, p_ad, cpu,
              "scene1_map.bmp", "scene1_anim", 1, 0, 11, "scene1_levels.txt",
              919, 677, 0, NULL, 0, 713, 679);
          }
        }

        if (!strcmp(res[resid].cParam[1], "SCENE2")) {
          if (iLanguageVersion == 4 && !CheckScenePresence(2)) {
            RunMenuCibron("buy.bmp");
            click = 0;
            i = lastabv;
            goto LEAVE_ANIMATION;
          }
          else {
            iActualScene = 2;
            RunStretchAnimation("scene2_map.bmp", 215, 0, p_ad);
            RunMenuSceneMap("Mmnew_game_scene2_map.txt", NULL, p_ad, cpu,
              "scene2_map.bmp", "scene2_anim", 2, 11, 10, "scene2_levels.txt",
              916, 8, 0, NULL, 0, 971, 703);
          }
        }

        if (!strcmp(res[resid].cParam[1], "SCENE3")) {
          if (iLanguageVersion == 4 && !CheckScenePresence(3)) {
            RunMenuCibron("buy.bmp");
            click = 0;
            i = lastabv;
            goto LEAVE_ANIMATION;
          }
          else {
            iActualScene = 3;
            RunStretchAnimation("scene3_map.bmp", 177, 248, p_ad);
            RunMenuSceneMap("Mmnew_game_scene3_map.txt", NULL, p_ad, cpu,
              "scene3_map.bmp", "scene3_anim", 3, 21, 12, "scene3_levels.txt",
              918, 7, 0, NULL, 0, 973, 413);
          }
        }

        if (!strcmp(res[resid].cParam[1], "SCENE4")) {
          if (iLanguageVersion == 4 && !CheckScenePresence(4)) {
            RunMenuCibron("buy.bmp");
            click = 0;
            i = lastabv;
            goto LEAVE_ANIMATION;
          }
          else {
            iActualScene = 4;
            RunStretchAnimation("scene4_map.bmp", 444, 64, p_ad);
            RunMenuSceneMap("Mmnew_game_scene4_map.txt", NULL, p_ad, cpu,
              "scene4_map.bmp", "scene4_anim", 4, 33, 10, "scene4_levels.txt",
              197, 9, 0, NULL, 0, 8, 16);
          }
        }

        if (!strcmp(res[resid].cParam[1], "SCENE5")) {
          if (iLanguageVersion == 4 && !CheckScenePresence(5)) {
            RunMenuCibron("buy.bmp");
            click = 0;
            i = lastabv;
            goto LEAVE_ANIMATION;
          }
          else {
            iActualScene = 5;
            RunStretchAnimation("scene5_map.bmp", 465, 386, p_ad);
            RunMenuSceneMap("Mmnew_game_scene5_map.txt", NULL, p_ad, cpu,
              "scene5_map.bmp", "scene5_anim", 5, 43, 10, "scene5_levels.txt",
              918, 9, 0, NULL, 0, 976, 309);
          }
        }

#ifndef __DEMO
        if (!strcmp(res[resid].cParam[1], "SCENE6")) {
          if (iLanguageVersion == 4 && !CheckScenePresence(6)) {
            RunMenuCibron("buy.bmp");
            click = 0;
            i = lastabv;
            goto LEAVE_ANIMATION;
          }
          else {
            iActualScene = 6;
            RunStretchAnimation("scene6_map.bmp", 37, 495, p_ad);
            RunMenuSceneMap("Mmnew_game_scene6_map.txt", NULL, p_ad, cpu,
              "scene6_map.bmp", "scene6_anim", 6, 53, 10, "scene6_levels.txt",
              920, 8, 0, NULL, 0, 967, 211);
          }
        }
#endif

#ifdef __DEMO
        if (!strcmp(res[resid].cParam[1], "SCENE6")) {
          iActualScene = 6;
          RunStretchAnimation("scene6_map.bmp", 37, 495, p_ad);
          RunMenuSceneMap("Mmnew_game_scene6_map.txt", NULL, p_ad, cpu,
            "scene6_map.bmp", "scene6_anim", 6, 53, 6, "scene6_levels.txt",
            920, 8, 0, NULL, 0, 967, 211);
        }
#endif

        if (!strcmp(res[resid].cParam[1], "SCENE7")) {
          if (iLanguageVersion == 4 && !CheckScenePresence(7)) {
            RunMenuCibron("buy.bmp");
            click = 0;
            i = lastabv;
            goto LEAVE_ANIMATION;
          }
          else {
            iActualScene = 7;
            RunStretchAnimation("scene7_map.bmp", 616, 638, p_ad);
            RunMenuSceneMap("Mmnew_game_scene7_map.txt", NULL, p_ad, cpu,
              "scene7_map.bmp", "scene7_anim", 7, 63, 10, "scene7_levels.txt",
              11, 6, 0, NULL, 0, 8, 279);
          }
        }

        if (!strcmp(res[resid].cParam[1], "SCENE8")) {
          if (iLanguageVersion == 4 && !CheckScenePresence(8)) {
            RunMenuCibron("buy.bmp");
            click = 0;
            i = lastabv;
            goto LEAVE_ANIMATION;
          }
          else {
            iActualScene = 8;
            RunStretchAnimation("scene8_map.bmp", 836, 469, p_ad);
            RunMenuSceneMap("Mmnew_game_scene8_map.txt", NULL, p_ad, cpu,
              "scene8_map.bmp", "scene8_anim", 8, 73, 12, "scene8_levels.txt",
              15, 677, 0, NULL, 0, 12, 490);
          }
        }

        if (!strcmp(res[resid].cParam[1], "SCENE9")) {
          if (iLanguageVersion == 4 && !CheckScenePresence(9)) {
            RunMenuCibron("buy.bmp");
            click = 0;
            i = lastabv;
            goto LEAVE_ANIMATION;
          }
          else {
            iActualScene = 9;
            RunStretchAnimation("scene9_map.bmp", 836, 49, p_ad);
            RunMenuSceneMap("Mmnew_game_scene9_map.txt", NULL, p_ad, cpu,
              "scene9_map.bmp", "scene9_anim", 9, 85, 10, "scene9_levels.txt",
              918, 7, 0, NULL, 0, 971, 338);
          }
        }

        resid = -1;

        if (!iActualScene)
          key[K_ESC] = 1;

        if (key[K_ESC]) {
          for(i=0;i<lastcmd;i++) {
            if(res[i].iParam[0] == COM_BINDSOUND && res[i].iParam[5] != -1)
            {
              adas_Release_Source(PARTICULAR_SOUND_SOURCE, UNDEFINED_VALUE, res[i].iParam[5]);
              res[i].iParam[5] = -1;
            }
          }
          goto __QUIT;
        }
        else {
          for (i = 0; i < lastcmd; i++) {
            switch (res[i].iParam[0]) {
              case COM_DRAW:
                ddxDrawSurface(BackDC, res[i].iParam, 0);
                break;
            }
          }

          RunMenuNewGameSceneActivate(res);
          ddxBitBltDisplay(0, 0, 1024, 768, BackDC, 0, 0);
          ddxCleareSurface(BackDC);

          goto BEGIN_MENU_NEWGAMESCENE;
        }
      }

    //pokud prisel cas, tak provedu nahodne animace (podle jejich pravdepodobnosti)
    if (timercnt > 500) {
      timercnt = 0;

      for (i = 0; i < lastcmd; i++)
        if (res[i].iParam[0] == COM_RANDOMANIMATION)
          if (rand() % 200 <= res[i].iParam[1] &&
            strcmp(dir, res[i].cParam[0])) {
            CheckAnimation(&res[i], p_ad);
            AddAnimation(&res[i], p_ad, 0, 0);
          }
    }

/*		spracuj_spravy(0);
		ddxUpdateMouse();

		if(dim.dx || dim.dy)
			DisplayFrame();*/

    dwStop = timeGetTime();

    dwEplased += dwStop - dwStart;

    spracuj_spravy(0);
    ddxUpdateMouse();
    AnimationEvent(dwStop, p_ad);

    if (dim.tf1) {
      dim.t1 = 1;
      dim.tf1 = 0;
    }

    if (dim.tf2) {
      dim.t2 = 1;
      dim.tf2 = 0;
    }

    if (ddxRestore(p_ad)) {
      StopAll();
      FreeAnimations(res, RES_NUM);
      goto BEGIN_MENU_NEWGAMESCENE_BRUTAL;
    }
  }

__QUIT:

  ddxCleareSurface(FontDC);
  ddxCleareSurface(BackDC);
  ddxCleareSurface(CompositDC);

  cRestartMainMenu = 1;
  key[K_ESC] = 0;

  FreeAnimations(res, RES_NUM);
  free((void *) res);
}

void LoadSceneMap(int *pBmp, char *cSceneBmp, char *cSceneAnim, int iScene,
  int *iClock)
{
  int i;
  char text[256];
  APAK_HANDLE *hArchive = NULL;
  char cFile[MAX_PATH];
  char cDir[MAX_PATH];

  if (iLanguageVersion == 4) {
    GetPrivateProfileString("game", "bitmap_dir", "c:\\", cDir, MAX_PATH,
      ini_file);
    working_file_translate(cDir, MAX_PATH);
    sprintf(cFile, "%s%cscene%d.pak", cDir, DIR_SLASH, iScene);
    hArchive = apakopen(cFile, cDir, &i);

    if (!hArchive) {
      kprintf(1, "Nepodarilo se otevrit archiv [%s]\n", cFile);
      return;
    }
    else
      hArchive->pActualNode = hArchive->pRootNode->pNextNode;

    pBmp[0] = ddxLoadBitmap(cSceneBmp, hArchive);

    for (i = 1; i < 6; i++) {
      sprintf(text, "%s%d.bmp", cSceneAnim, i);
      pBmp[i] = ddxLoadBitmap(text, hArchive);
      DrawClock(iClock, i);
    }

    apakclose(&hArchive);
  }
  else {
    pBmp[0] = ddxLoadBitmap(cSceneBmp, pBmpArchive);

    for (i = 1; i < 6; i++) {
      sprintf(text, "%s%d.bmp", cSceneAnim, i);
      pBmp[i] = ddxLoadBitmap(text, pBmpArchive);
      DrawClock(iClock, i);
    }
  }

  switch (iScene) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 7:
      pBmp[6] = ddxLoadBitmap("level_green.bmp", pBmpArchive);
      pBmp[7] = ddxLoadBitmap("level_ok_green.bmp", pBmpArchive);
      pBmp[8] = ddxLoadBitmap("b1_green.bmp", pBmpArchive);
      pBmp[9] = ddxLoadBitmap("b2_green.bmp", pBmpArchive);
      pBmp[10] = ddxLoadBitmap("level_green_l.bmp", pBmpArchive);
      pBmp[11] = ddxLoadBitmap("level_ok_green_l.bmp", pBmpArchive);
      break;
    case 5:
    case 8:
      pBmp[6] = ddxLoadBitmap("level_brown.bmp", pBmpArchive);
      pBmp[7] = ddxLoadBitmap("level_ok_brown.bmp", pBmpArchive);
      pBmp[8] = ddxLoadBitmap("b1_brown.bmp", pBmpArchive);
      pBmp[9] = ddxLoadBitmap("b2_brown.bmp", pBmpArchive);
      pBmp[10] = ddxLoadBitmap("level_brown_l.bmp", pBmpArchive);
      pBmp[11] = ddxLoadBitmap("level_ok_brown_l.bmp", pBmpArchive);
      break;
    case 0:
      pBmp[6] = ddxLoadBitmap("level_tutorial.bmp", pBmpArchive);
      pBmp[7] = ddxLoadBitmap("level_ok_tutorial.bmp", pBmpArchive);
      pBmp[8] = ddxLoadBitmap("b1_blue.bmp", pBmpArchive);
      pBmp[9] = ddxLoadBitmap("b2_blue.bmp", pBmpArchive);
      pBmp[10] = ddxLoadBitmap("level_tutorial_l.bmp", pBmpArchive);
      pBmp[11] = ddxLoadBitmap("level_ok_tutorial_l.bmp", pBmpArchive);
      break;
    case 6:
    case 9:
    case 10:
    case 11:
      pBmp[6] = ddxLoadBitmap("level_blue.bmp", pBmpArchive);
      pBmp[7] = ddxLoadBitmap("level_ok_blue.bmp", pBmpArchive);
      pBmp[8] = ddxLoadBitmap("b1_blue.bmp", pBmpArchive);
      pBmp[9] = ddxLoadBitmap("b2_blue.bmp", pBmpArchive);
      pBmp[10] = ddxLoadBitmap("level_blue_l.bmp", pBmpArchive);
      pBmp[11] = ddxLoadBitmap("level_ok_blue_l.bmp", pBmpArchive);
      break;
    case 12:
      pBmp[6] = ddxLoadBitmap("level_yellow.bmp", pBmpArchive);
      pBmp[7] = ddxLoadBitmap("level_ok_yellow.bmp", pBmpArchive);
      pBmp[8] = ddxLoadBitmap("b1_yellow.bmp", pBmpArchive);
      pBmp[9] = ddxLoadBitmap("b2_yellow.bmp", pBmpArchive);
      pBmp[10] = ddxLoadBitmap("level_yellow_l.bmp", pBmpArchive);
      pBmp[11] = ddxLoadBitmap("level_ok_yellow_l.bmp", pBmpArchive);
      break;
  }

  DrawClock(iClock, 6);
}

void DrawLevelHint(int x, int y, int iLevel)
{
  int idx;
  char text[256];

  sprintf(text, "t_%d.bmp", iLevel - 200);

  idx = ddxLoadBitmap(text, pBmpArchive);

  if (idx == -1)
    return;

  ddxBitBlt(FontDC, x, y, ddxGetWidth(idx), ddxGetHight(idx), idx, 0, 0);
  ddxTransparentBlt(BackDC, x, y, ddxGetWidth(idx), ddxGetHight(idx), idx, 0,
    0, ddxGetWidth(idx), ddxGetHight(idx), TRANSCOLOR);

  ddxReleaseBitmap(idx);
}

void CreateLevelButton(int x, int y, CMD_LINE * res, int *lastcmd, int *pBmp,
  char *cMessage, char bDone, int iDificulty, char bTuturial, int iLevel)
{
  int ii;
  char text[1024];

  //Draw(1,0,0)
  sprintf(text, "Draw(%d,%d,%d)", pBmp[6 + bDone], x, y);
  Parse_LineT(text, res[*lastcmd].iParam, 6, res[*lastcmd].cParam[0],
    res[*lastcmd].cParam[1]);
  res[*lastcmd].iLayer = 1;
  (*lastcmd)++;

  /*if(bTuturial)
     DrawLevelHint(x + ddxGetWidth(pBmp[6+bDone]), y, iLevel); */

  //OnAbove(16,661,100,748, NO_EXEPTION, NO_EXEPTION)
  sprintf(text, "OnAbove(%d,%d,%d,%d, %s, NO_EXEPTION)", x, y,
    x + ddxGetWidth(pBmp[6 + bDone]), y + ddxGetHight(pBmp[6 + bDone]),
    cMessage);

  Parse_LineT(text, res[*lastcmd].iParam, 6, res[*lastcmd].cParam[0],
    res[*lastcmd].cParam[1]);
  res[*lastcmd].iLayer = 1;

  res[*lastcmd].uiTimerID = 0;
  res[*lastcmd].iLastfrm = 1;
  res[*lastcmd].iCounter = 0;

  res[*lastcmd].iAnim[0][0] = 0;
  res[*lastcmd].iAnim[0][1] = pBmp[6 + bDone];
  res[*lastcmd].iAnim[0][2] = x;
  res[*lastcmd].iAnim[0][3] = y;
  res[*lastcmd].iAnim[0][4] = 0;
  res[*lastcmd].iAnim[0][5] = 1;

  for (ii = 6; ii < 13; ii++)
    res[*lastcmd].iAnim[0][ii] = -1;

  res[*lastcmd].iAnim[1][0] = 1;
  res[*lastcmd].iAnim[1][1] = pBmp[10 + bDone];
  res[*lastcmd].iAnim[1][2] = x;
  res[*lastcmd].iAnim[1][3] = y;
  res[*lastcmd].iAnim[1][4] = 50;
  res[*lastcmd].iAnim[1][5] = 2;

  for (ii = 6; ii < 13; ii++)
    res[*lastcmd].iAnim[1][ii] = -1;

  res[*lastcmd].iAnim[2][0] = 2;
  res[*lastcmd].iAnim[2][1] = pBmp[10 + bDone];
  res[*lastcmd].iAnim[2][2] = x;
  res[*lastcmd].iAnim[2][3] = y;
  res[*lastcmd].iAnim[2][4] = 50;
  res[*lastcmd].iAnim[2][5] = -1;

  for (ii = 6; ii < 13; ii++)
    res[*lastcmd].iAnim[2][ii] = -1;

  res[*lastcmd].iLayer = 1;
  (*lastcmd)++;

  if (!bTuturial) {

    if (!iDificulty) {
      //BindAnimation(118, -1, -1, -1, televize5.txt, televize.txt)
      sprintf(text,
        "BindAnimation(118, -1, -1, -1, televize5.txt, televize.txt)");
      Parse_LineT(text, res[*lastcmd].iParam, 6, res[*lastcmd].cParam[0],
        res[*lastcmd].cParam[1]);
      (*lastcmd)++;

      //BindAnimation(118, -1, -1, -1, televize5.txt, televize.txt)
      sprintf(text,
        "BindAnimation(118, -1, -1, -1, televize6.txt, televize.txt)");
      Parse_LineT(text, res[*lastcmd].iParam, 6, res[*lastcmd].cParam[0],
        res[*lastcmd].cParam[1]);
      (*lastcmd)++;
    }
    else if (iDificulty == 1) {
      //BindAnimation(118, -1, -1, -1, televize5.txt, televize.txt)
      sprintf(text,
        "BindAnimation(118, -1, -1, -1, televize7.txt, televize.txt)");
      Parse_LineT(text, res[*lastcmd].iParam, 6, res[*lastcmd].cParam[0],
        res[*lastcmd].cParam[1]);
      (*lastcmd)++;

      //BindAnimation(118, -1, -1, -1, televize5.txt, televize.txt)
      sprintf(text,
        "BindAnimation(118, -1, -1, -1, televize8.txt, televize.txt)");
      Parse_LineT(text, res[*lastcmd].iParam, 6, res[*lastcmd].cParam[0],
        res[*lastcmd].cParam[1]);
      (*lastcmd)++;
    }
    else if (iDificulty == 2) {
      //BindAnimation(118, -1, -1, -1, televize5.txt, televize.txt)
      sprintf(text,
        "BindAnimation(118, -1, -1, -1, televize9.txt, televize.txt)");
      Parse_LineT(text, res[*lastcmd].iParam, 6, res[*lastcmd].cParam[0],
        res[*lastcmd].cParam[1]);
      (*lastcmd)++;
    }
    else if (iDificulty == 3) {

      //BindAnimation(118, -1, -1, -1, televize5.txt, televize.txt)
      sprintf(text,
        "BindAnimation(118, -1, -1, -1, televize10.txt, televize.txt)");
      Parse_LineT(text, res[*lastcmd].iParam, 6, res[*lastcmd].cParam[0],
        res[*lastcmd].cParam[1]);
      (*lastcmd)++;
    }
    else {
      //BindAnimation(118, -1, -1, -1, televize5.txt, televize.txt)
      sprintf(text,
        "BindAnimation(118, -1, -1, -1, televize11.txt, televize.txt)");
      Parse_LineT(text, res[*lastcmd].iParam, 6, res[*lastcmd].cParam[0],
        res[*lastcmd].cParam[1]);
      (*lastcmd)++;

      //BindAnimation(118, -1, -1, -1, televize5.txt, televize.txt)
      sprintf(text,
        "BindAnimation(118, -1, -1, -1, televize12.txt, televize.txt)");
      Parse_LineT(text, res[*lastcmd].iParam, 6, res[*lastcmd].cParam[0],
        res[*lastcmd].cParam[1]);
      (*lastcmd)++;
    }

    //BindExitAnimation(114,115,116,117, televize.txt)
    sprintf(text, "BindExitAnimation(114,115,116,117, televize.txt)");
    Parse_LineT(text, res[*lastcmd].iParam, 6, res[*lastcmd].cParam[0],
      res[*lastcmd].cParam[1]);
    (*lastcmd)++;

  }
/*	else
	{
		//OnAbove(16,661,100,748, NO_EXEPTION, NO_EXEPTION)
		sprintf(text,"OnAbove(%d,%d,%d,%d, %s, NO_EXEPTION)", x, y, x + ddxGetWidth(pBmp[6+bDone]), y + ddxGetHight(pBmp[6+bDone]), cMessage);

		Parse_LineT(text, res[*lastcmd].iParam, 6, res[*lastcmd].cParam[0], res[*lastcmd].cParam[1]);
		res[*lastcmd].iLayer = 1;
		(*lastcmd)++;
	}*/

  //OnClick(16,661,100,748, quit_gamec.txt, EXIT)
  sprintf(text, "OnClick(%d,%d,%d,%d, NO_EXEPTION, %s)", x, y,
    x + ddxGetWidth(pBmp[6 + bDone]), y + ddxGetHight(pBmp[6 + bDone]),
    cMessage);

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

void CreateLevelButtons(CMD_LINE * res, int *lastcmd, int *pBmp, int iScene,
  int iLevelStart, int iNumOfLevels, char *cLevelList, char bTutorial)
{
  char text[256];
  int r[4];
  int i;
  FILE *file;

  file = aopen(pDataArchive, cLevelList, "r");

  if (!file)
    return;

  for (i = iLevelStart; i < iLevelStart + iNumOfLevels; i++) {
    Parse_AnimLine(file, r, 4);

    if (bTutorial && i != iLevelStart)
      if (!pPlayerProfile.cLevel[i - 1])
        break;

    CreateLevelButton(r[1], r[2], res, lastcmd, pBmp, itoa(r[0], text, 10),
      pPlayerProfile.cLevel[i], r[3], bTutorial, i);
  }

  aclose(file);
}

int LoadTV(void)
{
  int c = 0, i = -1;
  char text[256];
  FILE *file;

  file = aopen(pBmpArchive, "tvload.txt", "r");

  if (!file)
    return -1;

  while (!aeof(file)) {
    memset(text, 0, 256);

    agets(text, 256, file);
    if (text[0]) {
      newline_cut(text);

      if (!c)
        i = ddxLoadBitmap(text, pBmpArchive);
      else
        ddxLoadBitmap(text, pBmpArchive);

      c++;
    }
  }

  aclose(file);

  pBmpArchive->bError = 0;

  return i;
}

void CorrectTV(CMD_LINE * res, int iAnim, int iLast, int x, int y, int idx,
  int iScene)
{
  int tvcoridx = -1;
  int i, d;

  switch (iScene) {
    case 5:
    case 8:
      tvcoridx = ddxLoadBitmap("tvcor_brown.bmp", pBmpArchive);
      break;
    case 0:
    case 6:
    case 9:
    case 10:
    case 11:
      tvcoridx = ddxLoadBitmap("tvcor_blue.bmp", pBmpArchive);
      break;
    case 12:
      tvcoridx = ddxLoadBitmap("tvcor_yellow.bmp", pBmpArchive);
      break;
  }

  for (i = 0; i < iLast; i++) {
    res[iAnim].iAnim[i][2] = x;
    res[iAnim].iAnim[i][3] = y;
    res[iAnim].iAnim[i][1] += (idx - 250);

    d = res[iAnim].iAnim[i][1];

    if (tvcoridx != -1 && res[iAnim].iAnim[i][0] != -1)
      ddxTransparentBlt(d, 0, 0, ddxGetWidth(d), ddxGetHight(d), tvcoridx,
        0, 0, ddxGetWidth(tvcoridx), ddxGetHight(tvcoridx), TRANSCOLOR);

    //kprintf(1, "tv corrected to %d", res[iAnim].iAnim[i][1]);
  }

  if (tvcoridx != -1)
    ddxReleaseBitmap(tvcoridx);
}

void CreateBackButton(CMD_LINE * res, int *lastcmd, int sidx1, int sidx2,
  int xBack, int yBack)
{
  int lcmd = *lastcmd;

  int i, ii;

  for (i = 0; i < lcmd; i++)
    if (res[i].iParam[0] == COM_ONABOVE) {
      res[i].uiTimerID = 0;
      res[i].iLastfrm = 1;
      res[i].iCounter = 0;

      /*
         (0, 123, 0, 640, 0, 1)
         (1, 124, 0, 640, 25, 2)
         (2, 124, 0, 640, 25, -1)
       */
      res[i].iAnim[0][0] = 0;
      res[i].iAnim[0][1] = sidx1;
      res[i].iAnim[0][2] = xBack;
      res[i].iAnim[0][3] = yBack;
      res[i].iAnim[0][4] = 0;
      res[i].iAnim[0][5] = 1;

      for (ii = 6; ii < 13; ii++)
        res[i].iAnim[0][ii] = -1;

      res[i].iAnim[1][0] = 1;
      res[i].iAnim[1][1] = sidx2;
      res[i].iAnim[1][2] = xBack;
      res[i].iAnim[1][3] = yBack;
      res[i].iAnim[1][4] = 50;
      res[i].iAnim[1][5] = 2;

      for (ii = 6; ii < 13; ii++)
        res[i].iAnim[1][ii] = -1;

      res[i].iAnim[2][0] = 2;
      res[i].iAnim[2][1] = sidx2;
      res[i].iAnim[2][2] = xBack;
      res[i].iAnim[2][3] = yBack;
      res[i].iAnim[2][4] = 50;
      res[i].iAnim[2][5] = -1;

      for (ii = 6; ii < 13; ii++)
        res[i].iAnim[2][ii] = -1;

      res[i].iLayer = 0;

      return;
    }
}

void DrawClock(int *iClock, int i)
{
  if (iClock[i] == -1)
    return;

  spracuj_spravy(0);
  ddxSetCursorSurface(iClock[i]);
  ddxUpdateMouse();
  DisplayFrame();
}

int LoadClock(int *iClock)
{
  int c = 0, i = -1, t;
  char text[256];
  FILE *file;

  file = aopen(pBmpArchive, "loadclock.txt", "r");

  if (!file)
    return -1;

  while (!aeof(file)) {
    ZeroMemory(text, 256);

    agets(text, 256, file);
    if (!text[0])
      break;

    newline_cut(text);

    if (!c) {
      i = ddxLoadBitmap(text, pBmpArchive);
      iClock[c] = i;
    }
    else {
      t = ddxLoadBitmap(text, pBmpArchive);
      iClock[c] = t;
    }

    c++;
  }

  aclose(file);

  pBmpArchive->bError = 0;

  return i;
}

/*void MenuInitHint(char bTutorial)
{
	if(bTutorial)
		_2d_hint.iSurface = ddxLoadBitmap("hint_frame.bmp", pBmpArchive);
	else
		_2d_hint.iSurface = -1;

	_2d_hint.iBSurface = -1;
}

void DrawHint(int x, int y)
{
	RECT r;

	if(_2d_hint.iSurface == -1)
		return;

	_2d_hint.iBSurface = ddxCreateSurface(ddxGetWidth(_2d_hint.iSurface), ddxGetHight(_2d_hint.iSurface), ddxFindFreeSurface());

	if(_2d_hint.iBSurface == -1)
		return;

	if(x + ddxGetWidth(_2d_hint.iSurface) > 1014)
		x = 1014 - ddxGetWidth(_2d_hint.iSurface);

	_2d_hint.x = x;
	_2d_hint.y = y;

	ddxBitBlt(_2d_hint.iBSurface, 0, 0, ddxGetWidth(_2d_hint.iBSurface), ddxGetHight(_2d_hint.iBSurface), HDC2DD, x, y);

	ddxBitBlt(CompositDC, x, y, ddxGetWidth(_2d_hint.iSurface), ddxGetHight(_2d_hint.iSurface), _2d_hint.iSurface, 0, 0);

	ddxBitBltDisplay(x, y, ddxGetWidth(_2d_hint.iSurface), ddxGetHight(_2d_hint.iSurface), _2d_hint.iSurface, 0, 0);

	r.left = x;
	r.top = y;
	r.right = ddxGetWidth(_2d_hint.iSurface);
	r.bottom = ddxGetHight(_2d_hint.iSurface);

	_2d_Add_RectItem(&rline, r, 2);

	_2d_hint.bUsed = 1;
}

void SetHintRect(void)
{
	RECT r;

	if(!_2d_hint.bUsed)
		return;
	
	r.left = _2d_hint.x;
	r.top = _2d_hint.y;
	r.right = ddxGetWidth(_2d_hint.iSurface);
	r.bottom = ddxGetHight(_2d_hint.iSurface);

	_2d_Add_RectItem(&rline, r, 2);
}

void UpdateHintBackup(void)
{
	if(!_2d_hint.bUsed)
		return;

	if(_2d_hint.iSurface == -1)
		return;

	ddxBitBlt(CompositDC, _2d_hint.x, _2d_hint.y, ddxGetWidth(_2d_hint.iSurface), ddxGetHight(_2d_hint.iSurface), _2d_hint.iSurface, 0, 0);
}

void ReleaseHint(char bFull, char bRestore)
{
	if(_2d_hint.iSurface != -1 && bFull)
	{
		ddxReleaseBitmap(_2d_hint.iSurface);
		_2d_hint.iSurface = -1;
	}

	if(_2d_hint.iBSurface != -1)
	{

		if(bRestore)
		{
			RECT r;
			
			r.left = _2d_hint.x;
			r.top = _2d_hint.y;
			r.right = r.left + ddxGetWidth(_2d_hint.iBSurface);
			r.bottom = r.top + ddxGetHight(_2d_hint.iBSurface);

			ddxFillRect(CompositDC, &r, RGB(255, 0, 255));

			ddxBitBltDisplay(_2d_hint.x, _2d_hint.y, ddxGetWidth(_2d_hint.iBSurface), ddxGetHight(_2d_hint.iBSurface),
							 _2d_hint.iBSurface, 0, 0);
		}

		ddxReleaseBitmap(_2d_hint.iBSurface);
		_2d_hint.iBSurface = -1;
	}

	_2d_hint.bUsed = 0;
}
*/

void RunMenuSceneMap(char *p_File_Name, HWND hWnd, AUDIO_DATA * p_ad, int cpu,
  char *cSceneBmp, char *cSceneAnim, int iScene, int iLevelStart,
  int iNumOfLevels, char *cLevelList, int xTV, int yTV, char bLoadGame,
  char *cSaveFile, char bTutorial, int xBack, int yBack)
{
  DWORD dwEplased = 0, dwStart, dwStop;

  FILE *file;
  char dir[256];
  int lastcmd, lastanm, i;
  CMD_LINE *res = NULL;
  int lastabv = -1;
  char in, click = 0;
  int anmid = -1, resid = -1, anbind = -1;
  int bind;
  int iBmp[12];
  int iTV = -1;
  int iClock[12];
  int cc = 0;
  char cscenemap[64];
  char csceneanim[64];
  char csrriptname[64];
  char cscenelevels[64];
  char bReload = 0;

  ZeroMemory(&_2d_hint, sizeof(_2D_HINT));

  for (i = 0; i < 12; i++)
    iClock[i] = -1;

  pPlayerProfile.cScene[iScene] = 1;
  pr_SaveProfile(&pPlayerProfile);

  ////////////////////////////LOAD GAME/////////////////////////////////////////
  if (bLoadGame) {
    sprintf(dir, "%s.lv6", cSaveFile);

    if (RunLevel(NULL, p_ad, cpu, dir, "LOAD_GAME") == 1) {
      pPlayerProfile.cLevel[iActualLevel] = 1;
      pr_SaveProfile(&pPlayerProfile);
    }

    for (i = 0; i < 32; i++)
      anm[i].cmd = NULL;

    //TODO  -       pokud se jedna o tutorial, detskou ci custom, tak to crushne .... proc?
  }

  ////////////////////////////LOAD GAME/////////////////////////////////////////

  LoadClock(iClock);

  if (iClock[0] != -1) {
    ddxResizeCursorBack(iClock[0]);
    DrawClock(iClock, 0);
  }

  for (i = 0; i < 12; i++)
    iBmp[i] = -1;

  bBackDC = 0;

  res = (CMD_LINE *) malloc(RES_NUM * sizeof(CMD_LINE));

  if (!res) {
    kprintf(1, "RunMenuSceneMap: Out of Memory");
    return;
  }

BRUTAL_RESTART_SCENE_MAP_MENU:

  bReload = 1;
  bBackDC = 0;

  _2d_Clear_RectLine(&rline);

  ddxSetFlip(TRUE);

  cc = 0;
  iTV = -1;
  anmid = -1;
  resid = -1;
  anbind = -1;

  ZeroMemory(res, RES_NUM * sizeof(CMD_LINE));

  GetRunMenuNewGameSceneLoadGame(csrriptname, cscenemap, csceneanim,
    cscenelevels, &iLevelStart, &iNumOfLevels, &xTV, &yTV, &bTutorial, &xBack,
    &yBack);

  LoadSceneMap(iBmp, cscenemap, csceneanim, iActualScene, iClock);

  ddxCleareSurface(CompositDC);
  ddxCleareSurface(FontDC);
  ddxCleareSurface(BackDC);

  for (bind = 0; bind < RES_NUM; bind++) {
    for (lastcmd = 0; lastcmd < 200; lastcmd++) {
      res[bind].iAnim[lastcmd][0] = -1;
      res[bind].iAnim[lastcmd][11] = -1;
    }

    for (in = 0; in < 6; in++)
      res[bind].iParam[(int)in] = -1;

    res[bind].pCmdLine = res;
  }

  lastcmd = 0;
  timercnt = 0;

  GetPrivateProfileString("game", "data_dir", "c:\\", dir, 256, ini_file);
  working_file_translate(dir, 256);
  chdir((dir));

  //natadhe skript menu
  LoadMenuScript(csrriptname, res, &lastcmd);

  DrawClock(iClock, 7);
  in = 0;

  ddxBitBlt(BackDC, 0, 0, 1024, 768, iBmp[0], 0, 0);

  CreateLevelButtons(res, &lastcmd, iBmp, iActualScene, iLevelStart,
    iNumOfLevels, cscenelevels, bTutorial);

  DrawClock(iClock, 8);
  if (!bTutorial) {
    iTV = LoadTV();

    if (iTV == -1)
      kprintf(1, "Saaakra ... neporalo se nahrat bmpka televize");
  }

  DrawClock(iClock, 9);

  // privede prikazy, ketere se maji provest na zacatku a, kresleni, flip,
  // animace na OnAbove
  for (i = 0; i < lastcmd; i++) {
    lastanm = 0;

    switch (res[i].iParam[0]) {
      case COM_DRAW:
        if (!res[i].iLayer)
          ddxDrawSurface(BackDC, res[i].iParam, 0);
        else {
          ddxDrawSurfaceColorKey(BackDC, res[i].iParam, 2, TRANSCOLOR);
          ddxDrawSurface(FontDC, res[i].iParam, 3);
        }
        break;
      case COM_RANDOMANIMATION:
      case COM_ONCLICK:
      case COM_ONABOVE:
      case COM_RUNANIMATION:
      case COM_BINDEXITANIMATION:
      case COM_BINDANIMATION:
        //nahrati animace k udalosti OnAbove
        file = aopen(pDataArchive, res[i].cParam[0], "r");
        if (file) {
          while (!aeof(file)) {
            Parse_AnimLine(file, res[i].iAnim[lastanm], 18);

            if (!cc)
              res[i].iAnim[lastanm][1] = iBmp[res[i].iAnim[lastanm][1]];

            lastanm++;
          }

          if (!cc)
            res[i].iLayer = 10;
          else if (!bTutorial)
            CorrectTV(res, i, lastanm, xTV, yTV, iTV, iActualScene);

          aclose(file);
        }

        cc++;

        break;
    }
  }

  DrawClock(iClock, 10);

  CreateBackButton(res, &lastcmd, iBmp[8], iBmp[9], xBack, yBack);

//      MenuInitHint(bTutorial);

  ddxResizeCursorBack(0);
  ddxSetCursorSurface(0);

  for (i = 0; i < 12; i++)
    if (iClock[i] != -1) {
      ddxReleaseBitmap(iClock[i]);
      iClock[i] = -1;
    }

  ddxBitBltDisplay(0, 0, 1024, 768, BackDC, 0, 0);
  ddxCleareSurface(BackDC);

//BEGIN_MENU_NEWGAMESCENE:

  for (i = 0; i < lastcmd; i++)
    if (res[i].iParam[0] == COM_RUNANIMATION) {
      int iWave = AddAnimation(&res[i], p_ad, 0, 0);

      if (iWave != -1) {
        if (res[i + 1].iParam[0] == COM_BINDSOUND)
          anm[iWave].iWave = res[i + 1].iParam[5] =
            mPlaySound(&res[i + 1], p_ad, 2);
      }
    }

  dim.t1 = 0;
  dim.t2 = 0;
  dim.dx = 0;
  dim.dy = 0;
  anmid = -1;
  resid = -1;
  anbind = -1;
  bind = -1;
  lastabv = -1;
  in = 0;

  spracuj_spravy(0);

  while (!key[K_ESC]) {
    dwStart = timeGetTime();

    //pohnul mysi
    if (dim.dx || dim.dy) {
      //dostala se mys do akcni oblasti (OnAbove)?
      if (!click)
        for (i = 0; i < lastcmd; i++)
          if (res[i].iParam[0] == COM_ONABOVE) {
            if ((dim.x >= res[i].iParam[1]) &&
              (dim.x <= res[i].iParam[3]) &&
              (dim.y >= res[i].iParam[2]) && (dim.y <= res[i].iParam[4])) {
              //spusteni animace v OnAbove
              if (i != lastabv) {
                if (in) {
                  if (res[lastabv].iAnim[0][0] != -1) {
                    Stop(&res[lastabv]);

                    if (!res[lastabv].iLayer) {
                      ddxDrawDisplay(res[lastabv].iAnim[0], 0);
                      ddxDrawSurface(CompositDC, res[lastabv].iAnim[0], 3);
                    }
                    else {
                      ddxDrawDisplayColorKey(res[lastabv].iAnim[0], 0,
                        TRANSCOLOR);
                      ddxDrawSurface(FontDC, res[lastabv].iAnim[0], 3);
                    }
                  }

//                                                      ReleaseHint(0, 1);
                }

                CheckAnimation(&res[i], p_ad);

                lastabv = i;
                AddAnimation(&res[i], p_ad, 0, 1);
                in = 1;

                bind = ChooseBidedAnimation(res, i + 1, p_ad);

                if (bind != -1) {
                  CheckAnimation(&res[bind], p_ad);
                  AddAnimation(&res[bind], p_ad, 1, 1);
                  anbind = bind;

                  mPlaySound(&res[bind], p_ad, 0);
                }

                strcpy(dir, res[i].cParam[1]);

/*						if(bTutorial)
							DrawHint(res[lastabv].iAnim[0][2], res[lastabv].iAnim[0][3] - 35);*/
              }
            }
            else if (lastabv == i) {
              // odesel z oblasti, ktera byla aktivni -> stop animace                                 
              // a odznaceni oblasti
              Stop(&res[i]);

              if (res[i].iAnim[0][0] != -1) {
                if (!res[i].iLayer) {
                  ddxDrawDisplay(res[i].iAnim[0], 0);
                  ddxDrawSurface(CompositDC, res[i].iAnim[0], 3);
                }
                else {
                  ddxDrawDisplayColorKey(res[i].iAnim[0], 0, TRANSCOLOR);
                  ddxDrawSurface(FontDC, res[i].iAnim[0], 3);
                }
              }

              bind = ChooseBidedExitAnimation(res, i + 1, p_ad);

              if (bind != -1) {
                int iAnim;

                if (anbind != -1) {
                  Stop(&res[anbind]);

                  if (!res[anbind].iLayer) {
                    ddxDrawDisplay(res[anbind].iAnim[0], 0);
                    ddxDrawSurface(CompositDC, res[anbind].iAnim[0], 3);
                  }
                  else {
                    ddxDrawDisplayColorKey(res[anbind].iAnim[0], 0,
                      TRANSCOLOR);
                    ddxDrawSurface(FontDC, res[anbind].iAnim[0], 3);
                  }
                }

                iAnim = AddAnimation(&res[bind], p_ad, 1, 1);

                if (iAnim != -1)
                  anm[iAnim].iWave = mPlaySound(&res[bind], p_ad, 2);
              }

              lastabv = -1;
              anbind = -1;
              in = 0;

              strcpy(dir, "");

//                                              ReleaseHint(0, 1);
            }

            //dim.dx = 0;
            //dim.dy = 0;
          }
    }

    //stlacil leve tlacitko
    if (dim.t1 && !click) {
      //dostala se mys do akcni oblasti (OnClick)?
      for (i = 0; i < lastcmd; i++)
        if (res[i].iParam[0] == COM_ONCLICK)
          if ((dim.x >= res[i].iParam[1]) &&
            (dim.x <= res[i].iParam[3]) &&
            (dim.y >= res[i].iParam[2]) && (dim.y <= res[i].iParam[4])) {
            if (res[i].iAnim[0][0] >= 0) {
              //pokud je animace, tak ji spust
              anmid = AddAnimation(&res[i], p_ad, 0, 1);

              if (res[i + 1].iParam[0] == COM_BINDSOUND)
                mPlaySound(&res[i + 1], p_ad, 1);

              resid = i;
              click = 1;
            }
            else {
              if (res[i + 1].iParam[0] == COM_BINDSOUND)
                mPlaySound(&res[i + 1], p_ad, 1);

              resid = i;
              click = 1;
              anmid = 31;
            }
          }

      dim.t1 = 0;
    }

    //provedeni akce po animaci menu
    if (click)
      if (!anm[anmid].cmd) {
        click = 0;

        StopAll();

        if (!strcmp(res[resid].cParam[1], "EXIT") ||
          !strcmp(res[resid].cParam[1], "CANCEL")) {
          key[K_ESC] = 1;
          cRestartMainMenu = 1;
          //break;
        }
        else {
          char cLevel[256];

          iActualLevel = iLevelStart + atoi(res[resid].cParam[1]) - 1;

//                              ReleaseHint(1, 0);

          kprintf(1, "Kliknuto na level : %s", res[resid].cParam[1]);

          sprintf(cLevel, "level%d.lv6", iActualLevel);

          if (RunLevel(NULL, p_ad, cpu, cLevel, "RUN_LEVEL") == 1) {
            pPlayerProfile.cLevel[iActualLevel] = 1;
            pr_SaveProfile(&pPlayerProfile);
          }

          for (i = 0; i < 32; i++)
            anm[i].cmd = NULL;

          bReload = 0;

          if (bNewScene)
            key[K_ESC] = 1;
          else
            goto BRUTAL_RESTART_SCENE_MAP_MENU;
        }

        resid = -1;

        if (key[K_ESC]) {
          for(i=0;i<lastcmd;i++) {
            if(res[i].iParam[0] == COM_BINDSOUND && res[i].iParam[5] != -1)
            {
              adas_Release_Source(PARTICULAR_SOUND_SOURCE, UNDEFINED_VALUE, res[i].iParam[5]);
              res[i].iParam[5] = -1;
            }
            goto __QUIT;
          }
        }
      }

    //pokud prisel cas, tak provedu nahodne animace (podle jejich pravdepodobnosti)
    if (timercnt > 500) {
      timercnt = 0;

      for (i = 0; i < lastcmd; i++)
        if (res[i].iParam[0] == COM_RANDOMANIMATION)
          if (rand() % 200 <= res[i].iParam[1] &&
            strcmp(dir, res[i].cParam[0])) {
            CheckAnimation(&res[i], p_ad);
            AddAnimation(&res[i], p_ad, 0, 0);
          }
    }

    dwStop = timeGetTime();

    dwEplased += dwStop - dwStart;

    spracuj_spravy(0);
    ddxUpdateMouse();
    AnimationEvent(dwStop, p_ad);

    if (dim.tf1) {
      dim.t1 = 1;
      dim.tf1 = 0;
    }

    if (dim.tf2) {
      dim.t2 = 1;
      dim.tf2 = 0;
    }

    if (ddxRestore(p_ad)) {
      StopAll();
      FreeAnimations(res, RES_NUM);
      goto BRUTAL_RESTART_SCENE_MAP_MENU;
    }
  }

__QUIT:

	adas_Release_Source(-1, ALL_TYPES, UNDEFINED_VALUE);
	adas_Release_Source(ALL_SOUND_SOURCES, ALL_TYPES,UNDEFINED_VALUE); 

  StopAll();

  if (bReload) {
    for (i = 0; i < 12; i++)
      ddxReleaseBitmap(iBmp[i]);

    if (!bTutorial)
      for (i = iTV; i < iTV + 91; i++)
        ddxReleaseBitmap(i);
  }

  ddxCleareSurface(FontDC);
  ddxCleareSurface(BackDC);
  ddxCleareSurface(CompositDC);

  cRestartMainMenu = 1;
  key[K_ESC] = 0;

  FreeAnimations(res, RES_NUM);
  free((void *) res);
}

int RunMenuNewGame(char *p_File_Name, HWND hWnd, AUDIO_DATA * p_ad, int cpu)
{
  DWORD dwEplased = 0, dwStart, dwStop;
  int ret = 0;
  int c = 0;

  CONTROL_LIST_ITEM citem[CLIST_ITEMC];

//      FILE    *file;
  char dir[256];
  int lastcmd, lastanm, i;
  CMD_LINE *res = NULL;
  int lastabv = -1;
  char in, click = 0;
  int anmid = -1, resid = -1, anbind = -1;
  int bind;
  char bDone = 0;
  int inumofitems = 0;

  res = (CMD_LINE *) malloc(RES_NUM * sizeof(CMD_LINE));

  if (!res) {
    kprintf(1, "RunMenuNewGame: Out of Memory");
    return 0;
  }
  else
    ZeroMemory(res, RES_NUM * sizeof(CMD_LINE));

  bBackDC = 0;

  ddxCleareSurface(BackDC);
  ddxBitBlt(BackDC, 0, 0, ddxGetWidth(BackDC), ddxGetHight(BackDC), FontDC, 0, 0);
  ddxCleareSurface(FontDC);

  //kprintf(1, "bitblt");
  ZeroMemory(citem, CLIST_ITEMC * sizeof(CONTROL_LIST_ITEM));

  for (bind = 0; bind < RES_NUM; bind++) {
    for (lastcmd = 0; lastcmd < 200; lastcmd++) {
      res[bind].iAnim[lastcmd][0] = -1;
      res[bind].iAnim[lastcmd][11] = -1;
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

  fn_Set_Font(cFontFile[0]);
  fn_Load_Bitmaps();

  CreateFontAnimations(res, &lastcmd);

  // privede prikazy, ketere se maji provest na zacatku a, kresleni, flip,
  // animace na OnAbove
  for (i = 0; i < lastcmd; i++) {
    lastanm = 0;

    switch (res[i].iParam[0]) {
      case COM_DRAW:
        {
          if (!c) {
            RECT rr;

            ddxTransparentBlt(BackDC, res[i].iParam[2], res[i].iParam[3],
              ddxGetWidth(res[i].iParam[1]), ddxGetHight(res[i].iParam[1]),
              res[i].iParam[1], 0, 0, ddxGetWidth(res[i].iParam[1]),
              ddxGetHight(res[i].iParam[1]), TRANSCOLOR);

            rr.left = res[i].iParam[2];
            rr.top = res[i].iParam[3];
            rr.right = ddxGetWidth(res[i].iParam[1]);
            rr.bottom = ddxGetHight(res[i].iParam[1]);

            _2d_Add_RectItem(&rline, rr, 0);
          }
          else
            ddxDrawSurfaceColorKey(BackDC, res[i].iParam, 2, TRANSCOLOR);

          c++;
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

  if (co_Load_Graphic(1)) {
    int xx, sel = -1;
    RECT r;

    r.left = 299;
    r.top = 209;
    r.right = 743;
    r.bottom = 359;

    co_Set_Text_Center(BackDC, "##mainmenu_player_name", 0, r);

    r.top = 359;
    r.bottom = 509;
    co_Set_Text_Center(BackDC, "##mainmenu_new_player_name", 0, r);

    fn_Release_Font(1);
    fn_Set_Font(cFontFile[2]);
    fn_Load_Bitmaps();

    citem[0].p_edit = co_Create_Edit(BackDC, 360, 470, 0);
    citem[0].bActive = 1;

    citem[1].p_combo = co_Create_Combo(BackDC, 360, 320, 100, 0);
    citem[1].bActive = 1;

    xx = FillComboProfiles(citem[1].p_combo, &sel);

    if (xx > 5)
      co_Combo_Set_Params(citem[1].p_combo, 5);
    else
      co_Combo_Set_Params(citem[1].p_combo, xx);

    kprintf(1, "set_sel %d", sel);

    co_Combo_Set_Sel(BackDC, citem[1].p_combo, sel);

    inumofitems = xx;

    if (!inumofitems)
      citem[0].p_edit->bActive = 1;
  }

  ddxTransparentBltDisplay(0, 0, 1024, 768, BackDC, 0, 0, 1024, 768,
    TRANSCOLOR);
  bBackDC = 1;

BEGIN_MENU_NEWGAME:

  for (i = 0; i < lastcmd; i++)
    if (res[i].iParam[0] == COM_RUNANIMATION) {
      int iWave = AddAnimation(&res[i], p_ad, 0, 0);

      if (iWave != -1) {
        if (res[i + 1].iParam[0] == COM_BINDSOUND)
          anm[iWave].iWave = res[i + 1].iParam[5] =
            mPlaySound(&res[i + 1], p_ad, 2);
      }
    }

  dim.t1 = 0;
  dim.t2 = 0;
  dim.dx = 0;
  dim.dy = 0;
  anmid = -1;
  resid = -1;
  anbind = -1;
  bind = -1;
  lastabv = -1;
  in = 0;

  spracuj_spravy(0);

  while (!bDone) {
    dwStart = timeGetTime();

    //pohnul mysi
    if (dim.dx || dim.dy) {
      //dostala se mys do akcni oblasti (OnAbove)?
      if (!click) {
        for (i = 0; i < lastcmd; i++) {
          if (res[i].iParam[0] == COM_ONABOVE) {
            if ((dim.x >= res[i].iParam[1]) &&
              (dim.x <= res[i].iParam[3]) &&
              (dim.y >= res[i].iParam[2]) && (dim.y <= res[i].iParam[4])) {
              //spusteni animace v OnAbove
              if (i != lastabv) {
                if (in) {
                  Stop(&res[lastabv]);

                  if (!res[lastabv].iLayer) {
                    ddxDrawDisplay(res[lastabv].iAnim[0], 0);
                    ddxDrawSurface(CompositDC, res[lastabv].iAnim[0], 3);
                  }
                  else {
                    ddxDrawDisplayColorKey(res[lastabv].iAnim[0], 0,
                      TRANSCOLOR);
                    ddxDrawSurface(FontDC, res[lastabv].iAnim[0], 3);
                  }
                }

                CheckAnimation(&res[i], p_ad);

                lastabv = i;
                AddAnimation(&res[i], p_ad, 0, 1);
                in = 1;

                bind = ChooseBidedAnimation(res, i + 1, p_ad);

                if (bind != -1) {
                  CheckAnimation(&res[bind], p_ad);
                  AddAnimation(&res[bind], p_ad, 1, 1);
                  anbind = bind;

                  mPlaySound(&res[bind], p_ad, 0);
                }

                strcpy(dir, res[i].cParam[1]);
              }
            }
            else if (lastabv == i) {
              // odesel z oblasti, ktera byla aktivni -> stop animace                                 
              // a odznaceni oblasti
              Stop(&res[i]);

              if (!res[i].iLayer) {
                ddxDrawDisplay(res[i].iAnim[0], 0);
                ddxDrawSurface(CompositDC, res[i].iAnim[0], 3);
              }
              else {
                ddxDrawDisplayColorKey(res[i].iAnim[0], 0, TRANSCOLOR);
                ddxDrawSurface(FontDC, res[i].iAnim[0], 3);
              }

              bind = ChooseBidedExitAnimation(res, i + 1, p_ad);

              if (bind != -1) {
                int iAnim;

                if (anbind != -1) {
                  Stop(&res[anbind]);

                  if (!res[i].iLayer) {
                    ddxDrawDisplay(res[anbind].iAnim[0], 0);
                    ddxDrawSurface(CompositDC, res[anbind].iAnim[0], 3);
                  }
                  else {
                    ddxDrawDisplayColorKey(res[anbind].iAnim[0], 0,
                      TRANSCOLOR);
                    ddxDrawSurface(FontDC, res[anbind].iAnim[0], 3);
                  }
                }

                iAnim = AddAnimation(&res[bind], p_ad, 1, 1);

                if (iAnim != -1)
                  anm[iAnim].iWave = mPlaySound(&res[bind], p_ad, 2);
              }

              lastabv = -1;
              anbind = -1;
              in = 0;

              strcpy(dir, "");
            }
          }
        }
      }
      dim.dx = 0;
      dim.dy = 0;
    }

    co_Handle_Controls(citem, CLIST_ITEMC, dim.x, dim.y, BackDC, 0, 0);

    //stlacil leve tlacitko
    if (dim.t1 && !click) {
      //dostala se mys do akcni oblasti (OnClick)?
      for (i = 0; i < lastcmd; i++) {
        if (res[i].iParam[0] == COM_ONCLICK) {
          if ((dim.x >= res[i].iParam[1]) &&
            (dim.x <= res[i].iParam[3]) &&
            (dim.y >= res[i].iParam[2]) && (dim.y <= res[i].iParam[4])) {
            if (res[i].iAnim[0][0] >= 0) {
              //pokud je animace, tak ji spust
              anmid = AddAnimation(&res[i], p_ad, 0, 1);

              if (res[i + 1].iParam[0] == COM_BINDSOUND)
                mPlaySound(&res[i + 1], p_ad, 1);

              resid = i;
              click = 1;
            }
            else {
              if (res[i + 1].iParam[0] == COM_BINDSOUND)
                mPlaySound(&res[i + 1], p_ad, 1);

              resid = i;
              click = 1;
              anmid = 31;
            }
          }
        }
      }
      dim.t1 = 0;
    }

    //provedeni akce po animaci menu
    if (click || key[K_ENTER])
      if (!anm[anmid].cmd || key[K_ENTER]) {
        click = 0;

        if (key[K_ENTER])
          goto MENU_ENTER;

        if (!strcmp(res[resid].cParam[1], "EXIT") ||
          !strcmp(res[resid].cParam[1], "CANCEL")) {
          bDone = 1;
        }

        if (!strcmp(res[resid].cParam[1], "OK")) {
          int c;
          WCHAR *pName;

        MENU_ENTER:

          c = 0;
          pName = co_Edit_Get_Text(citem[0].p_edit);

          key[K_ENTER] = 0;

          if (!wcslen(pName)) {
            int iComboActSel = co_Combo_Get_Sel_Not_Opend(citem, CLIST_ITEMC, 0);

            kprintf(1, "%d", iComboActSel);
            if (iComboActSel != -1) {
              if (citem[1].p_combo->pItem) {
                if (citem[1].p_combo->pItem[iComboActSel].text) {
                  if (pr_ReadProfile(citem[1].p_combo->pItem[iComboActSel].text, &pPlayerProfile)) {
                    WritePrivateProfileString("hra", "last_profile",citem[1].p_combo->pItem[iComboActSel].text, ini_file);
                    c++;
                  }
                }
              }
            }
          }
          else if (pr_CreateProfile(pName))
            c++;

          if (c) {
            iActualScene = 1;
            bDone = 1;
          }
        }

        resid = -1;

        if (bDone) {
          for(i=0;i<lastcmd;i++) {
            if(res[i].iParam[0] == COM_BINDSOUND && res[i].iParam[5] != -1)
            {
               adas_Release_Source(PARTICULAR_SOUND_SOURCE, UNDEFINED_VALUE, res[i].iParam[5]);
               res[i].iParam[5] = -1;
            }
          }
          goto __QUIT;
        }
        else
          goto BEGIN_MENU_NEWGAME;
      }

    //pokud prisel cas, tak provedu nahodne animace (podle jejich pravdepodobnosti)
    if (timercnt > 500) {
      timercnt = 0;

      for (i = 0; i < lastcmd; i++)
        if (res[i].iParam[0] == COM_RANDOMANIMATION)
          if (rand() % 200 <= res[i].iParam[1] &&
            strcmp(dir, res[i].cParam[0])) {
            CheckAnimation(&res[i], p_ad);
            AddAnimation(&res[i], p_ad, 0, 0);
          }
    }

    dwStop = timeGetTime();

    dwEplased += dwStop - dwStart;

    spracuj_spravy(0);
    ddxUpdateMouse();
    AnimationEvent(dwStop, p_ad);

    if (dim.tf1) {
      dim.t1 = 1;
      dim.tf1 = 0;
    }

    if (dim.tf2) {
      dim.t2 = 1;
      dim.tf2 = 0;
    }

    if (ddxRestore(p_ad)) {
      fn_Release_Font(0);
      free((void *) res);
      return 1;
    }
  }

__QUIT:

  ddxCleareSurface(FontDC);
  ddxCleareSurface(CompositDC);

  bBackDC = 0;
  ddxCleareSurface(BackDC);

  fn_Release_Font(1);
  co_Handle_Release(citem, CLIST_ITEMC);
  co_Release_Graphic();

  key[K_ESC] = 0;

  FreeAnimations(res, RES_NUM);
  free((void *) res);

  return ret;
}

char check_profile(char *p_name)
{
  WCHAR wc[128];

  MultiByteToWideChar(CP_ACP, 0, p_name, strlen(p_name) + 1, wc,
    sizeof(wc) / sizeof(wc[0]));

  if (!wcscmp(pPlayerProfile.cName, wc))
    return 1;
  else
    return 0;
}

int compare(const void *arg1, const void *arg2)
{
  LIST_ITEM_ *pP1 = (LIST_ITEM_ *) arg1;
  LIST_ITEM_ *pP2 = (LIST_ITEM_ *) arg2;

  return pP2->timespamp - pP1->timespamp;
}

#ifdef LINUX
static char *p_file_mask;

static int filter(const struct dirent *file)
{
  return(!fnmatch(p_file_mask, file->d_name, 0));
}
  
int FillStringList(char *cmask, LIST_ITEM_ ** list, int *isize)
{  
  struct dirent **namelist;
  int i;
  struct stat sb;
    
  p_file_mask = cmask;
  int c = scandir(".", &namelist, &filter, alphasort); 
  
  if (c < 0) {
    (*isize) = 0;
    return 0;
  }

  (*list) = (LIST_ITEM_ *) malloc(c * sizeof(LIST_ITEM_));
  memset((*list), 0, c * sizeof(LIST_ITEM_));

  for(i = 0; i < c; i++) {
      strcpy((*list)[i].text, namelist[i]->d_name);
      (*list)[i].timespamp = !stat(namelist[i]->d_name, &sb) ? sb.st_mtime : 0;
      free(namelist[i]);
  } 
  free(namelist);
    
  qsort((*list), c, sizeof(LIST_ITEM_), compare);

  (*isize) = c;

  return c;
}
#endif

int FillComboProfiles(COMBO_CONTROL * p_co, int *iSel)
{
  int retsel = 0;
  char cprofile[128];
  char dir[256];
  WCHAR wName[256];

  PLAYER_PROFILE Profile;

  int c = 0;
  int x = 0;

  LIST_ITEM_ *list;
  int isize;

  GetPrivateProfileString("game", "profile_dir", "c:\\", dir, 256, ini_file);
  working_file_translate(dir, 256);

  GetPrivateProfileString("hra", "last_profile", "c:\\", cprofile, 128,
  ini_file);
  chdir((dir));

  FillStringList("*.prf", &list, &isize);

  if (!isize)
    return 0;

  for (c = 0; c < isize; c++) {
    ZeroMemory(wName, 256 * sizeof(WCHAR));
    if (pr_GetPlayerName(list[c].text, wName)) {
      co_Combo_Add_StringWC2(p_co, wName, list[c].text);

      if ((*iSel) == -1)        //new game -> nastavit profil, kterej byl hran naposed
      {
        if (!strcmp(cprofile, list[c].text)) {
          retsel = x;
          pr_ReadProfile(list[c].text, &Profile);
        }
      }

      if (!x)
        pr_ReadProfile(list[c].text, &Profile);

      x++;
    }
  }

  memcpy(&pPlayerProfile, &Profile, sizeof(PLAYER_PROFILE));

  *iSel = retsel;

  if ((*iSel) < 0)
    (*iSel) = 0;

  free((void *) list);

  return x;
}

int check_Save_Owner(char *cDir, WCHAR * wFileName)
{
  char text[256];
  FILE *file;
  PLAYER_PROFILE pProfile;

  LEVEL_HEADER l_h;

  char dir[MAX_PATH + 1];
  WCHAR wdir[MAX_PATH + 1];

  int ver;

  getcwd(dir, MAX_PATH);

  chdir((cDir));

  ZeroMemory(&pProfile, sizeof(PLAYER_PROFILE));

  strcpy(text, cDir);
  strcat(text, ".lvc");

  file = fopen(text, "rb");
  if (!file) {
    chdir((dir));
    return 0;
  }

  fread(&pProfile, sizeof(PLAYER_PROFILE), 1, file);
  fread(wFileName, 32 * sizeof(WCHAR), 1, file);

  fread(&ver, sizeof(int), 1, file);

  if (ver != SAVE_VER) {
    fclose(file);
    return 0;
  }

  fread(wdir, (MAX_PATH + 1) * sizeof(WCHAR), 1, file);

  fread(&l_h, sizeof(LEVEL_HEADER), 1, file);

  fclose(file);

  chdir((dir));

  if (wcscmp(pPlayerProfile.cName, pProfile.cName))
    return 0;

  if (iLanguageVersion == 4) {
    sprintf(dir, "scene%d", l_h.rezerved[1]);

    if (!GetPrivateProfileInt("Internet", dir, 0, ini_file))
      return 0;
  }

  return 1;
}

int FillListLoad(LIST_VIEW_CONTROL * p_li, char *mask, char bAdd,
  int LoadGame)
{
  char dir[256];
  WCHAR wFile[128];

  int x = 0;
  int c = 0;

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

    if (!LoadGame) {
      if (!demo_Check_Owner(pPlayerProfile.cName, list[c].text, wFile))
        continue;
    }
    else if (!check_Save_Owner(list[c].text, wFile))
      continue;

    if (bAdd)
      co_List_Add_StringWC2(p_li, x, 2, wFile, list[c].text, x, 0);

    x++;
  }

  free((void *) list);

  return x;
}

void CreateLevelCommandLine(char *cLevel, char *cLine)
{
  sprintf(cLine, "%s.lv6 fp:%s", cLevel, cLevel);
}

void RunMenuLoadGameLoad(char *p_File_Name, HWND hWnd, AUDIO_DATA * p_ad,
  int cpu, int LoadGame)
{
  DWORD dwEplased = 0, dwStart, dwStop;

  int c = 0;
  int idx = 0;
  int hdcBT = 0;
  RECT r;
  LIST_VIEW_CONTROL *p_li;
  PLAYER_PROFILE pOldProfile;

  CONTROL_LIST_ITEM citem[CLIST_ITEMC];

//      FILE    *file;
  char dir[256];
  int lastcmd, lastanm, i;

  //CMD_LINE      res[RES_NUM];
  CMD_LINE *res = NULL;
  int lastabv = -1;
  char in, click = 0;
  int anmid = -1, resid = -1, anbind = -1;
  int bind;

//      int             iComboSel = 0;
//      int             iComboActSel = 0;

  res = (CMD_LINE *) malloc(RES_NUM * sizeof(CMD_LINE));

  if (!res) {
    kprintf(1, "RunMenuLoadGameLoad: Out of Memory");
    return;
  }
  else
    ZeroMemory(res, RES_NUM * sizeof(CMD_LINE));

  ddxCleareSurface(FontDC);
//      ddxCleareSurface(BackDC);

  bBackDC = 0;

  memcpy(&pOldProfile, &pPlayerProfile, sizeof(PLAYER_PROFILE));

  //kprintf(1, "bitblt");
  ZeroMemory(citem, CLIST_ITEMC * sizeof(CONTROL_LIST_ITEM));

  for (bind = 0; bind < RES_NUM; bind++) {
    for (lastcmd = 0; lastcmd < 200; lastcmd++) {
      res[bind].iAnim[lastcmd][0] = -1;
      res[bind].iAnim[lastcmd][11] = -1;
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

  //kprintf(1, "load");
  in = 0;

  fn_Set_Font(cFontFile[0]);
  fn_Load_Bitmaps();

  //kprintf(1, "fn_Set_Font");
  CreateFontAnimations(res, &lastcmd);

  //fn_Release_Font();

  //kprintf(1, "CreateFontAnimations");

  // privede prikazy, ketere se maji provest na zacatku a, kresleni, flip,
  // animace na OnAbove
  for (i = 0; i < lastcmd; i++) {
    lastanm = 0;

    switch (res[i].iParam[0]) {
      case COM_DRAW:
        {
          if (!c) {
            hdcBT = ddxCreateSurface(ddxGetWidth(res[i].iParam[1]),
              ddxGetHight(res[i].iParam[1]), ddxFindFreeSurface());

            ddxTransparentBlt(BackDC, res[i].iParam[2], res[i].iParam[3],
              ddxGetWidth(res[i].iParam[1]), ddxGetHight(res[i].iParam[1]),
              res[i].iParam[1], 0, 0, ddxGetWidth(res[i].iParam[1]),
              ddxGetHight(res[i].iParam[1]), TRANSCOLOR);

            ddxBitBlt(hdcBT, 0, 0, ddxGetWidth(res[i].iParam[1]),
              ddxGetHight(res[i].iParam[1]),
              HDC2DD, res[i].iParam[2], res[i].iParam[3]);

            idx = i;
          }
          else {
            ddxDrawSurfaceColorKey(BackDC, res[i].iParam, 2, TRANSCOLOR);
          }
          c++;
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


  //co_Load_Graphic(1);

  //co_List_Redraw(Ltmp1DC, llcitem[1].p_list, 0);

  if (!co_Load_Graphic(1))
    assert(0);
  else {
    int xx;
    //int sel = 0;

    r.left = 299;
    r.top = 209;
    r.right = 743;
    r.bottom = 359;

    if (LoadGame)
      co_Set_Text_Center(BackDC, "##menu_loadgame", 0, r);
    else
      co_Set_Text_Center(BackDC, "##menu_playdemo", 0, r);

    fn_Release_Font(1);

    fn_Set_Font(cFontFile[2]);
    fn_Load_Bitmaps();

/*		citem[0].p_combo = co_Create_Combo(BackDC, 360, 320, 100, 0);
		citem[0].bActive = 1;

		xx = FillComboProfiles(citem[0].p_combo, &sel);

		if(xx > 5)
			co_Combo_Set_Params(citem[0].p_combo, 5);
		else
			co_Combo_Set_Params(citem[0].p_combo, xx);

		co_Combo_Set_Sel(BackDC, citem[0].p_combo, sel);*/

    if (LoadGame) {
      xx = FillListLoad(NULL, "*", 0, LoadGame);

      if (xx < 7)
        xx = 7;

      citem[1].p_list = co_Create_List(BackDC, 360, 320, 320, 200, 0, xx, 1);
      citem[1].bActive = 1;
      FillListLoad(citem[1].p_list, "*", 1, LoadGame);
    }
    else {
      xx = FillListLoad(NULL, "*.dem", 0, LoadGame);

      if (xx < 7)
        xx = 7;

      citem[1].p_list = co_Create_List(BackDC, 360, 320, 320, 200, 0, xx, 1);
      citem[1].bActive = 1;
      FillListLoad(citem[1].p_list, "*.dem", 1, LoadGame);
    }

    co_List_Redraw(BackDC, citem[1].p_list, 0);
    fn_Release_Font(1);
  }

//BEGIN_MENU:

  bBackDC = 1;
  //TransparentBltU(BackDC, 0, 0, 1024, 768, Ltmp1DC, 0, 0, 1024, 768, RGB(255, 0, 255));
//      ddxTransparentBlt(BackDC, 0, 0, 1024, 768, Ltmp1DC, 0, 0, 1024, 768, TRANSCOLOR);
  //Sleep(10000);
  //TransparentBltU(_2dd.hDC, 0, 0, 1024, 768, BackDC, 0, 0, 1024, 768, RGB(255, 0, 255));
  ddxTransparentBltDisplay(0, 0, 1024, 768, BackDC, 0, 0, 1024, 768, TRANSCOLOR);

  for (i = 0; i < lastcmd; i++)
    if (res[i].iParam[0] == COM_RUNANIMATION) {
      int iWave = AddAnimation(&res[i], p_ad, 0, 0);

      if (iWave != -1) {
        if (res[i + 1].iParam[0] == COM_BINDSOUND)
          anm[iWave].iWave = res[i + 1].iParam[5] =
            mPlaySound(&res[i + 1], p_ad, 2);
      }
    }

  dim.t1 = 0;
  dim.t2 = 0;
  dim.dx = 0;
  dim.dy = 0;
  anmid = -1;
  resid = -1;
  anbind = -1;
  bind = -1;
  lastabv = -1;
  in = 0;

  spracuj_spravy(0);

  while (!key[K_ESC]) {
    dwStart = timeGetTime();

    //pohnul mysi
    if (dim.dx || dim.dy) {
      //dostala se mys do akcni oblasti (OnAbove)?
      if (!click)
        for (i = 0; i < lastcmd; i++)
          if (res[i].iParam[0] == COM_ONABOVE) {
            if ((dim.x >= res[i].iParam[1]) &&
              (dim.x <= res[i].iParam[3]) &&
              (dim.y >= res[i].iParam[2]) && (dim.y <= res[i].iParam[4])) {
              //spusteni animace v OnAbove
              if (i != lastabv) {
                if (in) {
                  Stop(&res[lastabv]);

                  if (!res[lastabv].iLayer) {
                    //menucommand_Draw(_2dd.hDC, res[lastabv].iAnim[0], 0);
                    ddxDrawDisplay(res[lastabv].iAnim[0], 0);
                    ddxDrawSurface(CompositDC, res[lastabv].iAnim[0], 3);
                  }
                  else {
                    //menucommand_DrawT(_2dd.hDC, res[lastabv].iAnim[0]);
                    ddxDrawDisplayColorKey(res[lastabv].iAnim[0], 0, TRANSCOLOR);
                    //menucommand_Draw(FontDC, res[lastabv].iAnim[0], 3);
                    ddxDrawSurface(FontDC, res[lastabv].iAnim[0], 3);
                  }
                }

                CheckAnimation(&res[i], p_ad);

                lastabv = i;
                AddAnimation(&res[i], p_ad, 0, 1);
                in = 1;

                bind = ChooseBidedAnimation(res, i + 1, p_ad);

                if (bind != -1) {
                  CheckAnimation(&res[bind], p_ad);
                  AddAnimation(&res[bind], p_ad, 1, 1);
                  anbind = bind;

                  mPlaySound(&res[bind], p_ad, 0);
                }

                strcpy(dir, res[i].cParam[1]);
              }
            }
            else if (lastabv == i) {
              // odesel z oblasti, ktera byla aktivni -> stop animace                                 
              // a odznaceni oblasti
              Stop(&res[i]);

              if (!res[i].iLayer) {
                //menucommand_Draw(_2dd.hDC, res[i].iAnim[0], 0);
                ddxDrawDisplay(res[i].iAnim[0], 0);
                ddxDrawSurface(CompositDC, res[i].iAnim[0], 3);
              }
              else {
                //menucommand_DrawT(_2dd.hDC, res[i].iAnim[0]);
                ddxDrawDisplayColorKey(res[i].iAnim[0], 0, TRANSCOLOR);
                //menucommand_DrawT(FontDC, res[i].iAnim[0]);
                ddxDrawSurface(FontDC, res[i].iAnim[0], 3);
              }

              bind = ChooseBidedExitAnimation(res, i + 1, p_ad);

              if (bind != -1) {
                int iAnim;

                if (anbind != -1) {
                  Stop(&res[anbind]);

                  if (!res[i].iLayer) {
                    //menucommand_Draw(_2dd.hDC, res[anbind].iAnim[0], 0);
                    ddxDrawDisplay(res[anbind].iAnim[0], 0);
                    ddxDrawSurface(CompositDC, res[anbind].iAnim[0], 3);
                  }
                  else {
                    //menucommand_DrawT(_2dd.hDC, res[anbind].iAnim[0]);
                    ddxDrawDisplayColorKey(res[anbind].iAnim[0], 0, TRANSCOLOR);
                    //menucommand_DrawT(FontDC, res[anbind].iAnim[0]);
                    ddxDrawSurface(FontDC, res[anbind].iAnim[0], 3);
                  }
                }

                iAnim = AddAnimation(&res[bind], p_ad, 1, 1);

                if (iAnim != -1)
                  anm[iAnim].iWave = mPlaySound(&res[bind], p_ad, 2);
              }

              lastabv = -1;
              anbind = -1;
              in = 0;

              strcpy(dir, "");
            }
          }

      dim.dx = 0;
      dim.dy = 0;
    }

    co_Handle_Controls(citem, CLIST_ITEMC, dim.x, dim.y, BackDC, 0, 0);

    //////////////LOAD LEVEL/DEMO ///////////////////////////////////////////////////
    if (co_List_Get_Dbclck(citem, CLIST_ITEMC, 0, &p_li) == 1) {
      if (p_li->piValue[p_li->cSelected].cValue)
        if (strlen(p_li->piValue[p_li->cSelected].cValue)) {
          //citem[0].bActive = 0;
          citem[1].bActive = 0;

          if (LoadGame)         //LOAD GAME
          {
            //fn_Release_Font(1);
            StopAll();
            RunMenuNewGameScene("Mmnew_game_scene.txt", NULL, p_ad, cpu, 1, p_li->piValue[p_li->cSelected].cValue, 0);
            //fn_Set_Font("font_system.pak");
            //fn_Load_Bitmaps();
            key[K_ESC] = 1;
          }
          else                  //LOAD DEMO
          {
            char ctext[256];

            CreateLevelCommandLine(p_li->piValue[p_li->cSelected].cValue, ctext);
            demo_Set_Scene_Level(p_li->piValue[p_li->cSelected].cValue, &iActualScene, &iActualLevel);
            RunLevel(NULL, p_ad, cpu, ctext, "LOAD_DEMO");

            for (i = 0; i < 32; i++)
              anm[i].cmd = NULL;

            key[K_ESC] = 1;
          }

          p_li->bDblClck = 0;
          p_li->bClck = 0;
        }
    }

/*		iComboActSel = co_Combo_Get_Sel_Not_Opend(citem, CLIST_ITEMC, 0);

		if(iComboActSel != iComboSel && iComboActSel != -1)
		{
			int xx;

			pr_ReadProfile(citem[0].p_combo->pItem[iComboActSel].text, &pPlayerProfile);

			if(LoadGame)
			{
				xx = FillListLoad(NULL, "*", 0, LoadGame);
	
				if(xx < 4)
					xx = 4;

				co_Cleare_List(citem[1].p_list, xx, BackDC, 0, 0);

				FillListLoad(citem[1].p_list, "*", 1, LoadGame);
			}
			else
			{
				xx = FillListLoad(NULL, "*.dem", 0, LoadGame);
	
				if(xx < 4)
					xx = 4;

				co_Cleare_List(citem[1].p_list, xx, BackDC, 0, 0);

				FillListLoad(citem[1].p_list, "*.dem", 1, LoadGame);
			}
			
			co_List_Redraw(BackDC, citem[1].p_list, 0);
			iComboSel = iComboActSel;
		}
		*/

    //stlacil leve tlacitko
    if (dim.t1 && !click) {
      //dostala se mys do akcni oblasti (OnClick)?
      for (i = 0; i < lastcmd; i++)
        if (res[i].iParam[0] == COM_ONCLICK)
          if ((dim.x >= res[i].iParam[1]) &&
            (dim.x <= res[i].iParam[3]) &&
            (dim.y >= res[i].iParam[2]) && (dim.y <= res[i].iParam[4])) {
            if (res[i].iAnim[0][0] >= 0) {
              //pokud je animace, tak ji spust
              anmid = AddAnimation(&res[i], p_ad, 0, 1);

              if (res[i + 1].iParam[0] == COM_BINDSOUND)
                mPlaySound(&res[i + 1], p_ad, 1);

              resid = i;
              click = 1;
            }
            else {
              if (res[i + 1].iParam[0] == COM_BINDSOUND)
                mPlaySound(&res[i + 1], p_ad, 1);

              resid = i;
              click = 1;
              anmid = 31;
            }
          }

      dim.t1 = 0;
    }

    //provedeni akce po animaci menu
    if (click)
      if (!anm[anmid].cmd) {
        click = 0;

        //StopAll();


        if (!strcmp(res[resid].cParam[1], "CANCEL")) {
          memcpy(&pPlayerProfile, &pOldProfile, sizeof(PLAYER_PROFILE));
          key[K_ESC] = 1;
        }

        if (!strcmp(res[resid].cParam[1], "EXIT"))
          key[K_ESC] = 1;

        if (!strcmp(res[resid].cParam[1], "OK") && p_li->cClckSel != -1) {
          if (p_li->piValue[p_li->cClckSel].cValue)
            if (strlen(p_li->piValue[p_li->cClckSel].cValue)) {
              citem[0].bActive = 0;
              citem[1].bActive = 0;

              if (LoadGame)     //LOAD GAME
              {
                //fn_Release_Font(1);
                StopAll();
                RunMenuNewGameScene("Mmnew_game_scene.txt", NULL, p_ad, cpu, 1, p_li->piValue[p_li->cClckSel].cValue, 0);
                //fn_Set_Font("font_system.pak");
                //fn_Load_Bitmaps();
                key[K_ESC] = 1;
              }
              else              //LOAD DEMO
              {
                char ctext[256];

                CreateLevelCommandLine(p_li->piValue[p_li->cClckSel].cValue, ctext);
                demo_Set_Scene_Level(p_li->piValue[p_li->cClckSel].cValue, &iActualScene, &iActualLevel);
                RunLevel(NULL, p_ad, cpu, ctext, "LOAD_DEMO");

                for (i = 0; i < 32; i++)
                  anm[i].cmd = NULL;

                key[K_ESC] = 1;
              }

              p_li->bDblClck = 0;
              p_li->bClck = 0;
            }
        }

        resid = -1;

        if (key[K_ESC]) {
          for(i=0;i<lastcmd;i++) {
            if(res[i].iParam[0] == COM_BINDSOUND && res[i].iParam[5] != -1)
            {
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
            CheckAnimation(&res[i], p_ad);
            AddAnimation(&res[i], p_ad, 0, 0);
          }
    }

    dwStop = timeGetTime();

    dwEplased += dwStop - dwStart;

    spracuj_spravy(0);
    ddxUpdateMouse();
    AnimationEvent(dwStop, p_ad);

    if (dim.tf1) {
      dim.t1 = 1;
      dim.tf1 = 0;
    }

    if (dim.tf2) {
      dim.t2 = 1;
      dim.tf2 = 0;
    }

    if (ddxRestore(p_ad)) {
      cRestartMainMenu = 1;
      key[K_ESC] = 1;
    }
  }

__QUIT:

  if (!cBrutalRestart) {
    ddxBitBltDisplay(res[idx].iParam[2], res[idx].iParam[3],
      ddxGetWidth(res[idx].iParam[1]),
      ddxGetHight(res[idx].iParam[1]), hdcBT, 0, 0);

    ddxTransparentBlt(BackDC, res[idx].iParam[2], res[idx].iParam[3],
      ddxGetWidth(res[idx].iParam[1]),
      ddxGetHight(res[idx].iParam[1]),
      res[idx].iParam[1], 0, 0, ddxGetWidth(res[idx].iParam[1]),
      ddxGetHight(res[idx].iParam[1]), TRANSCOLOR);

    ddxReleaseBitmap(hdcBT);
    ddxCleareSurface(FontDC);
    co_Handle_Release(citem, CLIST_ITEMC);
    co_Release_Graphic();
  }

  key[K_ESC] = 0;

  FreeAnimations(res, RES_NUM);
  free((void *) res);
}

void RunMenuLoadGame(char *p_File_Name, HWND hWnd, AUDIO_DATA * p_ad, int cpu)
{
  DWORD dwEplased = 0, dwStart, dwStop;
  RECT r;

  int c = 0;
  int idx = 0;
  int hdcBT = 0;

  CONTROL_LIST_ITEM citem[CLIST_ITEMC];

//      FILE    *file;
  char dir[256];
  int lastcmd, lastanm, i;

  //CMD_LINE      res[RES_NUM];
  CMD_LINE *res = NULL;
  int lastabv = -1;
  char in, click = 0;
  int anmid = -1, resid = -1, anbind = -1;
  int bind;
  int iNadpisDC = -1;

  res = (CMD_LINE *) malloc(RES_NUM * sizeof(CMD_LINE));

  if (!res) {
    kprintf(1, "RunMenuLoadGame: Out of Memory");
    return;
  }
  else
    ZeroMemory(res, RES_NUM * sizeof(CMD_LINE));

  //bBackDC = 1;

//      ddxCleareSurface(FontDC);
//      ddxCleareSurface(BackDC);

  ddxCleareSurface(BackDC);
  ddxBitBlt(BackDC, 0, 0, ddxGetWidth(BackDC), ddxGetHight(BackDC), FontDC, 0,
    0);
  ddxCleareSurface(FontDC);

  //kprintf(1, "bitblt");
  ZeroMemory(citem, CLIST_ITEMC * sizeof(CONTROL_LIST_ITEM));

  for (bind = 0; bind < RES_NUM; bind++) {
    for (lastcmd = 0; lastcmd < 200; lastcmd++) {
      res[bind].iAnim[lastcmd][0] = -1;
      res[bind].iAnim[lastcmd][11] = -1;
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

  //kprintf(1, "load");
  in = 0;

  fn_Set_Font(cFontFile[0]);
  fn_Load_Bitmaps();

  //kprintf(1, "fn_Set_Font");
  CreateFontAnimations(res, &lastcmd);

  // privede prikazy, ketere se maji provest na zacatku a, kresleni, flip,
  // animace na OnAbove
  for (i = 0; i < lastcmd; i++) {
    lastanm = 0;

    switch (res[i].iParam[0]) {
      case COM_DRAW:
        {
          if (!c) {
            hdcBT = ddxCreateSurface(ddxGetWidth(res[i].iParam[1]),
              ddxGetHight(res[i].iParam[1]), ddxFindFreeSurface());

            ddxBitBlt(hdcBT, 0, 0, ddxGetWidth(res[i].iParam[1]),
              ddxGetHight(res[i].iParam[1]),
              HDC2DD, res[i].iParam[2], res[i].iParam[3]);

            ddxTransparentBlt(BackDC, res[i].iParam[2], res[i].iParam[3],
              ddxGetWidth(res[i].iParam[1]), ddxGetHight(res[i].iParam[1]),
              res[i].iParam[1], 0, 0, ddxGetWidth(res[i].iParam[1]),
              ddxGetHight(res[i].iParam[1]), TRANSCOLOR);

            idx = i;
          }
          else {
            ddxDrawSurfaceColorKey(BackDC, res[i].iParam, 2, TRANSCOLOR);
          }


          c++;
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

  r.left = 299;
  r.top = 209;
  r.right = 743;
  r.bottom = 359;

  co_Set_Text_Center(BackDC, "##menu_lgametitle", 0, r);

  iNadpisDC =
    ddxCreateSurface(r.right - r.left, r.bottom - r.top,
    ddxFindFreeSurface());

  ddxBitBlt(iNadpisDC, 0, 0, ddxGetWidth(iNadpisDC), ddxGetHight(iNadpisDC),
    BackDC, r.left, r.top);

  fn_Release_Font(1);


BEGIN_MENU_LOAD:

  bBackDC = 1;
  ddxTransparentBltDisplay(0, 0, 1024, 768, BackDC, 0, 0, 1024, 768,
    TRANSCOLOR);
  DisplayFrame();

  for (i = 0; i < lastcmd; i++)
    if (res[i].iParam[0] == COM_RUNANIMATION) {
      int iWave = AddAnimation(&res[i], p_ad, 0, 0);

      if (iWave != -1) {
        if (res[i + 1].iParam[0] == COM_BINDSOUND)
          anm[iWave].iWave = res[i + 1].iParam[5] =
            mPlaySound(&res[i + 1], p_ad, 2);
      }
    }

  dim.t1 = 0;
  dim.t2 = 0;
  dim.dx = 0;
  dim.dy = 0;
  anmid = -1;
  resid = -1;
  anbind = -1;
  bind = -1;
  lastabv = -1;
  in = 0;

  spracuj_spravy(0);

  while (!key[K_ESC]) {
    dwStart = timeGetTime();

    //pohnul mysi
    if (dim.dx || dim.dy) {
      //dostala se mys do akcni oblasti (OnAbove)?
      if (!click)
        for (i = 0; i < lastcmd; i++)
          if (res[i].iParam[0] == COM_ONABOVE) {
            if ((dim.x >= res[i].iParam[1]) &&
              (dim.x <= res[i].iParam[3]) &&
              (dim.y >= res[i].iParam[2]) && (dim.y <= res[i].iParam[4])) {
              //spusteni animace v OnAbove
              if (i != lastabv) {
                if (in) {
                  Stop(&res[lastabv]);

                  if (!res[lastabv].iLayer) {
                    //menucommand_Draw(_2dd.hDC, res[lastabv].iAnim[0], 0);
                    ddxDrawSurface(CompositDC, res[lastabv].iAnim[0], 3);
                    ddxDrawDisplay(res[lastabv].iAnim[0], 0);
                  }
                  else {
                    //menucommand_DrawT(_2dd.hDC, res[lastabv].iAnim[0]);
                    ddxDrawDisplayColorKey(res[lastabv].iAnim[0], 0,
                      TRANSCOLOR);
                    //menucommand_Draw(FontDC, res[lastabv].iAnim[0], 3);
                    ddxDrawSurface(FontDC, res[lastabv].iAnim[0], 3);
                  }
                }

                CheckAnimation(&res[i], p_ad);

                lastabv = i;
                AddAnimation(&res[i], p_ad, 0, 1);
                in = 1;

                bind = ChooseBidedAnimation(res, i + 1, p_ad);

                if (bind != -1) {
                  CheckAnimation(&res[bind], p_ad);
                  AddAnimation(&res[bind], p_ad, 1, 1);
                  anbind = bind;

                  mPlaySound(&res[bind], p_ad, 0);
                }

                strcpy(dir, res[i].cParam[1]);
              }
            }
            else if (lastabv == i) {
              // odesel z oblasti, ktera byla aktivni -> stop animace                                 
              // a odznaceni oblasti
              Stop(&res[i]);

              if (!res[i].iLayer) {
                //menucommand_Draw(_2dd.hDC, res[i].iAnim[0], 0);
                ddxDrawDisplay(res[i].iAnim[0], 0);
                ddxDrawSurface(CompositDC, res[i].iAnim[0], 3);
              }
              else {
                //menucommand_DrawT(_2dd.hDC, res[i].iAnim[0]);
                ddxDrawDisplayColorKey(res[i].iAnim[0], 0, TRANSCOLOR);
                //menucommand_DrawT(FontDC, res[i].iAnim[0]);
                ddxDrawSurface(FontDC, res[i].iAnim[0], 3);
              }

              bind = ChooseBidedExitAnimation(res, i + 1, p_ad);

              if (bind != -1) {
                int iAnim;

                if (anbind != -1) {
                  Stop(&res[anbind]);

                  if (!res[i].iLayer) {
                    //menucommand_Draw(_2dd.hDC, res[anbind].iAnim[0], 0);
                    ddxDrawDisplay(res[anbind].iAnim[0], 0);
                    ddxDrawSurface(CompositDC, res[anbind].iAnim[0], 3);
                  }
                  else {
                    //menucommand_DrawT(_2dd.hDC, res[anbind].iAnim[0]);
                    ddxDrawDisplayColorKey(res[anbind].iAnim[0], 0,
                      TRANSCOLOR);
                    //menucommand_DrawT(FontDC, res[anbind].iAnim[0]);
                    ddxDrawSurface(FontDC, res[anbind].iAnim[0], 3);
                  }
                }

                iAnim = AddAnimation(&res[bind], p_ad, 1, 1);

                if (iAnim != -1)
                  anm[iAnim].iWave = mPlaySound(&res[bind], p_ad, 2);
              }

              lastabv = -1;
              anbind = -1;
              in = 0;

              strcpy(dir, "");
            }
          }

      dim.dx = 0;
      dim.dy = 0;
    }

    //co_Handle_Controls(citem, CLIST_ITEMC, mi.x, mi.y);

    //stlacil leve tlacitko
    if (dim.t1 && !click) {
      //dostala se mys do akcni oblasti (OnClick)?
      for (i = 0; i < lastcmd; i++)
        if (res[i].iParam[0] == COM_ONCLICK)
          if ((dim.x >= res[i].iParam[1]) &&
            (dim.x <= res[i].iParam[3]) &&
            (dim.y >= res[i].iParam[2]) && (dim.y <= res[i].iParam[4])) {
            if (res[i].iAnim[0][0] >= 0) {
              //pokud je animace, tak ji spust
              anmid = AddAnimation(&res[i], p_ad, 0, 1);

              if (res[i + 1].iParam[0] == COM_BINDSOUND)
                mPlaySound(&res[i + 1], p_ad, 1);

              resid = i;
              click = 1;
            }
            else {
              if (res[i + 1].iParam[0] == COM_BINDSOUND)
                mPlaySound(&res[i + 1], p_ad, 1);

              resid = i;
              click = 1;
              anmid = 31;
            }
          }

      dim.t1 = 0;
    }

    //provedeni akce po animaci menu
    if (click)
      if (!anm[anmid].cmd) {
        click = 0;

        //StopAll();

        if (!strcmp(res[resid].cParam[1], "EXIT") ||
          !strcmp(res[resid].cParam[1], "CANCEL")) {
          key[K_ESC] = 1;
          //break;
        }

        if (!strcmp(res[resid].cParam[1], "LOAD_GAME_LOAD"))
          RunMenuLoadGameLoad("Mmload_game_load.txt", NULL, p_ad, cpu, 1);

        if (!strcmp(res[resid].cParam[1], "LOAD_GAME_DEMO"))
          RunMenuLoadGameLoad("Mmload_game_load.txt", NULL, p_ad, cpu, 0);

        if (cBrutalRestart)
          key[K_ESC] = 1;

        if (!cBrutalRestart) {
          for (i = 0; i < lastcmd; i++) {
            switch (res[i].iParam[0]) {
              case COM_DRAW:
                if (!res[i].iLayer) {
                  //menucommand_Draw(_2dd.hDC, res[i].iParam);
                }
                else {
                  ddxDrawDisplayColorKey(res[i].iParam, 0, TRANSCOLOR);
                  ddxDrawSurfaceColorKey(BackDC, res[i].iParam, 2,
                    TRANSCOLOR);
                  ddxDrawSurface(FontDC, res[i].iParam, 3);
                }
                break;
            }
          }

          ddxTransparentBltDisplay(r.left, r.top, ddxGetWidth(iNadpisDC),
            ddxGetHight(iNadpisDC), iNadpisDC, 0, 0, ddxGetWidth(iNadpisDC),
            ddxGetHight(iNadpisDC), TRANSCOLOR);

          ddxTransparentBlt(BackDC, r.left, r.top, ddxGetWidth(iNadpisDC),
            ddxGetHight(iNadpisDC), iNadpisDC, 0, 0, ddxGetWidth(iNadpisDC),
            ddxGetHight(iNadpisDC), TRANSCOLOR);
        }

        resid = -1;

        if (key[K_ESC]) {
          for(i=0;i<lastcmd;i++)
            if(res[i].iParam[0] == COM_BINDSOUND && res[i].iParam[5] != -1)
            {
              adas_Release_Source(PARTICULAR_SOUND_SOURCE, UNDEFINED_VALUE, res[i].iParam[5]);
              res[i].iParam[5] = -1;
            }
          goto __QUIT;
        }
        else
          goto BEGIN_MENU_LOAD;
      }

    //pokud prisel cas, tak provedu nahodne animace (podle jejich pravdepodobnosti)
    if (timercnt > 500) {
      timercnt = 0;

      for (i = 0; i < lastcmd; i++)
        if (res[i].iParam[0] == COM_RANDOMANIMATION)
          if (rand() % 200 <= res[i].iParam[1] &&
            strcmp(dir, res[i].cParam[0])) {
            CheckAnimation(&res[i], p_ad);
            AddAnimation(&res[i], p_ad, 0, 0);
          }
    }

    /*spracuj_spravy(0);  
       ddxUpdateMouse();

       if(dim.dx || dim.dy)
       DisplayFrame(); */

    dwStop = timeGetTime();

    dwEplased += dwStop - dwStart;

    spracuj_spravy(0);
    ddxUpdateMouse();
    AnimationEvent(dwStop, p_ad);

    if (dim.tf1) {
      dim.t1 = 1;
      dim.tf1 = 0;
    }

    if (dim.tf2) {
      dim.t2 = 1;
      dim.tf2 = 0;
    }

    if (ddxRestore(p_ad)) {
      cRestartMainMenu = 1;
      key[K_ESC] = 1;
    }
  }

__QUIT:

  //BitBltU(FontDC, 0, 0, 1024, 768, NULL, 0, 0, WHITENESS);
  ddxCleareSurface(FontDC);

  //TransparentBltU(_2dd.hDC, 0, 0, 1024, 768, BackDC, 0, 0, 1024, 768, RGB(255, 0, 255));
  if (!cBrutalRestart)
    ddxTransparentBltDisplay(0, 0, 1024, 768, BackDC, 0, 0, 1024, 768,
      TRANSCOLOR);

  //BitBltU(BackDC, 0, 0, 1024, 768, NULL, 0, 0, WHITENESS);
  ddxCleareSurface(BackDC);

  bBackDC = 0;

  /*BitBlt(_2dd.hDC, res[idx].iParam[2], res[idx].iParam[3], 
     _2dd.bitmap[res[idx].iParam[1]].bitmap.bmWidth, 
     _2dd.bitmap[res[idx].iParam[1]].bitmap.bmHeight,
     hdcBT, 0, 0, SRCCOPY); */

  if (!cBrutalRestart) {
    ddxBitBltDisplay(res[idx].iParam[2], res[idx].iParam[3],
      ddxGetWidth(res[idx].iParam[1]),
      ddxGetHight(res[idx].iParam[1]), hdcBT, 0, 0);

    ddxTransparentBlt(BackDC, res[idx].iParam[2], res[idx].iParam[3],
      ddxGetWidth(res[idx].iParam[1]),
      ddxGetHight(res[idx].iParam[1]),
      res[idx].iParam[1], 0, 0, ddxGetWidth(res[idx].iParam[1]),
      ddxGetHight(res[idx].iParam[1]), TRANSCOLOR);

    ddxReleaseBitmap(hdcBT);
    ddxReleaseBitmap(iNadpisDC);
  }

  ddxCleareSurface(CompositDC);
  //fn_Release_Font();
  //co_Handle_Release(citem, CLIST_ITEMC);
  //co_Release_Graphic();
  key[K_ESC] = 0;

  FreeAnimations(res, RES_NUM);

  free((void *) res);
}

void RunMenuCibron(char *cBmp)
{
  int idx = ddxCreateSurface(1024, 768, ddxFindFreeSurface());
  int cib;

  if (idx == -1)
    return;

  ddxBitBlt(idx, 0, 0, 1024, 768, HDC2DD, 0, 0);

  cib = ddxLoadBitmap(cBmp, pBmpArchive);

  if (cib == -1) {
    ddxReleaseBitmap(idx);
    return;
  }

  ddxBitBltDisplay(0, 0, 1024, 768, cib, 0, 0);
  DisplayFrame();

  while (!key[0] && !dim.t1 && !dim.t2) {
    spracuj_spravy(0);
    ddxUpdateMouse();
    DisplayFrame();
    Sleep(10);
  }

  dim.tf1 = 0;
  dim.tf2 = 0;
  dim.t1 = 0;
  dim.t2 = 0;

  memset(key, 0, POCET_KLAVES * sizeof(char));

  ddxBitBlt(HDC2DD, 0, 0, 1024, 768, idx, 0, 0);

  ddxReleaseBitmap(idx);
  ddxReleaseBitmap(cib);
}

void RunMenuCinemax(void)
{
  int c = 0;
  int cib;

  pBmpArchive->pActualNode = pBmpArchive->pRootNode->pNextNode;

  cib = ddxLoadBitmap("cinemax.bmp", pBmpArchive);

  if (cib == -1) {
    pBmpArchive->bError = 0;
    return;
  }

  ddxSetCursor(0);
  ddxSetFlip(0);
  ddxBitBltDisplay(0, 0, 1024, 768, cib, 0, 0);
  DisplayFrame();
  ddxBitBltDisplay(0, 0, 1024, 768, cib, 0, 0);

  while (!key[0] && !dim.t1 && !dim.t2) {
    Sleep(10);
    c += 10;

    if (c > 1000)
      break;
  }

  dim.tf1 = 0;
  dim.tf2 = 0;
  dim.t1 = 0;
  dim.t2 = 0;

  memset(key, 0, POCET_KLAVES * sizeof(char));

  ddxReleaseBitmap(cib);
  ddxSetCursor(1);
  ddxSetFlip(1);
}

void RunMenuDrawVersion(int iSurface)
{
  char text[256];

  sprintf(text, "v%d.%d", VERZEHI, VERZELO);

  //co_Set_Text_RightWC(HDC2DD, text, 1, 990, 318);
  co_Set_Text_RightWC(HDC2DD, text, 0, 1024, 0);
  //co_Set_Text(HDC2DD, 280, 0, "##sale", 0);

  co_Set_Text_RightWC(iSurface, text, 0, 1024, 0);
  //co_Set_Text(iSurface, 280, 0, "##sale", 0);
}

void RunMenuDrawDemoEndScreen(void)
{
  int idx;

  ddxSetFlip(1);

  key[0] = 0;
  idx = ddxLoadBitmap("final_screen.bmp", pBmpArchive);

  if (idx == -1)
    return;

  ddxSetCursor(1);
  ddxBitBltDisplay(0, 0, 1024, 768, idx, 0, 0);

  ddxSetFlip(0);

  while (!key[0] && !mi.t1 && !mi.t2) {
    Sleep(25);

    spracuj_spravy(1);
    ddxUpdateMouse();

    if (mi.dx || mi.dy)
      DisplayFrame();
  }

  ddxReleaseBitmap(idx);
  ddxSetCursor(0);
}

//------------------------------------------------------------------------------------------------
// spusti menu
//------------------------------------------------------------------------------------------------
void RunMenu(char *p_File_Name, HWND hWnd, AUDIO_DATA * p_ad, int cpu)
{
  DWORD dwEplased = 0, dwStart, dwStop;

  char bStop = 1;
  char dir[256];
  int lastcmd, lastanm, i, j;
  CMD_LINE *res = NULL;
  int lastabv = -1;
  char in, click = 0;
  int anmid = -1, resid = -1, anbind = -1;
  int bind;
  int cStartCount = 0;


  res = (CMD_LINE *) malloc(RES_NUM * sizeof(CMD_LINE));

  if (!res) {
    kprintf(1, "RunMenu: Out of Memory");
    return;
  }

RUN_MENU_BRUTAL_RESTART:

  ZeroAnimations();

  bNewScene = 0;
  cRestartMainMenu = 0;

  if (cBrutalRestart) {
    fn_Set_Font(cFontFile[0]);
    fn_Load_Bitmaps();

    for (i = 0; i < 32; i++)
      anm[i].cmd = NULL;

    cRestartMainMenu = 1;
  }

  ZeroMemory(res, RES_NUM * sizeof(CMD_LINE));

  GetPrivateProfileString("game", "data_dir", "c:\\", dir, 256, ini_file);
  working_file_translate(dir, 256);
  chdir((dir));

  if (!cBrutalRestart) {
    iCompositDC = ddxFindFreeSurface();
    CompositDC = ddxCreateSurface(1024, 768, iCompositDC);
    iFontDC = ddxFindFreeSurface();
    FontDC = ddxCreateSurface(1024, 768, iFontDC);
    iBackDC = ddxFindFreeSurface();
    BackDC = ddxCreateSurface(1024, 768, iBackDC);
  }

  cBrutalRestart = 0;

  for (bind = 0; bind < RES_NUM; bind++) {
    for (lastcmd = 0; lastcmd < 200; lastcmd++) {
      res[bind].iAnim[lastcmd][0] = -1;
      res[bind].iAnim[lastcmd][11] = -1;
    }

    for (in = 0; in < 6; in++)
      res[bind].iParam[(int)in] = -1;

    res[bind].iLayer = 0;
  }

  lastcmd = 0;
  timercnt = 0;

  //natadhe skript menu
  LoadMenuScript(p_File_Name, res, &lastcmd);

  in = 0;

  CreateFontAnimations(res, &lastcmd);
  fn_Release_Font(1);

  // privede prikazy, ketere se maji provest na zacatku a, kresleni, flip,
  // animace na OnAbove

  fn_Set_Font(cFontFile[2]);
  fn_Load_Bitmaps();

  ddxSetFlip(0);

  for (j = 0; j < 2; j++) {
    for (i = 0; i < lastcmd; i++) {
      lastanm = 0;

      switch (res[i].iParam[0]) {
        case COM_DRAW:
          if (!res[i].iLayer)
            ddxDrawDisplay(res[i].iParam, 0);
          else {
            ddxDrawDisplayColorKey(res[i].iParam, 0, TRANSCOLOR);
            ddxDrawSurface(FontDC, res[i].iParam, 3);
          }
          break;
        case COM_RANDOMANIMATION:
        case COM_ONCLICK:
        case COM_ONABOVE:
        case COM_RUNANIMATION:
        case COM_BINDEXITANIMATION:
        case COM_BINDANIMATION:
          //nahrati animace k udalosti OnAbove
          if (!j)
            LoadAnimationMenuScript(res, i, &lastanm);
          break;
      }
    }

    RunMenuDrawVersion(res[0].iParam[1]);
    DisplayFrame();
  }

  fn_Release_Font(1);
  ddxSetFlip(1);

BEGIN_MENU:

  if (bStop || cRestartMainMenu)
    for (i = 0; i < lastcmd; i++)
      if (res[i].iParam[0] == COM_RUNANIMATION) {
        int iWave = AddAnimation(&res[i], p_ad, 0, 0);

        kprintf(1, "COM_RUNANIMATION = %d, iWAVE = %d", i, iWave);

        if (iWave != -1) {
          if (res[i + 1].iParam[0] == COM_BINDSOUND)
            anm[iWave].iWave = res[i + 1].iParam[5] =
              mPlaySound(&res[i + 1], p_ad, 2);
        }
      }

  if (!cStartCount) {
    int idx = ddxCreateSurface(1024, 768, ddxFindFreeSurface());    

    ddxBitBlt(idx, 0, 0, 1024, 768, HDC2DD, 0, 0);

    if (RunMenuNewGame("Mmnew_game.txt", NULL, p_ad, cpu)) {
      //ddxReleaseBitmap(idx);
      goto RUN_MENU_BRUTAL_RESTART;
    }

    ddxBitBltDisplay(0, 0, 1024, 768, idx, 0, 0);

    for (i = 0; i < lastcmd; i++) {
      lastanm = 0;

      switch (res[i].iParam[0]) {
        case COM_DRAW:
          if (res[i].iLayer)
            ddxDrawSurface(FontDC, res[i].iParam, 3);
          break;
      }
    }

    ddxReleaseBitmap(idx);

    cStartCount = 1;
  }

  cRestartMainMenu = 0;
  dim.t1 = 0;
  dim.t2 = 0;
  dim.dx = 0;
  dim.dy = 0;
  anmid = -1;
  resid = -1;
  anbind = -1;
  bind = -1;
  lastabv = -1;                 // posledni animace -> co bezi?
  in = 0;
  bStop = 1;
  spracuj_spravy(0);

  dwEplased = 0;

  while (!key[K_ESC]) {
    dwStart = timeGetTime();

    //pohnul mysi
    if (dim.dx || dim.dy) {
      //dostala se mys do akcni oblasti (OnAbove)?
      if (!click)
        for (i = 0; i < lastcmd; i++)
          if (res[i].iParam[0] == COM_ONABOVE) {
            if ((dim.x >= res[i].iParam[1]) &&
              (dim.x <= res[i].iParam[3]) &&
              (dim.y >= res[i].iParam[2]) && (dim.y <= res[i].iParam[4])) {
              //spusteni animace v OnAbove
              if (i != lastabv) {
                if (in) {
                  // There's already running one - stop it
                  Stop(&res[lastabv]);

                  // Draw frame 0 -> clear the animation from screen
                  if (!res[lastabv].iLayer) {
                    ddxDrawSurface(CompositDC, res[lastabv].iAnim[0], 3);
                    ddxDrawDisplay(res[lastabv].iAnim[0], 0);
                  }
                  else {
                    ddxDrawDisplayColorKey(res[lastabv].iAnim[0], 0,
                      TRANSCOLOR);
                    ddxDrawSurface(FontDC, res[lastabv].iAnim[0], 3);
                    //menucommand_DrawT(_2dd.hDC, res[lastabv].iAnim[0]);
                    //menucommand_Draw(FontDC, res[lastabv].iAnim[0], 3);
                  }
                }

                CheckAnimation(&res[i], p_ad);

                lastabv = i;    // set last animation
                AddAnimation(&res[i], p_ad, 0, 1);
                in = 1;

                bind = ChooseBidedAnimation(res, i + 1, p_ad);

                if (bind != -1) {
                  CheckAnimation(&res[bind], p_ad);
                  AddAnimation(&res[bind], p_ad, 1, 1);
                  anbind = bind;

                  mPlaySound(&res[bind], p_ad, 0);
                }

                strcpy(dir, res[i].cParam[1]);
              }
            }
            else if (lastabv == i) {
              // odesel z oblasti, ktera byla aktivni -> stop animace                                 
              // a odznaceni oblasti
              Stop(&res[i]);

              if (!res[i].iLayer) {
                ddxDrawSurface(CompositDC, res[i].iAnim[0], 3);
                ddxDrawDisplay(res[i].iAnim[0], 0);
              }
              else {
                ddxDrawDisplayColorKey(res[i].iAnim[0], 0, TRANSCOLOR);
                ddxDrawSurface(FontDC, res[i].iAnim[0], 3);
                //menucommand_DrawT(_2dd.hDC, res[i].iAnim[0]);
                //menucommand_Draw(FontDC, res[i].iAnim[0], 3);
              }

              bind = ChooseBidedExitAnimation(res, i + 1, p_ad);

              if (bind != -1) {
                int iAnim;

                if (anbind != -1) {
                  Stop(&res[anbind]);

                  if (!res[i].iLayer) {
                    ddxDrawSurface(CompositDC, res[anbind].iAnim[0], 3);
                    ddxDrawDisplay(res[anbind].iAnim[0], 0);
                  }
                  else {
                    ddxDrawDisplayColorKey(res[anbind].iAnim[0], 0,
                      TRANSCOLOR);
                    ddxDrawSurface(FontDC, res[anbind].iAnim[0], 3);
                    //menucommand_DrawT(_2dd.hDC, res[anbind].iAnim[0]);
                    //menucommand_Draw(FontDC, res[anbind].iAnim[0], 3);
                  }
                }

                iAnim = AddAnimation(&res[bind], p_ad, 1, 1);

                if (iAnim != -1)
                  anm[iAnim].iWave = mPlaySound(&res[bind], p_ad, 2);
              }

              lastabv = -1;
              anbind = -1;
              in = 0;

              strcpy(dir, "");
            }
          }

      dim.dx = 0;
      dim.dy = 0;
    }

    //stlacil leve tlacitko
    if (dim.t1 && !click) {
      //dostala se mys do akcni oblasti (OnClick)?
      for (i = 0; i < lastcmd; i++)
        if (res[i].iParam[0] == COM_ONCLICK)
          if ((dim.x >= res[i].iParam[1]) &&
            (dim.x <= res[i].iParam[3]) &&
            (dim.y >= res[i].iParam[2]) && (dim.y <= res[i].iParam[4])) {
            if (res[i].iAnim[0][0] >= 0) {
              //pokud je animace, tak ji spust
              anmid = AddAnimation(&res[i], p_ad, 0, 1);

              if (res[i + 1].iParam[0] == COM_BINDSOUND)
                mPlaySound(&res[i + 1], p_ad, 1);

              resid = i;
              click = 1;
            }
            else {
              if (res[i + 1].iParam[0] == COM_BINDSOUND)
                mPlaySound(&res[i + 1], p_ad, 1);

              resid = i;
              click = 1;
              anmid = 31;
            }
          }

      dim.t1 = 0;
    }

    //provedeni akce po animaci menu
    if (click)
      if (!anm[anmid].cmd) {
        click = 0;

        if (!strcmp(res[resid].cParam[1], "NEW_GAME") ||
          !strcmp(res[resid].cParam[1], "LOAD_GAME") ||
          !strcmp(res[resid].cParam[1], "PLAYER_NAME"))
          bStop = 0;

        if (bStop)
          StopAll();

        if (!strcmp(res[resid].cParam[1], "CIBRA")) {
          StopAll();
          RunMenuCibron("cibra.bmp");
          click = 0;
        }

        if (!strcmp(res[resid].cParam[1], "EXIT"))
          key[K_ESC] = 1;


        if (!strcmp(res[resid].cParam[1], "PLAYER_NAME")) {
          if (RunMenuNewGame("Mmnew_game.txt", NULL, p_ad, cpu))
            goto RUN_MENU_BRUTAL_RESTART;

          bStop = 1;
        }

        if (!strcmp(res[resid].cParam[1], "CREDITS")) {
          int lid = GetPrivateProfileInt("game", "languageid", 0, ini_file);

          StopAll();
          bStop = 1;

          if (lid < 2 || lid == 4) {
            if (cr_Credits(NULL, p_ad))
              goto RUN_MENU_BRUTAL_RESTART;
          }
          else {
            if (cr_CreditsUNI(NULL, p_ad))
              goto RUN_MENU_BRUTAL_RESTART;
          }
        }

        if (!strcmp(res[resid].cParam[1], "NEW_GAME")) {
          //RunMenuNewGame("Mmnew_game.txt", hWnd, p_ad, cpu);
          RunMenuStartGame("Mmstart_game.txt", NULL, p_ad, cpu);
          if (cBrutalRestart)
            goto RUN_MENU_BRUTAL_RESTART;
        }

        if (!strcmp(res[resid].cParam[1], "LOAD_GAME")) {
          RunMenuLoadGame("Mmload_game.txt", NULL, p_ad, cpu);
          if (cBrutalRestart)
            goto RUN_MENU_BRUTAL_RESTART;
        }

        if (!strcmp(res[resid].cParam[1], "SETTINGS")) {
          RunMenuSettings("Mmsettings.txt", NULL, p_ad, cpu);

          if (cBrutalRestart)
            goto RUN_MENU_BRUTAL_RESTART;

          cRestartMainMenu = 1;
        }

        if (bStop || cRestartMainMenu)
          StopAll();

        for (i = 0; i < lastcmd; i++) {
          switch (res[i].iParam[0]) {
            case COM_DRAW:
              if (!res[i].iLayer) {
                if (bStop || cRestartMainMenu)
                  ddxDrawSurface(BackDC, res[i].iParam, 0);
              }
              else {
                ddxDrawSurfaceColorKey(BackDC, res[i].iParam, 0, TRANSCOLOR);
                ddxDrawSurface(FontDC, res[i].iParam, 3);
              }
              break;
          }
        }

        if (cRestartMainMenu || bStop) {
          ddxBitBltDisplay(0, 0, 1024, 768, BackDC, 0, 0);
          ddxCleareSurface(BackDC);
        }

        resid = -1;

        if (key[K_ESC]) {
				for(i=0;i<lastcmd;i++)
					if(res[i].iParam[0] == COM_BINDSOUND && res[i].iParam[5] != -1)
					{
						adas_Release_Source(PARTICULAR_SOUND_SOURCE, UNDEFINED_VALUE, res[i].iParam[5]);
						res[i].iParam[5] = -1;
					}
          goto __QUIT;
        }
        else
          goto BEGIN_MENU;
      }

    //pokud prisel cas, tak provedu nahodne animace (podle jejich pravdepodobnosti)
    if (timercnt > 500) {
      timercnt = 0;

      for (i = 0; i < lastcmd; i++) {
        if (res[i].iParam[0] == COM_RANDOMANIMATION) {
          if (rand() % 200 <= res[i].iParam[1] && strcmp(dir, res[i].cParam[0])) {
            CheckAnimation(&res[i], p_ad);
            AddAnimation(&res[i], p_ad, 0, 0);
          }
        }
      }
    }

    dwStop = timeGetTime();

    dwEplased += dwStop - dwStart;

    spracuj_spravy(0);
    ddxUpdateMouse();
    AnimationEvent(dwStop, p_ad);

    if (dim.tf1) {
      dim.t1 = 1;
      dim.tf1 = 0;
    }

    if (dim.tf2) {
      dim.t2 = 1;
      dim.tf2 = 0;
    }

    if (ddxRestore(p_ad))
      goto RUN_MENU_BRUTAL_RESTART;

//              updatuj_pozici_mysi();
  }

__QUIT:
	for(i=0;i<lastcmd;i++)
		if(res[i].iParam[0] == COM_BINDSOUND && res[i].iParam[5] != -1)
		{
			adas_Release_Source(PARTICULAR_SOUND_SOURCE, UNDEFINED_VALUE, res[i].iParam[5]);
			res[i].iParam[5] = -1;
		}

	//fn_Release_Font();
	adas_Release_Source(-1, ALL_TYPES, UNDEFINED_VALUE);
	adas_Release_Source(ALL_SOUND_SOURCES, ALL_TYPES,UNDEFINED_VALUE); 

  StopAll();

  ddxReleaseBitmap(iCompositDC);
  ddxReleaseBitmap(iFontDC);
  ddxReleaseBitmap(iBackDC);
  //ddxReleaseBitmap(Ltmp1DC);
//      free((void *) llcitem);
  key[K_ESC] = 0;

  FreeAnimations(res, RES_NUM);
  free((void *) res);
}

void RunMenuChildGame(char *p_File_Name, HWND hWnd, AUDIO_DATA * p_ad,
  int cpu)
{
  DWORD dwEplased = 0, dwStart, dwStop;

  int c = 0;
  int idx = 0;
  int hdcBT = 0;

  CONTROL_LIST_ITEM citem[CLIST_ITEMC];

//      FILE    *file;
  char dir[256];
  int lastcmd, lastanm, i;

  //CMD_LINE      res[RES_NUM];
  CMD_LINE *res = NULL;
  int lastabv = -1;
  char in, click = 0;
  int anmid = -1, resid = -1, anbind = -1;
  int bind;

  res = (CMD_LINE *) malloc(RES_NUM * sizeof(CMD_LINE));

  if (!res) {
    kprintf(1, "RunMenuChildGame: Out of Memory");
    return;
  }
  else
    ZeroMemory(res, RES_NUM * sizeof(CMD_LINE));

  ddxCleareSurface(FontDC);
  //ddxCleareSurface(BackDC);
/*	ddxCleareSurface(BackDC);
	ddxBitBlt(BackDC, 0, 0, ddxGetWidth(BackDC), ddxGetHight(BackDC), FontDC, 0, 0);
	ddxCleareSurface(FontDC);*/

  ZeroMemory(citem, CLIST_ITEMC * sizeof(CONTROL_LIST_ITEM));

  for (bind = 0; bind < RES_NUM; bind++) {
    for (lastcmd = 0; lastcmd < 200; lastcmd++) {
      res[bind].iAnim[lastcmd][0] = -1;
      res[bind].iAnim[lastcmd][11] = -1;
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

  fn_Set_Font(cFontFile[0]);
  fn_Load_Bitmaps();

  CreateFontAnimations(res, &lastcmd);

  fn_Release_Font(1);

  // privede prikazy, ketere se maji provest na zacatku a, kresleni, flip,
  // animace na OnAbove
  for (i = 0; i < lastcmd; i++) {
    lastanm = 0;

    switch (res[i].iParam[0]) {
      case COM_DRAW:
        {
          if (!c) {
            hdcBT = ddxCreateSurface(ddxGetWidth(res[i].iParam[1]),
              ddxGetHight(res[i].iParam[1]), ddxFindFreeSurface());
            ddxBitBlt(hdcBT, 0, 0, ddxGetWidth(res[i].iParam[1]),
              ddxGetHight(res[i].iParam[1]),
              HDC2DD, res[i].iParam[2], res[i].iParam[3]);


            ddxTransparentBlt(BackDC, res[i].iParam[2], res[i].iParam[3],
              ddxGetWidth(res[i].iParam[1]), ddxGetHight(res[i].iParam[1]),
              res[i].iParam[1], 0, 0, ddxGetWidth(res[i].iParam[1]),
              ddxGetHight(res[i].iParam[1]), TRANSCOLOR);
            idx = i;
          }
          else {
            ddxDrawSurfaceColorKey(BackDC, res[i].iParam, 2, TRANSCOLOR);
          }

          c++;
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

/*	{
		RECT r;

		r.left = 299;
		r.top = 209;
		r.right = 743;
		r.bottom = 359;

		co_Set_Text_Center(BackDC, "##menu_child_toughness", 0, r);
	}

	fn_Release_Font();*/

BEGIN_MENU_CHILDNEWGAME:

  bBackDC = 1;
  ddxTransparentBltDisplay(0, 0, 1024, 768, BackDC, 0, 0, 1024, 768,
    TRANSCOLOR);
  DisplayFrame();

  for (i = 0; i < lastcmd; i++)
    if (res[i].iParam[0] == COM_RUNANIMATION) {
      int iWave = AddAnimation(&res[i], p_ad, 0, 0);

      if (iWave != -1) {
        if (res[i + 1].iParam[0] == COM_BINDSOUND)
          anm[iWave].iWave = res[i + 1].iParam[5] =
            mPlaySound(&res[i + 1], p_ad, 2);
      }
    }

  dim.t1 = 0;
  dim.t2 = 0;
  dim.dx = 0;
  dim.dy = 0;
  anmid = -1;
  resid = -1;
  anbind = -1;
  bind = -1;
  lastabv = -1;
  in = 0;

  spracuj_spravy(0);

  while (!key[K_ESC]) {
    dwStart = timeGetTime();

    //pohnul mysi
    if (dim.dx || dim.dy) {
      //dostala se mys do akcni oblasti (OnAbove)?
      if (!click)
        for (i = 0; i < lastcmd; i++)
          if (res[i].iParam[0] == COM_ONABOVE) {
            if ((dim.x >= res[i].iParam[1]) &&
              (dim.x <= res[i].iParam[3]) &&
              (dim.y >= res[i].iParam[2]) && (dim.y <= res[i].iParam[4])) {
              //spusteni animace v OnAbove
              if (i != lastabv) {
                if (in) {
                  Stop(&res[lastabv]);

                  if (!res[lastabv].iLayer) {
                    //menucommand_Draw(_2dd.hDC, res[lastabv].iAnim[0], 0);
                    ddxDrawSurface(CompositDC, res[lastabv].iAnim[0], 3);
                    ddxDrawDisplay(res[lastabv].iAnim[0], 0);
                  }
                  else {
                    //menucommand_DrawT(_2dd.hDC, res[lastabv].iAnim[0]);
                    ddxDrawDisplayColorKey(res[lastabv].iAnim[0], 0,
                      TRANSCOLOR);
                    //menucommand_Draw(FontDC, res[lastabv].iAnim[0], 3);
                    ddxDrawSurface(FontDC, res[lastabv].iAnim[0], 3);
                  }
                }

                CheckAnimation(&res[i], p_ad);

                lastabv = i;
                AddAnimation(&res[i], p_ad, 0, 1);
                in = 1;

                bind = ChooseBidedAnimation(res, i + 1, p_ad);

                if (bind != -1) {
                  CheckAnimation(&res[bind], p_ad);
                  AddAnimation(&res[bind], p_ad, 1, 1);
                  anbind = bind;

                  mPlaySound(&res[bind], p_ad, 0);
                }

                strcpy(dir, res[i].cParam[1]);
              }
            }
            else if (lastabv == i) {
              // odesel z oblasti, ktera byla aktivni -> stop animace                                 
              // a odznaceni oblasti
              Stop(&res[i]);

              if (!res[i].iLayer) {
                //menucommand_Draw(_2dd.hDC, res[i].iAnim[0], 0);
                ddxDrawDisplay(res[i].iAnim[0], 0);
                ddxDrawSurface(CompositDC, res[i].iAnim[0], 3);
              }
              else {
                //menucommand_DrawT(_2dd.hDC, res[i].iAnim[0]);
                ddxDrawDisplayColorKey(res[i].iAnim[0], 0, TRANSCOLOR);
                //menucommand_DrawT(FontDC, res[i].iAnim[0]);
                ddxDrawSurface(FontDC, res[i].iAnim[0], 3);
              }

              bind = ChooseBidedExitAnimation(res, i + 1, p_ad);

              if (bind != -1) {
                int iAnim;

                if (anbind != -1) {
                  Stop(&res[anbind]);

                  if (!res[i].iLayer) {
                    //menucommand_Draw(_2dd.hDC, res[anbind].iAnim[0], 0);
                    ddxDrawDisplay(res[anbind].iAnim[0], 0);
                    ddxDrawSurface(CompositDC, res[anbind].iAnim[0], 3);
                  }
                  else {
                    //menucommand_DrawT(_2dd.hDC, res[anbind].iAnim[0]);
                    ddxDrawDisplayColorKey(res[anbind].iAnim[0], 0,
                      TRANSCOLOR);
                    //menucommand_DrawT(FontDC, res[anbind].iAnim[0]);
                    ddxDrawSurface(FontDC, res[anbind].iAnim[0], 3);
                  }
                }

                iAnim = AddAnimation(&res[bind], p_ad, 1, 1);

                if (iAnim != -1)
                  anm[iAnim].iWave = mPlaySound(&res[bind], p_ad, 2);
              }

              lastabv = -1;
              anbind = -1;
              in = 0;

              strcpy(dir, "");
            }
          }

      dim.dx = 0;
      dim.dy = 0;
    }

    //stlacil leve tlacitko
    if (dim.t1 && !click) {
      //dostala se mys do akcni oblasti (OnClick)?
      for (i = 0; i < lastcmd; i++)
        if (res[i].iParam[0] == COM_ONCLICK)
          if ((dim.x >= res[i].iParam[1]) &&
            (dim.x <= res[i].iParam[3]) &&
            (dim.y >= res[i].iParam[2]) && (dim.y <= res[i].iParam[4])) {
            if (res[i].iAnim[0][0] >= 0) {
              //pokud je animace, tak ji spust
              anmid = AddAnimation(&res[i], p_ad, 0, 1);

              if (res[i + 1].iParam[0] == COM_BINDSOUND)
                mPlaySound(&res[i + 1], p_ad, 1);

              resid = i;
              click = 1;
            }
            else {
              if (res[i + 1].iParam[0] == COM_BINDSOUND)
                mPlaySound(&res[i + 1], p_ad, 1);

              resid = i;
              click = 1;
              anmid = 31;
            }
          }

      dim.t1 = 0;
    }

    //provedeni akce po animaci menu
    if (click)
      if (!anm[anmid].cmd) {
        click = 0;

        //StopAll();

        if (!strcmp(res[resid].cParam[1], "EXIT") ||
          !strcmp(res[resid].cParam[1], "CANCEL")) {
          key[K_ESC] = 1;
          //break;
        }

        if (!strcmp(res[resid].cParam[1], "EASY")) {
          if (iLanguageVersion == 4 && !CheckScenePresence(10)) {
            RunMenuCibron("buy.bmp");
            click = 0;
          }
          else {
            StopAll();
            bBackDC = 0;
            iActualScene = 10;

            RunMenuNewGameScene("Mmnew_game_scene.txt", NULL, p_ad, cpu, 0, NULL, 0);
            key[K_ESC] = 1;
          }
        }

        if (!strcmp(res[resid].cParam[1], "NORMAL")) {
          if (iLanguageVersion == 4 && !CheckScenePresence(11)) {
            RunMenuCibron("buy.bmp");
            click = 0;
          }
          else {
            StopAll();
            bBackDC = 0;
            iActualScene = 11;

            RunMenuNewGameScene("Mmnew_game_scene.txt", NULL, p_ad, cpu, 0,
              NULL, 0);

            key[K_ESC] = 1;
          }
        }

        if (!strcmp(res[resid].cParam[1], "HARD")) {
          if (iLanguageVersion == 4 && !CheckScenePresence(12)) {
            RunMenuCibron("buy.bmp");
            click = 0;
          }
          else {
            StopAll();
            bBackDC = 0;
            iActualScene = 12;

            RunMenuNewGameScene("Mmnew_game_scene.txt", NULL, p_ad, cpu, 0,
              NULL, 0);

            key[K_ESC] = 1;
          }
        }

        if (!cRestartMainMenu)
          for (i = 0; i < lastcmd; i++) {
            switch (res[i].iParam[0]) {
              case COM_DRAW:
                if (!res[i].iLayer) {
                  //menucommand_Draw(_2dd.hDC, res[i].iParam);
                }
                else {
                  //menucommand_DrawT(_2dd.hDC, res[i].iParam);
                  ddxDrawDisplayColorKey(res[i].iParam, 0, TRANSCOLOR);

                  //menucommand_DrawT(BackDC, res[i].iParam);
                  ddxDrawSurfaceColorKey(BackDC, res[i].iParam, 2,
                    TRANSCOLOR);
                  //menucommand_Draw(FontDC, res[i].iParam, 3);
                  ddxDrawSurface(FontDC, res[i].iParam, 3);
                }
                break;
            }
          }

        resid = -1;

        if (key[K_ESC]) {
				for(i=0;i<lastcmd;i++)
					if(res[i].iParam[0] == COM_BINDSOUND && res[i].iParam[5] != -1)
					{
						adas_Release_Source(PARTICULAR_SOUND_SOURCE, UNDEFINED_VALUE, res[i].iParam[5]);
						res[i].iParam[5] = -1;
					}
          goto __QUIT;
        }
        else
          goto BEGIN_MENU_CHILDNEWGAME;
      }

    //pokud prisel cas, tak provedu nahodne animace (podle jejich pravdepodobnosti)
    if (timercnt > 500) {
      timercnt = 0;

      for (i = 0; i < lastcmd; i++)
        if (res[i].iParam[0] == COM_RANDOMANIMATION)
          if (rand() % 200 <= res[i].iParam[1] &&
            strcmp(dir, res[i].cParam[0])) {
            CheckAnimation(&res[i], p_ad);
            AddAnimation(&res[i], p_ad, 0, 0);
          }
    }

    dwStop = timeGetTime();

    dwEplased += dwStop - dwStart;

    spracuj_spravy(0);
    ddxUpdateMouse();
    AnimationEvent(dwStop, p_ad);

    if (dim.tf1) {
      dim.t1 = 1;
      dim.tf1 = 0;
    }

    if (dim.tf2) {
      dim.t2 = 1;
      dim.tf2 = 0;
    }

    if (ddxRestore(p_ad)) {
      cRestartMainMenu = 1;
      key[K_ESC] = 1;
    }
  }

__QUIT:

  ddxCleareSurface(FontDC);

/*	if(!cRestartMainMenu)
		ddxTransparentBltDisplay( 0, 0, 1024, 768, BackDC, 0, 0, 1024, 768, TRANSCOLOR);*/

  //ddxCleareSurface(BackDC);

  if (!cRestartMainMenu) {
    ddxBitBltDisplay(res[idx].iParam[2], res[idx].iParam[3],
      ddxGetWidth(res[idx].iParam[1]),
      ddxGetHight(res[idx].iParam[1]), hdcBT, 0, 0);

    ddxTransparentBlt(BackDC, res[idx].iParam[2], res[idx].iParam[3],
      ddxGetWidth(res[idx].iParam[1]),
      ddxGetHight(res[idx].iParam[1]),
      res[idx].iParam[1], 0, 0, ddxGetWidth(res[idx].iParam[1]),
      ddxGetHight(res[idx].iParam[1]), TRANSCOLOR);
  }

  //BRUTAL_RELEASE .... nenicit
  if (!cBrutalRestart)
    ddxReleaseBitmap(hdcBT);

  ddxCleareSurface(CompositDC);
  key[K_ESC] = 0;

  FreeAnimations(res, RES_NUM);

  free((void *) res);
}


void RunMenuStartGame(char *p_File_Name, HWND hWnd, AUDIO_DATA * p_ad,
  int cpu)
{
  DWORD dwEplased = 0, dwStart, dwStop;

  int c = 0;
  int idx = 0;
  int hdcBT = 0;

  CONTROL_LIST_ITEM citem[CLIST_ITEMC];

//      FILE    *file;
  char dir[256];
  int lastcmd, lastanm, i;

  //CMD_LINE      res[RES_NUM];
  CMD_LINE *res = NULL;
  int lastabv = -1;
  char in, click = 0;
  int anmid = -1, resid = -1, anbind = -1;
  int bind;

  res = (CMD_LINE *) malloc(RES_NUM * sizeof(CMD_LINE));

  if (!res) {
    kprintf(1, "RunMenuLoadGame: Out of Memory");
    return;
  }
  else
    ZeroMemory(res, RES_NUM * sizeof(CMD_LINE));

  //bBackDC = 1;

  //ddxCleareSurface(FontDC);
  //ddxCleareSurface(BackDC);
  ddxCleareSurface(BackDC);
  ddxBitBlt(BackDC, 0, 0, ddxGetWidth(BackDC), ddxGetHight(BackDC), FontDC, 0,
    0);
  ddxCleareSurface(FontDC);

  //kprintf(1, "bitblt");
  ZeroMemory(citem, CLIST_ITEMC * sizeof(CONTROL_LIST_ITEM));

  for (bind = 0; bind < RES_NUM; bind++) {
    for (lastcmd = 0; lastcmd < 200; lastcmd++) {
      res[bind].iAnim[lastcmd][0] = -1;
      res[bind].iAnim[lastcmd][11] = -1;
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

  //kprintf(1, "load");
  in = 0;

  fn_Set_Font(cFontFile[0]);
  fn_Load_Bitmaps();

  //kprintf(1, "fn_Set_Font");
  CreateFontAnimations(res, &lastcmd);
  fn_Release_Font(1);

  //kprintf(1, "CreateFontAnimations");

  // privede prikazy, ketere se maji provest na zacatku a, kresleni, flip,
  // animace na OnAbove
  for (i = 0; i < lastcmd; i++) {
    lastanm = 0;

    switch (res[i].iParam[0]) {
      case COM_DRAW:
        {
          if (!c) {
            hdcBT = ddxCreateSurface(ddxGetWidth(res[i].iParam[1]),
              ddxGetHight(res[i].iParam[1]), ddxFindFreeSurface());

            ddxBitBlt(hdcBT, 0, 0, ddxGetWidth(res[i].iParam[1]),
              ddxGetHight(res[i].iParam[1]),
              HDC2DD, res[i].iParam[2], res[i].iParam[3]);

            ddxTransparentBlt(BackDC, res[i].iParam[2], res[i].iParam[3],
              ddxGetWidth(res[i].iParam[1]), ddxGetHight(res[i].iParam[1]),
              res[i].iParam[1], 0, 0, ddxGetWidth(res[i].iParam[1]),
              ddxGetHight(res[i].iParam[1]), TRANSCOLOR);

            idx = i;
          }
          else {
            ddxDrawSurfaceColorKey(BackDC, res[i].iParam, 2, TRANSCOLOR);
          }
          c++;
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

BEGIN_MENU_NEWGAME:

  bBackDC = 1;
  //TransparentBltU(_2dd.hDC, 0, 0, 1024, 768, BackDC, 0, 0, 1024, 768, RGB(255, 0, 255));
  ddxTransparentBltDisplay(0, 0, 1024, 768, BackDC, 0, 0, 1024, 768,
    TRANSCOLOR);
  DisplayFrame();

  for (i = 0; i < lastcmd; i++)
    if (res[i].iParam[0] == COM_RUNANIMATION) {
      int iWave = AddAnimation(&res[i], p_ad, 0, 0);

      if (iWave != -1) {
        if (res[i + 1].iParam[0] == COM_BINDSOUND)
          anm[iWave].iWave = res[i + 1].iParam[5] =
            mPlaySound(&res[i + 1], p_ad, 2);
      }
    }

  dim.t1 = 0;
  dim.t2 = 0;
  dim.dx = 0;
  dim.dy = 0;
  anmid = -1;
  resid = -1;
  anbind = -1;
  bind = -1;
  lastabv = -1;
  in = 0;

  spracuj_spravy(0);

  while (!key[K_ESC]) {
    dwStart = timeGetTime();

    //pohnul mysi
    if (dim.dx || dim.dy) {
      //dostala se mys do akcni oblasti (OnAbove)?
      if (!click)
        for (i = 0; i < lastcmd; i++)
          if (res[i].iParam[0] == COM_ONABOVE) {
            if ((dim.x >= res[i].iParam[1]) &&
              (dim.x <= res[i].iParam[3]) &&
              (dim.y >= res[i].iParam[2]) && (dim.y <= res[i].iParam[4])) {
              //spusteni animace v OnAbove
              if (i != lastabv) {
                if (in) {
                  Stop(&res[lastabv]);

                  if (!res[lastabv].iLayer) {
                    //menucommand_Draw(_2dd.hDC, res[lastabv].iAnim[0], 0);
                    ddxDrawSurface(CompositDC, res[lastabv].iAnim[0], 3);
                    ddxDrawDisplay(res[lastabv].iAnim[0], 0);
                  }
                  else {
                    //menucommand_DrawT(_2dd.hDC, res[lastabv].iAnim[0]);
                    ddxDrawDisplayColorKey(res[lastabv].iAnim[0], 0,
                      TRANSCOLOR);
                    //menucommand_Draw(FontDC, res[lastabv].iAnim[0], 3);
                    ddxDrawSurface(FontDC, res[lastabv].iAnim[0], 3);
                  }
                }

                CheckAnimation(&res[i], p_ad);

                lastabv = i;
                AddAnimation(&res[i], p_ad, 0, 1);
                in = 1;

                bind = ChooseBidedAnimation(res, i + 1, p_ad);

                if (bind != -1) {
                  CheckAnimation(&res[bind], p_ad);
                  AddAnimation(&res[bind], p_ad, 1, 1);
                  anbind = bind;

                  mPlaySound(&res[bind], p_ad, 0);
                }

                strcpy(dir, res[i].cParam[1]);
              }
            }
            else if (lastabv == i) {
              // odesel z oblasti, ktera byla aktivni -> stop animace                                 
              // a odznaceni oblasti
              Stop(&res[i]);

              if (!res[i].iLayer) {
                //menucommand_Draw(_2dd.hDC, res[i].iAnim[0], 0);
                ddxDrawDisplay(res[i].iAnim[0], 0);
                ddxDrawSurface(CompositDC, res[i].iAnim[0], 3);
              }
              else {
                //menucommand_DrawT(_2dd.hDC, res[i].iAnim[0]);
                ddxDrawDisplayColorKey(res[i].iAnim[0], 0, TRANSCOLOR);
                //menucommand_DrawT(FontDC, res[i].iAnim[0]);
                ddxDrawSurface(FontDC, res[i].iAnim[0], 3);
              }

              bind = ChooseBidedExitAnimation(res, i + 1, p_ad);

              if (bind != -1) {
                int iAnim;

                if (anbind != -1) {
                  Stop(&res[anbind]);

                  if (!res[i].iLayer) {
                    //menucommand_Draw(_2dd.hDC, res[anbind].iAnim[0], 0);
                    ddxDrawDisplay(res[anbind].iAnim[0], 0);
                    ddxDrawSurface(CompositDC, res[anbind].iAnim[0], 3);
                  }
                  else {
                    //menucommand_DrawT(_2dd.hDC, res[anbind].iAnim[0]);
                    ddxDrawDisplayColorKey(res[anbind].iAnim[0], 0,
                      TRANSCOLOR);
                    //menucommand_DrawT(FontDC, res[anbind].iAnim[0]);
                    ddxDrawSurface(FontDC, res[anbind].iAnim[0], 3);
                  }
                }

                iAnim = AddAnimation(&res[bind], p_ad, 1, 1);

                if (iAnim != -1)
                  anm[iAnim].iWave = mPlaySound(&res[bind], p_ad, 2);
              }

              lastabv = -1;
              anbind = -1;
              in = 0;

              strcpy(dir, "");
            }
          }

      dim.dx = 0;
      dim.dy = 0;
    }

    //stlacil leve tlacitko
    if (dim.t1 && !click) {
      //dostala se mys do akcni oblasti (OnClick)?
      for (i = 0; i < lastcmd; i++)
        if (res[i].iParam[0] == COM_ONCLICK)
          if ((dim.x >= res[i].iParam[1]) &&
            (dim.x <= res[i].iParam[3]) &&
            (dim.y >= res[i].iParam[2]) && (dim.y <= res[i].iParam[4])) {
            if (res[i].iAnim[0][0] >= 0) {
              //pokud je animace, tak ji spust
              anmid = AddAnimation(&res[i], p_ad, 0, 1);

              if (res[i + 1].iParam[0] == COM_BINDSOUND)
                mPlaySound(&res[i + 1], p_ad, 1);

              resid = i;
              click = 1;
            }
            else {
              if (res[i + 1].iParam[0] == COM_BINDSOUND)
                mPlaySound(&res[i + 1], p_ad, 1);

              resid = i;
              click = 1;
              anmid = 31;
            }
          }

      dim.t1 = 0;
    }

    //provedeni akce po animaci menu
    if (click)
      if (!anm[anmid].cmd) {
        click = 0;

        //StopAll();

        if (!strcmp(res[resid].cParam[1], "EXIT") ||
          !strcmp(res[resid].cParam[1], "CANCEL")) {
          key[K_ESC] = 1;
          //break;
        }

        if (!strcmp(res[resid].cParam[1], "NEW_GAME")) {
          iActualScene = 1;
          StopAll();
          RunMenuNewGameScene("Mmnew_game_scene.txt", NULL, p_ad, cpu, 0,
            NULL, 1);
          key[K_ESC] = 1;
        }

        if (!strcmp(res[resid].cParam[1], "CHILD_SCENE")) {
          RunMenuChildGame("Mmstart_child_game.txt", NULL, p_ad, cpu);

          if (cRestartMainMenu) {
            StopAll();
            key[K_ESC] = 1;
          }
        }

        if (!strcmp(res[resid].cParam[1], "TUTORIAL")) {
          StopAll();
          bBackDC = 0;
          iActualScene = 0;

          RunMenuNewGameScene("Mmnew_game_scene.txt", NULL, p_ad, cpu, 0,
            NULL, 0);

          key[K_ESC] = 1;
        }

        if (!cRestartMainMenu)
          for (i = 0; i < lastcmd; i++) {
            switch (res[i].iParam[0]) {
              case COM_DRAW:
                if (!res[i].iLayer) {
                  //menucommand_Draw(_2dd.hDC, res[i].iParam);
                }
                else {
                  ddxDrawDisplayColorKey(res[i].iParam, 0, TRANSCOLOR);
                  ddxDrawSurfaceColorKey(BackDC, res[i].iParam, 2,
                    TRANSCOLOR);
                  ddxDrawSurface(FontDC, res[i].iParam, 3);
                }
                break;
            }
          }

        resid = -1;

        if (key[K_ESC]) {
				for(i=0;i<lastcmd;i++)
					if(res[i].iParam[0] == COM_BINDSOUND && res[i].iParam[5] != -1)
					{
						adas_Release_Source(PARTICULAR_SOUND_SOURCE, UNDEFINED_VALUE, res[i].iParam[5]);
						res[i].iParam[5] = -1;
					}
          goto __QUIT;
        }
        else
          goto BEGIN_MENU_NEWGAME;
      }

    //pokud prisel cas, tak provedu nahodne animace (podle jejich pravdepodobnosti)
    if (timercnt > 500) {
      timercnt = 0;

      for (i = 0; i < lastcmd; i++)
        if (res[i].iParam[0] == COM_RANDOMANIMATION)
          if (rand() % 200 <= res[i].iParam[1] &&
            strcmp(dir, res[i].cParam[0])) {
            CheckAnimation(&res[i], p_ad);
            AddAnimation(&res[i], p_ad, 0, 0);
          }
    }

    dwStop = timeGetTime();

    dwEplased += dwStop - dwStart;

    spracuj_spravy(0);
    ddxUpdateMouse();
    AnimationEvent(dwStop, p_ad);

    if (dim.tf1) {
      dim.t1 = 1;
      dim.tf1 = 0;
    }

    if (dim.tf2) {
      dim.t2 = 1;
      dim.tf2 = 0;
    }

    if (ddxRestore(p_ad)) {
      cRestartMainMenu = 1;
      key[K_ESC] = 1;
    }
  }

__QUIT:

  ddxCleareSurface(FontDC);
  if (!cRestartMainMenu)
    ddxTransparentBltDisplay(0, 0, 1024, 768, BackDC, 0, 0, 1024, 768,
      TRANSCOLOR);
  ddxCleareSurface(BackDC);

  bBackDC = 0;

  if (!cRestartMainMenu) {
    ddxBitBltDisplay(res[idx].iParam[2], res[idx].iParam[3],
      ddxGetWidth(res[idx].iParam[1]),
      ddxGetHight(res[idx].iParam[1]), hdcBT, 0, 0);
  }


  //BRUTAL_RELEASE .... nenicit
  if (!cBrutalRestart)
    ddxReleaseBitmap(hdcBT);

  ddxCleareSurface(CompositDC);
  key[K_ESC] = 0;

  FreeAnimations(res, RES_NUM);

  free((void *) res);
}

int LoadCList(char *cFile, int *p_count, int *p_bmp, int *iClock,
  APAK_HANDLE * hArchive)
{
  int c = 0, i = -1, t;
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

      if (!c) {
        i = ddxLoadBitmap(text, hArchive);
        p_bmp[c] = i;
      }
      else {
        t = ddxLoadBitmap(text, hArchive);
        p_bmp[c] = t;
      }

      DrawClock(iClock, 0);

      c++;
    }
  }

  aclose(file);

  pBmpArchive->bError = 0;

  (*p_count) = c;

  return i;
}

int GetComixTime(int iScene)
{
  char text[256] = "";
  int i;
  FILE *file;

  file = aopen(pDataArchive, "comix_times.txt", "r");

  if (!file)
    return 1000;

  for (i = 0; i < iScene + 1; i++)
    agets(text, 256, file);

  aclose(file);

  return atoi(text);
}

int RunMenuComixB(char *p_File_Name, HWND hWnd, AUDIO_DATA * p_ad, int iScene)
{
  DWORD dwEplased = 0,
        dwStart = 0,
        dwStop = 0;
  FILE *file;
  char dir[256];
  int lastcmd, lastanm, i, j;
  CMD_LINE *res = NULL;
  int lastabv = -1;
  char in, click = 0;
  int anmid = -1, resid = -1, anbind = -1;
  int bind;
  int cc = 0;
  int iTVBmp = -1;
  int iTVTBmp = -1;
  int iTVBut = -1;
  int iComix = -1;
  char ccomix[64];
  int y = 0;
  int iSongTime = 1000;
  int ccc;
  int iClock;

  iClock = ddxLoadBitmap("clock1-1.bmp", pBmpArchive);
  ddxResizeCursorBack(iClock);
  DrawClock(&iClock, 0);

	if(ogg_playing())
	{
		float f;

		f = p_ad->Music_Gain;

		while(f >= 0.05f)
		{
			f -= 0.05f;
			ogg_gain(f);
			Sleep(25);
		}

		ap_Stop_Song(p_ad);
	}

  Sleep(1000);

  bBackDC = 0;

  res = (CMD_LINE *) mmalloc(RES_NUM * sizeof(CMD_LINE));

  _2d_Clear_RectLine(&rline);

  ddxSetFlip(TRUE);

  cc = 0;
  anmid = -1;
  resid = -1;
  anbind = -1;

  ddxCleareSurface(CompositDC);
  ddxCleareSurface(FontDC);

  DrawClock(&iClock, 0);
  iTVBmp = ddxLoadBitmap("televize.bmp", pBmpArchive);
  iTVTBmp = ddxLoadBitmap("televizet.bmp", pBmpArchive);

  DrawClock(&iClock, 0);
  iTVBut = LoadCList("telload.txt", &ccc, &ccc, &ccc, pBmpArchive);

  sprintf(ccomix, "comix%d.bmp", iScene);  
  iComix = ddxLoadBitmap(ccomix, pBmpArchive);

  if (iTVBmp == -1 || iTVBut == -1 || iComix == -1 || iTVTBmp == -1) {
    kerror(1, "Unable to load comix graphics! (iTVBmp == %d, iTVBut == %d, iComix == %d, iTVTBmp == %d)",
           iTVBmp,iTVBut,iComix,iTVTBmp);
  }

  for (bind = 0; bind < RES_NUM; bind++) {
    for (lastcmd = 0; lastcmd < 200; lastcmd++) {
      res[bind].iAnim[lastcmd][0] = -1;
      res[bind].iAnim[lastcmd][11] = -1;
    }

    for (in = 0; in < 6; in++)
      res[bind].iParam[(int)in] = -1;

    res[bind].pCmdLine = res;
  }

  lastcmd = 0;
  timercnt = 0;

  GetPrivateProfileString("game", "data_dir", "", dir, 256, ini_file);
  if(!dir[0])
    kerror(TRUE, "Unable to read data_dir from ini_file!", ini_file);
  working_file_translate(dir, 256);
  chdir((dir));

  //natadhe skript menu
  LoadMenuScript(p_File_Name, res, &lastcmd);

  in = 0;

  //kerekce televize
  res[0].iParam[1] = iTVBmp;

  //korekce animace komixu;
  //strcpy(res[1].cParam[0], ccomix);

  DrawClock(&iClock, 0);
  // privede prikazy, ketere se maji provest na zacatku a, kresleni, flip,
  // animace na OnAbove
  for (i = 0; i < lastcmd; i++) {
    lastanm = 0;

    switch (res[i].iParam[0]) {
      case COM_DRAW:
        if (!res[i].iLayer)
          ddxDrawSurface(BackDC, res[i].iParam, 0);
        else {
          ddxDrawSurfaceColorKey(BackDC, res[i].iParam, 2, TRANSCOLOR);
          ddxDrawSurface(FontDC, res[i].iParam, 3);
        }
        break;
      case COM_RANDOMANIMATION:
      case COM_ONCLICK:
      case COM_ONABOVE:
      case COM_RUNANIMATION:
      case COM_BINDEXITANIMATION:
      case COM_BINDANIMATION:
        file = aopen(pDataArchive, res[i].cParam[0], "r");
        if (file) {
          while (!aeof(file)) {
            Parse_AnimLine(file, res[i].iAnim[lastanm], 18);
            lastanm++;
          }

          aclose(file);

          for (j = 0; j < lastanm; j++)
            res[i].iAnim[j][1] += iTVBut;
        }

        cc++;

        break;
    }
  }

  DrawClock(&iClock, 0);
  ddxSetCursor(0);
  DisplayFrame();
  DisplayFrame();
  ddxResizeCursorBack(0);
  ddxSetCursorSurface(0);
  ddxSetCursor(1);
  DisplayFrame();
  DisplayFrame();
  ddxReleaseBitmap(iClock);

  ddxBitBltDisplay(0, 0, 1024, 768, BackDC, 0, 0);
  ddxCleareSurface(BackDC);

  ddxTransparentBlt(FontDC, 0, 0, 1024, 768, iTVTBmp, 0, 0, 1024, 768,
    TRANSCOLOR);
  _2d_Clear_RectLine(&rline);

  for (i = 0; i < lastcmd; i++)
    if (res[i].iParam[0] == COM_RUNANIMATION) {
      int iWave = AddAnimation(&res[i], p_ad, 0, 0);

      if (iWave != -1) {
        if (res[i + 1].iParam[0] == COM_BINDSOUND)
          anm[iWave].iWave = res[i + 1].iParam[5] =
            mPlaySound(&res[i + 1], p_ad, 2);
      }
    }

  dim.t1 = 0;
  dim.t2 = 0;
  dim.dx = 0;
  dim.dy = 0;
  anmid = -1;
  resid = -1;
  anbind = -1;
  bind = -1;
  lastabv = -1;
  in = 0;

  spracuj_spravy(0);

	if(ogg_playing())
		ap_Stop_Song(p_ad);

  iSongTime = GetComixTime(iScene);

  ap_Play_Song(iScene+1,0, p_ad);

  dwLTime = timeGetTime();

  ddxSetFlip(FALSE);

  dwStart = timeGetTime();

  while (!key[K_ESC]) {

    //pohnul mysi
    if (dim.dx || dim.dy) {
      //dostala se mys do akcni oblasti (OnAbove)?
      if (!click)
        for (i = 0; i < lastcmd; i++)
          if (res[i].iParam[0] == COM_ONABOVE) {
            if ((dim.x >= res[i].iParam[1]) &&
              (dim.x <= res[i].iParam[3]) &&
              (dim.y >= res[i].iParam[2]) && (dim.y <= res[i].iParam[4])) {
              //spusteni animace v OnAbove
              if (i != lastabv) {
                if (in) {
                  if (res[lastabv].iAnim[0][0] != -1) {
                    Stop(&res[lastabv]);

                    if (!res[lastabv].iLayer) {
                      ddxDrawDisplay(res[lastabv].iAnim[0], 0);
                      ddxDrawSurface(CompositDC, res[lastabv].iAnim[0], 3);
                    }
                    else {
                      ddxDrawDisplayColorKey(res[lastabv].iAnim[0], 0,
                        TRANSCOLOR);
                      ddxDrawSurface(FontDC, res[lastabv].iAnim[0], 3);
                    }
                  }
                }

                CheckAnimation(&res[i], p_ad);

                lastabv = i;
                AddAnimation(&res[i], p_ad, 0, 1);
                in = 1;

                bind = ChooseBidedAnimation(res, i + 1, p_ad);

                if (bind != -1) {
                  CheckAnimation(&res[bind], p_ad);
                  AddAnimation(&res[bind], p_ad, 1, 1);
                  anbind = bind;

                  mPlaySound(&res[bind], p_ad, 0);
                }

                strcpy(dir, res[i].cParam[1]);
              }
            }
            else if (lastabv == i) {
              // odesel z oblasti, ktera byla aktivni -> stop animace                                 
              // a odznaceni oblasti
              Stop(&res[i]);

              if (res[i].iAnim[0][0] != -1) {
                if (!res[i].iLayer) {
                  ddxDrawDisplay(res[i].iAnim[0], 0);
                  ddxDrawSurface(CompositDC, res[i].iAnim[0], 3);
                }
                else {
                  ddxDrawDisplayColorKey(res[i].iAnim[0], 0, TRANSCOLOR);
                  ddxDrawSurface(FontDC, res[i].iAnim[0], 3);
                }
              }

              bind = ChooseBidedExitAnimation(res, i + 1, p_ad);

              if (bind != -1) {
                int iAnim;

                if (anbind != -1) {
                  Stop(&res[anbind]);

                  if (!res[anbind].iLayer) {
                    ddxDrawDisplay(res[anbind].iAnim[0], 0);
                    ddxDrawSurface(CompositDC, res[anbind].iAnim[0], 3);
                  }
                  else {
                    ddxDrawDisplayColorKey(res[anbind].iAnim[0], 0,
                      TRANSCOLOR);
                    ddxDrawSurface(FontDC, res[anbind].iAnim[0], 3);
                  }
                }

                iAnim = AddAnimation(&res[bind], p_ad, 1, 1);

                if (iAnim != -1)
                  anm[iAnim].iWave = mPlaySound(&res[bind], p_ad, 2);
              }

              lastabv = -1;
              anbind = -1;
              in = 0;

              strcpy(dir, "");
            }
          }
    }

    //stlacil leve tlacitko
    if (dim.t1 && !click) {
      //dostala se mys do akcni oblasti (OnClick)?
      for (i = 0; i < lastcmd; i++)
        if (res[i].iParam[0] == COM_ONCLICK)
          if ((dim.x >= res[i].iParam[1]) &&
            (dim.x <= res[i].iParam[3]) &&
            (dim.y >= res[i].iParam[2]) && (dim.y <= res[i].iParam[4])) {
            if (res[i].iAnim[0][0] >= 0) {
              //pokud je animace, tak ji spust
              anmid = AddAnimation(&res[i], p_ad, 0, 1);

              if (res[i + 1].iParam[0] == COM_BINDSOUND)
                mPlaySound(&res[i + 1], p_ad, 1);

              resid = i;
              click = 1;
            }
            else {
              if (res[i + 1].iParam[0] == COM_BINDSOUND)
                mPlaySound(&res[i + 1], p_ad, 1);

              resid = i;
              click = 1;
              anmid = 31;
            }
          }

      dim.t1 = 0;
    }

    //provedeni akce po animaci menu
    if (click)
      if (!anm[anmid].cmd) {
        click = 0;

        StopAll();

        if (!strcmp(res[resid].cParam[1], "EXIT") ||
          !strcmp(res[resid].cParam[1], "CANCEL")) {
          key[K_ESC] = 1;
          cRestartMainMenu = 1;
        }

        resid = -1;

        if (key[K_ESC]) {
				for(i=0;i<lastcmd;i++)
					if(res[i].iParam[0] == COM_BINDSOUND && res[i].iParam[5] != -1)
					{
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
            CheckAnimation(&res[i], p_ad);
            AddAnimation(&res[i], p_ad, 0, 0);
          }
    }

    //dwStop = timeGetTime();

    //dwEplased += dwStop - dwLTime;

    //dwStart = timeGetTime();
    spracuj_spravy(0);
    ddxUpdateMouse();

    {
      RECT rr;

      rr.left = 124;
      rr.top = 161;
      rr.right = 700;
      rr.bottom = 549;

      y =
        (int) ftoi((dwEplased * (ddxGetHight(iComix) -
            550)) / (float) iSongTime);

      if (y >= ddxGetHight(iComix) - 550)
        key[K_ESC] = 1;

      ddxBitBlt(CompositDC, 124, 161, ddxGetWidth(iComix), 549, iComix, 0, y);

      _2d_Add_RectItem(&rline, rr, 0);
    }

    AnimationEvent(dwStop, p_ad);

    if (dim.tf1) {
      dim.t1 = 1;
      dim.tf1 = 0;
    }

    if (dim.tf2) {
      dim.t2 = 1;
      dim.tf2 = 0;
    }

    dwStop = timeGetTime();

    dwEplased = dwStop - dwStart;
    //kprintf(1, "%d", dwEplased);

    if (ddxRestore(p_ad))
      return 1;
  }

__QUIT:

  //kprintf(1, "%d", dwEplased);
	adas_Release_Source(-1, ALL_TYPES, UNDEFINED_VALUE);
	adas_Release_Source(ALL_SOUND_SOURCES, ALL_TYPES,UNDEFINED_VALUE); 

  StopAll();

  ddxReleaseBitmap(iTVBmp);
  ddxReleaseBitmap(iTVTBmp);

  for (i = iTVBut; i < iTVBut + 9; i++)
    ddxReleaseBitmap(i);

  ddxReleaseBitmap(iComix);

  ddxCleareSurface(FontDC);
  ddxCleareSurface(BackDC);
  ddxCleareSurface(CompositDC);

  key[K_ESC] = 0;

  FreeAnimations(res, RES_NUM);
  free((void *) res);

	if(ogg_playing())
		ap_Stop_Song(p_ad);

  ap_Play_Song(0, 0, p_ad);

  ddxSetFlip(TRUE);

  return 0;
}

int RunMenuComix(char *p_File_Name, HWND hWnd, AUDIO_DATA * p_ad, int iScene)
{
  DWORD dwEplased = 0, dwStart, dwStop;

  //int iComix = -1;
  char ccomix[64];
  int iSongTime = 1000;
  int bmpc = 0;
  int iClock;
  int idx = -1;
  int lidx = -1;
  int bmp[64];
  int i;
  APAK_HANDLE *hArchive = NULL;
  char cFile[MAX_PATH];
  char cDir[MAX_PATH];

  if (iLanguageVersion == 4) {
    GetPrivateProfileString("game", "bitmap_dir", "c:\\", cDir, MAX_PATH,
      ini_file);
    working_file_translate(cDir, MAX_PATH);
    sprintf(cFile, "%s%cscene%d.pak", cDir, DIR_SLASH, iScene);
    hArchive = apakopen(cFile, cDir, &idx);

    if (!hArchive) {
      kprintf(1, "Nepodarilo se otevrit archiv [%s]\n", cFile);
      return 0;
    }
    else
      hArchive->pActualNode = hArchive->pRootNode->pNextNode;
  }

  cCheckMusicExeption = 1;

  for (i = 0; i < 64; i++)
    bmp[i] = -1;

  ddxSetFlip(0);

  iClock = ddxLoadBitmap("clock1-1.bmp", pBmpArchive);
  ddxResizeCursorBack(iClock);
  DrawClock(&iClock, 0);

	if(ogg_playing())
	{
		float f;

		f = p_ad->Music_Gain;

		while(f >= 0.05f)
		{
			f -= 0.05f;
			ogg_gain(f);
			Sleep(25);
		}

		ap_Stop_Song(p_ad);
	}

  ddxSetFlip(0);

  DrawClock(&iClock, 0);

  sprintf(ccomix, "comix%d.txt", iScene);

  if (iLanguageVersion == 4) {
    LoadCList(ccomix, &bmpc, bmp, &iClock, hArchive);
    apakclose(&hArchive);
  }
  else {
    LoadCList(ccomix, &bmpc, bmp, &iClock, pBmpArchive);
  }

  DrawClock(&iClock, 0);
  ddxSetCursor(0);
  DisplayFrame();
  DisplayFrame();
  ddxResizeCursorBack(0);
  ddxSetCursorSurface(0);
  ddxSetCursor(1);
  DisplayFrame();
  DisplayFrame();
  ddxReleaseBitmap(iClock);
  ddxSetCursor(0);

  spracuj_spravy(0);

  if (ogg_playing())
    ap_Stop_Song(p_ad);

  iSongTime = GetComixTime(iScene);

  ap_Play_Song(iScene+1,0, p_ad);

  dwStart = timeGetTime();

  while (!key[K_ESC]) {
    spracuj_spravy(0);
    dwStop = timeGetTime();
    dwEplased = dwStop - dwStart;

    if (dwEplased >= (unsigned) iSongTime)
      break;

    lidx = idx;
    idx = (int) floor((dwEplased * bmpc) / (float) iSongTime);

    if (lidx != idx) {
      lidx = idx;

      if (idx >= 0 && idx < 64)
        if (bmp[idx] != -1) {
          ddxBitBltDisplay(0, 0, 1024, 768, bmp[idx], 0, 0);
          DisplayFrame();
        }
        else
          break;
      else
        break;
    }

    if (key[K_SPACE]) {
      key[K_SPACE] = 0;

      do {
        dwStart -= 1000;
        dwEplased = dwStop - dwStart;
        idx = (int) floor((dwEplased * bmpc) / (float) iSongTime);
      }
      while (idx == lidx);

      dwStart -= ftoi(iSongTime / (float) bmpc);
    }

    if (ddxRestore(p_ad)) {
      cCheckMusicExeption = 0;
      return 1;
    }
  }

	adas_Release_Source(-1, ALL_TYPES, UNDEFINED_VALUE);
	adas_Release_Source(ALL_SOUND_SOURCES, ALL_TYPES,UNDEFINED_VALUE); 

  key[K_ESC] = 0;

  for (i = 0; i < 64; i++)
    if (bmp[i] != -1)
      ddxReleaseBitmap(bmp[i]);

	if(ogg_playing())
	{
		float f;

		f = p_ad->Music_Gain;

		while(f >= 0.05f)
		{
			f -= 0.05f;
			ogg_gain(f);
			Sleep(25);
		}

		ap_Stop_Song(p_ad);
	}

  Sleep(1000);

  ap_Play_Song(0, 0, p_ad);

  ddxCleareSurfaceColorDisplay(0);
  DisplayFrame();
  ddxCleareSurfaceColorDisplay(0);

  ddxSetFlip(1);
  ddxSetCursor(1);

  cCheckMusicExeption = 0;

  return 0;
}

char MenuCheckBossExit(void)
{
  spracuj_spravy(0);

  if ((key[K_SHIFT] || key[K_SHFT_P]) && key[K_F11])
    return 1;
  else
    return 0;
}

char MenuCheckSuccessExit(void)
{
  spracuj_spravy(0);

  if (key[K_CTRL] && key[K_E])
    return 1;
  else
    return 0;
}
