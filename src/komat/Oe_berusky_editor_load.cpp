/*
  Editor k beruskam  
*/

#include "mss_on.h"
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <direct.h>
#include <float.h>
#include <assert.h>
#include "objekt_editor_all.h"

// pokud je objekt s guid staticky vraci 1
// 
int be_je_objekt_viditelny(int guid)
{
  return(guid/1000 != GUID_PRAZDNA_STENA/1000);
}

__inline static void zrus_n(byte *p_string)
{
 byte *p_n;
 if(p_n = strchr(p_string,'\n')) {
   *p_n = 0;
 }
}

void be_konfiguruj_berusky(B_KONFIG *p_bnf, K_EDITOR *p_cnf, byte *p_file)
{
 BOD    p = {0,0,0};
 BOD    ks= {0,1,0};
 byte   pom[500];
 byte **p_sekce;
 int    sekci;
 int    i,j,k;
 FILE  *f;

 p_bnf->p_cnf = p_cnf;

 strcpy(p_bnf->jmeno_levelu,DEFAULT_LEVEL);

 if(p_bnf->p_kont[KONT_NEVID_STENA] || p_bnf->p_kont[KONT_NEVID_PRVEK])
   chyba("Zacpa !");
 p_bnf->p_kont[KONT_NEVID_STENA] = vyrob_krychli_rozlozenou(&ks,1,1,1); 
 p_bnf->p_kont[KONT_NEVID_PRVEK] = vyrob_krychli_rozlozenou(&ks,1,1,1);
 
          
 ko_Nahraj_herni_prvky();
 p_bnf->p_kofola_prv = Object;
 p_bnf->p_kofola_file = SecondData;
 be_nacti_prvky_do_databaze(p_bnf);

 p_bnf->grid.kreslit[0] = 1;
 p_bnf->grid.akt_patro = 0;
 p_bnf->kp.akt_prvek = 1;
 p_bnf->cnf.kostka_remake = TRUE;
 p_bnf->cnf.kostka = FALSE;
 //p_bnf->grid.grid = TRUE;

 p_bnf->grid.p_kurzor = vyrob_krychli_dratovou(&p, X_PRVEK, Y_PRVEK/10.0f, Z_PRVEK, DDRGB(1,0,0)); 

 init_matrix(&p_bnf->grid.m); 

 p_bnf->cnf.kreslit_dynamicke_objekty = TRUE;
 p_bnf->cnf.kreslit_staticke_objekty = TRUE;
 p_bnf->cnf.kreslit_stinove_objekty = FALSE;

 p_bnf->x_start = X_ZACATEK;
 p_bnf->y_start = Y_ZACATEK;
 p_bnf->z_start = Z_ZACATEK;

 chdir(p_cnf->dir.data_dir);
 if(!(f = fopen("prvky.txt","r"))) {
   kprintf(TRUE, "! Nemuzu otevrit seznamy prvku");
   return;
 }
 i = 0;
 while(fgets(p_bnf->seznam_tridy_prvku[i],MAX_TRID_JMENO,f)) {
   zrus_n(p_bnf->seznam_tridy_prvku[i]);
   i++;
 }
 p_bnf->seznam_trid = i;
 fclose(f); 
 
 for(j = 0; j < i; j++) {
   sprintf(pom,"pod_%d.txt",j);
   if(efile(pom)) {     
     if(!(f = fopen(pom,"r"))) {
       kprintf(TRUE, "! Nemuzu otevrit seznam podprvku '%s'.",pom);
       return;
     }
     k = 0;
     while(fgets(p_bnf->seznam_podtridy[j][k],MAX_TRID_JMENO,f)) {
       zrus_n(p_bnf->seznam_podtridy[j][k]);
       k++;
     }
     p_bnf->seznam_tridy_flagy[j] = k;
     fclose(f);
   }
 }

 /* Nahrani sadare
 */
 #define PRVKY_SADA_MUSTR "prvky_sada%d.ini"

 for(i = 0; i < MAX_SAD; i++) {
   chdir(p_cnf->dir.data_dir);
   sprintf(pom,PRVKY_SADA_MUSTR,i);
   if(be_spocti_sekce_v_ini(pom, &p_sekce, &sekci)) {
     strcpy(p_cnf->bnf.sad.jmena[i],p_sekce[0]);
     be_nahraj_sekce(pom, p_cnf->dir.data_dir, i, p_sekce+1, sekci-1, &p_cnf->bnf.sad);
     be_smaz_sekce(p_sekce, sekci);     
   }
 }
}

