/*
  Light&fog funkce
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

/* *********************************************************************
  Obarvovac bodu
*/
__inline void nastav_barvu_rgb(float *p_barva, float r, float g, float b, int cop)
{  
  if(cop == COLOR_SET) {
    p_barva[0] = r;
    p_barva[1] = g;
    p_barva[2] = b;
  }
  else if(cop == COLOR_ADD) {
    p_barva[0] += r;
    p_barva[1] += g;
    p_barva[2] += b;    
    
    if(p_barva[0] > 1.0f) r = 1.0f;
    if(p_barva[1] > 1.0f) g = 1.0f;
    if(p_barva[2] > 1.0f) b = 1.0f;    
  }      
  else {
    p_barva[0] -= r;
    p_barva[1] -= g;
    p_barva[2] -= b;    
    
    if(p_barva[0] < 0.0f) r = 0.0f;
    if(p_barva[1] < 0.0f) g = 0.0f;
    if(p_barva[2] < 0.0f) b = 0.0f;    
  }
}

__inline void nastav_barvu_rgba(float *p_barva, float r, float g, float b, float a, int cop)
{  
  if(cop == COLOR_SET) {
    p_barva[0] = r;
    p_barva[1] = g;
    p_barva[2] = b;
    p_barva[3] = a;
  }
  else if(cop == COLOR_ADD) {
    p_barva[0] += r;
    p_barva[1] += g;
    p_barva[2] += b;
    p_barva[3] += a;
    
    if(p_barva[0] > 1.0f) r = 1.0f;
    if(p_barva[1] > 1.0f) g = 1.0f;
    if(p_barva[2] > 1.0f) b = 1.0f;
    if(p_barva[3] > 1.0f) a = 1.0f;
  }      
  else {
    p_barva[0] -= r;
    p_barva[1] -= g;
    p_barva[2] -= b;
    p_barva[3] -= a;
    
    if(p_barva[0] < 0.0f) r = 0.0f;
    if(p_barva[1] < 0.0f) g = 0.0f;
    if(p_barva[2] < 0.0f) b = 0.0f;
    if(p_barva[3] < 0.0f) a = 0.0f;
  }
}

__inline void nastav_barvu_maska_rgba(float *p_barva, float r, float g, float b, float a, int cop, byte maska_r, byte maska_g, byte maska_b, byte maska_a)
{  
  if(cop == COLOR_SET) {
    if(maska_r) p_barva[0] = r;
    if(maska_g) p_barva[1] = g;
    if(maska_b) p_barva[2] = b;
    if(maska_a) p_barva[3] = a;
  }
  else if(cop == COLOR_ADD) {
    if(maska_r) p_barva[0] += r;
    if(maska_g) p_barva[1] += g;
    if(maska_b) p_barva[2] += b;
    if(maska_a) p_barva[3] += a;
    
    if(maska_r) if(p_barva[0] > 1.0f) r = 1.0f;
    if(maska_g) if(p_barva[1] > 1.0f) g = 1.0f;
    if(maska_b) if(p_barva[2] > 1.0f) b = 1.0f;
    if(maska_a) if(p_barva[3] > 1.0f) a = 1.0f;
  }      
  else {
    if(maska_r) p_barva[0] -= r;
    if(maska_g) p_barva[1] -= g;
    if(maska_b) p_barva[2] -= b;
    if(maska_a) p_barva[3] -= a;
    
    if(maska_r) if(p_barva[0] < 0.0f) r = 0.0f;
    if(maska_g) if(p_barva[1] < 0.0f) g = 0.0f;
    if(maska_b) if(p_barva[2] < 0.0f) b = 0.0f;
    if(maska_a) if(p_barva[3] < 0.0f) a = 0.0f;
  }
}

__inline void nastav_barvu_maska_rgb(float *p_barva, float r, float g, float b, int cop, byte maska_r, byte maska_g, byte maska_b)
{  
  if(cop == COLOR_SET) {
    if(maska_r) p_barva[0] = r;
    if(maska_g) p_barva[1] = g;
    if(maska_b) p_barva[2] = b;
  }
  else if(cop == COLOR_ADD) {
    if(maska_r) p_barva[0] += r;
    if(maska_g) p_barva[1] += g;
    if(maska_b) p_barva[2] += b;
    
    if(maska_r) if(p_barva[0] > 1.0f) r = 1.0f;
    if(maska_g) if(p_barva[1] > 1.0f) g = 1.0f;
    if(maska_b) if(p_barva[2] > 1.0f) b = 1.0f;
  }      
  else {
    if(maska_r) p_barva[0] -= r;
    if(maska_g) p_barva[1] -= g;
    if(maska_b) p_barva[2] -= b;
    
    if(maska_r) if(p_barva[0] < 0.0f) r = 0.0f;
    if(maska_g) if(p_barva[1] < 0.0f) g = 0.0f;
    if(maska_b) if(p_barva[2] < 0.0f) b = 0.0f;
  }
}

/* Gourandovo mapovani barev na objekt
*/
void obj_gourand_maping(K_EDITOR *p_cnf, EDIT_KONTEJNER *p_kont, EDIT_OBJEKT *p_obj, byte cop)
{
 GLMATRIX  *p_kw;
 MUJ_BOD   *p_bod;
 int       mod = p_cnf->mod;
 int       v;
 int       os = p_cnf->bar.oboustrany_stinovac;
 float     tx,ty,tz;
 float     px,py,pz;
 float     v1,v2,v3;
 float     uhel;
 float     r,g,b,a;
 float     rr,rg,rb,ra;

 if(p_kont->kflag&KONT_KEYFRAME)
   p_kw = &p_obj->m;
 else
   p_kw = kont_world_matrix(p_kont);   
 
 rr = p_cnf->bar.gr;
 rg = p_cnf->bar.gg;
 rb = p_cnf->bar.gb;
 ra = p_cnf->bar.ga;

 // z vektor pohledu
 v1 = p_cnf->p_camera->_13; 
 v2 = p_cnf->p_camera->_23; 
 v3 = p_cnf->p_camera->_33;
 
 norm_vect(&v1,&v2,&v3);
 
 p_bod = p_obj->p_vertex;
 for(v = 0; v < p_obj->vertexnum; v++) {
   px = p_bod[v].nx;
   py = p_bod[v].ny;
   pz = p_bod[v].nz;
   
   tx = p_kw->_11*px + p_kw->_21*py + p_kw->_31*pz;
   ty = p_kw->_12*px + p_kw->_22*py + p_kw->_32*pz;
   tz = p_kw->_13*px + p_kw->_23*py + p_kw->_33*pz;
   
   norm_vect(&tx,&ty,&tz);
      
   uhel = tx*v1+ty*v2+tz*v3;
   uhel = (p_cnf->bar.gourand_scale*(p_cnf->bar.gourand_pridat1+uhel))+p_cnf->bar.gourand_pridat2;
   
   // uhel je od 0 - 1
   // 
   //uhel += (rand()%1000)*0.0001f;
 
   if(os) {
     uhel = fabsf(uhel);
     r = (rr*uhel > 1.0f) ? 1.0f : rr*uhel;
     g = (rg*uhel > 1.0f) ? 1.0f : rg*uhel;
     b = (rb*uhel > 1.0f) ? 1.0f : rb*uhel;
     a = (ra*uhel > 1.0f) ? 1.0f : ra*uhel;
   } else {
     if(uhel > 0) {
       r = (rr*uhel > 1.0f) ? 1.0f : rr*uhel;
       g = (rg*uhel > 1.0f) ? 1.0f : rg*uhel;
       b = (rb*uhel > 1.0f) ? 1.0f : rb*uhel;
       a = (ra*uhel > 1.0f) ? 1.0f : ra*uhel;
     } else {
       r = g = b = a = 0;
     }
   }
   
   if(!p_cnf->bar.mod) {
     nastav_barvu_rgba(&p_bod[v].dr,r,g,b,a,cop);
     nastav_barvu_rgba(&p_bod[v].mdr,r,g,b,a,cop);
   } else {
     nastav_barvu_rgb(&p_bod[v].sr,r,g,b,cop);
     nastav_barvu_rgb(&p_bod[v].msr,r,g,b,cop);
   }
 }
}

void kont_gourand_maping(K_EDITOR *p_cnf, EDIT_KONTEJNER *p_kont_top, byte cop, int multi_mesh)
{
 EDIT_KONTEJNER *p_kont = p_kont_top;
 int       o;

 while(p_kont) {
   for(o = 0; o < p_kont->max_objektu; o++) {
     if(p_kont->p_obj[o]) {   
       obj_gourand_maping(p_cnf, p_kont, p_kont->p_obj[o], cop);
     }
   }
   if(multi_mesh)
     p_kont = p_kont->p_next;
   else
     break;
 }
}

