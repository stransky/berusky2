/*
  berusky3d_animace.c
  Animacni modul k beruskam II

  posuny objektu
  kyframe animace objektu
  animacni hierachie
*/
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "3d_all.h"

#include "Object.h"

#include "Berusky_universal.h"
#include "Berusky3d_castice.h"
#include "Berusky3d.h"
#include "Berusky3d_ini.h"
#include "Berusky3d_load.h"
#include "Berusky3d_render.h"
#include "Berusky3d_animace.h"
#include "Berusky3d_kofola_interface.h"

extern G_KONFIG ber, *p_ber;


/*--------------------------------------------------------
  Animovane materialy
  --------------------------------------------------------
*/
/* Prida animaci do animacniho listu z meshu
*/
int amat_pridej_mesh(G_KONFIG * p_ber, int mesh)
{
  EDIT_MATERIAL **p_imat = p_ber->p_mat;
  EDIT_MATERIAL *p_mat;
  GAME_MESH_OLD *p_mesh = p_ber->p_mesh[mesh];
  int i, aa;

  assert(p_mesh);

  if (p_mesh->p_data->m1flag & MAT_ANIM_FRAME) {
    for (i = 0; i < p_mesh->objektu; i++) {
      p_mat = p_imat[p_mesh->p_mat[i]];
      if (p_mat->flag & MAT_ANIM_FRAME) {
        //p_mesh->p_mat[i] -> tento material
        if (p_ber->mannum >= MAX_MAT_ANIMACI) {
          ddw("Nedostatek volnych slotu animaci !");
          assert(0);
        }
        else {
          aa = p_ber->mannum++;
          p_ber->p_man[aa].mesh = mesh;
          p_ber->p_man[aa].objekt = i;
          p_ber->p_man[aa].p_anim = p_mat;      // odkazuj material
        }
      }
    }
  }
  return (p_ber->mannum);
}

/* Prihodi na animovani material
*/
int amat_pridej_material(G_KONFIG * p_ber, EDIT_MATERIAL * p_mat)
{
  int aa;

  assert(p_mat);

  if (p_mat->flag & MAT_ANIM_FRAME) {
    if (p_ber->mannum >= MAX_MAT_ANIMACI) {
      ddw("Nedostatek volnych slotu animaci !");
      assert(0);
    }
    else {
      aa = p_ber->mannum++;
      p_ber->p_man[aa].mesh = K_CHYBA;
      p_ber->p_man[aa].objekt = K_CHYBA;
      p_ber->p_man[aa].p_anim = p_mat;
    }
  }
  return (p_ber->mannum);
}

/* Vrati aktivni frame u animovaheho materialu
   K_CHYBA pokud material neni animovany
*/
int amat_get_frame(EDIT_MATERIAL * p_mat)
{
  if (p_mat->anim.p_frame)
    return (p_mat->anim.frameakt);
  else {
    kprintfe(TRUE, "K_CHYBA - FILE %s LINE %d", __FILE__, __LINE__);
    return (K_CHYBA);
  }
}

/* Nastavi frame u animovaneho materialu
*/
int amat_set_frame_text(EDIT_MATERIAL * p_mat, int frame)
{
  ANIM_MATERIAL *p_anim = &p_mat->anim;

  if (p_anim->p_frame) {
    ANIM_FRAME *p_frame = p_anim->p_frame + frame;
    dword flag = p_frame->flag;

    if (flag & FRAME_TEXTURA) {
      text_set_num(0);
      text_set(p_frame->p_text->text, p_frame->p_text->typ);
    }
    return (TRUE);
  }
  else {
    kprintfe(TRUE, "K_CHYBA - FILE %s LINE %d", __FILE__, __LINE__);
    return (K_CHYBA);
  }
}

/* Prida animaci do animacniho listu z poly
*/
int amat_pridej_poly(G_KONFIG * p_ber, int poly)
{
  EDIT_MESH_POLY *p_poly = p_ber->p_poly + poly;
  int aa;

  assert(p_poly);

  //p_mesh->p_mat[i] -> tento material
  if (p_poly->m1flag & MAT_ANIM_FRAME) {
    if (p_ber->mannum >= MAX_MAT_ANIMACI) {
      ddw("Nedostatek volnych slotu animaci !");
      assert(0);
    }
    else {
      aa = p_ber->mannum++;
      p_ber->p_man[aa].mesh = poly;
      p_ber->p_man[aa].objekt = K_CHYBA;
      p_ber->p_man[aa].p_anim = p_ber->p_mat[p_poly->material]; // odkazuj material
    }
  }
  return (p_ber->mannum);
}

/* Updatuje frame animace -> vraci cas dalsi aktualizace
*/
inline int amat_frame_updatuj(EDIT_MATERIAL * p_mat, int *p_event)
{
  ANIM_MATERIAL *p_anim = &p_mat->anim;
  int next = (p_anim->frameakt + 1 >= p_anim->framenum) ? p_anim->frameakt =
    0 : ++p_anim->frameakt;
  ANIM_FRAME *p_frame = p_anim->p_frame + next;
  dword flag;

  assert(p_frame);
  flag = p_frame->flag;

  if (flag & FRAME_TEXTURA)
    p_mat->p_text[0] = p_frame->p_text;
  if (flag & FRAME_POSUN) {
    p_mat->ttext[0]._41 = p_frame->u;
    p_mat->ttext[0]._42 = p_frame->v;
  }

  *p_event = (flag & FRAME_EVENT);
  return (p_anim->p_frame[next].time);
}

void amat_updatuj(G_KONFIG * p_ber)
{
  MAL_BUNKA *p_man;
  int i;

  for (i = 0; i < p_ber->mannum; i++) {
    p_man = p_ber->p_man + i;
    if (p_man->p_anim->anim.p_frame) {
      if (p_man->next_time < p_ber->TimeEndLastFrame && !p_man->event) {
        // updatuj tento frame animace
        p_man->next_time = p_ber->TimeEndLastFrame +
          amat_frame_updatuj(p_man->p_anim, &p_man->event);
      }
    }
  }
}

/* Casove nahozeni animaci
*/
void ani_init(G_KONFIG * p_ber)
{
  int i;

  for (i = 0; i < p_ber->mannum; i++) {
    p_ber->p_man[i].next_time = p_ber->TimeBase;
  }
}

/* 
  Dotaz na existenci animovaneho materialu
*/
int kom_amat_mesh_get(int mesh, int objekt)
{
  int i;

  if (!(p_ber->p_mesh[mesh]) || objekt >= p_ber->p_mesh[mesh]->objektu) {
    return (K_CHYBA);
  }

  for (i = 0; i < p_ber->mannum; i++) {
    if (p_ber->p_man[i].mesh == mesh && p_ber->p_man[i].objekt == objekt)
      return (i);
  }

  return (K_CHYBA);
}

/* 
  Skoci animaci na tento frame a hodi casovac 0
*/
int kom_amat_set_frame(int handle, int frame)
{
  MAL_BUNKA *p_man = p_ber->p_man + handle;

  assert(handle < p_ber->mannum);

  p_man->p_anim->anim.frameakt = frame;
  p_man->next_time =
    p_ber->TimeEndLastFrame + amat_frame_updatuj(p_man->p_anim,
    &p_man->event);
  return (TRUE);
}

/* 
  Zrusi event flag animace
*/
int kom_amat_event(int handle)
{
  MAL_BUNKA *p_man = p_ber->p_man + handle;

  assert(handle < p_ber->mannum);
  p_ber->p_man[handle].event = 0;

  p_man->next_time =
    p_ber->TimeEndLastFrame + amat_frame_updatuj(p_man->p_anim,
    &p_man->event);
  return (TRUE);
}

