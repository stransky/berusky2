/*
  Berusky loaders
*/
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <alloca.h>

#include "3d_all.h"

#include "Object.h"

#include "Berusky_universal.h"
#include "Berusky3d_castice.h"
#include "Berusky3d.h"
#include "Berusky3d_ini.h"
#include "Berusky3d_load.h"
#include "Berusky3d_animace.h"
#include "Berusky3d_kofola_interface.h"
#include "Berusky3d_render.h"

#include "json_export.h"

/* Voda ma guid 12
*/
int ber_je_objekt_voda(int guid)
{
  return (guid / 1000 == 12);
}

/*
  Prace s levelem (vyrobeni/zruseni)
*/
void ber_vyrob_level(G_KONFIG * p_ber, int x, int y, int z)
{
  p_ber->x_num = x;
  p_ber->y_num = y;
  p_ber->z_num = z;
  p_ber->prvnum = 0;
}

void ber_zrus_level(G_KONFIG * p_ber)
{
  int i;

  // vymazni vsechny prvky
  for (i = 0; i < MAX_PRVKU_LEVEL; i++) {
    if (p_ber->p_prv_lev[i]) {
      free(p_ber->p_prv_lev[i]);
      p_ber->p_prv_lev[i] = NULL;
    }
  }

  // vymazni level
  p_ber->z_num = p_ber->y_num = p_ber->x_num = 0;
}

/*
  Loady levelu
*/
PRVEK_LEVELU_GAME *ber_vyrob_prvek_levelu(void)
{
  PRVEK_LEVELU_GAME *p_prv =
    (PRVEK_LEVELU_GAME *) mmalloc(sizeof(PRVEK_LEVELU_GAME));
  init_matrix(&p_prv->mg);
  init_matrix(&p_prv->mp);
  return (p_prv);
}

/* Load meshe
*/
int ber_najdi_mesh_id(GAME_MESH_OLD ** p_mesh, int max, int id)
{
  int i;

  for (i = 0; i < max; i++) {
    if (p_mesh[i] && p_mesh[i]->p_data->mesh_ID == id)
      return (i);
  }
  return (K_CHYBA);
}

/* Prida regulerni mesh z disku
*/
int ber_prvek_disk_do_bunky(G_KONFIG * p_ber, BUNKA_LEVELU_DISK * p_disk,
  int *p_handle, int ID, int x, int y, int z)
{
  PRVEK_LEVELU_GAME *p_lev;
  GAME_MESH_OLD *p_mesh;
  GLMATRIX *p_m, tmp1;
  int guid = p_disk->guid;
  int mesh;
  int i;

  if (!guid) {
    return (TRUE);
  }

  for (i = 0; i < MAX_PRVKU_LEVEL; i++) {
    if (!p_ber->p_prv_lev[i]) {
      p_lev = p_ber->p_prv_lev[i] = ber_vyrob_prvek_levelu();
      p_lev->guid = p_disk->guid;
      p_lev->rotace = p_disk->Rotace;
      p_lev->staticky = ber_je_objekt_staticky(p_disk->guid);

      if (p_lev->staticky && !ber_je_objekt_voda(p_disk->guid)) {
        assert(ber_pozice_v_levelu(x, y >> 1, z, p_ber->x_num,
            p_ber->z_num) < p_ber->hitnum);
        p_ber->p_hit_list[ber_pozice_v_levelu(x, y >> 1, z, p_ber->x_num,
            p_ber->z_num)] = TRUE;
      }

      if (ID && p_disk->mesh != -1) {
        mesh = ber_najdi_mesh_id(p_ber->p_mesh, p_ber->meshnum, p_disk->mesh);
        if (mesh == K_CHYBA) {
          mesh = p_disk->mesh;
        }
      }
      else {
        mesh = p_disk->mesh;
      }

      p_lev->mesh = mesh;

      if (mesh != K_CHYBA && p_ber->p_mesh[mesh]) {
        p_mesh = p_ber->p_mesh[mesh];
        p_lev->p_mesh_data = p_mesh->p_data;
        p_lev->mp = p_mesh->m;
        p_m = &p_lev->mp;

        // Nacti umisteni meshe na mape
        p_lev->x = p_m->_41;
        p_lev->y = p_m->_42 + (Y_PRVEK / 2);    // stred prvku je vyse
        p_lev->z = p_m->_43;
        p_lev->rotace = p_disk->Rotace;

        // Vyrob obalku prvku ve world-space
        transformuj_bod_matici(&p_lev->x, &p_lev->y, &p_lev->z, &p_mesh->m);
        init_matrix(&p_lev->mg);

        // Nastav pozici prvku
        if (p_mesh->p_data->kflag & KONT_KEYFRAME) {
          mat_mult_dir(&p_mesh->world, &p_mesh->m, &tmp1);
          key_mesh_reanimuj(p_mesh, p_mesh->sim_aktivni_time, &tmp1);
        }
        else {
          key_mesh_transformuj_obalky(p_mesh, &p_mesh->m);
        }


        p_mesh->p_data->kflag |= KONT_POHYB;
        p_mesh->p_data->mesh_handle = p_disk->mesh;
      }
      else {
        p_lev->mesh = K_CHYBA;
      }
      *p_handle = i;
      p_ber->prvnum++;
      return (TRUE);
    }
  }
  return (FALSE);
}

