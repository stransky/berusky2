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
  Geometric pipeline - propagates changes directly to OpenGL  
*/

#ifndef __GEOMETRIC_PIPELINE_H__
#define __GEOMETRIC_PIPELINE_H__

#define MAX_WORD_MATRIX_STACK           32

// matmult top_matrix*wordl*camera*project
// Geometric pipeline
typedef class gpipe :
  public camera_interface
{   
  // World matrix
  WORLD_INTERFACE matrix_word;
  
  // World-matrix stack
  GLMATRIX top_matrix[MAX_WORD_MATRIX_STACK];
  int      top_matrix_act;
  
  // Final model-view matrix
  GLMATRIX modelview;

  // State variables
private:
  
  bool     online;
  bool     matrix_2d_mode;

private:

  void projection_upload(void)
  {
    if(online) {      
      glMatrixMode(GL_PROJECTION);
      glLoadMatrixf((float *)(projection_get()));
    }
  }
  
private:

  void modelview_upload(void)
  {   
    if(top_matrix_act) {
      GLMATRIX tmp;
      matrix_mult_dir(matrix_word.world_get(),top_matrix+top_matrix_act-1,&tmp);
      matrix_mult_dir(&tmp,camera_get(),&modelview);
    } else {
      matrix_mult_dir(matrix_word.world_get(),camera_get(),&modelview);
    }
    
    if(online) {
      glMatrixMode(GL_MODELVIEW);
      glLoadMatrixf((float *)&modelview);
    }
  }
  
public:
  
  void view_upload(void)
  {      
    if(online) {      
      int sx,sy,dx,dy;
      view_get(&sx,&sy,&dx,&dy);
      sy = camera_interface::global_screen_height - sy - dy;
      glViewport(sx,sy,dx,dy);
    }
  }

  void view_absolute_set(int x, int y, int width, int height)
  {
    camera_interface::view_absolute_set(x, y, width, height);
    
    view_upload();
    projection_upload();
    
    camera_change_set();
  }
  
  void view_relative_set(float x, float y, float width, float height)
  {
    camera_interface::view_relative_set(x, y, width, height);
    
    view_upload();
    projection_upload();
    
    camera_change_set();
  }

  void view_set(camera_interface *p_camera)
  {
    camera_interface::view_set(p_camera);
    
    view_upload();    
    projection_upload();
    
    camera_change_set();
  }
  
public:  

  void projection_set(GLMATRIX *p_projection)
  {
    camera_interface::projection_set(p_projection);    
    projection_upload();
    camera_change_set();
  }

  void projection_set(float fovy, float zNear, float zFar)
  { 
    camera_interface::projection_set(fovy,zNear,zFar);    
    projection_upload();
    camera_change_set();
  }
    
  void projection_fov_set(float fovy)
  {
    camera_interface::projection_fov_set(fovy);    
    projection_upload();
    camera_change_set();
  }
  
  void projection_planes_set(float zNear, float zFar)
  {
    camera_interface::projection_planes_set(zNear, zFar);        
    projection_upload();
    camera_change_set();
  }
  
public:  
  
  void camera_set(GLMATRIX *p_camera)
  {
    camera_interface::camera_set(p_camera);
    modelview_upload();
    camera_change_set();
  }
  void camera_set(GLMATRIX *p_camera, GLMATRIX *p_camera_invert)
  {
    camera_interface::camera_set(p_camera,p_camera_invert);
    modelview_upload();
    camera_change_set();
  }
  void camera_invert_set(GLMATRIX *p_camera_invert)
  {
    camera_interface::camera_invert_set(p_camera_invert);
    modelview_upload();
    camera_change_set();
  }  
  
public:
  
  void world_set(GLMATRIX *p_world)
  {
    matrix_word.world_set(p_world);
    modelview_upload();
  }

  void world_init(void)
  {    
    matrix_word.world_init();
    modelview_upload();    
  }
  
public:
  
  void world_push(GLMATRIX *p_world)
  {
    assert(top_matrix_act+1 < MAX_WORD_MATRIX_STACK);
    
    if(!top_matrix_act) {
      top_matrix[0] = *p_world;
    }
    else {
      matrix_mult_dir(top_matrix+top_matrix_act-1,p_world,top_matrix+top_matrix_act);
    }
    top_matrix_act++;
    
    modelview_upload();
  }

  void world_pop(void)
  {
    if(top_matrix_act > 0) {
      top_matrix_act--;      
      modelview_upload();
    }
  }

public:

  GLMATRIX * modelview_get(void)
  {
    return(&modelview);
  }
  
  void modelview_set(GLMATRIX *p_camera, GLMATRIX *p_world)
  {
    camera_interface::camera_set(p_camera);
    matrix_word.world_set(p_world);
    modelview_upload();
    camera_change_set();
  }  

  // Independent 2D mode
public:

  void matrix_2d_mode_set(void)
  {
    if(!matrix_2d_mode) {
      matrix_2d_mode = TRUE;
      if(online) {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix(); 
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0,view_width_get(),view_height_get(),0.0f,0.0f,1.0f);
      }
    }
  }
  
  void matrix_3d_mode_set(void)
  {
    if(matrix_2d_mode) {
      matrix_2d_mode = FALSE;
      if(online) {
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
      }
    }
  }

public:

  void set(camera_interface *p_camera)
  { 
    camera_interface::set(p_camera);
    
    view_upload();
    projection_upload();
    modelview_upload();
    camera_change_set();
  }

public:
  
  bool render_state_get(void)
  {
    return(online);
  }
  
  bool render_state_set(bool render)
  {
    online = render;
    if(online && view_width_get() <= 0 && view_height_get() <= 0) {
      online = FALSE;
    }
    return(online == render);
  }
  
public:
  
  gpipe(void)
  {
    online = FALSE;
    top_matrix_act = 0;
    view_upload();
    projection_upload();
    modelview_upload();
  }
  
} GEOMETRIC_PIPELINE, GPIPE;

/*
  TODO -> 2D/3D transformations and so on
*/
extern GPIPE *p_gpipe;

inline GPIPE * gpipe_get(void)
{
  return(p_gpipe);
}

inline void gpipe_set(GPIPE *p_active)
{
  p_gpipe = p_active;
}

#endif
