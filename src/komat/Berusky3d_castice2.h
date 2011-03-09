/* Casticovy system pro Berusky II
*/

#ifndef __BERUSKY3D_CASTICE2_H_
#define __BERUSKY3D_CASTICE2_H_


int  pe_updatuj_strepiny(G_KONFIG *p_ber, PARMETAC *p_par);
int  pe_updatuj_fleky(G_KONFIG *p_ber, PARMETAC *p_par);
int  pe_updatuj_kour_stopa(G_KONFIG *p_ber, PARMETAC *p_mt);
void pe_updatuj(G_KONFIG *p_ber);

void vod_zrus_vodniky(VODNIK *p_vodnik);
void vod_zrus(VODA2 **p_tmp);
void vod_updatuj(VODA2 *p_voda);

#endif
