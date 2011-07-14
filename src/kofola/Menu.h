#ifndef __MENU_
#define __MENU_

void RunMenu(char *p_File_Name, HWND hWnd, AUDIO_DATA * p_ad, int cpu);

int RunLevel(HWND hWnd, AUDIO_DATA * p_ad, int cpu, char *lvl, char *env);

void RunMenuCinemax(void);

void RunMenuDrawDemoEndScreen(void);

#endif