/*
  #define ber_pozice_v_levelu(   _x,_y,_z,_x_num,_z_num)
  #define ber_pozice_na_disku(_i,_x,_y,_z,_x_num,_z_num)
*/

/* Prida divoky mesh
*/
int ber_mesh_do_bunky(G_KONFIG * p_ber, int mesh, int *p_handle)
{
  PRVEK_LEVELU_GAME *p_lev;
  GAME_MESH_OLD *p_mesh = p_ber->p_mesh[mesh];
  GLMATRIX *p_m, tmp1;
  int i;

  if (!p_ber->p_mesh[mesh]) {
    return (K_CHYBA);
  }

  for (i = 0; i < MAX_PRVKU_LEVEL; i++) {
    if (!p_ber->p_prv_lev[i]) {
      p_lev = p_ber->p_prv_lev[i] = ber_vyrob_prvek_levelu();
      p_lev->mesh = mesh;
      p_mesh = p_ber->p_mesh[mesh];
      p_lev->p_mesh_data = p_mesh->p_data;
      p_lev->mp = p_mesh->m;
      p_m = &p_lev->mp;
      p_lev->pivot = p_mesh->kofola_pivot;

      init_matrix(&p_lev->mg);
      if (p_mesh->p_data->kflag & KONT_KEYFRAME) {
        mat_mult_dir(&p_mesh->world, &p_mesh->m, &tmp1);
        key_mesh_reanimuj(p_mesh, p_mesh->sim_aktivni_time, &tmp1);
      }
      else {
        key_mesh_transformuj_obalky(p_mesh, &p_mesh->m);
      }

      p_mesh->p_data->kflag |= KONT_POHYB;
      p_mesh->p_data->mesh_handle = mesh;
      *p_handle = i;
      p_ber->prvnum++;
      return (TRUE);
    }
  }
  return (FALSE);
}

void ber_umisti_prvek(MeshHandle handle, int x, int y, int z, int rotace)
{
  PRVEK_LEVELU_GAME *p_lev = p_ber->p_prv_lev[handle];
  GAME_MESH_OLD *p_mesh;
  GLMATRIX *p_m, tmp1;

  if (p_lev) {
    p_m = &p_lev->mp;

    if (ber_je_objekt_staticky(p_lev->guid)) {

      p_lev->x = (float) (p_ber->x_start + x * X_PRVEK + X_PRVEK / 2);
      p_lev->y = (float) (p_ber->y_start + (y >> 1) * Y_PRVEK + Y_PRVEK / 2);
      p_lev->z = (float) (p_ber->z_start + z * Z_PRVEK + Z_PRVEK / 2);
      p_lev->rotace = rotace;

      init_matrix(&p_lev->mp);
      init_matrix(&p_lev->mg);

      if (p_lev->mesh != K_CHYBA) {
        p_mesh = p_ber->p_mesh[p_lev->mesh];
        init_matrix(&p_mesh->m);
        key_mesh_transformuj_obalky(p_mesh, &p_mesh->m);
      }
    }
    else {

      ber_position_matrix(p_ber, p_m, x, y, z, rotace);
      ber_position_level(p_lev, p_m, rotace);

      if (p_lev->mesh != K_CHYBA) {
        p_mesh = p_ber->p_mesh[p_lev->mesh];

        p_mesh->m = p_lev->mp;
        init_matrix(&p_lev->mg);

        p_mesh->p_data->kflag |= KONT_POHYB;

        if (p_mesh->p_data->kflag & KONT_KEYFRAME) {
          mat_mult(&p_mesh->world, &p_mesh->m, &tmp1);
          key_mesh_reanimuj(p_mesh, p_mesh->sim_aktivni_time, &tmp1);
        }
        else {
          key_mesh_transformuj_obalky(p_mesh, &p_mesh->m);
        }
      }
    }
  }
}

