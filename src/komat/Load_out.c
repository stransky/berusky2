/* Load/save out souboru
*/
#include "mss_on.h"
#include <windows.h>
#include <string.h>
#include <direct.h>
#include <io.h>
#include <limits.h>

#include "3d_all.h"

int lo_nacti_word_matrix_out(FFILE f, GLMATRIX *p_mat);
int lo_nahraj_objekty_out(EDIT_MATERIAL **p_mat, int max_mat, FFILE f, EDIT_KONTEJNER *p_kont, int mat);


typedef struct _KONTEJNER_KONFIG {
 
  dword flag;          // flagy konteneru
  BOD   zrcadlo[4];    // 4 body zrcadla
  byte  rezerved[52];

} KONTEJNER_KONFIG;

typedef struct _KONTEJNER_KONFIG_OLD {
 
  byte  flag;          // flagy konteneru
  byte  rezerved[2000];

} KONTEJNER_KONFIG_OLD;

typedef struct _GENERIC_HEAD { //typicka hlavicka

  byte h[4];  //flag (VVVV,FFFF,MMMM)
  word pocet; //pocet elementu

} GENERIC_HEAD;

int lo_je_tag(FFILE f, byte *p_tag)
{
 long t = fftell(f);
 byte string[10];
   
 ffgets(string, 5, f);
 string[4] = 0;
 
 if(strcmp(string,p_tag) != 0) {
   ffseek(f,t,SEEK_SET);
   return((int)NULL);
 }
 
 return(TRUE); 
}

int lo_nahraj_konfiguraci_kontejneru_out(FFILE f, EDIT_KONTEJNER *p_kont)
{
 KONTEJNER_KONFIG kkonf;
 KONTEJNER_KONFIG_OLD kkonf_old;
  
 if(lo_je_tag(f,KONFIG_TAG)) { // nacti nove info
   ffread(&kkonf,sizeof(kkonf),1,f);
   p_kont->m1flag = kkonf.flag&0xffff;
   p_kont->kflag = kkonf.flag&0xffff0000;
   return(TRUE);
 } else if(lo_je_tag(f,KONFIG_TAG_OLD)) { // nacti stare info
   ffread(&kkonf_old,sizeof(kkonf_old),1,f);
   p_kont->m1flag |= kkonf_old.flag ? MAT_PRUHLEDNY : 0;
   return(TRUE);
 } else
   return(FALSE);
}

int lo_nacti_word_matrix_out(FFILE f, GLMATRIX *p_mat)
{
 GENERIC_HEAD head;
 int t = fftell(f);
 
 if(!ffread(&head,sizeof(head),1,f)) {
   ffseek(f,t,SEEK_SET);
   return(FALSE);
 }
 if(head.h[0] != 'W' || head.h[1] != 'W' || head.h[2] != 'W' || head.h[3] != 'W') {
   ffseek(f,t,SEEK_SET);
   return(FALSE);
 }
 if(!ffread(p_mat,sizeof(GLMATRIX),1,f)) {
   ffseek(f,t,SEEK_SET);
   return(FALSE);
 }
 return(TRUE);
}

void nahraj_texturu_fc(FFILE f, EDIT_MATERIAL *p_mat, EDIT_TEXT *p_text, int max, int cislo_text, int save)
{
 OLD_MULTITEXT_FLAG mt;
 byte string[100];
 int  t;

 if(lo_je_tag(f, TEXT_TAG)) {
   str_read(string,f);
   _strlwr(string);
   if(!strcmp(string,S_NIC)) {
     p_mat->p_text[cislo_text] = NULL;
     p_mat->textfile[cislo_text][0] = 0;
   } else {
     if(save) {
       if((t = lo_najdi_texturu(p_text,max,string,FALSE)) == K_CHYBA) {
         if((t = lo_najdi_volnou_texturu(p_text,max)) == K_CHYBA)
           chyba("Textury");
         strcpy(p_text[t].jmeno,string);
       }
       p_mat->p_text[cislo_text] = p_text+t;
       strcpy(p_mat->textfile[cislo_text],string);       
     }
   }
 }
 if(lo_je_tag(f, MULTI_TAG))
   ffread(&mt,sizeof(mt),1,f);
}

