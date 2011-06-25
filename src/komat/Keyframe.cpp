/* Keyframovy animacni modul - slohnuto z:
   Splines & Quaternions (splines.txt)
   -----------------------------------
   by
   Jochen Wilhelmy
   a.k.a.
   digisnap
   digisnap@cs.tu-berlin.de
   06.08.1997
*/
#include <alloca.h>

#include "Berusky_universal.h"
#include "3d_all.h"

#define  DELTA_INTERVAL  0.01f

#define  LIB3DS_EPSILON (1e-8)

#define  MAX_SIM_RADKU 2000


/******************************************************************************
  Debug-rozhrani
 ******************************************************************************
*/

void dbg_print_matrix(GLMATRIX * m)
{

 #define PRESNOST2 "%f %f %f %f"
 dbgprintf(PRESNOST2,m->_11,m->_21,m->_31,m->_41);
 dbgprintf(PRESNOST2,m->_12,m->_22,m->_32,m->_42);
 dbgprintf(PRESNOST2,m->_13,m->_23,m->_33,m->_43);
 dbgprintf(PRESNOST2,m->_14,m->_24,m->_34,m->_44); 
}

void kprintf_matrix(GLMATRIX * m)
{
 #define PRESNOST2 "%f %f %f %f"
 kprintf(TRUE,PRESNOST2,m->_11,m->_21,m->_31,m->_41);
 kprintf(TRUE,PRESNOST2,m->_12,m->_22,m->_32,m->_42);
 kprintf(TRUE,PRESNOST2,m->_13,m->_23,m->_33,m->_43);
 kprintf(TRUE,PRESNOST2,m->_14,m->_24,m->_34,m->_44); 
}

void kprint_matrix(GLMATRIX * m)
{
 #define PRESNOST3 "%.2f %.2f %.2f %.2f"
 kprintf(TRUE,PRESNOST3,m->_11,m->_21,m->_31,m->_41);
 kprintf(TRUE,PRESNOST3,m->_12,m->_22,m->_32,m->_42);
 kprintf(TRUE,PRESNOST3,m->_13,m->_23,m->_33,m->_43);
 kprintf(TRUE,PRESNOST3,m->_14,m->_24,m->_34,m->_44); 
}

void dbg_print_track_uhel(TRACK_INFO *p_track)
{
 word  i;

 dbgprintf("Uhlovy track....");
 for(i = 0; i < p_track->rot_keys; i++) {
   dbgprintf("U: %d Uhel %f Osa [x = %f][y = %f][z = %f]",i,
   p_track->p_rot[i].angle*180.0f/PI,p_track->p_rot[i].x,
   p_track->p_rot[i].y,p_track->p_rot[i].z);   
 }
}

void dbg_print_track_quaternion(TRACK_INFO *p_track)
{
 QUAT qa;
 word  i;

 dbgprintf("Quat track....");
 for(i = 0; i < p_track->rot_keys; i++) {
   key_rotkey_to_quat(p_track->p_rot+i,&qa);
   dbgprintf("A: %d Absolutne [x = %f][y = %f][z = %f][w = %f]",i,qa.x,qa.y,qa.z,qa.w);
 }
}

void dbg_print_track_abs(TRACK_INFO *p_track)
{
 QUAT qa;
 word  i;

 dbgprintf("Absolutni track...");
 for(i = 0; i < p_track->rot_keys; i++) {
   qa = p_track->p_at[i];
   dbgprintf("%d Absolutne [x = %f][y = %f][z = %f][w = %f]",i,qa.x,qa.y,qa.z,qa.w);
 }
}

void dbg_print_track_abs_uhel(TRACK_INFO *p_track)
{
 QUAT qa;
 word  i;

 dbgprintf("Absolutni uhlovy track...");
 for(i = 0; i < p_track->rot_keys; i++) {   
   quat_to_quat_angle(p_track->p_at+i,&qa);
   dbgprintf("U: %d Uhel %f Osa [x = %f][y = %f][z = %f]",i,
   qa.w*180.0f/PI,qa.x,qa.y,qa.z);
 }
}



/*------------------------------------------------------------
  Modifikonvane universalni fce.
  ------------------------------------------------------------
*/
/* Pokud je frame klicem -> vrati jeho cislo, jinak K_CHYBA
*/
int key_je_frame_klic(KEY_POINT_BRS *p_list, int klicu, dword frame)
{
 int   l,p,s;
 dword time = frame*SIM_KONSTI;

 l = 0; p = klicu;

 if(!klicu || !p_list || time < p_list->time || time > p_list[klicu-1].time)
   return(K_CHYBA);

 if(time == p_list->time)
   return(0);
 if(time == p_list[klicu-1].time)
   return(klicu-1);

 while(l != p) {
// while(l < p) {
   s = l+((p-l)>>1);
   if(p_list[s].time == time)
     return(s);
   else {     
     if(time > p_list[s].time) {
       if(time < p_list[s+1].time)
         return(K_CHYBA);
       else
         l = s;
     }
     else
       p = s;
   }
 }
 return(K_CHYBA);
}

int key_je_time_klic(KEY_POINT_BRS *p_list, int klicu, dword time)
{
 int l,p,s; 

 l = 0; p = klicu;

 if(!klicu || !p_list || time < p_list->time || time > p_list[klicu-1].time)
   return(K_CHYBA);

 if(time == p_list->time)
   return(0);
 if(time == p_list[klicu-1].time)
   return(klicu-1);

 while(l != p) {
// while(l < p) {
   s = l+((p-l)>>1);
   if(p_list[s].time == time)
     return(s);
   else {     
     if(time > p_list[s].time) {
       if(time < p_list[s+1].time)
         return(K_CHYBA);
       else
         l = s;
     }
     else
       p = s;
   }
 }
 return(K_CHYBA);
}

// najde tento frame nebo nejblizsi levy klic
inline static int key_najdi_klice(KEY_POINT_BRS *p_list, int klicu, dword time)
{
 int l,p,s;

 l = 0; p = klicu;

 while(l != p) {
   s = l+((p-l)>>1);
   if(p_list[s].time == time)
     return(s);
   else {     
     if(time > p_list[s].time) {
       if(time < p_list[s+1].time)
         return(s);
       else
         l = s;
     }
     else
       p = s;
   }
 }
 return(K_CHYBA);
}

/* 
  ----------------------
    Interpolace floatu
  ----------------------
*/
/* spocita tangentu k pozicnimu klici
   trida =  1 -> trida a
         = -1 -> trida b
*/
float key_tangent_float(float *p_body, KEY_POINT_BRS *p_keys, int kn, int keynum, int trida, int loop, float *p_tn)
{
  float bias,continuity,tension;
  float t1,g1,g3;  

  assert(kn < keynum);

  bias = p_keys[kn].bias;
  continuity = p_keys[kn].continuity;
  tension = p_keys[kn].tension;

  if(!kn) {
    if(keynum == 2) {
      *p_tn = (p_body[1] - p_body[0])*(1.0f - tension);
      return(*p_tn);
    } else {
      if(loop) { // prvni klic
        g1 = (p_body[0] - p_body[keynum-1])*(1.0f+bias);
        g3 = (p_body[1] - p_body[0])*(1.0f-bias)-g1;
        *p_tn = (g1 + g3*(0.5f + trida*0.5f*continuity))*(1.0f-tension);        
      } else {
        key_tangent_float(p_body, p_keys, 1, keynum, TRIDA_A, loop, &t1);
        *p_tn = ((p_body[kn+1] - p_body[kn])*1.5f - t1*0.5f)*(1.0f - tension);
      }
      return(*p_tn);
    }
  } else if(kn == (keynum-1)) {// konec - p(n-1), p(n)
    if(keynum == 2) {
      *p_tn = (p_body[kn] - p_body[kn-1])*(1.0f - tension);
      return(*p_tn);
    } else {
      if(loop) { // posledni klic
        g1 = (p_body[kn] - p_body[kn-1])*(1.0f+bias);
        g3 = (p_body[0] - p_body[kn])*(1.0f-bias)-g1;
        *p_tn = (g1 + g3*(0.5f + trida*0.5f*continuity))*(1.0f-tension);
      } else {
        key_tangent_float(p_body, p_keys, kn-1, keynum, TRIDA_B, loop, &t1);
        *p_tn = ((p_body[kn]-p_body[kn-1])*1.5f - t1*0.5f)*(1.0f - tension);
      }
      return(*p_tn);
    }
  } else { // stredni bod - mam p(n-1), p(n), p(n+1)
    g1 = (p_body[kn] - p_body[kn-1])*(1.0f+bias);
    g3 = (p_body[kn+1] - p_body[kn])*(1.0f-bias)-g1;
    *p_tn = (g1 + g3*(0.5f + trida*0.5f*continuity))*(1.0f-tension);
    return(*p_tn);
  }
}

/* Prepocita kn, kn1, vraci t
   vraci:
   0 - pouzit vse (kn,kn1,time)
   1 - pouzit kn
   2 - pouzit kn1   
*/
#define KN_ALL   0 //pouzit 2 klice + cas
#define KN_KN    1 //pouzit klic 1


// Volat pouze pokud keynum > 1 !!!
inline int key_track_prepocet_knt(KEY_POINT_BRS *p_keys, int keynum, int time, int endtime, int loop, int *p_kn, int *p_kn1, float *p_time)
{
  int  start,stop,k1;
  int  dop,intrv;
  int  t1,t2;
  
  k1 = keynum-1;
  
  start = p_keys[0].time;
  stop = p_keys[k1].time;
  
  if(time > 2*endtime) {
    time = endtime + time%endtime;
  }
  assert(time < 2*endtime);

  if(time < start) {
    if(loop) {
      *p_kn = k1;
      *p_kn1 = 0;
      dop = endtime-stop;
      intrv = dop+start;
      *p_time = (intrv > DELTA_INTERVAL) ? (float)(time+dop)/(float)intrv : 0.0f;
      return(KN_ALL);
    } else {
      *p_kn = 0;
      return(KN_KN);
    }
  } else if(time >= stop) {
    if(loop) {
      *p_kn = k1;
      *p_kn1 = 0;        // presah do dalsiho intervalu
      intrv = (endtime-stop)+start;
      *p_time = (intrv > DELTA_INTERVAL) ? (float)(time-stop)/(float)intrv : 0.0f;
      return(KN_ALL);
    } else {
      *p_kn = k1;
      return(KN_KN);
    }
  } else {
    if(keynum == 2) {
      *p_kn = 0;
      *p_kn1 = 1;
      t1 = p_keys[0].time;
      t2 = p_keys[1].time;
    } else {
      *p_kn = key_najdi_klice(p_keys, keynum, time);
      *p_kn1 = (*p_kn)+1;
      t1 = p_keys[*p_kn].time;
      t2 = p_keys[*p_kn1].time;
    }
    *p_time = (float)(time-t1)/(float)(t2-t1);
    return(KN_ALL);
  }
}

float key_track_interpolace_float(float *p_vysl, float *p_body,
                                  KEY_POINT_BRS *p_keys, int time, int endtime,
                                  int keynum, int loop)
{
  float pt0,pt1;  
  float t,t2,t3;
  int   ret;
  int   kn,kn1;  

  if(keynum > 1) {
    ret = key_track_prepocet_knt(p_keys, keynum, time, endtime, loop, &kn, &kn1, &t);
    if(ret == KN_ALL) {
      t2 = t*t; t3 = t2*t;
      key_tangent_float(p_body, p_keys, kn,  keynum, TRIDA_B, loop, &pt0);
      key_tangent_float(p_body, p_keys, kn1, keynum, TRIDA_A, loop, &pt1);
      return((*p_vysl = p_body[kn]*(2*t3-3*t2+1) + pt0*(t3-2*t2+t) 
                      + p_body[kn1]*(-2*t3+3*t2) + pt1*(t3-t2)));
    } else {
      return(*p_vysl = p_body[kn]);
    }
  } else {
    if(keynum) {
      return(*p_vysl = *p_body);
    }
    else 
      return(FLT_MAX);
  }
}

float key_track_interpolace_float_linear(float *p_vysl, float *p_body,
                                         KEY_POINT_BRS *p_keys, int time, int endtime,
                                         int keynum, int loop)
{  
  float t,t2;  
  int   kn,kn1,ret;
  
  if(keynum > 1) {
    ret = key_track_prepocet_knt(p_keys, keynum, time, endtime, loop, &kn, &kn1, &t);
    if(ret == KN_ALL) {
      t2 = 1.0f-t;
      return(*p_vysl = p_body[kn]*t2+p_body[kn1]*t);
    } else {
      return(*p_vysl = p_body[kn]);
    }
  } else {
    if(keynum) {
      return(*p_vysl = *p_body);
    }
    else 
      return(FLT_MAX);
  }
}

/* 
  ----------------------
     Interpolace bodu
  ----------------------
*/
/* spocita tangentu k pozicnimu klici
   trida =  1 -> trida a
         = -1 -> trida b
*/
BOD * key_tangent_bod(BOD *p_body, KEY_POINT_BRS *p_keys, int kn, int keynum, int trida, int loop, BOD *p_tn)
{
  int   kn_1 = kn-1, // key k(n-1)
        kn1  = kn+1; // key k(n+1)
  float bias,continuity,tension,tmp,tmp2;
  BOD  *p_p0,*p_p1,*p_p2,
        t1,
        g1,g2,g3;

  bias = p_keys[kn].bias;
  continuity = p_keys[kn].continuity;
  tension = p_keys[kn].tension;

  if(!kn) {
    //kn = 0,  zacatek - mam p(n), p(n+1)    
    p_p1 = p_body+kn;
    p_p2 = p_body+kn1;

    if(keynum == 2) {
      //track ma jen 2 klice
      //b0 = (p1-p0)*(1-T)
      tmp = 1.0f - tension;
      p_tn->x = (p_p2->x - p_p1->x)*tmp;
      p_tn->y = (p_p2->y - p_p1->y)*tmp;
      p_tn->z = (p_p2->z - p_p1->z)*tmp;
      return(p_tn);
    } else { // zacatecni tangenta
      if(loop) {
        p_p0 = p_body+keynum-1;
      } else {
        // trak ma vice klicu
        key_tangent_bod(p_body, p_keys, 1, keynum, TRIDA_A, loop, &t1);
        
        // b0 = ((p1 - p0)*1.5 - a1*0.5)*(1-T)
        tmp = 1.0f - tension;
        p_tn->x = ((p_p2->x - p_p1->x)*1.5f - t1.x*0.5f)*tmp;
        p_tn->y = ((p_p2->y - p_p1->y)*1.5f - t1.y*0.5f)*tmp;
        p_tn->z = ((p_p2->z - p_p1->z)*1.5f - t1.z*0.5f)*tmp;
        return(p_tn);
      }
    }
  } else if(kn == (keynum-1)) {// konec - p(n-1), p(n)
    p_p0 = p_body+kn_1;
    p_p1 = p_body+kn;

    if(keynum == 2) {
      //track ma jen 2 klice
      //a1 = (p1-p0)*(1-T)
      tmp = 1.0f - tension;
      p_tn->x = (p_p1->x - p_p0->x)*tmp;
      p_tn->y = (p_p1->y - p_p0->y)*tmp;
      p_tn->z = (p_p1->z - p_p0->z)*tmp;
      return(p_tn);
    } else {
      if(loop) { // koncova tangenta
        p_p2 = p_body;
      } else {
        // bez opakovani      
        key_tangent_bod(p_body, p_keys, kn_1, keynum, TRIDA_B, loop, &t1);
        
        // an = ((pn - p(n-1))*1.5 - b(n-1)*0.5)*(1-T)
        tmp = 1.0f - tension;
        p_tn->x = ((p_p1->x - p_p0->x)*1.5f - t1.x*0.5f)*tmp;
        p_tn->y = ((p_p1->y - p_p0->y)*1.5f - t1.y*0.5f)*tmp;
        p_tn->z = ((p_p1->z - p_p0->z)*1.5f - t1.z*0.5f)*tmp;
        
        return(p_tn);
      }
    }
  } else { // stredni bod - mam p(n-1), p(n), p(n+1)
    p_p0 = p_body+kn_1;
    p_p1 = p_body+kn;
    p_p2 = p_body+kn1;
  }

  //  g1 = (pn - p(n-1))*(1+B)
  tmp = 1.0f+bias;
  g1.x = (p_p1->x - p_p0->x)*tmp;
  g1.y = (p_p1->y - p_p0->y)*tmp;
  g1.z = (p_p1->z - p_p0->z)*tmp;
  
  // g2 = (p(n+1) - pn)*(1-B)
  tmp = 1.0f-bias;
  g2.x = (p_p2->x - p_p1->x)*tmp;
  g2.y = (p_p2->y - p_p1->y)*tmp;
  g2.z = (p_p2->z - p_p1->z)*tmp;
  
  // g3 = g2 - g1,
  vektor_sub(&g2,&g1,&g3);
  
  /*
  tangent case a: an = (g1 + g3*(0.5 + 0.5*C))*(1-T),
  tangent case b: bn = (g1 + g3*(0.5 - 0.5*C))*(1-T).
  */
  tmp = 1.0f-tension;
  tmp2 = 0.5f + trida*0.5f*continuity;
  p_tn->x = (g1.x + g3.x*tmp2)*tmp;
  p_tn->y = (g1.y + g3.y*tmp2)*tmp;
  p_tn->z = (g1.z + g3.z*tmp2)*tmp;
  
  return(p_tn);
}

BOD * key_track_interpolace_bod(BOD *p_vysl, BOD *p_body, KEY_POINT_BRS *p_keys,
                                int time, int endtime, int keynum, int loop)
{
  BOD   *p0,*p1,pt0,pt1;
  float F1,F2,F3,F4;
  float t,t2,t3;
  int   kn,kn1;
  int   ret;
  
  if(keynum > 1) {
    ret = key_track_prepocet_knt(p_keys, keynum, time, endtime, loop, &kn, &kn1, &t);
    if(ret == KN_ALL) {
      t2 = t*t; t3 = t2*t;
      
      key_tangent_bod(p_body, p_keys, kn,  keynum, TRIDA_B, loop, &pt0);
      key_tangent_bod(p_body, p_keys, kn1, keynum, TRIDA_A, loop, &pt1);
      
      p0 = p_body+kn;
      p1 = p_body+kn1;
      
      F1 = 2*t3-3*t2+1;
      F2 = t3-2*t2+t;
      F3 = -2*t3+3*t2;
      F4 = t3-t2;
      
      p_vysl->x = p0->x*F1 + pt0.x*F2 + p1->x*F3 + pt1.x*F4;
      p_vysl->y = p0->y*F1 + pt0.y*F2 + p1->y*F3 + pt1.y*F4;
      p_vysl->z = p0->z*F1 + pt0.z*F2 + p1->z*F3 + pt1.z*F4;
    } else {
      *p_vysl = p_body[kn];
    }       
  } else {
    if(keynum) {
      *p_vysl = *p_body;
    }
  }

  return(p_vysl);
}

