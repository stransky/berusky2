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

/* Interface for keyboard/mouse/everything
*/

#ifndef __INPUT_H__
#define __INPUT_H__

/* Key definition - SDL */

#define  KEYNUM       0xff

#define  K_ESC        SDLK_ESCAPE

#define  K_F1         SDLK_F1
#define  K_F2         SDLK_F2
#define  K_F3         SDLK_F3
#define  K_F4         SDLK_F4
#define  K_F5         SDLK_F5
#define  K_F6         SDLK_F6
#define  K_F7         SDLK_F7
#define  K_F8         SDLK_F8
#define  K_F9         SDLK_F9
#define  K_F10        SDLK_F10
#define  K_F11        SDLK_F11
#define  K_F12        SDLK_F12

#define  K_1          SDLK_1
#define  K_2          SDLK_2
#define  K_3          SDLK_3
#define  K_4          SDLK_4
#define  K_5          SDLK_5
#define  K_6          SDLK_6
#define  K_7          SDLK_7
#define  K_8          SDLK_8
#define  K_9          SDLK_9
#define  K_0          SDLK_0

#define  KP_0         SDLK_KP0
#define  KP_1         SDLK_KP1
#define  KP_2         SDLK_KP2
#define  KP_3         SDLK_KP3
#define  KP_4         SDLK_KP4
#define  KP_5         SDLK_KP5
#define  KP_6         SDLK_KP6
#define  KP_7         SDLK_KP7
#define  KP_8         SDLK_KP8
#define  KP_9         SDLK_KP9

#define  K_A          SDLK_a
#define  K_B          SDLK_b
#define  K_C          SDLK_c
#define  K_D          SDLK_d
#define  K_E          SDLK_e
#define  K_F          SDLK_f
#define  K_G          SDLK_g
#define  K_H          SDLK_h
#define  K_I          SDLK_i
#define  K_J          SDLK_j
#define  K_K          SDLK_k
#define  K_L          SDLK_l
#define  K_M          SDLK_m
#define  K_N          SDLK_n
#define  K_O          SDLK_o
#define  K_P          SDLK_p
#define  K_Q          SDLK_q
#define  K_R          SDLK_r
#define  K_S          SDLK_s
#define  K_T          SDLK_t
#define  K_U          SDLK_u
#define  K_V          SDLK_v
#define  K_W          SDLK_w
#define  K_X          SDLK_x
#define  K_Y          SDLK_y
#define  K_Z          SDLK_z

#define  K_MINUS      SDLK_MINUS
#define  K_PLUS       SDLK_EQUALS //SDLK_PLUS
#define  K_BKSP       SDLK_BACKSPACE
#define  K_TAB        SDLK_TAB

#define  K_BRACKET_L  SDLK_LEFTBRACKET
#define  K_BRACKET_R  SDLK_RIGHTBRACKET
#define  K_ENTER      SDLK_RETURN

#define  K_SEMICOL    SDLK_SEMICOLON
#define  K_QUOTE      SDLK_QUOTE
#define  K_TILDA     
#define  K_BACKSLASH  SDLK_BACKSLASH
#define  K_COMMA      SDLK_COMMA
#define  K_PERIOD     SDLK_PERIOD
#define  K_SLASH      SDLK_SLASH
#define  K_SPACE      SDLK_SPACE

#define  K_UP         SDLK_UP
#define  K_LEFT       SDLK_LEFT
#define  K_RIGHT      SDLK_RIGHT
#define  K_DOWN       SDLK_DOWN

#define  K_HOME       SDLK_HOME
#define  K_PGUP       SDLK_PAGEUP
#define  K_END        SDLK_END
#define  K_PGDN       SDLK_PAGEDOWN
#define  K_INSERT     SDLK_INSERT
#define  K_DEL        SDLK_DELETE

#define  K_SHIFT_MASK KMOD_SHIFT
#define  K_CTRL_MASK  KMOD_CTRL

typedef  SDLKey       KEYTYPE;
typedef  SDLMod       KEYMOD;

inline bool in_rect(RECT2DI &dst, tpos x, tpos y)
{
  return(x >= dst.x && y >= dst.y && x < dst.x+dst.dx && y < dst.y+dst.dy);
}

inline bool in_rect(RECT2DI &dst, RECT2DI &src)
{
  return(src.x >= dst.x && src.y >= dst.y &&
         src.x < dst.x+dst.dx && src.y < dst.y+dst.dy);
}