/* Gourandovo mapovani barev na objekt
*/
void group_gourand_maping(K_EDITOR *p_cnf, byte cop)
{
 GLMATRIX  *p_kw;
 int       mod = p_cnf->mod;
 int       os = p_cnf->bar.oboustrany_stinovac;
 int       k,o,v,gr;
 float     tx,ty,tz;
 float     px,py,pz;
 float     v1,v2,v3;
 float     uhel;
 float     r,g,b,a;
 float     rr,rg,rb,ra;

 v1 = p_cnf->p_camera->_13; 
 v2 = p_cnf->p_camera->_23; 
 v3 = p_cnf->p_camera->_33;

 norm_vect(&v1,&v2,&v3);

 rr = p_cnf->bar.gr;
 rg = p_cnf->bar.gg;
 rb = p_cnf->bar.gb;
 ra = p_cnf->bar.ga;

 for(gr = 0; gr < p_cnf->groupnum; gr++) {
   k = p_cnf->p_group[gr].k;
   o = p_cnf->p_group[gr].o;
   v = p_cnf->p_group[gr].v;

   if(p_cnf->p_kont[k]->kflag&KONT_KEYFRAME) 
     p_kw = &p_cnf->p_kont[k]->p_obj[o]->m;
   else
     p_kw = kont_world_matrix(p_cnf->p_kont[k]);

   px = p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].nx;
   py = p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].ny;
   pz = p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].nz;   
   
   tx = p_kw->_11*px + p_kw->_21*py + p_kw->_31*pz;
   ty = p_kw->_12*px + p_kw->_22*py + p_kw->_32*pz;
   tz = p_kw->_13*px + p_kw->_23*py + p_kw->_33*pz;

   norm_vect(&tx,&ty,&tz);
   
   uhel = (tx*v1+ty*v2+tz*v3);
   uhel = (p_cnf->bar.gourand_scale*(p_cnf->bar.gourand_pridat1+uhel))+p_cnf->bar.gourand_pridat2;
   
   // uhel je od 0 - 1
   // 
   //uhel += (rand()%1000)*0.0001f;
   
   if(os) {
     uhel = fabsf(uhel);
     r = (rr*uhel > 1.0f) ? 1.0f : rr*uhel;
     g = (rg*uhel > 1.0f) ? 1.0f : rg*uhel;
     b = (rb*uhel > 1.0f) ? 1.0f : rb*uhel;
     a = (ra*uhel > 1.0f) ? 1.0f : ra*uhel;
   } else {
     if(uhel > 0) {
       r = (rr*uhel > 1.0f) ? 1.0f : rr*uhel;
       g = (rg*uhel > 1.0f) ? 1.0f : rg*uhel;
       b = (rb*uhel > 1.0f) ? 1.0f : rb*uhel;
       a = (ra*uhel > 1.0f) ? 1.0f : ra*uhel;
     } else {
       r = g = b = a = 0;
     }
   }
   
   if(!p_cnf->bar.mod) {
     nastav_barvu_rgba(&p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].dr,r,g,b,a,cop);
     nastav_barvu_rgba(&p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].mdr,r,g,b,a,cop);
   } else {
     nastav_barvu_rgb(&p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].sr,r,g,b,cop);
     nastav_barvu_rgb(&p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].msr,r,g,b,cop);
   }
 }
}


void oe_prikaz_gourand_maping_body(K_EDITOR *p_cnf)
{ 
  if(p_cnf->groupnum)
    group_gourand_maping(p_cnf,p_cnf->bar.gourand_cop);
}

void oe_prikaz_gourand_maping_obj(K_EDITOR *p_cnf)
{
  int k = p_cnf->vybr_kont;
  int o = p_cnf->vybr_objekt;
  n_objekt(k,o);
  obj_gourand_maping(p_cnf, p_cnf->p_kont[k], p_cnf->p_kont[k]->p_obj[o], p_cnf->bar.gourand_cop);
}

void oe_prikaz_gourand_maping_kont(K_EDITOR *p_cnf)
{
  int k = p_cnf->vybr_kont;
  n_kont(k);  
  kont_gourand_maping(p_cnf, p_cnf->p_kont[k], p_cnf->bar.gourand_cop,p_cnf->bar.multi_mesh);
}

/* ****************************************************************
  Normalove vektory
*/
void oe_prikaz_kont_norm_vect(K_EDITOR *p_cnf)
{ 
  int k = p_cnf->vybr_kont;  
  n_kont(k);
  kont_norm_vect(p_cnf->p_kont[k]);
}

void oe_prikaz_kont_norm_vect_all(K_EDITOR *p_cnf)
{ 
  int k,d = 0;
  
  kprintf(1,"Prepocitavam normalove vektory...");
  for(k = 0; k < p_cnf->max_kont; k++) {
    if(p_cnf->p_kont[k]) {
      kont_norm_vect(p_cnf->p_kont[k]);
    }
  }
}

/* *************************************************************************** 
  Prikazy nastavovace
*/

void oe_prikaz_set_barva_body(K_EDITOR *p_cnf)
{
 int   i,k,o,v;
 float r,g,b,a;
 
 r = p_cnf->bar.gr;
 g = p_cnf->bar.gg;
 b = p_cnf->bar.gb;
 a = p_cnf->bar.ga;

 for(i = 0; i < p_cnf->groupnum; i++) {
   k = p_cnf->p_group[i].k;
   o = p_cnf->p_group[i].o;
   v = p_cnf->p_group[i].v;

   if(!p_cnf->bar.mod) {
     nastav_barvu_rgba(&p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].dr,r,g,b,a,p_cnf->bar.set_operace);
     nastav_barvu_rgba(&p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].mdr,r,g,b,a,p_cnf->bar.set_operace);
   } else {
     nastav_barvu_rgb(&p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].sr,r,g,b,p_cnf->bar.set_operace);
     nastav_barvu_rgb(&p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].msr,r,g,b,p_cnf->bar.set_operace);
   }
 }
}

void oe_prikaz_set_barva_body_fast(K_EDITOR *p_cnf)
{ 
 if(!p_cnf->groupnum)
   return; 
 
 if(!doe_prikaz_vyber_barvu_float(p_cnf,hwnd_aplikace,&p_cnf->bar.gr))
   return; 

 // nastav i barvy v te liste
 if(p_cnf->bar.hDlg) {
   doe_printf(p_cnf->bar.hDlg,IDC_EDIT15,"%d",ftoi(p_cnf->bar.gr*MAX_BYTE_F));
   doe_printf(p_cnf->bar.hDlg,IDC_EDIT16,"%d",ftoi(p_cnf->bar.gg*MAX_BYTE_F));
   doe_printf(p_cnf->bar.hDlg,IDC_EDIT17,"%d",ftoi(p_cnf->bar.gb*MAX_BYTE_F));
   doe_printf(p_cnf->bar.hDlg,IDC_EDIT5, "%d",ftoi(p_cnf->bar.ga*MAX_BYTE_F));
 }

 oe_prikaz_set_barva_body(p_cnf);
}

void oe_prikaz_set_barva(K_EDITOR *p_cnf)
{ 
 if(!doe_prikaz_vyber_barvu_float(p_cnf,hwnd_aplikace,&p_cnf->bar.gr))
   return; 

 // nastav i barvy v te liste
 if(p_cnf->bar.hDlg) {
   doe_printf(p_cnf->bar.hDlg,IDC_EDIT15,"%d",ftoi(p_cnf->bar.gr*MAX_BYTE_F));
   doe_printf(p_cnf->bar.hDlg,IDC_EDIT16,"%d",ftoi(p_cnf->bar.gg*MAX_BYTE_F));
   doe_printf(p_cnf->bar.hDlg,IDC_EDIT17,"%d",ftoi(p_cnf->bar.gb*MAX_BYTE_F));
   doe_printf(p_cnf->bar.hDlg,IDC_EDIT5, "%d",ftoi(p_cnf->bar.ga*MAX_BYTE_F));
 }
 kprintf(1,"Barva R %d G %d B %d A %d",ftoi(p_cnf->bar.gr*MAX_BYTE_F),
                               ftoi(p_cnf->bar.gg*MAX_BYTE_F),
                               ftoi(p_cnf->bar.gb*MAX_BYTE_F),
                               ftoi(p_cnf->bar.ga*MAX_BYTE_F));
}


__inline static void oe_set_barva_objekt(EDIT_OBJEKT *p_obj, float r, float g, float b, float a, int mod, int op)
{
  int i;
  for(i = 0; i < p_obj->vertexnum; i++) {
    if(!mod) {
      nastav_barvu_rgba(&p_obj->p_vertex[i].dr,r,g,b,a,op);
      nastav_barvu_rgba(&p_obj->p_vertex[i].mdr,r,g,b,a,op);
    } else {
      nastav_barvu_rgb(&p_obj->p_vertex[i].sr,r,g,b,op);
      nastav_barvu_rgb(&p_obj->p_vertex[i].msr,r,g,b,op);
    }
  }
}

