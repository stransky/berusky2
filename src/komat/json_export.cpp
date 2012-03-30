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
#include "3d_all.h"
#include "Object.h"
#include "json_export.h"
#include <string>

int kom_get_mesh_id(MeshHandle prvek_handle);

FILE * export_file = NULL;
char export_level_dir[MAX_FILENAME];

void json_export_matrix(FILE *f, GLMATRIX *p_matrix, const char *p_name)
{
  fprintf(f,"{\n");
  fprintf(f,"    \"name\" : \"%s\",\n", p_name);
  fprintf(f,"    \"x-vector\" : [ %f, %f, %f, %f ],\n", p_matrix->_11,p_matrix->_21,p_matrix->_31,p_matrix->_41);
  fprintf(f,"    \"y-vector\" : [ %f, %f, %f, %f ],\n", p_matrix->_12,p_matrix->_22,p_matrix->_32,p_matrix->_42);
  fprintf(f,"    \"z-vector\" : [ %f, %f, %f, %f ],\n", p_matrix->_13,p_matrix->_23,p_matrix->_33,p_matrix->_43);
  fprintf(f,"    \"w-vector\" : [ %f, %f, %f, %f ]\n", p_matrix->_14,p_matrix->_24,p_matrix->_34,p_matrix->_44);
  fprintf(f,"  },\n");
}

void json_export_texture(FILE *f, EDIT_TEXT *p_text)
{
  fprintf(f,"{\n");
  fprintf(f," \"type\" : \"texture\",\n");
  fprintf(f," \"name\" : \"%s\",\n", p_text->jmeno);
  //fprintf(f," file: %s\n", p_text->p_bmp);
  fprintf(f,"}\n");
}

/*
typedef struct _EDIT_MATERIAL
{
  // Jmeno materialu
  char jmeno[MAX_JMENO];

  // Textury materialu
  char textfile[MAT_TEXTUR][MAX_JMENO];

  EDIT_TEXT *p_text[MAT_TEXTUR];
  int textflag[MAT_TEXTUR];     // flag textury
  GLMATRIX ttext[MAT_TEXTUR];   // matrice posunu co se dava jako settransformmatrix
  float texgen_koord_s[MAT_TEXTUR][4];  // texture-generovane veci
  float texgen_koord_t[MAT_TEXTUR][4];  // texture-generovane veci
  float texgen_koord_r[MAT_TEXTUR][4];  // texture-generovane veci
  float texgen_koord_q[MAT_TEXTUR][4];  // texture-generovane veci

  // Michaci funkce
  int alfa_state;               // alfa blok
  MATERIAL_TEXT_OLD text_state[MAT_TEXTUR];     // texture bloky
  float lesk;                   // lesklost 
  MATERIAL dxmat;               // barva objektu
  ANIM_MATERIAL anim;           // frame animace (u animovanych materialu)
  int atextnum[MAT_TEXTUR];
  int atextakt[MAT_TEXTUR];

  ANIM_TEXT *p_atext[MAT_TEXTUR][MAX_TEXT_ANIM];        // animace textur 
  int smaterial_pozice;         // pozice s-materialu
  int textur;                   // celkovy pocet textur ktery material pouziva

  dword flag;                   // flagy materialu
  dword flag2;                  // flagy2 materialu
  int material_handle;          // handle materialu

  TDS_MAT_FLAGS tds;            // modifikace importovane z 3d studia
  float env_r,                  // env spekularni barva
    env_g, env_b;
  float env_add1;               // aditivni konstanta 1
  float env_add2;               // aditivni konstanta 2
  float env_scale;              // scale env uhlu  
  struct _EDIT_MATERIAL *p_sub; // pole sub-materialu (pouze pro ase-loader)  
} EDIT_MATERIAL;
*/
void json_export_material(FILE *f, EDIT_MATERIAL *p_mat)
{
  fprintf(f,"{\n");
  fprintf(f,"  \"type\" : \"material\",\n");
  fprintf(f,"  \"name\" : \"%s\",\n", p_mat->jmeno);
  fprintf(f,"  \"transparent\" : \"%d\",\n", (p_mat->flag&MAT_PRUHLEDNY) ? 1 : 0);
  fprintf(f,"  \"z_buffer_mask\" : \"%d\",\n", (p_mat->flag&MAT_NO_ZMASK) ? 0 : 1);
  fprintf(f,"  \"z_buffer_test\" : \"%d\",\n", (p_mat->flag&MAT_NO_ZTEST) ? 0 : 1);
  fprintf(f,"  \"backface_culling\" : \"%d\",\n", (p_mat->flag&MAT_NO_CULL) ? 0 : 1);

  fprintf(f,"  \"diffuse_color\" : \"%d\",\n", (p_mat->flag2&MAT2_DIFFUSE) ? 1 : 0);
  fprintf(f,"  \"specular_color\" : \"%d\",\n", (p_mat->flag2&MAT2_SPECULAR) ? 1 : 0);

  fprintf(f,"  \"textures\" : [ ");
  int i;
  for(i = 0; i < MAT_TEXTUR; i++) {
    // Vzdy nove vytvoreny string -> prazdny
    std::string text;
    text.append(p_mat->textfile[i]);
    // Compare vraci 0 pri shode
    if(text.compare("")){
      // Prepis bmp na jpg
      text.resize(text.length() - 3);
      text.append("jpg");
      if(i == 0){
        fprintf(f,"\"%s\"", text.c_str());
      } 
      else{
        fprintf(f,", \"%s\"", text.c_str());
      }
     }
  }
  fprintf(f," ]\n");
  fprintf(f,"}");
}

