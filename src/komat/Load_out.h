/* Load/save out souboru
*/

#ifndef __LOAD_OUT_H_
#define __LOAD_OUT_H_

EDIT_KONTEJNER * lo_nahraj_kontejner_out(EDIT_MATERIAL **p_mat, int max_mat, EDIT_TEXT *p_text, int max_text, byte *p_jmeno, int mat);
int lo_nahraj_materialy_out_jmeno(EDIT_MATERIAL **p_mat, int max_mat, EDIT_TEXT *p_text, int max_text, byte *p_file, int save);

#endif