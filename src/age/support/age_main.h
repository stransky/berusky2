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
 * Loop event management
 */

#ifndef __AGE_MAIN_H__
#define __AGE_MAIN_H__

/*
  Main AGE class, it runs and owes whole graphics interface.
  
  AGE_CONFIG      config        - some configuration
  
Graphics interface:  
  
  GRAPH3D         *p_graph      - graphics interface, owed by AGE
  
  GRAPH3D_GTK * graph_gtk_set(tpos width, tpos height)
  GRAPH3D_SDL * graph_sdl_set(tpos width, tpos height, int screen_depth = BPP_DEFAULT, int fullscreen = FALSE)
  bool          graph_create_screen(void)
  void          graph_draw(void)
  GRAPH3D *     graph_get(void)
  
  
Scene interface:  
    
  SCENE           *p_scene      - active scene
  
  SCENE * scene_new(void)
  SCENE * scene_get(void)
  void    scene_delete(void)
  void    scene_active_set(SCENE *p_scene)
  SCENE * scene_active_get(void)
  
Age callback (called by every frame):
  
  AGE_EVENT_FUNC  p_callback    - AGE callback -> called every frame
  
  void    callback_set(AGE_EVENT_FUNC p_fnc)
  
Event stream:  
  
  EVENT_STREAM    estream       - events in system
  
  GAME_EVENT *  event_get(void)
  void          event_add(GAME_EVENT *p_event)
  
    Add event to system queue
  
  void          event_loop(void)
  
    Fetches all input events from SDL and transformate 
    them to the system events.

Timer interface:
  
  TIMER           ltimer        - timer interface

  TIMER *   timer_get(void)

Input interface:  
  
  INPUT           inp           - input interface

  INPUT * input_get(void)
  
Frame interface:

  int frame
  
  Frame configuration:
  
  int   current_frame_get(void)
  void  events_wait_set(bool state)
  
  void  fps_set(float fps)
  void  fps_clear(void)
  
  Frame/scene rendering:
  
  bool  draw(void)
  
    It does:
      - sets current frame
      - draw scene
      
  bool  run_frame(void)
  
    It does:
      - sets current frame
      - fetches all events from input and translate them to
        AGE event queue
      - calls age callback
      - waits until end of frame
  
  void  run(void)
  
    Run run_frame until the UI_QUIT appears in system queue.

*/

#define FPS_DEFAULT       30

#define LOOP_FPS_SET      0x1  // Keep given FPS
#define LOOP_EVENT_WAIT   0x2  // Wait for events (block)

class age_main;
typedef class age_main AGE_MAIN;

typedef bool (* AGE_EVENT_FUNC)(AGE_MAIN *p_loop);

/*
 * Graphics config
 */
typedef struct age_config {

  bool scene_selection;
    
} AGE_CONFIG;


