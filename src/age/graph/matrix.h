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
  Matrix module
  
  TODO -> rewrite to some reasonable function format (matrix_*)
  matrix.methods only for simple methods!!
  
  - optimalization -> pointers (->x) to local values (x)
  
*/
#ifndef __MATRIX_H__
#define __MATRIX_H__

#include <assert.h>
#include <string.h>

// Matrix definition
typedef class glmatrix {

  /*
   * Data definitions
   */
public:
  
  float _11,_12,_13,_14;
  float _21,_22,_23,_24;
  float _31,_32,_33,_34;
  float _41,_42,_43,_44;

  /*
   * Initialization
   */
public: 
  
  glmatrix * zero(void)
  {
    _11 = _22 = _33 = _44 = 0;
    _12 = _13 = _14 = _41 = 0;
    _21 = _23 = _24 = _42 = 0;
    _31 = _32 = _34 = _43 = 0;
    return(this);
  } 

  glmatrix * init(void)
  {
    _11 = _22 = _33 = _44 = 1;
    _12 = _13 = _14 = _41 = 0;
    _21 = _23 = _24 = _42 = 0;
    _31 = _32 = _34 = _43 = 0; 
    return(this);
  }

  /*
   * Algebra
   */
public: 

  glmatrix * invert(void)
  {
    float det;
    
    float t11 =  _22*_33-_23*_32;
    float t12 = -_12*_33+_13*_32;
    float t13 =  _12*_23-_13*_22;
    
    det = t11*_11+t12*_21+t13*_31;
    
    if(det != 0.0f) {
      
      float t21 = - _21 * _33 + _23 * _31;
      float t22 =   _11 * _33 - _13 * _31;
      float t23 = - _11 * _23 + _13 * _21;
      
      float t31 =   _21 * _32 - _22 * _31;
      float t32 = - _11 * _32 + _12 * _31;
      float t33 =   _11 * _22 - _12 * _21;
      
      _11 = t11 / det; _12 = t12 / det; _13 = t13 / det; 
      _21 = t21 / det; _22 = t22 / det; _23 = t23 / det; 
      _31 = t31 / det; _32 = t32 / det; _33 = t33 / det; 
      
      float t41 = - _41*_11 - _42*_21 - _43*_31;
      float t42 = - _41*_12 - _42*_22 - _43*_32;
      float t43 = - _41*_13 - _42*_23 - _43*_33;
      
      _41 = t41;
      _42 = t42;
      _43 = t43;
      
      _14 = 0;
      _24 = 0;
      _34 = 0;
      _44 = 1;
    }
    return(this);
  }

  class glmatrix * reflexion(VECT *p_a, VECT *p_b, VECT *p_c)
  {
    float  e;
    QUAT   q;
    
    q.x = (p_b->y - p_a->y)*(p_c->z - p_a->z)-(p_c->y - p_a->y)*(p_b->z - p_a->z);
    q.y = (p_b->z - p_a->z)*(p_c->x - p_a->x)-(p_c->z - p_a->z)*(p_b->x - p_a->x);
    q.z = (p_b->x - p_a->x)*(p_c->y - p_a->y)-(p_c->x - p_a->x)*(p_b->y - p_a->y);
        
    ((VECT)q).norm();
    q.w = 0.0f;
    e = 2*(q.x*p_a->x + q.y*p_a->y + q.z*p_a->z);
         
    /*
      Natoceni souradnic dle vektoru q
    */    
    rotation_set(&q);

    _14 = _24 = _34 = 0.0f; _44 = 1.0f;
    
    /*
      Posun souradnic do pozadovane roviny
    */
    translate(q.x*e,q.y*e,q.z*e);
    
    /*
      Reflexivituj souradnice
    */
    scale(-1.0f,-1.0f,-1.0f);
    
    return(this);
  }
                      
  glmatrix * mult(float scale)
  {
    _11 *= scale;
    _12 *= scale;
    _13 *= scale;
    _14 *= scale;
    
    _21 *= scale;
    _22 *= scale;
    _23 *= scale;
    _24 *= scale;
    
    _31 *= scale;
    _32 *= scale;
    _33 *= scale;
    _34 *= scale;
    
    _41 *= scale;
    _42 *= scale;
    _43 *= scale;
    _44 *= scale;      

    return(this);
  }
  
  float determinant(void)
  {
    float m1 =   _22 * _33 - _23 * _32;
    float m2 = - _12 * _33 + _13 * _32;
    float m3 =   _12 * _23 - _13 * _22;
    return((m1 * _11 + m2 * _21 + m3 * _31));
  }
  
  bool regular(void)
  {    
    return(determinant() != 0);
  }
  
public:
  
  /*
   * Translations
   */
  glmatrix * translate(float px, float py, float pz)
  {
    _41 += px*_11 + py*_21 + pz*_31;
    _42 += px*_12 + py*_22 + pz*_32;
    _43 += px*_13 + py*_23 + pz*_33;
    _44 += px*_14 + py*_24 + pz*_34;
    
    return(this);
  }

  glmatrix * translate(VECT *p_pos)
  {
    return(translate(p_pos->x, p_pos->y, p_pos->z));
  }

  glmatrix * translation_set(VECT *p_pos)
  {
    _41 = p_pos->x;
    _42 = p_pos->y;
    _43 = p_pos->z;    
    return(this);
  }
  
  glmatrix * translation_set(float px, float py, float pz)
  {
    _41 = px;
    _42 = py;
    _43 = pz;    
    return(this);
  }

  glmatrix * translation_get(VECT *p_trans)
  {
    p_trans->x = _41;
    p_trans->y = _42;
    p_trans->z = _43;
    return(this);
  }
  
  glmatrix * translation_add(VECT *p_pos)
  {
    _41 += p_pos->x;
    _42 += p_pos->y;
    _43 += p_pos->z;    
    return(this);
  }

  glmatrix * translation_add(float px, float py, float pz)
  {
    _41 += px;
    _42 += py;
    _43 += pz;    
    return(this);
  }
  
public:
  
  /*
   * Rotations
   */
  glmatrix * rotation_set(PLANE *p_r, float scale)
  {
    float x = p_r->x;
    float y = p_r->y;
    float z = p_r->z;
    float e = p_r->e;
    
    _11 = x*x*scale;
    _21 = x*y*scale;
    _31 = x*z*scale;
    _41 = x*e*scale;
    
    _12 = y*x*scale;
    _22 = y*y*scale;
    _32 = y*z*scale;
    _42 = y*e*scale;
    
    _13 = z*x*scale;
    _23 = z*y*scale;
    _33 = z*z*scale;
    _43 = z*e*scale;
    
    _14 = e*x*scale;
    _24 = e*y*scale;
    _34 = e*z*scale;
    _44 = e*e*scale;
    
    return(this);
  }
  
  glmatrix * rotation_set(VECT *p_vx, VECT *p_vy, VECT *p_vz)
  {
    _11 = p_vx->x; _21 = p_vx->y; _31 = p_vx->z;
    _12 = p_vy->x; _22 = p_vy->y; _32 = p_vy->z;
    _13 = p_vz->x; _23 = p_vz->y; _33 = p_vz->z;
    _14 = _24 = _34 = 0.0f;
    _41 = _42 = _43 = 0.0f;
    _44 = 1.0f;    
    
    return(this);
  }

  glmatrix * rotation_set(VECT *p_vx, VECT *p_vy, VECT *p_vz, VECT *p_pos)
  {
    _11 = p_vx->x; _21 = p_vx->y; _31 = p_vx->z;
    _12 = p_vy->x; _22 = p_vy->y; _32 = p_vy->z;
    _13 = p_vz->x; _23 = p_vz->y; _33 = p_vz->z;
    _14 = _24 = _34 = 0.0f;
    _41 = p_pos->x;
    _42 = p_pos->y;
    _43 = p_pos->z;
    _44 = 1.0f;  
    
    return(this);
  }

  /*
      ( 1-2yy-2zz        2xy+2wz          2xz-2wy  )
  M = (  2xy-2wz        1-2xx-2zz         2yz+2wx  )
      (  2xz+2wy         2yz-2wx         1-2xx-2yy )
  */
  glmatrix * rotation_set(QUAT *p_q)
  {
    float xx2 = 2*p_q->x*p_q->x;
    float yy2 = 2*p_q->y*p_q->y;
    float zz2 = 2*p_q->z*p_q->z;
    
    // vektor X
    _11 = 1.0f-yy2-zz2;
    _21 = 2*p_q->x*p_q->y - 2*p_q->w*p_q->z;
    _31 = 2*p_q->x*p_q->z + 2*p_q->w*p_q->y;
    
    // vektor Y
    _12 = 2*p_q->x*p_q->y + 2*p_q->w*p_q->z;
    _22 = 1.0f-xx2-zz2;
    _32 = 2*p_q->y*p_q->z - 2*p_q->w*p_q->x;
    
    // vektor Z
    _13 = 2*p_q->x*p_q->z - 2*p_q->w*p_q->y;
    _23 = 2*p_q->y*p_q->z + 2*p_q->w*p_q->x;
    _33 = 1.0f-xx2-yy2;
    
    return(this);
  }

  QUAT * rotation_get(QUAT *p_q)
  {
    // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
    // article "Quaternion Calculus and Fast Animation".
    
    float diagonala = _11+_22+_33;
    float root;
    
    if(diagonala > 0.0f) {
      
      // |w| > 1/2, may as well choose w > 1/2
      root = sqrtf(diagonala + 1.0f);  // 2w
      p_q->w = 0.5f*root;
      root = 0.5f/root;  // root = 1/(4w)
      
      p_q->x = (_23-_32)*root;
      p_q->y = (_31-_13)*root;
      p_q->z = (_12-_21)*root;
      
      return(p_q);
    } else {
      // |w| <= 1/2
      float  t;
      int    i;
      
      if(_22 > _11) {
        t = _22;
        i = 2;
      } else {
        t = _11;
        i = 1;
      }
      if(_33 > t) {
        i = 3;
      }
      
      // i je nejvetsi a pak j a k sou ty 2 zbyvajici
      // fRoot = sqrtf(kRot[i][i]-kRot[j][j]-kRot[k][k] + 1.0f);
      switch(i) {
      case 1: // x je nejvetsi -> 4*x^2 j = 2 k = 3
        root = sqrtf(_11 - _22 - _33 + 1.0f);
        p_q->x = 0.5f*root;
        root = 0.5f/root; // 1/(4*x)
        
        p_q->w = (_23-_32)*root; //kRot[k][j]-kRot[j][k];
        p_q->y = (_12+_21)*root; //kRot[j][i]+kRot[i][j];
        p_q->z = (_13+_31)*root; //kRot[k][i]+kRot[i][k];
        
        break;
      case 2: // y je nejvetsi -> 4*y^2 j = 3 k = 1
        root = sqrtf(_22 - _33 - _11 + 1.0f);
        p_q->y = 0.5f*root;
        root = 0.5f/root; // 1/(4*y)
        
        p_q->w = (_31-_13)*root;  //(kRot[k][j]-kRot[j][k];
        p_q->z = (_23+_32)*root;  //(kRot[j][i]+kRot[i][j];
        p_q->x = (_21+_12)*root;  //(kRot[k][i]+kRot[i][k];
        
        break;
      case 3: // z je nejvetsi -> 4*z^2 i = 3 j = 1 k = 2
        root = sqrtf(_33 - _11 - _22 + 1.0f);
        p_q->z = 0.5f*root;
        root = 0.5f/root; // 1/(4*z)
        
        p_q->w = (_12-_21)*root; //(kRot[k][j]-kRot[j][k];
        p_q->x = (_31+_13)*root; //(kRot[j][i]+kRot[i][j];
        p_q->y = (_32+_23)*root; //(kRot[k][i]+kRot[i][k];
        
        break;
      }
      
      return(p_q);
    }    
  }
  
  glmatrix * rotation_set(VECT *p_axis, float angle)
  {
    QUAT q(p_axis,angle);
    return(rotation_set(&q));
  }
  
  glmatrix * rotate_x(float angle)
  {
    float  ssin = (float)sinf(angle);
    float  ccos = (float)cosf(angle);

    float t21 = _21;    
    float t22 = _22;    
    float t23 = _23;    
    float t24 = _24;    
    
    _21 = ccos*t21 + ssin*_31;
    _31 =-ssin*t21 + ccos*_31;
            
    _22 = ccos*t22 + ssin*_32;
    _32 =-ssin*t22 + ccos*_32;
            
    _23 = ccos*t23 + ssin*_33;
    _33 =-ssin*t23 + ccos*_33;
            
    _24 = ccos*t24 + ssin*_34;
    _34 =-ssin*t24 + ccos*_34;

    return(this);
  }

  glmatrix * rotate_y(float angle)
  {
    float  ssin = (float)sinf(angle);
    float  ccos = (float)cosf(angle);
    
    float t11 = _11;
    float t12 = _12;
    float t13 = _13;
    float t14 = _14;
    
    _11 = ccos*t11 + ssin*_31;
    _31 =-ssin*t11 + ccos*_31;
    
    _12 = ccos*t12 + ssin*_32;
    _32 =-ssin*t12 + ccos*_32;
    
    _13 = ccos*t13 + ssin*_33;
    _33 =-ssin*t13 + ccos*_33;
    
    _14 = ccos*t14 + ssin*_34;
    _34 =-ssin*t14 + ccos*_34;

    return(this);
  }

  glmatrix * rotate_z(float angle)
  {    
    float  ssin = (float)sinf(angle);
    float  ccos = (float)cosf(angle);
    
    float t11 = _11;
    float t12 = _12;
    float t13 = _13;
    float t14 = _14;

    _11 = ccos*t11 + ssin*_21;
    _21 =-ssin*t11 + ccos*_21;
    
    _12 = ccos*t12 + ssin*_22;
    _22 =-ssin*t12 + ccos*_22;
    
    _13 = ccos*t13 + ssin*_23;
    _23 =-ssin*t13 + ccos*_23;
    
    _14 = ccos*t14 + ssin*_24;
    _24 =-ssin*t14 + ccos*_24;

    return(this);
  }

  glmatrix * rotation_set_z(float angle)
  {      
    _33 = _44 = 1;
    _13 = _14 = _41 = 0;
    _23 = _24 = _42 = 0;
    _31 = _32 = _34 = _43 = 0; 
    
    float  sinus   = (float)sinf(angle);
    float  cosinus = (float)cosf(angle);

    _11 = cosinus;
    _12 = sinus;
    _22 = cosinus;
    _21 = -sinus;

    return(this);
  }

  glmatrix * rotation_set_inverse(VECT *p_vx, VECT *p_vy, VECT *p_vz)
  {
    rotation_set(p_vx,p_vy,p_vz);
    return(invert());
  }
  
  glmatrix * rotation_set_inverse(VECT *p_vx, VECT *p_vy, VECT *p_vz, VECT *p_pos)
  {
    rotation_set(p_vx,p_vy,p_vz,p_pos);
    return(invert());
  }
  
  // 2D rotace
  glmatrix * rotate_2D(VECT *p_pivot, float angle)
  {
    glmatrix m2;
    float cosa = (float)cosf(angle);
    float sina = (float)sinf(angle);
    float px,py;
    
    
    if(!p_pivot) {
      _11 = cosa;
      _22 = cosa;
      _21 = sina;
      _12 = -sina;
    } else {
      
      px = -p_pivot->x;
      py = -p_pivot->y;
      
      /*
      posun matice a*b -> tmp
      a = pos matrix
      b = rot matrix
      */
      m2._11 = cosa;
      m2._21 = sina;
      m2._41 = px*cosa + py*sina + _41;
      
      m2._12 = -sina;
      m2._22 = cosa;
      m2._42 = px*(-sina) + py*cosa + _42;   
      
      /*
      a = rot
      b = pos - 1
      */
      _11 = m2._11;
      _21 = m2._21;
      _41 = m2._41 + _44*(-px);
      
      _12 = m2._12;
      _22 = m2._22;
      _42 = m2._42 + _44*(-py);
    }
    
    return(this);
  }
  
  /*
   * Scaling
   */
public:
  
  glmatrix * scale(float x, float y, float z)
  {
    _11 *= x;
    _21 *= x;
    _31 *= x;
    
    _12 *= y;
    _22 *= y;
    _32 *= y;
    
    _13 *= z;
    _23 *= z;
    _33 *= z;

    return(this);
  }
  
  glmatrix * scale_set(float x, float y, float z)
  {
    _11 = x;
    _22 = y;
    _33 = z;

    return(this);
  }
  
  glmatrix * scale(VECT *p_scale)
  {
    return(scale(p_scale->x,p_scale->y,p_scale->z));
  }

  glmatrix * scale_set(VECT *p_scale)
  {
    return(scale_set(p_scale->x,p_scale->y,p_scale->z));
  }

  glmatrix * scale_get(VECT *p_scale)
  {
    p_scale->x = vect_norm(&_11,&_21,&_31);
    p_scale->y = vect_norm(&_12,&_22,&_32);
    p_scale->z = vect_norm(&_13,&_23,&_33);
    return(this);
  }
  
  glmatrix * scale_get(float *p_x, float *p_y, float *p_z)
  {
    *p_x = vect_norm(&_11,&_21,&_31);
    *p_y = vect_norm(&_12,&_22,&_32);
    *p_z = vect_norm(&_13,&_23,&_33);
    return(this);
  }

  
  /*
   * Projection matrixes
   */
private:
  
  glmatrix * buildFrustumMatrix(float l, float r, float b, float t, float n, float f)
  {
    _11 = (2.0f*n)/(r-l);
    _12 = 0.0f;
    _13 = 0.0f;
    _14 = 0.0f;
    
    _21 = 0.0f;
    _22 = (2.0f*n)/(t-b);
    _23 = 0.0f;
    _24 = 0.0f;
    
    _31 = (r+l)/(r-l);
    _32 = (t+b)/(t-b);
    _33 = (f+n)/(f-n);
    _34 = 1.0f;
    
    _41 = 0.0f;
    _42 = 0.0f;
    _43 = -(2.0f*f*n)/(f-n);
    _44 = 0.0f;
    
    return(this);
  }
 
/* Common values
  #define DEF_FOVY       30.0f
  #define MAX_DIST_Z     50.0f
  #define MIN_DIST_Z     1.0f
  
  Build a transformation matrix
  
  It transformates world space coordinated to perspective
  All coordinates are from -1 to 1
  Z coordinates are transfromated from (z_near,z_far) -> (-1,1)
  (distribution is logarithmical)  
  
  X2d =   (2n/(r-l))*X3d  +       0*Y3d       +   ((r+l)/(r-l))*Z3d   +     0
  Y2d =       0*X3d       +   (2n/(t-b))*Y3d  +   ((t+b)/(t-b))*Z3d   +     0
  Z2d =       0*X3d       +       0*Y3d       +   ((f+n)/(f-n))*Z3d   -   (2fn/(f-n))*W3d
  W2d =       0*X3d       +       0*Y3d       +         Z3d           +     0
*/  
public:   

  glmatrix * projection(float fovy, float aspect, float zNear, float zFar)
  {
    float xmin, xmax, ymin, ymax;
    
    ymax = zNear * (float)tan(fovy);
    ymin = -ymax;
    
    xmin = ymin * aspect;
    xmax = ymax * aspect;
    
    return(buildFrustumMatrix(xmin, xmax, ymin, ymax, zNear, zFar));
  }

public:   
   
  PLANE * plane_get(PLANE *p_r)
  {
    p_r->x = (float)sqrt(_11);
    p_r->y = (float)sqrt(_22);
    p_r->z = (float)sqrt(_33);
    p_r->e = (float)sqrt(_44);
    return(p_r);
  }
  
  /*
   * Pivot transformations
   */
public:   
  
  glmatrix * pivot(VECT *p_pivot)
  {
    float    px = -p_pivot->x, py = -p_pivot->y, pz = -p_pivot->z;
    float    m14, m24, m34, m41, m42, m43, m44;  
    
    if(px == 0.0f && py == 0.0f && pz == 0.0f)
      return(this);
    
    // a = translate
    // b = p_mat
    m41 = px*_11 + py*_21 + pz*_31 + _41;
    m42 = px*_12 + py*_22 + pz*_32 + _42;
    m43 = px*_13 + py*_23 + pz*_33 + _43;
    m44 = px*_14 + py*_24 + pz*_34 + _44;
    
    m14 = _14;
    m24 = _24;
    m34 = _34;
    
    // posun zpet
    // a = mat
    // b = translate -
    px = -px;
    py = -py;
    pz = -pz;
    _11 += m14*(px);
    _21 += m24*(px);
    _31 += m34*(px);
    _41 =  m41 + m44*(px);
    
    _12 += m14*(py);
    _22 += m24*(py);
    _32 += m34*(py);
    _42  = m42 + m44*(py);
    
    _13 += m14*(pz);
    _23 += m24*(pz);
    _33 += m34*(pz);
    _43  = m43 + m44*(pz);
    
    _44 = m44;
    
    return(this);
  }
  
  glmatrix * pivot_forward(VECT *p_pivot)
  {
    float    px = -p_pivot->x, py = -p_pivot->y, pz = -p_pivot->z;
    
    if(px == 0.0f && py == 0.0f && pz == 0.0f)
      return(this);
    else {
      // a = translate
      // b = p_mat
      _41 += px*_11 + py*_21 + pz*_31;
      _42 += px*_12 + py*_22 + pz*_32;
      _43 += px*_13 + py*_23 + pz*_33;
      _44 += px*_14 + py*_24 + pz*_34;
      
      return(this);
    }
  }

public:  
  
  VECT * transformate(VECT *p_p, int num = 1)
  {
    VECT *p_ret = p_p;
    float x,y,z;
  
    int i;
    for(i = 0; i < num; i++, p_p++) {
      x = p_p->x; y = p_p->y; z = p_p->z;
      
      p_p->x = _11*x + _21*y + _31*z + _41;
      p_p->y = _12*x + _22*y + _32*z + _42;
      p_p->z = _13*x + _23*y + _33*z + _43;
    }
    
    return(p_ret);
  }

public:
  
  void print(const char *p_name = NULL, int ident = 0);

  
  /*
   * Constructors
   */
public:

  glmatrix(void) {};

  glmatrix(PLANE *p_r, float scale)
  {
    rotation_set(p_r,scale);
  }
  
  glmatrix(VECT *p_axis, float angle)
  {
    rotation_set(p_axis, angle);
  }
  
  glmatrix(VECT *p_pos, QUAT *p_rot, VECT *p_scs)
  {
    _14 = _24 = _34 = 0.0f;
    _44 = 1.0f;

    if(p_pos) {
      _41 = p_pos->x;
      _42 = p_pos->y;
      _43 = p_pos->z;
    } else {
      _41 = 0.0f;
      _42 = 0.0f;
      _43 = 0.0f;
    }

    if(p_rot) {
      rotation_set(p_rot);
    } else {
      _12 = _13 = _23 = _21 = _31 = _32 = 0.0f;
      _22 = _33 = _11 = 1.0f;
    }
    
    if(p_scs) {
      scale(p_scs);
    }
  }
  
  glmatrix(VECT *p_position)
  {
    _11 = _22 = _33 = _44 = 1;
    _12 = _13 = _14 = 0;
    _21 = _23 = _24 = 0;
    _31 = _32 = _34 = 0;
    
    translation_set(p_position);    
  }
  
} GLMATRIX;

