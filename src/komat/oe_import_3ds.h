/*
  import/export 3ds-mesh
*/

#define ROZLOZ_MAX 0xffffffff

EDIT_KONTEJNER *ds_import_kontejner(byte * p_file, int mat, byte key,
  byte text, byte staticky, byte kamery_import);
int ds_kontejner_rozloz(K_EDITOR * p_cnf, EDIT_KONTEJNER * p_kont,
  int m1flag);
void ds_aplikuj_3ds_text_mapping(EDIT_MATERIAL ** p_mat,
  EDIT_KONTEJNER * p_kont);
void ds_vloz_keyframy(EDIT_KONTEJNER * p_kont, KEYFRAMER_DATA * p_key);
void ds_kontejner_preved_na_staticky(EDIT_KONTEJNER * p_kont);

int ds_export_kontejner(byte * p_file, EDIT_KONTEJNER * p_kont);
int ds_export_sceny(byte * p_file);