int be_spocti_sekce_v_ini(byte *p_file, byte ***p_sekce, int *p_snum)
{
  byte  line[1000];
  byte  **p_seznam;
  FILE *f;
  int   sekci = 0,delka;

  if(f = fopen(p_file,"r")) {
    while(fgets(line,1000,f)) {
      if(line[0] == '[')
        sekci++;
    }
    if(sekci) {
      fseek(f,0,SEEK_SET);
      p_seznam = malloc(sizeof(p_seznam[0])*sekci);
      mtest(p_seznam);
      sekci = 0;

      while(fgets(line,1000,f)) {
        if(line[0] == '[') {
          delka = fgets_korekce(line);
                    
          p_seznam[sekci] = malloc(sizeof(p_seznam[sekci][0])*(delka+1));
          mtest(p_seznam[sekci]);
          
          memcpy(p_seznam[sekci],line+1,delka-2);
          p_seznam[sekci][delka-2] = 0;

          sekci++;
        }
      }
      *p_sekce = p_seznam;
      *p_snum = sekci;
    }
    fclose(f);
  }
  return(sekci);
}

int be_smaz_sekce(byte **p_sekce, int snum)
{
  int i;

  for(i = 0; i < snum; i++) {
    free(p_sekce[i]);
  }
  free(p_sekce);
  return(TRUE);
}

int be_nahraj_sekce(byte *p_file, byte *p_dir, int sada, byte **p_sekce, int snum, SADAR *p_sad)
{
  SADA_PRVKU *p_sady;
  byte pom[500];
  int  i;
  
  sprintf(pom,"%s\\%s",p_dir,p_file);
  p_sad->p_sada[sada] = malloc(sizeof(p_sad->p_sada[sada][0])*snum);
  mtest(p_sad->p_sada[sada]);
  memset(p_sad->p_sada[sada],0,sizeof(p_sad->p_sada[sada][0])*snum);
  p_sad->sadnum[sada] = snum;
  
  p_sady = p_sad->p_sada[sada];
  
  for(i = 0; i < snum; i++) {
    strcpy(p_sady[i].jmeno,p_sekce[i]);
    p_sady[i].trida = GetPrivateProfileInt(p_sekce[i],"trida",0,pom);
    p_sady[i].podtrida = GetPrivateProfileInt(p_sekce[i],"podtrida",0,pom);
    p_sady[i].guid = GetPrivateProfileInt(p_sekce[i],"guid1",0,pom);
    p_sady[i].prvek = doe_najdi_prvek(p_sady[i].guid);
  }
  return(TRUE);
}

void oe_wrlog(K_EDITOR *p_cnf, byte *p_text,...)
{
  B_KONFIG *p_bnf = &p_cnf->bnf;
  va_list argumenty;
  if(p_bnf->debug_file) {
    va_start(argumenty,p_text);
    vfprintf(p_bnf->debug_file,p_text,argumenty);
    fputc('\n',p_bnf->debug_file);
    va_end(argumenty);  
    fflush(p_bnf->debug_file);
  }
}

void print_matrix_file(FILE *f, GLMATRIX * m)
{

 #define PRESNOST2 "\n%f %f %f %f" 
 fprintf(f,PRESNOST2,m->_11,m->_21,m->_31,m->_41);
 fprintf(f,PRESNOST2,m->_12,m->_22,m->_32,m->_42);
 fprintf(f,PRESNOST2,m->_13,m->_23,m->_33,m->_43);
 fprintf(f,PRESNOST2,m->_14,m->_24,m->_34,m->_44);
 fputc('\n',f);
}

void oe_wrlog_matrix(K_EDITOR *p_cnf, GLMATRIX *p_mat, byte *p_text,...)
{
  B_KONFIG *p_bnf = &p_cnf->bnf;
  va_list argumenty;
  if(p_bnf->debug_file) {
    va_start(argumenty,p_text);
    vfprintf(p_bnf->debug_file,p_text,argumenty);
    va_end(argumenty);  
    print_matrix_file(p_bnf->debug_file, p_mat);
    fflush(p_bnf->debug_file);
  }
}


void be_nacti_prvky_do_databaze(B_KONFIG *p_bnf)
{
 p_bnf->prvku_databaze = be_nacti_prvky(p_bnf);
}

void be_vymaz_prvky_v_databazi(B_KONFIG *p_bnf)
{
  int i; 

  kprintf(TRUE,"Mazu databazi...");
  for(i = 0; i < MAX_PRVKU_DATABAZE; i++) {
    if(p_bnf->p_prv_dat[i]) {
      free(p_bnf->p_prv_dat[i]);
      p_bnf->p_prv_dat[i] = NULL;
    }
  }
}

void be_reload_databaze(B_KONFIG *p_bnf)
{
  be_vymaz_prvky_v_databazi(p_bnf);
  be_nacti_prvky_do_databaze(p_bnf);
}

