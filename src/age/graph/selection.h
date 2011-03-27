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
  Scene object selection
*/

#ifndef  __SELECTION_H__
#define  __SELECTION_H__

class selected_object_item;
typedef class selected_object_item SELECTED_OBJECT_ITEM;

class selected_object_head;
typedef class selected_object_head SELECTED_OBJECT_HEAD;

typedef class selected_object_item
  : public llist_item
{
  friend class selected_object_head;
 
private:
  
  OBJECT_SELECT * p_object;

public:
  
  OBJECT_SELECT * object_get(void)
  {
    return(p_object);
  }

public:

  SELECTED_OBJECT_ITEM * object_next(void)
  {
    return((SELECTED_OBJECT_ITEM *)list_next());
  }
  
} SELECTED_OBJECT_ITEM;

bool selection_search(void *p_search_data, LLIST_ITEM *p_current);

typedef class selected_object_head
  : public llist_head
{

public:
  
  SELECTED_OBJECT_ITEM * get_first(void)
  {  
    return((SELECTED_OBJECT_ITEM *)list_get_first());
  }

private:

  void remove(SELECTED_OBJECT_ITEM * p_item)
  {    
    p_item->p_object->mark_clear(MARK_SELECTED);
    list_remove(p_item);
    delete p_item;
  }
  
public:
  
  bool insert(OBJECT_SELECT *p_object)
  {    
    if(find(p_object, selection_search) == NULL)
    {
      SELECTED_OBJECT_ITEM *p_item = new SELECTED_OBJECT_ITEM;
      p_item->p_object = p_object;
      list_insert_last(p_item);
      p_object->mark_set(MARK_SELECTED);
      return(TRUE);
    }
    else {
      return(FALSE);
    }
  }
  
  bool remove(OBJECT_SELECT *p_object)
  {    
    SELECTED_OBJECT_ITEM *p_item = (SELECTED_OBJECT_ITEM *)find(p_object, selection_search);
    if(p_item != NULL)
    {
      remove(p_item);
      return(TRUE);
    }
    else {
      return(FALSE);
    }
  }

public:  
    
  void clear(void)
  {
    SELECTED_OBJECT_ITEM *p_item = (SELECTED_OBJECT_ITEM *)list_get_first();
    
    while(p_item) {
      SELECTED_OBJECT_ITEM *p_next = (SELECTED_OBJECT_ITEM *)p_item->list_next();
      p_item->p_object->mark_clear(MARK_SELECTED);
      delete p_item;
      p_item = p_next;
    }
    
    list_clear();
  }
  
  void destroy(void)
  {
    clear();
  }
  
public:

  selected_object_head(void)
  {
  }
  
  ~selected_object_head(void)
  {
    destroy();
  }
  
} SELECTED_OBJECT_HEAD;

#endif //  __SELECTION_H__
