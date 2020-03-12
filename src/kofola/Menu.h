#ifndef __MENU_
#define __MENU_

void RunMenu(char *p_File_Name, HWND hWnd, AUDIO_DATA * p_ad, int cpu);

int RunLevel(HWND hWnd, AUDIO_DATA * p_ad, int cpu, char *lvl, char *env);

void GetRunMenuNewGameSceneLoadGame(char *cscene, char *cscenemap,
  char *csceneanim, char *cscenelevel, int *iLevelStart, int *iNumOfLevels,
  int *xTV, int *yTV, char *bTutorial, int *xBack, int *yBack);

void RunMenuCinemax(void);

void RunMenuDrawDemoEndScreen(void);

#endif
