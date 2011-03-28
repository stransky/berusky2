/*
  Header k enginu
*/
#ifndef __MAT_3D_H
#define __MAT_3D_H

#define  MAT_TEXTUR    4

#define  T_WRAP          0x1
#define  T_MIRROR        0x2

#define  K_CHYBA         (-1)
#define  NIC             K_CHYBA

#define  MAX_TEXT_ANIM   50

#ifndef GK_LOOP
#define GK_LOOP          0x100   // Run-animace se prehrava furt dokola
#endif

#ifndef GK_REMOVE
#define GK_REMOVE        0x200   // Animace se po dobehnuti automaticky smaze
#endif


#define STATE_ADD        2
#define STATE_PRUHLEDNY  3


/* Definice typu
*/
typedef struct _MUJ_BOD {
 
  float x, y, z;
  float px,py,pz;
  float nx,ny,nz;
  
  float mdr,mdg,mdb,mda;
  float dr ,dg ,db ,da;
  float msr,msg,msb;
  float sr ,sg ,sb;
  
  float tu1,tv1; // 4 textury
  float tu2,tv2;
  float tu3,tv3;
  float tu4,tv4; // lightmapa
  float tul,tvl;

} MUJ_BOD,OBJ_VERTEX,OBJEKT_BOD;

/* Definice vertexovych poli
*/
typedef struct _VERTEX_ARRAYS {

  int ati_handle;      // handle na ati vertex-pole    
  int ati_velikost;    // velikost pole - pro cely objekt
  int ati_start_pos;
  int ati_start_norm;
  int ati_start_diff;
  int ati_start_spec;
  int ati_start_text[MAT_TEXTUR];

  int arb_handle_indicie;

} VERTEX_ARRAYS;

typedef struct _NORMALBOD {
 
  float x,y,z;
  dword diff;

} NORMALBOD;

typedef struct _MUJ_BOD_FLARE {
 
  float x,y,z,rhw;
  dword diff;
  float tu1,tv1;

} MUJ_BOD_FLARE;

typedef struct _DXBOD {
  
  float x,y,z;
  float r,g,b;
  
} DXBOD;

/* Animation key settings */
typedef struct _KEY_POINT_3DS
{
  word     frame;      /* Key's frame position */
  word     flags;      /* Spline terms used flag */
  float    tension;    /* Flagged with 0x01 */
  float    continuity; /* Flagged with 0x02 */
  float    bias;       /* Flagged with 0x04 */
  float    easeto;     /* Flagged with 0x08 */
  float    easefrom;   /* Flagged with 0x10 */
} KEY_POINT_3DS;

typedef struct _KEY_POINT_BRS
{
  dword    time;       /* cas tohoto klice */  
  float    tension;    /* Flagged with 0x01 */
  float    continuity; /* Flagged with 0x02 */
  float    bias;       /* Flagged with 0x04 */
  float    easeto;     /* Flagged with 0x08 */
  float    easefrom;   /* Flagged with 0x10 */
} KEY_POINT_BRS;

/* Rotation key */
typedef struct _ROTKEY {
   float angle; /* angle of rotation */
   float x;     /* rotation axis vector */
   float y;
   float z;
} ROTKEY;

typedef struct _MATERIAL {

  float  ambient_r;
  float  ambient_g;
  float  ambient_b;
  
  float  diffuse_r;   // difusni barva
  float  diffuse_g;
  float  diffuse_b;
  float  diffuse_a;
  
  float  specular_r;  // zrcadlova barva
  float  specular_g;
  float  specular_b;
  
  float  faktor_r;    // barva na dalsi fligny
  float  faktor_g;
  float  faktor_b;
  float  faktor_a;
 
} MATERIAL;


typedef struct _ZDRCADLO_DESC_POLY {

  int        zrcadlo_k; // kontejner zrcadla
  int        zrcadlo_o; // objekt zrcadla
  int        id_kont;   // id kontejneru
  int        poly;      // poly ktereho se to tyka
  
  struct    _ZDRCADLO_DESC_POLY *p_next;

} ZDRCADLO_DESC_POLY;


typedef struct _ZDRCADLO_DESC {

  // Zustava - popisuje rovine zrcadlo
  GLMATRIX   ref;       // reflexni matice
  BOD        p[4];      // 4 body roviny zrcadla
  ROVINAD    r[5];      // 4 plochy klipovaci pyramidy

  ZDRCADLO_DESC_POLY *p_poly;

} ZDRCADLO_DESC;

/*
typedef struct _OBB {

 BOD          obb[3];     // obb smerovy vektory
 BOD          obb_stred;
 BOD          obb_len;
 BOD          obb_hran[8]; // 8 hranicnich bodu obalky
 
 BOD          aabb_min;    // aabb obalka
 BOD          aabb_max;    // aabb obalka

} OBB;
*/

// Multitextuing pro 1 texturu
typedef struct _MULTITEXT_FLAG {

  GLenum cil;
  GLenum mod;
  GLint  param;
    
} MULTITEXT_FLAG;

typedef struct _OLD_MULTITEXT_FLAG {

  dword a_arg1,a_op,a_arg2;
  dword c_arg1,c_op,c_arg2;
  dword src,dest;

} OLD_MULTITEXT_FLAG;

// preddefinovany hodnoty pro alfa blending/ michani textur a pod.
// nastaveni michanice
typedef struct _EDIT_STATE_ALFA_BLOK {

  char   jmeno[MAX_JMENO];
  int    alfa_pruhledny;
  int    funkce;

} EDIT_STATE_ALFA_BLOK;

typedef struct _EDIT_STATE_TEXT_BLOK {

  char   jmeno[MAX_JMENO];        // jmeno bloku
  int    funkce;                  // michaci funkce
  int    edit_funkce;             // editacni funkce (-1 pro default)
  int    textur;                  // pocet porebnych textur 1 (2,3,4 po trebuji extra dialog)
  int    delka;                   // delka bloku (pocet textur)
  int    textury_typ[MAT_TEXTUR]; // pozadovany typ textur pro dany efekt
  int    text_funkce[MAT_TEXTUR];   // pozadovany typ textur pro dany efekt
  
} EDIT_STATE_TEXT_BLOK;

/* 1 renderovaci blok materialu
*/
/*
typedef struct _MATERIAL_TEXT {

  int   text_stage;                 // stage_funkce pro tento blok
  int   textury[MAT_TEXTUR];        // seznam textur pro dany efekt
  int   text_koord[MAT_TEXTUR];     // cislo koordinatu textur
  int   text_funkce[MAT_TEXTUR];    // cisla env funkci
  float text_ati_bump_matrix[4];    // Ati-bump-map parametry
  //float parametry...

} MATERIAL_TEXT;
*/

#define  MLHA_AKTIVNI    0x1

typedef struct _MLZNA_KOSTKA {

  char  jmeno[MAX_JMENO];// jmeno mlzne kostky
  BOD   min,max;         // rozmery kostky
  int   flag;            // flagy mlhy
  int   mod;             // mod mlhy (linear/exp/exp2)
  int   priorita;        // priorita mlzne kostky
  float start;           // zacatek
  float stop;            // konec
  float intenzita;       // intenzita
  float r,g,b,a;         // barva
  void *p_kont;          // pointer na kontejner ke kteremu je privazana
  int   poly;            // zazba na poly

  struct _MLZNA_KOSTKA *p_next;

} MLZNA_KOSTKA;

#define  SL2_DYNAMIC    0x1        // svetlo se aplikuje i na dynamicke veci
#define  SL2_MESH       0x2        // svetlo pouziva mesh

typedef struct _STATIC_LIGHT {

  int       akt;         // aktivni svetlo
  char      jmeno[MAX_JMENO]; // jmeno svetla
  int       index;       // index svetla (pozice + ID)  
  BOD       p;
  float     r,g,b;
  float     k1,k2,k3;    // tlumici koeficienty
  float     dosah;       // max. dosah svetla
  char      uhel;        // tlumit podle uhlu svetla
  char      utlum;       // tlumit podle vzdalenosti
  void     *p_flare;     // flare svetla - pointer na nej
  int       last_poly;   // posledni hitovany poly
  int       last_kont;   // posledni hitovany kontejner
  int       flag;        // flag vlasnosti svetla -> update a podobne
  int       flag2;       // druhy flag -> vlastnosti
  void     *p_mesh_data; // privazany mesh, ke kterymu to patri
  int       bez_prev;    // vyjmout z dyn osvetlovani

  struct _STATIC_LIGHT *p_next; // nasledujici svetlo (v seznamu u kontejneru)
  struct _STATIC_LIGHT *p_prev; // predchozi svetlo

} STATIC_LIGHT;

typedef struct _DYN_LIGHT_ANIM {

  dword          endtime;      // posledni cas

  int            pos_keys;     // klice posunu
  KEY_POINT_BRS *p_pkeys;
  BOD           *p_pos;
  
  int            trg_keys;     // klice cile
  KEY_POINT_BRS *p_tkeys;
  BOD           *p_trg;
  
  int            dos_keys;     // klice dosah/min/max
  KEY_POINT_BRS *p_dskeys;
  BOD           *p_dosah;

  int            diff_keys;    // diffusni klice
  KEY_POINT_BRS *p_dfkeys;
  BARVA_RGBA    *p_diff;  

  int            alfa_keys;    // alfa klice
  KEY_POINT_BRS *p_akeys;
  float         *p_alfa;

  int            spec_keys;    // spekularni klice
  KEY_POINT_BRS *p_spkeys;
  BARVA_RGBA    *p_spec;

} DYN_LIGHT_ANIM;

