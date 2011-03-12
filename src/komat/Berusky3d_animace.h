/*
  Animace.h
*/

#ifndef __BERUSKY_3D_ANIMACE
#define __BERUSKY_3D_ANIMACE

/*
  Init vsech animaci
*/
void ani_init(G_KONFIG *p_ber);

int  amat_pridej_mesh(G_KONFIG *p_ber, int mesh);
int  amat_pridej_poly(G_KONFIG *p_ber, int poly);
int  amat_pridej_material(G_KONFIG *p_ber, EDIT_MATERIAL *p_mat);
void amat_updatuj(G_KONFIG *p_ber);

int  amat_get_frame(EDIT_MATERIAL *p_mat);
int  amat_set_frame_text(EDIT_MATERIAL *p_mat, int frame);

void lani_updatuj(G_KONFIG *p_ber);

void rani_updatuj_prvek_matice(MeshHandle mh);
void rani_umisti_prvek_animace(MeshHandle mh);
void rani_updatuj(G_KONFIG *p_ber);

void tani_init_scena(G_KONFIG *p_ber);
void tani_updatuj(G_KONFIG *p_ber);

void tani_zrus(GK_TEXTANIM **p_tani);

void chani_updatuj(G_KONFIG *p_ber);
void chani_zrus_all(G_KONFIG *p_ber);

int  rani_je_mesh(RunHandle a_handle, MeshHandle m_handle);

#endif