__inline static void oe_set_barva_objekt_maska(EDIT_OBJEKT *p_obj, float r, float g, float b, float a, int mod, int op, byte mr, byte mg, byte mb, byte ma)
{
  int i;
  for(i = 0; i < p_obj->vertexnum; i++) {
    if(!mod) {
      nastav_barvu_maska_rgba(&p_obj->p_vertex[i].dr,r,g,b,a,op,mr,mg,mb,ma);
      nastav_barvu_maska_rgba(&p_obj->p_vertex[i].mdr,r,g,b,a,op,mr,mg,mb,ma);
    } else {
      nastav_barvu_maska_rgb(&p_obj->p_vertex[i].sr,r,g,b,op,mr,mg,mb);
      nastav_barvu_maska_rgb(&p_obj->p_vertex[i].msr,r,g,b,op,mr,mg,mb);
    }
  }
}

void oe_prikaz_set_barva_objekt(K_EDITOR *p_cnf)
{
  int k = p_cnf->vybr_kont;
  int o = p_cnf->vybr_objekt;
  int i;
  
  if(p_cnf->groupfacenum) {
    if(!oe_je_group_objekt(p_cnf))
      kprintf(1,"Nejsou vybrane objekty!");
    else {
      for(i = 0; i < p_cnf->groupfacenum; i++) {
        k = p_cnf->p_group_face[i].k;
        o = p_cnf->p_group_face[i].o;
        oe_set_barva_objekt(p_cnf->p_kont[k]->p_obj[o],p_cnf->bar.gr,p_cnf->bar.gg,
          p_cnf->bar.gb, p_cnf->bar.ga, p_cnf->bar.mod, p_cnf->bar.set_operace);
      }
    }
  } else {
    n_objekt(k,o);
    oe_set_barva_objekt(p_cnf->p_kont[k]->p_obj[o],p_cnf->bar.gr,p_cnf->bar.gg,
      p_cnf->bar.gb, p_cnf->bar.ga, p_cnf->bar.mod, 
      p_cnf->bar.set_operace);
  }
}

void oe_prikaz_set_barva_kontejner(K_EDITOR *p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  int k = p_cnf->vybr_kont;
  int o,i,
      multi = p_cnf->bar.multi_mesh;

  if(p_cnf->groupfacenum) {
    if(!oe_je_group_kontejner(p_cnf))
      kprintf(1,"Neni vybran mod kontejnery...");
    else {
      for(i = 0; i < p_cnf->groupfacenum; i++) {
        k = p_cnf->p_group_face[i].k;
        p_kont = p_cnf->p_kont[k];
        while(p_kont) {
          for(o = 0; o < p_kont->max_objektu; o++) {
            if(p_kont->p_obj[o]) {
              oe_set_barva_objekt(p_kont->p_obj[o],p_cnf->bar.gr,p_cnf->bar.gg,
                p_cnf->bar.gb, p_cnf->bar.ga, p_cnf->bar.mod, 
                p_cnf->bar.set_operace);
            }
          }
          if(multi)
            p_kont = p_kont->p_next;
          else
            break;
        }
      }
    }
  } else {
    n_kont(k);
    p_kont = p_cnf->p_kont[k];
    
    while(p_kont) {
      for(o = 0; o < p_kont->max_objektu; o++) {
        if(p_kont->p_obj[o]) {
          oe_set_barva_objekt(p_kont->p_obj[o],p_cnf->bar.gr,p_cnf->bar.gg,
            p_cnf->bar.gb, p_cnf->bar.ga, p_cnf->bar.mod, 
            p_cnf->bar.set_operace);
        }
      }
      if(multi)
        p_kont = p_kont->p_next;
      else
        break;
    }
  }
}

void oe_prikaz_set_barva_scena(K_EDITOR *p_cnf)
{
  int vybr = p_cnf->vybr_kont;
  int k;      

  for(k = 0; k < KONT_NUM; k++) {
    if(p_cnf->p_kont[k]) {
      p_cnf->vybr_kont = k;
      oe_prikaz_set_barva_kontejner(p_cnf);
    }
  }
  
  p_cnf->vybr_kont = vybr;
}

void oe_prikaz_set_barva_scena_guids(K_EDITOR *p_cnf, int lo, int hi, int stat)
{  
  B_KONFIG *p_bnf = &p_cnf->bnf;
  int vybr = p_cnf->vybr_kont;
  int i,k,guid;
  
  for(i = 0; i < MAX_PRVKU_LEVEL; i++) {
    if(p_bnf->p_prv_lev[i]) {
      guid = p_bnf->p_prv_lev[i]->guid;
      if(!guid)
        guid = p_bnf->p_prv_lev[i]->p_prv_databaze->guid;
      if((stat && ber_je_objekt_staticky_bez_exitu(guid)) || (lo <= guid && guid <= hi)) {
        k = p_bnf->p_prv_lev[i]->mesh;
        if(p_cnf->p_kont[k]) {
          p_cnf->vybr_kont = k;
          oe_prikaz_set_barva_kontejner(p_cnf);
        }
      }
    }
  }
  
  p_cnf->vybr_kont = vybr;
}

// importuje barvu z materialu
void oe_prikaz_reset_barva_objekt(K_EDITOR *p_cnf)
{
  int   k = p_cnf->vybr_kont;
  int   o = p_cnf->vybr_objekt;
  int   m;
  float r,g,b,a;
  
  n_objekt(k,o);
  
  m = p_cnf->p_kont[k]->p_obj[o]->material;

  if(!p_cnf->bar.mod) {
    r = p_cnf->p_mat[m]->dxmat.diffuse_r;
    g = p_cnf->p_mat[m]->dxmat.diffuse_g;
    b = p_cnf->p_mat[m]->dxmat.diffuse_b;
    a = p_cnf->p_mat[m]->dxmat.diffuse_a;
  } else {
    r = p_cnf->p_mat[m]->dxmat.specular_r;
    g = p_cnf->p_mat[m]->dxmat.specular_g;
    b = p_cnf->p_mat[m]->dxmat.specular_b;
    a = 1.0f;
  }

  oe_set_barva_objekt(p_cnf->p_kont[k]->p_obj[o],r,g,b,a,p_cnf->bar.mod,
                      COLOR_SET);
}

void oe_prikaz_reset_barva_kontejner(K_EDITOR *p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  int   k = p_cnf->vybr_kont;
  int   o,m,
        multi = p_cnf->bar.multi_mesh;
  float r,g,b,a;
  
  n_kont(k);
  p_kont = p_cnf->p_kont[k];

  while(p_kont) {
    for(o = 0; o < p_cnf->p_kont[k]->max_objektu; o++) {
      if(p_cnf->p_kont[k]->p_obj[o]) {
        m = p_cnf->p_kont[k]->p_obj[o]->material;
        if(!p_cnf->bar.mod) {
          r = p_cnf->p_mat[m]->dxmat.diffuse_r;
          g = p_cnf->p_mat[m]->dxmat.diffuse_g;
          b = p_cnf->p_mat[m]->dxmat.diffuse_b;
          a = p_cnf->p_mat[m]->dxmat.diffuse_a;
        } else {
          r = p_cnf->p_mat[m]->dxmat.specular_r;
          g = p_cnf->p_mat[m]->dxmat.specular_g;
          b = p_cnf->p_mat[m]->dxmat.specular_b;
          a = 1.0f;
        }
        oe_set_barva_objekt(p_cnf->p_kont[k]->p_obj[o],r,g,b,a,p_cnf->bar.mod,
          COLOR_SET);
      }
    }
    if(multi)
      p_kont = p_kont->p_next;
    else
      break;
  }
}

void oe_reset_barva_scena_all(K_EDITOR *p_cnf)
{
  int   k,v = p_cnf->vybr_kont;
  
  for(k = 0; k < KONT_NUM; k++) {
    if(p_cnf->p_kont[k]) {
      p_cnf->vybr_kont = k;
      oe_prikaz_reset_barva_kontejner(p_cnf);
    }
  }  
  p_cnf->vybr_kont = v;
}