BOD * key_track_interpolace_bod_linear(BOD *p_vysl, BOD *p_body, KEY_POINT_BRS *p_keys,
                                int time, int endtime, int keynum, int loop)
{
  BOD   *p_p0,*p_p1;
  float t,t2;
  int   kn,kn1;
  int   ret;
  
  if(keynum > 1) {
    ret = key_track_prepocet_knt(p_keys, keynum, time, endtime, loop, &kn, &kn1, &t);
    if(ret == KN_ALL) {
      t2 = 1.0f-t;
      p_p0 = p_body+kn;
      p_p1 = p_body+kn1;
      p_vysl->x = p_p0->x*t2 + p_p1->x*t;
      p_vysl->y = p_p0->y*t2 + p_p1->y*t;
      p_vysl->z = p_p0->z*t2 + p_p1->z*t;
    } else {
      *p_vysl = p_body[kn];
    }
  } else {
    if(keynum) {
      *p_vysl = *p_body;
    }
  }
  return(p_vysl);
}

/* 
  ---------------------------
     Interpolace ctyr-bodu
  ---------------------------
*/
/* spocita tangentu k pozicnimu klici
   trida =  1 -> trida a
         = -1 -> trida b
*/
WBOD * key_tangent_wbod(WBOD *p_body, KEY_POINT_BRS *p_keys, int kn, int keynum, int trida, int loop, WBOD *p_tn)
{
  int   kn_1 = kn-1, // key k(n-1)
        kn1  = kn+1; // key k(n+1)
  float bias,continuity,tension,tn1,bp1;
  WBOD *p_p0,*p_p1,*p_p2,
        t1,
        g1,g2,g3;

  bias = p_keys[kn].bias;
  continuity = p_keys[kn].continuity;
  tension = p_keys[kn].tension;

  if(!kn) {
    //kn = 0,  zacatek - mam p(n), p(n+1)    
    p_p1 = p_body+kn;
    p_p2 = p_body+kn1;

    if(keynum == 2) {
      //track ma jen 2 klice
      //b0 = (p1-p0)*(1-T)
      tn1 = (1.0f - tension);
      p_tn->x = (p_p2->x - p_p1->x)*tn1;
      p_tn->y = (p_p2->y - p_p1->y)*tn1;
      p_tn->z = (p_p2->z - p_p1->z)*tn1;
      p_tn->w = (p_p2->w - p_p1->w)*tn1;
      return(p_tn);
    } else {
      if(loop) {
        p_p0 = p_body+keynum-1;
      } else {
        // trak ma vice klicu            
        key_tangent_wbod(p_body, p_keys, 1, keynum, TRIDA_A, loop, &t1);
        
        tn1 = (1.0f - tension);
        // b0 = ((p1 - p0)*1.5 - a1*0.5)*(1-T)
        p_tn->x = ((p_p2->x - p_p1->x)*1.5f - t1.x*0.5f)*tn1;
        p_tn->y = ((p_p2->y - p_p1->y)*1.5f - t1.y*0.5f)*tn1;
        p_tn->z = ((p_p2->z - p_p1->z)*1.5f - t1.z*0.5f)*tn1;
        p_tn->w = ((p_p2->w - p_p1->w)*1.5f - t1.w*0.5f)*tn1;
        
        return(p_tn);
      }
    }
  } else if(kn == (keynum-1)) {// konec - p(n-1), p(n)
    p_p0 = p_body+kn_1;
    p_p1 = p_body+kn;

    if(keynum == 2) {
      //track ma jen 2 klice
      //a1 = (p1-p0)*(1-T)
      tn1 = (1.0f - tension);
      p_tn->x = (p_p1->x - p_p0->x)*tn1;
      p_tn->y = (p_p1->y - p_p0->y)*tn1;
      p_tn->z = (p_p1->z - p_p0->z)*tn1;
      p_tn->w = (p_p1->w - p_p0->w)*tn1;

      return(p_tn);
    } else {
      if(loop) {
        p_p2 = p_body;
      } else {
        // bez opakovani      
        key_tangent_wbod(p_body, p_keys, kn_1, keynum, TRIDA_B, loop, &t1);
        
        // an = ((pn - p(n-1))*1.5 - b(n-1)*0.5)*(1-T)
        tn1 = (1.0f - tension);
        p_tn->x = ((p_p1->x - p_p0->x)*1.5f - t1.x*0.5f)*tn1;
        p_tn->y = ((p_p1->y - p_p0->y)*1.5f - t1.y*0.5f)*tn1;
        p_tn->z = ((p_p1->z - p_p0->z)*1.5f - t1.z*0.5f)*tn1;
        p_tn->w = ((p_p1->w - p_p0->w)*1.5f - t1.w*0.5f)*tn1;
        
        return(p_tn);
      }
    }
  } else { // stredni bod - mam p(n-1), p(n), p(n+1)
    p_p0 = p_body+kn_1;
    p_p1 = p_body+kn;
    p_p2 = p_body+kn1;
  }

  bp1 = 1.0f+bias;
  
  //  g1 = (pn - p(n-1))*(1+B)
  g1.x = (p_p1->x - p_p0->x)*bp1;
  g1.y = (p_p1->y - p_p0->y)*bp1;
  g1.z = (p_p1->z - p_p0->z)*bp1;
  g1.w = (p_p1->w - p_p0->w)*bp1;
  
  // g2 = (p(n+1) - pn)*(1-B)
  g2.x = (p_p2->x - p_p1->x)*bp1;
  g2.y = (p_p2->y - p_p1->y)*bp1;
  g2.z = (p_p2->z - p_p1->z)*bp1;
  g2.w = (p_p2->w - p_p1->w)*bp1;
  
  // g3 = g2 - g1,
  wvektor_sub(&g2,&g1,&g3);
  
  /*
  tangent case a: an = (g1 + g3*(0.5 + 0.5*C))*(1-T),
  tangent case b: bn = (g1 + g3*(0.5 - 0.5*C))*(1-T).
  */
  
  tn1 = (1.0f - tension);
  bp1 = (0.5f + trida*0.5f*continuity);
  p_tn->x = (g1.x + g3.x*bp1)*tn1;
  p_tn->y = (g1.y + g3.y*bp1)*tn1;
  p_tn->z = (g1.z + g3.z*bp1)*tn1;
  p_tn->w = (g1.w + g3.w*bp1)*tn1;
  
  return(p_tn);
  
}

WBOD * key_track_interpolace_wbod(WBOD *p_vysl, WBOD *p_body,
                                  KEY_POINT_BRS *p_keys,int time, int endtime,
                                  int keynum, int loop)
{
  WBOD  *p0,*p1,pt0,pt1;
  float F1,F2,F3,F4;
  float t,t2,t3;
  int   kn,kn1;
  int   ret;
  
  if(keynum > 1) {
    ret = key_track_prepocet_knt(p_keys, keynum, time, endtime, loop, &kn, &kn1, &t);
    if(ret == KN_ALL) {
      t2 = t*t; t3 = t2*t;
      
      key_tangent_wbod(p_body, p_keys, kn,  keynum, TRIDA_B, loop, &pt0);
      key_tangent_wbod(p_body, p_keys, kn1, keynum, TRIDA_A, loop, &pt1);
      
      p0 = p_body+kn;
      p1 = p_body+kn1;
      
      F1 = 2*t3-3*t2+1;
      F2 = t3-2*t2+t;
      F3 = -2*t3+3*t2;
      F4 = t3-t2;
      
      p_vysl->x = p0->x*F1 + pt0.x*F2 + p1->x*F3 + pt1.x*F4;
      p_vysl->y = p0->y*F1 + pt0.y*F2 + p1->y*F3 + pt1.y*F4;
      p_vysl->z = p0->z*F1 + pt0.z*F2 + p1->z*F3 + pt1.z*F4;
      p_vysl->w = p0->w*F1 + pt0.w*F2 + p1->w*F3 + pt1.w*F4;
    } else {
      *p_vysl = p_body[kn];
    }
  } else {
    if(keynum) {
      *p_vysl = *p_body;
    }
  }

  return(p_vysl);
}

WBOD * key_track_interpolace_wbod_linear(WBOD *p_vysl, WBOD *p_body,
                                  KEY_POINT_BRS *p_keys, int time, int endtime,
                                  int keynum, int loop)
{
  WBOD  *p_p0,*p_p1;
  float t,t2;
  int   kn,kn1;
  int   ret;
  
  if(keynum > 1) {
    ret = key_track_prepocet_knt(p_keys, keynum, time, endtime, loop, &kn, &kn1, &t);
    if(ret == KN_ALL) {
      t2 = 1.0f-t;
      p_p0 = p_body+kn;
      p_p1 = p_body+kn1;    
      p_vysl->x = p_p0->x*t2 + p_p1->x*t;
      p_vysl->y = p_p0->y*t2 + p_p1->y*t;
      p_vysl->z = p_p0->z*t2 + p_p1->z*t;
      p_vysl->w = p_p0->w*t2 + p_p1->w*t;
    } else {
      *p_vysl = p_body[kn];
    }
  } else {
    if(keynum) {
      *p_vysl = *p_body;
    }
  }

  return(p_vysl);
}

/* 
  ---------------------------
    Interpolace Quaternionu
  ---------------------------
*/

QUAT * key_tangent_quat(QUAT *p_at, KEY_POINT_BRS *p_rkeys, int kn, int keynum, int trida, int loop, QUAT *p_q)
{
  int   kn_1 = kn-1, // key k(n-1)
        kn1  = kn+1; // key k(n+1)
  float bias,continuity,tension;
  QUAT  q0, /*q1,*/ q2,
        g1, g2, g3;

  bias = p_rkeys[kn].bias;
  continuity = p_rkeys[kn].continuity;
  tension = p_rkeys[kn].tension;

  if(!kn) { 
    //kn = 0,  zacatek - mam p(n), p(n+1)    
    if(keynum == 2) { //track ma jen 2 klice
      return(slerp(p_at+kn,p_at+kn1,(1.0f-tension)/3.0f,p_q));
    } else { // prvni body (-1,0,1)
      if(loop) {
        return(slerp(p_at+kn,slerp(slerp(p_at+kn,p_at+keynum-1,-(1+bias)/3.0f,&g1),
                     slerp(p_at+kn,p_at+kn1, (1-bias)/3.0f,&g2),
                     0.5f + trida*0.5f*continuity,&g3),
                     trida*(tension-1.0f),p_q));
      } else {
        key_tangent_quat(p_at, p_rkeys, kn1, keynum, TRIDA_B, loop, &q2);
        return(slerp(p_at+kn,&q2,(1.0f-tension)*0.5f,p_q));
      }
    }
  } else if(kn == (int)keynum-1) {// konec - p(n-1), p(n)
    if(keynum == 2) { //track ma jen 2 klice
      return(slerp(p_at+kn,p_at+kn_1,(1.0f-tension)/3.0f,p_q));
    } else { // posledni body bez opakovani
      if(loop) { // posledni (-2,-1,0)
        return(slerp(p_at+kn,slerp(slerp(p_at+kn,p_at+kn_1,-(1+bias)/3.0f,&g1),
          slerp(p_at+kn,p_at, (1-bias)/3.0f,&g2),
          0.5f + trida*0.5f*continuity,&g3), 
          trida*(tension-1.0f),p_q));
      } else {
        key_tangent_quat(p_at, p_rkeys, kn_1, keynum, TRIDA_A, loop, &q0);
        return(slerp(p_at+kn,&q0,(1.0f-tension)*0.5f,p_q));
      }
    }
  } else { // stredni bod - mam q(n-1), q(n), q(n+1)
    return(slerp(p_at+kn,slerp(slerp(p_at+kn,p_at+kn_1,-(1+bias)/3.0f,&g1),
                         slerp(p_at+kn,p_at+kn1, (1-bias)/3.0f,&g2),
                         0.5f + trida*0.5f*continuity,&g3), 
                         trida*(tension-1.0f),p_q));
  }
}

QUAT * key_track_interpolace_quat(QUAT *p_quat, QUAT *p_at,
            KEY_POINT_BRS *p_rkeys, int time, int endtime,
            int keynum, int loop)
{  
  QUAT  q0,q1,q2;
  QUAT  qn,qn1;
  QUAT  bn,an;  
  int   kn,kn1;
  float t;
  int   ret;
  
  if(keynum > 1) {
    ret = key_track_prepocet_knt(p_rkeys, keynum, time, endtime, loop, &kn, &kn1, &t);
    if(ret == KN_ALL) {
      assert(0.0f <= t && t <= 1.0f);
      
      key_tangent_quat(p_at, p_rkeys, kn,  keynum, TRIDA_B, loop, &bn);
      key_tangent_quat(p_at, p_rkeys, kn1, keynum, TRIDA_A, loop, &an);
      
      qn = p_at[kn];
      qn1= p_at[kn1];
      
      slerp(&qn,&bn, t,&q0);
      slerp(&bn,&an, t,&q1);
      slerp(&an,&qn1,t,&q2);  
      
      slerp(&q0,&q1,t,&qn);
      slerp(&q1,&q2,t,&qn1);
      
      return(slerp(&qn,&qn1,t,p_quat));
    } else {
      *p_quat = p_at[kn];
      return(p_quat);
    }
  } else {
    if(keynum) {
      *p_quat = *p_at;
      return(p_quat);
    } else
      return(NULL);
  }
}

QUAT * key_track_interpolace_quat_linear(QUAT *p_quat, QUAT *p_at,
            KEY_POINT_BRS *p_rkeys, int time, int endtime,
            int keynum, int loop)
{
  int   kn,kn1;
  float t;
  int   ret;
  
  if(keynum > 1) {
    ret = key_track_prepocet_knt(p_rkeys, keynum, time, endtime, loop, &kn, &kn1, &t);
    if(ret == KN_ALL) {
      assert(0.0f <= t && t <= 1.0f);
      return(slerp(p_at+kn,p_at+kn1,t,p_quat));
    } else {
      *p_quat = p_at[kn];
      return(p_quat);
    }
  } else {
    if(keynum) {
      *p_quat = *p_at;
      return(p_quat);
    } else
      return(NULL);
  }
}

/* Pole hodnot o velikosti size_of_hodnota*keynum, p_keys*size_of_hodnota
   najdu frame a vlozim to tam
*/
int key_track_klic_vloz(byte **p_hodnoty, KEY_POINT_BRS **p_skeys, int keynum, int size_of_hodnota, byte *p_new_hodnota, KEY_POINT_BRS *p_new_keys)
{
  KEY_POINT_BRS *p_nkeys;
  byte          *p_nhod;
  KEY_POINT_BRS *p_keys = *p_skeys;
  byte          *p_hod  = *p_hodnoty;
  int            i,j;
  dword          new_time = p_new_keys->time;
  
  p_nhod = (byte *)mmalloc(size_of_hodnota*(keynum+1));
  p_nkeys = (KEY_POINT_BRS *)mmalloc(sizeof(p_nkeys[0])*(keynum+1));
  
  // Vlozit pred nulovym framem
  if(keynum && new_time < p_keys[0].time) {
    p_nkeys[0] = *p_new_keys;
    memcpy(p_nhod,p_new_hodnota,size_of_hodnota);
    memcpy(p_nkeys+1,p_keys,sizeof(p_nkeys[0])*keynum);
    memcpy(p_nhod+size_of_hodnota,p_hod,size_of_hodnota*keynum);
  // Vlozit za nulovym framem
  } else if(!keynum || p_keys[keynum-1].time < new_time) {
    memcpy(p_nkeys,p_keys,sizeof(p_nkeys[0])*keynum);
    memcpy(p_nhod,p_hod,size_of_hodnota*keynum);
    p_nkeys[keynum] = *p_new_keys;
    memcpy(p_nhod+keynum*size_of_hodnota,p_new_hodnota,size_of_hodnota);
  } else {
  // kopie vseho bez noveho framu
    for(i = 0,j = 0; i < keynum; i++,j++) {
      if(p_keys[i].time <= new_time && new_time < p_keys[i+1].time) {
        p_nkeys[j] = p_keys[i];
        memcpy(p_nhod+j*size_of_hodnota,p_hod+i*size_of_hodnota,size_of_hodnota);
        j++;
        p_nkeys[j] = *p_new_keys;
        memcpy(p_nhod+j*size_of_hodnota,p_new_hodnota,size_of_hodnota);
      } else {
        p_nkeys[j] = p_keys[i];
        memcpy(p_nhod+j*size_of_hodnota,p_hod+i*size_of_hodnota,size_of_hodnota);
      }
    }
  }    
  free(p_keys);
  free(p_hod);
  
  *p_hodnoty = p_nhod;
  *p_skeys = p_nkeys;

  return(keynum+1);
}

// Automaticke vlozeni klice do seznamu
int key_track_klic_vloz_auto(byte **p_hodnoty, KEY_POINT_BRS **p_skeys, int keynum, int size_of_hodnota, byte *p_new_hodnota, KEY_POINT_BRS *p_new_keys)
{
  int klic;

  if((klic = key_je_frame_klic(*p_skeys,keynum,p_new_keys->time)) == K_CHYBA) {    
    return(key_track_klic_vloz(p_hodnoty, p_skeys, keynum, size_of_hodnota, 
                               p_new_hodnota, p_new_keys));
  } else {
    memcpy((*p_skeys)+klic,p_new_keys,sizeof(p_new_keys[0]));    
    memcpy((*p_hodnoty)+klic*size_of_hodnota,p_new_hodnota,size_of_hodnota);
    return(keynum);
  }  
}

