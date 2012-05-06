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
#define  KEYNUM       SDLK_LAST

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

// -------------------------------------------------------
// Mouse UI
// -------------------------------------------------------

#define MOUSE_BUTTONS                 5

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

void key_repeat_set(bool state);

#endif // __INPUT_H__
