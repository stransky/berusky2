/*
  Berusci smazba
*/
#define  WM_MOUSEWHEEL  0x020A

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#include <fenv.h>

#include "config.h"
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
#include "Berusky3d_kamery.h"
#include "Berusky3d_kofola2d.h"

#include "json_export.h"

#ifdef DEBUG_OKNO
dword start_debug;
#endif

#define WINDOW_TITLE "Berusky 2"

char ini_file[MAX_FILENAME] = "";
bool create_user_data = TRUE;
bool export_level = FALSE;
char export_level_file[MAX_FILENAME] = "";

void flip(void)
{
  AGE_MAIN *p_age = p_ber->p_age;
  GRAPH3D *p_grf = p_age->graph_get();

  p_grf->flip();

  static TIMER tm(hwconf.game_fps);
  tm.frame_end(TRUE);
  tm.frame_start();
}

void screen_resize_callback(tpos width, tpos height)
{
  printf("Screen resize %dx%d\n", width, height);

  SCREEN_XRES = width;
  SCREEN_YRES = height;

  // Set scale factor
  scale_factor_set();

  // Projection & View matrix
  kam_set_last(p_ber);
}

inline int filtr_klaves(int scancode)
{
  return (gl_Allow_Key(scancode) ? scancode : 0);
}

int nahraj_konfig(void)
{
  /* TODO - sync with Setup
   */
  nahraj_device_config(ini_file, "game");
  nahraj_universal_device_config(ini_file, "game");
  nahraj_texture_config(ini_file, &txconf);

  return (TRUE);
}

void kom_ret_default_text_config(void)
{
  txconf.text_mip_mapping = GetPrivateProfileInt(TXT_SEKCE, "text_mip_mapping", 1, ini_file);
  txconf.text_mip_filtr = GetPrivateProfileInt(TXT_SEKCE, "text_mip_filtr", 1, ini_file);
  txconf.text_wrap_x = 1;
  txconf.text_wrap_y = 1;
}

void kom_graf_konec(int menu)
{
  kprintf(TRUE, "kom_graf_konec...");

  /* Shodim GL
   */
  konec(FALSE);  
}