PRVEK_DATABAZE * be_vyrob_prvek_databaze(void)
{
  PRVEK_DATABAZE *p_prv = malloc(sizeof(PRVEK_DATABAZE));

  if(!p_prv)
    return(NULL);

  memset(p_prv,0,sizeof(PRVEK_DATABAZE));
  return(p_prv);
}


int be_najdi_volny_prvek(PRVEK_DATABAZE **p_prv, int max)
{
 int i;

 for(i = 0; i < max; i++) {
   if(!p_prv[i]) {
     return(i);
   }
 }
 return(K_CHYBA);
}

/*
  Nahraje prvky do databaze prvku
*/
int be_nacti_prvky(B_KONFIG *p_bnf)
{
  K_EDITOR *p_cnf = (K_EDITOR *)p_bnf->p_cnf;
  PRVEK_DATABAZE *p_prv;
  byte *p_file;
  int i,k,p,nactenych = 0;

  kprintf(TRUE, "Nacitam prvky do databaze");
  for(i = 0; i < MAX_PRVKU_DATABAZE; i++) {
    chdir(p_cnf->dir.prvky_dir);
    
    p_file = p_bnf->p_kofola_file[i].File_Name[0];
    
    if(!p_bnf->p_kofola_prv[i].Class)
      continue;

    nactenych++;

    if((p = be_najdi_volny_prvek(p_bnf->p_prv_dat, MAX_PRVKU_DATABAZE)) == K_CHYBA)
      chyba("Neni volny prvek");
    
    if((p_prv = p_bnf->p_prv_dat[p] = be_vyrob_prvek_databaze()) == NULL)
      chyba("Pamet");
    
    p_prv->guid = p_bnf->p_kofola_prv[i].GUID;
    p_prv->kof_index = i;
    
    if(!p_file[0]) { // soubor nema kontejner
      p_prv->p_kont = NULL;
      p_prv->kont = K_CHYBA;
    } else {
      if((k = lo_najdi_kontejner(p_bnf->p_kont, MAX_BERUSKY_KONTEJNERU, p_file)) != K_CHYBA) {
        p_prv->kont = k;
        p_prv->p_kont = p_bnf->p_kont[k];
        kprintf(TRUE, " nasel kont '%s' pos. %d ",p_file,k);
      } else {
        // load kontejneru prv.mesh
        if((k = lo_najdi_volny_kontejner(p_bnf->p_kont,MAX_BERUSKY_KONTEJNERU)) == K_CHYBA)
          chyba("Neni volny kontejner !");
        
        chdir(p_cnf->dir.out_dir);
        kprintf(TRUE,"Load kontejner %s...",p_file);
        p_bnf->p_kont[k] = lo_nahraj_kontejner(p_bnf->p_mat, MAX_EDIT_MATERIALU,
                           cnf.p_text, MAX_EDIT_TEXTUR,p_file, cnf.cnf.load_textur);
        if(p_bnf->p_kont[k]) {
          strcpy(p_bnf->p_kont[k]->jmeno,p_file);        
          p_bnf->p_kont[k]->kflag |= (ber_je_objekt_staticky(p_prv->guid)) ? KONT_STATICKY_PRVEK : KONT_DYNAMICKY_PRVEK;
          p_prv->kont = k;
          p_prv->p_kont = p_bnf->p_kont[k];
        } else {
          kprintf(TRUE,"Chyba loadu kontu '%s'...",p_file);
        }
      }
    }
  }
  return(nactenych);
}

/*
  BUNKA_LEVELU *p_lev;
  int           x,y,z;                 // rozmery levelu
  level ma 2 vrstvy (0,1)
*/
PRVEK_LEVELU ** vyrob_level(int x, int y, int z)
{
 PRVEK_LEVELU **p_lev = malloc(sizeof(PRVEK_LEVELU *)*x*y*z*2);
 if(!p_lev) chyba("Nedostatek pameti !"); 
 memset(p_lev,0,sizeof(PRVEK_LEVELU *)*x*y*z*2);
 return(p_lev);
}

PRVEK_LEVELU ** be_vyrob_level(B_KONFIG *p_bnf, int x, int y, int z)
{
 PRVEK_LEVELU **p_lev = malloc(sizeof(PRVEK_LEVELU *)*x*y*z*2);
 mtest(p_lev); 
 memset(p_lev,0,sizeof(PRVEK_LEVELU *)*x*y*z*2);
 p_bnf->x_num = x; p_bnf->y_num = y; p_bnf->z_num = z;
 p_bnf->p_lev = p_lev; 
 return(p_lev);
}