void ber_umisti_prvek_abs(PRVEK_LEVELU_GAME * p_lev, float x, float y,
  float z, int rotace)
{
  GAME_MESH_OLD *p_mesh;
  GLMATRIX *p_m, tmp1;

  if (p_lev) {
    p_m = &p_lev->mp;

    ber_position_matrix_abs(p_ber, p_m, x, y, z, rotace);
    ber_position_level(p_lev, p_m, rotace);

    if (p_lev->mesh != K_CHYBA) {
      p_mesh = p_ber->p_mesh[p_lev->mesh];

      p_mesh->m = p_lev->mp;
      init_matrix(&p_lev->mg);

      p_mesh->p_data->kflag |= KONT_POHYB;

      if (p_mesh->p_data->kflag & KONT_KEYFRAME) {
        mat_mult(&p_mesh->world, &p_mesh->m, &tmp1);
        key_mesh_reanimuj(p_mesh, p_mesh->sim_aktivni_time, &tmp1);
      }
      else {
        key_mesh_transformuj_obalky(p_mesh, &p_mesh->m);
      }
    }
  }
}

inline static int _najdi_cislo(char *p_string)
{
  char *pom = (char *) alloca(strlen(p_string));
  char *p_pom, *p_last;
  int k;

  strcpy(pom, p_string);
  p_pom = pom;

  while ((p_pom = strchr(p_pom, '_'))) {
    p_last = ++p_pom;
  }

  sscanf(p_last, "%d%s", &k, KONCOVKA_MESH);
  return (k);
}

/*
  Nahraje materialy
*/
int ber_nahraj_materialy(G_KONFIG * p_ber, char *p_jmeno, char *p_dir)
{
  char pom[200];

  strcpy(pom, p_jmeno);
  zamen_koncovku(pom, KONCOVKA_MATERIAL);

  chdir((p_dir));
  if (!lo_nahraj_materialy(p_ber->p_mat, MAX_CELKEM_MATERIALU, p_ber->p_text,
      MAX_CELKEM_TEXTUR, pom)) {
    kprintf(1, "Unable to load materials %s", p_jmeno);
    return (FALSE);
  }
  else
    return (TRUE);
}

/*
  Rozkopiruje materialy -> pouze prvni material!!!
*/
void ber_materialy_rozkopiruj(G_KONFIG * p_ber, GAME_MESH_OLD * p_mesh,
  int restart)
{
  EDIT_MATERIAL **p_imat = p_ber->p_mat;
  EDIT_MATERIAL *p_mat;
  char pom[MAX_JMENO];
  int mat, mnew = K_CHYBA;
  int i;

  if (p_mesh->p_data->m1flag & MAT_ANIM_FRAME) {
    for (i = 0; i < p_mesh->objektu; i++) {
      mat = p_mesh->p_mat[i];
      p_mat = p_imat[mat];
      if (p_mat->flag & MAT_ANIM_FRAME) {
        if (restart) {
          sprintf(pom, "%s_%d", p_mat->jmeno, i);
          mnew = lo_najdi_material(p_ber->p_mat, MAX_CELKEM_MATERIALU, pom);
          assert(mnew != K_CHYBA);
          p_mesh->p_mat[i] = mnew;
        }
        else {
          mnew =
            lo_najdi_prazdny_material(p_ber->p_mat, MAX_CELKEM_MATERIALU);
          assert(mnew != K_CHYBA);

          p_imat[mnew] = kopiruj_material(p_mat);
          p_mesh->p_mat[i] = mnew;
          sprintf(p_imat[mnew]->jmeno, "%s_%d", p_mat->jmeno, i);
        }
      }
    }
  }
}

/* Nahraje jeden mesh
*/
int ber_nahraj_mesh(G_KONFIG * p_ber, char *p_jmeno, GAME_MESH_OLD ** p_mesh)
{
  chdir((p_ber->dir.out_dir));
  p_mesh[0] = lo_nahraj_mesh(p_ber->p_mat, MAX_CELKEM_MATERIALU, p_ber->p_text,
                             MAX_CELKEM_TEXTUR, p_jmeno, TRUE, 
                             p_ber->conf_extra_light_vertex);
  return ((int) p_mesh[0]);
}

inline int najdi_volnou_texturu_mat(EDIT_MATERIAL * p_mat)
{
  int i;

  for (i = 0; i < MAT_TEXTUR; i++) {
    if (!p_mat->textfile[i][0]) {
      return (i);
    }
  }
  return (K_CHYBA);
}

#define AUTO_STAGE 0

