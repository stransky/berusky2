/* Chunky
*/
#include <string.h>
#include <limits.h>
#include <errno.h>

#include "3d_all.h"

#define  CHUNK_MATERIAL        10  // nacitat material (jmeno materialu)
#define  CHUNK_COLOR           11  // barvy materialu
#define  CHUNK_MATANIM         12  // animace materialu
#define  CHUNK_STAG_TAG        13  // stage tagy (cislo stagu + jejich cisla)
#define  CHUNK_COLOR_EXT       20  // barvy materialu rozsirene
#define  CHUNK_LESK            21  // sila odlesku
#define  CHUNK_FLAGS           22  // flagy materialu
#define  CHUNK_ENVSPEC         26  // spec-env udaje
#define  CHUNK_ALFA_STAGE      27  // alfa nastavenni materialu (pruhledny,pouzity,src,dest)
#define  CHUNK_TEXT_STAGE_1    28  // multi tag, pred texturou
#define  CHUNK_TEXT_STAGE_2    29  // multi tag, pred texturou
#define  CHUNK_TEXT_STAGE_3    30  // multi tag, pred texturou
#define  CHUNK_TEXT_STAGE_4    31  // multi tag, pred texturou
#define  CHUNK_TEXT_1          16  // textura, k multi tagu
#define  CHUNK_TEXT_2          18  // textura, k multi tagu
#define  CHUNK_TEXT_3          32  // textura, k multi tagu
#define  CHUNK_TEXT_4          33  // textura, k multi tagu
#define  CHUNK_MATANIM_T1      34  // animace textur 1
#define  CHUNK_MATANIM_T2      35  // animace textur 2
#define  CHUNK_MATANIM_T3      36  // animace textur 3
#define  CHUNK_MATANIM_T4      37  // animace textur 4
// tady se musi nechat rezerva kvuli ukadani dalsich textur

#define  CHUNK_MATMATRIX_EXT   50  // animacni matice textury 1

#define  CHUNK_TEXT_STAGE_AD_1 51  // multi tag, pred texturou
#define  CHUNK_TEXT_STAGE_AD_2 52  // multi tag, pred texturou
#define  CHUNK_TEXT_STAGE_AD_3 53  // multi tag, pred texturou
#define  CHUNK_TEXT_STAGE_AD_4 54  // multi tag, pred texturou

#define  CHUNK_TEXT_FLAG_1     55  // textura, k multi tagu
#define  CHUNK_TEXT_FLAG_2     56  // textura, k multi tagu
#define  CHUNK_TEXT_FLAG_3     57  // textura, k multi tagu
#define  CHUNK_TEXT_FLAG_4     58  // textura, k multi tagu

#define  CHUNK_MATANIM_EXT     60  // animace materialu
#define  CHUNK_MATANIM_FRAME   61  // frame materialu
#define  CHUNK_MAT_SMAT_POS    62  // pozice s-materialu
#define  CHUNK_MAT_TEXGEN      63  // pozice s-materialu
#define  CHUNK_MATANIM_EFRAME  64  // frame materialu

#define  CHUNK_KONT_JMENO     100  // zacatek kontejneru - jeho jmeno
#define  CHUNK_KONT_FLAG      101  // flag kontejneru
#define  CHUNK_KONT_WMATRIX   102  // matice kontejneru
#define  CHUNK_KONT_FLAG_EXT  103  // flag kontejneru
#define  CHUNK_KONT_MPMATRIX  104  // m-p matice kontejneru
#define  CHUNK_KONT_LOW_JMENO 105  // zacatek low kontejneru
#define  CHUNK_KONT_LOW_ID    106  // identifikator low-kontejneru
#define  CHUNK_KONT_ID        107  // identifikator kontejneru
#define  CHUNK_KONT_FLAG_EXT_2 108 // ext-flagy
// #define  CHUNK_KONT_OBB       109  - stara
#define  CHUNK_KONT_OBB       110  // OBB kontejneru

#define  CHUNK_OBJ_JMENO      1001 // jmeno objektu
#define  CHUNK_OBJ_MATERIAL   1002 // material objektu
#define  CHUNK_OBJ_VERTEXY    1003 // pole vertexu
#define  CHUNK_OBJ_MAPY_1     1004 // pole mapu
#define  CHUNK_OBJ_MAPY_2     1005 // pole mapu
#define  CHUNK_OBJ_FACY       1006 // pole facu
#define  CHUNK_OBJ_BARVY      1007 // pole barev
#define  CHUNK_OBJ_BARVY_EXT  1008 // pole rozsirenych barev
#define  CHUNK_OBJ_MATRIX     1009 // local-matrix kontejneru (pro keyframe-kontejnery)
#define  CHUNK_OBJ_ID         1010 // ID objektu
#define  CHUNK_OBJ_MATRIX_KEY 1011 // keyframe matice
#define  CHUNK_OBJ_MAPY_3     1012 // pole mapu 3
#define  CHUNK_OBJ_MAPY_4     1013 // pole mapu 4
#define  CHUNK_OBJ_PIVOT      1014 // pivot-point objektu
//#define  CHUNK_OBJ_OBB        1015 stara obb obalka
#define  CHUNK_OBJ_OBB        1016 // obb obalka
#define  CHUNK_OBJ_FACY_OPT   1017 // pole facu
#define  CHUNK_OBJ_NORMAL     1018 // pole normalu


#define  CHUNK_OBJ_JA         1500
#define  CHUNK_OBJ_JA_JMENO   1501
#define  CHUNK_OBJ_JA_FRAMU   1502

#define  CHUNK_OBJ_JNT_ID     1600
#define  CHUNK_OBJ_JNT_PIVOT  1601

#define  CHUNK_OBJ_JNT_VERT   1602
#define  CHUNK_OBJ_JNT_FLAG   1603
#define  CHUNK_OBJ_JNT_POS    1604
#define  CHUNK_OBJ_JNT_ROT    1605
#define  CHUNK_OBJ_JNT_SCS    1606

#define  CHUNK_OBJ_JNT_KEYPOS 1607
#define  CHUNK_OBJ_JNT_KEYROT 1608
#define  CHUNK_OBJ_JNT_KEYSCS 1609

//v 1.0                        // vsechny se davaji do aktivniho kontejneru
//#define  CHUNK_KEYFRAME_ROOT 2000 // hlavicka key-framove animace
//#define  CHUNK_KEYFRAME_ROT2 2005 // hlavicka key-framove animace
//#define  CHUNK_KEYFRAME_NODE 2001 // vetev animace - pro jeden objekt
//#define  CHUNK_KEYFRAME_POS  2002 // chunk s pozicema
//#define  CHUNK_KEYFRAME_ROT  2003 // chunk s rotacema
//#define  CHUNK_KEYFRAME_SCS  2004 // chunk s scalama

//v. 2
#define  CHUNK_KEYFRAME_ROT2  2006 // hlavicka key-framove animace
#define  CHUNK_KEYFRAME_NODE  2007 // vetev animace - pro jeden objekt
#define  CHUNK_KEYFRAME_POS   2008 // chunk s pozicema
#define  CHUNK_KEYFRAME_ROT   2009 // chunk s rotacema
#define  CHUNK_KEYFRAME_SCS   2010 // chunk s scalama

//v. 3 - sim animace
#define  CHUNK_SIM_ROOT        2020 // hlavicka key-framove animace
#define  CHUNK_SIM_NODE        2021 // vetev animace - pro jeden objekt
#define  CHUNK_SIM_NODE_EXT    2022 // vetev animace - pro jeden objekt
//#define  CHUNK_SIM_NODE_MAT   2023 // all-matice
#define  CHUNK_SIM_NODE_EXT2   2024 // vetev animace - pro jeden objekt

// v. 2
#define  CHUNK_STATIC_LIGHT        5003 // chunk se statickym svetlem sceny
#define  CHUNK_STATIC_LIGHT_MESH   5004 // chunk se statickym svetlem (patri k naposledy nactenemu meshi)

#define  CHUNK_DYNAMIC_LIGHT       5100 // chunk se dynamickycm svetlem
#define  CHUNK_DYNAMIC_LIGHT_MESH  5101 // chunk se dynamickycm svetlem  (patri k naposledy nactenemu meshi)

#define  CHUNK_EXTRA_LIGHT         5200 // chunk se extra dynamickycm svetlem
#define  CHUNK_EXTRA_LIGHT_MESH    5201 // chunk se extra dynamickycm svetlem  (patri k naposledy nactenemu meshi)

#define  CHUNK_ZRCADLO             6000 // chunk se statickym svetlem
#define  CHUNK_ZRCADLO_2           6001 // chunk se statickym svetlem
#define  CHUNK_ZRCADLO_3           6002 // chunk se statickym svetlem
#define  CHUNK_ZRCADLO_3_POLOZKA   6003 // chunk se statickym svetlem

#define  CHUNK_FLARE               7000 // chunk se flare
#define  CHUNK_FLARE_SLOZENY       7001 // chunk se slozkama ke flare
#define  CHUNK_FLARE_MESH          7002 // chunk se flare_pripojeny k mesi
#define  CHUNK_FLARE_LIGHT         7003 // chunk se flare ke svetlu

#define  CHUNK_FLARE_EXT           7010 // chunk se flare
#define  CHUNK_FLARE_MESH_EXT      7011 // chunk se flare_pripojeny k mesi
#define  CHUNK_FLARE_LIGHT_EXT     7012 // chunk se flare ke svetlu

#define  CHUNK_AMBIENT             8000 // chunk s ambientnim svetlem
#define  CHUNK_LEVEL_KONFIG        8001 // chunk s kofiguraci levelu
#define  CHUNK_LEVEL_KONFIG_EXT    8002 // chunk s kofiguraci levelu

#define  CHUNK_MLHOKOSTKA    8010 // chunk se starou mlznou kostkou
#define  CHUNK_MLHOKOSTKA_2  8011 // chunk s mlznou kostkou

#define  CHUNK_ENVIRONMENT   8020 // chunk s environmentem

#define  CHUNK_KAMSET        8021 // chunk s environmentem
#define  CHUNK_KAMSET_2      8022 // chunk s environmentem


#define  ROOT_NODE_JMENO     "$$$ROOT_NODE$$$"


typedef struct _OUT_CHUNK {

  int   typ;
  int   velikost; // od zacatku tohoto chunku

} OUT_CHUNK;


/* **********************************************************************
   Ulozeni environmentu
*/
int lo_chunk_save_environment(FFILE f, LEVEL_ENVIRONMENT *p_env)
{
  OUT_CHUNK ch;  
  
  ch.typ = CHUNK_ENVIRONMENT;
  ch.velikost = sizeof(ch)+sizeof(p_env[0]);

  ffwrite(&ch,sizeof(ch),1,f);
  ffwrite(p_env,sizeof(p_env[0]),1,f);
  
  return(TRUE);
}

/* **********************************************************************
   Ulozeni omezeni a nastaveni kamer
*/
int lo_chunk_save_kamset(FFILE f, NASTAVENI_KAMER *p_kam)
{
  OUT_CHUNK ch;  
  
  ch.typ = CHUNK_KAMSET;
  ch.velikost = sizeof(ch)+
                sizeof(p_kam->far_plane)+
                sizeof(p_kam->fov)+
                sizeof(p_kam->max_uhel)+
                sizeof(p_kam->min_uhel)+
                sizeof(p_kam->min_vzdal)+
                sizeof(p_kam->max_vzdal);

  ffwrite(&ch,sizeof(ch),1,f);
  ffwrite(&p_kam->far_plane,sizeof(p_kam->far_plane),1,f);
  ffwrite(&p_kam->fov,sizeof(p_kam->fov),1,f);
  ffwrite(&p_kam->max_uhel,sizeof(p_kam->max_uhel),1,f);
  ffwrite(&p_kam->min_uhel,sizeof(p_kam->min_uhel),1,f);
  ffwrite(&p_kam->min_vzdal,sizeof(p_kam->min_vzdal),1,f);
  ffwrite(&p_kam->max_vzdal,sizeof(p_kam->max_vzdal),1,f);
  
  return(TRUE);
}

int lo_chunk_save_kamset_2(FFILE f, NASTAVENI_KAMER *p_kam)
{
  OUT_CHUNK ch;  
  
  ch.typ = CHUNK_KAMSET_2;
  ch.velikost = sizeof(ch)+
                sizeof(p_kam->max)+
                sizeof(p_kam->min);

  ffwrite(&ch,sizeof(ch),1,f);
  ffwrite(&p_kam->max,sizeof(p_kam->max),1,f);
  ffwrite(&p_kam->min,sizeof(p_kam->min),1,f);
  
  return(TRUE);
}

/* **********************************************************************
   Ulozeni dynamickeho svetla
*/
int lo_chunk_save_dyn_light_anim(FFILE f, DYN_LIGHT_ANIM *p_an, int extra)
{  
  ffwrite(&p_an->pos_keys,sizeof(p_an->pos_keys),1,f);
  ffwrite(p_an->p_pkeys,sizeof(p_an->p_pkeys[0]),p_an->pos_keys,f);
  ffwrite(p_an->p_pos,sizeof(p_an->p_pos[0]),p_an->pos_keys,f);
  
  ffwrite(&p_an->trg_keys,sizeof(p_an->trg_keys),1,f);
  ffwrite(p_an->p_tkeys,sizeof(p_an->p_tkeys[0]),p_an->trg_keys,f);
  ffwrite(p_an->p_trg,sizeof(p_an->p_trg[0]),p_an->trg_keys,f);

  ffwrite(&p_an->dos_keys,sizeof(p_an->dos_keys),1,f);
  ffwrite(p_an->p_dskeys,sizeof(p_an->p_dskeys[0]),p_an->dos_keys,f);
  ffwrite(p_an->p_dosah,sizeof(p_an->p_dosah[0]),p_an->dos_keys,f);

  if(!extra) {
    ffwrite(&p_an->diff_keys,sizeof(p_an->diff_keys),1,f);
    ffwrite(p_an->p_dfkeys,sizeof(p_an->p_dfkeys[0]),p_an->diff_keys,f);
    ffwrite(p_an->p_diff,sizeof(p_an->p_diff[0]),p_an->diff_keys,f);
  } else {
    int i;  
    p_an->p_diff = (BARVA_RGBA *)mmalloc(sizeof(p_an->p_diff[0])*p_an->alfa_keys);
    for(i = 0 ; i < p_an->alfa_keys; i++) {
      p_an->p_diff[i].r = p_an->p_diff[i].g = p_an->p_diff[i].b = p_an->p_diff[i].a = p_an->p_alfa[i];
    }
    ffwrite(&p_an->alfa_keys,sizeof(p_an->alfa_keys),1,f);
    ffwrite(p_an->p_akeys,sizeof(p_an->p_akeys[0]),p_an->alfa_keys,f);
    ffwrite(p_an->p_diff,sizeof(p_an->p_diff[0]),p_an->alfa_keys,f);      
    null_free((void **)&p_an->p_diff);
  }

  ffwrite(&p_an->spec_keys,sizeof(p_an->spec_keys),1,f);
  ffwrite(p_an->p_spkeys,sizeof(p_an->p_spkeys[0]),p_an->spec_keys,f);
  ffwrite(p_an->p_spec,sizeof(p_an->p_spec[0]),p_an->spec_keys,f);
  
  return(TRUE);
}

int lo_chunk_get_dyn_light_size(FFILE f, DYN_LIGHT_ANIM *p_an)
{
  return(sizeof(p_an->pos_keys)+
    sizeof(p_an->p_pkeys[0])*p_an->pos_keys+
    sizeof(p_an->p_pos[0])*p_an->pos_keys+
    sizeof(p_an->trg_keys)+
    sizeof(p_an->p_tkeys[0])*p_an->trg_keys+
    sizeof(p_an->p_trg[0])*p_an->trg_keys+
    sizeof(p_an->dos_keys)+
    sizeof(p_an->p_dskeys[0])*p_an->dos_keys+
    sizeof(p_an->p_dosah[0])*p_an->dos_keys+
    sizeof(p_an->diff_keys)+
    sizeof(p_an->p_dfkeys[0])*p_an->diff_keys+
    sizeof(p_an->p_diff[0])*p_an->diff_keys+
    sizeof(p_an->spec_keys)+
    sizeof(p_an->p_spkeys[0])*p_an->spec_keys+
    sizeof(p_an->p_spec[0])*p_an->spec_keys);
}

int lo_chunk_save_dyn_light(FFILE f, DYN_LIGHT *p_light)
{
  OUT_CHUNK ch;  
  ch.typ = CHUNK_DYNAMIC_LIGHT;
  ch.velikost = sizeof(ch)+sizeof(p_light->akt)+sizeof(byte)+sizeof(p_light->np)+
                sizeof(p_light->dir)+sizeof(p_light->look_max)+sizeof(p_light->dosah)+
                sizeof(p_light->min)+sizeof(p_light->max)+sizeof(p_light->theta)+
                sizeof(p_light->dr)+sizeof(p_light->dg)+sizeof(p_light->db)+
                sizeof(p_light->an_flag)+sizeof(p_light->flag);

  if(p_light->an.endtime) {
    ch.velikost += lo_chunk_get_dyn_light_size(f, &p_light->an);
  }

  ffwrite(&ch,sizeof(ch),1,f);
  ffwrite(&p_light->akt,sizeof(p_light->akt),1,f);
  ffwrite(p_light->jmeno,sizeof(byte),strlen(p_light->jmeno)+1,f);
  ffwrite(&p_light->np,sizeof(p_light->np),1,f);
  ffwrite(&p_light->dir,sizeof(p_light->dir),1,f);
  ffwrite(&p_light->look_max,sizeof(p_light->look_max),1,f);
  ffwrite(&p_light->dosah,sizeof(p_light->dosah),1,f);
  ffwrite(&p_light->min,sizeof(p_light->min),1,f);
  ffwrite(&p_light->max,sizeof(p_light->max),1,f);
  ffwrite(&p_light->theta,sizeof(p_light->theta),1,f);
  ffwrite(&p_light->dr,sizeof(p_light->dr),1,f);
  ffwrite(&p_light->dg,sizeof(p_light->dg),1,f);
  ffwrite(&p_light->db,sizeof(p_light->db),1,f);
  ffwrite(&p_light->sr,sizeof(p_light->sr),1,f);
  ffwrite(&p_light->sg,sizeof(p_light->sg),1,f);
  ffwrite(&p_light->sb,sizeof(p_light->sb),1,f);
  ffwrite(&p_light->flag,sizeof(p_light->flag),1,f);
  ffwrite(&p_light->ktrida,sizeof(p_light->ktrida),1,f);
  ffwrite(&p_light->an_flag,sizeof(p_light->an_flag),1,f);
  ffwrite(&p_light->an.endtime,sizeof(p_light->an.endtime),1,f);
  if(p_light->an.endtime) {
    lo_chunk_save_dyn_light_anim(f, &p_light->an, FALSE);
  }

  return(TRUE);
}
// ulozi linearni seznam svetel
int lo_chunk_save_dyn_light_mesh(FFILE f, DYN_LIGHT *p_light)
{
  OUT_CHUNK ch;  
  while(p_light) {
    ch.typ = CHUNK_DYNAMIC_LIGHT_MESH;
    ch.velikost = sizeof(ch)+sizeof(p_light->akt)+sizeof(byte)+sizeof(p_light->np)+
      sizeof(p_light->dir)+sizeof(p_light->look_max)+sizeof(p_light->dosah)+
      sizeof(p_light->min)+sizeof(p_light->max)+sizeof(p_light->theta)+
      sizeof(p_light->dr)+sizeof(p_light->dg)+sizeof(p_light->db)+
      sizeof(p_light->an_flag)+sizeof(p_light->flag);

    if(p_light->an.endtime) {
      ch.velikost += lo_chunk_get_dyn_light_size(f, &p_light->an);
    }
  
    ffwrite(&ch,sizeof(ch),1,f);
    ffwrite(&p_light->akt,sizeof(p_light->akt),1,f);
    ffwrite(p_light->jmeno,sizeof(byte),strlen(p_light->jmeno)+1,f);
    ffwrite(&p_light->np,sizeof(p_light->np),1,f);
    ffwrite(&p_light->dir,sizeof(p_light->dir),1,f);
    ffwrite(&p_light->look_max,sizeof(p_light->look_max),1,f);
    ffwrite(&p_light->dosah,sizeof(p_light->dosah),1,f);
    ffwrite(&p_light->min,sizeof(p_light->min),1,f);
    ffwrite(&p_light->max,sizeof(p_light->max),1,f);
    ffwrite(&p_light->theta,sizeof(p_light->theta),1,f);
    ffwrite(&p_light->dr,sizeof(p_light->dr),1,f);
    ffwrite(&p_light->dg,sizeof(p_light->dg),1,f);
    ffwrite(&p_light->db,sizeof(p_light->db),1,f);
    ffwrite(&p_light->sr,sizeof(p_light->sr),1,f);
    ffwrite(&p_light->sg,sizeof(p_light->sg),1,f);
    ffwrite(&p_light->sb,sizeof(p_light->sb),1,f);
    ffwrite(&p_light->flag,sizeof(p_light->flag),1,f);
    ffwrite(&p_light->ktrida,sizeof(p_light->ktrida),1,f);
    ffwrite(&p_light->an_flag,sizeof(p_light->an_flag),1,f);
    ffwrite(&p_light->an.endtime,sizeof(p_light->an.endtime),1,f);
    if(p_light->an.endtime) {
      lo_chunk_save_dyn_light_anim(f, &p_light->an,FALSE);
    }
    p_light = p_light->p_next;
  }
  return(TRUE);
}

/************************************************************
   Ulozeni extra-svetel
  ***********************************************************
*/
int lo_chunk_save_extra_light(FFILE f, EXTRA_DYN_LIGHT *p_light)
{
  OUT_CHUNK ch;  
  ch.typ = CHUNK_EXTRA_LIGHT;
  ch.velikost = sizeof(ch)+sizeof(p_light->akt)+sizeof(byte)+
                sizeof(p_light->flag)+sizeof(p_light->mflag)+
                sizeof(p_light->ktrida)+sizeof(p_light->np)+
                sizeof(p_light->dir)+sizeof(p_light->dosah)+
                sizeof(p_light->theta)+sizeof(p_light->utlum)+
                sizeof(p_light->gourand)+4*sizeof(p_light->da)+
                sizeof(p_light->sr)+sizeof(p_light->sg)+sizeof(p_light->sb)+
                sizeof(p_light->an_flag)+sizeof(p_light->an.endtime);

  if(p_light->an.endtime) {
    ch.velikost += lo_chunk_get_dyn_light_size(f, &p_light->an);
  }

  ffwrite(&ch,sizeof(ch),1,f);
  ffwrite(&p_light->akt,sizeof(p_light->akt),1,f);
  ffwrite(p_light->jmeno,sizeof(byte),strlen(p_light->jmeno)+1,f);
  ffwrite(&p_light->flag,sizeof(p_light->flag),1,f);
  ffwrite(&p_light->mflag,sizeof(p_light->mflag),1,f);
  ffwrite(&p_light->ktrida,sizeof(p_light->ktrida),1,f);
  ffwrite(&p_light->np,sizeof(p_light->np),1,f);
  ffwrite(&p_light->dir,sizeof(p_light->dir),1,f);
  ffwrite(&p_light->dosah,sizeof(p_light->dosah),1,f);
  ffwrite(&p_light->theta,sizeof(p_light->theta),1,f);
  ffwrite(&p_light->utlum,sizeof(p_light->utlum),1,f);
  ffwrite(&p_light->gourand,sizeof(p_light->gourand),1,f);
  ffwrite(&p_light->da,sizeof(p_light->da),4,f); // alfa se ulozi jako duplikovana
  ffwrite(&p_light->sr,sizeof(p_light->sr),1,f);
  ffwrite(&p_light->sg,sizeof(p_light->sg),1,f);
  ffwrite(&p_light->sb,sizeof(p_light->sb),1,f);
  ffwrite(&p_light->an_flag,sizeof(p_light->an_flag),1,f);
  ffwrite(&p_light->an.endtime,sizeof(p_light->an.endtime),1,f);
  if(p_light->an.endtime) {
    lo_chunk_save_dyn_light_anim(f, &p_light->an,TRUE);
  }

  return(TRUE);
}

int lo_chunk_save_extra_light_mesh(FFILE f, EXTRA_DYN_LIGHT *p_light)
{
  OUT_CHUNK ch;  

  while(p_light) {
    ch.typ = CHUNK_EXTRA_LIGHT_MESH;
    ch.velikost = sizeof(ch)+sizeof(p_light->akt)+sizeof(byte)+
      sizeof(p_light->flag)+sizeof(p_light->mflag)+
      sizeof(p_light->ktrida)+sizeof(p_light->np)+
      sizeof(p_light->dir)+sizeof(p_light->dosah)+
      sizeof(p_light->theta)+sizeof(p_light->utlum)+
      sizeof(p_light->gourand)+4*sizeof(p_light->da)+
      sizeof(p_light->sr)+sizeof(p_light->sg)+sizeof(p_light->sb)+
      sizeof(p_light->an_flag)+sizeof(p_light->an.endtime);        
    
    if(p_light->an.endtime) {
      ch.velikost += lo_chunk_get_dyn_light_size(f, &p_light->an);
    }

    ffwrite(&ch,sizeof(ch),1,f);
    ffwrite(&p_light->akt,sizeof(p_light->akt),1,f);
    ffwrite(p_light->jmeno,sizeof(byte),strlen(p_light->jmeno)+1,f);
    ffwrite(&p_light->flag,sizeof(p_light->flag),1,f);
    ffwrite(&p_light->mflag,sizeof(p_light->mflag),1,f);
    ffwrite(&p_light->ktrida,sizeof(p_light->ktrida),1,f);
    ffwrite(&p_light->np,sizeof(p_light->np),1,f);
    ffwrite(&p_light->dir,sizeof(p_light->dir),1,f);
    ffwrite(&p_light->dosah,sizeof(p_light->dosah),1,f);
    ffwrite(&p_light->theta,sizeof(p_light->theta),1,f);
    ffwrite(&p_light->utlum,sizeof(p_light->utlum),1,f);
    ffwrite(&p_light->gourand,sizeof(p_light->gourand),1,f);
    ffwrite(&p_light->da,sizeof(p_light->da),4,f);
    ffwrite(&p_light->sr,sizeof(p_light->sr),1,f);
    ffwrite(&p_light->sg,sizeof(p_light->sg),1,f);
    ffwrite(&p_light->sb,sizeof(p_light->sb),1,f);
    ffwrite(&p_light->an_flag,sizeof(p_light->an_flag),1,f);
    ffwrite(&p_light->an.endtime,sizeof(p_light->an.endtime),1,f);
    if(p_light->an.endtime) {
      lo_chunk_save_dyn_light_anim(f, &p_light->an,TRUE);
    }
    p_light = p_light->p_next;
  }
  return(TRUE);
}

