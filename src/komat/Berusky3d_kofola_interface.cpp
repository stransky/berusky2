/*
  Kofolovo rozhrani - C
*/

#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "3d_all.h"

#include "Object.h"

#include "Berusky_universal.h"
#include "Berusky3d_castice.h"
#include "Berusky3d.h"
#include "Berusky3d_ini.h"
#include "Berusky3d_load.h"
#include "Berusky3d_render.h"
#include "Berusky3d_light.h"
#include "Berusky3d_animace.h"
#include "Berusky3d_kamery.h"
#include "Berusky3d_castice2.h"
#include "Berusky3d_kofola_interface.h"

extern G_KONFIG ber, *p_ber;

float kom_load_progres; // aktualni tick

/*
  Load levelu
*/
void kom_init_level(LEVEL_HEADER *p_head) // level header
{
  ber_zrus_level(p_ber);
  ber_vyrob_level(p_ber, p_head->x, p_head->y, p_head->z);
  
  p_ber->x_start = finite(p_head->x_start) ? p_head->x_start : 0.0f;
  p_ber->y_start = finite(p_head->y_start) ? p_head->y_start : 0.0f;
  p_ber->z_start = finite(p_head->z_start) ? p_head->z_start : 0.0f;
  
  p_ber->level_min.x = p_ber->x_start;
  p_ber->level_min.y = p_ber->y_start;
  p_ber->level_min.z = p_ber->z_start;
  
  p_ber->level_max.x = p_ber->x_start+p_head->x*X_PRVEK;
  p_ber->level_max.y = p_ber->y_start+p_head->y*Y_PRVEK;
  p_ber->level_max.z = p_ber->z_start+p_head->z*Z_PRVEK;

  p_ber->hitnum = p_head->x*p_head->y*p_head->z;
  p_ber->p_hit_list = mmalloc(sizeof(p_ber->p_hit_list[0])*p_ber->hitnum);

  p_ber->frame = 0;
}

/* Vraci handle meshe - vola kofola
*/
MeshHandle kom_pridej_prvek_levelu(BUNKA_LEVELU_DISK *p_bunka, int x, int y, int z)
{
  int  handle = K_CHYBA;
  int  mesh;  
  
  ber_prvek_disk_do_bunky( p_ber, p_bunka, &handle, TRUE, x, y, z);  
  mesh = p_ber->p_prv_lev[handle]->mesh;
  if(!p_ber->p_prv_lev[handle]->staticky && (mesh == K_CHYBA || !p_ber->p_mesh[mesh])) {
    kprintf(TRUE,"---- Error loading mesh number %d",mesh);
  }
  return(handle);
}

/* Loadne prvek levelu z disku
*/
MeshHandle kom_pridej_prvek_levelu_disk(byte *p_file, BUNKA_LEVELU_DISK *p_bunka, int x, int y, int z)
{
  int  handle = K_CHYBA;
  int  mesh;  
  int  l;
  
  l = lo_najdi_volny_mesh(p_ber->p_mesh,MAX_BERUSKY_KONTEJNERU);
  if(l == K_CHYBA)
    return(K_CHYBA);

  if(!ber_nahraj_mesh(p_ber, p_file, p_ber->p_mesh+l))
    return(K_CHYBA);
  if(l >= p_ber->meshnum)
    p_ber->meshnum = l+1;

  p_bunka->mesh = l;

  ber_prvek_disk_do_bunky( p_ber, p_bunka, &handle, TRUE, x, y, z);  
  mesh = p_ber->p_prv_lev[handle]->mesh;
  if(!p_ber->p_prv_lev[handle]->staticky && (mesh == K_CHYBA || !p_ber->p_mesh[mesh])) {
    kprintf(TRUE,"---- Error loading mesh number %d",mesh);
  } else {
    ber_dynamic_mesh_list_vloz_mesh(p_ber,mesh);
  }

  return(handle);
}

/* Pridani meshe do levelu
*/
MeshHandle kom_pridej_mesh_do_levelu(char *p_jmeno)
{
  int handle = K_CHYBA,kont;
  
  kont = kom_najdi_mesh(p_jmeno);
  if(kont == K_CHYBA || !p_ber->p_mesh[kont])
    return(K_CHYBA);
  else {
    ber_mesh_do_bunky(p_ber, kont, &handle);
    return(handle);
  }
}

/* Nastavi pivot point meshe (0,0,0) je default
*/
int kom_mesh_set_pivot(MeshHandle prvek_handle, float x, float y, float z)
{
  PRVEK_LEVELU_GAME *p_prv = p_ber->p_prv_lev[prvek_handle];
  if(p_prv) {
    GLMATRIX m;
    memcpy(&m,&p_ber->p_mesh[p_prv->mesh]->m,sizeof(m));
    m._41 = m._42 = m._43 = 0.0f;
    transformuj_bod_matici(&x,&y,&z,&m);
    p_prv->pivot.x = x;
    p_prv->pivot.y = y;
    p_prv->pivot.z = z;
    return(prvek_handle);
  } else {
    return(K_CHYBA);
  }
}

/* Cte pivot meshe
*/
int kom_mesh_get_pivot(MeshHandle prvek_handle, BOD *p_pivot)
{
  PRVEK_LEVELU_GAME *p_prv = p_ber->p_prv_lev[prvek_handle];
  if(p_prv) {
    GAME_MESH *p_mesh = p_ber->p_mesh[p_prv->mesh];
    *p_pivot = p_mesh->kofola_pivot;
    return(TRUE);
  } else {
    return(FALSE);
  }
}

/* Nastavi pivot point pomoci bodu
*/
int kom_mesh_set_pivot_bod(MeshHandle prvek_handle, BOD *p_bod)
{
  PRVEK_LEVELU_GAME *p_prv = p_ber->p_prv_lev[prvek_handle];
  if(p_prv) {
    BOD p = *p_bod;
    GLMATRIX m;
    memcpy(&m,&p_ber->p_mesh[p_prv->mesh]->m,sizeof(m));
    m._41 = m._42 = m._43 = 0.0f;
    transformuj_bod_matici(&p.x,&p.y,&p.z,&m);
    p_prv->pivot = p;
    return(prvek_handle);
  } else {
    return(K_CHYBA);
  }
}