void oe_reset_barva_scena(K_EDITOR *p_cnf, EDIT_MATERIAL *p_mat, int matnum, int barva)
{
  EDIT_KONTEJNER *p_kont;
  float r,g,b,a;
  int   o,k;

  if(!barva) {
    r = p_mat->dxmat.diffuse_r;
    g = p_mat->dxmat.diffuse_g;
    b = p_mat->dxmat.diffuse_b;
    a = p_mat->dxmat.diffuse_a;
  } else {
    r = p_mat->dxmat.specular_r;
    g = p_mat->dxmat.specular_g;
    b = p_mat->dxmat.specular_b;
    a = 1.0f;
  }

  for(k = 0; k < KONT_NUM; k++) {
    if(p_kont = p_cnf->p_kont[k]) {
      for(o = 0; o < p_kont->max_objektu; o++) {    
        if(p_kont->p_obj[o] && p_kont->p_obj[o]->material == matnum) {
          oe_set_barva_objekt(p_kont->p_obj[o],r,g,b,a,barva,COLOR_SET);
        }
      }
    }
  }
}

void oe_reset_barva_scena_alfa(K_EDITOR *p_cnf, EDIT_MATERIAL *p_mat, int matnum)
{
  EDIT_KONTEJNER *p_kont;
  float r,g,b,a;
  int   o,k;

  r = p_mat->dxmat.diffuse_r;
  g = p_mat->dxmat.diffuse_g;
  b = p_mat->dxmat.diffuse_b;
  a = p_mat->dxmat.diffuse_a;

  for(k = 0; k < KONT_NUM; k++) {
    if(p_kont = p_cnf->p_kont[k]) {
      for(o = 0; o < p_kont->max_objektu; o++) {    
        if(p_kont->p_obj[o] && p_kont->p_obj[o]->material == matnum) {
          oe_set_barva_objekt_maska(p_kont->p_obj[o],r,g,b,a,0,COLOR_SET,0,0,0,1);
        }
      }
    }
  }
}

/* Oznacene plosky vyrizne jako extra-plosky
   - musi byt oznacene plosky pouze 1 objektu
*/
void oe_prikaz_vyrizni_plosky(K_EDITOR *p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  EDIT_OBJEKT *p_obj;  
  OBJ_VERTEX *p_vertex;  
  int  vertexnum;
  int  vertexakt;
  int  i;
  int  k,o,f,fg;
  
  if(!p_cnf->groupfacenum) {
    kprintf(TRUE,"Nejsou vybrane zadne plosky...");
    return;
  }
  
  k = p_cnf->p_group_face[0].k;
  o = p_cnf->p_group_face[0].o;

  // 1 kontejner, 1 objekt
  for(i = 1; i < p_cnf->groupfacenum; i++) {
    if(k != p_cnf->p_group_face[i].k || o != p_cnf->p_group_face[i].o) {
      kprintf(TRUE,"Jsou vybrane plosky vice objektu/kontejneru...");
      return;
    }
  }

  if(!(p_kont = p_cnf->p_kont[k]) || !(p_obj = p_cnf->p_kont[k]->p_obj[o])) {
    kprintf(TRUE,"Bordel!");
    return;
  }

  vertexnum = p_obj->vertexnum+p_cnf->groupfacenum*3;
  vertexakt = p_obj->vertexnum;

  p_vertex = malloc(sizeof(p_vertex[0])*vertexnum);
  mtest(p_vertex);
  memcpy(p_vertex,p_obj->p_vertex,sizeof(p_vertex[0])*vertexakt);
  
  for(fg = 0; fg < p_cnf->groupfacenum; fg++) {
    f = p_cnf->p_group_face[fg].f;
    p_vertex[vertexakt] = p_vertex[p_obj->p_face[f]];
    p_obj->p_face[f] = vertexakt++;
    p_vertex[vertexakt] = p_vertex[p_obj->p_face[f+1]];
    p_obj->p_face[f+1] = vertexakt++;
    p_vertex[vertexakt] = p_vertex[p_obj->p_face[f+2]];
    p_obj->p_face[f+2] = vertexakt++;
  }

  assert(vertexnum == vertexakt);

  free(p_obj->p_vertex);
  p_obj->p_vertex = p_vertex;
  p_obj->vertexnum = vertexnum;

  vycisti_kontejner(p_kont);
}

/* **************************************************************************
  // vybrane face objektu oriznu a vyrobim jako extra objekt
  // Vyrizne oznacene facy pomoci bodu jako extra objekt daneho kontejneru
*/
void oe_prikaz_vyrizni_face(K_EDITOR *p_cnf, int crop)
{
  EDIT_OBJEKT *p_obj;
  EDIT_OBJEKT *p_new;
  int  *p_vertex;
  int   k,o,v,i,f,vnum,fnum;
  int  *p_face,*p_delete,del = 0;
  
  if(!p_cnf->groupnum) {
    ddw("Musi byt oznaceny face, ktery se maji vyriznout ! Pouzij klavesy F a CTRL+F na oznaceni facu pomoci bodu !");
    return;
  }

  k = p_cnf->p_group[0].k; //pouziju nulu
  o = p_cnf->p_group[0].o;
  p_obj = p_cnf->p_kont[k]->p_obj[o];

  p_face = _alloca(sizeof(int)*p_obj->facenum);
  memset(p_face,K_CHYBA,sizeof(sizeof(int))*p_obj->facenum);

  if(crop) {  
    p_delete = _alloca(sizeof(int)*p_obj->facenum/3);
    memset(p_delete,0,sizeof(sizeof(int))*p_obj->facenum/3);
  }
  
  p_vertex = _alloca(sizeof(int)*p_obj->vertexnum);
  memset(p_vertex,K_CHYBA,sizeof(sizeof(int))*p_obj->vertexnum);

  // vyberu facy ktere jsou skutecne pouzity
  for(i = 0; i < p_cnf->groupnum; i++) {
    if(p_cnf->p_group[i].k == k && p_cnf->p_group[i].o == o) {
      v = p_cnf->p_group[i].v;
      for(f = 0; f < p_obj->facenum; f++) {
        if(p_obj->p_face[f] == v)
          p_face[f] = v;
      }
    }
  }

  // skopiruju face
  fnum = 0;
  for(f = 0, i = 0; f < p_obj->facenum; f += 3) {
    if(p_face[f] != K_CHYBA && p_face[f+1] != K_CHYBA && p_face[f+2] != K_CHYBA){ 
      if(crop) {
        p_delete[del++] = f;
      }
      assert(i <= f);
      if(i != f) {
        p_face[i]   = p_face[f]; // setridit
        p_face[i+1] = p_face[f+1];
        p_face[i+2] = p_face[f+2];
      }
      i += 3;      
    }
  }

  assert(i > 0);
  fnum = i;
  vnum = 0;
  // najdu pocet jedinecnych bodu
  for(f = 0; f < i; f++) {
    for(v = 0; v < vnum; v++) {
      if(p_vertex[v] == p_face[f]) {
        p_face[f] = v;
        goto dale;
      }
    }
    p_vertex[vnum] = p_face[f];
    p_face[f] = vnum++;
    dale:;
  }
  
  if(crop) {
    for(f = del-1; f >= 0; f--) {
      smaz_face_bez_vertexu(p_obj->p_face,p_obj->facenum,p_delete[f]);
    }
    smaz_divoke_vertexy_objektu_dir(p_obj);
  }

  if(!(p_new = vyrob_objekt_komplet(vnum, fnum))) {
    assert(p_new);
    chyba("Pamet");
  }

  for(v = 0; v < vnum; v++) {
    p_new->p_vertex[v] = p_obj->p_vertex[p_vertex[v]];
  }

  for(f = 0; f < fnum; f++) {
    p_new->p_face[f] = p_face[f];
  }

  p_new->material = p_obj->material;
  p_new->m1flag = p_obj->m1flag;
  p_new->m2flag = p_obj->m2flag;
  p_new->oflag = p_obj->oflag;
  strcpy(p_new->jmeno,"Vyrez");
 
  if((o = lo_najdi_volny_kontejner(p_cnf->p_kont,p_cnf->max_kont)) == K_CHYBA) {
    ddw("Neni volny kontejner !");
    return;
  }

  p_cnf->p_kont[o] = vyrob_kontejner();
  p_cnf->p_kont[o]->p_obj[0] = p_new;
  strcpy(p_cnf->p_kont[o]->jmeno,"Vyrez");
  p_cnf->p_kont[o]->m1flag = p_cnf->p_kont[k]->m1flag;
  p_cnf->p_kont[o]->m2flag = p_cnf->p_kont[k]->m2flag;
  p_cnf->p_kont[o]->kflag = p_cnf->p_kont[k]->kflag;
  p_cnf->p_kont[o]->world = p_cnf->p_kont[k]->world;
  updatuj_kontejner_statistika(p_cnf->p_kont[o],TRUE);
  if(crop)
    updatuj_kontejner_statistika(p_cnf->p_kont[k],TRUE);
}