#define SDL_MESH            0x00000100 // svetlo patri k urcitemu meshi
#define SDL_ADD_SUB         0x00000080 // svetlo pracuje v komplementarnim add/sub modu
#define SDL_SUB             0x00000200 // svetla se nepricitaji ale moduluji
#define SDL_UTLUM_LINEAR    0x00000400 // linearni utlum - defaultni
#define SDL_UTLUM_KVADRATIC 0x00000800 // kvadraticky utlum

typedef struct _DYN_LIGHT {

  char   jmeno[MAX_JMENO];

  int    aup;         // aktualni upload
  float  avzdal;      // aktualni vzdalenost
  int    akt;         // svetlo je aktivni/neaktivni
  
  dword  flag;        // flagy svetla
  dword  ktrida;      // kofolova trida pouziti
  dword  pflag;       // editacni flagy svetla

  int    index;

  BOD    np;          // netransformovana pozice
  BOD    tp;          // transformovana pozice
  BOD    dir;         // umisteni a rozsahy svetla
  float  look_max;    // konec kuzele (default = 255 -> prepocitam podle vzdalenosi)
  float  dosah;       // konec svetla
  float  min;         // zacatek tlumeni kuzele
  float  max;         // konec tlumeni kuzele
  float  theta;       // sirka kuzele svetla
  
  float  dr,dg,db;    // diffusni barva svetla ve floatu    
  float  sr,sg,sb;    // diffusni barva svetla ve floatu - odecitaci
  void  *p_mesh_data; // pointer meshe, ke kteremu je svetlo pripojeny

  dword  time;
  dword  time_stop;
  dword  time_start;
  dword  time_delka;
  dword  start;
  dword  stop;
  int   *p_flag;       // pointer kam hazu prubeh animace
  dword  an_flag;      // animacni flagy (loop/auto remove)
  DYN_LIGHT_ANIM an;

  struct _DYN_LIGHT *p_next;
  struct _DYN_LIGHT *p_prev;

} DYN_LIGHT;

#define EDL_BODOVE       0x00000001 // Bodove svetlo
//#define EDL_REFLEKTOR    0x00000002 // Reflektor
#define EDL_PLOSNE_X     0x00000004 // Sviti pouze plosne
#define EDL_PLOSNE_Y     0x00000008

#define EDL_PLOSNE_Z     0x00000010
#define EDL_MESH         0x00000020 // Svetlo je privazane na meshe
#define EDL_SCENA        0x00000040 // Svetlo je privazane na scenu
#define EDL_ZEMNI_MLHA   0x00000080 // je to zemni mlha

#define EDL_GOURAND      0x00000100 // pocitat uhly
#define EDL_UTLUM_LIN    0x00000200 // pocita se utlumove faktory
#define EDL_UTLUM_KVAD   0x00000400 // pocita se utlumove faktory
#define EDL_DOSAH        0x00000800 // svetlo ma omezeny dosah

#define EDL_MAPPING      0x00001000 // Pouzije se na texturove koordinaty
#define EDL_MESH_LIGHT   0x00002000 // Svetlo sviti pouze na vybrane meshe (seznam meshu)
#define EDL_PRUHL_LIGHT  0x00004000 // "Zpruhlednovaci" svetlo
#define EDL_CAMERA_POINT 0x00008000 // svetlo je umistene ke kamere

#define EDL_ALFA_MOD     0x00100000 // Alfa kanal se bude modulovat
#define EDL_ALFA_SET     0x00200000 // Alfa kanal se nastavi (prepise puvodni hodnoty)
#define EDL_ALFA_ADD     0x00400000 // Alfa kanal se pricte
#define EDL_ALFA_SUB     0x00800000 // Alfa kanal se odecita

#define EDL_SPEC_MOD     0x01000000 // Spekularni Sveto se bude modulovat
#define EDL_SPEC_SET     0x02000000 // Spekularni Svetlo se nastavi (prepise puvodni hodnoty)
#define EDL_SPEC_ADD     0x04000000 // Spekularni Svetlo se pricte
#define EDL_SPEC_SUB     0x08000000 // Spekularni Svetlo se odecita

#define EDL_ZMENA_CAMERA 0x10000000 // Pozemni mlha - pri zmene kamery
#define EDL_ANIMACE      0x20000000 // svetlo se prave animuje
#define EDL_ZMENA_LOKAL  0x40000000 // lokalni zmena svetla
#define EDL_ZMENA_GLOBAL 0x80000000 // globalni zmena svetla

typedef struct _EXTRA_DYN_LIGHT {
   
  char   jmeno[MAX_JMENO];

  int    akt;         // svetlo je aktivni/neaktivni
  dword  flag;        // flagy svetla
  dword  pflag;       // posunovaci flag
  dword  mflag;       // map-flagy svetla
  dword  ktrida;      // kofolova trida svetla

  int    index;

  BOD    tp;
  BOD    np;
  BOD    dir;
  float  dosah;       // dosah svetla
  float  theta;       // sirka kuzele svetla
  float  utlum;       // utlum svetla
  float  gourand;     // koeficient gourandu -> lesk
  
  //float  dr,dg,db,da; // diffusni barva svetla
  float  da;          // diffusni alfa svetla
  float  sr,sg,sb;    // diffusni barva svetla
  
  void  *p_mesh_data; // zavisly mesh

  int   *p_lmesh;     // seznam privazanych meshu
  int    lmeshnum;    // max pocet meshu
  int    lmeshakt;    // aktualni pocet meshu

  dword  time;
  dword  time_stop;
  dword  time_start;
  dword  time_delka;
  dword  start;
  dword  stop;
  int   *p_flag;       // pointer kam hazu prubeh animace
  dword  an_flag;      // animacni flagy (loop/auto remove)
  DYN_LIGHT_ANIM an;

  float  v_start;      // zemni mlha - parametry
  float  v_stop;
  float  h_start;
  float  h_stop;

  struct _EXTRA_DYN_LIGHT *p_next;
  struct _EXTRA_DYN_LIGHT *p_prev;

} EXTRA_DYN_LIGHT;

/*
  Flagy materialu
*/
#define MAT_MATERIAL    0x00000001 // na objektu je material
#define MAT_PRUHLEDNY   0x00000002 // pruhledny material
#define MAT_ANIM_FRAME  0x00000010 // material je animovany framama
#define MAT_POSUN2D     0x00000100 // posun textury - u animovanych textur
#define MAT_3DS_FLAGS   0x00000200 // modifikace textur
#define MAT_POUZITY     0x00002000 // pouzity material
#define MAT_SYSTEM      0x00004000 // systemovy material
#define MAT_LIB         0x00008000 // knihovni material
#define MAT_NO_ZMASK    0x01000000
#define MAT_NO_ZTEST    0x02000000
#define MAT_NO_CULL     0x04000000
#define MAT_NO_BUMP     0x08000000 // nepouzit automaticky bump-maping
#define MAT_SCENE       0x10000000 // material sceny
#define MAT_SCENE_NUTNY 0x20000000 // material sceny nutny
#define MAT_NO_SCMT     0x40000000 // material bez causticu

#define MAT_T1_EYE_LIN  0x00000800
#define MAT_T2_EYE_LIN  0x00001000
#define MAT_T3_EYE_LIN  0x00100000
#define MAT_T4_EYE_LIN  0x00200000

#define MAT_T1_SPHERE   0x00400000
#define MAT_T2_SPHERE   0x00800000
#define MAT_T3_SPHERE   0x40000000
#define MAT_T4_SPHERE   0x80000000

/* 
  Flagy 2
*/
#define MAT2_DIFFUSE            0x00000001 // objekt pouziva difusni barvu, ma se kreslit
#define MAT2_SPECULAR           0x00000002 // objekt pouziva spekularni barvu, ma se kreslit

#define MAT2_DIFFUSE_RAY        0x00000010 // material se pouzije pro raytracing
#define MAT2_SPECULAR_RAY       0x00000020 // spocitaji se odlesky

#define MAT2_MAP1               0x00000040 // koordinaty 1 aktivni
#define MAT2_MAP2               0x00000080 // koordinaty 2 aktivni

#define MAT2_CALC_MAP1          0x00000400 // pocitaji se koordinaty1
#define MAT2_CALC_MAP2          0x00000800 // pocitaji se koordinaty2

#define MAT2_DEF_TEXTANIM       0x00001000 // defaultne se spousti prvni animace textur

#define MAT2_ENV_SPEC           0x00002000 // spekularni env mapping
#define MAT2_ENV_SPEC_OBE       0x00004000 // spekularni oboustrany env mapping

#define MAT2_MAP3               0x00008000 // koordinaty 3 aktivni
#define MAT2_MAP4               0x00010000 // koordinaty 4 aktivni

#define MAT2_CALC_MAP3          0x00020000 // pocitaji se koordinaty3
#define MAT2_CALC_MAP4          0x00040000 // pocitaji se koordinaty4

#define MAT2_T1_MATRIX          0x00080000 // pouzivat transformacni matici c.3
#define MAT2_T2_MATRIX          0x00100000 // pouzivat transformacni matici c.4
#define MAT2_T3_MATRIX          0x00200000 // pouzivat transformacni matici c.1
#define MAT2_T4_MATRIX          0x00400000 // pouzivat transformacni matici c.2

#define MAT2_MASKA              0x00800000 // pruhledny material

#define MAT2_BUMP               0x01000000 // pruhledny material


