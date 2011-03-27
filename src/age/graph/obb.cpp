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
 * OBB and it's trees
 */

// Constants and types
#include "age.h"

#define same_point(a,b)    (a->x == b->x && a->y == b->y && a->z == b->z)

/*
  By Miguel Gomez,Gamasutra,October 18, 1999 
*/
bool obb::intersect(OBB *p_obb1, OBB *p_obb2)
{
  VECT *p_a = &p_obb1->len;
  VECT *p_A =  p_obb1->dir;
  VECT *p_Pa = &p_obb1->center;

  VECT *p_b = &p_obb2->len;
  VECT *p_B =  p_obb2->dir;
  VECT *p_Pb = &p_obb2->center;

  VECT a;       // lengths
  VECT Ax,Ay,Az;// normal vectors
  VECT Pa;      // position
  
  VECT b;       // lengths
  VECT Bx,By,Bz;// normal vectors
  VECT Pb;      // position

  //ALGORITHM: Use the separating axis test for all 15 potential
  //separating axes. If a separating axis could not be found, the two
  //boxes overlap.
  
  VECT v,T,Rx,Ry,Rz;
  float ra,rb,t;
  
  v = *p_Pb - *p_Pa;
  
  T.x  = vect_dot_product(v,p_A[0]);
  Rx.x = vect_dot_product(p_A[0],p_B[0]);
  Rx.y = vect_dot_product(p_A[0],p_B[1]);
  Rx.z = vect_dot_product(p_A[0],p_B[2]);
  
  ra = p_a->x;
  rb = p_b->x*fabsf(Rx.x) + p_b->y*fabsf(Rx.y) + p_b->z*fabsf(Rx.z);
  t =  fabsf(T.x);
  if( t > ra + rb )
    return(FALSE);
  
  T.y  = vect_dot_product(v,p_A[1]);
  Ry.x = vect_dot_product(p_A[1],p_B[0]);
  Ry.y = vect_dot_product(p_A[1],p_B[1]);
  Ry.z = vect_dot_product(p_A[1],p_B[2]);
  
  ra = p_a->y;
  rb = p_b->x*fabsf(Ry.x) + p_b->y*fabsf(Ry.y) + p_b->z*fabsf(Ry.z);
  t =  fabsf(T.y);
  if( t > ra + rb )
    return(FALSE);
  
  T.z  = vect_dot_product(v,p_A[2]);
  Rz.x = vect_dot_product(p_A[2],p_B[0]);
  Rz.y = vect_dot_product(p_A[2],p_B[1]);
  Rz.z = vect_dot_product(p_A[2],p_B[2]);
  
  ra = p_a->z;
  rb = p_b->x*fabsf(Rz.x) + p_b->y*fabsf(Rz.y) + p_b->z*fabsf(Rz.z);
  t =  fabsf(T.z);
  if( t > ra + rb )
    return(FALSE);  
  
  //B->A v(x)
  ra = p_a->x*fabsf(Rx.x) + p_a->y*fabsf(Ry.x) + p_a->z*fabsf(Rz.x);
  rb = p_b->x;
  t =  fabsf(T.x*Rx.x + T.y*Ry.x + T.z*Rz.x);
  if( t > ra + rb )
    return(FALSE);
  
  //B->A v(y)
  ra = p_a->x*fabsf(Rx.y) + p_a->y*fabsf(Ry.y) + p_a->z*fabsf(Rz.y);
  rb = p_b->y;
  t =  fabsf(T.y*Rx.y + T.y*Ry.y + T.z*Rz.y);
  if( t > ra + rb )
    return(FALSE);
  
  //B->A v(z)
  ra = p_a->x*fabsf(Rx.z) + p_a->y*fabsf(Ry.z) + p_a->z*fabsf(Rz.z);
  rb = p_b->z;
  t =  fabsf(T.z*Rx.z + T.y*Ry.z + T.z*Rz.z);
  if( t > ra + rb )
    return(FALSE);
  
  //9 cross products   
  //L = A0 x B0
  ra = p_a->y*fabsf(Rz.x) + p_a->z*fabsf(Ry.z);
  rb = p_b->y*fabsf(Rx.z) + p_b->z*fabsf(Rx.y);
  t =  fabsf(T.z*Ry.x - T.y*Rz.x);
  if( t > ra + rb )
    return(FALSE);
  
  //L = A0 x B1
  ra = p_a->y*fabsf(Rz.y) + p_a->z*fabsf(Ry.y);
  rb = p_b->x*fabsf(Rx.z) + p_b->z*fabsf(Rx.x);
  t = fabsf(T.z*Ry.y - T.y*Rz.y);
  if( t > ra + rb )
    return(FALSE);
  
  //L = A0 x B2
  ra = p_a->y*fabsf(Rz.z) + p_a->z*fabsf(Ry.z);   
  rb = p_b->x*fabsf(Rx.y) + p_b->y*fabsf(Rx.x);
  t = fabsf(T.z*Ry.z - T.y*Rz.z);
  if( t > ra + rb )
    return(FALSE);
  
  //L = A1 x B0
  ra = p_a->x*fabsf(Rz.x) + p_a->z*fabsf(Rx.x);
  rb = p_b->y*fabsf(Ry.z) + p_b->z*fabsf(Ry.y);
  t = fabsf(T.x*Rz.x - T.z*Rx.x);
  if( t > ra + rb )
    return(FALSE);
  
  //L = A1 x B1
  ra = p_a->x*fabsf(Rz.y) + p_a->z*fabsf(Rx.y);
  rb = p_b->x*fabsf(Ry.z) + p_b->z*fabsf(Ry.x);
  t = fabsf(T.x*Rz.y - T.z*Rx.y);
  if( t > ra + rb )
    return(FALSE);
  
  //L = A1 x B2
  ra = p_a->x*fabsf(Rz.z) + p_a->z*fabsf(Rx.z);
  rb = p_b->x*fabsf(Ry.y) + p_b->y*fabsf(Ry.x);
  t = fabsf(T.x*Rz.z - T.z*Rx.z);
  if( t > ra + rb )
    return(FALSE);
  
  //L = A2 x B0
  ra = p_a->x*fabsf(Ry.x) + p_a->y*fabsf(Rx.x);
  rb = p_b->y*fabsf(Rz.z) + p_b->z*fabsf(Rz.y);
  t = fabsf(T.y*Rx.x - T.x*Ry.x);
  if( t > ra + rb )
    return(FALSE);
  
  //L = A2 x B1
  ra = p_a->x*fabsf(Ry.y) + p_a->y*fabsf(Rx.y);
  rb = p_b->x*fabsf(Rz.z) + p_b->z*fabsf(Rz.x);
  t = fabsf(T.y*Rx.y - T.x*Ry.y);
  if( t > ra + rb )
    return(FALSE);
  
  //L = A2 x B2
  ra = p_a->x*fabsf(Ry.z) + p_a->y*fabsf(Rx.z);
  rb = p_b->x*fabsf(Rz.y) + p_b->y*fabsf(Rz.x);
  t =  fabsf(T.y*Rx.z - T.x*Ry.z);
  if( t > ra + rb )
    return(FALSE);
  
  return(TRUE);
}        

