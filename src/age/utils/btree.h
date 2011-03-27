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
  Binary tree implementation
*/

#ifndef __BTREE_H__
#define __BTREE_H__

#include <assert.h>

class btree_leaf;

// btree node / leaf iterator
typedef class btree_iterator {
    
friend class btree_leaf;
  
  class btree_leaf *p_act;  
  
public:
  
  btree_iterator(class btree_leaf *p_start)
  {
    p_act = p_start;
  }
  
} BTREE_ITERATOR;

// Base class for tree nodes / leafs
typedef class btree_leaf {

  class btree_leaf *p_parent;        // Parent of this leaf
    
  class btree_leaf *p_childl;        // Left child
  class btree_leaf *p_childr;        // Right child
    
  /*
   * Basic routines
   */
public:

  class btree_leaf * leaf_parent_get(void)
  {
    return(p_parent);
  }
  
  /*
   * Hierarchy management
   */
public:
    
  // Addach a new sibling pass this node
  void leaf_sibling_add(class btree_leaf *p_sib)
  {
  }
  
  // add child to this node  
  void leaf_child_add(class btree_leaf *p_ch)
  {
  }
    
  /*
   * Remove this leaf and all it's siblings from tree
   */
  void leaf_remove(void)
  { 
  }
    
  /*
   * Iteration over scene hierarchy
   */
public:  
  
  class btree_leaf  * leaf_next(class btree_iterator *p_it);
  
  /*
   * Rest
   */
public:

  btree_leaf(void)
  {
    p_parent = NULL;
    p_childl = NULL;
    p_childr = NULL;
  }
  
  virtual ~btree_leaf(void)
  {
    
  }
  
} BTREE_LEAF;

#endif // __MTREE_H__
