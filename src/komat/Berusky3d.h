/*
  Berusky GL
*/

#ifndef __BERUSKY_3D_H
#define __BERUSKY_3D_H

typedef struct _KOFOLOVA_CARA
{

  BOD p1, p2;
  BODRGB b1, b2;

} KOFOLOVA_CARA;

typedef struct _VODOVERTEX
{

  BOD *p_pos;
  BODRGBA *p_diff_mat;
  BODRGBA *p_diff;
  BODRGB *p_spec_mat;
  BODRGB *p_spec;
  float vyska;                  // puvodni vyska vertexu (pricita se k ni aktualni)
  float sinus;                  // argument sinusu (podle toho se udelaji barvy)

} VODOVERTEX;

typedef struct _VODNIK
{

  VODOVERTEX *p_vert;
  float *p_vzdal;
  int vertexnum;

  int flag;                     // flagy vlnotvoru
  BOD p;                        // umisteni vlnotvoru

  float fi;                     // poc. faze
  float perioda;                // frekvence
  float amplituda;              // amplituda kmitu
  float dfi;                    // poc. faze
  float dperioda;               // frekvence
  float damplituda;             // amplituda kmitu

  float dr, dg, db, da;
  float adr, adg, adb, ada;
  float sr, sg, sb;
  float asr, asg, asb;

  struct _VODNIK *p_next;

} VODNIK;

typedef struct _VODA2
{

  int meshnum;                  // meshe privazany k vode (co se vlni)
  int meshmax;                  // max pocty meshu ve vodnim dile
  GAME_MESH_OLD **p_mesh;

  int vertexu;
  VODOVERTEX *p_vertexy;

  VODNIK *p_vodnik;

  struct _VODA2 *p_next;
  struct _VODA2 *p_prev;

} VODA2;

typedef struct _GAME_KAMERA
{

  /* Mod kamery - poly/3ds
   */
  int aktivni;

  /* Aktualni nastaveni kamery
   */
  GLMATRIX project, word, camera, invcam;       // inverzni viev matrix s normalizovanejma vektorama  

  /* Aktualni animace
   */
  KAMERA_TRACK_INFO *p_anim;    // animace kamery
  int a_handle;

  /* Aktualni hodnoty kamery - 3ds
   */
  BOD p, t;                     // umisteni kamery
  float fov;                    // field-of-view
  float roll;                   // nejakej jinej ?

  /* Aktualni hodnoty kamery - polarni
   */
  float vzdal;                  // vzdal kamery
  float vzdal_kor;              // korigovana vzdalenost kamery
  float fi;                     // uhel kamery
  float r;                      // rotace kamery  

  int zmena;                    // kamera se zmenila/posunula
  int zmena_last;               // kamera se zmenila/posunula v poslednim framu

  /* Flagy kontrolujici animaci
   */
  int flag;                     // flag animace  

  int time;                     // cas animace (0-1)
  int time_stop;
  int time_start;               // start-time animace
  int time_delka;               // stop-time animace (delka)

  int *p_flag;                  // kofoluv flag

  int start;                    // frame start
  int stop;                     // frame konec

} GAME_KAMERA;


/*
  Material-animacni cast
*/
#define MAX_MAT_ANIMACI 100

// setridim list techto animaci
typedef struct _MATERIAL_ANIM_LIST_BUNKA
{

  EDIT_MATERIAL *p_anim;        // material
  int mesh;                     // odkaz k mesh
  int objekt;                   // odkaz k objektu meshe  

  int next_time;                // cas pristi aktualizace animace
  int event;                    // ceka na udalost

} MAL_BUNKA;

/*
  Kontejner-animacni cast
*/
#define MAX_KONT_ANIMACI 100
#define MAX_SIM_ANIMACI  500
#define MAX_RUN_ANIMACI  200
#define MAX_ANIM_MATIC   20

#define GK_POUZITA       0x1    // Run-animace je pouzivana
#define GK_AKTIVNI       0x10   // Run-animace je aktivni
#define GK_LOOP          0x100  // Run-animace se prehrava furt dokola
#define GK_REMOVE        0x200  // Animace se po dobehnuti automaticky smaze

