/*
  Animace.h
*/

#ifndef __BERUSKY_3D_KAMERY
#define __BERUSKY_3D_KAMERY

/*
  Init vsech animaci
*/
void kani_updatuj(G_KONFIG *p_ber);
void ber_zpruhledni_prvky(G_KONFIG *p_ber);
void ber_zpruhlednuj_scenu_polylist_rec(G_KONFIG *p_ber, OBB_TREE *p_prvni, GLMATRIX *p_m, BOD *p, BOD *t);
void ber_zpruhledni_prvky_reset(G_KONFIG *p_ber);
int  ber_kamera_korekce_vzdalenosti(G_KONFIG *p_ber, int korekce, int korekce_vzdal);
void ber_kamera_zpruhledni_objekty(G_KONFIG *p_ber);
void ber_kamera_zpruhledni_objekty_reset(G_KONFIG *p_ber);

void kam_set_kino_screen(G_KONFIG *p_ber);
void kam_set_normal_screen(G_KONFIG *p_ber);
void kam_kino_set_clear(G_KONFIG *p_ber);
void kam_game_set_clear(G_KONFIG *p_ber);

__inline static void ber_reset_mesh(BODRGBA *p_d, BODRGBA *p_src, int num)
{
  int v;  
  for(v = 0; v < num; v++) {
    p_d->a = p_src->a;
    p_src++;
    p_d++;
  }  
}

__inline static void ber_reset_poly(TEXT_KOORD *p_d, int num)
{
  int v;  
  for(v = 0; v < num; v++) {
    p_d[v].da = p_d[v].mda;
  }
}

#endif