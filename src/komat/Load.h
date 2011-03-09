/*
  Header k load.c :-)
  funkce pro praci s databazi materialu a textur
  a nacitanim x1 a out souboru...
*/

#ifndef __LOAD_H
#define __LOAD_H

#define  TVRDE_CHYBY 0

#define  MAX_BYTE 0xff

#define  S_NIC            "nic"

#define  MTRL_TAG         "MTRL"
#define  FAKTOR_TAG       "AFCT"
#define  STAG_TAG         "STGT"
#define  TEXT_TAG         "TEXT"
#define  MULTI_TAG        "MLTT"
#define  MRIZKA_TAG       "MRIZ"
#define  MRIZKA_SIG       "0001"
#define  KONFIG_TAG       "KNFG"
#define  MATANIM_TAG      "MTAN"
#define  KONFIG_TAG_OLD   "CNFG"

#define  ANIM_LOAD_ALL    0
#define  ANIM_LOAD_FRAME  1
#define  ANIM_LOAD_POSUN  2

#define  COPY_GEOMETRY    0x0001
#define  COPY_DIFFUSE     0x0002
#define  COPY_SPECULAR    0x0004
#define  COPY_MAP1        0x0008
#define  COPY_MAP2        0x0010
#define  COPY_MATERIAL    0x0020


#define  LOAD_GEOM              0x0001
#define  LOAD_MATERIALY         0x0002
#define  LOAD_TEXTURY           0x0004
#define  LOAD_STAT_LIGHT        0x0008
#define  LOAD_DYN_LIGHT         0x0010
#define  LOAD_EXTRA_DYN_LIGHT   0x0020
#define  LOAD_AMBIENT           0x0040
#define  LOAD_MLHA              0x0080
#define  LOAD_ZRCADLO           0x0100
#define  LOAD_FLARE             0x0200
#define  LOAD_LEVEL_ENV         0x0400
#define  LOAD_KAMERY            0x0800
#define  LOAD_LEVEL_CONF        0x1000
#define  SAVE_GEOM_VYBR         0x2000

#define  LOAD_ALL               (LOAD_GEOM|LOAD_MATERIALY|LOAD_TEXTURY|LOAD_STAT_LIGHT|LOAD_DYN_LIGHT|LOAD_EXTRA_DYN_LIGHT|LOAD_AMBIENT|LOAD_MLHA|LOAD_ZRCADLO|LOAD_FLARE|LOAD_LEVEL_ENV|LOAD_LEVEL_CONF|LOAD_KAMERY)


__inline void null_free(void **p_mem)
{                    
 if((*p_mem) != NULL) {
   free(*p_mem);
   *p_mem = NULL;
 }
}

__inline void lo_vloz_mezery(FILE *f, int pocet)
{
  int i;
  for(i = 0; i < pocet; i++) 
    fwrite(" ",sizeof(byte),1,f);
}

extern int hlasit_kolize;

__inline void str_read(byte *p_string, FFILE f)
{      
 int ret;
 while((ret = ffread(p_string,sizeof(byte),1,f)) && ret > 0 && *p_string)
   p_string++;
}

/*
  Koncovky
*/
byte * zamen_koncovku(byte *p_file, byte *p_koncovka);
byte * zamen_koncovku_znak(byte *p_file, byte znak, byte *p_koncovka);
byte * cti_koncovku(byte *p_file);

/*
  Mlha
*/
MLZNA_KOSTKA * vyrob_mlhokostku(byte *p_jmeno);
MLZNA_KOSTKA * kopiruj_mlhokostku(MLZNA_KOSTKA *p_src, byte *p_jmeno);
void           zrus_mlhokostku(MLZNA_KOSTKA **p_first, MLZNA_KOSTKA *p_maz);
MLZNA_KOSTKA * zrus_mlhokostku_all(MLZNA_KOSTKA *p_first);

/* 
  Objekty
*/
int  smaz_divoke_vertexy_objektu_dir(EDIT_OBJEKT *p_obj);
void zrus_objekt(EDIT_OBJEKT **p_obj);
void kopiruj_objekt_geometrie(EDIT_OBJEKT *p_src, EDIT_OBJEKT *p_cil);
EDIT_OBJEKT * vyrob_objekt(void);
EDIT_OBJEKT * vyrob_objekt_komplet(int v, int f);
EDIT_OBJEKT * kopiruj_objekt(EDIT_OBJEKT *p_src);
EDIT_OBJEKT * slep_objekty(EDIT_OBJEKT *p_obj1, EDIT_OBJEKT *p_obj2);

