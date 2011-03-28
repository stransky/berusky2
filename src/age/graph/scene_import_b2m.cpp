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

class import_b2m;

/* 
 * This structure is stored in data-file, 
 * before every data-block (chunk)
 */
typedef struct _DATA_CHUNK {

  int   type;     // type of stored data
  int   size;     // size of stored data (in bytes)

} DATA_CHUNK;

/* 
 * A function for loading chunks from file
 */
typedef int (import_b2m::*CHUNK_LOAD_FUNC)(FHANDLE *f, DATA_CHUNK *p_chunk);

/* 
 * Chunk loader structure
 */
typedef struct _CHUNK_LOADER {
  
  CHUNK_LOAD_FUNC  p_func;
  int              chunk;

} CHUNK_LOADER;

/* 
 * Group of chunk with the same meaning (i.e. meshes / materials / ...)
 */
typedef struct chunk_group {

  CHUNK_LOADER  *p_chunks;
  int            chunk_num;
  
  int            chunk_range_min;
  int            chunk_range_max;

  bool check(DATA_CHUNK *p_chunk);
  
} CHUNK_GROUP;

/* 
 * Main import/export class
 */
typedef class import_b2m {

private:
  
  SCENE       *p_scene;

private:

  int chunk_skip(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    f->seek(p_ch->size - sizeof(*p_ch), SEEK_CUR);
    return(TRUE);
  }
  
// *************************************************************************
//  Load/Save routines - Meshes
// *************************************************************************
  
private:
  
  GAME_MESH   *p_mesh;
 
private:
  
  GAME_MESH * mesh_new(const char *p_name = NULL, const char *p_parent_name = NULL)
  {
    p_mesh = p_scene->mesh_create(p_name,p_parent_name);
    return(p_mesh);
  }

private:

  char        kont_name[MAX_NAME];
  int         kont_name_ID;
  int         kont_flag;
  int         kont_flag2;
  int         kont_material_flag;
  int         kont_material_flag2;

private:

  // Containers -> a logical group of mesh
  #define  CHUNK_KONT_NAME        100  // zacatek kontejneru - jeho jmeno
  #define  CHUNK_KONT_FLAG        101  // flag kontejneru
  #define  CHUNK_KONT_WMATRIX     102  // matice kontejneru
  #define  CHUNK_KONT_FLAG_EXT    103  // flag kontejneru
  #define  CHUNK_KONT_MPMATRIX    104  // m-p matice kontejneru
  #define  CHUNK_KONT_LOW_NAME    105  // zacatek low kontejneru
  #define  CHUNK_KONT_LOW_ID      106  // identifikator low-kontejneru
  #define  CHUNK_KONT_ID          107  // identifikator kontejneru
  #define  CHUNK_KONT_FLAG_EXT_2  108 // ext-flagy
  #define  CHUNK_KONT_OBB         110  // OBB kontejneru
  
  // Objects -> a single meshes
  #define  CHUNK_OBJ_NAME         1001 // jmeno objektu
  #define  CHUNK_OBJ_MATERIAL     1002 // material objektu
  #define  CHUNK_OBJ_VERTEXES     1003 // pole vertexu
  #define  CHUNK_OBJ_MAPS_1       1004 // pole mapu
  #define  CHUNK_OBJ_MAPS_2       1005 // pole mapu
  #define  CHUNK_OBJ_FACES        1006 // pole facu
  #define  CHUNK_OBJ_COLOR        1007 // pole barev
  #define  CHUNK_OBJ_COLOR_EXT    1008 // pole rozsirenych barev
  #define  CHUNK_OBJ_MATRIX       1009 // local-matrix kontejneru (pro keyframe-kontejnery)
  #define  CHUNK_OBJ_ID           1010 // ID objektu
  #define  CHUNK_OBJ_MATRIX_KEY   1011 // keyframe matice
  #define  CHUNK_OBJ_MAPS_3       1012 // pole mapu 3
  #define  CHUNK_OBJ_MAPS_4       1013 // pole mapu 4
  #define  CHUNK_OBJ_PIVOT        1014 // pivot-point objektu
  #define  CHUNK_OBJ_OBB          1016 // obb obalka
  #define  CHUNK_OBJ_FACES_OPT    1017 // pole facu
  #define  CHUNK_OBJ_NORMAL       1018 // pole normalu
  #define  CHUNK_OBJ_NAME_PARENT  1019 // name of object and name of it's parent
  
  #define  CHUNK_OBJ_JA           1500
  #define  CHUNK_OBJ_JA_NAME      1501
  #define  CHUNK_OBJ_JA_FRAME_NUM 1502
  
  #define  CHUNK_OBJ_JNT_ID       1600
  #define  CHUNK_OBJ_JNT_PIVOT    1601
  
  #define  CHUNK_OBJ_JNT_VERT     1602
  #define  CHUNK_OBJ_JNT_FLAG     1603
  #define  CHUNK_OBJ_JNT_POS      1604
  #define  CHUNK_OBJ_JNT_ROT      1605
  #define  CHUNK_OBJ_JNT_SCS      1606
  
  #define  CHUNK_OBJ_JNT_KEYPOS   1607
  #define  CHUNK_OBJ_JNT_KEYROT   1608
  #define  CHUNK_OBJ_JNT_KEYSCS   1609

  /*
   * Save routines
   */
  int game_mesh_save_mesh_name(FHANDLE *f, GAME_MESH *p_mesh)
  {
    DATA_CHUNK ch;
    if(p_mesh) {
      ch.type = CHUNK_OBJ_NAME;
      ch.size = sizeof(ch)+strlen(p_mesh->name_get())+1;
      f->write(&ch,sizeof(ch));      
      f->write_string(p_mesh->name_get());
      return(TRUE);
    } else
      return(FALSE);
  }
  
  int game_mesh_save_mesh_ID(FHANDLE *f, GAME_MESH *p_mesh)
  {
    DATA_CHUNK ch;
    if(p_mesh) {
      ch.type = CHUNK_OBJ_ID;
      ch.size = sizeof(ch)+sizeof(int);
      int id = p_mesh->name_ID_get();
      f->write(&ch,sizeof(ch));
      f->write(&id,sizeof(int));
      return(TRUE);
    } else
      return(FALSE);
  }
  
  int game_mesh_save_mesh_pivot(FHANDLE *f, GAME_MESH *p_mesh)
  {
    DATA_CHUNK ch;
    if(p_mesh && p_mesh->geometry_active()) {
      ch.type = CHUNK_OBJ_PIVOT;
      ch.size = sizeof(ch)+sizeof(VECT);
      f->write(&ch,sizeof(ch));
      
      MESH_GEOMETRY *p_geom = p_mesh->lock(LOCK_READ);
      f->write(p_geom->pivot_get(),sizeof(VECT));
      p_mesh->unlock();
      
      return(TRUE);
    } else
      return(FALSE);
  }
  
  int game_mesh_save_mesh_obb(FHANDLE *f, GAME_MESH *p_mesh)
  {
    DATA_CHUNK ch;
    if(p_mesh && p_mesh->object_box_type_get() == BOX_OBB) {

      OBB *p_obb = (OBB *)p_mesh->object_box_get();      
      if(p_obb) {
        ch.type = CHUNK_OBJ_OBB;
        ch.size = sizeof(ch)+
                  sizeof(VECT)*3+
                  sizeof(VECT)+
                  sizeof(VECT)+
                  sizeof(VECT)*8+
                  sizeof(VECT)*2;
        
        f->write(&ch,sizeof(ch));
        f->write(p_obb->dir,sizeof(p_obb->dir));
        f->write(&p_obb->center,sizeof(p_obb->center));
        f->write(&p_obb->len,sizeof(p_obb->len));
        f->write(p_obb->border,sizeof(p_obb->border));
        f->seek(sizeof(VECT)*2,SEEK_CUR);
        
        return(TRUE);
      }
    }
        
    return(FALSE);
  }
  
  int game_mesh_save_mesh_material(FHANDLE *f, GAME_MESH *p_mesh)
  {
    DATA_CHUNK ch;
    
    if(p_mesh && p_mesh->material_get()) {
      MESH_MATERIAL *p_mat = p_mesh->material_get();
      
      ch.type = CHUNK_OBJ_MATERIAL;
      ch.size = sizeof(ch)+strlen(p_mat->name_get())+1;
      
      f->write(&ch,sizeof(ch));
      f->write_string(p_mat->name_get());
      
      return(TRUE);
    } else
      return(FALSE);
  }
  
  int game_mesh_save_mesh_matrix(FHANDLE *f, GAME_MESH *p_mesh)
  {
    DATA_CHUNK ch;
    if(p_mesh) {
      ch.type = CHUNK_OBJ_MATRIX_KEY;
      ch.size = sizeof(ch)+sizeof(GLMATRIX);
      f->write(&ch,sizeof(ch));
      f->write(p_mesh->world_get(),sizeof(GLMATRIX));
      return(TRUE);
    } else
      return(FALSE);
  }
  
  int game_mesh_save_mesh_vertex(FHANDLE *f, GAME_MESH *p_mesh)
  {
    if(p_mesh && p_mesh->geometry_active()) {
      MESH_GEOMETRY *p_geom = p_mesh->lock(LOCK_READ);
      
      DATA_CHUNK ch;
      ch.type = CHUNK_OBJ_VERTEXES;
      ch.size = sizeof(ch)+
                sizeof(dword)+
                sizeof(float)*3*p_geom->vertexnum_get();
      
      f->write(&ch,sizeof(ch));
      
      int num = p_geom->vertexnum_get();
      f->write(&num,sizeof(num));
  
      VECT *p_pos = p_geom->position_get();
      int i;
      for(i = 0; i < num; i++) {
        f->write(p_pos+i,sizeof(VECT));
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
      
      DATA_CHUNK ch;
      ch.type = CHUNK_OBJ_FACES;      
      ch.size = sizeof(ch)+sizeof(dword)+sizeof(word)*facenum;
      
      f->write(&ch,sizeof(ch));            
      f->write(&facenum,sizeof(int));
      
      int i;
      for(i = 0; i < facenum; i++) {
        word face = p_geom->face_get(i);
        f->write(&face,sizeof(word));
      }
      
      p_mesh->unlock();
      
      return(TRUE);
    } else
      return(FALSE);
  }
  
  /*
  int game_mesh_save_mesh_face_ext(FHANDLE *f, GAME_MESH *p_mesh)
  {
    DATA_CHUNK ch;
    
    if(p_mesh && p_mesh->geometry_active()) {
      ch.type = CHUNK_OBJ_FACES_OPT;
      ch.size = sizeof(ch)+sizeof(p_mesh->optnum)+
      sizeof(p_mesh->p_opt[0])*p_mesh->optnum;
      
      f->write(&ch,sizeof(ch));    
      f->write(&p_mesh->optnum,sizeof(p_mesh->optnum));
      f->write(p_mesh->p_opt,sizeof(p_mesh->p_opt[0]),p_mesh->optnum,f);
      
      return(TRUE);
    } else
      return(FALSE);
  }
  */
  
  int game_mesh_save_mesh_maps(FHANDLE *f, GAME_MESH *p_mesh, int text_index)
  {  
    if(p_mesh && p_mesh->geometry_active()) {
      MESH_GEOMETRY *p_geom = p_mesh->lock(LOCK_READ);
      
      DATA_CHUNK ch;
      
      int map_table[] = 
      {
        CHUNK_OBJ_MAPS_1,
        CHUNK_OBJ_MAPS_2,
        CHUNK_OBJ_MAPS_3,
        CHUNK_OBJ_MAPS_4
      };

      int vertex_num = p_geom->vertexnum_get();
      
      ch.type = map_table[text_index];
      ch.size = sizeof(ch)+
                    sizeof(int)+
                    sizeof(float)*2*vertex_num;
      
      f->write(&ch,sizeof(ch));
      f->write(&vertex_num,sizeof(int));
      
      VECTUV *p_map = p_geom->uv_get(vertex_num);
      int i;      
      for(i = 0; i < vertex_num; i++) {
        f->write(p_map+i,sizeof(VECTUV));
      }    
      
      p_mesh->unlock();
      
      return(TRUE);
    } else
      return(FALSE);
  }
      
  int game_mesh_save_mesh_normal(FHANDLE *f, GAME_MESH *p_mesh)
  {
    if(p_mesh && p_mesh->geometry_active()) {
      
      MESH_GEOMETRY *p_geom = p_mesh->lock(LOCK_READ);
      int num = p_geom->vertexnum_get();
      
      DATA_CHUNK ch;
      
      ch.type = CHUNK_OBJ_NORMAL;
      ch.size = sizeof(ch)+sizeof(int)+sizeof(float)*3*num;
      
      f->write(&ch,sizeof(ch));          
      f->write(&num,sizeof(int));
  
      VECT *p_norm = p_geom->normal_get();
      int i;
      for(i = 0; i < num; i++) {
        f->write(p_norm+i,sizeof(VECT));
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
      
      DATA_CHUNK ch;
      
      ch.type = CHUNK_OBJ_COLOR_EXT;
      ch.size = sizeof(ch)+
                sizeof(dword)+
                sizeof(float)*4*num*2+
                sizeof(float)*4*num*2;
      
      f->write(&ch,sizeof(ch));
      f->write(&num,sizeof(int));
  
      RGBAF *p_diff = p_geom->diffuse_get();
      RGBF  *p_spec = p_geom->specular_get();
      
      int i;
      for(i = 0; i < num; i++) {        
        f->write(p_diff+i,sizeof(RGBAF));
        f->write(p_diff+i,sizeof(RGBAF));
        
        RGBAF tmp(p_spec[i]);
        f->write(&tmp,sizeof(RGBAF));
        f->write(&tmp,sizeof(RGBAF));
      }
  
      p_mesh->unlock();
      
      return(TRUE);
    } else
      return(FALSE);
  }
  /*
  int game_mesh_save_mesh_joint_spline(FHANDLE *f, JOINT_KEYS *p_keys)
  {
    DATA_CHUNK ch;
    
    if(!p_keys)
      return(FALSE);
  
    if(p_keys->p_pos) {
      ch.type = CHUNK_OBJ_JNT_KEYPOS;
      ch.size = sizeof(ch)+sizeof(p_keys->pos_keys)+p_keys->pos_keys*sizeof(p_keys->p_pos[0])+p_keys->pos_keys*sizeof(p_keys->p_pkeys[0]);
      f->write(&ch,sizeof(ch));
      f->write(&p_keys->pos_keys,sizeof(p_keys->pos_keys));
      f->write(p_keys->p_pos,sizeof(p_keys->p_pos[0]),p_keys->pos_keys,f);
      f->write(p_keys->p_pkeys,sizeof(p_keys->p_pkeys[0]),p_keys->pos_keys,f);
    }
    
    if(p_keys->p_rot) {
      ch.type = CHUNK_OBJ_JNT_KEYROT;
      ch.size = sizeof(ch)+sizeof(p_keys->rot_keys)+p_keys->rot_keys*sizeof(p_keys->p_rot[0])+p_keys->rot_keys*sizeof(p_keys->p_rkeys[0]);
      file_write(&ch,sizeof(ch),1,f);
      file_write(&p_keys->rot_keys,sizeof(p_keys->rot_keys),1,f);
      file_write(p_keys->p_rot,sizeof(p_keys->p_rot[0]),p_keys->rot_keys,f);
      file_write(p_keys->p_rkeys,sizeof(p_keys->p_rkeys[0]),p_keys->rot_keys,f);
    }
    
    if(p_keys->p_scale) {
      ch.type = CHUNK_OBJ_JNT_KEYSCS;
      ch.size = sizeof(ch)+sizeof(p_keys->scs_keys)+p_keys->scs_keys*sizeof(p_keys->p_scale[0])+p_keys->scs_keys*sizeof(p_keys->p_skeys[0]);
      file_write(&ch,sizeof(ch),1,f);
      file_write(&p_keys->scs_keys,sizeof(p_keys->scs_keys),1,f);
      file_write(p_keys->p_scale,sizeof(p_keys->p_scale[0]),p_keys->scs_keys,f);
      file_write(p_keys->p_skeys,sizeof(p_keys->p_skeys[0]),p_keys->scs_keys,f);
    }
  
    return(TRUE);
  }
  
  #define ROOT_ID 0
  
  int game_mesh_save_mesh_joint(FHANDLE *f, JOINT *p_joint, int id_prev)
  {
    DATA_CHUNK ch;
    
    if(!p_joint)
      return(FALSE);  
  
    ch.type = CHUNK_OBJ_JNT_ID;
    ch.size = sizeof(ch)+2*sizeof(p_joint->joint_ID);
    file_write(&ch,sizeof(ch),1,f);
    file_write(&p_joint->joint_ID,sizeof(p_joint->joint_ID),1,f);
    file_write(&id_prev,sizeof(id_prev),1,f);
  
    ch.type = CHUNK_OBJ_JNT_PIVOT;
    ch.size = sizeof(ch)+sizeof(p_joint->pivot);
    file_write(&ch,sizeof(ch),1,f);
    file_write(&p_joint->pivot,sizeof(p_joint->pivot),1,f);
  
    if(p_joint->p_vertexlist) {
      ch.type = CHUNK_OBJ_JNT_VERT;
      ch.size = sizeof(ch)+sizeof(p_joint->vertexnum)+p_joint->vertexnum*sizeof(p_joint->p_vertexlist[0]);
      file_write(&ch,sizeof(ch),1,f);
      file_write(&p_joint->vertexnum,sizeof(p_joint->vertexnum),1,f);
      file_write(p_joint->p_vertexlist,sizeof(p_joint->p_vertexlist[0]),p_joint->vertexnum,f);
    }
    
    ch.type = CHUNK_OBJ_JNT_FLAG;
    ch.size = sizeof(ch)+sizeof(p_joint->flag);
    file_write(&ch,sizeof(ch),1,f);
    file_write(&p_joint->flag,sizeof(p_joint->flag),1,f);
    
    if(p_joint->p_pos) {
      ch.type = CHUNK_OBJ_JNT_POS;
      ch.size = sizeof(ch)+sizeof(p_joint->pos_keys)+p_joint->pos_keys*sizeof(p_joint->p_pos[0]);
      file_write(&ch,sizeof(ch),1,f);
      file_write(&p_joint->pos_keys,sizeof(p_joint->pos_keys),1,f);
      file_write(p_joint->p_pos,sizeof(p_joint->p_pos[0]),p_joint->pos_keys,f);
    }
    
    if(p_joint->p_rot) {
      ch.type = CHUNK_OBJ_JNT_ROT;
      ch.size = sizeof(ch)+sizeof(p_joint->rot_keys)+p_joint->rot_keys*sizeof(p_joint->p_rot[0]);
      file_write(&ch,sizeof(ch),1,f);
      file_write(&p_joint->rot_keys,sizeof(p_joint->rot_keys),1,f);
      file_write(p_joint->p_rot,sizeof(p_joint->p_rot[0]),p_joint->rot_keys,f);
    }
    
    if(p_joint->p_scs) {
      ch.type = CHUNK_OBJ_JNT_SCS;
      ch.size = sizeof(ch)+sizeof(p_joint->scs_keys)+p_joint->scs_keys*sizeof(p_joint->p_scs[0]);
      file_write(&ch,sizeof(ch),1,f);
      file_write(&p_joint->scs_keys,sizeof(p_joint->scs_keys),1,f);
      file_write(p_joint->p_scs,sizeof(p_joint->p_scs[0]),p_joint->scs_keys,f);
    }
  
    if(p_joint->p_keys)
      game_mesh_save_mesh_joint_spline(f, p_joint->p_keys);
  
    game_mesh_save_mesh_joint(f, p_joint->p_next, p_joint->joint_ID);
    game_mesh_save_mesh_joint(f, p_joint->p_child, p_joint->joint_ID);  
    
    return(TRUE);
  }
  
  int game_mesh_save_mesh_joint_animaci(FHANDLE *f, JOINT_ANIMACE *p_jani)
  {
    DATA_CHUNK ch;
    
    ch.type = CHUNK_OBJ_JA;
    ch.size = sizeof(ch)+strlen(p_jani->name)+1;    
    file_write(&ch,sizeof(ch),1,f);
    file_write(p_jani->name,sizeof(byte),strlen(p_jani->name)+1,f);
    
    ch.type = CHUNK_OBJ_JA_FRAME_NUM;
    ch.size = sizeof(ch)+sizeof(p_jani->framenum);
    file_write(&ch,sizeof(ch),1,f);
    file_write(&p_jani->framenum,sizeof(p_jani->framenum),1,f);  
    
    game_mesh_save_mesh_joint(f, p_jani->p_child, ROOT_ID);  
    
    return(TRUE);
  }
  
  void lo_kost_ocisluj_rec(JOINT *p_joint, int *p_cislo)
  {
    if(p_joint->p_child)
      lo_kost_ocisluj_rec(p_joint->p_child,p_cislo);
    if(p_joint->p_next)
      lo_kost_ocisluj_rec(p_joint->p_next,p_cislo);
    
    p_joint->joint_ID = (*p_cislo)++;
  }
  
  int game_mesh_save_mesh_joint_animace(FHANDLE *f, GAME_MESH *p_mesh)
  {
    JOINT_ANIMACE *p_akt = p_mesh->p_joit_animace;
    int ID;
  
    while(p_akt) {
      ID = 1;
      lo_kost_ocisluj_rec(p_akt->p_child, &ID);
      game_mesh_save_mesh_joint_animaci(f, p_akt);
      p_akt = p_akt->p_next;
    }
    return(TRUE);
  }
  */
  
  /*
   * Save mesh to file
   */
  bool game_mesh_save(FHANDLE *f, GAME_MESH *p_mesh)
  {    
    
    // Save general info    
    game_mesh_save_mesh_name(f, p_mesh);
    game_mesh_save_mesh_ID(f, p_mesh);
    game_mesh_save_mesh_pivot(f, p_mesh);
    game_mesh_save_mesh_obb(f, p_mesh);

    // Save material        
    game_mesh_save_mesh_material(f,p_mesh);
    
    // Save vertexes
    game_mesh_save_mesh_vertex(f, p_mesh);

    // Save normals
    game_mesh_save_mesh_normal(f, p_mesh);

    // Save faces    
    game_mesh_save_mesh_face(f, p_mesh);    

    // Save maps    
    game_mesh_save_mesh_maps(f,p_mesh,0);
    game_mesh_save_mesh_maps(f,p_mesh,1);
    game_mesh_save_mesh_maps(f,p_mesh,2);
    game_mesh_save_mesh_maps(f,p_mesh,3);

    // Save colors
    game_mesh_save_mesh_colors(f,p_mesh);

    // Save matrixes    
    game_mesh_save_mesh_matrix(f,p_mesh);

    // Save mesh animations
    //if(p_mesh->p_obj[i]->p_joit_animace) {
    //  game_mesh_save_mesh_joint_animace((FILE *)f,p_mesh->p_obj[i]);
    
    return(TRUE);
  }
  
  /*  
  int game_mesh_save(FHANDLE *f, GAME_MESH *p_mesh, EDIT_MATERIAL **p_mat, int max_mat, int low, int kont, int kont_id, int transf_stat)
  {  
    int i;
  
     Oflagovani podle materialu
     
    updatuj_kontejner_flag(p_mesh, p_mat);
    
     Optimalizace jmen objektu
    
    lo_kontejner_jedinecne_jmena(p_mesh,FALSE);
  
    p_mesh->kontejner_ID = kont_id;
    if(low) {
      game_mesh_save_kont_low_name(f,p_mesh);
    } else {
      game_mesh_save_kont_name(f,p_mesh,kont);
    }
  
    game_mesh_save_kont_low_id(f, p_mesh);
    game_mesh_save_kont_id(f,p_mesh->kontejner_ID);
    game_mesh_save_kont_flag(f,p_mesh);
    game_mesh_save_kont_flag_ext(f,p_mesh);
    game_mesh_save_kont_flag_ext_2(f,p_mesh);  
    game_mesh_save_kont_obb(f, p_mesh);
    
     Podle transformacniho flagu transf_stat
    
    transf_stat = (p_mesh->kflag&KONT_STATIC && transf_stat);
    if(!transf_stat) {
      game_mesh_save_kont_wmatrix(f, p_mesh, &p_mesh->world);
      game_mesh_save_kont_mpmatrix(f, p_mesh, &p_mesh->mpmatrix);
    } else {
      GLMATRIX m;
      init_matrix(&m);
      game_mesh_save_kont_wmatrix(f,p_mesh,&m);
      game_mesh_save_kont_mpmatrix(f,p_mesh,&m);
    }
    i = game_mesh_save_kont_mesh(f,p_mesh,p_mat,max_mat,
                                transf_stat ? kont_world_matrix(p_mesh) : NULL);
    
    // Uloz key-framy
    
    if(p_mesh->kflag&KONT_KEYFRAME) {
      for(i = 0; i < KONT_MAX_ANIM; i++) {
        if(p_mesh->sim[i].keynum) {
          key_sim_root_vloz_pivoty_do_animace(p_mesh, p_mesh->sim+i);
          game_mesh_save_sim(f, p_mesh->sim+i);
        }
      }
    }
    
     Uloz svetla tohoto kontejneru
    
    game_mesh_save_static_light_mesh(f,p_mesh->p_slight,p_mat);
    game_mesh_save_dyn_light_mesh(f, p_mesh->p_dlight);
    game_mesh_save_extra_light_mesh(f, p_mesh->p_edlight);
  
     Uloz mlho-kostky
    
    game_mesh_save_mlhokostku(f, p_mesh->p_mlha, TRUE);
  
    return(i);
  }
  */
  /*
  int lo_uloz_kontejner_chunk(FHANDLE *f, EDIT_MATERIAL **p_mat, int max_mat, 
                              GAME_MESH *p_mesh, int low)
  { 
    Uloz materialy
   
   lo_uloz_materialy_chunk_kont(f, p_mat, max_mat, p_mesh, FALSE);
   
    Uloz kontejner
   
   game_mesh_save_kontejner(f, p_mesh, p_mat, max_mat, low, K_CHYBA, K_CHYBA, FALSE);
  
    Uloz mlho-kostky
   
   game_mesh_save_mlhokostku(f, p_kont->p_mlha, TRUE);
  
   return(TRUE);
  }
  */

  /*
   * Loading routines
   */
  int game_mesh_load_kont_name(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    int  i; 
    
    if(p_ch->type == CHUNK_KONT_NAME) {
      f->read(&i,sizeof(i));
      f->read_string(kont_name,MAX_NAME);
    } 
    else if(p_ch->type == CHUNK_KONT_LOW_NAME) {
      f->read_string(kont_name,MAX_NAME);
    }
    else {
      return(FALSE);
    }
    
    pprintf("Kont name %s",kont_name);
    
    mesh_new(kont_name);
    
    return(TRUE);
  }    
  
  int game_mesh_load_kont_id(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    if(p_ch->type == CHUNK_KONT_ID) {
      f->read(&kont_name_ID,sizeof(kont_name_ID),TRUE);      
      p_mesh->name_ID_set(kont_name_ID);
      return(TRUE);
    }
    else if(p_ch->type == CHUNK_KONT_LOW_ID) {
      f->read(&kont_name_ID,sizeof(kont_name_ID));      
      p_mesh->name_ID_set(kont_name_ID);
      return(TRUE);
    } else {
      return(FALSE);
    }
  }
  
  int game_mesh_load_kont_flag(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    if(p_ch->type == CHUNK_KONT_FLAG) {
      f->read(&kont_flag,sizeof(kont_flag));
      return(TRUE);
    } 
    else if(p_ch->type == CHUNK_KONT_FLAG_EXT_2) {
      f->read(&kont_flag2,sizeof(kont_flag2));
      return(TRUE);
    } 
    else if(p_ch->type == CHUNK_KONT_FLAG_EXT) {
      f->read(&kont_material_flag,sizeof(kont_material_flag));
      f->read(&kont_material_flag2,sizeof(kont_material_flag2));
      return(TRUE);
    } else
      return(FALSE);
  }
  
  int game_mesh_load_kont_obb(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    if(p_ch->type == CHUNK_KONT_OBB) {
      OBB tmp;
      
      f->read(&tmp.dir,sizeof(tmp.dir),TRUE);
      f->read(&tmp.center,sizeof(tmp.center),TRUE);
      f->read(&tmp.len,sizeof(tmp.len),TRUE);    
      f->read(&tmp.border,sizeof(tmp.border),TRUE);    
      f->seek(sizeof(VECT)*2,SEEK_CUR);
      
      p_mesh->object_box_set(&tmp);
      
      return(TRUE);
    } else
      return(FALSE);
  }
  
  int game_mesh_load_kont_matrix(FHANDLE *f, DATA_CHUNK *p_ch)
  {  
    if(p_ch->type == CHUNK_KONT_WMATRIX) {
      GLMATRIX m;
      if(f->read(&m,sizeof(m)) == sizeof(m)) {
        p_mesh->world_set(&m);
      }
      return(TRUE);
    } else {
      return(FALSE);
    }
    
    /*
    if(p_ch->type == CHUNK_KONT_MPMATRIX) {
      GLMATRIX m;
      if(f->read(&m,sizeof(m)) == sizeof(m)) {
        
      }    
      //p_kont->kflag |= KONT_MPMATRIX;
      //file_read(&p_kont->mpmatrix,sizeof(p_kont->mpmatrix),1,f);
      return(TRUE);
    } else
      return(FALSE);
    */
  }
  
  int game_mesh_load_mesh_name(FHANDLE *f, DATA_CHUNK *p_ch)
  {    
    char name[MAX_NAME] = "";
    char name_parent[MAX_NAME] = "";
    
    if(p_ch->type == CHUNK_OBJ_NAME) {
      f->read_string(name,MAX_NAME);
      strncpy(name_parent,kont_name,MAX_NAME);
    } 
    else if(p_ch->type == CHUNK_OBJ_NAME_PARENT) {
      f->read_string(name,MAX_NAME);
      f->read_string(name_parent,MAX_NAME);
    } 
    else {
      return(FALSE);
    }
        
    mesh_new(name,name_parent);    
    return(TRUE);
  }
  
  int game_mesh_load_mesh_ID(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    if(p_ch->type == CHUNK_OBJ_ID) {
      int name_ID;
      f->read(&name_ID,sizeof(name_ID),TRUE);
      p_mesh->name_ID_set(name_ID);
      return(TRUE);
    } else
      return(FALSE);
  }
  
  int game_mesh_load_mesh_pivot(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    if(p_ch->type == CHUNK_OBJ_PIVOT) {
      MESH_GEOMETRY *p_geom = p_mesh->lock(LOCK_READ_WRITE);
      f->read(p_geom->pivot_get(),sizeof(VECT),TRUE);
      p_mesh->unlock(LOCK_UNCHANGED);
      return(TRUE);
    } else
      return(FALSE);
  }
  
  int game_mesh_load_mesh_obb(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    if(p_ch->type == CHUNK_OBJ_OBB) {    
      OBB tmp;
      
      f->read(&tmp.dir,sizeof(tmp.dir),TRUE);
      f->read(&tmp.center,sizeof(tmp.center),TRUE);
      f->read(&tmp.len,sizeof(tmp.len),TRUE);
      f->read(&tmp.border,sizeof(tmp.border),TRUE);
      f->seek(sizeof(VECT)*2,SEEK_CUR);
      
      p_mesh->object_box_set(&tmp);
      
      return(TRUE);
    } else
      return(FALSE);
  }
  
  int game_mesh_load_mesh_material(FHANDLE *f, DATA_CHUNK *p_ch)
  {  
    if(p_ch->type == CHUNK_OBJ_MATERIAL) {
      char material_name[MAX_FILENAME];
      f->read_string(material_name,MAX_FILENAME);
      
      //pdebug("%s: material name '%s'",__FUNCTION__,material_name);      
      p_mat = p_scene->material_get(material_name);
      if(!p_mat) {
        pprintf("Can't find material %s",material_name);
      }
      
      return(TRUE);
    } else
      return(FALSE);
  }
  
  int game_mesh_load_mesh_matrix(FHANDLE *f, DATA_CHUNK *p_ch)
  {      
    if(p_ch->type == CHUNK_OBJ_MATRIX_KEY) {
      GLMATRIX m;
      if(f->read(&m,sizeof(m)) == sizeof(m)) {
        p_mesh->world_set(&m);
      }
      return(TRUE);
    } else
      return(FALSE);
  }
  
  int game_mesh_load_mesh_vertex(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    if(p_ch->type == CHUNK_OBJ_VERTEXES) {
      MESH_GEOMETRY *p_geom = p_mesh->lock(LOCK_READ_WRITE);
  
      int vertexes;
      f->read(&vertexes,sizeof(dword),TRUE);
      
      p_geom->vertex_new(vertexes);
          
      VECT *p_vert = p_geom->position_get();    
      int i;
      float v[3];    
      for(i = 0; i < vertexes; i++) {
        f->read(&v,sizeof(v),TRUE);
        
        p_vert[i].x = v[0];
        p_vert[i].y = v[1];
        p_vert[i].z = v[2];
      }
      
      p_mesh->unlock();
      
      return(TRUE);
    } else
      return(FALSE);
  }
  
  int game_mesh_load_mesh_normal(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    if(p_ch->type == CHUNK_OBJ_NORMAL) {    
      MESH_GEOMETRY *p_geom = p_mesh->lock(LOCK_READ_WRITE);
      
      int vertexes;
      f->read(&vertexes,sizeof(dword),TRUE);
            
      VECT *p_vert = p_geom->normal_get();
      
      int i;
      float v[3];
      
      for(i = 0; i < vertexes; i++) {
        f->read(&v,sizeof(v),TRUE);
        
        p_vert[i].x = v[0];
        p_vert[i].y = v[1];
        p_vert[i].z = v[2];
      }
      
      p_mesh->unlock();
      
      return(TRUE);
    } else
      return(FALSE);
  }
  
  
  int game_mesh_load_mesh_face(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    if(p_ch->type == CHUNK_OBJ_FACES) {
      MESH_GEOMETRY *p_geom = p_mesh->lock(LOCK_READ_WRITE);
  
      int faces;    
      f->read(&faces,sizeof(faces),TRUE);
      
      word *p_tmp = (word *)mmalloc(sizeof(word)*faces);
      f->read(p_tmp,sizeof(p_tmp[0])*faces,TRUE);
      
      p_geom->face_new(faces);
      
      int i;
      for(i = 0; i < faces; i++)
        p_geom->face_set(i,p_tmp[i]);
      
      p_mesh->unlock();
      
      return(TRUE);
    } 
    else if(p_ch->type == CHUNK_OBJ_FACES_OPT) {
      /*
      if(!file_read(&p_obj->optnum,sizeof(p_obj->optnum),1,f))
        chyba("Cteni");    
      
      p_obj->p_opt = (int *)mmalloc(sizeof(p_obj->p_opt[0])*p_obj->optnum);
      if(!file_read(p_obj->p_opt,sizeof(p_obj->p_opt[0]),p_obj->optnum,f))
        chyba("Cteni");
      */
      return(TRUE);
    } else
      return(FALSE);
  }
  
  int game_mesh_load_mesh_maps(FHANDLE *f, DATA_CHUNK *p_ch)
  {  
    int num;
    
    switch(p_ch->type) {
      case CHUNK_OBJ_MAPS_1:
        num = 0;
        break;
      case CHUNK_OBJ_MAPS_2:
        num = 1;
        break;
      case CHUNK_OBJ_MAPS_3:
        num = 2;
        break;
      case CHUNK_OBJ_MAPS_4:
        num = 3;
        break;
      default:
        return(FALSE);
    } 
    
    MESH_GEOMETRY *p_geom = p_mesh->lock(LOCK_READ_WRITE);
    
    int vertexes;
    f->read(&vertexes,sizeof(dword),TRUE);
    
    VECTUV *p_vert = p_geom->uv_get(num);  
    float m[2];
    int   i;  
    for(i = 0; i < vertexes; i++) {
      f->read(&m,sizeof(m),TRUE);
      p_vert[i].u = m[0];
      p_vert[i].v = m[1];
    }
    
    p_mesh->unlock();
    
    return(TRUE);
  }
  
  int game_mesh_load_mesh_color(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    if(p_ch->type == CHUNK_OBJ_COLOR) {
      MESH_GEOMETRY *p_geom = p_mesh->lock(LOCK_READ_WRITE);
      
      int vertexes;
      f->read(&vertexes,sizeof(dword),TRUE);
      
      RGBAF *p_vert = p_geom->diffuse_get();
      
      int i;
      float v[4];
      
      for(i = 0; i < vertexes; i++) {
        f->read(&v,sizeof(v),TRUE);
        
        p_vert[i].r = v[0];
        p_vert[i].g = v[1];
        p_vert[i].b = v[2];
      }
      
      p_mesh->unlock();
      
      return(TRUE);
    }
    if(p_ch->type == CHUNK_OBJ_COLOR_EXT) {
      MESH_GEOMETRY *p_geom = p_mesh->lock(LOCK_READ_WRITE);
      
      int vertexes;
      f->read(&vertexes,sizeof(dword),TRUE);
          
      RGBF  *p_spec = p_geom->specular_get(1);
      RGBF  *p_spec_base = p_geom->specular_get();
      RGBAF *p_diff = p_geom->diffuse_get(1);
      RGBAF *p_diff_base = p_geom->diffuse_get(0);
      
      int i;
      float v[4];
      
      for(i = 0; i < vertexes; i++) {
        f->read(&v,sizeof(v),TRUE);
        p_diff[i].r = v[0];
        p_diff[i].g = v[1];
        p_diff[i].b = v[2];
        p_diff[i].a = v[3];
  
        f->read(&v,sizeof(v),TRUE);
        p_diff_base[i].r = v[0];
        p_diff_base[i].g = v[1];
        p_diff_base[i].b = v[2];
        p_diff_base[i].a = v[3];
  
        f->read(&v,sizeof(v),TRUE);
        p_spec[i].r = v[0];
        p_spec[i].g = v[1];
        p_spec[i].b = v[2];
  
        f->read(&v,sizeof(v),TRUE);
        p_spec_base[i].r = v[0];
        p_spec_base[i].g = v[1];
        p_spec_base[i].b = v[2];
      }
      
      p_mesh->unlock();
      
      return(TRUE);
    }  
    else {
      return(FALSE);
    }  
  }
  
  /*
  JOINT * lo_najdi_joint(JOINT *p_joint, int ID)
  {
    JOINT *t1;
  
    if(!p_joint)
      return(NULL);
  
    if(p_joint->joint_ID == ID) {
      return(p_joint);
    } else {
      if((t1 = lo_najdi_joint(p_joint->p_child,ID)))
        return(t1);
      if((t1 = lo_najdi_joint(p_joint->p_next,ID)))
        return(t1);
      return(NULL);
    }
  }
  */
  int game_mesh_load_mesh_joint_ID(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    return(FALSE);
    /*
    if(p_joint_animace && p_ch->type == CHUNK_OBJ_JNT_ID) {
      JOINT *p_jnt = key_joint_vyrob();
      int    ID;
      int    prev_ID;
  
      // oflaguj kontejner
      p_kont->k2flag |= KONT2_JOINT_ANIM;
      p_kont->kflag |= KONT_VERTEX_LIGHT;
      p_kont->kflag &= ~KONT_STATIC;
  
      file_read(&ID,sizeof(ID),1,f);
      file_read(&prev_ID,sizeof(prev_ID),1,f);
      
      p_joint = p_jnt;
      if(prev_ID == ROOT_ID) {      
        if(!p_joint_animace->p_child) {
          p_joint_animace->p_child = p_joint;
        } else {
          p_joint->p_next = p_joint_animace->p_child->p_next;
          p_joint_animace->p_child->p_next = p_joint;
        }      
      } else {
        JOINT *p_prev = lo_najdi_joint(p_joint_animace->p_child,prev_ID);
        if(p_prev->p_child) {
          p_joint->p_next = p_prev->p_next;
          p_prev->p_next = p_joint;
        } else {
          p_prev->p_child = p_joint;
        }
      }
      p_joint->joint_ID = ID;
    }
    */
    return(TRUE);
  }
  
  int game_mesh_load_mesh_joint_pivot(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    return(FALSE);
    /*
    if(p_joint && p_ch->type == CHUNK_OBJ_JNT_PIVOT) {
      file_read(&p_joint->pivot,sizeof(p_joint->pivot),1,f);
    }
    */
    return(TRUE);
  }
  
  int game_mesh_load_mesh_joint_flag(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    return(FALSE);
    /*
    if(p_joint && p_ch->type == CHUNK_OBJ_JNT_FLAG) {
      file_read(&p_joint->flag,sizeof(p_joint->flag),1,f);
    }
    */
    return(TRUE);
  }
  
  int game_mesh_load_mesh_joint_vertex(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    return(FALSE);
    /*
    if(p_joint && p_ch->type == CHUNK_OBJ_JNT_VERT) {
      file_read(&p_joint->vertexnum,sizeof(p_joint->vertexnum),1,f);
      p_joint->p_vertexlist = (int *)mmalloc(sizeof(p_joint->p_vertexlist[0])*p_joint->vertexnum);
      file_read(p_joint->p_vertexlist,sizeof(p_joint->p_vertexlist[0]),p_joint->vertexnum,f);      
    }
    */
    return(TRUE);
  }
  
  int game_mesh_load_mesh_joint_pos(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    return(FALSE);
    /*
    if(p_joint && p_ch->type == CHUNK_OBJ_JNT_POS) {  
      file_read(&p_joint->pos_keys,sizeof(p_joint->pos_keys),1,f);
      p_joint->p_pos = (BOD *)mmalloc(sizeof(p_joint->p_pos[0])*p_joint->pos_keys);
      file_read(p_joint->p_pos,sizeof(p_joint->p_pos[0]),p_joint->pos_keys,f);
    }
    */
    return(TRUE);
  }
  
  int game_mesh_load_mesh_joint_rot(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    return(FALSE);
  
    /*
    if(p_joint && p_ch->type == CHUNK_OBJ_JNT_ROT) {
      file_read(&p_joint->rot_keys,sizeof(p_joint->rot_keys),1,f);
      p_joint->p_rot = (QUAT *)mmalloc(sizeof(p_joint->p_rot[0])*p_joint->rot_keys);
      file_read(p_joint->p_rot,sizeof(p_joint->p_rot[0]),p_joint->rot_keys,f);
    }
    */
    return(TRUE);
  }
  
  int game_mesh_load_mesh_joint_scs(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    return(FALSE);
    /*
    if(p_joint && p_ch->type == CHUNK_OBJ_JNT_SCS) {
      file_read(&p_joint->scs_keys,sizeof(p_joint->scs_keys),1,f);
      p_joint->p_scs = (BOD *)mmalloc(sizeof(p_joint->p_scs[0])*p_joint->scs_keys);
      file_read(p_joint->p_scs,sizeof(p_joint->p_scs[0]),p_joint->scs_keys,f);
    }
    */
    return(TRUE);
  }
  
  // Spline klice
  
  int game_mesh_load_mesh_joint_pos_spline(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    return(FALSE);
    /*
    JOINT_KEYS *p_key;
    if(p_joint && p_ch->type == CHUNK_OBJ_JNT_KEYPOS) {
      if(!p_joint->p_keys) {
        p_key = p_joint->p_keys = (JOINT_KEYS *)mmalloc(sizeof(p_joint->p_keys[0]));
      }
      p_key = p_joint->p_keys;
      file_read(&p_key->pos_keys,sizeof(p_key->pos_keys),1,f);
    
      p_key->p_pos = (BOD *)mmalloc(sizeof(p_key->p_pos[0])*p_key->pos_keys);
      file_read(p_key->p_pos,sizeof(p_key->p_pos[0]),p_key->pos_keys,f);
      
      p_key->p_pkeys = (KEY_POINT_BRS *)mmalloc(sizeof(p_key->p_pkeys[0])*p_key->pos_keys);
      file_read(p_key->p_pkeys,sizeof(p_key->p_pkeys[0]),p_key->pos_keys,f);
    }
    */
    return(TRUE);
  }
  
  int game_mesh_load_mesh_joint_rot_spline(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    return(FALSE);
    /*
    JOINT_KEYS *p_key;
    if(p_joint && p_ch->type == CHUNK_OBJ_JNT_KEYROT) {
      if(!p_joint->p_keys) {
        p_key = p_joint->p_keys = (JOINT_KEYS *)mmalloc(sizeof(p_joint->p_keys[0]));
      }
      p_key = p_joint->p_keys;
      file_read(&p_key->rot_keys,sizeof(p_key->rot_keys),1,f);
      
      p_key->p_rot = (QUAT *)mmalloc(sizeof(p_key->p_rot[0])*p_key->rot_keys);
      file_read(p_key->p_rot,sizeof(p_key->p_rot[0]),p_key->rot_keys,f);
      
      p_key->p_rkeys = (KEY_POINT_BRS *)mmalloc(sizeof(p_key->p_rkeys[0])*p_key->rot_keys);
      file_read(p_key->p_rkeys,sizeof(p_key->p_rkeys[0]),p_key->rot_keys,f);
    }
    */
    return(TRUE);
  }
  
  int game_mesh_load_mesh_joint_scs_spline(FHANDLE *f, DATA_CHUNK *p_ch)
  {  
    return(FALSE);
    /*
    JOINT_KEYS *p_key;
    if(p_joint && p_ch->type == CHUNK_OBJ_JNT_KEYSCS) {
      if(!p_joint->p_keys) {
        p_key = p_joint->p_keys = (JOINT_KEYS *)mmalloc(sizeof(p_joint->p_keys[0]));
      }
      p_key = p_joint->p_keys;
      file_read(&p_key->scs_keys,sizeof(p_key->scs_keys),1,f);
      
      p_key->p_scale = (BOD *)mmalloc(sizeof(p_key->p_scale[0])*p_key->scs_keys);
      file_read(p_key->p_scale,sizeof(p_key->p_scale[0]),p_key->scs_keys,f);
      
      p_key->p_skeys = (KEY_POINT_BRS *)mmalloc(sizeof(p_key->p_skeys[0])*p_key->scs_keys);
      file_read(p_key->p_skeys,sizeof(p_key->p_skeys[0]),p_key->scs_keys,f);
    }
    */
    return(TRUE);
  }
  
  int game_mesh_load_mesh_joint_anim(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    return(FALSE);
    /*
    if(p_ch->type == CHUNK_OBJ_JA) {
      JOINT_ANIMACE *p_anim = (JOINT_ANIMACE *)mmalloc(sizeof(p_anim[0]));
      
      str_read(p_anim->name,f);
      
      p_anim->p_next = p_joint_animace;
      p_joint_animace = p_anim;
      
      p_joint_animace->p_next = p_obj->p_joit_animace;
      p_obj->p_joit_animace = p_joint_animace;
    }
    */
    return(TRUE);
  }
  
  int game_mesh_load_mesh_joint_frames(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    return(FALSE);
    /*
    if(p_joint_animace && p_ch->type == CHUNK_OBJ_JA_FRAME_NUM) {  
      file_read(&p_joint_animace->framenum,sizeof(p_joint_animace->framenum),1,f);
    }
    */
    return(TRUE);
  }
  
  static CHUNK_LOADER game_mesh_loaders[];
  
// *************************************************************************
//  Load/Save routines - Materials
// *************************************************************************

private:

  MESH_MATERIAL   *p_mat;

private:
  
  MESH_MATERIAL * material_new(void)
  {    
    p_mat = p_scene->material_create();
    //p_mat->mark_set(MARK_LOADED);
    return(p_mat);
  }

private:
  
  /*
   * Material Load/save Chunks
   */
  #define  CHUNK_MATERIAL        10  // Material name
  #define  CHUNK_COLOR           11  // Colors
  #define  CHUNK_MATANIM         12  // Material animations
  #define  CHUNK_STAG_TAG        13  // stage tagy (cislo stagu + jejich cisla)
  #define  CHUNK_COLOR_EXT       20  // extended colors
  #define  CHUNK_SHINE           21  // sila odlesku
  #define  CHUNK_FLAGS           22  // flagy materialu
  #define  CHUNK_ENVSPEC         26  // spec-env udaje
  #define  CHUNK_ALFA_STAGE      27  // alfa nastavenni materialu (pruhledny,pouzity,src,dest)
  #define  CHUNK_TEXT_STAGE_1    28  // multi tag, pred texturou
  #define  CHUNK_TEXT_STAGE_2    29  // multi tag, pred texturou
  #define  CHUNK_TEXT_STAGE_3    30  // multi tag, pred texturou
  #define  CHUNK_TEXT_STAGE_4    31  // multi tag, pred texturou
  #define  CHUNK_TEXT_1          16  // textura, k multi tagu
  #define  CHUNK_TEXT_2          18  // textura, k multi tagu
  #define  CHUNK_TEXT_3          32  // textura, k multi tagu
  #define  CHUNK_TEXT_4          33  // textura, k multi tagu
  #define  CHUNK_MATANIM_T1      34  // animace textur 1
  #define  CHUNK_MATANIM_T2      35  // animace textur 2
  #define  CHUNK_MATANIM_T3      36  // animace textur 3
  #define  CHUNK_MATANIM_T4      37  // animace textur 4
  
  #define  CHUNK_MATMATRIX_EXT   50  // animacni matice textury 1
  
  #define  CHUNK_TEXT_STAGE_AD_1 51  // multi tag, pred texturou
  #define  CHUNK_TEXT_STAGE_AD_2 52  // multi tag, pred texturou
  #define  CHUNK_TEXT_STAGE_AD_3 53  // multi tag, pred texturou
  #define  CHUNK_TEXT_STAGE_AD_4 54  // multi tag, pred texturou
  
  #define  CHUNK_TEXT_FLAG_1     55  // textura, k multi tagu
  #define  CHUNK_TEXT_FLAG_2     56  // textura, k multi tagu
  #define  CHUNK_TEXT_FLAG_3     57  // textura, k multi tagu
  #define  CHUNK_TEXT_FLAG_4     58  // textura, k multi tagu
  
  #define  CHUNK_MATANIM_EXT     60  // animace materialu
  #define  CHUNK_MATANIM_FRAME   61  // frame materialu
  #define  CHUNK_MAT_SMAT_POS    62  // pozice s-materialu
  #define  CHUNK_MAT_TEXGEN      63  // pozice s-materialu
  #define  CHUNK_MATANIM_EFRAME  64  // frame materialu
  
  
  /*
   * Material save routines
   */  
  int mesh_material_save_name(FHANDLE *f, MESH_MATERIAL *p_mat)
  {
    DATA_CHUNK ch;
    
    ch.type = CHUNK_MATERIAL;
    ch.size = sizeof(ch)+sizeof(char)*(strlen(p_mat->name_get())+1);
    
    f->write(&ch,sizeof(ch));
    f->write_string(p_mat->name_get());
    
    return(TRUE);
  }
  
  int mesh_material_save_flags(FHANDLE *f, MESH_MATERIAL *p_mat)
  {
    DATA_CHUNK ch;  
    int        flag = 0,
               flag2 = 0;
    
    ch.type = CHUNK_FLAGS;
    ch.size = sizeof(ch)+sizeof(flag)+sizeof(flag2);
    
    f->write(&ch,sizeof(ch));
    f->write(&flag,sizeof(flag));
    f->write(&flag2,sizeof(flag2));
    
    return(TRUE);
  }
  
  /* TODO
  int mesh_material_save_smat_pos(FHANDLE *f, MESH_MATERIAL *p_mat)
  {    
    DATA_CHUNK ch;  
  
    if(flag&MAT_SCENE) {
      ch.type = CHUNK_MAT_SMAT_POS;
      ch.size = sizeof(ch)+sizeof(smaterial_pozice);
      f->write(&ch,sizeof(ch));
      f->write(&smaterial_pozice,sizeof(smaterial_pozice));
    }
  }
  */
  
  int mesh_material_save_color(FHANDLE *f, MESH_MATERIAL *p_mat)
  {  
    char        string[16];
    DATA_CHUNK  ch;
    
    ch.type = CHUNK_COLOR_EXT;
    ch.size = sizeof(ch)+16;
    
    memset(string,0,sizeof(string));
    
    RGBAB diff(p_mat->diffuse_get());
    string[4] = diff.r;
    string[5] = diff.g;
    string[6] = diff.b;
    string[7] = diff.a;
  
    RGBB spec(p_mat->specular_get());
    string[8] =  spec.r;
    string[9] =  spec.g;
    string[10] = spec.b;
    string[11] = MAX_BYTE;
      
    f->write(&ch,sizeof(ch));
    f->write(string,sizeof(char)*16);
    
    return(TRUE);
  }
  
  /* TODO
  int mesh_material_save_matanim_frame(FHANDLE *f, ANIM_FRAME *p_fram, int frame)
  {  
    EDIT_TEXT *p_text;
    DATA_CHUNK  ch;
    int        flag;
  
    if(p_fram) {
      p_text = p_fram->p_text;
      ch.type = CHUNK_MATANIM_EFRAME;
      ch.size = sizeof(ch)+strlen(p_text->name)+1+sizeof(p_text->flag)
                   +sizeof(p_fram->time)+sizeof(p_fram->alfa)+sizeof(p_fram->u)
                   +sizeof(p_fram->v)+sizeof(p_fram->flag)+sizeof(frame);
  
      f->write(&ch,sizeof(ch));
      f->write(&frame,sizeof(frame));
      f->write(&p_fram->time,sizeof(p_fram->time));
      f->write(&p_fram->alfa,sizeof(p_fram->alfa));
      f->write(&p_fram->u,sizeof(p_fram->u));
      f->write(&p_fram->v,sizeof(p_fram->v));
      f->write(&p_fram->flag,sizeof(p_fram->flag));
  
      //Prdy z vody - ulozim rovnou texturu
      p_text = p_fram->p_text;
      flag = p_text->flag;
      f->write(&p_text->name,sizeof(char),strlen(p_text->name)+1,f);
      f->write(&flag,sizeof(flag));
      
      return(TRUE);
    } else
      return(FALSE);
  }
  
  int mesh_material_save_matanim(FHANDLE *f, MESH_MATERIAL *p_mat)
  {  
    ANIM_MATERIAL *p_anim = &anim;
    DATA_CHUNK ch;  
    int i;
    
    if(flag&MAT_ANIM_FRAME) {
      
      ch.type = CHUNK_MATANIM_EXT;
      ch.size = sizeof(ch)+strlen(anim.name)+1+sizeof(p_anim->framenum);    
      
      f->write(&ch,sizeof(ch));
      f->write(p_anim->name,sizeof(char),strlen(p_anim->name)+1,f);
      f->write(&p_anim->framenum,sizeof(p_anim->framenum));        
  
      for(i = 0; i < p_anim->framenum; i++) {      
        save_mat_matanim_frame(f, p_anim->p_frame+i, i);
      }
      
      return(TRUE);
    } else
      return(FALSE); 
  }
  
  int mesh_material_save_matanim_text_dal(FHANDLE *f, ANIM_TEXT *p_track, int typ)
  {  
    DATA_CHUNK ch;
    int       velikost = 0;
    if(p_track->pos_keys && p_track->p_pos) {
      velikost += sizeof(p_track->p_pos[0])*p_track->pos_keys+
                  sizeof(p_track->p_pkeys[0])*p_track->pos_keys;
    }  
    if(p_track->piv_keys && p_track->p_piv) {
      velikost += sizeof(p_track->p_piv[0])*p_track->piv_keys+
                  sizeof(p_track->p_vkeys[0])*p_track->piv_keys;
    }  
    if(p_track->rot_keys && p_track->p_rot) {
      velikost += sizeof(p_track->p_rot[0])*p_track->rot_keys+
                  sizeof(p_track->p_rkeys[0])*p_track->rot_keys;
    }  
    if(p_track->scs_keys && p_track->p_scale) {
      velikost += sizeof(p_track->p_scale[0])*p_track->scs_keys+
                  sizeof(p_track->p_skeys[0])*p_track->scs_keys;
    }
    ch.type = typ;  
    ch.size = sizeof(ch)+sizeof(p_track[0])+velikost;
    f->write(&ch,sizeof(ch));
    f->write(p_track,sizeof(p_track[0]));  
    if(p_track->pos_keys && p_track->p_pos) {
      f->write(p_track->p_pos,sizeof(p_track->p_pos[0]),p_track->pos_keys,f);
      f->write(p_track->p_pkeys,sizeof(p_track->p_pkeys[0]),p_track->pos_keys,f);
    }  
    if(p_track->piv_keys && p_track->p_piv) {
      f->write(p_track->p_piv,sizeof(p_track->p_piv[0]),p_track->piv_keys,f);
      f->write(p_track->p_vkeys,sizeof(p_track->p_vkeys[0]),p_track->piv_keys,f);
    }  
    if(p_track->rot_keys && p_track->p_rot) {
      f->write(p_track->p_rot,sizeof(p_track->p_rot[0]),p_track->rot_keys,f);
      f->write(p_track->p_rkeys,sizeof(p_track->p_rkeys[0]),p_track->rot_keys,f);
    }  
    if(p_track->scs_keys && p_track->p_scale) {
      f->write(p_track->p_scale,sizeof(p_track->p_scale[0]),p_track->scs_keys,f);
      f->write(p_track->p_skeys,sizeof(p_track->p_skeys[0]),p_track->scs_keys,f);
    }
    return(TRUE);  
  }
  
  int mesh_material_save_matanim_text(FHANDLE *f, MESH_MATERIAL *p_mat)
  {
    int i,j;
    for(j = 0; j < MAT_TEXTUR; j++) {
      if(flag&glstav_posun_flagy2[j]) {
        for(i = 0; i < MAX_TEXT_ANIM; i++) {
          if(p_atext[j][i]) {
            save_mat_matanim_text_dal(f, p_atext[j][i],CHUNK_MATANIM_T1+j);
          }
        }
      }  
    }
    return(TRUE);
  }
  */
  
  int mesh_material_save_textmatrix(FHANDLE *f, MESH_MATERIAL *p_mat)
  {
    DATA_CHUNK ch;  
    ch.type = CHUNK_MATMATRIX_EXT;
    ch.size = sizeof(ch)+4*sizeof(GLMATRIX);
    f->write(&ch,sizeof(ch));    
    f->write(p_mat->text_coordinates_get(0)->text_matrix_get(),sizeof(GLMATRIX));
    f->write(p_mat->text_coordinates_get(1)->text_matrix_get(),sizeof(GLMATRIX));
    f->write(p_mat->text_coordinates_get(2)->text_matrix_get(),sizeof(GLMATRIX));
    f->write(p_mat->text_coordinates_get(3)->text_matrix_get(),sizeof(GLMATRIX));
    return(TRUE);
  }
  
  int mesh_material_save_texgen_param(FHANDLE *f, MESH_MATERIAL *p_mat)
  {
    DATA_CHUNK ch;  
    ch.type = CHUNK_MAT_TEXGEN;
    ch.size = sizeof(ch)+4*(sizeof(float)*4);
    f->write(&ch,sizeof(ch));
    
    /* TODO
    f->write(texgen_koord_t,sizeof(texgen_koord_t));
    f->write(texgen_koord_s,sizeof(texgen_koord_s));
    f->write(texgen_koord_r,sizeof(texgen_koord_r));
    f->write(texgen_koord_q,sizeof(texgen_koord_q));
    */
    
    float t[4];
    f->write(&t,sizeof(t));
    f->write(&t,sizeof(t));
    f->write(&t,sizeof(t));
    f->write(&t,sizeof(t));
    
    return(TRUE);
  }
  
  int mesh_material_save_blend(FHANDLE *f, MESH_MATERIAL *p_mat)
  {
    DATA_CHUNK ch;
    
    ch.type = CHUNK_ALFA_STAGE;
    ch.size = sizeof(ch)+sizeof(int);
    
    int state = p_mat->alpha_func_get();
    
    f->write(&ch,sizeof(ch));
    f->write(&state,sizeof(int));
    
    return(TRUE);
  }
  /* TODO
  int mesh_material_save_env_spec(FHANDLE *f, MESH_MATERIAL *p_mat)
  {
    DATA_CHUNK ch;
    
    ch.type = CHUNK_ENVSPEC;
    ch.size = sizeof(ch)+
              sizeof(env_add1)+
              sizeof(env_add2)+
              sizeof(env_scale)+
              sizeof(env_r)+
              sizeof(env_g)+
              sizeof(env_b);
    
    f->write(&ch,sizeof(ch));
    f->write(&env_add1,sizeof(env_add1));
    f->write(&env_add2,sizeof(env_add2));
    f->write(&env_scale,sizeof(env_scale));
    f->write(&env_r,sizeof(env_r));
    f->write(&env_g,sizeof(env_g));
    f->write(&env_b,sizeof(env_b));
    
    return(TRUE);
  }
  */
  
  int mesh_material_save_text_blend_ext(FHANDLE *f, MESH_MATERIAL *p_mat, int num)
  {
    DATA_CHUNK ch;  
    ch.type = CHUNK_TEXT_STAGE_AD_1+num;
    ch.size = sizeof(ch)+
              sizeof(int)*1+
              sizeof(int)*4+
              sizeof(int)*4+
              sizeof(int)*4+
              sizeof(float)*4;
    
    f->write(&ch,sizeof(ch));
    
    MATERIAL_TEXT_CONFIG *p_conf = p_mat->text_config_get(num);
    
    f->write(&p_conf->text_operator,sizeof(int));
    
    int tmp[4] = {0,0,0,0};
    tmp[0] = p_conf->text_index;
    f->write(&tmp,sizeof(int)*4,f);
    
    tmp[0] = p_conf->text_coordinates;
    f->write(&tmp,sizeof(int)*4,f);
    
    tmp[0] = 0;
    f->write(&tmp,sizeof(int)*4,f);
        
    float t[4] = {0,0,0,0};
    f->write(&t,sizeof(float)*4,f);
    
    return(TRUE);
  }  
  
  int mesh_material_save_text(FHANDLE *f, MESH_MATERIAL *p_mat, int num, int type)
  {
    DATA_CHUNK ch;
    
    MATERIAL_TEXT *p_text = p_mat->texture_get(num);
    if(p_text) {
      ch.type = type;
      ch.size = sizeof(ch)+strlen(p_text->name_get())+1;
      
      f->write(&ch,sizeof(ch));
      f->write_string(p_text->name_get());
      return(TRUE);
    } else
      return(FALSE);
  }
  
  int mesh_material_save_text_flag(FHANDLE *f, MESH_MATERIAL *p_mat, int num, int type)
  {
    DATA_CHUNK ch;
    if(p_mat) {
      ch.type = type;
      ch.size = sizeof(ch)+sizeof(int);
      
      int tmp = 0;
      f->write(&ch,sizeof(ch));      
      f->write(&tmp,sizeof(tmp));
      return(TRUE);
    } else
      return(FALSE);
  }
  
  int mesh_material_save_shine(FHANDLE *f, MESH_MATERIAL *p_mat)
  {
    DATA_CHUNK ch;  
    
    ch.type = CHUNK_SHINE;
    ch.size = sizeof(ch)+sizeof(float);
    
    f->write(&ch,sizeof(ch));
    float shine = p_mat->shine_get();
    f->write(&shine,sizeof(float));
    
    return(TRUE);
  }
  
  // *************************************************************************
  // Saves complete material to file
  // *************************************************************************
  
  bool mesh_material_save(FHANDLE *f, MESH_MATERIAL *p_mat)
  {
    pdebug(DEBUG_LOAD_CHUNK,"Saving material %s",p_mat->name_get());
    
    mesh_material_save_name(f,p_mat);
    mesh_material_save_flags(f,p_mat);
    mesh_material_save_color(f,p_mat);
    
    //mesh_material_save_matanim(f);
    //mesh_material_save_matanim_text(f);
    //mesh_material_save_textmatrix(f);
    //mesh_material_save_texgen_param(f);
    //mesh_material_save_blend(f);
    //mesh_material_save_env_spec(f);
    //mesh_material_save_smat_pos(f,p_mat);
  
    
    int i; 
    int text_flags[] = 
    {
      CHUNK_TEXT_FLAG_1,
      CHUNK_TEXT_FLAG_2,
      CHUNK_TEXT_FLAG_3,
      CHUNK_TEXT_FLAG_4
    };
    
    int text_nums[] = 
    {
      CHUNK_TEXT_1,
      CHUNK_TEXT_2,
      CHUNK_TEXT_3,
      CHUNK_TEXT_4
    };
    
    for(i = 0; i < 4; i++) {    
      mesh_material_save_text_blend_ext(f, p_mat, i);
      mesh_material_save_text_flag(f, p_mat, i, text_flags[i]);
      mesh_material_save_text(f, p_mat, i, text_nums[i]);
    }
        
    return(TRUE);
  }

  // *************************************************************************
  // Material loading
  // *************************************************************************

  int mesh_material_load_mat(FHANDLE *f, DATA_CHUNK *p_ch)
  {  
    if(p_ch->type == CHUNK_MATERIAL) {
      char name[MAX_NAME];
      f->read_string(name,MAX_NAME);
      
      material_new();
      p_mat->name_set(name);
      
      pprintf("Material name %s",name);
      return(TRUE);
    } else
      return(FALSE);
  }
  
  int mesh_material_load_mat_flags(FHANDLE *f, DATA_CHUNK *p_ch)
  {      
    if(p_ch->type == CHUNK_FLAGS) {
      int flag, flag2;
      f->read(&flag,sizeof(flag));
      f->read(&flag2,sizeof(flag2));
      return(TRUE);
    } else
      return(FALSE);
  }
  
  int mesh_material_load_mat_smat_pos(FHANDLE *f, DATA_CHUNK *p_ch)
  { 
    return(chunk_skip(f,p_ch));  
    /*     
    if(p_ch->type == CHUNK_MAT_SMAT_POS) {
      f->read(&smaterial_pozice,sizeof(smaterial_pozice));
      return(TRUE);
    } else
    */
      return(FALSE);
  }
  
  int mesh_material_load_color(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    if(p_ch->type == CHUNK_COLOR) {
      char string[16];
      
      f->read(string,sizeof(char)*16);

      RGBAF diff(string[3],string[4],string[5],(byte)MAX_BYTE);
      RGBF  spec(string[6],string[7],string[8]);

      p_mat->diffuse_set(&diff);
      p_mat->specular_set(&spec);
      
      return(TRUE);
    } else if (p_ch->type == CHUNK_COLOR_EXT) {
      char string[16];
      
      f->read(string,sizeof(byte)*16);
  /*
      p_dxmat->ambient_r = (float)string[0]/MAX_BYTE;
      p_dxmat->ambient_g = (float)string[1]/MAX_BYTE;
      p_dxmat->ambient_b = (float)string[2]/MAX_BYTE;
      p_dxmat->ambient_a = (float)string[3]/MAX_BYTE;
  */
      
      RGBAF diff(string[3],string[4],string[5],(byte)MAX_BYTE);
      RGBF  spec(string[6],string[7],string[8]);
      
      p_mat->diffuse_set(&diff);
      p_mat->specular_set(&spec);
      
  /*
      p_dxmat->faktor_r = (float)string[12]/MAX_BYTE;
      p_dxmat->faktor_g = (float)string[13]/MAX_BYTE;
      p_dxmat->faktor_b = (float)string[14]/MAX_BYTE;
      p_dxmat->faktor_a = (float)string[15]/MAX_BYTE;
  */
      return(TRUE);
    } else {
      return(FALSE);
    }
  }
  
  int mesh_material_load_matanim_frame(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    return(chunk_skip(f,p_ch));  
    
    /*
    ANIM_FRAME *p_fram;
    int frame,t;
    int flag = 0;
  
    if(p_fanim && p_ch->type == CHUNK_MATANIM_FRAME) {
      file_read(&frame,sizeof(frame));
      assert(frame < p_fanim->framenum);
      p_fram = p_fanim->p_frame+frame;    
      str_read(p_fram->file,f);
      file_read(&p_fram->time,sizeof(p_fram->time));
      file_read(&p_fram->alfa,sizeof(p_fram->alfa));
      file_read(&p_fram->u,sizeof(p_fram->u));
      file_read(&p_fram->v,sizeof(p_fram->v));
      file_read(&p_fram->flag,sizeof(p_fram->flag));
      
      if((t = lo_najdi_texturu(p_textlist,textlistnum,p_fram->file,flag)) == ERROR) {
        if((t = lo_najdi_volnou_texturu(p_textlist,textlistnum)) == ERROR)
          chyba("Neni volna textura !");      
        strcpy(p_textlist[t].name, p_fram->file);
        p_textlist[t].flag = flag;
      }
      p_fram->p_text = p_textlist+t;
  
      return(TRUE);
    } else
    */
      return(FALSE);
  }
  
  int mesh_material_load_matanim_frame_ext(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    return(chunk_skip(f,p_ch));  
    
    /*
    ANIM_FRAME *p_fram;
    int frame,t;
    int flag = 0;
  
    if(p_fanim && p_ch->type == CHUNK_MATANIM_EFRAME) {
      file_read(&frame,sizeof(frame));
      assert(frame < p_fanim->framenum);
      p_fram = p_fanim->p_frame+frame;
      file_read(&p_fram->time,sizeof(p_fram->time));
      file_read(&p_fram->alfa,sizeof(p_fram->alfa));
      file_read(&p_fram->u,sizeof(p_fram->u));
      file_read(&p_fram->v,sizeof(p_fram->v));
      file_read(&p_fram->flag,sizeof(p_fram->flag));
  
      // Load name a flag textury
      str_read(p_fram->file,f);
      file_read(&flag,sizeof(flag));
      
      if((t = lo_najdi_texturu(p_textlist,textlistnum,p_fram->file,flag))==ERROR) {
        if((t = lo_najdi_volnou_texturu(p_textlist,textlistnum)) == ERROR)
          chyba("Neni volna textura !");
        strcpy(p_textlist[t].name, p_fram->file);
        p_textlist[t].flag = flag;
      }
      p_fram->p_text = p_textlist+t;
  
      return(TRUE);
    } else
      */
      return(FALSE);
  }
  
  
  int mesh_material_load_matanim(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    return(chunk_skip(f,p_ch));  
    
    /*
    if(p_ch->type == CHUNK_MATANIM_EXT) {
      p_fanim = &anim;
      flag |= MAT_ANIM_FRAME;
      str_read(p_fanim->name,f);
      file_read(&p_fanim->framenum,sizeof(p_fanim->framenum));
      p_fanim->p_frame = (ANIM_FRAME *)mmalloc(sizeof(p_fanim->p_frame[0])*p_fanim->framenum);
      return(TRUE);
    } else
    */
      return(FALSE);
  }
  
  int mesh_material_load_textmatrix(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    if(p_ch->type == CHUNK_MATMATRIX_EXT) {
      MATERIAL_TEXT_COORDINATES *p_coord;
      
      p_coord = p_mat->text_coordinates_get(0);
      f->read(p_coord->text_matrix_get(),sizeof(GLMATRIX));
      
      p_coord = p_mat->text_coordinates_get(1);
      f->read(p_coord->text_matrix_get(),sizeof(GLMATRIX));
      
      p_coord = p_mat->text_coordinates_get(2);
      f->read(p_coord->text_matrix_get(),sizeof(GLMATRIX));
      
      p_coord = p_mat->text_coordinates_get(3);
      f->read(p_coord->text_matrix_get(),sizeof(GLMATRIX));
      
      return(TRUE);
    } else {
      return(FALSE);
    }
  }
  
  int mesh_material_load_mat_texgen_param(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    return(chunk_skip(f,p_ch));  
    
    /*
    if(p_ch->type == CHUNK_MAT_TEXGEN) {  
      f->read(texgen_koord_t,sizeof(texgen_koord_t));
      f->read(texgen_koord_s,sizeof(texgen_koord_s));
      f->read(texgen_koord_r,sizeof(texgen_koord_r));
      f->read(texgen_koord_q,sizeof(texgen_koord_q));
      return(TRUE);
    } else 
    */
      return(FALSE);  
  }
  
  int mesh_material_load_matanim_text_1(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    return(chunk_skip(f,p_ch));  
    
    /*
    ANIM_TEXT tmp,*p_track;
    int i;
    if(p_ch->type == CHUNK_MATANIM_T1) {
      flag |= MAT2_T1_MATRIX;
      for(i = 0; i < MAX_TEXT_ANIM; i++) {
        if(!p_atext[0][i])
          break;
      }
      assert(i < MAX_TEXT_ANIM);
    
      atextnum[0] = i+1;
      f->read(&tmp,sizeof(tmp));
      
      p_track = p_atext[0][i] = key_vyrob_material_animace(tmp.pos_keys,tmp.rot_keys,tmp.scs_keys,tmp.piv_keys);
      p_track->flag = tmp.flag;          // flagy animace (loop a pod)
      p_track->endtime = tmp.endtime;  // framenum = 0 - volna animace
      
      if(p_track->pos_keys) {
        f->read(p_track->p_pos,sizeof(p_track->p_pos[0]),p_track->pos_keys,f);
        f->read(p_track->p_pkeys,sizeof(p_track->p_pkeys[0]),p_track->pos_keys,f);
      }  
      
      if(p_track->piv_keys) {
        f->read(p_track->p_piv,sizeof(p_track->p_piv[0]),p_track->piv_keys,f);
        f->read(p_track->p_vkeys,sizeof(p_track->p_vkeys[0]),p_track->piv_keys,f);
      }  
  
      if(p_track->rot_keys) {
        f->read(p_track->p_rot,sizeof(p_track->p_rot[0]),p_track->rot_keys,f);
        f->read(p_track->p_rkeys,sizeof(p_track->p_rkeys[0]),p_track->rot_keys,f);
      }  
      
      if(p_track->scs_keys) {
        f->read(p_track->p_scale,sizeof(p_track->p_scale[0]),p_track->scs_keys,f);
        f->read(p_track->p_skeys,sizeof(p_track->p_skeys[0]),p_track->scs_keys,f);
      }
      return(TRUE);
    } else
    */
      return(FALSE);
  }
  
  int mesh_material_load_matanim_text_2(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    return(chunk_skip(f,p_ch));  
  
    /*
    ANIM_TEXT tmp,*p_track;
    int i;
    
    if(p_ch->type == CHUNK_MATANIM_T2) {
      flag |= MAT2_T2_MATRIX;
      for(i = 0; i < MAX_TEXT_ANIM; i++) {
        if(!p_atext[1][i])
          break;
      }
      assert(i < MAX_TEXT_ANIM);
      
      atextnum[1] = i+1;
      f->read(&tmp,sizeof(tmp));
      
      p_track = p_atext[1][i] = key_vyrob_material_animace(tmp.pos_keys,tmp.rot_keys,tmp.scs_keys,tmp.piv_keys);
      p_track->flag = tmp.flag;          // flagy animace (loop a pod)
      p_track->endtime = tmp.endtime;  // framenum = 0 - volna animace
      
      if(p_track->pos_keys) {
        f->read(p_track->p_pos,sizeof(p_track->p_pos[0]),p_track->pos_keys,f);
        f->read(p_track->p_pkeys,sizeof(p_track->p_pkeys[0]),p_track->pos_keys,f);
      }  
  
      if(p_track->piv_keys) {
        f->read(p_track->p_piv,sizeof(p_track->p_piv[0]),p_track->piv_keys,f);
        f->read(p_track->p_vkeys,sizeof(p_track->p_vkeys[0]),p_track->piv_keys,f);
      }  
      
      if(p_track->rot_keys) {
        f->read(p_track->p_rot,sizeof(p_track->p_rot[0]),p_track->rot_keys,f);
        f->read(p_track->p_rkeys,sizeof(p_track->p_rkeys[0]),p_track->rot_keys,f);
      }  
      
      if(p_track->scs_keys) {
        f->read(p_track->p_scale,sizeof(p_track->p_scale[0]),p_track->scs_keys,f);
        f->read(p_track->p_skeys,sizeof(p_track->p_skeys[0]),p_track->scs_keys,f);
      }
      return(TRUE);
    } else
    */
      return(FALSE);
  }
  
  int mesh_material_load_matanim_text_3(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    return(chunk_skip(f,p_ch));  
    
    /*
    ANIM_TEXT tmp,*p_track;
    int i;
    
    if(p_ch->type == CHUNK_MATANIM_T3) {
      flag |= MAT2_T3_MATRIX;
      for(i = 0; i < MAX_TEXT_ANIM; i++) {
        if(!p_atext[2][i])
          break;
      }
      assert(i < MAX_TEXT_ANIM);
      
      atextnum[2] = i+1;
      f->read(&tmp,sizeof(tmp));
      
      p_track = p_atext[2][i] = key_vyrob_material_animace(tmp.pos_keys,tmp.rot_keys,tmp.scs_keys,tmp.piv_keys);
      p_track->flag = tmp.flag;          // flagy animace (loop a pod)
      p_track->endtime = tmp.endtime;  // framenum = 0 - volna animace
      
      if(p_track->pos_keys) {
        f->read(p_track->p_pos,sizeof(p_track->p_pos[0]),p_track->pos_keys,f);
        f->read(p_track->p_pkeys,sizeof(p_track->p_pkeys[0]),p_track->pos_keys,f);
      }  
  
      if(p_track->piv_keys) {
        f->read(p_track->p_piv,sizeof(p_track->p_piv[0]),p_track->piv_keys,f);
        f->read(p_track->p_vkeys,sizeof(p_track->p_vkeys[0]),p_track->piv_keys,f);
      }  
      
      if(p_track->rot_keys) {
        f->read(p_track->p_rot,sizeof(p_track->p_rot[0]),p_track->rot_keys,f);
        f->read(p_track->p_rkeys,sizeof(p_track->p_rkeys[0]),p_track->rot_keys,f);
      }  
      
      if(p_track->scs_keys) {
        f->read(p_track->p_scale,sizeof(p_track->p_scale[0]),p_track->scs_keys,f);
        f->read(p_track->p_skeys,sizeof(p_track->p_skeys[0]),p_track->scs_keys,f);
      }
      return(TRUE);
    } else
    */
      return(FALSE);
  }
  
  int mesh_material_load_matanim_text_4(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    return(chunk_skip(f,p_ch));  
    
    /*
    ANIM_TEXT tmp,*p_track;
    int i;
    
    if(p_ch->type == CHUNK_MATANIM_T4) {
      flag |= MAT2_T4_MATRIX;
      for(i = 0; i < MAX_TEXT_ANIM; i++) {
        if(!p_atext[3][i])
          break;
      }
      assert(i < MAX_TEXT_ANIM);
      
      atextnum[3] = i+1;
      f->read(&tmp,sizeof(tmp));
      
      p_track = p_atext[3][i] = key_vyrob_material_animace(tmp.pos_keys,tmp.rot_keys,tmp.scs_keys,tmp.piv_keys);
      p_track->flag = tmp.flag;          // flagy animace (loop a pod)
      p_track->endtime = tmp.endtime;  // framenum = 0 - volna animace
      
      if(p_track->pos_keys) {
        f->read(p_track->p_pos,sizeof(p_track->p_pos[0]),p_track->pos_keys,f);
        f->read(p_track->p_pkeys,sizeof(p_track->p_pkeys[0]),p_track->pos_keys,f);
      }  
  
      if(p_track->piv_keys) {
        f->read(p_track->p_piv,sizeof(p_track->p_piv[0]),p_track->piv_keys,f);
        f->read(p_track->p_vkeys,sizeof(p_track->p_vkeys[0]),p_track->piv_keys,f);
      }  
      
      if(p_track->rot_keys) {
        f->read(p_track->p_rot,sizeof(p_track->p_rot[0]),p_track->rot_keys,f);
        f->read(p_track->p_rkeys,sizeof(p_track->p_rkeys[0]),p_track->rot_keys,f);
      }  
      
      if(p_track->scs_keys) {
        f->read(p_track->p_scale,sizeof(p_track->p_scale[0]),p_track->scs_keys,f);
        f->read(p_track->p_skeys,sizeof(p_track->p_skeys[0]),p_track->scs_keys,f);
      }
      return(TRUE);
    } else
    */
      return(FALSE);
  }
  
  int mesh_material_load_mat_shine(FHANDLE *f, DATA_CHUNK *p_ch)
  {  
    if(p_ch->type == CHUNK_SHINE) {
      float shine;
      f->read(&shine,sizeof(float));
      p_mat->shine_set(shine);
      return(TRUE);
    } else {
      return(FALSE);
    }
  }
  
  int mesh_material_load_stage(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    if(p_ch->type == CHUNK_STAG_TAG) {
      int alpha;
      int text_stage_1;
      int text_stage_2;
      
      f->read(&alpha,sizeof(int));
      f->read(&text_stage_1,sizeof(int));
      f->read(&text_stage_2,sizeof(int));
      
      MATERIAL_TEXT_CONFIG *p_config = p_mat->text_config_get(0);
      p_config->active = TRUE;
      p_config->text_index = 0;
      p_config->text_operator = text_stage_1;
      p_config->text_coordinates = 0;
      
      p_config = p_mat->text_config_get(1);
      p_config->active = FALSE;
      p_config->text_index = 1;
      p_config->text_operator = text_stage_2;
      p_config->text_coordinates = 1;
          
      return(TRUE);
    } else
      return(FALSE);
  }
  
  int mesh_material_load_mat_alfa_stage(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    if(p_ch->type == CHUNK_ALFA_STAGE) {
      int alpha;
      f->read(&alpha,sizeof(int));
      return(TRUE);
    } else {
      return(FALSE);
    }
  }
  
  int mesh_material_load_mat_text_stage_1(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    if(p_ch->type == CHUNK_TEXT_STAGE_1) {    
      int text_stage_1;
      
      f->read(&text_stage_1,sizeof(int));
      
      MATERIAL_TEXT_CONFIG *p_config = p_mat->text_config_get(0);
      p_config->active = TRUE;
      p_config->text_index = 0;
      p_config->text_operator = text_stage_1;
      p_config->text_coordinates = 0;
      return(TRUE);
    } else {    
      return(FALSE);
    }
  }
  
  int mesh_material_load_mat_text_stage_2(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    if(p_ch->type == CHUNK_TEXT_STAGE_2) {
      int text_stage_2;
      
      f->read(&text_stage_2,sizeof(int));
  
      MATERIAL_TEXT_CONFIG *p_config = p_mat->text_config_get(1);
      p_config->active = FALSE;
      p_config->text_index = 0;
      p_config->text_operator = text_stage_2;
      p_config->text_coordinates = 0;
      
      return(TRUE);
    } else {    
      return(FALSE);
    }
  }
  
  int mesh_material_load_mat_text_stage_3(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    if(p_ch->type == CHUNK_TEXT_STAGE_3) {
      int text_stage_3;
      
      f->read(&text_stage_3,sizeof(int));
  
      MATERIAL_TEXT_CONFIG *p_config = p_mat->text_config_get(2);
      p_config->active = FALSE;
      p_config->text_index = 0;
      p_config->text_operator = text_stage_3;
      p_config->text_coordinates = 0;
      
      return(TRUE);
    } else {
      return(FALSE);
    }
  }
  
  int mesh_material_load_mat_text_stage_4(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    if(p_ch->type == CHUNK_TEXT_STAGE_4) {
      int text_stage_4;
      
      f->read(&text_stage_4,sizeof(int));
  
      MATERIAL_TEXT_CONFIG *p_config = p_mat->text_config_get(3);
      p_config->active = FALSE;
      p_config->text_index = 0;
      p_config->text_operator = text_stage_4;
      p_config->text_coordinates = 0;
      
      return(TRUE);
    } else {    
      return(FALSE);
    }
  }
  
  int mesh_material_load_mat_text_stage_advaced(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    if(p_ch->type >= CHUNK_TEXT_STAGE_AD_1 && p_ch->type <= CHUNK_TEXT_STAGE_AD_4) {
      int stage;
      int par[4];
      
      int num = p_ch->type-CHUNK_TEXT_STAGE_AD_1;
      
      MATERIAL_TEXT_CONFIG *p_config = p_mat->text_config_get(num);
      
      p_config->active = !num;
      
      f->read(&stage,sizeof(int));
      p_config->text_operator = stage;
      
      f->read(par,sizeof(par));
      p_config->text_index = par[0];
      
      f->read(par,sizeof(par));
      p_config->text_coordinates = par[0];
      
      f->read(par,sizeof(par));
      //text_config[num].text_func[0] = par[0]; // Texture modification function
      
      float tmp[4];
      f->read(tmp, sizeof(tmp));
      
      return(TRUE);
    } else {
      return(FALSE);
    }
  }
  
  int mesh_material_load_mat_env_spec(FHANDLE *f, DATA_CHUNK *p_ch)
  { 
    return(chunk_skip(f,p_ch));  
    /*
    if(p_ch->type == CHUNK_ENVSPEC) {
      f->read(&env_add1,sizeof(env_add1));
      f->read(&env_add2,sizeof(env_add2));
      f->read(&env_scale,sizeof(env_scale));
      f->read(&env_r,sizeof(env_r));
      f->read(&env_g,sizeof(env_g));
      f->read(&env_b,sizeof(env_b));
      return(TRUE);
    } else
    */
      return(FALSE);
  }
    
  int mesh_material_load_mat_text_flag(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    return(chunk_skip(f,p_ch));  
    /*
    if(p_mat) {
      int num = p_ch->typ-CHUNK_TEXT_FLAG_1;
      f->read(textflag+num,sizeof(textflag[num]));
      return(TRUE);
    } else
    */
      return(FALSE);
  }
  
  int mesh_material_load_text(FHANDLE *f, DATA_CHUNK *p_ch)
  {
    int num;
    switch(p_ch->type) {
      case CHUNK_TEXT_1:
        num = 0;
        break;
      case CHUNK_TEXT_2:
        num = 1;
        break;
      case CHUNK_TEXT_3:
        num = 2;
        break;
      case CHUNK_TEXT_4:
        num = 3;
        break;
      default:
        return(FALSE);
    }
    
    char filename[MAX_FILENAME];
    f->read_string(filename,sizeof(filename));
            
    MATERIAL_TEXT *p_txt = p_scene->texture_get(filename);
    if(!p_txt) {
      p_txt = p_scene->texture_create(NULL,filename,NULL);
    }
    p_mat->texture_set(num, p_txt);      
        
    return(TRUE);
  }

private:
  
  static CHUNK_LOADER material_loaders[];

private:
  
  static CHUNK_GROUP loaders[];

private:
  
  bool load(DATA_CHUNK *p_chunk, FHANDLE *f);

public:
  
  bool load(FHANDLE *f);
  bool save(FHANDLE *f);

public:
  
  import_b2m(SCENE *p_scene_ref)
  {  
    kont_name[0] = '\0';
    kont_name_ID = 0;
    kont_flag = 0;
    kont_flag2 = 0;
    kont_material_flag = 0;
    kont_material_flag2 = 0;
    
    p_mesh = NULL;
    p_mat = NULL;
    p_scene = p_scene_ref;
  }
  
  ~import_b2m(void)
  {
    
  }
  
} IMPORT_B2M;


CHUNK_LOADER import_b2m::game_mesh_loaders[] = 
{
  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_kont_name),                  CHUNK_KONT_NAME},
  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_kont_name),                  CHUNK_KONT_LOW_NAME},
  
  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_kont_id),                    CHUNK_KONT_LOW_ID},
  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_kont_id),                    CHUNK_KONT_ID},
  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_kont_flag),                  CHUNK_KONT_FLAG},
  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_kont_flag),                  CHUNK_KONT_FLAG_EXT},
  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_kont_flag),                  CHUNK_KONT_FLAG_EXT_2},
  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_kont_matrix),                CHUNK_KONT_WMATRIX},
  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_kont_matrix),                CHUNK_KONT_MPMATRIX},
  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_kont_obb),                   CHUNK_KONT_OBB},
  
  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_mesh_name),                   CHUNK_OBJ_NAME},
  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_mesh_name),                   CHUNK_OBJ_NAME_PARENT},
  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_mesh_ID),                     CHUNK_OBJ_ID},
  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_mesh_pivot),                  CHUNK_OBJ_PIVOT},
  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_mesh_material),               CHUNK_OBJ_MATERIAL},
  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_mesh_vertex),                 CHUNK_OBJ_VERTEXES},
  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_mesh_normal),                 CHUNK_OBJ_NORMAL},

  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_mesh_maps),                   CHUNK_OBJ_MAPS_1},
  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_mesh_maps),                   CHUNK_OBJ_MAPS_2},
  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_mesh_maps),                   CHUNK_OBJ_MAPS_3},
  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_mesh_maps),                   CHUNK_OBJ_MAPS_4},
  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_mesh_face),                   CHUNK_OBJ_FACES},
  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_mesh_face),                   CHUNK_OBJ_FACES_OPT},

  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_mesh_color),                  CHUNK_OBJ_COLOR},
  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_mesh_color),                  CHUNK_OBJ_COLOR_EXT},
  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_mesh_matrix),                 CHUNK_OBJ_MATRIX_KEY},
  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_mesh_obb),                    CHUNK_OBJ_OBB},
 
  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_mesh_joint_anim),             CHUNK_OBJ_JA},
  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_mesh_joint_ID),               CHUNK_OBJ_JNT_ID},
  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_mesh_joint_frames),           CHUNK_OBJ_JA_FRAME_NUM},
  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_mesh_joint_pivot),            CHUNK_OBJ_JNT_PIVOT},
  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_mesh_joint_flag),             CHUNK_OBJ_JNT_FLAG},
  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_mesh_joint_vertex),           CHUNK_OBJ_JNT_VERT},
  
  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_mesh_joint_pos),              CHUNK_OBJ_JNT_POS},
  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_mesh_joint_rot),              CHUNK_OBJ_JNT_ROT},
  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_mesh_joint_scs),              CHUNK_OBJ_JNT_SCS},
  
  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_mesh_joint_pos_spline),       CHUNK_OBJ_JNT_KEYPOS},
  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_mesh_joint_rot_spline),       CHUNK_OBJ_JNT_KEYROT},
  {(CHUNK_LOAD_FUNC)(&import_b2m::game_mesh_load_mesh_joint_scs_spline),       CHUNK_OBJ_JNT_KEYSCS},
};

