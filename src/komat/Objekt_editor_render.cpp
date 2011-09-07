/*
  Renderovaci rutiny 
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

#define RIF(min,max) (((max)+(min))*0.5f)
#define DIF(min,max) (((max)-(min))*0.5f)

void oe_kresli_kameru(KAMERA * p_cam, int pyr)
{
  kresli_caru(&p_cam->p, &p_cam->t, DDRGB(0, 0, 1));

  set_matrix_world(&p_cam->p_objekt->m);
  kresli_objekt_dratove_jednoduse(p_cam->p_target);
  kresli_objekt_dratove_jednoduse(p_cam->p_pos);

  if (pyr) {
    kresli_objekt_dratove_jednoduse(p_cam->p_objekt);
  }
}

// kresli editacni flary (kostky)
void oe_kresli_edit_flare(K_EDITOR * p_cnf, LENS_FLARE * p_flare, int num)
{
  BOD p = { 0, 0, 0 };
  int f;

  text_off(GL_TEXTURE_2D);
  text_off(GL_TEXTURE_1D);
  for (f = 0; f < num; f++) {
    if (p_flare[f].akt) {

      if (p_flare[f].p_svetlo)
        oe_cti_pozici_static_light(p_flare[f].p_svetlo, &p);
      else
        p = p_flare[f].p;

      if (f == p_cnf->flareakt) {
        oe_test_kriz_stred_bod(p_cnf, VELIKOST_STREDU, &p, POPIS_OS_ON,
          STRED_ON, p_flare[f].rflag);
      }
      oe_kresli_kosoctverec(p_cnf, &p_cnf->init_matrix, &p, 0.5f, 1, 0, 0);
    }
  }
}


#define SLOZ_FLARE  14

// Prevest to na integery !!!
void oe_kresli_game_flare(K_EDITOR * p_cnf, LENS_FLARE * p_flare, int num)
{
  STATIC_LIGHT *p_light;
  LENS_FLARE_SLOZ *p_sloz;
  int f, s;
  float pixel_z;
  float faktor, t;
  float xw, yw, zw, w, ryw, sx, sy;
  float xq, yq;
  float dx, dy;
  GLMATRIX m;
  BOD p;
  int x, y, maxx, maxy;
  int ox, oy;

// disable_fog(p_cnf->ml.mlha);

  get_matrix_view(&x, &y, &maxx, &maxy);
  ox = x + (maxx >> 1);
  oy = y + (maxy >> 1);
  faktor = (float) (maxx) / 10.0f;

  mat_mult(p_cnf->p_camera, p_cnf->p_project, &m);

  /* Analyticka cast - zjistim ktere jsou viditelne
   */
  for (f = 0; f < num; f++) {
    p_flare[f].vid = FALSE;
    if (!p_flare[f].akt)
      continue;

    if (p_flare[f].p_svetlo) {
      oe_cti_pozici_static_light(p_flare[f].p_svetlo, &p);
    }
    else {
      p = p_flare[f].p;
      p_light = NULL;
    }

    w = 1.0f;
    transformuj_wbod_matici(&p.x, &p.y, &p.z, &w, &m);

    p_flare[f].vid = (0 < p.z && p.z < w && -w < p.x && p.x <= w && -w < p.y
      && p.y <= w);
    p_flare[f].wz = p.z;        //wz musi byt vetsi nez 0 a mensi nez w

    if (p.z > 0.0f && (p_flare[f].vid || p_flare[f].p_sloz)) {

      xw = (maxx >> 1) * (p.x / w) + ox;
      yw = (maxy >> 1) * (p.y / w) + oy;
      zw = (0.5f) * (p.z / w) + 0.5f;

      p_flare[f].wx = xw;
      p_flare[f].wy = ryw = maxy - 1 - yw;

      if (p_flare[f].vid) {
        glReadPixels(ftoi(xw), ftoi(yw), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT,
          &pixel_z);
        p_flare[f].vid = (pixel_z >= zw);
      }

      if (p_flare[f].p_sloz) {
        xq = (float) ox - xw;
        yq = (float) oy - ryw;
        p_sloz = p_flare[f].p_sloz;
        for (s = 0; s < SLOZ_FLARE; s++) {
          t = p_sloz[s].vzdal;
          if (t > 0.0f) {
            t *= 1.3f;
            p_sloz[s].wx = sx = xw + xq * t;
            p_sloz[s].wy = sy = ryw + yq * t;
            p_sloz[s].vid = (0 <= sx && sx < maxx && 0 <= sy && sy < maxy);
          }
        }
      }
    }
  }

  /* Rendering flaru
   */
  set_matrix_2d(maxx, maxy);

  deph_test_set(FALSE);

  text_set_num(1);
  text_off(GL_TEXTURE_1D);
  text_off(GL_TEXTURE_2D);

  text_set_num(0);
  text_on(GL_TEXTURE_2D);
  blend_on();

  for (f = 0; f < num; f++) {
    if (p_flare[f].vid) {
      blend_set(GL_SRC_ALPHA, p_flare[f].alfa);

      // Nastavim material flaru
      text_set(p_flare[f].p_mat->p_text[0]->text,
        p_flare[f].p_mat->p_text[0]->typ);
      glColor4f(p_flare[f].r, p_flare[f].g, p_flare[f].b, p_flare[f].a);

      dx = p_flare[f].dx * faktor;
      dy = p_flare[f].dy * faktor;

      xw = p_flare[f].wx;
      yw = p_flare[f].wy;

      glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 0.0f);
      glVertex3f(-dx + xw, dy + yw, 0.0f);

      glTexCoord2f(1.0f, 0.0f);
      glVertex3f(dx + xw, dy + yw, 0.0f);

      glTexCoord2f(1.0f, 1.0f);
      glVertex3f(dx + xw, -dy + yw, 0.0f);

      glTexCoord2f(0.0f, 1.0f);
      glVertex3f(-dx + xw, -dy + yw, 0.0f);
      glEnd();
    }

    if (p_flare[f].vid && p_flare[f].p_sloz /* && p_flare[f].wz > 0.0f */ ) {
      p_sloz = p_flare[f].p_sloz;
      for (s = 0; s < SLOZ_FLARE; s++) {
        if (p_sloz[s].vid) {
          xw = p_sloz[s].wx;
          yw = p_sloz[s].wy;

          dx = p_sloz[s].dx * faktor;
          dy = p_sloz[s].dy * faktor;

          text_set(p_sloz[s].p_mat->p_text[0]->text,
            p_sloz[s].p_mat->p_text[0]->typ);
          glColor4f(p_sloz[s].r, p_sloz[s].g, p_sloz[s].b, p_sloz[s].a);

          glBegin(GL_QUADS);
          glTexCoord2f(0.0f, 0.0f);
          glVertex3f(-dx + xw, dy + yw, 0.0f);

          glTexCoord2f(1.0f, 0.0f);
          glVertex3f(dx + xw, dy + yw, 0.0f);

          glTexCoord2f(1.0f, 1.0f);
          glVertex3f(dx + xw, -dy + yw, 0.0f);

          glTexCoord2f(0.0f, 1.0f);
          glVertex3f(-dx + xw, -dy + yw, 0.0f);
          glEnd();
        }
      }
    }
  }

  deph_test_set(TRUE);

  ret_matrix_2d();
// enable_fog(p_cnf->ml.mlha);
}

void oe_kresli_kosoctverec(K_EDITOR * p_cnf, GLMATRIX * p_world, BOD * p_p,
  float r, float dr, float dg, float db)
{
  kresli_kosoctverec(p_cnf->p_project, p_cnf->p_camera, p_world, p_p, r, dr,
    dg, db);
}


/*
  Kresli svetlo jako jehlan
*/
void oe_kresli_staticke_svetlo(K_EDITOR * p_cnf, STATIC_LIGHT * p_light,
  int akt)
{
  BOD s;

  if (p_light->akt) {
    oe_cti_pozici_static_light(p_light, &s);
    if (p_light->index == akt) {
      oe_test_kriz_stred_bod(p_cnf, VELIKOST_STREDU, &s, POPIS_OS_ON,
        STRED_ON, p_light->flag);
    }
    oe_kresli_kosoctverec(p_cnf, &p_cnf->init_matrix, &s, 0.5f, 1, 1, 0);
  }
}

/*
  Kresli dynamicke svetlo
*/
void oe_kresli_dynamicke_svetlo(K_EDITOR * p_cnf, DYN_LIGHT * p_light,
  int akt)
{
  BOD s;

  if (p_light->akt) {
    if (p_light->flag & SDL_MESH) {
      transformuj_bod_matici_bod(&p_light->np,
        kont_world_matrix(p_light->p_mesh_data), &s);
    }
    else {
      s = p_light->np;
    }
    if (akt) {
      oe_test_kriz_stred_bod(p_cnf, VELIKOST_STREDU, &s, POPIS_OS_ON,
        STRED_ON, p_light->pflag);
    }
    oe_kresli_kosoctverec(p_cnf, &p_cnf->init_matrix, &s, 0.5f, 1, 1, 0);
  }
}

/*
  Kresli extra svetla
*/
void oe_kresli_extra_dynamicke_svetlo(K_EDITOR * p_cnf,
  EXTRA_DYN_LIGHT * p_light, int akt)
{
  BOD s;

  if (p_light->akt) {
    if (p_light->flag & EDL_MESH) {
      transformuj_bod_matici_bod(&p_light->np,
        kont_world_matrix(p_light->p_mesh_data), &s);
    }
    else {
      s = p_light->np;
    }
    if (akt) {
      oe_test_kriz_stred_bod(p_cnf, VELIKOST_STREDU, &s, POPIS_OS_ON,
        STRED_ON, p_light->pflag);
    }
    oe_kresli_kosoctverec(p_cnf, &p_cnf->init_matrix, &s, 0.5f,
      113.0f / 255.0f, 250.0f / 255.0f, 88.0f / 255.0f);
  }
}

/*
  Kresli stred levelu
*/
void oe_kresli_stred_levelu(K_EDITOR * p_cnf)
{
  oe_test_kriz_stred_bod(p_cnf, VELIKOST_STREDU, &p_cnf->elm.p, POPIS_OS_ON,
    STRED_ON, p_cnf->elm.flag);
  oe_kresli_kosoctverec(p_cnf, &p_cnf->init_matrix, &p_cnf->elm.p, 0.7f, 1, 0,
    0);
}

/*
  - udelat grid
  - 2 druhy rotace

  - tedka delam prevedeni vseho n
*/