/* Run-time flagy
*/
#define KONT_POHYB              0x00000001  // Mesh se pohnul v poslednim tahu
#define KONT_VIDITELNY          0x00000002  // Mesh je viditelny
#define KONT_DRAW_SPEC          0x00000004  // Mesh/Poly pouziva na vykresleni docasne spekularni barvu
#define KONT_MPMATRIX           0x00000008  // kontejner pouziva MP matrix

#define KONT_DRAW_PRUHL         0x00000010  // Mesh/Poly se ma kreslit jako pruhledny material
#define KONT_DRAW_CAMERA        0x00000020  // Mesh/Poly je pred kamerou a ma se kreslit jako pruhledny
#define KONT_DRAW_LIGHT         0x00000040  // Mesh/Poly je privazan ke extra-svetlu
#define KONT_DRAW_NOBALKA       0x00000080  // Mesh/Poly nema obalku (nema se na ni testovat)

#define KONT_DRAW_VODA          0x00000100  // Mesh/Poly je voda (pouzivaji se 3.ti barevne slozky)
#define KONT_VIDITELNY_ZRC      0x00000200  // Mesh je viditelny v zrcadle
#define KONT_OBRUBA             0x00000400  // Mesh je obruba levelu
#define KONT_PODLOZKA           0x00000800  // Mesh je podlozka levelu

#define KONT_NO_SELF_SHADOW     0x00001000 // nevrha stiny na sebe
#define KONT_PLG_OFFSET         0x00002000 // Mesh pouziva polygon-offset
#define KONT_NORMAL_ANIM        0x00004000 // animace normalovych vektoru
#define KONT_VIDITELNY_PRUHL    0x00008000 // poly/mesh je viditelny a pruhledny

#define KONT_SVETLO             0x00010000 // kontejner ma pripojene svetlo
#define KONT_VODA               0x00020000 // kontejner ma pripojenou vodu/vlnovy generator
#define KONT_NORMAL_ANIM_POS    0x00040000 // animace normalovych vektoru
//#define KONT_VIDITELNY_ZRC_FULL 0x00040000 // Mesh je viditelny v zrcadle
#define KONT_UPLOAD             0x00080000 // Upload meshe do vertex-pole

#define KONT_NO_FOG             0x00100000 // - nekresli se mlha 
#define KONT_NO_CALC            0x00200000 // - je to needitovaelny objekt
#define KONT_POHYB_ZRUS         0x00400000 // zrusit pohybovej flag
#define KONT_STATIC             0x00800000 // je staticky

//#define KONT_LEVEL_CUBE         0x01000000 // je to kostka kolem levelu
#define KONT_BRUTALNORMAL       0x02000000 // uplne kulaty objekt
#define KONT_VIDITELNY_ZMENA    0x04000000 // zmenila se viditelnost meshe (z neviditelneho na viditelneho!)
#define KONT_PRVEK              0x08000000 // prvek levelu

#define KONT_VERTEX_LIGHT       0x10000000 // svetla ve vertexech
#define KONT_NO_DRAW_SHADOW     0x20000000 // nevrha stiny na dalsi objekty
#define KONT_KEYFRAME           0x40000000 // kontejner ma pripojene keyframy
#define KONT_ZRCADLO            0x80000000 // kontejner/poly je zrcadlo

#define KONT2_UPDATE_POS        0x00000001
#define KONT2_UPDATE_NORM       0x00000002
#define KONT2_UPDATE_DIFF       0x00000004
#define KONT2_UPDATE_SPEC       0x00000008
#define KONT2_UPDATE_TEXT1      0x00000010
#define KONT2_UPDATE_TEXT2      0x00000020
#define KONT2_UPDATE_TEXT3      0x00000040
#define KONT2_UPDATE_TEXT4      0x00000080
#define KONT2_UPDATE_NEW        0x00000100

#define KONT2_UPLOAD            (KONT2_UPDATE_POS|KONT2_UPDATE_NORM|\
                                 KONT2_UPDATE_DIFF|KONT2_UPDATE_SPEC|\
                                 KONT2_UPDATE_TEXT1|KONT2_UPDATE_TEXT2|\
                                 KONT2_UPDATE_TEXT3|KONT2_UPDATE_TEXT4|\
                                 KONT2_UPDATE_NEW)

#define KONT2_LOADED            0x00000100 // kontejner byl nahran z disku
#define KONT2_JOINT_ANIM        0x00000200 // kontejner ma joint animace
#define KONT2_NO_PHONG          0x00000400 // kontejner se renderuje bez phongu
#define KONT2_BERUSKA           0x00000800 // kontejner je beruska


#define KONT_STATICKY_PRVEK  (KONT_PRVEK|KONT_STATIC)
#define KONT_DYNAMICKY_PRVEK (KONT_PRVEK|KONT_VERTEX_LIGHT|KONT_NO_DRAW_SHADOW|KONT_NO_SELF_SHADOW)

/*
  Frame animace
*/

#define FRAME_EVENT       0x1  // timto framem se nepokracuje (ceka se na udalost)
#define FRAME_ALFA_FAKTOR 0x2  // tento frame ma nastavit alfa-stage
#define FRAME_TEXTURA     0x4  // tento frame ma nastavit texturu
#define FRAME_POSUN       0x8  // posun texturu oproti originalu


typedef struct _ANIM_FRAME {

  char                  file[MAX_JMENO];  // soubor s animaci
  EDIT_TEXT            *p_text;
  dword                 time;             // cas
  dword                 alfa;             // alfa faktor
  float                 u,v;              // posun textury oproti originalu
  dword                 flag;             // flagy framu

} ANIM_FRAME;


typedef struct _ANIM_MATERIAL {

  char        jmeno[MAX_JMENO];
  byte        pouzita;   // pouzita animace
  byte        aktivni;   // aktivni animace (animuje se)

  int         framenum;  // framova animace
  int         frameakt;  // aktualni frame
  ANIM_FRAME *p_frame;   // pole framu
  dword       last_time; // posledni cas animace (v jaky cas byl zobrazen 
                         // posledni frame)
} ANIM_MATERIAL;


// flagy importovane z 3D studia
typedef struct _TDS_MAT_FLAGS {

  float u_scale;
  float v_scale;
  float vyc1;

  float u_offset;
  float v_offset;
  float vyc2;

  float rot_angle;

} TDS_MAT_FLAGS;

#define ANIM_TEXT_DEFAULT   0x1

typedef struct _ANIM_TEXT {

  int      flag;           // flagy animace (loop a pod)  
  int     *p_flag;         // strkani vysleku animovani

  byte     stav;           // 0 = stop, 1 = run
  
  dword    endtime;        // konecny cas (pocet framu)
  dword    time;           // aktualni cas  
  dword    time_start;
  dword    time_stop;
  dword    time_delka;
  
  dword    start;          // start
  dword    stop;           // stop

  BOD      pos;            // vysledny pozucni klic
  int      pos_keys;       // pozicni klice
  BOD     *p_pos;          // pozice
  KEY_POINT_BRS *p_pkeys;  // Spline values for pos keys 
  
  BOD      pivot;          // vysledny pivot klic
  int      piv_keys;       // pivot klice
  BOD     *p_piv;          // pivot
  KEY_POINT_BRS *p_vkeys;  // Spline values for pivot keys

  BOD      scs;            // vysledny scale klic
  int      scs_keys;       // scale klice
  BOD     *p_scale;        // scaling keys
  KEY_POINT_BRS *p_skeys;  // Spline values for scale keys 
  
  float    rot;            // vysledny uhel
  int      rot_keys;       // Number of rotation keys 
  float   *p_rot;          // Absolutni roracni traky
  KEY_POINT_BRS *p_rkeys;  // Spline values for rotation keys 

  int      rez[10];        // rezervovano

} ANIM_TEXT;

#define SMATERIAL_POSLEDNI 10

typedef struct _EDIT_MATERIAL {

  // Jmeno materialu
  char            jmeno[MAX_JMENO];
  
  // Textury materialu
  char            textfile[MAT_TEXTUR][MAX_JMENO];
  EDIT_TEXT      *p_text[MAT_TEXTUR];
  int             textflag[MAT_TEXTUR];// flag textury
  GLMATRIX        ttext[MAT_TEXTUR];   // matrice posunu co se dava jako settransformmatrix
  float           texgen_koord_s[MAT_TEXTUR][4];// texture-generovane veci
  float           texgen_koord_t[MAT_TEXTUR][4];// texture-generovane veci
  float           texgen_koord_r[MAT_TEXTUR][4];// texture-generovane veci
  float           texgen_koord_q[MAT_TEXTUR][4];// texture-generovane veci

  /* Michaci funkce
  */
  int             alfa_state;             // alfa blok
  MATERIAL_TEXT   text_state[MAT_TEXTUR]; // texture bloky
  
  float           lesk;      // lesklost 
  MATERIAL        dxmat;     // barva objektu
  ANIM_MATERIAL   anim;      // frame animace (u animovanych materialu)
  
  int             atextnum[MAT_TEXTUR];
  int             atextakt[MAT_TEXTUR];
  ANIM_TEXT      *p_atext[MAT_TEXTUR][MAX_TEXT_ANIM];  // animace textur 

  int             smaterial_pozice; // pozice s-materialu
  int             textur;       // celkovy pocet textur ktery material pouziva
  
  dword           flag;         // flagy materialu
  dword           flag2;        // flagy2 materialu

  int             material_handle; // handle materialu
  
  TDS_MAT_FLAGS   tds;         // modifikace importovane z 3d studia

  float           env_r,       // env spekularni barva
                  env_g,
                  env_b;
  float           env_add1;    // aditivni konstanta 1
  float           env_add2;    // aditivni konstanta 2
  float           env_scale;   // scale env uhlu  

  struct _EDIT_MATERIAL *p_sub; // pole sub-materialu (pouze pro ase-loader)  

} EDIT_MATERIAL;



