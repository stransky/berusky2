/*
  Vypocty osvetleni sceny
*/

// def. nastaveni ray
#define MAX_X_SURFACE   512
#define MAX_Y_SURFACE   512
#define HLOUBKA_SURFACE 32

typedef struct _POLY_INDICIE
{

  int poly;
  int face;

} POLY_INDICIE;

void oe_uloz_screen(byte * p_file);
void oe_prikaz_pridej_staticke_svetlo(K_EDITOR * p_cnf);
void ray_prepocitej_staticke_osvetleni(K_EDITOR * p_cnf, int ticho);
void ray_zrus_staticke_osvetleni_vertexy(K_EDITOR * p_cnf);
void oe_uprav_staticke_svetlo(K_EDITOR * p_cnf, STATIC_LIGHT * p_light);
int oe_smaz_staticke_bodove_svetlo(K_EDITOR * p_cnf, STATIC_LIGHT * p_light);
void ray_uloz_lightmapy(K_EDITOR * p_cnf, byte * p_file, byte * p_dir);

void rozdrob_objekt(EDIT_KONTEJNER * p_kont, int o);
void ra_test(void);

void ray_transformuj_scenu_raytracing(K_EDITOR * p_cnf);
void ray_transformuj_scenu_raytracing_zpet(K_EDITOR * p_cnf);

void ray_hierarchie_sceny_zrus(K_EDITOR * p_cnf);

void sloz_vertex_zpet(TEXT_KOORD * p_koord, OBJ_VERTEX * p_bod);

void ray_slep_lightmapy(K_EDITOR * p_cnf, POLY_INDICIE * p_ind, int indnum,
  EDIT_TEXT * p_src);
void ray_preved_barvy_do_materialu(K_EDITOR * p_cnf);

int ray_bod_do_qbod(QUAD_TREE * p_qtree, QBOD * p_q, BOD * p_b);

void ray_smaz_blby_plosky(K_EDITOR * p_cnf);

//void ray_preved_plosna_svetla(K_EDITOR *p_cnf);
void ray_dopln_okraje(K_EDITOR * p_cnf, EDIT_TEXT * p_src, int textnum);
void ray_dopln_okraj_surf(bitmapa * p_bmp);

void sloz_vertex(OBJ_VERTEX * p_bod, TEXT_KOORD * p_koord);
void calc_face_matrix(TRIANGLE * p_tri, GLMATRIX * p_mat);
void trface(TRIANGLE * p_tri, TRIANGLE * p_tr, GLMATRIX * p_mat,
  float faktor);
void ray_dopln_okraj_spodni(bitmapa * p_bmp, int okraj);
void ray_uloz_okraj_textury_2(dword * p_okraj, int delka, int face, int okraj,
  int cislo);

void ray_prepocitej_staticke_osvetleni_vertexy(K_EDITOR * p_cnf);
void oe_prepocitat_stat_svetla(K_EDITOR * p_cnf);

void ray_surface_filtruj_ctverec_9(bitmapa * p_bmp);
void ray_surface_filtruj_ctverec_25(bitmapa * p_bmp);
void ray_surface_filtruj_krizem_9(bitmapa * p_bmp);
void ray_surface_filtruj_krizem_25(bitmapa * p_bmp);
void ray_surface_filtruj_medianem(bitmapa * p_bmp);
void ray_surface_filtruj_rovne_25(bitmapa * p_bmp);