EDIT_MATERIAL * lo_nahraj_material_out(FFILE f, EDIT_TEXT *p_text, int max, int save)
{
 EDIT_MATERIAL *p_mat;
 MATERIAL      *p_dxmat;
 OLD_MULTITEXT_FLAG mt;
 byte  string[50];
 dword tmp;
 int i = 0;
 
 if(!lo_je_tag(f, MTRL_TAG))
   return(NULL);
 else {
   p_mat = vyrob_material();
   str_read(p_mat->jmeno,f);
 }
 ffread(string,sizeof(byte),9,f);
 
 // nacteni grb
 if(save) {
   p_dxmat = &p_mat->dxmat;
   p_dxmat->ambient_r = (float)string[0]/MAX_BYTE;
   p_dxmat->ambient_g = (float)string[1]/MAX_BYTE;
   p_dxmat->ambient_b = (float)string[2]/MAX_BYTE;
   
   p_dxmat->diffuse_r = (string[3]) ? ((float)string[3])/MAX_BYTE : 1.0f;
   p_dxmat->diffuse_g = (string[4]) ? ((float)string[4])/MAX_BYTE : 1.0f;
   p_dxmat->diffuse_b = (string[5]) ? ((float)string[4])/MAX_BYTE : 1.0f;
   p_dxmat->diffuse_a = 1.0f;
   
   p_dxmat->specular_r = (float)string[6]/MAX_BYTE;
   p_dxmat->specular_g = (float)string[7]/MAX_BYTE;
   p_dxmat->specular_b = (float)string[8]/MAX_BYTE;

   p_dxmat->faktor_r = 1.0f;
   p_dxmat->faktor_g = 1.0f;
   p_dxmat->faktor_b = 1.0f;
   p_dxmat->faktor_a = 1.0f;   
 }

 // nacte soubor s animacema
 if(lo_je_tag(f, MATANIM_TAG)) {
   str_read(p_mat->anim.jmeno,f);
   //lo_vyrob_animaci_list(p_mat,p_mat->anim.jmeno,p_text,max,ANIM_LOAD_ALL);
 }

 // nacte alfa tag
 if(lo_je_tag(f, FAKTOR_TAG)) {    
   ffread(&tmp,sizeof(dword),1,f);
 }

 // Nacte stage blok
 if(lo_je_tag(f, STAG_TAG)) {
   ffread(string,sizeof(byte),1,f);
 
   ffread(&tmp,sizeof(dword),1,f);
   ffread(&tmp,sizeof(dword),1,f);
   
   p_mat->alfa_state = 0;
   p_mat->text_state[0].text_stage = 0;
   p_mat->text_state[1].text_stage = 0;
 }

 // nacteni nuloveho blendu
 if(lo_je_tag(f, MULTI_TAG))
   ffread(&mt,sizeof(mt),1,f);

 nahraj_texturu_fc(f, p_mat, p_text, max, 0, save);
 nahraj_texturu_fc(f, p_mat, p_text, max, 1, save);
 nahraj_texturu_fc(f, p_mat, p_text, max, 1, save);

 _strlwr(p_mat->jmeno);
 
 if(save)
   return(p_mat);
 else {
   zrus_material(&p_mat);
   return((EDIT_MATERIAL *)1);
 }
}

