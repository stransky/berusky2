/*
  H k objekt editoru
*/

#define  POPIS_OS_ON     1
#define  POPIS_OS_OFF    0
#define  MENIT_VZDAL_ON  1
#define  MENIT_VZDAL_OFF 0
#define  STRED_ON        1
#define  STRED_OFF       0

#define  PODLOZKA_KONT     99
#define  KAMERA_1          95
#define  KAMERA_2          96
#define  KAMERA_3          97
#define  KAMERA_4          98
#define  R_BOD             94
#define  R_BOD_1           93
#define  REZERVOVANE_KONT  10

#define  MAX_SVETEL              200
#define  MAX_PRUHLEDNYCH_OBJEKTU 200

#define  CELKEM_MODU 5
#define  MOD_XZ      0
#define  MOD_XY      1
#define  MOD_ZY      2
#define  MOD_GAME    3
#define  MOD_KAMERA  4

#define  KONT_NUM    2000

#define  VEL_CTVERCE 4

#define MAX_UNDO 5

#define NZ_STROM    128 // 1 x 1 kilometr
#define NX_STROM    128

#define DX_STROM    10 //x a z posun
#define DZ_STROM    10

#define X_STROM     0
#define Z_STROM     0

#define NZ_STROM_POSUN 7

#define OXRES       p_cnf->wxres
#define OYRES       p_cnf->wyres
#define OXSTART     p_cnf->wx
#define OYSTART     p_cnf->wy

#define MAX_EDIT_TEXTUR    1000
#define MAX_EDIT_ANIMACI   1000
#define MAX_RAY_TEXTUR     1000
#define MAX_EDIT_SVETEL    100
#define MAX_EDIT_KAMER     20
#define MAX_EDIT_VLN       20

#define SHAD_FLAT          0
#define SHAD_GOURAND       1
#define SHAD_PHONG         2

#define DUMMY_OBJEKT       "$$$DUMMY"

#define SCENA_LEVEL        0
#define SCENA_POLY         1

/*
  Bud je to bod kontejneru nebo divoky bod v prostoru na x,y,z
*/
typedef struct _I_VERTEX {
  
  word v;
  word o;
  word k;

} I_VERTEX;

typedef struct _F_VERTEX {
  
  word f; // vybrana face
  word o; // vybrane objekt
  word k; // vybrany kontejner

} F_VERTEX;

typedef struct _D_VERTEX {
  
  word  k,o,f;
  float x,y,z;
  byte  pozice;
  byte  d2; //true pokud je to 2d bod

} D_VERTEX;

typedef struct _I_OBJEKT {
  
  word o;
  word k;

} I_OBJEKT;

/* Texturova cast - texturovaci nastroje
*/
typedef struct _ROH {

  int x1,y1,x2,y2;

} ROH;

typedef struct _ROHF {

  float x1,y1,x2,y2;

} ROHF;


/*
    D3DFOG_NONE   = 0, 
    D3DFOG_EXP    = 1, 
    D3DFOG_EXP2   = 2, 
    D3DFOG_LINEAR = 3 
*/
typedef struct _EDIT_OKNO {

 GLMATRIX project,
           word,
           camera,
           invcam, // inverzni viev matrix s normalizovanejma vektorama
           r_bod; 
 float     vzdal;
 float     fi;

 byte      fs; // full-scrren -> po jeho deaktivaci prekresli zbyvajici okna
 
 int       min_x,
           min_y;
 int       max_x,
           max_y;

} EDIT_OKNO;

typedef struct _KONFIG_MATERIALU {

 int   aktivni;  // texturovaci lista aktivni
 int   vybr_polozka; // vybrana plozka v liste
 int   vybr_mat; // vybrany material
 HWND  hwnd_list; // hwnd listu materialu
 HWND  hwnd_dialog; // hwnd listu materialu

} KONFIG_MATERIALU;

typedef struct _KONFIG_OBJEKTU {

 int   aktivni;  // obj lista aktivni
 int   vybr_polozka; // vybrana plozka v liste  
 HWND  hwnd_dialog; // hwnd listu materialu

} KONFIG_OBJEKTU;

typedef struct _KONFIG_ANIMACE {

 int   aktivni;      // ani lista aktivni
 HWND  hwnd_dialog;  // hwnd listu animaci
 int   par1;

} KONFIG_ANIMACE;

#define OZNACOVAT_MESH   0x1
#define OZNACOVAT_KOSTI  0x2
#define OZNACOVAT_SVETLA 0x4

