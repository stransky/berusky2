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
 * Loop management
 */

#include "age.h"

// -------------------------------------------------------
//   AGE config
// -------------------------------------------------------

AGE_CONFIG age_main::config;

void age_main::config_scene_selection(bool state)
{
  config.scene_selection = state;
}

void age_main::event_loop(void)
{
  // Queue where are stored events captured from KB/Mouse/...
  SDL_Event event;
  int ret;

  // Clear event stream
  estream.clear();
  
  // Clear input, save old state...
  inp.start();
  
  // Loop until there are no SDL events left on the queue
  if(flag_get(LOOP_EVENT_WAIT)) {
    ret = SDL_WaitEvent(&event);    
  }
  else {
    ret = SDL_PollEvent(&event);
  }
  
  while(ret) {
    switch (event.type) {
      case SDL_KEYDOWN:
        {
          KEYTYPE key = event.key.keysym.sym;
          KEYMOD mod = event.key.keysym.mod;
          
          int shift = mod&K_SHIFT_MASK;      
          int ctrl = mod&K_CTRL_MASK;
          
          estream.add(GAME_EVENT(UI_KEY_DOWN, (int)key, shift, ctrl));
          estream.commit();          
                    
          inp.key_input(key, mod, TRUE);
        }
        break;
      case SDL_KEYUP:
        {
          KEYTYPE key = event.key.keysym.sym;
          KEYMOD mod = event.key.keysym.mod;
      
          int shift = mod&K_SHIFT_MASK;      
          int ctrl = mod&K_CTRL_MASK;
  
          estream.add(GAME_EVENT(UI_KEY_UP, (int)key, shift, ctrl));
          estream.commit();
          
          inp.key_input(key, mod, FALSE);
        }
        break;
      case SDL_MOUSEMOTION:
        if(event.motion.state) {          
          inp.mouse_input(event.motion.x, event.motion.y, ACTIVATE_MOUSE_MOVE_BUTTON, event.motion.state);
        } else {          
          inp.mouse_input(event.motion.x, event.motion.y, ACTIVATE_MOUSE_MOVE, 0);
        }
        break;
      case SDL_MOUSEBUTTONDOWN:
        estream.add(GAME_EVENT(UI_MOUSE_BUTTON, event.button.x, event.button.y, 1, event.button.button));
        estream.commit();        
        inp.mouse_input(event.button.x, event.button.y, ACTIVATE_BUTTON_PRESSED, MOUSE_BUTTON(event.button.button));
        break;
      case SDL_MOUSEBUTTONUP:
        estream.add(GAME_EVENT(UI_MOUSE_BUTTON, event.button.x, event.button.y, 0, event.button.button));
        estream.commit();        
        inp.mouse_input(event.button.x, event.button.y, ACTIVATE_BUTTON_RELEASED, MOUSE_BUTTON(event.button.button));
        break;
      case SDL_ACTIVEEVENT:
        if(event.active.state&SDL_APPACTIVE) {
          if(event.active.gain) {
            estream.add(GAME_EVENT(UI_APP_ACTIVATED));
            estream.commit();
          } else {
            estream.add(GAME_EVENT(UI_APP_DEACTIVATED));
            estream.commit();
          }
        }
        break;
      case SDL_QUIT:        
        estream.add(GAME_EVENT(UI_QUIT));
        estream.commit();
        return;
      default:
        break;
    }
    
    ret = SDL_PollEvent(&event);
  }
  
  // Process all pendind events...
  inp.end();
}

bool age_main::frame_run(void)
{
  ltimer.frame_start();
  
  // Propagate correct frame to system
  if(p_graph) {
    p_graph->current_frame_set(frame);
    p_graph->current_fps_set(ltimer.fps_get());
  }
  
  // Process all input events
  event_loop();
  
  // Call user defined callback
  if(p_callback) {
    if(!p_callback(this))
      return(FALSE);
  }    
  
  // Wait until end of the frame
  ltimer.frame_end(flag_get(LOOP_FPS_SET));
  
  // Increment frame counter
  frame++;
  if(frame < 0)
    frame = 0;
  
  return(TRUE);
}

void age_main::run(void)
{
  while(frame_run())
    ;
}

bool age_main::draw(void)
{
  ltimer.frame_start();
  
  // Propagate correct frame to system
  if(p_graph) {
    p_graph->current_frame_set(frame);
    p_graph->current_fps_set(ltimer.fps_get());
  }
  
  // Draw frame
  graph_draw();
  
  // Increment frame counter
  frame++;
  if(frame < 0)
    frame = 0;
  
  return(TRUE);
}

age_main::age_main(AGE_EVENT_FUNC p_callback)
  : inp(&estream)
{
  p_graph = NULL;
  p_scene = NULL;
  frame = 0;
  
  srand(current_time_get());
  callback_set(p_callback);
  inp.key_repeat_set(FALSE);
}

age_main::~age_main(void)
{
  if(p_graph)
    delete p_graph;
}
