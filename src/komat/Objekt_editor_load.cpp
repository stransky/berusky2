/*
  Load / save
*/
#include "mss_on.h"
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <direct.h>
#include <float.h>
#include <assert.h>
#include <process.h>
#include "objekt_editor_all.h"

/****************************************
   Loady vseho mozneho
 ****************************************
*/
EDIT_KONTEJNER *oe_nahraj_kontejner_bez_dialog(K_EDITOR * p_cnf,
  byte * p_jmeno, int mat)
{
  EDIT_KONTEJNER *p_kont = NULL;

  chdir(p_cnf->dir.out_dir);
  kprintf(TRUE, "Load kontejner '%s'...", p_jmeno);
  p_kont =
    lo_nahraj_kontejner(p_cnf->p_mat, MAX_EDIT_MATERIALU, p_cnf->p_text,
    MAX_EDIT_TEXTUR, p_jmeno, mat);
  lo_preved_flare_do_sceny(p_kont->p_slight, p_cnf->flare, MAX_EDIT_SVETEL);
  lo_preved_svetla_do_sceny(p_kont, p_cnf->light, MAX_EDIT_SVETEL,
    p_cnf->p_dlight, MAX_FLARE_SVETEL, p_cnf->p_edlight, MAX_FLARE_SVETEL);

  oe_reload_textur(p_cnf);

  if (!p_kont) {
    ddw("Chyba nahrani kontejneru '%s'", p_jmeno);
    return (NULL);
  }
  else {
    return (p_kont);
  }
}

EDIT_KONTEJNER *oe_nahraj_kontejner_bez_dialog_3ds(K_EDITOR * p_cnf,
  byte * p_jmeno, int mat)
{
  EDIT_KONTEJNER *p_kont = NULL;

  chdir(p_cnf->dir.out_dir);
  doe_3ds_importer(&p_cnf->cnf.keyframe_load, &p_cnf->cnf.keyframe_static,
    &p_cnf->cnf.keyframe_kont, &p_cnf->cnf.kamera_load);
  if ((p_kont =
      ds_import_kontejner(p_jmeno, mat, p_cnf->cnf.keyframe_load, TRUE, TRUE,
        p_cnf->cnf.kamera_load)) == NULL) {
    ddw("Chyba nahrani souboru '%s'", p_jmeno);
    return (NULL);
  }
  return (p_kont);
}

EDIT_KONTEJNER *oe_nahraj_kontejner_dialog(K_EDITOR * p_cnf, byte * p_jmeno,
  byte max_jmeno, int mat)
{
  EDIT_KONTEJNER *p_kont = NULL;
  byte pom[100];
  int i = 0;

  chdir(p_cnf->dir.out_dir);

  if (doe_dialog_load(hwnd_aplikace, pom, 100, "Nahrani kontejneru...",
      MASKA_MESH, "Mesh soubory")) {
    kprintf(TRUE, "Load kontejner '%s'...", pom);

    p_kont =
      lo_nahraj_kontejner(p_cnf->p_mat, MAX_EDIT_MATERIALU, p_cnf->p_text,
      MAX_EDIT_TEXTUR, pom, mat);
    lo_preved_flare_do_sceny(p_kont->p_slight, p_cnf->flare, MAX_EDIT_SVETEL);
    lo_preved_svetla_do_sceny(p_kont, p_cnf->light, MAX_EDIT_SVETEL,
      p_cnf->p_dlight, MAX_FLARE_SVETEL, p_cnf->p_edlight, MAX_FLARE_SVETEL);


    oe_reload_textur(p_cnf);

    if (!p_kont) {
      ddw("Chyba nahrani objektu '%s'", pom);
      return (NULL);
    }
    strcpy(p_jmeno, pom);
  }

  return (p_kont);
}

EDIT_KONTEJNER *oe_nahraj_kontejner_dialog_3ds(K_EDITOR * p_cnf,
  byte * p_jmeno, byte max_jmeno, int mat)
{
  EDIT_KONTEJNER *p_kont = NULL;
  byte pom[100];
  int i = 0;

  chdir(p_cnf->dir.out_dir);
  if (doe_dialog_load(hwnd_aplikace, pom, 99, "Import z 3D studia",
      "3D Studio project file (*.prj)\1*.prj",
      "3D Mesh (*.3ds)\1*.3ds\1 3D Studio Material library (*.mli)\1*.mli")) {
    doe_3ds_importer(&p_cnf->cnf.keyframe_load, &p_cnf->cnf.keyframe_static,
      &p_cnf->cnf.keyframe_kont, &p_cnf->cnf.kamera_load);
    if ((p_kont =
        ds_import_kontejner(pom, mat, p_cnf->cnf.keyframe_load, TRUE,
          p_cnf->cnf.keyframe_static, p_cnf->cnf.kamera_load)) == NULL) {
      ddw("Chyba nahrani souboru '%s'", pom);
      return (NULL);
    }
    strcpy(p_jmeno, pom);
  }
  return (p_kont);
}

EDIT_KONTEJNER *oe_nahraj_kontejner_dialog_ase(K_EDITOR * p_cnf,
  byte * p_jmeno, byte max_jmeno)
{
  EDIT_KONTEJNER *p_kont = NULL;
  byte pom[100];
  int i = 0;

  chdir(p_cnf->dir.out_dir);
  if (doe_dialog_load(hwnd_aplikace, pom, 99, "Import z ASCII", "*.ase",
      "MAX ASCII soubory(*.ASE)")) {
    if ((p_kont = ase_import(pom, ".")) == NULL) {
      ddw("Chyba nahrani souboru '%s'", pom);
      return (NULL);
    }
    strcpy(p_jmeno, pom);
  }
  return (p_kont);
}

