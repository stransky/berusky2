/*
  H
*/
#ifndef __BERINI_H
#define __BERINI_H

extern int mouse_move;

void konec(int konec);
int spracuj_spravy(int param);

//void nastav_okno(HW_KONFIG *p_conf, int menu);
int nahraj_konfig(void);

void minimalizuj_hru(void);
void maximalizuj_hru(void);

#define XRES_MENU 1024
#define YRES_MENU 768

#define INI_FILE_NAME           "berusky3d.ini"

#ifdef LINUX
#define INI_USER_DIRECTORY      "~/.berusky2"
#define INI_SAVES_DIRECTORY     "~/.berusky2/Save"
#define INI_PROFFILE_DIRECTORY  "~/.berusky2/Save/profiles"
#define INI_FILE_GLOBAL_DIR     "/var/games/berusky2/"

#define INI_FILE_GLOBAL         INI_FILE_GLOBAL_DIR INI_FILE_NAME
#define INI_FILE_USER           INI_USER_DIRECTORY INI_FILE_NAME
#define INI_FILE_LOCAL          "./"INI_FILE_NAME
#endif

#endif