void json_export_materialy(FILE *f, EDIT_MATERIAL ** p_mat, int max_mat)
{
  int i;
  for(i = 0; i < max_mat; i++) {
    if(p_mat[i]) {
      json_export_material(f,p_mat[i]);
      if(i + 1 < max_mat) fprintf(f, ",\n");
    }
  }
}


/*
// Konstni animace objektu
typedef struct _EDIT_OBJEKT
{
  char jmeno[MAX_JMENO];
  char jmeno_parent[MAX_JMENO];
  int Objekt_ID;                // ID objektu

  word vertexnum;
  OBJ_VERTEX *p_vertex;
  BOD *p_vertex_kosti;          // kostni vertexy
  word facenum;
  word *p_face;
  dword *p_fsous;
  GLenum face_typ;              // typ facu
  int optnum;                   // optimalizovane facy
  int *p_opt;

  word linenum;
  word *p_line;
  GLenum line_typ;              // typ car
  int material;                 // pouzity material (z tabulky materialu)
  int material_sub;
  float float_scale;            // aktualni float-scale
  float float_scale_last;       // minuly float scale

  word x_rect;                  // x ctvercu objektu
  word y_rect;                  // y ctvercu objektu
  char kreslit;

  BOD pivot;                    // pivot-point objektu (relativni)
  char trans;                   // transformovane svetlo (je preveden na billboard a spocitane okoli)
  dword m1flag;                 // flagy objektu
  dword m2flag;                 // flagy objektu
  dword oflag;

  TRACK_INFO *p_track;          // track info k animaci  
  GLMATRIX m;                   // matrix k animacim

  OBB_OLD obb;
  void *p_tmp_top;              // pomocny pointer na kontejner
  struct _EDIT_OBJEKT *p_tmp_next;      // pomocny pointer na dalsi objekt

  JOINT_ANIMACE *p_joit_animace;        // pointer na joint-animace
  struct _EDIT_OBJEKT *p_lod;   // LOD objekty tohodlenctoho

  SIMPLE_TRACK_INFO *p_tmp_sim;
} EDIT_OBJEKT;

typedef struct _MUJ_BOD
{
  float x, y, z;
  float px, py, pz;
  float nx, ny, nz;
  float mdr, mdg, mdb, mda;
  float dr, dg, db, da;
  float msr, msg, msb;
  float sr, sg, sb;
  float tu1, tv1;               // 4 textury
  float tu2, tv2;
  float tu3, tv3;
  float tu4, tv4;               // lightmapa
  float tul, tvl;
} MUJ_BOD, OBJ_VERTEX, OBJEKT_BOD;
*/
void json_export_object(FILE *f, EDIT_OBJEKT *p_obj, 
                        EDIT_MATERIAL ** p_mat, int max_mat)
{
  int i;

  fprintf(f,"   {\n");
  fprintf(f,"    \"name\" : \"%s\",\n", p_obj->jmeno);
  fprintf(f,"    \"name_parent\" : \"%s\",\n", p_obj->jmeno_parent);
  fprintf(f,"    \"object_id\" : \"%d\",\n", p_obj->Objekt_ID);
  fprintf(f,"    \"material\" : \"%s\",\n", p_mat[p_obj->material]->jmeno);

  fprintf(f,"    \"vertexPositions\" : [");
  for(i = 0; i < p_obj->vertexnum-1; i++) {
    fprintf(f,"%f,%f,%f,",(p_obj->p_vertex[i].x), (p_obj->p_vertex[i].y) ,(p_obj->p_vertex[i].z));
  }
  fprintf(f,"%f,%f,%f",p_obj->p_vertex[p_obj->vertexnum-1].x,
                       p_obj->p_vertex[p_obj->vertexnum-1].y,
                       p_obj->p_vertex[p_obj->vertexnum-1].z);
  fprintf(f,"],\n");

  fprintf(f,"    \"vertexDiffuse\" : [");
  for(i = 0; i < p_obj->vertexnum-1; i++) {
    fprintf(f,"%f,%f,%f,%f,",(p_obj->p_vertex[i].mdr), (p_obj->p_vertex[i].mdg) ,(p_obj->p_vertex[i].mdb), (p_obj->p_vertex[i].mda));
  }
  fprintf(f,"%f,%f,%f,%f",p_obj->p_vertex[p_obj->vertexnum-1].mdr,
                          p_obj->p_vertex[p_obj->vertexnum-1].mdg,
                          p_obj->p_vertex[p_obj->vertexnum-1].mdb);
  fprintf(f,"],\n");

  fprintf(f,"    \"vertexSpecular\" : [");
  for(i = 0; i < p_obj->vertexnum-1; i++) {
    fprintf(f,"%f,%f,%f,",(p_obj->p_vertex[i].msr), (p_obj->p_vertex[i].msg) ,(p_obj->p_vertex[i].msb));
  }
  fprintf(f,"%f,%f,%f",p_obj->p_vertex[p_obj->vertexnum-1].msr,
                       p_obj->p_vertex[p_obj->vertexnum-1].msg,
                       p_obj->p_vertex[p_obj->vertexnum-1].msb);
  fprintf(f,"],\n");

  fprintf(f,"    \"vertexNormals\" : [");
  for(i = 0; i < p_obj->vertexnum-1; i++) {
    fprintf(f,"%f,%f,%f,",p_obj->p_vertex[i].nx,p_obj->p_vertex[i].ny,p_obj->p_vertex[i].nz);
  }
  fprintf(f,"%f,%f,%f",p_obj->p_vertex[p_obj->vertexnum-1].nx,
                       p_obj->p_vertex[p_obj->vertexnum-1].ny,
                       p_obj->p_vertex[p_obj->vertexnum-1].nz);
  fprintf(f,"],\n");

  fprintf(f,"    \"vertexTextureCoords0\" : [");
  for(i = 0; i < p_obj->vertexnum-1; i++) {
    fprintf(f,"%f,%f,",p_obj->p_vertex[i].tu1,p_obj->p_vertex[i].tv1);
  }
  fprintf(f,"%f,%f",p_obj->p_vertex[p_obj->vertexnum-1].tu1,
                    p_obj->p_vertex[p_obj->vertexnum-1].tv1);
  fprintf(f,"],\n");

  fprintf(f,"    \"vertexTextureCoords1\" : [");
  for(i = 0; i < p_obj->vertexnum-1; i++) {
    fprintf(f,"%f,%f,",p_obj->p_vertex[i].tu2,p_obj->p_vertex[i].tv2);
  }
  fprintf(f,"%f,%f",p_obj->p_vertex[p_obj->vertexnum-1].tu2,
                    p_obj->p_vertex[p_obj->vertexnum-1].tv2);
  fprintf(f,"],\n");

  fprintf(f,"    \"indices\" : [");
  for(i = 0; i < p_obj->facenum-1; i++) {
    fprintf(f,"%d,",p_obj->p_face[i]);
  }
  fprintf(f,"%d",p_obj->p_face[p_obj->facenum-1]);
  fprintf(f,"]\n");

  fprintf(f,"   }");
}