void oe_prikaz_nahraj_kontejner(K_EDITOR * p_cnf, int soubor, int mat)
{
  byte string[30];
  int k, k1 = p_cnf->vybr_kont;

  if (oe_vyber_volny_kontejner(p_cnf) == K_CHYBA) {
    ddw("Neni volna pozice !");
    return;
  }

  k = p_cnf->vybr_kont;
  if (soubor == LOAD_KONT_BTX) {
    if ((p_cnf->p_kont[k] =
        oe_nahraj_kontejner_dialog(p_cnf, string, 29, mat)) == NULL) {
      p_cnf->vybr_kont = k1;
    }
    else {
      strcpy(p_cnf->p_kont[k]->jmeno, string);
    }
  }
  else if (soubor == LOAD_KONT_3DS) {
    if ((p_cnf->p_kont[k] =
        oe_nahraj_kontejner_dialog_3ds(p_cnf, string, 29, mat)) == NULL) {
      p_cnf->vybr_kont = k1;
    }
    else {
      strcpy(p_cnf->p_kont[k]->jmeno, string);
      if (p_cnf->cnf.keyframe_kont) {
        if (ds_kontejner_rozloz(p_cnf, p_cnf->p_kont[k], ROZLOZ_MAX))
          zrus_kontejner_rec(p_cnf->p_kont + k, &p_cnf->p_mlha);
      }
    }
  }
  else if (soubor == LOAD_KONT_ASE) {
    if ((p_cnf->p_kont[k] =
        oe_nahraj_kontejner_dialog_ase(p_cnf, string, 29)) == NULL) {
      p_cnf->vybr_kont = k1;
    }
    else {
      strcpy(p_cnf->p_kont[k]->jmeno, string);
    }
  }
}

void oe_prikaz_nahraj_objekt(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  byte string[30];
  int k = p_cnf->vybr_kont, o = p_cnf->vybr_objekt;

  n_objekt(k, o);

  if ((p_kont = oe_nahraj_kontejner_dialog(p_cnf, string, 29, FALSE)) == NULL)
    return;
  if (p_kont->p_obj[0])
    kopiruj_objekt_geometrie(p_kont->p_obj[0], p_cnf->p_kont[k]->p_obj[o]);
  zrus_kontejner(&p_kont, NULL);
}

static int najdi_volnou_animaci(HIERARCHY_SIM * p_root)
{
  int i;

  for (i = 0; i < KONT_MAX_ANIM; i++) {
    if (!p_root[i].keynum)
      return (i);
  }
  return (K_CHYBA);
}

void oe_prikaz_nahraj_3ds_animaci_dialog(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_anim_kont;
  EDIT_KONTEJNER *p_kont;
  byte string[30];
  int k = p_cnf->vybr_kont;
  int va;

  n_kont(k);
  p_kont = p_cnf->p_kont[k];
  va = p_kont->akt_animace;

  if ((p_anim_kont =
      oe_nahraj_kontejner_dialog_3ds(p_cnf, string, 29, FALSE))) {
    if (p_kont->sim[va].keynum) {
      va = najdi_volnou_animaci(p_kont->sim);
      if (va == K_CHYBA) {
        ddw("Neni volna animace");
        zrus_kontejner_rec(&p_anim_kont, &p_cnf->p_mlha);
        return;
      }
    }
    key_sim_root_to_sim_root_indir(p_kont, p_kont->sim + va,
      p_anim_kont->sim);
    strcpy(p_kont->sim[va].jmeno, p_anim_kont->jmeno);
    zrus_kontejner_rec(&p_anim_kont, &p_cnf->p_mlha);
  }
}

void oe_prikaz_nahraj_b2m_animaci_dialog(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_anim_kont;
  EDIT_KONTEJNER *p_kont;
  byte string[30];
  int k = p_cnf->vybr_kont;
  int va;

  n_kont(k);
  p_kont = p_cnf->p_kont[k];
  va = p_kont->akt_animace;

  if ((p_anim_kont = oe_nahraj_kontejner_dialog(p_cnf, string, 29, FALSE))) {
    if (p_kont->sim[va].keynum) {
      va = najdi_volnou_animaci(p_kont->sim);
      if (va == K_CHYBA) {
        ddw("Neni volna animace");
        zrus_kontejner_rec(&p_anim_kont, &p_cnf->p_mlha);
        return;
      }
    }
    key_sim_root_to_sim_root_indir(p_kont, p_kont->sim + va,
      p_anim_kont->sim);
    strcpy(p_kont->sim[va].jmeno, p_anim_kont->jmeno);
    zrus_kontejner_rec(&p_anim_kont, &p_cnf->p_mlha);
  }
}

void oe_prikaz_nahraj_sim_animaci_dialog(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  char pom[200];
  int k = p_cnf->vybr_kont;
  int va;

  n_kont(k);
  p_kont = p_cnf->p_kont[k];
  va = p_kont->akt_animace;

  if (p_kont->sim[va].keynum) {
    if (!ddwqueto(hwnd_aplikace, "Vybrana animace uz exituje, smazat?"))
      return;
  }

  oe_preindexuj_kontejner(p_kont);
  if (doe_dialog_load(hwnd_aplikace, pom, 200, "Load animace...", "*.sim",
      "Soubor s sim animaci")) {
    if (p_kont->sim[va].keynum)
      key_sim_root_zrus(p_kont->sim + va);
    key_sim_nahraj_extended(p_kont, va, pom, ".");
    strcpy(p_kont->sim[va].jmeno, pom);
    key_sim_root_vloz_pivoty_do_animace_kont(p_kont);
  }
}

void oe_prikaz_uloz_kontejner(K_EDITOR * p_cnf, int obalky)
{
  int k = p_cnf->vybr_kont;

  if (p_cnf->bnf.p_lev) {
    if (!ddwqueto(hwnd_aplikace,
        "Je aktivni level. Ulozit vybrany prvek levelu jako mesh?"))
      return;
  }

  chdir(p_cnf->dir.out_dir);
  if (p_cnf->p_kont[k]) {
    vycisti_kontejner(p_cnf->p_kont[k]);
    updatuj_kontejner_statistika(p_cnf->p_kont[k], obalky);
    zamen_koncovku(p_cnf->p_kont[k]->jmeno, KONCOVKA_MESH);
    lo_uloz_kontejner(p_cnf->p_mat, MAX_EDIT_MATERIALU, p_cnf->p_kont[k],
      p_cnf->p_kont[k]->jmeno, FALSE, NULL);
    ddw("Ukladam ak. kontejner jako %s", p_cnf->p_kont[k]->jmeno);
  }
  else
    ddw("Neni co ukladat !");
}

