/*
  pomocne funkce
*/
#include <string.h>
#include <limits.h>
#include "Apak.h"
#include "Berusky_universal.h"

#include "3d_all.h"

#include "strip.h"

int hlasit_kolize = 0;

#define vyrob_pole(p_dst,num)\
{\
  if(!(p_dst = mmalloc(sizeof(p_dst[0])*num)))\
    chyba("Pamet");\
  memset(p_dst,0,sizeof(p_dst[0])*num);\
}\

#define vyrob_pole_abs(p_dst,num)\
{\
  if(!(p_dst = mmalloc(sizeof(byte)*num)))\
    chyba("Pamet");\
  memset(p_dst,0,sizeof(byte)*num);\
}\

/*
  Koncovky
*/
/* Zameni koncovku, koncovka je zadana vcetne tecky
*/
char * zamen_koncovku(char *p_file, char *p_koncovka)
{
  char *p_pom = p_file,*p_last = NULL;
  while((p_pom = strchr(p_pom,'.'))) {
    p_last = p_pom++;
  }
  if(p_last) *p_last = 0;
  return((p_koncovka) ? strcat(p_file,p_koncovka) : p_file);
}

/* Zameni koncovku, oddelovaci znak se zadava
*/
char * zamen_koncovku_znak(char *p_file, char znak, char *p_koncovka)
{
  char *p_pom = strchr(p_file,znak);
  if(p_pom) *p_pom = 0;  
  return((p_koncovka) ? strcat(p_file,p_koncovka) : p_file);
}

/* Vrati koncovku souboru
*/
char * cti_koncovku(char *p_file)
{
  if(p_file) {
    char *p_pom = strchr(p_file,'.');
    return(p_pom);
  } else {
    return(NULL);
  }
}

/* Vrati znak
*/
char * cti_za_znak(char *p_string, char znak)
{
  char *p_pom = p_string;
  while(*p_pom && *p_pom == znak) p_pom++;
  return(p_pom);
}

/* Mlzna kostka
*/
MLZNA_KOSTKA * vyrob_mlhokostku(char *p_jmeno)
{
  MLZNA_KOSTKA *p_mlha = (MLZNA_KOSTKA *)mmalloc(sizeof(p_mlha[0]));
  
  strcpy(p_mlha->jmeno,p_jmeno);
  p_mlha->poly = K_CHYBA;
  p_mlha->a = 1.0f;
  p_mlha->mod = GL_LINEAR;
  p_mlha->max.x = 10.0f;
  p_mlha->max.y = 10.0f;
  p_mlha->max.z = 10.0f;
  return(p_mlha);
}

MLZNA_KOSTKA * kopiruj_mlhokostku(MLZNA_KOSTKA *p_src, char *p_jmeno)
{
  MLZNA_KOSTKA *p_mlha = (MLZNA_KOSTKA *)mmalloc(sizeof(p_mlha[0]));
  
  *p_mlha = *p_src;
  strcpy(p_mlha->jmeno,p_jmeno);
  
  p_mlha->p_kont = NULL;
  p_mlha->p_next = p_src->p_next;
  p_src->p_next = p_mlha;
  
  return(p_mlha);
}

void zrus_mlhokostku(MLZNA_KOSTKA **p_first, MLZNA_KOSTKA *p_maz)
{
  MLZNA_KOSTKA *p_tmp,*p_prev;

  if(!p_first) {
    p_maz->p_kont = NULL;
  } else {
    if((*p_first) == p_maz) {
      (*p_first) = p_maz->p_next;
    } else {
      p_tmp = (*p_first)->p_next;
      p_prev = (*p_first);
      while(p_tmp) {
        if(p_tmp == p_maz) {
          p_prev->p_next = p_maz->p_next;
          break;
        } else {    
          p_prev = p_tmp;
          p_tmp = p_tmp->p_next;
        }
      }
    }
    free(p_maz);
  }
}

MLZNA_KOSTKA * zrus_mlhokostku_all(MLZNA_KOSTKA *p_first)
{
  MLZNA_KOSTKA *p_tmp;
  while(p_first) {
    p_tmp = p_first->p_next;
    free(p_first);
    p_first = p_tmp;
  }
  return(NULL); 
}

/* **************************************************
  Objekty - vyrobeni / zruseni
*/
EDIT_OBJEKT * vyrob_objekt(void)
{
 EDIT_OBJEKT *p_obj;

 p_obj = (EDIT_OBJEKT *)mmalloc(sizeof(EDIT_OBJEKT));
 p_obj->kreslit = 1;
 p_obj->face_typ = GL_TRIANGLES;
 p_obj->line_typ = GL_LINES;
 init_matrix(&p_obj->m);
// init_matrix(&p_obj->local_matrix);
 return(p_obj);
}

// f = facenum * 3 !!!
EDIT_OBJEKT * vyrob_objekt_komplet(int v, int f)
{
 EDIT_OBJEKT *p_obj;

 p_obj = (EDIT_OBJEKT *) mmalloc(sizeof(EDIT_OBJEKT)); 
 p_obj->p_vertex = (MUJ_BOD *) mmalloc(sizeof(MUJ_BOD)*v);

 if(f) {
   if((p_obj->p_face = (FACE *) mmalloc(sizeof(FACE)*f)) == NULL) {
     chyba("pamet");
   }
 }
 
 p_obj->vertexnum = v;
 p_obj->facenum = f;

 p_obj->kreslit = 1;
 p_obj->face_typ = GL_TRIANGLES;
 p_obj->line_typ = GL_LINES;
 init_matrix(&p_obj->m);
 return(p_obj);
}

void zrus_kosti_key(JOINT_KEYS **p_key)
{ 
  null_free((void **)&(*p_key)->p_pkeys);
  null_free((void **)&(*p_key)->p_pos);
  null_free((void **)&(*p_key)->p_rkeys);
  null_free((void **)&(*p_key)->p_rot);
  null_free((void **)&(*p_key)->p_skeys);
  null_free((void **)&(*p_key)->p_scale);
  null_free((void **)p_key);
  *p_key = NULL;
}

void zrus_kosti_prs_rec(JOINT *p_joint)
{
  if(p_joint) {
    
    if(p_joint->p_child)
      zrus_kosti_prs_rec(p_joint->p_child);
    
    if(p_joint->p_next)
      zrus_kosti_prs_rec(p_joint->p_next);
    
    if(p_joint->p_keys)
      zrus_kosti_key(&(p_joint->p_keys));
    
    p_joint->pos_keys = 0;
    p_joint->p.x = p_joint->p.y = p_joint->p.z = 0.0f;
    null_free((void **)&p_joint->p_pos);
    
    p_joint->rot_keys = 0;
    p_joint->r.x = p_joint->r.y = p_joint->r.z = 0.0f;
    p_joint->r.w = 1.0f;
    null_free((void **)&p_joint->p_rot);
    
    p_joint->scs_keys = 0;
    p_joint->s.x = p_joint->s.y = p_joint->s.z = 1.0f;
    null_free((void **)&p_joint->p_scs);
  }
}

void zrus_kosti_rec(JOINT **p_joint)
{
  if(*p_joint) {
    if((*p_joint)->p_child)
      zrus_kosti_rec(&((*p_joint)->p_child));
    if((*p_joint)->p_next)
      zrus_kosti_rec(&((*p_joint)->p_next));  
    if((*p_joint)->p_keys)
      zrus_kosti_key(&((*p_joint)->p_keys));
    
    null_free((void **)&(*p_joint)->p_vertexlist);
    null_free((void **)&(*p_joint)->p_pos);
    null_free((void **)&(*p_joint)->p_rot);
    null_free((void **)&(*p_joint)->p_scs);
    null_free((void **)&(*p_joint));
    
    p_joint = NULL;
  }
}

void zrus_kosti(EDIT_OBJEKT *p_obj)
{
  JOINT_ANIMACE *p_tmp = p_obj->p_joit_animace, *p_next;
  
  while(p_tmp) {
    p_next = p_tmp->p_next;
    if(p_tmp->p_child)
      zrus_kosti_rec(&p_tmp->p_child);
    free(p_tmp);
    p_tmp = p_next;
  }  
}

void zrus_objekt(EDIT_OBJEKT **p_obj)
{
 zrus_kosti(*p_obj);
 if((*p_obj)->p_tmp_sim) {
   key_sim_zrus_vnitrek((*p_obj)->p_tmp_sim);
   null_free((void **)&(*p_obj)->p_tmp_sim);
 }
 null_free((void **)&(*p_obj)->p_face);
 null_free((void **)&(*p_obj)->p_opt);
 null_free((void **)&(*p_obj)->p_vertex);
 null_free((void **)&(*p_obj)->p_line);
 null_free((void **)p_obj);  
}

int smaz_divoke_vertexy_objektu_dir(EDIT_OBJEKT *p_obj)
{
 OBJ_VERTEX *p_vert = p_obj->p_vertex;
 byte *p_div;
 word *p_face = p_obj->p_face;
 int i,j,vn = p_obj->vertexnum,
     od,fn = p_obj->facenum;

 if(!vn || !fn)
   return(TRUE);

 if((p_div = (byte *)mmalloc(sizeof(byte)*vn)) == NULL) {
   chyba("pamet");
 }
 memset(p_div,1,sizeof(byte)*vn);

 for(i = 0; i < fn; i+=3) {
    p_div[p_face[i]] = p_div[p_face[i+1]] = p_div[p_face[i+2]] = 0;    
 }

 for(i = 0, od = 0; i < vn+od; i++) {
   if(p_div[i]) {
     smaz_vertex_bez_facu(p_vert,vn,(i-od));

     for(j = 0; j < fn; j+=3) {
       if(p_face[j] > i-od)
         p_face[j]--;
       if(p_face[j+1] > i-od)
         p_face[j+1]--;
       if(p_face[j+2] > i-od)
         p_face[j+2]--;
     }

     od++;
   }
 }
 
 free(p_div);
 
 p_obj->vertexnum -= od; 
 return(od);
}

EDIT_OBJEKT * kopiruj_objekt(EDIT_OBJEKT *p_src)
{
 EDIT_OBJEKT *p_cil;

 if(!p_src)
   return(p_src);

 p_cil = (EDIT_OBJEKT *)kopiruj_pole(p_src,sizeof(p_src[0])); 

 p_cil->p_vertex = (OBJ_VERTEX *)kopiruj_pole(p_src->p_vertex,sizeof(OBJ_VERTEX)*p_cil->vertexnum);
 p_cil->p_vertex_kosti = (BOD *)kopiruj_pole(p_src->p_vertex_kosti,sizeof(p_src->p_vertex_kosti[0])*p_cil->vertexnum);
 p_cil->p_face = (FACE *)kopiruj_pole(p_src->p_face,sizeof(FACE)*p_cil->facenum);
 p_cil->p_opt = (int *)kopiruj_pole(p_src->p_opt,sizeof(p_src->p_opt[0])*p_src->optnum);
 
 p_cil->p_line = NULL;
 p_cil->p_joit_animace = key_kosti_kopiruj_animace(p_src->p_joit_animace);

 return(p_cil);
}

/* kopiruje 
*/
void kopiruj_objekt_geometrie(EDIT_OBJEKT *p_src, EDIT_OBJEKT *p_cil)
{
 if(!p_src)
   return;

 free(p_cil->p_vertex);
 p_cil->vertexnum = p_src->vertexnum;
 p_cil->p_vertex = (OBJ_VERTEX *)mmalloc(sizeof(OBJ_VERTEX)*p_cil->vertexnum);
// mtest(p_cil->p_vertex);
 memcpy(p_cil->p_vertex,p_src->p_vertex,sizeof(OBJ_VERTEX)*p_cil->vertexnum);

 free(p_cil->p_face);
 p_cil->facenum = p_src->facenum;
 p_cil->p_face = (FACE *)mmalloc(sizeof(FACE)*p_cil->facenum);
// mtest(p_cil->p_face);
 memcpy(p_cil->p_face,p_src->p_face,sizeof(FACE)*p_cil->facenum);
}

EDIT_OBJEKT * kopiruj_objekt_indir_vyber(EDIT_OBJEKT *p_src, EDIT_OBJEKT *p_cil, int flag)
{
 int i;

 if(!p_src || !p_cil)
   return(NULL);

 for(i = 0; i < p_cil->vertexnum; i++) {
   if(flag&COPY_GEOMETRY) {
     p_cil->p_vertex[i].x = p_src->p_vertex[i].x;
     p_cil->p_vertex[i].y = p_src->p_vertex[i].y;
     p_cil->p_vertex[i].z = p_src->p_vertex[i].z;
   }
   if(flag&COPY_DIFFUSE) {
     p_cil->p_vertex[i].dr = p_src->p_vertex[i].dr;
     p_cil->p_vertex[i].dg = p_src->p_vertex[i].dg;
     p_cil->p_vertex[i].db = p_src->p_vertex[i].db;
     p_cil->p_vertex[i].da = p_src->p_vertex[i].da;
     p_cil->p_vertex[i].mdr = p_src->p_vertex[i].mdr;
     p_cil->p_vertex[i].mdg = p_src->p_vertex[i].mdg;
     p_cil->p_vertex[i].mdb = p_src->p_vertex[i].mdb;
     p_cil->p_vertex[i].mda = p_src->p_vertex[i].mda;
   }
   if(flag&COPY_SPECULAR) {
     p_cil->p_vertex[i].sr = p_src->p_vertex[i].sr;
     p_cil->p_vertex[i].sg = p_src->p_vertex[i].sg;
     p_cil->p_vertex[i].sb = p_src->p_vertex[i].sb;
     p_cil->p_vertex[i].msr = p_src->p_vertex[i].msr;
     p_cil->p_vertex[i].msg = p_src->p_vertex[i].msg;
     p_cil->p_vertex[i].msb = p_src->p_vertex[i].msb;
   }
   if(flag&COPY_MAP1) {
     p_cil->p_vertex[i].tu1 = p_src->p_vertex[i].tu1;
     p_cil->p_vertex[i].tv1 = p_src->p_vertex[i].tv1;
   }
   if(flag&COPY_MAP2) {
     p_cil->p_vertex[i].tu2 = p_src->p_vertex[i].tu2;
     p_cil->p_vertex[i].tv2 = p_src->p_vertex[i].tv2;
   }
 }

 if(flag&COPY_MATERIAL) {
   p_cil->material = p_src->material;
 }

 return(p_cil);
}

EDIT_OBJEKT * slep_objekty(EDIT_OBJEKT *p_obj1, EDIT_OBJEKT *p_obj2)
{

 EDIT_OBJEKT *p_obj = vyrob_objekt();
 int          i;
 int          f1,f2;

 if(strlen(p_obj1->jmeno)+strlen(p_obj2->jmeno)+1 >= MAX_JMENO) {
   strcpy(p_obj->jmeno,p_obj1->jmeno);
 } else {
   sprintf(p_obj->jmeno,"%s_%s",p_obj1->jmeno,p_obj2->jmeno);
 }

 assert(strlen(p_obj->jmeno) < MAX_JMENO);

 p_obj->vertexnum = p_obj1->vertexnum+p_obj2->vertexnum;
 if((p_obj->p_vertex = (OBJ_VERTEX *) malloc(sizeof(OBJ_VERTEX)*p_obj->vertexnum)) == NULL) {
   zrus_objekt(&p_obj);
   return(NULL);
 }
 memcpy(p_obj->p_vertex,p_obj1->p_vertex,sizeof(OBJ_VERTEX)*p_obj1->vertexnum);
 memcpy(p_obj->p_vertex+p_obj1->vertexnum,p_obj2->p_vertex,sizeof(OBJ_VERTEX)*p_obj2->vertexnum);


 p_obj->facenum = p_obj1->facenum+p_obj2->facenum;
 if((p_obj->p_face = (FACE *) mmalloc(sizeof(FACE)*p_obj->facenum)) == NULL) {
   zrus_objekt(&p_obj);
   return(NULL);
 }
 memcpy(p_obj->p_face,p_obj1->p_face,sizeof(FACE)*p_obj1->facenum);
 
 f1 = p_obj1->facenum;
 f2 = (p_obj2->facenum+f1);
 for(i = f1; i < f2; i++)
    p_obj->p_face[i] = p_obj1->vertexnum + p_obj2->p_face[i-f1];

 p_obj->material = p_obj1->material;
 p_obj->m1flag = p_obj1->m1flag;
 p_obj->m2flag = p_obj1->m2flag;
 p_obj->oflag = p_obj1->oflag;

 return(p_obj);
}

/* **************************************************
  Kontejner
*/
EDIT_KONTEJNER * vyrob_kontejner(void)
{
 EDIT_KONTEJNER *p_kont;

 if((p_kont = (EDIT_KONTEJNER *)mmalloc(sizeof(EDIT_KONTEJNER))) == NULL)
   chyba("pamet!");

 memset(p_kont,0,sizeof(p_kont[0]));
 p_kont->max_objektu = MAX_KONT_OBJEKTU;
 p_kont->kreslit = 1;
 p_kont->prvek = K_CHYBA;
 p_kont->kflag |= KONT_STATIC;
 p_kont->kontejner_ID = K_CHYBA; // defaultni id kontejneru
 init_matrix(&p_kont->world);    // globalni transformacni matice
 init_matrix(&p_kont->mpmatrix); // lokalni transformacni matice (mp matrix)
 return(p_kont);
}

void zrus_kontejner(EDIT_KONTEJNER **pp_kont, MLZNA_KOSTKA **p_first)
{
 int i;
 EDIT_KONTEJNER *p_kont;

 p_kont = *pp_kont;
 for(i = 0; i < p_kont->max_objektu; i++) {
   if(p_kont->p_obj[i]) {
     zrus_objekt(&p_kont->p_obj[i]);
   }
 }
 if(p_kont->p_mlha)
   zrus_mlhokostku(p_first,p_kont->p_mlha);
 for(i = 0; i < KONT_MAX_ANIM; i++) {
   key_sim_root_zrus(p_kont->sim+i);
 }
 free(p_kont);
 *pp_kont = NULL;
}

void zrus_kontejner_rec(EDIT_KONTEJNER **pp_kont, MLZNA_KOSTKA **p_first)
{
 int i;
 EDIT_KONTEJNER *p_kont,*p_next;

 p_kont = *pp_kont;
 while(p_kont) {
   
   for(i = 0; i < p_kont->max_objektu; i++) {
     if(p_kont->p_obj[i]) {
       zrus_objekt(&p_kont->p_obj[i]);
     }
   }   

   if(p_kont->p_mlha)
     zrus_mlhokostku(p_first,p_kont->p_mlha);
   
   for(i = 0; i < KONT_MAX_ANIM; i++) {
     key_sim_root_zrus(p_kont->sim+i);
   }
   
   p_next = p_kont->p_next;
   free(p_kont);
   p_kont = p_next;
 }
 *pp_kont = NULL;
}

void vyprazdni_kontejner(EDIT_KONTEJNER *p_kont)
{
  int i;

  if(!p_kont)
    return;
  
  for(i = 0; i < p_kont->max_objektu; i++) {
    if(p_kont->p_obj[i])
      zrus_objekt(&p_kont->p_obj[i]);
  }
  updatuj_kontejner_statistika(p_kont,TRUE);
}

void updatuj_kontejner_flag(EDIT_KONTEJNER *p_kont, EDIT_MATERIAL **p_mat)
{  
  int o,m;

  while(p_kont) {
    p_kont->m1flag = p_kont->m2flag = 0;
    for(o = 0; o < p_kont->max_objektu; o++) {
      if(p_kont->p_obj[o]) {
        m = p_kont->p_obj[o]->material;
        if(p_mat[m]) {
          p_kont->p_obj[o]->m1flag |= p_mat[m]->flag;
          p_kont->p_obj[o]->m2flag |= p_mat[m]->flag2;
        }
        p_kont->m1flag |= p_kont->p_obj[o]->m1flag;
        p_kont->m2flag |= p_kont->p_obj[o]->m2flag;
        p_kont->kflag |= p_kont->p_obj[o]->oflag;
      }
    }
    p_kont = p_kont->p_next;
  }
}

