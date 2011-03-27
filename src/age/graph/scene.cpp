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
 * Current scene frame
 */
int scene::current_frame = -1;

/*
 * Mesh methods
 */
GAME_MESH * scene::mesh_create(const char *p_name, const char *p_mesh_parent)
{
  GAME_MESH *p_mesh = meshes.create();
  
  // Set scene reference
  p_mesh->scene_set(this);
  if(p_name)
    p_mesh->name_set(p_name);  
  
  // Insert to render hierarchy
  render_hierarchy_insert(p_mesh, p_mesh_parent);
    
  return(p_mesh);
}

void scene::mesh_remove(GAME_MESH *p_mesh)
{
  // Remove this mesh
  
  // Check the reference count
  assert(p_mesh->reference_get() == 0);  
  
  // Remove this mesh from render hierrachy
  render_hierarchy_remove(p_mesh);
    
  // Remove this mesh
  meshes.remove(p_mesh);
}

GAME_MESH * scene::mesh_get(const char *p_name)
{
  return((GAME_MESH *)meshes.object_list_find(p_name));
}

GAME_MESH * scene::mesh_get(int name_ID)
{
  //return(meshes.find(name_ID));
  return(NULL);
}

GAME_MESH * scene::mesh_get_first(void)
{
  return(meshes.get_first());
}

GAME_MESH * scene::mesh_mark_first(int mark)
{
  return((GAME_MESH *)meshes.object_list_first_mark(mark));
}

/*
 *  Texture methods
 */
MATERIAL_TEXT * scene::texture_create(const char *p_name)
{
  MATERIAL_TEXT *p_text = textures.create();
  p_text->name_set(p_name);
  return(p_text);
}

MATERIAL_TEXT * scene::texture_create(const char *p_dir, const char *p_file, const char *p_name)
{
  MATERIAL_TEXT *p_text = textures.create_and_load(p_dir, p_file, p_name);
  return(p_text);
}

void scene::texture_remove(MATERIAL_TEXT *p_text)
{

}

MATERIAL_TEXT * scene::texture_get(char *p_name)
{
  return((MATERIAL_TEXT *)textures.object_list_find(p_name));
}

MATERIAL_TEXT * scene::texture_get(int name_ID)
{
  return(NULL);
}

MATERIAL_TEXT * scene::texture_get_first(void)
{
  return(textures.get_first());
}

void scene::texture_reload(void)
{
  textures.reload();
}

void scene::texture_dir_reset(void)
{
  textures.dir_reset();
}

bool scene::texture_dir_add(char *p_dir)
{
  return(textures.dir_add(p_dir));
}

/*
 *  Material methods
 */
MESH_MATERIAL * scene::material_create(const char *p_name)
{
  MESH_MATERIAL *p_mat = materials.create();
  p_mat->scene_set(this);
  if(p_name)
    p_mat->name_set(p_name);
  return(p_mat);  
}

void scene::material_remove(MESH_MATERIAL *p_mat)
{

}

MESH_MATERIAL * scene::material_get(const char *p_name)
{
  return((MESH_MATERIAL *)materials.object_list_find(p_name));
}

MESH_MATERIAL * scene::material_get(int name_ID)
{
  //return(materials.find(name_ID));  
  return(NULL);
}

MESH_MATERIAL * scene::material_get_first(void)
{
  return((MESH_MATERIAL *)materials.object_list_get_first());
}

/*
 *  Camera system
 */
CAMERA_OBJECT * scene::camera_create(const char *p_name)
{
  CAMERA_OBJECT *p_cam = cameras.create(CAMERA_TYPE_BASE);
  if(p_cam)
    p_cam->name_set(p_name);
  return(p_cam);
}

CAMERA_3DS * scene::camera_3ds_create(const char *p_name)
{
  CAMERA_3DS *p_cam = (CAMERA_3DS *)cameras.create(CAMERA_TYPE_3DS);
  if(p_cam)
    p_cam->name_set(p_name);
  return(p_cam);
}

CAMERA_POLAR * scene::camera_polar_create(const char *p_name)
{
  CAMERA_POLAR *p_cam = (CAMERA_POLAR *)cameras.create(CAMERA_TYPE_POLAR);
  if(p_cam)
    p_cam->name_set(p_name);
  return(p_cam);
}

CAMERA_OBJECT * scene::camera_get(const char *p_name)
{
  return((CAMERA_OBJECT *)cameras.object_list_find(p_name));
}