/* resetuje matici rotace meshe + prepocita mesh na urcenou pozici
   (ten na teto pozici musi byt, jinak bude spatne umisteny)
*/
int kom_mesh_reset_pr(MeshHandle prvek_handle, int x, int y, int z, int rotace)
{
  if(p_ber->p_prv_lev[prvek_handle]) {    
    PRVEK_LEVELU_GAME *p_prv = p_ber->p_prv_lev[prvek_handle];
    GAME_MESH *p_mesh = p_ber->p_mesh[p_prv->mesh];
    GLMATRIX   m1, m2, m3;

    invert_matrix(ber_position_matrix(p_ber,&m1,x,y,z,rotace),&m2);
    mat_mult(&p_mesh->m,&m2,&m3);
    transformuj_mesh_matici(p_mesh,&m3);
    
    memcpy(&p_mesh->m,&m1,sizeof(GLMATRIX));
    memcpy(&p_prv->mp,&m1,sizeof(GLMATRIX));

    ber_position_level(p_prv,&m1,rotace);

    return(prvek_handle);
  } else {
    return(K_CHYBA);
  }
}

/* Nastavi flag meshe ze se nema testovat kolize s timto meshem
*/
int kom_mesh_set_no_test(MeshHandle prvek_handle)
{
  PRVEK_LEVELU_GAME *p_prv = p_ber->p_prv_lev[prvek_handle];
  GAME_MESH *p_mesh;
  if(p_prv && p_prv->mesh != K_CHYBA) {
    p_mesh = p_ber->p_mesh[p_prv->mesh];
    p_mesh->p_data->kflag |= KONT_DRAW_NOBALKA;
    return(prvek_handle);
  } else {
    return(K_CHYBA);
  }
}

/* Nacte mesh - cislo pro ukladani
*/
int kom_mesh_get_save_num(MeshHandle prvek_handle)
{
  if(p_ber->p_prv_lev[prvek_handle]) {
    return(p_ber->p_prv_lev[prvek_handle]->mesh);
  } else {
    return(K_CHYBA);
  }
}


/* Nacte mesh - fyzickou pozici
*/
int kom_mesh_get_float(MeshHandle prvek_handle, float *p_x, float *p_y, float *p_z, int *p_rotace)
{
  if(p_ber->p_prv_lev[prvek_handle]) {
    PRVEK_LEVELU_GAME *p_prv = p_ber->p_prv_lev[prvek_handle];
    *p_x = p_prv->x;
    *p_y = p_prv->y;
    *p_z = p_prv->z;
    *p_rotace = p_prv->rotace;    
    return(prvek_handle);
  } else {
    return(K_CHYBA);
  }
}

/* Nacte mesh - logickou pozici
*/
int kom_mesh_get_int(MeshHandle prvek_handle, int *p_x, int *p_y, int *p_z, int *p_rotace)
{
  if(p_ber->p_prv_lev[prvek_handle]) {
    PRVEK_LEVELU_GAME *p_prv = p_ber->p_prv_lev[prvek_handle];
    *p_x = ftoi((p_prv->x-p_ber->x_start-X_PRVEK/2)/X_PRVEK);
    *p_y = ftoi((p_prv->y*2.0f-p_ber->y_start-Y_PRVEK/2)/Y_PRVEK);
    *p_z = ftoi((p_prv->z-p_ber->z_start-Z_PRVEK/2)/Z_PRVEK);
    *p_rotace = p_prv->rotace;
    return(prvek_handle);
  } else {
    return(K_CHYBA);
  }
}

/* Prida prvek do databaze
*/
DataHandle kom_pridej_prvek_do_databaze(SECONDDATADESC *p_sec)
{
  int handle;
  
  if(p_ber->mdatanum >= MAX_BERUSKY_DATABAZE)
    return(K_CHYBA);
  else    
    handle = p_ber->mdatanum++;
  ber_nahraj_mesh(p_ber, p_sec->File_Name[0], p_ber->p_mdata+handle);  
  return(handle);
}

MeshHandle kom_vloz_prvek_z_databaze(DataHandle data_handle, int x, int y, int z, int rotace, int guid)
{
  GAME_MESH_DATA *p_data;
  BUNKA_LEVELU_DISK disk;
  MeshHandle handle = K_CHYBA;
  int m;

  m = disk.mesh = lo_najdi_volny_mesh(p_ber->p_mesh,MAX_BERUSKY_KONTEJNERU);
  
  if(m == K_CHYBA)
    return(K_CHYBA);
  
  if(m >= p_ber->meshnum)
    p_ber->meshnum = m+1;
    
  assert(p_ber->p_mdata[data_handle]); // source ok?
  p_ber->p_mesh[m] = kopiruj_mesh(p_ber->p_mdata[data_handle],NULL);
  assert(p_ber->p_mesh[m]); // dest ok?

  ber_dynamic_mesh_list_vloz_mesh(p_ber, m);
  
  p_data = p_ber->p_mesh[m]->p_data;
  lo_kopiruj_svetla_do_sceny_mesh(p_data,p_ber->p_dlight,MAX_FLARE_SVETEL,p_ber->p_edlight,MAX_FLARE_SVETEL);
  if(p_data->p_ldlight)
    p_ber->dl_lightnum = lo_najdi_prepocitej_dsvetlo(p_ber->p_dlight, MAX_FLARE_SVETEL);
  if(p_data->p_lelight)
    p_ber->edl_lightnum = lo_najdi_prepocitej_extra_svetlo(p_ber->p_edlight, MAX_FLARE_SVETEL);

  disk.guid = guid;
  disk.Rotace = rotace;
  disk.Vazba1 = disk.Vazba2 = 0;

  ber_prvek_disk_do_bunky(p_ber, &disk, &handle, FALSE, x, y, z);
  ber_umisti_prvek(handle, x, y, z, rotace);
  return(handle);
}

