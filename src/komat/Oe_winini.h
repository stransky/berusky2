/*
  H
*/
#ifndef __WININI_H
#define __WININI_H

#define INI_FILE   ini_file     /*"berusky3d.ini" */
#define INI_DIR    ""
#define INI_SEKCE  "editor"

extern dword karmin_aktivni;
extern dword system_timer;
extern dword system_kurzor;
extern HWND hwnd_plocha;
extern HWND hwnd_aplikace;
extern HWND hwnd_konzole;
extern HWND hwnd_horni_lista;
extern HWND hwnd_horni_lista_dialog;
extern HWND hwnd_dolni_lista;
extern HWND hwnd_dolni_lista_dialog;
extern HWND hwnd_bocni_lista;
extern HWND hwnd_bocni_lista_dialog;
extern HWND hwnd_konzole;
extern HWND hwnd_konzole_dialog;
extern HINSTANCE hinst;
extern byte akt_editor;
extern byte work_dir[200];
extern byte ini_file[300];
extern int render_povolen;

extern HMENU handle_menu[10];

#define MENU_GAME_EDITOR 0
#define MENU_MESH_EDITOR 1
#define MENU_GAME_POPUP  2
#define MENU_MESH_POPUP  3

#define MESH_EDITOR      1
#define GAME_EDITOR      2


void konec(void);
HWND WInit(HINSTANCE handle_aplikace, int nCmdShow, int okno, byte * p_jmeno,
  byte * p_nadpis, int xres, int yres, void *p_call);

int spracuj_spravy(int priorita);

HWND doe_aktivace_bocni_listy_mesh_editor(HWND bocni_lista);
HWND doe_aktivace_bocni_listy_game_editor(HWND bocni_lista);
void konec_uvolni(void);
void get_work_dir(byte * p_work_dir, int max);

void go_renderovac(char *p_file);

#endif