void updatuj_obalku_kontejner(EDIT_KONTEJNER *p_kont)
{  
  int i;
  
  if(!p_kont)
    return;
  
  for(i = 0; i < p_kont->max_objektu; i++) {
    if(p_kont->p_obj[i]) {
      //obb_calc_obj_fast(p_kont->p_obj[i]);
    }
  }

  kontejner_obalka(p_kont);
}

void updatuj_kontejner_statistika(EDIT_KONTEJNER *p_kont, int obalky)
{
 int i,j,f,v;
 
 while(p_kont) {      
   for(i = 0, j = 0, f = 0, v = 0; i < p_kont->max_objektu; i++) {
     if(p_kont->p_obj[i]) {
       f += p_kont->p_obj[i]->facenum;
       v += p_kont->p_obj[i]->vertexnum;
       p_kont->m1flag |= p_kont->p_obj[i]->m1flag;
       p_kont->m2flag |= p_kont->p_obj[i]->m2flag;
       p_kont->kflag |= p_kont->p_obj[i]->oflag;
       j++;
     }
   }
   p_kont->objektu = j;
   p_kont->facu = f;
   p_kont->bodu = v; 
   
   vycisti_kontejner(p_kont);
   if(obalky)
     updatuj_obalku_kontejner(p_kont);

   p_kont = p_kont->p_next;
 }
}

void smaz_divoke_vertexy_kontejneru(EDIT_KONTEJNER *p_kont)
{
 int i;
 if(p_kont) {
   for(i = 0; i < p_kont->max_objektu; i++) {
     if(p_kont->p_obj[i] && !p_kont->p_obj[i]->p_joit_animace)
       smaz_divoke_vertexy_objektu_dir(p_kont->p_obj[i]);
   }
 }
}

void vycisti_kontejner(EDIT_KONTEJNER *p_kont)
{
  int i;

  if(!p_kont)
    return;
  
  smaz_divoke_vertexy_kontejneru(p_kont);
  for(i = 0; i < p_kont->max_objektu; i++) {
    if(p_kont->p_obj[i]&&(!p_kont->p_obj[i]->vertexnum||!p_kont->p_obj[i]->facenum))
      zrus_objekt(&p_kont->p_obj[i]);
  }  
}

// posune objekty v kontejneru k sobe a pojmenuje je jednotne
void preindexuj_kontejner(EDIT_KONTEJNER *p_kont)
{
 int  i,j;

 for(i = 0, j = 0; i < p_kont->max_objektu; i++) {
   if(!p_kont->p_obj[i])
     continue;
   if(j != i) {
     p_kont->p_obj[j] = p_kont->p_obj[i];
     p_kont->p_obj[i] = NULL;
   }
   j++;
 }
}

int slep_kontejner(EDIT_KONTEJNER *p_kont)
{
 EDIT_OBJEKT    *p_obj;
 int           i,j,ob;

 // 1000 je MAX_EDIT_MATERIALU
 for(i = 0; i < 1000 /*MAX_EDIT_MATERIALU*/; i++) {
   for(j = 0; j < p_kont->max_objektu; j++) {
     if(p_kont->p_obj[j] && p_kont->p_obj[j]->material == i) {
       ob = j;
       break;
     }
   }
   
   if(j == p_kont->max_objektu)
     continue;
   
   for(j = ob+1; j < p_kont->max_objektu; j++) {
     if(p_kont->p_obj[j] && p_kont->p_obj[j]->material == i) {
       p_obj = slep_objekty(p_kont->p_obj[ob],p_kont->p_obj[j]);
       zrus_objekt(&p_kont->p_obj[ob]);
       zrus_objekt(&p_kont->p_obj[j]);
       p_kont->p_obj[ob] = p_obj;
     }
   }
 }
 
 updatuj_kontejner_statistika(p_kont,TRUE);
 preindexuj_kontejner(p_kont);

 return(TRUE);
}

/*
  Slepi 2 objekty, pokud jsou od sebe vzdaleny mene nez min_dist 
*/

#define SLEP_FACE_LIMIT 500

int slep_kontejner_dist(EDIT_KONTEJNER *p_kont, float min_dist)
{
 GLMATRIX    *p_tmp;
 EDIT_OBJEKT *p_obj1,*p_obj2;
 int          i1,i2;

 p_tmp = kont_world_matrix(p_kont);

 // vraci pocet unikatnich materialu
 lo_setrid_kontejner(p_kont);

 for(i1 = 0, i2 = 1; i2 < p_kont->max_objektu;) {
   p_obj1 = p_kont->p_obj[i1];
   p_obj2 = p_kont->p_obj[i2];

   if(!p_obj2 || p_obj2->p_joit_animace) {
     i2++;
     continue;
   }

   if(!p_obj1 || p_obj1->p_joit_animace || p_obj1->material != p_obj2->material) {
     i1++;
     i2++;
     continue;
   }

   if(p_obj1->facenum + p_obj2->facenum > SLEP_FACE_LIMIT) {
     i1++;
     i2++;
     continue;
   }
/*
   if(vzdal_bodu_bod(&p_obj1->obb.obb_stred, &p_obj2->obb.obb_stred) < min_dist) {
     p_kont->p_obj[i1] = slep_objekty(p_obj1,p_obj2);
     p_kont->p_obj[i2] = NULL;     
     obb_calc_obj_fast(p_kont->p_obj[i1]);
     zrus_objekt(&p_obj1);
     zrus_objekt(&p_obj2);
     i2++;
   } else {
     i1++;
     i2++;
   }
 */
 }

 preindexuj_kontejner(p_kont);
 updatuj_kontejner_statistika(p_kont,TRUE);

 return(TRUE);
}

/* Kopie kontejneru */
EDIT_KONTEJNER * kopiruj_kontejner(EDIT_KONTEJNER *p_src, EDIT_MATERIAL **p_smat, 
                                   EDIT_MATERIAL **p_dmat, int dmatnum,
                                   STATIC_LIGHT *p_slight, int slightnum, 
                                   DYN_LIGHT *p_dlight, int dlightnum, 
                                   EXTRA_DYN_LIGHT *p_elight, int elightnum,
                                   LENS_FLARE *p_flare, int maxflares)
{
  EDIT_KONTEJNER *p_dest = NULL;
  EDIT_KONTEJNER *p_dest_top = NULL;
  int i,mat;

  while(p_src) {
  
    if(!p_dest) {
      p_dest_top = p_dest = vyrob_kontejner();
    } else {
      p_dest = (p_dest->p_next = vyrob_kontejner());
    }
    
    *p_dest = *p_src;
    if(p_src->p_mlha) {
      p_dest->p_mlha = kopiruj_mlhokostku(p_src->p_mlha,p_dest->jmeno);
      p_dest->p_mlha->p_kont = p_dest;
    }

    
    // Prekopiruju svetla
    lo_kopiruj_svetla_do_sceny(p_dest, p_slight, slightnum, p_dlight, dlightnum, 
                               p_elight, elightnum, p_flare, maxflares);
    
    for(i = 0; i < MAX_KONT_OBJEKTU; i++) {
      if(p_src->p_obj[i]) {
        p_dest->p_obj[i] = kopiruj_objekt(p_src->p_obj[i]);
        if(p_smat) {
          mat = lo_najdi_material(p_dmat,dmatnum,p_smat[p_src->p_obj[i]->material]->jmeno);
          if(mat == K_CHYBA) {
            mat = lo_najdi_prazdny_material(p_dmat,dmatnum);
            if(mat == K_CHYBA)
              continue;
            p_dmat[mat] = kopiruj_material(p_smat[p_src->p_obj[i]->material]);
          }
          p_dest->p_obj[i]->material = mat;
        }
      }
    }
    
    // kopie stromu animacniho stromu ...
    if(p_src->kflag&KONT_KEYFRAME) {
      for(i = 0; i < KONT_MAX_ANIM; i++) {
        if(p_src->sim[i].keynum)
          key_sim_root_to_sim_root_indir(p_dest, p_dest->sim+i, p_src->sim+i);
      }
    }
    
    p_src = p_src->p_next;
  }
  return(p_dest_top);
}

EDIT_KONTEJNER * kopiruj_kontejner_indir_vyber(EDIT_KONTEJNER *p_src, EDIT_KONTEJNER *p_dest, int flag)
{
  int i;  

  for(i = 0; i < MAX_KONT_OBJEKTU; i++) {
    if(p_src->p_obj[i] && p_dest->p_obj[i]) {
      kopiruj_objekt_indir_vyber(p_src->p_obj[i],p_dest->p_obj[i],flag);
    }
  }
    
  return(p_dest);
}

int lo_setrid_kontejner_compare(const int *p_obj1, const int *p_obj2)
{
/*
  int   o1 = INT_MAX, o2 = INT_MAX;
  float d1,d2;

  if(*p_obj1) {
    o1 = ((EDIT_OBJEKT *)*p_obj1)->material;
  }

  if(*p_obj2) {
    o2 = ((EDIT_OBJEKT *)*p_obj2)->material;
  }

  if(!(*p_obj1) || !(*p_obj2)) {
    if(o1 < o2)
      return(-1);
    if(o1 == o2)
      return(0);    
    else 
      return(1);
  }

  if(o1 < o2) // o1 je mensi nez o2
    return(-1);
  if(o1 == o2) {
    d1 = vektor_velikost(&(((EDIT_OBJEKT *)*p_obj1)->obb.obb_stred));
    d2 = vektor_velikost(&(((EDIT_OBJEKT *)*p_obj2)->obb.obb_stred));
    if(d1 < d2) 
      return(-1);
    if(d1 > d2)
      return(1);
    else
      return(0);
  }
  else 
    return(1); // o2 je vetsi nez o1
*/
}

/* setridi staticky kontejner podle pouzitych materialu 
   da stejne materily k sobe
*/
int lo_setrid_kontejner(EDIT_KONTEJNER *p_kont)
{ 
  GLMATRIX *p_tmp;
  int mat = 0,i,
      am = INT_MAX;
/*
  if(p_kont->kflag&KONT_KEYFRAME)
    return(0);

  p_tmp = kont_world_matrix(p_kont);
  for(i = 0; i < p_kont->max_objektu; i++) {
    if(p_kont->p_obj[i])
      obb_calc_obj_fast(p_kont->p_obj[i]);
  }  

  qsort((void *)p_kont->p_obj,(size_t)p_kont->max_objektu,
    (size_t)sizeof(EDIT_OBJEKT *), lo_setrid_kontejner_compare);

  for(i = 0; i < MAX_KONT_OBJEKTU; i++) {
    if(p_kont->p_obj[i] && p_kont->p_obj[i]->material != am) {
      am = p_kont->p_obj[i]->material;
      mat++;
    }
  }
*/
  return(mat);
}

/* **************************************************************************
   Setrideni kontejneru podle materialu
*/
int lo_setrid_kontejner_materialy_compare(const int *p_obj1, const int *p_obj2)
{
  int o1 = INT_MAX, o2 = INT_MAX;

  if(*p_obj1) {
    o1 = ((EDIT_OBJEKT *)*p_obj1)->material;
  }

  if(*p_obj2) {
    o2 = ((EDIT_OBJEKT *)*p_obj2)->material;
  }

  if(!(*p_obj1) || !(*p_obj2)) {
    if(o1 < o2)
      return(-1);
    if(o1 == o2)
      return(0);    
    else 
      return(1);
  }
  return(o1 - o2);
}

/* setridi staticky kontejner podle pouzitych materialu 
   da stejne materily k sobe
*/
void lo_setrid_kontejner_materialy(EDIT_KONTEJNER *p_kont)
{
/*
  if(!(p_kont->kflag&KONT_KEYFRAME)) {
    qsort((void *)p_kont->p_obj,(size_t)p_kont->max_objektu,
          (size_t)sizeof(EDIT_OBJEKT *), lo_setrid_kontejner_materialy_compare);
  }
*/
}


/* **************************************************
  Animace
*/
void zrus_tri_vnitrek(HIERARCHY_TRACK_INFO *p_tri)
{
  null_free((void **)&p_tri->p_pkeys);
  null_free((void **)&p_tri->p_pos);
  null_free((void **)&p_tri->p_rkeys);
  null_free((void **)&p_tri->p_at);
  null_free((void **)&p_tri->p_scale);
  null_free((void **)&p_tri->p_skeys);
}

void zrus_tri(HIERARCHY_TRACK_INFO **p_trii)
{
  zrus_tri_vnitrek(*p_trii);
  free(*p_trii);
  *p_trii = NULL;
}


void key_root_zrus_rec(HIERARCHY_TRACK_INFO *p_tri)
{
  int i;
  for(i = 0; i < p_tri->childnum; i++) {
    assert(p_tri->p_child[i]);
    key_root_zrus_rec(p_tri->p_child[i]);
  }
  zrus_tri(&p_tri);
}

// zrusi animacni strom
void key_root_zrus(HIERARCHY_ROOT *p_root)
{
  int i;
  if(p_root->framenum) {
    for(i = 0; i < p_root->childnum; i++) {
      assert(p_root->p_child[i]);
      key_root_zrus_rec(p_root->p_child[i]);
    }  
    memset(p_root,0,sizeof(*p_root));
  }
}

/* *******************************************************
  Materialy
*/

EDIT_MATERIAL * vyrob_material(void)
{
 EDIT_MATERIAL * p_mat;
 int i;

 if((p_mat = (EDIT_MATERIAL *) mmalloc(sizeof(EDIT_MATERIAL))) == NULL) {
   chyba("pamet");
 }
 memset(p_mat,0,sizeof(EDIT_MATERIAL)); 

 p_mat->dxmat.diffuse_r = 1.0f;
 p_mat->dxmat.diffuse_g = 1.0f;
 p_mat->dxmat.diffuse_b = 1.0f;
 p_mat->dxmat.diffuse_a = 1.0f;
 
 p_mat->dxmat.specular_r = 0.0f;
 p_mat->dxmat.specular_g = 0.0f;
 p_mat->dxmat.specular_b = 0.0f;

 p_mat->dxmat.faktor_r = 1.0f;
 p_mat->dxmat.faktor_g = 1.0f;
 p_mat->dxmat.faktor_b = 1.0f;
 p_mat->dxmat.faktor_a = 1.0f;

 p_mat->lesk = 1.0f;
  
 p_mat->flag2 |= (MAT2_DIFFUSE|MAT2_DIFFUSE_RAY|MAT2_MAP1);
/*
 mat_default_stage_bloky(p_mat);
*/
 for(i = 0; i < MAT_TEXTUR; i++) {
   init_matrix(p_mat->ttext+i); 
 }

 p_mat->env_scale = 1.0f;

 return(p_mat);
}

void zrus_material(EDIT_MATERIAL **p_mat)
{
  int i,j;

  if((*p_mat)->flag&MAT_ANIM_FRAME)
    lo_zrus_animaci(&(*p_mat)->anim);
  
  for(j = 0; j < MAT_TEXTUR; j++) {
    for(i = 0; i < MAX_TEXT_ANIM; i++) {
      if((*p_mat)->p_atext[j][i])
        key_zrus_material_animace((*p_mat)->p_atext[j]+i);
    }
  }

  if((*p_mat)->p_sub)
    zrus_material(&((*p_mat)->p_sub));

  null_free((void **)p_mat);
 *p_mat = NULL;
}

void lo_zrus_material_kont(EDIT_KONTEJNER *p_kont, EDIT_MATERIAL **p_list, int num)
{
  EDIT_OBJEKT *p_obj;
  int o;

  oe_olist_reset(&o);
  while((p_obj = oe_olist_next(p_kont,&o))) {
    if(p_obj->material < num && p_list[p_obj->material])
      zrus_material(p_list+p_obj->material);
  }
}

EDIT_MATERIAL * kopiruj_material_indir(EDIT_MATERIAL *p_src, EDIT_MATERIAL *p_dest)
{
 int i,j;
 
 if(!p_src || !p_dest) return(NULL);

 memcpy(p_dest,p_src,sizeof(EDIT_MATERIAL));

 // kopie animaci
 if(p_src->flag&MAT_ANIM_FRAME) {
 /*
   p_dest->anim.p_frame = mmalloc(sizeof(ANIM_FRAME)*p_dest->anim.framenum);
   memcpy(p_dest->anim.p_frame,p_src->anim.p_frame,sizeof(ANIM_FRAME)*p_dest->anim.framenum);
 */
 }

 // kopie animaci 1
 for(j = 0; j < MAT_TEXTUR; j++) {
   for(i = 0; i < MAX_TEXT_ANIM; i++) {
     if(p_src->p_atext[j][i])
       p_dest->p_atext[j][i] = key_kopiruj_material_animace(p_src->p_atext[j][i]);
   }
 }

 p_dest->p_sub = NULL;
 return(p_dest);
}

EDIT_MATERIAL * kopiruj_material(EDIT_MATERIAL *p_src)
{
 EDIT_MATERIAL *p_dest;
 if(!p_src) return(NULL);
/*
 p_dest = mmalloc(sizeof(EDIT_MATERIAL));
 return(kopiruj_material_indir(p_src,p_dest));
*/
}


/* ****************************************************
   Dalsi
*/


#define miror(promena)\
{\
  for(i = 0; i < p_dest->vertexnum; i++) {\
    stred += promena;\
  }\
  stred /= (float)p_dest->vertexnum;\
  for(i = 0; i < p_dest->vertexnum; i++) {\
    if(promena > stred)\
      promena -= (promena - stred);\
    else\
      promena += (stred - promena);\
  }     \
}\


/* *******************************************************
 Mesh 
*/


GAME_MESH * kopiruj_mesh(GAME_MESH *p_src, GAME_MESH_DATA *p_data)
{
/*
  GAME_MESH * p_dest = vyrob_mesh(p_src->objektu,p_data);
  int i;

  if(!p_dest) chyba("Pamet!");

  if(!p_data)
    kopiruj_mesh_data(p_src->p_data, p_dest, p_dest->p_data);

  p_dest->vertexnum = p_src->vertexnum;
  p_dest->objektu = p_src->objektu;
  p_dest->facevel = p_src->facevel;

  p_dest->p_vertex_pos = kopiruj_pole(p_src->p_vertex_pos,sizeof(p_src->p_vertex_pos[0])*p_src->vertexnum);
  p_dest->p_vertex_norm = kopiruj_pole(p_src->p_vertex_norm,sizeof(p_dest->p_vertex_norm[0])*p_src->vertexnum);
  p_dest->p_vertex_diff = kopiruj_pole(p_src->p_vertex_diff,sizeof(p_dest->p_vertex_diff[0])*p_src->vertexnum);
  p_dest->p_vertex_diff_material = kopiruj_pole(p_src->p_vertex_diff_material,sizeof(p_dest->p_vertex_diff_material[0])*p_src->vertexnum);
  p_dest->p_vertex_spec = kopiruj_pole(p_src->p_vertex_spec,sizeof(p_dest->p_vertex_spec[0])*p_src->vertexnum);
  p_dest->p_vertex_spec_material = kopiruj_pole(p_src->p_vertex_spec_material,sizeof(p_dest->p_vertex_spec_material[0])*p_src->vertexnum);
  p_dest->p_vertex_uv1_material = kopiruj_pole(p_src->p_vertex_uv1_material,sizeof(p_dest->p_vertex_uv1_material[0])*p_src->vertexnum);
  p_dest->p_vertex_uv2_material = kopiruj_pole(p_src->p_vertex_uv2_material,sizeof(p_dest->p_vertex_uv2_material[0])*p_src->vertexnum);

  memcpy(p_dest->objektnum,p_src->objektnum,sizeof(p_src->objektnum[0])*p_src->objektu);
  memcpy(p_dest->objektstart,p_src->objektstart,sizeof(p_src->objektstart[0])*p_src->objektu);
  memcpy(p_dest->facenum,p_src->facenum,sizeof(p_src->facenum[0])*p_src->objektu);
  memcpy(p_dest->facestart,p_src->facestart,sizeof(p_src->facestart[0])*p_src->objektu);
  memcpy(p_dest->p_mat,p_src->p_mat,sizeof(p_src->p_mat[0])*p_src->objektu);
    
  p_dest->p_face = mmalloc(sizeof(p_dest->p_face[0])*p_src->facevel);    
  memcpy(p_dest->p_face,p_src->p_face,sizeof(p_dest->p_face[0])*p_src->facevel);
  

  p_dest->simnum = p_src->simnum;

  if(p_dest->simnum) {
    p_dest->p_sim = mmalloc(sizeof(p_dest->p_sim[0])*p_dest->simnum);
    for(i = 0; i < p_dest->simnum; i++) {
      key_sim_to_sim_indir(p_dest->p_sim+i, p_src->p_sim+i);
    }
  }

  return(p_dest);
  */
}