int key_track_klic_smaz(byte **p_hodnoty, KEY_POINT_BRS **p_skeys, int *p_keynum, int size_of_hodnota, dword del_time)
{
  KEY_POINT_BRS *p_nkeys;
  byte          *p_nhod;
  KEY_POINT_BRS *p_keys = *p_skeys;
  byte          *p_hod  = *p_hodnoty;  
  int            i,j,keynum = *p_keynum;
  
  if(keynum == 1) {    
    null_free((void **)p_hodnoty);
    null_free((void **)p_skeys);
    return((*p_keynum = 0));
  } else {
    p_nhod = (byte *)mmalloc(size_of_hodnota*(keynum-1));
    p_nkeys = (KEY_POINT_BRS *)mmalloc(sizeof(p_nkeys[0])*(keynum-1));
    
    if(p_keys[keynum-1].time == del_time) {
      memcpy(p_nkeys,p_keys,sizeof(p_nkeys[0])*(keynum-1));
      memcpy(p_nhod, p_hod, size_of_hodnota*(keynum-1));
    } else {
      // kopie vseho bez noveho framu
      for(i = 0,j = 0; i < keynum; i++,j++) {
        if(p_keys[i].time != del_time) {
          p_nkeys[j] = p_keys[i];
          memcpy(p_nhod+j*size_of_hodnota,p_hod+i*size_of_hodnota,size_of_hodnota);
        } else {
          j--;
        }
      }
    }
    
    free(p_keys);
    free(p_hod);
    
    *p_hodnoty = p_nhod;
    *p_skeys = p_nkeys; 
    return((*p_keynum)--);
  }   
}

/*****************************************************************************
 Key-frame animace
 *****************************************************************************
*/
void key_pivot_sub(EDIT_OBJEKT *p_obj, BOD *p_pivot)
{
 int v;

 for(v = 0; v < p_obj->vertexnum; v++) {
   p_obj->p_vertex[v].x -= p_pivot->x;
   p_obj->p_vertex[v].y -= p_pivot->y;
   p_obj->p_vertex[v].z -= p_pivot->z;
 }
}

void key_tri_to_matrix_zero(GLMATRIX *p_m, HIERARCHY_TRACK_INFO *p_track)
{  
  if(p_track) {
    init_matrix(p_m);

    if(p_track->rot_keys)
      quat_to_matrix(p_m,p_track->p_at);

    if(p_track->scs_keys) {    
      scale_matrix(p_m,p_track->p_scale[0].x,
        p_track->p_scale[0].y,
        p_track->p_scale[0].z);
    }
    if(p_track->pos_keys) {
      p_m->_41 = p_track->p_pos[0].x;
      p_m->_42 = p_track->p_pos[0].y;
      p_m->_43 = p_track->p_pos[0].z;
    }
  }
}

static int  key_konec_prochazeni;

/* Rekurzivne prida tri do tri-stromu
*/
static void key_tri_node_pridej(HIERARCHY_TRACK_INFO *p_tri, 
                                HIERARCHY_TRACK_INFO *p_child)
{
  int i;

  if(key_konec_prochazeni)
    return;

  if(p_tri->objnum == p_child->parentnum) {
    key_tri_child_pridej(p_tri, p_child);
    key_konec_prochazeni = TRUE;
  } 
  else {
    for(i = 0; i < p_tri->childnum && !key_konec_prochazeni; i++) {
       key_tri_node_pridej(p_tri->p_child[i],p_child);
    }
  }
}

/* rekurzivne prida tri node do rootu
*/
static void key_root_node_pridej(HIERARCHY_ROOT *p_root,
                                 HIERARCHY_TRACK_INFO *p_child)
{
  int i;

  if(p_child->parentnum == ROOT_NODE) {
    key_root_child_pridej(p_root, p_child);
    key_konec_prochazeni = TRUE;
  } else {
    for(i = 0; i < p_root->childnum && !key_konec_prochazeni; i++)
      key_tri_node_pridej(p_root->p_child[i],p_child);
  }
}

/* vyrobi root animacni strom
*/
HIERARCHY_ROOT * key_root_vyrob_indir(EDIT_KONTEJNER *p_kont, HIERARCHY_ROOT *p_root)
{ 
  HIERARCHY_TRACK_INFO *p_tri;
  TRACK_INFO  *p_track;
  EDIT_OBJEKT *p_obj;
  //int objnum = p_kont->objektu;
  int o;

  // nastaveni zakladnich veci
  for(o = 0; o < p_kont->max_objektu; o++) {
    if(p_kont->p_obj[o] && p_kont->p_obj[o]->p_track)
      break;
  }
  
  if(o >= p_kont->max_objektu) {
    //assert(0);
    return(NULL);
  }
  
  p_track = p_kont->p_obj[o]->p_track;
  p_root->framenum = p_track->framenum;
  p_root->delka = (float)p_root->framenum/(float)FRAMU_PER_SEC;

  // kopie vsech tracku + provazani objektu se scenou
  for(o = 0; o < p_kont->max_objektu; o++) {
    p_obj = p_kont->p_obj[o];
    if(p_obj && (p_track = p_obj->p_track)) { // pokud tento objekt ma track
      
      p_tri = key_track_to_tri(p_obj->p_track);
      p_tri->p_obj = p_obj;
      key_zrus_track(&p_obj->p_track);

      key_konec_prochazeni = FALSE;
      key_root_node_pridej(p_root,p_tri);
      assert(key_konec_prochazeni);
    }
  }
   
  return(p_root);
}

static float key_najdi_otcovy_scaly(HIERARCHY_TRACK_INFO *p_track)
{
  if(p_track->p_otec == NULL)
    return((p_track->scs_keys) ? p_track->p_scale[0].x : 1.0f);
  else
    return((p_track->scs_keys) ? p_track->p_scale[0].x*key_najdi_otcovy_scaly(p_track->p_otec) : key_najdi_otcovy_scaly(p_track->p_otec));
}

static void key_tri_to_local(HIERARCHY_TRACK_INFO *p_track)
{
  EDIT_OBJEKT *p_obj;
  GLMATRIX m;
  int i;
    
  p_obj = (EDIT_OBJEKT *)p_track->p_obj;
  
  if(key_najdi_otcovy_scaly(p_track) < 0.0f)
    scale_matrix(&p_obj->m,-1.0f,1.0f,1.0f);

  // proval to matici a jedeme
  transformuj_objekt_matici(p_obj,invert_matrix(&p_obj->m,&m));

  if(p_track->childnum == 1) {
    key_tri_to_local(p_track->p_child[0]);
  } else {
    for(i = 0; i < p_track->childnum; i++) {
      key_tri_to_local(p_track->p_child[i]);
    }
  }
}

/*
static void key_tri_to_local(HIERARCHY_TRACK_INFO *p_track)
{
  EDIT_OBJEKT *p_obj;
  int i;
    
  p_obj = (EDIT_OBJEKT *)p_track->p_obj;
  

  if(key_najdi_otcovy_scaly(p_track) < 0.0f) {        
    //scale_matrix(&p_obj->local_matrix,-1.0f,1.0f,1.0f);
  }

  if(p_track->childnum == 1) {
    key_tri_to_local(p_track->p_child[0]);
  } else {
    for(i = 0; i < p_track->childnum; i++) {
      key_tri_to_local(p_track->p_child[i]);
    }
  }
}
*/
static void key_tri_to_world(HIERARCHY_TRACK_INFO *p_track)
{  
  EDIT_OBJEKT *p_obj,*p_otec;
  int i;
          
  p_obj = (EDIT_OBJEKT *)p_track->p_obj;
  key_tri_to_matrix_zero(&p_obj->m, p_track);
    
  if(p_track->p_otec) {
    p_otec = (EDIT_OBJEKT *)p_track->p_otec->p_obj;
    mat_mult(&p_obj->m,&p_otec->m,&p_obj->m);
  }

  pivotuj_matrix_tam(&p_track->pivot,&p_obj->m);

  if(p_track->childnum == 1) {
    key_tri_to_world(p_track->p_child[0]);
  } else {
    for(i = 0; i < p_track->childnum; i++) {
      key_tri_to_world(p_track->p_child[i]);
    }
  }
}

/*
static void key_tri_to_world(HIERARCHY_TRACK_INFO *p_track)
{  
  EDIT_OBJEKT *p_obj,*p_otec;
  int i;
          
  p_obj = (EDIT_OBJEKT *)p_track->p_obj;
  //key_tri_to_matrix_zero(&p_obj->m, p_track);
    
  if(p_track->p_otec) {
    p_otec = (EDIT_OBJEKT *)p_track->p_otec->p_obj;
    mat_mult(&p_obj->m,&p_otec->m,&p_obj->m);
  }

//  pivotuj_matrix_tam(&p_track->pivot,&p_obj->m);

  if(p_track->childnum == 1) {
    key_tri_to_world(p_track->p_child[0]);
  } else {
    for(i = 0; i < p_track->childnum; i++) {
      key_tri_to_world(p_track->p_child[i]);
    }
  }
}
*/

/* Prevede root do nulove vychozi pozice
*/
void key_root_to_zero(HIERARCHY_ROOT *p_root)
{
  int i;
  for(i = 0; i < p_root->childnum; i++) {
    key_tri_to_local(p_root->p_child[i]);
    key_tri_to_world(p_root->p_child[i]);
  }
}

/* rekurzivni animace stromu/objektu - 
   predavat i horni matici -> jako horni matice 0 je word-matrix !
*/
void key_tri_to_matrix(HIERARCHY_TRACK_INFO *p_track, int time, 
                      GLMATRIX *p_otec, int loop)
{  
  EDIT_OBJEKT *p_obj;
  GLMATRIX m;
  QUAT q;
  BOD p;
  int i;

  // spocitej hierarchii
  p_obj = (EDIT_OBJEKT *)p_track->p_obj;
  
  // spocitej pozici + ostatni veci
  init_matrix(&m);

  if(p_track->p_pos) {    
    key_track_interpolace_bod(&p,p_track->p_pos,p_track->p_pkeys,time,p_track->endtime,p_track->pos_keys,loop);
    m._41 = p.x;
    m._42 = p.y;
    m._43 = p.z;
  }

  if(p_track->p_at) {    
    key_track_interpolace_quat(&q,p_track->p_at,p_track->p_rkeys,time,p_track->endtime,p_track->rot_keys,loop);
    quat_to_matrix(&m,&q);    
  }

  if(p_track->p_scale) {    
    key_track_interpolace_bod(&p,p_track->p_scale,p_track->p_skeys,time,p_track->endtime,p_track->scs_keys,loop);

    m._11 *= p.x;
    m._21 *= p.x;
    m._31 *= p.x;
    
    m._12 *= p.y;
    m._22 *= p.y;
    m._32 *= p.y;
    
    m._13 *= p.z;
    m._23 *= p.z;
    m._33 *= p.z;
  }

  if(p_track->p_at) {
    pivotuj_matrix_tam(&p_track->pivot,&m);
  }

  // spocitej hierarchii
  p_obj = (EDIT_OBJEKT *)p_track->p_obj;
  mat_mult(&m,p_otec,&p_obj->m); // aktualni*otec => vysledek

  if(p_track->childnum == 1) {
    key_tri_to_matrix(p_track->p_child[0], time, &p_obj->m, loop);
  } else {
    for(i = 0; i < p_track->childnum; i++) {
      key_tri_to_matrix(p_track->p_child[i], time, &p_obj->m, loop);
    }
  }
}
/*
void key_tri_to_matrix(HIERARCHY_TRACK_INFO *p_track, int time, GLMATRIX *p_otec, int loop)
{  
  EDIT_OBJEKT *p_obj;
  GLMATRIX m;
  QUAT q;
  BOD p;
  int i;

  // spocitej hierarchii
  p_obj = (EDIT_OBJEKT *)p_track->p_obj;   

  // spocitej pozici + ostatni veci
  init_matrix(&m);

  if(p_track->p_pos) {
    key_track_interpolace_bod(&p,p_track->p_pos,p_track->p_pkeys,time,p_track->endtime,p_track->pos_keys,loop);
    m._41 = p.x;
    m._42 = p.y;
    m._43 = p.z;
  }

  if(p_track->p_at) {
    key_track_interpolace_quat(&q,p_track->p_at,p_track->p_rkeys,time,p_track->endtime,p_track->rot_keys,loop);
    quat_to_matrix(&m,&q);
  }

  if(p_track->p_scale) {
    key_track_interpolace_bod(&p,p_track->p_scale,p_track->p_skeys,time,p_track->endtime,p_track->scs_keys,loop);
    scale_matrix(&m,p.x,p.y,p.z);
  }

  // spocitej hierarchii
  p_obj = (EDIT_OBJEKT *)p_track->p_obj;
  mat_mult(&m,p_otec,&m); // aktualni*otec => vysledek

  p_obj->m = m;  
  pivotuj_matrix_tam(&p_track->pivot,&p_obj->m);
  
  mat_mult(&p_obj->local_matrix,&p_obj->m,&p_obj->m);
  
  if(p_track->childnum == 1) {
    key_tri_to_matrix(p_track->p_child[0], time, &m, loop);
  } else {
    for(i = 0; i < p_track->childnum; i++) {
      key_tri_to_matrix(p_track->p_child[i], time, &m, loop);
    }
  }
}
*/

void key_root_to_matrix(HIERARCHY_ROOT *p_root, int time, GLMATRIX *p_otec)
{
  GLMATRIX m;
  int i;

  if(!p_otec) {
    p_otec = init_matrix(&m);
  }
  
  for(i = 0; i < p_root->childnum; i++)
    key_tri_to_matrix(p_root->p_child[i], time, p_otec, p_root->flag&GK_LOOP);
}

// track veci
TRACK_INFO * key_track_vyrob(int poskey, int rotkey, int scalekey)
{
  TRACK_INFO *p_track;

  p_track = (TRACK_INFO *)mmalloc(sizeof(TRACK_INFO));

  // pozicni klice
  p_track->pos_keys = poskey;  
  p_track->p_pkeys = (KEY_POINT_BRS *)mmalloc(sizeof(p_track->p_pkeys[0])*poskey);
  p_track->p_pos = (BOD *)mmalloc(sizeof(BOD)*poskey);
  
  // rotacni klice
  p_track->rot_keys = rotkey;
  p_track->p_rkeys = (KEY_POINT_BRS *)mmalloc(sizeof(p_track->p_rkeys[0])*rotkey);
  p_track->p_rot = (ROTKEY *)mmalloc(sizeof(ROTKEY)*rotkey);
  p_track->p_at = (QUAT *)mmalloc(sizeof(p_track->p_at[0])*rotkey);

  // scalovaci klice
  p_track->scs_keys = scalekey;
  p_track->p_skeys = (KEY_POINT_BRS *)mmalloc(sizeof(p_track->p_skeys[0])*scalekey);
  p_track->p_scale = (BOD *)mmalloc(sizeof(BOD)*scalekey);
  
  return(p_track);
}

HIERARCHY_TRACK_INFO * key_tri_vyrob_indir(HIERARCHY_TRACK_INFO *p_hir, int poskey, int rotkey, int scalekey)
{
  
  memset(p_hir,0,sizeof(p_hir[0]));

  // pozicni klice
  if((p_hir->pos_keys = poskey)) {
    p_hir->p_pkeys = (KEY_POINT_BRS *)mmalloc(sizeof(p_hir->p_pkeys[0])*poskey);
    p_hir->p_pos = (BOD *)mmalloc(sizeof(BOD)*poskey);
  }
  
  // rotacni klice
  if((p_hir->rot_keys = rotkey)) {
    p_hir->p_rkeys = (KEY_POINT_BRS *)mmalloc(sizeof(p_hir->p_rkeys[0])*rotkey);
    p_hir->p_at = (QUAT *)mmalloc(sizeof(p_hir->p_at[0])*rotkey);
  }

  // scalovaci klice
  if((p_hir->scs_keys = scalekey)) {
    p_hir->p_scale = (BOD *)mmalloc(sizeof(BOD)*scalekey);
    p_hir->p_skeys = (KEY_POINT_BRS *)mmalloc(sizeof(p_hir->p_skeys[0])*scalekey);
  }
  
  return(p_hir);
}

HIERARCHY_TRACK_INFO * key_tri_vyrob(int poskey, int rotkey, int scalekey)
{
  HIERARCHY_TRACK_INFO *p_hir;

  p_hir = (HIERARCHY_TRACK_INFO *)mmalloc(sizeof(HIERARCHY_TRACK_INFO));
  key_tri_vyrob_indir(p_hir, poskey, rotkey, scalekey);
  return(p_hir);
}

HIERARCHY_TRACK_INFO * key_tri_vyrob_un_indir(void)
{
  HIERARCHY_TRACK_INFO *p_hir;
  p_hir = (HIERARCHY_TRACK_INFO *)mmalloc(sizeof(HIERARCHY_TRACK_INFO));
  return(p_hir);
}

// kopie tracku
HIERARCHY_TRACK_INFO * key_track_to_tri( TRACK_INFO *p_track)
{
 HIERARCHY_TRACK_INFO *p_tri = key_tri_vyrob(p_track->pos_keys,
                       p_track->rot_keys,p_track->scs_keys);

 p_tri->objnum = p_track->objekt;
 p_tri->parentnum = p_track->parent;
 p_tri->pivot = p_track->pivot;
 p_tri->endtime = calc_endtime(p_track->framenum);

 if(p_track->pos_keys) {
   memcpy(p_tri->p_pkeys, p_track->p_pkeys, sizeof(p_track->p_pkeys[0])*p_track->pos_keys);
   memcpy(p_tri->p_pos, p_track->p_pos, sizeof(p_track->p_pos[0])*p_track->pos_keys);
 }

 if(p_track->rot_keys) {
   memcpy(p_tri->p_rkeys, p_track->p_rkeys, sizeof(p_track->p_rkeys[0])*p_track->rot_keys);
   memcpy(p_tri->p_at, p_track->p_at, sizeof(p_track->p_at[0])*p_track->rot_keys);
 }
 
 if(p_track->scs_keys) {
   memcpy(p_tri->p_skeys, p_track->p_skeys, sizeof(p_track->p_skeys[0])*p_track->scs_keys);
   memcpy(p_tri->p_scale, p_track->p_scale, sizeof(p_track->p_scale[0])*p_track->scs_keys);
 }

 return(p_tri);
}