/*
  Cte flagy animace - bezici/jina
  0 = stoji
  1 = bezi
*/
int kom_amat_get(int handle)
{
  assert(handle < p_ber->mannum);
  return (!p_ber->p_man[handle].event);
}

int kom_amat_mesh_get_num(void)
{
  return (p_ber->mannum);
}

int kom_amat_mesh_get_all(MATERIAL_BUNKA * p_bunky)
{
  int i;

  for (i = 0; i < p_ber->mannum; i++) {
    p_bunky[i].handle_materialove_animace = i;
    p_bunky[i].handle_meshe = p_ber->p_man[i].mesh;
    p_bunky[i].cislo_objektu = p_ber->p_man[i].objekt;
  }
  return (p_ber->mannum);
}

int kom_amat_get_kontejner(MeshHandle mh)
{
  if (p_ber->p_prv_lev[mh]) {
    return (p_ber->p_prv_lev[mh]->mesh);
  }
  else {
    return (K_CHYBA);
  }
}

/*---------------------------------------------------------------------
  Globalni animace
  ---------------------------------------------------------------------
*/
/* Vraci handle nebo K_CHYBA kdyz nic/chyba loadu
*/
AnimHandle sim_nahraj_animaci(APAK_HANDLE * pHandle, char *p_jmeno,
  int linearne)
{
  AnimHandle sim;
  int i;

  for (i = 0; i < p_ber->simnum; i++) {
    if (!p_ber->p_sim[i].flag) {
      sim = i;
      goto naslo_se_to1;
    }
  }

  if (p_ber->simnum == MAX_SIM_ANIMACI) {
    kprintfe(TRUE, "K_CHYBA - FILE %s LINE %d", __FILE__, __LINE__);
    return (K_CHYBA);
  }
  else
    sim = p_ber->simnum++;

naslo_se_to1:;

  if (!key_sim_nahraj(pHandle, p_ber->p_sim + sim, p_jmeno,
      p_ber->dir.data_dir, linearne))
    return (K_CHYBA);

  p_ber->p_sim[sim].flag |= SIMPLE_AKTIVNI;

  return (sim);
}

/* vyrobi animaci padu (delku framu prekona za delku)
*/

/* vyrobi slot v teto animaci
*/
AnimHandle sim_vyrob_animaci(int pos_key, int rot_key, int scale_key)
{
  AnimHandle sim;
  int i;

  for (i = 0; i < p_ber->simnum; i++) {
    if (!p_ber->p_sim[i].flag) {
      sim = i;
      goto naslo_se_to2;
    }
  }

  if (p_ber->simnum >= MAX_SIM_ANIMACI) {
    kprintfe(TRUE, "sim_vyrob_animaci = -1, FILE %s LINE %d", __FILE__,
      __LINE__);
    return (K_CHYBA);
  }
  else
    sim = p_ber->simnum++;

naslo_se_to2:;

  p_ber->p_sim[sim].flag |= SIMPLE_AKTIVNI;
  key_tri_vyrob_indir(p_ber->p_track + sim, pos_key, rot_key, scale_key);
  return (sim);
}

AnimHandle sim_vloz_klic_posun(AnimHandle handle, int klic, float x, float y,
  float z, int frame)
{
  if (handle >= p_ber->simnum || klic >= p_ber->p_track[handle].pos_keys) {
    kprintfe(TRUE, "sim_vloz_klic_posun = -1, FILE %s LINE %d", __FILE__,
      __LINE__);
    return (K_CHYBA);
  }
  p_ber->p_track[handle].p_pos[klic].x = x;
  p_ber->p_track[handle].p_pos[klic].y = y;
  p_ber->p_track[handle].p_pos[klic].z = z;
  p_ber->p_track[handle].p_pkeys[klic].time = frame * SIM_KONSTI;
  return (handle);
}

AnimHandle sim_vloz_klic_posun_bod(AnimHandle handle, int klic, BOD * p_p,
  int frame)
{
  if (handle >= p_ber->simnum || klic >= p_ber->p_track[handle].pos_keys) {
    kprintfe(TRUE, "sim_vloz_klic_posun_bod = -1, FILE %s LINE %d", __FILE__,
      __LINE__);
    return (K_CHYBA);
  }
  p_ber->p_track[handle].p_pos[klic] = *p_p;
  p_ber->p_track[handle].p_pkeys[klic].time = frame * SIM_KONSTI;
  return (handle);
}

AnimHandle sim_vloz_klic_posun_parametry(AnimHandle handle, int klic,
  float tension, float continuity, float bias)
{
  if (handle >= p_ber->simnum || klic >= p_ber->p_track[handle].pos_keys) {
    kprintfe(TRUE, "sim_vyrob_animaci = -1, FILE %s LINE %d", __FILE__,
      __LINE__);
    return (K_CHYBA);
  }
  p_ber->p_track[handle].p_pkeys[klic].bias = bias;
  p_ber->p_track[handle].p_pkeys[klic].continuity = continuity;
  p_ber->p_track[handle].p_pkeys[klic].tension = tension;
  return (handle);
}

// uhel je ve stupnich
AnimHandle sim_vloz_klic_rotace(AnimHandle handle, int klic, BOD * p_osa,
  float uhel, int frame)
{
  if (handle >= p_ber->simnum || klic >= p_ber->p_track[handle].rot_keys) {
    kprintfe(TRUE, "K_CHYBA - FILE %s LINE %d", __FILE__, __LINE__);
    return (K_CHYBA);
  }

  /* Korekce zaporneho uhlu
   */
  if (uhel < 0.0f) {
    uhel = -uhel;
    p_osa->x = -p_osa->x;
    p_osa->y = -p_osa->y;
    p_osa->z = -p_osa->z;
  }
  quat_norm(angle_to_quat(p_ber->p_track[handle].p_at + klic, p_osa,
      (uhel * PI) / 180.0f));
  p_ber->p_track[handle].p_rkeys[klic].time = frame * SIM_KONSTI;
  return (handle);
}

AnimHandle sim_vloz_klic_rotace_quat(AnimHandle handle, int klic, QUAT * p_q,
  int frame)
{
  if (handle >= p_ber->simnum || klic >= p_ber->p_track[handle].rot_keys) {
    kprintfe(TRUE, "K_CHYBA - FILE %s LINE %d", __FILE__, __LINE__);
    return (K_CHYBA);
  }

  /* Korekce zaporneho uhlu
   */
  p_ber->p_track[handle].p_at[klic] = *p_q;
  quat_norm(p_ber->p_track[handle].p_at + klic);
  p_ber->p_track[handle].p_rkeys[klic].time = frame * SIM_KONSTI;
  return (handle);
}

AnimHandle sim_vloz_klic_rotace_parametry(AnimHandle handle, int klic,
  float tension, float continuity, float bias)
{
  if (handle >= p_ber->simnum || klic >= p_ber->p_track[handle].rot_keys) {
    kprintfe(TRUE, "K_CHYBA - FILE %s LINE %d", __FILE__, __LINE__);
    return (K_CHYBA);
  }
  p_ber->p_track[handle].p_rkeys[klic].bias = bias;
  p_ber->p_track[handle].p_rkeys[klic].continuity = continuity;
  p_ber->p_track[handle].p_rkeys[klic].tension = tension;
  return (handle);
}

AnimHandle sim_vloz_klic_scale(AnimHandle handle, int klic, float x, float y,
  float z, int frame)
{
  if (handle >= p_ber->simnum || klic >= p_ber->p_track[handle].scs_keys) {
    kprintfe(TRUE, "K_CHYBA - FILE %s LINE %d", __FILE__, __LINE__);
    return (K_CHYBA);
  }

  p_ber->p_track[handle].p_skeys[klic].time = frame * SIM_KONSTI;
  p_ber->p_track[handle].p_scale[klic].x = x;
  p_ber->p_track[handle].p_scale[klic].y = y;
  p_ber->p_track[handle].p_scale[klic].z = z;
  return (handle);
}

