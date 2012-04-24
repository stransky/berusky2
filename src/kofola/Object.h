//------------------------------------------------------------------------------------------------
// version 0.0.6
//------------------------------------------------------------------------------------------------

#ifndef _OBJECT__
#define _OBJECT__

#include <stdio.h>
//#include <d3d.h>
#include "3d_all.h"
#include "Berusky_universal.h"
#include "Berusky3d_castice.h"
#include "camera.h"

#define _3DKOREKCE 40
#define VERZELO 5
#define VERZEHI 1

#ifndef __DEMO
#define SAVE_VER	1
#define DEMO_VER	1
#define PROFILE_VER	1
#endif

#ifdef __DEMO
#define SAVE_VER	2
#define DEMO_VER	2
#define PROFILE_VER	2
#endif

typedef struct
{
  int Index;                    // index wavu
  int Type;                     // typ wavu
} SOUND;

typedef struct
{
  int item[16];                 // predmety
  char Count_of_Items;          // pocet predmetu co nese
  char Max_Items;               // maximalni pocet predmetu
  int Strength;                 // sila(kolik utlaci)
} BACK_PACK;

typedef struct
{
  char File_Name[10][30];       // jmena souboru (mesh, textury)
} SECONDDATADESC;

typedef struct
{
  int handle_materialove_animace;
  int handle_meshe;
  int cislo_objektu;
} MATERIAL_BUNKA_K;

typedef struct
{
  int bSwitch;
  int Mesh;
  int iRot;
  int Flag;
  int iID;
  DWORD dwTime;
  int iAnimCount;
  int iSound;
  int iEffect;
  int iEffectCounter;
} BEETLEANIMATION;

typedef struct
{
  char cType;
  int iFlag;
} MATERIAL_EVENT;

typedef struct
{
  char Name[30];                // jmeno
  int GUID;                     // Glabal Uniqe Identificator
  int Class;                    // trida objektu
  int SubClass;                 // pod-trida objektu
  int Material;                 // material pro zvuky   
  SOUND Specific[3];            // specificke zvuky
  SOUND Reserved[3];            // jine zvuky
  SECONDDATADESC Out_File;      // ponechano pro pridavani prvku do databaze
  MATERIAL_EVENT MatEvent[3];
} OBJECTDESC;

typedef struct
{
  float pos[3];
  int Flag;
  float nx[3];
  float nz[3];
  float dx;
  float dy;
  int Rotation;
  int Material;
  int pFlek;
} FLEK_K;

typedef struct
{
  FLEK_K Flek;                  // flek prvku
  int iUroven;                  // uroven fleku
  FLEK_K *pExtFlek;             // pointer na flek privazany k nemu jinym prvkem (napr. predmetem)
} SQUAREDESC;

typedef struct
{
  OBJECTDESC *p_Object;         // pointer na objekt
  int Pos[3];                   // logicka pozice
  int Connection[2];            // vazby
  int Rotation;                 // natoceni
  int Index_Of_Game_Mesh;       // mesh
  BACK_PACK *p_Back_Pack;       // pointer na batoh (predmety)
  char bDestroed;               // indikator, ze prvek se nepouziva
  int a_run;                    // animace
  int a_flag;                   // flag_animace
  int iItem;                    // jaky sem index v poli Itemu
  char bMatEvent[3];            // Flag pouziti jednotlivych eventu (pro animace)
  MATERIAL_BUNKA_K *mMaterial;  // pointer material (komat)
  SQUAREDESC Square;            // Informace o vecech s prvkem
  int _3ds_flag;                // flaq 3ds animace (beruska,...)
  int hSvetlo;                  // handle svetla
} ITEMDESC;

