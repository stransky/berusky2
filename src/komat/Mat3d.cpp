/*
  Engine - samotna parba
*/
#include "3d_all.h"

void transformuj_kontejner_matici(EDIT_KONTEJNER *p_kont, GLMATRIX *p_m)
{
 OBJ_VERTEX *p_vert;
 BOD        *p_kost;
 int        i,j;
 int        pocet;
 int        celkem = 0;

 if(p_kont == NULL)
   return;

 for(j = 0; j < p_kont->max_objektu; j++) {
   if(!p_kont->p_obj[j])
     continue;
    
   p_vert = p_kont->p_obj[j]->p_vertex;
   pocet = p_kont->p_obj[j]->vertexnum;
 
   for(i = 0; i < pocet; i++) {
     transformuj_bod_matici(&p_vert[i].x, &p_vert[i].y, &p_vert[i].z, p_m);
   }
   
   p_kost = p_kont->p_obj[j]->p_vertex_kosti;
   if(p_kost) {
     for(i = 0; i < pocet; i++) {
       transformuj_bod_matici(&p_kost[i].x, &p_kost[i].y, &p_kost[i].z, p_m);
     }
   }
 }
}

// Vymaze key-frame!!!
void transformuj_kontejner_keyframe(EDIT_KONTEJNER *p_kont)
{
  EDIT_OBJEKT *p_obj;
  int i,o;

  if(p_kont->kflag&KONT_KEYFRAME) {
    oe_olist_reset(&o);
    while(p_obj = oe_olist_next(p_kont,&o)) {
      transformuj_objekt_matici(p_obj,&p_obj->m);
      init_matrix(&p_obj->m);
    }
    init_matrix(&p_kont->world);
    init_matrix(&p_kont->mpmatrix);
    p_kont->kflag&=~KONT_KEYFRAME;
    for(i = 0; i < KONT_MAX_ANIM; i++) {
      key_sim_root_zrus(p_kont->sim+i);
    }
  } 
}

void transformuj_kontejner(EDIT_KONTEJNER *p_kont)
{
  if(!(p_kont->kflag&KONT_KEYFRAME)) {
    lo_transformuj_svetla_do_wordspace(p_kont);
    transformuj_kontejner_matici(p_kont, kont_world_matrix(p_kont));
    init_matrix(&p_kont->world);
    init_matrix(&p_kont->mpmatrix);
  }
}

void transformuj_mesh(GAME_MESH *p_mesh)
{   
/*
  dword i;
  if(!(p_mesh->p_data->kflag&KONT_KEYFRAME)) {
    for(i = 0; i < p_mesh->vertexnum; i++) {
      transformuj_bod_bod_matici(p_mesh->p_vertex_pos+i,&p_mesh->m);
    }
    init_matrix(&p_mesh->m);
  }
*/
}

void calc_norm_vec_inter_object(EDIT_OBJEKT *p_obj)
{
/*
 int    j,i,v,v1,k;
 float  p_x,p_y,p_z,p;
 ROVINA r;
 byte   *p_hv = mmalloc(p_obj->vertexnum);
 word   *p_nasle = mmalloc(p_obj->vertexnum * sizeof(word));
 word   naslych = 0;
 
 for(v = 0; v < p_obj->vertexnum; v++) {
   if(p_hv[v]) // pokud byl pouzit -> preskoc ho
     continue;
   else
     p_hv[v] = 1;
   
   p_nasle[0] = v;
   naslych = 1;

   for(v1 = 0; v1 < p_obj->vertexnum; v1++) {
     if(v == v1)
       continue;
     if(stejny_vertex(p_obj->p_vertex[v],p_obj->p_vertex[v1])) {
       p_nasle[naslych++] = v1;
       p_hv[v1] = 1;
     }
   }
   
   // mam seznam vsech stejnych vertexu -> najdu k nim facy
   p_x = p_y = p_z = p = 0;
   for(k = 0; k < naslych; k++) {
     j = p_nasle[k];
     for(i = 0; i < p_obj->facenum; i+=3) {
       if((p_obj->p_face[i] == j)||(p_obj->p_face[i+1] == j)||(p_obj->p_face[i+2] == j)) {
         // spocitat rovinu
         calc_rovinu(p_obj->p_vertex+p_obj->p_face[i], p_obj->p_vertex+p_obj->p_face[i+1], p_obj->p_vertex+p_obj->p_face[i+2], &r);
         p_x += r.x;
         p_y += r.y;
         p_z += r.z;
         p++;
       }
     }
   }
   p_x /= p; p_y /= p; p_z /= p;
   norm_vect(&p_x,&p_y,&p_z);
   if(p) {
     for(k = 0; k < naslych; k++) {
       j = p_nasle[k];
       p_obj->p_vertex[j].nx = p_x;
       p_obj->p_vertex[j].ny = p_y;
       p_obj->p_vertex[j].nz = p_z;
     }
   }
 }
   
 free(p_hv);
 free(p_nasle);
 */
}

void texturuj_zemi(EDIT_OBJEKT *p_obj, int rect_x, int rect_y, int c_text, 
                   int flag, int mat, float skok_u, float skok_v)
{
  float *p_fl;
  int    x,y,i;

  rect_x++;
  rect_y++;

  p_obj->material = mat;
  
  if(flag & T_MIRROR) {
    for(y = 0; y < rect_y-1; y += 2) {
      for(x = 0; x < rect_x-1; x += 2) {
        // spodni rada
        i = x + y*rect_x;
        p_fl = &p_obj->p_vertex[i].tu1;
        p_fl[0+c_text*2] = 0.0f; //text x
        p_fl[1+c_text*2] = 0.0f; //text y
   
        i = x+1+y*rect_x;
        p_fl = &p_obj->p_vertex[i].tu1;
        p_fl[0+c_text*2] = 1.0f;
        p_fl[1+c_text*2] = 0.0f;
   
        i = x+2+y*rect_x;
        p_fl = &p_obj->p_vertex[i].tu1;
        p_fl[0+c_text*2] = 0.0f;
        p_fl[1+c_text*2] = 0.0f;
   
        // stredni rada
        i = x + (y+1)*rect_x;
        p_fl = &p_obj->p_vertex[i].tu1;
        p_fl[0+c_text*2] = 0.0f;
        p_fl[1+c_text*2] = 1.0f;
   
        i = x+1+(y+1)*rect_x;
        p_fl = &p_obj->p_vertex[i].tu1;
        p_fl[0+c_text*2] = 1.0f;
        p_fl[1+c_text*2] = 1.0f;
   
        i = x+2+(y+1)*rect_x;
        p_fl = &p_obj->p_vertex[i].tu1;
        p_fl[0+c_text*2] = 0.0f;
        p_fl[1+c_text*2] = 1.0f;
   
        // horni rada
        i = x + (y+2)*rect_x;
        p_fl = &p_obj->p_vertex[i].tu1;
        p_fl[0+c_text*2] = 0.0f;
        p_fl[1+c_text*2] = 0.0f;
   
        i = x+1+(y+2)*rect_x;
        p_fl = &p_obj->p_vertex[i].tu1;
        p_fl[0+c_text*2] = 1.0f;     
        p_fl[1+c_text*2] = 0.0f;
   
        i = x+2+(y+2)*rect_x;
        p_fl = &p_obj->p_vertex[i].tu1;
        p_fl[0+c_text*2] = 0.0f;
        p_fl[1+c_text*2] = 0.0f;
      }
    }
  }

  if(flag & T_WRAP) {
    for(y = 0; y < rect_y; y++) {
      for(x = 0; x < rect_x; x++) {
        i = x + y*rect_x;
        p_fl = &p_obj->p_vertex[i].tu1;
        p_fl[0+c_text*2] = ((float)x)*skok_u; //text x
        p_fl[1+c_text*2] = ((float)y)*skok_v; //text y
      }
    }
  }
}

EDIT_OBJEKT * vyrob_zemi(int rect_x, int rect_y, float skok)
{
  return(vyrob_zemi_start(rect_x, rect_y, skok, 0.0f, 0.0f));
}

EDIT_OBJEKT * vyrob_zemi_start(int rect_x, int rect_y, float skok, float start_x, float start_y)
{
  int    num_x = rect_x+1;
  int    num_y = rect_y+1;
  int    vertexnum = num_x*num_y;
  int    facenum = rect_x*rect_y*2*3;
  EDIT_OBJEKT *p_obj = vyrob_objekt();
  int    i,x,y;

  p_obj->vertexnum = vertexnum;
  if((p_obj->p_vertex = (OBJ_VERTEX *)mmalloc(sizeof(p_obj->p_vertex[0])*p_obj->vertexnum)) == NULL) {
    zrus_objekt(&p_obj);
    return(NULL);
  }

  p_obj->facenum = facenum;
  if((p_obj->p_face = (word *)mmalloc(sizeof(p_obj->p_face[0])*p_obj->facenum)) == NULL) {
    zrus_objekt(&p_obj);
    return(NULL);
  }
    
  for(y = 0, i = 0; y < num_y; y++) {
    for(x = 0; x < num_x; x++) {
      p_obj->p_vertex[i].x = ((float)x)*skok;
      p_obj->p_vertex[i].z = ((float)y)*skok;
      p_obj->p_vertex[i].y = 0.0f;
      p_obj->p_vertex[i].db = p_obj->p_vertex[i].dg = 
      p_obj->p_vertex[i].dr = 1.0f;
      i++;
    }
  }

  for(y = 0, i = 0; y < rect_y; y++) {
    for(x = 0; x < rect_x; x++) {
       p_obj->p_face[i++] = x + y*num_x;
       p_obj->p_face[i++] = x+1+y*num_x;
       p_obj->p_face[i++] = x+1+(y+1)*num_x;      

       p_obj->p_face[i++] = x + (y+1)*num_x;
       p_obj->p_face[i++] = x + y*num_x;
       p_obj->p_face[i++] = x+1+(y+1)*num_x;
    }
  }

  return(p_obj);
}

void rozloz_zemi(EDIT_OBJEKT **p_src, int ctvercu)
{
  EDIT_OBJEKT *p_obj = *p_src;
  float skok;
  
  if(!p_obj->x_rect || !p_obj->y_rect)
    return;
  if(p_obj->x_rect != p_obj->y_rect != 1)
    return;

  skok = p_obj->p_vertex[1].x - p_obj->p_vertex[0].x;
  skok /= (float)ctvercu;


  p_obj = vyrob_zemi_start(ctvercu, ctvercu, skok, p_obj->p_vertex[0].x, p_obj->p_vertex[0].z);
  zrus_objekt(p_src);
  *p_src = p_obj;  
}

