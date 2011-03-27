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

#ifndef __STACK_H__
#define __STACK_H__

#include <assert.h>

#define STACK_DEFAULT_GRANULARITY_LARGE   1000
#define STACK_DEFAULT_GRANULARITY_SMALL   100

#define STACK_DEFAULT_GRANULARITY   STACK_DEFAULT_GRANULARITY_SMALL

#ifndef NULL
#define NULL (0)
#endif

template<class T> class stack {

  T * p_stack;

  int max_size;   // Max current size of stack
  int act_size;   // Points to next empty item in stack
  int step_size;  // Stack granularity

public:

  void clear(int index = 0)
  {
    assert(index >= 0 && index < max_size);
    
    if(index < act_size)
      act_size = index;
  }

private:
  
  void extend(void)
  {
    T * p_tmp = new T[max_size + step_size];
  
    int i;
    for(i = 0; i < max_size; i++)
      p_tmp[i] = p_stack[i];
    
    max_size += step_size;
  
    delete [] p_stack;
    p_stack = p_tmp;
  }

public:

  int push(T itm)
  {
    if(act_size + 1 >= max_size) {
      extend();
    }      
    p_stack[act_size] = itm;
    
    // Return index of the inserted item
    return(act_size++);
  }

  T pop(void)
  {
    assert(act_size > 0);
    return(p_stack[--act_size]);
  }

  bool is_empty(void)
  {
    return(act_size <= 0);
  }

public:

  bool is_index(int index)
  {
    return(act_size - 1 - index >= 0);
  }

  T get(int index)
  {
    assert(is_index(index));
    return(p_stack[act_size - 1 - index]);
  } 
  
public:  
  
  stack(int size_granularity = STACK_DEFAULT_GRANULARITY)
  {
    step_size = size_granularity;
    max_size = act_size = 0;
    p_stack = NULL;
  }
  
  ~stack(void)
  {
    if(p_stack) {  
      delete [] p_stack;
      max_size = act_size = 0;
    }
  }
};

#define CIRCLE_STACK_DEFAULT_GRANULARITY 100

template<class T> class circle_stack {

  T store[CIRCLE_STACK_DEFAULT_GRANULARITY];

  int first;  // the first used
  int next;   // the next one

private:
  
  int next_index(int index)
  {
    return((index+1 >= CIRCLE_STACK_DEFAULT_GRANULARITY) ? 0 : index+1);
  }

  int prev_index(int index)
  {
    return((index-1 >= 0) ? index-1 : CIRCLE_STACK_DEFAULT_GRANULARITY-1);
  }
  
public:

  void clear(void)
  {
    first = next = 0;
  }

  // Is empty when next == first
  bool is_empty(void)
  {
    return(first == next);
  }

  // Is full when next+1 == first
  bool is_full(void)
  {
    return(next_index(next) == first);
  }

public:

  void push(T &itm)
  {
    if(is_full()) {
      first = next_index(first);
    } 
    store[next] = itm;
    next = next_index(next);
  }

  T * push_pointer(void)
  {
    if(is_full()) {
      first = next_index(first);
    }
  
    T *p_tmp = store+next;
    next = next_index(next);
  
    return(p_tmp);
  }

  T pop(void)
  {
    if(!is_empty()) {
      next = prev_index(next);
      return(store[next]);
    }    
  }

  T * pop_pointer(void)
  {
    if(!is_empty()) {
      next = prev_index(next);
      return(store+next);
    }
    else {
      return(NULL);
    }
  }

  T * top(void)
  {
    if(!is_empty()) {
      return(store+prev_index(next));
    } 
    else {
      return(NULL);
    }
  }

public:

  circle_stack(void)
  {
    clear();
  }
};

#endif