typedef struct _KONFIG_EDITOR {

 byte    vsechno_obj;
 byte    vsechno_kont;
 byte    grid;

 byte    dratove;
 byte    textury;
 byte    objekt_origin;
 byte    objekt_stred;

 byte    high_face_render;
 byte    high_face_render_refresh;

 byte    mys_on;
 byte    nitkovy_kriz;
   
 byte    render_mod;
 byte    free_look;
 
 byte    svetla;
 byte    svetla_kreslit;
 byte    svetla_dynamicke;
 byte    svetla_bez_gourandu;
 byte    svetla_sceny;
 byte    max_svetla;

 byte    flare;
 byte    animace;

 byte    okoli;
 byte    light_maps;

 byte    load_textur;
 byte    load_prvku;

 byte    keyframe_load;
 byte    keyframe_static;
 byte    keyframe_kont;

 byte    druh_sceny;
 
 byte    kdtree_bunka;
 byte    kdtree_render;
 byte    kdtree_hloubka;
 byte    kdtree_render_bunky;
 byte    kdtree_kreslit_vse;

 byte    kamera_load;
 byte    kamera_kreslit;
 byte    kamera_aktivni;
 byte    kamera_vkladat_do_klicu;

 byte    mazat_dummy;

 byte    obalky_kontejner;
 byte    obalky_objekt;

 byte    barvy_z_materialu; 

 byte    render_zrcadla;
 byte    norm_vect;
 float   norm_vect_velikost;

 byte    mlhokostka_aktivni;   // aktivni mlhokostka
 byte    mlhokostku_kreslit;   // kreslit mlhokostky
 byte    mlhokostka_testy;     // testy na kostky

 float   slicka_delta;
 int     posledni_prikaz;

 int     kamera_kostka_kreslit;
 
 int     undo;
 int     undo_num;

 byte    aktivni_pozadi; // aktivni pozadi pro level

 int     delit_materialy;
 int     oznacovat;

 int     load;
 int     precizni_export_obb;

 int     editor_priorita;
 int     mazat_materialy;

 int     render;

} KONFIG_EDITOR;


/*
   mody: (X/Z) = 0 - shora
         (X/Y) = 1 - zepredu
         (Y/Z) = 2 - zleva (z boku)
        (GAME) = 3 - herni - sikmo zhora
        (USER) = 4...5 - dalsi
*/
typedef struct _STATISTIKA_SCENY {

  int kontejneru;
  int objektu;
  int facu;
  int polyface;
  int bodu;
  int textur;
  int materialu;
  int svetel;
  int ray_bodu;
  int kont_stat;
  int kont_dyn;

} STATISTIKA_SCENY;

typedef struct _BARVY_BODU {

  // == 0 - diffusni, 1 == spekularni
  HWND    hDlg; // handle dialogu == 0 nic
  byte    mod;
  
  float   gr,gg,gb,ga;
  byte    gourand_cop;
  float   gourand_scale;
  float   gourand_pridat1;
  float   gourand_pridat2;

  int     multi_mesh;
  int     oboustrany_stinovac;
  int     set_operace;
  float   set_krok;
  int     lo,hi;

} BARVY_BODU;

typedef struct _POLY_LIST {

 struct _POLY_LIST *p_next; // dalsi poly v seznamu
 int                poly;   // cislo poly

} POLY_LIST;

typedef struct _EDIT_STINY {
 
 POLY_LIST *p_poly[MAX_EDIT_SVETEL];
 // seznam objektu ktere stini poly v tomto svetlu

} EDIT_STINY;

typedef void (* P_FILRT_FUNC)(bitmapa *p_bmp);

typedef struct _FILTR_FUNC {
  char jmeno[50];
  P_FILRT_FUNC p_fnc;
} FILTR_FUNC; 

#define FILTR_FUNC_NUM 6

typedef struct _RAY_TRACER {

  dword ambient; // ambientni osvetleni sceny
  float faktor;  // scale faktor textur
  byte  disk;    // render na disk ?
  byte  disk_mezi;
  byte  stupen_filtrace; // stupen filtrace textur
  byte  filtrace;
  byte  stiny;   // pocitat stiny

  int   okraj;
  int   text_min_x;
  int   text_min_y;
  
  int   max_x_surface; // max x rozmer textur
  int   max_y_surface; // max y rozmer textur

  int   max_x_lepic; // max x rozmer textur
  int   max_y_lepic; // max y rozmer textur

  byte  stinovani;   // 0 - flat 1 - gourand 2 - phong

  byte  supersampl;
  byte  sup_hloubka;
  byte  sup_velikost_x;
  byte  sup_velikost_y;

  FILTR_FUNC  filtr_func[FILTR_FUNC_NUM];
  FILTR_FUNC *p_akt_filtr_func;

  int   lightmap_filtr;

} RAY_TRACER;

