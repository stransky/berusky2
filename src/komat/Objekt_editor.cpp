/*  
  Objekt editor + Scene editor
*/
#include "mss_on.h"
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <direct.h>
#include <float.h>
#include <assert.h>
#include <time.h>
#include "objekt_editor_all.h"
#include "..\\kofola\\editor levelu.h"

#include "dbgwnd.h"

K_EDITOR cnf, *p_cnf = &cnf;

char *ftoa(float cislo, char *p_string, int des)
{
  byte tmp[50];

  if (des) {
    sprintf(tmp, "%%.%df", des);
    sprintf(p_string, tmp, cislo);
  }
  else {
    sprintf(p_string, "%.3f", cislo);
  }

  return (p_string);
}

/* Prepnuti mezi pohledem a kamerou
*/
void oe_kamera_to_view(K_EDITOR * p_cnf, int k)
{
  static int mod_pohledu = 0;   // puvodne je pohled 0 -> view
  static int mod_pohledu_old = MOD_GAME;        // stary pohled

  if (k == K_CHYBA) {
    if (mod_pohledu == MOD_KAMERA) {
      mod_pohledu = mod_pohledu_old;
      oe_nahod_mod(p_cnf, mod_pohledu);
    }
  }
  else {
    if (mod_pohledu != MOD_KAMERA) {
      mod_pohledu_old = oe_cti_mod(p_cnf);
      mod_pohledu = MOD_KAMERA;
    }
    oe_nahod_mod(p_cnf, mod_pohledu);
    *p_cnf->p_invcam = p_cnf->kamery[k].p_objekt->m;

    invert_matrix_copy(p_cnf->p_invcam, p_cnf->p_camera);
    set_matrix_camera(p_cnf->p_camera);

    projection_matrix(p_cnf->p_project, p_cnf->kamery[k].fov,
      (float) OXRES / (float) OYRES, p_cnf->kam.near_plane,
      p_cnf->kam.far_plane);
    set_matrix_project(p_cnf->p_project);
  }
}

void oe_transformuj_scenu_staticke(K_EDITOR * p_cnf)
{
  EDIT_OBJEKT *p_obj;
  GLMATRIX *p_m;
  int i, v, o;

  for (i = 0; i < KONT_NUM; i++) {
    if (p_cnf->p_kont[i] && p_cnf->p_kont[i]->kflag & KONT_STATIC) {
      p_m = kont_world_matrix(p_cnf->p_kont[i]);
      for (o = 0; o < MAX_KONT_OBJEKTU; o++) {
        if ((p_obj = p_cnf->p_kont[i]->p_obj[o])) {
          for (v = 0; v < p_obj->vertexnum; v++) {
            transformuj_bod_matici(&p_obj->p_vertex[v].x,
              &p_obj->p_vertex[v].y, &p_obj->p_vertex[v].z, p_m);
          }
        }
      }
      init_matrix(&p_cnf->p_kont[i]->world);
      init_matrix(&p_cnf->p_kont[i]->mpmatrix);
    }
  }
}

/*
  0 = nemazat
  1 = smazat obe
  2 = smazat jednu
*/
__inline static int smazat_plosky(K_EDITOR * p_cnf, I_VERTEX * p_list,
  ROVINA * p_rov, int f1, int f2, byte spol_bodu)
{
  BOD face_1[3];
  BOD face_2[3], v;
  int k1, o1, f, k2, o2, pf1, pf2, pf3, i, j, h;
  float vel;

  /*
     Naplnim face 1
   */
  k1 = p_list[f1].k;
  o1 = p_list[f1].o;
  f = p_list[f1].v;
  pf1 = p_cnf->p_kont[k1]->p_obj[o1]->p_face[f];
  pf2 = p_cnf->p_kont[k1]->p_obj[o1]->p_face[f + 1];
  pf3 = p_cnf->p_kont[k1]->p_obj[o1]->p_face[f + 2];
  mujbod2bod(face_1, p_cnf->p_kont[k1]->p_obj[o1]->p_vertex + pf1);
  mujbod2bod(face_1 + 1, p_cnf->p_kont[k1]->p_obj[o1]->p_vertex + pf2);
  mujbod2bod(face_1 + 2, p_cnf->p_kont[k1]->p_obj[o1]->p_vertex + pf3);

  /*
     Naplnim face 2
   */
  k2 = p_list[f2].k;
  o2 = p_list[f2].o;
  f = p_list[f2].v;
  pf1 = p_cnf->p_kont[k2]->p_obj[o2]->p_face[f];
  pf2 = p_cnf->p_kont[k2]->p_obj[o2]->p_face[f + 1];
  pf3 = p_cnf->p_kont[k2]->p_obj[o2]->p_face[f + 2];
  mujbod2bod(face_2, p_cnf->p_kont[k2]->p_obj[o2]->p_vertex + pf1);
  mujbod2bod(face_2 + 1, p_cnf->p_kont[k2]->p_obj[o2]->p_vertex + pf2);
  mujbod2bod(face_2 + 2, p_cnf->p_kont[k2]->p_obj[o2]->p_vertex + pf3);

  /*
     Porovnam jejich body
   */
  //#define stejny_vertex(v1,v2) (((v1.x) == (v2.x)) && ((v1.y) == (v2.y)) &&((v1.z) == (v2.z)))

  h = 0;
  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      if (stejny_vertex(face_1[i], face_2[j]))
        h++;
    }
  }
  assert(h < 4);

  if (h < spol_bodu)
    return (FALSE);

  v.x = p_rov[f1].x + p_rov[f2].x;
  v.y = p_rov[f1].y + p_rov[f2].y;
  v.z = p_rov[f1].z + p_rov[f2].z;
  if ((vel = vektor_velikost(&v)) > 0.0f) {
    if (vel == 2.0f && (k1 != k2 && o1 != o2))  // je to billboard ?
      return (2);
    else
      return (0);
  }
  else
    return (1);
}

typedef struct _OPTIMAL_PARAM
{

  BOD dolu;
  float uhel;
  byte spol_bodu;

} OPTIMAL_PARAM;

/*
Optimalizce sceny podle stredu sceny:
Vyhledavani spolecnych facu:
  - norm vektory proti sobe (kolme)
  - stredy u sebe - takovy 2 face znicim !!!!
  - Face co ma sirku 0 (2 body face maji stejne souradnice)
  - Testy na ty degenerovany facy !
  - Face co jsou dolu -> poslat do haje
*/
void oe_optimalizuj_scenu(K_EDITOR * p_cnf, OPTIMAL_PARAM * p_opt)
{
  int i, f, o, k, f1, f2;
  I_VERTEX *p_list;
  ROVINA *p_rov;
  BOD b, *p_dolu = &p_opt->dolu;
  int facu = 0;
  float uh;


  norm_vect(&p_dolu->x, &p_dolu->y, &p_dolu->z);

  p_list = mmalloc(sizeof(I_VERTEX) * p_cnf->stat.facu);
  p_rov = mmalloc(sizeof(ROVINA) * p_cnf->stat.facu);

  /*
     Smazani vsech spodnich facu
   */
  for (k = 0; k < p_cnf->max_kont; k++) {
    // optimalizace pouze statickych objektu
    if (p_cnf->p_kont[k] && p_cnf->p_kont[k]->kflag & KONT_STATIC) {
      for (o = 0; o < MAX_KONT_OBJEKTU; o++) {
        if (p_cnf->p_kont[k]->p_obj[o]) {
          for (f = p_cnf->p_kont[k]->p_obj[o]->facenum - 1; f > 0; f -= 3) {
            p_list[facu].k = k;
            p_list[facu].o = o;
            p_list[facu].v = f - 3;
            calc_rovinu(p_cnf->p_kont[k]->p_obj[o]->p_vertex + (f - 2),
              p_cnf->p_kont[k]->p_obj[o]->p_vertex + (f - 1),
              p_cnf->p_kont[k]->p_obj[o]->p_vertex + f, p_rov + facu);
            norm_vect(&p_rov[facu].x, &p_rov[facu].y, &p_rov[facu].z);
            facu++;
          }
        }
      }
    }
  }

  // odmazat spolecne facy
  for (f1 = 0; f1 < facu; f1++) {
    for (f2 = 0; f2 < facu; f2++) {
      if (f1 == f2)
        continue;
      i = smazat_plosky(p_cnf, p_list, p_rov, f1, f2, p_opt->spol_bodu);
      if (i == 2) {             // smazat pouze jednu
        if (p_rov[f1].e != 0.0f && p_rov[f2].e != 0.0f)
          p_rov[f1].e = 0.0f;
      }
      if (i == 1) {             // smazat
        p_rov[f1].e = 0.0f;
        p_rov[f2].e = 0.0f;
      }
      if (!i) {
        p_rov[f1].e = 1.0f;
        p_rov[f2].e = 1.0f;
      }
    }
  }


  // facu je pocet spracovavanych plosek
  for (i = 0; i < facu; i++) {
    if (p_rov[i].e == 1.0f) {
      b.x = p_rov[i].x;
      b.y = p_rov[i].y;
      b.z = p_rov[i].z;
      uh = vektor_dot_product(&b, p_dolu);
      p_rov[i].e = (acos(uh) < p_opt->uhel) ? 0.0f : 1.0f;      // 0 - smazat, 1 - nechat
    }
  }

  /*
     Vymaz plosek
     for(i = 0; i < facu; i++) {
     if(p_rov[i].e != 1.0f) {
     k = p_list[i].k;
     o = p_list[i].o;
     f = p_list[i].v;
     p_face = p_cnf->p_kont[k]->p_obj[o]->p_face;
     smaz_face_bez_vertexu(p_face,p_cnf->p_kont[k]->p_obj[o]->facenum,f);
     }
     }
   */

  free(p_list);
}

/*
  Zalohuje scenu do p_cnf->kont_zaloha
*/
void oe_zalohuj_scenu(K_EDITOR * p_cnf)
{
  kprintf(TRUE, "Zalohuji scenu...");
  oe_oznac_pouzite_materialy(p_cnf);
  oe_uloz_projekt(p_cnf, "ray_zaloha.bpr", p_cnf->dir.data_dir, TRUE, FALSE);
}

/*
  Nahraje scenu zpet ze zalohy
*/
void oe_zalohuj_scenu_zpet(K_EDITOR * p_cnf)
{
  kprintf(TRUE, "Obnovuji scenu...");
  oe_smaz_scenu(p_cnf);
  oe_nahraj_projekt(p_cnf, "ray_zaloha.bpr", p_cnf->dir.data_dir, FALSE,
    TRUE);
}

ZDRCADLO_DESC_POLY *oe_najdi_podzrcadlo(ZDRCADLO_DESC_POLY * p_first,
  int kont_id)
{
  while (p_first) {
    if (p_first->id_kont == kont_id)
      return (p_first);
    p_first = p_first->p_next;
  }
  return (NULL);
}

/*
  Prevede statickou scenu do polylistu
*/
int oe_scena_to_poly(K_EDITOR * p_cnf, EDIT_MESH_POLY ** p_list)
{
  ZDRCADLO_DESC_POLY *p_zrc;
  EDIT_MESH_POLY *p_poly;
  int i, p, t;
  int kflag;

  /*
     Oflagovani materialu
   */
  kprintf(TRUE, "Vyber materialu...");
  oe_oznac_pouzite_materialy(p_cnf);

  kprintf(TRUE, "Pocitani kontejneru...");

  /*
     Spocitam pouzitelne kontejnery
   */
  for (i = 0, p = 0; i < p_cnf->max_kont; i++) {
    if (p_cnf->p_kont[i] && p_cnf->p_kont[i]->kflag & KONT_STATIC
      && !(p_cnf->p_kont[i]->kflag & KONT_VERTEX_LIGHT)) {
      p++;
    }
  }

  if (!p) {
    kprintf(TRUE, "Scena neobsahuje poly...");
    return (0);
  }

  kprintf(TRUE, "Vyrabim polylist...");

  /*
     Vyrobil polylist
   */
  if (!(p_poly = (EDIT_MESH_POLY *) mmalloc(sizeof(p_poly[0]) * p))) {
    chyba("Pamet");
    assert(0);
  }

  /*
     Nakopiruju tam prvky
   */
  for (i = 0, t = 0; i < p_cnf->max_kont; i++) {
    if (p_cnf->p_kont[i]) {
      kflag = p_cnf->p_kont[i]->kflag;
      if (kflag & KONT_PRVEK && kflag & KONT_STATIC
        && !(kflag & KONT_VERTEX_LIGHT)) {

        edit_to_poly_indir(p_cnf->p_kont[i], p_poly + t, t);

        p_poly[t].poly = t;     // vloz cislo poly
        p_poly[t].kont = i;     // vloz cislo kontejneru

        zrus_kontejner_rec(p_cnf->p_kont + i, NULL);
        p_cnf->p_kont[i] = NULL;
        t++;
        assert(t <= p);
      }
    }
  }

  /*
     Nakopiruju tam scenu
   */
  for (i = 0; i < p_cnf->max_kont; i++) {
    if (p_cnf->p_kont[i]) {
      kflag = p_cnf->p_kont[i]->kflag;
      if (!(kflag & KONT_PRVEK) && kflag & KONT_STATIC
        && !(kflag & KONT_VERTEX_LIGHT)) {

        edit_to_poly_indir(p_cnf->p_kont[i], p_poly + t, t);

        p_poly[t].poly = t;     // vloz cislo poly
        p_poly[t].kont = i;     // vloz cislo kontejneru

        if (p_cnf->p_kont[i]->kflag & KONT_ZRCADLO) {   // oflaguj zrcadlo
          p_zrc =
            oe_najdi_podzrcadlo(p_cnf->zrcadlo.p_poly,
            p_cnf->p_kont[i]->kontejner_ID);
          if (p_zrc) {
            p_zrc->poly = t;
          }
        }

        zrus_kontejner_rec(p_cnf->p_kont + i, NULL);
        p_cnf->p_kont[i] = NULL;
        t++;
        assert(t <= p);
      }
    }
  }

  p_cnf->p_poly = p_poly;

  return (t);
}

int oe_scena_to_raykont(K_EDITOR * p_cnf, EDIT_KONTEJNER ** p_ray, int max)
{
  int i, p;

  /*
     Spocitam pouzitelne kontejnery
   */
  for (i = 0, p = 0; i < p_cnf->max_kont; i++) {
    if (p_cnf->p_kont[i] &&
      p_cnf->p_kont[i]->kflag & KONT_STATIC &&
      p_cnf->p_kont[i]->kflag & KONT_VERTEX_LIGHT &&
      !(p_cnf->p_kont[i]->kflag & KONT_NO_DRAW_SHADOW)) {
      p_ray[p] = p_cnf->p_kont[i];
      p++;
    }
  }
  return (p);
}

__inline void oe_scena_to_game_reset_mlhokostek(MLZNA_KOSTKA * p_top)
{
  while (p_top) {
    p_top->poly = K_CHYBA;
    p_top = p_top->p_next;
  }
}

/* Vola se pouze pri exportu do hry!!!
*/
void oe_obb_calc_scena(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  int k;

  kprintf(TRUE, "Calc-OBB-Scena %d...", p_cnf->cnf.precizni_export_obb);

  oe_klist_reset(&k);
  while ((p_kont = oe_klist_next(p_cnf, &k))) {
    kprintf(TRUE, "calc OBB %s", p_kont->jmeno);
    obb_calc_kont(p_kont, p_cnf->cnf.precizni_export_obb);
  }
}


/*
  Prevede scenu do herni sceny
*/
#define MIN_DIST        10.0f

void oe_scena_to_game(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  EDIT_MESH_POLY *p_list;
  int i;

  if (p_cnf->cnf.druh_sceny == SCENA_POLY) {
    kprintf(TRUE, "Scena uz je v poly!");
    return;
  }

  /* Eliminace spatnych plosek
   */
  kprintf(TRUE, "Eliminace degenerovanych plosek...");
  ray_smaz_blby_plosky(p_cnf);

  kprintf(TRUE, "Lepim dist. kontejnery...");

  /*
     Zaloha kontejneru
   */
  oe_zalohuj_scenu(p_cnf);

  /*
     Smazani neviditelnych zdi
   */
  kprintf(TRUE, "Smazani neviditelnych zdi...");
  be_zrus_level_nevid_zdi(&p_cnf->bnf);

  /*
     Kopie staticity
   */
  kprintf(TRUE, "Kopie staticity...");
  oe_rozmnoz_staticke(p_cnf);

  /* Prevedu vsechny svetla statickych kontejneru do world-space
     a odriznu od nich jejich svetla
   */
  kprintf(TRUE, "Premapovavam svetla statickych kontejneru...");
  for (i = 0; i < p_cnf->max_kont; i++) {
    if (p_cnf->p_kont[i]) {
      lo_premapuj_svetla_do_wordspace(p_cnf->p_kont[i]);
    }
  }

  /*
     Slepim kontejnery podle materialu a jejich vzdalenosti
     + premapuju jejich svetla
   */
  kprintf(TRUE, "Dist. lepeni statickych kontejneru...");
  for (i = 0; i < p_cnf->max_kont; i++) {
    p_kont = p_cnf->p_kont[i];
    if (p_kont) {
      vycisti_kontejner(p_kont);
      updatuj_kontejner_statistika(p_kont, TRUE);

      if (!(p_kont->kflag & KONT_KEYFRAME)) {
        slep_kontejner_dist(p_kont, MIN_DIST);
      }
    }
  }

  /*
     Transformuju staticke kontejnery ve scene
   */
  kprintf(TRUE, "Transformace stat. kontejneru ve scene...");
  oe_transformuj_scenu_staticke(p_cnf);

  /*
     Vsechny staticke kontejnery rozlozi, nulove smaze
   */
  kprintf(TRUE, "Rozkladam scenu...");
  for (i = 0; i < p_cnf->max_kont; i++) {
    if (p_cnf->p_kont[i]) {
      if (!p_cnf->p_kont[i]->objektu) {
        zrus_kontejner_rec(p_cnf->p_kont + i, &p_cnf->p_mlha);
      }
      else {
        if (p_cnf->p_kont[i]->kflag & KONT_STATIC) {
          if (ds_kontejner_rozloz(p_cnf, p_cnf->p_kont[i], ROZLOZ_MAX))
            zrus_kontejner_rec(p_cnf->p_kont + i, &p_cnf->p_mlha);
        }
      }
    }
  }

  /*
     Statistika sceny+
   */
  kprintf(TRUE, "Kont norm vect all");
  oe_prikaz_kont_norm_vect_all(p_cnf);

  /*
     Statistika sceny+
   */
  kprintf(TRUE, "Statistika sceny+");
  oe_updatuj_scenu_stat(p_cnf);

  /*
     Prevede tyto kontejnery do poly-listu
   */
  kprintf(TRUE, "Scena to poly...");
  oe_scena_to_game_reset_mlhokostek(p_cnf->p_mlha);
  p_cnf->polynum = oe_scena_to_poly(p_cnf, &p_list);
  p_cnf->kontraynum =
    oe_scena_to_raykont(p_cnf, p_cnf->p_kont_raytracer, KONT_NUM);

  /* Zbytku dopocitej obalku
   */
  oe_obb_calc_scena(p_cnf);

  /*
     Z poly-listu vyrobi kvadraticky strom
   */
  kprintf(TRUE, "KDTree...");
  kd_strom_vyrob(p_cnf->p_poly, p_cnf->polynum,
    p_cnf->p_kont_raytracer, p_cnf->kontraynum, &p_cnf->kdtree);

  p_cnf->cnf.druh_sceny = SCENA_POLY;

  kprintf(TRUE, "OK");
}


/* Prevede vsechny staticke objekty do word space
   
   Staticke objekty prevede do wordl-space 
   Prevede na polylisty
   Dynamicke necha na pokoji
*/

void oe_poly_to_scena(K_EDITOR * p_cnf)
{
  int i;

  kprintf(TRUE, "Prevod zpet...");

  if (p_cnf->cnf.druh_sceny != SCENA_LEVEL) {

    if (p_cnf->polynum) {
      for (i = 0; i < p_cnf->polynum; i++)
        zrus_vnitrek_poly(p_cnf->p_poly + i);
      free(p_cnf->p_poly);
      p_cnf->polynum = 0;
    }

    oe_zalohuj_scenu_zpet(p_cnf);

    for (i = 0; i < MAX_RAY_TEXTUR; i++) {
      if (p_cnf->p_lightmap[i].load) {
        txt_zrus_texturu(p_cnf->p_lightmap + i);
      }
    }
    kprintf(TRUE, "OK");
    p_cnf->cnf.druh_sceny = SCENA_LEVEL;
  }
}

void oe_updatuj_scenu_stat(K_EDITOR * p_cnf)
{
  int k;

  memset(&p_cnf->stat, 0, sizeof(p_cnf->stat));
  for (k = 0; k < KONT_NUM; k++) {
    if (p_cnf->p_kont[k]) {
      updatuj_kontejner_statistika(p_cnf->p_kont[k], TRUE);
      p_cnf->stat.kontejneru++;
      p_cnf->stat.objektu += p_cnf->p_kont[k]->objektu;
      p_cnf->stat.facu += p_cnf->p_kont[k]->facu;
      p_cnf->stat.bodu += p_cnf->p_kont[k]->bodu;
      if (p_cnf->p_kont[k]->kflag & KONT_STATIC
        && !(p_cnf->p_kont[k]->kflag & KONT_VERTEX_LIGHT)) {
        p_cnf->stat.kont_stat++;
        p_cnf->stat.polyface += p_cnf->p_kont[k]->facu;
      }
      else
        p_cnf->stat.kont_dyn++;
    }
  }
  for (k = 0; k < MAX_EDIT_TEXTUR; k++) {
    if (p_cnf->p_text[k].jmeno[0])
      p_cnf->stat.textur++;
  }
  for (k = 0; k < MAX_EDIT_MATERIALU; k++) {
    if (p_cnf->p_mat[k])
      p_cnf->stat.materialu++;
  }
  p_cnf->stat.svetel = MAX_EDIT_SVETEL;
}