/*
void oe_prikaz_smaz_face(K_EDITOR *p_cnf)
{
 int   i;
 int   f,o,k;
 FACE *p_face;


 if(p_cnf->groupfacenum) {
   for(i = 0; i < p_cnf->groupfacenum; i++) {
     k = p_cnf->p_group_face[i].k;
     o = p_cnf->p_group_face[i].o;
     f = p_cnf->p_group_face[i].f;
     n_objekt(k,o);
     if(p_cnf->p_kont[k]->p_obj[o]->facenum) {
       p_face = p_cnf->p_kont[k]->p_obj[o]->p_face;
       smaz_face_bez_vertexu(p_face,p_cnf->p_kont[k]->p_obj[o]->facenum,f);
       face_korekce(p_cnf->p_group_face+i+1, p_cnf->groupfacenum-i-1, k, o, f);
       p_cnf->vybr_face = 0;
     }
   }
   for(i = 0; i < p_cnf->groupfacenum; i++) {
     k = p_cnf->p_group_face[i].k;
     if(p_cnf->p_kont[k]) {
       vycisti_kontejner(p_cnf->p_kont[k]);
       updatuj_kontejner_statistika(p_cnf->p_kont[k]);
       if(!p_cnf->p_kont[k]->objektu) {
         lo_vymaz_svetla_ze_sceny(p_cnf->p_kont[k]);
         zrus_kontejner_rec(&p_cnf->p_kont[k]);
       }
     }
   }
   oe_ungroup_face_all(p_cnf);
   oe_updatuj_scenu_stat(p_cnf);
 }
}
*/

// vyrizne vybrane face jako extra objekt
EDIT_OBJEKT * oe_vyrizni_face_objekt(K_EDITOR *p_cnf, int smaz_puvodni)
{
  int          vertexnum;
  int          vertexakt;
  I_VERTEX    *p_vertex;
  word        *p_face;
  int          facenum,f1;
  EDIT_OBJEKT *p_obj;
  int          i,k,o,f,v;


  if(!p_cnf->groupfacenum)
    return(NULL);

  vertexnum = p_cnf->groupfacenum*3;
  vertexakt = 0;  
  p_vertex = mmalloc(sizeof(p_vertex[0])*vertexnum);
  facenum = vertexnum;
  p_face = mmalloc(sizeof(p_face[0])*facenum);

  for(i = 0; i < p_cnf->groupfacenum; i++) {
    k = p_cnf->p_group_face[i].k;
    o = p_cnf->p_group_face[i].o;
    f = p_cnf->p_group_face[i].f;
        
    f1 = p_cnf->p_kont[k]->p_obj[o]->p_face[f];
    v = najdi_group_vertex(p_vertex, vertexakt, k, o, f1);
    if(v == K_CHYBA) {
      v = vertexakt++;
      assert(v < vertexnum);
      p_vertex[v].k = k;
      p_vertex[v].o = o;
      p_vertex[v].v = f1;
    }
    p_face[i*3] = v;


    f1 = p_cnf->p_kont[k]->p_obj[o]->p_face[f+1];
    v = najdi_group_vertex(p_vertex, vertexakt, k, o, f1);
    if(v == K_CHYBA) {
      v = vertexakt++;
      assert(v < vertexnum);
      p_vertex[v].k = k;
      p_vertex[v].o = o;
      p_vertex[v].v = f1;
    }
    p_face[i*3+1] = v;


    f1 = p_cnf->p_kont[k]->p_obj[o]->p_face[f+2];
    v = najdi_group_vertex(p_vertex, vertexakt, k, o, f1);
    if(v == K_CHYBA) {
      v = vertexakt++;
      assert(v < vertexnum);
      p_vertex[v].k = k;
      p_vertex[v].o = o;
      p_vertex[v].v = f1;
    }
    p_face[i*3+2] = v;
  }

  p_obj = vyrob_objekt_komplet(vertexakt,facenum);
  for(i = 0; i < vertexakt; i++) {
    k = p_vertex[i].k;
    o = p_vertex[i].o;
    v = p_vertex[i].v;
    p_obj->p_vertex[i] = p_cnf->p_kont[k]->p_obj[o]->p_vertex[v];
  }
  
  memcpy(p_obj->p_face,p_face,sizeof(p_face[0])*facenum);
  k = p_cnf->p_group_face[0].k;
  o = p_cnf->p_group_face[0].o;
  p_obj->material = p_cnf->p_kont[k]->p_obj[o]->material;
  sprintf(p_obj->jmeno,"f-vyrez%d",o);

  free(p_face);
  free(p_vertex);

  if(smaz_puvodni) {
    for(i = 0; i < p_cnf->groupfacenum; i++) {
      k = p_cnf->p_group_face[i].k;
      o = p_cnf->p_group_face[i].o;
      if(p_cnf->p_kont[k]->p_obj[o]) {
        zrus_objekt(p_cnf->p_kont[k]->p_obj+o);
        updatuj_kontejner_statistika(p_cnf->p_kont[k],TRUE);
      }
    }
  }

  return(p_obj);
}

void oe_prikaz_vyrizni_objekt(K_EDITOR *p_cnf, int crop)
{
  EDIT_OBJEKT *p_obj = oe_vyrizni_face_objekt(p_cnf,crop);
  EDIT_KONTEJNER *p_kont = p_cnf->p_kont[p_cnf->vybr_kont];
  int l;
  if(p_obj) {
    if(!crop)
      oe_prikaz_smaz_face(p_cnf,TRUE);
    l = lo_najdi_volny_objekt_kont(p_kont);
    if(l != K_CHYBA) {
      p_kont->p_obj[l] = p_obj;
      updatuj_kontejner_statistika(p_kont,TRUE);
    }
    kprintf(FALSE,"Vyrez objektu %s",p_obj->jmeno);
    oe_ungroup_face_all(p_cnf);
  }
}

void oe_prikaz_vyrizni_kontejner(K_EDITOR *p_cnf, int crop)
{
  EDIT_OBJEKT *p_obj = oe_vyrizni_face_objekt(p_cnf,crop);
  int l;
  if(p_obj) {
    oe_prikaz_smaz_face(p_cnf,TRUE);
    l = lo_najdi_volny_kontejner(p_cnf->p_kont,KONT_NUM);
    if(l != K_CHYBA) {
      p_cnf->p_kont[l] = vyrob_kontejner();
      p_cnf->p_kont[l]->p_obj[0] = p_obj;
      updatuj_kontejner_statistika(p_cnf->p_kont[l],TRUE);
    }
  }
}

void oe_group_smaz_stejne(K_EDITOR *p_cnf)
{
  I_VERTEX  *p_group;

  BOD p,*p_bod;
  int i,j,k,o,v,m;

  m = 0;
  for(i = 0; i < p_cnf->groupnum; i++) {
    k = p_cnf->p_group[i].k;
    o = p_cnf->p_group[i].o;
    v = p_cnf->p_group[i].v;

    p.x = p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].x;
    p.y = p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].y;
    p.z = p_cnf->p_kont[k]->p_obj[o]->p_vertex[v].z;

    for(j = i+1; j < p_cnf->groupnum; j++) {

      k = p_cnf->p_group[j].k;
      o = p_cnf->p_group[j].o;
      v = p_cnf->p_group[j].v;

      p_bod = (BOD *)p_cnf->p_kont[k]->p_obj[o]->p_vertex+v;
      
      if(p.x == p_bod->x && p.y == p_bod->y && p.z == p_bod->z) {
        p_cnf->p_group[j].k = -1;
        m++;
      }
    }
  }

  if(m) {
    p_group = malloc(sizeof(p_group[0])*(p_cnf->groupnum-m));
    mtest(p_group);
    
    for(m = 0, i = 0; i < p_cnf->groupnum; i++) {
      if(p_cnf->p_group[i].k != -1) {
        p_group[m].k = p_cnf->p_group[i].k;
        p_group[m].o = p_cnf->p_group[i].o;
        p_group[m].v = p_cnf->p_group[i].v;
        m++;
      }
    }
    null_free(&p_cnf->p_group);
    p_cnf->p_group = p_group;
    p_cnf->groupnum = m;
    p_cnf->vybr_group = 0;
  }
}

void oe_zrcadlo_prehod_poradi_bodu(K_EDITOR *p_cnf)
{
  BOD p = p_cnf->zrcadlo.p[1];
  p_cnf->zrcadlo.p[1] = p_cnf->zrcadlo.p[2];
  p_cnf->zrcadlo.p[2] = p;
}

