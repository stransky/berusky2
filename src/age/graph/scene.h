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

/* Scene class, contains:

  - Geometry pipe
  
  - Scene geometry
  - Animations

  - Materials
  - Environment
    - Lights
*/

#ifndef  __SCENE_H__
#define  __SCENE_H__

class graph3d;
typedef class graph3d GRAPH3D;

typedef enum { 

  SCENE_B2M = 0,
  SCENE_3DS,
  SCENE_TXT,
  
} SCENE_TYPE;

typedef class scene :
  public object_name,
  public flag_interface
{
  friend class scene_object;
  
  
  #define SCENE_NEEDS_UPDATE_BOX             (1<<1)
  
  /*
    Frame runtime flags
    
    These flags are used for determining what changes are made
    by user and what changes need to be done before rendering.  
  */
   
  #define FRAME_NEEDS_UPDATE_ANIMATIONS      (1<<10)
  #define FRAME_NEEDS_UPDATE_HIERARCHY       (1<<11)
  #define FRAME_NEEDS_UPDATE_RENDERLIST      (1<<12)
  
private:
  
  void update_flags_clear(void)
  {
    flag_set(FRAME_NEEDS_UPDATE_ANIMATIONS);
    flag_set(FRAME_NEEDS_UPDATE_HIERARCHY);
    flag_set(FRAME_NEEDS_UPDATE_RENDERLIST);
  }

  
  /*
   * Current frame
   */
private:
  
  static int current_frame;

public:
  
  static void current_frame_set(int frame)
  {
    current_frame = frame;
    frame_change_interface::current_frame = frame;
  }
  
  static int current_frame_get(void)
  {
    return(current_frame);
  }
  
  /*
   * Animation templates
   */
private:

  ANIMATION_TEMPLATE_HEAD   anim_templates;

public:

  ANIMATION_TEMPLATE      * anim_template_create(char *p_name = NULL);
  void                      anim_template_remove(ANIMATION_TEMPLATE *p_anim);

  ANIMATION_TEMPLATE      * anim_template_get(char *p_name);
  ANIMATION_TEMPLATE      * anim_template_get(int name_ID);

  ANIMATION_TEMPLATE      * anim_template_get_first(void);
  
  /*
   * Runtime animations
   */
private:
  
  ANIMATION_RUNTIME_HEAD    anim_runtime;
  
public:

  ANIMATION_RUNTIME       * anim_create(char *p_name = NULL);
  ANIMATION_RUNTIME       * anim_remove(ANIMATION_RUNTIME *p_anim);
  
  ANIMATION_RUNTIME       * anim_get(char *p_name);
  ANIMATION_RUNTIME       * anim_get(int name_ID);

  ANIMATION_RUNTIME       * anim_run(ANIMATION_RUNTIME *p_anim);
  void                      anim_stop(ANIMATION_RUNTIME *p_anim);


  ANIMATION_RUNTIME       * anim_get_first(void);
  
private:
  
  /*
   * Textures in the scene
   */
  MATERIAL_TEXT_LIST textures;
  
public:

  MATERIAL_TEXT * texture_create(const char *p_name);
  MATERIAL_TEXT * texture_create(const char *p_dir, const char *p_file, const char *p_name = NULL);
  void            texture_remove(MATERIAL_TEXT *p_mat);

  MATERIAL_TEXT * texture_get(char *p_name);
  MATERIAL_TEXT * texture_get(int name_ID);

  MATERIAL_TEXT * texture_get_first(void);  
  void            texture_reload(void);

  void            texture_dir_reset(void);
  bool            texture_dir_add(char *p_dir);

private:  
  
  /*
   * Materials in the scene
   */
  MATERIAL_LIST   materials;

public:

  MESH_MATERIAL * material_create(const char *p_name = NULL);
  void            material_remove(MESH_MATERIAL *p_mat);

  MESH_MATERIAL * material_get(const char *p_name);
  MESH_MATERIAL * material_get(int name_ID);

  MESH_MATERIAL * material_get_first(void);

public:

  MESH_MATERIAL * material_load(char *p_dir, char *p_file);
  MESH_MATERIAL * material_load(FHANDLE *f);

public:

  int             materials_load(char *p_dir, char *p_file);
  int             materials_load(FHANDLE *f);

  /*
   * Meshes in the scene
   */
private:
  
  GAME_MESH_LIST  meshes;
  
public:  

  GAME_MESH     * mesh_create(const char *p_name = NULL, const char *p_mesh_parent = NULL);
  void            mesh_remove(GAME_MESH *p_mesh);

  GAME_MESH     * mesh_get(const char *p_name);
  GAME_MESH     * mesh_get(int name_ID);

  GAME_MESH     * mesh_get_first(void);
  
  GAME_MESH     * mesh_mark_first(int mark);
  
public:  

  GAME_MESH     * mesh_load(const char *p_dir, const char *p_file);
  GAME_MESH     * mesh_load(FHANDLE *f);

public:  

  int             meshes_load(const char *p_dir, const char *p_file);
  int             meshes_load(FHANDLE *f);

  /*
   * Graphics pipeline
   */
private:
  
  GPIPE           gpipeline;

public:

  void gpipe_set(void)
  {
    ::gpipe_set(&gpipeline);
  }

  GPIPE * gpipe_get(void)
  {
    return(&gpipeline);
  }

  /*
   * Scene camera system
   */
private:

  CAMERA_LIST     cameras;

public:
  
  CAMERA_OBJECT * camera_create(const char *p_name = NULL);
  CAMERA_3DS    * camera_3ds_create(const char *p_name = NULL);
  CAMERA_POLAR  * camera_polar_create(const char *p_name = NULL);
  
  CAMERA_OBJECT * camera_get(const char *p_name);

  void            camera_active_set(CAMERA_OBJECT *p_camera);
  bool            camera_active_set(const char *p_name);
  CAMERA_OBJECT * camera_active_get(void);

  /*
   * Scene render hierarchy
   */
private:

  SCENE_OBJECT_TREE   render_hierarchy;

  /*
   * Those methods are private - should be called via. objects like
   * object->render_hierarchy_insert(...)
   */

private:

  /*
   * Insert a new object into scene hierarchy
   */
  bool render_hierarchy_insert(SCENE_OBJECT *p_obj, const char *p_parent_name = NULL)
  {
    // New mobject in scene....we need to update the scene box
    flag_set(SCENE_NEEDS_UPDATE_BOX);
    
    return(render_hierarchy.hierarchy_insert(p_obj, p_parent_name));
  }
  
  bool render_hierarchy_insert(SCENE_OBJECT *p_obj, SCENE_OBJECT *p_parent)
  {
    // New mobject in scene....we need to update the scene box
    flag_set(SCENE_NEEDS_UPDATE_BOX);
    
    return(render_hierarchy.hierarchy_insert(p_obj, p_parent));
  }
  
  /*
   * Remove already placed object from scene hierarchy
   */
  bool render_hierarchy_remove(SCENE_OBJECT *p_obj)
  {
    // Removed object from scene....we need to update the scene box
    flag_set(SCENE_NEEDS_UPDATE_BOX);
    
    return(render_hierarchy.hierarchy_remove(p_obj));
  }
    
  /*
   * Find object in scene hierarchy
   */
  SCENE_OBJECT * render_hierarchy_find(char *p_name)
  {
    return(render_hierarchy.hierarchy_find(p_name));
  }

public:  
  
  /*
   * Iterate over scene hierarchy
   * if p_start == NULL start from root
   */
  OBJECT_HIERARCHY_ITERATOR render_hierarchy_next_new(SCENE_OBJECT *p_start = NULL)
  {
    return(render_hierarchy.hierarchy_next_new(p_start));
  }
  
  SCENE_OBJECT * render_hierarchy_next(OBJECT_HIERARCHY_ITERATOR *p_it)
  {
    return(render_hierarchy.hierarchy_next(p_it));
  }

  /*
   * Render list
   */  
private:

  OBJECT_RENDER_LIST render_list;

private:

  void  render_list_compose(void);

public:
  
  SCENE_OBJECT * render_list_first(void)
  {    
    return((SCENE_OBJECT *)render_list.render_list_get_first());
  }
  
  SCENE_OBJECT * render_list_next(SCENE_OBJECT *p_item)
  {    
    return(p_item ? (SCENE_OBJECT *)(p_item->render_list_next()) : NULL);
  }

  /*
   * Scene bounding box
   */
private:

  AABB  scene_box;

private:

  void  scene_box_update(void);

public:
  
  VECT *scene_box_center_get(VECT *p_center);  // Return center of the scene
  VECT *scene_box_length_get(VECT *p_size);    // Return size of the scene
  float scene_box_depth_get(void);             // Return current depth of the scene
  void  scene_box_depth_get(float *p_averange, float *p_min, float *p_max);  
    
  /*
   * Scene main routines
   */
public:

  void create(void);
  void clear(void);
  void remove(void);

private:

  void load_insert(void);

private:

  int  load_b2m(char *p_dir, char *p_file);
  int  save_b2m(char *p_dir, char *p_file);

  int  load_3ds(char *p_dir, char *p_file);
  int  save_3ds(char *p_dir, char *p_file);

  int  load_txt(char *p_dir, char *p_file);
  int  save_txt(char *p_dir, char *p_file);

public:
  
  /* 
   * Load whole scene from file
   */
  int  load(char *p_dir, char *p_file, SCENE_TYPE type = SCENE_B2M);
  int  load(FHANDLE *f, SCENE_TYPE type = SCENE_B2M);

  /* 
   * Save scene to file
   */
  int  save(char *p_dir, char *p_file, SCENE_TYPE type = SCENE_B2M);
  int  save(FHANDLE *f, SCENE_TYPE type = SCENE_B2M);

public:
  
  /* 
   * Merge give scene with this one and release it
   */
  bool merge(class scene *p_merged);
    
  
  /*
   * Update scene parts
   */
public:
  
  void update_animations(bool forced = FALSE);
  void update_hierarchy(bool forced = FALSE);
  void update_renderlist(bool forced = FALSE);
  void update_box(bool forced = FALSE);

  /*
   * Update all scene animations, calc positions and so on
   */
public:
  
  void update(void);
  
public:
  
  void draw_start(int current_frame);
  void draw_end(bool all = FALSE);

public:
  
  void render_viewport_absolute_set(tpos sx, tpos sy, tpos dx, tpos dy);
  void render_viewport_relative_set(float sx, float sy, float dx, float dy);
  void render_viewport_get(tpos *p_sx, tpos *p_sy, tpos *p_dx, tpos *p_dy);
  bool render_state_set(bool state);
  bool render_state_get(void);

public:

  void lighting_on(void);
  void lighting_off(void);
  void lighting_set(bool state);

  /* 
   * Selection interface
   */
private:
  
  SELECTED_OBJECT_HEAD selection;

private:
  
  bool selection_add(OBJECT_SELECT * p_object);
  void selection_remove(OBJECT_SELECT * p_object);

public:

  void selection_clear(void);

public:
  
  SELECTED_OBJECT_ITEM * selection_get(void);

public:

  int  selection_add(RECT2DI *p_r);
  int  selection_add_meshes(RECT2DI *p_r);

  /* 
   * Graphics interface
   */
private:
  
  GRAPH3D *p_graph;

public:  
  
  GRAPH3D * graph_get(void)
  {
    return(p_graph);
  }
  
  void graph_set(GRAPH3D *p_graph)
  {
    this->p_graph = p_graph;
  }
  
public:

  scene(void);
  ~scene(void);

} SCENE;

#endif // __SCENE_H__
