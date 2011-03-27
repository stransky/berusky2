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

/*
 * Animation track - abstraction and implementation
 */

/* Written according to:

   Splines & Quaternions (splines.txt)
   -----------------------------------
   by
   Jochen Wilhelmy
   a.k.a.
   digisnap
   digisnap@cs.tu-berlin.de
   06.08.1997
*/

#ifndef __ANIMATION_TRACK_BEZIER_H__
#define __ANIMATION_TRACK_BEZIER_H__

/*
 * Class for bezier animation keys
 */
typedef struct anim_key_bez : public anim_key
{

  float    tension;
  float    continuity;
  float    bias;

} ANIM_KEY_BEZ;

typedef class anim_track_bezier : public anim_track {

  /*
   * Find k[n] for a given time and return the n
   */  
private:    
  
  int key_get_internal(int time);

  /*
   * Find k[n] for a given time
   */
public:
  
  int key_get_time(int time);
  
  /*
   * Get key for frame
   */
public:    
  
  int key_get_frame(int frame)
  {
    return(key_get_time(frame*LINEAR_FRAMES_DELAYI));
  }
  
  /*
   * Find k[n], k[n+1], time for a given time and return the type of an interval
   */  
public:
  
  KEY_TYPE key_get_time(int time, int *p_kn, int *p_kn1, float *p_time);
  
} ANIM_TRACK_BEZIER;

/* ---------------------------------------------------------------------
*/

/*
 * Class for besier animation keys - vectors
 */
typedef struct anim_key_bez_vect : public anim_key_bez
{

public:
  
  VECT value;
  
public:

  VECT * value_get(void)
  {
    return(&value);
  }

} ANIM_KEY_BEZ_VECT;

/*
 * Besier animation tracks - vectors
 */
typedef class anim_track_bez_vect : public anim_track_bezier
{
  
  ANIM_KEY_BEZ_VECT *p_track;
  int                keynum;

public:
  
  void destroy(void)
  {
    ffree(p_track);
  }

  void create(int key_num)
  {    
    destroy();
    if(key_num)
      p_track = (ANIM_KEY_BEZ_VECT *)mmalloc(sizeof(p_track[0])*key_num);
    keynum = key_num;
  }

public:  
  
  virtual ANIM_KEY_BEZ_VECT * key_get(int key_position)
  {
    if(p_track != NULL && key_position >= 0 && key_position < keynum) {
      return(p_track+key_position);
    }
    else {
      return(NULL);
    }    
  }
  
  bool key_set(int key_position, ANIM_KEY_BEZ_VECT *p_key)
  {
    if(p_track != NULL && key_position >= 0 && key_position < keynum) {
      p_track[key_position] = *p_key;
      return(TRUE);
    }
    else {
      return(FALSE);
    }
  }
  
public:
  
  int keynum_get(void)
  {
    return(keynum);
  }
  
  ANIM_KEY * track_get(void)
  {
    return((ANIM_KEY *)p_track);
  }
  
private:

  VECT * tangent(int kn, BEZIER_KEY_CLASS key_class, VECT *p_tn);

public:
  
  VECT * interpolate(VECT *p_result, int time);
  
public:
  
  anim_track_bez_vect(int key_num)
    : p_track(NULL), keynum(0)
  {
    if(key_num)
      create(key_num);
  }
  
  ~anim_track_bez_vect(void)
  {
    destroy();
  }

} ANIM_TRACK_BEZ_VECT;

/* ---------------------------------------------------------------------
*/

typedef class anim_key_bez_quat : public anim_key_bez
{
  
public:  
  
  QUAT value;
  
public:

  QUAT * value_get(void)
  {
    return(&value);
  }

} ANIM_KEY_BEZ_QUAT;

/*
 * Besier animation tracks - vectors
 */
typedef class anim_track_bez_quat : public anim_track_bezier
{
  
  ANIM_KEY_BEZ_QUAT *p_track;
  int                keynum;

public:
  
  void destroy(void)
  {
    ffree(p_track);
  }

  void create(int key_num)
  {    
    destroy();
    if(key_num)
      p_track = (ANIM_KEY_BEZ_QUAT *)mmalloc(sizeof(p_track[0])*key_num);
    keynum = key_num;
  }

public:  
  
  virtual ANIM_KEY_BEZ_QUAT * key_get(int key_position)
  {
    if(p_track != NULL && key_position >= 0 && key_position < keynum) {
      return(p_track+key_position);
    }
    else {
      return(NULL);
    }    
  }
  
  bool key_set(int key_position, ANIM_KEY_BEZ_QUAT *p_key)
  {
    if(p_track != NULL && key_position >= 0 && key_position < keynum) {
      p_track[key_position] = *p_key;
      return(TRUE);
    }
    else {
      return(FALSE);
    }
  }
  
public:
  
  int keynum_get(void)
  {
    return(keynum);
  }
  
  ANIM_KEY * track_get(void)
  {
    return((ANIM_KEY *)p_track);
  }
  
private:

  QUAT * tangent(int kn, BEZIER_KEY_CLASS key_class, QUAT *p_tn);

public:
  
  QUAT * interpolate(QUAT *p_result, int time);
  
public:
  
  anim_track_bez_quat(int key_num)
    : p_track(NULL), keynum(0)
  {
    if(key_num)
      create(key_num);
  }
  
  ~anim_track_bez_quat(void)
  {
    destroy();
  }

} ANIM_TRACK_BEZ_QUAT;

