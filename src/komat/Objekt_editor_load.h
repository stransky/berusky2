/*
  Load / save
*/

EDIT_KONTEJNER *oe_nahraj_kontejner_bez_dialog(K_EDITOR * p_cnf,
  byte * p_jmeno, int mat);
EDIT_KONTEJNER *oe_nahraj_kontejner_bez_dialog_3ds(K_EDITOR * p_cnf,
  byte * p_jmeno, int mat);
EDIT_KONTEJNER *oe_nahraj_kontejner_dialog(K_EDITOR * p_cnf, byte * p_jmeno,
  byte max_jmeno, int mat);
EDIT_KONTEJNER *oe_nahraj_kontejner_dialog_3ds(K_EDITOR * p_cnf,
  byte * p_jmeno, byte max_jmeno, int mat);
void oe_prikaz_nahraj_3ds_animaci_dialog(K_EDITOR * p_cnf);
void oe_prikaz_nahraj_sim_animaci_dialog(K_EDITOR * p_cnf);
void oe_prikaz_nahraj_kontejner(K_EDITOR * p_cnf, int soubor, int mat);
void oe_prikaz_uloz_kontejner(K_EDITOR * p_cnf, int obalky);
void oe_prikaz_uloz_kontejner_jako(K_EDITOR * p_cnf);
void oe_prikaz_uloz_projekt(K_EDITOR * p_cnf);
void oe_prikaz_nahraj_projekt(K_EDITOR * p_cnf);
void oe_prikaz_uloz_materialy(K_EDITOR * p_cnf);
void oe_uloz_pouzite_materialy(K_EDITOR * p_cnf, byte * p_file, byte * p_dir);
void oe_prikaz_vypis_pouzite_materialy(K_EDITOR * p_cnf);
void oe_prikaz_nahraj_materialy(K_EDITOR * p_cnf, int maz);

void oe_smaz_scenu(K_EDITOR * p_cnf);
int oe_uloz_projekt(K_EDITOR * p_cnf, byte * p_file, byte * p_dir, int level,
  int transf_stat);

void oe_prikaz_nahraj_objekt(K_EDITOR * p_cnf);
void oe_prikaz_nahraj_b2m_animaci_dialog(K_EDITOR * p_cnf);

void oe_prikaz_uloz_vybrane_kontejnery_jako(K_EDITOR * p_cnf);

void oe_prikaz_uloz_vybrany_material(K_EDITOR * p_cnf);
void oe_prikaz_nahraj_vybrany_material(K_EDITOR * p_cnf);

int oe_nahraj_projekt(K_EDITOR * p_cnf, byte * p_file, byte * p_dir,
  byte volny_load, int load_textury);
void oe_uloz_berusci_level(K_EDITOR * p_cnf, byte * p_level);
void oe_prikaz_hrej_berusci_level(K_EDITOR * p_cnf);
void oe_prikaz_uloz_berusci_level(K_EDITOR * p_cnf);
void oe_oznac_pouzite_materialy(K_EDITOR * p_cnf);
void oe_oznac_pouzite_materialy_kont(K_EDITOR * p_cnf,
  EDIT_KONTEJNER * p_kont);
int oe_oznac_dlouhe_materialy_kont(K_EDITOR * p_cnf, EDIT_KONTEJNER * p_kont);
void oe_oznac_pouzite_textury(K_EDITOR * p_cnf);
void oe_prikaz_uloz_3ds_kontejner_jako(K_EDITOR * p_cnf);
void oe_prikaz_uloz_3ds_scenu_jako(K_EDITOR * p_cnf);

int oe_reload_textur(K_EDITOR * p_cnf);
int oe_smaz_textury(K_EDITOR * p_cnf);
int oe_nahraj_materialy(EDIT_MATERIAL ** p_mat, int max_mat,
  EDIT_TEXT * p_text, int max_text, FILE * f, int save);

void oe_prikaz_vypis_nenahrane_textury(K_EDITOR * p_cnf);
void oe_smaz_scenu_prvky(K_EDITOR * p_cnf);
void oe_smaz_scenu_okoli(K_EDITOR * p_cnf);
void oe_prikaz_vypis_pouzite_materialy_kontejner(K_EDITOR * p_cnf);
void oe_prikaz_flare_export_textur(K_EDITOR * p_cnf);

EDIT_KONTEJNER *oe_najdi_volny_kontejner(K_EDITOR * p_cnf);

void oe_kontejner_to_game(K_EDITOR * p_cnf);

/*
  Hledaci funkce
*/
int oe_vyber_volny_kontejner(K_EDITOR * p_cnf);
int oe_najdi_volny_objekt(K_EDITOR * p_cnf);
int oe_vyber_volny_objekt(K_EDITOR * p_cnf);

/* Load konstanty
*/
#define LOAD_KONT_BTX 0
#define LOAD_KONT_3DS 1
#define LOAD_KONT_ASE 2
