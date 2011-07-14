/*
  Rendering berusek
*/  
  
#ifndef __BERUSKY3D_RENDER_H__
#define __BERUSKY3D_RENDER_H__
 void ber_renderuj_scenu(void);
 void ber_zrcadlo_nastav(void);
void ber_zrcadlo_zrus(void);

  
/*******************************************************************
   Postaveni dynamickeho render-listu
  ******************************************************************
*/ 
inline void ber_dynamic_mesh_list_vyrob(G_KONFIG * p_ber) 
{
  int i;

   memset(p_ber->p_dyn_meshlist, 0,
    sizeof(p_ber->p_dyn_meshlist[0]) * MAX_BERUSKY_KONTEJNERU);
  p_ber->dyn_meshlistnum = 0;
   for (i = 0; i < MAX_BERUSKY_KONTEJNERU; i++) {
    if (p_ber->p_mesh[i] && !(p_ber->p_mesh[i]->p_data->kflag & KONT_STATIC))
      p_ber->p_dyn_meshlist[p_ber->dyn_meshlistnum++] = &p_ber->p_mesh[i];
  }
}

 inline void ber_dynamic_mesh_list_vloz_mesh(G_KONFIG * p_ber,
  int meshnum) 
{
  p_ber->p_dyn_meshlist[p_ber->dyn_meshlistnum++] = &p_ber->p_mesh[meshnum];
}  inline void ber_dynamic_mesh_list_zrus(G_KONFIG * p_ber) 
{
  memset(p_ber->p_dyn_meshlist, 0,
    sizeof(p_ber->p_dyn_meshlist[0]) * MAX_BERUSKY_KONTEJNERU);
  p_ber->dyn_meshlistnum = 0;
}

   
/*******************************************************************
   Postaveni statickeho render-listu
  ******************************************************************
*/ 
void ber_render_list_vyrob(G_KONFIG * p_ber, int zrcadlo, int kamera_zmena);

  
/*******************************************************************
   Prochazeni mesh render listu
  ******************************************************************
*/ 
inline void ber_mesh_render_list_reset(G_KONFIG * p_ber) 
{
  p_ber->mesh_renderakt = 0;
}  inline GAME_MESH_OLD * ber_mesh_render_list_next(G_KONFIG * p_ber) 
{
  GAME_MESH_OLD ** p_tmp;
  while (p_ber->mesh_renderakt < p_ber->mesh_rendernum) {
    p_tmp = p_ber->p_mesh_renderlist[p_ber->mesh_renderakt++];
    if (p_tmp && *p_tmp) {
      return (*p_tmp);
    }
  }
  return (NULL);
}

 inline GAME_MESH_OLD * ber_mesh_render_list_next_flag(G_KONFIG * p_ber,
  int flag, int not_flag) 
{
  GAME_MESH_OLD ** p_tmp, *p_mesh;
  int kflag;

  while (p_ber->mesh_renderakt < p_ber->mesh_rendernum) {
    p_tmp = p_ber->p_mesh_renderlist[p_ber->mesh_renderakt++];
    if (p_tmp && *p_tmp) {
      p_mesh = *p_tmp;
      kflag = p_mesh->p_data->kflag;
      if ((kflag & flag) == flag && !(kflag & not_flag))
        return (p_mesh);
    }
  }
  return (NULL);
}

  
/*******************************************************************
   Prochazeni poly render listu
  ******************************************************************
*/ 
inline void ber_poly_render_list_zrus(G_KONFIG * p_ber) 
{
  null_free((void **) &p_ber->p_poly_renderlist);
  p_ber->poly_rendernum = 0;
  p_ber->poly_renderakt = 0;
}  inline void ber_poly_render_list_reset(G_KONFIG * p_ber) 
{
  p_ber->poly_renderakt = 0;
}  inline EDIT_MESH_POLY * ber_poly_render_list_next(G_KONFIG * p_ber) 
{
  return (p_ber->poly_renderakt <
    p_ber->poly_rendernum ? p_ber->p_poly_renderlist[p_ber->
      poly_renderakt++] : NULL);
}

 inline EDIT_MESH_POLY * ber_poly_render_list_next_flag(G_KONFIG * p_ber,
  int flag, int not_flag) 
{
  EDIT_MESH_POLY * p_poly;
  int kflag;

  while (p_ber->poly_renderakt < p_ber->poly_rendernum) {
    p_poly = p_ber->p_poly_renderlist[p_ber->poly_renderakt++];
    kflag = p_poly->kflag;
    if ((kflag & flag) == flag && !(kflag & not_flag))
      return (p_poly);
  }
  return (NULL);
}

 void ber_render_list_setrid(G_KONFIG * p_ber);
void ber_kresli_fleky(G_KONFIG * p_ber, FLEK * p_topflek);
void ber_renderuj_mlhu(G_KONFIG * p_ber);
void ber_nahod_render_funkce(void);
 void ber_kresli_poly_vertex(EDIT_MESH_POLY * p_poly,
  EDIT_MATERIAL ** p_mat);
void ber_kresli_poly_vertex_multitext(EDIT_MESH_POLY * p_poly,
  EDIT_MATERIAL ** p_mat);
void ber_kresli_poly_vertex_multitext_specular(EDIT_MESH_POLY * p_poly,
  EDIT_MATERIAL ** p_mat);
 void ber_kresli_poly_array(EDIT_MESH_POLY * p_poly,
  EDIT_MATERIAL ** p_mat);
void ber_kresli_poly_array_multitext(EDIT_MESH_POLY * p_poly,
  EDIT_MATERIAL ** p_mat, int spec);
  void ber_kresli_mesh_vertex(GAME_MESH_OLD * p_mesh,
  EDIT_MATERIAL ** p_mt);
void ber_kresli_mesh_vertex_multitext(GAME_MESH_OLD * p_mesh,
  EDIT_MATERIAL ** p_mt);
void ber_kresli_mesh_vertex_multitext_specular(GAME_MESH_OLD * p_mesh,
  EDIT_MATERIAL ** p_mt);
 void ber_kresli_mesh_array(GAME_MESH_OLD * p_mesh, EDIT_MATERIAL ** p_mt);
 int ber_nastav_material_single_multi_spec(EDIT_MATERIAL * p_smat,
  EDIT_MATERIAL * p_mat, dword kflag);
int ber_nastav_material_single_poly_multi_spec(EDIT_MATERIAL * p_smat,
  EDIT_MATERIAL * p_mat, dword kflag);
 int ber_nastav_material_single_multi(EDIT_MATERIAL * p_smat,
  EDIT_MATERIAL * p_mat, dword kflag);
int ber_nastav_material_single_poly_multi(EDIT_MATERIAL * p_smat,
  EDIT_MATERIAL * p_mat, dword kflag);
 int ber_nastav_material_single_nic(EDIT_MATERIAL * p_smat,
  EDIT_MATERIAL * p_mat, dword kflag);
int ber_nastav_material_single_poly_nic(EDIT_MATERIAL * p_smat,
  EDIT_MATERIAL * p_mat, dword kflag);
 void ber_ini_vertex_arrays(void);
void ber_ini_nic(void);
 void ber_test_render_mysi(void);
 extern EDIT_MATERIAL *__p_last_mat;
 void pe_renderuj(G_KONFIG * p_ber);
 void ber_render_list_vyrob_pruhlist(G_KONFIG * p_ber);

 
/* 
  Tabulka pointerovych funkci -> podle videokarty vyberu aktualni funkci
*/ 
  
#endif
