/*
  Editacni funkce
*/

void oe_scale_kontejner(EDIT_KONTEJNER *p_kont, float x, float y, float z);
void oe_prikaz_scale_kontejneru(K_EDITOR *p_cnf);
void oe_centruj_kontejner(EDIT_KONTEJNER *p_kont);
void oe_prikaz_scale_kontejneru_cisla(K_EDITOR *p_cnf);
//void oe_smaz_divoke_vertexy_kontejneru(K_EDITOR *p_cnf, int cislo);
void oe_smaz_divoke_vertexy_kontejneru(EDIT_KONTEJNER *p_kont);
int  smaz_divoke_vertexy_objektu_dir(EDIT_OBJEKT *p_obj);
void oe_prikaz_vycisti_kontejner(K_EDITOR *p_cnf);
void oe_prikaz_smaz_divoke_vertexy_kontejneru(K_EDITOR *p_cnf);
void oe_prikaz_vymazat_kontejner(K_EDITOR *p_cnf);
void oe_prikaz_posun_kontejner(K_EDITOR *p_cnf);
void oe_prikaz_kopiruj_kontejner(K_EDITOR *p_cnf);
void oe_prikaz_kopiruj_kontejner_vyber(K_EDITOR *p_cnf);
void oe_prikaz_mazani_kontejneru(K_EDITOR *p_cnf);
void oe_prikaz_slep_kontejner(K_EDITOR *p_cnf);
void oe_prikaz_rozdrob_kontejner(K_EDITOR *p_cnf);
void oe_prikaz_centruj_kontejner(K_EDITOR *p_cnf);
void oe_prikaz_init_matrix_kontejner(K_EDITOR *p_cnf);
void oe_prikaz_rotace_kontejneru(K_EDITOR *p_cnf);
void oe_prikaz_rotace_kontejneru_osove(K_EDITOR *p_cnf, int x, int y, int z);
void oe_prikaz_rotace_kontejneru_cislama(K_EDITOR *p_cnf, float x, float y, float z);
void oe_prikaz_transformace_kontejneru(K_EDITOR *p_cnf);
void oe_scale_objekt(EDIT_OBJEKT *p_obj, float x, float y, float z);
void oe_prikaz_scale_objektu(K_EDITOR *p_cnf);
void oe_prikaz_slepeni_objektu(K_EDITOR *p_cnf);
void oe_prikaz_rotace_objektu(K_EDITOR *p_cnf);
void oe_prikaz_rotace_objektu_osove(K_EDITOR *p_cnf);
void oe_prikaz_rotace_objektu_cislama(K_EDITOR *p_cnf, float x, float y, float z);
void oe_prikaz_mazani_objektu(K_EDITOR *p_cnf);
void oe_prikaz_posun_objekt(K_EDITOR *p_cnf);
void oe_prikaz_kopiruj_objekt(K_EDITOR *p_cnf);
EDIT_KONTEJNER * oe_nahraj_game_objekt(K_EDITOR *p_cnf, byte *p_file);
void transf_3d_2d_matrix_float_moje(float p_x, float p_y, float p_z, float *p_tx, float *p_ty, GLMATRIX *p_mat, dword x_c, dword y_c);;
void oe_prikaz_light_map(K_EDITOR *p_cnf);
void text_test(K_EDITOR *p_cnf, EDIT_OBJEKT *p_obj);
void oe_prikaz_mazani_sceny(K_EDITOR *p_cnf);
void oe_prikaz_smaz_face(K_EDITOR *p_cnf, int korekce);
void oe_smaz_vertex(K_EDITOR *p_cnf, int k, int o, int v);
void oe_prikaz_smaz_vertex(K_EDITOR *p_cnf);
void oe_prikaz_smaz_group_vertex(K_EDITOR *p_cnf);
int oe_smaz_divoke_vertexy_objektu(K_EDITOR *p_cnf, int k, int o);
void oe_prikaz_smaz_divoke_vertexy_objektu(K_EDITOR *p_cnf);
void oe_prikaz_prusecik_cnf(K_EDITOR *p_cnf);
void oe_prikaz_prusecik(K_EDITOR *p_cnf);
void oe_prepis_kontejner_mrizkou(EDIT_KONTEJNER *p_kont, int rus);
void oe_prikaz_sjednot_group_vertex(K_EDITOR *p_cnf);
void oe_prikaz_sesij_group_vertex(K_EDITOR *p_cnf);
void oe_uloz_vertexy_ze_ctverce(K_EDITOR *p_cnf);
void oe_vrat_vertexy_ze_ctverce(K_EDITOR *p_cnf, I_VERTEX **p_group, int *p_groupnum);
void oe_prikaz_group_vertex(K_EDITOR *p_cnf);
int oe_je_vertex_v_seznamu(K_EDITOR *p_cnf, int k, int o, int v);
void oe_pridej_vertexy_ze_ctverce(K_EDITOR *p_cnf);
void oe_prikaz_group_vertex_pridej(K_EDITOR *p_cnf);
void oe_prikaz_vyber_vertex(K_EDITOR *p_cnf);
void oe_prikaz_pridej_vertex(K_EDITOR *p_cnf);
void oe_prikaz_vyber_vertex_face(K_EDITOR *p_cnf);
void oe_prikaz_pridej_vertex_face(K_EDITOR *p_cnf);
void oe_prikaz_all_ungroup_vertex(K_EDITOR *p_cnf);
void oe_ungroup_vertex(K_EDITOR *p_cnf, int cislo);
void oe_prikaz_ungroup_aktualni_vertex(K_EDITOR *p_cnf);
void oe_prikaz_ungroup_group_vertex(K_EDITOR *p_cnf);
void oe_prikaz_ungroup_vertex(K_EDITOR *p_cnf);
void oe_pis_vybrany_vertex(K_EDITOR *p_cnf);
void oe_prikaz_vyber_vertexu_nahoru(K_EDITOR *p_cnf);
void oe_prikaz_vyber_vertexu_dolu(K_EDITOR *p_cnf);
void oe_edit_vertex_xyz(K_EDITOR *p_cnf, int i, byte *p_flag, float *p_f);
void oe_nasob_vertex_xyz(K_EDITOR *p_cnf, int i, float x, float y, float z);
void oe_edit_vertex_uv(K_EDITOR *p_cnf, int i, int t, byte *p_flag, float u, float t2);
void oe_nasob_vertex_uv(K_EDITOR *p_cnf, int i, int t, byte *p_flag, float u, float t2);
void oe_prikaz_editace_group(K_EDITOR *p_cnf, float *p_f, byte *p_flag);
void oe_prikaz_editace_vertex(K_EDITOR *p_cnf, float *p_f, byte *p_flag);
void oe_prikaz_barva_vertex(K_EDITOR *p_cnf);
void oe_prikaz_nasobeni_group(K_EDITOR *p_cnf, float *p_f, byte *p_flag);
void oe_prikaz_nasobeni_vertex(K_EDITOR *p_cnf, float *p_f, byte *p_flag);
void oe_prikaz_posun_group_vertex(K_EDITOR *p_cnf);
void oe_prikaz_posun_vertex(K_EDITOR *p_cnf);
void oe_prikaz_zvedni_vertex(K_EDITOR *p_cnf);
void oe_prikaz_zvedni_group_vertex(K_EDITOR *p_cnf);
void oe_eliminuj_stejne_group(K_EDITOR *p_cnf);
void oe_vyber_nejblizsi_group_vertex(K_EDITOR *p_cnf);
void oe_prikaz_rozdrob_objekt(K_EDITOR *p_cnf, int vybr_face);
void oe_prikaz_posun_flare(K_EDITOR *p_cnf);
void oe_vyrob_okoli(K_EDITOR *p_cnf);
void oe_uprav_okoli(K_EDITOR *p_cnf);
void oe_tiskni_akt_objekt(K_EDITOR *p_cnf);
void oe_prikaz_pridej_akt_objekt(K_EDITOR *p_cnf, int presun);
void oe_sloz_objekty_do_kontejneru(K_EDITOR *p_cnf, EDIT_KONTEJNER *p_kont, int presun);
void oe_serad_tri(OBJ_VERTEX *p_list, int num);
int  oe_smaz_stejne_vertexy(OBJ_VERTEX *p_list, int num);
int  oe_najdi_vertex(OBJ_VERTEX *p_list, int num, OBJ_VERTEX *p_hled);
void oe_prikaz_posun_staticke_svetlo(K_EDITOR *p_cnf);
void oe_prikaz_scale_staticke_svetlo(K_EDITOR *p_cnf);
int  oe_najdi_volne_stat_svetlo(K_EDITOR *p_cnf);
void oe_smaz_flare(K_EDITOR *p_cnf, LENS_FLARE *p_flare);
void oe_prikaz_desintegrator(K_EDITOR *p_cnf);
void oe_prikaz_osamostatnit_objekt(K_EDITOR *p_cnf);
void oe_prikaz_vertex_objekt(K_EDITOR *p_cnf);
void oe_prikaz_autoscale_kontejneru(K_EDITOR *p_cnf);
void oe_prikaz_slij_group_vertex(K_EDITOR *p_cnf);
void oe_pridej_groupvertex(K_EDITOR *p_cnf, int k, int o, int v);
void oe_objekt_izolovane_vertexy(K_EDITOR *p_cnf, int k, int o);
void oe_face_izolovane_vertexy(K_EDITOR *p_cnf);
void oe_prikaz_slivacka_group_vertex(K_EDITOR *p_cnf);
void oe_prikaz_decimace_pruhu_body(K_EDITOR *p_cnf);
void oe_prikaz_vyrovnej_group_vertex(K_EDITOR *p_cnf, int level);

