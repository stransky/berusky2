/*
  Editor k beruskam
*/
/*
  Rozlozeni prvku - v tabulce prvku
*/

#define MAX_EDIT_MATERIALU 1000

typedef struct _JMENO_PRVKU {

  int  trida;
  byte jmeno[100];

} JMENO_PRVKU;


typedef struct _GRID_DATA {

  int         vert;
  DXBOD      *p_v[MAX_PATER];
  byte        kreslit[MAX_PATER];
  byte        akt_patro;
  EDIT_OBJEKT *p_kurzor;
  GLMATRIX    m;
  int         x_kurzor, 
              y_kurzor, 
              z_kurzor;

} GRID_DATA;

typedef struct _KONFIG_PRVKU {

 int   lista_aktivni;
 
 int   akt_prvek; // aktivni prvek v databazi (jeho index)
                 //  ten budu sypat do levelu 
 int   vybr_trida;    // vybrana trida
 int   vybr_podtrida; // vybrana podtrida
 int   vybr_varianta; // vybrana varianta

 HWND  hwnd_list_trida; // hwnd listu materialu
 HWND  hwnd_list_pod_trida; // hwnd listu materialu
 HWND  hwnd_list_varianta; // hwnd listu materialu
 
 HWND  hwnd_dialog; // hwnd listu materialu

} KONFIG_PRVKU;

typedef struct _KONFIG_BERUSEK {

  byte kreslit_dynamicke_objekty;
  byte kreslit_staticke_objekty;
  byte kreslit_stinove_objekty;
  byte kostka;
  byte kostka_remake;
  byte akt_vrstva;
  byte vybrane_patro;
  
} KONFIG_BERUSEK;

#define MAX_SAD   20

typedef struct _SADA_PRVKU {

  byte  jmeno[MAX_JMENO];
  int   trida;
  int   podtrida;
  int   guid;
  int   prvek;

} SADA_PRVKU;

typedef struct _SADAR {

  byte        jmena[MAX_SAD][MAX_JMENO];
  SADA_PRVKU *p_sada[MAX_SAD];
  int         sadnum[MAX_SAD];
  int         sada_akt;  // vybrana sada
  int         sada_prvek;// vybrany prvek sady

  HWND        hwnd_list_trida;
  HWND        hwnd_dialog;

} SADAR;

typedef struct _B_KONFIG {  

  byte jmeno_levelu[100];
 
  int             prvku_databaze;              // pocet prvkuv databazi (odpovida kofolovym prvkum)
  PRVEK_DATABAZE *p_prv_dat[MAX_PRVKU_DATABAZE];  // seznamy vsech prvku (databaze)
  int             prvnum;                     // pocet prvku na mape
  PRVEK_LEVELU   *p_prv_lev[MAX_PRVKU_LEVEL]; // seznam  prvku umistenych nekde v levelu
  PRVEK_LEVELU  **p_lev;                      // pointery podle poradi v levelu
  int             x_num,y_num,z_num;          // rozmery levelu
  float           x_start,y_start,z_start;
  
  int             default_x;
  int             default_y;
  int             default_z;
  
  EDIT_KONTEJNER *p_kont[MAX_BERUSKY_KONTEJNERU]; //list vsech kontejneru (zasoba)
  EDIT_MATERIAL  *p_mat[MAX_EDIT_MATERIALU]; // list materialu z databazi
  
  OBJECTDESC     *p_kofola_prv; // pole kofolovych prvku
  SECONDDATADESC *p_kofola_file;

  GRID_DATA       grid;
  KONFIG_PRVKU    kp;
  
  SADAR           sad; //sadar
  
  void           *p_cnf; // pointer na cnf :)

  KONFIG_BERUSEK  cnf;   // konfigurace berusi

  FILE           *debug_file; // handle na log debug file
    
  byte seznam_tridy_prvku[MAX_TRID][MAX_TRID_JMENO];
  byte seznam_tridy_flagy[MAX_TRID];
  byte seznam_podtridy[MAX_TRID][MAX_PODTRID][MAX_TRID_JMENO];
  byte seznam_trid;


#define NAHRAZOVAC_NUM 10
  int  nahr_akt[NAHRAZOVAC_NUM];
  int  nahr_guid[NAHRAZOVAC_NUM];

} B_KONFIG;


