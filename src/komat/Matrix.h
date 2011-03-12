/*
  h k modulu s maticema
*/
#ifndef __MATRIX_H
#define __MATRIX_H

// definuje matici
typedef struct _GLMATRIX {

  float _11,_12,_13,_14;
  float _21,_22,_23,_24;
  float _31,_32,_33,_34;
  float _41,_42,_43,_44;

} GLMATRIX;

typedef struct _GLDMATRIX {

  double _11,_12,_13,_14;
  double _21,_22,_23,_24;
  double _31,_32,_33,_34;
  double _41,_42,_43,_44;

} GLDMATRIX;

__inline GLDMATRIX * gl2gld_matrix(GLMATRIX * mf, GLDMATRIX * md)
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

__inline GLMATRIX * gld2gl_matrix(GLDMATRIX * mf, GLMATRIX * md)
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

// vynuluje matici
__inline GLMATRIX * zero_matrix(GLMATRIX * m)
{
 memset(m,0,sizeof(*m));
 return(m);
} 

//  Standartni kody na praci s maticemi
//  Nastavi matici na jednotkovou
__inline GLMATRIX * init_matrix (GLMATRIX *m)
{
 m->_11 = m->_22 = m->_33 = m->_44 = 1;
 m->_12 = m->_13 = m->_14 = m->_41 = 0;
 m->_21 = m->_23 = m->_24 = m->_42 = 0;
 m->_31 = m->_32 = m->_34 = m->_43 = 0;
 return(m);
}

//  Nasobeni matic 'a' a 'b' do matice 'r'
//  Alespon doufam po me konverzi :-)
__inline GLMATRIX * mat_mult(GLMATRIX * a, GLMATRIX * b, GLMATRIX * r)
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

__inline GLMATRIX * mat_add(GLMATRIX * a, GLMATRIX * b, GLMATRIX * r)
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
                    
__inline GLMATRIX * mat_mult_skalar(GLMATRIX * r, float scale)
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

__inline int mat_porovnej(GLMATRIX * r, GLMATRIX * a)
{
  if(r->_11 != a->_11)
    return(FALSE);
  if(r->_12 != a->_12)
    return(FALSE);
  if(r->_13 != a->_13)
    return(FALSE);
  if(r->_14 != a->_14)
    return(FALSE);

  if(r->_21 != a->_21)
    return(FALSE);
  if(r->_22 != a->_22)
    return(FALSE);
  if(r->_23 != a->_23)
    return(FALSE);
  if(r->_24 != a->_24)
    return(FALSE);

  if(r->_31 != a->_31)
    return(FALSE);
  if(r->_32 != a->_32)
    return(FALSE);
  if(r->_33 != a->_33)
    return(FALSE);
  if(r->_34 != a->_34)
    return(FALSE);

  if(r->_41 != a->_41)
    return(FALSE);
  if(r->_42 != a->_42)
    return(FALSE);
  if(r->_43 != a->_43)
    return(FALSE);
  if(r->_44 != a->_44)
    return(FALSE);

  return(TRUE);
}

__inline GLMATRIX * mat_copy(GLMATRIX * d, GLMATRIX * s)
{
  return((GLMATRIX *)memcpy(d,s,sizeof(*d)));
}

__inline GLMATRIX * mat_mult_dir(GLMATRIX * a, GLMATRIX * b, GLMATRIX * r)
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


