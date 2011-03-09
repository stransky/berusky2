/*
  Svetla.h
*/

#ifndef __BERUSKY_3D_LIGHT
#define __BERUSKY_3D_LIGHT

/***********************************************************************
  Dynamicka svetla - scena 
  **********************************************************************
*/

/* Prace se scenou
*/
void  dl_scena_set_diff(float r, float g, float b, float a);
void  dl_scena_add_diff(float r, float g, float b, float a);

void  dl_scena_modulate_spec(float r, float g, float b, float a);
void  dl_scena_set_spec(float r, float g, float b, float a);
void  dl_scena_add_spec(float r, float g, float b, float a);

void  dl_scena_set_ambient(float r, float g, float b, float a);

/* Prace s meshama
*/
int  dl_mesh_modulate_diff(MeshHandle mh, float r, float g, float b, float a);
int  dl_mesh_set_diff(MeshHandle mh, float r, float g, float b, float a);
int  dl_mesh_add_diff(MeshHandle mh, float r, float g, float b, float a);

int  dl_mesh_modulate_spec(MeshHandle mh, float r, float g, float b, float a);
int  dl_mesh_set_spec(MeshHandle mh, float r, float g, float b, float a);
int  dl_mesh_add_spec(MeshHandle mh, float r, float g, float b, float a);

/* ********************************************************
   Dyn. svetla - jednoducha
   ********************************************************
*/

#define SDL_ADD_SUB         0x00000080 // svetlo pracuje v komplementarnim add/sub modu
#define SDL_MESH            0x00000100 // svetlo patri k urcitemu meshi
#define SDL_SUB             0x00000200 // svetla se nepricitaji ale moduluji
#define SDL_UTLUM_LINEAR    0x00000400 // linearni utlum - defaultni
#define SDL_UTLUM_KVADRATIC 0x00000800 // kvadraticky utlum

/* Moje flagy
*/
#define SDL_REMOVE          0x10000000 // svetlo se ma smazat
#define SDL_ANIMACE         0x20000000 // svetlo se prave animuje
#define SDL_ZMENA_LOKAL     0x40000000 // lokalni zmena svetla
#define SDL_ZMENA_GLOBAL    0x80000000 // globalni zmena svetla
#define SDL_KOFOLA          0x08000000 // kofolovo svetlo

/* Nacteni seznamu existujich dynamickych svetel
*/
typedef struct _LIGHT_INFO {

  int    light_handle;     // handle na svetlo (dynamicke/extra dynamicke)
  byte   jmeno[MAX_JMENO]; // jeho jmeno
  int    kof_cislo;        // kofolovo cislo svetla

} LIGHT_INFO;

// cte obycejna svetla
int sdl_get_num(void);
int sdl_get_list(LIGHT_INFO *p_li, int max);

// cte extra svetla
int edl_get_num(void);
int edl_get_list(LIGHT_INFO *p_li, int max);


/* Vyrobi/zrusi svetlo
*/
LightHandle sdl_svetlo_vyrob(dword flag);
LightHandle sdl_svetlo_pojmenuj(LightHandle handle, byte *p_jmeno);
int         sdl_svetlo_zrus(LightHandle handle);

/* Jednorazove nastaveni
*/
LightHandle sdl_svetlo_set_pos(LightHandle lh, BOD *p_bod);
LightHandle sdl_svetlo_set_pos_ext(LightHandle lh, float x, float y, float z);
LightHandle sdl_svetlo_set_diff(LightHandle lh, float dr, float dg, float db, float min, float max, float dosah);
LightHandle sdl_svetlo_set_diff_sub(LightHandle lh, float sr, float sg, float sb);


/* Jednoducha animace - od katualniho nastaveni po nove
   s autoremove
*/
RunHandle   sdl_svetlo_anim(LightHandle handle, BOD *p_bod, 
                            float dr, float dg, float db, float da, 
                            float dosah, float min, float max,
                            int flag, int *p_flag, int framenum);

/* Animace svetel
*/
LightHandle sdl_anim_vyrob(LightHandle lh, int framenum, int pos_keys, int diff_keys, int dosah_keys);
LightHandle sdl_anim_zrus(LightHandle lh);

LightHandle sdl_anim_vloz_klic_posun(LightHandle lh, int klic, float x, float y, float z, int frame);
LightHandle sdl_anim_vloz_klic_posun_bod(LightHandle lh, int klic, BOD *p_p, int frame);

LightHandle sdl_anim_vloz_klic_diff(LightHandle lh, int klic, float r, float g, float b, float a, int frame);

LightHandle sdl_anim_vloz_klic_vzdal(LightHandle lh, int klic, float dosah, float min, float max, int frame);

/* Run/Stop animaci
   Akceptovane flagy do promenne flag:
   GK_LOOP       - animace se bude opakovat
   GK_REMOVE     - animace se po dobehnuti smaze i se svetlem
*/
RunHandle   sdl_anim_start(LightHandle lh, int *p_flag, int flag, int start, int stop);
int         sdl_anim_stop(RunHandle rh);
void        sdla_animaci_zrus_online(DYN_LIGHT_ANIM *p_an);


/* ********************************************************
   Dyn. svetla - extra svetla, v akci na:
   - diffusni svetla
   - spekularni svetla
   - textury u1 - env/flat mapping
   - textury u2
*/

