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
 * Check visibility of this cube.  
 */
 
/*
 * Is at least one point of this cube visible?
 */
bool aabb::visible(GLMATRIX *p_m)
{
 int i,j,xplane,yplane,xa,ya,m;
 float x,y,z,xt,yt,zt,w,fx,fy,fw;

 // ... // -> vyrobeni 8 hranicnich bodu krychle  
 xplane = 0; // -2, -1, 1, 2
 yplane = 0; // -2, -1, 1, 2

 for(i = 0, j = 0; i < AABB_BORDER_VERTEXNUM; i++) {
   xt = (i & 0x1) ? max.x : min.x;
   yt = (i & 0x2) ? max.y : min.y;
   zt = (i & 0x4) ? max.z : min.z;
   
   x = p_m->_11*xt + p_m->_21*yt + p_m->_31*zt + p_m->_41;
   y = p_m->_12*xt + p_m->_22*yt + p_m->_32*zt + p_m->_42;
   z = p_m->_13*xt + p_m->_23*yt + p_m->_33*zt + p_m->_43;   
   w = p_m->_14*xt + p_m->_24*yt + p_m->_34*zt + p_m->_44;
     
   // bod je pred kamerou
   m = (-w <= z && z <= w);
   if(m)
     j++;

   // bod lezi v obrazovce -> automaticky uspech
   if(m && -w <= x && x <= w && -w <= y && y <= w) {
     return(TRUE);
   }
   
   fx = (float)fabs(x);
   fy = (float)fabs(y);
   fw = (float)fabs(w);
      
   if(fx/fw > 1.0f) {
     xa = (x < 0.0f) ? -2 : 2;
   } else {
     xa = (x < 0.0f) ? -1 : 1;
   }
   
   if(fy/fw > 1.0f) {
     ya = (y < 0.0f) ? -2 : 2;
   } else {
     ya = (y < 0.0f) ? -1 : 1;
   }
      
   xplane += xa;
   yplane += ya;
 }
// if(!j)
  // dbgprintf("j = 0");
 return((j > 0) ? ((abs(xplane) != 16) && (abs(yplane) != 16)) : FALSE);
}

/*
 * Are all 8 points of the cube visible?
 */
bool aabb::visible_full(GLMATRIX *p_m)
{
 int i,j,xplane,yplane,xa,ya,m;
 int hits = 0;
 float x,y,z,xt,yt,zt,w,fx,fy,fw;

 // ... // -> vyrobeni 8 hranicnich bodu krychle  
 xplane = 0; // -2, -1, 1, 2
 yplane = 0; // -2, -1, 1, 2

 for(i = 0, j = 0; i < AABB_BORDER_VERTEXNUM; i++) {
   xt = (i & 0x1) ? max.x : min.x;
   yt = (i & 0x2) ? max.y : min.y;
   zt = (i & 0x4) ? max.z : min.z;
   
   x = p_m->_11*xt + p_m->_21*yt + p_m->_31*zt + p_m->_41;
   y = p_m->_12*xt + p_m->_22*yt + p_m->_32*zt + p_m->_42;
   z = p_m->_13*xt + p_m->_23*yt + p_m->_33*zt + p_m->_43;   
   w = p_m->_14*xt + p_m->_24*yt + p_m->_34*zt + p_m->_44;
     
   // bod je pred kamerou
   m = (-w <= z && z <= w);
   if(m)
     j++;

   // bod lezi v obrazovce -> automaticky uspech
   if(m && -w <= x && x <= w && -w <= y && y <= w) {
     hits++;
     continue;
   }
   
   fx = (float)fabs(x);
   fy = (float)fabs(y);
   fw = (float)fabs(w);

   if(fx/fw > 1.0f) {
     xa = (x < 0.0f) ? -2 : 2;
   } else {
     xa = (x < 0.0f) ? -1 : 1;
   }
   
   if(fy/fw > 1.0f) {
     ya = (y < 0.0f) ? -2 : 2;
   } else {
     ya = (y < 0.0f) ? -1 : 1;
   }
      
   xplane += xa;
   yplane += ya;
 }
// if(!j)
  // dbgprintf("j = 0");
 return((hits == AABB_BORDER_VERTEXNUM) ? hits : (j > 0) ? ((abs(xplane) != 16) && (abs(yplane) != 16)) : FALSE);
}

