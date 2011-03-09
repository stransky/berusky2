/*
  Kofolovo rozhrani - H
  verze 0.8.0
 
  Poradne si prostuduj ty komentare tady, vsechno sem ti napsal
  jak se ovlada.
*/

/*
  Header files
*/

#ifndef __BERUSKY3D_KOFOLA_INTERFACE_H__
#define __BERUSKY3D_KOFOLA_INTERFACE_H__

#include "mss_on.h"
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "..\resource.h"
#include "wzakl_typ.h"
#include "ddx.h"
#include "di.h"
#include "matrix.h"
#include "..\\kofola\\Object.h"
#include "mat3d.h"
#include "kdtree.h"
#include "berusky_universal.h"
#include "berusky3d_castice.h"
#include "berusky3d.h"
#include "berusky3d_ini.h"
#include "berusky3d_load.h"
#include "berusky3d_animace.h"

extern G_KONFIG ber, *p_ber;
extern byte     ini_file[300];

/* Load indicie pro slider - 0-100%
*/
extern float kom_load_progres;

/*
  Promene validni po ber_graf_init()

  tyhle promeny muzes pouzivat po zavolani funkce ber_graf_init,
  jsou v nich aktualni hodnoty...
*/
extern dword      system_timer;    // systemovy casovac
extern dword      karmin_aktivni;  // TRUE pokud sou ber aktivni (na popredi)
extern HWND       hwnd_hry;        // hwnd okna berusek
extern HINSTANCE  hinst;           // hinst berusek


/********************************************************************** 
  Inicializace - myslim celkem jasny funkce
 **********************************************************************
*/
int  kom_graf_init(void);           // nahodi DirectX
void kom_graf_konec(int menu);      // zrusi DirectX


/**********************************************************************
  Load a zruseni levelu - to je taky urcite jasny
 **********************************************************************
*/
void kom_init_level(LEVEL_HEADER *p_head); // konfiguruje nahravany level, volat pred "void kom_load_level(byte *p_file)"
void kom_load_level(byte *p_file, int zmen_dir, int restart, BUNKA_LEVELU_DISK *p_bunka, int bunek);
void kom_post_init_level(void);            // konfiguruje level, volat po "void kom_load_level(byte *p_file)"
void kom_zrus_level(int restart);          // rusi level, pokud je to restart tak zrus spec zpusobem

// Nacte fyzickou souradnici policka levelu
BOD * kom_get_fyz_souradnice(int x, int y, int z, BOD *p_bod);


/**********************************************************************
  Hledaci funkce
 **********************************************************************
*/
// Hleda material podle jeho jmena, K_CHYBA pri neuspechu
MatHandle kom_najdi_material(byte *p_jmeno);

// Vrati pointer na material - strkat do flaru
EDIT_MATERIAL * kom_preloz_material(MatHandle mh);

// Hleda meshe prvku
MeshHandle kom_najdi_mesh_prvek(byte *p_jmeno);



/**********************************************************************
  Herni prvky a predmety
 **********************************************************************
*/

/* load/ruseni
*/
MeshHandle kom_pridej_prvek_levelu(BUNKA_LEVELU_DISK *p_bunka, int x, int y, int z);
MeshHandle kom_pridej_prvek_levelu_disk(byte *p_file, BUNKA_LEVELU_DISK *p_bunka, int x, int y, int z);

/* Vlozis jmeno kontejneru co chces najit a on ti z nejvyrobi prvek levelu.
   vraci K_CHYBA pokud nenasel kontejner toho jmena
*/
MeshHandle kom_pridej_mesh_do_levelu(char *p_jmeno);
void       kom_zrus_prvek(MeshHandle prvek_handle);
ExMeshHandle kom_najdi_mesh(byte *p_jmeno);

/* Nastavi pivot point pro prvek, prepise nastaveni pivotu u sim animace
   pokud to nepojede, dej vedet...
*/
int  kom_mesh_set_pivot_bod(MeshHandle prvek_handle, BOD *p_bod);
int  kom_mesh_set_pivot(MeshHandle prvek_handle, float x, float y, float z);
int  kom_mesh_get_pivot(MeshHandle prvek_handle, BOD *p_pivot);

/* Nastaveni aktivniho meshe u multi-mesh meshu
*/
MeshHandle kom_mesh_set_mesh(MeshHandle mh, int ID, int rotace);