//  Rotace kolem osy x o uhel angle
__inline GLMATRIX * rotate_matrix_x(GLMATRIX * m, float angle)
{
  GLMATRIX r;
  float  ssin = (float)sin(angle);
  float  ccos = (float)cos(angle);

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

//  Rotace kolem osy y o uhel angle
__inline GLMATRIX * rotate_matrix_y(GLMATRIX * m, float angle)
{
  GLMATRIX r;
  float  ssin = (float)sin(angle);
  float  ccos = (float)cos(angle);

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

//  Rotace kolem osy z o uhel angle
__inline GLMATRIX * rotate_matrix_z(GLMATRIX * m, float angle)
{
  GLMATRIX r;
  float  ssin = (float)sin(angle);
  float  ccos = (float)cos(angle);

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

__inline GLMATRIX * rotation_matrix_z(GLMATRIX * p_m, float angle)
{  
  float  sinus   = (float)sin(angle);
  float  cosinus = (float)cos(angle);

  init_matrix(p_m);
  p_m->_11 = cosinus;
  p_m->_12 = sinus;
  p_m->_22 = cosinus;
  p_m->_21 = -sinus;
  return(p_m);
}

//  Posunuti matice m o dx,dy,dz
__inline GLMATRIX * translate_matrix(GLMATRIX * m, float px, float py, float pz)
{
  m->_41 += px*m->_11 + py*m->_21 + pz*m->_31;
  m->_42 += px*m->_12 + py*m->_22 + pz*m->_32;
  m->_43 += px*m->_13 + py*m->_23 + pz*m->_33;
  m->_44 += px*m->_14 + py*m->_24 + pz*m->_34;
  return(m);  
}

__inline GLMATRIX * translate_matrix2(GLMATRIX * m, BOD *p_pos)
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

__inline GLMATRIX * translate_matrix_set(GLMATRIX * m, BOD *p_pos)
{
  m->_41 = p_pos->x;
  m->_42 = p_pos->y;
  m->_43 = p_pos->z;
  return(m);  
}

//  scale matice m o dx,dy,dz
__inline GLMATRIX * scale_matrix(GLMATRIX * m, float x, float y, float z)
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
__inline GLMATRIX * invert_matrix(GLMATRIX * src, GLMATRIX * inv)
{
  float det;

  inv->_11 =   src->_22 * src->_33 - src->_23 * src->_32;
  inv->_12 = - src->_12 * src->_33 + src->_13 * src->_32;
  inv->_13 =   src->_12 * src->_23 - src->_13 * src->_22;

  det = inv->_11 * src->_11 + inv->_12 * src->_21 + inv->_13 * src->_31;
  if(det == 0.0f)
  {
    //assert(0);
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

__inline void __gluMakeIdentityf(float m[16])
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
__inline int __gluInvertMatrixf(const float src[16], float inverse[16])
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
      return GL_FALSE;
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
  return GL_TRUE;
}

__inline int matrix_det(GLMATRIX *p_mat)
{
  float m1 =   p_mat->_22 * p_mat->_33 - p_mat->_23 * p_mat->_32;
  float m2 = - p_mat->_12 * p_mat->_33 + p_mat->_13 * p_mat->_32;
  float m3 =   p_mat->_12 * p_mat->_23 - p_mat->_13 * p_mat->_22;
  return((m1 * p_mat->_11 + m2 * p_mat->_21 + m3 * p_mat->_31) != 0.0);
}

__inline GLMATRIX * invert_matrix_copy(GLMATRIX * src, GLMATRIX * inv)
{
  GLMATRIX m;
  float    det;

  m._11 =   src->_22 * src->_33 - src->_23 * src->_32;
  m._12 = - src->_12 * src->_33 + src->_13 * src->_32;
  m._13 =   src->_12 * src->_23 - src->_13 * src->_22;

  det = m._11 * src->_11 + m._12 * src->_21 + m._13 * src->_31;
  if (det==0)
  {
    //assert(0);
    return(NULL);
  }

  m._21 = - src->_21 * src->_33 + src->_23 * src->_31;
  m._22 =   src->_11 * src->_33 - src->_13 * src->_31;
  m._23 = - src->_11 * src->_23 + src->_13 * src->_21;

  m._31 =   src->_21 * src->_32 - src->_22 * src->_31;
  m._32 = - src->_11 * src->_32 + src->_12 * src->_31;
  m._33 =   src->_11 * src->_22 - src->_12 * src->_21;

  m._11 /= det; m._12 /= det; m._13 /= det;
  m._21 /= det; m._22 /= det; m._23 /= det;
  m._31 /= det; m._32 /= det; m._33 /= det;

  m._41 = - src->_41*m._11 - src->_42*m._21 - src->_43*m._31;
  m._42 = - src->_41*m._12 - src->_42*m._22 - src->_43*m._32;
  m._43 = - src->_41*m._13 - src->_42*m._23 - src->_43*m._33;

  m._14 = 0;
  m._24 = 0;
  m._34 = 0;
  m._44 = 1;
  
  *inv = m;
  return(inv);
}

__inline GLMATRIX * mat_rot(GLMATRIX *p_mat, BOD *p_vx, BOD *p_vy, BOD *p_vz)
{
  p_mat->_11 = p_vx->x; p_mat->_21 = p_vx->y; p_mat->_31 = p_vx->z;
  p_mat->_12 = p_vy->x; p_mat->_22 = p_vy->y; p_mat->_32 = p_vy->z;
  p_mat->_13 = p_vz->x; p_mat->_23 = p_vz->y; p_mat->_33 = p_vz->z;
  p_mat->_14 = p_mat->_24 = p_mat->_34 = 0.0f;
  p_mat->_41 = p_mat->_42 = p_mat->_43 = 0.0f;
  p_mat->_44 = 1.0f;
  return(p_mat);
}

__inline GLMATRIX * mat_rot_2(GLMATRIX *p_mat, BOD *p_vx, BOD *p_vy, BOD *p_vz, BOD *p_pos)
{
  p_mat->_11 = p_vx->x; p_mat->_21 = p_vx->y; p_mat->_31 = p_vx->z;
  p_mat->_12 = p_vy->x; p_mat->_22 = p_vy->y; p_mat->_32 = p_vy->z;
  p_mat->_13 = p_vz->x; p_mat->_23 = p_vz->y; p_mat->_33 = p_vz->z;
  p_mat->_14 = p_mat->_24 = p_mat->_34 = 0.0f;
  p_mat->_41 = p_pos->x;
  p_mat->_42 = p_pos->y;
  p_mat->_43 = p_pos->z;
  p_mat->_44 = 1.0f;
  return(p_mat);
}

__inline GLMATRIX * mat_rot_inv(GLMATRIX *p_mat, BOD *p_vx, BOD *p_vy, BOD *p_vz)
{
  return(invert_matrix_copy(mat_rot(p_mat,p_vx,p_vy,p_vz),p_mat));
}

__inline GLMATRIX * mat_rot_2_inv(GLMATRIX *p_mat, BOD *p_vx, BOD *p_vy, BOD *p_vz, BOD *p_pos)
{
  return(invert_matrix_copy(mat_rot_2(p_mat,p_vx,p_vy,p_vz,p_pos),p_mat));
}


__inline GLMATRIX * buildFrustumMatrix(GLMATRIX *m, float l, float r, float b, float t, float n, float f)
{
  m->_11 = (2.0f*n)/(r-l);
  m->_12 = 0.0f;
  m->_13 = 0.0f;
  m->_14 = 0.0f;

  m->_21 = 0.0f;
  m->_22 = (2.0f*n)/(t-b);
  m->_23 = 0.0f;
  m->_24 = 0.0f;

  m->_31 = (r+l)/(r-l);
  m->_32 = (t+b)/(t-b);
  m->_33 = (f+n)/(f-n);
  m->_34 = 1.0f;

  m->_41 = 0.0f;
  m->_42 = 0.0f;
  m->_43 = -(2.0f*f*n)/(f-n);
  m->_44 = 0.0f;

  return(m);
}



#ifndef PI
#define PI 3.14159265358979323846f
#endif

#ifndef RAD2DEG
#define RAD2DEG(fi) (((fi)/PI)*180.0f)
#endif

#define MAX_VZDAL_Z     50.0f 
#define MIN_VZDAL_Z     1.0f


__inline GLMATRIX * projection_matrix(GLMATRIX *p_mat, float fovy, float aspect, float zNear, float zFar)
{
  float xmin, xmax, ymin, ymax;

  ymax = zNear * (float)tan(fovy);
  ymin = -ymax;

  xmin = ymin * aspect;
  xmax = ymax * aspect;

  return(buildFrustumMatrix(p_mat, xmin, xmax, ymin, ymax, zNear, zFar));
}

//__inline GLMATRIX * mat_mult(GLMATRIX * a, GLMATRIX * b, GLMATRIX * r)
__inline GLMATRIX * float_to_matrix(GLMATRIX *p_mat, float uhel, BOD *p_pivot)
{
  GLMATRIX m2;
  float cosa = (float)cos(uhel);
  float sina = (float)sin(uhel);
  float px,py;


  if(!p_pivot) {
    p_mat->_11 = cosa;
    p_mat->_22 = cosa;
    p_mat->_21 = sina;
    p_mat->_12 = -sina;
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
    m2._41 = px*cosa + py*sina + p_mat->_41;
    
    m2._12 = -sina;
    m2._22 = cosa;
    m2._42 = px*(-sina) + py*cosa + p_mat->_42;   
    
    /*
      a = rot
      b = pos - 1
    */
    p_mat->_11 = m2._11;
    p_mat->_21 = m2._21;
    p_mat->_41 = m2._41 + p_mat->_44*(-px);
    
    p_mat->_12 = m2._12;
    p_mat->_22 = m2._22;
    p_mat->_42 = m2._42 + p_mat->_44*(-py);
  }
  
  return(p_mat);
}

__inline GLMATRIX * pivotuj_matrix(GLMATRIX *p_mat, BOD *p_pivot)
{
  float    px = -p_pivot->x, py = -p_pivot->y, pz = -p_pivot->z;
  float    m14, m24, m34, m41, m42, m43, m44;  

  if(px == 0.0f && py == 0.0f && pz == 0.0f)
    return(p_mat);

  // a = translate
  // b = p_mat
  m41 = px*p_mat->_11 + py*p_mat->_21 + pz*p_mat->_31 + p_mat->_41;
  m42 = px*p_mat->_12 + py*p_mat->_22 + pz*p_mat->_32 + p_mat->_42;
  m43 = px*p_mat->_13 + py*p_mat->_23 + pz*p_mat->_33 + p_mat->_43;
  m44 = px*p_mat->_14 + py*p_mat->_24 + pz*p_mat->_34 + p_mat->_44;

  m14 = p_mat->_14;
  m24 = p_mat->_24;
  m34 = p_mat->_34;

  // posun zpet
  // a = mat
  // b = translate -
  px = -px;
  py = -py;
  pz = -pz;
  p_mat->_11 += m14*(px);
  p_mat->_21 += m24*(px);
  p_mat->_31 += m34*(px);
  p_mat->_41 =  m41 + m44*(px);

  p_mat->_12 += m14*(py);
  p_mat->_22 += m24*(py);
  p_mat->_32 += m34*(py);
  p_mat->_42  = m42 + m44*(py);

  p_mat->_13 += m14*(pz);
  p_mat->_23 += m24*(pz);
  p_mat->_33 += m34*(pz);
  p_mat->_43  = m43 + m44*(pz);

  p_mat->_44 = m44;

  return(p_mat);
}

__inline GLMATRIX * pivotuj_matrix_tam(BOD *p_pivot, GLMATRIX *p_mat)
{
  float    px = -p_pivot->x, py = -p_pivot->y, pz = -p_pivot->z;

  if(px == 0.0f && py == 0.0f && pz == 0.0f)
    return(p_mat);
  else {
    // a = translate
    // b = p_mat
    p_mat->_41 += px*p_mat->_11 + py*p_mat->_21 + pz*p_mat->_31;
    p_mat->_42 += px*p_mat->_12 + py*p_mat->_22 + pz*p_mat->_32;
    p_mat->_43 += px*p_mat->_13 + py*p_mat->_23 + pz*p_mat->_33;
    p_mat->_44 += px*p_mat->_14 + py*p_mat->_24 + pz*p_mat->_34;
    
    return(p_mat);
  }
}

GLfloat * glu_invert_matrix(GLfloat * m, GLfloat * out);

__inline GLMATRIX * rovina_to_matrix(ROVINA *p_r, GLMATRIX *p_mat, float scale)
{
  p_mat->_11 = p_r->x*p_r->x*scale;
  p_mat->_21 = p_r->x*p_r->y*scale;
  p_mat->_31 = p_r->x*p_r->z*scale;
  p_mat->_41 = p_r->x*p_r->e*scale;

  p_mat->_12 = p_r->y*p_r->x*scale;
  p_mat->_22 = p_r->y*p_r->y*scale;
  p_mat->_32 = p_r->y*p_r->z*scale;
  p_mat->_42 = p_r->y*p_r->e*scale;

  p_mat->_13 = p_r->z*p_r->x*scale;
  p_mat->_23 = p_r->z*p_r->y*scale;
  p_mat->_33 = p_r->z*p_r->z*scale;
  p_mat->_43 = p_r->z*p_r->e*scale;

  p_mat->_14 = p_r->e*p_r->x*scale;
  p_mat->_24 = p_r->e*p_r->y*scale;
  p_mat->_34 = p_r->e*p_r->z*scale;
  p_mat->_44 = p_r->e*p_r->e*scale;
  
  return(p_mat);
}

__inline void matrix_to_rovina(GLMATRIX *p_mat, ROVINA *p_r)
{
  p_r->x = (float)sqrt(p_mat->_11);
  p_r->y = (float)sqrt(p_mat->_22);
  p_r->z = (float)sqrt(p_mat->_33);
  p_r->e = (float)sqrt(p_mat->_44);
}

GLMATRIX * matrix_all(GLMATRIX *m, BOD *p_pos, QUAT *p_rot, BOD *p_scs);

#endif