void oe_updatuj_statistiku_sceny(K_EDITOR * p_cnf, SCENE_STATISTIC * p_stat)
{
  int i;

  memset(p_stat, 0, sizeof(SCENE_STATISTIC));
/*
  memset(&caps,0,sizeof(caps));
  caps.dwCaps = DDSCAPS_TEXTURE|DDSCAPS_VIDEOMEMORY|DDSCAPS_LOCALVIDMEM;
  IDirectDraw7_GetAvailableVidMem(p_dd,&caps,&p_stat->totmem,&p_stat->freemem);
*/
  /*
     // prosvistim textury
     for(i = 0; i < MAX_EDIT_TEXTUR; i++) {
     if(p_cnf->p_text[i].p_text) {
     memset(&ddsd,0,sizeof(ddsd));
     ddsd.dwSize = sizeof(ddsd);
     //glGetTexLevelParameterfv
     if((hr = IDirectDrawSurface7_GetSurfaceDesc(p_cnf->p_text[i].p_text,&ddsd)) != DD_OK) {
     rozeber_chybu(hr);
     ddw("FUCK!!!");
     }
     p_stat->texnum++;
     p_stat->textsize += ddsd.dwHeight*ddsd.dwWidth*ddsd.ddpfPixelFormat.dwRGBBitCount/8;
     }
     }

     // prosvistim lightmapy
     for(i = 0; i < MAX_RAY_TEXTUR; i++) {
     if(p_cnf->p_lightmap[i].p_text) {
     memset(&ddsd,0,sizeof(ddsd));
     ddsd.dwSize = sizeof(ddsd);
     if((hr = IDirectDrawSurface7_GetSurfaceDesc(p_cnf->p_lightmap[i].p_text,&ddsd)) != DD_OK) {
     rozeber_chybu(hr);
     ddw("FUCK!!!");
     }
     p_stat->lightnum++;
     p_stat->lightsize += ddsd.dwHeight*ddsd.dwWidth*ddsd.ddpfPixelFormat.dwRGBBitCount/8;
     }
     }
   */
  // prosvistim objekty
  for (i = 0; i < p_cnf->polynum; i++) {
    p_stat->polyface += p_cnf->p_poly[i].facenum / 3;
  }
  p_stat->polynum = p_cnf->polynum;


  // prosvistim objekty
  for (i = 0; i < p_cnf->max_kont; i++) {
    if (p_cnf->p_kont[i]) {
      p_stat->kontnum++;
      p_stat->facu += p_cnf->p_kont[i]->facu;
      p_stat->bodu += p_cnf->p_kont[i]->bodu;
      cti_velikost_kontejneru(p_cnf->p_kont[i], &p_stat->kontsize,
        &p_stat->objnum);
    }
  }
  p_stat->facu /= 3;

  // prosvistim objekty
  for (i = 0; i < MAX_BERUSKY_KONTEJNERU; i++) {
    if (p_cnf->bnf.p_kont[i]) {
      p_stat->lev_kontnum++;
      cti_velikost_kontejneru(p_cnf->bnf.p_kont[i], &p_stat->lev_kontsize,
        &p_stat->lev_objnum);
    }
  }

  // materialy
  for (i = 0; i < MAX_EDIT_MATERIALU; i++) {
    if (p_cnf->p_mat[i]) {
      p_stat->matnum++;
      p_stat->matsize += sizeof(*p_cnf->p_mat[0]);
    }
  }
}

void oe_cekej_na_klavesu(K_EDITOR * p_cnf)
{
  _a_klavesa = 0;
  while (spracuj_spravy(TRUE) && !_a_klavesa) {
    oe_renderuj_scenu_pre(p_cnf);
    oe_renderuj_scenu_po(p_cnf);
  }
}

void oe_cekej_na_mys(K_EDITOR * p_cnf)
{
  mi.t2 = 0;
  mi.t1 = 0;

  while (spracuj_spravy(TRUE) && !(mi.t1 || mi.t2 || key[K_SLASH])) {
    oe_renderuj_scenu_pre(p_cnf);
    oe_renderuj_scenu_po(p_cnf);
  }
}

// vraci vzdalenost kamery od stredu sceny nebo vybraneho objektu
float oe_stredni_vzdalenost_ve_scene(K_EDITOR * p_cnf)
{
  B_KONFIG *p_bnf = &p_cnf->bnf;
  BOD a, b, p;
  EDIT_KONTEJNER *p_kont;
  EDIT_MESH_POLY *p_poly;
  EDIT_OBJEKT *p_obj;
  OBJ_VERTEX *p_vert;
  GLMATRIX m, mt;
  word pocet;
  float x, y, z, w, p_x, p_y, p_z, vsechna_z, vzdal;
  int bodu, ty, tx;
  int k, ob, v, kolik = p_cnf->max_kont, kolik_ob, x_c, y_c;

  x_c = OXRES / 2;
  y_c = OYRES / 2;

  vsechna_z = 0;
  bodu = 0;

  if (akt_editor == GAME_EDITOR) {
    ROVINA r = { 0.0f, 1.0f, 0.0f,
      (float) (-p_bnf->grid.akt_patro * Y_PRVEK - p_bnf->y_start)
    };
    transf_2d_3d((float) OXRES / 2, (float) OYRES / 2, 0, &a.x, &a.y, &a.z,
      p_cnf->p_camera, p_cnf->p_project, OXRES, OYRES, OXSTART, OYSTART);
    transf_2d_3d((float) OXRES / 2, (float) OYRES / 2, 1.0f, &b.x, &b.y, &b.z,
      p_cnf->p_camera, p_cnf->p_project, OXRES, OYRES, OXSTART, OYSTART);
    if (!calc_prusek_bod(&r, &a, &b, &p))
      return (10.0f);
    else {
      vzdal = vzdal_bodu_bod(&a, &p);
      if (vzdal > 100000.0f)
        vzdal = 10.0f;
      return (vzdal);
    }
  }
  else {
    for (k = 0; k < kolik; k++) {
      p_kont = p_cnf->p_kont[k];
      if (!p_kont || !p_kont->kreslit || p_kont->kflag & KONT_NO_CALC)
        continue;

      mat_mult_dir(kont_world_matrix(p_kont), p_cnf->p_camera, &m);
      mat_mult_dir(&m, p_cnf->p_project, &mt);

      kolik_ob = p_kont->max_objektu;
      for (ob = 0; ob < kolik_ob; ob++) {
        p_obj = p_kont->p_obj[ob];
        if (!p_obj || !p_obj->kreslit)
          continue;

        pocet = p_obj->vertexnum;
        p_vert = p_obj->p_vertex;

        for (v = 0; v < pocet; v++) {
          p_x = p_vert[v].x;
          p_y = p_vert[v].y;
          p_z = p_vert[v].z;

          x = mt._11 * (p_x) + mt._21 * (p_y) + mt._31 * (p_z) + mt._41;
          y = mt._12 * (p_x) + mt._22 * (p_y) + mt._32 * (p_z) + mt._42;
          z = mt._13 * (p_x) + mt._23 * (p_y) + mt._33 * (p_z) + mt._43;
          w = mt._14 * (p_x) + mt._24 * (p_y) + mt._34 * (p_z) + mt._44;
          tx = (dword) ((1.0f + (x / w)) * x_c);
          ty = (dword) ((1.0f - (y / w)) * y_c);

          if (tx > 0 && tx < OXRES && ty > 0 && ty < OYRES) {
            if (z / w > 0.0f && z / w < 1.0f) {
              vsechna_z += z;
              bodu++;
            }
          }
        }
      }
    }
    for (k = 0; k < p_cnf->polynum; k++) {
      p_poly = p_cnf->p_poly + k;
      for (v = 0; v < p_poly->facenum; v++) {
        x = p_poly->p_koord[v].x;
        y = p_poly->p_koord[v].y;
        z = p_poly->p_koord[v].z;

        tx = (dword) ((1.0f + (x / w)) * x_c);
        ty = (dword) ((1.0f - (y / w)) * y_c);

        if (tx > 0 && tx < OXRES && ty > 0 && ty < OYRES) {
          if (z / w > 0.0f && z / w < 1.0f) {
            vsechna_z += z;
            bodu++;
          }
        }
      }
    }
    if (bodu) {
      z = vsechna_z / (float) bodu;
      return (z > 1.0f ? z : 1.0f);
    }
    else
      return (10.0f);
  }
}

float oe_stredni_vzdalenost_ve_scene_vybr(K_EDITOR * p_cnf)
{
  B_KONFIG *p_bnf = &p_cnf->bnf;
  BOD a, b, p;
  EDIT_KONTEJNER *p_kont;
  EDIT_OBJEKT *p_obj;
  GLMATRIX m, mt;
  float x, y, z, w, p_x, p_y, p_z, vsechna_z, vzdal;
  int bodu, ty, tx;
  int v, x_c, y_c;
  int k = p_cnf->vybr_kont;
  int ob = p_cnf->vybr_objekt;

  // vybr kont/objekt -> stredni vzdal
  if (!p_cnf->p_kont[k] || !p_cnf->p_kont[k]->p_obj[ob]) {
    return (oe_stredni_vzdalenost_ve_scene(p_cnf));
  }

  x_c = OXRES / 2;
  y_c = OYRES / 2;

  vsechna_z = 0;
  bodu = 0;

  if (akt_editor == GAME_EDITOR) {
    ROVINA r = { 0.0f, 1.0f, 0.0f,
      (float) (-p_bnf->grid.akt_patro * Y_PRVEK - p_bnf->y_start)
    };
    transf_2d_3d((float) OXRES / 2, (float) OYRES / 2, 0, &a.x, &a.y, &a.z,
      p_cnf->p_camera, p_cnf->p_project, OXRES, OYRES, OXSTART, OYSTART);
    transf_2d_3d((float) OXRES / 2, (float) OYRES / 2, 1.0f, &b.x, &b.y, &b.z,
      p_cnf->p_camera, p_cnf->p_project, OXRES, OYRES, OXSTART, OYSTART);
    if (!calc_prusek_bod(&r, &a, &b, &p))
      return (10.0f);
    else {
      vzdal = vzdal_bodu_bod(&a, &p);
      if (vzdal > 1000.0f)
        vzdal = 10.0f;
      return (vzdal);
    }
  }
  else {
    p_kont = p_cnf->p_kont[k];
    if (!p_kont || !p_kont->kreslit || p_kont->kflag & KONT_NO_CALC)
      return (oe_stredni_vzdalenost_ve_scene(p_cnf));

    mat_mult_dir(kont_world_matrix(p_kont), p_cnf->p_camera, &m);
    mat_mult_dir(&m, p_cnf->p_project, &mt);

    p_obj = p_kont->p_obj[ob];
    if (!p_obj || !p_obj->kreslit)
      return (oe_stredni_vzdalenost_ve_scene(p_cnf));

    for (v = 0; v < p_obj->vertexnum; v++) {
      p_x = p_obj->p_vertex[v].x;
      p_y = p_obj->p_vertex[v].y;
      p_z = p_obj->p_vertex[v].z;

      x = mt._11 * (p_x) + mt._21 * (p_y) + mt._31 * (p_z) + mt._41;
      y = mt._12 * (p_x) + mt._22 * (p_y) + mt._32 * (p_z) + mt._42;
      z = mt._13 * (p_x) + mt._23 * (p_y) + mt._33 * (p_z) + mt._43;
      w = mt._14 * (p_x) + mt._24 * (p_y) + mt._34 * (p_z) + mt._44;
      tx = (dword) ((1.0f + (x / w)) * x_c);
      ty = (dword) ((1.0f - (y / w)) * y_c);

      if (tx > 0 && tx < OXRES && ty > 0 && ty < OYRES) {
        if (z / w > 0.0f && z / w < 1.0f) {
          vsechna_z += z;
          bodu++;
        }
      }
    }
    if (bodu) {
      z = vsechna_z / (float) bodu;
      return (z > 1.0f ? z : 1.0f);
    }
    else {
      return (10.0f);
    }
  }
}

// Hleda vzdalenost stredu obrazovky k nulove rovine
float oe_nulova_vzdalenost_ve_scene(K_EDITOR * p_cnf)
{
  return (10.0f);
}

// vraci vzdalenost k kamery vybranemu objektu ve scene
float oe_objektova_vzdalenost_ve_scene(K_EDITOR * p_cnf)
{
  return (10.0f);
}

void oe_objekty_nacucni_z_materialu(K_EDITOR * p_cnf)
{
  int k, o, m;

  for (k = 0; k < p_cnf->max_kont; k++) {
    if (p_cnf->p_kont[k]) {
      for (o = 0; o < MAX_KONT_OBJEKTU; o++) {
        if (p_cnf->p_kont[k]->p_obj[o]) {
          m = p_cnf->p_kont[k]->p_obj[o]->material;
          if (p_cnf->p_mat[m]) {
            p_cnf->p_kont[k]->p_obj[o]->m1flag = p_cnf->p_mat[m]->flag;
            p_cnf->p_kont[k]->p_obj[o]->m2flag = p_cnf->p_mat[m]->flag2;
            p_cnf->p_kont[k]->m1flag |= p_cnf->p_mat[m]->flag;
            p_cnf->p_kont[k]->m2flag |= p_cnf->p_mat[m]->flag2;
          }
        }
      }
    }
  }
}

// vola se po material editoru -> rozdistribuje zmeny v materialech
// do flagu v kontejnerech
void oe_updatuj_scenu_flag(K_EDITOR * p_cnf)
{
  int k;

  for (k = 0; k < p_cnf->max_kont; k++) {
    if (p_cnf->p_kont[k])
      updatuj_kontejner_flag(p_cnf->p_kont[k], p_cnf->p_mat);
  }
}

void be_konfiguruj_berusky(B_KONFIG * p_bnf, K_EDITOR * p_cnf, byte * p_file);

__inline int oe_work_dir(byte * p_text)
{
  byte *p_pom;
  byte tmp[500];

  // nahradit work-dir retezcem
  if ((p_pom = strstr(_strlwr(p_text), "work_dir"))) {
    p_pom += 8;
    strcpy(tmp, p_text);
    strcpy(p_text, work_dir);
    strcat(p_text, tmp + (p_pom - p_text));
    return (TRUE);
  }
  return (FALSE);
}

