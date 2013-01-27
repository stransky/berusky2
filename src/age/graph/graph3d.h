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
  2D/3D Graphics library
*/

#ifndef  __GRAPH3D_H__
#define  __GRAPH3D_H__

#include <assert.h>
#include <limits.h>

#ifdef ENABLE_GTK_BACKEND
  #include <gtk/gtk.h>
  #include <gdk/gdkkeysyms.h>  
  #include <gtkgl/gtkglarea.h>
#endif


/* 
 * Graphics dirs
 */
typedef struct grf_dirs {

  char mesh_dir[MAX_FILENAME];
  char material_dir[MAX_FILENAME];
  char scene_dir[MAX_FILENAME];
  
public:

  void mesh_dir_set(char *p_dir)
  {
    strncpy(mesh_dir,p_dir,MAX_FILENAME);
  }

  char * mesh_dir_get(void)
  {
    return(mesh_dir);
  }

  void material_dir_set(char *p_dir)
  {
    strncpy(material_dir,p_dir,MAX_FILENAME);
  }

  char * material_dir_get(void)
  {
    return(material_dir);
  }

  void scene_dir_set(char *p_dir)
  {
    strncpy(scene_dir,p_dir,MAX_FILENAME);
  }

  char * scene_dir_get(void)
  {
    return(scene_dir);
  }

} GRF_DIRS;

/* 
 * Mouse cursors
 */
typedef enum {
  
  MOUSE_CURSOR_NORMAL = 0,
  MOUSE_CURSOR_RECTANGLE_SELECT,
  MOUSE_CURSOR_RECTANGLE_SELECTING,
  MOUSE_CURSOR_SCENE_MOVE,
  MOUSE_CURSOR_SCENE_MOVING,
  MOUSE_CURSOR_SCENE_ROTATE,
  MOUSE_CURSOR_SCENE_ROTATING
  
} MOUSE_CURSOR_TYPE;

/* 
 * Render mode (high-level)
 */
typedef enum {

  RENDER_POINTS,
  RENDER_WIRED,
  RENDER_SOLID,
  RENDER_TEXTURED
  
} RENDER_MODE;

/* 
 * Graphics config
 */
typedef struct grf_config {

  bool opengl_lighting;
  bool debug_culling;
  
  bool draw_bouding_boxes;
  bool draw_grid;
  bool draw_mouse_cursor;
  bool draw_console;
  bool draw_pivots;
  bool draw_selection;
  bool draw_selection_rectangle;
  
  bool draw_debug_fps;
  bool draw_debug_wired;
  bool draw_debug_points;
  bool draw_debug_normals;
  bool draw_debug_materials;
  bool draw_debug_all_objects;
    
} GRF_CONFIG;

/* 
 * Error handling
 */
void gl_error(void);

/* 
 * Type of graphics backend
 */
typedef enum {
  
  GRAPH_GTK,
  GRAPH_SDL
  
} GRAPH_TYPE;

