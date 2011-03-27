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
  Change interface
*/

#ifndef __CHANGE_H__
#define __CHANGE_H__

#include <assert.h>

typedef class change_interface {
  
  bool  change;
  
public:

  bool changed(void)
  {
    return(change);
  }
  
  void change_set(void)
  {
    change = TRUE;
  }
  
  void change_clear(void)
  {
    change = FALSE;
  }
  
public:

  change_interface(void)
  {
    change_clear();
  }
  
} CHANGE_INTERFACE;

typedef class frame_change_interface {

  /*
   * Current scene frame
   */
public:  
  
  static int current_frame;
  
  /*
   * Frame when it was changed
   */
private:  
  
  int last_frame;
  
public:
  
  bool changed(void)
  {
    return(last_frame == current_frame);
  }
    
  void change_set(void)
  {
    last_frame = current_frame;
  }
  
} FRAME_CHANGE_INTERFACE;

#endif
