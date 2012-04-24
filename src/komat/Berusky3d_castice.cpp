/* 
  Casticovy system pro Berusky II
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

extern G_KONFIG ber, *p_ber;

ParHandle par_vyrob(void)
{
  PARMETAC *p_par = (PARMETAC *) mmalloc(sizeof(p_par[0]));

  if (p_ber->p_par) {
    p_ber->p_par->p_prev = p_par;
    p_par->p_next = p_ber->p_par;
  }
  p_ber->p_par = p_par;

  return ((ParHandle) p_par);
}

ParHandle par_set_param(ParHandle ph, int material, int flag, BOD * p_pos,
  BOD * p_pivot)
{
  PARMETAC *p_par = (PARMETAC *) ph;

  p_par->material = material;
  p_par->flag = flag;

  init_matrix(&p_par->world);
  p_par->world._41 = p_pos->x;
  p_par->world._42 = p_pos->y;
  p_par->world._43 = p_pos->z;
  return ((ParHandle) p_par);
}

ParHandle par_set_y_plane(ParHandle ph, float y_plane)
{
  PARMETAC *p_par = (PARMETAC *) ph;

  p_par->y_plane = y_plane;
  return ((ParHandle) p_par);
}

void par_zrus(ParHandle ph)
{
  PARMETAC *p_par = (PARMETAC *) ph;
  PARMETAC_HNIZDO *p_hnizdo = p_par->p_hnizdo, *p_next;

  if (p_par->p_flag)
    *(p_par->p_flag) = K_CHYBA;
  if (p_par->p_endfce)
    p_par->p_endfce(p_par->param, p_par->param2, 
                    reinterpret_cast<size_ptr>(p_par->p_param));

  if (p_ber->p_par == p_par) {  // smaz prvni
    p_ber->p_par = p_par->p_next;
    if (p_par->p_next)
      p_par->p_next->p_prev = NULL;
  }
  else {
    p_par->p_prev->p_next = p_par->p_next;
    if (p_par->p_next)
      p_par->p_next->p_prev = p_par->p_prev;
  }

  while (p_hnizdo) {
    p_next = p_hnizdo->p_next;
    free(p_hnizdo);
    p_hnizdo = p_next;
  }

  free(p_par);
}

void par_zrus_end(PARMETAC * p_par)
{
  PARMETAC_HNIZDO *p_hnizdo = p_par->p_hnizdo, *p_next;

  if (p_par->p_endfce)
    p_par->p_endfce(p_par->param, p_par->param2, 
                    reinterpret_cast<size_ptr>(p_par->p_param));

  while (p_hnizdo) {
    p_next = p_hnizdo->p_next;
    free(p_hnizdo);
    p_hnizdo = p_next;
  }
  free(p_par);
}

int par_get(ParHandle ph)
{
  PARMETAC *p_par = (PARMETAC *) ph;

  return (p_par->pnum);
}

ParHandle par_pripoj_funkci(ParHandle ph, END_FUNKCE p_fce, size_ptr param,
  size_ptr param2, size_ptr p_param)
{
  PARMETAC *p_par = (PARMETAC *) ph;

  p_par->p_endfce = p_fce;
  p_par->p_param = reinterpret_cast<void *>(p_param);
  p_par->param = param;
  p_par->param2 = param2;
  return (ph);
}

ParHandle par_go(ParHandle ph, int *p_flag, int start, int stop)
{
  PARMETAC *p_par = (PARMETAC *) ph;
  PARMETAC_HNIZDO *p_hnizdo = p_par->p_hnizdo;

  p_par->flag |= TPAR_ANIMATE;
  p_par->p_flag = p_flag;

  p_par->start = p_ber->TimeEndLastFrame + start;
  if (stop)
    p_par->stop = p_ber->TimeEndLastFrame + stop;

  while (p_hnizdo) {
    p_hnizdo->time_last = p_par->start;
    if (p_par->flag & TPAR_FIRST_LOST)
      p_hnizdo->time_last += p_hnizdo->time_interval;
    p_hnizdo->time_last += p_hnizdo->time_first;
    p_hnizdo = p_hnizdo->p_next;
  }

  return (ph);
}

void par_vloz_strepy(ParHandle ph, PAR_STREPINA * p_part, int pocet)
{
  PARMETAC *p_par = (PARMETAC *) ph;
  int i, p1;

  p_par->flag |= TPAR_STREP;

  p1 = pocet - 1;

  for (i = 1; i < p1; i++) {
    p_part[i].aktivni = TRUE;
    p_part[i].p_next = p_part + i + 1;
    p_part[i].p_prev = p_part + i - 1;
    transformuj_bod_bod_matici(&p_part[i].p, &p_par->world);
  }

  p_part[0].aktivni = TRUE;
  p_part[0].p_next = p_part + 1;
  p_part[0].p_prev = NULL;
  transformuj_bod_bod_matici(&p_part[0].p, &p_par->world);
  if (p1) {
    p_part[p1].p_prev = p_part + p1 - 1;
    transformuj_bod_bod_matici(&p_part[p1].p, &p_par->world);
  }
  p_part[p1].p_next = NULL;
  p_part[p1].aktivni = TRUE;
  p_par->p_first = p_part;
  p_par->pnum = pocet;
}

void *par_cti_prvni(ParHandle ph)
{
  PARMETAC *p_par = (PARMETAC *) ph;

  return (p_par->p_first);
}

void par_vloz_fleky(ParHandle ph, PAR_FLEK * p_part, int pocet)
{
  PARMETAC *p_par = (PARMETAC *) ph;
  int i, p1;

  p_par->flag |= TPAR_FLEK;

  p1 = pocet - 1;

  for (i = 1; i < p1; i++) {
    p_part[i].p_next = p_part + i + 1;
    p_part[i].p_prev = p_part + i - 1;
    transformuj_bod_bod_matici(&p_part[i].p, &p_par->world);
  }

  p_part[0].p_next = p_part + 1;
  p_part[0].p_prev = NULL;
  transformuj_bod_bod_matici(&p_part[0].p, &p_par->world);
  if (p1) {
    p_part[p1].p_prev = p_part + i - 1;
    transformuj_bod_bod_matici(&p_part[p1].p, &p_par->world);
  }
  p_part[p1].p_next = NULL;
  p_par->p_first = p_part;
  p_par->pnum = pocet;
}


int pe_updatuj_strepiny(G_KONFIG * p_ber, PARMETAC * p_par)
{
  GAME_MESH_OLD *p_mesh;
  PAR_STREPINA *p_part;
  BOD t;
  BOD *p_str;
  BOD *p_max;
  BOD *p_min;
  float koef;
  int vid, atime;
  int htest;
  int x, y, z;
  int yplane_low;
  int yplane_top;
  int frame;

  atime = p_ber->TimeEndLastFrame;
  if (!(p_par->flag & TPAR_ANIMATE))
    return (FALSE);
  if (p_par->start > atime)
    return (FALSE);
  if (p_par->stop && p_par->stop < atime) {
    *(p_par->p_flag) = K_CHYBA;
    return (FALSE);
  }

  frame = p_par->flag & TPAR_FRAME;
  htest = p_par->flag & TPAR_HTEST;
  yplane_top = p_par->flag & TPAR_YPLANE_TOP;
  yplane_low = p_par->flag & TPAR_YPLANE_LOW;

  koef = p_ber->TimeLastFrame / 1000.0f;

  p_min = &p_par->min;
  p_max = &p_par->max;

  p_max->x = -FLT_MAX;
  p_max->y = -FLT_MAX;
  p_max->z = -FLT_MAX;

  p_min->x = FLT_MAX;
  p_min->y = FLT_MAX;
  p_min->z = FLT_MAX;

  p_par->pnum = 0;

  p_part = (PAR_STREPINA *) p_par->p_first;
  while (p_part) {

    if (frame) {
      p_part->frame += p_part->framedelta * koef;
    }

    if (p_part->rychlost > 0.0f) {
      p_part->rychlost -= p_part->rychlost * (p_part->utlum * koef);
      if (p_part->rychlost < 0.0f)
        p_part->rychlost = 0.0f;
    }

    p_part->vaha += p_part->vaha * (p_part->g * koef);
    p_str = &p_part->p;

    vektor_add(p_str, vektor_mult_skalar(&p_part->dir,
        p_part->rychlost * koef, &t), p_str);
    p_str->y -= p_part->vaha * koef;

    vid = TRUE;
    if ((yplane_low && p_str->y < p_par->y_plane) ||
      (yplane_top && p_str->y > p_par->y_plane)) {
      vid = 0;
    }
    else if (htest) {
      // test - neni pod podlahou
      if (p_str->y <= p_ber->y_start) {
        vid = 0;
      }
      else {
        x = (int) floor(p_str->x - p_ber->x_start) >> 1;
        y = (int) floor(p_str->y - p_ber->y_start) >> 1;
        z = (int) floor(p_str->z - p_ber->z_start) >> 1;

        if (x >= 0 && x < p_ber->x_num &&
          y >= 0 && y < p_ber->y_num && z >= 0 && z < p_ber->z_num) {
          assert(ber_pozice_v_levelu(x, y, z, p_ber->x_num,
              p_ber->z_num) < p_ber->hitnum);
          vid =
            !p_ber->p_hit_list[ber_pozice_v_levelu(x, y, z, p_ber->x_num,
              p_ber->z_num)];
          if (vid) {
            ber_mesh_render_list_reset(p_ber);
            while ((p_mesh =
                ber_mesh_render_list_next_flag(p_ber, KONT_PRVEK,
                  KONT_DRAW_NOBALKA))) {
              if (obb_je_bod_v_kostce_aabb(&p_mesh->obb_world, p_str)) {
                vid = 0;
                goto konec_testu;
              }
            }
          }
        }
        else {
          vid = TRUE;
        }
      }
    }

  konec_testu:;

    p_part->aktivni = vid;

    //.. vyhod neaktivni castici ze seznamu    
    if (!vid) {
      //p_part->p.x = p_part->p.y = p_part->p.z = 0.0f;
      if (p_part->p_prev) {     // je uprostred
        p_part->p_prev->p_next = p_part->p_next;
        if (p_part->p_next)
          p_part->p_next->p_prev = p_part->p_prev;
      }
      else {                    // je na zacatku seznamu
        p_par->p_first = p_part->p_next;
        if (p_part->p_next)
          p_part->p_next->p_prev = NULL;
      }
    }
    else {
      if (p_str->x < p_min->x)
        p_min->x = p_str->x;
      if (p_str->y < p_min->y)
        p_min->y = p_str->y;
      if (p_str->z < p_min->z)
        p_min->z = p_str->z;

      if (p_str->x > p_max->x)
        p_max->x = p_str->x;
      if (p_str->y > p_max->y)
        p_max->y = p_str->y;
      if (p_str->z > p_max->z)
        p_max->z = p_str->z;

      p_par->pnum++;
    }

    p_part = p_part->p_next;
  }

  if (!p_par->pnum) {
    if (p_par->flag & TPAR_AUTOREMOVE) {
      par_zrus((ParHandle)p_par);    // tady uz to neexistuje !!!
    }
    else {
      if (p_par->p_flag)
        *(p_par->p_flag) = K_CHYBA;
      if (p_par->p_endfce)
        p_par->p_endfce(p_par->param, p_par->param2, 
                        reinterpret_cast<size_ptr>(p_par->p_param));
    }
    return (FALSE);
  }
  return (p_par->pnum);
}


/*
  Updatovac fleku...
*/
int pe_updatuj_fleky(G_KONFIG * p_ber, PARMETAC * p_par)
{
  PAR_FLEK *p_part;
  BOD *p_max;
  BOD *p_min;
  BOD *p_bod;
  BOD *p_pos, nx, ny;
  float koef;
  int vid, atime;
  int diff;
  int scale, scale2;
  int operace;
  int frame;

  atime = p_ber->TimeEndLastFrame;
  if (!(p_par->flag & TPAR_ANIMATE))
    return (FALSE);
  if (p_par->start > atime)
    return (FALSE);
  if (p_par->stop && p_par->stop < atime) {
    *(p_par->p_flag) = K_CHYBA;
    return (FALSE);
  }

  koef = p_ber->TimeLastFrame / 1000.0f;

  p_min = &p_par->min;
  p_max = &p_par->max;

  p_max->x = -FLT_MAX;
  p_max->y = -FLT_MAX;
  p_max->z = -FLT_MAX;

  p_min->x = FLT_MAX;
  p_min->y = FLT_MAX;
  p_min->z = FLT_MAX;

  p_par->pnum = 0;

  scale = p_par->flag & TPAR_SCALE;
  scale2 = p_par->flag & TPAR_SCALE_ADD;
  diff = p_par->flag & TPAR_DIFF;
  operace = p_par->flag & TPAR_VETSI;
  frame = p_par->flag & TPAR_FRAME;

  p_part = (PAR_FLEK *) p_par->p_first;
  while (p_part) {

    if (scale) {
      p_part->rychlost_x += p_part->rychlost_x * (p_part->utlum_x * koef);
      p_part->rychlost_y += p_part->rychlost_y * (p_part->utlum_y * koef);
    }
    else if (scale2) {
      p_part->rychlost_x += (p_part->utlum_x * koef);
      p_part->rychlost_y += (p_part->utlum_y * koef);
    }
    if (diff) {
      p_part->r += p_part->dr * koef;
      p_part->g += p_part->dg * koef;
      p_part->b += p_part->db * koef;
    }
    if (frame) {
      p_part->frame += p_part->framedelta * koef;
    }
    p_part->a += p_part->da * koef;


    p_part->aktivni = vid =
      operace ? (p_part->a > p_part->ka) : (p_part->a < p_part->ka);

    //.. vyhod neaktivni castici ze seznamu
    if (!vid) {
      if (p_part->p_prev) {
        p_part->p_prev->p_next = p_part->p_next;
        if (p_part->p_next)
          p_part->p_next->p_prev = p_part->p_prev;
      }
      else {
        p_par->p_first = p_part->p_next;
        if (p_part->p_next)
          p_part->p_next->p_prev = NULL;
      }
    }
    else {

      p_pos = &p_part->p;
      vektor_mult_skalar(&p_part->nx, p_part->rychlost_x, &nx);
      vektor_mult_skalar(&p_part->ny, p_part->rychlost_y, &ny);

      // vypocet 4 tmp bodu
      p_bod = p_part->tmp;
      p_bod->x = p_pos->x - nx.x - ny.x;
      p_bod->y = p_pos->y - nx.y - ny.y;
      p_bod->z = p_pos->z - nx.z - ny.z;

      if (p_bod->x < p_min->x)
        p_min->x = p_bod->x;
      if (p_bod->y < p_min->y)
        p_min->y = p_bod->y;
      if (p_bod->z < p_min->z)
        p_min->z = p_bod->z;
      if (p_bod->x > p_max->x)
        p_max->x = p_bod->x;
      if (p_bod->y > p_max->y)
        p_max->y = p_bod->y;
      if (p_bod->z > p_max->z)
        p_max->z = p_bod->z;

      p_bod = p_part->tmp + 1;
      p_bod->x = p_pos->x + nx.x - ny.x;
      p_bod->y = p_pos->y + nx.y - ny.y;
      p_bod->z = p_pos->z + nx.z - ny.z;

      if (p_bod->x < p_min->x)
        p_min->x = p_bod->x;
      if (p_bod->y < p_min->y)
        p_min->y = p_bod->y;
      if (p_bod->z < p_min->z)
        p_min->z = p_bod->z;
      if (p_bod->x > p_max->x)
        p_max->x = p_bod->x;
      if (p_bod->y > p_max->y)
        p_max->y = p_bod->y;
      if (p_bod->z > p_max->z)
        p_max->z = p_bod->z;

      p_bod = p_part->tmp + 2;
      p_bod->x = p_pos->x - nx.x + ny.x;
      p_bod->y = p_pos->y - nx.y + ny.y;
      p_bod->z = p_pos->z - nx.z + ny.z;

      if (p_bod->x < p_min->x)
        p_min->x = p_bod->x;
      if (p_bod->y < p_min->y)
        p_min->y = p_bod->y;
      if (p_bod->z < p_min->z)
        p_min->z = p_bod->z;
      if (p_bod->x > p_max->x)
        p_max->x = p_bod->x;
      if (p_bod->y > p_max->y)
        p_max->y = p_bod->y;
      if (p_bod->z > p_max->z)
        p_max->z = p_bod->z;

      p_bod = p_part->tmp + 3;
      p_bod->x = p_pos->x + nx.x + ny.x;
      p_bod->y = p_pos->y + nx.y + ny.y;
      p_bod->z = p_pos->z + nx.z + ny.z;

      if (p_bod->x < p_min->x)
        p_min->x = p_bod->x;
      if (p_bod->y < p_min->y)
        p_min->y = p_bod->y;
      if (p_bod->z < p_min->z)
        p_min->z = p_bod->z;
      if (p_bod->x > p_max->x)
        p_max->x = p_bod->x;
      if (p_bod->y > p_max->y)
        p_max->y = p_bod->y;
      if (p_bod->z > p_max->z)
        p_max->z = p_bod->z;

      p_par->pnum++;
    }

    p_part = p_part->p_next;
  }

  if (!p_par->pnum) {
    if (p_par->flag & TPAR_AUTOREMOVE) {
      par_zrus((ParHandle) p_par);    // tady uz to neexistuje !!!
    }
    else {
      if (p_par->p_flag)
        *(p_par->p_flag) = K_CHYBA;
      if (p_par->p_endfce)
        p_par->p_endfce(p_par->param, p_par->param2, 
                        reinterpret_cast<size_ptr>(p_par->p_param));
    }
    return (FALSE);
  }
  return (p_par->pnum);
}