void oe_prikaz_uloz_kontejner_jako(K_EDITOR * p_cnf)
{
  byte pom[100];
  int k = p_cnf->vybr_kont;

  if (p_cnf->bnf.p_lev) {
    if (!ddwqueto(hwnd_aplikace,
        "Je aktivni level. Ulozit vybrany prvek levelu jako mesh?"))
      return;
  }

  chdir(p_cnf->dir.out_dir);
  if (!p_cnf->p_kont[k])
    ddw("Neni co ukladat !");
  else {
    if (doe_dialog_save(pom, 99, "Ulozit jako:", MASKA_MESH, "Mesh soubory")) {
      transformuj_kontejner(p_cnf->p_kont[k]);
      vycisti_kontejner(p_cnf->p_kont[k]);
      updatuj_kontejner_statistika(p_cnf->p_kont[k], TRUE);
      strcpy(p_cnf->p_kont[k]->jmeno, pom);
      lo_uloz_kontejner(p_cnf->p_mat, MAX_EDIT_MATERIALU, p_cnf->p_kont[k],
        pom, FALSE, NULL);
    }
  }
}

void oe_prikaz_uloz_3ds_kontejner_jako(K_EDITOR * p_cnf)
{
  byte pom[100];
  int k = p_cnf->vybr_kont;

  chdir(p_cnf->dir.out_dir);
  if (!p_cnf->p_kont[k])
    ddw("Neni co ukladat !");
  else {
    if (doe_dialog_save(pom, 99, "Ulozit jako:", "*.3ds", "3DS Mesh (*.3ds)")) {
      transformuj_kontejner(p_cnf->p_kont[k]);
      vycisti_kontejner(p_cnf->p_kont[k]);
      updatuj_kontejner_statistika(p_cnf->p_kont[k], TRUE);
      strcpy(p_cnf->p_kont[k]->jmeno, pom);
      ds_export_kontejner(pom, p_cnf->p_kont[k]);
    }
  }
}

void oe_prikaz_uloz_3ds_scenu_jako(K_EDITOR * p_cnf)
{
  byte pom[100];

  chdir(p_cnf->dir.out_dir);
  if (doe_dialog_save(pom, 99, "Ulozit jako:", "*.3ds", "3DS Mesh (*.3ds)")) {
    ds_export_sceny(pom);
  }
}

void oe_prikaz_uloz_vybrane_kontejnery_jako(K_EDITOR * p_cnf)
{
  FFILE f = FALSE;
  byte pom[100];
  int i, k;

  chdir(p_cnf->dir.scene_dir);

  if (doe_dialog_save(pom, 99, "Ulozit jako:", MASKA_SCENY,
      "Vybrane kontejnery")) {
    if (p_cnf->groupfacenum && p_cnf->groupflag & GROUP_KONT
      && !(p_cnf->groupflag & (GROUP_OBJ | GROUP_FACE))) {
      for (i = 0; i < p_cnf->groupfacenum; i++) {
        k = p_cnf->p_group_face[i].k;
        if (p_cnf->p_kont[k]) {
          transformuj_kontejner(p_cnf->p_kont[k]);
          vycisti_kontejner(p_cnf->p_kont[k]);
          updatuj_kontejner_statistika(p_cnf->p_kont[k], TRUE);
          f =
            lo_uloz_kontejner(p_cnf->p_mat, MAX_EDIT_MATERIALU,
            p_cnf->p_kont[k], pom, TRUE, f);
        }
      }
      ffclose(f);
    }
  }
}

int oe_uloz_projekt(K_EDITOR * p_cnf, byte * p_file, byte * p_dir, int level,
  int transf_stat)
{
  EDIT_KONTEJNER **p_kont = NULL, *p_tmp;
  LEVEL_KONFIG lc;
  byte file[50];
  int k, kont_num, ret;

  if (p_cnf->cnf.load & SAVE_GEOM_VYBR) {
    int handle;

    if (handle = oe_vybr_kont_reset()) {
      kont_num = p_cnf->groupfacenum;
      p_kont = malloc(sizeof(p_kont[0]) * kont_num);
      k = 0;
      while (p_tmp = oe_vybr_kont_get(&handle)) {
        assert(k < kont_num);
        p_kont[k++] = p_tmp;
      }
    }
    else {
      kprintf(TRUE, "Neni vybrany zadny kontejner!!!");
      return (FALSE);
    }
  }
  else {
    p_kont = p_cnf->p_kont;
    kont_num = KONT_NUM;
  }

  lc.barva_pozadi_aktivni = p_cnf->cnf.aktivni_pozadi;
  lc.barva_pozadi = p_cnf->barva_pozadi;
  lc.zm = p_cnf->zm;

  chdir(p_dir);
  strcpy(file, p_file);
  zamen_koncovku(file, KONCOVKA_SCENY);
  ret = lo_uloz_projekt(p_cnf->p_mat, MAX_EDIT_MATERIALU,
    p_kont, kont_num, file,
    p_cnf->light, MAX_EDIT_SVETEL,
    (p_cnf->zdcadlo_akt) ? &p_cnf->zrcadlo : NULL,
    p_cnf->flare, MAX_EDIT_SVETEL, p_cnf->ray.ambient,
    p_cnf->p_mlha, p_cnf->p_dlight, MAX_FLARE_SVETEL,
    p_cnf->p_edlight, MAX_FLARE_SVETEL, &p_cnf->env,
    &p_cnf->kam, &lc, level, transf_stat);
  if (p_cnf->cnf.load & SAVE_GEOM_VYBR)
    free(p_kont);
  return (ret);
}

