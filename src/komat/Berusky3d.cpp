/*
  Berusky

  - udelat konzoli (alt+tab) - prepinani na konzoli
  (konzole misto hry a zpet)
*/
#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include "3d_all.h"

#include "Object.h"

#include "Berusky_universal.h"
#include "Berusky3d_castice.h"
#include "Berusky3d.h"
#include "Berusky3d_ini.h"
#include "Berusky3d_load.h"
#include "Berusky3d_render.h"
#include "Berusky3d_animace.h"
#include "Berusky3d_kofola_interface.h"
#include "Berusky3d_light.h"
#include "Berusky3d_kamery.h"
#include "Berusky3d_kofola2d.h"

#define  KAM_MIN_VZDAL   1.0f

G_KONFIG ber, *p_ber = &ber;

void ber_ini_mod(G_KONFIG * p_ber)
{
  projection_matrix(&p_ber->kamera.project, p_ber->kam.fov,
    (float) OXRES / (float) OYRES, p_ber->kam.near_plane,
    p_ber->kam.far_plane);
  init_matrix(&p_ber->kamera.word);
  init_matrix(&p_ber->kamera.camera);

  p_ber->kamera.vzdal = MIN_VZDAL_KAMERY;
  p_ber->kamera.fi = (float) MIN_UHEL_KAMERY;
  p_ber->kamera.r = 0.0f;
  p_ber->kamera.roll = 0.0f;
  p_ber->kamera.fov = p_ber->kam.fov;
  p_ber->kamera.p.x = p_ber->x_start;
  p_ber->kamera.p.y = p_ber->y_start;
  p_ber->kamera.p.z = p_ber->z_start;

  calc_camera_bod(&p_ber->kamera.camera, &p_ber->kamera.invcam,
    &p_ber->kamera.p, p_ber->kamera.vzdal, p_ber->kamera.fi, p_ber->kamera.r);
}

void ber_nahod_mod(G_KONFIG * p_ber)
{
  p_ber->p_word = &p_ber->kamera.word;
  p_ber->p_project = &p_ber->kamera.project;
  p_ber->p_camera = &p_ber->kamera.camera;
  p_ber->p_invcam = &p_ber->kamera.invcam;

  p_ber->p_vzdal = &p_ber->kamera.vzdal;
  p_ber->p_vzdal_kor = &p_ber->kamera.vzdal_kor;
  p_ber->p_fi = &p_ber->kamera.fi;
  p_ber->p_rfi = &p_ber->kamera.r;
  p_ber->p_target = &p_ber->kamera.t;
  p_ber->p_pos = &p_ber->kamera.p;
  p_ber->p_roll = &p_ber->kamera.roll;
  p_ber->p_mod = &p_ber->kamera.aktivni;
  p_ber->kamera.aktivni |= GAME_KAMERA_POLAR;
  p_ber->kamera.zmena = TRUE;

  kprintf(TRUE, "Screen %dx%d", OXRES, OYRES);

  set_matrix_world(p_ber->p_word);
  set_matrix_camera(p_ber->p_camera);
  set_matrix_project(p_ber->p_project);
  set_matrix_view(OXSTART, OYSTART, OXRES, OYRES);
  set_matrix_camera_project(p_ber->p_project);

  kprintf(TRUE, "kam.fov = %.2fs", RAD2DEG(p_ber->kam.fov));
  kprintf(TRUE, "kam.near_plane = %.2fm", p_ber->kam.near_plane);
  kprintf(TRUE, "kam.far_plane = %.2fm", p_ber->kam.far_plane);
  kprintf(TRUE, "kam.min_degree = %.2fs", RAD2DEG(p_ber->kam.min_uhel));
  kprintf(TRUE, "kam.max_degree = %.2fs", RAD2DEG(p_ber->kam.max_uhel));
  kprintf(TRUE, "kam.min_dist = %.2fm", p_ber->kam.min_vzdal);
  kprintf(TRUE, "kam.max_dist = %.2fm", p_ber->kam.max_vzdal);
}

void ber_uloz_lookup(G_KONFIG * p_ber)
{
  bitmapa *p_bmp1 = bmp_vyrob(256, 256);
  bitmapa *p_bmp2 = bmp_vyrob(256, 256);
  dword barva;
  float i1, i2;
  int x, y, i;

  for (y = 0; y < 256; y++) {
    for (x = 0; x < 256; x++) {
      i = y * MAX_LOOK_TABLE + x;

      i1 = (float) p_ber->p_looksqrt_linear[i] / (0xffffff);
      i2 = (float) p_ber->p_looksqrt_quadratic[i] / (0xffffff);

      barva = 0;
      add_mult_color(&barva, 0xffffffff, i1);
      bmp_putpixel(p_bmp1, x, y, barva);

      barva = 0;
      add_mult_color(&barva, 0xffffffff, i2);
      bmp_putpixel(p_bmp2, x, y, barva);
    }
  }
  bmp_uloz("e:\\linear.bmp", p_bmp1);
  bmp_uloz("e:\\kvad.bmp", p_bmp2);
}

