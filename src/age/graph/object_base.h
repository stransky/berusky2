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
  Base scene object
  
  It's used as base for all superior scene objects
  (boxes/transformations and so on)
*/

#ifndef __OBJECT_BASE_H__
#define __OBJECT_BASE_H__

/*
  A base class for all objects
  It contains some basic functions (optional)
  and reference counting.
*/
typedef class object_base  
{
  
  int   reference_num;

public:  
  
  int reference_get(void)
  {    
    return(reference_num);
  }

  int reference_add(void)
  {
    reference_num++;
    return(reference_num);
  }
  
  int reference_dec(void)
  {
    assert(reference_num > 0);
    reference_num--;
    return(reference_num);
  }
  
public:

  /*
   * Creates internal content of the object
   * it can be called only once and usually from object constructor
   */
  void create(void)
  {
  }
  
  /*
   * Clear all internal values of the object
   * it can be called from object constructor many times
   */
  void clear(void)
  {
  }
  
  /*
   * Destroy the object (if it isn't referenced)
   */
  void destroy(void)
  {
    assert(reference_num >= 0);
    if(reference_num == 0)
      delete this;
  }
  
/*
  void operator delete(void *p_tmp)
  {
    assert(0);
  }
*/
  
public:
  
  object_base(void)
  {
    reference_num = 0;
  }
  
  virtual ~object_base(void)
  {
    assert(reference_num == 0);
  }
  
} OBJECT_BASE;

#endif // __OBJECT_BASE_H__
