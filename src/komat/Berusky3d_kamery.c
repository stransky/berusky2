/*
  berusky3d_animace.c
  Animacni modul k beruskam II

  posuny objektu
  kyframe animace objektu
  animacni hierachie
*/
#include "mss_on.h"
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "..\resource.h" 
#include "wzakl_typ.h"

#include "3d_all.h"

#include "..\\kofola\\Object.h"

#include "berusky_universal.h"
#include "berusky3d_castice.h"
#include "berusky3d.h"
#include "berusky3d_ini.h"
#include "berusky3d_load.h"
#include "berusky3d_render.h"
#include "berusky3d_animace.h"
#include "berusky3d_kofola_interface.h"
#include "berusky3d_kamery.h"

#include "dbgwnd.h"

extern G_KONFIG ber, *p_ber;

__inline int bod_mezi(BOD *p, BOD *t, BOD *i)
{
  BOD t1,i1;
  float vt1,vt2,vi1;

  vektor_sub(t,p,&t1);
  vektor_sub(i,p,&i1);

  vt1 = vektor_velikost(&t1);

  vektor_add(&t1,&i1,&i1);
  vektor_add(&t1,&t1,&t1);

  vt2 = vektor_velikost(&t1);
  vi1 = vektor_velikost(&i1);

  return(vi1 > vt1 && vi1 < vt2);
}

#define PRUHL_ALFA 0.35f

void ber_kamera_zpruhledni_mesh(GAME_MESH *p_mesh, BOD *p_cam, BOD *p_stred)
{
  BODRGBA *p_d,*p_dm;
  OBB     *p_obb;
  int      v,vn,o,in;
  int      pruhlednych;
  BOD      dir;
  
  vektor_sub(p_stred,p_cam,&dir);
  in = obb_intersect_line_dist(&p_mesh->obb_world,p_cam,&dir);
  if(in) {    
    p_mesh->p_data->kflag |=  KONT_DRAW_CAMERA;
    p_mesh->p_data->k2flag |= KONT2_UPDATE_DIFF;
    p_d = p_mesh->p_vertex_diff;
    p_dm = p_mesh->p_vertex_diff_material;
    p_obb = p_mesh->p_obb_world;
    
    pruhlednych = 0;
    for(o = 0; o < p_mesh->objektu; o++, p_obb++) {
      vn = p_mesh->objektnum[o];
      in = obb_intersect_line_dist(p_obb,p_cam,&dir);
      if(in && intersect_mesh_objekt(p_mesh,o,p_cam,&dir)) {
        pruhlednych++;
        for(v = 0; v < vn; v++, p_d++, p_dm++) {
          if(p_d->a > PRUHL_ALFA)
            p_d->a = PRUHL_ALFA;
        }
      } else {
        for(v = 0; v < vn; v++, p_d++, p_dm++) {
          p_d->a = p_dm->a;
        }
      }
    }
    if(!pruhlednych) { // Zrus pruhlednost - neni pruhledny zadny
      p_mesh->p_data->kflag &= ~KONT_DRAW_CAMERA;
    }
  } else {    
    p_mesh->p_data->kflag &= ~KONT_DRAW_CAMERA;
    if(p_mesh->p_data->m1flag&MAT_PRUHLEDNY || p_mesh->p_data->m2flag&MAT2_MASKA) {
      ber_reset_mesh(p_mesh->p_vertex_diff,p_mesh->p_vertex_diff_material,p_mesh->vertexnum);
      p_mesh->p_data->k2flag |= KONT2_UPDATE_DIFF;
    }    
  }
}

void ber_kamera_zpruhledni_poly(EDIT_MESH_POLY *p_poly, BOD *p_cam, BOD *p_stred)
{
  TEXT_KOORD *p_koord;
  BOD         dir;
  int         v,in;

  vektor_sub(p_stred,p_cam,&dir);
  in = obb_intersect_line_dist(&p_poly->obb,p_cam,&dir);
  if(in && intersect_poly(p_poly, p_cam, &dir)) {
    
    p_poly->kflag |= (KONT_UPLOAD|KONT_DRAW_CAMERA);

    p_koord = p_poly->p_koord;
    for(v = 0; v < p_poly->facenum; v++, p_koord++) {
      if(p_koord->da > PRUHL_ALFA)
        p_koord->da = PRUHL_ALFA;
    }
  } else {
    p_poly->kflag &= ~KONT_DRAW_CAMERA;
    if(p_poly->m1flag&MAT_PRUHLEDNY || p_poly->m2flag&MAT2_MASKA) {
      ber_reset_poly(p_poly->p_koord, p_poly->facenum);
      p_poly->k2flag |= KONT_UPLOAD;
    }
  }
}

/* viditelny objekt/poly
   pokud je kamera do nej kamera zariznuta
   pokud je na primce stred kamery/stred levelu
*/
void ber_kamera_zpruhledni_objekty(G_KONFIG *p_ber)
{
  EDIT_MESH_POLY *p_poly;
  GAME_MESH      *p_mesh;
  GLMATRIX       *p_inv = p_ber->p_invcam;
  int             mh;
  static BOD      lt = {FLT_MAX,FLT_MAX,FLT_MAX};
  BOD             t,p;
  int             pohyb_kamery = p_ber->kamera.zmena || p_ber->kamera.zmena_last;
  
  if(!p_ber->conf_zpruhlednovac)
    return;

  mh = gl_Get_Active_Beetle_Mesh();
  if(mh == K_CHYBA)
    return;
  
  t.x = p_ber->p_prv_lev[mh]->x;
  t.y = p_ber->p_prv_lev[mh]->y;
  t.z = p_ber->p_prv_lev[mh]->z;

  if(lt.x != t.x || lt.y != t.y || lt.z != t.z) {
    lt.x = t.x;
    lt.y = t.y;
    lt.z = t.z;
    pohyb_kamery = TRUE;
  }  

  p.x = p_ber->p_invcam->_41;
  p.y = p_ber->p_invcam->_42;
  p.z = p_ber->p_invcam->_43;

  /* Projedu seznam viditelnych meshu
  */
  ber_mesh_render_list_reset(p_ber);
  while(p_mesh = ber_mesh_render_list_next_flag(p_ber,KONT_VIDITELNY,KONT_PRVEK)) {
    if(p_mesh->p_vertex_diff && (pohyb_kamery || p_mesh->p_data->kflag&KONT_POHYB))
      ber_kamera_zpruhledni_mesh(p_mesh,&p,&t);
  }
  
  /* Projedu seznam viditelnych poly
  */  
  if(pohyb_kamery) {
    ber_poly_render_list_reset(p_ber);
    while(p_poly = ber_poly_render_list_next_flag(p_ber,KONT_VIDITELNY,KONT_PRVEK)) {
      ber_kamera_zpruhledni_poly(p_poly,&p,&t);
    }
  }
}

