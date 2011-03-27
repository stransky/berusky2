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

/* Meshes
*/

#include "age.h"

// ********************************
// Mesh vertexes
// ********************************

void mesh_vertex::vertex_allocate(void)
{
  int i;
  
  for(i = 0; i < position_num; i++) {
    if(!p_pos[i])
      p_pos[i] = (VECT *)mmalloc(sizeof(p_pos[0][0])*vertexnum);
  }
  for(i = 0; i < normal_num; i++) {
    if(!p_norm[i])
      p_norm[i] = (VECT *)mmalloc(sizeof(p_norm[0][0])*vertexnum);
  }
  for(i = 0; i < diffuse_num; i++) {
    if(!p_diffuse[i])
      p_diffuse[i] = (RGBAF *)mmalloc(sizeof(p_diffuse[0][0])*vertexnum);
  }
  for(i = 0; i < specular_num; i++) {
    if(!p_specular[i])
      p_specular[i] = (RGBF *)mmalloc(sizeof(p_specular[0][0])*vertexnum);
  }
  for(i = 0; i < text_num; i++) {
    if(!p_uv[i])
      p_uv[i] = (VECTUV *)mmalloc(sizeof(p_uv[0][0])*vertexnum);
  }
}

void mesh_vertex::vertex_delete(void)
{
  int i;
  
  for(i = 0; i < position_num; i++) {
    if(p_pos[i])
      ffree(p_pos[i]);
  }
  for(i = 0; i < normal_num; i++) {
    if(p_norm[i])
      ffree(p_norm[i]);
  }
  for(i = 0; i < diffuse_num; i++) {
    if(p_diffuse[i])
      ffree(p_diffuse[i]);
  }
  for(i = 0; i < specular_num; i++) {
    if(p_specular[i])
      ffree(p_specular[i]);
  }
  for(i = 0; i < text_num; i++) {
    if(p_uv[i])
      ffree(p_uv[i]);
  }    
}

void mesh_vertex::array_position_add(int num)
{    
  int new_num = position_num+num;
  
  p_pos = (VECT **)rrealloc(p_pos,new_num*sizeof(p_pos[0]));
  
  int i;
  for(i = position_num; i < new_num; i++)
    p_pos[i] = (VECT *)mmalloc(sizeof(p_pos[0][0])*vertexnum);
  
  position_num = new_num;
}
  
void mesh_vertex::array_normal_add(int num)
{
  int new_num = normal_num+num;
  
  p_norm = (VECT **)rrealloc(p_norm,new_num*sizeof(p_norm[0]));

  int i;
  for(i = normal_num; i < new_num; i++)
    p_norm[i] = (VECT *)mmalloc(sizeof(p_norm[0][0])*vertexnum);
 
  normal_num = new_num;
}

void mesh_vertex::array_diff_add(int num)
{
  int new_num = diffuse_num+num;
  
  p_diffuse = (RGBAF **)rrealloc(p_diffuse,new_num*sizeof(p_diffuse[0]));
  
  int i;
  for(i = diffuse_num; i < new_num; i++)
    p_diffuse[i] = (RGBAF *)mmalloc(sizeof(p_diffuse[0][0])*vertexnum);
  
  diffuse_num = new_num;
}
  
void mesh_vertex::array_spec_add(int num)
{    
  int new_num = specular_num+num;
  
  p_specular = (RGBF **)rrealloc(p_specular,new_num*sizeof(p_specular[0]));
  
  int i;
  for(i = specular_num; i < new_num; i++)
    p_specular[i] = (RGBF *)mmalloc(sizeof(p_specular[0][0])*vertexnum);
  
  specular_num = new_num;
}

void mesh_vertex::array_text_add(int num)
{    
  int new_num = text_num+num;
  
  p_uv = (VECTUV **)rrealloc(p_uv,new_num*sizeof(p_uv[0]));
  
  int i;
  for(i = text_num; i < new_num; i++)
    p_uv[i] = (VECTUV *)mmalloc(sizeof(p_uv[0][0])*vertexnum);
  
  text_num = new_num;
}

void mesh_vertex::array_set(int positions, int normals, int diff, int spec, int texts)
{    
  if(positions)
    array_position_add(positions);
  if(normals)
    array_normal_add(normals);
  if(diff)
    array_diff_add(diff);
  if(spec)
    array_spec_add(spec);
  if(texts)
    array_text_add(texts);
}

