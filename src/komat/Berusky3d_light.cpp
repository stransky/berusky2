/*
  berusky3d_light.c
  Svetelny modul k beruskam II

*/
#include <stdio.h>
#include <math.h>
#include <assert.h>
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

extern G_KONFIG ber, *p_ber;

inline void sdl_maxlook_korekce(DYN_LIGHT * p_light, float min, float max,
  float dosah)
{
  p_light->min = min;           // utlum je od 0 do 1
  if (dosah >= max) {
    p_light->max = max;
    p_light->dosah = max;
    p_light->look_max = 255;
  }
  else {
    p_light->max = max;
    p_light->dosah = dosah;
    p_light->look_max = (dosah * 255.0f) / max;
  }
}

/* 
  Test mod->render statickych a dynamickych svetel
*/
void dl_renderuj_svetla(G_KONFIG * p_ber)
{
  int i;

  set_matrix_world_init();

  for (i = 0; i < p_ber->slightnum; i++) {
    kresli_kosoctverec(p_ber->p_project, p_ber->p_camera, get_matrix_init(),
      &p_ber->p_slight[i].p, 0.5f, 1.0f, 1.0f, 1.0f);
  }
  for (i = 0; i < p_ber->dl_lightnum; i++) {
    if (p_ber->p_dlight[i].akt)
      kresli_kosoctverec(p_ber->p_project, p_ber->p_camera, get_matrix_init(),
        &p_ber->p_dlight[i].tp, 0.5f, 1.0f, 0.0f, 0.0f);
  }
  for (i = 0; i < p_ber->edl_lightnum; i++) {
    if (p_ber->p_edlight[i].akt) {
      kresli_kosoctverec(p_ber->p_project, p_ber->p_camera, get_matrix_init(),
        &p_ber->p_edlight[i].tp, 0.5f, 1.0f, 0.0f, 0.0f);
    }
  }
  glColor3f(1.0f, 1.0f, 1.0f);
}

/* ***********************************************
    Dynamicka scena
   ***********************************************
*/

static inline void dl_poly_diff_color_set(EDIT_MESH_POLY * p_poly, float r,
  float g, float b, float a)
{
  TEXT_KOORD *p_koord = p_poly->p_koord;
  int i;

  for (i = 0; i < p_poly->facenum; i++) {
    p_koord->da = a;
    p_koord->dr = r;
    p_koord->dg = g;
    p_koord->db = b;
    p_koord++;
  }
}

static inline void dl_poly_diff_color_add(EDIT_MESH_POLY * p_poly, float r,
  float g, float b, float a)
{
  TEXT_KOORD *p_koord = p_poly->p_koord;
  int i;

  for (i = 0; i < p_poly->facenum; i++) {
    p_koord->da += a;
    p_koord->dr += r;
    p_koord->dg += g;
    p_koord->db += b;
    p_koord++;
  }
}

static inline void dl_poly_diff_color_mult(EDIT_MESH_POLY * p_poly, float r,
  float g, float b, float a)
{
  TEXT_KOORD *p_koord = p_poly->p_koord;
  int i;

  for (i = 0; i < p_poly->facenum; i++) {
    p_koord->da *= a;
    p_koord->dr *= r;
    p_koord->dg *= g;
    p_koord->db *= b;
    p_koord++;
  }
}

static inline void dl_mesh_diff_color_set(GAME_MESH_OLD * p_mesh, float r,
  float g, float b, float a)
{
  BODRGBA *p_vertex;
  int i, vnum;

  p_vertex = p_mesh->p_vertex_diff;
  vnum = p_mesh->vertexnum;
  for (i = 0; i < vnum; i++) {
    p_vertex->a = a;
    p_vertex->r = r;
    p_vertex->g = g;
    p_vertex->b = b;
    p_vertex++;
  }
}

static inline void dl_mesh_diff_color_add(GAME_MESH_OLD * p_mesh, float r,
  float g, float b, float a)
{
  BODRGBA *p_vertex;
  int i, vnum;

  p_vertex = p_mesh->p_vertex_diff;
  vnum = p_mesh->vertexnum;
  for (i = 0; i < vnum; i++) {
    p_vertex->a += a;
    p_vertex->r += r;
    p_vertex->g += g;
    p_vertex->b += b;
    p_vertex++;
  }
}

inline void ld_mesh_diff_color_mul(GAME_MESH_OLD * p_mesh, float r, float g,
  float b, float a)
{
  BODRGBA *p_vertex;
  int i, vnum;

  p_vertex = p_mesh->p_vertex_diff;
  vnum = p_mesh->vertexnum;
  for (i = 0; i < vnum; i++) {
    p_vertex->a *= a;
    p_vertex->r *= r;
    p_vertex->g *= g;
    p_vertex->b *= b;
    p_vertex++;
  }
}

/* Spekularni
*/
static inline void dl_poly_spec_color_set(EDIT_MESH_POLY * p_poly, float r,
  float g, float b, float a)
{
  TEXT_KOORD *p_koord = p_poly->p_koord;
  int i;

  for (i = 0; i < p_poly->facenum; i++) {
    p_koord->sr = r;
    p_koord->sg = g;
    p_koord->sb = b;
    p_koord++;
  }
}

static inline void dl_poly_spec_color_add(EDIT_MESH_POLY * p_poly, float r,
  float g, float b, float a)
{
  TEXT_KOORD *p_koord = p_poly->p_koord;
  int i;

  for (i = 0; i < p_poly->facenum; i++) {
    p_koord->sr += r;
    p_koord->sg += g;
    p_koord->sb += b;
    p_koord++;
  }
}

static inline void dl_poly_spec_color_mult(EDIT_MESH_POLY * p_poly, float r,
  float g, float b, float a)
{
  TEXT_KOORD *p_koord = p_poly->p_koord;
  int i;

  for (i = 0; i < p_poly->facenum; i++) {
    p_koord->sr *= r;
    p_koord->sg *= g;
    p_koord->sb *= b;
    p_koord++;
  }
}

static inline void dl_mesh_spec_color_set(GAME_MESH_OLD * p_mesh, float r,
  float g, float b, float a)
{
  BODRGB *p_vertex;
  int i, vnum;

  p_vertex = p_mesh->p_vertex_spec;
  vnum = p_mesh->vertexnum;
  for (i = 0; i < vnum; i++) {
    p_vertex->r = r;
    p_vertex->g = g;
    p_vertex->b = b;
    p_vertex++;
  }
}

static inline void dl_mesh_spec_color_add(GAME_MESH_OLD * p_mesh, float r,
  float g, float b, float a)
{
  BODRGB *p_vertex;
  int i, vnum;

  p_vertex = p_mesh->p_vertex_spec;
  vnum = p_mesh->vertexnum;
  for (i = 0; i < vnum; i++) {
    p_vertex->r += r;
    p_vertex->g += g;
    p_vertex->b += b;
    p_vertex++;
  }
}

static inline void dl_mesh_spec_color_mul(GAME_MESH_OLD * p_mesh, float r,
  float g, float b, float a)
{
  BODRGB *p_vertex;
  int i, vnum;

  p_vertex = p_mesh->p_vertex_spec;
  vnum = p_mesh->vertexnum;
  for (i = 0; i < vnum; i++) {
    p_vertex->r *= r;
    p_vertex->g *= g;
    p_vertex->b *= b;
    p_vertex++;
  }
}

/* Aplikuje se na vsechny meshe a poly
*/

void dl_scena_set_ambient(float r, float g, float b, float a)
{
  p_ber->dl_ambient[0] = r;
  p_ber->dl_ambient[1] = g;
  p_ber->dl_ambient[2] = b;
  p_ber->dl_ambient[3] = a;
}

/* Prace s meshama
*/
int dl_mesh_modulate_diff(MeshHandle mh, float r, float g, float b, float a)
{
  GAME_MESH_OLD *p_mesh = p_ber->p_mesh[mh];

  if (p_mesh) {
    ld_mesh_diff_color_mul(p_mesh, r, g, b, a);
    return (TRUE);
  }
  return (FALSE);
}

int dl_mesh_set_diff(MeshHandle mh, float r, float g, float b, float a)
{
  GAME_MESH_OLD *p_mesh = p_ber->p_mesh[p_ber->p_prv_lev[mh]->mesh];

  if (p_mesh) {
    dl_mesh_diff_color_set(p_mesh, r, g, b, a);
    return (TRUE);
  }
  return (FALSE);
}

int dl_mesh_add_diff(MeshHandle mh, float r, float g, float b, float a)
{
  GAME_MESH_OLD *p_mesh = p_ber->p_mesh[p_ber->p_prv_lev[mh]->mesh];

  if (p_mesh) {
    dl_mesh_diff_color_add(p_mesh, r, g, b, a);
    return (TRUE);
  }
  return (FALSE);
}

int dl_mesh_modulate_spec(MeshHandle mh, float r, float g, float b, float a)
{
  GAME_MESH_OLD *p_mesh = p_ber->p_mesh[p_ber->p_prv_lev[mh]->mesh];

  if (p_mesh && p_mesh->p_data->m2flag & MAT2_SPECULAR) {
    dl_mesh_spec_color_mul(p_mesh, r, g, b, a);
    return (TRUE);
  }

  return (FALSE);
}

int dl_mesh_set_spec(MeshHandle mh, float r, float g, float b, float a)
{
  GAME_MESH_OLD *p_mesh = p_ber->p_mesh[p_ber->p_prv_lev[mh]->mesh];

  if (p_mesh && p_mesh->p_data->m2flag & MAT2_SPECULAR) {
    dl_mesh_spec_color_set(p_mesh, r, g, b, a);
    return (TRUE);
  }

  return (FALSE);
}

int dl_mesh_add_spec(MeshHandle mh, float r, float g, float b, float a)
{
  GAME_MESH_OLD *p_mesh = p_ber->p_mesh[p_ber->p_prv_lev[mh]->mesh];

  if (p_mesh && p_mesh->p_data->m2flag & MAT2_SPECULAR) {
    dl_mesh_spec_color_add(p_mesh, r, g, b, a);
    return (TRUE);
  }

  return (FALSE);
}

/* *******************************************************************
     Dynamicke svetla
   *******************************************************************
*/
inline void sdla_animaci_zrus(DYN_LIGHT_ANIM * p_an)
{
  p_an->endtime = 0;

  if (p_an->diff_keys) {
    null_free((void **) &p_an->p_dfkeys);
    null_free((void **) &p_an->p_diff);
    p_an->diff_keys = 0;
  }
  if (p_an->alfa_keys) {
    null_free((void **) &p_an->p_akeys);
    null_free((void **) &p_an->p_alfa);
    p_an->alfa_keys = 0;
  }
  if (p_an->pos_keys) {
    null_free((void **) &p_an->p_pkeys);
    null_free((void **) &p_an->p_pos);
    p_an->pos_keys = 0;
  }
  if (p_an->trg_keys) {
    null_free((void **) &p_an->p_tkeys);
    null_free((void **) &p_an->p_trg);
    p_an->trg_keys = 0;
  }
  if (p_an->dos_keys) {
    null_free((void **) &p_an->p_dskeys);
    null_free((void **) &p_an->p_dosah);
    p_an->dos_keys = 0;
  }
}

void sdla_animaci_zrus_online(DYN_LIGHT_ANIM * p_an)
{
  sdla_animaci_zrus(p_an);
}

inline void sdla_animaci_vyrob(DYN_LIGHT_ANIM * p_an, int framenum,
  int pos_keys, int dir_keys, int diff_keys,
  int spec_keys, int dos_keys, int alfa_keys)
{
  p_an->endtime = calc_endtime(framenum);

  p_an->pos_keys = pos_keys;
  if (pos_keys) {
    p_an->p_pkeys =
      (KEY_POINT_BRS *) mmalloc(sizeof(p_an->p_pkeys[0]) * pos_keys);
    p_an->p_pos = (BOD *) mmalloc(sizeof(p_an->p_pos[0]) * pos_keys);
  }
  p_an->trg_keys = dir_keys;
  if (dir_keys) {
    p_an->p_tkeys =
      (KEY_POINT_BRS *) mmalloc(sizeof(p_an->p_tkeys[0]) * dir_keys);
    p_an->p_trg = (BOD *) mmalloc(sizeof(p_an->p_trg[0]) * dir_keys);
  }
  p_an->dos_keys = dos_keys;
  if (dos_keys) {
    p_an->p_dskeys =
      (KEY_POINT_BRS *) mmalloc(sizeof(p_an->p_dskeys[0]) * dos_keys);
    p_an->p_dosah = (BOD *) mmalloc(sizeof(p_an->p_dosah[0]) * dos_keys);
  }
  p_an->diff_keys = diff_keys;
  if (diff_keys) {
    p_an->p_dfkeys =
      (KEY_POINT_BRS *) mmalloc(sizeof(p_an->p_dfkeys[0]) * diff_keys);
    p_an->p_diff =
      (BARVA_RGBA *) mmalloc(sizeof(p_an->p_diff[0]) * diff_keys);
  }
  p_an->alfa_keys = alfa_keys;
  if (alfa_keys) {
    p_an->p_akeys =
      (KEY_POINT_BRS *) mmalloc(sizeof(p_an->p_akeys[0]) * alfa_keys);
    p_an->p_alfa = (float *) mmalloc(sizeof(p_an->p_alfa[0]) * alfa_keys);
  }
  p_an->spec_keys = spec_keys;
  if (spec_keys) {
    p_an->p_spkeys =
      (KEY_POINT_BRS *) mmalloc(sizeof(p_an->p_spkeys[0]) * spec_keys);
    p_an->p_spec =
      (BARVA_RGBA *) mmalloc(sizeof(p_an->p_spec[0]) * spec_keys);
  }
}

/* keyframova animace svetel
*/
inline void sdl_light_to_light(DYN_LIGHT * p_light, DYN_LIGHT_ANIM * p_track,
  int time)
{
  int loop = p_light->an_flag & GK_LOOP;
  BOD d;

  if (p_track->pos_keys) {
    key_track_interpolace_bod(&p_light->np, p_track->p_pos, p_track->p_pkeys,
      time, p_track->endtime, p_track->pos_keys, loop);
    p_light->tp = p_light->np;
  }
  if (p_track->dos_keys) {
    key_track_interpolace_bod_linear(&d, p_track->p_dosah, p_track->p_dskeys,
      time, p_track->endtime, p_track->dos_keys, loop);
    sdl_maxlook_korekce(p_light, d.y, d.z, d.x);        // interpolace je dosah/min/max
  }
  if (p_track->diff_keys) {
    key_track_interpolace_wbod_linear((WBOD *) & p_light->dr,
      (WBOD *) p_track->p_diff, p_track->p_dfkeys, time, p_track->endtime,
      p_track->diff_keys, loop);
  }
}

LightHandle sdl_svetlo_vyrob(dword flag)
{
  DYN_LIGHT *p_light;
  int l;

  for (l = 0; l < p_ber->dl_lightnum; l++) {
    if (!p_ber->p_dlight[l].akt) {
      p_light = p_ber->p_dlight + l;
      goto nasel;
    }
  }
  p_light = p_ber->p_dlight + l;
  p_ber->dl_lightnum++;
nasel:

  p_light->akt = TRUE;
  p_light->flag = flag | SDL_ZMENA_GLOBAL | SDL_KOFOLA;

  p_ber->dl_new = TRUE;
  sprintf(p_light->jmeno, "Run-time svetlo %d", l);
  return (l);
}

LightHandle sdl_svetlo_pojmenuj(LightHandle handle, char *p_jmeno)
{
  if (!p_ber->p_dlight[handle].akt)
    return (FALSE);
  else
    strcpy(p_ber->p_dlight[handle].jmeno, p_jmeno);
  return (handle);
}

int sdl_svetlo_zrus(LightHandle handle)
{
  if (!p_ber->p_dlight[handle].akt)
    return (K_CHYBA);
  else {
    // Zruseni svetla
    p_ber->p_dlight[handle].akt = FALSE;
    if (p_ber->dl_lightnum == handle + 1)
      p_ber->dl_lightnum--;
    p_ber->dl_new = TRUE;

    // Zruseni pripadne animace
    if (p_ber->p_dlight[handle].an.endtime)
      sdla_animaci_zrus(&p_ber->p_dlight[handle].an);

    return (TRUE);
  }
}