int obb::intersect(VECT *p_orig, VECT *p_dir, VECT *p_intersect, float *p_t)
{  
  VECT *p_A = dir;
  VECT *p_Pa = &center;
  
  VECT t,o,dir,min;

  t = *p_orig - *p_Pa;
  o.x = vect_dot_product(p_A[0],t);
  o.y = vect_dot_product(p_A[1],t);
  o.z = vect_dot_product(p_A[2],t);

  dir.x = vect_dot_product(p_A[0],*p_dir);
  dir.y = vect_dot_product(p_A[1],*p_dir);
  dir.z = vect_dot_product(p_A[2],*p_dir);
  
  min = len * -1.0f;
  AABB tmp(&min,&len);
  return(tmp.intersect(&o,&dir,p_intersect,p_t));
}

int obb::intersect_vector(VECT *p_a, VECT *p_b, VECT *p_intersect, float *p_t)
{  
  VECT  *p_A = dir;
  VECT  *p_Pa = &center;
  VECT   p,target;
  VECT   t,o,dir,min;
  int    ret;

  t = *p_a - *p_Pa;
  o.x = vect_dot_product(p_A[0],t);
  o.y = vect_dot_product(p_A[1],t);
  o.z = vect_dot_product(p_A[2],t);

  target = *p_b - *p_a;
  dir.x = vect_dot_product(p_A[0],target);
  dir.y = vect_dot_product(p_A[1],target);
  dir.z = vect_dot_product(p_A[2],target);
  
  min = len * -1.0f;
  AABB tmp(&min,&len);
  ret = tmp.intersect(&o,&dir,p_intersect,p_t);
  
  return((*p_t < 1.0f) ? ret : FALSE);
}