void ber_rekonfiguruj_hra(G_KONFIG * p_ber)
{
  char pom[200];

  p_ber->conf_start_zpruhlednovac =
    GetPrivateProfileInt("hra", "start_zpruhlednovani", 0, ini_file);
  p_ber->conf_start_zvyraznovac =
    GetPrivateProfileInt("hra", "start_zvyraznovani", 0, ini_file);
  p_ber->conf_ovladani_rohy =
    GetPrivateProfileInt("hra", "ovladani_rohy", 1, ini_file);
  p_ber->conf_ovladani_rohy_default =
    GetPrivateProfileInt("hra", "ovladani_rohy_default", 0, ini_file);
  p_ber->conf_ovladani_rohy_smer =
    GetPrivateProfileInt("hra", "ovladani_rohy_smer", 0, ini_file);
  GetPrivateProfileString("hra", "ovladani_rohy_rychlost", "0.5", pom, 200,
    ini_file);
  p_ber->conf_ovladani_rohy_rychlost = (float) atof(pom);
  GetPrivateProfileString("hra", "p_kamera_radius", "4.0", pom, 200,
    ini_file);
  p_ber->conf_pruhledna_kamera_radius = (float) atof(pom);
}

void ber_rekonfiguruj_3D(G_KONFIG * p_ber)
{
  char pom[200];

  hwconf.ditering = GetPrivateProfileInt("hra", "ditering", 0, ini_file);

  p_ber->conf_zrcadlo =
    GetPrivateProfileInt("hra", "zrcado_aktivni", 1, ini_file);
  p_ber->conf_animace_okoli =
    GetPrivateProfileInt("hra", "animace_okoli", 1, ini_file);
  p_ber->conf_caustic =
    GetPrivateProfileInt("hra", "scene_materialy", 1, ini_file);

  GetPrivateProfileString("textury", "text_ostrost", "0.0", pom, 200,
    ini_file);
  txconf.text_ostrost = (float) atof(pom);

  // 0/1 - on/off
  // 1,2,3 - vysoka/stredni/nizka
  p_ber->conf_dyn_light =
    GetPrivateProfileInt("hra", "light_dyn", 1, ini_file);
  if (p_ber->conf_dyn_light)
    p_ber->conf_dyn_rychlost =
      GetPrivateProfileInt("hra", "light_rychlost", 1, ini_file);
  else
    p_ber->conf_dyn_rychlost = LIGHT_NIC;
}

void ber_rekonfiguruj_3D_aplikuj(G_KONFIG * p_ber)
{
  if (hwconf.ditering) {
    glEnable(GL_DITHER);
  }
  else {
    glDisable(GL_DITHER);
  }

  dl_nastav(p_ber);
}

#define DEF_PROJECT_FOV DEG2RAD(20.0f)

// taky float lok-up tabulka !!!
void ber_konfiguruj_berusky(G_KONFIG * p_ber)
{
  char pom[200];
  int i;

  p_ber->conf_barva_pozadi = 0xff000000;

  p_ber->kam_omezeni =
    !GetPrivateProfileInt("debug", "debug_kamery", 0, ini_file);
  p_ber->kam.fov = DEF_PROJECT_FOV;
  p_ber->kam.far_plane = MAX_VZDAL_Z;
  p_ber->kam.near_plane = MIN_VZDAL_Z;
  p_ber->kam.max_uhel = MAX_UHEL_KAMERY;
  p_ber->kam.min_uhel = MIN_UHEL_KAMERY;
  p_ber->kam.max_vzdal = MAX_VZDAL_KAMERY;
  p_ber->kam.min_vzdal = MIN_VZDAL_KAMERY;

  p_ber->conf_zpruhlednovac = FALSE;

  set_matrix_init();

  konfiguruj_look_up(p_ber->p_looksqrt_linear,
    p_ber->p_looksqrt_quadratic,
    p_ber->p_looksqrt_linear_float, p_ber->p_looksqrt_quadratic_float);

  for (i = 0; i < SIN_TABLE_SIZE; i++)
    p_ber->sinus_table[i] = sinf(DEG2RAD(i));

  GetPrivateProfileString("game", "prvky_dir", ".", p_ber->dir.prvky_dir,
    MAX_PATH, ini_file);
  working_file_translate(p_ber->dir.prvky_dir, MAX_PATH);
  GetPrivateProfileString("game", "level_dir", ".", p_ber->dir.level_dir,
    MAX_PATH, ini_file);
  working_file_translate(p_ber->dir.level_dir, MAX_PATH);
  GetPrivateProfileString("game", "out_dir", ".", p_ber->dir.out_dir,
    MAX_PATH, ini_file);
  working_file_translate(p_ber->dir.out_dir, MAX_PATH);
  GetPrivateProfileString("game", "material_dir", ".",
    p_ber->dir.material_dir, MAX_PATH, ini_file);
  working_file_translate(p_ber->dir.material_dir, MAX_PATH);
  GetPrivateProfileString("game", "data_dir", ".", p_ber->dir.data_dir,
    MAX_PATH, ini_file);
  working_file_translate(p_ber->dir.data_dir, MAX_PATH);
  GetPrivateProfileString("game", "game_level_dir", ".",
    p_ber->dir.game_level_dir, MAX_PATH, ini_file);
  working_file_translate(p_ber->dir.game_level_dir, MAX_PATH);
  GetPrivateProfileString("game", "game_data_dir", ".",
    p_ber->dir.game_data_dir, MAX_PATH, ini_file);
  working_file_translate(p_ber->dir.game_data_dir, MAX_PATH);
  GetPrivateProfileString("game", "texture_dir", ".",
    p_ber->tdir.texture_dir[0], MAX_PATH, ini_file);
  working_file_translate(p_ber->tdir.texture_dir[0], MAX_PATH);

  for (i = 0; i < TEXT_DIRS; i++) {
    sprintf(pom, "texture_dir%d", i);
    GetPrivateProfileString("game", pom, "", p_ber->tdir.texture_dir[i],
      MAX_PATH, ini_file);
    working_file_translate(p_ber->tdir.texture_dir[i], MAX_PATH);
    if (p_ber->tdir.texture_dir[i][0] == '.'
      && !p_ber->tdir.texture_dir[i][1])
      p_ber->tdir.texture_dir[i][0] = 0;
    sprintf(pom, "texture_dir%d_class", i);
    p_ber->tdir.texture_dir_class[i] =
      GetPrivateProfileInt("game", pom, 0, ini_file);
  }

  for (i = 0; i < TEXT_DIRS; i++) {
    sprintf(pom, "texture_file%d", i);
    GetPrivateProfileString("game", pom, "", p_ber->tdir.texture_file[i],
      MAX_PATH, ini_file);
    working_file_translate(p_ber->tdir.texture_file[i], MAX_PATH);
    if (p_ber->tdir.texture_file[i][0] == '.'
      && !p_ber->tdir.texture_file[i][1])
      p_ber->tdir.texture_file[i][0] = 0;
    sprintf(pom, "texture_file%d_class", i);
    p_ber->tdir.texture_file_class[i] =
      GetPrivateProfileInt("game", pom, 0, ini_file);
  }

  p_ber->conf_barva_pozadi_pouzit_default = p_ber->conf_barva_pozadi_pouzit =
    GetPrivateProfileInt("hra", "barva_pozadi", 1, ini_file);
  p_ber->conf_extra_light_vertex =
    !GetPrivateProfileInt("hra", "extra_light_vertex", 0, ini_file);

  ber_rekonfiguruj_hra(p_ber);
  ber_rekonfiguruj_3D(p_ber);

#ifdef DEBUG_MOD
  p_ber->debug_fps = GetPrivateProfileInt("debug", "debug_fps", 0, ini_file);
  p_ber->debug_vypisy =
    GetPrivateProfileInt("debug", "debug_vypisy", 0, ini_file);
  p_ber->debug_light =
    GetPrivateProfileInt("debug", "debug_light", 0, ini_file);
  p_ber->debug_draty =
    GetPrivateProfileInt("debug", "debug_draty", 0, ini_file);
#endif

  nahod_state_bloky();

  /* Nastaveni debugu
   */
#ifdef DEBUG_MOD
  p_ber->debug_frame = TRUE;
#endif

  kprintf(TRUE, "Configuration done");
}

