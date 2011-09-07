/*
  Quaterniony
*/

#ifndef __QUAT_H
#define __QUAT_H

#define  DELTA      0.0001f
#define  ZERO_QUAT {0.0f,0.0f,0.0f,1.0f}
inline float quat_delka(QUAT * p_v)
{
  return ((float) sqrt(p_v->x * p_v->x + p_v->y * p_v->y + p_v->z * p_v->z +
      p_v->w * p_v->w));
} inline QUAT *quat_set_zero(QUAT * p_v)
{
  p_v->x = 0;
  p_v->y = 0;
  p_v->z = 0;
  p_v->w = 1;
  return (p_v);
}

inline QUAT *angle_to_quat(QUAT * p_q, BOD * p_o, float uhel)
{
  float vel = sqrtf(p_o->x * p_o->x + p_o->y * p_o->y + p_o->z * p_o->z);

  if (vel > 0.0001) {
    vel = 1.0f / vel;
    p_q->x = p_o->x * vel;
    p_q->y = p_o->y * vel;
    p_q->z = p_o->z * vel;
  }
  uhel *= 0.5f;
  p_q->w = (float) cos(uhel);
  vektor_mult_skalar((BOD *) p_q, sinf(uhel), (BOD *) p_q);
  quat_norm(p_q);
  return (p_q);
}


/*
     ( 1-2yy-2zz        2xy+2wz          2xz-2wy  )
M =  (  2xy-2wz        1-2xx-2zz         2yz+2wx  )
     (  2xz+2wy         2yz-2wx         1-2xx-2yy )
*/
inline GLMATRIX *quat_to_matrix(GLMATRIX * p_mat, QUAT * p_q)
{
  float xx2 = 2 * p_q->x * p_q->x;
  float yy2 = 2 * p_q->y * p_q->y;
  float zz2 = 2 * p_q->z * p_q->z;


  // vektor X
  p_mat->_11 = 1.0f - yy2 - zz2;
  p_mat->_21 = 2 * p_q->x * p_q->y - 2 * p_q->w * p_q->z;
  p_mat->_31 = 2 * p_q->x * p_q->z + 2 * p_q->w * p_q->y;

  // vektor Y
  p_mat->_12 = 2 * p_q->x * p_q->y + 2 * p_q->w * p_q->z;
  p_mat->_22 = 1.0f - xx2 - zz2;
  p_mat->_32 = 2 * p_q->y * p_q->z - 2 * p_q->w * p_q->x;

  // vektor Z
  p_mat->_13 = 2 * p_q->x * p_q->z - 2 * p_q->w * p_q->y;
  p_mat->_23 = 2 * p_q->y * p_q->z + 2 * p_q->w * p_q->x;
  p_mat->_33 = 1.0f - xx2 - yy2;
  return (p_mat);
}


/* y rotace
  // *p_r = acosf(1.0f-(2*p_q->x*p_q->x)-(2*p_q->y*p_q->y));
  temp._11 = cosinus;
  temp._13 = sinus;
  temp._33 = cosinus;
  temp._31 = -sinus;
*/
inline QUAT *matrix_to_quat(GLMATRIX * p_m, QUAT * p_q)
{

  // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
  // article "Quaternion Calculus and Fast Animation".
  float diagonala = p_m->_11 + p_m->_22 + p_m->_33;
  float root;

  if (diagonala > 0.0f) {

    // |w| > 1/2, may as well choose w > 1/2
    root = sqrtf(diagonala + 1.0f);     // 2w
    p_q->w = 0.5f * root;
    root = 0.5f / root;         // root = 1/(4w)
    p_q->x = (p_m->_23 - p_m->_32) * root;
    p_q->y = (p_m->_31 - p_m->_13) * root;
    p_q->z = (p_m->_12 - p_m->_21) * root;
    return (p_q);
  }
  else {

    // |w| <= 1/2
    float t;
    int i;

    if (p_m->_22 > p_m->_11) {
      t = p_m->_22;
      i = 2;
    }
    else {
      t = p_m->_11;
      i = 1;
    }
    if (p_m->_33 > t) {
      i = 3;
    }

    // i je nejvetsi a pak j a k sou ty 2 zbyvajici
    // fRoot = sqrtf(kRot[i][i]-kRot[j][j]-kRot[k][k] + 1.0f);
    switch (i) {
      case 1:                  // x je nejvetsi -> 4*x^2 j = 2 k = 3
        root = sqrtf(p_m->_11 - p_m->_22 - p_m->_33 + 1.0f);
        p_q->x = 0.5f * root;
        root = 0.5f / root;     // 1/(4*x)
        p_q->w = (p_m->_23 - p_m->_32) * root;  //kRot[k][j]-kRot[j][k];
        p_q->y = (p_m->_12 + p_m->_21) * root;  //kRot[j][i]+kRot[i][j];
        p_q->z = (p_m->_13 + p_m->_31) * root;  //kRot[k][i]+kRot[i][k];
        break;
      case 2:                  // y je nejvetsi -> 4*y^2 j = 3 k = 1
        root = sqrtf(p_m->_22 - p_m->_33 - p_m->_11 + 1.0f);
        p_q->y = 0.5f * root;
        root = 0.5f / root;     // 1/(4*y)
        p_q->w = (p_m->_31 - p_m->_13) * root;  //(kRot[k][j]-kRot[j][k];
        p_q->z = (p_m->_23 + p_m->_32) * root;  //(kRot[j][i]+kRot[i][j];
        p_q->x = (p_m->_21 + p_m->_12) * root;  //(kRot[k][i]+kRot[i][k];
        break;
      case 3:                  // z je nejvetsi -> 4*z^2 i = 3 j = 1 k = 2
        root = sqrtf(p_m->_33 - p_m->_11 - p_m->_22 + 1.0f);
        p_q->z = 0.5f * root;
        root = 0.5f / root;     // 1/(4*z)
        p_q->w = (p_m->_12 - p_m->_21) * root;  //(kRot[k][j]-kRot[j][k];
        p_q->x = (p_m->_31 + p_m->_13) * root;  //(kRot[j][i]+kRot[i][j];
        p_q->y = (p_m->_32 + p_m->_23) * root;  //(kRot[k][i]+kRot[i][k];
        break;
    }
    return (p_q);
  }
}


/*
  Nasobeni quaternionu skalarem
*/
inline QUAT *quat_mult_skalar(QUAT * p_q1, float skalar, QUAT * p_qv)
{
  p_qv->x = p_q1->x * skalar;
  p_qv->y = p_q1->y * skalar;
  p_qv->z = p_q1->z * skalar;
  p_qv->w = p_q1->w * skalar;
  return (p_qv);
}


/* Slepi 2 uhly do jednoho kvaternionu
*/
inline QUAT *kam_angle_to_quat(float r, float fi, QUAT * p_q)
{
  QUAT q1, q2;
  BOD o1, o2;

  if (r > 0.0f)
    o1.y = -1.0f;

  else {
    r = -r;
    o1.y = 1.0f;
  }
  o1.z = 0.0f;
  o1.x = 0.0f;
  if (fi > 0.0f)
    o2.x = 1.0f;

  else {
    fi = -fi;
    o2.x = -1.0f;
  }
  o2.y = 0.0f;
  o2.z = 0.0f;
  quat_mult(angle_to_quat(&q1, &o1, r), angle_to_quat(&q2, &o2, fi), p_q);
  return (p_q);
}

inline QUAT *key_rotkey_to_quat(ROTKEY * p_rot, QUAT * p_q)
{
  return (angle_to_quat(p_q, (BOD *) & p_rot->x, p_rot->angle));
}


#endif
