/* 
   Projekcni funkce - rene ja a rudolf, chodivame na golf
   spec podekovani - 
*/
#include "mss_on.h"
#include <windows.h>
#include "3d_all.h"


int transf_3d_2d_word(float p_x, float p_y, float p_z,
                      int *p_tx, int *p_ty, float *p_tz,
                      GLMATRIX *p_cam, GLMATRIX *p_proj,
                      int x_c, int y_c, int xs, int ys,
                      float z_near, float z_far)
{  
  float x,y,z,w,w2;

  x_c >>= 1;
  y_c >>= 1;

  p_x -= xs;
  p_y -= ys;
  
  x = p_cam->_11*p_x + p_cam->_21*p_y + p_cam->_31*p_z + p_cam->_41;
  y = p_cam->_12*p_x + p_cam->_22*p_y + p_cam->_32*p_z + p_cam->_42;
  z = p_cam->_13*p_x + p_cam->_23*p_y + p_cam->_33*p_z + p_cam->_43;
  w = p_cam->_14*p_x + p_cam->_24*p_y + p_cam->_34*p_z + p_cam->_44;
  
  p_x = x;
  p_y = y;
  p_z = z;
  w2 = w;

  x = p_proj->_11*p_x + p_proj->_21*p_y + p_proj->_31*p_z + p_proj->_41*w2;
  y = p_proj->_12*p_x + p_proj->_22*p_y + p_proj->_32*p_z + p_proj->_42*w2;
  z = p_proj->_13*p_x + p_proj->_23*p_y + p_proj->_33*p_z + p_proj->_43*w2;
  w = p_proj->_14*p_x + p_proj->_24*p_y + p_proj->_34*p_z + p_proj->_44*w2;

  if(w == 0.0f)
    return(FALSE);

  if(-w <= z && z <= w && -w <= x && x <= w && -w < y && y <= w) {
    *p_tx = ftoi(x_c*(x/w+1))+xs;
    *p_ty = ftoi(y_c*(1-y/w))+ys;    
    *p_tz = z_near+((z/w+1)*0.5f)*(z_far-z_near);
    return(TRUE);
  } else {
    return(FALSE);
 }
}

int transf_3d_2d(float p_x, float p_y, float p_z,
                 int *p_tx, int *p_ty, float *p_tz,
                 GLMATRIX *p_word, GLMATRIX *p_cam, GLMATRIX *p_proj,
                 int x_c, int y_c, int xs, int ys, 
                 float z_near, float z_far)
{
  float x,y,z,w,w2;

  x_c >>= 1;
  y_c >>= 1;

  p_x -= xs;
  p_y -= ys;
  
  x = p_word->_11*p_x + p_word->_21*p_y + p_word->_31*p_z + p_word->_41;
  y = p_word->_12*p_x + p_word->_22*p_y + p_word->_32*p_z + p_word->_42;
  z = p_word->_13*p_x + p_word->_23*p_y + p_word->_33*p_z + p_word->_43;
  w = p_word->_14*p_x + p_word->_24*p_y + p_word->_34*p_z + p_word->_44;

  p_x = p_cam->_11*x + p_cam->_21*y + p_cam->_31*z + p_cam->_41*w;
  p_y = p_cam->_12*x + p_cam->_22*y + p_cam->_32*z + p_cam->_42*w;
  p_z = p_cam->_13*x + p_cam->_23*y + p_cam->_33*z + p_cam->_43*w;
   w2 = p_cam->_14*x + p_cam->_24*y + p_cam->_34*z + p_cam->_44*w;
  
  x = p_proj->_11*p_x + p_proj->_21*p_y + p_proj->_31*p_z + p_proj->_41*w2;
  y = p_proj->_12*p_x + p_proj->_22*p_y + p_proj->_32*p_z + p_proj->_42*w2;
  z = p_proj->_13*p_x + p_proj->_23*p_y + p_proj->_33*p_z + p_proj->_43*w2;
  w = p_proj->_14*p_x + p_proj->_24*p_y + p_proj->_34*p_z + p_proj->_44*w2;

  if(w == 0.0f)
    return(FALSE);  
  if(-w <= z && z <= w && -w <= x && x <= w && -w < y && y <= w) {
    *p_tx = ftoi(x_c*(x/w+1))+xs;
    *p_ty = ftoi(y_c*(1-y/w))+ys;
    *p_tz = z_near+((z/w+1)*0.5f)*(z_far-z_near);
    return(TRUE);
  } else {
    return(FALSE);
  }
}