/* Kopie Mesh-Dat
*/
GAME_MESH_DATA * kopiruj_mesh_data(GAME_MESH_DATA *p_src, GAME_MESH *p_mesh_top, GAME_MESH_DATA *p_desc)
{  
  struct _STATIC_LIGHT    *p_lss,*p_lsd;
  struct _DYN_LIGHT       *p_lds,*p_ldd;
  struct _EXTRA_DYN_LIGHT *p_les,*p_led;

  if(!p_src || !p_desc)
    return(NULL);
/*
  p_desc->p_mesh = p_mesh_top;
  p_desc->kflag  = p_src->kflag;
  p_desc->m1flag = p_src->m1flag;
  p_desc->m2flag = p_src->m2flag;
  p_desc->mesh_handle = p_src->mesh_handle;
  p_desc->mesh_ID = p_src->mesh_ID;

  p_desc->slightnum = p_src->slightnum;
  p_desc->lightmax  = p_src->lightmax;
  p_desc->lightakt  = p_src->lightakt;

  if(p_src->lightmax && p_src->p_light) {
    p_desc->p_light = mmalloc(sizeof(void *)*p_src->lightmax);
    memcpy(p_desc->p_light,p_src->p_light,sizeof(void *)*p_src->lightmax);
  }

  p_desc->top_edlight = p_src->top_edlight;
  p_desc->edlightnum = p_src->edlightnum;
  p_desc->edlightmax = p_src->edlightmax;
  
  if(p_src->edlightmax && p_src->p_edlight) {
    p_desc->p_edlight = mmalloc(sizeof(p_desc->p_edlight[0])*p_src->edlightnum);
    memcpy(p_desc->p_edlight,p_src->p_edlight,sizeof(p_desc->p_edlight[0])*p_src->edlightnum);
  }

  // Kopie statickych svetel
  if(p_src->p_lslight) {
    p_desc->p_lslight = mmalloc(sizeof(p_src->p_lslight[0]));
    memcpy(p_desc->p_lslight,p_src->p_lslight,sizeof(p_src->p_lslight[0]));
    p_lss = p_src->p_lslight->p_next;
    p_lsd = p_desc->p_lslight;
    while(p_lss) {
      p_lsd->p_next = mmalloc(sizeof(p_lsd->p_next[0]));
      memcpy(p_lsd->p_next,p_lss,sizeof(p_lss[0]));
      p_lss = p_lss->p_next;
      p_lsd = p_lsd->p_next;
    }
  }

  // Kopie dynamickych svetel
  if(p_src->p_ldlight) {
    p_desc->p_ldlight = mmalloc(sizeof(p_src->p_ldlight[0]));
    memcpy(p_desc->p_ldlight,p_src->p_ldlight,sizeof(p_src->p_ldlight[0]));
    p_lds = p_src->p_ldlight->p_next;
    p_ldd = p_desc->p_ldlight;
    while(p_lds) {
      p_ldd->p_next = mmalloc(sizeof(p_ldd->p_next[0]));
      memcpy(p_ldd->p_next,p_lss,sizeof(p_lds[0]));
      p_lds = p_lds->p_next;
      p_ldd = p_ldd->p_next;
    }
  }

  // Kopie extra-svetel
  if(p_src->p_lelight) {
    p_desc->p_lelight = mmalloc(sizeof(p_src->p_lelight[0]));
    memcpy(p_desc->p_lelight,p_src->p_lelight,sizeof(p_src->p_lelight[0]));
    p_les = p_src->p_lelight->p_next;
    p_led = p_desc->p_lelight;
    while(p_les) {
      p_led->p_next = mmalloc(sizeof(p_led->p_next[0]));
      memcpy(p_led->p_next,p_les,sizeof(p_les[0]));
      p_les = p_les->p_next;
      p_led = p_led->p_next;
    }
  }
*/
  return(p_desc);
}


/*
 word                    *facenum;     // pocty facu v extra-facich - polich
 word                    *facestart;   // starty facu jednotlivych objektu
 EXTRA_FACE             **p_face;
 int                     
*/

void rozsir_objekt_light(EDIT_OBJEKT *p_obj)
{  
  MUJ_BOD v,*p_v1,*p_v2;
  MUJ_BOD nov[5];
  word    face[3*4],f[6];
  int i,j,p[6],n,num = 0;

  if(p_obj->vertexnum != 4 || p_obj->facenum != 6)
    return;

  for(i = 0; i < 6; i++) {
    for(j = i; j < 6; j++) {
      if(i == j)
        continue;
      if(p_obj->p_face[i] == p_obj->p_face[j]) {
        p[num] = p_obj->p_face[i];
        num++;
      }
    }
  }
/*
  if(num == 2 && vzdal_bodu(p_obj->p_vertex+p[0],p_obj->p_vertex+p[1]) > 2.0f) {
    p_v1 = p_obj->p_vertex+p[0];
    p_v2 = p_obj->p_vertex+p[1];
    
    v.x = p_v1->x*0.5f + p_v2->x*0.5f;
    v.y = p_v1->y*0.5f + p_v2->y*0.5f;
    v.z = p_v1->z*0.5f + p_v2->z*0.5f;
    
    v.nx = p_v1->nx*0.5f + p_v2->nx*0.5f;
    v.ny = p_v1->ny*0.5f + p_v2->ny*0.5f;
    v.nz = p_v1->nz*0.5f + p_v2->nz*0.5f;

    v.tu1 = p_v1->tu1*0.5f + p_v2->tu1*0.5f;
    v.tu2 = p_v1->tu2*0.5f + p_v2->tu2*0.5f;
    v.tv1 = p_v1->tv1*0.5f + p_v2->tv1*0.5f;
    v.tv2 = p_v1->tv2*0.5f + p_v2->tv2*0.5f;
    
    v.dr = p_v1->dr*0.5f + p_v2->dr*0.5f;
    v.dg = p_v1->dg*0.5f + p_v2->dg*0.5f;
    v.db = p_v1->db*0.5f + p_v2->db*0.5f;
    v.da = p_v1->da*0.5f + p_v2->da*0.5f;

    v.mdr = p_v1->mdr*0.5f + p_v2->mdr*0.5f;
    v.mdg = p_v1->mdg*0.5f + p_v2->mdg*0.5f;
    v.mdb = p_v1->mdb*0.5f + p_v2->mdb*0.5f;
    v.mda = p_v1->mda*0.5f + p_v2->mda*0.5f;

    v.sr = p_v1->sr*0.5f + p_v2->sr*0.5f;
    v.sg = p_v1->sg*0.5f + p_v2->sg*0.5f;
    v.sb = p_v1->sb*0.5f + p_v2->sb*0.5f;

    v.msr = p_v1->msr*0.5f + p_v2->msr*0.5f;
    v.msg = p_v1->msg*0.5f + p_v2->msg*0.5f;
    v.msb = p_v1->msb*0.5f + p_v2->msb*0.5f;

    for(i = 0; i < 6; i += 3) {
      n = 0;
      f[0] = p_obj->p_face[i];
      if(p_obj->p_face[i] == p[0] || p_obj->p_face[i] == p[1]) {
        n++;
      }      
      if(p_obj->p_face[i+1] == p[0] || p_obj->p_face[i+1] == p[1]) {
        if(n) { // druhy spolecny
          f[3] = f[2] = p_obj->p_face[i+2];
          f[4] = f[1] = 4;
          f[5] = p_obj->p_face[i+1];
        } else { // prvni spolecny          
          f[1] = p_obj->p_face[i+1];
          f[4] = f[2] = 4;
          f[3] = f[0];
          f[5] = p_obj->p_face[i+2];
        }
      } else {
        assert(n);        
        f[3] = f[1] = p_obj->p_face[i+1];
        f[5] = f[2] = 4;
        f[4] = p_obj->p_face[i+2];
      }
      memcpy(face+(i*2),f,sizeof(f));
    }
    free(p_obj->p_face);
    p_obj->p_face = mmalloc(sizeof(f[0])*12);
    memcpy(p_obj->p_face,face,sizeof(f[0])*12);
    memcpy(nov,p_obj->p_vertex,sizeof(p_obj->p_vertex[0])*4);
    free(p_obj->p_vertex);
    p_obj->p_vertex = mmalloc(sizeof(p_obj->p_vertex[0])*5);
    memcpy(p_obj->p_vertex,nov,sizeof(p_obj->p_vertex[0])*4);
    p_obj->p_vertex[4] = v;
    p_obj->vertexnum = 5;
    p_obj->facenum = 12;
  }
  */
}

// Prida extra-vertex
void kont_extra_light(EDIT_KONTEJNER *p_kont)
{
  int i;
  for(i = 0; i < p_kont->objektu; i++) {
    if(p_kont->objektu) 
      rozsir_objekt_light(p_kont->p_obj[i]);
  }
  updatuj_kontejner_statistika(p_kont,FALSE);
}

void mesh_calc_varray(GAME_MESH *p_mesh)
{
/*
  VERTEX_ARRAYS *p_varray = &p_mesh->varray;
  int vertexu = p_mesh->vertexnum;
  int m2flag = p_mesh->p_data->m2flag;
  int last,norm = glstav_pn_triangles || p_mesh->p_data->m2flag&(MAT2_CALC_MAP1|MAT2_CALC_MAP2|MAT2_CALC_MAP3|MAT2_CALC_MAP4);
 
  if(extlist_vertex_array) {
    p_varray->ati_start_pos = 0;
    last = sizeof(p_mesh->p_vertex_pos[0])*vertexu;

    if(norm) {
      p_varray->ati_start_norm = last;
      last = last+sizeof(p_mesh->p_vertex_norm[0])*vertexu;
    }
    
    if(m2flag&MAT2_DIFFUSE) {
      p_varray->ati_start_diff = last;
      last = last+sizeof(p_mesh->p_vertex_diff[0])*vertexu;
    }

    p_varray->ati_start_spec = last;
    last = last+sizeof(p_mesh->p_vertex_spec[0])*vertexu;

    if(m2flag&MAT2_MAP1) {
      p_varray->ati_start_text[0] = last;
      last = last+sizeof(p_mesh->p_vertex_uv2_material[0])*vertexu;
    }  
    if(m2flag&MAT2_MAP2) {
      p_varray->ati_start_text[1] = last;
      last = last+sizeof(p_mesh->p_vertex_uv3_material[0])*vertexu;
    }  
    if(m2flag&MAT2_MAP3) {
      p_varray->ati_start_text[2] = last;
      last = last+sizeof(p_mesh->p_vertex_uv4_material[0])*vertexu;
    }  
    if(m2flag&MAT2_MAP4) {
      p_varray->ati_start_text[3] = last;
      last = last+sizeof(p_mesh->p_vertex_uv4_material[0])*vertexu;
    }
    //p_varray->ati_velikost = last+sizeof(p_mesh->p_face[0])*p_mesh->facevel;
    p_varray->ati_velikost = last;
  }
  */
}

void mesh_preved_jointy_rec(JOINT *p_joint, int prvni_vertex)
{
  int i;
  for(i = 0; i < p_joint->vertexnum; i++) {
    p_joint->p_vertexlist[i] += prvni_vertex;
  }
  if(p_joint->p_child)
    mesh_preved_jointy_rec(p_joint->p_child,prvni_vertex);
  if(p_joint->p_next)
    mesh_preved_jointy_rec(p_joint->p_next,prvni_vertex);
  if(p_joint->p_keys)
    zrus_kosti_key(&p_joint->p_keys);
}

// prevede animace
JOINT_ANIMACE * mesh_preved_jointy(EDIT_OBJEKT *p_obj, int prvni_vertex, int objnum, GAME_MESH *p_mesh)
{
  JOINT_ANIMACE *p_ret = p_obj->p_joit_animace;
  JOINT_ANIMACE *p_tmp = p_ret;
/*
  if(!p_ret) {
    p_mesh->p_data->k2flag &= ~KONT2_JOINT_ANIM;
    return(NULL);
  }

  key_kosti_interpoluj(p_ret);

  while(p_tmp) {
    if(p_tmp->p_child) {
      mesh_preved_jointy_rec(p_tmp->p_child,prvni_vertex);
      p_tmp->p_norm_in = p_mesh->p_vertex_norm_kosti;
      p_tmp->p_norm_out = p_mesh->p_vertex_norm;
      p_tmp->p_pos_in = p_mesh->p_vertex_pos_kosti;
      p_tmp->p_pos_out = p_mesh->p_vertex_pos;      
    }
    p_tmp = p_tmp->p_next;
  }
  p_ret->p_k2flag = &p_mesh->p_data->k2flag;
  p_ret->p_kkflag = &p_mesh->p_data->kflag;
  p_ret->p_koflag =  p_mesh->p_kflag+objnum;
  p_obj->p_joit_animace = NULL;
  */
  return(p_ret);
}

/*
#define O_INDEX_POCET   0
#define O_INDEX_TYP     1
#define O_INDEX_DATA    2
*/

/* Slepi dohromady pole p_opt

  p_mesh->p_face = opt_slep(p_opt, p_optnum, p_mesh->facestart, p_mesh->objektu, &p_mesh->facevel);
  null_free((void **)&p_opt);
  null_free((void **)&p_optnum);

*/

/*
  p_opt    - pole poli p_opt
  p_optnum - delky v jednotlivych pol i
  p_start  - pole zacatku poli facu jednotlivych objektu
  objnum   - pocet objektu
  p_vysnum - vysledna delka slepeneho pole


  - vraci slepene pole a jeho delku
*/
int * opt_slep(int **p_opt, int *p_optnum, int *p_start, int objnum, int *p_vysnum)
{
  int  start,i,vel,num;
  int *p_tmp;

  vel = 0;
  for(i = 0; i < objnum; i++) {
    vel += p_optnum[i];
  }
 *p_vysnum = vel;
/*
  p_tmp = mmalloc(sizeof(p_tmp[0])*vel);
  start = 0;
  for(i = 0; i < objnum; i++) {
    num = p_optnum[i];
    memcpy(p_tmp+start,p_opt[i],sizeof(p_tmp[0])*num);
    free(p_opt[i]);
    p_start[i] = start;
    start += num;
  }
*/
  return(p_tmp);
}

/*
#define O_INDEX_POCET   0
#define O_INDEX_TYP     1
#define O_INDEX_DATA    2
*/

/* Pripocte k facum add
  *p_opt - pointer na list optu
   optnum - delka pole *p_opt (vcetne vsech indexu)
*/
int opt_add(int *p_opt, int optnum, int add)
{
  int i,j,num;  
  int sekci;
  
  if(p_opt) {
    j = 0;    
    sekci = 0;
  
    while(j < optnum) {
      num = p_opt[O_INDEX_POCET];
      
      for(i = O_INDEX_DATA; i < num+O_INDEX_DATA; i++) {
        p_opt[i] += add;
      }
      
      j += num+O_INDEX_DATA;
      p_opt += num+O_INDEX_DATA;
      sekci++;
    }
    
    return(sekci);
  } else {
    return(0);
  }
}

void opt_dopln(EDIT_OBJEKT *p_obj)
{
  int  optnum;
  int *p_opt;
  int  i;

  if(!p_obj->p_opt) {
    kprintf(TRUE,"Stary objekt - calc strip %s...",p_obj->jmeno);
    
    optnum = p_obj->facenum+2;
/*    
    p_opt = mmalloc(sizeof(p_opt[0])*optnum);
    
    p_opt[O_INDEX_POCET] = p_obj->facenum;
    p_opt[O_INDEX_TYP] = p_obj->face_typ;

    for(i = 0; i < p_obj->facenum; i++)
      p_opt[i+O_INDEX_DATA] = p_obj->p_face[i];

    p_obj->p_opt = p_opt;
    p_obj->optnum = optnum;
*/
  }
}