void scene::camera_active_set(CAMERA_OBJECT *p_camera)
{
  cameras.active_set(p_camera);
}

CAMERA_OBJECT * scene::camera_active_get(void)
{
  return(cameras.active_get());
}

/*
 * Scene hierarchy
 */

/*
 * Scene methods
 */
void scene::create(void)
{  
  
}

void scene::clear(void)
{  
  
}

void scene::remove(void)
{
  
}

scene::scene(void)
  : textures(this), 
    materials(this), 
    meshes(this), 
    cameras(this), 
    render_hierarchy(this),
    render_list(this)
{  
  ::gpipe_set(&gpipeline);

  scene_box_update();
}

scene::~scene(void)
{
  
}

void scene::render_viewport_absolute_set(tpos sx, tpos sy, tpos dx, tpos dy)
{
  gpipe_get()->view_absolute_set(sx,sy,dx,dy);
}
void scene::render_viewport_relative_set(float sx, float sy, float dx, float dy)
{
  gpipe_get()->view_relative_set(sx,sy,dx,dy);
}
void scene::render_viewport_get(tpos *p_sx, tpos *p_sy, tpos *p_dx, tpos *p_dy)
{
  gpipe_get()->view_get(p_sx, p_sy, p_dx, p_dy);
}
bool scene::render_state_set(bool state)
{
  return(gpipe_get()->render_state_set(state));
}
bool scene::render_state_get(void)
{
  return(gpipe_get()->render_state_get());
}

/*
 * Load / Save
 */
void scene::load_insert(void)
{
  // Add all loaded meshes into scene hierrachy
  GAME_MESH *p_tmp = mesh_mark_first(MARK_CREATED);
  while(p_tmp) {
  
    p_tmp->mark_clear(MARK_CREATED);    
    MESH_GEOMETRY *p_geom = p_tmp->lock(LOCK_READ);
    if(p_geom) {
      p_geom->update(TRUE);
    }
    p_tmp->unlock();
    
    p_tmp = (GAME_MESH *)p_tmp->object_list_next_mark(MARK_CREATED);
  }
  
  // Recalculate complete hierarchy
  update_hierarchy(TRUE);
}

/*
 * Load whole scene from file
 */
int scene::load_b2m(char *p_dir, char *p_file)
{
  return(scene_import_b2m(this, p_dir, p_file));
}
int scene::save_b2m(char *p_dir, char *p_file)
{
  return(scene_export_b2m(this, p_dir, p_file));
}

int scene::load_3ds(char *p_dir, char *p_file)
{
  return(scene_import_3ds(this, p_dir, p_file));
}
int scene::save_3ds(char *p_dir, char *p_file)
{
  return(scene_export_3ds(this, p_dir, p_file));
}

int scene::load_txt(char *p_dir, char *p_file)
{
  return(scene_import_txt(this, p_dir, p_file));
}
int scene::save_txt(char *p_dir, char *p_file)
{
  return(scene_export_txt(this, p_dir, p_file));
}

int scene::load(char *p_dir, char *p_file, SCENE_TYPE type)
{
  int ret = 0;
  
  switch(type) {
    case SCENE_B2M:
      ret = load_b2m(p_dir,p_file);
      break;
    case SCENE_3DS:
      ret = load_3ds(p_dir,p_file);
      break;
    case SCENE_TXT:
      ret = load_txt(p_dir,p_file);
      break;
  }
  
  load_insert();
  
  return(ret);
}

/*
 * Save scene to file
 */
int scene::save(char *p_dir, char *p_file, SCENE_TYPE type)
{
  int ret = 0;
  
  switch(type) {
    case SCENE_B2M:
      ret = save_b2m(p_dir,p_file);
      break;
    case SCENE_3DS:
      ret = save_3ds(p_dir,p_file);
      break;
    case SCENE_TXT:
      ret = save_txt(p_dir,p_file);
      break;
  }
  
  return(ret);
}

/*
SCENE_OBJECT_HIERARCHY_ITERATOR it = hierarchy_next_new();
SCENE_OBJECT *p_obj;

while((p_obj = hierarchy_next(&it))) {
  p_obj->geometry_update_reset();
}
*/

void scene::update_animations(bool forced)
{
  if(!forced && !flag_get(FRAME_NEEDS_UPDATE_ANIMATIONS))
    return;
  
  
  flag_clear(FRAME_NEEDS_UPDATE_ANIMATIONS);  
}

