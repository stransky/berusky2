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
 * Vertex arrays support routines
 * A wrapper for ARB_vertex_buffer_object
 */

/*
  TODO 
  - rewrite this for closer interaction with meshes
  - strides
*/

#ifndef __VERTEX_ARRAYS_H__
#define __VERTEX_ARRAYS_H__

#include <assert.h>

/****************************************************************************
   Arrays settings configuration  
  ***************************************************************************
*/
typedef class varray_vertex_configuration {

  #define MAX_ARRAY_TEXT  8
  
public:

  // **************************************************
  // Vertex buffers
  // **************************************************

  // currently attached vertex array
  static GLuint glstate_handle; 

  // state of items
  static bool glstate_vertex; 
  static bool glstate_diffuse;
  static bool glstate_specular;
  static bool glstate_normal;
  static bool glstate_text_act;
  static bool glstate_text[MAX_ARRAY_TEXT];

public:

  static void init(void)
  {
    glstate_handle = ERROR;
    
    glstate_vertex = FALSE;
    glstate_diffuse = FALSE;
    glstate_specular = FALSE;
    glstate_normal = FALSE;
    glstate_text_act = FALSE;
    
    int i;
    for(i = 0; i < MAX_ARRAY_TEXT; i++)
      glstate_text[i] = FALSE;
  }

  // Bind vertex buffer
  static bool bind(GLuint buffer_handle)
  {
    assert(buffer_handle > 0);
    if(glstate_handle != buffer_handle) {
      glBindBufferARB(GL_ARRAY_BUFFER_ARB, buffer_handle);
      glstate_handle = buffer_handle;
    }
    // TODO -> test?
    return(TRUE);    
  }
  
  static void vertex_on(void)
  {
    if(!glstate_vertex) {
      glEnableClientState(GL_VERTEX_ARRAY);      
      glstate_vertex = TRUE;
    }
  }
  static void vertex_off(void)
  {
    if(glstate_vertex) {
      glDisableClientState(GL_VERTEX_ARRAY);
      glstate_vertex = FALSE;
    }
  }
  static void vertex_set(bool state)
  {
    if(state) {
      vertex_on();
    } else {
      vertex_off();
    }
  }
      
  // Specular colors
  static void specular_on(void)
  {
    if(!glstate_specular) {
      glEnableClientState(GL_SECONDARY_COLOR_ARRAY_EXT);
      glstate_specular = TRUE;
    }
  }  
  static void specular_off(void)
  {
    if(glstate_specular) {
      glDisableClientState(GL_SECONDARY_COLOR_ARRAY_EXT);
      glstate_specular = FALSE;
    }
  }
  static void specular_set(bool state)
  {
    if(state) {
      specular_on();
    } else {
      specular_off();
    }
  }
  
  // Diffuse colors  
  static void diffuse_on(void)
  {
    if(!glstate_diffuse) {
      glEnableClientState(GL_COLOR_ARRAY);        
      glstate_diffuse = TRUE;
    }
  }  
  static void diffuse_off(void)
  {
    if(glstate_diffuse) {
      glDisableClientState(GL_COLOR_ARRAY);
      glstate_diffuse = FALSE;
    }
  }
  static void diffuse_set(bool state)
  {
    if(state) {
      diffuse_on();
    } else {
      diffuse_off();
    }    
  }
    
  // Normals
  static void normal_on(void)
  {
    if(!glstate_normal) {
      glEnableClientState(GL_NORMAL_ARRAY);
      glstate_normal = TRUE;
    }
  }  
  static void normal_off(void)
  {
    if(glstate_normal) {
      glDisableClientState(GL_NORMAL_ARRAY);
      glstate_normal = FALSE;
    }
  }
  static void normal_set(bool state)
  {
    if(state) {
      normal_on();
    } else {
      normal_off();
    }
  }

  // Active texture coordinates array
  static bool text_set_act(int num)
  {
    if(num < MAX_ARRAY_TEXT) {
      if(num != glstate_text_act) {
        glstate_text_act = num;
        //glClientActiveTextureARB(arb_prevodni_tabulka[num]);
      }
      return(TRUE);
    } else 
      return(FALSE);
  }  
  
  // Texture coordinates
  static void text_on(void)
  {
    if(!glstate_text[glstate_text_act]) {
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glstate_text[glstate_text_act] = TRUE;
    }
  }  
  static void text_off(void)
  {
    if(glstate_text[glstate_text_act]) {
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      glstate_text[glstate_text_act] = FALSE;
    }
  }
  static void text_set(bool state)
  {
    if(state) {
      text_on();
    } else {
      text_off();
    }
  }

  static void text_on(int num)
  {
    text_set_act(num);
    if(!glstate_text[num]) {
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glstate_text[num] = TRUE;
    }
  }  
  static void text_off(int num)
  {
    if(glstate_text[num]) {
      text_set_act(num);
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      glstate_text[num] = FALSE;
    }
  }  
    
  static void enable(void)
  {
    vertex_on();        
    text_on(0);
  }
  
  static void disable(void)
  {
    vertex_off();
    diffuse_off();
    specular_off();
    normal_off();
    
    int i;
    for(i = 0; i < MAX_ARRAY_TEXT; i++) {
      text_off(i);
    }
  } 
  
} VARRAY_VERTEX_CONFIGURATION;