// Kopiruje p-lev do p_new na zadane souradnice - pokud je
// vysledek mensi dojde k orezani
// souradnice vcetne pater !!!
void be_kopiruj_level(PRVEK_LEVELU **p_lev, int sdx, int sdy, int sdz,
                      PRVEK_LEVELU **p_new, int ndx, int ndy, int ndz,
                      int x, int y, int z)
{
  int tx,ty,tz;
  int si,ni;

  for(tx = 0; tx < sdx; tx++) {
    for(ty = 0; ty < sdy; ty++) {
      for(tz = 0; tz < sdz; tz++) {
        if(tx+x >= 0 && tx+x < ndx && 
           ty+y >= 0 && ty+y < ndy && 
           tz+z >= 0 && tz+z < ndz) {
          si = be_pozice_v_levelu_dir(sdx,sdz,tx,ty,tz);
          ni = be_pozice_v_levelu_dir(ndx,ndz,tx+x,ty+y,tz+z);
          if(p_lev[si]) {
            p_new[ni] = p_lev[si];
            p_new[ni]->lev_index = ni; // preindexuju level
          }
        }
      }
    }
  }
}


// Rozsiri/oreze level podle zadanych cisel
void be_rozsir_level(B_KONFIG *p_bnf, int px, int x, int py, int y, int pz, int z)
{
  PRVEK_LEVELU *p_tmp;
  PRVEK_LEVELU **p_lev;
  int i;
  int dx,dy,dz;
  int sx,sy,sz,sp;
  int min_x, min_y, min_z; // mezni hodnoty x,y,z
  int max_x, max_y, max_z;
  
  dx = p_bnf->x_num+x;
  dy = p_bnf->y_num+y;
  dz = p_bnf->z_num+z;

  min_x = (px < 0) ? x*(-1) : 0;
  min_y = (py < 0) ? y*(-1) : 0;
  min_z = (pz < 0) ? z*(-1) : 0;

  max_x = (px > 0) ? p_bnf->x_num+x-1 : p_bnf->x_num-1;
  max_y = (py > 0) ? p_bnf->y_num+y-1 : p_bnf->y_num-1;
  max_z = (pz > 0) ? p_bnf->z_num+z-1 : p_bnf->z_num-1;
  

  /* Projdu level a smazu prvku co jsou mimo
  */
  for(i = 0; i < p_bnf->prvnum; i++) {
    p_tmp = p_bnf->p_prv_lev[i];
    if(p_tmp) {      
      be_pozice_na_disku(p_tmp->lev_index,sx,sy,sz,sp,p_bnf->x_num,p_bnf->z_num);
      if(min_x > sx || max_x < sx ||
         min_y > sy || max_y < sy ||
         min_z > sz || max_z < sz) {
        // Smazani prvku
        be_zrus_prvek_levelu(p_bnf, sx, sy, sz, sp);
      }
    }
  }


  p_lev = malloc(sizeof(PRVEK_LEVELU *)*dx*dy*dz*2);
  mtest(p_lev);
  memset(p_lev,0,sizeof(PRVEK_LEVELU *)*dx*dy*dz*2);

  x = (px < 0) ? x : 0;
  y = (py < 0) ? y : 0;
  z = (pz < 0) ? z : 0;
  
  be_kopiruj_level(p_bnf->p_lev, p_bnf->x_num, p_bnf->y_num*2, p_bnf->z_num,
                          p_lev, dx, dy*2, dz, x, y*2, z);
  // .. kopie

  p_bnf->x_num = dx;
  p_bnf->y_num = dy;
  p_bnf->z_num = dz;

  free(p_bnf->p_lev);
  p_bnf->p_lev = p_lev;

  /* Vyrobim novy grid levelu
  */
  for(i = 0; i < p_bnf->y_num; i++)
    be_vyrob_grid_level(p_bnf, i);
  p_bnf->cnf.kostka_remake = TRUE;

  for(i = 0; i < p_bnf->prvnum; i++) {
    p_tmp = p_bnf->p_prv_lev[i];
    if(p_tmp) {
      be_preumisti_prvek(p_bnf, p_tmp);
    }
  }
}

void be_zrus_level(B_KONFIG *p_bnf)
{
 int i;

 be_zrus_grid_level(p_bnf);

 for(i = 0; i < MAX_PRVKU_LEVEL; i++) {
   if(p_bnf->p_prv_lev[i]) {
     free(p_bnf->p_prv_lev[i]);
     p_bnf->p_prv_lev[i] = NULL;
   }
 }

 p_bnf->prvnum = 0;
 null_free((void **)&p_bnf->p_lev);
 p_bnf->z_num = p_bnf->y_num = p_bnf->x_num = 0;
}

void be_kontroluj_level(B_KONFIG *p_bnf)
{
 int i,j;

 if(p_bnf->p_lev) {
   for(i = 0; i < MAX_PRVKU_LEVEL; i++) {
     if(p_bnf->p_prv_lev[i] && 
       p_bnf->p_prv_lev[i]->mesh != K_CHYBA && 
       !p_cnf->p_kont[p_bnf->p_prv_lev[i]->mesh]) {
       j = p_bnf->p_prv_lev[i]->lev_index;
       free(p_bnf->p_prv_lev[i]);
       p_bnf->p_prv_lev[i] = NULL;
       p_bnf->p_lev[j] = NULL;
     }
   }
 }
}