/*
BOOL CALLBACK doe_callback_konzole(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)  {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDCANCEL:
  				EndDialog( hDlg, IDCANCEL );
          break;
        case IDOK: {          
          EndDialog( hDlg, IDCANCEL );
          break;
        }
        default:
          return TRUE;
      }
      break;    
    case WM_INITDIALOG: {
       break;
    }
    case WM_DESTROY:
      EndDialog( hDlg, IDCANCEL );
      break;
    default:
      return FALSE;
  }
  return TRUE;
}
*/

void kprintf(char log, const char *p_text, ...)
{
  char text[2000];
  va_list argumenty;

  va_start(argumenty, p_text);
  vsprintf(text, p_text, argumenty);
  va_end(argumenty);

  fprintf(stderr, "%s\n", text);

  if (log && p_ber->debug_file) {
    fputs(text, p_ber->debug_file);
    fputc('\n', p_ber->debug_file);
    fflush(p_ber->debug_file);
  }
}

void kprintfe(char log, const char *p_text, ...)
{
  char text[2000];
  va_list argumenty;

  va_start(argumenty, p_text);
  vsprintf(text, p_text, argumenty);
  va_end(argumenty);

  if (log && p_ber->debug_file) {
    fputs(text, p_ber->debug_file);
    fputc('\n', p_ber->debug_file);
    fflush(p_ber->debug_file);
  }
  exit(-1);
}

void kprintfl(char log, const char *p_text, ...)
{
  char text[2000];
  va_list argumenty;

  va_start(argumenty, p_text);
  vsprintf(text, p_text, argumenty);
  va_end(argumenty);

  fprintf(stderr, text);

  if (log && p_ber->debug_file) {
    fputs(text, p_ber->debug_file);
    fputc('\n', p_ber->debug_file);
    fflush(p_ber->debug_file);
  }
}

void kerror(char log, const char *p_text, ...)
{
  char text[2000];
  va_list argumenty;

  va_start(argumenty, p_text);
  vsprintf(text, p_text, argumenty);
  va_end(argumenty);

  fprintf(stderr, "FATAL ERROR: %s\n", text);

  if (log && p_ber->debug_file) {
    fputs(text, p_ber->debug_file);
    fputc('\n', p_ber->debug_file);
    fflush(p_ber->debug_file);
  }
  
  assert(0);
  exit(-1);
}

void ddw(const char *p_text, ...)
{
  char text[2000];
  va_list argumenty;

  va_start(argumenty, p_text);
  vsprintf(text, p_text, argumenty);
  va_end(argumenty);

  if (p_ber->debug_file) {
    fputs(text, p_ber->debug_file);
    fputc('\n', p_ber->debug_file);
    fflush(p_ber->debug_file);
  }
}


