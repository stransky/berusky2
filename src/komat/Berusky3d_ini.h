/*
  H
*/
#ifndef __BERINI_H
#define __BERINI_H

#define INI_FILE (p_ber->ini_file)
extern int mouse_move;

void konec(int konec);
int spracuj_spravy(int param);

//void nastav_okno(HW_KONFIG *p_conf, int menu);
int nahraj_konfig(void);

void minimalizuj_hru(void);
void maximalizuj_hru(void);

//HWND zavri_okno(HWND hwnd);

#define XRES_MENU 1024
#define YRES_MENU 768

#endif
