#include "3d_all.h"
#include "Berusky3d_kofola_interface.h"
#include "Setup.h"

SETUP setup;

void Save_ini(void)
{
  char text[256];

  setup.kvalita_castic = setup.kvalita_casticp = setup.kvalita_casticv;
  setup.text_detail_light = setup.text_detail;

  WritePrivateProfileString("game", "camera_type", itoa(setup.camera_type, text, 10), ini_file);
  WritePrivateProfileString("game", "fullscreen", itoa(setup.fullscreen, text, 10), ini_file);
  WritePrivateProfileString("game", "ditering", itoa(setup.ditering, text, 10), ini_file);  
  WritePrivateProfileString("game", "ovladani", itoa(setup.ovladani, text, 10), ini_file);
  WritePrivateProfileString("game", "posouvat_kameru", itoa(setup.posouvat_kameru, text, 10), ini_file);
  WritePrivateProfileString("game", "init_visibility", itoa(setup.init_visibility, text, 10), ini_file);
  WritePrivateProfileString("game", "zrcado_aktivni", itoa(setup.zrcado_aktivni, text, 10), ini_file);
  sprintf(text, "%f", setup.p_kamera_alfa);
  WritePrivateProfileString("game", "p_kamera_alfa", text, ini_file);
  sprintf(text, "%f", setup.p_kamera_radius);
  WritePrivateProfileString("game", "p_kamera_radius", text, ini_file);
  WritePrivateProfileString("game", "cursor", setup.cursor, ini_file);
  WritePrivateProfileString("game", "camera_intro", itoa(setup.camera_intro, text, 10), ini_file);

  sprintf(text, "%f", setup.ovladani_rohy_rychlost);
  WritePrivateProfileString("game", "ovladani_rohy_rychlost", text, ini_file);
  WritePrivateProfileString("game", "start_zpruhlednovani", itoa(setup.start_zpruhlednovani, text, 10), ini_file);
  WritePrivateProfileString("game", "start_zvyraznovani", itoa(setup.start_zvyraznovani, text, 10), ini_file);
  WritePrivateProfileString("game", "ovladani_rohy", itoa(setup.ovladani_rohy, text, 10), ini_file);
  WritePrivateProfileString("game", "ovladani_rohy_default", itoa(setup.ovladani_rohy_default, text, 10), ini_file);
  WritePrivateProfileString("game", "ovladani_rohy_smer", itoa(setup.ovladani_rohy_smer, text, 10), ini_file);
  WritePrivateProfileString("game", "ovladani_pr_posun", itoa(setup.ovladani_pr_posun, text, 10), ini_file);

  WritePrivateProfileString("textures", "text_detail_light", itoa(setup.text_detail_light, text, 10), ini_file);
  WritePrivateProfileString("textures", "text_mip_mapping", itoa(setup.text_mip_mapping, text, 10), ini_file);
  WritePrivateProfileString("textures", "text_mip_filtr", itoa(setup.text_mip_filtr, text, 10), ini_file);
  WritePrivateProfileString("textures", "text_hi_res", itoa(setup.hires_textury, text, 10), ini_file);
  WritePrivateProfileString("textures", "text_ans", itoa(setup.text_ans, text, 10), ini_file);
  WritePrivateProfileString("textures", "text_ans_level", text, ini_file);
  sprintf(text, "%f", setup.text_ostrost);
  WritePrivateProfileString("textures", "text_ostrost", text, ini_file);

  WritePrivateProfileString("textures", "text_ans_stupen", itoa(setup.text_ans_stupen, text, 10), ini_file);

  WritePrivateProfileString("soundengine", "channels", itoa(setup.channels, text, 10), ini_file);
  WritePrivateProfileString("soundengine", "pre_load", itoa(setup.pre_load, text, 10), ini_file);
  WritePrivateProfileString("soundengine", "soundvolume", itoa(setup.soundvolume, text, 10), ini_file);
  WritePrivateProfileString("soundengine", "musicvolume", itoa(setup.musicvolume, text, 10), ini_file);
  WritePrivateProfileString("soundengine", "ambientvolume", itoa(setup.ambientvolume, text, 10), ini_file);

  WritePrivateProfileString("controls", "move_forward", itoa(setup.key[1], text, 10), ini_file);
  WritePrivateProfileString("controls", "turn_back", itoa(setup.key[2], text, 10), ini_file);
  WritePrivateProfileString("controls", "turn_left", itoa(setup.key[3], text, 10), ini_file);
  WritePrivateProfileString("controls", "turn_right", itoa(setup.key[4], text, 10), ini_file);
  WritePrivateProfileString("controls", "next_beatle", itoa(setup.key[5], text, 10), ini_file);
  WritePrivateProfileString("controls", "beatle1", itoa(setup.key[6], text, 10), ini_file);
  WritePrivateProfileString("controls", "beatle2", itoa(setup.key[7], text, 10), ini_file);
  WritePrivateProfileString("controls", "beatle3", itoa(setup.key[8], text, 10), ini_file);
  WritePrivateProfileString("controls", "beatle4", itoa(setup.key[9], text, 10), ini_file);
  WritePrivateProfileString("controls", "beatle5", itoa(setup.key[10], text, 10), ini_file);

  WritePrivateProfileString("controls", "camera_rotation", itoa(setup.key[12], text, 10), ini_file);
  WritePrivateProfileString("controls", "camera_move", itoa(setup.key[13], text, 10), ini_file);
  WritePrivateProfileString("controls", "camera_zoom_in", itoa(setup.key[14], text, 10), ini_file);
  WritePrivateProfileString("controls", "camera_zoom_out", itoa(setup.key[15],  text, 10), ini_file);
  WritePrivateProfileString("controls", "camera_fast_turn_left", itoa(setup.key[16], text, 10), ini_file);
  WritePrivateProfileString("controls", "camera_fast_turn_right", itoa(setup.key[17], text, 10), ini_file);
  WritePrivateProfileString("controls", "camera_center", itoa(setup.key[18], text, 10), ini_file);

  WritePrivateProfileString("controls", "menu", itoa(setup.key[20], text, 10), ini_file);
  WritePrivateProfileString("controls", "restart", itoa(setup.key[21], text, 10), ini_file);
  WritePrivateProfileString("controls", "item_lock", itoa(setup.key[22], text, 10), ini_file);
  //WritePrivateProfileString("controls","blow_detonation_pack",itoa(setup.key[23],text,10),ini_file);
  WritePrivateProfileString("controls", "next_song", itoa(setup.key[23], text, 10), ini_file);
  WritePrivateProfileString("controls", "disable_inventory", itoa(setup.key[24], text, 10), ini_file);
  WritePrivateProfileString("controls", "disable_top_ledge", itoa(setup.key[25], text, 10), ini_file);
  WritePrivateProfileString("controls", "screenshot", itoa(setup.key[26], text, 10), ini_file);
  WritePrivateProfileString("controls", "visibility", itoa(setup.key[27], text, 10), ini_file);
  WritePrivateProfileString("controls", "highlight", itoa(setup.key[28], text, 10), ini_file);
  WritePrivateProfileString("controls", "hint", itoa(setup.key[29], text, 10), ini_file);
  WritePrivateProfileString("controls", "pause", itoa(setup.key[30], text, 10), ini_file);
}