// Vrati pocet dynamickych svetel
int sdl_get_num(void)
{
  int i, num = 0;

  for (i = 0; i < p_ber->dl_lightnum; i++)
    if (p_ber->p_dlight[i].akt)
      num++;
  return (num);
}

// Pointer na pole dynamickych svetel
int sdl_get_list(LIGHT_INFO * p_li, int max)
{
  int i, num = 0;

  for (i = 0; i < p_ber->dl_lightnum; i++) {
    if (p_ber->p_dlight[i].akt && num < max) {
      strcpy(p_li[num].jmeno, p_ber->p_dlight[i].jmeno);
      p_li[num].light_handle = i;
      p_li[num].light_handle = p_ber->p_dlight[i].ktrida;
      num++;
    }
  }
  return (num);
}

// Vrati pocet dynamickych svetel
int edl_get_num(void)
{
  int i, num = 0;

  for (i = 0; i < p_ber->edl_lightnum; i++)
    if (p_ber->p_edlight[i].akt)
      num++;
  return (num);
}

// Pointer na pole dynamickych svetel
int edl_get_list(LIGHT_INFO * p_li, int max)
{
  int i, num = 0;

  for (i = 0; i < p_ber->edl_lightnum; i++) {
    if (p_ber->p_edlight[i].akt && num < max) {
      strcpy(p_li[num].jmeno, p_ber->p_edlight[i].jmeno);
      p_li[num].light_handle = i;
      p_li[num].light_handle = p_ber->p_edlight[i].ktrida;
      num++;
    }
  }
  return (num);
}

/* *********************************************** 
     vyroba / ruseni svetel
   ***********************************************
*/
LightHandle sdl_svetlo_set_pos(LightHandle lh, BOD * p_bod)
{
  DYN_LIGHT *p_light = p_ber->p_dlight + lh;

  if (lh < p_ber->dl_lightnum && p_light->akt) {
    p_light->tp = p_light->np = *p_bod;
    p_light->flag |= SDL_ZMENA_GLOBAL;
    p_ber->dl_recalc = TRUE;
    return (lh);
  }
  else
    return (K_CHYBA);
}

LightHandle sdl_svetlo_set_pos_ext(LightHandle lh, float x, float y, float z)
{
  DYN_LIGHT *p_light = p_ber->p_dlight + lh;

  if (lh < p_ber->dl_lightnum && p_light->akt) {
    p_light->np.x = x;
    p_light->np.y = y;
    p_light->np.z = z;
    p_light->tp = p_light->np;
    p_light->flag |= SDL_ZMENA_GLOBAL;
    p_ber->dl_recalc = TRUE;
    return (lh);
  }
  else
    return (K_CHYBA);
}

/*
  min -> zacatek utlumu svetla
  max -> tady uz je to svetlo uplne neviditelne
*/
LightHandle sdl_svetlo_set_diff(LightHandle lh, float dr, float dg, float db,
  float min, float max, float dosah)
{
  DYN_LIGHT *p_light = p_ber->p_dlight + lh;

  if (lh < p_ber->dl_lightnum && p_light->akt) {
    p_light->dr = dr;           // zakladni barva svetla
    p_light->dg = dg;
    p_light->db = db;

    sdl_maxlook_korekce(p_light, min, max, dosah);

    p_light->flag |= SDL_ZMENA_LOKAL;
    p_ber->dl_recalc = TRUE;

    return (lh);
  }
  else
    return (K_CHYBA);
}

LightHandle sdl_svetlo_set_diff_sub(LightHandle lh, float sr, float sg,
  float sb)
{
  DYN_LIGHT *p_light = p_ber->p_dlight + lh;

  if (lh < p_ber->dl_lightnum && p_light->akt) {
    p_light->sr = sr;           // zakladni barva svetla
    p_light->sg = sg;
    p_light->sb = sb;
    p_light->flag |= SDL_ZMENA_LOKAL;
    p_ber->dl_recalc = TRUE;
    return (lh);
  }
  else
    return (K_CHYBA);
}

RunHandle sdl_svetlo_anim(LightHandle handle, BOD * p_bod,
  float dr, float dg, float db, float da,
  float dosah, float min, float max, int flag, int *p_flag, int framenum)
{
  return (TRUE);
}



LightHandle sdl_anim_vyrob(LightHandle lh, int framenum, int pos_keys,
  int diff_keys, int dosah_keys)
{
  DYN_LIGHT *p_light = p_ber->p_dlight + lh;
  DYN_LIGHT_ANIM *p_an;

  if (lh < p_ber->dl_lightnum && p_light->akt) {
    p_an = &p_light->an;
    if (p_an->endtime)
      sdla_animaci_zrus(p_an);
    sdla_animaci_vyrob(p_an, framenum, pos_keys, 0, diff_keys, 0, dosah_keys,
      0);
    return (lh);
  }
  else
    return (K_CHYBA);
}

LightHandle sdl_anim_zrus(LightHandle lh)
{
  DYN_LIGHT *p_light = p_ber->p_dlight + lh;

  if (lh < p_ber->dl_lightnum && p_light->akt && p_light->an.endtime) {
    sdla_animaci_zrus(&p_light->an);
    return (lh);
  }
  else
    return (K_CHYBA);
}

LightHandle sdl_anim_vloz_klic_posun(LightHandle lh, int klic, float x,
  float y, float z, int frame)
{
  DYN_LIGHT *p_light = p_ber->p_dlight + lh;
  DYN_LIGHT_ANIM *p_an = &p_light->an;

  if (lh < p_ber->dl_lightnum && p_light->akt &&
    p_an->endtime && klic < p_an->pos_keys) {
    p_an->p_pos[klic].x = x;
    p_an->p_pos[klic].y = y;
    p_an->p_pos[klic].z = z;
    p_an->p_pkeys[klic].time = frame * SIM_KONSTI;
    return (lh);
  }
  else
    return (K_CHYBA);
}

LightHandle sdl_anim_vloz_klic_posun_bod(LightHandle lh, int klic, BOD * p_p,
  int frame)
{
  DYN_LIGHT *p_light = p_ber->p_dlight + lh;
  DYN_LIGHT_ANIM *p_an = &p_light->an;

  if (lh < p_ber->dl_lightnum && p_light->akt && p_an->endtime
    && klic < p_an->pos_keys) {
    p_an->p_pos[klic] = *p_p;
    p_an->p_pkeys[klic].time = frame * SIM_KONSTI;
    return (lh);
  }
  else
    return (K_CHYBA);
}

LightHandle sdl_anim_vloz_klic_diff(LightHandle lh, int klic, float r,
  float g, float b, float a, int frame)
{
  DYN_LIGHT *p_light = p_ber->p_dlight + lh;
  DYN_LIGHT_ANIM *p_an = &p_light->an;

  if (lh < p_ber->dl_lightnum && p_light->akt && p_an->endtime
    && klic < p_an->diff_keys) {
    p_an->p_diff[klic].r = r;
    p_an->p_diff[klic].g = g;
    p_an->p_diff[klic].b = b;
    p_an->p_diff[klic].a = a;
    p_an->p_dfkeys[klic].time = frame * SIM_KONSTI;
    return (lh);
  }
  else
    return (K_CHYBA);
}

LightHandle sdl_anim_vloz_klic_vzdal(LightHandle lh, int klic, float dosah,
  float min, float max, int frame)
{
  DYN_LIGHT *p_light = p_ber->p_dlight + lh;
  DYN_LIGHT_ANIM *p_an = &p_light->an;

  if (lh < p_ber->dl_lightnum && p_light->akt && p_an->endtime
    && klic < p_an->dos_keys) {
    p_an->p_dosah[klic].x = dosah;
    p_an->p_dosah[klic].y = min;
    p_an->p_dosah[klic].z = max;
    p_an->p_dskeys[klic].time = frame * SIM_KONSTI;
    return (lh);
  }
  else
    return (K_CHYBA);
}



/* **************************************
    Run-time animace
   **************************************
*/
RunHandle sdl_anim_start(LightHandle lh, int *p_flag, int flag, int start,
  int stop)
{
  DYN_LIGHT *p_light = p_ber->p_dlight + lh;
  DYN_LIGHT_ANIM *p_an = &p_light->an;

  if (lh < p_ber->dl_lightnum && p_light->akt && p_an->endtime) {

    calc_time_end(p_an->endtime, p_ber->TimeEndLastFrame,
      start, stop,
      &p_light->start,
      &p_light->stop,
      &p_light->time_start, &p_light->time_stop, &p_light->time_delka);

    p_light->p_flag = p_flag;
    p_light->an_flag = flag;
    p_light->flag |= SDL_ANIMACE;
    return (lh);
  }
  else
    return (K_CHYBA);
}

int sdl_anim_stop(RunHandle rh)
{
  DYN_LIGHT *p_light = p_ber->p_dlight + rh;
  DYN_LIGHT_ANIM *p_an = &p_light->an;

  if (rh < p_ber->dl_lightnum && p_light->akt && p_an->endtime
    && p_light->flag & SDL_ANIMACE) {
    p_light->flag &= ~SDL_ANIMACE;
    return (rh);
  }
  else
    return (K_CHYBA);
}

/* keyframova animace svetel
*/
void sdla_updatuj(G_KONFIG * p_ber)
{
  dword next_time = p_ber->TimeEndLastFrame;
  DYN_LIGHT *p_light;
  DYN_LIGHT_ANIM *p_an;
  int i;
  int konec;

  /*
     Animace se updatuji kazdy frame
   */
  for (i = 0; i < p_ber->dl_lightnum; i++) {
    p_light = p_ber->p_dlight + i;
    p_an = &p_light->an;
    if (p_light->flag & SDL_REMOVE) {
      sdl_svetlo_zrus(i);
      continue;
    }
    if (p_light->flag & SDL_ANIMACE) {

      /* Test na konec animace
       */
      if ((konec = (next_time > p_light->time_stop))) {
        if (p_light->an_flag & GK_LOOP) {
          calc_time_loop(next_time, p_light->start,
            &p_light->time_start,
            &p_light->time_stop, &p_light->time_delka, &p_light->time);
          konec = 0;
          if (p_light->p_flag)
            *(p_light->p_flag) = 0;
        }
        else {
          p_light->time = p_light->time_delka;
          p_light->flag &= ~SDL_ANIMACE;
          if (p_light->p_flag)
            *(p_light->p_flag) = ANIM_FLAG_KONEC;
          if (p_light->an_flag & GK_REMOVE)
            p_light->flag |= SDL_REMOVE;
        }
      }
      else {
        p_light->time = calc_time_akt(next_time, p_light->time_start);
        if (p_light->p_flag)
          *(p_light->p_flag) =
            ftoi((p_light->time / (float) p_light->time_delka) * 100.0f);
      }

      /* Vypocitej animaci svetla
       */
      sdl_light_to_light(p_light, p_an, p_light->time);
      p_light->flag |= SDL_ZMENA_GLOBAL;
      p_ber->dl_recalc = TRUE;
    }
  }
}

// 1. Update svetel podle meshu
// 2. Update svetel privazane k meshum -> update specialnima rutinama !!!
// 3. Update svetel privazane k scene -> update specialnima rutinama !!!
int dl_mesh_pridej_svetla(G_KONFIG * p_ber, GAME_MESH_OLD * p_mesh)
{
  GAME_MESH_DATA *p_data = p_mesh->p_data;
  STATIC_LIGHT *p_slight = p_ber->p_slight;
  DYN_LIGHT *p_dlight = p_ber->p_dlight;
  void *p_light_dyn[MAX_FLARE_SVETEL * 2];
  void *p_light_stat[MAX_FLARE_SVETEL * 2];
  int lightakt = 0;
  int slightnum = 0;
  int dlightnum = 0;
  int i;
  int kflag = p_data->kflag;

  if (kflag & (KONT_POHYB | KONT_VIDITELNY_ZMENA) || p_ber->dl_recalc
    || p_ber->dl_new) {

    // Zapocitam staticke svetla pri pohybu nebo je prekopiruju
    if (p_data->kflag & KONT_POHYB) {
      for (lightakt = 0, i = 0; i < p_ber->slightnum; i++) {
        p_slight = p_ber->p_slight + i;
        if (p_slight->flag2 & SL2_DYNAMIC
          && p_slight->dosah > obb_vzdal_bod(&p_mesh->obb_world,
            &p_slight->p))
          p_light_stat[lightakt++] = p_slight;
      }
      slightnum = lightakt;
    }
    else {
      slightnum = p_data->slightnum;
    }

    // Zaradim dynamicke svetla
    for (lightakt = 0, i = 0; i < p_ber->dl_lightnum; i++) {
      p_dlight = p_ber->p_dlight + i;
      if (p_dlight->akt
        && p_dlight->dosah > obb_vzdal_bod(&p_mesh->obb_world,
          &p_dlight->tp)) {
        p_light_dyn[lightakt++] = p_dlight;
      }
    }
    dlightnum = lightakt;

    i = dlightnum + slightnum;

    if (!i) {
      null_free((void **) &p_data->p_light);
      p_data->lightakt = p_data->lightmax = p_data->slightnum = 0;
    }
    else {
      if (i >= p_data->lightmax) {
        if (!(p_mesh->p_data->kflag & KONT_POHYB))
          memcpy(p_light_stat, p_data->p_light,
            slightnum * sizeof(p_data->p_light[0]));

        null_free((void **) &p_data->p_light);
        i += (i >> 1) + 10;
        p_data->p_light = (void **) mmalloc(sizeof(p_data->p_light[0]) * i);
        p_data->lightmax = i;
        memcpy(p_data->p_light, p_light_stat,
          slightnum * sizeof(p_data->p_light[0]));
        memcpy(p_data->p_light + slightnum, p_light_dyn,
          dlightnum * sizeof(p_data->p_light[0]));
      }
      else {
        if (p_data->kflag & KONT_POHYB)
          memcpy(p_data->p_light, p_light_stat,
            slightnum * sizeof(p_data->p_light[0]));

        memcpy(p_data->p_light + slightnum, p_light_dyn,
          dlightnum * sizeof(p_data->p_light[0]));
      }
      p_data->lightakt = slightnum + dlightnum;
      p_data->slightnum = slightnum;
    }

    return (TRUE);
  }
  else {
    return (FALSE);
  }
}

/* Zjisti jestli se nektery dynamicky svetlo meshe zmenilo 
   a musi se teda prepocitat
*/
inline int dl_mesh_svetlo_update(GAME_MESH_DATA * p_data)
{
  DYN_LIGHT **p_dlight = (DYN_LIGHT **) p_data->p_light;
  dword flags = 0;
  int i;

  for (i = p_data->slightnum; i < p_data->lightakt; i++) {
    flags |= p_dlight[i]->flag;
  }
  return (flags & (SDL_ZMENA_GLOBAL | SDL_ZMENA_LOKAL));
}