VECT * obb::border_get(VECT *p_border)
{
  VECT  a1,a2,a3;
  VECT  a12,a22,a32;

  a1 = dir[0] * len.x;
  a2 = dir[1] * len.y;
  a3 = dir[2] * len.z;
  
  a12 = a1 * 2.0f;
  a22 = a2 * 2.0f;
  a32 = a3 * 2.0f;

  p_border[0] = center - a1;
  p_border[0] -= a2;
  p_border[0] -= a3;
  
  p_border[1] = *p_border + a12;

  p_border[2] = p_border[0] + a32;
  p_border[3] = p_border[1] + a32;
    
  p_border[4] = p_border[0] + a22;
  p_border[5] = p_border[1] + a22;
  p_border[6] = p_border[2] + a22;
  p_border[7] = p_border[3] + a22;
  
  return(p_border);
}

bool obb::intersect(BOX *p_box)
{
  switch(p_box->box_type_get()) {
    case BOX_AABB:
      {
        // OBB x AABB intersection
        OBB tmp((AABB *)p_box);
        return(intersect(&tmp, (OBB *)p_box));
      }
    case BOX_OBB:
      {
        // OBB x OBB intersection
        return(intersect(this, (OBB *)p_box));
      }
    default:
      return(FALSE);
  }
}

void obb::transformate(OBB *p_src, GLMATRIX *p_m, OBB *p_dest)
{
  GLMATRIX m;
  VECT     a1;
  float    lx,ly,lz;

  matrix_transformate(&p_src->center,p_m,&p_dest->center);
  
  lx = p_src->len.x;
  ly = p_src->len.y;
  lz = p_src->len.z;
  
  m = *p_m; m._41 = m._42 = m._43 = 0.0f;
  
  if(lx != 0.0f) {
    a1 = p_src->dir[0] * lx;
    matrix_transformate(&a1, &m, p_dest->dir);
    p_dest->len.x = p_dest->dir[0].norm();
  } else {
    matrix_transformate(p_src->dir, &m, p_dest->dir);
    p_dest->len.x = 0.0f;
  }
  
  if(ly != 0.0f) {
    a1 = p_src->dir[1] * ly;
    matrix_transformate(&a1, &m, p_dest->dir+1);
    p_dest->len.y = p_dest->dir[1].norm();
  } else {
    matrix_transformate(p_src->dir+1, &m, p_dest->dir+1);
    p_dest->len.y = 0.0f;
  }
  
  if(lz != 0.0f) {
    a1 = p_src->dir[2] * lz;
    matrix_transformate(&a1, &m, p_dest->dir+2);
    p_dest->len.z = p_dest->dir[2].norm();
  } else {
    matrix_transformate(p_src->dir+2, &m, p_dest->dir+2);
    p_dest->len.z = 0.0f;
  }
}

BOX * obb::transformate(GLMATRIX *p_m)
{
  obb tmp;
  tmp = *this;
  transformate(&tmp, p_m, this);
  return(this);
}

/*
 * Is at least one point of this cube visible?
 */
bool obb::visible(GLMATRIX *p_m)
{  
  int i,j,xplane,yplane,xa,ya,m;
  float x,y,z,xt,yt,zt,w,fx,fy,fw;
  VECT *p_vert = border;
  
  // ... // -> vyrobeni 8 hranicnich bodu krychle  
  xplane = 0; // -2, -1, 1, 2
  yplane = 0; // -2, -1, 1, 2
  
  for(i = 0, j = 0; i < 8; i++) {
    xt = p_vert->x;
    yt = p_vert->y;
    zt = p_vert->z;
    
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
    p_vert++;
  }
  
  return((j > 0) ? ((abs(xplane) != 16) && (abs(yplane) != 16)) : FALSE);      
}