#define ANIM_FLAG_KONEC  (-1)

typedef struct _GKA_MATICE
{

  GLMATRIX *p_mat;              // matice
  int *p_notify;                // notify flag
  int flagy;                    // flagy
  int fce_param;                // promena predavana funkcim
  void (*funkce_start) (int prom);      // funkce volana na konci animace
  void (*funkce_frame) (int prom);      // funkce volana kazdy frame
  void (*funkce_end) (int prom);        // funkce volana na konci animace

} GKA_MATICE;

typedef struct _GLOBALNI_KONT_ANIMACE
{

  int flag;                     // flag animace
  SIMPLE_TRACK_INFO *p_sim;     // sim-animace

  dword time;                   // cas animace
  dword time_stop;              // stop-cas animace
  dword time_start;             // start-time animace  
  dword time_delka;             // delka animace

  int matic;                    // pocet matic  
  GKA_MATICE p_mat[MAX_ANIM_MATIC];     // animacni matice
  BOD p, s;                     // vysledky aktualni interpolace
  QUAT q;

  int *p_flag;                  // kofoluv flag
  void *p_param;                // kofolova end funkce
  int param;
  int param2;
  END_FUNKCE p_endfce;
  int konec;

  dword start;                  // frame start/stop
  dword stop;

  int next_ah;                  // nastaveni dalsi animace v poradi
  int next_flag;
  int next_start;
  int next_stop;

  struct _GLOBALNI_KONT_ANIMACE *p_next;        // nasledujici polozka renderovaciho seznamu
  struct _GLOBALNI_KONT_ANIMACE *p_prev;        // predchozi polozka renderovaciho seznamu

  // odmazani = setrideni seznamu
} GK_ANIM;

typedef struct _GK_TEXTANIM
{

  int material;
  ANIM_TEXT *p_track;
  GLMATRIX *p_mat;

  int flag;                     // flagy animace (loop a pod)
  int *p_flag;                  // strkani vysleku animovani

  dword time;                   // aktualni cas  
  dword time_start;
  dword time_stop;
  dword time_delka;

  dword start;                  // start
  dword stop;                   // stop

  struct _GK_TEXTANIM *p_next;
  struct _GK_TEXTANIM *p_prev;

} GK_TEXTANIM;

#define  PRUHLEDNY_MESH  0x1
#define  PRUHLEDNY_POLY  0x2
#define  PRUHLEDNY_FLARE 0x3

typedef struct _PRUHLEDNY_OBJEKT
{

  void *p_objekt;               // pointer na objekt
  int typ;                      // mesh/poly/flare
  float vzdal;                  // vzdalenost od kamery
  float vzdal_zrc;              // vzdalenost od kamery

} PRUHLEDNY_OBJEKT;

/*
  Debug-cast
*/
typedef struct _DEBUG_STATISTIKA
{

  int bodu, facu, objektu, meshu;
  int hernich_prvku;
  int poly;
  int alfa_stage_change;
  int text_stage_change;
  int light_meshu;
  int light_vertexu;
  int hnizd, hnizd_vid;
  int lani_vid;
  int vertex_obj;

} DEBUG_STATISTIKA;

typedef struct _EDIT_OKNO
{

  GLMATRIX project, word, camera, invcam;       // inverzni viev matrix s normalizovanejma vektorama  

  BOD p;                        // pomene pro prepocitani kamery
  float vzdal;
  float fi;
  float rfi;
  float roll;

  char fs;                      // full-scrren -> pojeho deaktivaci prekresli zbyvajici okna
  char vp;                      // jakej pouziva view-port

} EDIT_OKNO;

#define SFL_POSUN        0x0001
#define SFL_ROTACE       0x0002
#define SFL_ZOOM         0x0004