/* **************************************************************************
     Castice na koure - kourovy stopy
   **************************************************************************
*/
int par_vloz_kour_stopu(ParHandle ph, PAR_KOUR_STOPA * p_part, int pocet)
{
  PARMETAC *p_par = (PARMETAC *) ph;
  int i, p1;

  p_par->flag |= TPAR_KOUR_STOPA;

  p1 = pocet - 1;
  for (i = 0; i < p1; i++) {
    p_part[i].p_next = p_part + i + 1;
  }
  p_part[i].p_next = NULL;
  p_par->p_first = p_part;

  return (TRUE);
}

/* Set-Up hnizda
*/
HnizdoHandle par_vloz_hnizdo(ParHandle ph)
{
  PARMETAC *p_par = (PARMETAC *) ph;
  PARMETAC_HNIZDO *p_hnizdo =
    (PARMETAC_HNIZDO *) mmalloc(sizeof(p_hnizdo[0]));
  p_par->hnum++;
  p_hnizdo->p_next = p_par->p_hnizdo;
  if (p_hnizdo->p_next)
    p_hnizdo->p_next->p_prev = p_hnizdo;
  p_par->p_hnizdo = p_hnizdo;

  return ((ParHandle) p_hnizdo);
}

HnizdoHandle par_vloz_hnizdo_komplet(HnizdoHandle hh, int time_interval,
  BOD * p_pivot, PAR_KOUR_STOPA * p_part)
{
  PARMETAC_HNIZDO *p_hnizdo = (PARMETAC_HNIZDO *) hh;

  p_hnizdo->p_pivot = p_pivot;
  p_hnizdo->time_interval = time_interval;
  memcpy(&p_hnizdo->rychlost_x, &p_part->rychlost_x, sizeof(float) * 15);
  return (hh);
}