#define NO_STATE  (-1)

// ------------------------------------------------------------
// Event prototypes
// ------------------------------------------------------------

class age_main;
typedef class age_main AGE_MAIN;

// Input event handler
typedef bool (* INPUT_EVENT_FUNC)(AGE_MAIN *p_age);

// -------------------------------------------------------
// Keyboard UI
// -------------------------------------------------------

// Event activators
#define  ACTIVATE_KEY_PRESSED   0x1
#define  ACTIVATE_KEY_RELEASED  0x2

// Event modificators
#define  MODIFICATOR_KEY_ALT    0x1
#define  MODIFICATOR_KEY_SHIFT  0x2
#define  MODIFICATOR_KEY_CTRL   0x4

typedef class key_state {

public:    
  
  KEYTYPE     e_key;              // active key
  int         e_activator;        // activators
  int         e_modificator;      // modificators

public:

  void set(KEYTYPE key, int modificator = 0, int activator = ACTIVATE_KEY_PRESSED)
  {
    e_key = key;
    e_activator = activator;
    e_modificator = modificator;
  }

public:  
  
  bool match(class key_state *p_event);
    
public:

  key_state(KEYTYPE key = (KEYTYPE)0,
            int modificator = 0,
            int activator = ACTIVATE_KEY_PRESSED)
  { 
    set(key,modificator,activator);
  }

} KEY_STATE;

// -------------------------------------------------------
// Mouse UI
// -------------------------------------------------------
// Activators of mouse events
#define ACTIVATE_BUTTON_PRESSED       0x0001
#define ACTIVATE_BUTTON_RELEASED      0x0002
#define ACTIVATE_MOUSE_MOVE           0x0004
#define ACTIVATE_MOUSE_MOVE_BUTTON    0x0010

#define ACTIVATE_MOVEMENT             (ACTIVATE_MOUSE_MOVE|ACTIVATE_MOUSE_MOVE_BUTTON)
#define ACTIVATE_BUTTON               (ACTIVATE_BUTTON_PRESSED|ACTIVATE_BUTTON_RELEASED|ACTIVATE_MOUSE_MOVE_BUTTON)

// Event modificators
#define MOUSE_BUTTON_LEFT             0x0001
#define MOUSE_BUTTON_MIDDLE           0x0002
#define MOUSE_BUTTON_RIGHT            0x0004
#define MOUSE_WHEEL_UP                0x0008
#define MOUSE_WHEEL_DOWN              0x0010
#define MOUSE_CURSOR_INSIDE           0x2000
#define MOUSE_CURSOR_OUTSIDE          0x4000

#define MOUSE_BUTTON_MASK             0x00ff
#define MOUSE_CURSOR_MASK             0xff00

#define MOUSE_BUTTONS                 5
#define NO_BUTTON                     0

#define MOUSE_BUTTON(button)          (1<<(button-1))

typedef class mouse_state {

public:

  RECT2DI e_r;             // Mouse coordinators
  int     e_activator;     // Activators
  int     e_modificator;   // Modificators

public:

  void set(tpos x, tpos y, int activator = 0, int modificator = 0)
  {
    e_r.x = x;
    e_r.y = y;
    e_activator = activator;
    e_modificator = modificator;
  }
    
  void position_set(tpos x, tpos y)
  {
    e_r.x = x;
    e_r.y = y;
  }
  void position_get(tpos *p_x, tpos *p_y)
  {
    *p_x = e_r.x;
    *p_y = e_r.y;
  }
  
  void button_set(int button, int state)
  {
    if(state) {
      e_activator |= MOUSE_BUTTON(button);
    }
    else {
      e_activator &= ~(MOUSE_BUTTON(button));
    }
  }  
  bool button_get(int button)
  {  
    return(e_activator&MOUSE_BUTTON(button));
  }
  
public:
  
  bool match(class mouse_state *p_current);
  
public:
  
  mouse_state(void)
  {
    set(0,0,0,0);
  }

  mouse_state(int activator, int modificator = 0)
  {
    set(0,0,activator,modificator);
  }
    
  mouse_state(tpos x, tpos y, int activator, int modificator = 0)
  {
    set(x,y,activator,modificator);
  }
  
} MOUSE_STATE;

/*
 *  Activators
 */
#define INPUT_MOUSE             0x0001
#define INPUT_KEY               0x0002
#define INPUT_ALWAYS            0x0004