/* Pokud je svetlo privazany -> automaticky ho updatuju
*/
int dl_mesh_pripocitej_svetla(G_KONFIG * p_ber, GAME_MESH_OLD * p_mesh,
  int dyn_light, int extra_light, int full_light, int all)
{
  EDIT_MATERIAL **p_mat = p_ber->p_mat;
  GAME_MESH_DATA *p_data = p_mesh->p_data;
  STATIC_LIGHT *p_slight;
  DYN_LIGHT *p_dlight;
  EXTRA_DYN_LIGHT *p_light;
  GLMATRIX *p_m = NULL;
  BOD *p_vertex_pos = NULL;
  BOD *p_vertex_norm = NULL;
  BODRGBA *p_vertex_diff = NULL;
  BODRGBA *p_vertex_diff_mat = NULL;
  BODRGB *p_vertex_spec = NULL;
  BODRGB *p_vertex_spec_mat = NULL;
  BOD p, n, dir, cam;
  int i, v, vnum, dflag, m2flag, j;
  int diffuse, specular, specular_light;
  int diffuse_ray, specular_ray;
  int kflag = p_data->kflag;
  int *p_kflag;
  int keyframe;
  int norm, o, objektu;
  int staticky, bump;
  int beruska = ber_je_mesh_beruska_fast(p_mesh);
  float vzdal, vzdal2, I, lesk = 1.0f;
  float Ar, Ag, Ab;
  float *p_looksqrt_linear_float = p_ber->p_looksqrt_linear_float;
  float *p_looksqrt_quad_float = p_ber->p_looksqrt_quadratic_float;

  m2flag = p_data->m2flag;
  diffuse = m2flag & MAT2_DIFFUSE && p_mesh->p_vertex_diff;
  specular = m2flag & MAT2_SPECULAR;
  if (!diffuse && !specular)
    return (FALSE);

  if (!diffuse || (dyn_light && !(kflag & KONT_POHYB) && !p_ber->dl_new
      && !dl_mesh_svetlo_update(p_data))) {
    dyn_light = FALSE;
  }

  extra_light = extra_light ? p_data->edlightnum : 0;

  specular_light = extra_light && kflag & KONT_DRAW_SPEC;
  keyframe = kflag & KONT_KEYFRAME;
  staticky = kflag & KONT_STATIC;
  vnum = p_mesh->vertexnum;

  if (diffuse) {
    diffuse_ray = m2flag & MAT2_DIFFUSE_RAY;
    specular_ray = m2flag & MAT2_SPECULAR_RAY;

    Ar = p_ber->dl_ambient[0];
    Ag = p_ber->dl_ambient[1];
    Ab = p_ber->dl_ambient[2];
  }
  else {
    diffuse_ray = specular_ray = FALSE;
  }

  if (extra_light) {
    cam.x = p_ber->p_invcam->_41;
    cam.y = p_ber->p_invcam->_42;
    cam.z = p_ber->p_invcam->_43;
  }

  if (!keyframe)
    p_m = &p_mesh->m;

#ifdef DEBUG_MOD
  p_ber->debug.light_meshu++;
#endif

  objektu = p_mesh->objektu;
  p_kflag = (int *) p_mesh->p_kflag;
  for (o = 0; o < objektu; o++) {

    if (all || p_kflag[o] & (KONT_VIDITELNY | KONT_VIDITELNY_ZRC)) {

      v = p_mesh->objektstart[o];
      vnum = v + p_mesh->objektnum[o];

      bump = p_mat[p_mesh->p_mat[o]]->flag2 & MAT2_BUMP;

      if (keyframe)
        p_m = p_mesh->p_key + o;

      p_vertex_pos = p_mesh->p_vertex_pos + v;
      p_vertex_diff = p_mesh->p_vertex_diff + v;
      p_vertex_diff_mat = p_mesh->p_vertex_diff_material + v;

      if (diffuse_ray)
        p_vertex_norm = p_mesh->p_vertex_norm + v;

      if (specular || specular_light) {
        p_vertex_spec = p_mesh->p_vertex_spec + v;
        p_vertex_spec_mat = p_mesh->p_vertex_spec_material + v;
      }

      for (; v < vnum; v++, p_vertex_pos++, p_vertex_norm++) {

        if (full_light) {
          if (p_vertex_diff) {
            if (beruska) {
              p_vertex_diff->r = p_vertex_diff_mat->r;
              p_vertex_diff->g = p_vertex_diff_mat->g;
              p_vertex_diff->b = p_vertex_diff_mat->b;
            }
            else {
              p_vertex_diff->r = 1.0f;
              p_vertex_diff->g = 1.0f;
              p_vertex_diff->b = 1.0f;
            }
          }
          if (p_vertex_spec) {
            p_vertex_spec->r = 0.0f;
            p_vertex_spec->g = 0.0f;
            p_vertex_spec->b = 0.0f;
          }
        }
        else {

          norm = keyframe;
          transformuj_bod_matici_bod(p_vertex_pos, p_m, &p);

          if (diffuse) {
            if (!staticky && diffuse_ray) {
              transformuj_nbod_matici_bod(p_vertex_norm, p_m, &n);

              p_vertex_diff->r = p_vertex_diff_mat->r * Ar;
              p_vertex_diff->g = p_vertex_diff_mat->g * Ag;
              p_vertex_diff->b = p_vertex_diff_mat->b * Ab;
            }
            else {
              p_vertex_diff->r = p_vertex_diff_mat->r;
              p_vertex_diff->g = p_vertex_diff_mat->g;
              p_vertex_diff->b = p_vertex_diff_mat->b;
            }
          }

          if (specular) {
            if (specular_ray) { // defaultni spekularni svetlo bude modifikovano
              vektor_set((BOD *) p_vertex_spec, 0.0f);
            }
            else {              // defaultni spekularni se kopiruje
              *p_vertex_spec = *p_vertex_spec_mat;
            }
          }
          else if (specular_light) {    // defaultni spekularni neexistuje
            vektor_set((BOD *) p_vertex_spec, 0.0f);
          }

          // Update statickych svetel podle materialu
          if (diffuse && !staticky) {
            for (i = 0; i < p_data->slightnum; i++) {
              p_slight = (STATIC_LIGHT *) p_data->p_light[i];
              vzdal = vzdal_bodu_bod(&p_slight->p, &p);
              if (vzdal < p_slight->dosah) {
                if (p_slight->uhel) {
                  vektor_sub(&p, &p_slight->p, &dir);
                  vektor_norm(&dir);
                  if (norm) {
                    vektor_norm(&n);
                    norm = FALSE;
                  }
                  I = vektor_dot_product(&n, &dir);
                  if (I <= 0.0f)
                    continue;
                }
                else {
                  I = 1.0f;
                }
                if (p_slight->utlum) {
                  I *=
                    1.0f / (p_slight->k1 + p_slight->k2 * vzdal +
                    p_slight->k3 * vzdal * vzdal);
                }
                if (diffuse_ray) {
                  p_vertex_diff->r += p_vertex_diff_mat->r * p_slight->r * I;
                  p_vertex_diff->g += p_vertex_diff_mat->g * p_slight->g * I;
                  p_vertex_diff->b += p_vertex_diff_mat->b * p_slight->b * I;
                }
                if (specular_ray) {
                  I *= lesk;
                  p_vertex_spec->r += p_vertex_spec_mat->r * p_slight->r * I;
                  p_vertex_spec->g += p_vertex_spec_mat->g * p_slight->g * I;
                  p_vertex_spec->b += p_vertex_spec_mat->b * p_slight->b * I;
                }

#ifdef DEBUG_MOD
                p_ber->debug.light_vertexu++;
#endif
              }
            }

            // Normalizace barev
            if (diffuse_ray) {
              vektor_norm_up((BOD *) p_vertex_diff);
            }
            if (specular_ray) {
              vektor_norm_up((BOD *) p_vertex_spec);
            }
          }

          // Update jednoduchych dynamickych svetel
          if (dyn_light) {
            for (i = p_data->slightnum; i < p_data->lightakt; i++) {
              p_dlight = (DYN_LIGHT *) p_data->p_light[i];
              dflag = p_dlight->flag;
              vzdal = vzdal_bodu_bod(&p_dlight->tp, &p);
              if (vzdal > p_dlight->dosah)
                continue;
              if (vzdal <= p_dlight->min) {
                if (dflag & SDL_ADD_SUB) {
                  p_vertex_diff->r -= p_dlight->sr;
                  p_vertex_diff->g -= p_dlight->sg;
                  p_vertex_diff->b -= p_dlight->sb;
                  vektor_norm_down((BOD *) p_vertex_diff);
                  p_vertex_diff->r += p_dlight->dr;
                  p_vertex_diff->g += p_dlight->dg;
                  p_vertex_diff->b += p_dlight->db;
                }
                else if (dflag & SDL_SUB) {
                  p_vertex_diff->r -= p_dlight->dr;
                  p_vertex_diff->g -= p_dlight->dg;
                  p_vertex_diff->b -= p_dlight->db;
                }
                else {
                  p_vertex_diff->r += p_dlight->dr;
                  p_vertex_diff->g += p_dlight->dg;
                  p_vertex_diff->b += p_dlight->db;
                }
              }
              else {
                j =
                  ftoi(((vzdal - p_dlight->min) / (p_dlight->max -
                      p_dlight->min)) * 255.0f);
                I =
                  (dflag & SDL_UTLUM_LINEAR) ? p_looksqrt_linear_float[j] :
                  p_looksqrt_quad_float[j];
                if (I < DELTA_MIN)
                  continue;
                if (dflag & SDL_ADD_SUB) {
                  p_vertex_diff->r -= p_dlight->sr * I;
                  p_vertex_diff->g -= p_dlight->sg * I;
                  p_vertex_diff->b -= p_dlight->sb * I;
                  vektor_norm_down((BOD *) p_vertex_diff);
                  p_vertex_diff->r += p_dlight->dr * I;
                  p_vertex_diff->g += p_dlight->dg * I;
                  p_vertex_diff->b += p_dlight->db * I;
                }
                else if (dflag & SDL_SUB) {
                  p_vertex_diff->r -= p_dlight->dr * I;
                  p_vertex_diff->g -= p_dlight->dg * I;
                  p_vertex_diff->b -= p_dlight->db * I;
                }
                else {
                  p_vertex_diff->r += p_dlight->dr * I;
                  p_vertex_diff->g += p_dlight->dg * I;
                  p_vertex_diff->b += p_dlight->db * I;
                }
              }
            }
          }

          // Update extra-svetel - pokud jsou          
          if (extra_light) {
            for (i = 0; i < extra_light; i++) {
              p_light = p_data->p_edlight[i];

              dflag = p_light->flag;

              if (dflag & EDL_ZEMNI_MLHA) {
                vzdal = fabsf(p_light->tp.y - p.y);
                if (vzdal <= p_light->v_start) {
                  p_vertex_spec->r = p_light->sr;
                  p_vertex_spec->g = p_light->sg;
                  p_vertex_spec->b = p_light->sb;
                }
                else if (vzdal < p_light->v_stop) {
                  vzdal2 = vzdal_bodu_bod(&cam, &p);
                  if (vzdal2 < p_light->h_start) {
                    I =
                      1.0f - (vzdal - p_light->v_start) / (p_light->v_stop -
                      p_light->v_start);
                    p_vertex_spec->r = p_light->sr * I;
                    p_vertex_spec->g = p_light->sg * I;
                    p_vertex_spec->b = p_light->sb * I;
                  }
                  else if (vzdal2 > p_light->h_stop) {
                    p_vertex_spec->r = p_light->sr;
                    p_vertex_spec->g = p_light->sg;
                    p_vertex_spec->b = p_light->sb;
                  }
                  else {
                    vzdal =
                      1.0f - (vzdal - p_light->v_start) / (p_light->v_stop -
                      p_light->v_start);
                    vzdal2 =
                      (vzdal2 - p_light->h_start) / (p_light->h_stop -
                      p_light->h_start);
                    I = MAX(vzdal, vzdal2);
                    p_vertex_spec->r = p_light->sr * I;
                    p_vertex_spec->g = p_light->sg * I;
                    p_vertex_spec->b = p_light->sb * I;
                  }
                }
              }
              else {
                if (dflag & EDL_DOSAH) {
                  if (dflag & EDL_BODOVE) {
                    vzdal = vzdal_bodu_bod(&p_light->tp, &p);
                  }
                  else if (dflag & (EDL_PLOSNE_X | EDL_PLOSNE_Y |
                      EDL_PLOSNE_Z)) {
                    if (dflag & EDL_PLOSNE_X) {
                      vzdal = fabsf(p_light->tp.x - p.x);
                    }
                    else if (dflag & EDL_PLOSNE_Y) {
                      vzdal = fabsf(p_light->tp.y - p.y);
                    }
                    else {
                      vzdal = fabsf(p_light->tp.z - p.z);
                    }
                  }
                  if (vzdal >= p_light->dosah)
                    continue;

                  if (dflag & EDL_UTLUM_LIN) {
                    vzdal =
                      1.0f - (vzdal) / (p_light->dosah * p_light->utlum);
                    if (vzdal < DELTA_MIN)
                      continue;
                  }
                  else if (dflag & EDL_UTLUM_KVAD) {
                    vzdal =
                      1.0f -
                      (vzdal * vzdal) / (p_light->dosah * p_light->utlum);
                    if (vzdal < DELTA_MIN)
                      continue;
                  }
                  else {
                    vzdal = 1.0f;
                  }
                }
                else {
                  vzdal = 1.0f;
                }

                if (dflag & EDL_GOURAND) {
                  vektor_sub(&p, &p_light->tp, &dir);
                  vektor_norm(&dir);
                  if (norm) {
                    vektor_norm(&n);
                    norm = FALSE;
                  }
                  I = vektor_dot_product(&n, &dir) * vzdal * p_light->gourand;
                  if (I < DELTA_MIN)
                    continue;
                }
                else {
                  I = vzdal;
                }

                // Nastav alfa slozku
                if (dflag & (EDL_ALFA_MOD | EDL_ALFA_SET | EDL_ALFA_ADD |
                    EDL_ALFA_SUB)) {
                  if (dflag & EDL_ALFA_SET) {
                    p_vertex_diff->a = 1.0f - p_light->da * I;
                  }
                  else if (dflag & EDL_ALFA_ADD) {
                    p_vertex_diff->a =
                      p_vertex_diff_mat->a + (1.0f - p_light->da * I);
                  }
                  else if (dflag & EDL_ALFA_MOD) {
                    p_vertex_diff->a =
                      p_vertex_diff_mat->a * (1.0f - p_light->da * I);
                  }
                  else {
                    p_vertex_diff->a =
                      p_vertex_diff_mat->a - (1.0f - p_light->da * I);
                  }
                }

                // Nastav spekularni slozku
                if (dflag & (EDL_SPEC_MOD | EDL_SPEC_SET | EDL_SPEC_ADD |
                    EDL_SPEC_SUB)) {
                  if (dflag & EDL_SPEC_ADD) {
                    p_vertex_spec->r += p_light->sr * I;
                    p_vertex_spec->g += p_light->sg * I;
                    p_vertex_spec->b += p_light->sb * I;
                  }
                  else if (dflag & EDL_SPEC_MOD) {
                    p_vertex_spec->r *= p_light->sr * I;
                    p_vertex_spec->g *= p_light->sg * I;
                    p_vertex_spec->b *= p_light->sb * I;
                  }
                  else if (dflag & EDL_SPEC_SUB) {
                    p_vertex_spec->r -= p_light->sr * I;
                    p_vertex_spec->g -= p_light->sg * I;
                    p_vertex_spec->b -= p_light->sb * I;
                  }
                  else {
                    p_vertex_spec->r = p_light->sr * I;
                    p_vertex_spec->g = p_light->sg * I;
                    p_vertex_spec->b = p_light->sb * I;
                  }
                }
              }
            }
          }
        }

        if (bump && diffuse) {
          vektor_norm_bump((BOD *) p_vertex_diff);
        }

        // Posun na dalsi vertex        
        p_vertex_diff++;
        p_vertex_diff_mat++;

        if (specular || specular_light) {
          p_vertex_spec++;
          p_vertex_spec_mat++;
        }
      }
    }
  }

  p_mesh->p_data->k2flag |= KONT2_UPDATE_DIFF;
  if (specular || specular_light)
    p_mesh->p_data->k2flag |= KONT2_UPDATE_SPEC;

  return (TRUE);
}

void dl_reset_svetel(G_KONFIG * p_ber)
{
  DYN_LIGHT *p_light;
  EXTRA_DYN_LIGHT *p_edlight;
  int i;

  for (i = 0; i < p_ber->dl_lightnum; i++) {
    p_light = p_ber->p_dlight + i;
    if (p_light->akt) {
      p_light->flag &= ~(SDL_ZMENA_GLOBAL | SDL_ZMENA_LOKAL);
    }
  }
  for (i = 0; i < p_ber->edl_lightnum; i++) {
    p_edlight = p_ber->p_edlight + i;
    if (p_edlight->akt) {
      p_edlight->flag &= ~(EDL_ZMENA_GLOBAL | EDL_ZMENA_LOKAL);
    }
  }
}

inline void dl_updatuj_mesh(G_KONFIG * p_ber, GAME_MESH_OLD * p_mesh,
  int full_light, int all)
{
  int v1, v2;

  v1 = dl_mesh_pridej_svetla(p_ber, p_mesh);
  v2 = edl_mesh_pridej_svetla(p_ber, p_mesh);
  if (v1 || v2) {
    dl_mesh_pripocitej_svetla(p_ber, p_mesh, v1, v2, full_light, all);
    p_mesh->p_data->kflag |= KONT_POHYB_ZRUS;
  }
}