void key_zrus_track(TRACK_INFO **p_track)
{
  TRACK_INFO *p_tri = *p_track;
  null_free((void **)&p_tri->p_pkeys);
  null_free((void **)&p_tri->p_pos);
  null_free((void **)&p_tri->p_rkeys);
  null_free((void **)&p_tri->p_rot);
  null_free((void **)&p_tri->p_at);
  null_free((void **)&p_tri->p_skeys);
  null_free((void **)&p_tri->p_scale);
  null_free((void **)&p_tri);
  *p_track = NULL;
}

// prida child do track_node
void key_tri_child_pridej(HIERARCHY_TRACK_INFO *p_root, HIERARCHY_TRACK_INFO *p_child)
{
  p_root->childnum += 1;
  p_root->p_child = (HIERARCHY_TRACK_INFO **)realloc(p_root->p_child,sizeof(p_root->p_child[0])*p_root->childnum);
  p_root->p_child[p_root->childnum-1] = p_child;
  p_child->p_otec = p_root;
}

// prida child do root_node
void key_root_child_pridej(HIERARCHY_ROOT *p_root, HIERARCHY_TRACK_INFO *p_child)
{
  p_root->childnum += 1;
  p_root->p_child = (HIERARCHY_TRACK_INFO **)realloc(p_root->p_child,sizeof(p_root->p_child[0])*p_root->childnum);
  p_root->p_child[p_root->childnum-1] = p_child;
  p_child->p_otec = NULL;
}

void key_tri_calc_absolutne(TRACK_INFO *p_track, QUAT *p_puvodni)
{
  QUAT q(0,0,0,1);
  word i;

  if(p_track->rot_keys) {
    if(!p_track->p_at) {
      p_track->p_at = (QUAT *)mmalloc(sizeof(p_track->p_at[0])*p_track->rot_keys);
    }    
    key_rotkey_to_quat(p_track->p_rot,p_track->p_at);
    for(i = 1; i < p_track->rot_keys; i++) {
      quat_mult(key_rotkey_to_quat(p_track->p_rot+i,&q),p_track->p_at+(i-1),p_track->p_at+i);
    }
  }
}

void key_sim_calc_absolutne(SIMPLE_TRACK_INFO *p_sim)
{
  QUAT q(0,0,0,1),qz;
  int i,il;

  if(p_sim->p_at) {    
    for(i = 0; i < p_sim->keynum; i++) {
       if(p_sim->p_at[i].x != FLT_MAX)
         break;
    }

    angle_to_quat(&qz,(BOD *)(&p_sim->p_at[i].x),p_sim->p_at[i].w);
    p_sim->p_at[i] = qz;
    il = i;

    for(i++; i < p_sim->keynum; i++) {
      if(p_sim->p_at[i].x != FLT_MAX) {
        angle_to_quat(&qz,(BOD *)&(p_sim->p_at[i].x),p_sim->p_at[i].w);    
        quat_mult(&qz,p_sim->p_at+il,p_sim->p_at+i);
        il = i;
      }
    }
  }
}

/*
  0. Key_frame prevest na matice
  1. Vynasobit word matrix
  2. Prevest zpet
  
  BOD *p_pos, QUAT *p_q, BOD *p_scs
*/

// cilovy kontejner - kopie tracku
static void key_tri_to_tri_indir(EDIT_KONTEJNER *p_kont,
                                 HIERARCHY_TRACK_INFO *p_cil, 
                                 HIERARCHY_TRACK_INFO *p_src,
                                 HIERARCHY_TRACK_INFO *p_otec)
{
  int i;

  p_cil->childnum = p_src->childnum;
  p_cil->endtime = p_src->endtime;
  p_cil->objnum = p_src->objnum;
  p_cil->parentnum = p_src->parentnum;
  p_cil->pivot = p_src->pivot;
  p_cil->p_obj = p_kont->p_obj[p_src->objnum];
  p_cil->p_otec = p_otec;
  p_cil->p_m = &p_kont->p_obj[p_src->objnum]->m;

  if(p_cil->pos_keys) {
    memcpy(p_cil->p_pkeys,p_src->p_pkeys,sizeof(p_src->p_pkeys[0])*p_cil->pos_keys);
    memcpy(p_cil->p_pos,p_src->p_pos,sizeof(p_src->p_pos[0])*p_cil->pos_keys);
  }

  if(p_cil->scs_keys) {    
    memcpy(p_cil->p_scale,p_src->p_scale,sizeof(p_src->p_scale[0])*p_cil->scs_keys);
  }

  if(p_cil->rot_keys) {    
    memcpy(p_cil->p_rkeys,p_src->p_rkeys,sizeof(p_src->p_rkeys[0])*p_cil->rot_keys);
    memcpy(p_cil->p_at,p_src->p_at,sizeof(p_src->p_at[0])*p_cil->rot_keys);
  }

  p_cil->p_child = (HIERARCHY_TRACK_INFO **)mmalloc(sizeof(p_cil->p_child[0])*p_src->childnum);
  for(i = 0; i < p_src->childnum; i++) {
    p_cil->p_child[i] = key_tri_vyrob(p_src->p_child[i]->pos_keys, p_src->p_child[i]->rot_keys, p_src->p_child[i]->scs_keys);
    key_tri_to_tri_indir(p_kont, p_cil->p_child[i], p_src->p_child[i], p_cil);
  }
}

void key_root_to_root_indir(EDIT_KONTEJNER *p_kont, HIERARCHY_ROOT *p_cil, HIERARCHY_ROOT *p_src)
{
  int i;

  *p_cil = *p_src;

  p_cil->p_child = (HIERARCHY_TRACK_INFO **)mmalloc(sizeof(p_cil->p_child[0])*p_src->childnum);
  for(i = 0; i < p_src->childnum; i++) {
    p_cil->p_child[i] = key_tri_vyrob(p_src->p_child[i]->pos_keys, p_src->p_child[i]->rot_keys, p_src->p_child[i]->scs_keys);
    key_tri_to_tri_indir(p_kont, p_cil->p_child[i], p_src->p_child[i], NULL);
  }
}

/* kopie sim-animaci
*/
/*
typedef struct _SIMPLE_TRACK_INFO {
  
  int      Objekt_ID;      // id objektu ke kteremu animace patri
  dword    flag;
  
  int      keynum;         // pocet klicu
  BOD     *p_pos;          // Mesh position keys
  BOD     *p_scale;        // Mesh scaling keys
  QUAT    *p_at;           // Absolutni traky
  BOD      pivot;          // pivot point animace -> kopiruje se do objektu
  BOD      pivot3ds;       // pivot 3ds animace

  GLMATRIX *p_m;            // kam to hazet
  
  int       childnum;      // pocet svistich traku
  struct   _SIMPLE_TRACK_INFO *p_child; // svisti traky
  struct   _SIMPLE_TRACK_INFO *p_otec; // otcovy traky
  
} SIMPLE_TRACK_INFO;
*/

// Kopie sim-animaci
void key_sim_root_to_sim_root_indir(EDIT_KONTEJNER *p_kont, HIERARCHY_SIM *p_cil, HIERARCHY_SIM *p_src)
{
  int i;

  *p_cil = *p_src;

  p_cil->p_child = (SIMPLE_TRACK_INFO *)mmalloc(sizeof(p_cil->p_child[0])*p_src->childnum);
  for(i = 0; i < p_src->childnum; i++) {
    key_sim_to_sim_indir(p_cil->p_child+i, p_src->p_child+i);
  }
  key_sim_dopln_matrix_kont(p_kont, p_cil);
}

static void key_sim_dopln_matrix_kont_rec(EDIT_KONTEJNER *p_kont, SIMPLE_TRACK_INFO *p_sim)
{
  EDIT_OBJEKT *p_obj;
  int i;

  p_obj = lo_najdi_objekt_kont_poiter_ID(p_kont,p_sim->Objekt_ID);
  if(p_obj) {
    p_sim->p_m = &p_obj->m;
    p_sim->p_object_norm = &p_obj->float_scale;
  } else {
    p_sim->p_m = NULL;
    p_sim->p_object_norm = NULL;
  }

  for(i = 0; i < p_sim->childnum; i++) {
    key_sim_dopln_matrix_kont_rec(p_kont,p_sim->p_child+i);
  }
}

// Doplni matice
void key_sim_dopln_matrix_kont(EDIT_KONTEJNER *p_kont, HIERARCHY_SIM *p_src)
{
  int i;
  for(i = 0; i < p_src->childnum; i++) {
    key_sim_dopln_matrix_kont_rec(p_kont,p_src->p_child+i);
  }
}


static void key_sim_dopln_matrix_mesh_rec(GAME_MESH_OLD *p_mesh, SIMPLE_TRACK_INFO *p_sim)
{  
  int i;

  for(i = 0; i < p_mesh->objektu; i++) {
    if(p_mesh->p_Objekt_ID[i] == p_sim->Objekt_ID) {
      p_sim->p_m = p_mesh->p_key+i;
      p_sim->p_obb_local = p_mesh->p_obb_local+i;
      p_sim->p_obb_world = p_mesh->p_obb_world+i;
      p_sim->p_object_norm = p_mesh->p_normal_scale ? p_mesh->p_normal_scale+i : NULL;
      for(i = 0; i < p_sim->childnum; i++) {
        key_sim_dopln_matrix_mesh_rec(p_mesh,p_sim->p_child+i);
      }      
      return;
    }
  }

  // fuck-ati
}

void key_sim_dopln_matrix_mesh(GAME_MESH_OLD *p_mesh, SIMPLE_TRACK_INFO *p_sim)
{
  int i;
  for(i = 0; i < p_sim->childnum; i++) {
    key_sim_dopln_matrix_mesh_rec(p_mesh,p_sim->p_child+i);
  }
}

/*
typedef struct _HIERARCHY_SIM {
  
  float     delka;         // delka casu animace (fps)
  byte      stav;          // 0 = stop, 1 = run
  
  int       flag;          // flagy animace (GL_LOOP,GL_REMOVE)

  dword     time;          // aktualni cas animace
  dword     time_start;    // aktualni start
  dword     time_stop;     // aktualni stop
  dword     time_delka;    // aktualni delka animace
  
  dword     start;
  dword     stop;  
  
  int       keynum;         // pocet klicu
  char      jmeno[MAX_JMENO]; // jmeno animace
  int       childnum;         // pocet detskych traku
  SIMPLE_TRACK_INFO *p_child;

} HIERARCHY_SIM;
*/

/* Rekurzivni extra-interpolator
*/
static void key_root_to_sim_indir_rec(EDIT_KONTEJNER *p_kont, SIMPLE_TRACK_INFO *p_sim, HIERARCHY_TRACK_INFO *p_root, int loop, int float_scale)
{

  /* Kopie puvodnich traku
  */
  key_tri_to_sim_indir(p_root,p_sim,loop,float_scale);

  p_sim->p_m = &p_kont->p_obj[p_root->objnum]->m;
  p_sim->Objekt_ID = p_kont->p_obj[p_root->objnum]->Objekt_ID;
  p_sim->pivot = p_root->pivot_sim;
  

  /* Vytvoreni stromu
  */
  if(p_root->childnum) {
    int i;

    p_sim->childnum = p_root->childnum;
    
    p_sim->p_child = (SIMPLE_TRACK_INFO *)mmalloc(sizeof(p_sim->p_child[0])*p_root->childnum);    

    for(i = 0; i < p_sim->childnum; i++) {
      p_sim->p_child[i].p_otec = p_sim;
      key_root_to_sim_indir_rec(p_kont, p_sim->p_child+i, p_root->p_child[i],loop,float_scale);
    }
  }
}

/* Prevod root - stromu do simple animacniho stromu
*/
void key_root_to_sim_indir(EDIT_KONTEJNER *p_kont, HIERARCHY_SIM *p_sim, HIERARCHY_ROOT *p_root, int float_scale)
{
  int i;

  if(!p_root->childnum)
    return;

  strcpy(p_sim->jmeno,p_root->jmeno);
  p_sim->childnum = p_root->childnum;
  p_sim->keynum = p_root->framenum;
  
  p_sim->p_child = (SIMPLE_TRACK_INFO *)mmalloc(sizeof(p_sim->p_child[0])*p_root->childnum);
  for(i = 0; i < p_sim->childnum; i++) {
    key_root_to_sim_indir_rec(p_kont, p_sim->p_child+i, p_root->p_child[i], p_root->flag&GK_LOOP, float_scale);
  }
  
  key_sim_dopln_matrix_kont(p_kont,p_sim);
}

/* Reanimuje kontejner word-matrix - vkladat akt_animace
*/
void key_kontejner_reanimuj(EDIT_KONTEJNER *p_kont, HIERARCHY_ROOT *p_root)
{
  if(p_kont->kflag&KONT_KEYFRAME && p_root->framenum) {
    key_root_to_matrix(p_root,0,kont_world_matrix(p_kont));
  }
}

void key_kontejner_sim_reanimuj(EDIT_KONTEJNER *p_kont)
{
  if(p_kont->kflag&KONT_KEYFRAME && p_kont->sim[p_kont->akt_animace].keynum) {
    key_sim_root_to_matrix(p_kont,p_kont->sim+p_kont->akt_animace,0,kont_world_matrix(p_kont));
  }
}

void key_kontejner_sim_reanimuj_pivoty(EDIT_KONTEJNER *p_kont)
{
  HIERARCHY_SIM *p_sim;
  GLMATRIX       m;
  int i;

  init_matrix(&m);
  if(p_kont->kflag&KONT_KEYFRAME) {
    p_sim = p_kont->sim+p_kont->akt_animace;
    if(p_sim->keynum) {      
      for(i = 0; i < p_sim->childnum; i++) {        
        key_sim_animuj_strom_pivoty(p_sim->p_child+i, 0, &m);
      }
    }
  }
}

void key_kontejner_retransformuj(EDIT_KONTEJNER *p_kont_top)
{
  EDIT_KONTEJNER *p_kont = p_kont_top;
  EDIT_OBJEKT *p_obj;
  int o;
  while(p_kont) {
    if(p_kont->kflag&KONT_KEYFRAME) {
      for(o = 0; o < MAX_KONT_OBJEKTU; o++) {
        p_obj = p_kont->p_obj[o];
        if(p_obj) {
          transformuj_objekt_matici(p_obj,&p_obj->m);
          init_matrix(&p_obj->m);
        }
      }    
    }
    p_kont = p_kont->p_next;
  }
}

void key_mesh_reanimuj(GAME_MESH_OLD *p_mesh, int time, GLMATRIX *p_top)
{
  SIMPLE_TRACK_INFO *p_sim = p_mesh->p_sim_aktivni;
  int i;

  assert(p_mesh->p_data->kflag&KONT_KEYFRAME);

  for(i = 0; i < p_sim->childnum; i++) {
    key_sim_animuj_strom(p_sim->p_child+i, time, p_top);
  }

  // Transformace obalky meshe
  obb_transformuj(&p_mesh->obb_local,p_top,&p_mesh->obb_world);

  key_mesh_recalc_normal_anim(p_mesh);
}

/**************************************************************************** 
  Simple-track animace
 ****************************************************************************
*/

/* kopie sim->sim
*/
void key_sim_to_sim_indir(SIMPLE_TRACK_INFO *p_dest, SIMPLE_TRACK_INFO *p_src)
{
  int i;

  *p_dest = *p_src;

  // kopie pozicnich klicu
  if(p_src->p_pos) {
    p_dest->p_pos = (BOD *)kopiruj_pole(p_src->p_pos,sizeof(p_src->p_pos[0])*p_src->keynum);
  }
  
  // kopie rotacnich klicu
  if(p_src->p_at) {
    p_dest->p_at = (QUAT *)kopiruj_pole(p_src->p_at,sizeof(p_src->p_at[0])*p_src->keynum);
  }

  // kopie scale klicu
  if(p_src->p_scale) {
    p_dest->p_scale = (BOD *)kopiruj_pole(p_src->p_scale,sizeof(p_src->p_scale[0])*p_src->keynum);
  }

  // Kopie zero-matic
  if(p_src->p_all_matrix) {
    p_dest->p_all_matrix = (GLMATRIX *)kopiruj_pole(p_src->p_all_matrix,sizeof(p_src->p_all_matrix[0]));
  }

  if(p_dest->childnum) {
    p_dest->p_child = (SIMPLE_TRACK_INFO *)mmalloc(sizeof(p_dest->p_child[0])*p_dest->childnum);
    for(i = 0; i < p_dest->childnum; i++) {
      key_sim_to_sim_indir(p_dest->p_child+i, p_src->p_child+i);
    }
  }
}

/* Rekurzivne animuje animacni strom
*/
void key_sim_animuj_strom(SIMPLE_TRACK_INFO *p_sim, int time, GLMATRIX *p_otec)
{  
  OBB_OLD *p_src,
      *p_dest;
  GLMATRIX m, *p_top = p_otec;
  int i;

  // animuj hierarchii
  if(p_sim->p_m) {
    p_top = mat_mult_dir(key_sim_to_matrix(p_sim,time,&m),p_otec,p_sim->p_m);
    p_src = p_sim->p_obb_local;
    p_dest = p_sim->p_obb_world;
    if(p_src && p_dest) {
      obb_transformuj(p_src,p_top,p_dest);
    }
  } 

  // rozmazni i decka
  for(i = 0; i < p_sim->childnum; i++) {
    key_sim_animuj_strom(p_sim->p_child+i, time, p_top);
  }
}

void key_sim_animuj_strom_pivoty(SIMPLE_TRACK_INFO *p_sim, int time, GLMATRIX *p_otec)
{
  GLMATRIX m,m1, *p_top = p_otec;
  int i;

  // animuj hierarchii
  if(p_sim->p_at || p_sim->p_pos || p_sim->p_scale) {
    p_top = mat_mult(key_sim_to_matrix(p_sim,time,&m),p_otec,&m1);
  } else {      
    p_top = p_otec;
  }

  // Transformuje akt. pivot
  invert_matrix_copy(p_top,&m);
  transformuj_bod_bod_matici(&p_sim->pivot,&m);

  // rozmazni i decka
  for(i = 0; i < p_sim->childnum; i++) {
    key_sim_animuj_strom_pivoty(p_sim->p_child+i, time, p_top);
  }
}

