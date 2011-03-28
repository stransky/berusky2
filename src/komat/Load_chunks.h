/* Chunky
*/

#ifndef __LOAD_CHUNKS_H_
#define __LOAD_CHUNKS_H_


int  lo_uloz_kontejner_chunk(FFILE f, EDIT_MATERIAL **p_mat, int max_mat, EDIT_KONTEJNER *p_kont, int low);
EDIT_KONTEJNER * lo_nahraj_kontejner_chunk(EDIT_MATERIAL **p_mat, int max_mat, EDIT_TEXT *p_text, int max_text, char *p_jmeno, int mat, int normal);

int  lo_uloz_material_chunk(FFILE f, EDIT_MATERIAL *p_mat);
int  lo_nahraj_materialy_chunk(EDIT_MATERIAL **p_mat, int max_mat, EDIT_TEXT *p_text, int max_text, char *p_file, int save);

int  lo_uloz_projekt(EDIT_MATERIAL **p_mat, int max_mat, EDIT_KONTEJNER **p_kontlist, int kontnum, char *p_jmeno, STATIC_LIGHT *p_light, int lightnum, ZDRCADLO_DESC *p_zrcadlo, LENS_FLARE *p_flarr, int flaremax, dword ambient, MLZNA_KOSTKA *p_mlha, DYN_LIGHT *p_ddlight, int ddlightnum, EXTRA_DYN_LIGHT *p_eelight, int eelightnum, LEVEL_ENVIRONMENT *p_env, NASTAVENI_KAMER *p_kam, LEVEL_KONFIG *p_lc, int level, int transf_static);
int  lo_nahraj_projekt(EDIT_MATERIAL **p_mat, int max_mat, EDIT_KONTEJNER **p_kontls, int kontnum, EDIT_TEXT *p_text, int max_text, char *p_jmeno, STATIC_LIGHT *p_light, int lightnum,ZDRCADLO_DESC *p_zrcadlo, int *p_zrcadlo_aktivni, LENS_FLARE *p_flarr, int flaremax, dword *p_amb, MLZNA_KOSTKA **p_mlha, DYN_LIGHT *p_ddlight, int ddlightnum, EXTRA_DYN_LIGHT *p_eelight, int eelightnum, LEVEL_ENVIRONMENT *p_env, NASTAVENI_KAMER *p_kam, LEVEL_KONFIG *p_lev_konfig, int flag, int normal);

void lo_chunk_stage_load_korekce(EDIT_MATERIAL **p_mat, int num);

#endif
