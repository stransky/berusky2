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

#ifndef __ARRAY_H__
#define __ARRAY_H__

#include <assert.h>

#define ARRAY_DEFAULT_ITEMS 1000

#ifndef NULL
#define NULL (0)
#endif

template<class T> class array {

  T * p_array;

  // points after max/actual item
  int max_size;
  int act_size;
  
  int extend_step;

  int first;
  
private:
  
  void extend(int index)
  {     
    // Check if we overflow and extend the array if so
    int new_index = index+1;
    
    if(new_index < max_size)
      return;

    int new_size = ((new_index / extend_step)+1)*extend_step;
      
    T * p_tmp = new T[new_size];

    int i;
    for(i = 0; i < max_size; i++)
      p_tmp[i] = p_array[i];

    max_size = new_size;

    delete [] p_array;
    p_array = p_tmp;    
  }

public:

  void reset(void)
  {
    act_size = 0;
    first = 0;
  }

  void append(T itm)
  {
    // Save itm at p_array[act_size-1], extend 
    extend(act_size);
    p_array[act_size++] = itm;
  }

  T last_get(void)
  {    
    assert(!is_empty());
    return(p_array[--act_size]);
  }

public:  
  
  bool is_empty(void)
  {
    return(act_size <= 0 || first == act_size);
  }

public:

  T * first_get(void)
  {
    return(p_array+first);
  }

  int num_get(void)
  {
    return(act_size-first);
  }
  
public:

  T * item_get(int index)
  {
    extend(index);
    return(p_array+index);
  }

  void item_set(int index, T item)
  {
    extend(index);
    p_array[index] = item;
  }
  
public:

  T * fetch_ref(void)
  {
    assert(!is_empty());
    return(p_array+first++);
  } 

  T fetch(void)
  {
    return(*fetch_ref());
  } 
  
public:

  array(int step = ARRAY_DEFAULT_ITEMS)
  {
    p_array = NULL;
    max_size = act_size = first = 0;
    extend_step = step;
  }
  
  ~array(void)
  {
    if(p_array) {  
      delete [] p_array;
      max_size = act_size = first = 0;
    }
  }
};

#endif