void ber_kamera_zpruhledni_objekty_reset(G_KONFIG *p_ber)
{
  EDIT_MESH_POLY *p_poly;
  GAME_MESH *p_mesh;
  int i,kflag;

  for(i = 0; i < p_ber->meshnum; i++) {
    p_mesh = p_ber->p_mesh[i];
    if(p_mesh && !(p_mesh->p_data->kflag&KONT_PRVEK)) {
      if((p_mesh->p_data->m1flag&MAT_PRUHLEDNY || p_mesh->p_data->m2flag&MAT2_MASKA) && p_mesh->p_vertex_diff) {
        ber_reset_mesh(p_mesh->p_vertex_diff,p_mesh->p_vertex_diff_material,p_mesh->vertexnum);
        p_mesh->p_data->k2flag |= KONT2_UPDATE_DIFF;
      }
      p_mesh->p_data->kflag &= ~KONT_DRAW_CAMERA;
    }
  }
  
  p_poly = p_ber->p_poly;
  for(i = 0; i < p_ber->polynum; i++) {
    kflag = p_poly->kflag;
    if(!(kflag&KONT_PRVEK) && (kflag&KONT_DRAW_CAMERA)) {
      if(p_poly->m1flag&MAT_PRUHLEDNY || p_poly->m2flag&MAT2_MASKA) {
        ber_reset_poly(p_poly->p_koord, p_poly->facenum);
        p_poly->kflag |= KONT_UPLOAD;
      }
      p_poly->kflag &= ~KONT_DRAW_CAMERA;
    }
    p_poly++;
  }
}

int ber_kamera_korekce_vzdalenosti(G_KONFIG *p_ber, int korekce, int korekce_vzdal)
{
  NASTAVENI_KAMER *p_kam = &p_ber->kam;
  BOD              p,pos;
  GLMATRIX        *p_cam = p_ber->p_camera,
                  *p_invcam = p_ber->p_invcam;
  float            t;
  

  calc_camera_bod(p_cam,p_invcam,p_ber->p_target,*p_ber->p_vzdal,*p_ber->p_fi,*p_ber->p_rfi);
  p_ber->kamera.zmena = TRUE;

  /* bez korekce - nic nedelej, prdime na to
  */
  if(!korekce) { 
    *p_ber->p_vzdal_kor = *p_ber->p_vzdal;
    set_matrix_camera(p_cam);
    set_matrix_camera_project(p_ber->p_project);
    return(FALSE);
  } else {
    /* je kamera v orezavaci kostce || je uplne mimo - kaslu na to
    */
    if(kd_je_bod_v_kostce((BOD *)&(p_invcam->_41),&p_kam->min,&p_kam->max)||
      !kd_je_bod_v_kostce(p_ber->p_target,&p_kam->min,&p_kam->max)) {
      
      if(p_ber->kam.min_vzdal > *p_ber->p_vzdal) {
        *p_ber->p_vzdal_kor = p_ber->kam.min_vzdal;
        calc_camera_bod(p_cam,p_invcam,p_ber->p_target,*p_ber->p_vzdal_kor,*p_ber->p_fi,*p_ber->p_rfi);
      } else {
        *p_ber->p_vzdal_kor = *p_ber->p_vzdal;
      }
      set_matrix_camera(p_cam);
      set_matrix_camera_project(p_ber->p_project);
      
      return(TRUE);
    } else {
      /* kamera ma target v kostce a je mimo kostku - provedu korekci
      */
      polar2kar(*p_ber->p_rfi, *p_ber->p_fi, *p_ber->p_vzdal, &pos);
      
      pos.x += p_ber->p_target->x;
      pos.y += p_ber->p_target->y;
      pos.z += p_ber->p_target->z;
      
      kd_intersect_kostku_bod_inter(p_ber->p_target, &pos, &p_kam->min, &p_kam->max, &p, &t);
      t = vzdal_bodu_bod(p_ber->p_target,&p)+p_ber->kam.near_plane;
      
      // vyberu ten blizsi bod
      *p_ber->p_vzdal_kor = (t > *p_ber->p_vzdal) ? *p_ber->p_vzdal : t;            

      // pokud je to bliz nez povolena vzdalenost
      if(p_ber->kam.min_vzdal > *p_ber->p_vzdal_kor)
        *p_ber->p_vzdal_kor = p_ber->kam.min_vzdal;
      
      if(korekce_vzdal) {
        *p_ber->p_vzdal = *p_ber->p_vzdal_kor;
      }
      
      calc_camera_bod(p_cam,p_invcam,p_ber->p_target,*p_ber->p_vzdal_kor,*p_ber->p_fi,*p_ber->p_rfi);
      set_matrix_camera(p_cam);
      set_matrix_camera_project(p_ber->p_project);
      return(TRUE);
    }
  }
}

// kartezske souradnice -> polarni souradnice
// prepocita kameru z jednoho modu do druheho
__inline void kam_set_mod(int mod)
{
  BOD p;

  // Behem animace se neda nahazovat mod kamery
  if(p_ber->kamera.aktivni&GAME_KAMERA_ANIMACE)
   return;
 
  // mod uz je nahozenej
  if(mod == (p_ber->kamera.aktivni&0x3))
    return;  

  // 3ds->polarni
  if(p_ber->kamera.aktivni&GAME_KAMERA_3DS) {
    p_ber->kamera.aktivni &= (~0x3);
    p_ber->kamera.aktivni |= GAME_KAMERA_POLAR;
    p.x = p_ber->kamera.p.x - p_ber->kamera.t.x;
    p.y = p_ber->kamera.p.y - p_ber->kamera.t.y;
    p.z = p_ber->kamera.p.z - p_ber->kamera.t.z;         
    kar2polar(&p, &p_ber->kamera.r, &p_ber->kamera.fi, &p_ber->kamera.vzdal);
  } else {
    // polarni->3ds
    p_ber->kamera.aktivni &= (~0x3);
    p_ber->kamera.aktivni |= GAME_KAMERA_3DS;    
    p_ber->kamera.r = normalizuj_uhel(p_ber->kamera.r);
    polar2kar(p_ber->kamera.r, p_ber->kamera.fi, p_ber->kamera.vzdal, &p_ber->kamera.p);
    p_ber->kamera.p.x += p_ber->kamera.t.x;
    p_ber->kamera.p.y += p_ber->kamera.t.y;
    p_ber->kamera.p.z += p_ber->kamera.t.z;
  } 
}

void kam_set_mod_param(int mod)
{
  kam_set_mod(mod);
}

void kam_set_mod_3ds(void)
{
  kam_set_mod(GAME_KAMERA_3DS);
}

void kam_set_mod_polar(void)
{
  kam_set_mod(GAME_KAMERA_POLAR);
}

int kam_get_mod(void)
{
  return(p_ber->kamera.aktivni&0x3);
}

/* Nastavi kameru
*/
int kam_pol_set(BOD *p_p, float r, float fi, float vzdal, int korekce)
{
  if(p_ber->kamera.aktivni&GAME_KAMERA_ANIMACE && 
   (*p_ber->p_mod)&GAME_KAMERA_3DS)
   return(K_CHYBA);

  if((*p_ber->p_mod)&GAME_KAMERA_3DS)
    kam_set_mod(GAME_KAMERA_POLAR);

  *p_ber->p_target = *p_p;
  *p_ber->p_rfi = DEG2RAD(r);
  *p_ber->p_fi = DEG2RAD(fi);
  *p_ber->p_vzdal = vzdal;
    
  ber_kamera_korekce_vzdalenosti(p_ber,p_ber->kam_omezeni,FALSE);
  p_ber->kamera.zmena = TRUE;
  return(TRUE);
}

