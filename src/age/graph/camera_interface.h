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
  Camera interface update model
  
  -> TODO ->camera->projection matrix

  - metoda - matrix changed -> dam najevo ze sem editoval matici

*/

#ifndef __CAMERA_INTERFACE_H__
#define __CAMERA_INTERFACE_H__

#define DEFAULT_VIEWPORT_WIDTH            640
#define DEFAULT_VIEWPORT_HEIGHT           480

#define DEFAULT_PROJECTION_ANGLE          30.0f
#define DEFAULT_PROJECTION_NEAR_PLANE     1.0f
#define DEFAULT_PROJECTION_FAR_PLANE      100.0f
#define DEFAULT_PROJECTION_FAR_PLANE_MAX  FLT_MAX

// Global camera functions
void camera_calc(GLMATRIX *p_camera, GLMATRIX *p_camera_invert, 
                 VECT *p_position, VECT *p_target, float roll);
void camera_calc(GLMATRIX *p_camera, GLMATRIX *p_camera_invert, 
                 VECT *p_target, QUAT *p_rotation, float distance);
void camera_calc(GLMATRIX *p_camera, GLMATRIX *p_rotation_matrix, 
                 float elevation, float distance);
void camera_calc(GLMATRIX *p_camera, GLMATRIX *p_camera_invert, 
                 VECT *p_target, float rotation, float elevation, float distance);
void camera_calc_rotation(GLMATRIX *p_camera, GLMATRIX *p_rotation_matrix, 
                 float elevation, float distance);

