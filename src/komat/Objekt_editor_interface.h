/*
  H k okenimu rozhrani objekt editoru
*/

typedef struct _POM_PROMENE {

  float f[10];
  int   i[10];
  char  pom[50];
  char  pom1[50];
  char  pom2[50];
  int   prikaz;
  void  *p_pom;
  void  *p_pom1;
  void  *p_point[10];
  int   t[10];

} POM_PROMENE;

typedef struct _MAPING_PROM {

  int   i[10];
  int   druh; // flat/sfericky
  int   druh_sfer; // UV,U,V
  float f[3]; // scale parametry mappingu

} MAPING_PROM;

#define MAX_LIST 20

typedef struct _TABLE_LIST {

  byte aktivni;

  byte polozek;
  byte jmena[MAX_LIST][50];
  word dlg[MAX_LIST];
  word t_dlg;

  DLGTEMPLATE *p_template[MAX_LIST];
  HGLOBAL hglobal[MAX_LIST];
  BOOL (CALLBACK * call[MAX_LIST])( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );

  HWND hwndDlg; // hwnd akt dialogu
  HWND hwndTab; // hwnd zalozek

  RECT rc;

  void *p_sv;

} TABLE_LIST;


void doe_nastav_k_editor(K_EDITOR *p_config);
int  doe_dialog_load(HWND hwnd, byte *p_filename, byte max_delka, byte *p_nadpis_okna, byte *p_maska, byte *p_popis_masky);
int  doe_dialog_save(byte *p_filename, byte max_delka, byte *p_nadpis, byte *p_maska, byte *p_popis_masky);
void doe_edit_jmeno_textury(HWND hwnd);
void doe_edit_jmeno_kontejneru(HWND hwnd);
void doe_edit_jmeno_objektu(HWND hwnd);
int  doe_vyber_texturu_dialog(HWND hwnd, K_EDITOR *p_cnf, byte *p_text);
HRESULT doe_init_dialog_objekty(HWND hdlg);
HRESULT doe_init_dialog_kontejnery(HWND hdlg);
HRESULT doe_init_dialog_text(HWND hdlg);
HRESULT doe_init_dialog_textury(HWND hdlg);
HRESULT doe_init_dialog_materialy(HWND hdlg, int akt_pol);
HRESULT doe_init_dialog_world_matrix(HWND hDlg);
void doe_dialog_objekty(void);
void doe_prekresli_hlavni_okno(void);
void doe_prikaz_scale_kontejneru_cisla_dialog(void);
void doe_prikaz_scale_objektu_cisla(void);
void doe_prikaz_rotace_kontejneru_cislama(K_EDITOR *p_cnf);
void doe_prikaz_rotace_objektu_cislama(K_EDITOR *p_cnf);
HRESULT doe_init_hlavni_menu(HWND hDlg);
void doe_hlavni_menu(void);
int  doe_teselace_objektu(int *p_x, int *p_y);
void doe_dialog_edit_vertex(void);
void doe_dialog_nasob_vertex(int druh);
void doe_vyroba_zeme_textura(int cislo);
void doe_vyroba_zeme(void);
void doe_uprava_zeme(void);
void doe_novy_kontejner(void);
void vyber_param_vyrezu(K_EDITOR *p_cnf, HWND hDlg);
int  doe_mlhokostka_jmeno(HWND hwnd, MLZNA_KOSTKA *p_mlha);
int  doe_prikaz_vyber_barvu(K_EDITOR *p_cnf, HWND hwnd, dword *p_rgb);
void doe_prikaz_zrcadleni_objektu(void);
void doe_prikaz_zrcadleni_kontejneru(void);
void doe_prikaz_rotace_osove_objektu(void);
void doe_prikaz_rotace_osove_kontejneru(void);
int  doe_dialog_singlepass(HWND hwnd, MULTITEXT_FLAG *p_tblok);
int  doe_dialog_multipass(HWND hwnd, GLenum *p_src, GLenum *p_dest);
int  doe_prikaz_zrcadleni_textury(HWND hwnd);
int  doe_prikaz_rotace_textury(HWND hwnd);
int  doe_prikaz_vyrob_krychli(K_EDITOR *p_cnf);
void nahraj_list_textur_to_material(K_EDITOR *p_cnf, byte *p_file);
void doe_nahraj_list_textur(K_EDITOR *p_cnf);
HRESULT doe_init_dialog_databaze_objekty(HWND hdlg, byte *p_dir, byte *p_maska);
void doe_prikaz_nahraj_svetelne_textury(K_EDITOR *p_cnf);
void doe_prikaz_abount(void);
void doe_proved_vybr_prikaz(void);
dword doe_get_vybr_prikaz(void);
void doe_vyber_prikaz(void);
int  doe_proved_prikaz_menu(UINT prikaz, int rychle, int ulozit);
void doe_updatuj_konzoli_mod(K_EDITOR *p_cnf);
void doe_updatuj_konzoli_posun(K_EDITOR *p_cnf);
void doe_updatuj_konzoli_kamera(K_EDITOR *p_cnf);
void doe_updatuj_konzoli(K_EDITOR *p_cnf);
void doe_updatuj_konzoli_mys(K_EDITOR *p_cnf);
void aplikuj_zmeny_konfigurace(K_EDITOR *p_cnf);
DLGTEMPLATE * load_dialog_resource(byte *p_res); 
void doe_prikaz_konfigurace_table(K_EDITOR *p_cnf, HWND hwnd, int strana);
void doe_prikaz_texturace_2d(K_EDITOR *p_cnf, HWND hwnd, int fofr);
void doe_prikaz_posun_material_lista(K_EDITOR *p_cnf, int smer);
HWND doe_aktivace_bocni_listy_mesh_editor(HWND bocni_lista);
void doe_bocni_lista_prvky_dopln_tridy(HWND hDlg);
void doe_bocni_lista_prvky_dopln_varianty(HWND hDlg);
HWND doe_aktivace_bocni_listy_game_editor(HWND bocni_lista);
void doe_prepni_do_mesh_editor(void);
void doe_prepni_do_game_editor(void);
void doe_editor_materialu(K_EDITOR *p_cnf);
void doe_prikaz_editor_prvku(K_EDITOR *p_cnf);
int  doe_prikaz_bodove_svetlo(HWND hwnd, byte *p_nadpis, STATIC_LIGHT *p_light);
void doe_progresbar_nastav(HWND hwnd, int res, int min, int max);
void doe_progresbar_posun(HWND hwnd, int res, int krok);
void doe_progresbar_setpos(HWND hwnd, int res, int pos);
void doe_vertex_info(HWND hwnd, MUJ_BOD *p_bod, int k, int o, int v);
int  doe_vlastnosti_prvku(HWND hwnd, PRVEK_LEVELU *p_bunka);
int  doe_dialog_scale(byte *p_nadpis, byte *p_dalsi, float *p_s);
int  doe_podlozka_levelu(K_EDITOR *p_cnf, float *p_vyska);
int  doe_obruba_levelu(K_EDITOR *p_cnf, float *p_vyska_obruby, float *p_vyska_zeme, float *p_sirka, int *p_text_x);

