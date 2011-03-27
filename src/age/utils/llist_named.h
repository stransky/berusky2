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
 * Linear list
 */ 
 
#ifndef __LLIST_NAMED_H__
#define __LLIST_NAMED_H__


#define MEMBER_LIST_ITEM(name, item_class, head_class)          \
                                                                \
  friend class head_class;                                      \
                                                                \
private:                                                        \
                                                                \
  class item_class *p_name##_list_item_next;                    \
  class item_class *p_name##_list_item_prev;                    \
                                                                \
public:                                                         \
                                                                \
  class item_class * name##_list_append(class item_class *p_new) \
  {                                                             \
    p_name##_list_item_next = p_new;                            \
    if(p_new) {                                                 \
      p_new->p_name##_list_item_prev = this;                    \
    }                                                           \
    return(this);                                               \
  }                                                             \
                                                                \
  void name##_list_remove(void)                                 \
  {                                                             \
    if(p_name##_list_item_next) {                               \
      p_name##_list_item_next->p_name##_list_item_prev = p_name##_list_item_prev;  \
    }                                                           \
    if(p_name##_list_item_prev) {                               \
      p_name##_list_item_prev->p_name##_list_item_next = p_name##_list_item_next;  \
    }                                                           \
  }                                                             \
                                                                \
  class item_class * name##_list_next(void)                     \
  {                                                             \
    return(p_name##_list_item_next);                            \
  }                                                             \
                                                                \
  class item_class * name##_list_find_last(void)                \
  {                                                             \
    class item_class *p_tmp = this;                             \
    class item_class *p_last = this;                            \
    while((p_tmp = p_tmp->name##_list_next())) {                \
      p_last = p_tmp;                                           \
    }                                                           \
    return(p_last);                                             \
  }                                                             \
                                                                \
public:                                                         \
                                                                \
  void name##_list_item_init(void)                              \
  {                                                             \
    p_name##_list_item_next = NULL;                             \
    p_name##_list_item_prev = NULL;                             \
  }                                               

  
#define MEMBER_LIST_ITEM_MARK(name, item_class, head_class)     \
                                                                \
public:                                                         \
                                                                \
  class item_class * name##_list_next_mark(int mark)            \
  {                                                             \
    class item_class *p_tmp = this;                             \
    while((p_tmp = p_tmp->name##_list_next())) {                \
      if(p_tmp->mark_get(mark))                                 \
        return(p_tmp);                                          \
    }                                                           \
    return(NULL);                                               \
  }

  
#define MEMBER_LIST_ITEM_NAME(name, item_class, head_class)     \
                                                                \
public:                                                         \
                                                                \
  class item_class * name##_list_next_find(const char *p_name)  \
  {                                                             \
    class item_class *p_tmp = this;                             \
    while((p_tmp = p_tmp->name##_list_next())) {                \
      if(p_tmp->match_name(p_name))                             \
        return(p_tmp);                                          \
    }                                                           \
    return(NULL);                                               \
  }
  

#define MEMBER_LIST_HEAD(name, item_class, head_class)          \
                                                                \
private:                                                        \
                                                                \
  class item_class *p_name##_first_item;                        \
  class item_class *p_name##_last_item;                         \
                                                                \
public:                                                         \
                                                                \
  class item_class * name##_list_get_first(void)                \
  {                                                             \
    return(p_name##_first_item);                                \
  }                                                             \
  class item_class * name##_list_get_last(void)                 \
  {                                                             \
    return(p_name##_last_item);                                 \
  }                                                             \
                                                                \
  class item_class * name##_list_insert_first(class item_class *p_pol) \
  {                                                             \
    if(p_name##_first_item) {                                   \
      p_pol->name##_list_append(p_name##_first_item);           \
      p_pol->p_name##_list_item_prev = NULL;                    \
      p_name##_first_item = p_pol;                              \
    }                                                           \
    else {                                                      \
      p_name##_last_item = p_name##_first_item = p_pol;         \
      p_pol->p_name##_list_item_next = p_pol->p_name##_list_item_prev = NULL; \
    }                                                           \
    return(p_pol);                                              \
  }                                                             \
  class item_class * name##_list_insert_last(class item_class *p_pol) \
  {                                                             \
    if(p_name##_last_item) {                                    \
      p_name##_last_item->name##_list_append(p_pol);            \
      p_pol->p_name##_list_item_next = NULL;                    \
      p_name##_last_item = p_pol;                               \
    } else {                                                    \
      p_name##_last_item = p_name##_first_item = p_pol;         \
      p_pol->p_name##_list_item_next = p_pol->p_name##_list_item_prev = NULL; \
    }                                                           \
    return(p_pol);                                              \
  }                                                             \
                                                                \
  class item_class * name##_list_insert(class item_class *p_pol) \
  {                                                             \
    return(name##_list_insert_last(p_pol));                     \
  }                                                             \
  class item_class * name##_list_insert_list(class item_class *p_list) \
  {                                                             \
    if(p_name##_last_item) {                                    \
      p_name##_last_item->name##_list_append(p_list);           \
    } else {                                                    \
      p_name##_first_item = p_list;                             \
    }                                                           \
    p_name##_last_item = p_list->name##_list_find_last();       \
    return(p_list);                                             \
  }                                                             \
  void name##_list_remove(class item_class *p_pol)              \
  {                                                             \
    if(p_pol == p_name##_first_item)                            \
      p_name##_first_item = p_pol->p_name##_list_item_next;     \
                                                                \
    if(p_pol == p_name##_last_item)                             \
      p_name##_last_item = p_pol->p_name##_list_item_prev;      \
                                                                \
    p_pol->name##_list_remove();                                \
  }                                                             \
                                                                \
public:                                                         \
                                                                \
  void name##_list_head_init(void)                              \
  {                                                             \
    p_name##_first_item = NULL;                                 \
    p_name##_last_item = NULL;                                  \
  }                                                             \
  void name##_list_clear(void)                                  \
  {                                                             \
    class item_class *p_item = name##_list_get_first();         \
    while(p_item) {                                             \
      class item_class *p_next = p_item->name##_list_next();    \
      delete p_item;                                            \
      p_item = p_next;                                          \
    }                                                           \
    name##_list_head_init();                                    \
  }


#define MEMBER_LIST_HEAD_NAME(name, item_class, head_class)     \
                                                                \
public:                                                         \
                                                                \
  class item_class * name##_list_find(const char *p_name)       \
  {                                                             \
    if(!p_name)                                                 \
      return(NULL);                                             \
    class item_class *p_tmp = name##_list_get_first();          \
    while(p_tmp) {                                              \
      if(p_tmp->match_name(p_name))                             \
        return(p_tmp);                                          \
      p_tmp = p_tmp->name##_list_next();                        \
    }                                                           \
    return(NULL);                                               \
  }

#define MEMBER_LIST_HEAD_MARK(name, item_class, head_class)     \
                                                                \
public:                                                         \
                                                                \
  class item_class * name##_list_first_mark(int mark)           \
  {                                                             \
    class item_class *p_tmp = name##_list_get_first();          \
    if(!p_tmp)                                                  \
      return(NULL);                                             \
    if(p_tmp->mark_get(mark))                                   \
      return(p_tmp);                                            \
    return(p_tmp->name##_list_next_mark(mark));                 \
  }

  
#endif //__LLIST_NAMED_H__
