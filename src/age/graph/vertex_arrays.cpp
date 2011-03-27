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
 
// Constants and types
#include "age.h"

// Static configuration of vertex arrays
GLuint  varray_vertex_configuration::glstate_handle = 0;
bool    varray_vertex_configuration::glstate_vertex = FALSE; 
bool    varray_vertex_configuration::glstate_diffuse = FALSE;
bool    varray_vertex_configuration::glstate_specular = FALSE;
bool    varray_vertex_configuration::glstate_normal = FALSE;
bool    varray_vertex_configuration::glstate_text_act = FALSE;
bool    varray_vertex_configuration::glstate_text[MAX_ARRAY_TEXT] = 
       { FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE };

// Creates requested vertex array
bool varray_vertex::create(int vertex_num, bool normals, bool diff, bool spec, int text_num)
{
  vertexnum = vertex_num;

  size = SIZEOF_VERTEX*vertexnum;
  if(normals)
    size += SIZEOF_NORMAL*vertexnum;
  if(diff)
    size += SIZEOF_DIFFUSE*vertexnum;
  if(spec)
    size += SIZEOF_SPECULAR*vertexnum;
  if(text_num > 0)
    size += SIZEOF_TEXT*vertexnum*text_num;
  
  glGenBuffersARB(1, &handle);  
  glBufferDataARB(GL_ARRAY_BUFFER_ARB, size, NULL, GL_DYNAMIC_DRAW_ARB);
  return(TRUE);
}

// Destroy requested vertex array
void varray_vertex::free(void)
{
  if(handle > 0 && glIsBufferARB(handle)) {
    glDeleteBuffersARB(1,&handle);
    handle = ERROR;
  }  
}

// Sets arrays for rendering
void varray_vertex::set(void)
{
  bind(handle);

  assert(flag&RENDER_VERTEX);  
  vertex_on();
  glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(ivertexes));
  
  if(flag&RENDER_NORMALS) {
    normal_on();
    glNormalPointer(GL_FLOAT, 0, BUFFER_OFFSET(inormals));
  } else {
    normal_off();
  }
  
  if(flag&RENDER_NORMALS) {
    diffuse_on();
    glColorPointer(4, GL_FLOAT, 0, BUFFER_OFFSET(idiffuse));
  } else {
    diffuse_off();
  }
    
  if(flag&RENDER_SPECULAR) {
    specular_on();
    glSecondaryColorPointerEXT(3, GL_FLOAT, 0, BUFFER_OFFSET(ispecular));
  } else {
    specular_off();
  } 

  int i;
  for(i = 0; i < MAX_ARRAY_TEXT; i++) {
    if(flag&RENDER_TEXT(i)) {
      text_on(i);
      glTexCoordPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(itext[i]));
    } else {
      text_off(i);
    }
  }  
}