static int _najdi_cislo(byte * p_string)
{
  byte *pom = _alloca(strlen(p_string));
  byte *p_pom, *p_last;
  int k;

  strcpy(pom, p_string);
  p_pom = pom;

  while ((p_pom = strchr(p_pom, '_'))) {
    p_last = ++p_pom;
  }

  sscanf(p_last, "%d%s", &k, KONCOVKA_MESH);
  return (k);
}

int oe_nahraj_projekt_out(K_EDITOR * p_cnf, byte * p_file, byte volny_load)
{
  GAMESLOT slot;
  byte file[50];
  FILE *f;
  int k;

  chdir(p_cnf->dir.scene_dir);

  if (!volny_load)
    oe_smaz_scenu(p_cnf);

  strcpy(file, p_file);
  zamen_koncovku(file, KONCOVKA_SCENY_OLD);

  if (!volny_load)
    strcpy(p_cnf->jmeno_sceny, file);

  if ((f = fopen(file, "rb")) == NULL)
    tiskni_chybu(STC, "otevreni souboru %s", file);

  while (fread(&slot, sizeof(slot), 1, f)) {
    if (strstr(slot.jmeno, KONCOVKA_MESH)
      || strstr(slot.jmeno, KONCOVKA_MESH_OLD)) {
      chdir(p_cnf->dir.scene_dir);

      if (!volny_load)
        k = _najdi_cislo(slot.jmeno);
      else {
        if ((k =
            lo_najdi_volny_kontejner(p_cnf->p_kont,
              p_cnf->max_kont)) == K_CHYBA) {
          ddw("Neni volny kontejner !");
          return (FALSE);
        }
      }

      assert(!p_cnf->p_kont[k]);
      kprintf(TRUE, "Load kontejner %s...", slot.jmeno);
      p_cnf->p_kont[k] = lo_nahraj_kontejner(p_cnf->p_mat, MAX_EDIT_MATERIALU,
        p_cnf->p_text, MAX_EDIT_TEXTUR, slot.jmeno, TRUE);

      if (!p_cnf->p_kont[k]) {
        ddw("Chyba nahrani objektu '%s'", slot.jmeno);
        continue;
      }
      strcpy(p_cnf->p_kont[k]->jmeno, slot.jmeno);
      p_cnf->p_kont[k]->world = slot.word;
      k++;
    }
    else if (strstr(slot.jmeno, ".flr")) {
      kprintf(1, "Nahravanich starych flaru neni podporovany.");
    }
    else
      ddw("Co to je '%s' ?", slot.jmeno);
  }
  fclose(f);

  oe_reload_textur(p_cnf);
  return (TRUE);
}

void oe_recalc_zrcadla(EDIT_KONTEJNER ** p_kont, int kontnum,
  ZDRCADLO_DESC * p_zrcadlo)
{
  ZDRCADLO_DESC_POLY *p_poly = p_zrcadlo->p_poly;
  int i;

  while (p_poly) {
    if (p_poly->id_kont != K_CHYBA) {
      for (i = 0; i < kontnum; i++) {
        if (p_kont[i] && p_kont[i]->kontejner_ID == p_poly->id_kont) {
          p_poly->zrcadlo_k = i;
          return;
        }
      }
      kprintf(TRUE, "Load-Nenalezeno zrcadlo!");
    }
    p_poly = p_poly->p_next;
  }
}

void oe_load_pridej_smat(K_EDITOR * p_cnf)
{
  int m;

  p_cnf->p_smat = NULL;
  for (m = 0; m < MAX_CELKEM_MATERIALU; m++) {
    if (p_cnf->p_mat[m] && p_cnf->p_mat[m]->flag & MAT_SCENE) {
      p_cnf->p_smat = p_cnf->p_mat + m;
      return;
    }
  }
}

int oe_nahraj_projekt(K_EDITOR * p_cnf, byte * p_file, byte * p_dir,
  byte volny_load, int load_textury)
{
  LEVEL_KONFIG lc;
  byte zaloha[200];
  byte file[50];
  int stav;

  strcpy(zaloha, p_cnf->dir.scene_dir);
  strcpy(p_cnf->dir.scene_dir, p_dir);
  chdir(p_cnf->dir.scene_dir);
  strcpy(file, p_file);

  // Skusim nahrat stary projekt  
  zamen_koncovku(file, KONCOVKA_SCENY_OLD);
  if (efile(file)) {
    return (oe_nahraj_projekt_out(p_cnf, p_file, volny_load));
  }

  lc.barva_pozadi = p_cnf->barva_pozadi;
  lc.barva_pozadi_aktivni = p_cnf->cnf.aktivni_pozadi;
  lc.zm = p_cnf->zm;

  // Loaduju novy projekt
  zamen_koncovku(file, KONCOVKA_SCENY);
  if (efile(file)) {
    stav = lo_nahraj_projekt(p_cnf->p_mat, MAX_EDIT_MATERIALU,
      p_cnf->p_kont, KONT_NUM,
      p_cnf->p_text, MAX_EDIT_TEXTUR, file,
      p_cnf->light, MAX_EDIT_SVETEL,
      &p_cnf->zrcadlo, &p_cnf->zdcadlo_akt,
      p_cnf->flare, MAX_EDIT_SVETEL,
      &p_cnf->ray.ambient,
      &p_cnf->p_mlha,
      p_cnf->p_dlight, MAX_FLARE_SVETEL,
      p_cnf->p_edlight, MAX_FLARE_SVETEL,
      &p_cnf->env, &p_cnf->kam, &lc, p_cnf->cnf.load, TRUE);
    if (p_cnf->zdcadlo_akt)
      oe_recalc_zrcadla(p_cnf->p_kont, KONT_NUM, &p_cnf->zrcadlo);

    p_cnf->barva_pozadi = lc.barva_pozadi;
    p_cnf->cnf.aktivni_pozadi = lc.barva_pozadi_aktivni;
    p_cnf->zm = lc.zm;

    oe_load_pridej_smat(p_cnf);

    oe_najdi_prepocitej_dsvetlo(p_cnf);
    p_cnf->p_mlha_akt = p_cnf->p_mlha;

    // Reloadne vsechny textury
    if (load_textury) {
      oe_reload_textur(p_cnf);
    }

    strcpy(p_cnf->dir.scene_dir, zaloha);

    konfigurace_mlha_aktivuj();

  }
  else {
    kprintf(TRUE, "Soubor %s v adresari %s neexistuje!", file,
      p_cnf->dir.scene_dir);
  }

  return (stav);
}

