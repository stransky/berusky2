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

#define EPSILON 0.000001f

/*
 * Line/Ray and point p intersection
 */
bool intersect_ray_point(VECT *p_a, VECT *p_b, VECT *p_p, VECT *p_I, float *p_ti)
{
  float t_i,vz;

  t_i = (p_b->x - p_a->x)*(p_p->x-p_a->x)+(p_b->y - p_a->y)*(p_p->y-p_a->y)+(p_b->z - p_a->z)*(p_p->z-p_a->z);
  vz = point_dist(*p_a,*p_b);
  t_i /= (vz*vz);
    
  if(p_I) {
    p_I->x = p_a->x + (p_b->x - p_a->x)*t_i;
    p_I->y = p_a->y + (p_b->y - p_a->y)*t_i;
    p_I->z = p_a->z + (p_b->z - p_a->z)*t_i;
  }
  
  if(p_ti)
    *p_ti = t_i;
  
  return(t_i >= 0.0);
}

bool intersect_ray_point(VECT2DI *p_a, VECT2DI *p_b, VECT2DI *p_p, VECT2DI *p_I, float *p_ti)
{
  float t_i,vz;

  t_i = (p_b->x - p_a->x)*(p_p->x-p_a->x)+(p_b->y - p_a->y)*(p_p->y-p_a->y);
  vz = point_dist(*p_a,*p_b);
  t_i /= (vz*vz);
    
  if(p_I) {
    p_I->x = (tpos)(p_a->x + (p_b->x - p_a->x)*t_i);
    p_I->y = (tpos)(p_a->y + (p_b->y - p_a->y)*t_i);
  }
  
  if(p_ti)
    *p_ti = t_i;
  
  return(t_i >= 0.0);
}

bool intersect_line_point(VECT *p_a, VECT *p_b, VECT *p_p, VECT *p_I, float *p_ti)
{
  float t_i,vz;

  t_i = (p_b->x - p_a->x)*(p_p->x-p_a->x)+(p_b->y - p_a->y)*(p_p->y-p_a->y)+(p_b->z - p_a->z)*(p_p->z-p_a->z);
  vz = point_dist(*p_a,*p_b);
  t_i /= (vz*vz);

  if(p_I) {
    p_I->x = p_a->x + (p_b->x - p_a->x)*t_i;
    p_I->y = p_a->y + (p_b->y - p_a->y)*t_i;
    p_I->z = p_a->z + (p_b->z - p_a->z)*t_i;
  }
  
  if(p_ti)
    *p_ti = t_i;
  
  return(t_i >= 0.0 && t_i <= 1.0);
}

bool intersect_line_point(VECT2DI *p_a, VECT2DI *p_b, VECT2DI *p_p, VECT2DI *p_I, float *p_ti)
{
  float t_i,vz;

  t_i = (p_b->x - p_a->x)*(p_p->x-p_a->x)+(p_b->y - p_a->y)*(p_p->y-p_a->y);
  vz = point_dist(*p_a,*p_b);
  t_i /= (vz*vz);

  if(p_I) {
    p_I->x = (tpos)(p_a->x + (p_b->x - p_a->x)*t_i);
    p_I->y = (tpos)(p_a->y + (p_b->y - p_a->y)*t_i);
  }
  
  if(p_ti)
    *p_ti = t_i;
  
  return(t_i >= 0.0 && t_i <= 1.0);
}


/*
 * Line/Ray and Line/Ray intersection
 */
// Ray/Ray intersection
bool intersect_ray_ray(VECT2DI *p_a1, VECT2DI *p_b1, VECT2DI *p_a2, VECT2DI *p_b2, VECT2DI *p_p, float *p_t1, float *p_t2)
{
  VECT2DI Q1,Q2;
  
  Q1 = static_cast<VECT2DI>(*p_b1) - static_cast<VECT2DI>(*p_a1);
  Q2 = static_cast<VECT2DI>(*p_b2) - static_cast<VECT2DI>(*p_a2);
  
  if(vect_mult(Q1,Q2) == 0)
    return(FALSE);
  
  float t1,t2;
  
  if(abs(Q1.x) < abs(Q1.y)) {
    float tmp = (float)Q1.x/(float)Q1.y;
    
    t2 = (p_a1->x + tmp*p_a2->y - tmp*p_a1->y - p_a2->x) / (-tmp*Q2.y + Q2.x);  
    if(t2 < 0.0f)
      return(FALSE);
    
    t1 = (p_a2->y + Q2.y*t2 + p_a1->y) / (Q1.y);
    if(t1 < 0.0f)
      return(FALSE);
  }
  else {
    float tmp = (float)Q1.y/(float)Q1.x;
    
    t2 = (p_a1->y + tmp*p_a2->x - tmp*p_a1->x - p_a2->y) / (-tmp*Q2.x + Q2.y);  
    if(t2 < 0.0f)
      return(FALSE);
    
    t1 = (p_a2->x + Q2.x*t2 + p_a1->x) / (Q1.x);
    if(t1 < 0.0f)
      return(FALSE);    
  }
  
  if(p_p) {
    p_p->x = (tpos)(p_a2->x + Q2.x*t2);
    p_p->y = (tpos)(p_a2->y + Q2.y*t2);
  }  
  if(p_t1) {
    *p_t1 = t1;
  }
  if(p_t2) {
    *p_t2 = t2;
  }
  
  return(TRUE);
}