/*
  mirror == 1 - v ose x
         == 2 - v ose y
         == 3 - v ose z
*/
EDIT_OBJEKT * mirror_objekt(EDIT_OBJEKT *p_src, int st, int mx, int my, int mz)
{
 EDIT_OBJEKT *p_dest = vyrob_objekt();
 int          i;
 float        stred = 0;

 *p_dest = *p_src;

 /* Skopiruju vertexy */
 if((p_dest->p_vertex = (OBJ_VERTEX *) mmalloc(sizeof(OBJ_VERTEX)*p_dest->vertexnum)) == NULL) {
   zrus_objekt(&p_dest);
   return(NULL);
 }
 memcpy(p_dest->p_vertex,p_src->p_vertex,sizeof(OBJ_VERTEX)*p_dest->vertexnum);

 if(st) {
   if(mx) {
     for(i = 0; i < p_dest->vertexnum; i++) 
       stred += p_src->p_vertex[i].x;   
     stred /= (float)p_dest->vertexnum;
     for(i = 0; i < p_dest->vertexnum; i++) 
       p_dest->p_vertex[i].x = (stred - p_src->p_vertex[i].x);           
   }
   if(my) {
     for(i = 0; i < p_dest->vertexnum; i++) 
       stred += p_src->p_vertex[i].y;   
     stred /= (float)p_dest->vertexnum;
     for(i = 0; i < p_dest->vertexnum; i++) 
       p_dest->p_vertex[i].y = (stred - p_src->p_vertex[i].y);          
   }
   if(mz) {
     for(i = 0; i < p_dest->vertexnum; i++) 
       stred += p_src->p_vertex[i].z;   
     stred /= (float)p_dest->vertexnum;
     for(i = 0; i < p_dest->vertexnum; i++) 
       p_dest->p_vertex[i].z = (stred - p_src->p_vertex[i].z);           
   }
 } else {   
   for(i = 0; i < p_dest->vertexnum; i++) {
     if(mx) p_dest->p_vertex[i].x = -p_src->p_vertex[i].x;
     if(my) p_dest->p_vertex[i].y = -p_src->p_vertex[i].y;
     if(mz) p_dest->p_vertex[i].z = -p_src->p_vertex[i].z;
   }
 }

 /* Skopiruju facy */
 if((p_dest->p_face = (FACE *) mmalloc(sizeof(FACE)*p_dest->facenum)) == NULL) {
   zrus_objekt(&p_dest);
   return(NULL);
 }
 memcpy(p_dest->p_face,p_src->p_face,sizeof(FACE)*p_dest->facenum);
 
 p_dest->p_line = NULL;
 
 return(p_dest);
}

/*
  mirror == 1 - v ose u
         == 2 - v ose v
*/
void mirror_text(EDIT_OBJEKT *p_src, int t, int mu, int mv)
{
 int   i;
 float *p_fl;

 if(!mu && !mv)
   return;

 for(i = 0; i < p_src->vertexnum; i++) {
   p_fl = &p_src->p_vertex[i].tu1;
   if(mu) p_fl[0+t*2] = - p_fl[0+t*2];
   if(mv) p_fl[1+t*2] = - p_fl[1+t*2];
 }
}

/*
  rotace == 1 - 90
         == 2 - 180
         == 3 - 270
*/
void rotuj_text(EDIT_OBJEKT *p_obj, int t, int uhel)
{
  OBJ_VERTEX *p_vertex;
  int x,y, x_num = p_obj->x_rect+1, y_num = p_obj->y_rect+1;
  float *p_fls,*p_fld;

  if(!p_obj->x_rect || !p_obj->y_rect || !uhel ||(p_obj->x_rect != p_obj->y_rect)) {
    return;
  }
  
  if((p_vertex = (OBJ_VERTEX *) mmalloc(sizeof(OBJ_VERTEX)*p_obj->vertexnum)) == NULL) {
    return;
  }
  memcpy(p_vertex,p_obj->p_vertex,sizeof(OBJ_VERTEX)*p_obj->vertexnum);
  
  if(uhel == 1) { // 90
    for(y = 0; y < x_num; y++) {
      for(x = 0; x < y_num; x++) {
        p_fld = &p_obj->p_vertex[(y_num-x-1)*x_num+y].tu1;
        p_fls = &p_vertex[y*x_num+x].tu1;
        p_fld[0+t*2] = p_fls[0+t*2];
        p_fld[1+t*2] = p_fls[1+t*2];
      }
    }
  } else if(uhel == 2) { // 180
    for(y = 0; y < x_num; y++) {
      for(x = 0; x < y_num; x++) {
        p_fld = &p_obj->p_vertex[(y_num-y-1)*x_num+(x_num-x-1)].tu1;
        p_fls = &p_vertex[y*x_num+x].tu1;
        p_fld[0+t*2] = p_fls[0+t*2];
        p_fld[1+t*2] = p_fls[1+t*2];
      }
    }
  } else  if(uhel == 3) { // 270
    for(y = 0; y < x_num; y++) {
      for(x = 0; x < y_num; x++) {
        p_fld = &p_obj->p_vertex[x*x_num+(y_num-y-1)].tu1;
        p_fls = &p_vertex[y*x_num+x].tu1;
        p_fld[0+t*2] = p_fls[0+t*2];
        p_fld[1+t*2] = p_fls[1+t*2];
      }
    }
  }
  free(p_vertex);
}

void dopln_cary(EDIT_OBJEKT *p_obj)
{
 word   j,k,mf;
 word   *p_line,*p_face;

 mf = p_obj->facenum;

 if(p_obj->p_line != NULL) {
   free(p_obj->p_line);
   p_obj->p_line = NULL;
 }
 
 if((p_line = (word *)mmalloc(sizeof(word)*mf*2)) == NULL)
   chyba("pamet");

 p_face = p_obj->p_face;
 for(j = 0, k = 0; j < p_obj->facenum; j += 3, k += 6) {
    p_line[k+5] = p_line[k]   = p_face[j];
    p_line[k+2] = p_line[k+1] = p_face[j+1];
    p_line[k+4] = p_line[k+3] = p_face[j+2];
 }
 p_obj->p_line = p_line;
 p_obj->linenum = mf*2;
}

void kresli_ctverec_2d(int x1, int y1, int x2, int y2, float r, float g, float b)
{ 
 glColor3f(r,g,b);
 glBegin(GL_LINE_STRIP);
   glVertex2d(x1,y1);
   glVertex2d(x2,y1);
   glVertex2d(x2,y2);
   glVertex2d(x1,y2);
   glVertex2d(x1,y1);
 glEnd();
}

void kresli_ctverec_2d_diff(int x1, int y1, int dx, int dy, float r, float g, float b)
{ 
  kresli_ctverec_2d(x1-dx, y1-dy, x1+dx, y1+dy, r, g, b);
}

void kresli_ctverec_2d_plnej(int x1, int y1, int x2, int y2, float r, float g, float b)
{  
 glColor3f(r,g,b);
 glBegin(GL_QUADS);
   glVertex2d(x1,y1);
   glVertex2d(x1,y2);
   glVertex2d(x2,y2);
   glVertex2d(x2,y1);
 glEnd();
}

void kresli_caru(BOD *p_s, BOD *p_k, dword barva)
{
 glColor4ubv((byte *)&barva);
 
 glBegin(GL_LINES);
   glVertex3f(p_s->x,p_s->y,p_s->z);
   glVertex3f(p_k->x,p_k->y,p_k->z);
 glEnd();
}

void kresli_kurzor_mysi_kriz(int xres, int yres)
{     
 glColor3f(1.0f,1.0f,1.0f); 
 glBegin(GL_LINES);
   glVertex2i(mi.x,0);
   glVertex2i(mi.x,yres);
   glVertex2i(0,mi.y);
   glVertex2i(xres,mi.y);
 glEnd(); 
}


/*
  Stredovy bod + dx,dy,dz
*/
EDIT_OBJEKT * vyrob_krychli(BOD *p_bod, float x, float y, float z, dword barva)
{
 EDIT_OBJEKT *p_obj = vyrob_objekt_komplet(8,12*3);
 int i;

 x /= 2; y /= 2; z /= 2;
 
 p_obj->p_vertex[0].x =-x;
 p_obj->p_vertex[0].y =-y;
 p_obj->p_vertex[0].z =-z;
 p_obj->p_vertex[1].x = x;
 p_obj->p_vertex[1].y =-y;
 p_obj->p_vertex[1].z =-z;
 p_obj->p_vertex[2].x = x;
 p_obj->p_vertex[2].y = y;
 p_obj->p_vertex[2].z =-z;
 p_obj->p_vertex[3].x =-x;
 p_obj->p_vertex[3].y = y;
 p_obj->p_vertex[3].z =-z;
 p_obj->p_vertex[4].x =-x;
 p_obj->p_vertex[4].y =-y;
 p_obj->p_vertex[4].z = z;
 p_obj->p_vertex[5].x = x;
 p_obj->p_vertex[5].y =-y;
 p_obj->p_vertex[5].z = z;
 p_obj->p_vertex[6].x = x;
 p_obj->p_vertex[6].y = y;
 p_obj->p_vertex[6].z = z;
 p_obj->p_vertex[7].x =-x;
 p_obj->p_vertex[7].y = y;
 p_obj->p_vertex[7].z = z;

 for(i = 0; i < 8; i++) {
   p_obj->p_vertex[i].x += p_bod->x;
   p_obj->p_vertex[i].y += p_bod->y;
   p_obj->p_vertex[i].z += p_bod->z;
   rgb_float(barva,&p_obj->p_vertex[i].dr);
 }

 // predni strana
 p_obj->p_face[0] = 0;
 p_obj->p_face[1] = 1;
 p_obj->p_face[2] = 2;
 p_obj->p_face[3] = 2;
 p_obj->p_face[4] = 3;
 p_obj->p_face[5] = 0;

 //zadni strana
 p_obj->p_face[6] = 5;
 p_obj->p_face[7] = 6;
 p_obj->p_face[8] = 7;
 p_obj->p_face[9] = 7;
 p_obj->p_face[10] = 4;
 p_obj->p_face[11] = 5;

 // bok pravej
 p_obj->p_face[12] = 1;
 p_obj->p_face[13] = 5;
 p_obj->p_face[14] = 6;
 p_obj->p_face[15] = 6;
 p_obj->p_face[16] = 2;
 p_obj->p_face[17] = 1;

 // bok levej
 p_obj->p_face[18] = 4;
 p_obj->p_face[19] = 0;
 p_obj->p_face[20] = 3;
 p_obj->p_face[21] = 3;
 p_obj->p_face[22] = 7;
 p_obj->p_face[23] = 4;

 // vrsek
 p_obj->p_face[24] = 3;
 p_obj->p_face[25] = 2;
 p_obj->p_face[26] = 6;
 p_obj->p_face[27] = 6;
 p_obj->p_face[28] = 7;
 p_obj->p_face[29] = 3;

 // spodek
 p_obj->p_face[30] = 4;
 p_obj->p_face[31] = 0;
 p_obj->p_face[32] = 1;
 p_obj->p_face[33] = 1;
 p_obj->p_face[34] = 5;
 p_obj->p_face[35] = 4;

 return(p_obj);
}


EDIT_OBJEKT * vyrob_krychli_dratovou(BOD *p_bod, float x, float y, float z, dword barva)
{
 EDIT_OBJEKT *p_obj = vyrob_objekt_komplet(8,1);
 int i,mf;

 x /= 2; y /= 2; z /= 2;
 
 p_obj->p_vertex[0].x =-x;
 p_obj->p_vertex[0].y =-y;
 p_obj->p_vertex[0].z =-z;
 p_obj->p_vertex[1].x = x;
 p_obj->p_vertex[1].y =-y;
 p_obj->p_vertex[1].z =-z;
 p_obj->p_vertex[2].x = x;
 p_obj->p_vertex[2].y = y;
 p_obj->p_vertex[2].z =-z;
 p_obj->p_vertex[3].x =-x;
 p_obj->p_vertex[3].y = y;
 p_obj->p_vertex[3].z =-z;
 p_obj->p_vertex[4].x =-x;
 p_obj->p_vertex[4].y =-y;
 p_obj->p_vertex[4].z = z;
 p_obj->p_vertex[5].x = x;
 p_obj->p_vertex[5].y =-y;
 p_obj->p_vertex[5].z = z;
 p_obj->p_vertex[6].x = x;
 p_obj->p_vertex[6].y = y;
 p_obj->p_vertex[6].z = z;
 p_obj->p_vertex[7].x =-x;
 p_obj->p_vertex[7].y = y;
 p_obj->p_vertex[7].z = z;

 for(i = 0; i < 8; i++) {
   p_obj->p_vertex[i].x += p_bod->x;
   p_obj->p_vertex[i].y += p_bod->y;
   p_obj->p_vertex[i].z += p_bod->z;
 }

 p_obj->linenum = mf = 12*2;
 if((p_obj->p_line = (word *)mmalloc(sizeof(word)*mf*2)) == NULL)
   chyba("pamet");

 p_obj->p_line[0] = 0;
 p_obj->p_line[1] = 1;
 p_obj->p_line[2] = 1;
 p_obj->p_line[3] = 2;
 p_obj->p_line[4] = 2;
 p_obj->p_line[5] = 3;
 p_obj->p_line[6] = 3;
 p_obj->p_line[7] = 0;
 
 p_obj->p_line[8] = 4;
 p_obj->p_line[9] = 5;
 p_obj->p_line[10] = 5;
 p_obj->p_line[11] = 6;
 p_obj->p_line[12] = 6;
 p_obj->p_line[13] = 7;
 p_obj->p_line[14] = 7;
 p_obj->p_line[15] = 4;

 p_obj->p_line[16] = 0;
 p_obj->p_line[17] = 4;
 p_obj->p_line[18] = 1;
 p_obj->p_line[19] = 5;
 p_obj->p_line[20] = 2;
 p_obj->p_line[21] = 6;
 p_obj->p_line[22] = 3;
 p_obj->p_line[23] = 7;

 nastav_barvu_bodu_obj_dword(p_obj,barva);
 return(p_obj);
}

