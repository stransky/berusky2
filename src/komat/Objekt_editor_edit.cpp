/*
  Editacni funkce
*/
#include "mss_on.h"
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <direct.h>
#include <float.h>
#include <assert.h>
#include "objekt_editor_all.h"

#include "dbgwnd.h"

/*******************************************************************************
  Univerzalni edit-funkce
 *******************************************************************************
*/
int oe_vybr_kont_reset(void)
{
  if (p_cnf->groupfacenum &&
    p_cnf->groupflag & GROUP_KONT &&
    !(p_cnf->groupflag & (GROUP_OBJ | GROUP_FACE)) && p_cnf->groupfacenum) {
    return (1);
  }
  else {
    if (p_cnf->p_kont[p_cnf->vybr_kont])
      return (1);
  }
  return (0);
}

EDIT_KONTEJNER *oe_vybr_kont_get(int *p_handle)
{
  EDIT_KONTEJNER *p_kont = NULL;
  int h = *p_handle - 1;

  if (p_cnf->groupfacenum &&
    p_cnf->groupflag & GROUP_KONT &&
    !(p_cnf->groupflag & (GROUP_OBJ | GROUP_FACE))) {
    if (h < p_cnf->groupfacenum) {
      p_kont = p_cnf->p_kont[p_cnf->p_group_face[h].k];
      (*p_handle)++;
    }
  }
  else {
    if (!h) {
      p_kont = p_cnf->p_kont[p_cnf->vybr_kont];
      (*p_handle)++;
    }
  }
  return (p_kont);
}

int oe_vybr_obj_reset(void)
{
  if (p_cnf->groupfacenum &&
    p_cnf->groupflag & GROUP_KONT &&
    p_cnf->groupflag & GROUP_OBJ &&
    !(p_cnf->groupflag & GROUP_FACE) && p_cnf->groupfacenum) {
    return (1);
  }
  else {
    if (p_cnf->p_kont[p_cnf->vybr_kont] &&
      p_cnf->p_kont[p_cnf->vybr_kont]->p_obj[p_cnf->vybr_objekt])
      return (1);
  }
  return (0);
}

EDIT_OBJEKT *oe_vybr_obj_get(int *p_handle)
{
  EDIT_KONTEJNER *p_kont = NULL;
  EDIT_OBJEKT *p_obj = NULL;
  int h = *p_handle - 1;

  if (p_cnf->groupfacenum &&
    p_cnf->groupflag & GROUP_KONT &&
    p_cnf->groupflag & GROUP_OBJ && !(p_cnf->groupflag & GROUP_FACE)) {
    if (h < p_cnf->groupfacenum) {
      p_kont = p_cnf->p_kont[p_cnf->p_group_face[h].k];
      if (p_kont) {
        p_obj = p_kont->p_obj[p_cnf->p_group_face[h].o];
        (*p_handle)++;
      }
    }
  }
  else {
    if (!h) {
      p_kont = p_cnf->p_kont[p_cnf->vybr_kont];
      p_obj = p_kont->p_obj[p_cnf->vybr_objekt];
      (*p_handle)++;
    }
  }
  return (p_obj);
}

EDIT_OBJEKT *oe_vybr_obj_get_ind(int *p_handle, int *p_k, int *p_o)
{
  EDIT_KONTEJNER *p_kont = NULL;
  EDIT_OBJEKT *p_obj = NULL;
  int h = *p_handle - 1;

  if (p_cnf->groupfacenum &&
    p_cnf->groupflag & GROUP_KONT &&
    p_cnf->groupflag & GROUP_OBJ && !(p_cnf->groupflag & GROUP_FACE)) {
    if (h < p_cnf->groupfacenum) {
      p_kont = p_cnf->p_kont[p_cnf->p_group_face[h].k];
      if (p_kont) {
        p_obj = p_kont->p_obj[p_cnf->p_group_face[h].o];
        if (p_k)
          *p_k = p_cnf->p_group_face[h].k;
        if (p_o)
          *p_o = p_cnf->p_group_face[h].o;
        (*p_handle)++;
      }
    }
  }
  else {
    if (!h) {
      p_kont = p_cnf->p_kont[p_cnf->vybr_kont];
      p_obj = p_kont->p_obj[p_cnf->vybr_objekt];
      if (p_k)
        *p_k = p_cnf->vybr_kont;
      if (p_o)
        *p_o = p_cnf->vybr_objekt;
      (*p_handle)++;
    }
  }
  return (p_obj);
}

EDIT_KONTEJNER *oe_vybr_obj_get_kont(int *p_handle)
{
  EDIT_KONTEJNER *p_kont = NULL;
  int h = *p_handle - 2;

  if (p_cnf->groupfacenum &&
    p_cnf->groupflag & GROUP_KONT &&
    p_cnf->groupflag & GROUP_OBJ && !(p_cnf->groupflag & GROUP_FACE)) {
    if (h < p_cnf->groupfacenum) {
      p_kont = p_cnf->p_kont[p_cnf->p_group_face[h].k];
    }
  }
  else {
    if (!h) {
      p_kont = p_cnf->p_kont[p_cnf->vybr_kont];
    }
  }
  return (p_kont);
}

int oe_vybr_obj_get_num(void)
{
  if (p_cnf->groupfacenum &&
    p_cnf->groupflag & GROUP_KONT &&
    p_cnf->groupflag & GROUP_OBJ && !(p_cnf->groupflag & GROUP_FACE)) {
    return (p_cnf->groupfacenum);
  }
  else {
    return (1);
  }
}

/*******************************************************************************
  Editace kontejneru
********************************************************************************
*/
void oe_scale_kontejner(EDIT_KONTEJNER * p_kont, float x, float y, float z)
{
  if (p_kont == NULL)
    return;
  scale_matrix(&p_kont->world, x, y, z);
}

void oe_prikaz_scale_kontejneru(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  int handle;
  float f1;

  if (!(handle = oe_vybr_kont_reset()))
    return;

  nuluj_mys();
  while (stisk_mys()) {

    f1 = 1.0f;
    if (key[K_CTRL])
      f1 += (float) mi.dx * 0.0001f;
    else
      f1 += (float) mi.dx * 0.01f;

    handle = oe_vybr_kont_reset();
    while (p_kont = oe_vybr_kont_get(&handle)) {
      scale_matrix(&p_kont->world, f1, f1, f1);
      key_kontejner_sim_reanimuj(p_kont);
    }

    nuluj_dif();

    oe_renderuj_scenu_pre(p_cnf);
    oe_renderuj_scenu_po(p_cnf);
  }
  kprintf(TRUE, "Zmena meritka o %f", f1);
}

#define DEF_ROZMER 2.0f

void oe_prikaz_autoscale_kontejneru(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  BOD s, t;
  float scale;
  int handle;

  if (!(handle = oe_vybr_kont_reset()))
    return;

  kprintf(TRUE, "Autoscale kontejneru...");

  while (p_kont = oe_vybr_kont_get(&handle)) {
    updatuj_obalku_kontejner(p_kont);

    vektor_mult_skalar(&p_kont->obb.obb_len, 2.0f, &t);
    transformuj_bod_matici_bod(&t, kont_world_matrix(p_kont), &s);

    scale = (s.x > s.y) ? s.x : s.y;
    scale = (scale > s.z) ? scale : s.z;

    scale = DEF_ROZMER / scale;

    scale_matrix(&p_kont->world, scale, scale, scale);
    key_kontejner_sim_reanimuj(p_kont);
    updatuj_obalku_kontejner(p_kont);

    s.x = p_kont->obb.obb_stred.x;
    s.y = p_kont->obb.obb_stred.y - p_kont->obb.obb_len.y;
    s.z = p_kont->obb.obb_stred.z;

    transformuj_bod_bod_matici(&s, kont_world_matrix(p_kont));

    p_kont->world._41 -= s.x;
    p_kont->world._42 -= s.y;
    p_kont->world._43 -= s.z;

    key_kontejner_sim_reanimuj(p_kont);
    updatuj_obalku_kontejner(p_kont);

    kprintf(TRUE, "Zmena meritka o %f", scale);
  }
}

void oe_centruj_kontejner(EDIT_KONTEJNER * p_kont)
{
  OBJ_VERTEX *p_vert;
  OBJ_VERTEX stred;
  int i, j;
  word pocet;
  int celkem = 0;

  if (p_kont == NULL)
    return;

  stred.x = stred.y = stred.z = 0;

  for (j = 0; j < p_kont->max_objektu; j++) {
    if (!p_kont->p_obj[j])
      continue;

    p_vert = p_kont->p_obj[j]->p_vertex;
    pocet = p_kont->p_obj[j]->vertexnum;

    // Nalezeni stredu objektu
    for (i = 0; i < pocet; i++) {
      stred.x += p_vert[i].x;
      stred.y += p_vert[i].y;
      stred.z += p_vert[i].z;
    }
    celkem += pocet;
  }
  stred.x /= (float) celkem;
  stred.y /= (float) celkem;
  stred.z /= (float) celkem;

  for (j = 0; j < p_kont->max_objektu; j++) {
    if (!p_kont->p_obj[j])
      continue;

    p_vert = p_kont->p_obj[j]->p_vertex;
    pocet = p_kont->p_obj[j]->vertexnum;

    // Nalezeni stredu objektu
    for (i = 0; i < pocet; i++) {
      p_vert[i].x -= stred.x;
      p_vert[i].y -= stred.y;
      p_vert[i].z -= stred.z;
    }
  }
}

void oe_prikaz_scale_kontejneru_cisla(K_EDITOR * p_cnf)
{
  doe_prikaz_scale_kontejneru_cisla_dialog();
}

void oe_prikaz_smaz_divoke_vertexy_kontejneru(K_EDITOR * p_cnf)
{
  if (p_cnf->p_kont[p_cnf->vybr_kont])
    smaz_divoke_vertexy_kontejneru(p_cnf->p_kont[p_cnf->vybr_kont]);
}

void oe_prikaz_vycisti_kontejner(K_EDITOR * p_cnf)
{
  int k = p_cnf->vybr_kont;

  n_kont(k);
  vycisti_kontejner(p_cnf->p_kont[k]);
  updatuj_kontejner_statistika(p_cnf->p_kont[k], TRUE);
}

void oe_prikaz_vymazat_kontejner(K_EDITOR * p_cnf)
{
  int k = p_cnf->vybr_kont;

  n_kont(k);

  ddw("Cistim kontejner %d", k);
  vyprazdni_kontejner(p_cnf->p_kont[k]);
}

void oe_prikaz_posun_kontejner(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  VEKTOR v1, v2;
  GLMATRIX mat;
  GLMATRIX *p_world;
  int k = p_cnf->vybr_kont;
  int flag;


  if (key[K_CTRL]) {
    oe_prikaz_kopiruj_kontejner(p_cnf);
    k = p_cnf->vybr_kont;
    key[K_CTRL] = FALSE;
  }

  n_kont(k);
  p_kont = p_cnf->p_kont[k];

  p_world = kont_world_matrix(p_kont);
  calc_transf_3d_2d_matrix(p_world, p_cnf->p_camera, p_cnf->p_project, &mat);
  *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);
  transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
    &v1.x, &v1.y, &v1.z, &mat, OXRES, OYRES, OXSTART, OYSTART,
    p_cnf->kam.near_plane, p_cnf->kam.far_plane);

  nuluj_mys();
  while (stisk_mys()) {
    transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
      &v2.x, &v2.y, &v2.z, &mat, OXRES, OYRES, OXSTART, OYSTART,
      p_cnf->kam.near_plane, p_cnf->kam.far_plane);

    flag = 0;
    if (!key[K_X] && !key[K_Y] && !key[K_Z]) {
      translate_matrix(p_world, v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);
      flag = POSUN_VSE;
    }
    else {
      if (key[K_X]) {
        translate_matrix(p_world, v2.x - v1.x, 0, 0);
        flag = POSUN_OSA_X;
      }
      if (key[K_Y]) {
        translate_matrix(p_world, 0, v2.y - v1.y, 0);
        flag = POSUN_OSA_Y;
      }
      if (key[K_Z]) {
        translate_matrix(p_world, 0, 0, v2.z - v1.z);
        flag = POSUN_OSA_Z;
      }
    }
    key_kontejner_sim_reanimuj(p_kont);

    nuluj_dif();
    oe_renderuj_scenu_pre(p_cnf);
    oe_kresli_editacni_stred(p_cnf, (BOD *) & p_world->_41, flag);
    oe_renderuj_scenu_po(p_cnf);
    transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
      &v1.x, &v1.y, &v1.z, &mat, OXRES, OYRES, OXSTART, OYSTART,
      p_cnf->kam.near_plane, p_cnf->kam.far_plane);
  }
  *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);
}

void oe_prikaz_zvedni_kontejner(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  GLMATRIX *p_world;
  int k = p_cnf->vybr_kont;

  n_kont(k);
  p_kont = p_cnf->p_kont[k];
  while (p_kont) {
    p_world = kont_world_matrix(p_kont);
    translate_matrix(p_world, 0, 0.05f, 0);
    key_kontejner_sim_reanimuj(p_kont);
    p_kont = p_kont->p_next;
  }
}

void oe_prikaz_kopiruj_kontejner(K_EDITOR * p_cnf)
{
  int k = p_cnf->vybr_kont, d;

  n_kont(k);

  if (oe_vyber_volny_kontejner(p_cnf) == K_CHYBA)
    return;


  {
    d = p_cnf->vybr_kont;
    p_cnf->p_kont[d] = kopiruj_kontejner(p_cnf->p_kont[k], NULL, NULL, 0,
      p_cnf->light, MAX_EDIT_SVETEL, p_cnf->p_dlight, MAX_FLARE_SVETEL,
      p_cnf->p_edlight, MAX_FLARE_SVETEL, p_cnf->flare, MAX_EDIT_SVETEL);
    strcpy(p_cnf->p_kont[d]->jmeno, p_cnf->p_kont[k]->jmeno);
  }

}

void oe_prikaz_kopiruj_kontejner_vyber(K_EDITOR * p_cnf)
{
  int k = p_cnf->vybr_kont, k2 = p_cnf->vybr_kont_last, flag = 0;

  n_kont(k);
  n_kont(k2);

  if (doe_kopie_kontejneru(NULL, &flag)) {
    kprintf(TRUE, "Kopiruji z '%s' do '%s'...", p_cnf->p_kont[k2]->jmeno,
      p_cnf->p_kont[k]->jmeno);
    kopiruj_kontejner_indir_vyber(p_cnf->p_kont[k2], p_cnf->p_kont[k], flag);
  }
}

void oe_vymaz_flare_svetel_ze_sceny(K_EDITOR * p_cnf, EDIT_KONTEJNER * p_kont)
{
  STATIC_LIGHT *p_slight = p_kont->p_slight;

  while (p_slight) {
    if (p_slight->p_flare)
      oe_smaz_flare(p_cnf, p_slight->p_flare);
    p_slight = p_slight->p_next;
  }
}

void oe_prikaz_mazani_kontejneru_int(K_EDITOR * p_cnf)
{
  int flag;

  n_kont(p_cnf->vybr_kont);
  flag = p_cnf->p_kont[p_cnf->vybr_kont]->kflag;
  if (flag & KONT_PRVEK) {
    kprintf(TRUE, "Tento kontejner levelu se musi mazat jako prvek !");
    return;
  }
  if (flag & KONT_ZRCADLO) {
    oe_zrcadlo_zrus(p_cnf);
  }
  if (oe_prikaz_kontroluj_ungroup_vertex(p_cnf, p_cnf->vybr_kont))
    oe_prikaz_all_ungroup_vertex(p_cnf);
  oe_vymaz_flare_svetel_ze_sceny(p_cnf, p_cnf->p_kont[p_cnf->vybr_kont]);
  lo_vymaz_svetla_ze_sceny(p_cnf->p_kont[p_cnf->vybr_kont]);
  zrus_kontejner_rec(&p_cnf->p_kont[p_cnf->vybr_kont], &p_cnf->p_mlha);
}

void oe_prikaz_mazani_kontejneru(K_EDITOR * p_cnf)
{
  int i;

  if (p_cnf->groupfacenum &&
    p_cnf->groupflag & GROUP_KONT &&
    !(p_cnf->groupflag & (GROUP_OBJ | GROUP_FACE))) {
    for (i = 0; i < p_cnf->groupfacenum; i++) {
      p_cnf->vybr_kont = p_cnf->p_group_face[i].k;
      oe_prikaz_mazani_kontejneru_int(p_cnf);
    }
    oe_ungroup_face_all(p_cnf);
  }
  else {
    oe_prikaz_mazani_kontejneru_int(p_cnf);
  }
  be_kontroluj_level(&p_cnf->bnf);
  oe_prikaz_smaz_jcf(p_cnf);
}

void oe_prikaz_slep_kontejner(K_EDITOR * p_cnf)
{
  int cislo = p_cnf->vybr_kont;

  n_kont(cislo);

  oe_prikaz_all_ungroup_vertex(p_cnf);
  kprintf(1, "Lepim kontejner...");
  slep_kontejner(p_cnf->p_kont[cislo]);
  kprintf(1, "OK");
}


void oe_slep_kontejnery(EDIT_KONTEJNER * p_dest, EDIT_KONTEJNER * p_src)
{
  int i, n;

  for (i = 0; i < p_src->objektu; i++) {
    if (p_src->p_obj[i]) {
      n = lo_najdi_volny_objekt_kont(p_dest);
      assert(n != K_CHYBA);
      p_dest->p_obj[n] = p_src->p_obj[i];
      p_src->p_obj[i] = NULL;
    }
  }
  p_dest->kflag |= p_src->kflag;
  p_dest->m1flag |= p_src->m1flag;
  p_dest->m2flag |= p_src->m2flag;
  updatuj_kontejner_statistika(p_dest, TRUE);
}

// Slepi vybrane kontejnery do jednoho
void oe_prikaz_slep_kontejnery(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  int i, k;

  if (p_cnf->groupfacenum < 2)
    kprintf(TRUE, "Musi byt vybrany min 2 kontejnery !!!");
  else {
    if (p_cnf->groupflag & (GROUP_OBJ | GROUP_FACE)) {
      kprintf(TRUE, "Musi byt vybrane pouze kontejnery...");
      return;
    }
    else {
      oe_prikaz_all_ungroup_vertex(p_cnf);
      p_kont = p_cnf->p_kont[p_cnf->p_group_face[0].k];
      transformuj_kontejner(p_kont);
      kprintf(1, "Lepim do kontejneru %s", p_kont->jmeno);
      for (i = 1; i < p_cnf->groupfacenum; i++) {
        k = p_cnf->p_group_face[i].k;
        if (p_cnf->p_kont[k]) {
          kprintf(1, "Lepim kontejner %s", p_cnf->p_kont[k]->jmeno);
          transformuj_kontejner(p_cnf->p_kont[k]);
          oe_slep_kontejnery(p_kont, p_cnf->p_kont[k]);
          zrus_kontejner_rec(p_cnf->p_kont + k, &p_cnf->p_mlha);
        }
      }
      oe_ungroup_face_all(p_cnf);
    }
  }
}

EDIT_KONTEJNER *oe_slep_kontejnery_vsechny(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_vys = NULL;
  EDIT_KONTEJNER *p_kont;
  int i;

  for (i = 0; i < KONT_NUM; i++) {
    p_kont = p_cnf->p_kont[i];
    if (p_kont) {
      p_kont =
        kopiruj_kontejner(p_kont, NULL, NULL, 0, p_cnf->light,
        MAX_EDIT_SVETEL, p_cnf->p_dlight, MAX_FLARE_SVETEL, p_cnf->p_edlight,
        MAX_FLARE_SVETEL, p_cnf->flare, MAX_EDIT_SVETEL);
      transformuj_kontejner(p_kont);
      kprintf(1, "Lepim kontejner %s", p_kont->jmeno);
      if (!p_vys) {
        p_vys = p_kont;
      }
      else {
        oe_slep_kontejnery(p_vys, p_kont);
        zrus_kontejner_rec(&p_kont, NULL);
      }
    }
  }
  return (p_vys);
}

void oe_prikaz_centruj_kontejner(K_EDITOR * p_cnf)
{
  int cislo = p_cnf->vybr_kont;

  n_kont(cislo);

  oe_centruj_kontejner(p_cnf->p_kont[cislo]);
}

void oe_prikaz_init_matrix_kontejner(K_EDITOR * p_cnf)
{
  int cislo = p_cnf->vybr_kont;

  n_kont(cislo);
  init_matrix(&p_cnf->p_kont[cislo]->world);
  init_matrix(&p_cnf->p_kont[cislo]->mpmatrix);
}

void oe_prikaz_rotace_kontejneru(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  GLMATRIX m, *p_m;
  float fi_z = 0, fi = 0;
  int k = p_cnf->vybr_kont;

  n_kont(k);

  p_kont = p_cnf->p_kont[k];


  nuluj_mys();

  transformuj_kontejner(p_cnf->p_kont[p_cnf->vybr_kont]);

  m = *p_cnf->p_camera;
  m._41 = m._42 = m._43 = 0;
  transformuj_kontejner_matici(p_kont, &m);

  p_m = &p_kont->world;
  invert_matrix(p_cnf->p_camera, p_m);
  p_m->_41 = p_m->_42 = p_m->_43 = 0;

  while (stisk_mys()) {

    fi_z = ((float) PI / 180) * (float) mi.dx * 0.5f;
    if (fi + fi_z > -PI && fi + fi_z < PI) {
      rotate_matrix_z(&p_kont->world, fi_z);
      fi += fi_z;
    }

    kprintf(TRUE, "Rotace o %.3f stupnu", fi / (PI / 180));
    key_kontejner_sim_reanimuj(p_kont);
    nuluj_dif();

    oe_renderuj_scenu_pre(p_cnf);
    oe_renderuj_scenu_po(p_cnf);
  }

  transformuj_kontejner(p_kont);
}

void oe_prikaz_rotace_kontejneru_osove(K_EDITOR * p_cnf, int x, int y, int z)
{
  EDIT_KONTEJNER *p_kont;
  float fi = 0;
  int k = p_cnf->vybr_kont;

  n_kont(k);

  p_kont = p_cnf->p_kont[k];


  nuluj_mys();

  while (stisk_mys()) {

    if (x) {
      rotate_matrix_x(&p_kont->world,
        ((float) PI / 180) * (float) mi.dx * 0.5f);
    }
    if (y) {
      rotate_matrix_y(&p_kont->world,
        ((float) PI / 180) * (float) mi.dx * 0.5f);
    }
    if (z) {
      rotate_matrix_z(&p_kont->world,
        ((float) PI / 180) * (float) mi.dx * 0.5f);
    }
    key_kontejner_sim_reanimuj(p_kont);
    nuluj_dif();
    oe_renderuj_scenu_pre(p_cnf);
    oe_renderuj_scenu_po(p_cnf);
  }
}

void oe_prikaz_rotace_kontejneru_cislama(K_EDITOR * p_cnf, float x, float y,
  float z)
{
  EDIT_KONTEJNER *p_kont;
  int k = p_cnf->vybr_kont;

  n_kont(k);



  if (!(p_kont = p_cnf->p_kont[p_cnf->vybr_kont]))
    return;
  if (x) {
    rotate_matrix_x(&p_kont->world, (x / 180.0f) * PI);
  }
  if (y) {
    rotate_matrix_y(&p_kont->world, (y / 180.0f) * PI);
  }
  if (z) {
    rotate_matrix_z(&p_kont->world, (z / 180.0f) * PI);
  }
  key_kontejner_sim_reanimuj(p_kont);
}

void oe_prikaz_transformace_kontejneru(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  int cislo = p_cnf->vybr_kont;

  n_kont(cislo);
  p_kont = p_cnf->p_kont[cislo];

  if (p_kont->kflag & KONT_KEYFRAME
    && ddwqueto(hwnd_aplikace, "Transformovat key-frame?")) {
    transformuj_kontejner_keyframe(p_kont);
  }
  else {
    transformuj_kontejner(p_kont);
    key_kontejner_sim_reanimuj(p_kont);
  }
}

/*******************************************************************************
  Editace Objektu
********************************************************************************
*/
void scale_vec(OBJ_VERTEX * p_vert, word pocet, float t_x, float t_y,
  float t_z)
{
  GLMATRIX m;
  dword i;
  float x, y, z;

  init_matrix(&m);
  m._11 = t_x;
  m._22 = t_y;
  m._33 = t_z;

  for (i = 0; i < pocet; i++) {
    x = p_vert[i].x;
    y = p_vert[i].y;
    z = p_vert[i].z;
    p_vert[i].x = x * m._11 + y * m._12 + z * m._13 + m._41;
    p_vert[i].y = x * m._21 + y * m._22 + z * m._23 + m._42;
    p_vert[i].z = x * m._31 + y * m._32 + z * m._33 + m._43;
  }
}

void oe_scale_objekt(EDIT_OBJEKT * p_obj, float x, float y, float z)
{
  if (p_obj == NULL)
    return;
  scale_vec(p_obj->p_vertex, p_obj->vertexnum, x, y, z);
}

void oe_prikaz_scale_objektu(K_EDITOR * p_cnf)
{
  EDIT_OBJEKT *p_obj;
  float f1;
  int k = p_cnf->vybr_kont;
  int o = p_cnf->vybr_objekt;

  n_objekt(k, o);

  oe_zrus_vyber_kosti(p_cnf, p_cnf->p_kont[k]->p_obj[o]);
  p_obj = kopiruj_objekt(p_cnf->p_kont[k]->p_obj[o]);
  f1 = 1;

  nuluj_mys();
  while (stisk_mys()) {

    if (key[K_CTRL])
      f1 += (float) mi.dx * 0.0001f;
    else
      f1 += (float) mi.dx * 0.01f;

    zrus_objekt(&p_cnf->p_kont[k]->p_obj[o]);
    p_cnf->p_kont[k]->p_obj[o] = kopiruj_objekt(p_obj);
    scale_vec(p_cnf->p_kont[k]->p_obj[o]->p_vertex,
      p_cnf->p_kont[k]->p_obj[o]->vertexnum, f1, f1, f1);

    nuluj_dif();

    oe_renderuj_scenu_pre(p_cnf);
    oe_renderuj_scenu_po(p_cnf);
  }
  zrus_objekt(&p_obj);
}

void oe_prikaz_slepeni_objektu(K_EDITOR * p_cnf)
{
  // Strcim to vsechno k prvnimu objektu
  EDIT_OBJEKT *p_obj;
  int handle = oe_vybr_kont_reset();
  int o, k, kl;
  int o1, k1;

  if (!handle)
    return;

  p_obj = oe_vybr_obj_get_ind(&handle, &k1, &o1);
  transformuj_kontejner(p_cnf->p_kont[k1]);
  kl = k1;

  while (oe_vybr_obj_get_ind(&handle, &k, &o)) {
    if (k != kl) {
      kl = k;
      transformuj_kontejner(p_cnf->p_kont[k]);
    }
    p_obj = slep_objekty(p_cnf->p_kont[k1]->p_obj[o1],
      p_cnf->p_kont[k]->p_obj[o]);
    zrus_objekt(&p_cnf->p_kont[k1]->p_obj[o1]);
    zrus_objekt(&p_cnf->p_kont[k]->p_obj[o]);
    p_cnf->p_kont[k1]->p_obj[o1] = p_obj;
  }

  strcpy(p_cnf->p_kont[k1]->p_obj[o1]->jmeno, "Slepenec");
  updatuj_kontejner_statistika(p_cnf->p_kont[k1], TRUE);
}


void oe_prikaz_rotace_objektu(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  GLMATRIX m, v;
  EDIT_OBJEKT *p_obj, *p_obj_z;
  float fi_z = 0, fi = 0;
  int i;
  int k = p_cnf->vybr_kont;
  int o = p_cnf->vybr_objekt;

  n_objekt(k, o);

  p_kont = p_cnf->p_kont[k];


  nuluj_mys();

  p_obj = p_kont->p_obj[o];

  transformuj_kontejner(p_kont);

  m = *p_cnf->p_camera;
  m._41 = m._42 = m._43 = 0;

  for (i = 0; i < p_obj->vertexnum; i++) {
    transformuj_bod_matici(&p_obj->p_vertex[i].x,
      &p_obj->p_vertex[i].y, &p_obj->p_vertex[i].z, &m);
  }

  invert_matrix(p_cnf->p_camera, &v);
  v._41 = v._42 = v._43 = 0;

  p_obj_z = kopiruj_objekt(p_obj);

  while (stisk_mys()) {
    fi_z = ((float) PI / 180) * (float) mi.dx * 0.5f;
    if (fi + fi_z > -PI && fi + fi_z < PI) {
      rotate_matrix_z(&v, fi_z);
      fi += fi_z;
    }
    zrus_objekt(&p_obj);
    p_obj = kopiruj_objekt(p_obj_z);
    for (i = 0; i < p_obj->vertexnum; i++) {
      transformuj_bod_matici(&p_obj->p_vertex[i].x, &p_obj->p_vertex[i].y,
        &p_obj->p_vertex[i].z, &v);
    }

    nuluj_dif();
    oe_renderuj_scenu_pre(p_cnf);
    oe_renderuj_scenu_po(p_cnf);
  }

  zrus_objekt(&p_obj_z);
  transformuj_kontejner(p_kont);
}


void oe_prikaz_rotace_objektu_osove(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_kont = p_cnf->p_kont[p_cnf->vybr_kont];
  EDIT_OBJEKT *p_obj, *p_zal;
  GLMATRIX m;
  float fi = 0;
  int k = p_cnf->vybr_kont;
  int o = p_cnf->vybr_objekt;
  int i;

  n_objekt(k, o);

  nuluj_mys();

  p_zal = kopiruj_objekt(p_kont->p_obj[o]);

  transformuj_kontejner(p_kont);
  init_matrix(&m);

  while (stisk_mys()) {
    if (key[K_X]) {
      rotate_matrix_x(&m, ((float) PI / 180) * (float) mi.dx * 0.5f);
    }
    if (key[K_Y]) {
      rotate_matrix_y(&m, ((float) PI / 180) * (float) mi.dx * 0.5f);
    }
    if (key[K_Z]) {
      rotate_matrix_z(&m, ((float) PI / 180) * (float) mi.dx * 0.5f);
    }
    nuluj_dif();

    zrus_objekt(&p_kont->p_obj[o]);
    p_obj = p_kont->p_obj[o] = kopiruj_objekt(p_zal);

    for (i = 0; i < p_obj->vertexnum; i++)
      transformuj_bod_matici(&p_obj->p_vertex[i].x, &p_obj->p_vertex[i].y,
        &p_obj->p_vertex[i].z, &m);

    oe_renderuj_scenu_pre(p_cnf);
    oe_renderuj_scenu_po(p_cnf);
  }

  transformuj_kontejner(p_cnf->p_kont[p_cnf->vybr_kont]);
}

void oe_prikaz_rotace_objektu_cislama(K_EDITOR * p_cnf, float x, float y,
  float z)
{
  EDIT_KONTEJNER *p_kont = p_cnf->p_kont[p_cnf->vybr_kont];
  EDIT_OBJEKT *p_obj, *p_zal;
  GLMATRIX m;
  int k = p_cnf->vybr_kont;
  int o = p_cnf->vybr_objekt;
  int i;

  n_objekt(k, o);



  p_zal = kopiruj_objekt(p_kont->p_obj[o]);

  transformuj_kontejner(p_kont);
  init_matrix(&m);

  if (x) {
    rotate_matrix_x(&m, ((float) PI / 180) * x);
  }
  if (y) {
    rotate_matrix_y(&m, ((float) PI / 180) * y);
  }
  if (z) {
    rotate_matrix_z(&m, ((float) PI / 180) * z);
  }

  zrus_objekt(&p_kont->p_obj[o]);
  p_obj = p_kont->p_obj[o] = kopiruj_objekt(p_zal);

  for (i = 0; i < p_obj->vertexnum; i++)
    transformuj_bod_matici(&p_obj->p_vertex[i].x, &p_obj->p_vertex[i].y,
      &p_obj->p_vertex[i].z, &m);

  transformuj_kontejner(p_cnf->p_kont[p_cnf->vybr_kont]);
}

/*
int              oe_vybr_kont_reset(void);
EDIT_KONTEJNER * oe_vybr_kont_get(int *p_handle);
int              oe_vybr_obj_reset(void);
EDIT_OBJEKT *    oe_vybr_obj_get(int *p_handle);
EDIT_OBJEKT *    oe_vybr_obj_get_ind(int *p_handle, int *p_k, int *p_o);
EDIT_KONTEJNER * oe_vybr_obj_get_kont(int *p_handle);
*/

void oe_prikaz_mazani_objektu(K_EDITOR * p_cnf)
{
  int handle = oe_vybr_obj_reset();
  EDIT_OBJEKT *p_obj;
  int o, k;

  while ((p_obj = oe_vybr_obj_get_ind(&handle, &k, &o))) {
    kprintf(TRUE, "Mazu objekt %s", p_cnf->p_kont[k]->p_obj[o]->jmeno);
    oe_zrus_vyber_kosti(p_cnf, p_cnf->p_kont[k]->p_obj[o]);
    zrus_objekt(&p_cnf->p_kont[k]->p_obj[o]);
    updatuj_kontejner_statistika(p_cnf->p_kont[k], TRUE);
  }

  be_kontroluj_level(&p_cnf->bnf);
  oe_prikaz_smaz_jcf(p_cnf);
  oe_prikaz_all_ungroup_vertex(p_cnf);
  oe_ungroup_face_all(p_cnf);
}

/*
typedef struct _JOINT {
  
  int     joint_ID;       // ID a cislo jointu

  int     vertexnum;      // seznamy vertexu
  int    *p_vertexlist;
  
  int     flag;  
  float   tuhost;         // tuhost kloubu (pro in-game animace)
   
  dword   pos_keys;       // absolutni pozice
  BOD    *p_pos;
  BOD     p;
  
  dword   rot_keys;       // absolutni rotace - tracky
  QUAT   *p_rot;
  QUAT    r;
  
  dword   scs_keys;       // absolutni scale
  BOD    *p_scs;
  BOD     s;

  BOD     pivot;          // pivot point kosti

  GLMATRIX    m;
  JOINT_KEYS *p_keys;
  
  struct _JOINT *p_child; // linearni seznam child kosti
  struct _JOINT *p_next;  // linearni seznam next kosti
  
} JOINT;
*/

void oe_prikaz_posun_animace_kosti_rec(JOINT * p_child, float dx, float dy,
  float dz)
{
  while (p_child) {
    p_child->pivot.x += dx;
    p_child->pivot.y += dy;
    p_child->pivot.z += dz;
    if (p_child->p_child)
      oe_prikaz_posun_animace_kosti_rec(p_child->p_child, dx, dy, dz);
    p_child = p_child->p_next;
  }
}

void oe_prikaz_posun_animace_kosti(EDIT_OBJEKT * p_obj, float dx, float dy,
  float dz)
{
  JOINT_ANIMACE *p_anim = p_obj->p_joit_animace;

  while (p_anim) {
    if (p_anim->p_child)
      oe_prikaz_posun_animace_kosti_rec(p_anim->p_child, dx, dy, dz);
    p_anim = p_anim->p_next;
  }
}

void oe_prikaz_posun_objekt(K_EDITOR * p_cnf)
{
  EDIT_OBJEKT *p_obj;
  OBJ_VERTEX *p_vert1;
  BOD *p_vert2;
  VEKTOR v1, v2;
  float dx, dy, dz;
  int b, i;
  GLMATRIX mat;
  int o = p_cnf->vybr_objekt;
  int k = p_cnf->vybr_kont;

  n_objekt(k, o);

  if (key[K_CTRL]) {
    oe_prikaz_kopiruj_objekt(p_cnf);
    key[K_CTRL] = FALSE;
  }


  calc_transf_3d_2d_matrix(kont_world_matrix(p_cnf->p_kont[k]),
    p_cnf->p_camera, p_cnf->p_project, &mat);

  *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);

  transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
    &v1.x, &v1.y, &v1.z, &mat, OXRES, OYRES, OXSTART, OYSTART,
    p_cnf->kam.near_plane, p_cnf->kam.far_plane);

  p_obj = p_cnf->p_kont[k]->p_obj[o];

  nuluj_mys();
  while (stisk_mys()) {
    transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
      &v2.x, &v2.y, &v2.z, &mat, OXRES, OYRES, OXSTART, OYSTART,
      p_cnf->kam.near_plane, p_cnf->kam.far_plane);

    b = p_obj->vertexnum;
    p_vert1 = p_obj->p_vertex;
    p_vert2 = p_obj->p_vertex_kosti;


    dx = v2.x - v1.x;
    dy = v2.y - v1.y;
    dz = v2.z - v1.z;

    if (key[K_X]) {
      dy = 0;
      dz = 0;
    }
    else if (key[K_Y]) {
      dx = 0;
      dz = 0;
    }
    else if (key[K_Z]) {
      dx = 0;
      dy = 0;
    }

    for (i = 0; i < b; i++) {
      p_vert1[i].x += dx;
      p_vert1[i].y += dy;
      p_vert1[i].z += dz;
    }
    if (p_vert2) {
      for (i = 0; i < b; i++) {
        p_vert2[i].x += dx;
        p_vert2[i].y += dy;
        p_vert2[i].z += dz;
      }
    }

    oe_prikaz_posun_animace_kosti(p_obj, dx, dy, dz);

    nuluj_dif();

    oe_renderuj_scenu_pre(p_cnf);
    oe_renderuj_scenu_po(p_cnf);
    transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
      &v1.x, &v1.y, &v1.z, &mat, OXRES, OYRES, OXSTART, OYSTART,
      p_cnf->kam.near_plane, p_cnf->kam.far_plane);
  }

  *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);
}

