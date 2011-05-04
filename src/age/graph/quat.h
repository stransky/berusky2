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
  Quaternions class
*/

#ifndef __QUAT_H__
#define __QUAT_H__

#define  DELTA       0.0001f
#define  ZERO_QUAT  {0.0f,0.0f,0.0f,1.0f}

typedef class quat : public vect3df {

public:
  
  float w;

public:
  
  void  zero(void)
  {
    x = 0; y = 0; z = 0; w = 0; 
  }

  void  init(void) 
  {
    x = 0; y = 0; z = 0; w = 1; 
  }

  float size(void)
  {
    return((float)sqrt(x*x + y*y + z*z + w*w));
  }
  
  quat * norm(void)
  {
    float dl = 1.0f / size();
    if(dl > 0.0001) {
      x *= dl;
      y *= dl;
      z *= dl;
      w *= dl;
    }
    return(this);
  }

  quat * inverse(void)
  {
    x = -x;
    y = -y;
    z = -z;
    return(this);
  }

  quat * negative(void)
  {
    x = -x;
    y = -y;
    z = -z;
    w = -w;
    return(this);
  }
  
  void get_angle(VECT *p_o, float *p_angle)
  {
    *p_angle = (float)(2.0f*acos(w));
    if(*p_angle < DELTA) {
      p_o->x = p_o->y = p_o->z = 0.0f;
    } else {
      *p_o = *this * (float)(1.0f/sin(*p_angle/2.0f));      
    }  
  }
    
  void get_angle(quat *p_v)
  {
    float c;
    p_v->w = (float)(2.0f*acos(w));
    c = sinf(p_v->w/2.0f);
    c = (c > DELTA) ? 1.0f/c : 0.0f;
    *(VECT *)p_v = *this * c;
    p_v->inverse();    
  }
  
  quat * mult(float scalar)
  {
    x = x*scalar;
    y = y*scalar;
    z = z*scalar;
    w = w*scalar;    
    return(this);
  }
 
  quat * add(quat *p_q1)
  {
    x += p_q1->x;
    y += p_q1->y;
    z += p_q1->z;
    w += p_q1->w;
    return(this);
  }

  void to_euler(float *p_r, float *p_fi)
  {
    float p1 = 2*y*z - 2*w*x,
          p2 = 2*x*z - 2*w*y,
          p3;

    p3    = -asinf((fabsf(p1) > 1.0f) ? copysignf(1,p1) : p1);
   *p_fi  = (fabsf(w) < fabsf(x)) ? copysignf(DEG2RAD(180.0f)-fabsf(p3),p3) : p3;
    p3    = asinf((fabsf(p2) > 1.0f) ? copysignf(1,p2) : p2);
   *p_r   = (fabsf(w) < fabsf(y)) ? copysignf(DEG2RAD(180.0f)-fabsf(p3),p3) : p3;
  }
  
  static float dot_product(quat *p_q1, quat *p_q2)
  {
    return(p_q1->x*p_q2->x + p_q1->y*p_q2->y + p_q1->z*p_q2->z + p_q1->w*p_q2->w);
  }
  
  static quat * add(quat *p_q1, quat *p_q2, quat *p_qv)
  {
    p_qv->x = p_q1->x+p_q2->x;
    p_qv->y = p_q1->y+p_q2->y;
    p_qv->z = p_q1->z+p_q2->z;
    p_qv->w = p_q1->w+p_q2->w;
    return(p_qv);
  }
  
  static quat * sub(quat *p_q1, quat *p_q2, quat *p_qv)
  {
    p_qv->x = p_q1->x-p_q2->x;
    p_qv->y = p_q1->y-p_q2->y;
    p_qv->z = p_q1->z-p_q2->z;
    p_qv->w = p_q1->w-p_q2->w;
    return(p_qv);
  }
  
  /*
  q1 x q2 = [s1,v1] x [s2,v2] = [(s1*s2 - v1*v2),(s1*v2 + s2*v1 + v1xv2)].
  */
  static quat * mult(quat *p_q1, quat *p_q2, quat *p_qv)
  {
    quat q1,q2;
    
    p_qv->w = p_q1->w*p_q2->w - vect_dot_product(*p_q1,*p_q2);
    
    (VECT)q1 = *p_q2 * p_q1->w;
    (VECT)q2 = *p_q1 * p_q2->w;
    
    ::vect_mult(p_q1,p_q2,p_qv);
    
    *(VECT *)p_qv += q1;
    *(VECT *)p_qv += q2;
    
    return(p_qv);
  }
  
  quat * mult(quat *p_q1, quat *p_q2)
  {
    quat q1,q2;
    
    w = p_q1->w*p_q2->w - ::vect_dot_product(p_q1,p_q2);
    
    (VECT)q1 = *p_q2 * p_q1->w;
    (VECT)q2 = *p_q1 * p_q2->w;
    
    ::vect_mult(p_q1,p_q2,this);
    
    *this += q1;
    *this += q2;
    
    return(this);
  }
  
  static quat * mult(quat *p_q1, float scalar, quat *p_qv)
  {
    p_qv->x = p_q1->x*scalar;
    p_qv->y = p_q1->y*scalar;
    p_qv->z = p_q1->z*scalar;
    p_qv->w = p_q1->w*scalar;
    
    return(p_qv);
  }
  
  // Spherical Linear intERPolation
  // by Carmack & spol.  
  static quat * slerp(quat *p_q1, quat *p_q2, float t, quat *p_qv)
  {
    float cosin;
    float a;
    float sina;
    float u1;
    float u2;
    quat  qp,qm;
    
    if(t == 0.0f) return(*p_qv = *p_q1,p_qv);
    if(t == 1.0f) return(*p_qv = *p_q2,p_qv);
    
    sub(p_q1,p_q2,&qm);
    add(p_q1,p_q2,&qp);
    
    //(q1-q2)*(q1-q2)>(q1+q2)*(q1+q2) -> false -> prehodit to
    if(quat::dot_product(&qm,&qm) >= quat::dot_product(&qp,&qp)) {
      p_q2->negative();
    }
    
    cosin = quat::dot_product(p_q1,p_q2);
    
    if((1.0f+cosin) > DELTA) { // angle je mensi nez 90 stupnu    
      if ((1.0f-cosin) > DELTA) { // angle je vetsi nez delta
        a = acosf(cosin);
        sina = sinf(a);
        u1 = sinf((1.0f - t)*a)/sina;
        u2 = sinf(t*a)/sina;
      } else { // angle je mensi    
        u1 = 1.0f - t;
        u2 = t;
      }    
      quat::mult(p_q1, u1, p_qv);
      quat::mult(p_q2, u2, &qp);
      return(p_qv->add(&qp));
    } else { // vetsi nez 90 stupnu
      qp.x =-p_q1->y;
      qp.y = p_q1->x;
      qp.z =-p_q1->w;
      qp.w = p_q1->z;
      
      u1 = sinf((1.0f - t)*0.5f*PI);
      u2 = sinf(t*0.5f*PI);
      
      quat::mult(p_q1,u1, p_qv);
      quat::mult(&qp, u2, &qm);
      return(p_qv->add(&qm));
    }
  }
  
public:  
  
  quat(void) {};
  
  quat(VECT *p_o, float angle)
  {
    float size = sqrtf(p_o->x*p_o->x + p_o->y*p_o->y + p_o->z*p_o->z);
    
    if(size > DELTA) {
      size = 1.0f/size;
      x = p_o->x*size;
      y = p_o->y*size;
      z = p_o->z*size;
    } 
    
    angle *= 0.5f;
    w = (float)cos(angle);    
    *this *= sinf(angle);
    norm();
  }
    
  quat(float x, float y, float z, float angle)
  {
    VECT v(x,y,z);
    quat(&v,angle);
  }
  
  quat(float r, float fi)
  {    
    VECT  o1,o2;
    
    if(r > 0.0f) 
      o1.y = -1.0f;
    else {
      r = -r; 
      o1.y = 1.0f;
    }
    o1.z = 0.0f;
    o1.x = 0.0f; 
    
    if(fi > 0.0f) 
      o2.x = 1.0f;
    else { 
      fi = -fi;
      o2.x = -1.0f;
    }
    o2.y = 0.0f; 
    o2.z = 0.0f;
    
    quat q1(&o1,r),q2(&o2,fi);
    mult(&q1,&q2);
  } 
    
} QUAT;