CHUNK_LOADER import_b2m::material_loaders[] = 
{
  {(CHUNK_LOAD_FUNC)(&import_b2m::mesh_material_load_mat),                    CHUNK_MATERIAL},
  {(CHUNK_LOAD_FUNC)(&import_b2m::mesh_material_load_color),                  CHUNK_COLOR},
  {(CHUNK_LOAD_FUNC)(&import_b2m::mesh_material_load_color),                  CHUNK_COLOR_EXT},
  {(CHUNK_LOAD_FUNC)(&import_b2m::mesh_material_load_matanim),                CHUNK_MATANIM_EXT},
  {(CHUNK_LOAD_FUNC)(&import_b2m::mesh_material_load_matanim_frame),          CHUNK_MATANIM_FRAME},
  {(CHUNK_LOAD_FUNC)(&import_b2m::mesh_material_load_matanim_frame_ext),      CHUNK_MATANIM_EFRAME},
  {(CHUNK_LOAD_FUNC)(&import_b2m::mesh_material_load_matanim_text_1),         CHUNK_MATANIM_T1},
  {(CHUNK_LOAD_FUNC)(&import_b2m::mesh_material_load_matanim_text_2),         CHUNK_MATANIM_T2},
  {(CHUNK_LOAD_FUNC)(&import_b2m::mesh_material_load_matanim_text_3),         CHUNK_MATANIM_T3},
  {(CHUNK_LOAD_FUNC)(&import_b2m::mesh_material_load_matanim_text_4),         CHUNK_MATANIM_T4},
  {(CHUNK_LOAD_FUNC)(&import_b2m::mesh_material_load_textmatrix),             CHUNK_MATMATRIX_EXT},
  {(CHUNK_LOAD_FUNC)(&import_b2m::mesh_material_load_stage),                  CHUNK_STAG_TAG},
  {(CHUNK_LOAD_FUNC)(&import_b2m::mesh_material_load_text),                   CHUNK_TEXT_1},
  {(CHUNK_LOAD_FUNC)(&import_b2m::mesh_material_load_text),                   CHUNK_TEXT_2},
  {(CHUNK_LOAD_FUNC)(&import_b2m::mesh_material_load_text),                   CHUNK_TEXT_3},
  {(CHUNK_LOAD_FUNC)(&import_b2m::mesh_material_load_text),                   CHUNK_TEXT_4},
  {(CHUNK_LOAD_FUNC)(&import_b2m::mesh_material_load_mat_shine),              CHUNK_SHINE},
  {(CHUNK_LOAD_FUNC)(&import_b2m::mesh_material_load_mat_flags),              CHUNK_FLAGS},
  {(CHUNK_LOAD_FUNC)(&import_b2m::mesh_material_load_mat_smat_pos),           CHUNK_MAT_SMAT_POS},
  {(CHUNK_LOAD_FUNC)(&import_b2m::mesh_material_load_mat_env_spec),           CHUNK_ENVSPEC},
  {(CHUNK_LOAD_FUNC)(&import_b2m::mesh_material_load_mat_alfa_stage),         CHUNK_ALFA_STAGE},
  {(CHUNK_LOAD_FUNC)(&import_b2m::mesh_material_load_mat_text_stage_1),       CHUNK_TEXT_STAGE_1},
  {(CHUNK_LOAD_FUNC)(&import_b2m::mesh_material_load_mat_text_stage_2),       CHUNK_TEXT_STAGE_2},
  {(CHUNK_LOAD_FUNC)(&import_b2m::mesh_material_load_mat_text_stage_3),       CHUNK_TEXT_STAGE_3},
  {(CHUNK_LOAD_FUNC)(&import_b2m::mesh_material_load_mat_text_stage_4),       CHUNK_TEXT_STAGE_4},
  {(CHUNK_LOAD_FUNC)(&import_b2m::mesh_material_load_mat_text_stage_advaced), CHUNK_TEXT_STAGE_AD_1},
  {(CHUNK_LOAD_FUNC)(&import_b2m::mesh_material_load_mat_text_stage_advaced), CHUNK_TEXT_STAGE_AD_2},
  {(CHUNK_LOAD_FUNC)(&import_b2m::mesh_material_load_mat_text_stage_advaced), CHUNK_TEXT_STAGE_AD_3},
  {(CHUNK_LOAD_FUNC)(&import_b2m::mesh_material_load_mat_text_stage_advaced), CHUNK_TEXT_STAGE_AD_4},
  {(CHUNK_LOAD_FUNC)(&import_b2m::mesh_material_load_mat_text_flag),          CHUNK_TEXT_FLAG_1},
  {(CHUNK_LOAD_FUNC)(&import_b2m::mesh_material_load_mat_text_flag),          CHUNK_TEXT_FLAG_2},
  {(CHUNK_LOAD_FUNC)(&import_b2m::mesh_material_load_mat_text_flag),          CHUNK_TEXT_FLAG_3},
  {(CHUNK_LOAD_FUNC)(&import_b2m::mesh_material_load_mat_text_flag),          CHUNK_TEXT_FLAG_4},
  {(CHUNK_LOAD_FUNC)(&import_b2m::mesh_material_load_mat_texgen_param),       CHUNK_MAT_TEXGEN}
};