void oe_prikaz_posun_objekt_pivot_point(K_EDITOR * p_cnf)
{
  EDIT_OBJEKT *p_obj;
  VEKTOR v1, v2;
  float dx, dy, dz;
  GLMATRIX mat;
  int o = p_cnf->vybr_objekt;
  int k = p_cnf->vybr_kont;

  if (!p_cnf->cnf.objekt_stred) {
    ddw("Nejsou zapnute stredy objektu!");
    return;
  }

  n_objekt(k, o);

  p_obj = p_cnf->p_kont[k]->p_obj[o];

  calc_transf_3d_2d_matrix(kont_world_matrix(p_cnf->p_kont[k]),
    p_cnf->p_camera, p_cnf->p_project, &mat);

  *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);

  transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
    &v1.x, &v1.y, &v1.z, &mat, OXRES, OYRES, OXSTART, OYSTART,
    p_cnf->kam.near_plane, p_cnf->kam.far_plane);

  nuluj_mys();
  while (stisk_mys()) {
    transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
      &v2.x, &v2.y, &v2.z, &mat, OXRES, OYRES, OXSTART, OYSTART,
      p_cnf->kam.near_plane, p_cnf->kam.far_plane);


    dx = v2.x - v1.x;
    dy = v2.y - v1.y;
    dz = v2.z - v1.z;

    if (key[K_X]) {
      p_obj->pivot.x += dx;
    }
    else if (key[K_Y]) {
      p_obj->pivot.y += dy;
    }
    else if (key[K_Z]) {
      p_obj->pivot.z += dz;
    }
    else {
      p_obj->pivot.x += dx;
      p_obj->pivot.y += dy;
      p_obj->pivot.z += dz;
    }

    nuluj_dif();

    oe_renderuj_scenu_pre(p_cnf);
    oe_renderuj_scenu_po(p_cnf);
    transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
      &v1.x, &v1.y, &v1.z, &mat, OXRES, OYRES, OXSTART, OYSTART,
      p_cnf->kam.near_plane, p_cnf->kam.far_plane);
  }

  key_sim_root_vloz_pivoty_do_animace_kont(p_cnf->p_kont[k]);
  *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);
}


void oe_centr_pivot_point(EDIT_KONTEJNER * p_kont, EDIT_OBJEKT * p_obj)
{
  float dx, dy, dz;
  int i;

  dx = dy = dz = 0.0f;
  for (i = 0; i < p_obj->vertexnum; i++) {
    dx += p_obj->p_vertex[i].x;
    dy += p_obj->p_vertex[i].y;
    dz += p_obj->p_vertex[i].z;
  }

  p_obj->pivot.x = dx / (float) p_obj->vertexnum;
  p_obj->pivot.y = dy / (float) p_obj->vertexnum;
  p_obj->pivot.z = dz / (float) p_obj->vertexnum;
  key_sim_root_vloz_pivoty_do_animace_kont(p_kont);
}

void oe_down_pivot_point(EDIT_KONTEJNER * p_kont, EDIT_OBJEKT * p_obj)
{
  float y;
  int i;

  y = 0.0f;
  y = p_obj->p_vertex[0].y;
  for (i = 1; i < p_obj->vertexnum; i++) {
    if (y > p_obj->p_vertex[i].y)
      y = p_obj->p_vertex[i].y;
  }
  p_obj->pivot.y = y;
  key_sim_root_vloz_pivoty_do_animace_kont(p_kont);
}

void oe_prikaz_centr_pivot_point(K_EDITOR * p_cnf)
{
  int o = p_cnf->vybr_objekt;
  int k = p_cnf->vybr_kont;

  n_objekt(k, o);
  oe_centr_pivot_point(p_cnf->p_kont[k], p_cnf->p_kont[k]->p_obj[o]);
}

void oe_prikaz_down_centr_pivot(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  EDIT_OBJEKT *p_obj;
  int o;

  o = oe_vybr_obj_reset();
  if (o) {
    while ((p_obj = oe_vybr_obj_get(&o))) {
      p_kont = oe_vybr_obj_get_kont(&o);
      if (p_kont->kflag & KONT_STATIC)
        continue;
      oe_centr_pivot_point(p_kont, p_obj);
      oe_down_pivot_point(p_kont, p_obj);
    }
  }
}

void oe_prikaz_kopiruj_objekt(K_EDITOR * p_cnf)
{
// skopiruje vybrany objekt v akt. kontejneru
// a tento objekt vybere
  int k = p_cnf->vybr_kont;
  int o = p_cnf->vybr_objekt;

  n_objekt(k, o);




  if (!oe_vyber_volny_objekt(p_cnf))
    return;

  p_cnf->p_kont[k]->p_obj[p_cnf->vybr_objekt] =
    kopiruj_objekt(p_cnf->p_kont[k]->p_obj[o]);
  strcpy(p_cnf->p_kont[k]->p_obj[p_cnf->vybr_objekt]->jmeno,
    p_cnf->p_kont[k]->p_obj[o]->jmeno);
  updatuj_kontejner_statistika(p_cnf->p_kont[k], TRUE);
}

typedef struct _TEXT_INFO
{

  GLMATRIX m;                   // light/camera matrix world object*cam-1*2d= souradnice v texture

} TEXT_INFO;

void pretexturuj_objekt(EDIT_OBJEKT * p_obj, TEXT_INFO * p_info);
void transf_3d_2d_matrix_float_moje(float p_x, float p_y, float p_z,
  float *p_tx, float *p_ty, GLMATRIX * p_mat, dword x_c, dword y_c);

void oe_prikaz_light_map(K_EDITOR * p_cnf)
{
  int i, k = p_cnf->vybr_kont;

  n_kont(k);
  for (i = 0; i < p_cnf->p_kont[k]->max_objektu; i++)
    if (p_cnf->p_kont[k]->p_obj[i])
      text_test(p_cnf, p_cnf->p_kont[k]->p_obj[i]);
}

void text_test(K_EDITOR * p_cnf, EDIT_OBJEKT * p_obj)
{
  GLMATRIX m;
  TEXT_INFO ti;

// init_matrix(&m);
// translate_matrix(&m,5,5,5);
// rotate_matrix_x(&m,PI/2);

// invert_matrix(&m,p_cnf->p_camera);
// IDirect3DDevice7_SetTransform(p_dev, D3DTRANSFORMSTATE_VIEW, p_cnf->p_camera); 
  invert_matrix(p_cnf->p_camera, &m);
  ti.m = m;
  pretexturuj_objekt(p_obj, &ti);
}

void pretexturuj_objekt(EDIT_OBJEKT * p_obj, TEXT_INFO * p_info)
{
/*
 GLMATRIX w,c,p,v;
 OBJ_VERTEX *p_b;
 int i;

 init_matrix(&w); // world bude jedno
 invert_matrix(&p_info->m,&c); // camera
 projection_matrix(&p,4,OXRES,OYRES);
 
 calc_3d_2d_matrix(&w, &c, &p, &v);

 for(i = 0; i < p_obj->vertexnum; i++) {
   p_b = p_obj->p_vertex+i;
//   transf_3d_2d_matrix_float_text(p_b->x, p_b->y, p_b->z, &p_b->tu2, &p_b->tv2, &v, 1);
 }
*/
}

void oe_prikaz_mazani_sceny(K_EDITOR * p_cnf)
{
  system_kurzor = 1;
  if (ddwqueto(hwnd_aplikace,
      "Chceö smazat celou scÈnu ? PeËlivÏ tento riskatnÌ postoj zvaû, neb jiû nelze vzÌti zpÏt!\nNechù tv˘j monitor nepozn· sÌlu tvÈno hnÏvu a aù tvoji kl·vesnici neskr·pÌ krokod˝lÌ slzy!"))
  {
    oe_smaz_scenu(p_cnf);
  }
  system_kurzor = 0;
}

/*
  Vsechny face ce jsou vetsi nez tento snizi v seznamu o 1
*/
__inline static void face_korekce(F_VERTEX * p_list, int num, int k, int o,
  int f)
{
  int i;

  for (i = 0; i < num; i++) {
    if (p_list[i].f > f && p_list[i].k == k && p_list[i].o == o)
      p_list[i].f -= 3;
  }
}

/*
  Smaze vybrany face bez jejich bodu
*/
void oe_prikaz_smaz_face(K_EDITOR * p_cnf, int korekce)
{
  int i;
  int f, o, k;
  FACE *p_face;


  if (p_cnf->groupfacenum) {
    for (i = 0; i < p_cnf->groupfacenum; i++) {
      k = p_cnf->p_group_face[i].k;
      o = p_cnf->p_group_face[i].o;
      f = p_cnf->p_group_face[i].f;
      n_objekt(k, o);
      if (p_cnf->p_kont[k]->p_obj[o]->facenum) {
        p_face = p_cnf->p_kont[k]->p_obj[o]->p_face;
        smaz_face_bez_vertexu(p_face, p_cnf->p_kont[k]->p_obj[o]->facenum, f);
        face_korekce(p_cnf->p_group_face + i + 1, p_cnf->groupfacenum - i - 1,
          k, o, f);
        p_cnf->vybr_face = 0;
      }
    }
    if (korekce) {
      for (i = 0; i < p_cnf->groupfacenum; i++) {
        k = p_cnf->p_group_face[i].k;
        if (p_cnf->p_kont[k]) {
          vycisti_kontejner(p_cnf->p_kont[k]);
          updatuj_kontejner_statistika(p_cnf->p_kont[k], TRUE);
          if (!p_cnf->p_kont[k]->objektu) {
            lo_vymaz_svetla_ze_sceny(p_cnf->p_kont[k]);
            zrus_kontejner_rec(&p_cnf->p_kont[k], &p_cnf->p_mlha);
          }
        }
      }
      oe_ungroup_face_all(p_cnf);
      oe_updatuj_scenu_stat(p_cnf);
    }
  }
  else {
    k = p_cnf->vybr_kont;
    o = p_cnf->vybr_objekt;
    f = p_cnf->vybr_face;
    n_objekt(k, o);
    if (p_cnf->p_kont[k]->p_obj[o]->facenum) {
      p_face = p_cnf->p_kont[k]->p_obj[o]->p_face;
      smaz_face_bez_vertexu(p_face, p_cnf->p_kont[k]->p_obj[o]->facenum, f);
      p_cnf->vybr_face = 0;
      vycisti_kontejner(p_cnf->p_kont[k]);
      updatuj_kontejner_statistika(p_cnf->p_kont[k], TRUE);
      if (!p_cnf->p_kont[k]->objektu) {
        lo_vymaz_svetla_ze_sceny(p_cnf->p_kont[k]);
        zrus_kontejner_rec(&p_cnf->p_kont[k], &p_cnf->p_mlha);
      }
    }
  }
  be_kontroluj_level(&p_cnf->bnf);
}

void oe_smaz_vertex(K_EDITOR * p_cnf, int k, int o, int v)
{
  OBJ_VERTEX *p_vert = p_cnf->p_kont[k]->p_obj[o]->p_vertex;
  int vertexnum = p_cnf->p_kont[k]->p_obj[o]->vertexnum;
  FACE *p_face = p_cnf->p_kont[k]->p_obj[o]->p_face;
  int i, facenum = p_cnf->p_kont[k]->p_obj[o]->facenum;


  for (i = 0; i < p_cnf->p_kont[k]->p_obj[o]->facenum; i += 3) {
    if (p_face[i] == v || p_face[i + 1] == v || p_face[i + 2] == v) {
      smaz_face_bez_vertexu(p_face, p_cnf->p_kont[k]->p_obj[o]->facenum, i);
      i -= 3;
    }
    else {
      if (p_face[i] > v)
        p_face[i]--;
      if (p_face[i + 1] > v)
        p_face[i + 1]--;
      if (p_face[i + 2] > v)
        p_face[i + 2]--;
    }
  }

  smaz_vertex_bez_facu(p_vert, p_cnf->p_kont[k]->p_obj[o]->vertexnum, v);
}

void oe_prikaz_smaz_vertex(K_EDITOR * p_cnf)
{
  int i, v, k, o, j = p_cnf->vybr_group;

  if (!p_cnf->groupnum)
    return;


  v = p_cnf->p_group[j].v;
  k = p_cnf->p_group[j].k;
  o = p_cnf->p_group[j].o;

  oe_smaz_vertex(p_cnf, k, o, v);

  for (i = 0; i < p_cnf->groupnum; i++) {
    if (p_cnf->p_group[i].k == k && p_cnf->p_group[i].o == o &&
      p_cnf->p_group[i].v > v)
      p_cnf->p_group[i].v--;
  }

  oe_ungroup_vertex(p_cnf, j);
  p_cnf->vybr_group = 0;
  be_kontroluj_level(&p_cnf->bnf);
}

/* Vyrobim seznam vertexu k smazani (k objektum)
*/
typedef struct _GROUP_MAZ_POM_OBJ
{

  int o;
  int vertexnum;                // velikost buferu
  int vertexakt;                // aktualni vertex
  int *p_vertexlist;

  struct _GROUP_MAZ_POM_OBJ *p_obj;

} GROUP_MAZ_POM_OBJ;

typedef struct _GROUP_MAZ_POM_KONT
{

  int k;
  GROUP_MAZ_POM_OBJ *p_obj;
  struct _GROUP_MAZ_POM_KONT *p_kont;

} GROUP_MAZ_POM_KONT;

#define SEGMENT (16*20)

static void pridej_vertex(GROUP_MAZ_POM_OBJ * p_grp, int v)
{
  if (!p_grp->p_vertexlist) {
    p_grp->vertexnum = SEGMENT;
    p_grp->vertexakt = 0;
    p_grp->p_vertexlist =
      mmalloc(sizeof(p_grp->p_vertexlist) * p_grp->vertexnum);
  }

  if (p_grp->vertexakt + 1 >= p_grp->vertexnum) {
    p_grp->vertexnum += SEGMENT;
    p_grp->p_vertexlist =
      realloc(p_grp->p_vertexlist,
      sizeof(p_grp->p_vertexlist) * p_grp->vertexnum);
  }

  p_grp->p_vertexlist[p_grp->vertexakt++] = v;
}

static GROUP_MAZ_POM_OBJ *oe_pridej_vertex_do_seznamu_obj(GROUP_MAZ_POM_OBJ *
  p_klo, int o, int v)
{
  GROUP_MAZ_POM_OBJ *p_tmp = p_klo;

  while (p_tmp) {
    if (p_tmp->o == o) {
      pridej_vertex(p_tmp, v);
      return (p_klo);
    }
    p_tmp = p_tmp->p_obj;
  }

  // nenalezeno-vyrobime nove
  p_tmp = mmalloc(sizeof(p_tmp[0]));
  pridej_vertex(p_tmp, v);
  p_tmp->p_obj = p_klo;
  p_tmp->o = o;
  return (p_tmp);
}

static GROUP_MAZ_POM_KONT *oe_pridej_vertex_do_seznamu(GROUP_MAZ_POM_KONT *
  p_kls, int k, int o, int v)
{
  GROUP_MAZ_POM_KONT *p_tmp = p_kls;

  while (p_tmp) {
    if (p_tmp->k == k) {
      p_tmp->p_obj = oe_pridej_vertex_do_seznamu_obj(p_tmp->p_obj, o, v);
      return (p_kls);
    }
    p_tmp = p_tmp->p_kont;
  }

  // nenalezeno - udelej novej
  p_tmp = mmalloc(sizeof(p_tmp[0]));
  p_tmp->p_obj = oe_pridej_vertex_do_seznamu_obj(NULL, o, v);
  p_tmp->p_kont = p_kls;
  p_tmp->k = k;
  return (p_tmp);
}

static void oe_vymaz_vertexy_objektu(EDIT_OBJEKT * p_obj, int *p_list,
  int num)
{
  OBJ_VERTEX *p_tmp;
  int *p_ind;
  FACE *p_fc;
  int f[3];
  int i, v, a;

  p_tmp = mmalloc(sizeof(p_tmp[0]) * p_obj->vertexnum);
  p_ind = mmalloc(sizeof(p_ind[0]) * p_obj->vertexnum);
  p_fc = mmalloc(sizeof(p_fc[0]) * p_obj->facenum);

  for (i = 0; i < num; i++) {
    v = p_list[i];
    assert(v < p_obj->vertexnum);
    p_obj->p_vertex[v].x = FLT_MAX;
  }

  // prekopiruju vertexy
  for (a = 0, i = 0; i < p_obj->vertexnum; i++) {
    if (p_obj->p_vertex[i].x != FLT_MAX) {
      p_ind[i] = a;
      p_tmp[a] = p_obj->p_vertex[i];
      a++;
    }
    else {
      p_ind[i] = -1;
    }
  }
  free(p_obj->p_vertex);
  p_obj->vertexnum = a;
  p_obj->p_vertex = realloc(p_tmp, sizeof(p_tmp[0]) * a);

  // prevalim face
  for (a = 0, i = 0; i < p_obj->facenum; i += 3) {
    f[0] = p_obj->p_face[i];
    f[1] = p_obj->p_face[i + 1];
    f[2] = p_obj->p_face[i + 2];

    f[0] = p_ind[f[0]];
    f[1] = p_ind[f[1]];
    f[2] = p_ind[f[2]];

    if (f[0] == -1 || f[1] == -1 || f[2] == -1)
      continue;

    p_fc[a] = f[0];
    p_fc[a + 1] = f[1];
    p_fc[a + 2] = f[2];
    a += 3;
  }
  free(p_obj->p_face);
  p_obj->facenum = a;
  p_obj->p_face = realloc(p_fc, sizeof(p_fc[0]) * a);

  free(p_ind);
}

static void oe_group_vymaz_seznam_obj(K_EDITOR * p_cnf,
  GROUP_MAZ_POM_OBJ * p_klo, int k)
{
  // vymazu vertexy aktualni objektu
  oe_vymaz_vertexy_objektu(p_cnf->p_kont[k]->p_obj[p_klo->o],
    p_klo->p_vertexlist, p_klo->vertexakt);
  if (p_klo->p_obj) {
    oe_group_vymaz_seznam_obj(p_cnf, p_klo->p_obj, k);
  }
}

static void oe_group_vymaz_seznam_kont(K_EDITOR * p_cnf,
  GROUP_MAZ_POM_KONT * p_kls)
{
  if (!p_kls)
    return;
  oe_group_vymaz_seznam_obj(p_cnf, p_kls->p_obj, p_kls->k);
  updatuj_kontejner_statistika(p_cnf->p_kont[p_kls->k], TRUE);
  if (p_kls->p_kont)
    oe_group_vymaz_seznam_kont(p_cnf, p_kls->p_kont);
}

static void oe_group_seznam_obj_smaz(GROUP_MAZ_POM_OBJ * p_klo)
{
  GROUP_MAZ_POM_OBJ *p_next;

  if (!p_klo)
    return;

  // vymaz vnitrku
  p_next = p_klo->p_obj;
  free(p_klo->p_vertexlist);
  free(p_klo);

  if (p_next)
    oe_group_seznam_obj_smaz(p_next);
}

static void oe_group_seznam_kont_smaz(GROUP_MAZ_POM_KONT * p_kls)
{
  GROUP_MAZ_POM_KONT *p_next;

  if (!p_kls)
    return;

  if (p_kls->p_obj)
    oe_group_seznam_obj_smaz(p_kls->p_obj);

  p_next = p_kls->p_kont;
  free(p_kls);

  if (p_next)
    oe_group_seznam_kont_smaz(p_next);
}

void oe_prikaz_smaz_group_vertex(K_EDITOR * p_cnf)
{
  GROUP_MAZ_POM_KONT *p_kls = NULL;
  int i, k, o, v = p_cnf->vybr_group;

  if (!p_cnf->groupnum)
    return;

  kprintf(TRUE, "Vyrabim seznam vertexu...");
  for (i = 0; i < p_cnf->groupnum; i++) {
    v = p_cnf->p_group[i].v;
    k = p_cnf->p_group[i].k;
    o = p_cnf->p_group[i].o;
    p_kls = oe_pridej_vertex_do_seznamu(p_kls, k, o, v);
  }

  kprintf(TRUE, "Mazu vertexy....");
  oe_group_vymaz_seznam_kont(p_cnf, p_kls);

  kprintf(TRUE, "OK");
  oe_group_seznam_kont_smaz(p_kls);

  oe_prikaz_all_ungroup_vertex(p_cnf);
  be_kontroluj_level(&p_cnf->bnf);
}

int oe_smaz_divoke_vertexy_objektu(K_EDITOR * p_cnf, int k, int o)
{
  OBJ_VERTEX *p_vert = p_cnf->p_kont[k]->p_obj[o]->p_vertex;
  byte *p_div;
  word *p_face = p_cnf->p_kont[k]->p_obj[o]->p_face;
  int i, j, vn = p_cnf->p_kont[k]->p_obj[o]->vertexnum,
    od, fn = p_cnf->p_kont[k]->p_obj[o]->facenum;

  if (p_cnf->p_kont[k]->p_obj[o]->p_joit_animace)
    return (0);

  if ((p_div = (byte *) mmalloc(sizeof(byte) * vn)) == NULL) {
    chyba("pamet");
  }
  memset(p_div, 1, sizeof(byte) * vn);

  for (i = 0; i < fn; i += 3) {
    p_div[p_face[i]] = p_div[p_face[i + 1]] = p_div[p_face[i + 2]] = 0;
  }

  for (i = 0, od = 0; i < vn + od; i++) {
    if (p_div[i]) {
      smaz_vertex_bez_facu(p_vert, vn, (i - od));

      for (j = 0; j < fn; j += 3) {
        if (p_face[j] > i - od)
          p_face[j]--;
        if (p_face[j + 1] > i - od)
          p_face[j + 1]--;
        if (p_face[j + 2] > i - od)
          p_face[j + 2]--;
      }

      od++;
    }
  }

  free(p_div);

  p_cnf->p_kont[k]->p_obj[o]->vertexnum -= od;
  return (od);
}

void oe_prikaz_smaz_divoke_vertexy_objektu(K_EDITOR * p_cnf)
{
  oe_smaz_divoke_vertexy_objektu(p_cnf, p_cnf->vybr_kont, p_cnf->vybr_objekt);
}

void oe_prikaz_prusecik_cnf(K_EDITOR * p_cnf)
{
  BOD p, m;
  int k, o, v, i;

  if (p_cnf->groupnum == 0) {
    ddw("Musi byt oznacene body !");
    return;
  }

  oe_eliminuj_stejne_group(p_cnf);

  for (i = 0; i < p_cnf->groupnum; i++) {
    k = p_cnf->p_group[i].k;
    o = p_cnf->p_group[i].o;
    v = p_cnf->p_group[i].v;
    p.x = p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].x;
    p.y = p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].y;
    p.z = p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].z;
    prusecik(p_cnf->primka, p_cnf->primka + 1, &p, &m);
    p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].x = m.x;
    p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].y = m.y;
    p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].z = m.z;
  }
  oe_prikaz_all_ungroup_vertex(p_cnf);
}

void oe_prikaz_prusecik(K_EDITOR * p_cnf)
{
  BOD a[2], p, m;
  int k, o, v, i, j;

  if (p_cnf->groupnum == 0) {
    ddw("Musi byt oznacene 3 body !");
    return;
  }

  oe_eliminuj_stejne_group(p_cnf);

  if (p_cnf->groupnum != 3) {
    ddw("Spatny pocet bodu.");
    return;
  }

  for (i = 0, j = 0; i < p_cnf->groupnum; i++) {
    k = p_cnf->p_group[i].k;
    o = p_cnf->p_group[i].o;
    v = p_cnf->p_group[i].v;
    if (i == p_cnf->vybr_group) {
      p.x = p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].x;
      p.y = p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].y;
      p.z = p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].z;
    }
    else {
      a[j].x = p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].x;
      a[j].y = p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].y;
      a[j].z = p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].z;
      j++;
    }
  }

  prusecik(a, a + 1, &p, &m);

  for (i = 0, j = 0; i < 3; i++) {
    if (i == p_cnf->vybr_group) {
      k = p_cnf->p_group[i].k;
      o = p_cnf->p_group[i].o;
      v = p_cnf->p_group[i].v;
      p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].x = m.x;
      p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].y = m.y;
      p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].z = m.z;
      break;
    }
  }
  oe_prikaz_all_ungroup_vertex(p_cnf);
}

void prusecik_normalbod(NORMALBOD * p_a, NORMALBOD * p_b, NORMALBOD * p_p,
  NORMALBOD * p_I)
{
  float t_i, vz;
  BOD q;

  t_i =
    (p_b->x - p_a->x) * (p_p->x - p_a->x) + (p_b->y - p_a->y) * (p_p->y -
    p_a->y) + (p_b->z - p_a->z) * (p_p->z - p_a->z);
  vz = (float) vzdal_bodu_xy(p_a->x, p_a->y, p_a->z, p_b->x, p_b->y, p_b->z);
  t_i /= (vz * vz);

  q.x = p_b->x - p_a->x;
  q.y = p_b->y - p_a->y;
  q.z = p_b->z - p_a->z;

  p_I->x = p_a->x + q.x * t_i;
  p_I->y = p_a->y + q.y * t_i;
  p_I->z = p_a->z + q.z * t_i;
}


/********************************************************************************
  Operace s group vertexy
 ********************************************************************************
*/
// Umisti body na stejnou pozici
void oe_prikaz_sjednot_group_vertex(K_EDITOR * p_cnf)
{
  float x, y, z;
  int i, k, o, v, g = p_cnf->vybr_group;

  if (!p_cnf->groupnum)
    return;

  k = p_cnf->p_group[g].k;
  o = p_cnf->p_group[g].o;
  v = p_cnf->p_group[g].v;

  x = p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].x;
  y = p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].y;
  z = p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].z;

  for (i = 0; i < p_cnf->groupnum; i++) {
    if (k != p_cnf->p_group[i].k)
      continue;
    o = p_cnf->p_group[i].o;
    v = p_cnf->p_group[i].v;
    p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].x = x;
    p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].y = y;
    p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].z = z;
  }

  kprintf(TRUE, "Sjednoceni vertexu...");
}

// Udela z x bodu prvni bod
void oe_prikaz_slij_group_vertex(K_EDITOR * p_cnf)
{
  FACE *p_face;
  int facenum;
  int f, i, k, o, v, nv, g = p_cnf->vybr_group;

  if (!p_cnf->groupnum)
    return;

  k = p_cnf->p_group[g].k;
  o = p_cnf->p_group[g].o;
  v = p_cnf->p_group[g].v;

  for (i = 0; i < p_cnf->groupnum; i++) {
    if (i == g || k != p_cnf->p_group[i].k || o != p_cnf->p_group[i].o)
      continue;
    nv = p_cnf->p_group[i].v;
    p_face = p_cnf->p_kont[k]->p_obj[o]->p_face;
    facenum = p_cnf->p_kont[k]->p_obj[o]->facenum;
    for (f = 0; f < facenum; f++) {
      if (p_face[f] == nv)
        p_face[f] = v;
    }
  }

  vycisti_kontejner(p_cnf->p_kont[k]);
  kprintf(TRUE, "Sliti vertexu...");
}

#define DELTA_SLICKA     (p_cnf->cnf.slicka_delta)

void oe_prikaz_slivacka_group_vertex(K_EDITOR * p_cnf)
{
  EDIT_OBJEKT *p_obj;
  FACE *p_face;
  int facenum;
  int f, i, j, k, o, v, nv, g = p_cnf->vybr_group;

  if (!p_cnf->groupnum)
    return;

  kprintf(TRUE, "Slivacka sliva!");

  k = p_cnf->p_group[0].k;
  o = p_cnf->p_group[0].o;

  p_obj = p_cnf->p_kont[k]->p_obj[o];

  for (i = 0; i < p_cnf->groupnum; i++) {
    if (k != p_cnf->p_group[i].k || o != p_cnf->p_group[i].o)
      continue;
    v = p_cnf->p_group[i].v;
    for (j = i + 1; j < p_cnf->groupnum; j++) {
      nv = p_cnf->p_group[j].v;
      if (stejny_vertex_point_delta((BOD *) (p_obj->p_vertex + v),
          (BOD *) (p_obj->p_vertex + nv), DELTA_SLICKA)) {
        // nahradim bod vn bodem v
        p_face = p_obj->p_face;
        facenum = p_obj->facenum;
        for (f = 0; f < facenum; f++) {
          if (p_face[f] == nv)
            p_face[f] = v;
        }
      }
    }
  }
  oe_prikaz_all_ungroup_vertex(p_cnf);
  vycisti_kontejner(p_cnf->p_kont[k]);
}

void oe_prikaz_sesij_group_vertex(K_EDITOR * p_cnf)
{
  //GLMATRIX m;
  float x, y, z, nx, ny, nz;

//  float rx1,ry1,rx2,ry2;
  float min_vzdal, p;
  int b = 0;
  int i, k, o, v, g;


  while (p_cnf->groupnum) {
    if (p_cnf->groupnum == 1)
      continue;

    g = p_cnf->vybr_group;
    k = p_cnf->p_group[g].k;
    o = p_cnf->p_group[g].o;
    v = p_cnf->p_group[g].v;

    x = p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].x;
    y = p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].y;
    z = p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].z;

    min_vzdal = 0xfffff;
    b = 0;

    oe_renderuj_scenu_pre(p_cnf);
    oe_renderuj_scenu_po(p_cnf);
//    nuluj_mys();
//    while(stisk_mys());

    for (i = 0; i < p_cnf->groupnum; i++) {
      if (k != p_cnf->p_group[i].k)
        continue;
      if (i == g)
        continue;
      o = p_cnf->p_group[i].o;
      v = p_cnf->p_group[i].v;
      nx = p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].x;
      ny = p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].y;
      nz = p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].z;
//      transf_3d_2d_matrix(nx,ny,nz,&rx2, &ry2, &m, XRES, YRES);
//      p = (float)vzdal_bodu_2D_xy(rx1,ry1,rx2,ry2);
      p = (float) vzdal_bodu_xy(x, y, z, nx, ny, nz);
      if (p < min_vzdal) {
        min_vzdal = p;
        b = i;
      }
    }
    o = p_cnf->p_group[b].o;
    v = p_cnf->p_group[b].v;
    p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].x = x;
    p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].y = y;
    p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].z = z;
    if (b > g) {
      oe_ungroup_vertex(p_cnf, b);
      oe_ungroup_vertex(p_cnf, g);
    }
    else {
      oe_ungroup_vertex(p_cnf, g);
      oe_ungroup_vertex(p_cnf, b);
    }
  }
}

void oe_uloz_vertexy_ze_ctverce(K_EDITOR * p_cnf)
{
  p_cnf->groupnum =
    oe_vrat_body_ve_scene(p_cnf, p_cnf->x1, p_cnf->y1, p_cnf->x2, p_cnf->y2,
    NULL);
  p_cnf->vybr_group = 0;
  if (p_cnf->p_group) {
    free(p_cnf->p_group);
    p_cnf->p_group = NULL;
  }
  if (p_cnf->groupnum) {
    if ((p_cnf->p_group =
        (I_VERTEX *) malloc(sizeof(I_VERTEX) * p_cnf->groupnum)) == NULL) {
      chyba("Pamet");
    }
    oe_vrat_body_ve_scene(p_cnf, p_cnf->x1, p_cnf->y1, p_cnf->x2, p_cnf->y2,
      p_cnf->p_group);
  }
}

void oe_vrat_vertexy_ze_ctverce(K_EDITOR * p_cnf, I_VERTEX ** p_group,
  int *p_groupnum)
{
  *p_groupnum =
    oe_vrat_body_ve_scene(p_cnf, p_cnf->x1, p_cnf->y1, p_cnf->x2, p_cnf->y2,
    NULL);
  if ((*p_group =
      (I_VERTEX *) malloc(sizeof(I_VERTEX) * (*p_groupnum))) == NULL) {
    chyba("Pamet");
  }
  oe_vrat_body_ve_scene(p_cnf, p_cnf->x1, p_cnf->y1, p_cnf->x2, p_cnf->y2,
    *p_group);
}

void oe_prikaz_group_vertex(K_EDITOR * p_cnf)
{
  /*
     1. mysi se musi udelat vyber okna
     2. v tomto okne oznacit vsechny vertexy a zapsat do struktury
   */

  oe_vybrani_ctverce(p_cnf, 0x0000ff);
  oe_uloz_vertexy_ze_ctverce(p_cnf);
}

int oe_je_vertex_v_seznamu(K_EDITOR * p_cnf, int k, int o, int v)
{
  int i;
  int n = p_cnf->groupnum;

  if (!n)
    return (0);

  for (i = 0; i < n; i++) {
    if (p_cnf->p_group[i].v == v && p_cnf->p_group[i].o == o
      && p_cnf->p_group[i].k == k) {
      return (i + 1);
    }
  }
  return (0);
}

void oe_pridej_vertexy_ze_ctverce(K_EDITOR * p_cnf)
{
  int v, novych, i, j;
  I_VERTEX *p_grp;

  novych =
    oe_vrat_body_ve_scene(p_cnf, p_cnf->x1, p_cnf->y1, p_cnf->x2, p_cnf->y2,
    NULL);
  if ((p_grp = (I_VERTEX *) malloc(sizeof(I_VERTEX) * novych)) == NULL) {
    chyba("Pamet");
  }

  oe_vrat_body_ve_scene(p_cnf, p_cnf->x1, p_cnf->y1, p_cnf->x2, p_cnf->y2,
    p_grp);

  j = novych;
  for (i = 0; i < novych; i++) {
    if (oe_je_vertex_v_seznamu(p_cnf, p_grp[i].k, p_grp[i].o, p_grp[i].v)) {
      j--;
      p_grp[i].k = 0xffff;
    }
  }

  if ((v = j) <= 0)
    return;

  if ((p_cnf->p_group =
      (I_VERTEX *) realloc(p_cnf->p_group,
        sizeof(I_VERTEX) * (p_cnf->groupnum + j))) == NULL)
    chyba("Pamet");

  for (i = 0, j = p_cnf->groupnum; i < novych; i++) {
    if (p_grp[i].k != 0xffff)
      p_cnf->p_group[j++] = p_grp[i];
  }

  free(p_grp);
  p_cnf->groupnum += v;
}

void oe_prikaz_group_vertex_pridej(K_EDITOR * p_cnf)
{
  oe_vybrani_ctverce(p_cnf, 0x0000ff);
  oe_pridej_vertexy_ze_ctverce(p_cnf);
}

void oe_pridej_groupvertex(K_EDITOR * p_cnf, int k, int o, int v)
{
  int i, g;

  if (p_cnf->groupnum) {
    for (i = 0; i < p_cnf->groupnum; i++) {
      if (p_cnf->p_group[i].v == v && p_cnf->p_group[i].o == o
        && p_cnf->p_group[i].k == k)
        return;
    }
    p_cnf->p_group =
      realloc(p_cnf->p_group,
      (p_cnf->groupnum + 1) * sizeof(p_cnf->p_group[0]));
    mtest(p_cnf->p_group);
  }
  else {
    p_cnf->p_group =
      malloc((p_cnf->groupnum + 1) * sizeof(p_cnf->p_group[0]));
    mtest(p_cnf->p_group);
  }

  g = p_cnf->groupnum++;

  p_cnf->p_group[g].k = k;
  p_cnf->p_group[g].o = o;
  p_cnf->p_group[g].v = v;
}

void oe_prikaz_vyber_vertex(K_EDITOR * p_cnf)
{
  oe_prikaz_all_ungroup_vertex(p_cnf);

  oe_vybrani_konstantniho_ctverce(p_cnf);
  oe_uloz_vertexy_ze_ctverce(p_cnf);
}

void oe_prikaz_pridej_vertex(K_EDITOR * p_cnf)
{

// //oe_hlaska_konzole(p_con,"Pridani vertexu...",0);

  oe_vybrani_konstantniho_ctverce(p_cnf);
  oe_pridej_vertexy_ze_ctverce(p_cnf);

  //oe_smaz_malou_konzoli(p_con);
  //printfs(p_con,XK,YK,"Vybranych vertexu %d",p_cnf->groupnum);

}

void oe_prikaz_vyber_vertex_face(K_EDITOR * p_cnf)
{
  int k, o, f;

  oe_prikaz_all_ungroup_vertex(p_cnf);

  k = p_cnf->vybr_kont;
  o = p_cnf->vybr_objekt;

  if (!p_cnf->p_kont[k] || !p_cnf->p_kont[k]->p_obj[o])
    return;

  p_cnf->groupnum = 3;
  p_cnf->vybr_group = 0;
  if (p_cnf->p_group) {
    free(p_cnf->p_group);
    p_cnf->p_group = NULL;
  }
  if ((p_cnf->p_group =
      (I_VERTEX *) malloc(sizeof(I_VERTEX) * p_cnf->groupnum)) == NULL) {
    chyba("Pamet");
  }

  f = p_cnf->vybr_face;
  k = p_cnf->p_group[0].k = p_cnf->p_group[1].k = p_cnf->p_group[2].k = k;
  o = p_cnf->p_group[0].o = p_cnf->p_group[1].o = p_cnf->p_group[2].o = o;

  p_cnf->p_group[0].v = p_cnf->p_kont[k]->p_obj[o]->p_face[f];
  p_cnf->p_group[1].v = p_cnf->p_kont[k]->p_obj[o]->p_face[f + 1];
  p_cnf->p_group[2].v = p_cnf->p_kont[k]->p_obj[o]->p_face[f + 2];

  //oe_smaz_malou_konzoli(p_con);
  //printfs(p_con,XK,YK,"Vybranych vertexu %d",p_cnf->groupnum);
}