void json_export_kont(FILE *f, EDIT_KONTEJNER *p_kont, 
                      EDIT_MATERIAL ** p_mat, int max_mat)
{
  fprintf(f,"{\n");
  fprintf(f,"  \"type\" : \"geometry_container\",\n");
  fprintf(f,"  \"name\" : \"%s\",\n", p_kont->jmeno);
  fprintf(f,"  \"container_id\" : \"%d\",\n", p_kont->kontejner_ID);
  fprintf(f,"  \"matrix\" : ");
  json_export_matrix(f, &p_kont->world, "world");

  fprintf(f,"  \"geometry_objects\" : [\n");
  int i;
  for(i = 0; i < MAX_KONT_OBJEKTU; i++) {
    if(p_kont->p_obj[i]) {
      if(i != 0){
        fprintf(f, ",\n");
      }
      json_export_object(f,p_kont->p_obj[i], p_mat, max_mat);
      //if(i + 1 < MAX_KONT_OBJEKTU){
      //  fprintf(f, ",");
      //}
    }
  }
  fprintf(f,"\n  ]\n");

  fprintf(f,"}");
}

void json_export_kontejnery(FILE *f, EDIT_KONTEJNER **p_kont, int max_kont,
                            EDIT_MATERIAL ** p_mat, int max_mat)
{
  int i;
  for(i = 0; i < max_kont; i++) {
    if(p_kont[i]) {
      if(i != 0) { fprintf(f, ",\n"); }
      json_export_kont(f,p_kont[i], p_mat, max_kont);
    }
  }
}

