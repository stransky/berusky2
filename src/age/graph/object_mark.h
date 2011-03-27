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
  Object marking
*/

#ifndef __OBJECT_MARK_H__
#define __OBJECT_MARK_H__

/*
 * Global object marks
 */

/* 
 * MARK_CREATED has all recently created objects.
 */
#define MARK_CREATED                    (1<<0)

/* 
 * MARK_RENDERER_HIERARCHY has objects which are placed in scene rendering tree
 * (and rendered if they don't have MARK_RENDERER_DISABLED mark set)
 */
#define MARK_RENDERER_HIERARCHY         (1<<1)

/* 
 * Object with MARK_RENDERER_DISABLED mark are not rendered
 */
#define MARK_RENDERER_DISABLED          (1<<2)

/* 
 * MARK_SELECTED has all objects which are selected by scene selection
 */
#define MARK_SELECTED                   (1<<3)

/* 
 * MARK_CREATED_BY_GENERATOR has all objects which have been created
 * by editor generator. Those objects are removed during the clean-up phase
 */
#define MARK_CREATED_BY_GENERATOR       (1<<4)


typedef class object_mark 
  : public object_name
{

  int iflag;
  
public:
  
  bool mark_get(int mark)
  {
    return(iflag&mark);
  }
  
  void mark_set(int mark)
  {
    iflag |= mark;
  }
  
  void mark_clear(int mark)
  {
    iflag &= ~mark;
  }

private:
  
  void mark_set_all(void)
  {
    iflag = 0;
    iflag = ~iflag;
  }
  
  void mark_clear_all(void)
  {
    iflag = 0;
  }

public:
  
  object_mark(void)
  {
    mark_clear_all();
    mark_set(MARK_CREATED);
  }
  
} OBJECT_MARK;

#endif // __OBJECT_MARK_H__