/* Pricte vlozene hodnoty ke stavajicim
*/
int kam_pol_add(BOD *p_p, float r, float fi, float vzdal, int korekce)
{
  if((*p_ber->p_mod)&GAME_KAMERA_3DS)
    kam_set_mod(GAME_KAMERA_POLAR);

  p_ber->p_target->x += p_p->x;
  p_ber->p_target->y += p_p->y;
  p_ber->p_target->z += p_p->z;
  
  *p_ber->p_rfi += DEG2RAD(r);
  *p_ber->p_fi += DEG2RAD(fi);
  *p_ber->p_vzdal += vzdal;
  
  ber_kamera_korekce_vzdalenosti(p_ber,korekce,FALSE);
  p_ber->kamera.zmena = TRUE;
  return(TRUE);
}

/* Cte nastaveni kamery
*/
int kam_pol_get(BOD *p_p, float *p_r, float *p_fi, float *p_vzdal)
{
  if(p_ber->kamera.aktivni&GAME_KAMERA_ANIMACE && 
   (*p_ber->p_mod)&GAME_KAMERA_3DS)
   return(K_CHYBA);

  if((*p_ber->p_mod)&GAME_KAMERA_3DS)
    kam_set_mod(GAME_KAMERA_POLAR);
  *p_p = *p_ber->p_target;
  *p_r = RAD2DEG(*p_ber->p_rfi);
  *p_fi = RAD2DEG(*p_ber->p_fi);
  *p_vzdal = *p_ber->p_vzdal_kor;
  return(TRUE);
}

/* Cte aktualni smer pozici kamery
*/
void kam_get_float(float *p_body)
{
 p_body[0] = p_ber->p_invcam->_13; // vektor Z
 p_body[1] = p_ber->p_invcam->_23;
 p_body[2] = p_ber->p_invcam->_33;

 p_body[3] = p_ber->p_invcam->_12; // vektor Y
 p_body[4] = p_ber->p_invcam->_22;
 p_body[5] = p_ber->p_invcam->_32;
}

/* Cte pozici kamery + smery
*/
void kam_get_float_ext(float *p_dir, float *p_pos)
{
 p_dir[0] = p_ber->p_invcam->_13; // vektor Z
 p_dir[1] = p_ber->p_invcam->_23;
 p_dir[2] = p_ber->p_invcam->_33;

 p_dir[3] = p_ber->p_invcam->_12; // vektor Y
 p_dir[4] = p_ber->p_invcam->_22;
 p_dir[5] = p_ber->p_invcam->_32;

 p_pos[0] = p_ber->p_invcam->_41; // Pozice kamery
 p_pos[1] = p_ber->p_invcam->_42;
 p_pos[2] = p_ber->p_invcam->_43;
}

/* Nastavi kameru
*/
int kam_3ds_set(BOD *p_pos, BOD *p_target, float roll)
{ 
 if(p_ber->kamera.aktivni&GAME_KAMERA_ANIMACE && 
   (*p_ber->p_mod)&GAME_KAMERA_POLAR)
   return(K_CHYBA);
 
 if((*p_ber->p_mod)&GAME_KAMERA_POLAR)
   kam_set_mod(GAME_KAMERA_3DS);
 
 // nastavi kameru
 calc_camera_3ds(p_ber->p_camera,p_ber->p_invcam,p_pos,p_target,roll); 
 set_matrix_camera(p_ber->p_camera);
 set_matrix_camera_project(p_ber->p_project);
 p_ber->kamera.zmena = TRUE;
  
 // zapise
 *p_ber->p_target = *p_target;
 *p_ber->p_pos = *p_pos;
 *p_ber->p_roll = roll;
 return(TRUE);
}

/* Pricte kameru
*/
int kam_3ds_add(BOD *p_pos, BOD *p_target, float roll)
{
 if((*p_ber->p_mod)&GAME_KAMERA_POLAR)
   kam_set_mod(GAME_KAMERA_3DS);

 // zapise
 vektor_add(p_ber->p_target,p_target,p_ber->p_target);
 vektor_add(p_ber->p_pos,p_pos,p_ber->p_pos);
 *p_ber->p_roll += DEG2RAD(roll);

 calc_camera_3ds(p_ber->p_camera,p_ber->p_invcam,p_ber->p_pos,
                 p_ber->p_target,*p_ber->p_roll); 
 set_matrix_camera(p_ber->p_camera); 
 set_matrix_camera_project(p_ber->p_project);
 p_ber->kamera.zmena = TRUE;
 return(TRUE);
}

/* Nacte kameru 3ds
*/
int kam_3ds_get(BOD *p_pos, BOD *p_target, float *p_roll)
{
 if(p_ber->kamera.aktivni&GAME_KAMERA_ANIMACE && 
   (*p_ber->p_mod)&GAME_KAMERA_POLAR)
   return(K_CHYBA);

  if((*p_ber->p_mod)&GAME_KAMERA_POLAR)
   kam_set_mod(GAME_KAMERA_3DS);

 *p_pos = *p_ber->p_pos;
 *p_target = *p_ber->p_target;
 *p_roll = *p_ber->p_roll;
 return(TRUE);
}

/********************************************
  Polarni animace  
 ********************************************
*/
AnimHandle kam_pol_vyrob_animaci(int framenum, int pos_keys, int rot_keys, int vzdal_keys, int korekce)
{
  int kam;
  int i;
  
  for(i = 0; i < p_ber->kamnum; i++) {
    if(!p_ber->kamery[i].flag) {
      kam = i;
      goto  naslo_se_to2;
    }
  }
  if(p_ber->kamnum >= KAM_MAX_ANIM) {
    return(K_CHYBA);   
  }
  else
    kam = p_ber->kamnum++;

  naslo_se_to2:;  
  key_kamera_vyrob_indir(p_ber->kamery+kam,pos_keys,0,vzdal_keys,0,rot_keys);
  p_ber->kamery[kam].flag = (korekce) ? GAME_KAMERA_KOREKCE|GAME_KAMERA_POLAR : GAME_KAMERA_POLAR;
  p_ber->kamery[kam].endtime = calc_endtime(framenum);
  return(kam);
}

AnimHandle kam_pol_vloz_klic_posun(AnimHandle handle, int klic, float x, float y, float z, int frame)
{
  KAMERA_TRACK_INFO *p_kam;
  p_kam = p_ber->kamery+handle;
 
  if(handle >= p_ber->kamnum || klic >= p_kam->pos_keys)
    return(K_CHYBA);
  
  p_kam->p_pos[klic].x = x;
  p_kam->p_pos[klic].y = y;
  p_kam->p_pos[klic].z = z;
  p_kam->p_pkeys[klic].time = frame*SIM_KONSTI;
  
  return(handle);
}