typedef struct _LIGHTMAP_INFO {
  
  BOD normal;       // face normal
  BOD normal_bod;   // normala pocitaneho bodu
  int k,o,f;        // umisteni light-mapy  
  int dx,dy;        // rozmery light-mapy
  int material;
  int mail;         // mail-box
  int poly;
  int indicie;
  int ray_phit;     // naposledy protnuty poly
  int ray_khit;     // naposledy protnuty kontejner
  int umistena;     // lightmapa je uz umistena
  int vface[3];     // seznam facu co se ma vyhodit

} LIGHTMAP_INFO;

typedef struct _QUAD_FACE {

  void *p_item;   // polozka v stromu
  int   kont;     // cislo kontejneru (je to pointer na kontejner)
  int   poly;     // cislo poly (je to poly)
  int   face;     // cislo plosky (je to face)
  
//EDIT_MESH_POLY *p_poly; // poly ktereho se to tyka
//TEXT_KOORD     *p_koord;// koordinaty plosky 

  struct _QUAD_FACE *p_dalsi; // dalsi plosky

} QUAD_FACE;
                   
typedef struct _QUAD_TREE {

  QBOD       rad;   // rozmery stromu
  BOD        st;    // start stromu
  BOD        bunka; // velikost bunek stromu
  QUAD_FACE  ****p_face;

} QUAD_TREE;

typedef struct _DEKOVAC {
  
  int     aktivni;

  float   radius; // kam dekovac dosahne
  float   sila;   // sila dekovace na vzdalenost

} DEKOVAC;


#define  ELIM_STRED           0x01 // Eliminovat podle stredu
#define  ELIM_MAZAT           0x02 // Premazavat oznaceni
#define  ELIM_NORMAL          0x04 // Eliminator podle normaly
#define  ELIM_INTELIGENT_1    0x10 // eliminator podle sousednosti
#define  ELIM_INTELIGENT_2    0x20 // eliminator podle polohy bodu
#define  ELIM_INTELIGENT_2_BRUTAL 0x40 // eliminator podle polohy bodu
#define  ELIM_STATIC          0x80 // eliminator podle sousednosti
#define  ELIM_DYNAMIC        0x100 // eliminator podle sousednosti
#define  ELIM_PRVKY          0x200 // eliminuji se herni prvky
#define  ELIM_OKOLI          0x400 // eliminuje se okoli

typedef struct _ELIMINATOR {
  
  int   flag;
  BOD   p;
  BOD   norm;
  float uhel;
  int   spol_bodu_1;
  int   spol_bodu_2;

  int   dlg_aktivni;
  HWND  hwnd_dialog; // hwnd listu materialu

  float delta_vzdalenost;

} ELIMINATOR;

typedef struct _LISTA_PRVKU {

  int  aktivni;
  int  prvnum; // pocet prvku
  int *p_prv;  // seznam cisel prvku dle databaze
  int *p_pos;  // pozice v listboxu
  int  prvni;  // prvni prvek seznamu
  int  prvakt; // vybrany prvek v seznamu (okne) - vybranu = prvni+akt
  HWND hDlg;   // list-box kam nastavovat pozici (NULL pokud nic)
  HWND hDlg_box;// dialog-box handle
  int  control; // kontrol-dialog listu
  int  oken;   // oken na horni liste

} LISTA_PRVKU;

#define  GROUP_KONT          0x01
#define  GROUP_OBJ           0x02
#define  GROUP_FACE          0x04

typedef struct _LIGHT_CONFIG {

  HWND  hDlg;          // handle na listu/0 pokud nic
  int   extra_svetlo;  // akt svetla jsou extra/obycejna
  int   scena_light;   // svetla jsou sceny/kontejneru

} LIGHT_CONFIG;


typedef struct _EDIT_JOINT {

  int            k,o;                 // vybrany objekt
  JOINT         *p_joint_akt;         // aktualni joint
  JOINT_ANIMACE *p_joint_animace_akt; // aktualni joint-animace
  
  // dialog-box flagy
  int            ak;
  HWND           hDlg;

  JOINT_ANIMACE *p_play; // prehravane joint-animace

} EDIT_JOINT;

typedef struct _JOINT_ANIM_GENERATOR {

  byte  jmeno[MAX_JMENO];

  int   framu_min;
  int   framu_max;

  float scale_0;
  float scale_1;

  float udr_sila;    // pomer nd-rady
  float udr_rozsah;    // rosah nd-rady
  int   udr_frame_min;
  int   udr_frame_max;
  
  float ud_rozsah; // rozsah generatoru
  int   ud_frame_min; // min vzdalenost framu
  int   ud_frame_max; // mix vzdalenost framu

  float vtr_sila;    // pomer nd-rady
  float vtr_rozsah;    // rosah nd-rady
  int   vtr_frame_min;
  int   vtr_frame_max;

  float vt_rozsah; // rozsah generatoru vetru
  int   vt_frame_min; // min vzdalenost framu
  int   vt_frame_max; // mix vzdalenost framu
  BOD   vt_smer;      // smer vetru
  
  JOINT_ANIMACE  *p_anim;

  BOD   osa;
  float znaminko;

  BOD   osa_vitr;
  float sila_vetru;

  int   framu;
  int   anim_vse;
  
  int   vtr_keys;
  KEY_POINT_BRS *p_vkeys;  // Spline values for scaling
  float *p_vtr;
  float *p_vtr_all;

  int   udr_keys;
  KEY_POINT_BRS *p_ukeys;  // Spline values for scaling
  float *p_udr;
  float *p_udr_all;

} JOINT_ANIM_GENERATOR;