/*
 int i = be_pozice_v_levelu(p_bnf, x, y, z, vrstva);
 int mesh;
 int prv;

 if(p_bnf->p_lev[i]) {
   prv = p_bnf->p_lev[i]->prv_index;
   mesh = p_bnf->p_lev[i]->mesh;
   if(mesh != K_CHYBA && p_cnf->p_kont[mesh]) {
     p_kont = p_cnf->p_kont[mesh];
     p_cnf->p_kont[mesh] = NULL;
     while(p_kont) {
       p_next = p_kont->p_next;
       lo_vymaz_svetla_ze_sceny(p_kont);
       zrus_kontejner(&p_kont);
       p_kont = p_next;
     }
   } 
   if(p_bnf->p_prv_lev[prv])
     free(p_bnf->p_prv_lev[prv]);
   else {
     ddw("Free null %d %s",__LINE__,__FILE__);
   }
   p_bnf->p_prv_lev[prv] = NULL;
   p_bnf->p_lev[i] = NULL;
 }
*/

/*
typedef struct _BUNKA_LEVELU_DISK {

  int guid;
  int Vazba;  ??
  int Rotace; ??

} BUNKA_LEVELU_DISK;
*/

/*
Cinnost load:

  0. otestovat, jestli to neni zeme (volne misto)
  1. najit prvek v databazi prvku
  2. pridat ho do seznamu prvku v levelu (na urcite misto + init matrice)
  3. provazat odkaz z bunky na tento prvek

  guid 0 -> zeme (nenahravam tento prvek)

  - optimalizace - projiti levelu a zapsani vsech prvku (kolik jich je)
    a vyrobit ty prvky jako blok
*/

PRVEK_LEVELU * be_vyrob_prvek_levelu(void)
{
 PRVEK_LEVELU *p_prv = malloc(sizeof(PRVEK_LEVELU));
 if(!p_prv)
   return(NULL);
 memset(p_prv,0,sizeof(PRVEK_LEVELU));
 init_matrix(&p_prv->m);
 return(p_prv);
}

// zrusi prvek levelu
void be_zrus_prvek_levelu_index(B_KONFIG *p_bnf, int i)
{ 
 EDIT_KONTEJNER *p_kont,*p_next;
 K_EDITOR *p_cnf = (K_EDITOR *)p_bnf->p_cnf; 
 int mesh;
 int prv;

 if(p_bnf->p_lev[i]) {
   prv = p_bnf->p_lev[i]->prv_index;
   mesh = p_bnf->p_lev[i]->mesh;
   if(mesh != K_CHYBA && p_cnf->p_kont[mesh]) {
     p_kont = p_cnf->p_kont[mesh];
     p_cnf->p_kont[mesh] = NULL;
     while(p_kont) {
       p_next = p_kont->p_next;
       lo_vymaz_svetla_ze_sceny(p_kont);
       zrus_kontejner(&p_kont,&p_cnf->p_mlha);
       p_kont = p_next;
     }
   } 
   if(p_bnf->p_prv_lev[prv])
     free(p_bnf->p_prv_lev[prv]);
   else {
     ddw("Free null %d %s",__LINE__,__FILE__);
   }
   p_bnf->p_prv_lev[prv] = NULL;
   p_bnf->p_lev[i] = NULL;
 }
}

void be_zrus_prvek_levelu(B_KONFIG *p_bnf, int x, int y, int z, int vrstva)
{ 
  be_zrus_prvek_levelu_index(p_bnf,be_pozice_v_levelu(p_bnf, x, y, z, vrstva));
}

void be_zrus_prvek_levelu_mesh(B_KONFIG *p_bnf, int i)
{  
 K_EDITOR *p_cnf = (K_EDITOR *)p_bnf->p_cnf;
 EDIT_KONTEJNER *p_kont,*p_next;
 int k;

 if(p_bnf->p_prv_lev[i]) {
   if(p_bnf->p_prv_lev[i]->mesh != K_CHYBA) {
     k = p_bnf->p_prv_lev[i]->mesh;
     p_bnf->p_prv_lev[i]->mesh = K_CHYBA;
     p_kont = p_cnf->p_kont[k];
     p_cnf->p_kont[k] = NULL;
     while(p_kont) {
       p_next = p_kont->p_next;
       lo_vymaz_svetla_ze_sceny(p_kont);
       zrus_kontejner(&p_kont,&p_cnf->p_mlha);
       p_kont = p_next;
     }
   } 
 }
}

