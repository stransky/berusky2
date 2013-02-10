/*
 *        .þÛÛþ þ    þ þÛÛþ.     þ    þ þÛÛÛþ.  þÛÛÛþ .þÛÛþ. þ    þ
 *       .þ   Û Ûþ.  Û Û   þ.    Û    Û Û    þ  Û.    Û.   Û Ûþ.  Û
 *       Û    Û Û Û  Û Û    Û    Û   þ. Û.   Û  Û     Û    Û Û Û  Û
 *     .þþÛÛÛÛþ Û  Û Û þÛÛÛÛþþ.  þþÛÛ.  þþÛÛþ.  þÛ    Û    Û Û  Û Û
 *    .Û      Û Û  .þÛ Û      Û. Û   Û  Û    Û  Û.    þ.   Û Û  .þÛ
 *    þ.      þ þ    þ þ      .þ þ   .þ þ    .þ þÛÛÛþ .þÛÛþ. þ    þ
 **
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
  3D Graphics library
*/

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "age.h"

// -------------------------------------------------------
//   Graphics config
// -------------------------------------------------------

GRF_CONFIG graph3d::config;

// -------------------------------------------------------
//   Error handling
// -------------------------------------------------------
void gl_error(void)
{
  dword error = glGetError();
  switch(error) {
     case 0:
       break;
     
    case GL_INVALID_ENUM:
      PERROR(true, "GL_INVALID_ENUM");
      break;
    
    case GL_INVALID_VALUE:
      PERROR(true, "GL_INVALID_VALUE");      
      break;
    
    case GL_INVALID_OPERATION:
      PERROR(true, "GL_INVALID_OPERATION");      
      break;
    
    case GL_STACK_OVERFLOW:
      PERROR(true, "GL_STACK_OVERFLOW");
      break;
    
    case GL_STACK_UNDERFLOW:
      PERROR(true, "GL_STACK_UNDERFLOW");
      break;
    
    case GL_OUT_OF_MEMORY:
      PERROR(true, "GL_OUT_OF_MEMORY");
      break;
    
    default:
      PERROR(true, "Error %d",error);
      break;
  }
}

void sdl_start(void)
{
  pprintf("SDL Init...");
  if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER)) {
    pprintf("SDL initialization failed: %s\n", SDL_GetError());
  } 

  // Some default SDL config
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,SDL_DEFAULT_REPEAT_INTERVAL);

  // Disable for now
  SDL_EnableUNICODE(FALSE);
}

// -------------------------------------------------------
//  Graphics configuration
// -------------------------------------------------------
void graph3d::config_draw_grid(bool state)
{
  config.draw_grid = state;
}

void graph3d::config_draw_boxes(bool state)
{
  config.draw_bouding_boxes = state;
}

void graph3d::config_draw_console(bool state)
{
  config.draw_console = state;
}

void graph3d::config_draw_mouse_cursor(bool state)
{
  config.draw_mouse_cursor = state;
  SDL_ShowCursor(!state);
}

void graph3d::config_draw_pivots(bool state)
{
  config.draw_pivots = state;
}

void graph3d::config_draw_fps(bool state)
{
  config.draw_debug_fps = state;
}

