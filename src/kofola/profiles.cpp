#include <stdio.h>

#include "3d_all.h"
#include "Berusky3d_kofola_interface.h"

#ifdef LINUX
#include <dirent.h>
#include <fnmatch.h>
#endif

#include "profiles.h"

PLAYER_PROFILE pPlayerProfile;

void pr_DiscToProfile(PLAYER_PROFILE_DISC * disc, PLAYER_PROFILE * prof)
{
  prof->iVer = disc->iVer;
  memcpy(prof->cScene, disc->cScene, sizeof(disc->cScene));
  memcpy(prof->cMovie, disc->cMovie, sizeof(disc->cMovie));
  memcpy(prof->cLevel, disc->cLevel, sizeof(disc->cLevel));
  memcpy(prof->Reserved, disc->Reserved, sizeof(disc->Reserved));
  wchar_windows_to_linux(disc->cName, PLAYER_NAME_LENGTH, prof->cName);  
}

void pr_ProfileToDisc(PLAYER_PROFILE * prof, PLAYER_PROFILE_DISC * disc)
{
  disc->iVer = prof->iVer;
  memcpy(disc->cScene, prof->cScene, sizeof(disc->cScene));
  memcpy(disc->cMovie, prof->cMovie, sizeof(disc->cMovie));
  memcpy(disc->cLevel, prof->cLevel, sizeof(disc->cLevel));
  memcpy(disc->Reserved, prof->Reserved, sizeof(disc->Reserved));
  wchar_linux_to_windows(prof->cName, PLAYER_NAME_LENGTH, disc->cName);
}

int pr_GetPlayerName(char *cFile, WCHAR * cName)
{
  FILE *file;
  PLAYER_PROFILE_DISC Profile;

  file = fopen(cFile, "rb");

  if (!file)
    return 0;

  if (!fread(&Profile, sizeof(PLAYER_PROFILE_DISC), 1, file)) {
    fclose(file);
    return 0;
  }

  if (Profile.iVer != PROFILE_VER) {
    fclose(file);
    return 0;
  }

  fclose(file);
  
  wchar_windows_to_linux(Profile.cName, PLAYER_NAME_LENGTH, cName);

  return 1;
}

