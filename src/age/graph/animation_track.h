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

#ifndef __ANIMATION_TRACK_H__
#define __ANIMATION_TRACK_H__

#include <values.h>

/*
  The default animation rate for linear animation is:

    20 frames/keys per sec
    1 frame = 1 key
    framenum = keynum
    delays between frames is 50ms
*/

#define LINEAR_FRAMES_DELAY       50.0f   // Delay between frames (in ms)
#define LINEAR_FRAMES_DELAYI      50

#define LINEAR_FRAMES_PER_SEC     20
#define LINEAR_LENGTH_SEC         1000

#define DELTA_INTERVAL            0.01f

/*
 * Animation types
 */
typedef enum {
  
  LINEAR_VECT,
  LINEAR_QUAT,
  
  BEZIER_VECT,
  BEZIER_QUAT
  
} ANIM_TYPE;

/*
 * Basic class for all animation keys
 */
typedef struct anim_key {
  
  int time;

public:
  
  int time_get(void)
  {
    return(time);
  }
  
  void time_set(int ktime)
  {
    time = ktime;
  }
  
} ANIM_KEY;

/*
 * A Class for all animation track
 */
typedef enum {
  
  KEYS_ALL = 0, // use 2 keys + time
  KEYS_ONE      // use only one key
  
} KEY_TYPE;

#define   NO_KEY      (-1)

/*
 * Flags of animation tracks
 */

#define   FLAG_TRACK_LOOP   (1<<0)

/*
 * Basic class for all animation track
 */
typedef class anim_track : 
  public flag_interface,
  public llist_item
{

  ANIM_TYPE type;
  
public:

  ANIM_TYPE type_get(void)
  {
    return(type);
  }

public:

  bool                looped(void) { return(flag_get(FLAG_TRACK_LOOP)); }
  void                loop_set(bool loop_animation) { flag_set(FLAG_TRACK_LOOP,loop_animation); }
  
  /*
   * Total length of the animation
   */
private:
  
  int                 time_length;
  
public:
  
  int                 length_get(void)  { return(time_length); }
  void                length_set(int time) { time_length = time; }

  int                 framenum_get(void) { return(time_length/LINEAR_FRAMES_DELAYI); }
  void                framenum_set(int frames) { time_length = LINEAR_FRAMES_DELAYI*frames; }

public:

  virtual void        destroy(void) = 0;
  virtual void        create(int key_num) = 0;
  
  /*
   * insert and get key to/from specified position
   * if p_key == NULL clear that key
   */
public:

  virtual ANIM_KEY *  key_get(int key_position) = 0;
  bool                key_set(int key_position, ANIM_KEY *p_key) { return(FALSE); };

public:
  
  virtual int         keynum_get(void) = 0;
  virtual ANIM_KEY *  track_get(void) = 0;

public:

  virtual int         key_get_internal(int time) { return(0); }
  virtual int         key_get_time(int time) { return(0); }
  virtual int         key_get_frame(int frame) { return(0); }
  virtual KEY_TYPE    key_get_time(int time, int *p_kn, int *p_kn1, float *p_time) { return(KEYS_ALL); }
  
public:

  virtual void        update(int time) {}
  virtual GLMATRIX *  get(GLMATRIX *p_result) { return(NULL); }

public:
  
  anim_track(void) {};
  virtual ~anim_track(void) {};
  
} ANIM_TRACK;

typedef class anim_track_head : public llist_head {} ANIM_TRACK_HEAD;

#endif // __ANIMATION_TRACK_H__