int ber_mat_vloz_dot3(EDIT_TEXT * p_text, int textnum, EDIT_MATERIAL * p_mat)
{
  MATERIAL_TEXT_OLD state_stary;
  int btext, t, mat_text;

  // Nahradim prvni blok dot-3 a skopiruju operaci    
  if ((t = najdi_volnou_texturu_mat(p_mat)) == K_CHYBA)
    return (FALSE);
  state_stary = p_mat->text_state[AUTO_STAGE];
  if (text_stage_edit_blok[state_stary.text_stage].textur > 1)
    return (FALSE);

  p_mat->text_state[AUTO_STAGE].text_stage = AUTO_BUMP_DOT3;
  p_mat->text_state[AUTO_STAGE].textury[DOT3_TEXT] = mat_text =
    state_stary.textury[0];
  p_mat->text_state[AUTO_STAGE].text_funkce[DOT3_TEXT] =
    state_stary.text_funkce[0];
  p_mat->text_state[AUTO_STAGE].text_koord[DOT3_TEXT] =
    state_stary.text_koord[0];

  p_mat->text_state[AUTO_STAGE].textury[DOT3_BUMP] = t;
  p_mat->text_state[AUTO_STAGE].text_funkce[DOT3_BUMP] = BLOK_DOT3_BUMP;
  p_mat->text_state[AUTO_STAGE].text_koord[DOT3_BUMP] =
    state_stary.text_koord[0];

  // Pridam texturu stejneho jmena ale dot-3  
  if ((btext =
      lo_najdi_texturu(p_text, textnum, p_mat->textfile[mat_text],
        TEXT_GEN_DOT3)) == K_CHYBA) {
    if ((btext = lo_najdi_volnou_texturu(p_text, textnum)) == K_CHYBA)
      chyba("Textury");
    txt_kopiruj(p_text + btext, p_mat->p_text[mat_text]);
    p_text[btext].flag = TEXT_GEN_DOT3;
    txt_to_dot3(p_text + btext, FALSE);
  }

  p_mat->textflag[t] = TEXT_GEN_DOT3;
  p_mat->p_text[t] = p_text + btext;
  strcpy(p_mat->textfile[t], p_mat->textfile[mat_text]);

  p_mat->flag2 |= MAT2_BUMP;

  return (TRUE);
}

#define BUMP_ENV_TEXT "bumpflare.bmp"

int ber_mat_vloz_dot3_env(EDIT_TEXT * p_text, int textnum,
  EDIT_MATERIAL * p_mat)
{
  MATERIAL_TEXT_OLD state_stary;
  int btext, t, mat_text;

  /* Nahradim prvni blok dot-3 a skopiruju operaci    
   */
  state_stary = p_mat->text_state[AUTO_STAGE];
  if (text_stage_edit_blok[state_stary.text_stage].textur > 1)
    return (FALSE);

  p_mat->text_state[AUTO_STAGE].text_stage = AUTO_BUMP_DOT3ENV;
  p_mat->text_state[AUTO_STAGE].textury[DOT3_TEXT] = mat_text =
    state_stary.textury[0];
  p_mat->text_state[AUTO_STAGE].text_funkce[DOT3_TEXT] =
    state_stary.text_funkce[0];
  p_mat->text_state[AUTO_STAGE].text_koord[DOT3_TEXT] =
    state_stary.text_koord[0];

  // Pridam bump-mapu
  if ((t = najdi_volnou_texturu_mat(p_mat)) == K_CHYBA) {
    p_mat->text_state[AUTO_STAGE] = state_stary;
    return (FALSE);
  }

  p_mat->text_state[AUTO_STAGE].textury[DOT3_BUMP] = t;
  p_mat->text_state[AUTO_STAGE].text_funkce[DOT3_BUMP] = BLOK_DOT3_BUMP;
  p_mat->text_state[AUTO_STAGE].text_koord[DOT3_BUMP] =
    state_stary.text_koord[0];

  // Pridam texturu stejneho jmena ale dot-3  
  if ((btext =
      lo_najdi_texturu(p_text, textnum, p_mat->textfile[mat_text],
        TEXT_GEN_DOT3)) == K_CHYBA) {
    if ((btext = lo_najdi_volnou_texturu(p_text, textnum)) == K_CHYBA)
      chyba("Textury");

    txt_kopiruj(p_text + btext, p_mat->p_text[mat_text]);
    p_text[btext].flag = TEXT_GEN_DOT3;
    txt_to_dot3(p_text + btext, FALSE);
  }

  p_mat->textflag[t] = TEXT_GEN_DOT3;
  p_mat->p_text[t] = p_text + btext;
  strcpy(p_mat->textfile[t], p_mat->textfile[mat_text]);

  // Pridam env mapu  
  if ((t = najdi_volnou_texturu_mat(p_mat)) == K_CHYBA) {
    p_mat->text_state[AUTO_STAGE] = state_stary;
    return (FALSE);
  }

  p_mat->text_state[AUTO_STAGE].textury[DOT3_ENV] = t;
  p_mat->text_state[AUTO_STAGE].text_funkce[DOT3_ENV] = BLOK_MODULACE;
  p_mat->text_state[AUTO_STAGE].text_koord[DOT3_ENV] =
    state_stary.text_koord[0];

  if ((btext =
      lo_najdi_texturu(p_text, textnum, BUMP_ENV_TEXT, FALSE)) == K_CHYBA) {
    if ((btext = lo_najdi_volnou_texturu(p_text, textnum)) == K_CHYBA)
      chyba("Textury");
    strcpy(p_text[btext].jmeno, BUMP_ENV_TEXT);
    p_text[btext].flag = FALSE;
  }

  p_mat->textflag[t] = FALSE;
  p_mat->p_text[t] = p_text + btext;
  p_mat->flag2 |= glstav_texgen_on[t];
  strcpy(p_mat->textfile[t], BUMP_ENV_TEXT);

  p_mat->flag2 |= MAT2_BUMP;

  return (TRUE);
}

