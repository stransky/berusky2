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

/*
  KEY_TYPE anim_track_linear::key_get_time(int time, int *p_kn, int *p_kn1, float *p_time)
  {
    int lasttime = (keynum-1)*LINEAR_FRAMES_DELAYI;
    if(time < lasttime) {
      t = (float)time/LINEAR_FRAMES_DELAYI;
      float frame = (float)floor((float)t);
      t -= frame;
      kn = (int)frame;
      t1 = 1.0f-t;
    } else {
      kn = keynum-1;
      t = t1 = 0.0f;
    }
  }
*/

KEY_TYPE anim_track_linear::key_get_time(int time, int *p_kn, int *p_kn1, float *p_time)
{
  int   keynum = keynum_get();
  int   start,stop,k1;
  int   dop,intrv;
  int   t1,t2;
  
  k1 = keynum-1;
  
  start = 0;
  stop = keynum*LINEAR_FRAMES_DELAYI;
  
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

VECT * anim_track_lin_vect::interpolate(VECT *p_result, int time)
{
  VECT   *p_p0,*p_p1;
  float t,t2;
  int   kn,kn1;
  int   ret;
  
  if(keynum > 1) {
    ret = key_get_time(time, &kn, &kn1, &t);
    if(ret == KEYS_ALL) {
      t2 = 1.0f-t;
      p_p0 = p_track[kn].value_get();
      p_p1 = p_track[kn1].value_get();
      p_result->x = p_p0->x*t2 + p_p1->x*t;
      p_result->y = p_p0->y*t2 + p_p1->y*t;
      p_result->z = p_p0->z*t2 + p_p1->z*t;
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

QUAT * anim_track_lin_quat::interpolate(QUAT *p_result, int time)
{
  int   kn,kn1;
  float t;
  int   ret;
  
  if(keynum > 1) {
    ret = key_get_time(time, &kn, &kn1, &t);
    if(ret == KEYS_ALL) {
      assert(0.0f <= t && t <= 1.0f);
      return(slerp(p_track[kn].value_get(),p_track[kn1].value_get(),t,p_result));
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

float anim_track_lin_float::interpolate(float *p_result, int time)
{
  float t,t2;  
  int   kn,kn1,ret;
  
  if(keynum > 1) {
    ret = key_get_time(time, &kn, &kn1, &t);
    if(ret == KEYS_ALL) {
      t2 = 1.0f-t;
      return(*p_result = p_track[kn].value*t2+p_track[kn1].value*t);
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

WVECT * anim_track_lin_wvect::interpolate(WVECT *p_result, int time)
{
  WVECT  *p_p0,*p_p1;
  float t,t2;
  int   kn,kn1;
  int   ret;
  
  if(keynum > 1) {
    ret = key_get_time(time, &kn, &kn1, &t);
    if(ret == KEYS_ALL) {
      t2 = 1.0f-t;
      p_p0 = p_track[kn].value_get();
      p_p1 = p_track[kn1].value_get();    
      p_result->x = p_p0->x*t2 + p_p1->x*t;
      p_result->y = p_p0->y*t2 + p_p1->y*t;
      p_result->z = p_p0->z*t2 + p_p1->z*t;
      p_result->w = p_p0->w*t2 + p_p1->w*t;
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