/*--------------------------------------------------------------------------
   Quaternion functions
  --------------------------------------------------------------------------
*/
inline float quat_dot_product(QUAT *p_q1, QUAT *p_q2)
{
 return(p_q1->x*p_q2->x + p_q1->y*p_q2->y + p_q1->z*p_q2->z + p_q1->w*p_q2->w);
}

inline float quat_length(QUAT *p_v)
{
 return((float)sqrt(p_v->x*p_v->x + p_v->y*p_v->y + p_v->z*p_v->z + p_v->w*p_v->w));
}

inline QUAT * quat_norm(QUAT *p_v)
{
  return(p_v->norm());
}

inline QUAT * quat_inverse(QUAT *p_v)
{
 p_v->x = -p_v->x;
 p_v->y = -p_v->y;
 p_v->z = -p_v->z;
 return(p_v);
}

inline QUAT * quat_add(QUAT *p_q1, QUAT *p_q2, QUAT *p_qv)
{
 p_qv->x = p_q1->x+p_q2->x;
 p_qv->y = p_q1->y+p_q2->y;
 p_qv->z = p_q1->z+p_q2->z;
 p_qv->w = p_q1->w+p_q2->w;
 return(p_qv);
}

inline QUAT * quat_sub(QUAT *p_q1, QUAT *p_q2, QUAT *p_qv)
{
 p_qv->x = p_q1->x-p_q2->x;
 p_qv->y = p_q1->y-p_q2->y;
 p_qv->z = p_q1->z-p_q2->z;
 p_qv->w = p_q1->w-p_q2->w;
 return(p_qv);
}