void oe_prikaz_pridej_vertex_face(K_EDITOR * p_cnf)
{
  int i, k, o, f, n, v1, v2, v3, d = 0;

  n = p_cnf->groupnum;
  if (!n)
    return;

  f = p_cnf->vybr_face;
  k = p_cnf->vybr_kont;
  o = p_cnf->vybr_objekt;
  v1 = p_cnf->p_kont[k]->p_obj[o]->p_face[f];
  v2 = p_cnf->p_kont[k]->p_obj[o]->p_face[f + 1];
  v3 = p_cnf->p_kont[k]->p_obj[o]->p_face[f + 2];

  for (i = 0; i < n; i++) {
    if (p_cnf->p_group[i].v == v1 && p_cnf->p_group[i].o == o
      && p_cnf->p_group[i].k == k) {
      v1 = 0xfffff;
      d++;
    }
    if (p_cnf->p_group[i].v == v2 && p_cnf->p_group[i].o == o
      && p_cnf->p_group[i].k == k) {
      v2 = 0xfffff;
      d++;
    }
    if (p_cnf->p_group[i].v == v3 && p_cnf->p_group[i].o == o
      && p_cnf->p_group[i].k == k) {
      v3 = 0xfffff;
      d++;
    }
  }

  if (d > 3)
    chyba("Vnitrni chyba");
  if (d == 3)
    return;

  p_cnf->groupnum += 3 - d;
  if ((p_cnf->p_group =
      (I_VERTEX *) realloc(p_cnf->p_group,
        sizeof(I_VERTEX) * p_cnf->groupnum)) == NULL)
    chyba("Pamet");

  if (v1 != 0xfffff) {
    p_cnf->p_group[n].k = k;
    p_cnf->p_group[n].o = o;
    p_cnf->p_group[n].v = v1;
    n++;
  }

  if (v2 != 0xfffff) {
    p_cnf->p_group[n].k = k;
    p_cnf->p_group[n].o = o;
    p_cnf->p_group[n].v = v2;
    n++;
  }

  if (v3 != 0xfffff) {
    p_cnf->p_group[n].k = k;
    p_cnf->p_group[n].o = o;
    p_cnf->p_group[n].v = v3;
    n++;
  }

  //oe_smaz_malou_konzoli(p_con);
  //printfs(p_con,XK,YK,"Vybranych vertexu %d",p_cnf->groupnum); 
}

void oe_prikaz_all_ungroup_vertex(K_EDITOR * p_cnf)
{
  p_cnf->groupnum = 0;
  p_cnf->vybr_group = 0;
  if (p_cnf->p_group) {
    free(p_cnf->p_group);
    p_cnf->p_group = NULL;
  }
}

int oe_prikaz_kontroluj_ungroup_vertex(K_EDITOR * p_cnf, int k)
{
  int i;

  for (i = 0; i < p_cnf->groupnum; i++) {
    if (p_cnf->p_group[k].k == k)
      return (TRUE);
  }
  return (FALSE);
}

void oe_ungroup_vertex(K_EDITOR * p_cnf, int cislo)
{
  if (p_cnf->groupnum) {
    if (cislo + 1 < p_cnf->groupnum)
      memmove(p_cnf->p_group + cislo, p_cnf->p_group + cislo + 1,
        (p_cnf->groupnum - cislo - 1) * sizeof(p_cnf->p_group[0]));
    p_cnf->groupnum--;
  }
  if (!p_cnf->groupnum) {
    free(p_cnf->p_group);
    p_cnf->p_group = NULL;
  }
}

void oe_prikaz_ungroup_aktualni_vertex(K_EDITOR * p_cnf)
{
  oe_ungroup_vertex(p_cnf, p_cnf->vybr_group);
  p_cnf->vybr_group = 0;
}

void oe_prikaz_ungroup_group_vertex(K_EDITOR * p_cnf)
{
  I_VERTEX *p_group;
  int groupnum, i, v;

  oe_vybrani_ctverce(p_cnf, 0x0000ff);
  oe_vrat_vertexy_ze_ctverce(p_cnf, &p_group, &groupnum);

  for (i = 0; i < groupnum; i++) {
    v =
      oe_je_vertex_v_seznamu(p_cnf, p_group[i].k, p_group[i].o, p_group[i].v);
    if (v)
      oe_ungroup_vertex(p_cnf, v - 1);
  }
  p_cnf->vybr_group = 0;
}

void oe_prikaz_ungroup_vertex(K_EDITOR * p_cnf)
{
  I_VERTEX *p_group;
  int groupnum, i, v;

  oe_vybrani_konstantniho_ctverce(p_cnf);
  oe_vrat_vertexy_ze_ctverce(p_cnf, &p_group, &groupnum);

  for (i = 0; i < groupnum; i++) {
    v =
      oe_je_vertex_v_seznamu(p_cnf, p_group[i].k, p_group[i].o, p_group[i].v);
    if (v)
      oe_ungroup_vertex(p_cnf, v - 1);
  }
  p_cnf->vybr_group = 0;
}


void oe_pis_vybrany_vertex(K_EDITOR * p_cnf)
{
  int g, v, k, o;

  if (!p_cnf->groupnum)
    return;

  g = p_cnf->vybr_group;
  v = p_cnf->p_group[g].v;
  k = p_cnf->p_group[g].k;
  o = p_cnf->p_group[g].o;

  //printfs(p_con,XK,YK,"Vybranych vertexu %d g[%d]",p_cnf->groupnum,g);
  //printfs(p_con,XK,YK,"g[%d], v[%d]",g,v);
  //printfs(p_con,XK,YK+1,"vertex %d %s\\%s",v,p_cnf->jmena_kont[k],p_cnf->p_kont[k]->objekty[o]);
  //printfs(p_con,XK,YK+1,"vertex %d",v);

  //printfs(p_con,XK,YK+2,"S = [ %f, %f, %f]",p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].x,p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].y,p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].z);
  //printfs(p_con,XK,YK+3,"T1 = [ %f, %f]",p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].tu1,p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].tv1);
  //printfs(p_con,XK,YK+4,"T2 = [ %f, %f]",p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].tu2,p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].tv2);
  //printfs(p_con,XK,YK+5,"N = [ %f, %f, %f]",p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].nx,p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].ny,p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].nz);
}


void oe_prikaz_vyber_vertexu_nahoru(K_EDITOR * p_cnf)
{
  if (!p_cnf->groupnum)
    return;

  if (--p_cnf->vybr_group < 0)
    p_cnf->vybr_group = p_cnf->groupnum - 1;

  //oe_smaz_malou_konzoli(p_con);
  oe_pis_vybrany_vertex(p_cnf);
}

void oe_prikaz_vyber_vertexu_dolu(K_EDITOR * p_cnf)
{
  if (!p_cnf->groupnum)
    return;

  if (++p_cnf->vybr_group == p_cnf->groupnum)
    p_cnf->vybr_group = 0;

  //oe_smaz_malou_konzoli(p_con);
  oe_pis_vybrany_vertex(p_cnf);
}


void oe_edit_vertex_xyz(K_EDITOR * p_cnf, int i, byte * p_flag, float *p_f)
{
  int k, o, v;

  v = p_cnf->p_group[i].v;
  k = p_cnf->p_group[i].k;
  o = p_cnf->p_group[i].o;

  if (p_flag[0])
    p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].x = p_f[0];
  if (p_flag[1])
    p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].y = p_f[1];
  if (p_flag[2])
    p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].z = p_f[2];
}

void oe_nasob_vertex_xyz(K_EDITOR * p_cnf, int i, float x, float y, float z)
{
  int k, o, v;

  v = p_cnf->p_group[i].v;
  k = p_cnf->p_group[i].k;
  o = p_cnf->p_group[i].o;

  p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].x *= x;
  p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].y *= y;
  p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].z *= z;
}

void oe_add_vertex_xyz(K_EDITOR * p_cnf, int i, byte * p_flag, float *p_f)
{
  int k, o, v;

  v = p_cnf->p_group[i].v;
  k = p_cnf->p_group[i].k;
  o = p_cnf->p_group[i].o;

  if (p_flag[0])
    p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].x += p_f[0];
  if (p_flag[1])
    p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].y += p_f[1];
  if (p_flag[2])
    p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].z += p_f[2];
}

void oe_edit_vertex_uv(K_EDITOR * p_cnf, int i, int t, byte * p_flag, float u,
  float t2)
{
  float *p_fl;
  int k, o, v;

  v = p_cnf->p_group[i].v;
  k = p_cnf->p_group[i].k;
  o = p_cnf->p_group[i].o;

  p_fl = &p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].tu1;
  if (p_flag[0])
    p_fl[0 + t * 2] = u;
  if (p_flag[1])
    p_fl[1 + t * 2] = t2;
}

void oe_nasob_vertex_uv(K_EDITOR * p_cnf, int i, int t, byte * p_flag,
  float u, float t2)
{
  float *p_fl;
  int k, o, v;

  v = p_cnf->p_group[i].v;
  k = p_cnf->p_group[i].k;
  o = p_cnf->p_group[i].o;

  p_fl = &p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].tu1;
  if (p_flag[0])
    p_fl[0 + t * 2] *= u;
  if (p_flag[1])
    p_fl[1 + t * 2] *= t2;
}

void oe_add_vertex_uv(K_EDITOR * p_cnf, int i, int t, byte * p_flag, float u,
  float t2)
{
  float *p_fl;
  int k, o, v;

  v = p_cnf->p_group[i].v;
  k = p_cnf->p_group[i].k;
  o = p_cnf->p_group[i].o;

  p_fl = &p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].tu1;
  if (p_flag[0])
    p_fl[0 + t * 2] += u;
  if (p_flag[1])
    p_fl[1 + t * 2] += t2;
}

void oe_prikaz_editace_group(K_EDITOR * p_cnf, float *p_f, byte * p_flag)
{
  int i;

  if (!p_cnf->groupnum)
    return;

  for (i = 0; i < p_cnf->groupnum; i++)
    oe_edit_vertex_xyz(p_cnf, i, p_flag, p_f);

  if (p_flag[3] || p_flag[4]) {
    for (i = 0; i < p_cnf->groupnum; i++)
      oe_edit_vertex_uv(p_cnf, i, 0, p_flag + 3, p_f[3], p_f[4]);
  }

  if (p_flag[5] || p_flag[6]) {
    for (i = 0; i < p_cnf->groupnum; i++)
      oe_edit_vertex_uv(p_cnf, i, 1, p_flag + 5, p_f[5], p_f[6]);
  }
}

void oe_prikaz_editace_vertex(K_EDITOR * p_cnf, float *p_f, byte * p_flag)
{
  int i;

  if (!p_cnf->groupnum)
    return;

  i = p_cnf->vybr_group;

  oe_edit_vertex_xyz(p_cnf, i, p_flag, p_f);

  if (p_flag[3] || p_flag[4]) {
    oe_edit_vertex_uv(p_cnf, i, 0, p_flag + 3, p_f[3], p_f[4]);
  }
  if (p_flag[5] || p_flag[6]) {
    oe_edit_vertex_uv(p_cnf, i, 1, p_flag + 5, p_f[5], p_f[6]);
  }
}

void oe_prikaz_nasobeni_group(K_EDITOR * p_cnf, float *p_f, byte * p_flag)
{
  int i;

  if (!p_cnf->groupnum)
    return;


  if (p_flag[0]) {
    for (i = 0; i < p_cnf->groupnum; i++)
      oe_nasob_vertex_xyz(p_cnf, i, p_f[0], p_f[1], p_f[2]);
  }
  if (p_flag[3] || p_flag[4]) {
    for (i = 0; i < p_cnf->groupnum; i++)
      oe_nasob_vertex_uv(p_cnf, i, 0, p_flag + 3, p_f[3], p_f[4]);
  }
  if (p_flag[5] || p_flag[6]) {
    for (i = 0; i < p_cnf->groupnum; i++)
      oe_nasob_vertex_uv(p_cnf, i, 1, p_flag + 5, p_f[5], p_f[6]);
  }
}

void oe_prikaz_nasobeni_vertex(K_EDITOR * p_cnf, float *p_f, byte * p_flag)
{
  if (!p_cnf->groupnum)
    return;


  if (p_flag[0]) {
    oe_nasob_vertex_xyz(p_cnf, p_cnf->vybr_group, p_f[0], p_f[1], p_f[2]);
  }
  if (p_flag[3] || p_flag[4]) {
    oe_nasob_vertex_uv(p_cnf, p_cnf->vybr_group, 0, p_flag + 3, p_f[3],
      p_f[4]);
  }
  if (p_flag[5] || p_flag[6]) {
    oe_nasob_vertex_uv(p_cnf, p_cnf->vybr_group, 1, p_flag + 5, p_f[5],
      p_f[6]);
  }
}

void oe_prikaz_add_group(K_EDITOR * p_cnf, float *p_f, byte * p_flag)
{
  int i;

  if (!p_cnf->groupnum)
    return;

  for (i = 0; i < p_cnf->groupnum; i++)
    oe_add_vertex_xyz(p_cnf, i, p_flag, p_f);

  if (p_flag[3] || p_flag[4]) {
    for (i = 0; i < p_cnf->groupnum; i++)
      oe_add_vertex_uv(p_cnf, i, 0, p_flag + 3, p_f[3], p_f[4]);
  }

  if (p_flag[5] || p_flag[6]) {
    for (i = 0; i < p_cnf->groupnum; i++)
      oe_add_vertex_uv(p_cnf, i, 1, p_flag + 5, p_f[5], p_f[6]);
  }
}

void oe_prikaz_add_vertex(K_EDITOR * p_cnf, float *p_f, byte * p_flag)
{
  int i;

  if (!p_cnf->groupnum)
    return;

  i = p_cnf->vybr_group;

  oe_add_vertex_xyz(p_cnf, i, p_flag, p_f);

  if (p_flag[3] || p_flag[4]) {
    oe_add_vertex_uv(p_cnf, i, 0, p_flag + 3, p_f[3], p_f[4]);
  }
  if (p_flag[5] || p_flag[6]) {
    oe_add_vertex_uv(p_cnf, i, 1, p_flag + 5, p_f[5], p_f[6]);
  }
}


void oe_prikaz_posun_group_vertex(K_EDITOR * p_cnf)
{
  DEKOVAC *p_deka = &p_cnf->deka;
  VEKTOR v1, v2;
  int k, o, b, i, mx1, my1, mx2, my2, flag, vybr, deka;
  GLMATRIX *p_mat;
  float vzdal;
  BOD rg, tr, tw;

  if (!p_cnf->groupnum)
    return;



  if ((p_mat =
      (GLMATRIX *) malloc(sizeof(GLMATRIX) * p_cnf->groupnum)) == NULL)
    chyba("Nedostatek pameti.");

  for (i = 0; i < p_cnf->groupnum; i++)
    calc_transf_3d_2d_matrix(kont_world_matrix(p_cnf->
        p_kont[p_cnf->p_group[i].k]), p_cnf->p_camera, p_cnf->p_project,
      p_mat + i);

  *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);

  // Vypocet referencniho vertexu
  deka = p_deka->aktivni;
  if (deka) {
    vybr = p_cnf->vybr_group;

    k = p_cnf->p_group[vybr].k;
    o = p_cnf->p_group[vybr].o;
    b = p_cnf->p_group[vybr].v;

    rg.x = p_cnf->p_kont[k]->p_obj[o]->p_vertex[b].x;
    rg.y = p_cnf->p_kont[k]->p_obj[o]->p_vertex[b].y;
    rg.z = p_cnf->p_kont[k]->p_obj[o]->p_vertex[b].z;
    transformuj_bod_bod_matici(&rg, kont_world_matrix(p_cnf->p_kont[k]));
  }

  kprintf(FALSE, "Posun vertexu...");

  mx1 = mi.x;
  my1 = mi.y;

  nuluj_mys();
  while (stisk_mys()) {

    mx2 = mi.x;
    my2 = mi.y;

    for (i = 0; i < p_cnf->groupnum; i++) {

      transf_2d_3d_z_matrix((float) mx1, (float) my1, *p_cnf->p_vzdal,
        &v1.x, &v1.y, &v1.z, p_mat + i, OXRES, OYRES, OXSTART, OYSTART,
        p_cnf->kam.near_plane, p_cnf->kam.far_plane);

      transf_2d_3d_z_matrix((float) mx2, (float) my2, *p_cnf->p_vzdal,
        &v2.x, &v2.y, &v2.z, p_mat + i, OXRES, OYRES, OXSTART, OYSTART,
        p_cnf->kam.near_plane, p_cnf->kam.far_plane);

      k = p_cnf->p_group[i].k;
      o = p_cnf->p_group[i].o;
      b = p_cnf->p_group[i].v;

      flag = 0;
      vektor_sub(&v2, &v1, &tr);

      if (deka && b != vybr) {
        tw.x = p_cnf->p_kont[k]->p_obj[o]->p_vertex[b].x;
        tw.y = p_cnf->p_kont[k]->p_obj[o]->p_vertex[b].y;
        tw.z = p_cnf->p_kont[k]->p_obj[o]->p_vertex[b].z;
        transformuj_bod_bod_matici(&tw, kont_world_matrix(p_cnf->p_kont[k]));

        vzdal = vzdal_bodu_bod(&tw, &rg);
        if (vzdal < p_deka->radius) {
          vzdal = 1.0f - (vzdal / p_deka->radius);
          vzdal *= p_deka->sila;
          if (vzdal > 1.0f)
            vzdal = 1.0f;
          else if (vzdal < 0.0f)
            vzdal = 0.0f;
          vektor_mult_skalar(&tr, vzdal, &tr);
        }
        else
          continue;
      }
      else {
        if (deka && b == vybr) {
          rg.x = p_cnf->p_kont[k]->p_obj[o]->p_vertex[b].x;
          rg.y = p_cnf->p_kont[k]->p_obj[o]->p_vertex[b].y;
          rg.z = p_cnf->p_kont[k]->p_obj[o]->p_vertex[b].z;
          transformuj_bod_bod_matici(&rg,
            kont_world_matrix(p_cnf->p_kont[k]));
        }
      }

      if (!key[K_X] && !key[K_Y] && !key[K_Z]) {
        p_cnf->p_kont[k]->p_obj[o]->p_vertex[b].x += tr.x;
        p_cnf->p_kont[k]->p_obj[o]->p_vertex[b].y += tr.y;
        p_cnf->p_kont[k]->p_obj[o]->p_vertex[b].z += tr.z;
        flag = POSUN_VSE;
      }
      else {
        if (key[K_X]) {
          p_cnf->p_kont[k]->p_obj[o]->p_vertex[b].x += tr.x;
          flag = POSUN_OSA_X;
        }
        if (key[K_Y]) {
          p_cnf->p_kont[k]->p_obj[o]->p_vertex[b].y += tr.y;
          flag = POSUN_OSA_X;
        }
        if (key[K_Z]) {
          p_cnf->p_kont[k]->p_obj[o]->p_vertex[b].z += tr.z;
          flag = POSUN_OSA_X;
        }
      }
    }

    k = p_cnf->p_group[p_cnf->vybr_group].k;
    o = p_cnf->p_group[p_cnf->vybr_group].o;
    b = p_cnf->p_group[p_cnf->vybr_group].v;

    oe_renderuj_scenu_pre(p_cnf);
    oe_kresli_editacni_stred(p_cnf,
      (BOD *) & p_cnf->p_kont[k]->p_obj[o]->p_vertex[b].x, flag);
    oe_renderuj_scenu_po(p_cnf);

    mx1 = mi.x;
    my1 = mi.y;
  }

  free(p_mat);

  *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);
}

void oe_prikaz_posun_vertex(K_EDITOR * p_cnf)
{
  VEKTOR v1, v2;
  int k, o, b, i, flag;
  GLMATRIX mat, *p_world;

  if (!p_cnf->groupnum)
    return;

  i = p_cnf->vybr_group;
  k = p_cnf->p_group[i].k;
  o = p_cnf->p_group[i].o;
  b = p_cnf->p_group[i].v;

  p_world = kont_world_matrix(p_cnf->p_kont[k]);
  calc_transf_3d_2d_matrix(p_world, p_cnf->p_camera, p_cnf->p_project, &mat);

  *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);


  transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
    &v1.x, &v1.y, &v1.z, &mat,
    OXRES, OYRES, OXSTART, OYSTART,
    p_cnf->kam.near_plane, p_cnf->kam.far_plane);
  nuluj_mys();
  while (stisk_mys()) {
    transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
      &v2.x, &v2.y, &v2.z, &mat,
      OXRES, OYRES, OXSTART, OYSTART,
      p_cnf->kam.near_plane, p_cnf->kam.far_plane);
    flag = 0;
    if (!key[K_X] && !key[K_Y] && !key[K_Z]) {
      p_cnf->p_kont[k]->p_obj[o]->p_vertex[b].x += v2.x - v1.x;
      p_cnf->p_kont[k]->p_obj[o]->p_vertex[b].y += v2.y - v1.y;
      p_cnf->p_kont[k]->p_obj[o]->p_vertex[b].z += v2.z - v1.z;
      flag = POSUN_VSE;
    }
    else {
      if (key[K_X]) {
        p_cnf->p_kont[k]->p_obj[o]->p_vertex[b].x += v2.x - v1.x;
        flag = POSUN_OSA_X;
      }
      if (key[K_Y]) {
        p_cnf->p_kont[k]->p_obj[o]->p_vertex[b].y += v2.y - v1.y;
        flag = POSUN_OSA_Y;
      }
      if (key[K_Z]) {
        p_cnf->p_kont[k]->p_obj[o]->p_vertex[b].z += v2.z - v1.z;
        flag = POSUN_OSA_Z;
      }
    }

    oe_renderuj_scenu_pre(p_cnf);
    oe_kresli_editacni_stred(p_cnf,
      (BOD *) & p_cnf->p_kont[k]->p_obj[o]->p_vertex[b].x, flag);
    oe_renderuj_scenu_po(p_cnf);

    transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
      &v1.x, &v1.y, &v1.z, &mat,
      OXRES, OYRES, OXSTART, OYSTART,
      p_cnf->kam.near_plane, p_cnf->kam.far_plane);
  }

  *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);
}

void oe_prikaz_zvedni_vertex(K_EDITOR * p_cnf)
{
  int k, o, b, i;

  if (!p_cnf->groupnum)
    return;



  i = p_cnf->vybr_group;

  *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);

  //oe_hlaska_konzole(p_con,"Zvednuti jednoho vertexu...",0);

  nuluj_mys();
  while (stisk_mys()) {
    k = p_cnf->p_group[i].k;
    o = p_cnf->p_group[i].o;
    b = p_cnf->p_group[i].v;

    if (key[K_CTRL])
      p_cnf->p_kont[k]->p_obj[o]->p_vertex[b].y -= mi.dy * 0.2f;
    else
      p_cnf->p_kont[k]->p_obj[o]->p_vertex[b].y -= mi.dy * 0.01f;

    nuluj_dif();

    oe_renderuj_scenu_pre(p_cnf);
    oe_renderuj_scenu_po(p_cnf);
  }

  *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);
  //oe_smaz_malou_konzoli(p_con);
}

void oe_prikaz_zvedni_group_vertex(K_EDITOR * p_cnf)
{
  int k, o, b, i;

  if (!p_cnf->groupnum)
    return;


  *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);

  //oe_hlaska_konzole(p_con,"Zvednuti vertexu...",0);

  nuluj_mys();
  while (stisk_mys()) {
    for (i = 0; i < p_cnf->groupnum; i++) {
      k = p_cnf->p_group[i].k;
      o = p_cnf->p_group[i].o;
      b = p_cnf->p_group[i].v;
      if (key[K_CTRL])
        p_cnf->p_kont[k]->p_obj[o]->p_vertex[b].y -= mi.dy * 0.05f;
      else
        p_cnf->p_kont[k]->p_obj[o]->p_vertex[b].y -= mi.dy * 0.01f;
    }
    nuluj_dif();

    oe_renderuj_scenu_pre(p_cnf);
    oe_renderuj_scenu_po(p_cnf);
  }

  *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);
}

// Vyrovna group vertex - na uroven levelu/vybraneho vertexu
void oe_prikaz_vyrovnej_group_vertex(K_EDITOR * p_cnf, int level)
{
  int lk = -1, k, o, b, i, v, refk = -1;
  float vyska;

  if (!p_cnf->groupnum)
    return;

  if (level) {
    vyska = p_cnf->bnf.y_start;
    kprintf(FALSE, "Rovnam vertexy podle levelu");
  }
  else {
    v = p_cnf->vybr_group;
    refk = k = p_cnf->p_group[v].k;
    o = p_cnf->p_group[v].o;
    b = p_cnf->p_group[v].v;
    transformuj_kontejner(p_cnf->p_kont[k]);
    vyska = p_cnf->p_kont[k]->p_obj[o]->p_vertex[b].y;
    kprintf(FALSE, "Rovnam vertexy podle vybr. vertexu");
  }

  for (i = 0; i < p_cnf->groupnum; i++) {
    k = p_cnf->p_group[i].k;
    o = p_cnf->p_group[i].o;
    b = p_cnf->p_group[i].v;
    if (k != lk && k != refk) {
      transformuj_kontejner(p_cnf->p_kont[k]);
      lk = k;
    }
    p_cnf->p_kont[k]->p_obj[o]->p_vertex[b].y = vyska;
  }
}

void oe_eliminuj_stejne_group(K_EDITOR * p_cnf)
{
  int k, i, j, o, v;
  int zk, zo, zv;
  float x, y, z;
  float vx, vy, vz;

  if (!p_cnf->groupnum)
    return;

  i = p_cnf->vybr_group;
  o = p_cnf->p_group[i].o;
  v = p_cnf->p_group[i].v;
  k = p_cnf->p_group[i].k;
  vx = p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].x;
  vy = p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].y;
  vz = p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].z;

  for (j = 0; j < p_cnf->groupnum; j++) {
    zv = p_cnf->p_group[j].v;
    zo = p_cnf->p_group[j].o;
    zk = p_cnf->p_group[j].k;
    x = p_cnf->p_kont[zk]->p_obj[zo]->p_vertex[zv].x;
    y = p_cnf->p_kont[zk]->p_obj[zo]->p_vertex[zv].y;
    z = p_cnf->p_kont[zk]->p_obj[zo]->p_vertex[zv].z;

    for (i = 0; i < p_cnf->groupnum; i++) {
      o = p_cnf->p_group[i].o;
      v = p_cnf->p_group[i].v;
      k = p_cnf->p_group[i].k;

      if (v == zv && o == zo && k == zk)
        continue;

      if (x == p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].x &&
        y == p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].y &&
        z == p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].z) {
        oe_ungroup_vertex(p_cnf, i--);
      }
    }
  }

  for (j = 0; j < p_cnf->groupnum; j++) {
    o = p_cnf->p_group[j].o;
    v = p_cnf->p_group[j].v;
    k = p_cnf->p_group[j].k;
    x = p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].x;
    y = p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].y;
    z = p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].z;
    if (x == vx && y == vy && z == vz) {
      p_cnf->vybr_group = j;
      break;
    }
  }
}


void oe_vyber_nejblizsi_group_vertex(K_EDITOR * p_cnf)
{
  // vybere vertex z group nejblize mysi
  // 1. transformace vsech group do 2d
  // 2. najit nejblizsi
  GLMATRIX m;
  int i, k, o, v;
  int x, y, transform = 0, g = 0;
  float min_vzdal = 0xfffff, p, z;

  if (!p_cnf->groupnum)
    return;

  for (i = 0; i < p_cnf->groupnum; i++) {
    if (k != p_cnf->p_group[i].k) {
      k = p_cnf->p_group[i].k;
      transform++;
    }
    o = p_cnf->p_group[i].o;
    v = p_cnf->p_group[i].v;
    if (transform) {
      calc_3d_2d_matrix(kont_world_matrix(p_cnf->p_kont[k]),
        p_cnf->p_camera, p_cnf->p_project, &m);
      transform = 0;
    }
    if (transf_3d_2d_matrix(p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].x,
        p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].y,
        p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].z,
        &x, &y, &z, &m, OXRES, OYRES, OXSTART, OYSTART,
        p_cnf->kam.near_plane, p_cnf->kam.far_plane)) {
      p = (float) vzdal_bodu_2D_xy(x, y, mi.x, mi.y);
      if (min_vzdal > p) {
        min_vzdal = p;
        g = i;
      }
    }
  }
  p_cnf->vybr_group = g;
}

void oe_smaz_flare(K_EDITOR * p_cnf, LENS_FLARE * p_flare)
{
  p_flare->akt = FALSE;
  null_free(&p_flare->p_sloz);
  if (p_flare->p_svetlo) {
    (LENS_FLARE *) (p_flare->p_svetlo)->p_flare = NULL;
  }
}

STATIC_LIGHT *oe_kopiruj_slight(K_EDITOR * p_cnf, STATIC_LIGHT * p_svetlo)
{
  int l = oe_najdi_volne_stat_svetlo(p_cnf);

  /* Kopie svetla
   */
  p_cnf->light[l] = *p_svetlo;
  p_cnf->light[l].index = l;
  p_cnf->lightakt = l;

  /* Kopie flare
   */
  if (p_svetlo->p_flare) {
    p_cnf->light[l].p_flare = lo_kopiruj_flare(p_cnf->flare, MAX_EDIT_SVETEL,
      p_svetlo->p_flare);
    ((LENS_FLARE *) (p_cnf->light[l].p_flare))->p_svetlo = p_cnf->light + l;
  }

  return (p_cnf->light + l);
}

/*
*/

/*
  Flare
*/
void oe_prikaz_posun_flare(K_EDITOR * p_cnf)
{
  LENS_FLARE *p_flare;
  VEKTOR v1, v2;
  GLMATRIX mat;

  // kopie flaru
  if (key[K_CTRL]) {
    key[K_CTRL] = FALSE;
    lo_kopiruj_flare(p_cnf->flare, MAX_EDIT_SVETEL,
      p_cnf->flare + p_cnf->flareakt);
  }

  init_matrix(&mat);

  p_flare = p_cnf->flare + p_cnf->flareakt;
  p_flare->p_svetlo = NULL;

  calc_transf_3d_2d_matrix(&mat, p_cnf->p_camera, p_cnf->p_project, &mat);
  *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);
  transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
    &v1.x, &v1.y, &v1.z, &mat, OXRES, OYRES, OXSTART, OYSTART,
    p_cnf->kam.near_plane, p_cnf->kam.far_plane);

  kprintf(0, "Posun flare...");

  nuluj_mys();
  while (stisk_mys()) {
    transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
      &v2.x, &v2.y, &v2.z, &mat, OXRES, OYRES, OXSTART, OYSTART,
      p_cnf->kam.near_plane, p_cnf->kam.far_plane);

    p_flare->rflag = FALSE;
    if (!key[K_X] && !key[K_Y] && !key[K_Z]) {
      p_flare->p.x += v2.x - v1.x;
      p_flare->p.y += v2.y - v1.y;
      p_flare->p.z += v2.z - v1.z;
      p_flare->rflag = POSUN_VSE;
    }
    else {
      if (key[K_X]) {
        p_flare->p.x += v2.x - v1.x;
        p_flare->rflag = POSUN_OSA_X;
      }
      if (key[K_Y]) {
        p_flare->p.y += v2.y - v1.y;
        p_flare->rflag = POSUN_OSA_Y;
      }
      if (key[K_Z]) {
        p_flare->p.z += v2.z - v1.z;
        p_flare->rflag = POSUN_OSA_Z;
      }
    }

    nuluj_dif();
    oe_renderuj_scenu_pre(p_cnf);
    oe_renderuj_scenu_po(p_cnf);

    transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
      &v1.x, &v1.y, &v1.z, &mat, OXRES, OYRES, OXSTART, OYSTART,
      p_cnf->kam.near_plane, p_cnf->kam.far_plane);
  }
  *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);
  kprintf(0, "OK");
}

/*
  Posun svetla
*/
void oe_prikaz_posun_staticke_svetlo(K_EDITOR * p_cnf)
{
  STATIC_LIGHT *p_light;
  VEKTOR v1, v2;
  GLMATRIX mat;

  kprintf(0, "Posun svetla...");

  // kopie svetla
  if (key[K_CTRL]) {
    oe_kopiruj_slight(p_cnf, p_cnf->light + p_cnf->lightakt);
    sprintf(p_cnf->light[p_cnf->lightakt].jmeno, "Svetlo %d",
      p_cnf->lightakt);
    key[K_CTRL] = FALSE;
  }

  p_light = p_cnf->light + p_cnf->lightakt;

  if (p_light->flag2 & SL2_MESH)
    mat_copy(&mat, kont_world_matrix(p_light->p_mesh_data));
  else {
    init_matrix(&mat);
  }

  calc_transf_3d_2d_matrix(&mat, p_cnf->p_camera, p_cnf->p_project, &mat);
  *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);
  transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
    &v1.x, &v1.y, &v1.z, &mat, OXRES, OYRES, OXSTART, OYSTART,
    p_cnf->kam.near_plane, p_cnf->kam.far_plane);

  nuluj_mys();
  while (stisk_mys()) {
    transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
      &v2.x, &v2.y, &v2.z, &mat, OXRES, OYRES, OXSTART, OYSTART,
      p_cnf->kam.near_plane, p_cnf->kam.far_plane);

    p_light->flag = FALSE;
    if (!key[K_X] && !key[K_Y] && !key[K_Z]) {
      p_light->p.x += v2.x - v1.x;
      p_light->p.y += v2.y - v1.y;
      p_light->p.z += v2.z - v1.z;
      p_light->flag = POSUN_VSE;
    }
    else {
      if (key[K_X]) {
        p_light->p.x += v2.x - v1.x;
        p_light->flag = POSUN_OSA_X;
      }
      if (key[K_Y]) {
        p_light->p.y += v2.y - v1.y;
        p_light->flag = POSUN_OSA_Y;
      }
      if (key[K_Z]) {
        p_light->p.z += v2.z - v1.z;
        p_light->flag = POSUN_OSA_Z;
      }
    }

    if (p_cnf->cnf.svetla_dynamicke) {
      ray_prepocitej_staticke_osvetleni_vertexy(p_cnf);
    }

    nuluj_dif();
    oe_renderuj_scenu_pre(p_cnf);
    oe_renderuj_scenu_po(p_cnf);

    transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
      &v1.x, &v1.y, &v1.z, &mat, OXRES, OYRES, OXSTART, OYSTART,
      p_cnf->kam.near_plane, p_cnf->kam.far_plane);
  }
  *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);
  p_light->flag = POSUN_VSE;
  kprintf(0, "OK");
}

void oe_prikaz_scale_staticke_svetlo(K_EDITOR * p_cnf)
{
  STATIC_LIGHT *p_light;
  VEKTOR v1, v2, vysl;
  GLMATRIX mat;
  float dosah, nas;

  // kopie svetla
  init_matrix(&mat);

  p_light = p_cnf->light + p_cnf->lightakt;
  calc_transf_3d_2d_matrix(&mat, p_cnf->p_camera, p_cnf->p_project, &mat);
  *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);
  transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
    &v1.x, &v1.y, &v1.z, &mat, OXRES, OYRES, OXSTART, OYSTART,
    p_cnf->kam.near_plane, p_cnf->kam.far_plane);

  dosah = p_light->dosah;
  nuluj_mys();
  while (stisk_mys()) {
    transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
      &v2.x, &v2.y, &v2.z, &mat, OXRES, OYRES, OXSTART, OYSTART,
      p_cnf->kam.near_plane, p_cnf->kam.far_plane);

    vysl.x = v2.x - v1.x;
    vysl.y = v2.y - v1.y;
    vysl.z = v2.z - v1.z;
    nas = (mi.dx < 0) ? -1.0f : 1.0f;
    p_light->dosah += nas * vektor_velikost(&vysl);

    if (key[K_ESC]) {
      key[K_ESC] = FALSE;
      p_light->dosah = dosah;
      break;
    }

    if (p_cnf->cnf.svetla_dynamicke) {
      ray_prepocitej_staticke_osvetleni_vertexy(p_cnf);
    }

    nuluj_dif();
    oe_renderuj_scenu_pre(p_cnf);
    oe_renderuj_scenu_po(p_cnf);

    transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
      &v1.x, &v1.y, &v1.z, &mat, OXRES, OYRES, OXSTART, OYSTART,
      p_cnf->kam.near_plane, p_cnf->kam.far_plane);
  }
  *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);
}


/*
  Posun stredu
*/
void oe_prikaz_posun_stredu_levelu(K_EDITOR * p_cnf)
{
  ELIMINATOR *p_el = &p_cnf->elm;
  GLMATRIX mat;
  BOD v1, v2;

  if (!(p_el->flag & ELIM_STRED)) {
    kprintf(0, "Stred neni aktivni !");
  }
  else {
    kprintf(0, "Posun stredu...");

    init_matrix(&mat);

    calc_transf_3d_2d_matrix(&mat, p_cnf->p_camera, p_cnf->p_project, &mat);
    *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);
    transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
      &v1.x, &v1.y, &v1.z, &mat, OXRES, OYRES, OXSTART, OYSTART,
      p_cnf->kam.near_plane, p_cnf->kam.far_plane);

    nuluj_mys();
    while (stisk_mys()) {
      transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
        &v2.x, &v2.y, &v2.z, &mat, OXRES, OYRES, OXSTART, OYSTART,
        p_cnf->kam.near_plane, p_cnf->kam.far_plane);

      p_el->flag = FALSE;
      if (!key[K_X] && !key[K_Y] && !key[K_Z]) {
        p_el->p.x += v2.x - v1.x;
        p_el->p.y += v2.y - v1.y;
        p_el->p.z += v2.z - v1.z;
        p_el->flag = POSUN_VSE;
      }
      else {
        if (key[K_X]) {
          p_el->p.x += v2.x - v1.x;
          p_el->flag = POSUN_OSA_X;
        }
        if (key[K_Y]) {
          p_el->p.y += v2.y - v1.y;
          p_el->flag = POSUN_OSA_Y;
        }
        if (key[K_Z]) {
          p_el->p.z += v2.z - v1.z;
          p_el->flag = POSUN_OSA_Z;
        }
      }

      if (p_el->dlg_aktivni) {
        doe_printf(p_el->hwnd_dialog, IDC_EDIT1, "%.3f", p_el->p.x);
        doe_printf(p_el->hwnd_dialog, IDC_EDIT8, "%.3f", p_el->p.y);
        doe_printf(p_el->hwnd_dialog, IDC_EDIT9, "%.3f", p_el->p.z);
      }

      nuluj_dif();
      oe_renderuj_scenu_pre(p_cnf);
      oe_renderuj_scenu_po(p_cnf);

      transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
        &v1.x, &v1.y, &v1.z, &mat, OXRES, OYRES, OXSTART, OYSTART,
        p_cnf->kam.near_plane, p_cnf->kam.far_plane);
    }

    *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);
    p_el->flag = FALSE;
    kprintf(0, "OK");
  }
}