// vlozi novy prvek do levelu z databaze
PRVEK_LEVELU * be_pridej_prvek_levelu_z_databaze(B_KONFIG *p_bnf, PRVEK_DATABAZE *p_prv_databaze, int guid)
{
  K_EDITOR *p_cnf = (K_EDITOR *)p_bnf->p_cnf;
  int i,k;
  
  for(i = 0; i < MAX_PRVKU_LEVEL; i++) {
    if(!p_bnf->p_prv_lev[i]) {
      p_bnf->p_prv_lev[i] = be_vyrob_prvek_levelu();
      p_bnf->p_prv_lev[i]->prv_index = i;
      p_bnf->p_prv_lev[i]->p_prv_databaze = p_prv_databaze;
      p_bnf->p_prv_lev[i]->guid = p_prv_databaze->guid;
      if(p_prv_databaze->p_kont) {
        if((k = lo_najdi_volny_kontejner(p_cnf->p_kont, p_cnf->max_kont)) == K_CHYBA)
          chyba("Neni volny kontejner.");
        p_bnf->p_prv_lev[i]->mesh = k;
        p_cnf->p_kont[k] = kopiruj_kontejner(p_prv_databaze->p_kont,p_cnf->bnf.p_mat,p_cnf->p_mat,MAX_EDIT_MATERIALU,p_cnf->light,MAX_EDIT_SVETEL,p_cnf->p_dlight,MAX_FLARE_SVETEL,p_cnf->p_edlight,MAX_FLARE_SVETEL,p_cnf->flare,MAX_EDIT_SVETEL);
        strcat(p_cnf->p_kont[k]->jmeno,"##");
        p_cnf->p_kont[k]->prvek = i;
      }
      if(i >= p_bnf->prvnum)
        p_bnf->prvnum = i+1;
      return(p_bnf->p_prv_lev[i]);
    }
  }
  return(NULL);
}

int be_najdi_kontejner(EDIT_KONTEJNER **p_kont, int max, int id)
{
  int i;
  for(i = 0; i < max; i++) {
    if(p_kont[i] && p_kont[i]->kontejner_ID == id)
      return(i);
  }
  return(K_CHYBA);
}

int be_prvek_disk_do_bunky(B_KONFIG *p_bnf, PRVEK_LEVELU **p_lev, BUNKA_LEVELU_DISK *p_disk, int rekonstrukce)
{
  PRVEK_DATABAZE *p_prv_databaze;
  K_EDITOR *p_cnf = (K_EDITOR *)p_bnf->p_cnf;
  int guid = p_disk->guid;
  int i,p,mesh;

  // jakej-si bordel? - load nulovych meshu
  if(!rekonstrukce && p_disk->mesh == K_CHYBA) {
    assert(0);
    kprintf(TRUE,"+prvek '-1' guid %d",p_disk->guid);
    p_disk->guid = 0;
    p_lev = NULL;
    return(TRUE);
  }
  
  if(!guid) {
    p_lev = NULL;
    return(TRUE);
  }

  *p_lev = NULL;

dalsi_kolo:;

  for(i = 0; i < p_bnf->prvku_databaze; i++) {
    if(p_bnf->p_prv_dat[i] && guid == p_bnf->p_prv_dat[i]->guid) {
      p_prv_databaze = p_bnf->p_prv_dat[i];
      for(p = 0; p < MAX_PRVKU_LEVEL; p++) {
        if(!p_bnf->p_prv_lev[p]) {
          kprintf(TRUE, " + prvek guid '%d' mesh %d %s",guid,p_disk->mesh,p_cnf->p_kont[p_disk->mesh]->jmeno);

          p_bnf->p_prv_lev[p] = be_vyrob_prvek_levelu();
          p_bnf->p_prv_lev[p]->prv_index = p;
          p_bnf->p_prv_lev[p]->p_prv_databaze = p_prv_databaze;
          
          if(rekonstrukce) {
            p_bnf->p_prv_lev[p]->mesh = p_disk->mesh = mesh = lo_najdi_volny_kontejner(p_cnf->p_kont, KONT_NUM);
            if(mesh == K_CHYBA) {
              chyba("Nedostatek volnych kontejneru...");
            }
            p_cnf->p_kont[mesh] = kopiruj_kontejner(p_prv_databaze->p_kont,p_cnf->bnf.p_mat,p_cnf->p_mat,MAX_EDIT_MATERIALU,p_cnf->light,MAX_EDIT_SVETEL,p_cnf->p_dlight,MAX_FLARE_SVETEL,p_cnf->p_edlight,MAX_FLARE_SVETEL,p_cnf->flare,MAX_EDIT_SVETEL);
            strcat(p_cnf->p_kont[mesh]->jmeno,"##");            
          } else {            
            mesh = be_najdi_kontejner(p_cnf->p_kont, KONT_NUM, p_disk->mesh);
            if(mesh == K_CHYBA) { 
              mesh = p_disk->mesh; // stara verze levelu bez id kontejneru
            }
            p_bnf->p_prv_lev[p]->mesh = mesh;            
          }

          if(mesh != K_CHYBA) {
            assert(p_cnf->p_kont[mesh]);
            p_cnf->p_kont[mesh]->prvek = p;
          }

          *p_lev = p_bnf->p_prv_lev[p];
          p_bnf->prvnum++;
          break;
        }
      }
      
      if(*p_lev) {
        (*p_lev)->Rotace = p_disk->Rotace;
        (*p_lev)->Vazba1 = p_disk->Vazba1;
        (*p_lev)->Vazba2 = p_disk->Vazba2;
      } else {
        kprintf(TRUE, " ! Neni volna pozice.");
      }
      return(TRUE);
    }
  }

  // guid nenalezeno v databazi
  // strcim tam to nejblizsi co mam
  {
    int min_prvek = K_CHYBA, min_rozdil = 0xffffffff;

    for(i = 0; i < p_bnf->prvku_databaze; i++) {
      if(p_bnf->p_prv_dat[i] && p_bnf->p_prv_dat[i]->guid/1000 == guid/1000 &&
         abs(p_bnf->p_prv_dat[i]->guid - guid) < min_rozdil) {
        min_rozdil = abs(p_bnf->p_prv_dat[i]->guid - guid);
        min_prvek = i;
        break;
      }
    }

    if(min_prvek != K_CHYBA) {
      guid = p_bnf->p_prv_dat[min_prvek]->guid;
      goto dalsi_kolo;
    }
  }


  *p_lev = NULL;
  return(FALSE);
}