// SMAT je bez bump-mappingu!!!
int ber_uprava_materialu_bump_mapping(G_KONFIG * p_ber)
{
  int matnum, i, bmptyp = hwconf.bump_mapping_typ, trida;
  EDIT_MATERIAL *p_mat;

  matnum = lo_pocet_materialu(p_ber->p_mat, MAX_CELKEM_MATERIALU);
/*
  for(i = 0; i < MAX_CELKEM_TEXTUR; i++) {
    if(p_ber->p_text[i].load && p_ber->p_text[i].bump) {
      trida = p_ber->p_text[i].trida;
      if(txconf.text_bump[trida]) {
        txt_trida(trida);
        if(p_mat = lo_najdi_material_text_point(p_ber->p_mat,matnum,p_ber->p_text+i)) {
          if(!(p_mat->flag&(MAT_NO_BUMP|MAT_SCENE|MAT_PRUHLEDNY))) {
            switch(bmptyp) {
            case AUTO_BUMP_DOT3:
              kprintf(TRUE,"Dot3-Bump-material %s",p_mat->jmeno);
              ber_mat_vloz_dot3(p_ber->p_text,MAX_CELKEM_TEXTUR,p_mat);
              break;
            case AUTO_BUMP_DOT3ENV:
              kprintf(TRUE,"Env-Dot3-Bump-material %s",p_mat->jmeno);
              ber_mat_vloz_dot3_env(p_ber->p_text,MAX_CELKEM_TEXTUR,p_mat);
              break;
            default:
              break;
            }
          }
        }
      }
    }
  }  
  */
  return (TRUE);
}

void ber_load_pridej_smat(G_KONFIG * p_ber, int reload)
{
  int m;

  if (p_ber->conf_caustic && !reload && glstav_multitext_units > 2) {
    p_ber->p_smat = NULL;
    for (m = 0; m < MAX_CELKEM_MATERIALU; m++) {
      if (p_ber->p_mat[m] && p_ber->p_mat[m]->flag & MAT_SCENE) {
        p_ber->p_smat = p_ber->p_mat[m];
        amat_pridej_material(p_ber, p_ber->p_smat);
        p_ber->p_smat->flag &= ~MAT_SCENE_NUTNY;
        kprintf(TRUE, "           - Scene-Material %s", p_ber->p_smat->jmeno);
        return;
      }
    }
    kprintf(TRUE, "           - Scene-Material neni");
  }
}

int ber_nahraj_kurzor(G_KONFIG * p_ber)
{
  int l;

  l = lo_najdi_volny_mesh(p_ber->p_mesh, MAX_BERUSKY_KONTEJNERU);
  if (l == K_CHYBA)
    return (FALSE);

  if (ber_nahraj_mesh(p_ber, JMENO_KURZOR, p_ber->p_mesh + l)) {
    if (l >= p_ber->meshnum)
      p_ber->meshnum = l + 1;
    p_ber->p_mesh[l]->p_data->k2flag |= KONT2_BERUSKA;
    return (TRUE);
  }
  else {
    return (FALSE);
  }
}

/*
  Zluta
  Hneda
  Cervena
  Modra
  Zelena
*/

static char ber_list[][50] = { "beruska_zluta_game.b2m",
  "beruska_hneda_game.b2m",
  "beruska_cervena_game.b2m",
  "beruska_modra_game.b2m",
  "beruska_zelena_game.b2m"
};