void oe_smaz_scenu(K_EDITOR * p_cnf)
{
  int i;

  oe_prikaz_all_ungroup_vertex(p_cnf);

  for (i = 0; i < p_cnf->max_kont; i++) {
    if (p_cnf->p_kont[i])
      zrus_kontejner_rec(&p_cnf->p_kont[i], &p_cnf->p_mlha);
  }

  p_cnf->lightnum = 0;
  p_cnf->lightakt = 0;
  for (i = 0; i < MAX_EDIT_SVETEL; i++) {
    p_cnf->light[i].akt = 0;
  }
  memset(&p_cnf->light, 0, sizeof(p_cnf->light[0]) * MAX_EDIT_SVETEL);

  p_cnf->flareakt = 0;
  for (i = 0; i < MAX_EDIT_SVETEL; i++) {
    p_cnf->flare[i].akt = 0;
  }
  memset(&p_cnf->flare, 0, sizeof(p_cnf->flare[0]) * MAX_EDIT_SVETEL);

  p_cnf->dl_lightnum = 0;
  for (i = 0; i < MAX_FLARE_SVETEL; i++) {
    p_cnf->p_dlight[i].akt = 0;
  }
  memset(p_cnf->p_dlight, 0, sizeof(p_cnf->p_dlight[0]) * MAX_FLARE_SVETEL);

  p_cnf->edl_lightnum = 0;
  for (i = 0; i < MAX_FLARE_SVETEL; i++) {
    p_cnf->p_edlight[i].akt = 0;
  }
  memset(p_cnf->p_edlight, 0, sizeof(p_cnf->p_edlight[0]) * MAX_FLARE_SVETEL);

  p_cnf->cnf.druh_sceny = SCENA_LEVEL;

  if (p_cnf->polynum) {
    for (i = 0; i < p_cnf->polynum; i++)
      zrus_vnitrek_poly(p_cnf->p_poly + i);
    free(p_cnf->p_poly);
    p_cnf->p_poly = NULL;
    p_cnf->polynum = 0;
  }

  zrus_mlhokostku_all(p_cnf->p_mlha);
  p_cnf->p_mlha = NULL;
  p_cnf->p_mlha_akt = NULL;

  memset(&p_cnf->zrcadlo, 0, sizeof(p_cnf->zrcadlo));
  p_cnf->zdcadlo_akt = 0;

  p_cnf->tdir.scene_texture_dir[0] = 0;
}

void oe_smaz_scenu_prvky(K_EDITOR * p_cnf)
{
  int i;

  oe_prikaz_all_ungroup_vertex(p_cnf);

  for (i = 0; i < p_cnf->max_kont; i++) {
    if (p_cnf->p_kont[i] && (p_cnf->p_kont[i]->kflag & KONT_PRVEK)) {
      lo_vymaz_svetla_ze_sceny(p_cnf->p_kont[i]);
      zrus_kontejner_rec(&p_cnf->p_kont[i], &p_cnf->p_mlha);
    }
  }
}

void oe_smaz_scenu_okoli(K_EDITOR * p_cnf)
{
  int i;

  oe_prikaz_all_ungroup_vertex(p_cnf);

  for (i = 0; i < p_cnf->max_kont; i++) {
    if (p_cnf->p_kont[i] && !(p_cnf->p_kont[i]->kflag & KONT_PRVEK)) {
      lo_vymaz_svetla_ze_sceny(p_cnf->p_kont[i]);
      zrus_kontejner_rec(&p_cnf->p_kont[i], &p_cnf->p_mlha);
    }
  }

  p_cnf->lightnum = 0;
  p_cnf->lightakt = 0;
  for (i = 0; i < MAX_EDIT_SVETEL; i++) {
    p_cnf->light[i].akt = 0;
  }
  memset(&p_cnf->light, 0, sizeof(p_cnf->light[0]) * MAX_EDIT_SVETEL);

  p_cnf->flareakt = 0;
  for (i = 0; i < MAX_EDIT_SVETEL; i++) {
    p_cnf->flare[i].akt = 0;
  }
  memset(&p_cnf->flare, 0, sizeof(p_cnf->flare[0]) * MAX_EDIT_SVETEL);

  p_cnf->dl_lightnum = 0;
  for (i = 0; i < MAX_FLARE_SVETEL; i++) {
    p_cnf->p_dlight[i].akt = 0;
  }
  memset(p_cnf->p_dlight, 0, sizeof(p_cnf->p_dlight[0]) * MAX_FLARE_SVETEL);

  p_cnf->edl_lightnum = 0;
  for (i = 0; i < MAX_FLARE_SVETEL; i++) {
    p_cnf->p_edlight[i].akt = 0;
  }
  memset(p_cnf->p_edlight, 0, sizeof(p_cnf->p_edlight[0]) * MAX_FLARE_SVETEL);

  p_cnf->cnf.druh_sceny = SCENA_LEVEL;

  if (p_cnf->polynum) {
    for (i = 0; i < p_cnf->polynum; i++)
      zrus_vnitrek_poly(p_cnf->p_poly + i);
    free(p_cnf->p_poly);
    p_cnf->p_poly = NULL;
    p_cnf->polynum = 0;
  }

  zrus_mlhokostku_all(p_cnf->p_mlha);
  p_cnf->p_mlha = NULL;
  p_cnf->p_mlha_akt = NULL;

  memset(&p_cnf->zrcadlo, 0, sizeof(p_cnf->zrcadlo));
  p_cnf->zdcadlo_akt = 0;
}

