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

/*
 * Scene Object routines
 */

scene_object_geometry::scene_object_geometry(class scene *p_scene) 
  : object_box(p_scene)    
{
  
}

bool scene_object_geometry::visible(void)
{
  BOX *p_box = object_box_get();  
  if(p_box) {
    SCENE *p_scene = scene_get();
    if(p_scene) {
      GPIPE *p_gpipe = p_scene->gpipe_get();      
      return(p_box->visible(p_gpipe->camera_project_get()));
    }
  }  
  return(FALSE);
}

void scene_object_geometry::print(int ident)
{
  pprintf("%*cObject '%s'",ident,' ',name_get());
  if(render_hierarchy_parent_get())
    pprintf("%*cObject parent '%s'",ident,' ',render_hierarchy_parent_get()->name_get());
  
  if(render_hierarchy_child_get()) {
    ((SCENE_OBJECT_GEOMETRY *)render_hierarchy_child_get())->print(ident+2);
  }
  if(render_hierarchy_sibling_get()) {
    ((SCENE_OBJECT_GEOMETRY *)render_hierarchy_sibling_get())->print(ident);
  }
}

void scene_object_tree::print(const char *p_title)
{
  root.print(0);
}