GAME_MESH * edit_to_mesh(GAME_MESH_DATA *p_mesh_data, EDIT_KONTEJNER *p_kont, EDIT_MATERIAL **p_mat, int max_mat, int shadow_volume)
{
/*
  GAME_MESH     *p_mesh = vyrob_mesh(p_kont->objektu,p_mesh_data);
  VERTEX_ARRAYS *p_varray = &p_mesh->varray;
  GLMATRIX       tmp;
  BOD           *p_piv;
  int          **p_opt;
  int           *p_optnum;
  int            i,j,p,k,vertexnum,vel,bodu = p_kont->bodu;
  int            shut = 0;
  int            pivot = 0;

  kprintf(TRUE,"Edit-to-mesh %s (f:%d v:%d)",p_kont->jmeno,p_kont->facu,p_kont->bodu);
  strcpy(p_mesh->jmeno,p_kont->jmeno);
  p_mesh->p_data->kflag  |= (p_kont->kflag&KONT_STATIC ? p_kont->kflag : p_kont->kflag|KONT_POHYB);
  p_mesh->p_data->m1flag |= p_kont->m1flag;
  p_mesh->p_data->m2flag |= p_kont->m2flag;
  p_mesh->objektu         = p_kont->objektu;
  p_mesh->vertexnum       = p_kont->bodu;
  p_mesh->world           = p_kont->world;    // world matrix meshe
  p_mesh->m               = p_kont->mpmatrix; // mp (mesh-pozition) matrix meshe/kontejneru
  p_mesh->low_id          = p_kont->low_id;
  p_mesh->p_data->mesh_ID = p_kont->kontejner_ID;  
  p_mesh->obb_local       = p_kont->obb;
  p_mesh->obb_world       = p_kont->obb;
  
  p_mesh->p_kflag = mmalloc(sizeof(p_mesh->p_kflag[0])*p_kont->objektu);
  
  p_opt    = mmalloc(sizeof(p_opt[0])*p_kont->objektu);
  p_optnum = mmalloc(sizeof(p_optnum[0])*p_kont->objektu);  

  p_mesh->p_mlha = p_kont->p_mlha;
  if(p_mesh->p_mlha) {
    p_kont->p_mlha->p_kont = p_mesh;
    p_kont->p_mlha = NULL;
  }

    // Obalkovy system
  p_mesh->p_obb_local = mmalloc(sizeof(p_mesh->p_obb_local[0])*p_mesh->objektu);
  p_mesh->p_obb_world = mmalloc(sizeof(p_mesh->p_obb_world[0])*p_mesh->objektu);
  
  if(p_kont->kflag&KONT_KEYFRAME) {
    p_mesh->p_key = mmalloc(sizeof(p_mesh->p_key[0])*p_mesh->objektu);
    for(i = 0; i < p_mesh->objektu; i++) {
      init_matrix(p_mesh->p_key+i);
    }
  }

  // joint-animace
  if(p_kont->k2flag&KONT2_JOINT_ANIM) {
    p_mesh->p_joint_anim = mmalloc(sizeof(p_mesh->p_joint_anim[0])*p_mesh->objektu);
  }

  if(p_kont->kflag&KONT_NORMAL_ANIM) {
    vyrob_pole(p_mesh->p_normal_scale,p_mesh->objektu);
    vyrob_pole(p_mesh->p_normal_scale_last,p_mesh->objektu);
  }    

  
  vyrob_pole(p_mesh->p_Objekt_ID,p_mesh->objektu);

  vel = sizeof(p_mesh->p_vertex_pos[0])+
        sizeof(p_mesh->p_vertex_norm[0]);
  if(p_kont->m2flag&MAT2_DIFFUSE) {
    vel += sizeof(p_mesh->p_vertex_diff[0])+
           sizeof(p_mesh->p_vertex_diff_material[0]);
  }
  if(p_kont->m2flag&MAT2_SPECULAR) {
    vel += sizeof(p_mesh->p_vertex_spec[0])+
           sizeof(p_mesh->p_vertex_spec_material[0]);
  } else if(p_kont->m2flag&MAT2_ENV_SPEC) {
    vel += sizeof(p_mesh->p_vertex_spec[0]);
  }  
  if(p_kont->m2flag&MAT2_MAP1) {
    vel += sizeof(p_mesh->p_vertex_uv1_material[0]);
  }
  if(p_kont->m2flag&MAT2_MAP2) {
    vel += sizeof(p_mesh->p_vertex_uv2_material[0]);
  }
  if(p_kont->m2flag&MAT2_MAP3) {
    vel += sizeof(p_mesh->p_vertex_uv3_material[0]);
  }
  if(p_kont->m2flag&MAT2_MAP4) {
    vel += sizeof(p_mesh->p_vertex_uv4_material[0]);
  }

  
  vyrob_pole_abs(p_mesh->p_vertex_pos,vel*bodu);
  
  p_mesh->p_vertex_norm = p_mesh->p_vertex_pos+bodu;

  
  vel = (int)(p_mesh->p_vertex_norm+bodu);
  if(p_kont->m2flag&MAT2_DIFFUSE) {
    p_mesh->p_vertex_diff = (BARVA_RGBA *)vel;
    p_mesh->p_vertex_diff_material = p_mesh->p_vertex_diff+bodu;
    vel = (int)(p_mesh->p_vertex_diff_material+bodu);
  }
  if(p_kont->m2flag&MAT2_SPECULAR) {
    p_mesh->p_vertex_spec = (BARVA_RGB *)vel;
    p_mesh->p_vertex_spec_material = p_mesh->p_vertex_spec+bodu;
    vel = (int)(p_mesh->p_vertex_spec_material+bodu);
  } else if(p_kont->m2flag&MAT2_ENV_SPEC) {    
    p_mesh->p_vertex_spec = (BARVA_RGB *)vel;
    vel = (int)(p_mesh->p_vertex_spec+bodu);
  }  
  if(p_kont->m2flag&MAT2_MAP1) {
    p_mesh->p_vertex_uv1_material = (BODUV *)vel;
    vel = (int)(p_mesh->p_vertex_uv1_material+bodu);
  }
  if(p_kont->m2flag&MAT2_MAP2) {
    p_mesh->p_vertex_uv2_material = (BODUV *)vel;
    vel = (int)(p_mesh->p_vertex_uv2_material+bodu);
  }
  if(p_kont->m2flag&MAT2_MAP3) {
    p_mesh->p_vertex_uv3_material = (BODUV *)vel;
    vel = (int)(p_mesh->p_vertex_uv3_material+bodu);
  }
  if(p_kont->m2flag&MAT2_MAP4) {
    p_mesh->p_vertex_uv4_material = (BODUV *)vel;
    vel = (int)(p_mesh->p_vertex_uv4_material+bodu);
  }
  if(p_kont->k2flag&KONT2_JOINT_ANIM) {
    p_mesh->p_vertex_pos_kosti = mmalloc(sizeof(p_mesh->p_vertex_pos_kosti[0])*bodu);
    p_mesh->p_vertex_norm_kosti = mmalloc(sizeof(p_mesh->p_vertex_norm_kosti[0])*bodu);
  }

  for(i = 0, p = 0, j = 0; i < MAX_KONT_OBJEKTU; i++) {
    if(!p_kont->p_obj[i]) 
      continue;

    opt_dopln(p_kont->p_obj[i]);

    if(p_kont->kflag&KONT_KEYFRAME) {
      p_mesh->p_key[j] = p_kont->p_obj[i]->m;
    }
    
    p_piv = &p_kont->p_obj[i]->pivot;
    if(!pivot && (p_piv->x != 0.0f || p_piv->y != 0.0f || p_piv->z != 0.0f)) {
      p_mesh->kofola_pivot = *p_piv;
      pivot = TRUE;
    }
    
    p_mesh->p_obb_world[j] = p_mesh->p_obb_local[j] = p_kont->p_obj[i]->obb;
    p_mesh->p_Objekt_ID[j] = p_kont->p_obj[i]->Objekt_ID;

    vertexnum = p_kont->p_obj[i]->vertexnum;

    for(k = 0; k < vertexnum; k++) {
      p_mesh->p_vertex_pos[k+p].x = p_kont->p_obj[i]->p_vertex[k].x;
      p_mesh->p_vertex_pos[k+p].y = p_kont->p_obj[i]->p_vertex[k].y;
      p_mesh->p_vertex_pos[k+p].z = p_kont->p_obj[i]->p_vertex[k].z;
      p_mesh->p_vertex_norm[k+p].x = p_kont->p_obj[i]->p_vertex[k].nx;
      p_mesh->p_vertex_norm[k+p].y = p_kont->p_obj[i]->p_vertex[k].ny;
      p_mesh->p_vertex_norm[k+p].z = p_kont->p_obj[i]->p_vertex[k].nz;
      if(p_mesh->p_vertex_pos_kosti)
        p_mesh->p_vertex_pos_kosti[k+p] = p_mesh->p_vertex_pos[k+p];
      if(p_mesh->p_vertex_norm_kosti)
        p_mesh->p_vertex_norm_kosti[k+p] = p_mesh->p_vertex_norm[k+p];
    }

    if(p_kont->m2flag&MAT2_DIFFUSE) {
      for(k = 0; k < vertexnum; k++) {
        p_mesh->p_vertex_diff[k+p].r = p_kont->p_obj[i]->p_vertex[k].dr;
        p_mesh->p_vertex_diff[k+p].g = p_kont->p_obj[i]->p_vertex[k].dg;
        p_mesh->p_vertex_diff[k+p].b = p_kont->p_obj[i]->p_vertex[k].db;
        p_mesh->p_vertex_diff[k+p].a = p_kont->p_obj[i]->p_vertex[k].da;
        p_mesh->p_vertex_diff_material[k+p].r = p_kont->p_obj[i]->p_vertex[k].mdr;
        p_mesh->p_vertex_diff_material[k+p].g = p_kont->p_obj[i]->p_vertex[k].mdg;
        p_mesh->p_vertex_diff_material[k+p].b = p_kont->p_obj[i]->p_vertex[k].mdb;
        p_mesh->p_vertex_diff_material[k+p].a = p_kont->p_obj[i]->p_vertex[k].mda;
      }
    }
    if(p_kont->m2flag&MAT2_SPECULAR) {
      for(k = 0; k < vertexnum; k++) {
        p_mesh->p_vertex_spec[k+p].r = p_kont->p_obj[i]->p_vertex[k].sr;
        p_mesh->p_vertex_spec[k+p].g = p_kont->p_obj[i]->p_vertex[k].sg;
        p_mesh->p_vertex_spec[k+p].b = p_kont->p_obj[i]->p_vertex[k].sb;
        p_mesh->p_vertex_spec_material[k+p].r = p_kont->p_obj[i]->p_vertex[k].msr;
        p_mesh->p_vertex_spec_material[k+p].g = p_kont->p_obj[i]->p_vertex[k].msg;
        p_mesh->p_vertex_spec_material[k+p].b = p_kont->p_obj[i]->p_vertex[k].msb;
      }
    }    
    if(p_kont->m2flag&MAT2_MAP1) {
      for(k = 0; k < vertexnum; k++) {
        p_mesh->p_vertex_uv1_material[k+p].u = p_kont->p_obj[i]->p_vertex[k].tu1;
        p_mesh->p_vertex_uv1_material[k+p].v = p_kont->p_obj[i]->p_vertex[k].tv1;
      }
    }
    if(p_kont->m2flag&MAT2_MAP2) {
      for(k = 0; k < vertexnum; k++) {
        p_mesh->p_vertex_uv2_material[k+p].u = p_kont->p_obj[i]->p_vertex[k].tu2;
        p_mesh->p_vertex_uv2_material[k+p].v = p_kont->p_obj[i]->p_vertex[k].tv2;
      }
    }
    if(p_kont->m2flag&MAT2_MAP3) {
      for(k = 0; k < vertexnum; k++) {
        p_mesh->p_vertex_uv3_material[k+p].u = p_kont->p_obj[i]->p_vertex[k].tu3;
        p_mesh->p_vertex_uv3_material[k+p].v = p_kont->p_obj[i]->p_vertex[k].tv3;
      }
    }
    if(p_kont->m2flag&MAT2_MAP4) {
      for(k = 0; k < vertexnum; k++) {
        p_mesh->p_vertex_uv4_material[k+p].u = p_kont->p_obj[i]->p_vertex[k].tu4;
        p_mesh->p_vertex_uv4_material[k+p].v = p_kont->p_obj[i]->p_vertex[k].tv4;
      }
    }

    p_opt[j] = p_kont->p_obj[i]->p_opt;
    p_optnum[j] = p_kont->p_obj[i]->optnum;
    p_kont->p_obj[i]->p_opt = NULL;
    p_kont->p_obj[i]->optnum = 0;
    
    p_mesh->facenum[j] = opt_add(p_opt[j],p_optnum[j],p);

    p_mesh->objektnum[j] = p_kont->p_obj[i]->vertexnum;
    p_mesh->objektstart[j] = p;
    p_mesh->p_mat[j] = p_kont->p_obj[i]->material;
    
    if(p_kont->k2flag&KONT2_JOINT_ANIM) {
      p_mesh->p_joint_anim[j] = mesh_preved_jointy(p_kont->p_obj[i],p,j,p_mesh);
    }

    p += p_kont->p_obj[i]->vertexnum;
    j++;
  }
  
  p_mesh->objektu = j; // pocet skutecne prevedenych objektu
  assert(j == p_kont->objektu);

  p_mesh->p_face = opt_slep(p_opt, p_optnum, p_mesh->facestart, p_mesh->objektu, &p_mesh->facevel);
  null_free((void **)&p_opt);
  null_free((void **)&p_optnum);

  //assert(!p_kont->p_slight);
  lo_premapuj_svetla_kont_mesh(p_kont, p_mesh);
  
  // kopie lokalnich animaci
  if(p_kont->kflag&KONT_KEYFRAME) {
    for(i = 0, p = K_CHYBA; i < KONT_MAX_ANIM; i++) {
      if(p_kont->sim[i].keynum) {
        p = i;
      }
    }

    if(p == K_CHYBA) {
      p_kont->kflag&=~KONT_KEYFRAME;
    } else {
      p++;
      
      // p je posledni pouzity mesh      
      p_mesh->p_sim = mmalloc(sizeof(p_mesh->p_sim[0])*p);
      p_mesh->simnum = p;
      
      // kopirovani animacnich stromu
      for(i = 0; i < p; i++) {
        if(p_kont->sim[i].keynum) {
          key_sim_root_to_sim_indir(p_kont->sim+i,p_mesh->p_sim+i);
          key_sim_dopln_matrix_mesh(p_mesh,p_mesh->p_sim+i);          
        }
      }

//        ____OPRAVA____
      p_mesh->p_sim_aktivni = p_mesh->p_sim;
      p_mesh->sim_aktivni_time = 0;

      mat_mult_dir(&p_mesh->world,&p_mesh->m,&tmp);
      key_mesh_reanimuj(p_mesh,p_mesh->sim_aktivni_time,&tmp);
    }
  }
  
  // Vertex-arrays inicializace
  mesh_calc_varray(p_mesh);
  mesh_pridej_vertex_array(p_mesh);

  if(!(p_mesh->p_data->kflag&KONT_KEYFRAME)) {
    mat_mult_dir(&p_mesh->world,&p_mesh->m,&tmp);
    obb_transformuj(&p_mesh->obb_local,&tmp,&p_mesh->obb_world);
    for(i = 0; i < p_mesh->objektu; i++)
      obb_transformuj(p_mesh->p_obb_local+i,&tmp,p_mesh->p_obb_world+i);
  }
  return(p_mesh);
*/
}

GAME_MESH * vyrob_mesh(int objektu, GAME_MESH_DATA *p_data)
{
  GAME_MESH *p_mesh;
  int i;
  
  if((p_mesh = (GAME_MESH *)mmalloc(sizeof(p_mesh[0]))) == NULL) {
    chyba("pamet");
  }
  memset(p_mesh,0,sizeof(p_mesh[0]));
/* 
  p_mesh->p_data = (p_data) ? p_data : vyrob_mesh_data();
  p_mesh->objektu = objektu;
  p_mesh->p_data->top_edlight = K_CHYBA;
  
  p_mesh->objektnum = mmalloc(sizeof(p_mesh->objektnum[0])*objektu);
  p_mesh->objektstart = mmalloc(sizeof(p_mesh->objektstart[0])*objektu);
  p_mesh->facenum = mmalloc(sizeof(p_mesh->facenum[0])*objektu);
  p_mesh->facestart = mmalloc(sizeof(p_mesh->facestart[0])*objektu);
  p_mesh->p_mat = mmalloc(sizeof(p_mesh->p_mat[0])*objektu);
  
  p_mesh->siminfo.odkaz = K_CHYBA;
  for(i = 0; i < LANI_FRONTA; i++)
    p_mesh->siminfo.akt[i] = K_CHYBA;
*/
  return(p_mesh);
}

GAME_MESH_DATA * vyrob_mesh_data(void)
{
  GAME_MESH_DATA *p_data;
// TODO
//  p_data = mmalloc(sizeof(p_data[0]));
  return(p_data);
}

void mesh_pridej_vodavertexy(GAME_MESH *p_mesh)
{
/*
  vyrob_pole(p_mesh->p_vertex_diff_voda,p_mesh->vertexnum);
  vyrob_pole(p_mesh->p_vertex_spec_voda,p_mesh->vertexnum);
*/
}

void zrus_fleky(FLEK **p_flek)
{
  if(*p_flek) {
    zrus_fleky(&((*p_flek)->p_next));
    free(*p_flek);
    *p_flek = NULL;
  }
}

void zrus_flare(LENS_FLARE **p_flare)
{
  if(*p_flare) {
    zrus_flare(&((*p_flare)->p_next));
    null_free((void **)&(*p_flare)->p_sloz);
    free(*p_flare);
    *p_flare = NULL;
  }
}

void zrus_mesh_data(GAME_MESH_DATA **p_tmp)
{
  GAME_MESH_DATA *p_data = *p_tmp;
  null_free((void **)&p_data->p_light);
  null_free((void **)&p_data->p_edlight);
  null_free((void **)p_tmp);
}

void zrus_mesh(GAME_MESH **p_mesh_top)
{
  GAME_MESH *p_mesh = *p_mesh_top;
  GAME_MESH *p_tmp;
  int i;
/*  
  zrus_mesh_data(&p_mesh->p_data);

  while(p_mesh) {
  
    if(p_mesh->p_sim) {
      for(i = 0; i < p_mesh->simnum; i++) {
        key_sim_zrus(p_mesh->p_sim+i);
      }
      free(p_mesh->p_sim);
    }
    
    null_free(&p_mesh->p_vertex_pos);
    null_free(&p_mesh->objektnum);
    null_free(&p_mesh->objektstart);
    null_free(&p_mesh->facenum);
    null_free(&p_mesh->facestart);
    null_free(&p_mesh->p_face);
    null_free((void **)&p_mesh->p_mat);
    null_free(&p_mesh->p_key);
    null_free(&p_mesh->p_obb_local);
    null_free(&p_mesh->p_obb_world);
    null_free(&p_mesh->p_Objekt_ID);
    null_free(&p_mesh->p_kflag);

    if(mesh_vertex_array_zrus)
      mesh_vertex_array_zrus(p_mesh);
    
    p_tmp = p_mesh;
    p_mesh = p_mesh->p_next;
    
    null_free(&p_tmp);
  }
*/  
  *p_mesh_top = NULL;
}

/************************************************************************
 Hledaci rutiny - co hledaji ruzne veci jako treba
 volne textury, plne textury a podobne
 ************************************************************************
*/
// pocet textur - MAX_EDIT_TEXTUR
int lo_najdi_texturu(EDIT_TEXT *p_text, int max, char *p_file, int flag)
{ 
  int i;
/*  
  p_file = strlwr(p_file);
  for(i = 0; i < max; i++) {
    if(flag == p_text[i].flag && !strcmp(p_file,strlwr(p_text[i].jmeno)))
      return(i);
  }
*/
  return(K_CHYBA);
}

int lo_najdi_volnou_texturu(EDIT_TEXT *p_text, int max)
{
  int i;
/*
  for(i = 0; i < max; i++) {
    if(!p_text[i].jmeno[0]) {
      return(i);
    }
  }
*/
  return(K_CHYBA);
}

// Pocet materialu - MAX_EDIT_MATERIALU
int lo_najdi_material(EDIT_MATERIAL **p_mat, int max, char *p_text)
{ 
  int i;
  
  p_text = strlwr(p_text);
  for(i = 0; i < max; i++) {
    if(p_mat[i] && !strcmp(p_text,strlwr(p_mat[i]->jmeno)))
      return(i);
  }
  return(K_CHYBA);
}

EDIT_MATERIAL * lo_najdi_material_text_point(EDIT_MATERIAL **p_mat, int max, EDIT_TEXT *p_text)
{ 
  int i;    
  for(i = 0; i < max; i++) {
    if(p_mat[i] && p_mat[i]->p_text[0] == p_text)
      return(p_mat[i]);
  }
  return(NULL);
}

int lo_najdi_prazdny_material(EDIT_MATERIAL **p_mat, int max)
{ 
  int i;
  
  for(i = 0; i < max; i++) {
    if(!p_mat[i])
      return(i);
  }
  return(K_CHYBA);
}

int lo_pocet_materialu(EDIT_MATERIAL **p_mat, int max)
{ 
  int i,vel = 0;
  for(i = 0; i < max; i++) {
    if(p_mat[i])
      vel = i+1;
  }
  return(vel);
}

int lo_najdi_kameru(KAMERA *p_kam, int max, char *p_jmeno)
{
 int i;
 strlwr(p_jmeno);
 for(i = 0; i < max; i++) {
   if(p_kam[i].cislo != K_CHYBA && !strcmp(p_kam[i].jmeno,p_jmeno))
     return(i);
 }
 return(K_CHYBA);
}

int lo_najdi_volnou_kameru(KAMERA *p_kam, int max)
{
 int i;

 for(i = 0; i < max; i++) {
   if(p_kam[i].cislo == K_CHYBA) {
     return(i);
   }
 }
 return(K_CHYBA);
}


int lo_najdi_volny_kontejner(EDIT_KONTEJNER **p_kont, int max)
{
 int i;

 for(i = 0; i < max; i++) {
   if(!p_kont[i]) {
     return(i);
   }
 }
 return(K_CHYBA);
}

