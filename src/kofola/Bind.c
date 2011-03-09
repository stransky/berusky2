//------------------------------------------------------------------------------------------------
// 0.0.1
//------------------------------------------------------------------------------------------------
#include "..\\komat\\mss_on.h"
#include <windows.h>
#include "..\Komat\Berusky_universal.h"
#include "bind.h"

//------------------------------------------------------------------------------------------------
// nabinduje klavesy
//------------------------------------------------------------------------------------------------
void Bind_Keys(CONTROL_KEYS *p_keys)
{
	p_keys->camera_fast_turn_left =		GetPrivateProfileInt("controls", "camera_fast_turn_left",255, ini_file);
	p_keys->camera_fast_turn_right =	GetPrivateProfileInt("controls", "camera_fast_turn_right",255,ini_file);
	p_keys->camera_move_back =			GetPrivateProfileInt("controls", "camera_move_back",255,ini_file);
	p_keys->camera_move_forward =		GetPrivateProfileInt("controls", "camera_move_forward",255,ini_file);
	p_keys->camera_move_left =			GetPrivateProfileInt("controls", "camera_move_left",255,ini_file);
	p_keys->camera_move_right =			GetPrivateProfileInt("controls", "camera_move_right",255,ini_file);
	p_keys->camera_turn_down =			GetPrivateProfileInt("controls", "camera_turn_down",255,ini_file);
	p_keys->camera_turn_left =			GetPrivateProfileInt("controls", "camera_turn_left",255,ini_file);
	p_keys->camera_turn_right =			GetPrivateProfileInt("controls", "camera_turn_right",255,ini_file);
	p_keys->camera_turn_up =			GetPrivateProfileInt("controls", "camera_turn_up",255,ini_file);
	p_keys->camera_zoom_in =			GetPrivateProfileInt("controls", "camera_zoom_in",255,ini_file);
	p_keys->camera_zoom_out =			GetPrivateProfileInt("controls", "camera_zoom_out",255,ini_file);
	p_keys->camera_center =				GetPrivateProfileInt("controls", "camera_center",255,ini_file);
	p_keys->inventory =					GetPrivateProfileInt("controls", "inventory",255,ini_file);
	p_keys->inventory_cancel =			GetPrivateProfileInt("controls", "inventory_cancel",255,ini_file);
	p_keys->inventory_left =			GetPrivateProfileInt("controls", "inventory_left",255,ini_file);
	p_keys->inventory_right =			GetPrivateProfileInt("controls", "inventory_right",255,ini_file);
	p_keys->inventory_select =			GetPrivateProfileInt("controls", "inventory_select",255,ini_file);
	p_keys->menu =						GetPrivateProfileInt("controls", "menu",255,ini_file);
	p_keys->move_forward =				GetPrivateProfileInt("controls", "move_forward",255,ini_file);
	p_keys->next_beatle =				GetPrivateProfileInt("controls", "next_beatle",255,ini_file);
	p_keys->turn_back =					GetPrivateProfileInt("controls", "turn_back",255,ini_file);
	p_keys->turn_left =					GetPrivateProfileInt("controls", "turn_left",255,ini_file);
	p_keys->turn_right =				GetPrivateProfileInt("controls", "turn_right",255,ini_file);
	p_keys->item_lock =					GetPrivateProfileInt("controls", "item_lock",255,ini_file);
	p_keys->blow_detonation_pack =		GetPrivateProfileInt("controls", "blow_detonation_pack",255,ini_file);
	p_keys->beatle1 =					GetPrivateProfileInt("controls", "beatle1",255,ini_file);
	p_keys->beatle2 =					GetPrivateProfileInt("controls", "beatle2",255,ini_file);
	p_keys->beatle3 =					GetPrivateProfileInt("controls", "beatle3",255,ini_file);
	p_keys->beatle4 =					GetPrivateProfileInt("controls", "beatle4",255,ini_file);
	p_keys->beatle5 =					GetPrivateProfileInt("controls", "beatle5",255,ini_file);
	p_keys->beatle6 =					GetPrivateProfileInt("controls", "beatle6",255,ini_file);
	p_keys->item1 =						GetPrivateProfileInt("controls", "item1",255,ini_file);
	p_keys->item2 =						GetPrivateProfileInt("controls", "item2",255,ini_file);
	p_keys->item3 =						GetPrivateProfileInt("controls", "item3",255,ini_file);
	p_keys->item4 =						GetPrivateProfileInt("controls", "item4",255,ini_file);
	p_keys->next_song =					GetPrivateProfileInt("controls", "next_song",255,ini_file);
	p_keys->disable_inventory =			GetPrivateProfileInt("controls", "disable_inventory",255,ini_file);
	p_keys->disable_top_ledge =			GetPrivateProfileInt("controls", "disable_top_ledge",255,ini_file);
	p_keys->screenshot =				GetPrivateProfileInt("controls", "screenshot",255,ini_file);
	p_keys->visibility =				GetPrivateProfileInt("controls", "visibility",255,ini_file);
	p_keys->hint =						GetPrivateProfileInt("controls", "hint",255,ini_file);
	p_keys->camera_move =				GetPrivateProfileInt("controls", "camera_move",255,ini_file);
	p_keys->camera_rotation =			GetPrivateProfileInt("controls", "camera_rotation",255,ini_file);
	p_keys->restart =					GetPrivateProfileInt("controls", "restart",255,ini_file);
	p_keys->highlight =					GetPrivateProfileInt("controls", "highlight",255,ini_file);
	p_keys->pause =						GetPrivateProfileInt("controls", "pause",255,ini_file);
}
