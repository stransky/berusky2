//------------------------------------------------------------------------------------------------
// version 0.0.2
//------------------------------------------------------------------------------------------------
#include <time.h>
#include "compat_mini.h"
#include "game_init.h"
#include "game_logic.h"
#include <math.h>
#include "3d_all.h"
#include "Berusky3d_kofola_interface.h"
#include "2D_graphic.h"
#include "3D_graphic.h"
#include "3D_menus.h"
#include "Menu.h"
#include "Comics.h"
#include "font.h"
#include "profiles.h"
#include "Menu.h"
#include "Menu2.h"

char pBmpDir[MAX_FILENAME] = { 0 };
char pControlsDir[MAX_FILENAME] = { 0 };
char p3DMDir[MAX_FILENAME] = { 0 };
char pSndDir[MAX_FILENAME] = { 0 };
char pDataDir[MAX_FILENAME] = { 0 };
char pGDataDir[MAX_FILENAME] = { 0 };

char CurrentWorkingDirectory[MAX_FILENAME];
int bWindowMenu;
int iLanguageVersion;

extern int iWinVer;
extern HINT_TEXTURE pHintTexture[26];
extern PLAYER_PROFILE pPlayerProfile;
extern AUDIO_DATA ad;
extern int iActualScene;
extern int iActualLevel;
char bInMenu = 0;

char cFontDir[5][64];