void doe_prikaz_load_flags(K_EDITOR *p_cnf);

void doe_toogle_kreslit_level(K_EDITOR *p_cnf);
void doe_toogle_kreslit_scenu(K_EDITOR *p_cnf);
void doe_zaskrkni_menu(K_EDITOR *p_cnf);

void doe_editor_materialu_flagovac(K_EDITOR *p_cnf);

int  doe_prikaz_flare(HWND hwnd, byte *p_nadpis, LENS_FLARE *p_flare);
void doe_statistika_sceny(K_EDITOR *p_cnf);

int  doe_edit_barva_bodu(dword *p_barva, byte *p_op);

void oe_prikaz_nahraj_frame(K_EDITOR *p_cnf, HWND hwnd, EDIT_MATERIAL *p_mat, byte *p_textdir);
void oe_prikaz_nahraj_posun(HWND hwnd, EDIT_MATERIAL *p_mat, byte *p_textdir, EDIT_TEXT *p_text, int max);
void oe_prikaz_nahraj_animaci(K_EDITOR *p_cnf, HWND hwnd, EDIT_MATERIAL *p_mat, byte *p_textdir);
void oe_prikaz_uloz_animaci(HWND hwnd, EDIT_MATERIAL *p_mat, byte *p_textdir);
int  doe_drobeni_facu(dword *p_hloubka, dword *p_nahod);
int  doe_uprava_okoli(BOD *p_dist, BOD *p_bod);
void doe_updatuj_lista_objekty(K_EDITOR *p_cnf);
void doe_printf(HWND hwnd, int cntrl, byte *p_text, ...);

int  doe_prikaz_raytracer(RAY_TRACER *p_ray);
int  doe_cti_jmeno(HWND hwnd, byte *p_jmeno);
int  doe_cti_jmeno_ext(HWND hwnd, byte *p_jmeno, byte *p_nadpis);

void doe_animace_updatuj(K_EDITOR *p_cnf);
void doe_texture_list(K_EDITOR *p_cnf, int druh);

void doe_animace_updatuj_kamery_text(K_EDITOR *p_cnf, KAMERA *p_kam);

int  doe_kamera_animace_uprav(HWND hwnd, KAMERA_TRACK_INFO *p_track);