HnizdoHandle par_vloz_hnizdo_timer(HnizdoHandle hh, int time_interval,
  int time_first)
{
  PARMETAC_HNIZDO *p_hnizdo = (PARMETAC_HNIZDO *) hh;

  p_hnizdo->time_interval = time_interval;
  p_hnizdo->time_first = time_first;
  return (hh);
}

HnizdoHandle par_vloz_hnizdo_pust_castici(HnizdoHandle hh)
{
  PARMETAC_HNIZDO *p_hnizdo = (PARMETAC_HNIZDO *) hh;

  p_hnizdo->time_last -= p_hnizdo->time_interval;
  return (hh);
}

PAR_KOUR_STOPA *par_vloz_hnizdo_pust_castice(ParHandle ph, HnizdoHandle hh,
  int pocet)
{
  PARMETAC_HNIZDO *p_hnizdo = (PARMETAC_HNIZDO *) hh;
  PARMETAC *p_mt = (PARMETAC *) ph;
  PAR_KOUR_STOPA *p_par;
  int i;
  int dir;
  int par3d;
  dword atime;

  if (!p_hnizdo->p_pivot || !p_mt->p_first)
    return (NULL);

  atime = p_ber->TimeEndLastFrame;
  dir = p_mt->flag & TPAR_DIR;
  par3d = p_mt->flag & TPAR_3D;

  for (i = 0; i < pocet; i++) {
    if (!p_mt->p_first)
      return ((PAR_KOUR_STOPA *) p_hnizdo->p_first);

    p_par = (PAR_KOUR_STOPA *) p_mt->p_first;
    p_mt->p_first = p_par->p_next;

    p_par->p = *p_hnizdo->p_pivot;
    if (dir && p_hnizdo->p_dir)
      p_par->dir = *p_hnizdo->p_dir;
    if (par3d) {
      p_par->nx = *p_hnizdo->p_nx;
      p_par->ny = *p_hnizdo->p_ny;
    }

    p_par->time_vznik = atime;

    p_par->r = p_hnizdo->r;
    p_par->g = p_hnizdo->g;
    p_par->b = p_hnizdo->b;
    p_par->a = p_hnizdo->a;
    p_par->rychlost_x = p_hnizdo->rychlost_x;
    p_par->rychlost_y = p_hnizdo->rychlost_y;
    p_par->dr = p_hnizdo->dr;
    p_par->dg = p_hnizdo->dg;
    p_par->db = p_hnizdo->db;
    p_par->da = p_hnizdo->da;
    p_par->ka = p_hnizdo->ka;
    p_par->utlum_x = p_hnizdo->utlum_x;
    p_par->utlum_y = p_hnizdo->utlum_y;
    p_par->frame = p_hnizdo->frame;
    p_par->framedelta = p_hnizdo->framedelta;
    p_par->x_max = p_hnizdo->x_max;
    p_par->x_min = p_hnizdo->x_min;
    p_par->z_max = p_hnizdo->z_max;
    p_par->z_min = p_hnizdo->z_min;
    p_par->rotace = p_hnizdo->rotace;
    p_par->y_plane = p_hnizdo->y_plane;


    // Insert-first
    p_par->p_next = (PAR_KOUR_STOPA *) p_hnizdo->p_first;
    if (p_par->p_next) {
      p_par->p_next->p_prev = p_par;
    }
    p_hnizdo->p_first = p_par;
    p_par->p_prev = NULL;
  }

  return ((PAR_KOUR_STOPA *) p_hnizdo->p_first);
}

