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

int camera_interface::global_screen_width = 0;
int camera_interface::global_screen_height = 0;

void camera_calc(GLMATRIX *p_camera, GLMATRIX *p_camera_invert, VECT *p_position, VECT *p_target, float roll)
{
  VECT    x,y,z;
  
  z = *p_target - *p_position;
  z.norm();  
  
  y.x = y.z = 0.0f; y.y = 1.0f;
  
  x = vect_mult(y,z);
  x.norm();
  
  y = vect_mult(z,x);
  y.norm();
  
  p_camera->_14 = p_camera->_24 = p_camera->_34 = 0.0f; p_camera->_44 = 1.0f;
  p_camera->_41 = p_camera->_42 = p_camera->_43 = 0.0f;
  p_camera->_11 = x.x; p_camera->_21 = x.y; p_camera->_31 = x.z;
  p_camera->_12 = y.x; p_camera->_22 = y.y; p_camera->_32 = y.z;
  p_camera->_13 = z.x; p_camera->_23 = z.y; p_camera->_33 = z.z;
  
  matrix_translate(p_camera,-p_position->x,-p_position->y,-p_position->z);
  
  if(roll != 0.0f) {
    GLMATRIX m;
    matrix_rotation_z(&m, roll);
    matrix_mult(p_camera,&m,p_camera);
  }
  
  matrix_invert(p_camera, p_camera_invert);
}

void camera_calc(GLMATRIX *p_camera, GLMATRIX *p_camera_invert, VECT *p_target, QUAT *p_rotation, float distance)
{    
  matrix_translation(p_camera_invert,p_target);
  
  matrix_rotation_set(p_camera_invert,p_rotation);
  matrix_translate(p_camera_invert,0,0,-distance);
  
  matrix_invert(p_camera_invert,p_camera);
}

void camera_calc(GLMATRIX *p_camera, GLMATRIX *p_rotation_matrix, float elevation, float distance)
{
  GLMATRIX m;
  
  m = *p_rotation_matrix;
  matrix_rotate_x(&m,elevation);
  matrix_translate(&m,0,0,-distance);
  matrix_invert(&m,p_camera);
}

void camera_calc(GLMATRIX *p_camera, GLMATRIX *p_camera_invert, VECT *p_target, float rotation, float elevation, float distance)
{
  matrix_translation(p_camera_invert,p_target);
  
  matrix_rotate_y(p_camera_invert,rotation);
  matrix_rotate_x(p_camera_invert,elevation);
  
  matrix_translate(p_camera_invert,0,0,-distance);
  matrix_invert(p_camera_invert,p_camera);
}

void camera_calc_rotation(GLMATRIX *p_camera, GLMATRIX *p_rotation_matrix, float elevation, float distance)
{
  *p_rotation_matrix = *p_camera;
  matrix_translate(p_rotation_matrix,0,0,distance);
  matrix_rotate_x(p_rotation_matrix,-elevation);
}

camera_interface::camera_interface(void)
{
  // Init screen sizes
  camera_screen_width = 0;
  camera_screen_height = 0;
  
  // Init viewport
  view_relative_set(0,0,1,1);
  
  // Init projection interface
  projection_set(DEG2RAD(DEFAULT_PROJECTION_ANGLE),
                 DEFAULT_PROJECTION_NEAR_PLANE,
                 DEFAULT_PROJECTION_FAR_PLANE);
  projection_update();
  
  // Init camera interface
  camera.init();
  camera_invert.init();
  
  projection_needs_update = FALSE;
  camera_projection_needs_update = FALSE;
  
  camera_change_clear();
  
  //plane_auto_adjust = TRUE;
  plane_auto_adjust = FALSE;
}

bool camera_interface::camera_to_screen_normal(VECT *p_vect)
{
  float x = p_vect->x;
  float y = p_vect->y;
  float z = p_vect->z;
  float w = 1;

  /*
   * Transform from camera space to normalized coordinates [-1, 1]:
   */
  matrix_transformate(&x,&y,&z,&w,projection_get());

  if(w == 0.0f)
   return(FALSE);
    
  p_vect->x = x/w;
  p_vect->y = y/w;
  p_vect->z = z/w;
      
  return(TRUE);
}

void camera_interface::camera_to_screen_normal(VECT *p_vect, int num)
{
  int i;
  for(i = 0; i < num; i++, p_vect++) {
    if(!camera_to_screen_normal(p_vect)) {
      p_vect->set(FLT_MAX);
    }
  }
}

void camera_interface::screen_normal_to_camera(VECT *p_vect)
{
  float x = p_vect->x;
  float y = p_vect->y;
  float z = p_vect->z;
  float w = 1;
  
  /*
   * Transform normalized coordinates [-1, 1] to camera space:
   */
  matrix_transformate(&x,&y,&z,&w,projection_inverted_get());
    
  p_vect->x = x/w;
  p_vect->y = y/w;
  p_vect->z = z/w;
}

