#include "..\\komat\\mss_on.h"
#include <windows.h>
#include "..\komat\3d_all.h"
#include "..\Komat\berusky3d_kofola_interface.h"
#include "setup.h"

SETUP setup;

void Save_ini(void)
{
	char text[256];

	setup.kvalita_castic = setup.kvalita_casticp = setup.kvalita_casticv;
	setup.text_detail_light = setup.text_detail;

	//WritePrivateProfileString("hra","no_menu",itoa(setup.no_menu,text,10),ini_file);
	//WritePrivateProfileString("hra","no_sound",itoa(setup.no_sound,text,10),ini_file);
	WritePrivateProfileString("hra","camera_type",itoa(setup.camera_type,text,10),ini_file);
	//WritePrivateProfileString("hra","log_file",setup.log_file,ini_file);
	WritePrivateProfileString("hra","xres",itoa(setup.xres,text,10),ini_file);
	WritePrivateProfileString("hra","yres",itoa(setup.yres,text,10),ini_file);
	WritePrivateProfileString("hra","bpp",itoa(setup.bpp,text,10),ini_file);
	WritePrivateProfileString("hra","fullscreen",itoa(setup.fullscreen,text,10),ini_file);
	WritePrivateProfileString("hra","ditering",itoa(setup.ditering,text,10),ini_file);
	WritePrivateProfileString("hra","barva_pozadi",itoa(setup.barva_pozadi,text,10),ini_file);
	WritePrivateProfileString("hra","strip",itoa(setup.strip,text,10),ini_file);
	WritePrivateProfileString("hra","typ_mlhy",itoa(setup.typ_mlhy,text,10),ini_file);
	WritePrivateProfileString("hra","pohled_berusky",itoa(setup.pohled_berusky,text,10),ini_file);
	WritePrivateProfileString("hra","kvalita_castic",itoa(setup.kvalita_castic,text,10),ini_file);
	WritePrivateProfileString("hra","kvalita_casticv",itoa(setup.kvalita_casticv,text,10),ini_file);
	WritePrivateProfileString("hra","kvalita_casticp",itoa(setup.kvalita_casticp,text,10),ini_file);
	WritePrivateProfileString("hra","ovladani",itoa(setup.ovladani,text,10),ini_file);
	WritePrivateProfileString("hra","posouvat_kameru",itoa(setup.posouvat_kameru,text,10),ini_file);
	WritePrivateProfileString("hra","extra_light_vertex",itoa(setup.extra_light_vertex,text,10),ini_file);
	WritePrivateProfileString("hra","init_visibility",itoa(setup.init_visibility,text,10),ini_file);
	WritePrivateProfileString("hra","menu_vram_load",itoa(setup.menu_vram_load,text,10),ini_file);
	WritePrivateProfileString("hra","zrcado_aktivni",itoa(setup.zrcado_aktivni,text,10),ini_file);
	sprintf(text,"%f", setup.p_kamera_alfa);
	WritePrivateProfileString("hra","p_kamera_alfa",text,ini_file);
	sprintf(text,"%f", setup.p_kamera_radius);
	WritePrivateProfileString("hra","p_kamera_radius",text,ini_file);
	WritePrivateProfileString("hra","cursor",setup.cursor,ini_file);
	WritePrivateProfileString("hra","bump_mapping",itoa(setup.bump_mapping,text,10),ini_file);
	WritePrivateProfileString("hra","bump_mapping_typ",itoa(setup.bump_mapping_typ,text,10),ini_file);
	WritePrivateProfileString("hra","camera_intro",itoa(setup.camera_intro,text,10),ini_file);
	
	WritePrivateProfileString("hra","light_dyn",itoa(setup.light_dyn,text,10),ini_file);
	WritePrivateProfileString("hra","light_rychlost",itoa(setup.light_rychlost,text,10),ini_file);
	WritePrivateProfileString("hra","scene_materialy",itoa(setup.scene_materialy,text,10),ini_file);
	WritePrivateProfileString("hra","vertex_arrays",itoa(setup.vertex_arrays,text,10),ini_file);

	sprintf(text,"%f", setup.ovladani_rohy_rychlost);
	WritePrivateProfileString("hra","ovladani_rohy_rychlost",text,ini_file);
	WritePrivateProfileString("hra","start_zpruhlednovani",itoa(setup.start_zpruhlednovani,text,10),ini_file);
	WritePrivateProfileString("hra","start_zvyraznovani",itoa(setup.start_zvyraznovani,text,10),ini_file);
	WritePrivateProfileString("hra","ovladani_rohy",itoa(setup.ovladani_rohy,text,10),ini_file);
	WritePrivateProfileString("hra","ovladani_rohy_default",itoa(setup.ovladani_rohy_default,text,10),ini_file);
	WritePrivateProfileString("hra","ovladani_rohy_smer",itoa(setup.ovladani_rohy_smer,text,10),ini_file);
	WritePrivateProfileString("hra","ovladani_pr_posun",itoa(setup.ovladani_pr_posun,text,10),ini_file);
	WritePrivateProfileString("hra","animace_okoli",itoa(setup.animace_okoli,text,10),ini_file);
	WritePrivateProfileString("hra","general_3d",itoa(setup.general_3d,text,10),ini_file);

	WritePrivateProfileString("textury","text_detail",itoa(setup.text_detail,text,10),ini_file);
	WritePrivateProfileString("textury","text_detail_light",itoa(setup.text_detail_light,text,10),ini_file);
	WritePrivateProfileString("textury","text_kvalita",itoa(setup.text_kvalita,text,10),ini_file);
	WritePrivateProfileString("textury","text_komprese",itoa(setup.text_komprese,text,10),ini_file);
	WritePrivateProfileString("textury","text_mip_mapping",itoa(setup.text_mip_mapping,text,10),ini_file);
	WritePrivateProfileString("textury","text_mip_filtr",itoa(setup.text_mip_filtr,text,10),ini_file);
	WritePrivateProfileString("textury","text_bpp",itoa(setup.text_bpp,text,10),ini_file);
	WritePrivateProfileString("textury","text_bpp_plocha",itoa(setup.text_bpp_plocha,text,10),ini_file);
	WritePrivateProfileString("textury","text_perspective",itoa(setup.text_perspective,text,10),ini_file);
	WritePrivateProfileString("textury","text_hi_res",itoa(setup.hires_textury,text,10),ini_file);
	WritePrivateProfileString("textury","text_ans",itoa(setup.text_ans,text,10),ini_file);
	sprintf(text,"%f", setup.text_ans_level);
	WritePrivateProfileString("textury","text_ans_level",text,ini_file);
	sprintf(text,"%f", setup.text_ostrost);
	WritePrivateProfileString("textury","text_ostrost",text,ini_file);

	WritePrivateProfileString("textury","text_detail2",itoa(setup.text_detail2,text,10),ini_file);
	WritePrivateProfileString("textury","text_detail3",itoa(setup.text_detail3,text,10),ini_file);
	WritePrivateProfileString("textury","text_bump2",itoa(setup.text_bump2,text,10),ini_file);
	WritePrivateProfileString("textury","text_bump3",itoa(setup.text_bump3,text,10),ini_file);
	WritePrivateProfileString("textury","text_ans_stupen",itoa(setup.text_ans_stupen,text,10),ini_file);

	WritePrivateProfileString("soundengine","channels",itoa(setup.channels,text,10),ini_file);
	WritePrivateProfileString("soundengine","pre_load",itoa(setup.pre_load,text,10),ini_file);
	WritePrivateProfileString("soundengine","soundvolume",itoa(setup.soundvolume,text,10),ini_file);
	WritePrivateProfileString("soundengine","musicvolume",itoa(setup.musicvolume,text,10),ini_file);
	WritePrivateProfileString("soundengine","ambientvolume",itoa(setup.ambientvolume,text,10),ini_file);

	WritePrivateProfileString("controls","move_forward",itoa(setup.key[1],text,10),ini_file);
	WritePrivateProfileString("controls","turn_back",itoa(setup.key[2],text,10),ini_file);
	WritePrivateProfileString("controls","turn_left",itoa(setup.key[3],text,10),ini_file);
	WritePrivateProfileString("controls","turn_right",itoa(setup.key[4],text,10),ini_file);
	WritePrivateProfileString("controls","next_beatle",itoa(setup.key[5],text,10),ini_file);
	WritePrivateProfileString("controls","beatle1",itoa(setup.key[6],text,10),ini_file);
	WritePrivateProfileString("controls","beatle2",itoa(setup.key[7],text,10),ini_file);
	WritePrivateProfileString("controls","beatle3",itoa(setup.key[8],text,10),ini_file);
	WritePrivateProfileString("controls","beatle4",itoa(setup.key[9],text,10),ini_file);
	WritePrivateProfileString("controls","beatle5",itoa(setup.key[10],text,10),ini_file);
	
	WritePrivateProfileString("controls","camera_rotation",itoa(setup.key[12],text,10),ini_file);
	WritePrivateProfileString("controls","camera_move",itoa(setup.key[13],text,10),ini_file);
	WritePrivateProfileString("controls","camera_zoom_in",itoa(setup.key[14],text,10),ini_file);
	WritePrivateProfileString("controls","camera_zoom_out",itoa(setup.key[15],text,10),ini_file);
	WritePrivateProfileString("controls","camera_fast_turn_left",itoa(setup.key[16],text,10),ini_file);
	WritePrivateProfileString("controls","camera_fast_turn_right",itoa(setup.key[17],text,10),ini_file);
	WritePrivateProfileString("controls","camera_center",itoa(setup.key[18],text,10),ini_file);
	
	WritePrivateProfileString("controls","menu",itoa(setup.key[20],text,10),ini_file);
	WritePrivateProfileString("controls","restart",itoa(setup.key[21],text,10),ini_file);
	WritePrivateProfileString("controls","item_lock",itoa(setup.key[22],text,10),ini_file);
	//WritePrivateProfileString("controls","blow_detonation_pack",itoa(setup.key[23],text,10),ini_file);
	WritePrivateProfileString("controls","next_song",itoa(setup.key[23],text,10),ini_file);
	WritePrivateProfileString("controls","disable_inventory",itoa(setup.key[24],text,10),ini_file);
	WritePrivateProfileString("controls","disable_top_ledge",itoa(setup.key[25],text,10),ini_file);
	WritePrivateProfileString("controls","screenshot",itoa(setup.key[26],text,10),ini_file);
	WritePrivateProfileString("controls","visibility",itoa(setup.key[27],text,10),ini_file);
	WritePrivateProfileString("controls","highlight",itoa(setup.key[28],text,10),ini_file);
	WritePrivateProfileString("controls","hint",itoa(setup.key[29],text,10),ini_file);
	WritePrivateProfileString("controls","pause",itoa(setup.key[30],text,10),ini_file);
		
	/*WritePrivateProfileString("game","bitmap_dir",setup.bitmap_dir,ini_file);
	WritePrivateProfileString("game","data_dir",setup.data_dir,ini_file);
	WritePrivateProfileString("game","prvky_dir",setup.prvky_dir,ini_file);
	WritePrivateProfileString("game","level_dir",setup.level_dir,ini_file);
	WritePrivateProfileString("game","out_dir",setup.out_dir,ini_file);
	WritePrivateProfileString("game","texture_dir",setup.texture_dir,ini_file);
	WritePrivateProfileString("game","material_dir",setup.material_dir,ini_file);
	WritePrivateProfileString("game","game_level_dir",setup.game_level_dir,ini_file);
	WritePrivateProfileString("game","scene_dir",setup.scene_dir,ini_file);
	WritePrivateProfileString("game","game_data_dir",setup.game_data_dir,ini_file);
	WritePrivateProfileString("game","save_dir",setup.save_dir,ini_file);
	WritePrivateProfileString("soundengine","music_dir",setup.music_dir,ini_file);
	WritePrivateProfileString("soundengine","sound_dir",setup.sound_dir,ini_file);*/

	/*WritePrivateProfileString("editor","ditering",itoa(setup.e_ditering,text,10),ini_file);
	WritePrivateProfileString("editor","mazat_dummy",itoa(setup.mazat_dummy,text,10),ini_file);
	WritePrivateProfileString("editor","kolize_materialu",itoa(setup.kolize_materialu,text,10),ini_file);
	WritePrivateProfileString("editor","load_prvky",itoa(setup.load_prvky,text,10),ini_file);
	WritePrivateProfileString("editor","load_textury",itoa(setup.load_textury,text,10),ini_file);
	WritePrivateProfileString("editor","load_default_materialy",itoa(setup.load_default_materialy,text,10),ini_file);
	WritePrivateProfileString("editor","obalky_kontejner",itoa(setup.obalky_kontejner,text,10),ini_file);
	WritePrivateProfileString("editor","obalky_objekt",itoa(setup.obalky_objekt,text,10),ini_file);
	WritePrivateProfileString("editor","lista_prvku",itoa(setup.lista_prvku,text,10),ini_file);
	WritePrivateProfileString("editor","default_materialy",setup.default_materialy,ini_file);
	WritePrivateProfileString("editor","berusky_hra",setup.berusky_hra,ini_file);
	WritePrivateProfileString("editor","berusky_hra_dir",setup.berusky_hra_dir,ini_file);
	WritePrivateProfileString("editor","log_file",setup.e_log_file,ini_file);

	WritePrivateProfileString("debug","debug_vypisy",itoa(setup.debug_vypisy,text,10),ini_file);
	WritePrivateProfileString("debug","debug_light",itoa(setup.debug_light,text,10),ini_file);
	WritePrivateProfileString("debug","debug_fps",itoa(setup.debug_fps,text,10),ini_file);
	WritePrivateProfileString("debug","debug_draty",itoa(setup.debug_draty,text,10),ini_file);
	WritePrivateProfileString("debug","debug_kamery",itoa(setup.debug_kamery,text,10),ini_file);

	WritePrivateProfileString("game","system_material_1",setup.sada[0],ini_file);
	WritePrivateProfileString("game","system_material_2",setup.sada[1],ini_file);
	WritePrivateProfileString("game","system_material_3",setup.sada[2],ini_file);
	WritePrivateProfileString("game","system_material_4",setup.sada[3],ini_file);
	WritePrivateProfileString("game","system_material_5",setup.sada[4],ini_file);
	WritePrivateProfileString("game","system_material_6",setup.sada[5],ini_file);
	WritePrivateProfileString("game","system_material_7",setup.sada[6],ini_file);
	WritePrivateProfileString("game","system_material_8",setup.sada[7],ini_file);
	WritePrivateProfileString("game","system_material_9",setup.sada[8],ini_file);
	WritePrivateProfileString("game","system_material_10",setup.sada[9],ini_file);
	WritePrivateProfileString("game","system_material_11",setup.sada[10],ini_file);
	WritePrivateProfileString("game","system_material_12",setup.sada[11],ini_file);
	WritePrivateProfileString("game","system_material_13",setup.sada[12],ini_file);
	WritePrivateProfileString("game","system_material_14",setup.sada[13],ini_file);
	WritePrivateProfileString("game","system_material_15",setup.sada[14],ini_file);
	WritePrivateProfileString("game","system_material_16",setup.sada[15],ini_file);
	WritePrivateProfileString("game","system_material_17",setup.sada[16],ini_file);
	WritePrivateProfileString("game","system_material_18",setup.sada[17],ini_file);
	WritePrivateProfileString("game","system_material_19",setup.sada[18],ini_file);
	WritePrivateProfileString("game","system_material_20",setup.sada[19],ini_file);*/
}