AnimHandle kam_pol_vloz_klic_posun_bod(AnimHandle handle, int klic, BOD *p_p, int frame)
{
  if(handle >= p_ber->kamnum || klic >= p_ber->kamery[handle].pos_keys)
    return(K_CHYBA);
  p_ber->kamery[handle].p_pos[klic] = *p_p;
  p_ber->kamery[handle].p_pkeys[klic].time = frame*SIM_KONSTI;
  return(handle);
}

AnimHandle kam_pol_vloz_klic_posun_parametry(AnimHandle handle, int klic, float tension, float continuity, float bias)
{
  KAMERA_TRACK_INFO *p_kam;
  p_kam = p_ber->kamery+handle;
  
  if(handle >= p_ber->kamnum || klic >= p_kam->pos_keys)
    return(K_CHYBA);  
  
  p_kam->p_pkeys[klic].tension = tension;
  p_kam->p_pkeys[klic].continuity = continuity;
  p_kam->p_pkeys[klic].bias = bias;
  
  return(handle);
}

AnimHandle kam_pol_vloz_klic_rotace(AnimHandle handle, int klic, float r, float fi, int frame)
{
  if(handle >= p_ber->kamnum || klic >= p_ber->kamery[handle].quat_keys)
    return(K_CHYBA);  
  kam_angle_to_quat(DEG2RAD(r),DEG2RAD(fi),p_ber->kamery[handle].p_quat+klic);
  p_ber->kamery[handle].p_qkeys[klic].time = frame*SIM_KONSTI;
  return(handle);
}

AnimHandle kam_pol_vloz_klic_rotace_parametry(AnimHandle handle, int klic, float tension, float continuity, float bias)
{
  if(handle >= p_ber->kamnum || klic >= p_ber->kamery[handle].quat_keys)
    return(K_CHYBA);  
  p_ber->kamery[handle].p_qkeys[klic].bias = bias;
  p_ber->kamery[handle].p_qkeys[klic].continuity = continuity;
  p_ber->kamery[handle].p_qkeys[klic].tension = tension;
  return(handle);
}

AnimHandle kam_pol_vloz_klic_vzdal(AnimHandle handle, int klic, float vzdal, int frame, float tension, float continuity, float bias)
{
  KAMERA_TRACK_INFO *p_kam;
  p_kam = p_ber->kamery+handle;
  
  if(handle >= p_ber->kamnum || klic >= p_kam->roll_keys)
    return(K_CHYBA);  
  
  p_kam->p_roll[klic] = vzdal;
  p_kam->p_rlkeys[klic].time = frame*SIM_KONSTI;
  p_kam->p_rlkeys[klic].tension = tension;
  p_kam->p_rlkeys[klic].bias = bias;
  p_kam->p_rlkeys[klic].continuity = continuity;
  return(handle);
}

AnimHandle kam_pol_vloz_klic_all(AnimHandle handle, int klic, BOD *p_p, float r, float fi, float vzdal, int frame, float tension)
{
  KAMERA_TRACK_INFO *p_trk;
  int time = frame*SIM_KONSTI;
  p_trk = p_ber->kamery+handle;

  if(handle >= p_ber->kamnum)
    return(K_CHYBA);

  p_trk->p_pos[klic] = *p_p;
  p_trk->p_pkeys[klic].time = time;
  p_trk->p_pkeys[klic].tension = tension;
  
  kam_angle_to_quat(DEG2RAD(r),DEG2RAD(fi),p_trk->p_quat+klic);
  p_trk->p_qkeys[klic].time = time;
  p_trk->p_qkeys[klic].tension = tension;
  
  p_trk->p_roll[klic] = vzdal;
  p_trk->p_rlkeys[klic].time = time;
  p_trk->p_rlkeys[klic].tension = tension;
  
  return(handle);
}

AnimHandle kam_zrus_animaci(AnimHandle handle)
{
  if(handle >= p_ber->kamnum)
    return(K_CHYBA);
  key_kamera_zrus_indir(p_ber->kamery+handle);
  memset(p_ber->kamery+handle,0,sizeof(p_ber->kamery[0]));
  return(handle);
}


/********************************************
  3ds animace
 ********************************************
*/
AnimHandle kam_3ds_vyrob_animaci(int framenum, int pos_keys, int target_keys, int roll_keys, int fov_keys)
{
  int kam;
  int i;
  
  for(i = 0; i < p_ber->kamnum; i++) {
    if(!p_ber->kamery[i].flag) {
      kam = i;
      goto  naslo_se_to2;
    }
  }

  if(p_ber->kamnum >= KAM_MAX_ANIM) {
    return(K_CHYBA);   
  }
  else
    kam = p_ber->kamnum++;

  naslo_se_to2:;
  key_kamera_vyrob_indir(p_ber->kamery+kam,pos_keys,target_keys,roll_keys,fov_keys,0);
  p_ber->kamery[kam].flag = GAME_KAMERA_3DS;
  p_ber->kamery[kam].endtime = calc_endtime(framenum);
  return(kam);
}

/* Load animace
*/
AnimHandle kam_3ds_nahraj_animaci(APAK_HANDLE *pHandle, byte *p_jmeno)
{
  byte jmeno_kam[200];
  int  kam;
  int  i;
  
  for(i = 0; i < p_ber->kamnum; i++) {
    if(!p_ber->kamery[i].flag) {
      kam = i;
      goto  naslo_se_to2;
    }
  }

  if(p_ber->kamnum >= KAM_MAX_ANIM) {
    return(K_CHYBA);   
  }
  else
    kam = p_ber->kamnum++;

  naslo_se_to2:;
  if(!key_kamera_nahraj(p_ber->kamery+kam, jmeno_kam, p_ber->kamery[kam].jmeno, 
	                      pHandle, p_jmeno, p_ber->dir.game_data_dir))
    return(K_CHYBA);
  p_ber->kamery[kam].flag = GAME_KAMERA_3DS;
  return(kam);
}

/* Position-key
*/
AnimHandle kam_3ds_vloz_klic_posun(AnimHandle handle, int klic, float x, float y, float z, int frame)
{
  KAMERA_TRACK_INFO *p_kam;
  p_kam = p_ber->kamery+handle;
  
  if(handle >= p_ber->kamnum || klic >= p_kam->pos_keys)
    return(K_CHYBA);
  
  p_kam->p_pos[klic].x = x;
  p_kam->p_pos[klic].y = y;
  p_kam->p_pos[klic].z = z;
  p_kam->p_pkeys[klic].time = frame;
  return(handle);
}

AnimHandle kam_3ds_vloz_klic_posun_bod(AnimHandle handle, int klic, BOD *p_p, int frame)
{
  if(handle >= p_ber->kamnum || klic >= p_ber->kamery[handle].pos_keys)
    return(K_CHYBA);
  p_ber->kamery[handle].p_pos[klic] = *p_p;
  p_ber->kamery[handle].p_pkeys[klic].time = frame;
  return(handle);
}

AnimHandle kam_3ds_vloz_klic_posun_parametry(AnimHandle handle, int klic, float tension, float continuity, float bias)
{
  KAMERA_TRACK_INFO *p_kam;
  p_kam = p_ber->kamery+handle;
  
  if(handle >= p_ber->kamnum || klic >= p_kam->pos_keys)
    return(K_CHYBA);  
  
  p_kam->p_pkeys[klic].tension = tension;
  p_kam->p_pkeys[klic].continuity = continuity;
  p_kam->p_pkeys[klic].bias = bias;
  return(handle);
}

