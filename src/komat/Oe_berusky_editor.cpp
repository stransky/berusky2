/*
  Editor k beruskam
*/
#include "mss_on.h"
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <direct.h>
#include <float.h>
#include <assert.h>
#include "objekt_editor_all.h"
#include "..\\kofola\\editor levelu.h"

static K_EDITOR *p_cnf = &cnf;
static POM_PROMENE tmp;

BOOL CALLBACK be_calback_new_level(HWND hDlg, UINT msg, WPARAM wParam,
  LPARAM lParam)
{
  switch (msg) {
    case WM_COMMAND:
      switch (LOWORD(wParam)) {
        case IDCANCEL:
          EndDialog(hDlg, 0);
          break;
        case IDOK:{
            byte pom[21];

            GetWindowText(GetDlgItem(hDlg, IDC_EDIT1), pom, 20);
            tmp.i[1] = atoi(pom);
            GetWindowText(GetDlgItem(hDlg, IDC_EDIT8), pom, 20);
            tmp.i[2] = atoi(pom);
            GetWindowText(GetDlgItem(hDlg, IDC_EDIT9), pom, 20);
            tmp.i[3] = atoi(pom);
            GetWindowText(GetDlgItem(hDlg, IDC_EDIT2), tmp.p_pom, 100);
            EndDialog(hDlg, 1);
            break;
          }
        default:
          return FALSE;
      }
      break;
    case WM_INITDIALOG:{
        byte pom[30];

        itoa(tmp.i[0], pom, 10);
        SetDlgItemText(hDlg, IDC_EDIT1, pom);
        itoa(tmp.i[1], pom, 10);
        SetDlgItemText(hDlg, IDC_EDIT8, pom);
        itoa(tmp.i[2], pom, 10);
        SetDlgItemText(hDlg, IDC_EDIT9, pom);
        SetDlgItemText(hDlg, IDC_EDIT2, tmp.p_pom);
        break;
      }
    case WM_KEYDOWN:
      _a_klavesa = MapVirtualKey(wParam, 2);
      key[MapVirtualKey(wParam, 0)] = TRUE;
      break;
    case WM_DESTROY:
      EndDialog(hDlg, 0);
      break;
    default:
      return FALSE;
  }
  return TRUE;
}

void be_level_to_window(B_KONFIG * p_bnf)
{
  byte pom[100];

  sprintf(pom, "3D Level editor - %s", p_cnf->bnf.jmeno_levelu);
  SetWindowText(hwnd_aplikace, pom);
}

// x,z = sirka,delka
// y = vyska
void be_prikaz_novy_level(B_KONFIG * p_bnf)
{
  int i;

  if (p_bnf->p_lev) {
    if (!ddwqueto(hwnd_aplikace, "Smazat aktualni level ?", "?"))
      return;
  }

  strcpy(p_bnf->jmeno_levelu, DEFAULT_LEVEL);
  zamen_koncovku(p_bnf->jmeno_levelu, NULL);

  tmp.i[0] = p_bnf->default_x;
  tmp.i[1] = p_bnf->default_z;
  tmp.i[2] = p_bnf->default_y;
  tmp.p_pom = p_bnf->jmeno_levelu;

  if (DialogBox(hinst, MAKEINTRESOURCE(IDD_BERUSKY_EDITOR_NEW_LEVEL),
      hwnd_aplikace, be_calback_new_level)) {

    zamen_koncovku(p_bnf->jmeno_levelu, NULL);

    if (p_bnf->p_lev) {
      be_zrus_level(&p_cnf->bnf);
      oe_smaz_scenu(p_cnf);
    }

    be_vyrob_level(p_bnf, tmp.i[1], tmp.i[3], tmp.i[2]);

    chdir(p_cnf->dir.edit_level_dir);
    _mkdir(p_bnf->jmeno_levelu);

    strcpy(p_cnf->dir.level_dir, p_cnf->dir.edit_level_dir);
    strcat(p_cnf->dir.level_dir, "\\");
    strcat(p_cnf->dir.level_dir, p_bnf->jmeno_levelu);
    strcat(p_bnf->jmeno_levelu, KONCOVKA_LEVELU);

    kprintf(1, "Zakladam level %s", p_bnf->jmeno_levelu);

    for (i = 0; i < p_bnf->y_num; i++)
      be_vyrob_grid_level(p_bnf, i);

    p_bnf->cnf.kostka_remake = TRUE;
    be_level_to_window(p_bnf);

    kof_Init_Level_Environment(&p_cnf->env);
  }
}

int be_nahraj_level(K_EDITOR * p_cnf, byte * p_file, int rekonstrukce,
  int textury, int mazat_materialy)
{
  B_KONFIG *p_bnf = &p_cnf->bnf;
  int i;

  if (p_bnf->p_lev) {
    be_zrus_level(&p_cnf->bnf);
    oe_smaz_scenu(p_cnf);
  }
  else {
    oe_zrus_indexy_scena(p_cnf);
  }

  if (mazat_materialy) {
    kprintf(1, "Mazu materialy...");
    lo_vymaz_materialy(p_cnf->p_mat, MAX_EDIT_MATERIALU, p_cnf->p_text, 0);
  }

  kprintf(1, "Nahravam level %s", p_file);
  _getcwd(p_cnf->dir.level_dir, 100);

  if (!rekonstrukce)
    be_nahraj_level_pozadi(p_cnf, p_file, textury);

  if (be_load_level(p_bnf, p_file, p_cnf->dir.level_dir, rekonstrukce)) {
    strcpy(p_bnf->jmeno_levelu, p_file);
    for (i = 0; i < p_bnf->y_num; i++)
      be_vyrob_grid_level(p_bnf, i);

    be_level_to_window(p_bnf);
    p_bnf->cnf.kostka_remake = TRUE;
    return (TRUE);
  }
  else {
    return (FALSE);
  }
}

void be_prikaz_nahraj_level(K_EDITOR * p_cnf, int rekonstrukce)
{
  B_KONFIG *p_bnf = &p_cnf->bnf;
  byte file[200];

  if (p_bnf->p_lev) {
    if (!ddwqueto(hwnd_aplikace, "Prepsat aktualni level ?", "Že by?"))
      return;
  }

  chdir(p_cnf->dir.edit_level_dir);
  if (doe_dialog_load(hwnd_aplikace, file, 99, "Nahrani levelu...",
      MASKA_LEVELU, "Levely do berusek II")) {
    be_nahraj_level(p_cnf, file, rekonstrukce, TRUE,
      p_cnf->cnf.mazat_materialy);
  }
}

int be_nahraj_level_render(K_EDITOR * p_cnf, char *p_level)
{
  B_KONFIG *p_bnf = &p_cnf->bnf;

  if (p_bnf->p_lev) {
    be_zrus_level(&p_cnf->bnf);
    oe_smaz_scenu(p_cnf);
  }
  else {
    oe_zrus_indexy_scena(p_cnf);
  }

  strcpy(p_cnf->dir.level_dir, p_cnf->dir.edit_level_dir);
  strcat(p_cnf->dir.level_dir, "\\");
  strcat(p_cnf->dir.level_dir, p_level);
  chdir(zamen_koncovku(p_cnf->dir.level_dir, NULL));

  kprintf(1, "Nahravam level %s", p_level);

  if (be_nahraj_level_pozadi(p_cnf, p_level, FALSE)) {
    if (be_load_level(p_bnf, p_level, p_cnf->dir.level_dir, FALSE)) {
      strcpy(p_bnf->jmeno_levelu, p_level);
      return (TRUE);
    }
  }
  return (FALSE);
}

void be_prikaz_uloz_level(K_EDITOR * p_cnf)
{
  B_KONFIG *p_bnf = &p_cnf->bnf;

  if (p_cnf->cnf.druh_sceny == SCENA_POLY) {
    ddw
      ("Level je preveden do hry a neda se editovat. Pro export levelu do hry pouzij v menu BeruskyII/export");
    return;
  }

  kprintf(1, "Ukladam level %s", p_bnf->jmeno_levelu);

  zamen_koncovku(p_bnf->jmeno_levelu, NULL);
  chdir(p_cnf->dir.edit_level_dir);
  _mkdir(p_bnf->jmeno_levelu);
  strcpy(p_cnf->dir.level_dir, p_cnf->dir.edit_level_dir);
  strcat(p_cnf->dir.level_dir, "\\");
  strcat(p_cnf->dir.level_dir, p_bnf->jmeno_levelu);
  strcat(p_bnf->jmeno_levelu, KONCOVKA_LEVELU);
  chdir(p_cnf->dir.level_dir);

  if (!p_bnf->p_lev) {
    ddw("Neni co ukladat !");
    return;
  }
  be_kontroluj_level(&p_cnf->bnf);
  be_uloz_level_pozadi(p_cnf, p_cnf->bnf.jmeno_levelu);
  be_save_level(&p_cnf->bnf, p_cnf->bnf.jmeno_levelu, p_cnf->dir.level_dir);
}