/*
// pouze na staticke objekty
typedef struct _EDIT_MESH_POLY
{                               //celej kontejner    
  char jmeno[MAX_JMENO];        // jmeno poly
  int facenum;                  // co face to objekt
  TEXT_KOORD *p_koord;          // list texture koordinatu - plosky po 3

  LIGHTMAP_FACE *p_ind;         // lightmapovaci indicie, je jich facenum/3
  dword *p_fsous;               // sousedni plosky
  dword m1flag;                 // flag materialu -> stejne jako u kontejneru
  dword m2flag;                 // flag mat 2 -> stejne jako u kontejneru
  dword kflag;                  // flag kontejneru
  dword k2flag;                 // flag kontejneru
  int material;                 // material poly
  int kont;                     // cislo kontejneru, ze ktereho bylo poly vyrobeno
  int poly;                     // poradove cislo poly v seznamu polyu

  MLZNA_KOSTKA *p_mlha;         // mlzna kostka polyho
  int lightnum;                 // pocet lightmap
  int *p_lightnum;              // pocty light-map
  EDIT_TEXT **p_light;          // pole  light-map    
  char kreslit;
  int mail;                     // mailbox poly :)
  OBB_OLD obb;
  // Svetla poly - dynamicke 
  void **p_dlight;              // pole svetel meshu 
  int lightmax;                 // velikost pole na svetla
  int lightakt;                 // aktualni vekikost

  // Svetla poly - extra-dynamicke
  int top_edlight;              // top-svetlo
  void **p_edlight;             // pole extra svetel meshu 
  int edlightmax;               // velikost pole na extra svetla
  int edlightakt;               // aktualni velikost

  VERTEX_ARRAYS varray;         // vertex-arrays
} EDIT_MESH_POLY;

for (l = 0; l < p_poly->lightnum; l++) {
  glBegin(GL_TRIANGLES);
  p_koord = p_poly->p_koord + last;
  for (i = 0; i < p_poly->p_lightnum[l]; i++) {
    glTexCoord2fv(&p_koord[i].tu1);
    glColor4fv(&p_koord[i].dr);
    glNormal3fv(&p_koord[i].nx);
    glVertex3fv(&p_koord[i].x);
  }
  glEnd();
  last += p_poly->p_lightnum[l];
}

*/
void json_export_poly(FILE *f, EDIT_MESH_POLY *p_poly,
                      EDIT_MATERIAL ** p_mat, int max_mat, 
                      int poly_id)
{
  fprintf(f,"{\n");
  fprintf(f,"  \"type\" : \"geometry_container_poly\",\n");
  fprintf(f,"  \"name\" : \"%s\",\n", p_poly->jmeno);
  fprintf(f,"  \"container_id\" : \"%d\",\n", -1);
  fprintf(f,"  \"poly_id\" : \"%d\",\n", poly_id);

  GLMATRIX m;
  init_matrix(&m);
  fprintf(f,"  \"matrix\" : ");
  json_export_matrix(f, &m, "world");
  
  fprintf(f,"  \"geometry_objects\" : [\n");
  fprintf(f,"   {\n");
  fprintf(f,"    \"name\" : \"%s\",\n", p_poly->jmeno);
  fprintf(f,"    \"material\" : \"%s\",\n", p_mat[p_poly->material]->jmeno);

  fprintf(f,"    \"vertexPositions\" : [");
  int i;
  TEXT_KOORD *p_koord = p_poly->p_koord;
  for(i = 0; i < p_poly->facenum-1; i++, p_koord++) {
    fprintf(f,"%f,%f,%f,",p_koord->x,p_koord->y,p_koord->z);
  }
  fprintf(f,"%f,%f,%f",p_koord->x,p_koord->y,p_koord->z);
  fprintf(f,"],\n");

  fprintf(f,"    \"vertexNormals\" : [");
  p_koord = p_poly->p_koord;
  for(i = 0; i < p_poly->facenum-1; i++, p_koord++) {
    fprintf(f,"%f,%f,%f,",p_koord->nx,p_koord->ny,p_koord->nz);
  }
  fprintf(f,"%f,%f,%f",p_koord->nx,p_koord->ny,p_koord->nz);
  fprintf(f,"],\n");

  fprintf(f,"    \"vertexTextureCoords0\" : [");
  p_koord = p_poly->p_koord;
  for(i = 0; i < p_poly->facenum-1; i++, p_koord++) {
    fprintf(f,"%f,%f,",p_koord->tu1,p_koord->tv1);
  }
  fprintf(f,"%f,%f",p_koord->tu1,p_koord->tv1);
  fprintf(f,"],\n");

  fprintf(f,"    \"vertexTextureCoords1\" : [");
  p_koord = p_poly->p_koord;
  for(i = 0; i < p_poly->facenum-1; i++, p_koord++) {
    fprintf(f,"%f,%f,",p_koord->tu2,p_koord->tv2);
  }
  fprintf(f,"%f,%f",p_koord->tu2,p_koord->tv2);
  fprintf(f,"],\n");

  fprintf(f,"    \"vertexTextureCoordsLight\" : [");
  p_koord = p_poly->p_koord;
  for(i = 0; i < p_poly->facenum-1; i++, p_koord++) {
    fprintf(f,"%f,%f,",p_koord->tul,p_koord->tvl);
  }
  fprintf(f,"%f,%f",p_koord->tul,p_koord->tvl);
  fprintf(f,"],\n");

  fprintf(f,"    \"indices\" : [");  
  for (i = 0; i < p_poly->facenum-1; i++) {
    fprintf(f,"%d,",i);
  }
  fprintf(f,"%d",i);    
  fprintf(f,"]\n");

  fprintf(f,"   }\n  ]\n");
  fprintf(f,"}");
}