/* Target-key
*/
AnimHandle kam_3ds_vloz_klic_target(AnimHandle handle, int klic, float x, float y, float z, int frame)
{
  KAMERA_TRACK_INFO *p_kam;
  p_kam = p_ber->kamery+handle;
  
  if(handle >= p_ber->kamnum || klic >= p_kam->trg_keys)
    return(K_CHYBA);
  
  p_kam->p_trg[klic].x = x;
  p_kam->p_trg[klic].y = y;
  p_kam->p_trg[klic].z = z;
  p_kam->p_tkeys[klic].time = frame;
  return(handle);
}

AnimHandle kam_3ds_vloz_klic_target_bod(AnimHandle handle, int klic, BOD *p_target, int frame)
{
  if(handle >= p_ber->kamnum || klic >= p_ber->kamery[handle].trg_keys)
    return(K_CHYBA);
  p_ber->kamery[handle].p_trg[klic] = *p_target;
  p_ber->kamery[handle].p_tkeys[klic].time = frame;
  return(handle);
}

AnimHandle kam_3ds_vloz_klic_target_parametry(AnimHandle handle, int klic, float tension, float continuity, float bias)
{
  KAMERA_TRACK_INFO *p_kam;
  p_kam = p_ber->kamery+handle;
  
  if(handle >= p_ber->kamnum || klic >= p_kam->trg_keys)
    return(K_CHYBA);
  
  p_kam->p_tkeys[klic].bias = bias;
  p_kam->p_tkeys[klic].tension = tension;
  p_kam->p_tkeys[klic].continuity = continuity;
  return(handle);
}

/* Roll-keys
*/
AnimHandle kam_3ds_vloz_klic_roll(AnimHandle handle, int klic, float roll, int frame, float tension, float continuity, float bias)
{
  KAMERA_TRACK_INFO *p_kam;
  p_kam = p_ber->kamery+handle;
  
  if(handle >= p_ber->kamnum || klic >= p_kam->roll_keys)
    return(K_CHYBA);
  
  p_kam->p_roll[klic] = roll;
  p_kam->p_rlkeys[klic].time = frame*SIM_KONSTI;
  p_kam->p_rlkeys[klic].bias = bias;
  p_kam->p_rlkeys[klic].tension = tension;
  p_kam->p_rlkeys[klic].continuity = continuity;
  return(handle);
}

/* Fov-keys
*/
AnimHandle kam_3ds_vloz_klic_fov(AnimHandle handle, int klic, float fov, int frame, float tension, float continuity, float bias)
{
  KAMERA_TRACK_INFO *p_kam;
  p_kam = p_ber->kamery+handle;
  
  if(handle >= p_ber->kamnum || klic >= p_kam->fov_keys)
    return(K_CHYBA);
  
  p_kam->p_fov[klic] = fov;
  p_kam->p_fkeys[klic].time = frame*SIM_KONSTI;
  p_kam->p_fkeys[klic].bias = bias;
  p_kam->p_fkeys[klic].tension = tension;
  p_kam->p_fkeys[klic].continuity = continuity;
  return(handle);
}

/* Vlozit vsechny klice
*/
AnimHandle kam_3ds_vloz_klic_all(AnimHandle handle, int klic, BOD *p_p, BOD *p_t,
                                 float roll, int frame, float tension)
{
  KAMERA_TRACK_INFO *p_kam;
  int time = frame*SIM_KONSTI;
  p_kam = p_ber->kamery+handle;

  if(handle >= p_ber->kamnum || klic >= p_kam->pos_keys || 
    klic >= p_kam->trg_keys || klic >= p_kam->roll_keys)
    return(K_CHYBA);
  
  p_kam->p_pkeys[klic].time = time;
  p_kam->p_pkeys[klic].tension = tension;
  p_kam->p_pos[klic] = *p_p;  
  
  p_kam->p_tkeys[klic].time = time;
  p_kam->p_tkeys[klic].tension = tension;
  p_kam->p_trg[klic] = *p_t;  
  
  p_kam->p_rlkeys[klic].time = time;
  p_kam->p_rlkeys[klic].tension = tension;
  p_kam->p_roll[klic] = roll;
    
  return(handle);
}

/********************************************
  Vsechny animace - Animacni stroj
 ********************************************
*/
//start/stop animace
int kam_start(int a_handle, int *p_flag, int flag, int start, int stop)
{
 if(p_ber->kamery[a_handle].endtime) {
   p_ber->kamera.p_anim = p_ber->kamery+a_handle;
   p_ber->kamera.aktivni = p_ber->kamery[a_handle].flag|GAME_KAMERA_ANIMACE;
   
   p_ber->kamera.p_flag = p_flag;
   p_ber->kamera.flag = flag;
   p_ber->kamera.a_handle = a_handle;

   *p_flag = 0;

   calc_time_end(p_ber->kamera.p_anim->endtime,p_ber->TimeEndLastFrame,
                 start,stop,
                 &p_ber->kamera.start,
                 &p_ber->kamera.stop,
                 &p_ber->kamera.time_start,
                 &p_ber->kamera.time_stop,
                 &p_ber->kamera.time_delka);
 }
 return(TRUE);
}

void kam_stop(void)
{
  if(p_ber->kamera.aktivni&GAME_KAMERA_ANIMACE) {
    if(p_ber->kamera.flag&GK_REMOVE)
      kam_zrus_animaci(p_ber->kamera.a_handle);
    
    p_ber->kamera.aktivni &= ~GAME_KAMERA_ANIMACE;
    p_ber->kamera.p_anim = NULL;
    p_ber->kamera.a_handle = K_CHYBA;
  }
}