/*------------------------------------------------------------------
  Simple animace
  ------------------------------------------------------------------
*/
/* Prevede tri do simple tracku
*/
SIMPLE_TRACK_INFO * key_tri_to_sim_indir(HIERARCHY_TRACK_INFO *p_track, SIMPLE_TRACK_INFO *p_sim, int loop, int float_scale)
{
  int   i,endtime;

  // prevedu hlavni track
  memset(p_sim,0,sizeof(SIMPLE_TRACK_INFO));
  
  p_sim->keynum = calc_keynum(p_track->endtime);

  endtime = p_track->endtime;

  /* Kopie pozicnich klicu
  */
  if(p_track->p_pos) {
    p_sim->p_pos = (BOD *)mmalloc(sizeof(p_sim->p_pos[0])*p_sim->keynum);
    for(i = 0; i < p_sim->keynum; i++)
      key_track_interpolace_bod(p_sim->p_pos+i,p_track->p_pos,p_track->p_pkeys,
                               i*SIM_KONSTI,endtime,p_track->pos_keys,loop);
  }
  
  /* Kopie rotacnich klicu
  */
  if(p_track->p_at) {
    p_sim->p_at = (QUAT *)mmalloc(sizeof(p_sim->p_at[0])*p_sim->keynum);
    p_sim->pivot3ds = p_track->pivot;
    key_track_quat_zkontroluj(p_track->p_at,p_track->rot_keys);
    for(i = 0; i < p_sim->keynum; i++)
      key_track_interpolace_quat(p_sim->p_at+i,p_track->p_at,p_track->p_rkeys,
                                i*SIM_KONSTI,endtime,p_track->rot_keys,loop);
  }

  /* Kopie scale klicu
  */
  if(p_track->p_scale) {
    if(float_scale) {
      BOD s;
      p_sim->p_norm = (float *)mmalloc(sizeof(p_sim->p_norm[0])*p_sim->keynum);
      for(i = 0; i < p_sim->keynum; i++) {
        key_track_interpolace_bod(&s,p_track->p_scale,p_track->p_skeys,i*SIM_KONSTI,endtime,
          p_track->scs_keys,loop);
        p_sim->p_norm[i] = s.x;
      }
    } else {
      p_sim->p_scale = (BOD *)mmalloc(sizeof(p_sim->p_scale[0])*p_sim->keynum);
      for(i = 0; i < p_sim->keynum; i++)
        key_track_interpolace_bod(p_sim->p_scale+i,p_track->p_scale,p_track->p_skeys,
                                  i*SIM_KONSTI,endtime,p_track->scs_keys,loop);
    }
  }
  return(p_sim);
}

/* Prevede tri linearne do simple tracku
*/
SIMPLE_TRACK_INFO * key_tri_to_sim_indir_lin(HIERARCHY_TRACK_INFO *p_track, SIMPLE_TRACK_INFO *p_sim, int loop)
{
  int   i,keynum, endtime;

  // prevedu hlavni track
  memset(p_sim,0,sizeof(SIMPLE_TRACK_INFO));
  
  keynum = p_sim->keynum = calc_keynum(p_track->endtime);
  keynum--;
  endtime = p_track->endtime;

  /* Kopie pozicnich klicu
  */
  if(p_track->p_pos) {
    p_sim->p_pos = (BOD *)mmalloc(sizeof(p_sim->p_pos[0])*p_sim->keynum);
    for(i = 0; i < p_sim->keynum; i++)
      key_track_interpolace_bod_linear(p_sim->p_pos+i,p_track->p_pos,p_track->p_pkeys,
                                      i*SIM_KONSTI,endtime,p_track->pos_keys,
                                      loop);
  }
  
  /* Kopie rotacnich klicu
  */
  if(p_track->p_at) {
    p_sim->p_at = (QUAT *)mmalloc(sizeof(p_sim->p_at[0])*p_sim->keynum);
    p_sim->pivot3ds = p_track->pivot;
    key_track_quat_zkontroluj(p_track->p_at,p_track->rot_keys);
    for(i = 0; i < p_sim->keynum; i++)
      key_track_interpolace_quat_linear(p_sim->p_at+i,p_track->p_at,p_track->p_rkeys,
                                       i*SIM_KONSTI,endtime,p_track->rot_keys,
                                       loop);
  }

  /* Kopie scale klicu
  */
  if(p_track->p_scale) {
    p_sim->p_scale = (BOD *)mmalloc(sizeof(p_sim->p_scale[0])*p_sim->keynum);
    for(i = 0; i < p_sim->keynum; i++)
      key_track_interpolace_bod_linear(p_sim->p_scale+i,p_track->p_scale,p_track->p_skeys,
                                      i*SIM_KONSTI,endtime,p_track->scs_keys,
                                      loop);
  }

  return(p_sim);
}

void key_sim_zrus_vnitrek(SIMPLE_TRACK_INFO *p_sim)
{
  null_free((void **)&p_sim->p_pos);
  null_free((void **)&p_sim->p_at);
  null_free((void **)&p_sim->p_scale);
  null_free((void **)&p_sim->p_all_matrix);
}

// Zrusi animacni strom sim animaci
void key_sim_zrus(SIMPLE_TRACK_INFO *p_sim)
{
  int i;

  if(p_sim->p_child) {
    for(i = 0; i < p_sim->childnum; i++) {
      key_sim_zrus(p_sim->p_child+i);
    }
    free(p_sim->p_child);
  }
  key_sim_zrus_vnitrek(p_sim);
}

void key_sim_root_zrus(HIERARCHY_SIM *p_sim)
{
  int i;

  if(p_sim->p_child) {
    for(i = 0; i < p_sim->childnum; i++) {
      key_sim_zrus(p_sim->p_child+i);
    }
    free(p_sim->p_child);
  }  
  memset(p_sim,0,sizeof(p_sim[0]));
}

// ulozi simple animaci jako text-file
/*
C 20      // (pocet framu zacinajici od 0)
V 0 0 0   // pivot point

F 0       // Pro kazdy frame jeho cislo
P 0 2 3   // relativni posun x,y,z
R 0 0 0 0 // relativni rotace x,y,z + uhel
S 0 0 0   // relativni scale x y z

T 0       // tension
O 0       // continuity
B 0       // bias
L 0 0 0   // tension/continuity/bias
*/

void key_sim_uloz(SIMPLE_TRACK_INFO *p_sim, char *p_file, char *p_dir)
{
  BOD   b;
  float uhel;
  FILE *f;
  int   i;
  int   loop = p_sim->flag&GK_LOOP;

  chdir((p_dir));

  if(!(f = fopen(p_file,"w"))) {
    ddw("File %s Line %d Chyba otevreni souboru '%s' v %s",__FILE__,__LINE__,p_file,p_dir);
    return;
  }

  fprintf(f,"C %d\n",p_sim->keynum);
  if(loop)
    fputs("N\n",f);
  fprintf(f,"M %f %f %f",p_sim->pivot3ds.x,p_sim->pivot3ds.y,p_sim->pivot3ds.z);
  fprintf(f,"O %f %f %f",p_sim->pivot.x,p_sim->pivot.y,p_sim->pivot.z);
  for(i = 0; i < p_sim->keynum; i++) {
    fprintf(f,"F %d\n",i);
    fprintf(f,"P %f %f %f\n",p_sim->p_pos[i].x,p_sim->p_pos[i].y,p_sim->p_pos[i].z);
    quat_to_angle(p_sim->p_at+i,&b,&uhel);
    uhel=(uhel/PI)*180.0f;
    fprintf(f,"R %f %f %f %f\n",b.x,b.y,b.z,uhel);
    fprintf(f,"S %f %f %f\n",p_sim->p_scale[i].x,p_sim->p_scale[i].y,p_sim->p_scale[i].z);
  }
  
  fclose(f);
}

// load sim z text-file
int key_sim_nahraj(APAK_HANDLE *pHandle, SIMPLE_TRACK_INFO *p_sim, char *p_file, char *p_dir, int linear)
{
  HIERARCHY_TRACK_INFO track; // load jako hierarchie -> prevedu do simple tracku
  KEY_POINT_BRS       *p_spline = NULL;
  char                 line[1000],*p_pom;
  KFILE               *f;
  int                  p,r,s,fr,loop = 0;  
  BOD                  pivot(0.0f,0.0f,0.0f);
  BOD                  osa;
  float                uhel;
  char               **p_line;
  int                  l,lmax;

  chdir((p_dir));
  if(!(f = kopen(pHandle,p_file,"r"))) {
    kprintf(TRUE,"File %s Line %d Chyba otevreni souboru '%s' v %s",__FILE__,__LINE__,p_file,(p_dir));
    return(FALSE);
  } else {
    kprintf(TRUE,"Nahravam animaci z %s...",p_file);
  }

  p_line = (char **)mmalloc(sizeof(p_line[0])*MAX_SIM_RADKU);
  l = 0;
  while(kgets(line,999,f)) {
    assert(l < MAX_SIM_RADKU);
    if(line[0] && line[0] != '\n' && line[0] != ';') {
      p_line[l] = (char *)kopiruj_pole(line,sizeof(char)*(strlen(line)+1));
      l++;
    }
  }
  kclose(f);
  lmax = l;


  p = r = s = 0;
  for(l = 0; l < lmax; l++) {
    p_pom = p_line[l];
    switch(toupper(p_pom[0])) {
    case 'P': // posun
      p++;
      break;
    case 'R': // rotace
      r++;
      break;
    case 'S': // scale
      s++;
      break;
    default:
      break;
    }    
  }  
  
  key_tri_vyrob_indir(&track,p,r,s);

  p = r = s = fr = 0;
  for(l = 0; l < lmax; l++) {
    p_pom = p_line[l];
    switch(toupper(p_pom[0])) {
    case 'F': // frame
      sscanf(p_pom+1,"%d",&fr);
      break;
      
    case 'P': // posun
      track.p_pkeys[p].time = fr*SIM_KONSTI;
      p_spline = track.p_pkeys+p;
      sscanf(p_pom+1,"%f %f %f",&track.p_pos[p].x,&track.p_pos[p].y,&track.p_pos[p].z);
      p++;
      break;
    case 'R': // rotace
      track.p_rkeys[r].time = fr*SIM_KONSTI;
      p_spline = track.p_rkeys+r;
      sscanf(p_pom+1,"%f %f %f %f",&osa.x,&osa.y,&osa.z,&uhel);
      /* Korekce zaporneho uhlu
      */
      if(uhel < 0.0f) {
        uhel = -uhel;
        osa.x = -osa.x;
        osa.y = -osa.y;
        osa.z = -osa.z;            
      }
      uhel = DEG2RAD(uhel);
      angle_to_quat(track.p_at+r,&osa,uhel);
      r++;
      break;
    case 'S': // scale
      track.p_skeys[s].time = fr*SIM_KONSTI;
      p_spline = track.p_skeys+s;
      sscanf(p_pom+1,"%f %f %f",&track.p_scale[s].x,&track.p_scale[s].y,&track.p_scale[s].z);
      s++;
      break;
      
      
    case 'L':
      if(p_spline) {
        sscanf(p_pom+1,"%f %f %f",&p_spline->tension,&p_spline->continuity,&p_spline->bias);
      }
      break;
    case 'T':
      if(p_spline) {
        sscanf(p_pom+1,"%f",&p_spline->tension);
      }
      break;
    case 'O':
      if(p_spline) {
        sscanf(p_pom+1,"%f",&p_spline->continuity);                                    
      }
      break;
    case 'B':
      if(p_spline) {
        sscanf(p_pom+1,"%f",&p_spline->bias);
      }
      break;
    case 'C': // celkem framu
      sscanf(p_pom+1,"%d",&track.endtime);
      track.endtime = calc_endtime(track.endtime);
      break;
    case 'N': // loop animace
      loop = TRUE;
      break;
    case 'V': // pivot
      sscanf(p_pom+1,"%f %f %f",&pivot.x,&pivot.y,&pivot.z);
      break;
    case 'M': // 3ds pivot
      sscanf(p_pom+1,"%f %f %f",&track.pivot.x,&track.pivot.y,&track.pivot.z);
      break;
    case 'Q':
      linear = TRUE;
      break;
    case 'W':
      linear = FALSE;
      break;
    default:
      break;
    }  
  }

  for(l = 0; l < lmax; l++) {
    free(p_line[l]);
  }
  free(p_line);
  
  if(linear) {
    key_tri_to_sim_indir_lin(&track,p_sim,loop);
  } else {
    key_tri_to_sim_indir(&track,p_sim,loop,FALSE);
  }
  zrus_tri_vnitrek(&track);
  
  p_sim->pivot = pivot;// dokopiruj standartni pivot
  
  return(TRUE);
}

// rozsirene sim-animace load
int key_sim_nahraj_extended(EDIT_KONTEJNER *p_kont, int cislo_anim, char *p_file, char *p_dir)
{
  HIERARCHY_TRACK_INFO **p_track_list;
  HIERARCHY_TRACK_INFO  *p_track;
  HIERARCHY_ROOT         root;
  int                    akt_track;
  KEY_POINT_BRS         *p_spline = NULL;
  char                   pom[1000];
  char                   pom1[1000];
  FILE                  *f;
  int                    p,r,s,fr,loop = 0,framu;  
  BOD                    pivot(0.0f,0.0f,0.0f);
  BOD                    osa;
  float                  uhel;
  int                    i,objektu = 0,stop,ob,end,float_scale = FALSE;

  chdir((p_dir));
  if(!(f = fopen(p_file,"r"))) {
    ddw("File %s Line %d Chyba otevreni souboru '%s' v %s",__FILE__,__LINE__,p_file,(p_dir));
    return(FALSE);
  }

  kprintf(TRUE,"Nahravam animaci z %s...",p_file);

  while(fgets(pom,999,f)) {
    fgets_korekce(pom);
    if(pom[0] && pom[0] != '\n' && pom[0] != ';') {
      switch(toupper(pom[0])) {
      case 'C': // celkem framu
        sscanf(pom+1,"%d",&framu);
        break;
      case 'X':
        switch(toupper(pom[1])) {
        case 'O': // vlastnik
          objektu++;
          break;
        case 'P': // otec
          break;
        }
        break;
        default:
          break;
      }
    }
  }
  
  if(!objektu)
    objektu = 1;
  p_track_list = (HIERARCHY_TRACK_INFO **)alloca(sizeof(p_track_list[0])*objektu);
  akt_track = 0;  
  
  stop = 0;
  
  do {    

    p_track = p_track_list[akt_track++] = (HIERARCHY_TRACK_INFO *)alloca(sizeof(p_track[0]));
    memset(p_track,0,sizeof(p_track[0]));
    
    /* reset pivot-pointu
    */
    pivot.x = pivot.y = pivot.z = FLT_MAX;

    /* Nactu jmena objektu
    */
    fseek(f,stop,SEEK_SET);
    while(fgets(pom,999,f)) {
      fgets_korekce(pom);
      if(pom[0] && pom[0] != '\n' && pom[0] != ';') {
        if(toupper(pom[0]) == 'X' && toupper(pom[1]) == 'O') {
          strcpy(p_track->jmeno,cti_za_znak(pom+2,' '));
          continue;
        }
        if(toupper(pom[0]) == 'X' && toupper(pom[1]) == 'P') {
          strcpy(p_track->jmeno_otec,cti_za_znak(pom+2,' '));
          continue;        
        }
        if(toupper(pom[0]) == 'V') {
          sscanf(pom+1,"%f %f %f",&pivot.x,&pivot.y,&pivot.z);
          break;
        }
        if(toupper(pom[0]) == 'F') {
          sscanf(pom+1,"%d",&fr);
          break;
        }
      }
    }

    /* Nactu pozici za jmenama objektu
    */
    stop = ftell(f);

    /* Nactu pocty klicu
    */
    p = r = s = 0;
    while(fgets(pom,999,f)) {
      fgets_korekce(pom);
      if(pom[0] && pom[0] != '\n' && pom[0] != ';') {
        if(toupper(pom[0]) == 'X') {
          break;
        }
        if(toupper(pom[0]) == 'P') {
          p++;        
          continue;
        }
        if(toupper(pom[0]) == 'R') {
          r++;        
          continue;
        }
        if(toupper(pom[0]) == 'S' || toupper(pom[0]) == 'U') {
          s++;
          continue;
        }
      }
    }  
    
    /* Vyrobim animaci podle poctu klicu
    */
    strcpy(pom,p_track->jmeno);
    strcpy(pom1,p_track->jmeno_otec);
    key_tri_vyrob_indir(p_track,p,r,s);
    strcpy(p_track->jmeno,pom);
    strcpy(p_track->jmeno_otec,pom1);
    p = r = s = 0;
    // fame zbylo zposledniho klice
    
    /* Vratim se za prvni frame a ctu klice
    */
    fseek(f,stop,SEEK_SET);
    while((end = (int)fgets(pom,999,f))) {
      fgets_korekce(pom);
      if(pom[0] && pom[0] != '\n' && pom[0] != ';') {
        /* Test - je dalsi objekt?
        */
        if(toupper(pom[0]) != 'X')
          stop = ftell(f);
        else
          break;

        /* neni - cti klice animace
        */
        switch(toupper(pom[0])) {
        case 'F': // frame
          sscanf(pom+1,"%d",&fr);
          break;
        case 'P': // posun
          p_track->p_pkeys[p].time = fr*SIM_KONSTI;
          p_spline = p_track->p_pkeys+p;
          sscanf(pom+1,"%f %f %f",&p_track->p_pos[p].x,&p_track->p_pos[p].y,&p_track->p_pos[p].z);
          p++;
          break;
        case 'R': // rotace
          p_track->p_rkeys[r].time = fr*SIM_KONSTI;
          p_spline = p_track->p_rkeys+r;
          sscanf(pom+1,"%f %f %f %f",&osa.x,&osa.y,&osa.z,&uhel);
          /* Korekce zaporneho uhlu
          */
          if(uhel < 0.0f) {
            uhel = -uhel;
            osa.x = -osa.x;
            osa.y = -osa.y;
            osa.z = -osa.z;            
          }
          uhel = DEG2RAD(uhel);
          angle_to_quat(p_track->p_at+r,&osa,uhel);
          r++;
          break;
        case 'S': // scale
          p_track->p_skeys[s].time = fr*SIM_KONSTI;
          p_spline = p_track->p_skeys+s;
          sscanf(pom+1,"%f %f %f",&p_track->p_scale[s].x,&p_track->p_scale[s].y,&p_track->p_scale[s].z);
          s++;
          break;
        case 'U': // scale
          p_track->p_skeys[s].time = fr*SIM_KONSTI;
          p_spline = p_track->p_skeys+s;
          sscanf(pom+1,"%f",&p_track->p_scale[s].x);
          p_track->p_scale[s].y = p_track->p_scale[s].z = 0.0f;
          float_scale = TRUE;
          s++;
          break;
        case 'L':
          if(p_spline) {
            sscanf(pom+1,"%f %f %f",&p_spline->tension,
              &p_spline->continuity,
              &p_spline->bias);
          }
          break;
        case 'T':
          if(p_spline) {
            sscanf(pom+1,"%f",&p_spline->tension);
          }
          break;
        case 'O':
          if(p_spline) {
            sscanf(pom+1,"%f",&p_spline->continuity);                                    
          }
          break;
        case 'B':
          if(p_spline) {
            sscanf(pom+1,"%f",&p_spline->bias);
          }
          break;
        case 'N': // loop animace
          loop = TRUE;
          break;
        case 'V': // pivot
          sscanf(pom+1,"%f %f %f",&pivot.x,&pivot.y,&pivot.z);
          break;
        case 'M': // 3ds pivot
          sscanf(pom+1,"%f %f %f",&p_track->pivot.x,&p_track->pivot.y,&p_track->pivot.z);
          break;
        default:
          break;
        }
      }
    }
    p_track->pivot_sim = pivot;
  } while(end);
  fclose(f);

  assert(akt_track == objektu);

  /* Navazani animace na kontejner
  */
  memset(&root,0,sizeof(root));
  for(i = 0; i < objektu; i++ ) {    
    p_track = p_track_list[i];
    ob = p_track->objnum = lo_najdi_objekt_kont(p_kont,p_track->jmeno);
    if(ob == K_CHYBA) {
      kprintf(TRUE,"!!!! Nenalezen objekt '%s' v kontejneru '%s'",p_track->jmeno,p_kont->jmeno);
      continue;
    } else {
      kprintf(TRUE,"Objekt %s OK",p_track->jmeno);
    }
    p_track->p_obj = p_kont->p_obj[ob];
    
    if(p_track->pivot_sim.x != FLT_MAX &&
       p_track->pivot_sim.y != FLT_MAX &&
       p_track->pivot_sim.z != FLT_MAX) {
      p_kont->p_obj[ob]->pivot = p_track->pivot_sim;
    }

    p_track->parentnum = lo_najdi_objekt_kont(p_kont,p_track->jmeno_otec);
    if(p_track->parentnum == K_CHYBA)
      p_track->parentnum = ROOT_NODE;
    key_konec_prochazeni = FALSE;
    key_root_node_pridej(&root,p_track);
    p_track->endtime = calc_endtime(framu);
  }
  root.framenum = framu;
  kprintf(TRUE,"Framu animace %d",framu);
  
  if(loop) {
    kprintf(TRUE,"Loop animace");
    root.flag |= GK_LOOP;  
  }
  

  /* Prevod do sim-animaci
  */  
  key_root_to_sim_indir(p_kont, p_kont->sim+cislo_anim, &root, float_scale);
  
  p_kont->kflag |= (float_scale) ? KONT_NORMAL_ANIM|KONT_KEYFRAME : KONT_KEYFRAME;
  p_kont->kflag &= ~KONT_STATIC;

  return(TRUE);
}