void oe_konfiguruj_editor(K_EDITOR * p_cnf, byte * p_file, int renderovac)
{
  float barvy[4];
  byte file[100];
  FILE *f;
  HWND h2;
  int i;
  byte pom[100];

  kprintf(TRUE, "Konfiguruju editor...");

  h2 = p_cnf->kk.hwnd_dialog;
  f = p_cnf->bnf.debug_file;
  memset(p_cnf, 0, sizeof(K_EDITOR));
  p_cnf->bnf.debug_file = f;

  XK = 17;
  YK = (OYRES / VYSKA_FONTU) - KONZOLE_RADKU - 1;


  memset(p_cnf->user_color, 0xffffffff, sizeof(p_cnf->user_color[0]) * 16);

  p_cnf->groupflag |= (GROUP_KONT | GROUP_OBJ | GROUP_FACE);
  p_cnf->cnf.oznacovat = OZNACOVAT_MESH | OZNACOVAT_KOSTI | OZNACOVAT_SVETLA;
  p_cnf->cnf.load = LOAD_ALL;


  for (i = 0; i < MAX_EDIT_KAMER; i++) {
    p_cnf->kamery[i].cislo = K_CHYBA;
  }

  srand(timeGetTime());

  kof_Init_Level_Environment(&p_cnf->env);

  p_cnf->cnf.light_maps = TRUE;
  p_cnf->lc.scena_light = TRUE;

  p_cnf->cnf.delit_materialy = TRUE;
  p_cnf->kk.aktivni = TRUE;
  p_cnf->kk.hwnd_dialog = h2;
  p_cnf->kreslit_level = TRUE;
  p_cnf->kreslit_scenu = TRUE;
  p_cnf->cnf.keyframe_load = TRUE;
  p_cnf->cnf.keyframe_kont = FALSE;
  p_cnf->cnf.keyframe_static = TRUE;
  p_cnf->cnf.kamera_load = TRUE;
  p_cnf->cnf.kamera_kreslit = TRUE;
  p_cnf->cnf.kdtree_render = FALSE;
  p_cnf->cnf.kdtree_render_bunky = TRUE;
  p_cnf->cnf.render_zrcadla = TRUE;
  p_cnf->cnf.svetla_sceny = TRUE;
  p_cnf->cnf.norm_vect_velikost = 3.0f;
  GetPrivateProfileString("editor", "slicka", "0.001f", pom, 99, p_file);
  p_cnf->cnf.slicka_delta = (float) atof(pom);
  GetPrivateProfileString("editor", "k_far_plane", "1000000.0f", pom, 99,
    p_file);
  p_cnf->kam.far_plane = atoff(pom);
  p_cnf->kam.near_plane = 1.0f;
  p_cnf->kam.fov = DEG2RAD(30.0f);
  p_cnf->kam.min_uhel = DEG2RAD(20.0f);
  p_cnf->kam.max_uhel = DEG2RAD(85.0f);
  p_cnf->kam.min_vzdal = 10.0f;
  p_cnf->kam.max_vzdal = 50.0f;
  p_cnf->kam.min.x = -60.0f;
  p_cnf->kam.min.y = 0.0f;
  p_cnf->kam.min.z = -60.0f;
  p_cnf->kam.max.x = 60.0f;
  p_cnf->kam.max.y = 60.0f;
  p_cnf->kam.max.z = 60.0f;
  p_cnf->cnf.objekt_stred = TRUE;
  p_cnf->cnf.kamera_vkladat_do_klicu = TRUE;

  p_cnf->ds.max_err = FALSE;
  p_cnf->ds.max_err_flt = 0.7f;
  p_cnf->ds.vaha = TRUE;
  p_cnf->ds.vaha_flt = 0.5f;
  p_cnf->ds.hrany = TRUE;
  p_cnf->ds.hrany_flt = 1000;
  p_cnf->ds.prah = TRUE;
  p_cnf->ds.prah_flt = 0.01f;
  p_cnf->ds.stupen = 0.8f;
  p_cnf->ds.vertexu = 1;

  p_cnf->jgen.framu_min = 100;
  p_cnf->jgen.framu_max = 200;
  p_cnf->jgen.scale_0 = 1.0f;
  p_cnf->jgen.scale_1 = 0.7f;
  p_cnf->jgen.sila_vetru = 0.0f;

  p_cnf->jgen.udr_sila = 0.5f;
  p_cnf->jgen.udr_rozsah = 30.0f;
  p_cnf->jgen.udr_frame_min = 20;
  p_cnf->jgen.udr_frame_max = 50;

  p_cnf->jgen.ud_rozsah = 30.0f;
  p_cnf->jgen.ud_frame_min = 20;
  p_cnf->jgen.ud_frame_max = 50;

  p_cnf->jgen.vt_rozsah = 30.0f;
  p_cnf->jgen.vt_frame_min = 20;
  p_cnf->jgen.vt_frame_max = 50;
  p_cnf->jgen.vt_smer.x = -1;
  p_cnf->jgen.vt_smer.y = 0;
  p_cnf->jgen.vt_smer.z = 0;

  p_cnf->jgen.vtr_sila = 0.0f;
  p_cnf->jgen.vtr_rozsah = 30.0f;
  p_cnf->jgen.vtr_frame_min = 20;
  p_cnf->jgen.vtr_frame_max = 50;

  p_cnf->cnf.undo = GetPrivateProfileInt("editor", "undo", 1, p_file);

  p_cnf->cnf.posledni_prikaz = K_CHYBA;

  doe_zaskrkni_menu(p_cnf);

  p_cnf->drobic_hloubka = 1;
  if (p_cnf->kk.aktivni) {
    itoa(p_cnf->drobic_hloubka, pom, 10);
    SetDlgItemText(p_cnf->kk.hwnd_dialog, IDC_EDIT10, pom);
  }

  set_matrix_init();
  init_matrix(&p_cnf->init_matrix);

//#define FAKTOR  10.0f
//#define FAKTOR  20.0f
//#define AMBIENT 0x90909090

  p_cnf->ray.ambient = 0x0;
  p_cnf->ray.faktor = 1.0f;
  p_cnf->ray.stupen_filtrace = 3;
  p_cnf->ray.disk = TRUE;
  p_cnf->ray.stiny = TRUE;
  p_cnf->ray.max_x_surface = MAX_X_SURFACE;
  p_cnf->ray.max_y_surface = MAX_Y_SURFACE;
  p_cnf->ray.max_x_lepic = MAX_X_SURFACE;
  p_cnf->ray.max_y_lepic = MAX_Y_SURFACE;
  p_cnf->ray.disk_mezi = FALSE;
  p_cnf->ray.stinovani = 0;
  p_cnf->ray.supersampl = 0;
  p_cnf->ray.filtrace = 1;
  p_cnf->ray.lightmap_filtr = 0;
  strcpy(p_cnf->ray.filtr_func[0].jmeno, "ctverec 9");
  strcpy(p_cnf->ray.filtr_func[1].jmeno, "ctverec 25");
  strcpy(p_cnf->ray.filtr_func[2].jmeno, "krizem 9");
  strcpy(p_cnf->ray.filtr_func[3].jmeno, "krizem 25");
  strcpy(p_cnf->ray.filtr_func[4].jmeno, "medianem");
  strcpy(p_cnf->ray.filtr_func[5].jmeno, "rovne 25");
  p_cnf->ray.filtr_func[0].p_fnc = ray_surface_filtruj_ctverec_9;
  p_cnf->ray.filtr_func[1].p_fnc = ray_surface_filtruj_ctverec_25;
  p_cnf->ray.filtr_func[2].p_fnc = ray_surface_filtruj_krizem_9;
  p_cnf->ray.filtr_func[3].p_fnc = ray_surface_filtruj_krizem_25;
  p_cnf->ray.filtr_func[4].p_fnc = ray_surface_filtruj_medianem;
  p_cnf->ray.filtr_func[5].p_fnc = ray_surface_filtruj_rovne_25;

  p_cnf->ray.p_akt_filtr_func = p_cnf->ray.filtr_func;

  p_cnf->cnf.precizni_export_obb =
    GetPrivateProfileInt("editor", "precizni_export_obb", 0, p_file);
  p_cnf->cnf.mazat_materialy =
    GetPrivateProfileInt("editor", "mazat_materialy", 1, p_file);

  p_cnf->ray.ambient =
    GetPrivateProfileInt("ray-tracer", "ambient", p_cnf->ray.ambient, p_file);
  GetPrivateProfileString("ray-tracer", "faktor", "5.0", pom, 99, p_file);
  p_cnf->ray.faktor = (float) atof(pom);
  p_cnf->ray.stupen_filtrace =
    GetPrivateProfileInt("ray-tracer", "stupen_filtrace",
    p_cnf->ray.stupen_filtrace, p_file);
  p_cnf->ray.disk =
    GetPrivateProfileInt("ray-tracer", "disk", p_cnf->ray.disk, p_file);
  p_cnf->ray.stiny =
    GetPrivateProfileInt("ray-tracer", "stiny", p_cnf->ray.stiny, p_file);
  p_cnf->ray.max_x_surface =
    GetPrivateProfileInt("ray-tracer", "max_x_surface",
    p_cnf->ray.max_x_surface, p_file);
  p_cnf->ray.max_y_surface =
    GetPrivateProfileInt("ray-tracer", "max_y_surface",
    p_cnf->ray.max_y_surface, p_file);
  p_cnf->ray.max_x_lepic =
    GetPrivateProfileInt("ray-tracer", "max_x_lepic", p_cnf->ray.max_x_lepic,
    p_file);
  p_cnf->ray.max_y_lepic =
    GetPrivateProfileInt("ray-tracer", "max_x_lepic", p_cnf->ray.max_x_lepic,
    p_file);
  p_cnf->ray.disk_mezi =
    GetPrivateProfileInt("ray-tracer", "disk_mezi", p_cnf->ray.disk_mezi,
    p_file);
  p_cnf->ray.stinovani =
    GetPrivateProfileInt("ray-tracer", "stinovani", p_cnf->ray.stinovani,
    p_file);
  p_cnf->ray.supersampl =
    GetPrivateProfileInt("ray-tracer", "supersampl", p_cnf->ray.supersampl,
    p_file);
  p_cnf->ray.filtrace =
    GetPrivateProfileInt("ray-tracer", "filtrace", p_cnf->ray.filtrace,
    p_file);

  p_cnf->ray.sup_hloubka =
    GetPrivateProfileInt("ray-tracer", "sup_hloubka", 1, p_file);
  p_cnf->ray.sup_velikost_x =
    GetPrivateProfileInt("ray-tracer", "sup_velikost_x", 2, p_file);
  p_cnf->ray.sup_velikost_y =
    GetPrivateProfileInt("ray-tracer", "sup_velikost_y", 2, p_file);

  p_cnf->ray.okraj = GetPrivateProfileInt("ray-tracer", "okraj", 4, p_file);
  p_cnf->ray.text_min_x =
    GetPrivateProfileInt("ray-tracer", "text_min_x", 16, p_file);
  p_cnf->ray.text_min_y =
    GetPrivateProfileInt("ray-tracer", "text_min_y", 16, p_file);

  p_cnf->cnf.mlhokostku_kreslit =
    GetPrivateProfileInt("editor", "mlhokostku_kreslit", 0, p_file);
  p_cnf->cnf.mlhokostka_aktivni =
    GetPrivateProfileInt("editor", "mlhokostka_aktivni", 0, p_file);
  p_cnf->cnf.mlhokostka_testy =
    GetPrivateProfileInt("editor", "mlhokostka_testy", 0, p_file);

  p_cnf->cnf.editor_priorita =
    GetPrivateProfileInt("editor", "editor_priorita", 0, p_file);

  //--- filtrace pres vsechny souvislosti ---

  p_cnf->bar.gourand_cop = COLOR_SET;

  nastav_material_rgb(&p_cnf->barva_neaktivni, 0.7f, 0.7f, 0.7f);       // bila
  nastav_material_rgb(&p_cnf->barva_grid, 0.8f, 0.8f, 0.8f);
  nastav_material_rgb(&p_cnf->barva_vybr_kont, 1, 1, 1);        // zluta
  nastav_material_rgb(&p_cnf->barva_vybr_objekt, 73.0f / 255.0f, 36.0f / 255.0f, 223.0f / 255.0f);      // modra
  nastav_material_rgb(&p_cnf->barva_vybr_face, 1, 0, 0);        // cervena

  strcpy(p_cnf->jmeno_sceny, "no_name.sc");

  p_cnf->cnf.grid = TRUE;

  p_cnf->svetlost_sceny = 0xffffffff;
  p_cnf->barva_pozadi = 0xff909090;
  rgb_float(p_cnf->barva_pozadi, barvy);
  glClearColor(barvy[0], barvy[1], barvy[2], 1.0f);

  p_cnf->cnf.dratove = FALSE;
  p_cnf->cnf.textury = TRUE;
  p_cnf->cnf.nitkovy_kriz = TRUE;
  p_cnf->cnf.mys_on = TRUE;
  p_cnf->cnf.svetla_kreslit = TRUE;

  p_cnf->ctverec_barva[0] = 1;
  p_cnf->ctverec_barva[1] = 1;
  p_cnf->ctverec_barva[2] = 1;
  p_cnf->d_listu = 15;
  p_cnf->d_listu_kont = 5;
  p_cnf->max_kont = KONT_NUM;

  // prehod do adresare s ini souborem 
  GetPrivateProfileString("game", "prvky_dir", "work_dir",
    p_cnf->dir.prvky_dir, 99, p_file);
  if (oe_work_dir(p_cnf->dir.prvky_dir))
    kprintf(TRUE, "work dir \"%s\" = \"%s\"", "prvky_dir", work_dir);

  GetPrivateProfileString("game", "level_dir", "work_dir",
    p_cnf->dir.level_dir, 99, p_file);
  if (oe_work_dir(p_cnf->dir.level_dir))
    kprintf(TRUE, "work dir \"%s\" = \"%s\"", "level_dir", work_dir);
  strcpy(p_cnf->dir.edit_level_dir, p_cnf->dir.level_dir);

  GetPrivateProfileString("game", "out_dir", "work_dir", p_cnf->dir.out_dir,
    99, p_file);
  if (oe_work_dir(p_cnf->dir.out_dir))
    kprintf(TRUE, "work dir \"%s\" = \"%s\"", "out_dir", work_dir);

  // Load texture dir
  for (i = 0; i < TEXT_DIRS; i++) {
    sprintf(pom, "texture_dir%d", i);
    GetPrivateProfileString("game", pom, "", p_cnf->tdir.texture_dir[i], 99,
      p_file);
    if (p_cnf->tdir.texture_dir[i][0] == '.'
      && !p_cnf->tdir.texture_dir[i][1])
      p_cnf->tdir.texture_dir[i][0] = 0;
  }

  // Load texture file
  for (i = 0; i < TEXT_DIRS; i++) {
    sprintf(pom, "texture_file%d", i);
    GetPrivateProfileString("game", pom, "", p_cnf->tdir.texture_file[i], 99,
      p_file);
    if (p_cnf->tdir.texture_file[i][0] == '.'
      && !p_cnf->tdir.texture_file[i][1])
      p_cnf->tdir.texture_file[i][0] = 0;
  }

  GetPrivateProfileString("game", "material_dir", "work_dir",
    p_cnf->dir.material_dir, 99, p_file);
  if (oe_work_dir(p_cnf->dir.material_dir))
    kprintf(TRUE, "work dir \"%s\" = \"%s\"", "material_dir", work_dir);

  GetPrivateProfileString("game", "data_dir", "work_dir", p_cnf->dir.data_dir,
    99, p_file);
  if (oe_work_dir(p_cnf->dir.data_dir))
    kprintf(TRUE, "work dir \"%s\" = \"%s\"", "data_dir", work_dir);

  GetPrivateProfileString("game", "scene_dir", "work_dir",
    p_cnf->dir.scene_dir, 99, p_file);
  if (oe_work_dir(p_cnf->dir.scene_dir))
    kprintf(TRUE, "work dir \"%s\" = \"%s\"", "scene_dir", work_dir);

  GetPrivateProfileString("game", "game_level_dir", "work_dir",
    p_cnf->dir.game_level_dir, 99, p_file);
  if (oe_work_dir(p_cnf->dir.game_level_dir))
    kprintf(TRUE, "work dir \"%s\" = \"%s\"", "game_level_dir", work_dir);

  GetPrivateProfileString("game", "game_data_dir", "work_dir",
    p_cnf->dir.game_data_dir, 99, p_file);
  if (oe_work_dir(p_cnf->dir.game_data_dir))
    kprintf(TRUE, "work dir \"%s\" = \"%s\"", "game_data_dir", work_dir);

  kprintf(1, "Prvky '%s'", p_cnf->dir.prvky_dir);
  kprintf(1, "Edit Level '%s'", p_cnf->dir.edit_level_dir);
  kprintf(1, "Game Level '%s'", p_cnf->dir.game_level_dir);
  kprintf(1, "Out '%s'", p_cnf->dir.out_dir);
  kprintf(1, "Textury '%s'", p_cnf->tdir.texture_dir[0]);
  for (i = 1; i < TEXT_DIRS; i++) {
    if (p_cnf->tdir.texture_dir[i][0])
      kprintf(1, "Textury '%s'", p_cnf->tdir.texture_dir[i]);
  }
  if (p_cnf->tdir.texture_file[0][0])
    kprintf(1, "Textury file '%s'", p_cnf->tdir.texture_file[0]);
  for (i = 1; i < TEXT_DIRS; i++) {
    if (p_cnf->tdir.texture_file[i][0])
      kprintf(1, "Textury file '%s'", p_cnf->tdir.texture_file[i]);
  }
  kprintf(1, "Materialy '%s'", p_cnf->dir.material_dir);
  kprintf(1, "Data '%s'", p_cnf->dir.data_dir);
  kprintf(1, "Projekty '%s'", p_cnf->dir.scene_dir);

  p_cnf->bnf.default_x =
    GetPrivateProfileInt("editor", "default_x", 10, p_file);
  p_cnf->bnf.default_y =
    GetPrivateProfileInt("editor", "default_y", 3, p_file);
  p_cnf->bnf.default_z =
    GetPrivateProfileInt("editor", "default_z", 10, p_file);

  p_cnf->lp.aktivni =
    GetPrivateProfileInt("editor", "lista_prvku", 0, p_file);
  p_cnf->cnf.load_textur =
    GetPrivateProfileInt("editor", "load_textury", 1, p_file);
  hlasit_kolize =
    GetPrivateProfileInt("editor", "kolize_materialu", 0, p_file);
  GetPrivateProfileString("editor", "berusky_hra", "berusky.exe",
    p_cnf->berusky_hra, 199, p_file);
  GetPrivateProfileString("editor", "berusky_hra_dir", ".",
    p_cnf->berusky_hra_dir, 199, p_file);


  GetPrivateProfileString("editor", "editor", "1", pom, 19, p_file);
  akt_editor = atoi(pom);

  p_cnf->cnf.mazat_dummy =
    GetPrivateProfileInt("editor", "mazat_dummy", 1, p_file);

  p_cnf->cnf.obalky_kontejner =
    GetPrivateProfileInt("editor", "obalky_kontejner", 0, p_file);
  p_cnf->cnf.obalky_objekt =
    GetPrivateProfileInt("editor", "obalky_objekt", 1, p_file);

  p_cnf->drobic_hloubka =
    GetPrivateProfileInt("editor", "drobic_hloubka", 1, p_file);

  p_cnf->deka.aktivni =
    GetPrivateProfileInt("editor", "deka_aktivni", 0, p_file);
  GetPrivateProfileString("editor", "deka_sila", "1.0", pom, 99, p_file);
  p_cnf->deka.sila = (float) atof(pom);
  GetPrivateProfileString("editor", "deka_radius", "1.0", pom, 99, p_file);
  p_cnf->deka.radius = (float) atof(pom);

  light_ambient_d(p_cnf->svetlost_sceny);
  light_on_off(p_cnf->cnf.svetla);

  p_cnf->cul = FALSE;
  cull_nastav(p_cnf->cul);

  p_cnf->elm.spol_bodu_1 = 2;
  p_cnf->elm.spol_bodu_2 = 4;
  p_cnf->elm.flag = ELIM_STATIC | ELIM_PRVKY;
  p_cnf->elm.norm.y = 1.0f;
  p_cnf->elm.delta_vzdalenost = 0.001f;

#define DEF_VEL_TX_OKNA 300

/*
  Nastaveni gouranda
*/
  //p_cnf->bar.gourand = 0x00ffffff;
  p_cnf->bar.gr = p_cnf->bar.gg = p_cnf->bar.gb = 1.0f;
  p_cnf->bar.ga = 1.0f;

  p_cnf->bar.gourand_scale = 1.0f;
  p_cnf->bar.gourand_pridat1 = 0.0f;
  p_cnf->bar.gourand_pridat2 = 0.0f;

  p_cnf->bar.set_operace = COLOR_SET;
  p_cnf->bar.set_krok =
    GetPrivateProfileInt("editor", "krok_barvy", 1, p_file) * 0.001f;
  p_cnf->bar.oboustrany_stinovac =
    GetPrivateProfileInt("editor", "oboustrany_stinovac", 0, p_file);
  p_cnf->bar.multi_mesh =
    GetPrivateProfileInt("editor", "multi_mesh", 1, p_file);
  //printfs(p_con,0,0,"Edit-mod");

  nahod_state_bloky();

// Generovani dat
  for (i = 0; i < 360; i++) {
    p_cnf->sinus_table[i] = (float) sin((2 * PI * (float) i) / (float) 360.0f); // spocitam sinus od 0 do 360 stupnu
//   if(errno == EDOM || errno == ERANGE)
    //   p_cnf->sinus_table[i] = 0;
  }

/*
  Final-fight -> nahozeni modu
*/
  for (i = 0; i < CELKEM_MODU; i++) {
    oe_ini_mod(p_cnf, i, 0);
  }
  oe_nahod_mod(p_cnf, MOD_GAME);

/*
  Nahrani systemovych materialu
*/
  if (GetPrivateProfileInt("editor", "load_system_materialy", 0, p_file)) {
    kprintf(TRUE, "Nahravam systemovou sadu textur...");
    GetPrivateProfileString("editor", "system_materialy", "syssada.b2t", file,
      99, p_file);
    chdir(p_cnf->dir.material_dir);
    lo_nahraj_materialy(p_cnf->p_mat, MAX_EDIT_MATERIALU, p_cnf->p_text,
      MAX_EDIT_TEXTUR, file);
  }

/*
  Nahrani defaultnich materialu
*/
  if (GetPrivateProfileInt("editor", "load_default_materialy", 0, p_file)) {
    kprintf(TRUE, "Nahravam defaultni sadu textur...");
    GetPrivateProfileString("editor", "default_materialy", "defsada.b2t",
      file, 99, p_file);
    chdir(p_cnf->dir.material_dir);
    lo_nahraj_materialy(p_cnf->p_mat, MAX_EDIT_MATERIALU, p_cnf->p_text,
      MAX_EDIT_TEXTUR, file);
  }


  /* Nacteni konfigu berusek
   */
  p_cnf->cnf.load_prvku =
    GetPrivateProfileInt("editor", "load_prvky", 0, p_file);
  if (!renderovac && p_cnf->cnf.load_prvku) {
    kprintf(TRUE, "Nahravam herni prvky...");
    be_konfiguruj_berusky(&p_cnf->bnf, p_cnf, p_file);
  }
  else {
    kprintf(TRUE, "EDITOR BEZI POUZE JAKO MESH !!!!");
  }

  p_cnf->TimeEndLastFrame = p_cnf->TimeBase = timeGetTime();

  /* Povoleni indexu
   */
  text_init();
  text_off(GL_TEXTURE_1D);
  text_off(GL_TEXTURE_2D);

  oe_reload_textur(p_cnf);

  p_cnf->lp.prvakt = 3;
  p_cnf->lp.oken = p_cnf->wxres / LISTA_X;

  konfiguruj_look_up(p_cnf->p_looksqrt_linear, p_cnf->p_looksqrt_quadratic,
    p_cnf->p_looksqrt_linear_float, p_cnf->p_looksqrt_quadratic_float);

  text_sharp(-1.0f);

  kprintf(TRUE, "Init OK");

  p_cnf->cnf.render = TRUE;
}


void oe_uloz_konfiguraci(K_EDITOR * p_cnf, byte * p_dir, byte * p_file,
  byte dir)
{
  byte soubor[500];
  byte pom[100];

  if (p_dir[0]) {
    strcpy(soubor, p_dir);
    strcat(soubor, "\\");
  }
  else
    soubor[0] = 0;
  strcat(soubor, p_file);

/*
 if(dir) {
   WritePrivateProfileString("game","prvky_dir",p_cnf->dir.prvky_dir,soubor);
   WritePrivateProfileString("game","level_dir",p_cnf->dir.level_dir,soubor);
   WritePrivateProfileString("game","out_dir",p_cnf->dir.out_dir,soubor);
   WritePrivateProfileString("game","texture_dir",p_cnf->dir.texture_dir,soubor);
   WritePrivateProfileString("game","material_dir",p_cnf->dir.material_dir,soubor);
   WritePrivateProfileString("game","data_dir",p_cnf->dir.data_dir,soubor); 
   WritePrivateProfileString("game","scene_dir",p_cnf->dir.scene_dir,soubor); 
 }
*/

  itoa(p_cnf->cnf.mlhokostku_kreslit, pom, 10);
  WritePrivateProfileString(INI_SEKCE, "mlhokostku_kreslit", pom, soubor);
  itoa(p_cnf->cnf.mlhokostka_aktivni, pom, 10);
  WritePrivateProfileString(INI_SEKCE, "mlhokostka_aktivni", pom, soubor);
  itoa(p_cnf->cnf.mlhokostka_testy, pom, 10);
  WritePrivateProfileString(INI_SEKCE, "mlhokostka_testy", pom, soubor);

  itoa(p_cnf->bar.oboustrany_stinovac, pom, 10);
  WritePrivateProfileString(INI_SEKCE, "oboustrany_stinovac", pom, soubor);
  itoa(p_cnf->bar.multi_mesh, pom, 10);
  WritePrivateProfileString(INI_SEKCE, "multi_mesh", pom, soubor);

  itoa(p_cnf->bnf.default_x, pom, 10);
  WritePrivateProfileString(INI_SEKCE, "default_x", pom, soubor);
  itoa(p_cnf->bnf.default_y, pom, 10);
  WritePrivateProfileString(INI_SEKCE, "default_y", pom, soubor);
  itoa(p_cnf->bnf.default_z, pom, 10);
  WritePrivateProfileString(INI_SEKCE, "default_z", pom, soubor);

  itoa(p_cnf->lp.aktivni, pom, 10);
  WritePrivateProfileString(INI_SEKCE, "lista_prvku", pom, soubor);

  itoa(p_cnf->cnf.precizni_export_obb, pom, 10);
  WritePrivateProfileString(INI_SEKCE, "precizni_export_obb", pom, soubor);

  // Hloubka drobice
  itoa(p_cnf->drobic_hloubka, pom, 10);
  WritePrivateProfileString(INI_SEKCE, "drobic_hloubka", pom, soubor);

  // Dekovac
  itoa(p_cnf->deka.aktivni, pom, 10);
  WritePrivateProfileString(INI_SEKCE, "deka_aktivni", pom, soubor);
  ftoa(p_cnf->deka.radius, pom, 5);
  WritePrivateProfileString(INI_SEKCE, "deka_radius", pom, soubor);
  ftoa(p_cnf->deka.sila, pom, 5);
  WritePrivateProfileString(INI_SEKCE, "deka_sila", pom, soubor);

  // uloz akt editor
  itoa(akt_editor, pom, 10);
  WritePrivateProfileString(INI_SEKCE, "editor", pom, soubor);

  // uloz dalsi veci
  itoa(p_cnf->ray.ambient, pom, 10);
  WritePrivateProfileString("ray-tracer", "ambient", pom, soubor);
  sprintf(pom, "%.3f", p_cnf->ray.faktor);
  WritePrivateProfileString("ray-tracer", "faktor", pom, soubor);
  itoa(p_cnf->ray.stupen_filtrace, pom, 10);
  WritePrivateProfileString("ray-tracer", "stupen_filtrace", pom, soubor);
  itoa(p_cnf->ray.disk, pom, 10);
  WritePrivateProfileString("ray-tracer", "disk", pom, soubor);
  itoa(p_cnf->ray.stiny, pom, 10);
  WritePrivateProfileString("ray-tracer", "stiny", pom, soubor);
  itoa(p_cnf->ray.max_x_surface, pom, 10);
  WritePrivateProfileString("ray-tracer", "max_x_surface", pom, soubor);
  itoa(p_cnf->ray.max_y_surface, pom, 10);
  WritePrivateProfileString("ray-tracer", "max_y_surface", pom, soubor);
  itoa(p_cnf->ray.max_x_lepic, pom, 10);
  WritePrivateProfileString("ray-tracer", "max_x_lepic", pom, soubor);
  itoa(p_cnf->ray.max_y_lepic, pom, 10);
  WritePrivateProfileString("ray-tracer", "max_y_lepic", pom, soubor);
  itoa(p_cnf->ray.max_y_lepic, pom, 10);
  WritePrivateProfileString("ray-tracer", "max_y_lepic", pom, soubor);
  itoa(p_cnf->ray.disk_mezi, pom, 10);
  WritePrivateProfileString("ray-tracer", "disk_mezi", pom, soubor);
  itoa(p_cnf->ray.stinovani, pom, 10);
  WritePrivateProfileString("ray-tracer", "stinovani", pom, soubor);
  itoa(p_cnf->ray.supersampl, pom, 10);
  WritePrivateProfileString("ray-tracer", "supersampl", pom, soubor);
  itoa(p_cnf->ray.filtrace, pom, 10);
  WritePrivateProfileString("ray-tracer", "filtrace", pom, soubor);
  itoa(p_cnf->ray.sup_hloubka, pom, 10);
  WritePrivateProfileString("ray-tracer", "sup_hloubka", pom, soubor);
  itoa(p_cnf->ray.sup_velikost_x, pom, 10);
  WritePrivateProfileString("ray-tracer", "sup_velikost_x", pom, soubor);
  itoa(p_cnf->ray.sup_velikost_y, pom, 10);
  WritePrivateProfileString("ray-tracer", "sup_velikost_y", pom, soubor);

  itoa(p_cnf->ray.okraj, pom, 10);
  WritePrivateProfileString("ray-tracer", "okraj", pom, soubor);
  itoa(p_cnf->ray.text_min_x, pom, 10);
  WritePrivateProfileString("ray-tracer", "text_min_x", pom, soubor);
  itoa(p_cnf->ray.text_min_y, pom, 10);
  WritePrivateProfileString("ray-tracer", "text_min_y", pom, soubor);
}

void oe_ini_mod(K_EDITOR * p_cnf, int mod, int styl)
{
  int xres, yres;

  if (!styl) {
    init_matrix(&p_cnf->viewports[mod].word);
    init_matrix(&p_cnf->viewports[mod].camera);
    init_matrix(&p_cnf->viewports[mod].r_bod);
    doe_okno_cti(&p_cnf->viewports[mod].min_x, &p_cnf->viewports[mod].min_y,
      &p_cnf->viewports[mod].max_x, &p_cnf->viewports[mod].max_y);

    xres = p_cnf->viewports[mod].max_x - p_cnf->viewports[mod].min_x;
    yres = p_cnf->viewports[mod].max_y - p_cnf->viewports[mod].min_y;
    projection_matrix(&p_cnf->viewports[mod].project,
      p_cnf->kam.fov,
      (float) xres / (float) yres,
      p_cnf->kam.near_plane, p_cnf->kam.far_plane);

    switch (mod) {
      case MOD_XZ:             // z vrsku
        p_cnf->viewports[mod].vzdal = MIN_VZDAL_KAMERY;
        p_cnf->viewports[mod].fi = (float) PI / 2;
        break;

      case MOD_XY:             // z boku
        p_cnf->viewports[mod].vzdal = MIN_VZDAL_KAMERY;
        p_cnf->viewports[mod].fi = (float) PI / 2;      //0;
        break;

      case MOD_ZY:             //zepredu
        //rotate_matrix_y(&p_cnf->viewports[mod].r_bod,-(float)PI/2);
        p_cnf->viewports[mod].vzdal = MIN_VZDAL_KAMERY;
        p_cnf->viewports[mod].fi = (float) PI / 2;      //0;
        break;

      case MOD_GAME:
        p_cnf->viewports[mod].vzdal = MIN_VZDAL_KAMERY;
        p_cnf->viewports[mod].fi = (float) MIN_UHEL_KAMERY;
        break;

      default:
        break;
    }
    calc_camera(&p_cnf->viewports[mod].camera, &p_cnf->viewports[mod].r_bod,
      p_cnf->viewports[mod].vzdal, p_cnf->viewports[mod].fi);
    invert_matrix(&p_cnf->viewports[mod].camera,
      &p_cnf->viewports[mod].invcam);
  }
  else {
    int md = p_cnf->mod;
    GLMATRIX *p_m = &p_cnf->viewports[mod].camera;

    if (mod == MOD_GAME)
      return;

    p_m->_11 = 0;
    p_m->_21 = 0;
    p_m->_31 = 0;               // x
    p_m->_12 = 0;
    p_m->_22 = 0;
    p_m->_32 = 0;               // y 
    p_m->_13 = 0;
    p_m->_23 = 0;
    p_m->_33 = 0;               // z

    switch (mod) {
      case MOD_XZ:             // z vrsku
        p_m->_11 = 1;
        p_m->_32 = 1;
        p_m->_23 = -1;
        p_cnf->viewports[mod].fi = PI / 2;
        break;

      case MOD_XY:             // z boku
        p_m->_11 = 1;
        p_m->_22 = 1;
        p_m->_33 = 1;
        p_cnf->viewports[mod].fi = 0;
        break;

      case MOD_ZY:             //zepredu
        p_m->_31 = 1;
        p_m->_22 = 1;
        p_m->_13 = 1;
        p_cnf->viewports[mod].fi = 0;
        break;

      default:
        return;
    }
    calc_r_bod(&p_cnf->viewports[mod].camera, &p_cnf->viewports[mod].r_bod,
      p_cnf->viewports[mod].vzdal, p_cnf->viewports[mod].fi);
    invert_matrix(&p_cnf->viewports[mod].camera,
      &p_cnf->viewports[mod].invcam);
    p_cnf->viewports[mod].vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);
  }
}