void mesh_vertex::array_delete(void)
{
  if(p_pos)
    ffree(p_pos)
  if(p_norm)
    ffree(p_norm)
  if(p_diffuse)
    ffree(p_diffuse)
  if(p_specular)
    ffree(p_specular)
  if(p_uv)
    ffree(p_uv);
}

VECT  * mesh_vertex::position_get(int num)
{
  assert(num >= 0);
  
  if(locked()) {
    if(num >= position_num) {
      array_position_add((num+1)-position_num);      
    }
    return(p_pos[num]);
  }
  else {
    return(NULL);
  }
}

VECT  * mesh_vertex::normal_get(int num)
{    
  assert(num >= 0);
  
  if(locked()) {
    if(num >= normal_num) {
      array_normal_add((num+1)-normal_num);
    }    
    return(p_norm[num]);
  }
  else {
    return(NULL);
  }
}

RGBAF * mesh_vertex::diffuse_get(int num)
{
  assert(num >= 0);
  if(locked()) {
    if(num >= diffuse_num) {
      array_diff_add((num+1)-diffuse_num);
    }    
    return(p_diffuse[num]);
  }
  else {
    return(NULL);
  }
}

RGBF  * mesh_vertex::specular_get(int num)
{
  assert(num >= 0);  
  if(locked()) {
    if(num >= specular_num) {
      array_spec_add((num+1)-specular_num);
    }    
    return(p_specular[num]);
  }
  else {
    return(NULL);
  }
}

VECTUV * mesh_vertex::uv_get(int num)
{
  assert(num >= 0);
  if(locked()) {
    if(num >= text_num) {
      array_text_add((num+1)-text_num);
    }    
    return(p_uv[num]);
  }
  else {
    return(NULL);
  }
}

// submit one indice for rendering (direct-mode)
void mesh_vertex::set(int indice)
{
  int i;
  for(i = 0; i < text_num; i++) {
    if(p_uv[i]) {
      //pprintf("[%d][%d] glMultiTexCoord2fvARB(%f,%f)",indice,i,p_uv[i][indice].u,p_uv[i][indice].v);
      glMultiTexCoord2fvARB(gl_ext::ext_multitexture_text_index[i],(float *)(p_uv[i]+indice));
    }
  }
  /*
  if(specular_num && p_specular[0]) {
    pprintf("[%d] glSecondaryColor3fvEXT(%f,%f,%f)",indice,p_specular[0][indice].r,p_specular[0][indice].g,p_specular[0][indice].b);
    glSecondaryColor3fvEXT((float *)(p_specular[0]+indice));
  }
  */
  if(diffuse_num && p_diffuse[0]) {
    //pprintf("[%d] glColor4fv(%f,%f,%f,%f)",indice,p_diffuse[0][indice].r,p_diffuse[0][indice].g,p_diffuse[0][indice].b,p_diffuse[0][indice].a);
    glColor4fv((float *)(p_diffuse[0]+indice));
  }
  if(normal_num && p_norm[0]) {
    //pprintf("[%d] glNormal3fv(%f,%f,%f)",indice,p_norm[0][indice].x,p_norm[0][indice].y,p_norm[0][indice].z);
    glNormal3fv((float *)(p_norm[0]+indice));
  }
  if(p_pos[0]) {
    //pprintf("[%d] glVertex3fv(%f,%f,%f)",indice,p_pos[0][indice].x,p_pos[0][indice].y,p_pos[0][indice].z);
    glVertex3fv((float *)(p_pos[0]+indice));
  }
}



// ********************************
// Mesh faces
// ********************************

void mesh_face_list::draw_direct(void)
{
  assert(p_vertex != NULL);
  glBegin(GL_TRIANGLES);
  int i;
  for(i = 0; i < facenum; i++)
    p_vertex->set(p_face[i]);
  glEnd();
}

void mesh_face_list::draw_array(void)
{
  
}

// ********************************
// Mesh geometry
// ********************************
void mesh_geometry::box_update(bool force)
{
  if((force || flag_get(GEOMETRY_BOX_DIRTY)) && vertexnum_get() && facenum_get()) {
    if(!p_box) {
      p_box = box_create(box_type);
    }
    
    lock();
    p_box->set(position_get(), vertexnum_get(), faces_get(), facenum_get());
    unlock();
    
    flag_clear(GEOMETRY_BOX_DIRTY);
    box_change_set();
  }
}

void mesh_geometry::normals_update(bool force)
{
  if((force || flag_get(GEOMETRY_NORMALS_DIRTY)) && vertexnum_get() && facenum_get()) {
    
    lock();
    normals_calc(position_get(),vertexnum_get(),
                 faces_get(),facenum_get(),
                 normal_get());
    unlock();
    
    flag_set(GEOMETRY_NORMALS_DIRTY);
  }
}