void kom_zrus_level(int restart)
{
  int i;  

  kprintf(TRUE,"Kom_zrus_level - start...");
  /* Zrusi casticovy systemy
  */
  {
    PARMETAC *p_par = p_ber->p_par;
    PARMETAC *p_next;
    while(p_par) {
      p_next = p_par->p_next;
      par_zrus_end(p_par);
      p_par = p_next;
    }
  }
  p_ber->p_par = NULL;

  kprintf(TRUE,"Kom_zrus_level - level...");

  /* Zrus level
  */
  for(i = 0; i < MAX_PRVKU_LEVEL; i++) {
    if(p_ber->p_prv_lev[i]) {
      free(p_ber->p_prv_lev[i]);
      p_ber->p_prv_lev[i] = NULL;
    }
  }
  p_ber->prvnum = 0;
  memset(p_ber->p_prv_lev,0,sizeof(p_ber->p_prv_lev[0])*MAX_PRVKU_LEVEL);

  kprintf(TRUE,"Kom_zrus_level - poly...");

  /* Zrus poly
  */
  if(p_ber->p_poly) {
    for(i = 0; i < p_ber->polynum; i++) {
      zrus_vnitrek_poly(p_ber->p_poly+i);
    }
    free(p_ber->p_poly);
    p_ber->p_poly = NULL;    
  }
    
  kprintf(TRUE,"Kom_zrus_level - mesh...");

  /* Zrus meshe
  */
  for(i = 0; i < p_ber->meshnum; i++) {
    if(p_ber->p_mesh[i])
      zrus_mesh(p_ber->p_mesh+i);
  }

  kprintf(TRUE,"Kom_zrus_level - databaze...");

  /* Zrus meshe v databazi
  */
  for(i = 0; i < MAX_BERUSKY_DATABAZE; i++) {
    if(p_ber->p_mdata[i]) {
      lo_vymaz_svetla_mesh(p_ber->p_mdata[i]->p_data);
      zrus_mesh(p_ber->p_mdata+i);
    }
  }

  if(!restart) {

    kprintf(TRUE,"Kom_zrus_level - materialy...");

    /* Zrus materiay + smaterial
    */
    for(i = 0; i < MAX_CELKEM_MATERIALU; i++) {
      if(p_ber->p_mat[i])
        zrus_material(p_ber->p_mat+i);
    }  
    p_ber->p_smat = NULL;

    kprintf(TRUE,"Kom_zrus_level - textury...");

    /* Zrus textury
    */
    lo_smaz_textury(p_ber->p_text,MAX_CELKEM_TEXTUR);
    memset(p_ber->p_text,0,sizeof(p_ber->p_text[0])*MAX_CELKEM_TEXTUR);    
  }

  kprintf(TRUE,"Kom_zrus_level - lightmapy...");

  /* Zrus lightmapy
  */
  for(i = 0; i < MAX_RAY_TEXTUR; i++) {
    if(p_ber->p_lightmap[i].load) {
      txt_zrus_lightmapu(p_ber->p_lightmap+i);
    }
  }
  memset(p_ber->p_lightmap,0,sizeof(p_ber->p_lightmap[0])*MAX_CELKEM_TEXTUR);

  kprintf(TRUE,"Kom_zrus_level - mlho-kostky...");

  /* Zrus mlho-kostky
  */
  p_ber->p_mlha = zrus_mlhokostku_all(p_ber->p_mlha);
  p_ber->p_mlha = p_ber->p_mlha_akt = NULL;

  kprintf(TRUE,"Kom_zrus_level - fleky...");

  /* Zrus fleky
  */
  zrus_fleky(&p_ber->p_fleky);
  zrus_fleky(&p_ber->p_fleky_mesh);

  kprintf(TRUE,"Kom_zrus_level - flare...");

  /* Zrus flare
  */
  zrus_flare(&p_ber->p_flare);

  kprintf(TRUE,"Kom_zrus_level - voda...");

  /* Zrusi vodu
  */
  vod_zrus(&p_ber->p_voda);

  kprintf(TRUE,"Kom_zrus_level - animace textur...");

  /* Zrusi vsechny koordinatovy animace textur
  */
  tani_zrus(&p_ber->p_textanim);

  kprintf(TRUE,"Kom_zrus_level - animace kamer...");

  /* Animace kamer
  */
  for(i = 0; i < KAM_MAX_ANIM; i++) {
    key_kamera_zrus_indir(p_ber->kamery+i);
  }
  memset(p_ber->kamery,0,sizeof(p_ber->kamery[0])*KAM_MAX_ANIM);
  p_ber->kamnum = 0;

  kprintf(TRUE,"Kom_zrus_level - stat svetla...");

  /* Zrusi staticky svetla
     int                  slightnum;
     STATIC_LIGHT         p_slight[MAX_FLARE_SVETEL];
  */
  p_ber->slightnum = 0;
  memset(p_ber->p_slight,0,sizeof(p_ber->p_slight[0])*MAX_FLARE_SVETEL);

  kprintf(TRUE,"Kom_zrus_level - dyn svetla...");

  /* Zrusi dynamicky svetla
  */
  for(i = 0; i < MAX_FLARE_SVETEL; i++) {
    if(p_ber->p_dlight[i].akt) {
      sdla_animaci_zrus_online(&p_ber->p_dlight[i].an);
    }
  }
  memset(p_ber->p_dlight,0,sizeof(p_ber->p_dlight[0])*MAX_FLARE_SVETEL);

  kprintf(TRUE,"Kom_zrus_level - extra-dyn svetla...");

  /* Extra-dynamicky svetla
  */
  for(i = 0; i < MAX_FLARE_SVETEL; i++) {
    if(p_ber->p_edlight[i].akt) {
      sdla_animaci_zrus_online(&p_ber->p_edlight[i].an);
    }
  }
  memset(p_ber->p_edlight,0,sizeof(p_ber->p_edlight[0])*MAX_FLARE_SVETEL);

  kprintf(TRUE,"Kom_zrus_level - sim-animaci...");

  /* Zruseni zbyvajicich sim/track animaci
  */
  for(i = 0; i < MAX_SIM_ANIMACI; i++) {
    if(p_ber->p_sim[i].keynum) {
      key_sim_zrus_vnitrek(p_ber->p_sim+i);
    }
    if(p_ber->p_track[i].endtime) {
      zrus_tri_vnitrek(p_ber->p_track+i);
    }
  }
  memset(p_ber->p_sim,0,sizeof(p_ber->p_sim[0])*MAX_SIM_ANIMACI);
  memset(p_ber->p_track,0,sizeof(p_ber->p_track[0])*MAX_SIM_ANIMACI);

  kprintf(TRUE,"Kom_zrus_level - hit-list...");

  /*
    Zruseni hit-listu
  */
  free(p_ber->p_hit_list);
  p_ber->p_hit_list = NULL;
  p_ber->hitnum = FALSE;

  kprintf(TRUE,"Kom_zrus_level - zrcadla...");

  /* Smaz zrcadla pokud jsou
  */
  {
    ZDRCADLO_DESC_POLY *p_poly = p_ber->zrc.p_poly,*p_tmp;
    while(p_poly) {
      p_tmp = p_poly->p_next;
      free(p_poly);
      p_poly = p_tmp;
    }
    p_ber->zrc_akt = FALSE;
    memset(&p_ber->zrc,0,sizeof(p_ber->zrc));
  }

  kprintf(TRUE,"Kom_zrus_level - obb a renderlist...");

  /* Smaze obalky + poly render list
  */
  obbtree_zrus(&p_ber->obbtree);
  memset(&p_ber->obbtree,0,sizeof(p_ber->obbtree));
  ber_poly_render_list_zrus(p_ber);
  
  memset(p_ber->p_dyn_meshlist,0,sizeof(p_ber->p_dyn_meshlist[0])*MAX_BERUSKY_KONTEJNERU);
  memset(p_ber->p_mesh_renderlist,0,sizeof(p_ber->p_mesh_renderlist[0])*MAX_BERUSKY_KONTEJNERU);

  kprintf(TRUE,"Kom_zrus_level - fronta animaci...");

  /* Smazu frontu animaci
  */
  memset(p_ber->p_lokal,0,sizeof(p_ber->p_lokal[0])*MAX_RUN_ANIMACI);
  p_ber->lokalnum = 0;

  kprintf(TRUE,"Kom_zrus_level - anim materialu...");

  /* Animace materialu
  */
  memset(&p_ber->p_man,0,sizeof(p_ber->p_man[0])*MAX_MAT_ANIMACI); // materialove animace
  p_ber->mannum = 0;

  kprintf(TRUE,"Kom_zrus_level - gkanimace...");

  /* Animace vseho
  */
  p_ber->p_gkanim = NULL;
  p_ber->gknum = 0;
  memset(p_ber->p_gkzas,0,sizeof(p_ber->p_gkzas[0])*MAX_RUN_ANIMACI);
  
  kprintf(TRUE,"Kom_zrus_level - animace chapadel...");

  /* Animace chapadel
  */
  chani_zrus_all(p_ber);

  kprintf(TRUE,"Kom_zrus_level - kofolovy cary...");

  /* Kofolovy cary - ven
  */
  p_ber->p_cary = NULL;
  p_ber->mail = 0;

  /* Vymaznu vse co tady mam
  */
  p_ber->x_num = p_ber->y_num = p_ber->z_num = 0;
  p_ber->dyn_meshlistnum = 0;
  p_ber->mesh_renderakt = 0;
  p_ber->mesh_rendernum = 0;
  p_ber->meshnum = 0;
  p_ber->polynum = 0;
  p_ber->anim_mail = 0;
  p_ber->simnum = 0;
  p_ber->dl_recalc = p_ber->dl_new = p_ber->dl_lightnum = p_ber->dl_on = 0;  
 
  kprintf(TRUE,"Kom_zrus_level - all done...");
}