void krychli_dratovou_recalc(EDIT_OBJEKT *p_obj, BOD *p_bod, float x, float y, float z, dword barva)
{
 int i;

 x /= 2; y /= 2; z /= 2;
 
 p_obj->p_vertex[0].x =-x;
 p_obj->p_vertex[0].y =-y;
 p_obj->p_vertex[0].z =-z;
 p_obj->p_vertex[1].x = x;
 p_obj->p_vertex[1].y =-y;
 p_obj->p_vertex[1].z =-z;
 p_obj->p_vertex[2].x = x;
 p_obj->p_vertex[2].y = y;
 p_obj->p_vertex[2].z =-z;
 p_obj->p_vertex[3].x =-x;
 p_obj->p_vertex[3].y = y;
 p_obj->p_vertex[3].z =-z;
 p_obj->p_vertex[4].x =-x;
 p_obj->p_vertex[4].y =-y;
 p_obj->p_vertex[4].z = z;
 p_obj->p_vertex[5].x = x;
 p_obj->p_vertex[5].y =-y;
 p_obj->p_vertex[5].z = z;
 p_obj->p_vertex[6].x = x;
 p_obj->p_vertex[6].y = y;
 p_obj->p_vertex[6].z = z;
 p_obj->p_vertex[7].x =-x;
 p_obj->p_vertex[7].y = y;
 p_obj->p_vertex[7].z = z;

 for(i = 0; i < 8; i++) {
   p_obj->p_vertex[i].x += p_bod->x;
   p_obj->p_vertex[i].y += p_bod->y;
   p_obj->p_vertex[i].z += p_bod->z;   
 }

 nastav_barvu_bodu_obj_dword(p_obj,barva);
}

EDIT_OBJEKT * umisti_krychli_dratovou(EDIT_OBJEKT *p_obj, BOD *p_bod)
{
 int i; 
 for(i = 0; i < p_obj->vertexnum; i++) {
   p_obj->p_vertex[i].x += p_bod->x;
   p_obj->p_vertex[i].y += p_bod->y;
   p_obj->p_vertex[i].z += p_bod->z;
 }

 return(p_obj);
}

EDIT_OBJEKT * kameru_dratovou_calc(EDIT_OBJEKT *p_obj, BOD *p_p, BOD *p_t, float fov, float roll, dword barva)
{
 BOD   p; 
 float z = vektor_velikost(vektor_sub(p_p,p_t,&p));
 float s = (float)tan(fov)*z/2;

 p_obj->p_vertex[0].x = 0.0f;
 p_obj->p_vertex[0].y = 0.0f;
 p_obj->p_vertex[0].z = 0.0f; 
 rgb_float(barva,&p_obj->p_vertex[0].dr);

 p_obj->p_vertex[1].x = s;
 p_obj->p_vertex[1].y =-s;
 p_obj->p_vertex[1].z = z; 
 rgb_float(barva,&p_obj->p_vertex[1].dr);

 p_obj->p_vertex[2].x = s;
 p_obj->p_vertex[2].y = s;
 p_obj->p_vertex[2].z = z; 
 rgb_float(barva,&p_obj->p_vertex[2].dr);
 
 p_obj->p_vertex[3].x =-s;
 p_obj->p_vertex[3].y = s;
 p_obj->p_vertex[3].z = z; 
 rgb_float(barva,&p_obj->p_vertex[3].dr);
 
 p_obj->p_vertex[4].x =-s;
 p_obj->p_vertex[4].y =-s;
 p_obj->p_vertex[4].z = z; 
 rgb_float(barva,&p_obj->p_vertex[4].dr);

 p_obj->linenum = 16;
 assert(p_obj->p_line);   

 p_obj->p_line[0] = 0;
 p_obj->p_line[1] = 1;
 p_obj->p_line[2] = 0;
 p_obj->p_line[3] = 2;
 p_obj->p_line[4] = 0;
 p_obj->p_line[5] = 3;
 p_obj->p_line[6] = 0;
 p_obj->p_line[7] = 4;
 p_obj->p_line[8] = 4;
 p_obj->p_line[9] = 1;
 p_obj->p_line[10] = 1;
 p_obj->p_line[11] = 2;
 p_obj->p_line[12] = 2;
 p_obj->p_line[13] = 3;
 p_obj->p_line[14] = 3;
 p_obj->p_line[15] = 4;

 nastav_barvu_bodu_obj_dword(p_obj,barva);
 
 return(p_obj);
}

EDIT_OBJEKT * kameru_dratovou_vyrob(BOD *p_p, BOD *p_t, float fov, float roll)
{
 EDIT_OBJEKT *p_obj = vyrob_objekt_komplet(5,0);
 GLMATRIX m;
 if((p_obj->p_line = (word *)mmalloc(sizeof(word)*16)) == NULL)
   chyba("pamet");
 kameru_dratovou_calc(p_obj, p_p, p_t, fov, roll, DDRGB(0,0,1));
 calc_camera_3ds(&m,&p_obj->m,p_p,p_t,roll);
 return(p_obj);
}

int vyrob_kameru_indir(KAMERA *p_kam, char *p_jmeno, BOD *p_p, BOD *p_t, float fov, float roll)
{
  BOD p(0,0,0);

  strcpy(p_kam->jmeno,p_jmeno);

  p_kam->p = *p_p;
  p_kam->t = *p_t;
  p_kam->roll = roll;
  p_kam->fov = fov;
  
  p_kam->p_objekt = kameru_dratovou_vyrob(p_p,p_t,fov,roll);
  
  p_kam->p_pos = vyrob_krychli_dratovou(&p,0.2f,0.2f,0.2f,0xffffffff);
  p.z = vzdal_bodu_bod(&p_kam->p,&p_kam->t);
  p_kam->p_target = vyrob_krychli_dratovou(&p,0.2f,0.2f,0.2f,0xffffffff);
    
  return(TRUE);
}

void kameru_dratovou_reanimuj(KAMERA *p_kam)
{
  GLMATRIX m;
  BOD       p(0,0,0);

  kameru_dratovou_calc(p_kam->p_objekt,&p_kam->p,&p_kam->t, p_kam->fov, 0.0f, 0xff0000);
  krychli_dratovou_recalc(p_kam->p_pos,&p,0.2f,0.2f,0.2f,0xffffffff);
  p.z = vzdal_bodu_bod(&p_kam->p,&p_kam->t);
  krychli_dratovou_recalc(p_kam->p_target,&p,0.2f,0.2f,0.2f,0xffffffff);
  calc_camera_3ds(&m,&p_kam->p_objekt->m,&p_kam->p,&p_kam->t,p_kam->roll);
}

// invertuje poradi plosek
void invertuj_face_objektu(EDIT_OBJEKT *p_obj)
{
 word fcp[3];
 int f;

 for(f = 0; f < p_obj->facenum; f+=3) {
   fcp[0] = p_obj->p_face[f];
   fcp[1] = p_obj->p_face[f+1];
   p_obj->p_face[f] = fcp[1];
   p_obj->p_face[f+1] = fcp[0];
 }
}

void invertuj_face_kontejner(EDIT_KONTEJNER *p_kont)
{
 int o;

 for(o = 0; o < p_kont->max_objektu; o++) {
   if(p_kont->p_obj[o])
     invertuj_face_objektu(p_kont->p_obj[o]);
 }
}

