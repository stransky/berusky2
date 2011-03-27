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
 * Axis aligned boundig boxes (AABB)
 */

#ifndef __AABB_H__
#define __AABB_H__

#include <limits.h>

#define ADJUST_PREP_FLT(min, max)  { (min) = FLT_MAX; (max) = FLT_MIN; }
#define ADJUST_PREP_INT(min, max)  { (min) = INT_MAX; (max) = INT_MIN; }

#define ADJUST_MIN(min, a)         { if((a) < (min)) (min) = (a); }
#define ADJUST_MAX(max, a)         { if((a) > (max)) (max) = (a); }
#define ADJUST(min, max, a)        { if((a) < (min)) (min) = (a); if((a) > (max)) (max) = (a); }

#define INSIDE(base, a, lenght)   (((base) <= (a)) && ((a) < (base+lenght)))
#define INSIDE_RANGE(min, a, max) (((min) <= (a)) && ((a) < (max)))

#ifndef RIF
#define RIF(min,max) (((max)+(min))*0.5f)
#endif

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MAXA
#define MAXA(a,b) (abs(a) > abs(b) ? (a) : (b))
#endif

#ifndef MAX3
#define MAX3(a,b,c) ((a) > (b) ? ((a) > (c) ? (a) : (c)) : ((b) > (c) ? (b) : (c)))
#endif
  
#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MIN3
#define MIN3(a,b,c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))
#endif

#define MIN_MAX_SORT(min_out, max_out, min_in, max_in) \
{                                                      \
  min_out = MIN(min_in, max_in);                       \
  max_out = MAX(min_in, max_in);                       \
}                                                      \
  
#define AABB_BORDER_VERTEXNUM   8