/* 
  Vypne zrcadlo pokud je zrcadlo vypnute - vypni zrcadlo
*/
int ber_test_zrcadla(void)
{
  ZDRCADLO_DESC_POLY *p_poly = p_ber->zrc.p_poly;
  int mat;
  
  // Ma se vypnout zrcadlo
  if(p_ber->zrc_akt && !p_ber->conf_zrcadlo) {
    p_ber->zrc_akt = FALSE;
    while(p_poly) {
      if(p_poly->poly != K_CHYBA && p_ber->polynum > p_poly->poly) {
        p_ber->p_poly[p_poly->poly].kflag &= ~KONT_ZRCADLO;
        p_ber->p_poly[p_poly->poly].m1flag &= ~MAT_PRUHLEDNY;
        
        mat = p_ber->p_poly[p_poly->poly].material;
        p_ber->p_mat[mat]->alfa_state = FALSE;
        
        p_poly->poly = K_CHYBA;
      } else  if(p_ber->polynum <= p_poly->poly) {
        p_poly->poly = K_CHYBA;
      }
      p_poly = p_poly->p_next;
    }
    return(TRUE);
  } else
    return(FALSE);
}

void kom_load_sys_material(int i)
{
 byte  file[200];
 byte  pom[200];
 
 sprintf(pom,"system_material_%d",i);
 GetPrivateProfileString("game",pom,"",file,200,ini_file);
 if(file[0]) {
   kprintf(1,"Load system materials '%s' from %s....",pom,file);
   if(!ber_nahraj_materialy(p_ber,file,p_ber->dir.material_dir))
     kprintf(1,"---- Fail load file %s....",file);
 }
}

void kom_posun_slider(void)
{
  if(p_ber->conf_load_tik < p_ber->conf_load_tiku && p_ber->p_tik_fce) {
    p_ber->p_tik_fce();
    p_ber->conf_load_tik += p_ber->conf_load_krok;
  }
}



int ber_vyber_berusky(BUNKA_LEVELU_DISK *p_disk, int num, BUNKA_LEVELU_DISK *p_berusky)
{
  int i,berusek = 0;
  
  for(i = 0; i < num; i++, p_disk++) {
    if(ber_je_objekt_beruska(p_disk->guid)) {
      *p_berusky++ = *p_disk;
      berusek++;
    }
  }  
  
  if(berusek > 5) {
    kprintfe(TRUE,"Mas tam moc berusek!!! %d",berusek);
  }
  
  return(berusek);
}