HnizdoHandle par_vloz_hnizdo_pivot(HnizdoHandle hh, BOD * p_pivot)
{
  PARMETAC_HNIZDO *p_hnizdo = (PARMETAC_HNIZDO *) hh;

  p_hnizdo->p_pivot = p_pivot;
  return (hh);
}

HnizdoHandle par_vloz_hnizdo_rotaci(HnizdoHandle hh, char rotace)
{
  PARMETAC_HNIZDO *p_hnizdo = (PARMETAC_HNIZDO *) hh;

  p_hnizdo->rotace = rotace;
  return (hh);
}

HnizdoHandle par_vloz_hnizdo_pivot_normal(HnizdoHandle hh, BOD * p_nx,
  BOD * p_ny)
{
  PARMETAC_HNIZDO *p_hnizdo = (PARMETAC_HNIZDO *) hh;

  p_hnizdo->p_nx = p_nx;
  p_hnizdo->p_ny = p_ny;
  return (hh);
}

HnizdoHandle par_vloz_hnizdo_dir(HnizdoHandle hh, BOD * p_dir)
{
  PARMETAC_HNIZDO *p_hnizdo = (PARMETAC_HNIZDO *) hh;

  p_hnizdo->p_dir = p_dir;
  return (hh);
}

HnizdoHandle par_vloz_hnizdo_vitr(HnizdoHandle hh, BOD * p_vitr)
{
  PARMETAC_HNIZDO *p_hnizdo = (PARMETAC_HNIZDO *) hh;

  p_hnizdo->p_vitr = p_vitr;
  return (hh);
}

HnizdoHandle par_vloz_hnizdo_scale(HnizdoHandle hh, float rx, float ry,
  float utlum_x, float utlum_y)
{
  PARMETAC_HNIZDO *p_hnizdo = (PARMETAC_HNIZDO *) hh;

  p_hnizdo->rychlost_x = rx;
  p_hnizdo->rychlost_y = ry;
  p_hnizdo->utlum_x = utlum_x;
  p_hnizdo->utlum_y = utlum_y;
  return (hh);
}

HnizdoHandle par_vloz_hnizdo_diff(HnizdoHandle hh, float r, float g, float b,
  float a, float dr, float dg, float db, float da, float ka)
{
  PARMETAC_HNIZDO *p_hnizdo = (PARMETAC_HNIZDO *) hh;

  p_hnizdo->r = r;
  p_hnizdo->g = g;
  p_hnizdo->b = b;
  p_hnizdo->a = a;
  p_hnizdo->dr = dr;
  p_hnizdo->dg = dg;
  p_hnizdo->db = db;
  p_hnizdo->da = da;
  p_hnizdo->ka = ka;
  return (hh);
}

HnizdoHandle par_vloz_hnizdo_y_plane(HnizdoHandle hh, float y_plane)
{
  PARMETAC_HNIZDO *p_hnizdo = (PARMETAC_HNIZDO *) hh;

  p_hnizdo->y_plane = y_plane;
  return (hh);
}

HnizdoHandle par_vloz_hnizdo_frame(HnizdoHandle hh, float frame,
  float framedelta)
{
  PARMETAC_HNIZDO *p_hnizdo = (PARMETAC_HNIZDO *) hh;

  p_hnizdo->frame = frame;
  p_hnizdo->framedelta = framedelta;
  return (hh);
}

HnizdoHandle par_vloz_hnizdo_clip(HnizdoHandle hh, float min_x, float min_z,
  float max_x, float max_z)
{
  PARMETAC_HNIZDO *p_hnizdo = (PARMETAC_HNIZDO *) hh;

  p_hnizdo->x_min = min_x;
  p_hnizdo->x_max = max_x;
  p_hnizdo->z_min = min_z;
  p_hnizdo->z_max = max_z;
  return (hh);
}

PARMETAC_HNIZDO *par_cti_hnizdo(HnizdoHandle hh)
{
  return ((PARMETAC_HNIZDO *) hh);
}

PAR_KOUR_STOPA *par_cti_hnizdo_castice(HnizdoHandle hh)
{
  PARMETAC_HNIZDO *p_hnizdo = (PARMETAC_HNIZDO *) hh;

  return ((PAR_KOUR_STOPA *) p_hnizdo->p_first);
}