inline GLMATRIX * matrix_zero(GLMATRIX * m)
{
  memset(m,0,sizeof(*m));
  return(m);
} 

inline GLMATRIX * matrix_init (GLMATRIX *m)
{
  m->_11 = m->_22 = m->_33 = m->_44 = 1;
  m->_12 = m->_13 = m->_14 = m->_41 = 0;
  m->_21 = m->_23 = m->_24 = m->_42 = 0;
  m->_31 = m->_32 = m->_34 = m->_43 = 0;
  return(m);
}

inline GLMATRIX * matrix_translation(GLMATRIX *m, VECT *p_pos)
{
  m->_11 = m->_22 = m->_33 = m->_44 = 1;
  m->_12 = m->_13 = m->_14 = 0;
  m->_21 = m->_23 = m->_24 = 0;
  m->_31 = m->_32 = m->_34 = 0;
  
  m->translation_set(p_pos);
  return(m);
}

//  Nasobeni matic 'a' a 'b' do matice 'r'
//  Alespon doufam po me konverzi :-)
inline GLMATRIX * matrix_mult(GLMATRIX * a, GLMATRIX * b, GLMATRIX * r)
{
  GLMATRIX tmp;

  tmp._11 = a->_11*b->_11 + a->_12*b->_21 + a->_13*b->_31 + a->_14*b->_41;
  tmp._21 = a->_21*b->_11 + a->_22*b->_21 + a->_23*b->_31 + a->_24*b->_41;
  tmp._31 = a->_31*b->_11 + a->_32*b->_21 + a->_33*b->_31 + a->_34*b->_41;
  tmp._41 = a->_41*b->_11 + a->_42*b->_21 + a->_43*b->_31 + a->_44*b->_41;

  tmp._12 = a->_11*b->_12 + a->_12*b->_22 + a->_13*b->_32 + a->_14*b->_42;
  tmp._22 = a->_21*b->_12 + a->_22*b->_22 + a->_23*b->_32 + a->_24*b->_42;
  tmp._32 = a->_31*b->_12 + a->_32*b->_22 + a->_33*b->_32 + a->_34*b->_42;
  tmp._42 = a->_41*b->_12 + a->_42*b->_22 + a->_43*b->_32 + a->_44*b->_42;

  tmp._13 = a->_11*b->_13 + a->_12*b->_23 + a->_13*b->_33 + a->_14*b->_43;
  tmp._23 = a->_21*b->_13 + a->_22*b->_23 + a->_23*b->_33 + a->_24*b->_43;
  tmp._33 = a->_31*b->_13 + a->_32*b->_23 + a->_33*b->_33 + a->_34*b->_43;
  tmp._43 = a->_41*b->_13 + a->_42*b->_23 + a->_43*b->_33 + a->_44*b->_43;

  tmp._14 = a->_11*b->_14 + a->_12*b->_24 + a->_13*b->_34 + a->_14*b->_44;
  tmp._24 = a->_21*b->_14 + a->_22*b->_24 + a->_23*b->_34 + a->_24*b->_44;
  tmp._34 = a->_31*b->_14 + a->_32*b->_24 + a->_33*b->_34 + a->_34*b->_44;
  tmp._44 = a->_41*b->_14 + a->_42*b->_24 + a->_43*b->_34 + a->_44*b->_44;

  memcpy(r,&tmp,sizeof(GLMATRIX));
  return(r);
}