AnimHandle sim_vloz_klic_scale_bod(AnimHandle handle, int klic, BOD * p_scs,
  int frame)
{
  if (handle >= p_ber->simnum || klic >= p_ber->p_track[handle].scs_keys) {
    kprintfe(TRUE, "K_CHYBA - FILE %s LINE %d", __FILE__, __LINE__);
    return (K_CHYBA);
  }
  p_ber->p_track[handle].p_scale[klic] = *p_scs;
  p_ber->p_track[handle].p_skeys[klic].time = frame * SIM_KONSTI;
  return (handle);
}

AnimHandle sim_vloz_klic_scale_parametry(AnimHandle handle, int klic,
  float tension, float continuity, float bias)
{
  if (handle >= p_ber->simnum || klic >= p_ber->p_track[handle].scs_keys) {
    kprintfe(TRUE, "K_CHYBA - FILE %s LINE %d", __FILE__, __LINE__);
    return (K_CHYBA);
  }
  p_ber->p_track[handle].p_skeys[klic].bias = bias;
  p_ber->p_track[handle].p_skeys[klic].continuity = continuity;
  p_ber->p_track[handle].p_skeys[klic].tension = tension;
  return (handle);
}

AnimHandle sim_vloz_pivot(AnimHandle handle, float x, float y, float z)
{
  if (handle >= p_ber->simnum) {
    kprintfe(TRUE, "K_CHYBA - FILE %s LINE %d", __FILE__, __LINE__);
    return (K_CHYBA);
  }
  p_ber->p_track[handle].pivot.x = x;
  p_ber->p_track[handle].pivot.y = y;
  p_ber->p_track[handle].pivot.z = z;
  return (handle);
}

AnimHandle sim_vloz_pivot_bod(AnimHandle handle, BOD * p_bod)
{
  if (handle >= p_ber->simnum) {
    kprintfe(TRUE, "K_CHYBA - FILE %s LINE %d", __FILE__, __LINE__);
    return (K_CHYBA);
  }
  p_ber->p_track[handle].pivot = *p_bod;
  return (handle);
}

/* Rozinterpoluje key-frame animaci
*/
AnimHandle sim_interpoluj_animaci(AnimHandle handle, int framenum, int loop)
{
  if (handle >= p_ber->simnum) {
    kprintfe(TRUE, "K_CHYBA - FILE %s LINE %d", __FILE__, __LINE__);
    return (K_CHYBA);
  }
  p_ber->p_track[handle].endtime = calc_endtime(framenum);
  key_tri_to_sim_indir(p_ber->p_track + handle, p_ber->p_sim + handle, loop,
    FALSE);
  p_ber->p_sim[handle].flag |= SIMPLE_AKTIVNI;
  return (handle);
}

/* Zrusi animaci
*/
AnimHandle sim_zrus_animaci(AnimHandle handle)
{
  if (handle >= p_ber->simnum || !p_ber->p_sim[handle].flag) {
    kprintfe(TRUE, "K_CHYBA - FILE %s LINE %d", __FILE__, __LINE__);
    return (K_CHYBA);
  }
  else {
    p_ber->p_sim[handle].flag = 0;
    zrus_tri_vnitrek(p_ber->p_track + handle);
    key_sim_zrus_vnitrek(p_ber->p_sim + handle);
    if (handle + 1 == p_ber->simnum)
      p_ber->simnum--;
  }
  return (handle);
}

/**************************************************************************
  Run-animace
 **************************************************************************

  Aktivace animace
*/

RunHandle rani_aktivuj(AnimHandle shandle, int *p_flag, int flag, int start,
  int stop)
{
  GK_ANIM *p_gk;
  RunHandle ah;
  int i;

  for (i = 0; i < p_ber->gknum; i++) {
    if (!p_ber->p_gkzas[i].flag) {
      ah = i;
      goto naslo_se_to;
    }
  }

  if (p_ber->gknum == MAX_RUN_ANIMACI) {
    kprintfe(TRUE, "K_CHYBA - FILE %s LINE %d", __FILE__, __LINE__);
    return (K_CHYBA);
  }
  else {
    ah = p_ber->gknum++;
  }

naslo_se_to:;
  p_gk = p_ber->p_gkzas + ah;

  p_gk->p_flag = p_flag;
  p_gk->p_sim = p_ber->p_sim + shandle;
  p_gk->flag |= GK_POUZITA;
  p_gk->matic = 0;
  p_gk->next_ah = K_CHYBA;
  p_gk->p_endfce = NULL;

  // Zarazeni do render-listu
  p_gk->p_next = p_ber->p_gkanim;
  p_gk->p_prev = NULL;
  if (p_gk->p_next)
    p_gk->p_next->p_prev = p_gk;
  p_ber->p_gkanim = p_gk;

  return (rani_go((int) p_gk, flag, start, stop));
}

RunHandle rani_aktivuj_cekej(AnimHandle shandle, int *p_flag)
{
  GK_ANIM *p_gk;
  RunHandle ah;
  int i;

  for (i = 0; i < p_ber->gknum; i++) {
    if (!p_ber->p_gkzas[i].flag) {
      ah = i;
      goto naslo_se_to;
    }
  }

  if (p_ber->gknum == MAX_RUN_ANIMACI) {
    kprintfe(TRUE, "K_CHYBA - FILE %s LINE %d", __FILE__, __LINE__);
    return (K_CHYBA);
  }
  else {
    ah = p_ber->gknum++;
  }

naslo_se_to:;
  p_gk = p_ber->p_gkzas + ah;

  p_gk->p_flag = p_flag;
  p_gk->p_sim = p_ber->p_sim + shandle;
  p_gk->flag |= GK_POUZITA;
  p_gk->matic = 0;
  p_gk->next_ah = K_CHYBA;
  p_gk->p_endfce = NULL;

  // Zarazeni do render-listu
  p_gk->p_next = p_ber->p_gkanim;
  p_gk->p_prev = NULL;
  if (p_gk->p_next)
    p_gk->p_next->p_prev = p_gk;
  p_ber->p_gkanim = p_gk;

  return ((int) p_gk);
}

RunHandle rani_pripoj_funkci(RunHandle a_handle, END_FUNKCE p_fce, int param,
  int param2, void *p_param)
{
  GK_ANIM *p_gk = (GK_ANIM *) a_handle;

  p_gk->p_endfce = p_fce;
  p_gk->p_param = p_param;
  p_gk->param = param;
  p_gk->param2 = param2;
  return (a_handle);
}

RunHandle rani_next_animace(RunHandle ah, RunHandle next_ah, int flag,
  int start, int stop)
{
  GK_ANIM *p_gk = (GK_ANIM *) ah;

  p_gk->next_ah = next_ah;
  p_gk->next_flag = flag;
  p_gk->next_start = start;
  p_gk->next_stop = stop;
  return (ah);
}