int be_prvek_bunky_na_disk(B_KONFIG *p_bnf, PRVEK_LEVELU *p_lev, BUNKA_LEVELU_DISK *p_disk)
{
  K_EDITOR *p_cnf = (K_EDITOR *)p_bnf->p_cnf;
  if(p_lev) {
    p_disk->guid = p_lev->guid ? p_lev->guid : p_lev->p_prv_databaze->guid;
    
    p_disk->Rotace = p_lev->Rotace;
    p_disk->Vazba1 = p_lev->Vazba1;
    p_disk->Vazba2 = p_lev->Vazba2;
    
    p_disk->mesh = (p_lev->mesh != K_CHYBA && p_cnf->p_kont[p_lev->mesh]) ? p_cnf->p_kont[p_lev->mesh]->kontejner_ID : K_CHYBA;
  }
  else {
    memset(p_disk,0,sizeof(BUNKA_LEVELU_DISK));    
  }
  return(TRUE);
}

void be_preumisti_prvek(B_KONFIG *p_bnf, PRVEK_LEVELU *p_lev)
{
  int x,y,z,p;
  be_pozice_na_disku(p_lev->lev_index,x,y,z,p,p_bnf->x_num,p_bnf->z_num);
  be_umisti_prvek(p_bnf, p_lev, x, y, z);
  //...zde pridat perumisteni cile teleportu/vytahu...
}


void be_umisti_prvek(B_KONFIG *p_bnf, PRVEK_LEVELU *p_lev, int x, int y, int z)
{
  EDIT_KONTEJNER *p_kont;
  K_EDITOR *p_cnf = (K_EDITOR *)p_bnf->p_cnf;
  GLMATRIX *p_m;
  
  if(p_lev) {
    p_m = &p_lev->m;
    init_matrix(p_m);
    p_m->_41 = (float)(p_bnf->x_start+x*X_PRVEK+X_PRVEK/2);
    p_m->_42 = (float)(p_bnf->y_start+y*Y_PRVEK);
    p_m->_43 = (float)(p_bnf->z_start+z*Z_PRVEK+Z_PRVEK/2);
    if(p_lev->Rotace)
      rotate_matrix_y(p_m,-(p_lev->Rotace*PI/2));
    
    p_lev->lev_index = be_pozice_v_levelu(p_bnf, x, y, z, ko_GetLayer(p_lev->p_prv_databaze->guid));
    
    p_kont = p_cnf->p_kont[p_lev->mesh];
    while(p_kont) {
      p_kont->mpmatrix = *p_m;
      p_kont->kflag |= KONT_MPMATRIX;
      if(p_kont->kflag&KONT_KEYFRAME)
        key_kontejner_sim_reanimuj(p_kont);
      p_kont = p_kont->p_next;
    }
  }
}