/*
  Kontejnery
*/
EDIT_KONTEJNER * vyrob_kontejner(void);
void zrus_kontejner(EDIT_KONTEJNER **pp_kont, MLZNA_KOSTKA **p_first);
void zrus_kontejner_rec(EDIT_KONTEJNER **pp_kont, MLZNA_KOSTKA **p_first);
void vyprazdni_kontejner(EDIT_KONTEJNER *p_kont);
void updatuj_kontejner_flag(EDIT_KONTEJNER *p_kont, EDIT_MATERIAL **p_mat);
void updatuj_kontejner_statistika(EDIT_KONTEJNER *p_kont, int obalky);
void updatuj_obalku_kontejner(EDIT_KONTEJNER *p_kont);
void smaz_divoke_vertexy_kontejneru(EDIT_KONTEJNER *p_kont);
void vycisti_kontejner(EDIT_KONTEJNER *p_kont);
void preindexuj_kontejner(EDIT_KONTEJNER *p_kont);
int  slep_kontejner(EDIT_KONTEJNER *p_kont);
int  slep_kontejner_dist(EDIT_KONTEJNER *p_kont, float min_dist);
EDIT_KONTEJNER * kopiruj_kontejner(EDIT_KONTEJNER *p_src, EDIT_MATERIAL **p_smat, 
                                   EDIT_MATERIAL **p_dmat, int dmatnum,
                                   STATIC_LIGHT *p_slight, int slightnum, 
                                   DYN_LIGHT *p_dlight, int dlightnum, 
                                   EXTRA_DYN_LIGHT *p_elight, int elightnum,
                                   LENS_FLARE *p_flare, int maxflares);
EDIT_KONTEJNER * kopiruj_kontejner_indir_vyber(EDIT_KONTEJNER *p_src, EDIT_KONTEJNER *p_dest, int flag);
int  lo_setrid_kontejner(EDIT_KONTEJNER *p_kont);
void lo_setrid_kontejner_materialy(EDIT_KONTEJNER *p_kont);
void kont_extra_light(EDIT_KONTEJNER *p_kont);

/*
  Animace
*/
void zrus_tri_vnitrek(HIERARCHY_TRACK_INFO *p_tri);
void zrus_tri(HIERARCHY_TRACK_INFO **p_trii);
void key_root_zrus_rec(HIERARCHY_TRACK_INFO *p_tri);
void key_root_zrus(HIERARCHY_ROOT *p_root);


/*
  Materialy
*/
EDIT_MATERIAL * vyrob_material(void);
void zrus_material(EDIT_MATERIAL **p_mat);
EDIT_MATERIAL * kopiruj_material(EDIT_MATERIAL *p_src);
EDIT_MATERIAL * kopiruj_material_indir(EDIT_MATERIAL *p_src, EDIT_MATERIAL *p_dest);
int  lo_pocet_materialu(EDIT_MATERIAL **p_mat, int max);
EDIT_MATERIAL * lo_najdi_material_text_point(EDIT_MATERIAL **p_mat, int max, EDIT_TEXT *p_text);
void lo_zrus_material_kont(EDIT_KONTEJNER *p_kont, EDIT_MATERIAL **p_list, int num);

/*
  Soubor
*/
int efile(byte *p_file);
int efileAPAK(APAK_HANDLE *pAHandle, byte *p_file);


/*
  Meshe
*/
GAME_MESH * kopiruj_mesh(GAME_MESH *p_src, GAME_MESH_DATA *p_data);
GAME_MESH * edit_to_mesh(GAME_MESH_DATA *p_mesh_data, EDIT_KONTEJNER *p_kont, EDIT_MATERIAL **p_mat, int max_mat, int shadow_volume);
GAME_MESH * vyrob_mesh(int objektu, GAME_MESH_DATA *p_data);
GAME_MESH_DATA * vyrob_mesh_data(void);
GAME_MESH_DATA * kopiruj_mesh_data(GAME_MESH_DATA *p_src, GAME_MESH *p_mesh_top, GAME_MESH_DATA *p_desc);
void zrus_mesh(GAME_MESH **p_mesh);
void mesh_pridej_vodavertexy(GAME_MESH *p_mesh);
void mesh_pridej_vertex_array(GAME_MESH *p_mesh);