/*
  Pripoji k animaci (druh)
  MeshHandle
*/
MatrixHandle rani_privaz_mesh(RunHandle a_handle, MeshHandle m_handle,
  int transformuj_pivot)
{
  GAME_MESH_OLD *p_mesh;
  GK_ANIM *p_gk = (GK_ANIM *) a_handle;
  GKA_MATICE *p_mat;
  MatrixHandle matice;

  if (!p_ber->p_prv_lev[m_handle]) {
    ddw("Chybne privazani mesh-handle %d", m_handle);
    kprintfe(TRUE, "K_CHYBA - FILE %s LINE %d", __FILE__, __LINE__);
    return (K_CHYBA);
  }

  if (p_gk->matic >= MAX_ANIM_MATIC) {
    kprintfe(TRUE, "K_CHYBA - FILE %s LINE %d", __FILE__, __LINE__);
    return (K_CHYBA);
  }

  matice = p_gk->matic++;
  p_mat = p_gk->p_mat + matice;

  p_mat->p_mat = &p_ber->p_prv_lev[m_handle]->mg;
  p_mat->p_notify = NULL;
  p_mat->flagy = MESH_USE_MATRIX | MESH_PRVEK;
  p_mat->fce_param = m_handle;
  p_mat->funkce_start = NULL;
  p_mat->funkce_frame = rani_updatuj_prvek_matice;
  p_mat->funkce_end = rani_umisti_prvek_animace;

  // Nastavi pivot point pro rotaci --- sim animace -> mesh
  if (p_gk->p_sim->p_at) {
    p_mat->flagy |=
      (transformuj_pivot) ? MESH_TRANSFORMUJ_PIVOT | MESH_PIVOT : MESH_PIVOT;
  }

  //__OPRAVA__
  // vsechny meshe naflaguj
  p_mesh = p_ber->p_mesh[p_ber->p_prv_lev[m_handle]->mesh];
  p_mesh->sim_flag |= GAME_MESH_RANI;

  return (matice);
}

int rani_je_mesh(RunHandle a_handle, MeshHandle m_handle)
{
  GK_ANIM *p_gk = (GK_ANIM *) a_handle;
  GKA_MATICE *p_mat;
  int m;

  assert(m_handle >= 0 && m_handle < MAX_PRVKU_LEVEL);
  if (!p_ber->p_prv_lev[m_handle])
    return (FALSE);

  p_mat = p_gk->p_mat;
  for (m = 0; m < p_gk->matic; m++, p_mat++) {
    if (p_mat->fce_param == m_handle)
      return (TRUE);
  }
  return (FALSE);
}

/*
  Pripoji k animaci (druh)
*/
RunHandle rani_rozvaz(RunHandle a_handle, MatrixHandle m_handle)
{
  GK_ANIM *p_gk = (GK_ANIM *) a_handle;

  if (m_handle < MAX_ANIM_MATIC) {
    p_gk->p_mat[m_handle].p_mat = NULL;
    p_gk->p_mat[m_handle].p_notify = NULL;
    return (TRUE);
  }
  else {
    kprintfe(TRUE, "K_CHYBA - FILE %s LINE %d", __FILE__, __LINE__);
    return (K_CHYBA);
  }
}


/* Znovu - Spusteni animace
*/
RunHandle rani_go(RunHandle ah, int flag, int start, int stop)
{
  GAME_MESH_OLD *p_mesh;
  GK_ANIM *p_gk = (GK_ANIM *) ah;
  dword mflag;
  int m_handle;
  int i;

  calc_time(p_gk->p_sim->keynum, p_ber->TimeEndLastFrame, start, stop,
    &p_gk->start, &p_gk->stop, &p_gk->time_start, &p_gk->time_stop,
    &p_gk->time_delka);

  p_gk->flag |= (GK_AKTIVNI | flag);
  p_gk->konec = FALSE;

  for (i = 0; i < p_gk->matic; i++) {
    m_handle = p_gk->p_mat[i].fce_param;
    if (p_gk->p_mat[i].funkce_start)
      p_gk->p_mat[i].funkce_start(ah);
    if (p_gk->p_mat[i].flagy & MESH_PRVEK)
      p_ber->p_prv_lev[m_handle]->anim_mail = K_CHYBA;
    mflag = p_gk->p_mat[i].flagy;
    if (mflag & MESH_PIVOT) {
      if (mflag & MESH_TRANSFORMUJ_PIVOT) {
        GLMATRIX m;

        p_mesh = p_ber->p_mesh[p_ber->p_prv_lev[m_handle]->mesh];
        memcpy(&m, &p_mesh->m, sizeof(m));
        m._41 = m._42 = m._43 = 0.0f;
        transformuj_bod_matici_bod(&p_gk->p_sim->pivot, &m,
          &p_ber->p_prv_lev[m_handle]->pivot);
      }
      else {
        p_ber->p_prv_lev[m_handle]->pivot = p_gk->p_sim->pivot;
      }
    }
  }

  return (ah);
}

// stopne animaci
int rani_zrus(RunHandle ahandle)
{
  GK_ANIM *p_gk = (GK_ANIM *) ahandle;

  // zrusi posledni
  p_gk->flag = 0;

  // Rozvaze ji ze seznamu aktivnich animaci
  if (!p_gk->p_prev) {
    p_ber->p_gkanim = p_gk->p_next;
    if (p_ber->p_gkanim)
      p_ber->p_gkanim->p_prev = NULL;
  }
  else {
    p_gk->p_prev->p_next = p_gk->p_next;
    if (p_gk->p_next)
      p_gk->p_next->p_prev = p_gk->p_prev;
  }

  return (TRUE);
}

/* ---------------------------------
   Interni rutiny pro run-animace
   ---------------------------------
*/
void rani_updatuj_prvek_matice_vnitrek(PRVEK_LEVELU_GAME * p_prv)
{
  PRVEK_LEVELU_GAME *p_top;
  BOD *p_pivot, p;              // pouzij pivot point meshe
  GLMATRIX tmp1;
  GAME_MESH_OLD *p_mesh;

  if (p_prv->anim_mail == p_ber->anim_mail)
    return;
  else
    p_prv->anim_mail = p_ber->anim_mail;

  p_pivot = &p_prv->pivot;
  p_mesh = p_ber->p_mesh[p_prv->mesh];

  if (p_prv->flag & PRV_TOP && p_prv->p_next_low) {     // Provaz matici mg vsechny pod-objekty a updatuj je
    // Pivotuj matici MG  
    mat_copy(&tmp1, &p_prv->mg);
    vektor_add((BOD *) (&p_prv->mp._41), p_pivot, &p);
    pivotuj_matrix(&tmp1, &p);

    /* Ukonci animovani - konec + pronasobeni p_mg_top
     */
    if (p_prv->flag & PRV_ANIM_END) {
      p_top = p_prv->p_next_low;
      while (p_top) {
        p_top->p_mg_top = NULL;
        mat_mult(&p_top->mp, &tmp1, &p_top->mp);
        rani_updatuj_prvek_matice_vnitrek(p_top);
        p_top = p_top->p_next_low;
      }
    }

    /* Nastav top_mg a vynasob to
     */
    else {
      p_top = p_prv->p_next_low;
      while (p_top) {
        p_top->p_mg_top = &tmp1;
        rani_updatuj_prvek_matice_vnitrek(p_top);
        p_top = p_top->p_next_low;
      }
    }
  }
  else if (p_prv->flag & PRV_LOW && p_prv->p_mg_top) {
    mat_mult(p_prv->p_mg_top, &p_prv->mg, &tmp1);
    vektor_add((BOD *) (&p_prv->mp._41), p_pivot, &p);  // nakopiruj si pivot + pozice
    transformuj_bod_bod_matici(&p, p_prv->p_mg_top);
    pivotuj_matrix(&tmp1, &p);  // pivotuj animovanou matici
  }
  else {
    mat_copy(&tmp1, &p_prv->mg);
    vektor_add((BOD *) (&p_prv->mp._41), p_pivot, &p);
    pivotuj_matrix(&tmp1, &p);
  }

  // vysledna transformacni matice polohy = p_mesh->m
  mat_mult_dir(&p_prv->mp, &tmp1, &p_mesh->m);

  // p_prv->p_mesh->m - vysledna matice
  if (p_mesh->p_data->kflag & KONT_KEYFRAME) {
    mat_mult_dir(&p_mesh->world, &p_mesh->m, &tmp1);
    key_mesh_reanimuj(p_mesh, p_mesh->sim_aktivni_time, &tmp1);
  }
  else {
    key_mesh_transformuj_obalky(p_mesh, &p_mesh->m);
  }

  p_prv->x = p_mesh->m._41;
  p_prv->y = p_mesh->m._42 + Y_PRVEK2;
  p_prv->z = p_mesh->m._43;

  p_mesh->p_data->kflag |= KONT_POHYB;

  if (p_prv->flag & PRV_ANIM_END) {
    p_prv->flag &= ~PRV_ANIM_END;
    p_prv->mp = p_mesh->m;
    init_matrix(&p_prv->mg);
    p_mesh->sim_flag = FALSE;
  }
}