/****************************************************************************
   Vertex array support routines
  ***************************************************************************
*/

#define SIZEOF_VERTEX   (sizeof(VECT))
#define SIZEOF_NORMAL   (sizeof(VECT))
#define SIZEOF_DIFFUSE  (sizeof(RGBAF))
#define SIZEOF_SPECULAR (sizeof(RGBF))
#define SIZEOF_TEXT     (sizeof(VECTUV))
  
#define RENDER_VERTEX   0x1
#define RENDER_NORMALS  0x2
#define RENDER_DIFFUSE  0x4
#define RENDER_SPECULAR 0x8
#define RENDER_TEXT(n)  (0x10 << n)
  
typedef class varray_vertex : public varray_vertex_configuration {

  int flag;

  // Handle of this vertex array and it's whole size
  GLuint handle;
  int size;

  // Indexes of data in vertex arrays
  int vertexnum;
  
  // Indexes of data in vertex arrays
  // ERROR (-1) if it isn't active
  int ivertexes;
  int inormals;
  int idiffuse;
  int ispecular;
  int itext[MAX_ARRAY_TEXT];
  
public:

  // Creates/destroy requested vertex array
  bool create(int vertex_num, bool normals, bool diff, bool spec, int text_num);
  void free(void);

  varray_vertex(void)
  {
    handle = ERROR;
    size = 0;
    vertexnum = 0;
    flag = 0;
  }
  
  ~varray_vertex(void)
  {
    free();
  }

  // Uploads data to vertex array  
  void upload_vertexes(void *p_data)
  {
    bind(handle);
    glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, ivertexes, SIZEOF_VERTEX*vertexnum, p_data);
    flag |= RENDER_VERTEX;
  }
  
  void upload_normals(void *p_data)
  {  
    bind(handle);
    glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, inormals, SIZEOF_NORMAL*vertexnum, p_data);
    flag |= RENDER_NORMALS;
  }
  
  void upload_diffuse(void *p_data)
  {
    bind(handle);
    glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, idiffuse, SIZEOF_DIFFUSE*vertexnum, p_data);
    flag |= RENDER_DIFFUSE;
  }
  
  void upload_specular(void *p_data)
  {
    bind(handle);
    glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, ispecular, SIZEOF_SPECULAR*vertexnum, p_data);
    flag |= RENDER_SPECULAR;
  }
  
  void upload_text(int num, void *p_data)
  {
    bind(handle);
    glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, itext[num], SIZEOF_TEXT*vertexnum, p_data);
    flag |= RENDER_TEXT(num);
  }
  
  // Sets arrays for rendering
  void set(void);
 
} VARRAY_VERTEX;

// *************************************************************
// Index array_configure
// *************************************************************

typedef class varray_index_configuration {
  
public:

  // **************************************************
  // Index buffers
  // **************************************************

  // currently attached index array
  static GLuint glstate_handle; 

  // state of items
  static bool glstate_index;

public:

  static void init(void)
  {  
    glstate_index = FALSE;    
  }
  
  // Bind index buffer
  static bool bind(GLuint buffer_handle)
  {
    assert(buffer_handle > 0);
    if(glstate_handle != buffer_handle) {
      glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, buffer_handle);
      glstate_handle = buffer_handle;
    }
    // TODO -> test?
    return(TRUE);    
  }  
  
  static void enable(void)
  {
  }
  
  static void disable(void)
  {
  } 
  
} VARRAY_INDEX_CONFIGURATION;

typedef class varray_index : public varray_index_configuration {

  int flag;

  // Handle of this vertex array and it's whole size
  GLuint handle;
  int size;  

public:

  // Creates/destroy requested vertex array
  bool create(void);
  void free(void);

  varray_index(void)
  {
    handle = (GLuint)ERROR;
    size = 0;
    flag = 0;
  }
  
  ~varray_index(void)
  {
    free();
  }

  // Uploads data to vertex array  
  void upload_indexes(void *p_data)
  {
    bind(handle);
  }
    
  // Sets arrays for rendering
  void set(void);
 
} VARRAY_INDEX;

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

#endif //  __VERTEX_ARRAYS_H__
