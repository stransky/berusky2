/* Keyframovy animacni modul
*/

#ifndef __KEYFRAME_H
#define __KEYFRAME_H

#define TRIDA_A     1           // incoming case
#define TRIDA_B    -1           // outgoing case
#define TRIDA_LOOP  2           // loop animace

void dbg_print_matrix(GLMATRIX * m);
void dbg_print_track_uhel(TRACK_INFO * p_track);
void dbg_print_track_quaternion(TRACK_INFO * p_track);
void dbg_print_track_abs(TRACK_INFO * p_track);
void dbg_print_track_abs_uhel(TRACK_INFO * p_track);

void kprint_matrix(GLMATRIX * m);

void key_pivot_sub(EDIT_OBJEKT * p_obj, BOD * p_pivot);
void key_tri_to_matrix_zero(GLMATRIX * p_m, HIERARCHY_TRACK_INFO * p_track);
void key_root_to_zero(HIERARCHY_ROOT * p_root);
void key_root_to_matrix(HIERARCHY_ROOT * p_root, int time, GLMATRIX * p_otec);
TRACK_INFO *key_track_vyrob(int poskey, int rotkey, int scalekey);
HIERARCHY_ROOT *key_root_vyrob_indir(EDIT_KONTEJNER * p_kont,
  HIERARCHY_ROOT * p_root);
HIERARCHY_TRACK_INFO *key_tri_vyrob_indir(HIERARCHY_TRACK_INFO * p_hir,
  int poskey, int rotkey, int scalekey);
HIERARCHY_TRACK_INFO *key_tri_vyrob_un_indir(void);
HIERARCHY_TRACK_INFO *key_tri_vyrob(int poskey, int rotkey, int scalekey);
HIERARCHY_TRACK_INFO *key_track_to_tri(TRACK_INFO * p_track);
BOD *key_position_tangent(HIERARCHY_TRACK_INFO * p_track, int kn, int trida,
  BOD * p_tn);
inline QUAT *key_rotkey_to_quat(ROTKEY * p_rot, QUAT * p_q);
void key_zrus_track(TRACK_INFO ** p_track);
void key_tri_child_pridej(HIERARCHY_TRACK_INFO * p_root,
  HIERARCHY_TRACK_INFO * p_child);
void key_root_child_pridej(HIERARCHY_ROOT * p_root,
  HIERARCHY_TRACK_INFO * p_child);
void key_root_to_root_indir(EDIT_KONTEJNER * p_kont, HIERARCHY_ROOT * p_cil,
  HIERARCHY_ROOT * p_src);
void key_root_to_sim_indir(EDIT_KONTEJNER * p_kont, HIERARCHY_SIM * p_sim,
  HIERARCHY_ROOT * p_root, int float_scale);
void key_kontejner_reanimuj(EDIT_KONTEJNER * p_kont, HIERARCHY_ROOT * p_root);
void key_kontejner_reanimuj_matrix(EDIT_KONTEJNER * p_kont, GLMATRIX * p_m);
void key_sim_to_sim_indir(SIMPLE_TRACK_INFO * p_dest,
  SIMPLE_TRACK_INFO * p_src);
void key_sim_animuj_strom(SIMPLE_TRACK_INFO * p_sim, int time,
  GLMATRIX * p_otec);
SIMPLE_TRACK_INFO *key_sim_vyrob_indir(SIMPLE_TRACK_INFO * p_sim,
  int framenum);
SIMPLE_TRACK_INFO *key_tri_to_sim_indir(HIERARCHY_TRACK_INFO * p_track,
  SIMPLE_TRACK_INFO * p_sim, int loop, int float_scale);
void key_sim_uloz(SIMPLE_TRACK_INFO * p_sim, char *p_file, char *p_dir);
int key_sim_nahraj(APAK_HANDLE * pHandle, SIMPLE_TRACK_INFO * p_sim,
  char *p_file, char *p_dir, int linear);
GLMATRIX *key_sim_to_matrix(SIMPLE_TRACK_INFO * p_sim, int time,
  GLMATRIX * p_m);
GLMATRIX *key_sim_to_matrix_param(SIMPLE_TRACK_INFO * p_sim, int time,
  GLMATRIX * p_m, BOD * p_pos, QUAT * p_quat, BOD * p_scale);
void key_sim_root_to_matrix(EDIT_KONTEJNER * p_kont, HIERARCHY_SIM * p_sim,
  int time, GLMATRIX * p_m);