void rani_updatuj_prvek_matice_vnitrek_end_top(PRVEK_LEVELU_GAME * p_prv)
{
  BOD *p_pivot, p;              // pouzij pivot point meshe
  GLMATRIX tmp1;
  GAME_MESH_OLD *p_mesh;

  if (p_prv->p_mg_top) {
    p_pivot = &p_prv->pivot;
    p_mesh = p_ber->p_mesh[p_prv->mesh];

    // Pivotuj matici MG  
    mat_copy(&tmp1, &p_prv->mg);
    vektor_add((BOD *) (&p_prv->mp._41), p_pivot, &p);
    pivotuj_matrix(&tmp1, &p);

    mat_mult(&p_prv->mp, p_prv->p_mg_top, &p_prv->mp);
    mat_mult_dir(&p_prv->mp, &tmp1, &p_mesh->m);

    // p_prv->p_mesh->m - vysledna matice
    if (p_mesh->p_data->kflag & KONT_KEYFRAME) {
      mat_mult_dir(&p_mesh->world, &p_mesh->m, &tmp1);
      key_mesh_reanimuj(p_mesh, p_mesh->sim_aktivni_time, &tmp1);
    }
    else {
      key_mesh_transformuj_obalky(p_mesh, &p_mesh->m);
    }

    p_prv->x = p_mesh->m._41;
    p_prv->y = p_mesh->m._42 + Y_PRVEK2;
    p_prv->z = p_mesh->m._43;

    p_mesh->p_data->kflag |= KONT_POHYB;
  }
}

/* Frame-funkce posunu meshu
*/
void rani_updatuj_prvek_matice(MeshHandle mh)
{
  PRVEK_LEVELU_GAME *p_prv = p_ber->p_prv_lev[mh];
  int flag = p_prv->flag;

  p_prv->flag &= ~PRV_ANIM_END;

  if (flag & PRV_TOP) {         // pridej to do top-listu
    p_ber->p_prv_lev_top[p_ber->prvnum_top++] = p_prv;
    assert(p_ber->prvnum_top < MAX_PRVKU_LEVEL_TOP);
  }
  else if (flag & PRV_LOW) {    // pridej to do low-listu
    p_ber->p_prv_lev_low[p_ber->prvnum_low++] = p_prv;
    assert(p_ber->prvnum_low < MAX_PRVKU_LEVEL_LOW);
  }
  else {
    rani_updatuj_prvek_matice_vnitrek(p_prv);
  }
}

/* End-funkce posunu prvku
*/
void rani_umisti_prvek_animace(MeshHandle mh)
{
  PRVEK_LEVELU_GAME *p_prv = p_ber->p_prv_lev[mh];
  int flag = p_prv->flag;

  p_prv->flag |= PRV_ANIM_END;

  if (flag & PRV_TOP) {         // pridej to do top-listu
    p_ber->p_prv_lev_top[p_ber->prvnum_top++] = p_prv;
    assert(p_ber->prvnum_top < MAX_PRVKU_LEVEL_TOP);
  }
  else if (flag & PRV_LOW) {    // pridej to do low-listu
    p_ber->p_prv_lev_low[p_ber->prvnum_low++] = p_prv;
    assert(p_ber->prvnum_low < MAX_PRVKU_LEVEL_LOW);
  }
  else {
    rani_updatuj_prvek_matice_vnitrek(p_prv);
  }
}

/* Updatuje vsechny top objekty a s nimi i low objekty ktere se zmenily
*/
void rani_updatuj_top_objekty(void)
{
  int i;

  for (i = 0; i < p_ber->prvnum_top; i++)
    rani_updatuj_prvek_matice_vnitrek(p_ber->p_prv_lev_top[i]);
  for (i = 0; i < p_ber->prvnum_low; i++)
    rani_updatuj_prvek_matice_vnitrek(p_ber->p_prv_lev_low[i]);
}

/*
  Postup update:

  1. Nastaveni mailu
  2. update animaci v lin seznamu
  3. update top-low objektu
  4. zavolani end funkci vsech animaci
*/

inline void rani_updatuj_1(GK_ANIM * p_gk)
{
  GKA_MATICE *p_mat;
  dword next_time = p_ber->TimeEndLastFrame;
  int m;                        //mesh;
  GLMATRIX tmp;

  /* Animace se updatuji kazdy frame
   */
  if (p_gk->flag & GK_AKTIVNI) {
    /* Test na konec animace
     */
    if ((p_gk->konec = (next_time > p_gk->time_stop))) {
      if (p_gk->flag & GK_LOOP) {
        calc_time_loop(next_time, p_gk->start, &p_gk->time_start,
          &p_gk->time_stop, &p_gk->time_delka, &p_gk->time);
        // zruseni flagu konec
        p_gk->konec = 0;
        if (p_gk->p_flag)
          *(p_gk->p_flag) = 0;
      }
      else {
        if (p_gk->next_ah != K_CHYBA) {
          p_gk->next_start += next_time - p_gk->time_stop;
          p_gk->time = p_gk->time_delka;
          if (p_gk->p_flag)
            *(p_gk->p_flag) = ANIM_FLAG_KONEC;
        }
        else {
          p_gk->time = p_gk->time_delka;
          if (p_gk->p_flag)
            *(p_gk->p_flag) = ANIM_FLAG_KONEC;
        }
      }
    }
    else {
      p_gk->time = calc_time_akt(next_time, p_gk->time_start);
      if (p_gk->p_flag)
        *(p_gk->p_flag) =
          ftoi((p_gk->time / (float) p_gk->time_delka) * 100.0f);
    }

    /* Vypocitej animaci
     */
    key_sim_to_matrix_param(p_gk->p_sim, p_gk->time, &tmp, &p_gk->p, &p_gk->q,
      &p_gk->s);

    for (m = 0; m < p_gk->matic; m++) {
      p_mat = p_gk->p_mat + m;

      /* Spocitani animacni matice
       */
      if (p_mat->p_mat && p_mat->flagy & MESH_USE_MATRIX) {
        if (p_mat->flagy & MESH_MULT_MATRIX) {
          if (p_mat->flagy & MESH_LOCAL_MATRIX)
            mat_mult(&tmp, p_mat->p_mat, p_mat->p_mat);
          else
            mat_mult(p_mat->p_mat, &tmp, p_mat->p_mat);
        }
        else {
          *(p_mat->p_mat) = tmp;
        }
      }

      /* Zapise flag notify, pokud je pozadovan
       */
      if (p_mat->p_notify)
        *(p_mat->p_notify) = TRUE;

      if (p_gk->konec) {
        /* volani end_funkce pokud je definovana
         */
        if (p_mat->funkce_end)
          p_mat->funkce_end(p_mat->fce_param);
      }
      else {
        /* Volani frame-funkce pokud je definovana
         */
        if (p_mat->funkce_frame)
          p_mat->funkce_frame(p_mat->fce_param);
      }
    }
  }
}