typedef class age_main :
  public flag_interface,
  public class_user_data
{
  /* 
   * AGE configuration
   */
public:
  
  static AGE_CONFIG config;
  
private:

  GRAPH3D *p_graph;

public:
  
#ifdef ENABLE_GTK_BACKEND

  GRAPH3D_GTK * graph_gtk_set(tpos width, tpos height)
  {
    if(p_graph)
      delete p_graph;
    
    p_graph = new GRAPH3D_GTK(width, height);
    return((GRAPH3D_GTK *)p_graph);
  }
  
  GtkWidget * graph_gtk_window_get(void)
  {
    if(p_graph == NULL)
      return(NULL);
    
    assert(p_graph->type_get() == GRAPH_GTK);
    
    return(((GRAPH3D_GTK *)p_graph)->gtk_widget_get());
  }
  
#endif // ENABLE_GTK_BACKEND
  
#ifdef ENABLE_SDL_BACKEND  
  
  GRAPH3D_SDL * graph_sdl_set(tpos width, tpos height, int screen_depth = BPP_DEFAULT, int fullscreen = FALSE)
  {
    if(p_graph)
      delete p_graph;
    
    p_graph = new GRAPH3D_SDL(width, height, screen_depth, fullscreen);            
    return((GRAPH3D_SDL *)p_graph);
  }
  
#endif // ENABLE_SDL_BACKEND
  
  bool graph_screen_create(void)
  {
    assert(p_graph);
    return(p_graph->screen_create());
  }
  
  void graph_screen_resize(tpos width, tpos height)
  {
    assert(p_graph);
    return(p_graph->screen_resize(width, height));
  }
  
  GRAPH3D * graph_get(void)
  {
    return(p_graph);
  }

  // Draw whole scene
  void graph_draw(void)
  {
    assert(p_graph);
    if(p_graph) {
      p_graph->draw_start();
      p_graph->draw_end();
    }
  }
  
  /*
   * Scene interfaces
   */
public:
  
  SCENE * p_scene;

public:
  
  SCENE * scene_new(void)
  {
    scene_delete();
    p_scene = new SCENE;
    p_scene->graph_set(p_graph);
    return(p_scene);
  }
  
  SCENE * scene_get(void)
  {
    return(p_scene);
  } 
  
  void scene_delete(void)
  {
    if(p_scene) {
      delete p_scene;
      p_scene = NULL;
    }
  }
  
public:  
  
  void scene_active_set(SCENE *p_scene)
  {
    if(p_graph) {
      p_graph->scene_active_set(p_scene);
      p_scene->render_state_set(TRUE);
    }
  }
  
  SCENE * scene_active_get(void)
  {
    return(p_graph ? p_graph->scene_active_get() : NULL);
  }
  
  /*
   * Scene selection interface
   */
  /*
public:
  
  void scene_selection_add(OBJECT_SELECT * p_object)
  {
    SCENE *p_scene = scene_get();
    if(p_scene)
      p_scene->selection_add(p_object);
  }
  
  void scene_selection_remove(OBJECT_SELECT *p_object)
  {
    SCENE *p_scene = scene_get();
    if(p_scene)
      p_scene->selection_remove(p_object);
  }
  */
public:

  void scene_selection_clear(void)
  {
    SCENE *p_scene = scene_get();
    if(p_scene)
      p_scene->selection_clear();
  }
  
  SELECTED_OBJECT_ITEM * scene_selection_get(void)
  {
    SCENE *p_scene = scene_get();
    return(p_scene ? p_scene->selection_get() : NULL);
  }
  
public:

  void config_scene_selection(bool state);

  
  /*
   * Callback interface
   */
private:  
  
  AGE_EVENT_FUNC    p_callback;
  
public:
  
  void callback_set(AGE_EVENT_FUNC p_fnc)
  {
    p_callback = p_fnc;
  }

  /*
   * Main event stream - is cleared every frame
   */
private:  
  
  EVENT_STREAM  estream;

public:
  
  GAME_EVENT * event_get(void)
  {
    static GAME_EVENT ev;
        
    while(!estream.empty()) {
      ev = estream.get();
      
      if(ev.type_get() != EV_CALLBACK)
        return(&ev);
      
      INPUT_EVENT_FUNC p_func = (INPUT_EVENT_FUNC)ev.param_pointer_get(0);
      p_func(this);
    }
        
    return(NULL);
  }

  void event_add(GAME_EVENT *p_event)
  {
    estream.add(p_event);
  }
  
  void event_wait_set(bool state)
  {
    flag_set(LOOP_EVENT_WAIT,state);    
  }  

private:
  
  void event_loop(void);

  /*
   * Main timer class
   */
private:  
  
  TIMER ltimer;

public:
  
  TIMER * timer_get(void)
  {
    return(&ltimer);
  }

  // Input class - process input from system
private:  
  
  INPUT inp;

public:
  
  INPUT * input_get(void)
  {
    return(&inp);
  }
  
  /*
   * Frame interface
   */
private:

  int frame;
  
public:
  
  int  current_frame_get(void)
  {
    return(frame);
  }
    
public:

  void fps_set(float fps)
  {    
    ltimer.fps_set(fps);
    flag_set(LOOP_FPS_SET);
  }
  
  void fps_clear(void)
  {        
    flag_clear(LOOP_FPS_SET);
  }
  
  /*
   * Scene rendering
   */
public:
  // Draw single frame
  bool draw(void);

  // Run single frame (i.e.: process events/render/wait)
  bool frame_run(void);

  // Run all frames
  void run(void);
  
public:

  age_main(AGE_EVENT_FUNC p_callback = NULL);
  ~age_main(void);
  
} AGE_MAIN;

#endif // __AGE_MAIN__