int par_get_hnizda(ParHandle ph)
{
  PARMETAC *p_par = (PARMETAC *) ph;

  return (p_par->hnum);
}

void par_zrus_hnizdo(ParHandle ph, HnizdoHandle hh)
{
  PARMETAC_HNIZDO *p_hnizdo = (PARMETAC_HNIZDO *) hh;
  PARMETAC *p_par = (PARMETAC *) ph;
  PAR_KOUR_STOPA *p_next = (PAR_KOUR_STOPA *) p_hnizdo->p_first, *p_tmp;


  // Prevedu zbyvajici castice do spolecneho fondu  
  while (p_next) {
    p_tmp = p_next->p_next;
    p_next->p_next = (PAR_KOUR_STOPA *) p_par->p_first;
    p_par->p_first = p_next;
    p_next = p_tmp;
  }

  // delete-first
  if (p_par->p_hnizdo == p_hnizdo) {
    p_par->p_hnizdo = p_hnizdo->p_next;
    if (p_par->p_hnizdo) {
      p_par->p_hnizdo->p_prev = NULL;
    }
  }
  else {
    p_hnizdo->p_prev->p_next = p_hnizdo->p_next;
    if (p_hnizdo->p_next)
      p_hnizdo->p_next->p_prev = p_hnizdo->p_prev;
  }


  free(p_hnizdo);
}

/* Updatuj 
*/
int pe_updatuj_kour_stopa(G_KONFIG * p_ber, PARMETAC * p_mt)
{
  GAME_MESH_OLD *p_mesh;
  GLMATRIX *p_cam = p_ber->p_camera;
  GLMATRIX *p_inv = p_ber->p_invcam;
  PARMETAC_HNIZDO *p_hnizdo, *p_tmp;
  PAR_KOUR_STOPA *p_par, *p_next;
  BOD *p_max;
  BOD *p_min;
  BOD tmp;
  BOD *p_pivot = NULL;
  BOD *p_pos;
  float koef;
  int tx, ty, tz;
  int vid, atime;
  int diff;
  int scale;
  int scale2;
  int operace;
  int vitr;
  int dir;
  int par3d;
  int parhl;
  int yplane_top;
  int yplane_low;
  int htest;
  int frame;
  int pivot;
  int flag;

  atime = p_ber->TimeEndLastFrame;

  if (!(p_mt->flag & TPAR_ANIMATE)) {
    return (FALSE);
  }
  if (p_mt->start > atime) {
    return (FALSE);
  }
  if (p_mt->stop && p_mt->stop < atime) {
    *(p_mt->p_flag) = K_CHYBA;
    return (FALSE);
  }

  koef = p_ber->TimeLastFrame / 1000.0f;

  p_mt->pnum = 0;
  p_mt->hnum = 0;

  flag = p_mt->flag;
  scale = flag & TPAR_SCALE;
  scale2 = flag & TPAR_SCALE_ADD;
  diff = flag & TPAR_DIFF;
  operace = flag & TPAR_VETSI;
  vitr = flag & TPAR_VITR;
  dir = flag & TPAR_DIR;
  par3d = flag & TPAR_3D;
  parhl = flag & TPAR_HALF_LIFE;
  yplane_top = flag & TPAR_YPLANE_TOP;
  yplane_low = flag & TPAR_YPLANE_LOW;
  htest = flag & TPAR_HTEST;
  frame = flag & TPAR_FRAME;
  pivot = flag & TPAR_POS_PIVOT;

  if (pivot) {
    p_pivot = p_mt->p_pivot;
  }

  p_hnizdo = p_mt->p_hnizdo;
  while (p_hnizdo) {

    p_hnizdo->pnum = 0;

    /* Pridani nove castice (pokud je na ne cas)
       - vezmu ji v seznamu volnych castic
     */
    if (p_hnizdo->p_pivot &&
      (int) atime - (int) p_hnizdo->time_last >=
      (int) p_hnizdo->time_interval) {
      p_hnizdo->time_last = atime;

      if (!p_mt->p_first)
        continue;

      p_par = (PAR_KOUR_STOPA *) p_mt->p_first;
      p_mt->p_first = p_par->p_next;

      p_par->p = *p_hnizdo->p_pivot;
      if (dir && p_hnizdo->p_dir)
        p_par->dir = *p_hnizdo->p_dir;
      if (par3d) {
        p_par->nx = *p_hnizdo->p_nx;
        p_par->ny = *p_hnizdo->p_ny;
      }

      p_par->time_vznik = atime;

      p_par->r = p_hnizdo->r;
      p_par->g = p_hnizdo->g;
      p_par->b = p_hnizdo->b;
      p_par->a = p_hnizdo->a;
      p_par->rychlost_x = p_hnizdo->rychlost_x;
      p_par->rychlost_y = p_hnizdo->rychlost_y;
      p_par->dr = p_hnizdo->dr;
      p_par->dg = p_hnizdo->dg;
      p_par->db = p_hnizdo->db;
      p_par->da = p_hnizdo->da;
      p_par->ka = p_hnizdo->ka;
      p_par->utlum_x = p_hnizdo->utlum_x;
      p_par->utlum_y = p_hnizdo->utlum_y;
      p_par->frame = p_hnizdo->frame;
      p_par->framedelta = p_hnizdo->framedelta;
      p_par->x_max = p_hnizdo->x_max;
      p_par->x_min = p_hnizdo->x_min;
      p_par->z_max = p_hnizdo->z_max;
      p_par->z_min = p_hnizdo->z_min;
      p_par->rotace = p_hnizdo->rotace;
      p_par->y_plane = p_hnizdo->y_plane;

      // Insert-first
      p_par->p_next = (PAR_KOUR_STOPA *) p_hnizdo->p_first;
      if (p_par->p_next) {
        p_par->p_next->p_prev = p_par;
      }
      p_hnizdo->p_first = p_par;
      p_par->p_prev = NULL;
    }

    p_min = &p_hnizdo->min;
    p_max = &p_hnizdo->max;

    p_max->x = -FLT_MAX;
    p_max->y = -FLT_MAX;
    p_max->z = -FLT_MAX;

    p_min->x = FLT_MAX;
    p_min->y = FLT_MAX;
    p_min->z = FLT_MAX;

    /* Update stavajicich
     */
    p_par = (PAR_KOUR_STOPA *) p_hnizdo->p_first;
    while (p_par) {
      p_next = p_par->p_next;

      p_par->a += p_par->da * koef;
      vid = operace ? (p_par->a > p_par->ka) : (p_par->a < p_par->ka);

      //.. vyhod neaktivni castici ze seznamu
      if (!vid) {
        /* Vyrad castici z hnizda a strc ji do seznamu volnych castic
         */
        if (p_par->p_prev) {
          p_par->p_prev->p_next = p_par->p_next;
          if (p_par->p_next)
            p_par->p_next->p_prev = p_par->p_prev;
        }
        else {
          p_hnizdo->p_first = p_par->p_next;
          if (p_par->p_next)
            p_par->p_next->p_prev = NULL;
        }

        /* Soupni ji od seznamu volnych castic
         */
        p_par->p_next = (PAR_KOUR_STOPA *) p_mt->p_first;
        p_mt->p_first = p_par;

      }
      else {
        if (scale) {
          p_par->rychlost_x += p_par->rychlost_x * (p_par->utlum_x * koef);
          p_par->rychlost_y += p_par->rychlost_y * (p_par->utlum_y * koef);
        }
        else if (scale2) {
          p_par->rychlost_x += (p_par->utlum_x * koef);
          p_par->rychlost_y += (p_par->utlum_y * koef);
        }
        if (diff) {
          p_par->r += p_par->dr * koef;
          p_par->g += p_par->dg * koef;
          p_par->b += p_par->db * koef;
        }
        if (dir) {
          vektor_add(&p_par->p, vektor_mult_skalar(&p_par->dir, koef, &tmp),
            &p_par->p);
        }
        if (vitr) {
          vektor_add(&p_par->p, vektor_mult_skalar(p_hnizdo->p_vitr, koef,
              &tmp), &p_par->p);
        }
        if (frame) {
          p_par->frame += p_par->framedelta * koef;
        }

        /* Aktivni castici pouzij k vypoctu kostky
         */
        p_pos = &p_par->rp;
        if (pivot) {
          vektor_add(&p_par->p, p_pivot, p_pos);
        }
        else {
          *p_pos = p_par->p;
        }

        /* Testy na umisteni castice v prostoru
         */

        if ((yplane_low && p_pos->y < p_par->y_plane) ||
          (yplane_top && p_pos->y > p_par->y_plane)) {
          // Vyrad castici z hnizda a strc ji do seznamu volnych castic

        vyrad_castici:
          if (p_par->p_prev) {
            p_par->p_prev->p_next = p_par->p_next;
            if (p_par->p_next)
              p_par->p_next->p_prev = p_par->p_prev;
          }
          else {
            p_hnizdo->p_first = p_par->p_next;
            if (p_par->p_next)
              p_par->p_next->p_prev = NULL;
          }

          // Soupni ji do seznamu volnych castic
          p_par->p_next = (PAR_KOUR_STOPA *) p_mt->p_first;
          p_mt->p_first = p_par;

          p_par = p_next;
          continue;
        }


        if (htest) {
          if (p_pos->y <= p_ber->y_start) {
            goto vyrad_castici;
          }
          else {

            tx = (int) floor(p_pos->x - p_ber->x_start) >> 1;
            ty = (int) floor(p_pos->y - p_ber->y_start) >> 1;
            tz = (int) floor(p_pos->z - p_ber->z_start) >> 1;

            if (tx >= 0 && tx < p_ber->x_num &&
              ty >= 0 && ty < p_ber->y_num && tz >= 0 && tz < p_ber->z_num) {

              assert(ber_pozice_v_levelu(tx, ty, tz, p_ber->x_num,
                  p_ber->z_num) < p_ber->hitnum);
              if (p_ber->p_hit_list[ber_pozice_v_levelu(tx, ty, tz,
                    p_ber->x_num, p_ber->z_num)])
                goto vyrad_castici;
              else {
                ber_mesh_render_list_reset(p_ber);

                while ((p_mesh =
                    (GAME_MESH_OLD *) ber_mesh_render_list_next_flag(p_ber,
                      KONT_PRVEK, KONT_DRAW_NOBALKA))) {
                  if (obb_je_bod_v_kostce_aabb(&p_mesh->obb_world, p_pos)) {
                    goto vyrad_castici;
                  }
                }
              }
            }
          }
        }

        if (!par3d) {
          kd_bunka_min_max(p_pos, p_min, p_max);
        }
        else if (par3d || parhl) {
          kd_bunka_min_max_koule(p_pos, MAX(p_par->rychlost_x,
              p_par->rychlost_y), p_min, p_max);
        }

        p_hnizdo->pnum++;
      }
      p_par = p_next;
    }
    p_mt->pnum += p_hnizdo->pnum;

    p_tmp = p_hnizdo->p_next;

    // Zrusi se hnizdo
    if (p_mt->flag & TPAR_AUTOREMOVE && !p_hnizdo->pnum && !p_hnizdo->p_pivot) {
      par_zrus_hnizdo((ParHandle) p_mt, (HnizdoHandle) p_hnizdo);
    }
    else {
      p_mt->hnum++;
    }

    p_hnizdo = p_tmp;
  }

  return (p_mt->pnum);
}