void camera_interface::screen_normal_to_camera(VECT *p_vect, int num)
{
  int i;
  for(i = 0; i < num; i++, p_vect++) {
    screen_normal_to_camera(p_vect);
  }
}

// Convert vectors from/to normalized form
void camera_interface::screen_to_screen_normal(VECT *p_vect)
{
  float x = p_vect->x;
  float y = p_vect->y;
  float z = p_vect->z;
    
  /*
    Shrink x/y screen coordinates to normalized range [-1, 1]:
  
    [view_x, view_dx] => x[-1, 1]
    [view_y, view_dy] => y[-1, 1]
  */
  
  tpos vx = view_x_get();
  tpos vy = view_y_get();
  tpos vdx = view_width_get();
  tpos vdy = view_height_get();
  
  p_vect->x =     (x-vx)/(vdx>>1) - 1;
  p_vect->y = 1 - (y-vy)/(vdy>>1);
    
  /*
    Shrink z screen coordinates to normalized range [-1, 1]:
  
    [plane_near, plane_far] => z[-1, 1]
  */
  float   z_range = (plane_far-plane_near)*0.5f;
  p_vect->z =     (z-plane_near)/z_range - 1;
}

void camera_interface::screen_to_screen_normal(VECT *p_vect, int num)
{
  int i;
  for(i = 0; i < num; i++, p_vect++) {
    screen_to_screen_normal(p_vect);
  }
}

void camera_interface::screen_normal_to_screen(VECT *p_vect)
{
  float x = p_vect->x;
  float y = p_vect->y;
  float z = p_vect->z;
  
  /*
    [x,y,z] are normalized to [-1, 1]
  */
  tpos vx = view_x_get();
  tpos vy = view_y_get();
  tpos vdx = view_width_get();
  tpos vdy = view_height_get();
  
  int   x_c = vdx >> 1;
  int   y_c = vdy >> 1;
  float y_z = (plane_far-plane_near)*0.5f;
  
  /*
    Extend x/y screen coordinates:
  
    x[-1, 1] => [view_x, view_dx]
    y[-1, 1] => [view_y, view_dy]
  */
  
  p_vect->x = ftoi(x_c*(x+1))+vx;
  p_vect->y = ftoi(y_c*(1-y))+vy;
  
  /*
    Extend z screen coordinates:
  
    z[-1, 1] => [plane_near, plane_far]
  */  
  p_vect->z = y_z*(z+1)+plane_near;
}

void camera_interface::screen_normal_to_screen(VECT *p_vect, int num)
{
  int i;
  for(i = 0; i < num; i++, p_vect++) {
    screen_normal_to_screen(p_vect);
  }
}

// Convert vectors 
bool camera_interface::world_to_screen_normal_clip(VECT *p_vect)
{
  float x = p_vect->x;
  float y = p_vect->y;
  float z = p_vect->z;
  float w = 1;
  
  matrix_transformate(&x,&y,&z,&w,camera_project_get());
  
  if(w == 0.0f) {
    p_vect->x = p_vect->y = p_vect->z = FLT_MAX;
    return(FALSE);
  }
  
  bool ret = (-w <= z && z <= w && -w <= x && x <= w && -w < y && y <= w);
          
  p_vect->x = x/w;
  p_vect->y = y/w;
  p_vect->z = z/w;
            
  return(ret);
}

void camera_interface::world_to_screen_normal_clip(VECT *p_vect, int num)
{
  int i;
  for(i = 0; i < num; i++, p_vect++) {
    if(!world_to_screen_normal_clip(p_vect)) {
      p_vect->set(FLT_MAX);
    }
  }
}

// Convert vectors from/to screen
bool camera_interface::world_to_screen_normal(VECT *p_vect)
{
  float x = p_vect->x;
  float y = p_vect->y;
  float z = p_vect->z;
  float w = 1;

  matrix_transformate(&x,&y,&z,&w,camera_project_get());

  if(w == 0.0f)
   return(FALSE);
    
  p_vect->x = x/w;
  p_vect->y = y/w;
  p_vect->z = z/w;
      
  return(TRUE);
}

void camera_interface::world_to_screen_normal(VECT *p_vect, int num)
{
  int i;
  for(i = 0; i < num; i++, p_vect++) {
    if(!world_to_screen_normal(p_vect)) {
      p_vect->set(FLT_MAX);
    }
  }
}

// 2D -> 3D transformation
void camera_interface::screen_normal_to_world(VECT *p_vect)
{
  float x = p_vect->x;
  float y = p_vect->y;
  float z = p_vect->z;
  float w = 1;
  
  /*
    Transform normalized coordinates [-1, 1] to world space:
  */
  matrix_transformate(&x,&y,&z,&w,camera_project_inverted_get());
    
  p_vect->x = x/w;
  p_vect->y = y/w;
  p_vect->z = z/w;
}

void camera_interface::screen_normal_to_world(VECT *p_vect, int num)
{
  int i;
  for(i = 0; i < num; i++, p_vect++) {
    screen_normal_to_world(p_vect);
  }
}