void Load_ini(void)
{
  char text[256];

  setup.no_sound = GetPrivateProfileInt("game", "no_sound", 0, ini_file);
  setup.camera_type = GetPrivateProfileInt("game", "camera_type", 0, ini_file);
  setup.fullscreen = GetPrivateProfileInt("game", "fullscreen", 0, ini_file);
  setup.ditering = GetPrivateProfileInt("game", "ditering", 0, ini_file);
  setup.pohled_berusky = GetPrivateProfileInt("game", "pohled_berusky", 0, ini_file);
  setup.kvalita_castic = GetPrivateProfileInt("game", "kvalita_castic", 0, ini_file);
  setup.kvalita_casticv = GetPrivateProfileInt("game", "kvalita_casticv", 0, ini_file);
  setup.kvalita_casticp = GetPrivateProfileInt("game", "kvalita_casticp", 0, ini_file);
  setup.ovladani = GetPrivateProfileInt("game", "ovladani", 0, ini_file);
  setup.posouvat_kameru = GetPrivateProfileInt("game", "posouvat_kameru", 0, ini_file);
  setup.extra_light_vertex = GetPrivateProfileInt("game", "extra_light_vertex", 0, ini_file);
  setup.init_visibility = GetPrivateProfileInt("game", "init_visibility", 0, ini_file);
  setup.zrcado_aktivni = GetPrivateProfileInt("game", "zrcado_aktivni", 0, ini_file);
  
  GetPrivateProfileString("game", "p_kamera_alfa", "0.35", text, 256, ini_file);
  setup.p_kamera_alfa = (float) atof(text);
  GetPrivateProfileString("game", "p_kamera_radius", "3.0", text, 256, ini_file);
  setup.p_kamera_radius = (float) atof(text);
  
  GetPrivateProfileString("game", "cursor", "cursor", setup.cursor, 32, ini_file);
  GetPrivateProfileString("game", "ovladani_rohy_rychlost", "0.5", text, 256, ini_file);
  setup.ovladani_rohy_rychlost = (float) atof(text);
  
  setup.start_zpruhlednovani = GetPrivateProfileInt("game", "start_zpruhlednovani", 0, ini_file);
  setup.start_zvyraznovani = GetPrivateProfileInt("game", "start_zvyraznovani", 0, ini_file);
  setup.ovladani_rohy = GetPrivateProfileInt("game", "ovladani_rohy", 0, ini_file);
  setup.ovladani_rohy_default = GetPrivateProfileInt("game", "ovladani_rohy_default", 0, ini_file);
  setup.ovladani_rohy_smer = GetPrivateProfileInt("game", "ovladani_rohy_smer", 0, ini_file);
  setup.ovladani_pr_posun = GetPrivateProfileInt("game", "ovladani_pr_posun", 0, ini_file);
  setup.animace_okoli = GetPrivateProfileInt("game", "animace_okoli", 1, ini_file);
  setup.camera_intro = GetPrivateProfileInt("game", "camera_intro", 0, ini_file);

  setup.text_detail = GetPrivateProfileInt("textures", "text_detail", 0, ini_file);
  setup.text_detail_light = GetPrivateProfileInt("textures", "text_detail_light", 0, ini_file);
  setup.text_mip_mapping = GetPrivateProfileInt("textures", "text_mip_mapping", 0, ini_file);
  setup.text_mip_filtr = GetPrivateProfileInt("textures", "text_mip_filtr", 0, ini_file);
  setup.hires_textury = GetPrivateProfileInt("textures", "text_hi_res", 0, ini_file);
  setup.text_ans = GetPrivateProfileInt("textures", "text_ans", 0, ini_file);
  setup.text_ans_stupen = GetPrivateProfileInt("textures", "text_ans_stupen", 0, ini_file);

  GetPrivateProfileString("textures", "text_ostrost", "1", text, 256, ini_file);
  setup.text_ostrost = (float) atof(text);

  setup.channels = GetPrivateProfileInt("soundengine", "channels", 0, ini_file);
  setup.pre_load = GetPrivateProfileInt("soundengine", "pre_load", 0, ini_file);
  setup.soundvolume = GetPrivateProfileInt("soundengine", "soundvolume", 0, ini_file);
  setup.musicvolume = GetPrivateProfileInt("soundengine", "musicvolume", 0, ini_file);
  setup.ambientvolume = GetPrivateProfileInt("soundengine", "ambientvolume", 0, ini_file);

  setup.key[0] = -1;
  setup.key[1] = GetPrivateProfileInt("controls", "move_forward", 0, ini_file);
  setup.key[2] = GetPrivateProfileInt("controls", "turn_back", 0, ini_file);
  setup.key[3] = GetPrivateProfileInt("controls", "turn_left", 0, ini_file);
  setup.key[4] = GetPrivateProfileInt("controls", "turn_right", 0, ini_file);
  setup.key[5] = GetPrivateProfileInt("controls", "next_beatle", 0, ini_file);
  setup.key[6] = GetPrivateProfileInt("controls", "beatle1", 0, ini_file);
  setup.key[7] = GetPrivateProfileInt("controls", "beatle2", 0, ini_file);
  setup.key[8] = GetPrivateProfileInt("controls", "beatle3", 0, ini_file);
  setup.key[9] = GetPrivateProfileInt("controls", "beatle4", 0, ini_file);
  setup.key[10] = GetPrivateProfileInt("controls", "beatle5", 0, ini_file);
  setup.key[11] = -1;
  setup.key[12] = GetPrivateProfileInt("controls", "camera_rotation", 0, ini_file);
  setup.key[13] = GetPrivateProfileInt("controls", "camera_move", 0, ini_file);
  setup.key[14] = GetPrivateProfileInt("controls", "camera_zoom_in", 0, ini_file);
  setup.key[15] = GetPrivateProfileInt("controls", "camera_zoom_out", 0, ini_file);
  setup.key[16] = GetPrivateProfileInt("controls", "camera_fast_turn_left", 0, ini_file);
  setup.key[17] = GetPrivateProfileInt("controls", "camera_fast_turn_right", 0, ini_file);
  setup.key[18] = GetPrivateProfileInt("controls", "camera_center", 0, ini_file);
  setup.key[19] = -1;
  setup.key[20] = GetPrivateProfileInt("controls", "menu", 0, ini_file);
  setup.key[21] = GetPrivateProfileInt("controls", "restart", 0, ini_file);
  setup.key[22] = GetPrivateProfileInt("controls", "item_lock", 0, ini_file);
  //setup.key[23] = GetPrivateProfileInt("controls", "blow_detonation_pack",0,ini_file);
  setup.key[23] = GetPrivateProfileInt("controls", "next_song", 0, ini_file);
  setup.key[24] = GetPrivateProfileInt("controls", "disable_inventory", 0, ini_file);
  setup.key[25] = GetPrivateProfileInt("controls", "disable_top_ledge", 0, ini_file);
  setup.key[26] = GetPrivateProfileInt("controls", "screenshot", 0, ini_file);
  setup.key[27] = GetPrivateProfileInt("controls", "visibility", 0, ini_file);
  setup.key[28] = GetPrivateProfileInt("controls", "highlight", 0, ini_file);
  setup.key[29] = GetPrivateProfileInt("controls", "hint", 0, ini_file);
  setup.key[30] = GetPrivateProfileInt("controls", "pause", 0, ini_file);
  setup.key[31] = -1;

  setup.debug_vypisy = GetPrivateProfileInt("debug", "debug_vypisy", 0, ini_file);
  setup.debug_light = GetPrivateProfileInt("debug", "debug_light", 0, ini_file);
  setup.debug_fps = GetPrivateProfileInt("debug", "debug_fps", 0, ini_file);
  setup.debug_draty = GetPrivateProfileInt("debug", "debug_draty", 0, ini_file);
  setup.debug_kamery = GetPrivateProfileInt("debug", "debug_kamery", 0, ini_file);

  GetPrivateProfileString("files", "system_material_1", "", setup.sada[0], MAX_FILENAME, ini_file);
  GetPrivateProfileString("files", "system_material_2", "", setup.sada[1], MAX_FILENAME, ini_file);
  GetPrivateProfileString("files", "system_material_3", "", setup.sada[2], MAX_FILENAME, ini_file);
  GetPrivateProfileString("files", "system_material_4", "", setup.sada[3], MAX_FILENAME, ini_file);
  GetPrivateProfileString("files", "system_material_5", "", setup.sada[4], MAX_FILENAME, ini_file);
  GetPrivateProfileString("files", "system_material_6", "", setup.sada[5], MAX_FILENAME, ini_file);
  GetPrivateProfileString("files", "system_material_7", "", setup.sada[6], MAX_FILENAME, ini_file);
  GetPrivateProfileString("files", "system_material_8", "", setup.sada[7], MAX_FILENAME, ini_file);
  GetPrivateProfileString("files", "system_material_9", "", setup.sada[8], MAX_FILENAME, ini_file);
  GetPrivateProfileString("files", "system_material_10", "", setup.sada[9], MAX_FILENAME, ini_file);
  GetPrivateProfileString("files", "system_material_11", "", setup.sada[10], MAX_FILENAME, ini_file);
  GetPrivateProfileString("files", "system_material_12", "", setup.sada[11], MAX_FILENAME, ini_file);
  GetPrivateProfileString("files", "system_material_13", "", setup.sada[12], MAX_FILENAME, ini_file);
  GetPrivateProfileString("files", "system_material_14", "", setup.sada[13], MAX_FILENAME, ini_file);
  GetPrivateProfileString("files", "system_material_15", "", setup.sada[14], MAX_FILENAME, ini_file);
  GetPrivateProfileString("files", "system_material_16", "", setup.sada[15], MAX_FILENAME, ini_file);
  GetPrivateProfileString("files", "system_material_17", "", setup.sada[16], MAX_FILENAME, ini_file);
  GetPrivateProfileString("files", "system_material_18", "", setup.sada[17], MAX_FILENAME, ini_file);
  GetPrivateProfileString("files", "system_material_19", "", setup.sada[18], MAX_FILENAME, ini_file);
  GetPrivateProfileString("files", "system_material_20", "", setup.sada[19], MAX_FILENAME, ini_file);
}