EDIT_KONTEJNER * vyrob_krychli_rozlozenou(BOD *p_bod, float x, float y, float z)
{
 EDIT_KONTEJNER *p_kont = vyrob_kontejner();
 EDIT_OBJEKT *p_obj;
 int i;

 x /= 2; y /= 2; z /= 2;
 
 // predni strana
 p_obj = vyrob_objekt_komplet(4,6);
 strcpy(p_obj->jmeno,"obj1");
 p_obj->x_rect = p_obj->y_rect = 1;
 
 p_obj->p_vertex[0].x =-x;
 p_obj->p_vertex[0].y =-y;
 p_obj->p_vertex[0].z =-z;
 p_obj->p_vertex[0].tu1 = 0;
 p_obj->p_vertex[0].tv1 = 0;
 p_obj->p_vertex[0].tu2 = 0;
 p_obj->p_vertex[0].tv2 = 0;
 p_obj->p_vertex[0].tu3 = 0;
 p_obj->p_vertex[0].tv3 = 0;
 p_obj->p_vertex[0].tu4 = 0;
 p_obj->p_vertex[0].tv4 = 0;

 p_obj->p_vertex[1].x = x;
 p_obj->p_vertex[1].y =-y;
 p_obj->p_vertex[1].z =-z;
 p_obj->p_vertex[1].tu1 = 1;
 p_obj->p_vertex[1].tv1 = 0;
 p_obj->p_vertex[1].tu2 = 1;
 p_obj->p_vertex[1].tv2 = 0;
 p_obj->p_vertex[1].tu3 = 1;
 p_obj->p_vertex[1].tv3 = 0;
 p_obj->p_vertex[1].tu4 = 1;
 p_obj->p_vertex[1].tv4 = 0;
  
 p_obj->p_vertex[2].x =-x;
 p_obj->p_vertex[2].y = y;
 p_obj->p_vertex[2].z =-z;
 p_obj->p_vertex[2].tu1=0;
 p_obj->p_vertex[2].tv1=1;
 p_obj->p_vertex[2].tu2=0;
 p_obj->p_vertex[2].tv2=1;
 p_obj->p_vertex[2].tu3=0;
 p_obj->p_vertex[2].tv3=1;
 p_obj->p_vertex[2].tu4=0;
 p_obj->p_vertex[2].tv4=1;

 p_obj->p_vertex[3].x = x;
 p_obj->p_vertex[3].y = y;
 p_obj->p_vertex[3].z =-z;
 p_obj->p_vertex[3].tu1 = 1;
 p_obj->p_vertex[3].tv1 = 1;
 p_obj->p_vertex[3].tu2 = 1;
 p_obj->p_vertex[3].tv2 = 1;
 p_obj->p_vertex[3].tu3 = 1;
 p_obj->p_vertex[3].tv3 = 1;
 p_obj->p_vertex[3].tu4 = 1;
 p_obj->p_vertex[3].tv4 = 1;

 p_obj->p_face[0] = 0;
 p_obj->p_face[1] = 1;
 p_obj->p_face[2] = 3;
 p_obj->p_face[3] = 3;
 p_obj->p_face[4] = 2;
 p_obj->p_face[5] = 0;

 for(i = 0; i < 4; i++) {
   p_obj->p_vertex[i].x += p_bod->x;
   p_obj->p_vertex[i].y += p_bod->y;
   p_obj->p_vertex[i].z += p_bod->z;
 }
 p_kont->p_obj[0] = p_obj;
 

 // Bocni strana
 p_obj = vyrob_objekt_komplet(4,6);
 strcpy(p_obj->jmeno,"obj2");
 p_obj->x_rect = p_obj->y_rect = 1;

 p_obj->p_vertex[0].x = x;
 p_obj->p_vertex[0].y =-y;
 p_obj->p_vertex[0].z =-z;
 p_obj->p_vertex[0].tu1 = 0;
 p_obj->p_vertex[0].tv1 = 0;
 p_obj->p_vertex[0].tu2 = 0;
 p_obj->p_vertex[0].tv2 = 0;
 p_obj->p_vertex[0].tu3 = 0;
 p_obj->p_vertex[0].tv3 = 0;
 p_obj->p_vertex[0].tu4 = 0;
 p_obj->p_vertex[0].tv4 = 0;
 
 p_obj->p_vertex[1].x = x;
 p_obj->p_vertex[1].y =-y;
 p_obj->p_vertex[1].z = z;
 p_obj->p_vertex[1].tu1 = 1;
 p_obj->p_vertex[1].tv1 = 0;
 p_obj->p_vertex[1].tu2 = 1;
 p_obj->p_vertex[1].tv2 = 0;
 p_obj->p_vertex[1].tu3 = 1;
 p_obj->p_vertex[1].tv3 = 0;
 p_obj->p_vertex[1].tu4 = 1;
 p_obj->p_vertex[1].tv4 = 0;
 
 p_obj->p_vertex[3].x = x;
 p_obj->p_vertex[3].y = y;
 p_obj->p_vertex[3].z = z;
 p_obj->p_vertex[3].tu1 = 1;
 p_obj->p_vertex[3].tv1 = 1;
 p_obj->p_vertex[3].tu2 = 1;
 p_obj->p_vertex[3].tv2 = 1;
 p_obj->p_vertex[3].tu3 = 1;
 p_obj->p_vertex[3].tv3 = 1;
 p_obj->p_vertex[3].tu4 = 1;
 p_obj->p_vertex[3].tv4 = 1;
 
 p_obj->p_vertex[2].x = x;
 p_obj->p_vertex[2].y = y;
 p_obj->p_vertex[2].z =-z;
 p_obj->p_vertex[2].tu1=0;
 p_obj->p_vertex[2].tv1=1;
 p_obj->p_vertex[2].tu2=0;
 p_obj->p_vertex[2].tv2=1;
 p_obj->p_vertex[2].tu3=0;
 p_obj->p_vertex[2].tv3=1;
 p_obj->p_vertex[2].tu4=0;
 p_obj->p_vertex[2].tv4=1;

 p_obj->p_face[0] = 0;
 p_obj->p_face[1] = 1;
 p_obj->p_face[2] = 3;
 p_obj->p_face[3] = 3;
 p_obj->p_face[4] = 2;
 p_obj->p_face[5] = 0;

 for(i = 0; i < 4; i++) {
   p_obj->p_vertex[i].x += p_bod->x;
   p_obj->p_vertex[i].y += p_bod->y;
   p_obj->p_vertex[i].z += p_bod->z;
 }
 p_kont->p_obj[1] = p_obj;
 

 // Zadni strana
 p_obj = vyrob_objekt_komplet(4,6);
 strcpy(p_obj->jmeno,"obj3");
 p_obj->x_rect = p_obj->y_rect = 1;

 p_obj->p_vertex[2].x =-x;
 p_obj->p_vertex[2].y =-y;
 p_obj->p_vertex[2].z = z;
 p_obj->p_vertex[2].tu1 = 1;
 p_obj->p_vertex[2].tv1 = 0;
 p_obj->p_vertex[2].tu2 = 1;
 p_obj->p_vertex[2].tv2 = 0;
 p_obj->p_vertex[2].tu3 = 1;
 p_obj->p_vertex[2].tv3 = 0;
 p_obj->p_vertex[2].tu4 = 1;
 p_obj->p_vertex[2].tv4 = 0;
 
 p_obj->p_vertex[3].x = x;
 p_obj->p_vertex[3].y =-y;
 p_obj->p_vertex[3].z = z;
 p_obj->p_vertex[3].tu1 = 0;
 p_obj->p_vertex[3].tv1 = 0;
 p_obj->p_vertex[3].tu2 = 0;
 p_obj->p_vertex[3].tv2 = 0;
 p_obj->p_vertex[3].tu3 = 0;
 p_obj->p_vertex[3].tv3 = 0;
 p_obj->p_vertex[3].tu4 = 0;
 p_obj->p_vertex[3].tv4 = 0;
 
 p_obj->p_vertex[1].x = x;
 p_obj->p_vertex[1].y = y;
 p_obj->p_vertex[1].z = z;
 p_obj->p_vertex[1].tu1 = 0;
 p_obj->p_vertex[1].tv1 = 1;
 p_obj->p_vertex[1].tu2 = 0;
 p_obj->p_vertex[1].tv2 = 1;
 p_obj->p_vertex[1].tu3 = 0;
 p_obj->p_vertex[1].tv3 = 1;
 p_obj->p_vertex[1].tu4 = 0;
 p_obj->p_vertex[1].tv4 = 1;

 p_obj->p_vertex[0].x =-x;
 p_obj->p_vertex[0].y = y;
 p_obj->p_vertex[0].z = z;
 p_obj->p_vertex[0].tu1 = 1;
 p_obj->p_vertex[0].tv1 = 1;
 p_obj->p_vertex[0].tu2 = 1;
 p_obj->p_vertex[0].tv2 = 1;
 p_obj->p_vertex[0].tu3 = 1;
 p_obj->p_vertex[0].tv3 = 1;
 p_obj->p_vertex[0].tu4 = 1;
 p_obj->p_vertex[0].tv4 = 1;

 p_obj->p_face[0] = 2;
 p_obj->p_face[1] = 0;
 p_obj->p_face[2] = 3;
 p_obj->p_face[3] = 3;
 p_obj->p_face[4] = 0;
 p_obj->p_face[5] = 1;

 for(i = 0; i < 4; i++) {
   p_obj->p_vertex[i].x += p_bod->x;
   p_obj->p_vertex[i].y += p_bod->y;
   p_obj->p_vertex[i].z += p_bod->z;
 }
 p_kont->p_obj[2] = p_obj;
 

 // Dalsi bocni strana
 p_obj = vyrob_objekt_komplet(4,6);
 strcpy(p_obj->jmeno,"obj4");
 p_obj->x_rect = p_obj->y_rect = 1;

 p_obj->p_vertex[0].x = -x;
 p_obj->p_vertex[0].y = -y;
 p_obj->p_vertex[0].z =  z;
 p_obj->p_vertex[0].tu1 = 0;
 p_obj->p_vertex[0].tv1 = 0;
 p_obj->p_vertex[0].tu2 = 0;
 p_obj->p_vertex[0].tv2 = 0;
 p_obj->p_vertex[0].tu3 = 0;
 p_obj->p_vertex[0].tv3 = 0;
 p_obj->p_vertex[0].tu4 = 0;
 p_obj->p_vertex[0].tv4 = 0;
 
 p_obj->p_vertex[1].x = -x;
 p_obj->p_vertex[1].y =  y;
 p_obj->p_vertex[1].z =  z;
 p_obj->p_vertex[1].tu1 = 0;
 p_obj->p_vertex[1].tv1 = 1;
 p_obj->p_vertex[1].tu2 = 0;
 p_obj->p_vertex[1].tv2 = 1;
 p_obj->p_vertex[1].tu3 = 0;
 p_obj->p_vertex[1].tv3 = 1;
 p_obj->p_vertex[1].tu4 = 0;
 p_obj->p_vertex[1].tv4 = 1;
 
 p_obj->p_vertex[3].x = -x;
 p_obj->p_vertex[3].y =  y;
 p_obj->p_vertex[3].z = -z;
 p_obj->p_vertex[3].tu1 = 1;
 p_obj->p_vertex[3].tv1 = 1;
 p_obj->p_vertex[3].tu2 = 1;
 p_obj->p_vertex[3].tv2 = 1;
 p_obj->p_vertex[3].tu3 = 1;
 p_obj->p_vertex[3].tv3 = 1;
 p_obj->p_vertex[3].tu4 = 1;
 p_obj->p_vertex[3].tv4 = 1;
 
 p_obj->p_vertex[2].x = -x;
 p_obj->p_vertex[2].y = -y;
 p_obj->p_vertex[2].z = -z;
 p_obj->p_vertex[2].tu1 = 1;
 p_obj->p_vertex[2].tv1 = 0;
 p_obj->p_vertex[2].tu2 = 1;
 p_obj->p_vertex[2].tv2 = 0;
 p_obj->p_vertex[2].tu3 = 1;
 p_obj->p_vertex[2].tv3 = 0;
 p_obj->p_vertex[2].tu4 = 1;
 p_obj->p_vertex[2].tv4 = 0;

 p_obj->p_face[0] = 1;
 p_obj->p_face[1] = 0;
 p_obj->p_face[2] = 3;
 p_obj->p_face[3] = 3;
 p_obj->p_face[4] = 0;
 p_obj->p_face[5] = 2;

 for(i = 0; i < 4; i++) {
   p_obj->p_vertex[i].x += p_bod->x;
   p_obj->p_vertex[i].y += p_bod->y;
   p_obj->p_vertex[i].z += p_bod->z;
 }
 p_kont->p_obj[3] = p_obj;
 

 // vrchni strana
 p_obj = vyrob_objekt_komplet(4,6);
 strcpy(p_obj->jmeno,"obj5");
 p_obj->x_rect = p_obj->y_rect = 1;
 
 p_obj->p_vertex[0].x =-x;
 p_obj->p_vertex[0].y = y;
 p_obj->p_vertex[0].z =-z;
 p_obj->p_vertex[0].tu1 = 0;
 p_obj->p_vertex[0].tv1 = 0;
 p_obj->p_vertex[0].tu2 = 0;
 p_obj->p_vertex[0].tv2 = 0;
 p_obj->p_vertex[0].tu3 = 0;
 p_obj->p_vertex[0].tv3 = 0;
 p_obj->p_vertex[0].tu4 = 0;
 p_obj->p_vertex[0].tv4 = 0;
 
 p_obj->p_vertex[1].x = x;
 p_obj->p_vertex[1].y = y;
 p_obj->p_vertex[1].z =-z;
 p_obj->p_vertex[1].tu1 = 1;
 p_obj->p_vertex[1].tv1 = 0;
 p_obj->p_vertex[1].tu2 = 1;
 p_obj->p_vertex[1].tv2 = 0;
 p_obj->p_vertex[1].tu3 = 1;
 p_obj->p_vertex[1].tv3 = 0;
 p_obj->p_vertex[1].tu4 = 1;
 p_obj->p_vertex[1].tv4 = 0;
 
 p_obj->p_vertex[3].x = x;
 p_obj->p_vertex[3].y = y;
 p_obj->p_vertex[3].z = z;
 p_obj->p_vertex[3].tu1 = 1;
 p_obj->p_vertex[3].tv1 = 1;
 p_obj->p_vertex[3].tu2 = 1;
 p_obj->p_vertex[3].tv2 = 1;
 p_obj->p_vertex[3].tu3 = 1;
 p_obj->p_vertex[3].tv3 = 1;
 p_obj->p_vertex[3].tu4 = 1;
 p_obj->p_vertex[3].tv4 = 1;
 
 p_obj->p_vertex[2].x = -x;
 p_obj->p_vertex[2].y =  y;
 p_obj->p_vertex[2].z =  z;
 p_obj->p_vertex[2].tu1=0;
 p_obj->p_vertex[2].tv1=1;
 p_obj->p_vertex[2].tu2=0;
 p_obj->p_vertex[2].tv2=1;
 p_obj->p_vertex[2].tu3=0;
 p_obj->p_vertex[2].tv3=1;
 p_obj->p_vertex[2].tu4=0;
 p_obj->p_vertex[2].tv4=1;

 p_obj->p_face[0] = 0;
 p_obj->p_face[1] = 1;
 p_obj->p_face[2] = 3;
 p_obj->p_face[3] = 3;
 p_obj->p_face[4] = 2;
 p_obj->p_face[5] = 0;

 for(i = 0; i < 4; i++) {
   p_obj->p_vertex[i].x += p_bod->x;
   p_obj->p_vertex[i].y += p_bod->y;
   p_obj->p_vertex[i].z += p_bod->z;
 }
 p_kont->p_obj[4] = p_obj;
 

 // dolni strana
 
 p_obj = vyrob_objekt_komplet(4,6);
 strcpy(p_obj->jmeno,"obj6");
 p_obj->x_rect = p_obj->y_rect = 1;
 
 p_obj->p_vertex[0].x = -x;
 p_obj->p_vertex[0].y = -y;
 p_obj->p_vertex[0].z = -z;
 p_obj->p_vertex[0].tu1 = 0;
 p_obj->p_vertex[0].tv1 = 0;
 p_obj->p_vertex[0].tu2 = 0;
 p_obj->p_vertex[0].tv2 = 0;
 p_obj->p_vertex[0].tu3 = 0;
 p_obj->p_vertex[0].tv3 = 0;
 p_obj->p_vertex[0].tu4 = 0;
 p_obj->p_vertex[0].tv4 = 0;
 
 p_obj->p_vertex[1].x =  x;
 p_obj->p_vertex[1].y = -y;
 p_obj->p_vertex[1].z = -z;
 p_obj->p_vertex[1].tu1 = 1;
 p_obj->p_vertex[1].tv1 = 0;
 p_obj->p_vertex[1].tu2 = 1;
 p_obj->p_vertex[1].tv2 = 0;
 p_obj->p_vertex[1].tu3 = 1;
 p_obj->p_vertex[1].tv3 = 0;
 p_obj->p_vertex[1].tu4 = 1;
 p_obj->p_vertex[1].tv4 = 0;
 
 p_obj->p_vertex[3].x =  x;
 p_obj->p_vertex[3].y = -y;
 p_obj->p_vertex[3].z =  z;
 p_obj->p_vertex[3].tu1 = 1;
 p_obj->p_vertex[3].tv1 = 1;
 p_obj->p_vertex[3].tu2 = 1;
 p_obj->p_vertex[3].tv2 = 1;
 p_obj->p_vertex[3].tu3 = 1;
 p_obj->p_vertex[3].tv3 = 1;
 p_obj->p_vertex[3].tu4 = 1;
 p_obj->p_vertex[3].tv4 = 1;
 
 p_obj->p_vertex[2].x = -x;
 p_obj->p_vertex[2].y = -y;
 p_obj->p_vertex[2].z =  z;
 p_obj->p_vertex[2].tu1=0;
 p_obj->p_vertex[2].tv1=1;
 p_obj->p_vertex[2].tu2=0;
 p_obj->p_vertex[2].tv2=1;
 p_obj->p_vertex[2].tu3=0;
 p_obj->p_vertex[2].tv3=1;
 p_obj->p_vertex[2].tu4=0;
 p_obj->p_vertex[2].tv4=1;

 p_obj->p_face[0] = 1;
 p_obj->p_face[1] = 0;
 p_obj->p_face[2] = 3;
 p_obj->p_face[3] = 3;
 p_obj->p_face[4] = 0;
 p_obj->p_face[5] = 2;

 for(i = 0; i < 4; i++) {
   p_obj->p_vertex[i].x += p_bod->x;
   p_obj->p_vertex[i].y += p_bod->y;
   p_obj->p_vertex[i].z += p_bod->z;
 }
 p_kont->p_obj[5] = p_obj;
 

 updatuj_kontejner_statistika(p_kont,TRUE);
 nastav_barvu_bodu_kont_dword(p_kont,0xffffffff,COLOR_SET);
 kont_norm_vect(p_kont);
 return(p_kont);
}