// bunka pro generovani flare
// generuje se z vybranych vertexu
typedef struct _FLARE_GEN {

  STATIC_LIGHT   *p_slight; // svetlo ktere to haze
  BOD             p;        // gen-bod
  
  float           start;
  dword           start_color;
  
  float           konec;
  dword           konec_color;

} FLARE_GEN;

// Decimator plosek

#define HFLAG_HRANA 0x1

// Popis jedne hrany
typedef struct _HRAN {

  int        ind1,ind2;
  GLMATRIX   q;
  OBJ_VERTEX bod;
  float      err;
  float      err_uhel;
  int        flag;
  BOD        face_normal;

} HRAN;

#define HRN_SEGMENT 1000

typedef struct _OBJ_LIST {

  int  k;
  int  o;

  EDIT_OBJEKT *p_obj;

} OBJ_LIST;

typedef struct _HRAN_STACK {

  int        max_err;
  float      max_err_flt;

  int        vaha;
  float      vaha_flt;

  int        hrany;
  float      hrany_flt;

  int        prah;
  float      prah_flt;

  float      stupen;
  int        vertexu;
  int        vertexy;

  HWND       hDlg;

  HRAN      *p_list;
  int        listnum;
  int        listakt;  

  GLMATRIX  *p_mat;      // seznam quadrik

  int        zalnum;
  int        zalakt;
  OBJ_LIST  *p_zal;

  int        flag;
  int       *p_vmask;

} HRAN_STACK;