void oe_zrus_indexy_scena(K_EDITOR * p_cnf)
{
  int i;

  for (i = 0; i < p_cnf->kontejneru; i++) {
    if (p_cnf->p_kont[i])
      p_cnf->p_kont[i]->kontejner_ID = K_CHYBA;
  }
}

void be_prikaz_uloz_level_jako(K_EDITOR * p_cnf)
{
  B_KONFIG *p_bnf = &p_cnf->bnf;
  byte file[100];

  if (p_cnf->cnf.druh_sceny == SCENA_POLY) {
    ddw
      ("Level je preveden do hry a neda se editovat. Pro export levelu do hry pouzij v menu BeruskyII/export");
    return;
  }

  chdir(p_cnf->dir.edit_level_dir);

  if (!p_bnf->p_lev) {
    ddw("Neni co ukladat !");
    return;
  }

  if (doe_dialog_save(file, 99, "Ulozeni levelu...", MASKA_LEVELU,
      "Levely do berusek II")) {

    be_kontroluj_level(&p_cnf->bnf);

    zamen_koncovku(file, NULL);
    chdir(p_cnf->dir.edit_level_dir);
    _mkdir(file);
    strcpy(p_cnf->dir.level_dir, p_cnf->dir.edit_level_dir);
    strcat(p_cnf->dir.level_dir, "\\");
    strcat(p_cnf->dir.level_dir, file);
    strcat(file, KONCOVKA_LEVELU);
    strcpy(p_bnf->jmeno_levelu, file);
    kprintf(1, "Ukladam level jako %s", file);

    be_uloz_level_pozadi(p_cnf, file);
    be_save_level(p_bnf, file, p_cnf->dir.level_dir);
  }
}

void be_prikaz_smaz_level(K_EDITOR * p_cnf, int level, int scena, int ticho)
{
  B_KONFIG *p_bnf = &p_cnf->bnf;

  system_kurzor = 1;
  if (ticho || ddwqueto(hwnd_aplikace, "Smazat level ?", "?")) {
    if (scena)
      oe_smaz_scenu_okoli(p_cnf);
    if (level) {
      strcpy(p_cnf->bnf.jmeno_levelu, DEFAULT_LEVEL);
      be_zrus_level(&p_cnf->bnf);
      oe_smaz_scenu_prvky(p_cnf);
      oe_zrus_indexy_scena(p_cnf);
    }
    p_bnf->cnf.kostka_remake = TRUE;
    kprintf(1, "Smazan level...");
  }
  system_kurzor = 0;
}

void be_prikaz_vyber_prvku_lista(K_EDITOR * p_cnf)
{
  LISTA_PRVKU *p_lista = &p_cnf->lp;
  int p;

  if (p_lista->aktivni && p_lista->prvnum) {
    p_lista->prvakt = mi.x / LISTA_X;
    p = p_lista->prvakt + p_lista->prvni;

    if (p_lista->prvnum <= p) {
      p_lista->prvakt = p_lista->oken - 1;
      p_lista->prvni = p_lista->prvnum - p_lista->prvakt;
      p = p_lista->prvnum - 1;
    }
    p_cnf->bnf.kp.akt_prvek = p_lista->p_prv[p];
    p_cnf->bnf.kp.vybr_varianta = p_lista->p_pos[p];
    if (p_lista->hDlg) {
      SendMessage(p_lista->hDlg, LB_SETCURSEL, p_lista->p_pos[p], 0);
      //SendMessage(p_lista->hDlg_box,WM_COMMAND, MAKEWPARAM(p_lista->control,LBN_SELCHANGE), (LPARAM)p_lista->hDlg);
    }
  }
}

void be_vyrob_grid_level(B_KONFIG * p_bnf, int patro)
{
  DXBOD *p_v;
  int x = p_bnf->x_num;
  int z = p_bnf->z_num;
  int i, k, vert = ((x + 1) << 1) + ((z + 1) << 1);
  float r = 0.8f, g = 0.8f, b = 0.8f;

  if ((p_v = malloc(sizeof(DXBOD) * vert)) == NULL)
    chyba("Pamet");

  // svisle pruhy
  for (i = 0, k = 0; i < (z + 1) << 1; i += 2, k++) {
    p_v[i].x = 0;
    p_v[i].y = (float) patro *Y_PRVEK;
    p_v[i].z = (float) k *Z_PRVEK;

    p_v[i].r = r;
    p_v[i].g = g;
    p_v[i].b = b;

    p_v[i + 1].x = (float) x *X_PRVEK;
    p_v[i + 1].y = (float) patro *Y_PRVEK;
    p_v[i + 1].z = (float) k *Z_PRVEK;

    p_v[i + 1].r = r;
    p_v[i + 1].g = g;
    p_v[i + 1].b = b;
  }

  // vodorovne pruhy
  for (k = 0; i < vert; i += 2, k++) {
    p_v[i].x = (float) k *X_PRVEK;
    p_v[i].y = (float) patro *Y_PRVEK;

    p_v[i].z = (float) 0;

    p_v[i].r = r;
    p_v[i].g = g;
    p_v[i].b = b;

    p_v[i + 1].x = (float) k *X_PRVEK;
    p_v[i + 1].y = (float) patro *Y_PRVEK;
    p_v[i + 1].z = (float) z *Z_PRVEK;

    p_v[i + 1].r = r;
    p_v[i + 1].g = g;
    p_v[i + 1].b = b;
  }

  if (p_bnf->grid.p_v[patro])
    free(p_bnf->grid.p_v[patro]);

  p_bnf->grid.p_v[patro] = p_v;
  p_bnf->grid.vert = vert;
}

void be_zrus_grid_level(B_KONFIG * p_bnf)
{
  int i;

  for (i = 0; i < MAX_PATER; i++) {
    if (p_bnf->grid.p_v[i]) {
      free(p_bnf->grid.p_v[i]);
      p_bnf->grid.p_v[i] = NULL;
    }
  }
}

void be_kresli_grid_level(B_KONFIG * p_bnf)
{
  int i, j;
  static GLMATRIX m;

  if (!m._11)
    init_matrix(&m);

  m._41 = p_cnf->bnf.x_start;
  m._42 = p_cnf->bnf.y_start;
  m._43 = p_cnf->bnf.z_start;

  set_matrix_world(&m);
  blend_off();
  text_set_num(0);
  text_off(GL_TEXTURE_1D);
  text_off(GL_TEXTURE_2D);

  for (i = 0; i < MAX_PATER; i++) {
    if (p_bnf->grid.kreslit[i] && p_bnf->grid.p_v[i]) {
      glBegin(GL_LINES);
      for (j = 0; j < p_bnf->grid.vert; j++) {
        glColor3f(p_bnf->grid.p_v[i][j].r, p_bnf->grid.p_v[i][j].g,
          p_bnf->grid.p_v[i][j].b);
        glVertex3f(p_bnf->grid.p_v[i][j].x, p_bnf->grid.p_v[i][j].y,
          p_bnf->grid.p_v[i][j].z);
      }
      glEnd();
    }
  }
}

int be_prepni_na_patro(B_KONFIG * p_bnf, int patro)
{
  if (patro > -1 && patro < p_bnf->y_num && p_bnf->grid.akt_patro != patro) {
    byte pom[100];

    memset(p_bnf->grid.kreslit, 0,
      sizeof(p_bnf->grid.kreslit[0]) * MAX_PATER);
    p_bnf->grid.akt_patro = patro;
    p_bnf->grid.kreslit[patro] = TRUE;
    if (p_cnf->bnf.cnf.vybrane_patro) {
      be_prikaz_selekce_patra_on(p_cnf);
    }
    if (p_bnf->kp.lista_aktivni) {
      SetDlgItemText(p_bnf->kp.hwnd_dialog, IDC_EDIT1, itoa(patro, pom, 10));
    }
    return (TRUE);
  }
  return (FALSE);
}

