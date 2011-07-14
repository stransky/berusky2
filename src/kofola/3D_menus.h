#ifndef __3D_MENUS_
#define __3D_MENUS_

typedef struct
{
  float x, y;
  int iHint;
  char bHint;
  DWORD dwTCounter;
} HINT_STATE;

typedef struct
{
  float x;
  float y;
  EDIT_TEXT text;
  DWORD dwTCounter;
  float Speed;
  int iState;
  char bKUK;
} KUK_STATE;

        // draws all 3d menus in game to the surface
void _3d_Draw_Menus(char *bCursor, int *Cursor_Time_Out,
  float *_3d_Scale_Factor, LEVELINFO * p_Level, int act_item, int *p_Return,
  int demo);

int _3d_Check_Beatle_Select(float *_3d_Scale_Factor);

int _3d_Is_There_Beetle(int SubClass, LEVELINFO * p_Level);

void _3d_Load_Animations(float *fScaleFactor);
int _3d_Start_Animation(int iIndex, float *vfLocation, int *pFlag);

void _3d_Run_Cibule(void);

void _3d_Release_Display_Lists(void);
void _3d_Create_Top_Ledge_Display_List(float *_3d_Scale_Factor);
void _3d_Create_Inventory_Display_List(float *_3d_Scale_Factor);

void _3d_Cancel_Animations(LEVELINFO * p_Level);

void _3d_Draw_MenusB(LEVELINFO * p_Level, int act_item, char *bCursor,
  float *_3d_Scale_Factor, int bTutor, int bTText, int ty);

void _3d_Display_Hint(LEVELINFO * p_Level, float *_3d_Scale_Factor);
void _3d_Display_Hint_Inventory(LEVELINFO * p_Level, ITEMDESC * pItem,
  int idx, int x, int y, float *_3d_Scale_Factor);

void _3d_Nahraj_Kuk(void);
void _3d_Release_Kuk(void);

void _3d_Load_Indikace(void);

int _3d_Draw_MessageBox(int iMessage, float *_3d_Scale_Factor);

void _3d_Cancel_Menu(LEVELINFO * p_Level);

void _3d_Stop_Menu_Select(void);

#endif