void doe_3ds_importer(byte *p_key, byte *p_druh, byte *p_rozloz, byte *p_kamera);

void doe_editor_materialu_flagy(EDIT_MATERIAL *p_mat);

void kprintf(byte log, byte *p_text,...);
void konzole_smaz(K_EDITOR *p_cnf);
void doe_konzole_max(K_EDITOR *p_cnf);

void doe_okno_cti(int *p_min_x, int *p_min_y, int *p_max_x, int *p_max_y);

void doe_animace_updatuj_kamery(K_EDITOR *p_cnf);

void  doe_slider_nastav(HWND hwnd, int res, int min, int max, int akt);
void  doe_slider_setpos(HWND hwnd, int res, int pos);
dword doe_slider_getpos(HWND hwnd, int res);

void doe_hs_init(void);
void doe_hs_nastav(int min, int max, int akt, byte *p_text,...);
void doe_hs_setpos(int pos);
int  doe_hs_getpos(void);
void doe_hs_setmark(int pozice);
void doe_hs_delmark(void);
void doe_hs_posfce(int (*p_fce)(int));
void doe_hs_zrus(void);

int  doe_prikaz_vyber_barvu_float(K_EDITOR *p_cnf, HWND hwnd, float *p_rgb);
void doe_prikaz_vypis_group(K_EDITOR *p_cnf);

void doe_animace_updatuj_materialy(K_EDITOR *p_cnf);
int  doe_kopie_kontejneru(HWND hwnd, int *p_flag);

int  doe_prikaz_dynamicke_svetlo(HWND hwnd, byte *p_nadpis, DYN_LIGHT *p_light);
int  doe_najdi_prvek(int guid);

void oe_normalizuj_listu(LISTA_PRVKU *p_lista, int vyber);

void doe_lista_vyrob(int pocet);
void doe_lista_zrus(void);

int  doe_animace_uprav(HWND hwnd, int *p_framenum);

int  doe_prikaz_extra_dynamicke_svetlo(HWND hwnd, byte *p_nadpis, EXTRA_DYN_LIGHT *p_light);

int  oe_uloz_undo(K_EDITOR *p_cnf);
int  oe_proved_undo(K_EDITOR *p_cnf);

void doe_editor_materialu_stage_dot3_color(EDIT_MATERIAL *p_mat, MATERIAL_TEXT *p_text);
void doe_editor_materialu_stage_dot3_env(EDIT_MATERIAL *p_mat, MATERIAL_TEXT *p_text);
void doe_editor_materialu_stage_dudv_env(EDIT_MATERIAL *p_mat, MATERIAL_TEXT *p_text);

int  doe_export_sceny(HWND hwnd, byte *p_prefix, int *p_dos);

void konfigurace_mlha_aktivuj(void);

int  pridej_texturu_mat_generovanou(K_EDITOR *p_cnf, EDIT_MATERIAL *p_mat, int stage);
void doe_joint_animace_vloz_text(K_EDITOR *p_cnf);
void doe_joint_animace_vloz_objekt(K_EDITOR *p_cnf);
void doe_joint_animace_vloz_frame(K_EDITOR *p_cnf);
void doe_joint_animace_vloz_tiky(K_EDITOR *p_cnf);

int  doe_cti_flare_gen(HWND hwnd, FLARE_GEN *p_gen);

#define ID_PRIKAZ_PRIRAD_MATERIAL_OBJEKT       0x1
#define ID_PRIKAZ_PRIRAD_MATERIAL_KONTEJNER    0x2
#define ID_PRIKAZ_SET_BARVA_KONTEJNERU         0x3
#define ID_PRIKAZ_RESET_BARVA_KONTEJNER        0x4
#define ID_PRIKAZ_GOURAND_MAPING_OBJ           0x5
#define ID_PRIKAZ_SET_BARVA_OBJEKT             0x6
#define ID_PRIKAZ_RESET_BARVA_OBJEKT           0x7
#define ID_VERTEX_INFO                         0x8
#define ID_PRIKAZ_GOURAND_MAPING_BODY          0xa
#define ID_PRIKAZ_SET_BARVA_BODY               0xb
#define ID_PRIKAZ_SET_BARVA                    0xc
#define ID_PRIKAZ_RYCHLY_MIRROR_U              0xd
#define ID_PRIKAZ_RYCHLY_MIRROR_V              0xe
#define ID_PRIKAZ_RYCHLA_ROTACE                0xf

int doe_decimator_plosek(K_EDITOR *p_cnf);
void doe_anim_start_et(int next_time, int endtime,
                       int *p_time_start, int *p_time_stop, 
                       int *p_time_delka, int *p_time, 
                       int *p_start, int *p_stop);

int doe_kontejner_flagovac(int *p_flags);