void oe_nahod_mod(K_EDITOR * p_cnf, int mod)
{
  p_cnf->mod = mod;

  p_cnf->p_word = &p_cnf->viewports[mod].word;
  p_cnf->p_project = &p_cnf->viewports[mod].project;
  p_cnf->p_camera = &p_cnf->viewports[mod].camera;
  p_cnf->p_invcam = &p_cnf->viewports[mod].invcam;
  p_cnf->p_r_bod = &p_cnf->viewports[mod].r_bod;

  p_cnf->p_vzdal = &p_cnf->viewports[mod].vzdal;
  p_cnf->p_fi = &p_cnf->viewports[mod].fi;

  p_cnf->wx = p_cnf->viewports[mod].min_x;
  p_cnf->wy = p_cnf->viewports[mod].min_y;
  p_cnf->wxres = p_cnf->viewports[mod].max_x;
  p_cnf->wyres = p_cnf->viewports[mod].max_y;

  set_matrix_camera(p_cnf->p_camera);
  set_matrix_project(p_cnf->p_project);
  set_matrix_view(p_cnf->wx, p_cnf->wy, p_cnf->wxres, p_cnf->wyres);

  /*
     na konzoli nakresli schematickou znacku modu
   */

}

int oe_cti_mod(K_EDITOR * p_cnf)
{
  return (p_cnf->mod);
}

void oe_klavesy_v_game_modu(K_EDITOR * p_cnf)
{
#define MIN_ZOOM  1
#define MAX_ZOOM  10000

  int mod = p_cnf->mod;
  GLMATRIX m;
  static float zoom = 4;

  if (key[K_P]) {
    key[K_P] = FALSE;
    oe_prikaz_posun_sceny(p_cnf);
  }
  if (key[K_H] && !key[K_CTRL]) {
    key[K_H] = FALSE;
    oe_prikaz_posun_sceny_xz(p_cnf);
  }
  if (key[K_R]) {
    key[K_R] = FALSE;
    oe_prikaz_rotace_sceny(p_cnf, key[K_CTRL]);
  }
  if (key[K_ENTER]) {
    key[K_ENTER] = TRUE;
    oe_prikaz_narovnej_kameru(p_cnf);
  }

  if (!p_cnf->cnf.free_look) {
    if (!key[K_CTRL] && (key[K_PLUS] || key[K_MINUS])) {
      invert_matrix(p_cnf->p_camera, &m);

      if (key[K_SHFT_L]) {
        if (key[K_PLUS]) {
          key[K_PLUS] = FALSE;
          translate_matrix(&m, 0.0f, 0.0f, +10.0f);
        }
        if (key[K_MINUS]) {
          key[K_MINUS] = FALSE;
          translate_matrix(&m, 0.0f, 0.0f, -10.0f);
        }
      }
      else if (key[K_TAB]) {
        if (key[K_PLUS]) {
          key[K_PLUS] = FALSE;
          translate_matrix(&m, 0.0f, 0.0f, +0.1f);
        }
        if (key[K_MINUS]) {
          key[K_MINUS] = FALSE;
          translate_matrix(&m, 0.0f, 0.0f, -0.1f);
        }
      }
      else {
        if (key[K_PLUS]) {
          key[K_PLUS] = FALSE;
          translate_matrix(&m, 0.0f, 0.0f, +1.0f);
        }
        if (key[K_MINUS]) {
          key[K_MINUS] = FALSE;
          translate_matrix(&m, 0.0f, 0.0f, -1.0f);
        }
      }
      invert_matrix(&m, p_cnf->p_camera);
      calc_r_bod(&m, p_cnf->p_r_bod, *p_cnf->p_vzdal, *p_cnf->p_fi);
      *p_cnf->p_invcam = m;
      set_matrix_camera(p_cnf->p_camera);
    }
    else {
      if (key[K_PLUS]) {
        float pomer = (float) OXRES / (float) OYRES;

        key[K_PLUS] = FALSE;

        p_cnf->p_project->_11 *= 1.5f;
        p_cnf->p_project->_22 *= 1.5f;
        if (p_cnf->p_project->_11 > MAX_ZOOM) {
          p_cnf->p_project->_11 = MAX_ZOOM;
          p_cnf->p_project->_22 = MAX_ZOOM * pomer;
        }
        set_matrix_project(p_cnf->p_project);
        *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);
      }
      if (key[K_MINUS]) {
        float pomer = (float) OXRES / (float) OYRES;

        key[K_MINUS] = FALSE;
        p_cnf->p_project->_11 *= 0.5;
        p_cnf->p_project->_22 *= 0.5;
        if (p_cnf->p_project->_11 < MIN_ZOOM) {
          p_cnf->p_project->_11 = MIN_ZOOM;
          p_cnf->p_project->_22 = MIN_ZOOM * pomer;
        }
        set_matrix_project(p_cnf->p_project);
        *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);
      }
    }
  }
  else {
    invert_matrix(p_cnf->p_camera, &m);
    if (key[K_NAHORU]) {
      key[K_NAHORU] = FALSE;
      translate_matrix(&m, 0.0f, 0.0f, +1.0f);
    }
    if (key[K_DOLU]) {
      key[K_DOLU] = FALSE;
      translate_matrix(&m, 0.0f, 0.0f, -1.0f);
    }
    if (key[K_DOPRAVA]) {
      key[K_DOPRAVA] = FALSE;
      translate_matrix(&m, 1.0f, 0.0f, 0.0f);
    }
    if (key[K_DOLEVA]) {
      key[K_DOLEVA] = FALSE;
      translate_matrix(&m, -1.0f, 0.0f, 0.0f);
    }
    if (key[K_TAB] && mi.dx) {
      rotate_matrix_y(&m, mi.dx * 0.001f);
      mi.dx = 0;
    }
    if (key[K_TAB] && mi.dy) {
      rotate_matrix_x(&m, mi.dy * 0.001f);
      mi.dy = 0;
    }
    invert_matrix(&m, p_cnf->p_camera);
    calc_r_bod(&m, p_cnf->p_r_bod, *p_cnf->p_vzdal, *p_cnf->p_fi);
    *p_cnf->p_invcam = m;
    set_matrix_camera(p_cnf->p_camera);
  }
}

void oe_prikaz_scena_zoom(int koeficient)
{
  if (!key[K_CTRL]) {
    GLMATRIX m;

    invert_matrix(cnf.p_camera, &m);

    if (key[K_SHFT_L]) {
      translate_matrix(&m, 0.0f, 0.0f, koeficient * 10.0f);
    }
    else if (key[K_TAB]) {
      translate_matrix(&m, 0.0f, 0.0f, koeficient * 0.05f);
    }
    else {
      translate_matrix(&m, 0.0f, 0.0f, koeficient * 1.0f);
    }

    invert_matrix(&m, cnf.p_camera);
    calc_r_bod(&m, cnf.p_r_bod, *cnf.p_vzdal, *cnf.p_fi);
    *cnf.p_invcam = m;
    set_matrix_camera(cnf.p_camera);
  }
}

void oe_klavesy_zmena_modu(K_EDITOR * p_cnf)
{
  int i;

  for (i = K_1; i < K_1 + CELKEM_MODU; i++) {
    if (key[i] && !key[K_CTRL] && !key[K_SHIFT]) {
      oe_nahod_mod(p_cnf, i - K_1);
    }
    if (key[i] && key[K_CTRL] && !key[K_SHIFT]) {
      oe_ini_mod(p_cnf, i - K_1, 1);
      oe_nahod_mod(p_cnf, i - K_1);
    }
  }
}


void oe_klavesy_load_save(K_EDITOR * p_cnf)
{
  if (key[K_F3] && !key[K_CTRL]) {
    key[K_F3] = FALSE;
    prikaz(oe_prikaz_nahraj_kontejner(p_cnf, key[K_SHFT_L], TRUE));
    key[K_SHIFT] = FALSE;
  }
  if (key[K_F3] && key[K_CTRL]) {
    key[K_F3] = FALSE;
    prikaz(oe_prikaz_nahraj_kontejner(p_cnf, key[K_SHFT_L], TRUE));
    key[K_SHIFT] = FALSE;
  }

  if (key[K_F4] && !key[K_CTRL]) {
    key[K_F4] = FALSE;
    prikaz(oe_prikaz_nahraj_kontejner(p_cnf, LOAD_KONT_ASE, TRUE));
  }

  if (key[K_F2] && !key[K_CTRL]) {
    key[K_F2] = FALSE;
    prikaz(oe_prikaz_uloz_kontejner(p_cnf, TRUE));
  }
  if (key[K_F2] && key[K_CTRL]) {
    key[K_F2] = FALSE;
    key[K_CTRL] = FALSE;
    prikaz(oe_prikaz_uloz_kontejner_jako(p_cnf));
  }
  if (key[K_F7] && !key[K_CTRL]) {
    key[K_F7] = FALSE;
    prikaz(oe_prikaz_uloz_projekt(p_cnf));
  }
  if (key[K_F8] && !key[K_CTRL]) {
    key[K_F8] = FALSE;
    doe_proved_prikaz_menu(ID_SOUBOR_IMPORTOVATOKOLI, TRUE, FALSE);
  }
  if (key[K_F9] && !key[K_CTRL]) {
    key[K_F9] = FALSE;
    oe_prikaz_hrej_berusci_level(p_cnf);
  }
  if (key[K_E] && key[K_CTRL]) {
    key[K_E] = FALSE;
    kprintf(1, "Export levelu do hry...");
    oe_prikaz_uloz_berusci_level(p_cnf);
  }
}

void oe_prikaz_posun_sceny(K_EDITOR * p_cnf)
{
  GLMATRIX mj, m;
  VEKTOR v1, v2;


  init_matrix(&mj);
  *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);

  transf_2d_3d_z((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
    &v1.x, &v1.y, &v1.z,
    &mj, p_cnf->p_project,
    OXRES, OYRES, OXSTART, OYSTART,
    p_cnf->kam.near_plane, p_cnf->kam.far_plane);

  nuluj_mys();
  while (stisk_mys()) {
    invert_matrix(p_cnf->p_camera, &m);
    transf_2d_3d_z((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
      &v2.x, &v2.y, &v2.z,
      &mj, p_cnf->p_project,
      OXRES, OYRES, OXSTART, OYSTART,
      p_cnf->kam.near_plane, p_cnf->kam.far_plane);

    translate_matrix(&m, v1.x - v2.x, v1.y - v2.y, 0);

    invert_matrix(&m, p_cnf->p_camera);
    calc_r_bod(&m, p_cnf->p_r_bod, *p_cnf->p_vzdal, *p_cnf->p_fi);
    *p_cnf->p_invcam = m;
    set_matrix_camera(p_cnf->p_camera);

    nuluj_dif();


    oe_renderuj_scenu_pre(p_cnf);
    oe_renderuj_scenu_po(p_cnf);

    transf_2d_3d_z((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
      &v1.x, &v1.y, &v1.z,
      &mj, p_cnf->p_project,
      OXRES, OYRES, OXSTART, OYSTART,
      p_cnf->kam.near_plane, p_cnf->kam.far_plane);
  }
  *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);
  mi.t1 = FALSE;
}

void oe_prikaz_posun_sceny_xz(K_EDITOR * p_cnf)
{

  nuluj_mys();
  while (stisk_mys()) {
    translate_matrix(p_cnf->p_r_bod, -(float) mi.dx * 0.02f, 0,
      (float) mi.dy * 0.02f);
    calc_camera(p_cnf->p_camera, p_cnf->p_r_bod, *p_cnf->p_vzdal,
      *p_cnf->p_fi);
    invert_matrix(p_cnf->p_camera, p_cnf->p_invcam);

    set_matrix_camera(p_cnf->p_camera);

    nuluj_dif();
    oe_renderuj_scenu_pre(p_cnf);
    oe_renderuj_scenu_po(p_cnf);
  }
  *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);
}

void oe_prikaz_rotace_sceny(K_EDITOR * p_cnf, int mod)
{
  GLMATRIX m;


  nuluj_mys();

  if (!mod)
    *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene_vybr(p_cnf);
  else
    *p_cnf->p_vzdal = 0.0001f;

  invert_matrix(p_cnf->p_camera, &m);
  calc_r_bod(&m, p_cnf->p_r_bod, *p_cnf->p_vzdal, *p_cnf->p_fi);
  *p_cnf->p_invcam = m;

  while (stisk_mys()) {

    *p_cnf->p_fi += ((float) PI / 180) * (float) mi.dy * 0.5f;
    rotate_matrix_y(p_cnf->p_r_bod,
      -((float) PI / 180) * (float) mi.dx * 0.5f);
    calc_camera(p_cnf->p_camera, p_cnf->p_r_bod, *p_cnf->p_vzdal,
      *p_cnf->p_fi);
    invert_matrix(p_cnf->p_camera, p_cnf->p_invcam);

    set_matrix_camera(p_cnf->p_camera);

    nuluj_dif();

    oe_renderuj_scenu_pre(p_cnf);
    oe_renderuj_scenu_po(p_cnf);
  }
  *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);
  p_cnf->cnf.high_face_render_refresh = TRUE;
  mi.t1 = FALSE;
}

void oe_nastav_kameru(K_EDITOR * p_cnf, BOD * p_bod, float r, float fi,
  float vzdal)
{
  *p_cnf->p_vzdal = vzdal;
  *p_cnf->p_fi = fi;
  init_matrix(p_cnf->p_r_bod);
  translate_matrix(p_cnf->p_r_bod, p_bod->x, p_bod->y, p_bod->z);
  rotate_matrix_y(p_cnf->p_r_bod, r);
  calc_camera(p_cnf->p_camera, p_cnf->p_r_bod, *p_cnf->p_vzdal, *p_cnf->p_fi);
  invert_matrix(p_cnf->p_camera, p_cnf->p_invcam);
  set_matrix_camera(p_cnf->p_camera);
}


__inline static void zvyrazni_vektor(BOD * p_vec)
{
  // x je nejvetsi
  if (fabs(p_vec->x) > fabs(p_vec->y) && fabs(p_vec->x) > fabs(p_vec->z)) {
    p_vec->y = p_vec->z = 0.0f;
    p_vec->x = p_vec->x > 0.0f ? 1.0f : -1.0f;
  }
  else                          // y je nejvetsi
  if (fabs(p_vec->y) > fabs(p_vec->x) && fabs(p_vec->y) > fabs(p_vec->z)) {
    p_vec->x = p_vec->z = 0.0f;
    p_vec->y = p_vec->y > 0.0f ? 1.0f : -1.0f;
  }
  else                          // z je nejvetsi
  if (fabs(p_vec->z) > fabs(p_vec->y) && fabs(p_vec->z) > fabs(p_vec->x)) {
    p_vec->x = p_vec->y = 0.0f;
    p_vec->z = p_vec->z > 0.0f ? 1.0f : -1.0f;
  }
}

__inline static void zvyrazni_vektor_dir(float *p_x, float *p_y, float *p_z)
{
  // x je nejvetsi
  if (fabs(*p_x) > fabs(*p_y) && fabs(*p_x) > fabs(*p_z)) {
    *p_y = *p_z = 0.0f;
    *p_x = *p_x > 0.0f ? 1.0f : -1.0f;
  }
  else                          // y je nejvetsi
  if (fabs(*p_y) > fabs(*p_x) && fabs(*p_y) > fabs(*p_z)) {
    *p_x = *p_z = 0.0f;
    *p_y = *p_y > 0.0f ? 1.0f : -1.0f;
  }
  else                          // z je nejvetsi
  if (fabs(*p_z) > fabs(*p_y) && fabs(*p_z) > fabs(*p_x)) {
    *p_x = *p_y = 0.0f;
    *p_z = *p_z > 0.0f ? 1.0f : -1.0f;
  }
}

void oe_prikaz_narovnej_kameru(K_EDITOR * p_cnf)
{
  GLMATRIX m;

  invert_matrix(p_cnf->p_camera, &m);

  zvyrazni_vektor_dir(&m._11, &m._21, &m._31);  // x vektor
  zvyrazni_vektor_dir(&m._12, &m._22, &m._32);  // y vektor
  zvyrazni_vektor_dir(&m._13, &m._23, &m._33);  // z vektor

  *p_cnf->p_invcam = m;
  invert_matrix(&m, p_cnf->p_camera);

  *p_cnf->p_fi = (m._13 == 0.0f && m._33 == 0.0f) ? PI / 2 : 0.0f;

  *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);
  calc_r_bod(&m, p_cnf->p_r_bod, *p_cnf->p_vzdal, *p_cnf->p_fi);

  set_matrix_camera(p_cnf->p_camera);
}

void oe_prikaz_no_off_popisy_vertexu(K_EDITOR * p_cnf)
{
  p_cnf->group_param = !p_cnf->group_param;
}


void oe_vybrani_ctverce(K_EDITOR * p_cnf, dword barva)
{
  int v;

  nuluj_mys();
  p_cnf->x1 = mi.x;
  p_cnf->y1 = mi.y;
  p_cnf->ctverec = 1;
  rgb_float(barva, p_cnf->ctverec_barva);

  nuluj_mys();

  while (stisk_mys()) {
    p_cnf->x2 = mi.x;
    p_cnf->y2 = mi.y;
    oe_renderuj_scenu_pre(p_cnf);
    oe_renderuj_scenu_po(p_cnf);
  }

  if (p_cnf->x1 > p_cnf->x2) {
    v = p_cnf->x2;
    p_cnf->x2 = p_cnf->x1;
    p_cnf->x1 = v;
  }
  if (p_cnf->y1 > p_cnf->y2) {
    v = p_cnf->y2;
    p_cnf->y2 = p_cnf->y1;
    p_cnf->y1 = v;
  }

  p_cnf->ctverec = 0;
// p_cnf->nitkovy_kriz = 1;
}

void oe_posun_ctverce(K_EDITOR * p_cnf)
{
  int v;

  nuluj_mys();
  while (stisk_mys()) {
    p_cnf->x1 += mi.dx;
    p_cnf->x2 += mi.dx;
    p_cnf->y1 += mi.dy;
    p_cnf->y2 += mi.dy;
    nuluj_dif();
    oe_renderuj_scenu_pre(p_cnf);
    oe_renderuj_scenu_po(p_cnf);
  }

  if (p_cnf->x1 > p_cnf->x2) {
    v = p_cnf->x2;
    p_cnf->x2 = p_cnf->x1;
    p_cnf->x1 = v;
  }
  if (p_cnf->y1 > p_cnf->y2) {
    v = p_cnf->y2;
    p_cnf->y2 = p_cnf->y1;
    p_cnf->y1 = v;
  }
}

void oe_posun_strany_ctverce(K_EDITOR * p_cnf)
{
  int v;

  nuluj_mys();

  while (stisk_mys()) {
    p_cnf->x1 += mi.x;
    p_cnf->x2 += mi.x;
    p_cnf->y1 += mi.y;
    p_cnf->y2 += mi.y;
    oe_renderuj_scenu_pre(p_cnf);
    oe_renderuj_scenu_po(p_cnf);
  }

  if (p_cnf->x1 > p_cnf->x2) {
    v = p_cnf->x2;
    p_cnf->x2 = p_cnf->x1;
    p_cnf->x1 = v;
  }
  if (p_cnf->y1 > p_cnf->y2) {
    v = p_cnf->y2;
    p_cnf->y2 = p_cnf->y1;
    p_cnf->y1 = v;
  }
}

void oe_vybrani_konstantniho_ctverce(K_EDITOR * p_cnf)
{
  nuluj_mys();
  p_cnf->cnf.nitkovy_kriz = 0;

  while (stisk_mys()) {
    p_cnf->x1 = mi.x - 2 * VEL_CTVERCE;
    p_cnf->y1 = mi.y - 2 * VEL_CTVERCE;
    p_cnf->x2 = mi.x + 2 * VEL_CTVERCE;
    p_cnf->y2 = mi.y + 2 * VEL_CTVERCE;
    p_cnf->ctverec = 1;
    oe_renderuj_scenu_pre(p_cnf);
    oe_renderuj_scenu_po(p_cnf);
  }

  p_cnf->cnf.nitkovy_kriz = 1;
  p_cnf->ctverec = 0;
}


void oe_klavesy_editace_facu(K_EDITOR * p_cnf)
{
  if (key[K_M] && key[K_CTRL]) {
    key[K_M] = FALSE;
    oe_cekej_na_klavesu(p_cnf);

    if (key[K_M]) {
      key[K_M] = FALSE;
      doe_proved_prikaz_menu(ID_K_FACE_SMAZAT, TRUE, TRUE);
    }
    if (key[K_K]) {
      key[K_K] = FALSE;
      doe_proved_prikaz_menu(ID_K_OBJEKT_ROZDROB, TRUE, TRUE);
    }

    /* Editace kamer
     */
    if (key[K_P]) {
      key[K_P] = FALSE;
      doe_proved_prikaz_menu(ID_K_KAMERY_POSUNPOZICE, TRUE, TRUE);
    }
    if (key[K_T]) {
      key[K_T] = FALSE;
      doe_proved_prikaz_menu(ID_K_KAMERY_POSUNTARGET, TRUE, TRUE);
    }
    if (key[K_A]) {
      key[K_A] = FALSE;
      doe_proved_prikaz_menu(ID_K_KAMERA_POSUNPOZICETARGET, TRUE, TRUE);
    }
    if (key[K_R]) {
      key[K_R] = FALSE;
      doe_proved_prikaz_menu(ID_K_KAMERY_ROTACEROLL, TRUE, TRUE);
    }
    if (key[K_F]) {
      key[K_F] = FALSE;
      doe_proved_prikaz_menu(ID_K_KAMERY_VELIKOSTFOV, TRUE, TRUE);
    }

    /* Nova/Stara kamera
     */
    if (key[K_N]) {             // nova kamera
      key[K_N] = FALSE;
      doe_proved_prikaz_menu(ID_K_KAMERY_NOVAKAMERA, TRUE, TRUE);
    }
    if (key[K_U]) {             // upravit kameru
      key[K_U] = FALSE;
      doe_proved_prikaz_menu(ID_K_KAMERY_UPRAVITKAMERU, TRUE, TRUE);
    }
    if (key[K_S]) {             // smazat kameru
      key[K_S] = FALSE;
      doe_proved_prikaz_menu(ID_K_KAMERY_SMAZATKAMERU, TRUE, TRUE);
    }

    /* Animace
     */
    if (key[K_B]) {             // nova kamera
      key[K_B] = FALSE;
      doe_proved_prikaz_menu(ID_K_KAMERA_NOVAANIMACE, TRUE, TRUE);
    }
    if (key[K_V]) {             // upravit kameru
      key[K_V] = FALSE;
      doe_proved_prikaz_menu(ID_K_KAMERA_UPRAVITANIMACI, TRUE, TRUE);
    }
    if (key[K_C]) {             // smazat kameru
      key[K_C] = FALSE;
      doe_proved_prikaz_menu(ID_K_KAMERA_SMAZATANIMACI, TRUE, TRUE);
    }
  }
}

