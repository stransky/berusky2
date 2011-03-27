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
 * Calculator for normal vectors 
 */

#include "age.h"


static VECT * calc_face_normals(VECT *p_vertexes, int vertexnum, tface *p_face, int facenum)
{
  VECT *p_face_normals = (VECT *)mmalloc(sizeof(p_face_normals[0])*(facenum/3));
  
  int i,face;
  for(i = 0, face = 0; i < facenum; i+=3, face++) {
    face_normal(p_vertexes+p_face[i],
                p_vertexes+p_face[i+1],
                p_vertexes+p_face[i+2],
                p_face_normals+face);
  }  
  
  return(p_face_normals);
}

#define MAX_FACES_PER_VERTEX  50

typedef struct face_references {
  
  int   facenum;
  tface faces[MAX_FACES_PER_VERTEX];
  
public:

  void reference_add(int face)
  {
    assert(facenum < MAX_FACES_PER_VERTEX);
    if(facenum < MAX_FACES_PER_VERTEX) {
      faces[facenum] = face;
      facenum++;
    }
  }  
  
} FACE_REFERENCES;

static FACE_REFERENCES * calc_face_references(VECT *p_vertexes, int vertexnum, tface *p_face, int facenum)
{
  FACE_REFERENCES *p_face_references = (FACE_REFERENCES *)mmalloc(sizeof(p_face_references[0])*vertexnum);
  
  int i,face;
  for(i = 0, face = 0; i < facenum; i+=3, face++) {    
    p_face_references[p_face[i]].reference_add(face);
    p_face_references[p_face[i+1]].reference_add(face);
    p_face_references[p_face[i+2]].reference_add(face);
  }  
  
  return(p_face_references);
}

void normals_calc(VECT *p_vertexes, int vertexnum, tface *p_face, int facenum, VECT *p_normals)
{
  VECT *p_face_normals = calc_face_normals(p_vertexes, vertexnum, p_face, facenum);
  FACE_REFERENCES *p_face_references = calc_face_references(p_vertexes, vertexnum, p_face, facenum);
  
  int i;
  for(i = 0; i < vertexnum; i++) {
    FACE_REFERENCES *p_ref = p_face_references+i;
    VECT norm(0.0f);
    
    int f;
    for(f = 0; f < p_ref->facenum; f++) {
      norm += p_face_normals[p_ref->faces[f]];
    }
    
    norm.norm();
    p_normals[i] = norm;
  }
  
  ffree(p_face_references);
  ffree(p_face_normals);
}