void aabb::draw(GLMATRIX *p_m, RGBB color)
{
  VECT3DF center,points[AABB_BORDER_VERTEXNUM*6];
  float dx, dy, dz;
  int   i;

  center = max - min;
  center *= 0.05f;
  min -= center;
  max -= center;

  dx = max.x - min.x, dy = max.y - min.y, dz = max.z - min.z;
  dx *= 0.3f; dy *= 0.3f; dz *= 0.3f;

  points[(0*6)].set(min.x   ,min.y   ,min.z);
  points[(0*6+1)].set(min.x+dx,min.y   ,min.z);
  points[(0*6+2)].set(min.x   ,min.y   ,min.z);
  points[(0*6+3)].set(min.x   ,min.y+dy,min.z);
  points[(0*6+4)].set(min.x   ,min.y   ,min.z);
  points[(0*6+5)].set(min.x   ,min.y   ,min.z+dz);
  
  points[(1*6)].set(max.x   ,min.y   ,min.z);
  points[(1*6+1)].set(max.x-dx,min.y   ,min.z);
  points[(1*6+2)].set(max.x   ,min.y   ,min.z);
  points[(1*6+3)].set(max.x   ,min.y+dy,min.z);
  points[(1*6+4)].set(max.x   ,min.y   ,min.z);
  points[(1*6+5)].set(max.x   ,min.y   ,min.z+dz);
  
  points[(2*6)].set(min.x   ,min.y   ,max.z);
  points[(2*6+1)].set(min.x+dx,min.y   ,max.z);
  points[(2*6+2)].set(min.x   ,min.y   ,max.z);
  points[(2*6+3)].set(min.x   ,min.y+dy,max.z);
  points[(2*6+4)].set(min.x   ,min.y   ,max.z);
  points[(2*6+5)].set(min.x   ,min.y   ,max.z-dz);
  
  points[(3*6)].set(  max.x   ,min.y   ,max.z);
  points[(3*6+1)].set(max.x-dx,min.y   ,max.z);
  points[(3*6+2)].set(max.x   ,min.y   ,max.z);
  points[(3*6+3)].set(max.x   ,min.y+dy,max.z);
  points[(3*6+4)].set(max.x   ,min.y   ,max.z);
  points[(3*6+5)].set(max.x   ,min.y   ,max.z-dz);
  
  points[(4*6)].set(min.x   ,max.y   ,min.z);
  points[(4*6+1)].set(min.x+dx,max.y   ,min.z);
  points[(4*6+2)].set(min.x   ,max.y   ,min.z);
  points[(4*6+3)].set(min.x   ,max.y-dy,min.z);
  points[(4*6+4)].set(min.x   ,max.y   ,min.z);
  points[(4*6+5)].set(min.x   ,max.y   ,min.z+dz);
  
  points[(5*6)].set(  max.x   ,max.y   ,min.z);
  points[(5*6+1)].set(max.x-dx,max.y   ,min.z);
  points[(5*6+2)].set(max.x   ,max.y   ,min.z);
  points[(5*6+3)].set(max.x   ,max.y-dy,min.z);
  points[(5*6+4)].set(max.x   ,max.y   ,min.z);
  points[(5*6+5)].set(max.x   ,max.y   ,min.z+dz);
  
  points[(6*6)].set(  min.x   ,max.y   ,max.z);
  points[(6*6+1)].set(min.x+dx,max.y   ,max.z);
  points[(6*6+2)].set(min.x   ,max.y   ,max.z);
  points[(6*6+3)].set(min.x   ,max.y-dy,max.z);
  points[(6*6+4)].set(min.x   ,max.y   ,max.z);
  points[(6*6+5)].set(min.x   ,max.y   ,max.z-dz);
  
  points[(7*6)].set(max.x   ,max.y   ,max.z);
  points[(7*6+1)].set(max.x-dx,max.y   ,max.z);
  points[(7*6+2)].set(max.x   ,max.y   ,max.z);
  points[(7*6+3)].set(max.x   ,max.y-dy,max.z);
  points[(7*6+4)].set(max.x   ,max.y   ,max.z);
  points[(7*6+5)].set(max.x   ,max.y   ,max.z-dz);

  if(p_m) {
    for(i = 0; i < AABB_BORDER_VERTEXNUM*6; i++) {
      matrix_transformate(points+i,p_m);
    }
  }

  glColor3ub(color.r,color.g,color.b);
  glBegin(GL_LINES);
  for(i = 0; i < AABB_BORDER_VERTEXNUM*6; i++)
    glVertex3fv((const float *)(points+i));
  glEnd();
}

VECT * aabb::border_get(VECT *p_border)
{
  int i;    
  for(i = 0; i < AABB_BORDER_VERTEXNUM; i++) {
    p_border[i].x = (i & 0x1) ? max.x : min.x;
    p_border[i].y = (i & 0x2) ? max.y : min.y;
    p_border[i].z = (i & 0x4) ? max.z : min.z;
  }
  return(p_border);
}

