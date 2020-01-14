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

#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <math.h>
#include <float.h>
#include <limits.h>

// Basic vector operators
#include "vector_operators.h"

// All vector classes
struct vect2di;
typedef struct vect2di VECT2DI;
  
class vect2df;
typedef class vect2df VECT2DF;

class vect3df;
typedef class vect3df VECT3DF;

class vect3di;
typedef class vect3di VECT3DI;

struct polar2di;
typedef struct polar2di POLAR2DI;

struct polar2df;
typedef struct polar2df POLAR2DF;

struct polar3di;
typedef struct polar3di POLAR3DI;

struct polar3df;
typedef struct polar3df POLAR3DF;

struct rect2di;
typedef struct rect2di RECT2DI;

/*--------------------------------------------------------------------------
   Vector class
  --------------------------------------------------------------------------
*/

// Simple clases for parser
#ifdef __cplusplus
typedef struct vect3df_simple {

  float x,y,z;  

} VECT3DF_SIMPLE;

typedef struct rect2di_simple {

  float x,y;
  float dx,dy;

} RECT2DI_SIMPLE;
#endif

typedef struct vect2di
{
  tpos x,y;
  
  vect2di(void)
  {
  }
  
  vect2di(tpos val)
  {
    x = val; y = val;
  }
  
  vect2di(tpos x_, tpos y_)
  {
    x = x_; y = y_;
  }
  
  vect2di(VECT2DF src);
  vect2di(POLAR2DI src);
  
  vect2di(RECT2DI src);
  
  void min(void)
  {
    x = -INT_MAX; y = -INT_MAX;
  }
  
  void max(void)
  {
    x = INT_MAX; y = INT_MAX;
  }

  OPERATORS2D(VECT2DI,tpos);
  
} VECT2DI;

METHODS2D(VECT2DI,tpos);


typedef struct vect2df
{
  union { float x; float u; };
  union { float y; float v; };
  
  vect2df(void)
  {
  }
  
  vect2df(float val)
  {
    x = val; y = val;
  }
  
  vect2df(float x_, float y_)
  {
    x = x_; y = y_;
  }
  
  vect2df(VECT2DI  src);  
  vect2df(POLAR2DF src);
  
  void min(void)
  {
    x = -FLT_MAX; y = -FLT_MAX;
  }
  
  void max(void)
  {
    x = FLT_MAX; y = FLT_MAX;
  }
  
  OPERATORS2D(VECT2DF,float);
 
} VECT2DF, VECTUV;

METHODS2D(VECT2DF,float);


typedef struct vect3di
{
  tpos x,y,z;
    
  vect3di(void)
  {
    x = y = z = 0;
  }
  
  vect3di(tpos val)
  {
    x = y = z = val;
  }
  
  vect3di(tpos x_, tpos y_, tpos z_)
  {
    x = x_; y = y_; z = z_;
  }
  
  vect3di(RGBB src)
  {
    x = src.r;
    y = src.g;
    z = src.b;
  }
  
  vect3di(VECT3DF  src);
  vect3di(POLAR3DI src);
  
  void min(void)
  {
    x = -INT_MAX; y = -INT_MAX; z = -INT_MAX;
  }
  
  void max(void)
  {
    x = INT_MAX; y = INT_MAX; z = INT_MAX;
  }
  
  OPERATORS3D(VECT3DI,tpos);
  
} VECT3DI;

METHODS3D(VECT3DI,tpos);


typedef struct vect3df
{
  float x,y,z;
    
  vect3df(void)
  {
    x = y = z = 0;
  }
  
  vect3df(float val)
  {
    x = y = z = val;
  }
  
  vect3df(float x_, float y_, float z_)
  {
    x = x_; y = y_; z = z_;
  }
  
  vect3df(VECT3DI  src);
  vect3df(POLAR3DF src);
  
  vect3df(VECT3DF_SIMPLE src)
  {
    x = src.x;
    y = src.y;
    z = src.z;
  }
  
  operator VECT3DF_SIMPLE(void)
  {    
    VECT3DF_SIMPLE tmp;
    tmp.x = x;
    tmp.y = y;
    tmp.z = z;
    return(tmp);
  }  
  
  void min(void)
  {
    x = -FLT_MAX; y = -FLT_MAX; z = -FLT_MAX;
  }
  
  void max(void)
  {
    x = FLT_MAX; y = FLT_MAX; z = FLT_MAX;
  }
  
  OPERATORS3D(VECT3DF,float);
  
} VECT3DF, VECT;

METHODS3D(VECT3DF,float);

inline float dist_2D(VECT *a, VECT *b)
{
  float v  = (b->x-a->x)*(b->x-a->x)+(b->z-a->z)*(b->z-a->z);
  return((v > 0.0f) ? (float)sqrtf(v) : v);
}