void kani_updatuj(G_KONFIG *p_ber)
{
  KAMERA_TRACK_INFO *p_track;
  GAME_KAMERA *p_kam; 
  int          konec, loop, zrusit = FALSE;
  int          next_time = p_ber->TimeEndLastFrame;
  QUAT         q = {1.0f,0.0f,0.0f,0.0f};

  p_kam = &p_ber->kamera;
  p_track = p_ber->kamera.p_anim;   

  loop = p_kam->flag&GK_LOOP;

  if((konec = (next_time > p_kam->time_stop))) {
    if(loop) {
      calc_time_loop(next_time,p_kam->start,
                    &p_kam->time_start,
                    &p_kam->time_stop,
                    &p_kam->time_delka,
                    &p_kam->time);
      if(p_kam->p_flag)
       *(p_kam->p_flag) = 0;
      konec = 0;
    } else {
      p_kam->time = p_kam->time_delka;
      zrusit = (p_kam->flag&GK_REMOVE);
      if(p_kam->p_flag)
       *(p_kam->p_flag) = K_CHYBA;
    }
  } else {
    p_kam->time = calc_time_akt(next_time,p_kam->time_start);
    if(p_kam->p_flag)
     *(p_kam->p_flag) = ftoi((p_kam->time/(float)p_kam->time_delka)*100.0f);
  }
  

  if(p_kam->aktivni&GAME_KAMERA_3DS) {
/*
    if(p_track->fov_keys) {
      key_track_interpolace_float(&p_kam->fov,  p_track->p_fov,  p_track->p_fkeys, p_kam->time, p_track->endtime, p_track->fov_keys, loop);
      projection_matrix(&p_ber->kamera.project,p_kam->fov,(float)OXRES/(float)OYRES,p_ber->kam.near_plane,p_ber->kam.far_plane);
      set_matrix_project(p_ber->p_project);
    }
*/
    if(p_track->pos_keys)
      key_track_interpolace_bod(&p_kam->p, p_track->p_pos, p_track->p_pkeys, p_kam->time, p_track->endtime, p_track->pos_keys,loop);
    if(p_track->trg_keys)
      key_track_interpolace_bod(&p_kam->t, p_track->p_trg, p_track->p_tkeys, p_kam->time, p_track->endtime, p_track->trg_keys,loop);
    if(p_track->roll_keys)
      key_track_interpolace_float(&p_kam->roll, p_track->p_roll, p_track->p_rlkeys, p_kam->time, p_track->endtime, p_track->roll_keys,loop);
    
    calc_camera_3ds(p_ber->p_camera,p_ber->p_invcam,&p_kam->p,&p_kam->t,p_kam->roll);
    set_matrix_camera(p_ber->p_camera);    
    set_matrix_camera_project(p_ber->p_project);
    p_ber->kamera.zmena = TRUE;
  } else {
    if(p_track->pos_keys)
      key_track_interpolace_bod(&p_kam->t, p_track->p_pos, p_track->p_pkeys, p_kam->time, p_track->endtime, p_track->pos_keys,loop);
    if(p_track->quat_keys)
      key_track_interpolace_quat(&q, p_track->p_quat, p_track->p_qkeys, p_kam->time, p_track->endtime, p_track->quat_keys,loop);
    if(p_track->roll_keys)
      key_track_interpolace_float(&p_kam->vzdal, p_track->p_roll, p_track->p_rlkeys, p_kam->time, p_track->endtime, p_track->roll_keys,loop);
    
    calc_camera_polar(p_ber->p_camera, p_ber->p_invcam, &p_kam->t, &q, p_kam->vzdal);
    quat_to_euler(&q, &p_kam->r, &p_kam->fi);

    ber_kamera_korekce_vzdalenosti(p_ber,p_ber->kam_omezeni,FALSE);

    p_ber->kamera.zmena = TRUE;
  }

  // Zrus animaci
  if(zrusit)
    kam_stop();
}

/* Animace polarni
*/
RunHandle kam_pol_anim(BOD *p_p, float r, float fi, float vzdal, 
                       int *p_flag, int flag, int framenum, float tension)
{
  GAME_KAMERA *p_kam = &p_ber->kamera;
  AnimHandle ah;    
 
  kam_stop();  
  kam_set_mod(GAME_KAMERA_POLAR);
  
  ah = kam_pol_vyrob_animaci(framenum,2,2,2,TRUE);
  
  kam_pol_vloz_klic_all(ah,0,&p_kam->t,RAD2DEG(p_kam->r),RAD2DEG(p_kam->fi),p_kam->vzdal,0,tension);
  kam_pol_vloz_klic_all(ah,1,p_p,r,fi,vzdal,framenum-1,tension);
  
  kam_start(ah,p_flag,flag|GK_REMOVE,0,0); 
  
  return(0);
}

/* Animace polarni add
*/
RunHandle kam_pol_anim_add(BOD *p_p, float r, float fi, float vzdal, 
                           int *p_flag, int flag, int framenum, float tension)
{
  GAME_KAMERA *p_kam = &p_ber->kamera;
  AnimHandle ah;
  BOD pp;
  
  kam_stop();  
  kam_set_mod(GAME_KAMERA_POLAR);
  
  ah = kam_pol_vyrob_animaci(framenum,2,2,2,TRUE);
  
  kam_pol_vloz_klic_all(ah,0,&p_kam->t,RAD2DEG(p_kam->r),RAD2DEG(p_kam->fi),p_kam->vzdal,0,tension);
  kam_pol_vloz_klic_all(ah,1,vektor_add(&p_kam->t,p_p,&pp),
                        RAD2DEG(p_kam->r)+r,
                        RAD2DEG(p_kam->fi)+fi,
                        p_kam->vzdal+vzdal,
                        framenum-1,tension);
  
  kam_start(ah,p_flag,flag|GK_REMOVE,0,0);
  
  return(0);
}

/* Animace 3ds
*/
RunHandle kam_3ds_anim(BOD *p_p, BOD *p_t, float roll, int *p_flag, int flag, 
                       int framenum, float tension)
{
  GAME_KAMERA *p_kam = &p_ber->kamera;
  AnimHandle ah;  
  
  kam_stop(); 
  kam_set_mod(GAME_KAMERA_3DS);  
  
  ah = kam_3ds_vyrob_animaci(framenum,2,2,2,0);
  
  kam_3ds_vloz_klic_all(ah,0,&p_kam->p,&p_kam->t,p_kam->roll,0,tension);
  kam_3ds_vloz_klic_all(ah,1,p_p,p_t,DEG2RAD(roll),framenum-1,tension);
  
  kam_start(ah,p_flag,flag|GK_REMOVE,0,0);
  
  return(0);
}

/* Animace 3ds add
*/
RunHandle kam_3ds_anim_add(BOD *p_p, BOD *p_t, float roll,int *p_flag, int flag, 
                           int framenum, float tension)
{
  GAME_KAMERA *p_kam = &p_ber->kamera;
  AnimHandle ah;
  BOD        p,t;
  
  kam_stop();
  kam_set_mod(GAME_KAMERA_3DS);  
  
  ah = kam_3ds_vyrob_animaci(framenum,2,2,2,0);
  
  kam_3ds_vloz_klic_all(ah,0,&p_kam->p,&p_kam->t,p_kam->roll,0,tension);
  kam_3ds_vloz_klic_all(ah,1,vektor_add(&p_kam->p,p_p,&p),
                             vektor_add(&p_kam->t,p_t,&t),
                             p_kam->roll+DEG2RAD(roll),
                             framenum-1,tension);
  
  kam_start(ah,p_flag,flag|GK_REMOVE,0,0);
  return(0);
}

void kam_set_fov(float fov)
{
  p_ber->kamera.fov = DEG2RAD(fov);
  projection_matrix(p_ber->p_project,p_ber->kamera.fov,(float)OXRES/(float)OYRES,p_ber->kam.near_plane,p_ber->kam.far_plane);
  set_matrix_project(p_ber->p_project);
  set_matrix_camera_project(p_ber->p_project);
}

float kam_get_fov(void)
{
  return(RAD2DEG(p_ber->kamera.fov));
}


static BOD __last_kam_bod;