// Nahraje vlastnosti levelu a nestavy tyto vlastnosti
void ber_init_level(G_KONFIG * p_ber)
{
  float b[4];

  glstav_reset();

  if (p_ber->conf_barva_pozadi_pouzit) {
    rgb_float(p_ber->conf_barva_pozadi, b);
    glClearColor(b[0], b[1], b[2], 1.0f);
  }

  p_ber->dl_recalc = TRUE;
  p_ber->dl_new = TRUE;
  p_ber->dl_on = TRUE;
  p_ber->edl_recalc = TRUE;
  p_ber->edl_new = TRUE;

  /* Inicializace casu
   */
  p_ber->TimeEndLastFrame = p_ber->TimeBase = timeGetTime() / DELICKA_CASU;
  p_ber->TimeLastFrame = 30;

  /* Prvotni init anim materialu
   */
  ani_init(p_ber);

  /* Nahozeni animaci
   */
  tani_init_scena(p_ber);

  /* Inicializace kamery
   */
  ber_ini_mod(p_ber);
  ber_nahod_mod(p_ber);

  /* Nahozeni zemni mlhy
   */
  if (p_ber->zm.aktivni) {
    p_ber->zm.handle =
      edl_svetlo_vyrob_zemni_mlha(p_ber->zm.barva, p_ber->zm.v_start,
      p_ber->zm.v_stop, p_ber->zm.h_start, p_ber->zm.h_stop);
  }

  /* Nactu scan-code pro P,R
   */
  gl_Get_PR_ScanCode(&p_ber->conf_p_key, &p_ber->conf_r_key);

  /* Zpruhlednovani prvku pred kamerou
   */
  if (p_ber->conf_start_zpruhlednovac)
    kom_zpruhlednovat_prvky(TRUE);

  /* Nahodi zvyraznovani prvku
   */
  p_ber->conf_full_light = !p_ber->conf_start_zvyraznovac;
  ber_prikaz_zvyraznovac(p_ber);

  /* Vypne kurzor
   */
  vypni_kurzor();

  /* Hodim kurzor do poloviny obrazovky
   */
  //ber_nastav_kurzor(OXRES/2,OYRES/2);

  /* Nastavi svetla v levelu
   */
  dl_nastav(p_ber);

  /* Reset mlhy
   */
  mlha_reset();

  /* Reload ini
   */
  kom_reload_ini();

  /* Nastavi flagose
   */
  p_ber->ref_pos = TRUE;
  p_ber->ref_rot = TRUE;
  p_ber->ref_zmena = FALSE;

  p_ber->sflag1 = FALSE;
}


#ifdef DEBUG_MOD
void ber_tiskni_statistiku(G_KONFIG * p_ber)
{
  PARMETAC *p_par;
  GAME_MESH_OLD *p_mesh;
  GK_ANIM *p_gk;
  float dir[6];
  BOD pos;
  int start = 2;
  int i;

  //int        posun = 12;
  int posun = 20;
  int ppart, psystemu;
  int dlnum = 0;
  int edlnum = 0;
  float time;

  if (p_ber->debug_fps) {
    printfs(start, posun++, "f %.2f m %d o %d f %d", p_ber->fps,
      p_ber->debug.meshu, p_ber->debug.objektu, p_ber->debug.facu / 3);
  }

  kam_get_float_ext(dir, (float *) &pos);

  if (p_ber->debug_vypisy) {
    printfs(start, posun++, "bodu %d", p_ber->debug.bodu);
    printfs(start, posun++, "facu %d", p_ber->debug.facu / 3);
    printfs(start, posun++, "objektu %d", p_ber->debug.objektu);
    printfs(start, posun++, "poly rendered %d / total %d", p_ber->debug.poly,
      p_ber->polynum);
    printfs(start, posun++, "meshu rendered %d / total %d",
      p_ber->debug.meshu, p_ber->meshnum);
    printfs(start, posun++,
      "render list poly num %d / akt = %d, mesh num = %d / akt = %d",
      p_ber->poly_rendernum, p_ber->poly_renderakt, p_ber->mesh_rendernum,
      p_ber->mesh_renderakt);
    printfs(start, posun++, "ac %d tc %d", p_ber->debug.alfa_stage_change,
      p_ber->debug.text_stage_change);
    printfs(start, posun++, "pruhlednych %d", p_ber->prhlnum);
    printfs(start, posun++, "mouse [mi.t1 = %d mi.t2 = %d]", mi.t1, mi.t2);
    printfs(start, posun++, "r[%.2f] fi[%.2f] vzdal[%.2f] vzdal_kor[%.2f]",
      RAD2DEG(*p_ber->p_rfi), RAD2DEG(*p_ber->p_fi), *p_ber->p_vzdal,
      *p_ber->p_vzdal_kor);
    printfs(start, posun++, "t[%.2f,%.2f,%.2f]", p_ber->p_target->x,
      p_ber->p_target->y, p_ber->p_target->z);
    printfs(start, posun++, "p[%.2f,%.2f,%.2f]", p_ber->p_pos->x,
      p_ber->p_pos->y, p_ber->p_pos->z);
    printfs(start, posun++, "AK = %d PRCameraFlag = %d",
      p_ber->kamera.a_handle, PRCameraFlag);
    for (i = 0; i < p_ber->dl_lightnum; i++) {
      if (p_ber->p_dlight[i].akt)
        dlnum++;
    }
    for (i = 0; i < p_ber->edl_lightnum; i++) {
      if (p_ber->p_edlight[i].akt)
        edlnum++;
    }
    printfs(start, posun++, "LVertexu = %d sl = %d dl = %d edl = %d",
      p_ber->debug.light_vertexu, p_ber->slightnum, dlnum, edlnum);
    printfs(start, posun++, "LMeshu = %d", p_ber->debug.light_meshu);

    ppart = 0;
    psystemu = 0;
    p_par = p_ber->p_par;
    while (p_par) {
      ppart += p_par->pnum;
      psystemu++;
      p_par = p_par->p_next;
    }
    printfs(start, posun++, "systemu %d castic %d", psystemu, ppart);
    printfs(start, posun++, "hnizd %d viditelnych %d", p_ber->debug.hnizd,
      p_ber->debug.hnizd_vid);
    printfs(start, posun++, "Lani_vid %d", p_ber->debug.lani_vid);
    printfs(start, posun++, "Vertex Obj %d", p_ber->debug.vertex_obj);
//    p_light = &p_ber->p_edlight[0].tp;
//    printfs(start,posun++, "svetlo [%.3f %.3f %.3f] dosah %f",p_light->x,p_light->y,p_light->z,p_ber->p_edlight[0].dosah);

    for (i = 0; i < p_ber->gknum; i++) {
      p_gk = p_ber->p_gkzas + i;
      if (p_gk->flag & GK_AKTIVNI) {
        time = p_gk->time / (float) p_gk->time_delka;
        printfs(start, posun++, "G slot %d time %f", i, time);
      }
    }

    for (i = 0; i < p_ber->lokalnum; i++) {
      if (p_ber->p_lokal[i]) {
        p_mesh = p_ber->p_lokal[i];
        time =
          p_mesh->sim_aktivni_time / (float) (p_mesh->p_sim_aktivni->keynum *
          SIM_KONSTI);
        printfs(start, posun++, "L slot %d time %f", i, time);
      }
    }
    printfs(start, posun++, "time = %d", timeGetTime());
  }
}
#endif