inline GLMATRIX * matrix_mult_dir(GLMATRIX * a, GLMATRIX * b, GLMATRIX * r)
{
  r->_11 = a->_11*b->_11 + a->_12*b->_21 + a->_13*b->_31 + a->_14*b->_41;
  r->_21 = a->_21*b->_11 + a->_22*b->_21 + a->_23*b->_31 + a->_24*b->_41;
  r->_31 = a->_31*b->_11 + a->_32*b->_21 + a->_33*b->_31 + a->_34*b->_41;
  r->_41 = a->_41*b->_11 + a->_42*b->_21 + a->_43*b->_31 + a->_44*b->_41;

  r->_12 = a->_11*b->_12 + a->_12*b->_22 + a->_13*b->_32 + a->_14*b->_42;
  r->_22 = a->_21*b->_12 + a->_22*b->_22 + a->_23*b->_32 + a->_24*b->_42;
  r->_32 = a->_31*b->_12 + a->_32*b->_22 + a->_33*b->_32 + a->_34*b->_42;
  r->_42 = a->_41*b->_12 + a->_42*b->_22 + a->_43*b->_32 + a->_44*b->_42;

  r->_13 = a->_11*b->_13 + a->_12*b->_23 + a->_13*b->_33 + a->_14*b->_43;
  r->_23 = a->_21*b->_13 + a->_22*b->_23 + a->_23*b->_33 + a->_24*b->_43;
  r->_33 = a->_31*b->_13 + a->_32*b->_23 + a->_33*b->_33 + a->_34*b->_43;
  r->_43 = a->_41*b->_13 + a->_42*b->_23 + a->_43*b->_33 + a->_44*b->_43;

  r->_14 = a->_11*b->_14 + a->_12*b->_24 + a->_13*b->_34 + a->_14*b->_44;
  r->_24 = a->_21*b->_14 + a->_22*b->_24 + a->_23*b->_34 + a->_24*b->_44;
  r->_34 = a->_31*b->_14 + a->_32*b->_24 + a->_33*b->_34 + a->_34*b->_44;
  r->_44 = a->_41*b->_14 + a->_42*b->_24 + a->_43*b->_34 + a->_44*b->_44;

  return(r);
}