#define PART_AKT   0x1   // castice je aktivni
#define PART_POHYB 0x2   // castice se pohybuje
#define PART_SCALE 0x4   // castice se roztahuje/smrstuje
#define PART_ANIM  0x8   // castice je animovana

/*
  Tracky nacitane z 3ds souboru
*/
typedef struct _TRACK_INFO {
  
  char jmeno[MAX_JMENO];
  int  objekt;  //objekt kteremu tento track patri
                //kazdy objekt ma 1 track
  int  parent;  //rodic tohoto objektu
  BOD  pivot;   //pivot-point
  word flags[2];//flagy animace
  
  int  framenum;
  int  framestart;

  // Position keys
  dword     pos_keys;      // Number of position keys 
  word      pos_flag[7];   // Loop control flag for position keys 
  KEY_POINT_BRS *p_pkeys;  // Spline values for position keys 
  BOD      *p_pos;         // Mesh position keys 
  
  // Rotation keys
  dword    rot_keys;       // Number of rotation keys 
  word     rot_flag[7];    // Loop control flag for rotation keys 
  KEY_POINT_BRS *p_rkeys;  // Spline values for rotation keys 
  QUAT     *p_at;          // Absolutni traky
  ROTKEY   *p_rot;         // Rotation keys 
  
  // Scale keys
  dword    scs_keys;       // Number of scaling keys 
  word     scs_flag[7];    // Loop control flag for scaling keys 
  KEY_POINT_BRS *p_skeys;  // Spline values for scaling
  BOD      *p_scale;       // Mesh scaling keys 
  
  struct _TRACK_INFO *p_next; //dalsi track

} TRACK_INFO;

typedef struct _KEYFRAMER_DATA {

  int         frame_start;
  int         frame_stop;
  TRACK_INFO *p_track;

} KEYFRAMER_DATA;

typedef struct _KAMERA_TRACK_INFO {

  char      jmeno[MAX_JMENO];
  int       flag; 
  int       parent;
  int       objekt;
  dword     endtime;

  int       stav;
  
  dword     time;
  dword     time_start;
  dword     time_stop;
  dword     time_delka;
  
  dword     start;
  dword     stop;

  // Position keys
  int       pos_keys;      // Number of position keys 
  KEY_POINT_BRS *p_pkeys;  // Spline values for position keys 
  BOD      *p_pos;         // Kamera position keys 
  
  // Target keys
  int       trg_keys;      // Number of target keys 
  KEY_POINT_BRS *p_tkeys;  // Spline values for target keys 
  BOD      *p_trg;         // target keys 

  // Roll keys
  int      roll_keys;       // Number of roll keys 
  KEY_POINT_BRS *p_rlkeys;  // Spline values for roll keys 
  float   *p_roll;
  
  // FOV keys
  int      fov_keys;       // Number of fov keys 
  KEY_POINT_BRS *p_fkeys;  // Spline values for fov keys
  float   *p_fov;

  // Quat-keys
  int      quat_keys;      // Number of scaling keys 
  KEY_POINT_BRS *p_qkeys;  // Spline values for scaling
  QUAT    *p_quat;         // Mesh scaling keys 
  
} KAMERA_TRACK_INFO;

/*
  Tracky udrzovane v pameti
*/
typedef struct _HIERARCHY_TRACK_INFO {
  
  // provazani se scenou
  struct _HIERARCHY_TRACK_INFO  *p_otec;
  int                            childnum; // pocet detskych traku
  struct _HIERARCHY_TRACK_INFO **p_child;  // detske tracky
  
  // Aktualni objekt
  void      *p_obj; //-> kam to mam sypat
  GLMATRIX  *p_m;   //-> do teto matice to soupat
  BOD        pivot;  
  BOD        pivot_sim;

  // Jmena rodicu
  char       jmeno[MAX_JMENO];
  char       jmeno_otec[MAX_JMENO];

  // loadovaci promene
  int      parentnum;
  int      objnum;  
  
  dword    endtime;        // posledni cas animace (vetsinou delka)

  int      pos_keys;       // pozicni klice
  KEY_POINT_BRS *p_pkeys;  // Spline values for position keys
  BOD     *p_pos;          // Mesh position keys   

  int      scs_keys;       // scale klice
  KEY_POINT_BRS *p_skeys;  // Spline values for scaling
  BOD     *p_scale;        // Mesh scaling keys
  
  int      rot_keys;       // Number of rotation keys 
  KEY_POINT_BRS *p_rkeys;  // Spline values for rotation keys 
  QUAT    *p_at;           // Absolutni traky
  
  
} HIERARCHY_TRACK_INFO;

typedef struct _HIERARCHY_ROOT {
  
  float     delka;         // delka casu animace (fps)
  byte      stav;          // 0 = stop, 1 = run
  
  int       flag;          // flagy animace (GL_LOOP,GL_REMOVE)

  dword     time;          // aktualni cas animace
  dword     time_start;    // aktualni start
  dword     time_stop;     // aktualni stop
  dword     time_delka;    // aktualni delka animace
  
  dword     start;
  dword     stop;

  int       framenum;      // framenum = 0 - volna animace
  
  int       childnum;      // pocet detskych traku
  char      jmeno[MAX_JMENO]; // jmena animaci
  HIERARCHY_TRACK_INFO **p_child;

} HIERARCHY_ROOT;

/*
typedef struct _MESH_OBALKY {

 BOD lmin;      // lokalni obalky min (pouze vertexy)
 BOD lmax;      // lokalni obalky max 
 BOD gmin;      // globalni obalky min (transformovane pozicni matici meshe)
 BOD gmax;      // globalni obalky max

} MESH_OBALKY;
*/

#define         KONT_MAX_ANIM   40
#define         KAM_MAX_ANIM    10

#define         SIMPLE_AKTIVNI  0x8

/*
  simple track info -> na cely kontejner jeden track
*/
typedef struct _SIMPLE_TRACK_INFO {
  
  int       Objekt_ID;      // id objektu ke kteremu animace patri
  dword     flag;
  
  int       keynum;         // pocet klicu
  BOD      *p_pos;          // Mesh position keys
  BOD      *p_scale;        // Mesh scaling keys
  QUAT     *p_at;           // Absolutni traky
  float    *p_norm;         // Normal-scale animace
  BOD       pivot;          // pivot point animace -> kopiruje se z objektu
  BOD       pivot3ds;       // pivot 3ds animace

  GLMATRIX *p_m;          // kam to hazet
  GLMATRIX *p_all_matrix; // v pripade all-inclusive
  float    *p_object_norm;// semka strkat normal-scale

  OBB      *p_obb_local;
  OBB      *p_obb_world;
  
  int       childnum;      // pocet svistich traku
  struct   _SIMPLE_TRACK_INFO *p_child; // svisti traky
  struct   _SIMPLE_TRACK_INFO *p_otec;  // otcovy traky
  
} SIMPLE_TRACK_INFO;

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

// vyrobi list sousednosti plosek -> na filtraci
typedef struct _FACE_SOUS {

  word f[3]; // pokud face == K_CHYBA -> je to to samy

} FACE_SOUS;

// Bezier PRS klice pro uzel
typedef struct _JOINT_KEYS {   

  int            pos_keys; // pozicni klice
  KEY_POINT_BRS *p_pkeys;  // Spline values for position keys
  BOD           *p_pos;    // Mesh position keys   

  int            rot_keys; // Number of rotation keys
  KEY_POINT_BRS *p_rkeys;  // Spline values for rotation keys
  QUAT          *p_rot;    // Absolutni traky

  int            scs_keys; // scale klice
  KEY_POINT_BRS *p_skeys;  // Spline values for scaling
  BOD           *p_scale;  // Mesh scaling keys  

} JOINT_KEYS;

// tuhost kloubu?
// typ kloubu? (rotace/...)
typedef struct _JOINT {
  
  int     joint_ID;       // ID a cislo jointu

  int     vertexnum;      // seznamy vertexu
  int    *p_vertexlist;
  
  int     flag;  
  float   tuhost;         // tuhost kloubu (pro in-game animace)
   
  dword   pos_keys;       // absolutni pozice
  BOD    *p_pos;
  BOD     p;
  
  dword   rot_keys;       // absolutni rotace - tracky
  QUAT   *p_rot;
  QUAT    r;
  
  dword   scs_keys;       // absolutni scale
  BOD    *p_scs;
  BOD     s;

  BOD     pivot;          // pivot point kosti

  GLMATRIX    m;
  JOINT_KEYS *p_keys;
  
  struct _JOINT *p_child; // linearni seznam child kosti
  struct _JOINT *p_next;  // linearni seznam next kosti
  
} JOINT;


typedef struct _JOINT_ANIMACE {

  char   jmeno[MAX_JMENO];
  
  int    framenum;

  int   *p_flag;
  int    flag;          //GK_LOOP a spol.
  int    konec;         //flag konce

  int    deb_kn;
  float  deb_time;

  dword  time;          // aktualni cas animace
  dword  time_start;    // aktualni start
  dword  time_stop;     // aktualni stop
  dword  time_delka;    // aktualni delka animace
  
  dword  start;
  dword  stop;

  int    stav;

  BOD    *p_pos_in;
  BOD    *p_pos_out;
  BOD    *p_norm_in;
  BOD    *p_norm_out;
  
  int    *p_k2flag;  // k2 flag kontejneru
  int    *p_kkflag;  // kflag kontejneru
  int    *p_koflag;  // kflag objektu

  JOINT  *p_child;
  struct _JOINT_ANIMACE *p_next; // dalsi animace v seznamu objektu
  struct _JOINT_ANIMACE *p_game_next; // dalsi animace v seznamu animaci
  struct _JOINT_ANIMACE *p_game_prev; // predchozi animace v seznamu animaci
  
  struct _JOINT_ANIMACE *p_next_gen; // dalsi animace v seznamu pro generovani
  void   *p_obj;                     // tmp pointer na objekt

} JOINT_ANIMACE;