void oe_pridej_group_face_list(K_EDITOR *p_cnf, F_VERTEX *p_new, int num, int flag);
void oe_ungroup_face_vybrany(K_EDITOR *p_cnf);
void oe_ungroup_face_all(K_EDITOR *p_cnf);
void oe_ungroup_face_ctverec(K_EDITOR *p_cnf);
void oe_group_face_pridej(K_EDITOR *p_cnf); // group face pomoci ctverce
void oe_group_face_vyber(K_EDITOR *p_cnf);
F_VERTEX * oe_vrat_face_ze_ctverce(K_EDITOR *p_cnf, int *p_num);

void oe_prikaz_posun_stredu_levelu(K_EDITOR *p_cnf);
void oe_prikaz_scena_oznac_placate_plosky(K_EDITOR *p_cnf);

EDIT_KONTEJNER * oe_slep_kontejnery_vsechny(K_EDITOR *p_cnf);

void oe_prikaz_decimace_sceny(K_EDITOR *p_cnf);
void oe_prikaz_decimace_kontejneru(K_EDITOR *p_cnf);
void oe_prikaz_decimace_vyberu(K_EDITOR *p_cnf);
void oe_prikaz_decimace_trsu(K_EDITOR *p_cnf);
void oe_prikaz_decimace_pruhu(K_EDITOR *p_cnf);