void graph3d::config_draw_wired(bool state)
{
  config.draw_debug_wired = state;
  config.draw_debug_points = FALSE;
  if(state) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  else {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
}

void graph3d::config_draw_points(bool state)
{
  config.draw_debug_points = state;
  config.draw_debug_wired = FALSE;
  if(state) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
  }
  else {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
}

void graph3d::config_draw_normals(bool state)
{
  config.draw_debug_normals = state;
}

void graph3d::config_draw_materials(bool state)
{
  config.draw_debug_materials = state;
}

void graph3d::config_opengl_lighting(bool state)
{
  config.opengl_lighting = state;
}

void graph3d::config_draw_all_objects(bool state)
{
  config.draw_debug_all_objects = state;  
}

void graph3d::config_draw_selection(bool state)
{
  config.draw_selection = state;  
}

/* 
 * Sets a simple render-mode
 */
void graph3d::config_render_mode_set(RENDER_MODE mode)
{
  switch(mode) {
    case RENDER_POINTS:
      config_draw_points(TRUE);
      config_draw_materials(FALSE);
      break;
    case RENDER_WIRED:
      config_draw_wired(TRUE);
      config_draw_materials(FALSE);
      break;
    case RENDER_SOLID:
      config_draw_wired(FALSE);
      config_draw_materials(FALSE);
      break;
    case RENDER_TEXTURED:
      config_draw_wired(FALSE);
      config_draw_materials(TRUE);
      break;      
  }
}

/* 
 * Selection rectangle
 */
void graph3d::selection_rectangle_draw(void)
{
  if(selection_rectangle_active) {
    mesh_material::set_default();
    draw_rect_line_2d(selection_rectangle_start.x, selection_rectangle_start.y,
                      selection_rectangle_stop.x, selection_rectangle_stop.y,
                      selection_rectangle_color);
  }
}

void graph3d::selection_rectangle_clear(void)
{
  selection_rectangle_active = FALSE;
}

void graph3d::selection_rectangle_set_start(void)
{
  selection_rectangle_active = TRUE;
  SDL_GetMouseState(&selection_rectangle_start.x,&selection_rectangle_start.y);
  selection_rectangle_stop = selection_rectangle_start;
}

void graph3d::selection_rectangle_set(void)
{
  SDL_GetMouseState(&selection_rectangle_stop.x,&selection_rectangle_stop.y);
}

bool graph3d::selection_rectangle_get(RECT2DI *p_r)
{
  if(selection_rectangle_active) {
    VECT2DI min,max;
    
    MIN_MAX_SORT(min.x,max.x,selection_rectangle_start.x,selection_rectangle_stop.x);
    MIN_MAX_SORT(min.y,max.y,selection_rectangle_start.y,selection_rectangle_stop.y);
    
    p_r->set(&min, &max);
    
    return(TRUE);
  }
  return(FALSE);
}

/*
 * Drawing start
 */
void graph3d::clear(void)
{
  if(graphics) {    
    
    /*
     * Clear all buffers
     */
    glClear(GL_DEPTH_BUFFER_BIT);
    if(screen_clear) {
      glClear(GL_COLOR_BUFFER_BIT);
    }
    
    /*
     * Set a default material
     */
    mesh_material::set_default(TRUE);
  }
}

void graph3d::grid_draw(void)
{  
  mesh_material::set_default();
  
  VECT vn(0.0f,0.0f,-1.0f);
  VECT vf(0.0f,0.0f, 1.0f);
  
  GPIPE *p_gpipe = gpipe_get();
  p_gpipe->screen_normal_to_world(&vn);
  p_gpipe->screen_normal_to_world(&vf);
  
  // v1->v2 
  VECT dir;
  dir  = vf-vn;
  dir *= 0.5f;  
  vn  += dir;
  vn.y = 0.0f;
  
  VECT vl(-1.0f,0.0f,1.0f);
  VECT vr( 1.0f,0.0f,1.0f);
  
  p_gpipe->screen_normal_to_world(&vl);
  p_gpipe->screen_normal_to_world(&vr);
  
  VECT width;
  width = vr-vl;
  draw_floor_line(&vn,width.size(),2,40);
}

void graph3d::mouse_cursor_draw(void)
{
  GPIPE *p_gpipe = gpipe_get();
  
  int sx, sy, max_x, max_y;  
  p_gpipe->view_get(&sx, &sy, &max_x, &max_y);
  
  int mx, my;
  SDL_GetMouseState(&mx,&my);
  
  mesh_material::set_default();
  
  switch(mouse_cursor_type) {
    case MOUSE_CURSOR_NORMAL:
      draw_cursor_normal(mx-sx, my-sy, 0, 0, max_x, max_y);
      break;
    case MOUSE_CURSOR_RECTANGLE_SELECT:
      draw_cursor_rect_select(mx-sx, my-sx, RGBB(255,0,0));
      break;
    case MOUSE_CURSOR_RECTANGLE_SELECTING:
      draw_cursor_normal(mx-sx, my-sy, 0, 0, max_x, max_y, RGBB(255,0,0));
      break;
    case MOUSE_CURSOR_SCENE_MOVE:
      draw_cursor_rect_select(mx-sx, my-sx, RGBB(0,255,0));
      break;
    case MOUSE_CURSOR_SCENE_MOVING:
      //draw_cursor_normal(mx-sx, my-sy, 0, 0, max_x, max_y, RGBB(0,255,0));
      draw_cursor_rect_select(mx-sx, my-sx, RGBB(0,255,0));
      break;
    case MOUSE_CURSOR_SCENE_ROTATE:
      draw_cursor_rect_select(mx-sx, my-sx, RGBB(255,255,0));
      break;
    case MOUSE_CURSOR_SCENE_ROTATING:
      //draw_cursor_normal(mx-sx, my-sy, 0, 0, max_x, max_y, RGBB(255,255,0));
      draw_cursor_rect_select(mx-sx, my-sx, RGBB(255,255,0));
      break;
    default:
      draw_cursor_normal(mx-sx, my-sy, 0, 0, max_x, max_y);
      break;
  }
}

void graph3d::fps_draw(void)
{
  mesh_material::set_default();
  console_print(0,0,"fps %f",current_fps);
}

// Start scene drawing
void graph3d::draw_start(void)
{
  if(p_scene_active) {
    p_scene_active->draw_start(current_frame);
  }
}

// Draw whole scene
void graph3d::draw_end(void)
{
  if(graphics) {
    clear();

    if(config.opengl_lighting) {
      gl_light::on();
    }
    
    if(p_scene_active) {
      p_scene_active->draw_end(config.draw_debug_all_objects);
    }
    
    if(config.opengl_lighting) {
      gl_light::off();
    }

    // Reset world matrix
    GPIPE *p_pipe = gpipe_get();
    if(p_pipe)
      p_pipe->world_init();
    
    if(config.draw_grid)
      grid_draw();
        
    if(p_pipe)
      p_pipe->matrix_2d_mode_set();

    if(config.draw_debug_fps)
      fps_draw();
    
    if(config.draw_console)
      console_draw();
    
    if(config.draw_mouse_cursor)
      mouse_cursor_draw();
  
    if(config.draw_selection)
      selection_rectangle_draw();
    
    if(p_pipe)
      p_pipe->matrix_3d_mode_set();
    
    flip();
  }
}

void graph3d::set(void)
{
  graphics = FALSE;
  graphics_width = 0;
  graphics_height = 0;
  graphics_bpp = 0;
  graphics_bpp_zbuffer = 0;
  graphics_opengl = FALSE;
  graphics_fullscreen = 0;
  color_zbuffer = 1.0f;
  
  // GL/3D settings
  //p_scene = NULL;
  screen_clear = TRUE;
  screen_clear_color.set(0.4f);
  
  current_frame = -1;
  
  selection_rectangle_active = FALSE;
  selection_rectangle_color.set(255,0,0);

  p_screen_resize_callback = NULL;
}

void graph3d::set(tpos width, tpos height, int screen_depth, bool full_screen)
{
  // reset all values
  set();
  
  // Set the default values for graphics  
  graphics_width = width;
  graphics_height = height;
  graphics_bpp = screen_depth;
  graphics_fullscreen = full_screen;
}

void graph3d::get(tpos *p_width, tpos *p_height, int *p_screen_depth)
{
  if(p_width)
    *p_width = graphics_width;
  if(p_height)
    *p_height = graphics_height;
  if(p_screen_depth)
    *p_screen_depth = graphics_bpp;
}

void graph3d::resize_callback_set(SCREEN_RESIZE_CALLBACK p_callback)
{
  p_screen_resize_callback = p_callback;
  if(p_callback)
    p_callback(graphics_width, graphics_height);
}

bool graph3d::create_GL(void)
{
  // Are we already created?
  assert(graphics == FALSE);
  
  // Create new screen
  pprintf("Screen resolution %dx%d\nColor depth %d bits\nFullscreen = %d\nOpenGL = %d\n",
          graphics_width, 
          graphics_height, 
          graphics_bpp, 
          graphics_fullscreen, 
          graphics_opengl);
  
  pprintf("Creating screen...");
  screen_clear_color_set(screen_clear_color);
  
  pprintf("Loading extensions...");      
  glext.init_extension();
  gl_texture::init(gl_ext::ext_multitext_units);
  
  pprintf("Creating fonts...");
  console_font_create();  
  
  pprintf("Done.");  
  graphics = TRUE;
  gl_active = TRUE;
  return(graphics);
}

void graph3d::destroy_GL(void)
{
  if(graphics) {
    console_font_destroy();
  }
  gl_active = FALSE;
}

void graph3d::destroy(void)
{
  destroy_GL();
  screen_destroy();  
  SDL_Quit();
}

graph3d::graph3d(GRAPH_TYPE type)
  : graph_type(type)
{   
  set();
  sdl_start();
}

graph3d::graph3d(GRAPH_TYPE type, tpos width, tpos height, int screen_depth, bool full_screen)
  : graph_type(type)
{  
  set(width, height, screen_depth, full_screen);  
  sdl_start();
}

graph3d::~graph3d(void)
{
}

#ifdef ENABLE_GTK_BACKEND

bool graph3d_gtk::screen_create(void)
{ 
  /* Attribute list for gtkglarea widget. Specifies a
     list of Boolean attributes and enum/integer
     attribute/value pairs. The last attribute must be
     GDK_GL_NONE. See glXChooseVisual manpage for further
     explanation.
  */
  int attrlist[] = {
    GDK_GL_RGBA,
    GDK_GL_RED_SIZE,1,
    GDK_GL_GREEN_SIZE,1,
    GDK_GL_BLUE_SIZE,1,
    GDK_GL_DOUBLEBUFFER,
    GDK_GL_NONE
  };

  /* Check if OpenGL is supported. */
  if(gdk_gl_query() == FALSE) {
    PERROR(FALSE, "OpenGL is not supported!");
    return(FALSE);
  }
  
  assert(p_area == NULL);
  
  /* Create new OpenGL widget. */
  p_area = GTK_WIDGET(gtk_gl_area_new(attrlist));
  if(!p_area) {
    PERROR(true, "gtk_gl_area_new failed!");
  }  
  gtk_widget_set_size_request(GTK_WIDGET(p_area), graphics_width, graphics_height);
    
  screen_resize(graphics_width, graphics_height);
  return(TRUE);
}

void graph3d_gtk::screen_destroy(void)
{
  
}

void graph3d_gtk::screen_resize(tpos width, tpos height)
{
  graphics_width = width;
  graphics_height = height;
  
  camera_interface::screen_size_set(width, height);

  if(p_screen_resize_callback)
    p_screen_resize_callback(width, height);
}

// flip GL buffers
void graph3d_gtk::flip(void)
{ 
  /* Swap backbuffer to front */
  if(graphics && p_area && gtk_gl_rendering(p_area)) {
    gtk_gl_area_swapbuffers(GTK_GL_AREA(p_area));
  }
}

graph3d_gtk::graph3d_gtk(void)
  : graph3d(GRAPH_GTK)
{
  p_area = NULL;
}

graph3d_gtk::graph3d_gtk(tpos width, tpos height)
  : graph3d(GRAPH_GTK, width, height)
{
  p_area = NULL;
}

graph3d_gtk::~graph3d_gtk(void)
{  
  destroy();
}

#endif // ENABLE_GTK_BACKEND

#ifdef ENABLE_SDL_BACKEND

// Obtain the screen from SDL
// If we have any screen/surface, release them first
bool graph3d_sdl::screen_regenerate(void)
{
  screen_destroy();

  pprintf("Init video surface...\n");
  SDL_Surface *p_hwscreen = SDL_SetVideoMode(graphics_width, graphics_height,
                                             graphics_bpp, sdl_video_flags);
  
  if(!p_hwscreen) {
    fprintf(stderr, "Unable to set the video mode: %s", SDL_GetError());
    exit(-1);
  }
   
  p_screen_surface = new SURFACE_SDL(p_hwscreen);
  return(TRUE);
}

bool graph3d_sdl::screen_create(void)
{ 
  assert(graphics == FALSE);
  
  // Fetch the video info...
  const SDL_VideoInfo *videoInfo = SDL_GetVideoInfo();
  if(!videoInfo) {
    pprintf("Video query failed: %s\n",SDL_GetError());
    return(FALSE);
  }
    
  /* the flags to pass to SDL_SetVideoMode */
  sdl_video_flags = SDL_OPENGL;       /* Enable OpenGL in SDL */
  sdl_video_flags |= SDL_GL_DOUBLEBUFFER; /* Enable double buffering */
  sdl_video_flags |= SDL_HWPALETTE;       /* Store the palette in hardware */
  sdl_video_flags |= SDL_RESIZABLE;       /* Enable window resizing */

  /* This checks to see if surfaces can be stored in memory */
  if(videoInfo->hw_available)
    sdl_video_flags |= SDL_HWSURFACE;
  else
    sdl_video_flags |= SDL_SWSURFACE;

  /* This checks if hardware blits can be done */
  if(videoInfo->blit_hw)
    sdl_video_flags |= SDL_HWACCEL;

  if(graphics_fullscreen)
    sdl_video_flags |= SDL_FULLSCREEN;
  
  /* Sets up OpenGL double buffering */
  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
  
  /* Get resolution from screen */
  if(!graphics_width || !graphics_height) {  
    graphics_width = videoInfo->current_w;
    graphics_height = videoInfo->current_h; 
  }
  if(!graphics_bpp) {
    graphics_bpp = videoInfo->vfmt->BitsPerPixel;
  }  

  screen_resize(graphics_width, graphics_height);

#ifdef ENABLE_GL
  create_GL();
#endif

  return(TRUE);
}

void graph3d_sdl::screen_destroy(void)
{
  if(p_screen_surface)
    delete p_screen_surface;
}

void graph3d_sdl::screen_resize(tpos width, tpos height)
{
  graphics_width = width;
  graphics_height = height;
  
  camera_interface::screen_size_set(width, height);

  screen_regenerate();

  if(p_screen_resize_callback)
    p_screen_resize_callback(width, height);
}

void graph3d_sdl::fullscreen_toggle(void)
{
  if(!SDL_WM_ToggleFullScreen(((SURFACE_SDL*)p_screen_surface)->surf_get())) {
    fprintf(stderr, "SDL_WM_ToggleFullScreen() failed!");
  }
  else {
    graphics_fullscreen = !graphics_fullscreen;
  }
}

// flip GL buffers
void graph3d_sdl::flip(void)
{ 
  if(graphics) {
    SDL_GL_SwapBuffers();
  }
}

graph3d_sdl::graph3d_sdl(void)
  : graph3d(GRAPH_SDL)
{
}

graph3d_sdl::graph3d_sdl(tpos width, tpos height, int screen_depth, bool full_screen)
  : graph3d(GRAPH_SDL, width, height, screen_depth, full_screen), 
    p_screen_surface(NULL),
    sdl_video_flags(0)
{
}

graph3d_sdl::~graph3d_sdl(void)
{
  destroy();
}

#endif // ENABLE_SDL_BACKEND