int ber_je_mesh_beruska(int kont, BUNKA_LEVELU_DISK * p_berusky, int bernum,
  char *p_jmeno)
{
  int i, br;

  for (i = 0; i < bernum; i++, p_berusky++) {
    if (p_berusky->mesh == kont) {
      br = p_berusky->guid % 10;
      strcpy(p_jmeno, ber_list[br]);
      return (TRUE);
    }
  }
  return (FALSE);
}

/*
  Nahraje scenu
*/
int ber_nahraj_scenu(G_KONFIG * p_ber, char *p_jmeno, char *p_dir, int reload,
  BUNKA_LEVELU_DISK * p_berusky, int bernum)
{
  EDIT_KONTEJNER *p_kont[MAX_BERUSKY_KONTEJNERU];
  NASTAVENI_KAMER kam;
  LENS_FLARE flare[MAX_FLARE_SVETEL];
  LEVEL_KONFIG lc;
  char file[200], tmp[200];
  dword ambient;
  int k, f, kflag, p, m, ret;

  memset(p_kont, 0, sizeof(p_kont[0]) * MAX_BERUSKY_KONTEJNERU);
  memset(flare, 0, sizeof(flare[0]) * MAX_FLARE_SVETEL);

  strcpy(file, p_jmeno);
  zamen_koncovku(file, KONCOVKA_SCENY);

  chdir((p_dir));

  kprintf(1, "           - Load project....");
  if (lo_nahraj_projekt(p_ber->p_mat, MAX_CELKEM_MATERIALU,
      p_kont, MAX_BERUSKY_KONTEJNERU,
      p_ber->p_text, MAX_CELKEM_TEXTUR,
      file, p_ber->p_slight, MAX_FLARE_SVETEL,
      &p_ber->zrc, &p_ber->zrc_akt,
      flare, MAX_FLARE_SVETEL,
      &ambient, &p_ber->p_mlha,
      p_ber->p_dlight, MAX_FLARE_SVETEL,
      p_ber->p_edlight, MAX_FLARE_SVETEL,
      &p_ber->env, &kam, &lc, LOAD_ALL, FALSE)) 
  {

    p_ber->conf_barva_pozadi = lc.barva_pozadi;
    p_ber->conf_barva_pozadi_pouzit =
      p_ber->
      conf_barva_pozadi_pouzit_default ? TRUE : lc.barva_pozadi_aktivni;
    p_ber->zm = lc.zm;

    p_ber->kam.min = kam.min;
    p_ber->kam.max = kam.max;

    p_ber->kam.max_uhel = MAX_UHEL_KAMERY;
    p_ber->kam.min_uhel = MIN_UHEL_KAMERY;
    p_ber->kam.max_vzdal = MAX_VZDAL_KAMERY;
    p_ber->kam.min_vzdal = MIN_VZDAL_KAMERY;

    // Nahodim scene-material
    ber_load_pridej_smat(p_ber, reload);

    p_ber->dl_lightnum =
      lo_najdi_prepocitej_dsvetlo(p_ber->p_dlight, MAX_FLARE_SVETEL);
    p_ber->edl_lightnum =
      lo_najdi_prepocitej_extra_svetlo(p_ber->p_edlight, MAX_FLARE_SVETEL);
    rgb_float(ambient, p_ber->dl_ambient);

    f = lo_najdi_volny_flare(flare, MAX_FLARE_SVETEL);
    if (f) {
      p_ber->p_flare = lo_flare2linear(flare, f);
    }
    p_ber->slightnum = lo_posledni_svetlo(p_ber->p_slight, MAX_FLARE_SVETEL);

    if(export_level) {
      json_export_level(p_kont, MAX_BERUSKY_KONTEJNERU,
                        p_ber->p_mat, MAX_CELKEM_MATERIALU,
                        p_ber->p_slight, MAX_FLARE_SVETEL);
    }  
  
    // Smazu materialy mazanych objektu
    for (k = 0; k < MAX_BERUSKY_KONTEJNERU; k++) {
      if (p_kont[k]) {
        if (ber_je_mesh_beruska(k, p_berusky, bernum, tmp)) {
          lo_zrus_material_kont(p_kont[k], p_ber->p_mat, MAX_CELKEM_MATERIALU);
        }
      }
    }

    // prekopiruje kontejnery-meshe 1:1
    for (k = 0, m = 0, p = -1; k < MAX_BERUSKY_KONTEJNERU; k++) {
      if (p_kont[k]) {
        if (ber_je_mesh_beruska(k, p_berusky, bernum, tmp)) {
          ret = ber_nahraj_mesh(p_ber, tmp, p_ber->p_mesh + m);
          assert(ret);
          zrus_kontejner_rec(p_kont + k, NULL);
          p_ber->p_mesh[m]->p_data->kflag |= KONT_PRVEK;
          p_ber->p_mesh[m]->p_data->k2flag |= KONT2_BERUSKA;
        }
        else {
          p_ber->p_mesh[m] = lo_kontejner_to_mesh(p_kont + k, p_ber->p_mat,
            MAX_CELKEM_MATERIALU, p_ber->conf_extra_light_vertex);
        }

        if (!p_ber->p_mesh[m])
          continue;

        // Kvuli animacim rozkopiruj materialy
        ber_materialy_rozkopiruj(p_ber, p_ber->p_mesh[m], reload);

        // Pridam do animacniho listu materialu        
        amat_pridej_mesh(p_ber, m);

        kflag = p_ber->p_mesh[m]->p_data->kflag;

        // roztridit to na prvky/ne prvky
        if (!(kflag & (KONT_PRVEK | KONT_KEYFRAME))) {
          p_ber->p_mesh[m]->m = p_ber->p_mesh[m]->world;
        }

        // Kontejner je zrcadlo - zkusim ho najit
        if (p_ber->zrc_akt && kflag & KONT_ZRCADLO) {
          ZDRCADLO_DESC_POLY *p_poly = p_ber->zrc.p_poly;

          while (p_poly) {
            if (p_ber->p_mesh[m]->p_data->mesh_ID == p_poly->id_kont) {
              p_poly->zrcadlo_k = m;
              p_poly->poly = K_CHYBA;
              break;
            }
            p_poly = p_poly->p_next;
          }
          if (!p_poly)
            p_ber->p_mesh[m]->p_data->kflag &= ~KONT_ZRCADLO;
        }

        m++;
      }
    }
    p_ber->meshnum = m + 1;

    p_ber->conf_kurzor_mesh = ber_nahraj_kurzor(p_ber);
    p_ber->dl_lightnum = lo_najdi_prepocitej_dsvetlo(p_ber->p_dlight, MAX_FLARE_SVETEL);
      
    return (TRUE);
  }
  else {
    return (FALSE);
  }
}

