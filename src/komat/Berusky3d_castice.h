/* Casticovy system pro Berusky II
*/

#ifndef __BERUSKY3D_CASTICE_H_
#define __BERUSKY3D_CASTICE_H_


/* Castice popisujici strepinu z krabice
   ma tvar obecneho 4-uhelnika
   a muze mit libovolne mapovani v 
   jednolivych rozich castice
*/
typedef struct _PAR_STREPINA
{

  BOD p;                        // akt umisteni castice ve 3D
  BOD dir;                      // smer letu castice
  float rychlost;               // aktualni rychlost letu castice
  float vaha;                   // vaha castice, jak moc klesa k zemi
  float utlum;                  // utlum rychlosti letu castice
  float g;                      // gravitacni konstanta
  float u1, u2, u3, u4;         // mapping ve vsech 4 rozich castice
  float v1, v2, v3, v4;

  float vel1;                   // 1 velikost castice
  float vel2;                   // 2 velikost castice
  float vel3;                   // 3 velikost castice
  float vel4;                   // 4 velikost castice  
  float barva;                  // barva castice (0..1)

  int aktivni;                  // nastavuju ja -> je castice viditelna?
  float frame;                  // pokud je material animovany, tak aktivni frame
  float framedelta;
  //dword time_vznik;      // cas vzniku castice

  struct _PAR_STREPINA *p_next; // navazani castice na dalsi
  struct _PAR_STREPINA *p_prev; // navazani castice na predchozi

} PAR_STREPINA;

/* Flekova castice - je definovana plochou,
   nepohybuje se, pouze scale
*/
typedef struct _PAR_FLEK
{

  BOD p;                        // akt umisteni castice ve 3D
  BOD nx;                       // smer pohledu castice
  BOD ny;                       // smer pohledu castice
  BOD tmp[4];                   // 4 tmp vertexy pouzite k renderingu

  float rychlost_x;             // x rychlost scale castice
  float rychlost_y;             // y rychlost scale castice
  float utlum_x;                // x utlum scale
  float utlum_y;                // y utlum scale


  float r, g, b, a;             // barva castice
  float dr, dg, db, da;         // zmena barvy castice (za sec)
  float ka;                     // konecna alfa (test na konec)

  int aktivni;                  // nastavuju ja -> je castice viditelna?

  float frame;                  // pokud je material animovany, tak aktivni frame
  float framedelta;
  //dword time_vznik;      // cas vzniku castice

  struct _PAR_FLEK *p_next;     // navazani castice na dalsi
  struct _PAR_FLEK *p_prev;     // navazani castice na predchozi

} PAR_FLEK;


/* Kourova castice - ma scale faktor
   a meni svoji barvu, mapping je defaultni
   rozmery ve 2 smerech
   a je staticka (stoji na miste)
   Nepouzivaji se zadny testy na rendering
*/
typedef struct _PAR_KOUR_STOPA
{

  BOD p;                        // umisteni castice ve 3D
  BOD nx, ny;                   // smerove-flekove vektory
  BOD dir;                      // smer letu
//  float dx,dy;
//  BOD   tmp[4];        // tmp body pro rendering
  BOD rp;                       // renderovaci pozice

  float rychlost_x;             // x rychlost scale castice a zaroven jeji rozmery
  float rychlost_y;             // y rychlost scale castice
  float utlum_x;                // x utlum scale
  float utlum_y;                // y utlum scale

  float r, g, b, a;             // barva castice
  float dr, dg, db, da;         // zmena barvy castice
  float ka;

  float frame;                  // pokud je material animovany, tak aktivni frame
  float framedelta;
  float y_plane;                // y-plane castice

  dword time_vznik;             // cas vzniku castice

  char rotace;                  // rotace vypoustenych castic

  float x_max;                  // klipovaci roviny -> pouzite pro kresleni castic
  float x_min;
  float z_max;
  float z_min;

  struct _PAR_KOUR_STOPA *p_next;       // navazani castice na dalsi
  struct _PAR_KOUR_STOPA *p_prev;       // navazani castice na predchozi

} PAR_KOUR_STOPA;