inline QUAT * quat_negative(QUAT *p_q)
{
 p_q->x = -p_q->x;
 p_q->y = -p_q->y;
 p_q->z = -p_q->z;
 p_q->w = -p_q->w;
 return(p_q);
}

inline void quat_to_euler(QUAT *p_q, float *p_r, float *p_fi)
{
  p_q->to_euler(p_r,p_fi);
}


/*
  q1 x q2 = [s1,v1] x [s2,v2] = [(s1*s2 - v1*v2),(s1*v2 + s2*v1 + v1xv2)].
*/
inline QUAT * quat_mult(QUAT *p_q1, QUAT *p_q2, QUAT *p_qv)
{
 return(p_qv->mult(p_q1,p_q2));
}

/*
  Scale quaternion by a scalar
*/
inline QUAT * quat_mult_scalar(QUAT *p_q1, float scalar, QUAT *p_qv)
{
 p_qv->x = p_q1->x*scalar;
 p_qv->y = p_q1->y*scalar;
 p_qv->z = p_q1->z*scalar;
 p_qv->w = p_q1->w*scalar;
 return(p_qv);
}

/*
  Spherical Linear intERPolation
  by Carmack & spol.
*/
inline QUAT * slerp(QUAT *p_q1, QUAT *p_q2, float t, QUAT *p_qv)
{
  return(quat::slerp(p_q1,p_q2,t,p_qv));
}

// quat -> angle
inline QUAT * quat_to_angle(QUAT *p_q, VECT *p_o, float *p_angle)
{
 *p_angle = (float)(2.0f*acos(p_q->w));
 if(*p_angle < DELTA) {
   p_o->x = p_o->y = p_o->z = 0.0f;
 } else {   
   *p_o = *p_q * (float)(1.0f/sin(*p_angle/2.0f));
   //vect_mult_scalar((VECT *)p_q,(float)(1.0f/sin(*p_angle/2.0f)),p_o);
 }
 return(p_q);
}

// quat -> angle
inline QUAT * quat_to_quat_angle(QUAT *p_q, QUAT *p_v)
{
 float c;
 p_v->w = (float)(2.0f*acos(p_q->w));
 c = sinf(p_v->w/2.0f);
 c = (c > DELTA) ? 1.0f/c : 0.0f;
 *(VECT *)p_v = *(VECT *)p_q * c;
 return(quat_inverse(p_v));
}

/* Slepi 2 uhly do jednoho kvaternionu
*/
/*
QUAT * kam_angle_to_quat(float r, float fi, QUAT *p_q)
{
  p_q->quat(r,fi);
  return(p_q);
}
*/

/*
quat * key_rotkey_to_quat(ROTKEY *p_rot, quat *p_q)
{
  return(angle_to_quat(p_q,(VECT *)&p_rot->x,p_rot->angle));
}
*/

#endif //__QUAT_H__
