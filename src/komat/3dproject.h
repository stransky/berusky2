/* Projekcni funkce
*/
#ifndef   __3D_PROJECT_H_
#define   __3D_PROJECT_H_

int  transf_3d_2d(float p_x, float p_y, float p_z, int *p_tx, int *p_ty, float *p_tz, GLMATRIX *p_word, GLMATRIX *p_cam, GLMATRIX *p_proj, int x_c, int y_c, int xs, int ys, float z_near, float z_far);
int  transf_3d_2d_word(float p_x, float p_y, float p_z, int *p_tx, int *p_ty, float *p_tz, GLMATRIX *p_cam, GLMATRIX *p_proj, int x_c, int y_c, int xs, int ys, float z_near, float z_far);
int  transf_3d_2d_matrix(float p_x, float p_y, float p_z, int *p_tx, int *p_ty, float *p_tz, GLMATRIX *p_mat, int x_c, int y_c, int xs, int ys, float z_near, float z_far);

void transf_2d_3d(float p_x, float p_y, float p_z, float *p_tx, float *p_ty, float *p_tz, GLMATRIX *p_cam, GLMATRIX *p_proj, dword x_c, dword y_c, dword xs, dword ys);
void transf_2d_3d_world(float p_x, float p_y, float p_z,float *p_tx, float *p_ty, float *p_tz, GLMATRIX *p_world,GLMATRIX *p_cam, GLMATRIX *p_proj, dword x_c, dword y_c, dword xs, dword ys);
void transf_2d_3d_matrix(float p_x, float p_y, float p_z, float *p_tx, float *p_ty, float *p_tz, GLMATRIX *p_m, dword x_c, dword y_c, dword xs, dword ys);

void transf_2d_3d_z(float p_x, float p_y, float p_z, float *p_tx, float *p_ty, float *p_tz, GLMATRIX *p_cam, GLMATRIX *p_proj, dword x_c, dword y_c, dword xs, dword ys, float z_near, float z_far);
void transf_2d_3d_z_world(float p_x, float p_y, float p_z,float *p_tx, float *p_ty, float *p_tz, GLMATRIX *p_world,GLMATRIX *p_cam, GLMATRIX *p_proj, dword x_c, dword y_c, dword xs, dword ys, float z_near, float z_far);
void transf_2d_3d_z_matrix(float p_x, float p_y, float p_z, float *p_tx, float *p_ty, float *p_tz, GLMATRIX *p_m, dword x_c, dword y_c, dword xs, dword ys, float z_near, float z_far);

//void transf_2d_3d_z_matrix(float p_x,   float p_y,   float p_z, float *p_tx, float *p_ty, float *p_tz,  GLMATRIX *p_mat, dword x_c, dword y_c, dword xs, dword ys, float z_near, float z_far)
//void calc_2d_3d_z_matrix(GLMATRIX *p_cam, GLMATRIX *p_proj, GLMATRIX *p_vysl)

__inline int transf_3d_2d_matrix(float p_x, float p_y, float p_z, int *p_tx, int *p_ty, float *p_tz, GLMATRIX *p_mat, int x_c, int y_c, int xs, int ys, float z_near, float z_far)
{
 float x,y,z,w;

 x_c >>= 1;
 y_c >>= 1;
 
 x = p_mat->_11*(p_x) + p_mat->_21*(p_y) + p_mat->_31*(p_z) + p_mat->_41;
 y = p_mat->_12*(p_x) + p_mat->_22*(p_y) + p_mat->_32*(p_z) + p_mat->_42;
 z = p_mat->_13*(p_x) + p_mat->_23*(p_y) + p_mat->_33*(p_z) + p_mat->_43;
 w = p_mat->_14*(p_x) + p_mat->_24*(p_y) + p_mat->_34*(p_z) + p_mat->_44;

 if(w == 0.0f)
   return(FALSE);
 if(-w <= z && z <= w && -w <= x && x <= w && -w < y && y <= w) {
   *p_tx = (int)(x_c*(x/w+1))+xs;
   *p_ty = (int)(y_c*(1-y/w))+ys;
   *p_tz = z_near+((z/w+1)*0.5f)*(z_far-z_near);
   return(TRUE); 
 } else {
   return(FALSE); 
 }
}

