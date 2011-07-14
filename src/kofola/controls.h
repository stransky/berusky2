#ifndef __CONTROLS_
#define __CONTROLS_

#define BUTTON_LARGE 0
#define BUTTON_SMALL 1
typedef struct _HDC_INFO
{
  int x;
  int y;
  //HDC hdc;
  //HBITMAP       hbitmap;
  //HBITMAP       hbitmapold;
} HDC_INFO;

typedef struct _HDC_EDIT_CONTROL
{
  int hdcEdit;
} HDC_EDIT_CONTROL;

typedef struct _HDC_COMBO_CONTROL
{
  int hdcCombo;
  int hdcComboEnd;
  int hdcComboMid;
  int hdcComboUp;
  int hdcComboDown;
  int hdcComboDrop;
  int hdcComboMover;
  int hdcComboMoverb;
} HDC_COMBO_CONTROL;

typedef struct _HDC_BUTTON_CONTROL
{
  int hdcButtonL;
  int hdcButtonS;
} HDC_BUTTON_CONTROL;

typedef struct _HDC_FRAME_CONTROL
{
  int hdcFrame;
} HDC_FRAME_CONTROL;

typedef struct _HDC_PROGRES_CONTROL
{
  int hdcLine;
  int hdcMover;
} HDC_PROGRES_CONTROL;

typedef struct _HDC_CHECKBOX_CONTROL
{
  int hdcCheck;
  int hdcGray;
} HDC_CHECKBOX_CONTROL;

typedef struct _HDC_LIST_CONTROL
{
  int hdcListDown;
  int hdcListUp;
  int hdcListMover;
  int hdcListTop;
  int hdcListBottom;
  int hdcListLeft;
  int hdcListRight;
  int hdcListTL;
  int hdcListTR;
  int hdcListBL;
  int hdcListBR;
} HDC_LIST_CONTROL;

typedef struct _COMBO_ITEM
{
  char text[256];
  int Sel;
  int Norm;
  float fValue;
} COMBO_ITEM;

typedef struct _COMBO_CONTROL
{
  int comboID;
  int x;
  int y;
  int Width;
  int WidthR;
  int Hight;
  int ListHight;
  int ListMaxHight;
  int ListMaxHightR;
  int Selected;
  int CSelected;
  int SSelected;
  int OSelected;
  COMBO_ITEM *pItem;
  int CounfOfItems;
  int CounfOfItemsL;
  RECT coEditRect;
  RECT coDownRect;
  RECT coListRect;
  RECT coLUp;
  RECT coLDown;
  RECT coLMov;
  RECT coLMovA;
  char bEdit;
  char bList;
  char bOpen;
  int *pBDC;
  int xmstart;
  int xm;
  int xmp;
  char bIn;
} COMBO_CONTROL;

typedef struct _COMBO_DROP_CONTROL
{
  int dropID;
  RECT coLUp;
  RECT coLDown;
  int x;
  int y;
  int Selected;
  COMBO_ITEM *pItem;
  int CounfOfItems;
} COMBO_DROP_CONTROL;

typedef struct _BUTTON_CONTROL
{
  int x;
  int y;
  int type;
  RECT Rect;
  int dc;
  int ButtonID;
  char bActivated;
} BUTTON_CONTROL;

typedef struct _CHECKBOX_CONTROL
{
  int x;
  int y;
  RECT Rect;
  RECT RectFull;
  int iDC;
  int checkID;
  char bChecked;
  char bChange;
} CHECKBOX_CONTROL;

typedef struct _PROGRES_CONTROL
{
  RECT rectMover;
  RECT rectProgres;
  int progID;
  int min;
  int max;
  int pos;
  int cor;
  int x;
  int y;
  int bDC;
  char bIn;
  char bChange;
  char bExclusive;
  RECT RectFull;
  int iDC;
} PROGRES_CONTROL;

typedef struct _LIST_VIEW_ITEM
{
  int iValue;
  char *cValue;
  WCHAR *wcValue;
} LIST_VIEW_ITEM;

typedef struct _LIST_VIEW_CONTROL
{
  int listID;
  int dx;
  int listnum;
  int x;
  int y;
  int mpmin;
  int mpmax;
  int mpos;
  RECT rectList;
  RECT rectUp;
  RECT rectDown;
  RECT rectMoverA;
  RECT rectMover;
  int bDCn;
  int bDCs;
  int bDCm;
  char bIn;
  char bInE;
  int Selected;
  int Pushc;
  DWORD pTime;
  int cSelected;
  int cClckSel;
  char bClck;
  char bDblClck;
  int bSelection;
  LIST_VIEW_ITEM *piValue;
} LIST_VIEW_CONTROL;

typedef struct _CONTROL_EDIT
{
  int bDC;
  //int                   bDCb;
  int x;
  int y;
  WCHAR wtext[128];
  int lchar;
  int editID;
  RECT rect;
  char bActive;
  char bcActive;
  DWORD pTime;
  int tx;
} CONTROL_EDIT;

typedef struct _CONTROL_LIST_ITEM
{
  COMBO_CONTROL *p_combo;
  BUTTON_CONTROL *p_button;
  CHECKBOX_CONTROL *p_check;
  PROGRES_CONTROL *p_prog;
  LIST_VIEW_CONTROL *p_list;
  CONTROL_EDIT *p_edit;
  COMBO_DROP_CONTROL *p_combod;
  int iTab;
  char bActive;
  char bFocus;
  char bDisabled;
} CONTROL_LIST_ITEM;

int co_Load_Graphic(int combo_var);
int co_Release_Graphic(void);

COMBO_CONTROL *co_Create_Combo(int hdc, int x, int y, int maxlisthight,
  int id);