void json_export_poly(EDIT_MESH_POLY *p_poly, int polynum,
                      EDIT_MATERIAL ** p_mat, int max_mat)
{
  fprintf((FILE*)export_file, ",\n");
  //fprintf((FILE*)export_file, "\n\n\nPOLY EXPORT BEGIN\n\n\n");
  if(export_file) {
    int i;
    for(i = 0; i < polynum; i++) {
      if(i != 0) {fprintf((FILE*)export_file, ",\n");}
      json_export_poly(export_file, p_poly+i, p_mat, max_mat, i);
    }
  }
  fprintf((FILE*)(export_file), ",");
}

/*
typedef struct _STATIC_LIGHT
{
  int akt;                      // aktivni svetlo
  char jmeno[MAX_JMENO];        // jmeno svetla
  int index;                    // index svetla (pozice + ID)  
  BOD p;
  float r, g, b;
  float k1, k2, k3;             // tlumici koeficienty
  float dosah;                  // max. dosah svetla
  char uhel;                    // tlumit podle uhlu svetla
  char utlum;                   // tlumit podle vzdalenosti
  void *p_flare;                // flare svetla - pointer na nej
  int last_poly;                // posledni hitovany poly
  int last_kont;                // posledni hitovany kontejner
  int flag;                     // flag vlasnosti svetla -> update a podobne
  int flag2;                    // druhy flag -> vlastnosti
  void *p_mesh_data;            // privazany mesh, ke kterymu to patri
  int bez_prev;                 // vyjmout z dyn osvetlovani
  struct _STATIC_LIGHT *p_next; // nasledujici svetlo (v seznamu u kontejneru)
  struct _STATIC_LIGHT *p_prev; // predchozi svetlo
} STATIC_LIGHT;
*/
void json_export_svetlo(FILE *f, STATIC_LIGHT * p_light)
{
  fprintf(f,"{\n");
  fprintf(f,"  \"type\" : \"light\",\n");
  fprintf(f,"  \"name\" : \"%s\",\n", p_light->jmeno);
  fprintf(f,"  \"position\" : [ %f, %f, %f],\n", p_light->p.x, p_light->p.y, p_light->p.z);
  fprintf(f,"  \"diffuse_color\" : [ %f, %f, %f],\n", p_light->r, p_light->g, p_light->b);
  fprintf(f,"  \"coeficients\" : [ %f, %f, %f],\n", p_light->k1, p_light->k2, p_light->k3);
  fprintf(f,"  \"max_range\" : \"%f\",\n", p_light->dosah);
  fprintf(f,"}");
}

