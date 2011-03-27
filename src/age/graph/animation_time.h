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
  Time 
*/

#ifndef __ANIMATION_TIME_H__
#define __ANIMATION_TIME_H__

typedef enum {
  
  ANIMATION_CREATED = 0,
  ANIMATION_STOPPED,
  ANIMATION_RUNNING  
  
} ANIMATION_STATE;

typedef class animation_track_time {
  
  ANIMATION_STATE state;

public:

  bool running(void)
  {
    return(state == ANIMATION_RUNNING);
  }
  
  bool stopped(void)
  {
    return(state == ANIMATION_STOPPED);
  }
  
  ANIMATION_STATE anim_state_get(void)
  {
    return(state);
  }

private:

  int             animation_length;
  bool            looped;

  int             time_run;        // current time in animation (0...length)
  int             time_lenght;     // animation length
  int             time_start;      // skipped time from start (default is 0)
  int             time_stop;       // time when the animation stops (default is length)
  
  int             system_time_start;  // start time in system time
  int             system_time_stop;   // stop time in system time

public:
  
  void anim_start(int system_time_next, int anim_start_time = 0, int anim_stop_time = 0)
  {
    time_start     = anim_start_time;
    time_stop      = anim_stop_time;
    
    time_lenght = !time_stop ? animation_length : time_stop;
    system_time_start = system_time_next - time_start;
    system_time_stop  = system_time_start + time_lenght;
    
    time_run = 0;
    
    state = ANIMATION_RUNNING;
  }
  
  void anim_loop(int system_time_next)
  {
    system_time_start = system_time_stop - time_start;
    time_run          = system_time_next - system_time_start;
    
    if(time_run > time_lenght) {
      time_run = time_run % time_lenght;
      // TODO - correction
    } 
    
    system_time_stop  = system_time_start + time_lenght;
  }
    
  void anim_stop(void)
  {
    state = ANIMATION_STOPPED;
  }  
  
  void update(int system_time_next)
  {    
    if(state == ANIMATION_RUNNING) {
      bool end = system_time_next > system_time_stop;      
      if(end) {
        if(looped) {
          anim_loop(system_time_next);
        } else {
          time_run = time_lenght;          
          // Stop animation
        }
      } else {
        time_run = system_time_next - system_time_start;
      }
    }
  }  

public:

  int run_time_get(void)
  {
    return(time_run);
  }
  
public:

  void length_set(int animation_time_lenght)
  {
    assert(stopped());
    animation_length = animation_time_lenght;
    
    time_run = 0;
    time_start = 0;
    time_stop = 0;
    
    system_time_start = 0;
    system_time_stop = 0;
  }
  
  int length_get(void)
  {
    return(time_lenght);
  }
  
public:

  animation_track_time(int animation_time_lenght = 0, bool loop = FALSE)
    : state(ANIMATION_CREATED)
  {    
    length_set(animation_time_lenght);
    looped = loop;
  }
  
  animation_track_time(ANIM_TRACK *p_track)
    : state(ANIMATION_CREATED)
  {
    length_set(p_track->length_get());
    looped = p_track->looped();
  }
  
} ANIMATION_TRACK_TIME;

/*
int calc_time_akt(int next_time, int time_start)
{
 return(next_time-time_start);
}

int calc_endtime(int framenum)
{
 return((framenum-1)*SIM_KONSTI);
}

int calc_keynum(int endtime)
{
 return((endtime/SIM_KONSTI)+1);
}

int calc_framenum(int endtime)
{
 return((endtime/SIM_KONSTI)+1);
}

int endtime2framenum(int endtime)
{
 return((endtime/SIM_KONSTI)+1);
}

int time2frame(int time)
{
 return(time/SIM_KONSTI);
}

int frame2time(int frame)
{
  return(frame*SIM_KONSTI);
}

float frame2sec(int frame)
{
  return(frame*0.05f);
}

float time2sec(int time)
{
  return(time/1000.0f);
}
*/

#endif //  __ANIMATION_TIME_H__