int co_Combo_Add_String(COMBO_CONTROL * p_co, char *text);
int co_Combo_Add_StringWC(COMBO_CONTROL * p_co, char *text);
int co_Combo_Add_StringWC2(COMBO_CONTROL * p_co, WCHAR * wc, char *text);
int co_Combo_Set_Params(COMBO_CONTROL * p_co, int itemnum);
int co_Combo_Set_Sel(int hdc, COMBO_CONTROL * p_co, int i);
int co_Combo_Get_Sel(CONTROL_LIST_ITEM * p_list, int lsize, int id);
void co_Release_Combo(COMBO_CONTROL * p_co);

BUTTON_CONTROL *co_Create_Button(int hdc, int x, int y, int type, char *text,
  int isection, int buttonID);
int co_Is_Button_Activated(CONTROL_LIST_ITEM * p_list, int lsize, int id);
void co_Release_Button(BUTTON_CONTROL * p_bu);

CHECKBOX_CONTROL *co_Create_CheckBox(int hdc, int x, int y, char *text,
  int isection, int checkID);
int co_Check_Get_State(CONTROL_LIST_ITEM * p_list, int lsize, int id);
int co_Check_Set_State(CHECKBOX_CONTROL * p_ch, int hdc, int state,
  char bDraw);
void co_Release_CheckBox(CHECKBOX_CONTROL * p_ch);

PROGRES_CONTROL *co_Create_Progres(int hdc, int x, int y, int min, int max,
  int progID);
int co_Progres_Set(PROGRES_CONTROL * p_pr, int hdc, int i);
int co_Progres_Get(CONTROL_LIST_ITEM * p_list, int lsize, int id);
int co_Progres_Changed(CONTROL_LIST_ITEM * p_list, int lsize, int id);
void co_Release_Progres(PROGRES_CONTROL * p_pr);

int co_Set_Text(int hdc, int x, int y, char *text, int isection);
int co_Set_Text_Center(int hdc, char *text, int isection, RECT r);
int co_Set_Text_Right(int hdc, char *text, int isection, int x, int y);
int co_Set_Text_RightWC(int hdc, char *text, int isection, int x, int y);

LIST_VIEW_CONTROL *co_Create_List(int hdc, int x, int y, int width, int hight,
  int id, int numofitems, int bSelection);
int co_List_Add_String(LIST_VIEW_CONTROL * p_li, int index, int x, char *text,
  int iValue, char bSelected);
int co_List_Add_StringWC(LIST_VIEW_CONTROL * p_li, int index, int x,
  char *text, int iValue, char bSelected);
int co_List_Add_StringWC2(LIST_VIEW_CONTROL * p_li, int index, int x,
  WCHAR * wc, char *text, int iValue, char bSelected);
int co_List_Redraw(int hdc, LIST_VIEW_CONTROL * p_li, int y);
int co_List_Get_Dbclck(CONTROL_LIST_ITEM * p_list, int lsize, int id,
  LIST_VIEW_CONTROL ** p_li);
int co_List_Get_Clck(CONTROL_LIST_ITEM * p_list, int lsize, int id,
  LIST_VIEW_CONTROL ** p_li);
int co_List_Get_Value(CONTROL_LIST_ITEM * p_list, int lsize, int id,
  int index);
int co_List_Delete_Item(LIST_VIEW_CONTROL * p_li, char **cValue);
void co_Release_List(LIST_VIEW_CONTROL * p_li);

CONTROL_EDIT *co_Create_Edit(int hdc, int x, int y, int editID);
WCHAR *co_Edit_Get_Text(CONTROL_EDIT * p_ed);

COMBO_DROP_CONTROL *co_Create_Combo_Drop(int hdc, int x, int y, int id);
int co_Combo_Drop_Add_String(COMBO_DROP_CONTROL * p_co, char *text,
  float fValue);
int co_Combo_Drop_Set_Sel(int hdc, COMBO_DROP_CONTROL * p_co, int i);
int co_Combo_Drop_Set_String(int hdc, COMBO_DROP_CONTROL * p_co, int xcor,
  int ycor);
int co_Combo_Drop_Add_StringWC(COMBO_DROP_CONTROL * p_co, char *text,
  float fValue);
int co_Combo_Drop_Get_Sel(CONTROL_LIST_ITEM * p_list, int lsize, int id,
  float *pf);

void co_Handle_Controls(CONTROL_LIST_ITEM * p_list, int lsize, int x, int y,
  int hdc, int xcor, int ycor);
void co_Handle_Release(CONTROL_LIST_ITEM * p_list, int lsize);

        //void co_Frame_Draw(HDC hdc, int x, int y, int xr, int yr);

        //HDC co_CreateDC(HDC hdc, int x, int y, HDC_INFO *pdcinfo);
int co_Rect_Hit(RECT rect, int x, int y);

int co_Combo_Get_Sel_Not_Opend(CONTROL_LIST_ITEM * p_list, int lsize, int id);
void co_Cleare_List(LIST_VIEW_CONTROL * p_li, int numofitems, int hdc,
  int xcor, int ycor);

void co_Release_Combo_Drop(COMBO_DROP_CONTROL * p_co);

void co_Check_Disable(int hdc, int xcor, int ycor, CONTROL_LIST_ITEM * p_list,
  int lsize, int id);
void co_Check_Enable(int hdc, int xcor, int ycor, CONTROL_LIST_ITEM * p_list,
  int lsize, int id);
int co_Check_Get_State_Change(CONTROL_LIST_ITEM * p_list, int lsize, int id);

void co_Progres_Disable(int hdc, int xcor, int ycor,
  CONTROL_LIST_ITEM * p_list, int lsize, int id, char bSave, int shdc);
void co_Progres_Enable(int hdc, int xcor, int ycor,
  CONTROL_LIST_ITEM * p_list, int lsize, int id);

#endif