void json_export_svetla(FILE *f, STATIC_LIGHT * p_light, int lightnum)
{
  int i;
  for(i = 0; i < lightnum; i++) {
    if(!p_light[i].akt)
      break;
    json_export_svetlo(f,p_light+i);
    fprintf(f, ",\n");    
  }
}

void json_export_level(EDIT_KONTEJNER **p_kont, int max_kont,
                       EDIT_MATERIAL ** p_mat, int max_mat,
                       STATIC_LIGHT * p_light, int lightnum)
{
  if(export_file) {
    fprintf((FILE*)(export_file), "[\n");
    json_export_materialy(export_file, p_mat, max_mat);
    json_export_kontejnery(export_file, p_kont, max_kont, p_mat, max_mat);
    json_export_svetla(export_file, p_light, lightnum);
    //fprintf((FILE*)(export_file), "\n]");
  }
}

void json_export_lightmap(int num, bitmapa *p_bmp)
{
  char file[MAX_FILENAME];
  sprintf(file,"%s/%s_%d.bmp",export_level_dir,"lightmap",num);
  bmp_uloz(file, p_bmp);
}

char* name_cleaner(char* name){
  
  /*FILE *pFile;
  pFile = fopen("log", "w");
  if(pFile != NULL){
    fputs("LOG FILE", pFile);
  }
  else{
    return NULL;
  }*/
  
  if(name == NULL) return;
  
  int i = 0;
  char *tempString = malloc(30*sizeof(char));
  //std::string temp_string;
  for(i; i < 30; i++){
    //fputc((int) name[i], pFile);
    //fputc('_', pFile);
    // pokud je nalezen ukoncovaci znak, koncim a vracim string
    if(name[i] == '\0'){
      tempString[i] == '\0';
      return tempString;
    }
    if((int)(name[i]) <= 127 && (int(name[i]) >= 32)){
      tempString[i] = name[i];
    }
    else{
      tempString[i] = '_';
    }
  }
  tempString[30] = '\0';
  return tempString;
}

