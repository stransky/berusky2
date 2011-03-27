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

#include "age.h"

void camera_object::projection_adjust(void)
{
  SCENE *p_scene = scene_get();
  if(!p_scene)
    return;
  
  float max;
  p_scene->scene_box_depth_get(NULL,NULL,&max);
  
  if(max < DEFAULT_PROJECTION_FAR_PLANE)
    max = DEFAULT_PROJECTION_FAR_PLANE;
  
  assert(max < DEFAULT_PROJECTION_FAR_PLANE_MAX);
  projection_far_plane_set(max*2.0f);
}

void camera_object::set(void)
{
  SCENE *p_scene = scene_get();
  
  if(!p_scene) {
    PWARNING("Camera %s isn't connected to scene!",name_get());
    return;
  }

  GPIPE *p_gpipe = p_scene->gpipe_get();
  if(!p_gpipe) {
    PWARNING("Scene doesn't have any active GPIPE!");
    return;    
  }

  if(plane_auto_adjust_get()) {
    projection_adjust();
  }
  
  p_gpipe->set(this);
}

void camera_3ds::update(void)
{
  if(camera_changes.flag_get(CHANGED_CAMERA_3DS)) {
    camera_3ds_update();
    camera_changes.flag_clear(CHANGED_CAMERA_3DS);
  }
}

void camera_polar::update(void)
{ 
  if(camera_changes.flag_get(CHANGED_CAMERA_POLAR|CHANGED_CAMERA_3DS)) {
    camera_polar_update();
    camera_changes.flag_clear(CHANGED_CAMERA_POLAR|CHANGED_CAMERA_3DS);
    camera_interface::camera_invert_get()->translation_get(&pos);
  }  
}

CAMERA_OBJECT * camera_list::create_basic_object(CAMERA_TYPE type)
{ 
  CAMERA_OBJECT *p_camera = NULL;
  switch(type) {    
    case CAMERA_TYPE_BASE:
      p_camera = new CAMERA_OBJECT(scene_get());
      break;
    case CAMERA_TYPE_3DS:
      p_camera = new CAMERA_3DS(scene_get());
      break;
    case CAMERA_TYPE_POLAR:
      p_camera = new CAMERA_POLAR(scene_get());
      break;
  }
  
  // Copy viewport from scene
  SCENE *p_scene = scene_get();
  if(p_scene) {
    GPIPE *p_pipe = p_scene->gpipe_get();
    p_camera->view_set(p_pipe);
  }
  
  return(p_camera);
}