/* 
   Update casticovych systemu... 
*/
void pe_updatuj(G_KONFIG * p_ber)
{
  PARMETAC *p_par = p_ber->p_par;
  PARMETAC *p_next = NULL;

  while (p_par) {
    p_next = p_par->p_next;

    if (p_par->flag & TPAR_STREP)
      pe_updatuj_strepiny(p_ber, p_par);
    else if (p_par->flag & TPAR_KOUR_STOPA)
      pe_updatuj_kour_stopa(p_ber, p_par);
    else if (p_par->flag & TPAR_FLEK)
      pe_updatuj_fleky(p_ber, p_par);
    else
      assert(0);

    p_par = p_next;
  }
}


/* ***************************************************************
     Vodni dilo milesovsko
   ***************************************************************
*/
/* ***************************************************************
     Vodni dilo 2
   ***************************************************************
*/
VodaHandle vod_vyrob(int meshu)
{
  VODA2 *p_voda;

  p_voda = (VODA2 *) mmalloc(sizeof(p_voda[0]));

  p_voda->p_mesh =
    (GAME_MESH_OLD **) mmalloc(sizeof(p_voda->p_mesh[0]) * meshu);

  p_voda->meshmax = meshu;

  p_voda->p_next = p_ber->p_voda;
  if (p_voda->p_next) {
    p_voda->p_next->p_prev = p_voda;
  }
  p_ber->p_voda = p_voda;

  return ((VodaHandle) p_voda);
}

void vod_zrus_vodniky(VODNIK * p_vodnik)
{
  VODNIK *p_next;

  while (p_vodnik) {
    p_next = p_vodnik->p_next;
    free(p_vodnik->p_vzdal);
    free(p_vodnik);
    p_vodnik = p_next;
  }
}

void vod_zrus(VODA2 ** p_tmp)
{
  VODA2 *p_voda = *p_tmp;
  VODA2 *p_next;

  while (p_voda) {
    p_next = p_voda->p_next;
    vod_zrus_vodniky(p_voda->p_vodnik);
    free(p_voda->p_vertexy);
    free(p_voda->p_mesh);
    free(p_voda);
    p_voda = p_next;
  }
  if (p_tmp)
    *p_tmp = NULL;
}

