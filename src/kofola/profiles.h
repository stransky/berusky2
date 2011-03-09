//------------------------------------------------------------------------------------------------
// 1.0.0
//------------------------------------------------------------------------------------------------

#ifndef __PROFILES_
#define __PROFILES_

typedef struct
{
	int		iVer;
	char	cScene[20];
	char	cMovie[20];
	char	cLevel[600];
	WCHAR	cName[32];
	char	Reserved[1000];
} PLAYER_PROFILE;

	int pr_CreateProfile(WCHAR *cPlayerName);
	int pr_ReadProfile(char *cPlayerName, PLAYER_PROFILE *pProfile);
	int pr_SaveProfile(PLAYER_PROFILE *pProfile);
	int pr_GetPlayerName(char *cFile, WCHAR *cName);

#endif