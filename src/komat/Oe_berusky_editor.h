/*
  Editor k beruskam
*/

void be_prikaz_novy_level(B_KONFIG * p_bnf);
void be_renderuj_scenu(K_EDITOR * p_cnf);
void be_vyrob_grid_level(B_KONFIG * p_bnf, int patro);
void be_zrus_grid_level(B_KONFIG * p_bnf);
void be_klavesy_berusky_editor(B_KONFIG * p_bnf);
void be_kresli_kurzor(K_EDITOR * p_cnf);
void be_level_to_window(B_KONFIG * p_bnf);
void be_prikaz_smaz_prvek(K_EDITOR * p_cnf);
int be_prepni_na_patro(B_KONFIG * p_bnf, int patro);
void be_prikaz_uprav_prvek_levelu(K_EDITOR * p_cnf);
void be_prikaz_nahraj_level(K_EDITOR * p_cnf, int rekonstrukce);
void be_prikaz_uloz_level(K_EDITOR * p_cnf);
void be_prikaz_uloz_level_jako(K_EDITOR * p_cnf);
void be_prikaz_rotace_prvku(K_EDITOR * p_cnf);

void be_prikaz_vyrob_obrubu_levelu(K_EDITOR * p_cnf, int material, int fast);
void be_prikaz_vyrob_podlozku_levelu(K_EDITOR * p_cnf, int material);
int be_najdi_obrubu(K_EDITOR * p_cnf);
void be_prikaz_vyrezani_levelu(K_EDITOR * p_cnf);

void be_prikaz_patro_nahoru(K_EDITOR * p_cnf);
void be_prikaz_patro_dolu(K_EDITOR * p_cnf);

void be_prikaz_vloz_prvek(K_EDITOR * p_cnf);

int be_nahraj_level_pozadi(K_EDITOR * p_cnf, byte * p_file, int textury);
void be_uloz_level_pozadi(K_EDITOR * p_cnf, byte * p_file);

void be_prikaz_smaz_level(K_EDITOR * p_cnf, int level, int scena, int ticho);
void be_prepni_kresleni_stat_objektu(K_EDITOR * p_cnf, int kreslit);
void be_prepni_kresleni_dynam_objektu(K_EDITOR * p_cnf, int kreslit);

void be_prikaz_posun_levelu(B_KONFIG * p_bnf);
void be_kresli_berusky_kontejner(K_EDITOR * p_cnf, EDIT_KONTEJNER * p_kont);
void oe_rozmnoz_staticke(K_EDITOR * p_cnf);
int be_spocitej_prvky(K_EDITOR * p_cnf);

void be_zrus_level_nevid_zdi(B_KONFIG * p_bnf);

void be_prikaz_selekce_patra_on(K_EDITOR * p_cnf);
void be_prikaz_selekce_patra_off(K_EDITOR * p_cnf);

void be_preumisti_level(B_KONFIG * p_bnf);
void be_prikaz_nahrad_aktualni_prvek(K_EDITOR * p_cnf);
void oe_zrus_indexy_scena(K_EDITOR * p_cnf);

void be_prikaz_nahrad_prvek(K_EDITOR * p_cnf, int guid);
int be_nahraj_level_render(K_EDITOR * p_cnf, char *p_level);

int be_najdi_obruba_material(K_EDITOR * p_cnf);

int be_nahraj_level(K_EDITOR * p_cnf, byte * p_file, int rekonstrukce,
  int textury, int mazat_materialy);

void be_prikaz_prepis_guid(K_EDITOR * p_cnf, int guid_min, int guid_max,
  int guid_vysl);

__inline be_cti_guid(PRVEK_LEVELU * p_lev)
{
  if (p_lev->guid)
    return (p_lev->guid);
  else
    return (p_lev->p_prv_databaze->guid);
}