VodaHandle vod_pridej_mesh(VodaHandle vh, MeshHandle mh)
{
  int mesh = p_ber->p_prv_lev[mh]->mesh;
  VODA2 *p_voda = (VODA2 *) vh;
  GAME_MESH_OLD *p_mesh;

  if (mesh == K_CHYBA)
    return (K_CHYBA);
  else
    p_mesh = p_ber->p_mesh[p_ber->p_prv_lev[mh]->mesh];

  if (p_mesh) {
    p_voda->p_mesh[p_voda->meshnum++] = p_mesh;
    assert(p_voda->meshnum <= p_voda->meshmax);

    p_mesh->p_data->kflag |= KONT_DRAW_VODA;
    transformuj_mesh(p_mesh);
    mesh_pridej_vodavertexy(p_mesh);
    return (vh);
  }
  else                          // pridava se staticky mesh -> k_chyba
    return (K_CHYBA);
}

VodaHandle vod_uzavri_meshe(VodaHandle vh)
{
  VODA2 *p_voda = (VODA2 *) vh;
  VODOVERTEX *p_vert;
  BOD *p_bod;
  BODRGBA *p_diff;
  BODRGB *p_spec;
  BODRGBA *p_diff_mat;
  BODRGB *p_spec_mat;
  int vert = 0, vnum;
  int i, j;

  for (i = 0; i < p_voda->meshnum; i++) {
    vert += p_voda->p_mesh[i]->vertexnum;
  }

  p_vert = p_voda->p_vertexy = (VODOVERTEX *)mmalloc(sizeof(p_voda->p_vertexy[0]) * vert);

  p_voda->vertexu = vert;
  vert = 0;
  for (i = 0; i < p_voda->meshnum; i++) {
    vnum = p_voda->p_mesh[i]->vertexnum;
    p_bod = p_voda->p_mesh[i]->p_vertex_pos;
    p_diff = p_voda->p_mesh[i]->p_vertex_diff_voda;
    p_spec = p_voda->p_mesh[i]->p_vertex_spec_voda;
    p_diff_mat = p_voda->p_mesh[i]->p_vertex_diff;
    p_spec_mat = p_voda->p_mesh[i]->p_vertex_spec;
    for (j = 0; j < vnum; j++) {
      p_vert[vert].p_pos = p_bod + j;
      p_vert[vert].p_diff = p_diff + j;
      p_vert[vert].p_diff_mat = p_diff_mat + j;
      if (p_spec) {
        p_vert[vert].p_spec_mat = p_spec_mat + j;
        p_vert[vert].p_spec = p_spec + j;
      }
      else {
        p_vert[vert].p_spec_mat = p_vert[vert].p_spec = NULL;
      }
      p_vert[vert].vyska = p_bod[j].y;
      vert++;
    }
  }

  assert(p_voda->vertexu == vert);
  return (vh);
}

VodnikHandle vod_vloz_vodnika(VodaHandle vh, BOD * p_pos, int flag,
  float amplituda, float perioda, float fi,
  float damplituda, float dperioda, float dfi)
{
  VODOVERTEX *p_vert, *p_vr;
  VODA2 *p_voda = (VODA2 *) vh;
  float *p_vzdal;
  VODNIK *p_vodnik;
  int i;

  p_vodnik = (VODNIK *)mmalloc(sizeof(p_vodnik[0]));

  p_vodnik->p = *p_pos;
  p_vodnik->flag = flag;
  p_vodnik->amplituda = amplituda;
  p_vodnik->perioda = perioda;
  p_vodnik->fi = fi;
  p_vodnik->damplituda = damplituda;
  p_vodnik->dperioda = dperioda;
  p_vodnik->dfi = dfi;

  p_vodnik->vertexnum = p_voda->vertexu;
  p_vert = p_vodnik->p_vert = p_voda->p_vertexy;

  p_vzdal = p_vodnik->p_vzdal = (float *)
    mmalloc(sizeof(p_vzdal[0]) * p_vodnik->vertexnum);

  for (i = 0; i < p_vodnik->vertexnum; i++) {
    p_vr = p_vert + i;
    if (flag & VOD_BOD) {
      p_vzdal[i] = vzdal_bodu_bod(p_vr->p_pos, p_pos);
    }
    else {
      if (flag & VOD_X_PLANE) {
        p_vzdal[i] = fabsf(p_vr->p_pos->x - p_pos->x);
      }
      else if (flag & VOD_Y_PLANE) {
        p_vzdal[i] = fabsf(p_vr->p_pos->y - p_pos->y);
      }
      else {
        p_vzdal[i] = fabsf(p_vr->p_pos->z - p_pos->z);
      }
    }
  }

  p_vodnik->p_next = p_voda->p_vodnik;
  p_voda->p_vodnik = p_vodnik;

  return ((VodnikHandle) p_vodnik);
}

VodnikHandle vod_uprav_vodnika_par(VodnikHandle vh, float amplituda,
  float perioda, float fi, float damplituda, float dperioda, float dfi)
{
  VODNIK *p_vodnik = (VODNIK *) vh;

  p_vodnik->amplituda = amplituda;
  p_vodnik->perioda = perioda;
  p_vodnik->fi = fi;
  p_vodnik->damplituda = damplituda;
  p_vodnik->dperioda = dperioda;
  p_vodnik->dfi = dfi;
  return (vh);
}

VodnikHandle vod_cti_vodnika_par(VodnikHandle vh, float *p_amplituda,
  float *p_perioda, float *p_fi, float *p_damplituda, float *p_dperioda,
  float *p_dfi)
{
  VODNIK *p_vodnik = (VODNIK *) vh;

  *p_amplituda = p_vodnik->amplituda;
  *p_perioda = p_vodnik->perioda;
  *p_fi = p_vodnik->fi;
  *p_damplituda = p_vodnik->damplituda;
  *p_dperioda = p_vodnik->dperioda;
  *p_dfi = p_vodnik->dfi;
  return (vh);
}

VodnikHandle vod_uprav_vodnika_flag(VodnikHandle vh, int flag)
{
  VODNIK *p_vodnik = (VODNIK *) vh;

  p_vodnik->flag = flag;
  return (vh);
}

VodnikHandle vod_cti_vodnika_flag(VodnikHandle vh, int *p_flag)
{
  VODNIK *p_vodnik = (VODNIK *) vh;

  *p_flag = p_vodnik->flag;
  return (vh);
}

VodnikHandle vod_vloz_vodnika_diff(VodnikHandle vh, float dr, float dg,
  float db, float da)
{
  VODNIK *p_vodnik = (VODNIK *) vh;

  p_vodnik->dr = dr;
  p_vodnik->dg = dg;
  p_vodnik->db = db;
  p_vodnik->da = da;
  return (vh);
}

VodnikHandle vod_vloz_vodnika_add_diff(VodnikHandle vh, float dr, float dg,
  float db, float da)
{
  VODNIK *p_vodnik = (VODNIK *) vh;

  p_vodnik->adr = dr;
  p_vodnik->adg = dg;
  p_vodnik->adb = db;
  p_vodnik->ada = da;
  return (vh);
}