void oe_prikaz_uloz_projekt(K_EDITOR * p_cnf)
{
  byte file[50];

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
      oe_oznac_pouzite_materialy(p_cnf);
      oe_uloz_projekt(p_cnf, file, work_dir, FALSE, FALSE);
    }
  }
}

void oe_prikaz_nahraj_projekt(K_EDITOR * p_cnf)
{
  byte file[50];

  chdir(p_cnf->dir.scene_dir);
  if (doe_dialog_load(hwnd_aplikace, file, 49, "Nahrat scenu", MASKA_SCENY,
      "Scena")) {
    oe_nahraj_projekt(p_cnf, file, work_dir, TRUE, TRUE);
    strcpy(p_cnf->tdir.scene_texture_dir, work_dir);
  }
  doe_updatuj_lista_objekty(p_cnf);
}

void oe_prikaz_uloz_materialy(K_EDITOR * p_cnf)
{
  byte file[100];

  chdir(p_cnf->dir.material_dir);
  if (!doe_dialog_save(file, 99, "Ulozeni materialu", MASKA_MATERIAL,
      "Materialove soubory")) {
    return;
  }
  else {
    lo_uloz_materialy(p_cnf->p_mat, MAX_EDIT_MATERIALU, file,
      p_cnf->dir.material_dir);
  }
}

void oe_prikaz_nahraj_materialy(K_EDITOR * p_cnf, int maz)
{
  byte file[100];

  chdir(p_cnf->dir.material_dir);
  if (doe_dialog_load(hwnd_aplikace, file, 99, "Nahrani materialu",
      MASKA_MATERIAL, "Materialove soubory")) {
    if (maz)
      lo_vymaz_materialy(p_cnf->p_mat, MAX_EDIT_MATERIALU, p_cnf->p_text,
        MAX_EDIT_TEXTUR);
    if (lo_nahraj_materialy(p_cnf->p_mat, MAX_EDIT_MATERIALU, p_cnf->p_text,
        MAX_EDIT_TEXTUR, file)) {
      oe_reload_textur(p_cnf);
    }
  }
}

// ulozi vybrany material
void oe_prikaz_uloz_vybrany_material(K_EDITOR * p_cnf)
{
  byte file[100];

  chdir(p_cnf->dir.material_dir);

  if (p_cnf->p_mat[p_cnf->km.vybr_mat]) {
    if (doe_dialog_save(file, 99, "Ulozeni materialu", MASKA_MATERIAL,
        "Materialove soubory")) {
      lo_uloz_material(p_cnf->p_mat[p_cnf->km.vybr_mat], file, ".");
    }
  }
  else
    kprintf(1, "Uloz-material: Neni vybran zadny material!");
}

// nahraje vybrany material
void oe_prikaz_nahraj_vybrany_material(K_EDITOR * p_cnf)
{
  byte file[100];

  chdir(p_cnf->dir.material_dir);
  if (doe_dialog_load(hwnd_aplikace, file, 99, "Nahrani materialu",
      MASKA_MATERIAL, "Materialove soubory")) {
    if (lo_nahraj_materialy(p_cnf->p_mat, MAX_EDIT_MATERIALU, p_cnf->p_text,
        MAX_EDIT_TEXTUR, file)) {
      oe_reload_textur(p_cnf);
    }
  }
}


/************************************************************************
 Hledaci rutiny - co hledaji ruzne veci jako treba
 volne textury, plne textury a podobne
 ************************************************************************
*/
int oe_vyber_volny_kontejner(K_EDITOR * p_cnf)
{
  int i, k = p_cnf->vybr_kont;

  if (!p_cnf->p_kont[k])
    return (TRUE);

  for (i = 0; i < p_cnf->max_kont; i++) {
    if (!p_cnf->p_kont[i]) {
      p_cnf->vybr_kont = i;
      return (TRUE);
    }
  }
  return (K_CHYBA);
}

int oe_vyber_volny_objekt(K_EDITOR * p_cnf)
{
  int i, k = p_cnf->vybr_kont, o = p_cnf->vybr_objekt;

  if (!p_cnf->p_kont[k])
    return (K_CHYBA);
  if (p_cnf->p_kont[k] && !p_cnf->p_kont[k]->p_obj[o])
    return (TRUE);
  for (i = 0; i < p_cnf->p_kont[k]->max_objektu; i++) {
    if (!p_cnf->p_kont[k]->p_obj[i]) {
      p_cnf->vybr_objekt = i;
      return (TRUE);
    }
  }
  return (K_CHYBA);
}

int oe_najdi_volny_objekt(K_EDITOR * p_cnf)
{
  int i, k = p_cnf->vybr_kont, o = p_cnf->vybr_objekt;

  if (!p_cnf->p_kont[k])
    return (FALSE);
  if (p_cnf->p_kont[k] && !p_cnf->p_kont[k]->p_obj[o])
    return (o);
  for (i = 0; i < p_cnf->p_kont[k]->max_objektu; i++) {
    if (!p_cnf->p_kont[k]->p_obj[i]) {
      return (i);
    }
  }
  return (K_CHYBA);
}

EDIT_KONTEJNER *oe_najdi_volny_kontejner(K_EDITOR * p_cnf)
{
  int k = lo_najdi_volny_kontejner(p_cnf->p_kont, KONT_NUM);

  p_cnf->p_kont[k] = vyrob_kontejner();
  return (p_cnf->p_kont[k]);
}