void scene::update_hierarchy(bool forced)
{
  if(!forced && !flag_get(FRAME_NEEDS_UPDATE_HIERARCHY))
    return;
  
  SCENE_OBJECT *p_obj;
  
  // Update all positions of local objects
  OBJECT_HIERARCHY_ITERATOR it = render_hierarchy_next_new();
  while((p_obj = render_hierarchy_next(&it))) {
    if(p_obj->object_is_geometry()) {
      ((SCENE_OBJECT_GEOMETRY *)p_obj)->object_world_hierarchy_update(forced);
    }
  }

  // Update complete hierarchy
  int  changed = 0;
  it = render_hierarchy_next_new();
  while((p_obj = render_hierarchy_next(&it))) {
    if(p_obj->object_is_geometry()) {
      changed += ((SCENE_OBJECT_GEOMETRY *)p_obj)->object_box_hierarchy_update(forced);
    }
  }
  
  flag_clear(FRAME_NEEDS_UPDATE_HIERARCHY);
  
  if(changed) {
    flag_set(SCENE_NEEDS_UPDATE_BOX);
  }
}

void scene::update_renderlist(bool forced)
{
  if(!forced && !flag_get(FRAME_NEEDS_UPDATE_RENDERLIST))
    return;

  int rendered = 0;
  
  render_list.render_list_head_init();
  
  OBJECT_HIERARCHY_ITERATOR it = render_hierarchy_next_new();
  SCENE_OBJECT *p_obj;
  
  while((p_obj = render_hierarchy_next(&it))) {
    if(p_obj->object_is_rendered()) {      
      if(((SCENE_OBJECT_GEOMETRY *)p_obj)->visible()) {
        render_list.render_list_insert(p_obj);
        rendered++;
      }
    }
  }  
  
  flag_clear(FRAME_NEEDS_UPDATE_RENDERLIST);
}

void scene::update_box(bool forced)
{
  if(!forced && !flag_get(SCENE_NEEDS_UPDATE_BOX))
    return;
  scene_box_update();  
  flag_clear(SCENE_NEEDS_UPDATE_BOX);
}

/*
 * Update all scene animations, calc positions and so on
 */
void scene::update(void)
{
  update_animations();
  update_hierarchy();
  update_renderlist();
  update_box();
}

#define DEFAULT_SCENE_BOX_SIZE  10

void scene::scene_box_update(void)
{ 
  scene_box.set();
  
  OBJECT_HIERARCHY_ITERATOR it = render_hierarchy_next_new();
  SCENE_OBJECT_GEOMETRY *p_obj;
  int objnum = 0;
  while((p_obj = (SCENE_OBJECT_GEOMETRY *)render_hierarchy_next(&it))) {
    if(p_obj->object_is_geometry()) {
      BOX *p_box = p_obj->object_box_global_get();
      if(p_box) {   
        AABB tmp(p_box);
        scene_box.adjust(&tmp);
        objnum++;
      }
    }
  }
  
  if(!objnum) {
    scene_box.adjust(VECT(-DEFAULT_SCENE_BOX_SIZE,
                          -DEFAULT_SCENE_BOX_SIZE,
                          -DEFAULT_SCENE_BOX_SIZE));
    scene_box.adjust(VECT(DEFAULT_SCENE_BOX_SIZE, 
                          DEFAULT_SCENE_BOX_SIZE,
                          DEFAULT_SCENE_BOX_SIZE));
  }
}

// Return center of the scene
VECT * scene::scene_box_center_get(VECT *p_center)
{
  return(scene_box.center_get(p_center));
}

// Return size of the scene
VECT * scene::scene_box_length_get(VECT *p_size)
{
  return(scene_box.length_get(p_size));
}

// Return current depth of the scene
void scene::scene_box_depth_get(float *p_averange, float *p_min, float *p_max)
{
  // Project scene box to 2D and calculate the averange distance
  int border_vertexnum = scene_box.border_vertexnum_get();
  
  assert(border_vertexnum == AABB_BORDER_VERTEXNUM);
  
  VECT border[AABB_BORDER_VERTEXNUM];
  
  scene_box.border_get(border);
  
  GPIPE *p_gpipe = gpipe_get();
  
  int i;
  int num = 0;
  float dist_averange = 0;
  float dist_min = FLT_MAX;
  float dist_max = FLT_MIN;

  for(i = 0; i < AABB_BORDER_VERTEXNUM; i++) {
    p_gpipe->world_to_camera(border+i);
    if(border[i].z > 0) {      
      dist_min = MIN(border[i].z, dist_min);
      dist_max = MAX(border[i].z, dist_max);
      dist_averange += border[i].z;
      num++;
    }
  }
  
  if(num == 0) {
    dist_averange = 10;
    dist_min = -5;
    dist_max = 5;
  }
  else {
    dist_averange /= (float)num;
  }

  if(p_averange)
    *p_averange = dist_averange;
  if(p_min)
    *p_min = dist_min;
  if(p_max)
    *p_max = dist_max;
}