/*
  Vola se jako soucast "renderuj_scenu"

  // tyto prvky budu kreslit
  int           prvnum;
  PRVEK_LEVELU *p_prv_lev[MAX_PRVKU_LEVEL];
*/
void be_kresli_level_kostku(K_EDITOR * p_cnf, byte remake, byte posun)
{
  B_KONFIG *p_bnf = &p_cnf->bnf;
  static EDIT_KONTEJNER *p_kont = NULL;
  static GLMATRIX m;
  BOD s, d;

  if (!p_kont) {
    p_kont = vyrob_kontejner();
  }

  if (remake) {
    posun = TRUE;

    if (p_kont->p_obj[0])
      zrus_objekt(p_kont->p_obj);

    if (p_bnf->p_lev) {
      d.x = (float) (p_bnf->x_num * X_PRVEK);
      d.y = (float) (p_bnf->y_num * Y_PRVEK);
      d.z = (float) (p_bnf->z_num * Z_PRVEK);

      s.x = p_bnf->x_start + d.x / 2.0f;
      s.y = p_bnf->y_start + d.y / 2.0f;
      s.z = p_bnf->z_start + d.z / 2.0f;

      p_kont->p_obj[0] = vyrob_krychli(&s, d.x, d.y, d.z, 0xffffffff);

      p_kont->p_obj[0]->material =
        p_bnf->p_kont[KONT_NEVID_STENA]->p_obj[0]->material;

      updatuj_kontejner_statistika(p_kont, TRUE);
    }
  }

  if (posun) {
    s.x = p_bnf->x_start;
    s.y = p_bnf->y_start;
    s.z = p_bnf->z_start;

    init_matrix(&m);
    m._41 = s.x;
    m._42 = s.y;
    m._43 = s.z;
  }

  if (p_bnf->p_lev) {
    set_matrix_world(&m);
    oe_kresli_objekt(p_cnf, p_kont, p_kont->p_obj[0], p_cnf->p_mat, FALSE,
      FALSE);
    //oe_kresli_objekt_dratove(p_cnf, p_kont, p_kont->p_obj[1], FALSE, FALSE);
    //__DODELAT__
  }
}


void be_kresli_berusky_kontejner(K_EDITOR * p_cnf, EDIT_KONTEJNER * p_kont)
{
  EDIT_OBJEKT *p_obj;
  int i, k = p_kont->max_objektu;

  for (i = 0; i < k; i++) {
    p_obj = p_kont->p_obj[i];
    if (p_obj)
      oe_kresli_objekt(p_cnf, p_kont, p_obj, p_cnf->p_mat, FALSE, FALSE);
  }
}

void be_renderuj_scenu(K_EDITOR * p_cnf)
{
  B_KONFIG *p_bnf = &p_cnf->bnf;
  int i, s;

  if (!p_bnf->p_lev)
    return;

  if (akt_editor == 2) {
    be_kresli_kurzor(p_cnf);
  }
  be_kresli_grid_level(p_bnf);

  /* Rendering stinovych objektu
   */
  if (p_bnf->cnf.kreslit_stinove_objekty) {
    for (i = 0; i < MAX_PRVKU_LEVEL; i++) {
      if (p_bnf->p_prv_lev[i]) {
        set_matrix_world(&p_bnf->p_prv_lev[i]->m);
        s =
          ber_je_objekt_staticky(p_bnf->p_prv_lev[i]->
          p_prv_databaze->guid) ? KONT_NEVID_STENA : KONT_NEVID_PRVEK;
        be_kresli_berusky_kontejner(p_cnf, p_bnf->p_kont[s]);
      }
    }
  }

  if (p_bnf->cnf.kostka) {
    be_kresli_level_kostku(p_cnf, p_bnf->cnf.kostka_remake, FALSE);
    if (p_bnf->cnf.kostka_remake)
      p_bnf->cnf.kostka_remake = FALSE;
  }
}

void be_prepni_kresleni_stat_objektu(K_EDITOR * p_cnf, int kreslit)
{
  PRVEK_LEVELU *p_prv;
  B_KONFIG *p_bnf = &p_cnf->bnf;
  int i;

  for (i = 0; i < MAX_PRVKU_LEVEL; i++) {
    if ((p_prv = p_bnf->p_prv_lev[i])) {
      if (ber_je_objekt_staticky(p_prv->p_prv_databaze->guid)) {
        p_cnf->p_kont[p_prv->mesh]->kreslit = kreslit;
      }
    }
  }
}

void be_prepni_kresleni_dynam_objektu(K_EDITOR * p_cnf, int kreslit)
{
  PRVEK_LEVELU *p_prv;
  B_KONFIG *p_bnf = &p_cnf->bnf;
  int i;

  for (i = 0; i < MAX_PRVKU_LEVEL; i++) {
    if ((p_prv = p_bnf->p_prv_lev[i])) {
      if (!ber_je_objekt_staticky(p_prv->p_prv_databaze->guid)) {
        p_cnf->p_kont[p_prv->mesh]->kreslit = kreslit;
      }
    }
  }
}

void be_prikaz_posun_tridy_lista(K_EDITOR * p_cnf, int smer)
{
  // -1 == dolu
  //  1 == nahoru
  int max;

  if (!p_cnf->bnf.kp.lista_aktivni)
    return;

  max = SendMessage(p_cnf->bnf.kp.hwnd_list_trida, LB_GETCOUNT, 0, 0);

  if (smer > 0) {               // nahoru          
    if (p_cnf->bnf.kp.vybr_trida + 1 < max)
      p_cnf->bnf.kp.vybr_trida += 1;
    else
      return;
  }
  else {                        //dolu
    if (p_cnf->bnf.kp.vybr_trida - 1 >= 0)
      p_cnf->bnf.kp.vybr_trida -= 1;
    else
      return;
  }

  SendMessage(p_cnf->bnf.kp.hwnd_list_trida, LB_SETCURSEL,
    p_cnf->bnf.kp.vybr_trida, 0);
  SendMessage(p_cnf->bnf.kp.hwnd_dialog, WM_COMMAND, MAKEWPARAM(IDC_LIST2,
      LBN_SELCHANGE), (LPARAM) p_cnf->bnf.kp.hwnd_list_trida);
}

void be_prikaz_posun_pod_tridy_lista(K_EDITOR * p_cnf, int smer)
{
  // -1 == dolu
  //  1 == nahoru
  int max;

  if (!p_cnf->bnf.kp.lista_aktivni)
    return;

  max = SendMessage(p_cnf->bnf.kp.hwnd_list_pod_trida, LB_GETCOUNT, 0, 0);

  if (smer > 0) {               // nahoru          
    if (p_cnf->bnf.kp.vybr_podtrida + 1 < max)
      p_cnf->bnf.kp.vybr_podtrida += 1;
    else
      return;
  }
  else {                        //dolu
    if (p_cnf->bnf.kp.vybr_podtrida - 1 >= 0)
      p_cnf->bnf.kp.vybr_podtrida -= 1;
    else
      return;
  }

  SendMessage(p_cnf->bnf.kp.hwnd_list_pod_trida, LB_SETCURSEL,
    p_cnf->bnf.kp.vybr_podtrida, 0);
  SendMessage(p_cnf->bnf.kp.hwnd_dialog, WM_COMMAND, MAKEWPARAM(IDC_LIST1,
      LBN_SELCHANGE), (LPARAM) p_cnf->bnf.kp.hwnd_list_pod_trida);
}

// -1 == dolu
//  1 == nahoru
void be_prikaz_posun_varianty_lista(K_EDITOR * p_cnf, int smer)
{
  B_KONFIG *p_bnf = &p_cnf->bnf;
  SADAR *p_sad = &p_bnf->sad;
  int max;

  if (p_cnf->bnf.kp.lista_aktivni) {
    max = SendMessage(p_cnf->bnf.kp.hwnd_list_varianta, LB_GETCOUNT, 0, 0);
    if (smer > 0) {             // nahoru          
      if (p_cnf->bnf.kp.vybr_varianta + 1 < max)
        p_cnf->bnf.kp.vybr_varianta += 1;
      else
        return;
    }
    else {                      //dolu
      if (p_cnf->bnf.kp.vybr_varianta - 1 >= 0)
        p_cnf->bnf.kp.vybr_varianta -= 1;
      else
        return;
    }
    SendMessage(p_cnf->bnf.kp.hwnd_list_varianta, LB_SETCURSEL,
      p_cnf->bnf.kp.vybr_varianta, 0);
    SendMessage(p_cnf->bnf.kp.hwnd_dialog, WM_COMMAND, MAKEWPARAM(IDC_LIST8,
        LBN_SELCHANGE), (LPARAM) p_cnf->bnf.kp.hwnd_list_varianta);
  }
  else {
    if (p_bnf->sad.hwnd_list_trida) {
      max = SendMessage(p_sad->hwnd_list_trida, LB_GETCOUNT, 0, 0);
      if (smer > 0) {           // nahoru          
        if (p_sad->sada_prvek + 1 < max)
          p_sad->sada_prvek += 1;
        else
          return;
      }
      else {                    //dolu
        if (p_sad->sada_prvek - 1 >= 0)
          p_sad->sada_prvek -= 1;
        else
          return;
      }
      SendMessage(p_sad->hwnd_list_trida, LB_SETCURSEL, p_sad->sada_prvek, 0);
      SendMessage(p_sad->hwnd_dialog, WM_COMMAND, MAKEWPARAM(IDC_LIST1,
          LBN_SELCHANGE), (LPARAM) p_sad->hwnd_list_trida);
    }
  }
}