static void kam_privaz_kameru_updatuj(int a_handle)
{
  GK_ANIM *p_gk = (GK_ANIM *)a_handle;  
  BOD      vysl;
  
  vektor_sub(&p_gk->p,&__last_kam_bod,&vysl);
  __last_kam_bod = p_gk->p;
  kam_pol_add(&vysl,0,0,0,TRUE);
}

static void kam_privaz_kameru_start(int a_handle)
{
  __last_kam_bod.x = __last_kam_bod.y = __last_kam_bod.z = 0.0f;
}


// Privaze target-point kamery na std. animaci
MatrixHandle rani_privaz_kameru(RunHandle a_handle)
{
  GK_ANIM *p_gk = (GK_ANIM *)a_handle;
  GKA_MATICE *p_mat;
  MatrixHandle matice;
  
  if(p_gk->matic < MAX_ANIM_MATIC) {
    
    matice              = p_gk->matic++;
    p_mat               = p_gk->p_mat+matice;
    
    p_mat->p_mat        = NULL;
    p_mat->p_notify     = NULL;
    p_mat->flagy        = MESH_KAMERA;
    p_mat->fce_param    = a_handle;
    p_mat->funkce_start = kam_privaz_kameru_start;
    p_mat->funkce_frame = kam_privaz_kameru_updatuj;
    p_mat->funkce_end   = kam_privaz_kameru_updatuj;
    
    return(matice);
  } else
    return(K_CHYBA);
}

int kam_3ds_cti_klic(AnimHandle handle, float time, BOD *p_p, BOD *p_t, float *p_roll)
{
  KAMERA_TRACK_INFO *p_track;
  int loop;
  int dtime;

  if(handle >= p_ber->kamnum)
    return(K_CHYBA);

  p_track = p_ber->kamery+handle;
  
  if(!p_track || p_track->flag&GAME_KAMERA_POLAR)
    return(K_CHYBA);
  
  loop = p_track->flag&GK_LOOP;
  if(time > 1.0f)
    time = 1.0f;
  else if(time < 0.0f)
    time = 0.0f;  

  dtime = ftoi(time*p_track->endtime);
  
  if(p_track->pos_keys)
    key_track_interpolace_bod(p_p, p_track->p_pos, p_track->p_pkeys, dtime, p_track->endtime, p_track->pos_keys,loop);
  if(p_track->trg_keys)
    key_track_interpolace_bod(p_t, p_track->p_trg, p_track->p_tkeys, dtime, p_track->endtime, p_track->trg_keys,loop);
  if(p_track->roll_keys)
    key_track_interpolace_float(p_roll, p_track->p_roll, p_track->p_rlkeys, dtime, p_track->endtime, p_track->roll_keys, loop);
  else
    *p_roll = 0.0f;
  
  return(TRUE);
}

int kam_pol_cti_klic(AnimHandle handle, float time, BOD *p_t, float *p_r, float *p_fi, float *p_vzdal)
{
  KAMERA_TRACK_INFO *p_track;
  int  loop,dtime;
  QUAT q = {1,0,0,0};

  if(handle >= p_ber->kamnum)
    return(K_CHYBA);

  p_track = p_ber->kamery+handle;
  
  if(!p_track || p_track->flag&GAME_KAMERA_3DS)
    return(K_CHYBA);  

  loop = p_track->flag&GK_LOOP;

  if(time > 1.0f)
    time = 1.0f;
  else if(time < 0.0f)
    time = 0.0f;  
  
  dtime = ftoi(time*p_track->endtime);

  if(p_track->pos_keys)
    key_track_interpolace_bod(p_t, p_track->p_pos, p_track->p_pkeys, dtime, p_track->endtime, p_track->pos_keys,loop);
  if(p_track->quat_keys)
    key_track_interpolace_quat(&q, p_track->p_quat, p_track->p_qkeys, dtime, p_track->endtime, p_track->quat_keys,loop);
  if(p_track->roll_keys)
    key_track_interpolace_float(p_vzdal, p_track->p_roll, p_track->p_rlkeys, dtime, p_track->endtime, p_track->roll_keys,loop);
  quat_to_euler(&q, p_r, p_fi);
  
  return(TRUE);
}

void ber_zpruhledni_prvky_reset(G_KONFIG *p_ber)
{   
  EDIT_MESH_POLY *p_poly;
  GAME_MESH      *p_mesh;
  int             i;

  for(i = 0; i < p_ber->meshnum; i++) {
    p_mesh = p_ber->p_mesh[i];
    if(p_mesh) {      
      if(p_mesh->p_data->kflag&KONT_PRVEK &&
         (p_mesh->p_data->m1flag&MAT_PRUHLEDNY || p_mesh->p_data->m2flag&MAT2_MASKA) && 
         p_mesh->p_vertex_diff) {
        ber_reset_mesh(p_mesh->p_vertex_diff,p_mesh->p_vertex_diff_material,p_mesh->vertexnum);
        p_mesh->p_data->k2flag |= KONT2_UPDATE_DIFF;
      }
      p_mesh->p_data->kflag &= ~KONT_DRAW_CAMERA;
    }
  }
  
  p_poly = p_ber->p_poly;
  for(i = 0; i < p_ber->polynum; i++) {    
    if(p_poly->kflag&(KONT_DRAW_CAMERA|KONT_PRVEK)) {
      if(p_poly->m1flag&MAT_PRUHLEDNY || p_poly->m2flag&MAT2_MASKA) {
        ber_reset_poly(p_poly->p_koord, p_poly->facenum);
        p_poly->kflag |= KONT_UPLOAD;
      }      
      p_poly->kflag &= ~KONT_DRAW_CAMERA;
    }
    p_poly++;
  }
}