void mesh_geometry::draw_normals(void)
{
  lock();
  
  if(position_get()) {
        
    if(flag_get(GEOMETRY_NORMALS_DIRTY))
      normals_update();
    
    int i,vertexnum = vertexnum_get();
    VECT *p_vert = position_get();
    VECT *p_norm = normal_get();
    
    if(p_norm == NULL)
      return;

    mesh_material::set_default();
    
    glColor3f(0.0f,0.0f,1.0f);
    glBegin(GL_LINES);
    for(i = 0; i < vertexnum; i++) {      
      glVertex3fv((float *)(p_vert+i));
      VECT endpoint = p_norm[i]*10.0f;
      endpoint += p_vert[i];
      glVertex3fv((float *)(&endpoint));
    }  
    glEnd();    
  }
  
  unlock();
}

void mesh_geometry::face_invert(void)
{ 
  int f;
  int facenum = facenum_get();
  
  for(f = 0; f < facenum; f+=3) {
    int i1 = ((MESH_FACE_LIST *)p_face)->get(f);
    int i2 = ((MESH_FACE_LIST *)p_face)->get(f+1);
    
    ((MESH_FACE_LIST *)p_face)->set(f,  i2);
    ((MESH_FACE_LIST *)p_face)->set(f+1,i1);
  }  
}

VECT * mesh_geometry::center_get(VECT *p_center)
{
  lock();
  
  if(position_get()) {
    int i,vertexnum = vertexnum_get();
    VECT *p_vert = position_get();
    
    p_center->set();        
    for(i = 0; i < vertexnum; i++) {
      *p_center += p_vert[i];
    }      
    *p_center *= (1.0f/(float)(vertexnum));
  }  
  
  unlock();
  
  return(p_center);
}

VECT * mesh_geometry::size_get(VECT *p_size)
{
  lock();
  
  if(position_get()) {
    AABB tmp(position_get(),vertexnum_get(),NULL,0);
    tmp.size_get(p_size);
  }  
  
  unlock();
  return(p_size);
}

void mesh_geometry::transformate(GLMATRIX *p_mt)
{
  lock(LOCK_READ_WRITE);
  
  if(position_get()) {
    int i,vertexnum = vertexnum_get();
    VECT *p_vert = position_get();
    
    for(i = 0; i < vertexnum; i++) {
      p_mt->transformate(p_vert+i);
    }    
  }  
  unlock();
}

// ********************************
// Whole game mesh
// ********************************
void game_mesh::draw_world_set(void)
{
  SCENE *p_scene = scene_get();
  if(p_scene) {
    GPIPE *p_gpipe = p_scene->gpipe_get();
    assert(p_gpipe);
    p_gpipe->world_set(world_global_get());
  }
}

void game_mesh::draw_box(void)
{
  BOX *p_box = object_box_get();
  if(p_box) {
    mesh_material::set_default();
    p_box->draw(NULL,RGBB(0,255,0));
  }
}

void game_mesh::draw_pivot(void)
{  
  GRAPH3D *p_graph = scene_get()->graph_get();
  VECT     pivot;
  if(p_geom && p_graph) {
    p_geom->pivot_get(&pivot);
    p_graph->draw_cross(&pivot, 1.0f);
  }
}

void game_mesh::draw_selection(void)
{  
  mesh_material::set_selection();
  p_geom->draw();
}

void game_mesh::draw(void)
{  
  if(p_geom) {
    
    /* Set a world matrix
    */
    draw_world_set();
    
    
    /* Set material
    */
    if(p_mat) {
      p_mat->set();
    }
    else {
      mesh_material::set_default();
    }
    
    /* Draw geometry
    */
    p_geom->draw();
    
    /* Draw normals if requested
    */    
    if(graph3d::config.draw_debug_normals) {
      p_geom->draw_normals();
    }
    
    /* Draw selection if this object is selected
    */
    if(graph3d::config.draw_selection) {
      if(mark_get(MARK_SELECTED)) {
        draw_selection();
      }
    }
  }
  
  /* Draw pivot if requested
  */    
  if(graph3d::config.draw_pivots) {
    draw_pivot();
  }
    
  /* Draw bouding boxes if requested
  */    
  else {
    if(graph3d::config.draw_bouding_boxes) {
      draw_box();
    }
  }
}