//Intersection with line a->b
int aabb::intersect_vector(VECT *p_a, VECT *p_b, VECT *p_intersect, float *p_t)
{
  // line a->b
  float tn = 0.0f,tf = FLT_MAX;
  float t1,t2,t;
  VECT  Q;

  if(inside(p_a)) {
    *p_intersect = *p_a;
    *p_t = 0.0f;
    return(inside(p_b) ? 2 : 1);
  }
      
  // p_a->p_b
  Q = *p_b - *p_a;
  
  // X
  t1 = max.x-p_a->x; // default Tf
  t2 = min.x-p_a->x; // default Tn
  if(Q.x < 0.0f) {
    t = t1;
    t1 = t2;
    t2 = t;
  }
  tn = MAX(t2/Q.x,tn);
  tf = MIN(t1/Q.x,tf);
  if(tn > tf || tf < 0.0f)
    return(FALSE);
    
  // Y
  t1 = max.y-p_a->y; // default Tf
  t2 = min.y-p_a->y; // default Tn
  if(Q.y < 0.0f) {
    t = t1;
    t1 = t2;
    t2 = t;
  }
  tn = MAX(t2/Q.y,tn);
  tf = MIN(t1/Q.y,tf);
  if(tn > tf || tf < 0.0f)
    return(FALSE);
  
  // Z
  t1 = max.z-p_a->z; // default Tf
  t2 = min.z-p_a->z; // default Tn
  if(Q.z < 0.0f) {
    t = t1;
    t1 = t2;
    t2 = t;
  }
  tn = MAX(t2/Q.z,tn);
  tf = MIN(t1/Q.z,tf);
  if(tn > tf || tf < 0.0f)
    return(FALSE);

  if(p_t) {
    *p_t = tn;
  }
  
  if(p_intersect) {
    p_intersect->x = p_a->x+Q.x*tn;
    p_intersect->y = p_a->y+Q.y*tn;
    p_intersect->z = p_a->z+Q.z*tn;
  }
  
  return(3);
}

bool aabb::intersect(aabb *p_box)
{
  return(min.x < p_box->max.x && max.x > p_box->min.x &&
         min.y < p_box->max.y && max.y > p_box->min.y &&
         min.z < p_box->max.z && max.z > p_box->min.z);
}

bool aabb::intersect(BOX *p_box)
{
  switch(p_box->box_type_get()) {
    case BOX_AABB:
      {
        // AABB x AABB intersection
        return(intersect((AABB *)p_box));
      }
    case BOX_OBB:
      {
        // AABB x OBB intersection
        OBB tmp((AABB *)this);
        return(obb::intersect(&tmp, (OBB *)p_box));
      }
      break;
    default:
      return(FALSE);
  }
}

// diffrence between point and plane
float aabb::intersect_plane(VECT *p_a, PLANE_TYPE type)
{
  float v1,v2;
  
  if(type == PLANE_X) {
    if(p_a->x >= min.x && p_a->x <= max.x) {
      return(0.0f);
    } else {
      v1 = (float)fabs(p_a->x-min.x);
      v2 = (float)fabs(p_a->x-max.x);
      return(v1 < v2 ? v1 : v2);
    }
  } else if(type == PLANE_Y) {    
    if(p_a->y >= min.y && p_a->y <= max.y) {
      return(0.0f);
    } else {
      v1 = (float)fabs(p_a->y-min.y);
      v2 = (float)fabs(p_a->y-max.y);
      return(v1 < v2 ? v1 : v2);
    }
  } else if(type == PLANE_Z) {
    if(p_a->z >= min.z && p_a->z <= max.z) {
      return(0.0f);
    } else {
      v1 = (float)fabs(p_a->z-min.z);
      v2 = (float)fabs(p_a->z-max.z);
      return(v1 < v2 ? v1 : v2);
    }
  } 
  return(FLT_MAX);
}

// diffrence between point and the cube
void aabb::intersect_point(VECT *p_vec)
{
  if(min.x > p_vec->x)
    p_vec->x = min.x;
  else {
    if(max.x < p_vec->x)
      p_vec->x = max.x;
  }
  
  if(min.y > p_vec->y)
    p_vec->y = min.y;
  else {
    if(max.y < p_vec->y)
      p_vec->y = max.y;
  }
  
  if(min.z > p_vec->z)
    p_vec->z = min.z;
  else {
    if(max.z < p_vec->z)
      p_vec->z = max.z;
  }
}

// Convert AABB to OBB
AABB * aabb::to_aabb(AABB *p_dest)
{
  *p_dest = *this;
  return(p_dest);
}

OBB  * aabb::to_obb(OBB *p_dest)
{
  center_get(&p_dest->center);
  length_get(&p_dest->len);
  
  p_dest->dir[0].set(1,0,0);
  p_dest->dir[1].set(0,1,0);
  p_dest->dir[2].set(0,0,1);
  
  return(p_dest);
}

void aabb::set(class box *p_src)
{  
  p_src->to_aabb(this);
}

aabb::aabb(class box *p_src)
{  
  set(p_src);
}