inline void dl_updatuj_seznam_meshu(G_KONFIG * p_ber, int all)
{
  GAME_MESH_OLD *p_mesh;
  int flag = (p_ber->conf_dyn_light_prvky
    || p_ber->conf_dyn_light_beruska) ? KONT_PRVEK : FALSE;
  int full = p_ber->conf_full_light;
  int m;

  if (all) {
    for (m = 0; m < p_ber->meshnum; m++) {
      p_mesh = p_ber->p_mesh[m];
      if (p_mesh) {
        dl_updatuj_mesh(p_ber, p_mesh,
          full ? ber_je_mesh_prvek_fast(p_mesh) : FALSE, all);
      }
    }
  }
  else {
    ber_mesh_render_list_reset(p_ber);
    while (p_mesh = ber_mesh_render_list_next_flag(p_ber, flag, FALSE)) {
      if (!p_ber->conf_dyn_light_beruska || (p_ber->conf_dyn_light_beruska
          && ber_je_mesh_beruska_fast(p_mesh)))
        dl_updatuj_mesh(p_ber, p_mesh,
          full ? ber_je_mesh_prvek_fast(p_mesh) : FALSE, FALSE);
    }
  }
}

// Vzdalenost od ctvercove obalky
int dl_poly_pridej_svetla(G_KONFIG * p_ber, EDIT_MESH_POLY * p_poly)
{
  DYN_LIGHT *p_dlight = p_ber->p_dlight;
  void *p_light_dyn[MAX_FLARE_SVETEL * 2];
  int lightakt = 0;
  int i;

  if (!p_ber->dl_recalc && !p_ber->dl_new)
    return (FALSE);

  for (lightakt = 0, i = 0; i < p_ber->dl_lightnum; i++) {
    p_dlight = p_ber->p_dlight + i;

    if (p_dlight->akt
      && p_dlight->dosah > obb_vzdal_bod(&p_poly->obb, &p_dlight->tp)) {
      p_light_dyn[lightakt++] = p_dlight;
    }
  }
  p_poly->lightakt = i = lightakt;

  if (!i) {
    null_free((void **) &p_poly->p_dlight);
    p_poly->lightmax = 0;
  }
  else {
    if (i >= p_poly->lightmax) {
      null_free((void **) &p_poly->p_dlight);
      i += (i >> 1) + 10;
      p_poly->p_dlight = (void **) mmalloc(sizeof(p_poly->p_dlight[0]) * i);
      p_poly->lightmax = i;
    }
    memcpy(p_poly->p_dlight, p_light_dyn, i * sizeof(p_poly->p_dlight[0]));
  }
  return (TRUE);
}

// Vzdalenost od ctvercove obalky
int edl_poly_pridej_svetla(G_KONFIG * p_ber, EDIT_MESH_POLY * p_poly)
{
  EXTRA_DYN_LIGHT *p_light;
  EXTRA_DYN_LIGHT *p_light_dyn[MAX_FLARE_SVETEL * 2];
  float vzdal;
  int kflag = p_poly->kflag;
  dword flags = 0;
  int flag;
  int lightnum;
  int i;

  if (!p_ber->edl_recalc && !p_ber->edl_new)
    return (FALSE);

  // Zaradim dynamicke svetla  
  lightnum = 0;
  for (i = 0; i < p_ber->edl_lightnum; i++) {
    p_light = p_ber->p_edlight + i;
    if (p_light->akt) {
      flag = p_light->flag;
      if (!(flag & EDL_SCENA)) {
        if (flag & EDL_MESH_LIGHT) {
          if (kflag & KONT_DRAW_LIGHT && p_poly->top_edlight == i) {
            p_light_dyn[lightnum++] = p_light;
            flags |= p_light->flag;
          }
        }
        else if (flag & EDL_DOSAH) {
          if (flag & EDL_BODOVE) {
            if (p_light->dosah > obb_vzdal_bod(&p_poly->obb, &p_light->tp)) {
              p_light_dyn[lightnum++] = p_light;
              flags |= p_light->flag;
            }
          }
          else if (flag & (EDL_PLOSNE_X | EDL_PLOSNE_Y | EDL_PLOSNE_Z)) {
            if (flag & EDL_PLOSNE_X) {
              vzdal = obb_vzdal_rovina_x(&p_poly->obb, p_light->tp.x);
            }
            else if (flag & EDL_PLOSNE_Y) {
              vzdal = obb_vzdal_rovina_y(&p_poly->obb, p_light->tp.y);
            }
            else {
              vzdal = obb_vzdal_rovina_z(&p_poly->obb, p_light->tp.z);
            }
            if (p_light->dosah > vzdal) {
              p_light_dyn[lightnum++] = p_light;
              flags |= p_light->flag;
            }
          }
        }
      }
      else {
        p_light_dyn[lightnum++] = p_light;
        flags |= p_light->flag;
      }
    }
  }

  if (!lightnum) {
    null_free((void **) &p_poly->p_edlight);
    p_poly->edlightakt = p_poly->edlightmax = 0;
    if (!(kflag & KONT_DRAW_LIGHT)) {
      p_poly->kflag &= ~(KONT_DRAW_SPEC);
      p_poly->kflag &= ~(KONT_DRAW_PRUHL);
    }
  }
  else {
    // Nastav ostrou spekularni barvu
    if (flags & (EDL_SPEC_MOD | EDL_SPEC_SET | EDL_SPEC_ADD | EDL_SPEC_SUB)) {
      p_poly->kflag |= KONT_DRAW_SPEC;
    }

    if (flags & EDL_PRUHL_LIGHT) {
      p_poly->kflag |= KONT_DRAW_PRUHL;
    }

    if (lightnum >= p_poly->edlightmax) {
      null_free((void **) &p_poly->p_edlight);
      i += (lightnum >> 1) + 10;
      p_poly->p_edlight = (void **) mmalloc(sizeof(p_poly->p_edlight[0]) * i);
      p_poly->edlightmax = i;
      memcpy(p_poly->p_edlight, p_light_dyn,
        sizeof(p_poly->p_edlight[0]) * lightnum);
    }
    else {
      memcpy(p_poly->p_edlight, p_light_dyn,
        sizeof(p_poly->p_edlight[0]) * lightnum);
    }
    p_poly->edlightakt = lightnum;
  }

  return (TRUE);
}

inline int dl_poly_svetlo_update(EDIT_MESH_POLY * p_poly)
{
  DYN_LIGHT **p_dlight = (DYN_LIGHT **) p_poly->p_dlight;
  dword flags = 0;
  int i;

  for (i = 0; i < p_poly->lightakt; i++) {
    flags |= p_dlight[i]->flag;
  }
  return (flags & (SDL_ZMENA_GLOBAL | SDL_ZMENA_LOKAL));
}

inline int edl_poly_svetlo_update(EDIT_MESH_POLY * p_poly, int *p_flag)
{
  EXTRA_DYN_LIGHT **p_edlight = (EXTRA_DYN_LIGHT **) p_poly->p_edlight;
  dword flags = 0;
  int i;

  for (i = 0; i < p_poly->edlightakt; i++) {
    flags |= p_edlight[i]->flag;
  }
  *p_flag = flags;
  return (flags & (EDL_ZMENA_GLOBAL | EDL_ZMENA_LOKAL));
}

/* Updatuju dynamicky svetla a extra-svetla
*/
int edl_poly_pripocitej_svetla(G_KONFIG * p_ber, EDIT_MESH_POLY * p_poly)
{
  EXTRA_DYN_LIGHT *p_light;
  BODRGBA *p_vertex_diff;
  BODRGBA *p_vertex_diff_mat;
  BODRGB *p_vertex_spec;
  BODRGB *p_vertex_spec_mat;
  BOD *p, *n, dir, cam;
  int i, v, vnum, dflag;
  int specular, specular_light;
  int extra_light;
  int lflag, diffuse, update;
  float vzdal, vzdal2, I;

  update = edl_poly_svetlo_update(p_poly, &lflag);
  if (!p_ber->edl_new && !update) {
    return (FALSE);
  }

  diffuse =
    lflag & (EDL_ALFA_MOD | EDL_ALFA_SET | EDL_ALFA_ADD | EDL_ALFA_SUB);
  specular =
    lflag & (EDL_SPEC_MOD | EDL_SPEC_SET | EDL_SPEC_ADD | EDL_SPEC_SUB);
  specular_light = p_poly->kflag & KONT_DRAW_SPEC;

  if (lflag & EDL_ZEMNI_MLHA) {
    cam.x = p_ber->p_invcam->_41;
    cam.y = p_ber->p_invcam->_42;
    cam.z = p_ber->p_invcam->_43;
  }

#ifdef DEBUG_MOD
  p_ber->debug.light_meshu++;
#endif
  extra_light = p_poly->edlightakt;

  vnum = p_poly->facenum;
  for (v = 0; v < vnum; v++) {
    p = (BOD *) & p_poly->p_koord[v].x;
    n = (BOD *) & p_poly->p_koord[v].nx;

    if (diffuse) {              // je aktivni diff-operace
      p_vertex_diff = (BODRGBA *) & p_poly->p_koord[v].dr;
      p_vertex_diff_mat = (BODRGBA *) & p_poly->p_koord[v].mdr;
    }
    if (specular) {             // je aktivni spec-operace -> loaduju defaultni specy
      p_vertex_spec = (BODRGB *) & p_poly->p_koord[v].sr;
      p_vertex_spec_mat = (BODRGB *) & p_poly->p_koord[v].msr;
      *p_vertex_spec = *p_vertex_spec_mat;
    }
    else if (specular_light) {  // defaultni spekularni neexistuje
      p_vertex_spec = (BODRGB *) & p_poly->p_koord[v].sr;
      p_vertex_spec->r = 0.0f;
      p_vertex_spec->g = 0.0f;
      p_vertex_spec->b = 0.0f;
    }

    /* Update extra-svetel
     */
    for (i = 0; i < extra_light; i++) {
      p_light = (EXTRA_DYN_LIGHT *) p_poly->p_edlight[i];
      dflag = p_light->flag;

      if (dflag & EDL_ZEMNI_MLHA) {
        vzdal = fabsf(p_light->tp.y - p->y);
        if (vzdal <= p_light->v_start) {
          p_vertex_spec->r = p_light->sr;
          p_vertex_spec->g = p_light->sg;
          p_vertex_spec->b = p_light->sb;
        }
        else if (vzdal <= p_light->v_stop) {
          vzdal2 = vzdal_bodu_bod(&cam, p);
          if (vzdal2 < p_light->h_start) {
            I =
              1.0f - (vzdal - p_light->v_start) / (p_light->v_stop -
              p_light->v_start);
            p_vertex_spec->r = p_light->sr * I;
            p_vertex_spec->g = p_light->sg * I;
            p_vertex_spec->b = p_light->sb * I;
          }
          else if (vzdal2 > p_light->h_stop) {
            p_vertex_spec->r = p_light->sr;
            p_vertex_spec->g = p_light->sg;
            p_vertex_spec->b = p_light->sb;
          }
          else {
            vzdal =
              1.0f - (vzdal - p_light->v_start) / (p_light->v_stop -
              p_light->v_start);
            vzdal2 =
              (vzdal2 - p_light->h_start) / (p_light->h_stop -
              p_light->h_start);
            I = MAX(vzdal, vzdal2);
            p_vertex_spec->r = p_light->sr * I;
            p_vertex_spec->g = p_light->sg * I;
            p_vertex_spec->b = p_light->sb * I;
          }
        }
      }
      else {
        if (dflag & EDL_DOSAH) {
          if (dflag & EDL_BODOVE) {
            vzdal = vzdal_bodu_bod(&p_light->tp, p);
          }
          else if (dflag & (EDL_PLOSNE_X | EDL_PLOSNE_Y | EDL_PLOSNE_Z)) {
            if (dflag & EDL_PLOSNE_X) {
              vzdal = fabsf(p_light->tp.x - p->x);
            }
            else if (dflag & EDL_PLOSNE_Y) {
              vzdal = fabsf(p_light->tp.y - p->y);
            }
            else {
              vzdal = fabsf(p_light->tp.z - p->z);
            }
          }
          if (vzdal >= p_light->dosah)
            continue;

          if (dflag & EDL_UTLUM_LIN) {
            vzdal = 1.0f - (vzdal) / (p_light->dosah * p_light->utlum);
            if (vzdal < DELTA_MIN)
              continue;
          }
          else if (dflag & EDL_UTLUM_KVAD) {
            vzdal =
              1.0f - (vzdal * vzdal) / (p_light->dosah * p_light->utlum);
            if (vzdal < DELTA_MIN)
              continue;
          }
          else {
            vzdal = 1.0f;
          }
        }
        else {
          vzdal = 1.0f;
        }

        if (dflag & EDL_GOURAND) {
          vektor_sub(p, &p_light->tp, &dir);
          vektor_norm(&dir);
          I = vektor_dot_product(n, &dir) * vzdal * p_light->gourand;
          if (I < DELTA_MIN)
            continue;
        }
        else {
          I = vzdal;
        }

        // Nastav alfa slozku
        if (dflag & (EDL_ALFA_MOD | EDL_ALFA_SET | EDL_ALFA_ADD |
            EDL_ALFA_SUB)) {
          if (dflag & EDL_ALFA_SET) {
            p_vertex_diff->a = 1.0f - p_light->da * I;
          }
          else if (dflag & EDL_ALFA_ADD) {
            p_vertex_diff->a = p_vertex_diff_mat->a + 1.0f - p_light->da * I;
          }
          else if (dflag & EDL_ALFA_MOD) {
            p_vertex_diff->a =
              p_vertex_diff_mat->a * (1.0f - p_light->da * I);
          }
          else {
            p_vertex_diff->a =
              p_vertex_diff_mat->a - (1.0f - p_light->da * I);
          }
        }

        // Nastav spekularni slozku
        if (dflag & (EDL_SPEC_MOD | EDL_SPEC_SET | EDL_SPEC_ADD |
            EDL_SPEC_SUB)) {
          if (dflag & EDL_SPEC_ADD) {
            p_vertex_spec->r += p_light->sr * I;
            p_vertex_spec->g += p_light->sg * I;
            p_vertex_spec->b += p_light->sb * I;
          }
          else if (dflag & EDL_SPEC_MOD) {
            p_vertex_spec->r *= p_light->sr * I;
            p_vertex_spec->g *= p_light->sg * I;
            p_vertex_spec->b *= p_light->sb * I;
          }
          else if (dflag & EDL_SPEC_SUB) {
            p_vertex_spec->r -= p_light->sr * I;
            p_vertex_spec->g -= p_light->sg * I;
            p_vertex_spec->b -= p_light->sb * I;
          }
          else {
            p_vertex_spec->r = p_light->sr * I;
            p_vertex_spec->g = p_light->sg * I;
            p_vertex_spec->b = p_light->sb * I;
          }
        }
      }
    }
  }
  p_poly->kflag |= KONT_UPLOAD;
  return (TRUE);
}


/*
  - nahodim lightmapu a provedu update vsech regionu
  - U dynamickeho svetla udelam nakvantovani celeho jeho dosahu
  po n krocich pro vsechny barvy po kroku vzdal nadruhou max 255 kombinaci)
*/

/*
  #define MAX_LOOKF 255.0f
  #define MAX_LOOK  255
*/

#define MIN_LOOKF 0.0f
#define MIN_LOOK  0