/*
  Polylisty
*/
void poly_pridej_vertex_array(EDIT_MESH_POLY *p_poly);
EDIT_MESH_POLY * vyrob_poly(void);
void zrus_poly(EDIT_MESH_POLY **p_poly);
void zrus_vnitrek_poly(EDIT_MESH_POLY *p_poly);
EDIT_MESH_POLY * edit_to_poly_indir(EDIT_KONTEJNER *p_kont, EDIT_MESH_POLY *p_poly, int kID);
FFILE lo_poly_file_vyrob(byte *p_file, int filenum, int velikost);
FFILE lo_poly_file_otevri(byte *p_file, int *p_filenum, int velikost);
void lo_poly_file_zavri(FFILE f);
void lo_poly_uloz(FFILE f, EDIT_MESH_POLY *p_poly, EDIT_TEXT *p_light);
void lo_poly_nahraj_indir(FFILE f, EDIT_MESH_POLY *p_poly, EDIT_TEXT *p_light);
EDIT_MESH_POLY * lo_nahraj_poly_list(byte *p_file, int *p_polynum, EDIT_TEXT *p_light, EDIT_MATERIAL **p_mat, int matnum);
void lo_uloz_poly_list(byte *p_file, EDIT_MESH_POLY *p_poly, int polynum, EDIT_TEXT *p_light, EDIT_MATERIAL **p_mat, int matnum);

/* 
  Animacni list
*/
void lo_vyrob_animaci_list(EDIT_MATERIAL *p_mat, byte *p_list, EDIT_TEXT *p_text, int max, byte komplet, byte *p_dir);
void lo_uloz_animaci_list(EDIT_MATERIAL *p_mat, byte *p_list, byte *p_dir);

/*
  Hledaci funkce
*/
int lo_najdi_texturu(EDIT_TEXT *p_text, int max, byte *p_file, int flag);
int lo_najdi_volnou_texturu(EDIT_TEXT *p_text, int max);

int lo_najdi_material(EDIT_MATERIAL **p_mat, int max, byte *p_text);
int lo_najdi_prazdny_material(EDIT_MATERIAL **p_mat, int max);

int lo_najdi_volny_kontejner(EDIT_KONTEJNER **p_kont, int max);
int lo_najdi_volny_kontejner_up(EDIT_KONTEJNER **p_kont, int max, int up);
int lo_najdi_volny_kontejner_zpet(EDIT_KONTEJNER **p_kont, int max);
int lo_najdi_kontejner(EDIT_KONTEJNER **p_kont, int max, byte *p_jmeno);

int lo_najdi_volny_objekt_kont(EDIT_KONTEJNER *p_kont);
int lo_najdi_volny_mesh(GAME_MESH **p_mesh, int max);

int lo_najdi_prazdnou_animaci(ANIM_MATERIAL *p_anim, int max);
void lo_zrus_animaci(ANIM_MATERIAL *p_amat);

int lo_najdi_kameru(KAMERA *p_kam, int max, byte *p_jmeno);
int lo_najdi_volnou_kameru(KAMERA *p_kam, int max);

int lo_najdi_volny_flare(LENS_FLARE *p_flare, int max);
int lo_posledni_svetlo(STATIC_LIGHT *p_light, int max);


int           lo_najdi_objekt_kont_ID(EDIT_KONTEJNER *p_kont, int ID);
EDIT_OBJEKT * lo_najdi_objekt_kont_poiter_ID(EDIT_KONTEJNER *p_kont, int ID);

/*
  Loady
*/

void lo_poly_calc_lightmap_face(EDIT_MESH_POLY *p_poly);