// aplikuje siple track do matrice
GLMATRIX * key_sim_to_matrix(SIMPLE_TRACK_INFO *p_sim, int time, GLMATRIX *p_m)
{
  float frame,t,t1;  
  BOD   *p1,*p2;
  QUAT  q;
  int   kn,lasttime;

  if(p_sim->p_all_matrix) {
    *p_m = *p_sim->p_all_matrix;
  
  } else {
    init_matrix(p_m);
    
    lasttime = (p_sim->keynum-1)*SIM_KONSTI;
    if(time < lasttime) {
      t = (float)time/SIM_KONST;
      frame = (float)floor((float)t);
      t -= frame;
      kn = ftoi(frame);
      t1 = 1.0f-t;
      if(p_sim->p_pos) {
        p1 = p_sim->p_pos+kn;
        p2 = p_sim->p_pos+kn+1;
        p_m->_41 = p1->x*t1 + p2->x*t;
        p_m->_42 = p1->y*t1 + p2->y*t;
        p_m->_43 = p1->z*t1 + p2->z*t;
      }
      if(p_sim->p_at) {
        quat_to_matrix(p_m,slerp(p_sim->p_at+kn,p_sim->p_at+kn+1,t,&q));
      }
      if(p_sim->p_scale) {
        p1 = p_sim->p_scale+kn;
        p2 = p_sim->p_scale+kn+1;
        scale_matrix(p_m,p1->x*t1 + p2->x*t,p1->y*t1 + p2->y*t,p1->z*t1 + p2->z*t);        
      }
      if(p_sim->p_norm) {      
        *p_sim->p_object_norm = p_sim->p_norm[kn]*t1+p_sim->p_norm[kn+1]*t;
      }
    } else {
      kn = p_sim->keynum-1;
      if(p_sim->p_pos) {
        p1 = (BOD *)&p_m->_41;
        *p1 = p_sim->p_pos[kn];
      }
      if(p_sim->p_at) {
        quat_to_matrix(p_m,p_sim->p_at+kn);
      }
      if(p_sim->p_scale) {
        p1 = p_sim->p_scale+kn;
        scale_matrix(p_m,p1->x,p1->y,p1->z);
      }
      if(p_sim->p_norm) {
        *p_sim->p_object_norm = p_sim->p_norm[kn];
      }
    }
    pivotuj_matrix_tam(&p_sim->pivot3ds,p_m);
    pivotuj_matrix(p_m,&p_sim->pivot);
  }
  return(p_m);
}

void key_sim_root_to_matrix(EDIT_KONTEJNER *p_kont, HIERARCHY_SIM *p_sim, int time, GLMATRIX *p_m)
{
 int i;
 
 for(i = 0; i < p_sim->childnum; i++) {
   key_sim_animuj_strom(p_sim->p_child+i, time, p_m);
 }
 if(p_kont->kflag&KONT_NORMAL_ANIM) {
   key_kontejner_recalc_normal_anim(p_kont);
 }
}

void key_kontejner_recalc_normal_anim(EDIT_KONTEJNER *p_kont)
{
  EDIT_OBJEKT *p_obj;
  OBJ_VERTEX *p_vertex;
  BOD n;
  float last,akt,num,i;

  // pokud se last=aktualni - nic nedelam
  int o;
  for(o = 0; o < p_kont->objektu; o++) {
    if((p_obj = p_kont->p_obj[o])) {
      if((last = p_obj->float_scale_last) == (akt = p_obj->float_scale))
        continue;
      p_vertex = p_obj->p_vertex;
      num = p_obj->vertexnum;
      for(i = 0; i < num; i++, p_vertex++) {
        vektor_add((BOD *)&p_vertex->x,vektor_mult_skalar((BOD *)&p_vertex->nx,-akt,&n),(BOD *)&p_vertex->px);
      }
      p_obj->float_scale_last = akt;
    }
  }
}

/*
typedef struct _SIMPLE_TRACK_INFO {
  
  int      Objekt_ID;      // id objektu ke kteremu animace patri
  dword    flag;
  
  int      keynum;         // pocet klicu
  BOD     *p_pos;          // Mesh position keys
  BOD     *p_scale;        // Mesh scaling keys
  QUAT    *p_at;           // Absolutni traky
  BOD      pivot;          // pivot point animace -> kopiruje se do objektu
  BOD      pivot3ds;       // pivot 3ds animace

  GLMATRIX *p_m;            // kam to hazet
  
  int       childnum;      // pocet svistich traku
  struct   _SIMPLE_TRACK_INFO *p_child; // svisti traky
  struct   _SIMPLE_TRACK_INFO *p_otec; // otcovy traky
  
} SIMPLE_TRACK_INFO;

typedef struct _HIERARCHY_SIM {
  
  float     delka;         // delka casu animace (fps)
  char      stav;          // 0 = stop, 1 = run
  
  int       flag;          // flagy animace (GL_LOOP,GL_REMOVE)

  dword     time;          // aktualni cas animace
  dword     time_start;    // aktualni start
  dword     time_stop;     // aktualni stop
  dword     time_delka;    // aktualni delka animace
  
  dword     start;
  dword     stop;  
  
  int       keynum;         // pocet klicu
  char      jmeno[MAX_JMENO]; // jmeno animace
  int       childnum;         // pocet detskych traku
  SIMPLE_TRACK_INFO *p_child;

} HIERARCHY_SIM;
*/

void key_sim_root_to_sim_indir(HIERARCHY_SIM *p_root, SIMPLE_TRACK_INFO *p_sim)
{  
  memset(p_sim,0,sizeof(SIMPLE_TRACK_INFO));

  p_sim->flag = p_root->flag;
  p_sim->keynum = p_root->keynum;
  p_sim->childnum = p_root->childnum;
  p_sim->p_child = p_root->p_child;
  p_sim->Objekt_ID = K_CHYBA;
  memset(p_root,0,sizeof(p_root[0]));  
}

// aplikuje sim do matice a bodu
GLMATRIX * key_sim_to_matrix_param(SIMPLE_TRACK_INFO *p_sim, int time, GLMATRIX *p_m, BOD *p_pos, QUAT *p_quat, BOD *p_scale)
{
  float frame,t,t1;
  BOD   *p1,*p2; 
  int   kn;
  int   lasttime;

   
  init_matrix(p_m);
  
  lasttime = (p_sim->keynum-1)*SIM_KONSTI;
  if(time < lasttime) {
    t = (float)time/SIM_KONST;
    frame = (float)floor((float)t);
    t -= frame;
    kn = ftoi(frame);
    t1 = 1.0f-t;
    if(p_sim->p_pos) {
      p1 = p_sim->p_pos+kn;
      p2 = p_sim->p_pos+kn+1;
      p_pos->x = p_m->_41 = p1->x*t1 + p2->x*t;
      p_pos->y = p_m->_42 = p1->y*t1 + p2->y*t;
      p_pos->z = p_m->_43 = p1->z*t1 + p2->z*t;
    } else {
      vektor_set(p_pos,0);
    }
    if(p_sim->p_at) {
      quat_to_matrix(p_m,slerp(p_sim->p_at+kn,p_sim->p_at+kn+1,t,p_quat));
    } else {
      quat_set_zero(p_quat);
    }
    if(p_sim->p_scale) {
      p1 = p_sim->p_scale+kn;
      p2 = p_sim->p_scale+kn+1;
      p_scale->x = p1->x*t1 + p2->x*t;
      p_scale->y = p1->y*t1 + p2->y*t;
      p_scale->z = p1->z*t1 + p2->z*t;
      scale_matrix(p_m,p_scale->x,p_scale->y,p_scale->z);
    } else {
      vektor_set(p_scale,1);
    }
  } else {
    kn = p_sim->keynum-1;
    if(p_sim->p_pos) {
      p1 = (BOD *)&p_m->_41; 
      *p_pos = *p1 = p_sim->p_pos[kn];
    } else {
      vektor_set(p_pos,0);
    }
    if(p_sim->p_at) {
      *p_quat = p_sim->p_at[kn];
      quat_to_matrix(p_m,p_quat);
    } else {
      quat_set_zero(p_quat);
    }
    if(p_sim->p_scale) {
      p1 = p_sim->p_scale+kn;
      *p_scale = *p1;
      scale_matrix(p_m,p1->x,p1->y,p1->z);
    } else {
      vektor_set(p_scale,1);
    }
  }

  return(p_m);
}

/* Prevede animaci kamery do kamery
*/
void key_kan_to_kamera(KAMERA *p_kam, KAMERA_TRACK_INFO *p_track, int time)
{
  GLMATRIX m;
  int loop = p_track->flag&GK_LOOP;
  key_track_interpolace_bod(&p_kam->p, p_track->p_pos, p_track->p_pkeys, time, p_track->endtime, p_track->pos_keys, loop);
  key_track_interpolace_bod(&p_kam->t, p_track->p_trg, p_track->p_tkeys, time, p_track->endtime, p_track->trg_keys, loop);
  key_track_interpolace_float(&p_kam->roll, p_track->p_roll, p_track->p_rlkeys, time, p_track->endtime, p_track->roll_keys, loop);
  key_track_interpolace_float(&p_kam->fov,  p_track->p_fov,  p_track->p_fkeys,  time, p_track->endtime, p_track->fov_keys, loop);
  calc_camera_3ds(&m,&p_kam->p_objekt->m,&p_kam->p,&p_kam->t,p_kam->roll);
}

/* Vyrobi animaci kamery
*/
int key_kamera_vyrob_indir(KAMERA_TRACK_INFO *p_track, int pos_keys, int trg_keys, int roll_keys, int fov_keys, int quat_keys)
{
  memset(p_track,0,sizeof(*p_track));

  if((p_track->pos_keys = pos_keys)) {
    p_track->p_pkeys = (KEY_POINT_BRS *)mmalloc(sizeof(p_track->p_pkeys[0])*pos_keys);    
    p_track->p_pos = (BOD *)mmalloc(sizeof(p_track->p_pos[0])*pos_keys);
  }
  
  if((p_track->trg_keys = trg_keys)) {
    p_track->p_tkeys = (KEY_POINT_BRS *)mmalloc(sizeof(p_track->p_tkeys[0])*trg_keys);
    p_track->p_trg = (BOD *)mmalloc(sizeof(p_track->p_trg[0])*trg_keys);
  }

  if((p_track->roll_keys = roll_keys)) {
    p_track->p_rlkeys = (KEY_POINT_BRS *)mmalloc(sizeof(p_track->p_rlkeys[0])*roll_keys);
    p_track->p_roll = (float *)mmalloc(sizeof(p_track->p_roll[0])*roll_keys);
  }
  
  if((p_track->fov_keys = fov_keys)) {
    p_track->p_fkeys = (KEY_POINT_BRS *)mmalloc(sizeof(p_track->p_fkeys[0])*fov_keys);
    p_track->p_fov = (float *)mmalloc(sizeof(p_track->p_fov[0])*fov_keys);
  }
  
  if((p_track->quat_keys = quat_keys)) {
    p_track->p_qkeys = (KEY_POINT_BRS *)mmalloc(sizeof(p_track->p_qkeys[0])*quat_keys);
    p_track->p_quat = (QUAT *)mmalloc(sizeof(p_track->p_quat[0])*quat_keys);
  }

  return(TRUE);
}

/* Zrusi animaci kamery
*/
void key_kamera_zrus_indir(KAMERA_TRACK_INFO *p_track)
{
  if(p_track->fov_keys) {
    null_free((void **)&p_track->p_fkeys);
    null_free((void **)&p_track->p_fov);
  }
  if(p_track->pos_keys) {
    null_free((void **)&p_track->p_pkeys);
    null_free((void **)&p_track->p_pos);
  }
  if(p_track->roll_keys) {
    null_free((void **)&p_track->p_rlkeys);
    null_free((void **)&p_track->p_roll);
  }
  if(p_track->trg_keys) {
    null_free((void **)&p_track->p_tkeys);
    null_free((void **)&p_track->p_trg);
  }
  if(p_track->quat_keys) {
    null_free((void **)&p_track->p_qkeys);
    null_free((void **)&p_track->p_quat);
  }
  memset(p_track,0,sizeof(*p_track));
}


/* Ulozi kameru
*/
int key_kamera_uloz(KAMERA_TRACK_INFO *p_track, char *p_jmeno_kamery, char *p_jmeno_anim, char *p_file, char *p_dir)
{
  FILE *f;
  int   i,keynum;
  int   p,t,r,fv;

  chdir((p_dir));

  if(!(f = fopen(p_file,"w"))) {
    ddw("File %s Line %d Chyba otevreni souboru '%s' v %s",__FILE__,__LINE__,p_file,(p_dir));
    return(FALSE);
  }
  
  keynum = calc_keynum(p_track->endtime);

  fprintf(f,"C %d\n",keynum);
  fprintf(f,"J %s\n",p_jmeno_kamery); // jmeno kamery
  fprintf(f,"A %s\n",p_jmeno_anim);  // jmeno animace

  for(i = 0; i < keynum; i++) {

    p = key_je_frame_klic(p_track->p_pkeys,p_track->pos_keys,i);
    t = key_je_frame_klic(p_track->p_tkeys,p_track->trg_keys,i);
    r = key_je_frame_klic(p_track->p_rlkeys,p_track->roll_keys,i);
    fv= key_je_frame_klic(p_track->p_fkeys,p_track->fov_keys,i);

    if(p != K_CHYBA || t != K_CHYBA || r != K_CHYBA || fv != K_CHYBA) {
      fprintf(f,"F %d\n",i);

      /* uloz pozici
      */
      if(p != K_CHYBA) {
        fprintf(f,"P %f %f %f\n",p_track->p_pos[p].x,p_track->p_pos[p].y,
                                 p_track->p_pos[p].z);
        fprintf(f,"L %f %f %f\n",p_track->p_pkeys[p].tension,
                                 p_track->p_pkeys[p].continuity,
                                 p_track->p_pkeys[p].bias);
      }
      /* uloz cil
      */
      if(t != K_CHYBA) {
        fprintf(f,"D %f %f %f\n",p_track->p_trg[t].x,p_track->p_trg[t].y,
                                 p_track->p_trg[t].z);
        fprintf(f,"L %f %f %f\n",p_track->p_tkeys[t].tension,
                                 p_track->p_tkeys[t].continuity,
                                 p_track->p_tkeys[t].bias);
      }
      /* uloz roll
      */
      if(r != K_CHYBA) {        
        fprintf(f,"D %f\n",RAD2DEG(p_track->p_roll[r]));
        fprintf(f,"L %f %f %f\n",p_track->p_rlkeys[r].tension,
                                 p_track->p_rlkeys[r].continuity,
                                 p_track->p_rlkeys[r].bias);
      }
      /* uloz fov
      */
      if(fv != K_CHYBA) {
        fprintf(f,"V %f\n",RAD2DEG(p_track->p_fov[fv]));
        fprintf(f,"L %f %f %f\n",p_track->p_fkeys[fv].tension,
                                 p_track->p_fkeys[fv].continuity,
                                 p_track->p_fkeys[fv].bias);
      }
    }
  }  
  fclose(f);
  return(TRUE);
}