typedef struct _K_EDITOR {

 byte    jmeno_sceny[100];

  
 GLMATRIX *p_project,
          *p_word,
          *p_camera,
          *p_invcam, // inverzni viev matrix s normalizovanejma vektorama
          *p_r_bod;
 
 float    *p_vzdal;
 float    *p_fi;

 int       wx,wy,wxres,wyres;

 EDIT_OKNO viewports[CELKEM_MODU];
 int     mod; // aktivni okno

 UINT    vybr_prikaz;
 UINT    fprikaz;

 BOD     primka[2];

 float   skok_x, skok_fi, skok_vzdal;

 NASTAVENI_KAMER  kam;
 KONFIG_MATERIALU km;
 KONFIG_OBJEKTU   kk;
 KONFIG_EDITOR    cnf;
 KONFIG_DIR       dir;
 TEXT_DIR         tdir;
 STATISTIKA_SCENY stat;
 BARVY_BODU       bar;
 RAY_TRACER       ray;
 KONFIG_ANIMACE   ak;
 KONFIG_ANIMACE   akkam;
 KONFIG_ANIMACE   akmat;
 DEKOVAC          deka;     // dekovac
 ELIMINATOR       elm;      // eliminator plosek
 LISTA_PRVKU      lp;
 LIGHT_CONFIG     lc; 
 EDIT_JOINT       jcf;      // konfigurace jointu
 FLARE_GEN        fgen;     // flare-generator
 HRAN_STACK       ds;       // decimacni stack
 ZEMNI_MLHA       zm;       // zemni mlha
 JOINT_ANIM_GENERATOR jgen; // joint-generator

 B_KONFIG         bnf;      // berusci konfig
 byte             kreslit_level; // ma se kreslit level ?
 byte             kreslit_scenu; // ma se kreslit scena ?
 
 dword   svetlost_sceny; //RGBA ambientniho svetla
 dword   barva_pozadi;

 void    *p_test;

 int     cul;

 RGB_M   barva_grid;
 RGB_M   barva_vybr_kont;
 RGB_M   barva_vybr_objekt;
 RGB_M   barva_vybr_face;
 RGB_M   barva_neaktivni;

 int     TimeBase;        // cas spusteni programu -> casova zakladna (v milisec)
 int     TimeEndLastFrame;  // cas na konci posledniho framu (v milisec)
 int     TimeLastFrame;     // delka posledniho framu (obracena hodnota je fps)
 //int     TimePredpoklad;    // predpokladana hodnota na konci dalsiho framu
 float   fps;               // fps = 1000/TimeLongLastFrame

 int     drobic_hloubka;

 int     lep_obj_1;
 int     lep_obj_2;

 int     prvni_kont;
 int     d_listu_kont;

 int     prvni_objekt;
 int     vybr_objekt;
 int     vybr_objekt_last;
 int     d_listu;

 int     vybr_kont;
 int     vybr_kont_last;
 int     vybr_face;
 int     vybr_text;
 dword   blend_faktor; 

 byte    ctverec;
 int     x1,y1,x2,y2;
 float   ctverec_barva[3];

 byte    linelist;
 dword   linelist_barva;
 byte    linelist_num;
 ROHF    linelist_cara[20];
 
 // staticka svetla pro metodu ray-castingu
 int     lightnum;
 int     lightakt;
 STATIC_LIGHT light[MAX_EDIT_SVETEL];

 // flare placky
 int        flareakt;
 LENS_FLARE flare[MAX_EDIT_SVETEL];

 // Dynamicke svetla
 int             dl_recalc;     // flag - prepocitat svetla (neco se posunulo)  
 int             dl_new;        // flag - zmenil se pocet svetel (pribylo/ubylo)
 int             dl_lightnum;
 int             dl_on;         // zapnute prepocty statickych svetel
 int             dl_akt;        // vybrane svetlo
 DYN_LIGHT       p_dlight[MAX_FLARE_SVETEL];
 
 // Extra dynamicke svetla
 int             edl_akt;
 int             edl_recalc;     // flag - prepocitat svetla (neco se posunulo)  
 int             edl_new;        // flag - zmenil se pocet svetel (pribylo/ubylo)
 int             edl_lightnum;  
 EXTRA_DYN_LIGHT p_edlight[MAX_FLARE_SVETEL];
 
 // Kamery
 int     kamnum;  // pocet kamer sceny
 int     kamakt;  // aktivni kamera sceny 
 KAMERA  kamery[MAX_EDIT_KAMER];

 /* Enviromentalni prostredi pro level
 */
 LEVEL_ENVIRONMENT env;

 /* Oplzle Mlzne kostky
 */
 MLZNA_KOSTKA *p_mlha_akt;
 MLZNA_KOSTKA *p_mlha;

 // zrcadla ve scene
 int           zdcadlo_akt;
 ZDRCADLO_DESC zrcadlo;

 COLORREF user_color[16];

 int             group_param;
 int             vybr_group;
 int             groupnum;
 I_VERTEX       *p_group;

 int             groupflag;                    // group flag
 int             vybr_face_group;
 int             groupfacenum;
 F_VERTEX       *p_group_face;

 int             vybr_dvert;
 int             dvertnum;
 D_VERTEX       *p_dvert;

 int             max_kont;
 int             kontejneru; 
 EDIT_KONTEJNER *p_kont[KONT_NUM];             // kontejnery sceny
 int             kontraynum;
 EDIT_KONTEJNER *p_kont_raytracer[KONT_NUM];   // staticke kontejnery sceny
 EDIT_MATERIAL  *p_mat[MAX_EDIT_MATERIALU];    // materialy
 EDIT_TEXT       p_text[MAX_EDIT_TEXTUR];      // textury objektu
 
 int             polynum; // pocet poly
 EDIT_MESH_POLY *p_poly;  // seznam poly
 EDIT_STINY     *p_stiny; // seznam stinicich poly
 EDIT_TEXT       p_lightmap[MAX_RAY_TEXTUR];  // staticke light-mapy 
 KD_BUNKA        kdtree;  // kd strom sceny
 
 EDIT_MATERIAL **p_smat;  // material sceny
 
 // keyframe data
 float           key_time;

 // Predpocitane data
 float           sinus_table[360];
 GLMATRIX        init_matrix;

 // Modifikator matic
 int             mod_matrix_akt;// modifikacni matice aktivni ?
 GLMATRIX        mod_matrix;    // modifikacni matice ke slozeni 

 byte            berusky_hra[200];     // cesta ke hre
 byte            berusky_hra_dir[200]; // cesta ke hre 

  // look-up tabulka pro druhou odmocninu
  int            p_looksqrt_linear[MAX_LOOK_TABLE*MAX_LOOK_TABLE];
  int            p_looksqrt_quadratic[MAX_LOOK_TABLE*MAX_LOOK_TABLE];
  
  // Jednorozmerne look-up tabulky
  float          p_looksqrt_linear_float[MAX_LOOK_TABLE];
  float          p_looksqrt_quadratic_float[MAX_LOOK_TABLE];

  GLMATRIX       tmp_matrix;

} K_EDITOR;

extern K_EDITOR  cnf, *p_cnf;

int     XK;
int     YK;
#define KONZOLE_RADKU 5