/*
 * Are all 8 points of the cube visible?
 */
bool obb::visible_full(GLMATRIX *p_m)
{  
  int i,j,xplane,yplane,xa,ya,m;
  float x,y,z,xt,yt,zt,w,fx,fy,fw;
  int hits = 0;
  VECT *p_vert = border;
  
  // ... // -> vyrobeni 8 hranicnich bodu krychle  
  xplane = 0; // -2, -1, 1, 2
  yplane = 0; // -2, -1, 1, 2
  
  for(i = 0, j = 0; i < 8; i++) {
    xt = p_vert->x;
    yt = p_vert->y;
    zt = p_vert->z;
    
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
    p_vert++;
  }
  
  return((hits == 8) ? hits : (j > 0) ? ((abs(xplane) != 16) && (abs(yplane) != 16)) : FALSE);  
}

void obb::adjust(VECT *p_center, float radius)
{
  assert(0);
}

void obb::adjust_len(VECT *p_vec)
{
  VECT tmp;
  float v;
  float ln;
  
  // Adjust X axis
  v = fabsf(p_vec->x);
  ln = fabsf(len.x);
  if(v > ln) {
    float adj = (v-ln)*0.5f;
    tmp = dir[0] * adj;
    center += tmp;
    len.x += adj;
  }

  // Adjust Y axis
  v = fabsf(p_vec->y);
  ln = fabsf(len.y);
  if(v > ln) {
    float adj = (v-ln)*0.5f;
    tmp = dir[0] * adj;
    center += tmp;
    len.y += adj;
  }

  // Adjust Z axis
  v = fabsf(p_vec->z);
  ln = fabsf(len.z);
  if(v > ln) {
    float adj = (v-ln)*0.5f;
    tmp = dir[0] * adj;
    center += tmp;
    len.z += adj;
  }  
}

void obb::adjust(VECT *p_vec)
{
  GLMATRIX m;  
  VECT tmp;
  
  matrix_to_obb_get(&m);  
  matrix_transformate(p_vec,&m,&tmp);  
  adjust_len(&tmp);
}

void obb::adjust(class obb *p_obb)
{
  GLMATRIX m;  
  VECT tmp;
  int  i;
  
  matrix_to_obb_get(&m);  
  for(i = 0; i < OBB_BORDER_VERTEXNUM; i++) {
    matrix_transformate(p_obb->border+i,&m,&tmp);
    adjust_len(&tmp);
  }
}

/*
// Converts border points to AABB
AABB * obb::aabb_get(GLMATRIX *p_m, AABB *p_out)
{
  VECT a;
  int i;

  if(p_m) {
    matrix_transformate(border,p_m,&a);
  } else {
    a = border[0];
  }

  p_out->set(&a,&a);
  for(i = 1; i < 8; i++) {
    if(p_m) {
      matrix_transformate(border+i,p_m,&a);
    } else {
      a = border[i];
    }
    p_out->adjust(&a);
  }
  
  return(p_out);
}
*/

void obb::set(OBB *p_list, int num)
{
  OBB       best;
  int       i,j;
  float     volume, bvolume = FLT_MAX;  

  if(num == 1) {
    *this = p_list[0];
    return;
  }  
    
  for(i = 0; i < num; i++) {
    OBB tmp(p_list+i);
    
    for(j = 0; j < num; j++) {
      if(i != j)
        tmp.adjust(p_list+j);
    }

    volume = tmp.volume_get();
    if(volume < bvolume) {      
      bvolume = volume;
      best = tmp;
    }
  }
  
  *this = best;
}