CHUNK_GROUP import_b2m::loaders[] = 
{
  { import_b2m::game_mesh_loaders, 
    sizeof(import_b2m::game_mesh_loaders) / sizeof(import_b2m::game_mesh_loaders[0]), 
    CHUNK_KONT_NAME, 
    CHUNK_OBJ_JNT_KEYSCS},
    
  { import_b2m::material_loaders, 
    sizeof(import_b2m::material_loaders) / sizeof(import_b2m::material_loaders[0]), 
    CHUNK_MATERIAL, 
    CHUNK_MATANIM_EFRAME},
};

bool chunk_group::check(DATA_CHUNK *p_chunk)
{
  return(p_chunk->type >= chunk_range_min && p_chunk->type <= chunk_range_max);
}

bool import_b2m::load(DATA_CHUNK *p_chunk, FHANDLE *f)
{
  CHUNK_GROUP *p_loader = NULL;
  int i;
  
  for(i = 0; i < (int)(sizeof(loaders)/sizeof(loaders[0])); i++) {
    if(loaders[i].check(p_chunk)) {
      p_loader = loaders+i;
      break;
    }
  }

  if(p_loader == NULL)
    return(FALSE);  

  for(i = 0; i < p_loader->chunk_num; i++) {
    if(p_loader->p_chunks[i].chunk == p_chunk->type) {
      //pdebug(DEBUG_LOAD_CHUNK,"Chunk %d",p_chunk->type);
      return((this->*p_loader->p_chunks[i].p_func)(f,p_chunk));
    }
  }
  
  return(FALSE);
}