void key_mesh_reanimuj(GAME_MESH_OLD * p_mesh, int time, GLMATRIX * p_top);
void key_kan_to_kamera(KAMERA * p_kam, KAMERA_TRACK_INFO * p_track, int time);
int key_je_frame_klic(KEY_POINT_BRS * p_list, int klicu, dword frame);
int key_je_time_klic(KEY_POINT_BRS * p_list, int klicu, dword time);
void key_kontejner_retransformuj(EDIT_KONTEJNER * p_kont_top);
void key_kontejner_recalc_normal_anim(EDIT_KONTEJNER * p_kont);
void key_kont_updatuj_flagy(EDIT_KONTEJNER * p_kont);
void key_sim_animuj_strom_pivoty(SIMPLE_TRACK_INFO * p_sim, int time,
  GLMATRIX * p_otec);
void key_kontejner_sim_reanimuj_pivoty(EDIT_KONTEJNER * p_kont);

int key_track_klic_vloz(byte ** p_hodnoty, KEY_POINT_BRS ** p_skeys,
  int keynum, int size_of_hodnota, byte * p_new_hodnota,
  KEY_POINT_BRS * p_new_keys);
int key_track_klic_vloz_auto(byte ** p_hodnoty, KEY_POINT_BRS ** p_skeys,
  int keynum, int size_of_hodnota, byte * p_new_hodnota,
  KEY_POINT_BRS * p_new_keys);
int key_track_klic_smaz(byte ** p_hodnoty, KEY_POINT_BRS ** p_skeys,
  int *p_keynum, int size_of_hodnota, dword del_time);

int key_kamera_vyrob_indir(KAMERA_TRACK_INFO * p_track, int pos_keys,
  int trg_keys, int roll_keys, int fov_keys, int quat_keys);
int key_kamera_uloz(KAMERA_TRACK_INFO * p_track, char *p_jmeno_kamery,
  char *p_jmeno_anim, char *p_file, char *p_dir);
int key_kamera_nahraj(KAMERA_TRACK_INFO * p_track, char *p_jmeno_kamery,
  char *p_jmeno_anim, APAK_HANDLE * pHandle, char *p_file, char *p_dir);
void key_kamera_zrus_indir(KAMERA_TRACK_INFO * p_track);

int key_kamera_track_cti(KAMERA_TRACK_INFO * p_track, BOD * p_p, BOD * p_t,
  float *p_roll, float *p_fov);

float key_track_interpolace_float(float *p_vysl, float *p_body,
  KEY_POINT_BRS * p_keys, int time, int endtime, int keynum, int loop);
BOD *key_track_interpolace_bod(BOD * p_vysl, BOD * p_body,
  KEY_POINT_BRS * p_keys, int time, int endtime, int keynum, int loop);
QUAT *key_track_interpolace_quat(QUAT * p_quat, QUAT * p_at,
  KEY_POINT_BRS * p_rkeys, int time, int endtime, int keynum, int loop);
WBOD *key_track_interpolace_wbod(WBOD * p_vysl, WBOD * p_body,
  KEY_POINT_BRS * p_keys, int time, int endtime, int keynum, int loop);

float key_track_interpolace_float_linear(float *p_vysl, float *p_body,
  KEY_POINT_BRS * p_keys, int time, int endtime, int keynum, int loop);
BOD *key_track_interpolace_bod_linear(BOD * p_vysl, BOD * p_body,
  KEY_POINT_BRS * p_keys, int time, int endtime, int keynum, int loop);
QUAT *key_track_interpolace_quat_linear(QUAT * p_quat, QUAT * p_at,
  KEY_POINT_BRS * p_rkeys, int time, int endtime, int keynum, int loop);
WBOD *key_track_interpolace_wbod_linear(WBOD * p_vysl, WBOD * p_body,
  KEY_POINT_BRS * p_keys, int time, int endtime, int keynum, int loop);

ANIM_TEXT *key_vyrob_material_animace(int poskey, int rotkey, int scalekey,
  int pivotkey);
ANIM_TEXT *key_kopiruj_material_animace(ANIM_TEXT * p_src);
void key_zrus_material_animace(ANIM_TEXT ** p_mat);

void key_sim_to_text_matrix(ANIM_TEXT * p_text, int time, GLMATRIX * p_ttext);
void key_mat_to_text_matrix(GLMATRIX * p_ttext, BOD * p_pos, float uhel,
  BOD * p_scale, BOD * p_pivot);
ANIM_TEXT *key_text_nahraj(char *p_file, char *p_dir);
int key_text_uloz(ANIM_TEXT * p_track, char *p_file, char *p_dir);

void key_sim_zrus_vnitrek(SIMPLE_TRACK_INFO * p_sim);
void key_sim_root_zrus(HIERARCHY_SIM * p_sim);
void key_sim_zrus(SIMPLE_TRACK_INFO * p_sim);

void key_tri_calc_absolutne(TRACK_INFO * p_track, QUAT * p_puvodni);