typedef struct
{
  unsigned long room;
  float size;
  char bsize;
  long wall_occlusion;
  char bwall_occlusion;
  float wall_occlusionLF;
  char bwall_occlusionLF;
  float wall_occlusion_room;
  char bwall_occlusion_room;
  long out_room;
  char bout_room;
  long out_roomHF;
  char bout_roomHF;
  float out_room_rolloff;
  char bout_room_rolloff;
  float out_air_absorbtion;
  char bout_air_absorbtion;
  long id;
  char extsoundf[64];
  char bunderwater;
} ENVIRONMENT;

typedef struct
{
  PAR_STREPINA *pCastice;
  int Sizeof;
  int flag;
  DWORD dwStart;
} SYSTEMCASTIC;

typedef struct
{
  PAR_KOUR_STOPA *pCastice;
  int System;
  int Sizeof;
  int flag;
  size_ptr hHnizdo[64];
  float dir[64][3];
  float pivot[64][3];
  float nx[64][3];
  float ny[64][3];
  DWORD dwStart;
  DWORD dwTime;
  DWORD dwStop;
  DWORD dwExpire;
  DWORD dwExpireTime;
} SYSTEMKOUROVYCHCASTIC;

typedef struct
{
  PAR_KOUR_STOPA *pCastice;
  PAR_KOUR_STOPA *pWCastice;
  int SizeofCastice;
  int SizeofWCastice;
  int pSystem;
  int pWSystem;
  int flag;
  int *hHnizdo;
  int *hWHnizdo;
  int SizeofHnizda;
  int SizeofWHnizda;
  BOD dir;
  BOD nx;
  BOD ny;
  BOD Wnx;
  BOD Wny;
  BOD *pivot;
  BOD *Wpivot;
  BOD *Wpos;
  BOD RainCenter;
  unsigned int uiDensity;
  unsigned int uiIntensity;
  float fmaxDivergence;
  float fDist;
  float fSky;
  float fLowPlain;
  char bRain;
  char bWaterCircles;
} RAINSYSTEM;

typedef struct
{
  PAR_KOUR_STOPA *pCastice;
  int SizeofCastice;
  int pSystem;
  int flag;
  int *hHnizdo;
  int SizeofHnizda;
  BOD dir;
  BOD *pivot;
  BOD SnowCenter;
  unsigned int uiDensity;
  unsigned int uiIntensity;
  float fmaxDivergence;
  float fDist;
  float fSky;
  float fLowPlain;
  char bSnow;
} SNOWSYSTEM;

typedef struct
{
  int hSvetlo;
  char bMist;
} MISTSYSTEM;

typedef struct
{
  int EffectID;
  PAR_KOUR_STOPA *pCastice;
  int SizeofCastice;
  int pSystem;
  int flag;
  int *hHnizdo;
  int SizeofHnizda;
  BOD *dir;
  int SizeofDir;
  BOD *pivot;
  int SizeofPivot;
  BOD *nx;
  int SizeofNX;
  BOD *ny;
  int SizeofNY;
  float fRadius;
  unsigned int uiDensity;
  unsigned int uiIntensity;
  float fSky;
  float fLowPlain;
  char bEffect;
  DWORD dwTime;
  DWORD dwRegTime;
  DWORD dwGenTime;
  DWORD dwGenTimeCounter;
  int iGenProbability;
  BOD AddDir;
} NATUREEFFECTSYSTEM;

typedef struct
{
  SYSTEMCASTIC System;
  int hSvetlo[64];
  int hFlare[64];
  int iKourStopa;
} SYSTEMZHAVYCHCASTIC;

typedef struct
{
  PAR_FLEK *pCastice;
  int Sizeof;
  int flag;
  DWORD dwStart;
} SYSTEMFLEKCASTIC;

typedef struct
{
  int pFlare;
  int hSvetlo;
  int hEXSvetlo;
  int hEXSvetloAnim;
  int mesh;
  float mSpeed;
  float Speed[14];
  ITEMDESC *Item;
  void *pPointer;
  int Teleport;
  DWORD dwStart;
} FLARESTRUCT;

typedef struct
{
  char bUnderWater;
} SQUAREINFO;

