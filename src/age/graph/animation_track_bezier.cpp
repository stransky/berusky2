/*
 *        .þÛÛþ þ    þ þÛÛþ.     þ    þ þÛÛÛþ.  þÛÛÛþ .þÛÛþ. þ    þ
 *       .þ   Û Ûþ.  Û Û   þ.    Û    Û Û    þ  Û.    Û.   Û Ûþ.  Û
 *       Û    Û Û Û  Û Û    Û    Û   þ. Û.   Û  Û     Û    Û Û Û  Û
 *     .þþÛÛÛÛþ Û  Û Û þÛÛÛÛþþ.  þþÛÛ.  þþÛÛþ.  þÛ    Û    Û Û  Û Û
 *    .Û      Û Û  .þÛ Û      Û. Û   Û  Û    Û  Û.    þ.   Û Û  .þÛ
 *    þ.      þ þ    þ þ      .þ þ   .þ þ    .þ þÛÛÛþ .þÛÛþ. þ    þ
 *
 * AGE (C) AnakreoN
 * Martin Stransky <stransky@anakreon.cz> 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "age.h"

int anim_track_bezier::key_get_internal(int time)
{    
  ANIM_KEY *p_tmp;
  int l,p,s;
  
  l = 0; p = keynum_get();
  
  while(l != p) {
    s = l+((p-l)>>1);
    p_tmp = key_get(s);
    if(p_tmp->time == time)
      return(s);
    else {     
      if(time > p_tmp->time) {
        p_tmp = key_get(s+1);
        if(time < p_tmp->time)
          return(s);
        else
          l = s;
      }
      else
        p = s;
    }
  }
  return(NO_KEY);
}

int anim_track_bezier::key_get_time(int time)
{ 
  assert(track_get());
  
  int keynum = keynum_get();
  
  if(keynum == 0) {
    return(NO_KEY);
  }
  else if(keynum < 6) {
    int i;
    for(i = 0; i < keynum; i++)
      if(time == key_get(i)->time)
        return(i);
    return(NO_KEY);    
  }
  else {
    if(time < key_get(0)->time || time > key_get(keynum-1)->time)
      return(NO_KEY);
    
    return(key_get_internal(time));
  }
}

KEY_TYPE anim_track_bezier::key_get_time(int time, int *p_kn, int *p_kn1, float *p_time)
{
  int   keynum = keynum_get();
  int   start,stop,k1;
  int   dop,intrv;
  int   t1,t2;
  
  k1 = keynum-1;
  
  start = key_get(0)->time;
  stop = key_get(k1)->time;
  
  int endtime = length_get();
  
  if(flag_get(FLAG_TRACK_LOOP) && time > 2*endtime) {
    time = endtime + time%endtime;
  }
  
  if(time < start) {
    if(flag_get(FLAG_TRACK_LOOP)) {
      *p_kn = k1;
      *p_kn1 = 0;
      dop = endtime-stop;
      intrv = dop+start;
      *p_time = (intrv > DELTA_INTERVAL) ? (float)(time+dop)/(float)intrv : 0.0f;
      return(KEYS_ALL);
    } else {
      *p_kn = 0;
      return(KEYS_ONE);
    }
  } else if(time >= stop) {
    if(flag_get(FLAG_TRACK_LOOP)) {
      *p_kn = k1;
      *p_kn1 = 0;
      intrv = (endtime-stop)+start;
      *p_time = (intrv > DELTA_INTERVAL) ? (float)(time-stop)/(float)intrv : 0.0f;
      return(KEYS_ALL);
    } else {
      *p_kn = k1;
      return(KEYS_ONE);
    }
  } else {
    if(keynum == 2) {
      *p_kn = 0;
      *p_kn1 = 1;
      t1 = key_get(0)->time;
      t2 = key_get(1)->time;
    } else {
      *p_kn = key_get_internal(time);
      *p_kn1 = (*p_kn)+1;
      t1 = key_get(*p_kn)->time;
      t2 = key_get(*p_kn1)->time;
    }
    *p_time = (float)(time-t1)/(float)(t2-t1);
    return(KEYS_ALL);
  }
}

VECT * anim_track_bez_vect::tangent(int kn, BEZIER_KEY_CLASS key_class, VECT *p_tn)
{
  int   kn_1 = kn-1, // key k(n-1)
        kn1  = kn+1; // key k(n+1)
  float bias,continuity,tension,tmp,tmp2;
  VECT  *p_p0,*p_p1,*p_p2,
    t1,
    g1,g2,g3;
  
  bias = p_track[kn].bias;
  continuity = p_track[kn].continuity;
  tension = p_track[kn].tension;
  
  if(!kn) {
    //kn = 0,  zacatek - mam p(n), p(n+1)    
    p_p1 = p_track[kn].value_get();
    p_p2 = p_track[kn1].value_get();
    
    if(keynum == 2) {
      //track ma jen 2 klice
      //b0 = (p1-p0)*(1-T)
      tmp = 1.0f - tension;
      p_tn->x = (p_p2->x - p_p1->x)*tmp;
      p_tn->y = (p_p2->y - p_p1->y)*tmp;
      p_tn->z = (p_p2->z - p_p1->z)*tmp;
      return(p_tn);
    } else { // zacatecni tangenta
      if(flag_get(FLAG_TRACK_LOOP)) {
        p_p0 = p_track[keynum-1].value_get();
      } else {
        // trak ma vice klicu
        tangent(1, CLASS_IN, &t1);
        
        // b0 = ((p1 - p0)*1.5 - a1*0.5)*(1-T)
        tmp = 1.0f - tension;
        p_tn->x = ((p_p2->x - p_p1->x)*1.5f - t1.x*0.5f)*tmp;
        p_tn->y = ((p_p2->y - p_p1->y)*1.5f - t1.y*0.5f)*tmp;
        p_tn->z = ((p_p2->z - p_p1->z)*1.5f - t1.z*0.5f)*tmp;
        return(p_tn);
      }
    }
  } else if(kn == (keynum-1)) {// konec - p(n-1), p(n)    
    p_p0 = p_track[kn_1].value_get();
    p_p1 = p_track[kn].value_get();
    
    if(keynum == 2) {
      //track ma jen 2 klice
      //a1 = (p1-p0)*(1-T)
      tmp = 1.0f - tension;
      p_tn->x = (p_p1->x - p_p0->x)*tmp;
      p_tn->y = (p_p1->y - p_p0->y)*tmp;
      p_tn->z = (p_p1->z - p_p0->z)*tmp;
      return(p_tn);
    } else {
      if(flag_get(FLAG_TRACK_LOOP)) { // koncova tangenta
        p_p2 = p_track->value_get();        
      } else {
        // bez opakovani      
        tangent(kn_1, CLASS_OUT, &t1);
        
        // an = ((pn - p(n-1))*1.5 - b(n-1)*0.5)*(1-T)
        tmp = 1.0f - tension;
        p_tn->x = ((p_p1->x - p_p0->x)*1.5f - t1.x*0.5f)*tmp;
        p_tn->y = ((p_p1->y - p_p0->y)*1.5f - t1.y*0.5f)*tmp;
        p_tn->z = ((p_p1->z - p_p0->z)*1.5f - t1.z*0.5f)*tmp;
        
        return(p_tn);
      }
    }
  } else { // stredni bod - mam p(n-1), p(n), p(n+1)
    p_p0 = p_track[kn_1].value_get();
    p_p1 = p_track[kn].value_get();
    p_p2 = p_track[kn1].value_get();
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
  g3 = g2 - g1;
  
  // tangent case a: an = (g1 + g3*(0.5 + 0.5*C))*(1-T),
  // tangent case b: bn = (g1 + g3*(0.5 - 0.5*C))*(1-T).
  tmp = 1.0f-tension;
  tmp2 = 0.5f + key_class*0.5f*continuity;
  p_tn->x = (g1.x + g3.x*tmp2)*tmp;
  p_tn->y = (g1.y + g3.y*tmp2)*tmp;
  p_tn->z = (g1.z + g3.z*tmp2)*tmp;
  
  return(p_tn);
}

VECT * anim_track_bez_vect::interpolate(VECT *p_result, int time)
{
  VECT   *p0,*p1,pt0,pt1;
  float F1,F2,F3,F4;
  float t,t2,t3;
  int   kn,kn1;
  int   ret;
  
  if(keynum > 1) {
    ret = key_get_time(time, &kn, &kn1, &t);
    if(ret == KEYS_ALL) {
      t2 = t*t; t3 = t2*t;
      
      tangent(kn,  CLASS_OUT, &pt0);
      tangent(kn1, CLASS_IN, &pt1);
      
      p0 = p_track[kn].value_get();
      p1 = p_track[kn1].value_get();
      
      F1 = 2*t3-3*t2+1;
      F2 = t3-2*t2+t;
      F3 = -2*t3+3*t2;
      F4 = t3-t2;
      
      p_result->x = p0->x*F1 + pt0.x*F2 + p1->x*F3 + pt1.x*F4;
      p_result->y = p0->y*F1 + pt0.y*F2 + p1->y*F3 + pt1.y*F4;
      p_result->z = p0->z*F1 + pt0.z*F2 + p1->z*F3 + pt1.z*F4;
    } else {
      *p_result = p_track[kn].value;
    }       
  } else {
    if(keynum) {
      *p_result = p_track->value;
    }
  }
  
  return(p_result);
}

QUAT * anim_track_bez_quat::tangent(int kn, BEZIER_KEY_CLASS key_class, QUAT *p_q)
{
  int   kn_1 = kn-1, // key k(n-1)
        kn1  = kn+1; // key k(n+1)
  float bias,continuity,tension;
  QUAT  q0, q2, //q1
        g1, g2, g3;
  
  bias = p_track[kn].bias;
  continuity = p_track[kn].continuity;
  tension = p_track[kn].tension;
  
  if(!kn) { 
    //kn = 0,  zacatek - mam p(n), p(n+1)    
    if(keynum == 2) { //track ma jen 2 klice
      return(slerp(p_track[kn].value_get(),p_track[kn1].value_get(),(1.0f-tension)/3.0f,p_q));
    } else { // prvni body (-1,0,1)
      if(flag_get(FLAG_TRACK_LOOP)) {
        return(slerp(p_track[kn].value_get(),slerp(slerp(p_track[kn].value_get(),p_track[keynum-1].value_get(),-(1+bias)/3.0f,&g1),
          slerp(p_track[kn].value_get(),p_track[kn1].value_get(), (1-bias)/3.0f,&g2),0.5f + key_class*0.5f*continuity,&g3),
          key_class*(tension-1.0f),p_q));
      } else {
        tangent(kn1, CLASS_OUT, &q2);
        return(slerp(p_track[kn].value_get(),&q2,(1.0f-tension)*0.5f,p_q));
      }
    }
  } else if(kn == (int)keynum-1) {// konec - p(n-1), p(n)
    if(keynum == 2) { //track ma jen 2 klice
      return(slerp(p_track[kn].value_get(),p_track[kn_1].value_get(),(1.0f-tension)/3.0f,p_q));
    } else { // posledni body bez opakovani
      if(flag_get(FLAG_TRACK_LOOP)) { // posledni (-2,-1,0)
        return(slerp(p_track[kn].value_get(),
                    slerp(slerp(p_track[kn].value_get(),
                          p_track[kn_1].value_get(),-(1+bias)/3.0f,&g1),
                    slerp(p_track[kn].value_get(),p_track->value_get(), 
                          (1-bias)/3.0f,&g2), 0.5f + key_class*0.5f*continuity,&g3), 
                          key_class*(tension-1.0f),p_q));
      } else {
        tangent(kn_1, CLASS_IN, &q0);
        return(slerp(p_track[kn].value_get(),&q0,(1.0f-tension)*0.5f,p_q));
      }
    }
  } else { // stredni bod - mam q(n-1), q(n), q(n+1)
    return(slerp(p_track[kn].value_get(),slerp(slerp(p_track[kn].value_get(),p_track[kn_1].value_get(),-(1+bias)/3.0f,&g1),
      slerp(p_track[kn].value_get(),p_track[kn1].value_get(), (1-bias)/3.0f,&g2),
      0.5f + key_class*0.5f*continuity,&g3), 
      key_class*(tension-1.0f),p_q));
  }
}

QUAT * anim_track_bez_quat::interpolate(QUAT *p_result, int time)
{  
  QUAT  q0,q1,q2;
  QUAT  qn,qn1;
  QUAT  bn,an;  
  int   kn,kn1;
  float t;
  int   ret;
  
  if(keynum > 1) {
    ret = key_get_time(time, &kn, &kn1, &t);
    if(ret == KEYS_ALL) {
      assert(0.0f <= t && t <= 1.0f);
      
      tangent(kn,  CLASS_OUT, &bn);
      tangent(kn1, CLASS_IN, &an);
      
      qn = p_track[kn].value;
      qn1= p_track[kn1].value;
      
      slerp(&qn,&bn, t,&q0);
      slerp(&bn,&an, t,&q1);
      slerp(&an,&qn1,t,&q2);  
      
      slerp(&q0,&q1,t,&qn);
      slerp(&q1,&q2,t,&qn1);
      
      return(slerp(&qn,&qn1,t,p_result));
    } else {
      *p_result = p_track[kn].value;
      return(p_result);
    }
  } else {
    if(keynum) {
      *p_result = p_track->value;
      return(p_result);
    } else
      return(NULL);
  }
}

float anim_track_bez_float::tangent(int kn, BEZIER_KEY_CLASS key_class, float *p_tn)
{
  float bias,continuity,tension;
  float t1,g1,g3;  

  assert(kn < keynum);
  
  bias = p_track[kn].bias;
  continuity = p_track[kn].continuity;
  tension = p_track[kn].tension;
  
  if(!kn) {
    if(keynum == 2) {
      *p_tn = (p_track[1].value - p_track[0].value)*(1.0f - tension);
      return(*p_tn);
    } else {
      if(flag_get(FLAG_TRACK_LOOP)) { // prvni klic
        g1 = (p_track[0].value - p_track[keynum-1].value)*(1.0f+bias);
        g3 = (p_track[1].value - p_track[0].value)*(1.0f-bias)-g1;
        *p_tn = (g1 + g3*(0.5f + key_class*0.5f*continuity))*(1.0f-tension);        
      } else {
        tangent(1, CLASS_IN, &t1);
        *p_tn = ((p_track[kn+1].value - p_track[kn].value)*1.5f - t1*0.5f)*(1.0f - tension);
      }
      return(*p_tn);
    }
  } else if(kn == (keynum-1)) {// konec - p(n-1), p(n)
    if(keynum == 2) {
      *p_tn = (p_track[kn].value - p_track[kn-1].value)*(1.0f - tension);
      return(*p_tn);
    } else {
      if(flag_get(FLAG_TRACK_LOOP)) { // posledni klic
        g1 = (p_track[kn].value - p_track[kn-1].value)*(1.0f+bias);
        g3 = (p_track[0].value - p_track[kn].value)*(1.0f-bias)-g1;
        *p_tn = (g1 + g3*(0.5f + key_class*0.5f*continuity))*(1.0f-tension);
      } else {
        tangent(kn-1, CLASS_OUT, &t1);
        *p_tn = ((p_track[kn].value-p_track[kn-1].value)*1.5f - t1*0.5f)*(1.0f - tension);
      }
      return(*p_tn);
    }
  } else { // stredni bod - mam p(n-1), p(n), p(n+1)
    g1 = (p_track[kn].value - p_track[kn-1].value)*(1.0f+bias);
    g3 = (p_track[kn+1].value - p_track[kn].value)*(1.0f-bias)-g1;
    *p_tn = (g1 + g3*(0.5f + key_class*0.5f*continuity))*(1.0f-tension);
    return(*p_tn);
  }
}

float anim_track_bez_float::interpolate(float *p_result, int time)
{
  float pt0,pt1;  
  float t,t2,t3;
  int   ret;
  int   kn,kn1;  
  
  if(keynum > 1) {
    ret = key_get_time(time, &kn, &kn1, &t);
    if(ret == KEYS_ALL) {
      t2 = t*t; t3 = t2*t;
      tangent(kn,  CLASS_OUT, &pt0);
      tangent(kn1, CLASS_IN, &pt1);
      return((*p_result = p_track[kn].value*(2*t3-3*t2+1) + pt0*(t3-2*t2+t) 
              + p_track[kn1].value*(-2*t3+3*t2) + pt1*(t3-t2)));
    } else {
      return(*p_result = p_track[kn].value);
    }
  } else {
    if(keynum) {
      return(*p_result = p_track->value);
    }
    else 
      return(FLT_MAX);
  }
}

WVECT * anim_track_bez_wvect::tangent(int kn, BEZIER_KEY_CLASS key_class, WVECT *p_tn)
{
  int   kn_1 = kn-1, // key k(n-1)
        kn1  = kn+1; // key k(n+1)
  float bias,continuity,tension,tn1,bp1;
  WVECT *p_p0,*p_p1,*p_p2,
        t1,
        g1,g2,g3;
  
  bias = p_track[kn].bias;
  continuity = p_track[kn].continuity;
  tension = p_track[kn].tension;
  
  if(!kn) {
    //kn = 0,  zacatek - mam p(n), p(n+1)    
    p_p1 = p_track[kn].value_get();
    p_p2 = p_track[kn1].value_get();
    
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
      if(flag_get(FLAG_TRACK_LOOP)) {
        p_p0 = p_track[keynum-1].value_get();
      } else {
        // trak ma vice klicu            
        tangent(1, CLASS_IN, &t1);
        
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
    p_p0 = p_track[kn_1].value_get();
    p_p1 = p_track[kn].value_get();
    
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
      if(flag_get(FLAG_TRACK_LOOP)) {
        p_p2 = p_track->value_get();
      } else {
        // bez opakovani      
        tangent(kn_1, CLASS_OUT, &t1);
        
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
    p_p0 = p_track[kn_1].value_get();
    p_p1 = p_track[kn].value_get();
    p_p2 = p_track[kn1].value_get();
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
  wvect_sub(&g2,&g1,&g3);
  
  // tangent case a: an = (g1 + g3*(0.5 + 0.5*C))*(1-T),
  // tangent case b: bn = (g1 + g3*(0.5 - 0.5*C))*(1-T).
  
  tn1 = (1.0f - tension);
  bp1 = (0.5f + key_class*0.5f*continuity);
  p_tn->x = (g1.x + g3.x*bp1)*tn1;
  p_tn->y = (g1.y + g3.y*bp1)*tn1;
  p_tn->z = (g1.z + g3.z*bp1)*tn1;
  p_tn->w = (g1.w + g3.w*bp1)*tn1;
  
  return(p_tn);    
}

WVECT * anim_track_bez_wvect::interpolate(WVECT *p_result, int time)
{
  WVECT  *p0,*p1,pt0,pt1;
  float F1,F2,F3,F4;
  float t,t2,t3;
  int   kn,kn1;
  int   ret;
  
  if(keynum > 1) {
    ret = key_get_time(time, &kn, &kn1, &t);
    if(ret == KEYS_ALL) {
      t2 = t*t; t3 = t2*t;
      
      tangent(kn,  CLASS_OUT, &pt0);
      tangent(kn1, CLASS_IN, &pt1);
      
      p0 = p_track[kn].value_get();
      p1 = p_track[kn1].value_get();
      
      F1 = 2*t3-3*t2+1;
      F2 = t3-2*t2+t;
      F3 = -2*t3+3*t2;
      F4 = t3-t2;
      
      p_result->x = p0->x*F1 + pt0.x*F2 + p1->x*F3 + pt1.x*F4;
      p_result->y = p0->y*F1 + pt0.y*F2 + p1->y*F3 + pt1.y*F4;
      p_result->z = p0->z*F1 + pt0.z*F2 + p1->z*F3 + pt1.z*F4;
      p_result->w = p0->w*F1 + pt0.w*F2 + p1->w*F3 + pt1.w*F4;
    } else {
      *p_result = p_track[kn].value;
    }
  } else {
    if(keynum) {
      *p_result = p_track->value;
    }
  }
  
  return(p_result);
}