void oe_klavesy_editace_svetel(K_EDITOR * p_cnf)
{
  if (key[K_J] && key[K_CTRL]) {
    key[K_J] = FALSE;
    oe_cekej_na_klavesu(p_cnf);

    if (key[K_R] && !key[K_CTRL]) {
      key[K_R] = FALSE;
      ray_prepocitej_staticke_osvetleni(p_cnf, FALSE);
    }
    if (key[K_R] && key[K_CTRL]) {
      key[K_R] = FALSE;
      oe_poly_to_scena(p_cnf);
    }

    if (key[K_L]) {
      key[K_L] = FALSE;
      p_cnf->cnf.light_maps = !p_cnf->cnf.light_maps;
    }

/* ----
*/
    if (key[K_N]) {
      key[K_N] = FALSE;
      if (p_cnf->lc.hDlg) {
        if (p_cnf->lc.extra_svetlo)
          doe_proved_prikaz_menu(ID_K_EFEKTOVESVETLA_NOVE, TRUE, TRUE);
        else
          doe_proved_prikaz_menu(ID_K_DYNAMICKESVETLA_NOVE, TRUE, TRUE);
      }
      else {
        doe_proved_prikaz_menu(ID_K_SVETLA_NOVE, TRUE, TRUE);
      }
    }

    if (key[K_M]) {
      key[K_M] = FALSE;
      if (p_cnf->lc.hDlg) {
        if (p_cnf->lc.extra_svetlo)
          doe_proved_prikaz_menu(ID_K_EFEKTOVESVETLA_UPRAVIT, TRUE, TRUE);
        else
          doe_proved_prikaz_menu(ID_K_DYNAMICKESVETLA_UPRAVIT, TRUE, TRUE);
      }
      else {
        doe_proved_prikaz_menu(ID_K_SVETLA_UPRAVIT, TRUE, TRUE);
      }
    }

    if (key[K_P]) {
      key[K_P] = FALSE;
      if (p_cnf->lc.hDlg) {
        if (p_cnf->lc.extra_svetlo)
          doe_proved_prikaz_menu(key[K_CTRL] ? ID_K_EFEKTOVESVETLA_POSUNKOPIE
            : ID_K_EFEKTOVESVETLA_POSUN, TRUE, TRUE);
        else
          doe_proved_prikaz_menu(key[K_CTRL] ?
            ID_K_DYNAMICKESVETLA_POSUNOUTKOPII :
            ID_K_DYNAMICKESVETLA_POSUNOUT, TRUE, TRUE);
      }
      else {
        doe_proved_prikaz_menu(key[K_CTRL] ? ID_K_SVETLA_POSUNOUTKOPII :
          ID_K_SVETLA_POSUNOUT, TRUE, TRUE);
      }
    }
/* ----
*/
    if (key[K_S]) {
      key[K_S] = FALSE;         // scale
      oe_prikaz_scale_staticke_svetlo(p_cnf);
    }
    // p - posun
    // p + CTRL - posun + kopie
  }

  if (key[K_S] && key[K_CTRL]) {
    key[K_S] = FALSE;
    oe_cekej_na_klavesu(p_cnf);

    if (key[K_N]) {
      key[K_N] = FALSE;         // novy flare
      doe_proved_prikaz_menu(ID_K_FLARE_NOVY, TRUE, TRUE);
    }
    if (key[K_M]) {
      key[K_M] = FALSE;         // modifikovat flare
      doe_proved_prikaz_menu(ID_K_FLARE_UPRAVIT, TRUE, TRUE);
    }
    if (key[K_P]) {
      key[K_P] = FALSE;         // posun
      doe_proved_prikaz_menu(ID_K_FLARE_POSUNOUT, TRUE, TRUE);
    }
    if (key[K_V]) {
      key[K_V] = FALSE;         // posun stredu
      doe_proved_prikaz_menu(ID_K_STREDLEVELU_POSUNSTREDU, TRUE, TRUE);
    }
    if (key[K_D]) {
      key[K_D] = FALSE;         // posun stredu mlhokostky
      doe_proved_prikaz_menu(ID_K_MIX_POSUNMLHOKOSTKY, TRUE, TRUE);
    }
    if (key[K_S]) {
      key[K_S] = FALSE;         // scale mlhokostky
      doe_proved_prikaz_menu(ID_K_MIX_SCALEMLHOKOSTKY, TRUE, TRUE);
    }
  }
}


/*
  2 druhy stinu
  & svetel

  - iplicidni (databaze)
  - spocitane - specialni

*/
void oe_pridej_dvertex(K_EDITOR * p_cnf, BOD * p, int k, int o, int f,
  byte pozice, byte d2)
{
  if (!p_cnf->dvertnum) {       // maloc - groupnum = 0
    if (!(p_cnf->p_dvert = (D_VERTEX *) malloc(sizeof(D_VERTEX))))
      chyba("pamet");
  }
  else {
    if (!(p_cnf->p_dvert =
        (D_VERTEX *) realloc(p_cnf->p_dvert,
          sizeof(D_VERTEX) * (p_cnf->dvertnum + 1))))
      chyba("Pamet");
  }
  p_cnf->p_dvert[p_cnf->dvertnum].x = p->x;
  p_cnf->p_dvert[p_cnf->dvertnum].y = p->y;
  p_cnf->p_dvert[p_cnf->dvertnum].z = p->z;
  p_cnf->p_dvert[p_cnf->dvertnum].k = k;
  p_cnf->p_dvert[p_cnf->dvertnum].o = o;
  p_cnf->p_dvert[p_cnf->dvertnum].f = f;
  p_cnf->p_dvert[p_cnf->dvertnum].pozice = pozice;
  p_cnf->p_dvert[p_cnf->dvertnum].d2 = d2;
  p_cnf->dvertnum++;
}

void oe_pridej_texture_tag_3d(K_EDITOR * p_cnf, int cislo)
{
  BOD p;
  int k, o, f;

  if (oe_nejblizsi_bod_ve_scene(p_cnf, &p, &k, &o, &f)) {
    oe_pridej_dvertex(p_cnf, &p, k, o, f, (byte) cislo, FALSE);
  }
}

void oe_pridej_cary_texture_tag(K_EDITOR * p_cnf)
{
  int n, dv;

  if ((dv = p_cnf->dvertnum) < 4)
    return;

  p_cnf->linelist_num = 4;
  p_cnf->linelist = 1;
  p_cnf->linelist_barva = DDRGB(0, 1, 0);

  for (n = 0; n < 3; n++) {
    p_cnf->linelist_cara[n].x1 = p_cnf->p_dvert[n].x;
    p_cnf->linelist_cara[n].y1 = p_cnf->p_dvert[n].y;
    p_cnf->linelist_cara[n].x2 = p_cnf->p_dvert[n + 1].x;
    p_cnf->linelist_cara[n].y2 = p_cnf->p_dvert[n + 1].y;
  }

  p_cnf->linelist_cara[n].x1 = p_cnf->p_dvert[n].x;
  p_cnf->linelist_cara[n].y1 = p_cnf->p_dvert[n].y;
  p_cnf->linelist_cara[n].x2 = p_cnf->p_dvert[0].x;
  p_cnf->linelist_cara[n].y2 = p_cnf->p_dvert[0].y;
}

void oe_pridej_texture_tag_2d(K_EDITOR * p_cnf, int cislo, float x, float y)
{
  BOD p;
  int num, dv;

  if (3 < (dv = p_cnf->dvertnum))
    return;

  p.x = x;
  p.y = y;

  if (dv) {
    num = p_cnf->linelist_num++;
    if (!num) {
      p_cnf->linelist = 1;
      p_cnf->linelist_barva = DDRGB(0, 1, 0);
    }

    p_cnf->linelist_cara[num].x1 = p_cnf->p_dvert[dv - 1].x;
    p_cnf->linelist_cara[num].y1 = p_cnf->p_dvert[dv - 1].y;
    p_cnf->linelist_cara[num].x2 = p.x;
    p_cnf->linelist_cara[num].y2 = p.y;

    if (dv == 3) {
      num = p_cnf->linelist_num++;
      p_cnf->linelist_cara[num].x1 = p_cnf->p_dvert[0].x;
      p_cnf->linelist_cara[num].y1 = p_cnf->p_dvert[0].y;
      p_cnf->linelist_cara[num].x2 = p.x;
      p_cnf->linelist_cara[num].y2 = p.y;
    }
  }

  oe_pridej_dvertex(p_cnf, &p, 0, 0, 0, (byte) cislo, TRUE);
}

void oe_zrus_texture_tagy(K_EDITOR * p_cnf)
{
  p_cnf->dvertnum = 0;
  free(p_cnf->p_dvert);
  p_cnf->linelist_num = 0;
}

/*
  Zapisuje do druhe textury
  - naprogramovat pohyb light-map !

  Zapisuje do druhe textury

  - naprogramovat pohyb light-map !

  - 2D verze toho sameho
*/

/*
  Bod t se prevede tak, aby lezel na primce bodu t1 a t2
  if(fabs(dx) > fabs(dy)) { // standartni pojeti

  }
  else {

  }
*/
void interpolace_primky(BOD * t1, BOD * t2, BOD * t, BOD * p)
{
  float k, q;
  float dx, dy;

  dx = t2->x - t1->x;
  dy = t2->y - t1->y;

  if (dx == 0.0f) {             // je svisla -> vsechny y -> x = t1.x
    p->x = t1->x;
    p->y = t->y;
  }
  else if (dy == 0.0f) {        // je vodorovna -> vsechno se dostane na 1 y
    p->x = t->x;
    p->y = t1->y;
  }
  else {
    calc_primku_2d(t1, t2, &k, &q);
    if (fabs(k) > 1.0f) {
      p->y = t->y;
      p->x = (t->y - q) / k;
    }
    else {
      p->x = t->x;
      p->y = k * t->x + q;
    }
  }
}

void interpolace_textury(BOD * t1, BOD * t2, BOD * t3, BOD * t4, BOD * t,
  BOD * p)
{
  // 1. primka od 14, 23 -> dosadit t a zjistit pruseky
  // 2. primka od 43, 12 -> dosadit a zjistit pruseky
  BOD i1, i2, i3, i4;
  BOD _4t, _42, _v_add, _v_sub;
  BOD _3t, _13;
  float du, dv;
  float u, v;

  // 1. rovnobezne, 4-1
  // p = _1 + qt
  interpolace_primky(t1, t2, t, &i1);
  interpolace_primky(t2, t3, t, &i2);
  interpolace_primky(t4, t3, t, &i3);
  interpolace_primky(t1, t4, t, &i4);

  _42.x = i2.x - i4.x;
  _42.y = i2.y - i4.y;
  _4t.x = t->x - i4.x;
  _4t.y = t->y - i4.y;

  du = vel_vect_dir_2d(_42);
  u = vel_vect_dir_2d(_4t);
  _v_add.x = _42.x + _4t.x;
  _v_add.y = _42.y + _4t.y;
  _v_sub.x = _42.x - _4t.x;
  _v_sub.y = _42.y - _4t.y;
  if (vel_vect_dir_2d(_v_add) < vel_vect_dir_2d(_v_sub))
    u = -u;

  _13.x = i3.x - i1.x;
  _13.y = i3.y - i1.y;
  _3t.x = t->x - i3.x;
  _3t.y = t->y - i3.y;

  dv = vel_vect_dir_2d(_13);
  v = vel_vect_dir_2d(_3t);
  _v_add.x = _13.x - _3t.x;
  _v_add.y = _13.y - _3t.y;
  _v_sub.x = _13.x + _3t.x;
  _v_sub.y = _13.y + _3t.y;
  if (vel_vect_dir_2d(_v_add) < vel_vect_dir_2d(_v_sub))
    v = -v;

  p->x = u / du;
  p->y = v / dv;
}

//#define ZOOM 10.0f


void oe_transformuj_2d_texture_tagy(K_EDITOR * p_cnf, int k, int o,
  byte textura)
{
  EDIT_OBJEKT *p_obj = p_cnf->p_kont[k]->p_obj[o];
  MUJ_BOD *p_bod = p_obj->p_vertex;
  GLMATRIX m;
  BOD t, tr[4], p;
  float *p_tx;
  int v, x_c, y_c, xs, ys;
  float x, y, z, w;
  float tx, ty, tz;

  x_c = p_cnf->wxres / 2;
  y_c = p_cnf->wyres / 2;
  xs = p_cnf->wx;
  ys = p_cnf->wy;

  for (v = 0; v < 4; v++) {
    tr[v].x = p_cnf->p_dvert[v].x;
    tr[v].y = p_cnf->p_dvert[v].y;
  }

  mat_mult(kont_world_matrix(p_cnf->p_kont[k]), p_cnf->p_camera, &m);
  mat_mult(&m, p_cnf->p_project, &m);

  //  Pro kazdy bod se provede prespektivni korekce a transformace
  //  z 2d do 3d

  for (v = 0; v < p_obj->vertexnum; v++) {
    tx = p_bod[v].x;
    ty = p_bod[v].y;
    tz = p_bod[v].z;
    x = m._11 * (tx) + m._21 * (ty) + m._31 * (tz) + m._41;
    y = m._12 * (tx) + m._22 * (ty) + m._32 * (tz) + m._42;
    z = m._13 * (tx) + m._23 * (ty) + m._33 * (tz) + m._43;
    w = m._14 * (tx) + m._24 * (ty) + m._34 * (tz) + m._44;

    t.x = ((1.0f + (x / w)) * x_c);
    t.y = ((1.0f - (y / w)) * y_c);

    interpolace_textury(tr, tr + 1, tr + 2, tr + 3, &t, &p);

    p_tx = &p_bod[v].tu1;
    p_tx[textura << 1] = p.x;
    p_tx[(textura << 1) + 1] = p.y;
  }
}

/*
  Jede na vybrane body v group
*/
void oe_transformuj_2d_texture_tagy_body(K_EDITOR * p_cnf, byte textura)
{
  EDIT_OBJEKT *p_obj;
  MUJ_BOD *p_bod;
  GLMATRIX m;
  BOD t, tr[4], p;
  float *p_tx;
  int v, x_c, y_c, xs, ys, g, ok = 0, k, o;
  float x, y, z, w;
  float tx, ty, tz;

  x_c = p_cnf->wxres / 2;
  y_c = p_cnf->wyres / 2;
  xs = p_cnf->wx;
  ys = p_cnf->wy;

  for (v = 0; v < 4; v++) {
    tr[v].x = p_cnf->p_dvert[v].x;
    tr[v].y = p_cnf->p_dvert[v].y;
  }

  for (g = 0; g < p_cnf->groupnum; g++) {
    k = p_cnf->p_group[g].k;
    o = p_cnf->p_group[g].o;
    v = p_cnf->p_group[g].v;

    p_obj = p_cnf->p_kont[k]->p_obj[o];
    p_bod = p_obj->p_vertex;

    if (ok != k) {
      ok = k;
      mat_mult(kont_world_matrix(p_cnf->p_kont[k]), p_cnf->p_camera, &m);
      mat_mult(&m, p_cnf->p_project, &m);
    }

    //  Pro kazdy bod se provede prespektivni korekce a transformace
    //  z 2d do 3d

    tx = p_bod[v].x;
    ty = p_bod[v].y;
    tz = p_bod[v].z;
    x = m._11 * (tx) + m._21 * (ty) + m._31 * (tz) + m._41;
    y = m._12 * (tx) + m._22 * (ty) + m._32 * (tz) + m._42;
    z = m._13 * (tx) + m._23 * (ty) + m._33 * (tz) + m._43;
    w = m._14 * (tx) + m._24 * (ty) + m._34 * (tz) + m._44;

    t.x = ((1.0f + (x / w)) * (float) x_c);
    t.y = ((1.0f - (y / w)) * (float) y_c);
    t.x += (float) xs;
    t.y += (float) ys;

    interpolace_textury(tr, tr + 1, tr + 2, tr + 3, &t, &p);

    p_tx = &p_bod[v].tu1;
    p_tx[textura << 1] = p.x;
    p_tx[(textura << 1) + 1] = p.y;

  }
}

void pridej_objekt_normalove_vektory(K_EDITOR * p_cnf)
{
  int k = p_cnf->vybr_kont;
  int o = p_cnf->vybr_objekt;
  int no = lo_najdi_volny_objekt_kont(p_cnf->p_kont[k]);
  EDIT_OBJEKT *p_obj, *p_puv;
  word *p_line;
  int v, vn;

  if (no == K_CHYBA) {
    ddw("Chyba - neni volny objekt !");
    return;
  }

  p_puv = p_cnf->p_kont[k]->p_obj[o];
  p_obj = vyrob_objekt_komplet(p_puv->vertexnum * 2, 0);

  for (v = 0, vn = 0; v < p_puv->vertexnum; v++) {
    p_obj->p_vertex[vn++] = p_puv->p_vertex[v];
    p_obj->p_vertex[vn].x = p_puv->p_vertex[v].x + p_puv->p_vertex[v].nx;
    p_obj->p_vertex[vn].y = p_puv->p_vertex[v].y + p_puv->p_vertex[v].ny;
    p_obj->p_vertex[vn].z = p_puv->p_vertex[v].z + p_puv->p_vertex[v].nz;
    vn++;
  }
  p_obj->vertexnum = vn;

  if ((p_line = (word *) mmalloc(sizeof(word) * p_obj->vertexnum)) == NULL)
    chyba("pamet");

  for (v = 0; v < p_obj->vertexnum; v++) {
    p_line[v] = v;
  }

  p_obj->p_line = p_line;
  p_obj->linenum = p_obj->vertexnum;
  p_cnf->p_kont[k]->p_obj[no] = p_obj;

  updatuj_kontejner_statistika(p_cnf->p_kont[k], TRUE);
}

void oe_prikaz_vyber_texture_tag_ctverec(K_EDITOR * p_cnf)
{
  float x1, x2, y1, y2;

  oe_zrus_texture_tagy(p_cnf);
  oe_vybrani_ctverce(p_cnf, DDRGB(0, 1, 0));
  x1 = (float) p_cnf->x1;
  x2 = (float) p_cnf->x2;
  y1 = (float) p_cnf->y1;
  y2 = (float) p_cnf->y2;
  oe_pridej_texture_tag_2d(p_cnf, 0, x1, y2);
  oe_pridej_texture_tag_2d(p_cnf, 1, x2, y2);
  oe_pridej_texture_tag_2d(p_cnf, 2, x2, y1);
  oe_pridej_texture_tag_2d(p_cnf, 3, x1, y1);
}

void oe_posun_vsechny_texture_tag(K_EDITOR * p_cnf)
{
  float x, y;
  int i;

  nuluj_mys();
  while (stisk_mys()) {
    x = (float) mi.dx;
    y = (float) mi.dy;

    for (i = 0; i < p_cnf->dvertnum; i++) {
      p_cnf->p_dvert[i].x += x;
      p_cnf->p_dvert[i].y += y;
    }
    oe_pridej_cary_texture_tag(p_cnf);

    nuluj_dif();
    oe_renderuj_scenu_pre(p_cnf);
    oe_renderuj_scenu_po(p_cnf);
  }
}

void oe_posun_nejblizsi_texture_tag(K_EDITOR * p_cnf)
{
  float x, y, vz, vz_max = 10000;
  int i, v;

  if (!p_cnf->dvertnum)
    return;

  nuluj_mys();
  x = (float) mi.x;
  y = (float) mi.y;

  for (i = 0; i < p_cnf->dvertnum; i++) {
    vz = vzdal_bodu_2D_xy(x, y, (p_cnf->p_dvert[i].x), (p_cnf->p_dvert[i].y));
    if (vz < vz_max) {
      vz_max = vz;
      v = i;
    }
  }

  p_cnf->vybr_dvert = v;

  while (stisk_mys()) {

    p_cnf->p_dvert[v].x += (float) mi.dx;
    p_cnf->p_dvert[v].y += (float) mi.dy;

    oe_pridej_cary_texture_tag(p_cnf);

    nuluj_dif();
    oe_renderuj_scenu_pre(p_cnf);
    oe_renderuj_scenu_po(p_cnf);
  }
}

void oe_klavesy_editace_stinu(K_EDITOR * p_cnf)
{
  if (key[K_A] && key[K_CTRL]) {
    key[K_A] = FALSE;
    oe_cekej_na_klavesu(p_cnf);

    if (key[K_Q]) {
      key[K_Q] = FALSE;         // vyber oblasti pomoci ctverce
      oe_prikaz_vyber_texture_tag_ctverec(p_cnf);
    }

    if (key[K_W]) {
      key[K_W] = FALSE;         // posun celeho ctverce
      oe_posun_vsechny_texture_tag(p_cnf);
    }

    if (key[K_S]) {
      key[K_S] = FALSE;         // posun nejblizsiho bodu
      oe_posun_nejblizsi_texture_tag(p_cnf);
    }

    if (key[K_R]) {
      key[K_R] = FALSE;
      oe_zrus_texture_tagy(p_cnf);
    }

    if (key[K_X] && !key[K_CTRL]) {     // transformace 2D flagu//vybrane body//kontejner//objekt
      key[K_X] = FALSE;
      doe_prikaz_texturace_2d(p_cnf, hwnd_aplikace, FALSE);
    }
    if (key[K_X] && key[K_CTRL]) {
      key[K_X] = FALSE;
      doe_prikaz_texturace_2d(p_cnf, hwnd_aplikace, TRUE);
    }

    if (key[K_Z]) {             // transformace 3D flagu//vybrane body//kontejner//objekt
      key[K_Z] = FALSE;
    }

    if (key[K_A]) {
      key[K_A] = FALSE;
      pridej_objekt_normalove_vektory(p_cnf);
    }
  }
  else {

    // 3D texturova kotva
    if (key[K_1] && !key[K_CTRL] && key[K_SHFT_L]) {
      key[K_1] = FALSE;
      oe_pridej_texture_tag_3d(p_cnf, 0);
    }

    // 2D texturova kotva
    if (key[K_2] && !key[K_CTRL] && key[K_SHFT_L]) {
      key[K_2] = FALSE;
      oe_pridej_texture_tag_2d(p_cnf, 0, (float) mi.x, (float) mi.y);
    }

/*
   // 3D texturova kotva
   for(i = K_1; i < K_5; i++) {
     if(key[i]&&!key[K_CTRL]&&key[K_SHFT_L]) {
       key[i] = FALSE;
       oe_hlaska_konzole(p_con, "Pridavam 3D texturovou kotvu...",0);
       oe_pridej_texture_tag_3d(p_cnf,i-K_1);
     }
   }
   // 2D texturova kotva
   for(i = K_5; i <= K_9; i++) {
     if(key[K_5]&&!key[K_CTRL]&&key[K_SHFT_L]) {
       key[K_5] = FALSE;
       oe_hlaska_konzole(p_con, "Pridavam 2D texturovou kotvu...",0);
       oe_pridej_texture_tag_2d(p_cnf,0,(float)mi.x,(float)mi.y);
     }
   }
*/
  }
}