float scene::scene_box_depth_get(void)
{
  float dist;
  scene_box_depth_get(&dist,NULL,NULL);
  return(dist);
}

void scene::draw_start(int current_frame)
{
  current_frame_set(current_frame);
  update_flags_clear();
}

/*
 * Drawing
 */
void scene::draw_end(bool all)
{ 
  /*
   * Update all scene objects
   */  
  update();
  
  
  /*
   * Set the current camera as the active one
   */
  CAMERA_OBJECT *p_camera = camera_active_get();
  if(p_camera)
    p_camera->set();
  
  
  /*
   * Draw all objects in the scene
   */    
  if(all) {
    OBJECT_HIERARCHY_ITERATOR it = render_hierarchy_next_new();
    SCENE_OBJECT *p_obj;
    while((p_obj = render_hierarchy_next(&it))) {
      if(p_obj->object_type_get() == SCENE_OBJECT_MESH) {
        GAME_MESH *p_mesh = (GAME_MESH *)p_obj;
        p_mesh->draw();
      }
    }    
  }
  
  /*
   * Draw only visible objects
   */
  else {
    SCENE_OBJECT *p_obj = render_list_first();
    while(p_obj) {
      if(p_obj->object_is_type(SCENE_OBJECT_MESH)) {
        GAME_MESH *p_mesh = (GAME_MESH *)p_obj;
        p_mesh->draw();
      }
      p_obj = render_list_next(p_obj);
    }
  }
  
  /*
   * Reset scene geometry
   */
}

/*
 * Lighting interface
 */
void scene::lighting_on(void)
{
  
}

void scene::lighting_off(void)
{
  
}

void scene::lighting_set(bool state)
{
  
}

/*
 * Selection interface
 */
void scene::selection_clear(void)
{
  selection.clear();
}

void scene::selection_remove(OBJECT_SELECT *p_object)
{
  selection.remove(p_object);
}

bool scene::selection_add(OBJECT_SELECT * p_object)
{  
  if(selection.insert(p_object)) {
    return(TRUE);
  }
  return(FALSE);
}

SELECTED_OBJECT_ITEM  * scene::selection_get(void)
{
  return(selection.get_first());
}

/*
 * Transform all objects to 2D and intersect them
 */
int scene::selection_add(RECT2DI *p_r)
{
  return(selection_add_meshes(p_r));
}

int scene::selection_add_meshes(RECT2DI *p_r)
{  
  // Select meshes
  int inserted = 0;
  
  GAME_MESH *p_mesh = mesh_get_first();
  while(p_mesh) {
    
    int vertexnum;
    VECT2DI *p_vertexes = p_mesh->vertex_position_screen_space_get(&vertexnum);
    
    if(p_vertexes) {
          
      RECT2DI mesh_rectangle;
      mesh_rectangle.set(p_vertexes,vertexnum);
      
      int ret = intersect_rect_rect(p_r, &mesh_rectangle);
      if(ret == INTERSECT_INSIDE) {
        selection_add(p_mesh);
      }
      else if(ret == INTERSECT_CROSS) {
        int facenum;      
        tface *p_faces = p_mesh->faces_get(&facenum);
        assert(p_faces);
        
        int i;
        for(i = 0; i < facenum; i += 3) {
          
          VECT2DI *p_v1 = p_vertexes+(*p_faces)++;
          VECT2DI *p_v2 = p_vertexes+(*p_faces)++;
          VECT2DI *p_v3 = p_vertexes+(*p_faces)++;
          
          if(p_v1->x == INT_MAX || p_v2->x == INT_MAX || p_v3->x == INT_MAX) {
            continue;
          }
          
          if(intersect_rect_triangle(p_r,p_v1,p_v2,p_v3)) {
            if(selection_add(p_mesh))
              inserted++;
            break;
          }
        }
        
        free(p_faces);
      }
      free(p_vertexes);
    }
    
    p_mesh = (GAME_MESH *)p_mesh->object_list_next();
  }
  
  return(inserted);
}
