/*
  Berusky
*/

#ifndef __BERUSKY_3D_LOAD
#define __BERUSKY_3D_LOAD

typedef struct _GAMESLOT
{

  char jmeno[30];
  int vzor;
  GLMATRIX word;
  char rezerved[10];

} GAMESLOT;

int ber_load_level(G_KONFIG * p_ber, char *p_file);
int ber_pridej_prvek_databaze(G_KONFIG * p_ber, SECONDDATADESC * p_scd,
  int guid);
void ber_zrus_level(G_KONFIG * p_ber);
PRVEK_LEVELU_GAME **vyrob_level(int x, int y, int z);
void ber_vyrob_level(G_KONFIG * p_ber, int x, int y, int z);
int ber_prvek_disk_do_bunky(G_KONFIG * p_ber, BUNKA_LEVELU_DISK * p_disk,
  MeshHandle *p_handle, int ID, int x, int y, int z);
int ber_je_objekt_voda(int guid);
int ber_nahraj_materialy(G_KONFIG * p_ber, char *p_jmeno, char *p_dir);
int ber_nahraj_scenu(G_KONFIG * p_ber, char *p_jmeno, char *p_dir, int reload,
  BUNKA_LEVELU_DISK * p_berusky, int bernum);
void ber_nahraj_poly(G_KONFIG * p_ber, char *p_jmeno, char *p_dir);
int ber_nahraj_materialy_frame_animace(G_KONFIG * p_ber, char *p_jmeno,
  char *p_dir);
void ber_nahraj_lightmap(G_KONFIG * p_ber, char *p_jmeno, char *p_dir);
bool ber_nahraj_mesh(G_KONFIG * p_ber, char *p_jmeno, GAME_MESH_OLD ** p_mesh, int json_export = FALSE);
void ber_velikost_sceny(G_KONFIG * p_ber);

void ber_umisti_prvek(MeshHandle handle, int x, int y, int z, int rotace);
void ber_umisti_prvek_abs(PRVEK_LEVELU_GAME * p_lev, float x, float y,
  float z, int rotace);
int ber_mesh_do_bunky(G_KONFIG * p_ber, int mesh, int *p_handle);

GAME_MESH_OLD **ber_mesh_vyber_staticke(G_KONFIG * p_ber, int *p_snum,
  int **p_indexy);

int ber_uprava_materialu_bump_mapping(G_KONFIG * p_ber);

inline int ber_je_mesh_beruska_fast(GAME_MESH_OLD * p_mesh)
{
  return (p_mesh->p_data->k2flag & KONT2_BERUSKA);
}

inline int ber_je_mesh_prvek_fast(GAME_MESH_OLD * p_mesh)
{
  return ((p_mesh->p_data->kflag) & KONT_PRVEK);
}

#define JMENO_KURZOR "sv2.b2m"

#endif