#define O_INDEX_POCET   0
#define O_INDEX_TYP     1
#define O_INDEX_DATA    2

// Konstni animace objektu
typedef struct _EDIT_OBJEKT {

  char          jmeno[MAX_JMENO];
  char          jmeno_parent[MAX_JMENO];
  int           Objekt_ID;        // ID objektu

  word          vertexnum; 
  OBJ_VERTEX   *p_vertex;
  BOD          *p_vertex_kosti; // kostni vertexy

  word          facenum;
  word         *p_face;
  dword        *p_fsous;
  GLenum        face_typ;    // typ facu

  int           optnum; // optimalizovane facy
  int          *p_opt;
  
  word          linenum;
  word         *p_line;
  GLenum        line_typ;    // typ car

  int           material;    // pouzity material (z tabulky materialu)
  int           material_sub;

  float         float_scale; // aktualni float-scale
  float         float_scale_last; // minuly float scale

  word          x_rect;     // x ctvercu objektu
  word          y_rect;     // y ctvercu objektu

  byte          kreslit;

  BOD           pivot;      // pivot-point objektu (relativni)

  byte          trans;      // transformovane svetlo (je preveden na billboard a spocitane okoli)
  dword         m1flag;     // flagy objektu
  dword         m2flag;     // flagy objektu
  dword         oflag;
  
  TRACK_INFO   *p_track;    // track info k animaci  
  GLMATRIX      m;          // matrix k animacim
  
  OBB           obb;

  void                *p_tmp_top;  // pomocny pointer na kontejner
  struct _EDIT_OBJEKT *p_tmp_next; // pomocny pointer na dalsi objekt
  
  JOINT_ANIMACE       *p_joit_animace; // pointer na joint-animace

  struct _EDIT_OBJEKT *p_lod;      // LOD objekty tohodlenctoho

  SIMPLE_TRACK_INFO   *p_tmp_sim;

} EDIT_OBJEKT;

typedef struct _BUNKA_ZEME {
  
  int  s;     // preskocit bodu
  int  dx,dy; // bodu
  word o;
    
} BUNKA_ZEME;


#define MAX_KONT_OBJEKTU   3000
#define MAX_KONT_TEXTUR    200
#define MAX_KONT_MATERIALU 200

typedef struct _EDIT_KONTEJNER {

 char         jmeno[MAX_JMENO];
 int          kontejner_ID;  // identifikacni cislo kontejneru -> jedinecne pri ukldani sceny
 EDIT_OBJEKT *p_obj[MAX_KONT_OBJEKTU];
 
 GLMATRIX     world;     // hlavni modifikovaci matice (scale a pod -> nulova transfromace na stred)
 GLMATRIX     mpmatrix;  // pozicni matice - aktivni pouze pri ukladani/nahravani
 
 int          max_objektu;
 byte         kreslit;
 
 int          prvek;      // pokud prvek -> je to logicky prvek k beruskam == K_CHYBA -> neni to prvek

 dword        m1flag;     // slepenec vsech materialu
 dword        m2flag;     // slepenec vsech materialu
 dword        kflag;      // slepenec vsech objektu
 dword        k2flag;

 int          facu;
 int          bodu;
 int          objektu; 

 int          mailbox; 
 int          kont;
 int          low_id;     // identifikator low/top-kontejneru
 
 OBB           obb;

 MLZNA_KOSTKA *p_mlha;    // pointer na mlznou kostku kontejneru

 HIERARCHY_SIM   sim[KONT_MAX_ANIM];  // seznam animaci - sim-animace
 int             akt_animace;    // aktualne vybrana animace 

 struct _STATIC_LIGHT    *p_slight; // seznam statickych svetel
 struct _DYN_LIGHT       *p_dlight; // seznam dynamickych svetel
 struct _EXTRA_DYN_LIGHT *p_edlight; // seznam extra svetel

 struct _EDIT_KONTEJNER  *p_next;     // dalsi kontejner v multi-mesh kontejnerech
 struct _EDIT_KONTEJNER  *p_next_low; // dalsi kontejner v low hierarchii
 struct _EDIT_KONTEJNER  *p_next_top; // top-kontejner

} EDIT_KONTEJNER;

// Seznam fleku v levelu
typedef struct _FLEK {

  BOD        p;            // pozice fleku
  BOD        nx,nz;        // normalovy vektor fleku
  float      u[4];         // mapovaci souradnice u
  float      v[4];         // mapovaci souradnice v
  int        material;     // material fleku  
  int        uroven;
  float      bias;
  int        mesh;
  int        flag;  

  struct _FLEK *p_next;
  struct _FLEK *p_prew;

} FLEK;

#define LANI_FRONTA   6

typedef struct _GAME_MESH_ANIMACE {

  int                update;               // update animacniho stromu

  int                akt[LANI_FRONTA];     // cislo aktivni animace, -1 pro zadnou
  int                odkaz;                // odkaz do tabulky

  int                flag[LANI_FRONTA];    // flag animace
  SIMPLE_TRACK_INFO *p_sim[LANI_FRONTA];   // vrchni animace animacniho stromu
  
  dword              time;                 // cas animace (0-1)
  dword              time_stop;
  dword              time_start;           // start-time animace
  dword              time_delka;

  int               *p_flag[LANI_FRONTA];  // kofoluv flag

  dword              start[LANI_FRONTA];   // frame start
  dword              stop[LANI_FRONTA];    // frame konec

} GAME_MESH_ANIMACE;


#define   GAME_MESH_RANI   0x1  //Game-mesh se animuje v sim_animaci

typedef struct _EXTRA_FACE {

  int      typ;
  word     facenum;
  word    *p_face;

} EXTRA_FACE;

typedef struct _GAME_MESH_DATA {

 void                    *p_mesh;      // tatik-mesh

 dword                    m1flag;      // flag materialu 1
 dword                    m2flag;      // flag materialu 2
 dword                    kflag;       // flag kontejneru
 dword                    k2flag;      // druhy flag kontejneru
 int                      mesh_handle; // handle meshe
 int                      mesh_ID;
 
 // Svetla meshe
 void                   **p_light;     // pole svetel meshu
 int                      slightnum;
 int                      lightmax;    // velikost pole na svetla
 int                      lightakt;    // aktualni velikost

 // Extra-dynamicke svetla meshe
 int                      top_edlight; // top_dynamicke svetlo
 EXTRA_DYN_LIGHT        **p_edlight;
 int                      edlightnum;  // aktualni pocet svetel
 int                      edlightmax;  // maximalni pocet svetel

 // Loadovaci svetla meshe -> kopie z kontejneru
 struct _STATIC_LIGHT    *p_lslight;   // seznam statickych svetel
 struct _DYN_LIGHT       *p_ldlight;   // seznam dynamickych svetel
 struct _EXTRA_DYN_LIGHT *p_lelight;   // seznam extra svetel

} GAME_MESH_DATA;

typedef struct _GAME_MESH { //celej kontejner

 char                     jmeno[MAX_JMENO];

 dword                    vertexnum;             // pocet vsech vertexu
 BOD                     *p_vertex_pos;          // pozice bodu (po trojicich)
 BOD                     *p_vertex_pos_kosti;    // pozice bodu (po trojicich) pro kostni transformaci
 BOD                     *p_vertex_norm;         // normalove vektory
 BOD                     *p_vertex_norm_kosti;   // normalove vektory pro kosti
 BODRGBA                 *p_vertex_diff_material;// materialove difusni barvy
 BODRGBA                 *p_vertex_diff;         // vysledne difusni barvy
 BODRGBA                 *p_vertex_diff_voda;    // vysledne difusni barvy vody
 BODRGB                  *p_vertex_spec_material;// materialove spekularni barvy
 BODRGB                  *p_vertex_spec;         // vysledne spekularni barvy
 BODRGB                  *p_vertex_spec_voda;    // vysledne spekularni barvy
 BODUV                   *p_vertex_uv1_material; // materialove text-koordinaty 1
 BODUV                   *p_vertex_uv2_material; // materialove text-koordinaty 2
 BODUV                   *p_vertex_uv3_material; // materialove text-koordinaty 3
 BODUV                   *p_vertex_uv4_material; // materialove text-koordinaty 4

 int                      objektu;     // pocet objektu v meshi - pocet pointeru v extra face
 int                     *facenum;     // pocty facu v extra-facich - polich
 int                     *facestart;   // starty facu jednotlivych objektu
 int                     *p_face;
 int                      facevel;     // velikost bloku plosek

 word                    *objektnum;   // pocty vertextu v objektech
 word                    *objektstart; // starty vertextu objektu

 OBB                     *p_obb_local; // lokalni obalky objektu
 OBB                     *p_obb_world; // obalky transformovane do world-space
 OBB                      obb_local;   // lokalni obalka kontejneru
 OBB                      obb_world;   // world-space obalka kontejneru
 
// EDIT_MATERIAL          **p_mat;       // pole pointeru na materialy na jednotlive objekty
 int                     *p_mat;       // odkaz na meshe
 MLZNA_KOSTKA            *p_mlha;      // mlzna kostka meshe
 
 GLMATRIX                *p_key;       // klicove animacni matice pro kazdy objekt 
 int                     *p_Objekt_ID; // ID objektu
 GLMATRIX                 m;           // top-level matice
 GLMATRIX                 world;       // world-transformace

 GAME_MESH_ANIMACE        siminfo;     // informace o aktualni animaci
 SIMPLE_TRACK_INFO       *p_sim;       // seznam vsech top-level animaci
 int                      simnum;      // pocet simu
 
 dword                   *p_kflag;     // kont-flagy pro urcite kontaky
 
 float                   *p_normal_scale;     // scale normalovych vektoru
 float                   *p_normal_scale_last;// posledni scale normalovych vektoru
 float                   *p_normal_y;         // puvodni y objektu

 // Aktualni (vybrana) animace meshe a jeji cas
 SIMPLE_TRACK_INFO       *p_sim_aktivni;   // aktivni animace 
 dword                    sim_aktivni_time;// jeji aktualni cas
 dword                    sim_flag;        // flag meshe

 int                      low_id;      // low-id meshe
 struct _GAME_MESH       *p_next;      // pointer na dalsi mesh v poradi

 GAME_MESH_DATA          *p_data;      // stejne data pro vsechny meshe v multimeshu

 VERTEX_ARRAYS            varray;      // vertex-arrays

 // kazdy objekt ma svoji joint-animaci (pokud to je joint-anim kontejner)
 JOINT_ANIMACE          **p_joint_anim;   // seznam joint animaci

 BOD                      kofola_pivot; // kofolovy pivoty
 int                      mail;
 int                      nekreslit;

} GAME_MESH;