void oe_prikaz_posun_mlhokostky(K_EDITOR *p_cnf);
void oe_prikaz_scale_mlhokostky(K_EDITOR *p_cnf);

void oe_prikaz_slep_kontejnery(K_EDITOR *p_cnf);
void oe_vymaz_flare_svetel_ze_sceny(K_EDITOR *p_cnf, EDIT_KONTEJNER *p_kont);

void oe_prikaz_add_group(K_EDITOR *p_cnf, float *p_f, byte *p_flag);
void oe_prikaz_add_vertex(K_EDITOR *p_cnf, float *p_f, byte *p_flag);
void setrid_body_ctverec(OBJ_VERTEX *p_body);
void setrid_body_ctverec_bod(BOD *p_body);
void oe_prikaz_teselace_objektu(K_EDITOR *p_cnf);

void oe_prikaz_rozmnoz_texture_koord(K_EDITOR *p_cnf, int src, int dest);
void oe_preindexuj_kontejner(EDIT_KONTEJNER *p_kont);
void oe_prikaz_preindexuj_kontejner(K_EDITOR *p_cnf);
void oe_prikaz_posun_objekt_pivot_point(K_EDITOR *p_cnf);
void oe_prikaz_centr_pivot_point(K_EDITOR *p_cnf);

int  oe_vybr_obj_reset(void);
EDIT_OBJEKT * oe_vybr_obj_get(int *p_handle);
EDIT_OBJEKT * oe_vybr_obj_get_ind(int *p_handle, int *p_k, int *p_o);
EDIT_KONTEJNER * oe_vybr_obj_get_kont(int *p_handle);

void oe_prikaz_kost_pridej_start(K_EDITOR *p_cnf);
void oe_prikaz_kost_pridej_navaz(K_EDITOR *p_cnf);
void oe_prikaz_kost_pridej_vertexy(K_EDITOR *p_cnf);
void oe_prikaz_kost_uber_vertexy(K_EDITOR *p_cnf);
void oe_prikaz_kost_smaz(K_EDITOR *p_cnf);
void oe_prikaz_kost_smaz_animaci(K_EDITOR *p_cnf);
void oe_prikaz_kost_posun(K_EDITOR *p_cnf, int pivot);
void oe_prikaz_generuj_flare(K_EDITOR *p_cnf);

void oe_prikaz_kost_kopiruj(K_EDITOR *p_cnf);

void oe_prikaz_zjednodus_objekty(K_EDITOR *p_cnf, int flag);

