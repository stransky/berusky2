/*
 *        .þÛÛþ þ    þ þÛÛþ.     þ    þ þÛÛÛþ.  þÛÛÛþ .þÛÛþ. þ    þ
 *       .þ   Û Ûþ.  Û Û   þ.    Û    Û Û    þ  Û.    Û.   Û Ûþ.  Û
 *       Û    Û Û Û  Û Û    Û    Û   þ. Û.   Û  Û     Û    Û Û Û  Û
 *     .þþÛÛÛÛþ Û  Û Û þÛÛÛÛþþ.  þþÛÛ.  þþÛÛþ.  þÛ    Û    Û Û  Û Û
 *    .Û      Û Û  .þÛ Û      Û. Û   Û  Û    Û  Û.    þ.   Û Û  .þÛ
 *    þ.      þ þ    þ þ      .þ þ   .þ þ    .þ þÛÛÛþ .þÛÛþ. þ    þ
 *
 * Berusky (C) AnakreoN
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

#ifndef __OBJECT_LIST_H__
#define __OBJECT_LIST_H__

class object_list_head;

/*
 * Basic object what can be stored in a list
 */
typedef class object_list : 
  public object_select,
  public scene_reference
{
  
  MEMBER_LIST_ITEM(object, object_list, object_list_head);
  MEMBER_LIST_ITEM_MARK(object, object_list, object_list_head);
  MEMBER_LIST_ITEM_NAME(object, object_list, object_list_head);
  
public:  

  object_list(SCENE *p_scene)
    : scene_reference(p_scene)
  {
    object_list_item_init();
  }
  
  virtual ~object_list(void)
  {
  }

} OBJECT_LIST;

/*
 * Mark iterators
 */
typedef class object_list_head :
  public scene_reference
{
  
  MEMBER_LIST_HEAD(object, object_list, object_list_head);
  MEMBER_LIST_HEAD_NAME(object, object_list, object_list_head);  
  MEMBER_LIST_HEAD_MARK(object, object_list, object_list_head);  

public:  
  
  object_list_head(SCENE *p_scene)
    : scene_reference(p_scene)
  {
    object_list_head_init();
  }
  
  virtual ~object_list_head(void)
  {
    object_list_clear();
  }
  
} OBJECT_LIST_HEAD;

#endif // __OBJECT_LIST_H__