void oe_kresli_mriz(float sx, float sz, float dx, int num_x, float r, float g,
  float b)
{
  int vert = num_x << 1;
  int x, z;
  float p;

  p = dx * (num_x - 1);

  glColor3f(r, g, b);

  glBegin(GL_LINES);
  for (z = 0; z < vert; z += 2) {
    glVertex3f(sx, 0, sz + (float) (z >> 1) * dx);
    glVertex3f(sx + p, 0, sz + (float) (z >> 1) * dx);
  }
  p = dx * (num_x - 1);
  for (x = 0; x < vert; x += 2) {
    glVertex3f(sx + (float) (x >> 1) * dx, 0, (float) sz);
    glVertex3f(sx + (float) (x >> 1) * dx, 0, (float) sz + p);
  }
  glEnd();
}

void oe_kresli_stred(float d, float r, float g, float b)
{
  glEnable(GL_POLYGON_OFFSET_LINE);
  glPolygonOffset(-20.0f, 1.0f);
  glColor3f(r, g, b);
  glBegin(GL_LINES);
  glVertex3f(d, 0, 0);
  glVertex3f(-d, 0, 0);
  glVertex3f(0, 0, d);
  glVertex3f(0, 0, -d);
  glEnd();
  glDisable(GL_POLYGON_OFFSET_LINE);
}


void oe_kresli_grid(K_EDITOR * p_cnf)
{
  ROVINA r = { 0, 1, 0, 0 };
  BOD a, b, p, k;
  float x, z;
  float vzd;
  static int num_x, dx;
  static GLMATRIX m;

  if (!m._11)
    init_matrix(&m);

  set_matrix_world(&m);

  text_off(GL_TEXTURE_1D);
  text_off(GL_TEXTURE_2D);
  blend_off();

  if (p_cnf->cnf.svetla)
    nastav_material_rgb_MAT(&p_cnf->barva_grid);

  // spocitat prusek s nulovou plochou

  // stredovy bod obrazovky
  transf_2d_3d((float) OXRES / 2, (float) OYRES / 2, 0,
    &a.x, &a.y, &a.z,
    p_cnf->p_camera, p_cnf->p_project, OXRES, OYRES, OXSTART, OYSTART);
  transf_2d_3d((float) OXRES / 2, (float) OYRES / 2, 0.5f,
    &b.x, &b.y, &b.z,
    p_cnf->p_camera, p_cnf->p_project, OXRES, OYRES, OXSTART, OYSTART);
  calc_prusek_bod(&r, &a, &b, &p);

  // krajni bod obrazovky
  transf_2d_3d((float) 0, (float) OYRES, 0,
    &a.x, &a.y, &a.z,
    p_cnf->p_camera, p_cnf->p_project, OXRES, OYRES, OXSTART, OYSTART);
  transf_2d_3d((float) 0, (float) OYRES, 0.5f,
    &b.x, &b.y, &b.z,
    p_cnf->p_camera, p_cnf->p_project, OXRES, OYRES, OXSTART, OYSTART);
  calc_prusek_bod(&r, &a, &b, &k);


  // 2 3d body v jedne rovine
  // p - stredovy bod
  // b - krajni bod
  //oe_kresli_mriz_2(&k, &p, 1.0f, 0.8f, 0.8f, 0.8f);

//  x >>= 1; x <<= 1;
//  z >>= 1; z <<= 1;
//  oe_kresli_mriz(x-100, z-100, 2, 100, 0.8f, 0.8f, 0.8f);

  x = (float) ceil(p.x);
  z = (float) ceil(p.z);
  vzd = (float) ceil(vzdal_bodu_bod(&p, &k)) * 4;
  if (vzd < 50) {
    oe_kresli_mriz(x - vzd, z - vzd, 1, (int) vzd << 1, 0.8f, 0.8f, 0.8f);
  }
  else
    oe_kresli_mriz(x - 50, z - 50, 1, 100, 0.8f, 0.8f, 0.8f);

  oe_kresli_stred(5, 0, 0, 0);
}

void oe_kresli_linelist(K_EDITOR * p_cnf)
{
  int i;

  glColor3ubv((byte *) & p_cnf->linelist_barva);
  glBegin(GL_LINES);
  for (i = 0; i < p_cnf->linelist_num; i++) {
    glVertex2d(p_cnf->linelist_cara[i].x1, p_cnf->linelist_cara[i].y1);
    glVertex2d(p_cnf->linelist_cara[i].x2, p_cnf->linelist_cara[i].y2);
  }
  glEnd();
}

void oe_kresli_kd_strom(KD_BUNKA * p_prvni, int uroven)
{
  dword b = 0x0, a = 0x505050;

  add_mult_color(&b, a, (float) uroven);

  kresli_oktanovou_bunku_minmax(&p_prvni->min, &p_prvni->max, b);
  if (p_prvni->p_next) {
    oe_kresli_kd_strom(p_prvni->p_next, uroven + 1);
    oe_kresli_kd_strom(p_prvni->p_next + 1, uroven + 1);
  }
}

void oe_kresli_group_vertexy(K_EDITOR * p_cnf)
{
  GLMATRIX m;
  float z;
  int o, v, x, y, i, k = K_CHYBA;
  int transform = 0;

  for (i = 0; i < p_cnf->groupnum; i++) {
    if (k != p_cnf->p_group[i].k) {
      k = p_cnf->p_group[i].k;
      transform++;
    }
    if (o != p_cnf->p_group[i].o) {
      o = p_cnf->p_group[i].o;
      transform++;
    }
    if (!p_cnf->p_kont[k] || !p_cnf->p_kont[k]->p_obj[o]) {
      oe_ungroup_vertex(p_cnf, i);
      continue;
    }
    v = p_cnf->p_group[i].v;
    if (v >= p_cnf->p_kont[k]->p_obj[o]->vertexnum) {
      oe_ungroup_vertex(p_cnf, i);
      continue;
    }
    if (transform) {
      if (p_cnf->p_kont[k]->kflag & KONT_KEYFRAME) {
        calc_3d_2d_matrix(&p_cnf->p_kont[k]->p_obj[o]->m,
          p_cnf->p_camera, p_cnf->p_project, &m);
      }
      else {
        calc_3d_2d_matrix(kont_world_matrix(p_cnf->p_kont[k]),
          p_cnf->p_camera, p_cnf->p_project, &m);
      }
      transform = 0;
    }
    if (transf_3d_2d_matrix(p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].x,
        p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].y,
        p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].z,
        &x, &y, &z, &m, OXRES, OYRES, OXSTART, OYSTART,
        p_cnf->kam.near_plane, p_cnf->kam.far_plane)) {
      if (p_cnf->vybr_group == i)
        kresli_ctverec_2d_plnej(x - VEL_CTVERCE, y - VEL_CTVERCE,
          x + VEL_CTVERCE, y + VEL_CTVERCE, 1, 0, 0);
      else
        kresli_ctverec_2d(x - VEL_CTVERCE, y - VEL_CTVERCE, x + VEL_CTVERCE,
          y + VEL_CTVERCE, 1, 0, 0);
    }
  }
}

/*
  Kreslit mlho-kostku
*/
void oe_kresli_mlho_kostky(K_EDITOR * p_cnf)
{
  MLZNA_KOSTKA *p_mlha = p_cnf->p_mlha;
  GLMATRIX world;
  BOD s;

  init_matrix(&world);

  while (p_mlha) {
    kresli_oktanovou_bunku_minmax(&p_mlha->min, &p_mlha->max,
      float_rgba(&p_mlha->r));
    kd_stred_bunky(&p_mlha->min, &p_mlha->max, &s);
    if (p_mlha == p_cnf->p_mlha_akt)
      oe_kresli_kosoctverec(p_cnf, &world, &s, 0.5f, 0.0f, 1.0f, 0.0f);
    else
      oe_kresli_kosoctverec(p_cnf, &world, &s, 0.5f, 0.0f, 0.0f, 1.0f);
    oe_test_kriz_stred_bod(p_cnf, VELIKOST_STREDU, &s, POPIS_OS_ON, STRED_ON,
      p_cnf->p_mlha->flag);
    p_mlha = p_mlha->p_next;
  }
}

void oe_renderuj_mlho_kostky(K_EDITOR * p_cnf)
{
  MLZNA_KOSTKA *p_mlha = p_cnf->p_mlha_akt;

  if (p_mlha && p_cnf->cnf.mlhokostka_aktivni && p_cnf->cnf.mlhokostka_testy) {
    if (kd_je_bod_v_kostce((BOD *) & p_cnf->p_invcam->_41, &p_mlha->min,
        &p_mlha->max))
      enable_fog();
    else
      disable_fog();
  }
}

void oe_kresli_bb_kamery(K_EDITOR * p_cnf)
{
  float r[4] = { 1, 1, 1, 1 };
  int flag = POSUN_OSA_X | POSUN_OSA_Y | POSUN_OSA_Z;
  GLMATRIX world;
  BOD s;

  init_matrix(&world);
  kresli_oktanovou_bunku_minmax(&p_cnf->kam.min, &p_cnf->kam.max,
    float_rgba(r));
  kd_stred_bunky(&p_cnf->kam.min, &p_cnf->kam.max, &s);
  oe_kresli_kosoctverec(p_cnf, &world, &s, 0.5f, 0.0f, 0.0f, 1.0f);
  oe_test_kriz_stred_bod(p_cnf, VELIKOST_STREDU, &s, POPIS_OS_ON, STRED_ON,
    flag);
}

