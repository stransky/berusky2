#include <stdio.h>

#include "3d_all.h"
#include "Berusky3d_kofola_interface.h"

#include "profiles.h"

PLAYER_PROFILE pPlayerProfile;

int pr_GetPlayerName(char *cFile, WCHAR * cName)
{
  FILE *file;
  PLAYER_PROFILE Profile;

  file = fopen(cFile, "rb");

  if (!file)
    return 0;

  if (!fread(&Profile, sizeof(PLAYER_PROFILE), 1, file)) {
    fclose(file);
    return 0;
  }

  if (Profile.iVer != PROFILE_VER) {
    fclose(file);
    return 0;
  }

  fclose(file);

  wcscpy(cName, Profile.cName);

  return 1;
}

int pr_FindFreeFileName(char *cFile)
{
  int i = 0;
  char text[256], t[32];
  char z[] = "0000";
  FILE *f = NULL;

  do {
    strcpy(text, "");

    itoa(i, t, 10);

    strncat(text, z, 4 - strlen(t));
    strcat(text, t);
    strcat(text, ".prf");

    f = fopen(text, "r");
    if (!f)
      break;

    i++;
    fclose(f);
  }
  while (f);

  strcpy(cFile, text);

  return i;
}

int pr_CreateProfile(WCHAR * cPlayerName)
{
  //int                           i;
  char cFile[256];
  FILE *file;
  PLAYER_PROFILE Profile;
  char dir[256];

  ZeroMemory(cFile, 256);
  ZeroMemory(&Profile, sizeof(PLAYER_PROFILE));

  GetPrivateProfileString("game", "profile_dir", "c:\\", dir, 256, ini_file);
  working_file_translate(dir, 256);
  chdir((dir));

  pr_FindFreeFileName(cFile);

  getcwd(dir, 256);
  kprintf(1, "pr_CreateProfile adr = %s", dir);

  file = fopen(cFile, "wb");

  if (!file) {
    kprintf(1, "Nejde vytvorit profil %s", cFile);
    return 0;
  }

  Profile.iVer = PROFILE_VER;
  wcscpy(Profile.cName, cPlayerName);

  //tyto sceny povazovat za zapocate
  Profile.cScene[0] = 1;        //Tutorial
  Profile.cScene[1] = 1;        //Pokoj

  // ALFA !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  /*Profile.cScene[2] = 1;
     Profile.cScene[3] = 1;
     Profile.cScene[4] = 1;
     Profile.cScene[5] = 1;
     Profile.cScene[6] = 1;
     Profile.cScene[7] = 1;
     Profile.cScene[8] = 1;
     Profile.cScene[9] = 1; */
  // ALFA !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  //Detska scena
  Profile.cScene[10] = 1;
  Profile.cMovie[10] = 1;

  Profile.cScene[11] = 1;
  Profile.cMovie[11] = 1;

  Profile.cScene[12] = 1;
  Profile.cMovie[12] = 1;

  /*for(i=0;i<33;i++)
     Profile.cLevel[i] = 1; */

  if (!fwrite(&Profile, sizeof(PLAYER_PROFILE), 1, file)) {
    kprintf(1, "!fwrite");
    fclose(file);
    return 0;
  }

  fclose(file);

  memcpy(&pPlayerProfile, &Profile, sizeof(PLAYER_PROFILE));

  WritePrivateProfileString("hra", "last_profile", cFile, ini_file);

  return 1;
}

int pr_ReadProfile(char *cFileName, PLAYER_PROFILE * pProfile)
{
  FILE *file;
  char dir[256];

  ZeroMemory(pProfile, sizeof(PLAYER_PROFILE));

  GetPrivateProfileString("game", "profile_dir", "c:\\", dir, 256, ini_file);
  working_file_translate(dir, 256);
  chdir((dir));

  strcpy(dir, cFileName);

  {
    char ddir[256];

    getcwd(ddir, 256);
    kprintf(1, "pr_ReadProfile adr = %s", ddir);
  }

  file = fopen(dir, "rb");

  if (!file)
    return 0;

  if (!fread(pProfile, sizeof(PLAYER_PROFILE), 1, file)) {
    fclose(file);
    return 0;
  }

  if (pProfile->iVer != PROFILE_VER) {
    fclose(file);
    return 0;
  }

  fclose(file);

  WritePrivateProfileString("hra", "last_profile", cFileName, ini_file);

  /*{
     int i;

     for(i=0;i<13;i++)
     pProfile->cScene[i] = 1;

     for(i=0;i<9;i++)
     pProfile->cMovie[i] = 1;

     for(i=0;i<85;i++)
     pProfile->cLevel[i] = 1;

     for(i=200;i<220;i++)
     pProfile->cLevel[i] = 1;

     } */

  return 1;
}

int pr_FindFileToProfile(WCHAR * wName, char *cFile)
{
/*
	PLAYER_PROFILE	Profile;
	FILE *file;
	long Done, error;
	struct _finddata_t	Data;

	Done = _findfirst("*.prf",&Data);
	error = Done;
			
	while(error != -1)
	{
		if(error != -1)
		{
			file = fopen(Data.name, "rb");

			if(file)
			{
				fread(&Profile, sizeof(PLAYER_PROFILE), 1, file);
				fclose(file);

				if(!wcscmp(Profile.cName, wName))
				{
					strcpy(cFile, Data.name);
					_findclose(Done); 
					return 1;
				}
			}

			error = _findnext(Done,&Data);
		}
	}
	_findclose(Done); 
*/
  return 0;
}

int pr_SaveProfile(PLAYER_PROFILE * pProfile)
{
  char cFile[256];
  FILE *file;
  char dir[256];

  ZeroMemory(cFile, 256 * sizeof(char));

  GetPrivateProfileString("game", "profile_dir", "c:\\", dir, 256, ini_file);
  working_file_translate(dir, 256);
  chdir((dir));

  if (!pr_FindFileToProfile(pProfile->cName, cFile))
    return 0;

  getcwd(dir, 256);
  kprintf(1, "pr_SaveProfile adr = %s", dir);

  file = fopen(cFile, "wb");

  if (!file)
    return 0;

  if (!fwrite(pProfile, sizeof(PLAYER_PROFILE), 1, file)) {
    fclose(file);
    return 0;
  }

  fclose(file);

  return 1;
}