typedef struct _GAMESLOT {
  
  byte      jmeno[30];
  int       vzor;
  GLMATRIX  word;
  byte      rezerved[10];

} GAMESLOT;

typedef struct _SCENE_STATISTIC {

  int   textsize; // velikost textur v byte
  int   texnum;   // pocet textur ve scene

  int   lightsize; // velikost lightmap
  int   lightnum;  // pocet lightmap

  int   matnum;   //  pocet materialu
  int   matsize;  //  velikost materialu (B)

  int   kontnum;  // pocet kontejneru
  int   objnum;   // pocet objektu
  int   kontsize; // velikost kontejneru (B)

  int   lev_kontnum;  // pocet kontejneru
  int   lev_objnum;   // pocet objektu
  int   lev_kontsize; // velikost kontejneru (B)

  int   polynum;      // pocet poly
  int   polyface;     // pocet facu v poly
  
  int   vidmemuse; // pouzito videopameti (primary surf + back buffers + z-buffer)
  int   totmem;
  int   freemem;

  int   facu;
  int   bodu;

} SCENE_STATISTIC;


#define oe_smaz_malou_konzoli(p_con)           \
        smaz_surf_rec(p_con,akt_konzole_pozadi,\
                      0,YK*VYSKA_FONTU,XRES-1,YRES-1);

#define prikaz(prikaz)\
{\
  system_kurzor = 1;\
  prikaz;\
  system_kurzor = 0;\
}\


#define n_objekt(kont,obj)        { if(!p_cnf->p_kont[kont] || !p_cnf->p_kont[kont]->p_obj[obj]) { ddw("Spatny objekt file %s line %d",__FILE__,__LINE__); return; }}
#define n_kont(kont)              { if(!p_cnf->p_kont[kont]) { ddw("Spatny kontejner -> file %s line %d",__FILE__,__LINE__); return;} }
#define n_kont_ret(kont)          { if(!p_cnf->p_kont[kont]) { return;} }
#define n_objekt_mrizka(kont,obj) { if(!p_cnf->p_kont[kont] || !p_cnf->p_kont[kont]->p_obj[obj] || !p_cnf->p_kont[kont]->p_mt) { ddw("Spatny objekt file %s line %d",__FILE__,__LINE__); return;} }
#define n_kont_mrizka(kont)       { if(!p_cnf->p_kont[kont] || !p_cnf->p_kont[kont]->p_mt) { ddw("Spatny kontejner file %s line %d",__FILE__,__LINE__); return;} }
#define n_svetlo(kont,obj)        { if(!p_cnf->p_kont[kont] || !p_cnf->p_kont[kont]->p_obj[obj] || !p_cnf->p_kont[kont]->p_obj[obj]->p_light) { ddw("Spatne svetlo file %s line %d",__FILE__,__LINE__); return; }}
#define n_editlock(kont)          { if(p_cnf->p_kont[kont]->editlock) return;}
#define n_je_editlock(kont)       { if(!p_cnf->p_kont[kont]->editlock) return;}


#define  X_LOAD  100
#define  Y_LOAD  450
#define  X_RADKU 50
#define  Y_RADKU 6