// Viewport configuration
typedef class camera_interface {
  
  /*
   * Projection matrix has to be recomputed
   */
protected:
    
  bool    projection_needs_update;

  /*
   * Camera-projection matrix has to be recomputed
   */
protected:

  bool    camera_projection_needs_update;

  /*
   * Camera matrix has been changed
   */
private:

  bool      camera_change;
  
  /*
   * Camera viewport
   */
private:
  
  bool      view_relative;

  union {
    int      view_ax;
    float    view_rx;
  };
  union {
    int      view_ay;
    float    view_ry;
  };
  union {
    int      view_adx;
    float    view_rdx;
  };
  union {
    int      view_ady;
    float    view_rdy;
  };
  
public:
  
  void view_absolute_set(int x, int y, int widht, int height)
  {
    view_relative = FALSE;
    
    view_ax = x;
    view_ay = y;
    view_adx = widht;
    view_ady = height;
    
    projection_needs_update = TRUE;
    camera_projection_needs_update = TRUE;
  }
  
  void view_relative_set(float x, float y, float widht, float height)
  {
    view_relative = TRUE;
    
    view_rx = x;
    view_ry = y;
    view_rdx = widht;
    view_rdy = height;
    
    projection_needs_update = TRUE;
    camera_projection_needs_update = TRUE;
  }
  
  void view_set(class camera_interface *p_src)
  {
    view_relative = p_src->view_relative;
    
    if(p_src->view_relative) {
      view_rx = p_src->view_rx;
      view_ry = p_src->view_ry;
      view_rdx = p_src->view_rdx;
      view_rdy = p_src->view_rdy;      
    }
    else {
      view_ax = p_src->view_ax;
      view_ay = p_src->view_ay;
      view_adx = p_src->view_adx;
      view_ady = p_src->view_ady;      
    }
    
    projection_needs_update = TRUE;
    camera_projection_needs_update = TRUE;
  }

  int view_x_get(void)
  {
    return(view_relative ? view_ax*camera_screen_width : view_ax);
  }
  
  int view_y_get(void)
  {
    return(view_relative ? view_ay*camera_screen_height : view_ay);
  }  
  
  int view_width_get(void)
  {
    return(view_relative ? ftoi(view_rdx*camera_screen_width) : view_adx);
  }
  
  int view_height_get(void)
  {
    return(view_relative ? ftoi(view_rdy*camera_screen_height) : view_ady);
  }
  
  void view_get(int *p_x, int *p_y, int *p_width, int *p_height)
  {
    if(p_x)
      *p_x = view_x_get();
    if(p_y)
      *p_y = view_y_get();
    if(p_width)
      *p_width = view_width_get();
    if(p_height)
      *p_height = view_height_get();
  }
  
  float view_aspect_get(void)
  {
    if(view_relative) {
      if(view_rdx == 0.0f || view_rdy == 0.0f) {
        return(0);
      }
      else {
        return(view_rdx/view_rdy);
      }
    }
    else {
      if(view_adx == 0 || view_ady == 0) {
        return(0);
      }
      else {
        return((float)view_adx/(float)view_ady);
      }      
    }
  }    
  
  /*
   * Camera screen size
   */
public:

  static int global_screen_width;
  static int global_screen_height;

public:

  static void screen_size_set(int width, int height)
  {
    global_screen_width = width;
    global_screen_height = height;
  }  
  
private:
  
  int camera_screen_width;
  int camera_screen_height;

public:
  
  int camera_screen_width_get(void)
  {
    return(camera_screen_width);
  }
  
  int camera_screen_height_get(void)
  {
    return(camera_screen_height);
  }

private:

  bool screen_size_changed(void)
  {
    return(camera_screen_width != global_screen_width || 
           camera_screen_height != global_screen_height);
  }
  
  void screen_size_update(void)
  {
    camera_screen_width = global_screen_width; 
    camera_screen_height = global_screen_height;
  }
  
private:
  
  float     fov;                // camera's field-of-view
  float     plane_far;          // far/near clipping planes
  float     plane_near;

private:
  
  bool      plane_auto_adjust;  // Auto adjust a far clipping plane
  
public:
  
  bool plane_auto_adjust_get(void)
  {
    return(plane_auto_adjust);
  }
  
  void plane_auto_adjust_set(bool adjust)
  {
    plane_auto_adjust = adjust;
  }

private:

  GLMATRIX  project;            // a projection matrix
  GLMATRIX  project_inverted;   // a inverted projection matrix
  
private:
  
  void projection_update(void)
  {
    if(screen_size_changed()) {
      screen_size_update();
      projection_needs_update = TRUE;
    }
    
    if(projection_needs_update) {
      
      project.projection(fov, view_aspect_get(), plane_near, plane_far);
      glu_invert_matrix(&project, &project_inverted);
      
      projection_needs_update = FALSE;
      camera_projection_needs_update = TRUE;
    }
  }

public:
  
  GLMATRIX * projection_get(GLMATRIX *p_projection)
  {     
    projection_update();
    *p_projection = project;
    return(p_projection);
  }
  
  GLMATRIX * projection_inverted_get(GLMATRIX *p_projection_inverted)
  { 
    projection_update();
    *p_projection_inverted = project_inverted;
    return(p_projection_inverted);
  }

public:  
  
  GLMATRIX * projection_get(void)
  {     
    projection_update();
    return(&project);
  }
  
  GLMATRIX * projection_inverted_get(void)
  { 
    projection_update();
    return(&project_inverted);
  }
  
public:
  
  void projection_set(class camera_interface *p_src)
  {
    project = p_src->project;
    project_inverted = p_src->project_inverted;
  
    fov = p_src->fov;
    plane_far = p_src->plane_far;
    plane_near = p_src->plane_near;
    
    projection_needs_update = FALSE;
    camera_projection_needs_update = TRUE;
  }
  
  void projection_set(GLMATRIX *p_projection)
  {
    project = *p_projection;
    glu_invert_matrix(&project, &project_inverted);
    
    projection_needs_update = FALSE;
    camera_projection_needs_update = TRUE;
  }
  
  void projection_set(float fovy, float zNear, float zFar)
  { 
    fov = fovy;
    plane_near = zNear;
    plane_far = zFar;
    
    projection_needs_update = TRUE;
    camera_projection_needs_update = TRUE;
  }
    
  float projection_fov_get(void)
  {
    return(fov);
  }
  
  void projection_fov_set(float fovy)
  {
    fov = fovy;
    
    projection_needs_update = TRUE;
    camera_projection_needs_update = TRUE;
  }
  
  void projection_planes_set(float zNear, float zFar)
  {
    plane_near = zNear;
    plane_far = zFar;
    
    projection_needs_update = TRUE;
    camera_projection_needs_update = TRUE;
  }
  
  void projection_far_plane_set(float zFar)
  {    
    plane_far = zFar;
    
    projection_needs_update = TRUE;
    camera_projection_needs_update = TRUE;
  }

  void projection_planes_get(float *p_zNear, float *p_zFar)
  {
    *p_zNear = plane_near;
    *p_zFar = plane_far;
  }
  
private:
  
  // Current camera configuration
  GLMATRIX camera;
  GLMATRIX camera_invert;

public:
  
  // Return camera matrix
  virtual GLMATRIX * camera_get(GLMATRIX *p_camera)
  {
    *p_camera = camera;
    return(p_camera);
  }  
  virtual GLMATRIX * camera_invert_get(GLMATRIX *p_camera_invert)
  {
    *p_camera_invert = camera_invert;
    return(p_camera_invert);
  }

protected:

  // Return camera matrix
  virtual GLMATRIX * camera_get(void)
  {
    return(&camera);
  }  
  virtual GLMATRIX * camera_invert_get(void)
  {
    return(&camera_invert);
  }
  
public:
    
  void camera_set(GLMATRIX *p_camera)
  {
    camera = *p_camera;
    matrix_invert(&camera,&camera_invert);
    
    camera_projection_needs_update = TRUE;
  }
  void camera_set(class camera_interface *p_src)
  {
    camera = p_src->camera;
    camera_invert = p_src->camera_invert;
    camera_projection_needs_update = TRUE;
  }
  void camera_set(GLMATRIX *p_camera, GLMATRIX *p_camera_invert)
  {
    camera = *p_camera;
    camera_invert = *p_camera_invert;
    camera_projection_needs_update = TRUE;
  }
  void camera_set(GLMATRIX *p_camera, GLMATRIX *p_camera_invert, GLMATRIX *p_projection)
  {
    camera = *p_camera;
    camera_invert = *p_camera_invert;
    project = *p_projection;
    glu_invert_matrix(&project,&project_inverted);
    
    camera_projection_needs_update = TRUE;
  }
  
public:
  
  void camera_invert_set(GLMATRIX *p_camera_invert)
  {
    camera_invert = *p_camera_invert;
    matrix_invert(&camera_invert,&camera);
    camera_projection_needs_update = TRUE;
  }

public:
  
  void camera_update(void)
  {
    matrix_invert(&camera_invert,&camera);
    camera_projection_needs_update = TRUE;
  }

public:  
  
  VECT * position_get(VECT *p_position)
  {
    GLMATRIX *p_camera = camera_invert_get();
    p_position->x = p_camera->_41;
    p_position->y = p_camera->_42;
    p_position->z = p_camera->_43;
    return(p_position);
  }

  VECT * direction_get(VECT *p_direction)
  {
    GLMATRIX *p_camera = camera_invert_get();
    p_direction->x = p_camera->_13;
    p_direction->y = p_camera->_23;
    p_direction->z = p_camera->_33;
    return(p_direction);
  }
  
private:
    
  // Those two are computed only on request
  GLMATRIX camera_project;
  GLMATRIX camera_project_invert;

public:
  
  // Projection->camera matrix is computed only on demand
  void camera_projection_update(void)
  {
    if(camera_projection_needs_update) {
      matrix_mult_dir(camera_get(),projection_get(),&camera_project);
      glu_invert_matrix(&camera_project,&camera_project_invert);      
      camera_projection_needs_update = FALSE;
    }
  }
  
public:

  GLMATRIX * camera_project_get(void)
  {    
    camera_projection_update();
    return(&camera_project);
  }
  GLMATRIX * camera_project_inverted_get(void)
  {
    camera_projection_update();
    return(&camera_project_invert);
  }
  
public:

  void set(class camera_interface *p_src)
  {
    p_src->update();
    *this = *p_src;
  }
  
  /*
    Transformation from world space <=> screen space:
      
    Geometry pipeline:
  
    World space <=> Camera space <=> Screen nomalized <=> Screen
  
   */
    
  /*
   * Camera space <=> Screen nomalized
   */
public:

  bool camera_to_screen_normal(VECT *p_vect);
  void camera_to_screen_normal(VECT *p_vect, int num);

  void screen_normal_to_camera(VECT *p_vect);
  void screen_normal_to_camera(VECT *p_vect, int num);

  /*
   * Screen nomalized <=> Screen
   */
public:
    
  void screen_to_screen_normal(VECT *p_vect);
  void screen_to_screen_normal(VECT *p_vect, int num);

  void screen_normal_to_screen(VECT *p_vect);
  void screen_normal_to_screen(VECT *p_vect, int num);

  /*
   * World space <=> Camera space
   */
public:

  void world_to_camera(VECT *p_vect)
  {
    matrix_transformate(p_vect,camera_get());
  }  
  void world_to_camera(VECT *p_vect, int num)  
  {
    int i;
    for(i = 0; i < num; i++, p_vect++) {
      world_to_camera(p_vect);
    }
  }
  
  void camera_to_world(VECT *p_vect)
  {
    matrix_transformate(p_vect,camera_invert_get());
  }
  void camera_to_world(VECT *p_vect, int num)
  {
    int i;
    for(i = 0; i < num; i++, p_vect++) {
      camera_to_world(p_vect);
    }    
  }

  /*
   * World space <=> Camera space <=> Screen nomalized
   */
public:
  
  bool world_to_screen_normal_clip(VECT *p_vect);
  void world_to_screen_normal_clip(VECT *p_vect, int num);

  bool world_to_screen_normal(VECT *p_vect);
  void world_to_screen_normal(VECT *p_vect, int num);

  void screen_normal_to_world(VECT *p_vect);
  void screen_normal_to_world(VECT *p_vect, int num);

  /*
   * World space <=> Camera space <=> Screen nomalized <=> Screen
   */
public:
  
  bool world_to_screen(VECT *p_vect);
  bool world_to_screen(VECT *p_vect, VECT2DI *p_vect_out);

  void world_to_screen(VECT *p_vect, int num);
  void world_to_screen(VECT *p_vect, int num, VECT2DI *p_vect_out);

  void screen_to_world(VECT *p_vect);
  void screen_to_world(VECT *p_vect, int num);
  
  /*
   * Camera space <=> Screen nomalized <=> Screen
   */
public:
    
  void screen_to_camera(VECT *p_vect);
  void screen_to_camera(VECT *p_vect, int num);

  void camera_to_screen(VECT *p_vect);
  void camera_to_screen(VECT *p_vect, int num);

public:
   
  bool camera_changed(void)
  {
    return(camera_change);
  }
  
  void camera_change_set(void)
  {
    camera_change = TRUE;
  }  
  
  void camera_change_clear(void)
  {
    camera_change = FALSE;
  }  

public:  
  
  virtual void update(void)
  {
  }
  
public:
  
  camera_interface(void);  
  virtual ~camera_interface(void) {};
    
} CAMERA_INTERFACE;

#define NORMALIZED_IN_RANGE(value)  (value >= -1 && value < 1)

#endif // __CAMERA_INTERFACE_H__
