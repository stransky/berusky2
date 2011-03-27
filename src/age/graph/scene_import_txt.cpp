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

class import_txt;

/* 
 * This structure is stored in data-file, 
 * before every data-block (chunk)
 */
typedef struct _data_chunk_txt {

  int   type;     // type of stored data
  int   size;     // size of stored data (in bytes)

} DATA_CHUNK_TXT;

/* 
 * A function for loading chunks from file
 */
typedef int (import_txt::*CHUNK_LOAD_TXT_FUNC)(FHANDLE *f, DATA_CHUNK_TXT *p_chunk);

/* 
 * Chunk loader structure
 */
typedef struct _chunk_loader_txt {
  
  CHUNK_LOAD_TXT_FUNC  p_func;
  int                  chunk;

} CHUNK_LOADER_TXT;

/* 
 * Group of chunk with the same meaning (i.e. meshes / materials / ...)
 */
typedef struct chunk_group_txt {

  CHUNK_LOADER_TXT   *p_chunks;
  int                 chunk_num;
  
  int                 chunk_range_min;
  int                 chunk_range_max;

  bool check(DATA_CHUNK_TXT *p_chunk);
  
} CHUNK_GROUP_TXT;

/* 
 * Main import/export class
 */
typedef class import_txt {
 
  SCENE *p_scene;
  
// *************************************************************************
//  Save routines - Meshes
// *************************************************************************

  /*
   * Save routines
   */
  int game_mesh_save_mesh_name(FHANDLE *f, GAME_MESH *p_mesh)
  {
    if(p_mesh) {      
      fprintf(*f, "mesh: '%s'\n", p_mesh->name_get());
      return(TRUE);
    } else
      return(FALSE);
  }
  
  int game_mesh_save_mesh_material(FHANDLE *f, GAME_MESH *p_mesh)
  {
    if(p_mesh && p_mesh->material_get()) {
      MESH_MATERIAL *p_mat = p_mesh->material_get();    
      fprintf(*f, "mesh material: '%s'\n", p_mat->name_get());
      return(TRUE);
    } else
      return(FALSE);
  }
  
  int game_mesh_save_mesh_vertex(FHANDLE *f, GAME_MESH *p_mesh)
  {
    if(p_mesh && p_mesh->geometry_active()) {
      MESH_GEOMETRY *p_geom = p_mesh->lock(LOCK_READ);
      
      int num = p_geom->vertexnum_get();
      fprintf(*f, "mesh vertex num: %d\n", num);
      fprintf(*f, "mesh vertex list:\n");
  
      VECT *p_pos = p_geom->position_get();
      int i;
      for(i = 0; i < num; i++) {
        fprintf(*f, "%d: %f %f %f\n",i,p_pos[i].x,p_pos[i].y,p_pos[i].z);
      }
      
      p_mesh->unlock();
      
      return(TRUE);
    } else
      return(FALSE);
  }
  
  int game_mesh_save_mesh_face(FHANDLE *f, GAME_MESH *p_mesh)
  {
    if(p_mesh && p_mesh->geometry_active()) {
      
      MESH_GEOMETRY *p_geom = p_mesh->lock(LOCK_READ);      
      int facenum = p_geom->facenum_get();
      
      fprintf(*f, "mesh face num: %d\n", facenum/3);
      fprintf(*f, "mesh face list:\n");
      
      int i;
      for(i = 0; i < facenum; i += 3) {
        fprintf(*f, "%d: %d %d %d\n",i/3, p_geom->face_get(i),
                                          p_geom->face_get(i+1),
                                          p_geom->face_get(i+2));
      }
      
      p_mesh->unlock();
      
      return(TRUE);
    } else
      return(FALSE);
  }
    
  int game_mesh_save_mesh_maps(FHANDLE *f, GAME_MESH *p_mesh, int text_index)
  {  
    if(p_mesh && p_mesh->geometry_active()) {
      MESH_GEOMETRY *p_geom = p_mesh->lock(LOCK_READ);
            
      int vertex_num = p_geom->vertexnum_get();
      
      fprintf(*f, "mesh vertex map num: %d\n", vertex_num);
      fprintf(*f, "mesh vertex map index: %d\n", text_index);
      fprintf(*f, "mesh vertex map list:\n");
            
      VECTUV *p_map = p_geom->uv_get(text_index);
      int i;      
      for(i = 0; i < vertex_num; i++) {
        fprintf(*f, "%d: %f %f\n",i,p_map[i].u,p_map[i].v);
      }    
      
      p_mesh->unlock();
      
      return(TRUE);
    } else
      return(FALSE);
  }
       
  /*
   * Save colors
   */  
  int game_mesh_save_mesh_colors(FHANDLE *f, GAME_MESH *p_mesh)
  {
    if(p_mesh && p_mesh->geometry_active()) {
      
      MESH_GEOMETRY *p_geom = p_mesh->lock(LOCK_READ);
      int num = p_geom->vertexnum_get();
      
      fprintf(*f, "mesh vertex color diffuse num: %d\n", num);
      fprintf(*f, "mesh vertex color diffuse list:\n");
        
      RGBAF *p_diff = p_geom->diffuse_get();
      int i;
      for(i = 0; i < num; i++) {
        fprintf(*f, "%d: %f %f %f %f\n",i,p_diff[i].r,
                                          p_diff[i].g,
                                          p_diff[i].b,
                                          p_diff[i].a);
      }
      
      fprintf(*f, "mesh vertex color specular num: %d\n", num);
      fprintf(*f, "mesh vertex color specular list:\n");
      
      RGBF  *p_spec = p_geom->specular_get();      
      for(i = 0; i < num; i++) {        
        fprintf(*f, "%d: %f %f %f %f\n",i,p_spec[i].r,
                                          p_spec[i].g,
                                          p_spec[i].b,
                                          1.0f);
      }
  
      p_mesh->unlock();
      
      return(TRUE);
    } else
      return(FALSE);
  }

  /*
   * Save mesh to file
   */
  bool game_mesh_save(FHANDLE *f, GAME_MESH *p_mesh)
  {    
    
    // Save general info    
    game_mesh_save_mesh_name(f, p_mesh);

    // Save material        
    game_mesh_save_mesh_material(f,p_mesh);
    
    // Save vertexes
    game_mesh_save_mesh_vertex(f, p_mesh);

    // Save faces    
    game_mesh_save_mesh_face(f, p_mesh);    

    // Save maps    
    game_mesh_save_mesh_maps(f,p_mesh,0);
    game_mesh_save_mesh_maps(f,p_mesh,1);
    game_mesh_save_mesh_maps(f,p_mesh,2);
    game_mesh_save_mesh_maps(f,p_mesh,3);

    // Save colors
    game_mesh_save_mesh_colors(f,p_mesh);
    
    return(TRUE);
  }
 
// *************************************************************************
//  Save routines - Materials
// *************************************************************************
  
  /*
   * Material save routines
   */  
  int mesh_material_save_name(FHANDLE *f, MESH_MATERIAL *p_mat)
  {
    fprintf(*f, "material: '%s'\n", p_mat->name_get());
    return(TRUE);
  }
    
  int mesh_material_save_color(FHANDLE *f, MESH_MATERIAL *p_mat)
  {  
    fprintf(*f, "material diffuse:\n");
    RGBAF *p_diff = p_mat->diffuse_get();
    fprintf(*f, "%f %f %f %f\n",p_diff->r,
                                p_diff->g,
                                p_diff->b,
                                p_diff->a);
    
    fprintf(*f, "material specular:\n");
    RGBF *p_spec = p_mat->specular_get();
    fprintf(*f, "%f %f %f %f\n",p_spec->r,
                                p_spec->g,
                                p_spec->b,
                                1.0f);
    return(TRUE);
  }
        
  int mesh_material_save_text(FHANDLE *f, MESH_MATERIAL *p_mat, int num)
  {
    MATERIAL_TEXT *p_text = p_mat->texture_get(num);
    if(p_text) {
      fprintf(*f, "material texture: '%s'\n",p_text->name_get());
      return(TRUE);
    } else
      return(FALSE);
  }
  
  bool mesh_texture_save(FHANDLE *f, MATERIAL_TEXT *p_text, char *p_dir)
  {
    if(file_exists(p_dir, p_text->name_get()))
      return(TRUE);
    return(p_text->save(p_dir));
  }
    
  // *************************************************************************
  // Saves complete material to file
  // *************************************************************************
  
  bool mesh_material_save(FHANDLE *f, MESH_MATERIAL *p_mat, char *p_dir)
  {
    pdebug(DEBUG_LOAD_CHUNK,"Saving material %s",p_mat->name_get());
    
    mesh_material_save_name(f,p_mat);
    mesh_material_save_color(f,p_mat);
    
    int i;
    for(i = 0; i < 4; i++) {    
      mesh_material_save_text(f, p_mat, i);
      
      MATERIAL_TEXT *p_text = p_mat->texture_get();
      if(p_text)
        mesh_texture_save(f, p_text, p_dir);
    }
        
    return(TRUE);
  }
  
public:
  
  /*
   * Save objects to data file
   */
  bool save(char *p_dir, char *p_file)
  {
    FFILE f;
    
    // Open the file
    if(!f.open(p_dir, p_file, "wt", FALSE)) {
      return(FALSE);
    }
    
    // go through all materials and save them
    MESH_MATERIAL *p_mat = p_scene->material_get_first();
    while(p_mat) {
      pdebug(DEBUG_LOAD_CHUNK,"Saving material %s",p_mat->name_get());
      mesh_material_save(&f, p_mat, p_dir);
      p_mat = p_mat->material_next();
    }
    
    // go through all meshes and save them
    GAME_MESH *p_mesh = p_scene->mesh_get_first();
    while(p_mesh) {
      pdebug(DEBUG_LOAD_CHUNK,"Saving mesh %s",p_mesh->name_get());
      game_mesh_save(&f, p_mesh);
      p_mesh = p_mesh->mesh_next();
    }
    
    f.close();
    
    return(TRUE);    
  }

public:
  
  import_txt(SCENE *p_scene_ref)
  {
    p_scene = p_scene_ref;
  }
  
  ~import_txt(void)
  {
  }
  
} IMPORT_TXT;

/*
 * Save objects to data file
 */
static bool scene_import_txt(SCENE *p_scene, FHANDLE *f)
{
  return(FALSE);
}

bool scene_import_txt(SCENE *p_scene, char *p_dir, char *p_file)
{
  MMHANDLE f;
  bool ret = FALSE;
  
  if(f.open(p_dir, p_file, "rb", FALSE)) {
    ret = scene_import_txt(p_scene, &f);
    f.close();
  }
  
  return(ret);  
}

bool scene_export_txt(SCENE *p_scene, char *p_dir, char *p_file)
{
  IMPORT_TXT import(p_scene);
  return(import.save(p_dir, p_file));
}