/*
  Posun mlho-kostky
*/
void oe_prikaz_posun_mlhokostky(K_EDITOR * p_cnf)
{
  MLZNA_KOSTKA *p_mlha;
  GLMATRIX mat;
  BOD v1, v2;

  if (!p_cnf->p_mlha_akt) {
    kprintf(0, "Mlhokostka neni aktivni !");
  }
  else {
    kprintf(0, "Posun mlhokostky...");
    p_mlha = p_cnf->p_mlha_akt;

    init_matrix(&mat);

    calc_transf_3d_2d_matrix(&mat, p_cnf->p_camera, p_cnf->p_project, &mat);
    *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);
    transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
      &v1.x, &v1.y, &v1.z, &mat, OXRES, OYRES, OXSTART, OYSTART,
      p_cnf->kam.near_plane, p_cnf->kam.far_plane);

    nuluj_mys();
    while (stisk_mys()) {
      transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
        &v2.x, &v2.y, &v2.z, &mat, OXRES, OYRES, OXSTART, OYSTART,
        p_cnf->kam.near_plane, p_cnf->kam.far_plane);

      p_mlha->flag = FALSE;
      if (!key[K_X] && !key[K_Y] && !key[K_Z]) {
        p_mlha->min.x += v2.x - v1.x;
        p_mlha->min.y += v2.y - v1.y;
        p_mlha->min.z += v2.z - v1.z;
        p_mlha->max.x += v2.x - v1.x;
        p_mlha->max.y += v2.y - v1.y;
        p_mlha->max.z += v2.z - v1.z;
        p_mlha->flag = POSUN_VSE;
      }
      else {
        if (key[K_X]) {
          p_mlha->min.x += v2.x - v1.x;
          p_mlha->max.x += v2.x - v1.x;
          p_mlha->flag = POSUN_OSA_X;
        }
        if (key[K_Y]) {
          p_mlha->min.y += v2.y - v1.y;
          p_mlha->max.y += v2.y - v1.y;
          p_mlha->flag = POSUN_OSA_Y;
        }
        if (key[K_Z]) {
          p_mlha->min.z += v2.z - v1.z;
          p_mlha->max.z += v2.z - v1.z;
          p_mlha->flag = POSUN_OSA_Z;
        }
      }

      nuluj_dif();
      oe_renderuj_scenu_pre(p_cnf);
      oe_renderuj_scenu_po(p_cnf);

      transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
        &v1.x, &v1.y, &v1.z, &mat, OXRES, OYRES, OXSTART, OYSTART,
        p_cnf->kam.near_plane, p_cnf->kam.far_plane);
    }

    *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);
    p_mlha->flag = FALSE;
    kprintf(0, "OK");
  }
}

/*
  Scale mlho-kostky
*/
void oe_prikaz_scale_mlhokostky(K_EDITOR * p_cnf)
{
  MLZNA_KOSTKA *p_mlha;
  BOD v;
  float scale;

  if (!p_cnf->p_mlha_akt) {
    kprintf(0, "Mlhokostka neni aktivni !");
  }
  else {
    kprintf(0, "Scale mlhokostky...");
    p_mlha = p_cnf->p_mlha_akt;


    nuluj_mys();
    while (stisk_mys()) {

      scale =
        mi.dx * 0.002f * vektor_velikost(vektor_sub(&p_mlha->max,
          &p_mlha->min, &v));

      p_mlha->flag = FALSE;
      if (!key[K_X] && !key[K_Y] && !key[K_Z]) {
        p_mlha->min.x -= scale;
        p_mlha->min.y -= scale;
        p_mlha->min.z -= scale;
        p_mlha->max.x += scale;
        p_mlha->max.y += scale;
        p_mlha->max.z += scale;
        p_mlha->flag = POSUN_VSE;
      }
      else {
        if (key[K_X]) {
          p_mlha->min.x -= scale;
          p_mlha->max.x += scale;
          p_mlha->flag = POSUN_OSA_X;
        }
        if (key[K_Y]) {
          p_mlha->min.y -= scale;
          p_mlha->max.y += scale;
          p_mlha->flag = POSUN_OSA_Y;
        }
        if (key[K_Z]) {
          p_mlha->min.z -= scale;
          p_mlha->max.z += scale;
          p_mlha->flag = POSUN_OSA_Z;
        }
      }

      nuluj_dif();
      oe_renderuj_scenu_pre(p_cnf);
      oe_renderuj_scenu_po(p_cnf);
    }
    p_mlha->flag = FALSE;
    kprintf(0, "OK");
  }
}

/* Kamero-kostka
*/
void oe_prikaz_posun_bb_kamery(K_EDITOR * p_cnf)
{
  NASTAVENI_KAMER *p_kam;
  GLMATRIX mat;
  BOD v1, v2, s;
  float vzdal;

  if (!p_cnf->cnf.kamera_kostka_kreslit) {
    kprintf(0, "BB Kamery neni aktivni !");
  }
  else {
    kprintf(0, "Posun BB Kamery...");
    p_kam = &p_cnf->kam;

    init_matrix(&mat);
    calc_transf_3d_2d_matrix(&mat, p_cnf->p_camera, p_cnf->p_project, &mat);

    kd_stred_bunky(&p_kam->min, &p_kam->max, &s);
    vzdal = vzdal_bodu_bod(&s, (BOD *) & p_cnf->p_camera->_41);
    transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, vzdal,
      &v1.x, &v1.y, &v1.z, &mat, OXRES, OYRES, OXSTART, OYSTART,
      p_cnf->kam.near_plane, p_cnf->kam.far_plane);

    nuluj_mys();
    while (stisk_mys()) {
      transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, vzdal,
        &v2.x, &v2.y, &v2.z, &mat, OXRES, OYRES, OXSTART, OYSTART,
        p_cnf->kam.near_plane, p_cnf->kam.far_plane);

      p_kam->flag = FALSE;
      if (!key[K_X] && !key[K_Y] && !key[K_Z]) {
        p_kam->min.x += v2.x - v1.x;
        p_kam->min.y += v2.y - v1.y;
        p_kam->min.z += v2.z - v1.z;
        p_kam->max.x += v2.x - v1.x;
        p_kam->max.y += v2.y - v1.y;
        p_kam->max.z += v2.z - v1.z;
        p_kam->flag = POSUN_VSE;
      }
      else {
        if (key[K_X]) {
          p_kam->min.x += v2.x - v1.x;
          p_kam->max.x += v2.x - v1.x;
          p_kam->flag = POSUN_OSA_X;
        }
        if (key[K_Y]) {
          p_kam->min.y += v2.y - v1.y;
          p_kam->max.y += v2.y - v1.y;
          p_kam->flag = POSUN_OSA_Y;
        }
        if (key[K_Z]) {
          p_kam->min.z += v2.z - v1.z;
          p_kam->max.z += v2.z - v1.z;
          p_kam->flag = POSUN_OSA_Z;
        }
      }

      nuluj_dif();
      oe_renderuj_scenu_pre(p_cnf);
      oe_renderuj_scenu_po(p_cnf);

      transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, vzdal,
        &v1.x, &v1.y, &v1.z, &mat, OXRES, OYRES, OXSTART, OYSTART,
        p_cnf->kam.near_plane, p_cnf->kam.far_plane);
    }

    *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);
    p_kam->flag = FALSE;
    kprintf(0, "OK");
  }
}

/*
  Scale mlho-kostky
*/
void oe_prikaz_scale_bb_kamery(K_EDITOR * p_cnf)
{
  NASTAVENI_KAMER *p_kam;
  BOD v;
  float scale;

  if (!p_cnf->cnf.kamera_kostka_kreslit) {
    kprintf(0, "BB Kamery neni aktivni !");
  }
  else {
    kprintf(0, "Scale BB Kamery...");
    p_kam = &p_cnf->kam;

    nuluj_mys();
    while (stisk_mys()) {

      scale =
        mi.dx * 0.002f * vektor_velikost(vektor_sub(&p_kam->max, &p_kam->min,
          &v));

      p_kam->flag = FALSE;
      if (!key[K_X] && !key[K_Y] && !key[K_Z]) {
        p_kam->min.x -= scale;
        p_kam->min.y -= scale;
        p_kam->min.z -= scale;
        p_kam->max.x += scale;
        p_kam->max.y += scale;
        p_kam->max.z += scale;
        p_kam->flag = POSUN_VSE;
      }
      else {
        if (key[K_X]) {
          p_kam->min.x -= scale;
          p_kam->max.x += scale;
          p_kam->flag = POSUN_OSA_X;
        }
        if (key[K_Y]) {
          p_kam->min.y -= scale;
          p_kam->max.y += scale;
          p_kam->flag = POSUN_OSA_Y;
        }
        if (key[K_Z]) {
          p_kam->min.z -= scale;
          p_kam->max.z += scale;
          p_kam->flag = POSUN_OSA_Z;
        }
      }

      nuluj_dif();
      oe_renderuj_scenu_pre(p_cnf);
      oe_renderuj_scenu_po(p_cnf);
    }
    p_kam->flag = FALSE;
    kprintf(0, "OK");
  }
}


#define X_OKOLI_CUBE 20000.0f
#define Y_OKOLI_CUBE 10000.0f
#define Z_OKOLI_CUBE 20000.0f
#define KILOMETR     1000.0f

void oe_vyrob_okoli(K_EDITOR * p_cnf)
{
  BOD p = { 0, 0, 0 }, d = {
  X_OKOLI_CUBE / KILOMETR, Y_OKOLI_CUBE / KILOMETR, Z_OKOLI_CUBE / KILOMETR};
  int k = lo_najdi_volny_kontejner(p_cnf->p_kont, p_cnf->max_kont);

  if (doe_uprava_okoli(&d, &p)) {
    d.x *= KILOMETR;
    d.y *= KILOMETR;
    d.z *= KILOMETR;
    p.x *= KILOMETR;
    p.y *= KILOMETR;
    p.z *= KILOMETR;
    p_cnf->p_kont[k] = vyrob_krychli_rozlozenou(&p, d.x, d.y, d.z);
    invertuj_face_kontejner(p_cnf->p_kont[k]);  // invert kvuli culingu facu
    strcpy(p_cnf->p_kont[k]->jmeno, "Okoli levelu");
    p_cnf->p_kont[k]->kflag |= KONT_NO_FOG;     // kreslit naposledy a bez cullingu a mlhy   
  }
}

void oe_uprav_okoli(K_EDITOR * p_cnf)
{
  BOD p = { 0, 0, 0 }, d = {
  X_OKOLI_CUBE / KILOMETR, Y_OKOLI_CUBE / KILOMETR, Z_OKOLI_CUBE / KILOMETR};
  int k = p_cnf->vybr_kont;

  n_kont(k);

  if (doe_uprava_okoli(&d, &p)) {
    zrus_kontejner_rec(p_cnf->p_kont + k, &p_cnf->p_mlha);
    d.x *= KILOMETR;
    d.y *= KILOMETR;
    d.z *= KILOMETR;
    p.x *= KILOMETR;
    p.y *= KILOMETR;
    p.z *= KILOMETR;
    p_cnf->p_kont[k] = vyrob_krychli_rozlozenou(&p, d.x, d.y, d.z);
    invertuj_face_kontejner(p_cnf->p_kont[k]);
    strcpy(p_cnf->p_kont[k]->jmeno, "Okoli levelu");
    p_cnf->p_kont[k]->kflag |= KONT_NO_FOG;
  }
}

void oe_tiskni_akt_objekt(K_EDITOR * p_cnf)
{
  EDIT_OBJEKT *p_obj;
  FILE *f;
  byte soubor[200];
  int v;

  n_objekt(p_cnf->vybr_kont, p_cnf->vybr_objekt);
  sprintf(soubor, "e:\\obj%.5d.txt", p_cnf->vybr_objekt);
  if ((f = fopen(soubor, "w")) == NULL)
    return;
  p_obj = p_cnf->p_kont[p_cnf->vybr_kont]->p_obj[p_cnf->vybr_objekt];
  for (v = 0; v < p_obj->vertexnum; v++) {
    fprintf(f, "V %.3d u = %.3f v = %.3f\n", v, p_obj->p_vertex[v].tu1,
      p_obj->p_vertex[v].tv1);
  }
  fclose(f);
}

int oe_serad_group_compare(const int *p_grp1, const int *p_grp2)
{
  if (((I_VERTEX *) p_grp1)->k < ((I_VERTEX *) p_grp2)->k)
    return (1);
  else if (((I_VERTEX *) p_grp1)->k > ((I_VERTEX *) p_grp2)->k)
    return (-1);
  else if (((I_VERTEX *) p_grp1)->o < ((I_VERTEX *) p_grp2)->o)
    return (1);
  else if (((I_VERTEX *) p_grp1)->o > ((I_VERTEX *) p_grp2)->o)
    return (-1);
  else if (((I_VERTEX *) p_grp1)->v < ((I_VERTEX *) p_grp2)->v)
    return (1);
  else if (((I_VERTEX *) p_grp1)->v > ((I_VERTEX *) p_grp2)->v)
    return (-1);
  else
    return (0);
}

void oe_serad_group(K_EDITOR * p_cnf)
{
  qsort((void *) p_cnf->p_group, (size_t) p_cnf->groupnum,
    (size_t) sizeof(p_cnf->p_group[0]), oe_serad_group_compare);
}


/************************************************************************
    Face group
  ***********************************************************************
*/

/*
  Tmp kod na serazeni group facu
static int serad_group_face_compare(const int *p_grp1,const int *p_grp2)
{
  int k;
  k = ((F_VERTEX *)p_grp2)->k - ((F_VERTEX *)p_grp1)->k;
  if(k) 
    return(k);
  k = ((F_VERTEX *)p_grp2)->o - ((F_VERTEX *)p_grp1)->o;
  if(k) 
    return(k);
  k = ((F_VERTEX *)p_grp2)->f - ((F_VERTEX *)p_grp1)->f;
  return(k);
}

static void serad_group_face(F_VERTEX *p_new, int num)
{  
  qsort((void *)p_new,(size_t)num,(size_t)sizeof(p_new[0]), serad_group_face_compare);
}

// odstrunuje duplicitu v seznamu
static void odstran_duplicitu(F_VERTEX *p_new, int *p_num)
{
  F_VERTEX *p_last = p_new;
  int i;

  if(p_new && *p_num) {
    serad_group_face(p_new, *p_num);
    for(i = 1; i < *p_num; i++) {
      if(p_new[i].k == p_last->k && p_new[i].o == p_last->o && p_new[i].f == p_last->f) {
        memcpy(p_new+i,p_new+i+1,sizeof(p_new[0])*(*p_num-i-1));
        (*p_num)--;
        i--;
      }
    }
  }
}
*/

void oe_pridej_group_face_list_list(F_VERTEX ** p_group_face, int *p_groupnum,
  F_VERTEX * p_new, int num, int flag)
{
  F_VERTEX *p_group = *p_group_face;
  F_VERTEX *p_tmp;
  int i, groupnum = *p_groupnum;

  if (p_new) {
    i = groupnum + num;
    p_tmp = malloc(sizeof(p_tmp[0]) * i);
    mtest(p_tmp);

    // Smaze uz obsazene plosky v seznamu
    smaz_group_face_stejne_vertexy(p_group, groupnum, p_new, &num, flag);

    if (groupnum > 0) {
      memcpy(p_tmp, p_group, sizeof(p_tmp[0]) * groupnum);
    }
    memcpy(p_tmp + groupnum, p_new, sizeof(p_tmp[0]) * num);
    (*p_groupnum) += num;

    null_free(&p_group);
    *p_group_face = p_tmp;
  }
}

void oe_pridej_group_face_list(K_EDITOR * p_cnf, F_VERTEX * p_new, int num,
  int flag)
{
  oe_pridej_group_face_list_list(&p_cnf->p_group_face, &p_cnf->groupfacenum,
    p_new, num, flag);
}

void oe_ungroup_face_seznam(K_EDITOR * p_cnf, F_VERTEX * p_list, int num)
{
  int i, n;

  for (i = 0; i < num; i++) {
    n =
      najdi_group_face_vertex(p_cnf->p_group_face, p_cnf->groupfacenum,
      p_list + i, p_cnf->groupflag);
    if (n != K_CHYBA) {
      smaz_group_face_vertex(p_cnf->p_group_face, &p_cnf->groupfacenum, n);
    }
  }
  p_cnf->vybr_face_group = 0;
}

void oe_ungroup_face_ctverec(K_EDITOR * p_cnf)
{
  F_VERTEX *p_tmp;
  int num;

  oe_vybrani_ctverce(p_cnf, 0x0000ff);
  p_tmp = oe_vrat_face_ze_ctverce(p_cnf, &num);
  if (p_tmp)
    oe_ungroup_face_seznam(p_cnf, p_tmp, num);
}

void oe_ungroup_face_vybrany(K_EDITOR * p_cnf)
{
  smaz_group_face_vertex(p_cnf->p_group_face, &p_cnf->groupfacenum,
    p_cnf->vybr_face_group);
  p_cnf->vybr_face_group = 0;
}

void oe_ungroup_face_all(K_EDITOR * p_cnf)
{
  null_free(&p_cnf->p_group_face);
  p_cnf->vybr_face_group = 0;
  p_cnf->groupfacenum = 0;
}

__inline static F_VERTEX *najdi_face_k_vertexu(FACE * p_list, int fnum,
  int face, int *p_naslych, int k, int o, int flag)
{
  F_VERTEX *p_ret;
  int i, j, naslych;
  int f;

  naslych = 0;
  for (i = 0; i < fnum; i++) {
    if (p_list[i] == face)
      naslych++;
  }

  if (naslych) {
    p_ret = malloc(sizeof(p_ret[0]) * naslych);
    mtest(p_ret);
  }
  else {
    return (NULL);
  }

  naslych = 0;

  if (flag & GROUP_KONT && !(flag & (GROUP_OBJ | GROUP_FACE))) {
    p_ret[naslych].k = k;
    p_ret[naslych].o = 0;
    p_ret[naslych].f = 0;
    naslych++;
  }
  else if (flag & GROUP_KONT && flag & GROUP_OBJ && !(flag & GROUP_FACE)) {
    p_ret[naslych].k = k;
    p_ret[naslych].o = o;
    p_ret[naslych].f = 0;
    naslych++;
  }
  else if (flag & GROUP_KONT && flag & GROUP_OBJ && flag & GROUP_FACE) {
    for (i = 0; i < fnum; i++) {
      if (p_list[i] == face) {
        f = i - (i % 3);
        for (j = 0; j < naslych; j++) { // zkontroluju jestli uz pridavany neni v seznamu
          if (p_ret[j].k == k && p_ret[j].o == o && p_ret[j].f == f)
            continue;
        }
        p_ret[naslych].f = f;
        p_ret[naslych].k = k;
        p_ret[naslych].o = o;
        naslych++;
      }
    }
  }
  *p_naslych = naslych;
  return (p_ret);
}

F_VERTEX *oe_vrat_face_ze_ctverce(K_EDITOR * p_cnf, int *p_num)
{
  int v, novych, i, fnum;
  int k, o, ret = 0;
  I_VERTEX *p_grp;
  F_VERTEX *p_face;
  F_VERTEX *p_ret = NULL;
  int retnum = 0;

  novych =
    oe_vrat_body_ve_scene(p_cnf, p_cnf->x1, p_cnf->y1, p_cnf->x2, p_cnf->y2,
    NULL);
  p_grp = (I_VERTEX *) malloc(sizeof(I_VERTEX) * novych);
  mtest(p_grp);

  oe_vrat_body_ve_scene(p_cnf, p_cnf->x1, p_cnf->y1, p_cnf->x2, p_cnf->y2,
    p_grp);

  for (i = 0; i < novych; i++) {
    k = p_grp[i].k;
    o = p_grp[i].o;
    v = p_grp[i].v;
    p_face = najdi_face_k_vertexu(p_cnf->p_kont[k]->p_obj[o]->p_face,
      p_cnf->p_kont[k]->p_obj[o]->facenum, v, &fnum, k, o, p_cnf->groupflag);
    if (p_face) {
      oe_pridej_group_face_list_list(&p_ret, &retnum, p_face, fnum,
        p_cnf->groupflag);
      free(p_face);
    }
  }
  free(p_grp);

  *p_num = retnum;
  return (p_ret);
}

void oe_pridej_face_ze_ctverce(K_EDITOR * p_cnf)
{
  int v, novych, i, fnum;
  int k, o;
  I_VERTEX *p_grp;
  F_VERTEX *p_face;

  novych =
    oe_vrat_body_ve_scene(p_cnf, p_cnf->x1, p_cnf->y1, p_cnf->x2, p_cnf->y2,
    NULL);
  p_grp = (I_VERTEX *) malloc(sizeof(I_VERTEX) * novych);
  mtest(p_grp);

  oe_vrat_body_ve_scene(p_cnf, p_cnf->x1, p_cnf->y1, p_cnf->x2, p_cnf->y2,
    p_grp);

  for (i = 0; i < novych; i++) {
    k = p_grp[i].k;
    o = p_grp[i].o;
    v = p_grp[i].v;
    p_face = najdi_face_k_vertexu(p_cnf->p_kont[k]->p_obj[o]->p_face,
      p_cnf->p_kont[k]->p_obj[o]->facenum, v, &fnum, k, o, p_cnf->groupflag);
    if (p_face) {
      oe_pridej_group_face_list(p_cnf, p_face, fnum, p_cnf->groupflag);
      free(p_face);
    }
  }
  free(p_grp);
}

void oe_group_face_vyber(K_EDITOR * p_cnf)
{
  oe_vybrani_ctverce(p_cnf, 0x0000ff);
  oe_ungroup_face_all(p_cnf);
  oe_pridej_face_ze_ctverce(p_cnf);
}

void oe_group_face_pridej(K_EDITOR * p_cnf)
{
  oe_vybrani_ctverce(p_cnf, 0x0000ff);
  oe_pridej_face_ze_ctverce(p_cnf);
}


void oe_pridej_objekt_do_kont(EDIT_KONTEJNER * p_kont, EDIT_OBJEKT * p_obj)
{
  int o = lo_najdi_volny_objekt_kont(p_kont);

  if (o != K_CHYBA) {
    p_kont->p_obj[o] = kopiruj_objekt(p_obj);
  }
  else
    ddw("Neni volny kontejner !");
}

void oe_sloz_objekty_do_kontejneru(K_EDITOR * p_cnf, EDIT_KONTEJNER * p_kont,
  int presun)
{
  int g, k = -1, o = -1, v = -1;

  if (p_kont && p_cnf->groupnum) {
    oe_serad_group(p_cnf);
    for (g = 0; g < p_cnf->groupnum; g++) {
      if (o != p_cnf->p_group[g].o || k != p_cnf->p_group[g].k) {
        k = p_cnf->p_group[g].k;
        o = p_cnf->p_group[g].o;
        oe_pridej_objekt_do_kont(p_kont, p_cnf->p_kont[k]->p_obj[o]);
        if (presun)
          zrus_objekt(p_cnf->p_kont[k]->p_obj + o);
      }
    }
  }
}

// prida vybrany objekt do akt kontejneru
void oe_prikaz_pridej_akt_objekt(K_EDITOR * p_cnf, int presun)
{
  int k = p_cnf->vybr_kont;

  if (!p_cnf->groupnum)
    ddw("Musi byt vybrane objekty pomoci bodu !");
  else {
    if (!p_cnf->p_kont[k])
      p_cnf->p_kont[k] = vyrob_kontejner();
    oe_sloz_objekty_do_kontejneru(p_cnf, p_cnf->p_kont[k], presun);
  }
}

/*
//  transformuje 2D bod do 3D prostoru
//  dynamicky se zanorovat podle velikosti trojuhelniku

void triangle_div(TRIANGLE tri, int hloubka)
{
  TRIANGLE t1,t2;
  OBJ_VERTEX p,a,b,c;
  float vzd1;
  float vzd2;
  float vzd3;

  if(hloubka > 4) { // max 3 iterace
    if(posledni < MAX_LIST_TRI)
      list[posledni++] = tri;
    else
      chybat(STC,"Prekrocena max pocet !!! posledni = %d max = %d",posledni,MAX_LIST);
    return;
  }

  
  // 1. Zjistim nejdelsi stranu trianglu
  // strany:
  
//    tri.a - tri.b
//    tri.b - tri.c
//    tri.c - tri.a
  
  vzd1 = vzdal_bodu_bod(tri.a,tri.b);
  vzd2 = vzdal_bodu_bod(tri.b,tri.c);
  vzd3 = vzdal_bodu_bod(tri.c,tri.a);
  
  if(vzd1 > vzd2) {
    if(vzd1 > vzd3) {
      // nejvetsi vzd1
      a = tri.a;
      b = tri.b;
      c = tri.c;
    } else {
      // nejvetsi vzd3
      a = tri.c;
      b = tri.a;
      c = tri.b;
    }
  } else {
    if(vzd2 > vzd3) {
      // nejvetsi vzd2
      a = tri.b;
      b = tri.c;
      c = tri.a;
    } else {
      // nejvetsi vzd3
      a = tri.c;
      b = tri.a;
      c = tri.b;
    }
  }
  
  p.diff = a.diff; // nakopirovani zakl. dat
  // interpolace normaloveho vektoru mezi vybranymi a,b
  interpoluj_vec_bod(a,b,p);
   
//  p.nx = a.nx; // kopie norm vektoru
//  p.ny = a.ny;
//  p.nz = a.nz; 

  // p je stred
  // a->b
  p.x = a.x+(b.x-a.x)*0.5f;
  p.y = a.y+(b.y-a.y)*0.5f;
  p.z = a.z+(b.z-a.z)*0.5f;
  
  // rozdelim i texturu
  p.tu1 = a.tu1+(b.tu1-a.tu1)*0.5f;
  p.tv1 = a.tv1+(b.tv1-a.tv1)*0.5f;
  p.tu2 = a.tu2+(b.tu2-a.tu2)*0.5f;
  p.tv2 = a.tv2+(b.tv2-a.tv2)*0.5f;
  p.tu3 = a.tu3+(b.tu3-a.tu3)*0.5f;
  p.tv3 = a.tv3+(b.tv3-a.tv3)*0.5f;

  // trojuhelniky jsou :
  t1.a = c; t1.b = a; t1.c = p;
  t2.a = c; t2.b = p; t2.c = b;
  
  // rekurzivne pokracuj
  triangle_div(t1,hloubka+1);
  triangle_div(t2,hloubka+1);
}

void rozdrob_objekt(EDIT_KONTEJNER *p_kont, int o)
{
 EDIT_OBJEKT *p_obj,*p_new_obj;
 TRIANGLE tri;
 int i,j,f;

 posledni = 0;
 p_obj = p_kont->p_obj[o];

 for(f = 0; f < p_obj->facenum; f+=3) {
   tri.a = p_obj->p_vertex[p_obj->p_face[f]];
   tri.b = p_obj->p_vertex[p_obj->p_face[f+1]];
   tri.c = p_obj->p_vertex[p_obj->p_face[f+2]];
    
   triangle_div(tri, 0); // rozdeli na 3
   
   p_new_obj = vyrob_objekt_komplet(posledni*3,posledni*3);
   
   for(i = 0, j = 0; i < posledni*3; i+=3,j++) {
     p_new_obj->p_vertex[i]   = list[j].a;
     p_new_obj->p_vertex[i+1] = list[j].b;
     p_new_obj->p_vertex[i+2] = list[j].c;
     
     p_new_obj->p_face[i] = i;
     p_new_obj->p_face[i+1] = i+1;
     p_new_obj->p_face[i+2] = i+2;
   }
   p_new_obj->material = p_obj->material;
   p_new_obj->flag = p_obj->flag;
 }
 zrus_objekt(p_kont->p_obj+o);
 p_kont->p_obj[o] = p_new_obj;
}
*/
/*
void rozdrob_scenu(K_EDITOR *p_cnf, HWND hwnd)
{  
  int k,o;
  MEMORYSTATUS mst,oldmst;
  byte pom[100];

  memset(&mst,0,sizeof(mst));
  mst.dwLength = sizeof(mst);

  SetWindowText(GetDlgItem(hwnd,IDC_STATIC1),"Rozdrobuji scenu ...");
  doe_nastav_progresbar(hwnd, IDC_PROGRESS5, 0, p_cnf->stat.objektu);
  for(k = 0; k < KONT_NUM; k++) {
    if(p_cnf->p_kont[k]) {
      sprintf(pom,"Kontejner %d - %s",k,p_cnf->p_kont[k]->jmeno);
      SetWindowText(GetDlgItem(hwnd,IDC_STATIC4),pom);
      for(o = 0; o < p_cnf->p_kont[k]->objektu; o++) {
        if(p_cnf->p_kont[k]->p_obj[o]) {
          doe_posun_progresbar(hwnd, IDC_PROGRESS5,1);
          sprintf(pom,"Objekt %d - %s",o,p_cnf->p_kont[k]->p_obj[o]->jmeno);
          SetWindowText(GetDlgItem(hwnd,IDC_STATIC2),pom);
          oldmst = mst;
          GlobalMemoryStatus(&mst);
          sprintf(pom,"Fyzicke %d rozdil %d",mst.dwAvailPhys,mst.dwAvailPhys-oldmst.dwAvailPhys);
          SetWindowText(GetDlgItem(hwnd,IDC_STATIC5),pom);
          sprintf(pom,"Virtualni %d rozdil %d",mst.dwAvailVirtual,mst.dwAvailVirtual-oldmst.dwAvailVirtual);
          SetWindowText(GetDlgItem(hwnd,IDC_STATIC6),pom);
          rozdrob_objekt(p_cnf->p_kont[k],o);
        }
      }     
    }
  }  
}
*/

// max hloubka drobice je 10
#define   MAX_LIST_TRI  1025
int _tri_posledni = 0;
TRIANGLE _tri_list[MAX_LIST_TRI];


void triangle_div(TRIANGLE tri, int hloubka)
{
  OBJ_VERTEX p, a, b, c;
  TRIANGLE t1, t2;
  float vzd1, vzd2, vzd3;


  if (hloubka <= 0) {
    if (_tri_posledni < MAX_LIST_TRI)
      _tri_list[_tri_posledni++] = tri;
    else
      ddw("Prekrocena max pocet !!! posledni = %d max = %d", _tri_posledni,
        MAX_LIST_TRI);
    return;
  }


// 1. Zjistim nejdelsi stranu trianglu
// strany:
// tri.a - tri.b
// tri.b - tri.c
// tri.c - tri.a

  vzd1 = vzdal_bodu(&tri.a, &tri.b);
  vzd2 = vzdal_bodu(&tri.b, &tri.c);
  vzd3 = vzdal_bodu(&tri.c, &tri.a);

  if (vzd1 > vzd2) {
    if (vzd1 > vzd3) {
      // nejvetsi vzd1
      a = tri.a;
      b = tri.b;
      c = tri.c;
    }
    else {
      // nejvetsi vzd3
      a = tri.c;
      b = tri.a;
      c = tri.b;
    }
  }
  else {
    if (vzd2 > vzd3) {
      // nejvetsi vzd2
      a = tri.b;
      b = tri.c;
      c = tri.a;
    }
    else {
      // nejvetsi vzd3
      a = tri.c;
      b = tri.a;
      c = tri.b;
    }
  }

  // interpolace normaloveho vektoru mezi vybranymi a,b
  interpoluj_vec_bod(a, b, p);

  // p je stred
  // a->b
  p.x = a.x + (b.x - a.x) * 0.5f;
  p.y = a.y + (b.y - a.y) * 0.5f;
  p.z = a.z + (b.z - a.z) * 0.5f;

  // rozdelim i texturu 1/2
  p.tu1 = a.tu1 + (b.tu1 - a.tu1) * 0.5f;
  p.tv1 = a.tv1 + (b.tv1 - a.tv1) * 0.5f;
  p.tu2 = a.tu2 + (b.tu2 - a.tu2) * 0.5f;
  p.tv2 = a.tv2 + (b.tv2 - a.tv2) * 0.5f;

  // kopie barev
  p.dr = a.dr + (b.dr - a.dr) * 0.5f;
  p.dg = a.dg + (b.dg - a.dg) * 0.5f;
  p.db = a.db + (b.db - a.db) * 0.5f;
  p.da = a.da + (b.da - a.da) * 0.5f;

  p.mdr = a.mdr + (b.mdr - a.mdr) * 0.5f;
  p.mdg = a.mdg + (b.mdg - a.mdg) * 0.5f;
  p.mdb = a.mdb + (b.mdb - a.mdb) * 0.5f;
  p.mda = a.mda + (b.mda - a.mda) * 0.5f;

  p.sr = a.sr + (b.sr - a.sr) * 0.5f;
  p.sg = a.sg + (b.sg - a.sg) * 0.5f;
  p.sb = a.sb + (b.sb - a.sb) * 0.5f;

  p.msr = a.msr + (b.msr - a.msr) * 0.5f;
  p.msg = a.msg + (b.msg - a.msg) * 0.5f;
  p.msb = a.msb + (b.msb - a.msb) * 0.5f;

  // trojuhelniky jsou :
  t1.a = c;
  t1.b = a;
  t1.c = p;
  t2.a = c;
  t2.b = p;
  t2.c = b;

  // rekurzivne pokracuj
  triangle_div(t1, hloubka - 1);
  triangle_div(t2, hloubka - 1);
}

// rozdrobim vsechny facy objektu
// zrusim objekt
// vybuduju objekt z listu seznamu

// pokud jsou vertexy stejne -> x,y,z a tu1 tv1 -> slepim jej do jednoho
void kopiruj_vlastnosti_objektu(EDIT_OBJEKT * p_dest, EDIT_OBJEKT * p_src)
{
  strcpy(p_dest->jmeno, p_src->jmeno);
  p_dest->m1flag = p_src->m1flag;
  p_dest->m2flag = p_src->m2flag;
  p_dest->oflag = p_src->oflag;
  p_dest->kreslit = p_src->kreslit;
  p_dest->material = p_src->material;
  p_dest->obb = p_src->obb;
}

void oe_prikaz_rozdrob_objekt(K_EDITOR * p_cnf, int vybr_face)
{
  int k = p_cnf->vybr_kont;
  int o = p_cnf->vybr_objekt;
  int vf = p_cnf->vybr_face;
  int v, f;
  EDIT_OBJEKT *p_obj, *p_new;
  TRIANGLE tri;
  OBJ_VERTEX *p_tri_list;

  n_objekt(k, o);

  // omezeni max hloubky rekurze na 10
  if (p_cnf->drobic_hloubka > 10)
    p_cnf->drobic_hloubka = 10;

  p_obj = p_cnf->p_kont[k]->p_obj[o];
  _tri_posledni = 0;

  if (!vybr_face) {
    for (f = 0; f < p_obj->facenum; f += 3) {
      tri.a = p_obj->p_vertex[p_obj->p_face[f]];
      tri.b = p_obj->p_vertex[p_obj->p_face[f + 1]];
      tri.c = p_obj->p_vertex[p_obj->p_face[f + 2]];
      triangle_div(tri, p_cnf->drobic_hloubka);
    }
  }
  else {
    for (f = 0; f < p_obj->facenum; f += 3) {
      tri.a = p_obj->p_vertex[p_obj->p_face[f]];
      tri.b = p_obj->p_vertex[p_obj->p_face[f + 1]];
      tri.c = p_obj->p_vertex[p_obj->p_face[f + 2]];
      if (vf == f)
        triangle_div(tri, p_cnf->drobic_hloubka);
      else
        _tri_list[_tri_posledni++] = tri;
    }
  }
  p_tri_list = malloc(sizeof(OBJ_VERTEX) * _tri_posledni * 3);
  if (!p_tri_list)
    chyba("Pamet !");

  for (f = 0; f < _tri_posledni; f++) {
    p_tri_list[f * 3] = _tri_list[f].a;
    p_tri_list[f * 3 + 1] = _tri_list[f].b;
    p_tri_list[f * 3 + 2] = _tri_list[f].c;
  }

  // eliminuje stejne body
  oe_serad_tri(p_tri_list, _tri_posledni * 3);
  v = oe_smaz_stejne_vertexy(p_tri_list, _tri_posledni * 3);

  // v _tri_list jsou trojice trojuhelniku
  p_new = vyrob_objekt_komplet(v, _tri_posledni * 3);
  kopiruj_vlastnosti_objektu(p_new, p_obj);
  p_new->x_rect = p_new->y_rect = 0;

  // prekopirovani bodu do noveho objektu
  for (v = 0, f = 0; v < _tri_posledni * 3; v++) {
    if (p_tri_list[v].x != FLT_MAX) {
      assert(f < p_new->vertexnum);
      p_new->p_vertex[f++] = p_tri_list[v];
    }
  }

  // doplneni facu
  for (f = 0; f < _tri_posledni; f++) {

    v = oe_najdi_vertex(p_new->p_vertex, p_new->vertexnum, &_tri_list[f].a);
    p_new->p_face[f * 3] = v;

    v = oe_najdi_vertex(p_new->p_vertex, p_new->vertexnum, &_tri_list[f].b);
    p_new->p_face[f * 3 + 1] = v;

    v = oe_najdi_vertex(p_new->p_vertex, p_new->vertexnum, &_tri_list[f].c);
    p_new->p_face[f * 3 + 2] = v;
  }

  zrus_objekt(p_cnf->p_kont[k]->p_obj + o);
  p_cnf->p_kont[k]->p_obj[o] = p_new;

  free(p_tri_list);
}