void transf_2d_3d(float p_x,   float p_y,   float p_z,
                  float *p_tx, float *p_ty, float *p_tz,
                  GLMATRIX *p_cam, GLMATRIX *p_proj,
                  dword x_c, dword y_c, dword xs, dword ys)
{
 GLMATRIX  m,mi; 
 float  x,y,z;
 float  x3,y3,z3,w3;

 x_c >>= 1;
 y_c >>= 1;

 p_x -= xs;
 p_y -= ys; 

 x = p_x/x_c - 1;
 y = 1 - p_y/y_c;
 z = 2*p_z - 1;

 glu_invert_matrix((float *)mat_mult(p_cam,p_proj,&mi),(float *)&m);

 x3 = m._11*x + m._21*y + m._31*z + m._41;
 y3 = m._12*x + m._22*y + m._32*z + m._42;
 z3 = m._13*x + m._23*y + m._33*z + m._43;
 w3 = m._14*x + m._24*y + m._34*z + m._44;

 *p_tx = x3/w3;
 *p_ty = y3/w3;
 *p_tz = z3/w3;
}

/*
  Problem -> 2D ->3D
*/
void transf_2d_3d_z(float p_x,   float p_y,   float p_z,
                    float *p_tx, float *p_ty, float *p_tz,
                    GLMATRIX *p_cam, GLMATRIX *p_proj,
                    dword x_c, dword y_c, dword xs, dword ys,
                    float z_near, float z_far)
{
 GLMATRIX  m,mi; 
 float  x,y,z,a,b,c;
 float  x3,y3,z3,w3;

 x_c >>= 1;
 y_c >>= 1;
 p_x -= xs;
 p_y -= ys;
 x = p_x/x_c - 1;
 y = 1 - p_y/y_c; 
 c = z_far-z_near;
 a = (z_near+z_far)/c;
 b =-(z_near*z_far)/c;
 z = a+b/p_z;

 glu_invert_matrix((float *)mat_mult(p_cam,p_proj,&mi),(float *)&m);

 x3 = m._11*x + m._21*y + m._31*z + m._41;
 y3 = m._12*x + m._22*y + m._32*z + m._42;
 z3 = m._13*x + m._23*y + m._33*z + m._43;
 w3 = m._14*x + m._24*y + m._34*z + m._44;
 
 *p_tx = x3/w3;
 *p_ty = y3/w3;
 *p_tz = z3/w3;
}

void transf_2d_3d_world(float p_x,   float p_y,   float p_z,
                        float *p_tx, float *p_ty, float *p_tz, 
                        GLMATRIX *p_world, GLMATRIX *p_cam, GLMATRIX *p_proj, 
                        dword x_c, dword y_c, dword xs, dword ys)
{
 GLMATRIX  m,mi; 
 float  x,y,z,w; 

 x_c >>= 1;
 y_c >>= 1;

 p_x -= xs;
 p_y -= ys;

 x = p_x/x_c - 1;
 y = 1 - p_y/y_c;
 z = 2*p_z - 1;

 mat_mult(p_world,p_cam,&m);
 mat_mult(&m,p_proj,&mi);
 glu_invert_matrix((float *)&mi,(float *)&m); 

 w = m._14*x + m._24*y + m._34*z + m._44;
 *p_tx = (m._11*x + m._21*y + m._31*z + m._41)/w;
 *p_ty = (m._12*x + m._22*y + m._32*z + m._42)/w;
 *p_tz = (m._13*x + m._23*y + m._33*z + m._43)/w;
}

void transf_2d_3d_z_world(float p_x,   float p_y,   float p_z,
                          float *p_tx, float *p_ty, float *p_tz, 
                          GLMATRIX *p_world, GLMATRIX *p_cam, GLMATRIX *p_proj, 
                          dword x_c, dword y_c, dword xs, dword ys,
                          float z_near, float z_far)
{
 GLMATRIX  m,mi; 
 float  x,y,z;
 float  a,b,c;
 float  x3,y3,z3,w3;

 x_c >>= 1;
 y_c >>= 1;

 p_x -= xs;
 p_y -= ys;

 x = p_x/x_c - 1;
 y = 1 - p_y/y_c; 
 c = z_far-z_near;
 a = (z_near+z_far)/c;
 b =-(z_near*z_far)/c;
 z = a+b/p_z;

 mat_mult(p_world,p_cam,&m);
 mat_mult(&m,p_proj,&mi);
 glu_invert_matrix((float *)&mi,(float *)&m); 

 x3 = m._11*x + m._21*y + m._31*z + m._41;
 y3 = m._12*x + m._22*y + m._32*z + m._42;
 z3 = m._13*x + m._23*y + m._33*z + m._43;
 w3 = m._14*x + m._24*y + m._34*z + m._44;
 
 *p_tx = x3/w3;
 *p_ty = y3/w3;
 *p_tz = z3/w3;
}

