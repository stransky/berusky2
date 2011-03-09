/*
  H
*/
#ifndef __BERINI_H
#define __BERINI_H

extern dword karmin_aktivni;
extern dword system_timer;
extern dword system_kurzor;
extern dword obsluha_okna;
extern dword play_demo;
extern HWND  hwnd_hry;
extern HINSTANCE hinst;
extern byte  ini_file[300];
extern int   mouse_move;

void konec(int konec);
void kprintf(byte log, byte *p_text,...);
int  spracuj_spravy(int param);
void nastav_okno(HW_KONFIG *p_conf, int menu);
int  nahraj_konfig(void);

void minimalizuj_hru(void);
void maximalizuj_hru(void);

void clip_set(HW_KONFIG *p_conf);
void clip_ret(void);


HWND zavri_okno(HWND hwnd);

#define XRES_MENU 1024
#define YRES_MENU 768

#endif