typedef struct _POZICE_BODU_2D {

  float x,y,z,w;  // transformovane souradnice
  
} POZICE_BODU_2D;

typedef struct _TEXT_KOORD {
 
  float     x,y,z;
  float     nx,ny,nz;

  float     tu1,tv1;  // souradnice prvni textury
  float     tu2,tv2;  // souradnice druhe textury
  float     tu3,tv3;  // souradnice druhe textury
  float     tu4,tv4;  // souradnice druhe textury
  float     tul,tvl;  // souradnice light-mapy
  
  float     dr, dg, db,  da;// difusni barva barva r g b a
  float     mdr,mdg,mdb,mda;// difusni barva barva r g b a
  float     sr, sg, sb;     // spekularni barva barva r g b
  float     msr,msg,msb;    // spekularni barva barva r g b  

} TEXT_KOORD;

typedef struct _LIGHTMAP_FACE {

  int      last_u;
  int      upload;      // face se ma uploadovat do videokarty
  bitmapa *p_bmp;       // pointer na bitmapu textury
  bitmapa *p_zal;       // pointer na zalozni bitmapu textury

  ROVINA   n,rv,ru;     // rovina plosky, rovina vektoru r, rovina vektoru u
  BOD      s0,s1,s2,s3; // body na koncich ligthmapy
  float    ddu,ddv;     // delka vektoru u,v
  
  int      u,v;         // start body lightmapy
  int      nu,nv;       // pocet kroku

} LIGHTMAP_FACE;

// pouze na staticke objekty
typedef struct _EDIT_MESH_POLY { //celej kontejner
  
 char         jmeno[MAX_JMENO]; // jmeno poly

 int          facenum;   // co face to objekt
 TEXT_KOORD  *p_koord;   // list texture koordinatu - plosky po 3

 LIGHTMAP_FACE *p_ind;   // lightmapovaci indicie, je jich facenum/3
 dword       *p_fsous;   // sousedni plosky
 dword        m1flag;    // flag materialu -> stejne jako u kontejneru
 dword        m2flag;    // flag mat 2 -> stejne jako u kontejneru
 dword        kflag;     // flag kontejneru
 dword        k2flag;     // flag kontejneru
 
 int          material;  // material poly
 int          kont;      // cislo kontejneru, ze ktereho bylo poly vyrobeno
 int          poly;      // poradove cislo poly v seznamu polyu

 MLZNA_KOSTKA *p_mlha;      // mlzna kostka polyho

 int          lightnum;    // pocet lightmap
 int         *p_lightnum;  // pocty light-map
 EDIT_TEXT  **p_light;     // pole  light-map
 
 byte         kreslit;
 int          mail;        // mailbox poly :)

 OBB          obb;

 // Svetla poly - dynamicke 
 void       **p_dlight;  // pole svetel meshu 
 int          lightmax;  // velikost pole na svetla
 int          lightakt;  // aktualni vekikost

 // Svetla poly - extra-dynamicke
 int          top_edlight;// top-svetlo
 void       **p_edlight;  // pole extra svetel meshu 
 int          edlightmax; // velikost pole na extra svetla
 int          edlightakt; // aktualni velikost

 VERTEX_ARRAYS varray;    // vertex-arrays

} EDIT_MESH_POLY;


typedef struct _PRVEK_DATABAZE {  // prvek, ktery je v aktivni databazi prvku
  
  int kont;
  int guid;
  int kof_index;
  EDIT_KONTEJNER *p_kont;
  
} PRVEK_DATABAZE;

typedef struct _PRVEK_LEVELU {  // prvek, ktery je umisteny v levelu

  int       lev_index;             // pozice prvku v levelu
  int       prv_index;             // index v seznamu prvku v levelu
  GLMATRIX  m;                     // matrix kontejneru pokud je dynamicky
  PRVEK_DATABAZE *p_prv_databaze;  // odkaz do databaze prvku
  int       mesh;                  // cislo kontejneru pro kresleni prvku
  int       Vazba1;                // jedninecne vlastnosti prvku
  int       Vazba2;
  int       Rotace;
  int       guid;                  // guid prvku

} PRVEK_LEVELU;

typedef struct _PRVEK_LEVELU_GAME {  // prvek, ktery je umisteny v levelu

  int        staticky;           // je staticky ?
  float      x,y,z;              // fyzicky 3D stred objektu
  int        rotace;             // natoceni prvku

  int        guid;               // guid prvku
  
  GLMATRIX   mg;                 // animacni matice (1 pri stani)
  GLMATRIX  *p_mg_top;           // top-animacni matice v anim strome
  GLMATRIX   mp;                 // pozicni matice (umistuje prvek na pozici)
  BOD        pivot;              // pivot-point pro animace (kopiruje se z animaci)
    
  GAME_MESH_DATA *p_mesh_data;     // mesh-data jsou stejne
  int        mesh;
  int        anim_mail;

  int        flag;               // flag prvku (low/top) prvek
  struct _PRVEK_LEVELU_GAME *p_next_low; // pointer na dalsi low/top prvek v poradi
  struct _PRVEK_LEVELU_GAME *p_prev_low; // pointer na predchozi low/top prvek v poradi

} PRVEK_LEVELU_GAME;

#define TEXT_DIRS 20

typedef struct _KONFIG_DIR {

 char    prvky_dir[100];
 char    level_dir[100];
 
 char    out_dir[100];
 char    material_dir[100];

 char    data_dir[100];
 char    scene_dir[100];

 char    game_level_dir[100];
 char    game_data_dir[100];
 char    edit_level_dir[100];

} KONFIG_DIR;

typedef struct _TEXT_DIR {

  char    texture_dir[TEXT_DIRS][100];   // adresare textur
  int     texture_dir_class[TEXT_DIRS];
  char    texture_file[TEXT_DIRS][100];  // soubory textur
  int     texture_file_class[TEXT_DIRS]; // tridy textur v paku
  char    scene_texture_dir[100];

} TEXT_DIR;

#define SCENE_GAME_CUBE     0x1 // scena obsahuje game cube

#define COLOR_SET           0
#define COLOR_ADD           1
#define COLOR_SUB           2

#define DEFAULT_ALFA_FAKTOR 0xffffffff

typedef struct _TRIANGLE {
  OBJ_VERTEX a,b,c;
} TRIANGLE;

typedef struct _KAMERA {

  int   cislo; // "use" - flag nebo K_CHYBA
  char  jmeno[MAX_JMENO]; // jmeno kamery
  int   flag;  // 0 - nic, 1,2 -> kreslit osy
  
  /* Pocatecni hodnoty kamery
  */
  BOD   p,t;   // umisteni kamery
  float fov;   // field-of-view
  float roll;  // nejakej jinej ?
  
  /* Objekty kamery
  */
  EDIT_OBJEKT *p_objekt; // objekt kamery
  EDIT_OBJEKT *p_target; // cil kam kamera ukazuje
  EDIT_OBJEKT *p_pos;    // pozice kamery

  /* Animace kamery
  */
  KAMERA_TRACK_INFO tracky[KAM_MAX_ANIM]; //animace kamer
  int               akt_animace; // aktualne vybrana animace kamery

} KAMERA;

#define SLOZ_FLARE  14

typedef struct _LENS_FLARE_SLOZ {
  
  float vzdal;     // vzdalenost flare (0 = neaktivni)
  float r,g,b,a;   // barva lensu
  float dx,dy;     // rozmery dx,dy
  int   material;
  float wx,wy,wz;
  int   vid;
  EDIT_MATERIAL  *p_mat;  // material flaru

} LENS_FLARE_SLOZ;