void konec(int konec)
{
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

void print_banner(void)
{
  printf("%s v.%s (C) Anakreon 2011, http://www.anakreon.cz/\n", WINDOW_TITLE, VERSION);
  printf("This is free software; see the source for copying conditions.\n");
  printf("There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\n");
}

void print_help(void)
{
  printf("Using: berusky2 [OPTIONS][LEVEL_NAME]\n");
  printf("\nOptions:\n");
  printf("  -h --help                           Print help\n");
  printf("  -i --ini-file file_name.ini         Set game ini file (It overides the default one in ~user/.berusky2/berusky3d.ini)\n");
  printf("  -l --no-local-copy                  Don't create local config at ~user/.berusky2\n");
  printf("  -e --export file_name               Export loaded level to JSON\n");
  printf("\nLevel name:\n");
  printf("  level_name.lv6                      Run this level (from game dir) instead of default menu\n");
  printf("\n");
  exit(0);
}

void process_params(G_KONFIG * p_ber, int argc, char **argv)
{
  int i;

  for (i = 1; i < argc; i++) {
    if (!strcasecmp(argv[i], "-h") || !strcasecmp(argv[i], "--help")) {
      print_help();
    }
    else if (!strcasecmp(argv[i], "-i") || !strcasecmp(argv[i], "--ini-file")) {
      i++;
      if (i < argc) {
        strcpy(ini_file, argv[i]);
      }
    }
    else if (!strcasecmp(argv[i], "-l") || !strcasecmp(argv[i], "--no-local-copy")) {
      create_user_data = FALSE;
    }
    else if (!strcasecmp(argv[i], "-e") || !strcasecmp(argv[i], "--export")) {
      export_level = TRUE;
      i++;
      if (i < argc) {
        strcpy(export_level_file, argv[i]);
      }
    }
    else {                      // it's a level name?
      strcpy(p_ber->level_name, argv[i]);
      pprintf("Custom level set - %s", p_ber->level_name);
    }
  }
}

/*
  Config file initialization

  1) Check used refined ini file (-i param)
  2) try the one from ~./berusky2
  3) check local config dir
*/
char ini_file_dirs[][MAX_FILENAME] = {
  "",                           // ~./berusky2
  "",                           // current working dir  
  INI_FILE_GLOBAL_DIR
};

void ini_file_init(void)
{
  if (ini_file[0]) {
    if (!efile(ini_file)) {
      pperror(1, "Unable to open ini file '%s'!", ini_file);
    }
    else {
      char tmp_ini[MAX_FILENAME];

      if (!realpath(ini_file, tmp_ini)) {
        pperror(1, "Unable to resolve ini file path! (%s)", ini_file);
      }
      strcpy(ini_file, tmp_ini);
      pprintf("Using custom ini file: %s", ini_file);
    }
  }
  else {
    for (unsigned int i = 0;
      i < sizeof(ini_file_dirs) / sizeof(ini_file_dirs[0]); i++) {

      // ~./berusky2
      if (i == 0) {
        return_path(INI_USER_DIRECTORY, "", ini_file_dirs[i], MAX_FILENAME);
      }

      // current working directory init
      if (i == 1) {
        if (getcwd(ini_file_dirs[i], MAX_FILENAME) == NULL)
	  pperror(1, "Cannot get current directory");
      }

      strcat(ini_file_dirs[i], "/" INI_FILE_NAME);

      pprintfnl("Trying to open ini file at %s...", ini_file_dirs[i]);
      if (efile(ini_file_dirs[i])) {
        pprintf("OK");
        strcpy(ini_file, ini_file_dirs[i]);
        return;
      }
      else {
        pprintf("FAILED");
      }
    }

    pperror(1, "Unable to open any ini file!");
  }
}

void debug_file_init(void)
{
  char tmp[200];
  GetPrivateProfileString("install", "log_file", "", tmp, 200, ini_file);
  if(tmp[0]) {
    p_ber->debug_file = fopen(tmp, "a");
  }
}

/* It tries to create the user directory (~./berusky2)
 * and copy berusky3d.ini file there
 *
 */
void user_directory_create(void)
{
  // Check ~./berusky2
  dir_create(INI_USER_DIRECTORY);
  dir_create(INI_SAVES_DIRECTORY);
  dir_create(INI_PROFILE_DIRECTORY);

  // Check ~./berusky2/berusky3d.ini
  pprintfnl("Checking %s/%s...", INI_USER_DIRECTORY, INI_FILE_NAME);
  if (!file_exists(INI_USER_DIRECTORY, INI_FILE_NAME)) {
    pprintfnl("missing, try to copy it from %s...", INI_FILE_GLOBAL);
    bool ret =
      file_copy(INI_FILE_GLOBAL, NULL, INI_FILE_NAME, INI_USER_DIRECTORY,
      FALSE);
    if (ret) {
      pprintf("OK");
    }
    else {
      print_errno(TRUE);
      pprintf("FAILED");
    }
  }
  else {
    pprintf("OK");
  }
}

void game_exit(void)
{
  AGE_MAIN *p_age = p_ber->p_age;
  if(p_age)      
    delete p_age;
}

int main(int argc, char **argv)
{
  //feenableexcept(FE_DIVBYZERO|FE_INEXACT|FE_INVALID|FE_OVERFLOW|FE_UNDERFLOW);
  //feenableexcept(FE_DIVBYZERO|FE_INVALID);  
  //quat_test();
  //alut_test(TRUE);

  atexit(game_exit);

  print_banner();
  process_params(p_ber, argc, argv);

  if (create_user_data) {
    user_directory_create();
  }

  ini_file_init();

  working_dir_init();
  debug_file_init();

  nahraj_konfig();

  AGE_MAIN *p_age = p_ber->p_age = new AGE_MAIN(main_callback);
  
  p_age->graph_set(SCREEN_XRES, SCREEN_YRES, hwconf.bpp, hwconf.fullscreen);
  if (!p_age->graph_screen_create())
    return(FALSE);
  
  GRAPH3D *p_grf = p_age->graph_get();
  p_grf->get(&SCREEN_XRES, &SCREEN_YRES, &hwconf.bpp);
  
  window_set_title(WINDOW_TITLE);
  
  if (!grf_start(ini_file))
    pperror(1, "Init");
  
  ber_nahod_render_funkce();
  ber_konfiguruj_berusky(&ber);

  timeGetTimeInit();
  
  SCENE *p_scene = p_age->scene_new();

  p_age->scene_active_set(p_scene);
  
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
  
  p_grf->resize_callback_set(screen_resize_callback);
  
  glstav_reset();

  if(export_level) {
    json_export_start(export_level_file);
  }

  winmain_Game_Run(p_ber->level_name);  

  if(export_level) {
    json_export_end();
  } 

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
  //kprintf(TRUE, "nacti_tlacitka_mysi(): num = %d, state = %d", num, state);
  switch (num) {
    case 1:
      //kprintf(TRUE, "nacti_tlacitka_mysi(): mi.t1 = %d", state);
      mi.ot1 = mi.dt1 = mi.t1 = state;
      break;
    case 3:
      //kprintf(TRUE, "nacti_tlacitka_mysi(): mi.t2 = %d", state);
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
            //kprintf(TRUE, "nacti_tlacitka_mysi(): key[K_PLUS] = %d", TRUE);
            key[K_PLUS] = TRUE;
          }
          else {
            //kprintf(TRUE, "nacti_tlacitka_mysi(): key[K_MINUS] = %d", TRUE);
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
      case SDL_KEYDOWN:
        {
          int keycode = event.key.keysym.sym;
	  SDLMod mod = event.key.keysym.mod;

          //printf("SDL_KEYDOWN(%d)\n",keycode);
        
          key[keycode] = TRUE;
          key[0] = TRUE;

          key[K_SHIFT] = mod & KMOD_SHIFT;
          key[K_CTRL] = mod & KMOD_CTRL;
          key[K_ALT] = mod & KMOD_ALT;

          // store unicode character for GUI interface
          key_pressed = event.key.keysym.unicode;
        }
        break;
      case SDL_KEYUP:
        {
          int keycode = event.key.keysym.sym;
	  SDLMod mod = event.key.keysym.mod;

          key[keycode] = FALSE;
          key[0] = FALSE;

          key[K_SHIFT] = mod & KMOD_SHIFT;
          key[K_CTRL] = mod & KMOD_CTRL;
          key[K_ALT] = mod & KMOD_ALT;
        }
        break;
      case SDL_MOUSEMOTION:
        {
          nacti_polohu_mysi(event.motion.x, event.motion.y);
          int i;

          for (i = 0; i < MOUSE_BUTTONS; i++) {
            bool state = event.motion.state & SDL_BUTTON(i);
            if (state) {
              nacti_tlacitka_mysi(i, 1);              
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
        if (event.active.state&SDL_APPINPUTFOCUS) {        
          kprintf(TRUE,"SDL_APPINPUTFOCUS, gain = %d",event.active.gain);
        }
        if (event.active.state&SDL_APPMOUSEFOCUS) {        
          kprintf(TRUE,"SDL_APPMOUSEFOCUS, gain = %d",event.active.gain);
          if (event.active.gain) {
            p_ber->system_kurzor = FALSE;
          }
          else {
            p_ber->system_kurzor = TRUE;
          }
        }
        if (event.active.state&SDL_APPACTIVE) {
          kprintf(TRUE,"SDL_APPACTIVE, gain = %d",event.active.gain);
          if (event.active.gain) {
            karmin_aktivni = TRUE;
          }
          else {
            karmin_aktivni = FALSE;
          }
        }
        break;
      case SDL_VIDEORESIZE:
        {
          AGE_MAIN *p_age = p_ber->p_age;
          if(p_age) {
            GRAPH3D *p_grf = p_age->graph_get();
            p_grf->screen_resize(event.resize.w, event.resize.h);
          }
          break;
        }
      case SDL_QUIT:
        gl_Kofola_End(bInMenu);
        break;
      default:
        printf("I don't know what this event is!\n");
    }
  }
  return (TRUE);
}