void cekej(int kolik)
{
  int i, j, k, l;

  for (i = 0; i < kolik; i++) {
    for (j = 0; j < kolik; j++) {
      for (k = 0; k < kolik; k++) {
        for (l = 0; l < kolik; l++) {
          spracuj_spravy(1);
        }
      }
    }
  }
}

/*
  1 - ok, dohral to
  0 - abort
*/
/*
  p - posun sceny
  r - rotace sceny
  +,-  - zoom kamery
*/
float ber_stredni_vzdalenost_ve_scene(G_KONFIG * p_ber)
{
  BOD a, b, p;
  ROVINA r(0.0f, 1.0f, 0.0f, 0.0f);

  transf_2d_3d((float) OXRES / 2, (float) OYRES / 2, 0,
    &a.x, &a.y, &a.z,
    p_ber->p_camera, p_ber->p_project, OXRES, OYRES, OXSTART, OYSTART);
  transf_2d_3d((float) OXRES / 2, (float) OYRES / 2, 0.5f,
    &b.x, &b.y, &b.z,
    p_ber->p_camera, p_ber->p_project, OXRES, OYRES, OXSTART, OYSTART);
  if (!calc_prusek_bod(&r, &a, &b, &p))
    return (10.0f);
  else
    return (vzdal_bodu_bod(&a, &p));
}

/*
  inline ROVINA * calc_rovinu_bod_vektor(BOD *p, BOD *v, ROVINA *r)
*/
void ber_prikaz_posun_sceny(G_KONFIG * p_ber, int ref)
{
  static BOD o1, v1;
  static int rx, ry;
  BOD v2;

  if (!PRCameraFlag)
    return;

  PRCameraFlagChange = TRUE;

  nuluj_mys();
  nuluj_dif();

  kam_stop();
  kam_set_mod_polar();

  if (ref) {
    rx = mi.x;
    ry = mi.y;
  }
  else {
    transf_2d_3d_z((float) rx, (float) ry, *p_ber->p_vzdal_kor,
      &v1.x, &v1.y, &v1.z,
      p_ber->p_camera, p_ber->p_project,
      OXRES, OYRES, OXSTART, OYSTART,
      p_ber->kam.near_plane, p_ber->kam.far_plane);

    transf_2d_3d_z((float) mi.x, (float) mi.y, *p_ber->p_vzdal_kor,
      &v2.x, &v2.y, &v2.z,
      p_ber->p_camera, p_ber->p_project,
      OXRES, OYRES, OXSTART, OYSTART,
      p_ber->kam.near_plane, p_ber->kam.far_plane);

    p_ber->p_target->x += v1.x - v2.x;
    p_ber->p_target->z += v1.z - v2.z;
    if (!p_ber->kam_omezeni) {
      p_ber->p_target->y += v1.y - v2.y;
    }

    if (p_ber->kam_omezeni) {
      kd_je_bod_v_kostce_orez(p_ber->p_target, &p_ber->level_min,
        &p_ber->level_max);
    }

    ber_kamera_korekce_vzdalenosti(p_ber, p_ber->kam_omezeni, FALSE);

    nuluj_dif();

    rx = mi.x;
    ry = mi.y;
  }

  mi.dx = 1;                    // kvuli kofolovy - aby se ten kurzor nestracel
  p_ber->kamera.zmena = TRUE;
}