void oe_prikaz_rozdrob_kontejner(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  int k = p_cnf->vybr_kont;
  int o = p_cnf->vybr_objekt;
  int i;

  n_kont(k);

  p_kont = p_cnf->p_kont[k];
  for (i = 0; i < p_kont->max_objektu; i++) {
    if (p_kont->p_obj[i]) {
      p_cnf->vybr_objekt = i;
      oe_prikaz_rozdrob_objekt(p_cnf, FALSE);
    }
  }
  p_cnf->vybr_objekt = o;
}

int oe_serad_tri_compare(const int *p_vert1, const int *p_vert2)
{
  float cmp;

  cmp = ((OBJ_VERTEX *) p_vert1)->x - ((OBJ_VERTEX *) p_vert2)->x;
  if (cmp != 0.0f)
    return (cmp > 0.0f ? 1 : -1);

  cmp = ((OBJ_VERTEX *) p_vert1)->y - ((OBJ_VERTEX *) p_vert2)->y;
  if (cmp != 0.0f)
    return (cmp > 0.0f ? 1 : -1);

  cmp = ((OBJ_VERTEX *) p_vert1)->z - ((OBJ_VERTEX *) p_vert2)->z;
  if (cmp != 0.0f)
    return (cmp > 0.0f ? 1 : -1);

  cmp = ((OBJ_VERTEX *) p_vert1)->tu1 - ((OBJ_VERTEX *) p_vert2)->tu1;
  if (cmp != 0.0f)
    return (cmp > 0.0f ? 1 : -1);

  cmp = ((OBJ_VERTEX *) p_vert1)->tv1 - ((OBJ_VERTEX *) p_vert2)->tv1;
  if (cmp != 0.0f)
    return (cmp > 0.0f ? 1 : -1);

  return (0);
}

// seradi triangly podle x,y,z,tu,uv
void oe_serad_tri(OBJ_VERTEX * p_list, int num)
{
  qsort((void *) p_list, (size_t) num, (size_t) sizeof(OBJ_VERTEX),
    oe_serad_tri_compare);
}

// bin. vyhledavani
int oe_najdi_vertex(OBJ_VERTEX * p_list, int num, OBJ_VERTEX * p_hled)
{
  int l, p, s, res;

  l = 0;
  p = num - 1;
  s = (l + p) >> 1;

  while ((res =
      oe_serad_tri_compare((const int *) p_hled, (const int *) (p_list + s)))
    && l <= p) {
    if (res > 0)
      l = s + 1;
    else
      p = s - 1;
    s = (l + p) >> 1;
  }

  assert(l <= p);

  return (s);
}

__inline static void oe_zaslep_vertex(OBJ_VERTEX * p_vertex)
{
  p_vertex->x = p_vertex->y = p_vertex->z = FLT_MAX;
}

int oe_smaz_stejne_vertexy(OBJ_VERTEX * p_list, int num)
{
  int v, celkem = num;

  for (v = 1; v < num; v++) {
    // jsou stejne
    if (!oe_serad_tri_compare((const int *) (p_list + v - 1),
        (const int *) (p_list + v))) {
      oe_zaslep_vertex(p_list + v - 1);
      celkem--;
    }
  }
  return (celkem);
}

//void oe_prikaz_rozdrob_face
void oe_prikaz_desintegrator(K_EDITOR * p_cnf)
{
  int k = p_cnf->vybr_kont;

  n_kont(k);
  if (p_cnf->p_kont[k]->kflag & KONT_STATIC) {
    lo_premapuj_svetla_do_wordspace(p_cnf->p_kont[k]);
    if (ds_kontejner_rozloz(p_cnf, p_cnf->p_kont[k], ROZLOZ_MAX))
      zrus_kontejner_rec(p_cnf->p_kont + k, &p_cnf->p_mlha);
  }
  else {
    ddw("Expandovat se daji pouze staticke kontejnery!");
  }
}

void oe_prikaz_osamostatnit_objekt(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_kont = NULL, *p_prev = NULL, *p_new = NULL;
  EDIT_OBJEKT *p_obj;
  byte jmeno[MAX_JMENO];
  int handle;
  int k;
  int o;
  int no;
  int kn;


  handle = oe_vybr_obj_reset();
  if (handle) {
    if ((kn =
        lo_najdi_volny_kontejner(p_cnf->p_kont,
          p_cnf->max_kont)) == K_CHYBA) {
      ddw("Neni volny kontejner!");
      return;
    }

    p_cnf->vybr_objekt = 0;
    p_new = vyrob_kontejner();

    while ((p_obj = oe_vybr_obj_get_ind(&handle, &k, &o))) {
      p_kont = oe_vybr_obj_get_kont(&handle);
      if (!p_prev)
        strcpy(jmeno, p_obj->jmeno);
      if (p_kont != p_prev) {
        p_prev = p_kont;
        transformuj_kontejner(p_kont);
      }
      no = lo_najdi_volny_objekt_kont(p_new);
      p_new->p_obj[no] = p_obj;
      p_kont->p_obj[o] = NULL;
    }

    updatuj_kontejner_statistika(p_kont, TRUE);
    updatuj_kontejner_statistika(p_new, TRUE);
    oe_ungroup_face_all(p_cnf);

    doe_cti_jmeno_ext(hwnd_aplikace, jmeno, "Jmeno vyrizleho kontejneru:");
    strcpy(p_new->jmeno, jmeno);

    p_cnf->p_kont[kn] = p_new;
    kprintf(TRUE, "Novy kontejner je %s na pozici %d",
      p_cnf->p_kont[kn]->jmeno, kn);
  }
}

void oe_prikaz_vertex_objekt(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_kont = NULL, *p_prev = NULL, *p_new = NULL;
  EDIT_OBJEKT *p_obj;
  byte jmeno[MAX_JMENO];
  int handle;
  int k;
  int o;
  int no;
  int kn;


  handle = oe_vybr_obj_reset();
  if (handle) {
    if ((kn =
        lo_najdi_volny_kontejner(p_cnf->p_kont,
          p_cnf->max_kont)) == K_CHYBA) {
      ddw("Neni volny kontejner!");
      return;
    }

    p_new = vyrob_kontejner();

    while ((p_obj = oe_vybr_obj_get_ind(&handle, &k, &o))) {
      p_kont = oe_vybr_obj_get_kont(&handle);
      if (!p_prev)
        strcpy(jmeno, p_obj->jmeno);
      if (p_kont != p_prev) {
        p_prev = p_kont;
        transformuj_kontejner(p_kont);
      }
      no = lo_najdi_volny_objekt_kont(p_new);
      p_new->p_obj[no] = p_obj;
      p_kont->p_obj[o] = NULL;
    }

    updatuj_kontejner_statistika(p_kont, TRUE);
    updatuj_kontejner_statistika(p_new, TRUE);
    oe_ungroup_face_all(p_cnf);

    doe_cti_jmeno_ext(hwnd_aplikace, jmeno, "Jmeno vyrizleho kontejneru:");
    strcpy(p_new->jmeno, jmeno);

    p_cnf->p_kont[kn] = p_new;
    kprintf(TRUE, "Novy kontejner je %s na pozici %d",
      p_cnf->p_kont[kn]->jmeno, kn);

    p_new->kflag |= (KONT_BRUTALNORMAL | KONT_VERTEX_LIGHT);
    p_cnf->vybr_objekt = 0;
  }
}

/* Vlozi parametry kamery do klice kamery
*/
void kamera_vloz_do_klice(KAMERA * p_kam, int flag)
{
  KAMERA_TRACK_INFO *p_track = p_kam->tracky + p_kam->akt_animace;
  KEY_POINT_BRS nklic;
  int klic;
  int time;
  int frame;

  if (!p_track->endtime || p_track->time > p_track->endtime)
    return;

  time = p_track->time;
  frame = time2frame(time);

  if (flag & POSUN_POZICI) {
    if ((klic =
        key_je_time_klic(p_track->p_pkeys, p_track->pos_keys,
          time)) == K_CHYBA) {
      // zaloz novy klic
      memset(&nklic, 0, sizeof(nklic));
      nklic.time = time;
      p_track->pos_keys = key_track_klic_vloz((byte **) & p_track->p_pos,
        &p_track->p_pkeys, p_track->pos_keys,
        sizeof(p_track->p_pos[0]), (byte *) & p_kam->p, &nklic);
      doe_hs_setmark(frame);
    }
    else {
      memset(p_track->p_pkeys + klic, 0, sizeof(p_track->p_pkeys[klic]));
      p_track->p_pkeys[klic].time = time;
      p_track->p_pos[klic] = p_kam->p;
    }
  }

  if (flag & POSUN_TARGET) {
    if ((klic =
        key_je_time_klic(p_track->p_tkeys, p_track->trg_keys,
          time)) == K_CHYBA) {
      // zaloz novy klic
      memset(&nklic, 0, sizeof(nklic));
      nklic.time = time;
      p_track->trg_keys =
        key_track_klic_vloz((byte **) & p_track->p_trg, &p_track->p_tkeys,
        p_track->trg_keys, sizeof(p_track->p_trg[0]), (byte *) & p_kam->t,
        &nklic);
      doe_hs_setmark(frame);
    }
    else {
      memset(p_track->p_tkeys + klic, 0, sizeof(p_track->p_tkeys[klic]));
      p_track->p_tkeys[klic].time = time;
      p_track->p_trg[klic] = p_kam->t;
    }
  }

  if (flag & POSUN_ROLL) {
    if ((klic =
        key_je_time_klic(p_track->p_rlkeys, p_track->roll_keys,
          time)) == K_CHYBA) {
      // zaloz novy klic
      memset(&nklic, 0, sizeof(nklic));
      nklic.time = time;
      p_track->roll_keys =
        key_track_klic_vloz((byte **) & p_track->p_roll, &p_track->p_rlkeys,
        p_track->roll_keys, sizeof(p_track->p_roll[0]),
        (byte *) & p_kam->roll, &nklic);
      doe_hs_setmark(frame);
    }
    else {
      memset(p_track->p_rlkeys + klic, 0, sizeof(p_track->p_rlkeys[klic]));
      p_track->p_rlkeys[klic].time = time;
      p_track->p_roll[klic] = p_kam->roll;
    }
  }

  if (flag & POSUN_FOV) {
    if ((klic =
        key_je_time_klic(p_track->p_fkeys, p_track->fov_keys,
          time)) == K_CHYBA) {
      // zaloz novy klic
      memset(&nklic, 0, sizeof(nklic));
      nklic.time = time;
      p_track->fov_keys =
        key_track_klic_vloz((byte **) & p_track->p_fov, &p_track->p_fkeys,
        p_track->fov_keys, sizeof(p_track->p_fov[0]), (byte *) & p_kam->fov,
        &nklic);
      doe_hs_setmark(frame);
    }
    else {
      memset(p_track->p_fkeys + klic, 0, sizeof(p_track->p_fkeys[0]));
      p_track->p_fkeys[klic].time = time;
      p_track->p_fov[klic] = p_kam->fov;
    }
  }

}

// bod = 3 -vse, 1- target,2-position
void oe_prikaz_posun_kameru(K_EDITOR * p_cnf, int bod)
{
  VEKTOR v1, v2;
  GLMATRIX mat, mrt;
  int k = p_cnf->kamakt, zmena;
  KAMERA *p_kam = p_cnf->kamery + k;

  if (p_kam->cislo == K_CHYBA) {
    ddw("Neni vybrana aktivni kamera!");
    return;
  }

  calc_transf_3d_2d_matrix(init_matrix(&mrt), p_cnf->p_camera,
    p_cnf->p_project, &mat);
  *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);
  transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal, &v1.x,
    &v1.y, &v1.z, &mat, OXRES, OYRES, OXSTART, OYSTART, p_cnf->kam.near_plane,
    p_cnf->kam.far_plane);

  kprintf(TRUE, "Posun kamery %s", p_kam->jmeno);
  p_kam->flag = bod;

  nuluj_mys();
  while (stisk_mys()) {

    transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal, &v2.x,
      &v2.y, &v2.z, &mat, OXRES, OYRES, OXSTART, OYSTART,
      p_cnf->kam.near_plane, p_cnf->kam.far_plane);

    zmena = FALSE;
    if (!key[K_X] && !key[K_Y] && !key[K_Z]) {
      p_kam->flag |= (POSUN_OSA_X | POSUN_OSA_Y | POSUN_OSA_Z);
      if (POSUN_TARGET & bod) {
        p_kam->t.x += v2.x - v1.x;
        p_kam->t.y += v2.y - v1.y;
        p_kam->t.z += v2.z - v1.z;
        zmena = TRUE;
      }
      if (POSUN_POZICI & bod) {
        p_kam->p.x += v2.x - v1.x;
        p_kam->p.y += v2.y - v1.y;
        p_kam->p.z += v2.z - v1.z;
        zmena = TRUE;
      }
    }
    else {
      if (key[K_X]) {
        p_kam->flag = POSUN_OSA_X | bod;
        if (POSUN_TARGET & bod) {
          p_kam->t.x += v2.x - v1.x;
          zmena = TRUE;
        }
        if (POSUN_POZICI & bod) {
          p_kam->p.x += v2.x - v1.x;
          zmena = TRUE;
        }
      }
      if (key[K_Y]) {
        p_kam->flag = POSUN_OSA_Y | bod;
        if (POSUN_TARGET & bod) {
          p_kam->t.y += v2.y - v1.y;
          zmena = TRUE;
        }
        if (POSUN_POZICI & bod) {
          p_kam->p.y += v2.y - v1.y;
          zmena = TRUE;
        }
      }
      if (key[K_Z]) {
        p_kam->flag = POSUN_OSA_Z | bod;
        if (POSUN_TARGET & bod) {
          p_kam->t.z += v2.z - v1.z;
          zmena = TRUE;
        }
        if (POSUN_POZICI & bod) {
          p_kam->p.z += v2.z - v1.z;
          zmena = TRUE;
        }
      }
    }

    if (zmena) {
      kameru_dratovou_reanimuj(p_kam);

      if (p_cnf->cnf.kamera_aktivni)
        oe_kamera_to_view(p_cnf, p_cnf->kamakt);

      if (p_cnf->cnf.kamera_vkladat_do_klicu) {
        kamera_vloz_do_klice(p_kam, bod);
        doe_animace_updatuj_kamery_text(p_cnf, p_kam);
      }
    }

    nuluj_dif();
    oe_renderuj_scenu_pre(p_cnf);
    oe_renderuj_scenu_po(p_cnf);
    transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
      &v1.x, &v1.y, &v1.z, &mat, OXRES, OYRES, OXSTART, OYSTART,
      p_cnf->kam.near_plane, p_cnf->kam.far_plane);
  }

  p_kam->flag = 0;
}

/* Editace parametru roll u kamery
*/
void oe_prikaz_roll_kameru(K_EDITOR * p_cnf)
{
  int k = p_cnf->kamakt;
  KAMERA *p_kam = p_cnf->kamery + k;

  if (p_kam->cislo == K_CHYBA) {
    ddw("Neni vybrana aktivni kamera!");
    return;
  }

  kprintf(TRUE, "Roll kamery %s", p_kam->jmeno);

  nuluj_mys();
  while (stisk_mys()) {
    if (mi.dy) {
      p_kam->roll += 0.01f * mi.dy;
      kprintf(FALSE, "Roll kamery %.2f stupnu", RAD2DEG(p_kam->roll));
      kameru_dratovou_reanimuj(p_kam);

      if (p_cnf->cnf.kamera_aktivni)
        oe_kamera_to_view(p_cnf, p_cnf->kamakt);

      if (p_cnf->cnf.kamera_vkladat_do_klicu) {
        kamera_vloz_do_klice(p_kam, POSUN_ROLL);
        doe_animace_updatuj_kamery_text(p_cnf, p_kam);
      }
    }
    nuluj_dif();
    oe_renderuj_scenu_pre(p_cnf);
    oe_renderuj_scenu_po(p_cnf);
  }
}

/* Editace parametru fov u kamery
*/

#define MAX_FOV      (DEG2RAD(120.0f))
#define MIN_FOV      (DEG2RAD(1.0f))

void oe_prikaz_fov_kameru(K_EDITOR * p_cnf)
{
  int k = p_cnf->kamakt;
  KAMERA *p_kam = p_cnf->kamery + k;

  if (p_kam->cislo == K_CHYBA) {
    ddw("Neni vybrana aktivni kamera!");
    return;
  }

  kprintf(TRUE, "Fov kamery %s", p_kam->jmeno);

  nuluj_mys();
  while (stisk_mys()) {
    if (mi.dy) {
      p_kam->fov += 0.01f * mi.dy;

      if (p_kam->fov > MAX_FOV)
        p_kam->fov = MAX_FOV;
      if (p_kam->fov < MIN_FOV)
        p_kam->fov = MIN_FOV;

      kprintf(FALSE, "Fov kamery (zoom) %.2f stupnu", RAD2DEG(p_kam->fov));
      kameru_dratovou_reanimuj(p_kam);

      if (p_cnf->cnf.kamera_aktivni)
        oe_kamera_to_view(p_cnf, p_cnf->kamakt);

      if (p_cnf->cnf.kamera_vkladat_do_klicu) {
        kamera_vloz_do_klice(p_kam, POSUN_FOV);
        doe_animace_updatuj_kamery_text(p_cnf, p_kam);
      }
    }
    nuluj_dif();
    oe_renderuj_scenu_pre(p_cnf);
    oe_renderuj_scenu_po(p_cnf);
  }
}


KAMERA *oe_kamera_najdi_volnou(KAMERA * p_kam, int maxkam, int *p_kamnum)
{
  int i;

  for (i = 0; i < maxkam; i++) {
    if (p_kam[i].cislo == K_CHYBA) {
      p_kam[i].cislo = i;
      if (i >= *p_kamnum)
        *p_kamnum = i + 1;
      return (p_kam + i);
    }
  }
  return (NULL);
}

/* Nova kamera
*/
void oe_prikaz_kamera_nova(K_EDITOR * p_cnf)
{
  KAMERA *p_kam =
    oe_kamera_najdi_volnou(p_cnf->kamery, MAX_EDIT_KAMER, &p_cnf->kamnum);
  BOD p, t;
  byte pom[200];
  int knum;

  /* Najiti kamery
   */
  if (!p_kam) {
    ddw("Neni volna kamera!");
    return;
  }

  knum = p_kam->cislo;
  memset(p_kam, 0, sizeof(*p_kam));
  p_kam->cislo = K_CHYBA;

  /* Vlozeni pozice
   */

  kprintf(TRUE, "Vyber pozici kamery...");
  //nuluj_mys();
  while (spracuj_spravy(TRUE)) {
    if (mi.t1) {
      if (oe_prikaz_vyber_mysi_bod(p_cnf, &p))
        break;
      else {
        ddw("Nemuzu chytit kameru!");
      }
    }
    oe_renderuj_scenu_pre(p_cnf);
    oe_renderuj_scenu_po(p_cnf);
  }
  mi.t1 = FALSE;

  /* Vlozeni target
   */
  kprintf(TRUE, "Vyber cil kamery...");
  nuluj_mys();
  while (spracuj_spravy(TRUE)) {
    if (mi.t1) {
      if (oe_prikaz_vyber_mysi_bod(p_cnf, &t))
        break;
      else {
        ddw("Nemuzu chytit kameru!");
      }
    }
    oe_renderuj_scenu_pre(p_cnf);
    oe_renderuj_scenu_po(p_cnf);
  }
  mi.t1 = FALSE;

  sprintf(pom, "Nkamera %d", knum);
  if (doe_cti_jmeno(hwnd_aplikace, pom)) {
    vyrob_kameru_indir(p_kam, pom, &p, &t, DEG2RAD(45), 0.0f);
    p_cnf->kamakt = p_kam->cislo = knum;
    kprintf(TRUE, "Pridana kamera '%s'", pom);
  }
}

void oe_prikaz_kamera_smazat(K_EDITOR * p_cnf)
{
  int i, cislo = p_cnf->kamakt;
  KAMERA *p_kam = p_cnf->kamery + cislo;

  kprintf(TRUE, "Mazani kamery %s...", p_kam->jmeno);

  // Zruseni animaci
  for (i = 0; i < KAM_MAX_ANIM; i++) {
    if (p_kam->tracky[i].endtime) {     // vyrus animace      
      key_kamera_zrus_indir(p_kam->tracky + i);
    }
  }

  // Vymaznuti kamery
  memset(p_kam, 0, sizeof(*p_kam));
  p_kam->cislo = K_CHYBA;
}

void oe_prikaz_kamera_upravit(K_EDITOR * p_cnf)
{
  KAMERA *p_kam = p_cnf->kamery + p_cnf->kamakt;

  if (p_kam->cislo == K_CHYBA) {
    kprintf(TRUE, "Neni vybrana zadna kamera!");
  }
  else {
    doe_cti_jmeno(hwnd_aplikace, p_kam->jmeno);
  }
}

/* Obslouzeni animaci
*/

void oe_prikaz_kamera_animace_nova(K_EDITOR * p_cnf)
{
  KAMERA *p_kam = p_cnf->kamery + p_cnf->kamakt;
  KAMERA_TRACK_INFO *p_track;
  int i;

  kprintf(TRUE, "Nova animace");
  if (p_kam->cislo == K_CHYBA) {
    kprintf(TRUE, "Neni vybrana zadna kamera!");
  }
  else {
    i = p_kam->akt_animace;
    if (p_kam->tracky[i].endtime) {
      for (i = 0; i < KAM_MAX_ANIM; i++)
        if (!p_kam->tracky[i].endtime) {
          p_kam->akt_animace = i;
          goto nalezeno;
        }
      kprintf(TRUE, "Neni volna animace!");
    }
  nalezeno:
    p_track = p_kam->tracky + i;
    memset(p_track, 0, sizeof(*p_track));
    sprintf(p_track->jmeno, "animace %d", i);
    p_track->endtime = calc_endtime(100);
    if (!doe_kamera_animace_uprav(hwnd_aplikace, p_track)) {
      p_track->endtime = 0;
    }
  }
}

void oe_prikaz_kamera_animace_upravit(K_EDITOR * p_cnf)
{
  KAMERA *p_kam = p_cnf->kamery + p_cnf->kamakt;
  int i = p_kam->akt_animace;
  KAMERA_TRACK_INFO *p_track = p_kam->tracky + i;

  if (p_kam->cislo == K_CHYBA)
    kprintf(TRUE, "Neni vybrana kamera!");
  else {
    if (!p_track->endtime)
      kprintf(TRUE, "Prazdna animace !");
    else {
      kprintf(TRUE, "Uprava animace %s...", p_track->jmeno);
      doe_kamera_animace_uprav(hwnd_aplikace, p_track);
    }
  }
}

void oe_prikaz_kamera_animace_smazat(K_EDITOR * p_cnf)
{
  KAMERA *p_kam = p_cnf->kamery + p_cnf->kamakt;
  int i = p_kam->akt_animace;
  KAMERA_TRACK_INFO *p_track = p_kam->tracky + i;

  if (p_kam->cislo == K_CHYBA)
    kprintf(TRUE, "Neni vybrana kamera!");
  else {
    if (!p_track->endtime)
      kprintf(TRUE, "Prazdna animace !");
    else {
      kprintf(TRUE, "Mazani animace %s...", p_track->jmeno);
      key_kamera_zrus_indir(p_kam->tracky + i);
    }
  }
}

/* Kamery load/save vcetne animaci
*/
void oe_prikaz_kamera_nahrat(K_EDITOR * p_cnf)
{
  KAMERA *p_kam = p_cnf->kamery + p_cnf->kamakt;
  KAMERA_TRACK_INFO *p_track;
  byte pom[200];
  BOD p, t;
  float roll, fov;
  int i;


  if (p_kam->cislo != K_CHYBA) {
    i = lo_najdi_volnou_kameru(p_cnf->kamery, MAX_EDIT_KAMER);
    kprintf(TRUE, "Neni vybrana prazdna kamera. Pouzivam %d", i);
    if (i == K_CHYBA) {
      kprintf(TRUE, "Neni volna kamera!");
      return;
    }
    p_cnf->kamakt = i;
    p_kam = p_cnf->kamery + p_cnf->kamakt;
  }

  chdir(p_cnf->dir.game_data_dir);

  if (doe_dialog_load(hwnd_aplikace, pom, 200, "Vyber soubor s kamerou",
      "*.sik", "Soubor s animacema kamer (*.sik)")) {
    memset(p_kam, 0, sizeof(*p_kam));
    p_kam->cislo = K_CHYBA;
    p_track = p_kam->tracky;

    if (key_kamera_nahraj(p_track, p_kam->jmeno, p_track->jmeno, NULL, pom,
        ".")) {
      p_kam->cislo = p_cnf->kamakt;
      if (p_cnf->kamnum <= p_kam->cislo)
        p_cnf->kamnum = p_kam->cislo + 1;
      key_kamera_track_cti(p_track, &p, &t, &roll, &fov);
      vyrob_kameru_indir(p_kam, p_kam->jmeno, &p, &t, fov, roll);
    }
  }
}


void oe_prikaz_kamera_ulozit(K_EDITOR * p_cnf)
{
  KAMERA *p_kam = p_cnf->kamery + p_cnf->kamakt;
  int i = p_kam->akt_animace;
  KAMERA_TRACK_INFO *p_track = p_kam->tracky + i;
  byte pom[200];

  if (p_kam->cislo == K_CHYBA)
    kprintf(TRUE, "Neni vybrana kamera!");
  else {
    if (!p_track->endtime)
      kprintf(TRUE, "Prazdna animace !");
    else {
      // ulozeni kamery
      chdir(p_cnf->dir.game_data_dir);
      if (doe_dialog_save(pom, 200, "Ulozit kameru jako", "*.sik",
          "Soubor s animacema kamer (*.sik)")) {
        if (key_kamera_uloz(p_track, p_kam->jmeno, p_track->jmeno, pom, "."))
          kprintf(TRUE, "Save kamery do %s OK", pom);
      }
    }
  }
}

/* Ulozi vsechny kamery + vsechny animace
*/
void oe_prikaz_kamera_ulozit_vse(K_EDITOR * p_cnf)
{
  KAMERA_TRACK_INFO *p_track;
  KAMERA *p_kam;
  byte pom[200];
  int i, j;

  kprintf(1, "Ukladam vsechny animace kamery do adresare %s...",
    p_cnf->dir.game_data_dir);
  chdir(p_cnf->dir.game_data_dir);

  for (i = 0; i < MAX_EDIT_KAMER; i++) {
    p_kam = p_cnf->kamery + i;
    if (p_kam->cislo != K_CHYBA) {
      for (j = 0; j < KAM_MAX_ANIM; j++) {
        p_track = p_kam->tracky + j;
        if (p_track->endtime) {
          sprintf(pom, "kamera%d_animace%d.sik", i, j);
          if (key_kamera_uloz(p_track, p_kam->jmeno, p_track->jmeno, pom,
              "."))
            kprintf(1, "Ukladam kameru %s", pom);
        }
      }
    }
  }
}

int oe_spol_body(BOD * p1, BOD * p2, BOD * p3, BOD * v1, BOD * v2, BOD * v3)
{
  int spol = 0;

//  int s = 0,
//      v = 0;
  if (podobny_vec(p1, v1) || podobny_vec(p1, v2) || podobny_vec(p1, v3))
    spol++;
  if (podobny_vec(p2, v1) || podobny_vec(p2, v2) || podobny_vec(p2, v3))
    spol++;
  if (podobny_vec(p3, v1) || podobny_vec(p3, v2) || podobny_vec(p3, v3))
    spol++;
  return (spol);
}

/*
  p1 = A+q1*t1
  p2 = B+q2*t2

  0 < t1 < 1
  0 < t2 < 1

  p1 == p2
*/

__inline int oe_sprusecik(BOD * p1, BOD * p2, BOD * v1, BOD * v2)
{
  BOD A, B, q1, q2, l, r;
  float t1, t2, rx, ry, rz;

  if (stejny_bod(p1, v1) || stejny_bod(p1, v2) ||
    stejny_bod(p2, v1) || stejny_bod(p2, v2))
    return (FALSE);

  A = *p1;
  vektor_sub(p2, p1, &q1);
  B = *v1;
  vektor_sub(v2, v1, &q2);

  vektor_sub(&A, &B, &l);
  vektor_sub(&q1, &q2, &r);
  // l = q2*t2-q1*t1
  // p1,p2 = t1
  // v1,v2 = t2

  rx = fabsf(r.x);
  ry = fabsf(r.y);
  rz = fabsf(r.z);

  if ((rx > rz && ry >= rz) || (rx >= rz && ry > rz)) {
    t1 = (q2.x * l.y - q2.y * l.x) / (q2.y * q1.x - q1.y * q2.x);
    t2 = (l.x + q1.x * t1) / q2.x;
  }
  else if ((rz > rx && ry >= rx) || (rz >= rx && ry > rx)) {
    t1 = (q2.z * l.y - q2.y * l.z) / (q2.y * q1.z - q1.y * q2.z);
    t2 = (l.z + q1.z * t1) / q2.z;
  }
  else {
    t1 = (q2.x * l.z - q2.z * l.x) / (q2.z * q1.x - q1.z * q2.x);
    t2 = (l.x + q1.x * t1) / q2.x;
  }
  return (t1 > 0.0f && t1 < 1.0f && t2 > 0.0f && t2 < 1.0f);
}

int oe_spol_prusecik(BOD * p1, BOD * p2, BOD * p3, BOD * v1, BOD * v2,
  BOD * v3)
{
  int num = 0;

  num += oe_sprusecik(p1, p2, v1, v2);
  num += oe_sprusecik(p1, p2, v1, v3);
  num += oe_sprusecik(p1, p2, v2, v3);

  num += oe_sprusecik(p1, p3, v1, v2);
  num += oe_sprusecik(p1, p3, v1, v3);
  num += oe_sprusecik(p1, p3, v2, v3);

  num += oe_sprusecik(p2, p3, v1, v2);
  num += oe_sprusecik(p2, p3, v1, v3);
  num += oe_sprusecik(p2, p3, v2, v3);

  return (num);
}

int oe_spol_face(word * p_face1, word * p_face2)
{
  int i = 0;

  if (p_face1[0] == p_face2[0] ||
    p_face1[0] == p_face2[1] || p_face1[0] == p_face2[2])
    i++;

  if (p_face1[1] == p_face2[0] ||
    p_face1[1] == p_face2[1] || p_face1[1] == p_face2[2])
    i++;

  if (p_face1[2] == p_face2[0] ||
    p_face1[2] == p_face2[1] || p_face1[2] == p_face2[2])
    i++;

  return (i >= 3);
}

void oe_oznac_placate_plosky(K_EDITOR * p_cnf, EDIT_OBJEKT * p_obj, int k,
  int o)
{
  F_VERTEX face;
  int f, f1;

  for (f = 0; f < p_obj->facenum; f += 3) {
    for (f1 = 0; f1 < p_obj->facenum; f1 += 3) {
      if (f == f1)
        continue;
      if (oe_spol_face(p_obj->p_face + f, p_obj->p_face + f1)) {
        // oznac face f
        face.k = k;
        face.o = o;
        face.f = f;
        oe_pridej_group_face_list(p_cnf, &face, 1,
          GROUP_KONT | GROUP_OBJ | GROUP_FACE);
      }
    }
  }
}

// Oznaci placate plosky sceny
void oe_prikaz_scena_oznac_placate_plosky(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  EDIT_OBJEKT *p_obj;
  int k, o;

  oe_ungroup_face_all(p_cnf);

  oe_klist_reset(&k);
  while ((p_kont = oe_klist_next(p_cnf, &k))) {
    oe_olist_reset(&o);
    while ((p_obj = oe_olist_next(p_kont, &o))) {
      oe_oznac_placate_plosky(p_cnf, p_obj, k, o);
    }
  }

}

int blba_ploska(BOD * p_p1, BOD * p_p2, BOD * p_p3)
{
  BOD norm;

  if (podobny_bod(p_p1, p_p2) || podobny_bod(p_p1, p_p3)
    || podobny_bod(p_p2, p_p3))
    return (TRUE);
  calc_face_normal(p_p1, p_p2, p_p3, &norm);
  return (vektor_velikost(&norm) == 0.0f);
}

// test blizkosti kontejneru podle obalek !!
// prohledava to i v okolni scene
void oe_decimace_objektu(K_EDITOR * p_cnf, int k, int o, ELIMINATOR * p_elim)
{
  F_VERTEX face;
  GLMATRIX world;
  GLMATRIX wlow;
  EDIT_KONTEJNER *p_kont;
  EDIT_KONTEJNER *p_ktmp;
  EDIT_OBJEKT *p_obj;
  EDIT_OBJEKT *p_tmp;
  EDIT_OBJEKT *p_list = NULL;
  int f, v, facenum;
  int tf, tfacenum;
  int maska, flag;
  int spol, to, tk;
  int i, j, vnum1, vnum2;
  BOD *p_stred = NULL;
  BOD *p_norm = NULL;           // normalovy vektor na porovnavani
  BOD *p_transf_1, *p_transf_2;
  BOD p1, p2, p3;               // vertexy plosky ve world space
  BOD v1, v2, v3;               // vertexy plosky ve world space
  BOD norm, fnorm, ifnorm;

  p_kont = p_cnf->p_kont[k];    // skoumany kontejner
  p_obj = p_kont->p_obj[o];     // skoumany objekt
  facenum = p_obj->facenum;

  maska = KONT_STATIC;
  flag = p_kont->kflag & KONT_STATIC;

  mat_copy(&world, kont_world_matrix(p_kont));

  if (p_elim->flag & ELIM_STRED)
    p_stred = &p_elim->p;

  if (p_elim->flag & ELIM_NORMAL) {
    p_norm = &p_elim->norm;
    vektor_norm(p_norm);
  }

  if (p_elim->flag & (ELIM_INTELIGENT_1 | ELIM_INTELIGENT_2)) {
    oe_klist_reset(&tk);
    while ((p_ktmp = oe_klist_next_maska(p_cnf, &tk, maska, flag))) {
      if (p_ktmp != p_kont
        && vzdal_bodu_bod(&p_ktmp->obb.obb_stred,
          &p_obj->obb.obb_stred) < 3.0f) {
        oe_olist_reset(&to);
        while ((p_tmp = oe_olist_next(p_ktmp, &to))) {
          if (vzdal_bodu_bod(&p_tmp->obb.obb_stred,
              &p_obj->obb.obb_stred) < 3.0f) {
            p_tmp->p_tmp_next = p_list;
            p_tmp->p_tmp_top = p_ktmp;
            p_list = p_tmp;
          }
        }
      }
    }
  }

  for (f = 0; f < facenum; f += 3) {
    v = p_obj->p_face[f];
    p1.x = p_obj->p_vertex[v].x;
    p1.y = p_obj->p_vertex[v].y;
    p1.z = p_obj->p_vertex[v].z;

    v = p_obj->p_face[f + 1];
    p2.x = p_obj->p_vertex[v].x;
    p2.y = p_obj->p_vertex[v].y;
    p2.z = p_obj->p_vertex[v].z;

    v = p_obj->p_face[f + 2];
    p3.x = p_obj->p_vertex[v].x;
    p3.y = p_obj->p_vertex[v].y;
    p3.z = p_obj->p_vertex[v].z;

    if (blba_ploska(&p1, &p2, &p3)) {
      face.k = k;
      face.o = o;
      face.f = f;
      oe_pridej_group_face_list(p_cnf, &face, 1,
        GROUP_KONT | GROUP_OBJ | GROUP_FACE);
      continue;
    }

    transformuj_bod_bod_matici(&p1, &world);
    transformuj_bod_bod_matici(&p2, &world);
    transformuj_bod_bod_matici(&p3, &world);

    calc_face_normal(&p1, &p2, &p3, &fnorm);
    vektor_norm(&fnorm);

    if (p_elim->flag & (ELIM_NORMAL | ELIM_STRED)) {

      if (p_elim->flag & ELIM_STRED) {
        norm.x = (p1.x + p2.x + p3.x) / 3.0f;
        norm.y = (p1.y + p2.y + p3.y) / 3.0f;
        norm.z = (p1.z + p2.z + p3.z) / 3.0f;

        vektor_sub(p_stred, &norm, &norm);
        vektor_norm(&norm);

        if (acosf(vektor_dot_product(&fnorm, &norm)) <= p_elim->uhel) {
          face.f = f;
          face.o = o;
          face.k = k;
          oe_pridej_group_face_list(p_cnf, &face, 1,
            GROUP_KONT | GROUP_OBJ | GROUP_FACE);
          continue;
        }
      }

      if (p_elim->flag & ELIM_NORMAL) {
        if (acosf(vektor_dot_product(&fnorm, p_norm)) <= p_elim->uhel) {
          face.f = f;
          face.o = o;
          face.k = k;
          oe_pridej_group_face_list(p_cnf, &face, 1,
            GROUP_KONT | GROUP_OBJ | GROUP_FACE);
          continue;
        }
      }
    }

    /* Inteligentni eliminator c.1 -> podle plosek a spolecnych bodu
     */
    if (p_elim->flag & ELIM_INTELIGENT_1) {
      while (p_list) {
        // prusek obalek - hledam spolecne facy-vertexy mezi p_obj a p_list
        p_ktmp = p_list->p_tmp_top;

        mat_copy(&wlow, kont_world_matrix(p_ktmp));

        tfacenum = p_list->facenum;
        for (tf = 0; tf < tfacenum; tf += 3) {

          v = p_list->p_face[tf];
          v1.x = p_list->p_vertex[v].x;
          v1.y = p_list->p_vertex[v].y;
          v1.z = p_list->p_vertex[v].z;

          v = p_list->p_face[tf + 1];
          v2.x = p_list->p_vertex[v].x;
          v2.y = p_list->p_vertex[v].y;
          v2.z = p_list->p_vertex[v].z;

          v = p_list->p_face[tf + 2];
          v3.x = p_list->p_vertex[v].x;
          v3.y = p_list->p_vertex[v].y;
          v3.z = p_list->p_vertex[v].z;

          transformuj_bod_bod_matici(&v1, &wlow);
          transformuj_bod_bod_matici(&v2, &wlow);
          transformuj_bod_bod_matici(&v3, &wlow);

          calc_face_normal(&v1, &v2, &v3, &ifnorm);
          vektor_norm(&ifnorm);

          ifnorm.x += fnorm.x;
          ifnorm.y += fnorm.y;
          ifnorm.z += fnorm.z;

          if (vektor_velikost(&ifnorm) < 0x000001) {
            spol = oe_spol_body(&p1, &p2, &p3, &v1, &v2, &v3);
            if (spol >= p_elim->spol_bodu_1) {
              if (spol < 3 && !oe_spol_prusecik(&p1, &p2, &p3, &v1, &v2, &v3))
                continue;
              face.f = f;
              face.o = o;
              face.k = k;
              oe_pridej_group_face_list(p_cnf, &face, 1,
                GROUP_KONT | GROUP_OBJ | GROUP_FACE);
              goto dalsi_face;
            }
          }
        }
        p_list = p_list->p_tmp_next;
      }
    }
  dalsi_face:;
  }

  /* Intreligentni decimator c.2 -> podle bodu a polohy plosek
   */
  if (p_elim->flag & ELIM_INTELIGENT_2) {

    // p_list -> seznam sousednich objektu
    while (p_list) {
      if (p_list->vertexnum != p_obj->vertexnum) {
        p_list = p_list->p_tmp_next;
        continue;
      }

      p_ktmp = p_list->p_tmp_top;
      mat_copy(&wlow, kont_world_matrix(p_ktmp));

      vnum1 = p_obj->vertexnum;
      vnum2 = p_list->vertexnum;

      p_transf_1 = malloc(sizeof(p_transf_1[0]) * vnum1);
      mtest(p_transf_1);
      p_transf_2 = malloc(sizeof(p_transf_2[0]) * vnum2);
      mtest(p_transf_2);

      // stejny pocet vertexu      
      for (i = 0; i < vnum1; i++)
        transformuj_bod_matici_bod((BOD *) (p_obj->p_vertex + i), &world,
          p_transf_1 + i);
      for (i = 0; i < vnum2; i++)
        transformuj_bod_matici_bod((BOD *) (p_list->p_vertex + i), &wlow,
          p_transf_2 + i);

      spol = 0;
      for (i = 0; i < vnum1; i++) {
        for (j = 0; j < vnum2; j++) {
          if (stejny_vertex_point_delta(p_transf_1 + i, p_transf_2 + j,
              p_elim->delta_vzdalenost)) {
            p_transf_1[i].x = p_transf_1[i].y = p_transf_1[i].z = FLT_MAX;
            spol++;
          }
        }
      }

      // Je kompletni zhoda
      if (spol == vnum1) {
        for (f = 0; f < facenum; f += 3) {      // smazu celej objekt
          face.f = f;
          face.o = o;
          face.k = k;
          oe_pridej_group_face_list(p_cnf, &face, 1,
            GROUP_KONT | GROUP_OBJ | GROUP_FACE);
        }
      }

      free(p_transf_1);
      free(p_transf_2);
      p_list = p_list->p_tmp_next;
    }
  }
}