// s bude 3D bod v levelu podle vybraneho patra a podobne
// true - pokud byl klik od levelu
// false - klik mimo level
int be_mys_do_policka_levelu(K_EDITOR * p_cnf, BOD3D * p_bod)
{
  B_KONFIG *p_bnf = &p_cnf->bnf;

  ROVINA r = { 0.0f, 1.0f, 0.0f,
    (float) (-p_bnf->grid.akt_patro * Y_PRVEK) - Y_ZACATEK
  };
  BOD a, b, p;
  int x, y, z;

  // 1. spocitat prusek paprsku s aktivnim patrem
  // 2. zjistit ktere je to policko
  //Faze 1
  transf_2d_3d((float) mi.x, (float) mi.y, 0,
    &a.x, &a.y, &a.z,
    p_cnf->p_camera, p_cnf->p_project, OXRES, OYRES, OXSTART, OYSTART);
  transf_2d_3d((float) mi.x, (float) mi.y, 0.5f,
    &b.x, &b.y, &b.z,
    p_cnf->p_camera, p_cnf->p_project, OXRES, OYRES, OXSTART, OYSTART);

  if (!calc_prusek_bod(&r, &a, &b, &p))
    return (FALSE);

  //Faze 2
  // p.x , p.z me zajima

  x = (int) (ftoi(p.x) - p_bnf->x_start) / X_PRVEK;
  z = (int) (ftoi(p.z) - p_bnf->z_start) / Z_PRVEK;
  y = p_bnf->grid.akt_patro;

  if (x < 0 || x >= p_bnf->x_num)
    return (FALSE);
  if (z < 0 || z >= p_bnf->z_num)
    return (FALSE);

  p_bod->x = x;
  p_bod->y = y;
  p_bod->z = z;

  return (TRUE);
}


// Vlozi akt prvek na aktualni pozici kuzoru
void be_prikaz_vloz_prvek(K_EDITOR * p_cnf)
{
  B_KONFIG *p_bnf = &p_cnf->bnf;
  int i, d, lay;
  BOD3D b;

  if (!p_bnf->p_lev)
    return;

  // 3. pridat prvek na toto policko
  if (be_mys_do_policka_levelu(p_cnf, &b)) {
    d = p_bnf->kp.akt_prvek;
    if (d != -1) {
      lay = ko_GetLayer(p_bnf->p_prv_dat[d]->guid);
      i = be_pozice_v_levelu(p_bnf, b.x, b.y, b.z, lay);
      be_zrus_prvek_levelu(p_bnf, b.x, b.y, b.z, lay);
      p_bnf->p_lev[i] =
        be_pridej_prvek_levelu_z_databaze(p_bnf, p_bnf->p_prv_dat[d],
        p_bnf->p_prv_dat[d]->guid);
      be_umisti_prvek(p_bnf, p_bnf->p_lev[i], b.x, b.y, b.z);
      kprintf(1, "+ prvek guid %d [%d %d %d] lay %d",
        p_bnf->p_prv_dat[d]->guid, b.x, b.y, b.z, lay);
    }
  }
}

/* Nahradi prvky z databaze
*/
void be_prikaz_nahrad_prvek(K_EDITOR * p_cnf, int guid)
{
  B_KONFIG *p_bnf = &p_cnf->bnf;
  PRVEK_DATABAZE *p_prv_databaze;
  int p, i;
  int x, y, z, pp;
  int Vazba1;
  int Vazba2;
  int Rotace;

  if (!p_bnf->p_lev)
    return;

  for (p = 0; p < p_bnf->prvnum; p++) {
    if (p_bnf->p_prv_lev[p]) {
      p_prv_databaze = p_bnf->p_prv_lev[p]->p_prv_databaze;
      if (guid != p_prv_databaze->guid)
        continue;

      i = p_bnf->p_prv_lev[p]->lev_index;
      Vazba1 = p_bnf->p_lev[i]->Vazba1;
      Vazba2 = p_bnf->p_lev[i]->Vazba2;
      Rotace = p_bnf->p_lev[i]->Rotace;

      be_zrus_prvek_levelu_index(p_bnf, i);
      p_bnf->p_lev[i] =
        be_pridej_prvek_levelu_z_databaze(p_bnf, p_prv_databaze,
        p_prv_databaze->guid);

      p_bnf->p_lev[i]->Vazba1 = Vazba1;
      p_bnf->p_lev[i]->Vazba2 = Vazba2;
      be_pozice_na_disku(i, x, y, z, pp, p_bnf->x_num, p_bnf->z_num);
      be_umisti_prvek(p_bnf, p_bnf->p_lev[i], x, y, z);

      p_bnf->p_lev[i]->Rotace = Rotace;
      be_preumisti_prvek(p_bnf, p_bnf->p_lev[i]);

      kprintf(1, "+ prvek guid %d [%d %d %d] lay %d", p_prv_databaze->guid, x,
        y, z, pp);
    }
  }
}

/* Nahradi prvky z databaze - guidy
*/
void be_prikaz_prepis_guid(K_EDITOR * p_cnf, int guid_min, int guid_max,
  int guid_vysl)
{
  B_KONFIG *p_bnf = &p_cnf->bnf;
  int p;
  int guid;

  if (!p_bnf->p_lev)
    return;

  for (p = 0; p < p_bnf->prvnum; p++) {
    if (p_bnf->p_prv_lev[p]) {
      guid = be_cti_guid(p_bnf->p_prv_lev[p]);
      if (guid_min <= guid && guid <= guid_max) {
        p_bnf->p_prv_lev[p]->guid = guid_vysl;
        kprintf(1, "prvek guid %d -> %d", guid, guid_vysl);
      }
    }
  }
}

void be_prikaz_rotace_prvku(K_EDITOR * p_cnf)
{
  B_KONFIG *p_bnf = &p_cnf->bnf;
  PRVEK_LEVELU *p_lev;
  int i, j;
  BOD3D b;

  if (!p_bnf->p_lev)
    return;

  if (!be_mys_do_policka_levelu(p_cnf, &b))
    return;

  i = be_pozice_v_levelu(p_bnf, b.x, b.y, b.z, 0);
  j = be_pozice_v_levelu(p_bnf, b.x, b.y, b.z, 1);

  if (p_bnf->p_lev[i] && p_bnf->p_lev[j]) {
    i = be_pozice_v_levelu(p_bnf, b.x, b.y, b.z, p_cnf->bnf.cnf.akt_vrstva);
    p_lev = p_bnf->p_lev[i];
  }
  else {
    if (p_bnf->p_lev[i]) {
      p_lev = p_bnf->p_lev[i];
    }
    else {
      p_lev = p_bnf->p_lev[j];
    }
  }
  if (p_lev) {
    p_lev->Rotace = (p_lev->Rotace + 1) % 4;
    be_preumisti_prvek(p_bnf, p_lev);
  }
}

// Vygeneruje ohranicujici obrubu k levelu
void be_prikaz_generuj_okoli_levelu(K_EDITOR * p_cnf)
{



}