/*
 * Modificators
 */
#define INPUT_FIRST_ONLY        0x0100  // Call only the first event of this type during single frame
#define INPUT_LAST_ONLY         0x0200  // Call only the last event of this type during single frame (default)
#define INPUT_RUNTIME           0x0400  // Fetch all events

typedef class input_event {

public:
  
  int                 flag;
  
public:

  // Conditions what can trigger this event
  KEY_STATE           key;
  MOUSE_STATE         mouse;
  
  // Events what are triggered
  GAME_EVENT         *p_event;
  int                 event_num;
  
  // Event functions what is triggered
  INPUT_EVENT_FUNC    p_func;

  // Last and current state of this event
  bool                state_last;
  bool                state_current;

public:
  
  bool valid(void)
  {
    return(p_event != NULL || p_func != NULL);
  }

public:
    
  void create(void)
  {
    flag = INPUT_LAST_ONLY;
    event_num = 0;
    p_event = NULL;
    p_func = NULL;
    state_last = FALSE;
    state_current = FALSE;
  }
  
  void flag_set(int flags_input)
  {
    if(flags_input&INPUT_ALWAYS) {
      flag &= ~(INPUT_MOUSE|INPUT_KEY);
    }
    if(flags_input&INPUT_RUNTIME) {
      flag &= ~(INPUT_FIRST_ONLY|INPUT_LAST_ONLY);
    }
    if(flags_input&INPUT_FIRST_ONLY) {
      flag &= ~(INPUT_RUNTIME|INPUT_LAST_ONLY);
    }
    if(flags_input&INPUT_LAST_ONLY) {
      flag &= ~(INPUT_RUNTIME|INPUT_FIRST_ONLY);
    }
    flag |= flags_input;
  }
  
public:  

  // Basic - none initializer
  input_event(void)
  {
    create();
  }
  
  // Allways lauched events
  input_event(GAME_EVENT *p_events, int events_num = 0, int flags = 0)
  {
    create();
    flag_set(flags|INPUT_ALWAYS);
    
    event_num = events_num;
    p_event = p_events;
  }  
  input_event(INPUT_EVENT_FUNC p_event_func = NULL, int flags = 0)
  {
    create();
    flag_set(flags|INPUT_ALWAYS);
    
    p_func = p_event_func;
  }      
  
  // Key driven events
  input_event(KEY_STATE kstate, GAME_EVENT *p_events = NULL, int events_num = 0, int flags = 0)
  {
    create();
    flag_set(flags|INPUT_KEY);
    
    key = kstate;    
    event_num = events_num;
    p_event = p_events;
  }
  
  input_event(KEY_STATE kstate, INPUT_EVENT_FUNC p_event_func = NULL, int flags = 0)
  {
    create();
    flag_set(flags|INPUT_KEY);
    
    key = kstate;
    p_func = p_event_func;
  }      
  
  // Mouse driven events
  input_event(MOUSE_STATE mstate, GAME_EVENT *p_events = NULL, int events_num = 0, int flags = 0)
  {    
    create();
    flag_set(flags|INPUT_MOUSE);
    
    mouse = mstate;
    event_num = events_num;
    p_event = p_events;
  }

  input_event(MOUSE_STATE mstate, INPUT_EVENT_FUNC p_event_func = NULL, int flags = 0)
  {    
    create();
    flag_set(flags|INPUT_MOUSE);
    
    mouse = mstate;
    p_func = p_event_func;
  }
  
  // All events
  input_event(KEY_STATE kstate, MOUSE_STATE mstate, GAME_EVENT *p_events = NULL, int events_num = 0, int flags = 0)
  {
    create();
    flag_set(flags|INPUT_MOUSE|INPUT_KEY);
    
    key = kstate;
    mouse = mstate;
    event_num = events_num;
    p_event = p_events;
  }  
  
  input_event(KEY_STATE kstate, MOUSE_STATE mstate, INPUT_EVENT_FUNC p_event_func = NULL, int flags = 0)
  {
    create();
    flag_set(flags|INPUT_MOUSE|INPUT_KEY);
    
    key = kstate;
    mouse = mstate;
    p_func = p_event_func;
  }  

  // Basic - none initializer
public:
  
  void state_clear(void)
  {
    state_last = FALSE;
  }

  bool state_get(void)
  {
    return(state_last);
  }
  
public:

  bool proceed(KEY_STATE *p_akey, MOUSE_STATE *p_amouse, EVENT_STREAM *p_output, bool end = FALSE);
  
} INPUT_EVENT;

