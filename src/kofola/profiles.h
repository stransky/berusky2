//------------------------------------------------------------------------------------------------
// 1.0.0
//------------------------------------------------------------------------------------------------

#ifndef __PROFILES_
#define __PROFILES_

#define PLAYER_NAME_LENGTH 32

typedef struct
{
  int  iVer;
  char cScene[20];
  char cMovie[20];
  char cLevel[600];
  WCHAR cName[PLAYER_NAME_LENGTH];
  char Reserved[1000];
} PLAYER_PROFILE;

typedef struct
{
  int iVer;
  char cScene[20];
  char cMovie[20];
  char cLevel[600];
  word cName[PLAYER_NAME_LENGTH];
  char Reserved[1000];
} PLAYER_PROFILE_DISC;

int pr_CreateProfile(WCHAR * cPlayerName);
int pr_ReadProfile(char *cPlayerName, PLAYER_PROFILE * pProfile);
int pr_SaveProfile(PLAYER_PROFILE * pProfile);
int pr_GetPlayerName(char *cFile, WCHAR * cName);

int pr_DiscToProfile(PLAYER_PROFILE_DISC * disc, PLAYER_PROFILE * prof);
int pr_ProfileToDisc(PLAYER_PROFILE * prof, PLAYER_PROFILE_DISC * disc);

#endif