/* Cte okamzite fyzicke!!! umisteni prvku ve scene + jeho rotaci
   Tato pozice zavisi take na umisteni levelu a velikosti hernich policek
   Tuto pozici pouzivam pro urcovani viditelnosti prvku a odpovida stredu policka
   kde je prvek umisten. Tato pozice se meni v realnem case pri behu animaci
*/
int  kom_mesh_get_float(MeshHandle prvek_handle, float *p_x, float *p_y, float *p_z, int *p_rotace);

/* Cte logicke umisteni prvku v levelu + jeho rotaci
   Vsechny prvky vraci v 0 vrstve, takze y je 0,2,4,6,...
*/
int  kom_mesh_get_int(MeshHandle prvek_handle, int *p_x, int *p_y, int *p_z, int *p_rotace);

/* Resetuje umisteni prvku ve scene
   Rika animacim, na ktere logicke souradnici je prvek umisten a ze jej ma na tuto
   pozici resetovat. Prvek bude mit stred (0,0,0) na zadanych logickych souradnicich
   jako by byl vlozen znova do levelu.
   Typicky tuto funkci zavolej, pokud se prvek nejak otoci a ty ho chces
   resetovat na defaultni smer rotace
*/
int  kom_mesh_reset_pr(MeshHandle prvek_handle, int x, int y, int z, int rotace);

/* Nastavi no-test flag (u vody)
*/
int  kom_mesh_set_no_test(MeshHandle prvek_handle);

/* Vrati savovaci cislo meshe (cislo ktere se strka do
   ukladacich struktur)
*/
int kom_mesh_get_save_num(MeshHandle prvek_handle);

/* Sladi logicke umisteni prvku s jeho fyzickym umistenim
*/
__inline int kom_mesh_reset(MeshHandle prvek_handle)
{
  int x,y,z,rotace; 
  kom_mesh_get_int(prvek_handle,&x,&y,&z,&rotace);
  kom_mesh_reset_pr(prvek_handle,x,y,z,rotace);
}

/* Premisteni prvku - Presune prvek na zadanou pozici
*/
#define kom_umisti_prvek(prvek_handle,x,y,z,rotace) ber_umisti_prvek(prvek_handle,x,y,z,rotace)


/* Databaze -> vznikani prvku z databaze za behu
   Vlozi prvek do databaze, ze ktere muze byt behem hry zkopirovan do levelu
*/
DataHandle kom_pridej_prvek_do_databaze(SECONDDATADESC *p_sec);

/* Kopiruje a umisti prvek z databaze do levelu.
   Kazdy prvek muzes kopirovat kolikrat chces.
*/
MeshHandle kom_vloz_prvek_z_databaze(DataHandle data_handle, int x, int y, int z, int rotace, int guid);



/***********************************************************************
  Kresleni levelu 
 ***********************************************************************
*/
void kom_klavesy_hra(void);
void ber_renderuj_scenu(void);
#define kom_kresli_level() ber_renderuj_scenu()


/***********************************************************************
  Animace materialu
 ***********************************************************************
*/

/* Nacte handle materilove animace konkretniho meshe a jeho podobjektu
*/
MatHandle kom_amat_mesh_get(MeshHandle mesh, int objekt);

/* Cte pocet materialovych animaci
*/
int kom_amat_mesh_get_num(void);


typedef struct _MATERIAL_BUNKA {

  MatHandle  handle_materialove_animace;
  MeshHandle handle_meshe;
  int        cislo_objektu;
  
} MATERIAL_BUNKA;

/* Vrati vsechny materialove animace ve scene, uklada do bunek o dvojicich 
   mesh_handle,objekt
   p_bunky musi byt predem naalokovany
*/
int kom_amat_mesh_get_all(MATERIAL_BUNKA *p_bunky);

/* Nastavi frame materialove animace
*/
int kom_amat_set_frame(MatHandle handle, int frame);

/* Vyvola udalost v materialove animaci
*/
int kom_amat_event(MatHandle handle);

/* Cte stav animace 
   0 = stoji
   1 = bezi
*/
int kom_amat_get(MatHandle handle);

/* Vrati cislo kontejneru k meshi
*/
int kom_amat_get_kontejner(MeshHandle mh);

/* Vrati pocet materialu konkretniho meshe
*/
int kom_mesh_get_matnum(MeshHandle mh);

/* Naplni pole handlu materialu
*/
int kom_mesh_get_mat(MeshHandle mh, MatHandle *p_mat);

/* Vrati jmeno konkretniho materialu
*/
byte * kom_get_mat_jmeno(MatHandle mh, byte *p_buffer, int max_znaku);


/**********************************************************************
  Makra pro prevod framu na cas a zpet
 **********************************************************************/