/* Moje interni flagy na castice
*/
#define TPAR_STREP           0x0001
#define TPAR_KOUR_STOPA      0x0002
#define TPAR_FLEK            0x0004
#define TPAR_ANIMATE         0x0008     // animovat tento system

typedef struct _PARMETAC_HNIZDO
{

  void *p_first;                // Linearni seznam aktivnich castic

  BOD *p_nx;                    // Normalovy vektor nx pro fleky
  BOD *p_ny;                    // Normalovy vektor ny pro fleky
  BOD *p_pivot;                 // pointer na bod, na kterem se castice uvolnuji
  BOD *p_dir;                   // pointer na smerovy vektor
  BOD *p_vitr;                  // pivot-pointer na vitr co se meni

  float rychlost_x;             // x rychlost scale castice a zaroven jeji rozmery
  float rychlost_y;             // y rychlost scale castice
  float utlum_x;                // x utlum scale
  float utlum_y;                // y utlum scale

  float r, g, b, a;             // barva castice
  float dr, dg, db, da;         // zmena barvy castice
  float ka;                     // mezni hodnota

  float frame;
  float framedelta;

  float y_plane;                // hranicni y-plocha
  float x_max;                  // klipovaci roviny -> pouzite pro kresleni castic
  float x_min;
  float z_max;
  float z_min;

  BOD min, max;                 // min,max
  int pnum;                     // pocet aktivnich castic

  char rotace;                  // rotace vypoustenych castic

  dword time_last;              // cas posledniho uvolneni
  dword time_interval;          // interval uvolnovani novych castic
  int time_first;               // cas uvolneni prvni castice

  struct _PARMETAC_HNIZDO *p_next;
  struct _PARMETAC_HNIZDO *p_prev;

} PARMETAC_HNIZDO;


/* Bomba - struktura kterou me kofola predava?
*/
typedef struct _PARMETAC
{

  void *p_first;                // seznam aktivnich/zasobnich castic

  PARMETAC_HNIZDO *p_hnizdo;    // seznam aktivnich hnizd

  int *p_flag;

  int flag;
  int material;

  int start;
  int stop;
  float time;                   // cas animace - 0..1

  BOD min;                      // kostka generatoru min  
  BOD max;                      // kostka generatoru max
  int hnum;                     // pocet aktivnich hnizd
  int pnum;                     // pocet aktivnich castic
  BOD *p_pivot;

  float y_plane;

  GLMATRIX world;               // transfromacni matice

  void *p_param;
  int param;
  int param2;
  END_FUNKCE p_endfce;          // kofolova end funkce

  struct _PARMETAC *p_next;
  struct _PARMETAC *p_prev;

} PARMETAC;

// Flagy vkladane do promenne flag

// castice se meni rozmerove
#define TPAR_SCALE          0x00100

// castice se pouze pricita
#define TPAR_SCALE_ADD      0x04000

// castice se meni barevne
#define TPAR_DIFF           0x00200

// castice je aktualni alfa vetsi nez referencni,je castice aktivni
#define TPAR_VETSI          0x00400

// parametr dir - pouzivar vektor posunu u castic
#define TPAR_DIR            0x00800

// parametr vitr - pouzivar vektor vetru
#define TPAR_VITR           0x01000

// kourova stopa se bere jako soubor 3D fleku
#define TPAR_3D             0x02000

// kourova stopa se bere jako soubor half-life fleku
#define TPAR_HALF_LIFE      0x08000

// pouziva frame-animace materialu
#define TPAR_FRAME          0x10000

// nevypusti prvni castici v poradi
#define TPAR_FIRST_LOST     0x20000

// Casticovy system se posouva s timto pivot-pointem
#define TPAR_POS_PIVOT      0x40000

// Casticovy system se renderuje bez mlhy
#define TPAR_NO_FOG         0x80000

// Bude se pouzivat 2D klipovani (x/z)
#define TPAR_2DCLIP        0x100000