typedef struct g_config
{
  // Interface to 3D engine
  AGE_MAIN *p_age;

  GLMATRIX *p_project, *p_word, *p_camera, *p_invcam;   // inverzni viev matrix s normalizovanejma vektorama  

  float *p_vzdal;               // vzdal kamery
  float *p_vzdal_kor;           // korigovana vzdalenost kamery
  float *p_fi;                  // uhel kamery
  float *p_rfi;                 // rotace kamery
  BOD *p_target;                // target-point kamery
  BOD *p_pos;                   // pozice kamery
  float *p_roll;                // roll kamery
  int *p_mod;                   // mod kamery

  int kam_omezeni;

  GAME_KAMERA kamera;
  NASTAVENI_KAMER kam;

  KONFIG_DIR dir;
  TEXT_DIR tdir;

  /* Informace o levelu
   */
  int prvnum;                   // pocet prvku na mape
  PRVEK_LEVELU_GAME *p_prv_lev[MAX_PRVKU_LEVEL];        // seznam  prvku umistenych nekde v levelu
  int hitnum;                   // mrizka levelu-staticke prvky - steny
  int *p_hit_list;
  int prvnum_top;               // pocet top prvku na mape
  PRVEK_LEVELU_GAME *p_prv_lev_top[MAX_PRVKU_LEVEL_TOP];        // seznam top prvku umistenych nekde v levelu
  int prvnum_low;               // pocet low prvku na mape
  PRVEK_LEVELU_GAME *p_prv_lev_low[MAX_PRVKU_LEVEL_LOW];        // seznam low prvku umistenych nekde v levelu
  int x_num;
  int y_num;
  int z_num;                    // rozmery levelu
  float x_start;
  float y_start;
  float z_start;
  BOD level_min;
  BOD level_max;

  /* Seznamy pro kresleni
   */
  int dyn_meshlistnum;
  GAME_MESH_OLD **p_dyn_meshlist[MAX_BERUSKY_KONTEJNERU];

  /* Standartni scena
   */
  int mesh_rendernum;
  int mesh_renderakt;
  GAME_MESH_OLD **p_mesh_renderlist[MAX_BERUSKY_KONTEJNERU];

  int poly_rendernum;
  int poly_renderakt;
  EDIT_MESH_POLY **p_poly_renderlist;

  int meshnum;
  GAME_MESH_OLD *p_mesh[MAX_BERUSKY_KONTEJNERU];        // seznam dynamickych hernich objektu
  int mdatanum;
  GAME_MESH_OLD *p_mdata[MAX_BERUSKY_DATABAZE]; // seznam databaze
  int polynum;
  EDIT_MESH_POLY *p_poly;       // seznam statickych objektu
  EDIT_MATERIAL *p_mat[MAX_CELKEM_MATERIALU];   // seznam materialu
  EDIT_MATERIAL *p_smat;        // scene-material
  EDIT_TEXT p_text[MAX_CELKEM_TEXTUR];  // seznam textur
  EDIT_TEXT p_lightmap[MAX_RAY_TEXTUR]; // seznam lightmap
  OBB_TREE_OLD obbtree;         // obb-tree levelu

  /* Fleky v levelu
   */
  FLEK *p_fleky;
  FLEK *p_fleky_mesh;

  /* Casticove systemy
   */
  PARMETAC *p_par;

  /* Materialove animace -> animace materialu
   */
  MAL_BUNKA p_man[MAX_MAT_ANIMACI];     // materialove animace
  int mannum;

  /* Animacni mail
   */
  int anim_mail;

  /* Seznam animaci materialu
   */
  GK_TEXTANIM *p_textanim;

  /* Globalni kontejnerove anmimace
     a jejich simple tracky
   */
  HIERARCHY_TRACK_INFO p_track[MAX_SIM_ANIMACI];
  SIMPLE_TRACK_INFO p_sim[MAX_SIM_ANIMACI];
  int simnum;

  /* Svazane traky
   */
  GK_ANIM *p_gkanim;            // seznam bezicich animaci
  GK_ANIM p_gkzas[MAX_RUN_ANIMACI];     // zasobnik animaci
  int gknum;

  /* Lokalni animacni traky
   */
  GAME_MESH_OLD *p_lokal[MAX_RUN_ANIMACI];
  int lokalnum;

  /* Animace kamer
   */
  KAMERA_TRACK_INFO kamery[KAM_MAX_ANIM];
  int kamnum;

  /* Animace kosti
   */
  JOINT_ANIMACE *p_chap;        // seznam aktivnich joint-animaci

  /* Flare - linearni seznam
   */
  LENS_FLARE *p_flare;

  /* Staticka svetla
   */
  int slightnum;
  STATIC_LIGHT p_slight[MAX_FLARE_SVETEL];

  /* Dynamicke svetla
   */
  float dl_ambient[4];          // ambientni svetlo pro scenu
  int dl_recalc;                // flag - prepocitat svetla (neco se posunulo)  
  int dl_new;                   // flag - zmenil se pocet svetel (pribylo/ubylo)
  int dl_lightnum;
  int dl_on;                    // zapnute prepocty statickych svetel
  DYN_LIGHT p_dlight[MAX_FLARE_SVETEL];

  /* Extra dynamicke svetla
   */
  int edl_recalc;               // flag - prepocitat svetla (neco se posunulo)  
  int edl_new;                  // flag - zmenil se pocet svetel (pribylo/ubylo)
  int edl_lightnum;
  EXTRA_DYN_LIGHT p_edlight[MAX_FLARE_SVETEL];

  /* Linearni seznam mlznych kostek  
   */
  MLZNA_KOSTKA *p_mlha;
  MLZNA_KOSTKA *p_mlha_akt;     // aktnivni mlzna kostka
  ZEMNI_MLHA zm;                // zemni mlha

  /* Linearni seznam vod
   */
  VODA2 *p_voda;

  /* Zasobnik na pruhledne objektu
   */
  int prhlnum;
  PRUHLEDNY_OBJEKT prhl[MAX_PRHL_OBJEKTU];

  /* Kofolovy cary
   */
  int car_a_kouzel;
  KOFOLOVA_CARA *p_cary;

  /*
     Zrcadlo
   */
  int zrc_akt;                  // zrcadlo aktivni
  ZDRCADLO_DESC zrc;            // struktura zrcadla

#ifdef DEBUG_MOD
  DEBUG_STATISTIKA debug;
  int debug_vypisy;
  int debug_fps;
  int debug_light;
  int debug_frame;
  int debug_draty;
#endif
  FILE *debug_file;

  int TimeBase;                 // cas spusteni programu -> casova zakladna (v milisec)
  int TimeEndLastFrame;         // cas na konci posledniho framu (v milisec)
  int TimeLastFrame;            // delka posledniho framu (obracena hodnota je fps)
  float fps;                    // fps = 1000/TimeLongLastFrame
  int frame;                    // aktualni farme hry


  // predpocitana data
  dword conf_barva_pozadi;
  int conf_barva_pozadi_pouzit;
  int conf_barva_pozadi_pouzit_default;
  int conf_extra_light_vertex;
  int conf_pruhledna_kamera;
  float conf_pruhledna_kamera_radius;
  int conf_zpruhlednovac;
  int conf_menu_render;
  int conf_zrcadlo;
  int conf_p_key;
  int conf_r_key;
  int conf_full_light;
  int conf_full_light_last;

  int conf_start_zpruhlednovac;
  int conf_start_zvyraznovac;

  int conf_ovladani_rohy;
  int conf_ovladani_rohy_default;
  int conf_ovladani_rohy_smer;
  float conf_ovladani_rohy_rychlost;

  int conf_animace_okoli;
  int conf_dyn_light;
  int conf_dyn_rychlost;
  int conf_dyn_light_update_all;
  int conf_dyn_light_beruska;   // TRUE - pouze beruska
  int conf_dyn_light_prvky;     // TRUE - pouze prvky
  int conf_dyn_light_mesh;      // TRUE - meshe
  int conf_dyn_light_poly;      // TRUE - poly  
  int conf_caustic;

  int conf_kurzor_handle;
  int conf_kurzor_mesh;

  int conf_load_tiku;
  int conf_load_tik;
  int conf_load_krok;
  void (*p_tik_fce) (void);


  // look-up tabulka pro druhou odmocninu
#define              MAX_LOOK_TABLE 256
  int p_looksqrt_linear[MAX_LOOK_TABLE * MAX_LOOK_TABLE];
  int p_looksqrt_quadratic[MAX_LOOK_TABLE * MAX_LOOK_TABLE];

  // Jednorozmerne look-up tabulky
  float p_looksqrt_linear_float[MAX_LOOK_TABLE];
  float p_looksqrt_quadratic_float[MAX_LOOK_TABLE];

#define SIN_TABLE_SIZE  360
  // Sinus tabulka
  float sinus_table[SIN_TABLE_SIZE];

  // Flag sceny 1
  int sflag1;
  int mail;

  // environment sceny
  LEVEL_ENVIRONMENT env;

  int ref_pos;
  int ref_rot;
  int ref_zmena;

  // local config
  int system_kurzor;
  int karmin_aktivni;
  int mouse_move;

  char level_name[MAX_FILENAME];

    g_config(void)
  {
    memset(this, 0, sizeof(*this));
    karmin_aktivni = 1;
  }

}
G_KONFIG;

