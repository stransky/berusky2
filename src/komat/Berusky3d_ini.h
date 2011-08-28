/*
  H
*/
#ifndef __BERINI_H
#define __BERINI_H

extern int mouse_move;

void konec(int konec);
int spracuj_spravy(int param);
int nahraj_konfig(void);

void minimalizuj_hru(void);
void maximalizuj_hru(void);

#define XRES_MENU 1024
#define YRES_MENU 768

#ifndef INI_FILE_NAME
#define INI_FILE_NAME           "berusky3d.ini"
#endif

#ifdef LINUX

#ifndef INI_USER_DIRECTORY
#define INI_USER_DIRECTORY      "~/.berusky2"
#endif

#ifndef INI_SAVES_DIRECTORY
#define INI_SAVES_DIRECTORY     "~/.berusky2/Save"
#endif

#ifndef INI_PROFILE_DIRECTORY
#define INI_PROFILE_DIRECTORY  "~/.berusky2/Save/profiles"
#endif

#ifndef INI_FILE_GLOBAL_DIR
#define INI_FILE_GLOBAL_DIR     "/var/games/berusky2/"
#endif

// /var/games/berusky2/berusky3d.ini
#ifndef INI_FILE_GLOBAL
#define INI_FILE_GLOBAL         INI_FILE_GLOBAL_DIR INI_FILE_NAME
#endif

// ~/.berusky2/berusky3d.ini
#ifndef INI_FILE_USER
#define INI_FILE_USER           INI_USER_DIRECTORY INI_FILE_NAME
#endif

// ./berusky3d.ini
#ifndef INI_FILE_LOCAL
#define INI_FILE_LOCAL          "./"INI_FILE_NAME
#endif

#endif

#endif
