/*
  H
*/
#ifndef __BERINI_H
#define __BERINI_H

extern int mouse_move;
extern bool export_level;

int  spracuj_spravy(int param);
int  nahraj_konfig(void);
void konec(int konec);

#define XRES_MENU 1024
#define YRES_MENU 768

/* Moved to config.h
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

#endif
*/

#ifdef LINUX
#define INI_FILE_GLOBAL         INI_FILE_GLOBAL_DIR"/"INI_FILE_NAME
#define INI_FILE_USER           INI_USER_DIRECTORY"/"INI_FILE_NAME
#define INI_FILE_LOCAL          "./"INI_FILE_NAME
#endif

#endif
