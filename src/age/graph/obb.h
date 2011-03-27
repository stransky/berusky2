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
 * Oriented Bounding Bores and Trees
 */

#ifndef __OBB_H__
#define __OBB_H__

class obb;
typedef class obb OBB;

/*
 * OBB envelope
 *
 * Precalculated bounding points
 *
 *
 */
typedef class obb : public box {

public:  
  
  VECT3DF         dir[3];      // OBB direction vectors
  VECT3DF         center;      // Center of the OBB
  VECT3DF         len;         // Lengths

public:  
  
  virtual obb * copy(void)
  {
    return(new obb);
  }
  
public:
  
  virtual void set(void)
  {
    center.set(0,0,0);
    
    dir[0].set(1,0,0);
    dir[1].set(0,1,0);
    dir[2].set(0,0,1);
    
    len.set(1,1,1);
  }
  
  virtual void set(VECT *p_min, VECT *p_max)
  { 
    ::center_get(p_min,p_max,&center);
    ::length_get(p_min,p_max,&len);
    
    dir[0].set(1,0,0);
    dir[1].set(0,1,0);
    dir[2].set(0,0,1);
  }
  
  virtual void set(float x, float y, float z, float dx, float dy, float dz)
  {    
  }
  
  virtual void set_len(VECT *p_center, VECT *p_len)
  {
  }
  
  virtual void set_len(VECT *p_len)
  {
  }  
  
  void set_most_precise(VECT *p_vertex, int vertexnum, tface *p_face, int facenum);
  void set_precise(VECT *p_vertex, int vertexnum, tface *p_face, int facenum);
  void set_fast(VECT *p_vertex, int vertexnum, tface *p_face, int facenum);
  
  virtual void set(VECT *p_vertex, int vertexnum, tface *p_face, int facenum);
  virtual void set(GLMATRIX *p_mat, VECT *p_vertex, int vertexnum, tface *p_face, int facenum);
  
  virtual void set(class box *p_src);
    
public:
  
  void set(VECT *p_center, VECT **p_dir, VECT *p_len)
  {
    center = *p_center;
    
    dir[0] = *p_dir[0];
    dir[1] = *p_dir[1];
    dir[2] = *p_dir[2];
    
    len = *p_len;
  }

  void set(OBB *p_list, int num);
  
public:

  virtual void adjust(VECT *p_vec);
  virtual void adjust(VECT *p_center, float radius);
 
public:
  
  void adjust_len(VECT *p_vec);
  void adjust(class obb *p_obb);
  
public:
  
  virtual VECT * center_get(VECT *p_vec)
  {
    *p_vec = center;
    return(p_vec);
  }
  
  virtual VECT * center_get(GLMATRIX *p_mat, VECT *p_vec)
  { 
    *p_vec = center;
    return(matrix_transformate(p_vec,p_mat));
  }
  
  virtual VECT * length_get(VECT *p_len)
  {
    *p_len = len;
    return(p_len);
  }
  
public:
  
  virtual bool visible(GLMATRIX *p_m);
  virtual bool visible_full(GLMATRIX *p_m);
  
public:
  
  virtual bool inside(VECT *p_vert)
  {
    VECT   v,v1;
    
    v1 = *p_vert - center;
    
    v.x = vect_dot_product(dir[0],v1);
    v.y = vect_dot_product(dir[1],v1);
    v.z = vect_dot_product(dir[2],v1);
       
    if(fabsf(v.x) <= fabsf(len.x) && 
      fabsf(v.y) <= fabsf(len.y) && 
      fabsf(v.z) <= fabsf(len.z))
      return(TRUE);
    else
      return(FALSE);
  }
  
  virtual bool inside(GLMATRIX *p_m, VECT *p_vec)
  {
    obb tmp = *this;
    tmp.transformate(p_m);
    return(inside(p_vec));    
  }

public:
  
  virtual int intersect_vector(VECT *p_a, VECT *p_b, VECT *p_intersect = NULL, float *p_t = NULL);

  virtual int intersect_vector(GLMATRIX *p_m, VECT *p_a, VECT *p_b, VECT *p_intersect = NULL, float *p_t = NULL)
  {
    obb tmp = *this;
    tmp.transformate(p_m);
    return(tmp.intersect_vector(p_a,p_b,p_intersect,p_t));
  }
  
  virtual int intersect(VECT *p_orig, VECT *p_dir, VECT *p_intersect = NULL, float *p_t = NULL);

  virtual int intersect(GLMATRIX *p_m, VECT *p_orig, VECT *p_dir, VECT *p_intersect = NULL, float *p_t = NULL)
  {
    obb tmp = *this;
    tmp.transformate(p_m);
    return(tmp.intersect(p_orig,p_dir,p_intersect,p_t));
  }

public:

  static bool intersect(OBB *p_obb1, OBB *p_obb2);
  
public:

  bool intersect(OBB *p_obb)
  {
    return(intersect(this,p_obb));
  }
  
public:

  virtual bool  intersect(BOX *p_box);
  virtual float intersect_plane(VECT *p_a, PLANE_TYPE type) { return(0.0f); };
  virtual void  intersect_point(VECT *p_vec) {};

public:
  
  float dist(VECT *p_vert)
  {
    VECT  v,v1,v2;
    float tmp_len;
    
    v1 = *p_vert - center;
    
    v.x = vect_dot_product(dir[0],v1);
    v.y = vect_dot_product(dir[1],v1);
    v.z = vect_dot_product(dir[2],v1);
    
    v2 = v1;
    tmp_len = v2.norm();
    v2 *= len;
    
    if(fabsf(v1.x) <= fabsf(v2.x) &&
      fabsf(v1.y) <= fabsf(v2.y) &&
      fabsf(v1.z) <= fabsf(v2.z)) {
      return(0.0f);
    } else {    
      v1 -= v2;
      return(v1.size());
    }
  }

public:
  
  void draw(GLMATRIX *p_m, RGBB color);

public:
  
  void print(char *p_name = NULL);

private:

  static void transformate(class obb *p_src, GLMATRIX *p_m, class obb *p_dest);  
    
public:
  
  BOX * transformate(GLMATRIX *p_m);
    
public:
    
  virtual AABB * to_aabb(AABB *p_dest);
  virtual OBB  * to_obb(OBB *p_dest);
  
public:

/*
  float dist_plane_x(float x)
  {
    float min = aabb_get()->min.x,
          max = aabb_get()->max.x;
    float dist1,dist2;
    
    if(min <= x && x <= max)
      return(0.0f);
    else {
      dist1 = fabsf(min-x);
      dist2 = fabsf(max-x);
      return(dist1 > dist2 ? dist1 : dist2);
    }
  }
  
  float dist_plane_y(float y)
  {
    float min = aabb_get()->min.y,
          max = aabb_get()->max.y;
    float dist1,dist2;
    
    if(min <= y && y <= max)
      return(0.0f);
    else {
      dist1 = fabsf(min-y);
      dist2 = fabsf(max-y);
      return(dist1 > dist2 ? dist1 : dist2);
    }
  }
  
  float dist_plane_z(float z)
  {
    float min = aabb_get()->min.z,
          max = aabb_get()->max.z;
    float dist1,dist2;
    
    if(min <= z && z <= max)
      return(0.0f);
    else {
      dist1 = fabsf(min-z);
      dist2 = fabsf(max-z);
      return(dist1 > dist2 ? dist1 : dist2);
    }
  }
*/
  
public:
  
#define  OBB_BORDER_VERTEXNUM   8
  VECT   border[OBB_BORDER_VERTEXNUM];

public:
  
  virtual int  border_vertexnum_get(void)
  { 
    return(OBB_BORDER_VERTEXNUM); 
  }
  
  virtual VECT * border_get(VECT *p_border);

private:
   
  void   border_update(void)
  {
    border_get(border);
  }
  
public:

  // transformates World -> OBB
  GLMATRIX * matrix_to_obb_get(GLMATRIX *p_mat)
  {
    p_mat->rotation_set(dir,dir+1,dir+2);
    p_mat->translation_set(-center.x,-center.y,-center.z);
    return(p_mat);
  }
  
  // transformates OBB -> World
  GLMATRIX * matrix_to_world_get(GLMATRIX *p_mat)
  {  
    matrix_to_obb_get(p_mat);
    return(p_mat->invert());
  }

public:
  
  obb(void)
  {
    type = BOX_OBB;
  }
  
  obb(class obb *p_src)
  {
    *this = *p_src;
  }

  /*
   * Create an obb from one and more obb's
   */
  obb(class obb *p_list, int num)
  {
    type = BOX_OBB;    
    set(p_list, num);
  }
  
  obb(class box *p_src);
  
} OBB;

/* OBB Envelope tree - an envelope
*/
typedef class obb_tree_item {

  OBB              *p_obb;
  void             *p_item;
  int               viditelny;
  int               zrc_viditelny;


} OBB_TREE_ITEM;

/* OBB Envelope tree
*/
typedef class obb_tree {

  int                mail;  
  OBB                obb;

  int                itnum;     // polozky stromu
  OBB_TREE_ITEM     *p_item;

  class obb_tree    *p_up;      // otec stromu
  int                nextnum;   // pocet podobalek
  class obb_tree    *p_next;    // podobalky

public:
  
  //void obb_tree::kresli(dword barva);

} OBB_TREE;

#endif // __OBB_H__
