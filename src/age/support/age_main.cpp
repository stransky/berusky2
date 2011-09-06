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
 * Loop management
 */

#include "age.h"

// -------------------------------------------------------
//   AGE config
// -------------------------------------------------------

AGE_CONFIG age_main::config;

void age_main::config_scene_selection(bool state)
{
  config.scene_selection = state;
}

bool age_main::draw(void)
{
  ltimer.frame_start();
  
  // Propagate correct frame to system
  if(p_graph) {
    p_graph->current_frame_set(frame);
    p_graph->current_fps_set(ltimer.fps_get());
  }
  
  // Draw frame
  graph_draw();
  
  // Increment frame counter
  frame++;
  if(frame < 0)
    frame = 0;
  
  return(TRUE);
}

age_main::age_main(AGE_EVENT_FUNC p_callback)  
{
  p_graph = NULL;
  p_scene = NULL;
  frame = 0;
  
  srand(current_time_get());
  callback_set(p_callback);  
}

age_main::~age_main(void)
{
  if(p_graph)
    delete p_graph;
}