// Testuje se viditelnost castic kolizi s obalkama
#define TPAR_HTEST        0x0200000

// Testuje horni y-plochu u hnizd (cas viditelna kdyz yc < y_plane)
#define TPAR_YPLANE_TOP   0x0400000

// Testuje dolni y-plochy u hnizd (cas viditelna kdyz yc > y_plane)
#define TPAR_YPLANE_LOW   0x0800000

// Casticovy system se sam odmaze po dobehnuti
#define TPAR_AUTOREMOVE   0x1000000


/* Vyrobi generator castic
   vraci handle casticoveho systemu
*/
ParHandle par_vyrob(void);

/* Nastavy vlastnosti casticoveho systemu
   material - material pouzity pro castice
   flag     - TPAR flag pro castice
   p_pos    - pozice casticoveho systemu v absolutnich souradnicich
   p_pivot  - pivot poin casticoveho systemu (bod s kterym se posouva casticovy system)
*/
ParHandle par_set_param(ParHandle ph, int material, int flag, BOD * p_pos,
  BOD * p_pivot);

/* Nastavi Y rovinu pro strepinove systemy
*/
ParHandle par_set_y_plane(ParHandle ph, float y_plane);

/* Zrusi generator castic, podle zadaneho handle 
*/
void par_zrus(ParHandle ph);

/* Cte pocet aktivnich (zivych) castic casticoveho systemu
*/
int par_get(ParHandle ph);

/* Spusti animaci casticoveho systemu, parametry jsou:
   
   *p_flag - flag, -1 = konec animace
    flag   - animacni flag, zatim davat FALSE
    start  - cas za ktery to ma vybuchnout (v milisec), 0 = okamzite
    stop   - cas konce vybuchu, 0 = neomezeny konec
*/
ParHandle par_go(ParHandle ph, int *p_flag, int start, int stop);

/* Pripoji tu dabelskou end funkci
*/
ParHandle par_pripoj_funkci(ParHandle ph, END_FUNKCE p_fce, size_ptr param,
  size_ptr param2, size_ptr p_param);


/* Cte pointer na prvni castici v linearnim seznamu castic
*/
void *par_cti_prvni(int ph);

/* Vlozi do casticoveho systemu castice typu strepina:

   - po celou dobu animace maji stejne rozmery
   - pohybuji se po zadane balisticke krivce
   - pusobi na ne gravitace
*/
void par_vloz_strepy(int ph, PAR_STREPINA * p_part, int pocet);


/* Vlozi castice fleku - staticle fleky urcene pozici a vektorama
*/
void par_vloz_fleky(int ph, PAR_FLEK * p_part, int pocet);


/***********************************************************************
    Hnizdove casticove systemy
  **********************************************************************
*/

/* Vlozi zasobu kourovych castic
*/
int par_vloz_kour_stopu(ParHandle ph, PAR_KOUR_STOPA * p_part, int pocet);

/* Vyrobi generacni hnizdo
*/
HnizdoHandle par_vloz_hnizdo(ParHandle ph);

/* Zruseni hnizda
*/
void par_zrus_hnizdo(ParHandle ph, HnizdoHandle hh);

/* Nastavuje parametry hnizda
*/
HnizdoHandle par_vloz_hnizdo_komplet(HnizdoHandle hh, int time_interval,
  BOD * p_pivot, PAR_KOUR_STOPA * p_part);
HnizdoHandle par_vloz_hnizdo_timer(HnizdoHandle hh, int time_interval,
  int time_first);
HnizdoHandle par_vloz_hnizdo_pivot(HnizdoHandle hh, BOD * p_pivot);
HnizdoHandle par_vloz_hnizdo_pivot_normal(HnizdoHandle hh, BOD * p_nx,
  BOD * p_ny);
HnizdoHandle par_vloz_hnizdo_dir(HnizdoHandle hh, BOD * p_dir);
HnizdoHandle par_vloz_hnizdo_vitr(HnizdoHandle hh, BOD * p_vitr);
HnizdoHandle par_vloz_hnizdo_scale(HnizdoHandle hh, float rx, float ry,
  float utlum_x, float utlum_y);
