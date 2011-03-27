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
 * Object selection
 */
void scene_object::select(void)
{
  // Check if it's already selected
  if(is_selected())
    return;
  
  // If not, mark it as selected...
  object_select::select();
  
  // ...and add to selection list
  SCENE * p_scene = scene_get();
  if(p_scene) {
    //p_scene->selection_add(this);
  }
}
// TODO -> muze se delat i vyber materialu a podobne? -> Zatim jen pro geometricke 
// objekty protoze maji v sobe typ objektu
void scene_object::unselect(void)
{
  if(!is_selected())
    return;
  
  object_select::unselect();
  
  SCENE * p_scene = scene_get();
  if(p_scene) {
    //p_scene->selection_remove(this);
  }
}

/*
 * Print
 */
void scene_object::print(int ident)
{  
}

/*
 * Is the mesh in render hierrachy?
 */
bool scene_object::is_render_hierarchy(void)
{
  return(mark_get(MARK_RENDERER_HIERARCHY));
}

/*
 * Insert mesh to scene render hierarchy
 * mesh will be rendered after it.
 * It's base on the mesh parrent name.
 */
bool scene_object::render_hierarchy_insert(const char *p_parent_name)
{
  SCENE *p_scene = scene_get();
  if(p_scene) {
    return(p_scene->render_hierarchy_insert(this, p_parent_name));
  }
  return(FALSE);
}

bool scene_object::render_hierarchy_insert(SCENE_OBJECT *p_parent)
{  
  SCENE *p_scene = scene_get();
  if(p_scene) {
    return(p_scene->render_hierarchy_insert(this,p_parent));      
  }
  return(FALSE); 
}

/*
 * Remove mesh from render hierarchy,
 * it will not be rendered any more.
 */
bool scene_object::render_hierarchy_remove(void)
{
  SCENE *p_scene = scene_get();
  if(p_scene) {
    return(p_scene->render_hierarchy_remove(this));      
  }
  return(FALSE);
}

/*
 * Searches for object it sub-tree of this object
 */
SCENE_OBJECT * scene_object::render_hierarchy_find(const char *p_name)
{
  if(p_name) {
    OBJECT_HIERARCHY_ITERATOR it = render_hierarchy_next_new();
    SCENE_OBJECT *p_act;
    
    while((p_act = (SCENE_OBJECT *)hierarchy_next(&it))) {
      if(p_act->match_name(p_name))
        return(p_act);        
    }
  }
  return(NULL);
}
     
/*
 * Get siblings (objects on the same level)
 */
SCENE_OBJECT * scene_object::render_hierarchy_sibling_get(void)
{
  return((SCENE_OBJECT *)hierarchy_sibling_get());
}

SCENE_OBJECT * scene_object::render_hierarchy_sibling_last_get(void)
{
  return((SCENE_OBJECT *)hierarchy_sibling_last_get());
}

/*
 * Get first and last chold of this object
 * Seek between childs with render_hierarchy_sibling_get()
 */
SCENE_OBJECT * scene_object::render_hierarchy_child_get(void)
{
  return((SCENE_OBJECT *)hierarchy_child_get());
}

SCENE_OBJECT * scene_object::render_hierarchy_child_last_get(void)
{
  return((SCENE_OBJECT *)hierarchy_child_last_get());
}

/*
 * Get parent of this object   
 */
SCENE_OBJECT * scene_object::render_hierarchy_parent_get(void)
{
  return((SCENE_OBJECT *)hierarchy_parent_get());
}

OBJECT_HIERARCHY_ITERATOR scene_object::render_hierarchy_next_new(void)
{
  return(OBJECT_HIERARCHY_ITERATOR(this));
}

/*
 * Next-in-flow object
 */
SCENE_OBJECT * scene_object::render_hierarchy_next(OBJECT_HIERARCHY_ITERATOR *p_it)
{
  return((SCENE_OBJECT *)hierarchy_next(p_it));
}


bool scene_object::is_render_hierarchy_root(void)
{
  return(is_hierarchy_root());
}

void scene_object::render_hierarchy_root_set(void)
{
  return(hierarchy_root_set());
}

void scene_object::render_hierarchy_root_clear(void)
{
  return(hierarchy_root_clear());
}

/*
 * Scene Object Tree routines
 */
bool scene_object_tree::hierarchy_insert(SCENE_OBJECT *p_obj,
                                         SCENE_OBJECT *p_parent)
{
  // Is it re-insert?
  if(p_obj->mark_get(MARK_RENDERER_HIERARCHY)) {
    p_obj->hierarchy_remove();
  }
  else {
    p_obj->mark_set(MARK_RENDERER_HIERARCHY);
  }
      
  // Newly inserted object is a root
  // so add it to top of the hierarchy
  if(p_obj->is_hierarchy_root()) {
    root.hierarchy_child_add(p_obj->hierarchy_child_get());
  }
  else {
    if(p_parent == NULL) {
      root.hierarchy_child_add(p_obj);
    }
    else {
      p_parent->hierarchy_child_add(p_obj);
    }
  }
  
  return(TRUE);
}

/*
 * Scene Object Tree routines
 */
bool scene_object_tree::hierarchy_insert(SCENE_OBJECT *p_obj, const char *p_parent_name)
{      
  if(p_obj->is_hierarchy_root()) {
    hierarchy_insert(p_obj,(SCENE_OBJECT *)NULL);
  }
  else {    
    SCENE_OBJECT *p_parent = root.render_hierarchy_find(p_parent_name);
    hierarchy_insert(p_obj,p_parent);
  }
  
  return(TRUE);
}

bool scene_object_tree::hierarchy_remove(SCENE_OBJECT *p_obj)
{
  if(p_obj->mark_get(MARK_RENDERER_HIERARCHY)) {
    p_obj->hierarchy_remove();
    p_obj->mark_clear(MARK_RENDERER_HIERARCHY);
    return(FALSE);
  }
  return(TRUE);
}