inline GLMATRIX * matrix_mult(GLMATRIX * r, float scale)
{
  r->_11 *= scale;
  r->_12 *= scale;
  r->_13 *= scale;
  r->_14 *= scale;

  r->_21 *= scale;
  r->_22 *= scale;
  r->_23 *= scale;
  r->_24 *= scale;

  r->_31 *= scale;
  r->_32 *= scale;
  r->_33 *= scale;
  r->_34 *= scale;

  r->_41 *= scale;
  r->_42 *= scale;
  r->_43 *= scale;
  r->_44 *= scale;

  return(r);
}

inline GLMATRIX * matrix_add(GLMATRIX * a, GLMATRIX * b, GLMATRIX * r)
{
  r->_11 = a->_11 + b->_11;
  r->_12 = a->_12 + b->_12;
  r->_13 = a->_13 + b->_13;
  r->_14 = a->_14 + b->_14;

  r->_21 = a->_21 + b->_21;
  r->_22 = a->_22 + b->_22;
  r->_23 = a->_23 + b->_23;
  r->_24 = a->_24 + b->_24;

  r->_31 = a->_31 + b->_31;
  r->_32 = a->_32 + b->_32;
  r->_33 = a->_33 + b->_33;
  r->_34 = a->_34 + b->_34;

  r->_41 = a->_41 + b->_41;
  r->_42 = a->_42 + b->_42;
  r->_43 = a->_43 + b->_43;
  r->_44 = a->_44 + b->_44;

  return(r);
}