// -------------------------------------------------------
//   the graph3d store class
// -------------------------------------------------------
/*
  A frame is rendered by sequence:

  1. draw_start()
  2. draw_end()

*/
typedef class graph3d :  
  public grf_console_interface,
  public grf_dirs
{

private:
  
  GRAPH_TYPE graph_type;

public:
  
  GRAPH_TYPE type_get(void)
  {
    return(graph_type);
  }

  /* 
   * Configuration
   */
public:
  
  static GRF_CONFIG config;

  /*
   * Current active scene
   */
private:
  
  SCENE *p_scene_active;

public:
  
  void scene_active_set(SCENE *p_scene)
  {
    p_scene_active = p_scene;
  }

  SCENE * scene_active_get(void)
  {
    return(p_scene_active);
  }
  
  /* 
   * Frame interface
   */  
private:
  
  int   current_frame;
  float current_fps;

public:

  void current_frame_set(int frame)
  {
    current_frame = frame;
    if(p_scene_active)
      p_scene_active->current_frame_set(frame);
  }
  
  void current_fps_set(float fps)
  {
    current_fps = fps;
  }
  
public:

  void fill(tcolor color)
  {
  
  }

  void fill(tpos x, tpos y, tpos dx, tpos dy, tcolor color)
  {
  
  }

  void fill(RECT2DI *p_rect, tcolor color)
  {
  
  }

  void fill(void)
  {
    
  }
  
  // Drawing sequence:
  // 1. draw_start
  // 2. draw_end
public:

  void draw_start(void);
  void draw_end(void);

public:
  
  // Flip GL buffers
  virtual void flip(void) = 0;

private:

  // Clear buffers
  void clear(void);

  void grid_draw(void);
  void fps_draw(void);

public:
  
  void config_draw_grid(bool state);
  void config_draw_boxes(bool state);
  void config_draw_console(bool state);
  void config_draw_mouse_cursor(bool state);
  void config_draw_pivots(bool state);
  void config_draw_fps(bool state);
  void config_draw_points(bool state);
  void config_draw_wired(bool state);
  void config_draw_normals(bool state);
  void config_draw_materials(bool state);
  void config_opengl_lighting(bool state);
  void config_draw_all_objects(bool state);
  void config_draw_selection(bool state);
  void config_draw_selection_rectangle(bool state);

public:
  
  void config_render_mode_set(RENDER_MODE mode);

private:
  
  bool  screen_clear;
  RGBAF screen_clear_color;

public:
  
  void screen_clear_color_set(float r, float g, float b, float a)
  {
    screen_clear_color = RGBAF(r,g,b,a);
    glClearColor(screen_clear_color.r,screen_clear_color.g,screen_clear_color.b,screen_clear_color.a);
  }
  
  void screen_clear_color_set(RGBAF color)
  {
    screen_clear_color = color;
    glClearColor(screen_clear_color.r,screen_clear_color.g,screen_clear_color.b,screen_clear_color.a);
  }

private:
  
  float color_zbuffer;

public:
  
  void z_buffer_clear_set(float z)
  {
    color_zbuffer = z;
  }

private:

  /* 
   * Basic drawing
   */
public:
  
  void draw_rect_line_2d(int x1, int y1, int x2, int y2, RGBB color = RGBB(255,255,255));
  void draw_rect_solid_2d(int x1, int y1, int x2, int y2, RGBB color = RGBB(255,255,255));

  void draw_line_3d(VECT *p_s, VECT *p_k, RGBB color = RGBB(255,255,255));
  void draw_line_3d(float sx, float sy, float sz, float ex, float ey, float ez, RGBB color = RGBB(255,255,255));
  
  void draw_cursor_normal(int mx, int my, int sx, int sy, int xres, int yres, RGBB color = RGBB(255,255,255));
  void draw_cursor_rect_select(int mx, int my, RGBB color = RGBB(255,255,255));

  void draw_cube_line(VECT *p_center, VECT *p_size, RGBB color = RGBB(255,255,255));
  void draw_cube_line(float sx, float sy, float sz, float dx, float dy, float dz, RGBB color = RGBB(255,255,255));
  
  void draw_cube_solid(VECT *p_center, VECT *p_size, RGBB color = RGBB(255,255,255));
  void draw_cube_solid(float sx, float sy, float sz, float dx, float dy, float dz, RGBB color = RGBB(255,255,255));
  
  void draw_label(VECT *p_position, const char *p_label, RGBB color = RGBB(255,255,255));
  void draw_label(float x, float y, float z, const char *p_label, RGBB color = RGBB(255,255,255));
  
  void draw_floor_line(VECT *p_center, float size, int lines_min, int lines_max);

  void draw_camera(VECT *p_position, VECT *p_target, float fov, RGBB color = RGBB(255,255,255));
  void draw_camera(CAMERA_INTERFACE *p_camera, float size, RGBB color = RGBB(255,255,255));

  /* 
   * Axis cross
   */
public:

  #define CROSS_COLOR_AXIS_X  (1<<0)  // Draw x axis as active
  #define CROSS_COLOR_AXIS_Y  (1<<1)  // Draw y axis as active
  #define CROSS_COLOR_AXIS_Z  (1<<2)  // Draw z axis as active
  #define CROSS_AXIS_NAMES    (1<<3)  // Draw axis names
  #define CROSS_WORLD_SPACE   (1<<4)  // Draw cros in the global worldspace

  #define CROSS_DEFAULT_FLAGS (CROSS_COLOR_AXIS_X|CROSS_COLOR_AXIS_Y|CROSS_COLOR_AXIS_Z|CROSS_AXIS_NAMES)

  void draw_cross(VECT *p_position, float size, tflag flags = CROSS_DEFAULT_FLAGS);

  /* 
   * Selection rectangle
   */
private:
  
  bool    selection_rectangle_active;

  RGBB    selection_rectangle_color;

  VECT2DI selection_rectangle_start;
  VECT2DI selection_rectangle_stop;

private:

  void selection_rectangle_draw(void);

public:

  void selection_rectangle_clear(void);
  void selection_rectangle_set_start(void);
  void selection_rectangle_set(void);
  bool selection_rectangle_get(RECT2DI *p_r);

  /* 
   * Graphics configuration
   */
private:

  MOUSE_CURSOR_TYPE mouse_cursor_type;

public:

  void mouse_cursor_set(MOUSE_CURSOR_TYPE type = MOUSE_CURSOR_NORMAL)
  {
    mouse_cursor_type = type;
  }
  
  void mouse_cursor_draw(void);

  /* 
   * Graphics configuration
   */
public:

  bool  graphics;
  tpos  graphics_width;
  tpos  graphics_height;
  int   graphics_bpp;
  int   graphics_bpp_zbuffer;
  bool  graphics_opengl;
  bool  graphics_fullscreen;

public:
  
  void set(void);  
  void set(tpos width, tpos height, int screen_depth = 0, bool full_screen = FALSE);
  void get(tpos *p_width, tpos *p_height, int *p_screen_depth);

public:

  bool create_GL(void);
  void destroy_GL(void);

public:

  virtual bool screen_create(void) = 0;
  virtual void screen_destroy(void) = 0;
  virtual void screen_resize(tpos width, tpos height) = 0;

public:

  void destroy(void);

public:

  graph3d(GRAPH_TYPE type);
  graph3d(GRAPH_TYPE type, tpos width, tpos height,
          int screen_depth = BPP_DEFAULT, bool full_screen = FALSE);

  virtual ~graph3d(void);

} GRAPH3D;