inline void calc_line_2D(VECT *a, VECT *b, float *p_k, float *p_q)
{
  *p_k = (a->y - b->y)/(a->x - b->x);
  *p_q = a->y - (*p_k)*a->x;
}

inline void calc_line_3D(VECT *a, VECT *b, VECT *q) // t je parametr od 0 do 1
{
  q->x = b->x - a->x;
  q->y = b->y - a->y;
  q->z = b->z - a->z;
}

inline float vect_norm_vect_2D(float *p_x, float *p_y)
{
 float vel = (float)sqrtf((*p_x)*(*p_x) + (*p_y)*(*p_y));
 *p_x /= vel;
 *p_y /= vel;
 return(vel);
}

/*--------------------------------------------------------------------------
   4D vector
  --------------------------------------------------------------------------
*/
typedef struct vect4df : public vect3df {

  float     w;

  vect4df(void)
  {
  }

  vect4df(RGBAF &src)
  {
    x = src.r;
    y = src.g;
    z = src.b;
    w = src.a;
  }

  operator RGBAF(void)
  { 
    return(RGBAF(x,y,z,w));
  }
  
  static vect4df * sub(vect4df *v, vect4df *u, vect4df *c)
  {
    c->x = v->x - u->x;
    c->y = v->y - u->y;
    c->z = v->z - u->z;
    c->w = v->w - u->w;
    return(c);
  }

} VECT4DF, WVECT;

inline vect4df * wvect_sub(vect4df *v, vect4df *u, vect4df *c)
{
  c->x = v->x - u->x;
  c->y = v->y - u->y;
  c->z = v->z - u->z;
  c->w = v->w - u->w;
  return(c);
}

/*--------------------------------------------------------------------------
   2D Polar vectors / coordinates
  --------------------------------------------------------------------------
*/
typedef struct polar2di
{
  tpos  length;
  float rotation;
  
  polar2di(void)
  {
  }
    
  polar2di(POLAR2DF src);
  polar2di(VECT2DI  src);
  
  polar2di(tpos length, float rotation)
  {
    this->length = length;
    this->rotation = rotation;
  }
  
  void min(void)
  {
    length = 0;
    rotation = 0;
  }
  
  void max(void)
  {
    length = INT_MAX;
    rotation = 0;
  }  
  
  // Normalize angle to <0,2PI> radius
  void norm(void);
  
} POLAR2DI;

POLAR2DI interpolate(POLAR2DI p1, POLAR2DI p2, float i);
POLAR2DI interpolate(POLAR2DI p1, POLAR2DI p2, float i1, float i2);

typedef struct polar2df
{
  float length;
  float rotation;
  
  polar2df(void)
  {
  }
    
  polar2df(POLAR2DI src);
  polar2df(VECT2DF  src);
  
  polar2df(float length, float rotation)
  {
    this->length = length;
    this->rotation = rotation;
  }
  
  void min(void)
  {
    length = 0;
    rotation = 0;
  }
  
  void max(void)
  {
    length = FLT_MAX;
    rotation = 0;
  }  
  
} POLAR2DF;

/*--------------------------------------------------------------------------
   3D Polar vectors / coordinates
  --------------------------------------------------------------------------
*/
typedef struct polar3di
{
  tpos  length;
  float rotation;
  float elevation;
  
  polar3di(void)
  {
  }
    
  polar3di(tpos length, float rotation, float elevation)
  {
    this->length = length;
    this->rotation = rotation;
    this->rotation = elevation;
  }
  
  polar3di(POLAR3DF src);
  polar3di(VECT3DI  src);
  
  void min(void)
  {
    length = 0;
    rotation = 0;
    elevation = 0;
  }
  
  void max(void)
  {
    length = INT_MAX;
    rotation = 0;
    elevation = 0;
  }  
  
} POLAR3DI;

typedef struct polar3df
{
  float length;
  float rotation;
  float elevation;
  
  polar3df(void)
  {
  }
    
  polar3df(float length, float rotation, float elevation)
  {
    this->length = length;
    this->rotation = rotation;
    this->rotation = elevation;
  }
  
  polar3df(POLAR3DI src);
  polar3df(VECT3DF  src);
  
  void min(void)
  {
    length = 0;
    rotation = 0;
    elevation = 0;
  }
  
  void max(void)
  {
    length = FLT_MAX;
    rotation = 0;
    elevation = 0;
  }  
  
} POLAR3DF;

