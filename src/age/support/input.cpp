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
  Global input interface
*/
#include "age.h"

/*
// Event activators
#define  ACTIVATE_KEY_PRESSED   0x1
#define  ACTIVATE_KEY_RELEASED  0x2

// Event modificators
#define  MODIFICATOR_KEY_ALT    0x1
#define  MODIFICATOR_KEY_SHIFT  0x2
#define  MODIFICATOR_KEY_CTRL   0x4
*/
bool key_state::match(class key_state *p_event)
{      
  if(e_modificator == p_event->e_modificator && 
    (e_activator&p_event->e_activator)) {
    return(e_key == p_event->e_key);
  }
  return(FALSE);
}

/*
// Activators of mouse events
#define ACTIVATE_BUTTON_PRESSED       0x01
#define ACTIVATE_BUTTON_RELEASED      0x02
#define ACTIVATE_MOUSE_MOVE           0x04

// Event modificators
#define MOUSE_BUTTON_LEFT             0x01
#define MOUSE_BUTTON_MIDDLE           0x02
#define MOUSE_BUTTON_RIGHT            0x04
#define MOUSE_WHEEL_UP                0x08
#define MOUSE_WHEEL_DOWN              0x10
#define MOUSE_CURSOR_INSIDE           0x20
#define MOUSE_CURSOR_OUTSIDE          0x40

#define MOUSE_BUTTON_MASK             0x00ff
#define MOUSE_CURSOR_MASK             0xff00

*/
bool mouse_state::match(class mouse_state *p_event)
{
  // Is the event activated by mouse movement?
  if(e_activator&ACTIVATE_MOVEMENT) {
    if(!(p_event->e_activator&ACTIVATE_MOVEMENT))
      return(FALSE);
  
    if(e_modificator&MOUSE_CURSOR_INSIDE) {
      if(!in_rect(e_r,p_event->e_r))
        return(FALSE);
    } else if(e_modificator&MOUSE_CURSOR_OUTSIDE) {
      if(in_rect(e_r,p_event->e_r))
        return(FALSE);
    }
  
    // Check only the movement -> success
    if(e_activator&ACTIVATE_MOUSE_MOVE)
      return(TRUE);
  }
  
  // Is the event activated by mouse click?
  if(e_activator&ACTIVATE_BUTTON) {
    if((e_activator&ACTIVATE_BUTTON_PRESSED && !(p_event->e_activator&ACTIVATE_BUTTON_PRESSED)) ||
       (e_activator&ACTIVATE_BUTTON_RELEASED && !(p_event->e_activator&ACTIVATE_BUTTON_RELEASED)) ||
       (e_activator&ACTIVATE_MOUSE_MOVE_BUTTON && !(p_event->e_activator&ACTIVATE_MOUSE_MOVE_BUTTON)))
    {
      return(FALSE);
    }
    
    if((e_modificator&MOUSE_BUTTON_MASK) != ((p_event->e_modificator)&MOUSE_BUTTON_MASK))
    {
      return(FALSE);
    }
  }  
    
  return(TRUE);
}  

/*
// Activators
#define INPUT_MOUSE             0x0001
#define INPUT_KEY               0x0002

// Modificators
#define INPUT_FIRST_ONLY        0x0100  // Call only the first event of this type during single frame
#define INPUT_LAST_ONLY         0x0200  // Call only the last event of this type during single frame (default)
#define INPUT_RUNTIME           0x0300  // Fetch all events
*/
bool input_event::proceed(KEY_STATE *p_akey, 
                          MOUSE_STATE *p_amouse, 
                          EVENT_STREAM *p_output, 
                          bool end)
{
  bool active = FALSE;
    
  if(end) {
    if(flag&INPUT_LAST_ONLY && state_current) {
      state_current = FALSE;
      active = TRUE;
    }
  }
  else {  
    bool match_key = (flag&INPUT_KEY) ? key.match(p_akey) : TRUE;
    bool match_mouse = (flag&INPUT_MOUSE) ? mouse.match(p_amouse) : TRUE;    
    
    if(match_key && match_mouse) {
      if(flag&INPUT_FIRST_ONLY) {
        if(state_last) {
          active = FALSE;
        } else {
          active = state_last = TRUE;
        }
      }
      else {
        active = TRUE;
      }
    } else {
      active = state_last = FALSE;
    }
  
    if(flag&INPUT_LAST_ONLY && active) {
      state_current = TRUE;
      active = FALSE;
    }
  }
    
  if(active) {
    if(p_event && p_output) {
      p_output->add(p_event,event_num);
    }
    if(p_func) {        
      p_output->add(GAME_EVENT(EV_CALLBACK,(void *)p_func));
    }
    p_output->commit();
  }
    
  return(active);
}

