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
 * Bounding boxes basic class
 */

#ifndef __BOX_H__
#define __BOX_H__

typedef enum {
  
  PLANE_X,
  PLANE_Y,
  PLANE_Z
  
} PLANE_TYPE;

typedef enum box_type {
  
  BOX_NONE,
  BOX_AABB,
  BOX_OBB,
  BOX_CIRCLE
  
} BOX_TYPE;

class aabb;
typedef class aabb AABB;
  
class obb;
typedef class obb OBB;

typedef class box {

protected:  
  
  BOX_TYPE  type;
  
public:
  
  BOX_TYPE box_type_get(void)
  {
    return(type);
  }
  
public:

  virtual box * copy(void)
  {
    return(new box);
  }
  
public:
  
  virtual void set(void)  {};
  virtual void set(VECT *p_min, VECT *p_max)  {};
  virtual void set(float x, float y, float z, float dx, float dy, float dz) {};
  virtual void set_len(VECT *p_center, VECT *p_len) {};
  virtual void set_len(VECT *p_len) {};
  virtual void set(VECT *p_list, int num, tface *p_face, int facenum) {};
  virtual void set(GLMATRIX *p_mat, VECT *p_list, int num, tface *p_face, int facenum) {};
  virtual void set(box *p_box)  {};
  
public:    
    
  virtual void adjust(VECT *p_vec) {};
    
  void adjust(VECT *p_vec, int num)
  {
    int i;
    for(i = 0; i < num; i++, p_vec++)
      adjust(p_vec);
  }
    
  virtual void adjust(VECT *p_center, float radius) {};
  
public:
  
  virtual VECT * center_get(VECT *p_vec) { return(NULL); };
  virtual VECT * center_get(GLMATRIX *p_mat, VECT *p_vec) { return(NULL); };
  virtual VECT * length_get(VECT *p_len) { return(NULL); };
    
public:

  virtual bool visible(GLMATRIX *p_m) { return(FALSE); };
  virtual bool visible_full(GLMATRIX *p_m) { return(FALSE); };
  
public:
  
  virtual bool inside(VECT *p_vec) { return(FALSE); };
  virtual bool inside(GLMATRIX *p_m, VECT *p_vec) { return(FALSE); };

public:

  virtual int intersect_vector(VECT *p_a, VECT *p_b, VECT *p_intersect = NULL, float *p_t = NULL) { return(FALSE); };
  virtual int intersect_vector(GLMATRIX *p_m, VECT *p_a, VECT *p_b, VECT *p_intersect = NULL, float *p_t = NULL) { return(FALSE); };
  virtual int intersect(VECT *p_orig, VECT *p_dir, VECT *p_intersect = NULL, float *p_t = NULL) { return(FALSE); };
  virtual int intersect(GLMATRIX *p_m, VECT *p_orig, VECT *p_dir, VECT *p_intersect = NULL, float *p_t = NULL) { return(FALSE); };
    
public:
  
  virtual bool  intersect(box *p_box) { return(FALSE); }
  virtual float intersect_plane(VECT *p_a, PLANE_TYPE type) { return(0.0f); };
  virtual void  intersect_point(VECT *p_vec) {};

public:

  virtual float dist(VECT *p_vec) { return(0.0f); };
  virtual void  draw(GLMATRIX *p_m, RGBB color) {};
  virtual void  print(char *p_name = NULL) {};
  
public:
  
  virtual int   border_vertexnum_get(void) { return(0); };
  virtual VECT *border_get(VECT *p_border) { return(NULL); };

public:    

  virtual box * transformate(GLMATRIX *p_m) { return(this); };
  
public:    
    
  virtual float volume_get(void)  { return(0.0f); };

public:
  
  virtual AABB * to_aabb(AABB *p_dest) { return(NULL); };
  virtual OBB  * to_obb(OBB *p_dest) { return(NULL); };
  
public:
  
  box(BOX_TYPE btype = BOX_NONE)
  {
    type = btype;
  }
  
  virtual ~box(void)
  {
  }
  
} BOX;

BOX * box_create(BOX_TYPE type);
BOX * box_create(BOX * p_list, int num);
BOX * box_copy(BOX *p_src);
void  box_remove(BOX **p_box);

#endif // __BOX_H__
