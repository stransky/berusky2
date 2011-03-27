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

#ifndef __MTREE_NAMED_H__
#define __MTREE_NAMED_H__

#include <assert.h>

#define CLASS_TREE_LEAF_ITERATOR(name, leaf_class)  \
                                                    \
class name##_leaf_iterator {                        \
                                                    \
  friend class leaf_class;                          \
                                                    \
  class leaf_class *p_stop;                         \
  class leaf_class *p_act;                          \
                                                    \
public:                                             \
                                                    \
  name##_leaf_iterator(class leaf_class *p_start)   \
  {                                                 \
    p_stop = p_act = p_start;                       \
  }                                                 \
};


#define MEMBER_TREE_LEAF(name, leaf_class)          \
                                                    \
private:                                            \
                                                    \
  class leaf_class *p_name##_leaf_parent;           \
  class leaf_class *p_name##_leaf_child;            \
                                                    \
  class leaf_class *p_name##_leaf_sibling_prev;     \
  class leaf_class *p_name##_leaf_sibling_next;     \
                                                    \
public:                                             \
                                                    \
  class leaf_class * name##_leaf_parent_get(void)   \
  {                                                 \
    return(p_name##_leaf_parent);                   \
  }                                                 \
                                                    \
  class leaf_class * name##_leaf_parent_set(class leaf_class *p_parent) \
  {                                                 \
    class leaf_class *p_tmp = this;                 \
    class leaf_class *p_tmp_last = NULL;            \
    while(p_tmp) {                                  \
      p_tmp->p_name##_leaf_parent = p_parent;       \
      p_tmp_last = p_tmp;                           \
      p_tmp = p_tmp->p_name##_leaf_sibling_next;    \
    }                                               \
    return(p_tmp_last);                             \
  }                                                 \
                                                    \
  class leaf_class * name##_leaf_sibling_get(void)  \
  {                                                 \
    return(p_name##_leaf_sibling_next);             \
  }                                                 \
                                                    \
  class leaf_class * name##_leaf_sibling_last_get(void) \
  {                                                 \
    class leaf_class *p_tmp = this;                 \
                                                    \
    while(p_tmp->p_name##_leaf_sibling_next)        \
      p_tmp = p_tmp->p_name##_leaf_sibling_next;    \
                                                    \
    return(p_tmp);                                  \
  }                                                 \
                                                    \
  class leaf_class * name##_leaf_child_get(void)    \
  {                                                 \
    return(p_name##_leaf_child);                    \
  }                                                 \
                                                    \
  class leaf_class * name##_leaf_child_last_get(void) \
  {                                                 \
    if(p_name##_leaf_child)                         \
      return(p_name##_leaf_child->name##_leaf_sibling_last_get());  \
    else                                            \
      return(NULL);                                 \
  }                                                 \
                                                    \
public:                                             \
                                                    \
  void name##_leaf_sibling_add(class leaf_class *p_sib) \
  {                                                 \
    class leaf_class *p_last_new_sibling = p_sib->name##_leaf_parent_set(p_name##_leaf_parent); \
    assert(p_last_new_sibling);                     \
                                                    \
    p_last_new_sibling->p_name##_leaf_sibling_next = p_name##_leaf_sibling_next;    \
    if(p_name##_leaf_sibling_next)                  \
      p_name##_leaf_sibling_next->p_name##_leaf_sibling_prev = p_last_new_sibling;  \
                                                    \
    p_sib->p_name##_leaf_sibling_prev = this;       \
    p_name##_leaf_sibling_next = p_sib;             \
  }                                                 \
                                                    \
  void name##_leaf_child_add(class leaf_class *p_ch)  \
  {                                                 \
    class leaf_class *p_last_new_child = p_ch->name##_leaf_parent_set(this);  \
    assert(p_last_new_child);                       \
                                                    \
    if(p_name##_leaf_child) {                       \
      p_last_new_child->p_name##_leaf_sibling_next = p_name##_leaf_child; \
      p_name##_leaf_child->p_name##_leaf_sibling_prev = p_last_new_child; \
    }                                               \
                                                    \
    p_name##_leaf_child = p_ch;                     \
    p_ch->p_name##_leaf_sibling_prev = NULL;        \
  }                                                 \
                                                    \
  void name##_leaf_remove(void)                     \
  {                                                 \
    if(p_name##_leaf_parent && p_name##_leaf_parent->p_name##_leaf_child == this) \
      p_name##_leaf_parent->p_name##_leaf_child = p_name##_leaf_sibling_next;     \
                                                    \
    if(p_name##_leaf_sibling_prev) {                \
      p_name##_leaf_sibling_prev->p_name##_leaf_sibling_next = p_name##_leaf_sibling_next;  \
    }                                               \
    if(p_name##_leaf_sibling_next) {                \
      p_name##_leaf_sibling_next->p_name##_leaf_sibling_prev = p_name##_leaf_sibling_prev;  \
    }                                               \
                                                    \
    p_name##_leaf_parent = NULL;                    \
    p_name##_leaf_sibling_prev = NULL;              \
    p_name##_leaf_sibling_next = NULL;              \
  }                                                 \
                                                    \
public:                                             \
                                                    \
  void name##_leaf_init(void)                       \
  {                                                 \
    p_name##_leaf_parent = NULL;                    \
    p_name##_leaf_child = NULL;                     \
    p_name##_leaf_sibling_prev = NULL;              \
    p_name##_leaf_sibling_next = NULL;              \
  }                                                 \
                                                    \
public:                                             \
                                                    \
  class name##_leaf_iterator name##_leaf_next_start(void) \
  {                                                 \
    class name##_leaf_iterator tmp(this);           \
    return(tmp);                                    \
  }                                                 \
                                                    \
  class leaf_class * name##_leaf_next(class name##_leaf_iterator *p_it) \
  {                                                 \
    class leaf_class * p_act = p_it->p_act;         \
                                                    \
    if(p_it->p_stop == p_act) {                     \
      if(p_act)                                     \
        p_it->p_stop = p_act->name##_leaf_parent_get(); \
      return(p_act);                                \
    }                                               \
                                                    \
    class leaf_class * p_last_act = p_it->p_act;    \
                                                    \
    p_act = p_last_act->name##_leaf_child_get();    \
    if(p_act) {                                     \
      p_it->p_act = p_act;                          \
      return(p_act);                                \
    }                                               \
                                                    \
    p_act = p_last_act->name##_leaf_sibling_get();  \
    if(p_act) {                                     \
      p_it->p_act = p_act;                          \
      return(p_act);                                \
    }                                               \
                                                    \
    do {                                            \
      p_act = p_last_act->name##_leaf_parent_get(); \
      if(p_act == p_it->p_stop) {                   \
        return(NULL);                               \
      }                                             \
      assert(p_act != NULL);                        \
      p_last_act = p_act;                           \
      p_act = p_act->name##_leaf_sibling_get();     \
    } while(p_act == NULL);                         \
                                                    \
    p_it->p_act = p_act;                            \
    return(p_act);                                  \
  }

#endif // __MTREE_NAMED_H__