void oe_zrcadlo_aktivuj_body(K_EDITOR *p_cnf)
{
  int k,o,v,i;

  if(p_cnf->groupnum < 4) {
    kprintf(TRUE,"Chyba -> Zrcadlo musi mit 4 body! Vybranych bodu %d",p_cnf->groupnum);
    return;
  }

  oe_group_smaz_stejne(p_cnf);

  p_cnf->zdcadlo_akt = TRUE;

  /*
    Klipovaci pyramida a,b,c,d -> 0,1,2,3
  */
  for(i = 0; i < 4; i++) {
    k = p_cnf->p_group[i].k;
    o = p_cnf->p_group[i].o;
    v = p_cnf->p_group[i].v;
    mujbod2bod(p_cnf->zrcadlo.p+i,p_cnf->p_kont[k]->p_obj[o]->p_vertex+v);
  }
  setrid_body_ctverec_bod(p_cnf->zrcadlo.p);
  calc_reflex_matrix(&p_cnf->zrcadlo.ref,p_cnf->zrcadlo.p+0,
                      p_cnf->zrcadlo.p+1,p_cnf->zrcadlo.p+2);
}

void oe_zrcadlo_aktivuj_plocha(K_EDITOR *p_cnf)
{ 
  ZDRCADLO_DESC_POLY *p_tmp, *p_first = NULL;  
  EDIT_MATERIAL *p_mat;
  int i,k;

  if(!p_cnf->groupfacenum) {
    kprintf(TRUE,"Musi byt vybrane kontejnery pro aktivaci zrcadla...");
    return;
  }

  if(!oe_je_group_kontejner(p_cnf)) {
    kprintf(TRUE,"Face-mod musi byt kontejnery...");
    return;
  }

  oe_zrcadlo_zrus_poly(p_cnf);

  for(i = 0; i < p_cnf->groupfacenum; i++) {
    k = p_cnf->p_group_face[i].k;
    if(!p_cnf->p_kont[k]) {
      kprintf(TRUE,"V seznamu je neaktivni kontejner...");
      continue;
    }
    if(!(p_cnf->p_kont[k]->kflag&KONT_STATIC)) {
      kprintf(TRUE,"V seznamu je nestaticky kontejner %d...",k);
      continue;
    }
    transformuj_kontejner(p_cnf->p_kont[k]);
    p_cnf->p_kont[k]->kflag |= KONT_ZRCADLO;


    p_tmp = mmalloc(sizeof(p_tmp[0]));

    p_tmp->zrcadlo_k = k;
    p_tmp->zrcadlo_o = lo_najdi_objekt_kont_plny(p_cnf->p_kont[k]);
    p_tmp->poly = K_CHYBA;

    p_mat = p_cnf->p_mat[p_cnf->p_kont[k]->p_obj[p_tmp->zrcadlo_o]->material];     
    if(p_mat) {
      p_mat->alfa_state = 3;
      p_mat->flag |= MAT_PRUHLEDNY;
    }    
  
    p_tmp->p_next = p_first;
    p_first = p_tmp;
  }
  
  p_cnf->zrcadlo.p_poly = p_first;
}


void oe_calc_clip_focus(K_EDITOR *p_cnf)
{
  BOD cp,*p_a,*p_b,*p_c,*p_d;
  
  cp.x = p_cnf->p_invcam->_41;
  cp.y = p_cnf->p_invcam->_42;
  cp.z = p_cnf->p_invcam->_43;    

  // 4 body zrcadla
  p_a = p_cnf->zrcadlo.p;
  p_b = p_cnf->zrcadlo.p+1;
  p_c = p_cnf->zrcadlo.p+2;
  p_d = p_cnf->zrcadlo.p+3;
  
  calc_drovinu_bod( p_b, p_a, &cp, p_cnf->zrcadlo.r);
  
  calc_drovinu_bod( p_d, p_b, &cp, p_cnf->zrcadlo.r+1);
  calc_drovinu_bod( p_c, p_d, &cp, p_cnf->zrcadlo.r+2);
  calc_drovinu_bod( p_a, p_c, &cp, p_cnf->zrcadlo.r+3);

  calc_drovinu_bod( p_a, p_c, p_b, p_cnf->zrcadlo.r+4);
}

void oe_zrcadlo_zrus_poly(K_EDITOR *p_cnf)
{
  ZDRCADLO_DESC_POLY *p_tmp, *p_rtmp;  

  p_tmp = p_cnf->zrcadlo.p_poly;
  while(p_tmp) {
    p_cnf->p_kont[p_tmp->zrcadlo_k]->p_obj[p_tmp->zrcadlo_o]->kreslit = TRUE;
    p_cnf->p_kont[p_tmp->zrcadlo_k]->p_obj[p_tmp->zrcadlo_o]->oflag &= ~KONT_ZRCADLO;
    p_cnf->p_kont[p_tmp->zrcadlo_k]->kflag &= ~KONT_ZRCADLO;

    p_rtmp = p_tmp;
    p_tmp = p_tmp->p_next;
    free(p_rtmp);
  }    
  p_cnf->zrcadlo.p_poly = NULL;
}

void oe_zrcadlo_zrus(K_EDITOR *p_cnf)
{
  oe_zrcadlo_zrus_poly(p_cnf);
  p_cnf->zdcadlo_akt = FALSE;
}

// true - je tam
int oe_je_zaznam_iobjekt(I_OBJEKT *p_obj, int max, word k, word o)
{
 int i = 0;

 while(p_obj[i].k != k && p_obj[i].o != o && i++ < max);

 return(i == max);
}


void oe_prikaz_env_mapping(K_EDITOR *p_cnf)
{
  int k = p_cnf->vybr_kont;
  n_kont(k);
  kont_env_maping(p_cnf, p_cnf->p_kont[k]);
}

void oe_prikaz_env_mapping_all(K_EDITOR *p_cnf)
{
  int k;
  
  for(k = 0; k < p_cnf->max_kont; k++) {
    if(p_cnf->p_kont[k]) {
      kont_env_maping(p_cnf, p_cnf->p_kont[k]);
    }
  }
}

void kont_env_maping(K_EDITOR *p_cnf, EDIT_KONTEJNER *p_kont)
{
 GLMATRIX m;
 int       mod = p_cnf->mod;
 int       o,v;
 float     tx,ty,tz;
 float     m11,m21,m31,m12,m22,m32;

 mat_mult(p_cnf->p_word, p_cnf->p_camera, &m);

 m11 = m._11; m21 = m._21; m31 = m._31;
 m12 = m._12; m22 = m._22; m32 = m._32;

 for(o = 0; o < p_kont->max_objektu; o++) {
   if(!p_kont->p_obj[o]) continue;
   for(v = 0; v < p_kont->p_obj[o]->vertexnum; v++) {
     tx = p_kont->p_obj[o]->p_vertex[v].nx;
     ty = p_kont->p_obj[o]->p_vertex[v].ny;
     tz = p_kont->p_obj[o]->p_vertex[v].nz;

     p_kont->p_obj[o]->p_vertex[v].tu2 = 0.5f*(1.0f+( tx*m11 + ty*m21 + tz*m31 )); // x bez posunu
     p_kont->p_obj[o]->p_vertex[v].tv2 = 0.5f*(1.0f+( tx*m12 + ty*m22 + tz*m32 )); // y bez posunu
   }
 }
}

int oe_vloz_flare(K_EDITOR *p_cnf, LENS_FLARE *p_flare)
{  
  p_cnf->flare[p_flare->index] = *p_flare;
  kprintf(1,"flare %d upraven.",p_flare->index);
  return(p_flare->index);
}


void oe_prikaz_pridej_flare(K_EDITOR *p_cnf)
{
 int f = lo_najdi_volny_flare(p_cnf->flare,MAX_EDIT_SVETEL);
 LENS_FLARE flare;

 memset(&flare,0,sizeof(flare));
 flare.r = flare.g = flare.b = 1.0f; flare.a = 0.3f;
 flare.dx = flare.dy = 1;
 flare.index = f;
 flare.akt = TRUE;
 flare.alfa = GL_ONE;
 if(doe_prikaz_flare(hwnd_aplikace,"Pridat flare...", &flare)) {
   oe_vloz_flare(p_cnf, &flare);   
 }
}

void oe_uprav_flare(K_EDITOR *p_cnf, LENS_FLARE *p_flare)
{  
 LENS_FLARE flare = *p_flare;
 byte pom[200];
 if(p_flare->akt) {
   sprintf(pom,"Uprav flare %d...",p_flare->index);
   if(doe_prikaz_flare(hwnd_aplikace, pom, &flare))
     oe_vloz_flare(p_cnf, &flare);
 } else 
   kprintf(1,"Neni vybrane zadne flare pro upravu !");
}

void oe_prikaz_smaz_flare(K_EDITOR *p_cnf)
{
 int f = p_cnf->flareakt; 
 p_cnf->flare[f].akt = FALSE;
}