int lo_nahraj_materialy_out(EDIT_MATERIAL **p_mat, int max_mat, EDIT_TEXT *p_text,
                         int max_text, FFILE f, int save)
{
 EDIT_MATERIAL *p_tmp;
 int i = 0;
 int m,nm;

 while(1) {
   if(save) {
     if((m = lo_najdi_prazdny_material( p_mat, max_mat)) == K_CHYBA)
       chyba("Malo materialu!");
   } 
   p_tmp = lo_nahraj_material_out(f, p_text, max_text, save);
   if(!p_tmp) {
     break;
   }         
   if(save) { 
     if((nm = lo_najdi_material(p_mat,max_mat,p_tmp->jmeno)) == K_CHYBA)
       p_mat[m] = p_tmp;
     else {
       /* Nahravany material byl nalezen na pozici nm
       */
       if(hlasit_kolize) {
         ddw("Kolize materialu - krizi se materialy %d \"%s\" a %d \"%s\"!",m,p_tmp->jmeno,nm,p_mat[nm]->jmeno);
           //p_mat[nm] - puvodni material
           //doe_cti_jmeno(NULL,p_mat[nm]->jmeno);
         //}
       }
       zrus_material(&p_tmp);
     }
   }
   i++;
 }
 return(i);
}

int lo_nahraj_materialy_out_jmeno(EDIT_MATERIAL **p_mat, int max_mat,
                                  EDIT_TEXT *p_text, int max_text, byte *p_file,
                                  int save)
{

 EDIT_KONTEJNER *p_kont = NULL;
 FFILE f;
  
 if((f = ffopen(p_file,"rb")) == NULL) {
   //ddw("Chyba nahrani materialu %s chyba '%s'",p_file,strerror(errno));
   return(FALSE);
 }

 p_kont = vyrob_kontejner();
 lo_nahraj_konfiguraci_kontejneru_out(f, p_kont);
 lo_nacti_word_matrix_out(f, &p_kont->world);
 lo_nahraj_materialy_out(p_mat, max_mat, p_text, max_text, f, save);
 ffclose(f);

 // neni treba premapovat svetla
 zrus_kontejner_rec(&p_kont,NULL);
 return(TRUE);
}

int lo_nacti_vertexlist_out(FFILE f, OBJEKT_BOD **p_vert)
{
 GENERIC_HEAD head;
 long  t = fftell(f);
 float v[3];
 int   i;

 if(*p_vert != NULL) {
   return((int)NULL);
 }

 if(!ffread(&head,sizeof(head),1,f)) {
   ffseek(f,t,SEEK_SET);
   return((int)NULL);
 }

 if((head.h[0] != 'V')||
    (head.h[1] != 'V')||
    (head.h[2] != 'V')||
    (head.h[3] != 'V')) {
   ffseek(f,t,SEEK_SET);
   return((int)NULL);
 }

 if((*p_vert=(OBJEKT_BOD *)mmalloc(sizeof(OBJEKT_BOD)*head.pocet))==NULL) {
   chyba("Nedostatek pameti...");
 }

 memset(*p_vert,0,sizeof(OBJEKT_BOD)*head.pocet);

 for(i = 0; i < head.pocet; i++) {
   if(!ffread(&v,sizeof(v),1,f)) {
     ffseek(f,t,SEEK_SET);
     null_free(p_vert);
     return((int)NULL);
   }
   (*p_vert)[i].x = v[0];
   (*p_vert)[i].y = v[1];
   (*p_vert)[i].z = v[2];
   (*p_vert)[i].dr = (*p_vert)[i].dg = (*p_vert)[i].db = 1.0f;
 }

 return(head.pocet);
}