void oe_cekej_na_klavesu(K_EDITOR *p_cnf);
void oe_cekej_na_mys(K_EDITOR *p_cnf);
float oe_stredni_vzdalenost_ve_scene(K_EDITOR *p_cnf);
float oe_nulova_vzdalenost_ve_scene(K_EDITOR *p_cnf);
float oe_objektova_vzdalenost_ve_scene(K_EDITOR *p_cnf);
void oe_konfiguruj_editor(K_EDITOR *p_cnf, byte *p_file, int renderovac);
void oe_uloz_konfiguraci(K_EDITOR *p_cnf, byte *p_dir, byte *p_file, byte dir);
void oe_ini_mod(K_EDITOR *p_cnf, int mod, int styl);
void oe_nahod_mod(K_EDITOR *p_cnf, int mod);
void oe_klavesy_v_game_modu(K_EDITOR *p_cnf);
void oe_klavesy_zmena_modu(K_EDITOR *p_cnf);
int  oe_dotaz_konzole_char(byte *p_dotaz, int radek, byte *p_ret);
int  oe_dotaz_konzole_int(byte *p_dotaz, int radek, int *p_cislo, int aktualni);
//int  oe_hlaska_konzole(LPDIRECTDRAWSURFACE7 p_con, byte *p_hlaska,int radek);
void oe_klavesy_load_save(K_EDITOR *p_cnf);
void oe_prikaz_posun_sceny(K_EDITOR *p_cnf);
void oe_prikaz_posun_sceny_xz(K_EDITOR *p_cnf);
void oe_prikaz_rotace_sceny(K_EDITOR *p_cnf, int mod);
void oe_prikaz_no_off_popisy_vertexu(K_EDITOR *p_cnf);
void oe_vybrani_ctverce(K_EDITOR *p_cnf, dword barva);
void oe_posun_ctverce(K_EDITOR *p_cnf);
void oe_posun_strany_ctverce(K_EDITOR *p_cnf);
void oe_vybrani_konstantniho_ctverce(K_EDITOR *p_cnf);
void oe_klavesy_editace_facu(K_EDITOR *p_cnf);
void oe_klavesy_editace_svetel(K_EDITOR *p_cnf);
void oe_pridej_dvertex(K_EDITOR *p_cnf, BOD *p, int k, int o, int f, byte pozice, byte d2);
void oe_pridej_texture_tag_3d(K_EDITOR *p_cnf, int cislo);
void oe_pridej_cary_texture_tag(K_EDITOR *p_cnf);
void oe_pridej_texture_tag_2d(K_EDITOR *p_cnf, int cislo, float x, float y);
void oe_zrus_texture_tagy(K_EDITOR *p_cnf);
void interpolace_primky(BOD *t1, BOD *t2, BOD *t, BOD *p);
void interpolace_textury(BOD *t1, BOD *t2, BOD *t3, BOD *t4, BOD *t, BOD *p);
void oe_transformuj_2d_texture_tagy(K_EDITOR *p_cnf, int k, int o, byte textura);
void oe_transformuj_2d_texture_tagy_body(K_EDITOR *p_cnf, byte textura);
void pridej_objekt_normalove_vektory(K_EDITOR *p_cnf);
void oe_prikaz_vyber_texture_tag_ctverec(K_EDITOR *p_cnf);
void oe_posun_vsechny_texture_tag(K_EDITOR *p_cnf);
void oe_posun_nejblizsi_texture_tag(K_EDITOR *p_cnf);
void oe_klavesy_editace_stinu(K_EDITOR *p_cnf);
void oe_prikaz_test(K_EDITOR *p_cnf);
void oe_prikaz_prepsat(K_EDITOR *p_cnf);
void oe_test(K_EDITOR *p_cnf);
void oe_klavesy_test(K_EDITOR *p_cnf);
void oe_klavesy_editace_kontejneru(K_EDITOR *p_cnf);
void oe_klavesy_editace_objektu(K_EDITOR *p_cnf);
void oe_prikaz_rychla_rotace(K_EDITOR *p_cnf, int t);
void oe_prikaz_rychly_mirror(K_EDITOR *p_cnf, int t, int mu, int mv);
void oe_klavesy_textury(K_EDITOR *p_cnf);
void oe_klavesy_editace_vertexu(K_EDITOR *p_cnf);
void oe_prikaz_zmena_textury_draty(K_EDITOR *p_cnf);
void oe_prikaz_zmena_textury_draty2(K_EDITOR *p_cnf);
void oe_prikaz_konzole_on_off(K_EDITOR *p_cnf);
void oe_prikaz_list_objekty(K_EDITOR *p_cnf, int smer);
void oe_prikaz_list_kontejnery(K_EDITOR *p_cnf, int smer);
void oe_prikaz_zmena_cul(K_EDITOR *p_cnf);
void oe_prikaz_selekce_objektu(K_EDITOR *p_cnf);
void oe_prikaz_selekce_kontejneru(K_EDITOR *p_cnf);
void oe_prikaz_zmena_kresleni_kontejneru(K_EDITOR *p_cnf);
void oe_prikaz_zmena_kresleni_objekt(K_EDITOR *p_cnf);
void oe_prikaz_zmena_kresleni_kamer(K_EDITOR *p_cnf);
void oe_prikaz_zmena_origin(K_EDITOR *p_cnf);
void oe_prikaz_smazani_konzole(K_EDITOR *p_cnf);
void oe_prikaz_nitkovy_kriz(K_EDITOR *p_cnf);
void oe_prikaz_zmena_konzole_objekty(K_EDITOR *p_cnf);
void oe_klavesy_konfigurace(K_EDITOR *p_cnf);
void oe_klavesy_vyroba_objektu(K_EDITOR *p_cnf);
dword oe_vrat_body_ve_scene(K_EDITOR *p_cnf, int x1, int y1, int x2, int y2, I_VERTEX *p_index);
int  oe_nejblizsi_bod_ve_scene(K_EDITOR *p_cnf, BOD *p_bod, int *p_k, int *p_o, int *p_f);
int  oe_prikaz_vyber_mysi_mesh(K_EDITOR *p_cnf);
void oe_prikaz_prirad_material_objekt(K_EDITOR *p_cnf);
void oe_prikaz_prirad_material_kontejner(K_EDITOR *p_cnf);
void oe_prikaz_kapatko(K_EDITOR *p_cnf);
void oe_mys_editace(K_EDITOR *p_cnf);
void oe_prikaz_edituj_rozlozeni_textury(K_EDITOR *p_cnf);
void oe_get_souradnice_mysi_2d(K_EDITOR *p_cnf, BOD *p_bod);
void oe_vyber_primku(K_EDITOR *p_cnf);
void oe_test_int(K_EDITOR *p_cnf);
void oe_klavesy_object_editor(K_EDITOR *p_cnf);
void go_object_editor(char *p_edit_kont, char *p_text_dir);
void oe_updatuj_scenu_stat(K_EDITOR *p_cnf);
void oe_poly_to_scena(K_EDITOR *p_cnf);
void oe_konec(K_EDITOR *p_cnf);
void oe_updatuj_scenu_flag(K_EDITOR *p_cnf);
void oe_updatuj_statistiku_sceny(K_EDITOR *p_cnf, SCENE_STATISTIC *p_stat);
void oe_updatuj_fps(K_EDITOR *p_cnf);
void oe_objekty_nacucni_z_materialu(K_EDITOR *p_cnf);
void oe_transformuj_scenu_staticke(K_EDITOR *p_cnf);
void oe_prikaz_vypis_face(K_EDITOR *p_cnf);
void oe_prikaz_posun_scenu(K_EDITOR *p_cnf);