// chdir - TRUE - nastavit adresar
void kom_load_level(byte *p_file, int zmen_dir, int restart, BUNKA_LEVELU_DISK *p_bunka, int bunek)
{
 BUNKA_LEVELU_DISK berusky[5];
 int               berusek;
 GAME_MESH **p_slist;
 int         snum;
 byte  dir[200];
 byte  file[200];
 int   vel1,vel2,vel3,vel4,ret,i,facu,vertexu;
 int  *p_ind;
 dword time;

 berusek = ber_vyber_berusky(p_bunka, bunek, berusky);

 kprintf(1,"Loading level '%s'",p_file);
 
 kom_posun_slider();

 // Load systemovych materialu
 getcwd(dir,200);
 
 for(i = 0; i < 10; i++)
   kom_load_sys_material(i);
 
 if(zmen_dir) {
   chdir(p_ber->dir.game_level_dir);
   strcpy(dir,p_file);
   zamen_koncovku(dir,"");   
 } 

 chdir(dir);
 getcwd(dir,200);
 strcpy(file,p_file);
 
 kom_posun_slider();

 kprintf(1,"Level directory '%s'",dir); 
 // leveldir+jmenobez koncovky
 // jmeno

 kprintf(1,"Load materials....");
 if(!ber_nahraj_materialy(p_ber,file,dir)) {
   kprintf(TRUE,"Error loading level materials %s from directory %s, stop loading..",file,dir);
   konec(TRUE);
 }

 kom_posun_slider();
 
 time = timeGetTime();

 kprintf(1,"Load scene - dynamic items....");
 ret = ber_nahraj_scenu(p_ber,file,dir,restart,berusky,berusek); // scena (dynamicke veci)
 kprintfl(TRUE,"Scene load ret %d",ret);
 if(!ret) {
   kprintf(TRUE,"Error loading scene %s from directory %s, stop loading...",file,dir);
   konec(TRUE);
 } else {
   ber_dynamic_mesh_list_vyrob(p_ber);
 }

 kprintf(TRUE,"time %d",timeGetTime()-time);

 kom_posun_slider();

 kprintf(1,"Load scene - static items....");
 ber_nahraj_poly(p_ber,file,dir);         // poly (staticke veci)

 kom_posun_slider();

 if(ber_test_zrcadla())
   kprintf(TRUE,"Mirror off");
 else
   kprintf(TRUE,"Mirror on");


 time = timeGetTime();
 
 kprintf(1,"Load scene - static items - OBB Tree...");
 p_slist = ber_mesh_vyber_staticke(p_ber, &snum, &p_ind);
 obbtree_vyrob(&p_ber->obbtree, p_ber->p_poly, p_ber->polynum, p_slist, snum, p_ind, p_ber->p_mat);
 null_free((void **)&p_slist);
 null_free((void **)&p_ind);
 
 kprintf(TRUE,"time %d",timeGetTime()-time);

 kom_posun_slider();

 kprintf(1,"Load scene - lightmaps...."); 
 ber_nahraj_lightmap(p_ber,file,dir);
 
 time = timeGetTime();
 if(!restart) {
   int bump = hwconf.bump_mapping && p_ber->conf_dyn_light && p_ber->conf_dyn_rychlost == LIGHT_ALL;
   lo_reload_textur(&p_ber->tdir, p_ber->p_text, MAX_CELKEM_TEXTUR, bump);
   kom_posun_slider();
   if(bump) {
     ber_uprava_materialu_bump_mapping(p_ber);
     lo_reload_textur(&p_ber->tdir, p_ber->p_text, MAX_CELKEM_TEXTUR, FALSE);
     lo_smaz_textury_bmp(p_ber->p_text,MAX_CELKEM_TEXTUR);
   }
 } else {
   lo_reload_textur(&p_ber->tdir, p_ber->p_text, MAX_CELKEM_TEXTUR, FALSE);
 }
 lo_reload_stage(p_ber->p_mat,MAX_CELKEM_MATERIALU);
 kprintf(TRUE,"time %d",timeGetTime()-time);

 kom_posun_slider();
 
 kprintf(1,"Static lights  %d",p_ber->slightnum);
 
 vel1 = (lo_velikost_textur(p_ber->p_text, MAX_CELKEM_TEXTUR))/8000;
 vel2 = (lo_velikost_textur(p_ber->p_lightmap, MAX_RAY_TEXTUR))/8000;
 facu = 0; vertexu = 0;
 vel3 = lo_velikost_meshu(p_ber->p_mesh, p_ber->meshnum, &facu, &vertexu)/1000;
 vel4 = lo_velikost_poly(p_ber->p_poly, p_ber->polynum, &facu, &vertexu)/1000;
 
 kprintf(1,"Size of mesh vert. %d KB",vel3);
 kprintf(1,"Size of poly vert. %d KB",vel4);
 kprintf(1,"Size of textures %d KB",vel1);
 kprintf(1,"Size of lightmaps %d KB",vel2);
 kprintf(1,"Size of all %d KB",vel1+vel2);
 kprintf(1,"Vertex-num %d",vertexu);
 kprintf(1,"Face-num %d",facu);

 ber_velikost_sceny(p_ber);
 kom_posun_slider(); 
}

void kom_post_init_level(void)
{
 ber_init_level(p_ber);
}

/* Zruseni meshe
*/  
void kom_zrus_prvek(MeshHandle prvek_handle)
{
  PRVEK_LEVELU_GAME *p_prv = p_ber->p_prv_lev[prvek_handle];
  int                mh = p_prv->mesh;
  GAME_MESH_DATA    *p_data;

  if(p_prv) {
    if(mh != K_CHYBA) {      
      p_data = p_ber->p_mesh[mh]->p_data;
      lo_vymaz_svetla_ze_sceny_mesh(p_data);
      if(p_data->p_ldlight)
        p_ber->dl_lightnum = lo_najdi_prepocitej_dsvetlo(p_ber->p_dlight, MAX_FLARE_SVETEL);
      if(p_data->p_lelight)
        p_ber->edl_lightnum = lo_najdi_prepocitej_extra_svetlo(p_ber->p_edlight, MAX_FLARE_SVETEL);
      zrus_mesh(p_ber->p_mesh+mh);
    }
    free(p_ber->p_prv_lev[prvek_handle]);
    p_ber->p_prv_lev[prvek_handle] = NULL;
  }
}

float kom_get_framerate(void)
{
 return(p_ber->fps);
}

void kom_klavesy_hra(void)
{
 ber_klavesy_hra(&ber);
}

BOD * kom_get_fyz_souradnice(int x, int y, int z, BOD *p_bod)
{
  p_bod->x = (float)(p_ber->x_start+x*X_PRVEK+X_PRVEK/2);
  p_bod->y = (float)(p_ber->y_start+(y>>1)*Y_PRVEK);  
  p_bod->z = (float)(p_ber->z_start+z*Z_PRVEK+Z_PRVEK/2);
  return(p_bod);
}