// full_light - plne osvetleni
inline int sdl_poly_pripocitej_svetla_face(G_KONFIG * p_ber,
  EDIT_MESH_POLY * p_poly, LIGHTMAP_FACE * p_lf, int full_light)
{
  int lnum = p_poly->lightakt;
  dword *p_data;
  int ddx;
  float suf1, suf2, svf1, svf2;
  float vzdal, dosah2d, look_max, max_vzdal;
  BOD s;
  DYN_LIGHT *p_dlight;
  int l, sv, su, u;
  dword r, g, b;
  dword sr, sg, sb;
  int nup, nul;                 // pocet pravych/levych pixelu
  int sup, sul;                 // pocet pravych/levych pixelu
  int nvp, nvl;                 // pocet pravych/levych pixelu
  int svp, svl;                 // pocet pravych/levych pixelu
  int kup, skup;                // fixed pricitaci krok prave/leve strany
  int kul, skul;                // fixed start prave/leve strany
  int kvp, skvp;                // fixed pricitaci krok prave/leve strany
  int kvl, skvl;                // fixed start prave/leve strany
  int fvzdal;                   // fixed-point 8:24 korekcni hodnota v look-up tabulce
  int min_u, max_u;
  int min_v, max_v;
  int zal, nusu, nvsv;
  int *p_looksqrt;
  int nu, nv;


  if (!full_light) {
    for (l = 0, u = 0; l < lnum; l++) {
      p_dlight = (DYN_LIGHT *) p_poly->p_dlight[l];
      vzdal = vzdal_bodu_a_roviny(&p_dlight->tp, &p_lf->n);
      if (p_dlight->aup = (vzdal < p_dlight->dosah)) {
        p_dlight->avzdal = vzdal;
        u++;
      }
    }

    if (!u && !p_lf->last_u && !p_ber->conf_full_light_last) {
      p_lf->upload = FALSE;
      return (FALSE);
    }
  }

  // kopie ze zalohy do vysledku  
  bmp_kopiruj(p_lf->p_zal, p_lf->p_bmp);

  if (full_light) {
    bmp_smaz(p_lf->p_bmp, 0xffffff);
    u = 0;
  }

  p_lf->last_u = u;
  if (!u) {
    p_lf->upload = TRUE;
    return (TRUE);
  }

  ddx = p_lf->p_bmp->x;
  p_data = p_lf->p_bmp->data;

  // udelam to pro kazde svetlo extra zvlaste
  for (l = 0; l < lnum; l++) {
    p_dlight = (DYN_LIGHT *) p_poly->p_dlight[l];
    if (!p_dlight->aup)
      continue;

    p_looksqrt = p_dlight->flag & SDL_UTLUM_KVADRATIC ?
      p_ber->p_looksqrt_quadratic : p_ber->p_looksqrt_linear;

    // 3D bod na rovine
    s.x = p_dlight->tp.x - p_lf->n.x * p_dlight->avzdal;
    s.y = p_dlight->tp.y - p_lf->n.y * p_dlight->avzdal;
    s.z = p_dlight->tp.z - p_lf->n.z * p_dlight->avzdal;

    // 2D bod ve svetle
    nu = p_lf->nu + 2;
    nv = p_lf->nv + 2;

    su =
      ftoi(vzdal_bodu_a_roviny_nabs(&s,
        &p_lf->rv) * ((float) nu / p_lf->ddu));
    sv =
      ftoi(vzdal_bodu_a_roviny_nabs(&s,
        &p_lf->ru) * ((float) nv / p_lf->ddv));

    suf1 = vzdal_bodu_a_primky(&s, &p_lf->s0, &p_lf->s1);
    svf1 = vzdal_bodu_a_primky(&s, &p_lf->s0, &p_lf->s2);

    suf2 = vzdal_bodu_a_primky(&s, &p_lf->s2, &p_lf->s3);
    svf2 = vzdal_bodu_a_primky(&s, &p_lf->s1, &p_lf->s3);

    r = (byte) ftoi(255.0f * p_dlight->dr);
    g = (byte) ftoi(255.0f * p_dlight->dg);
    b = (byte) ftoi(255.0f * p_dlight->db);
    if (p_dlight->flag & SDL_ADD_SUB) {
      sr = (byte) ftoi(255.0f * p_dlight->sr);
      sg = (byte) ftoi(255.0f * p_dlight->sg);
      sb = (byte) ftoi(255.0f * p_dlight->sb);
    }

    look_max = p_dlight->look_max;
    if (p_dlight->avzdal >= p_dlight->min) {
      fvzdal =
        0x01000000 - ftoi((float) 0x01000000 * ((p_dlight->avzdal -
            p_dlight->min) / p_dlight->max));
    }
    else {
      fvzdal = 0x01000000;
    }

    dosah2d =
      sqrtf(p_dlight->dosah * p_dlight->dosah -
      p_dlight->avzdal * p_dlight->avzdal);
    nusu = nu - su;
    nvsv = nv - sv;

    // Strana U lightmapy
    if (su <= 0) {
      if (suf1 > dosah2d)
        continue;

      min_u = ftoi(look_max * suf1 / dosah2d);

      if (suf2 > dosah2d) {
        max_u = ftoi(look_max);
        nup = ftoi(((dosah2d - suf1) / (suf2 - suf1)) * nu);
      }
      else {
        nup = nu;
        max_u = ftoi(look_max * suf2 / dosah2d);
      }

      nul = sul = sup = 0;

      if (nup > 0) {
        kup = ((max_u - min_u) << 16) / nup;
        skup = (min_u << 16) - kup;
      }
    }
    else if (su >= nu - 1) {

      if (suf2 > dosah2d)
        continue;

      min_u = ftoi(look_max * suf2 / dosah2d);

      if (suf1 > dosah2d) {
        max_u = ftoi(look_max);
        nul = ftoi(((dosah2d - suf2) / (suf1 - suf2)) * nu);
      }
      else {
        nul = nu;
        max_u = ftoi(look_max * suf1 / dosah2d);
      }

      nup = sup = 0;
      sul = nu - 1;

      if (nul > 0) {
        kul = ((max_u - min_u) << 16) / nul;
        skul = (min_u << 16) - kul;
      }

    }
    else {

      min_u = MIN_LOOK;

      max_vzdal = (suf2 > suf1) ? suf2 : suf1;
      max_u =
        (max_vzdal >
        dosah2d) ? ftoi(look_max) : ftoi(look_max * max_vzdal / dosah2d);

      // Spocitam pravou stranu -> sf2
      nup = (suf2 > dosah2d) ? ftoi((dosah2d / suf2) * nusu) : nusu;

      // Leva strana -> su
      nul = (suf1 > dosah2d) ? ftoi((dosah2d / suf1) * su) : su;
/*      
      if(--nul < 0)
        nul = 0;
*/

      sup = su;                 // Start prave strany
      sul = su - 1;             // Start leve strany

      if (nup > 0 && nul > 0)
        kup = kul = (nul > nup) ? (max_u << 16) / nul : (max_u << 16) / nup;
      else if (nul > nup && nul > 0)
        kup = kul = (max_u << 16) / nul;
      else if (nup > 0)
        kup = kul = (max_u << 16) / nup;

      skul = skup = -kup;
    }

    /*
       Strana V lightmapy
     */

    if (sv <= 0) {
      if (svf1 > dosah2d)
        continue;

      min_v = ftoi(look_max * svf1 / dosah2d);

      if (svf2 > dosah2d) {
        max_v = ftoi(look_max);
        nvp = ftoi(((dosah2d - svf1) / (svf2 - svf1)) * nv);
      }
      else {
        nvp = nv;
        max_v = ftoi(look_max * svf2 / dosah2d);
      }

      nvl = svl = svp = 0;

      if (nvp > 0) {
        kvp = ((max_v - min_v) << 16) / nvp;
        skvp = (min_v << 16) - kvp;
      }
    }
    else if (sv >= nv - 1) {

      if (svf2 > dosah2d)
        continue;

      min_v = ftoi(look_max * svf2 / dosah2d);

      if (svf1 > dosah2d) {
        max_v = ftoi(look_max);
        nvl = ftoi(((dosah2d - svf2) / (svf1 - svf2)) * nv);
      }
      else {
        nvl = nv;
        max_v = ftoi(look_max * svf1 / dosah2d);
      }

      nvp = svp = 0;
      svl = nv - 1;

      if (nvl > 0) {
        kvl = ((max_v - min_v) << 16) / nvl;
        skvl = (min_v << 16) - kvl;
      }

    }
    else {

      min_v = MIN_LOOK;

      max_vzdal = (svf2 > svf1) ? svf2 : svf1;
      max_v =
        (max_vzdal >
        dosah2d) ? ftoi(look_max) : ftoi(look_max * max_vzdal / dosah2d);

      // Spocitam pravou stranu -> sf2
      nvp = (svf2 > dosah2d) ? ftoi((dosah2d / svf2) * nvsv) : nvsv;

      // Leva strana -> su
      nvl = (svf1 > dosah2d) ? ftoi((dosah2d / svf1) * sv) : sv;
/*      
      if(--nvl < 0)
        nvl = 0;
*/

      svp = sv;                 // Start prave strany
      svl = sv - 1;             // Start leve strany

      if (nvp > 0 && nvl > 0)
        kvp = kvl = (nvl > nvp) ? (max_v << 16) / nvl : (max_v << 16) / nvp;
      else if (nvl > nvp && nvl > 0)
        kvp = kvl = (max_v << 16) / nvl;
      else if (nvp > 0)
        kvp = kvl = (max_v << 16) / nvp;

      skvl = skvp = -kvp;
    }


    // pokud je index po priteni mensi nez 0 -> vezmu maximum !!!
    // pokud je vetsi nez 255 -> kaslu na to    

    // Prvne spocitam prave sloupce v lightmape
    // potom leve sloupce    
/*
    if(p_dlight->flag&SDL_ADD_SUB) {
    __asm  {
      
      pushfd
      pushad

      // Pocet pocitanych radku
      mov  ecx, nvp    // pocet radku smerem nahoru
      test ecx, ecx
      jz   __prava_strana_ms
           
    ____dalsi_radek_prava_strana_ms:

      mov  edi, p_data
      mov  eax, svp
      mov  edx, nu
      add  eax, ecx
      dec  eax
      mul  edx
      shl  eax, 2
      add  edi, eax     // edi -> mam cilovy radek v lightmape

      mov  eax, ecx
      mov  edx, kvp
      mul  edx
      
      add  eax, skvp
      shr  eax, 16       // vypoctu radku tabulky
      shl  eax, 10       // krat sirka tabulky * 4
      
      mov  ebx, p_looksqrt
      add  ebx, eax
      mov  zal, ebx      // cilovy radk v look-up

      push ecx
      call __spocitej_radek_ms
      pop  ecx
            
      dec  ecx
      jnz  ____dalsi_radek_prava_strana_ms


      // udelam levou stranu
    __prava_strana_ms:
    
      mov  ecx, nvl    // pocet radku smerem nahoru      
      test ecx, ecx
      jz   __konec_vseho_ms
           
    ____dalsi_radek_leva_strana_ms: // prvne udelam levou stranu a potom pravou

      mov  edi, p_data
      mov  eax, svl
      mov  edx, nu
      sub  eax, ecx
      inc  eax
      mul  edx
      shl  eax, 2
      add  edi, eax     // edi -> mam cilovy radek v lightmape

      mov  eax, ecx
      mov  edx, kvl
      mul  edx

      add  eax, skvl
      shr  eax, 16
      shl  eax, 10
      
      mov  ebx, p_looksqrt
      add  ebx, eax
      mov  zal, ebx                  

      push ecx
      call __spocitej_radek_ms
      pop  ecx
            
      dec  ecx
      jnz  ____dalsi_radek_leva_strana_ms

      jmp  __konec_vseho_ms
*/
    /* **************************************************
       Vypocty scan_line
     */
/*
// edi -> zacatek radku cilove ligthmapy
// zal -> zacatek radku look-up tabulky
    __spocitej_radek_ms:
      // spocitam jeden radek

      push edi
    
      cmp  nup,0
      jz   __radek_dozadu_ms  // pravy radek neexistuje -> preskoc ho
      
      mov  eax,sup
      shl  eax,2
      add  edi,eax     // prictu sup -> zacatek v tabulce

      mov  ecx,nup     // VV // cx -> pocet opakovani - pocet radku ligthmapy
      mov  edx,kup     // krok v look-up tabulce (fixed)
      mov  ebx,skup    // start bod v look-up tabulce (fixed)
                
    sloupec_dopredu_ms:   // prava strana
            
      add  ebx,edx     // ebx -> aktualni fixed point bod
                       // edx -> fixed point krok
                       // cx -> pocet kroku
                       // edi -> kam zapisuju
      mov  eax,ebx     // eax je tmp registr
      shr  eax,16
      shl  eax,2

      mov  esi,zal
      add  esi,eax
      
      push edx

      mov  eax,[esi]   // nactu intenzitu rgb barev
      mul  fvzdal      // vynasobim fixed-point koeficientem a-vzdal
      shl  edx,8
      shr  eax,24
      or   edx,eax
      mov  esi,edx     // schovam si fixed-point hodnotu
      
      mov  eax,r       // add-mod
      mul  esi
      shr  eax,24
      mov  dl,[edi]
      add  dl,al
      jnc  __neni_prenos_r1_msa
      mov  dl,0xff
    __neni_prenos_r1_msa:
      push edx
      mov  eax,sr      // sub-mod
      mul  esi
      pop  edx
      shr  eax,24
      sub  dl,al
      ja   __neni_prenos_r1_ms
      mov  dl,0x0
    __neni_prenos_r1_ms:
      mov  [edi],dl      
      inc  edi

      mov  eax,g
      mul  esi
      shr  eax,24
      mov  dl,[edi]
      add  dl,al
      jnc  __neni_prenos_g1_msa
      mov  dl,0xff
    __neni_prenos_g1_msa:
      push edx
      mov  eax,sg
      mul  esi
      pop  edx
      shr  eax,24    
      sub  dl,al
      ja   __neni_prenos_g1_ms
      mov  dl,0x0
    __neni_prenos_g1_ms:
      mov  [edi],dl
      inc  edi

      mov  eax,b
      mul  esi
      shr  eax,24
      mov  dl,[edi]
      add  dl,al
      jnc  __neni_prenos_b1_msa
      mov  dl,0xff
    __neni_prenos_b1_msa:
      push edx
      mov  eax,sb
      mul  esi
      pop  edx
      shr  eax,24
      sub  dl,al
      ja   __neni_prenos_b1_ms
      mov  dl,0x0
    __neni_prenos_b1_ms:
      mov  [edi],dl
      inc  edi

      inc  edi

      pop  edx

      dec  cx
      jnz  short sloupec_dopredu_ms
      
    // Frcime nazpatek -> opacne

    __radek_dozadu_ms:
      pop  edi         // vyber puvodni zacatek      

      cmp  nul,0
      jz   __radek_konec_ms  // levy radek neexistuje -> preskoc ho
      
      mov  eax,sul
      shl  eax,2
      add  edi,eax

      mov  ecx,nul     //VV// cx -> pocet opakovani - pocet radku ligthmapy
      mov  edx,kul     // krok v look-up tabulce (fixed)
      mov  ebx,skul    // start bod v look-up tabulce (fixed)      
       
    sloupec_dozadu_ms:   // prava strana
            
      add  ebx,edx     // ebx -> aktualni fixed point bod
                       // edx -> fixed point krok
                       // cx -> pocet kroku
                       // edi -> kam zapisuju
      mov  eax,ebx     // eax je tmp registr
      shr  eax,16
      shl  eax,2

      mov  esi,zal
      add  esi,eax
      
      push edx

      mov  eax,[esi]   // nactu intenzitu rgb barev
      mul  fvzdal
      shl  edx,8
      shr  eax,24
      or   edx,eax
      mov  esi,edx     // schovam si fixed-point hodnotu

      
      mov  eax,r
      mul  esi
      shr  eax,24      
      mov  dl,[edi]
      add  dl,al
      jnc  __neni_prenos_r2_msa
      mov  dl,0xff
    __neni_prenos_r2_msa:
      push edx
      mov  eax,sr
      mul  esi
      pop  edx
      shr  eax,24
      sub  dl,al
      ja   __neni_prenos_r2_ms
      mov  dl,0x0
    __neni_prenos_r2_ms:
      mov  [edi],dl
      inc  edi

      mov  eax,g
      mul  esi
      shr  eax,24
      mov  dl,[edi]
      add  dl,al
      jnc  __neni_prenos_g2_msa
      mov  dl,0xff
    __neni_prenos_g2_msa:
      push edx
      mov  eax,sg
      mul  esi
      pop  edx
      shr  eax,24
      sub  dl,al
      ja   __neni_prenos_g2_ms
      mov  dl,0x0
    __neni_prenos_g2_ms:
      mov  [edi],dl
      inc  edi

      mov  eax,s
      mul  esi
      shr  eax,24
      mov  dl,[edi]
      sub  dl,al
      jnc  __neni_prenos_b2_msa
      mov  dl,0xff
    __neni_prenos_b2_msa:
      push edx
      mov  eax,sb
      mul  esi
      pop  edx
      shr  eax,24
      sub  dl,al
      ja  __neni_prenos_b2_ms
      mov  dl,0x0
    __neni_prenos_b2_ms:
      mov  [edi],dl
      inc  edi

      sub  edi,7

      pop  edx

      dec  cx
      jnz  short sloupec_dozadu_ms
    
    __radek_konec_ms:
      ret            // vrat se zpet

    __konec_vseho_ms:
      popad
      popfd
    }
    } else if(p_dlight->flag&SDL_SUB) {
    __asm  {
      
      pushfd
      pushad

      // Pocet pocitanych radku
      mov  ecx, nvp    // pocet radku smerem nahoru
      test ecx, ecx
      jz   __prava_strana_m
           
    ____dalsi_radek_prava_strana_m:

      mov  edi, p_data
      mov  eax, svp
      mov  edx, nu
      add  eax, ecx
      dec  eax
      mul  edx
      shl  eax, 2
      add  edi, eax     // edi -> mam cilovy radek v lightmape

      mov  eax, ecx
      mov  edx, kvp
      mul  edx
      
      add  eax, skvp
      shr  eax, 16       // vypoctu radku tabulky
      shl  eax, 10       // krat sirka tabulky * 4
      
      mov  ebx, p_looksqrt
      add  ebx, eax
      mov  zal, ebx      // cilovy radk v look-up

      push ecx
      call __spocitej_radek_m
      pop  ecx
            
      dec  ecx
      jnz  ____dalsi_radek_prava_strana_m


      // udelam levou stranu
    __prava_strana_m:
    
      mov  ecx, nvl    // pocet radku smerem nahoru      
      test ecx, ecx
      jz   __konec_vseho_m
           
    ____dalsi_radek_leva_strana_m: // prvne udelam levou stranu a potom pravou

      mov  edi, p_data
      mov  eax, svl
      mov  edx, nu
      sub  eax, ecx
      inc  eax
      mul  edx
      shl  eax, 2
      add  edi, eax     // edi -> mam cilovy radek v lightmape

      mov  eax, ecx
      mov  edx, kvl
      mul  edx

      add  eax, skvl
      shr  eax, 16
      shl  eax, 10
      
      mov  ebx, p_looksqrt
      add  ebx, eax
      mov  zal, ebx                  

      push ecx
      call __spocitej_radek_m
      pop  ecx
            
      dec  ecx
      jnz  ____dalsi_radek_leva_strana_m

      jmp  __konec_vseho_m
*/
    /* **************************************************
       Vypocty scan_line
     */
/*
// edi -> zacatek radku cilove ligthmapy
// zal -> zacatek radku look-up tabulky
    __spocitej_radek_m:
      // spocitam jeden radek

      push edi
    
      cmp  nup,0
      jz   __radek_dozadu_m  // pravy radek neexistuje -> preskoc ho
      
      mov  eax,sup
      shl  eax,2
      add  edi,eax     // prictu sup -> zacatek v tabulce

      mov  ecx,nup     // VV // cx -> pocet opakovani - pocet radku ligthmapy
      mov  edx,kup     // krok v look-up tabulce (fixed)
      mov  ebx,skup    // start bod v look-up tabulce (fixed)
                
    sloupec_dopredu_m:   // prava strana
            
      add  ebx,edx     // ebx -> aktualni fixed point bod
                       // edx -> fixed point krok
                       // cx -> pocet kroku
                       // edi -> kam zapisuju
      mov  eax,ebx     // eax je tmp registr
      shr  eax,16
      shl  eax,2

      mov  esi,zal
      add  esi,eax
      
      push edx

      mov  eax,[esi]   // nactu intenzitu rgb barev
      mul  fvzdal      // vynasobim fixed-point koeficientem a-vzdal
      shl  edx,8
      shr  eax,24
      or   edx,eax
      mov  esi,edx     // schovam si fixed-point hodnotu
      
      mov  eax,r
      mul  esi
      shr  eax,24
      mov  dl,[edi]
      sub  dl,al
      ja   __neni_prenos_r1_m
      mov  dl,0x0
    __neni_prenos_r1_m:
      mov  [edi],dl      
      inc  edi

      mov  eax,g
      mul  esi
      shr  eax,24
      mov  dl,[edi]
      sub  dl,al
      ja   __neni_prenos_g1_m
      mov  dl,0x0
    __neni_prenos_g1_m:
      mov  [edi],dl
      inc  edi

      mov  eax,b
      mul  esi
      shr  eax,24
      mov  dl,[edi]
      sub  dl,al
      ja   __neni_prenos_b1_m
      mov  dl,0x0
    __neni_prenos_b1_m:
      mov  [edi],dl
      inc  edi

      inc  edi

      pop  edx

      dec  cx
      jnz  short sloupec_dopredu_m
      
    // Frcime nazpatek -> opacne

    __radek_dozadu_m:
      pop  edi         // vyber puvodni zacatek      

      cmp  nul,0
      jz   __radek_konec_m  // levy radek neexistuje -> preskoc ho
      
      mov  eax,sul
      shl  eax,2
      add  edi,eax

      mov  ecx,nul     //VV// cx -> pocet opakovani - pocet radku ligthmapy
      mov  edx,kul     // krok v look-up tabulce (fixed)
      mov  ebx,skul    // start bod v look-up tabulce (fixed)      
       
    sloupec_dozadu_m:   // prava strana
            
      add  ebx,edx     // ebx -> aktualni fixed point bod
                       // edx -> fixed point krok
                       // cx -> pocet kroku
                       // edi -> kam zapisuju
      mov  eax,ebx     // eax je tmp registr
      shr  eax,16
      shl  eax,2

      mov  esi,zal
      add  esi,eax
      
      push edx

      mov  eax,[esi]   // nactu intenzitu rgb barev
      mul  fvzdal
      shl  edx,8
      shr  eax,24
      or   edx,eax
      mov  esi,edx     // schovam si fixed-point hodnotu

      
      mov  eax,r
      mul  esi
      shr  eax,24
      mov  dl,[edi]
      sub  dl,al
      ja   __neni_prenos_r2_m
      mov  dl,0x0
    __neni_prenos_r2_m:
      mov  [edi],dl
      inc  edi

      mov  eax,g
      mul  esi
      shr  eax,24
      mov  dl,[edi]
      sub  dl,al
      ja   __neni_prenos_g2_m
      mov  dl,0x0
    __neni_prenos_g2_m:
      mov  [edi],dl
      inc  edi

      mov  eax,b
      mul  esi
      shr  eax,24
      mov  dl,[edi]
      sub  dl,al
      ja  __neni_prenos_b2_m
      mov  dl,0x0
    __neni_prenos_b2_m:
      mov  [edi],dl
      inc  edi

      sub  edi,7

      pop  edx

      dec  cx
      jnz  short sloupec_dozadu_m
    
    __radek_konec_m:
      ret            // vrat se zpet

    __konec_vseho_m:
      popad
      popfd
    }
    } else {
    __asm  {
      
      pushfd
      pushad

      // Pocet pocitanych radku
      mov  ecx, nvp    // pocet radku smerem nahoru
      test ecx, ecx
      jz   __prava_strana
           
    ____dalsi_radek_prava_strana:

      mov  edi, p_data
      mov  eax, svp
      mov  edx, nu
      add  eax, ecx
      dec  eax
      mul  edx
      shl  eax, 2
      add  edi, eax     // edi -> mam cilovy radek v lightmape

      mov  eax, ecx
      mov  edx, kvp
      mul  edx
      
      add  eax, skvp
      shr  eax, 16       // vypoctu radku tabulky
      shl  eax, 10       // krat sirka tabulky * 4
      
      mov  ebx, p_looksqrt
      add  ebx, eax
      mov  zal, ebx      // cilovy radk v look-up

      push ecx
      call __spocitej_radek
      pop  ecx
            
      dec  ecx
      jnz  ____dalsi_radek_prava_strana


      // udelam levou stranu
    __prava_strana:
    
      mov  ecx, nvl    // pocet radku smerem nahoru      
      test ecx, ecx
      jz   __konec_vseho
           
    ____dalsi_radek_leva_strana: // prvne udelam levou stranu a potom pravou

      mov  edi, p_data
      mov  eax, svl
      mov  edx, nu
      sub  eax, ecx
      inc  eax
      mul  edx
      shl  eax, 2
      add  edi, eax     // edi -> mam cilovy radek v lightmape

      mov  eax, ecx
      mov  edx, kvl
      mul  edx

      add  eax, skvl
      shr  eax, 16
      shl  eax, 10
      
      mov  ebx, p_looksqrt
      add  ebx, eax
      mov  zal, ebx                  

      push ecx
      call __spocitej_radek
      pop  ecx
            
      dec  ecx
      jnz  ____dalsi_radek_leva_strana

      jmp  __konec_vseho
*/
    /* **************************************************
       Vypocty scan_line
     */
/*
// edi -> zacatek radku cilove ligthmapy
// zal -> zacatek radku look-up tabulky
    __spocitej_radek:
      // spocitam jeden radek

      push edi
    
      cmp  nup,0
      jz   __radek_dozadu  // pravy radek neexistuje -> preskoc ho
      
      mov  eax,sup
      shl  eax,2
      add  edi,eax     // prictu sup -> zacatek v tabulce

      mov  ecx,nup     // VV // cx -> pocet opakovani - pocet radku ligthmapy
      mov  edx,kup     // krok v look-up tabulce (fixed)
      mov  ebx,skup    // start bod v look-up tabulce (fixed)
                
    sloupec_dopredu:   // prava strana
            
      add  ebx,edx     // ebx -> aktualni fixed point bod
                       // edx -> fixed point krok
                       // cx -> pocet kroku
                       // edi -> kam zapisuju
      mov  eax,ebx     // eax je tmp registr
      shr  eax,16
      shl  eax,2

      mov  esi,zal
      add  esi,eax
      
      push edx

      mov  eax,[esi]   // nactu intenzitu rgb barev
      mul  fvzdal      // vynasobim fixed-point koeficientem a-vzdal
      shl  edx,8
      shr  eax,24
      or   edx,eax
      mov  esi,edx     // schovam si fixed-point hodnotu
      
      mov  eax,r
      mul  esi
      shr  eax,24
      mov  dl,[edi]
      add  dl,al
      jnc  __neni_prenos_r1
      mov  dl,0xff
    __neni_prenos_r1:
      mov  [edi],dl      
      inc  edi

      mov  eax,g
      mul  esi
      shr  eax,24
      mov  dl,[edi]
      add  dl,al
      jnc  __neni_prenos_g1
      mov  dl,0xff
    __neni_prenos_g1:
      mov  [edi],dl
      inc  edi

      mov  eax,b
      mul  esi
      shr  eax,24
      mov  dl,[edi]
      add  dl,al
      jnc  __neni_prenos_b1
      mov  dl,0xff
    __neni_prenos_b1:
      mov  [edi],dl
      inc  edi

      inc  edi

      pop  edx

      dec  cx
      jnz  short sloupec_dopredu
      
    // Frcime nazpatek -> opacne

    __radek_dozadu:
      pop  edi         // vyber puvodni zacatek      

      cmp  nul,0
      jz   __radek_konec  // levy radek neexistuje -> preskoc ho
      
      mov  eax,sul
      shl  eax,2
      add  edi,eax

      mov  ecx,nul     //VV// cx -> pocet opakovani - pocet radku ligthmapy
      mov  edx,kul     // krok v look-up tabulce (fixed)
      mov  ebx,skul    // start bod v look-up tabulce (fixed)      
       
    sloupec_dozadu:   // prava strana
            
      add  ebx,edx     // ebx -> aktualni fixed point bod
                       // edx -> fixed point krok
                       // cx -> pocet kroku
                       // edi -> kam zapisuju
      mov  eax,ebx     // eax je tmp registr
      shr  eax,16
      shl  eax,2

      mov  esi,zal
      add  esi,eax
      
      push edx

      mov  eax,[esi]   // nactu intenzitu rgb barev
      mul  fvzdal
      shl  edx,8
      shr  eax,24
      or   edx,eax
      mov  esi,edx     // schovam si fixed-point hodnotu

      
      mov  eax,r
      mul  esi
      shr  eax,24
      mov  dl,[edi]
      add  dl,al
      jnc  __neni_prenos_r2
      mov  dl,0xff
    __neni_prenos_r2:
      mov  [edi],dl
      inc  edi

      mov  eax,g
      mul  esi
      shr  eax,24
      mov  dl,[edi]
      add  dl,al
      jnc  __neni_prenos_g2
      mov  dl,0xff
    __neni_prenos_g2:
      mov  [edi],dl
      inc  edi

      mov  eax,b
      mul  esi
      shr  eax,24
      mov  dl,[edi]
      add  dl,al
      jnc  __neni_prenos_b2
      mov  dl,0xff
    __neni_prenos_b2:
      mov  [edi],dl
      inc  edi

      sub  edi,7

      pop  edx

      dec  cx
      jnz  short sloupec_dozadu
    
    __radek_konec:
      ret            // vrat se zpet

    __konec_vseho:
      popad
      popfd
    }
    }  
*/
  }
  p_lf->upload = TRUE;

  return (TRUE);
}