EDIT_KONTEJNER * lo_nahraj_kontejner(EDIT_MATERIAL **p_mat, int max_mat, EDIT_TEXT *p_text, int max_text, byte *p_jmeno, int mat);
int  lo_reload_textur_dir(EDIT_TEXT *p_text, int max, byte *p_dir, int save);
int  lo_smaz_textury(EDIT_TEXT *p_text, int max);
int  lo_nahraj_materialy(EDIT_MATERIAL **p_mat, int max_mat, EDIT_TEXT *p_text, int max_text, byte *p_file);
int  lo_uloz_material(EDIT_MATERIAL *p_mat, byte *p_file, byte *p_dir);
int  lo_uloz_materialy(EDIT_MATERIAL **p_mat, int max_mat, byte *p_file, byte *p_dir);
int  lo_uloz_materialy_pouzite(EDIT_MATERIAL **p_mat, int max_mat, byte *p_file, byte *p_dir);
FFILE lo_uloz_kontejner(EDIT_MATERIAL **p_mat, int max_mat, EDIT_KONTEJNER *p_kont_top, byte *p_jmeno, int file, FFILE f);
void lo_vymaz_materialy(EDIT_MATERIAL **p_mat, int max_mat, EDIT_TEXT *p_text, int max_text);
GAME_MESH * lo_nahraj_mesh(EDIT_MATERIAL **p_mat, int max_mat, EDIT_TEXT *p_text, int max_text, byte *p_file, int mat, int extra_light);
int  lo_najdi_objekt_kont(EDIT_KONTEJNER *p_kont, byte *p_jmeno);
EDIT_OBJEKT * lo_najdi_objekt_kont_poiter(EDIT_KONTEJNER *p_kont, byte *p_jmeno);
EDIT_MESH_POLY * lo_nahraj_poly_list(byte *p_file, int *p_polynum, EDIT_TEXT *p_light, EDIT_MATERIAL **p_mat, int matnum);
void lo_uloz_poly_list(byte *p_file, EDIT_MESH_POLY *p_poly, int polynum, EDIT_TEXT *p_light, EDIT_MATERIAL **p_mat, int matnum);
void lo_poly_flaguj_materialy(EDIT_MESH_POLY *p_poly, EDIT_MATERIAL **p_mat);
void lo_poly_oznac_zrcadlo(EDIT_MESH_POLY *p_poly);
void lo_vloz_mezery(FILE *f, int pocet);
int  lo_velikost_textur(EDIT_TEXT *p_text, int max);
void lo_kontejner_jedinecne_jmena(EDIT_KONTEJNER *p_kont, int ds3);
GAME_MESH * lo_kontejner_to_mesh(EDIT_KONTEJNER **p_kont_top, EDIT_MATERIAL **p_mat, int max_mat, int extra_light);

int lo_reload_textur(TEXT_DIR *p_dir, EDIT_TEXT *p_text, int num, int save);
int lo_reload_textur_chyby(EDIT_TEXT *p_text, int num);

int lo_velikost_meshu(GAME_MESH **p_mesh, int num, int *p_facu, int *p_vertexu);
int lo_velikost_poly(EDIT_MESH_POLY *p_poly, int num, int *p_facu, int *p_vertexu);

void zrus_fleky(FLEK **p_flek);
void zrus_flare(LENS_FLARE **p_flare);

int lo_najdi_volne_dsvetlo(DYN_LIGHT *p_dlight, int max);
int lo_najdi_plne_dsvetlo(DYN_LIGHT *p_dlight, int max);
DYN_LIGHT * lo_najdi_volne_dsvetlo_point(DYN_LIGHT *p_dlight, int max);
int lo_najdi_prepocitej_dsvetlo(DYN_LIGHT *p_dlight, int max);

int lo_najdi_prepocitej_extra_svetlo(EXTRA_DYN_LIGHT *p_dlight, int max);
int lo_najdi_volne_extra_svetlo(EXTRA_DYN_LIGHT *p_dlight, int max);
int lo_najdi_plne_extra_svetlo(EXTRA_DYN_LIGHT *p_dlight, int max);
EXTRA_DYN_LIGHT * lo_najdi_volne_extra_svetlo_point(EXTRA_DYN_LIGHT *p_dlight, int max);

int lo_najdi_volne_stat_svetlo(STATIC_LIGHT *p_light, int max);
int lo_najdi_plne_stat_svetlo(STATIC_LIGHT *p_light, int max);

LENS_FLARE * lo_flare2linear(LENS_FLARE *p_flare, int max);
void lo_kontlist_jedinecne_jmena(EDIT_KONTEJNER **p_kont, int listnum);

byte * cti_za_znak(byte *p_string, char znak);

#define smaz_face_bez_vertexu(p_face,facenum,f) \
{                                               \
 if(f < facenum)                              \
   memmove(p_face+f,p_face+(f+3),(facenum-f-1)*sizeof(p_face[0]));\
 facenum -= 3;                                  \
}                                               

#define smaz_vertex_bez_facu(p_vert,vertexnum,v)\
{                                               \
 if(v+1 < vertexnum)                            \
   memmove(p_vert+v,p_vert+v+1,(vertexnum-v-1)*sizeof(p_vert[0]));\
 vertexnum--;                                   \
}                                               