typedef class input_event_list {
  
  INPUT_EVENT   *p_ievents;
  int            ievent_num;
  
public:

  input_event_list(void)
  {
    p_ievents = NULL;
    ievent_num = 0;
  }

  input_event_list(INPUT_EVENT *p_ev, int e_num)
  {
    p_ievents = p_ev;
    ievent_num = e_num;
  }

public:

  bool proceed(KEY_STATE *p_akey, MOUSE_STATE *p_amouse, EVENT_STREAM *p_output, bool end = FALSE);
  
} INPUT_EVENT_LIST;

// ------------------------------------------------------------
// Basic game input class
// ------------------------------------------------------------
#define INPUT_BLOCK_SETS        0x1       // Block all key-sets (is captured by console)

typedef class input {

  /* 
   * Event output interface
   */  
  EVENT_STREAM *p_output;

public:
  
  EVENT_STREAM * output_get(void)
  {
    return(p_output);
  }
  
  void output_set(EVENT_STREAM *p_out)
  {
    p_output = p_out;
  }
  
  /* 
   * Event interface
   */
  #define ELIST_NUM_MAX   20

private:
  
  int                 elist_num;
  INPUT_EVENT_LIST   *elist[ELIST_NUM_MAX];
  
private:

  int events_list_add(INPUT_EVENT_LIST *p_elist)
  {
    if(elist_num+1 < ELIST_NUM_MAX) {
      elist[elist_num] = p_elist;
      return(elist_num++);
    }
    else {
      return(ERROR);
    }
  }
  
public:

  void events_clear(void)
  {
    int i;
    
    for(i = 0; i < elist_num; i++) {
      if(elist[i])
        delete elist[i];
    }
    
    elist_num = 0;
  }
  
  int events_add(INPUT_EVENT *p_events, int ievent_num)
  {
    INPUT_EVENT_LIST *p_elist = new INPUT_EVENT_LIST(p_events, ievent_num);
    return(events_list_add(p_elist));
  }
  
  bool events_remove(int handle)
  {
    if(handle < elist_num && elist[handle]) {    
      delete elist[handle];
      return(TRUE);
    }
    else {
      return(FALSE);
    }
  }
  
  /* 
   * Shadow events -> shadows all regular events
   */
private:
    
  INPUT_EVENT_LIST   *p_shadow_elist;
    
public:

  void shadow_events_clear(void)
  {
    if(p_shadow_elist) {
      delete p_shadow_elist;
      p_shadow_elist = NULL;
    }
  }
  
  void shadow_events_set(INPUT_EVENT *p_events, int ievent_num)
  {    
    shadow_events_clear();
    p_shadow_elist = new INPUT_EVENT_LIST(p_events, ievent_num);
  }
  
  /* 
   * Key interface
   */
private:  
  
  KEY_STATE akey;       // Current key state

public:

  void key_repeat_set(bool state);

public:

  void key_input(KEYTYPE key, KEYMOD mod, bool pressed);

  /* 
   * Mouse interface
   */
private:  
  
  MOUSE_STATE amouse;         // Current mouse state
  MOUSE_STATE amouse_last;    // Last mouse state

public:
    
  void mouse_position_set(tpos mx, tpos my);
  void mouse_position_get(tpos *p_mx, tpos *p_my);

  void mouse_position_last_get(tpos *p_mx, tpos *p_my);
  void mouse_position_diff_get(tpos *p_dx, tpos *p_dy);
  
  void mouse_button_set(int button, int state);
  bool mouse_button_get(int button);

public:
  
  void mouse_input(tpos mx, tpos my, int m_activator = 0, int m_modificator = 0);
  
  /* 
   * All events
   */
private:
  
  void  proceed(bool end);

public:
  
  void  start(void);   // Save current state
  void  end(void);     // Process all pending events

public:

  input(EVENT_STREAM  *p_out)
  {
    p_output = p_out;
    
    elist_num = 0;
    memset(elist,0,sizeof(elist));
    
    p_shadow_elist = NULL;
  }
  
} INPUT;

inline bool key_to_ascii(int key, char *p_char)
{
  if(key < 127) {
    *p_char = key;
    return(TRUE);
  }
  else {
    return(FALSE);
  }
}

#endif // __INPUT_H__