#define OXRES       hwconf.xres
#define OYRES       hwconf.yres
#define OXSTART     0
#define OYSTART     0

void ber_updatuj_fps(G_KONFIG * p_ber);
void ber_konfiguruj_berusky(G_KONFIG * p_ber);
void ber_init_level(G_KONFIG * p_ber);
void ber_klavesy_hra(G_KONFIG * p_ber);
void ber_tiskni_statistiku(G_KONFIG * p_ber);
void ber_prikaz_zvyraznovac(G_KONFIG * p_ber);
void ber_rekonfiguruj_hra(G_KONFIG * p_ber);
void ber_rekonfiguruj_3D(G_KONFIG * p_ber);
void ber_rekonfiguruj_3D_aplikuj(G_KONFIG * p_ber);

#define centruj_kurzor() ber_nastav_kurzor(OXRES/2, OYRES/2)

#define KONST_KVAD_UTLUM 0.00392156862745098039215686274509804f

/* Nastavi kurzor na zadanou pozici
*/
void ber_nastav_kurzor(int x, int y);

/* Vyrobi pozicni matici prvku
*/
inline GLMATRIX *ber_position_matrix(G_KONFIG * p_ber, GLMATRIX * p_m, int x,
  int y, int z, int rotace)
{
  init_matrix(p_m);
  p_m->_41 = (float) (p_ber->x_start + x * X_PRVEK + X_PRVEK / 2);
  p_m->_42 = (float) (p_ber->y_start + (y >> 1) * Y_PRVEK);
  p_m->_43 = (float) (p_ber->z_start + z * Z_PRVEK + Z_PRVEK / 2);
  if (rotace)
    rotate_matrix_y(p_m, -(rotace * PI / 2));
  return (p_m);
}

