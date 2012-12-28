/*
  Universalni definice pro berusky - do editoru a do hry

  Kofola:
  -------

  adas_ -> sound system
  gl_   -> logika
  lsi_  -> load/save/init
  gi_   -> game init
  ko_   -> kofolovy fce v editoru

  Komat:
  ------
  be_   -> berusky editor
  ber_  -> berusky hra
*/


#ifndef __BERUSKY_UNIVERSAL
#define __BERUSKY_UNIVERSAL

#define VERZE_KOMAT            "MASTER 1.2"

#ifdef  _DEBUG
#define DEBUG_MOD               // sbiraji se citlive informace
//#define DEBUG_MOD_OBALKY  // sbiraji se citlive informace
#endif

#define MAX_VZDAL_KAMERY       70
#define MIN_VZDAL_KAMERY       10

#define MAX_UHEL_KAMERY        DEG2RAD(85.0f)   //max uhel 85 stupnu
#define MIN_UHEL_KAMERY        DEG2RAD(30.0f)   //min uhel 35 stupnu

#define MAX_CELKEM_MATERIALU   1000
#define MAX_CELKEM_TEXTUR      500
#define MAX_RAY_TEXTUR         1000

#define MAX_BERUSKY_KONTEJNERU 2000
#define MAX_TOP_KONTEJNERU     100
#define MAX_BERUSKY_DATABAZE   100
#define MAX_PRVKU_DATABAZE     1000
#define MAX_PRVKU_LEVEL        1000
#define MAX_PRVKU_LEVEL_TOP    100
#define MAX_PRVKU_LEVEL_LOW    200
#define MAX_PATER              100

#define MAX_FLARE_SVETEL       100
#define MAX_STATE_BLOKU        100      // celkovy pocet state bloku

#define MAX_PRHL_OBJEKTU       1000

#define X_ZACATEK              (p_bnf->x_start)
#define Y_ZACATEK              (p_bnf->y_start)
#define Z_ZACATEK              (p_bnf->z_start)

#define X_PRVEK                2
#define Y_PRVEK                2
#define Z_PRVEK                2

#define X_PRVEK2               (X_PRVEK>>1)
#define Y_PRVEK2               (Y_PRVEK>>1)
#define Z_PRVEK2               (Z_PRVEK>>1)

typedef struct _LEVEL_HEADER
{

  int magic;                    // magicke cislo
  int x, z, y;                  // rozmery levelu
  // LV 6
  int prvku;                    // pocet prvku v levelu
  float x_start,                // starty levelu
    y_start, z_start;
  int rezerved[97];             // rezervovano

} LEVEL_HEADER;

typedef struct _BUNKA_LEVELU_DISK
{

  int guid;
  int Vazba1;
  int Vazba2;
  int Rotace;
  int mesh;                     // provazani se scenou
  int rez[10];                  // rezerva

} BUNKA_LEVELU_DISK;

typedef int      MeshHandle;         // handle to mesh/item
typedef int      ExMeshHandle;       // handle extra-mesh (neni to prvek ale jen mesh)
typedef int      DataHandle;         // handle meshe/prvku v datazazi
typedef int      AnimHandle;         // handle simple animace (polotovaru animace)
typedef size_ptr RunHandle;          // handle bezici animace
typedef size_ptr MatrixHandle;       // handle matice
typedef int      MatHandle;          // handle animovaneho materialu
typedef int      LightHandle;        // handle bezneho dynamickeho svetla
typedef int      ExtraLightHandle;   // handle extra-dynamickeho svetla
typedef size_ptr FlekHandle;         // handle na fleky
typedef size_ptr ParHandle;          // handle na casticovac
typedef size_ptr FlareHandle;        // handle na flare
typedef size_ptr HnizdoHandle;       // handle na hnizdo
typedef size_ptr TextHandle;         // handle na texturovou animaci
typedef size_ptr VodaHandle;         // handle na vodni system
typedef size_ptr VodnikHandle;       // handle na vodnika ve vodnim systemu
typedef size_ptr ChapadelnikHandle;  // handle na chapadelnika s chapadlama
typedef size_ptr ChapadloHandle;     // handle na chapadlo

#define FRAMU_PER_SEC         20
#define DELKA_SEC             1000

#define KONCOVKA_LEVELU       ".lv6"
#define MASKA_LEVELU          "*.lv6"

#define KONCOVKA_SCENY_OLD    ".sc2"

#define KONCOVKA_SCENY        ".bpr"
#define MASKA_SCENY           "*.bpr"

#define KONCOVKA_MESH_OLD     ".out"
#define MASKA_MESH_OLD        "*.out"

#define KONCOVKA_MESH         ".b2m"
#define MASKA_MESH            "*.b2m"

#define KONCOVKA_MATERIAL_OLD ".mnt"
#define MASKA_MATERIAL_OLD    "*.mnt"

#define KONCOVKA_MATERIAL     ".b2t"
#define MASKA_MATERIAL        "*.b2t"

#define KONCOVKA_LIGHTMAPY    ".b2l"
#define MASKA_LIGHTMAPY       "*.b2l"

#define LIST_PRVKU            "prvky.txt"
#define DEFAULT_LEVEL         "a.lv6"

#define GUID_PRAZDNA_STENA     21000    // guid neviditelne steny (bez modelu)

#define KONT_NEVID_STENA       0
#define KONT_NEVID_PRVEK       1

#define MAX_TRID               30
#define MAX_PODTRID            30
#define MAX_TRID_JMENO         50

#define DELICKA_CASU           1

// pokud je objekt s guid staticky vraci 1
// 
inline int ber_je_objekt_staticky(int guid)
{
  int trida = guid / 1000;

  return (trida == 2 || trida == 4 || trida == 9 || trida == 17 ||
    trida == 19 || trida == 20 || trida == 12);
}

inline int ber_je_objekt_beruska(int guid)
{
  int trida = guid / 1000;

  return (trida == 1);
}

inline int ber_je_objekt_staticky_bez_exitu(int guid)
{
  int trida = guid / 1000;

  return (trida == 2 || trida == 9 || trida == 17 ||
    trida == 19 || trida == 20 || trida == 12);
}

#endif
