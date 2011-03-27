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

#ifndef __EVENTS_TYPES_H__
#define __EVENTS_TYPES_H__

#define FIRST_SYSTEM_EVENT            (INT_MAX>>1)
#define FIRST_SYSTEM_GENERATED_EVENT  (FIRST_SYSTEM_EVENT+1000)

/****************************************************************
  events
  
  UI_ - events from user interface
  
 ****************************************************************
 
 Common parameters:
 
 change_sprite = true -> change sprites on screen
 change_sprite = false -> don't touch sprites on screen  
 
*/

typedef enum {

  // Zero-event
  EV_NONE = FIRST_SYSTEM_EVENT,

  // Test-event
  EV_TEST = FIRST_SYSTEM_EVENT+1,

  // Call event callback function
  // format: [EV_CALLBACK, callback_adress]
  EV_CALLBACK = FIRST_SYSTEM_EVENT+2,
  
  // ------------------------------------------------------------------------
  // GUI events

  // user interface - block selected event (player move and so on)
  // format: [GI_BLOCK_KEYS, key_group, blocked]
  // key_group = key group to block
  // blocked = true/false
  UI_BLOCK_KEYS = FIRST_SYSTEM_EVENT+100,

  // user interface - enable all groups
  // format: [GI_UNBLOCK_ALL]
  UI_UNBLOCK_ALL = FIRST_SYSTEM_EVENT+101,

  // pressed key
  // format: [GI_KEY_DOWN, key, shift, ctrl]
  UI_KEY_DOWN = FIRST_SYSTEM_EVENT+102,
  
  // released key
  // format: [GI_KEY_UP, key, shift, ctrl]
  UI_KEY_UP = FIRST_SYSTEM_EVENT+103,

  // Mouse was moved
  // format: [GI_MOUSE_MOVE, x, y, state, button]
  UI_MOUSE_MOVE = FIRST_SYSTEM_EVENT+104,

  // format: [GI_MOUSE_EVENT, x, y, state, button]
  UI_MOUSE_BUTTON = FIRST_SYSTEM_EVENT+105,
  
  // Application was activated
  UI_APP_ACTIVATED = FIRST_SYSTEM_EVENT+106,
     
  // Application was deactivated
  UI_APP_DEACTIVATED = FIRST_SYSTEM_EVENT+107,
  
  // SDL quit event
  UI_QUIT = FIRST_SYSTEM_EVENT+108

} EVENT_TYPE;

#endif // __EVENTS_TYPES_H__
