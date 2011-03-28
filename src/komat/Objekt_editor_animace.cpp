/*
  animace
*/
#include "mss_on.h"
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <direct.h>
#include <float.h>
#include <assert.h>
#include "objekt_editor_all.h"

#include "dbgwnd.h"


void oe_animace_cti_t(K_EDITOR *p_cnf)
{
  byte string[100];
  doe_cti_jmeno(hwnd_aplikace, string);
  p_cnf->key_time = (float)atof(string);

  //oe_animace_aplikuj(p_cnf);
}

/*
if((konec = (next_time > p_sinfo->time_stop))) { // chyby mi tady osetreni loop flagu
  if(p_sinfo->flag[0]&GK_LOOP) {
    if(p_sinfo->p_flag[0])
      *(p_sinfo->p_flag[0]) = 0;
  } else {
    p_sinfo->time = p_sinfo->time_delka;
    if(p_sinfo->p_flag[0])
      *(p_sinfo->p_flag[0]) = ANIM_FLAG_KONEC;
    p_sinfo->akt[0] = K_CHYBA; 
  }
} else {
  p_sinfo->time = calc_time_akt(next_time,p_sinfo->time_start);
  if(p_sinfo->p_flag[0])
    *(p_sinfo->p_flag[0]) = ftoi((p_sinfo->time/(float)p_sinfo->time_delka)*100.0f);
}
*/

int ani_animuj_scenu(K_EDITOR *p_cnf)
{
  HIERARCHY_SIM  *p_sim;
  GLMATRIX       *p_world;
  int             k, va, konec, update = 0;
  dword           next_time = p_cnf->TimeEndLastFrame;

  for(k = 0; k < p_cnf->max_kont; k++) {
    if(p_cnf->p_kont[k]) {
      va = p_cnf->p_kont[k]->akt_animace;
      p_sim = p_cnf->p_kont[k]->sim+va;
      p_world = kont_world_matrix(p_cnf->p_kont[k]);
      if(p_sim->stav == 1) {
        update++;
        if((konec = (next_time > p_sim->time_stop))) {
          if(p_sim->flag&GK_LOOP) {
            calc_time_loop(next_time,p_sim->start,p_sim->time_start,
                           p_sim->time_stop,p_sim->time_delka,p_sim->time);
          } else {            
            p_sim->time = p_sim->time_delka;
            p_sim->stav = 0;
          }
        } else {
          p_sim->time = calc_time_akt(next_time,p_sim->time_start);
        }
        key_sim_root_to_matrix(p_cnf->p_kont[k],p_sim,p_sim->time,p_world);
      }
    }
  }
  return(update);
}

void ani_animuj_scenu_vybr_animace(K_EDITOR *p_cnf, EDIT_KONTEJNER *p_kont, dword time)
{
  HIERARCHY_SIM *p_sim;
  GLMATRIX *p_world;

  p_sim = p_kont->sim+p_kont->akt_animace;
  p_world = kont_world_matrix(p_kont);
  key_sim_root_to_matrix(p_kont,p_sim,time,p_world);
}

void ani_anim_strom_tiskni_rec(K_EDITOR *p_cnf, EDIT_KONTEJNER *p_kont, HIERARCHY_TRACK_INFO *p_tri, int hloubka)
{
 byte pom[100];
 int  i;

 assert(hloubka*2 < 100);
 memset(pom,' ',sizeof(byte)*hloubka*2);
 pom[hloubka*2] = '\0';

 for(i = 0; i < p_tri->childnum; i++) {
   assert(p_tri->p_child[i]);
   oe_wrlog(p_cnf,"%s %d %s",pom,
                             p_tri->p_child[i]->objnum,
                             p_kont->p_obj[p_tri->p_child[i]->objnum]->jmeno);
   ani_anim_strom_tiskni_rec(p_cnf,p_kont,p_tri->p_child[i],hloubka+1);
 }
}

// tiskne animacni hierarchii - strom
void ani_anim_strom_tiskni(K_EDITOR *p_cnf, EDIT_KONTEJNER *p_kont, HIERARCHY_ROOT *p_root)
{
 int i; 
 oe_wrlog(p_cnf,"Hierarchie animacniho stromu animace %s...",p_root->jmeno);
 for(i = 0; i < p_root->childnum; i++) {
   assert(p_root->p_child[i]);
   oe_wrlog(p_cnf,"%d %s",p_root->p_child[i]->objnum,
                          p_kont->p_obj[p_root->p_child[i]->objnum]->jmeno);   
   ani_anim_strom_tiskni_rec(p_cnf,p_kont,p_root->p_child[i],0);
 }
}

void ani_tiskni_kontejner(K_EDITOR *p_cnf, EDIT_KONTEJNER *p_kont)
{
  int o;

  for(o = 0; o < p_kont->max_objektu; o++) {
    if(p_kont->p_obj[o]) {
      oe_wrlog_matrix(p_cnf,&p_kont->p_obj[o]->m,"Objekt %d %s...",
                                           o,p_kont->p_obj[o]->jmeno);
    }
  }
}