void oe_stack_zaloha_zrus(HRAN_STACK *p_stack);
void oe_stack_zaloha_vyrob(HRAN_STACK *p_stack, int num);
void oe_stack_zaloha_pridej(HRAN_STACK *p_stack, EDIT_OBJEKT *p_obj, int o, int k);
void oe_stack_zaloha_obnov(K_EDITOR *p_cnf);

__inline int najdi_group_vertex(I_VERTEX *p_list, int num, int k, int o, int v)
{
  int i;  
  for(i = 0; i < num; i++) {
    if(p_list[i].k == k && p_list[i].o == o && p_list[i].v == v)
      return(i);
  }
  return(K_CHYBA);
}

__inline int najdi_group_face_vertex(F_VERTEX *p_list, int num, F_VERTEX *p_hled, int flag)
{
  int i;

  if(flag&GROUP_KONT && !(flag&(GROUP_OBJ|GROUP_FACE))) {
    for(i = 0; i < num; i++) {
      if(p_list[i].k == p_hled->k)
        return(i);
    }
    return(K_CHYBA);
  
  } else if(flag&GROUP_KONT && flag&GROUP_OBJ && !(flag&GROUP_FACE)) {
    for(i = 0; i < num; i++) {
      if(p_list[i].k == p_hled->k && p_list[i].o == p_hled->o)
        return(i);
    }
    return(K_CHYBA);
  
  } else if(flag&GROUP_KONT && flag&GROUP_OBJ && flag&GROUP_FACE) {  
    for(i = 0; i < num; i++) {
      if(p_list[i].k == p_hled->k && p_list[i].o == p_hled->o && p_list[i].f == p_hled->f)
        return(i);
    }
    return(K_CHYBA);
  }
  return(K_CHYBA);
}

__inline void smaz_group_face_vertex(F_VERTEX *p_list, int *p_num, int maz)
{
  if(maz < (*p_num)-1)
    memcpy(p_list+maz,p_list+maz+1,sizeof(p_list[0])*((*p_num)-maz-1));
  if(maz < *p_num)
    (*p_num)--;
}

__inline void smaz_group_face_stejne_vertexy(F_VERTEX *p_list, int num, F_VERTEX *p_new, int *p_newnum, int flag)
{
  int i;

  for(i = 0; i < *p_newnum; i++) {
    if(najdi_group_face_vertex(p_list, num, p_new+i, flag) != K_CHYBA) {
      smaz_group_face_vertex(p_new, p_newnum, i);
      i--;
    }
  }
}


#define POSUN_TARGET 0x001
#define POSUN_POZICI 0x002
#define POSUN_ROLL   0x004
#define POSUN_FOV    0x008
#define POSUN_OSA_X  0x010
#define POSUN_OSA_Y  0x020
#define POSUN_OSA_Z  0x040
#define POSUN_ROT    0x080
#define POSUN_SCALE  0x100

#define POSUN_VSE    (POSUN_OSA_X|POSUN_OSA_Y|POSUN_OSA_Z)

void oe_prikaz_posun_kameru(K_EDITOR *p_cnf, int bod);
void oe_prikaz_roll_kameru(K_EDITOR *p_cnf);
void oe_prikaz_fov_kameru(K_EDITOR *p_cnf);

void oe_prikaz_kamera_nova(K_EDITOR *p_cnf);
void oe_prikaz_kamera_upravit(K_EDITOR *p_cnf);
void oe_prikaz_kamera_smazat(K_EDITOR *p_cnf);

void oe_prikaz_kamera_animace_nova(K_EDITOR *p_cnf);
void oe_prikaz_kamera_animace_upravit(K_EDITOR *p_cnf);
void oe_prikaz_kamera_animace_smazat(K_EDITOR *p_cnf);

void oe_prikaz_kamera_nahrat(K_EDITOR *p_cnf);
void oe_prikaz_kamera_ulozit(K_EDITOR *p_cnf);
void oe_prikaz_kamera_ulozit_vse(K_EDITOR *p_cnf);

void oe_prikaz_kamera_animace_nahrat(K_EDITOR *p_cnf);
void oe_prikaz_kamera_animace_ulozit(K_EDITOR *p_cnf);
void oe_prikaz_kamera_animace_ulozit_vse(K_EDITOR *p_cnf);

void oe_prikaz_face_invert_kontejner(K_EDITOR *p_cnf);
void oe_prikaz_face_invert_objekt(K_EDITOR *p_cnf);
void oe_prikaz_face_invert_vyber(K_EDITOR *p_cnf);