int lo_najdi_volny_kontejner_up(EDIT_KONTEJNER **p_kont, int max, int up)
{
 int i;

 for(i = up; i < max; i++) {
   if(!p_kont[i]) {
     return(i);
   }
 }
 return(K_CHYBA);
}

int lo_najdi_volny_kontejner_zpet(EDIT_KONTEJNER **p_kont, int max)
{
 int i;

 for(i = max-1; i > -1; i--) {
   if(!p_kont[i]) {
     return(i);
   }
 }
 return(K_CHYBA);
}

int lo_najdi_kontejner(EDIT_KONTEJNER **p_kont, int max, char *p_jmeno)
{
 int i;

 strlwr(p_jmeno);
 for(i = 0; i < max; i++) {
   if(!p_kont[i])
     continue;
   if(!strcmp(p_kont[i]->jmeno,p_jmeno))
     return(i);
 }
 return(K_CHYBA);
}

int lo_najdi_volny_objekt_kont(EDIT_KONTEJNER *p_kont)
{
 int i;

 if(!p_kont)
   return(K_CHYBA);
 for(i = 0; i < p_kont->max_objektu; i++) {
   if(!p_kont->p_obj[i]) {
     return(i);
   }
 }
 return(K_CHYBA);
}

int lo_najdi_objekt_kont(EDIT_KONTEJNER *p_kont, char *p_jmeno)
{
 int i;

 if(!p_kont)
   return(K_CHYBA);
 strlwr(p_jmeno);
 for(i = 0; i < p_kont->max_objektu; i++) {
   if(p_kont->p_obj[i]) {
     strlwr(p_kont->p_obj[i]->jmeno);
     if(!strcmp(p_kont->p_obj[i]->jmeno,p_jmeno))
     return(i);
   }
 }
 return(K_CHYBA);
}

int lo_najdi_objekt_kont_ID(EDIT_KONTEJNER *p_kont, int ID)
{
 int i;

 if(!p_kont)
   return(K_CHYBA);
 for(i = 0; i < p_kont->max_objektu; i++) {
   if(p_kont->p_obj[i] && p_kont->p_obj[i]->Objekt_ID == ID)
     return(i);
 }
 return(K_CHYBA);
}

int lo_najdi_objekt_kont_plny(EDIT_KONTEJNER *p_kont)
{
 int i;

 if(!p_kont)
   return(K_CHYBA);
 for(i = 0; i < p_kont->max_objektu; i++) {
   if(p_kont->p_obj[i])
     return(i);
 }
 return(K_CHYBA);
}

EDIT_OBJEKT * lo_najdi_objekt_kont_poiter(EDIT_KONTEJNER *p_kont, char *p_jmeno)
{
 int i;

 if(!p_kont)
   return(NULL);
 for(i = 0; i < p_kont->max_objektu; i++) {
   if(p_kont->p_obj[i] && !strcmp(p_kont->p_obj[i]->jmeno,p_jmeno))
     return(p_kont->p_obj[i]);
 }
 return(NULL);
}

EDIT_OBJEKT * lo_najdi_objekt_kont_poiter_ID(EDIT_KONTEJNER *p_kont, int ID)
{
 int i;

 if(!p_kont)
   return(NULL);
 for(i = 0; i < p_kont->max_objektu; i++) {
   if(p_kont->p_obj[i] && p_kont->p_obj[i]->Objekt_ID == ID)
     return(p_kont->p_obj[i]);
 }
 return(NULL);
}

int lo_najdi_volny_mesh(GAME_MESH **p_mesh, int max)
{
  int i = -1;
  while(p_mesh[++i] && i < max);
  return((i == max) ? K_CHYBA : i);
}

int lo_najdi_volny_flare(LENS_FLARE *p_flare, int max)
{
  int i = -1;
  while(p_flare[++i].akt && i < max);
  return((i == max) ? K_CHYBA : i);
}

// Prevede pole flaru na oboustrany linearni seznam
LENS_FLARE * lo_flare2linear(LENS_FLARE *p_flare_list, int max)
{ 
  LENS_FLARE *p_flare,*p_flare_first;
  LENS_FLARE *p_prev = NULL;
  int i;
    
  for(i = 0; i < max; i++) {
    
    if(!p_flare_list[i].akt)
      continue;
  /*
    p_flare = mmalloc(sizeof(p_flare[0]));
    *p_flare = p_flare_list[i];
    p_flare->p_next = NULL;
    
    if(p_prev) {
      p_flare->p_prev = p_prev;
      p_prev->p_next = p_flare;
    } else {
      p_flare->p_prev = NULL;
    }
    
    if(!i) {
      p_flare_first = p_flare;
    }
*/
    p_prev = p_flare;
  } 
  return(p_flare_first);
}

int lo_posledni_svetlo(STATIC_LIGHT *p_light, int max)
{
  int i = 0, mmx = 0;
  while(i < max) {
    if(p_light[i].akt)
      mmx = i+1;
    i++;
  }
  return(mmx);
}

int lo_reload_textur_formaty(APAK_HANDLE *pHandle, EDIT_TEXT *p_text, int max, int save)
{
  char file[MAX_JMENO];
  int i;
/*
  for(i = 0; i < max; i++) {
    if(p_text[i].jmeno[0] && !p_text[i].load) {
      kprintf(TRUE,"Texture %s...",p_text[i].jmeno);
      if(strstr(strlwr(p_text[i].jmeno),".bmp") || strstr(p_text[i].jmeno,".btx")) {
		  if(!txt_nahraj_texturu_z_func(pHandle,p_text[i].jmeno,p_text+i,save,TRUE,NULL,txt_lib_to_aux)) {     
             zamen_koncovku(strcpy(file,p_text[i].jmeno),".jpg");
             if(txt_nahraj_texturu_z_func(pHandle,file,p_text+i,save,TRUE,NULL,txt_lib_to_aux)) {
               strcpy(p_text[i].jmeno,file);
			 }
		  }
      }
      else if(strstr(p_text[i].jmeno,".dds")) {
        txt_nahraj_texturu_z_dds(pHandle,p_text[i].jmeno,p_text+i,save);
      }
      else if(strstr(p_text[i].jmeno,".tga")) {
        txt_nahraj_texturu_z_tga(pHandle,p_text[i].jmeno, p_text+i);
      }
      else {              
        char file[MAX_JMENO];
        zamen_koncovku(strcpy(file,p_text[i].jmeno),".bmp");
        if(txt_nahraj_texturu_z_func(pHandle,file,p_text+i,save,TRUE,NULL,txt_lib_to_aux)) {
          strcpy(p_text[i].jmeno,file);
        }
      }
    }
  }
  */
  return(0);
}


int lo_reload_textur_dir(EDIT_TEXT *p_text, int max, char *p_dir, int save)
{
  if(chdir(p_dir)) {
    kprintfl(TRUE,"Chyba dir %s...",p_dir);
    return(FALSE);
  }
  lo_reload_textur_formaty(NULL, p_text, max, save);
  return(0);
}

int lo_reload_textur_file(EDIT_TEXT *p_text, int max, char *p_file, int save)
{
  APAK_HANDLE *pHandle;
  int err;
      
	pHandle = apakopen(p_file,".", &err);
  if(!pHandle) {
    kprintf(TRUE,"Chyba otevreni pak file %s...",p_file);
    return(FALSE);
  } else {
    pHandle->pActualNode = pHandle->pRootNode->pNextNode;
  }
  
  kprintf(TRUE,"Texture pak file %s...",p_file);

  lo_reload_textur_formaty(pHandle,p_text,max, save);
  
  apakclose(pHandle);

  return(0);
}

int lo_reload_textur(TEXT_DIR *p_dir, EDIT_TEXT *p_text, int num, int save)
{  
  int i;    
/*
  for(i = 0; i < TEXT_DIRS; i++) {
    if(p_dir->texture_dir[i][0]) {
      txt_trida(p_dir->texture_dir_class[i]);
      lo_reload_textur_dir(p_text,num,p_dir->texture_dir[i],save);
    }
  }

  for(i = 0; i < TEXT_DIRS; i++) {
    if(p_dir->texture_file[i][0]) {
      txt_trida(p_dir->texture_file_class[i]);
      lo_reload_textur_file(p_text,num,p_dir->texture_file[i],save);
    }
  }
*/
  lo_reload_textur_chyby(p_text, num);    

  return(TRUE);
}

int lo_reload_textur_chyby(EDIT_TEXT *p_text, int num)
{  
  int i,n;
/*
  for(i = 0, n = 0; i < num; i++) {
    if(p_text[i].jmeno[0] && !p_text[i].load) {
      kprintf(TRUE,"Chyba loadu textury %s...",p_text[i].jmeno);
      n++;
    }
  }  
*/
  kprintf(TRUE,"Chybne nahrano %d textur",n);
  return(TRUE);
}

int lo_reload_textur_vypis(EDIT_TEXT *p_text, int num)
{  
  int i;
/*
  for(i = 0; i < num; i++) {
    if(p_text[i].jmeno[0]) {
      kprintf(TRUE,"%d textura %s %d...",i,p_text[i].jmeno,p_text[i].load);      
    }
  }
*/
  return(TRUE);
}

int lo_smaz_textury(EDIT_TEXT *p_text, int max)
{
  int i;
/* 
  for(i = 0; i < max; i++) {
    if(p_text[i].load) {
      txt_zrus_texturu(p_text+i);
    }
  }
*/
  return(0);
}

int lo_smaz_textury_bmp(EDIT_TEXT *p_text, int max)
{
  int i; 
/*
  for(i = 0; i < max; i++) {
    if(p_text[i].load && p_text[i].p_bmp) {
      bmp_zrus(&p_text[i].p_bmp);
    }
  }
*/
  return(0);
}

// Vraci velikost pixelu v texturach
int lo_velikost_textur(EDIT_TEXT *p_text, int max)
{
  int i,xres,yres,vel = 0;
  int r,g,b,a,l,in;
/* 
  for(i = 0; i < max; i++) {
    if(p_text[i].load) {
      glBindTexture(p_text[i].typ,p_text[i].text);
      glGetTexLevelParameteriv(p_text[i].typ,0,GL_TEXTURE_WIDTH,&xres);
      glGetTexLevelParameteriv(p_text[i].typ,0,GL_TEXTURE_HEIGHT,&yres);
      glGetTexLevelParameteriv(p_text[i].typ,0,GL_TEXTURE_RED_SIZE,&r);
      glGetTexLevelParameteriv(p_text[i].typ,0,GL_TEXTURE_GREEN_SIZE,&g);
      glGetTexLevelParameteriv(p_text[i].typ,0,GL_TEXTURE_BLUE_SIZE,&b);
      glGetTexLevelParameteriv(p_text[i].typ,0,GL_TEXTURE_ALPHA_SIZE,&a);
      glGetTexLevelParameteriv(p_text[i].typ,0,GL_TEXTURE_LUMINANCE_SIZE,&l);
      glGetTexLevelParameteriv(p_text[i].typ,0,GL_TEXTURE_INTENSITY_SIZE,&in);
      vel += xres*yres*(r+b+g+a+l+in);
    }
  }
*/
  return(vel);
}

/* reloadne stage z textur do materialu
*/
void lo_reload_stage(EDIT_MATERIAL **p_mat, int num)
{
  int i,t,as;
  /*
  for(i = 0; i < num; i++) {
    if(p_mat[i]) {
      for(t = 0; t < MAT_TEXTUR; t++) {
        if(p_mat[i]->p_text[t]) {
          
          as = p_mat[i]->p_text[t]->alfa_stage;
          if(as != K_CHYBA) {
            p_mat[i]->alfa_state = as; //- alfa stage pouze od 1 textury
            if(as == STAGE_ALFA_MASKA) {
              p_mat[i]->flag &=~(MAT_PRUHLEDNY|MAT_NO_ZMASK|MAT_NO_ZTEST|MAT_NO_CULL);
            }
          }

          as = p_mat[i]->p_text[t]->no_cull;
          if(as) {
            p_mat[i]->flag |= (MAT_NO_CULL);
          }
        }
      }
    }
  }
  */
}

int lo_pocet_textur(EDIT_TEXT *p_text, int max)
{/*
  int i,vel = 0;
  for(i = 0; i < max; i++) {
    if(p_text[i].load) {
      vel = i;
    }
  }
  return(vel);
*/
}

int lo_velikost_meshu(GAME_MESH **p_mesh, int num, int *p_facu, int *p_vertexu)
{
/*
  int i,vel = 0;
  for(i = 0; i < num; i++) {
    if(p_mesh[i]) {
      vel += p_mesh[i]->varray.ati_velikost;
      vel += p_mesh[i]->facevel*sizeof(p_mesh[0]->p_face[0]);
      *p_facu += p_mesh[i]->facevel;
      *p_vertexu += p_mesh[i]->vertexnum;
    }
  }
  return(vel);
*/
}

int lo_velikost_poly(EDIT_MESH_POLY *p_poly, int num, int *p_facu, int *p_vertexu)
{
  int i,vel = 0;
  for(i = 0; i < num; i++) {
     vel += p_poly[i].varray.ati_velikost;
     *p_facu += p_poly[i].facenum;
     *p_vertexu += p_poly[i].facenum;
  }
  return(vel);
}

/*
  *.mnt - soubory s materialama
*/
void lo_vymaz_materialy(EDIT_MATERIAL **p_mat, int max_mat, EDIT_TEXT *p_text, int max_text)
{
 int i;
/*
 for(i = 0; i < max_mat; i++) {
   if(p_mat[i])
     zrus_material(p_mat+i);
 }
 for(i = 0; i < max_text; i++) {   
   if(p_text[i].load) {
     txt_zrus_texturu(p_text+i);
   }
 }
*/
}


/******************************************************************************
 Materialy
 ******************************************************************************
*/
int lo_najdi_prazdnou_animaci(ANIM_MATERIAL *p_anim, int max)
{
 int i;

 for(i = 0; i < max; i++) {
   if(!p_anim[i].pouzita)
     return(i);
 }
 return(K_CHYBA);
}

/*
  komplet = 0 - vsechno
  --||--- = 1 - frame
  --||--- = 2 - posun

#define  ANIM_LOAD_ALL   0
#define  ANIM_LOAD_FRAME 1
#define  ANIM_LOAD_POSUN 2
*/

#define FFF_FILE   'F' // |F |
#define FFF_TIME   'T'
#define FFF_EVENT  'E'
#define FFF_ALFA   'A'
#define FFF_POSUN  'P'

void lo_vyrob_animaci_list(EDIT_MATERIAL *p_mat, char *p_list, EDIT_TEXT *p_text, int max, char komplet, char *p_dir)
{
 ANIM_MATERIAL *p_amat = &p_mat->anim;
 FILE *f;
 char  line[200]; // max delka radku
 char  *p_pom,*p_next;
 int   linenum;
 int   first_text = 1,
       i,t,akt,pos,alfa,a;

 chdir(p_dir);

 if((f = fopen(p_list,"r")) == NULL) {
   ddw("Nemuzu otevrit soubor %s",p_list);
   assert(0);
   return;
 }
 
 lo_zrus_animaci(p_amat);
 
 // Prekopiruj list
 strcpy(p_amat->jmeno,p_list);
 
 // nahraj informace o frame animaci
 if(komplet == ANIM_LOAD_ALL || komplet == ANIM_LOAD_FRAME) {
   fgets(line,200,f);
   sscanf(line,"FRAME %d %d %d",&akt,&p_amat->frameakt,&alfa);
   if(akt) {
     p_mat->flag = akt ? p_mat->flag|MAT_ANIM_FRAME : p_mat->flag&~MAT_ANIM_FRAME;
//     p_mat->flag = alfa ? p_mat->flag|MAT_ALFA_FAKTOR : p_mat->flag&~MAT_ALFA_FAKTOR;
   }
 }
 else
   fgets(line,200,f);
 
 // nahraj informace o posunu
 if(komplet == ANIM_LOAD_ALL || komplet == ANIM_LOAD_POSUN) {
   fgets(line,200,f);
   sscanf(line,"POSUN %d\n",&pos);
 }
 else
   fgets(line,200,f);

 // pokud je pozadavek na load frame animace
 if(komplet == ANIM_LOAD_ALL || komplet == ANIM_LOAD_FRAME) {
   linenum = 0;
   while(fgets(line,200,f)) 
     linenum++;
//   ddw("%d radek animace",linenum);
   
   strcpy(p_amat->jmeno,p_list);
   p_amat->framenum = linenum;
   p_amat->frameakt = 0;
   p_amat->p_frame = (ANIM_FRAME *)mmalloc(sizeof(ANIM_FRAME)*p_amat->framenum);
      
   fseek(f,0,SEEK_SET);
   fgets(line,200,f);
   fgets(line,200,f);
   
   for(i = 0; i < p_amat->framenum; i++) {
     fgets(line,200,f); // nacti jmeno souboru
     
     if((p_pom = strchr(line,'\n')))
       *p_pom = 0;
     
     p_amat->p_frame[i].file[0] = 0;
     p_amat->p_frame[i].flag &= ~FRAME_TEXTURA;
     
     p_pom = line;
     
     while(p_pom && p_pom[0]) {
       if((p_next = strchr(p_pom,'|'))) {
         *p_next++ = 0;
       }
       /* Format p_pom:
          A ' '
          p_next -> dalsi zaznam
       */
       // Load textury
       a = tolower((int)p_pom[0]);
/*       
       switch(a) {
         case 'f': // je to texture file
           // pridej do texture listu
           t = lo_najdi_texturu(p_text,max,p_pom+2,FALSE);
           if(t == K_CHYBA) {
             t = lo_najdi_volnou_texturu(p_text,max);
             if(t == K_CHYBA)
               chyba("Neni volna textura !");
             else {
               memset(p_text+t,0,sizeof(p_text[t]));
               strcpy(p_text[t].jmeno,p_pom+2);
             }
           }
           p_amat->p_frame[i].p_text = p_text+t;
           strcpy(p_amat->p_frame[i].file,p_pom+2);
           p_amat->p_frame[i].flag |= FRAME_TEXTURA;
           if(first_text) {
             strcpy(p_mat->textfile[0],p_text[t].jmeno);
             p_mat->p_text[0] = p_text+t;
             p_mat->textflag[0] = 0;
             first_text = FALSE;
           }
           break;

         case 't': // time flag
           p_amat->p_frame[i].time = (int)(atof(p_pom+2)*1000);
           break;

         case 'e': // event flag
           p_amat->p_frame[i].flag |= FRAME_EVENT;
           break;        
         
         case 'a':
           sscanf(p_pom+2,"%x",&p_amat->p_frame[i].alfa);
           p_amat->p_frame[i].flag |= FRAME_ALFA_FAKTOR;
           break;

         case 'p':
           sscanf(p_pom+2,"%f %f",&p_amat->p_frame[i].u, &p_amat->p_frame[i].v);
           p_amat->p_frame[i].flag |= FRAME_POSUN;
           p_mat->flag |= MAT_POSUN2D;
           break;

         default:
           ddw("Neznamy flag ! (%s)",p_pom);
           break;
       }
       */
       p_pom = p_next;
     }
   }
 }
 fclose(f);
}