/* ---------------------------------------------------------------------
*/

typedef class anim_key_bez_float : public anim_key_bez
{
  
public:  
  
  float value;
  
public:

  float * value_get(void)
  {
    return(&value);
  }

} ANIM_KEY_BEZ_FLOAT;

/*
 * Besier animation tracks - floats
 */
typedef class anim_track_bez_float : public anim_track_bezier
{
  
  ANIM_KEY_BEZ_FLOAT *p_track;
  int                 keynum;

public:
  
  void destroy(void)
  {
    ffree(p_track);
  }

  void create(int key_num)
  {    
    destroy();
    if(key_num)
      p_track = (ANIM_KEY_BEZ_FLOAT *)mmalloc(sizeof(p_track[0])*key_num);
    keynum = key_num;
  }

public:  
  
  virtual ANIM_KEY_BEZ_FLOAT * key_get(int key_position)
  {
    if(p_track != NULL && key_position >= 0 && key_position < keynum) {
      return(p_track+key_position);
    }
    else {
      return(NULL);
    }    
  }
  
  bool key_set(int key_position, ANIM_KEY_BEZ_FLOAT *p_key)
  {
    if(p_track != NULL && key_position >= 0 && key_position < keynum) {
      p_track[key_position] = *p_key;
      return(TRUE);
    }
    else {
      return(FALSE);
    }
  }
  
public:
  
  int keynum_get(void)
  {
    return(keynum);
  }
  
  ANIM_KEY * track_get(void)
  {
    return((ANIM_KEY *)p_track);
  }
  
private:

  float tangent(int kn, BEZIER_KEY_CLASS key_class, float *p_tn);

public:
  
  float interpolate(float *p_result, int time);
  
public:
  
  anim_track_bez_float(int key_num)
    : p_track(NULL), keynum(0)
  {
    if(key_num)
      create(key_num);
  }
  
  ~anim_track_bez_float(void)
  {
    destroy();
  }

} ANIM_TRACK_BEZ_FLOAT;

/* ---------------------------------------------------------------------
*/

typedef class anim_key_bez_wvect : public anim_key_bez
{
  
public:  
  
  WVECT value;
  
public:

  WVECT * value_get(void)
  {
    return(&value);
  }

} ANIM_KEY_BEZ_WVECT;

/*
 * Besier animation tracks - WVECT
 */
typedef class anim_track_bez_wvect : public anim_track_bezier
{
  
  ANIM_KEY_BEZ_WVECT *p_track;
  int                 keynum;

public:
  
  void destroy(void)
  {
    ffree(p_track);
  }

  void create(int key_num)
  {    
    destroy();
    if(key_num)
      p_track = (ANIM_KEY_BEZ_WVECT *)mmalloc(sizeof(p_track[0])*key_num);
    keynum = key_num;
  }

public:  
  
  virtual ANIM_KEY_BEZ_WVECT * key_get(int key_position)
  {
    if(p_track != NULL && key_position >= 0 && key_position < keynum) {
      return(p_track+key_position);
    }
    else {
      return(NULL);
    }    
  }
  
  bool key_set(int key_position, ANIM_KEY_BEZ_WVECT *p_key)
  {
    if(p_track != NULL && key_position >= 0 && key_position < keynum) {
      p_track[key_position] = *p_key;
      return(TRUE);
    }
    else {
      return(FALSE);
    }
  }
  
public:
  
  int keynum_get(void)
  {
    return(keynum);
  }
  
  ANIM_KEY * track_get(void)
  {
    return((ANIM_KEY *)p_track);
  }
  
private:

  WVECT * tangent(int kn, BEZIER_KEY_CLASS key_class, WVECT *p_tn);

public:

  WVECT * interpolate(WVECT *p_result, int time);
  
public:
  
  anim_track_bez_wvect(int key_num)
    : p_track(NULL), keynum(0)
  {
    if(key_num)
      create(key_num);
  }
  
  ~anim_track_bez_wvect(void)
  {
    destroy();
  }

} ANIM_TRACK_BEZ_WVECT;

#endif // __ANIMATION_TRACK_BEZIER_H__