void game_mesh::print(int ident)
{
  pprintf("%*cMesh name '%s'",ident,' ',name);
  pprintf("%*cMesh parent name '%s'",ident,' ',render_hierarchy_parent_get() ? render_hierarchy_parent_get()->name_get() : "None");
  pprintf("%*cMaterial name '%s'",ident,' ',p_mat ? p_mat->name_get() : "NULL");
  
  GLMATRIX *p_world = world_get();
  p_world->print("Global world matrix",ident);
  p_world = world_get();
  p_world->print("Local world matrix",ident);
/*
  pprintf("%*cMesh vertexes",ident,' ');
  MESH_GEOMETRY *p_geom = lock(LOCK_READ);
  if(p_geom) {    
    
    int num = p_geom->vertexnum_get();
    VECT *p_pos = p_geom->position_get();
    
    int i;
    for(i = 0; i < num; i++, p_pos++) {
      pprintf("%*c[V:%d][%f,%f,%f]",ident,' ',i,p_pos->x,p_pos->y,p_pos->z);
    }            
  }
  unlock(FALSE);
*/
}

VECT * game_mesh::vertex_position_object_space_get(int *p_vertexnum)
{
  MESH_GEOMETRY *p_geom = lock(LOCK_READ);

  int vertexnum = p_geom->vertexnum_get();
  VECT * p_vect = NULL;
  
  if(vertexnum) {
    p_vect = (VECT *)mmalloc(sizeof(p_vect[0])*vertexnum);
    memcpy(p_vect, p_geom->position_get(), sizeof(p_vect[0])*vertexnum);
  }
  
  unlock();
  
  *p_vertexnum = vertexnum;
  return(p_vect);
}

VECT * game_mesh::vertex_position_world_space_get(int *p_vertexnum)
{
  VECT *p_vect = vertex_position_object_space_get(p_vertexnum);
  if(p_vect) {
    int vertexnum = *p_vertexnum;
    GLMATRIX *p_mat = world_global_get();
    p_mat->transformate(p_vect,vertexnum);
  }
  return(p_vect);
}

VECT * game_mesh::vertex_position_camera_space_get(int *p_vertexnum)
{
  VECT *p_vect = vertex_position_world_space_get(p_vertexnum);
  if(p_vect) {
    GPIPE *p_pipe = gpipe_get();
    p_pipe->world_to_camera(p_vect,*p_vertexnum);
  }
  return(p_vect);
}

VECT2DI * game_mesh::vertex_position_screen_space_get(int *p_vertexnum)
{
  VECT2DI  *p_vect_2d = NULL;
  int       vertexnum;
  
  VECT *p_vect = vertex_position_world_space_get(&vertexnum);
  if(p_vect) {
    p_vect_2d = (VECT2DI *)mmalloc(sizeof(p_vect_2d[0])*vertexnum);
    (gpipe_get())->world_to_screen(p_vect,vertexnum,p_vect_2d);
    *p_vertexnum = vertexnum;
    ffree(p_vect);
  }
  
  return(p_vect_2d);
}

tface * game_mesh::faces_get(int *p_facenum)
{
  MESH_GEOMETRY *p_geom = lock(LOCK_READ);

  int facenum = p_geom->facenum_get();
  tface * p_faces = NULL;
  
  if(facenum) {
    p_faces = (tface *)mmalloc(sizeof(p_faces[0])*facenum);
    memcpy(p_faces, p_geom->faces_get(), sizeof(p_faces[0])*facenum);
  }
  
  unlock();
  
  *p_facenum = facenum;
  return(p_faces);
}

void game_mesh::mesh_remove(void)
{
  SCENE *p_scene = scene_get();
  assert(p_scene != NULL);
  p_scene->mesh_remove(this);
}

game_mesh::game_mesh(class scene *p_scene)
  : scene_object_geometry(p_scene)
{
  p_geom = NULL;
  p_mat = NULL;
  
  mflag = 0;
  m2flag = 0;
  
  object_type_set(SCENE_OBJECT_MESH);
  object_box_type_set(BOX_OBB);
}

game_mesh::game_mesh(class game_mesh &src)
  : scene_object_geometry(NULL)
{
  assert(0);
}

game_mesh::~game_mesh(void)
{
  geometry_delete();
}

void game_mesh_list::print(void)
{
  GAME_MESH *p_tmp = (GAME_MESH *)object_list_get_first();
  while(p_tmp) {
    p_tmp->print();
    p_tmp = (GAME_MESH *)(p_tmp->object_list_next());
  }  
}