int lo_nacti_maplist_out(FFILE f, OBJEKT_BOD **p_vert, int c_text)
{
 GENERIC_HEAD head;
 long  t = fftell(f);
 float *p_map;
 float m[2];
 int   i;
 
 if(*p_vert == NULL)
   return((int)NULL);

 if(!ffread(&head,sizeof(head),1,f)) {
   ffseek(f,t,SEEK_SET);
   return((int)NULL);
 }

 if((head.h[0] != 'M')||
    (head.h[1] != 'M')||
    (head.h[2] != 'M')||
    (head.h[3] != 'M')) {
   ffseek(f,t,SEEK_SET);
   return((int)NULL);
 }

 if(c_text >= 2) {
   c_text = 1;
 }

 for(i = 0; i < head.pocet; i++) {
   if(!ffread(&m,sizeof(m),1,f)) {
     ffseek(f,t,SEEK_SET);
     return((int)NULL);
   }
   p_map = &(*p_vert)[i].tu1;
   p_map[0+c_text*2] = m[0];
   p_map[1+c_text*2] =-m[1];
 }

 return(head.pocet);
}

int lo_nacti_facelist_out(FFILE f, FACE **p_face)
{
 GENERIC_HEAD head;
 long  t = fftell(f);


 if(*p_face != NULL)
   return((int)NULL);

 if(!ffread(&head,sizeof(head),1,f)) {
   ffseek(f,t,SEEK_SET);
   return((int)NULL);
 }

 if((head.h[0] != 'F')||
    (head.h[1] != 'F')||
    (head.h[2] != 'F')||
    (head.h[3] != 'F')) {
   ffseek(f,t,SEEK_SET);
   return((int)NULL);
 }

 head.pocet *= 3;
 if(!head.pocet)
   chyba("load facelist -> 0 facu !");

 if((*p_face=(FACE *)mmalloc(sizeof(FACE)*head.pocet))==NULL) {
   chyba("Nedostatek pameti...");
 }

 if(!ffread(*p_face,sizeof(FACE),head.pocet,f)) {
   null_free(p_face);
   ffseek(f,t,SEEK_SET);
   return((int)NULL);
 }

 return(head.pocet);
}

int lo_nacti_barvy_out(FFILE f, OBJ_VERTEX *p_vert, word pocet)
{
 GENERIC_HEAD head;
 dword barva;
 long  t = fftell(f);
 int i;

 if(p_vert == NULL)
   return(FALSE);

 for(i = 0; i < pocet; i++) {
   p_vert[i].dr = p_vert[i].dg = p_vert[i].db = 1.0f;
 }

 if(!ffread(&head,sizeof(head),1,f)) {
   ffseek(f,t,SEEK_SET);
   return(FALSE);
 }

 if((head.h[0] != 'D')||
    (head.h[1] != 'D')||
    (head.h[2] != 'D')||
    (head.h[3] != 'D')) {
   ffseek(f,t,SEEK_SET);
   return(FALSE);
 }

 for(i = 0; i < head.pocet; i++) {
   if(!ffread(&barva,sizeof(dword),1,f)) {     
     ffseek(f,t,SEEK_SET);
     return(FALSE);
   } else {
     rgb_float(barva,&p_vert[i].dr);
   }
 }

 return(head.pocet);
}

EDIT_OBJEKT * lo_nacti_vec_FILE_out(FFILE f)
{
 
 EDIT_OBJEKT *p_obj = vyrob_objekt();
 int          i = 0;

 if((p_obj->vertexnum = lo_nacti_vertexlist_out(f, &p_obj->p_vertex)) == (int)NULL) {
   zrus_objekt(&p_obj);
   ffclose(f);
   return(NULL);
 }

 while(lo_nacti_maplist_out(f, &p_obj->p_vertex,i++));
 
 if((p_obj->facenum=lo_nacti_facelist_out(f, &p_obj->p_face)) == (int)NULL) {
   zrus_objekt(&p_obj);
   ffclose(f);
   return(NULL);
 }
 
 lo_nacti_barvy_out(f, p_obj->p_vertex, p_obj->vertexnum);

 return(p_obj);
}

EDIT_OBJEKT * lo_nacti_vec_out(byte *p_filename)
{
 EDIT_OBJEKT *p_obj;
 FILE        *f;

 if((f = ffopen(p_filename,"rb")) == NULL)
   return(NULL);

 p_obj = lo_nacti_vec_FILE_out(f);
 ffclose(f);

 return(p_obj);
}

