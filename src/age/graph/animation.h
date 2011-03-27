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
 * Animation engine
 */

#ifndef __ANIMATION_H__
#define __ANIMATION_H__

/*
  Structure of animation:
  -----------------------

  -------------------
  | animation_track |
  -------------------
          ||
          \/
  -------------------
  |    animation    |   -->  GLMATRIX
  -------------------
          /\
          ||
  -------------------
  |  animation_time |
  -------------------
  
  
  Animation sequence:
  
    - update(time)
    - get
    
    or
    
    - update(time)
    - loop over all tracks
*/

/*
 * Animation templates - generic animations
 */
 
typedef class animation_template :  
  public object_list
{
  
  int lenght;
  
public:

  int length_get(void)
  {
    return(lenght);
  }

  /*
   * Attached tracks
   */
private:
  
  ANIM_TRACK_HEAD tracks;
  
public:  
  
  ANIM_TRACK * track_new(ANIM_TYPE type);

public:  

  ANIM_TRACK * track_first_get(void);  

public:  
  
  void update(int time);

public:  
  
  GLMATRIX * get(GLMATRIX *p_result);

public:
  
  animation_template(int time_lenght)
    : object_list(NULL)
  {
  }
  
  ~animation_template(void)
  {
  }
  
} ANIMATION_TEMPLATE;

typedef class animation_template_head 
  : public object_list_head
{
  
public:

  animation_template_head(void)
    : object_list_head(NULL)
  {
  }
} ANIMATION_TEMPLATE_HEAD;

/*
 * ANIMATION_TEMPLATE - handle to animation template
 */
typedef ANIMATION_TEMPLATE * ANIMATION_TEMPLATE_HANDLE;

/*
 * Animated object - reference to object/matrix what will be animated
 */
typedef class animated_object :
  public llist_item
{
  
  /*
   * Matrix what will be animated
   */
  MATRIX_NOTIFIED         *p_mat;
  
public:  
  
  animated_object(void)
    : p_mat(NULL)
  {
  }
  
} ANIMATED_OBJECT;

typedef struct animated_object_head : 
  public llist_head 
{
  
} ANIMATED_OBJECT_HEAD;


/* Animation flags
  
  Animation features:

    - 

    - autoremove after end
    - callback after end
    - run next animation after end
 */

#define ANIMATION_AUTOREMOVE  (1<<0)
#define ANIMATION_CHAIN       (1<<0)


/*
 * Runtime animation object
 */
typedef class animation_runtime :   
  public flag_interface,
  public object_list
{
  
  /*
   * Template what defines the animation
   */
  ANIMATION_TEMPLATE    *p_template;
  
  /*
   * Time in animation (and it's current state)
   */
  ANIMATION_TRACK_TIME   atime;
  
  /*
   * List of affected objects
   */
  ANIMATED_OBJECT_HEAD   objects;  
  
public:

  animation_runtime(ANIMATION_TEMPLATE *p_template);
  ~animation_runtime(void);
  
} ANIMATION_RUNTIME;

typedef class animation_runtime_head
  : public object_list_head
{

public:

  animation_runtime_head(void)
  : object_list_head(NULL)
  {
  }
  
} ANIMATION_RUNTIME_HEAD;

/*
 * ANIMATION_RUNTIME_HANDLE - handle to runtime animation
 */
typedef ANIMATION_RUNTIME * ANIMATION_RUNTIME_HANDLE;

#endif //  __ANIMATION_H__