void oe_prikaz_posun_bb_kamery(K_EDITOR *p_cnf);
void oe_prikaz_scale_bb_kamery(K_EDITOR *p_cnf);

void oe_prikaz_kopiruj_objekt_geometrie(K_EDITOR *p_cnf);

int  oe_prikaz_kontroluj_ungroup_vertex(K_EDITOR *p_cnf, int k);

void oe_prikaz_exportuj_scenu(K_EDITOR *p_cnf);

int              oe_vybr_kont_reset(void);
EDIT_KONTEJNER * oe_vybr_kont_get(int *p_handle);
int              oe_vybr_obj_reset(void);
EDIT_OBJEKT *    oe_vybr_obj_get(int *p_handle);
EDIT_OBJEKT *    oe_vybr_obj_get_ind(int *p_handle, int *p_k, int *p_o);
EDIT_KONTEJNER * oe_vybr_obj_get_kont(int *p_handle);
int              oe_vybr_obj_get_num(void);

void joint_vloz_do_klice(JOINT *p_joint, int flag);
void oe_prikaz_kost_rotace(K_EDITOR *p_cnf);
void oe_prikaz_kost_scale(K_EDITOR *p_cnf);
void oe_kost_smaz_obj(K_EDITOR *p_cnf, EDIT_OBJEKT *p_obj);
void oe_prikaz_smaz_jcf(K_EDITOR *p_cnf);

void oe_prikaz_sjednoceni_vertexu_obj(K_EDITOR *p_cnf);

int   oe_hran_vloz_hrany_stack(HRAN_STACK *p_stack, EDIT_OBJEKT *p_obj);
void  oe_hran_zrus_hranu_stack(HRAN_STACK *p_stack, HRAN *p_hrana, int v1, int v2);
int   oe_hran_zrus_hranu(HRAN_STACK *p_stack, HRAN *p_hrana, EDIT_OBJEKT *p_obj, int *p_vmask);
void  oe_hran_reset(HRAN_STACK *p_stack);
int   oe_hran_vloz(HRAN_STACK *p_stack, int ind1, int ind2, BOD *p_norm);
int   oe_hran_setrid_sort(const void *p_h1, const void *p_h2);
void  oe_hran_vyhodnot(HRAN_STACK *p_stack, EDIT_OBJEKT *p_obj, int *p_vmask);
void  oe_hran_decimuj(HRAN_STACK *p_stack, GLMATRIX *p_mat, EDIT_OBJEKT *p_obj, int vertexu);
void  oe_hran_pridej_hranici(HRAN_STACK *p_stack, HRAN *p_hran, EDIT_OBJEKT *p_obj);

float calc_equad_vert(GLMATRIX *p_mat, BOD *p_v);
void  calc_equad_obj_mat(HRAN_STACK *p_stack, EDIT_OBJEKT *p_obj, GLMATRIX *p_mat);
void  calc_equad_hrana(OBJ_VERTEX *p_vert, GLMATRIX *p_mat, HRAN *p_hran, int *p_vmask);

EDIT_OBJEKT * oe_obj_zjednodus(EDIT_OBJEKT *p_obj, int vertexu);

#define VERT_NECHEJ 0x1
#define VERT_SMAZ   0x2
#define VERT_MASK   (VERT_NECHEJ|VERT_SMAZ)
int  oe_stack_vertexy_pridej(K_EDITOR *p_cnf, EDIT_OBJEKT *p_obj, HRAN_STACK *p_stack, int k, int o, int flag);

void oe_generator_animaci(K_EDITOR *p_cnf);
void oe_prikaz_flagovac(K_EDITOR *p_cnf);

void oe_prikaz_down_centr_pivot(K_EDITOR *p_cnf);
void oe_prikaz_vycisti_scenu(K_EDITOR *p_cnf);

void oe_prikaz_prevadec_bmp(K_EDITOR *p_cnf);
void oe_prikaz_zvedni_kontejner(K_EDITOR *p_cnf);

void oe_prikaz_zrcadelnik(K_EDITOR *p_cnf);

#define stejny_bod(a,b)    (a->x == b->x && a->y == b->y && a->z == b->z)
#define podobny_bod(a,b)   (fabsf(a->x-b->x) < 0.001f &&\
                            fabsf(a->y-b->y) < 0.001f &&\
                            fabsf(a->z-b->z) < 0.001f)
#define podobny_vec(a,b)   (fabsf(a->x-b->x) < 0.001f &&\
                            fabsf(a->y-b->y) < 0.001f &&\
                            fabsf(a->z-b->z) < 0.001f)