// prevod frame->cas(sec)
#define FRAME2SEC(frame) ((frame)/FRAMU_PER_SEC) 

// prevod cas(sec)->frame
#define SEC2FRAME(sec)   ((sec)*FRAMU_PER_SEC) 


/***********************************************************************
  Vyroba animaci hernich prvku
 ***********************************************************************
*/
AnimHandle sim_nahraj_animaci(APAK_HANDLE *pHandle, byte *p_jmeno, int linearne);
AnimHandle sim_vyrob_animaci(int pos_key, int rot_key, int scale_key);

AnimHandle sim_vloz_klic_posun(AnimHandle handle, int klic, float x, float y, float z, int frame);
AnimHandle sim_vloz_klic_posun_bod(AnimHandle handle, int klic, BOD *p_p, int frame);
AnimHandle sim_vloz_klic_posun_parametry(AnimHandle handle, int klic, float tension, float continuity, float bias);

AnimHandle sim_vloz_klic_rotace(AnimHandle handle, int klic, BOD *p_osa, float uhel, int frame);
AnimHandle sim_vloz_klic_rotace_quat(AnimHandle handle, int klic, QUAT *p_q, int frame);
AnimHandle sim_vloz_klic_rotace_parametry(AnimHandle handle, int klic, float tension, float continuity, float bias);

AnimHandle sim_vloz_klic_scale(AnimHandle handle, int klic, float x, float y, float z, int frame);
AnimHandle sim_vloz_klic_scale_bod(AnimHandle handle, int klic, BOD *p_scs, int frame);
AnimHandle sim_vloz_klic_scale_parametry(AnimHandle handle, int klic, float tension, float continuity, float bias);

AnimHandle sim_vloz_pivot(AnimHandle handle, float x, float y, float z);
AnimHandle sim_vloz_pivot_bod(AnimHandle handle, BOD *p_bod);

AnimHandle sim_interpoluj_animaci(AnimHandle handle, int framenum, int loop);
AnimHandle sim_zrus_animaci(AnimHandle handle);

/***********************************************************************
  Run Animace
 ***********************************************************************
*/

#define  MESH_PRVEK             0x1 // interni
#define  MESH_LOCAL_MATRIX      0x2 // interni
#define  MESH_MULT_MATRIX       0x4 // interni
#define  MESH_USE_MATRIX        0x8 // interni
#define  MESH_PIVOT             0x20 // interni
#define  MESH_TRANSFORMUJ_PIVOT 0x10 // interni
#define  MESH_KAMERA            0x40 // je to kamera

typedef void (* END_FUNKCE)(int param, int param2, void *p_param);

RunHandle    rani_aktivuj(AnimHandle shandle, int *p_flag, int flag, int start, int stop);

/* Stejne jako rani_aktivuj, jenom se nespusti. Musis ji ructe spustit funkci
   RunHandle rani_go(RunHandle ah, int flag, int start, int stop),
   kde flag,start,stop jsou flagy co jinak strkas do te rani_aktivuj()
   a ah je handle run animace kterou ti vrati to rani_aktivuj
*/
RunHandle    rani_aktivuj_cekej(AnimHandle shandle, int *p_flag);

/* Pripoji na run animaci funkci p_fce, funkce je ve tvaru:
   void moje_funkce(int param, void *p_param)
   pokud p_fce = NULL tak nastavenou funkci zase smazes
   privazat ji muzes kdykoliv v prubehu animace, ale samozrejme
   ze to musis stihnout do konce animace, jinak se ti nezavola
   ty promeny param, p_param ti do te funkce strcim.
*/
RunHandle rani_pripoj_funkci(RunHandle a_handle, END_FUNKCE p_fce, int param, int param2, void *p_param);

/* Pripoji na run animaci dalsi run animaci s handlem next_ah,
   po skonceni animace ah se zavola automaticky funkce rani_go takto:

   rani_go(next_ah, flag, start, stop);

  cimz se automaticky spusti animace next_ah.
  pokud next_ah = -1 (K_CHYBA) tak se fronta vymaze.

  Je jedno jestli animace ah bezi nebo stoji.
*/
RunHandle rani_next_animace(RunHandle ah, RunHandle next_ah, int flag, int start, int stop);

/* Jedina zmena - promena transformuj_pivot = 1 znamena ze pivot se ma transformovat
   do souradnic meshe. zere to trochu pri privazovani cas, takze to nepouzivej pokud
   ten mesh je resetovanej
*/
MatrixHandle rani_privaz_mesh(RunHandle a_handle, MeshHandle m_handle, int transformuj_pivot);
MatrixHandle rani_privaz_kameru(RunHandle a_handle);
MatrixHandle rani_privaz_svetlo(RunHandle a_handle, LightHandle l_handle);
RunHandle    rani_rozvaz(RunHandle a_handle, MatrixHandle m_handle);

