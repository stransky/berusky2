/*
  Berusci smazba
*/
#define  WM_MOUSEWHEEL  0x020A

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#define _GNU_SOURCE
#include <fenv.h>

#include "3d_all.h"

#include "Object.h"
#include "game_main.h"

#include "Berusky_universal.h"
#include "Berusky3d_castice.h"
#include "Berusky3d.h"
#include "Berusky3d_ini.h"
#include "Berusky3d_load.h"
#include "Berusky3d_render.h"
#include "Berusky3d_animace.h"
#include "Berusky3d_kofola_interface.h"

dword system_timer = 0;
dword system_kurzor = 0;
dword karmin_aktivni = 1;
dword obsluha_okna = 0;
dword play_demo = 0;
HWND hwnd_hry;
HINSTANCE hinst;
char ini_file[300];
int mouse_move = FALSE;

#ifdef DEBUG_OKNO
dword start_debug;
#endif

char work_dir[200] = ".";

void flip(void)
{
  AGE_MAIN *p_age = p_ber->p_age;
  GRAPH3D *p_grf = p_age->graph_get();

  p_grf->flip();
}

inline int filtr_klaves(int scancode)
{
  return (gl_Allow_Key(scancode) ? scancode : 0);
}

void minimalizuj_hru(void)
{
/*
  if(hwnd_hry) {
    grf_prehod_mod_zpet(&hwconf);
    ShowWindow(hwnd_hry,SW_MINIMIZE);
    UpdateWindow(hwnd_hry);
    gl_Kofola_Minimalize();
    clip_ret();
  }
*/
}

void maximalizuj_hru(void)
{
/*
  if(hwnd_hry) {
    grf_prehod_mod_hra(&hwconf);
    ShowWindow(hwnd_hry,SW_RESTORE);
    UpdateWindow(hwnd_hry);
    nastav_okno(&hwconf,FALSE);
    SetFocus(hwnd_hry);
    gl_Kofola_Maximalize();
  }
*/
}

int nahraj_konfig(void)
{
  /* Nahraje konfiguraci grafiky
   */
  nahraj_device_config(ini_file, "hra", &hwconf);
  nahraj_universal_device_config(ini_file, "hra", &hwconf);
  nahraj_texture_config(ini_file, &txconf);

  return (TRUE);
}

int kom_graf_init(void)
{
  int full;

  nahraj_konfig();

  full = hwconf.fullscreen;

  /* Nahodi graficky rezim
   */
  if (full) {
//    nastav_okno(&hwconf,FALSE);
    if (!grf_prehod_mod_hra(&hwconf))
      return (FALSE);
//    hwnd_hry = zavri_okno(hwnd_hry);
//    hwnd_hry = otevri_okno(hinst,hwconf.fullscreen,0,0,&hwconf);
  }

  /* Nahodi grafiku
   */
  if (!grf_start(ini_file))
    chyba("Inicializace");

  /* Nahozeni renderovacich funkci
   */
  ber_nahod_render_funkce();

  /* Vyflushni veci z fronty
   */
  spracuj_spravy(TRUE);

  /* Nahodim svoji obsluhu okna
   */
  obsluha_okna = TRUE;

  return (TRUE);
}

void kom_ret_default_text_config(void)
{
  txconf.text_mip_mapping =
    GetPrivateProfileInt(TXT_SEKCE, "text_mip_mapping", 1, ini_file);
  txconf.text_mip_filtr =
    GetPrivateProfileInt(TXT_SEKCE, "text_mip_filtr", 1, ini_file);
  txconf.text_wrap_x = 1;
  txconf.text_wrap_y = 1;
}

void kom_graf_konec(int menu)
{
  kprintf(TRUE, "kom_graf_konec...");

  /* Shodim svoji obsluhu okna
   */
  obsluha_okna = FALSE;

  /* Shodim GL
   */
  konec(FALSE);

  if (menu) {                   // - zpet do menu - full screen
    grf_prehod_mod_menu();
//    nastav_okno(&hwconf,TRUE);
    spracuj_spravy(0);
  }
}

void konec(int konec)
{
  /* Stop grafiky
   */
  grf_stop();

  /* Zapne kurzor
   */
  zapni_kurzor();

  if (konec) {
    exit(0);
  }
}

bool main_callback(AGE_MAIN * p_age)
{
  // Draw frame
  p_age->graph_draw();
  return (TRUE);
}

void quat_test(void)
{
  QUAT q;
  VECT o(1, 0, 0);
  float r, fi = DEG2RAD(30);

  angle_to_quat(&q, &o, fi);
  quat_to_angle(&q, &o, &fi);

  r = DEG2RAD(0);
  fi = DEG2RAD(30);
  kam_angle_to_quat(r, fi, &q);
  quat_to_euler(&q, &r, &fi);
  assert(r == 0 && RAD2DEG(fi) == 30);
}