/*
#define FFF_FILE   'F' // |F |
#define FFF_TIME   'T'
#define FFF_EVENT  'E'
#define FFF_ALFA   'A'
#define FFF_POSUN  'P'

#define FRAME_EVENT       0x1  // timto framem se nepokracuje (ceka se na udalost)
#define FRAME_ALFA_FAKTOR 0x2  // tento frame ma nastavit alfa-stage
#define FRAME_TEXTURA     0x4  // tento frame ma nastavit texturu
#define FRAME_POSUN       0x8  // posun texturu oproti originalu
*/
void lo_uloz_animaci_list(EDIT_MATERIAL *p_mat, char *p_list, char *p_dir)
{
 ANIM_MATERIAL *p_amat = &p_mat->anim;
 int   i,akt;
 FILE *f;

 if((f = fopen(p_list,"w")) == NULL)
   chybat(STC,"File %s",p_list);

 akt = p_mat->flag&MAT_ANIM_FRAME; 
 fprintf(f,"FRAME %d %d %d\n",akt,p_amat->frameakt,0);
 fprintf(f,"POSUN %d\n",0);

 for(i = 0; i < p_amat->framenum; i++) {
   /* Soubor
   */
   fputc(FFF_FILE,f); fputc(' ',f);
   fputs(p_amat->p_frame[i].file,f);
   fputc('|',f);
   
   /* Cas
   */
   fprintf(f,"T %.3f|",(float)(p_amat->p_frame[i].time)*0.001f);
   
   /* Event flag
   */
   if(p_amat->p_frame[i].flag&FRAME_EVENT) {
     fputc(FFF_EVENT,f);
     fputc('|',f);
   } 

   /* Alfa flag
   */
   if(p_amat->p_frame[i].flag&FRAME_ALFA_FAKTOR) {
     fprintf(f,"%c %x|",FFF_ALFA,p_amat->p_frame[i].alfa);
   } 

   /* Posunovaci flag
   */
   if(p_amat->p_frame[i].flag&FRAME_POSUN) {     
     fprintf(f,"%c %f %f |",FFF_POSUN,p_amat->p_frame[i].u, p_amat->p_frame[i].v);     
   }
   fputc('\n',f);
 }
 fclose(f);
}

void lo_zrus_animaci(ANIM_MATERIAL *p_amat)
{
 if(p_amat->p_frame)
   free(p_amat->p_frame);
 memset(p_amat,0,sizeof(ANIM_MATERIAL));
}

/*
  Poly-listy
*/
EDIT_MESH_POLY * vyrob_poly(void)
{ 
// TODO
// return(mmalloc(sizeof(EDIT_MESH_POLY)));
}


void zrus_vnitrek_poly(EDIT_MESH_POLY *p_poly)
{
 int i;

 if(poly_vertex_array_zrus)
   poly_vertex_array_zrus(p_poly);

 if(p_poly->p_ind) {
   for(i = 0; i < (p_poly->facenum/3); i++) {
     bmp_zrus(&p_poly->p_ind[i].p_bmp);
     bmp_zrus(&p_poly->p_ind[i].p_zal);
   }
   null_free((void **)&p_poly->p_ind);
 }

 null_free((void **)&p_poly->p_koord);
 null_free((void **)&p_poly->p_light);
 null_free((void **)&p_poly->p_lightnum);
 null_free((void **)&p_poly->p_edlight);
}

void zrus_poly(EDIT_MESH_POLY **p_poly)
{
 zrus_vnitrek_poly(*p_poly);
 free((*p_poly));
 *p_poly = NULL;
}

BOD * lo_edit_to_poly_najdi_vetex(BOD *p_pozice, int bodu, OBJ_VERTEX *p_bod, int *p_index)
{
  int v;

  for(v = 0; v < bodu; v++) {
    if(p_pozice[v].x == p_bod->x &&
       p_pozice[v].y == p_bod->y &&
       p_pozice[v].z == p_bod->z) {
       *p_index = v;
       return(p_pozice);
    }
  }
  return(NULL);
}

/*
  Frci pouze na kontejnery 1:1 (1 kont s 1 objektem - poly ma jeden material)
*/
EDIT_MESH_POLY * edit_to_poly_indir(EDIT_KONTEJNER *p_kont, EDIT_MESH_POLY *p_poly, int kID)
{
  OBJ_VERTEX  *p_bod;
  EDIT_OBJEKT *p_obj;
  TEXT_KOORD  *p_koord;  
  int          bodu,f;
  int          facu_material;
  int          spec = p_kont->m2flag&MAT2_SPECULAR;

  if(!p_poly)
    return(NULL);

  if(p_kont->objektu != 1) {
    ddw("Poly s %d objektama !",p_kont->objektu);
    assert(0);    
  }

  p_obj = p_kont->p_obj[0];
  assert(p_obj);

  memset(p_poly,0,sizeof(*p_poly));

  strcpy(p_poly->jmeno,p_kont->jmeno);

  kont_norm_vect(p_kont);
  updatuj_kontejner_statistika(p_kont,FALSE); // pocty objektu a pod.
  facu_material = p_kont->facu/3;
 
  // koordinaty pro vsecny facy kontejneru
  p_koord = (TEXT_KOORD *)mmalloc(sizeof(TEXT_KOORD)*p_obj->facenum);
    
  // prevede facy  
  bodu = 0;
   
  // vyrobim list sousednosti
  if(!p_obj->p_fsous)
    obj_vyrob_list_sousednosti(p_obj);
  
  p_poly->p_fsous = (dword *)kopiruj_pole(p_obj->p_fsous,sizeof(p_obj->p_fsous[0])*p_obj->facenum);
  
  p_poly->material = p_obj->material;
  
  // prekopiruj plosky
  for(f = 0; f < p_obj->facenum; f++) {
    p_bod = p_obj->p_vertex+p_obj->p_face[f];

    p_koord[f].x = p_bod->x;
    p_koord[f].y = p_bod->y;
    p_koord[f].z = p_bod->z;
    p_koord[f].tu1 = p_bod->tu1;
    p_koord[f].tv1 = p_bod->tv1;
    p_koord[f].tu2 = p_bod->tu2;
    p_koord[f].tv2 = p_bod->tv2;
    p_koord[f].tu3 = p_bod->tu3;
    p_koord[f].tv3 = p_bod->tv3;
    p_koord[f].tu4 = p_bod->tu4;
    p_koord[f].tv4 = p_bod->tv4;
    
    p_koord[f].nx = p_bod->nx;
    p_koord[f].ny = p_bod->ny;
    p_koord[f].nz = p_bod->nz;
    
    p_koord[f].dr = p_bod->dr;
    p_koord[f].dg = p_bod->dg;
    p_koord[f].db = p_bod->db;
    p_koord[f].da = p_bod->da;
    
    p_koord[f].mdr = p_bod->mdr;
    p_koord[f].mdg = p_bod->mdg;
    p_koord[f].mdb = p_bod->mdb;
    p_koord[f].mda = p_bod->mda;
  
    if(spec) {
      p_koord[f].sr = p_bod->sr;
      p_koord[f].sg = p_bod->sg;
      p_koord[f].sb = p_bod->sb;
      
      p_koord[f].msr = p_bod->msr;
      p_koord[f].msg = p_bod->msg;
      p_koord[f].msb = p_bod->msb;
    } else {
      p_koord[f].msr = p_koord[f].sr = 0.0f;
      p_koord[f].msg = p_koord[f].sg = 0.0f;
      p_koord[f].msb = p_koord[f].sb = 0.0f;
    }
  }
  
  p_poly->p_koord = p_koord;
  p_poly->facenum = p_kont->facu;
  p_poly->kflag   = p_kont->kflag;
  p_poly->k2flag  = p_kont->k2flag;
  p_poly->m1flag  = p_kont->m1flag;
  p_poly->m2flag  = p_kont->m2flag;
  
  p_poly->p_mlha = p_kont->p_mlha;
  if(p_kont->p_mlha) {
    p_kont->p_mlha->poly = kID;
    p_kont->p_mlha->p_kont = NULL;
    p_kont->p_mlha = NULL;
  }
  
  p_poly->kreslit = TRUE;  
// TODO
// obb_calc_poly(p_poly);
  
  return(p_poly);
}

void lo_poly_flaguj_materialy(EDIT_MESH_POLY *p_poly, EDIT_MATERIAL **p_mat)
{
  int mat;
  p_poly->m1flag |= p_mat[p_poly->material]->flag;
  p_poly->m2flag |= p_mat[p_poly->material]->flag2;
  mat = p_poly->material;
  
  if(alfa_stage_edit_blok[p_mat[mat]->alfa_state].alfa_pruhledny) {
    p_poly->m1flag |=  MAT_PRUHLEDNY;
  } else {
    p_poly->m1flag &= ~MAT_PRUHLEDNY;
  }
  
  if(p_mat[mat]->alfa_state == STAGE_ALFA_MASKA) {
    p_poly->m2flag |=  MAT2_MASKA;
  } else {
    p_poly->m2flag &= ~MAT2_MASKA;
  }  
}

void lo_poly_oznac_zrcadlo(EDIT_MESH_POLY *p_poly)
{
  p_poly->kflag |= KONT_ZRCADLO;
}

FFILE lo_poly_file_vyrob(char *p_file, int filenum, int velikost)
{
  FFILE f;

  if(!(f = ffopen(p_file,"wb"))) {
    assert(0);
    chyba("file");
  }

  ffwrite(&filenum,sizeof(int),1,f);
  ffwrite(&velikost,sizeof(int),1,f);

  return(f);
}

FFILE lo_poly_file_otevri(char *p_file, int *p_filenum, int velikost)
{
  FFILE f;
  int   vel;

  if(!(f = ffopen(p_file,"rb"))) {
    kprintf(1,"Unable to open file %s",p_file);
    assert(0);
    chyba("file");
  }
  ffread(p_filenum,sizeof(int),1,f);
  ffread(&vel,sizeof(int),1,f);
  if(vel != velikost) {
    ddw("Nesouhlasi verze poly !! Ulozte ho znova.");
    ffclose(f);
    return(NULL);
  }
  return(f);
}

void lo_poly_file_zavri(FFILE f)
{
  ffclose(f);
}

void lo_poly_uloz(FFILE f, EDIT_MESH_POLY *p_poly, EDIT_TEXT *p_light)
{ 
 int i;
  
 ffwrite(p_poly,sizeof(p_poly[0]),1,f);
 ffwrite(p_poly->p_koord,sizeof(p_poly->p_koord[0]),p_poly->facenum,f); 

 for(i = 0; i < p_poly->lightnum; i++) {
   p_poly->p_light[i] = (EDIT_TEXT *)(p_poly->p_light[i]-p_light);
 }
 ffwrite(p_poly->p_light,sizeof(p_poly->p_light[0]),p_poly->lightnum,f);
 ffwrite(p_poly->p_lightnum,sizeof(p_poly->p_lightnum[0]),p_poly->lightnum,f);
 for(i = 0; i < p_poly->lightnum; i++) {
   p_poly->p_light[i] = p_light+(int)p_poly->p_light[i];
 }
}

void lo_poly_nahraj_indir(FFILE f, EDIT_MESH_POLY *p_poly, EDIT_TEXT *p_light)
{
  int i;

  ffread(p_poly,sizeof(p_poly[0]),1,f);
  p_poly->mail = 0;

  p_poly->p_koord = (TEXT_KOORD *)mmalloc(sizeof(p_poly->p_koord[0])*p_poly->facenum);
  ffread(p_poly->p_koord,sizeof(p_poly->p_koord[0]),p_poly->facenum,f);

  p_poly->p_light = (EDIT_TEXT **)mmalloc(sizeof(p_poly->p_light[0])*p_poly->lightnum);
  p_poly->p_lightnum = (int *)mmalloc(sizeof(p_poly->p_lightnum[0])*p_poly->lightnum);
  
  ffread(p_poly->p_light,sizeof(p_poly->p_light[0]),p_poly->lightnum,f);
  ffread(p_poly->p_lightnum,sizeof(p_poly->p_lightnum[0]),p_poly->lightnum,f);
  
  for(i = 0; i < p_poly->lightnum; i++) {
    p_poly->p_light[i] = p_light+(int)p_poly->p_light[i]; 
  }
}

EDIT_MESH_POLY * lo_nahraj_poly_list(char *p_file, int *p_polynum, EDIT_TEXT *p_light, EDIT_MATERIAL **p_mat, int matnum)
{
  EDIT_MESH_POLY *p_poly;
  char  material[50];
  int   i,filenum,mat,mn;
  FFILE f;

  if(!(f = lo_poly_file_otevri(p_file, &filenum, sizeof(p_poly[0])))) {
    *p_polynum = 0;
    return(NULL);
  }

  if(!filenum) {
    *p_polynum = 0;
    return(NULL);
  }

  if(!(p_poly = (EDIT_MESH_POLY *)mmalloc(sizeof(p_poly[0])*filenum))) {
    assert(0); chyba("Pamet!");
  }
  for(i = 0; i < filenum; i++) {
    ffread(material,sizeof(char),50,f);      
    lo_poly_nahraj_indir(f,p_poly+i,p_light);
    
    if(p_poly[i].m1flag&MAT_ANIM_FRAME) { 
      // animovany material u poly-listu skopiruju
      mat = lo_najdi_material(p_mat, matnum, material);
      assert(mat != K_CHYBA);
      
      p_poly[i].material = mn = lo_najdi_prazdny_material(p_mat, matnum);
      assert(mn != K_CHYBA);
      
      p_mat[mn] = kopiruj_material(p_mat[mat]);
      sprintf(p_mat[mn]->jmeno,"%s_ak%d",p_mat[mat]->jmeno,mn);
    } else {
      p_poly[i].material = lo_najdi_material(p_mat, matnum, material);
    }

    assert(p_poly[i].material != K_CHYBA);
  }
  lo_poly_file_zavri(f);
  
  *p_polynum = filenum;

  for(i = 0; i < filenum; i++)
    p_poly[i].kflag |= KONT_UPLOAD;

  return(p_poly);
}


void lo_uloz_poly_list(char *p_file, EDIT_MESH_POLY *p_poly, int polynum, EDIT_TEXT *p_light, EDIT_MATERIAL **p_mat, int matnum)
{
  char  material[50];
  FFILE f;
  int   i;
  
  f = lo_poly_file_vyrob(p_file,polynum,sizeof(p_poly[0]));
  for(i = 0; i < polynum; i++) {

    assert(p_poly[i].material < matnum && p_mat[p_poly[i].material]);
    
    strcpy(material,p_mat[p_poly[i].material]->jmeno);
    ffwrite(material,sizeof(char),50,f);
    
    lo_poly_uloz(f,p_poly+i,p_light);
  }
  lo_poly_file_zavri(f);
}


void lo_poly_calc_lightmap_face(EDIT_MESH_POLY *p_poly)
{
  LIGHTMAP_FACE *p_lf;
  int   facu = p_poly->facenum/3;
  int   ind;
  int   l,i,last = 0;
  int   s,u,v,t;
  BOD   prus;
  float bmp_u,bmp_v;
  BOD   vu,vv;     // smerove vektory x,y
  
  p_poly->p_ind = (LIGHTMAP_FACE *)mmalloc(sizeof(p_poly->p_ind[0])*facu);

  for(l = 0; l < p_poly->lightnum; l++) {        
    for(i = 0; i < p_poly->p_lightnum[l]; i+=3) {
      ind = (last+i)/3;
      p_lf = p_poly->p_ind+ind;
      s = last+i; u = last+i+1; v = last+i+2;

      // Setrideni podle u
      if(p_poly->p_koord[s].tul > p_poly->p_koord[u].tul) {
        t = s; s = u; u = t;
        if(p_poly->p_koord[s].tul > p_poly->p_koord[v].tul) {
          t = s; s = v; v = t;
        } 
      } else {
        if(p_poly->p_koord[v].tul > p_poly->p_koord[u].tul) {
          t = u; u = v; v = t;
        } 
      }

      // Setrideni podle v
      if(p_poly->p_koord[s].tvl > p_poly->p_koord[u].tvl) {
        t = s; s = u; u = t;
        if(p_poly->p_koord[s].tvl > p_poly->p_koord[v].tvl) {
          t = s; s = v; v = t;
        } 
      } else {
        if(p_poly->p_koord[v].tvl < p_poly->p_koord[u].tvl) {
          t = u; u = v; v = t;
        } 
      }

      p_lf->last_u = 0;

      // Ted to je OK, s-u-v      
      p_lf->s0.x = p_poly->p_koord[s].x;
      p_lf->s0.y = p_poly->p_koord[s].y;
      p_lf->s0.z = p_poly->p_koord[s].z;

      vektor_sub((BOD *)(p_poly->p_koord+u),(BOD *)(p_poly->p_koord+s),&vu);
      prusecik((BOD *)(p_poly->p_koord+s), (BOD *)(p_poly->p_koord+u), 
               (BOD *)(p_poly->p_koord+v), &prus);
      vektor_sub((BOD *)(p_poly->p_koord+v),&prus,&vv);
      
      calc_rovinu_bod((BOD *)(p_poly->p_koord+s), (BOD *)(p_poly->p_koord+s+2),
                      (BOD *)(p_poly->p_koord+s+1), &p_poly->p_ind[ind].n);
      norm_rovinu(&p_poly->p_ind[ind].n);
      
      calc_rovinu_bod_vektor((BOD *)(p_poly->p_koord+s), &vu, &p_poly->p_ind[ind].rv);
      norm_rovinu(&p_poly->p_ind[ind].rv);

      calc_rovinu_bod_vektor((BOD *)(p_poly->p_koord+s), &vv, &p_poly->p_ind[ind].ru);
      norm_rovinu(&p_poly->p_ind[ind].ru);

      p_poly->p_ind[ind].ddu = vektor_velikost(&vu);
      p_poly->p_ind[ind].ddv = vektor_velikost(&vv);

      vektor_add(&p_lf->s0,&vv,&p_lf->s1);
      vektor_add(&p_lf->s0,&vu,&p_lf->s2);
      vektor_add(&p_lf->s2,&vv,&p_lf->s3);
/*
      if(p_poly->p_light[l]->p_bmp) {

        bmp_u = (float)p_poly->p_light[l]->p_bmp->x;
        bmp_v = (float)p_poly->p_light[l]->p_bmp->y;
        
        p_poly->p_ind[ind].u = ftoi((float)floor(p_poly->p_koord[s].tul*bmp_u));
        p_poly->p_ind[ind].v = ftoi((float)floor(p_poly->p_koord[s].tvl*bmp_v));
        
        p_poly->p_ind[ind].nu = ftoi((float)ceil(p_poly->p_koord[u].tul*bmp_u));
        p_poly->p_ind[ind].nv = ftoi((float)ceil(p_poly->p_koord[v].tvl*bmp_v));
        
        p_poly->p_ind[ind].nu -= p_poly->p_ind[ind].u;
        p_poly->p_ind[ind].nv -= p_poly->p_ind[ind].v;
        
        p_poly->p_ind[ind].p_bmp = bmp_vyrob(p_poly->p_ind[ind].nu+2,p_poly->p_ind[ind].nv+2);
        p_poly->p_ind[ind].p_zal = bmp_vyrob(p_poly->p_ind[ind].nu+2,p_poly->p_ind[ind].nv+2);


//      MSS_SET_BLOCK_LABEL(p_poly->p_ind[ind].p_bmp, "lo_poly_calc_lightmap_face1");
//      MSS_SET_BLOCK_LABEL(p_poly->p_ind[ind].p_zal, "lo_poly_calc_lightmap_face2");


        bmp_vyber_rec(p_poly->p_light[l]->p_bmp,
                      p_poly->p_ind[ind].p_bmp,
                      p_poly->p_ind[ind].u-1,
                      p_poly->p_ind[ind].v-1);
        bmp_kopiruj(p_poly->p_ind[ind].p_bmp,p_poly->p_ind[ind].p_zal);
      } else {
        kprintf(1,"lo_poly_calc_lightmap_face -> p_bmp = NULL!");
      }
*/      
    }
    last += p_poly->p_lightnum[l];
  }
}