// Znovu spousti uz dobehnutou nebo jeste nespustenou run animaci
RunHandle    rani_go(RunHandle ah, int flag, int start, int stop);

// Zrusi run animaci
int          rani_zrus(RunHandle ahandle);

/***********************************************************************
  Vazani meshu dohromady -> top mesh je pod low meshem
 ***********************************************************************  
*/
int kom_svaz_meshe(MeshHandle m_top, MeshHandle m_low);
int kom_rozvaz_mesh(MeshHandle m_low);


/***********************************************************************
  Lokalni animace meshu (lokani key-frame animace)
 ***********************************************************************
*/
MeshHandle lani_set(MeshHandle mh, int poradi, int c_anim, int *p_flag, int flag, int start, int stop);
MeshHandle lani_smaz_frontu(MeshHandle mh);
MeshHandle lani_next(MeshHandle mh);
int lani_get(MeshHandle mh);

/******************************************************************************* 
 
  Rezimy kamer:

  1. Polarni rezim:

  Kamera je zadana pomozi 3 parametru
  
  bod   - Stredovy bod kolem ktereho se kamera otaci
  r     - uhel otoceni kamery kolem osy Y (pravo/levo)
  fi    - uhel otoceni kamery kolem osy X (nahoru/dolu)
  vzdal - vzdalenost od bodu
   
  2. 3DS Rezim
  
  Kamera je zadana pomoci 2 bodu a 1 nebo 2 uhlu

  Position   - Bod kde lezi kamera
  Target     - Bod kam se kamera diva
  Roll       - Rotace kamery kolem osy kterou se diva (otaci se jakoby kolem stredu obrazovky)
  Fov        - Field-Of-View (perspektiva/zoom kamery - je defaultne nastavene, vetsinou neni potreba)
 
  Rezimy (polar/3ds) se prepinaji automaticky podle toho kterou funkci pouzijes.  

  Ovladani kamer:

 * Nacteni orientace kamery (vsechny rezimy)
    void kam_get_float(float *p_body);
    
    Smer pohledu kamery - (p_body[0],p_body[1],p_body[2])
    Smer nahoru kamery - (p_body[3],p_body[4],p_body[5])

 * Nastaveni / Cteni orientace kamery

  1. Nastaveni souradnic kamery 

    int  kam_pol_set(BOD *p_p, float r, float fi, float vzdal);
    int  kam_3ds_set(BOD *p_pos, BOD *p_target, float roll);

  2. Pricteni k aktualnimu nastaveni kamery  
    
    Pokud chces pricist vzdalenost nebo otocit kameru o 90 stupnu
    tak toto jsou idelani funkce. Ty hodnoty se prictou k aktualnimu
    nastaveni
  
    int  kam_pol_add(BOD *p_p, float r, float fi, float vzdal);
    int  kam_3ds_add(BOD *p_pos, BOD *p_target, float roll);
 
  3. Nacteni aktualniho stavu kamery
  
    Vraci nastaveni kamery v 3ds nebo polarnich souradnicich

    int  kam_pol_get(BOD *p_p, float *p_r, float *p_fi, float *p_vzdal);
    int  kam_3ds_get(BOD *p_pos, BOD *p_target, float *p_roll);

  4. Ovladani perspektivy/zoomu - pro vsechny mody
    
    void  kam_set_fov(float fov);  - Nastavi uhel kameru (def. je asi 55 stupnu)
    float kam_get_fov(void);       - cte fov kamery

 * Jednoduche animace kamer

   Tohle jsou rychle a jednoduche animace, kdy zadas  kam se ma kamera
   presunout, strcis ji promenou do ktere zapisuje stav animace 
   (jestli uz kamera dobehla nebo ne) a vsechno bezi mimo tebe. Po dobehnuti 
   se animace sama smaze, takze se nemusis o nic uz starat.
   Jako vychozi pozice animace se pouzije aktualni nastaveni kamery, jako
   cil se pouzije to co zadas. To RunHandle vubec nemusis nijak kontrolovat.


   1. Nastavovaci funkce

   Tyhle funkce pracuji aktulani_pozice -> zadana_pozice.

   Parametry:
     *p_flag - promena (globalni! ne lokalni zasobnikova), kam kamera 
               haze stav animace, 0..100 procenta ubehle animace (50 = 1/2 animace)
               -1 = konec animace, animace skoncila
      flag   - flag animace, tady 0
    framenum - pocet framu animace, 20 framu = 1 sec.
     tension - tenze animace, je od -1.0f do 1.0f, 0 = default znamena to jak rychle
               bude animace zacinat/koncit. vetsinou pouzivam 1.0f 
               coz vypada celkem pekne.
    
      Zbyle 4(3) parametry jsou cilove nastaveni kamery.

      RunHandle kam_pol_anim(BOD *p_p, float r, float fi, float vzdal, int *p_flag, int flag, int framenum, float tension);
      RunHandle kam_3ds_anim(BOD *p_p, BOD *p_t, float roll, int *p_flag, int flag, int framenum, float tension);

   2. Pricitaci funkce

   Tyhle funkce jsou uplne stejne jako predchozi, ale zadane hodnoty se prictou
   k aktualnimu nastaveni kamery. (takze pokud chces otocit animovane kameru 
   o 90 stupnu, priblizit/vzdalit a pod. tak timto)
  
      RunHandle kam_pol_anim_add(BOD *p_p, float r, float fi, float vzdal, int *p_flag, int flag, int framenum, float tension);
      RunHandle kam_3ds_anim_add(BOD *p_p, BOD *p_t, float roll, int *p_flag, int flag, int framenum, float tension);


  * Uplne animace kamer

  Funguji uplne stejne jako ty na mesh. Vyrobis si animaci, vlozis klice
  ktere nemusis interpolovat a rovnou to spustis. Animaci kamer muze byt vice
  ale beze muze jen jedna.

   1. Vyrobeni animace

     Vraci klasicke hadle na animaci

     AnimHandle kam_pol_vyrob_animaci(int framenum, int pos_keys, int rot_keys, int vzdal_keys);
     (rot keys jsou klice rotace r a fi, tyto 2 uhly se zadavaji dohromady)
  
     AnimHandle kam_3ds_vyrob_animaci(int framenum, int pos_keys, int target_keys, int roll_keys, int fov_keys);


  2. Nahrani animace

    Kdyz si animaci udelas v editoru, muzes si ju imto nahrat:    
     AnimHandle kam_3ds_nahraj_animaci(byte *p_jmeno);
     

  3. Vlozeni klicu 
  
    polarnich klicu
      AnimHandle kam_pol_vloz_klic_posun(AnimHandle handle, int klic, float x, float y, float z, int frame);
      AnimHandle kam_pol_vloz_klic_posun_bod(AnimHandle handle, int klic, BOD *p_p, int frame);
      AnimHandle kam_pol_vloz_klic_posun_parametry(AnimHandle handle, int klic, float tension, float continuity, float bias);
      AnimHandle kam_pol_vloz_klic_rotace(AnimHandle handle, int klic, float r, float fi, int frame);
      AnimHandle kam_pol_vloz_klic_rotace_parametry(AnimHandle handle, int klic, float tension, float continuity, float bias);
      AnimHandle kam_pol_vloz_klic_vzdal(AnimHandle handle, int klic, float vzdal, int frame, float tension, float continuity, float bias);
      AnimHandle kam_pol_vloz_klic_all(AnimHandle handle, int klic, BOD *p_p, float r, float fi, float vzdal, int frame, float tension);

   3ds klicu
      AnimHandle kam_3ds_vloz_klic_posun(AnimHandle handle, int klic, float x, float y, float z, int frame);
      AnimHandle kam_3ds_vloz_klic_posun_bod(AnimHandle handle, int klic, BOD *p_p, int frame);
      AnimHandle kam_3ds_vloz_klic_posun_parametry(AnimHandle handle, int klic, float tension, float continuity, float bias);
      AnimHandle kam_3ds_vloz_klic_target(AnimHandle handle, int klic, float x, float y, float z, int frame);
      AnimHandle kam_3ds_vloz_klic_target_bod(AnimHandle handle, int klic, BOD *p_target, int frame);
      AnimHandle kam_3ds_vloz_klic_target_parametry(AnimHandle handle, int klic, float tension, float continuity, float bias);
      AnimHandle kam_3ds_vloz_klic_roll(AnimHandle handle, int klic, float roll, int frame, float tension, float continuity, float bias);
      AnimHandle kam_3ds_vloz_klic_fov(AnimHandle handle, int klic, float fov, int frame, float tension, float continuity, float bias);
      AnimHandle kam_3ds_vloz_klic_all(AnimHandle handle, int klic, BOD *p_p, BOD *p_t, float roll, int frame, float tension);


   Promene ve funkcich sou parametry jsou od -1.0f do 1.0f pro kazdy klic a znamenaji:

   tension      - Jak silne bude animace v tomto bode zrychlovat/zpomalovat
   continuity   - Spojitost v tomto bode (tj. velikost zrychlen)
   bias         - Jak silne bude animace v tomto bode pokracovat
                  ve svem puvodnim smeru 
   0 = defaultni nastaveni pro vsechny (kdyz je nezadas tak jsou nulove)

   vic info je ve poc. grafice (ta hneda knizka) na strane 361/Pocitacova animace


  4. Zruseni animace (3ds i polarni)
     AnimHandle kam_zrus_animaci(AnimHandle handle);


  5. Prehrani animace

     int  kam_start(int a_handle, int *p_flag, int flag, int start, int stop);

     Spusti animaci s handle a_handle. Parametry jsou


    a_handle - AnimHandle animace
     *p_flag - promena (globalni! ne lokalni zasobnikova), kam kamera 
               haze stav animace, 0..100 procenta ubehle animace (50 = 1/2 animace)
               -1 = konec animace, animace skoncila
      flag   - flag animace, muze byt GK_LOOP|GK_REMOVE
               kde: GK_LOOP   - animace se bude opakovat
                    GK_REMOVE - animace se po skonceni automaticky smaze
    start    - frame od ktereho se ma animovat
    stop     - frame do ktereho ma animace bezet (0 = az do konce)

  6. Stop animace

     void kam_stop(void);

     Pokud animace bezi, muzes ji zastavit touto funkci. Pokud animace skonci
     (*p_flag = -1) tak uz nebezi a tuto funkci nevolej, uz se volala :)
     Pokud do startu das flag GK_REMOVE, v teto funkci se animace taky smaze.
     Nepredava se handle, protoze nezet muze jen 1 animace kamery



 *******************************************************************************
*/
// Interni flagy
#define GAME_KAMERA_POLAR    0x1 // kamera je v polarnim modu
#define GAME_KAMERA_3DS      0x2 // kamera je v 3ds modu
#define GAME_KAMERA_ANIMACE  0x4 // kamera se prave animuje
#define GAME_KAMERA_KOREKCE  0x8 // kamera se koriguje