/****************************************************
   Dynamicky svetla
  ***************************************************
*/
int oe_najdi_prepocitej_dsvetlo(K_EDITOR *p_cnf)
{
 int i;
 p_cnf->dl_lightnum = -1;

 for(i = 0; i < MAX_FLARE_SVETEL; i++) {
   if(p_cnf->p_dlight[i].akt && i > p_cnf->dl_lightnum) {
     p_cnf->dl_lightnum = i;
   }     
 }  
 return(++p_cnf->dl_lightnum);
}

int oe_najdi_volne_dsvetlo(K_EDITOR *p_cnf)
{
 int i;

 for(i = 0; i < MAX_FLARE_SVETEL; i++) {
   if(!p_cnf->p_dlight[i].akt)
     return(i);
 }
 return(K_CHYBA); 
}

void oe_prikaz_dsvetlo_nove(K_EDITOR *p_cnf)
{
  DYN_LIGHT *p_dlight;
  byte pom[200];
  int  v,k = p_cnf->vybr_kont;

  if(!p_cnf->lc.scena_light) 
    n_kont(k);

  v = lo_najdi_volne_dsvetlo(p_cnf->p_dlight,MAX_FLARE_SVETEL);
  if(v != K_CHYBA) {
    p_dlight = p_cnf->p_dlight+v;
    sprintf(p_dlight->jmeno,"DSvetlo %d",v);
    p_dlight->flag = SDL_UTLUM_LINEAR;
    p_dlight->dr = p_dlight->dg = p_dlight->db = 1.0f;
    p_dlight->dosah = p_dlight->max = 10.0f;
    p_dlight->index = v;

    if(p_cnf->lc.scena_light) {
      strcpy(pom,"Zadej nove svetlo sceny ty prdolo!");
      p_dlight->p_mesh_data = NULL;
    } else {
      sprintf(pom,"Nove svetlo kontejneru %d '%s'",k,p_cnf->p_kont[k]->jmeno);
      p_dlight->flag |= SDL_MESH;
      p_dlight->p_mesh_data = p_cnf->p_kont[k];
      p_dlight->p_next = p_cnf->p_kont[k]->p_dlight;
      p_dlight->p_prev = NULL;
      p_cnf->p_kont[k]->p_dlight = p_dlight;
      if(p_dlight->p_next) {
        p_dlight->p_next->p_prev = p_dlight;
      }
    }
        
    if(doe_prikaz_dynamicke_svetlo(hwnd_aplikace,pom,p_dlight)) {
      p_cnf->dl_akt = v;
      p_dlight->akt = TRUE;
      oe_najdi_prepocitej_dsvetlo(p_cnf);
      if(p_cnf->cnf.svetla_dynamicke) {
        ray_prepocitej_staticke_osvetleni_vertexy(p_cnf);
      }
    }    
  }
}

void oe_prikaz_dsvetlo_uprav(K_EDITOR *p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  DYN_LIGHT *p_dlight;
  byte pom[200];
  int  v = p_cnf->dl_akt;
  
  if(v < MAX_FLARE_SVETEL && p_cnf->p_dlight[v].akt) {
    p_dlight = p_cnf->p_dlight+v;
    if(p_cnf->lc.scena_light) {
      strcpy(pom,"Uprav svetlo sceny ty prdolo!");
      doe_prikaz_dynamicke_svetlo(hwnd_aplikace,pom,p_dlight);
    } else {
      p_kont = p_dlight->p_mesh_data;
      if(p_kont) {
        sprintf(pom,"Upravit svetlo kontejneru '%s'",p_kont->jmeno);
        doe_prikaz_dynamicke_svetlo(hwnd_aplikace,pom,p_dlight);
        if(p_cnf->cnf.svetla_dynamicke) {
          ray_prepocitej_staticke_osvetleni_vertexy(p_cnf);
        }
      }
    }
  }
}

/* Prikaz - mazani svetla
*/
void oe_prikaz_dsvetlo_smaz(K_EDITOR *p_cnf)
{
  int  v = p_cnf->dl_akt;
  DYN_LIGHT *p_light = p_cnf->p_dlight+v;
  
  if(v < MAX_FLARE_SVETEL && p_light->akt) {
    if(ddwqueto(hwnd_aplikace,"Smazat svetlo %s ?",p_light->jmeno)) {          
      if(p_light->flag&SDL_MESH) {
        if(!p_light->p_prev) { // mazu prvni svetlo kontejneru
          EDIT_KONTEJNER *p_kont = (EDIT_KONTEJNER *)p_light->p_mesh_data;
          assert(p_light == p_kont->p_dlight);
          p_kont->p_dlight = p_light->p_next;
          if(p_light->p_next)
            p_light->p_next->p_prev = NULL;       
        } else {
          p_light->p_prev->p_next = p_light->p_next;
          if(p_light->p_next)
            p_light->p_next->p_prev = p_light->p_prev;
        }
      }
      
      memset(p_light,0,sizeof(p_light[0]));

      oe_najdi_prepocitej_dsvetlo(p_cnf);
      if(p_cnf->cnf.svetla_dynamicke) {
        ray_prepocitej_staticke_osvetleni_vertexy(p_cnf);
      }
      p_cnf->dl_akt = lo_najdi_plne_dsvetlo(p_cnf->p_dlight,MAX_FLARE_SVETEL);
      if(p_cnf->dl_akt == K_CHYBA)
        p_cnf->dl_akt = 0;
    }
  }
}

void oe_prikaz_dsvetlo_posun(K_EDITOR *p_cnf)
{
 DYN_LIGHT *p_light;
 VEKTOR     v1,v2;
 GLMATRIX   mat;

 kprintf(0,"Posun dynamickeho svetla...");
 
 // kopie svetla
 if(key[K_CTRL]) {
   int l = oe_najdi_volne_dsvetlo(p_cnf);
   p_cnf->p_dlight[l] = p_cnf->p_dlight[p_cnf->dl_akt];
   //p_cnf->p_dlight[l].index = l;
   p_cnf->dl_akt = l;
   sprintf(p_cnf->p_dlight[l].jmeno,"DSvetlo %d",l);
   key[K_CTRL] = FALSE;
 }

 p_light = p_cnf->p_dlight+p_cnf->dl_akt;
 
 if(p_light->flag&SDL_MESH)
   mat_copy(&mat,kont_world_matrix(p_light->p_mesh_data));
 else {
   init_matrix(&mat);
 }

 if(!spracuj_spravy(FALSE)) oe_konec(p_cnf);
 calc_transf_3d_2d_matrix(&mat,p_cnf->p_camera,p_cnf->p_project,&mat);
 *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);
 transf_2d_3d_z_matrix((float)mi.x, (float)mi.y, *p_cnf->p_vzdal,
        &v1.x, &v1.y, &v1.z, &mat,OXRES, OYRES, OXSTART, OYSTART,
        p_cnf->kam.near_plane,p_cnf->kam.far_plane);

 nuluj_mys();
 while(stisk_mys()) {
   transf_2d_3d_z_matrix((float)mi.x, (float)mi.y, *p_cnf->p_vzdal,
        &v2.x, &v2.y, &v2.z, &mat,OXRES, OYRES, OXSTART, OYSTART,
        p_cnf->kam.near_plane,p_cnf->kam.far_plane);   
   
   p_light->pflag = FALSE;
   if(!key[K_X]&&!key[K_Y]&&!key[K_Z]) {
     p_light->np.x += v2.x-v1.x;
     p_light->np.y += v2.y-v1.y;
     p_light->np.z += v2.z-v1.z;
     p_light->pflag = POSUN_VSE;
   } else {
     if(key[K_X]) {
       p_light->np.x += v2.x-v1.x;
       p_light->pflag = POSUN_OSA_X;
     }
     if(key[K_Y]) {
       p_light->np.y += v2.y-v1.y;
       p_light->pflag = POSUN_OSA_Y;
     }
     if(key[K_Z]) {
       p_light->np.z += v2.z-v1.z;
       p_light->pflag = POSUN_OSA_Z;
     }
   }   

   if(p_cnf->cnf.svetla_dynamicke) {
     ray_prepocitej_staticke_osvetleni_vertexy(p_cnf);
   }
   
   nuluj_dif();
   oe_renderuj_scenu_pre(p_cnf);
   oe_renderuj_scenu_po(p_cnf);

   transf_2d_3d_z_matrix((float)mi.x, (float)mi.y, *p_cnf->p_vzdal,
     &v1.x, &v1.y, &v1.z, &mat,OXRES, OYRES, OXSTART, OYSTART,
        p_cnf->kam.near_plane,p_cnf->kam.far_plane);
 }
 *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);
 p_light->pflag = POSUN_VSE;
 kprintf(0,"OK");
}