typedef struct _LENS_FLARE {

  byte  akt;
  BOD   p;         // kde je ve 3d  
  BOD  *p_bod;     // pointer na bod
  float r,g,b,a;   // barva lensu
  float dx,dy;     // rozmery dx,dy
  int   material;
  EDIT_MATERIAL  *p_mat;  // material flaru
  int   index;

  STATIC_LIGHT *p_svetlo; // staticke svetlo flaru

  float wx,wy,wz;  // 2D souradnice
  int   vid;
  int   alfa;      // alfa mod
  int   rflag;     // renderivaci flag -> jak kreslit osy
  int   zflag;     // z-test flag

  LENS_FLARE_SLOZ *p_sloz; // zbytek slozenych flaru

  struct _LENS_FLARE *p_next; // pointer na dalsi flare
  struct _LENS_FLARE *p_prev; // pointer na predchozi flare

} LENS_FLARE;

// flagy pro lens-flary
#define  FLR_NO_ZTEST   0x1

typedef struct _NASTAVENI_KAMER {  

  float fov;
  float far_plane;
  float near_plane;

  float max_uhel;
  float min_uhel;

  float max_vzdal;
  float min_vzdal;

  BOD   min;
  BOD   max;
  int   flag;

} NASTAVENI_KAMER;

typedef struct _ZEMNI_MLHA {

  int   aktivni;

  float v_start;
  float v_stop;

  float h_start;
  float h_stop;

  dword barva;

  int   handle;

} ZEMNI_MLHA;

typedef struct _LEVEL_KONFIG {

  dword      barva_pozadi; // pouzit barvu pozadi
  int        barva_pozadi_aktivni; // aktivni
  ZEMNI_MLHA zm;           // nastaveni zemni mlhy

} LEVEL_KONFIG;

typedef struct _OBB_TREE_ITEM {

  OBB              *p_obb;
  EDIT_MESH_POLY   *p_poly;
  GAME_MESH        *p_mesh;
  int               mesh;
  int               viditelny;
  int               zrc_viditelny;

} OBB_TREE_ITEM;

typedef struct _OBB_TREE {

  int                mail;  
  OBB                obb;

  int                itnum;     // polozky stromu
  OBB_TREE_ITEM     *p_item;

  struct _OBB_TREE  *p_up;      // otec stromu
  int                nextnum;   // pocet podobalek
  struct _OBB_TREE  *p_next;    // podobalky
  
} OBB_TREE;


/*
  U skladaneho potrebuju: 
*/
inline void transformuj_bod_matici(float *p_x, float *p_y, float *p_z, GLMATRIX *m)
{
 float x,y,z;

 x = *p_x; y = *p_y; z = *p_z;
 
 *p_x = m->_11*x + m->_21*y + m->_31*z + m->_41;
 *p_y = m->_12*x + m->_22*y + m->_32*z + m->_42;
 *p_z = m->_13*x + m->_23*y + m->_33*z + m->_43;
// wp = matSet._14*x + matSet._24*y + matSet._34*z + matSet._44;
}

inline BOD * transformuj_bod_bod_matici(BOD *p_p, GLMATRIX *m)
{
 float x,y,z;

 x = p_p->x; y = p_p->y; z = p_p->z;
 
 p_p->x = m->_11*x + m->_21*y + m->_31*z + m->_41;
 p_p->y = m->_12*x + m->_22*y + m->_32*z + m->_42;
 p_p->z = m->_13*x + m->_23*y + m->_33*z + m->_43;
 
 return(p_p);
}

inline void transformuj_wbod_matici(float *p_x, float *p_y, float *p_z, float *p_w, GLMATRIX *m)
{
 float x,y,z,w;

 x = *p_x; y = *p_y; z = *p_z, w = *p_w;;
 
 *p_x = m->_11*x + m->_21*y + m->_31*z + m->_41*w;
 *p_y = m->_12*x + m->_22*y + m->_32*z + m->_42*w;
 *p_z = m->_13*x + m->_23*y + m->_33*z + m->_43*w;
 *p_w = m->_14*x + m->_24*y + m->_34*z + m->_44*w;
}

inline void transformuj_wbod_matici_wbod(WBOD *p_s, GLMATRIX *m, WBOD *p_c)
{
 float x,y,z,w;

 x = p_s->x; y = p_s->y; z = p_s->z, w = p_s->w;;
 
 p_c->x = m->_11*x + m->_21*y + m->_31*z + m->_41*w;
 p_c->y = m->_12*x + m->_22*y + m->_32*z + m->_42*w;
 p_c->z = m->_13*x + m->_23*y + m->_33*z + m->_43*w;
 p_c->w = m->_14*x + m->_24*y + m->_34*z + m->_44*w;
}

inline BOD * transformuj_bod_matici_bod(BOD *p_bod, GLMATRIX *m, BOD *p_cil)
{
 float x,y,z;

 x = p_bod->x; y = p_bod->y; z = p_bod->z;
 
 p_cil->x = m->_11*x + m->_21*y + m->_31*z + m->_41;
 p_cil->y = m->_12*x + m->_22*y + m->_32*z + m->_42;
 p_cil->z = m->_13*x + m->_23*y + m->_33*z + m->_43;
 
 return(p_cil);
}

inline void transformuj_nbod_matici(BOD *p_bod, GLMATRIX *m)
{
 float x,y,z;

 x = p_bod->x; y = p_bod->y; z = p_bod->z;
 
 p_bod->x = m->_11*x + m->_21*y + m->_31*z;
 p_bod->y = m->_12*x + m->_22*y + m->_32*z;
 p_bod->z = m->_13*x + m->_23*y + m->_33*z;
}

inline void transformuj_nbod_matici_bod(BOD *p_bod, GLMATRIX *m, BOD *p_cil)
{
 float x,y,z;

 x = p_bod->x; y = p_bod->y; z = p_bod->z; 
 p_cil->x = m->_11*x + m->_21*y + m->_31*z;
 p_cil->y = m->_12*x + m->_22*y + m->_32*z;
 p_cil->z = m->_13*x + m->_23*y + m->_33*z;
}

inline void transformuj_objekt_matici(EDIT_OBJEKT *p_obj, GLMATRIX *p_mat)
{
  int v;
  for(v = 0; v < p_obj->vertexnum; v++)
    transformuj_bod_matici(&p_obj->p_vertex[v].x,
                           &p_obj->p_vertex[v].y,
                           &p_obj->p_vertex[v].z,
                           p_mat);
}

inline void nastav_material_rgb(RGB_M *p_rgb, float r, float g, float b)
{
 p_rgb->r = r;
 p_rgb->g = g;
 p_rgb->b = b;
}

// nastavi materialovou i vyslednou barvu
inline void nastav_barvu_bodu_obj(EDIT_OBJEKT *p_obj, float r, float g, float b, float a)
{
  int v;
  for(v = 0; v < p_obj->vertexnum; v++) {
    p_obj->p_vertex[v].mdr = p_obj->p_vertex[v].dr = r;
    p_obj->p_vertex[v].mdg = p_obj->p_vertex[v].dg = g;
    p_obj->p_vertex[v].mdb = p_obj->p_vertex[v].db = b;
    p_obj->p_vertex[v].mda = p_obj->p_vertex[v].da = a;
  }
}

inline void nastav_barvu_bodu_obj_dword(EDIT_OBJEKT *p_obj, dword barva)
{
  int v;
  float b[4];
  rgba_float(barva,b);
  for(v = 0; v < p_obj->vertexnum; v++) {
    p_obj->p_vertex[v].mdr = p_obj->p_vertex[v].dr = b[0];
    p_obj->p_vertex[v].mdg = p_obj->p_vertex[v].dg = b[1];
    p_obj->p_vertex[v].mdb = p_obj->p_vertex[v].db = b[2];
    p_obj->p_vertex[v].mda = p_obj->p_vertex[v].da = b[3];
  }
}

inline void add_barvu_bodu_obj(EDIT_OBJEKT *p_obj, float r, float g, float b, float a)
{
  int v;
  for(v = 0; v < p_obj->vertexnum; v++) {    
    add_color_fa(&p_obj->p_vertex[v].mdr, r, g, b, a);
    add_color_fa(&p_obj->p_vertex[v].dr, r, g, b, a);
  }
}

inline void sub_barvu_bodu_obj(EDIT_OBJEKT *p_obj, float r, float g, float b, float a)
{
  int v;
  for(v = 0; v < p_obj->vertexnum; v++) {
    sub_color_fa(&p_obj->p_vertex[v].dr,r,g,b,a);
    sub_color_fa(&p_obj->p_vertex[v].mdr,r,g,b,a);
  }
}


inline void nastav_barvu_bodu_kont(EDIT_KONTEJNER *p_kont, float r, float g, float b, float a, byte op)
{
  int o,mo;

  mo = p_kont->max_objektu;
  for(o = 0; o < mo; o++) {
    if(p_kont->p_obj[o]) {
      switch (op) {
      case 0: //rep
        nastav_barvu_bodu_obj(p_kont->p_obj[o],r,g,b,a);
        break;
      case 1: //add
        add_barvu_bodu_obj(p_kont->p_obj[o],r,g,b,a);
        break;
      case 2: //sub
        sub_barvu_bodu_obj(p_kont->p_obj[o],r,g,b,a);
        break;
      }
    }
  }
}
/*
inline void nastav_barvu_bodu_kont_alfa(EDIT_KONTEJNER *p_kont, float a)
{
  int o;

  for(o = 0; o < p_kont->max_objektu; o++) {    
    if(p_kont->p_obj[o]) {
      oe_set_barva_objekt_maska(p_kont->p_obj[o],1,1,1,a,0,COLOR_SET,0,0,0,1);
    }
  }
}
*/