/*--------------------------------------------------------------------------
   Rectangle classes
  --------------------------------------------------------------------------
*/
typedef struct rect2di
{
  tpos x,y;
  tpos dx,dy;

  rect2di(void)
  {
  }

  rect2di(int val)
  {
    set(val);
  }
  
  rect2di(tpos x, tpos y, tpos dx = 1, tpos dy = 1)
  {
    this->x = x;
    this->y = y;
    this->dx = dx;
    this->dy = dy;
  }
  
  rect2di(VECT2DI v1)
  {
    x = v1.x;
    y = v1.y;
    dx = 1;
    dy = 1;
  }
  
  rect2di(VECT2DI v1, VECT2DI v2)
  {
    set(&v1,&v2);
  }
  
  rect2di(RECT2DI_SIMPLE src)
  {
    x = src.x;
    y = src.y;
    dx = src.dx;
    dy = src.dy;
  }
  
  operator RECT2DI_SIMPLE(void)
  {
    RECT2DI_SIMPLE tmp;
    tmp.x = x;
    tmp.y = y;
    tmp.dx = dx;
    tmp.dy = dy;
    return(tmp);
  }  
  
  void set(tpos val = 0)
  {
    x = y = val;
    dx = dy = val;
  }
  
  void set(VECT2DI *p_min, VECT2DI *p_max)
  {
    x = p_min->x;
    y = p_min->y;
    
    dx = p_max->x-p_min->x;
    dy = p_max->y-p_min->y;
  }
  
  void set(VECT2DI *p_vect, int num);
  
  void get(VECT2DI *p_min, VECT2DI *p_max)
  {
    p_min->x = x;
    p_min->y = y;
    
    p_max->x = dx + p_min->x;
    p_max->y = dy + p_min->y;
  }
  
  void get(VECT2DI *p_vect)
  {
    p_vect[0].set(x   ,y);
    p_vect[1].set(x+dx,y);
    p_vect[2].set(x   ,y+dy);
    p_vect[3].set(x+dx,y+dy);
  }

  VECT2DI center_get(void)
  {
    return(VECT2DI(x+dx/2,y+dy/2));
  }
  
  VECT2DI center_size_get(void)
  {
    return(VECT2DI(dx/2,dy/2));
  }

  VECT2DI start_get(void)
  {
    return(VECT2DI(x,y));
  }
  
  VECT2DI end_get(void)
  {
    return(VECT2DI(x+dx,y+dy));
  }

  VECT2DI size_get(void)
  {
    return(VECT2DI(dx,dy));
  }
  void size_set(VECT2DI size)
  {
    dx = size.x;
    dy = size.y;
  }

  void extend(tpos ex, tpos ey)
  {
    if(ex < x) {
      dx += x-ex;
      x = ex;
    }
    if(ey < y) {
      dy += y-ey;
      y = ey;
    }    
    if(ex > x+dx) {
      dx = ex-x+1;
    }
    if(ey > y+dy) {
      dy = ey-y+1;
    }
  }
  
  void extend(tpos size)
  {
    x -= size;
    y -= size;
    
    size *= 2;
    
    dx += size;
    dy += size;
  }
  
} RECT2DI;

typedef struct rect3di
{
  tpos x,y,z;
  tpos dx,dy,dz;
  
  rect3di(void)
  {    
  }
  
  rect3di(tpos val)
  {    
    set(val);
  }
  
  rect3di(VECT3DI v1)
  {
    x = v1.x;
    y = v1.y;
    z = v1.z;
    
    dx = 1;
    dy = 1;
    dz = 1;
  }
  
  rect3di(VECT3DI v1, VECT3DI v2)
  {
    x = v1.x;
    y = v1.y;
    z = v1.z;
    
    dx = v2.x;
    dy = v2.y;
    dz = v2.z;
  }
  
  rect3di(tpos x_, tpos y_, tpos z_, 
          tpos dx_ = 1, tpos dy_ = 1, tpos dz_ = 1)
  {
    x = x_;
    y = y_;
    z = z_;
    
    dx = dx_;
    dy = dy_;
    dz = dz_;
  }
  
  void set(tpos val = 0)
  {
    x = y = z = val;
    dx = dy = dz = val;
  }  
  
  void set(VECT3DI *p_min, VECT3DI *p_max)
  {
    x = p_min->x;
    y = p_min->y;
    z = p_min->z;
    
    dx = p_max->x-p_min->x;
    dy = p_max->y-p_min->y;
    dz = p_max->z-p_min->z;
  }
  
  void get(VECT3DI *p_min, VECT3DI *p_max)
  {
    p_min->x = x;
    p_min->y = y;
    p_min->z = z;
    
    p_max->x = dx + p_min->x;
    p_max->y = dy + p_min->y;
    p_max->z = dz + p_min->z;
  }
  
  VECT3DI center_get(void)
  {
    return(VECT3DI(x+dx/2, y+dy/2, z+dz/2));
  }
  
  VECT3DI center_size_get(void)
  {
    return(VECT3DI(dx/2, dy/2, dz/2));
  }
  
} RECT3DI;