/*
  Load levelu z disku
*/
int be_load_level(B_KONFIG *p_bnf, byte *p_file, byte *p_dir, int rekonstrukce)
{
  EDIT_KONTEJNER *p_kont;
  K_EDITOR *p_cnf = (K_EDITOR *)p_bnf->p_cnf;
  BUNKA_LEVELU_DISK bnk;
  LEVEL_HEADER hdr;
  PRVEK_LEVELU **p_lev;   // level
  FILE *f;
  int  x,y,z,p,i,max;
  
  chdir(p_dir);

  kprintf(TRUE,"Load level '%s'",p_file);
  if((f = fopen(p_file,"rb")) == NULL)
    return(FALSE);

  fread(&hdr,sizeof(LEVEL_HEADER),1,f);  
  
  be_zrus_level(p_bnf);
  p_lev = be_vyrob_level(p_bnf, hdr.x, hdr.y, hdr.z);
  p_bnf->x_start = _finite(hdr.x_start) ? hdr.x_start : 0.0f;
  p_bnf->y_start = _finite(hdr.y_start) ? hdr.y_start : 0.0f;
  p_bnf->z_start = _finite(hdr.z_start) ? hdr.z_start : 0.0f;

  // ted nejakej dobrej cyklus na nacteni celeho levelu
  max = hdr.x*hdr.y*hdr.z*2;
  for(i = 0; i < max; i++) {
    fread(&bnk,sizeof(BUNKA_LEVELU_DISK),1,f);
    if(be_prvek_disk_do_bunky( p_bnf, p_lev+i, &bnk, rekonstrukce)) {
      if(bnk.guid) {
        be_pozice_na_disku(i,x,y,z,p,hdr.x,hdr.z);
        kprintf(TRUE,"Pozice [%d %d %d]",x,y,z);
        be_umisti_prvek( p_bnf, p_lev[i], x, y, z);
        p_kont = p_cnf->p_kont[p_lev[i]->mesh];
        if(!(p_kont->kflag&KONT_KEYFRAME))
          init_matrix(&p_kont->world);        
      }
    } else {      
      kprintf(TRUE,"Prvek GUID = %d Nenalzen v databazi prvku !!!!!",bnk.guid);
    }
  } 
  fclose(f);
  return(TRUE);
}

/*
  Ulozeni levelu na disk
*/
int be_save_level(B_KONFIG *p_bnf, byte *p_file, byte *p_dir)
{   
  K_EDITOR *p_cnf = (K_EDITOR *)p_bnf->p_cnf;
  BUNKA_LEVELU_DISK bnk;
  LEVEL_HEADER hdr;
  PRVEK_LEVELU **p_lev = p_bnf->p_lev;
  FILE *f;
  int  i,max;

  if(!p_lev)
    return(FALSE);
  
  chdir(p_dir);

  if((f = fopen(p_file,"wb")) == NULL) {
//    kprintf(TRUE,"Nepodarilo se otevrit soubor %s pro zapis !!! chyba %s",p_file,strerror(errno));
    return(FALSE);
  }

  memset(&hdr,0,sizeof(LEVEL_HEADER));
  hdr.magic = (dword)'VL2B';
  
  hdr.x = p_bnf->x_num; 
  hdr.y = p_bnf->y_num; 
  hdr.z = p_bnf->z_num;
  
  hdr.x_start = p_bnf->x_start;
  hdr.y_start = p_bnf->y_start;
  hdr.z_start = p_bnf->z_start;
  
  hdr.prvku = be_spocitej_prvky(p_cnf);
  fwrite(&hdr,sizeof(LEVEL_HEADER),1,f);
     
  // ted nejakej dobrej cyklus na save celeho levelu
  // 
  max = hdr.x*hdr.y*hdr.z*2;
  for(i = 0; i < max; i++) {
    be_prvek_bunky_na_disk( p_bnf, p_lev[i], &bnk);
    fwrite(&bnk,sizeof(BUNKA_LEVELU_DISK),1,f);
  }    
  fclose(f);
  return(TRUE);
}

/*
  Nahraje pozadi k levelu
*/
int be_nahraj_level_pozadi(K_EDITOR *p_cnf, byte *p_file, int textury)
{
  return(oe_nahraj_projekt(p_cnf,p_file,p_cnf->dir.level_dir,FALSE,textury));
}

/*
  Ulozi pozadi k levelu
*/
void be_uloz_level_pozadi(K_EDITOR *p_cnf, byte *p_file)
{
  oe_updatuj_scenu_stat(p_cnf);
  
  if(p_cnf->stat.kontejneru) {
    oe_oznac_pouzite_materialy(p_cnf);
    oe_uloz_projekt(p_cnf,p_file,p_cnf->dir.level_dir,TRUE,FALSE);
  } else {
    ddw("Neuklada se prazdny level ! -> smaz si to rucne !");
  }
}

/*
   Optimalizuje level
*/
void be_optimalizuj_level(K_EDITOR *p_cnf)
{


}