int pr_FindFreeFileName(char *cFile)
{
  int i = 0;
  char text[256];
  FILE *f = NULL;

  do {
    if (snprintf(text, sizeof(text), "%04d.prf", i) >= (int) sizeof(text))
      break;

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

// Returns 0 on success, a positive value if the name is a duplicate,
// or a negative value if some other error occured.
int pr_CreateProfile(WCHAR * cPlayerName)
{
  char cFile[MAX_FILENAME];
  FILE *file;
  PLAYER_PROFILE_DISC Profile;
  char dir[MAX_FILENAME];
  struct dirent **profiles;
  int n_profiles;

  ZeroMemory(cFile, sizeof(cFile));
  ZeroMemory(&Profile, sizeof(PLAYER_PROFILE_DISC));

  strcpy(dir, PROFILE_DIR);
  if (chdir(dir)) {
    kprintf(1, "Cannot change directory to %s", dir);
    return -1;
  }

  // Check if this name already exists.
  file_filter_mask("*.prf");
  n_profiles = scandir(".", &profiles, &file_filter, alphasort);
  if (n_profiles >= 0) {
    int duplicate = 0;
    for (int i = 0; i < n_profiles; i++) {
      WCHAR check_name[PLAYER_NAME_LENGTH] = { 0 };

      if (!duplicate &&
          pr_GetPlayerName(profiles[i]->d_name, check_name) &&
          !wcscmp(cPlayerName, check_name))
        duplicate = 1;

      free(profiles[i]);
    }
    free(profiles);
    if (duplicate)
      return 1;
  }

  pr_FindFreeFileName(cFile);

  if (getcwd(dir, MAX_FILENAME) == NULL) {
    kprintf(1, "Cannot get current directory");
    return -1;
  }
  kprintf(1, "pr_CreateProfile adr = %s", dir);

  file = fopen(cFile, "wb");

  if (!file) {
    kprintf(1, "Nejde vytvorit profil %s", cFile);
    return -1;
  }

  Profile.iVer = PROFILE_VER;
  wchar_linux_to_windows(cPlayerName, PLAYER_NAME_LENGTH, Profile.cName);

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

  if (!fwrite(&Profile, sizeof(PLAYER_PROFILE_DISC), 1, file)) {
    kprintf(1, "!fwrite");
    fclose(file);
    return -1;
  }

  fclose(file);

  memcpy(&pPlayerProfile, &Profile, sizeof(PLAYER_PROFILE_DISC));

  WritePrivateProfileString("game", "last_profile", cFile, ini_file);

  return 0;
}

int pr_ReadProfile(char *cFileName, PLAYER_PROFILE * pProfile)
{
  PLAYER_PROFILE_DISC disc;
  char dir[MAX_FILENAME];
  FILE *file;

  ZeroMemory(&disc, sizeof(PLAYER_PROFILE_DISC));

  if (chdir(PROFILE_DIR))
    return 0;
  strcpy(dir, cFileName);

  {
    char ddir[MAX_FILENAME];

    if (getcwd(ddir, MAX_FILENAME) == NULL)
      return 0;
    kprintf(1, "pr_ReadProfile adr = %s", ddir);
  }

  file = fopen(dir, "rb");

  if (!file)
    return 0;

  if (!fread(&disc, sizeof(PLAYER_PROFILE_DISC), 1, file)) {
    fclose(file);
    return 0;
  }

  pr_DiscToProfile(&disc, pProfile);

  if (pProfile->iVer != PROFILE_VER) {
    fclose(file);
    return 0;
  }

  fclose(file);

  WritePrivateProfileString("game", "last_profile", cFileName, ini_file);

  return 1;
}

#ifdef LINUX
int pr_FindFileToProfile(WCHAR * wName, char *cFile)
{
  struct dirent **namelist;
  int ret = FALSE;
  int i;
	
  file_filter_mask("*.prf");
  int c = scandir(".", &namelist, &file_filter, alphasort);
  if (c < 0) {
    return 0;
  }

  for(i = 0; i < c; i++) {
    FILE *file = fopen(namelist[i]->d_name, "rb");

    if(file)
    {
      PLAYER_PROFILE_DISC	tmp;
      if (fread(&tmp, sizeof(tmp), 1, file) != 1) {
	fclose(file);
	return 0;
      }
      fclose(file);

      PLAYER_PROFILE Profile;
      pr_DiscToProfile(&tmp, &Profile);
    
      if(!wcscmp(Profile.cName, wName))
      {
        strcpy(cFile, namelist[i]->d_name);
        ret = TRUE;
        break;
      }
    }
  } 

  for(i = 0; i < c; i++) {
      free(namelist[i]);
  } 
  free(namelist);

  return ret;
}
#endif

#ifdef WINDOWS
int pr_FindFileToProfile(WCHAR * wName, char *cFile)
{
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

  return 0;
}
#endif

int pr_SaveProfile(PLAYER_PROFILE * pProfile)
{
  char cFile[MAX_FILENAME];
  FILE *file;
  char dir[MAX_FILENAME];

  ZeroMemory(cFile, sizeof(cFile));

  strcpy(dir, PROFILE_DIR);
  if (chdir(dir))
    return 0;

  if (!pr_FindFileToProfile(pProfile->cName, cFile))
    return 0;

  if (getcwd(dir, MAX_FILENAME) == NULL)
    return 0;
  kprintf(1, "pr_SaveProfile adr = %s", dir);

  file = fopen(cFile, "wb");

  if (!file)
    return 0;

  PLAYER_PROFILE_DISC tmp;
  pr_ProfileToDisc(pProfile, &tmp);
  if (!fwrite(&tmp, sizeof(PLAYER_PROFILE_DISC), 1, file)) {
    fclose(file);
    return 0;
  }

  fclose(file);

  return 1;
}