/* Konstanty predavane do promene flag
*/
#ifndef GK_LOOP
  #define GK_LOOP          0x100   // Run-animace se prehrava furt dokola
#endif

#ifndef GK_REMOVE
  #define GK_REMOVE        0x200   // Animace se po dobehnuti automaticky smaze
#endif


// Nacte vektory kamery
void kam_get_float(float *p_body);

// Nacte vektory kamery + pozici
void kam_get_float_ext(float *p_dir, float *p_pos);

// ovladani perspektivy/zoomu kamery
void  kam_set_fov(float fov);    
float kam_get_fov(void);


// nastaveni, pricteni a nacteni kamery v polarnich souradnicich
int  kam_pol_set(BOD *p_p, float r, float fi, float vzdal, int korekce);
int  kam_pol_add(BOD *p_p, float r, float fi, float vzdal, int korekce);
int  kam_pol_get(BOD *p_p, float *p_r, float *p_fi, float *p_vzdal);

// nastaveni, pricteni a nacteni kamery v 3ds souradnicich
int  kam_3ds_set(BOD *p_pos, BOD *p_target, float roll);
int  kam_3ds_add(BOD *p_pos, BOD *p_target, float roll);
int  kam_3ds_get(BOD *p_pos, BOD *p_target, float *p_roll);