// Pripocita vsechna svetla v seznamu a provede update lightmapy
int sdl_poly_pripocitej_svetla(G_KONFIG * p_ber, EDIT_MESH_POLY * p_poly,
  int full_light)
{
  int f, ret;

  if (!p_ber->dl_recalc && !p_ber->dl_new)
    return (FALSE);
  if (!p_ber->dl_new && !dl_poly_svetlo_update(p_poly))
    return (FALSE);

  // pripocitej k tomuto poly ke kazde face vsechny lightmapy
  for (f = 0, ret = 0; f < p_poly->facenum / 3; f++) {
    ret +=
      sdl_poly_pripocitej_svetla_face(p_ber, p_poly, p_poly->p_ind + f,
      full_light);
  }

  return (ret);
}

// uploaduje lightmapu do akceleratoru
int sdl_poly_updatuj_lightmap(G_KONFIG * p_ber, EDIT_MESH_POLY * p_poly)
{
  LIGHTMAP_FACE *p_lf = p_poly->p_ind;
  int l, i, ln, up, last;
  int uploaded = 0;

  last = 0;
  for (l = 0; l < p_poly->lightnum; l++) {
    ln = last + p_poly->p_lightnum[l] / 3;
    up = 0;

    for (i = last; i < ln; i++)
      up += p_lf[i].upload;

    if (up) {
      glBindTexture(p_poly->p_light[l]->typ, p_poly->p_light[l]->text);
      for (i = last; i < ln; i++) {
        if (p_lf[i].upload) {
          glTexSubImage2D(p_poly->p_light[l]->typ, 0, p_lf[i].u - 1,
            p_lf[i].v - 1, p_lf[i].nu + 2, p_lf[i].nv + 2, GL_RGBA,
            GL_UNSIGNED_BYTE, p_lf[i].p_bmp->data);
          uploaded += p_lf[i].p_bmp->x * p_lf[i].p_bmp->y * 4;
        }
      }
    }

    last += p_poly->p_lightnum[l] / 3;
  }
  return (uploaded);
}

void sdl_save_lightmap(EDIT_MESH_POLY * p_poly, int p)
{
  char pom[200];
  int l;

  chdir("/home/komat");
  for (l = 0; l < p_poly->lightnum; l++) {
    sprintf(pom, "poly%dlight%d.bmp", p, l);
    bmp_uloz(pom, p_poly->p_light[l]->p_bmp);
  }
  for (l = 0; l < p_poly->facenum / 3; l++) {
    sprintf(pom, "poly%dfacelight%d.bmp", p, l);
    bmp_uloz(pom, p_poly->p_ind[l].p_bmp);
  }

}

inline void sdl_updatuj_poly(G_KONFIG * p_ber, EDIT_MESH_POLY * p_poly,
  int full_light)
{
  if (dl_poly_pridej_svetla(p_ber, p_poly)) {
    if (sdl_poly_pripocitej_svetla(p_ber, p_poly, full_light)) {
      sdl_poly_updatuj_lightmap(p_ber, p_poly);
    }
  }
  if (edl_poly_pridej_svetla(p_ber, p_poly))
    edl_poly_pripocitej_svetla(p_ber, p_poly);
}

