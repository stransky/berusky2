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
 
/*
  Parent
    ||
    \/
   Leaf => Leaf => Leaf
    ||
    \/
   Child
*/

#ifndef __MTREE_H__
#define __MTREE_H__

#include <assert.h>

class mtree_leaf;

// mtree node / leaf iterator
typedef class mtree_iterator {
    
friend class mtree_leaf;
  
  class mtree_leaf *p_stop;
  class mtree_leaf *p_act;  
  
public:
  
  mtree_iterator(class mtree_leaf *p_start)
  {
    p_stop = p_act = p_start;
  }
  
} MTREE_ITERATOR;

// Base class for tree nodes / leafs
typedef class mtree_leaf {

  class mtree_leaf *p_parent;        // Parent of this leaf
  class mtree_leaf *p_child;         // First child of this leaf
  
  class mtree_leaf *p_sibling_prev;  // List of leafs on the same level
  class mtree_leaf *p_sibling_next;  // 
  
  /*
   * Basic routines
   */
public:

  class mtree_leaf * leaf_parent_get(void)
  {
    return(p_parent);
  }
  
  // Set parent for this leaf and all its siblings
  // return the last sibling
  class mtree_leaf *leaf_parent_set(class mtree_leaf *p_parent)
  {
    class mtree_leaf *p_tmp = this;
    class mtree_leaf *p_tmp_last = NULL;
    while(p_tmp) {
      p_tmp->p_parent = p_parent;
      p_tmp_last = p_tmp;
      p_tmp = p_tmp->p_sibling_next;      
    }
    return(p_tmp_last);
  }  
    
  class mtree_leaf *leaf_sibling_get(void)
  {
    return(p_sibling_next);
  }
  
  class mtree_leaf *leaf_sibling_last_get(void)
  {
    class mtree_leaf *p_tmp = this;
      
    while(p_tmp->p_sibling_next)
      p_tmp = p_tmp->p_sibling_next;
    
    return(p_tmp);
  }
    
  class mtree_leaf *leaf_child_get(void)
  {
    return(p_child);    
  }
  
  class mtree_leaf *leaf_child_last_get(void)
  {
    if(p_child)
      return(p_child->leaf_sibling_last_get());
    else      
      return(NULL);
  }
  
  /*
   * Hierarchy management
   */
public:
    
  // Addach a new sibling pass this node
  void leaf_sibling_add(class mtree_leaf *p_sib)
  {
    // all the newly inserted siblings have 
    // to point to the current parent
    class mtree_leaf *p_last_new_sibling = p_sib->leaf_parent_set(p_parent);
    assert(p_last_new_sibling);    
    
    p_last_new_sibling->p_sibling_next = p_sibling_next;    
    if(p_sibling_next)
      p_sibling_next->p_sibling_prev = p_last_new_sibling;
    
    p_sib->p_sibling_prev = this;
    p_sibling_next = p_sib;  
  }
  
  // add child to this node  
  void leaf_child_add(class mtree_leaf *p_ch)
  {
    // all the newly inserted childs have 
    // to point to this
    class mtree_leaf *p_last_new_child = p_ch->leaf_parent_set(this);
    assert(p_last_new_child);
    
    // Link current children after the new
    if(p_child) {
      p_last_new_child->p_sibling_next = p_child;
      p_child->p_sibling_prev = p_last_new_child;
    }
    
    // insert-firts the new children
    p_child = p_ch;
    p_ch->p_sibling_prev = NULL;
  }
    
  /*
   * Remove this leaf and all it's siblings from tree
   */
  void leaf_remove(void)
  { 
    // check if we're the first child of its parent
    if(p_parent && p_parent->p_child == this)
      p_parent->p_child = p_sibling_next;
  
    if(p_sibling_prev) {
      p_sibling_prev->p_sibling_next = p_sibling_next;
    }
    if(p_sibling_next) {
      p_sibling_next->p_sibling_prev = p_sibling_prev;
    }    
    
    p_parent = NULL;
    p_sibling_prev = NULL;
    p_sibling_next = NULL;        
  }
    
  /*
   * Iteration over scene hierarchy
   */
public:  
  
  class mtree_leaf  * leaf_next(class mtree_iterator *p_it);
    
  MTREE_ITERATOR      leaf_next_start(void)
  {
    return(MTREE_ITERATOR(this));
  }
  
  /*
   * Rest
   */
public:

  mtree_leaf(void)
  {
    p_parent = NULL;
    p_child = NULL;
    p_sibling_prev = NULL;
    p_sibling_next = NULL;
  }
  
  virtual ~mtree_leaf(void)
  {
    
  }
  
} MTREE_LEAF;

#endif // __MTREE_H__