/* Hledaci funkce
*/
MatHandle kom_najdi_material(byte *p_jmeno)
{
  int i;
  for(i = 0; i < MAX_CELKEM_MATERIALU; i++) {
    if(p_ber->p_mat[i] && !strcmp(p_ber->p_mat[i]->jmeno,p_jmeno))
      return(i);
  }
  return(K_CHYBA);
}

EDIT_MATERIAL * kom_preloz_material(MatHandle mh)
{
 return(p_ber->p_mat[mh]);
}

MeshHandle kom_najdi_mesh_prvek(byte *p_jmeno)
{
  PRVEK_LEVELU_GAME *p_prv;
  int i;

  for(i = 0; i < p_ber->prvnum; i++) {
    if((p_prv = p_ber->p_prv_lev[i]) && p_prv->mesh != K_CHYBA && 
      !strcmp(p_ber->p_mesh[p_prv->mesh]->jmeno,p_jmeno))
      return((MeshHandle)i);
  }
  return(K_CHYBA);
}

ExMeshHandle kom_najdi_mesh(byte *p_jmeno)
{
  int i;
  for(i = 0; i < p_ber->meshnum; i++) {
    if(p_ber->p_mesh[i] && !strcmp(p_ber->p_mesh[i]->jmeno,p_jmeno))
      return(i);
  }
  return(K_CHYBA);
}

ExMeshHandle kom_najdi_mesh_joint(ExMeshHandle *p_next)
{
  int i;
  
  if(*p_next < 0)
    return(-1);

  for(i = *p_next; i < p_ber->meshnum; i++) {
    if(p_ber->p_mesh[i] && p_ber->p_mesh[i]->p_joint_anim) {
      *p_next = i+1;
      return(i);
    }
  }
  
  *p_next = K_CHYBA;
  return(K_CHYBA);
}

/***************************************************
   Fleky - flekovac
  **************************************************
*/
#define FLEK_BIAS     2.0f
#define FLEK_BIAS_NAS 1.0f

void kom_flek_zmen(FlekHandle fh, BOD *p_p, BOD *p_nx, BOD *p_nz, float dx, float dz, int rotace, int uroven, MatHandle material)
{
  FLEK *p_flek = (FLEK *)fh;

  p_flek->material = material;
  p_flek->p = *p_p;
  
  p_flek->nx.x = p_nx->x*dx;
  p_flek->nx.y = p_nx->y*dx;
  p_flek->nx.z = p_nx->z*dx;
    
  p_flek->nz.x = p_nz->x*dz;
  p_flek->nz.y = p_nz->y*dz;
  p_flek->nz.z = p_nz->z*dz;  

  p_flek->uroven = uroven;
  p_flek->bias = -(FLEK_BIAS+(float)uroven*FLEK_BIAS_NAS);
  p_flek->flag = FALSE;

  if(!rotace) {
    p_flek->u[0] = 0.0f; p_flek->v[0] = 0.0f;
    p_flek->u[1] = 1.0f; p_flek->v[1] = 0.0f;
    p_flek->u[2] = 1.0f; p_flek->v[2] = 1.0f;
    p_flek->u[3] = 0.0f; p_flek->v[3] = 1.0f;
  } else if(rotace == 1) {
    p_flek->u[1] = 0.0f; p_flek->v[1] = 0.0f;
    p_flek->u[2] = 1.0f; p_flek->v[2] = 0.0f;
    p_flek->u[3] = 1.0f; p_flek->v[3] = 1.0f;
    p_flek->u[0] = 0.0f; p_flek->v[0] = 1.0f;
  } else if(rotace == 2) {
    p_flek->u[2] = 0.0f; p_flek->v[2] = 0.0f;
    p_flek->u[3] = 1.0f; p_flek->v[3] = 0.0f;
    p_flek->u[0] = 1.0f; p_flek->v[0] = 1.0f;
    p_flek->u[1] = 0.0f; p_flek->v[1] = 1.0f;
  } else {
    p_flek->u[3] = 0.0f; p_flek->v[3] = 0.0f;
    p_flek->u[0] = 1.0f; p_flek->v[0] = 0.0f;
    p_flek->u[1] = 1.0f; p_flek->v[1] = 1.0f;
    p_flek->u[2] = 0.0f; p_flek->v[2] = 1.0f;
  } 
}

void kom_vloz_flek(FLEK **p_first, FLEK *p_flek)
{
  FLEK *p_tmp = *p_first;
  FLEK *p_prev = NULL;

  while(p_tmp) {
    if(p_tmp->uroven < p_flek->uroven) {
      p_prev = p_tmp;
      p_tmp = p_tmp->p_next;      
    }
    else
      break;
  }

  if(!p_tmp && p_prev) { // insert-last
    p_flek->p_next = NULL;
    p_flek->p_prew = p_prev;
    p_prev->p_next = p_flek;
  } else if(!p_prev) { // insert-first    
    p_flek->p_next = p_tmp;
    p_flek->p_prew = NULL;
    if(p_tmp)
      p_tmp->p_prew = p_flek;
    *p_first = p_flek;
  } else { // insert mezi-kozy - predejdu to o 1
    p_flek->p_next = p_prev->p_next;
    p_flek->p_prew = p_prev;
    p_prev->p_next = p_flek;
    if(p_flek->p_next)
      p_flek->p_next->p_prew = p_flek; 
  }
}

FlekHandle kom_flek_pridej(MeshHandle mh, BOD *p_p, BOD *p_nx, BOD *p_nz, float dx, float dz, int rotace, int uroven,  MatHandle material)
{
  PRVEK_LEVELU_GAME *p_prv;
  GAME_MESH         *p_mesh;
  FLEK              *p_flek;

  /* Flek se pripojuje ke scene
  */
  if(mh == K_CHYBA) {
    p_flek = mmalloc(sizeof(FLEK));
    
    kom_flek_zmen((FlekHandle)p_flek, p_p, p_nx, p_nz, dx, dz, rotace, uroven, material);
    p_flek->mesh = K_CHYBA;    
    
    kom_vloz_flek(&p_ber->p_fleky,p_flek);
  }
  
  /* Flek se pripojuje ke meshi
  */
  else {
    p_prv = p_ber->p_prv_lev[mh];
    if(!p_prv || p_prv->mesh == K_CHYBA)
      return(K_CHYBA);
    
    p_mesh = p_ber->p_mesh[p_prv->mesh];
    p_flek = mmalloc(sizeof(FLEK));

    kom_flek_zmen((FlekHandle)p_flek, p_p, p_nx, p_nz, dx, dz, rotace, uroven, material);
    p_flek->mesh = p_prv->mesh;    
    
    /* Pripojeni fleku ke scene
    */
    kom_vloz_flek(&p_ber->p_fleky_mesh,p_flek);
  }
  return((int)p_flek);
}