// Ray/line intersection
// a1,b1 -> ray
// a2,b2 -> line
// returns  p_p -> intersection point
//          p_t -> intersection param
bool intersect_ray_line(VECT2DI *p_a1, VECT2DI *p_b1, VECT2DI *p_a2, VECT2DI *p_b2, VECT2DI *p_p, float *p_t)
{
  float t1,t2;
  if(intersect_ray_ray(p_a1, p_b1, p_a2, p_b2, p_p, &t1, &t2)) {
    if(t2 > 1.0f)
      return(FALSE);
    if(p_t) {
      *p_t = t1;
    }
    return(TRUE);
  }
  return(FALSE);
}

bool intersect_line_line(VECT2DI *p_a1, VECT2DI *p_b1, VECT2DI *p_a2, VECT2DI *p_b2, VECT2DI *p_p)
{
  float t1,t2;
  if(intersect_ray_ray(p_a1, p_b1, p_a2, p_b2, p_p, &t1, &t2)) {
    return(t2 <= 1.0f && t1 <= 1.0f);
  }
  return(FALSE);
}

/*
 * Triangle intersection
 */
// p = a*p0 + b*p1 + g*p2
// 2 linear equations (x,y)
// 1 = a + b + g;
// a > 0
// b > 0
// g > 0
// TODO -> Two ways -> if v2.y or v2.x are 0
bool intersect_triangle_point(VECT2DI *p_p0, VECT2DI *p_p1, VECT2DI *p_p2, VECT2DI *p_p)
{
  VECT2DI v1,v2;
  
  v1 = *p_p1 - *p_p0;
  v2 = *p_p2 - *p_p0;
  
  // Y is bigger than X
  if(abs(v2.y) < abs(v2.x)) {  
    float tmp = (float)v2.y/(float)v2.x;
    
    float b = (p_p0->y - p_p->y + tmp*p_p->x - tmp*p_p0->x) / (v1.x*tmp - v1.y);  
    if(b < 0.0f || b > 1.0f)
      return(FALSE);
    
    float g = (p_p->x - p_p0->x - b*v1.x) / (float)(v2.x);
    return(g > 0.0f && b+g < 1.0f);
  }
  else {
    float tmp = ((float)v2.x/(float)v2.y);
    
    float b = (p_p0->x - p_p->x + tmp*p_p->y - tmp*p_p0->y) / (v1.y*tmp - v1.x);
    if(b < 0.0f || b > 1.0f)
      return(FALSE);
    
    float g = (p_p->y - p_p0->y - b*v1.y) / (float)(v2.y);
    return(g > 0.0f && b+g < 1.0f);  
  }
}

bool intersect_triangle_ray(VECT2DI *p_p0, VECT2DI *p_p1, VECT2DI *p_p2,
                             VECT2DI *p_a, VECT2DI *p_b,
                             float *p_tn)
{
  if(intersect_triangle_point(p_p0, p_p1, p_p2, p_a)) {
    if(p_tn)
      *p_tn = 0.0f;
    return(TRUE);
  }
  
  if(p_tn) {
    float t,tn = FLT_MAX;
    if(intersect_ray_line(p_a, p_b, p_p0, p_p1, NULL, &t)) {
      if(t > 0.0f && t < tn) {
        tn = t;
      }
    }    
    if(intersect_ray_line(p_a, p_b, p_p0, p_p2, NULL, &t)) {
      if(t > 0.0f && t < tn) {
        tn = t;
      }
    }    
    if(intersect_ray_line(p_a, p_b, p_p1, p_p2, NULL, &t)) {
      if(t > 0.0f && t < tn) {
        tn = t;
      }
    }
    return(tn < FLT_MAX);
  }
  else {
    return( intersect_ray_line(p_a, p_b, p_p0, p_p1, NULL, NULL) ||
            intersect_ray_line(p_a, p_b, p_p0, p_p2, NULL, NULL) ||
            intersect_ray_line(p_a, p_b, p_p1, p_p2, NULL, NULL));
  }
}