void key_sim_root_to_sim_root_indir(EDIT_KONTEJNER * p_kont,
  HIERARCHY_SIM * p_cil, HIERARCHY_SIM * p_src);
void key_sim_dopln_matrix_kont(EDIT_KONTEJNER * p_kont,
  HIERARCHY_SIM * p_src);
void key_sim_root_to_sim_indir(HIERARCHY_SIM * p_root,
  SIMPLE_TRACK_INFO * p_sim);

void key_kontejner_sim_reanimuj(EDIT_KONTEJNER * p_kont);

void key_sim_dopln_matrix_mesh(GAME_MESH_OLD * p_mesh,
  SIMPLE_TRACK_INFO * p_sim);
int key_sim_nahraj_extended(EDIT_KONTEJNER * p_kont, int cislo_anim,
  char *p_file, char *p_dir);
void key_sim_root_vloz_pivoty_do_animace(EDIT_KONTEJNER * p_kont,
  HIERARCHY_SIM * p_root);
void key_sim_root_vloz_pivoty_do_animace_kont(EDIT_KONTEJNER * p_kont);

void key_mesh_transformuj_obalky(GAME_MESH_OLD * p_mesh, GLMATRIX * p_mat);
void key_mesh_transformuj_obalky_keyframe(GAME_MESH_OLD * p_mesh,
  GLMATRIX * p_mat);
//void        key_mesh_calc_global_obalku(GAME_MESH_OLD *p_mesh);
inline void key_mesh_recalc_normal_anim(GAME_MESH_OLD * p_mesh);

void key_sim_calc_absolutne(SIMPLE_TRACK_INFO * p_sim);

#define SIM_KONST  50.0f        //delici kostanta casu pro sim animace
#define SIM_KONSTI 50           //delici kostanta casu pro sim animace
#define ROOT_NODE  0xffff

// 20 framu/sec, 1 frame = 1 klic
// framenum = keynum
/*
  start      - cas ktery se ma preskocit
  stop       - cas konce (kdy se to konci)

  time_start - predpocitany absolutni cas startu animace
  time_stop  - predpocitani absolutni cas konce animace
  time_delka - predpocitana delka animace

  time       - relativni cas animace (0..delka)
*/

/* Prepocitava start/stop na casy start/stop animace
*/
inline void calc_time(dword framenum, dword next_time, dword start,
  dword stop, dword * pstart, dword * pstop, dword * time_start,
  dword * time_stop, dword * time_delka)
{
  *pstart = start;
  *pstop = stop;
  *time_delka = !stop ? (framenum - 1) * SIM_KONSTI : stop;
  *time_start = next_time - start;
  *time_stop = *time_start + *time_delka;
}

inline void calc_time_bez_stop(dword framenum, dword next_time, dword pstart,
  dword pstop, dword * p_time_start, dword * p_time_stop,
  dword * p_time_delka)
{
  *p_time_delka = !(pstop) ? (framenum - 1) * SIM_KONSTI : pstop;
  *p_time_start = next_time - pstart;
  *p_time_stop = *p_time_start + *p_time_delka;
}

inline void calc_time_end(dword endtime,
  dword next_time,
  dword start,
  dword stop,
  dword * p_pstart,
  dword * p_pstop,
  dword * p_time_start, dword * p_time_stop, dword * p_time_delka)
{
  *p_pstart = start * SIM_KONSTI;
  *p_pstop = stop * SIM_KONSTI;
  *p_time_delka = !(*p_pstop) ? endtime : *p_pstop;
  *p_time_start = next_time - *p_pstart;
  *p_time_stop = *p_time_start + *p_time_delka;
}

/*
  int next_time = p_ber->TimeEndLastFrame;
  if((konec = (next_time > p_kam->time_stop))) {  
   calc_time_loop(next_time,
                  p_kam->start,
                  p_kam->time_start,
                  p_kam->time_stop,
                  p_kam->time_delka,
                  p_kam->time);
*/
/* Vypocet casu do dalsi smycky
   start     - start cas animace
   next_time - 

*/
inline void calc_time_loop(dword next_time,
  dword start,
  dword * p_time_start,
  dword * p_time_stop, dword * p_time_delka, dword * p_time)
{
  *p_time_start = *p_time_stop - start;
  *p_time = next_time - *p_time_start;

  if (*p_time > 2 * (*p_time_delka)) {  // pokud to je 2x pretocene, zacneme znova
    *p_time_start = next_time - start;
    *p_time = next_time - *p_time_start;
  }

  *p_time_stop = *p_time_start + *p_time_delka;
}