__inline void mat_default_stage_bloky(EDIT_MATERIAL *p_mat)
{
  MATERIAL_TEXT *p_stg;
  int i,j;

  p_mat->alfa_state = 0;
  memset(p_mat->text_state,0,sizeof(p_mat->text_state[0])*MAT_TEXTUR);
  
  for(i = 0; i < MAT_TEXTUR; i++) {
    p_stg = p_mat->text_state+i;
    p_stg->text_stage = !i ? 0 : K_CHYBA;    
    for(j = 0; j < MAT_TEXTUR; j++)
      p_stg->textury[j] = j;

    /* Defaultni bump-mapping
    */
    p_stg->text_ati_bump_matrix[0] = 2.0f/256.0f;
    p_stg->text_ati_bump_matrix[1] = 0.0f;
    p_stg->text_ati_bump_matrix[2] = 2.0f/256.0f;
    p_stg->text_ati_bump_matrix[3] = 0.0f;  
  }
}

/* Prevod svetel do sceny
*/
void lo_preved_svetla_do_sceny(EDIT_KONTEJNER *p_kont, STATIC_LIGHT *p_light, int lightnum, DYN_LIGHT *p_dlist, int dlistnum, EXTRA_DYN_LIGHT *p_elist, int elistnum);
void lo_kopiruj_svetla_do_sceny(EDIT_KONTEJNER *p_kont, STATIC_LIGHT *p_light, int lightnum, DYN_LIGHT *p_dlist, int dlistnum, EXTRA_DYN_LIGHT *p_elist, int elistnum, LENS_FLARE *p_flares, int maxflares);
void lo_kopiruj_svetla_do_sceny_mesh(GAME_MESH_DATA *p_data, DYN_LIGHT *p_dlist, int dlistnum, EXTRA_DYN_LIGHT *p_elist, int elistnum);
void lo_preved_svetla_do_sceny_mesh(GAME_MESH_DATA *p_data, DYN_LIGHT *p_dlist, int dlistnum, EXTRA_DYN_LIGHT *p_elist, int elistnum);
void lo_preved_flare_do_sceny(STATIC_LIGHT *p_light, LENS_FLARE *p_flarelist, int flaremax);
void lo_vymaz_svetla_ze_sceny(EDIT_KONTEJNER *p_kont);
void lo_vymaz_svetla(EDIT_KONTEJNER *p_kont);
void lo_vymaz_svetla_ze_sceny_mesh(GAME_MESH_DATA *p_data);
void lo_vymaz_svetla_mesh(GAME_MESH_DATA *p_data);
void lo_premapuj_svetla_kont(EDIT_KONTEJNER *p_src, EDIT_KONTEJNER *p_dest);
void lo_premapuj_svetla_kont_mesh(EDIT_KONTEJNER *p_src, GAME_MESH *p_dest);
void lo_premapuj_svetla_mesh(GAME_MESH *p_src, GAME_MESH *p_dest);
void lo_transformuj_svetla_do_wordspace(EDIT_KONTEJNER *p_src);
void lo_premapuj_svetla_do_wordspace(EDIT_KONTEJNER *p_src);
LENS_FLARE * lo_kopiruj_flare(LENS_FLARE *p_flarelist, int max, LENS_FLARE *p_flare);


int   lo_najdi_objekt_kont_plny(EDIT_KONTEJNER *p_kont);
void  zrus_kosti_rec(JOINT **p_joint);
void  zrus_kosti(EDIT_OBJEKT *p_obj);
void  zrus_kosti_key(JOINT_KEYS **p_key);

void  zrus_kosti_prs_rec(JOINT *p_joint);

int * NvConsolidateSingleFace(EXTRA_FACE *p_eface, int facenum, int *p_facevel);
void  NvStripToGL(EXTRA_FACE *p_face, int facenum);
void  NvGenerateStrips_fast(unsigned short *p_ind, int num, STRIP_FACE **p_fface, int *p_facenum);
void  oe_obj_to_strip(EDIT_OBJEKT *p_obj);
void  oe_kont_to_strip(EDIT_KONTEJNER *p_kont_top);
int   lo_reload_textur_vypis(EDIT_TEXT *p_text, int num);
int   lo_pocet_textur(EDIT_TEXT *p_text, int max);
int   lo_smaz_textury_bmp(EDIT_TEXT *p_text, int max);

void  lo_reload_stage(EDIT_MATERIAL **p_mat, int num);

#endif