/*
  Save/load berusci sceny
  Ulozi:
  Poly-listy
  Lightmapy
  Dynamicke Kontejnery (ty co zbydou)
*/
void oe_uloz_berusci_level(K_EDITOR * p_cnf, byte * p_level)
{
  int i;

  byte levelname[200];
  byte dirname[200];

  strcpy(levelname, p_level);
  zamen_koncovku(levelname, "");

  chdir(p_cnf->dir.game_level_dir);
  _mkdir(levelname);
  chdir(levelname);
  _getcwd(dirname, 200);

  /* mnozic & transformator
   */
  oe_rozmnoz_staticke(p_cnf);

  /* Rozlozi staticke a pruhledne kontejnery
   */
  if (p_cnf->cnf.delit_materialy) {
    for (i = 0; i < p_cnf->max_kont; i++) {
      if (p_cnf->p_kont[i]) {
        ds_kontejner_rozloz(p_cnf, p_cnf->p_kont[i], MAT_PRUHLEDNY);
      }
    }
  }

  /* Prevedu vsechny svetla statickych kontejneru do world-space
     a odriznu od nich jejich svetla
   */

  for (i = 0; i < p_cnf->max_kont; i++) {
    if (p_cnf->p_kont[i]) {
      lo_premapuj_svetla_do_wordspace(p_cnf->p_kont[i]);
    }
  }

  /*
     Ulozi poly-listy statickych objektu
   */
  lo_uloz_poly_list(zamen_koncovku(levelname, ".ply"), p_cnf->p_poly,
    p_cnf->polynum, p_cnf->p_lightmap, p_cnf->p_mat, MAX_EDIT_MATERIALU);

  /*
     Ulozi lightmapy
   */
  ray_uloz_lightmapy(p_cnf, zamen_koncovku(levelname, ""), dirname);

  /* U statickych vertex-objektu prevede ray-tracovane barvy
     do materialu
   */
  ray_preved_barvy_do_materialu(p_cnf);

  /*
     Prevede do stripu
   */
  for (i = 0; i < p_cnf->max_kont; i++) {
    if (p_cnf->p_kont[i]) {
      kprintf(TRUE, "Strip-kont %s...", p_cnf->p_kont[i]->jmeno);
      oe_kont_to_strip(p_cnf->p_kont[i]);
    }
  }

  /*
     Ulozi zbyle dynamicke kontejnery
   */
  oe_uloz_projekt(p_cnf, p_level, dirname, TRUE, TRUE);

  /*
     Uloz level - kofolovy informace
   */
  be_save_level(&p_cnf->bnf, zamen_koncovku(levelname, KONCOVKA_LEVELU),
    dirname);

  /*
     Ulozi materialy + exportuje animovane materialy
   */
  oe_uloz_pouzite_materialy(p_cnf, p_level, dirname);
}

void oe_kontejner_to_game(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  int h = oe_vybr_kont_reset();

  while ((p_kont = oe_vybr_kont_get(&h))) {
    obb_calc_kont(p_kont, TRUE);
    oe_kont_to_strip(p_kont);
    oe_prikaz_uloz_kontejner(p_cnf, FALSE);
  }
}

void oe_oznac_pouzite_materialy(K_EDITOR * p_cnf)
{
  int i, o, m;

  for (m = 0; m < MAX_CELKEM_MATERIALU; m++) {
    if (p_cnf->p_mat[m]) {
      p_cnf->p_mat[m]->flag &= ~MAT_POUZITY;
      if (p_cnf->p_mat[m]->flag & MAT_SCENE)
        p_cnf->p_mat[m]->flag |= MAT_POUZITY;
    }
  }

  for (i = 0; i < p_cnf->max_kont; i++) {
    if (p_cnf->p_kont[i]) {
      for (o = 0; o < p_cnf->p_kont[i]->max_objektu; o++) {
        if (p_cnf->p_kont[i]->p_obj[o]) {
          m = p_cnf->p_kont[i]->p_obj[o]->material;
          assert(p_cnf->p_mat[m]);
          p_cnf->p_mat[m]->flag |= MAT_POUZITY;
        }
      }
    }
  }
}

void oe_oznac_pouzite_textury(K_EDITOR * p_cnf)
{
  int m;

  for (m = 0; m < MAX_EDIT_TEXTUR; m++) {
    if (p_cnf->p_text[m].load)
      p_cnf->p_text[m].flag2 &= ~TEXT2_POUZITA;
  }

  for (m = 0; m < MAX_CELKEM_MATERIALU; m++) {
    if (p_cnf->p_mat[m] &&
      p_cnf->p_mat[m]->flag & MAT_POUZITY && p_cnf->p_mat[m]->p_text[0])
      p_cnf->p_mat[m]->p_text[0]->flag2 |= TEXT2_POUZITA;
  }
}

void oe_oznac_pouzite_materialy_kont(K_EDITOR * p_cnf,
  EDIT_KONTEJNER * p_kont)
{
  int o, m;

  for (m = 0; m < MAX_CELKEM_MATERIALU; m++) {
    if (p_cnf->p_mat[m]) {
      p_cnf->p_mat[m]->flag &= ~MAT_POUZITY;
      if (p_cnf->p_mat[m]->flag & MAT_SCENE)
        p_cnf->p_mat[m]->flag |= MAT_POUZITY;
    }
  }

  if (p_kont) {
    for (o = 0; o < p_kont->max_objektu; o++) {
      if (p_kont->p_obj[o]) {
        m = p_kont->p_obj[o]->material;
        assert(p_cnf->p_mat[m]);
        p_cnf->p_mat[m]->flag |= MAT_POUZITY;
      }
    }
  }
}

int oe_oznac_dlouhe_materialy_kont(K_EDITOR * p_cnf, EDIT_KONTEJNER * p_kont)
{
  int m, l, dl = 0;

  for (m = 0; m < MAX_CELKEM_MATERIALU; m++) {
    if (p_cnf->p_mat[m] && p_cnf->p_mat[m]->flag & MAT_POUZITY) {
      l = strlen(p_cnf->p_mat[m]->textfile[0]);
      if (l > 8 + 3 + 1) {
        kprintf(TRUE, "DL Mat '%s' Text '%s'", p_cnf->p_mat[m]->jmeno,
          p_cnf->p_mat[m]->textfile[0]);
        dl++;
      }
    }
  }
  return (dl);
}

void oe_uloz_pouzite_materialy(K_EDITOR * p_cnf, byte * p_file, byte * p_dir)
{
  lo_uloz_materialy_pouzite(p_cnf->p_mat, MAX_EDIT_MATERIALU, p_file, p_dir);
}

