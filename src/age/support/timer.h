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
 * A timer support
 */

#ifndef __TIMER_H__
#define __TIMER_H__

inline Uint32 current_time_get(void)
{
  return(SDL_GetTicks());
}

typedef class timer {
  
  // Updated by timer routines
  Uint32 frame_time_start;
  Uint32 frame_time_end;  
  Uint32 frame_time_length_last;
  
  // Set by user
  Uint32 frame_time_delay;
  
public:
    
  void fps_set(float fps)
  {
    frame_time_delay = (Uint32)(1000.0f / fps);
  }
  
  float fps_get(void)
  {
    return(1000.0f / (float)frame_time_delay);
  }
  
  void frame_length_set(Uint32 frame_length)
  {
    frame_time_delay = frame_length;
  }
  
  Uint32 frame_length_get(void)
  {
    return(frame_time_delay);
  }
  
public:
  
  float current_fps_get(void)
  {
    return(1000.0f / (float)frame_time_length_last);
  }
  
  Uint32 current_frame_length_get(void)
  {
    return(frame_time_length_last);
  }
      
  void delay(int msecs)
  {
    SDL_Delay((Uint32)msecs);
  }

public:
    
  void frame_start(void)
  {
    frame_time_start = SDL_GetTicks();
    frame_time_end = frame_time_start + frame_time_delay;
  }
  
  void frame_end(bool wait)
  {
    Uint32 now;
    
    // Wait until the end of this frame
    now = SDL_GetTicks();
    if(now < frame_time_end && wait)
      SDL_Delay(frame_time_end - now);
    
    // Calculate a length of this frame
    now = SDL_GetTicks();
    frame_time_length_last = now - frame_time_start;
  }
    
public:
  
  timer(void)
  {
    frame_time_start = 0;
    frame_time_end = 0;    
    frame_time_delay = 50;
    frame_time_length_last = 50;
  } 
  
} TIMER;

#endif // __TIMER_H__