/*
  Grop-vertexy pro plosky
*/
void oe_kresli_group_face(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  int i, k, o, f;
  int flag = p_cnf->groupflag;

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glEnable(GL_POLYGON_OFFSET_LINE);
  glPolygonOffset(-20.0f, 1.0f);

  if (flag & GROUP_KONT && !(flag & (GROUP_OBJ | GROUP_FACE))) {
    for (i = 0; i < p_cnf->groupfacenum; i++) {
      k = p_cnf->p_group_face[i].k;
      if ((p_kont = p_cnf->p_kont[k])) {
        obb_kresli_obalku_kont(p_kont, kont_world_matrix(p_kont), DDRGB(0, 1,
            0));
      }
    }
  }
  else if (flag & GROUP_KONT && flag & GROUP_OBJ && !(flag & GROUP_FACE)) {
    for (i = 0; i < p_cnf->groupfacenum; i++) {
      k = p_cnf->p_group_face[i].k;
      o = p_cnf->p_group_face[i].o;
      p_kont = p_cnf->p_kont[k];
      if (p_kont && p_kont->p_obj[o]) {
        obb_kresli_obalku_obj(p_kont->p_obj[o], kont_world_matrix(p_kont),
          DDRGB(0, 1, 0));
      }
    }
  }
  else if (flag & GROUP_KONT && flag & GROUP_OBJ && flag & GROUP_FACE) {
    glColor3f(0.0f, 1.0f, 0.0f);
    for (i = 0; i < p_cnf->groupfacenum; i++) {
      k = p_cnf->p_group_face[i].k;
      o = p_cnf->p_group_face[i].o;
      f = p_cnf->p_group_face[i].f;
      p_kont = p_cnf->p_kont[k];
      if (p_kont && p_kont->p_obj[o]) {
        set_matrix_world(kont_world_matrix(p_kont));
        oe_kresli_face(p_kont->p_obj[o], f);
      }
    }
  }

  glDisable(GL_POLYGON_OFFSET_LINE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


void oe_kresli_d_vertexy(K_EDITOR * p_cnf)
{
  GLMATRIX m, m2;
  int i;
  float z;
  int x, y, f = 0;

  calc_3d_2d_matrix(init_matrix(&m2), p_cnf->p_camera, p_cnf->p_project, &m);

  for (i = 0; i < p_cnf->dvertnum; i++) {
    if (p_cnf->p_dvert[i].d2) {
      x = (int) p_cnf->p_dvert[i].x;
      y = (int) p_cnf->p_dvert[i].y;
      if (p_cnf->vybr_dvert == i)
        kresli_ctverec_2d_plnej(x - VEL_CTVERCE, y - VEL_CTVERCE,
          x + VEL_CTVERCE, y + VEL_CTVERCE, 0, 1, 0);
      else
        kresli_ctverec_2d(x - VEL_CTVERCE, y - VEL_CTVERCE, x + VEL_CTVERCE,
          y + VEL_CTVERCE, 0, 1, 0);
    }
    else {
      if (transf_3d_2d_matrix(p_cnf->p_dvert[i].x, p_cnf->p_dvert[i].y,
          p_cnf->p_dvert[i].z, &x, &y, &z, &m, OXRES, OYRES, OXSTART, OYSTART,
          p_cnf->kam.near_plane, p_cnf->kam.far_plane)) {
        if (p_cnf->vybr_dvert == i)
          kresli_ctverec_2d_plnej(x - VEL_CTVERCE, y - VEL_CTVERCE,
            x + VEL_CTVERCE, y + VEL_CTVERCE, 0, 1, 0);
        else
          kresli_ctverec_2d(x - VEL_CTVERCE, y - VEL_CTVERCE, x + VEL_CTVERCE,
            y + VEL_CTVERCE, 0, 1, 0);
      }
    }
  }
}

/* Objektovy env mapping
*/
void oe_obj_env_spec(EDIT_MATERIAL * p_mat, EDIT_OBJEKT * p_obj,
  GLMATRIX * p_kw)
{
  MUJ_BOD *p_bod;
  int os = p_mat->flag2 & MAT2_ENV_SPEC_OBE;
  int v;
  float tx, ty, tz;
  float px, py, pz;
  float v1, v2, v3;
  float uhel;
  float r, g, b;
  float rr, rg, rb;

  rr = p_mat->env_r;
  rg = p_mat->env_g;
  rb = p_mat->env_b;

  // z vektor pohledu - optimalizace!
  v1 = p_cnf->p_camera->_13;
  v2 = p_cnf->p_camera->_23;
  v3 = p_cnf->p_camera->_33;

  norm_vect(&v1, &v2, &v3);

  p_bod = p_obj->p_vertex;
  for (v = 0; v < p_obj->vertexnum; v++) {
    px = p_bod[v].nx;
    py = p_bod[v].ny;
    pz = p_bod[v].nz;

    tx = p_kw->_11 * px + p_kw->_21 * py + p_kw->_31 * pz;
    ty = p_kw->_12 * px + p_kw->_22 * py + p_kw->_32 * pz;
    tz = p_kw->_13 * px + p_kw->_23 * py + p_kw->_33 * pz;

    norm_vect(&tx, &ty, &tz);

    uhel = tx * v1 + ty * v2 + tz * v3;
    uhel = (p_mat->env_scale * (p_mat->env_add1 + uhel)) + p_mat->env_add2;

    // uhel je od 0 - 1
    // 
    //uhel += (rand()%1000)*0.0001f;

    if (os) {
      uhel = fabsf(uhel);
      r = (rr * uhel > 1.0f) ? 1.0f : rr * uhel;
      g = (rg * uhel > 1.0f) ? 1.0f : rg * uhel;
      b = (rb * uhel > 1.0f) ? 1.0f : rb * uhel;
    }
    else {
      if (uhel > 0) {
        r = (rr * uhel > 1.0f) ? 1.0f : rr * uhel;
        g = (rg * uhel > 1.0f) ? 1.0f : rg * uhel;
        b = (rb * uhel > 1.0f) ? 1.0f : rb * uhel;
      }
      else {
        r = g = b = 0;
      }
    }
    p_bod[v].sr = r;
    p_bod[v].sg = g;
    p_bod[v].sb = b;
  }
}

void oe_poly_env_spec(EDIT_MATERIAL * p_mat, EDIT_MESH_POLY * p_poly)
{
  poly_env_maping_spec(p_poly, p_mat, p_cnf->p_camera);
}

void oe_animuj_frame_material(dword akt_time, EDIT_MATERIAL * p_mat,
  EDIT_TEXT * p_text)
{
  ANIM_MATERIAL *p_anim = &p_mat->anim;
  ANIM_FRAME *p_frame;
  int next_frame;
  dword flag;

  if (p_anim->frameakt >= p_anim->framenum)
    p_anim->frameakt = 0;

  next_frame = p_anim->frameakt;
  flag = p_anim->p_frame[next_frame].flag;
  p_frame = p_anim->p_frame + next_frame;
  if (p_anim->last_time + p_anim->p_frame[next_frame].time < akt_time
    && !(flag & FRAME_EVENT)) {
    if (flag & FRAME_TEXTURA)
      p_mat->p_text[0] = p_frame->p_text;
    if (flag & FRAME_POSUN) {
      p_mat->ttext[0]._41 = p_frame->u;
      p_mat->ttext[0]._42 = p_frame->v;
    }
    p_anim->last_time = akt_time;
    p_anim->frameakt++;
  }
}

void oe_animuj_frame_material_event(dword akt_time, EDIT_MATERIAL * p_mat,
  EDIT_TEXT * p_text)
{
  ANIM_MATERIAL *p_anim = &p_mat->anim;
  ANIM_FRAME *p_frame;
  int next_frame;
  dword flag;

  if (p_anim->frameakt >= p_anim->framenum)
    p_anim->frameakt = 0;

  next_frame = p_anim->frameakt;
  flag = p_anim->p_frame[next_frame].flag;
  p_frame = p_anim->p_frame + next_frame;
  if (flag & FRAME_EVENT) {
    if (flag & FRAME_TEXTURA)
      p_mat->p_text[0] = p_anim->p_frame[next_frame].p_text;
    if (flag & FRAME_POSUN) {
      p_mat->ttext[0]._41 = p_frame->u;
      p_mat->ttext[0]._42 = p_frame->v;
    }
    p_anim->last_time = akt_time;
    p_anim->frameakt++;
  }
}

void oe_animuj_materialy(K_EDITOR * p_cnf, byte event)
{
  int m;

  if (!event) {
    for (m = 0; m < MAX_EDIT_MATERIALU; m++) {
      if (p_cnf->p_mat[m] && p_cnf->p_mat[m]->flag & MAT_ANIM_FRAME)
        oe_animuj_frame_material(p_cnf->TimeEndLastFrame, p_cnf->p_mat[m],
          p_cnf->p_text);
    }
  }
  else {
    for (m = 0; m < MAX_EDIT_MATERIALU; m++) {
      if (p_cnf->p_mat[m] && p_cnf->p_mat[m]->flag & MAT_ANIM_FRAME)
        oe_animuj_frame_material_event(p_cnf->TimeEndLastFrame,
          p_cnf->p_mat[m], p_cnf->p_text);
    }
  }
}

void oe_init_animace_materialy(K_EDITOR * p_cnf)
{
  int m;

  for (m = 0; m < MAX_EDIT_MATERIALU; m++) {
    if (p_cnf->p_mat[m] && p_cnf->p_mat[m]->flag & MAT_ANIM_FRAME) {
      p_cnf->p_mat[m]->anim.frameakt = 0;
      p_cnf->p_mat[m]->anim.last_time = p_cnf->TimeLastFrame;
      p_cnf->p_mat[m]->p_text[0] = p_cnf->p_mat[m]->anim.p_frame[0].p_text;
    }
  }
}

void oe_kresli_objekt_jednoduse(EDIT_OBJEKT * p_obj, int flag, int kflag)
{
  int i, f1, f2, f3, ind, *p_int;
  int kalk1 = flag & MAT2_MAP1;
  int kalk2 = flag & MAT2_MAP2;
  int kalk3 = flag & MAT2_MAP3;
  int kalk4 = flag & MAT2_MAP4;
  int norm = glstav_pn_triangles
    || flag & (MAT2_CALC_MAP1 | MAT2_CALC_MAP2 | MAT2_CALC_MAP3 |
    MAT2_CALC_MAP4);
  int scale = kflag & KONT_NORMAL_ANIM && p_obj->float_scale != 0.0f;

  glBegin(GL_TRIANGLES);
  for (i = 0; i < p_obj->facenum; i += 3) {
    f1 = p_obj->p_face[i];
    f2 = p_obj->p_face[i + 1];
    f3 = p_obj->p_face[i + 2];

    p_int = glstav_text_map_indicie;
    if ((ind = *p_int) != K_CHYBA)
      glMultiTexCoord2fvARB(GL_TEXTURE0_ARB,
        ((float *) (&p_obj->p_vertex[f1].tu1)) + ((ind) << 1));
    p_int++;
    if ((ind = *p_int) != K_CHYBA)
      glMultiTexCoord2fvARB(GL_TEXTURE1_ARB,
        ((float *) (&p_obj->p_vertex[f1].tu1)) + ((ind) << 1));
    p_int++;
    if ((ind = *p_int) != K_CHYBA)
      glMultiTexCoord2fvARB(GL_TEXTURE2_ARB,
        ((float *) (&p_obj->p_vertex[f1].tu1)) + ((ind) << 1));
    p_int++;
    if ((ind = *p_int) != K_CHYBA)
      glMultiTexCoord2fvARB(GL_TEXTURE3_ARB,
        ((float *) (&p_obj->p_vertex[f1].tu1)) + ((ind) << 1));

    if (norm)
      glNormal3fv(&p_obj->p_vertex[f1].nx);
    glColor4fv(&p_obj->p_vertex[f1].dr);
    glSecondaryColor3fvEXT(&p_obj->p_vertex[f1].sr);
    glVertex3fv(p_obj->p_vertex_kosti ? (float *) (p_obj->p_vertex_kosti +
        f1) : &p_obj->p_vertex[f1].x);

    p_int = glstav_text_map_indicie;
    if ((ind = *p_int) != K_CHYBA)
      glMultiTexCoord2fvARB(GL_TEXTURE0_ARB,
        ((float *) (&p_obj->p_vertex[f2].tu1)) + ((ind) << 1));
    p_int++;
    if ((ind = *p_int) != K_CHYBA)
      glMultiTexCoord2fvARB(GL_TEXTURE1_ARB,
        ((float *) (&p_obj->p_vertex[f2].tu1)) + ((ind) << 1));
    p_int++;
    if ((ind = *p_int) != K_CHYBA)
      glMultiTexCoord2fvARB(GL_TEXTURE2_ARB,
        ((float *) (&p_obj->p_vertex[f2].tu1)) + ((ind) << 1));
    p_int++;
    if ((ind = *p_int) != K_CHYBA)
      glMultiTexCoord2fvARB(GL_TEXTURE3_ARB,
        ((float *) (&p_obj->p_vertex[f2].tu1)) + ((ind) << 1));
    if (norm)
      glNormal3fv(&p_obj->p_vertex[f2].nx);
    glColor4fv(&p_obj->p_vertex[f2].dr);
    glSecondaryColor3fvEXT(&p_obj->p_vertex[f2].sr);
    glVertex3fv(p_obj->p_vertex_kosti ? (float *) (p_obj->p_vertex_kosti +
        f2) : &p_obj->p_vertex[f2].x);

    p_int = glstav_text_map_indicie;
    if ((ind = *p_int) != K_CHYBA)
      glMultiTexCoord2fvARB(GL_TEXTURE0_ARB,
        ((float *) (&p_obj->p_vertex[f3].tu1)) + ((ind) << 1));
    p_int++;
    if ((ind = *p_int) != K_CHYBA)
      glMultiTexCoord2fvARB(GL_TEXTURE1_ARB,
        ((float *) (&p_obj->p_vertex[f3].tu1)) + ((ind) << 1));
    p_int++;
    if ((ind = *p_int) != K_CHYBA)
      glMultiTexCoord2fvARB(GL_TEXTURE2_ARB,
        ((float *) (&p_obj->p_vertex[f3].tu1)) + ((ind) << 1));
    p_int++;
    if ((ind = *p_int) != K_CHYBA)
      glMultiTexCoord2fvARB(GL_TEXTURE3_ARB,
        ((float *) (&p_obj->p_vertex[f3].tu1)) + ((ind) << 1));
    if (norm)
      glNormal3fv(&p_obj->p_vertex[f3].nx);
    glColor4fv(&p_obj->p_vertex[f3].dr);
    glSecondaryColor3fvEXT(&p_obj->p_vertex[f3].sr);
    glVertex3fv(p_obj->p_vertex_kosti ? (float *) (p_obj->p_vertex_kosti +
        f3) : &p_obj->p_vertex[f3].x);
  }
  glEnd();
}

void oe_kresli_objekt_jednoduse_normal(EDIT_OBJEKT * p_obj, float dv)
{
  int i, f1, f2, f3;
  BOD v;


  glBegin(GL_LINES);
  for (i = 0; i < p_obj->facenum; i += 3) {
    f1 = p_obj->p_face[i];
    f2 = p_obj->p_face[i + 1];
    f3 = p_obj->p_face[i + 2];

    glColor3f(0, 1, 0);
    if (p_obj->p_vertex_kosti) {
      v = p_obj->p_vertex_kosti[f1];
    }
    else {
      mujbod2bod(&v, p_obj->p_vertex + f1);
    }
    glVertex3fv((float *) &v);
    glVertex3f(-p_obj->p_vertex[f1].nx * dv + v.x,
      -p_obj->p_vertex[f1].ny * dv + v.y, -p_obj->p_vertex[f1].nz * dv + v.z);

    glColor3f(0, 1, 0);
    if (p_obj->p_vertex_kosti) {
      v = p_obj->p_vertex_kosti[f2];
    }
    else {
      mujbod2bod(&v, p_obj->p_vertex + f2);
    }
    glVertex3fv((float *) &v);
    glVertex3f(-p_obj->p_vertex[f2].nx * dv + v.x,
      -p_obj->p_vertex[f2].ny * dv + v.y, -p_obj->p_vertex[f2].nz * dv + v.z);

    glColor3f(0, 1, 0);
    if (p_obj->p_vertex_kosti) {
      v = p_obj->p_vertex_kosti[f3];
    }
    else {
      mujbod2bod(&v, p_obj->p_vertex + f3);
    }
    glVertex3fv((float *) &v);
    glVertex3f(-p_obj->p_vertex[f3].nx * dv + v.x,
      -p_obj->p_vertex[f3].ny * dv + v.y, -p_obj->p_vertex[f3].nz * dv + v.z);
  }
  glEnd();
}

void oe_kresli_face(EDIT_OBJEKT * p_obj, int face)
{
  if (p_obj->face_typ == GL_TRIANGLES && face < p_obj->facenum - 2) {
    glBegin(GL_TRIANGLES);
    glVertex3f(p_obj->p_vertex[p_obj->p_face[face]].x,
      p_obj->p_vertex[p_obj->p_face[face]].y,
      p_obj->p_vertex[p_obj->p_face[face]].z);
    glVertex3f(p_obj->p_vertex[p_obj->p_face[face + 1]].x,
      p_obj->p_vertex[p_obj->p_face[face + 1]].y,
      p_obj->p_vertex[p_obj->p_face[face + 1]].z);
    glVertex3f(p_obj->p_vertex[p_obj->p_face[face + 2]].x,
      p_obj->p_vertex[p_obj->p_face[face + 2]].y,
      p_obj->p_vertex[p_obj->p_face[face + 2]].z);
    glEnd();
  }
}

// Nastavi material se scene-materialem
void oe_nastav_material_single(EDIT_MATERIAL * p_smat, EDIT_MATERIAL * p_mat,
  EDIT_OBJEKT * p_obj, GLMATRIX * p_world)
{
  MATERIAL_TEXT *p_stg;
  MATERIAL_TEXT *p_stg_smat;
  int i, s, t, rezerva_smat, rezerva_mat, pozice_smat, stage_smat;

  if (!p_mat)
    return;

  /* Alfa-blending - z puvodni funkce
   */
  p_alfa_stage_func[p_mat->alfa_state] ();

  /* Je to nekde uprostred - nastav rezervu
   */
  if (!p_smat || p_mat->flag & MAT_NO_SCMT) {
    p_stg = p_mat->text_state;
    t = 0;
    for (i = 0; i < MAT_TEXTUR; i++) {
      if ((s = p_stg->text_stage) != K_CHYBA) {
        if ((t =
            p_text_stage_func[s] (p_mat, text_stage_edit_blok + s, p_stg, t,
              0)) == K_CHYBA)
          break;
      }
      else {                    // defaultni stage - nic
        text_stage_func_nic(t);
        break;
      }
      p_stg++;
    }
  }
  else {
    p_stg = p_mat->text_state;
    p_stg_smat = p_smat->text_state;
    pozice_smat = p_smat->smaterial_pozice;
    stage_smat = p_stg_smat->text_stage;

    if (p_smat->flag & MAT_SCENE_NUTNY) {
      rezerva_smat = p_smat->textur;
      rezerva_mat = 0;
    }
    else {
      rezerva_smat = 0;
      rezerva_mat = p_mat->textur;
    }

    t = 0;
    for (i = 0; i < MAT_TEXTUR; i++) {
      if (i == pozice_smat) {
        if ((t =
            p_text_stage_func[stage_smat] (p_smat,
              text_stage_edit_blok + stage_smat, p_stg_smat, t,
              rezerva_mat)) == K_CHYBA)
          break;
        rezerva_smat = 0;
      }
      if ((s = p_stg->text_stage) != K_CHYBA) {
        if ((t =
            p_text_stage_func[s] (p_mat, text_stage_edit_blok + s, p_stg, t,
              rezerva_smat)) == K_CHYBA)
          break;
      }
      else {                    // defaultni stage - nic - konec
        break;
      }
      p_stg++;
    }
    if (i < pozice_smat) {
      t =
        p_text_stage_func[stage_smat] (p_smat,
        text_stage_edit_blok + stage_smat, p_stg_smat, t, 0);
    }
    text_stage_func_nic(t);
  }
  /* Nastaveni difusni barvy
   */
  if (p_mat->flag2 & MAT2_DIFFUSE) {
    diffuse_on();
  }
  else {
    diffuse_off(p_mat->dxmat.diffuse_r, p_mat->dxmat.diffuse_g,
      p_mat->dxmat.diffuse_b, p_mat->dxmat.diffuse_a);
  }


  /* Nastaveni spekularni barvy
   */
  if (p_mat->flag2 & MAT2_SPECULAR)
    specular_on();
  else
    specular_off();

  if (p_mat->flag2 & MAT2_ENV_SPEC && p_world) {
    specular_on();
    oe_obj_env_spec(p_mat, p_obj, p_world);
  }

  /* Nastaveni z-bufferu
   */
  if (p_mat->flag & MAT_NO_ZTEST)
    glDisable(GL_DEPTH_TEST);
  deph_test_set((~p_mat->flag) & MAT_NO_ZMASK);

  /* Nastaveni cullingu
   */
  if (p_cnf->cul) {
    if (p_mat->flag & MAT_NO_CULL)
      cull_off();
    else
      cull_on();
  }
}

void oe_zrus_material_single(EDIT_MATERIAL * p_mat)
{
  /* Vyrus z-test
   */
  if (!p_mat)
    return;

  /* Alfa-blending
   */
  p_alfa_stage_func[0] ();

  /* Texturing
   */
  text_stage_func_nic(0);

  if (p_mat->flag & MAT_NO_ZTEST)
    glEnable(GL_DEPTH_TEST);
  if (p_mat->flag & MAT_NO_ZMASK)
    deph_test_set(p_mat->flag & MAT_NO_ZMASK);
}

/*
  Vraci pocet textur na sobe
*/
int oe_nastav_material_single_poly(EDIT_MATERIAL * p_smat,
  EDIT_MATERIAL * p_mat, EDIT_MESH_POLY * p_poly)
{
  MATERIAL_TEXT *p_stg;
  MATERIAL_TEXT *p_stg_smat;
  int i, s, t, rezerva_smat, rezerva_mat, pozice_smat, stage_smat, poly_text;
  int text_num = 1;

  if (!p_mat)
    return (FALSE);

  /* Alfa-blending
   */
  p_alfa_stage_func[p_mat->alfa_state] ();

  /* Texturing
   */
  if (!p_smat || p_mat->flag & MAT_NO_SCMT) {
    p_stg = p_mat->text_state;
    t = 0;
    for (i = 0; i < MAT_TEXTUR; i++) {
      if ((s = p_stg->text_stage) != K_CHYBA) {
        if ((t =
            p_text_stage_func[s] (p_mat, text_stage_edit_blok + s, p_stg, t,
              1)) == K_CHYBA)
          break;
      }
      else {                    // defaultni stage - nic
        text_stage_func_nic_poly(t + 1, t);
        break;
      }
      p_stg++;
    }
  }
  else {
    p_stg = p_mat->text_state;
    p_stg_smat = p_smat->text_state;
    pozice_smat = p_smat->smaterial_pozice;
    stage_smat = p_stg_smat->text_stage;

    if (p_smat->flag & MAT_SCENE_NUTNY) {
      rezerva_smat = p_smat->textur + 1;
      rezerva_mat = 1;
    }
    else {
      rezerva_smat = 1;
      rezerva_mat = p_mat->textur + 1;
    }

    t = 0;
    for (i = 0; i < MAT_TEXTUR; i++) {
      if (i == pozice_smat) {
        if ((t =
            p_text_stage_func[stage_smat] (p_smat,
              text_stage_edit_blok + stage_smat, p_stg_smat, t,
              rezerva_mat)) == K_CHYBA)
          break;
        rezerva_smat = 1;
      }
      if ((s = p_stg->text_stage) != K_CHYBA) {
        if ((t =
            p_text_stage_func[s] (p_mat, text_stage_edit_blok + s, p_stg, t,
              rezerva_smat)) == K_CHYBA)
          break;
      }
      else {                    // defaultni stage - nic - konec
        break;
      }
      p_stg++;
    }
    poly_text = t++;
    if (i < pozice_smat) {
      t =
        p_text_stage_func[stage_smat] (p_smat,
        text_stage_edit_blok + stage_smat, p_stg_smat, t, 1);
    }
    text_stage_func_nic_poly(t, poly_text);
  }

  /* Nastaveni difusni barvy
   */
  if (p_mat->flag2 & MAT2_DIFFUSE) {
    diffuse_on();
  }
  else {
    diffuse_off(p_mat->dxmat.diffuse_r, p_mat->dxmat.diffuse_g,
      p_mat->dxmat.diffuse_b, p_mat->dxmat.diffuse_a);
  }

  /* Nastaveni spekularni barvy
   */
  if (p_mat->flag2 & MAT2_SPECULAR)
    specular_on();
  else
    specular_off();

  /* Spekularni env mapping
   */
  if (p_mat->flag2 & MAT2_ENV_SPEC) {
    specular_on();
    oe_poly_env_spec(p_mat, p_poly);
  }

  /* Nastaveni z-bufferu
   */
  if (p_mat->flag & MAT_NO_ZTEST)
    glDisable(GL_DEPTH_TEST);
  deph_test_set((~p_mat->flag) & MAT_NO_ZMASK);

  /* Nastaveni cullingu
   */
  if (p_cnf->cul) {
    if (p_mat->flag & MAT_NO_CULL)
      cull_off();
    else
      cull_on();
  }

  return (text_num);
}

#define VEL_CTVERCE_KOST 2

void oe_kresli_objekt_kosti_rec_body(K_EDITOR * p_cnf,
  EDIT_KONTEJNER * p_kont, EDIT_OBJEKT * p_obj, JOINT * p_joint)
{
  GLMATRIX m;
  int *p_ind = p_joint->p_vertexlist;
  BOD *p_vert, p;
  int v, x, y;
  float z;

  set_matrix_2d(OXRES, OYRES);

  calc_3d_2d_matrix(p_kont->
    kflag & KONT_KEYFRAME ? &p_obj->m : kont_world_matrix(p_kont),
    p_cnf->p_camera, p_cnf->p_project, &m);

  for (v = 0; v < p_joint->vertexnum; v++) {
    p_vert = (BOD *) (p_obj->p_vertex + p_ind[v]);
    transformuj_bod_matici_bod(p_vert, &p_joint->m, &p);
    if (transf_3d_2d_matrix(p.x, p.y, p.z, &x, &y, &z, &m, OXRES, OYRES,
        OXSTART, OYSTART, p_cnf->kam.near_plane, p_cnf->kam.far_plane)) {
      kresli_ctverec_2d_plnej(x - VEL_CTVERCE_KOST, y - VEL_CTVERCE_KOST,
        x + VEL_CTVERCE_KOST, y + VEL_CTVERCE_KOST, 1, 0, 0);
    }
  }

  ret_matrix_2d();
}

// pivot point root-jointu
void oe_kresli_objekt_kosti_rec(K_EDITOR * p_cnf, BOD * p_bod,
  EDIT_KONTEJNER * p_kont, EDIT_OBJEKT * p_obj, JOINT * p_joint)
{
  BOD p;

  if (!p_joint)
    return;

  p = p_joint->pivot;
  transformuj_bod_bod_matici(&p, &p_joint->m);

  if (p_bod) {
    kresli_caru(p_bod, &p, BARVA_KOSTI);
  }
  kresli_kosoctverec_word(&p, RADIUS_KOSTI,
    p_cnf->jcf.p_joint_akt == p_joint ? BARVA_KOSTI_AKT : BARVA_KOSTI);

  // kresli body aktualni kosti
  if (p_cnf->jcf.p_joint_akt == p_joint) {
    oe_kresli_objekt_kosti_rec_body(p_cnf, p_kont, p_obj, p_joint);
  }

  if (p_joint->p_next)
    oe_kresli_objekt_kosti_rec(p_cnf, p_bod, p_kont, p_obj, p_joint->p_next);
  if (p_joint->p_child)
    oe_kresli_objekt_kosti_rec(p_cnf, &p, p_kont, p_obj, p_joint->p_child);
}

void oe_kresli_objekt_kosti(K_EDITOR * p_cnf, EDIT_KONTEJNER * p_kont,
  EDIT_OBJEKT * p_obj)
{
  EDIT_OBJEKT *p_jobj = oe_cti_joint_objekt(p_cnf);

  if (p_jobj == p_obj && p_cnf->jcf.p_joint_animace_akt) {
    oe_kresli_objekt_kosti_rec(p_cnf, NULL, p_kont, p_obj,
      p_cnf->jcf.p_joint_animace_akt->p_child);
  }
  else if (p_obj->p_joit_animace) {
    oe_kresli_objekt_kosti_rec(p_cnf, NULL, p_kont, p_obj,
      p_obj->p_joit_animace->p_child);
  }
}

void oe_kresli_objekt(K_EDITOR * p_cnf, EDIT_KONTEJNER * p_kont,
  EDIT_OBJEKT * p_obj, EDIT_MATERIAL ** p_mat, int akt_objekt, int akt_kont)
{
  GLMATRIX *p_word =
    p_kont->kflag & KONT_KEYFRAME ? &p_obj->m : kont_world_matrix(p_kont);
  int flag2;

  /* Rendering obalky
   */
  if (p_cnf->cnf.obalky_objekt && akt_kont && akt_objekt) {
    obb_kresli_obalku_obj(p_obj, NULL, DDRGB(1, 1, 1));
  }

  /* Nastav material
   */
  oe_nastav_material_single(p_cnf->p_smat ? *(p_cnf->p_smat) : NULL,
    p_mat[p_obj->material], p_obj, p_word);

  /* Rendering
   */
  flag2 = p_mat[p_obj->material] ? p_mat[p_obj->material]->flag2 : FALSE;
  oe_kresli_objekt_jednoduse(p_obj, flag2, p_kont->kflag);

  /* Zrusi nastaveni materialu
   */
  oe_zrus_material_single(p_mat[p_obj->material]);
}

EDIT_OBJEKT *oe_cti_joint_objekt(K_EDITOR * p_cnf)
{
  int k = p_cnf->jcf.k;
  int o = p_cnf->jcf.o;

  return ((p_cnf->p_kont[k]
      && p_cnf->p_kont[k]->p_obj[o]) ? p_cnf->p_kont[k]->p_obj[o] : NULL);
}

// pokud pruhledne == true -> nekresli se pruhledne objekty
void oe_kresli_kontejner(K_EDITOR * p_cnf, EDIT_MATERIAL ** p_mat,
  EDIT_KONTEJNER * p_kont, int akt_objekt)
{
  EDIT_OBJEKT *p_obj;
  GLMATRIX conv, *p_world = NULL;
  int i, k;

  if (!p_kont)
    return;

  k = p_kont->max_objektu;

  if (p_kont->kflag & KONT_PLG_OFFSET) {
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.0f, -2.0f);
  }

  if (p_kont->kflag & KONT_NO_FOG)
    disable_fog_causal();

  if (!(p_kont->kflag & KONT_KEYFRAME)) {
    p_world = kont_world_matrix(p_kont);
    if (p_cnf->mod_matrix_akt)
      p_world = mat_mult_dir(p_world, &p_cnf->mod_matrix, &conv);
    set_matrix_world(p_world);
  }

  if (p_cnf->cnf.obalky_kontejner && akt_objekt != K_CHYBA) {
    if (!p_world)               // Je to key-frame - nastav world matrix pro obalku
      set_matrix_world(kont_world_matrix(p_kont));
    obb_kresli_obalku_kont(p_kont, NULL, DDRGB(1, 1, 1));
  }

  for (i = 0; i < k; i++) {
    p_obj = p_kont->p_obj[i];
    if (p_obj && p_obj->kreslit) {
      if (p_kont->kflag & KONT_KEYFRAME) {
        p_world =
          (p_cnf->mod_matrix_akt) ? mat_mult_dir(&p_obj->m,
          &p_cnf->mod_matrix, &conv) : &p_obj->m;
        set_matrix_world(p_world);
      }
      if (akt_objekt == i && p_cnf->cnf.objekt_stred) {
        oe_test_kriz_stred(p_cnf, VELIKOST_STREDU, p_obj->pivot.x,
          p_obj->pivot.y, p_obj->pivot.z, POPIS_OS_ON, STRED_OFF, POSUN_VSE);
      }

      oe_kresli_objekt(p_cnf, p_kont, p_obj, p_mat, i == akt_objekt,
        akt_objekt != K_CHYBA);
      oe_kresli_objekt_kosti(p_cnf, p_kont, p_obj);
    }
  }

  if (p_kont->kflag & KONT_NO_FOG)
    enable_fog_causal();

  if (p_kont->kflag & KONT_PLG_OFFSET) {
    glDisable(GL_POLYGON_OFFSET_FILL);
  }
}

void oe_kresli_objekt_dratove(K_EDITOR * p_cnf, EDIT_MATERIAL ** p_mat,
  EDIT_KONTEJNER * p_kont, EDIT_OBJEKT * p_obj, int akt_objekt, int akt_kont)
{

  if (!p_obj || !p_obj->kreslit)
    return;

  if (!p_cnf->cnf.textury && p_cnf->cnf.obalky_objekt && akt_kont
    && akt_objekt) {
    obb_kresli_obalku_obj(p_obj, NULL, DDRGB(1, 0, 0));
  }

  oe_kresli_objekt_jednoduse(p_obj, FALSE, p_kont->kflag);
  if (p_cnf->cnf.norm_vect)
    oe_kresli_objekt_jednoduse_normal(p_obj, p_cnf->cnf.norm_vect_velikost);

  if (akt_kont && akt_objekt) {
    glColor3f(1.0f, 0.0f, 0.0f);
    glPolygonOffset(-20.0f, 1.0f);
    oe_kresli_face(p_obj, p_cnf->vybr_face);
    glPolygonOffset(-1.0f, 1.0f);
  }
}

void oe_kresli_kontejner_dratove(K_EDITOR * p_cnf, EDIT_MATERIAL ** p_mat,
  EDIT_KONTEJNER * p_kont, int akt_objekt)
{
  EDIT_OBJEKT *p_obj;
  GLMATRIX *p_world;
  int i, k = p_kont->max_objektu;

  if (!p_kont || !p_kont->kreslit)
    return;

  if (!(p_kont->kflag & KONT_KEYFRAME)) {
    p_world = kont_world_matrix(p_kont);
    set_matrix_world(p_world);
  }

  text_set_num(0);
  text_off(GL_TEXTURE_1D);
  text_off(GL_TEXTURE_2D);

  if (!p_cnf->cnf.textury && p_cnf->cnf.obalky_kontejner
    && akt_objekt != K_CHYBA) {
    obb_kresli_obalku_kont(p_kont, NULL, DDRGB(0, 1, 0));
  }

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glEnable(GL_POLYGON_OFFSET_LINE);
  glPolygonOffset(-2.0f, 1.0f);

  for (i = 0; i < k; i++) {
    p_obj = p_kont->p_obj[i];
    if (p_obj && p_obj->kreslit) {
      if (p_kont->kflag & KONT_KEYFRAME) {
        set_matrix_world(&p_obj->m);
      }

      if (p_cnf->cnf.objekt_stred)
        oe_test_kriz_stred(p_cnf, VELIKOST_STREDU, p_obj->pivot.x,
          p_obj->pivot.y, p_obj->pivot.z, POPIS_OS_ON, STRED_OFF, POSUN_VSE);

      oe_kresli_objekt_dratove(p_cnf, p_mat, p_kont, p_obj, i == akt_objekt,
        akt_objekt != K_CHYBA);
      oe_kresli_objekt_kosti(p_cnf, p_kont, p_obj);
    }
  }

  glDisable(GL_POLYGON_OFFSET_LINE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void oe_renderuj_scenu_system_2d(K_EDITOR * p_cnf)
{
  text_off(GL_TEXTURE_1D);
  text_off(GL_TEXTURE_2D);
  blend_off();

  set_matrix_2d(OXRES, OYRES);

  if (p_cnf->groupnum)
    oe_kresli_group_vertexy(p_cnf);

  if (p_cnf->dvertnum)
    oe_kresli_d_vertexy(p_cnf);

  if (p_cnf->cnf.nitkovy_kriz) {
    kresli_kurzor_mysi_kriz(OXRES, OYRES);
  }

  if (p_cnf->ctverec) {
    kresli_ctverec_2d(p_cnf->x1, p_cnf->y1, p_cnf->x2, p_cnf->y2,
      p_cnf->ctverec_barva[0], p_cnf->ctverec_barva[1],
      p_cnf->ctverec_barva[2]);
  }

  if (p_cnf->linelist) {
    oe_kresli_linelist(p_cnf);
  }

  ret_matrix_2d();
}

void oe_renderuj_scenu_system_3d(K_EDITOR * p_cnf)
{
  BOD p = { 40.0f, OYRES - 40.0f, 5.0f }, d;
  int i;


  /* Kresli znacku modu
   */
  transf_2d_3d_z(p.x, p.y, p.z, &d.x, &d.y, &d.z, p_cnf->p_camera,
    p_cnf->p_project, OXRES, OYRES, OXSTART, OYSTART, p_cnf->kam.near_plane,
    p_cnf->kam.far_plane);
  glDisable(GL_DEPTH_TEST);
  oe_test_kriz_stred(p_cnf, 2.0f, d.x, d.y, d.z, POPIS_OS_ON, STRED_ON,
    POSUN_VSE);
  glEnable(GL_DEPTH_TEST);

  /*
     Kresli staticke svetla
   */
  if (p_cnf->cnf.svetla_kreslit) {
    for (i = 0; i < MAX_EDIT_SVETEL; i++)
      oe_kresli_staticke_svetlo(p_cnf, p_cnf->light + i, p_cnf->lightakt);
  }

  /*
     Kresli dynamicky svetla
   */
  if (p_cnf->cnf.svetla_kreslit) {
    for (i = 0; i < MAX_FLARE_SVETEL; i++)
      oe_kresli_dynamicke_svetlo(p_cnf, p_cnf->p_dlight + i,
        i == p_cnf->dl_akt);
  }

  /*
     Kresli extra dynamicky svetla
   */
  if (p_cnf->cnf.svetla_kreslit) {
    for (i = 0; i < MAX_FLARE_SVETEL; i++)
      oe_kresli_extra_dynamicke_svetlo(p_cnf, p_cnf->p_edlight + i,
        i == p_cnf->edl_akt);
  }

  /*
     Renderuj Flare-efekty
   */
  if (!p_cnf->cnf.flare)
    oe_kresli_edit_flare(p_cnf, p_cnf->flare, MAX_EDIT_SVETEL);

  /*
     Kresli kamery
   */
  if (p_cnf->cnf.kamera_kreslit) {
    oe_kresli_kamery(p_cnf);
  }

  /*
     Kresli stred levelu
   */
  if (p_cnf->elm.flag & ELIM_MAZAT)
    oe_kresli_stred_levelu(p_cnf);

  /*
     Kresli seznam vybranych plosek
   */
  oe_kresli_group_face(p_cnf);

  /*
     Kresli 3d kostku podle mlznych kostek
   */
  if (p_cnf->cnf.mlhokostku_kreslit) {
    oe_kresli_mlho_kostky(p_cnf);
  }

  /*
     Kresli 3d kostku podle bounding-boxu kamery
   */
  if (p_cnf->cnf.kamera_kostka_kreslit) {
    oe_kresli_bb_kamery(p_cnf);
  }

  /*
     Mrizka pod levelem
   */
  if (p_cnf->cnf.grid && akt_editor == 1)
    oe_kresli_grid(p_cnf);

  /* Kresli-listu
   */
  if (p_cnf->lp.aktivni && akt_editor == GAME_EDITOR)
    oe_kresli_listu_prvku(p_cnf);

  /* Reset nastaveni (textury/barvy)
   */
  oe_renderuj_all_off();
}

void oe_renderuj_all_off(void)
{
  text_init();
  if (glstav_multitext_units > 3) {
    text_set_num(3);
    text_off(GL_TEXTURE_1D);
    text_off(GL_TEXTURE_2D);
  }
  if (glstav_multitext_units > 2) {
    text_set_num(2);
    text_off(GL_TEXTURE_1D);
    text_off(GL_TEXTURE_2D);
  }
  if (glstav_multitext_units > 1) {
    text_set_num(1);
    text_off(GL_TEXTURE_1D);
    text_off(GL_TEXTURE_2D);
  }
  text_set_num(0);
  text_off(GL_TEXTURE_1D);
  text_off(GL_TEXTURE_2D);
  blend_off();
  specular_off();
  reset_stage_bloky();
}

void oe_renderuj_scenu_pre(K_EDITOR * p_cnf)
{
  if (!p_cnf->cnf.render || !karmin_aktivni)
    return;

  text_stage_func_nic(0);
  reset_stage_bloky();
  reset_kamera_flag();
  glstav_zmena_kamery = TRUE;

  /*
     Animuje texturove animace
   */
  if (p_cnf->cnf.animace)
    oe_animuj_materialy(p_cnf, FALSE);

  /*
     Animuje keyframy
   */
  if (ani_animuj_scenu(p_cnf))
    doe_animace_updatuj(p_cnf);

  /* 
     Animuje kosti
   */
  ani_animuj_kosti(p_cnf);

  /*
     Animuje texture materialy
   */
  if (ani_animuj_materialy(p_cnf))
    doe_animace_updatuj_materialy(p_cnf);

  /* Animuje kamery
   */
  if (ani_animuj_kamery(p_cnf))
    doe_animace_updatuj_kamery(p_cnf);

  /* Test mlho-kostek
   */
  oe_renderuj_mlho_kostky(p_cnf);

  /* Mazani barvou
   */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (p_cnf->zdcadlo_akt && p_cnf->cnf.render_zrcadla)
    oe_renderuj_scenu_zrcadlo(p_cnf);

  if (p_cnf->cnf.druh_sceny == SCENA_POLY && p_cnf->cnf.kdtree_render) {
    oe_renderuj_scenu_kdtree(p_cnf, &p_cnf->kdtree);
  }
  else {
    oe_renderuj_scenu_obyc(p_cnf, p_cnf->p_kont, p_cnf->max_kont);
  }

  /* Reset nastaveni
   */
  oe_renderuj_all_off();
}

void oe_renderuj_scenu_po(K_EDITOR * p_cnf)
{

  if (!p_cnf->cnf.render || !karmin_aktivni)
    return;

  /* Vykresleni 3D systemove grafiky
   */
  oe_renderuj_scenu_system_3d(p_cnf);

  /* Vykresleni 2D grafiky
   */
  oe_renderuj_scenu_system_2d(p_cnf);

  /* Prehodi se buffery
   */
  flip();

  doe_updatuj_konzoli_mys(p_cnf);
  oe_updatuj_fps(p_cnf);
}


void oe_renderuj_scenu_editace(K_EDITOR * p_cnf,
  EDIT_KONTEJNER * p_kont[KONT_NUM], int max_kont)
{
  GLMATRIX *p_world;
  dword x, y;
  int akt, i;
  float fx, fy, fz, z;

  for (i = 0; i < max_kont; i++) {
    if (!p_kont[i] || !p_kont[i]->kreslit)
      continue;

    // preskoc pro nekresleni sceny
    if (!p_cnf->kreslit_scenu && p_kont[i]->prvek == K_CHYBA)
      continue;

    // preskoc pro nekresleni levelu
    if (!p_cnf->kreslit_level && p_kont[i]->prvek != K_CHYBA)
      continue;

    if (p_kont[i]->kflag & KONT_ZRCADLO)
      continue;

    p_world = kont_world_matrix(p_kont[i]);

    if (i == p_cnf->vybr_kont)
      akt = p_cnf->vybr_objekt;
    else
      akt = K_CHYBA;

    if (p_cnf->cnf.textury)
      oe_kresli_kontejner(p_cnf, p_cnf->p_mat, p_kont[i], akt);
    if (p_cnf->cnf.dratove)
      oe_kresli_kontejner_dratove(p_cnf, p_cnf->p_mat, p_kont[i], akt);

/*  Kresleni stredu kontejneru
    if(p_cnf->cnf.objekt_stred) {
      text_off();
      if(i == p_cnf->vybr_kont)
        oe_test_kriz_stred(p_cnf,VELIKOST_STREDU,0,0,0,POPIS_OS_ON,STRED_OFF,POSUN_VSE);
    }
*/

    if (p_cnf->cnf.objekt_origin) {
      if (transf_3d_2d(0, 0, 0, &x, &y, &z,
          p_world, p_cnf->p_camera, p_cnf->p_project,
          OXRES, OYRES, OXSTART, OYSTART,
          p_cnf->kam.near_plane, p_cnf->kam.far_plane)) {
        fx = fy = fz = 0;
        transformuj_bod_matici(&fx, &fy, &fz, p_world);
      }
    }
  }
}

/*
   p_kam->flag &= ~(POSUN_OSA_X|POSUN_OSA_Y|POSUN_OSA_Z);
*/
void oe_kresli_kamery(K_EDITOR * p_cnf)
{
  KAMERA *p_kam;
  int i;

  for (i = 0; i < MAX_EDIT_KAMER; i++) {
    p_kam = p_cnf->kamery + i;
    if (p_kam->cislo != K_CHYBA) {
      set_matrix_world(&p_cnf->init_matrix);
      oe_kresli_kameru(p_kam, !p_cnf->cnf.kamera_aktivni
        && p_cnf->kamakt == i);
      if (POSUN_TARGET & p_kam->flag)
        oe_kresli_editacni_stred(p_cnf, &p_kam->t, p_kam->flag);
      if (POSUN_POZICI & p_kam->flag)
        oe_kresli_editacni_stred(p_cnf, &p_kam->p, p_kam->flag);
    }
  }
}

/* Rendering prez obalky
*/
void oe_renderuj_scenu_kdtree_rec(K_EDITOR * p_cnf, KD_BUNKA * p_prvni,
  GLMATRIX * p_m, int kreslit)
{
  EDIT_MESH_POLY *p_poly;
  EDIT_KONTEJNER *p_kont;
  int i;

  if (p_cnf->cnf.kdtree_bunka == p_prvni->cislo || kreslit) {
    text_set(0, GL_TEXTURE_2D);
    kresli_oktanovou_bunku_minmax(&p_prvni->min, &p_prvni->max, 0xffffffff);
    kreslit = TRUE;
  }


  if (p_cnf->cnf.kdtree_bunka == p_prvni->cislo
    || p_cnf->cnf.kdtree_kreslit_vse || kreslit) {
    for (i = 0; i < p_prvni->polozek; i++) {
      if (p_prvni->p_polozka_typ[i] == KD_POLY) {
        p_poly = p_prvni->p_polozka[i];
        if (p_poly->p_lightnum)
          oe_kresli_poly_3d_game(p_poly, p_cnf->p_mat, p_cnf->cnf.light_maps);
        else
          oe_kresli_poly_3d(p_poly, p_cnf->p_mat);
      }
      else {
        p_kont = p_prvni->p_polozka[i];
        oe_kresli_kontejner(p_cnf, p_cnf->p_mat, p_kont, -1);
      }
    }
  }

  if (p_prvni->p_next) {
    oe_renderuj_scenu_kdtree_rec(p_cnf, p_prvni->p_next, p_m, kreslit);
    oe_renderuj_scenu_kdtree_rec(p_cnf, p_prvni->p_next + 1, p_m, kreslit);
  }
}

int viditelnych = 0;
int polygonu = 0;
int facu = 0;

void oe_renderuj_scenu_kdtree(K_EDITOR * p_cnf, KD_BUNKA * p_prvni)
{
  GLMATRIX m;

  /*
     Nulova transformace
   */
  if (p_cnf->polynum) {
    blend_off();

    text_set_num(0);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    text_on(GL_TEXTURE_2D);

    text_set_num(1);
    text_on(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    viditelnych = 0;
    polygonu = 0;
    facu = 0;
    calc_3d_2d_matrix(&p_cnf->init_matrix, p_cnf->p_camera, p_cnf->p_project,
      &m);
    set_matrix_world(&p_cnf->init_matrix);
    if (kd_visibility(&p_prvni->min, &p_prvni->max, &m)) {
      oe_renderuj_scenu_kdtree_rec(p_cnf, p_prvni, &m, FALSE);
    }
  }
}

// Ve hre skusit mazat akorat Z-buffer !!!
void oe_renderuj_scenu_obyc(K_EDITOR * p_cnf,
  EDIT_KONTEJNER * p_kont[KONT_NUM], int max_kont)
{
  int i;

  /*
     Kresleni levelu/sceny
   */
  oe_renderuj_scenu_editace(p_cnf, p_kont, max_kont);

  /*
     Kresli berusci level
   */
  if (p_cnf->kreslit_level)
    be_renderuj_scenu(p_cnf);

  /*
     Nulova transformace
   */
  set_matrix_world(&p_cnf->init_matrix);

  /*
     Kresli seznamy polygonu
   */
  if (p_cnf->polynum) {
    text_set_num(1);
    text_on(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    for (i = 0; i < p_cnf->polynum; i++) {
      if (p_cnf->p_poly[i].kreslit
        && !(p_cnf->p_poly[i].kflag & KONT_ZRCADLO)) {
        if (p_cnf->p_poly[i].p_lightnum)
          oe_kresli_poly_3d_game(p_cnf->p_poly + i, p_cnf->p_mat,
            p_cnf->cnf.light_maps);
        else
          oe_kresli_poly_3d(p_cnf->p_poly + i, p_cnf->p_mat);
      }
    }
  }

  /*
     Renderuj Flare-efekty
   */
  if (p_cnf->cnf.flare)
    oe_kresli_game_flare(p_cnf, p_cnf->flare, MAX_EDIT_SVETEL);
}


void oe_renderuj_scenu_zrcadlo(K_EDITOR * p_cnf)
{
  ZDRCADLO_DESC_POLY *p_poly;
  EDIT_KONTEJNER **p_kont = p_cnf->p_kont;
  int i, max_kont = p_cnf->max_kont;
  int k, o;

  if (p_cnf->cnf.render_zrcadla) {

    p_cnf->mod_matrix_akt = TRUE;
    p_cnf->mod_matrix = p_cnf->zrcadlo.ref;

    oe_calc_clip_focus(p_cnf);

    assert(GL_MAX_CLIP_PLANES >= 4);

    glEnable(GL_CLIP_PLANE0);
    glEnable(GL_CLIP_PLANE1);
    glEnable(GL_CLIP_PLANE2);
    glEnable(GL_CLIP_PLANE3);
    glEnable(GL_CLIP_PLANE4);

    glClipPlane(GL_CLIP_PLANE0, (double *) p_cnf->zrcadlo.r);
    glClipPlane(GL_CLIP_PLANE1, (double *) (p_cnf->zrcadlo.r + 1));
    glClipPlane(GL_CLIP_PLANE2, (double *) (p_cnf->zrcadlo.r + 2));
    glClipPlane(GL_CLIP_PLANE3, (double *) (p_cnf->zrcadlo.r + 3));
    glClipPlane(GL_CLIP_PLANE4, (double *) (p_cnf->zrcadlo.r + 4));

    cull_prehod();

    // Ma se kreslit berusci level ?
    if (p_cnf->kreslit_level)
      be_renderuj_scenu(p_cnf);

    // Ma se kreslit scena ?
    if (p_cnf->kreslit_scenu) {
      for (i = 0; i < max_kont; i++) {
        if (p_kont[i] &&
          p_kont[i]->kreslit &&
          p_cnf->cnf.textury && !(p_kont[i]->kflag & KONT_ZRCADLO))
          oe_kresli_kontejner(p_cnf, p_cnf->p_mat, p_kont[i], K_CHYBA);
      }

      set_matrix_world(&p_cnf->mod_matrix);

      for (i = 0; i < p_cnf->polynum; i++) {
        if (p_cnf->p_poly[i].kreslit
          && !(p_cnf->p_poly[i].kflag & KONT_ZRCADLO))
          if (p_cnf->p_poly[i].p_lightnum)
            oe_kresli_poly_3d_game(p_cnf->p_poly + i, p_cnf->p_mat,
              p_cnf->cnf.light_maps);
          else
            oe_kresli_poly_3d(p_cnf->p_poly + i, p_cnf->p_mat);
      }

      if (p_cnf->cnf.flare)
        oe_kresli_game_flare(p_cnf, p_cnf->flare, MAX_EDIT_SVETEL);
    }

    cull_prehod();

    glDisable(GL_CLIP_PLANE0);
    glDisable(GL_CLIP_PLANE1);
    glDisable(GL_CLIP_PLANE2);
    glDisable(GL_CLIP_PLANE3);
    glDisable(GL_CLIP_PLANE4);

    p_cnf->mod_matrix_akt = FALSE;

  }

  p_poly = p_cnf->zrcadlo.p_poly;
  while (p_poly) {
    k = p_poly->zrcadlo_k;
    o = p_poly->zrcadlo_o;

    if (p_cnf->p_kont[k] && p_cnf->p_kont[k]->p_obj[o]) {
      set_matrix_world(kont_world_matrix(p_cnf->p_kont[k]));
      oe_kresli_objekt(p_cnf, p_cnf->p_kont[k],
        p_cnf->p_kont[k]->p_obj[o], p_cnf->p_mat, FALSE, FALSE);
    }
    else {
      if (p_poly->poly != K_CHYBA && p_poly->poly < p_cnf->polynum) {

        set_matrix_world_init();
        if (p_cnf->p_poly[p_poly->poly].p_lightnum)
          oe_kresli_poly_3d_game(p_cnf->p_poly + p_poly->poly,
            p_cnf->p_mat, p_cnf->cnf.light_maps);
        else
          oe_kresli_poly_3d(p_cnf->p_poly + p_poly->poly, p_cnf->p_mat);
      }
    }
    p_poly = p_poly->p_next;
  }
}

//p_kam->flag &= ~(POSUN_OSA_X|POSUN_OSA_Y|POSUN_OSA_Z);

int oe_test_kriz_stred(K_EDITOR * p_cnf, float vzdal, float x_s, float y_s,
  float z_s, int popis, int stred, int akt_osa)
{
  GLMATRIX inv, rr;
  dword barva;
  int x, y;
  float z;
  BOD p;

  oe_renderuj_all_off();

  if (stred)
    push_matrix_world(init_matrix(&inv));
  else {
    get_matrix_world(&inv);
  }

  p.x = x_s;
  p.y = y_s;
  p.z = z_s;
  calc_3d_2d_matrix(&inv, p_cnf->p_camera, p_cnf->p_project, &rr);
  transformuj_bod_matici(&p.x, &p.y, &p.z, &rr);
  vzdal = vzdal * p.z * 0.1f;

  if (popis) {
    set_matrix_2d(OXRES, OYRES);

    if (transf_3d_2d_matrix(vzdal + x_s, y_s, z_s, &x, &y, &z, &rr,
        OXRES, OYRES, OXSTART, OYSTART,
        p_cnf->kam.near_plane, p_cnf->kam.far_plane))
      ddw_surf_xy(x, y, "X");

    if (transf_3d_2d_matrix(x_s, vzdal + y_s, z_s, &x, &y, &z, &rr,
        OXRES, OYRES, OXSTART, OYSTART,
        p_cnf->kam.near_plane, p_cnf->kam.far_plane))
      ddw_surf_xy(x, y, "Y");

    if (transf_3d_2d_matrix(x_s, y_s, vzdal + z_s, &x, &y, &z, &rr,
        OXRES, OYRES, OXSTART, OYSTART,
        p_cnf->kam.near_plane, p_cnf->kam.far_plane))
      ddw_surf_xy(x, y, "Z");

    ret_matrix_2d();
  }

  glBegin(GL_LINES);
  barva = (akt_osa & POSUN_OSA_X) ? DDRGB(1, 0, 0) : DDRGB(0, 0, 0);
  glColor3ubv((byte *) & barva);
  glVertex3f(x_s, y_s, z_s);
  glVertex3f(vzdal + x_s, y_s, z_s);

  barva = (akt_osa & POSUN_OSA_Y) ? DDRGB(0, 0, 1) : DDRGB(0, 0, 0);
  glColor3ubv((byte *) & barva);
  glVertex3f(x_s, y_s, z_s);
  glVertex3f(x_s, vzdal + y_s, z_s);

  barva = (akt_osa & POSUN_OSA_Z) ? DDRGB(0, 1, 0) : DDRGB(0, 0, 0);
  glColor3ubv((byte *) & barva);
  glVertex3f(x_s, y_s, z_s);
  glVertex3f(x_s, y_s, vzdal + z_s);
  glEnd();

  if (stred)
    pop_matrix_world();

  return (TRUE);
}

int oe_test_kriz_stred_bod(K_EDITOR * p_cnf, float vzdal, BOD * p_bod,
  int popis, int stred, int akt_osa)
{
  return (oe_test_kriz_stred(p_cnf, vzdal, p_bod->x, p_bod->y, p_bod->z,
      popis, stred, akt_osa));
}


// renderovat 2D nebo 3D poly ?
/*
typedef struct _POZICE_BODU {

  D3DVALUE x,y,z;       // 3D body
  D3DVALUE nx,ny,nz;    // normalove vektory
  DWORD    diff;        // diffuse color
  
  D3DVALUE tx,ty,tz,tw; // transformovane souradnice
  dword    transf;      // flag transformace

} POZICE_BODU;

typedef struct _POZICE_BODU_2D {

  D3DVALUE x,y,z,w;  // transformovane souradnice
  DWORD    diff;      // diffuse color 
  dword    transf;    // flag transformace

} POZICE_BODU_2D;

typedef struct _TEXT_KOORD {
 
  D3DVALUE     tu1,tv1;
  D3DVALUE     tu2,tv2;
  POZICE_BODU *p_bod;

} TEXT_KOORD;

typedef struct _EDIT_MESH_POLY { //celej kontejner
 
 POZICE_BODU *p_pozice;  // list bodu
 TEXT_KOORD  *p_koord;   // list texture koordinatu - plosky po 3
  
 dword    flag;          // flag meshu -> stejne jako u kontejneru
 dword    facenum;       // co face to objekt -> kazdy face ma svuj material
 dword    matnum;        // pocet materialu
 word     *p_pocty;      // pocty plosek
 dword    *p_mat;        // pole pointeru na materialy na jednotlive objekty

} EDIT_MESH_POLY;

  Renderovaci postup:

  1. transformace vsech bodu (pokud nejsou)
  2. kopie vsech bodu do listu (vertex-casch)
  3. render z casch
*/

void oe_kresli_poly_3d(EDIT_MESH_POLY * p_poly, EDIT_MATERIAL ** p_mat)
{
  int i;

  oe_nastav_material_single_poly(p_cnf->p_smat ? *(p_cnf->p_smat) : NULL,
    p_mat[p_poly->material], p_poly);
  text_off(GL_TEXTURE_1D);
  text_off(GL_TEXTURE_2D);

  glBegin(GL_TRIANGLES);
  for (i = 0; i < p_poly->facenum; i++) {
    glTexCoord2fv(&p_poly->p_koord[i].tu1);
    glColor4fv(&p_poly->p_koord[i].dr);
    glSecondaryColor3fvEXT(&p_poly->p_koord[i].sr);
    glNormal3fv(&p_poly->p_koord[i].nx);
    glVertex3fv(&p_poly->p_koord[i].x);
  }
  glEnd();
}

void oe_kresli_poly_3d_game(EDIT_MESH_POLY * p_poly, EDIT_MATERIAL ** p_mat,
  int light)
{
  // pozice bodu k 3D renderingu
  int i, l, last, textur, *p_int, ind;

  last = 0;

  if (!light) {
    textur =
      oe_nastav_material_single_poly(p_cnf->p_smat ? *(p_cnf->p_smat) : NULL,
      p_mat[p_poly->material], p_poly);
    text_on(GL_TEXTURE_2D);
  }
  else {
    text_set_num(1);
    text_off(GL_TEXTURE_1D);
    text_off(GL_TEXTURE_2D);
    text_set_num(0);
    text_on(GL_TEXTURE_2D);
  }

  for (l = 0; l < p_poly->lightnum; l++) {
    if (p_poly->p_light[l]) {
      text_set(p_poly->p_light[l]->text, p_poly->p_light[l]->typ);
    }

    glBegin(GL_TRIANGLES);
    for (i = 0; i < p_poly->p_lightnum[l]; i++) {
      if (!light) {
        p_int = glstav_text_map_indicie;
        if ((ind = *p_int) != K_CHYBA)
          glMultiTexCoord2fvARB(GL_TEXTURE0_ARB,
            ((float *) (&p_poly->p_koord[last + i].tu1)) + ((ind) << 1));
        p_int++;
        if ((ind = *p_int) != K_CHYBA)
          glMultiTexCoord2fvARB(GL_TEXTURE1_ARB,
            ((float *) (&p_poly->p_koord[last + i].tu1)) + ((ind) << 1));
        p_int++;
        if ((ind = *p_int) != K_CHYBA)
          glMultiTexCoord2fvARB(GL_TEXTURE2_ARB,
            ((float *) (&p_poly->p_koord[last + i].tu1)) + ((ind) << 1));
        p_int++;
        if ((ind = *p_int) != K_CHYBA)
          glMultiTexCoord2fvARB(GL_TEXTURE3_ARB,
            ((float *) (&p_poly->p_koord[last + i].tu1)) + ((ind) << 1));

        glMultiTexCoord2fvARB(GL_TEXTURE0_ARB + glstav_text_poly_indicie,
          &p_poly->p_koord[last + i].tul);
      }
      else {
        glMultiTexCoord2fvARB(GL_TEXTURE0_ARB,
          &p_poly->p_koord[last + i].tul);
      }
      glColor4fv(&p_poly->p_koord[last + i].dr);
      glSecondaryColor3fvEXT(&p_poly->p_koord[last + i].sr);
      glNormal3fv(&p_poly->p_koord[last + i].nx);
      glVertex3fv(&p_poly->p_koord[last + i].x);
    }
    glEnd();

    last += p_poly->p_lightnum[l];
  }
}

void oe_kresli_editacni_stred(K_EDITOR * p_cnf, BOD * p_stred, int flag)
{
  text_off(GL_TEXTURE_1D);
  text_off(GL_TEXTURE_2D);
  glDisable(GL_DEPTH_TEST);
  oe_test_kriz_stred(p_cnf, VELIKOST_STREDU, p_stred->x, p_stred->y,
    p_stred->z, POPIS_OS_ON, STRED_ON, flag);
  glEnable(GL_DEPTH_TEST);
}

void oe_kresli_listu_prvku(K_EDITOR * p_cnf)
{
  LISTA_PRVKU *p_lista = &p_cnf->lp;
  B_KONFIG *p_bnf = &p_cnf->bnf;
  GLMATRIX cam, inv, prj;
  BOD p = { -0.6f, 0, 0.6f };
  float barvy[4], nr;
  int i, k, oken;
  int x, y, dx, dy;

  rgb_float(p_cnf->barva_pozadi, barvy);

  x = 0;
  y = p_cnf->wyres - LISTA_X;
  dx = LISTA_X;
  dy = LISTA_X;
  oken = p_cnf->lp.oken;

  nr = 1.0f;
  projection_matrix(&prj, DEG2RAD(30.0f), (float) dx / (float) dy, nr,
    100.0f);
  set_matrix_project_push(&prj);

  calc_camera_bod(&cam, &inv, &p, 6.5f, DEG2RAD(45.0f), DEG2RAD(45.0f));
  get_matrix_camera(&inv);
  set_matrix_camera(&cam);

  glEnable(GL_SCISSOR_TEST);

  glViewport(x, y, p_cnf->wxres, dy);
  glScissor(x, y, p_cnf->wxres, dy);
  glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0.5f, 0.5f, 1.0f, 1.0f);

  for (i = 0; i < oken; i++) {
    k = i + p_lista->prvni;

    if (k < p_lista->prvnum) {
      k = p_lista->p_prv[k];
      glViewport(x, y, dx, dy);
      glScissor(x, y, dx, dy);
      if (i == p_lista->prvakt)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      if (k != K_CHYBA && p_bnf->p_prv_dat[k]) {
        oe_kresli_kontejner(p_cnf, p_cnf->bnf.p_mat,
          p_bnf->p_prv_dat[k]->p_kont, K_CHYBA);
      }
      x += LISTA_X;
    }
  }

  glClearColor(barvy[0], barvy[1], barvy[2], 1.0f);

  glDisable(GL_SCISSOR_TEST);
  glViewport(p_cnf->wx, p_cnf->wy, p_cnf->wxres, p_cnf->wyres);

  set_matrix_project_pop();
  set_matrix_camera(&inv);
}