void be_prikaz_uprav_prvek_levelu(K_EDITOR * p_cnf)
{
  B_KONFIG *p_bnf = &p_cnf->bnf;
  PRVEK_LEVELU *p_lev;
  int i, j;
  BOD3D b;

  if (!p_bnf->p_lev)
    return;

  if (be_mys_do_policka_levelu(p_cnf, &b)) {

    i = be_pozice_v_levelu(p_bnf, b.x, b.y, b.z, 0);
    j = be_pozice_v_levelu(p_bnf, b.x, b.y, b.z, 1);

    if (p_bnf->p_lev[i] && p_bnf->p_lev[j]) {
      i = be_pozice_v_levelu(p_bnf, b.x, b.y, b.z, p_cnf->bnf.cnf.akt_vrstva);
      p_lev = p_bnf->p_lev[i];
    }
    else {
      if (p_bnf->p_lev[i]) {
        p_lev = p_bnf->p_lev[i];
      }
      else {
        p_lev = p_bnf->p_lev[j];
      }
    }
    if (p_lev) {
      doe_vlastnosti_prvku(hwnd_aplikace, p_lev);
      be_preumisti_prvek(p_bnf, p_lev);
    }
    else {
      kprintf(TRUE, "Policko je prazdne !");
    }
  }
  else {
    kprintf(TRUE, "Neni vybrano policko levelu!");
  }
}

int be_najdi_prvek_v_levelu(B_KONFIG * p_bnf, PRVEK_LEVELU * p_prv)
{
  int i;

  for (i = 0; i < MAX_PRVKU_LEVEL; i++) {
    if (p_bnf->p_prv_lev[i] == p_prv)
      return (i);
  }
  return (K_CHYBA);
}

// Smaze prvek
void be_prikaz_smaz_prvek(K_EDITOR * p_cnf)
{
  B_KONFIG *p_bnf = &p_cnf->bnf;
  BOD3D b;

  if (p_bnf->p_lev && be_mys_do_policka_levelu(p_cnf, &b)) {
    be_zrus_prvek_levelu(p_bnf, b.x, b.y, b.z, 0);
    be_zrus_prvek_levelu(p_bnf, b.x, b.y, b.z, 1);
  }
}

void be_zrus_level_nevid_zdi(B_KONFIG * p_bnf)
{
  int i, trida;

  for (i = 0; i < MAX_PRVKU_LEVEL; i++) {
    if (p_bnf->p_prv_lev[i]) {
      trida = p_bnf->p_prv_lev[i]->p_prv_databaze->guid / 1000;
      if (trida == 21) {
        be_zrus_prvek_levelu_mesh(&p_cnf->bnf, i);
      }
    }
  }
}

void be_kresli_kurzor(K_EDITOR * p_cnf)
{
  B_KONFIG *p_bnf = &p_cnf->bnf;
  GLMATRIX *p_m;
  BOD3D b;

  if (be_mys_do_policka_levelu(p_cnf, &b)) {

    p_bnf->grid.x_kurzor = b.x;
    p_bnf->grid.z_kurzor = b.y;
    p_bnf->grid.y_kurzor = b.z;

    // souradnice v levelu je x,y,z
    p_m = &p_bnf->grid.m;
    init_matrix(p_m);

    p_m->_41 = (float) (X_ZACATEK + b.x * X_PRVEK + X_PRVEK / 2);
    p_m->_42 = (float) (Y_ZACATEK + b.y * Y_PRVEK);
    p_m->_43 = (float) (Z_ZACATEK + b.z * Z_PRVEK + Z_PRVEK / 2);

    set_matrix_world(p_m);
    kresli_objekt_dratove_jednoduse(p_bnf->grid.p_kurzor);
  }
}


void be_klavesy_editace(K_EDITOR * p_cnf)
{
  if (key[K_M] && key[K_CTRL]) {
    key[K_M] = FALSE;
    oe_cekej_na_klavesu(p_cnf);

  }
  else {
    if (key[K_I]) {             // lista nahoru !
      key[K_I] = FALSE;
      be_prikaz_posun_tridy_lista(p_cnf, -1);
    }
    if (key[K_O]) {             // lista dolu !
      key[K_O] = FALSE;
      be_prikaz_posun_tridy_lista(p_cnf, 1);
    }
    if (key[K_K]) {             // lista nahoru !
      key[K_K] = FALSE;
      be_prikaz_posun_pod_tridy_lista(p_cnf, -1);
    }
    if (key[K_L]) {             // lista dolu !
      key[K_L] = FALSE;
      be_prikaz_posun_pod_tridy_lista(p_cnf, 1);
    }
    if (key[K_CARKA]) {         // lista nahoru !
      key[K_CARKA] = FALSE;
      be_prikaz_posun_varianty_lista(p_cnf, -1);
    }
    if (key[K_TECKA]) {         // lista dolu !
      key[K_TECKA] = FALSE;
      be_prikaz_posun_varianty_lista(p_cnf, 1);
    }
    if (mi.t1 && mi.y < LISTA_X) {
      be_prikaz_vyber_prvku_lista(p_cnf);
    }
    if (mi.t1 && key[K_SHIFT]) {
      mi.t1 = FALSE;
      doe_proved_prikaz_menu(ID_K_VLOZITPRVEK, TRUE, TRUE);
    }
    if (mi.t1 && key[K_SLASH]) {
      mi.t1 = FALSE;
      doe_proved_prikaz_menu(ID_K_SMAZATPRVEK, TRUE, TRUE);
    }
    if (key[K_Y]) {
      key[K_Y] = FALSE;
      doe_proved_prikaz_menu(ID_K_VLASTNOSTIPRVKU, TRUE, TRUE);
    }
    if (key[K_E] && !key[K_CTRL]) {
      key[K_E] = FALSE;
      doe_proved_prikaz_menu(ID_K_ROTACEPRVKU, TRUE, TRUE);
    }
    if (key[K_F9] && !key[K_CTRL]) {
      key[K_F9] = FALSE;
      oe_prikaz_hrej_berusci_level(p_cnf);
    }
    if (key[K_U] && !key[K_CTRL]) {
      key[K_U] = FALSE;
      doe_proved_prikaz_menu(ID_K_PATRONAHORU, TRUE, FALSE);
    }
    if (key[K_J] && !key[K_CTRL]) {
      key[K_J] = FALSE;
      doe_proved_prikaz_menu(ID_K_PATRODOLU, TRUE, FALSE);
    }
  }
}

void be_klavesy_load_save(K_EDITOR * p_cnf)
{
  if (key[K_Q] && key[K_CTRL]) {
    key[K_Q] = FALSE;
    oe_cekej_na_klavesu(p_cnf);

    if (key[K_N]) {             // novy level
      key[K_N] = FALSE;
      doe_proved_prikaz_menu(ID_SOUBOR_NOVYLEVEL, TRUE, FALSE);
    }
    if (key[K_M]) {             // smazat
      key[K_M] = FALSE;
      doe_proved_prikaz_menu(ID_SOUBOR_SMAZATLEVEL, TRUE, FALSE);
    }
    if (key[K_P]) {             // smazat
      key[K_P] = FALSE;
      doe_proved_prikaz_menu(ID_SOUBOR_POSUNOUTLEVEL, TRUE, TRUE);
    }
  }
  else {
    if (key[K_F2] && key[K_CTRL]) {     // ulozit level jako
      key[K_F2] = FALSE;
      doe_proved_prikaz_menu(ID_SOUBOR_ULOZITLEVELJAKO, TRUE, FALSE);
    }
    if (key[K_F2] && !key[K_CTRL]) {    // ulozit level
      key[K_F2] = FALSE;
      doe_proved_prikaz_menu(ID_SOUBOR_ULOZITLEVEL, TRUE, FALSE);
    }
    if (key[K_F3]) {            // nahrat level
      key[K_F3] = FALSE;
      doe_proved_prikaz_menu(ID_SOUBOR_NAHRATLEVEL, TRUE, FALSE);
    }
    if (key[K_F8]) {            // nahrat pozadi
      key[K_F8] = FALSE;
      doe_proved_prikaz_menu(ID_SOUBOR_IMPORTOVATOKOLI, TRUE, FALSE);
    }
    if (key[K_E] && key[K_CTRL]) {
      key[K_E] = FALSE;
      kprintf(1, "Export levelu do hry...");
      oe_prikaz_uloz_berusci_level(p_cnf);
    }
  }
}