inline GLMATRIX * matrix_rotate_x(GLMATRIX * m, float angle)
{
  GLMATRIX r;
  float  ssin = (float)sinf(angle);
  float  ccos = (float)cosf(angle);

  // a = rotate matrix
  // b = m
  r._11 = m->_11;
  r._21 = ccos*m->_21 + ssin*m->_31;
  r._31 =-ssin*m->_21 + ccos*m->_31;
  r._41 = m->_41;

  r._12 = m->_12;
  r._22 = ccos*m->_22 + ssin*m->_32;
  r._32 =-ssin*m->_22 + ccos*m->_32;
  r._42 = m->_42;

  r._13 = m->_13;
  r._23 = ccos*m->_23 + ssin*m->_33;
  r._33 =-ssin*m->_23 + ccos*m->_33;
  r._43 = m->_43;

  r._14 = m->_14;
  r._24 = ccos*m->_24 + ssin*m->_34;
  r._34 =-ssin*m->_24 + ccos*m->_34;
  r._44 = m->_44;

  *m = r;
  return(m);
}

inline GLMATRIX * matrix_rotate_y(GLMATRIX * m, float angle)
{
  GLMATRIX r;
  float  ssin = (float)sinf(angle);
  float  ccos = (float)cosf(angle);

  // a = temp
  // b = m
  r._11 = ccos*m->_11 + ssin*m->_31;
  r._21 = m->_21;
  r._31 =-ssin*m->_11 + ccos*m->_31;
  r._41 = m->_41;

  r._12 = ccos*m->_12 + ssin*m->_32;
  r._22 = m->_22;
  r._32 =-ssin*m->_12 + ccos*m->_32;
  r._42 = m->_42;

  r._13 = ccos*m->_13 + ssin*m->_33;
  r._23 = m->_23;
  r._33 =-ssin*m->_13 + ccos*m->_33;
  r._43 = m->_43;

  r._14 = ccos*m->_14 + ssin*m->_34;
  r._24 = m->_24;
  r._34 =-ssin*m->_14 + ccos*m->_34;
  r._44 = m->_44;

  *m = r;
  return(m);
}