/*
  *p_time_start = *p_time_stop  - start;
  *p_time_stop  = *p_time_start + *p_time_delka;
  *p_time       =  next_time    - *p_time_start;
*/

inline int calc_time_akt(int next_time, int time_start)
{
  return (next_time - time_start);
}

inline int calc_endtime(int framenum)
{
  return (framenum * SIM_KONSTI);
}

inline int calc_keynum(int endtime)
{
  return ((endtime / SIM_KONSTI) + 1);
}

inline int calc_framenum(int endtime)
{
  return ((endtime / SIM_KONSTI) + 1);
}

inline int endtime2framenum(int endtime)
{
  return ((endtime / SIM_KONSTI) + 1);
}

inline int time2frame(int time)
{
  return (time / SIM_KONSTI);
}

inline float frame2sec(int frame)
{
  return (frame * 0.05f);
}

inline int frame2time(int frame)
{
  return (frame * SIM_KONSTI);
}

inline float time2sec(int time)
{
  return (time / 1000.0f);
}

inline void key_mesh_recalc_normal_anim(GAME_MESH_OLD * p_mesh)
{
  BOD *p_vertex;
  BOD *p_vertex_norm;
  BOD n, up(0, 1, 0);
  int o, i, kflag, vr;
  float last, akt, num;

  kflag = p_mesh->p_data->kflag;

  if (kflag & KONT_NORMAL_ANIM) {
    vr = kflag & KONT_NORMAL_ANIM_POS;
    for (o = 0; o < p_mesh->objektu; o++) {
      if ((last = p_mesh->p_normal_scale_last[o]) == (akt =
          p_mesh->p_normal_scale[o]))
        continue;
      p_vertex = p_mesh->p_vertex_pos + p_mesh->objektstart[o];
      p_vertex_norm = p_mesh->p_vertex_norm + p_mesh->objektstart[o];
      num = p_mesh->objektnum[o];
      for (i = 0; i < num; i++, p_vertex++, p_vertex_norm++) {
        vektor_add(p_vertex, vektor_mult_skalar(p_vertex_norm, (last - akt),
            &n), p_vertex);
        if (vr) {
          vektor_add(p_vertex, vektor_mult_skalar(&up, (akt - last), &n),
            p_vertex);
        }
      }
      p_mesh->p_normal_scale_last[o] = akt;
      p_mesh->p_data->k2flag |= KONT2_UPDATE_POS;
    }
  }
}

JOINT_KEYS *key_kost_spline_vyrob(JOINT * p_joint);
void key_kosti_updatuj_vertexy(EDIT_OBJEKT * p_obj,
  JOINT_ANIMACE * p_animace);
void key_kosti_interpoluj(JOINT_ANIMACE * p_animace);
void key_kosti_animuj(JOINT_ANIMACE * p_animace);

void key_kosti_serad_klice(byte * p_hodnoty, KEY_POINT_BRS * p_skeys,
  int keynum, int size_of_hodnota);
void key_kosti_stream_animuj_rec(JOINT_ANIMACE * p_anim, JOINT * p_joint,
  GLMATRIX * p_top, int time, int keynum);

JOINT_ANIMACE *key_kosti_kopiruj(JOINT_ANIMACE * p_src);
JOINT_ANIMACE *key_kosti_kopiruj_animace(JOINT_ANIMACE * p_prvni);

JOINT *key_joint_vyrob(void);


inline void key_kosti_reanimuj_kont(EDIT_KONTEJNER * p_kont)
{
  EDIT_OBJEKT *p_obj;
  int i;

  if (p_kont->k2flag & KONT2_JOINT_ANIM) {
    for (i = 0; i < MAX_KONT_OBJEKTU; i++) {
      p_obj = p_kont->p_obj[i];
      if (p_obj && p_obj->p_joit_animace) {
        key_kosti_interpoluj(p_obj->p_joit_animace);
        key_kosti_updatuj_vertexy(p_obj, p_obj->p_joit_animace);
      }
    }
  }
}

void kprintf_matrix(GLMATRIX * m);

inline void key_kosti_stream_animuj(JOINT_ANIMACE * p_animace)
{
  if (p_animace) {
    key_kosti_stream_animuj_rec(p_animace, p_animace->p_child, NULL,
      p_animace->time, p_animace->framenum);
    *p_animace->p_k2flag |= KONT2_UPDATE_POS | KONT2_UPDATE_NORM;
  }
}

inline void key_track_quat_zkontroluj(QUAT * p_quat, int keynum)
{
  int i;

  for (i = 0; i < keynum; i++) {
    if (p_quat[i].x == 0 && p_quat[i].y == 0 && p_quat[i].z == 0
      && p_quat[i].w == 0)
      p_quat[i].w = 1;
  }
}

#endif