/*
  Stredovy bod + dx,dy,dz
*/
void pridej_krychli(EDIT_OBJEKT **p_src, BOD *p_bod, float x, float y, float z)
{
 EDIT_OBJEKT *p_obj = vyrob_krychli(p_bod,x,y,z,0xffffffff);
 EDIT_OBJEKT *p_obj2;

 p_obj2 = slep_objekty(*p_src,p_obj);
 zrus_objekt(p_src);
 zrus_objekt(&p_obj);
 *p_src = p_obj2;
}

/*
  Vyrobi billboard na BOD + dx,dy
*/
EDIT_OBJEKT * vyrob_billboard(BOD *p_bod, float x, float y)
{
 EDIT_OBJEKT *p_obj = vyrob_objekt_komplet(4,6);
 int i; 

 p_obj->p_vertex[0].x =-x;
 p_obj->p_vertex[0].y =-y;
 p_obj->p_vertex[0].z = 0;
 p_obj->p_vertex[1].x = x;
 p_obj->p_vertex[1].y =-y;
 p_obj->p_vertex[1].z = 0;
 p_obj->p_vertex[2].x = x;
 p_obj->p_vertex[2].y = y;
 p_obj->p_vertex[2].z = 0;
 p_obj->p_vertex[3].x =-x;
 p_obj->p_vertex[3].y = y;
 p_obj->p_vertex[3].z = 0;

 p_obj->p_vertex[0].tu1 = 0;
 p_obj->p_vertex[0].tv1 = 0;
 p_obj->p_vertex[1].tu1 = 1;
 p_obj->p_vertex[1].tv1 = 0; 
 p_obj->p_vertex[2].tu1 = 1;
 p_obj->p_vertex[2].tv1 = 1;
 p_obj->p_vertex[3].tu1 = 0;
 p_obj->p_vertex[3].tv1 = 1;

 for(i = 0; i < 4; i++) {
   p_obj->p_vertex[i].x += p_bod->x;
   p_obj->p_vertex[i].y += p_bod->y;
   p_obj->p_vertex[i].z += p_bod->z;
 }

 // predni strana
 p_obj->p_face[0] = 0;
 p_obj->p_face[1] = 1;
 p_obj->p_face[2] = 2;
 p_obj->p_face[3] = 2;
 p_obj->p_face[4] = 3;
 p_obj->p_face[5] = 0;

 return(p_obj);
}

// Spocita obalku objektu
void objekt_obalka(EDIT_OBJEKT *p_obj, GLMATRIX *p_m, BOD *p_min, BOD *p_max)
{
  MUJ_BOD *p_vert;
  BOD a;
  int v;

  p_vert = p_obj->p_vertex;
  assert(p_vert && p_obj->vertexnum);

  mujbod2bod(&a,p_obj->p_vertex);
  if(p_m)
     transformuj_bod_bod_matici(&a,p_m);
  *p_min = *p_max = a;

  for(v = 1; v < p_obj->vertexnum; v++) {
    mujbod2bod(&a,p_obj->p_vertex+v);
    if(p_m)
      transformuj_bod_bod_matici(&a,p_m);
    
    if(a.x < p_min->x) p_min->x = a.x;
    if(a.y < p_min->y) p_min->y = a.y;
    if(a.z < p_min->z) p_min->z = a.z;
    
    if(a.x > p_max->x) p_max->x = a.x;
    if(a.y > p_max->y) p_max->y = a.y;
    if(a.z > p_max->z) p_max->z = a.z;
  }
}

void poly_obalka(EDIT_MESH_POLY *p_poly, GLMATRIX *p_mat, BOD *p_min, BOD *p_max)
{ 
  BOD max(-FLT_MAX,-FLT_MAX,-FLT_MAX),
      min(FLT_MAX, FLT_MAX, FLT_MAX), a;
  int f;
   
  for(f = 0; f < p_poly->facenum; f++) {
    a.x = p_poly->p_koord[f].x;
    a.y = p_poly->p_koord[f].y;
    a.z = p_poly->p_koord[f].z;

    if(p_mat)
      transformuj_bod_bod_matici(&a,p_mat);
    
    if(a.x < min.x) min.x = a.x;
    if(a.y < min.y) min.y = a.y;
    if(a.z < min.z) min.z = a.z;
    
    if(a.x > max.x) max.x = a.x;
    if(a.y > max.y) max.y = a.y;
    if(a.z > max.z) max.z = a.z;
  }

  *p_max = max;
  *p_min = min;  
}

// spocita obalku kontejneru - spocita z matic objektu
void kontejner_obalka(EDIT_KONTEJNER *p_kont)
{
  EDIT_OBJEKT *p_obj;
  OBB *p_obb = &p_kont->obb;
  BOD   max(-FLT_MAX,-FLT_MAX,-FLT_MAX),
        min( FLT_MAX, FLT_MAX, FLT_MAX);
  int   k;
/*
  for(k = 0; k < MAX_KONT_OBJEKTU; k++) {
    p_obj = p_kont->p_obj[k];
    if(p_obj) {
      kd_min_max_bod(&p_obj->obb.aabb_min,&min,&max);
      kd_min_max_bod(&p_obj->obb.aabb_max,&min,&max);
    }
  }

  vektor_set_all(p_obb->obb,  1,0,0);
  vektor_set_all(p_obb->obb+1,0,1,0);
  vektor_set_all(p_obb->obb+2,0,0,1);
  
  kd_stred_bunky(&min,&max,&p_kont->obb.obb_stred);
  kd_len_bunky(&min,&max,&p_kont->obb.obb_len); 
*/
}

void kontejner_obalka_aabb(EDIT_KONTEJNER *p_kont, BOD *p_min, BOD *p_max)
{
  EDIT_OBJEKT *p_obj;
  int   k;

  vektor_set(p_min, FLT_MAX);
  vektor_set(p_max,-FLT_MAX);
/*
  for(k = 0; k < MAX_KONT_OBJEKTU; k++) {
    p_obj = p_kont->p_obj[k];
    if(p_obj) {
      kd_min_max_bod(&p_obj->obb.aabb_min,p_min,p_max);
      kd_min_max_bod(&p_obj->obb.aabb_max,p_min,p_max);
    }
  }
*/
}

// cte velikost objektu v bytech
int cti_velikost_objektu(EDIT_OBJEKT *p_obj)
{
 if(p_obj)
   return(sizeof(EDIT_OBJEKT)+p_obj->vertexnum*sizeof(p_obj->p_vertex[0])+p_obj->facenum*sizeof(p_obj->p_face[0])+p_obj->linenum*sizeof(p_obj->p_line[0]));
 else
   return(0);
}

void cti_velikost_kontejneru(EDIT_KONTEJNER *p_kont, int *p_size, int *p_objektu)
{
  int o;
  int size;
  int objektu = 0;

  size = sizeof(EDIT_KONTEJNER);

  for(o = 0; o < p_kont->max_objektu; o++) {
    if(p_kont->p_obj[o]) {
      size += cti_velikost_objektu(p_kont->p_obj[o]);
      objektu++;
    }
  }
  *p_size += size;
  *p_objektu += objektu;
}