void json_export_level_item(FILE *f, LEVELINFO * p_Level, ITEMDESC *p_Item)
{
  OBJECTDESC *p_Object = p_Item->p_Object;
  char *cleanedName = name_cleaner(p_Object->Name);
  if(cleanedName == NULL) return;
  fprintf(f,"  {\n");
  fprintf(f,"    \"name\" : \"%s\",\n",cleanedName);
  fprintf(f,"    \"guid\" : \"%d\",\n",p_Object->GUID);
  fprintf(f,"    \"class\" : \"%d\",\n",p_Object->Class);
  fprintf(f,"    \"subclass\" : \"%d\",\n",p_Object->SubClass);
  fprintf(f,"    \"position\" : [ %d, %d, %d ],\n", p_Item->Pos[0], p_Item->Pos[2], p_Item->Pos[1]);
  fprintf(f,"    \"rotation\" : \"%d\",\n", p_Item->Rotation);
  fprintf(f,"    \"container_id\" : \"%d\"\n", kom_get_mesh_id(p_Item->Index_Of_Game_Mesh));
  fprintf(f,"  }");
  free(cleanedName);
}

void json_export_level(LEVELINFO * p_Level)
{
  if(!export_file)
    return;
  
  FILE *f = export_file;
  
  fprintf((FILE*)(export_file), "\n");
  fprintf(f,"{\n");
  fprintf(f,"  \"type\" : \"level_logic\",\n");
  fprintf(f,"  \"logic_level_size\" : [ %d, %d, %d],\n", p_Level->Size[0], p_Level->Size[2], p_Level->Size[1]);
  
  float x_start = finite(p_Level->LevelHeader.x_start) ? p_Level->LevelHeader.x_start : 0.0f;
  float y_start = finite(p_Level->LevelHeader.y_start) ? p_Level->LevelHeader.y_start : 0.0f;
  float z_start = finite(p_Level->LevelHeader.z_start) ? p_Level->LevelHeader.z_start : 0.0f;
  
  fprintf(f,"  \"level_start\" : [ %f, %f, %f],\n", x_start, y_start, z_start);
  fprintf(f,"  \"item_size\" : \"%d\",\n", X_PRVEK);
  fprintf(f,"  \"level_items_num\" : \"%d\",\n", p_Level->Count_Of_Items);
  
  fprintf(f,"  \"level_items\" : [\n", p_Level->Count_Of_Items);
  for (int i = 0; i < p_Level->Count_Of_Items; i++){  
    if(i != 0){
      fprintf(f, ",\n");
    }
    json_export_level_item(f, p_Level, p_Level->Item+i);
  }
  fprintf((FILE*)(export_file), "  ]\n");
  
  fprintf(f,"}\n");
  fprintf((FILE*)(export_file), "\n]\n");
}

void json_export_start(char *p_file)
{
  if(!export_file) {    
    export_file = fopen(p_file,"w");
    getcwd(export_level_dir,MAX_FILENAME);
  }
}

void json_export_end(void)
{
  if(export_file) {
    fclose(export_file);
    export_file = NULL;
  }
}