void oe_prikaz_decimace_sceny(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  EDIT_OBJEKT *p_obj;
  int k, o;

  oe_rozmnoz_staticke(p_cnf);

  if (p_cnf->elm.flag & ELIM_MAZAT)
    oe_ungroup_face_all(p_cnf);
  p_cnf->groupflag = (GROUP_KONT | GROUP_OBJ | GROUP_FACE);
  for (k = 0; k < p_cnf->max_kont; k++) {
    p_kont = p_cnf->p_kont[k];
    if (p_kont &&
      ((p_cnf->elm.flag & ELIM_STATIC && p_kont->kflag & KONT_STATIC) ||
        (p_cnf->elm.flag & ELIM_DYNAMIC && !(p_kont->kflag & KONT_STATIC))) &&
      ((p_cnf->elm.flag & ELIM_PRVKY && p_kont->kflag & KONT_PRVEK) ||
        (p_cnf->elm.flag & ELIM_OKOLI && !(p_kont->kflag & KONT_PRVEK)))
      ) {

      for (o = 0; o < MAX_KONT_OBJEKTU; o++) {
        p_obj = p_kont->p_obj[o];
        if (p_obj) {
          oe_decimace_objektu(p_cnf, k, o, &p_cnf->elm);
        }
      }
    }
  }
}

void oe_prikaz_decimace_kontejneru(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  EDIT_OBJEKT *p_obj;
  int k = p_cnf->vybr_kont, o;

  oe_rozmnoz_staticke(p_cnf);

  if (p_cnf->elm.flag & ELIM_MAZAT)
    oe_ungroup_face_all(p_cnf);
  p_cnf->groupflag = (GROUP_KONT | GROUP_OBJ | GROUP_FACE);
  n_kont(k);
  p_kont = p_cnf->p_kont[k];
  if (p_kont) {
    for (o = 0; o < MAX_KONT_OBJEKTU; o++) {
      p_obj = p_kont->p_obj[o];
      if (p_obj) {
        oe_decimace_objektu(p_cnf, k, o, &p_cnf->elm);
      }
    }
  }
}

void oe_prikaz_decimace_vyberu(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  F_VERTEX *p_vyber;
  int v, vyber;
  int k, o;

  if (p_cnf->groupfacenum) {

    oe_rozmnoz_staticke(p_cnf);

    p_vyber = _alloca(sizeof(p_vyber[0]) * p_cnf->groupfacenum);
    memcpy(p_vyber, p_cnf->p_group_face,
      sizeof(p_vyber[0]) * p_cnf->groupfacenum);
    vyber = p_cnf->groupfacenum;

    null_free(&p_cnf->p_group_face);
    p_cnf->p_group_face = NULL;
    p_cnf->groupfacenum = 0;

    k = p_cnf->groupflag & GROUP_KONT;
    o = p_cnf->groupflag & GROUP_OBJ;

    p_cnf->groupflag = (GROUP_KONT | GROUP_OBJ | GROUP_FACE);

    /* Kontejnery a objekty
     */
    if (k && !o) {              // vyber podle kontejneru      
      for (v = 0; v < vyber; v++) {
        k = p_vyber[v].k;
        p_kont = p_cnf->p_kont[k];
        if (p_kont && ((p_kont->kflag & KONT_STATIC
              && p_cnf->elm.flag & ELIM_STATIC)
            || ((!(p_kont->kflag & KONT_STATIC)
                && p_cnf->elm.flag & ELIM_DYNAMIC)))) {
          for (o = 0; o < MAX_KONT_OBJEKTU; o++) {
            if (p_kont->p_obj[o])
              oe_decimace_objektu(p_cnf, k, o, &p_cnf->elm);
          }
        }
      }
    }
    /* Kontejnery
     */
    else if (k && o) {
      for (v = 0; v < vyber; v++) {
        k = p_vyber[v].k;
        o = p_vyber[v].o;
        if (p_cnf->p_kont[k] && p_cnf->p_kont[k]->p_obj[o])
          oe_decimace_objektu(p_cnf, k, o, &p_cnf->elm);
      }
    }
  }
}

void oe_prikaz_face_invert_kontejner(K_EDITOR * p_cnf)
{
  int k = p_cnf->vybr_kont;

  n_kont(k);
  kprintf(TRUE, "Invert kontejneru...");
  invertuj_face_kontejner(p_cnf->p_kont[k]);
}

void oe_prikaz_face_invert_objekt(K_EDITOR * p_cnf)
{
  int k = p_cnf->vybr_kont;
  int o = p_cnf->vybr_objekt;

  n_objekt(k, o);
  kprintf(TRUE, "Invert objektu...");
  invertuj_face_objektu(p_cnf->p_kont[k]->p_obj[o]);
}

void oe_prikaz_face_invert_vyber(K_EDITOR * p_cnf)
{
  word *p_face;
  word fcp[2];
  int i;
  int k, o, f;

  kprintf(TRUE, "Invert seznamu facu...");
  for (i = 0; i < p_cnf->groupfacenum; i++) {
    k = p_cnf->p_group_face[i].k;
    o = p_cnf->p_group_face[i].o;
    f = p_cnf->p_group_face[i].f;
    n_objekt(k, o);
    p_face = p_cnf->p_kont[k]->p_obj[o]->p_face + f;
    fcp[0] = p_face[0];
    fcp[1] = p_face[1];
    p_face[0] = fcp[1];
    p_face[1] = fcp[0];
  }
}

// decimuje vybrany trs facu
// fecenum*3 -> seznam vertexu (cislo + pocet hitu)
void oe_prikaz_decimace_trsu(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  EDIT_OBJEKT *p_obj;
  BOD n, n2;
  int *p_vhit;
  int i, vl[2], va, mhit, vmhit;
  int k, o, f;

  if (!p_cnf->groupfacenum) {
    kprintf(TRUE, "Nejsou vybrane zadne plosky...");
    return;
  }

  k = p_cnf->p_group_face[0].k;
  o = p_cnf->p_group_face[0].o;

  // 1 kontejner, 1 objekt
  for (i = 1; i < p_cnf->groupfacenum; i++) {
    if (k != p_cnf->p_group_face[i].k || o != p_cnf->p_group_face[i].o) {
      kprintf(TRUE, "Jsou vybrane plosky vice objektu/kontejneru...");
      return;
    }
  }

  if (!(p_kont = p_cnf->p_kont[k]) || !(p_obj = p_cnf->p_kont[k]->p_obj[o])) {
    kprintf(TRUE, "Bordel!");
    return;
  }

  p_vhit = _alloca(sizeof(p_vhit[0]) * p_obj->vertexnum);
  memset(p_vhit, 0, sizeof(p_vhit[0]) * p_obj->vertexnum);

  for (i = 0; i < p_cnf->groupfacenum; i++) {
    f = p_cnf->p_group_face[i].f;
    p_vhit[p_obj->p_face[f]]++;
    p_vhit[p_obj->p_face[f + 1]]++;
    p_vhit[p_obj->p_face[f + 2]]++;
  }

  va = 0;
  mhit = 0;
  vmhit = 0;
  for (i = 0; i < p_obj->vertexnum; i++) {
    if (p_vhit[i] > mhit) {
      mhit = p_vhit[i];
      vmhit = i;
    }
    if (p_vhit[i] == 1 && va < 2) {
      vl[va++] = i;
    }
  }

  /* vmhit - cislo nejvice hitoveho vertex
     mhit - pocet hitu

     vl[2] - seznamy 1-vertexu
   */
  /* Prepocitam normalovy vektor plosky
     a podle toho udelam novou
   */
  f = p_cnf->p_group_face[0].f;
  calc_face_normal((BOD *) (p_obj->p_vertex + p_obj->p_face[f]),
    (BOD *) (p_obj->p_vertex + p_obj->p_face[f + 1]),
    (BOD *) (p_obj->p_vertex + p_obj->p_face[f + 2]), &n);
  vektor_norm(&n);

  oe_prikaz_smaz_face(p_cnf, FALSE);
  oe_ungroup_face_all(p_cnf);

  p_obj->facenum += 3;
  p_obj->p_face =
    realloc(p_obj->p_face, p_obj->facenum * sizeof(p_obj->p_face[0]));
  mtest(p_obj->p_face);

  f = p_obj->facenum - 3;
  p_obj->p_face[f] = vmhit;
  p_obj->p_face[f + 1] = vl[0];
  p_obj->p_face[f + 2] = vl[1];
  calc_face_normal((BOD *) (p_obj->p_vertex + p_obj->p_face[f]),
    (BOD *) (p_obj->p_vertex + p_obj->p_face[f + 1]),
    (BOD *) (p_obj->p_vertex + p_obj->p_face[f + 2]), &n2);
  vektor_norm(&n2);
  vektor_add(&n, &n2, &n2);
  if (vektor_velikost(&n2) < 0.5f) {
    p_obj->p_face[f + 1] = vl[1];
    p_obj->p_face[f + 2] = vl[0];
  }

  kprintf(TRUE, "Optimalizace OK");
}

/* Decimace pruhu trojuhelniku
*/
void oe_prikaz_decimace_pruhu(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  EDIT_OBJEKT *p_obj;
  int *p_vhit;
  int i, vl1[10], vl2[10], va1, va2;
  int k, o, f;

  if (!p_cnf->groupfacenum) {
    kprintf(TRUE, "Nejsou vybrane zadne plosky...");
    return;
  }

  k = p_cnf->p_group_face[0].k;
  o = p_cnf->p_group_face[0].o;

  // 1 kontejner, 1 objekt
  for (i = 1; i < p_cnf->groupfacenum; i++) {
    if (k != p_cnf->p_group_face[i].k || o != p_cnf->p_group_face[i].o) {
      kprintf(TRUE, "Jsou vybrane plosky vice objektu/kontejneru...");
      return;
    }
  }

  if (!(p_kont = p_cnf->p_kont[k]) || !(p_obj = p_cnf->p_kont[k]->p_obj[o])) {
    kprintf(TRUE, "Bordel!");
    return;
  }

  p_vhit = _alloca(sizeof(p_vhit[0]) * p_obj->vertexnum);
  memset(p_vhit, 0, sizeof(p_vhit[0]) * p_obj->vertexnum);

  for (i = 0; i < p_cnf->groupfacenum; i++) {
    f = p_cnf->p_group_face[i].f;
    p_vhit[p_obj->p_face[f]]++;
    p_vhit[p_obj->p_face[f + 1]]++;
    p_vhit[p_obj->p_face[f + 2]]++;
  }

  va1 = va2 = 0;
  for (i = 0; i < p_obj->vertexnum; i++) {
    if (p_vhit[i] == 1 && va1 < 10) {
      vl1[va1++] = i;
      continue;
    }
    if (p_vhit[i] == 2 && va2 < 10) {
      vl2[va2++] = i;
      continue;
    }
  }

  if (va1 != 2 || va2 != 2) {
    kprintf(FALSE, "Spatny vyber! va1 = %d va2 = %d", va1, va2);
    oe_prikaz_all_ungroup_vertex(p_cnf);
    for (i = 0; i < p_obj->vertexnum; i++) {
      if (p_vhit[i] == 2) {
        oe_pridej_groupvertex(p_cnf, k, o, i);
      }
    }
    return;
  }

  oe_prikaz_smaz_face(p_cnf, FALSE);
  oe_ungroup_face_all(p_cnf);

  p_obj->facenum += 6;
  p_obj->p_face =
    realloc(p_obj->p_face, p_obj->facenum * sizeof(p_obj->p_face[0]));
  mtest(p_obj->p_face);

  f = p_obj->facenum - 6;
  p_obj->p_face[f] = vl1[0];
  p_obj->p_face[f + 1] = vl2[0];
  p_obj->p_face[f + 2] = vl2[1];
  f += 3;
  p_obj->p_face[f] = vl1[1];
  p_obj->p_face[f + 1] = vl2[1];
  p_obj->p_face[f + 2] = vl2[0];

  kprintf(TRUE, "Optimalizace OK");
}

/* Decimuje vybrany pruh plosek podle koncovych bodu
*/
void oe_prikaz_decimace_pruhu_body(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  EDIT_OBJEKT *p_obj;
  BOD n, n2, b, c;
  int i, vl[4];
  int k, o, f;
  int v[3];
  int j, l, hit;

  if (!p_cnf->groupfacenum) {
    kprintf(TRUE, "Nejsou vybrane zadne plosky...");
    return;
  }

  k = p_cnf->p_group_face[0].k;
  o = p_cnf->p_group_face[0].o;

  // 1 kontejner, 1 objekt
  for (i = 1; i < p_cnf->groupfacenum; i++) {
    if (k != p_cnf->p_group_face[i].k || o != p_cnf->p_group_face[i].o) {
      kprintf(TRUE, "Jsou vybrane plosky vice objektu/kontejneru...");
      return;
    }
  }

  if (!(p_kont = p_cnf->p_kont[k]) || !(p_obj = p_cnf->p_kont[k]->p_obj[o])) {
    kprintf(TRUE, "Bordel!");
    return;
  }

  if (p_cnf->groupnum != 4) {
    kprintf(FALSE, "Spatny pocet bodu!");
    return;
  }

  hit = 0;
  for (i = 0; i < p_cnf->groupfacenum; i++) {
    f = p_cnf->p_group_face[i].f;
    v[0] = p_obj->p_face[f];
    v[1] = p_obj->p_face[f + 1];
    v[2] = p_obj->p_face[f + 2];

    for (l = 0; l < 3; l++)
      for (j = 0; j < 4; j++)
        if (v[l] == p_cnf->p_group[j].v)
          vl[hit++] = p_cnf->p_group[j].v;
    if (hit == 1)
      hit = 0;
    else if (hit > 1)           // tato ploska ma 2 body z vyberu
      break;
  }

  for (i = 0; i < p_cnf->groupnum; i++) {
    for (j = 0; j <= hit; j++)
      if (vl[j] == p_cnf->p_group[i].v)
        goto knc;
    vl[hit++] = p_cnf->p_group[i].v;
    assert(hit < 5);
  knc:;
  }


  /* Rozlozeni vertexu:
     strana 1: 
     0,1
     strana 2:
     2,3
   */

  /* Podle vektoroveho soucinu zjistim
     ctverec
   */
  vektor_sub((BOD *) (p_obj->p_vertex + vl[2]),
    (BOD *) (p_obj->p_vertex + vl[0]), &b);
  vektor_sub((BOD *) (p_obj->p_vertex + vl[2]),
    (BOD *) (p_obj->p_vertex + vl[1]), &c);

  if (vektor_velikost(&b) > vektor_velikost(&c)) {
    v[0] = vl[0];               // prohodim body 0,1
    vl[0] = vl[1];
    vl[1] = v[0];
  }

  f = p_cnf->p_group_face[0].f;
  calc_face_normal((BOD *) (p_obj->p_vertex + p_obj->p_face[f]),
    (BOD *) (p_obj->p_vertex + p_obj->p_face[f + 1]),
    (BOD *) (p_obj->p_vertex + p_obj->p_face[f + 2]), &n);
  vektor_norm(&n);

  oe_prikaz_smaz_face(p_cnf, FALSE);
  oe_ungroup_face_all(p_cnf);

  p_obj->facenum += 6;
  p_obj->p_face =
    realloc(p_obj->p_face, p_obj->facenum * sizeof(p_obj->p_face[0]));
  mtest(p_obj->p_face);

  f = p_obj->facenum - 6;
  p_obj->p_face[f] = vl[0];
  p_obj->p_face[f + 1] = vl[1];
  p_obj->p_face[f + 2] = vl[2];
  calc_face_normal((BOD *) (p_obj->p_vertex + p_obj->p_face[f]),
    (BOD *) (p_obj->p_vertex + p_obj->p_face[f + 1]),
    (BOD *) (p_obj->p_vertex + p_obj->p_face[f + 2]), &n2);
  vektor_norm(&n2);
  vektor_add(&n, &n2, &n2);
  if (vektor_velikost(&n2) < 0.5f) {
    p_obj->p_face[f + 1] = vl[2];
    p_obj->p_face[f + 2] = vl[1];
  }

  f += 3;
  p_obj->p_face[f] = vl[2];
  p_obj->p_face[f + 1] = vl[1];
  p_obj->p_face[f + 2] = vl[3];
  calc_face_normal((BOD *) (p_obj->p_vertex + p_obj->p_face[f]),
    (BOD *) (p_obj->p_vertex + p_obj->p_face[f + 1]),
    (BOD *) (p_obj->p_vertex + p_obj->p_face[f + 2]), &n2);
  vektor_norm(&n2);
  vektor_add(&n, &n2, &n2);
  if (vektor_velikost(&n2) < 0.5f) {
    p_obj->p_face[f + 1] = vl[3];
    p_obj->p_face[f + 2] = vl[1];
  }

  kprintf(TRUE, "Optimalizace OK");
}

/*
  Ozvaci vertexy, ktere jsou na stejnych pozicich ale maji jine mapovani
  na plosky - pak je sesiju
*/
void oe_objekt_izolovane_vertexy(K_EDITOR * p_cnf, int k, int o)
{
  EDIT_KONTEJNER *p_kont;
  EDIT_OBJEKT *p_obj;
  int *p_vhit;                  // pole spolecnych vertexu
  int i, j;

  if (!(p_kont = p_cnf->p_kont[k]) || !(p_obj = p_cnf->p_kont[k]->p_obj[o])) {
    kprintf(TRUE, "Neni aktivni zadny objekt/kontejner.");
    return;
  }

  p_vhit = _alloca(sizeof(p_vhit[0]) * p_obj->vertexnum);
  memset(p_vhit, 0, sizeof(p_vhit[0]) * p_obj->vertexnum);

  oe_prikaz_all_ungroup_vertex(p_cnf);
  for (i = 0; i < p_obj->vertexnum; i++) {
    for (j = i + 1; j < p_obj->vertexnum; j++) {
      if (stejny_vertex_point_delta((BOD *) (p_obj->p_vertex + i),
          (BOD *) (p_obj->p_vertex + j), DELTA_SLICKA)) {
        p_vhit[i]++;
        p_vhit[j]++;
      }
    }
    if (p_vhit[i]) {
      oe_pridej_groupvertex(p_cnf, k, o, i);
    }
  }
}

/* Jede na vybrane plosky
*/
void oe_face_izolovane_vertexy(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  EDIT_OBJEKT *p_obj;
  int *p_vhit;                  // pole spolecnych vertexu
  int i, j, v;
  int k, o, f, fm;
  int vf, fv, mfv;

  if (!p_cnf->groupfacenum) {
    kprintf(TRUE, "Nejsou vybrane zadne plosky...");
    return;
  }

  k = p_cnf->p_group_face[0].k;
  o = p_cnf->p_group_face[0].o;

  // 1 kontejner, 1 objekt
  for (i = 1; i < p_cnf->groupfacenum; i++) {
    if (k != p_cnf->p_group_face[i].k || o != p_cnf->p_group_face[i].o) {
      kprintf(TRUE, "Jsou vybrane plosky vice objektu/kontejneru...");
      return;
    }
  }

  if (!(p_kont = p_cnf->p_kont[k]) || !(p_obj = p_cnf->p_kont[k]->p_obj[o])) {
    kprintf(TRUE, "Bordel!");
    return;
  }

  p_vhit = _alloca(sizeof(p_vhit[0]) * p_obj->vertexnum);
  memset(p_vhit, 0, sizeof(p_vhit[0]) * p_obj->vertexnum);

  oe_prikaz_all_ungroup_vertex(p_cnf);
  for (v = 0; v < p_cnf->groupfacenum; v++) {
    for (f = p_cnf->p_group_face[v].f, fm = f + 3; f < fm; f++) {
      i = p_obj->p_face[f];
      for (vf = 0; vf < p_cnf->groupfacenum; vf++) {
        if (vf != v) {
          for (fv = p_cnf->p_group_face[vf].f, mfv = fv + 3; fv < mfv; fv++) {
            j = p_obj->p_face[fv];
            if (i != j
              && stejny_vertex_point_delta((BOD *) (p_obj->p_vertex + i),
                (BOD *) (p_obj->p_vertex + j), DELTA_SLICKA)) {
              p_vhit[i]++;
              p_vhit[j]++;
            }
          }
        }
      }
    }
  }
  for (v = 0; v < p_obj->vertexnum; v++) {
    if (p_vhit[v])
      oe_pridej_groupvertex(p_cnf, k, o, v);
  }
}

EDIT_OBJEKT *oe_vyrob_sub_objekt(OBJ_VERTEX * body, int dx, int dz,
  int px, int pz, int material)
{
  EDIT_OBJEKT *p_obj = vyrob_objekt_komplet(4, 6);
  BOD dirx, dirz;
  OBJ_VERTEX *p_start = body;
  int i;

  vektor_mult_skalar(vektor_sub((BOD *) (body + 1), (BOD *) body, &dirx),
    1.0f / (float) dx, &dirx);
  vektor_mult_skalar(vektor_sub((BOD *) (body + 2), (BOD *) body, &dirz),
    1.0f / (float) dz, &dirz);

  for (i = 0; i < 4; i++)
    p_obj->p_vertex[i] = body[i];

  p_obj->material = material;

  p_obj->p_vertex[0].x = p_start->x + dirx.x * px + dirz.x * pz;
  p_obj->p_vertex[0].y = p_start->y + dirx.y * px + dirz.y * pz;
  p_obj->p_vertex[0].z = p_start->z + dirx.z * px + dirz.z * pz;
  p_obj->p_vertex[0].tu1 = 0;
  p_obj->p_vertex[0].tv1 = 0;

  p_obj->p_vertex[1].x = p_start->x + dirx.x * (px + 1) + dirz.x * pz;
  p_obj->p_vertex[1].y = p_start->y + dirx.y * (px + 1) + dirz.y * pz;
  p_obj->p_vertex[1].z = p_start->z + dirx.z * (px + 1) + dirz.z * pz;
  p_obj->p_vertex[1].tu1 = 1;
  p_obj->p_vertex[1].tv1 = 0;

  p_obj->p_vertex[2].x = p_start->x + dirx.x * (px + 1) + dirz.x * (pz + 1);
  p_obj->p_vertex[2].y = p_start->y + dirx.y * (px + 1) + dirz.y * (pz + 1);
  p_obj->p_vertex[2].z = p_start->z + dirx.z * (px + 1) + dirz.z * (pz + 1);
  p_obj->p_vertex[2].tu1 = 1;
  p_obj->p_vertex[2].tv1 = 1;

  p_obj->p_vertex[3].x = p_start->x + dirx.x * px + dirz.x * (pz + 1);
  p_obj->p_vertex[3].y = p_start->y + dirx.y * px + dirz.y * (pz + 1);
  p_obj->p_vertex[3].z = p_start->z + dirx.z * px + dirz.z * (pz + 1);
  p_obj->p_vertex[3].tu1 = 0;
  p_obj->p_vertex[3].tv1 = 1;

  p_obj->p_face[0] = 0;
  p_obj->p_face[1] = 2;
  p_obj->p_face[2] = 1;
  p_obj->p_face[3] = 0;
  p_obj->p_face[4] = 3;
  p_obj->p_face[5] = 2;

  return (p_obj);
}

void setrid_body_ctverec(OBJ_VERTEX * p_body)
{
  // 0 je vztazny bod
  OBJ_VERTEX tmp[4];
  float vel[4];
  BOD v[4];
  int i;

  for (i = 1; i < 4; i++)
    tmp[i] = p_body[i];

  vektor_sub((BOD *) (p_body + 1), (BOD *) p_body, &v[1]);
  vektor_sub((BOD *) (p_body + 2), (BOD *) p_body, &v[2]);
  vektor_sub((BOD *) (p_body + 3), (BOD *) p_body, &v[3]);

  for (i = 1; i < 4; i++)
    vel[i] = vektor_velikost((BOD *) (v + i));

  if (vel[1] > vel[2] && vel[1] > vel[3]) {
    p_body[3] = tmp[1];
    if (vel[2] > vel[3]) {
      p_body[1] = tmp[2];
      p_body[2] = tmp[3];
    }
    else {
      p_body[1] = tmp[3];
      p_body[2] = tmp[2];
    }
  }
  else if (vel[2] > vel[1] && vel[2] > vel[3]) {
    p_body[3] = tmp[2];
    if (vel[1] > vel[3]) {
      p_body[1] = tmp[1];
      p_body[2] = tmp[3];
    }
    else {
      p_body[1] = tmp[3];
      p_body[2] = tmp[1];
    }
  }
  else if (vel[3] > vel[1] && vel[3] > vel[2]) {
    p_body[3] = tmp[3];
    if (vel[1] > vel[2]) {
      p_body[1] = tmp[1];
      p_body[2] = tmp[2];
    }
    else {
      p_body[1] = tmp[2];
      p_body[2] = tmp[1];
    }
  }
}

void setrid_body_ctverec_bod(BOD * p_body)
{
  // 0 je vztazny bod
  float vel[4];
  BOD tmp[4];
  BOD v[4];
  int i;

  for (i = 1; i < 4; i++)
    tmp[i] = p_body[i];

  vektor_sub(p_body + 1, p_body, &v[1]);
  vektor_sub(p_body + 2, p_body, &v[2]);
  vektor_sub(p_body + 3, p_body, &v[3]);

  for (i = 1; i < 4; i++)
    vel[i] = vektor_velikost((BOD *) (v + i));

  if (vel[1] > vel[2] && vel[1] > vel[3]) {
    p_body[3] = tmp[1];
    if (vel[2] > vel[3]) {
      p_body[1] = tmp[2];
      p_body[2] = tmp[3];
    }
    else {
      p_body[1] = tmp[3];
      p_body[2] = tmp[2];
    }
  }
  else if (vel[2] > vel[1] && vel[2] > vel[3]) {
    p_body[3] = tmp[2];
    if (vel[1] > vel[3]) {
      p_body[1] = tmp[1];
      p_body[2] = tmp[3];
    }
    else {
      p_body[1] = tmp[3];
      p_body[2] = tmp[1];
    }
  }
  else if (vel[3] > vel[1] && vel[3] > vel[2]) {
    p_body[3] = tmp[3];
    if (vel[1] > vel[2]) {
      p_body[1] = tmp[1];
      p_body[2] = tmp[2];
    }
    else {
      p_body[1] = tmp[2];
      p_body[2] = tmp[1];
    }
  }
}

void oe_prikaz_teselace_objektu(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  int k = p_cnf->vybr_kont;
  int o = p_cnf->vybr_objekt;
  int material;
  OBJ_VERTEX body[4];
  EDIT_OBJEKT *p_obj;
  int dx = 2, dz = 2, x, z, i;

  n_objekt(k, o);

  // Vyber bodu jak se to ma rozdelit
  if (!doe_teselace_objektu(&dx, &dz))
    return;

  transformuj_kontejner(p_cnf->p_kont[k]);

  p_obj = p_cnf->p_kont[k]->p_obj[o];
  material = p_obj->material;

  for (i = 0; i < 4; i++)
    body[i] = p_obj->p_vertex[i];
  setrid_body_ctverec(body);

  zrus_objekt(p_cnf->p_kont[k]->p_obj + o);

  for (z = 0; z < dz; z++) {
    for (x = 0; x < dx; x++) {
      k = lo_najdi_volny_kontejner(p_cnf->p_kont, KONT_NUM);
      if (k == K_CHYBA)
        kprintf(TRUE, "DOPRDELE!!!!!");
      p_kont = p_cnf->p_kont[k] = vyrob_kontejner();
      p_kont->p_obj[0] = oe_vyrob_sub_objekt(body, dx, dz, x, z, material);
      updatuj_kontejner_statistika(p_kont, TRUE);
    }
  }
}

/*
*/
void oe_prikaz_rozmnoz_texture_koord_objekt(EDIT_OBJEKT * p_obj, int src,
  int dest)
{
  BODUV *p_src, *p_dest;
  int i;

  for (i = 0; i < p_obj->vertexnum; i++) {
    p_src = (BODUV *) & p_obj->p_vertex[i].tu1;
    p_dest = (BODUV *) & p_obj->p_vertex[i].tu1;
    *(p_dest + dest) = *(p_src + src);
  }
}

/*
*/
void oe_prikaz_rozmnoz_texture_koord(K_EDITOR * p_cnf, int src, int dest)
{
  EDIT_KONTEJNER *p_kont;
  int o, i;

  if (p_cnf->groupfacenum && p_cnf->groupflag & GROUP_KONT &&
    !(p_cnf->groupflag & (GROUP_OBJ | GROUP_FACE))) {
    for (i = 0; i < p_cnf->groupfacenum; i++) {
      p_kont = p_cnf->p_kont[p_cnf->p_group_face[i].k];
      for (o = 0; o < MAX_KONT_OBJEKTU; o++) {
        if (p_kont->p_obj[o])
          oe_prikaz_rozmnoz_texture_koord_objekt(p_kont->p_obj[o], src, dest);
      }
    }
  }
  else {
    int k = p_cnf->vybr_kont;

    n_kont(k);
    p_kont = p_cnf->p_kont[k];
    for (o = 0; o < MAX_KONT_OBJEKTU; o++) {
      if (p_kont->p_obj[o])
        oe_prikaz_rozmnoz_texture_koord_objekt(p_kont->p_obj[o], src, dest);
    }
  }
}

/* Preidnexuj kontejner pouze v pripade se nema sim-animaci
*/
void oe_preindexuj_kontejner(EDIT_KONTEJNER * p_kont)
{
  int i;

  for (i = 0; i < KONT_MAX_ANIM; i++) {
    if (p_kont->sim[i].keynum)
      return;
  }

  // jinak proved preindexaci kontejneru
  for (i = 0; i < MAX_KONT_OBJEKTU; i++) {
    if (p_kont->p_obj[i])
      p_kont->p_obj[i]->Objekt_ID = i;
  }
}

void oe_prikaz_preindexuj_kontejner(K_EDITOR * p_cnf)
{
  int k = p_cnf->vybr_kont;

  n_kont(k);
  oe_preindexuj_kontejner(p_cnf->p_kont[k]);
}


int najdi_vertex_koord(MUJ_BOD * p_vertexy, int vertexnum, MUJ_BOD * p_bod)
{
  int i;

  for (i = 0; i < vertexnum; i++) {
    if (p_vertexy[i].x == p_bod->x &&
      p_vertexy[i].y == p_bod->y &&
      p_vertexy[i].z == p_bod->z &&
      p_vertexy[i].tu1 == p_bod->tu1 && p_vertexy[i].tv1 == p_bod->tv1) {
      return (i);
    }
  }
  return (K_CHYBA);
}

void oe_calc_rel_vzdal(MUJ_BOD * p_vertex, int num, float *p_vzdal,
  BOD * p_str, float max_vzdal)
{
  float vzd;
  int i;

  for (i = 0; i < num; i++) {
    vzd = vzdal_bodu_bod((BOD *) (p_vertex + i), p_str);
    p_vzdal[i] = vzd / max_vzdal;
  }
}

void oe_prikaz_kopiruj_objekt_geometrie(K_EDITOR * p_cnf)
{
  int k = p_cnf->vybr_kont, o = p_cnf->vybr_objekt;
  int lk = p_cnf->vybr_kont_last, lo = p_cnf->vybr_objekt_last;

  n_objekt(k, o);
  if (p_cnf->p_kont[lk]->p_obj[lo])
    kopiruj_objekt_geometrie(p_cnf->p_kont[lk]->p_obj[lo],
      p_cnf->p_kont[k]->p_obj[o]);
}

#define RWBUFF_VEL 60000

int file_copy(byte * p_src, byte * p_out)
{
  FILE *in, *out;
  byte *p_buffer;
  int nc;

  in = fopen(p_src, "rb");
  if (!in)
    return (FALSE);
  out = fopen(p_out, "wb");
  if (!out) {
    fclose(in);
    return (FALSE);
  }

  p_buffer = malloc(RWBUFF_VEL);
  mtest(p_buffer);

  while ((nc = fread(p_buffer, 1, RWBUFF_VEL, in))) {
    fwrite(p_buffer, 1, nc, out);
  }

  free(p_buffer);
  fclose(in);
  fclose(out);
  return (TRUE);
}

int oe_kopiruj_texturu(byte * p_file, byte * p_srcdir, byte * p_outdir,
  byte * p_outfile)
{
  byte fullpath[500];
  byte srcfile[500];

  //byte  koncovka[500];
  byte *p_dir = NULL;
  int vys;

  // Kopie textury
  _chdir(p_srcdir);
  if (efile(p_file))
    p_dir = p_srcdir;
  else {
    int i;

    for (i = 0; i < TEXT_DIRS; i++) {
      _chdir(p_cnf->tdir.texture_dir[i]);
      if (efile(p_file)) {
        p_dir = p_cnf->tdir.texture_dir[i];
        break;
      }
    }
    if (!p_dir) {
      kprintf(TRUE, "oe_kopiruj_texturu: dir-chyba chyba (file %s)", p_file);
      return (FALSE);
    }
  }

  strcpy(fullpath, p_outdir);
  strcat(fullpath, "\\");
  strcat(fullpath, p_outfile);
  vys = file_copy(p_file, fullpath);
  if (vys) {
    zamen_koncovku(fullpath, KONCOVKA_TEXT_INFO);
    strcpy(srcfile, p_file);
    zamen_koncovku(srcfile, KONCOVKA_TEXT_INFO);
    file_copy(srcfile, fullpath);
  }
  else {
    kprintf(TRUE, "oe_kopiruj_texturu: file-copy chyba");
  }
  return (vys);
}