/*
 * Loads objects from data file
 */
bool import_b2m::load(FHANDLE *f)
{
  DATA_CHUNK    ch;
  int           loaded_chunks = 0;  
 
  while(f->read(&ch,sizeof(ch)) == sizeof(ch)) {
        
    /*
     * Set current position for recovery
     */
    t_length data_start = f->tell();
    
    bool loaded = FALSE;
    
    loaded = load(&ch,f);
    
    if(loaded)
      loaded_chunks++;
    if(!loaded)
      f->seek(data_start+ch.size-sizeof(ch), SEEK_SET);   
  }

  return(loaded_chunks);
}

/*
 * Save objects to data file
 */
bool import_b2m::save(FHANDLE *f)
{
  // go through all materials and save them
  MESH_MATERIAL *p_mat = p_scene->material_get_first();
  while(p_mat) {
    pdebug(DEBUG_LOAD_CHUNK,"Saving material %s",p_mat->name_get());
    mesh_material_save(f, p_mat);
    p_mat = p_mat->material_next();
  }
  
  // go through all meshes and save them
  GAME_MESH *p_mesh = p_scene->mesh_get_first();
  while(p_mesh) {
    pdebug(DEBUG_LOAD_CHUNK,"Saving mesh %s",p_mesh->name_get());
    game_mesh_save(f, p_mesh);
    p_mesh = p_mesh->mesh_next();
  }
  
  return(TRUE);
}

/*
 * Save objects to data file
 */
bool scene_import_b2m(SCENE *p_scene, FHANDLE *f)
{
  if(p_scene) {
    IMPORT_B2M import(p_scene);
    return(import.load(f));
  }
  else {
    return(FALSE);
  }
}

bool scene_import_b2m(SCENE *p_scene, char *p_dir, char *p_file)
{
  MMHANDLE f;
  bool ret = FALSE;
  
  if(f.open(p_dir, p_file, "rb", FALSE)) {  
    ret = scene_import_b2m(p_scene, &f);
    f.close();
  }
  
  return(ret);  
}

bool scene_export_b2m(SCENE *p_scene, FHANDLE *f)
{
  if(p_scene) {
    IMPORT_B2M import(p_scene);
    return(import.save(f));
  }
  else {
    return(FALSE);
  }
}

bool scene_export_b2m(SCENE *p_scene, char *p_dir, char *p_file)
{
  GZHANDLE f;
  bool ret = FALSE;
  
  if(f.open(p_dir, p_file, "wb", FALSE)) {
    ret = scene_export_b2m(p_scene, &f);
    f.close();
  }
  
  return(ret);  
}