/*
  ber_load_poly
*/
void ber_nahraj_poly(G_KONFIG * p_ber, char *p_jmeno, char *p_dir)
{
  MLZNA_KOSTKA *p_tmp;
  char file[200];
  int i;

  chdir((p_dir));
  strcpy(file, p_jmeno);
  zamen_koncovku(file, ".ply");

  // Load poly
  p_ber->p_poly =
    lo_nahraj_poly_list(file, &p_ber->polynum, p_ber->p_lightmap,
    p_ber->p_mat, MAX_CELKEM_MATERIALU);
  for (i = 0; i < p_ber->polynum; i++) {
    poly_pridej_vertex_array(p_ber->p_poly + i);
    amat_pridej_poly(p_ber, i);
    lo_poly_flaguj_materialy(p_ber->p_poly + i, p_ber->p_mat);
  }

  if(export_level) {
    json_export_poly(p_ber->p_poly, p_ber->polynum,
                     p_ber->p_mat, MAX_CELKEM_MATERIALU);
  }

  /* Flagovani zrcadla
   */
  {
    ZDRCADLO_DESC_POLY *p_poly = p_ber->zrc.p_poly;

    while (p_poly) {
      if (p_poly->poly != K_CHYBA) {
        p_ber->p_poly[p_poly->poly].kflag |= KONT_ZRCADLO;
      }
      p_poly = p_poly->p_next;
    }
  }


  /* Prismazeni mlho-kostek
   */
  if (p_ber->polynum) {
    p_tmp = p_ber->p_mlha;
    while (p_tmp) {
      if (p_tmp->poly != K_CHYBA) {
        p_ber->p_poly[p_tmp->poly].p_mlha = p_tmp;
        p_tmp->p_kont = p_ber->p_poly + p_tmp->poly;
      }
      p_tmp = p_tmp->p_next;
    }
  }
}