int kom_flek_getflag(FlekHandle fh, int flag)
{
  FLEK *p_flek = (FLEK *)fh;
  return(p_flek->flag);
}

int kom_flek_setflag(FlekHandle fh, int flag)
{
  FLEK *p_flek = (FLEK *)fh;
  return(p_flek->flag = flag);
}

void kom_flek_zrus(FlekHandle fh)
{
  FLEK *p_flek = (FLEK *)fh;  
  
  if(p_flek->mesh == K_CHYBA) {
    /* Ruseni fleku sceny
    */
    if(p_flek->p_next)
      p_flek->p_next->p_prew = p_flek->p_prew;
    if(p_flek->p_prew)
      p_flek->p_prew->p_next = p_flek->p_next;
    else
      p_ber->p_fleky = p_flek->p_next;
  }   
  else {
    /* Ruseni fleku meshu
    */
    if(p_flek->p_next)
      p_flek->p_next->p_prew = p_flek->p_prew;
    if(p_flek->p_prew)
      p_flek->p_prew->p_next = p_flek->p_next;
    else
      p_ber->p_fleky_mesh = p_flek->p_next;
  }
  free(p_flek);
}

/* Nacte pocet materialu v kontejneru
*/
int kom_mesh_get_matnum(MeshHandle mh)
{
  PRVEK_LEVELU_GAME *p_prv;
  EDIT_MATERIAL *p_mat = NULL;
  GAME_MESH     *p_mesh;
  int            i,amat = -1,mat = 0;

  if((p_prv = p_ber->p_prv_lev[mh]) && p_prv->mesh != K_CHYBA) {
    p_mesh = p_ber->p_mesh[p_prv->mesh];
    for(i = mat = 0; i < p_mesh->objektu; i++) {
      if(p_mesh->p_mat[i] != amat) {
        amat = p_mesh->p_mat[i];
        mat++;
      }
    }
  }
  return(mat);
}

// Nacte samotne materialy
int kom_mesh_get_mat(MeshHandle mh, MatHandle *p_mat)
{
  PRVEK_LEVELU_GAME *p_prv;
  EDIT_MATERIAL     *p_lmat = NULL;
  GAME_MESH         *p_mesh;
  int                i, amat = -1, mat = 0;

  if((p_prv = p_ber->p_prv_lev[mh]) && p_prv->mesh != K_CHYBA) {
    p_mesh = p_ber->p_mesh[p_prv->mesh];
    for(i = mat = 0; i < p_mesh->objektu; i++) {
      if(p_mesh->p_mat[i] != amat) {
        amat = p_mesh->p_mat[i];
        p_mat[mat++] = amat;
      }
    }
  }
  return(mat);
}

// Nacte jmeno materialu z materialu
byte * kom_get_mat_jmeno(MatHandle mh, byte *p_buffer, int max_znaku)
{  
  if(p_ber->p_mat[mh] && (strlen(p_ber->p_mat[mh]->jmeno) < (dword)max_znaku)) {
    return(strcpy(p_buffer,p_ber->p_mat[mh]->jmeno));    
  } else
    return(NULL);
}

int kom_je_prvek_staticky(int guid)
{
  return(ber_je_objekt_staticky(guid));
}

/* *******************************************************
    Nastaveni mlznych kostek
*/
MLZNA_KOSTKA * kom_mlhokostka_najdi(byte *p_jmeno)
{
 MLZNA_KOSTKA *p_top = p_ber->p_mlha;

 while(p_top) {
   if(!strcmp(p_top->jmeno,p_jmeno))
     return(p_top);
 }
 return(NULL);
}

/* Nastaveni zpruhlednovani prvku
*/
void kom_zpruhlednovat_prvky(int stav)
{
  p_ber->kamera.zmena = TRUE;
  p_ber->conf_pruhledna_kamera = stav;
  if(!stav)
    ber_zpruhledni_prvky_reset(p_ber);
}

/* Zmena zpruhlednovani prvku
*/
void kom_zpruhlednovat_prvky_zmena_berusky(void)
{
  p_ber->kamera.zmena = TRUE;
}


/* Nastavi linelist
*/
void kom_set_linelist(KOFOLOVA_CARA *p_list, int pocet)
{  
  p_ber->car_a_kouzel = pocet;
  p_ber->p_cary = p_list;
}

/* Cte level environment levelu
*/
LEVEL_ENVIRONMENT * kom_get_level_environment(void)
{
  return(&p_ber->env);
}

/* Cte level pozici
*/
void kom_get_level_size(BOD *p_min, BOD *p_max)
{  
 *p_min = p_ber->level_min; 
 *p_max = p_ber->level_max;
}

