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

#ifndef __EVENTS_H__
#define __EVENTS_H__

#include <sys/time.h>
#include <assert.h>

#include "events_types.h"

#define EVENT_LAST        0
#define EVENT_FIRST       1

#define PARAM_0           0
#define PARAM_1           1
#define PARAM_2           2
#define PARAM_3           3
#define PARAM_4           4
#define PARAM_5           5

#define PARAM_GROUP       PARAM_0
#define PARAM_BLOCK       PARAM_1

#define PARAMS            6

typedef void * EVENT_PARAM_TYPE;

#define EVENT_TYPE_NONE   ((EVENT_PARAM_TYPE)(NULL))
#define ET(p)             ((EVENT_PARAM_TYPE)(p))

/* Everything
*/
typedef class game_event {

  static const char * translation_table[];
  static const char * translate_event(int type);

private:
  
  int               type;
  int               depend_events;
  EVENT_PARAM_TYPE  params[PARAMS];

  // Basic controls
public:
  
  bool valid(void)
  {
    return(type != EV_NONE);
  }

  bool used(void)
  {
    return(valid());
  }
  
  // Clear this event
public:

  void clear(void)
  {
    type = EV_NONE;
  }

  // Set type of event
public:

  void type_set(int etype)
  {
    type = etype;
  }

  int type_get(void)
  {
    return(type);
  }
  
  // Set parameters of event
public:
  
  void params_pointer_set(EVENT_PARAM_TYPE p0,
                          EVENT_PARAM_TYPE p1 = EVENT_TYPE_NONE,
                          EVENT_PARAM_TYPE p2 = EVENT_TYPE_NONE,
                          EVENT_PARAM_TYPE p3 = EVENT_TYPE_NONE, 
                          EVENT_PARAM_TYPE p4 = EVENT_TYPE_NONE,
                          EVENT_PARAM_TYPE p5 = EVENT_TYPE_NONE)
  {
    params[0] = p0;
    params[1] = p1;
    params[2] = p2;
    params[3] = p3;
    params[4] = p4;
    params[5] = p5;
  }

  void params_int_set(int p0, int p1 = 0, int p2 = 0, int p3 = 0, int p4 = 0,
                      int p5 = 0, int p6 = 0)
  {
    params[0] = (EVENT_PARAM_TYPE)p0;
    params[1] = (EVENT_PARAM_TYPE)p1;
    params[2] = (EVENT_PARAM_TYPE)p2;
    params[3] = (EVENT_PARAM_TYPE)p3;
    params[4] = (EVENT_PARAM_TYPE)p4;
    params[5] = (EVENT_PARAM_TYPE)p5;
  }
  
  int param_int_get(int index)
  {    
    assert(index < PARAMS);
    int *p_tmp = (int *)(params+index);
    return((int)(*p_tmp));
  }

  void param_int_set(int index, int value)
  {    
    assert(index < PARAMS);
    params[index] = (EVENT_PARAM_TYPE)value;
  }
  
  void * param_pointer_get(int index)
  {
    assert(index < PARAMS);
    return((void *)(params[index]));
  }

  void param_pointer_get(int index, void *p_value)
  {
    assert(index < PARAMS);
    params[index] = p_value;
  }

  // Helpers
public:
  
  const char * translate(void)
  {
    return(translate_event(type));
  }

  void dump(void)
  {
    pprintf("event = %s, params = [%d,%d,%d,%d,%d,%d]",
            translate(),
            param_int_get(0),
            param_int_get(1),
            param_int_get(2),
            param_int_get(3),
            param_int_get(4),
            param_int_get(5));
  }

  void depends_add(int num)
  {
    depend_events += num;
  }  

  // Constructors
public:
  
  game_event(void)
  : type(EV_NONE), depend_events(0)
  {
    params_int_set(0,0,0,0,0,0);
  }

  game_event(EVENT_TYPE  ev)
  : type(ev), depend_events(0)
  {
    params_int_set(0,0,0,0,0,0);
  }

  game_event(EVENT_TYPE  ev,
              EVENT_PARAM_TYPE p0,
              EVENT_PARAM_TYPE p1 = EVENT_TYPE_NONE,
              EVENT_PARAM_TYPE p2 = EVENT_TYPE_NONE,
              EVENT_PARAM_TYPE p3 = EVENT_TYPE_NONE, 
              EVENT_PARAM_TYPE p4 = EVENT_TYPE_NONE,
              EVENT_PARAM_TYPE p5 = EVENT_TYPE_NONE)
  : type(ev), depend_events(0)
  {
    params_pointer_set(p0,p1,p2,p3,p4,p5);
  }

  game_event(EVENT_TYPE  ev,
              int p0,
              int p1 = 0,
              int p2 = 0,
              int p3 = 0, 
              int p4 = 0,
              int p5 = 0)
  : type(ev), depend_events(0)
  {
    params_int_set(p0,p1,p2,p3,p4,p5);
  }

} GAME_EVENT;

#define EXTEND_STEP 1000