/*
void sdla_updatuj(K_EDITOR *p_cnf)
{
  dword           next_time = p_cnf->TimeEndLastFrame;
  DYN_LIGHT      *p_light;
  DYN_LIGHT_ANIM *p_an;
  int             i;
  int             konec;

  //Animace se updatuji kazdy frame
  for(i = 0; i < p_cnf->dl_lightnum; i++) {
    p_light = p_cnf->p_dlight+i;
    p_an = &p_light->an;
    
    if(p_an->endtime) {
      // Test na konec animace     
      if((konec = (next_time > p_light->time_stop))) {
        if(p_light->an_flag&GK_LOOP) {
          p_light->time = p_light->time_delka;
          konec = 0;
        } else {
          p_light->time = p_light->time_delka;
        }
      } else {
        p_light->time = calc_time_akt(next_time,p_light->time_start);
      }

      //Vypocitej animaci svetla
      
      sdl_light_to_light(p_light,p_an,p_light->time);      
    }
  }
}
*/
/* ******************************************************************
    Extra-Dynamicka svetla
   ******************************************************************
*/
int oe_najdi_prepocitej_extra_svetlo(K_EDITOR *p_cnf)
{
 int i;
 p_cnf->edl_lightnum = -1;

 for(i = 0; i < MAX_FLARE_SVETEL; i++) {
   if(p_cnf->p_edlight[i].akt && i > p_cnf->edl_lightnum) {
     p_cnf->edl_lightnum = i;
   }     
 }  
 return(++p_cnf->edl_lightnum);
}

int oe_najdi_volne_extra_svetlo(K_EDITOR *p_cnf)
{
 int i;

 for(i = 0; i < MAX_FLARE_SVETEL; i++) {
   if(!p_cnf->p_edlight[i].akt)
     return(i);
 }
 return(K_CHYBA); 
}

void oe_prikaz_extra_svetlo_nove(K_EDITOR *p_cnf)
{
  EXTRA_DYN_LIGHT *p_dlight;
  byte pom[200];
  int  v,k = p_cnf->vybr_kont;

  if(!p_cnf->lc.scena_light) 
    n_kont(k);

  v = lo_najdi_volne_extra_svetlo(p_cnf->p_edlight,MAX_FLARE_SVETEL);
  if(v != K_CHYBA) {
    p_dlight = p_cnf->p_edlight+v;
    memset(p_dlight,0,sizeof(p_dlight[0]));
    sprintf(p_dlight->jmeno,"EDSvetlo %d",v);
    p_dlight->flag = EDL_UTLUM_LIN|EDL_BODOVE|EDL_DOSAH;    
    p_dlight->dosah = 10.0f;
    p_dlight->index = v;

    if(p_cnf->lc.scena_light) {
      strcpy(pom,"Zadej nove svetlo sceny ty prdolo!");
      p_dlight->p_mesh_data = NULL;
    } else {
      sprintf(pom,"Nove svetlo kontejneru %d '%s'",k,p_cnf->p_kont[k]->jmeno);
      p_dlight->p_mesh_data = p_cnf->p_kont[k];
      p_dlight->p_next = p_cnf->p_kont[k]->p_edlight;
      p_dlight->p_prev = NULL;
      p_cnf->p_kont[k]->p_edlight = p_dlight;
      if(p_dlight->p_next) {
        p_dlight->p_next->p_prev = p_dlight;
      }
      p_dlight->flag |= EDL_MESH;
    }
    
    if(doe_prikaz_extra_dynamicke_svetlo(hwnd_aplikace,pom,p_dlight)) {
      p_cnf->edl_akt = v;
      p_dlight->akt = TRUE;      
      oe_najdi_prepocitej_extra_svetlo(p_cnf);
      if(p_cnf->cnf.svetla_dynamicke) {
        ray_prepocitej_staticke_osvetleni_vertexy(p_cnf);
      }
    }    
  }
}

void oe_prikaz_extra_svetlo_uprav(K_EDITOR *p_cnf)
{
  EDIT_KONTEJNER *p_kont;
  EXTRA_DYN_LIGHT *p_dlight;
  byte pom[200];
  int  v = p_cnf->dl_akt;
  
  if(v < MAX_FLARE_SVETEL && p_cnf->p_edlight[v].akt) {
    p_dlight = p_cnf->p_edlight+v;
    if(p_cnf->lc.scena_light) {
      strcpy(pom,"Uprav svetlo sceny ty prdolo!");
      doe_prikaz_extra_dynamicke_svetlo(hwnd_aplikace,pom,p_dlight);
    } else {
      p_kont = p_dlight->p_mesh_data;
      if(p_kont) {
        sprintf(pom,"Upravit svetlo kontejneru '%s'",p_kont->jmeno);
        doe_prikaz_extra_dynamicke_svetlo(hwnd_aplikace,pom,p_dlight);
        if(p_cnf->cnf.svetla_dynamicke) {
          ray_prepocitej_staticke_osvetleni_vertexy(p_cnf);
        }
      }
    }
  }
}

void oe_prikaz_extra_svetlo_smaz(K_EDITOR *p_cnf)
{
  int  v = p_cnf->dl_akt;
  
  if(v < MAX_FLARE_SVETEL && p_cnf->p_edlight[v].akt) {
    if(ddwqueto(hwnd_aplikace,"Smazat svetlo %s ?",p_cnf->p_edlight[v].jmeno)) {
      p_cnf->p_edlight[v].akt = FALSE;
      oe_najdi_prepocitej_extra_svetlo(p_cnf);
      if(p_cnf->cnf.svetla_dynamicke) {
        ray_prepocitej_staticke_osvetleni_vertexy(p_cnf);
      }
    }
  }
}

void oe_prikaz_extra_svetlo_posun(K_EDITOR *p_cnf)
{
 EXTRA_DYN_LIGHT *p_light;
 VEKTOR     v1,v2;
 GLMATRIX   mat;

 kprintf(0,"Posun dynamickeho svetla...");
 
 // kopie svetla
 if(key[K_CTRL]) {
   int l = oe_najdi_volne_extra_svetlo(p_cnf);
   p_cnf->p_edlight[l] = p_cnf->p_edlight[p_cnf->edl_akt];
   p_cnf->edl_akt = l;
   sprintf(p_cnf->p_edlight[l].jmeno,"EDSvetlo %d",l);
   key[K_CTRL] = FALSE;
 }

 init_matrix(&mat);

 p_light = p_cnf->p_edlight+p_cnf->edl_akt;
 if(!spracuj_spravy(FALSE)) oe_konec(p_cnf);
 calc_transf_3d_2d_matrix(&mat,p_cnf->p_camera,p_cnf->p_project,&mat);
 *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);
 transf_2d_3d_z_matrix((float)mi.x, (float)mi.y, *p_cnf->p_vzdal,
     &v1.x, &v1.y, &v1.z, &mat,OXRES, OYRES, OXSTART, OYSTART,
        p_cnf->kam.near_plane,p_cnf->kam.far_plane);

 nuluj_mys();
 while(stisk_mys()) {
   transf_2d_3d_z_matrix((float)mi.x, (float)mi.y, *p_cnf->p_vzdal,
     &v2.x, &v2.y, &v2.z, &mat,OXRES, OYRES, OXSTART, OYSTART,
        p_cnf->kam.near_plane,p_cnf->kam.far_plane);   
   
   p_light->pflag = FALSE;
   if(!key[K_X]&&!key[K_Y]&&!key[K_Z]) {
     p_light->np.x += v2.x-v1.x;
     p_light->np.y += v2.y-v1.y;
     p_light->np.z += v2.z-v1.z;
     p_light->pflag = POSUN_VSE;
   } else {
     if(key[K_X]) {
       p_light->np.x += v2.x-v1.x;
       p_light->pflag = POSUN_OSA_X;
     }
     if(key[K_Y]) {
       p_light->np.y += v2.y-v1.y;
       p_light->pflag = POSUN_OSA_Y;
     }
     if(key[K_Z]) {
       p_light->np.z += v2.z-v1.z;       
       p_light->pflag = POSUN_OSA_Z;
     }
   }   

   if(p_cnf->cnf.svetla_dynamicke) {
     ray_prepocitej_staticke_osvetleni_vertexy(p_cnf);
   }
   
   nuluj_dif();
   oe_renderuj_scenu_pre(p_cnf);
   oe_renderuj_scenu_po(p_cnf);

   transf_2d_3d_z_matrix((float)mi.x, (float)mi.y, *p_cnf->p_vzdal,
     &v1.x, &v1.y, &v1.z, &mat,OXRES, OYRES, OXSTART, OYSTART,
        p_cnf->kam.near_plane,p_cnf->kam.far_plane);
 }
 *p_cnf->p_vzdal = oe_stredni_vzdalenost_ve_scene(p_cnf);
 p_light->pflag = POSUN_VSE;
 kprintf(0,"OK");
}