inline void sdl_updatuj_seznam_poly(G_KONFIG * p_ber, int all)
{
  int flag = (p_ber->conf_dyn_light_prvky) ? KONT_PRVEK : FALSE;
  int full = p_ber->conf_full_light;
  EDIT_MESH_POLY *p_poly;
  int p;

  if (all) {
    for (p = 0; p < p_ber->polynum; p++) {
      p_poly = p_ber->p_poly + p;
      sdl_updatuj_poly(p_ber, p_poly,
        full ? p_poly->kflag & KONT_PRVEK : FALSE);
    }
  }
  else {
    ber_poly_render_list_reset(p_ber);
    while (p_poly = ber_poly_render_list_next_flag(p_ber, flag, FALSE)) {
      sdl_updatuj_poly(p_ber, p_poly,
        full ? p_poly->kflag & KONT_PRVEK : FALSE);
    }
  }
}

void dl_transformuj_svetla(G_KONFIG * p_ber)
{
  GAME_MESH_DATA *p_data;
  GAME_MESH_OLD *p_mesh;
  DYN_LIGHT *p_light;
  EXTRA_DYN_LIGHT *p_edlight;
  int kamera_zmena = p_ber->kamera.zmena || p_ber->kamera.zmena_last;
  int i;


  for (i = 0; i < p_ber->dl_lightnum; i++) {
    p_light = p_ber->p_dlight + i;
    if (p_light->akt && p_light->flag & SDL_MESH) {
      p_data = p_light->p_mesh_data;
      if (p_data->kflag & KONT_POHYB) {
        p_mesh = p_data->p_mesh;
        transformuj_bod_matici_bod(&p_light->np, &p_mesh->m, &p_light->tp);
      }
    }
  }

  for (i = 0; i < p_ber->edl_lightnum; i++) {
    p_edlight = p_ber->p_edlight + i;
    if (p_edlight->akt) {
      if (p_edlight->flag & EDL_MESH) {
        p_data = p_edlight->p_mesh_data;
        if (p_data->kflag & KONT_POHYB) {
          p_mesh = p_data->p_mesh;
          transformuj_bod_matici_bod(&p_edlight->np, &p_mesh->m,
            &p_edlight->tp);
        }
      }
      else if (p_edlight->flag & EDL_CAMERA_POINT && kamera_zmena) {
        p_edlight->tp.x = p_ber->p_invcam->_41;
        p_edlight->tp.y = p_ber->p_invcam->_42;
        p_edlight->tp.z = p_ber->p_invcam->_43;
        p_ber->edl_recalc = TRUE;
        p_edlight->flag |= EDL_ZMENA_GLOBAL;
      }
      if (p_edlight->flag & (EDL_CAMERA_POINT | EDL_ZMENA_CAMERA)
        && kamera_zmena) {
        p_ber->edl_recalc = TRUE;
        p_edlight->flag |= EDL_ZMENA_GLOBAL;
      }
    }
  }
}

void dl_updatuj(G_KONFIG * p_ber)
{
  int all = p_ber->conf_dyn_light_update_all;

  if (p_ber->dl_on || all) {
    if (all)
      p_ber->conf_dyn_light_update_all = FALSE;

    dl_transformuj_svetla(p_ber);

    if (p_ber->conf_dyn_light_mesh || p_ber->conf_dyn_light_beruska)
      dl_updatuj_seznam_meshu(p_ber, all);

    if (p_ber->conf_dyn_light_poly) {
      sdl_updatuj_seznam_poly(p_ber, all);
      p_ber->conf_full_light_last = FALSE;
    }

    dl_reset_svetel(p_ber);
    p_ber->dl_recalc = FALSE;
    p_ber->dl_new = FALSE;

    p_ber->edl_recalc = FALSE;
    p_ber->edl_new = FALSE;
  }
}

/*
  Udelat update-list -> ktere meshe a svetla se maji prepocitat

  - mesh -> pri posunu meshe (dyn+staticke)
  - svetlo -> vse v okoli
*/
/*
  Svetlo privazat k:

  - targetu kamery
  - kamere
  - meshi

  kameru privazat k:  
  - meshi
  - svetlu
*/

ExtraLightHandle edl_svetlo_vyrob(dword flag, int meshnum)
{
  EXTRA_DYN_LIGHT *p_light;
  int l;

  if (flag & EDL_MESH_LIGHT && flag & EDL_SCENA) {
    return (K_CHYBA);
  }

  for (l = 0; l < p_ber->edl_lightnum; l++) {
    if (!p_ber->p_edlight[l].akt) {
      p_light = p_ber->p_edlight + l;
      goto nasel;
    }
  }

  if (l == MAX_FLARE_SVETEL)
    return (K_CHYBA);

  p_light = p_ber->p_edlight + l;
  p_ber->edl_lightnum++;
nasel:

  p_light->akt = TRUE;
  p_light->flag = flag | EDL_ZMENA_GLOBAL;

  if (flag | EDL_MESH_LIGHT && meshnum) {
    p_light->p_lmesh = mmalloc(sizeof(p_light->p_lmesh[0]) * meshnum);
    p_light->lmeshnum = meshnum;
    p_light->lmeshakt = 0;
  }

  p_ber->kamera.zmena = TRUE;
  p_ber->edl_new = TRUE;
  return (l);
}

ExtraLightHandle edl_svetlo_zrus_meshe(ExtraLightHandle lh)
{
  EXTRA_DYN_LIGHT *p_light = p_ber->p_edlight + lh;
  GAME_MESH_DATA *p_data;
  GAME_MESH_OLD *p_mesh;
  int i;

  if (!p_light->akt)
    return (FALSE);
  else {
    p_ber->edl_new = TRUE;

    // Zruseni meshu
    if (p_light->flag | EDL_MESH_LIGHT && p_light->lmeshnum) {
      for (i = 0; i < p_light->lmeshakt; i++) {
        p_mesh = p_ber->p_mesh[p_light->p_lmesh[i]];
        if (p_mesh) {
          p_data = p_mesh->p_data;
          p_data->kflag &= ~KONT_DRAW_LIGHT;
          p_data->kflag |= KONT_POHYB;
          p_data->top_edlight = K_CHYBA;
        }
      }
      null_free(&p_light->p_lmesh);
      p_light->lmeshnum = p_light->lmeshakt = 0;
    }
    return (TRUE);
  }
}


int edl_svetlo_zrus(ExtraLightHandle lh)
{
  EXTRA_DYN_LIGHT *p_light = p_ber->p_edlight + lh;

  if (!p_light->akt)
    return (FALSE);
  else {
    edl_svetlo_zrus_meshe(lh);

    // Zruseni svetla
    p_light->akt = FALSE;
    if (p_ber->edl_lightnum == lh + 1)
      p_ber->edl_lightnum--;
    p_ber->edl_new = TRUE;

    // Zruseni animace
    if (p_light->an.endtime)
      sdla_animaci_zrus(&p_light->an);

    // Zajisti prepocitani svetel
    p_ber->dl_new = TRUE;

    return (TRUE);
  }
}

ExtraLightHandle edl_svetlo_pridej_mesh(ExtraLightHandle lh, MeshHandle mh)
{
  EXTRA_DYN_LIGHT *p_light = p_ber->p_edlight + lh;
  GAME_MESH_DATA *p_data;
  int m;

  if (lh < p_ber->edl_lightnum &&
    p_light->akt &&
    p_light->flag | EDL_MESH_LIGHT &&
    p_light->lmeshnum && p_light->lmeshakt < p_light->lmeshnum) {

    assert(p_ber->p_prv_lev[mh]);
    p_data = p_ber->p_prv_lev[mh]->p_mesh_data;
    m = p_ber->p_prv_lev[mh]->mesh;

    if (p_data->kflag & KONT_DRAW_LIGHT) {
      return (K_CHYBA);
    }
    else {
      p_light->p_lmesh[p_light->lmeshakt++] = m;
      p_data->top_edlight = lh;
      return (lh);
    }
  }
  else {
    return (K_CHYBA);
  }
}

ExtraLightHandle edl_svetlo_set_pos(ExtraLightHandle lh, BOD * p_bod,
  BOD * p_dir)
{
  EXTRA_DYN_LIGHT *p_light = p_ber->p_edlight + lh;

  if (lh < p_ber->edl_lightnum && p_light->akt) {
    p_light->tp = p_light->np = *p_bod;
    p_light->dir = *p_dir;
    p_light->flag |= EDL_ZMENA_GLOBAL;
    p_ber->edl_recalc = TRUE;
    return (lh);
  }
  else
    return (K_CHYBA);
}

ExtraLightHandle edl_svetlo_set_pos_ext(ExtraLightHandle lh,
  float px, float py, float pz, float dx, float dy, float dz)
{
  EXTRA_DYN_LIGHT *p_light = p_ber->p_edlight + lh;

  if (lh < p_ber->edl_lightnum && p_light->akt) {
    p_light->np.x = px;
    p_light->np.y = py;
    p_light->np.z = pz;
    p_light->dir.x = dx;
    p_light->dir.y = dy;
    p_light->dir.z = dz;
    p_light->tp = p_light->np;
    p_light->flag |= EDL_ZMENA_GLOBAL;
    p_ber->edl_recalc = TRUE;
    return (lh);
  }
  else
    return (K_CHYBA);
}

ExtraLightHandle edl_svetlo_set_par(ExtraLightHandle lh, float utlum,
  float dosah, float theta, float gourand)
{
  EXTRA_DYN_LIGHT *p_light = p_ber->p_edlight + lh;

  if (lh < p_ber->edl_lightnum && p_light->akt) {
    p_light->dosah = dosah;
    p_light->utlum = utlum;
    p_light->theta = theta;
    p_light->gourand = gourand;
    p_light->flag |= EDL_ZMENA_GLOBAL;
    p_ber->edl_recalc = TRUE;
    return (lh);
  }
  else
    return (K_CHYBA);
}

ExtraLightHandle edl_svetlo_set_diff(ExtraLightHandle lh, float da)
{
  EXTRA_DYN_LIGHT *p_light = p_ber->p_edlight + lh;

  if (lh < p_ber->edl_lightnum && p_light->akt) {
    p_light->da = da;
    p_light->flag |= EDL_ZMENA_LOKAL;
    p_ber->edl_recalc = TRUE;
    return (lh);
  }
  else
    return (K_CHYBA);
}

ExtraLightHandle edl_svetlo_set_spec(ExtraLightHandle lh, float sr, float sg,
  float sb)
{
  EXTRA_DYN_LIGHT *p_light = p_ber->p_edlight + lh;

  if (lh < p_ber->edl_lightnum && p_light->akt) {
    p_light->sr = sr;
    p_light->sg = sg;
    p_light->sb = sb;
    p_light->flag |= EDL_ZMENA_LOKAL;
    p_ber->edl_recalc = TRUE;
    return (lh);
  }
  else
    return (K_CHYBA);
}

ExtraLightHandle edl_svetlo_vyrob_zemni_mlha(dword barva, float v_start,
  float v_stop, float h_start, float h_stop)
{
  EXTRA_DYN_LIGHT *p_light;
  int l;

  for (l = 0; l < p_ber->edl_lightnum; l++) {
    if (!p_ber->p_edlight[l].akt) {
      p_light = p_ber->p_edlight + l;
      goto nasel;
    }
  }

  if (l == MAX_FLARE_SVETEL)
    return (K_CHYBA);

  p_light = p_ber->p_edlight + l;
  p_ber->edl_lightnum++;
nasel:

  p_light->akt = TRUE;
  p_light->flag =
    EDL_ZEMNI_MLHA | EDL_SPEC_ADD | EDL_SCENA | EDL_ZMENA_GLOBAL |
    EDL_ZMENA_LOKAL | EDL_ZMENA_CAMERA;
  rgb_float(barva, &p_light->sr);
  p_light->v_start = v_start;
  p_light->v_stop = v_stop;
  p_light->h_start = h_start;
  p_light->h_stop = h_stop;

  p_ber->kamera.zmena = TRUE;
  p_ber->edl_new = TRUE;
  return (l);
}

/*
  Privaze svetlo k meshum
*/
ExtraLightHandle edl_svetlo_uzavri_meshe(ExtraLightHandle lh)
{
  EXTRA_DYN_LIGHT *p_light = p_ber->p_edlight + lh;
  GAME_MESH_OLD *p_mesh;
  GAME_MESH_DATA *p_data;
  int i, flags;


  if (lh < p_ber->edl_lightnum && p_light->akt
    && p_light->flag | EDL_MESH_LIGHT) {

    flags = p_light->flag;

    for (i = 0; i < p_light->lmeshakt; i++) {
      p_mesh = p_ber->p_mesh[p_light->p_lmesh[i]];
      p_data = p_mesh->p_data;
      assert(p_mesh);

      if (flags & (EDL_SPEC_MOD | EDL_SPEC_SET | EDL_SPEC_ADD | EDL_SPEC_SUB)) {
        p_data->kflag |= KONT_DRAW_SPEC;
        if (!(p_data->m2flag & MAT2_SPECULAR)) {
          p_mesh->p_vertex_spec =
            mmalloc(sizeof(p_mesh->p_vertex_spec[0]) * p_mesh->vertexnum);
        }
      }

      if (flags & EDL_PRUHL_LIGHT) {
        p_data->kflag |= KONT_DRAW_PRUHL;
      }

      p_data->kflag |= KONT_DRAW_LIGHT;
    }
    return (lh);
  }
  else {
    return (K_CHYBA);
  }
}


/*
  Animace extra-svetel
*/
ExtraLightHandle edl_anim_vyrob(ExtraLightHandle lh, int framenum,
  int pos_keys, int trg_keys, int alfa_keys, int spec_keys, int dosah_keys)
{
  EXTRA_DYN_LIGHT *p_light = p_ber->p_edlight + lh;
  DYN_LIGHT_ANIM *p_an;

  if (lh < p_ber->edl_lightnum && p_light->akt) {
    p_an = &p_light->an;
    if (p_an->endtime)
      sdla_animaci_zrus(p_an);
    sdla_animaci_vyrob(p_an, framenum, pos_keys, trg_keys,
      0, spec_keys, dosah_keys, alfa_keys);
    return (lh);
  }
  else
    return (K_CHYBA);
}

ExtraLightHandle edl_anim_zrus(ExtraLightHandle lh)
{
  EXTRA_DYN_LIGHT *p_light = p_ber->p_edlight + lh;

  if (lh < p_ber->edl_lightnum && p_light->akt && p_light->an.endtime) {
    sdla_animaci_zrus(&p_light->an);
    return (lh);
  }
  else
    return (K_CHYBA);
}

/*
  In-Klice extra-dyn-light
*/
ExtraLightHandle edl_anim_vloz_klic_posun(ExtraLightHandle lh,
  int klic, float x, float y, float z, int frame)
{
  EXTRA_DYN_LIGHT *p_light = p_ber->p_edlight + lh;
  DYN_LIGHT_ANIM *p_an = &p_light->an;

  if (lh < p_ber->edl_lightnum && p_light->akt &&
    p_an->endtime && klic < p_an->pos_keys) {
    p_an->p_pos[klic].x = x;
    p_an->p_pos[klic].y = y;
    p_an->p_pos[klic].z = z;
    p_an->p_pkeys[klic].time = frame * SIM_KONSTI;
    return (lh);
  }
  else
    return (K_CHYBA);
}

ExtraLightHandle edl_anim_vloz_klic_posun_bod(ExtraLightHandle lh, int klic,
  BOD * p_p, int frame)
{
  EXTRA_DYN_LIGHT *p_light = p_ber->p_edlight + lh;
  DYN_LIGHT_ANIM *p_an = &p_light->an;

  if (lh < p_ber->edl_lightnum && p_light->akt && p_an->endtime &&
    klic < p_an->pos_keys) {
    p_an->p_pos[klic] = *p_p;
    p_an->p_pkeys[klic].time = frame * SIM_KONSTI;
    return (lh);
  }
  else
    return (K_CHYBA);
}