void be_klavesy_konfigurace(K_EDITOR * p_cnf)
{
  if (key[K_F8] && key[K_CTRL]) {       // samotny prikaz konfigurace
    key[K_CTRL] = FALSE;
    key[K_F8] = FALSE;
    prikaz(doe_prikaz_konfigurace_table(p_cnf, hwnd_aplikace, 0));
  }

  if (key[K_Z] && !key[K_CTRL]) {
    key[K_Z] = FALSE;
    oe_prikaz_selekce_objektu(p_cnf);
  }
  if (key[K_Z] && key[K_CTRL]) {
    key[K_Z] = FALSE;
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
  if (key[K_F11]) {
    key[K_F11] = FALSE;
    doe_prepni_do_mesh_editor();
  }
}

void be_klavesy_patra(B_KONFIG * p_bnf)
{
  int i;

  if (key[K_SHIFT] && !key[K_CTRL]) {
    for (i = K_1; i < K_0; i++) {
      if (key[i])
        be_prepni_na_patro(p_bnf, i - K_1);
    }
  }
}

void be_prikaz_patro_nahoru(K_EDITOR * p_cnf)
{
  be_prepni_na_patro(&p_cnf->bnf, p_cnf->bnf.grid.akt_patro + 1);
}

void be_prikaz_patro_dolu(K_EDITOR * p_cnf)
{
  be_prepni_na_patro(&p_cnf->bnf, p_cnf->bnf.grid.akt_patro - 1);
}

void be_posun_level(K_EDITOR * p_cnf)
{
  B_KONFIG *p_bnf = &p_cnf->bnf;
  int i;

  if (!p_bnf->p_lev)
    return;

  for (i = 0; i < p_bnf->prvnum; i++) {
    be_preumisti_prvek(p_bnf, p_bnf->p_prv_lev[i]);
  }
}

void be_preumisti_level(B_KONFIG * p_bnf)
{
  int i;

  for (i = 0; i < p_bnf->prvnum; i++) {
    if (p_bnf->p_prv_lev[i])
      be_preumisti_prvek(p_bnf, p_bnf->p_prv_lev[i]);
  }
}

void be_prikaz_posun_levelu(B_KONFIG * p_bnf)
{
  VEKTOR v1, v2;
  GLMATRIX mat, m;

  init_matrix(&m);
  calc_transf_3d_2d_matrix(&m, p_cnf->p_camera, p_cnf->p_project, &mat);
  *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);
  transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
    &v1.x, &v1.y, &v1.z, &mat, OXRES, OYRES, OXSTART, OYSTART,
    p_cnf->kam.near_plane, p_cnf->kam.far_plane);

  m._41 = p_bnf->x_start;
  m._42 = p_bnf->y_start;
  m._43 = p_bnf->z_start;

  kprintf(TRUE, "Posun levelu...");

  nuluj_mys();
  while (stisk_mys()) {
    transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
      &v2.x, &v2.y, &v2.z, &mat, OXRES, OYRES, OXSTART, OYSTART,
      p_cnf->kam.near_plane, p_cnf->kam.far_plane);

    if (!key[K_X] && !key[K_Y] && !key[K_Z])
      translate_matrix(&m, v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);
    else {
      if (key[K_X])
        translate_matrix(&m, v2.x - v1.x, 0, 0);
      if (key[K_Y])
        translate_matrix(&m, 0, v2.y - v1.y, 0);
      if (key[K_Z])
        translate_matrix(&m, 0, 0, v2.z - v1.z);
    }

    p_bnf->x_start = m._41;
    p_bnf->y_start = m._42;
    p_bnf->z_start = m._43;

    be_kresli_level_kostku(p_cnf, p_bnf->cnf.kostka_remake, TRUE);

    nuluj_dif();
    oe_renderuj_scenu_pre(p_cnf);
    oe_renderuj_scenu_po(p_cnf);
    transf_2d_3d_z_matrix((float) mi.x, (float) mi.y, *p_cnf->p_vzdal,
      &v1.x, &v1.y, &v1.z, &mat, OXRES, OYRES, OXSTART, OYSTART,
      p_cnf->kam.near_plane, p_cnf->kam.far_plane);
  }
  be_preumisti_level(&p_cnf->bnf);
  kprintf(TRUE, "Posun levelu na [%.3f %.3f %.3f]", p_bnf->x_start,
    p_bnf->y_start, p_bnf->z_start);
}

// Pres vsechny konty co jsou prvky
void oe_rozmnoz_staticke(K_EDITOR * p_cnf)
{
  B_KONFIG *p_bnf = &p_cnf->bnf;
  PRVEK_LEVELU *p_lev;
  int i, m;

  for (i = 0; i < p_bnf->prvnum; i++) {
    p_lev = p_bnf->p_prv_lev[i];
    if (p_lev) {
      m = p_lev->mesh;
      if (p_cnf->p_kont[m]) {
        if (!ber_je_objekt_staticky(p_lev->p_prv_databaze->guid)) {
          p_cnf->p_kont[m]->kflag &= ~KONT_STATIC;
        }
        else {
          p_cnf->p_kont[m]->kflag |= KONT_STATIC;
        }
      }
    }
  }
}

int be_spocitej_prvky(K_EDITOR * p_cnf)
{
  B_KONFIG *p_bnf = &p_cnf->bnf;
  int i, m = 0;

  for (i = 0; i < p_bnf->prvnum; i++) {
    if (p_bnf->p_prv_lev[i]) {
      m++;
    }
  }
  return (m);
}

void be_klavesy_berusky_editor(B_KONFIG * p_bnf)
{
  if (key[K_ESC]) {
    key[K_ESC] = FALSE;
    oe_konec(&cnf);
  }
  if (key[K_CTRL] && key[K_BKSP]) {
    key[K_BKSP] = FALSE;
    doe_proved_prikaz_menu(ID_K_KONFIGURACE_UNDO, TRUE, FALSE);
  }
  oe_klavesy_v_game_modu(p_cnf);
  oe_klavesy_zmena_modu(p_cnf);
  be_klavesy_konfigurace(p_cnf);
  be_klavesy_editace(p_cnf);
  be_klavesy_load_save(p_cnf);
  be_klavesy_patra(&p_cnf->bnf);
}

void be_prikaz_selekce_patra_on(K_EDITOR * p_cnf)
{
  B_KONFIG *p_bnf = &p_cnf->bnf;
  int i, j, mesh, x, y, z, p;

  for (i = 0; i < p_bnf->prvnum; i++) {
    if (p_bnf->p_prv_lev[i]) {
      mesh = p_bnf->p_prv_lev[i]->mesh;

      j = p_bnf->p_prv_lev[i]->lev_index;
      be_pozice_na_disku(j, x, y, z, p, p_bnf->x_num, p_bnf->z_num);

      if (p_cnf->p_kont[mesh])
        p_cnf->p_kont[mesh]->kreslit = (p_bnf->grid.akt_patro == y);
    }
  }
}

void be_prikaz_selekce_patra_off(K_EDITOR * p_cnf)
{
  B_KONFIG *p_bnf = &p_cnf->bnf;
  int i, mesh;

  for (i = 0; i < p_bnf->prvnum; i++) {
    if (p_bnf->p_prv_lev[i]) {
      mesh = p_bnf->p_prv_lev[i]->mesh;
      if (p_cnf->p_kont[mesh])
        p_cnf->p_kont[mesh]->kreslit = TRUE;
    }
  }
}

__inline void face(word * p, int num, int p1, int p2, int p3)
{
  num *= 3;
  p[num] = p1;
  p[num + 1] = p2;
  p[num + 2] = p3;
}