inline GLMATRIX * matrix_rotate_z(GLMATRIX * m, float angle)
{
  GLMATRIX r;
  float  ssin = (float)sinf(angle);
  float  ccos = (float)cosf(angle);

  r._11 = ccos*m->_11 + ssin*m->_21;
  r._21 =-ssin*m->_11 + ccos*m->_21;
  r._31 = m->_31;
  r._41 = m->_41;

  r._12 = ccos*m->_12 + ssin*m->_22;
  r._22 =-ssin*m->_12 + ccos*m->_22;
  r._32 = m->_32;
  r._42 = m->_42;

  r._13 = ccos*m->_13 + ssin*m->_23;
  r._23 =-ssin*m->_13 + ccos*m->_23;
  r._33 = m->_33;
  r._43 = m->_43;

  r._14 = ccos*m->_14 + ssin*m->_24;
  r._24 =-ssin*m->_14 + ccos*m->_24;
  r._34 = m->_34;
  r._44 = m->_44;
  
  *m = r;
  return(m);
}

// Create the rotation matrix (Z axis)
inline GLMATRIX * matrix_rotation_z(GLMATRIX * m, float angle)
{  
  float  sinus   = (float)sinf(angle);
  float  cosinus = (float)cosf(angle);

  m->init();
  m->_11 = cosinus;
  m->_12 = sinus;
  m->_22 = cosinus;
  m->_21 = -sinus;
  return(m);
}