/* **********************************************************************
  Load/save kont/materialy
*/
/* Uklada utomaticky do *.b2m
*/
FFILE lo_uloz_kontejner(EDIT_MATERIAL **p_mat, int max_mat, EDIT_KONTEJNER *p_kont_top, char *p_jmeno, int file, FFILE f)
{   
  EDIT_KONTEJNER *p_kont;  

  if(f == FALSE || !file) {
    if((f = ffopen(p_jmeno,"wb")) == NULL) {
      return(FALSE);
    }
  }
/* TODO  
  lo_uloz_kontejner_chunk(f,p_mat,max_mat,p_kont_top,FALSE);
  p_kont = p_kont_top->p_next;
  while(p_kont) {
    lo_uloz_kontejner_chunk(f,p_mat,max_mat,p_kont,TRUE);
    p_kont = p_kont->p_next;
  }   
*/  
  if(!file) {
    ffclose(f);
    return(NULL);
  } else {
    return(f);
  }
}

/*
  Nacti b2m (OGL) a out meshe
*/
EDIT_KONTEJNER * lo_nahraj_kontejner(EDIT_MATERIAL **p_mat, int max_mat, EDIT_TEXT *p_text, int max_text, char *p_jmeno, int mat)
{
/*
  if(!strcmp(cti_koncovku(p_jmeno),KONCOVKA_MESH_OLD)) {
    return(lo_nahraj_kontejner_out(p_mat,max_mat,p_text,max_text,p_jmeno,mat));
  } else {
    return(lo_nahraj_kontejner_chunk(p_mat,max_mat,p_text,max_text,p_jmeno,mat,FALSE));
  }
*/
}

/* Nahraje b2t nebo mnt materialy
*/
int lo_nahraj_materialy(EDIT_MATERIAL **p_mat, int max_mat, EDIT_TEXT *p_text, 
                        int max_text, char *p_file)
{
/*
 if(!strcmp(cti_koncovku(p_file),KONCOVKA_MATERIAL_OLD)) {
   return(lo_nahraj_materialy_out_jmeno(p_mat,max_mat,p_text,max_text,p_file,TRUE));
 } else {
   return(lo_nahraj_materialy_chunk(p_mat,max_mat,p_text,max_text,p_file,TRUE));
 }
*/
}

/*
  Ulozi vsechny materialy vcetne animaci - pouze jako chunk
*/
int lo_uloz_materialy(EDIT_MATERIAL **p_mat, int max_mat, char *p_file, char *p_dir)
{
 char file[500];
 FFILE f;
 int   i;
 
 strcpy(file,p_file);
 zamen_koncovku(file,KONCOVKA_MATERIAL);
 
 chdir(p_dir);
 if((f = ffopen(file,"wb")) == NULL) {
   return(FALSE);
 } 
 /*
 for(i = 0; i < max_mat; i++) {
   if(p_mat[i])
     lo_uloz_material_chunk(f, p_mat[i]);   
 }
 */
 ffclose(f);
 return(TRUE);
}

int lo_uloz_material(EDIT_MATERIAL *p_mat, char *p_file, char *p_dir)
{
 char file[500];
 FFILE f;
 
 strcpy(file,p_file);
 zamen_koncovku(file,KONCOVKA_MATERIAL);
 
 chdir(p_dir);
 if((f = ffopen(file,"wb")) == NULL) {
   return(FALSE);
 } 
 /*
 lo_uloz_material_chunk(f, p_mat);
 */
 ffclose(f);
 return(TRUE);
}

/*
  ulozi pouzite materily vcetne animaci
*/
int lo_uloz_materialy_pouzite(EDIT_MATERIAL **p_mat, int max_mat, char *p_file, char *p_dir)
{ 
  char file[200];
  FFILE f;
  int i;
  
  strcpy(file,p_file);
  zamen_koncovku(file,KONCOVKA_MATERIAL);
  
  chdir(p_dir);
  if((f = ffopen(file,"wb")) == NULL) {
    return(FALSE);
  } 
  /*
  for(i = 0; i < max_mat; i++) {
    if(p_mat[i] && p_mat[i]->flag&(MAT_POUZITY|MAT_SYSTEM)) {
      lo_uloz_material_chunk(f, p_mat[i]);
    }
  }
  */
  ffclose(f);  
  return(i);
}

/*
  Mesi sekce
*/
GAME_MESH * lo_nahraj_mesh(EDIT_MATERIAL **p_mat, int max_mat, 
                           EDIT_TEXT *p_text, int max_text,
                           char *p_file, int mat,
                           int extra_light)
{
  EDIT_KONTEJNER *p_kont_top,
                 *p_kont;
  GAME_MESH *p_mesh_top = NULL,
            *p_mesh = NULL;

  p_kont = p_kont_top = lo_nahraj_kontejner(p_mat,max_mat,p_text,max_text,p_file,mat);
/*
  if(p_kont && p_kont->bodu && p_kont->objektu) {
    while(p_kont) {
      if(extra_light)
        kont_extra_light(p_kont);    
      lo_setrid_kontejner_materialy(p_kont);
      if(p_mesh) {
        p_mesh = (p_mesh->p_next = edit_to_mesh(p_mesh_top->p_data,p_kont,p_mat,max_mat,FALSE));
      }
      else {
        p_mesh_top = p_mesh = edit_to_mesh(NULL,p_kont,p_mat,max_mat,FALSE);
        p_mesh->p_data->p_mesh = p_mesh;
      }
      p_kont = p_kont->p_next;
    }
  } else {
    p_mesh_top = NULL;
  }
*/
  zrus_kontejner_rec(&p_kont_top,NULL);
  return(p_mesh_top);
}

/* Kontejner to mesh
*/
GAME_MESH * lo_kontejner_to_mesh(EDIT_KONTEJNER **p_kont_top, EDIT_MATERIAL **p_mat, int max_mat, int extra_light)
{
  EDIT_KONTEJNER *p_kont;
  GAME_MESH      *p_mesh_top = NULL,
                 *p_mesh = NULL;
  
  p_kont = *p_kont_top;
/*  
  if(p_kont->bodu && p_kont->objektu) {
    while(p_kont) {
      if(extra_light)
        kont_extra_light(p_kont);
      lo_setrid_kontejner_materialy(p_kont);
      updatuj_kontejner_statistika(p_kont,FALSE);
      if(p_mesh) {
        p_mesh = (p_mesh->p_next = edit_to_mesh(p_mesh_top->p_data,p_kont,p_mat,max_mat,FALSE));
      }
      else {
        p_mesh_top = p_mesh = edit_to_mesh(NULL,p_kont,p_mat,max_mat,FALSE);
        p_mesh->p_data->p_mesh = p_mesh;
      }        
      p_kont = p_kont->p_next;
    }
  } else {
    p_mesh_top = NULL;
  }
*/
  zrus_kontejner_rec(p_kont_top,NULL);
  return(p_mesh_top);
}


// Prevede kontejner tak, aby jeho objekty meli jedinecne jmena
void lo_kontejner_jedinecne_jmena(EDIT_KONTEJNER *p_kont, int ds3)
{
  char pom[200];
  int  i,j,max = 0;

  if(ds3) {
    for(i = 0; i < p_kont->max_objektu; i++) {
      if(p_kont->p_obj[i]) {
        sprintf(p_kont->p_obj[i]->jmeno,"Obj_%d",i);
      }
    }
  } else {
    for(i = 0; i < p_kont->max_objektu; i++) {
      if(p_kont->p_obj[i]) {
        max = i+1;
      }
    }
    
    for(i = 0; i < max; i++) {
      if(!p_kont->p_obj[i])
        continue;
      for(j = i+1; j < max; j++) {
        if(!p_kont->p_obj[j])
          continue;
        if(!strcmp(p_kont->p_obj[i]->jmeno,p_kont->p_obj[j]->jmeno)) {
          strcat(p_kont->p_obj[i]->jmeno,"#");
          strcat(p_kont->p_obj[i]->jmeno,itoa(i,pom,10));
          assert(strlen(p_kont->p_obj[i]->jmeno) < MAX_JMENO);
          break; // koncim a modlim se
        }
      }
    }
  }
}

void lo_kontlist_jedinecne_jmena(EDIT_KONTEJNER **p_kont, int listnum)
{
  char pom[200];
  int  i,j,max = 0;

  // Zjistim posledni objekt
  for(i = 0; i < listnum; i++) {
    if(p_kont[i])
      max = i+1;
  }

  for(i = 0; i < max; i++) {
    if(!p_kont[i])
      continue;
    for(j = i+1; j < max; j++) {
      if(!p_kont[j])
        continue;
      if(!strcmp(p_kont[i]->jmeno,p_kont[j]->jmeno)) {
        strcat(p_kont[i]->jmeno,"#");
        strcat(p_kont[i]->jmeno,itoa(i,pom,10));
        assert(strlen(p_kont[i]->jmeno) < MAX_JMENO);
        break; // koncim a modlim se
      }
    }
  }
}

int lo_najdi_volne_stat_svetlo(STATIC_LIGHT *p_light, int max)
{
  int i;
  for(i = 0; i < max; i++) {
    if(!p_light[i].akt)
      return(i);
  }
  return(K_CHYBA);
}

int lo_najdi_plne_stat_svetlo(STATIC_LIGHT *p_light, int max)
{
  int i;
  for(i = 0; i < max; i++) {
    if(p_light[i].akt)
      return(i);
  }
  return(K_CHYBA);
}

int lo_najdi_volne_dsvetlo(DYN_LIGHT *p_dlight, int max)
{
  int i;
  for(i = 0; i < max; i++) {
    if(!p_dlight[i].akt)
      return(i);
  }
  return(K_CHYBA);
}

int lo_najdi_plne_dsvetlo(DYN_LIGHT *p_dlight, int max)
{
  int i;
  for(i = 0; i < max; i++) {
    if(p_dlight[i].akt)
      return(i);
  }
  return(K_CHYBA);
}

DYN_LIGHT * lo_najdi_volne_dsvetlo_point(DYN_LIGHT *p_dlight, int max)
{
  int i;
  for(i = 0; i < max; i++) {
    if(!p_dlight[i].akt)
      return(p_dlight+i);
  }
  return(NULL);
}

int lo_najdi_prepocitej_dsvetlo(DYN_LIGHT *p_dlight, int max)
{
 int i,dl_lightnum = -1;
 for(i = 0; i < MAX_FLARE_SVETEL; i++) {
   if(p_dlight[i].akt && i > dl_lightnum) {
     dl_lightnum = i;
   }     
 }  
 return(dl_lightnum+1);
}

/*
  Extra-svetla
*/
int lo_najdi_prepocitej_extra_svetlo(EXTRA_DYN_LIGHT *p_dlight, int max)
{
 int i,dl_lightnum = -1;
 for(i = 0; i < MAX_FLARE_SVETEL; i++) {
   if(p_dlight[i].akt && i > dl_lightnum) {
     dl_lightnum = i;
   }     
 }  
 return(dl_lightnum+1);
}

int lo_najdi_volne_extra_svetlo(EXTRA_DYN_LIGHT *p_dlight, int max)
{
  int i;
  for(i = 0; i < max; i++) {
    if(!p_dlight[i].akt)
      return(i);
  }
  return(K_CHYBA);
}

int lo_najdi_plne_extra_svetlo(EXTRA_DYN_LIGHT *p_dlight, int max)
{
  int i;
  for(i = 0; i < max; i++) {
    if(p_dlight[i].akt)
      return(i);
  }
  return(K_CHYBA);
}

EXTRA_DYN_LIGHT * lo_najdi_volne_extra_svetlo_point(EXTRA_DYN_LIGHT *p_dlight, int max)
{
  int i;
  for(i = 0; i < max; i++) {
    if(!p_dlight[i].akt)
      return(p_dlight+i);
  }
  return(NULL);
}

void lo_preved_flare_do_sceny(STATIC_LIGHT *p_light, LENS_FLARE *p_flarelist, int flaremax)
{
  LENS_FLARE      *p_flare;
/*  
  while(p_light) {    
    if(p_light->p_flare) {
      p_flare = lo_kopiruj_flare(p_flarelist,flaremax,p_light->p_flare);
      free(p_light->p_flare);
      p_light->p_flare = p_flare;
      p_flare->p_svetlo = p_light;
    }
    p_light = p_light->p_next;
  }
*/
}

void lo_preved_svetla_do_sceny(EDIT_KONTEJNER *p_kont, STATIC_LIGHT *p_light, int lightnum, DYN_LIGHT *p_dlist, int dlistnum, EXTRA_DYN_LIGHT *p_elist, int elistnum)
{
  STATIC_LIGHT    *p_slight,*p_next;
  DYN_LIGHT       *p_dlight,*p_dnext;
  EXTRA_DYN_LIGHT *p_elight,*p_enext;
  int l,p;
  
  while(p_kont) {
    
  /* Prevedu staticky svetla
    */
    if(p_light) {
      p_slight = p_kont->p_slight;
      
      if(p_slight) {
        l = lo_najdi_volne_stat_svetlo(p_light, lightnum);
        if(l != K_CHYBA) {
          
          p_light[l] = *p_slight;
          if(p_light[l].p_flare)
            ((LENS_FLARE *)(p_light[l].p_flare))->p_svetlo = p_light+l;
          p_light[l].index = l;
          
          p_light[l].p_mesh_data = p_kont;
          p_light[l].p_prev = NULL;
          p_light[l].p_next = NULL;
          p_kont->p_slight = p_light+l;
          
          p = l;
          p_next = p_slight->p_next;
          free(p_slight);
          p_slight = p_next;
          
          while(p_slight) {
            l = lo_najdi_volne_stat_svetlo(p_light, lightnum);
            if(l != K_CHYBA) {
              
              p_light[l] = *p_slight;
              if(p_light[l].p_flare)
                ((LENS_FLARE *)(p_light[l].p_flare))->p_svetlo = p_light+l;
              p_light[l].index = l;
              
              p_light[l].p_mesh_data = p_kont;
              p_light[l].p_next = NULL;
              p_light[l].p_prev = p_light+p;
              p_light[p].p_next = p_light+l;
              
              p = l;
              p_next = p_slight->p_next;
              free(p_slight);
              p_slight = p_next;
            } else {
              ddw("Madrfakr!!!! - Dosly nam svetla!");
            }
          }
        }
      }
    }
    
    
    /* Prevedu dynamicky svetla
    */
    if(p_dlist) {
      p_dlight = p_kont->p_dlight;
      
      if(p_dlight) {
        l = lo_najdi_volne_dsvetlo(p_dlist, dlistnum);
        if(l != K_CHYBA) {
          
          p_dlist[l] = *p_dlight;
          p_dlist[l].index = l;
          
          p_dlist[l].p_mesh_data = p_kont;
          p_dlist[l].p_prev = NULL;
          p_dlist[l].p_next = NULL;
          p_kont->p_dlight = p_dlist+l;
          
          p = l;
          p_dnext = p_dlight->p_next;
          free(p_dlight);
          p_dlight = p_dnext;
          
          while(p_dlight) {
            l = lo_najdi_volne_dsvetlo(p_dlist, dlistnum);
            if(l != K_CHYBA) {
              
              p_dlist[l] = *p_dlight;
              p_dlist[l].index = l;
              
              p_dlist[l].p_mesh_data = p_kont;
              p_dlist[l].p_next = NULL;
              p_dlist[l].p_prev = p_dlist+p;
              p_dlist[p].p_next = p_dlist+l;
              
              p = l;
              p_dnext = p_dlight->p_next;
              free(p_dlight);
              p_dlight = p_dnext;
            } else {
              ddw("Madrfakr!!!! - Dosly nam dyn. svetla!");
            }
          }
        }
      }
    }
    
    /* Prevedu extra-dynamicky svetla
    */
    if(p_elist) {
      p_elight = p_kont->p_edlight;
      
      if(p_elight) {
        l = lo_najdi_volne_extra_svetlo(p_elist, elistnum);
        if(l != K_CHYBA) {
          
          p_elist[l] = *p_elight;
          p_elist[l].index = l;
          
          p_elist[l].p_mesh_data = p_kont;
          p_elist[l].p_prev = NULL;
          p_elist[l].p_next = NULL;
          p_kont->p_edlight = p_elist+l;
          
          p = l;
          p_enext = p_elight->p_next;
          free(p_elight);
          p_elight = p_enext;
          
          while(p_elight) {
            l = lo_najdi_volne_extra_svetlo(p_elist, elistnum);
            if(l != K_CHYBA) {
              
              p_elist[l] = *p_elight;
              p_elist[l].index = l;
              
              p_elist[l].p_mesh_data = p_kont;
              p_elist[l].p_next = NULL;
              p_elist[l].p_prev = p_elist+p;
              p_elist[p].p_next = p_elist+l;
              
              p = l;
              p_enext = p_elight->p_next;
              free(p_elight);
              p_elight = p_enext;
            } else {
              ddw("Madrfakr!!!! - Dosly nam extra svetla!");
            }
          }
        }
      }
    }
    p_kont = p_kont->p_next;
  }
}

// skopiruje svetla do sceny a navaze se na nove svetla
void lo_kopiruj_svetla_do_sceny(EDIT_KONTEJNER *p_kont, STATIC_LIGHT *p_light, int lightnum, DYN_LIGHT *p_dlist, int dlistnum, EXTRA_DYN_LIGHT *p_elist, int elistnum, LENS_FLARE *p_flares, int maxflares)
{
  STATIC_LIGHT    *p_slight;
  DYN_LIGHT       *p_dlight;
  EXTRA_DYN_LIGHT *p_elight;
  int l,p;

  // Prevedu staticky svetla
/*
  if(p_light) {
    p_slight = p_kont->p_slight;

    if(p_slight) {
      l = lo_najdi_volne_stat_svetlo(p_light, lightnum);
      if(l != K_CHYBA) {
        
        p_light[l] = *p_slight;
        if(p_light[l].p_flare) {
          p_light[l].p_flare = lo_kopiruj_flare(p_flares,maxflares,p_light[l].p_flare);
          ((LENS_FLARE *)(p_light[l].p_flare))->p_svetlo = p_light+l;
        }

        p_light[l].index = l;

        p_light[l].p_mesh_data = p_kont;
        p_light[l].p_prev = NULL;
        p_light[l].p_next = NULL;
        p_kont->p_slight = p_light+l;
        
        p = l;
        p_slight = p_slight->p_next;        
        
        while(p_slight) {
          l = lo_najdi_volne_stat_svetlo(p_light, lightnum);
          if(l != K_CHYBA) {
                        
            p_light[l] = *p_slight;
            if(p_light[l].p_flare) {
              p_light[l].p_flare = lo_kopiruj_flare(p_flares,maxflares,p_light[l].p_flare);
              ((LENS_FLARE *)(p_light[l].p_flare))->p_svetlo = p_light+l;
            }
            p_light[l].index = l;
            
            p_light[l].p_mesh_data = p_kont;
            p_light[l].p_next = NULL;
            p_light[l].p_prev = p_light+p;
            p_light[p].p_next = p_light+l;
            
            p = l;
            p_slight = p_slight->p_next;
          } else {
            ddw("Madrfakr!!!! - Dosly nam svetla!");
          }
        }
      }
    }
  }

  // Prevedu dynamicky svetla
  if(p_dlist) {
    p_dlight = p_kont->p_dlight;
    
    if(p_dlight) {
      l = lo_najdi_volne_dsvetlo(p_dlist, dlistnum);
      if(l != K_CHYBA) {
        
        p_dlist[l] = *p_dlight;
        p_dlist[l].index = l;

        p_dlist[l].p_mesh_data = p_kont;
        p_dlist[l].p_prev = NULL;
        p_dlist[l].p_next = NULL;
        p_kont->p_dlight = p_dlist+l;
        
        p = l;        
        p_dlight = p_dlight->p_next;
        
        while(p_dlight) {
          l = lo_najdi_volne_dsvetlo(p_dlist, dlistnum);
          if(l != K_CHYBA) {
                        
            p_dlist[l] = *p_dlight;
            p_dlist[l].index = l;
            
            p_dlist[l].p_mesh_data = p_kont;
            p_dlist[l].p_next = NULL;
            p_dlist[l].p_prev = p_dlist+p;
            p_dlist[p].p_next = p_dlist+l;
            
            p = l;
            p_dlight = p_dlight->p_next;
          } else {
            ddw("Madrfakr!!!! - Dosly nam dyn. svetla!");
          }
        }
      }
    }
  }

  // Prevedu extra svetla
  if(p_elist) {
    p_elight = p_kont->p_edlight;
    
    if(p_elight) {
      l = lo_najdi_volne_extra_svetlo(p_elist, elistnum);
      if(l != K_CHYBA) {
        
        p_elist[l] = *p_elight;
        p_elist[l].index = l;

        p_elist[l].p_mesh_data = p_kont;
        p_elist[l].p_prev = NULL;
        p_elist[l].p_next = NULL;
        p_kont->p_edlight = p_elist+l;
        
        p = l;        
        p_elight = p_elight->p_next;
        
        while(p_elight) {
          l = lo_najdi_volne_extra_svetlo(p_elist, elistnum);
          if(l != K_CHYBA) {
                        
            p_elist[l] = *p_elight;
            p_elist[l].index = l;
            
            p_elist[l].p_mesh_data = p_kont;
            p_elist[l].p_next = NULL;
            p_elist[l].p_prev = p_elist+p;
            p_elist[p].p_next = p_elist+l;
            
            p = l;
            p_elight = p_elight->p_next;
          } else {
            ddw("Madrfakr!!!! - Dosly nam extra svetla!");
          }
        }
      }
    }
  }
  */
}