/* Out-dir je vystupni adresar
   prefix max 4 znaky
   1. projedu materialy
   2. projedu seznam textur a ty prejmenuju
*/
void oe_zkrat_materialy(K_EDITOR * p_cnf, byte * p_prefix, byte * p_strcdir,
  byte * p_outdir, int dos)
{
  EDIT_TEXT *p_text;
  byte file[256], *p_pom;
  int m, dl, d, v;
  char c;

  oe_oznac_pouzite_materialy(p_cnf);
  oe_oznac_pouzite_textury(p_cnf);

  for (m = 0, dl = 0; m < MAX_EDIT_TEXTUR; m++) {
    if (p_cnf->p_text[m].load && p_cnf->p_text[m].flag2 & TEXT2_POUZITA) {
      if (dos) {
        sprintf(file, "%s%.4d%s", p_prefix, dl,
          cti_koncovku(p_cnf->p_text[m].jmeno));
      }
      else {
        p_pom = p_cnf->p_text[m].jmeno;
        d = strlen(p_prefix);
        c = p_pom[d];
        p_pom[d] = 0;
        v = strcmp(p_prefix, p_pom);
        p_pom[d] = c;
        if (v) {
          strcpy(file, p_prefix);
          strcat(file, p_cnf->p_text[m].jmeno);
        }
      }
      kprintf(TRUE, "Text '%s' -> '%s'", p_cnf->p_text[m].jmeno, file);
      oe_kopiruj_texturu(p_cnf->p_text[m].jmeno, p_strcdir, p_outdir, file);
      strcpy(p_cnf->p_text[m].jmeno, file);
      dl++;
    }
  }

  for (m = 0; m < MAX_EDIT_MATERIALU; m++) {
    if (p_cnf->p_mat[m] && p_cnf->p_mat[m]->flag & MAT_POUZITY) {
      p_text = p_cnf->p_mat[m]->p_text[0];
      if (p_text) {
        strcpy(p_cnf->p_mat[m]->textfile[0], p_text->jmeno);
      }
      if (dos) {
        sprintf(p_cnf->p_mat[m]->jmeno, "Mat%.4d", m);
      }
    }
  }
}

// Zkrati jmena textur na format 8.3 a kopiruje
// textury + model do extra adresare
void oe_prikaz_exportuj_scenu(K_EDITOR * p_cnf)
{
  byte file[50];
  byte prefix[500] = "";
  int dos = 0;

  if (p_cnf->cnf.druh_sceny == SCENA_POLY) {
    if (!ddwqueto(hwnd_aplikace,
        "Level je preveden do hry. Ulozeni do standartni sceny nemusi byt nejlepsi. Ulozit ?"))
      return;
  }

  chdir(p_cnf->dir.scene_dir);
  if (doe_dialog_save(file, 49, "Ulozit scenu jako", MASKA_SCENY, "Scena")) {
    if (efile(file)
      && (MessageBox(hwnd_aplikace, "Tento soubor jiz existuje ! Prepsat ?",
          "KZH", MB_YESNO | MB_ICONASTERISK | MB_DEFBUTTON2) == IDNO))
      return;
    else {
      if (doe_export_sceny(hwnd_aplikace, prefix, &dos)) {
        oe_zkrat_materialy(p_cnf, prefix, p_cnf->tdir.scene_texture_dir,
          work_dir, dos);
        oe_oznac_pouzite_materialy(p_cnf);
        oe_uloz_projekt(p_cnf, file, work_dir, FALSE, FALSE);
      }
    }
  }
}

__inline static int oe_kontroluj_objekt(K_EDITOR * p_cnf, EDIT_OBJEKT * p_obj,
  int k, int o)
{
  return (p_cnf->p_kont[k] ? p_cnf->p_kont[k]->p_obj[o] ? p_cnf->
    p_kont[k]->p_obj[o] == p_obj : FALSE : FALSE);
}

JOINT *oe_kost_vyrob(K_EDITOR * p_cnf, EDIT_OBJEKT * p_obj, int *p_k,
  int *p_o)
{
  JOINT *p_ja;
  int i, k, o, v, b = 0;
  BOD *p_vert, str = { 0, 0, 0 };

  if (!p_obj)
    return (NULL);

  for (i = 0, b = 0; i < p_cnf->groupnum; i++) {
    k = p_cnf->p_group[i].k;
    o = p_cnf->p_group[i].o;
    if (oe_kontroluj_objekt(p_cnf, p_obj, k, o)) {
      v = p_cnf->p_group[i].v;
      p_vert = (BOD *) (p_cnf->p_kont[k]->p_obj[o]->p_vertex + v);
      vektor_add(p_vert, &str, &str);
      *p_k = k;
      *p_o = o;
      b++;
    }
  }
  vektor_mult_skalar(&str, 1.0f / (float) b, &str);

  p_ja = key_joint_vyrob();
  p_ja->pivot = str;

  p_ja->vertexnum = b;
  p_ja->p_vertexlist = mmalloc(b * sizeof(p_ja->p_vertexlist[0]));
  for (i = 0, b = 0; i < p_cnf->groupnum; i++) {
    k = p_cnf->p_group[i].k;
    o = p_cnf->p_group[i].o;
    if (oe_kontroluj_objekt(p_cnf, p_obj, k, o)) {
      p_ja->p_vertexlist[b] = p_cnf->p_group[i].v;
      b++;
    }
  }

  return (p_ja);
}

__inline static int *oe_je_v_seznamu(int *p_ind, int num, int hled)
{
  int i;

  for (i = 0; i < num; i++) {
    if (p_ind[i] == hled)
      return (p_ind + i);
  }
  return (FALSE);
}

void oe_kost_pridej_vertexy(K_EDITOR * p_cnf, JOINT * p_joint,
  EDIT_OBJEKT * p_obj)
{
  int i, k, o, v, b = 0;

  if (!p_joint || !p_obj)
    return;

  for (i = 0, b = 0; i < p_cnf->groupnum; i++) {
    k = p_cnf->p_group[i].k;
    o = p_cnf->p_group[i].o;
    if (oe_kontroluj_objekt(p_cnf, p_obj, k, o)) {
      v = p_cnf->p_group[i].v;
      if (!oe_je_v_seznamu(p_joint->p_vertexlist, p_joint->vertexnum, v)) {
        b++;
      }
    }
  }

  if (!b)
    return;

  p_joint->p_vertexlist =
    realloc(p_joint->p_vertexlist,
    (b + p_joint->vertexnum) * sizeof(p_joint->p_vertexlist[0]));
  mtest(p_joint->p_vertexlist);
  for (i = 0, b = p_joint->vertexnum; i < p_cnf->groupnum; i++) {
    k = p_cnf->p_group[i].k;
    o = p_cnf->p_group[i].o;
    if (oe_kontroluj_objekt(p_cnf, p_obj, k, o)) {
      v = p_cnf->p_group[i].v;
      if (!oe_je_v_seznamu(p_joint->p_vertexlist, p_joint->vertexnum, v)) {
        p_joint->p_vertexlist[b] = v;
        b++;
      }
    }
  }
  p_joint->vertexnum = b;
}

void oe_kost_uber_vertexy(K_EDITOR * p_cnf, JOINT * p_joint,
  EDIT_OBJEKT * p_obj)
{
  int *p_ind;
  int i, k, o, v, b = 0;

  if (!p_joint || !p_obj)
    return;

  for (i = 0, b = 0; i < p_cnf->groupnum; i++) {
    k = p_cnf->p_group[i].k;
    o = p_cnf->p_group[i].o;
    if (oe_kontroluj_objekt(p_cnf, p_obj, k, o)) {
      v = p_cnf->p_group[i].v;
      if (p_ind =
        oe_je_v_seznamu(p_joint->p_vertexlist, p_joint->vertexnum, v)) {
        *p_ind = -1;
        b++;
      }
    }
  }

  if (!b)
    return;

  p_ind =
    mmalloc((p_joint->vertexnum - b) * sizeof(p_joint->p_vertexlist[0]));
  for (i = 0, b = 0; i < p_joint->vertexnum; i++) {
    if (p_joint->p_vertexlist[i] != -1) {
      p_ind[b] = p_joint->p_vertexlist[i];
      b++;
    }
  }
  free(p_joint->p_vertexlist);
  p_joint->p_vertexlist = p_ind;
  p_joint->vertexnum = b;
}

/*
*/

void oe_prikaz_kost_pridej_vertexy(K_EDITOR * p_cnf)
{
  if (p_cnf->jcf.p_joint_akt) {
    oe_kost_pridej_vertexy(p_cnf, p_cnf->jcf.p_joint_akt,
      p_cnf->p_kont[p_cnf->jcf.k]->p_obj[p_cnf->jcf.o]);
  }
}

void oe_prikaz_kost_uber_vertexy(K_EDITOR * p_cnf)
{
  if (p_cnf->jcf.p_joint_akt) {
    oe_kost_uber_vertexy(p_cnf, p_cnf->jcf.p_joint_akt,
      p_cnf->p_kont[p_cnf->jcf.k]->p_obj[p_cnf->jcf.o]);
  }
}

/* Nova joint-animace objektu
*/
void oe_prikaz_kost_pridej_start(K_EDITOR * p_cnf)
{
  EDIT_JOINT *p_jc = &p_cnf->jcf;
  int handle = oe_vybr_obj_reset();
  byte jmeno[MAX_JMENO] = { "Default" };

  if (!p_cnf->groupnum) {
    kprintf(TRUE, "Nejsou vybrane veretxy!");
    return;
  }

  if (!handle) {
    kprintf(TRUE, "Neni vybrany zadny objekt!");
    return;
  }
/*
  if(!doe_cti_jmeno_ext(hwnd_aplikace, jmeno, "Jmeno nove Joint-animace")) {
    return;
  }
*/

  {
    int k, o;
    JOINT_ANIMACE *p_ja;
    EDIT_OBJEKT *p_obj = oe_vybr_obj_get_ind(&handle, &k, &o);  // tohoto objektu se to tyka
    JOINT *p_j = oe_kost_vyrob(p_cnf, p_obj, &k, &o);

    if (!p_j)
      return;

    p_ja = mmalloc(sizeof(p_obj->p_joit_animace[0]));
    strcpy(p_ja->jmeno, jmeno);

    p_ja->p_child = p_j;
    p_ja->p_next = p_obj->p_joit_animace;
    p_obj->p_joit_animace = p_ja;

    p_jc->k = k;
    p_jc->o = o;
    p_jc->p_joint_akt = p_j;
    p_jc->p_joint_animace_akt = p_ja;


    if (!p_obj->p_vertex_kosti) {
      int i;

      p_obj->p_vertex_kosti =
        mmalloc(sizeof(p_obj->p_vertex_kosti[0]) * p_obj->vertexnum);
      for (i = 0; i < p_obj->vertexnum; i++) {
        mujbod2bod(p_obj->p_vertex_kosti + i, p_obj->p_vertex + i);
      }
    }

    doe_joint_animace_vloz_objekt(p_cnf);
    doe_joint_animace_vloz_frame(p_cnf);
    doe_joint_animace_vloz_tiky(p_cnf);
    doe_joint_animace_vloz_text(p_cnf);

    p_cnf->p_kont[k]->k2flag |= KONT2_JOINT_ANIM;
    p_cnf->p_kont[k]->kflag |= KONT_VERTEX_LIGHT;
    p_cnf->p_kont[k]->kflag &= ~KONT_STATIC;

    kprintf(TRUE, "Zakladam Kost");
  }
}

void oe_prikaz_kost_kopiruj(K_EDITOR * p_cnf)
{
  EDIT_JOINT *p_jc = &p_cnf->jcf;
  byte jmeno[MAX_JMENO] = { "Default" };

  if (!p_jc->p_joint_animace_akt) {
    kprintf(TRUE, "Neni vybrana animace");
    return;
  }

  if (doe_cti_jmeno_ext(hwnd_aplikace, jmeno, "Jmeno nove Joint-animace")) {
    EDIT_OBJEKT *p_obj = oe_cti_joint_objekt(p_cnf);
    JOINT_ANIMACE *p_anim = key_kosti_kopiruj(p_jc->p_joint_animace_akt);

    if (!p_anim)
      return;

    strcpy(p_anim->jmeno, jmeno);

    p_jc->p_joint_akt = p_anim->p_child;
    p_jc->p_joint_animace_akt = p_anim;

    p_anim->p_next = p_obj->p_joit_animace;
    p_obj->p_joit_animace = p_anim;

    doe_joint_animace_vloz_objekt(p_cnf);
    doe_joint_animace_vloz_frame(p_cnf);
    doe_joint_animace_vloz_tiky(p_cnf);
    doe_joint_animace_vloz_text(p_cnf);
  }
}

/* Navaze kost na aktualni kostni end-point
   Navaze to na aktualni kost
*/
void oe_prikaz_kost_pridej_navaz(K_EDITOR * p_cnf)
{
  EDIT_JOINT *p_conf = &p_cnf->jcf;
  EDIT_OBJEKT *p_obj;
  JOINT *p_new;
  int k = p_conf->k, o = p_conf->o;

  if (!p_cnf->groupnum) {
    ddw("Nejsou vybrane vertexy");
    return;
  }

  if (!p_cnf->p_kont[k] || !p_cnf->p_kont[k]->p_obj[o])
    return;
  p_obj = p_cnf->p_kont[k]->p_obj[o];

  if (!p_obj->p_joit_animace) {
    oe_prikaz_kost_pridej_start(p_cnf);
  }
  else {
    if (!p_conf->p_joint_akt) {
      ddw("Neni aktivni zadny uzel");
      return;
    }

    p_new = oe_kost_vyrob(p_cnf, p_obj, &k, &o);
    if (!p_new)
      return;

    if (p_conf->p_joint_akt->p_child) {
      p_new->p_next = p_conf->p_joint_akt->p_child->p_next;
      p_conf->p_joint_akt->p_child->p_next = p_new;
    }
    else {
      p_conf->p_joint_akt->p_child = p_new;
    }
    doe_joint_animace_vloz_objekt(p_cnf);
    doe_joint_animace_vloz_frame(p_cnf);
    doe_joint_animace_vloz_tiky(p_cnf);
    doe_joint_animace_vloz_text(p_cnf);

    p_conf->p_joint_akt = p_new;
    kprintf(TRUE, "Navazuju joint");
  }
}

/* Hleda predchudce kosti
*/
JOINT *oe_kost_najdi_rec(JOINT * p_joint, JOINT * p_hled)
{
  JOINT *t1, *t2;

  if (!p_joint)
    return (NULL);

  if (p_joint->p_child == p_hled || p_joint->p_next == p_hled)
    return (p_joint);

  t1 = oe_kost_najdi_rec(p_joint->p_child, p_hled);
  t2 = oe_kost_najdi_rec(p_joint->p_next, p_hled);

  return (t1 ? t1 : t2);
}

JOINT_ANIMACE *oe_kost_smaz_root(JOINT_ANIMACE * p_joint)
{
  JOINT_ANIMACE *p_next = p_joint->p_next;

  zrus_kosti_rec(&p_joint->p_child);
  free(p_joint);
  return (p_next);
}

JOINT_ANIMACE *oe_kost_animaci_smaz(K_EDITOR * p_cnf, EDIT_OBJEKT * p_obj,
  JOINT_ANIMACE * p_maz)
{
  if (p_obj->p_joit_animace == p_maz) {
    p_obj->p_joit_animace = oe_kost_smaz_root(p_maz);
    return (p_obj->p_joit_animace);
  }
  else {
    JOINT_ANIMACE *p_akt = p_obj->p_joit_animace->p_next,
      *p_prev = p_obj->p_joit_animace;
    while (p_akt && p_akt != p_maz) {
      p_prev = p_akt;
      p_akt = p_akt->p_next;
    }
    if (p_akt) {
      p_prev->p_next = oe_kost_smaz_root(p_maz);
    }
    return (p_prev);
  }
}

/* Smaze kost
*/
void oe_prikaz_kost_smaz(K_EDITOR * p_cnf)
{
  EDIT_JOINT *p_conf = &p_cnf->jcf;
  EDIT_OBJEKT *p_obj = oe_cti_joint_objekt(p_cnf);
  JOINT *p_prev = NULL;

  if (!p_conf->p_joint_akt || !p_obj)
    return;

  if (p_conf->p_joint_animace_akt->p_child == p_conf->p_joint_akt) {    // smaz celou animaci
    p_conf->p_joint_animace_akt =
      oe_kost_animaci_smaz(p_cnf, p_obj, p_conf->p_joint_animace_akt);
    if (p_conf->p_joint_animace_akt)
      p_conf->p_joint_akt = p_conf->p_joint_animace_akt->p_child;
  }
  else {
    p_prev =
      oe_kost_najdi_rec(p_conf->p_joint_animace_akt->p_child,
      p_conf->p_joint_akt);
    if (!p_prev)
      return;
    if (p_prev->p_child == p_conf->p_joint_akt) {
      p_prev->p_child = NULL;
    }
    else {
      p_prev->p_next = p_conf->p_joint_akt->p_next;
    }
    zrus_kosti_rec(&p_conf->p_joint_akt);
    p_conf->p_joint_akt = p_prev;
  }
}

void oe_prikaz_kost_smaz_animaci(K_EDITOR * p_cnf)
{
  EDIT_JOINT *p_conf = &p_cnf->jcf;
  EDIT_OBJEKT *p_obj = oe_cti_joint_objekt(p_cnf);
  JOINT *p_prev = NULL;

  if (!p_conf->p_joint_animace_akt || !p_obj)
    return;

  p_conf->p_joint_animace_akt =
    oe_kost_animaci_smaz(p_cnf, p_obj, p_conf->p_joint_animace_akt);
  if (p_conf->p_joint_animace_akt)
    p_conf->p_joint_akt = p_conf->p_joint_animace_akt->p_child;
}

/* Vlozi parametry do klicu

#define POSUN_ROT    0x080
#define POSUN_SCALE  0x100
*/
void joint_vloz_do_klice(JOINT * p_joint, int flag)
{
  JOINT_KEYS *p_track;
  JOINT_ANIMACE *p_anim = p_cnf->jcf.p_joint_animace_akt;
  KEY_POINT_BRS nklic;
  int klic;
  int time;
  int frame;

  if (!p_joint || !p_anim)
    return;

  p_track = key_kost_spline_vyrob(p_joint);
  assert(p_track);

  time = p_anim->time;
  frame = time2frame(time);

  if (flag & POSUN_POZICI) {
    if ((klic =
        key_je_time_klic(p_track->p_pkeys, p_track->pos_keys,
          time)) == K_CHYBA) {
      memset(&nklic, 0, sizeof(nklic));
      nklic.time = time;
      p_track->pos_keys = key_track_klic_vloz((byte **) & p_track->p_pos,
        &p_track->p_pkeys, p_track->pos_keys,
        sizeof(p_track->p_pos[0]), (byte *) & p_joint->p, &nklic);
      doe_hs_setmark(frame);
    }
    else {
      memset(p_track->p_pkeys + klic, 0, sizeof(p_track->p_pkeys[klic]));
      p_track->p_pkeys[klic].time = time;
      p_track->p_pos[klic] = p_joint->p;
    }
  }

  if (flag & POSUN_ROT) {
    if ((klic =
        key_je_time_klic(p_track->p_rkeys, p_track->rot_keys,
          time)) == K_CHYBA) {
      memset(&nklic, 0, sizeof(nklic));
      nklic.time = time;
      p_track->rot_keys = key_track_klic_vloz((byte **) & p_track->p_rot,
        &p_track->p_rkeys, p_track->rot_keys,
        sizeof(p_track->p_rot[0]), (byte *) & p_joint->r, &nklic);
      doe_hs_setmark(frame);
    }
    else {
      memset(p_track->p_rkeys + klic, 0, sizeof(p_track->p_rkeys[klic]));
      p_track->p_rkeys[klic].time = time;
      p_track->p_rot[klic] = p_joint->r;
    }
  }

  if (flag & POSUN_SCALE) {
    if ((klic =
        key_je_time_klic(p_track->p_skeys, p_track->scs_keys,
          time)) == K_CHYBA) {
      memset(&nklic, 0, sizeof(nklic));
      nklic.time = time;
      p_track->scs_keys = key_track_klic_vloz((byte **) & p_track->p_scale,
        &p_track->p_skeys, p_track->scs_keys,
        sizeof(p_track->p_scale[0]), (byte *) & p_joint->s, &nklic);
      doe_hs_setmark(frame);
    }
    else {
      memset(p_track->p_skeys + klic, 0, sizeof(p_track->p_skeys[klic]));
      p_track->p_skeys[klic].time = time;
      p_track->p_scale[klic] = p_joint->s;
    }
  }
}


// Posune pivot/kost
void oe_prikaz_kost_posun(K_EDITOR * p_cnf, int pivot)
{
  JOINT *p_joint = p_cnf->jcf.p_joint_akt;
  JOINT_ANIMACE *p_animace = p_cnf->jcf.p_joint_animace_akt;
  EDIT_OBJEKT *p_obj = oe_cti_joint_objekt(p_cnf);
  int k = p_cnf->jcf.k;
  VEKTOR v1, v2;
  BOD *p_piv;
  int flag;
  GLMATRIX mat, *p_world;

  if (!p_joint || !p_obj)
    return;

  if (!pivot) {
    if (!p_animace || !p_animace->framenum) {
      kprintf(TRUE, "Neni aktivni animace!");
      return;
    }
    p_piv = &p_joint->p;
  }
  else {
    p_piv = &p_joint->pivot;
  }

  p_world = kont_world_matrix(p_cnf->p_kont[k]);
  calc_transf_3d_2d_matrix(p_world, p_cnf->p_camera, p_cnf->p_project, &mat);

  *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);

  transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
    &v1.x, &v1.y, &v1.z, &mat,
    OXRES, OYRES, OXSTART, OYSTART,
    p_cnf->kam.near_plane, p_cnf->kam.far_plane);
  nuluj_mys();
  while (stisk_mys()) {
    transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
      &v2.x, &v2.y, &v2.z, &mat,
      OXRES, OYRES, OXSTART, OYSTART,
      p_cnf->kam.near_plane, p_cnf->kam.far_plane);
    flag = 0;
    if (!key[K_X] && !key[K_Y] && !key[K_Z]) {
      p_piv->x += v2.x - v1.x;
      p_piv->y += v2.y - v1.y;
      p_piv->z += v2.z - v1.z;
      flag = POSUN_VSE;
    }
    else {
      if (key[K_X]) {
        p_piv->x += v2.x - v1.x;
        flag = POSUN_OSA_X;
      }
      if (key[K_Y]) {
        p_piv->y += v2.y - v1.y;
        flag = POSUN_OSA_Y;
      }
      if (key[K_Z]) {
        p_piv->z += v2.z - v1.z;
        flag = POSUN_OSA_Z;
      }
    }

    if (!pivot) {
      joint_vloz_do_klice(p_joint, POSUN_POZICI);
      doe_joint_animace_vloz_text(p_cnf);
      doe_joint_animace_vloz_tiky(p_cnf);
      key_kosti_interpoluj(p_animace);
      key_kosti_animuj(p_animace);
      key_kosti_updatuj_vertexy(p_obj, p_animace);
    }

    oe_renderuj_scenu_pre(p_cnf);
    oe_kresli_editacni_stred(p_cnf, p_piv, flag);
    oe_renderuj_scenu_po(p_cnf);

    transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
      &v1.x, &v1.y, &v1.z, &mat,
      OXRES, OYRES, OXSTART, OYSTART,
      p_cnf->kam.near_plane, p_cnf->kam.far_plane);
  }

  *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);
}

void oe_prikaz_kost_rotace(K_EDITOR * p_cnf)
{
  JOINT *p_joint = p_cnf->jcf.p_joint_akt;
  JOINT_ANIMACE *p_animace = p_cnf->jcf.p_joint_animace_akt;

// EDIT_OBJEKT *p_obj = oe_cti_joint_kont(p_cnf);
  EDIT_OBJEKT *p_obj = NULL;
  int k = p_cnf->jcf.k;
  BOD *p_piv;
  int flag;
  GLMATRIX mat, zal, vys;

  if (!p_joint) {
    kprintf(TRUE, "Neni aktivni joint!");
    return;
  }

  if (!p_animace || !p_animace->framenum) {
    kprintf(TRUE, "Neni aktivni animace!");
    return;
  }

  get_matrix_world(&zal);
  init_matrix(&mat);
  quat_to_matrix(&mat, &p_joint->r);
  p_piv = &p_joint->pivot;

  nuluj_mys();
  while (stisk_mys()) {
    flag = 0;
    if (key[K_X]) {
      rotate_matrix_x(&mat, ((float) PI / 180) * (float) mi.dx * 0.5f);
      flag = POSUN_OSA_X;
    }
    if (key[K_Y]) {
      rotate_matrix_y(&mat, ((float) PI / 180) * (float) mi.dx * 0.5f);
      flag = POSUN_OSA_Y;
    }
    if (key[K_Z]) {
      rotate_matrix_z(&mat, ((float) PI / 180) * (float) mi.dx * 0.5f);
      flag = POSUN_OSA_Z;
    }
    nuluj_dif();

    matrix_to_quat(&mat, &p_joint->r);

    joint_vloz_do_klice(p_joint, POSUN_ROT);
    doe_joint_animace_vloz_text(p_cnf);
    doe_joint_animace_vloz_tiky(p_cnf);
    key_kosti_interpoluj(p_animace);
    key_kosti_animuj(p_animace);
    key_kosti_updatuj_vertexy(p_obj, p_animace);

    oe_renderuj_scenu_pre(p_cnf);
    mat_mult_dir(&zal, &mat, &vys);
    set_matrix_world(&vys);
    oe_kresli_editacni_stred(p_cnf, p_piv, flag);
    oe_renderuj_scenu_po(p_cnf);
  }
  set_matrix_world(&zal);
}

// Posune pivot/kost
void oe_prikaz_kost_scale(K_EDITOR * p_cnf)
{
  JOINT *p_joint = p_cnf->jcf.p_joint_akt;
  JOINT_ANIMACE *p_animace = p_cnf->jcf.p_joint_animace_akt;
  EDIT_OBJEKT *p_obj = oe_cti_joint_objekt(p_cnf);
  int k = p_cnf->jcf.k;
  VEKTOR v1, v2;
  BOD *p_piv;
  int flag;
  GLMATRIX mat, *p_world;

  if (!p_joint)
    return;

  p_piv = &p_joint->s;

  p_world = kont_world_matrix(p_cnf->p_kont[k]);
  calc_transf_3d_2d_matrix(p_world, p_cnf->p_camera, p_cnf->p_project, &mat);

  *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);

  transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
    &v1.x, &v1.y, &v1.z, &mat,
    OXRES, OYRES, OXSTART, OYSTART,
    p_cnf->kam.near_plane, p_cnf->kam.far_plane);
  nuluj_mys();
  while (stisk_mys()) {
    transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
      &v2.x, &v2.y, &v2.z, &mat,
      OXRES, OYRES, OXSTART, OYSTART,
      p_cnf->kam.near_plane, p_cnf->kam.far_plane);
    flag = 0;
    if (!key[K_X] && !key[K_Y] && !key[K_Z]) {
      p_piv->x += v2.x - v1.x;
      p_piv->y += v2.y - v1.y;
      p_piv->z += v2.z - v1.z;
      flag = POSUN_VSE;
    }
    else {
      if (key[K_X]) {
        p_piv->x += v2.x - v1.x;
        flag = POSUN_OSA_X;
      }
      if (key[K_Y]) {
        p_piv->y += v2.y - v1.y;
        flag = POSUN_OSA_Y;
      }
      if (key[K_Z]) {
        p_piv->z += v2.z - v1.z;
        flag = POSUN_OSA_Z;
      }
    }

    joint_vloz_do_klice(p_joint, POSUN_SCALE);
    doe_joint_animace_vloz_text(p_cnf);
    doe_joint_animace_vloz_tiky(p_cnf);
    key_kosti_interpoluj(p_animace);
    key_kosti_animuj(p_animace);
    key_kosti_updatuj_vertexy(p_obj, p_animace);


    oe_renderuj_scenu_pre(p_cnf);
    oe_kresli_editacni_stred(p_cnf, p_piv, flag);
    oe_renderuj_scenu_po(p_cnf);

    transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
      &v1.x, &v1.y, &v1.z, &mat,
      OXRES, OYRES, OXSTART, OYSTART,
      p_cnf->kam.near_plane, p_cnf->kam.far_plane);
  }

  *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);
}

void oe_prikaz_smaz_jcf(K_EDITOR * p_cnf)
{
  EDIT_JOINT *p_jcf = &p_cnf->jcf;

  p_jcf->p_joint_akt = NULL;    // aktualni joint
  p_jcf->p_joint_animace_akt = NULL;    // aktualni joint-animace
}

/*
typedef struct _FLARE_GEN {

  STATIC_LIGHT   *p_slight; // svetlo ktere to haze
  BOD             p;        // gen-bod
  
  float           start;
  dword           start_color;
  
  float           konec;
  dword           konec_color;

} FLARE_GEN;
--generovat podle vybranych hodnot...pokud je mezi body face - udelej flare
--text koordinaty podle toho
--textura 256x1
*/

bitmapa *oe_vyrob_light_texturu(int delka, dword start, dword konec)
{
  bitmapa *p_bmp = bmp_vyrob(delka, 1);
  dword barva;
  int i;
  float krok[4];
  float sb[4];
  float kb[4];

  rgba_float(start, sb);
  rgba_float(konec, kb);

  kb[0] -= sb[0];
  kb[1] -= sb[1];
  kb[2] -= sb[2];
  kb[3] -= sb[3];

  krok[0] = kb[0] / (float) delka;
  krok[1] = kb[1] / (float) delka;
  krok[2] = kb[2] / (float) delka;
  krok[3] = kb[3] / (float) delka;

  barva = 0xff;

  for (i = 0; i < delka; i++) {
    barva = float_rgba(sb);
    bmp_putpixel(p_bmp, i, 0, barva);
    sb[0] += krok[0];
    sb[1] += krok[1];
    sb[2] += krok[2];
    sb[3] += krok[3];
  }

  return (p_bmp);
}

typedef struct _LINKA
{

  BOD p1, p2;
  int i1, i2;

} LINKA;

EDIT_OBJEKT *oe_vyrob_objekt_flare(K_EDITOR * p_cnf, FLARE_GEN * p_gen)
{
  GLMATRIX *p_word;
  int *p_flist;
  MUJ_BOD *p_vr;
  EDIT_OBJEKT *p_obj;
  EDIT_OBJEKT *p_flare;
  LINKA *p_linky;
  BOD *p_b1, *p_b2, *p_b3, *p_b4, a;
  float t, delka = p_gen->konec - p_gen->start, vzd[3];
  int body[3];
  int i, j, facu = 0;
  int k, o, f, v, linek;
  int l1, l2, l3, l4;

  k = p_cnf->p_group[0].k;
  o = p_cnf->p_group[0].o;

  p_obj = p_cnf->p_kont[k]->p_obj[o];
  p_word = kont_world_matrix(p_cnf->p_kont[k]);
  p_flist = mmalloc(sizeof(p_flist[0]) * p_obj->facenum);

  for (f = 0; f < p_obj->facenum; f++) {
    p_flist[f] = -1;
  }

  for (i = 0; i < p_cnf->groupnum; i++) {
    v = p_cnf->p_group[i].v;
    for (f = 0; f < p_obj->facenum; f++) {
      if (p_obj->p_face[f] == v) {
        p_flist[f] = p_obj->p_face[f];
      }
    }
  }

  // najdu face ktere maji 2 a 3 body
  linek = 0;
  for (f = 0; f < p_obj->facenum; f += 3) {
    v = 0;
    if (p_flist[f] != -1)
      v++;
    if (p_flist[f + 1] != -1)
      v++;
    if (p_flist[f + 2] != -1)
      v++;

    if (v == 2)
      linek += 1;
    if (v == 3)
      linek += 2;
  }

  p_linky = mmalloc(sizeof(p_linky[0]) * linek);
  linek = 0;
  for (f = 0; f < p_obj->facenum; f += 3) {
    v = 0;
    if (p_flist[f] != -1) {
      body[v] = p_flist[f];
      v++;
    }
    if (p_flist[f + 1] != -1) {
      body[v] = p_flist[f + 1];
      v++;
    }
    if (p_flist[f + 2] != -1) {
      body[v] = p_flist[f + 2];
      v++;
    }
    if (v == 3) {
      vzd[0] =
        vzdal_bodu(p_obj->p_vertex + p_flist[f],
        p_obj->p_vertex + p_flist[f + 1]);
      vzd[1] =
        vzdal_bodu(p_obj->p_vertex + p_flist[f + 1],
        p_obj->p_vertex + p_flist[f + 2]);
      vzd[2] =
        vzdal_bodu(p_obj->p_vertex + p_flist[f + 2],
        p_obj->p_vertex + p_flist[f]);

      if (vzd[0] > vzd[1] && vzd[0] > vzd[2]) {
        l1 = 1;
        l2 = 2;
        l3 = 2;
        l4 = 0;
      }
      if (vzd[1] > vzd[0] && vzd[1] > vzd[2]) {
        l1 = 0;
        l2 = 1;
        l3 = 2;
        l4 = 0;
      }
      if (vzd[2] > vzd[0] && vzd[2] > vzd[1]) {
        l1 = 0;
        l2 = 1;
        l3 = 1;
        l4 = 2;
      }

      mujbod2bod(&p_linky[linek].p1, p_obj->p_vertex + body[l1]);
      mujbod2bod(&p_linky[linek].p2, p_obj->p_vertex + body[l2]);
      p_linky[linek].i1 = body[l1];
      p_linky[linek].i2 = body[l2];
      linek++;

      mujbod2bod(&p_linky[linek].p1, p_obj->p_vertex + body[l3]);
      mujbod2bod(&p_linky[linek].p2, p_obj->p_vertex + body[l4]);
      p_linky[linek].i1 = body[l3];
      p_linky[linek].i2 = body[l4];
      linek++;
    }
    if (v == 2) {
      mujbod2bod(&p_linky[linek].p1, p_obj->p_vertex + body[0]);
      mujbod2bod(&p_linky[linek].p2, p_obj->p_vertex + body[1]);
      p_linky[linek].i1 = body[0];
      p_linky[linek].i2 = body[1];
      linek++;
    }
  }

  // eliminace stejnych linek
  for (i = 0; i < linek; i++) {
    if (p_linky[i].i1 == -1)
      continue;
    for (j = i + 1; j < linek; j++) {
      if ((p_linky[i].i1 == p_linky[j].i1 && p_linky[i].i2 == p_linky[j].i2)
        || (p_linky[i].i1 == p_linky[j].i2 && p_linky[i].i2 == p_linky[j].i1))
        p_linky[j].i2 = p_linky[j].i1 = -1;
    }
  }

  j = 0;
  for (i = 0; i < linek; i++) {
    if (p_linky[i].i1 == -1)
      continue;
    if (i != j) {
      p_linky[j] = p_linky[i];
    }
    j++;
  }
  linek = j;

  // Konstrukce objektu - v lokalnim space objektu
  p_flare = vyrob_objekt_komplet(linek * 4, linek * 2 * 3);

  for (i = 0; i < linek; i++) {
    p_flare->p_face[i * 6] = i * 4;
    p_flare->p_face[i * 6 + 1] = i * 4 + 1;
    p_flare->p_face[i * 6 + 2] = i * 4 + 3;
    p_flare->p_face[i * 6 + 3] = i * 4 + 1;
    p_flare->p_face[i * 6 + 4] = i * 4 + 2;
    p_flare->p_face[i * 6 + 5] = i * 4 + 3;

    p_b1 = (BOD *) (p_flare->p_vertex + i * 4);
    p_b2 = (BOD *) (p_flare->p_vertex + i * 4 + 1);
    p_b3 = (BOD *) (p_flare->p_vertex + i * 4 + 2);
    p_b4 = (BOD *) (p_flare->p_vertex + i * 4 + 3);

    // p - bod - skopirovat z objektu
    transformuj_bod_matici_bod(&p_linky[i].p1, p_word, p_b1);
    transformuj_bod_matici_bod(&p_linky[i].p2, p_word, p_b2);

    // a je p->b1
    vektor_sub(p_b1, &p_gen->p, &a);
    t = vektor_norm(&a);        // vzdal od svetla
    vektor_mult_skalar(&a, p_gen->konec, p_b4);
    vektor_add(&p_gen->p, p_b4, p_b4);

    p_vr = (MUJ_BOD *) p_b1;
    p_vr->tu1 = (t - p_gen->start) / delka;
    p_vr->dr = p_vr->dg = p_vr->db = p_vr->da = 1.0f;
    p_vr->mdr = p_vr->mdg = p_vr->mdb = p_vr->mda = 1.0f;

    p_vr = (MUJ_BOD *) p_b4;
    p_vr->tu1 = 1.0f;
    p_vr->dr = p_vr->dg = p_vr->db = p_vr->da = 1.0f;
    p_vr->mdr = p_vr->mdg = p_vr->mdb = p_vr->mda = 1.0f;

    // a je p->b2
    vektor_sub(p_b2, &p_gen->p, &a);
    t = vektor_norm(&a);        // vzdal od svetla
    vektor_mult_skalar(&a, p_gen->konec, p_b3);
    vektor_add(&p_gen->p, p_b3, p_b3);

    p_vr = (MUJ_BOD *) p_b2;
    p_vr->tu1 = (t - p_gen->start) / delka;
    p_vr->dr = p_vr->dg = p_vr->db = p_vr->da = 1.0f;
    p_vr->mdr = p_vr->mdg = p_vr->mdb = p_vr->mda = 1.0f;

    p_vr = (MUJ_BOD *) p_b3;
    p_vr->tu1 = 1.0f;
    p_vr->dr = p_vr->dg = p_vr->db = p_vr->da = 1.0f;
    p_vr->mdr = p_vr->mdg = p_vr->mdb = p_vr->mda = 1.0f;

    strcpy(p_flare->jmeno, "gen_lens_flare");
  }
  return (p_flare);
}