inline GLMATRIX * matrix_rotation_set(GLMATRIX *m, QUAT *p_q)
{  
  return(m->rotation_set(p_q));
}

inline QUAT * matrix_rotation_get(GLMATRIX *m, QUAT *p_q)
{
  return(m->rotation_get(p_q));
}

inline GLMATRIX * matrix_translate(GLMATRIX * m, float px, float py, float pz)
{
  m->_41 += px*m->_11 + py*m->_21 + pz*m->_31;
  m->_42 += px*m->_12 + py*m->_22 + pz*m->_32;
  m->_43 += px*m->_13 + py*m->_23 + pz*m->_33;
  m->_44 += px*m->_14 + py*m->_24 + pz*m->_34;
  return(m);  
}

inline GLMATRIX * matrix_translate(GLMATRIX * m, VECT *p_pos)
{
  float px = p_pos->x; 
  float py = p_pos->y; 
  float pz = p_pos->z;

  m->_41 += px*m->_11 + py*m->_21 + pz*m->_31;
  m->_42 += px*m->_12 + py*m->_22 + pz*m->_32;
  m->_43 += px*m->_13 + py*m->_23 + pz*m->_33;
  m->_44 += px*m->_14 + py*m->_24 + pz*m->_34;

  return(m);  
}

inline GLMATRIX * matrix_translatation_set(GLMATRIX * m, VECT *p_pos)
{
  m->_41 = p_pos->x;
  m->_42 = p_pos->y;
  m->_43 = p_pos->z;
  return(m);  
}

//  scale matice m o dx,dy,dz
inline GLMATRIX * matrix_scale(GLMATRIX * m, float x, float y, float z)
{
  m->_11 *= x;
  m->_21 *= x;
  m->_31 *= x;
  
  m->_12 *= y;
  m->_22 *= y;
  m->_32 *= y;
  
  m->_13 *= z;
  m->_23 *= z;
  m->_33 *= z;

  return(m);
}

//  vypocet inverzni matice k src
// Spocita inverzni matici k *src a ulozi ji do *inv
inline GLMATRIX * matrix_invert(GLMATRIX * src, GLMATRIX * inv)
{
  float det;

  inv->_11 =   src->_22 * src->_33 - src->_23 * src->_32;
  inv->_12 = - src->_12 * src->_33 + src->_13 * src->_32;
  inv->_13 =   src->_12 * src->_23 - src->_13 * src->_22;

  det = inv->_11 * src->_11 + inv->_12 * src->_21 + inv->_13 * src->_31;
  if(det == 0.0f)
  {
    assert(0);
    return(NULL);
  }

  inv->_21 = - src->_21 * src->_33 + src->_23 * src->_31;
  inv->_22 =   src->_11 * src->_33 - src->_13 * src->_31;
  inv->_23 = - src->_11 * src->_23 + src->_13 * src->_21;

  inv->_31 =   src->_21 * src->_32 - src->_22 * src->_31;
  inv->_32 = - src->_11 * src->_32 + src->_12 * src->_31;
  inv->_33 =   src->_11 * src->_22 - src->_12 * src->_21;

  inv->_11 /= det; inv->_12 /= det; inv->_13 /= det; 
  inv->_21 /= det; inv->_22 /= det; inv->_23 /= det; 
  inv->_31 /= det; inv->_32 /= det; inv->_33 /= det; 

  inv->_41 = - src->_41*inv->_11 - src->_42*inv->_21 - src->_43*inv->_31;
  inv->_42 = - src->_41*inv->_12 - src->_42*inv->_22 - src->_43*inv->_32;
  inv->_43 = - src->_41*inv->_13 - src->_42*inv->_23 - src->_43*inv->_33;

  inv->_14 = 0;
  inv->_24 = 0;
  inv->_34 = 0;
  inv->_44 = 1;
  
  return(inv);
}

inline void matrix_transformate(float *p_x, float *p_y, float *p_z, GLMATRIX *m)
{
 float x,y,z;

 x = *p_x; y = *p_y; z = *p_z;
 
 *p_x = m->_11*x + m->_21*y + m->_31*z + m->_41;
 *p_y = m->_12*x + m->_22*y + m->_32*z + m->_42;
 *p_z = m->_13*x + m->_23*y + m->_33*z + m->_43;
// wp = matSet._14*x + matSet._24*y + matSet._34*z + matSet._44;
}

inline VECT * matrix_transformate(VECT *p_p, GLMATRIX *m)
{
 float x,y,z;

 x = p_p->x; y = p_p->y; z = p_p->z;
 
 p_p->x = m->_11*x + m->_21*y + m->_31*z + m->_41;
 p_p->y = m->_12*x + m->_22*y + m->_32*z + m->_42;
 p_p->z = m->_13*x + m->_23*y + m->_33*z + m->_43;
 
 return(p_p);
}

inline void matrix_transformate(float *p_x, float *p_y, float *p_z, float *p_w, GLMATRIX *m)
{
 float x,y,z,w;

 x = *p_x; y = *p_y; z = *p_z, w = *p_w;;
 
 *p_x = m->_11*x + m->_21*y + m->_31*z + m->_41*w;
 *p_y = m->_12*x + m->_22*y + m->_32*z + m->_42*w;
 *p_z = m->_13*x + m->_23*y + m->_33*z + m->_43*w;
 *p_w = m->_14*x + m->_24*y + m->_34*z + m->_44*w;
}