/* Provede animace kamer
*/
int ani_animuj_kamery(K_EDITOR *p_cnf)
{
  KAMERA_TRACK_INFO *p_track;
  KAMERA *p_kam;
  dword   next_time = p_cnf->TimeEndLastFrame;
  
  if(p_cnf->kamakt < p_cnf->kamnum) {
    p_kam = p_cnf->kamery+p_cnf->kamakt;    
    p_track = p_kam->tracky+p_kam->akt_animace;
    if(p_track->stav && p_track->endtime) {
      if(next_time > p_track->time_stop) {
        p_track->time = p_track->time_delka;
        p_track->stav = 0;
      } else {
        p_track->time = calc_time_akt(next_time, p_track->time_start);          
      }
      key_kan_to_kamera(p_kam, p_track, p_track->time);
      kameru_dratovou_reanimuj(p_kam);
      if(p_cnf->cnf.kamera_aktivni)
        oe_kamera_to_view(p_cnf,p_cnf->kamakt);
      return(TRUE);
    }
  }
  return(FALSE);
}

/*   
*/

/* Animace kostnich systemu - animuje se od aktivniho jointu vys
*/
int ani_animuj_kosti(K_EDITOR *p_cnf)
{
  JOINT_ANIMACE *p_anim = p_cnf->jcf.p_play;
  dword next_time = p_cnf->TimeEndLastFrame;

  if(!p_anim)
    return(FALSE);

  // Jen aktualni joint
  if(!p_cnf->jgen.anim_vse) {
    if(p_anim->stav && p_anim->framenum) {
      if(next_time > p_anim->time_stop) {
        if(p_anim->flag&GK_LOOP) {
          doe_anim_start_et(next_time,calc_endtime(p_anim->framenum),
            &p_anim->time_start,&p_anim->time_stop,
            &p_anim->time_delka,&p_anim->time,
            &p_anim->start,&p_anim->stop);
        } else {
          p_anim->time = p_anim->time_delka;
          p_anim->stav = 0;
        }
      } else {
        p_anim->time = calc_time_akt(next_time, p_anim->time_start);          
      }      
      doe_joint_animace_vloz_frame(p_cnf);
      doe_joint_animace_vloz_text(p_cnf);
      return(TRUE);
    }
  } else {    
    while(p_anim) {
      if(p_anim->p_obj && p_anim->stav && p_anim->framenum) {
        if(next_time > p_anim->time_stop) {
          if(p_anim->flag&GK_LOOP) {
            doe_anim_start_et(next_time,calc_endtime(p_anim->framenum),
              &p_anim->time_start,&p_anim->time_stop,
              &p_anim->time_delka,&p_anim->time,
              &p_anim->start,&p_anim->stop);
          } else {
            p_anim->time = p_anim->time_delka;
            p_anim->stav = 0;
          }
        } else {
          p_anim->time = calc_time_akt(next_time, p_anim->time_start);
        }
        key_kosti_animuj(p_anim);
        key_kosti_updatuj_vertexy((EDIT_OBJEKT *)p_anim->p_obj, p_anim);
      } else {
        // vyhodim animaci ze seznamu
        if(p_anim->p_game_prev) {
          p_anim->p_game_prev->p_game_next = p_anim->p_game_next;
          if(p_anim->p_game_next) {
            p_anim->p_game_next->p_game_prev = p_anim->p_game_prev;
          }
        } else {
          p_cnf->jcf.p_play = p_anim->p_game_next;
          if(p_anim->p_game_next) {
            p_anim->p_game_next->p_game_prev = NULL;
          }
        }
      } 
      p_anim = p_anim->p_game_next;
    }
  }
  return(TRUE);
}

/*
  Animace texture-koordinatu
*/
int ani_animuj_materialy(K_EDITOR *p_cnf)
{
  EDIT_MATERIAL *p_mat;
  ANIM_TEXT *p_text;
  GLMATRIX  *p_ttext;
  dword      next_time = p_cnf->TimeEndLastFrame;
  int konec,m,update = 0;
  int am,j;

  for(m = 0; m < MAX_EDIT_MATERIALU; m++) {
    p_mat = p_cnf->p_mat[m];
    if(p_mat) {
      for(j = 0; j < MAT_TEXTUR; j++) {
        am = p_mat->atextakt[j];
        p_text = p_mat->p_atext[j][am];
        p_ttext = p_mat->ttext+j;
        if(p_text && p_text->stav == 1) {
          update++;
          if((konec = (next_time > p_text->time_stop))) {
            if(p_text->flag&GK_LOOP) {
              calc_time_loop(next_time,p_text->start,p_text->time_start,
                             p_text->time_stop,p_text->time_delka,p_text->time);
            } else {
              p_text->time = p_text->time_delka;
              p_text->stav = 0; // konec animace
            }
          } else {
            p_text->time = calc_time_akt(next_time,p_text->time_start);
          }
          key_sim_to_text_matrix(p_text, p_text->time, p_ttext);        
        }
      }
    }
  }
  return(update);
}