//------------------------------------------------------------------------------------------------
// kostra behu hry
//------------------------------------------------------------------------------------------------
int winmain_Game_Run(char *p_Level_Name)
{
  TIMER_ID Timer_ID;
  int cpu;
  char bGame = strlen(p_Level_Name);
  char filename[MAX_FILENAME];

  ShowCursor(FALSE);

  cpu = sizeof(AUDIO_DATA);

  kprintf(1, "Kofola - verze zdrojaku: MASTER %d.%d", VERZEHI, VERZELO);

  gi_Set_Win_Version();
  if (getcwd(CurrentWorkingDirectory, MAX_FILENAME) == NULL)
    return 0;

  srand((unsigned) time(NULL));

  GetPrivateProfileString("files", "bitmap_subdir", "/", filename, MAX_FILENAME, ini_file);
  construct_path(pBmpDir, MAX_FILENAME, 2, p_ber->dir.bitmap_dir, filename);

  GetPrivateProfileString("files", "font_dir1", "/", cFontDir[0], 64, ini_file);
  kprintf(1, "font_dir1 = %s", cFontDir[0]);

  GetPrivateProfileString("files", "font_dir2", "/", cFontDir[1], 64, ini_file);
  kprintf(1, "font_dir2 = %s", cFontDir[1]);

  GetPrivateProfileString("files", "font_dir3", "/", cFontDir[2], 64, ini_file);
  kprintf(1, "font_dir3 = %s", cFontDir[2]);

  GetPrivateProfileString("files", "font_dir4", "/", cFontDir[3], 64, ini_file);
  kprintf(1, "font_dir4 = %s", cFontDir[3]);

  GetPrivateProfileString("files", "font_dir5", "/", cFontDir[4], 64, ini_file);
  kprintf(1, "font_dir5 = %s", cFontDir[4]);

  iLanguageVersion = GetPrivateProfileInt("files", "languageid", 0, ini_file);
  kprintf(1, "Language ID = %d", iLanguageVersion);

  construct_path(pControlsDir, MAX_FILENAME, 2, p_ber->dir.bitmap_dir, "controls");

  construct_path(pSndDir, MAX_FILENAME, 2, p_ber->dir.sound_dir, "sound");

  GetPrivateProfileString("files", "3dmenu_dir", "/", filename, MAX_FILENAME, ini_file);
  construct_path(p3DMDir, MAX_FILENAME, 2, p_ber->dir.bitmap_dir, filename);

  GetPrivateProfileString("files", "data_subdir", "/", filename, MAX_FILENAME, ini_file);
  construct_path(pDataDir, MAX_FILENAME, 2, p_ber->dir.data_dir, filename);

  strncpy(pGDataDir, p_ber->dir.game_data_dir, MAX_FILENAME);

	gi_Init_Sound_Engine(&ad);
	if (chdir(p_ber->dir.music_dir))
	  return 0;
	ap_Load_Play_List("play_list.dat",&ad);
  if (chdir(p_ber->dir.sound_dir))
    return 0;

  ap_Load_Material_List("material.dat", &ad);

  if (!bGame) {
    InitDirectDraw();
    spracuj_spravy(0);
  }

  if (bGame) {
    int ret = sscanf(p_Level_Name, "level%d.lv6", &iActualLevel);
    assert(ret == 1);
    // Search for the scene this level is in.
    for (iActualScene = 0; iActualScene < 13; iActualScene++) {
      char cTmp[64];
      int iTmp;
      int iLevelStart, iNumOfLevels;

      GetRunMenuNewGameSceneLoadGame(cTmp, cTmp, cTmp, cTmp,
                                     &iLevelStart, &iNumOfLevels,
                                     &iTmp, &iTmp, cTmp, &iTmp, &iTmp);
      if (iActualLevel >= iLevelStart &&
          iActualLevel < iLevelStart + iNumOfLevels)
        break;
    }
    assert(iActualScene < 13);

    RunMenuLoadScreen2();
    RunMenuLoadScreenInitBar(15);
    RunMenuLoadScreenAddProgress(-1);
    RunMenuLoadScreenDrawProgress(-1, -1);
    _3d_Init();
    _3d_Load_List("3d_load.dat");

    _3d_Gen_Hints(pHintTexture, 26);

    if (!fn_Set_Font(cFontDir[1])) {
      kprintf(1, "Unable to set font!");
      return 0;
    }

    if (!fn_Load_Bitmaps())
      kprintf(1, "Unable to load font bitmaps");

    RunMenuLoadScreenAddProgress(-1);
    RunMenuLoadScreenDrawProgress(-1, -1);
  }

  if (!bGame) {
    //schovej_konzoli();
    SetCursor(NULL);
    ddxInit();

    RunMenuCinemax();

    ap_Play_Song(0, 0, &ad);

    ddxLoadList("2d_load.dat", 1);

    if (!fn_Set_Font(cFontDir[0])) {
      return 0;
    }

    if (!fn_Load_Bitmaps())
      kprintf(1, "Unable to load font bitmaps");
  }  

  if (bGame) {
    pr_ReadProfile("Default", &pPlayerProfile);

    SetCursor(NULL);
    Timer_ID = SetTimer(NULL, 0, 250, (TIMERPROC) gl_Set_Frame_Rate);
    _3d_Load_Indikace();
    char cenv[64] = "default.env";
    gl_Run_Level(p_Level_Name, cenv, &ad, cpu);
    _3d_Release_Hints(pHintTexture, 26);
    KillTimer(NULL, Timer_ID);
  }
  else {    
    SetCursor(NULL);
    ShowCursor(FALSE);
    RunMenu("mainmenu.txt", NULL, &ad, cpu);
  }

  if (ogg_playing())
    ap_Stop_Song(&ad);

  if (!bGame) {
  /*_2d_Blackness();
		_2d_Release();*/
    ddxRelease();
    FreeDirectDraw();
  }
  else {
    //fn_Release_Font();
    _3d_Release();
  }

  //kprintf(1, "cmcs_Play_Intro");
  //cmcs_Play_Intro("gamelogo.txt", hWnd, &ad);

  //kprintf(1, "release FONT");
  //fn_Release_Font();

  kprintf(1, "gi_Release_Sound_Engine");
  gi_Release_Sound_Engine();
  kprintf(1, "ap_Release_Play_List");
  ap_Release_Play_List(&ad);
  kprintf(1, "ap_Release_Material_List");
  ap_Release_Material_List(&ad);
  kprintf(1, "ap_Release");
  ap_Release(&ad);

  //ChangeDisplaySettings(NULL,0);
  //ShowWindow(hwnd_hry, SW_MAXIMIZE);
  spracuj_spravy(0);
  ShowCursor(TRUE);
  spracuj_spravy(0);

  //UnloadTransparentBlt();

  //MSS_LOG_BLOCK_LIST;

  //DestroyWindow(hwnd_hry);
  exit(0);

  return 0;
}