ExtraLightHandle edl_anim_vloz_klic_target(ExtraLightHandle lh,
  int klic, float x, float y, float z, int frame)
{
  EXTRA_DYN_LIGHT *p_light = p_ber->p_edlight + lh;
  DYN_LIGHT_ANIM *p_an = &p_light->an;

  if (lh < p_ber->edl_lightnum && p_light->akt &&
    p_an->endtime && klic < p_an->trg_keys) {
    p_an->p_trg[klic].x = x;
    p_an->p_trg[klic].y = y;
    p_an->p_trg[klic].z = z;
    p_an->p_tkeys[klic].time = frame * SIM_KONSTI;
    return (lh);
  }
  else
    return (K_CHYBA);
}

ExtraLightHandle edl_anim_vloz_klic_target_bod(ExtraLightHandle lh, int klic,
  BOD * p_p, int frame)
{
  EXTRA_DYN_LIGHT *p_light = p_ber->p_edlight + lh;
  DYN_LIGHT_ANIM *p_an = &p_light->an;

  if (lh < p_ber->edl_lightnum && p_light->akt && p_an->endtime &&
    klic < p_an->trg_keys) {
    p_an->p_trg[klic] = *p_p;
    p_an->p_tkeys[klic].time = frame * SIM_KONSTI;
    return (lh);
  }
  else
    return (K_CHYBA);
}


ExtraLightHandle edl_anim_vloz_klic_diff(ExtraLightHandle lh, int klic,
  float a, int frame)
{
  EXTRA_DYN_LIGHT *p_light = p_ber->p_edlight + lh;
  DYN_LIGHT_ANIM *p_an = &p_light->an;

  if (lh < p_ber->edl_lightnum && p_light->akt && p_an->endtime
    && klic < p_an->alfa_keys) {
    p_an->p_alfa[klic] = a;
    p_an->p_akeys[klic].time = frame * SIM_KONSTI;
    return (lh);
  }
  else
    return (K_CHYBA);
}

ExtraLightHandle edl_anim_vloz_klic_spec(ExtraLightHandle lh, int klic,
  float r, float g, float b, int frame)
{
  EXTRA_DYN_LIGHT *p_light = p_ber->p_edlight + lh;
  DYN_LIGHT_ANIM *p_an = &p_light->an;

  if (lh < p_ber->edl_lightnum && p_light->akt && p_an->endtime &&
    klic < p_an->spec_keys) {
    p_an->p_spec[klic].r = r;
    p_an->p_spec[klic].g = g;
    p_an->p_spec[klic].b = b;
    p_an->p_spkeys[klic].time = frame * SIM_KONSTI;
    return (lh);
  }
  else
    return (K_CHYBA);
}

LightHandle edl_anim_vloz_klic_vzdal(LightHandle lh, int klic, float dosah,
  float theta, float utlum, int frame)
{
  EXTRA_DYN_LIGHT *p_light = p_ber->p_edlight + lh;
  DYN_LIGHT_ANIM *p_an = &p_light->an;

  if (lh < p_ber->edl_lightnum && p_light->akt && p_an->endtime &&
    klic < p_an->dos_keys) {
    p_an->p_dosah[klic].x = dosah;
    p_an->p_dosah[klic].y = theta;
    p_an->p_dosah[klic].z = utlum;
    p_an->p_dskeys[klic].time = frame * SIM_KONSTI;
    return (lh);
  }
  else
    return (K_CHYBA);
}

/* ***************************************************
    Run-time animace extra-dynamickych svetel
   ***************************************************
*/
RunHandle edl_anim_start(ExtraLightHandle lh, int *p_flag, int flag,
  int start, int stop)
{
  EXTRA_DYN_LIGHT *p_light = p_ber->p_edlight + lh;
  DYN_LIGHT_ANIM *p_an = &p_light->an;

  if (lh < p_ber->edl_lightnum && p_light->akt && p_an->endtime) {

    calc_time_end(p_an->endtime, p_ber->TimeEndLastFrame, start, stop,
      &p_light->start,
      &p_light->stop,
      &p_light->time_start, &p_light->time_stop, &p_light->time_delka);

    p_light->p_flag = p_flag;
    p_light->an_flag = flag;
    p_light->flag |= EDL_ANIMACE;
    return (lh);
  }
  else
    return (K_CHYBA);
}

int edl_anim_stop(RunHandle rh)
{
  EXTRA_DYN_LIGHT *p_light = p_ber->p_edlight + rh;
  DYN_LIGHT_ANIM *p_an = &p_light->an;

  if (rh < p_ber->edl_lightnum && p_light->akt &&
    p_an->endtime && p_light->flag & EDL_ANIMACE) {
    p_light->flag &= ~EDL_ANIMACE;
    return (rh);
  }
  else
    return (K_CHYBA);
}

/* **************************************************************** 
     Extra-svetla - keyframova animace svetel
   ****************************************************************
*/
inline void edl_light_to_light(EXTRA_DYN_LIGHT * p_light,
  DYN_LIGHT_ANIM * p_track, int time)
{
  int loop = p_light->an_flag & GK_LOOP;

  if (p_track->pos_keys) {
    key_track_interpolace_bod(&p_light->np, p_track->p_pos, p_track->p_pkeys,
      time, p_track->endtime, p_track->pos_keys, loop);
    p_light->tp = p_light->np;
  }
  if (p_track->trg_keys) {
    key_track_interpolace_bod(&p_light->dir, p_track->p_trg, p_track->p_tkeys,
      time, p_track->endtime, p_track->trg_keys, loop);
  }
  if (p_track->dos_keys) {
    key_track_interpolace_bod_linear((BOD *) & p_light->dosah,
      p_track->p_dosah, p_track->p_dskeys, time, p_track->endtime,
      p_track->dos_keys, loop);
  }
  if (p_track->alfa_keys) {
    key_track_interpolace_float_linear(&p_light->da, p_track->p_alfa,
      p_track->p_akeys, time, p_track->endtime, p_track->alfa_keys, loop);
  }
  if (p_track->spec_keys) {
    key_track_interpolace_bod_linear((BOD *) & p_light->sr,
      (BOD *) p_track->p_spec, p_track->p_spkeys, time, p_track->endtime,
      p_track->spec_keys, loop);
  }
}

void edla_updatuj(G_KONFIG * p_ber)
{
  dword next_time = p_ber->TimeEndLastFrame;
  EXTRA_DYN_LIGHT *p_light;
  DYN_LIGHT_ANIM *p_an;
  int i;
  int konec;

  /*
     Animace se updatuji kazdy frame
   */
  for (i = 0; i < p_ber->edl_lightnum; i++) {
    p_light = p_ber->p_edlight + i;
    p_an = &p_light->an;
    if (p_light->flag & EDL_ANIMACE) {

      /* Test na konec animace
       */
      if ((konec = (next_time > p_light->time_stop))) {
        if (p_light->an_flag & GK_LOOP) {
          calc_time_loop(next_time, p_light->start,
            &p_light->time_start,
            &p_light->time_stop, &p_light->time_delka, &p_light->time);
          if (p_light->p_flag)
            *(p_light->p_flag) = 0;
          konec = 0;
        }
        else {
          p_light->time = p_light->time_delka;
          p_light->flag &= ~EDL_ANIMACE;
          if (p_light->p_flag)
            *(p_light->p_flag) = ANIM_FLAG_KONEC;
        }
      }
      else {
        p_light->time = calc_time_akt(next_time, p_light->time_start);
        if (p_light->p_flag)
          *(p_light->p_flag) =
            ftoi((p_light->time / (float) p_light->time_delka) * 100.0f);
      }

      /* Vypocitej animaci svetla
       */
      edl_light_to_light(p_light, p_an, p_light->time);
      p_light->flag |= EDL_ZMENA_GLOBAL;
      p_ber->edl_recalc = TRUE;
    }
  }
}



/*
   Run-time rutiny pro extra-dynamicke svetla
*/
/*
  Prepocitam seznamy svetel -> pouze pri update
*/
int edl_mesh_pridej_svetla(G_KONFIG * p_ber, GAME_MESH_OLD * p_mesh)
{
  GAME_MESH_DATA *p_data = p_mesh->p_data;
  EXTRA_DYN_LIGHT *p_light;
  EXTRA_DYN_LIGHT *p_light_dyn[MAX_FLARE_SVETEL * 2];
  dword flags = 0;
  float vzdal;
  int flag;
  int lightnum = 0;
  int i;
  int kflag = p_data->kflag;

  if (kflag & (KONT_POHYB | KONT_VIDITELNY_ZMENA) || p_ber->edl_recalc
    || p_ber->edl_new) {

    // Zaradim dynamicke svetla
    lightnum = 0;
    for (i = 0; i < p_ber->edl_lightnum; i++) {
      p_light = p_ber->p_edlight + i;
      if (p_light->akt) {
        flag = p_light->flag;
        if (!(flag & EDL_SCENA)) {
          if (flag & EDL_MESH_LIGHT) {
            if (kflag & KONT_DRAW_LIGHT && p_data->top_edlight == i) {
              p_light_dyn[lightnum++] = p_light;
              flags |= p_light->flag;
            }
          }
          else if (flag & EDL_DOSAH) {
            if (flag & EDL_BODOVE) {
              if (p_light->dosah > obb_vzdal_bod(&p_mesh->obb_world,
                  &p_light->tp)) {
                p_light_dyn[lightnum++] = p_light;
                flags |= p_light->flag;
              }
            }
            else if (flag & (EDL_PLOSNE_X | EDL_PLOSNE_Y | EDL_PLOSNE_Z)) {
              if (flag & EDL_PLOSNE_X) {
                vzdal = obb_vzdal_rovina_x(&p_mesh->obb_world, p_light->tp.x);
              }
              else if (flag & EDL_PLOSNE_Y) {
                vzdal = obb_vzdal_rovina_y(&p_mesh->obb_world, p_light->tp.y);
              }
              else {
                vzdal = obb_vzdal_rovina_z(&p_mesh->obb_world, p_light->tp.z);
              }
              if (p_light->dosah > vzdal) {
                p_light_dyn[lightnum++] = p_light;
                flags |= p_light->flag;
              }
            }
          }
        }
        else {
          p_light_dyn[lightnum++] = p_light;
          flags |= p_light->flag;
        }
      }
    }

    if (!lightnum) {
      null_free((void **) &p_data->p_edlight);
      p_data->edlightnum = p_data->edlightmax = 0;
      if (!(kflag & KONT_DRAW_LIGHT)) {
        p_data->kflag &= ~(KONT_DRAW_SPEC);
        p_data->kflag &= ~(KONT_DRAW_PRUHL);
        if (!(p_data->m2flag & (MAT2_SPECULAR | MAT2_ENV_SPEC))
          && p_mesh->p_vertex_spec) {
          free(p_mesh->p_vertex_spec);
          p_mesh->p_vertex_spec = NULL;
        }
      }
    }
    else {
      // Nastav ostrou spekularni barvu
      if (flags & (EDL_SPEC_MOD | EDL_SPEC_SET | EDL_SPEC_ADD | EDL_SPEC_SUB)) {
        p_data->kflag |= KONT_DRAW_SPEC;
        if (!p_mesh->p_vertex_spec) {
          p_mesh->p_vertex_spec =
            mmalloc(sizeof(p_mesh->p_vertex_spec[0]) * p_mesh->vertexnum);
        }
      }

      if (flags & EDL_PRUHL_LIGHT) {
        p_data->kflag |= KONT_DRAW_PRUHL;
      }

      if (lightnum >= p_data->edlightmax) {
        null_free((void **) &p_data->p_edlight);
        i += (lightnum >> 1) + 10;
        p_data->p_edlight = mmalloc(sizeof(p_data->p_edlight[0]) * i);
        p_data->edlightmax = i;
        memcpy(p_data->p_edlight, p_light_dyn,
          sizeof(p_data->p_edlight[0]) * lightnum);
      }
      else {
        memcpy(p_data->p_edlight, p_light_dyn,
          sizeof(p_data->p_edlight[0]) * lightnum);
      }
      p_data->edlightnum = lightnum;
    }
    return (TRUE);
  }
  else {
    return (FALSE);
  }

}


/***********************************************************************
   Dynamicke flare
  **********************************************************************
*/

FlareHandle kom_flare_set_param(FlareHandle fh, float r, float g, float b,
  float a, float dx, float dy, int material, int alfa)
{
  LENS_FLARE *p_flare = (LENS_FLARE *) fh;

  p_flare->akt = TRUE;
  p_flare->r = r;
  p_flare->g = g;
  p_flare->b = b;
  p_flare->a = a;
  p_flare->dx = dx;
  p_flare->dy = dy;
  p_flare->material = material;
  p_flare->alfa = alfa;
  p_flare->p_mat = p_ber->p_mat[material];
  return (fh);
}

LENS_FLARE *kom_flare_get_pointer(FlareHandle fh)
{
  return ((LENS_FLARE *) fh);
}

FlareHandle kom_flare_set_pivot(FlareHandle fh, BOD * p_pivot)
{
  LENS_FLARE *p_flare = (LENS_FLARE *) fh;

  p_flare->p_bod = p_pivot;
  return (fh);
}

FlareHandle kom_flare_set_pos(FlareHandle fh, BOD * p_pos)
{
  LENS_FLARE *p_flare = (LENS_FLARE *) fh;

  p_flare->p = *p_pos;
  return (fh);
}

FlareHandle kom_flare_vyrob(int flag)
{
  LENS_FLARE *p_flare;

  p_flare = mmalloc(sizeof(p_flare[0]));

  p_flare->p_next = p_ber->p_flare;
  p_flare->p_prev = NULL;
  if (p_ber->p_flare) {
    p_ber->p_flare->p_prev = p_flare;
  }

  p_ber->p_flare = p_flare;
  p_flare->zflag = flag;
  p_flare->p_svetlo = NULL;
  return ((int) p_flare);
}

void kom_flare_zrus(FlareHandle fh)
{
  LENS_FLARE *p_flare = (LENS_FLARE *) fh;

  if (p_flare->p_next)
    p_flare->p_next->p_prev = p_flare->p_prev;
  if (p_flare->p_prev)
    p_flare->p_prev->p_next = p_flare->p_next;
  else
    p_ber->p_flare = p_flare->p_next;

  free(p_flare);
}

void dl_nastav_kvalitu(G_KONFIG * p_ber)
{
  if (p_ber->conf_dyn_rychlost == LIGHT_ALL) {
    p_ber->conf_dyn_light_prvky = FALSE;
    p_ber->conf_dyn_light_beruska = FALSE;
    p_ber->conf_dyn_light_mesh = TRUE;
    p_ber->conf_dyn_light_poly = TRUE;
  }
  else if (p_ber->conf_dyn_rychlost == LIGHT_PRVKY) {
    p_ber->conf_dyn_light_prvky = TRUE; // pouze prvky
    p_ber->conf_dyn_light_beruska = FALSE;
    p_ber->conf_dyn_light_mesh = TRUE;
    p_ber->conf_dyn_light_poly = TRUE;
  }
  else if (p_ber->conf_dyn_rychlost == LIGHT_VERTEXY) {
    p_ber->conf_dyn_light_prvky = TRUE; // pouze vertexove prvky
    p_ber->conf_dyn_light_beruska = FALSE;
    p_ber->conf_dyn_light_mesh = TRUE;
    p_ber->conf_dyn_light_poly = FALSE;
  }
  else if (p_ber->conf_dyn_rychlost == LIGHT_NIC) {
    p_ber->conf_dyn_light_beruska = TRUE;       // pouze beruska
    p_ber->conf_dyn_light_prvky = FALSE;
    p_ber->conf_dyn_light_mesh = FALSE;
    p_ber->conf_dyn_light_poly = FALSE;
  }
}

/* Nastavi detaily pro dynamicke svetla
*/
void dl_nastav(G_KONFIG * p_ber)
{
  int rych = p_ber->conf_dyn_rychlost;

  // on-off svetel
  p_ber->conf_dyn_rychlost = LIGHT_ALL;
  p_ber->conf_dyn_light_update_all = TRUE;
  dl_nastav_kvalitu(p_ber);
  dl_updatuj(p_ber);

  p_ber->conf_dyn_rychlost = rych;
  dl_nastav_kvalitu(p_ber);
}

void mlha_reset(void)
{
  p_ber->p_mlha_akt = NULL;
}