typedef struct
{
  int mesh;
  int iItem;
  SYSTEMKOUROVYCHCASTIC System;
  int iStart;
  int bOn;
} BUBLSYSTEM;

typedef struct
{
  DWORD dwTime;
  int actFrame;
  int forward;
} MENUITEMANIM;

typedef struct
{
  int hSvetlo;
  int hEXSvetlo[2];
  SYSTEMKOUROVYCHCASTIC System;
  int mesh;
} EXITEFECT;

typedef struct
{
  int hSvetlo;
  float pos[3];
  unsigned long eStart;
  unsigned long eCounter;
  unsigned long ePause;
  unsigned long epCounter;
  unsigned long dwEfTime;
  unsigned long dwRiseTime;
  unsigned long dwLightTime;
  float Speed;
  int hFlare;
  float fdx;
  char bUsed;
  char bConnected;
} STREET_LIGHT_EFFECT;

typedef struct _KOFOLOVA_CARA2
{

  BOD p1, p2;
  BODRGB b1, b2;

} KOFOLOVA_CARA2;

typedef struct
{
  int hWater;
  int hVodnik[3];
  char bWater;
} WATERDESC;

typedef struct
{
  DWORD dwTime;
  DWORD dwExpTime;
  DWORD dwRealTime;
  int iItem;
  SYSTEMKOUROVYCHCASTIC sStopy;
  int iStepc;
} BEATLE_SMOKE_SYSTEM;

typedef struct
{
  PAR_KOUR_STOPA *pCastice;
  int csize;
  int hHnizdo;
  int pSystem;
  BOD pivot;
  BOD dir;
  float fLowPlain;
  int flag;
  DWORD dwTime;
  DWORD dwRegTime;
  RECT rScene;
  float pos[3];
  float speed;
  float faire_dir[3];
  int iSwampLight;
  float falngle[3];
  float fmult[3];
  float fsinspeed[3];
  float fScene[6];
  int iSound;
} FAIRY_EFFECT;

typedef struct
{
  PAR_KOUR_STOPA *pCastice;
  int csize;
  int hHnizdo;
  int pSystem;
  BOD pivot;
  BOD dir;
  float fLowPlain;
  int flag;
  float pos[3];
  float speed;
  int hSvetlo;
} CANDLE_EFFECT;