/* Zavolani end-funkci podle mistra kofolace...
*/
inline void rani_updatuj_2(GK_ANIM * p_gk)
{
  if (p_gk->flag & GK_AKTIVNI && p_gk->konec) {
    p_gk->flag &= ~GK_AKTIVNI;
    if (p_gk->p_endfce)
      p_gk->p_endfce(p_gk->param, p_gk->param2, p_gk->p_param);
    if (p_gk->next_ah != K_CHYBA) {
      rani_go(p_gk->next_ah, p_gk->next_flag, p_gk->next_start,
        p_gk->next_stop);
      rani_updatuj_1((GK_ANIM *) p_gk->next_ah);
    }
    if (p_gk->flag & GK_REMOVE) {
      rani_zrus((int) p_gk);
    }
  }
}


/* Samotny animacni motor
*/
void rani_updatuj(G_KONFIG * p_ber)
{
  GK_ANIM *p_gk;

  /* Vynulovani poctu low-prvku a top-prvku na mape + animacni mail
   */
  p_ber->prvnum_top = 0;
  p_ber->prvnum_low = 0;
  p_ber->anim_mail++;

  /* Update animaci
   */
  p_gk = p_ber->p_gkanim;
  while (p_gk) {
    if (p_gk->flag & GK_AKTIVNI) {
      rani_updatuj_1(p_gk);
    }
    p_gk = p_gk->p_next;
  }

  /* Updatuji se top-low objekty
   */
  rani_updatuj_top_objekty();

  /* Volani end-funkci
   */
  p_gk = p_ber->p_gkanim;
  while (p_gk) {
    if (p_gk->flag & GK_AKTIVNI && p_gk->konec) {
      rani_updatuj_2(p_gk);
    }
    p_gk = p_gk->p_next;
  }
}

/* -------------------------------------
     Animace meshe
   -------------------------------------
*/
/* Rozbehne animaci meshe
*/
void lani_go(G_KONFIG * p_ber, GAME_MESH_OLD * p_mesh)
{
  GAME_MESH_ANIMACE *p_sinfo = &p_mesh->siminfo;
  SIMPLE_TRACK_INFO *p_sim = p_sinfo->p_sim[0];
  int i;

  calc_time_bez_stop(p_sim->keynum, p_ber->TimeEndLastFrame,
    p_sinfo->start[0], p_sinfo->stop[0],
    &p_sinfo->time_start, &p_sinfo->time_stop, &p_sinfo->time_delka);

  p_sinfo->flag[0] |= GK_AKTIVNI;

  if (p_sinfo->odkaz == K_CHYBA) {
    for (i = 0; i < p_ber->lokalnum; i++) {
      if (!p_ber->p_lokal[i]) {
        goto nasel;
      }
    }
    i = p_ber->lokalnum++;
  nasel:;
    p_ber->p_lokal[i] = p_mesh;
    p_sinfo->odkaz = i;
  }
}

/* Strepe animace - prvni na nultou, pokud je aktivni
*/
inline void lani_setrep(GAME_MESH_ANIMACE * p_anim)
{
  int i, j;

  for (i = 0, j = 1; j < LANI_FRONTA; j++, i++) {
    if (p_anim->akt[j] != K_CHYBA) {
      p_anim->akt[i] = p_anim->akt[j];
      p_anim->flag[i] = p_anim->flag[j];
      p_anim->p_flag[i] = p_anim->p_flag[j];
      p_anim->p_sim[i] = p_anim->p_sim[j];
      p_anim->start[i] = p_anim->start[j];
      p_anim->stop[i] = p_anim->stop[j];
      p_anim->akt[j] = K_CHYBA;
    }
    else {
      break;
    }
  }
}

/* Nastavi aktivni lokalni animaci nebo K_CHYBA pro stop animace
*/
MeshHandle lani_set(MeshHandle mh, int poradi, int c_anim, int *p_flag,
  int flag, int start, int stop)
{
  GAME_MESH_OLD *p_mesh = p_ber->p_mesh[p_ber->p_prv_lev[mh]->mesh];

  if (!p_mesh || poradi >= LANI_FRONTA) {
    kprintfe(TRUE, "K_CHYBA - FILE %s LINE %d", __FILE__, __LINE__);
    return (K_CHYBA);
  }

  // Pokud je cislo animace mimo rozsah - vezmi prvni animaci
  if (p_mesh->simnum <= c_anim)
    c_anim = 0;

  if ((p_mesh->siminfo.akt[poradi] = c_anim) != K_CHYBA) {
    p_mesh->siminfo.flag[poradi] = flag;
    p_mesh->siminfo.p_flag[poradi] = p_flag;
    p_mesh->siminfo.p_sim[poradi] = p_mesh->p_sim + c_anim;
    p_mesh->siminfo.start[poradi] = start * SIM_KONSTI;
    p_mesh->siminfo.stop[poradi] = stop * SIM_KONSTI;
    if (!poradi)
      lani_go(p_ber, p_mesh);
    return (mh);
  }
  else {
    if (!poradi && p_mesh->siminfo.odkaz != K_CHYBA) {
      p_ber->p_lokal[p_mesh->siminfo.odkaz] = NULL;
      p_mesh->siminfo.odkaz = K_CHYBA;
    }
  }

  return (mh);
}

/* Prejde okamzite na dalsi animaci v poradi jinak skonci
*/
MeshHandle lani_next(MeshHandle mh)
{
  GAME_MESH_OLD *p_mesh = p_ber->p_mesh[p_ber->p_prv_lev[mh]->mesh];

  if (!p_mesh) {
    kprintfe(TRUE, "K_CHYBA - FILE %s LINE %d", __FILE__, __LINE__);
    return (K_CHYBA);
  }

  lani_setrep(&p_mesh->siminfo);
  if (p_mesh->siminfo.akt[0] != K_CHYBA) {
    lani_go(p_ber, p_mesh);
  }

  return (mh);
}

/* Vraci pocet bezicich animaci meshe
   0 - zadna (mesh se neanimuje)
   1 - jedna bezici animace
   2 - jedna bezici animace + jedna v zasobe
*/
int lani_get(MeshHandle mh)
{
  GAME_MESH_OLD *p_mesh = p_ber->p_mesh[p_ber->p_prv_lev[mh]->mesh];

  if (!p_mesh) {
    kprintfe(TRUE, "K_CHYBA - FILE %s LINE %d", __FILE__, __LINE__);
    return (K_CHYBA);
  }

  if (p_mesh->siminfo.akt[0] == K_CHYBA)
    return (0);
  else {
    return ((p_mesh->siminfo.akt[1] == K_CHYBA) ? 1 : 2);
  }
}

/* Smaze frontu animaci a vsechny je zastavi
*/
MeshHandle lani_smaz_frontu(MeshHandle mh)
{
  GAME_MESH_OLD *p_mesh = p_ber->p_mesh[p_ber->p_prv_lev[mh]->mesh];
  int i;

  if (!p_mesh) {
    kprintfe(TRUE, "K_CHYBA - FILE %s LINE %d", __FILE__, __LINE__);
    return (K_CHYBA);
  }

  for (i = 0; i < LANI_FRONTA; i++)
    p_mesh->siminfo.akt[i] = K_CHYBA;

  if (p_mesh->siminfo.odkaz != K_CHYBA) {
    p_ber->p_lokal[p_mesh->siminfo.odkaz] = NULL;
    p_mesh->siminfo.odkaz = K_CHYBA;
  }

  return (mh);
}