void obb::set_most_precise(VECT *p_vertex, int vertexnum, tface *p_face, int facenum)
{
  VECT  bnorm(0,1,0),bv1(1,0,0),bv2(0,0,1); // best values
  VECT  norm,v1,v2;                         // current computed values
  
  VECT  bcenter(0,0,0);
  VECT  blength(1,1,1);
  int   f0,f1,f2,f;
  float volume, bvolume;

  bvolume = FLT_MAX;
  
  for(f = 0; f < facenum; f += 3) {

    f0 = p_face[f];
    f1 = p_face[f+1];
    f2 = p_face[f+2];

    face_normal(p_vertex+f0, p_vertex+f1, p_vertex+f2, &norm);

    if(norm.zero())
      continue;

    // f0->f1
    {
      v1 = p_vertex[f1] - p_vertex[f0];
      v2 = vect_mult(v1,norm);
  
      v1.norm();
      v2.norm();
      norm.norm();
      
      GLMATRIX m;
      m.rotation_set(&v1,&norm,&v2);
      
      AABB tmp(&m,p_vertex, vertexnum, p_face, facenum);
      volume = tmp.volume_get();
      
      if(volume < bvolume) {
        bvolume = volume;
        
        // save the best values
        bv1 = v1;
        bv2 = v2;
        bnorm = norm;
                
        tmp.center_get(&bcenter);
        tmp.length_get(&blength);
        
        m.invert();
        m.transformate(&bcenter);
      }
    }

    // f0->f2
    {
      v1 = p_vertex[f2] - p_vertex[f0];
      v2 = vect_mult(v1,norm);
  
      v1.norm();
      v2.norm();
      norm.norm();
      
      GLMATRIX m;
      m.rotation_set(&v1,&norm,&v2);
      
      AABB tmp(&m,p_vertex, vertexnum, p_face, facenum);
      volume = tmp.volume_get();
      
      if(volume < bvolume) {
        bvolume = volume;
        
        // save the best values
        bv1 = v1;
        bv2 = v2;
        bnorm = norm;
        
        tmp.center_get(&bcenter);
        tmp.length_get(&blength);
        
        m.invert();
        m.transformate(&bcenter);
      }
    }

    // f1->f2
    {
      v1 = p_vertex[f2] - p_vertex[f1];
      v2 = vect_mult(v1,norm);
  
      v1.norm();
      v2.norm();
      norm.norm();
      
      GLMATRIX m;
      m.rotation_set(&v1,&norm,&v2);
      
      AABB tmp(&m,p_vertex, vertexnum, p_face, facenum);    
      volume = tmp.volume_get();
      
      if(volume < bvolume) {
        bvolume = volume;
        
        // save the best values
        bv1 = v1;
        bv2 = v2;
        bnorm = norm;
        
        tmp.center_get(&bcenter);
        tmp.length_get(&blength);
        
        m.invert();
        m.transformate(&bcenter);        
      }
    }
  }
  
  dir[0] = bv1;
  dir[1] = bnorm;
  dir[2] = bv2;
  
  center = bcenter;
  len = blength;
  
  border_update();
}

void obb::set_precise(VECT *p_vertex, int vertexnum, tface *p_face, int facenum)
{
  VECT  bnorm(0,1,0),bv1(1,0,0),bv2(0,0,1); // best values
  VECT  norm,v1,v2;                         // current computed values
  
  VECT  bcenter(0,0,0);
  VECT  blength(1,1,1);
  int   f0,f1,f2,f;
  float volume, bvolume;

  bvolume = FLT_MAX;
  
  for(f = 0; f < facenum; f += 3) {

    f0 = p_face[f];
    f1 = p_face[f+1];
    f2 = p_face[f+2];

    face_normal(p_vertex+f0, p_vertex+f1, p_vertex+f2, &norm);

    if(norm.zero())
      continue;

    // f0->f1
    {
      v1 = p_vertex[f1] - p_vertex[f0];
      v2 = vect_mult(v1,norm);
  
      v1.norm();
      v2.norm();
      norm.norm();
      
      GLMATRIX m;
      m.rotation_set(&v1,&norm,&v2);
      
      AABB tmp(&m,p_vertex, vertexnum, p_face, facenum);
      volume = tmp.volume_get();
      
      if(volume < bvolume) {
        bvolume = volume;
        
        // save the best values
        bv1 = v1;
        bv2 = v2;
        bnorm = norm;
                
        tmp.center_get(&bcenter);
        tmp.length_get(&blength);
        
        m.invert();
        m.transformate(&bcenter);
      }
    }
  }
  
  dir[0] = bv1;
  dir[1] = bnorm;
  dir[2] = bv2;
  
  center = bcenter;
  len = blength;
  
  border_update();
}