HnizdoHandle par_vloz_hnizdo_diff(HnizdoHandle hh, float r, float g, float b,
  float a, float dr, float dg, float db, float da, float ka);
HnizdoHandle par_vloz_hnizdo_y_plane(HnizdoHandle hh, float y_plane);
HnizdoHandle par_vloz_hnizdo_frame(HnizdoHandle hh, float frame,
  float framedelta);
HnizdoHandle par_vloz_hnizdo_clip(HnizdoHandle hh, float min_x, float min_z,
  float max_x, float max_z);
HnizdoHandle par_vloz_hnizdo_rotaci(HnizdoHandle hh, char rotace);

/* Generuje castici z hnizda (okamzite, neceka na cas)
*/
HnizdoHandle par_vloz_hnizdo_pust_castici(HnizdoHandle hh);

/* Prida okamzite zadany pocet castic do hnizda
   Vraci pointer na prvni pridavanou castici (lin seznam 
   vsech dalsich pridavanych)
*/
PAR_KOUR_STOPA *par_vloz_hnizdo_pust_castice(ParHandle ph, HnizdoHandle hh,
  int pocet);

/* Vrati pointer na hnizdo - muzes si ho nastavit sam primo
*/
PARMETAC_HNIZDO *par_cti_hnizdo(HnizdoHandle hh);

/* Vrati pointer na prvni aktivni castici hnizda,
   NULL pokud neni zadna aktivni
*/
PAR_KOUR_STOPA *par_cti_hnizdo_castice(HnizdoHandle hh);

/* Cte pocet hnizd casticoveho systemu
*/
int par_get_hnizda(ParHandle ph);

/* Zrusi se casticovy system - end
*/
void par_zrus_end(PARMETAC * p_par);

/* ***************************************************************
     Vodni dilo 2
   ***************************************************************
*/
#define  VOD_AKTIVNI  0x0001
#define  VOD_BOD      0x0002
#define  VOD_X_PLANE  0x0004
#define  VOD_Y_PLANE  0x0008
#define  VOD_Z_PLANE  0x0010

#define  VOD_DIFF_MOD 0x0020
#define  VOD_DIFF_ADD 0x0040
#define  VOD_DIFF_SUB 0x0080

#define  VOD_SPEC_MOD 0x0100
#define  VOD_SPEC_ADD 0x0200
#define  VOD_SPEC_SUB 0x0400

VodaHandle vod_vyrob(int meshu);
VodaHandle vod_pridej_mesh(VodaHandle vh, MeshHandle mh);
VodaHandle vod_uzavri_meshe(VodaHandle vh);
VodnikHandle vod_vloz_vodnika(VodaHandle vh, BOD * p_pos, int flag,
  float amplituda, float perioda, float fi, float damplituda, float dperioda,
  float dfi);

VodnikHandle vod_uprav_vodnika_par(VodnikHandle vh, float amplituda,
  float perioda, float fi, float damplituda, float dperioda, float dfi);
VodnikHandle vod_cti_vodnika_par(VodnikHandle vh, float *p_amplituda,
  float *p_perioda, float *p_fi, float *p_damplituda, float *p_dperioda,
  float *p_dfi);

VodnikHandle vod_uprav_vodnika_flag(VodnikHandle vh, int flag);
VodnikHandle vod_cti_vodnika_flag(VodnikHandle vh, int *p_flag);

VodnikHandle vod_vloz_vodnika_diff(VodnikHandle vh, float dr, float dg,
  float db, float da);
VodnikHandle vod_vloz_vodnika_add_diff(VodnikHandle vh, float dr, float dg,
  float db, float da);
VodnikHandle vod_vloz_vodnika_spec(VodnikHandle vh, float sr, float sg,
  float sb);
VodnikHandle vod_vloz_vodnika_add_spec(VodnikHandle vh, float sr, float sg,
  float sb);


#endif