/* Udelat seznam key-frame hierarchickych animaci a ty animovat pred objektama
*/
void lani_updatuj(G_KONFIG * p_ber)
{
  GAME_MESH_ANIMACE *p_sinfo;
  GAME_MESH_OLD *p_mesh;
  GLMATRIX tmp1;
  dword next_time = p_ber->TimeEndLastFrame;
  int i, konec;
  int animovat;

  // animace podle lokalu
  for (i = 0; i < p_ber->lokalnum; i++) {
    if ((p_mesh = p_ber->p_lokal[i])) {
      animovat = p_ber->conf_animace_okoli
        || (p_mesh->p_data->kflag) & KONT_PRVEK;
      p_sinfo = &p_mesh->siminfo;
      if (p_sinfo->akt[0] == K_CHYBA) {
        lani_setrep(p_sinfo);
        if (p_sinfo->akt[0] != K_CHYBA) {
          lani_go(p_ber, p_mesh);
        }
        else {
          p_ber->p_lokal[i] = NULL;
          p_sinfo->odkaz = K_CHYBA;
        }
      }
      else {
        if ((konec = (next_time > p_sinfo->time_stop))) {       // chyby mi tady osetreni loop flagu
          if (p_sinfo->flag[0] & GK_LOOP) {
            calc_time_loop(next_time, p_sinfo->start[0],
              &p_sinfo->time_start,
              &p_sinfo->time_stop, &p_sinfo->time_delka, &p_sinfo->time);
            if (p_sinfo->p_flag[0])
              *(p_sinfo->p_flag[0]) = 0;
          }
          else {
            p_sinfo->time = p_sinfo->time_delka;
            if (p_sinfo->p_flag[0])
              *(p_sinfo->p_flag[0]) = ANIM_FLAG_KONEC;
            p_sinfo->akt[0] = K_CHYBA;
          }
        }
        else {
          p_sinfo->time = calc_time_akt(next_time, p_sinfo->time_start);
          if (p_sinfo->p_flag[0])
            *(p_sinfo->p_flag[0]) =
              ftoi((p_sinfo->time / (float) p_sinfo->time_delka) * 100.0f);
        }

        p_mesh->p_sim_aktivni = p_sinfo->p_sim[0];
        p_mesh->sim_aktivni_time = p_sinfo->time;

        if (animovat) {
          if ((p_mesh->p_data->kflag & (KONT_VIDITELNY | KONT_VIDITELNY_ZRC))
            && !(p_mesh->sim_flag & GAME_MESH_RANI)) {
            mat_mult(&p_mesh->world, &p_mesh->m, &tmp1);
            key_sim_animuj_strom(p_sinfo->p_sim[0], p_sinfo->time, &tmp1);
            key_mesh_recalc_normal_anim(p_mesh);
            p_mesh->p_data->kflag |= KONT_POHYB;
#ifdef DEBUG_MOD
            p_ber->debug.lani_vid = TRUE;
#endif
          }
          else {
#ifdef DEBUG_MOD
            p_ber->debug.lani_vid = FALSE;
#endif
          }
        }
      }
    }
  }
}


/********************************************************
     Zavislosti mezi meshama
 ********************************************************
*/

/* 
  Vazani meshu na sebe
*/
int kom_svaz_meshe(MeshHandle m_top, MeshHandle m_low)
{
  PRVEK_LEVELU_GAME *p_top = p_ber->p_prv_lev[m_top];
  PRVEK_LEVELU_GAME *p_low = p_ber->p_prv_lev[m_low];

  assert(p_top && p_low);

  if (p_top->flag & PRV_LOW || p_low->flag != 0) {
    kprintfe(TRUE, "K_CHYBA - FILE %s LINE %d", __FILE__, __LINE__);
    return (K_CHYBA);
  }

  p_top->flag |= PRV_TOP;
  p_low->flag |= PRV_LOW;

  // insert first
  p_low->p_next_low = p_top->p_next_low;
  p_low->p_prev_low = p_top;

  p_top->p_next_low = p_low;
  p_top->p_prev_low = NULL;

  return (TRUE);
}

/*
  Rozvazani zavislosti 2 meshu
*/
int kom_rozvaz_mesh(MeshHandle m_low)
{
  PRVEK_LEVELU_GAME *p_low = p_ber->p_prv_lev[m_low];

  assert(p_low);

  // Test - low objekt neni low objekt
  if (!(p_low->flag & PRV_LOW)) {
    kprintfe(TRUE, "K_CHYBA - FILE %s LINE %d", __FILE__, __LINE__);
    return (K_CHYBA);
  }

  // Provazani prvku smerem dopredu
  p_low->p_prev_low->p_next_low = p_low->p_next_low;

  // Provazani zpet
  if (p_low->p_next_low) {
    p_low->p_next_low->p_prev_low = p_low->p_prev_low;
  }
  else {
    if (p_low->p_prev_low->flag & PRV_TOP)
      p_low->p_prev_low->flag &= ~PRV_TOP;
  }

  // Odstraneni low-flagu
  p_low->flag &= ~PRV_LOW;

  // Odstraneni vazeb
  p_low->p_next_low = FALSE;
  p_low->p_prev_low = FALSE;

  // reanimuj jako konecny prvek
  rani_updatuj_prvek_matice_vnitrek_end_top(p_low);

  return (TRUE);
}

TextHandle tani_go(MatHandle mh, int textura, int animace, int *p_flag,
  int flag, int start, int stop)
{
  GK_TEXTANIM *p_tani;
  ANIM_TEXT *p_track;

  if (p_ber->p_mat[mh]) {
    p_track = p_ber->p_mat[mh]->p_atext[textura][animace];

    if (!p_track) {
      kprintfe(TRUE, "K_CHYBA - FILE %s LINE %d", __FILE__, __LINE__);
      return (K_CHYBA);
    }

    p_tani = (GK_TEXTANIM *) mmalloc(sizeof(p_tani[0]));

    p_tani->material = mh;
    p_tani->p_track = p_track;
    p_tani->p_mat = p_ber->p_mat[mh]->ttext + textura;

    p_tani->p_flag = p_flag;
    p_tani->flag = p_track->flag | flag;

    calc_time_end(p_tani->p_track->endtime, p_ber->TimeEndLastFrame,
      start, stop,
      &p_tani->start,
      &p_tani->stop,
      &p_tani->time_start, &p_tani->time_stop, &p_tani->time_delka);

    p_tani->p_next = p_ber->p_textanim;
    p_tani->p_prev = NULL;
    if (p_tani->p_next) {
      p_tani->p_next->p_prev = p_tani;
    }
    p_ber->p_textanim = p_tani;

    return ((int) p_tani);

  }
  //kprintfe(TRUE,"K_CHYBA - FILE %s LINE %d",__FILE__,__LINE__);
  return (K_CHYBA);
}

/*
  Animace texture materialu  
*/
void tani_init_scena(G_KONFIG * p_ber)
{
  EDIT_MATERIAL *p_mat;
  int i, j, k;

  for (i = 0; i < MAX_CELKEM_MATERIALU; i++) {
    p_mat = p_ber->p_mat[i];
    if (p_mat) {
      for (k = 0; k < MAT_TEXTUR; k++) {
        for (j = 0; j < p_mat->atextnum[k]; j++) {
          if (p_mat->p_atext[k][j]
            && p_mat->p_atext[k][j]->flag & ANIM_TEXT_DEFAULT) {
            tani_go(i, k, j, NULL, FALSE, 0, 0);
            break;
          }
        }
      }
    }
  }
}