inline void matrix_transformate(WVECT *p_source, GLMATRIX *m, WVECT *p_target)
{
 float x,y,z,w;

 x = p_source->x; y = p_source->y; z = p_source->z; w = p_source->w;
 
 p_target->x = m->_11*x + m->_21*y + m->_31*z + m->_41*w;
 p_target->y = m->_12*x + m->_22*y + m->_32*z + m->_42*w;
 p_target->z = m->_13*x + m->_23*y + m->_33*z + m->_43*w;
 p_target->w = m->_14*x + m->_24*y + m->_34*z + m->_44*w;
}

inline VECT * matrix_transformate(VECT *p_source, GLMATRIX *m, VECT *p_target)
{
 float x,y,z;

 x = p_source->x; y = p_source->y; z = p_source->z;
 
 p_target->x = m->_11*x + m->_21*y + m->_31*z + m->_41;
 p_target->y = m->_12*x + m->_22*y + m->_32*z + m->_42;
 p_target->z = m->_13*x + m->_23*y + m->_33*z + m->_43;
 
 return(p_target);
}

inline void matrix_transformate_vector(VECT *p_vect, GLMATRIX *m)
{
 float x,y,z;

 x = p_vect->x; y = p_vect->y; z = p_vect->z;
 
 p_vect->x = m->_11*x + m->_21*y + m->_31*z;
 p_vect->y = m->_12*x + m->_22*y + m->_32*z;
 p_vect->z = m->_13*x + m->_23*y + m->_33*z;
}

inline void matrix_transformate_vector(VECT *p_vect, GLMATRIX *m, VECT *p_target)
{
 float x,y,z;

 x = p_vect->x; y = p_vect->y; z = p_vect->z; 
 p_target->x = m->_11*x + m->_21*y + m->_31*z;
 p_target->y = m->_12*x + m->_22*y + m->_32*z;
 p_target->z = m->_13*x + m->_23*y + m->_33*z;
}

inline void __gluMakeIdentityf(float m[16])
{
  memset(m,16,sizeof(m[0]));
  m[0+4*0] = 1;
  m[1+4*1] = 1;
  m[2+4*2] = 1;
  m[3+4*3] = 1;
}

/* SGI-GLU implementace
** inverse = invert(src)
*/
inline int __gluInvertMatrixf(const float src[16], float inverse[16])
{
  int i, j, k, swap;
  float  t;
  float  temp[4][4];
  
  for (i=0; i<4; i++) {
    for (j=0; j<4; j++) {
      temp[i][j] = src[i*4+j];
    }
  }
  __gluMakeIdentityf(inverse);
  
  for (i = 0; i < 4; i++) {
  /*
  ** Look for largest element in column
    */
    swap = i;
    for (j = i + 1; j < 4; j++) {
      if (fabs(temp[j][i]) > fabs(temp[i][i])) {
        swap = j;
      }
    }
    
    if (swap != i) {
    /*
    ** Swap rows.
      */
      for (k = 0; k < 4; k++) {
        t = temp[i][k];
        temp[i][k] = temp[swap][k];
        temp[swap][k] = t;
        
        t = inverse[i*4+k];
        inverse[i*4+k] = inverse[swap*4+k];
        inverse[swap*4+k] = t;
      }
    }
    
    if (temp[i][i] == 0) {
    /*
    ** No non-zero pivot.  The matrix is singular, which shouldn't
    ** happen.  This means the user gave us a bad matrix.
      */
      return FALSE;
    }
    
    t = temp[i][i];
    for (k = 0; k < 4; k++) {
      temp[i][k] /= t;
      inverse[i*4+k] /= t;
    }
    for (j = 0; j < 4; j++) {
      if (j != i) {
        t = temp[j][i];
        for (k = 0; k < 4; k++) {
          temp[j][k] -= temp[i][k]*t;
          inverse[j*4+k] -= inverse[i*4+k]*t;
        }
      }
    }
  }
  return TRUE;
}

GLMATRIX * glu_invert_matrix(GLMATRIX *p_in, GLMATRIX *p_out);

typedef struct _GLDMATRIX {

  double _11,_12,_13,_14;
  double _21,_22,_23,_24;
  double _31,_32,_33,_34;
  double _41,_42,_43,_44;

} GLDMATRIX;

inline GLDMATRIX * gl2gld_matrix(glmatrix * mf, GLDMATRIX * md)
{ 
  md->_11 = (double)mf->_11;
  md->_12 = (double)mf->_12;
  md->_13 = (double)mf->_13;
  md->_14 = (double)mf->_14;

  md->_21 = (double)mf->_21;
  md->_22 = (double)mf->_22;
  md->_23 = (double)mf->_23;
  md->_24 = (double)mf->_24;

  md->_31 = (double)mf->_31;
  md->_32 = (double)mf->_32;
  md->_33 = (double)mf->_33;
  md->_34 = (double)mf->_34;

  md->_41 = (double)mf->_41;
  md->_42 = (double)mf->_42;
  md->_43 = (double)mf->_43;
  md->_44 = (double)mf->_44; 
  
  return(md);
}

inline GLMATRIX * gld2gl_matrix(GLDMATRIX * mf, glmatrix * md)
{ 
  md->_11 = (float)mf->_11;
  md->_12 = (float)mf->_12;
  md->_13 = (float)mf->_13;
  md->_14 = (float)mf->_14;

  md->_21 = (float)mf->_21;
  md->_22 = (float)mf->_22;
  md->_23 = (float)mf->_23;
  md->_24 = (float)mf->_24;

  md->_31 = (float)mf->_31;
  md->_32 = (float)mf->_32;
  md->_33 = (float)mf->_33;
  md->_34 = (float)mf->_34;

  md->_41 = (float)mf->_41;
  md->_42 = (float)mf->_42;
  md->_43 = (float)mf->_43;
  md->_44 = (float)mf->_44; 
  
  return(md);
}

#endif //__MATRIX_H__