void oe_prikaz_scena_zoom(int koeficient);

int  intersect_kont_fast(EDIT_KONTEJNER *p_kont, BOD *p_orig, BOD *p_dir);
int  intersect_kont(EDIT_KONTEJNER *p_kont, BOD *p_orig, BOD *p_dir, int *p_o, int *p_f, float *p_t);

// sledovani paprsku
int oe_intersect_scena(K_EDITOR *p_cnf, BOD *p_orig, BOD *p_dir, float *p_t, int *p_k, int *p_o, int *p_f);

//void oe_scena_to_poly(K_EDITOR *p_cnf);
//int oe_scena_to_poly(K_EDITOR *p_cnf, EDIT_MESH_POLY **p_list);
//void oe_poly_to_scena(K_EDITOR *p_cnf);

void oe_prikaz_narovnej_kameru(K_EDITOR *p_cnf);

void oe_wrlog(K_EDITOR *p_cnf, byte *p_text,...);
void oe_wrlog_matrix(K_EDITOR *p_cnf, GLMATRIX *p_mat, byte *p_text,...);

void oe_scena_to_game(K_EDITOR *p_cnf);

void oe_zalohuj_scenu(K_EDITOR *p_cnf);
void oe_zalohuj_scenu_zpet(K_EDITOR *p_cnf);

void oe_kamera_to_view(K_EDITOR *p_cnf, int k);
int  oe_prikaz_vyber_mysi_bod(K_EDITOR *p_cnf, BOD *p_p);

int  oe_cti_mod(K_EDITOR *p_cnf);

void oe_vyber_material(K_EDITOR *p_cnf);

char * ftoa(float cislo, char *p_string, int des);

void oe_prikaz_scale_sceny(K_EDITOR *p_cnf);

int  oe_scena_to_raykont(K_EDITOR *p_cnf, EDIT_KONTEJNER **p_ray, int max);

int  oe_je_group_kontejner(K_EDITOR *p_cnf);
int  oe_je_group_objekt(K_EDITOR *p_cnf);
int  oe_je_group_face(K_EDITOR *p_cnf);
void oe_vypis_poly(K_EDITOR *p_cnf);
void oe_zrus_vyber_kosti(K_EDITOR *p_cnf, EDIT_OBJEKT *p_obj);

void oe_obb_calc_scena(K_EDITOR *p_cnf);

int oe_prikaz_vyber_mysi_kosti(K_EDITOR *p_cnf);
int go_renderovac_renderuj(char *p_file, char *p_out);

// list konteneru
__inline void oe_klist_reset(int *k)
{
  *k = -1;
}

__inline EDIT_KONTEJNER * oe_klist_next_maska(K_EDITOR *p_cnf, int *k, dword maska, dword vysledek)
{
  EDIT_KONTEJNER *p_kont;
  while((*k)+1 < p_cnf->max_kont) {
    p_kont = p_cnf->p_kont[++(*k)];
    if(p_kont && ((p_kont->kflag&maska) == vysledek))
      return(p_kont);    
  }   
  return(NULL);
}

__inline EDIT_KONTEJNER * oe_klist_next(K_EDITOR *p_cnf, int *k)
{
  EDIT_KONTEJNER *p_kont;
  while((*k)+1 < p_cnf->max_kont) {
    p_kont = p_cnf->p_kont[++(*k)];
    if(p_kont)
      return(p_kont);    
  }   
  return(NULL);
}