void oe_test_bmp(K_EDITOR * p_cnf)
{
  bitmapa *p_bmp = bmp_vyrob(400, 400);

  bmp_uloz("e:\\test.bmp", p_bmp);
  bmp_zrus(&p_bmp);
}

void oe_prikaz_test(K_EDITOR * p_cnf)
{
  BOD p0 = { 0, 0, 0 }, p1 = {
  1, 2.89f, 0}, p2 = {
  1, 2.89f, 1};
  BOD orig = { 0, 3, 0 }, dir = {
  2, -3, 1};
  BOD a = { 0, 3, 0 }, b = {
  2, 0, 1}, p;
  int vysl;
  ROVINA r;


  //  vysl = intersect_triangle(&orig,&dir,&p0,&p1,&p2,&t,&u,&v);
//  ddw("Vysl = %d",vysl);
  calc_rovinu_bod(&p0, &p1, &p2, &r);
  vysl = calc_prusek_bod(&r, &a, &b, &p);

}



/*
int spocti_mezery(char *p_string)
{
  int mezer = 0;
  while((p_string = strchr(p_string))) {
    p_string++; 
    mezer++;
  }
  return(mezer);
}

char ** parse_line(char *line_str, int stringu) 
{
  char **p_list;
  int mezer = spocti_mezery(line_str);

  p_list = malloc(sizeof(char *)*
}
*/

void oe_test(K_EDITOR * p_cnf)
{
  bitmapa *p_bmp = bmp_nahraj_pack("e:\\kill-gimp.bmp", NULL);


  /*
     int show = 1;
     bitmapa *p_bmp = bmp_nahraj("e:\\test2.bmp",FALSE);
     bmp_uloz("e:\\vystup.bmp",p_bmp);
   */

  /*
     EDIT_OBJEKT *p_obj;
     int oh,v;

     oh = oe_vybr_obj_reset();
     while(p_obj = oe_vybr_obj_get(&oh)) {
     obb_calc_obj(p_obj);
     //for(v = 0; v < p_obj->vertexnum; v++) {
     //kprintf(TRUE,"%d [%.2f %.2f %.2f]",v,p_obj->p_vertex[v].nx,p_obj->p_vertex[v].ny,p_obj->p_vertex[v].nz);
     //}
     }
   */
}


void oe_test_text_resident(K_EDITOR * p_cnf)
{
  int i, rez;

  for (i = 0; i < MAX_EDIT_TEXTUR; i++) {
    if (p_cnf->p_text[i].load) {        // textura je loadnuta
      glGetTexParameteriv(p_cnf->p_text[i].typ, GL_TEXTURE_RESIDENT, &rez);
      kprintf(1, "Textura %d,%s je %d", p_cnf->p_text[i].text,
        p_cnf->p_text[i].jmeno, rez);
    }
  }
}

void oe_test_vybuch(K_EDITOR * p_cnf);

void oe_klavesy_test(K_EDITOR * p_cnf)
{
  if (key[K_A] && key[K_CTRL]) {
    key[K_A] = FALSE;
    oe_test(p_cnf);
  }

  if (key[K_A] && !key[K_CTRL]) {
    key[K_A] = FALSE;
    oe_test(p_cnf);
  }
}

void oe_klavesy_editace_kontejneru(K_EDITOR * p_cnf)
{
  if (key[K_K] && key[K_CTRL]) {
    key[K_K] = FALSE;
    oe_cekej_na_klavesu(p_cnf);

    if (key[K_N]) {
      key[K_N] = FALSE;
      doe_proved_prikaz_menu(ID_KONTEJNER_NOVY, TRUE, TRUE);
    }
    if (key[K_S] && key[K_CTRL]) {
      key[K_CTRL] = FALSE;
      key[K_S] = FALSE;
      doe_proved_prikaz_menu(ID_KONTEJNER_SCALECISLAMA, TRUE, TRUE);
    }
    if (key[K_S] && !key[K_CTRL]) {
      key[K_S] = FALSE;
      doe_proved_prikaz_menu(ID_KONTEJNER_SCALEMYSI, TRUE, TRUE);
    }
    if (key[K_J]) {
      key[K_J] = FALSE;
      doe_proved_prikaz_menu(ID_KONTEJNER_TRANSFORMUJ, TRUE, TRUE);
    }
    if (key[K_L] && !key[K_CTRL]) {
      key[K_L] = FALSE;
      doe_proved_prikaz_menu(ID_KONTEJNER_SLEPMAXIMALNE, TRUE, TRUE);
    }
    if (key[K_C]) {
      key[K_C] = FALSE;
      doe_proved_prikaz_menu(ID_KONTEJNER_CETRUJ, TRUE, TRUE);
    }
    if (key[K_R] && !key[K_CTRL]) {
      key[K_R] = FALSE;
      doe_proved_prikaz_menu(ID_KONTEJNER_ROTACE, TRUE, TRUE);
    }
    if (key[K_R] && key[K_CTRL]) {
      key[K_CTRL] = FALSE;
      key[K_R] = FALSE;
      doe_proved_prikaz_menu(ID_K_KONTEJNER_ROTACECISLAMA, TRUE, TRUE);
    }
    if (key[K_O]) {
      key[K_O] = FALSE;
      doe_proved_prikaz_menu(ID_K_KONTEJNER_ROTACEOSOVE, TRUE, TRUE);
    }
    if (key[K_P]) {
      key[K_P] = FALSE;
      doe_proved_prikaz_menu(ID_KONTEJNER_POSUN, TRUE, TRUE);
    }
    if (key[K_M] && !key[K_CTRL]) {
      key[K_M] = FALSE;
      doe_proved_prikaz_menu(ID_K_SMAZATKONTEJNER, TRUE, TRUE);
    }
    if (key[K_D]) {
      key[K_D] = FALSE;
      doe_proved_prikaz_menu(ID_KONTEJNER_OPTIMALIZACE, TRUE, TRUE);
    }
    if (key[K_Z]) {
      key[K_Z] = FALSE;
      doe_proved_prikaz_menu(ID_K_KONTEJNER_ZRCADLENI, TRUE, TRUE);
    }
    if (key[K_K] && !key[K_CTRL]) {
      key[K_K] = FALSE;
      doe_proved_prikaz_menu(ID_K_KONTEJNER_KOPIE, TRUE, TRUE);
    }
    if (key[K_K] && key[K_CTRL]) {
      key[K_K] = FALSE;
      doe_proved_prikaz_menu(ID_K_KONTEJNER_KOPIE_2, TRUE, TRUE);
    }
    if (key[K_V] && !key[K_CTRL]) {
      key[K_V] = FALSE;
      doe_proved_prikaz_menu(ID_K_KONTEJNER_NORMALOVEVEKTORY, TRUE, TRUE);
    }
    if (key[K_V] && key[K_CTRL]) {
      key[K_V] = FALSE;
      doe_proved_prikaz_menu(ID_K_KONTEJNER_NORMALOVEVEKTORYNAVSECHNO, TRUE,
        TRUE);
    }
    if (key[K_X]) {
      key[K_X] = FALSE;
      doe_proved_prikaz_menu(ID_K_KONTEJNER_ZMENAKRESLENI, TRUE, TRUE);
    }
    if (key[K_B]) {
      key[K_B] = FALSE;
      doe_proved_prikaz_menu(ID_K_SVETLA_DXSVETLA, TRUE, TRUE);
    }
    if (key[K_I]) {
      key[K_I] = FALSE;
      doe_proved_prikaz_menu(ID_PRIKAZ_SET_BARVA_KONTEJNERU, TRUE, TRUE);
    }
    if (key[K_U]) {
      key[K_U] = FALSE;
      doe_proved_prikaz_menu(ID_PRIKAZ_RESET_BARVA_KONTEJNER, TRUE, TRUE);
    }
    if (key[K_A]) {
      key[K_A] = FALSE;
      doe_proved_prikaz_menu(ID_K_KONTEJNER_AUTOSCALE, TRUE, TRUE);
    }
  }
  else {
    if (key[K_W]) {
      key[K_W] = FALSE;
      doe_proved_prikaz_menu(ID_K_CELASCENA_SOUHRN, TRUE, TRUE);
    }
  }
}

void oe_klavesy_editace_objektu(K_EDITOR * p_cnf)
{

  if (key[K_O] && key[K_CTRL]) {
    key[K_O] = FALSE;
    oe_cekej_na_klavesu(p_cnf);

    if (key[K_M]) {
      key[K_M] = FALSE;
      doe_proved_prikaz_menu(ID_OBJEKT_SMAZ, TRUE, TRUE);
    }
    if (key[K_P] && !key[K_CTRL]) {
      key[K_P] = FALSE;
      doe_proved_prikaz_menu(ID_OBJEKT_POSUN, TRUE, TRUE);
    }
    if (key[K_P] && key[K_CTRL]) {
      key[K_P] = FALSE;
      key[K_CTRL] = FALSE;
      doe_proved_prikaz_menu(ID_K_OBJEKT_POSUNKOPII, TRUE, TRUE);
    }
    if (key[K_K]) {
      key[K_K] = FALSE;
      doe_proved_prikaz_menu(ID_K_OBJEKT_ROZDROB, TRUE, TRUE);
    }
    if (key[K_L] && !key[K_CTRL]) {
      key[K_L] = FALSE;
      doe_proved_prikaz_menu(ID_KONTEJNER_SLEPENIDVOUOBJEKTU, TRUE, TRUE);
    }
    if (key[K_C]) {
      key[K_C] = FALSE;
      doe_proved_prikaz_menu(ID_K_OBJEKT_KOPIRUJ, TRUE, TRUE);
    }
    if (key[K_Z]) {
      key[K_Z] = FALSE;
      doe_proved_prikaz_menu(ID_K_OBJEKT_ZRCADLENI, TRUE, TRUE);
    }
    if (key[K_R] && !key[K_CTRL]) {
      key[K_R] = FALSE;
      doe_proved_prikaz_menu(ID_K_OBJEKT_ROTACE, TRUE, TRUE);
    }
    if (key[K_R] && key[K_CTRL]) {
      key[K_CTRL] = FALSE;
      key[K_R] = FALSE;
      doe_proved_prikaz_menu(ID_K_OBJEKT_ROTACECISLAMA, TRUE, TRUE);
    }
    if (key[K_O]) {
      key[K_O] = FALSE;
      doe_proved_prikaz_menu(ID_K_OBJEKT_ROTACEOSOVE, TRUE, TRUE);
    }
    if (key[K_S] && !key[K_CTRL]) {
      key[K_S] = FALSE;
      doe_proved_prikaz_menu(ID_K_OBJEKT_SCALE, TRUE, TRUE);
    }
    if (key[K_S] && key[K_CTRL]) {
      key[K_CTRL] = FALSE;
      key[K_S] = FALSE;
      doe_proved_prikaz_menu(ID_K_OBJEKT_SCALECISLAMA, TRUE, TRUE);
    }
    if (key[K_X]) {
      key[K_X] = FALSE;
      doe_proved_prikaz_menu(ID_K_OBJEKT_ZMENAKRESLENI, TRUE, TRUE);
    }
    if (key[K_V]) {
      key[K_V] = FALSE;
      doe_proved_prikaz_menu(ID_K_OBJEKT_VERTEXOVYRAYTRACER, TRUE, TRUE);
    }
    if (key[K_I]) {
      key[K_I] = FALSE;
      doe_proved_prikaz_menu(ID_PRIKAZ_SET_BARVA_OBJEKT, TRUE, TRUE);
    }
    if (key[K_U]) {
      key[K_U] = FALSE;
      doe_proved_prikaz_menu(ID_PRIKAZ_RESET_BARVA_OBJEKT, TRUE, TRUE);
    }
    if (key[K_J]) {
      key[K_J] = FALSE;
      doe_proved_prikaz_menu(ID_K_OBJEKT_POSUNPIVOTBODU, TRUE, TRUE);
    }
  }
  else {
    if (key[K_Z] && !key[K_CTRL]) {
      key[K_Z] = FALSE;
      doe_proved_prikaz_menu(ID_K_OBJEKT_OSAMOSTATNIT, TRUE, TRUE);
    }
    if (key[K_N] && !key[K_CTRL]) {     // novy root
      key[K_N] = FALSE;
      //doe_proved_prikaz_menu(ID_K_KOSTI_VLOZJOINTMEZIKOZY,TRUE,TRUE);
      //doe_proved_prikaz_menu(ID_K_BODY_NASTAVBARVU,TRUE,TRUE);
      oe_prikaz_set_barva_body(p_cnf);
    }
    if (key[K_B] && !key[K_CTRL]) {     // novy child
      key[K_B] = FALSE;
      doe_proved_prikaz_menu(ID_K_KOSTI_PRIPOJJOINTMEZIKOZY, TRUE, TRUE);
    }
  }
}

void oe_prikaz_rychla_rotace(K_EDITOR * p_cnf, int t)
{
  int k = p_cnf->vybr_kont;
  int o = p_cnf->vybr_objekt;

  n_objekt(k, o);

  rotuj_text(p_cnf->p_kont[k]->p_obj[o], t, 1);
}

void oe_prikaz_rychly_mirror(K_EDITOR * p_cnf, int t, int mu, int mv)
{
  int k = p_cnf->vybr_kont;
  int o = p_cnf->vybr_objekt;

  n_objekt(k, o);

  mirror_text(p_cnf->p_kont[k]->p_obj[o], t, mu, mv);
}


void oe_klavesy_textury(K_EDITOR * p_cnf)
{
  /*
     if(key[K_T]&&key[K_CTRL]) {
     key[K_CTRL] = FALSE;
     key[K_T] = FALSE;
     oe_cekej_na_klavesu(p_cnf);

     if(key[K_Z]) {
     key[K_Z] = FALSE;
     doe_proved_prikaz_menu(ID_K_TEXTURY_ZRCADLENI,TRUE,TRUE);
     }
     if(key[K_R]) {
     key[K_R] = FALSE;
     doe_proved_prikaz_menu(ID_K_TEXTURY_ROTACE,TRUE,TRUE);
     }
     } else { //ted a tady pracujeme s texturovaci listou
   */
  if (key[K_CARKA]) {           // lista nahoru !
    doe_proved_prikaz_menu(ID_NASTROJE_OBRUBALEVELU, TRUE, TRUE);
    key[K_CARKA] = FALSE;
  }
  /*
     if(key[K_TECKA]) { // lista dolu !
     key[K_TECKA] = FALSE;
     doe_prikaz_posun_material_lista(p_cnf,1);
     }
     if(key[K_7]&&!key[K_CTRL]&&!key[K_SHFT_L]) { // mirror dle u
     key[K_7] = FALSE;     
     doe_proved_prikaz_menu(ID_PRIKAZ_RYCHLY_MIRROR_U,TRUE,TRUE);
     }
     if(key[K_8]&&!key[K_CTRL]&&!key[K_SHFT_L]) { // rychly mirror dle v
     key[K_8] = FALSE;
     doe_proved_prikaz_menu(ID_PRIKAZ_RYCHLY_MIRROR_V,TRUE,TRUE);
     }
     if(key[K_0]&&!key[K_CTRL]&&!key[K_SHFT_L]) { // rychla rotace
     key[K_0] = FALSE;
     doe_proved_prikaz_menu(ID_PRIKAZ_RYCHLA_ROTACE,TRUE,TRUE);
     }
   */
  //}
}

void oe_klavesy_editace_vertexu(K_EDITOR * p_cnf)
{
  if (key[K_G] && key[K_CTRL]) {
    /* Editace group */
    key[K_CTRL] = FALSE;
    key[K_G] = FALSE;
    oe_cekej_na_klavesu(p_cnf);

    if (key[K_0]) {
      key[K_0] = FALSE;
      oe_prikaz_no_off_popisy_vertexu(p_cnf);
    }
    if (key[K_1]) {
      key[K_1] = FALSE;
      doe_proved_prikaz_menu(ID_K_GROUPBODU_PRIMAEDITACE, TRUE, TRUE);
    }
    if (key[K_2]) {
      key[K_2] = FALSE;
      doe_proved_prikaz_menu(ID_K_GROUPBODU_NASOBENI, TRUE, TRUE);
    }
    if (key[K_P]) {
      key[K_P] = FALSE;
      doe_proved_prikaz_menu(ID_K_EDITACEZEME_POSUNVSECHBODU, TRUE, TRUE);
    }
    if (key[K_M]) {
      key[K_M] = FALSE;
      doe_proved_prikaz_menu(ID_K_GROUPBODU_SMAZANI, TRUE, TRUE);
    }
  }
  else {
    if (key[K_C] && !key[K_CTRL]) {
      key[K_C] = FALSE;
      doe_proved_prikaz_menu(ID_K_GROUPBODU_VYBRAT, TRUE, FALSE);
    }
    if (key[K_C] && key[K_CTRL]) {
      key[K_C] = FALSE;
      doe_proved_prikaz_menu(ID_K_GROUPBODU_PRIDATDOVYBERU, TRUE, FALSE);
    }
    if (key[K_F] && !key[K_CTRL]) {
      key[K_F] = FALSE;
      doe_proved_prikaz_menu(ID_K_GROUPBODU_VYBRATPLOSKOU, TRUE, FALSE);
    }
    if (key[K_F] && key[K_CTRL]) {
      key[K_F] = FALSE;
      doe_proved_prikaz_menu(ID_K_GROUPBODU_PRIDATFACE, TRUE, FALSE);
    }
    if (key[K_Y] && key[K_CTRL]) {
      key[K_Y] = FALSE;
      doe_proved_prikaz_menu(ID_K_VYBERBODU_UNGROUPJEDNOHO, TRUE, FALSE);
    }
    if (key[K_ZAV_L] && !key[K_CTRL]) {
      key[K_ZAV_L] = FALSE;
      doe_proved_prikaz_menu(ID_K_EDITACEZEME_VYSKAVSECHBODU, TRUE, TRUE);
    }
    if (key[K_ZAV_P] && !key[K_CTRL]) {
      key[K_ZAV_P] = FALSE;
      doe_proved_prikaz_menu(ID_K_EDITACEZEME_POSUNVSECHBODU, TRUE, TRUE);
    }
    if (key[K_STREDNIK] && !key[K_CTRL]) {
      key[K_STREDNIK] = FALSE;
      doe_proved_prikaz_menu(ID_K_EDITACEZEME_VYSKAVYBRANEHOBODU, TRUE, TRUE);
    }
    if (key[K_UVOZ] && !key[K_CTRL]) {
      key[K_UVOZ] = FALSE;
      doe_proved_prikaz_menu(ID_K_EDITACEZEME_POSUNVYBRANEHOBODU, TRUE, TRUE);
    }
    if (key[K_L] && !key[K_CTRL]) {
      key[K_L] = FALSE;
      doe_proved_prikaz_menu(ID_K_EDITACEZEME_VYBERBODU, TRUE, TRUE);
    }
    if (key[K_L] && key[K_CTRL]) {
      key[K_CTRL] = FALSE;
      key[K_L] = FALSE;
      doe_proved_prikaz_menu(ID_VERTEX_INFO, TRUE, TRUE);
    }

    /* Editace facu
     */
    if (key[K_I] && key[K_CTRL]) {
      key[K_I] = FALSE;
      doe_proved_prikaz_menu(ID_K_VYBERFACE_ODEBRATPLOSKU, TRUE, FALSE);
    }
    if (key[K_U] && !key[K_CTRL]) {
      key[K_U] = FALSE;
      doe_proved_prikaz_menu(ID_K_VYBERFACE_ODEBRATCTVERCEM, TRUE, FALSE);
    }
    if (key[K_U] && key[K_CTRL]) {
      key[K_U] = FALSE;
      doe_proved_prikaz_menu(ID_K_VYBERFACE_ODEBRATVSECHNY, TRUE, FALSE);
    }
    if (key[K_V] && !key[K_CTRL]) {
      key[K_V] = FALSE;
      doe_proved_prikaz_menu(ID_K_VYBERFACE_VYBRATFACECTVERCEM, TRUE, FALSE);
    }
    if (key[K_V] && key[K_CTRL]) {
      key[K_V] = FALSE;
      doe_proved_prikaz_menu(ID_K_VYBERFACE_PRIDATFACECTVERCEM, TRUE, FALSE);
    }

/*
  Editace texelu
*/
    if (key[K_Z] && key[K_CTRL]) {
      key[K_CTRL] = FALSE;
      key[K_Z] = FALSE;
      doe_proved_prikaz_menu(ID_K_BODY_SLIJBODY, TRUE, TRUE);
    }
    if (!key[K_CTRL] && key[K_B]) {
      key[K_B] = FALSE;
      doe_proved_prikaz_menu(ID_K_VSECHNYBODY_SJEDNOT, TRUE, TRUE);
    }
    if (key[K_CTRL] && key[K_B]) {
      key[K_B] = FALSE;
      doe_proved_prikaz_menu(ID_PRIKAZ_GOURAND_MAPING_BODY, TRUE, TRUE);
    }
/*
   if(!key[K_CTRL]&&key[K_N]) {
     key[K_N] = FALSE;
     doe_proved_prikaz_menu(ID_K_FACE_VYRIZNIOBJEKT_CROP,TRUE,TRUE);
   }
   if(key[K_CTRL]&&key[K_N]) {
     key[K_N] = FALSE;
     doe_proved_prikaz_menu(ID_PRIKAZ_SET_BARVA,TRUE,TRUE);
   }
*/
    if (key[K_CTRL] && key[K_ZAV_L]) {  // vyska podle levelu
      key[K_ZAV_L] = FALSE;
      doe_proved_prikaz_menu(ID_K_BODY_SJEDNOTVYSKU2, TRUE, TRUE);
    }
    if (key[K_CTRL] && key[K_ZAV_P]) {  // vyska podle vybraneho
      key[K_ZAV_P] = FALSE;
      doe_proved_prikaz_menu(ID_K_BODY_SJEDNOTVYSKU, TRUE, TRUE);
    }
  }
}


void oe_prikaz_zmena_textury_draty(K_EDITOR * p_cnf)
{
  if (p_cnf->cnf.dratove) {
    p_cnf->cnf.dratove = 0;
    nastav_material(1, 1, 1);
  }
  else {
    p_cnf->cnf.dratove = 1;
    nastav_material(0, 0, 1);
  }
}

void oe_prikaz_zmena_textury_draty2(K_EDITOR * p_cnf)
{
  if (p_cnf->cnf.textury) {
    p_cnf->cnf.textury = 0;
    p_cnf->cnf.dratove = 1;
    nastav_material(1, 1, 1);
  }
  else {
    p_cnf->cnf.textury = 1;
    p_cnf->cnf.dratove = 0;
    nastav_material(0, 0, 1);
  }
}

void oe_prikaz_konzole_on_off(K_EDITOR * p_cnf)
{
  //p_cnf->cnf.konzole = (p_cnf->cnf.konzole) ? 0 : 1;
  //oe_updatuj_konzoli(p_con, p_cnf);
}

