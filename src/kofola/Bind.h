#ifndef __BIND_
#define __BIND_

typedef struct
{
  int move_forward;
  int turn_back;
  int turn_left;
  int turn_right;
  int next_beatle;
  int inventory;
  int inventory_select;
  int inventory_left;
  int inventory_right;
  int inventory_cancel;
  int camera_move_left;
  int camera_move_right;
  int camera_move_forward;
  int camera_move_back;
  int camera_zoom_in;
  int camera_zoom_out;
  int camera_turn_left;
  int camera_turn_right;
  int camera_turn_up;
  int camera_turn_down;
  int camera_fast_turn_left;
  int camera_fast_turn_right;
  int camera_center;
  int menu;
  int item_lock;
  int blow_detonation_pack;
  int beatle1;
  int beatle2;
  int beatle3;
  int beatle4;
  int beatle5;
  int beatle6;
  int item1;
  int item2;
  int item3;
  int item4;
  int next_song;
  int disable_inventory;
  int disable_top_ledge;
  int screenshot;
  int visibility;
  int hint;
  int camera_rotation;
  int camera_move;
  int restart;
  int highlight;
  int pause;
} CONTROL_KEYS;

void Bind_Keys(CONTROL_KEYS * p_keys);

#endif