void ber_zpruhledni_prvky(G_KONFIG *p_ber)
{
  EDIT_MESH_POLY *p_poly;
  TEXT_KOORD     *p_koord;
  GLMATRIX       *p_camera_project = get_matrix_camera_project();
  GLMATRIX       *p_m;
  float           ra,radius = p_ber->conf_pruhledna_kamera_radius;
  float           vzdal,alfa;
  static BOD      lt = {FLT_MAX,FLT_MAX,FLT_MAX};
  OBB_TREE       *p_prvni = &p_ber->obbtree;
  GAME_MESH      *p_mesh,*p_beruska;
  BOD             t,p,i,*p_vrt,vr;
  BODRGBA        *p_d;
  BODRGBA        *p_dm;
  int             mh,o,vs,vn;
  int             v,key;
  word           *p_objektstart;
  word           *p_objektnum;
  BODRGBA        *p_vertex_diff;
  BODRGBA        *p_vertex_diff_material;
  BOD            *p_vertex_pos;
  OBB            *p_obb;  
  int             kamera_zmena = p_ber->kamera.zmena||p_ber->kamera.zmena_last;

  if(!p_ber->conf_pruhledna_kamera)
    return;

  mh = gl_Get_Active_Beetle_Mesh();
  if(mh == K_CHYBA)
    return;
  t.x = p_ber->p_prv_lev[mh]->x;
  t.y = p_ber->p_prv_lev[mh]->y;
  t.z = p_ber->p_prv_lev[mh]->z;

  if(!kamera_zmena && lt.x == t.x && lt.y == t.y && lt.z == t.z) {
    return;
  }
  else {
    lt.x = t.x;
    lt.y = t.y;
    lt.z = t.z;
  }

  p_beruska = p_ber->p_mesh[p_ber->p_prv_lev[mh]->mesh];

  p.x = p_ber->p_invcam->_41;
  p.y = p_ber->p_invcam->_42;
  p.z = p_ber->p_invcam->_43;

  //t - pozice berusky
  ber_mesh_render_list_reset(p_ber);
  while(p_mesh = ber_mesh_render_list_next_flag(p_ber,KONT_VIDITELNY|KONT_PRVEK,FALSE)) {
    if(p_beruska == p_mesh) {
      p_mesh->p_data->kflag &= ~KONT_DRAW_CAMERA;
    } else {      
      BOD *p_str = &p_mesh->obb_world.obb_stred;
      
      bod_primka_bod_bod(p_str, &p, &t, &i);
      if(!bod_mezi(&p, &t, &i)) {
        if((p_mesh->p_data->m1flag&MAT_PRUHLEDNY || p_mesh->p_data->m2flag&MAT2_MASKA) && p_mesh->p_vertex_diff) {
          ber_reset_mesh(p_mesh->p_vertex_diff,p_mesh->p_vertex_diff_material,p_mesh->vertexnum);
          p_mesh->p_data->k2flag |= KONT2_UPDATE_DIFF;
        }
        p_mesh->p_data->kflag &= ~KONT_DRAW_CAMERA;
      } else {
        vzdal = obb_vzdal_bod_aabb(&p_mesh->obb_world, &i);
        if(vzdal > radius) {
          if((p_mesh->p_data->m1flag&MAT_PRUHLEDNY || p_mesh->p_data->m2flag&MAT2_MASKA) && p_mesh->p_vertex_diff) {
            ber_reset_mesh(p_mesh->p_vertex_diff,p_mesh->p_vertex_diff_material,p_mesh->vertexnum);
            p_mesh->p_data->k2flag |= KONT2_UPDATE_DIFF;
          }
          p_mesh->p_data->kflag &= ~KONT_DRAW_CAMERA;          
        } else {                
          if(p_mesh->p_vertex_diff) {
            p_mesh->p_data->kflag |=  KONT_DRAW_CAMERA;
            p_mesh->p_data->k2flag |= KONT2_UPDATE_DIFF;
            key = p_mesh->p_data->kflag&KONT_KEYFRAME;
            p_objektstart = p_mesh->objektstart;
            p_objektnum = p_mesh->objektnum;
            p_vertex_diff = p_mesh->p_vertex_diff;
            p_vertex_diff_material = p_mesh->p_vertex_diff_material;
            p_vertex_pos = p_mesh->p_vertex_pos;
            
            if(!key)
              p_m = &p_mesh->m;

            p_obb = p_mesh->p_obb_world;
            for(o = 0; o < p_mesh->objektu; o++, p_obb++) {
              vs = *p_objektstart++;
              vn = *p_objektnum++;
              p_d = p_vertex_diff+vs;
              p_dm = p_vertex_diff_material+vs;

              bod_primka_bod_bod(&p_obb->obb_stred, &p, &t, &i);
              if(obb_vzdal_bod_aabb(p_obb, &i) < radius) {

                if(key)
                  p_m = p_mesh->p_key+o;
                
                p_vrt = p_vertex_pos+vs;

                for(v = 0; v < vn; v++, p_d++, p_dm++, p_vrt++) {
                  transformuj_bod_matici_bod(p_vrt,p_m,&vr);
                  ra = vzdal_bodu_a_primky(&vr,&p, &t);
                  if(ra < radius) {
                    alfa = (ra/radius);
                    if(p_d->a > alfa)
                      p_d->a = alfa;
                  } else {
                    p_d->a = p_dm->a;
                  }
                }
              } else {
                for(v = 0; v < vn; v++, p_d++, p_dm++) {
                  p_d->a = p_dm->a;
                }
              }
            }            
          }
        }
      }
    }  
  }

  ber_poly_render_list_reset(p_ber);
  while(p_poly = ber_poly_render_list_next_flag(p_ber,KONT_VIDITELNY|KONT_PRVEK,FALSE)) {  
    bod_primka_bod_bod(&p_poly->obb.obb_stred, &p, &t, &i);
    if(!bod_mezi(&p, &t, &i)) {
      if(p_poly->m1flag&MAT_PRUHLEDNY || p_poly->m2flag&MAT2_MASKA) {
        ber_reset_poly(p_poly->p_koord, p_poly->facenum);
        p_poly->kflag |= KONT_UPLOAD;
      }
      p_poly->kflag &= ~KONT_DRAW_CAMERA;
      continue;
    }
    vzdal = obb_vzdal_bod_aabb(&p_poly->obb,&i);
    if(vzdal > radius) {
      if(p_poly->m1flag&MAT_PRUHLEDNY || p_poly->m2flag&MAT2_MASKA) {
        ber_reset_poly(p_poly->p_koord, p_poly->facenum);
        p_poly->kflag |= KONT_UPLOAD;
      }
      p_poly->kflag &= ~KONT_DRAW_CAMERA;
      continue;
    }
    p_koord = p_poly->p_koord;
    for(v = 0; v < p_poly->facenum; v++, p_koord++) {
      ra = vzdal_bodu_a_primky((BOD *)(&p_koord->x),&p,&t);
      if(ra < radius) {
        alfa = (ra/radius);
        if(p_koord->da > alfa)
          p_koord->da = alfa;
      } else {
        p_koord->da = p_koord->mda;
      }
    }
    p_poly->kflag |= (KONT_UPLOAD|KONT_DRAW_CAMERA);
  }
}

#define KINO_POMER (6.0f/12.0f)

void kam_kino_set_clear(G_KONFIG *p_ber)
{
  glClearColor(0.0f,0.0f,0.0f,1.0f);
}

void kam_game_set_clear(G_KONFIG *p_ber)
{
  float b[4];
  if(p_ber->conf_barva_pozadi_pouzit) {
    rgb_float(p_ber->conf_barva_pozadi,b);
    glClearColor(b[0],b[1],b[2],1.0f);
  }
}

void kam_set_kino_screen(G_KONFIG *p_ber)
{
  int kxres = p_ber->wxres;
  int kyres = ftoi(p_ber->wxres*KINO_POMER);
  int kx = p_ber->wx,
      ky = p_ber->wy + (p_ber->wyres-kyres)/2;

  set_matrix_view(kx, ky, kxres, kyres);
  projection_matrix(&p_ber->kamera.project,p_ber->kam.fov,(float)kxres/(float)kyres,p_ber->kam.near_plane,p_ber->kam.far_plane);
  set_matrix_project(p_ber->p_project);
}

void kam_set_normal_screen(G_KONFIG *p_ber)
{
  set_matrix_view(p_ber->wx, p_ber->wy, p_ber->wxres, p_ber->wyres);
  projection_matrix(&p_ber->kamera.project,p_ber->kam.fov,(float)OXRES/(float)OYRES,p_ber->kam.near_plane,p_ber->kam.far_plane);
  set_matrix_project(p_ber->p_project);
}