// Jednoduche animace kamer (odkud-kam) - samorusici se
// flag musi byt staticka/globalni promena (ne zasobnikova!!)
RunHandle kam_pol_anim(BOD *p_p, float r, float fi, float vzdal, int *p_flag, int flag, int framenum, float tension);
RunHandle kam_pol_anim_add(BOD *p_p, float r, float fi, float vzdal, int *p_flag, int flag, int framenum, float tension);
RunHandle kam_3ds_anim(BOD *p_p, BOD *p_t, float roll, int *p_flag, int flag, int framenum, float tension);
RunHandle kam_3ds_anim_add(BOD *p_p, BOD *p_t, float roll, int *p_flag, int flag, int framenum, float tension);

// Slozite animace kamer
AnimHandle kam_pol_vyrob_animaci(int framenum, int pos_keys, int rot_keys, int vzdal_keys, int korekce);
AnimHandle kam_pol_vloz_klic_posun(AnimHandle handle, int klic, float x, float y, float z, int frame);
AnimHandle kam_pol_vloz_klic_posun_bod(AnimHandle handle, int klic, BOD *p_p, int frame);
AnimHandle kam_pol_vloz_klic_posun_parametry(AnimHandle handle, int klic, float tension, float continuity, float bias);
AnimHandle kam_pol_vloz_klic_rotace(AnimHandle handle, int klic, float r, float fi, int frame);
AnimHandle kam_pol_vloz_klic_rotace_parametry(AnimHandle handle, int klic, float tension, float continuity, float bias);
AnimHandle kam_pol_vloz_klic_vzdal(AnimHandle handle, int klic, float vzdal, int frame, float tension, float continuity, float bias);
AnimHandle kam_pol_vloz_klic_all(AnimHandle handle, int klic, BOD *p_p, float r, float fi, float vzdal, int frame, float tension);
AnimHandle kam_zrus_animaci(AnimHandle handle);