bool input_event_list::proceed(KEY_STATE *p_akey,
                               MOUSE_STATE *p_amouse,
                               EVENT_STREAM *p_output,
                               bool end)
{
  int ret = 0;
  int i;
  for(i = 0; i < ievent_num; i++) {
    if(p_ievents[i].valid()) {
      ret |= p_ievents[i].proceed(p_akey,p_amouse,p_output,end);
    }
  }
  return(ret);
}

// -------------------------------------------------------------------------
// Keyboard interface
// -------------------------------------------------------------------------

void input::key_repeat_set(bool state)
{
  if(state) {
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,SDL_DEFAULT_REPEAT_INTERVAL);
  }
  else {
    SDL_EnableKeyRepeat(0,0);
  }
}

void input::key_input(KEYTYPE key, KEYMOD mod, bool pressed)
{  
  int activator = pressed ? ACTIVATE_KEY_PRESSED : ACTIVATE_KEY_RELEASED;
  
  int modificator = 0;
  
  if(mod&K_SHIFT_MASK)
    modificator |= MODIFICATOR_KEY_SHIFT;
  if(mod&K_CTRL_MASK)
    modificator |= MODIFICATOR_KEY_CTRL;

  akey.set(key, modificator, activator);
  
  proceed(FALSE);
}

// -------------------------------------------------------------------------
// Mouse interface
// -------------------------------------------------------------------------
void input::mouse_position_get(tpos *p_mx, tpos *p_my)
{
  *p_mx = amouse.e_r.x;
  *p_my = amouse.e_r.y;
}

void input::mouse_position_set(tpos mx, tpos my)
{
  amouse.e_r.x = mx;
  amouse.e_r.y = my;
}

void input::mouse_position_last_get(tpos *p_mx, tpos *p_my)
{
  *p_mx = amouse_last.e_r.x;
  *p_my = amouse_last.e_r.y;
}

void input::mouse_position_diff_get(tpos *p_dx, tpos *p_dy)
{
  *p_dx = amouse.e_r.x - amouse_last.e_r.x;
  *p_dy = amouse.e_r.y - amouse_last.e_r.y;
}

void input::mouse_button_set(int button, int state)
{
  amouse.button_set(button, state);
}

bool input::mouse_button_get(int button)
{  
  return(amouse.button_get(button));
}

void input::mouse_input(tpos mx, tpos my, int m_activator, int m_modificator)
{
  amouse.set(mx,my,m_activator,m_modificator);
  proceed(FALSE);
}

// -------------------------------------------------------------------------
// Process all events
// -------------------------------------------------------------------------

// Process all events in the system
// end -> is it the final call from the end of a frame?
void input::proceed(bool end)
{
  if(p_shadow_elist) {
    p_shadow_elist->proceed(&akey, &amouse, p_output, end);
  }
  else {
    int i;
    for(i = 0; i < elist_num; i++) {
      elist[i]->proceed(&akey, &amouse, p_output, end);
    }
  }
}

void input::start(void)
{
  amouse_last = amouse;
}

void input::end(void)
{
  proceed(TRUE);
}
