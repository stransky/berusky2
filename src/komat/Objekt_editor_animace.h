/*
  Header k animacim
*/

void ani_tiskni_kontejner(K_EDITOR *p_cnf, EDIT_KONTEJNER *p_kont);
void ani_anim_strom_tiskni(K_EDITOR *p_cnf, EDIT_KONTEJNER *p_kont, HIERARCHY_ROOT *p_root);
int  ani_animuj_scenu(K_EDITOR *p_cnf);
void oe_animace_cti_t(K_EDITOR *p_cnf);
void ani_animuj_scenu_vybr_animace(K_EDITOR *p_cnf, EDIT_KONTEJNER *p_kont, dword time);
int  ani_animuj_kamery(K_EDITOR *p_cnf);
int  ani_animuj_materialy(K_EDITOR *p_cnf);
int  ani_animuj_kosti(K_EDITOR *p_cnf);