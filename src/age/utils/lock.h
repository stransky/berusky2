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
  Lock interface
*/

#ifndef __LOCK_H__
#define __LOCK_H__

#include <assert.h>

typedef enum {
  
  LOCK_NONE = 0,
  LOCK_READ = 1,
  LOCK_READ_WRITE = 2
  
} LOCK_TYPE;

typedef enum {
  
  LOCK_UNCHANGED = 0,
  LOCK_CHANGED = 1,
  LOCK_DEFAULT = 2
  
} LOCK_CHANGE_TYPE;

typedef class lock_interface {

  int       lock_flag;
  bool      lock_data_change;
  LOCK_TYPE lock_type;
  
  /*
   * Lock data changes
   */
private:

  bool lock_data_changed(void)
  {
    return(lock_data_change);
  }
  
  void lock_data_change_set(void)
  {
    lock_data_change = TRUE;
  }
  
  void lock_data_change_clear(void)
  {
    lock_data_change = FALSE;
  }
  
  /*
   * General lock
   */
public:

  // is locked?
  bool locked(void)
  {
    return(lock_flag > 0);
  }

  // Lock data
  void lock(LOCK_TYPE type = LOCK_READ)
  {
    /*
     * Clear data change flag if it's a first lock
     */
    if(lock_flag == 0)
      lock_data_change_clear();
    
    /*
     * Lock it
     */    
    lock_flag++;
    if(type > lock_type)
      lock_type = type;
  }
    
  /*
   * Unlock the interface
   * Returns state of the lock
   *
   * FALSE - It's not unlocked or data were not changed
   * TRUE  - It's unlocked and data were changed
   */
  bool unlock(LOCK_CHANGE_TYPE data_changed = LOCK_DEFAULT)
  {
    bool ret = FALSE;
    
    if(locked()) {
      if(data_changed == LOCK_DEFAULT) {
        data_changed = (lock_type == LOCK_READ_WRITE) ? LOCK_CHANGED : LOCK_UNCHANGED;
      }      
      if(data_changed == LOCK_CHANGED) {
        lock_data_change_set();
      }
      
      lock_flag--;
      
      if(!locked()) {
        ret = lock_data_changed();
        lock_clear();    
      }
    }
    
    return(ret);
  }

  void lock_clear(void)
  {
    lock_flag = 0;
    lock_data_change = FALSE;
    lock_type = LOCK_NONE;
  }
  
  void lock_set(LOCK_TYPE type)
  {
    lock_flag = 1;
    lock_type = type;
  }
  
public:

  void lock_set(class lock_interface *p_src)
  {
    *this = *p_src;
  }
  
public:

  lock_interface(void)
  {
    lock_clear();
  }
  
} LOCK_INTERFACE;

#endif // __LOCK_H__