typedef class event_stream {
 
  int          max_events;   // the lenght of queue
  GAME_EVENT  *p_queue;      // read queue

  int          read_first;
  int          read_next;

  int          write_first;
  int          write_next;

  // ----------------------------------------------------
  // Helpers
  // ----------------------------------------------------  
private:  
  
  bool empty(int first, int next)
  {
    return(first == next);
  }
  bool full(int first, int next)
  {
    return(first == index_next(next));
  }
  
  bool in_range(int first, int last, int index)
  {
    if(first < last) { // normal order
      return(index >= first && index <= last);
    } else {           // flipped order
      return(index >= first || index <= last);
    }    
  }
  
  void dump(const char *p_desc, int first, int next)
  {
    int akt;
  
    if(!empty(first,next)) {
      pprintf(p_desc);
      for(akt = first; akt != next; akt = index_next(akt)) {
        p_queue[akt].dump();
      }
    }
  }
  
  void testcase(void);
  
  // ----------------------------------------------------
  // basic operations
  // ----------------------------------------------------
public:
  
  int index_next(int index)
  {    
    return ++index < max_events ? index : index = 0;
  }
  int index_prev(int index)
  {    
    return --index < 0 ? index = max_events -1 : index;
  }

  void dump_read(const char *p_desc)
  {
    dump(p_desc,read_first,read_next);
  }
  void dump_write(const char *p_desc)
  {
    dump(p_desc,write_first,write_next);
  }
  
  void dump(void)
  {
    dump_read("Read stream");
    dump_write("Write stream");
  }
  
  // ----------------------------------------------------
  // basic checks & sets
  // ----------------------------------------------------
public:  
  
  bool empty(void)
  {
    return(empty(read_first, read_next));
  }
  bool full(void)
  {
    return(full(read_first, write_next));
  }
    
  void clear(void)
  {
    read_first = read_next = write_first = write_next = 0;
  }

  // ----------------------------------------------------
  // Read events
  // ----------------------------------------------------
public:
  
  GAME_EVENT get(void)
  {
    assert(!empty());
  
    if(!empty()) {
      int tmp_index = read_first;
      read_first = index_next(read_first);
      return p_queue[tmp_index];
    }
    else {
      return(GAME_EVENT());
    }
  }

  // ----------------------------------------------------
  // Write events
  // ----------------------------------------------------
public:  

  void add(GAME_EVENT *p_event, bool safe = TRUE)
  {
    if(!full()) {
      if(p_event->valid()) {
        p_queue[write_next] = *p_event;
        write_next = index_next(write_next);
      }
    }
    else if(safe)
      assert(0);
  }

  void add(GAME_EVENT event, bool safe = TRUE)
  {
    add(&event,safe);
  }
  
  #define DIRECTION_ARRAY   1
  #define DIRECTION_STACK (-1)

  int add(GAME_EVENT *p_event, int num, int direction = 1,
          bool safe = TRUE)
  {
    assert(p_event);
  
    int i;
  
    for(i = 0; i < num; i++, p_event += direction) {
      if(full()) {
        if(safe)
          assert(0);
        else
          return(num);
      } else if(p_event->valid()) {
        p_queue[write_next] = *p_event;
        write_next = index_next(write_next);
      }
    }
  
    return(num);
  }

  void add(class event_stream *p_stream, bool safe = TRUE)
  {  
    if(!p_stream || p_stream->empty())
      return;
        
    while(!p_stream->empty()) {
      if(!full()) {
        GAME_EVENT ev = p_stream->get();
        if(ev.valid())
          add(ev);
      }
      else if(safe)
        assert(0);
    }
  }

  // ----------------------------------------------------
  // move all items from write stream to read stream
  // empty the write stream
  // ----------------------------------------------------  
public:  
  
  void commit(void)
  {
    read_next = write_first = write_next;
  }

  // ----------------------------------------------------
  // Initialization & creation
  // ----------------------------------------------------  
public:  
  
  void extend(int step)
  {
    if(!p_queue) {
      max_events = step;
      p_queue = new GAME_EVENT[step];
    } else {
    
      commit();
    
      int min = read_first;
      int max = read_next;
    
      GAME_EVENT *p_tmp = new GAME_EVENT[max_events+step];
    
      if(!empty(read_first, read_next)) {
        int akt,i;        
      
        for(i = 0, akt = min; akt != max; i++, akt = index_next(akt)) {
          p_tmp[i++] = p_queue[akt];
        }
      
        read_first = 0;
        write_first = write_next = read_next = max;
      } else {      
        clear();
      }
    
      delete [] p_queue;
      p_queue = p_tmp;

      max_events += step;
    }
  }

  // ----------------------------------------------------
  // Constructors
  // ----------------------------------------------------  
public:   
  
  event_stream(void) : max_events(0), p_queue(NULL)
  {    
    clear();
    extend(EXTEND_STEP);
  }

  ~event_stream(void)
  {
    max_events = 0;
    if(p_queue)
      delete [] p_queue;
    clear();
  }

} EVENT_STREAM, GAME_EVENT_QUEUE;

// Create an unique event numbers for user-defined events
typedef class event_generator {

  static int last_event;
  
public:  
  
  static int generate(void)
  {
    last_event++;
    return(last_event);
  }
  
} EVENT_GENERATOR;

#endif // __EVENTS_H__