void ber_prikaz_rotace_sceny(G_KONFIG * p_ber, int ref)
{
  static int rx, ry;

  if (!PRCameraFlag)
    return;

  PRCameraFlagChange = TRUE;

  nuluj_mys();
  nuluj_dif();

  kam_stop();
  kam_set_mod_polar();

  if (ref) {
    rx = mi.x;
    ry = mi.y;
  }
  else {
    *p_ber->p_fi += ((float) PI / 180) * (float) (mi.y - ry) * 0.5f;
    *p_ber->p_rfi -= ((float) PI / 180) * (float) (mi.x - rx) * 0.5f;

    if (p_ber->kam_omezeni) {
      if (*p_ber->p_fi < p_ber->kam.min_uhel)
        *p_ber->p_fi = p_ber->kam.min_uhel;
      else {
        if (*p_ber->p_fi > p_ber->kam.max_uhel)
          *p_ber->p_fi = p_ber->kam.max_uhel;
      }
    }

    ber_kamera_korekce_vzdalenosti(p_ber, p_ber->kam_omezeni, FALSE);

    rx = mi.x;
    ry = mi.y;
  }

  p_ber->kamera.zmena = TRUE;
  mi.dx = 1;                    // kvuli kofolovy - aby se ten kurzor nestracel
}

void ber_prikaz_scena_zoom(int koeficient)
{
  kam_stop();
  kam_set_mod_polar();

  *p_ber->p_vzdal += koeficient * (*p_ber->p_vzdal) * 0.1f;

  if (*p_ber->p_vzdal < KAM_MIN_VZDAL)
    *p_ber->p_vzdal = KAM_MIN_VZDAL;
  if (p_ber->kam_omezeni && *p_ber->p_vzdal < p_ber->kam.min_vzdal)
    *p_ber->p_vzdal = p_ber->kam.min_vzdal;
  if (p_ber->kam_omezeni && *p_ber->p_vzdal > MAX_VZDAL_KAMERY)
    *p_ber->p_vzdal = MAX_VZDAL_KAMERY;

  ber_kamera_korekce_vzdalenosti(p_ber, p_ber->kam_omezeni, TRUE);
  p_ber->kamera.zmena = TRUE;
  PRCameraFlagChange = FALSE;
}

void ber_prikaz_zvyraznovac(G_KONFIG * p_ber)
{
  p_ber->conf_full_light_last = p_ber->conf_full_light;
  p_ber->conf_full_light = !p_ber->conf_full_light;
  p_ber->dl_new = p_ber->dl_recalc = TRUE;

  dl_nastav(p_ber);
}

void ber_test_animaci(G_KONFIG * p_ber)
{
  ExMeshHandle mh, next;
  ChapadloHandle *p_handle = NULL;
  int chapadel;
  static int flag;
  int i, j, objektu;

  /* -----------------------------------------------------------
     Tuto cast musis delat nekde na zacatku - trva to dlouho
     (nacteni animaci objektu)
   */

  /* Smycka - hleda a vraci postupne vsechny meshe ve scene ktere
     maji joint-animace
   */
  next = 0;
  while ((mh = kom_najdi_mesh_joint(&next)) != K_CHYBA) {
    /* U nalezeneho meshe nacte pocet objektu ktere maji joint-animace
     */
    objektu = chani_mesh_cti_objekty(mh);

    /* Projede objekty meshe (pocet objektu s animacema = objektu)
       a spusti jejich prvni animaci
     */
    for (i = 0; i < objektu; i++) {

      /* Nacni animace i-teho objektu v poradi
       */
      chani_mesh_cti_chapadelniky(mh, i, &p_handle, &chapadel);

      /* chapadel = pocet animaci objektu
         p_handle = pole handlu na tyto animace
       */

      /* Vrati jmena animaci objektu
       */
      for (j = 0; j < chapadel; j++) {
        char *p_jmeno = chani_cti_jmeno(p_handle[j]);
        (void)p_jmeno; //let's make gcc happy :)
      }

      /* -----------------------------------------------------------
         Tuto cast muzes delat behem hry - je to rychle 
       */

      /* Vyberu si 1. animaci v objektu a tu zaradim do seznamu animaci
         (!!! pozor !!! pri zarazovani animace nekontroluju jestli ta animace 
         tam uz je nebo ne, tak to musis volat jen 1x! nejlepe pred spustenim
         animace)
       */
      j = 0;
      chani_zarad(p_handle[j]);

      /* Zarazenou animaci spustim
       */
      chani_go(p_handle[j], &flag, GK_LOOP, 0, 0);

      /* Uvolnim pole handlu (je to pole integeru, alokuje se 
         pri volani chani_mesh_cti_chapadelniky pomoci malloc
         a ty to musis uvolnit kdyz to pole nepotrebujes
         To pole si klidne muzes ulozit do nejake sve struktury,
         ja ho k nicemu nepozuivam a je urceny jen pro tebe
       */
      free(p_handle);
    }
  }


  /* Postup pri ruseni bezici animace:

     1) Zastavim animaci
     chani_stop(p_handle[j]);

     2) Vymazu ji z fronty animaci
     chani_zrus(p_handle[j]);

   */
}

void ber_test_animaci2(G_KONFIG * p_ber)
{
}