void obb::set_fast(VECT *p_vertex, int vertexnum, tface *p_face, int facenum)
{  
  AABB tmp(p_vertex, vertexnum, p_face, facenum);
  set(&tmp.min,&tmp.max);
}

/*
  TODO:
    - create some metrixes how to determine envelope creation method

*/

#define OBB_MOST_PRECISE  200
#define OBB_PRECISE       1000

void obb::set(VECT *p_vertex, int vertexnum, tface *p_face, int facenum)
{
  if(facenum < OBB_MOST_PRECISE) {
    set_most_precise(p_vertex, vertexnum, p_face, facenum);
  } else if(facenum < OBB_MOST_PRECISE) {
    set_precise(p_vertex, vertexnum, p_face, facenum);
  } else {
    set_fast(p_vertex, vertexnum, p_face, facenum);
  }  
}

void obb::set(GLMATRIX *p_mat, VECT *p_vertex, int vertexnum, tface *p_face, int facenum)
{
  VECT *p_tvert = (VECT *)malloc(sizeof(p_vertex[0])*vertexnum);
  int i;
  for(i = 0; i < vertexnum; i++)
    matrix_transformate(p_vertex+i,p_mat,p_tvert+i);
  set(p_tvert, vertexnum, p_face, facenum);
  free(p_tvert);
}

void obb::draw(GLMATRIX *p_m, RGBB color)
{ 
  AABB     tmp;
  GLMATRIX m;
  
  // Transform World Space->OBB
  m.rotation_set(dir,dir+1,dir+2);
  if(m.determinant()) {
    
    // OBB->World Space
    m.invert();
    m.translation_set(&center);
    
    if(p_m) 
      matrix_mult(p_m,&m,&m);
    
    tmp.set_len(&len);
    tmp.draw(&m,color);
  }
}

void obb::print(char *p_name)
{
  if(p_name) {
    pprintf("Box %s",p_name);
  }  
  pprintf("OBB dir[0] = [%f,%f,%f]",dir[0].x,dir[0].y,dir[0].z);
  pprintf("OBB dir[1] = [%f,%f,%f]",dir[1].x,dir[1].y,dir[1].z);
  pprintf("OBB dir[2] = [%f,%f,%f]",dir[2].x,dir[2].y,dir[2].z);
  pprintf("OBB center = [%f,%f,%f]",center.x,center.y,center.z);
  pprintf("OBB length = [%f,%f,%f]",center.x,center.y,center.z);
}

OBB * obb::to_obb(OBB *p_dest)
{
  *p_dest = *this;
  return(p_dest);
}

// Convert OBB to AABB
AABB * obb::to_aabb(AABB *p_dest)
{
  VECT border[8];  
  p_dest->set(border_get(border),8,NULL,0);
  return(p_dest);
}

void obb::set(class box *p_src)
{
  p_src->to_obb(this);
}

obb::obb(class box *p_src)
{
  set(p_src);
}

/* OBB Tree
*/

