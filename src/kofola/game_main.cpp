//------------------------------------------------------------------------------------------------
// version 0.0.2
//------------------------------------------------------------------------------------------------
#include <time.h>
#include <signal.h>
#include "compat_mini.h"
#include "game_init.h"
#include "game_logic.h"
#include <math.h>
#include "config.h"
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
#include "utils.h"

char pBmpDir[MAX_FILENAME] = { 0 };
char pControlsDir[MAX_FILENAME] = { 0 };
char p3DMDir[MAX_FILENAME] = { 0 };
char pSndDir[MAX_FILENAME] = { 0 };
char pDataDir[MAX_FILENAME] = { 0 };
char pGDataDir[MAX_FILENAME] = { 0 };

char CurrentWorkingDirectory[MAX_FILENAME];
int bWindowMenu;

extern int iWinVer;
extern HINT_TEXTURE pHintTexture[26];
extern PLAYER_PROFILE pPlayerProfile;
extern AUDIO_DATA ad;
extern int iActualScene;
extern int iActualLevel;
extern int bScreenshot;
char bInMenu = 0;
int bScreenshot = 0;

char cFontDir[5][64];

// Save a screenshot.
void do_screenshot(int signal)
{
  // We can't call gl_Make_Screenshot directly, because it isn't
  // async-signal-safe. So set a flag instead, and let flip() do the
  // work.
  bScreenshot = 1;
}

//------------------------------------------------------------------------------------------------
// kostra behu hry
//------------------------------------------------------------------------------------------------
int winmain_Game_Run(char *p_Level_Name)
{
  TIMER_ID Timer_ID;
  int cpu;
  char bGame = strlen(p_Level_Name);
  char filename[MAX_FILENAME];
  struct sigaction act;

  // Save a screenshot on SIGUSR1.
  act.sa_handler = do_screenshot;
  sigemptyset(&(act.sa_mask));
  act.sa_flags = 0;
  // Errors should never occur.
  if (sigaction(SIGUSR1, &act, NULL))
    assert(0);

  ShowCursor(FALSE);

  cpu = sizeof(AUDIO_DATA);

  kprintf(1, "Kofola - verze zdrojaku: MASTER %s", VERSION);

  gi_Set_Win_Version();
  if (getcwd(CurrentWorkingDirectory, MAX_FILENAME) == NULL)
    return 0;

  srand((unsigned) time(NULL));

  get_dir_from_pak("files", "bitmap_subdir", "bitmap_pak",
                   "/", filename, MAX_FILENAME, ini_file);
  GetPrivateProfileString("files", "bitmap_subdir", "/", filename, MAX_FILENAME, ini_file);
  construct_path(pBmpDir, MAX_FILENAME, 2, p_ber->dir.bitmap_dir, filename);

  for (int i = 0; i < 5; i++) {
    char dir[64], pak[64];
    snprintf(dir, sizeof(dir), "font_dir%d", i + 1);
    snprintf(pak, sizeof(pak), "font_pak%d", i + 1);
    get_dir_from_pak("files", dir, pak, "/", cFontDir[i], 64, ini_file);
    kprintf(1, "font_dir%d = %s", i + 1, cFontDir[i]);
  }

  construct_path(pControlsDir, MAX_FILENAME, 2, p_ber->dir.bitmap_dir, "controls");

  construct_path(pSndDir, MAX_FILENAME, 2, p_ber->dir.sound_dir, "sound");

  get_dir_from_pak("files", "3dmenu_dir", "3dmenu_pak",
                   "/", filename, MAX_FILENAME, ini_file);
  construct_path(p3DMDir, MAX_FILENAME, 2, p_ber->dir.bitmap_dir, filename);

  strncpy(pDataDir, p_ber->dir.data_dir, MAX_FILENAME);
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