typedef struct rect3df
{
  float x,y,z;
  float dx,dy,dz;

  rect3df(void)
  {    
  }
  
  rect3df(tpos val)
  {    
    set(val);
  }
  
  rect3df(VECT3DF v1);
  rect3df(VECT3DF v1, VECT3DF v2);
  
  rect3df(float x_, float y_, float z_,
          float dx_ = 1, float dy_ = 1, float dz_ = 1)
  {
    x = x_;
    y = y_;
    z = z_;
    
    dx = dx_;
    dy = dy_;
    dz = dz_;
  }
  
  void set(float val = 0)
  {
    x = y = z = val;
    dx = dy = dz = val;
  }  
  
  VECT3DF center_get(void)
  {
    return(VECT3DF(x+dx/2, y+dy/2, z+dz/2));
  }
  
  VECT3DF center_size_get(void)
  {
    return(VECT3DF(dx/2, dy/2, dz/2));
  }
  
} RECT3DF;

/*--------------------------------------------------------------------------
   Plane
  --------------------------------------------------------------------------
*/
typedef class plane : public vect3df {

public:
  
  float e;

public:

  float norm(void)
  {
    float vel = (float)sqrtf(x*x + y*y + z*z);
    if(vel == 0.0f) return(0.0f);//vel = 1;
    vel = 1.0f/vel;
    x *= vel;
    y *= vel;
    z *= vel;
    e *= vel;
    return(vel);
  }  
  
  float dist_to_point(VECT3DF *p)
  {
    return((float)fabs(x*p->x+y*p->y+z*p->z+e));
  }
  
  float dist_to_point_nabs(VECT3DF *p)
  {
    return(x*p->x+y*p->y+z*p->z+e);
  }

  bool intersection(VECT3DF *a, VECT3DF *b, VECT3DF *p_p, float *p_t)
  {
    #define TOL 0.001
    
    float t;
    float c,j;
    float qx,qy,qz;
    
    // q = B-A
    qx = b->x - a->x;
    qy = b->y - a->y;
    qz = b->z - a->z;
    
    c = -(x*a->x + y*a->y + z*a->z + e);
    j = x*qx + y*qy + z*qz;
    
    // is line parallel with the plane?
    if((j < TOL)&&(j > -TOL)) {
      return(FALSE);
    }
    t = c/j;
    
    // P = A - qt
    if(p_p) {
      p_p->x = a->x + qx*t;
      p_p->y = a->y + qy*t;
      p_p->z = a->z + qz*t;
    }
    
    if(p_t) {
      *p_t = t;
    }
    
    return(TRUE);
  }
  
public:

  plane(void) {}

  plane(float x_, float y_, float z_, float e_) 
  {
    x = x_;
    y = y_;
    z = z_;
    e = e_;
  }
  
  plane(VECT3DF a, VECT3DF b, VECT3DF c)
  {
    x = (b.y - a.y)*(c.z - a.z)-(c.y - a.y)*(b.z - a.z);
    y = (b.z - a.z)*(c.x - a.x)-(c.z - a.z)*(b.x - a.x);
    z = (b.x - a.x)*(c.y - a.y)-(c.x - a.x)*(b.y - a.y);
    vect3df::norm();
    e = -(x*a.x + y*a.y + z*a.z);
  }
  
  plane(VECT3DF position, VECT3DF normal)
  {
    x = normal.x;
    y = normal.y;
    z = normal.z;
    e = - x*position.x - y*position.y - z*position.z;
  }  
   
} PLANE;

/*
 * Face helpers
 */
inline VECT * face_normal(VECT *a, VECT *b, VECT *c, VECT *normal)
{   
 normal->x = (b->y - a->y)*(c->z - a->z)-(c->y - a->y)*(b->z - a->z);
 normal->y = (b->z - a->z)*(c->x - a->x)-(c->z - a->z)*(b->x - a->x);
 normal->z = (b->x - a->x)*(c->y - a->y)-(c->x - a->x)*(b->y - a->y);
 normal->norm();
 return(normal);
}   

inline VECT * face_center(VECT *a, VECT *b, VECT *c, VECT *center)
{   
 center->x = (a->x + b->x + c->x)/3.0f;
 center->y = (a->y + b->y + c->y)/3.0f;
 center->z = (a->z + b->z + c->z)/3.0f;
 return(center);
}

inline tpos square_dist(tpos x1, tpos y1, tpos x2, tpos y2)
{
  tpos d1 = abs(x2-x1);
  tpos d2 = abs(y2-y1);
  return(d1 > d2 ? d1 : d2);
}

/*
 * Math helpers
 */
MATH_HELPERS(float);

#endif //__VECTOR_H__