int main(int argc, char **argv)
{
  char *p_level;
  char pom[200];

  //feenableexcept(FE_DIVBYZERO|FE_INEXACT|FE_INVALID|FE_OVERFLOW|FE_UNDERFLOW);
  feenableexcept(FE_DIVBYZERO | FE_INVALID);

  //quat_test();

  working_dir_init();

  getcwd(ini_file, 500);
  strcat(ini_file, "//berusky3d.ini");
  if (!efile(ini_file)) {
    strcpy(ini_file, "berusky3d.ini");
  }

  nahraj_konfig();

  if (GetPrivateProfileInt("debug", "debug_file", 0, ini_file)) {
    GetPrivateProfileString("hra", "log_file", "c:\\berusky2log.txt", pom,
      500, ini_file);
    p_ber->debug_file = fopen(pom, "a");
  }

  ber_konfiguruj_berusky(&ber);

  AGE_MAIN *p_age = p_ber->p_age = new AGE_MAIN(main_callback);

  p_age->graph_set(XRES_MENU, YRES_MENU);
  if (!p_age->graph_screen_create())
    return (FALSE);

  timeGetTimeInit();

  SCENE *p_scene = p_age->scene_new();

  p_age->scene_active_set(p_scene);

  GRAPH3D *p_grf = p_age->graph_get();

  if (p_grf) {
    p_grf->config_draw_grid(FALSE);
    p_grf->config_draw_mouse_cursor(FALSE);
    p_grf->config_draw_boxes(FALSE);
    p_grf->config_draw_console(FALSE);
    p_grf->config_draw_fps(FALSE);
    p_grf->config_draw_normals(FALSE);
    p_grf->config_opengl_lighting(FALSE);
    p_grf->config_draw_all_objects(TRUE);
    p_grf->config_draw_pivots(TRUE);
    p_grf->config_draw_selection(TRUE);
  }
  
  glstav_reset();

  p_level = (argc > 1) ? argv[1] : "";
  winmain_Game_Run(p_level);

  return (TRUE);
}

inline void nacti_polohu_mysi(int nx, int ny)
{
  mi.dx = nx - mi.x;
  mi.dy = ny - mi.y;
  mi.x = nx;
  mi.y = ny;
}

inline void nacti_tlacitka_mysi(int num, bool state)
{
  kprintf(TRUE, "nacti_tlacitka_mysi(): num = %d, state = %d", num, state);
  switch (num) {
    case 1:
      kprintf(TRUE, "nacti_tlacitka_mysi(): mi.t1 = %d", state);
      mi.ot1 = mi.dt1 = mi.t1 = state;
      break;
    case 3:
      kprintf(TRUE, "nacti_tlacitka_mysi(): mi.t2 = %d", state);
      mi.ot2 = mi.dt2 = mi.t2 = state;
      break;
    case 4:
    case 5:
      {
        if (state) {
          if (!PRCameraFlag)
            break;

          PRCameraFlagChange = FALSE;

          if (num == 4) {
            kprintf(TRUE, "nacti_tlacitka_mysi(): key[K_PLUS] = %d", TRUE);
            key[K_PLUS] = TRUE;
          }
          else {
            kprintf(TRUE, "nacti_tlacitka_mysi(): key[K_MINUS] = %d", TRUE);
            key[K_MINUS] = TRUE;
          }
        }
      }
      break;
    default:
      break;
  }
}

/*
  case WM_CHAR:
    key_pressed = (TCHAR) wParam;
    break;
  case WM_SETCURSOR:
    if(!mouse_move)
      mouse_move = FALSE;
    break;    
    
  }  
*/
int spracuj_spravy(int param)
{
  SDL_Event event;              /* Event structure */

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case  SDL_KEYDOWN:
        {
          int keycode = event.key.keysym.sym;

          key[keycode] = TRUE;
          key[0] = TRUE;

          SDLMod mod = SDL_GetModState();

          key[K_SHIFT] = mod & KMOD_SHIFT;
          key[K_CTRL] = mod & KMOD_CTRL;
          key[K_ALT] = mod & KMOD_ALT;
       
          // store ascii character for GUI interface
          // ASCI only, we may change it to unicode someday
          key_pressed = keycode;
        }
        break;
      case SDL_KEYUP:
        {
          int keycode = event.key.keysym.sym;

          key[keycode] = FALSE;
          key[0] = FALSE;

          SDLMod mod = SDL_GetModState();

          key[K_SHIFT] = mod & KMOD_SHIFT;
          key[K_CTRL] = mod & KMOD_CTRL;
          key[K_ALT] = mod & KMOD_ALT;
        }
        break;
      case SDL_MOUSEMOTION:
        {
          nacti_polohu_mysi(event.motion.x, event.motion.y);
          bool pressed = FALSE;
          int i;

          for (i = 0; i < MOUSE_BUTTONS; i++) {
            bool state = event.motion.state & SDL_BUTTON(i);

            if (state) {
              nacti_tlacitka_mysi(i, 1);
              pressed = TRUE;
            }
          }
        }
        break;
      case SDL_MOUSEBUTTONDOWN:
        nacti_polohu_mysi(event.motion.x, event.motion.y);
        nacti_tlacitka_mysi(event.button.button, 1);
        break;
      case SDL_MOUSEBUTTONUP:
        nacti_polohu_mysi(event.motion.x, event.motion.y);
        nacti_tlacitka_mysi(event.button.button, 0);
        break;
      case SDL_ACTIVEEVENT:
        if (event.active.state & SDL_APPACTIVE) {
          if (event.active.gain) {
            karmin_aktivni = TRUE;
            system_kurzor = FALSE;
          }
          else {
            karmin_aktivni = FALSE;
            system_kurzor = TRUE;
          }
        }
        break;
      case SDL_QUIT:
        gl_Kofola_End(bDXAktivni);
        break;
      default:
        printf("I don't know what this event is!\n");
    }
  }
  return (TRUE);
}
