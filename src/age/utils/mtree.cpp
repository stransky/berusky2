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
 
#include <assert.h>

#include "defs.h"
#include "typ.h"
#include "mtree.h"

/*
  Parent
    ||
    \/
   Leaf => Leaf => Leaf
    ||
    \/
   Child
*/

/*
it(start);
while((p_act = next(it))) {
  p_act;
}
*/

class mtree_leaf * mtree_leaf::leaf_next(MTREE_ITERATOR *p_it)
{
  class mtree_leaf * p_act = p_it->p_act;
  
  if(p_it->p_stop == p_act) {
    if(p_act)  
      p_it->p_stop = p_act->leaf_parent_get();
    return(p_act);
  }
  
  class mtree_leaf * p_last_act = p_it->p_act;
    
  // We have a child -> check it
  p_act = p_last_act->leaf_child_get();
  if(p_act) {
    p_it->p_act = p_act;
    return(p_act);
  }
  
  // We don't have a child -> check next sibling
  p_act = p_last_act->leaf_sibling_get();
  if(p_act) {
    p_it->p_act = p_act;
    return(p_act);
  }
  
  // We don't have a sibling -> go up
  // find a parent with sibling
  do {
    p_act = p_last_act->leaf_parent_get();
    if(p_act == p_it->p_stop) {
      return(NULL);
    }
    assert(p_act != NULL);
    p_last_act = p_act;
    p_act = p_act->leaf_sibling_get();
  } while(p_act == NULL);
  
  p_it->p_act = p_act;
  return(p_act);
}