/* Nahraje kameru
*/
int key_kamera_nahraj(KAMERA_TRACK_INFO *p_track, char *p_jmeno_kamery, char *p_jmeno_anim, APAK_HANDLE *pHandle, char *p_file, char *p_dir)
{
  KEY_POINT_BRS *p_spline = NULL;
  char           line[1000];
  char          *p_pom;
  int            p,t,r,fv,fr;
  float          uhel;
  KFILE         *f;
  char         **p_line;
  int            l,lmax;


  chdir((p_dir));
  if(!(f = kopen(pHandle,p_file,"r"))) {
    kprintf(TRUE,"File %s Line %d Chyba otevreni souboru '%s' v %s",
            __FILE__,__LINE__,p_file,(p_dir));
    return(FALSE);
  }

  p_line = (char **)mmalloc(sizeof(p_line[0])*MAX_SIM_RADKU);
  l = 0;
  while(kgets(line,999,f)) {
    assert(l < MAX_SIM_RADKU);
    if(line[0] && line[0] != '\n' && line[0] != ';') {
      p_line[l] = (char *)kopiruj_pole(line,sizeof(char)*(strlen(line)+1));
      l++;
    }
  }
  kclose(f);
  lmax = l;
  
  p = t = r = fv = 0;
  for(l = 0; l < lmax; l++) {
    p_pom = p_line[l];    
    switch(toupper(p_pom[0])) {
    case 'P': // posun
      p++;
      break;
    case 'D': // rotace
      t++;
      break;
    case 'R': // roll
      r++;
      break;
    case 'V': // fov
      fv++;
      break;
    default:
      break;
    }
  }

  key_kamera_vyrob_indir(p_track, p, t, r, fv, 0);
  
  p = r = t = fr = fv = 0;
  for(l = 0; l < lmax; l++) {
    p_pom = p_line[l];    
    switch(toupper(p_pom[0])) {
    case 'F': // frame
      sscanf(p_pom+1,"%d",&fr);
      break;      
    case 'P': // posun
      p_spline = p_track->p_pkeys+p;    // nastaveni aktivniho splajnu
      p_track->p_pkeys[p].time = fr*SIM_KONSTI;
      sscanf(p_pom+1,"%f %f %f",&p_track->p_pos[p].x,&p_track->p_pos[p].y,&p_track->p_pos[p].z);
      p++;
      break;
    case 'D': // target
      p_track->p_tkeys[t].time = fr*SIM_KONSTI;
      p_spline = p_track->p_tkeys+t;    // nastaveni aktivniho splajnu
      sscanf(p_pom+1,"%f %f %f",&p_track->p_trg[t].x,&p_track->p_trg[t].y,&p_track->p_trg[t].z);
      t++;
      break;
    case 'R': // roll
      p_track->p_rlkeys[r].time = fr*SIM_KONSTI;
      p_spline = p_track->p_rlkeys+r;
      sscanf(p_pom+1,"%f",&uhel);
      p_track->p_roll[r] = DEG2RAD(uhel);
      r++;
      break;
      
    case 'V': // roll
      p_track->p_fkeys[fv].time = fr*SIM_KONSTI;
      p_spline = p_track->p_fkeys+fv;
      sscanf(p_pom+1,"%f",&uhel);
      p_track->p_fov[fv] = DEG2RAD(uhel);
      fv++;
      break;
      
    case 'L':
      if(p_spline) {
        sscanf(p_pom+1,"%f %f %f",&p_spline->tension,
          &p_spline->continuity,
          &p_spline->bias);
      }
      break;
      
    case 'T':
      if(p_spline) {
        sscanf(p_pom+1,"%f",&p_spline->tension);
      }
      break;
    case 'O':
      if(p_spline) {
        sscanf(p_pom+1,"%f",&p_spline->continuity);                                    
      }
      break;
    case 'B':
      if(p_spline) {
        sscanf(p_pom+1,"%f",&p_spline->bias);
      }
      break;
      
    case 'C': // celkem framu
      sscanf(p_pom+1,"%d",&p_track->endtime);
      p_track->endtime = calc_endtime(p_track->endtime);
      break;
      
    case 'J': // jmeno kamery
      strcpy(p_jmeno_kamery,p_pom+2);
      break;
    case 'A': // jmeno animace          
      strcpy(p_track->jmeno,p_pom+2);
      break;
    }
  }  
  
  zamen_koncovku_znak(p_jmeno_kamery,'\n',NULL);
  zamen_koncovku_znak(p_track->jmeno,'\n',NULL);
  
  return(TRUE);
}

int key_kamera_track_cti(KAMERA_TRACK_INFO *p_track, BOD *p_p, BOD *p_t, float *p_roll, float *p_fov)
{

  if(p_track->pos_keys) {
    *p_p = p_track->p_pos[0];
  } else {
    p_p->x = 0.0f; p_p->y = 0.0f; p_p->z = 0.0f;    
  }

  if(p_track->trg_keys) {
    *p_t = p_track->p_trg[0];
  } else {
    p_t->x = 0.0f; p_t->y = 0.0f; p_t->z = 0.0f;
  }

  if(p_track->roll_keys) {
    *p_roll = p_track->p_roll[0];
  } else {
    *p_roll = 0.0f;
  }

  if(p_track->fov_keys) {
    *p_fov = p_track->p_fov[0];
  } else {
    *p_fov = DEG2RAD(45.0f);
  }

  return(TRUE);
}

/*
  Animace t1-t2 materialu
*/
ANIM_TEXT * key_vyrob_material_animace(int poskey, int rotkey, int scalekey, int pivotkey)
{
  ANIM_TEXT *p_mat = (ANIM_TEXT *)mmalloc(sizeof(p_mat[0]));

  if(p_mat->pos_keys = poskey) {
    p_mat->p_pos = (BOD *)mmalloc(sizeof(BOD)*poskey);    
    p_mat->p_pkeys = (KEY_POINT_BRS *)mmalloc(sizeof(p_mat->p_pkeys[0])*poskey);
  }
  if(p_mat->piv_keys = pivotkey) {
    p_mat->p_piv = (BOD *)mmalloc(sizeof(BOD)*pivotkey);    
    p_mat->p_vkeys = (KEY_POINT_BRS *)mmalloc(sizeof(p_mat->p_vkeys[0])*pivotkey);
  }
  if(p_mat->rot_keys = rotkey) {
    p_mat->p_rot = (float *)mmalloc(sizeof(p_mat->p_rot[0])*rotkey);
    p_mat->p_rkeys = (KEY_POINT_BRS *)mmalloc(sizeof(p_mat->p_rkeys[0])*rotkey);
  }
  if(p_mat->scs_keys = scalekey) {  
    p_mat->p_scale = (BOD *)mmalloc(sizeof(BOD)*scalekey);    
    p_mat->p_skeys = (KEY_POINT_BRS *)mmalloc(sizeof(p_mat->p_skeys[0])*scalekey);
  }
  return(p_mat);
}

ANIM_TEXT * key_kopiruj_material_animace(ANIM_TEXT *p_src)
{
  ANIM_TEXT *p_mat = (ANIM_TEXT *)mmalloc(sizeof(p_mat[0]));
  int poskey = p_src->pos_keys;
  int pivkey = p_src->piv_keys;
  int rotkey = p_src->rot_keys;
  int scalekey = p_src->scs_keys;

  if(p_mat->pos_keys = poskey) {
    p_mat->p_pos = (BOD *)mmalloc(sizeof(p_mat->p_pos[0])*poskey);
    p_mat->p_pkeys = (KEY_POINT_BRS *)mmalloc(sizeof(p_mat->p_pkeys[0])*poskey);
    memcpy(p_mat->p_pos,p_src->p_pos,sizeof(BOD)*poskey);
    memcpy(p_mat->p_pkeys,p_src->p_pkeys,sizeof(p_mat->p_pkeys[0])*poskey);
  }

  if(p_mat->piv_keys = pivkey) {
    p_mat->p_piv = (BOD *)mmalloc(sizeof(p_mat->p_piv[0])*pivkey);
    p_mat->p_vkeys = (KEY_POINT_BRS *)mmalloc(sizeof(p_mat->p_vkeys[0])*pivkey);
    memcpy(p_mat->p_piv,p_src->p_piv,sizeof(BOD)*pivkey);
    memcpy(p_mat->p_vkeys,p_src->p_vkeys,sizeof(p_mat->p_vkeys[0])*pivkey);
  }

  if(p_mat->rot_keys = rotkey) {
    p_mat->p_rot = (float *)mmalloc(sizeof(p_mat->p_rot[0])*rotkey);
    p_mat->p_rkeys = (KEY_POINT_BRS *)mmalloc(sizeof(p_mat->p_rkeys[0])*rotkey);
    memcpy(p_mat->p_rot,p_src->p_rot,sizeof(p_mat->p_rot[0])*rotkey);
    memcpy(p_mat->p_rkeys,p_src->p_rkeys,sizeof(p_mat->p_rkeys[0])*rotkey);
  }
  if(p_mat->scs_keys = scalekey) {  
    p_mat->p_scale = (BOD *)mmalloc(sizeof(p_mat->p_scale[0])*scalekey);    
    p_mat->p_skeys = (KEY_POINT_BRS *)mmalloc(sizeof(p_mat->p_skeys[0])*scalekey);
    memcpy(p_mat->p_scale,p_src->p_scale,sizeof(p_src->p_scale[0])*scalekey);
    memcpy(p_mat->p_skeys,p_src->p_skeys,sizeof(p_mat->p_skeys[0])*scalekey);
  }
  return(p_mat);
}

void key_zrus_material_animace(ANIM_TEXT **p_mat)
{
  null_free((void **)&(*p_mat)->p_pos);
  null_free((void **)&(*p_mat)->p_piv);
  null_free((void **)&(*p_mat)->p_rot);
  null_free((void **)&(*p_mat)->p_scale);
  null_free((void **)&(*p_mat));
}

/*
  Animace text-koordinatu materialu
*/
void key_sim_to_text_matrix(ANIM_TEXT *p_text, int time, GLMATRIX *p_ttext)
{
  BOD *p;

  init_matrix(p_ttext);

  if(p_text->p_piv) {
    key_track_interpolace_bod_linear(&p_text->pivot,p_text->p_piv,p_text->p_vkeys,time,p_text->endtime,p_text->piv_keys,FALSE);
  }
  
  if(p_text->p_pos) {
    key_track_interpolace_bod_linear(&p_text->pos,p_text->p_pos,p_text->p_pkeys,time,p_text->endtime,p_text->pos_keys,FALSE);
    p_ttext->_41 = p_text->pos.x;
    p_ttext->_42 = p_text->pos.y;
  }

  if(p_text->p_rot) {
    key_track_interpolace_float_linear(&p_text->rot, p_text->p_rot, p_text->p_rkeys, time, p_text->endtime, p_text->rot_keys, FALSE);
    float_to_matrix(p_ttext,p_text->rot,p_text->p_piv ? &p_text->pivot : NULL);
  }

  if(p_text->p_scale) {
    p = &p_text->scs;
    key_track_interpolace_bod_linear(p,p_text->p_scale,p_text->p_skeys,time,p_text->endtime,p_text->scs_keys,FALSE);
    p_ttext->_11 *= p->x;
    p_ttext->_21 *= p->x;    
    p_ttext->_12 *= p->y;
    p_ttext->_22 *= p->y;
  }
}

void key_mat_to_text_matrix(GLMATRIX *p_ttext, BOD *p_pos, float uhel, BOD *p_scale, BOD *p_pivot)
{ 
  init_matrix(p_ttext);
  if(p_pos) {
    p_ttext->_41 = p_pos->x;
    p_ttext->_42 = p_pos->y;
  }
  if(uhel != 0.0f)
    float_to_matrix(p_ttext,uhel,p_pivot);
  if(p_scale) {
    p_ttext->_11 *= p_scale->x;
    p_ttext->_21 *= p_scale->x;    
    
    p_ttext->_12 *= p_scale->y;
    p_ttext->_22 *= p_scale->y;    
  }
}

// load text animace z text-file
ANIM_TEXT * key_text_nahraj(char *p_file, char *p_dir)
{
  ANIM_TEXT           *p_track;
  KEY_POINT_BRS       *p_spline = NULL;
  char                 pom[1000];
  FILE                *f;
  int                  p,r,s,v,fr;
  float                uhel;

  chdir((p_dir));
  if(!(f = fopen(p_file,"r"))) {
    ddw("File %s Line %d Chyba otevreni souboru '%s' v %s",__FILE__,__LINE__,p_file,(p_dir));
    return(FALSE);
  }

  p = r = s = v = 0;
  while(fgets(pom,999,f)) {
    if(pom[0] && pom[0] != '\n' && pom[0] != ';') {
      switch(toupper(pom[0])) {
        case 'P': // posun
          p++;
          break;
        case 'R': // rotace
          r++;
          break;
        case 'S': // scale
          s++;
          break;
        case 'V': // pivot
          v++;
          break;
        case 'F': // frame
        case 'C': // celkem framu
        case 'L': // spline-hodnoty
          break;
      }
    }
  }  
  
  fseek(f,0,SEEK_SET);
  p_track = key_vyrob_material_animace(p,r,s,v);

  p = r = s = v = fr = 0;
  while(fgets(pom,999,f)) {
    if(pom[0] && pom[0] != '\n' && pom[0] != ';') {
      switch(toupper(pom[0])) {
        case 'F': // frame
          sscanf(pom+1,"%d",&fr);
          break;
        case 'P': // posun
          p_track->p_pkeys[p].time = fr*SIM_KONSTI;
          p_spline = p_track->p_pkeys+p;
          sscanf(pom+1,"%f %f %f",&p_track->p_pos[p].x,
                                  &p_track->p_pos[p].y);
          p++;
          break;
        case 'V': // pivot
          p_track->p_vkeys[v].time = fr*SIM_KONSTI;
          p_spline = p_track->p_vkeys+v;
          sscanf(pom+1,"%f %f %f",&p_track->p_piv[v].x,
                                  &p_track->p_piv[v].y);
          v++;
          break;
        case 'R': // rotace
          p_track->p_rkeys[r].time = fr*SIM_KONSTI;
          p_spline = p_track->p_rkeys+r;
          sscanf(pom+1,"%f",&uhel);
          p_track->p_rot[r] = DEG2RAD(uhel);
          r++;
          break;
        case 'S': // scale
          p_track->p_skeys[s].time = fr*SIM_KONSTI;
          p_spline = p_track->p_skeys+s;
          sscanf(pom+1,"%f %f %f",&p_track->p_scale[s].x,
                                  &p_track->p_scale[s].y);
          s++;
          break;
        case 'L':
          if(p_spline) {
            sscanf(pom+1,"%f %f %f",&p_spline->tension,
                                    &p_spline->continuity,
                                    &p_spline->bias);
          }
          break;
        case 'T':
          if(p_spline) {
            sscanf(pom+1,"%f",&p_spline->tension);
          }
          break;
        case 'O':
          if(p_spline) {
            sscanf(pom+1,"%f",&p_spline->continuity);                                    
          }
          break;
        case 'B':
          if(p_spline) {
            sscanf(pom+1,"%f",&p_spline->bias);
          }
          break;
        case 'C': // celkem framu
          sscanf(pom+1,"%d",&p_track->endtime);
          p_track->endtime = calc_endtime(p_track->endtime);
          break;
      }
    }
  }  
  fclose(f);    
  return(p_track);
}

int key_text_uloz(ANIM_TEXT *p_track, char *p_file, char *p_dir)
{
  FILE *f;
  int   i,p,r,s,v,frame;
  int   keynum;
  dword time;

  chdir((p_dir));

  if(!(f = fopen(p_file,"w"))) {
    ddw("File %s Line %d Chyba otevreni souboru '%s' v %s",__FILE__,__LINE__,p_file,(p_dir));
    return(FALSE);
  }

  frame = p = r = s = v = 0;
  keynum = calc_keynum(p_track->endtime);
  fprintf(f,"C %d\n",keynum);
  for(i = 0; i < keynum; i++) {
    frame = 0;
    time = i*SIM_KONSTI;
    
    if(p < p_track->pos_keys && p_track->p_pkeys && 
      p_track->p_pkeys[p].time == time) {
      fprintf(f,"F %d\n",i);
      frame++;
      fprintf(f,"P %f %f\n",p_track->p_pos[p].x,p_track->p_pos[p].y);                               
      p++;
    }
    
    if(v < p_track->piv_keys && p_track->p_vkeys && 
       p_track->p_vkeys[v].time == time) {
      if(!frame) {
        fprintf(f,"F %d\n",i);
        frame++;
      }
      fprintf(f,"V %f %f\n",p_track->p_piv[v].x,p_track->p_piv[v].y);
      v++;
    }

    if(r < p_track->rot_keys && p_track->p_rkeys && 
       p_track->p_rkeys[r].time == time) {
      if(!frame) {
        fprintf(f,"F %d\n",i);
        frame++;
      }
      fprintf(f,"R %f\n",RAD2DEG(p_track->p_rot[r]));
      r++;
    }
    
    if(s < p_track->scs_keys && p_track->p_skeys && 
       p_track->p_skeys[s].time == time) {
      if(!frame) {
        fprintf(f,"F %d\n",i);
        frame++;
      }
      fprintf(f,"S %f %f\n",p_track->p_scale[s].x,p_track->p_scale[s].y);
      s++;
    }

  }
  fclose(f);
  return(TRUE);
}

void key_sim_root_vloz_pivoty_do_animace_rec(EDIT_KONTEJNER *p_kont, SIMPLE_TRACK_INFO *p_sim)
{
  int i,o = lo_najdi_objekt_kont_ID(p_kont,p_sim->Objekt_ID);
  
  if(o != K_CHYBA) {
    p_sim->pivot = p_kont->p_obj[o]->pivot;
  }
  for(i = 0; i < p_sim->childnum; i++) {
    key_sim_root_vloz_pivoty_do_animace_rec(p_kont,p_sim->p_child+i);
  }
}

void key_sim_root_vloz_pivoty_do_animace(EDIT_KONTEJNER *p_kont, HIERARCHY_SIM *p_root)
{
  int i;
  for(i = 0; i < p_root->childnum; i++) {
    key_sim_root_vloz_pivoty_do_animace_rec(p_kont,p_root->p_child+i);
  }
}

void key_sim_root_vloz_pivoty_do_animace_kont(EDIT_KONTEJNER *p_kont)
{
  int i;
  for(i = 0; i < KONT_MAX_ANIM; i++) {
    if(p_kont->sim[i].keynum)
      key_sim_root_vloz_pivoty_do_animace(p_kont,p_kont->sim+i);
  }
}

