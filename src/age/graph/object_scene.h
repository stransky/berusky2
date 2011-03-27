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
  Basic scene geometry object
*/
class scene_object;
typedef class scene_object SCENE_OBJECT;

typedef class scene_object :
  public object_hierarchy
{

public:
  
  void print(int ident = 0);

  /*
   * Selection helpers
   */
public:
  
  virtual void select(void);
  virtual void unselect(void);

  /*
   * Scene hierarchy for position/rendering
   */
public:

  /*
   * Is the mesh in render hierrachy?
   */
  bool is_render_hierarchy(void);

  /*
   * Insert/Reparent mesh to scene render hierarchy
   * mesh will be rendered after it.
   * It's base on the mesh parrent name.
   */
  bool render_hierarchy_insert(const char *p_parent_name);
  bool render_hierarchy_insert(SCENE_OBJECT *p_parent);
  
  /*
   * Remove mesh from render hierarchy,
   * it will not be rendered any more.
   */
  bool render_hierarchy_remove(void);

  /*
   * Searches for object it sub-tree of this object
   */
  SCENE_OBJECT * render_hierarchy_find(const char *p_name);
     
  /*
   * Get siblings (objects on the same level)
   */
  SCENE_OBJECT * render_hierarchy_sibling_get(void);
  SCENE_OBJECT * render_hierarchy_sibling_last_get(void);

  /*
   * Get first and last chold of this object
   * Seek between childs with render_hierarchy_sibling_get()
   */
  SCENE_OBJECT * render_hierarchy_child_get(void);
  SCENE_OBJECT * render_hierarchy_child_last_get(void);

  /*
   * Get parent of this object   
   */
  SCENE_OBJECT * render_hierarchy_parent_get(void);

  /*
   * Next-in-flow object
   */
  OBJECT_HIERARCHY_ITERATOR render_hierarchy_next_new(void);
  SCENE_OBJECT * render_hierarchy_next(OBJECT_HIERARCHY_ITERATOR *p_it);

public:

  bool is_render_hierarchy_root(void);

private:

  void render_hierarchy_root_set(void);
  void render_hierarchy_root_clear(void);

public:
  
  scene_object(SCENE *p_scene)
    : object_hierarchy(p_scene)
  {
    
  }

} SCENE_OBJECT;

/*
 * Basic list of scene objects
 */
typedef class scene_object_list :
  public object_list_head
{
    
public:  
  
  scene_object_list(SCENE *p_scene)
    : object_list_head(p_scene)
  {
    
  }
  
} SCENE_OBJECT_LIST;

/*
 * Hierarchy tree of scene objects
 */
#define ROOT_NAME   "###ROOT###"

typedef class scene_object_tree :  
  public scene_reference
{
  /* Special "root" scene object - root of all objects
    i.e.: super-root :)
  */
  SCENE_OBJECT root;
  
public:

  SCENE_OBJECT * get_root(void)
  {
    return(&root);
  }

  bool hierarchy_insert(SCENE_OBJECT *p_obj, SCENE_OBJECT *p_parent);
  bool hierarchy_insert(SCENE_OBJECT *p_obj, const char *p_parent_name = NULL);
  bool hierarchy_remove(SCENE_OBJECT *p_obj);
  
public:  
  
  SCENE_OBJECT * hierarchy_find(const char *p_name)
  {    
    return(root.render_hierarchy_find(p_name));
  }
  
  SCENE_OBJECT * hierarchy_find_child(const char *p_name)
  { 
    SCENE_OBJECT *p_obj = root.render_hierarchy_find(p_name);
    if(p_obj) {
      return((SCENE_OBJECT *)p_obj->render_hierarchy_child_get());
    }
    return(NULL);
  }
  
  SCENE_OBJECT * hierarchy_find_parent(const char *p_name)
  {    
    SCENE_OBJECT *p_obj = root.render_hierarchy_find(p_name);
    if(p_obj) {
      return((SCENE_OBJECT *)p_obj->render_hierarchy_parent_get());
    }
    return(NULL);
  }
  
public:

  SCENE_OBJECT * hierarchy_next(OBJECT_HIERARCHY_ITERATOR *p_it)
  {
    return((SCENE_OBJECT *)root.render_hierarchy_next(p_it));
  }
  
  OBJECT_HIERARCHY_ITERATOR hierarchy_next_new(SCENE_OBJECT *p_start = NULL)
  {
    if(!p_start) {
      p_start = root.render_hierarchy_child_get();
      if(!p_start) {
        return(OBJECT_HIERARCHY_ITERATOR(NULL));
      }
    }
    return(p_start->render_hierarchy_next_new());
  }
  
public:
  
  void print(const char *p_title = NULL);
  
public:

  scene_object_tree(SCENE *p_scene)
    : scene_reference(p_scene),
      root(p_scene)
  {    
    root.name_set(ROOT_NAME);
    root.hierarchy_root_set();
  }

} SCENE_OBJECT_TREE;