/*
  Texture animace
*/
void tani_updatuj(G_KONFIG * p_ber)
{
  dword next_time = p_ber->TimeEndLastFrame;
  GK_TEXTANIM *p_tani = p_ber->p_textanim;

  while (p_tani) {
    if (next_time > p_tani->time_stop) {
      if (p_tani->flag & GK_LOOP) {
        calc_time_loop(next_time, p_tani->start,
          &p_tani->time_start,
          &p_tani->time_stop, &p_tani->time_delka, &p_tani->time);
        // zruseni flagu konec
        if (p_tani->p_flag)
          *(p_tani->p_flag) = 0;
      }
      else {
        p_tani->time = p_tani->time_delka;
        if (p_tani->p_flag)
          *(p_tani->p_flag) = ANIM_FLAG_KONEC;
      }
    }
    else {
      p_tani->time = calc_time_akt(next_time, p_tani->time_start);
      if (p_tani->p_flag)
        *(p_tani->p_flag) =
          ftoi((p_tani->time / (float) p_tani->time_delka) * 100.0f);
    }

    //p_tani->time -> aktualni cas animace    
    key_sim_to_text_matrix(p_tani->p_track, p_tani->time, p_tani->p_mat);

    p_tani = p_tani->p_next;
  }
}

void tani_zrus(GK_TEXTANIM ** p_tmp)
{
  GK_TEXTANIM *p_tani = *p_tmp;
  GK_TEXTANIM *p_next;

  while (p_tani) {
    p_next = p_tani->p_next;
    null_free((void **) &p_tani);
    p_tani = p_next;
  }
  if (p_tmp) {
    *p_tmp = NULL;
  }

}

/* Joint-animace - chapadelnici  
 -> vraci pocet objektu s animacema a pocet animaci celkem
*/
ExMeshHandle mesh_to_ex_mesh(MeshHandle mh)
{
  return (p_ber->p_prv_lev[mh]->mesh);
}

/* Vrati pocet objektu ktere maji mesh-animace
*/
int chani_mesh_cti_objekty(ExMeshHandle mh)
{
  GAME_MESH_OLD *p_mesh = p_ber->p_mesh[mh];
  int i, anim = 0;

  if (p_mesh->p_joint_anim) {
    for (i = 0; i < p_mesh->objektu; i++) {
      if (p_mesh->p_joint_anim[i])
        anim++;
    }
    return (anim);
  }
  else
    return (0);
}

/* Cte objekt v poradi s animacema
*/
int chani_mesh_cti_chapadelniky(ExMeshHandle mh, int objekt,
  ChapadloHandle ** p_chap, int *p_chapadel)
{
  GAME_MESH_OLD *p_mesh = p_ber->p_mesh[mh];
  ChapadloHandle *p_handle;
  JOINT_ANIMACE *p_anim;
  int i, chapadel, anim = 0;

  if (p_mesh->p_joint_anim) {
    for (i = 0; i < p_mesh->objektu; i++) {
      if (p_mesh->p_joint_anim[i]) {
        if (objekt == anim) {
          chapadel = 0;

          p_anim = p_mesh->p_joint_anim[i];
          while (p_anim) {
            chapadel++;
            p_anim = p_anim->p_next;
          }

          *p_chapadel = chapadel;
          if (chapadel) {
            *p_chap = p_handle =
              (ChapadloHandle *) mmalloc(sizeof(p_handle[0]) * chapadel);
            p_anim = p_mesh->p_joint_anim[i];
            chapadel = 0;
            while (p_anim) {
              p_handle[chapadel++] = (ChapadloHandle) p_anim;
              p_anim = p_anim->p_next;
            }
          }

          return (chapadel);
        }
        anim++;
      }
    }
    return (anim);
  }
  else
    return (0);
}

char *chani_cti_jmeno(ChapadloHandle chh)
{
  JOINT_ANIMACE *p_ani = (JOINT_ANIMACE *) chh;

  return (p_ani->jmeno);
}

// zaradim to do seznamu animaci na chapadlo
ChapadloHandle chani_zarad(ChapadloHandle chh)
{
  JOINT_ANIMACE *p_ani = (JOINT_ANIMACE *) chh;

  p_ani->p_game_next = p_ber->p_chap;
  p_ber->p_chap = p_ani;
  p_ani->p_game_prev = NULL;
  if (p_ani->p_game_next) {
    p_ani->p_game_next->p_game_prev = p_ani;
  }

  return (chh);
}

/* Spusti animaci chapadelniha - vrati handle na bezici animaci
*/
ChapadloHandle chani_go(ChapadloHandle chh, int *p_flag, int flag, int start,
  int stop)
{
  JOINT_ANIMACE *p_ani = (JOINT_ANIMACE *) chh;

  calc_time(p_ani->framenum, p_ber->TimeEndLastFrame, start, stop,
    &p_ani->start, &p_ani->stop, &p_ani->time_start, &p_ani->time_stop,
    &p_ani->time_delka);

  p_ani->flag |= (GK_AKTIVNI | flag);
  p_ani->konec = FALSE;

  return (chh);
}

/* Stopne animaci chapadelnika
*/
ChapadloHandle chani_stop(ChapadloHandle chh)
{
  JOINT_ANIMACE *p_ani = (JOINT_ANIMACE *) chh;

  p_ani->flag = 0;
  return (chh);
}

/* Zrusi animaci
*/
ChapadloHandle chani_zrus(ChapadloHandle chh)
{
  JOINT_ANIMACE *p_ani = (JOINT_ANIMACE *) chh;

  if (!p_ani->p_game_prev) {
    p_ber->p_chap = p_ani->p_game_next;
    if (p_ber->p_chap) {
      p_ber->p_chap->p_game_prev = NULL;
    }
  }
  else {
    p_ani->p_game_prev = p_ani->p_game_next;
    if (p_ani->p_game_next)
      p_ani->p_game_next->p_game_prev = p_ani->p_game_prev;
  }

  return (chh);
}

void chani_zrus_all(G_KONFIG * p_ber)
{
  p_ber->p_chap = NULL;
  return;
}

void chani_updatuj(G_KONFIG * p_ber)
{
  int akt = 0;
  JOINT_ANIMACE *p_ani = p_ber->p_chap;
  dword next_time = p_ber->TimeEndLastFrame;
  int anim = 0;

#ifdef DEBUG_MOD
  p_ber->debug.vertex_obj = 0;
#endif

  while (p_ani) {
    if (next_time > p_ani->time_stop) {
      if (p_ani->flag & GK_LOOP) {
        calc_time_loop(next_time, p_ani->start,
          &p_ani->time_start,
          &p_ani->time_stop, &p_ani->time_delka, &p_ani->time);
        // zruseni flagu konec
        if (p_ani->p_flag)
          *(p_ani->p_flag) = 0;
      }
      else {
        p_ani->time = p_ani->time_delka;
        if (p_ani->p_flag)
          *(p_ani->p_flag) = ANIM_FLAG_KONEC;
      }
    }
    else {
      p_ani->time = calc_time_akt(next_time, p_ani->time_start);
      if (p_ani->p_flag)
        *(p_ani->p_flag) =
          ftoi((p_ani->time / (float) p_ani->time_delka) * 100.0f);
    }

    if (p_ber->conf_animace_okoli) {
      if ((*p_ani->p_kkflag) & (KONT_VIDITELNY | KONT_VIDITELNY_ZRC) &&
        (*p_ani->p_koflag) & (KONT_VIDITELNY | KONT_VIDITELNY_ZRC)) {
        key_kosti_stream_animuj(p_ani);
        (*p_ani->p_kkflag) |= KONT_POHYB;
#ifdef DEBUG_MOD
        p_ber->debug.vertex_obj++;
#endif
      }
    }

    p_ani = p_ani->p_game_next;
  }
}