inline void nastav_barvu_bodu_kont_dword(EDIT_KONTEJNER *p_kont, dword barva, byte op)
{
  int o,mo;
  float v[4];
  rgba_float(barva,v);
  mo = p_kont->max_objektu;
  for(o = 0; o < mo; o++) {
    if(p_kont->p_obj[o]) {
      switch (op) {
      case 0: //rep
        nastav_barvu_bodu_obj(p_kont->p_obj[o],v[0],v[1],v[2],v[3]);
        break;
      case 1: //add
        add_barvu_bodu_obj(p_kont->p_obj[o],v[0],v[1],v[2],v[3]);
        break;
      case 2: //sub
        sub_barvu_bodu_obj(p_kont->p_obj[o],v[0],v[1],v[2],v[3]);
        break;
      }
    }
  }
}

inline void transformuj_mesh_matici(GAME_MESH *p_mesh, GLMATRIX *p_m)
{ 
 int i,pocet = p_mesh->vertexnum;
 BOD *p_vert = p_mesh->p_vertex_pos;
 for(i = 0; i < pocet; i++) {
   transformuj_bod_matici(&p_vert[i].x, &p_vert[i].y, &p_vert[i].z, p_m);
 }
}

GLMATRIX __kont_world_matrix;

inline GLMATRIX * kont_world_matrix(EDIT_KONTEJNER *p_kont)
{
  if(p_kont->kflag&KONT_MPMATRIX) {
    return(mat_mult_dir(&p_kont->world,&p_kont->mpmatrix,&__kont_world_matrix));
  } else {
    return(&p_kont->world);
  }
}

#define DDRGB(r, g, b)     (0xff000000 | (((long)((b) * 255)) << 16) |(((long)((g) * 255)) << 8) | (long)((r) * 255))
#define DDRGBA(r, g, b, a) ((((long)((a) * 255)) << 24) | (((long)((b) * 255)) << 16) | (((long)((g) * 255)) << 8) | (long)((r) * 255))


typedef int  (* STAGE_FUNC)(EDIT_MATERIAL *p_mat, EDIT_STATE_TEXT_BLOK *p_stage, MATERIAL_TEXT *p_text, int text, int rezerva);
typedef void (* STAGE_FUNC_ELEM)(void);
typedef void (* STAGE_FUNC_ALFA)(void);

void kont_norm_vect(EDIT_KONTEJNER *p_kont_top);
void transformuj_kontejner_matici(EDIT_KONTEJNER *p_kont, GLMATRIX *p_m);
void transformuj_kontejner_keyframe(EDIT_KONTEJNER *p_kont);
void transformuj_kontejner(EDIT_KONTEJNER *p_kont);
void calc_norm_vec_inter_object(EDIT_OBJEKT *p_obj);
//void mesh_norm_vect(GAME_MESH *p_mesh);
void texturuj_zemi(EDIT_OBJEKT *p_obj, int rect_x, int rect_y, int c_text, int flag, int mat, float skok_u, float skok_v);
EDIT_OBJEKT * vyrob_zemi(int rect_x, int rect_y, float skok);
EDIT_OBJEKT * vyrob_zemi_start(int rect_x, int rect_y, float skok, float start_x, float start_y);
void rozloz_zemi(EDIT_OBJEKT **p_src, int ctvercu);
EDIT_OBJEKT * mirror_objekt(EDIT_OBJEKT *p_src, int st, int mx, int my, int mz);
void mirror_text(EDIT_OBJEKT *p_src, int t, int mu, int mv);
void rotuj_text(EDIT_OBJEKT *p_obj, int t, int uhel);
void dopln_cary(EDIT_OBJEKT *p_obj);
void kresli_ctverec_2d(int x1, int y1, int x2, int y2, float r, float g, float b);
void kresli_ctverec_2d_plnej(int x1, int y1, int x2, int y2, float r, float g, float b);
void kresli_kurzor_mysi_kriz(int xres, int yres);
EDIT_OBJEKT * vyrob_krychli(BOD *p_bod, float x, float y, float z, dword barva);
EDIT_OBJEKT * vyrob_krychli_dratovou(BOD *p_bod, float x, float y, float z, dword barva);
void krychli_dratovou_recalc(EDIT_OBJEKT *p_obj, BOD *p_bod, float x, float y, float z, dword barva);
EDIT_OBJEKT * kameru_dratovou_calc(EDIT_OBJEKT *p_obj, BOD *p_p, BOD *p_t, float fov, float roll, dword barva);
EDIT_OBJEKT * kameru_dratovou_vyrob(BOD *p_p, BOD *p_t, float fov, float roll);
void kameru_dratovou_reanimuj(KAMERA *p_kam);
void kameru_dratovou_recalc(EDIT_OBJEKT *p_obj, BOD *p_p, BOD *p_t, float fov, float roll);
void invertuj_face_objektu(EDIT_OBJEKT *p_obj);
void invertuj_face_kontejner(EDIT_KONTEJNER *p_kont);
EDIT_KONTEJNER * vyrob_krychli_rozlozenou(BOD *p_bod, float x, float y, float z);
void pridej_krychli(EDIT_OBJEKT **p_src, BOD *p_bod, float x, float y, float z);
EDIT_OBJEKT * vyrob_billboard(BOD *p_bod, float x, float y);
void objekt_obalka(EDIT_OBJEKT *p_obj, GLMATRIX *p_m, BOD *p_min, BOD *p_max);
void kontejner_obalka(EDIT_KONTEJNER *p_kont);
int  cti_velikost_objektu(EDIT_OBJEKT *p_obj);
void cti_velikost_kontejneru(EDIT_KONTEJNER *p_kont, int *p_size, int *p_objektu);
int  intersect_ray_koule(BOD *p_stred, float radius, BOD *p_orig, BOD *p_dir);
void obj_vyrob_list_sousednosti(EDIT_OBJEKT *p_obj);
void obj_vyrob_list_sousednosti_full(EDIT_OBJEKT *p_obj);
void calc_norm_vec(EDIT_OBJEKT *p_obj);
void kresli_oktanovou_bunku(BOD *p_str, BOD *p_vel);
void kresli_oktanovou_bunku_minmax(BOD *p_min, BOD *p_max, dword barva);
void kresli_objekt_dratove_jednoduse(EDIT_OBJEKT *p_obj);
void nastav_material_rgb_MAT(RGB_M *p_rgb);
void nastav_material(float r, float g, float b);
EDIT_OBJEKT * umisti_krychli_dratovou(EDIT_OBJEKT *p_obj, BOD *p_bod);
void kresli_caru(BOD *p_s, BOD *p_k, dword barva);
int  vyrob_kameru_indir(KAMERA *p_kam, char *p_jmeno, BOD *p_p, BOD *p_t, float fov, float roll);
void kresli_kosoctverec(GLMATRIX *p_project, GLMATRIX *p_camera, GLMATRIX *p_world, BOD *p_p, float r, float dr, float dg, float db);
void transformuj_objekt_text_coord(EDIT_OBJEKT *p_obj, GLMATRIX *p_mat, int coord);
void transformuj_kontejner_text_coord(EDIT_KONTEJNER *p_kont, GLMATRIX *p_mat, int coord);
void mesh_env_maping_camera(GAME_MESH *p_mesh, GLMATRIX *p_cam);
void poly_env_maping_camera(EDIT_MESH_POLY *p_poly, EDIT_MATERIAL *p_mat, GLMATRIX *p_cam);
void transformuj_mesh(GAME_MESH *p_mesh);
void kresli_ctverec_2d_diff(int x1, int y1, int dx, int dy, float r, float g, float b);
inline int  stejny_vertex_point(BOD *v1, BOD *v2);
inline int  stejny_vertex_point_delta(BOD *v1, BOD *v2, float delta);
void mesh_env_maping_spec(GAME_MESH *p_mesh, GLMATRIX *p_cam, EDIT_MATERIAL **p_mt);
void poly_env_maping_spec(EDIT_MESH_POLY *p_poly, EDIT_MATERIAL *p_mat, GLMATRIX *p_cam);
void kresli_kosoctverec_word(BOD *p_p, float r, dword barva);
void poly_obalka(EDIT_MESH_POLY *p_poly, GLMATRIX *p_mat, BOD *p_min, BOD *p_max);
int  intersect_poly(EDIT_MESH_POLY *p_poly, BOD *p_orig, BOD *p_dir);
void kontejner_obalka_aabb(EDIT_KONTEJNER *p_kont, BOD *p_min, BOD *p_max);
//int  intersect_mesh(GAME_MESH *p_mesh, BOD *p_orig, BOD *p_dir);
int  intersect_mesh_objekt(GAME_MESH *p_mesh, int o, BOD *p_orig, BOD *p_dir);

inline void oe_olist_reset(int *o)
{
  *o = -1;
}

inline EDIT_OBJEKT * oe_olist_next(EDIT_KONTEJNER *p_kont, int *o)
{   
  EDIT_OBJEKT *p_obj;
  if(p_kont) {
    while((*o)+1 < MAX_KONT_OBJEKTU) {
      p_obj = p_kont->p_obj[++(*o)];
      if(p_obj)
        return(p_obj);      
    }
  }
  return(NULL);
}

inline float randf(void)
{
  return((float)rand()/(float)(RAND_MAX));
}

/*
  Look-up tabulka
*/
#define MAX_LOOK_TABLE 256
void konfiguruj_look_up(int *p_looksqrt_linear, int *p_looksqrt_quadratic, float *p_looksqrt_linear_float, float *p_looksqrt_quadratic_float);

// Minimalni intenzita svetla
#define DELTA_MIN 0.00392156f

inline void * kopiruj_pole(void *p_src, int velikost)
{
  if(p_src) {
    void *p_mem = mmalloc(velikost);
    memcpy(p_mem,p_src,velikost);
    return(p_mem);
  } else {
    return(NULL);
  }
}


#endif