typedef struct
{
  int Size[3];                  // rozmery levelu
  int Count_Of_Objects;         // pocet objektu
  int Count_Of_Items;           // pocet predmetu
  int Count_Of_Action_Items;    // pocet akcnich prvku
  int Size_of_Level;            // velikost levelu
  OBJECTDESC *Object;           // pointer na objekty
  ITEMDESC **Level;             // mapa
  ITEMDESC *Item;               // pointer na prvky
  long *Action_Item;            // seznam spec. log. prvku (teleport, tkacitko,...)
  long *Anim_Item;              // seznam prvku, ktere se konstantne 
//      HWND                    hWnd;                                   // hWnd aplikace
  long Actual_Item;             // aktualni predmet (brouk)
  int status;                   // status hry
  int Column[3];                // nejzassi posouvany sloupec
  char bColumn;                 // identifikace jeho zvoleni
  int SecColumn[3];
  char bSecColumn;
  char bExit;                   // identfifikater stoupnuti na exit
  char Level_Exit;              // exit z levelu
  char Throw_off;               // setrasat?
  int Base_Priority;            // Zakladni priotita podle rychlosti CPU
  AUDIO_DATA *p_ad;             // Audio Data
  char Item_Lock;               // Zamikani predmetu
  char Move_Exeption;           // Vijimka z pravidel posunu
  ENVIRONMENT Environment;      // prostredi
  int Beetle[6];                // aktivni brouci;
  BEETLEANIMATION BeetleAnim[6];        // meshe brouku;
  char Beetle_Index;            // index aktivniho brouka v strukture Beetle
  char Sikmina_Flag;            // flag se nastavi, jestlize se sunulo na sikminu
  char Sikmina_FlagOld;         // udrzuje posledni flag na sikminu;
  int Actual_Qset;              // actual queue set
  int Actual_Anim_Qset;         // actual animation queue set
  char bCheck_Result;           // retrun value of gl_Check_Logical_Dependences
  char bCheck;                  // retrun value of gl_Throwoff_Test
  char Flip;                    // if animation set must be fliped
  char bAllow_Key;              // player is allows to press next key
  ITEMDESC *pWall;              // pointer na zed v levelu. Pouziva se u vytahu
  char bInventory;              // kresleni inventorare
  char bLInventory;
  char bTopLedge;               // kresleni horni listy
  char bLastTopLedge;
  char bSikminaSound;           // slo se pres sikminu
  MATERIAL_BUNKA_K *pMaterialb; // seznam animujicich se materuali
  int iMaterialb;               // pocet animujicich se materialu
  SQUAREDESC *pSquare;          // informace o spodnim patre (fleky,...)
  char bSunuti;                 // indikator, zda beruska neco sune;
  char bVypniFlek;              // indikator, ze se musi vypnout flek
  char bTriger;                 // indikator, trigeru
  SYSTEMCASTIC Exploze[20];     // 10 predvyrobenych vybuchu
  SYSTEMCASTIC Jiskra[20];      // 10 predvyrobenych vybuchu zhavych strepin
  SYSTEMCASTIC Propadla[20];    // 10 predvyrobenych propadel
  SYSTEMCASTIC Kamen[20];       // 10 predvyrobenych kamen
  SYSTEMCASTIC Krompac[10];     // 10 rozpad krompace
  int TrashFlag;                // odpadni flag
  char bPohled_Berusky;         // indikator pohledu berusky
  float fVitr[3];               // vitr v levelu
  SYSTEMZHAVYCHCASTIC ZhaveCastice[20]; // castice se zhavimi strepinamy (svetly)
  SYSTEMKOUROVYCHCASTIC KourovaStopa[20];       // system kourovych stop
  SYSTEMKOUROVYCHCASTIC Kour[20];       // kour po vybuchu;
  SYSTEMKOUROVYCHCASTIC KourKameni[20]; // kour po vybuchu;
  BEATLE_SMOKE_SYSTEM BeatleSmoke[6];
  SYSTEMKOUROVYCHCASTIC KourUst[10];    // kour z ust;
  SYSTEMFLEKCASTIC Prach[20];   // zvyrany prach po dopadu
  char Sub_Svetla;              // flag, zda pouzivat sub svetla
  LEVEL_HEADER LevelHeader;     // hlavicka nahraneho levelu
  char bSikminaUp;              // posun bedny na sikminu smerem navrch
  char bSikminaDown;
  char bSikminaUpExeption;      // vijimka na animaci na na sikmine
  FLARESTRUCT Flare[20];        // flary teleportu
  SYSTEMKOUROVYCHCASTIC TeleportSparks[20];     // jiskry po teleportaci
  SYSTEMKOUROVYCHCASTIC LiftParticles[10];      // caticky odpadavajici z vitahu za jizdy
  SYSTEMKOUROVYCHCASTIC LiftVParticles[10];     // caticky odpadavajici z vitahu za jizdy
  SYSTEMKOUROVYCHCASTIC VodniKola[10];  // kona na vode
  SYSTEMKOUROVYCHCASTIC VodniKolaB[100];        // kona na vode po pohybujicich se prvcich 
  // (beruska, bedna,...)
  SYSTEMCASTIC VodniCakanec1[10];       // System tvorici hnanove castice pri dopadu 
  // bedny do vody
  SYSTEMCASTIC VodniCakanec2[10];       // System tvorici hnanove castice pri dopadu 
  // bedny do vody
  int LastMesh;                 // mesh posledniho prvku
  FLEK_K SelectionFlek;         // flek ukazujici vybranou berusku
  SQUAREINFO *Square;
  BUBLSYSTEM BublSystem[6];     // system pro bublinky berusek pod vodou
  BUBLSYSTEM BublSystemE;       // nature effect bubl system
  BUBLSYSTEM BublSystemC;       // nature effect bubl system
  SYSTEMKOUROVYCHCASTIC BublVybuch[20]; // vybuch z bublin
  char bPosouvatKameru;         // ma se posouvat kamera s beruskou?
  int KvalitaCastic;            // kvalita castic
  int KvalitaCasticV;           // kvalita castic vody
  MENUITEMANIM MenuItemAnim[6]; // animace predmetu v inventorari
  char bMenuRunning;            // indikuje, ze bezi menu
  char bMenuButton;             // je nad tlacitkem menu
  EXITEFECT ExitEfect[10];      // efekt exitu
  SYSTEMKOUROVYCHCASTIC ExitSparks[10]; // konstantni efekt exitu
  SYSTEMKOUROVYCHCASTIC TelCSparks[10]; // konstantni efekt teleportu
  SYSTEMKOUROVYCHCASTIC BarelSparks[30];        // konstantni efekt barelu
  KOFOLOVA_CARA2 *pCara;        // list cas ke kresleni
  RAINSYSTEM Rain;              // system na dest
  SNOWSYSTEM Snow;              // system na snih
  NATUREEFFECTSYSTEM NatureESystem[10]; // system prirodnich efektu (chmiri,...)
  WATERDESC Water;              // vlny na vode
  MISTSYSTEM Mist;              // mlha
  long lLastKeyCounter;
  char bMultiConnect;
  char bCheckDependencesNeeded;
  char bB1ExtraCameraRot;
  CAMERA_STRUCT lCamera;
  long lLevel_Exitc;
  char bSetStep;
  int iNSrart;
  char bUnderWaterLevel;
  char bExtraSoundSunuti;
  STREET_LIGHT_EFFECT StreetL[8];
  STREET_LIGHT_EFFECT SwampL[10];
  RAINSYSTEM StreetRain;
  char bResetini;
  char bSaveDemo;
  FAIRY_EFFECT FairyEffect[4];
  int iFloorMaterial;
  STREET_LIGHT_EFFECT StarL[200];
  FAIRY_EFFECT StarFall[2];
  CANDLE_EFFECT CandleEffect[6];
  CANDLE_EFFECT CandleSmoke[6];
  char cLevelName[64];
  //char                                  cLevelDir[MAX_PATH+1];
  char cLoadedFrom[MAX_PATH + 1];
  DWORD dwPlayTime;
  DWORD dwStartTime;
  int iNumOfSteps;
  int iWalkAnimation;
  char bStone;
  char bStoneSound;
  char bSikminaMoveExeption;
  int iCursor;
  int iKursorMesh;
  int iKursorAnimation;
  char bRestart;
  int iMessageBoxReturn;
  char bCancelMenu;
  char bReturnToMenu;
  DWORD dwLastMusicCheck;
  char bDemoSaveExeption;
  char bGameResume;
} LEVELINFO;

typedef struct
{
  ITEMDESC Item;
  int GUID;
  int mesh;
} SAVE_FILE_ITEM;

extern int PRCameraFlag;        //indikator, ze je pozno pohybovat kamerou pomoci P&R
extern int PRCameraFlagChange;  // byl pozit pohyb pomoci P&R

int gl_Get_Active_Beetle_Mesh(void);

// musi se volat az po kom_prvotni_init
void gl_Get_PR_ScanCode(int *pPScan, int *pRScan);

void gl_Kofola_Minimalize(void);

void gl_Kofola_Maximalize(void);

int gl_Allow_Key(int iScanCode);

void gl_Kofola_End(int DirectX);

extern char bDXAktivni;

#endif