bool intersect_triangle_line(VECT2DI *p_p0, VECT2DI *p_p1, VECT2DI *p_p2, 
                             VECT2DI *p_a, VECT2DI *p_b)
{
  float tn;
  if(intersect_triangle_ray(p_p0,p_p1,p_p2,p_a,p_b,&tn)) {
    return(tn <= 1.0f);
  }
  return(FALSE);
}

bool intersect_triangle_ray_cull(VECT *p_v0, VECT *p_v1, VECT *p_v2, // Triangle
                                 VECT *p_orig, VECT *p_dir,          // Line
                                 float *p_t, float *p_u, float *p_v) // Results
{
  VECT  edge1,edge2,tvec,pvec,qvec;
  float det, inv_det;
  float t,u,v;

  // spocitam ramena trojuhelnika
  edge1.x = p_v1->x - p_v0->x;
  edge1.y = p_v1->y - p_v0->y;
  edge1.z = p_v1->z - p_v0->z;

  edge2.x = p_v2->x - p_v0->x;
  edge2.y = p_v2->y - p_v0->y;
  edge2.z = p_v2->z - p_v0->z;

  //  determinant - uhel mezi plochou dir&edge2 a vektorem edge1
  //  spocitam rovinu mezi dir&edge2
  pvec.x = p_dir->y*edge2.z - p_dir->z*edge2.y;
  pvec.y = p_dir->z*edge2.x - p_dir->x*edge2.z;
  pvec.z = p_dir->x*edge2.y - p_dir->y*edge2.x;

  // uhel mezi normalovym vektorem plochy dir&edge2 a edge1
  det = edge1.x*pvec.x + edge1.y*pvec.y + edge1.z*pvec.z;

  // lezi paprsek v rovine plosky (je rovnobezny s ploskou ?)
  // uhel mezi paprskem a ploskou je det
  // culling off
  if(det > -EPSILON && det < EPSILON)
    return(FALSE);

  // t = 0->orig
  tvec.x = p_orig->x - p_v0->x;
  tvec.y = p_orig->y - p_v0->y;
  tvec.z = p_orig->z - p_v0->z;

  // uhel mezi t a p
  u = tvec.x*pvec.x + tvec.y*pvec.y + tvec.z*pvec.z;
  if( u < 0.0f || u > det)
    return(FALSE);

  // plocha mezi tvec a edge1
  qvec.x = tvec.y*edge1.z - tvec.z*edge1.y;
  qvec.y = tvec.z*edge1.x - tvec.x*edge1.z;
  qvec.z = tvec.x*edge1.y - tvec.y*edge1.x;

  // uhel mezi p_dir a qvec
  v = (p_dir->x*qvec.x + p_dir->y*qvec.y + p_dir->z*qvec.z);

  // test hranic s = u, t = v
  if(v < 0.0f || u + v > det)
    return(FALSE);

  // spocitani t parametru
  if(p_t || p_u || p_v) {
    t = (edge2.x*qvec.x + edge2.y*qvec.y + edge2.z*qvec.z);    
    inv_det = 1.0f / det;
    
    if(p_u)
      *p_u = u*inv_det;
    if(p_v)
      *p_v = v*inv_det;
    if(p_t)
      *p_t = t*inv_det;
  }
    
  return(TRUE);
}