//bitmapa * oe_vyrob_light_texturu(int delka, dword start, dword konec)
int oe_vyrob_material_flare(K_EDITOR * p_cnf, FLARE_GEN * p_gen)
{
  EDIT_TEXT_KONFIG konf;
  EDIT_MATERIAL *p_mat;
  EDIT_TEXT *p_text;
  int text = lo_najdi_volnou_texturu(p_cnf->p_text, MAX_EDIT_TEXTUR);
  int mat;

  p_text = p_cnf->p_text + text;
  p_text->p_bmp =
    oe_vyrob_light_texturu(128, p_gen->start_color, p_gen->konec_color);
  sprintf(p_text->jmeno, "flare_text_%d", text);
  p_text->flag2 |= TEXT2_FLARE;

  txt_default_konfig("", &konf, ".bmp");
  konf.typ = GL_TEXTURE_1D;
  konf.format = txconf.format1d[FORMAT_RGB];
  konf.wrap_x = 0;
  konf.wrap_y = 0;
  txt_nahraj_texturu_do_vram(p_text, &konf);

  mat = lo_najdi_prazdny_material(p_cnf->p_mat, MAX_EDIT_MATERIALU);
  if (mat == K_CHYBA) {
    kprintf(TRUE, "Neni volny material");
    return (K_CHYBA);
  }

  p_mat = p_cnf->p_mat[mat] = vyrob_material();
  sprintf(p_mat->jmeno, "flare_mat_%d", mat);

  p_mat->alfa_state = STATE_ADD;
  p_mat->flag |= (MAT_NO_ZMASK | MAT_NO_CULL | MAT_PRUHLEDNY);
  p_mat->textflag[0] = 0;
  p_mat->p_text[0] = p_text;
  strcpy(p_mat->textfile[0], p_text->jmeno);

  p_mat->flag2 &=
    ~(MAT2_DIFFUSE | MAT2_SPECULAR | MAT2_DIFFUSE_RAY | MAT2_SPECULAR_RAY);
  return (mat);
}

int oe_generuj_flare_kontroluj_group(K_EDITOR * p_cnf)
{
  int i, k, o, nk, no;

  k = p_cnf->p_group[0].k;
  o = p_cnf->p_group[0].o;

  for (i = 1; i < p_cnf->groupnum; i++) {
    nk = p_cnf->p_group[i].k;
    no = p_cnf->p_group[i].o;
    if (o != no || k != nk)
      return (FALSE);
  }
  return (TRUE);
}

void oe_prikaz_generuj_flare(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_kont, *p_new;
  EDIT_OBJEKT *p_obj;
  FLARE_GEN gen;

  if (!p_cnf->groupnum) {
    kprintf(TRUE, "Nejsou vybrane hranicni vetexy!");
    return;
  }

  if (!oe_generuj_flare_kontroluj_group(p_cnf)) {
    kprintf(TRUE, "Vybrane vertexy musi byt jednoho objektu!!!");
    return;
  }

  memset(&gen, 0, sizeof(gen));
  gen.p_slight = p_cnf->light + p_cnf->lightakt;
  if (!gen.p_slight->akt)
    gen.p_slight = p_cnf->light;

  gen.konec = 10;
  gen.start_color = 0xffffffff;
  p_kont = p_cnf->p_kont[p_cnf->p_group[0].k];

  if (doe_cti_flare_gen(hwnd_aplikace, &gen)) {
    p_obj = oe_vyrob_objekt_flare(p_cnf, &gen);
    p_new = oe_najdi_volny_kontejner(p_cnf);
    p_new->kflag |=
      (KONT_STATIC | KONT_VERTEX_LIGHT | KONT_NO_SELF_SHADOW |
      KONT_NO_DRAW_SHADOW);
    p_new->p_obj[0] = p_obj;
    p_obj->material = oe_vyrob_material_flare(p_cnf, &gen);
  }
}


/* Optimalizace meshu
*/
#define VERTEX_DIST 0.0001f

float calc_equad_vert(GLMATRIX * p_mat, BOD * p_v)
{
  return (p_mat->_11 * p_v->x * p_v->x +
    2 * p_mat->_21 * p_v->x * p_v->y +
    2 * p_mat->_31 * p_v->x * p_v->z +
    2 * p_mat->_41 * p_v->x +
    p_mat->_22 * p_v->y * p_v->y +
    2 * p_mat->_32 * p_v->y * p_v->z +
    2 * p_mat->_42 * p_v->y +
    p_mat->_33 * p_v->z * p_v->z + 2 * p_mat->_43 * p_v->z + p_mat->_44);
}

void calc_equad_obj_mat(HRAN_STACK * p_stack, EDIT_OBJEKT * p_obj,
  GLMATRIX * p_mat)
{
  GLMATRIX t;
  ROVINA r;
  int f, f0, f1, f2, i;
  float vel;
  int vaha = p_stack->vaha;
  float vaha_flt = p_stack->vaha_flt;
  int prah = p_stack->prah;
  float prah_flt = p_stack->prah_flt;

  for (i = 0; i < p_obj->vertexnum; i++) {
    zero_matrix(p_mat + i);
  }

  for (f = 0; f < p_obj->facenum; f += 3) {
    f0 = p_obj->p_face[f];
    f1 = p_obj->p_face[f + 1];
    f2 = p_obj->p_face[f + 2];

    calc_rovinu(p_obj->p_vertex + f0, p_obj->p_vertex + f1,
      p_obj->p_vertex + f2, &r);
    vel = norm_rovinu(&r);      // - vel = velikost plochy plosky
    if (prah && vel <= prah_flt) {
      zero_matrix(&t);
    }
    else {
      rovina_to_matrix(&r, &t, vaha ? vaha_flt * vel : 1.0f);
    }

    mat_add(p_mat + f0, &t, p_mat + f0);
    mat_add(p_mat + f1, &t, p_mat + f1);
    mat_add(p_mat + f2, &t, p_mat + f2);
  }
}

void calc_equad_hrana(OBJ_VERTEX * p_vert, GLMATRIX * p_mat, HRAN * p_hran,
  int *p_vmask)
{
  GLMATRIX *p_q = &p_hran->q;
  OBJ_VERTEX vn;
  float vd0, vd1, vd2;
  int v1 = p_hran->ind1, v2 = p_hran->ind2;
  OBJ_VERTEX *p_new = &p_hran->bod;

  mat_add(p_mat + v1, p_mat + v2, p_q);
  vektor_bod_muj(p_vert + v1, p_vert + v2, &vn, 0.5f);

  if (p_vmask) {
    vd0 = (p_vmask[v2]) ? calc_equad_vert(p_q, (BOD *) (p_vert + v1)) : -1;
    vd1 = (p_vmask[v1]) ? calc_equad_vert(p_q, (BOD *) (p_vert + v2)) : -1;
    vd2 = (p_vmask[v1]
      && p_vmask[v2]) ? calc_equad_vert(p_q, (BOD *) (&vn)) : -1;
    if (vd0 != -1 && (vd0 <= vd1 || vd1 == -1) && (vd0 <= vd2 || vd2 == -1)) {
      *p_new = p_vert[v1];
      p_hran->err_uhel = p_hran->err = vd0;
    }
    else if (vd1 != -1 && (vd1 <= vd0 || vd0 == -1) && (vd1 <= vd2
        || vd2 == -1)) {
      *p_new = p_vert[v2];
      p_hran->err_uhel = p_hran->err = vd1;
    }
    else if (vd2 != -1 && (vd2 < vd0 || vd0 == -1) && (vd2 < vd1
        || vd1 == -1)) {
      *p_new = vn;
      p_hran->err_uhel = p_hran->err = vd2;
    }
    else {
      p_hran->err_uhel = p_hran->err = FLT_MAX;
    }
  }
  else {
    vd0 = calc_equad_vert(p_q, (BOD *) (p_vert + v1));
    vd1 = calc_equad_vert(p_q, (BOD *) (p_vert + v2));
    vd2 = calc_equad_vert(p_q, (BOD *) (&vn));

    if (vd0 <= vd1 && vd0 <= vd2) {
      *p_new = p_vert[v1];
      p_hran->err_uhel = p_hran->err = vd0;
    }
    else if (vd1 < vd0 && vd1 < vd2) {
      *p_new = p_vert[v2];
      p_hran->err_uhel = p_hran->err = vd1;
    }
    else if (vd2 < vd0 && vd2 < vd1) {
      *p_new = vn;
      p_hran->err_uhel = p_hran->err = vd2;
    }
    else {
      p_hran->err_uhel = p_hran->err = FLT_MAX;
    }
  }
}

/*
*/
int oe_hran_vloz_hrany_stack(HRAN_STACK * p_stack, EDIT_OBJEKT * p_obj)
{
  BOD norm;
  int f, f0, f1, f2;
  int i;
  int maska = p_stack->flag & VERT_MASK;

  // Pridam hrany do seznamu
  for (f = 0; f < p_obj->facenum; f += 3) {
    f0 = p_obj->p_face[f];
    f1 = p_obj->p_face[f + 1];
    f2 = p_obj->p_face[f + 2];
    calc_face_normal((BOD *) (p_obj->p_vertex + f0),
      (BOD *) (p_obj->p_vertex + f1), (BOD *) (p_obj->p_vertex + f2), &norm);
    if (maska) {
      if (p_stack->p_vmask[f0] || p_stack->p_vmask[f1])
        oe_hran_vloz(p_stack, f0, f1, &norm);
      if (p_stack->p_vmask[f0] || p_stack->p_vmask[f2])
        oe_hran_vloz(p_stack, f0, f2, &norm);
      if (p_stack->p_vmask[f1] || p_stack->p_vmask[f2])
        oe_hran_vloz(p_stack, f1, f2, &norm);
    }
    else {
      oe_hran_vloz(p_stack, f0, f1, &norm);
      oe_hran_vloz(p_stack, f0, f2, &norm);
      oe_hran_vloz(p_stack, f1, f2, &norm);
    }
  }

  // Penalizace koncovych hran
  if (p_stack->hrany) {
    for (i = 0; i < p_stack->listakt; i++) {
      if (p_stack->p_list[i].flag & HFLAG_HRANA) {
        oe_hran_pridej_hranici(p_stack, p_stack->p_list + i, p_obj);
        p_stack->p_list[i].flag &= ~HFLAG_HRANA;
      }
    }
  }

  return (p_stack->listakt);
}

/*
  v1 - novy-stavajici
  v2 - rusenej  
  Vsechno co sousedi s v1 a v2 potrebuje prepocitat
*/
void oe_hran_zrus_hranu_stack(HRAN_STACK * p_stack, HRAN * p_hrana, int v1,
  int v2)
{
  p_stack->listakt = 0;
}

int oe_hran_zrus_hranu(HRAN_STACK * p_stack, HRAN * p_hrana,
  EDIT_OBJEKT * p_obj, int *p_vmask)
{
  GLMATRIX *p_mat = p_stack->p_mat;
  int v1, v2;
  int f, i, j;
  int f0, f1, f2, vr;
  HRAN hr;

  if (p_stack->max_err && p_hrana->err_uhel > p_stack->max_err_flt)
    return (-1);

  if (p_hrana->ind1 > p_hrana->ind2) {
    v1 = p_hrana->ind2;
    v2 = p_hrana->ind1;
  }
  else {
    v1 = p_hrana->ind1;
    v2 = p_hrana->ind2;
  }

  hr = *p_hrana;

  // 0. vymaz hrany ze zasobniku
  oe_hran_zrus_hranu_stack(p_stack, p_hrana, v1, v2);

  // 1. vymaz matice quat
  // 2. vymaz indicii
  // 3. vymaz bodu  
  p_obj->p_vertex[v1] = hr.bod;

  if (v2 + 1 < p_obj->vertexnum) {
    memmove(p_obj->p_vertex + v2, p_obj->p_vertex + v2 + 1,
      sizeof(p_obj->p_vertex[0]) * (p_obj->vertexnum - v2 - 1));
  }

  // posun masky
  if (p_vmask) {
    vr = p_vmask[v1] && p_vmask[v2];
    if (v2 + 1 < p_obj->vertexnum)
      memmove(p_vmask + v2, p_vmask + v2 + 1,
        sizeof(p_vmask[0]) * (p_obj->vertexnum - v2 - 1));
    p_vmask[v1] = vr;
  }

  p_obj->vertexnum--;

  // 3. vymaz face
  for (f = 0, j = 0; f < p_obj->facenum; f += 3) {
    f0 = p_obj->p_face[f];
    f1 = p_obj->p_face[f + 1];
    f2 = p_obj->p_face[f + 2];
    i = 0;
    if (f0 == v1 || f0 == v2)
      i++;
    if (f1 == v1 || f1 == v2)
      i++;
    if (f2 == v1 || f2 == v2)
      i++;
    if (i < 2) {                // face i -> j
      p_obj->p_face[j] = (f0 == v2) ? v1 : (f0 > v2) ? f0 - 1 : f0;
      p_obj->p_face[j + 1] = (f1 == v2) ? v1 : (f1 > v2) ? f1 - 1 : f1;
      p_obj->p_face[j + 2] = (f2 == v2) ? v1 : (f2 > v2) ? f2 - 1 : f2;
      j += 3;
    }
  }
  p_obj->facenum = j;

  // 5. prepocitam quady ve vertexech ktere se zmenily
  calc_equad_obj_mat(p_stack, p_obj, p_stack->p_mat);

  // 6. pridam znova vsechny hrany
  if (!oe_hran_vloz_hrany_stack(p_stack, p_obj))
    return (K_CHYBA);

  return (v1);
}

void oe_hran_reset(HRAN_STACK * p_stack)
{
  if (p_stack->p_list) {
    free(p_stack->p_list);
    p_stack->p_list = NULL;
  }

  if (p_stack->p_mat) {
    free(p_stack->p_mat);
    p_stack->p_mat = NULL;
  }

  p_stack->listnum = 0;
  p_stack->listakt = 0;
}

int oe_hran_vloz(HRAN_STACK * p_stack, int ind1, int ind2, BOD * p_norm)
{
  HRAN *p_hran;
  int i, i1, i2;

  for (i = 0; i < p_stack->listakt; i++) {
    i1 = p_stack->p_list[i].ind1;
    i2 = p_stack->p_list[i].ind2;
    if ((i1 == ind1 || i1 == ind2) && (i2 == ind1 || i2 == ind2)) {
      p_stack->p_list[i].flag &= ~HFLAG_HRANA;
      return (FALSE);
    }
  }

  if (p_stack->listakt + 1 >= p_stack->listnum) {
    p_stack->listnum += HRN_SEGMENT;
    p_stack->p_list =
      p_stack->p_list ? realloc(p_stack->p_list,
      sizeof(p_stack->p_list[0]) *
      p_stack->listnum) : mmalloc(sizeof(p_stack->p_list[0]) *
      p_stack->listnum);
    mtest(p_stack->p_list);
  }

  p_hran = p_stack->p_list + p_stack->listakt++;
  p_hran->ind1 = ind1;
  p_hran->ind2 = ind2;
  p_hran->face_normal = *p_norm;
  p_hran->flag |= HFLAG_HRANA;
  return (TRUE);
}

int oe_hran_setrid_sort(const void *p_h1, const void *p_h2)
{
  float err1 = (float) fabs(((HRAN *) p_h1)->err), err2 =
    (float) fabs(((HRAN *) p_h2)->err);
  return ((err1 > err2) ? -1 : (err1 == err2) ? 0 : 1);
}

void oe_hran_vyhodnot(HRAN_STACK * p_stack, EDIT_OBJEKT * p_obj, int *p_vmask)
{
  int i;

  // Ohodnot vertexy
  for (i = 0; i < p_stack->listakt; i++) {
    calc_equad_hrana(p_obj->p_vertex, p_stack->p_mat, p_stack->p_list + i,
      p_vmask);
  }

  // Setrid podle chyb
  qsort(p_stack->p_list, p_stack->listakt, sizeof(p_stack->p_list[0]),
    oe_hran_setrid_sort);
}

void oe_hran_decimuj(HRAN_STACK * p_stack, GLMATRIX * p_mat,
  EDIT_OBJEKT * p_obj, int vertexu)
{
  HRAN *p_hrana;
  int i, j;

  for (i = 0; i < vertexu; i++) {

    oe_hran_vyhodnot(p_stack, p_obj, p_stack->p_vmask);

    p_stack->listakt--;
    p_hrana = p_stack->p_list + p_stack->listakt;

    if (p_hrana->err == FLT_MAX)
      break;

    j = oe_hran_zrus_hranu(p_stack, p_hrana, p_obj, p_stack->p_vmask);
    if (j == K_CHYBA)
      break;

    if (!(i % 10)) {
      doe_printf(p_stack->hDlg, IDC_EDIT12, "%d", i);
      doe_printf(p_stack->hDlg, IDC_EDIT13, "%d", vertexu);
      doe_progresbar_posun(p_stack->hDlg, IDC_PROGRESS1, 10);
    }
    spracuj_spravy(1);
  }
}

void oe_hran_pridej_hranici(HRAN_STACK * p_stack, HRAN * p_hran,
  EDIT_OBJEKT * p_obj)
{
  GLMATRIX *p_mat = p_stack->p_mat;
  int i1, i2;
  BOD v0, v1, v3;
  ROVINA r;
  GLMATRIX m;

  i1 = p_hran->ind1;
  i2 = p_hran->ind2;

  mujbod2bod(&v0, p_obj->p_vertex + i1);
  mujbod2bod(&v1, p_obj->p_vertex + i2);
  vektor_add(&p_hran->face_normal, &v0, &v3);
  calc_rovinu_bod(&v0, &v3, &v1, &r);
  norm_rovinu(&r);

  rovina_to_matrix(&r, &m, p_stack->hrany_flt);
  mat_add(&m, p_mat + i1, p_mat + i1);
  mat_add(&m, p_mat + i2, p_mat + i2);
}

EDIT_OBJEKT *oe_obj_zjednodus(EDIT_OBJEKT * p_obj, int vertexu)
{
  EDIT_OBJEKT *p_new = kopiruj_objekt(p_obj);
  HRAN_STACK *p_stack = &p_cnf->ds;

  oe_hran_reset(p_stack);

  p_stack->p_mat = mmalloc(sizeof(p_stack->p_mat[0]) * p_new->vertexnum);
  calc_equad_obj_mat(p_stack, p_new, p_stack->p_mat);
  if (!oe_hran_vloz_hrany_stack(p_stack, p_new))
    return (p_new);

  oe_hran_decimuj(p_stack, p_stack->p_mat, p_new, vertexu);

  oe_hran_reset(p_stack);

  return (p_new);
}

BOOL CALLBACK doe_callback_decimator_plosek(HWND hDlg, UINT msg,
  WPARAM wParam, LPARAM lParam)
{
  switch (msg) {
    case WM_COMMAND:
      switch (LOWORD(wParam)) {
        case IDOK:
          EndDialog(hDlg, 1);
          break;
        case IDCANCEL:
          EndDialog(hDlg, 0);
          break;
        default:
          return FALSE;
      }
      break;
    case WM_INITDIALOG:
      break;
    case WM_DESTROY:
      EndDialog(hDlg, IDCANCEL);
      break;
    default:
      return FALSE;
  }
  return TRUE;
}

void oe_stack_zaloha_zrus(HRAN_STACK * p_stack)
{
  int i;

  for (i = 0; i < p_stack->zalakt; i++) {
    if (p_stack->p_zal[i].p_obj)
      free(p_stack->p_zal[i].p_obj);
  }
  if (p_stack->p_zal) {
    free(p_stack->p_zal);
    p_stack->p_zal = NULL;
  }
  p_stack->zalnum = 0;
  p_stack->zalakt = 0;
}

void oe_stack_zaloha_vyrob(HRAN_STACK * p_stack, int num)
{
  p_stack->p_zal = mmalloc(sizeof(p_stack->p_zal[0]) * num);
  p_stack->zalnum = num;
  p_stack->zalakt = 0;
}

void oe_stack_zaloha_pridej(HRAN_STACK * p_stack, EDIT_OBJEKT * p_obj, int o,
  int k)
{
  if (p_stack->zalakt < p_stack->zalnum) {
    p_stack->p_zal[p_stack->zalakt].p_obj = kopiruj_objekt(p_obj);
    p_stack->p_zal[p_stack->zalakt].k = k;
    p_stack->p_zal[p_stack->zalakt].o = o;
    p_stack->zalakt++;
  }
}

void oe_stack_zaloha_obnov(K_EDITOR * p_cnf)
{
  HRAN_STACK *p_stack = &p_cnf->ds;
  int k, o, i;

  for (i = 0; i < p_stack->zalakt; i++) {
    k = p_stack->p_zal[i].k;
    o = p_stack->p_zal[i].o;
    if (p_cnf->p_kont[k] && p_cnf->p_kont[k]->p_obj[o]) {
      zrus_objekt(&p_cnf->p_kont[k]->p_obj[o]);
      p_cnf->p_kont[k]->p_obj[o] = p_stack->p_zal[i].p_obj;
      p_stack->p_zal[i].p_obj = NULL;
    }
  }

  oe_stack_zaloha_zrus(p_stack);
}

void oe_stack_vertexy_zrus(HRAN_STACK * p_stack)
{
  if (p_stack->p_vmask) {
    free(p_stack->p_vmask);
    p_stack->p_vmask = NULL;
  }
}

int oe_stack_vertexy_pridej(K_EDITOR * p_cnf, EDIT_OBJEKT * p_obj,
  HRAN_STACK * p_stack, int k, int o, int flag)
{
  int v, vnum;

  oe_stack_vertexy_zrus(p_stack);

  p_stack->p_vmask = mmalloc(sizeof(p_stack->p_vmask[0]) * p_obj->vertexnum);

  if (flag & VERT_SMAZ) {
    for (vnum = 0, v = 0; v < p_cnf->groupnum; v++) {
      if (p_cnf->p_group[v].k == k && p_cnf->p_group[v].o == o) {
        p_stack->p_vmask[p_cnf->p_group[v].v] = 1;
        vnum++;
      }
    }
  }

  if (flag & VERT_NECHEJ) {
    memset(p_stack->p_vmask, 0xffffffff,
      sizeof(p_stack->p_vmask[0]) * p_obj->vertexnum);
    for (vnum = 0, v = 0; v < p_cnf->groupnum; v++) {
      if (p_cnf->p_group[v].k == k && p_cnf->p_group[v].o == o) {
        p_stack->p_vmask[p_cnf->p_group[v].v] = 0;
      }
    }
    vnum = p_obj->vertexnum - vnum;
  }
  return (vnum);
}

void oe_prikaz_zjednodus_objekty(K_EDITOR * p_cnf, int flag)
{
  HRAN_STACK *p_stack = &p_cnf->ds;
  EDIT_OBJEKT *p_obj;
  int handle = oe_vybr_obj_reset();
  int o, k, vertexu, obj = 0;
  int objnum;

  oe_uloz_undo(p_cnf);

  p_cnf->ds.hDlg =
    CreateDialog(hinst, MAKEINTRESOURCE(IDD_DECIMATOR), hwnd_plocha,
    doe_callback_decimator_plosek);

  objnum = oe_vybr_obj_get_num();

  doe_progresbar_nastav(p_stack->hDlg, IDC_PROGRESS4, 0, objnum);
  doe_printf(p_stack->hDlg, IDC_EDIT11, "%d", objnum);

  oe_stack_zaloha_zrus(p_stack);
  oe_stack_zaloha_vyrob(p_stack, objnum);

  while ((p_obj = oe_vybr_obj_get_ind(&handle, &k, &o))) {
    p_stack->flag = flag;
    if (flag & (VERT_NECHEJ | VERT_SMAZ)) {
      vertexu = oe_stack_vertexy_pridej(p_cnf, p_obj, p_stack, k, o, flag);
      vertexu = vertexu - ftoi((float) (vertexu) * p_stack->stupen);
    }
    else {
      vertexu =
        p_cnf->ds.vertexy ? p_stack->vertexu : (p_obj->vertexnum -
        ftoi((float) (p_obj->vertexnum) * p_stack->stupen));
    }
    doe_progresbar_nastav(p_stack->hDlg, IDC_PROGRESS1, 0, vertexu);
    doe_printf(p_stack->hDlg, IDC_EDIT9, "%d", obj++);
    spracuj_spravy(1);
    oe_stack_zaloha_pridej(p_stack, p_obj, o, k);
    p_cnf->p_kont[k]->p_obj[o] = oe_obj_zjednodus(p_obj, vertexu);
    zrus_objekt(&p_obj);
    doe_progresbar_posun(p_stack->hDlg, IDC_PROGRESS4, 1);
  }

  oe_stack_vertexy_zrus(p_stack);
  oe_prikaz_all_ungroup_vertex(p_cnf);
  EndDialog(p_cnf->ds.hDlg, TRUE);
  p_cnf->ds.hDlg = NULL;
}

#define sjednoc_stejny_vertex(p1,p2) (p1.x == p2.x && p1.y == p2.y && p1.z == p2.z && p1.tu1 == p2.tu1 && p1.tv1 == p2.tv1)

// Sliva se podle pozice/tu/tv textury
int oe_sjednoceni_vertexu_obj(EDIT_OBJEKT * p_obj)
{
  int v, vv, f;
  int f0, vnum;

  if (p_obj->p_joit_animace)
    return (0);

  vnum = 0;
  for (v = 0; v < p_obj->vertexnum; v++) {
    for (vv = v + 1; vv < p_obj->vertexnum; vv++) {
      if (p_obj->p_vertex[vv].x != FLT_MAX
        && sjednoc_stejny_vertex(p_obj->p_vertex[v], p_obj->p_vertex[vv])) {
        p_obj->p_vertex[vv].x = FLT_MAX;
        p_obj->p_vertex[vv].y = (float) v;
        vnum++;
      }
    }
  }

  for (f = 0; f < p_obj->facenum; f++) {
    f0 = p_obj->p_face[f];
    if (p_obj->p_vertex[f0].x == FLT_MAX) {
      p_obj->p_face[f] = (FACE) (p_obj->p_vertex[f0].y);
    }
  }

  smaz_divoke_vertexy_objektu_dir(p_obj);
  return (vnum);
}

void oe_prikaz_sjednoceni_vertexu_obj(K_EDITOR * p_cnf)
{
  EDIT_OBJEKT *p_obj;
  int handle = oe_vybr_obj_reset();
  int num;

  while ((p_obj = oe_vybr_obj_get(&handle))) {
    kprintf(TRUE, "Sjednoceni vertexu %s...", p_obj->jmeno);
    num = oe_sjednoceni_vertexu_obj(p_obj);
    kprintf(TRUE, "vert %d", num);
  }
}

void oe_generator_animaci_frame(JOINT * p_joint, JOINT_ANIM_GENERATOR * p_gen,
  int level, int frame)
{
  KEY_POINT_BRS key = { 0, 0, 0, 0, 0, 0 };
  JOINT_KEYS *p_keys;
  QUAT q, udq, vtq;
  float scale = p_gen->scale_0 + p_gen->scale_1 * level;
  float uhel;
  float ud_uhel;
  float udr_uhel;
  float vt_uhel;
  float vtr_uhel;


  /* UD-rada
   */
  ud_uhel = randf() * DEG2RAD(p_gen->ud_rozsah) * scale * p_gen->znaminko;
  udr_uhel = p_gen->p_udr_all[frame];
  uhel = ud_uhel * (1.0f - p_gen->udr_sila) + udr_uhel * p_gen->udr_sila;
  angle_to_quat(&udq, &p_gen->osa, uhel);

  /* VT-rada
   */
  vt_uhel = randf() * DEG2RAD(p_gen->vt_rozsah) * scale * p_gen->znaminko;
  vtr_uhel = p_gen->p_vtr_all[frame];
  uhel = vt_uhel * (1.0f - p_gen->vtr_sila) + vtr_uhel * p_gen->vtr_sila;
  angle_to_quat(&vtq, &p_gen->osa_vitr, uhel);

  slerp(&udq, &vtq, p_gen->sila_vetru, &q);

  p_keys = key_kost_spline_vyrob(p_joint);
  key.time = frame2time(frame);
  p_keys->rot_keys = key_track_klic_vloz((byte **) & p_keys->p_rot,
    &p_keys->p_rkeys, p_keys->rot_keys,
    sizeof(p_keys->p_rot[0]), (byte *) & q, &key);

  if (p_joint->p_child)
    oe_generator_animaci_frame(p_joint->p_child, p_gen, level + 1, frame);
}


BOD *oe_gen_anim_calc_vektor(JOINT_ANIMACE * p_anim, BOD * p_vec)
{
  JOINT *p_joint = p_anim->p_child;
  BOD v1, v2, v3;

  if (!p_joint || !p_joint->p_child)
    return (NULL);

  v1 = p_joint->pivot;
  v2 = p_joint->p_child->pivot;

  if (!p_joint->p_child->p_child) {
    vektor_set_all(&v3, 0, 1, 0);
  }
  else {
    while (p_joint->p_child)
      p_joint = p_joint->p_child;
    v3 = p_joint->pivot;
  }

  vektor_sub(&v2, &v1, &v2);
  vektor_sub(&v3, &v1, &v3);

  vektor_soucin(&v3, &v2, p_vec);
  vektor_norm(p_vec);
  return (p_vec);
}

void oe_generator_animaci_rady(JOINT_ANIM_GENERATOR * p_gen)
{
  KEY_POINT_BRS key = { 0, 0, 0, 0, 0, 0 };
  int udr_min_framu = p_gen->udr_frame_min;
  int vtr_min_framu = p_gen->vtr_frame_min;
  float udr_ros_framu = (float) (p_gen->udr_frame_max - p_gen->udr_frame_min);
  float vtr_ros_framu = (float) (p_gen->vtr_frame_max - p_gen->vtr_frame_min);
  BOD up = { 0, 1, 0 };
  float znaminko;
  float num;
  int i, f;
  int endtime;


  /* UDR-klice
   */

  p_gen->udr_keys = 0;
  null_free(&p_gen->p_ukeys);
  null_free(&p_gen->p_udr);
  null_free(&p_gen->p_udr_all);

  num = 0;
  key.time = 0;
  p_gen->udr_keys = key_track_klic_vloz((byte **) & p_gen->p_udr,
    &p_gen->p_ukeys, p_gen->udr_keys,
    sizeof(p_gen->p_udr[0]), (byte *) & num, &key);
  f = 0;
  do {
    f += udr_min_framu + ftoi(randf() * udr_ros_framu);
    if (f > p_gen->framu - udr_min_framu) {
      break;
    }
    znaminko = randf() > 0.5f ? 1.0f : -1.0f;
    key.time = frame2time(f);
    num = randf() * DEG2RAD(p_gen->udr_rozsah) * znaminko;
    p_gen->udr_keys = key_track_klic_vloz((byte **) & p_gen->p_udr,
      &p_gen->p_ukeys, p_gen->udr_keys,
      sizeof(p_gen->p_udr[0]), (byte *) & num, &key);
  } while (1);

  /* UDR-klice - sim-interpolace do rady
   */
  endtime = calc_endtime(p_gen->framu);
  p_gen->p_udr_all = mmalloc(sizeof(p_gen->p_udr_all[0]) * p_gen->framu);
  for (i = 0; i < p_gen->framu; i++)
    key_track_interpolace_float(p_gen->p_udr_all + i, p_gen->p_udr,
      p_gen->p_ukeys, i * SIM_KONSTI, endtime, p_gen->udr_keys, TRUE);


  /* VTR-klice
   */
  p_gen->vtr_keys = 0;
  null_free(&p_gen->p_vkeys);
  null_free(&p_gen->p_vtr);
  null_free(&p_gen->p_vtr_all);


  num = 0;
  key.time = 0;
  p_gen->vtr_keys = key_track_klic_vloz((byte **) & p_gen->p_vtr,
    &p_gen->p_vkeys, p_gen->vtr_keys,
    sizeof(p_gen->p_vtr[0]), (byte *) & num, &key);
  f = 0;
  do {
    f += vtr_min_framu + ftoi(randf() * vtr_ros_framu);
    if (f > p_gen->framu - vtr_min_framu) {
      break;
    }
    znaminko = randf() > 0.5f ? 1.0f : -1.0f;
    key.time = frame2time(f);
    num = randf() * DEG2RAD(p_gen->vtr_rozsah) * znaminko;
    p_gen->vtr_keys = key_track_klic_vloz((byte **) & p_gen->p_vtr,
      &p_gen->p_vkeys, p_gen->vtr_keys,
      sizeof(p_gen->p_vtr[0]), (byte *) & num, &key);
  } while (1);

  /* VTR-klice - sim-interpolace do rady
   */
  p_gen->p_vtr_all = mmalloc(sizeof(p_gen->p_vtr_all[0]) * p_gen->framu);
  for (i = 0; i < p_gen->framu; i++)
    key_track_interpolace_float(p_gen->p_vtr_all + i, p_gen->p_vtr,
      p_gen->p_vkeys, i * SIM_KONSTI, endtime, p_gen->vtr_keys, TRUE);

  /* Vypocet osy vetru
   */
  vektor_soucin(&p_gen->vt_smer, &up, &p_gen->osa_vitr);
  vektor_norm(&p_gen->osa_vitr);
}


void oe_generator_animaci(K_EDITOR * p_cnf)
{
  JOINT_ANIM_GENERATOR *p_gen = &p_cnf->jgen;
  JOINT_ANIMACE *p_anim = p_gen->p_anim;
  int ud_min_framu = p_gen->ud_frame_min;
  float ud_ros_framu = (float) (p_gen->ud_frame_max - p_gen->ud_frame_min);
  int f;

  p_gen->framu =
    p_gen->framu_min + ftoi((float) (p_gen->framu_max -
      p_gen->framu_min) * randf());

  oe_generator_animaci_rady(p_gen);

  while (p_anim) {
    oe_gen_anim_calc_vektor(p_anim, &p_gen->osa);
    p_anim->flag |= GK_LOOP;

    zrus_kosti_prs_rec(p_anim->p_child);
    p_anim->framenum = p_gen->framu;

    f = 0;
    p_gen->znaminko = 0.0f;
    oe_generator_animaci_frame(p_anim->p_child, p_gen, 0, f);

    do {
      f += ud_min_framu + ftoi(randf() * ud_ros_framu);
      if (f > p_gen->framu - ud_min_framu)
        break;
      p_gen->znaminko = randf() > 0.5f ? 1.0f : -1.0f;
      oe_generator_animaci_frame(p_anim->p_child, p_gen, 0, f);
    } while (1);

    key_kosti_interpoluj(p_anim);

    p_anim = p_anim->p_next_gen;
  }
}

#define KFLAG1_SET 0
#define KFLAG1_REM 1

#define KFLAG2_SET 2
#define KFLAG2_REM 3

#define MFLAG1_SET 4
#define MFLAG1_REM 5

void oe_prikaz_flagovac(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  int k;
  int flags[6];

  if (doe_kontejner_flagovac(flags)) {
    k = oe_vybr_kont_reset();
    if (k) {
      while ((p_kont = oe_vybr_kont_get(&k))) {
        p_kont->kflag |= flags[KFLAG1_SET];
        p_kont->kflag &= ~flags[KFLAG1_REM];
        p_kont->k2flag |= flags[KFLAG2_SET];
        p_kont->k2flag &= ~flags[KFLAG2_REM];
        p_kont->m1flag |= flags[MFLAG1_SET];
        p_kont->m1flag &= ~flags[MFLAG1_REM];
      }
    }
  }
}

void oe_prikaz_vycisti_scenu(K_EDITOR * p_cnf)
{
  int i;

  kprintf(TRUE, "Cistim scenu...");
  for (i = 0; i < p_cnf->max_kont; i++) {
    if (p_cnf->p_kont[i]) {
      updatuj_kontejner_statistika(p_cnf->p_kont[i], FALSE);
      if (!p_cnf->p_kont[i]->objektu) {
        zrus_kontejner_rec(p_cnf->p_kont + i, &p_cnf->p_mlha);
      }
    }
  }
}

void oe_prikaz_zrcadelnik(K_EDITOR * p_cnf)
{
  EDIT_MATERIAL *p_mat;
  EDIT_OBJEKT *p_obj;
  int oh;

  oh = oe_vybr_obj_reset();
  if (oh) {
    while (p_obj = oe_vybr_obj_get(&oh)) {
      p_mat = p_cnf->p_mat[p_obj->material];
      if (p_mat) {
        p_mat->alfa_state = 3;
        p_mat->flag |= MAT_PRUHLEDNY;
      }
    }
  }
}

/*
255, 0, 255
0,   0, 0
54,  47, 45
237, 77, 0
238, 77, 0
*/
void oe_prikaz_prevadec_bmp(K_EDITOR * p_cnf)
{
  byte pom[200];
  struct _finddata_t data;
  long handle;

  if (doe_dialog_load(hwnd_aplikace, pom, 199, "Vyber prvni prevadenou bmp",
      "*.bmp", "Bitmapy")) {
    if ((handle = _findfirst("*.bmp", &data)) == -1)
      return;
    do {
      bitmapa *p_bmp = bmp_nahraj(data.name, FALSE);

      kprintf(TRUE, "Prevadim bitmapu %s...", data.name);
      bmp_to_16bit(p_bmp, 0x7, TRUE);
      bmp_uloz(data.name, p_bmp);
      bmp_zrus(&p_bmp);
    } while (!_findnext(handle, &data));
    _findclose(handle);
  }
}