AnimHandle kam_3ds_vyrob_animaci(int framenum, int pos_keys, int target_keys, int roll_keys, int fov_keys);
AnimHandle kam_3ds_nahraj_animaci(APAK_HANDLE *pHandle, byte *p_jmeno);
AnimHandle kam_3ds_vloz_klic_posun(AnimHandle handle, int klic, float x, float y, float z, int frame);
AnimHandle kam_3ds_vloz_klic_posun_bod(AnimHandle handle, int klic, BOD *p_p, int frame);
AnimHandle kam_3ds_vloz_klic_posun_parametry(AnimHandle handle, int klic, float tension, float continuity, float bias);
AnimHandle kam_3ds_vloz_klic_target(AnimHandle handle, int klic, float x, float y, float z, int frame);
AnimHandle kam_3ds_vloz_klic_target_bod(AnimHandle handle, int klic, BOD *p_target, int frame);
AnimHandle kam_3ds_vloz_klic_target_parametry(AnimHandle handle, int klic, float tension, float continuity, float bias);
AnimHandle kam_3ds_vloz_klic_roll(AnimHandle handle, int klic, float roll, int frame, float tension, float continuity, float bias);
AnimHandle kam_3ds_vloz_klic_fov(AnimHandle handle, int klic, float fov, int frame, float tension, float continuity, float bias);
AnimHandle kam_3ds_vloz_klic_all(AnimHandle handle, int klic, BOD *p_p, BOD *p_t, float roll, int frame, float tension);

// Prehravani slozitych animaci kamer
RunHandle  kam_start(AnimHandle a_handle, int *p_flag, int flag, int start, int stop);
void       kam_stop(void);

// Cteni klicu kamery
int kam_3ds_cti_klic(AnimHandle handle, float time, BOD *p_p, BOD *p_t, float *p_roll);
int kam_pol_cti_klic(AnimHandle handle, float time, BOD *p_t, float *p_r, float *p_fi, float *p_vzdal);

// Servisni funkce
void kam_set_mod_param(int mod); // nastavi mod kamery (3ds/polarni)
void kam_set_mod_3ds(void);      // nastavi 3ds mod kamerty
void kam_set_mod_polar(void);    // nastavi polarni mod kamery
int  kam_get_mod(void);          // zjisti aktualni mod kamery

/***********************************************************************
  Nastaveni mlznych kostek
 ***********************************************************************
*/
/*
  Nacte pointer na mlhokostku, ktera se muze libovolne upravovat:

  typedef struct _MLZNA_KOSTKA {
  
    byte  jmeno[MAX_JMENO];// jmeno mlzne kostky
    BOD   min,max;         // rozmery kostky
    int   flag;            // flagy mlhy
    int   mod;             // mod mlhy (linear/exp/exp2)
    int   priorita;        // priorita mlzne kostky
    float start;           // zacatek mlhy
    float stop;            // konec mlhy (za tim je maximalni)
    float intenzita;       // intenzita mlhy (pro exp a pod)
    float r,g,b,a;         // barva
    
    struct _MLZNA_KOSTKA *p_next;
      
  } MLZNA_KOSTKA;
*/

MLZNA_KOSTKA * kom_mlhokostka_najdi(byte *p_jmeno);