/*
  nahraje lightmapy
*/
void ber_nahraj_lightmap(G_KONFIG * p_ber, char *p_jmeno, char *p_dir)
{
  TXT_KONFIG zal;
  char pom[200];
  KFILE *f;
  int i, r;

  chdir((p_dir));
  strcpy(pom, p_jmeno);
  zamen_koncovku(pom, KONCOVKA_LIGHTMAPY);

  zal = txconf;
  txconf.text_trida = TEXT_LIGHT;
  txconf.text_mip_mapping = FALSE;
  txconf.text_filtr = FALSE;

  if ((f = kopen(NULL, pom, "rb"))) {
    while (!keof(f)) {
      if (kread(&i, sizeof(i), 1, f)) {
        assert(i >= 0 && i < MAX_RAY_TEXTUR);
        sprintf(p_ber->p_lightmap[i].jmeno, "%s_lp%.3d.bmp", pom, i);
        r = txt_nahraj_lightmapu_z_bmp(NULL, f, p_ber->p_lightmap + i, TRUE);
        assert(r);
      }
    }
    kclose(f);
  }
  else {                        // stara verze lightmap
    for (i = 0; i < MAX_RAY_TEXTUR; i++) {
      sprintf(p_ber->p_lightmap[i].jmeno, "%s\\%s_lp%.3d.bmp", p_dir, pom, i);
      if (efile(p_ber->p_lightmap[i].jmeno)) {
        kprintfl(TRUE, "Lightmap %s...", p_ber->p_lightmap[i].jmeno);
        txt_nahraj_lightmapu_z_bmp(p_ber->p_lightmap[i].jmeno, NULL,
          p_ber->p_lightmap + i, TRUE);
      }
    }
  }

  txconf = zal;

  /* Prepocitej lightmapy
   */
  for (i = 0; i < p_ber->polynum; i++) {
    lo_poly_calc_lightmap_face(p_ber->p_poly + i);
  }

  /* Zrus prebytecne bitmapy
   */
  for (i = 0; i < MAX_RAY_TEXTUR; i++) {
    if (p_ber->p_lightmap[i].p_bmp) {
      if(export_level) {
        json_export_lightmap(i, p_ber->p_lightmap[i].p_bmp);
      }  
      bmp_zrus(&p_ber->p_lightmap[i].p_bmp);
    }
  }
}

inline void bod_min_max(BOD * p_ref, BOD * p_min, BOD * p_max)
{
  if (p_ref->x < p_min->x)
    p_min->x = p_ref->x;
  if (p_ref->y < p_min->y)
    p_min->y = p_ref->y;
  if (p_ref->z < p_min->z)
    p_min->z = p_ref->z;

  if (p_ref->x > p_max->x)
    p_max->x = p_ref->x;
  if (p_ref->y > p_max->y)
    p_max->y = p_ref->y;
  if (p_ref->z > p_max->z)
    p_max->z = p_ref->z;
}

void ber_velikost_sceny(G_KONFIG * p_ber)
{
  GAME_MESH_OLD *p_mesh;
  BOD min, max;
  int i;
  float vzdal;

  min.x = min.y = min.z = FLT_MAX;
  max.x = max.y = max.z = -FLT_MAX;

  // Spocitam obalky vsech meshu
  for (i = 0; i < p_ber->meshnum; i++) {
    p_mesh = p_ber->p_mesh[i];
    if (p_mesh) {
      bod_min_max(&p_mesh->obb_world.aabb_min, &min, &max);
      bod_min_max(&p_mesh->obb_world.aabb_max, &min, &max);
    }
  }

  for (i = 0; i < p_ber->polynum; i++) {
    bod_min_max(&p_ber->p_poly[i].obb.aabb_min, &min, &max);
    bod_min_max(&p_ber->p_poly[i].obb.aabb_max, &min, &max);
  }

  vzdal = vzdal_bodu_bod(&min, &max) * 1.3f;
  if (vzdal > p_ber->kam.far_plane)
    p_ber->kam.far_plane = vzdal;

  kprintf(TRUE, "Z-Buffer range %.3f", p_ber->kam.far_plane);
}

GAME_MESH_OLD **ber_mesh_vyber_staticke(G_KONFIG * p_ber, int *p_snum,
  int **p_indexy)
{
  GAME_MESH_OLD **p_list = p_ber->p_mesh;
  int listnum = p_ber->meshnum;
  GAME_MESH_OLD **p_stat;
  int i, snum;
  int *p_ind;

  snum = 0;
  for (i = 0; i < listnum; i++) {
    if (p_list[i] && p_list[i]->p_data->kflag & KONT_STATIC)
      snum++;
  }

  if (!snum) {
    *p_snum = 0;
    *p_indexy = NULL;
    return (NULL);
  }

  p_stat = (GAME_MESH_OLD **) mmalloc(sizeof(p_stat[0]) * snum);
  p_ind = (int *) mmalloc(sizeof(p_ind[0]) * snum);
  snum = 0;
  for (i = 0; i < listnum; i++) {
    if (p_list[i] && p_list[i]->p_data->kflag & KONT_STATIC) {
      p_stat[snum] = p_list[i];
      p_ind[snum] = i;
      snum++;
    }
  }
  *p_snum = snum;
  *p_indexy = p_ind;

  return (p_stat);
}