bool camera_interface::world_to_screen(VECT *p_vect)
{
  float x = p_vect->x;
  float y = p_vect->y;
  float z = p_vect->z;
  float w = 1;

  /*
    Calculate Z coordinate -> we want to know real (not transformed) Z depth
  */  
  GLMATRIX *p_cam = camera_get();
  p_vect->z = p_cam->_13*x + p_cam->_23*y + p_cam->_33*z + p_cam->_43;
  
  /*
    Calculate X/Y coordinates
  */
  matrix_transformate(&x,&y,&z,&w,camera_project_get());

  if(w == 0.0f) {
    p_vect->x = p_vect->y = p_vect->z = FLT_MAX;
    return(FALSE);
  }
  
  bool ret = (-w <= z && z <= w && -w <= x && x <= w && -w < y && y <= w);
  
  x = x/w;
  y = y/w;
  z = z/w;
    
  /*
    [x,y,z] are transformed and normalized to [-1, 1]
  */
    
  /*
    Extend x/y screen coordinates:
  
    x[-1, 1] => [view_x, view_dx]
    y[-1, 1] => [view_y, view_dy]
  */
  tpos vx = view_x_get();
  tpos vy = view_y_get();
  tpos vdx = view_width_get();
  tpos vdy = view_height_get();  
  
  int   x_c = vdx >> 1;
  int   y_c = vdy >> 1;
  
  p_vect->x = ftoi(x_c*(x+1))+vx;
  p_vect->y = ftoi(y_c*(1-y))+vy;
    
  return(ret);
}

void camera_interface::world_to_screen(VECT *p_vect, int num)
{
  int i;
  for(i = 0; i < num; i++, p_vect++) {
    if(!world_to_screen(p_vect)) {
      p_vect->set(FLT_MAX);
    }
  }
}

bool camera_interface::world_to_screen(VECT *p_vect, VECT2DI *p_vect_out)
{
  float x = p_vect->x;
  float y = p_vect->y;
  float z = p_vect->z;
  float w = 1;
  
  /*
    Calculate X/Y coordinates
  */
  matrix_transformate(&x,&y,&z,&w,camera_project_get());

  if(w == 0.0f) {
    p_vect->set(FLT_MAX);
    return(FALSE);
  }
  
  bool ret = (-w <= z && z <= w && -w <= x && x <= w && -w < y && y <= w);
  
  x = x/w;
  y = y/w;
  z = z/w;
    
  /*
    [x,y,z] are transformed and normalized to [-1, 1]
  */
    
  /*
    Extend x/y screen coordinates:
  
    x[-1, 1] => [view_x, view_dx]
    y[-1, 1] => [view_y, view_dy]
  */
  tpos vx = view_x_get();
  tpos vy = view_y_get();
  tpos vdx = view_width_get();
  tpos vdy = view_height_get();
  
  int   x_c = vdx >> 1;
  int   y_c = vdy >> 1;
  
  p_vect_out->x = (tpos)(x_c*(x+1))+vx;
  p_vect_out->y = (tpos)(y_c*(1-y))+vy;
    
  return(ret);
}

void camera_interface::world_to_screen(VECT *p_vect, int num, VECT2DI *p_vect_out)
{
  int i;
  for(i = 0; i < num; i++, p_vect++, p_vect_out++) {
    if(!world_to_screen(p_vect,p_vect_out)) {
      p_vect_out->set(INT_MAX);
    }
  }
}

// 2D -> 3D transformation
void camera_interface::screen_to_world(VECT *p_vect)
{
  float x = p_vect->x;
  float y = p_vect->y;
  float z = p_vect->z;
  float w = 1;
  
  /*
    Shrink x/y screen coordinates to normalized range [-1, 1]:
  
    [view_x, view_dx] => x[-1, 1]
    [view_y, view_dy] => y[-1, 1]
  */
  tpos vx = view_x_get();
  tpos vy = view_y_get();
  tpos vdx = view_width_get();
  tpos vdy = view_height_get();
  
  x =     (x-vx)/(vdx>>1) - 1;
  y = 1 - (y-vy)/(vdy>>1);
  
  /*
    Shrink z screen coordinates to normalized range [-1, 1]:
  
    [plane_near, plane_far] => z[-1, 1]
  
    We want to preserve original Z value from 3D space
    so we transform Z from 3D to 2D:
  
    Z2d = Z3d*_33+W3d*_43
    W2d = Z3d*_34+W3d*_44
  
    Z = Z2d / W2d;
    
    And then this 2D Z coordinate to back to 3D space:
  */
  
  GLMATRIX *p_cp = projection_get();
  z = p_cp->_33+p_cp->_43/z;

  /*
    Transform normalized coordinates [-1, 1] to world space:
  */
  matrix_transformate(&x,&y,&z,&w,camera_project_inverted_get());
    
  p_vect->x = x/w;
  p_vect->y = y/w;
  p_vect->z = z/w;
}

void camera_interface::screen_to_world(VECT *p_vect, int num)
{
  int i;
  for(i = 0; i < num; i++, p_vect++) {
    screen_to_world(p_vect);
  }
}