VodnikHandle vod_vloz_vodnika_spec(VodnikHandle vh, float sr, float sg,
  float sb)
{
  VODNIK *p_vodnik = (VODNIK *) vh;

  p_vodnik->sr = sr;
  p_vodnik->sg = sg;
  p_vodnik->sb = sb;
  return (vh);
}

VodnikHandle vod_vloz_vodnika_add_spec(VodnikHandle vh, float sr, float sg,
  float sb)
{
  VODNIK *p_vodnik = (VODNIK *) vh;

  p_vodnik->asr = sr;
  p_vodnik->asg = sg;
  p_vodnik->asb = sb;
  return (vh);
}

void vod_updatuj_vodniky(VODNIK * p_vodnik)
{
  float koef = p_ber->TimeLastFrame / 1000.0f;
  float *p_sinus_table = p_ber->sinus_table;
  float vzdal, am, fi, per;
  int diff, diff1, diff2, diff3;
  int spec, spec1, spec2, spec3;
  VODOVERTEX *p_vert;
  float *p_vzdal;
  float sinus;
  float dr, dg, db, da;
  float sr, sg, sb;
  float adr, adg, adb, ada;
  float asr, asg, asb;
  int i;


  /* Zpet do vodniho sveta !
   */
  while (p_vodnik) {
    if (p_vodnik->flag & VOD_AKTIVNI) {

      p_vodnik->amplituda += p_vodnik->damplituda * koef;
      p_vodnik->perioda += p_vodnik->dperioda * koef;
      p_vodnik->fi += p_vodnik->dfi * koef;

      am = p_vodnik->amplituda;
      per = p_vodnik->perioda;
      fi = p_vodnik->fi;

      diff = p_vodnik->flag & (VOD_DIFF_MOD | VOD_DIFF_ADD | VOD_DIFF_SUB);
      if (diff) {
        diff1 = p_vodnik->flag & VOD_DIFF_MOD;
        diff2 = p_vodnik->flag & VOD_DIFF_ADD;
        diff3 = p_vodnik->flag & VOD_DIFF_SUB;

        dr = p_vodnik->dr;
        dg = p_vodnik->dg;
        db = p_vodnik->db;
        da = p_vodnik->da;

        adr = p_vodnik->adr;
        adg = p_vodnik->adg;
        adb = p_vodnik->adb;
        ada = p_vodnik->ada;
      }

      spec = p_vodnik->flag & (VOD_SPEC_MOD | VOD_SPEC_ADD | VOD_SPEC_SUB);
      if (spec) {
        spec1 = p_vodnik->flag & VOD_SPEC_MOD;
        spec2 = p_vodnik->flag & VOD_SPEC_ADD;
        spec3 = p_vodnik->flag & VOD_SPEC_SUB;

        sr = p_vodnik->sr;
        sg = p_vodnik->sg;
        sb = p_vodnik->sb;

        asr = p_vodnik->asr;
        asg = p_vodnik->asg;
        asb = p_vodnik->asb;
      }

      p_vzdal = p_vodnik->p_vzdal;
      for (i = 0; i < p_vodnik->vertexnum; i++) {
        p_vert = p_vodnik->p_vert + i;

        vzdal = per * p_vzdal[i] + fi;
        sinus = p_sinus_table[ftoi(vzdal) % SIN_TABLE_SIZE];

        p_vert->sinus += sinus;
        p_vert->p_pos->y += am * sinus;

        sinus = p_vert->sinus;

        if (diff || spec) {
          if (sinus > 1.0f)
            sinus = 1.0f;
          if (sinus < 0.0f)
            sinus = 0.0f;
        }

        if (diff) {
          if (diff1) {
            p_vert->p_diff->r = p_vert->p_diff_mat->r * dr * sinus + adr;
            p_vert->p_diff->g = p_vert->p_diff_mat->g * dg * sinus + adg;
            p_vert->p_diff->b = p_vert->p_diff_mat->b * db * sinus + adb;
            p_vert->p_diff->a = p_vert->p_diff_mat->a * da * sinus + ada;
          }
          else if (diff2) {
            p_vert->p_diff->r = p_vert->p_diff_mat->r + dr * sinus + adr;
            p_vert->p_diff->g = p_vert->p_diff_mat->g + dg * sinus + adg;
            p_vert->p_diff->b = p_vert->p_diff_mat->b + db * sinus + adb;
            p_vert->p_diff->a = p_vert->p_diff_mat->a + da * sinus + ada;
          }
          else {
            p_vert->p_diff->r = p_vert->p_diff_mat->r - dr * sinus + adr;
            p_vert->p_diff->g = p_vert->p_diff_mat->g - dg * sinus + adg;
            p_vert->p_diff->b = p_vert->p_diff_mat->b - db * sinus + adb;
            p_vert->p_diff->a = p_vert->p_diff_mat->a - da * sinus + ada;
          }
        }

        if (spec) {
          if (spec1) {
            p_vert->p_spec->r = p_vert->p_spec_mat->r * sr * sinus + asr;
            p_vert->p_spec->g = p_vert->p_spec_mat->g * sg * sinus + asg;
            p_vert->p_spec->b = p_vert->p_spec_mat->b * sb * sinus + asb;
          }
          else if (spec2) {
            p_vert->p_spec->r = p_vert->p_spec_mat->r + sr * sinus + asr;
            p_vert->p_spec->g = p_vert->p_spec_mat->g + sg * sinus + asg;
            p_vert->p_spec->b = p_vert->p_spec_mat->b + sb * sinus + asb;
          }
          else {
            p_vert->p_spec->r = p_vert->p_spec_mat->r - sr * sinus + asr;
            p_vert->p_spec->g = p_vert->p_spec_mat->g - sg * sinus + asg;
            p_vert->p_spec->b = p_vert->p_spec_mat->b - sb * sinus + asb;
          }
        }

      }
    }
    p_vodnik = p_vodnik->p_next;
  }
}

void vod_updatuj(VODA2 * p_voda)
{
  VODOVERTEX *p_vert;
  int i, m;

  while (p_voda) {              // reset vody
    for (i = 0; i < p_voda->vertexu; i++) {
      p_vert = p_voda->p_vertexy + i;
      p_vert->p_pos->y = p_vert->vyska;
      p_vert->sinus = 0.0f;
      *p_vert->p_diff = *p_vert->p_diff_mat;
      if (p_vert->p_spec)
        *p_vert->p_spec = *p_vert->p_spec_mat;
    }
    vod_updatuj_vodniky(p_voda->p_vodnik);
    for (m = 0; m < p_voda->meshnum; m++) {
      p_voda->p_mesh[m]->p_data->k2flag |=
        KONT2_UPDATE_DIFF | KONT2_UPDATE_SPEC;
    }
    p_voda = p_voda->p_next;
  }
}
