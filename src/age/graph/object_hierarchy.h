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
 
#ifndef __OBJECT_HIERARCHY_H__
#define __OBJECT_HIERARCHY_H__


typedef enum {
  
  SCENE_OBJECT_NONE = 0,
  SCENE_OBJECT_MESH,
  SCENE_OBJECT_CAMERA
  
} HIERARCHY_OBJECT_TYPE;

/*
 * Scene geometry object types
 */
typedef class object_hierarchy_type
{
  /*
   * Object types
   */
private:  
  
  HIERARCHY_OBJECT_TYPE obj_type;
  
public:

  HIERARCHY_OBJECT_TYPE object_type_get(void)
  {
    return(obj_type);
  }
  
  void object_type_set(HIERARCHY_OBJECT_TYPE type)
  {
    obj_type = type;
  }
  
public:
  
  // Check type
  bool object_is_type(HIERARCHY_OBJECT_TYPE type)
  {
    return(obj_type == type);
  }
  
  // Is it a geometry object?
  bool object_is_geometry(void)
  {
    return(obj_type == SCENE_OBJECT_MESH);
  }
     
  // Can be rendered?
  bool object_is_rendered(void)
  {
    return(obj_type == SCENE_OBJECT_MESH);
  }

public:

  object_hierarchy_type(void)
  {
    obj_type = SCENE_OBJECT_NONE;
  }
  
} OBJECT_HIERARCHY_TYPE;


/*
 * Object located in scene hierarchy
 */
CLASS_TREE_LEAF_ITERATOR(hierarchy, object_hierarchy);
typedef class hierarchy_leaf_iterator OBJECT_HIERARCHY_ITERATOR;

typedef class object_hierarchy :
  public object_hierarchy_type,
  public object_list
{

  /*
   * Basic scene tree hierarchy
   */  
  MEMBER_TREE_LEAF(hierarchy, object_hierarchy);
    
  
public:
  
  bool  is_root;
  
public:
  
  class object_hierarchy * hierarchy_sibling_get(void)
  {
    return(hierarchy_leaf_sibling_get());
  }
  
  class object_hierarchy * hierarchy_sibling_last_get(void)
  {
    return(hierarchy_leaf_sibling_last_get());
  }
  
  class object_hierarchy * hierarchy_child_get(void)
  {
    return(hierarchy_leaf_child_get());    
  }
  
  class object_hierarchy * hierarchy_child_last_get(void)
  {
    return(hierarchy_leaf_child_last_get());
  }
  
public:
    
  // Addach a new sibling pass this node
  void hierarchy_sibling_add(class object_hierarchy *p_sib)
  {
    hierarchy_leaf_sibling_add(p_sib);
  }
  
  // add child to this node  
  void hierarchy_child_add(class object_hierarchy *p_child)
  {
    hierarchy_leaf_child_add(p_child);
  }
    
public:

  // Remove this leaf from tree
  void hierarchy_remove(void)
  {
    hierarchy_leaf_remove();
  }
  
public:

  class object_hierarchy * hierarchy_parent_get(void)
  {
    return(hierarchy_leaf_parent_get());
  }
  
public:
  // TODO -> sync with tree structure?
  void hierarchy_parent_set(class object_hierarchy *p_parent)
  {
    if(p_parent != hierarchy_leaf_parent_get()) {
      hierarchy_leaf_remove();
      hierarchy_leaf_parent_set(p_parent);
    }
  }

  // Tree traversal
public:
  
  class object_hierarchy * hierarchy_next(class hierarchy_leaf_iterator *p_it)
  {
    return(hierarchy_leaf_next(p_it));
  }
    
  // Special root node handling  
public:
  
  bool is_hierarchy_root(void)
  {
    return(is_root);
  }
  
  void hierarchy_root_set(void)
  {
    hierarchy_remove();
    is_root = TRUE;
  }  
  
  void hierarchy_root_clear(void)
  {
    is_root = FALSE;
  }
    
public:
  
  /*
   * List used for rendering
   */  
  MEMBER_LIST_ITEM(render, object_hierarchy, object_render_list);
  MEMBER_LIST_ITEM_MARK(render, object_hierarchy, object_render_list);
  
public:

  object_hierarchy(SCENE *p_scene)
    : object_list(p_scene)
  {
    hierarchy_root_clear();
    
    hierarchy_leaf_init();
    render_list_item_init();    
  }  
  
} OBJECT_HIERARCHY;

/*
 * Render-list of scene objects
 */
typedef class object_render_list
  : public scene_reference
{

  MEMBER_LIST_HEAD(render, object_hierarchy, object_render_list);
  
public:  
  
  object_render_list(SCENE *p_scene)
    : scene_reference(p_scene)
  {
    render_list_head_init();
  }
    
} OBJECT_RENDER_LIST;

#endif // __OBJECT_HIERARCHY_H__