/* **********************************************************************
   Ulozeni mlho-kostek
*/
void lo_chunk_save_mlhokostku(FFILE f, MLZNA_KOSTKA *p_top, int kont)
{   
  OUT_CHUNK ch;    
  int kid;

  while(p_top) {
    
    if(!kont && p_top->p_kont) {
      p_top = p_top->p_next; // uloz vsechny mlho-kostky bez kontejnerovych
      continue;
    }

    ch.typ = CHUNK_MLHOKOSTKA_2;
    ch.velikost = sizeof(ch)+
      sizeof(byte)+
      sizeof(BOD)+
      sizeof(BOD)+
      sizeof(p_top->flag)+
      sizeof(p_top->mod)+
      sizeof(p_top->priorita)+
      sizeof(p_top->start)+
      sizeof(p_top->stop)+
      sizeof(p_top->intenzita)+
      sizeof(float)*4+
      sizeof(int)+sizeof(int);
    
    ffwrite(&ch,sizeof(ch),1,f);
    ffwrite(p_top->jmeno,sizeof(byte),strlen(p_top->jmeno)+1,f);
    ffwrite(&p_top->min,sizeof(BOD),1,f);
    ffwrite(&p_top->max,sizeof(BOD),1,f);
    ffwrite(&p_top->flag,sizeof(p_top->flag),1,f);
    ffwrite(&p_top->mod,sizeof(p_top->mod),1,f);
    ffwrite(&p_top->priorita,sizeof(p_top->priorita),1,f);
    ffwrite(&p_top->start,sizeof(p_top->start),1,f);
    ffwrite(&p_top->stop,sizeof(p_top->stop),1,f);
    ffwrite(&p_top->intenzita,sizeof(p_top->intenzita),1,f);
    ffwrite(&p_top->r,sizeof(float)*4,1,f);
    
    kid = p_top->p_kont ? 1 : K_CHYBA;
    ffwrite(&kid,sizeof(kid),1,f); 
    
    ffwrite(&p_top->poly,sizeof(p_top->poly),1,f);
    
    if(kont) // pokud je kont - uloz 1 kostku kontejneru
      return;
    p_top = p_top->p_next;
  }  
}


/* **********************************************************************
   Ulozeni flare
*/
void lo_chunk_save_flare(FFILE f, LENS_FLARE *p_flare, EDIT_MATERIAL **p_mat, int light)
{ 
  OUT_CHUNK ch;    
  char pom[200];
  int  nic = K_CHYBA;

  if(!p_flare)
    return;

  strcpy(pom,p_mat[p_flare->material]->jmeno);
  ch.typ = light ? CHUNK_FLARE_LIGHT_EXT : CHUNK_FLARE_EXT;
  ch.velikost = sizeof(ch)+
                sizeof(BOD)+
                sizeof(float)*4+
                sizeof(float)*2+
                sizeof(int)+
                sizeof(int)+
                sizeof(int)+
                strlen(pom)+1;

  ffwrite(&ch,sizeof(ch),1,f);
  ffwrite(&p_flare->p,sizeof(BOD),1,f);      // umisteni
  ffwrite(&p_flare->r,sizeof(float),4,f);    // barva
  ffwrite(&p_flare->dx,sizeof(float),2,f);   // rozmery
  ffwrite(&nic,sizeof(int),1,f); // navazani  
  ffwrite(&p_flare->alfa,sizeof(int),1,f);
  ffwrite(&p_flare->zflag,sizeof(int),1,f);
  ffwrite(pom,sizeof(char),strlen(pom)+1,f);  
}

void lo_chunk_save_flare_mesh(FFILE f, LENS_FLARE *p_flare, EDIT_MATERIAL **p_mat)
{ 
  OUT_CHUNK ch;    
  char pom[200];
  int  nic = K_CHYBA;

  if(!p_flare)
    return;

  strcpy(pom,p_mat[p_flare->material]->jmeno);
  ch.typ = CHUNK_FLARE_MESH_EXT;
  ch.velikost = sizeof(ch)+
                sizeof(BOD)+
                sizeof(float)*4+
                sizeof(float)*2+
                sizeof(int)+
                sizeof(int)+
                sizeof(int)+
                strlen(pom)+1;

  ffwrite(&ch,sizeof(ch),1,f);
  ffwrite(&p_flare->p,sizeof(BOD),1,f);      // umisteni
  ffwrite(&p_flare->r,sizeof(float),4,f);    // barva
  ffwrite(&p_flare->dx,sizeof(float),2,f);   // rozmery
  //ffwrite(&p_flare->svetlo,sizeof(int),1,f); // navazani
  ffwrite(&nic,sizeof(int),1,f); // navazani  
  ffwrite(&p_flare->alfa,sizeof(int),1,f);
  ffwrite(&p_flare->zflag,sizeof(int),1,f);
  ffwrite(pom,sizeof(char),strlen(pom)+1,f);  
}

void lo_chunk_save_flare_slozeny(FFILE f, LENS_FLARE *p_flare, EDIT_MATERIAL **p_mat)
{
  OUT_CHUNK ch;
  char      pom[SLOZ_FLARE][200];
  int       s,d = 0;

  if(!p_flare || !p_flare->p_sloz)
    return;

  for(s = 0; s < SLOZ_FLARE; s++) {
    if(p_flare->p_sloz[s].vzdal > 0.0f)
      strcpy(pom[s],p_mat[p_flare->p_sloz[s].material]->jmeno);
    else
      pom[s][0] = 0;
    d += strlen(pom[s])+1;
  }

  ch.typ = CHUNK_FLARE_SLOZENY;
  ch.velikost = sizeof(ch)+
                sizeof(float)*SLOZ_FLARE+
                sizeof(float)*4*SLOZ_FLARE+
                sizeof(float)*2*SLOZ_FLARE+
                d;

  ffwrite(&ch,sizeof(ch),1,f);
  for(s = 0; s < SLOZ_FLARE; s++) {
    ffwrite(&p_flare->p_sloz[s].vzdal,sizeof(float),1,f);
    ffwrite(&p_flare->p_sloz[s].r,sizeof(float),4,f);
    ffwrite(&p_flare->p_sloz[s].dx,sizeof(float),2,f);
    ffwrite(pom[s],sizeof(char),strlen(pom[s])+1,f);
  }
}


/* **********************************************************************
   Ulozeni ambientu
*/
void lo_chunk_save_ambient(FFILE f, dword ambient)
{ 
  OUT_CHUNK ch;    
  ch.typ = CHUNK_AMBIENT;
  ch.velikost = sizeof(ch)+sizeof(ambient);
  ffwrite(&ch,sizeof(ch),1,f);
  ffwrite(&ambient,sizeof(ambient),1,f);
}

/* **********************************************************************
   Ulozeni ambientu
*/
void lo_chunk_save_lc_ext(FFILE f, LEVEL_KONFIG *p_lc)
{ 
  OUT_CHUNK ch;    
  ch.typ = CHUNK_LEVEL_KONFIG_EXT;
  ch.velikost = sizeof(ch)+
                sizeof(p_lc->barva_pozadi)+
                sizeof(p_lc->barva_pozadi_aktivni)+
                sizeof(p_lc->zm.aktivni)+
                sizeof(p_lc->zm.v_start)+
                sizeof(p_lc->zm.v_stop)+
                sizeof(p_lc->zm.h_start)+
                sizeof(p_lc->zm.h_stop)+
                sizeof(p_lc->zm.barva);

  ffwrite(&ch,sizeof(ch),1,f);
  ffwrite(&p_lc->barva_pozadi,sizeof(p_lc->barva_pozadi),1,f);
  ffwrite(&p_lc->barva_pozadi_aktivni,sizeof(p_lc->barva_pozadi_aktivni),1,f);
  ffwrite(&p_lc->zm.aktivni,sizeof(p_lc->zm.aktivni),1,f);
  ffwrite(&p_lc->zm.v_start,sizeof(p_lc->zm.v_start),1,f);
  ffwrite(&p_lc->zm.v_stop,sizeof(p_lc->zm.v_stop),1,f);
  ffwrite(&p_lc->zm.h_start,sizeof(p_lc->zm.h_start),1,f);
  ffwrite(&p_lc->zm.h_stop,sizeof(p_lc->zm.h_stop),1,f);
  ffwrite(&p_lc->zm.barva,sizeof(p_lc->zm.barva),1,f);
}

/* **********************************************************************
   Ulozeni zrcadla
*/
void lo_chunk_preved_zrcadlo(EDIT_KONTEJNER **p_kont, int kontnum, ZDRCADLO_DESC *p_desc)
{
  ZDRCADLO_DESC_POLY *p_tmp = p_desc->p_poly;  
  while(p_tmp) {
    int k = p_tmp->zrcadlo_k;
    if(k != K_CHYBA && k < kontnum && p_kont[k]) {
      p_tmp->id_kont = p_kont[k]->kontejner_ID;
    } else {
      p_tmp->id_kont = K_CHYBA;
    }
    p_tmp = p_tmp->p_next;
  }
  
}

void lo_chunk_save_zrcadlo_sub(FFILE f, ZDRCADLO_DESC_POLY *p_desc)
{ 
  OUT_CHUNK ch;
  ch.typ = CHUNK_ZRCADLO_3_POLOZKA;
  ch.velikost = sizeof(ch)+sizeof(p_desc[0]);
  ffwrite(&ch,sizeof(ch),1,f);
  ffwrite(p_desc,sizeof(p_desc[0]),1,f);
}

void lo_chunk_save_zrcadlo(FFILE f, ZDRCADLO_DESC *p_desc)
{ 
  ZDRCADLO_DESC_POLY *p_poly;
  OUT_CHUNK ch;
  
  ch.typ = CHUNK_ZRCADLO_3;
  ch.velikost = sizeof(ch)+sizeof(ZDRCADLO_DESC);
  
  ffwrite(&ch,sizeof(ch),1,f);
  ffwrite(p_desc,sizeof(ZDRCADLO_DESC),1,f);
  
  p_poly = p_desc->p_poly;
  while(p_poly) {
    lo_chunk_save_zrcadlo_sub(f, p_poly);
    p_poly = p_poly->p_next;
  }
}

/* **********************************************************************
   Ulozeni svetel
*/

int lo_chunk_save_static_light(FFILE f, STATIC_LIGHT *p_light, EDIT_MATERIAL **p_mat)
{
  LENS_FLARE *p_flare;
  OUT_CHUNK   ch;
  int j;

  ch.typ = CHUNK_STATIC_LIGHT;
  ch.velikost = sizeof(ch)+sizeof(p_light->akt)+strlen(p_light->jmeno)+1+
                sizeof(p_light->index)+sizeof(p_light->p)+sizeof(float)*3+
                sizeof(float)*4+sizeof(p_light->uhel)+sizeof(p_light->utlum)
                +sizeof(p_light->flag)+sizeof(p_light->flag2);
  
  ffwrite(&ch,sizeof(ch),1,f);
  ffwrite(&p_light->akt,sizeof(p_light->akt),1,f);
  ffwrite(p_light->jmeno,sizeof(char),strlen(p_light->jmeno)+1,f);
  ffwrite(&p_light->index,sizeof(p_light->index),1,f);
  ffwrite(&p_light->p,sizeof(p_light->p),1,f);
  ffwrite(&p_light->r,sizeof(float),1,f);
  ffwrite(&p_light->g,sizeof(float),1,f);
  ffwrite(&p_light->b,sizeof(float),1,f);  
  ffwrite(&p_light->k1,sizeof(float),4,f);
  ffwrite(&p_light->uhel,sizeof(p_light->uhel),1,f);
  ffwrite(&p_light->utlum,sizeof(p_light->utlum),1,f);
  ffwrite(&p_light->flag,sizeof(p_light->flag),1,f);
  ffwrite(&p_light->flag2,sizeof(p_light->flag2),1,f);
/*
  p_flare = (LENS_FLARE *)p_light->p_flare;
  if(p_flare && p_flare->akt) {      
    lo_uloz_material_chunk(f, p_flare->p_mat);
    lo_chunk_save_flare(f,p_flare,p_mat,TRUE);
    if(p_flare->p_sloz) {
      for(j = 0; j < SLOZ_FLARE; j++) {
        if(p_flare->p_sloz[j].vzdal > 0.0f) 
          lo_uloz_material_chunk(f, p_flare->p_sloz[j].p_mat);
      }
      lo_chunk_save_flare_slozeny(f,p_flare,p_mat);
    }
  }
*/
  return(TRUE);
}

int lo_chunk_save_static_light_mesh(FFILE f, STATIC_LIGHT *p_light, EDIT_MATERIAL **p_mat)
{
  LENS_FLARE *p_flare;
  OUT_CHUNK   ch;
  int         j;

  while(p_light) {
    
    /* Ulozeni statickeho svetla
    */
    
    ch.typ = CHUNK_STATIC_LIGHT_MESH;
    ch.velikost = sizeof(ch)+sizeof(p_light->akt)+strlen(p_light->jmeno)+1+
      sizeof(p_light->index)+sizeof(p_light->p)+sizeof(float)*3+
      sizeof(float)*4+sizeof(p_light->uhel)+sizeof(p_light->utlum)
      +sizeof(p_light->flag)+sizeof(p_light->flag2);
    
    ffwrite(&ch,sizeof(ch),1,f);
    ffwrite(&p_light->akt,sizeof(p_light->akt),1,f);
    ffwrite(p_light->jmeno,sizeof(char),strlen(p_light->jmeno)+1,f);
    ffwrite(&p_light->index,sizeof(p_light->index),1,f);
    ffwrite(&p_light->p,sizeof(p_light->p),1,f);
    ffwrite(&p_light->r,sizeof(float),1,f);
    ffwrite(&p_light->g,sizeof(float),1,f);
    ffwrite(&p_light->b,sizeof(float),1,f);  
    ffwrite(&p_light->k1,sizeof(float),4,f);
    ffwrite(&p_light->uhel,sizeof(p_light->uhel),1,f);
    ffwrite(&p_light->utlum,sizeof(p_light->utlum),1,f);
    ffwrite(&p_light->flag,sizeof(p_light->flag),1,f);
    ffwrite(&p_light->flag2,sizeof(p_light->flag2),1,f);
    
    /* Ulozeni flaru ke statickemu svetlu
    */
    /*
    p_flare = (LENS_FLARE *)p_light->p_flare;
    if(p_flare && p_flare->akt) {      
      lo_uloz_material_chunk(f, p_flare->p_mat);
      lo_chunk_save_flare_mesh(f,p_flare,p_mat);
      if(p_flare->p_sloz) {
        for(j = 0; j < SLOZ_FLARE; j++) {
          if(p_flare->p_sloz[j].vzdal > 0.0f) 
            lo_uloz_material_chunk(f, p_flare->p_sloz[j].p_mat);
        }
        lo_chunk_save_flare_slozeny(f,p_flare,p_mat);
      }
    }
      */  
    /* Dalsi svetlo
    */
    p_light = p_light->p_next;
  }
  return(TRUE);
}

/************************************************************************
  Ukladani materialu
*/

int lo_chunk_save_mat(FFILE f, EDIT_MATERIAL *p_mat)
{
  OUT_CHUNK ch;  
  ch.typ = CHUNK_MATERIAL;
  ch.velikost = sizeof(ch)+strlen(p_mat->jmeno)+1;  
  ffwrite(&ch,sizeof(ch),1,f);
  
  strlwr(p_mat->jmeno);
  
  ffwrite(p_mat->jmeno,sizeof(char),strlen(p_mat->jmeno)+1,f);
  return(TRUE);
}

int lo_chunk_save_mat_flags(FFILE f, EDIT_MATERIAL *p_mat)
{
  OUT_CHUNK ch;  
  
  ch.typ = CHUNK_FLAGS;
  ch.velikost = sizeof(ch)+sizeof(p_mat->flag)+sizeof(p_mat->flag2);
  
  ffwrite(&ch,sizeof(ch),1,f);
  ffwrite(&p_mat->flag,sizeof(p_mat->flag),1,f);
  ffwrite(&p_mat->flag2,sizeof(p_mat->flag2),1,f);
  
  return(TRUE);
}

int lo_chunk_save_mat_smat_pos(FFILE f, EDIT_MATERIAL *p_mat)
{
  OUT_CHUNK ch;  

  if(p_mat->flag&MAT_SCENE) {
    ch.typ = CHUNK_MAT_SMAT_POS;
    ch.velikost = sizeof(ch)+sizeof(p_mat->smaterial_pozice);
    ffwrite(&ch,sizeof(ch),1,f);
    ffwrite(&p_mat->smaterial_pozice,sizeof(p_mat->smaterial_pozice),1,f);
  }
  return(TRUE);
}

int lo_chunk_save_mat_color(FFILE f, EDIT_MATERIAL *p_mat)
{
  MATERIAL *p_dxmat;
  char      string[16];
  OUT_CHUNK ch;
  
  ch.typ = CHUNK_COLOR_EXT;
  ch.velikost = sizeof(ch)+16;
  
  p_dxmat = &p_mat->dxmat;
  
  string[0] = (byte)(p_dxmat->ambient_r*MAX_BYTE);
  string[1] = (byte)(p_dxmat->ambient_g*MAX_BYTE);
  string[2] = (byte)(p_dxmat->ambient_b*MAX_BYTE);
  string[3] = (byte)(1.0f*MAX_BYTE);

  string[4] = (byte)(p_dxmat->diffuse_r*MAX_BYTE);
  string[5] = (byte)(p_dxmat->diffuse_g*MAX_BYTE);
  string[6] = (byte)(p_dxmat->diffuse_b*MAX_BYTE);
  string[7] = (byte)(p_dxmat->diffuse_a*MAX_BYTE);

  string[8] = (byte)(p_dxmat->specular_r*MAX_BYTE);
  string[9] = (byte)(p_dxmat->specular_g*MAX_BYTE);
  string[10] = (byte)(p_dxmat->specular_b*MAX_BYTE);
  string[11] = (byte)(1.0f*MAX_BYTE);
  
  string[12] = (byte)(p_dxmat->faktor_r*MAX_BYTE);
  string[13] = (byte)(p_dxmat->faktor_g*MAX_BYTE);
  string[14] = (byte)(p_dxmat->faktor_b*MAX_BYTE);
  string[15] = (byte)(p_dxmat->faktor_a*MAX_BYTE);
  
  ffwrite(&ch,sizeof(ch),1,f);
  ffwrite(string,sizeof(char),16,f);
  
  return(TRUE);
}

int lo_chunk_save_mat_matanim_frame(FFILE f, ANIM_FRAME *p_fram, int frame)
{  
  EDIT_TEXT *p_text;
  OUT_CHUNK  ch;
  int        flag;
/*
  if(p_fram) {
    p_text = p_fram->p_text;
    ch.typ = CHUNK_MATANIM_EFRAME;
    ch.velikost = sizeof(ch)+strlen(p_text->jmeno)+1+sizeof(p_text->flag)
                 +sizeof(p_fram->time)+sizeof(p_fram->alfa)+sizeof(p_fram->u)
                 +sizeof(p_fram->v)+sizeof(p_fram->flag)+sizeof(frame);

    ffwrite(&ch,sizeof(ch),1,f);
    ffwrite(&frame,sizeof(frame),1,f);
    ffwrite(&p_fram->time,sizeof(p_fram->time),1,f);
    ffwrite(&p_fram->alfa,sizeof(p_fram->alfa),1,f);
    ffwrite(&p_fram->u,sizeof(p_fram->u),1,f);
    ffwrite(&p_fram->v,sizeof(p_fram->v),1,f);
    ffwrite(&p_fram->flag,sizeof(p_fram->flag),1,f);

    //Prdy z vody - ulozim rovnou texturu
    p_text = p_fram->p_text;
    flag = p_text->flag;
    ffwrite(&p_text->jmeno,sizeof(char),strlen(p_text->jmeno)+1,f);
    ffwrite(&flag,sizeof(flag),1,f);
    return(TRUE);
  } else
    return(FALSE);
  */  
}