int lo_nahraj_objekty_out(EDIT_MATERIAL **p_mat, int max_mat, FFILE f, EDIT_KONTEJNER *p_kont, int mat)
{
 byte  string[50];
 int   i,j,k;
 word  r1;

 for(i = 0; i < p_kont->max_objektu; i++) {
   if(!lo_je_tag(f,"JMOB"))
     break;
   else
     str_read(string,f);

   if(!lo_je_tag(f,"TMSH"))
     continue; //je to objekt ale neni to mes

   if((p_kont->p_obj[i] = lo_nacti_vec_FILE_out(f)) == NULL)
     chyba("Nacitani objektu...");
   
   strcpy(p_kont->p_obj[i]->jmeno, string);
  
   for(k = 0; k < 2; k++) {
     //nactenej jeden objekt - pricist jeste material
     if(!lo_je_tag(f,"FMAT")) {
       break; // neni material - na dalsi objekt
     }
     else
       str_read(string,f);

     if(mat) {
       if(!k) {
         _strlwr(string);
         if(!strcmp(string,S_NIC)) {
           p_kont->p_obj[i]->material = 0;
           p_kont->p_obj[i]->m1flag = 0;
           p_kont->p_obj[i]->m2flag = 0;
           p_kont->p_obj[i]->oflag = 0;
         }
         else {
           for(j = 0; j < max_mat; j++) {
             if(p_mat[j] && !strcmp(string,p_mat[j]->jmeno)) {
               p_kont->p_obj[i]->material = (word)j;
               p_kont->p_obj[i]->m1flag = p_mat[j]->flag;
               p_kont->p_obj[i]->m2flag = p_mat[j]->flag2;
               break;
             }
           }
           if(j == max_mat) {
             ddw("Neznamy material '%s' v objektu '%s'",string,p_kont->p_obj[i]->jmeno);
             p_kont->p_obj[i]->material = 0;             
             p_kont->p_obj[i]->m1flag = 0;
             p_kont->p_obj[i]->m2flag = 0;
           } 
         }
       }
     }
     else {
       p_kont->p_obj[i]->material = 0;
       p_kont->p_obj[i]->m1flag = 0;
       p_kont->p_obj[i]->m2flag = 0;
     }
   }
   if(lo_je_tag(f,"RECT")) { // sou tam recy
     ffread(&r1,sizeof(word),1,f);
     ffread(&r1,sizeof(word),1,f);
   }
 } 
 return(i);
}

EDIT_KONTEJNER * lo_nahraj_kontejner_out(EDIT_MATERIAL **p_mat, int max_mat, EDIT_TEXT *p_text, int max_text,
                 byte *p_jmeno, int mat)
{
 EDIT_KONTEJNER *p_kont = NULL;
 FFILE f;
 int   i = 0;
 
 p_kont = vyrob_kontejner();

 if((f = ffopen(p_jmeno,"rb")) == NULL) {
   /*
   if(TVRDE_CHYBY)
     tiskni_chybu(__LINE__,__FILE__,NULL,"Chyba '%s' file '%s'",strerror(errno),p_jmeno);
   else {
     ddw("Chyba nahrani kontejneru %s chyba '%s'",p_jmeno,strerror(errno));
     return(NULL);
   }
   */
   return(NULL);
 }
 
 lo_nahraj_konfiguraci_kontejneru_out(f, p_kont);
 lo_nacti_word_matrix_out(f, &p_kont->world);
 lo_nahraj_materialy_out(p_mat, max_mat, p_text, max_text, f, mat);
 i = lo_nahraj_objekty_out(p_mat, max_mat, f, p_kont, mat);
 
 ffclose(f);

 if(i)
   updatuj_kontejner_statistika(p_kont,FALSE);

 return(p_kont);
}