void oe_prikaz_uloz_berusci_level(K_EDITOR * p_cnf)
{
  byte pom[100];

  chdir(p_cnf->dir.game_level_dir);
  if (doe_dialog_save(pom, 100, "Save Berusky2 game level", "*.lv6",
      "Game level")) {
    oe_uloz_berusci_level(p_cnf, pom);
  }
}

void oe_prikaz_hrej_berusci_level(K_EDITOR * p_cnf)
{
  if (p_cnf->bnf.p_lev) {
    kprintf(TRUE, "Hraju aktivni level pomoci '%s'", p_cnf->berusky_hra);

    chdir(p_cnf->dir.game_level_dir);
    oe_uloz_berusci_level(p_cnf, "tmp");

    chdir(p_cnf->berusky_hra_dir);
    _spawnl(_P_WAIT, p_cnf->berusky_hra, p_cnf->berusky_hra, "tmp.lv6", NULL);
  }
  else {
    ddw("Neni zadny level ke hrani !");
  }
}

int oe_reload_textur(K_EDITOR * p_cnf)
{
  kprintf(TRUE, "Reload textur...");
  if (p_cnf->cnf.load_textur) {
//    ilInit();
    lo_reload_textur_dir(p_cnf->p_text, MAX_EDIT_TEXTUR, work_dir, FALSE);
    if (p_cnf->tdir.scene_texture_dir[0]) {
      lo_reload_textur_dir(p_cnf->p_text, MAX_EDIT_TEXTUR,
        p_cnf->tdir.scene_texture_dir, FALSE);
    }
    lo_reload_textur(&p_cnf->tdir, p_cnf->p_text, MAX_EDIT_TEXTUR, FALSE);
    lo_reload_stage(p_cnf->p_mat, MAX_EDIT_MATERIALU);
//    ilShutDown();
  }
  return (TRUE);
}

int oe_smaz_textury(K_EDITOR * p_cnf)
{
  int i;

  for (i = 0; i < MAX_EDIT_TEXTUR; i++) {
    txt_zrus_2D_texturu(&p_cnf->p_text[i].text);
  }
  return (TRUE);
}

void oe_prikaz_vypis_pouzite_materialy(K_EDITOR * p_cnf)
{
  int i;

  kprintf(TRUE, "Materialy pouzite ve scene:");
  oe_oznac_pouzite_materialy(p_cnf);
  for (i = 0; i < MAX_EDIT_MATERIALU; i++) {
    if (p_cnf->p_mat[i] && p_cnf->p_mat[i]->flag & MAT_POUZITY) {
      kprintf(TRUE, "%s : Text1 : %s", p_cnf->p_mat[i]->jmeno,
        p_cnf->p_mat[i]->textfile[0]);
      if (p_cnf->p_mat[i]->textfile[1][0])
        kprintf(TRUE, "%s : Text2 : %s", p_cnf->p_mat[i]->jmeno,
          p_cnf->p_mat[i]->textfile[1]);
    }
  }
}

void oe_prikaz_vypis_pouzite_materialy_kontejner(K_EDITOR * p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  int handle;
  int i;

  if (!(handle = oe_vybr_kont_reset()))
    return;

  while (p_kont = oe_vybr_kont_get(&handle)) {
    kprintf(TRUE, "Materialy kontejneru %s", p_kont->jmeno);
    oe_oznac_pouzite_materialy_kont(p_cnf, p_kont);
    for (i = 0; i < MAX_EDIT_MATERIALU; i++) {
      if (p_cnf->p_mat[i] && p_cnf->p_mat[i]->flag & MAT_POUZITY) {
        kprintf(TRUE, "%c %s : Text1 : %s",
          (p_cnf->p_mat[i]->flag & MAT_PRUHLEDNY) ? 'P' : 'N',
          p_cnf->p_mat[i]->jmeno, p_cnf->p_mat[i]->textfile[0]);
      }
    }
  }
  doe_konzole_max(p_cnf);
}

void oe_prikaz_vypis_nenahrane_textury(K_EDITOR * p_cnf)
{
  int t;

  kprintf(TRUE, "Chyba loadu textur:");
  for (t = 0; t < MAX_EDIT_TEXTUR; t++) {
    if (!p_cnf->p_text[t].load && p_cnf->p_text[t].jmeno[0]) {
      kprintf(TRUE, "%s", p_cnf->p_text[t].jmeno);
    }
  }
  doe_konzole_max(p_cnf);
}

// Exportuje textury flaru
// exportuje do prvniho texturoveho adresare
void oe_prikaz_flare_export_textur(K_EDITOR * p_cnf)
{
  EDIT_MATERIAL *p_mat;
  EDIT_TEXT *p_text;
  int i;
  byte pom[300];

  for (i = 0; i < MAX_EDIT_TEXTUR; i++) {
    if (p_cnf->p_text[i].p_bmp && p_cnf->p_text[i].flag2 & TEXT2_FLARE)
      p_cnf->p_text[i].flag2 |= TEXT2_POUZITA;
  }

  pom[0] = 0;
  if (doe_cti_jmeno_ext(hwnd_aplikace, pom, "Prefix textury")) {
    chdir(p_cnf->tdir.texture_dir[0]);
    for (i = 0; i < MAX_EDIT_MATERIALU; i++) {
      p_mat = p_cnf->p_mat[i];
      if (p_mat) {
        p_text = p_mat->p_text[0];
        if (p_text->p_bmp && p_text->flag2 & TEXT2_FLARE) {
          if (p_text->flag2 & TEXT2_POUZITA) {
            p_text->flag2 &= ~TEXT2_POUZITA;
            strcat(pom, p_text->jmeno);
            zamen_koncovku(pom, ".png");
            strcpy(p_text->jmeno, pom);
            bmp_uloz(p_text->jmeno, p_text->p_bmp);
            zamen_koncovku(pom, ".btx");
            txt_uloz_btx(pom, GL_TEXTURE_1D, 0, 0);
            kprintf(TRUE, "Ukladam texturu %s", pom);
          }
          strcpy(p_mat->textfile[0], p_text->jmeno);
        }
      }
    }
  }
}