typedef class aabb : public box {

public:
  
  VECT min;
  VECT max;

public:

  virtual aabb * copy(void)
  {
    return(new aabb);
  }
  
public:

  virtual void set(void)
  {
    min.max();
    max.min();
  }

  virtual void set(VECT *p_min, VECT *p_max)
  {
    min = *p_min;
    max = *p_max;
  }
  
  virtual void set(float x, float y, float z, float dx, float dy, float dz)
  {
    min.set(x-dx,y-dy,z-dz);
    max.set(x+dx,y+dy,z+dz);
  }

  virtual void set_len(VECT *p_center, VECT *p_len)
  {
    min = *p_center - *p_len;
    max = *p_center + *p_len;
  }
  
  virtual void set_len(VECT *p_len)
  {    
    min.set(-p_len->x,-p_len->y,-p_len->z);
    max.set( p_len->x, p_len->y, p_len->z);
  }
  
  virtual void set(VECT *p_list, int num, tface *p_face, int facenum)
  {
    int      i;
    set(p_list,p_list);
    for(i = 1; i < num; i++) {
      adjust(p_list+i);
    }
  }
  
  virtual void set(GLMATRIX *p_mat, VECT *p_list, int num, tface *p_face, int facenum)
  {
    int      i;
    
    VECT t;
    matrix_transformate(p_list,p_mat,&t);
    set(&t,&t);
    
    for(i = 1; i < num; i++) {
      adjust(matrix_transformate(p_list+i,p_mat,&t));
    }    
  }
  
  virtual void set(class box *p_src);
    
  void set(aabb *p_list, int num)
  {
    int      i;
    
    *this = p_list[0];
    if(num == 1) {
      return;
    }
    for(i = 1; i < num; i++) {
      adjust(p_list+i);
    }
  }
  
public:
  
  void adjust_min(VECT *p_vec)
  {
    if(p_vec->x < min.x) min.x = p_vec->x;
    if(p_vec->y < min.y) min.y = p_vec->y;
    if(p_vec->z < min.z) min.z = p_vec->z;
  }

  void adjust_max(VECT *p_vec)
  {
    if(p_vec->x > max.x) max.x = p_vec->x;
    if(p_vec->y > max.y) max.y = p_vec->y;
    if(p_vec->z > max.z) max.z = p_vec->z;
  }

  virtual void adjust(VECT *p_vec)
  {
    if(p_vec->x < min.x) min.x = p_vec->x;
    if(p_vec->y < min.y) min.y = p_vec->y;
    if(p_vec->z < min.z) min.z = p_vec->z;
  
    if(p_vec->x > max.x) max.x = p_vec->x;
    if(p_vec->y > max.y) max.y = p_vec->y;
    if(p_vec->z > max.z) max.z = p_vec->z;
  }
  
  void adjust(VECT vec)
  {
    if(vec.x < min.x) min.x = vec.x;
    if(vec.y < min.y) min.y = vec.y;
    if(vec.z < min.z) min.z = vec.z;
  
    if(vec.x > max.x) max.x = vec.x;
    if(vec.y > max.y) max.y = vec.y;
    if(vec.z > max.z) max.z = vec.z;
  }
  
  virtual void adjust(VECT *p_center, float radius)
  {
    if(p_center->x+radius < min.x) min.x = p_center->x+radius;
    if(p_center->y+radius < min.y) min.y = p_center->y+radius;
    if(p_center->z+radius < min.z) min.z = p_center->z+radius;
  
    if(p_center->x-radius > max.x) max.x = p_center->x-radius;
    if(p_center->y-radius > max.y) max.y = p_center->y-radius;
    if(p_center->z-radius > max.z) max.z = p_center->z-radius;    
  }
  
  void adjust(class aabb *p_aabb)
  {
    adjust(&p_aabb->min);
    adjust(&p_aabb->max);
  }  
    
public:

  virtual VECT * center_get(VECT *p_vec)
  {
    p_vec->x = RIF(min.x, max.x);
    p_vec->y = RIF(min.y, max.y);
    p_vec->z = RIF(min.z, max.z);
    return(p_vec);
  }
  
  virtual VECT * center_get(GLMATRIX *p_mat, VECT *p_vec)
  {
    p_vec->x = RIF(min.x, max.x);
    p_vec->y = RIF(min.y, max.y);
    p_vec->z = RIF(min.z, max.z);
    return(matrix_transformate(p_vec,p_mat));
  }
  
  virtual VECT * length_get(VECT *p_len)
  {
    p_len->x = (max.x-min.x)*0.5f;
    p_len->y = (max.y-min.y)*0.5f;
    p_len->z = (max.z-min.z)*0.5f;
    return(p_len);
  }  

  virtual VECT * size_get(VECT *p_size)
  {
    p_size->x = (max.x-min.x);
    p_size->y = (max.y-min.y);
    p_size->z = (max.z-min.z);
    return(p_size);
  }  
  
public:  

  virtual bool visible(GLMATRIX *p_m);
  virtual bool visible_full(GLMATRIX *p_m);  

public:

  virtual bool inside(VECT *p_vec)
  {
    return(min.x <= p_vec->x && p_vec->x <= max.x &&
           min.y <= p_vec->y && p_vec->y <= max.y &&
           min.z <= p_vec->z && p_vec->z <= max.z);
  }   
  
  virtual bool inside(GLMATRIX *p_m, VECT *p_vec)
  {
    aabb tmp = *this;
    tmp.transformate(p_m);
    return(inside(p_vec));
  }

public:

  virtual int intersect_vector(VECT *p_a, VECT *p_b, VECT *p_intersect = NULL, float *p_t = NULL);

  virtual int intersect_vector(GLMATRIX *p_m, VECT *p_a, VECT *p_b, VECT *p_intersect = NULL, float *p_t = NULL)
  {
    aabb tmp = *this;
    tmp.transformate(p_m);
    return(tmp.intersect_vector(p_a,p_b,p_intersect,p_t));
  }
  
  virtual int intersect(VECT *p_orig, VECT *p_dir, VECT *p_intersect = NULL, float *p_t = NULL)
  {
    VECT p;
    p = *p_orig + *p_dir;
    return(intersect_vector(p_orig,&p,p_intersect,p_t));
  }

  virtual int intersect(GLMATRIX *p_m, VECT *p_orig, VECT *p_dir, VECT *p_intersect = NULL, float *p_t = NULL)
  {
    aabb tmp = *this;
    tmp.transformate(p_m);
    return(tmp.intersect(p_orig,p_dir,p_intersect,p_t));
  }

public:
  
  virtual bool  intersect(BOX *p_box);
  virtual float intersect_plane(VECT *p_a, PLANE_TYPE type);
  virtual void  intersect_point(VECT *p_vec);

public:

  bool  intersect(aabb *p_box);

public:

  virtual float dist(VECT *p_vec)
  {
    if(inside(p_vec))
      return(0.0f);
    /*
    VECT   v1,v2;
  
    vect_sub(p_vert,&center,&v1);
    v2 = v1;
    v2.norm_mult(&len);
    
    if(fabsf(v1.x) <= fabsf(v2.x) && 
      fabsf(v1.y) <= fabsf(v2.y) && 
      fabsf(v1.z) <= fabsf(v2.z)) {
      return(0.0f);
    } else {
      return(vect_size(v1.sub(&v2)));
    }
    */
    return(0.0f);
  }

public:
  
  virtual void draw(GLMATRIX *p_m, RGBB color);

public:

  virtual int   border_vertexnum_get(void)
  { 
    return(AABB_BORDER_VERTEXNUM); 
  }
  
  virtual VECT * border_get(VECT *p_border);

public:  
  
  void correction(void)
  {
    if(min.x > max.x) {
      float tmp = min.x;
      min.x = max.x;
      max.x = tmp;
    }
    if(min.y > max.y) {
      float tmp = min.y;
      min.y = max.y;
      max.y = tmp;
    }
    if(min.z > max.z) {
      float tmp = min.z;
      min.z = max.z;
      max.z = tmp;
    }
  }
  
public:
  
  virtual BOX * transformate(GLMATRIX *p_m)
  {
    matrix_transformate(&min,p_m);
    matrix_transformate(&max,p_m);
    correction();
    return(this);
  }

public:

  virtual float volume_get(void)
  {
    float a = max.x-min.x;
    float b = max.y-min.y;
    float c = max.z-min.z;
    return(a*a+b*b+c*c);
  }
  
public:
  
  virtual AABB * to_aabb(AABB *p_dest);
  virtual OBB  * to_obb(OBB *p_dest);
  
public:

  aabb(void)
  {
    set();
    type = BOX_AABB;
  }
  
  aabb(VECT *p_min, VECT *p_max)
  {
    set(p_min, p_max);
    type = BOX_AABB;
  }
  
  aabb(VECT *p_len)
  {
    set_len(p_len);
    type = BOX_AABB;
  }
  
  aabb(float x, float y, float z, float dx, float dy, float dz)
  {
    set(x, y, z, dx, dy, dz);
    type = BOX_AABB;
  }  
  
  aabb(class aabb *p_src)
  { 
    *this = *p_src;
    type = BOX_AABB;
  }
    
  aabb(class aabb *p_src, int num)
  { 
    type = BOX_AABB;
    int i;
    for(i = 0; i < num; i++)
      adjust(p_src+i);
  }
  
  aabb(VECT *p_list, int num, tface *p_face, int facenum)
  {
    set(p_list, num, p_face, facenum);
    type = BOX_AABB;
  }
  
  aabb(GLMATRIX *p_mat, VECT *p_list, int num, tface *p_face, int facenum)
  {
    set(p_mat, p_list, num, p_face, facenum);    
    type = BOX_AABB;
  }
    
  aabb(class box *p_src);
  
} AABB;

inline VECT * center_get(VECT *p_min, VECT *p_max, VECT *p_center)
{
  p_center->x = RIF(p_min->x, p_max->x);
  p_center->y = RIF(p_min->y, p_max->y);
  p_center->z = RIF(p_min->z, p_max->z);
  return(p_center);
}

inline VECT * length_get(VECT *p_min, VECT *p_max, VECT *p_len)
{
  p_len->x = (p_max->x-p_min->x)*0.5f;
  p_len->y = (p_max->y-p_min->y)*0.5f;
  p_len->z = (p_max->z-p_min->z)*0.5f;
  return(p_len);
}

#endif // __AABB_H__