/* Vyrobi pozicni matici divokeho prvku
*/
inline GLMATRIX *ber_position_matrix_abs(G_KONFIG * p_ber, GLMATRIX * p_m,
  float x, float y, float z, int rotace)
{
  init_matrix(p_m);
  p_m->_41 = x;
  p_m->_42 = y;
  p_m->_43 = z;
  if (rotace)
    rotate_matrix_y(p_m, -(rotace * PI / 2));
  return (p_m);
}

inline void ber_position_level(PRVEK_LEVELU_GAME * p_lev, GLMATRIX * p_m,
  int rotace)
{
  if (p_lev) {
    p_lev->x = p_m->_41;
    p_lev->y = p_m->_42 + (Y_PRVEK / 2);        // stred prvku je vyse
    p_lev->z = p_m->_43;
    p_lev->rotace = rotace;
  }
}

#define ber_pozice_v_levelu(_x, _y, _z,_x_num,_z_num)    ((_x_num)*(_z_num)*(_y) + (_x_num)*(_z) + (_x))
#define ber_pozice_na_disku(_i,_x,_y,_z,_x_num,_z_num)\
{\
 int zb;\
 _y = (_i)/((_z_num)*(_x_num));\
 zb = (_i)%((_z_num)*(_x_num));\
 _z = zb/(_x_num);\
 _x = zb%(_x_num);\
}\


#define PRV_LOW      0x01       // Low-prvek
#define PRV_TOP      0x02       // Tow-prvek
#define PRV_ANIM_END 0x04       // Konec top-animace prvku

#endif
