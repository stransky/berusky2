#ifndef __MENU2_
#define __MENU2_

#include "Object.h"
#include "animationk.h"

void RunMenuSettings2(char *p_File_Name, AUDIO_DATA * p_ad,
  LEVELINFO * p_Level, ANIMATION_MODULE * p_am);
int RunMenuLoadGameLoad2(char *p_File_Name, AUDIO_DATA * p_ad,
  LEVELINFO * p_Level, ANIMATION_MODULE * p_am, char bLoad, WCHAR * cText);
void RunMenuHelp2(char *p_File_Name, AUDIO_DATA * p_ad, LEVELINFO * p_Level,
  ANIMATION_MODULE * p_am);
void RunMenuLevelStats2(char *p_File_Name, AUDIO_DATA * p_ad,
  LEVELINFO * p_Level, ANIMATION_MODULE * p_am);
void RunMenuTutorial2(char *p_File_Name, AUDIO_DATA * p_ad,
  LEVELINFO * p_Level, ANIMATION_MODULE * p_am);

void RunMenuHelp2(char *p_File_Name, AUDIO_DATA * p_ad, LEVELINFO * p_Level,
  ANIMATION_MODULE * p_am);
void RunMenuLevelStats2(char *p_File_Name, AUDIO_DATA * p_ad,
  LEVELINFO * p_Level, ANIMATION_MODULE * p_am);
void RunMenuTutorial2(char *p_File_Name, AUDIO_DATA * p_ad,
  LEVELINFO * p_Level, ANIMATION_MODULE * p_am);

void RunMenuLoadScreen2(void);
void RunMenuLoadScreenRelease(int iCount);
void RunMenuLoadScreenInitBar(int iNumOfItems);
void RunMenuLoadScreenAddProgress(float fPercent);
void RunMenuLoadScreenDrawProgress(int x, int y);

#endif