EDIT_OBJEKT *be_vyrob_obrubu_levelu(BOD * p_add, int material,
  int num_x, int num_z,
  float obruba_sirka, float obruba_vyska, float obruba_zeme, int text)
{
  OBJ_VERTEX *p_vertex;
  EDIT_OBJEKT *p_obj;
  word *p_face;
  int i;

  p_obj = vyrob_objekt_komplet(16, 10 * 3);
  p_vertex = p_obj->p_vertex;

  /* Leva placka
   */
  p_vertex[0].x = 0.0f;
  p_vertex[0].y = obruba_zeme;
  p_vertex[0].z = num_z == 1 ? obruba_sirka : (float) (num_z * Z_PRVEK);
  if (text) {
    p_vertex[0].tu1 = (float) (num_z - 1);
    p_vertex[0].tv1 = 0;
  }
  else {
    p_vertex[0].tv1 = (float) (num_z - 1);
    p_vertex[0].tu1 = 0;
  }

  p_vertex[1].x = 0.0f;
  p_vertex[1].y = obruba_zeme + obruba_vyska;
  p_vertex[1].z = num_z == 1 ? obruba_sirka : (float) (num_z * Z_PRVEK);
  if (text) {
    p_vertex[1].tu1 = (float) (num_z);
    p_vertex[1].tv1 = 0;
  }
  else {
    p_vertex[1].tv1 = (float) (num_z);
    p_vertex[1].tu1 = 0;
  }

  p_vertex[2].x = 0.0f;
  p_vertex[2].y = obruba_zeme + obruba_vyska;
  p_vertex[2].z = 0;
  p_vertex[2].tu1 = 0;
  p_vertex[2].tv1 = 0;

  p_vertex[3].x = 0.0f;
  p_vertex[3].y = obruba_zeme;
  p_vertex[3].z = 0;
  if (text) {
    p_vertex[3].tu1 = 1;
    p_vertex[3].tv1 = 0;
  }
  else {
    p_vertex[3].tv1 = 1;
    p_vertex[3].tu1 = 0;
  }


  /* Prava placka
   */
  p_vertex[4].x = num_x == 1 ? obruba_sirka : (float) (num_x * X_PRVEK);
  p_vertex[4].y = obruba_zeme;
  p_vertex[4].z = 0.0f;
  if (text) {
    p_vertex[4].tu1 = 1;
    p_vertex[4].tv1 = (float) num_x;
  }
  else {
    p_vertex[4].tv1 = 1;
    p_vertex[4].tu1 = (float) num_x;
  }

  p_vertex[5].x = num_x == 1 ? obruba_sirka : (float) (num_x * X_PRVEK);
  p_vertex[5].y = obruba_zeme + obruba_vyska;
  p_vertex[5].z = 0.0f;
  if (text) {
    p_vertex[5].tu1 = 0;
    p_vertex[5].tv1 = (float) num_x;
  }
  else {
    p_vertex[5].tv1 = 0;
    p_vertex[5].tu1 = (float) num_x;
  }

  p_vertex[6].x = num_x == 1 ? obruba_sirka : (float) (num_x * X_PRVEK);
  p_vertex[6].y = obruba_zeme + obruba_vyska;
  p_vertex[6].z = (num_z == 1 ? obruba_sirka : num_z * Z_PRVEK);
  if (text) {
    p_vertex[6].tu1 = (float) num_z;
    p_vertex[6].tv1 = (float) num_x;
  }
  else {
    p_vertex[6].tv1 = (float) num_z;
    p_vertex[6].tu1 = (float) num_x;
  }

  p_vertex[7].x = num_x == 1 ? obruba_sirka : (float) (num_x * X_PRVEK);
  p_vertex[7].y = obruba_zeme;
  p_vertex[7].z = (num_z == 1 ? obruba_sirka : num_z * Z_PRVEK);
  if (text) {
    p_vertex[7].tu1 = (float) (num_z - 1);
    p_vertex[7].tv1 = (float) num_x;
  }
  else {
    p_vertex[7].tv1 = (float) (num_z - 1);
    p_vertex[7].tu1 = (float) num_x;
  }


  /* Leva strana
   */
  p_vertex[8].x = 0.0f;
  p_vertex[8].y = obruba_zeme;
  p_vertex[8].z = num_z == 1 ? obruba_sirka : (float) (num_z * Z_PRVEK);
  if (text) {
    p_vertex[8].tu1 = (float) num_z;
    p_vertex[8].tv1 = 0;
  }
  else {
    p_vertex[8].tv1 = (float) num_z;
    p_vertex[8].tu1 = 0;
  }


  p_vertex[9].x = 0.0f;
  p_vertex[9].y = obruba_zeme + obruba_vyska;
  p_vertex[9].z = num_z == 1 ? obruba_sirka : (float) (num_z * Z_PRVEK);
  if (text) {
    p_vertex[9].tu1 = (float) num_z;
    p_vertex[9].tv1 = 1;
  }
  else {
    p_vertex[9].tv1 = (float) num_z;
    p_vertex[9].tu1 = 1;
  }

  p_vertex[10].x = 0.0f;
  p_vertex[10].y = obruba_zeme + obruba_vyska;
  p_vertex[10].z = 0;
  if (text) {
    p_vertex[10].tu1 = 0;
    p_vertex[10].tv1 = 1;
  }
  else {
    p_vertex[10].tv1 = 0;
    p_vertex[10].tu1 = 1;
  }

  p_vertex[11].x = 0.0f;
  p_vertex[11].y = obruba_zeme;
  p_vertex[11].z = 0;
  if (text) {
    p_vertex[11].tu1 = 0;
    p_vertex[11].tv1 = 0;
  }
  else {
    p_vertex[11].tv1 = 0;
    p_vertex[11].tu1 = 0;
  }

  /* Prava placka
   */
  p_vertex[12].x = num_x == 1 ? obruba_sirka : (float) (num_x * X_PRVEK);
  p_vertex[12].y = obruba_zeme;
  p_vertex[12].z = 0.0f;
  if (text) {
    p_vertex[12].tu1 = (float) num_z;
    p_vertex[12].tv1 = 0;
  }
  else {
    p_vertex[12].tv1 = (float) num_z;
    p_vertex[12].tu1 = 0;
  }

  p_vertex[13].x = num_x == 1 ? obruba_sirka : (float) (num_x * X_PRVEK);
  p_vertex[13].y = obruba_zeme + obruba_vyska;
  p_vertex[13].z = 0.0f;
  if (text) {
    p_vertex[13].tu1 = (float) num_z;
    p_vertex[13].tv1 = 1;
  }
  else {
    p_vertex[13].tv1 = (float) num_z;
    p_vertex[13].tu1 = 1;
  }
  p_vertex[14].x = num_x == 1 ? obruba_sirka : (float) (num_x * X_PRVEK);
  p_vertex[14].y = obruba_zeme + obruba_vyska;
  p_vertex[14].z = num_z == 1 ? obruba_sirka : (float) (num_z * Z_PRVEK);
  if (text) {
    p_vertex[14].tu1 = 0;
    p_vertex[14].tv1 = 1;
  }
  else {
    p_vertex[14].tv1 = 0;
    p_vertex[14].tu1 = 1;
  }
  p_vertex[15].x = num_x == 1 ? obruba_sirka : (float) (num_x * X_PRVEK);
  p_vertex[15].y = obruba_zeme;
  p_vertex[15].z = num_z == 1 ? obruba_sirka : (float) (num_z * Z_PRVEK);
  if (text) {
    p_vertex[15].tu1 = 0;
    p_vertex[15].tv1 = 0;
  }
  else {
    p_vertex[15].tv1 = 0;
    p_vertex[15].tu1 = 0;
  }

  for (i = 0; i < 16; i++) {
    p_vertex[i].x += p_add->x;
    p_vertex[i].y += p_add->y;
    p_vertex[i].z += p_add->z;
  }

  p_face = p_obj->p_face;

  face(p_face, 0, 0, 6, 7);
  face(p_face, 1, 0, 1, 6);

  face(p_face, 2, 1, 5, 6);
  face(p_face, 3, 1, 2, 5);

  face(p_face, 4, 2, 4, 5);
  face(p_face, 5, 2, 3, 4);

  face(p_face, 6, 4 + 8, 7 + 8, 5 + 8);
  face(p_face, 7, 5 + 8, 7 + 8, 6 + 8);

  face(p_face, 8, 3 + 8, 2 + 8, 0 + 8);
  face(p_face, 9, 2 + 8, 1 + 8, 0 + 8);

  p_obj->material = material;
  nastav_barvu_bodu_obj(p_obj, 1, 1, 1, 1);

  return (p_obj);
}

#define OBRUBA_SIRKA 0.3f
#define OBRUBA_VYSKA 0.3f
#define OBRUBA_ZEME  0.1f