__inline void transf_2d_3d_matrix(float p_x,   float p_y,   float p_z,
                         float *p_tx, float *p_ty, float *p_tz, 
                         GLMATRIX *p_m,
                         dword x_c, dword y_c, dword xs, dword ys)
{
 float x, y, z;
 float x3,y3,z3,w3;
 
 x_c >>= 1;
 y_c >>= 1;

 p_x -= xs;
 p_y -= ys;

 x = p_x/x_c - 1;
 y = 1 - p_y/y_c;
 z = 2*p_z - 1;

 x3 = p_m->_11*x + p_m->_21*y + p_m->_31*z + p_m->_41;
 y3 = p_m->_12*x + p_m->_22*y + p_m->_32*z + p_m->_42;
 z3 = p_m->_13*x + p_m->_23*y + p_m->_33*z + p_m->_43;
 w3 = p_m->_14*x + p_m->_24*y + p_m->_34*z + p_m->_44;

 *p_tx = x3/w3;
 *p_ty = y3/w3;
 *p_tz = z3/w3;
}

__inline void transf_2d_3d_z_matrix(float p_x,   float p_y,   float p_z,
                           float *p_tx, float *p_ty, float *p_tz, 
                           GLMATRIX *p_m,
                           dword x_c, dword y_c, dword xs, dword ys,
                           float z_near, float z_far)
{
 float a, b, c;
 float x, y, z;
 float x3,y3,z3,w3;
 
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

 x3 = p_m->_11*x + p_m->_21*y + p_m->_31*z + p_m->_41;
 y3 = p_m->_12*x + p_m->_22*y + p_m->_32*z + p_m->_42;
 z3 = p_m->_13*x + p_m->_23*y + p_m->_33*z + p_m->_43;
 w3 = p_m->_14*x + p_m->_24*y + p_m->_34*z + p_m->_44;

 *p_tx = x3/w3;
 *p_ty = y3/w3;
 *p_tz = z3/w3;
}

__inline void transf_3d_2d_matrix_float_text(float p_x, float p_y, float p_z, float *p_tx, float *p_ty, float *p_tz, GLMATRIX *p_mat, float faktor)
{
 float x,y,z,w;
 
 x = p_mat->_11*(p_x) + p_mat->_21*(p_y) + p_mat->_31*(p_z) + p_mat->_41;
 y = p_mat->_12*(p_x) + p_mat->_22*(p_y) + p_mat->_32*(p_z) + p_mat->_42;
 z = p_mat->_13*(p_x) + p_mat->_23*(p_y) + p_mat->_33*(p_z) + p_mat->_43;
 w = p_mat->_14*(p_x) + p_mat->_24*(p_y) + p_mat->_34*(p_z) + p_mat->_44;

 *p_tx = (x/w)*faktor;
 *p_ty = (y/w)*faktor;
 *p_tz = (z/w)*faktor;
}

__inline void transf_2d_3d_matrix_float_text(float p_tx, float p_ty, float p_tz, float *p_x, float *p_y, float *p_z, GLMATRIX *p_mat, float faktor)
{
 float x,y,z,w;
 
 w = 1;
 x = (p_tx*w)/faktor;
 y = (p_ty*w)/faktor;
 z = (p_tz*w)/faktor;
 
 *p_x = p_mat->_11*x + p_mat->_21*y + p_mat->_31*z + p_mat->_41;
 *p_y = p_mat->_12*x + p_mat->_22*y + p_mat->_32*z + p_mat->_42;
 *p_z = p_mat->_13*x + p_mat->_23*y + p_mat->_33*z + p_mat->_43;
 //*p_w = p_mat->_14*x + p_mat->_24*y + p_mat->_34*z + p_mat->_44;
}

#endif