// Nahraje svetlo ze sim animace
ExtraLightHandle edl_svetlo_nahraj(byte *p_file);

// Vyrobi/Zrusi svetlo
// Prida/Odebere meshe svetla

ExtraLightHandle edl_svetlo_vyrob(dword flag, int meshnum);
ExtraLightHandle edl_svetlo_vyrob_zemni_mlha(dword barva, float v_start, float v_stop, float h_start, float h_stop);
int              edl_svetlo_zrus(ExtraLightHandle handle);

ExtraLightHandle edl_svetlo_pridej_mesh(ExtraLightHandle lh, MeshHandle mh);
ExtraLightHandle edl_svetlo_uzavri_meshe(ExtraLightHandle lh);
ExtraLightHandle edl_svetlo_zrus_meshe(ExtraLightHandle lh);

// Jednorazove nastaveni

ExtraLightHandle edl_svetlo_set_pos(ExtraLightHandle lh, BOD *p_bod, BOD *p_dir);
ExtraLightHandle edl_svetlo_set_pos_ext(ExtraLightHandle lh, float px, float py, float pz, float dx, float dy, float dz);
ExtraLightHandle edl_svetlo_set_par(ExtraLightHandle lh, float utlum, float dosah, float theta, float gourand);
ExtraLightHandle edl_svetlo_set_diff(ExtraLightHandle lh, float da);
ExtraLightHandle edl_svetlo_set_spec(ExtraLightHandle lh, float sr, float sg, float sb);

// Jednoducha animace
RunHandle   edl_svetlo_anim(ExtraLightHandle handle, BOD *p_bod, BOD *p_dir, float dr, float dg, float db, float da, float sr, float sg, float sb, float sa, float k2, float dosah, float theta, int flag, int *p_flag, int framenum);

// Slozite animace svetel

ExtraLightHandle edl_anim_vyrob(ExtraLightHandle lh, int framenum, int pos_keys, int dir_keys, int diff_keys, int spec_keys, int dosah_keys);
ExtraLightHandle edl_anim_zrus(ExtraLightHandle lh);

ExtraLightHandle edl_anim_vloz_klic_posun(ExtraLightHandle lh, int klic, float x, float y, float z, int frame);
ExtraLightHandle edl_anim_vloz_klic_posun_bod(ExtraLightHandle lh, int klic, BOD *p_p, int frame);

ExtraLightHandle edl_anim_vloz_klic_dir(ExtraLightHandle lh, int klic, float x, float y, float z, int frame);
ExtraLightHandle edl_anim_vloz_klic_dir_bod(ExtraLightHandle lh, int klic, BOD *p_dir, int frame);

ExtraLightHandle edl_anim_vloz_klic_diff(ExtraLightHandle lh, int klic, float a, int frame);
ExtraLightHandle edl_anim_vloz_klic_spec(ExtraLightHandle lh, int klic, float r, float g, float b, int frame);

ExtraLightHandle edl_anim_vloz_klic_vzdal(ExtraLightHandle lh, int klic, float dosah, float k2, float k3, int frame);

// Prehravani slozitych animaci svetel

RunHandle   edl_anim_start(ExtraLightHandle lh, int *p_flag, int flag, int start, int stop);
int         edl_anim_stop(RunHandle rh);



/***********************
    Servisni rutiny
  **********************
*/

int  dl_mesh_pridej_svetla(G_KONFIG *p_ber, GAME_MESH *p_mesh);
int  edl_mesh_pridej_svetla(G_KONFIG *p_ber, GAME_MESH *p_mesh);
int  dl_mesh_pripocitej_svetla(G_KONFIG *p_ber, GAME_MESH *p_mesh, int dyn_light, int extra_light, int full_light, int all);

void edla_updatuj(G_KONFIG *p_ber);
void sdla_updatuj(G_KONFIG *p_ber);
void dl_renderuj_svetla(G_KONFIG *p_ber);
void dl_updatuj(G_KONFIG *p_ber);


/* *****************************************************************************
     Flare
   *****************************************************************************
*/

/* Klasicka vyroba/zruseni
*/
FlareHandle kom_flare_vyrob(int flag);
void        kom_flare_zrus(FlareHandle fh);

/* Nastaveni vlastnosti flare, alfa muze byt GL_ONE,GL_ONE_MINUS_SRC_ALPHA
*/
FlareHandle kom_flare_set_param(FlareHandle fh, float r, float g, float b, float a, float dx, float dy, int material, int alfa);

/* Nastaveni pointeru na vazaci bod
*/
FlareHandle kom_flare_set_pivot(FlareHandle fh, BOD *p_pivot);

/* Nastaveni pevne pozice
*/
FlareHandle kom_flare_set_pos(FlareHandle fh, BOD *p_pos);

/* Nacte pointer flaru pro primou praci s nim
*/
LENS_FLARE * kom_flare_get_pointer(FlareHandle fh);

/* Nastaveni kvality svetel
*/
void dl_nastav_kvalitu(G_KONFIG *p_ber);
void dl_nastav(G_KONFIG *p_ber);

/* Reset mlhy
*/
void mlha_reset(void);

/* Rychlosti svetel
*/
#define  LIGHT_ALL      1 // vsechno
#define  LIGHT_PRVKY    2 // pouze prvky
#define  LIGHT_VERTEXY  3 // poze vertexove prvky
#define  LIGHT_NIC      4 // pouze beruska

#endif