void oe_prikaz_list_objekty(K_EDITOR * p_cnf, int smer)
{
  int i;

  if (smer < 0) {
    if ((p_cnf->prvni_objekt - p_cnf->d_listu) < 0)
      p_cnf->prvni_objekt = 0;
    else
      p_cnf->prvni_objekt -= p_cnf->d_listu;
  }
  else {
    i = p_cnf->vybr_kont;
    i = p_cnf->p_kont[i]->max_objektu;

    if ((p_cnf->prvni_objekt += p_cnf->d_listu) > i - p_cnf->d_listu)
      p_cnf->prvni_objekt = i - p_cnf->d_listu;
  }
}

void oe_prikaz_list_kontejnery(K_EDITOR * p_cnf, int smer)
{
  int i;

  if (smer < 0) {
    if ((p_cnf->prvni_kont - p_cnf->d_listu_kont) < 0)
      p_cnf->prvni_kont = 0;
    else
      p_cnf->prvni_kont -= p_cnf->d_listu_kont;
  }
  else {
    i = p_cnf->max_kont;
    if ((p_cnf->prvni_kont += p_cnf->d_listu_kont) > i - p_cnf->d_listu_kont)
      p_cnf->prvni_kont = i - p_cnf->d_listu_kont;
  }
}

void oe_prikaz_zmena_cul(K_EDITOR * p_cnf)
{
  if (++p_cnf->cul > 2)
    p_cnf->cul = 0;

  cull_nastav(p_cnf->cul);
}

void oe_prikaz_zrus_preindexuj_scenu(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  int k, o;

  for (k = 0; k < p_cnf->max_kont; k++) {
    if (p_cnf->p_kont[k]) {
      p_kont = p_cnf->p_kont[k];
      p_kont->kreslit = TRUE;
      for (o = 0; o < p_kont->max_objektu; o++) {
        if (p_kont->p_obj[o])
          p_kont->p_obj[o]->kreslit = TRUE;
      }
    }
  }
}

// aby to zachovavalo vyber cetvercem
void oe_prikaz_selekce_objektu(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  int k, o;

  if (!p_cnf->cnf.vsechno_obj) {
    for (k = 0; k < p_cnf->max_kont; k++) {
      if (p_cnf->p_kont[k]) {
        if (k == p_cnf->vybr_kont) {
          p_cnf->p_kont[k]->kreslit = TRUE;
          p_kont = p_cnf->p_kont[k];
          for (o = 0; o < p_kont->max_objektu; o++) {
            if (p_kont->p_obj[o]) {
              if (o == p_cnf->vybr_objekt)
                p_kont->p_obj[o]->kreslit = TRUE;
              else
                p_kont->p_obj[o]->kreslit = FALSE;
            }
          }
        }
        else {
          p_cnf->p_kont[k]->kreslit = FALSE;
        }
      }
    }
  }
  else {
    oe_prikaz_zrus_preindexuj_scenu(p_cnf);
  }
  p_cnf->cnf.vsechno_obj = !p_cnf->cnf.vsechno_obj;
}

void oe_prikaz_selekce_kontejneru(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  int k, o;

  if (p_cnf->cnf.vsechno_kont) {
    for (k = 0; k < p_cnf->max_kont; k++) {
      if (p_cnf->p_kont[k]) {
        if (k == p_cnf->vybr_kont) {
          p_cnf->p_kont[k]->kreslit = TRUE;
          p_kont = p_cnf->p_kont[k];
          for (o = 0; o < p_kont->max_objektu; o++) {
            if (p_kont->p_obj[o])
              p_kont->p_obj[o]->kreslit = TRUE;
          }
        }
        else
          p_cnf->p_kont[k]->kreslit = FALSE;
      }
    }
  }
  else {
    oe_prikaz_zrus_preindexuj_scenu(p_cnf);
  }
  p_cnf->cnf.vsechno_kont = !p_cnf->cnf.vsechno_kont;
}

void oe_prikaz_zmena_kresleni_kontejneru(K_EDITOR * p_cnf)
{
  int k = p_cnf->vybr_kont;

  n_kont(k);

  p_cnf->p_kont[k]->kreslit = !p_cnf->p_kont[k]->kreslit;
}

void oe_prikaz_zmena_kresleni_objekt(K_EDITOR * p_cnf)
{
  int handle = oe_vybr_obj_reset();
  EDIT_KONTEJNER *p_kont;
  EDIT_OBJEKT *p_obj;
  int k, o;

  for (k = 0; k < p_cnf->max_kont; k++) {
    if (p_cnf->p_kont[k]) {
      p_cnf->p_kont[k]->kreslit = TRUE;
      p_kont = p_cnf->p_kont[k];
      for (o = 0; o < p_kont->max_objektu; o++) {
        if (p_kont->p_obj[o]) {
          p_kont->p_obj[o]->kreslit = FALSE;
        }
      }
    }
  }

  while ((p_obj = oe_vybr_obj_get_ind(&handle, &k, &o))) {
    p_obj->kreslit = TRUE;
  }
}

void oe_prikaz_zmena_kresleni_kamer(K_EDITOR * p_cnf)
{
  p_cnf->p_kont[KAMERA_1]->kreslit =
    p_cnf->p_kont[KAMERA_2]->kreslit =
    p_cnf->p_kont[KAMERA_3]->kreslit =
    p_cnf->p_kont[KAMERA_4]->kreslit = !p_cnf->p_kont[R_BOD]->kreslit;
}

void oe_prikaz_zmena_origin(K_EDITOR * p_cnf)
{
  p_cnf->cnf.objekt_origin = !p_cnf->cnf.objekt_origin;
}

void oe_prikaz_smazani_konzole(K_EDITOR * p_cnf)
{
// smaz_surf(p_con, akt_konzole_pozadi);
}

void oe_prikaz_nitkovy_kriz(K_EDITOR * p_cnf)
{
  p_cnf->cnf.nitkovy_kriz = !p_cnf->cnf.nitkovy_kriz;
}

void oe_prikaz_zmena_konzole_objekty(K_EDITOR * p_cnf)
{
  oe_prikaz_smazani_konzole(p_cnf);
// p_cnf->cnf.konzole_objekty = !p_cnf->cnf.konzole_objekty;
}

void oe_vyber_material(K_EDITOR * p_cnf)
{
  int k = p_cnf->vybr_kont;
  int o = p_cnf->vybr_objekt;

  n_objekt(k, o);
  p_cnf->km.vybr_mat = p_cnf->p_kont[k]->p_obj[o]->material;
}

void oe_klavesy_konfigurace(K_EDITOR * p_cnf)
{
  if (key[K_F8] && key[K_CTRL]) {       // samotny prikaz konfigurace
    key[K_CTRL] = FALSE;
    key[K_F8] = FALSE;
    prikaz(doe_prikaz_konfigurace_table(p_cnf, hwnd_aplikace, 0));
  }

  if (key[K_M] && !key[K_CTRL]) {
    key[K_M] = FALSE;
    oe_vyber_material(p_cnf);
    doe_editor_materialu(p_cnf);
  }

  if (key[K_X] && !key[K_CTRL]) {
    key[K_X] = FALSE;
    oe_prikaz_selekce_objektu(p_cnf);
  }
  if (key[K_X] && key[K_CTRL]) {
    key[K_X] = FALSE;
    oe_prikaz_selekce_kontejneru(p_cnf);
  }

  if (key[K_T] && !key[K_CTRL]) {
    key[K_T] = FALSE;
    oe_prikaz_zmena_textury_draty2(p_cnf);
  }
  if (key[K_D] && !key[K_CTRL]) {
    key[K_D] = FALSE;
    oe_prikaz_zmena_textury_draty(p_cnf);
  }

  if (key[K_Y] && !key[K_CTRL]) {
    key[K_Y] = FALSE;
    prikaz(doe_edit_jmeno_objektu(hwnd_aplikace));
  }
  if (key[K_I] && !key[K_CTRL]) {
    key[K_I] = FALSE;
    prikaz(doe_edit_jmeno_kontejneru(hwnd_aplikace));
  }
  if (key[K_F12]) {
    key[K_F12] = FALSE;
    doe_prepni_do_game_editor();
  }
}

void oe_klavesy_vyroba_objektu(K_EDITOR * p_cnf)
{
  if (key[K_5] && !key[K_CTRL] && !key[K_SHFT_L]) {
    key[K_5] = FALSE;
    doe_proved_prikaz_menu(ID_K_BODY_ULOZITPRIMKU, TRUE, TRUE);
  }
  if (key[K_6] && !key[K_CTRL] && !key[K_SHFT_L]) {
    key[K_6] = FALSE;
    doe_proved_prikaz_menu(ID_K_BODY_PRIPOJITBODY, TRUE, TRUE);
  }
}

dword oe_vrat_body_ve_scene(K_EDITOR * p_cnf, int x1, int y1,
  int x2, int y2, I_VERTEX * p_index)
{
  // pokud je p_vert == NULL - vrati jen pocet bodu
  // pokud je p_vert != NULL - vrati pocet a zaroven body zapisuje
  // do teto struktury
  EDIT_KONTEJNER *p_kont;
  EDIT_OBJEKT *p_obj;
  OBJ_VERTEX *p_vert;
  GLMATRIX m;
  dword bodu = 0;
  float z;
  int x, y, k, ob, v;
  int vertexnum, kontnum = p_cnf->max_kont, objnum;

  for (k = 0; k < kontnum; k++) {
    p_kont = p_cnf->p_kont[k];
    if (!p_kont || !p_kont->kreslit)
      continue;

    if (!(p_kont->kflag & KONT_KEYFRAME))
      calc_3d_2d_matrix(kont_world_matrix(p_kont),
        p_cnf->p_camera, p_cnf->p_project, &m);

    objnum = p_kont->max_objektu;
    for (ob = 0; ob < objnum; ob++) {
      p_obj = p_cnf->p_kont[k]->p_obj[ob];
      if (!p_obj || !p_obj->kreslit)
        continue;

      if (p_kont->kflag & KONT_KEYFRAME)
        calc_3d_2d_matrix(&p_obj->m, p_cnf->p_camera, p_cnf->p_project, &m);

      vertexnum = p_obj->vertexnum;
      p_vert = p_obj->p_vertex;

      for (v = 0; v < vertexnum; v++) {
        if (transf_3d_2d_matrix(p_vert[v].x, p_vert[v].y, p_vert[v].z,
            &x, &y, &z, &m,
            OXRES, OYRES, OXSTART, OYSTART,
            p_cnf->kam.near_plane, p_cnf->kam.far_plane)) {

          if (x >= x1 && x <= x2 && y >= y1 && y <= y2) {
            if (p_index) {
              p_index[bodu].v = v;
              p_index[bodu].o = ob;
              p_index[bodu].k = k;
            }
            bodu++;
          }
        }
      }
    }
  }
  return (bodu);
}


/* 
  Prusecik paprsku s objektama kontejneru
*/
int intersect_kont(EDIT_KONTEJNER * p_kont, BOD * p_orig, BOD * p_dir,
  int *p_o, int *p_f, float *p_t)
{
  GLMATRIX *p_world = kont_world_matrix(p_kont);
  EDIT_OBJEKT *p_obj;
  MUJ_BOD *p_vert;
  word *p_face;
  BOD p0, p1, p2;
  float u, v, t;
  int facenum;
  int o, f;

  *p_t = FLT_MAX;

  for (o = 0; o < p_kont->max_objektu; o++) {
    p_obj = p_kont->p_obj[o];
    if (!p_obj)
      continue;

    facenum = p_obj->facenum;
    p_vert = p_obj->p_vertex;
    p_face = p_obj->p_face;

    for (f = 0; f < facenum; f += 3) {
      p0.x = p_vert[p_face[f]].x;
      p0.y = p_vert[p_face[f]].y;
      p0.z = p_vert[p_face[f]].z;
      p1.x = p_vert[p_face[f + 1]].x;
      p1.y = p_vert[p_face[f + 1]].y;
      p1.z = p_vert[p_face[f + 1]].z;
      p2.x = p_vert[p_face[f + 2]].x;
      p2.y = p_vert[p_face[f + 2]].y;
      p2.z = p_vert[p_face[f + 2]].z;
      transformuj_bod_matici(&p0.x, &p0.y, &p0.z, p_world);
      transformuj_bod_matici(&p1.x, &p1.y, &p1.z, p_world);
      transformuj_bod_matici(&p2.x, &p2.y, &p2.z, p_world);
      if (intersect_triangle_ncull(p_orig, p_dir, &p0, &p1, &p2, &t, &u, &v)) {
        if (t > 0 && t < *p_t) {        // nalezen blizsi prusecik         
          *p_o = o;
          *p_f = f;
          *p_t = t;
        }
      }
    }
  }

  return ((*p_t != FLT_MAX));
}

/* 
  Vrati nejblizsi prusecik sceny s paprskem a->b
  pokud existuje -> kont k, objekt o, face f, bod p
*/
int oe_intersect_scena(K_EDITOR * p_cnf, BOD * p_orig, BOD * p_dir,
  float *p_t, int *p_k, int *p_o, int *p_f)
{
  // kazdy kontejner ma kolem sebe kulovou obalku 
  float vzdal;
  int k;
  int f, o;

  *p_t = FLT_MAX;

  for (k = 0; k < p_cnf->max_kont; k++) {
    if (p_cnf->p_kont[k]
      /*&& p_cnf->p_kont[k]->kreslit/* && intersect_kont_fast(p_cnf->p_kont[k],p_orig,p_dir) */
      ) {
      if (intersect_kont(p_cnf->p_kont[k], p_orig, p_dir, &o, &f, &vzdal)
        && vzdal < *p_t) {
        *p_k = k;
        *p_o = o;
        *p_f = f;
        *p_t = vzdal;
      }
    }
  }
  return ((*p_t != FLT_MAX));
}

int oe_nejblizsi_bod_ve_scene(K_EDITOR * p_cnf, BOD * p_bod, int *p_k,
  int *p_o, int *p_f)
{
  EDIT_KONTEJNER *p_kont;
  EDIT_OBJEKT *p_obj;
  GLMATRIX *p_world;
  OBJ_VERTEX _1, _2, p, *p_vert;
  VEKTOR v, vp, vs;
  BOD _1t, _2t;
  float vel_v, vel_vs;
  float vel_n = 100000000;
  word f, n_f, *p_face;
  int kontnum = p_cnf->max_kont, objnum, facenum, k, ob, n_k, n_ob, hit = 0;

  for (k = 0; k < kontnum; k++) {
    p_kont = p_cnf->p_kont[k];
    if (!p_kont || !p_kont->kreslit)
      continue;

    p_world = kont_world_matrix(p_kont);
    transf_2d_3d_world((float) mi.x, (float) mi.y, 0, &_1.x, &_1.y, &_1.z,
      p_world, p_cnf->p_camera, p_cnf->p_project,
      OXRES, OYRES, OXSTART, OYSTART);
    transf_2d_3d_world((float) mi.x, (float) mi.y, 0.5f, &_2.x, &_2.y, &_2.z,
      p_world, p_cnf->p_camera, p_cnf->p_project,
      OXRES, OYRES, OXSTART, OYSTART);

    // transformace paprsku do objectspace

    _1t.x = _1.x;
    _1t.y = _1.y;
    _1t.z = _1.z;
    _2t.x = _2.x;
    _2t.y = _2.y;
    _2t.z = _2.z;
    transformuj_bod_matici(&_1t.x, &_1t.y, &_1t.z, p_world);
    transformuj_bod_matici(&_2t.x, &_2t.y, &_2t.z, p_world);
    calc_vect(_1t, _2t, v);
    vel_vect(v, vel_v)

      objnum = p_kont->max_objektu;
    for (ob = 0; ob < objnum; ob++) {
      p_obj = p_cnf->p_kont[k]->p_obj[ob];
      if (!p_obj || !p_obj->kreslit)
        continue;

      facenum = p_obj->facenum;
      p_vert = p_obj->p_vertex;
      p_face = p_obj->p_face;

      for (f = 0; f < facenum; f += 3) {
        if (prusek_face_editor(&_1, &_2, &p, p_vert + p_face[f],
            p_vert + p_face[f + 1], p_vert + p_face[f + 2])) {
          transformuj_bod_matici(&p.x, &p.y, &p.z, p_world);
          calc_vect(_1t, p, vp);
          add_vect(v, vp, vs);
          vel_vect(vs, vel_vs)

            if (vel_v < vel_vs && vel_vs < vel_n) {     // je pred kamerou
            vel_n = vel_vs;
            n_k = k;
            n_ob = ob;
            n_f = f;
            p_bod->x = p.x;
            p_bod->y = p.y;
            p_bod->z = p.z;
            hit++;
          }
        }
      }
    }
  }

  if (hit) {
    *p_k = n_k;
    *p_o = n_ob;
    *p_f = n_f;
    p_world = kont_world_matrix(p_cnf->p_kont[n_k]);
    transformuj_bod_matici(&p_bod->x, &p_bod->y, &p_bod->z, p_world);
  }

  return (hit);
}

/*
  vybere kont/objekt/face podle kurzoru mysi (mi)
  a zapise do p_cnf

  P = A + Q*t
  A - prvni bod
  Q - vektor AB
  t = 0..1
*/
int oe_prikaz_vyber_mysi_mesh(K_EDITOR * p_cnf)
{
  GLMATRIX *p_world;
  F_VERTEX face;
  EDIT_KONTEJNER *p_kont;
  EDIT_OBJEKT *p_obj, *p_kobj;
  OBJ_VERTEX _1, _2, p, *p_vert;
  BOD _1t, _2t, A, Q;
  float t, max_t = FLT_MAX, *p_P, *p_Q, *p_A;
  word f, n_f, *p_face;
  int kontnum = p_cnf->max_kont,
    objnum, facenum, k, ob, n_k, n_ob, hit = 0, o;


  for (k = 0; k < kontnum; k++) {
    p_kont = p_cnf->p_kont[k];
    if (!p_kont || !p_kont->kreslit)
      continue;

    p_world = kont_world_matrix(p_kont);

    if (!(p_kont->kflag & KONT_KEYFRAME)) {

      transf_2d_3d_world((float) mi.x, (float) mi.y, 0, &_1.x, &_1.y, &_1.z,
        p_world, p_cnf->p_camera,
        p_cnf->p_project, OXRES, OYRES, OXSTART, OYSTART);
      transf_2d_3d_world((float) mi.x, (float) mi.y, 0.5f, &_2.x, &_2.y,
        &_2.z, p_world, p_cnf->p_camera, p_cnf->p_project, OXRES, OYRES,
        OXSTART, OYSTART);

      _1t.x = _1.x;
      _1t.y = _1.y;
      _1t.z = _1.z;
      _2t.x = _2.x;
      _2t.y = _2.y;
      _2t.z = _2.z;
      transformuj_bod_matici(&_1t.x, &_1t.y, &_1t.z, p_world);
      transformuj_bod_matici(&_2t.x, &_2t.y, &_2t.z, p_world);

      // vektor 1->2
      A.x = _1t.x;
      A.y = _1t.y;
      A.z = _1t.z;
      Q.x = _2t.x - _1t.x;
      Q.y = _2t.y - _1t.y;
      Q.z = _2t.z - _1t.z;
    }

    objnum = p_kont->max_objektu;
    for (ob = 0; ob < objnum; ob++) {
      p_obj = p_cnf->p_kont[k]->p_obj[ob];
      if (!p_obj || !p_obj->kreslit)
        continue;


      if (p_kont->kflag & KONT_KEYFRAME) {
        transf_2d_3d_world((float) mi.x, (float) mi.y, 0, &_1.x, &_1.y, &_1.z,
          &p_obj->m, p_cnf->p_camera,
          p_cnf->p_project, OXRES, OYRES, OXSTART, OYSTART);
        transf_2d_3d_world((float) mi.x, (float) mi.y, 0.5f, &_2.x, &_2.y,
          &_2.z, &p_obj->m, p_cnf->p_camera, p_cnf->p_project, OXRES, OYRES,
          OXSTART, OYSTART);

        _1t.x = _1.x;
        _1t.y = _1.y;
        _1t.z = _1.z;
        _2t.x = _2.x;
        _2t.y = _2.y;
        _2t.z = _2.z;
        transformuj_bod_matici(&_1t.x, &_1t.y, &_1t.z, &p_obj->m);
        transformuj_bod_matici(&_2t.x, &_2t.y, &_2t.z, &p_obj->m);

        // vektor 1->2
        A.x = _1t.x;
        A.y = _1t.y;
        A.z = _1t.z;
        Q.x = _2t.x - _1t.x;
        Q.y = _2t.y - _1t.y;
        Q.z = _2t.z - _1t.z;
      }

      facenum = p_obj->facenum;
      p_vert = p_obj->p_vertex;
      p_face = p_obj->p_face;

      for (f = 0; f < facenum; f += 3) {
        if (prusek_face_editor(&_1, &_2, &p, p_vert + p_face[f],
            p_vert + p_face[f + 1], p_vert + p_face[f + 2])) {
          transformuj_bod_matici(&p.x, &p.y, &p.z, p_world);
          if (Q.x > Q.y && Q.x > Q.z) {
            p_P = &p.x;
            p_Q = &Q.x;
            p_A = &A.x;
          }
          else if (Q.y > Q.x && Q.y > Q.z) {
            p_P = &p.y;
            p_Q = &Q.y;
            p_A = &A.y;
          }
          else {
            p_P = &p.z;
            p_Q = &Q.z;
            p_A = &A.z;
          }
          t = (*p_P - *p_A) / (*p_Q);
          if (t > 0.0f && t < max_t) {
            max_t = t;
            n_k = k;
            n_ob = ob;
            n_f = f;
            hit++;
          }
        }
      }
    }
  }

  if (hit) {
    k = (p_cnf->vybr_kont != n_k || p_cnf->vybr_objekt != n_ob) ? 1 : 0;

    if (p_cnf->vybr_kont != n_k || p_cnf->vybr_objekt != n_ob) {
      p_cnf->vybr_kont_last = p_cnf->vybr_kont;
      p_cnf->vybr_objekt_last = p_cnf->vybr_objekt;
    }
    p_cnf->vybr_kont = n_k;
    p_cnf->vybr_objekt = n_ob;
    p_cnf->vybr_face = n_f;

    // pridej do seznamu vybranych plosek
    if (key[K_CTRL]) {
      face.k = n_k;
      face.o = n_ob;
      face.f = n_f;
      oe_pridej_group_face_list(p_cnf, &face, 1, p_cnf->groupflag);
      p_cnf->vybr_face_group =
        najdi_group_face_vertex(p_cnf->p_group_face, p_cnf->groupfacenum,
        &face, p_cnf->groupflag);
      if (p_cnf->vybr_face_group == K_CHYBA) {
        p_cnf->vybr_face_group = 0;
      }
      kprintf(FALSE, "Vyber face %d", p_cnf->vybr_face_group);
    }

    k = p_cnf->vybr_kont;
    o = p_cnf->vybr_objekt;
    f = p_cnf->p_kont[k]->p_obj[o]->material;
    p_obj = p_cnf->p_kont[k]->p_obj[o];

    kprintf(0, "Kontejner: %s Objekt %s", p_cnf->p_kont[k]->jmeno,
      p_cnf->p_kont[k]->p_obj[o]->jmeno);
    if (f != K_CHYBA && p_cnf->p_mat[f]) {
      kprintf(0, "Mat %d,%s, Txt %s", f, p_cnf->p_mat[f]->jmeno,
        p_cnf->p_mat[f]->textfile[0]);
      kprintf(0, "Facu %d Bodu %d", p_obj->facenum / 3, p_obj->vertexnum);
    }

    updatuj_obalku_kontejner(p_cnf->p_kont[k]);

//   p_kobj = oe_cti_joint_kont(p_cnf);
    p_kobj = NULL;
    p_cnf->jcf.k = k;
    p_cnf->jcf.o = o;
    if (p_kobj != p_obj || !p_cnf->jcf.p_joint_akt
      || !p_cnf->jcf.p_joint_animace_akt) {
      p_cnf->jcf.p_joint_animace_akt =
        p_cnf->p_kont[k]->p_obj[o]->p_joit_animace;
      p_cnf->jcf.p_joint_akt =
        p_cnf->jcf.p_joint_animace_akt ? p_cnf->jcf.
        p_joint_animace_akt->p_child : NULL;
    }

    doe_joint_animace_vloz_objekt(p_cnf);
    doe_joint_animace_vloz_frame(p_cnf);
    doe_joint_animace_vloz_tiky(p_cnf);
    doe_joint_animace_vloz_text(p_cnf);
  }

  return (hit);
}

