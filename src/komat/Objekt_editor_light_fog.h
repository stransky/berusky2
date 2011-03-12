/*
  Light&fog funkce
*/

int oe_je_zaznam_iobjekt(I_OBJEKT *p_obj, int max, word k, word o);
void oe_zarad_svetla(K_EDITOR *p_cnf, EDIT_KONTEJNER *p_kont, int k);
void oe_zarad_pruhledne_objekty(K_EDITOR *p_cnf, EDIT_KONTEJNER *p_kont, int k);
void setup_vertex_fog(DWORD dwColor, DWORD dwMode);
void oe_prepocitej_svetlo_obj(K_EDITOR *p_cnf, EDIT_KONTEJNER *p_kont_light, EDIT_OBJEKT *p_obj_light, byte cop);
void oe_prepocitej_svetlo(K_EDITOR *p_cnf, EDIT_KONTEJNER *p_kont, EDIT_OBJEKT **p_obj, byte cop);
void oe_prepocitej_svetla(K_EDITOR *p_cnf, byte cop);
void oe_odecti_svetlo(K_EDITOR *p_cnf, EDIT_KONTEJNER *p_kont, EDIT_OBJEKT **p_obj);
void oe_odecti_svetla(K_EDITOR *p_cnf);

void oe_group_smaz_stejne(K_EDITOR *p_cnf);

//void oe_barva_vertex(K_EDITOR *p_cnf, float br, float bg, float bb, byte op);
//void oe_barva_vertex_dword(K_EDITOR *p_cnf, dword rgb, byte op);
void oe_add_barvu_bodu_obj(EDIT_OBJEKT *p_obj, dword rgb);
void oe_sub_barvu_bodu_obj(EDIT_OBJEKT *p_obj, dword rgb);
void oe_prikaz_env_mapping(K_EDITOR *p_cnf);
void oe_prikaz_env_mapping_all(K_EDITOR *p_cnf);


void kont_env_maping(K_EDITOR *p_cnf, EDIT_KONTEJNER *p_kont);
void kont_gourand_maping(K_EDITOR *p_cnf, EDIT_KONTEJNER *p_kont_top, byte cop, int multi_mesh);
void obj_gourand_maping(K_EDITOR *p_cnf, EDIT_KONTEJNER *p_kont, EDIT_OBJEKT *p_obj, byte cop);
void oe_prikaz_kont_norm_vect(K_EDITOR *p_cnf);
void oe_prikaz_kont_norm_vect_all(K_EDITOR *p_cnf);


void oe_prikaz_pridej_flare(K_EDITOR *p_cnf);
void oe_uprav_flare(K_EDITOR *p_cnf, LENS_FLARE *p_flare);

void oe_prikaz_vyrizni_objekt(K_EDITOR *p_cnf, int crop);
void oe_prikaz_vyrizni_kontejner(K_EDITOR *p_cnf, int crop);
void oe_prikaz_vyrizni_plosky(K_EDITOR *p_cnf);

//void oe_prikaz_barva_bodu_vsech(K_EDITOR *p_cnf);
//void oe_prikaz_barva_bodu(K_EDITOR *p_cnf);

void oe_prikaz_vyrizni_face(K_EDITOR *p_cnf, int crop);
void oe_zrcadlo_aktivuj_body(K_EDITOR *p_cnf);
void oe_zrcadlo_aktivuj_plocha(K_EDITOR *p_cnf);
void oe_zrcadlo_zrus(K_EDITOR *p_cnf);
void oe_zrcadlo_zrus_poly(K_EDITOR *p_cnf);
void oe_calc_clip_focus(K_EDITOR *p_cnf);
void oe_zrcadlo_prehod_poradi_bodu(K_EDITOR *p_cnf);


/* Serazene funkce
*/
void oe_prikaz_gourand_maping_kont(K_EDITOR *p_cnf);
void oe_prikaz_gourand_maping_obj(K_EDITOR *p_cnf);
void oe_prikaz_gourand_maping_body(K_EDITOR *p_cnf);

void oe_prikaz_set_barva(K_EDITOR *p_cnf);
void oe_prikaz_set_barva_body_fast(K_EDITOR *p_cnf);
void oe_prikaz_set_barva_body(K_EDITOR *p_cnf);
void oe_prikaz_set_barva_objekt(K_EDITOR *p_cnf);
void oe_prikaz_set_barva_kontejner(K_EDITOR *p_cnf);
void oe_prikaz_set_barva_scena(K_EDITOR *p_cnf);
void oe_prikaz_set_barva_scena_guids(K_EDITOR *p_cnf, int lo, int hi, int stat);

void oe_prikaz_reset_barva_objekt(K_EDITOR *p_cnf);
void oe_prikaz_reset_barva_kontejner(K_EDITOR *p_cnf);

void oe_reset_barva_scena_all(K_EDITOR *p_cnf);
void oe_reset_barva_scena(K_EDITOR *p_cnf, EDIT_MATERIAL *p_mat, int matnum, int barva);
void oe_reset_barva_scena_alfa(K_EDITOR *p_cnf, EDIT_MATERIAL *p_mat, int matnum);


/*
  Svetlo
*/
int  oe_najdi_volne_dsvetlo(K_EDITOR *p_cnf);
void oe_prikaz_dsvetlo_nove(K_EDITOR *p_cnf);
void oe_prikaz_dsvetlo_uprav(K_EDITOR *p_cnf);
void oe_prikaz_dsvetlo_smaz(K_EDITOR *p_cnf);
void oe_prikaz_dsvetlo_posun(K_EDITOR *p_cnf);
int  oe_najdi_prepocitej_dsvetlo(K_EDITOR *p_cnf);

/*
  Extra-svetla
*/
int oe_najdi_prepocitej_extra_svetlo(K_EDITOR *p_cnf);
int oe_najdi_volne_extra_svetlo(K_EDITOR *p_cnf);
void oe_prikaz_extra_svetlo_nove(K_EDITOR *p_cnf);
void oe_prikaz_extra_svetlo_uprav(K_EDITOR *p_cnf);
void oe_prikaz_extra_svetlo_smaz(K_EDITOR *p_cnf);
void oe_prikaz_extra_svetlo_posun(K_EDITOR *p_cnf);