void ber_test_animaci3(G_KONFIG * p_ber)
{
}

/*
  dword barva = 0xffffff;
  ExtraLightHandle elh = edl_svetlo_vyrob_zemni_mlha(barva, 0.0f, 3.0f, 0.0f, 60.0f);
*/
/*
   z_buffer_value = ( a + b / z )
  
   N = number of bits of Z precision
   a = zFar / ( zFar - zNear )
   b = zFar * zNear / ( zNear - zFar )
   z = distance from the eye to the object

   ...and z_buffer_value is an integer.
*/

#define PEVNA_RYCH_POSUN      100.0f
#define PEVNA_RYCH_ROTACE     100.0f
#define PEVNA_RYCH_ZOOM       0.5f

#define POHYBLIVA_RYCH_POSUN  600.0f
#define POHYBLIVA_RYCH_ROTACE 400.0f
#define POHYBLIVA_RYCH_ZOOM   1.0f

// Aktivovat posun v 2D osach x,y
void ber_ovladani_mysi_posun(G_KONFIG * p_ber, int x, int y)
{
  float rych_posun =
    PEVNA_RYCH_POSUN +
    p_ber->conf_ovladani_rohy_rychlost * POHYBLIVA_RYCH_POSUN;
  float koef = p_ber->TimeLastFrame / 1000.0f;
  float mx1, my1, mx2, my2;
  GLMATRIX proj;
  BOD v1, v2;

  if (!PRCameraFlag)
    return;

  PRCameraFlagChange = TRUE;

  kam_stop();
  kam_set_mod_polar();

  if (x) {
    mx1 = (float) OXSTART;
    mx2 = mx1 - x * rych_posun * koef;
  }
  else {
    mx1 = mx2 = 0;
  }

  if (y) {
    my1 = (float) (OYSTART);
    my2 = my1 - y * rych_posun * koef;
  }
  else {
    my1 = my2 = 0;
  }

  calc_transf_3d_2d_matrix_smpl(p_ber->p_camera, p_ber->p_project, &proj);
  transf_2d_3d_z_matrix(mx1, my1, *p_ber->p_vzdal_kor,
    &v1.x, &v1.y, &v1.z, &proj, OXRES, OYRES, OXSTART, OYSTART,
    p_ber->kam.near_plane, p_ber->kam.far_plane);
  transf_2d_3d_z_matrix(mx2, my2, *p_ber->p_vzdal_kor,
    &v2.x, &v2.y, &v2.z, &proj, OXRES, OYRES, OXSTART, OYSTART,
    p_ber->kam.near_plane, p_ber->kam.far_plane);

  p_ber->p_target->x += v1.x - v2.x;
  p_ber->p_target->z += v1.z - v2.z;
  if (!p_ber->kam_omezeni) {
    p_ber->p_target->y += v1.y - v2.y;
  }

  if (p_ber->kam_omezeni) {
    kd_je_bod_v_kostce_orez(p_ber->p_target, &p_ber->level_min,
      &p_ber->level_max);
  }

  ber_kamera_korekce_vzdalenosti(p_ber, p_ber->kam_omezeni, FALSE);

  p_ber->kamera.zmena = TRUE;
  mi.dx = 1;
}

// Rotace x,y - 2D osy
void ber_ovladani_mysi_rotace(G_KONFIG * p_ber, int x, int y)
{
  float rych_rotace =
    PEVNA_RYCH_ROTACE +
    p_ber->conf_ovladani_rohy_rychlost * POHYBLIVA_RYCH_ROTACE;
  float koef = p_ber->TimeLastFrame / 1000.0f;
  static int flag = K_CHYBA;

  if (!PRCameraFlag || flag != K_CHYBA)
    return;

  PRCameraFlagChange = TRUE;

  kam_stop();
  kam_set_mod_polar();

  if (x) {
    //camera_Fast_Turn(0,(char)(x < 0), &flag, 0.0f);
    *p_ber->p_rfi -=
      ((float) PI / 180) * (float) (-x * rych_rotace * koef) * 0.5f;
  }
  if (y) {
    *p_ber->p_fi +=
      ((float) PI / 180.0f) * (float) (-y * rych_rotace * koef) * 0.5f * 0.5f;
    if (*p_ber->p_fi >= DEG2RAD(88))
      *p_ber->p_fi = DEG2RAD(88);
  }

  if (p_ber->kam_omezeni) {
    if (*p_ber->p_fi < p_ber->kam.min_uhel)
      *p_ber->p_fi = p_ber->kam.min_uhel;
    else {
      if (*p_ber->p_fi > p_ber->kam.max_uhel)
        *p_ber->p_fi = p_ber->kam.max_uhel;
    }
  }

  ber_kamera_korekce_vzdalenosti(p_ber, p_ber->kam_omezeni, FALSE);
  p_ber->kamera.zmena = TRUE;
  mi.dx = 1;                    // kvuli kofolovy - aby se ten kurzor nestracel 
}