int intersect_ray_koule(BOD *p_stred, float radius, BOD *p_orig, BOD *p_dir)
{
  BOD q = *p_dir;
  BOD p(p_orig->x-p_stred->x,p_orig->y-p_stred->y,p_orig->z-p_stred->z);
  float a,b2,c;
  float D4,sqD4,t2;

  a = q.x*q.x+q.y*q.y+q.z*q.z;
  b2 = (skal_soucin(q,p))/2;
  c =  (q.x - p_stred->x)*(q.x - p_stred->x)
      +(q.y - p_stred->y)*(q.y - p_stred->y)
      +(q.z - p_stred->z)*(q.z - p_stred->z)
      - radius*radius;

  // att + bt + c = 0
  D4 = b2*b2 - a*c;

  if(D4 < 0)
    return(FALSE);

  sqD4 = (float)sqrt(D4);
  t2 = (sqD4 - b2)/a;

  if(t2 < 0)
    return(FALSE);
  else
    return(TRUE);
}


//  FACE_SOUS    *p_fsous;

static int _num_pass(word *p_f1, word *p_f2)
{
  int i,j;
  int hit = 0;
  for(i = 0; i < 3; i++) {
    for(j = 0; j < 3; j++) {
      if(p_f1[i] == p_f2[j])
        hit++;
    }
  }
  return(hit);
}

// importovat z 3ds modelama !!!!
// toto je moje berlicka
void obj_vyrob_list_sousednosti(EDIT_OBJEKT *p_obj)
{  
  int f,fi;
  int hit;

  if(p_obj->p_fsous) {
    free(p_obj->p_fsous);
  }
/*
  p_obj->p_fsous = mmalloc(sizeof(p_obj->p_fsous[0])*p_obj->facenum);
  memset(p_obj->p_fsous,K_CHYBA,sizeof(p_obj->p_fsous[0])*p_obj->facenum);
  
  for(f = 0; f < p_obj->facenum; f+=3) {
    hit = 0;
    for(fi = 0; fi < p_obj->facenum; fi+=3) {
      if(f == fi)
        continue;
      // face maji 2 spolecne body
      if(_num_pass(p_obj->p_face+f, p_obj->p_face+fi) == 2) {
        assert(hit < 3);
        p_obj->p_fsous[f+hit] = fi;
        hit++;
      }
    }
  }
*/
}

static int _num_pass_vertex(OBJ_VERTEX **p_f1, OBJ_VERTEX **p_f2)
{
  int i,j;
  int hit = 0;
  for(i = 0; i < 3; i++) {
    for(j = 0; j < 3; j++) {
      if(stejny_vertex_point((BOD *)(p_f1[i]),(BOD *)(p_f2[j])))
        hit++;
    }
  }
  return(hit);
}
 
void obj_vyrob_list_sousednosti_full(EDIT_OBJEKT *p_obj)
{  
  OBJ_VERTEX *p_face1[3];
  OBJ_VERTEX *p_face2[3];
  int f,fi;
  int hit;

  if(p_obj->p_fsous) {
    free(p_obj->p_fsous);
  }
/*
  p_obj->p_fsous = mmalloc(sizeof(p_obj->p_fsous[0])*p_obj->facenum);
  memset(p_obj->p_fsous,K_CHYBA,sizeof(p_obj->p_fsous[0])*p_obj->facenum);
  
  for(f = 0; f < p_obj->facenum; f+=3) {
    hit = 0;
    for(fi = 0; fi < p_obj->facenum; fi+=3) {
      if(f == fi)
        continue;
      // face maji 2 spolecne body
      p_face1[0] = p_obj->p_vertex+(p_obj->p_face[f]);
      p_face1[1] = p_obj->p_vertex+(p_obj->p_face[f+1]);
      p_face1[2] = p_obj->p_vertex+(p_obj->p_face[f+2]);
      
      p_face2[0] = p_obj->p_vertex+(p_obj->p_face[fi]);
      p_face2[1] = p_obj->p_vertex+(p_obj->p_face[fi+1]);
      p_face2[2] = p_obj->p_vertex+(p_obj->p_face[fi+2]);
      
      if(_num_pass_vertex(p_face1, p_face2) == 2) {
        assert(hit < 3);
        p_obj->p_fsous[f+hit++] = fi;
      }
    }
  }
  */
}

void scale_normal_roh(MUJ_BOD *p_vertex, FACE *p_face, int v1, int v2, int v3, BOD *p_normal)
{
  BOD   vektor1,vektor2,p1,p2;
  float uhel; // 180 stupnu = 1.0f

  vektor_sub(mujbod2bod(&p1,p_vertex+p_face[v2]),
             mujbod2bod(&p2,p_vertex+p_face[v1]),
             &vektor1);
  vektor_norm(&vektor1);
  vektor_sub(mujbod2bod(&p1,p_vertex+p_face[v3]),
             mujbod2bod(&p2,p_vertex+p_face[v1]),
             &vektor2);
  vektor_norm(&vektor2);

  uhel = acosf(vektor_uhel(&vektor1,&vektor2));
  if(finite(uhel) && !isnan(uhel))
    vektor_scale(p_normal,uhel/PI);
  else
    vektor_set(p_normal,0.0f);
}

typedef struct _NORM_INDICIE { // popisuje vertex

 BOD   *p_vert;
 BOD    norm;
 int    vertex;

} NORM_INDICIE;

typedef struct _FACE_INDICIE { // popisuje face

  int   vertex;
  BOD   norm;

} FACE_INDICIE;

typedef struct _NORM_INT_PROC {

  int           ind_akt; // Pole vertexu
  int           ind_max;
  NORM_INDICIE *p_ind;

  int           fn_akt;  // Pole facu
  int           fn_max;
  FACE_INDICIE *p_fn;

} NORM_INT_PROC;

void ind_init(NORM_INT_PROC *p_prc, int vert, int facu)
{
  memset(p_prc,0,sizeof(p_prc[0]));
/*
  p_prc->p_ind = mmalloc(sizeof(p_prc->p_ind[0])*vert);
  p_prc->ind_max = vert;

  p_prc->p_fn = mmalloc(sizeof(p_prc->p_fn[0])*facu);
  p_prc->fn_max = facu;
*/
}

void ind_smaz(NORM_INT_PROC *p_prc)
{
/*
  null_free(&p_prc->p_ind);
  null_free(&p_prc->p_fn);
  memset(p_prc,0,sizeof(p_prc[0]));
*/
}

BOD * ind_calc_fnorm(EDIT_OBJEKT *p_obj)
{
  BOD *p_fnorm;
  int i;
  p_fnorm = (BOD *)mmalloc(sizeof(p_fnorm[0])*p_obj->facenum);
  for(i = 0; i < p_obj->facenum; i+=3) {
    vektor_norm(calc_face_normal((BOD *)(p_obj->p_vertex+p_obj->p_face[i]),
                                 (BOD *)(p_obj->p_vertex+p_obj->p_face[i+1]),
                                 (BOD *)(p_obj->p_vertex+p_obj->p_face[i+2]),
                                 p_fnorm+i/3));
  }
  return(p_fnorm);
}

int ind_add_obj(NORM_INT_PROC *p_prc, EDIT_OBJEKT *p_obj)
{
  int i,vnum = p_obj->vertexnum,
        fnum = p_obj->facenum;
  int   handle;
  NORM_INDICIE *p_ind;
  FACE_INDICIE *p_fn;
  BOD *p_fnorm;

  if(p_prc->ind_akt+vnum > p_prc->ind_max)
    return(K_CHYBA);
  if(p_prc->fn_akt+fnum > p_prc->fn_max)
    return(K_CHYBA);

  p_fnorm = ind_calc_fnorm(p_obj);

  handle = p_prc->ind_akt;
  p_ind = p_prc->p_ind+handle;
  p_prc->ind_akt += vnum;
  for(i = 0; i < vnum; i++, p_ind++) {
    p_ind->p_vert = (BOD *)(p_obj->p_vertex+i);
    p_ind->vertex = handle+i;    
  }

  p_fn = p_prc->p_fn+p_prc->fn_akt;  
  p_prc->fn_akt += fnum;
  for(i = 0; i < fnum; i += 3) {
    p_fn->norm = p_fnorm[i/3];
    scale_normal_roh(p_obj->p_vertex, p_obj->p_face+i, 0, 1, 2, &p_fn->norm);
    p_fn->vertex = handle+p_obj->p_face[i];
    p_fn++;

    p_fn->norm = p_fnorm[i/3];
    scale_normal_roh(p_obj->p_vertex, p_obj->p_face+i, 1, 0, 2, &p_fn->norm);
    p_fn->vertex = handle+p_obj->p_face[i+1];
    p_fn++;

    p_fn->norm = p_fnorm[i/3];
    scale_normal_roh(p_obj->p_vertex, p_obj->p_face+i, 2, 0, 1, &p_fn->norm);
    p_fn->vertex = handle+p_obj->p_face[i+2];
    p_fn++;
  }
  
  free(p_fnorm);

  return(handle);
}

// Kopiruje normaly zpet do objektu
int ind_vypln_obj(NORM_INT_PROC *p_prc, EDIT_OBJEKT *p_obj, int handle)
{
  NORM_INDICIE *p_ind;
  int i,vnum = p_obj->vertexnum;
  
  if(handle + vnum > p_prc->ind_akt)
    return(K_CHYBA);  

  p_ind = p_prc->p_ind+handle;  
  for(i = 0; i < vnum; i++, p_ind++) {
    norm2mujbod(p_obj->p_vertex+i,&p_ind->norm);
  }
  return(handle);
}

inline int porovnej_vertexy(BOD *p_v1, BOD *p_v2)
{
  if(p_v1->x > p_v2->x) {
    return(1);
  } else if(p_v1->x < p_v2->x) {
    return(-1);

  } else if(p_v1->y > p_v2->y) {
    return(1);
  } else if(p_v1->y < p_v2->y) {
    return(-1);

  } else if(p_v1->z > p_v2->z) {
    return(1);
  } else if(p_v1->z < p_v2->z) {
    return(-1);
  
  } else {
    return(0);
  }
}

int ind_sort_rec(const void *p_m1, const void *p_m2)
{
  const NORM_INDICIE *p_i1 = (NORM_INDICIE *)p_m1;
  const NORM_INDICIE *p_i2 = (NORM_INDICIE *)p_m2;
  return(porovnej_vertexy(p_i1->p_vert, p_i2->p_vert));
}

int ind_sort_rec2(const void *p_m1, const void *p_m2)
{
  const FACE_INDICIE *p_i1 = (FACE_INDICIE *)p_m1;
  const FACE_INDICIE *p_i2 = (FACE_INDICIE *)p_m2;
  return(p_i1->vertex-p_i2->vertex);
}

void ind_sort(NORM_INT_PROC *p_prc, int brutal)
{
  if(brutal)
    qsort(p_prc->p_ind,p_prc->ind_akt,sizeof(p_prc->p_ind[0]),ind_sort_rec);
  qsort(p_prc->p_fn,p_prc->fn_akt,sizeof(p_prc->p_fn[0]),ind_sort_rec2);
}

int ind_sort_obj_rec(const void *p_m1, const void *p_m2)
{
  const NORM_INDICIE *p_i1 = (NORM_INDICIE *)p_m1;
  const NORM_INDICIE *p_i2 = (NORM_INDICIE *)p_m2;
  return(p_i1->vertex-p_i2->vertex);
}

void ind_sort_obj(NORM_INT_PROC *p_prc)
{
  qsort(p_prc->p_ind,p_prc->ind_akt,sizeof(p_prc->p_ind[0]),ind_sort_obj_rec);
}

