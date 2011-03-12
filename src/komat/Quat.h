/*
  Quaterniony
*/

#ifndef __QUAT_H
#define __QUAT_H

#define  DELTA      0.0001f
#define  ZERO_QUAT {0.0f,0.0f,0.0f,1.0f}

inline float quat_dot_product(QUAT *p_q1, QUAT *p_q2)
{
 return(p_q1->x*p_q2->x + p_q1->y*p_q2->y + p_q1->z*p_q2->z + p_q1->w*p_q2->w);
}

inline float quat_delka(QUAT *p_v)
{
 return((float)sqrt(p_v->x*p_v->x + p_v->y*p_v->y + p_v->z*p_v->z + p_v->w*p_v->w));
}

inline QUAT * quat_norm(QUAT *p_v)
{
  float delka;
  delka = 1.0f/quat_delka(p_v);
  if(delka > 0.0001) {
    p_v->x *= delka;
    p_v->y *= delka;
    p_v->z *= delka;
    p_v->w *= delka;
  }  
  return(p_v);
}

inline QUAT * quat_set_zero(QUAT *p_v)
{
  p_v->x = 0;
  p_v->y = 0;
  p_v->z = 0;
  p_v->w = 1;
  return(p_v);
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

inline QUAT * angle_to_quat(QUAT *p_q, BOD *p_o, float uhel)
{
 float vel = sqrtf(p_o->x*p_o->x + p_o->y*p_o->y + p_o->z*p_o->z);

 if(vel > 0.0001) {
   vel = 1.0f/vel;
   p_q->x = p_o->x*vel;
   p_q->y = p_o->y*vel;
   p_q->z = p_o->z*vel;
 } 

 uhel *= 0.5f;
 p_q->w = (float)cos(uhel);
 vektor_mult_skalar((BOD *)p_q,sinf(uhel),(BOD *)p_q);
 quat_norm(p_q);
 return(p_q);
}

/*
     ( 1-2yy-2zz        2xy+2wz          2xz-2wy  )
M =  (  2xy-2wz        1-2xx-2zz         2yz+2wx  )
     (  2xz+2wy         2yz-2wx         1-2xx-2yy )
*/
inline GLMATRIX * quat_to_matrix(GLMATRIX *p_mat, QUAT *p_q)
{
  float xx2 = 2*p_q->x*p_q->x;
  float yy2 = 2*p_q->y*p_q->y;
  float zz2 = 2*p_q->z*p_q->z;
  
  // vektor X
  p_mat->_11 = 1.0f-yy2-zz2;
  p_mat->_21 = 2*p_q->x*p_q->y - 2*p_q->w*p_q->z;
  p_mat->_31 = 2*p_q->x*p_q->z + 2*p_q->w*p_q->y;

  // vektor Y
  p_mat->_12 = 2*p_q->x*p_q->y + 2*p_q->w*p_q->z;
  p_mat->_22 = 1.0f-xx2-zz2;
  p_mat->_32 = 2*p_q->y*p_q->z - 2*p_q->w*p_q->x;

  // vektor Z
  p_mat->_13 = 2*p_q->x*p_q->z - 2*p_q->w*p_q->y;
  p_mat->_23 = 2*p_q->y*p_q->z + 2*p_q->w*p_q->x;
  p_mat->_33 = 1.0f-xx2-yy2;

  return(p_mat);
}

inline void quat_to_euler(QUAT *p_q, float *p_r, float *p_fi)
{
  float p1 = 2*p_q->y*p_q->z - 2*p_q->w*p_q->x,
        p2 = 2*p_q->x*p_q->z - 2*p_q->w*p_q->y,
        p3;

  p3 = -asinf((fabsf(p1) > 1.0f) ? (int)(p1) : p1);
  *p_fi = (fabs(p_q->w) < fabs(p_q->x)) ? 
          copysign(DEG2RAD(180.0f)-fabs(p3),p3) : p3;
  p3 = asinf((fabsf(p2) > 1.0f) ? (int)(p2) : p2);
  *p_r = (fabs(p_q->w) < fabs(p_q->y)) ? 
          copysign(DEG2RAD(180.0f)-fabs(p3),p3) : p3;
}

/* y rotace
  // *p_r = acosf(1.0f-(2*p_q->x*p_q->x)-(2*p_q->y*p_q->y));
  temp._11 = cosinus;
  temp._13 = sinus;
  temp._33 = cosinus;
  temp._31 = -sinus;
*/

inline QUAT * matrix_to_quat(GLMATRIX *p_m, QUAT *p_q)
{
  // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
  // article "Quaternion Calculus and Fast Animation".

  float diagonala = p_m->_11+p_m->_22+p_m->_33;
  float root;
  
  if(diagonala > 0.0f) {
    
    // |w| > 1/2, may as well choose w > 1/2
    root = sqrtf(diagonala + 1.0f);  // 2w
    p_q->w = 0.5f*root;
    root = 0.5f/root;  // root = 1/(4w)
    
    p_q->x = (p_m->_23-p_m->_32)*root;
    p_q->y = (p_m->_31-p_m->_13)*root;
    p_q->z = (p_m->_12-p_m->_21)*root;
    
    return(p_q);
  } else {
    // |w| <= 1/2
    float  t;
    int    i;
    
    if(p_m->_22 > p_m->_11) {
      t = p_m->_22;
      i = 2;
    } else {
      t = p_m->_11;
      i = 1;
    }
    if(p_m->_33 > t) {
      i = 3;
    }
    
    // i je nejvetsi a pak j a k sou ty 2 zbyvajici
    // fRoot = sqrtf(kRot[i][i]-kRot[j][j]-kRot[k][k] + 1.0f);
    switch(i) {
      case 1: // x je nejvetsi -> 4*x^2 j = 2 k = 3
        root = sqrtf(p_m->_11 - p_m->_22 - p_m->_33 + 1.0f);
        p_q->x = 0.5f*root;
        root = 0.5f/root; // 1/(4*x)
        
        p_q->w = (p_m->_23-p_m->_32)*root; //kRot[k][j]-kRot[j][k];
        p_q->y = (p_m->_12+p_m->_21)*root; //kRot[j][i]+kRot[i][j];
        p_q->z = (p_m->_13+p_m->_31)*root; //kRot[k][i]+kRot[i][k];
         
        break;
      case 2: // y je nejvetsi -> 4*y^2 j = 3 k = 1
        root = sqrtf(p_m->_22 - p_m->_33 - p_m->_11 + 1.0f);
        p_q->y = 0.5f*root;
        root = 0.5f/root; // 1/(4*y)

        p_q->w = (p_m->_31-p_m->_13)*root;  //(kRot[k][j]-kRot[j][k];
        p_q->z = (p_m->_23+p_m->_32)*root;  //(kRot[j][i]+kRot[i][j];
        p_q->x = (p_m->_21+p_m->_12)*root;  //(kRot[k][i]+kRot[i][k];

        break;
      case 3: // z je nejvetsi -> 4*z^2 i = 3 j = 1 k = 2
        root = sqrtf(p_m->_33 - p_m->_11 - p_m->_22 + 1.0f);
        p_q->z = 0.5f*root;
        root = 0.5f/root; // 1/(4*z)

        p_q->w = (p_m->_12-p_m->_21)*root; //(kRot[k][j]-kRot[j][k];
        p_q->x = (p_m->_31+p_m->_13)*root; //(kRot[j][i]+kRot[i][j];
        p_q->y = (p_m->_32+p_m->_23)*root; //(kRot[k][i]+kRot[i][k];
        
        break;
    }
    
    return(p_q);
  }    
}

/*
  q1 x q2 = [s1,v1] x [s2,v2] = [(s1*s2 - v1*v2),(s1*v2 + s2*v1 + v1xv2)].
*/
inline QUAT * quat_mult(QUAT *p_q1, QUAT *p_q2, QUAT *p_qv)
{
 QUAT q1,q2;
  
 p_qv->w = p_q1->w*p_q2->w - vektor_dot_product((BOD *)p_q1,(BOD *)p_q2);
 
 vektor_mult_skalar((BOD *)p_q2,p_q1->w,(BOD *)&q1);
 vektor_mult_skalar((BOD *)p_q1,p_q2->w,(BOD *)&q2);
 
 vektor_soucin((BOD *)p_q1,(BOD *)p_q2,(BOD *)p_qv);
 
 vektor_add((BOD *)&q1,(BOD *)p_qv,(BOD *)p_qv);
 vektor_add((BOD *)&q2,(BOD *)p_qv,(BOD *)p_qv);
 
 return(p_qv);
}

/*
  Nasobeni quaternionu skalarem
*/
inline QUAT * quat_mult_skalar(QUAT *p_q1, float skalar, QUAT *p_qv)
{
 p_qv->x = p_q1->x*skalar;
 p_qv->y = p_q1->y*skalar;
 p_qv->z = p_q1->z*skalar;
 p_qv->w = p_q1->w*skalar;
 return(p_qv);
}

/*
  Spherical Linear intERPolation
  by Carmack & spol.
*/
inline QUAT * slerp(QUAT *p_q1, QUAT *p_q2, float t, QUAT *p_qv)
{
  float cosin;
  float a;
  float sina;
  float u1;
  float u2;
  QUAT  qp,qm;

  if(t == 0.0f) return(*p_qv = *p_q1,p_qv);
  if(t == 1.0f) return(*p_qv = *p_q2,p_qv);
  
  quat_sub(p_q1,p_q2,&qm);
  quat_add(p_q1,p_q2,&qp);
  
  //(q1-q2)*(q1-q2)>(q1+q2)*(q1+q2) -> false -> prehodit to
  if(quat_dot_product(&qm,&qm) >= quat_dot_product(&qp,&qp)) {
    quat_negative(p_q2);
  }

  cosin = quat_dot_product(p_q1,p_q2);

  if((1.0f+cosin) > DELTA) { // uhel je mensi nez 90 stupnu    
    if ((1.0f-cosin) > DELTA) { // uhel je vetsi nez delta
      a = acosf(cosin);
      sina = sinf(a);
      u1 = sinf((1.0f - t)*a)/sina;
      u2 = sinf(t*a)/sina;
    } else { // uhel je mensi    
      u1 = 1.0f - t;
      u2 = t;
    }    
    quat_mult_skalar(p_q1, u1, p_qv);
    quat_mult_skalar(p_q2, u2, &qp);
    return(quat_add(p_qv,&qp,p_qv));
  } else { // vetsi nez 90 stupnu
    qp.x =-p_q1->y;
    qp.y = p_q1->x;
    qp.z =-p_q1->w;
    qp.w = p_q1->z;

    u1 = sinf((1.0f - t)*0.5f*PI);
    u2 = sinf(t*0.5f*PI);

    quat_mult_skalar(p_q1,u1, p_qv);
    quat_mult_skalar(&qp, u2, &qm);
    return(quat_add(p_qv,&qm,p_qv));
  }
}

// quat -> uhel
inline QUAT * quat_to_angle(QUAT *p_q, BOD *p_o, float *p_uhel)
{
 *p_uhel = (float)(2.0f*acos(p_q->w));
 if(*p_uhel < DELTA) {
   p_o->x = p_o->y = p_o->z = 0.0f;
 } else {
   vektor_mult_skalar((BOD *)p_q,(float)(1.0f/sin(*p_uhel/2.0f)),p_o);
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
 return(quat_inverse((QUAT *)vektor_mult_skalar((BOD *)p_q,(float)c,(BOD *)p_v)));
}

/* Slepi 2 uhly do jednoho kvaternionu
*/
inline QUAT * kam_angle_to_quat(float r, float fi, QUAT *p_q)
{
  QUAT q1,q2;
  BOD  o1,o2;

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

  quat_mult(angle_to_quat(&q1,&o1,r),angle_to_quat(&q2,&o2,fi),p_q);
  return(p_q);
}

inline QUAT * key_rotkey_to_quat(ROTKEY *p_rot, QUAT *p_q)
{
  return(angle_to_quat(p_q,(BOD *)&p_rot->x,p_rot->angle));
}

#endif