// Zoom y - +,-
void ber_ovladani_mysi_zoom(G_KONFIG * p_ber, int y)
{
  float rych_zoom =
    PEVNA_RYCH_ZOOM +
    p_ber->conf_ovladani_rohy_rychlost * POHYBLIVA_RYCH_ZOOM;
  float koef = p_ber->TimeLastFrame / 1000.0f;

  if (y > 0) {
    *p_ber->p_vzdal += rych_zoom * koef;
    if (p_ber->kam_omezeni && *p_ber->p_vzdal > MAX_VZDAL_KAMERY)
      *p_ber->p_vzdal = MAX_VZDAL_KAMERY;
  }
  else {
    *p_ber->p_vzdal -= rych_zoom * koef;
    if (p_ber->kam_omezeni && *p_ber->p_vzdal < p_ber->kam.min_vzdal)
      *p_ber->p_vzdal = p_ber->kam.min_vzdal;
    else if (*p_ber->p_vzdal < KAM_MIN_VZDAL)
      *p_ber->p_vzdal = KAM_MIN_VZDAL;
  }
}


// ovladani kdyz je kurzor na hranich obrazovky
#define X_HRANICE 4
#define Y_HRANICE 4

int ber_ovladani_mysi(G_KONFIG * p_ber, int *p_zmena)
{
  int posun, rotace, smer_pos, smer_rot;

  *p_zmena = FALSE;

  if (key[p_ber->conf_p_key] || key[p_ber->conf_r_key]
    || !(p_ber->conf_ovladani_rohy))
    return (TRUE);

  if (p_ber->conf_ovladani_rohy_default) {      // implicidne rotace
    posun = mi.ot2;
    rotace = !mi.ot2;
  }
  else {                        // implicidne posun
    posun = !mi.ot2;
    rotace = mi.ot2;
  }

  if (p_ber->conf_ovladani_rohy_smer) { // prehozeny smer ovladani
    smer_pos = -1;
    smer_rot = -1;
  }
  else {
    smer_pos = 1;
    smer_rot = 1;
  }

  if (mi.x <= OXSTART + X_HRANICE) {    // rotace/posun
    if (posun) {
      ber_ovladani_mysi_posun(p_ber, -smer_pos, FALSE);
      *p_zmena = TRUE;
    }
    else if (rotace) {
      ber_ovladani_mysi_rotace(p_ber, -smer_rot, FALSE);
      *p_zmena = TRUE;
    }
  }
  else if (mi.x >= OXSTART + OXRES - X_HRANICE) {       // rotace/posun/zoom
    if (posun) {
      ber_ovladani_mysi_posun(p_ber, smer_pos, FALSE);
      *p_zmena = TRUE;
    }
    else if (rotace) {
      ber_ovladani_mysi_rotace(p_ber, smer_rot, FALSE);
      *p_zmena = TRUE;
    }
  }

  if (mi.y <= OYSTART + Y_HRANICE) {    // rotace/posun
    if (posun) {
      ber_ovladani_mysi_posun(p_ber, 0, -smer_pos);
      *p_zmena = TRUE;
    }
    else if (rotace) {
      ber_ovladani_mysi_rotace(p_ber, 0, -smer_rot);
      *p_zmena = TRUE;
    }
  }
  else if (mi.y >= OYSTART + OYRES - Y_HRANICE) {       // rotace/posun/zoom
    if (posun) {
      ber_ovladani_mysi_posun(p_ber, 0, smer_pos);
      *p_zmena = TRUE;
    }
    else if (rotace) {
      ber_ovladani_mysi_rotace(p_ber, 0, smer_rot);
      *p_zmena = TRUE;
    }
  }
  return (TRUE);
}

void ber_klavesy_hra(G_KONFIG * p_ber)
{
  if (!karmin_aktivni)
    return;

  if (PRCameraFlag) {
    p_ber->ref_zmena = FALSE;

    if (ber_ovladani_mysi(p_ber, &p_ber->ref_zmena)) {
      if (p_ber->ref_zmena)
        p_ber->sflag1 |= SFL_POSUN;
    }

    if (key[p_ber->conf_p_key]) {
      p_ber->sflag1 |= SFL_POSUN;
      ber_prikaz_posun_sceny(p_ber, p_ber->ref_pos);
      p_ber->ref_pos = FALSE;
      p_ber->ref_zmena = TRUE;
    }
    else {
      p_ber->ref_pos = TRUE;
    }

    if (key[p_ber->conf_r_key]) {
      p_ber->sflag1 |= SFL_ROTACE;
      ber_prikaz_rotace_sceny(p_ber, p_ber->ref_rot);
      p_ber->ref_rot = FALSE;
      p_ber->ref_zmena = TRUE;
    }
    else {
      p_ber->ref_rot = TRUE;
    }

    if (key[K_PLUS] || key[K_MINUS]) {
      ber_prikaz_scena_zoom(key[K_PLUS] ? -1 : 1);
      p_ber->ref_zmena = TRUE;
      p_ber->sflag1 |= SFL_ZOOM;
      key[K_PLUS] = key[K_MINUS] = FALSE;
    }

    if (p_ber->sflag1 & (SFL_POSUN | SFL_ROTACE | SFL_ZOOM)
      && !p_ber->ref_zmena) {
      p_ber->sflag1 = FALSE;
      PRCameraFlagChange = 2;
    }
  }

#ifdef _DEBUG
  if (key[K_L]) {
    key[K_L] = FALSE;
    ber_test_animaci(p_ber);
  }
  if (key[K_K]) {
    key[K_K] = FALSE;
    ber_test_animaci2(p_ber);
  }
  if (key[K_J]) {
    key[K_J] = FALSE;
    ber_test_animaci3(p_ber);
  }
#endif

}