// skopiruje svetla do sceny a navaze se na nove svetla
void lo_kopiruj_svetla_do_sceny_mesh(GAME_MESH_DATA *p_data, DYN_LIGHT *p_dlist, int dlistnum, EXTRA_DYN_LIGHT *p_elist, int elistnum)
{
  DYN_LIGHT       *p_dlight;
  EXTRA_DYN_LIGHT *p_elight;
  int l,p;

  /* Prevedu dynamicky svetla
  */
  if(p_dlist) {
    p_dlight = p_data->p_ldlight;
    if(p_dlight) {
      l = lo_najdi_volne_dsvetlo(p_dlist, dlistnum);
      if(l != K_CHYBA) {
        
        p_dlist[l] = *p_dlight;
        p_dlist[l].index = l;

        p_dlist[l].p_mesh_data = p_data;
        p_dlist[l].p_prev = NULL;
        p_dlist[l].p_next = NULL;
        p_data->p_ldlight = p_dlist+l;
        
        p = l;        
        p_dlight = p_dlight->p_next;                
        
        while(p_dlight) {
          l = lo_najdi_volne_dsvetlo(p_dlist, dlistnum);
          if(l != K_CHYBA) {
                        
            p_dlist[l] = *p_dlight;
            p_dlist[l].index = l;
        
            p_dlist[l].p_mesh_data = p_data;
            p_dlist[l].p_next = NULL;
            p_dlist[l].p_prev = p_dlist+p;
            p_dlist[p].p_next = p_dlist+l;
            
            p = l;
            p_dlight = p_dlight->p_next;
          } else {
            ddw("Madrfakr!!!! - Dosly nam dyn. svetla!");
          }
        }
      }
    }
  }

  /* Prevedu extra svetla
  */
  if(p_elist) {
    p_elight = p_data->p_lelight;
    if(p_dlight) {
      l = lo_najdi_volne_extra_svetlo(p_elist, elistnum);
      if(l != K_CHYBA) {
        
        p_elist[l] = *p_elight;
        p_elist[l].index = l;

        p_elist[l].p_mesh_data = p_data;
        p_elist[l].p_prev = NULL;
        p_elist[l].p_next = NULL;
        p_data->p_lelight = p_elist+l;
        
        p = l;        
        p_elight = p_elight->p_next;                
        
        while(p_elight) {
          l = lo_najdi_volne_extra_svetlo(p_elist, elistnum);
          if(l != K_CHYBA) {
                        
            p_elist[l] = *p_elight;
            p_elist[l].index = l;
        
            p_elist[l].p_mesh_data = p_data;
            p_elist[l].p_next = NULL;
            p_elist[l].p_prev = p_elist+p;
            p_elist[p].p_next = p_elist+l;
            
            p = l;
            p_elight = p_elight->p_next;
          } else {
            ddw("Madrfakr!!!! - Dosly nam dyn. svetla!");
          }
        }
      }
    }
  }
}

void lo_preved_svetla_do_sceny_mesh(GAME_MESH_DATA *p_data, DYN_LIGHT *p_dlist, int dlistnum, EXTRA_DYN_LIGHT *p_elist, int elistnum)
{  
  DYN_LIGHT       *p_dlight,*p_dnext;
  EXTRA_DYN_LIGHT *p_elight,*p_enext;
  int l,p;

  /* Prevedu dynamicky svetla
  */
  if(p_dlist) {
    p_dlight = p_data->p_ldlight;
    
    if(p_dlight) {
      l = lo_najdi_volne_dsvetlo(p_dlist, dlistnum);
      if(l != K_CHYBA) {
        
        p_dlist[l] = *p_dlight;
        p_dlist[l].index = l;
        
        p_dlist[l].p_mesh_data = p_data;
        p_dlist[l].p_prev = NULL;
        p_dlist[l].p_next = NULL;
        p_data->p_ldlight = p_dlist+l;
        
        p = l;
        p_dnext = p_dlight->p_next;
        free(p_dlight);
        p_dlight = p_dnext;
        
        while(p_dlight) {
          l = lo_najdi_volne_dsvetlo(p_dlist, dlistnum);
          if(l != K_CHYBA) {
                        
            p_dlist[l] = *p_dlight;
            p_dlist[l].index = l;
            
            p_dlist[l].p_mesh_data = p_data;
            p_dlist[l].p_next = NULL;
            p_dlist[l].p_prev = p_dlist+p;
            p_dlist[p].p_next = p_dlist+l;
            
            p = l;
            p_dnext = p_dlight->p_next;
            free(p_dlight);
            p_dlight = p_dnext;
          } else {
            ddw("Madrfakr!!!! - Dosly nam dyn. svetla!");
          }
        }
      }
    }
  }

  /* Prevedu dynamicky svetla
  */
  if(p_elist) {
    p_elight = p_data->p_lelight;
    
    if(p_elight) {
      l = lo_najdi_volne_extra_svetlo(p_elist, elistnum);
      if(l != K_CHYBA) {
        
        p_elist[l] = *p_elight;
        p_elist[l].index = l;
        
        p_elist[l].p_mesh_data = p_data;
        p_elist[l].p_prev = NULL;
        p_elist[l].p_next = NULL;
        p_data->p_lelight = p_elist+l;
        
        p = l;
        p_enext = p_elight->p_next;
        free(p_elight);
        p_elight = p_enext;
        
        while(p_elight) {
          l = lo_najdi_volne_extra_svetlo(p_elist, elistnum);
          if(l != K_CHYBA) {
                        
            p_elist[l] = *p_elight;
            p_elist[l].index = l;
            
            p_elist[l].p_mesh_data = p_data;
            p_elist[l].p_next = NULL;
            p_elist[l].p_prev = p_elist+p;
            p_elist[p].p_next = p_elist+l;
            
            p = l;
            p_enext = p_elight->p_next;
            free(p_elight);
            p_elight = p_enext;
          } else {
            ddw("Madrfakr!!!! - Dosly nam dyn. svetla!");
          }
        }
      }
    }
  }
}

void lo_vymaz_svetla_ze_sceny(EDIT_KONTEJNER *p_kont)
{
  STATIC_LIGHT    *p_slight = p_kont->p_slight, *p_snext;
  DYN_LIGHT       *p_dlight = p_kont->p_dlight, *p_dnext;
  EXTRA_DYN_LIGHT *p_elight = p_kont->p_edlight,*p_enext;

  while(p_slight) {
    p_snext = p_slight->p_next;
    memset(p_slight,0,sizeof(p_slight[0]));
    p_slight = p_snext;
  }  
  while(p_dlight) {
    p_dnext = p_dlight->p_next;
    memset(p_dlight,0,sizeof(p_dlight[0]));
    p_dlight = p_dnext;
  }
  while(p_elight) {
    p_enext = p_elight->p_next;
    memset(p_elight,0,sizeof(p_elight[0]));
    p_elight = p_enext;
  }
}

void lo_vymaz_svetla(EDIT_KONTEJNER *p_kont)
{
  STATIC_LIGHT    *p_slight = p_kont->p_slight,*p_snext;
  DYN_LIGHT       *p_dlight = p_kont->p_dlight,*p_dnext;
  EXTRA_DYN_LIGHT *p_elight = p_kont->p_edlight,*p_enext;

  while(p_slight) {
    p_snext = p_slight->p_next;
    free(p_slight);
    p_slight = p_snext;
  }  
  while(p_dlight) {
    p_dnext = p_dlight->p_next;
    free(p_dlight);
    p_dlight = p_dnext;
  }
  while(p_elight) {
    p_enext = p_elight->p_next;
    free(p_elight);
    p_elight = p_enext;
  }
}

void lo_vymaz_svetla_ze_sceny_mesh(GAME_MESH_DATA *p_data)
{  
  DYN_LIGHT       *p_dlight = p_data->p_ldlight,*p_dnext;
  EXTRA_DYN_LIGHT *p_elight = p_data->p_lelight,*p_enext;

  while(p_dlight) {
    p_dnext = p_dlight->p_next;
    memset(p_dlight,0,sizeof(p_dlight[0]));
    p_dlight = p_dnext;    
  }
  while(p_elight) {
    p_enext = p_elight->p_next;
    memset(p_elight,0,sizeof(p_elight[0]));
    p_elight = p_enext;    
  }  
}

void lo_vymaz_svetla_mesh(GAME_MESH_DATA *p_data)
{  
  DYN_LIGHT       *p_dlight = p_data->p_ldlight,*p_dnext;
  EXTRA_DYN_LIGHT *p_elight = p_data->p_lelight,*p_enext;

  while(p_dlight) {
    p_dnext = p_dlight->p_next;
    free(p_dlight);
    p_dlight = p_dnext;    
  }  
  while(p_elight) {
    p_enext = p_elight->p_next;
    free(p_elight);
    p_elight = p_enext;
  }  
}

void lo_premapuj_svetla_kont_mesh(EDIT_KONTEJNER *p_src, GAME_MESH *p_dest)
{
  STATIC_LIGHT    *p_slight = p_src->p_slight;
  DYN_LIGHT       *p_dlight = p_src->p_dlight;
  EXTRA_DYN_LIGHT *p_elight = p_src->p_edlight;

  if(!p_src || !p_dest)
    return;
/*
  while(p_slight) {
    p_slight->p_mesh_data = p_dest->p_data;
    p_slight = p_slight->p_next;    
  }  
  p_dest->p_data->p_lslight = p_src->p_slight;
  p_src->p_slight = NULL;

  while(p_dlight) {
    p_dlight->p_mesh_data = p_dest->p_data;
    p_dlight = p_dlight->p_next;
  }
  p_dest->p_data->p_ldlight = p_src->p_dlight;
  p_src->p_dlight = NULL;

  while(p_elight) {
    p_elight->p_mesh_data = p_dest->p_data;
    p_elight = p_elight->p_next;
  }
  p_dest->p_data->p_lelight = p_src->p_edlight;
  p_src->p_edlight = NULL;
  */
}

void lo_premapuj_svetla_mesh(GAME_MESH *p_src, GAME_MESH *p_dest)
{
/*
  STATIC_LIGHT    *p_slight = p_src->p_data->p_lslight;
  DYN_LIGHT       *p_dlight = p_src->p_data->p_ldlight;
  EXTRA_DYN_LIGHT *p_elight = p_src->p_data->p_lelight;

  if(!p_src || !p_dest)
    return;

  while(p_slight) {
    p_slight->p_mesh_data = p_dest->p_data;
    p_slight = p_slight->p_next;    
  }  
  p_dest->p_data->p_lslight = p_src->p_data->p_lslight;
  p_src->p_data->p_lslight = NULL;

  while(p_dlight) {
    p_dlight->p_mesh_data = p_dest->p_data;
    p_dlight = p_dlight->p_next;
  }
  p_dest->p_data->p_ldlight = p_src->p_data->p_ldlight;
  p_src->p_data->p_ldlight = NULL;

  while(p_elight) {
    p_elight->p_mesh_data = p_dest->p_data;
    p_elight = p_elight->p_next;
  }
  p_dest->p_data->p_lelight = p_src->p_data->p_lelight;
  p_src->p_data->p_lelight = NULL;
  */
}

void lo_transformuj_svetla_do_wordspace(EDIT_KONTEJNER *p_src)
{
  STATIC_LIGHT    *p_slight = p_src->p_slight;
  DYN_LIGHT       *p_dlight = p_src->p_dlight;
  EXTRA_DYN_LIGHT *p_elight = p_src->p_edlight;
  GLMATRIX        *p_top = kont_world_matrix(p_src);

  if(!p_src)
    return;
  while(p_slight) {
    transformuj_bod_bod_matici(&p_slight->p,p_top);
    p_slight = p_slight->p_next;
  }
  while(p_dlight) {
    transformuj_bod_bod_matici(&p_dlight->np,p_top);
    p_dlight->tp = p_dlight->np;
    p_dlight = p_dlight->p_next;
  }  
  while(p_elight) {
    transformuj_bod_bod_matici(&p_elight->np,p_top);
    p_elight->tp = p_elight->np;
    p_elight = p_elight->p_next;
  }  
}

void lo_premapuj_svetla_do_wordspace(EDIT_KONTEJNER *p_src)
{
  STATIC_LIGHT    *p_slight = p_src->p_slight;
  DYN_LIGHT       *p_dlight = p_src->p_dlight;
  EXTRA_DYN_LIGHT *p_elight = p_src->p_edlight;
  GLMATRIX        *p_top = kont_world_matrix(p_src);

  if(!p_src)
    return;

  lo_transformuj_svetla_do_wordspace(p_src);

  while(p_slight) {
    if(p_slight->p_flare) {
      ((LENS_FLARE *)(p_slight->p_flare))->p = p_slight->p;
      ((LENS_FLARE *)(p_slight->p_flare))->p_svetlo = NULL;
    }
    p_slight->p_mesh_data = NULL;
    p_slight->flag2 &= ~SL2_MESH;
    p_slight = p_slight->p_next;
  }
  p_src->p_slight = NULL;

  while(p_dlight) {
    p_dlight->p_mesh_data = NULL;
    p_dlight->flag &= ~SDL_MESH;
    p_dlight = p_dlight->p_next;
  }  
  p_src->p_dlight = NULL;

  while(p_elight) {
    p_elight->p_mesh_data = NULL;
    p_elight->flag &= ~SDL_MESH;
    p_elight = p_elight->p_next;
  }  
  p_src->p_edlight = NULL;
}

LENS_FLARE * lo_kopiruj_flare(LENS_FLARE *p_flarelist, int max, LENS_FLARE *p_flare)
{   
/*
  int f = lo_najdi_volny_flare(p_flarelist,max);
  p_flarelist[f] = *p_flare;
  if(p_flare->p_sloz) {
    p_flarelist[f].p_sloz = mmalloc(sizeof(p_flarelist[f].p_sloz[0])*SLOZ_FLARE);
    memcpy(p_flarelist[f].p_sloz,p_flare->p_sloz,sizeof(p_flarelist[f].p_sloz[0])*SLOZ_FLARE);
  }
  p_flarelist[f].index = f;
  return(p_flarelist+f);
*/
}

void mesh_pridej_vertex_array(GAME_MESH *p_mesh)
{
/*
  int flag,m2flag = p_mesh->p_data->m2flag;
  int norm = 0;
  
  if(extlist_vertex_array) {
    mesh_vertex_array_init(p_mesh);

    flag = KONT2_UPDATE_POS|KONT2_UPDATE_NORM;
    if(m2flag&MAT2_DIFFUSE)  flag |= KONT2_UPDATE_DIFF;
    if(m2flag&(MAT2_SPECULAR|MAT2_ENV_SPEC)) flag |= KONT2_UPDATE_SPEC;
    if(m2flag&MAT2_MAP1) flag |= KONT2_UPDATE_TEXT1;
    if(m2flag&MAT2_MAP2) flag |= KONT2_UPDATE_TEXT2;
    if(m2flag&MAT2_MAP3) flag |= KONT2_UPDATE_TEXT3;
    if(m2flag&MAT2_MAP4) flag |= KONT2_UPDATE_TEXT4;
    p_mesh->p_data->k2flag |= flag|KONT2_UPDATE_NEW;

    mesh_vertex_array_upload(p_mesh);
  }
*/
}

void poly_pridej_vertex_array(EDIT_MESH_POLY *p_poly)
{
  memset(&p_poly->varray,0,sizeof(p_poly->varray));
  poly_vertex_array_init(p_poly);
  poly_vertex_array_upload(p_poly);
}

int * NvConsolidateSingleFace(EXTRA_FACE *p_eface, int facenum, int *p_facevel)
{
  int  f,velikost;
  int *p_face;
  int  akt,j;

  velikost = 0;
  for(f = 0; f < facenum; f++) {
    velikost += p_eface[f].facenum+2;
  }

  p_face = (int *)mmalloc(sizeof(int)*velikost);
  *p_facevel = velikost;

  akt = 0;
  for(f = 0; f < facenum; f++) {
    p_face[akt++] = p_eface[f].facenum;
    p_face[akt++] = p_eface[f].typ;
    for(j = 0; j < p_eface[f].facenum; j++)
      p_face[akt++] = p_eface[f].p_face[j];
    free(p_eface[f].p_face);
  }
  return(p_face);
}


void NvStripToGL(EXTRA_FACE *p_face, int facenum)
{
  int i;
  
  for(i = 0; i < facenum; i++) {
    if(p_face[i].typ == ST_LIST) {
      p_face[i].typ = GL_TRIANGLES;
    } else if(p_face[i].typ == ST_STRIP) {
      p_face[i].typ = GL_TRIANGLE_STRIP;    
    } else if(p_face[i].typ == ST_FAN) {
      p_face[i].typ = GL_TRIANGLE_FAN;    
    }    
  }
}

void NvGenerateStrips_fast(unsigned short *p_ind, int num, STRIP_FACE **p_fface, int *p_facenum)
{
  STRIP_FACE *p_face = (STRIP_FACE *)mmalloc(sizeof(STRIP_FACE));

  p_face->typ = GL_TRIANGLES;
  p_face->facenum = num;
  p_face->p_face = (unsigned short *)mmalloc(sizeof(p_ind[0])*num);
  memcpy(p_face->p_face,p_ind,sizeof(p_ind[0])*num);
  
 *p_fface = p_face;
 *p_facenum = 1;
}

void oe_obj_to_strip(EDIT_OBJEKT *p_obj)
{
  EXTRA_FACE    *p_face;
  unsigned short facenum;

  NvGenerateStrips(p_obj->p_face,p_obj->facenum,(STRIP_FACE **)&p_face,&facenum);
  NvStripToGL(p_face,facenum);
  
  p_obj->p_opt = NvConsolidateSingleFace(p_face, facenum, &p_obj->optnum);  
  free(p_face);
}

void oe_kont_to_strip(EDIT_KONTEJNER *p_kont_top)
{
  EDIT_KONTEJNER *p_kont;
  EDIT_OBJEKT    *p_obj;
  int o;

  p_kont = p_kont_top;
  while(p_kont) {
    oe_olist_reset(&o);
    while((p_obj = oe_olist_next(p_kont,&o))) {
      oe_obj_to_strip(p_obj);
    }
    p_kont = p_kont->p_next;
  }
}