BOD * ind_najdi_normal(NORM_INT_PROC *p_prc, int vertex, BOD *p_norm)
{
  FACE_INDICIE *p_face = p_prc->p_fn;
  int i,start = 0, stop = p_prc->fn_akt, stred;

  do {
    stred = start+((stop-start)>>1);
    if(p_face[stred].vertex == vertex) {
      break;
    } else {
      if(start+1 >= stop) {
        return(NULL);
      } else if(p_face[stred].vertex > vertex) {
        stop = stred;
      } else {
        start = stred;
      }
    }
  } while(1);

  for(i = stred; i < p_prc->fn_akt && p_face[i].vertex == vertex; i++) {
    vektor_add(p_norm,&p_face[i].norm,p_norm);
  }
  for(i = stred-1; i > -1 && p_face[i].vertex == vertex; i--) {
    vektor_add(p_norm,&p_face[i].norm,p_norm);
  }

  return(p_norm);
}

inline int stejny_ind(NORM_INDICIE *p_ind1, NORM_INDICIE *p_ind2)
{
  return(stejny_vertex_point(p_ind1->p_vert,p_ind2->p_vert));
}

void ind_calc_norm(NORM_INT_PROC *p_prc, int brutal)
{
  NORM_INDICIE *p_first;
  NORM_INDICIE *p_last;
  BOD  norm;
  int  i,j,num;
  
  ind_sort(p_prc,brutal);
  
  p_last = p_prc->p_ind;
  i = 0;
  do {
    p_first = p_last;
    if(brutal) {
      for(; (i < p_prc->ind_akt) && stejny_ind(p_first,p_last); i++,p_last++);
      num = p_last-p_first;
    } else {
      if(i < p_prc->ind_akt) {
        i++;
        p_last++;
        num = 1;
      }
    }
    vektor_set(&norm,0.0f);
    for(j = 0; j < num; j++) {
      ind_najdi_normal(p_prc, p_first[j].vertex, &norm);
    }
    vektor_norm(&norm);
    for(j = 0; j < num; j++) {
      p_first[j].norm = norm;
    }
  } while(i < p_prc->ind_akt);

  ind_sort_obj(p_prc);
}


void calc_norm_vec(EDIT_OBJEKT *p_obj)
{
 NORM_INT_PROC prc;
 int           h1;

 ind_init(&prc, p_obj->vertexnum, p_obj->facenum);
 h1 = ind_add_obj(&prc, p_obj); 
 ind_calc_norm(&prc,FALSE);
 ind_vypln_obj(&prc, p_obj, h1);
 ind_smaz(&prc);

}

void kont_norm_vect_iobject(EDIT_KONTEJNER *p_kont)
{
  EDIT_OBJEKT *p_obj;
  NORM_INT_PROC prc;
  int *p_handle;
  int  i,o;

  updatuj_kontejner_statistika(p_kont,FALSE);

  ind_init(&prc, p_kont->bodu, p_kont->facu);

  p_handle = (int *)mmalloc(sizeof(p_handle[0])*p_kont->objektu);

  i = 0;
  oe_olist_reset(&o);
  while((p_obj = oe_olist_next(p_kont,&o))) {
    p_handle[i] = ind_add_obj(&prc, p_obj);
    i++;
  }
  
  ind_calc_norm(&prc,TRUE);

  i = 0;
  oe_olist_reset(&o);
  while((p_obj = oe_olist_next(p_kont,&o))) {
    ind_vypln_obj(&prc, p_obj, p_handle[i]);    
    i++;
  }

  ind_smaz(&prc);
  free(p_handle);
}

void kont_norm_vect(EDIT_KONTEJNER *p_kont_top)
{
  EDIT_KONTEJNER *p_kont = p_kont_top;
  int i;  
  
  while(p_kont) {
    if(p_kont->kflag&KONT_BRUTALNORMAL) {
      kont_norm_vect_iobject(p_kont);
    } else {
      for(i = 0; i < p_kont->max_objektu; i++) {
        if(p_kont->p_obj[i])
          calc_norm_vec(p_kont->p_obj[i]);
      }
    }
    p_kont = p_kont->p_next;
  }
}

/*
  Rendering linelistu
*/
void kresli_objekt_dratove_jednoduse(EDIT_OBJEKT *p_obj)
{
  int i,l1,l2;
  
  assert(p_obj);
  glBegin(GL_LINES);
    for(i = 0; i < p_obj->linenum; i+=2) {
      l1 = p_obj->p_line[i];
      l2 = p_obj->p_line[i+1];
      glColor3fv(&p_obj->p_vertex[l1].dr);
      glVertex3f(p_obj->p_vertex[l1].x,p_obj->p_vertex[l1].y,p_obj->p_vertex[l1].z);
      glColor3fv(&p_obj->p_vertex[l2].dr);
      glVertex3f(p_obj->p_vertex[l2].x,p_obj->p_vertex[l2].y,p_obj->p_vertex[l2].z);
    }
  glEnd();
}

void nastav_material_rgb_MAT(RGB_M *p_rgb)
{
 MATERIAL   mtrl;

 ZeroMemory( &mtrl, sizeof(mtrl) );
 mtrl.ambient_r = p_rgb->r;
 mtrl.ambient_g = p_rgb->g;
 mtrl.ambient_b = p_rgb->b;
 glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,(float *)p_rgb);
}

void nastav_material(float r, float g, float b)
{
 MATERIAL   mtrl;

 ZeroMemory( &mtrl, sizeof(mtrl) );
 mtrl.ambient_r = r;
 mtrl.ambient_g = g;
 mtrl.ambient_b = b;
 glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,&mtrl.ambient_r);
}

void kresli_kosoctverec(GLMATRIX *p_project, GLMATRIX *p_camera, GLMATRIX *p_world, BOD *p_p, float r, float dr, float dg, float db)
{ 
  GLMATRIX rr;
  BOD p,vrc[6];

  p.x = p_p->x; p.y = p_p->y; p.z = p_p->z;
  calc_3d_2d_matrix(p_world,p_camera,p_project,&rr);
  transformuj_bod_matici(&p.x,&p.y,&p.z,&rr);
  r = r*p.z*0.1f;
  
  vrc[0].x = p_p->x+r;
  vrc[0].y = p_p->y;
  vrc[0].z = p_p->z;
  
  vrc[1].x = p_p->x;
  vrc[1].y = p_p->y;
  vrc[1].z = p_p->z+r;
  
  vrc[2].x = p_p->x-r;
  vrc[2].y = p_p->y;
  vrc[2].z = p_p->z;
  
  vrc[3].x = p_p->x;
  vrc[3].y = p_p->y;
  vrc[3].z = p_p->z-r;
  
  vrc[4].x = p_p->x;
  vrc[4].y = p_p->y+r;
  vrc[4].z = p_p->z;
  
  vrc[5].x = p_p->x;
  vrc[5].y = p_p->y-r;
  vrc[5].z = p_p->z;

  glColor3f(dr,dg,db);
  glBegin(GL_LINE_LOOP);
    glVertex3fv((float *)&vrc[0]);
    glVertex3fv((float *)&vrc[1]);
    glVertex3fv((float *)&vrc[2]);
    glVertex3fv((float *)&vrc[3]);
    glVertex3fv((float *)&vrc[0]);
    glVertex3fv((float *)&vrc[4]);
    glVertex3fv((float *)&vrc[2]);
    glVertex3fv((float *)&vrc[5]);
    glVertex3fv((float *)&vrc[0]);
  glEnd();

  glBegin(GL_LINE_LOOP);
    glVertex3fv((float *)&vrc[3]);
    glVertex3fv((float *)&vrc[4]);
    glVertex3fv((float *)&vrc[1]);
    glVertex3fv((float *)&vrc[5]);
    glVertex3fv((float *)&vrc[3]);
  glEnd();

  glFlush();
}

void kresli_kosoctverec_word(BOD *p_p, float r, dword barva)
{ 
  BOD vrc[6];    

  vrc[0].x = p_p->x+r;
  vrc[0].y = p_p->y;
  vrc[0].z = p_p->z;
  
  vrc[1].x = p_p->x;
  vrc[1].y = p_p->y;
  vrc[1].z = p_p->z+r;
  
  vrc[2].x = p_p->x-r;
  vrc[2].y = p_p->y;
  vrc[2].z = p_p->z;
  
  vrc[3].x = p_p->x;
  vrc[3].y = p_p->y;
  vrc[3].z = p_p->z-r;
  
  vrc[4].x = p_p->x;
  vrc[4].y = p_p->y+r;
  vrc[4].z = p_p->z;
  
  vrc[5].x = p_p->x;
  vrc[5].y = p_p->y-r;
  vrc[5].z = p_p->z;

  
 glColor4ubv((byte *)&barva);
 glBegin(GL_LINE_LOOP);
    glVertex3fv((float *)(&vrc[0]));
    glVertex3fv((float *)(&vrc[1]));
    glVertex3fv((float *)(&vrc[2]));
    glVertex3fv((float *)(&vrc[3]));
    glVertex3fv((float *)(&vrc[0]));
    glVertex3fv((float *)(&vrc[4]));
    glVertex3fv((float *)(&vrc[2]));
    glVertex3fv((float *)(&vrc[5]));
    glVertex3fv((float *)(&vrc[0]));
  glEnd();

  glBegin(GL_LINE_LOOP);
    glVertex3fv((float *)(&vrc[3]));
    glVertex3fv((float *)(&vrc[4]));
    glVertex3fv((float *)(&vrc[1]));
    glVertex3fv((float *)(&vrc[5]));
    glVertex3fv((float *)(&vrc[3]));
  glEnd();

  glFlush();
}

void kresli_oktanovou_bunku_minmax(BOD *p_min, BOD *p_max, dword barva)
{
  glColor3ubv((byte *)&barva);
  glBegin(GL_LINES);
    glVertex3f(p_min->x,p_min->y,p_min->z);
    glVertex3f(p_min->x,p_max->y,p_min->z);
    
    glVertex3f(p_min->x,p_min->y,p_min->z);
    glVertex3f(p_max->x,p_min->y,p_min->z);
    
    glVertex3f(p_min->x,p_min->y,p_min->z);
    glVertex3f(p_min->x,p_min->y,p_max->z);

    glVertex3f(p_max->x,p_max->y,p_max->z);
    glVertex3f(p_min->x,p_max->y,p_max->z);
    
    glVertex3f(p_max->x,p_max->y,p_max->z);
    glVertex3f(p_max->x,p_min->y,p_max->z);
    
    glVertex3f(p_max->x,p_max->y,p_max->z);
    glVertex3f(p_max->x,p_max->y,p_min->z);

    glVertex3f(p_min->x,p_max->y,p_min->z);
    glVertex3f(p_min->x,p_max->y,p_max->z);
    
    glVertex3f(p_min->x,p_max->y,p_min->z);
    glVertex3f(p_max->x,p_max->y,p_min->z);

    glVertex3f(p_max->x,p_min->y,p_max->z);
    glVertex3f(p_min->x,p_min->y,p_max->z);    
    
    glVertex3f(p_max->x,p_min->y,p_max->z);
    glVertex3f(p_max->x,p_min->y,p_min->z);
    
    glVertex3f(p_max->x,p_min->y,p_min->z);
    glVertex3f(p_max->x,p_max->y,p_min->z);

    glVertex3f(p_min->x,p_min->y,p_max->z);
    glVertex3f(p_min->x,p_max->y,p_max->z);
  glEnd();
}