/* Vrati mesh vybrany kurzorem mysi
*/
MeshHandle kom_get_mesh_mys(int itrida)
{
 GLMATRIX  *p_mat = get_matrix_camera_project(),m;
 GAME_MESH *p_mesh;
 PRVEK_LEVELU_GAME *p_prv;
 int   i,prvnum;
 float min_t = FLT_MAX;
 float t;
 int   min_mesh = K_CHYBA;
 BOD   a,b,p,*p_min,*p_max,min,max;

 glu_invert_matrix((float *)p_mat,(float *)&m);
 transf_2d_3d_matrix((float)mi.x, (float)mi.y, 0.0f, &a.x, &a.y, &a.z,
                     &m, p_ber->wxres, p_ber->wyres, p_ber->wx, p_ber->wy);
 transf_2d_3d_matrix((float)mi.x, (float)mi.y, 0.2f, &b.x, &b.y, &b.z,
                     &m, p_ber->wxres, p_ber->wyres, p_ber->wx, p_ber->wy);

 prvnum = p_ber->prvnum;
 for(i = 0; i < prvnum; i++) {
   p_prv = p_ber->p_prv_lev[i];
   if(p_prv && p_prv->guid && p_prv->guid/1000 != itrida) {
     if(p_prv->mesh != K_CHYBA) {
       p_mesh = p_ber->p_mesh[p_prv->mesh];
       p_min = &p_mesh->obb_world.aabb_min;
       p_max = &p_mesh->obb_world.aabb_max;
     } else {
       kd_bunka_expanze3(p_prv->x,p_prv->y,p_prv->z,
                         X_PRVEK2,Y_PRVEK2,Z_PRVEK2,
                         &min, &max);
       p_min = &min;
       p_max = &max;
     }
     if(kd_intersect_kostku_bod(&a,&b,p_min,p_max,&p,&t)) {
       t = vzdal_bodu_bod(&a,&p);
       if(t < min_t) {
         min_t = t;
         min_mesh = i;
       }
     }
   }
 }

 return(min_mesh);
}

int kom_get_mesh_mys_all(void)
{
 GLMATRIX  *p_mat = get_matrix_camera_project(),m;
 GAME_MESH *p_mesh; 
 int   i,meshnum;
 float min_t = FLT_MAX;
 float t;
 int   min_mesh = K_CHYBA;
 BOD   a,b,p,*p_min,*p_max;

 glu_invert_matrix((float *)p_mat,(float *)&m);
 transf_2d_3d_matrix((float)mi.x, (float)mi.y, 0.0f, &a.x, &a.y, &a.z,
                     &m, p_ber->wxres, p_ber->wyres, p_ber->wx, p_ber->wy);
 transf_2d_3d_matrix((float)mi.x, (float)mi.y, 0.2f, &b.x, &b.y, &b.z,
                     &m, p_ber->wxres, p_ber->wyres, p_ber->wx, p_ber->wy);

 meshnum = p_ber->meshnum;
 for(i = 0; i < meshnum; i++) {
   p_mesh = p_ber->p_mesh[i];
   if(p_mesh) {
     p_min = &p_mesh->obb_world.aabb_min;
     p_max = &p_mesh->obb_world.aabb_max;
     if(kd_intersect_kostku_bod(&a,&b,p_min,p_max,&p,&t)) {
       t = vzdal_bodu_bod(&a,&p);
       if(t < min_t) {
         min_t = t;
         min_mesh = i;
       }
     }
   }
 }

 return(min_mesh);
}

/* Nastavi aktivni mesh u multimesh charakteru
*/
MeshHandle kom_mesh_set_mesh(MeshHandle mh, int ID, int rotace)
{
  PRVEK_LEVELU_GAME *p_lev = p_ber->p_prv_lev[mh];
  int mesh = p_lev->mesh;
  GAME_MESH *p_prev = NULL,
            *p_mesh = p_ber->p_mesh[mesh],
            *p_last = p_mesh;

  assert(mesh != K_CHYBA);

  while(p_mesh) {
    if(p_mesh->low_id == ID) {
      if(p_prev) {
        p_prev->p_next = p_mesh->p_next;
        p_mesh->p_next = p_last;
        p_mesh->sim_flag = FALSE;
        p_mesh->p_data->p_mesh = p_mesh;
        p_ber->p_mesh[mesh] = p_mesh;
        p_lev->mesh = mesh;
      }      
      p_lev->rotace = rotace;
      ber_umisti_prvek_abs(p_lev,p_lev->x,p_lev->y-Y_PRVEK2,p_lev->z,
                           p_lev->rotace);
      return(mh);
    }
    p_prev = p_mesh;
    p_mesh = p_mesh->p_next;
  }  
  return(K_CHYBA);
}

/*
  Parametry funkce:
  mip_mapping = 0/1 (on/off)
  mip_filtr = 0/1 (linear/trilinear)
  std_filtr = 0/1 (near/liner)
  wrap_x/y = 0/1 (clamp/wrap)
  hi_res = 0/1 
*/

void kom_set_default_text_config(int mip_mapping, int mip_filtr, int std_filtr, int wrap_x, int wrap_y, int hi_res)
{
  txconf.text_mip_mapping = mip_mapping;
  txconf.text_mip_filtr = txconf.text_mip_filtr;
  txconf.text_filtr = txconf.text_filtr;
  txconf.text_wrap_x = wrap_x;
  txconf.text_wrap_y = wrap_y;
}

void kom_reload_ini(void)
{
  ber_rekonfiguruj_hra(p_ber);
  ber_rekonfiguruj_3D(p_ber);
  ber_rekonfiguruj_3D_aplikuj(p_ber);
}

void kom_zpruhlednovac_on(void)
{
  if(!p_ber->conf_zpruhlednovac) {
    p_ber->conf_zpruhlednovac = TRUE;
    p_ber->kamera.zmena = TRUE;
  }
}

void kom_zpruhlednovac_off(void)
{
  if(p_ber->conf_zpruhlednovac) {
    p_ber->conf_zpruhlednovac = FALSE;
    ber_kamera_zpruhledni_objekty_reset(p_ber);
  }
}

// Zvyraznovani prvku on/off
void kom_zvyraznovac(void)
{
  ber_prikaz_zvyraznovac(p_ber);
}

#define KOM_LOAD_KROKU 9

void kom_load_param_set(int tiku)
{
  p_ber->conf_load_tiku = tiku;
  p_ber->conf_load_tik = 0;
  p_ber->conf_load_krok = tiku/KOM_LOAD_KROKU;
}

void kom_load_param_fce(void (*p_load_fce)(void))
{
  p_ber->p_tik_fce = p_load_fce;
}

MeshHandle kom_pridej_kurzor_do_levelu(void)
{
  return(kom_pridej_mesh_do_levelu(JMENO_KURZOR));
}

MeshHandle kom_prvek_viditelnost(MeshHandle mh, int vid)
{
  PRVEK_LEVELU_GAME *p_prv = p_ber->p_prv_lev[mh];
  if(p_prv) {
    GAME_MESH *p_mesh = p_ber->p_mesh[p_prv->mesh];
    assert(p_mesh);
    p_mesh->nekreslit = !vid;
    return(TRUE);
  } else {
    return(FALSE);
  }
}