void oe_zrus_vyber_kosti(K_EDITOR * p_cnf, EDIT_OBJEKT * p_obj)
{
  if (p_obj == oe_cti_joint_objekt(p_cnf) && p_cnf->jcf.k == p_cnf->vybr_kont
    && p_cnf->jcf.o == p_cnf->vybr_objekt) {
    p_cnf->jcf.p_joint_akt = NULL;
    p_cnf->jcf.p_joint_animace_akt = NULL;
  }
}

float oe_prusek_kost(BOD * p_pivot, BOD * _1, BOD * _2)
{
  return ((vzdal_bodu_a_primky(p_pivot, _1,
        _2) < RADIUS_KOSTI) ? vzdal_bodu_bod(p_pivot, _1) : FLT_MAX);
}

JOINT *oe_najdi_nejlepsi_joint(JOINT * p_joint, float *p_nej, BOD * _1,
  BOD * _2)
{
  JOINT *t1 = NULL, *t2 = NULL, *p_best;
  float t, max_t;

  if (!p_joint)
    return (NULL);

  if (p_joint->p_child) {
    t1 = oe_najdi_nejlepsi_joint(p_joint->p_child, p_nej, _1, _2);
  }
  if (p_joint->p_next) {
    t2 = oe_najdi_nejlepsi_joint(p_joint->p_next, p_nej, _1, _2);
  }
  p_best = t2 ? t2 : t1;

  max_t = *p_nej;
  while (p_joint) {
    t = oe_prusek_kost(&p_joint->pivot, _1, _2);
    if (t > 0.0f && t < max_t) {
      *p_nej = max_t = t;
      p_best = p_joint;
    }
    p_joint = p_joint->p_next;
  }

  return (p_best);
}

int oe_prikaz_vyber_mysi_kosti(K_EDITOR * p_cnf)
{
  JOINT *p_joint, *p_joint_best = NULL;
  GLMATRIX *p_world;
  int handle;
  BOD _1, _2;
  float t, tmin = FLT_MAX;

  handle = oe_vybr_obj_reset();
  if (handle) {
    EDIT_OBJEKT *p_obj =
      oe_vybr_obj_get_ind(&handle, &p_cnf->jcf.k, &p_cnf->jcf.o);
    EDIT_KONTEJNER *p_kont = oe_vybr_obj_get_kont(&handle);
    JOINT_ANIMACE *p_joint_animace = p_cnf->jcf.p_joint_animace_akt;

    if (!p_obj || !p_obj->kreslit || !p_joint_animace)
      return (FALSE);

    p_world = kont_world_matrix(p_kont);

    transf_2d_3d_world((float) mi.x, (float) mi.y, 0, &_1.x, &_1.y, &_1.z,
      p_world, p_cnf->p_camera, p_cnf->p_project, OXRES, OYRES, OXSTART,
      OYSTART);
    transf_2d_3d_world((float) mi.x, (float) mi.y, 0.5f, &_2.x, &_2.y, &_2.z,
      p_world, p_cnf->p_camera, p_cnf->p_project, OXRES, OYRES, OXSTART,
      OYSTART);

    t = FLT_MAX;
    p_joint = oe_najdi_nejlepsi_joint(p_joint_animace->p_child, &t, &_1, &_2);
    if (t < tmin) {
      tmin = t;
      p_joint_best = p_joint;
    }

    if (p_joint_best) {
      p_cnf->jcf.p_joint_akt = p_joint_best;
    }
    return ((int) p_joint_best);
  }
  return (FALSE);
}

/*
 // staticka svetla pro metodu ray-castingu
 int     lightnum;
 int     lightakt;
 STATIC_LIGHT light[MAX_EDIT_SVETEL];
*/
#define RADIUS_SVETLA 1.0f

int oe_prikaz_vyber_mysi_svetlo(K_EDITOR * p_cnf)
{
  BOD _1, _2, *p_pos;
  float t, tmin = FLT_MAX;
  int i;
  int best = K_CHYBA;
  GLMATRIX m;

  init_matrix(&m);

  transf_2d_3d_world((float) mi.x, (float) mi.y, 0, &_1.x, &_1.y, &_1.z,
    &m, p_cnf->p_camera, p_cnf->p_project, OXRES, OYRES, OXSTART, OYSTART);
  transf_2d_3d_world((float) mi.x, (float) mi.y, 0.5f, &_2.x, &_2.y, &_2.z,
    &m, p_cnf->p_camera, p_cnf->p_project, OXRES, OYRES, OXSTART, OYSTART);

  for (i = 0; i < MAX_EDIT_SVETEL; i++) {
    if (p_cnf->light[i].akt) {
      p_pos = &p_cnf->light[i].p;
      t =
        (vzdal_bodu_a_primky(p_pos, &_1,
          &_2) < RADIUS_SVETLA) ? vzdal_bodu_bod(p_pos, &_1) : FLT_MAX;
      if (t < tmin) {
        tmin = t;
        best = i;
      }
    }
  }

  if (best != K_CHYBA)
    p_cnf->lightakt = best;

  return (FALSE);
}

int oe_prikaz_vyber_mysi_bod(K_EDITOR * p_cnf, BOD * p_p)
{
  ROVINA r = { 0, 1.0f, 0, 0 };
  BOD _1, _2;

  transf_2d_3d_world((float) mi.x, (float) mi.y, 0, &_1.x, &_1.y, &_1.z,
    &p_cnf->init_matrix, p_cnf->p_camera, p_cnf->p_project,
    OXRES, OYRES, OXSTART, OYSTART);
  transf_2d_3d_world((float) mi.x, (float) mi.y, 0.5f, &_2.x, &_2.y, &_2.z,
    &p_cnf->init_matrix, p_cnf->p_camera, p_cnf->p_project,
    OXRES, OYRES, OXSTART, OYSTART);

  return (calc_prusek_bod(&r, &_1, &_2, p_p));
}

void oe_prikaz_prirad_material_objekt(K_EDITOR * p_cnf)
{
  int k = p_cnf->vybr_kont;
  int o = p_cnf->vybr_objekt;

  n_objekt(k, o);

  p_cnf->p_kont[k]->p_obj[o]->material = p_cnf->km.vybr_mat;
  p_cnf->p_kont[k]->m1flag |= (p_cnf->p_kont[k]->p_obj[o]->m1flag =
    p_cnf->p_mat[p_cnf->km.vybr_mat]->flag);
  p_cnf->p_kont[k]->m2flag |= (p_cnf->p_kont[k]->p_obj[o]->m2flag =
    p_cnf->p_mat[p_cnf->km.vybr_mat]->flag2);

  if (p_cnf->cnf.barvy_z_materialu) {
    oe_prikaz_reset_barva_objekt(p_cnf);
  }
}

void oe_prikaz_prirad_material_kontejner(K_EDITOR * p_cnf)
{
  int k = p_cnf->vybr_kont;
  EDIT_KONTEJNER *p_kont = p_cnf->p_kont[k];
  int o;

// int x,y;

  n_kont(k);

  if (!p_cnf->p_mat[p_cnf->km.vybr_mat])
    return;

  for (o = 0; o < p_kont->max_objektu; o++) {
    if (p_kont->p_obj[o]) {
      p_kont->p_obj[o]->material = p_cnf->km.vybr_mat;
      p_kont->m1flag |= (p_kont->p_obj[o]->m1flag =
        p_cnf->p_mat[p_cnf->km.vybr_mat]->flag);
      p_kont->m2flag |= (p_kont->p_obj[o]->m2flag =
        p_cnf->p_mat[p_cnf->km.vybr_mat]->flag2);
    }
  }

  if (p_cnf->cnf.barvy_z_materialu) {
    oe_prikaz_reset_barva_kontejner(p_cnf);
  }
}

void oe_prikaz_kapatko(K_EDITOR * p_cnf)
{
  int k = p_cnf->vybr_kont;
  int o = p_cnf->vybr_objekt;

  n_objekt(k, o);
  p_cnf->km.vybr_mat = p_cnf->p_kont[k]->p_obj[o]->material;
}

void oe_mys_editace(K_EDITOR * p_cnf)
{
  if (mi.t1) {

    if (p_cnf->cnf.oznacovat & OZNACOVAT_MESH) {
      oe_prikaz_vyber_mysi_mesh(p_cnf);
      doe_updatuj_lista_objekty(p_cnf);
    }
    if (p_cnf->cnf.oznacovat & OZNACOVAT_KOSTI) {
      if (oe_prikaz_vyber_mysi_kosti(p_cnf)) {
        doe_joint_animace_vloz_objekt(p_cnf);
        doe_joint_animace_vloz_frame(p_cnf);
        doe_joint_animace_vloz_tiky(p_cnf);
        doe_joint_animace_vloz_text(p_cnf);
      }
    }
    if (p_cnf->cnf.oznacovat & OZNACOVAT_SVETLA) {
      oe_prikaz_vyber_mysi_svetlo(p_cnf);
    }

    if (key[K_SLASH] && !key[K_CTRL]) {
      doe_proved_prikaz_menu(ID_PRIKAZ_PRIRAD_MATERIAL_OBJEKT, TRUE, FALSE);
      mi.t1 = FALSE;
    }

    if (key[K_SLASH] && key[K_CTRL]) {
      key[K_SLASH] = FALSE;
      doe_proved_prikaz_menu(ID_PRIKAZ_PRIRAD_MATERIAL_KONTEJNER, TRUE,
        FALSE);
      mi.t1 = FALSE;
    }
  }
}


void oe_get_souradnice_mysi_2d(K_EDITOR * p_cnf, BOD * p_bod)
{
  BOD _1, _2;
  GLMATRIX m;
  ROVINA r;

  init_matrix(&m);

  r.e = 0;
  r.x = 0;
  r.y = 1;
  r.z = 0;

  transf_2d_3d_world((float) mi.x, (float) mi.y, 0, &_1.x, &_1.y, &_1.z,
    &m, p_cnf->p_camera, p_cnf->p_project, OXRES, OYRES, OXSTART, OYSTART);
  transf_2d_3d_world((float) mi.x, (float) mi.y, 0.5f, &_2.x, &_2.y, &_2.z,
    &m, p_cnf->p_camera, p_cnf->p_project, OXRES, OYRES, OXSTART, OYSTART);

  calc_prusek_bod(&r, &_1, &_2, p_bod);
}


void oe_vyber_primku(K_EDITOR * p_cnf)
{
  int i, k, o, v;

  if (p_cnf->groupnum == 0) {
    ddw("Musi byt oznacene 2 body !");
    return;
  }

  oe_eliminuj_stejne_group(p_cnf);

  if (p_cnf->groupnum != 2) {
    ddw("Spatny pocet bodu.");
    return;
  }

  for (i = 0; i < 2; i++) {
    k = p_cnf->p_group[i].k;
    o = p_cnf->p_group[i].o;
    v = p_cnf->p_group[i].v;
    p_cnf->primka[i].x = p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].x;
    p_cnf->primka[i].y = p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].y;
    p_cnf->primka[i].z = p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].z;
  }
  oe_prikaz_all_ungroup_vertex(p_cnf);
}


void oe_klavesy_object_editor(K_EDITOR * p_cnf)
{
  if (key[K_ESC]) {
    key[K_ESC] = FALSE;
    oe_konec(&cnf);
  }
  if (key[K_SPACE]) {
    key[K_SPACE] = FALSE;
    doe_proved_prikaz_menu(p_cnf->cnf.posledni_prikaz, TRUE, TRUE);
  }
  if (key[K_BKSP] && key[K_CTRL]) {
    key[K_BKSP] = FALSE;
    doe_proved_prikaz_menu(ID_K_KONFIGURACE_UNDO, TRUE, FALSE);
  }
  oe_mys_editace(&cnf);
  oe_klavesy_editace_facu(p_cnf);
  oe_klavesy_editace_kontejneru(p_cnf);
  oe_klavesy_editace_objektu(p_cnf);
  oe_klavesy_editace_vertexu(p_cnf);
  oe_klavesy_vyroba_objektu(p_cnf);
  oe_klavesy_v_game_modu(p_cnf);
  oe_klavesy_zmena_modu(p_cnf);
  oe_klavesy_load_save(p_cnf);
  oe_klavesy_konfigurace(p_cnf);
  oe_klavesy_textury(p_cnf);
  oe_klavesy_test(p_cnf);
  oe_klavesy_editace_svetel(p_cnf);
}

void go_object_editor(char *p_edit_kont, char *p_text_dir)
{
  byte pom[100];
  int k;

  oe_konfiguruj_editor(&cnf, INI_FILE, FALSE);
  doe_nastav_k_editor(&cnf);

  if (akt_editor == 2)
    doe_prepni_do_game_editor();
  else
    doe_prepni_do_mesh_editor();

  // parametr je out - nahraj ten kontejner
  _strlwr(p_edit_kont);
  if (p_edit_kont[0] && strstr(p_edit_kont, KONCOVKA_MESH)) {
    if ((k = lo_najdi_volny_kontejner(cnf.p_kont, cnf.max_kont)) == K_CHYBA) {
      ddw("Neni volna pozice !");
      return;
    }
    strcpy(pom, cnf.dir.out_dir);
    strcpy(cnf.dir.out_dir, work_dir);
    if ((cnf.p_kont[k] =
        oe_nahraj_kontejner_bez_dialog(&cnf, p_edit_kont, TRUE)) == NULL) {
      ddw("Sakra, nejaka chyba loadu ! file (%s)", p_edit_kont);
    }
    else {
      strcpy(cnf.p_kont[k]->jmeno, p_edit_kont);
    }
    strcpy(cnf.dir.out_dir, pom);
  }

  // parametr se sc2 - nahraj scenu
  if (p_edit_kont[0] && strstr(p_edit_kont, KONCOVKA_SCENY)) {
    oe_nahraj_projekt(&cnf, p_edit_kont, work_dir, FALSE, TRUE);
  }

  // parametr je 3ds - nahraj 3ds soubor
  if (p_edit_kont[0] && strstr(p_edit_kont, ".3ds")) {
    if ((k = lo_najdi_volny_kontejner(cnf.p_kont, cnf.max_kont)) == K_CHYBA) {
      ddw("Neni volna pozice !");
      return;
    }
    strcpy(pom, cnf.dir.out_dir);
    strcpy(cnf.dir.out_dir, work_dir);
    if ((cnf.p_kont[k] =
        oe_nahraj_kontejner_bez_dialog_3ds(&cnf, p_edit_kont,
          TRUE)) == NULL) {
      ddw("Sakra, nejaka chyba loadu ! file (%s)", p_edit_kont);
    }
    else {
      strcpy(cnf.p_kont[k]->jmeno, p_edit_kont);
      if (cnf.cnf.keyframe_kont) {
        if (ds_kontejner_rozloz(&cnf, cnf.p_kont[k], ROZLOZ_MAX))
          zrus_kontejner(cnf.p_kont + k, &p_cnf->p_mlha);
      }
    }
    strcpy(cnf.dir.out_dir, pom);
  }

  render_povolen = TRUE;

  while (spracuj_spravy(TRUE)) {

    if (akt_editor == 1)
      oe_klavesy_object_editor(&cnf);
    else
      be_klavesy_berusky_editor(&cnf.bnf);

    oe_renderuj_scenu_pre(&cnf);
    oe_renderuj_scenu_po(&cnf);
  }
}

int go_renderovac_renderuj(char *p_file, char *p_out)
{
  kprintf(TRUE, "Loaduji level %s...", p_file);
  if (be_nahraj_level_render(&cnf, p_file)) {
    kprintf(TRUE, "Renderuji level %s...", p_file);
    ray_prepocitej_staticke_osvetleni(p_cnf, TRUE);
    kprintf(TRUE, "Ukladam level %s...", p_file);
    oe_uloz_berusci_level(p_cnf, p_out);
    kprintf(TRUE, "Mazu level %s...", p_file);
    be_prikaz_smaz_level(p_cnf, TRUE, TRUE, TRUE);
    lo_vymaz_materialy(p_cnf->p_mat, MAX_EDIT_MATERIALU,
      p_cnf->p_text, MAX_EDIT_TEXTUR);
    return (TRUE);
  }
  else {
    return (FALSE);
  }
}

/* Renderovac levelu
*/
void go_renderovac(char *p_file)
{
  char level[200];
  char level_out[200];
  int l;
  FILE *f;

  chdir(work_dir);
  f = fopen(p_file, "r");
  if (!f) {
    kprintf(TRUE, "Chyba otevreni list-souboru %s", p_file);
    return;
  }

  if (!fgets(level, 200, f)) {
    kprintf(TRUE, "Chyba loadu list-souboru %s - pocet levelu", p_file);
    return;
  }
  l = atoi(level);

  while (fgets(level, 200, f)) {
    fgets_korekce(level);
    zamen_koncovku(level, KONCOVKA_LEVELU);
    sprintf(level_out, "level%d", l++);
    go_renderovac_renderuj(level, level_out);
  }

  fclose(f);
}

void oe_updatuj_fps(K_EDITOR * p_cnf)
{
  static dword akt = 0;
  static dword end = 0;
  static dword framu = 0;
  static dword time;

  time = timeGetTime();
  p_cnf->TimeLastFrame = time - p_cnf->TimeEndLastFrame;
  p_cnf->TimeEndLastFrame = time;

  framu++;
  if (time - end > 1000) {
    p_cnf->fps = (1000 * framu) / (float) (time - end);
    end = time;
    framu = 0;
  }
}

void oe_konec(K_EDITOR * p_cnf)
{
  oe_uloz_konfiguraci(p_cnf, INI_DIR, INI_FILE, FALSE);
  konec();
}

void ddw(const char * p_text, ...)
{
  byte text[500];
  va_list argumenty;

  va_start(argumenty, p_text);
  vsprintf(text, p_text, argumenty);
  va_end(argumenty);

  MessageBox(hwnd_aplikace, text, "DDW:",
    MB_ICONASTERISK | MB_OK | MB_SYSTEMMODAL);
}

void oe_prikaz_vypis_face(K_EDITOR * p_cnf)
{
  int i;

  for (i = 0; i < p_cnf->groupfacenum; i++) {
    kprintf(TRUE, "%d k[%d] o[%d] f[%d]", i,
      p_cnf->p_group_face[i].k,
      p_cnf->p_group_face[i].o, p_cnf->p_group_face[i].f);
  }
  doe_konzole_max(p_cnf);
}

/*
  Projit vsechny rutiny chyba a chybat -> skusit nahradit ddw + return
  - loga a pod.
*/
int oe_scale_sceny(K_EDITOR * p_cnf, float scale)
{
  EDIT_KONTEJNER *p_kont;
  int k;

  oe_klist_reset(&k);
  while ((p_kont = oe_klist_next(p_cnf, &k))) {
    if (p_kont->kflag & KONT_PRVEK)
      continue;
    scale_matrix(&p_kont->world, scale, scale, scale);
    //key_kontejner_reanimuj(p_kont);
  }
  return (TRUE);
}


void oe_prikaz_scale_sceny(K_EDITOR * p_cnf)
{
  float scale;

  if (doe_dialog_scale("Scale sceny",
      "Validni je prvni cislo - symetricky scale", &scale)) {
    oe_scale_sceny(p_cnf, scale);
  }
}

/* Posun sceny
*/
void oe_prikaz_posun_scenu(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  GLMATRIX mat;
  VEKTOR v1, v2, v;
  int k, flag;

  oe_transformuj_scenu_staticke(p_cnf);

  calc_transf_3d_2d_matrix(&p_cnf->init_matrix, p_cnf->p_camera,
    p_cnf->p_project, &mat);
  *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);
  transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
    &v1.x, &v1.y, &v1.z, &mat, OXRES, OYRES, OXSTART, OYSTART,
    p_cnf->kam.near_plane, p_cnf->kam.far_plane);

  nuluj_mys();
  while (stisk_mys()) {
    transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
      &v2.x, &v2.y, &v2.z, &mat, OXRES, OYRES, OXSTART, OYSTART,
      p_cnf->kam.near_plane, p_cnf->kam.far_plane);

    vektor_sub(&v2, &v1, &v);
    flag = 0;
    if (!key[K_X] && !key[K_Y] && !key[K_Z]) {
      flag = POSUN_VSE;
    }
    else {
      if (key[K_X]) {
        v.y = v.z = 0.0f;
        flag = POSUN_OSA_X;
      }
      if (key[K_Y]) {
        v.x = v.z = 0.0f;
        flag = POSUN_OSA_Y;
      }
      if (key[K_Z]) {
        v.x = v.y = 0.0f;
        flag = POSUN_OSA_Z;
      }
    }

    oe_klist_reset(&k);
    while ((p_kont = oe_klist_next(p_cnf, &k))) {
      if (p_kont->kflag & KONT_PRVEK)
        continue;
      translate_matrix(&p_kont->world, v.x, v.y, v.z);
      //key_kontejner_reanimuj(p_kont);
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

int oe_je_group_kontejner(K_EDITOR * p_cnf)
{
  return (p_cnf->groupflag & GROUP_KONT
    && !(p_cnf->groupflag & (GROUP_OBJ | GROUP_FACE)));
}

int oe_je_group_objekt(K_EDITOR * p_cnf)
{
  return (p_cnf->groupflag & GROUP_KONT &&
    p_cnf->groupflag & GROUP_OBJ && !(p_cnf->groupflag & GROUP_FACE));
}

int oe_je_group_face(K_EDITOR * p_cnf)
{
  return (p_cnf->groupflag & GROUP_KONT &&
    p_cnf->groupflag & GROUP_OBJ && p_cnf->groupflag & GROUP_FACE);
}

void oe_vypis_poly(K_EDITOR * p_cnf)
{
  EDIT_MESH_POLY *p_poly;
  int i, p;

  for (p = 0; p < p_cnf->polynum; p++) {
    p_poly = p_cnf->p_poly + p;
    for (i = 0; i < p_poly->facenum; i++) {
      kprintf(TRUE, "map1 [%f %f] map2 [%f %f] mapl [%f %f]",
        p_poly->p_koord[i].tu1, p_poly->p_koord[i].tv1,
        p_poly->p_koord[i].tu2, p_poly->p_koord[i].tv2,
        p_poly->p_koord[i].tul, p_poly->p_koord[i].tvl);
    }
  }
}