void konfiguruj_look_up(int *p_looksqrt_linear,
                        int *p_looksqrt_quadratic,
                        float *p_looksqrt_linear_float,
                        float *p_looksqrt_quadratic_float)
{  
  int   x,y,i;
  float vzdal;
  float fx,fy,fz,m1,m2,qk = (float)sqrt(255.0f);

  // 0 = max
  // 255 = min -> tak to nastavit
  // koeficienty utlumu si schovat !!
  for(x = 0; x < MAX_LOOK_TABLE; x++) {
    for(y = 0; y < MAX_LOOK_TABLE; y++) {
      i = y*MAX_LOOK_TABLE+x;
      // intenzity ve fixed-point
      fx = (float)(x);
      fy = (float)(y);
      fz = 0.0f;
      
      vzdal = sqrtf((float)(fx*fx+fy*fy+fz*fz)); // vzdal
      
      if((m1 = (255.0f-vzdal)/255.0f) < 0.0f)
        m1 = 0.0f;
      
      vzdal = qk/255.0f*vzdal;
      if((m2 = (255.0f-vzdal*vzdal)/255.0f) < 0.0f)
        m2 = 0.0f;
      p_looksqrt_linear[i] = ftoi(((float)(0xffffff))*m1);
      if(p_looksqrt_linear[i] > 0x01000000)
        p_looksqrt_linear[i] = 0x01000000;

      p_looksqrt_quadratic[i] = ftoi(((float)(0xffffff))*m2);
      if(p_looksqrt_quadratic[i] > 0x01000000)
        p_looksqrt_quadratic[i] = 0x01000000;
    }
  }

  for(i = 0; i < MAX_LOOK_TABLE; i++) {
    vzdal = (float)(i);      
    if((m1 = (255.0f-vzdal)/255.0f) < 0.0f)
      m1 = 0.0f;      
    vzdal = qk/255.0f*vzdal;
    if((m2 = (255.0f-vzdal*vzdal)/255.0f) < 0.0f)
      m2 = 0.0f;
    p_looksqrt_linear_float[i] = m1;
    p_looksqrt_quadratic_float[i] = m2;
  }

}

void transformuj_objekt_text_coord(EDIT_OBJEKT *p_obj, GLMATRIX *p_mat, int coord)
{
  int i;
  float z;

  for(i = 0; i < p_obj->vertexnum; i++) {
    if(coord&0x1) {
      z = 0.0f;
      transformuj_bod_matici(&p_obj->p_vertex[i].tu1, &p_obj->p_vertex[i].tv1, &z, p_mat);
    }
    if(coord&0x2) {
      z = 0.0f;
      transformuj_bod_matici(&p_obj->p_vertex[i].tu2, &p_obj->p_vertex[i].tv2, &z, p_mat);
    }
  }
}

void transformuj_kontejner_text_coord(EDIT_KONTEJNER *p_kont, GLMATRIX *p_mat, int coord)
{
  EDIT_OBJEKT *p_obj;
  int o;

  oe_olist_reset(&o);
  while(p_obj = oe_olist_next(p_kont, &o)) {
    transformuj_objekt_text_coord(p_obj,p_mat,coord);
  }
}


/* Spekularni ENV mapping na vybrany 
*/
void mesh_env_maping_spec(GAME_MESH *p_mesh, GLMATRIX *p_cam, EDIT_MATERIAL **p_mt)
{
  EDIT_MATERIAL *p_mat,*p_mat_prev = NULL;
  float     tx,ty,tz;
  float     px,py,pz;
  float     v1,v2,v3;
  float     r,g,b,uhel;
  BOD      *p_norm;
  BODRGB   *p_spec;
  BODRGB   *p_spec_src;
  float     add1,add2,scale;
  int       os,v,vertexnum;
  int       i,kframe;
  GLMATRIX *p_kw;
  int       flag2;

  p_mesh->m2flags.flag_set(KONT2_UPDATE_SPEC);
/*
  kframe = p_mesh->mflags.flag_get(KONT_KEYFRAME);
  if(!kframe) 
    p_kw = &p_mesh->m;

  v1 = p_cam->_13;
  v2 = p_cam->_23;
  v3 = p_cam->_33;
  
  for(i = 0; i < p_mesh->objektu; i++) {
    p_mat = p_mt[p_mesh->p_mat[i]];
    flag2 = p_mat->flag2;
    if(flag2&MAT2_ENV_SPEC) {
      if(p_mat_prev != p_mat) {

        // test na zmenu materialu
        os = p_mat->flag2&MAT2_ENV_SPEC_OBE;
        
        r = p_mat->env_r;
        g = p_mat->env_g;
        b = p_mat->env_b;
        add1 = p_mat->env_add1;
        add2 = p_mat->env_add2;
        scale = p_mat->env_scale;

        p_mat_prev = p_mat;
      }
      
      vertexnum = p_mesh->objektnum[i];
      v = p_mesh->objektstart[i];
      p_norm = p_mesh->p_vertex_norm+v;
      p_spec = p_mesh->p_vertex_spec+v;
      //p_spec_src = p_mesh->p_vertex_spec ? p_mesh->p_vertex_spec+v : NULL;
      p_spec_src = NULL;

      if(kframe) 
        p_kw = p_mesh->p_key+i;

      for(v = 0; v < vertexnum; v++) {
        px = p_norm->x;
        py = p_norm->y;
        pz = p_norm->z;
        p_norm++;

        tx = p_kw->_11*px + p_kw->_21*py + p_kw->_31*pz;
        ty = p_kw->_12*px + p_kw->_22*py + p_kw->_32*pz;
        tz = p_kw->_13*px + p_kw->_23*py + p_kw->_33*pz;
        
        norm_vect(&tx,&ty,&tz);
        uhel = (scale*(add1+tx*v1+ty*v2+tz*v3))+add2;
   
        if(p_spec_src) {
          if(os) {
            uhel = fabsf(uhel);
            p_spec->r = (r*uhel)+p_spec_src->r;
            p_spec->g = (g*uhel)+p_spec_src->g;
            p_spec->b = (b*uhel)+p_spec_src->b;
          } else {
            if(uhel > 0) {
              p_spec->r = (r*uhel)+p_spec_src->r;
              p_spec->g = (g*uhel)+p_spec_src->g;
              p_spec->b = (b*uhel)+p_spec_src->b;
            }
          }
          p_spec_src++;
          p_spec++;
        } else {
          if(os) {
            uhel = fabsf(uhel);
            p_spec->r = r*uhel;
            p_spec->g = g*uhel;
            p_spec->b = b*uhel;
          } else {
            if(uhel > 0) {
              p_spec->r = r*uhel;
              p_spec->g = g*uhel;
              p_spec->b = b*uhel;
            }
          }
          p_spec++;
        }
      }
    }
  }
  */
}


/*
  Env-mapping na poly
*/
void poly_env_maping_spec(EDIT_MESH_POLY *p_poly, EDIT_MATERIAL *p_mat, GLMATRIX *p_cam)
{
 TEXT_KOORD *p_koord;
 int       os = p_mat->flag2&MAT2_ENV_SPEC_OBE;
 float     tx,ty,tz;
 float     v1,v2,v3;
 int       v,vertexnum;
 float     uhel;
 float     r,g,b;
 float     add1,add2,scale;
 
 r = p_mat->env_r;
 g = p_mat->env_g;
 b = p_mat->env_b;

 v1 = p_cam->_13;
 v2 = p_cam->_23;
 v3 = p_cam->_33;
 
 add1 = p_mat->env_add1;
 add2 = p_mat->env_add2;
 scale = p_mat->env_scale;

 vertexnum = p_poly->facenum;
 p_koord = p_poly->p_koord;
 
 for(v = 0; v < vertexnum; v++) {
   tx = p_koord->nx;
   ty = p_koord->ny;
   tz = p_koord->nz;

   uhel = (scale*(add1+(tx*v1+ty*v2+tz*v3)))+add2;

   if(os)
     uhel = fabsf(uhel);
   else if(uhel < 0)
     continue;

   p_koord->sr = r*uhel+p_koord->msr;
   p_koord->sg = g*uhel+p_koord->msr;
   p_koord->sb = b*uhel+p_koord->msr;
   
   p_koord++;
 }

 p_poly->kflag |= KONT_UPLOAD;
}

int intersect_poly(EDIT_MESH_POLY *p_poly, BOD *p_orig, BOD *p_dir)
{
 TEXT_KOORD  *p_face;
 int          f,facenum = p_poly->facenum;
 float        t,u,v;
 
 p_face = p_poly->p_koord;
 for(f = 0; f < facenum; f+=3, p_face+=3) {
   if(intersect_triangle_ncull(p_orig,p_dir,(BOD *)p_face,(BOD *)(p_face+2),(BOD *)(p_face+1),&t,&u,&v) && t > 0.0f && t < 1.0f)
     return(TRUE);
 }

 return(FALSE);
}

int intersect_mesh_objekt(GAME_MESH *p_mesh, int o, BOD *p_orig, BOD *p_dir)
{
/*
  BOD        *p_vertex_pos;
  int         j,d,facenum,fnum2;
  int         kflag = p_mesh->p_data->kflag;
  int         key_flag = kflag&KONT_KEYFRAME;
  int        *p_face, typ;
  int        *p_face_prvni;

  float       t,u,v;

  // Dynamicke meshe - automaticky uspech
  if(p_mesh->p_data->kflag&KONT_STATIC || p_mesh->p_data->kflag&KONT_NORMAL_ANIM) {
    
    assert(o < p_mesh->objektu);
    p_vertex_pos = p_mesh->p_vertex_pos;
    
    p_face = p_mesh->p_face+p_mesh->facestart[o];
    facenum = p_mesh->facenum[o];
    for(j = 0; j < facenum; j++) {
      fnum2 = *p_face++;
      typ = *p_face++;
      switch(typ) {
      case GL_TRIANGLES:
        for(d = 0; d < fnum2; d += 3, p_face += 3) {
          if(intersect_triangle_ncull(p_orig,p_dir,p_vertex_pos+p_face[0],
            p_vertex_pos+p_face[2],p_vertex_pos+p_face[1],&t,&u,&v) && t > 0.0f && t < 1.0f)
            return(TRUE);
        }
        break;
        
      case GL_TRIANGLE_STRIP:
        if(intersect_triangle_ncull(p_orig,p_dir,p_vertex_pos+p_face[0],
          p_vertex_pos+p_face[2],p_vertex_pos+p_face[1],&t,&u,&v) && t > 0.0f && t < 1.0f)
          return(TRUE);
        
        p_face += 3;
        for(d = 3; d < fnum2; d++, p_face++) {          
          if(intersect_triangle_ncull(p_orig,p_dir,
            p_vertex_pos+p_face[-2],p_vertex_pos+p_face[-1],
            p_vertex_pos+p_face[0],&t,&u,&v) && t > 0.0f && t < 1.0f)
            return(TRUE);
        }
        break;
        
      case GL_TRIANGLE_FAN:      
        if(intersect_triangle_ncull(p_orig,p_dir,p_vertex_pos+p_face[0],
          p_vertex_pos+p_face[2],p_vertex_pos+p_face[1],&t,&u,&v) && t > 0.0f && t < 1.0f)
          return(TRUE);
        
        p_face_prvni = p_face;
        p_face += 3;
        fnum2 -= 3;
        
        for(d = 3; d < fnum2; d++, p_face++) {
          if(intersect_triangle_ncull(p_orig,p_dir,p_vertex_pos+p_face_prvni[0],
            p_vertex_pos+p_face[0],p_vertex_pos+p_face[-1],&t,&u,&v) && t > 0.0f && t < 1.0f)
            return(TRUE);
        }
        break;
      }
    }
    return(FALSE);
  } else {
    return(TRUE);
  }
  */
}