/*
void obb_tree::kresli(dword color)
{
  if(this) {  
    obb.draw(color, NULL);
    for(int i = 0; i < nextnum; i++)
      p_next[i].kresli(color);
  }
}
*/
/*
void obb_calc_item(OBB *p_vys, OBB_TREE_ITEM *p_item, int itemnum)
{ 
  OBB *p_list = (OBB *)mmalloc(sizeof(OBB)*itemnum);
  int  i;

  for(i = 0; i < itemnum; i++) {
    p_list[i] = *(p_item[i].p_obb);
  }
  
  p_vys->slep_aabb(p_list,itemnum);
  free(p_list);
}


void obbtree_calc_obalky(OBB_TREE *p_prvni)
{
  OBB_TREE_ITEM *p_itn;
  OBB *p_list;
  int  p,i;
  
  p_list = (OBB *)mmalloc(sizeof(p_list[0])*(p_prvni->nextnum+p_prvni->itnum));
  for(i = 0; i < p_prvni->nextnum; i++) {
    obbtree_calc_obalky(p_prvni->p_next+i);
    p_list[i] = p_prvni->p_next[i].obb;
  }
 
  p_itn = p_prvni->p_item;
  for(p = 0; p < p_prvni->itnum; p++, p_itn++) {
    if(p_itn->p_mesh) {
      p_list[i+p] = p_itn->p_mesh->obb_world;
    } else {
      p_list[i+p] = p_itn->p_poly->obb;
    }
  }

  obb_slep(&p_prvni->obb,p_list,i+p);
  free(p_list);
}

#define MAX_VZDAL_OBALEK          20.0f
#define MIN_PODOBALEK_OBJ         10
#define MIN_PODOBALEK_OBJ_ZBYTEK  10
*/
/* Kazdy polygon ma uz implicitne obalku
*/
/*
static EDIT_MATERIAL **p_mat;

void obbtree_vyrob(OBB_TREE *p_prvni, EDIT_MESH_POLY *p_poly, int polynum, GAME_MESH **p_mesh, int meshnum, int *p_ind, EDIT_MATERIAL **p_matlist)
{
  int i,j,itnum = polynum+meshnum;

  p_mat = p_matlist;

  memset(p_prvni,0,sizeof(p_prvni[0]));
  p_prvni->p_item = (OBB_TREE_ITEM *)mmalloc(sizeof(p_prvni->p_item[0])*itnum);
  p_prvni->itnum = itnum;

  for(i = 0; i < polynum; i++) {
    p_prvni->p_item[i].p_poly = p_poly+i;
    p_prvni->p_item[i].p_obb = &p_poly[i].obb;
  }
  for(j = 0; j < meshnum; j++) {
    if(p_ind)
      p_prvni->p_item[i+j].mesh = p_ind[j];
    p_prvni->p_item[i+j].p_mesh = p_mesh[j];
    p_prvni->p_item[i+j].p_obb = &p_mesh[j]->obb_world;
  }
  
//  obbtree_vyrob_rec(p_prvni, MAX_VZDAL_OBALEK);
  obbtree_calc_obalky(p_prvni);
}

static int obbtree_vyrob_rec_comp(const void *p_1, const void *p_2)
{
  OBB_TREE_ITEM *p_it1 = (OBB_TREE_ITEM *)p_1;
  OBB_TREE_ITEM *p_it2 = (OBB_TREE_ITEM *)p_2;
  EDIT_MESH_POLY *p_poly1,*p_poly2;
  
  if(p_it1->p_mesh && p_it2->p_poly)
    return(-1);
  else if(p_it2->p_mesh && p_it1->p_poly)
    return(1);  
  if(p_it1->p_mesh && p_it2->p_mesh)
    return(0);
  
  p_poly1 = p_it1->p_poly;
  p_poly2 = p_it2->p_poly;

  if(p_mat[p_poly1->material]->p_text[0] && p_mat[p_poly2->material]->p_text[0])
    return(p_mat[p_poly1->material]->p_text[0]->text - p_mat[p_poly2->material]->p_text[0]->text);
  else
    return(p_mat[p_poly1->material]->p_text[0] - p_mat[p_poly2->material]->p_text[0]);
}
*/
/* Ze seznamu polylistu vybere ty co se daji do obalky
   a zbytek se soupne do te hlavni
*/
/* p_prvni -> pointer na prvni obalku, do ktere se strci vsechny
   globalni poly a ty podobalky se strci do pod_obalek
*/
/*
void obbtree_vyrob_rec(OBB_TREE *p_prvni, float max_vzdal)
{
  OBB_TREE_ITEM *p_item = p_prvni->p_item;
  int            itemnum = p_prvni->itnum;
  int            itemzbyva = p_prvni->itnum;
  OBB_TREE_ITEM *p_tmp_item = (OBB_TREE_ITEM *)_alloca(sizeof(p_tmp_item[0])*itemnum);
  int            tmp_itemnum;
  OBB_TREE      *p_obalky = (OBB_TREE *)_alloca(sizeof(p_obalky[0])*itemnum);
  int            obalnum = 0;
  int           *p_hits = (int *)_alloca(sizeof(p_hits[0])*itemnum);   // globalni hity
  int           *p_lhits = (int *)_alloca(sizeof(p_lhits[0])*itemnum); // lokalni hity
  OBB            obb;
  int            p,i,itemzustava;

  if(p_prvni->itnum <= MIN_PODOBALEK_OBJ)
    return;

  memset(p_tmp_item,0,sizeof(p_tmp_item[0])*itemnum);
  memset(p_obalky,0,sizeof(p_obalky[0])*itemnum);
  memset(p_hits,0,sizeof(p_hits[0])*itemnum);
  memset(p_lhits,0,sizeof(p_lhits[0])*itemnum);

  // Cyklus pres vsechny poly, ktere nejsou jesne vyrazeny ze seznamu volnych
  //   - hit-list
  
  for(p = 0; p < itemnum; p++) {
    if(p_hits[p]) 
      continue;
       
    // seznam tmp poly -> zacatek seznamu potencialni obalky
    // momentalne to je pouze ten 1 prvek
    
    p_tmp_item[0] = p_item[p];
    tmp_itemnum = 1;
    p_lhits[p] = TRUE;
    
    // rozmery minimalni obalky o 1 poly
    
    obb = *p_item[p].p_obb;

    // Pridani vsech blizskych poly do 1 obalky
    
    for(i = 0; i < itemnum; i++) {
      if(i == p || p_hits[i])
        continue;
      p_lhits[i] = (vzdal_bodu_bod(&obb.obb_center,&p_item[i].p_obb->obb_center) <= max_vzdal);
      if(p_lhits[i]) {
        p_tmp_item[tmp_itemnum++] = p_item[i];
        obb_calc_item(&obb,p_tmp_item,tmp_itemnum);
      }
    }
    
    // Pokud je blizko sebe vice objektu nez co je minimum jedne obalky -> 
    // - udelej tuto samostanou pod-obalku se shluku objektu -> vyber je
    // - a to pouze pokud jeste neco zbyde v puvodni obalce
    if(tmp_itemnum >= MIN_PODOBALEK_OBJ && itemzbyva-tmp_itemnum >= MIN_PODOBALEK_OBJ_ZBYTEK) {
      itemzbyva -= tmp_itemnum;
      p_obalky[obalnum].obb = obb;
      p_obalky[obalnum].p_item = (OBB_TREE_ITEM *)kopiruj_pole(p_tmp_item,sizeof(p_tmp_item[0])*tmp_itemnum);
      p_obalky[obalnum].itnum = tmp_itemnum;
      p_obalky[obalnum].p_up = p_prvni;
      obalnum++;

      for(i = 0; i < itemnum; i++) // Prekopiruje hit-list
        p_hits[i] |= p_lhits[i];
    }
  }

  // Spocitam pocet neumistenych polozek  
  memcpy(p_tmp_item,p_item,sizeof(p_item[0])*itemnum);
  itemzustava = 0;
  for(i = 0; i < itemnum; i++) {
    if(!p_hits[i])
      itemzustava++;
  }
  
  free(p_prvni->p_item);

  p_prvni->p_item = (OBB_TREE_ITEM *)mmalloc(sizeof(p_prvni->p_item[0])*itemzustava);
  p_prvni->itnum = itemzustava;
  
  // Skopiruju neumistene polozky
  for(i = 0, itemzustava = 0; i < itemnum; i++) {
    if(!p_hits[i])
      p_prvni->p_item[itemzustava++] = p_tmp_item[i];
  }   

  // Setridim polozky podle pouzitych textur
  qsort(p_prvni->p_item,p_prvni->itnum,sizeof(p_prvni->p_item[0]),obbtree_vyrob_rec_comp);

  
  // Skopiruji se obalky
  p_prvni->nextnum = obalnum;
  if(obalnum) {
    p_prvni->p_next = (OBB_TREE *)kopiruj_pole(p_obalky,sizeof(p_prvni->p_next[0])*obalnum);

    // Pro kazdou pod-obalku zavolej delicku...
    max_vzdal /= 2.0f;
    for(i = 0; i < obalnum; i++) {
      obbtree_vyrob_rec(p_prvni->p_next+i,max_vzdal);
    }
  }
}

void obbtree_zrus(OBB_TREE *p_ob)
{  
  int  i;
  if(p_ob->p_item) {
    free(p_ob->p_item);
    p_ob->p_item = NULL;
  }
  if(p_ob->p_next) {
    for(i = 0; i < p_ob->nextnum; i++) {
      obbtree_zrus(p_ob->p_next+i);
    }
    free(p_ob->p_next);
  }
}
*/