bool intersect_triangle_ray_ncull(VECT *p_v0, VECT *p_v1, VECT *p_v2, 
                                  VECT *p_orig, VECT *p_dir, 
                                  float *p_t, float *p_u, float *p_v)
{
  VECT  edge1,edge2,tvec,pvec,qvec;
  float det, inv_det;
  float t,u,v;

  // spocitam ramena trojuhelnika
  edge1.x = p_v1->x - p_v0->x;
  edge1.y = p_v1->y - p_v0->y;
  edge1.z = p_v1->z - p_v0->z;

  edge2.x = p_v2->x - p_v0->x;
  edge2.y = p_v2->y - p_v0->y;
  edge2.z = p_v2->z - p_v0->z;

  //  determinant - uhel mezi plochou dir&edge2 a vektorem edge1
  //  spocitam rovinu mezi dir&edge2
  pvec.x = p_dir->y*edge2.z - p_dir->z*edge2.y;
  pvec.y = p_dir->z*edge2.x - p_dir->x*edge2.z;
  pvec.z = p_dir->x*edge2.y - p_dir->y*edge2.x;

  // uhel mezi normalovym vektorem plochy dir&edge2 a edge1
  det = edge1.x*pvec.x + edge1.y*pvec.y + edge1.z*pvec.z;

  // lezi paprsek v rovine plosky (je rovnobezny s ploskou ?)
  // uhel mezi paprskem a ploskou je det
  // culling off
  if(det > -EPSILON && det < EPSILON)
    return(FALSE);
  inv_det = 1.0f / det;

  // t = 0->orig
  tvec.x = p_orig->x - p_v0->x;
  tvec.y = p_orig->y - p_v0->y;
  tvec.z = p_orig->z - p_v0->z;

  // uhel mezi t a p
  u = (tvec.x*pvec.x + tvec.y*pvec.y + tvec.z*pvec.z)*inv_det;
  if( u < 0.0f || u > 1.0f)
    return(FALSE);

  // plocha mezi tvec a edge1
  qvec.x = tvec.y*edge1.z - tvec.z*edge1.y;
  qvec.y = tvec.z*edge1.x - tvec.x*edge1.z;
  qvec.z = tvec.x*edge1.y - tvec.y*edge1.x;

  // uhel mezi p_dir a qvec
  v = (p_dir->x*qvec.x + p_dir->y*qvec.y + p_dir->z*qvec.z)*inv_det;

  // test hranic s = u, t = v
  if(v < 0.0f || u + v > 1.0f)
    return(FALSE);

  // spocitani t parametru
  t = (edge2.x*qvec.x + edge2.y*qvec.y + edge2.z*qvec.z)*inv_det;
  
  // spocitani t parametru  
  if(p_u)
    *p_u = u;
  if(p_v)
    *p_v = v;
  if(p_t)
    *p_t = t;  
  
  return(TRUE);
}

bool intersect_triangle_line_cull(VECT *p_v0, VECT *p_v1, VECT *p_v2,
                                  VECT *p_orig, VECT *p_dir,
                                  float *p_t, float *p_u, float *p_v)
{
  float t;
  if(intersect_triangle_ray_cull(p_v0,p_v1,p_v2,p_orig,p_dir,&t,p_u,p_v)) {
    if(t <= 1.0f) {
      if(p_t)
        *p_t = t;
      return(TRUE);
    }      
  }
  return(FALSE);
}

bool intersect_triangle_line_ncull(VECT *p_v0, VECT *p_v1, VECT *p_v2,
                                   VECT *p_orig, VECT *p_dir,
                                   float *p_t, float *p_u, float *p_v)
{
  float t;
  if(intersect_triangle_ray_ncull(p_v0,p_v1,p_v2,p_orig,p_dir,&t,p_u,p_v)) {
    if(t <= 1.0f) {
      if(p_t)
        *p_t = t;
      return(TRUE);
    }      
  }
  return(FALSE);
}

/*
 * Rect and Point/Line/Ray intersection
 */

INTERSECTION_TYPE intersect_rect_ray(RECT2DI *p_r, VECT2DI *p_a, VECT2DI *p_b, float *p_tn, float *p_tf)
{  
  if(intersect_rect_point(p_r, p_a) || intersect_rect_point(p_r, p_b)) {
    return(INTERSECT_INSIDE);
  }
  
  // line a->b  
  float   tn = 0.0f,tf = FLT_MAX;
  float   t1,t2,t;
  VECT2DI Q;  

  // Q is vector p_a->p_b
  Q = *p_b - *p_a;

  // X-plane
  t1 = p_r->x+p_r->dx-p_a->x; // Tf
  t2 = p_r->x-p_a->x;         // Tn
  if(Q.x < 0.0f) {
    t = t1;
    t1 = t2;
    t2 = t;
  }
  tn = MAX(t2/((float)Q.x),tn);
  tf = MIN(t1/((float)Q.x),tf);
  if(tn > tf || tf < 0.0f)
    return(INTERSECT_NONE);  

  // Y-Souradnice
  t1 = p_r->y+p_r->dy-p_a->y; // defaultne Tf
  t2 = p_r->y-p_a->y; // defaultne Tn
  if(Q.y < 0.0f) {
    t = t1;
    t1 = t2;
    t2 = t;
  }
  tn = MAX(t2/((float)Q.y),tn);
  tf = MIN(t1/((float)Q.y),tf);
  if(tn > tf || tf < 0.0f)
    return(INTERSECT_NONE);

  if(p_tn && p_tf) {
    *p_tn = tn;
    *p_tf = tf;
  }
  
  return(INTERSECT_CROSS);
}