/* Transformuje lokalni OBB meshe do globalni OBB
   Pri posunu celeho objektu
*/
void key_mesh_transformuj_obalky(GAME_MESH_OLD *p_mesh, GLMATRIX *p_mat)
{
  OBB_OLD *p_src = p_mesh->p_obb_local;
  OBB_OLD *p_dst = p_mesh->p_obb_world;
  int  i;  
  for(i = 0; i < p_mesh->objektu; i++) {
    obb_transformuj(p_src+i,p_mat,p_dst+i);
  }
  obb_transformuj(&p_mesh->obb_local,p_mat,&p_mesh->obb_world);
}

/* Spocita pozuze obalku kontejneru - pri key-animaci kdy se obalky
   updatuji automaticky
*/
/*
void key_mesh_calc_global_obalku(GAME_MESH_OLD *p_mesh)
{  
  obb_slep_aabb(&p_mesh->obb_world, p_mesh->p_obb_world, p_mesh->objektu);
}
*/

void key_kont_updatuj_flagy(EDIT_KONTEJNER *p_kont)
{
  int i,anum = 0;

  for(i = 0; i < KONT_MAX_ANIM; i++) {
    if(p_kont->sim[i].keynum)
      anum++;
  }
  if(!anum) {
    p_kont->kflag &= ~(KONT_KEYFRAME|KONT_NORMAL_ANIM);
  }
}

JOINT_KEYS * key_kost_spline_vyrob(JOINT *p_joint)
{
  if(p_joint) {
    if(!p_joint->p_keys) {
      p_joint->p_keys = (JOINT_KEYS *)mmalloc(sizeof(p_joint->p_keys[0]));
    }
    return(p_joint->p_keys);
  } else {
    return(NULL);
  }
}

void key_kosti_updatuj_vertexy_rec(EDIT_OBJEKT *p_obj, JOINT *p_joint)
{  
  int      i,v;
  
  for(i = 0; i < p_joint->vertexnum; i++) {
    v = p_joint->p_vertexlist[i];
    if(v >= p_obj->vertexnum) {
      assert(v < p_obj->vertexnum);
      p_joint->p_vertexlist[i] = 0;
    }
    transformuj_bod_matici_bod((BOD *)(p_obj->p_vertex+v),&p_joint->m,p_obj->p_vertex_kosti+v);
  }

  // proved rekurzivni animace
  if(p_joint->p_next)
    key_kosti_updatuj_vertexy_rec(p_obj, p_joint->p_next);
  if(p_joint->p_child)
    key_kosti_updatuj_vertexy_rec(p_obj, p_joint->p_child);
}

void key_kosti_updatuj_vertexy(EDIT_OBJEKT *p_obj, JOINT_ANIMACE *p_animace)
{  
  if(p_animace && p_animace->p_child) {
    if(!p_obj->p_vertex_kosti) {
      int i;
      p_obj->p_vertex_kosti = (BOD *)mmalloc(sizeof(p_obj->p_vertex_kosti[0])*p_obj->vertexnum);
      for(i = 0; i < p_obj->vertexnum; i++) {
        mujbod2bod(p_obj->p_vertex_kosti+i,p_obj->p_vertex+i);
      }
    }
    key_kosti_updatuj_vertexy_rec(p_obj, p_animace->p_child);
  }
}

void key_kosti_interpoluj_rec(JOINT *p_joint, int framenum, int loop)
{
  JOINT_KEYS *p_key = p_joint->p_keys;
  int keynum = framenum;
  int endtime = calc_endtime(framenum);
  int i;

  init_matrix(&p_joint->m);

  if(p_joint->p_pos) {
    free(p_joint->p_pos);
    p_joint->p_pos = NULL;
    p_joint->pos_keys = 0;
  }
  if(p_joint->p_rot) {
    free(p_joint->p_rot);
    p_joint->p_rot = NULL;
    p_joint->rot_keys = 0;
  }
  if(p_joint->p_scs) {
    free(p_joint->p_scs);
    p_joint->p_scs = NULL;
    p_joint->scs_keys = 0;
  }
  
  if(p_key) {
    if(p_key->p_pos) {
      p_joint->p_pos = (BOD *)mmalloc(sizeof(p_joint->p_pos[0])*keynum);
      p_joint->pos_keys = keynum;
      for(i = 0; i < keynum; i++)
        key_track_interpolace_bod(p_joint->p_pos+i,p_key->p_pos,p_key->p_pkeys,i*SIM_KONSTI,endtime,p_key->pos_keys,loop);
    }
    if(p_key->p_rot) {
      p_joint->p_rot = (QUAT *)mmalloc(sizeof(p_joint->p_rot[0])*keynum);
      p_joint->rot_keys = keynum;
      for(i = 0; i < keynum; i++) {
        key_track_interpolace_quat(p_joint->p_rot+i,p_key->p_rot,p_key->p_rkeys,i*SIM_KONSTI,endtime,p_key->rot_keys,loop);
      }
    }
    if(p_key->p_scale) {
      p_joint->p_scs = (BOD *)mmalloc(sizeof(p_joint->p_scs[0])*keynum);
      p_joint->scs_keys = keynum;
      for(i = 0; i < keynum; i++)
        key_track_interpolace_bod(p_joint->p_scs+i,p_key->p_scale,p_key->p_skeys,i*SIM_KONSTI,endtime,p_key->scs_keys,loop);
    }
  }
  
  // proved rekurzivni animace
  if(p_joint->p_next)
    key_kosti_interpoluj_rec(p_joint->p_next,framenum,loop);
  if(p_joint->p_child)
    key_kosti_interpoluj_rec(p_joint->p_child,framenum,loop);
}

// prepocita animace z klicu do linearni animace
void key_kosti_interpoluj(JOINT_ANIMACE *p_animace)
{
  if(p_animace) {
    key_kosti_interpoluj_rec(p_animace->p_child, p_animace->framenum, TRUE);
  }
}

void key_kosti_animuj_rec(JOINT *p_joint, GLMATRIX *p_top, int time, int keynum)
{
  float frame,t,t1;  
  BOD   *p1,*p2,pv;
  int   kn,lasttime;
  GLMATRIX *p_m = &p_joint->m;

  init_matrix(p_m);

  lasttime = (keynum-1)*SIM_KONSTI;
  if(time < lasttime) {
    t = (float)time/SIM_KONST;
    frame = (float)floor((float)t);
    t -= frame;
    kn = ftoi(frame);
    t1 = 1.0f-t;
    if(p_joint->p_pos) {
      p1 = p_joint->p_pos+kn;
      p2 = p_joint->p_pos+kn+1;
      p_joint->p.x = p_m->_41 = p1->x*t1 + p2->x*t;
      p_joint->p.y = p_m->_42 = p1->y*t1 + p2->y*t;
      p_joint->p.z = p_m->_43 = p1->z*t1 + p2->z*t;
    }
    if(p_joint->p_rot) {      
      quat_to_matrix(p_m,slerp(p_joint->p_rot+kn,p_joint->p_rot+kn+1,t,&p_joint->r));
    }
    if(p_joint->p_scs) {
      p1 = p_joint->p_scs+kn;
      p2 = p_joint->p_scs+kn+1;
      p_joint->s.x = p1->x*t1 + p2->x*t;
      p_joint->s.y = p1->y*t1 + p2->y*t;
      p_joint->s.z = p1->z*t1 + p2->z*t;
      scale_matrix(p_m,p_joint->s.x,p_joint->s.y,p_joint->s.z);
    }
  } else {
    kn = keynum-1;
    if(p_joint->p_pos) {
      p1 = (BOD *)&p_m->_41;
      p_joint->p = *p1 = p_joint->p_pos[kn];
    }
    if(p_joint->p_rot) {
      p_joint->r = p_joint->p_rot[kn];
      quat_to_matrix(p_m,p_joint->p_rot+kn);
    }
    if(p_joint->p_scs) {
      p_joint->s = p_joint->p_scs[kn];
      p1 = &p_joint->s;
      scale_matrix(p_m,p1->x,p1->y,p1->z);
    }
  }  

  if(p_top) {
    transformuj_bod_matici_bod(&p_joint->pivot,p_top,&pv);
    pivotuj_matrix(p_m,&pv);
  } else {
    pivotuj_matrix(p_m,&p_joint->pivot);
  }
  
  if(p_top) {
    mat_mult(p_top,p_m,p_m);
  }  

  if(p_joint->p_next)
    key_kosti_animuj_rec(p_joint->p_next,p_m,time,keynum);
  if(p_joint->p_child)
    key_kosti_animuj_rec(p_joint->p_child,p_m,time,keynum);
}

void key_kosti_animuj(JOINT_ANIMACE *p_animace)
{
  if(p_animace) {
    key_kosti_animuj_rec(p_animace->p_child, NULL, p_animace->time, p_animace->framenum);
  }
}


#define MAX_TIME 0xfffffffe
#define NO_TIME  0xffffffff

void key_kosti_serad_klice(byte *p_hodnoty, KEY_POINT_BRS *p_skeys, int keynum, int size_of_hodnota)
{
  KEY_POINT_BRS *p_nkeys = (KEY_POINT_BRS *)mmalloc(sizeof(p_nkeys[0])*keynum);
  byte          *p_nhod = (byte *)mmalloc(sizeof(byte)*size_of_hodnota*keynum);
  dword          min_time,
                 min_klic;
  int            i,new_akt = 0;

  while(1) {
    min_time = MAX_TIME;
    for(i = 0; i < keynum; i++) {
      if(p_skeys[i].time <= min_time) {
        min_time = p_skeys[i].time;
        min_klic = i;
      }
    }
    if(min_time == MAX_TIME)
      break;
    
    p_nkeys[new_akt] = p_skeys[min_klic];
    memcpy(p_nhod+new_akt*size_of_hodnota,p_hodnoty+min_klic*size_of_hodnota,sizeof(byte)*size_of_hodnota);
    new_akt++;
    p_skeys[min_klic].time = NO_TIME;
  }

  memcpy(p_hodnoty,p_nhod,sizeof(byte)*size_of_hodnota*keynum);
  memcpy(p_skeys,p_nkeys,sizeof(p_nkeys[0])*keynum);

  free(p_nkeys);
  free(p_nhod);
}

/* 
   Streamovy animace vertexu
*/
void key_kosti_stream_animuj_rec(JOINT_ANIMACE *p_anim, JOINT *p_joint, GLMATRIX *p_top, int time, int keynum)
{
  float     frame,t,t1,x1,y1,z1,x2,y2,z2;
  BOD      *p1,*p2,pv;
  BOD      *ps1in,*ps1out;
  BOD      *ps2in,*ps2out;
  BOD      *pv1,*pv2,*pv3,*pv4;
  int       kn,lasttime,num,*p_ind,i,v;
  GLMATRIX *p_m;

  while(p_joint) {
    p_m = init_matrix(&p_joint->m);

    lasttime = (keynum-1)*SIM_KONSTI;
    if(time < lasttime) {
      t = ((float)time)/SIM_KONST;
      frame = (float)floor((float)t);
      
      t -= frame;
      kn = ftoi(frame);
      t1 = 1.0f-t;
      
      if(p_joint->p_pos) {
        p1 = p_joint->p_pos+kn;
        p2 = p_joint->p_pos+kn+1;      
        p_m->_41 = p1->x*t1 + p2->x*t;
        p_m->_42 = p1->y*t1 + p2->y*t;
        p_m->_43 = p1->z*t1 + p2->z*t;
      }
      if(p_joint->p_rot) {
        quat_to_matrix(p_m,slerp(p_joint->p_rot+kn,p_joint->p_rot+kn+1,t,&p_joint->r));
      }
      if(p_joint->p_scs) {
        p1 = p_joint->p_scs+kn;
        p2 = p_joint->p_scs+kn+1;
        scale_matrix(p_m,p1->x*t1 + p2->x*t,p1->y*t1 + p2->y*t,p1->z*t1 + p2->z*t);
      }
    } else {
      kn = keynum-1;
      if(p_joint->p_pos) {
        p1 = (BOD *)&p_m->_41;
        *p1 = p_joint->p_pos[kn];
      }
      if(p_joint->p_rot) {
        quat_to_matrix(p_m,p_joint->p_rot+kn);
      }
      if(p_joint->p_scs) {
        p1 = p_joint->p_scs+kn;
        scale_matrix(p_m,p1->x,p1->y,p1->z);
      }
    }  
    
    if(p_top) {
      transformuj_bod_matici_bod(&p_joint->pivot,p_top,&pv);
      pivotuj_matrix(p_m,&pv);
    } else {
      pivotuj_matrix(p_m,&p_joint->pivot);
    }  
    
    if(p_top) {
      mat_mult(p_top,p_m,p_m);
    }  
    
    ps1in = p_anim->p_pos_in;
    ps1out = p_anim->p_pos_out;
    ps2in = p_anim->p_norm_in;
    ps2out = p_anim->p_norm_out;
    
    num = p_joint->vertexnum;
    p_ind = p_joint->p_vertexlist;

    if(ps1in && ps1out) {
      
      num = p_joint->vertexnum;
      p_ind = p_joint->p_vertexlist;
      
      for(i = 0; i < num; i++) {
        v = *p_ind++;
        
        pv1 = ps1in+v;
        pv2 = ps1out+v;
        pv3 = ps2in+v;
        pv4 = ps2out+v;
        
        x1 = pv1->x;
        y1 = pv1->y;
        z1 = pv1->z;
        
        x2 = pv3->x;
        y2 = pv3->y;
        z2 = pv3->z;
        
        pv2->x = p_m->_11*x1 + p_m->_21*y1 + p_m->_31*z1 + p_m->_41;
        pv4->x = p_m->_11*x2 + p_m->_21*y2 + p_m->_31*z2;
        
        pv2->y = p_m->_12*x1 + p_m->_22*y1 + p_m->_32*z1 + p_m->_42;
        pv4->y = p_m->_12*x2 + p_m->_22*y2 + p_m->_32*z2;
        
        pv2->z = p_m->_13*x1 + p_m->_23*y1 + p_m->_33*z1 + p_m->_43;
        pv4->z = p_m->_13*x2 + p_m->_23*y2 + p_m->_33*z2;
      }
    }
    
    if(p_joint->p_next)
      key_kosti_stream_animuj_rec(p_anim,p_joint->p_next,p_m,time,keynum);
    
    p_top = p_m;
    p_joint = p_joint->p_child;
  }
}

JOINT_KEYS * key_joint_kopiruj_spline(JOINT_KEYS *p_src)
{
  if(p_src) {
    JOINT_KEYS *p_new = (JOINT_KEYS *)kopiruj_pole(p_src,sizeof(p_src[0]));
    
    p_new->p_pkeys = (KEY_POINT_BRS *)kopiruj_pole(p_new->p_pkeys,sizeof(p_new->p_pkeys[0])*p_new->pos_keys);
    p_new->p_pos   = (BOD *)kopiruj_pole(p_new->p_pos,  sizeof(p_new->p_pos[0])*p_new->pos_keys);
    
    p_new->p_rkeys = (KEY_POINT_BRS *)kopiruj_pole(p_new->p_rkeys,sizeof(p_new->p_rkeys[0])*p_new->rot_keys);
    p_new->p_rot   = (QUAT *)kopiruj_pole(p_new->p_rot,  sizeof(p_new->p_rot[0])*p_new->rot_keys);
    
    p_new->p_skeys = (KEY_POINT_BRS *)kopiruj_pole(p_new->p_skeys,sizeof(p_new->p_skeys[0])*p_new->scs_keys);
    p_new->p_scale = (BOD *)kopiruj_pole(p_new->p_scale,sizeof(p_new->p_scale[0])*p_new->scs_keys);
    
    return(p_new);
  } else {
    return(NULL);
  }
}

JOINT * key_joint_kopiruj(JOINT *p_src)
{
  JOINT *p_new = (JOINT *)kopiruj_pole(p_src,sizeof(p_src[0]));  

  p_new->p_vertexlist = (int*)kopiruj_pole(p_src->p_vertexlist,sizeof(p_new->p_vertexlist[0])*p_new->vertexnum);
  p_new->p_pos = (BOD *)kopiruj_pole(p_src->p_pos,sizeof(p_new->p_pos[0])*p_new->pos_keys);
  p_new->p_rot = (QUAT *)kopiruj_pole(p_src->p_rot,sizeof(p_new->p_rot[0])*p_new->rot_keys);
  p_new->p_scs = (BOD *)kopiruj_pole(p_src->p_scs,sizeof(p_new->p_scs[0])*p_new->scs_keys);
  
  p_new->p_keys = key_joint_kopiruj_spline(p_src->p_keys);
  return(p_new);
}

JOINT * key_joint_vyrob(void)
{
  JOINT *p_src = (JOINT *)mmalloc(sizeof(p_src[0]));
  p_src->r.w = 1;
  p_src->s.x = p_src->s.y = p_src->s.z = 1.0f;
  init_matrix(&p_src->m);
  return(p_src);
}

JOINT * key_kosti_kopiruj_rec(JOINT *p_src)
{
  JOINT *p_new = key_joint_kopiruj(p_src);
  if(p_src->p_next)
    p_new->p_next = key_kosti_kopiruj_rec(p_src->p_next);
  if(p_src->p_child)
    p_new->p_child = key_kosti_kopiruj_rec(p_src->p_child);
  return(p_new);
}

/* Skopiruje animace kosti
*/
JOINT_ANIMACE * key_kosti_kopiruj(JOINT_ANIMACE *p_src)
{
  JOINT_ANIMACE *p_new = (JOINT_ANIMACE *)mmalloc(sizeof(p_src[0]));
  memcpy(p_new,p_src,sizeof(p_src[0]));
  p_new->p_child = key_kosti_kopiruj_rec(p_new->p_child);
  return(p_new);
}

JOINT_ANIMACE * key_kosti_kopiruj_animace(JOINT_ANIMACE *p_prvni)
{
  JOINT_ANIMACE *p_tmp;
  JOINT_ANIMACE *p_frs = NULL;

  while(p_prvni) {
    if(!p_frs) {
      p_frs = p_tmp = key_kosti_kopiruj(p_prvni);
    } else {
      p_tmp->p_next = key_kosti_kopiruj(p_prvni);
    }
    p_prvni = p_prvni->p_next;
  }

  return(p_frs);
}
