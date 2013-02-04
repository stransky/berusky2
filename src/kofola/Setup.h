#ifndef __SETUP_
#define __SETUP_

#define POCETLISTU 8
#define POCETKLAVES 128
#define POCETAKTICNICHKLAVES 40

typedef struct
{
  char no_sound;
  char no_menu;
  char camera_type;
  char log_file[256];
  char fullscreen;
  char ditering;  
  char text_detail;             //Texture-level - Uroven detailu textur (0 = max)
  char text_detail_light;       //Uroven detailu textur lightmap (0 = max)  
  char text_mip_mapping;        //Generovat pro startu mip-mapy + jejich povoleni  TF
  char text_mip_filtr;          //Filtr mip-map 0 - linearni/1 - trilinearni TF
  char channels;
  long pre_load;
  char soundvolume;
  char musicvolume;
  char ambientvolume;
  int  key[POCETKLAVES];
  char bitmap_dir[256];
  char data_dir[256];
  char prvky_dir[256];
  char level_dir[256];
  char out_dir[256];
  char texture_dir[256];
  char material_dir[256];
  char game_level_dir[256];
  char scene_dir[256];
  char game_data_dir[256];
  char music_dir[256];
  char sound_dir[256];
  char save_dir[256];
  char e_ditering;
  char mazat_dummy;
  char kolize_materialu;
  char load_prvky;
  char load_textury;
  char load_default_materialy;
  char default_materialy[256];
  char obalky_kontejner;
  char obalky_objekt;
  char berusky_hra[256];
  char e_log_file[256];
  char debug_vypisy;
  char debug_light;
  char pohled_berusky;
  char kvalita_castic;
  char kvalita_casticv;
  char kvalita_casticp;
  char ovladani;
  char debug_fps;
  char lista_prvku;
  char berusky_hra_dir[256];
  char debug_draty;
  char sada[20][256];
  char posouvat_kameru;
  char extra_light_vertex;
  float text_ostrost;
  float p_kamera_alfa;
  float p_kamera_radius;
  char init_visibility;
  char cursor[32];
  char hires_textury;
  char zrcado_aktivni;
  char debug_kamery;
  char text_ans;  
  int  text_ans_stupen;
  int  start_zpruhlednovani;
  int  start_zvyraznovani;
  int  ovladani_rohy;
  int  ovladani_rohy_default;
  int  ovladani_rohy_smer;
  float ovladani_rohy_rychlost;
  int  ovladani_pr_posun;
  int  animace_okoli;
  char camera_intro;
} SETUP;

void Load_ini(void);
void Save_ini(void);

#endif