int lo_chunk_save_mat_matanim(FFILE f, EDIT_MATERIAL *p_mat)
{
  ANIM_MATERIAL *p_anim = &p_mat->anim;
  OUT_CHUNK ch;  
  int i;
  
  if(p_mat->flag&MAT_ANIM_FRAME) {
    
    ch.typ = CHUNK_MATANIM_EXT;
    ch.velikost = sizeof(ch)+strlen(p_mat->anim.jmeno)+1+sizeof(p_anim->framenum);    
    
    ffwrite(&ch,sizeof(ch),1,f);
    ffwrite(p_anim->jmeno,sizeof(char),strlen(p_anim->jmeno)+1,f);
    ffwrite(&p_anim->framenum,sizeof(p_anim->framenum),1,f);        

    for(i = 0; i < p_anim->framenum; i++) {      
      lo_chunk_save_mat_matanim_frame(f, p_anim->p_frame+i, i);
    }
    
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_save_mat_matanim_text_dal(FFILE f, ANIM_TEXT *p_track, int typ)
{
  OUT_CHUNK ch;
  int       velikost = 0;
  if(p_track->pos_keys && p_track->p_pos) {
    velikost += sizeof(p_track->p_pos[0])*p_track->pos_keys+
                sizeof(p_track->p_pkeys[0])*p_track->pos_keys;
  }  
  if(p_track->piv_keys && p_track->p_piv) {
    velikost += sizeof(p_track->p_piv[0])*p_track->piv_keys+
                sizeof(p_track->p_vkeys[0])*p_track->piv_keys;
  }  
  if(p_track->rot_keys && p_track->p_rot) {
    velikost += sizeof(p_track->p_rot[0])*p_track->rot_keys+
                sizeof(p_track->p_rkeys[0])*p_track->rot_keys;
  }  
  if(p_track->scs_keys && p_track->p_scale) {
    velikost += sizeof(p_track->p_scale[0])*p_track->scs_keys+
                sizeof(p_track->p_skeys[0])*p_track->scs_keys;
  }
  ch.typ = typ;  
  ch.velikost = sizeof(ch)+sizeof(p_track[0])+velikost;
  ffwrite(&ch,sizeof(ch),1,f);
  ffwrite(p_track,sizeof(p_track[0]),1,f);  
  if(p_track->pos_keys && p_track->p_pos) {
    ffwrite(p_track->p_pos,sizeof(p_track->p_pos[0]),p_track->pos_keys,f);
    ffwrite(p_track->p_pkeys,sizeof(p_track->p_pkeys[0]),p_track->pos_keys,f);
  }  
  if(p_track->piv_keys && p_track->p_piv) {
    ffwrite(p_track->p_piv,sizeof(p_track->p_piv[0]),p_track->piv_keys,f);
    ffwrite(p_track->p_vkeys,sizeof(p_track->p_vkeys[0]),p_track->piv_keys,f);
  }  
  if(p_track->rot_keys && p_track->p_rot) {
    ffwrite(p_track->p_rot,sizeof(p_track->p_rot[0]),p_track->rot_keys,f);
    ffwrite(p_track->p_rkeys,sizeof(p_track->p_rkeys[0]),p_track->rot_keys,f);
  }  
  if(p_track->scs_keys && p_track->p_scale) {
    ffwrite(p_track->p_scale,sizeof(p_track->p_scale[0]),p_track->scs_keys,f);
    ffwrite(p_track->p_skeys,sizeof(p_track->p_skeys[0]),p_track->scs_keys,f);
  }
  return(TRUE);
}

/* Ulozi animace textur  
*/
int lo_chunk_save_mat_matanim_text(FFILE f, EDIT_MATERIAL *p_mat)
{
  int i,j;
  for(j = 0; j < MAT_TEXTUR; j++) {
    if(p_mat->flag&glstav_posun_flagy2[j]) {
      for(i = 0; i < MAX_TEXT_ANIM; i++) {
        if(p_mat->p_atext[j][i]) {
          lo_chunk_save_mat_matanim_text_dal(f, p_mat->p_atext[j][i],CHUNK_MATANIM_T1+j);
        }
      }
    }  
  }
  return(TRUE);
}

int lo_chunk_save_mat_textmatrix(FFILE f, EDIT_MATERIAL *p_mat)
{
  OUT_CHUNK ch;  
  ch.typ = CHUNK_MATMATRIX_EXT;
  ch.velikost = sizeof(ch)+4*sizeof(p_mat->ttext[0]);
  ffwrite(&ch,sizeof(ch),1,f);
  ffwrite(p_mat->ttext,sizeof(p_mat->ttext[0]),1,f);
  ffwrite(p_mat->ttext+1,sizeof(p_mat->ttext[0]),1,f);
  ffwrite(p_mat->ttext+2,sizeof(p_mat->ttext[0]),1,f);
  ffwrite(p_mat->ttext+3,sizeof(p_mat->ttext[0]),1,f);
  return(TRUE);
}

int lo_chunk_save_mat_texgen_param(FFILE f, EDIT_MATERIAL *p_mat)
{
  OUT_CHUNK ch;  
  ch.typ = CHUNK_MAT_TEXGEN;
  ch.velikost = sizeof(ch)+4*sizeof(p_mat->texgen_koord_t);
  ffwrite(&ch,sizeof(ch),1,f);
  ffwrite(p_mat->texgen_koord_t,sizeof(p_mat->texgen_koord_t),1,f);
  ffwrite(p_mat->texgen_koord_s,sizeof(p_mat->texgen_koord_s),1,f);
  ffwrite(p_mat->texgen_koord_r,sizeof(p_mat->texgen_koord_r),1,f);
  ffwrite(p_mat->texgen_koord_q,sizeof(p_mat->texgen_koord_q),1,f);
  return(TRUE);
}

int lo_chunk_save_mat_blend(FFILE f, EDIT_MATERIAL *p_mat)
{
  OUT_CHUNK ch;  
  ch.typ = CHUNK_ALFA_STAGE;
  ch.velikost = sizeof(ch)+sizeof(p_mat->alfa_state);
  ffwrite(&ch,sizeof(ch),1,f);
  ffwrite(&p_mat->alfa_state,sizeof(p_mat->alfa_state),1,f);
  return(TRUE);
}
  
int lo_chunk_save_mat_env_spec(FFILE f, EDIT_MATERIAL *p_mat)
{
  OUT_CHUNK ch;
  
  ch.typ = CHUNK_ENVSPEC;
  ch.velikost = sizeof(ch)+
                sizeof(p_mat->env_add1)+
                sizeof(p_mat->env_add2)+
                sizeof(p_mat->env_scale)+
                sizeof(p_mat->env_r)+
                sizeof(p_mat->env_g)+
                sizeof(p_mat->env_b);
  
  ffwrite(&ch,sizeof(ch),1,f);
  ffwrite(&p_mat->env_add1,sizeof(p_mat->env_add1),1,f);
  ffwrite(&p_mat->env_add2,sizeof(p_mat->env_add2),1,f);
  ffwrite(&p_mat->env_scale,sizeof(p_mat->env_scale),1,f);
  ffwrite(&p_mat->env_r,sizeof(p_mat->env_r),1,f);
  ffwrite(&p_mat->env_g,sizeof(p_mat->env_g),1,f);
  ffwrite(&p_mat->env_b,sizeof(p_mat->env_b),1,f);
  
  return(TRUE);
}


int lo_chunk_save_mat_text_blend_ext(FFILE f, EDIT_MATERIAL *p_mat, int num)
{
/*
  OUT_CHUNK ch;  
  ch.typ = CHUNK_TEXT_STAGE_AD_1+num;
  ch.velikost = sizeof(ch)+sizeof(int)*1+sizeof(int)*4+sizeof(int)*4+
                sizeof(int)*4+sizeof(p_mat->text_state[num].text_ati_bump_matrix[0])*4;
  ffwrite(&ch,sizeof(ch),1,f);
  ffwrite(&p_mat->text_state[num].text_stage,sizeof(int),1,f);
  ffwrite(&p_mat->text_state[num].textury,sizeof(int),4,f);
  ffwrite(&p_mat->text_state[num].text_koord,sizeof(int),4,f);
  ffwrite(&p_mat->text_state[num].text_funkce,sizeof(int),4,f);
  ffwrite(&p_mat->text_state[num].text_ati_bump_matrix,
          sizeof(p_mat->text_state[num].text_ati_bump_matrix[0]),4,f);
*/
  return(TRUE);
}

int lo_chunk_save_mat_text(FFILE f, EDIT_MATERIAL *p_mat, int num, int typ)
{
  OUT_CHUNK ch;
  if(p_mat->textfile[num][0]) {
    ch.typ = typ;
    ch.velikost = sizeof(ch)+strlen(p_mat->textfile[num])+1;
    ffwrite(&ch,sizeof(ch),1,f);
    ffwrite(p_mat->textfile[num],sizeof(char),strlen(p_mat->textfile[num])+1,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_save_mat_text_flag(FFILE f, EDIT_MATERIAL *p_mat, int num, int typ)
{
  OUT_CHUNK ch;
  if(p_mat) {
    ch.typ = typ;
    ch.velikost = sizeof(ch)+sizeof(p_mat->textflag[num]);
    ffwrite(&ch,sizeof(ch),1,f);
    ffwrite(p_mat->textflag+num,sizeof(p_mat->textflag[num]),1,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_save_mat_lesk(FFILE f, EDIT_MATERIAL *p_mat)
{
  OUT_CHUNK ch;  
  
  ch.typ = CHUNK_LESK;
  ch.velikost = sizeof(ch)+sizeof(float);
  
  ffwrite(&ch,sizeof(ch),1,f);
  ffwrite(&p_mat->lesk,sizeof(float),1,f);
  
  return(TRUE);
}


/* Ulozi material do souboru
*/
int lo_uloz_material_chunk(FFILE f, EDIT_MATERIAL *p_mat)
{    

  // hlavni info
  lo_chunk_save_mat(f, p_mat);
  lo_chunk_save_mat_flags(f, p_mat);
  lo_chunk_save_mat_color(f, p_mat);
  lo_chunk_save_mat_matanim(f, p_mat);
  lo_chunk_save_mat_matanim_text(f, p_mat);
  lo_chunk_save_mat_textmatrix(f, p_mat);  
  lo_chunk_save_mat_texgen_param(f, p_mat);
  lo_chunk_save_mat_blend(f, p_mat);
  lo_chunk_save_mat_env_spec(f, p_mat);  
  lo_chunk_save_mat_smat_pos(f, p_mat);
  
  // prvni textura
  lo_chunk_save_mat_text_blend_ext(f, p_mat, 0);
  lo_chunk_save_mat_text_flag(f, p_mat, 0, CHUNK_TEXT_FLAG_1);
  lo_chunk_save_mat_text(f, p_mat, 0, CHUNK_TEXT_1);
  
  // druha textura
  lo_chunk_save_mat_text_blend_ext(f, p_mat, 1);
  lo_chunk_save_mat_text_flag(f, p_mat, 1, CHUNK_TEXT_FLAG_2);
  lo_chunk_save_mat_text(f, p_mat, 1, CHUNK_TEXT_2);

  // treti textura
  lo_chunk_save_mat_text_blend_ext(f, p_mat, 2);
  lo_chunk_save_mat_text_flag(f, p_mat, 2, CHUNK_TEXT_FLAG_3);
  lo_chunk_save_mat_text(f, p_mat, 2, CHUNK_TEXT_3);

  // ctvrta textura
  lo_chunk_save_mat_text_blend_ext(f, p_mat, 3);
  lo_chunk_save_mat_text_flag(f, p_mat, 3, CHUNK_TEXT_FLAG_4);
  lo_chunk_save_mat_text(f, p_mat, 3, CHUNK_TEXT_4);

  return(TRUE);
}

/* *********************************************************************
    Ukladani animaci
*/
int lo_chunk_save_keyframe_root(FFILE f, HIERARCHY_ROOT *p_root)
{
  OUT_CHUNK ch;  
  if(p_root) {
    ch.typ = CHUNK_KEYFRAME_ROT2;
    ch.velikost = sizeof(ch)+sizeof(HIERARCHY_ROOT);
    ffwrite(&ch,sizeof(ch),1,f);
    ffwrite(p_root,sizeof(*p_root),1,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_save_keyframe_node(FFILE f, HIERARCHY_TRACK_INFO *p_node)
{
  char jmeno[200];
  char otec[200];
  OUT_CHUNK ch;  

  if(p_node) {
    strcpy(jmeno,((EDIT_OBJEKT *)(p_node->p_obj))->jmeno);   
    strcpy(otec,p_node->p_otec ? ((EDIT_OBJEKT *)(p_node->p_otec->p_obj))->jmeno : ROOT_NODE_JMENO);

    ch.typ = CHUNK_KEYFRAME_NODE;
    ch.velikost = sizeof(ch)+
                  sizeof(HIERARCHY_TRACK_INFO)+
                  strlen(jmeno)+1+strlen(otec)+1;
    ffwrite(&ch,sizeof(ch),1,f);
    
    ffwrite(jmeno,sizeof(char),strlen(jmeno)+1,f);
    ffwrite(otec,sizeof(char),strlen(otec)+1,f);
    ffwrite(p_node,sizeof(*p_node),1,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_save_keyframe_node_pos(FFILE f, HIERARCHY_TRACK_INFO *p_node)
{
  OUT_CHUNK ch;  

  if(p_node) {
    ch.typ = CHUNK_KEYFRAME_POS;
    ch.velikost = sizeof(ch)+
                  sizeof(p_node->p_pos[0])*p_node->pos_keys+
                  sizeof(p_node->p_pkeys[0])*p_node->pos_keys;
    ffwrite(&ch,sizeof(ch),1,f);    
    ffwrite(p_node->p_pos,sizeof(p_node->p_pos[0]),p_node->pos_keys,f);
    ffwrite(p_node->p_pkeys,sizeof(p_node->p_pkeys[0]),p_node->pos_keys,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_save_keyframe_node_rot(FFILE f, HIERARCHY_TRACK_INFO *p_node)
{
  OUT_CHUNK ch;  

  if(p_node) {
    ch.typ = CHUNK_KEYFRAME_ROT;
    ch.velikost = sizeof(ch)+
                  sizeof(p_node->p_at[0])*p_node->rot_keys+
                  sizeof(p_node->p_rkeys[0])*p_node->rot_keys;
    ffwrite(&ch,sizeof(ch),1,f);    
    ffwrite(p_node->p_at,sizeof(p_node->p_at[0]),p_node->rot_keys,f);
    ffwrite(p_node->p_rkeys,sizeof(p_node->p_rkeys[0]),p_node->rot_keys,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_save_keyframe_node_scale(FFILE f, HIERARCHY_TRACK_INFO *p_node)
{
  OUT_CHUNK ch;  

  if(p_node) {
    ch.typ = CHUNK_KEYFRAME_SCS;
    ch.velikost = sizeof(ch)+
                  sizeof(p_node->p_scale[0])*p_node->scs_keys+
                  sizeof(p_node->p_skeys[0])*p_node->scs_keys;
    ffwrite(&ch,sizeof(ch),1,f);    
    ffwrite(p_node->p_scale,sizeof(p_node->p_scale[0]),p_node->scs_keys,f);
    ffwrite(p_node->p_skeys,sizeof(p_node->p_skeys[0]),p_node->scs_keys,f);
    return(TRUE);
  } else
    return(FALSE);
}

/* Ulozi root vcetne vsech decek
*/
int lo_chunk_save_keyframe_child(FFILE f, HIERARCHY_TRACK_INFO *p_node)
{
  int i;

  lo_chunk_save_keyframe_node(f,p_node);
  lo_chunk_save_keyframe_node_pos(f,p_node);  
  lo_chunk_save_keyframe_node_rot(f,p_node);
  lo_chunk_save_keyframe_node_scale(f,p_node);

  for(i = 0; i < p_node->childnum; i++)
    lo_chunk_save_keyframe_child(f,p_node->p_child[i]);
  
  return(TRUE);
}

int lo_chunk_save_keyframe(FFILE f, HIERARCHY_ROOT *p_root)
{
  int i;
  lo_chunk_save_keyframe_root(f,p_root);
  for(i = 0; i < p_root->childnum; i++) {
    lo_chunk_save_keyframe_child(f, p_root->p_child[i]);
  }
  return(TRUE);
}


/************************************************************************
  Ulozeni sim-animaci

  #define  CHUNK_SIM_ROOT      2020 // hlavicka key-framove animace
  #define  CHUNK_SIM_NODE      2021 // vetev animace - pro jeden objekt
*/


/*
typedef struct _HIERARCHY_SIM {
  
  float     delka;         // delka casu animace (fps)
  byte      stav;          // 0 = stop, 1 = run
  
  int       flag;          // flagy animace (GL_LOOP,GL_REMOVE)

  dword     time;          // aktualni cas animace
  dword     time_start;    // aktualni start
  dword     time_stop;     // aktualni stop
  dword     time_delka;    // aktualni delka animace
  
  dword     start;
  dword     stop;  
  
  int       keynum;         // pocet klicu
  char      jmeno[MAX_JMENO]; // jmeno animace
  int       childnum;         // pocet detskych traku
  SIMPLE_TRACK_INFO *p_child;

} HIERARCHY_SIM;
*/
int lo_chunk_save_sim_root(FFILE f, HIERARCHY_SIM *p_sim)
{
  OUT_CHUNK ch;  
  if(p_sim) {
    ch.typ = CHUNK_SIM_ROOT;
    ch.velikost = sizeof(ch)+
                  sizeof(p_sim->flag)+
                  sizeof(p_sim->keynum)+
                  sizeof(p_sim->jmeno[0])*MAX_JMENO+
                  sizeof(p_sim->childnum);

    ffwrite(&ch,sizeof(ch),1,f);
    ffwrite(&p_sim->flag,sizeof(p_sim->flag),1,f);
    ffwrite(&p_sim->keynum,sizeof(p_sim->keynum),1,f);
    ffwrite(&p_sim->jmeno,sizeof(p_sim->jmeno[0]),MAX_JMENO,f);
    ffwrite(&p_sim->childnum,sizeof(p_sim->childnum),1,f);    
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_save_sim_node_ext_all_matrix(FFILE f, SIMPLE_TRACK_INFO *p_sim)
{ 
  if(p_sim && p_sim->p_all_matrix) {
    ffwrite(p_sim->p_all_matrix,sizeof(p_sim->p_all_matrix[0]),1,f);
    return(TRUE);
  } else {
    return(FALSE);
  }
}

int lo_chunk_save_sim_node_ext(FFILE f, SIMPLE_TRACK_INFO *p_sim)
{    
  OUT_CHUNK ch;  
  int       klicu,i;

  if(p_sim) {
    ch.typ = p_sim->p_all_matrix ? CHUNK_SIM_NODE_EXT2 : CHUNK_SIM_NODE_EXT;
    ch.velikost = sizeof(ch)+
      sizeof(p_sim->Objekt_ID)+
      sizeof(p_sim->flag)+
      sizeof(p_sim->keynum)+
      sizeof(p_sim->pivot)+
      sizeof(p_sim->pivot3ds)+
      sizeof(p_sim->childnum)+    
      sizeof(klicu)*3+(p_sim->p_pos ? sizeof(p_sim->p_pos[0])*p_sim->keynum:0)
      +(p_sim->p_scale ? sizeof(p_sim->p_scale[0])*p_sim->keynum : 0)
      +(p_sim->p_at ? sizeof(p_sim->p_at[0])*p_sim->keynum : 0)
      +(p_sim->p_norm ? sizeof(p_sim->p_norm[0])*p_sim->keynum : 0)
      +(p_sim->p_all_matrix ? sizeof(p_sim->p_all_matrix[0]) : 0);

    
    ffwrite(&ch,sizeof(ch),1,f);    
    ffwrite(&p_sim->Objekt_ID,sizeof(p_sim->Objekt_ID),1,f);
    ffwrite(&p_sim->flag,sizeof(p_sim->flag),1,f);
    ffwrite(&p_sim->keynum,sizeof(p_sim->keynum),1,f);
    ffwrite(&p_sim->pivot,sizeof(p_sim->pivot),1,f);
    ffwrite(&p_sim->pivot3ds,sizeof(p_sim->pivot3ds),1,f);
    ffwrite(&p_sim->childnum,sizeof(p_sim->childnum),1,f);

    klicu = p_sim->p_pos ? p_sim->keynum : 0;
    ffwrite(&klicu,sizeof(klicu),1,f);
    if(klicu)
      ffwrite(p_sim->p_pos,sizeof(p_sim->p_pos[0]),p_sim->keynum,f);
    
    klicu = p_sim->p_scale ? p_sim->keynum : 0;
    ffwrite(&klicu,sizeof(klicu),1,f);
    if(klicu)
      ffwrite(p_sim->p_scale,sizeof(p_sim->p_scale[0]),p_sim->keynum,f);
    
    klicu = p_sim->p_at ? p_sim->keynum : 0;
    ffwrite(&klicu,sizeof(klicu),1,f);
    if(klicu)
      ffwrite(p_sim->p_at,sizeof(p_sim->p_at[0]),p_sim->keynum,f);

    klicu = p_sim->p_norm ? p_sim->keynum : 0;
    ffwrite(&klicu,sizeof(klicu),1,f);
    if(klicu)
      ffwrite(p_sim->p_norm,sizeof(p_sim->p_norm[0]),p_sim->keynum,f);

    if(p_sim->p_all_matrix)
      lo_chunk_save_sim_node_ext_all_matrix(f, p_sim);

    for(i = 0; i < p_sim->childnum; i++)
      lo_chunk_save_sim_node_ext(f,p_sim->p_child+i);
    
    return(TRUE);
  } else
    return(FALSE);
}

/*
  Ulozi koren sim animace
*/
int lo_chunk_save_sim(FFILE f, HIERARCHY_SIM *p_sim)
{
  int i;
  lo_chunk_save_sim_root(f,p_sim);
  for(i = 0; i < p_sim->childnum; i++) {
    lo_chunk_save_sim_node_ext(f, p_sim->p_child+i);
  }
  return(TRUE);
}



/************************************************************************
  Ukladani meshu do souboru
*/
int lo_chunk_save_kont_jmeno(FFILE f, EDIT_KONTEJNER *p_kont, int pozice)
{
  OUT_CHUNK ch;
  if(p_kont) {
    ch.typ = CHUNK_KONT_JMENO;
    ch.velikost = sizeof(ch)+sizeof(p_kont->prvek)+strlen(p_kont->jmeno)+1;
    ffwrite(&ch,sizeof(ch),1,f);
    ffwrite(&pozice,sizeof(int),1,f);
    ffwrite(p_kont->jmeno,sizeof(char),strlen(p_kont->jmeno)+1,f);    
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_save_kont_low_jmeno(FFILE f, EDIT_KONTEJNER *p_kont)
{
  OUT_CHUNK ch;
  if(p_kont) {
    ch.typ = CHUNK_KONT_LOW_JMENO;
    ch.velikost = sizeof(ch)+strlen(p_kont->jmeno)+1;
    ffwrite(&ch,sizeof(ch),1,f);
    ffwrite(p_kont->jmeno,sizeof(char),strlen(p_kont->jmeno)+1,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_save_kont_low_id(FFILE f, EDIT_KONTEJNER *p_kont)
{
  OUT_CHUNK ch;
  if(p_kont) {
    ch.typ = CHUNK_KONT_LOW_ID;
    ch.velikost = sizeof(ch)+sizeof(p_kont->low_id);
    ffwrite(&ch,sizeof(ch),1,f);
    ffwrite(&p_kont->low_id,sizeof(p_kont->low_id),1,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_save_kont_id(FFILE f, int id)
{
  OUT_CHUNK ch;  
  ch.typ = CHUNK_KONT_ID;
  ch.velikost = sizeof(ch)+sizeof(id);
  ffwrite(&ch,sizeof(ch),1,f);
  ffwrite(&id,sizeof(id),1,f);
  return(TRUE);
}

int lo_chunk_save_kont_flag(FFILE f, EDIT_KONTEJNER *p_kont)
{
  OUT_CHUNK ch;
  if(p_kont) {
    ch.typ = CHUNK_KONT_FLAG;
    ch.velikost = sizeof(ch)+sizeof(p_kont->kflag);
    ffwrite(&ch,sizeof(ch),1,f);
    ffwrite(&p_kont->kflag,sizeof(p_kont->kflag),1,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_save_kont_flag_ext(FFILE f, EDIT_KONTEJNER *p_kont)
{
  OUT_CHUNK ch;
  if(p_kont) {
    ch.typ = CHUNK_KONT_FLAG_EXT;
    ch.velikost = sizeof(ch)+sizeof(p_kont->m1flag)+sizeof(p_kont->m2flag);
    ffwrite(&ch,sizeof(ch),1,f);
    ffwrite(&p_kont->m1flag,sizeof(p_kont->m1flag),1,f);
    ffwrite(&p_kont->m2flag,sizeof(p_kont->m2flag),1,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_save_kont_flag_ext_2(FFILE f, EDIT_KONTEJNER *p_kont)
{
  OUT_CHUNK ch;
  if(p_kont) {
    ch.typ = CHUNK_KONT_FLAG_EXT_2;
    ch.velikost = sizeof(ch)+sizeof(p_kont->k2flag);
    ffwrite(&ch,sizeof(ch),1,f);
    ffwrite(&p_kont->k2flag,sizeof(p_kont->k2flag),1,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_save_kont_obb(FFILE f, EDIT_KONTEJNER *p_kont)
{
  OUT_CHUNK ch;
  if(p_kont) {
    ch.typ = CHUNK_KONT_OBB;
    ch.velikost = sizeof(ch)+sizeof(p_kont->obb);
    ffwrite(&ch,sizeof(ch),1,f);
    ffwrite(&p_kont->obb,sizeof(p_kont->obb),1,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_save_kont_wmatrix(FFILE f, EDIT_KONTEJNER *p_kont, GLMATRIX *p_matrix)
{
  OUT_CHUNK ch;
  if(p_kont) {
    ch.typ = CHUNK_KONT_WMATRIX;
    ch.velikost = sizeof(ch)+sizeof(*p_matrix);
    ffwrite(&ch,sizeof(ch),1,f);
    ffwrite(p_matrix,sizeof(*p_matrix),1,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_save_kont_mpmatrix(FFILE f, EDIT_KONTEJNER *p_kont, GLMATRIX *p_matrix)
{
  OUT_CHUNK ch;
  if(p_kont && p_kont->kflag&KONT_MPMATRIX) {
    ch.typ = CHUNK_KONT_MPMATRIX;
    ch.velikost = sizeof(ch)+sizeof(*p_matrix);
    ffwrite(&ch,sizeof(ch),1,f);
    ffwrite(p_matrix,sizeof(*p_matrix),1,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_save_obj_jmeno(FFILE f, EDIT_OBJEKT *p_obj)
{
  OUT_CHUNK ch;
  if(p_obj) {
    ch.typ = CHUNK_OBJ_JMENO;
    ch.velikost = sizeof(ch)+strlen(p_obj->jmeno)+1;
    ffwrite(&ch,sizeof(ch),1,f);      
    ffwrite(p_obj->jmeno,sizeof(char),strlen(p_obj->jmeno)+1,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_save_obj_ID(FFILE f, EDIT_OBJEKT *p_obj)
{
  OUT_CHUNK ch;
  if(p_obj) {
    ch.typ = CHUNK_OBJ_ID;
    ch.velikost = sizeof(ch)+sizeof(p_obj->Objekt_ID);
    ffwrite(&ch,sizeof(ch),1,f);
    ffwrite(&p_obj->Objekt_ID,sizeof(p_obj->Objekt_ID),1,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_save_obj_pivot(FFILE f, EDIT_OBJEKT *p_obj)
{
  OUT_CHUNK ch;
  if(p_obj) {
    ch.typ = CHUNK_OBJ_PIVOT;
    ch.velikost = sizeof(ch)+sizeof(p_obj->pivot);
    ffwrite(&ch,sizeof(ch),1,f);
    ffwrite(&p_obj->pivot,sizeof(p_obj->pivot),1,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_save_obj_obb(FFILE f, EDIT_OBJEKT *p_obj)
{
  OUT_CHUNK ch;
  if(p_obj) {
    ch.typ = CHUNK_OBJ_OBB;
    ch.velikost = sizeof(ch)+sizeof(p_obj->obb);
    ffwrite(&ch,sizeof(ch),1,f);
    ffwrite(&p_obj->obb,sizeof(p_obj->obb),1,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_save_obj_material(FFILE f, EDIT_MATERIAL *p_mat)
{
  OUT_CHUNK ch;
  if(p_mat) {
    ch.typ = CHUNK_OBJ_MATERIAL;
    ch.velikost = sizeof(ch)+strlen(p_mat->jmeno)+1;
    ffwrite(&ch,sizeof(ch),1,f);    
    ffwrite(p_mat->jmeno,sizeof(char),strlen(p_mat->jmeno)+1,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_save_obj_matrix(FFILE f, GLMATRIX *p_mat)
{
  OUT_CHUNK ch;
  if(p_mat) {
    ch.typ = CHUNK_OBJ_MATRIX_KEY;
    ch.velikost = sizeof(ch)+sizeof(p_mat[0]);
    ffwrite(&ch,sizeof(ch),1,f);
    ffwrite(p_mat,sizeof(p_mat[0]),1,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_save_obj_vertex(FFILE f, EDIT_OBJEKT *p_obj, GLMATRIX *p_transf)
{
  OUT_CHUNK ch;
  dword pocet,i;
  float v[3];

  if(p_obj) {
    ch.typ = CHUNK_OBJ_VERTEXY;
    ch.velikost = sizeof(ch)+
                  sizeof(dword)+
                  sizeof(float)*3*p_obj->vertexnum;
    ffwrite(&ch,sizeof(ch),1,f);
    
    pocet = p_obj->vertexnum;
    ffwrite(&pocet,sizeof(pocet),1,f);

    for(i = 0; i < pocet; i++) {
      v[0] = p_obj->p_vertex[i].x;
      v[1] = p_obj->p_vertex[i].y;
      v[2] = p_obj->p_vertex[i].z;
      if(p_transf)
        transformuj_bod_matici(v,v+1,v+2,p_transf);
      if(!ffwrite(&v,sizeof(v),1,f))
        chyba("Zapis!");
    }
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_save_obj_face(FFILE f, EDIT_OBJEKT *p_obj)
{
  OUT_CHUNK ch;
  int pocet;  

  if(p_obj) {
    ch.typ = CHUNK_OBJ_FACY;
    ch.velikost = sizeof(ch)+
                  sizeof(dword)+
                  sizeof(word)*p_obj->facenum;
    ffwrite(&ch,sizeof(ch),1,f);
    pocet = p_obj->facenum;
    if(!ffwrite(&pocet,sizeof(pocet),1,f))
      chyba("Zapis");
    if(!ffwrite(p_obj->p_face,sizeof(p_obj->p_face[0]),pocet,f))
      chyba("Zapis");
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_save_obj_face_ext(FFILE f, EDIT_OBJEKT *p_obj)
{
  OUT_CHUNK ch;
  
  if(p_obj->p_opt) {
    ch.typ = CHUNK_OBJ_FACY_OPT;
    ch.velikost = sizeof(ch)+sizeof(p_obj->optnum)+
    sizeof(p_obj->p_opt[0])*p_obj->optnum;
    
    ffwrite(&ch,sizeof(ch),1,f);    
    ffwrite(&p_obj->optnum,sizeof(p_obj->optnum),1,f);
    ffwrite(p_obj->p_opt,sizeof(p_obj->p_opt[0]),p_obj->optnum,f);
    
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_save_obj_mapy_1(FFILE f, EDIT_OBJEKT *p_obj)
{
  OUT_CHUNK ch;
  float m[2];
  dword pocet,i;

  if(p_obj) {
    ch.typ = CHUNK_OBJ_MAPY_1;
    ch.velikost = sizeof(ch)+
                  sizeof(dword)+
                  sizeof(float)*2*p_obj->vertexnum;
    ffwrite(&ch,sizeof(ch),1,f);
    pocet = p_obj->vertexnum;
    if(!ffwrite(&pocet,sizeof(pocet),1,f))
      chyba("Zapis");
    for(i = 0; i < pocet; i++) {
      m[0] = p_obj->p_vertex[i].tu1;
      m[1] = p_obj->p_vertex[i].tv1;
      ffwrite(&m,sizeof(m),1,f);
    }    
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_save_obj_mapy_2(FFILE f, EDIT_OBJEKT *p_obj)
{
  OUT_CHUNK ch;
  float m[2];
  dword pocet,i;

  if(p_obj) {
    ch.typ = CHUNK_OBJ_MAPY_2;
    ch.velikost = sizeof(ch)+
                  sizeof(dword)+
                  sizeof(float)*2*p_obj->vertexnum;
    ffwrite(&ch,sizeof(ch),1,f);
    pocet = p_obj->vertexnum;
    if(!ffwrite(&pocet,sizeof(pocet),1,f))
      chyba("Zapis");
    for(i = 0; i < pocet; i++) {
      m[0] = p_obj->p_vertex[i].tu2;
      m[1] = p_obj->p_vertex[i].tv2;
      ffwrite(&m,sizeof(m),1,f);
    }    
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_save_obj_mapy_3(FFILE f, EDIT_OBJEKT *p_obj)
{
  OUT_CHUNK ch;
  float m[2];
  dword pocet,i;

  if(p_obj) {
    ch.typ = CHUNK_OBJ_MAPY_3;
    ch.velikost = sizeof(ch)+
                  sizeof(dword)+
                  sizeof(float)*2*p_obj->vertexnum;
    ffwrite(&ch,sizeof(ch),1,f);
    pocet = p_obj->vertexnum;
    if(!ffwrite(&pocet,sizeof(pocet),1,f))
      chyba("Zapis");
    for(i = 0; i < pocet; i++) {
      m[0] = p_obj->p_vertex[i].tu3;
      m[1] = p_obj->p_vertex[i].tv3;
      ffwrite(&m,sizeof(m),1,f);
    }    
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_save_obj_mapy_4(FFILE f, EDIT_OBJEKT *p_obj)
{
  OUT_CHUNK ch;
  float m[2];
  dword pocet,i;

  if(p_obj) {
    ch.typ = CHUNK_OBJ_MAPY_4;
    ch.velikost = sizeof(ch)+
                  sizeof(dword)+
                  sizeof(float)*2*p_obj->vertexnum;
    ffwrite(&ch,sizeof(ch),1,f);
    pocet = p_obj->vertexnum;
    if(!ffwrite(&pocet,sizeof(pocet),1,f))
      chyba("Zapis");
    for(i = 0; i < pocet; i++) {
      m[0] = p_obj->p_vertex[i].tu4;
      m[1] = p_obj->p_vertex[i].tv4;
      ffwrite(&m,sizeof(m),1,f);
    }    
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_save_obj_barvy(FFILE f, EDIT_OBJEKT *p_obj)
{
  OUT_CHUNK ch;
  dword pocet,i;
  float v[4] = {0,0,0,0};

  if(p_obj) {
    ch.typ = CHUNK_OBJ_BARVY;
    ch.velikost = sizeof(ch)+
                  sizeof(dword)+
                  sizeof(float)*4*p_obj->vertexnum;
    ffwrite(&ch,sizeof(ch),1,f);
    
    pocet = p_obj->vertexnum;
    ffwrite(&pocet,sizeof(pocet),1,f);

    for(i = 0; i < pocet; i++) {
      v[0] = p_obj->p_vertex[i].dr;
      v[1] = p_obj->p_vertex[i].dg;
      v[2] = p_obj->p_vertex[i].db;
      v[3] = 1.0f;
      if(!ffwrite(&v,sizeof(float),4,f))
        chyba("Zapis!");
    }

    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_save_obj_normal(FFILE f, EDIT_OBJEKT *p_obj)
{
  OUT_CHUNK ch;
  dword pocet,i;
  float v[3] = {0,0,0};

  if(p_obj) {
    ch.typ = CHUNK_OBJ_NORMAL;
    ch.velikost = sizeof(ch)+sizeof(pocet)+sizeof(float)*3*p_obj->vertexnum;
    
    ffwrite(&ch,sizeof(ch),1,f);    
    pocet = p_obj->vertexnum;
    ffwrite(&pocet,sizeof(pocet),1,f);

    for(i = 0; i < pocet; i++) {
      v[0] = p_obj->p_vertex[i].nx;
      v[1] = p_obj->p_vertex[i].ny;
      v[2] = p_obj->p_vertex[i].nz;
      if(!ffwrite(&v,sizeof(float),3,f))
        chyba("Zapis!");
    }

    return(TRUE);
  } else
    return(FALSE);
}

/* Ulozi rozsirene barvy
*/
int lo_chunk_save_obj_barvy_ext(FFILE f, EDIT_OBJEKT *p_obj)
{
  OUT_CHUNK ch;
  dword pocet,i;
  float v[4] = {0,0,0,0};

  if(p_obj) {
    ch.typ = CHUNK_OBJ_BARVY_EXT;
    ch.velikost = sizeof(ch)+
                  sizeof(dword)+
                  sizeof(float)*4*p_obj->vertexnum*4;
    ffwrite(&ch,sizeof(ch),1,f);
    
    pocet = p_obj->vertexnum;
    ffwrite(&pocet,sizeof(pocet),1,f);

    for(i = 0; i < pocet; i++) {
      // Uloz vsechny difusni barvy
      v[0] = p_obj->p_vertex[i].dr;
      v[1] = p_obj->p_vertex[i].dg;
      v[2] = p_obj->p_vertex[i].db;
      v[3] = p_obj->p_vertex[i].da;
      if(!ffwrite(&v,sizeof(float),4,f))
        chyba("Zapis!");
      
      v[0] = p_obj->p_vertex[i].mdr;
      v[1] = p_obj->p_vertex[i].mdg;
      v[2] = p_obj->p_vertex[i].mdb;
      v[3] = p_obj->p_vertex[i].mda;
      if(!ffwrite(&v,sizeof(float),4,f))
        chyba("Zapis!");
      
      // Uloz vsechny spekularni barvy
      v[0] = p_obj->p_vertex[i].sr;
      v[1] = p_obj->p_vertex[i].sg;
      v[2] = p_obj->p_vertex[i].sb;
      v[3] = 1.0f;//p_obj->p_vertex[i].sa;
      if(!ffwrite(&v,sizeof(float),4,f))
        chyba("Zapis!");
      
      v[0] = p_obj->p_vertex[i].msr;
      v[1] = p_obj->p_vertex[i].msg;
      v[2] = p_obj->p_vertex[i].msb;
      v[3] = 1.0f;//p_obj->p_vertex[i].msa;
      if(!ffwrite(&v,sizeof(float),4,f))
        chyba("Zapis!");
    }

    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_save_obj_joint_spline(FILE *f, JOINT_KEYS *p_keys)
{
  OUT_CHUNK ch;
  
  if(!p_keys)
    return(FALSE);

  if(p_keys->p_pos) {
    ch.typ = CHUNK_OBJ_JNT_KEYPOS;
    ch.velikost = sizeof(ch)+sizeof(p_keys->pos_keys)+p_keys->pos_keys*sizeof(p_keys->p_pos[0])+p_keys->pos_keys*sizeof(p_keys->p_pkeys[0]);
    ffwrite(&ch,sizeof(ch),1,f);
    ffwrite(&p_keys->pos_keys,sizeof(p_keys->pos_keys),1,f);
    ffwrite(p_keys->p_pos,sizeof(p_keys->p_pos[0]),p_keys->pos_keys,f);
    ffwrite(p_keys->p_pkeys,sizeof(p_keys->p_pkeys[0]),p_keys->pos_keys,f);
  }
  
  if(p_keys->p_rot) {
    ch.typ = CHUNK_OBJ_JNT_KEYROT;
    ch.velikost = sizeof(ch)+sizeof(p_keys->rot_keys)+p_keys->rot_keys*sizeof(p_keys->p_rot[0])+p_keys->rot_keys*sizeof(p_keys->p_rkeys[0]);
    ffwrite(&ch,sizeof(ch),1,f);
    ffwrite(&p_keys->rot_keys,sizeof(p_keys->rot_keys),1,f);
    ffwrite(p_keys->p_rot,sizeof(p_keys->p_rot[0]),p_keys->rot_keys,f);
    ffwrite(p_keys->p_rkeys,sizeof(p_keys->p_rkeys[0]),p_keys->rot_keys,f);
  }
  
  if(p_keys->p_scale) {
    ch.typ = CHUNK_OBJ_JNT_KEYSCS;
    ch.velikost = sizeof(ch)+sizeof(p_keys->scs_keys)+p_keys->scs_keys*sizeof(p_keys->p_scale[0])+p_keys->scs_keys*sizeof(p_keys->p_skeys[0]);
    ffwrite(&ch,sizeof(ch),1,f);
    ffwrite(&p_keys->scs_keys,sizeof(p_keys->scs_keys),1,f);
    ffwrite(p_keys->p_scale,sizeof(p_keys->p_scale[0]),p_keys->scs_keys,f);
    ffwrite(p_keys->p_skeys,sizeof(p_keys->p_skeys[0]),p_keys->scs_keys,f);
  }

  return(TRUE);
}

#define ROOT_ID 0

int lo_chunk_save_obj_joint(FILE *f, JOINT *p_joint, int id_prev)
{
  OUT_CHUNK ch;
  
  if(!p_joint)
    return(FALSE);  

  ch.typ = CHUNK_OBJ_JNT_ID;
  ch.velikost = sizeof(ch)+2*sizeof(p_joint->joint_ID);
  ffwrite(&ch,sizeof(ch),1,f);
  ffwrite(&p_joint->joint_ID,sizeof(p_joint->joint_ID),1,f);
  ffwrite(&id_prev,sizeof(id_prev),1,f);

  ch.typ = CHUNK_OBJ_JNT_PIVOT;
  ch.velikost = sizeof(ch)+sizeof(p_joint->pivot);
  ffwrite(&ch,sizeof(ch),1,f);
  ffwrite(&p_joint->pivot,sizeof(p_joint->pivot),1,f);

  if(p_joint->p_vertexlist) {
    ch.typ = CHUNK_OBJ_JNT_VERT;
    ch.velikost = sizeof(ch)+sizeof(p_joint->vertexnum)+p_joint->vertexnum*sizeof(p_joint->p_vertexlist[0]);
    ffwrite(&ch,sizeof(ch),1,f);
    ffwrite(&p_joint->vertexnum,sizeof(p_joint->vertexnum),1,f);
    ffwrite(p_joint->p_vertexlist,sizeof(p_joint->p_vertexlist[0]),p_joint->vertexnum,f);
  }
  
  ch.typ = CHUNK_OBJ_JNT_FLAG;
  ch.velikost = sizeof(ch)+sizeof(p_joint->flag);
  ffwrite(&ch,sizeof(ch),1,f);
  ffwrite(&p_joint->flag,sizeof(p_joint->flag),1,f);
  
  if(p_joint->p_pos) {
    ch.typ = CHUNK_OBJ_JNT_POS;
    ch.velikost = sizeof(ch)+sizeof(p_joint->pos_keys)+p_joint->pos_keys*sizeof(p_joint->p_pos[0]);
    ffwrite(&ch,sizeof(ch),1,f);
    ffwrite(&p_joint->pos_keys,sizeof(p_joint->pos_keys),1,f);
    ffwrite(p_joint->p_pos,sizeof(p_joint->p_pos[0]),p_joint->pos_keys,f);
  }
  
  if(p_joint->p_rot) {
    ch.typ = CHUNK_OBJ_JNT_ROT;
    ch.velikost = sizeof(ch)+sizeof(p_joint->rot_keys)+p_joint->rot_keys*sizeof(p_joint->p_rot[0]);
    ffwrite(&ch,sizeof(ch),1,f);
    ffwrite(&p_joint->rot_keys,sizeof(p_joint->rot_keys),1,f);
    ffwrite(p_joint->p_rot,sizeof(p_joint->p_rot[0]),p_joint->rot_keys,f);
  }
  
  if(p_joint->p_scs) {
    ch.typ = CHUNK_OBJ_JNT_SCS;
    ch.velikost = sizeof(ch)+sizeof(p_joint->scs_keys)+p_joint->scs_keys*sizeof(p_joint->p_scs[0]);
    ffwrite(&ch,sizeof(ch),1,f);
    ffwrite(&p_joint->scs_keys,sizeof(p_joint->scs_keys),1,f);
    ffwrite(p_joint->p_scs,sizeof(p_joint->p_scs[0]),p_joint->scs_keys,f);
  }

  if(p_joint->p_keys)
    lo_chunk_save_obj_joint_spline(f, p_joint->p_keys);

  lo_chunk_save_obj_joint(f, p_joint->p_next, p_joint->joint_ID);
  lo_chunk_save_obj_joint(f, p_joint->p_child, p_joint->joint_ID);  
  
  return(TRUE);
}

int lo_chunk_save_obj_joint_animaci(FILE *f, JOINT_ANIMACE *p_jani)
{
  OUT_CHUNK ch;
  
  ch.typ = CHUNK_OBJ_JA;
  ch.velikost = sizeof(ch)+strlen(p_jani->jmeno)+1;    
  ffwrite(&ch,sizeof(ch),1,f);
  ffwrite(p_jani->jmeno,sizeof(char),strlen(p_jani->jmeno)+1,f);
  
  ch.typ = CHUNK_OBJ_JA_FRAMU;
  ch.velikost = sizeof(ch)+sizeof(p_jani->framenum);
  ffwrite(&ch,sizeof(ch),1,f);
  ffwrite(&p_jani->framenum,sizeof(p_jani->framenum),1,f);  
  
  lo_chunk_save_obj_joint(f, p_jani->p_child, ROOT_ID);  
  
  return(TRUE);
}

void lo_kost_ocisluj_rec(JOINT *p_joint, int *p_cislo)
{
  if(p_joint->p_child)
    lo_kost_ocisluj_rec(p_joint->p_child,p_cislo);
  if(p_joint->p_next)
    lo_kost_ocisluj_rec(p_joint->p_next,p_cislo);
  
  p_joint->joint_ID = (*p_cislo)++;
}

int lo_chunk_save_obj_joint_animace(FILE *f, EDIT_OBJEKT *p_obj)
{
  JOINT_ANIMACE *p_akt = p_obj->p_joit_animace;
  int ID;

  while(p_akt) {
    ID = 1;
    lo_kost_ocisluj_rec(p_akt->p_child, &ID);
    lo_chunk_save_obj_joint_animaci(f, p_akt);
    p_akt = p_akt->p_next;
  }
  return(TRUE);
}

int lo_chunk_save_kont_mesh(FFILE f, EDIT_KONTEJNER *p_kont, EDIT_MATERIAL **p_mat, int max_mat, GLMATRIX *p_world)
{
  int  i,j;
  
  if(p_kont) {
    for(i = 0,j = 0; i < p_kont->max_objektu; i++) {
      if(!p_kont->p_obj[i]) 
        continue;
      j++;

      /* Zapis jmeno objektu
      */
      lo_chunk_save_obj_jmeno(f, p_kont->p_obj[i]);
      lo_chunk_save_obj_ID(f, p_kont->p_obj[i]);
      lo_chunk_save_obj_pivot(f, p_kont->p_obj[i]);
      lo_chunk_save_obj_obb(f, p_kont->p_obj[i]);

      /* Uloz jmeno materialu
      */
      assert(p_kont->p_obj[i]->material < max_mat);
      lo_chunk_save_obj_material(f,p_mat[p_kont->p_obj[i]->material]);
      

      /* Uloz jeho vertexy
      */
      lo_chunk_save_obj_vertex(f, p_kont->p_obj[i], p_world);

      /* Uloz jeho normaly
      */
      lo_chunk_save_obj_normal(f, p_kont->p_obj[i]);

      /* Uloz facelist
      */
      lo_chunk_save_obj_face(f, p_kont->p_obj[i]);
    
      /* Uloz jeho normaly
      */
      lo_chunk_save_obj_face_ext(f, p_kont->p_obj[i]);

      /* Uloz maplisty
      */
      lo_chunk_save_obj_mapy_1(f,p_kont->p_obj[i]);
      lo_chunk_save_obj_mapy_2(f,p_kont->p_obj[i]);
      lo_chunk_save_obj_mapy_3(f,p_kont->p_obj[i]);
      lo_chunk_save_obj_mapy_4(f,p_kont->p_obj[i]);

      /* Uloz rozsirene barvy objektu
      */
      lo_chunk_save_obj_barvy_ext(f,p_kont->p_obj[i]);      

      /* Lokalni matice
      */
      if(p_kont->kflag&KONT_KEYFRAME)
        lo_chunk_save_obj_matrix(f,&p_kont->p_obj[i]->m);

      /* Single - mesh animace      
      */
      /* TODO
      if(p_kont->p_obj[i]->p_joit_animace) {
        lo_chunk_save_obj_joint_animace(f,p_kont->p_obj[i]);
      }
      */
    }
    return(j);
  } else
    return(FALSE);
}

int lo_chunk_save_kontejner(FFILE f, EDIT_KONTEJNER *p_kont, EDIT_MATERIAL **p_mat, int max_mat, int low, int kont, int kont_id, int transf_stat)
{  
  int i;

  /* Oflagovani podle materialu
  */ 
  updatuj_kontejner_flag(p_kont, p_mat);
  
  /* Optimalizace jmen objektu
  */
  lo_kontejner_jedinecne_jmena(p_kont,FALSE);

  p_kont->kontejner_ID = kont_id;
  if(low) {
    lo_chunk_save_kont_low_jmeno(f,p_kont);
  } else {
    lo_chunk_save_kont_jmeno(f,p_kont,kont);
  }

  lo_chunk_save_kont_low_id(f, p_kont);
  lo_chunk_save_kont_id(f,p_kont->kontejner_ID);
  lo_chunk_save_kont_flag(f,p_kont);
  lo_chunk_save_kont_flag_ext(f,p_kont);
  lo_chunk_save_kont_flag_ext_2(f,p_kont);  
  lo_chunk_save_kont_obb(f, p_kont);
  
  /* Podle transformacniho flagu transf_stat
  */
  transf_stat = (p_kont->kflag&KONT_STATIC && transf_stat);
  if(!transf_stat) {
    lo_chunk_save_kont_wmatrix(f, p_kont, &p_kont->world);
    lo_chunk_save_kont_mpmatrix(f, p_kont, &p_kont->mpmatrix);
  } else {
    GLMATRIX m;
    init_matrix(&m);
    lo_chunk_save_kont_wmatrix(f,p_kont,&m);
    lo_chunk_save_kont_mpmatrix(f,p_kont,&m);
  }
  i = lo_chunk_save_kont_mesh(f,p_kont,p_mat,max_mat,
                              transf_stat ? kont_world_matrix(p_kont) : NULL);
  
  /* Uloz key-framy
  */
  if(p_kont->kflag&KONT_KEYFRAME) {
    for(i = 0; i < KONT_MAX_ANIM; i++) {
      if(p_kont->sim[i].keynum) {
        key_sim_root_vloz_pivoty_do_animace(p_kont, p_kont->sim+i);
        lo_chunk_save_sim(f, p_kont->sim+i);
      }
    }
  }
  
  /* Uloz svetla tohoto kontejneru
  */
  lo_chunk_save_static_light_mesh(f,p_kont->p_slight,p_mat);
  lo_chunk_save_dyn_light_mesh(f, p_kont->p_dlight);
  lo_chunk_save_extra_light_mesh(f, p_kont->p_edlight);

  /* Uloz mlho-kostky
  */
  lo_chunk_save_mlhokostku(f, p_kont->p_mlha, TRUE);

  return(i);
}

int lo_uloz_materialy_chunk_kont(FFILE f, EDIT_MATERIAL **p_mat, int max_mat, EDIT_KONTEJNER *p_kont, int kontrola)
{
 int i,j;

 /* Uloz pouzite materialy
 */
 for(i = 0; i < max_mat; i++) {
   if(p_mat[i]) {
     for(j = 0; j < p_kont->max_objektu; j++) {
       if(p_kont->p_obj[j] && p_kont->p_obj[j]->material == i) {
         if(!kontrola || !(p_mat[i]->flag&(MAT_POUZITY|MAT_SYSTEM)))
           lo_uloz_material_chunk(f, p_mat[i]);
         break;
       }
     }
   }
 }

 return(TRUE);
}

int lo_uloz_materialy_chunk_pouzite(FFILE f, EDIT_MATERIAL **p_mat, int max_mat)
{ 
 int i;

 /* Uloz pouzite materialy
 */
 for(i = 0; i < max_mat; i++) {
   if(p_mat[i] && p_mat[i]->flag&(MAT_POUZITY|MAT_SYSTEM)) {
     lo_uloz_material_chunk(f, p_mat[i]);
   }
 }
 return(TRUE);
}

/* Ulozi kontejner
*/
int lo_uloz_kontejner_chunk(FFILE f, EDIT_MATERIAL **p_mat, int max_mat, 
                            EDIT_KONTEJNER *p_kont, int low)
{ 
 /* Uloz materialy
 */
 lo_uloz_materialy_chunk_kont(f, p_mat, max_mat, p_kont, FALSE);
 
 /* Uloz kontejner
 */
 lo_chunk_save_kontejner(f, p_kont, p_mat, max_mat, low, K_CHYBA, K_CHYBA, FALSE);

 /* Uloz mlho-kostky
 */
 lo_chunk_save_mlhokostku(f, p_kont->p_mlha, TRUE);

 return(TRUE);
}


/* Ulozi vsechny veci do .prj
*/
int lo_uloz_projekt(EDIT_MATERIAL **p_mat, int max_mat, EDIT_KONTEJNER **p_kontlist, int kontnum, char *p_jmeno, STATIC_LIGHT *p_light, int lightnum, ZDRCADLO_DESC *p_zrcadlo, LENS_FLARE *p_flarr, int flaremax, dword ambient, MLZNA_KOSTKA *p_mlha, DYN_LIGHT *p_ddlight, int ddlightnum, EXTRA_DYN_LIGHT *p_eelight, int eelightnum, LEVEL_ENVIRONMENT *p_env, NASTAVENI_KAMER *p_kam, LEVEL_KONFIG *p_lc, int level, int transf_static)
{
 EDIT_KONTEJNER *p_kont;
 FFILE f;
 int   i,j,kID;

 /* Preved scenu na jedinecna jmena
 */
 //lo_kontlist_jedinecne_jmena(p_kontlist,kontnum);

 if((f = ffopen(p_jmeno,"wb")) == NULL) {
   return(FALSE);
 }
 
 /* Ulozim ambientni svetlo sceny
 */
 lo_chunk_save_ambient(f,ambient);

 /* Ulozeni konfigurace levelu - pokud je
 */
 if(p_lc) {
   lo_chunk_save_lc_ext(f, p_lc);
 }

 /* Ulozeni pouzitych materialu
 */
 lo_uloz_materialy_chunk_pouzite(f, p_mat, max_mat); 
 
 /* Uloz kontejnery
 */
 kID = K_CHYBA;
 for(j = 0; j < kontnum; j++) {
   if((p_kont = p_kontlist[j])) {
     
     if(level)
       kID++;
     
     lo_uloz_materialy_chunk_kont(f, p_mat, max_mat, p_kont,TRUE);
     lo_chunk_save_kontejner(f, p_kont, p_mat, max_mat, FALSE, K_CHYBA, kID, transf_static);
     
     p_kont = p_kont->p_next;
     while(p_kont) {
       lo_uloz_materialy_chunk_kont(f, p_mat, max_mat, p_kont,TRUE);
       lo_chunk_save_kontejner(f, p_kont, p_mat, max_mat, TRUE, K_CHYBA, kID, transf_static);
       p_kont = p_kont->p_next;
     }
   }
 } 

 /* Staticka svetla sceny
 */
 if(p_light) {
   for(i = 0; i < lightnum; i++) {
     if(p_light[i].akt && !(p_light[i].flag2&SL2_MESH))
       lo_chunk_save_static_light(f,p_light+i,p_mat);
   }
 }

 /* Uloz dynamicka svetla sceny
 */
 if(p_ddlight) {
   for(i = 0; i < ddlightnum; i++)
     if(p_ddlight[i].akt && !(p_ddlight[i].flag&SDL_MESH))
       lo_chunk_save_dyn_light(f, p_ddlight+i);
 }

 /* Extra a efektova svetla sceny
 */
 if(p_eelight) {
   for(i = 0; i < eelightnum; i++)
     if(p_eelight[i].akt && !(p_eelight[i].flag&EDL_MESH))
       lo_chunk_save_extra_light(f, p_eelight+i);
 }

 /* Zrcadlo
 */
 if(p_zrcadlo) {   
   lo_chunk_preved_zrcadlo(p_kontlist, kontnum, p_zrcadlo);
   lo_chunk_save_zrcadlo(f,p_zrcadlo);
 }

 /* Flare efekty + jejich materialy (pouze ze sceny)
 */ 
 for(i = 0; i < flaremax; i++) {
   if(p_flarr[i].akt && !p_flarr[i].p_svetlo) {
     lo_uloz_material_chunk(f, p_flarr[i].p_mat);
     lo_chunk_save_flare(f,p_flarr+i,p_mat,FALSE);
     if(p_flarr[i].p_sloz) {
       for(j = 0; j < SLOZ_FLARE; j++) {
         if(p_flarr[i].p_sloz[j].vzdal > 0.0f) 
           lo_uloz_material_chunk(f, p_flarr[i].p_sloz[j].p_mat);
       }
       lo_chunk_save_flare_slozeny(f,p_flarr+i,p_mat);
     }
   }     
 }
 
 /* Mlho-kostku
 */
 lo_chunk_save_mlhokostku(f, p_mlha, FALSE);

 /* Ulozeni environmentu
 */
 if(p_env) {
   lo_chunk_save_environment(f, p_env);
 }

 /* Ulozeni nastaveni kamer
 */
 if(p_kam) {
   lo_chunk_save_kamset(f, p_kam);
   lo_chunk_save_kamset_2(f, p_kam);
 }

 ffclose(f);
 return(j);
}



/************************************************************************
  Loading objektu/materialu pres chunk
  Globalni promenne pro loading
*/

/* Nabinduje se na p_cnf->p_mat
*/
static int                matlistnum;    // pocet nactenych materialu
static EDIT_MATERIAL    **p_matlist;     // aktualni material
static ANIM_MATERIAL     *p_fanim;        // aktualni frame animace

static int                textlistnum;   // list textur
static EDIT_TEXT         *p_textlist;    // list textur

static EDIT_KONTEJNER   **p_kontlist;   // seznam kontejneru
static int                kontlistnum;   // max pocet kontejneru

static int                kont_load;
static EDIT_KONTEJNER    *p_kont_old; // stary kontejner
static EDIT_KONTEJNER    *p_kont; // aktualni kontejner
static EDIT_MATERIAL     *p_mat;  // aktualni material
static EDIT_OBJEKT       *p_obj;  // aktualni objekt

static SIMPLE_TRACK_INFO *p_sim_node; // aktivni loadovaci node
static HIERARCHY_SIM     *p_sim;      // aktivni loadovaci root

static STATIC_LIGHT      *p_lightlist;
static STATIC_LIGHT      *p_lightakt; // aktualni staticke svetlo
static int                lightlistnum;

static STATIC_LIGHT      *p_light_mesh;


static ZDRCADLO_DESC     *p_zrcadlolist;
static int               *p_zrcadloload;

static LENS_FLARE        *p_flarelist;  // seznam flaru
static LENS_FLARE        *p_flare;      // posledni nahrany flare
static int                flarelistnum;

static MLZNA_KOSTKA      *p_top_mlha;

static dword             *p_ambient;

static DYN_LIGHT         *p_dlist;  // seznam dynamickych svetel
static int                dlistnum;

static EXTRA_DYN_LIGHT   *p_elist;  // seznam extra svetel
static int                elistnum;

static LEVEL_ENVIRONMENT *p_lenv;
static NASTAVENI_KAMER   *p_kamset;

static LEVEL_KONFIG      *p_lc;

static JOINT             *p_joint;
static JOINT_ANIMACE     *p_joint_animace;

/* **********************************************************************
   Loading environmentu
*/
int lo_chunk_load_environment(FFILE f, OUT_CHUNK *p_ch)
{     
  if(p_ch->typ == CHUNK_ENVIRONMENT) {
    if(p_lenv) {
      ffread(p_lenv,sizeof(p_lenv[0]),1,f);
    } else {
      LEVEL_ENVIRONMENT env;
      ffread(&env,sizeof(env),1,f);
    }
    return(TRUE);
  }
  else
    return(FALSE);
}

/* **********************************************************************
   Loading kamsetu
*/
int lo_chunk_load_kamset(FFILE f, OUT_CHUNK *p_ch)
{     
  if(p_kamset && p_ch->typ == CHUNK_KAMSET) {    
    ffread(&p_kamset->far_plane,sizeof(p_kamset->far_plane),1,f);
    ffread(&p_kamset->fov,sizeof(p_kamset->fov),1,f);
    ffread(&p_kamset->max_uhel,sizeof(p_kamset->max_uhel),1,f);
    ffread(&p_kamset->min_uhel,sizeof(p_kamset->min_uhel),1,f);
    ffread(&p_kamset->min_vzdal,sizeof(p_kamset->min_vzdal),1,f);
    ffread(&p_kamset->max_vzdal,sizeof(p_kamset->max_vzdal),1,f);
    return(TRUE);
  }
  else
    return(FALSE);
}

int lo_chunk_load_kamset_2(FFILE f, OUT_CHUNK *p_ch)
{     
  if(p_kamset && p_ch->typ == CHUNK_KAMSET_2) {
    ffread(&p_kamset->max,sizeof(p_kamset->max),1,f);
    ffread(&p_kamset->min,sizeof(p_kamset->min),1,f);
    return(TRUE);
  }
  else
    return(FALSE);
}

/* **********************************************************************
   Loading dynamickeho svetla
*/
int lo_chunk_load_dyn_light_anim(FFILE f, DYN_LIGHT_ANIM *p_an, int extra)
{  
  /*
  ffread(&p_an->pos_keys,sizeof(p_an->pos_keys),1,f);
  if(p_an->pos_keys) {
    p_an->p_pkeys = mmalloc(sizeof(p_an->p_pkeys[0])*p_an->pos_keys);
    ffread(p_an->p_pkeys,sizeof(p_an->p_pkeys[0]),p_an->pos_keys,f);    

    p_an->p_pos = mmalloc(sizeof(p_an->p_pos[0])*p_an->pos_keys);
    ffread(p_an->p_pos,sizeof(p_an->p_pos[0]),p_an->pos_keys,f);
  } else {
    p_an->p_pkeys = NULL;
    p_an->p_pos = NULL;
  }
    
  ffread(&p_an->trg_keys,sizeof(p_an->trg_keys),1,f);
  if(p_an->trg_keys) {
    p_an->p_tkeys = mmalloc(sizeof(p_an->p_tkeys[0])*p_an->trg_keys);
    ffread(p_an->p_tkeys,sizeof(p_an->p_tkeys[0]),p_an->trg_keys,f);

    p_an->p_trg = mmalloc(sizeof(p_an->p_trg[0])*p_an->trg_keys);
    ffread(p_an->p_trg,sizeof(p_an->p_trg[0]),p_an->trg_keys,f);
  } else {
    p_an->p_tkeys = NULL;
    p_an->p_trg = NULL;
  }
  
  ffread(&p_an->dos_keys,sizeof(p_an->dos_keys),1,f);
  if(p_an->dos_keys) {
    p_an->p_dskeys = mmalloc(sizeof(p_an->p_dskeys[0])*p_an->dos_keys);
    ffread(p_an->p_dskeys,sizeof(p_an->p_dskeys[0]),p_an->dos_keys,f);

    p_an->p_dosah = mmalloc(sizeof(p_an->p_dosah[0])*p_an->dos_keys);
    ffread(p_an->p_dosah,sizeof(p_an->p_dosah[0]),p_an->dos_keys,f);
  } else {
    p_an->p_dskeys = NULL;
    p_an->p_dosah = NULL;
  }
  
  if(extra) {    
    ffread(&p_an->alfa_keys,sizeof(p_an->alfa_keys),1,f);
    
    p_an->diff_keys = 0;
    p_an->p_dfkeys = NULL;
    p_an->p_diff = NULL;

    if(p_an->alfa_keys) {
      int i;

      p_an->p_akeys = mmalloc(sizeof(p_an->p_akeys[0])*p_an->alfa_keys);
      ffread(p_an->p_akeys,sizeof(p_an->p_akeys[0]),p_an->alfa_keys,f);
      
      p_an->p_diff = mmalloc(sizeof(p_an->p_diff[0])*p_an->alfa_keys);
      ffread(p_an->p_diff,sizeof(p_an->p_diff[0]),p_an->alfa_keys,f);

      p_an->p_alfa = mmalloc(sizeof(p_an->p_alfa[0])*p_an->alfa_keys);
      for(i = 0; i < p_an->alfa_keys; i++) {
        p_an->p_alfa[i] = p_an->p_diff[i].a;
      } 
      
      null_free(&p_an->p_diff);
    } else {
      p_an->p_akeys = NULL;
      p_an->p_alfa = NULL;
    }
  } else {
    ffread(&p_an->diff_keys,sizeof(p_an->diff_keys),1,f);
    
    p_an->alfa_keys = 0;
    p_an->p_akeys = NULL;
    p_an->p_alfa = NULL;    
    
    if(p_an->diff_keys) {
      p_an->p_dfkeys = mmalloc(sizeof(p_an->p_dfkeys[0])*p_an->diff_keys);
      ffread(p_an->p_dfkeys,sizeof(p_an->p_dfkeys[0]),p_an->diff_keys,f);
      
      p_an->p_diff = mmalloc(sizeof(p_an->p_diff[0])*p_an->diff_keys);
      ffread(p_an->p_diff,sizeof(p_an->p_diff[0]),p_an->diff_keys,f);
    } else {
      p_an->p_dfkeys = NULL;
      p_an->p_diff = NULL;
    }
  }
  
  ffread(&p_an->spec_keys,sizeof(p_an->spec_keys),1,f);
  if(p_an->spec_keys) {
    p_an->p_spkeys = mmalloc(sizeof(p_an->p_spkeys[0])*p_an->spec_keys);
    ffread(p_an->p_spkeys,sizeof(p_an->p_spkeys[0]),p_an->spec_keys,f);

    p_an->p_spec = mmalloc(sizeof(p_an->p_spec[0])*p_an->spec_keys);
    ffread(p_an->p_spec,sizeof(p_an->p_spec[0]),p_an->spec_keys,f);
  } else {
    p_an->p_spkeys = NULL;
    p_an->p_spec = NULL;
  }
  */
  return(TRUE);
}

int lo_chunk_load_dyn_light(FFILE f, OUT_CHUNK *p_ch)
{
  DYN_LIGHT *p_light;
  
  if(p_ch->typ == CHUNK_DYNAMIC_LIGHT && p_dlist) {
    p_light = lo_najdi_volne_dsvetlo_point(p_dlist,dlistnum);
    if(p_light) {
      ffread(&p_light->akt,sizeof(p_light->akt),1,f);
      str_read(p_light->jmeno,f);
      ffread(&p_light->np,sizeof(p_light->np),1,f);
      ffread(&p_light->dir,sizeof(p_light->dir),1,f);
      ffread(&p_light->look_max,sizeof(p_light->look_max),1,f);
      ffread(&p_light->dosah,sizeof(p_light->dosah),1,f);
      ffread(&p_light->min,sizeof(p_light->min),1,f);
      ffread(&p_light->max,sizeof(p_light->max),1,f);
      ffread(&p_light->theta,sizeof(p_light->theta),1,f);
      ffread(&p_light->dr,sizeof(p_light->dr),1,f);
      ffread(&p_light->dg,sizeof(p_light->dg),1,f);
      ffread(&p_light->db,sizeof(p_light->db),1,f);
      ffread(&p_light->sr,sizeof(p_light->sr),1,f);
      ffread(&p_light->sg,sizeof(p_light->sg),1,f);
      ffread(&p_light->sb,sizeof(p_light->sb),1,f);
      ffread(&p_light->flag,sizeof(p_light->flag),1,f);
      ffread(&p_light->ktrida,sizeof(p_light->ktrida),1,f);
      ffread(&p_light->an_flag,sizeof(p_light->an_flag),1,f);
      ffread(&p_light->an.endtime,sizeof(p_light->an.endtime),1,f);
      p_light->tp = p_light->np;
      if(p_light->an.endtime) {
        lo_chunk_load_dyn_light_anim(f, &p_light->an,FALSE);
      }
      return(TRUE);
    } else
      return(FALSE);
  } else
    return(FALSE);
}

int lo_chunk_load_dyn_light_mesh(FFILE f, OUT_CHUNK *p_ch)
{
  DYN_LIGHT *p_light;
  /*
  if(p_kont && p_ch->typ == CHUNK_DYNAMIC_LIGHT_MESH) {
    p_light = mmalloc(sizeof(p_light[0]));
    ffread(&p_light->akt,sizeof(p_light->akt),1,f);
    str_read(p_light->jmeno,f);
    ffread(&p_light->np,sizeof(p_light->np),1,f);
    ffread(&p_light->dir,sizeof(p_light->dir),1,f);
    ffread(&p_light->look_max,sizeof(p_light->look_max),1,f);
    ffread(&p_light->dosah,sizeof(p_light->dosah),1,f);
    ffread(&p_light->min,sizeof(p_light->min),1,f);
    ffread(&p_light->max,sizeof(p_light->max),1,f);
    ffread(&p_light->theta,sizeof(p_light->theta),1,f);
    ffread(&p_light->dr,sizeof(p_light->dr),1,f);
    ffread(&p_light->dg,sizeof(p_light->dg),1,f);
    ffread(&p_light->db,sizeof(p_light->db),1,f);
    ffread(&p_light->sr,sizeof(p_light->sr),1,f);
    ffread(&p_light->sg,sizeof(p_light->sg),1,f);
    ffread(&p_light->sb,sizeof(p_light->sb),1,f);
    ffread(&p_light->flag,sizeof(p_light->flag),1,f);
    ffread(&p_light->ktrida,sizeof(p_light->ktrida),1,f);
    ffread(&p_light->an_flag,sizeof(p_light->an_flag),1,f);
    ffread(&p_light->an.endtime,sizeof(p_light->an.endtime),1,f);
    
    p_light->tp = p_light->np;
    
    if(p_light->an.endtime) {
      lo_chunk_load_dyn_light_anim(f, &p_light->an,FALSE);
    }

    p_light->p_mesh_data = p_kont;
    p_light->p_next = p_kont->p_dlight;
    p_light->p_prev = NULL;
    p_kont->p_dlight = p_light;
    if(p_light->p_next) {
      p_light->p_next->p_prev = p_light;
    }

    return(TRUE);
  } else
    return(FALSE);
*/
}

/* **************************************************************
     Nahraje Extra svetla
   **************************************************************
*/
int lo_chunk_load_extra_light(FFILE f, OUT_CHUNK *p_ch)
{
  EXTRA_DYN_LIGHT *p_light;
  
  if(p_ch->typ == CHUNK_EXTRA_LIGHT && p_elist) {
    p_light = lo_najdi_volne_extra_svetlo_point(p_elist,elistnum);
    if(p_light) {
      ffread(&p_light->akt,sizeof(p_light->akt),1,f);
      str_read(p_light->jmeno,f);
      ffread(&p_light->flag,sizeof(p_light->flag),1,f);
      ffread(&p_light->mflag,sizeof(p_light->mflag),1,f);
      ffread(&p_light->ktrida,sizeof(p_light->ktrida),1,f);
      ffread(&p_light->np,sizeof(p_light->np),1,f);
      ffread(&p_light->dir,sizeof(p_light->dir),1,f);
      ffread(&p_light->dosah,sizeof(p_light->dosah),1,f);
      ffread(&p_light->theta,sizeof(p_light->theta),1,f);
      ffread(&p_light->utlum,sizeof(p_light->utlum),1,f);
      ffread(&p_light->gourand,sizeof(p_light->gourand),1,f);
      ffread(&p_light->da,sizeof(p_light->da),1,f);
      ffread(&p_light->da,sizeof(p_light->da),1,f);
      ffread(&p_light->da,sizeof(p_light->da),1,f);
      ffread(&p_light->da,sizeof(p_light->da),1,f);
      ffread(&p_light->sr,sizeof(p_light->sr),1,f);
      ffread(&p_light->sg,sizeof(p_light->sg),1,f);
      ffread(&p_light->sb,sizeof(p_light->sb),1,f);
      ffread(&p_light->an_flag,sizeof(p_light->an_flag),1,f);
      ffread(&p_light->an.endtime,sizeof(p_light->an.endtime),1,f);
      p_light->tp = p_light->np;
      if(p_light->an.endtime) {
        lo_chunk_load_dyn_light_anim(f, &p_light->an,TRUE);
      }
      return(TRUE);
    } else
      return(FALSE);
  } else
    return(FALSE);
}

int lo_chunk_load_extra_light_mesh(FFILE f, OUT_CHUNK *p_ch)
{
  EXTRA_DYN_LIGHT *p_light;
/*  
  if(p_kont && p_ch->typ == CHUNK_EXTRA_LIGHT_MESH) {
    p_light = mmalloc(sizeof(p_light[0]));
    ffread(&p_light->akt,sizeof(p_light->akt),1,f);
    str_read(p_light->jmeno,f);
    ffread(&p_light->flag,sizeof(p_light->flag),1,f);
    ffread(&p_light->mflag,sizeof(p_light->mflag),1,f);
    ffread(&p_light->ktrida,sizeof(p_light->ktrida),1,f);
    ffread(&p_light->np,sizeof(p_light->np),1,f);
    ffread(&p_light->dir,sizeof(p_light->dir),1,f);
    ffread(&p_light->dosah,sizeof(p_light->dosah),1,f);
    ffread(&p_light->theta,sizeof(p_light->theta),1,f);
    ffread(&p_light->utlum,sizeof(p_light->utlum),1,f);
    ffread(&p_light->gourand,sizeof(p_light->gourand),1,f);
    ffread(&p_light->da,sizeof(p_light->da),1,f);
    ffread(&p_light->da,sizeof(p_light->da),1,f);
    ffread(&p_light->da,sizeof(p_light->da),1,f);
    ffread(&p_light->da,sizeof(p_light->da),1,f);
    ffread(&p_light->sr,sizeof(p_light->sr),1,f);
    ffread(&p_light->sg,sizeof(p_light->sg),1,f);
    ffread(&p_light->sb,sizeof(p_light->sb),1,f);
    ffread(&p_light->an_flag,sizeof(p_light->an_flag),1,f);
    ffread(&p_light->an.endtime,sizeof(p_light->an.endtime),1,f);   

    p_light->tp = p_light->np;
    
    if(p_light->an.endtime) {
      lo_chunk_load_dyn_light_anim(f, &p_light->an,TRUE);
    }

    p_light->p_mesh_data = p_kont;
    p_light->p_next = p_kont->p_edlight;
    p_light->p_prev = NULL;
    p_kont->p_edlight = p_light;
    if(p_light->p_next) {
      p_light->p_next->p_prev = p_light;
    }
    return(TRUE);
  } else
    return(FALSE);
    */
}

/* ***********************************************************************
   Loading mlho-kostek
*/
int lo_chunk_load_mlho_kostek(FFILE f, OUT_CHUNK *p_ch)
{   
  MLZNA_KOSTKA    *p_top;

  if(p_ch->typ == CHUNK_MLHOKOSTKA) {
    p_top = vyrob_mlhokostku("");
    str_read(p_top->jmeno,f);
    ffread(&p_top->min,sizeof(BOD),1,f);
    ffread(&p_top->max,sizeof(BOD),1,f);
    ffread(&p_top->flag,sizeof(p_top->flag),1,f);
    ffread(&p_top->mod,sizeof(p_top->mod),1,f);
    ffread(&p_top->priorita,sizeof(p_top->priorita),1,f);
    ffread(&p_top->start,sizeof(p_top->start),1,f);
    ffread(&p_top->stop,sizeof(p_top->stop),1,f);
    ffread(&p_top->intenzita,sizeof(p_top->intenzita),1,f);
    ffread(&p_top->r,sizeof(float)*4,1,f);
    p_top->p_next = p_top_mlha;
    p_top_mlha = p_top;
    return(TRUE);
  }
  else
    return(FALSE);
}

int lo_chunk_load_mlho_kostek_2(FFILE f, OUT_CHUNK *p_ch)
{   
  MLZNA_KOSTKA    *p_top;
  int              kid;

  if(p_ch->typ == CHUNK_MLHOKOSTKA_2) {
    p_top = vyrob_mlhokostku("");
    str_read(p_top->jmeno,f);
    ffread(&p_top->min,sizeof(BOD),1,f);
    ffread(&p_top->max,sizeof(BOD),1,f);
    ffread(&p_top->flag,sizeof(p_top->flag),1,f);
    ffread(&p_top->mod,sizeof(p_top->mod),1,f);
    ffread(&p_top->priorita,sizeof(p_top->priorita),1,f);
    ffread(&p_top->start,sizeof(p_top->start),1,f);
    ffread(&p_top->stop,sizeof(p_top->stop),1,f);
    ffread(&p_top->intenzita,sizeof(p_top->intenzita),1,f);
    ffread(&p_top->r,sizeof(float)*4,1,f);
    
    ffread(&kid,sizeof(int),1,f);
    if(kid != K_CHYBA) {
      p_kont->p_mlha = p_top;
      p_top->p_kont = p_kont;
    }
    
    ffread(&p_top->poly,sizeof(p_top->poly),1,f);

    p_top->p_next = p_top_mlha;
    p_top_mlha = p_top;

    return(TRUE);
  }
  else
    return(FALSE);
}

/* ***********************************************************************
   Loading flaru
*/
int lo_chunk_load_flare(FFILE f, OUT_CHUNK *p_ch)
{
  char pom[200];
  int  i,nic;

  if(p_flarelist && p_ch->typ == CHUNK_FLARE) {

    // najiti flare
    if((i = lo_najdi_volny_flare(p_flarelist, flarelistnum)) == K_CHYBA) {
      kprintf(1,"Neni volny flare !");
      p_flare = NULL;
      assert(0);
      return(FALSE);
    }
    p_flare = p_flarelist+i;
    memset(p_flare,0,sizeof(p_flare[0]));
    ffread(&p_flare->p,sizeof(BOD),1,f);
    ffread(&p_flare->r,sizeof(float),4,f);
    ffread(&p_flare->dx,sizeof(float),2,f);
    ffread(&nic,sizeof(int),1,f);
    ffread(&p_flare->alfa,sizeof(int),1,f);
    
    p_flare->index = i;
        
    p_flare->p_next = p_flare->p_prev = NULL;
    p_flare->p_bod = NULL;    
    p_flare->akt = TRUE;

    // load materialu
    str_read(pom,f);
    if((p_flare->material = lo_najdi_material(p_matlist,matlistnum,pom)) == K_CHYBA) {
      kprintf(1,"Cannot find material %s",pom);
      p_flare->p_mat = NULL;
      assert(0);
    } else
      p_flare->p_mat = (p_flare->material < matlistnum) ? p_matlist[p_flare->material] : NULL;    
    return(TRUE);
  }
  else
    return(FALSE);
}

int lo_chunk_load_flare_ext(FFILE f, OUT_CHUNK *p_ch)
{   
  char pom[200];
  int  i,nic;

  if(p_flarelist && p_ch->typ == CHUNK_FLARE_EXT) {

    // najiti flare
    if((i = lo_najdi_volny_flare(p_flarelist, flarelistnum)) == K_CHYBA) {
      kprintf(1,"Neni volny flare !");
      p_flare = NULL;
      assert(0);
      return(FALSE);
    }
    p_flare = p_flarelist+i;
    memset(p_flare,0,sizeof(p_flare[0]));
    ffread(&p_flare->p,sizeof(BOD),1,f);
    ffread(&p_flare->r,sizeof(float),4,f);
    ffread(&p_flare->dx,sizeof(float),2,f);
    ffread(&nic,sizeof(int),1,f);
    ffread(&p_flare->alfa,sizeof(int),1,f);
    ffread(&p_flare->zflag,sizeof(int),1,f);
    
    p_flare->index = i;
    
    p_flare->p_next = p_flare->p_prev = NULL;
    p_flare->p_bod = NULL;
    p_flare->akt = TRUE;

    // load materialu
    str_read(pom,f);
    if((p_flare->material = lo_najdi_material(p_matlist,matlistnum,pom)) == K_CHYBA) {
      kprintf(1,"Cannot find material %s",pom);
      p_flare->p_mat = NULL;
      assert(0);
    } else
      p_flare->p_mat = (p_flare->material < matlistnum) ? p_matlist[p_flare->material] : NULL;    
    return(TRUE);
  }
  else
    return(FALSE);
}

int lo_chunk_load_flare_light(FFILE f, OUT_CHUNK *p_ch)
{   
  char pom[200];
  int  i,nic;
 
  if(p_flarelist && p_lightakt && p_ch->typ == CHUNK_FLARE_LIGHT) {
    if((i = lo_najdi_volny_flare(p_flarelist, flarelistnum)) == K_CHYBA) {
      kprintf(1,"Neni volny flare !");
      p_flare = NULL;
      assert(0);
      return(FALSE);
    }
    p_flare = p_flarelist+i;

    ffread(&p_flare->p,sizeof(BOD),1,f);
    ffread(&p_flare->r,sizeof(float),4,f);
    ffread(&p_flare->dx,sizeof(float),2,f);
    ffread(&nic,sizeof(int),1,f);
    ffread(&p_flare->alfa,sizeof(int),1,f);
    
    p_flare->index = i;
    
    p_flare->p_next = p_flare->p_prev = NULL;
    p_flare->p_bod = NULL;    
    p_flare->akt = TRUE;

    p_flare->p_svetlo = p_lightakt;
    p_lightakt->p_flare = p_flare;

    // load materialu
    str_read(pom,f);
    if((p_flare->material = lo_najdi_material(p_matlist,matlistnum,pom)) == K_CHYBA) {
      kprintf(1,"Cannot find material %s",pom);
      p_flare->p_mat = NULL;
      assert(0);
    } else
      p_flare->p_mat = (p_flare->material < matlistnum) ? p_matlist[p_flare->material] : NULL;    
    return(TRUE);
  }
  else
    return(FALSE);
}

int lo_chunk_load_flare_light_ext(FFILE f, OUT_CHUNK *p_ch)
{   
  char pom[200];
  int  i,nic;
 
  if(p_flarelist && p_lightakt && p_ch->typ == CHUNK_FLARE_LIGHT_EXT) {
    if((i = lo_najdi_volny_flare(p_flarelist, flarelistnum)) == K_CHYBA) {
      kprintf(1,"Neni volny flare !");
      p_flare = NULL;
      assert(0);
      return(FALSE);
    }
    p_flare = p_flarelist+i;

    ffread(&p_flare->p,sizeof(BOD),1,f);
    ffread(&p_flare->r,sizeof(float),4,f);
    ffread(&p_flare->dx,sizeof(float),2,f);
    ffread(&nic,sizeof(int),1,f);
    ffread(&p_flare->alfa,sizeof(int),1,f);
    ffread(&p_flare->zflag,sizeof(int),1,f);
 
    p_flare->index = i;
    
    p_flare->p_next = p_flare->p_prev = NULL;
    p_flare->p_bod = NULL;    
    p_flare->akt = TRUE;

    p_flare->p_svetlo = p_lightakt;
    p_lightakt->p_flare = p_flare;

    // load materialu
    str_read(pom,f);
    if((p_flare->material = lo_najdi_material(p_matlist,matlistnum,pom)) == K_CHYBA) {
      kprintf(1,"Cannot find material %s",pom);
      p_flare->p_mat = NULL;
      assert(0);
    } else
      p_flare->p_mat = (p_flare->material < matlistnum) ? p_matlist[p_flare->material] : NULL;    
    return(TRUE);
  }
  else
    return(FALSE);
}

int lo_chunk_load_flare_mesh(FFILE f, OUT_CHUNK *p_ch)
{   
  char pom[200];
  int  nic;

  if(p_light_mesh && p_ch->typ == CHUNK_FLARE_MESH) {

    // vyroba flare
    p_flare = (LENS_FLARE *)mmalloc(sizeof(p_flare[0]));

    ffread(&p_flare->p,sizeof(BOD),1,f);
    ffread(&p_flare->r,sizeof(float),4,f);
    ffread(&p_flare->dx,sizeof(float),2,f);    
    ffread(&nic,sizeof(int),1,f);
    ffread(&p_flare->alfa,sizeof(int),1,f);
           
    p_flare->akt = TRUE;
    p_flare->p_svetlo = p_light_mesh;

    p_flare->p_next = (LENS_FLARE *)p_light_mesh->p_flare;
    p_light_mesh->p_flare = p_flare;

    // load materialu
    str_read(pom,f);
    if((p_flare->material = lo_najdi_material(p_matlist,matlistnum,pom)) == K_CHYBA) {      
      kprintf(1,"Cannot find material %s",pom);
      p_flare->p_mat = NULL;
      assert(0);
    } else
      p_flare->p_mat = (p_flare->material < matlistnum) ? p_matlist[p_flare->material] : NULL;    
    return(TRUE);
  }
  else
    return(FALSE);
}

int lo_chunk_load_flare_mesh_ext(FFILE f, OUT_CHUNK *p_ch)
{   
  char pom[200];
  int  nic;

  if(p_light_mesh && p_ch->typ == CHUNK_FLARE_MESH_EXT) {

    // vyroba flare
    p_flare = (LENS_FLARE *)mmalloc(sizeof(p_flare[0]));

    ffread(&p_flare->p,sizeof(BOD),1,f);
    ffread(&p_flare->r,sizeof(float),4,f);
    ffread(&p_flare->dx,sizeof(float),2,f);    
    ffread(&nic,sizeof(int),1,f);
    ffread(&p_flare->alfa,sizeof(int),1,f);
    ffread(&p_flare->zflag,sizeof(int),1,f);
           
    p_flare->akt = TRUE;
    p_flare->p_svetlo = p_light_mesh;

    p_flare->p_next = (LENS_FLARE *)p_light_mesh->p_flare;
    p_light_mesh->p_flare = p_flare;

    // load materialu
    str_read(pom,f);
    if((p_flare->material = lo_najdi_material(p_matlist,matlistnum,pom)) == K_CHYBA) {      
      kprintf(1,"Cannot find material %s",pom);
      p_flare->p_mat = NULL;
      assert(0);
    } else
      p_flare->p_mat = (p_flare->material < matlistnum) ? p_matlist[p_flare->material] : NULL;    
    return(TRUE);
  }
  else
    return(FALSE);
}

// Ulozeni slozeneho flare
int lo_chunk_load_flare_slozeny(FFILE f, OUT_CHUNK *p_ch)
{
  char      pom[200];
  int       s;

  if(p_flare && p_ch->typ == CHUNK_FLARE_SLOZENY) {
    p_flare->p_sloz = (LENS_FLARE_SLOZ *)mmalloc(sizeof(p_flare->p_sloz[0])*SLOZ_FLARE);

    for(s = 0; s < SLOZ_FLARE; s++) {
      ffread(&p_flare->p_sloz[s].vzdal,sizeof(float),1,f);
      ffread(&p_flare->p_sloz[s].r,sizeof(float),4,f);
      ffread(&p_flare->p_sloz[s].dx,sizeof(float),2,f);
      str_read(pom,f);
      if(p_flare->p_sloz[s].vzdal > 0.0f) {
        if((p_flare->p_sloz[s].material = lo_najdi_material(p_matlist,matlistnum,pom)) == K_CHYBA) {
          kprintf(1,"Cannot find material %s",pom);
          assert(0);
        }
        p_flare->p_sloz[s].p_mat = (p_flare->p_sloz[s].material < matlistnum) ? p_matlist[p_flare->p_sloz[s].material] : NULL;
      } else {
        p_flare->p_sloz[s].p_mat = NULL;
        p_flare->p_sloz[s].material = 0;
      }
    }
    return(TRUE);
  } else
    return(FALSE);
}

/* ***********************************************************************
   Loading ambientu
*/
int lo_chunk_load_ambient(FFILE f, OUT_CHUNK *p_ch)
{ 
  if(p_ambient && p_ch->typ == CHUNK_AMBIENT) {    
    ffread(p_ambient,sizeof(*p_ambient),1,f);
    return(TRUE);
  } else
    return(FALSE);
}

/* ***********************************************************************
   Loading ambientu
*/
int lo_chunk_load_levelconfig(FFILE f, OUT_CHUNK *p_ch)
{ 
  if(p_ch->typ == CHUNK_LEVEL_KONFIG_EXT) {    
    ffread(&p_lc->barva_pozadi,sizeof(p_lc->barva_pozadi),1,f);
    ffread(&p_lc->barva_pozadi_aktivni,sizeof(p_lc->barva_pozadi_aktivni),1,f);
    ffread(&p_lc->zm.aktivni,sizeof(p_lc->zm.aktivni),1,f);
    ffread(&p_lc->zm.v_start,sizeof(p_lc->zm.v_start),1,f);
    ffread(&p_lc->zm.v_stop,sizeof(p_lc->zm.v_stop),1,f);
    ffread(&p_lc->zm.h_start,sizeof(p_lc->zm.h_start),1,f);
    ffread(&p_lc->zm.h_stop,sizeof(p_lc->zm.h_stop),1,f);
    ffread(&p_lc->zm.barva,sizeof(p_lc->zm.barva),1,f);
    return(TRUE);
  } else
    return(FALSE);
}

/* ***********************************************************************
   Loading zrcadla
*/
/*
int lo_chunk_load_zrcadlo(FFILE f, OUT_CHUNK *p_ch)
{     
  if(p_zrcadlolist && p_ch->typ == CHUNK_ZRCADLO) {
    *p_zrcadloload = TRUE;
    ffread(&p_zrcadlolist->zrcadlo_k,sizeof(p_zrcadlolist->zrcadlo_k),1,f);
    ffread(&p_zrcadlolist->zrcadlo_o,sizeof(p_zrcadlolist->zrcadlo_o),1,f);
    ffread(&p_zrcadlolist->poly,sizeof(p_zrcadlolist->poly),1,f);
    ffread(&p_zrcadlolist->ref,sizeof(p_zrcadlolist->ref),1,f);
    ffread(p_zrcadlolist->p,sizeof(p_zrcadlolist->p[0]),4,f);
    ffread(p_zrcadlolist->r,sizeof(p_zrcadlolist->r[0]),5,f);    
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_zrcadlo_2(FFILE f, OUT_CHUNK *p_ch)
{     
  if(p_zrcadlolist && p_ch->typ == CHUNK_ZRCADLO_2) {
    *p_zrcadloload = TRUE;
    ffread(p_zrcadlolist,sizeof(ZDRCADLO_DESC),1,f);
    return(TRUE);
  } else
    return(FALSE);
}
*/

int lo_chunk_load_zrcadlo_3_sub(FFILE f, OUT_CHUNK *p_ch)
{   
  ZDRCADLO_DESC_POLY *p_poly;

  if(p_zrcadlolist && p_ch->typ == CHUNK_ZRCADLO_3_POLOZKA) {
    p_poly = (ZDRCADLO_DESC_POLY *)mmalloc(sizeof(p_poly[0]));
    ffread(p_poly,sizeof(p_poly[0]),1,f);
    p_poly->p_next = p_zrcadlolist->p_poly;
    p_zrcadlolist->p_poly = p_poly;
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_zrcadlo_3(FFILE f, OUT_CHUNK *p_ch)
{     
  if(p_zrcadlolist && p_ch->typ == CHUNK_ZRCADLO_3) {
    *p_zrcadloload = TRUE;
    
    ffread(p_zrcadlolist,sizeof(ZDRCADLO_DESC),1,f);    
    p_zrcadlolist->p_poly = NULL;

    return(TRUE);
  } else
    return(FALSE);
}

/* ***********************************************************************
   Loading statickych svetel
*/

int lo_chunk_load_static_light(FFILE f, OUT_CHUNK *p_ch)
{  
  STATIC_LIGHT light;
  if(p_lightlist && p_ch->typ == CHUNK_STATIC_LIGHT) {
    
    memset(&light,0,sizeof(light));    

    ffread(&light.akt,sizeof(light.akt),1,f);    
    str_read(light.jmeno,f);
    ffread(&light.index,sizeof(light.index),1,f);
    
    ffread(&light.p,sizeof(light.p),1,f);
    ffread(&light.r,sizeof(float),1,f);
    ffread(&light.g,sizeof(float),1,f);
    ffread(&light.b,sizeof(float),1,f);
    ffread(&light.k1,sizeof(float),4,f);
    ffread(&light.uhel,sizeof(light.uhel),1,f);
    ffread(&light.utlum,sizeof(light.utlum),1,f);
    ffread(&light.flag,sizeof(light.flag),1,f);
    ffread(&light.flag2,sizeof(light.flag2),1,f);

    if(p_lightlist[light.index].akt) {
      light.index = lo_najdi_volne_stat_svetlo(p_lightlist,lightlistnum);
      if(light.index != K_CHYBA) {
        p_lightlist[light.index] = light;
      }
    } else
      p_lightlist[light.index] = light;
    
    p_lightakt = p_lightlist+light.index;
          
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_static_light_mesh(FFILE f, OUT_CHUNK *p_ch)
{    
  STATIC_LIGHT *p_light;

  if(p_kont && p_ch->typ == CHUNK_STATIC_LIGHT_MESH) {
    p_light = (STATIC_LIGHT *)mmalloc(sizeof(p_light[0]));
    
    ffread(&p_light->akt,sizeof(p_light->akt),1,f);
    str_read(p_light->jmeno,f);
    ffread(&p_light->index,sizeof(p_light->index),1,f);
    
    ffread(&p_light->p,sizeof(p_light->p),1,f);
    ffread(&p_light->r,sizeof(float),1,f);
    ffread(&p_light->g,sizeof(float),1,f);
    ffread(&p_light->b,sizeof(float),1,f);
    ffread(&p_light->k1,sizeof(float),4,f);
    ffread(&p_light->uhel,sizeof(p_light->uhel),1,f);
    ffread(&p_light->utlum,sizeof(p_light->utlum),1,f);
    ffread(&p_light->flag,sizeof(p_light->flag),1,f);
    ffread(&p_light->flag2,sizeof(p_light->flag2),1,f);
    p_light->flag2 |= SL2_MESH;

    p_light->p_mesh_data = p_kont;
    p_light->p_next = p_kont->p_slight;
    p_light->p_prev = NULL;
    p_kont->p_slight = p_light;
    if(p_light->p_next) {
      p_light->p_next->p_prev = p_light;
    }

    p_light_mesh = p_light;

    return(TRUE);
  } else
    return(FALSE);
}

/************************************************************************
  Loading materialu
*/
int lo_chunk_load_mat(FFILE f, OUT_CHUNK *p_ch)
{  
  char pom[200];
  int m;

  /* Vybere novy material + zvoli jej jako aktivni
  */
  if(p_ch->typ == CHUNK_MATERIAL) {        
    str_read(pom,f);
    if(lo_najdi_material(p_matlist, matlistnum, pom) != K_CHYBA)
      p_mat = NULL;
    else {      
      if((m = lo_najdi_prazdny_material(p_matlist, matlistnum)) == K_CHYBA)
        chyba("Malo materialu!");      
      p_mat = p_matlist[m] = vyrob_material();
      p_mat->material_handle = m;
      strcpy(p_mat->jmeno,pom);
    }        
    return(TRUE);
  } else
    return(FALSE);  
}

int lo_chunk_load_mat_flags(FFILE f, OUT_CHUNK *p_ch)
{      
  if(p_mat && p_ch->typ == CHUNK_FLAGS) {
    ffread(&p_mat->flag,sizeof(p_mat->flag),1,f);
    ffread(&p_mat->flag2,sizeof(p_mat->flag2),1,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_mat_smat_pos(FFILE f, OUT_CHUNK *p_ch)
{      
  if(p_mat && p_ch->typ == CHUNK_MAT_SMAT_POS) {
    ffread(&p_mat->smaterial_pozice,sizeof(p_mat->smaterial_pozice),1,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_color(FFILE f, OUT_CHUNK *p_ch)
{
  MATERIAL *p_dxmat;
  char      string[9];
  
  if(p_mat && p_ch->typ == CHUNK_COLOR) {
    ffread(string,sizeof(char),9,f);
    p_dxmat = &p_mat->dxmat;
    
    p_dxmat->ambient_r = (float)string[0]/MAX_BYTE;
    p_dxmat->ambient_g = (float)string[1]/MAX_BYTE;
    p_dxmat->ambient_b = (float)string[2]/MAX_BYTE;
    
    p_dxmat->diffuse_r = (float)string[3]/MAX_BYTE;
    p_dxmat->diffuse_g = (float)string[4]/MAX_BYTE;
    p_dxmat->diffuse_b = (float)string[5]/MAX_BYTE;
    p_dxmat->diffuse_a = 1.0f;
    
    p_dxmat->specular_r = (float)string[6]/MAX_BYTE;
    p_dxmat->specular_g = (float)string[7]/MAX_BYTE;
    p_dxmat->specular_b = (float)string[8]/MAX_BYTE;
    
    p_dxmat->faktor_r = 1.0f;
    p_dxmat->faktor_g = 1.0f;
    p_dxmat->faktor_b = 1.0f;
    p_dxmat->faktor_a = 1.0f;

    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_color_ext(FFILE f, OUT_CHUNK *p_ch)
{
  MATERIAL *p_dxmat;
  char      string[16];
  
  if(p_mat && p_ch->typ == CHUNK_COLOR_EXT) {
    ffread(string,sizeof(char),16,f);
    p_dxmat = &p_mat->dxmat;
    
    p_dxmat->ambient_r = (float)string[0]/MAX_BYTE;
    p_dxmat->ambient_g = (float)string[1]/MAX_BYTE;
    p_dxmat->ambient_b = (float)string[2]/MAX_BYTE;
    //p_dxmat->ambient_a = (float)string[3]/MAX_BYTE;
    p_dxmat->diffuse_a = 1.0f;
    
    p_dxmat->diffuse_r = (float)string[4]/MAX_BYTE;
    p_dxmat->diffuse_g = (float)string[5]/MAX_BYTE;
    p_dxmat->diffuse_b = (float)string[6]/MAX_BYTE;
    p_dxmat->diffuse_a = (float)string[7]/MAX_BYTE;
    
    p_dxmat->specular_r = (float)string[8]/MAX_BYTE;
    p_dxmat->specular_g = (float)string[9]/MAX_BYTE;
    p_dxmat->specular_b = (float)string[10]/MAX_BYTE;
    //p_dxmat->specular_a = (float)string[11]/MAX_BYTE;    
    
    p_dxmat->faktor_r = (float)string[12]/MAX_BYTE;
    p_dxmat->faktor_g = (float)string[13]/MAX_BYTE;
    p_dxmat->faktor_b = (float)string[14]/MAX_BYTE;
    p_dxmat->faktor_a = (float)string[15]/MAX_BYTE;

    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_matanim_frame(FFILE f, OUT_CHUNK *p_ch)
{
  ANIM_FRAME *p_fram;
  int frame,t;
  int flag = 0;

  if(p_mat && p_fanim && p_ch->typ == CHUNK_MATANIM_FRAME) {
    ffread(&frame,sizeof(frame),1,f);
    assert(frame < p_fanim->framenum);
    p_fram = p_fanim->p_frame+frame;    
    str_read(p_fram->file,f);
    ffread(&p_fram->time,sizeof(p_fram->time),1,f);
    ffread(&p_fram->alfa,sizeof(p_fram->alfa),1,f);
    ffread(&p_fram->u,sizeof(p_fram->u),1,f);
    ffread(&p_fram->v,sizeof(p_fram->v),1,f);
    ffread(&p_fram->flag,sizeof(p_fram->flag),1,f);
    /* TODO
    if((t = lo_najdi_texturu(p_textlist,textlistnum,p_fram->file,flag))==K_CHYBA) {
      if((t = lo_najdi_volnou_texturu(p_textlist,textlistnum)) == K_CHYBA)
        chyba("Neni volna textura !");      
      strcpy(p_textlist[t].jmeno, p_fram->file);
      p_textlist[t].flag = flag;
    }
    */
    p_fram->p_text = p_textlist+t;

    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_matanim_frame_ext(FFILE f, OUT_CHUNK *p_ch)
{
  ANIM_FRAME *p_fram;
  int frame,t;
  int flag = 0;

  if(p_mat && p_fanim && p_ch->typ == CHUNK_MATANIM_EFRAME) {
    ffread(&frame,sizeof(frame),1,f);
    assert(frame < p_fanim->framenum);
    p_fram = p_fanim->p_frame+frame;
    ffread(&p_fram->time,sizeof(p_fram->time),1,f);
    ffread(&p_fram->alfa,sizeof(p_fram->alfa),1,f);
    ffread(&p_fram->u,sizeof(p_fram->u),1,f);
    ffread(&p_fram->v,sizeof(p_fram->v),1,f);
    ffread(&p_fram->flag,sizeof(p_fram->flag),1,f);

    // Load jmeno a flag textury
    str_read(p_fram->file,f);
    ffread(&flag,sizeof(flag),1,f);
    /* TODO
    if((t = lo_najdi_texturu(p_textlist,textlistnum,p_fram->file,flag))==K_CHYBA) {
      if((t = lo_najdi_volnou_texturu(p_textlist,textlistnum)) == K_CHYBA)
        chyba("Neni volna textura !");
      strcpy(p_textlist[t].jmeno, p_fram->file);
      p_textlist[t].flag = flag;
    }
  */
    p_fram->p_text = p_textlist+t;

    return(TRUE);
  } else
    return(FALSE);
}


int lo_chunk_load_matanim(FFILE f, OUT_CHUNK *p_ch)
{
  if(p_mat && p_ch->typ == CHUNK_MATANIM_EXT) {
    p_fanim = &p_mat->anim;
    p_mat->flag |= MAT_ANIM_FRAME;
    str_read(p_fanim->jmeno,f);
    ffread(&p_fanim->framenum,sizeof(p_fanim->framenum),1,f);
    p_fanim->p_frame = (ANIM_FRAME *)mmalloc(sizeof(p_fanim->p_frame[0])*p_fanim->framenum);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_textmatrix(FFILE f, OUT_CHUNK *p_ch)
{
  if(p_mat && p_ch->typ == CHUNK_MATMATRIX_EXT) {
    ffread(p_mat->ttext,sizeof(p_mat->ttext[0]),1,f);
    ffread(p_mat->ttext+1,sizeof(p_mat->ttext[0]),1,f);
    ffread(p_mat->ttext+2,sizeof(p_mat->ttext[0]),1,f);
    ffread(p_mat->ttext+3,sizeof(p_mat->ttext[0]),1,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_mat_texgen_param(FFILE f, OUT_CHUNK *p_ch)
{
  if(p_mat && p_ch->typ == CHUNK_MAT_TEXGEN) {  
    ffread(p_mat->texgen_koord_t,sizeof(p_mat->texgen_koord_t),1,f);
    ffread(p_mat->texgen_koord_s,sizeof(p_mat->texgen_koord_s),1,f);
    ffread(p_mat->texgen_koord_r,sizeof(p_mat->texgen_koord_r),1,f);
    ffread(p_mat->texgen_koord_q,sizeof(p_mat->texgen_koord_q),1,f);
    return(TRUE);
  } else {
    return(FALSE);
  }
}

int lo_chunk_load_matanim_text_1(FFILE f, OUT_CHUNK *p_ch)
{
  ANIM_TEXT tmp,*p_track;
  int i;
  if(p_mat && p_ch->typ == CHUNK_MATANIM_T1) {
    p_mat->flag |= MAT2_T1_MATRIX;
    for(i = 0; i < MAX_TEXT_ANIM; i++) {
      if(!p_mat->p_atext[0][i])
        break;
    }
    assert(i < MAX_TEXT_ANIM);
  
    p_mat->atextnum[0] = i+1;
    ffread(&tmp,sizeof(tmp),1,f);
    
    p_track = p_mat->p_atext[0][i] = key_vyrob_material_animace(tmp.pos_keys,tmp.rot_keys,tmp.scs_keys,tmp.piv_keys);
    p_track->flag = tmp.flag;          // flagy animace (loop a pod)
    p_track->endtime = tmp.endtime;  // framenum = 0 - volna animace
    
    if(p_track->pos_keys) {
      ffread(p_track->p_pos,sizeof(p_track->p_pos[0]),p_track->pos_keys,f);
      ffread(p_track->p_pkeys,sizeof(p_track->p_pkeys[0]),p_track->pos_keys,f);
    }  
    
    if(p_track->piv_keys) {
      ffread(p_track->p_piv,sizeof(p_track->p_piv[0]),p_track->piv_keys,f);
      ffread(p_track->p_vkeys,sizeof(p_track->p_vkeys[0]),p_track->piv_keys,f);
    }  

    if(p_track->rot_keys) {
      ffread(p_track->p_rot,sizeof(p_track->p_rot[0]),p_track->rot_keys,f);
      ffread(p_track->p_rkeys,sizeof(p_track->p_rkeys[0]),p_track->rot_keys,f);
    }  
    
    if(p_track->scs_keys) {
      ffread(p_track->p_scale,sizeof(p_track->p_scale[0]),p_track->scs_keys,f);
      ffread(p_track->p_skeys,sizeof(p_track->p_skeys[0]),p_track->scs_keys,f);
    }
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_matanim_text_2(FFILE f, OUT_CHUNK *p_ch)
{
  ANIM_TEXT tmp,*p_track;
  int i;
  
  if(p_mat && p_ch->typ == CHUNK_MATANIM_T2) {
    p_mat->flag |= MAT2_T2_MATRIX;
    for(i = 0; i < MAX_TEXT_ANIM; i++) {
      if(!p_mat->p_atext[1][i])
        break;
    }
    assert(i < MAX_TEXT_ANIM);
    
    p_mat->atextnum[1] = i+1;
    ffread(&tmp,sizeof(tmp),1,f);
    
    p_track = p_mat->p_atext[1][i] = key_vyrob_material_animace(tmp.pos_keys,tmp.rot_keys,tmp.scs_keys,tmp.piv_keys);
    p_track->flag = tmp.flag;          // flagy animace (loop a pod)
    p_track->endtime = tmp.endtime;  // framenum = 0 - volna animace
    
    if(p_track->pos_keys) {
      ffread(p_track->p_pos,sizeof(p_track->p_pos[0]),p_track->pos_keys,f);
      ffread(p_track->p_pkeys,sizeof(p_track->p_pkeys[0]),p_track->pos_keys,f);
    }  

    if(p_track->piv_keys) {
      ffread(p_track->p_piv,sizeof(p_track->p_piv[0]),p_track->piv_keys,f);
      ffread(p_track->p_vkeys,sizeof(p_track->p_vkeys[0]),p_track->piv_keys,f);
    }  
    
    if(p_track->rot_keys) {
      ffread(p_track->p_rot,sizeof(p_track->p_rot[0]),p_track->rot_keys,f);
      ffread(p_track->p_rkeys,sizeof(p_track->p_rkeys[0]),p_track->rot_keys,f);
    }  
    
    if(p_track->scs_keys) {
      ffread(p_track->p_scale,sizeof(p_track->p_scale[0]),p_track->scs_keys,f);
      ffread(p_track->p_skeys,sizeof(p_track->p_skeys[0]),p_track->scs_keys,f);
    }
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_matanim_text_3(FFILE f, OUT_CHUNK *p_ch)
{
  ANIM_TEXT tmp,*p_track;
  int i;
  
  if(p_mat && p_ch->typ == CHUNK_MATANIM_T3) {
    p_mat->flag |= MAT2_T3_MATRIX;
    for(i = 0; i < MAX_TEXT_ANIM; i++) {
      if(!p_mat->p_atext[2][i])
        break;
    }
    assert(i < MAX_TEXT_ANIM);
    
    p_mat->atextnum[2] = i+1;
    ffread(&tmp,sizeof(tmp),1,f);
    
    p_track = p_mat->p_atext[2][i] = key_vyrob_material_animace(tmp.pos_keys,tmp.rot_keys,tmp.scs_keys,tmp.piv_keys);
    p_track->flag = tmp.flag;          // flagy animace (loop a pod)
    p_track->endtime = tmp.endtime;  // framenum = 0 - volna animace
    
    if(p_track->pos_keys) {
      ffread(p_track->p_pos,sizeof(p_track->p_pos[0]),p_track->pos_keys,f);
      ffread(p_track->p_pkeys,sizeof(p_track->p_pkeys[0]),p_track->pos_keys,f);
    }  

    if(p_track->piv_keys) {
      ffread(p_track->p_piv,sizeof(p_track->p_piv[0]),p_track->piv_keys,f);
      ffread(p_track->p_vkeys,sizeof(p_track->p_vkeys[0]),p_track->piv_keys,f);
    }  
    
    if(p_track->rot_keys) {
      ffread(p_track->p_rot,sizeof(p_track->p_rot[0]),p_track->rot_keys,f);
      ffread(p_track->p_rkeys,sizeof(p_track->p_rkeys[0]),p_track->rot_keys,f);
    }  
    
    if(p_track->scs_keys) {
      ffread(p_track->p_scale,sizeof(p_track->p_scale[0]),p_track->scs_keys,f);
      ffread(p_track->p_skeys,sizeof(p_track->p_skeys[0]),p_track->scs_keys,f);
    }
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_matanim_text_4(FFILE f, OUT_CHUNK *p_ch)
{
  ANIM_TEXT tmp,*p_track;
  int i;
  
  if(p_mat && p_ch->typ == CHUNK_MATANIM_T4) {
    p_mat->flag |= MAT2_T4_MATRIX;
    for(i = 0; i < MAX_TEXT_ANIM; i++) {
      if(!p_mat->p_atext[3][i])
        break;
    }
    assert(i < MAX_TEXT_ANIM);
    
    p_mat->atextnum[3] = i+1;
    ffread(&tmp,sizeof(tmp),1,f);
    
    p_track = p_mat->p_atext[3][i] = key_vyrob_material_animace(tmp.pos_keys,tmp.rot_keys,tmp.scs_keys,tmp.piv_keys);
    p_track->flag = tmp.flag;          // flagy animace (loop a pod)
    p_track->endtime = tmp.endtime;  // framenum = 0 - volna animace
    
    if(p_track->pos_keys) {
      ffread(p_track->p_pos,sizeof(p_track->p_pos[0]),p_track->pos_keys,f);
      ffread(p_track->p_pkeys,sizeof(p_track->p_pkeys[0]),p_track->pos_keys,f);
    }  

    if(p_track->piv_keys) {
      ffread(p_track->p_piv,sizeof(p_track->p_piv[0]),p_track->piv_keys,f);
      ffread(p_track->p_vkeys,sizeof(p_track->p_vkeys[0]),p_track->piv_keys,f);
    }  
    
    if(p_track->rot_keys) {
      ffread(p_track->p_rot,sizeof(p_track->p_rot[0]),p_track->rot_keys,f);
      ffread(p_track->p_rkeys,sizeof(p_track->p_rkeys[0]),p_track->rot_keys,f);
    }  
    
    if(p_track->scs_keys) {
      ffread(p_track->p_scale,sizeof(p_track->p_scale[0]),p_track->scs_keys,f);
      ffread(p_track->p_skeys,sizeof(p_track->p_skeys[0]),p_track->scs_keys,f);
    }
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_mat_lesk(FFILE f, OUT_CHUNK *p_ch)
{  
  if(p_mat && p_ch->typ == CHUNK_LESK) {       
    ffread(&p_mat->lesk,sizeof(float),1,f);
    return(TRUE);
  } else {
    return(FALSE);
  }
}

int lo_chunk_load_stage(FFILE f, OUT_CHUNK *p_ch)
{
  if(p_mat && p_ch->typ == CHUNK_STAG_TAG) {
  /* TODO
    ffread(&p_mat->alfa_state,sizeof(p_mat->alfa_state),1,f);
    ffread(&p_mat->text_state[0].text_stage,sizeof(int),1,f);
    ffread(&p_mat->text_state[1].text_stage,sizeof(int),1,f);
  */
    return(TRUE);
  } else {    
    return(FALSE);
  }
}

int lo_chunk_load_mat_alfa_stage(FFILE f, OUT_CHUNK *p_ch)
{
  if(p_mat && p_ch->typ == CHUNK_ALFA_STAGE) {
    ffread(&p_mat->alfa_state,sizeof(p_mat->alfa_state),1,f);
    return(TRUE);
  } else {    
    return(FALSE);
  }
}

int lo_chunk_load_mat_text_stage_1(FFILE f, OUT_CHUNK *p_ch)
{/*
  if(p_mat && p_ch->typ == CHUNK_TEXT_STAGE_1) {
    ffread(&p_mat->text_state[0].text_stage,sizeof(int),1,f);
    return(TRUE);
  } else {    
    return(FALSE);
  }
*/
}

int lo_chunk_load_mat_text_stage_2(FFILE f, OUT_CHUNK *p_ch)
{
/*
  if(p_mat && p_ch->typ == CHUNK_TEXT_STAGE_2) {
    ffread(&p_mat->text_state[1].text_stage,sizeof(int),1,f);
    return(TRUE);
  } else {    
    return(FALSE);
  }
*/
}

int lo_chunk_load_mat_text_stage_3(FFILE f, OUT_CHUNK *p_ch)
{/*
  if(p_mat && p_ch->typ == CHUNK_TEXT_STAGE_3) {
    ffread(&p_mat->text_state[2].text_stage,sizeof(int),1,f);
    return(TRUE);
  } else {    
    return(FALSE);
  }
*/
}

int lo_chunk_load_mat_text_stage_4(FFILE f, OUT_CHUNK *p_ch)
{/*
  if(p_mat && p_ch->typ == CHUNK_TEXT_STAGE_4) {
    ffread(&p_mat->text_state[3].text_stage,sizeof(int),1,f);
    return(TRUE);
  } else {    
    return(FALSE);
  }*/
}

int lo_chunk_load_mat_text_stage_advaced(FFILE f, OUT_CHUNK *p_ch)
{/*
  if(p_mat) {
    int num = p_ch->typ-CHUNK_TEXT_STAGE_AD_1;
    ffread(&p_mat->text_state[num].text_stage,sizeof(int),1,f);
    ffread(&p_mat->text_state[num].textury,sizeof(int),4,f);
    ffread(&p_mat->text_state[num].text_koord,sizeof(int),4,f);
    ffread(&p_mat->text_state[num].text_funkce,sizeof(int),4,f);
    ffread(&p_mat->text_state[num].text_ati_bump_matrix,
      sizeof(p_mat->text_state[num].text_ati_bump_matrix[0]),4,f);    
    return(TRUE);
  } else {
    return(FALSE);
  }*/
}

int lo_chunk_load_mat_env_spec(FFILE f, OUT_CHUNK *p_ch)
{ 
  if(p_mat && p_ch->typ == CHUNK_ENVSPEC) {
    ffread(&p_mat->env_add1,sizeof(p_mat->env_add1),1,f);
    ffread(&p_mat->env_add2,sizeof(p_mat->env_add2),1,f);
    ffread(&p_mat->env_scale,sizeof(p_mat->env_scale),1,f);
    ffread(&p_mat->env_r,sizeof(p_mat->env_r),1,f);
    ffread(&p_mat->env_g,sizeof(p_mat->env_g),1,f);
    ffread(&p_mat->env_b,sizeof(p_mat->env_b),1,f);
    return(TRUE);
  } else
    return(FALSE);
}


int lo_chunk_load_mat_text_flag(FFILE f, OUT_CHUNK *p_ch)
{
  if(p_mat) {
    int num = p_ch->typ-CHUNK_TEXT_FLAG_1;
    ffread(p_mat->textflag+num,sizeof(p_mat->textflag[num]),1,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_text(FFILE f, OUT_CHUNK *p_ch)
{
  if(p_mat) {
    int t,num;
    switch(p_ch->typ) {
      case CHUNK_TEXT_1:
        num = 0;
        break;
      case CHUNK_TEXT_2:
        num = 1;
        break;
      case CHUNK_TEXT_3:
        num = 2;
        break;
      default:
      case CHUNK_TEXT_4:
        num = 3;
        break;
    }
    str_read(p_mat->textfile[num],f);
    if((t=lo_najdi_texturu(p_textlist,textlistnum,p_mat->textfile[num],p_mat->textflag[num]))==K_CHYBA) {
      if((t=lo_najdi_volnou_texturu(p_textlist,textlistnum)) == K_CHYBA)
        chyba("Neni volna textura !");      
      // Zaloz novou texturu
      // TODO
      //strcpy(p_textlist[t].jmeno, p_mat->textfile[num]);
      //p_textlist[t].flag = p_mat->textflag[num];
    }
    p_mat->p_text[num] = p_textlist+t;
    return(TRUE);
  } else
    return(FALSE);
}


/************************************************************************
  Loading kontejneru
*/
int lo_chunk_load_kont_jmeno(FFILE f, OUT_CHUNK *p_ch)
{
  int  pozice,i;
  char jmeno[200];

  if(p_ch->typ == CHUNK_KONT_JMENO) {

    ffread(&pozice,sizeof(pozice),1,f);
    str_read(jmeno,f);    

    if(p_kontlist) {
      if(pozice == K_CHYBA) {
        if((i = lo_najdi_volny_kontejner(p_kontlist,kontlistnum)) == K_CHYBA) {
          kprintf(1,"Cannot find free kont position!");
          return(FALSE);
        }          
      } else {
        i = pozice;        
        //assert(!p_kontlist[i]);
        if(p_kontlist[i]) {
          kprintf(1,"Pozition %d is full (%s)",i,p_kontlist[i]->jmeno);
          if((i = lo_najdi_volny_kontejner(p_kontlist,kontlistnum)) == K_CHYBA) {
            kprintf(1,"Cannot find free kont position!");
            return(FALSE);
          }
          kprintf(1,"New position is %d kont %s",i,jmeno);
        }
      }
      p_kont = p_kontlist[i] = vyrob_kontejner();
      strcpy(p_kont->jmeno,jmeno);    
    } else {
      if(p_kont) {
        if(kont_load) {
          p_kont_old = p_kont;
          p_kont = NULL;          
        } else {
          kont_load++;          
          strcpy(p_kont->jmeno,jmeno);
        }      
      }
    }
    p_kont->k2flag |= KONT2_LOADED;
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_kont_low_jmeno(FFILE f, OUT_CHUNK *p_ch)
{
  if(p_kont && p_ch->typ == CHUNK_KONT_LOW_JMENO) {
    p_kont = (p_kont->p_next = vyrob_kontejner());
    str_read(p_kont->jmeno,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_kont_low_id(FFILE f, OUT_CHUNK *p_ch)
{
  if(p_kont && p_ch->typ == CHUNK_KONT_LOW_ID) {
    ffread(&p_kont->low_id,sizeof(p_kont->low_id),1,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_kont_id(FFILE f, OUT_CHUNK *p_ch)
{
  if(p_kont && p_ch->typ == CHUNK_KONT_ID) {
    ffread(&p_kont->kontejner_ID,sizeof(p_kont->kontejner_ID),1,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_kont_flag(FFILE f, OUT_CHUNK *p_ch)
{
  if(p_kont && p_ch->typ == CHUNK_KONT_FLAG) {
    ffread(&p_kont->kflag,sizeof(dword),1,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_kont_flag_ext(FFILE f, OUT_CHUNK *p_ch)
{
  if(p_kont && p_ch->typ == CHUNK_KONT_FLAG_EXT) {
    ffread(&p_kont->m1flag,sizeof(dword),1,f);
    ffread(&p_kont->m2flag,sizeof(dword),1,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_kont_flag_ext_2(FFILE f, OUT_CHUNK *p_ch)
{
  if(p_kont && p_ch->typ == CHUNK_KONT_FLAG_EXT_2) {
    ffread(&p_kont->k2flag,sizeof(dword),1,f);  
    p_kont->k2flag |= KONT2_LOADED;
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_kont_obb(FFILE f, OUT_CHUNK *p_ch)
{
  if(p_kont && p_ch->typ == CHUNK_KONT_OBB) {
    ffread(&p_kont->obb,sizeof(p_kont->obb),1,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_kont_wmatrix(FFILE f, OUT_CHUNK *p_ch)
{  
  if(p_kont && p_ch->typ == CHUNK_KONT_WMATRIX) {          
    ffread(&p_kont->world,sizeof(p_kont->world),1,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_kont_mpmatrix(FFILE f, OUT_CHUNK *p_ch)
{  
  if(p_kont && p_ch->typ == CHUNK_KONT_MPMATRIX) {
    p_kont->kflag |= KONT_MPMATRIX;
    ffread(&p_kont->mpmatrix,sizeof(p_kont->mpmatrix),1,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_obj_jmeno(FFILE f, OUT_CHUNK *p_ch)
{
  int i;
  
  // Nacte jmeno objektu + vybere novy objekt
  if(p_kont && p_ch->typ == CHUNK_OBJ_JMENO) {
    i = lo_najdi_volny_objekt_kont(p_kont);
    if(i == K_CHYBA)
      chyba("Neni volny objekt!");
    
    p_obj = p_kont->p_obj[i] = vyrob_objekt();
    
    str_read(p_obj->jmeno,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_obj_ID(FFILE f, OUT_CHUNK *p_ch)
{
  if(p_kont && p_ch->typ == CHUNK_OBJ_ID) {
    ffread(&p_obj->Objekt_ID,sizeof(p_obj->Objekt_ID),1,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_obj_pivot(FFILE f, OUT_CHUNK *p_ch)
{
  if(p_kont && p_ch->typ == CHUNK_OBJ_PIVOT) {
    ffread(&p_obj->pivot,sizeof(p_obj->pivot),1,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_obj_obb(FFILE f, OUT_CHUNK *p_ch)
{
  if(p_kont && p_ch->typ == CHUNK_OBJ_OBB) {
    ffread(&p_obj->obb,sizeof(p_obj->obb),1,f);
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_obj_material(FFILE f, OUT_CHUNK *p_ch)
{  
  char jmeno_materialu[200];

  if(p_obj && p_ch->typ == CHUNK_OBJ_MATERIAL) {
    str_read(jmeno_materialu,f);
    if((p_obj->material = lo_najdi_material(p_matlist,matlistnum,jmeno_materialu)) == K_CHYBA) {
      kprintf(1,"Cannot find material %s",jmeno_materialu);
      assert(0);
    }
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_obj_matrix(FFILE f, OUT_CHUNK *p_ch)
{  
  if(p_obj && p_ch->typ == CHUNK_OBJ_MATRIX_KEY) {
    ffread(&p_obj->m,sizeof(p_obj->m),1,f);
    return(TRUE);
  } else
    return(FALSE);      
}

int lo_chunk_load_obj_vertex(FFILE f, OUT_CHUNK *p_ch)
{
  int   pocet,i;
  float v[3];

  if(p_obj && p_ch->typ == CHUNK_OBJ_VERTEXY) {
    
    if(!ffread(&pocet,sizeof(dword),1,f))
      chyba("Cteni");
    p_obj->vertexnum = pocet;
    
    if(!(p_obj->p_vertex = (OBJEKT_BOD *)mmalloc(sizeof(OBJEKT_BOD)*pocet)))
      chyba("Nedostatek pameti...");
    memset(p_obj->p_vertex,0,sizeof(OBJEKT_BOD)*pocet);

    for(i = 0; i < pocet; i++) {
      if(!ffread(&v,sizeof(v),1,f))
        chyba("Cteni!");
      p_obj->p_vertex[i].x = v[0];
      p_obj->p_vertex[i].y = v[1];
      p_obj->p_vertex[i].z = v[2];
    }

    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_obj_normal(FFILE f, OUT_CHUNK *p_ch)
{
  int   pocet,i;
  float v[3];

  if(p_obj && p_obj->p_vertex && p_ch->typ == CHUNK_OBJ_NORMAL) {
    if(!ffread(&pocet,sizeof(dword),1,f))
      chyba("Cteni");
    for(i = 0; i < pocet; i++) {
      if(!ffread(&v,sizeof(float),3,f))
        chyba("Cteni!");
      p_obj->p_vertex[i].nx = v[0];
      p_obj->p_vertex[i].ny = v[1];
      p_obj->p_vertex[i].nz = v[2];
    }

    return(TRUE);
  } else
    return(FALSE);
}


int lo_chunk_load_obj_face(FFILE f, OUT_CHUNK *p_ch)
{
  int pocet;

  if(p_obj && p_ch->typ == CHUNK_OBJ_FACY) {    
    
    if(!ffread(&pocet,sizeof(pocet),1,f))
      chyba("Cteni");
    p_obj->facenum = pocet;

    p_obj->p_face = (FACE *)mmalloc(sizeof(FACE)*pocet);
    if(!ffread(p_obj->p_face,sizeof(p_obj->p_face[0]),pocet,f))
      chyba("Cteni");

    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_obj_face_ext(FFILE f, OUT_CHUNK *p_ch)
{
  if(p_obj && p_ch->typ == CHUNK_OBJ_FACY_OPT) {
    
    if(!ffread(&p_obj->optnum,sizeof(p_obj->optnum),1,f))
      chyba("Cteni");    
    
    p_obj->p_opt = (int *)mmalloc(sizeof(p_obj->p_opt[0])*p_obj->optnum);
    if(!ffread(p_obj->p_opt,sizeof(p_obj->p_opt[0]),p_obj->optnum,f))
      chyba("Cteni");

    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_obj_mapy_1(FFILE f, OUT_CHUNK *p_ch)
{  
  float m[2];
  int  pocet,i;

  if(p_obj && p_ch->typ == CHUNK_OBJ_MAPY_1) {
    if(!ffread(&pocet,sizeof(pocet),1,f))
      chyba("Zapis");
    assert(p_obj->vertexnum == pocet);
    
    for(i = 0; i < pocet; i++) {
      ffread(&m,sizeof(m),1,f);
      p_obj->p_vertex[i].tu1 = m[0];
      p_obj->p_vertex[i].tv1 = m[1];        
    }    
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_obj_mapy_2(FFILE f, OUT_CHUNK *p_ch)
{  
  float m[2];
  int pocet,i;

  if(p_obj && p_ch->typ == CHUNK_OBJ_MAPY_2) {
    if(!ffread(&pocet,sizeof(pocet),1,f))
      chyba("Zapis");
    assert(p_obj->vertexnum == pocet);
    
    for(i = 0; i < pocet; i++) {
      ffread(&m,sizeof(m),1,f);
      p_obj->p_vertex[i].tu2 = m[0];
      p_obj->p_vertex[i].tv2 = m[1];        
    }    
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_obj_mapy_3(FFILE f, OUT_CHUNK *p_ch)
{  
  float m[2];
  int pocet,i;

  if(p_obj && p_ch->typ == CHUNK_OBJ_MAPY_3) {
    if(!ffread(&pocet,sizeof(pocet),1,f))
      chyba("Zapis");
    assert(p_obj->vertexnum == pocet);
    
    for(i = 0; i < pocet; i++) {
      ffread(&m,sizeof(m),1,f);
      p_obj->p_vertex[i].tu3 = m[0];
      p_obj->p_vertex[i].tv3 = m[1];        
    }    
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_obj_mapy_4(FFILE f, OUT_CHUNK *p_ch)
{  
  float m[2];
  int pocet,i;

  if(p_obj && p_ch->typ == CHUNK_OBJ_MAPY_4) {
    if(!ffread(&pocet,sizeof(pocet),1,f))
      chyba("Zapis");
    assert(p_obj->vertexnum == pocet);
    
    for(i = 0; i < pocet; i++) {
      ffread(&m,sizeof(m),1,f);
      p_obj->p_vertex[i].tu4 = m[0];
      p_obj->p_vertex[i].tv4 = m[1];        
    }    
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_obj_barvy(FFILE f, OUT_CHUNK *p_ch)
{
  int pocet,i;
  float v[4] = {0,0,0,0};

  if(p_obj && p_ch->typ == CHUNK_OBJ_BARVY) {    
    ffread(&pocet,sizeof(pocet),1,f);
    
    assert(p_obj->vertexnum == pocet);
    
    for(i = 0; i < pocet; i++) {
      if(!ffread(&v,sizeof(float),4,f))
        chyba("Zapis!");      
      p_obj->p_vertex[i].dr = v[0];
      p_obj->p_vertex[i].dg = v[1];
      p_obj->p_vertex[i].db = v[2];
    }
    
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_obj_barvy_ext(FFILE f, OUT_CHUNK *p_ch)
{
  int pocet,i;
  float v[4] = {0,0,0,0};

  if(p_obj && p_ch->typ == CHUNK_OBJ_BARVY_EXT) {
    ffread(&pocet,sizeof(pocet),1,f);
    
    assert(p_obj->vertexnum == pocet);
    
    for(i = 0; i < pocet; i++) {
      if(!ffread(&v,sizeof(float),4,f))
        chyba("Read!");      
      p_obj->p_vertex[i].dr = v[0];
      p_obj->p_vertex[i].dg = v[1];
      p_obj->p_vertex[i].db = v[2];
      p_obj->p_vertex[i].da = v[3];

      if(!ffread(&v,sizeof(float),4,f))
        chyba("Read!");      
      p_obj->p_vertex[i].mdr = v[0];
      p_obj->p_vertex[i].mdg = v[1];
      p_obj->p_vertex[i].mdb = v[2];
      p_obj->p_vertex[i].mda = v[3];

      if(!ffread(&v,sizeof(float),4,f))
        chyba("Read!");      
      p_obj->p_vertex[i].sr = v[0];
      p_obj->p_vertex[i].sg = v[1];
      p_obj->p_vertex[i].sb = v[2];

      if(!ffread(&v,sizeof(float),4,f))
        chyba("Read!");      
      p_obj->p_vertex[i].msr = v[0];
      p_obj->p_vertex[i].msg = v[1];
      p_obj->p_vertex[i].msb = v[2];
    }
    
    return(TRUE);
  } else
    return(FALSE);
}

JOINT * lo_najdi_joint(JOINT *p_joint, int ID)
{
  JOINT *t1;

  if(!p_joint)
    return(NULL);

  if(p_joint->joint_ID == ID) {
    return(p_joint);
  } else {
    if((t1 = lo_najdi_joint(p_joint->p_child,ID)))
      return(t1);
    if((t1 = lo_najdi_joint(p_joint->p_next,ID)))
      return(t1);
    return(NULL);
  }
}

int lo_chunk_load_obj_joint_ID(FFILE f, OUT_CHUNK *p_ch)
{
  if(p_joint_animace && p_ch->typ == CHUNK_OBJ_JNT_ID) {
    JOINT *p_jnt = key_joint_vyrob();
    int    ID;
    int    prev_ID;

    // oflaguj kontejner
    p_kont->k2flag |= KONT2_JOINT_ANIM;
    p_kont->kflag |= KONT_VERTEX_LIGHT;
    p_kont->kflag &= ~KONT_STATIC;

    ffread(&ID,sizeof(ID),1,f);
    ffread(&prev_ID,sizeof(prev_ID),1,f);
    
    p_joint = p_jnt;
    if(prev_ID == ROOT_ID) {      
      if(!p_joint_animace->p_child) {
        p_joint_animace->p_child = p_joint;
      } else {
        p_joint->p_next = p_joint_animace->p_child->p_next;
        p_joint_animace->p_child->p_next = p_joint;
      }      
    } else {
      JOINT *p_prev = lo_najdi_joint(p_joint_animace->p_child,prev_ID);
      if(p_prev->p_child) {
        p_joint->p_next = p_prev->p_next;
        p_prev->p_next = p_joint;
      } else {
        p_prev->p_child = p_joint;
      }
    }
    p_joint->joint_ID = ID;
  }
  return(TRUE);
}

int lo_chunk_load_obj_joint_pivot(FFILE f, OUT_CHUNK *p_ch)
{
  if(p_joint && p_ch->typ == CHUNK_OBJ_JNT_PIVOT) {
    ffread(&p_joint->pivot,sizeof(p_joint->pivot),1,f);
  }
  return(TRUE);
}

int lo_chunk_load_obj_joint_flag(FFILE f, OUT_CHUNK *p_ch)
{
  if(p_joint && p_ch->typ == CHUNK_OBJ_JNT_FLAG) {
    ffread(&p_joint->flag,sizeof(p_joint->flag),1,f);
  }
  return(TRUE);
}

int lo_chunk_load_obj_joint_vertexy(FFILE f, OUT_CHUNK *p_ch)
{
  if(p_joint && p_ch->typ == CHUNK_OBJ_JNT_VERT) {
    ffread(&p_joint->vertexnum,sizeof(p_joint->vertexnum),1,f);
    p_joint->p_vertexlist = (int *)mmalloc(sizeof(p_joint->p_vertexlist[0])*p_joint->vertexnum);
    ffread(p_joint->p_vertexlist,sizeof(p_joint->p_vertexlist[0]),p_joint->vertexnum,f);      
  }
  return(TRUE);
}

int lo_chunk_load_obj_joint_pos(FFILE f, OUT_CHUNK *p_ch)
{
  if(p_joint && p_ch->typ == CHUNK_OBJ_JNT_POS) {  
    ffread(&p_joint->pos_keys,sizeof(p_joint->pos_keys),1,f);
    p_joint->p_pos = (BOD *)mmalloc(sizeof(p_joint->p_pos[0])*p_joint->pos_keys);
    ffread(p_joint->p_pos,sizeof(p_joint->p_pos[0]),p_joint->pos_keys,f);
  }
  return(TRUE);
}

int lo_chunk_load_obj_joint_rot(FFILE f, OUT_CHUNK *p_ch)
{
  if(p_joint && p_ch->typ == CHUNK_OBJ_JNT_ROT) {
    ffread(&p_joint->rot_keys,sizeof(p_joint->rot_keys),1,f);
    p_joint->p_rot = (QUAT *)mmalloc(sizeof(p_joint->p_rot[0])*p_joint->rot_keys);
    ffread(p_joint->p_rot,sizeof(p_joint->p_rot[0]),p_joint->rot_keys,f);
  }
  return(TRUE);
}

int lo_chunk_load_obj_joint_scs(FFILE f, OUT_CHUNK *p_ch)
{
  if(p_joint && p_ch->typ == CHUNK_OBJ_JNT_SCS) {
    ffread(&p_joint->scs_keys,sizeof(p_joint->scs_keys),1,f);
    p_joint->p_scs = (BOD *)mmalloc(sizeof(p_joint->p_scs[0])*p_joint->scs_keys);
    ffread(p_joint->p_scs,sizeof(p_joint->p_scs[0]),p_joint->scs_keys,f);
  }
  return(TRUE);
}

/* Spline klice
*/
int lo_chunk_load_obj_joint_pos_spline(FFILE f, OUT_CHUNK *p_ch)
{
  JOINT_KEYS *p_key;
  if(p_joint && p_ch->typ == CHUNK_OBJ_JNT_KEYPOS) {
    if(!p_joint->p_keys) {
      p_key = p_joint->p_keys = (JOINT_KEYS *)mmalloc(sizeof(p_joint->p_keys[0]));
    }
    p_key = p_joint->p_keys;
    ffread(&p_key->pos_keys,sizeof(p_key->pos_keys),1,f);
  
    p_key->p_pos = (BOD *)mmalloc(sizeof(p_key->p_pos[0])*p_key->pos_keys);
    ffread(p_key->p_pos,sizeof(p_key->p_pos[0]),p_key->pos_keys,f);
    
    p_key->p_pkeys = (KEY_POINT_BRS *)mmalloc(sizeof(p_key->p_pkeys[0])*p_key->pos_keys);
    ffread(p_key->p_pkeys,sizeof(p_key->p_pkeys[0]),p_key->pos_keys,f);
  }
  return(TRUE);
}

int lo_chunk_load_obj_joint_rot_spline(FFILE f, OUT_CHUNK *p_ch)
{
  JOINT_KEYS *p_key;
  if(p_joint && p_ch->typ == CHUNK_OBJ_JNT_KEYROT) {
    if(!p_joint->p_keys) {
      p_key = p_joint->p_keys = (JOINT_KEYS *)mmalloc(sizeof(p_joint->p_keys[0]));
    }
    p_key = p_joint->p_keys;
    ffread(&p_key->rot_keys,sizeof(p_key->rot_keys),1,f);
    
    p_key->p_rot = (QUAT *)mmalloc(sizeof(p_key->p_rot[0])*p_key->rot_keys);
    ffread(p_key->p_rot,sizeof(p_key->p_rot[0]),p_key->rot_keys,f);
    
    p_key->p_rkeys = (KEY_POINT_BRS *)mmalloc(sizeof(p_key->p_rkeys[0])*p_key->rot_keys);
    ffread(p_key->p_rkeys,sizeof(p_key->p_rkeys[0]),p_key->rot_keys,f);
  }
  return(TRUE);
}

int lo_chunk_load_obj_joint_scs_spline(FFILE f, OUT_CHUNK *p_ch)
{  
  JOINT_KEYS *p_key;
  if(p_joint && p_ch->typ == CHUNK_OBJ_JNT_KEYSCS) {
    if(!p_joint->p_keys) {
      p_key = p_joint->p_keys = (JOINT_KEYS *)mmalloc(sizeof(p_joint->p_keys[0]));
    }
    p_key = p_joint->p_keys;
    ffread(&p_key->scs_keys,sizeof(p_key->scs_keys),1,f);
    
    p_key->p_scale = (BOD *)mmalloc(sizeof(p_key->p_scale[0])*p_key->scs_keys);
    ffread(p_key->p_scale,sizeof(p_key->p_scale[0]),p_key->scs_keys,f);
    
    p_key->p_skeys = (KEY_POINT_BRS *)mmalloc(sizeof(p_key->p_skeys[0])*p_key->scs_keys);
    ffread(p_key->p_skeys,sizeof(p_key->p_skeys[0]),p_key->scs_keys,f);
  }
  return(TRUE);
}

int lo_chunk_load_obj_joint_animace(FFILE f, OUT_CHUNK *p_ch)
{
  if(p_obj && p_ch->typ == CHUNK_OBJ_JA) {
    JOINT_ANIMACE *p_anim = (JOINT_ANIMACE *)mmalloc(sizeof(p_anim[0]));
    
    str_read(p_anim->jmeno,f);
    
    p_anim->p_next = p_joint_animace;
    p_joint_animace = p_anim;
    
    p_joint_animace->p_next = p_obj->p_joit_animace;
    p_obj->p_joit_animace = p_joint_animace;
  }
  return(TRUE);
}

int lo_chunk_load_obj_joint_framy(FFILE f, OUT_CHUNK *p_ch)
{
  if(p_joint_animace && p_ch->typ == CHUNK_OBJ_JA_FRAMU) {  
    ffread(&p_joint_animace->framenum,sizeof(p_joint_animace->framenum),1,f);
  }
  return(TRUE);
}

/******************************************************************************
  Load sim animaci
*/

inline int lo_najdi_volny_sim(HIERARCHY_SIM *p_sim, int max)
{
  int i;
  for(i = 0; i < max; i++) {
    if(!p_sim[i].keynum)
      return(i);
  }
  return(K_CHYBA);
}

int lo_chunk_load_sim_node(FFILE f, SIMPLE_TRACK_INFO *p_sim)
{    
  OUT_CHUNK ch;  
  int       klicu,i;

  ffread(&ch,sizeof(ch),1,f);

  if(p_sim && (ch.typ == CHUNK_SIM_NODE || ch.typ == CHUNK_SIM_NODE_EXT || ch.typ == CHUNK_SIM_NODE_EXT2)) {
    
    memset(p_sim,0,sizeof(p_sim[0]));

    ffread(&p_sim->Objekt_ID,sizeof(p_sim->Objekt_ID),1,f);
    ffread(&p_sim->flag,sizeof(p_sim->flag),1,f);
    ffread(&p_sim->keynum,sizeof(p_sim->keynum),1,f);
    ffread(&p_sim->pivot,sizeof(p_sim->pivot),1,f);
    ffread(&p_sim->pivot3ds,sizeof(p_sim->pivot3ds),1,f);
    ffread(&p_sim->childnum,sizeof(p_sim->childnum),1,f);
    
    ffread(&klicu,sizeof(klicu),1,f);
    if(klicu) {
      p_sim->p_pos = (BOD *)mmalloc(sizeof(p_sim->p_pos[0])*p_sim->keynum);
      ffread(p_sim->p_pos,sizeof(p_sim->p_pos[0]),p_sim->keynum,f);
    } else {
      p_sim->p_pos = NULL;
    }
    
    ffread(&klicu,sizeof(klicu),1,f);
    if(klicu) {
      p_sim->p_scale = (BOD *)mmalloc(sizeof(p_sim->p_scale[0])*p_sim->keynum);
      ffread(p_sim->p_scale,sizeof(p_sim->p_scale[0]),p_sim->keynum,f);
    } else {
      p_sim->p_scale = NULL;
    }
    
    ffread(&klicu,sizeof(klicu),1,f);
    if(klicu) {
      p_sim->p_at = (QUAT *)mmalloc(sizeof(p_sim->p_at[0])*p_sim->keynum);
      ffread(p_sim->p_at,sizeof(p_sim->p_at[0]),p_sim->keynum,f);
    } else {
      p_sim->p_at = NULL;
    }

    if(ch.typ == CHUNK_SIM_NODE_EXT || ch.typ == CHUNK_SIM_NODE_EXT2) {
      ffread(&klicu,sizeof(klicu),1,f);
      if(klicu) {
        p_sim->p_norm = (float *)mmalloc(sizeof(p_sim->p_norm[0])*p_sim->keynum);
        ffread(p_sim->p_norm,sizeof(p_sim->p_norm[0]),p_sim->keynum,f);
      } else {
        p_sim->p_norm = NULL;
      }
    }

    if(ch.typ == CHUNK_SIM_NODE_EXT2) {
      p_sim->p_all_matrix = (GLMATRIX *)mmalloc(sizeof(p_sim->p_all_matrix[0]));
      ffread(p_sim->p_all_matrix,sizeof(p_sim->p_all_matrix[0]),1,f);
    }

    if(p_sim->childnum) {
      p_sim->p_child = (SIMPLE_TRACK_INFO *)mmalloc(sizeof(p_sim->p_child[0])*p_sim->childnum);
      for(i = 0; i < p_sim->childnum; i++)
        lo_chunk_load_sim_node(f, p_sim->p_child+i);
    }
    
    return(TRUE);
  } else
    return(FALSE);
}

int lo_chunk_load_sim_root(FFILE f, OUT_CHUNK *p_ch)
{
  int i;
  if(p_kont && p_ch->typ == CHUNK_SIM_ROOT) {
    if((i = lo_najdi_volny_sim(p_kont->sim, KONT_MAX_ANIM)) == K_CHYBA)
      chyba("Fuck-off");    
    p_sim = p_kont->sim+i;
    memset(p_sim,0,sizeof(p_sim[0]));
    
    ffread(&p_sim->flag,sizeof(p_sim->flag),1,f);
    ffread(&p_sim->keynum,sizeof(p_sim->keynum),1,f);
    ffread(&p_sim->jmeno,sizeof(p_sim->jmeno[0]),MAX_JMENO,f);
    ffread(&p_sim->childnum,sizeof(p_sim->childnum),1,f);
        
    // udelej misto pro decka
    if(p_sim->childnum) {
      p_sim->p_child = (SIMPLE_TRACK_INFO *)malloc(sizeof(p_sim->p_child[0])*p_sim->childnum);
      for(i = 0; i < p_sim->childnum; i++)
        lo_chunk_load_sim_node(f, p_sim->p_child+i);
    }
    
    key_sim_dopln_matrix_kont(p_kont,p_sim);
    
    return(TRUE);
  } else {
    p_sim = NULL;
    return(FALSE);
  }
}


/*
  Preskoc neznamy chunk
*/
int lo_chunk_load_neznamy(FFILE f, OUT_CHUNK *p_ch)
{
  int delka = p_ch->velikost-sizeof(*p_ch);  
  //if(p_ch->typ < 0 && p_ch->typ > CHUNK_KONT_JMENO)
    //kprintfl(TRUE,"-load neznamy chunk typ %d velikost %d",p_ch->typ,p_ch->velikost);
  ffseek(f,delka,SEEK_CUR);  
  return(TRUE);
}


/* Load vseho v kontejneru
*/
typedef int (LPLOADFUNCCHUNK)(FFILE f, OUT_CHUNK *p_ch);

typedef struct _CHUNK_LOAD {
  
  LPLOADFUNCCHUNK *p_fce;
  int              chunk;

} CHUNK_LOAD;


CHUNK_LOAD chload[] = {
  {lo_chunk_load_ambient,            CHUNK_AMBIENT},
  {lo_chunk_load_levelconfig,        CHUNK_LEVEL_KONFIG_EXT},

  {lo_chunk_load_mat,                CHUNK_MATERIAL},
  {lo_chunk_load_color,              CHUNK_COLOR},
  {lo_chunk_load_color_ext,          CHUNK_COLOR_EXT},
  {lo_chunk_load_matanim,            CHUNK_MATANIM_EXT},
  {lo_chunk_load_matanim_frame,      CHUNK_MATANIM_FRAME},
  {lo_chunk_load_matanim_frame_ext,  CHUNK_MATANIM_EFRAME},
  {lo_chunk_load_matanim_text_1,     CHUNK_MATANIM_T1},
  {lo_chunk_load_matanim_text_2,     CHUNK_MATANIM_T2},
  {lo_chunk_load_matanim_text_3,     CHUNK_MATANIM_T3},
  {lo_chunk_load_matanim_text_4,     CHUNK_MATANIM_T4},
  {lo_chunk_load_textmatrix,         CHUNK_MATMATRIX_EXT},
  {lo_chunk_load_stage,              CHUNK_STAG_TAG},
  {lo_chunk_load_text,               CHUNK_TEXT_1},
  {lo_chunk_load_text,               CHUNK_TEXT_2},
  {lo_chunk_load_text,               CHUNK_TEXT_3},
  {lo_chunk_load_text,               CHUNK_TEXT_4},
  {lo_chunk_load_mat_lesk,           CHUNK_LESK},
  {lo_chunk_load_mat_flags,          CHUNK_FLAGS},
  {lo_chunk_load_mat_smat_pos,       CHUNK_MAT_SMAT_POS},
  {lo_chunk_load_mat_env_spec,       CHUNK_ENVSPEC},
  {lo_chunk_load_mat_alfa_stage,     CHUNK_ALFA_STAGE},
  {lo_chunk_load_mat_text_stage_1,   CHUNK_TEXT_STAGE_1},
  {lo_chunk_load_mat_text_stage_2,   CHUNK_TEXT_STAGE_2},
  {lo_chunk_load_mat_text_stage_3,   CHUNK_TEXT_STAGE_3},
  {lo_chunk_load_mat_text_stage_4,   CHUNK_TEXT_STAGE_4},
  {lo_chunk_load_mat_text_stage_advaced, CHUNK_TEXT_STAGE_AD_1},
  {lo_chunk_load_mat_text_stage_advaced, CHUNK_TEXT_STAGE_AD_2},
  {lo_chunk_load_mat_text_stage_advaced, CHUNK_TEXT_STAGE_AD_3},
  {lo_chunk_load_mat_text_stage_advaced, CHUNK_TEXT_STAGE_AD_4},
  {lo_chunk_load_mat_text_flag,      CHUNK_TEXT_FLAG_1},
  {lo_chunk_load_mat_text_flag,      CHUNK_TEXT_FLAG_2},
  {lo_chunk_load_mat_text_flag,      CHUNK_TEXT_FLAG_3},
  {lo_chunk_load_mat_text_flag,      CHUNK_TEXT_FLAG_4},
  {lo_chunk_load_mat_texgen_param,   CHUNK_MAT_TEXGEN},

  {lo_chunk_load_kont_jmeno,         CHUNK_KONT_JMENO},
  {lo_chunk_load_kont_low_jmeno,     CHUNK_KONT_LOW_JMENO},

  {lo_chunk_load_kont_low_id,        CHUNK_KONT_LOW_ID},
  {lo_chunk_load_kont_id,            CHUNK_KONT_ID},
  {lo_chunk_load_kont_flag,          CHUNK_KONT_FLAG},
  {lo_chunk_load_kont_flag_ext,      CHUNK_KONT_FLAG_EXT},
  {lo_chunk_load_kont_flag_ext_2,    CHUNK_KONT_FLAG_EXT_2},
  {lo_chunk_load_kont_wmatrix,       CHUNK_KONT_WMATRIX},
  {lo_chunk_load_kont_mpmatrix,      CHUNK_KONT_MPMATRIX},
  {lo_chunk_load_kont_obb,           CHUNK_KONT_OBB},
  
  {lo_chunk_load_obj_jmeno,          CHUNK_OBJ_JMENO},
  {lo_chunk_load_obj_ID,             CHUNK_OBJ_ID},
  {lo_chunk_load_obj_pivot,          CHUNK_OBJ_PIVOT},
  {lo_chunk_load_obj_material,       CHUNK_OBJ_MATERIAL},
  {lo_chunk_load_obj_vertex,         CHUNK_OBJ_VERTEXY},
  {lo_chunk_load_obj_normal,         CHUNK_OBJ_NORMAL},

  {lo_chunk_load_obj_mapy_1,         CHUNK_OBJ_MAPY_1},
  {lo_chunk_load_obj_mapy_2,         CHUNK_OBJ_MAPY_2},
  {lo_chunk_load_obj_mapy_3,         CHUNK_OBJ_MAPY_3},
  {lo_chunk_load_obj_mapy_4,         CHUNK_OBJ_MAPY_4},
  {lo_chunk_load_obj_face,           CHUNK_OBJ_FACY},
  {lo_chunk_load_obj_face_ext,       CHUNK_OBJ_FACY_OPT},

  {lo_chunk_load_obj_barvy,          CHUNK_OBJ_BARVY},
  {lo_chunk_load_obj_barvy_ext,      CHUNK_OBJ_BARVY_EXT},
  {lo_chunk_load_obj_matrix,         CHUNK_OBJ_MATRIX_KEY},
  {lo_chunk_load_obj_obb,            CHUNK_OBJ_OBB},

  {lo_chunk_load_sim_root,           CHUNK_SIM_ROOT},

  {lo_chunk_load_static_light,       CHUNK_STATIC_LIGHT},
  {lo_chunk_load_static_light_mesh,  CHUNK_STATIC_LIGHT_MESH},
  {lo_chunk_load_dyn_light,          CHUNK_DYNAMIC_LIGHT},
  {lo_chunk_load_dyn_light_mesh,     CHUNK_DYNAMIC_LIGHT_MESH},
  {lo_chunk_load_extra_light,        CHUNK_EXTRA_LIGHT},
  {lo_chunk_load_extra_light_mesh,   CHUNK_EXTRA_LIGHT_MESH},
  {lo_chunk_load_zrcadlo_3,          CHUNK_ZRCADLO_3},
  {lo_chunk_load_zrcadlo_3_sub,      CHUNK_ZRCADLO_3_POLOZKA},

  {lo_chunk_load_flare,              CHUNK_FLARE},
  {lo_chunk_load_flare_mesh,         CHUNK_FLARE_MESH},
  {lo_chunk_load_flare_slozeny,      CHUNK_FLARE_SLOZENY},
  {lo_chunk_load_flare_light,        CHUNK_FLARE_LIGHT},

  {lo_chunk_load_flare_ext,          CHUNK_FLARE_EXT},
  {lo_chunk_load_flare_mesh_ext,     CHUNK_FLARE_MESH_EXT},
  {lo_chunk_load_flare_light_ext,    CHUNK_FLARE_LIGHT_EXT},

  {lo_chunk_load_mlho_kostek,        CHUNK_MLHOKOSTKA},
  {lo_chunk_load_mlho_kostek_2,      CHUNK_MLHOKOSTKA_2},

  {lo_chunk_load_environment,        CHUNK_ENVIRONMENT},
  {lo_chunk_load_kamset,             CHUNK_KAMSET},
  {lo_chunk_load_kamset_2,           CHUNK_KAMSET_2},

  {lo_chunk_load_obj_joint_animace,  CHUNK_OBJ_JA},
  {lo_chunk_load_obj_joint_ID,       CHUNK_OBJ_JNT_ID},
  {lo_chunk_load_obj_joint_framy,    CHUNK_OBJ_JA_FRAMU},
  {lo_chunk_load_obj_joint_pivot,    CHUNK_OBJ_JNT_PIVOT},
  {lo_chunk_load_obj_joint_flag,     CHUNK_OBJ_JNT_FLAG},
  {lo_chunk_load_obj_joint_vertexy,  CHUNK_OBJ_JNT_VERT},
  {lo_chunk_load_obj_joint_pos,      CHUNK_OBJ_JNT_POS},
  {lo_chunk_load_obj_joint_rot,      CHUNK_OBJ_JNT_ROT},
  {lo_chunk_load_obj_joint_scs,      CHUNK_OBJ_JNT_SCS},
  {lo_chunk_load_obj_joint_pos_spline,CHUNK_OBJ_JNT_KEYPOS},
  {lo_chunk_load_obj_joint_rot_spline,CHUNK_OBJ_JNT_KEYROT},
  {lo_chunk_load_obj_joint_scs_spline,CHUNK_OBJ_JNT_KEYSCS}
};


int lo_load_chunky(FFILE f)
{
  OUT_CHUNK ch;
  int load, i, j = 0, ret;
  
  while((ret = ffread(&ch,sizeof(ch),1,f)) && ret > 0) {
    load = 0;

    for(i = 0; i < (sizeof(chload)/sizeof(chload[0])); i++) {
      if(chload[i].chunk == ch.typ) {        
        load = chload[i].p_fce(f,&ch);
        break;
      }
    }
    
    if(!load) {
      lo_chunk_load_neznamy(f, &ch);
    }

    j++;
  }

  if(ret < 0) {
    if(ret == -1) {
      //kprintf(TRUE,"Chyba loadu projektu '%s'",strerror(errno));
    }    
  }

  return(j);
}

/* Nahraje vsechny materialy + prvni kontejner
*/
EDIT_KONTEJNER * lo_nahraj_kontejner_chunk(EDIT_MATERIAL **p_mat, int max_mat,
                 EDIT_TEXT *p_text, int max_text, char *p_jmeno, int mat, int normal)                                  
{
  FFILE f;

  if((f = ffopen(p_jmeno,"rb")) == NULL) {
    //kprintf(1,"Fail loading kont %s error '%s'",p_jmeno,strerror(errno));
    return(NULL);
  }

  p_ambient = NULL;

  p_matlist = p_mat;
  matlistnum = max_mat;
  p_textlist = p_text;
  textlistnum = max_text;
  
  p_lightlist = NULL;
  lightlistnum = 0;
  p_zrcadlolist = NULL;
  p_zrcadloload = NULL;
  p_flarelist = NULL;
  p_flare = NULL;
  p_top_mlha = NULL;
  flarelistnum = 0;

  kont_load = 0;
  kontlistnum = 0;
  p_kontlist = NULL;

  p_kont_old = p_kont = vyrob_kontejner();
  p_mat = NULL;
  p_obj = NULL;

  p_dlist = NULL;
  dlistnum = 0;

  p_elist = NULL;
  elistnum = 0;

  p_lenv = NULL;
  p_kamset = NULL;

  p_light_mesh = NULL;
  
  p_joint = NULL;
  p_joint_animace = NULL;

  lo_load_chunky(f);
  ffclose(f);
  
  p_kont->k2flag &= ~KONT2_LOADED;  
  p_kont = p_kont_old;  
      
  lo_chunk_stage_load_korekce(p_matlist, max_mat);

  p_kont->k2flag&=~KONT2_LOADED;
  updatuj_kontejner_flag(p_kont,p_matlist);
  updatuj_kontejner_statistika(p_kont,FALSE);
  key_kontejner_sim_reanimuj(p_kont);
  if(normal)
    kont_norm_vect(p_kont);
  key_kosti_reanimuj_kont(p_kont);

  return(p_kont);
}

/* Nahraje vsechny veci z .prj
*/
int lo_nahraj_projekt(EDIT_MATERIAL **p_mat, int max_mat,
                      EDIT_KONTEJNER **p_kontls, int kontnum,
                      EDIT_TEXT_OLD *p_text, int max_text, char *p_jmeno,
                      STATIC_LIGHT *p_light, int lightnum,
                      ZDRCADLO_DESC *p_zrcadlo, int *p_zrcadlo_aktivni,
                      LENS_FLARE *p_flarr, int flaremax,
                      dword *p_amb, MLZNA_KOSTKA **p_mlha, 
                      DYN_LIGHT *p_ddlight, int ddlightnum,
                      EXTRA_DYN_LIGHT *p_eelight, int eelightnum,
                      LEVEL_ENVIRONMENT *p_env,                 
                      NASTAVENI_KAMER *p_kam,
                      LEVEL_KONFIG *p_lev_konfig,
                      int flag, int normal)
{
  FFILE f;
  int   i;

  if((f = ffopen(p_jmeno,"rb")) == NULL) {
    //kprintf(1,"Fail loading kont %s error '%s'",p_jmeno,strerror(errno));    
    return(FALSE);
  }

  p_kontlist  = (flag&LOAD_GEOM) ? p_kontls : NULL;
  kontlistnum = (flag&LOAD_GEOM) ? kontnum  : 0;
   
  p_matlist   = (flag&LOAD_MATERIALY) ? p_mat : NULL;
  matlistnum  = (flag&LOAD_MATERIALY) ? max_mat : 0;
    
  p_textlist  = (flag&LOAD_TEXTURY) ? p_text : NULL;
  textlistnum = (flag&LOAD_TEXTURY) ? max_text : 0;

  p_ambient   = (flag&LOAD_AMBIENT) ? p_amb : NULL;

  p_zrcadlolist = (flag&LOAD_ZRCADLO) ? p_zrcadlo : NULL;
  p_zrcadloload = (flag&LOAD_ZRCADLO) ? p_zrcadlo_aktivni : NULL;

  p_lightlist  = (flag&LOAD_STAT_LIGHT) ? p_light : NULL;
  lightlistnum = (flag&LOAD_STAT_LIGHT) ? lightnum : 0;

  p_dlist  = (flag&LOAD_DYN_LIGHT) ? p_ddlight : NULL;
  dlistnum = (flag&LOAD_DYN_LIGHT) ? ddlightnum : 0;

  p_elist  = (flag&LOAD_EXTRA_DYN_LIGHT) ? p_eelight : NULL;
  elistnum = (flag&LOAD_EXTRA_DYN_LIGHT) ? eelightnum : 0;

  p_top_mlha   = (flag&LOAD_MLHA) ? *p_mlha : NULL;

  p_flarelist  = (flag&LOAD_FLARE) ? p_flarr : NULL;
  flarelistnum = (flag&LOAD_FLARE) ? flaremax : 0;

  p_flare = NULL;
  
  p_lenv = (flag&LOAD_LEVEL_ENV) ? p_env : NULL;
  p_lc   = (flag&LOAD_LEVEL_CONF) ? p_lev_konfig : NULL;
  p_kamset = (flag&LOAD_KAMERY) ? p_kam : NULL;
  
  p_kont = NULL;
  p_mat = NULL;
  p_obj = NULL;

  p_light_mesh = NULL;

  p_joint = NULL;
  p_joint_animace = NULL;

  lo_load_chunky(f);
  ffclose(f);
  
  lo_chunk_stage_load_korekce(p_matlist, max_mat);

  for(i = 0; i < kontnum; i++) {
    if(p_kontls[i] && p_kontls[i]->k2flag&KONT2_LOADED) {
      p_kontls[i]->k2flag&=~KONT2_LOADED;
      updatuj_kontejner_flag(p_kontls[i],p_matlist);
      updatuj_kontejner_statistika(p_kontls[i],FALSE);
      lo_preved_flare_do_sceny(p_kontls[i]->p_slight, p_flarr, flaremax);
      lo_preved_svetla_do_sceny(p_kontls[i],p_light,lightnum,p_ddlight,ddlightnum,p_eelight,eelightnum);
      key_kontejner_sim_reanimuj(p_kontls[i]);
      if(normal)
        kont_norm_vect(p_kontls[i]);
      key_kosti_reanimuj_kont(p_kontls[i]);
    }
  }
  
 *p_mlha = p_top_mlha;

  return(TRUE);
}


int lo_nahraj_materialy_chunk(EDIT_MATERIAL **p_mat, int max_mat, EDIT_TEXT *p_text, 
                              int max_text, char *p_file, int save)
{
  FFILE f;

  p_matlist = p_mat;
  matlistnum = max_mat;
  p_textlist = p_text;
  textlistnum = max_text;
  
  p_lightlist = NULL;
  lightlistnum = 0;
  p_zrcadlolist = NULL;
  p_zrcadloload = NULL;
  p_flarelist = NULL;
  p_flare = NULL;
  p_top_mlha = NULL;
  flarelistnum = 0;
  kontlistnum = 0;
  kont_load = 0;
  p_kontlist = NULL;
  p_kont = NULL;
  p_mat = NULL;
  p_obj = NULL;
  p_dlist = NULL;
  dlistnum = 0;
  p_elist = NULL;
  elistnum = 0;
  p_lenv = NULL;
  p_kamset = NULL;
  p_light_mesh = NULL;
  p_joint = NULL;
  p_joint_animace = NULL;

  if((f = ffopen(p_file,"rb")) == NULL) {
    //kprintf(1,"Fail loading material %s error '%s'",p_file,strerror(errno));
    return(FALSE);
  }
  lo_load_chunky(f);
  ffclose(f);  
  
  lo_chunk_stage_load_korekce(p_matlist, max_mat);

  return(TRUE);
}

void lo_chunk_stage_load_korekce(EDIT_MATERIAL **p_mat, int num)
{
  MATERIAL_TEXT *p_stg; 
  int textur,i,j,s;

  for(j = 0; j < num; j++) {
    if(p_mat[j]) {
      
      /* korekce pruhlednosti
      */
      if(alfa_stage_edit_blok[p_mat[j]->alfa_state].alfa_pruhledny) {
        p_mat[j]->flag |= MAT_PRUHLEDNY;
      }
      else
        p_mat[j]->flag&=~MAT_PRUHLEDNY;

      /* Maskovany material
      */
      if(p_mat[j]->alfa_state == STAGE_ALFA_MASKA) {
        p_mat[j]->flag2 |=  MAT2_MASKA;
      } else {
        p_mat[j]->flag2 &= ~MAT2_MASKA;
      }

      /* korekce stagu
      */    
      for(i = 0; i < MAT_TEXTUR; i++) {
        if(!p_mat[j]->textfile[i][0]) {
          p_mat[j]->text_state[i].text_stage = K_CHYBA;
        }
      }
    
      /* Prepocitani textur
      */    
      p_stg = p_mat[j]->text_state;    
      for(i = 0, textur = 0; i < MAT_TEXTUR; i++) {
        if((s = p_stg->text_stage) != K_CHYBA) {
          textur += text_stage_edit_blok[s].delka;
        }
        p_stg++;
      }
    
      p_mat[j]->textur = textur;
    }
  }
}
