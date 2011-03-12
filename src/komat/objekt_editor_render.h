/*
  Renderovaci rutiny 
*/

typedef struct _KRESLIT_VECI {

  byte prvku;
  byte dekorace;
  byte okoli;
  byte staticke;
  byte dynamicke;

} KRESLIT_VECI;


void oe_obj_env_spec(EDIT_MATERIAL *p_mat, EDIT_OBJEKT *p_obj, GLMATRIX *p_kw);
void oe_kresli_mriz(float sx, float sz, float dx, int num_x, float r, float g, float b);
void oe_kresli_stred(float d, float r, float g, float b);
void oe_kresli_grid(K_EDITOR *p_cnf);
void oe_kresli_linelist(K_EDITOR *p_cnf);
void oe_kresli_group_vertexy(K_EDITOR *p_cnf);
void oe_kresli_d_vertexy(K_EDITOR *p_cnf);
void oe_kresli_objekt(K_EDITOR *p_cnf, EDIT_KONTEJNER *p_kont, EDIT_OBJEKT *p_obj, EDIT_MATERIAL **p_mat, int akt_objekt, int akt_kont);
void oe_kresli_objekt_jednoduse(EDIT_OBJEKT *p_obj, int flag, int kflag);
void oe_kresli_kontejner(K_EDITOR *p_cnf, EDIT_MATERIAL **p_mat, EDIT_KONTEJNER *p_kont, int akt_objekt);
void oe_kresli_objekt_dratove(K_EDITOR *p_cnf, EDIT_MATERIAL **p_mat, EDIT_KONTEJNER *p_kont, EDIT_OBJEKT *p_obj, int akt_objekt, int akt_kont);
void oe_kresli_kontejner_dratove(K_EDITOR *p_cnf, EDIT_MATERIAL **p_mat, EDIT_KONTEJNER *p_kont, int akt_objekt);
void oe_kresli_kosoctverec(K_EDITOR *p_cnf, GLMATRIX *p_world, BOD *p_p, float r, float dr, float dg, float db);
void oe_kresli_kamery(K_EDITOR *p_cnf);
void oe_renderuj_scenu_pre(K_EDITOR *p_cnf);
void oe_renderuj_scenu_po(K_EDITOR *p_cnf);
void oe_renderuj_scenu_obyc(K_EDITOR *p_cnf,EDIT_KONTEJNER *p_kont[KONT_NUM], int max_kont);
void oe_renderuj_scenu_optimal(K_EDITOR *p_cnf);
void oe_renderuj_scenu_high_face(K_EDITOR *p_cnf);
#define VELIKOST_STREDU       2
#define VELIKOST_STREDU_VYBER 4
int  oe_test_kriz_stred(K_EDITOR *p_cnf,float vzdal, float x_s, float y_s, float z_s, int popis, int stred, int akt_osa);
int  oe_test_kriz_stred_bod(K_EDITOR *p_cnf,float vzdal,BOD *p_bod,int popis,int stred,int akt_osa);
void oe_animuj_materialy(K_EDITOR *p_cnf, byte event);
void oe_init_animace_materialy(K_EDITOR *p_cnf);
void oe_animuj_frame_material_event(dword akt_time, EDIT_MATERIAL *p_mat, EDIT_TEXT *p_text);
BOD * oe_cti_pozici_static_light(STATIC_LIGHT *p_light, BOD *p_bod);

void oe_kresli_edit_flare(K_EDITOR *p_cnf, LENS_FLARE *p_flare, int num);
void oe_kresli_game_flare(K_EDITOR *p_cnf, LENS_FLARE *p_flare, int num);

void oe_kresli_poly_3d(EDIT_MESH_POLY *p_poly, EDIT_MATERIAL **p_mat);
void oe_kresli_poly_3d_game(EDIT_MESH_POLY *p_poly, EDIT_MATERIAL **p_mat, int light);
void oe_renderuj_scenu_zrcadlo(K_EDITOR *p_cnf);
void oe_renderuj_scenu_kdtree(K_EDITOR *p_cnf, KD_BUNKA *p_prvni);

void oe_kresli_face(EDIT_OBJEKT *p_obj, int face);

void oe_kresli_kameru(KAMERA *p_cam, int pyr);
void oe_kresli_editacni_stred(K_EDITOR *p_cnf, BOD *p_stred, int flag);

void oe_nastav_material_single(EDIT_MATERIAL *p_smat, EDIT_MATERIAL *p_mat, EDIT_OBJEKT *p_obj, GLMATRIX *p_world);
int  oe_nastav_material_single_poly(EDIT_MATERIAL *p_smat, EDIT_MATERIAL *p_mat, EDIT_MESH_POLY *p_poly);

void oe_zrus_material_single(EDIT_MATERIAL *p_mat);

void oe_renderuj_all_off(void);
void oe_kresli_stred_levelu(K_EDITOR *p_cnf);

void oe_kresli_mlho_kostky(K_EDITOR *p_cnf);
void oe_renderuj_mlho_kostky(K_EDITOR *p_cnf);

void oe_kresli_kd_strom(KD_BUNKA *p_prvni, int uroven);
void oe_kresli_listu_prvku(K_EDITOR *p_cnf);
void oe_kresli_bb_kamery(K_EDITOR *p_cnf);
void oe_poly_env_spec(EDIT_MATERIAL *p_mat, EDIT_MESH_POLY *p_poly);

EDIT_OBJEKT * oe_cti_joint_objekt(K_EDITOR *p_cnf);


#define LISTA_X 100

__inline BOD * oe_cti_pozici_static_light(STATIC_LIGHT *p_light, BOD *p_bod)
{
  if(p_light->flag2&SL2_MESH) {
    transformuj_bod_matici_bod(&p_light->p,kont_world_matrix((EDIT_KONTEJNER *)p_light->p_mesh_data),p_bod);
  } else {
    *p_bod = p_light->p;
  }
  return(p_bod);
}

#define BARVA_KOSTI     0x00ff0000
#define BARVA_KOSTI_AKT 0x000000ff
#define RADIUS_KOSTI    0.2f