/***********************************************************************
   Fleky
 ***********************************************************************
*/
/* Prida flek do sceny, vrati jeho handle
   Pokud je MeshHandle == K_CHYBA - je to globani flek
   jinak je flek privazanej ke konkretnimu meshu

   uroven je vrstva fleku (0,1,2,..pocet) (kdyz je vice fleku na 1 miste, tak 
   nejspodnejsi ma uroven "0" a nejvyssi uroven "pocet")
*/
FlekHandle kom_flek_pridej(MeshHandle mh, BOD *p_p, BOD *p_nx, BOD *p_nz, float dx, float dz, int rotace, int uroven,  MatHandle material);
void       kom_flek_zmen(FlekHandle fh, BOD *p_p, BOD *p_nx, BOD *p_nz, float dx, float dz, int rotace, int uroven, MatHandle material);
void       kom_flek_zrus(FlekHandle fh);
int        kom_flek_getflag(FlekHandle fh, int flag);
int        kom_flek_setflag(FlekHandle fh, int flag);

#define    FLEK_NEKRESLIT   0x1


/***********************************************************************
  Chapadelnici
 ***********************************************************************
*/

// Cteni animaci sceny
ExMeshHandle   kom_najdi_mesh_joint(ExMeshHandle *p_next);
ExMeshHandle   mesh_to_ex_mesh(MeshHandle mh);
int            chani_mesh_cti_objekty(ExMeshHandle mh);
int            chani_mesh_cti_chapadelniky(ExMeshHandle mh, int objekt, ChapadloHandle **p_chap, int *p_chapadel);
byte *         chani_cti_jmeno(ChapadloHandle chh);

// Vlozi/Vyhodi animace chapadelnika do aniamcniho seznamu
ChapadloHandle chani_zarad(ChapadloHandle chh);
ChapadloHandle chani_zrus(ChapadloHandle chh);

// Start/Stop animace
ChapadloHandle chani_go(ChapadloHandle chh, int *p_flag, int flag, int start, int stop);
ChapadloHandle chani_stop(ChapadloHandle chh);


/***********************************************************************
  Dynamicke svetla - jednoduche
 ***********************************************************************
*/

/***********************************************************************
  Dynamicke svetla - slozite
 ***********************************************************************
*/

/* Kresleni car v levelu
*/
/*
typedef struct _KOFOLOVA_CARA {

  BOD     p1,p2;
  BODRGB  b1,b2;

} KOFOLOVA_CARA;
*/
void kom_set_linelist(KOFOLOVA_CARA *p_list, int pocet);



/* Nastaveni zpruhlednovani prvku
*/
void kom_zpruhlednovat_prvky(int stav);
void kom_zpruhlednovat_prvky_zmena_berusky(void);

/* Cte stacicitu prvku
*/ 
int kom_je_prvek_staticky(int guid);

/* FPS 
*/
float kom_get_framerate(void);


/* Debug-soubor
*/
void kprintf(byte log, byte *p_text,...);
void kprintfe(byte log, byte *p_text,...);

/* Nacte level environment levelu
*/
LEVEL_ENVIRONMENT * kom_get_level_environment(void);

/* Nacte mesh, na ktery ukazuje mys
*/
MeshHandle kom_get_mesh_mys(int itrida);


/* Texture-config
*/
/*
  Parametry funkce:
  mip_mapping = 0/1 (on/off)
  mip_filtr = 0/1 (linear/trilinear)
  std_filtr = 0/1 (near/liner)
  wrap_x/y = 0/1 (clamp/wrap)
  hi_res = 0/1
*/
void kom_set_default_text_config(int mip_mapping, int mip_filtr, int std_filtr, int wrap_x, int wrap_y, int hi_res);

/* Load zpet defaultnich hodnot
*/
void kom_ret_default_text_config(void);

/* Nahraje konfiguraci z INI (po zmene hodnot)
*/
void kom_reload_ini(void);

/* Cte velikost levelu
*/
void kom_get_level_size(BOD *p_min, BOD *p_max);

/* Zapne/vypne zpruhlednovac
*/
void kom_zpruhlednovac_on(void);
void kom_zpruhlednovac_off(void);

/* Zvyraznovani prvku on/off
*/
void kom_zvyraznovac(void);

/* Load-slidery
*/
void kom_load_param_set(int tiku);
void kom_load_param_fce(void (*p_load_fce)(void));

/* Pridani kurzoru do hry
*/
MeshHandle kom_pridej_kurzor_do_levelu(void);

/* Nastavi viditelnost prvku
*/
MeshHandle kom_prvek_viditelnost(MeshHandle mh, int vid);

/* Zere mesh
*/
int kom_get_mesh_mys_all(void);

/* Dela okno
*/
HWND otevri_okno(HINSTANCE handle_aplikace, int full_screen, int dx, int dy, HW_KONFIG *p_conf);

#endif