//extern B_KONFIG *p_bnf;

#define be_pozice_v_levelu(p_bnf, _x, _y, _z, _p) ((p_bnf->x_num)*(p_bnf->z_num)*((_y)*2+(_p)) + (p_bnf->x_num)*(_z) + (_x))
#define be_pozice_v_levelu_dir(dx, dz, x, y, z)   ((dx)*(dz)*(y) + (dx)*(z) + (x))
#define be_pozice_na_disku(_i,_x,_y,_z,_p,_x_num,_z_num)\
{\
 int zb;\
 _y = (_i)/((_z_num)*(_x_num)*2);\
 zb = (_i)%((_z_num)*(_x_num)*2);\
 _p = zb/((_z_num)*(_x_num));\
 zb = zb%((_z_num)*(_x_num));\
 _z = zb/(_x_num);\
 _x = zb%(_x_num);\
}\


void be_rozsir_level(B_KONFIG *p_bnf, int px, int x, int py, int y, int pz, int z);
PRVEK_DATABAZE * be_vyrob_prvek_databaze(void);
int  be_najdi_volny_prvek(PRVEK_DATABAZE **p_prv, int max);
PRVEK_LEVELU ** vyrob_level(int x, int y, int z);
PRVEK_LEVELU ** be_vyrob_level(B_KONFIG *p_bnf, int x, int y, int z);
void be_zrus_level(B_KONFIG *p_bnf);
PRVEK_LEVELU * be_vyrob_prvek_levelu(void);
int  be_prvek_disk_do_bunky(B_KONFIG *p_bnf, PRVEK_LEVELU **p_lev, BUNKA_LEVELU_DISK *p_disk, int rekonstrukce);
int  be_prvek_bunky_na_disk(B_KONFIG *p_bnf, PRVEK_LEVELU  *p_lev, BUNKA_LEVELU_DISK *p_disk);
int  be_load_level(B_KONFIG *p_bnf, byte *p_file, byte *p_dir, int rekonstrukce);
int  be_save_level(B_KONFIG *p_bnf, byte *p_file, byte *p_dir);
void be_umisti_prvek(B_KONFIG *p_bnf, PRVEK_LEVELU *p_lev, int x, int y, int z);
void be_reload_databaze(B_KONFIG *p_bnf);
void be_vymaz_prvky_v_databazi(B_KONFIG *p_bnf);
void be_nacti_prvky_do_databaze(B_KONFIG *p_bnf);
int  be_nacti_prvky(B_KONFIG *p_bnf);
int  be_je_objekt_viditelny(int guid);
PRVEK_LEVELU * be_pridej_prvek_levelu_z_databaze(B_KONFIG *p_bnf, PRVEK_DATABAZE *p_prv_databaze, int guid);
int  be_spocti_sekce_v_ini(byte *p_file, byte ***p_sekce, int *p_snum);
int  be_nahraj_sekce(byte *p_file, byte *p_dir, int sada, byte **p_sekce, int snum, SADAR *p_sad);
int  be_smaz_sekce(byte **p_sekce, int snum);
void be_preumisti_prvek(B_KONFIG *p_bnf, PRVEK_LEVELU *p_lev);
void be_zrus_prvek_levelu(B_KONFIG *p_bnf, int x, int y, int z, int vrstva);
void be_zrus_prvek_levelu_index(B_KONFIG *p_bnf, int i);
void be_zrus_prvek_levelu_mesh(B_KONFIG *p_bnf, int i);
void be_kontroluj_level(B_KONFIG *p_bnf);
