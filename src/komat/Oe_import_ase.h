/*
  Header k animacim
*/
EDIT_KONTEJNER * ase_import(byte *p_file, byte *p_dir);
void oe_kont_dopln_prazdne_child(EDIT_KONTEJNER *p_kont);
void ase_transformuj_anim_objekt(EDIT_OBJEKT *p_obj);
void ase_dopln_animace(EDIT_KONTEJNER *p_kont);
void ase_transformuj_objekty(EDIT_KONTEJNER *p_kont);
void ase_dopln_sim_track_pos(SIMPLE_TRACK_INFO *p_track, BOD *p_pos);
void ase_dopln_sim_track_rot(SIMPLE_TRACK_INFO *p_track, QUAT *p_quat);
void ase_dopln_sim_track_scs(SIMPLE_TRACK_INFO *p_track, BOD *p_scs);