INTERSECTION_TYPE intersect_rect_line(RECT2DI *p_r, VECT2DI *p_a, VECT2DI *p_b, float *p_tn, float *p_tf)
{  
  if(intersect_rect_point(p_r, p_a) || intersect_rect_point(p_r, p_b)) {
    return(INTERSECT_INSIDE);
  }
  
  // line a->b  
  float   tn = 0.0f,tf = FLT_MAX;
  float   t1,t2,t;
  VECT2DI Q;

  // Q is vector p_a->p_b
  Q = (*p_b) - (*p_a);

  // X-plane
  t1 = p_r->x+p_r->dx-p_a->x; // Tf
  t2 = p_r->x-p_a->x;         // Tn
  if(Q.x < 0.0f) {
    t = t1;
    t1 = t2;
    t2 = t;
  }
  tn = MAX(t2/((float)Q.x),tn);
  tf = MIN(t1/((float)Q.x),tf);
  if(tn > tf || tf < 0.0f)
    return(INTERSECT_NONE);  

  // Y-Souradnice
  t1 = p_r->y+p_r->dy-p_a->y; // defaultne Tf
  t2 = p_r->y-p_a->y; // defaultne Tn
  if(Q.y < 0.0f) {
    t = t1;
    t1 = t2;
    t2 = t;
  }
  tn = MAX(t2/((float)Q.y),tn);
  tf = MIN(t1/((float)Q.y),tf);
  if(tn > tf || tf < 0.0f)
    return(INTERSECT_NONE);

  if(tn > 1.0f)
    return(INTERSECT_NONE);
  
  if(p_tn && p_tf) {
    *p_tn = tn;
    *p_tf = tf;
  }
  
  return(INTERSECT_CROSS);
}

INTERSECTION_TYPE intersect_rect_triangle(RECT2DI *p_r, VECT2DI *p_v0, VECT2DI *p_v1, VECT2DI *p_v2)
{ 
  int ret = intersect_rect_point(p_r, p_v0)+intersect_rect_point(p_r, p_v1)+
            intersect_rect_point(p_r, p_v2);
  
  if(ret) {
    return(ret == 3 ? INTERSECT_INSIDE : INTERSECT_CROSS);
  }
  
  VECT2DI tmp[4];  
  p_r->get(tmp);
    
  // Is the rectangle inside?
  if(intersect_triangle_point(p_v0,p_v1,p_v2,tmp)   || 
     intersect_triangle_point(p_v0,p_v1,p_v2,tmp+1) ||
     intersect_triangle_point(p_v0,p_v1,p_v2,tmp+2) ||
     intersect_triangle_point(p_v0,p_v1,p_v2,tmp+3))
  {
    return(INTERSECT_CROSS);
  }

  if(intersect_rect_line(p_r, p_v0, p_v1) ||
     intersect_rect_line(p_r, p_v1, p_v2) ||
     intersect_rect_line(p_r, p_v2, p_v1)) 
  {
    return(INTERSECT_CROSS );
  }
  else {
    return(INTERSECT_NONE);
  }
}

INTERSECTION_TYPE intersect_rect_rect(RECT2DI *p_r, RECT2DI *p_r2)
{
  VECT2DI tmp[4];
  
  p_r2->get(tmp);  
  int ret = intersect_rect_point(p_r, tmp)+
            intersect_rect_point(p_r, tmp+1)+
            intersect_rect_point(p_r, tmp+2)+
            intersect_rect_point(p_r, tmp+3);
  
  if(ret) {
    return(ret == 4 ? INTERSECT_INSIDE : INTERSECT_CROSS);
  }
 
  p_r->get(tmp);
  if(intersect_rect_point(p_r2, tmp)    ||
     intersect_rect_point(p_r2, tmp+1)  ||
     intersect_rect_point(p_r2, tmp+2)  ||
     intersect_rect_point(p_r2, tmp+3)) 
  {
    return(INTERSECT_CROSS);
  }
  else {  
    return(INTERSECT_NONE);
  }
}