#ifdef ENABLE_GTK_BACKEND

// Make our GTK widget ready for rendering
inline bool gtk_gl_rendering(GtkWidget *widget)
{
  assert(widget != NULL);
  return(gtk_gl_area_make_current(GTK_GL_AREA(widget)));
}

// GTK backend for graph3d
typedef class graph3d_gtk : 
  public graph3d
{  
  /* 
   * GTK widget
   */
private:
  
  GtkWidget * p_area;
  
public:

  GtkWidget * gtk_widget_get(void)
  {
    return(p_area);
  }

public:  
  
  // flip GL buffers
  void flip(void);

public:
  
  virtual bool screen_create(void);
  virtual void screen_destroy(void);
  virtual void screen_resize(tpos width, tpos height);
  
public:

  graph3d_gtk(void);
  graph3d_gtk(tpos width, tpos height);

  ~graph3d_gtk(void);
  
} GRAPH3D_GTK;

#endif // ENABLE_GTK_BACKEND

#ifdef ENABLE_SDL_BACKEND

// SDL backend for graph3d
typedef class graph3d_sdl :
  public graph3d
{
  
  /* 
   * Screen
   */
private:

  SURFACE *p_screen_surface;

public:
  
  // flip GL buffers
  void flip(void);

public:
  
  virtual bool screen_create(void);
  virtual void screen_destroy(void);
  virtual void screen_resize(tpos width, tpos height);

public:

  graph3d_sdl(void);
  graph3d_sdl(tpos width, tpos height, 
              int screen_depth = BPP_DEFAULT, bool full_screen = FALSE);
  ~graph3d_sdl(void);
  
} GRAPH3D_SDL;

#endif // ENABLE_SDL_BACKEND

#endif //__GRAPH3D_H__