void Load_ini(void)
{
	char text[256];

	setup.no_menu = GetPrivateProfileInt("hra","no_menu",0,ini_file);
	setup.no_sound = GetPrivateProfileInt("hra","no_sound",0,ini_file);
	setup.camera_type = GetPrivateProfileInt("hra","camera_type",0,ini_file);
	GetPrivateProfileString("hra","log_file","c:\\ber2log.txt", setup.log_file, 256, ini_file);

	setup.xres = GetPrivateProfileInt("hra","xres",0,ini_file);
	setup.yres = GetPrivateProfileInt("hra","yres",0,ini_file);
	setup.bpp = GetPrivateProfileInt("hra","bpp",0,ini_file);
	setup.fullscreen = GetPrivateProfileInt("hra","fullscreen",0,ini_file);
	setup.ditering = GetPrivateProfileInt("hra","ditering",0,ini_file);
	setup.barva_pozadi = GetPrivateProfileInt("hra","barva_pozadi",0,ini_file);
	setup.strip = GetPrivateProfileInt("hra","strip",0,ini_file);
	setup.typ_mlhy = GetPrivateProfileInt("hra","typ_mlhy",0,ini_file);
	setup.pohled_berusky = GetPrivateProfileInt("hra","pohled_berusky",0,ini_file);
	setup.kvalita_castic = GetPrivateProfileInt("hra","kvalita_castic",0,ini_file);
	setup.kvalita_casticv = GetPrivateProfileInt("hra","kvalita_casticv",0,ini_file);
	setup.kvalita_casticp = GetPrivateProfileInt("hra","kvalita_casticp",0,ini_file);
	setup.ovladani = GetPrivateProfileInt("hra","ovladani",0,ini_file);
	setup.posouvat_kameru = GetPrivateProfileInt("hra","posouvat_kameru",0,ini_file);
	setup.extra_light_vertex = GetPrivateProfileInt("hra","extra_light_vertex",0,ini_file);
	setup.init_visibility = GetPrivateProfileInt("hra","init_visibility",0,ini_file);
	setup.menu_vram_load = GetPrivateProfileInt("hra","menu_vram_load",0,ini_file);
	setup.zrcado_aktivni = GetPrivateProfileInt("hra","zrcado_aktivni",0,ini_file);
	GetPrivateProfileString("hra","p_kamera_alfa","c:\\", text, 256, ini_file);
	setup.p_kamera_alfa = (float)atof(text);
	GetPrivateProfileString("hra","p_kamera_radius","c:\\", text, 256, ini_file);
	setup.p_kamera_radius = (float)atof(text);
	GetPrivateProfileString("hra","cursor","cursor", setup.cursor, 32, ini_file);
	setup.bump_mapping = GetPrivateProfileInt("hra","bump_mapping",0,ini_file);
	setup.bump_mapping_typ = GetPrivateProfileInt("hra","bump_mapping_typ",0,ini_file);
	setup.light_dyn = GetPrivateProfileInt("hra","light_dyn",0,ini_file);
	setup.light_rychlost = GetPrivateProfileInt("hra","light_rychlost",0,ini_file);
	setup.scene_materialy = GetPrivateProfileInt("hra","scene_materialy",0,ini_file);
	setup.vertex_arrays = GetPrivateProfileInt("hra","vertex_arrays",0,ini_file);
	GetPrivateProfileString("hra","ovladani_rohy_rychlost","c:\\", text, 256, ini_file);
	setup.ovladani_rohy_rychlost = (float)atof(text);
	setup.start_zpruhlednovani = GetPrivateProfileInt("hra","start_zpruhlednovani",0,ini_file);
	setup.start_zvyraznovani = GetPrivateProfileInt("hra","start_zvyraznovani",0,ini_file);
	setup.ovladani_rohy = GetPrivateProfileInt("hra","ovladani_rohy",0,ini_file);
	setup.ovladani_rohy_default = GetPrivateProfileInt("hra","ovladani_rohy_default",0,ini_file);
	setup.ovladani_rohy_smer = GetPrivateProfileInt("hra","ovladani_rohy_smer",0,ini_file);
	setup.ovladani_pr_posun = GetPrivateProfileInt("hra","ovladani_pr_posun",0,ini_file);
	setup.animace_okoli = GetPrivateProfileInt("hra","animace_okoli",0,ini_file);
	setup.general_3d = GetPrivateProfileInt("hra","general_3d",0,ini_file);
	setup.camera_intro = GetPrivateProfileInt("hra","camera_intro",0,ini_file);

	setup.text_detail = GetPrivateProfileInt("textury","text_detail",0,ini_file);
	setup.text_detail_light = GetPrivateProfileInt("textury","text_detail_light",0,ini_file);
	setup.text_kvalita = GetPrivateProfileInt("textury","text_kvalita",0,ini_file);
	setup.text_komprese = GetPrivateProfileInt("textury","text_komprese",0,ini_file);
	setup.text_mip_mapping = GetPrivateProfileInt("textury","text_mip_mapping",0,ini_file);
	setup.text_mip_filtr = GetPrivateProfileInt("textury","text_mip_filtr",0,ini_file);
	setup.text_bpp = GetPrivateProfileInt("textury","text_bpp",0,ini_file);
	setup.text_bpp_plocha = GetPrivateProfileInt("textury","text_bpp_plocha",0,ini_file);
	setup.text_perspective = GetPrivateProfileInt("textury","text_perspective",0,ini_file);
	setup.hires_textury = GetPrivateProfileInt("textury","text_hi_res",0,ini_file);
	setup.text_ans = GetPrivateProfileInt("textury","text_ans",0,ini_file);
	setup.text_detail2 = GetPrivateProfileInt("textury","text_detail2",0,ini_file);
	setup.text_detail3 = GetPrivateProfileInt("textury","text_detail3",0,ini_file);
	setup.text_bump2 = GetPrivateProfileInt("textury","text_bump2",0,ini_file);
	setup.text_bump3 = GetPrivateProfileInt("textury","text_bump3",0,ini_file);
	setup.text_ans_stupen = GetPrivateProfileInt("textury","text_ans_stupen",0,ini_file);

	GetPrivateProfileString("textury","text_ans_level","c:\\", text, 256, ini_file);
	setup.text_ans_level = (float)atof(text);
	GetPrivateProfileString("textury","text_ostrost","c:\\", text, 256, ini_file);
	setup.text_ostrost = (float)atof(text);

	setup.channels = GetPrivateProfileInt("soundengine","channels",0,ini_file);
	setup.pre_load = GetPrivateProfileInt("soundengine","pre_load",0,ini_file);
	setup.soundvolume = GetPrivateProfileInt("soundengine","soundvolume",0,ini_file);
	setup.musicvolume = GetPrivateProfileInt("soundengine","musicvolume",0,ini_file);
	setup.ambientvolume = GetPrivateProfileInt("soundengine","ambientvolume",0,ini_file);

	setup.key[0] = -1;
	setup.key[1] = GetPrivateProfileInt("controls", "move_forward",0,ini_file);
	setup.key[2] = GetPrivateProfileInt("controls", "turn_back",0,ini_file);
	setup.key[3] = GetPrivateProfileInt("controls", "turn_left",0,ini_file);
	setup.key[4] = GetPrivateProfileInt("controls", "turn_right",0,ini_file);
	setup.key[5] = GetPrivateProfileInt("controls", "next_beatle",0,ini_file);
	setup.key[6] =	GetPrivateProfileInt("controls", "beatle1",0,ini_file);
	setup.key[7] =	GetPrivateProfileInt("controls", "beatle2",0,ini_file);
	setup.key[8] =	GetPrivateProfileInt("controls", "beatle3",0,ini_file);
	setup.key[9] =	GetPrivateProfileInt("controls", "beatle4",0,ini_file);
	setup.key[10] =	GetPrivateProfileInt("controls", "beatle5",0,ini_file);
	setup.key[11] = -1;
	setup.key[12] = GetPrivateProfileInt("controls", "camera_rotation",0,ini_file);
	setup.key[13] = GetPrivateProfileInt("controls", "camera_move",0,ini_file);
	setup.key[14] = GetPrivateProfileInt("controls", "camera_zoom_in",0,ini_file);
	setup.key[15] = GetPrivateProfileInt("controls", "camera_zoom_out",0,ini_file);
	setup.key[16] = GetPrivateProfileInt("controls", "camera_fast_turn_left",0, ini_file);
	setup.key[17] = GetPrivateProfileInt("controls", "camera_fast_turn_right",0,ini_file);
	setup.key[18] = GetPrivateProfileInt("controls", "camera_center",0,ini_file);
	setup.key[19] = -1;
	setup.key[20] = GetPrivateProfileInt("controls", "menu",0,ini_file);
	setup.key[21] = GetPrivateProfileInt("controls", "restart",0,ini_file);
	setup.key[22] = GetPrivateProfileInt("controls", "item_lock",0,ini_file);
	//setup.key[23] = GetPrivateProfileInt("controls", "blow_detonation_pack",0,ini_file);
	setup.key[23] = GetPrivateProfileInt("controls", "next_song",0,ini_file);
	setup.key[24] = GetPrivateProfileInt("controls", "disable_inventory",0,ini_file);
	setup.key[25] = GetPrivateProfileInt("controls", "disable_top_ledge",0,ini_file);
	setup.key[26] = GetPrivateProfileInt("controls", "screenshot",0,ini_file);
	setup.key[27] = GetPrivateProfileInt("controls", "visibility",0,ini_file);
	setup.key[28] = GetPrivateProfileInt("controls", "highlight",0,ini_file);
	setup.key[29] = GetPrivateProfileInt("controls", "hint",0,ini_file);
	setup.key[30] = GetPrivateProfileInt("controls", "pause",0,ini_file);
	setup.key[31] = -1;


	/*GetPrivateProfileString("game","bitmap_dir","c:\\", setup.bitmap_dir, 256, ini_file);
	GetPrivateProfileString("game","data_dir","c:\\", setup.data_dir, 256, ini_file);
	GetPrivateProfileString("game","prvky_dir","c:\\", setup.prvky_dir, 256, ini_file);
	GetPrivateProfileString("game","level_dir","c:\\", setup.level_dir, 256, ini_file);
	GetPrivateProfileString("game","out_dir","c:\\", setup.out_dir, 256, ini_file);
	GetPrivateProfileString("game","texture_dir","c:\\", setup.texture_dir, 256, ini_file);
	GetPrivateProfileString("game","material_dir","c:\\", setup.material_dir, 256, ini_file);
	GetPrivateProfileString("game","game_level_dir","c:\\", setup.game_level_dir, 256, ini_file);
	GetPrivateProfileString("game","scene_dir","c:\\", setup.scene_dir, 256, ini_file);
	GetPrivateProfileString("game","game_data_dir","c:\\", setup.game_data_dir, 256, ini_file);
	GetPrivateProfileString("game","save_dir","c:\\", setup.save_dir, 256, ini_file);
	GetPrivateProfileString("soundengine","music_dir","c:\\", setup.music_dir, 256, ini_file);
	GetPrivateProfileString("soundengine","sound_dir","c:\\", setup.sound_dir, 256, ini_file);*/

	setup.e_ditering = GetPrivateProfileInt("editor","ditering",0,ini_file);
	setup.mazat_dummy = GetPrivateProfileInt("editor","mazat_dummy",0,ini_file);
	setup.kolize_materialu = GetPrivateProfileInt("editor","kolize_materialu",0,ini_file);
	setup.load_prvky = GetPrivateProfileInt("editor","load_prvky",0,ini_file);
	setup.load_textury = GetPrivateProfileInt("editor","load_textury",0,ini_file);
	setup.load_default_materialy = GetPrivateProfileInt("editor","load_default_materialy",0,ini_file);
	setup.obalky_kontejner = GetPrivateProfileInt("editor","obalky_kontejner",0,ini_file);
	setup.obalky_objekt = GetPrivateProfileInt("editor","obalky_objekt",0,ini_file);
	setup.lista_prvku = GetPrivateProfileInt("editor","lista_prvku",0,ini_file);
	GetPrivateProfileString("editor","default_materialy","c:\\", setup.default_materialy, 256, ini_file);
	GetPrivateProfileString("editor","berusky_hra","c:\\", setup.berusky_hra, 256, ini_file);
	GetPrivateProfileString("editor","berusky_hra_dir","c:\\", setup.berusky_hra_dir, 256, ini_file);
	GetPrivateProfileString("editor","log_file","c:\\", setup.e_log_file, 256, ini_file);

	setup.debug_vypisy = GetPrivateProfileInt("debug","debug_vypisy",0,ini_file);
	setup.debug_light = GetPrivateProfileInt("debug","debug_light",0,ini_file);
	setup.debug_fps = GetPrivateProfileInt("debug","debug_fps",0,ini_file);
	setup.debug_draty = GetPrivateProfileInt("debug","debug_draty",0,ini_file);
	setup.debug_kamery = GetPrivateProfileInt("debug","debug_kamery",0,ini_file);

	GetPrivateProfileString("game","system_material_1","", setup.sada[0], 256, ini_file);
	GetPrivateProfileString("game","system_material_2","", setup.sada[1], 256, ini_file);
	GetPrivateProfileString("game","system_material_3","", setup.sada[2], 256, ini_file);
	GetPrivateProfileString("game","system_material_4","", setup.sada[3], 256, ini_file);
	GetPrivateProfileString("game","system_material_5","", setup.sada[4], 256, ini_file);
	GetPrivateProfileString("game","system_material_6","", setup.sada[5], 256, ini_file);
	GetPrivateProfileString("game","system_material_7","", setup.sada[6], 256, ini_file);
	GetPrivateProfileString("game","system_material_8","", setup.sada[7], 256, ini_file);
	GetPrivateProfileString("game","system_material_9","", setup.sada[8], 256, ini_file);
	GetPrivateProfileString("game","system_material_10","", setup.sada[9], 256, ini_file);
	GetPrivateProfileString("game","system_material_11","", setup.sada[10], 256, ini_file);
	GetPrivateProfileString("game","system_material_12","", setup.sada[11], 256, ini_file);
	GetPrivateProfileString("game","system_material_13","", setup.sada[12], 256, ini_file);
	GetPrivateProfileString("game","system_material_14","", setup.sada[13], 256, ini_file);
	GetPrivateProfileString("game","system_material_15","", setup.sada[14], 256, ini_file);
	GetPrivateProfileString("game","system_material_16","", setup.sada[15], 256, ini_file);
	GetPrivateProfileString("game","system_material_17","", setup.sada[16], 256, ini_file);
	GetPrivateProfileString("game","system_material_18","", setup.sada[17], 256, ini_file);
	GetPrivateProfileString("game","system_material_19","", setup.sada[18], 256, ini_file);
	GetPrivateProfileString("game","system_material_20","", setup.sada[19], 256, ini_file);
}

int GetResCombobox(ROZLISENI *p_roz, int roz_size)
{
	int i;

	for(i=0;i<roz_size;i++)
		if((p_roz[i].x == (unsigned int)setup.xres) && 
		   (p_roz[i].y == (unsigned int)setup.yres) && 
		   (p_roz[i].bpp == (unsigned int)setup.bpp))
			return i;

	kprintf(1, "-- SETTINGS -- nenasel jsem rozliseni %dx%dx%d v seznamu", setup.xres, setup.yres, setup.bpp);

	return 0;
}