// Prida do levelu obrubu
void be_prikaz_vyrob_obrubu_levelu(K_EDITOR * p_cnf, int material, int fast)
{
  EDIT_KONTEJNER *p_kont;
  B_KONFIG *p_bnf = &p_cnf->bnf;
  BOD add;
  int k;
  float obruba_sirka = OBRUBA_SIRKA;
  float obruba_vyska = OBRUBA_VYSKA;
  float obruba_zeme = OBRUBA_ZEME;
  int text_x = FALSE;

  if (!p_bnf->p_lev) {
    kprintf(TRUE, "Neni aktivni level!");
    return;
  }

  if (!fast) {
    if (!doe_obruba_levelu(p_cnf, &obruba_vyska, &obruba_zeme, &obruba_sirka,
        &text_x)) {
      return;
    }
  }

  for (k = 0; k < p_cnf->max_kont; k++) {
    if (p_cnf->p_kont[k] && p_cnf->p_kont[k]->kflag & KONT_OBRUBA)
      zrus_kontejner_rec(p_cnf->p_kont + k, &p_cnf->p_mlha);
  }

  // dolni lista
  k = lo_najdi_volny_kontejner(p_cnf->p_kont, p_cnf->max_kont);
  p_kont = p_cnf->p_kont[k] = vyrob_kontejner();
  p_kont->kflag |= KONT_OBRUBA;
  add.x = p_bnf->x_start;
  add.y = p_bnf->y_start;
  add.z = p_bnf->z_start - obruba_sirka;
  p_kont->p_obj[0] =
    be_vyrob_obrubu_levelu(&add, material, p_bnf->x_num, 1, obruba_sirka,
    obruba_vyska, obruba_zeme, text_x);
  updatuj_kontejner_statistika(p_kont, TRUE);

  // horni lista
  k = lo_najdi_volny_kontejner(p_cnf->p_kont, p_cnf->max_kont);
  p_kont = p_cnf->p_kont[k] = vyrob_kontejner();
  p_kont->kflag |= KONT_OBRUBA;
  add.x = p_bnf->x_start;
  add.y = p_bnf->y_start;
  add.z = p_bnf->z_start + p_bnf->z_num * X_PRVEK;
  p_kont->p_obj[0] =
    be_vyrob_obrubu_levelu(&add, material, p_bnf->x_num, 1, obruba_sirka,
    obruba_vyska, obruba_zeme, text_x);
  updatuj_kontejner_statistika(p_kont, TRUE);

  // bocni lista 1
  k = lo_najdi_volny_kontejner(p_cnf->p_kont, p_cnf->max_kont);
  p_kont = p_cnf->p_kont[k] = vyrob_kontejner();
  p_kont->kflag |= KONT_OBRUBA;
  add.x = p_bnf->x_start - obruba_vyska;
  add.y = p_bnf->y_start;
  add.z = p_bnf->z_start;
  p_kont->p_obj[0] =
    be_vyrob_obrubu_levelu(&add, material, 1, p_bnf->z_num, obruba_sirka,
    obruba_vyska, obruba_zeme, !text_x);
  updatuj_kontejner_statistika(p_kont, TRUE);

  // bocni lista 2
  k = lo_najdi_volny_kontejner(p_cnf->p_kont, p_cnf->max_kont);
  p_kont = p_cnf->p_kont[k] = vyrob_kontejner();
  p_kont->kflag |= KONT_OBRUBA;
  add.x = p_bnf->x_start + p_bnf->x_num * X_PRVEK;
  add.y = p_bnf->y_start;
  add.z = p_bnf->z_start;
  p_kont->p_obj[0] =
    be_vyrob_obrubu_levelu(&add, material, 1, p_bnf->z_num, obruba_sirka,
    obruba_vyska, obruba_zeme, !text_x);
  updatuj_kontejner_statistika(p_kont, TRUE);
}

int be_najdi_obruba_material(K_EDITOR * p_cnf)
{
  int i, k;

  for (k = 0; k < p_cnf->max_kont; k++) {
    if (p_cnf->p_kont[k] && p_cnf->p_kont[k]->kflag & KONT_OBRUBA) {
      for (i = 0; i < p_cnf->p_kont[k]->objektu; i++) {
        if (p_cnf->p_kont[k]->p_obj[i])
          return (p_cnf->km.vybr_mat = p_cnf->p_kont[k]->p_obj[i]->material);
      }
    }
  }
  return (p_cnf->km.vybr_mat);
}

int be_najdi_obrubu(K_EDITOR * p_cnf)
{
  int k;

  for (k = 0; k < p_cnf->max_kont; k++) {
    if (p_cnf->p_kont[k] && p_cnf->p_kont[k]->kflag & KONT_OBRUBA)
      return (k);
  }
  return (K_CHYBA);
}

/* Vyrobi podlozku pod level
*/
#define VYSKA_PODLOZKY 0.1f

void be_prikaz_vyrob_podlozku_levelu(K_EDITOR * p_cnf, int material)
{
  EDIT_KONTEJNER *p_kont;
  B_KONFIG *p_bnf = &p_cnf->bnf;
  int x, z, k;
  BOD add;
  float vyska_podlozky = VYSKA_PODLOZKY;

  if (!p_bnf->p_lev) {
    kprintf(TRUE, "Neni aktivni level!");
    return;
  }

  if (!doe_podlozka_levelu(p_cnf, &vyska_podlozky)) {
    return;
  }

  for (k = 0; k < p_cnf->max_kont; k++) {
    if (p_cnf->p_kont[k] && p_cnf->p_kont[k]->kflag & KONT_PODLOZKA)
      zrus_kontejner_rec(p_cnf->p_kont + k, &p_cnf->p_mlha);
  }

  for (x = 0; x < p_bnf->x_num; x++) {
    for (z = 0; z < p_bnf->z_num; z++) {
      k = lo_najdi_volny_kontejner(p_cnf->p_kont, p_cnf->max_kont);
      p_kont = p_cnf->p_kont[k] = vyrob_kontejner();
      p_kont->kflag |= (KONT_PODLOZKA | KONT_PRVEK);
      add.x = p_bnf->x_start + x * X_PRVEK;
      add.y = p_bnf->y_start - vyska_podlozky;
      add.z = p_bnf->z_start + z * Z_PRVEK;
      p_kont->p_obj[0] =
        be_vyrob_obrubu_levelu(&add, material, 1, 1, X_PRVEK, vyska_podlozky,
        0, FALSE);
      updatuj_kontejner_statistika(p_kont, TRUE);
    }
  }
}

void be_prikaz_vyrezani_levelu(K_EDITOR * p_cnf)
{
  int flag = p_cnf->elm.flag;

  vektor_set_all(&p_cnf->elm.norm, 0, 1, 0);

  p_cnf->elm.flag =
    ELIM_MAZAT | ELIM_STATIC | ELIM_PRVKY | ELIM_NORMAL | ELIM_INTELIGENT_2 |
    ELIM_INTELIGENT_2_BRUTAL;
  oe_prikaz_decimace_sceny(p_cnf);
  if (p_cnf->groupfacenum) {
    oe_prikaz_smaz_face(p_cnf, TRUE);
  }

  p_cnf->elm.flag =
    ELIM_MAZAT | ELIM_STATIC | ELIM_DYNAMIC | ELIM_PRVKY | ELIM_NORMAL;
  oe_prikaz_decimace_sceny(p_cnf);
  if (p_cnf->groupfacenum) {
    oe_prikaz_smaz_face(p_cnf, TRUE);
  }

  p_cnf->elm.flag = flag;
}

// Nahradi aktualni prvek
void be_prikaz_nahrad_aktualni_prvek(K_EDITOR * p_cnf)
{
  B_KONFIG *p_bnf = &p_cnf->bnf;
  BOD3D b;
  int guid, i, j, vrstva, d;
  int x, y, z, lay;

  d = p_bnf->kp.akt_prvek;
  if (d == -1) {
    kprintf(FALSE, "Neni vybrany zadny prvek.");
    return;
  }

  if (p_bnf->p_lev && be_mys_do_policka_levelu(p_cnf, &b)) {
    i = be_pozice_v_levelu(p_bnf, b.x, b.y, b.z, 0);
    j = be_pozice_v_levelu(p_bnf, b.x, b.y, b.z, 1);
    if (p_bnf->p_lev[i] && p_bnf->p_lev[j]) {
      vrstva = p_cnf->bnf.cnf.akt_vrstva;
    }
    else {
      vrstva = p_bnf->p_lev[i] ? i : j;
    }

    if (!p_bnf->p_lev[vrstva])
      return;

    guid = p_bnf->p_lev[vrstva]->p_prv_databaze->guid;
    if (ko_GetLayer(guid) != ko_GetLayer(p_bnf->p_prv_dat[d]->guid)) {
      kprintf(FALSE, "Nahrazovat se daji pouze prvky se stejnou vrstvou.");
      return;
    }

    for (i = 0; i < MAX_PRVKU_LEVEL; i++) {
      if (p_bnf->p_prv_lev[i]
        && p_bnf->p_prv_lev[i]->p_prv_databaze->guid == guid) {
        j = p_bnf->p_prv_lev[i]->lev_index;
        be_pozice_na_disku(j, x, y, z, lay, p_bnf->x_num, p_bnf->z_num);
        be_zrus_prvek_levelu(p_bnf, x, y, z, lay);
        p_bnf->p_lev[j] =
          be_pridej_prvek_levelu_z_databaze(p_bnf, p_bnf->p_prv_dat[d],
          p_bnf->p_prv_dat[d]->guid);
        be_umisti_prvek(p_bnf, p_bnf->p_lev[j], x, y, z);
      }
    }
  }
}
