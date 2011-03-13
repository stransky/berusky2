#ifndef __CONTROLS_
#define __CONTROLS_

#define BUTTON_LARGE 0
#define BUTTON_SMALL 1

typedef struct _HDC_INFO2
{
	int x;
	int y;
	HDC hdc;
	//HBITMAP	hbitmap;
	//HBITMAP	hbitmapold;
} HDC_INFO2;

typedef struct _HDC_EDIT_CONTROL2
{
	int	hdcEdit;
} HDC_EDIT_CONTROL2;

typedef struct _HDC_COMBO_CONTROL2
{
	int	hdcCombo;
	int	hdcComboEnd;
	int	hdcComboMid;
	int	hdcComboUp;
	int	hdcComboDown;
	int	hdcComboDrop;
	int hdcComboMover;
	int hdcComboMoverb;
} HDC_COMBO_CONTROL2;

typedef struct _HDC_BUTTON_CONTROL2
{
	int hdcButtonL;
	int hdcButtonS;
} HDC_BUTTON_CONTROL2;

typedef struct _HDC_FRAME_CONTROL2
{
	int hdcFrame;
} HDC_FRAME_CONTROL2;

typedef struct _HDC_PROGRES_CONTROL2
{
	int hdcLine;
	int hdcMover;
} HDC_PROGRES_CONTROL2;

typedef struct _HDC_CHECKBOX_CONTROL2
{
	int hdcCheck;
	int	hdcGray;
} HDC_CHECKBOX_CONTROL2;

typedef struct _HDC_LIST_CONTROL2
{
	int	hdcListDown;
	int	hdcListUp;
	int	hdcListMover;
	int	hdcListTop;
	int	hdcListBottom;
	int hdcListLeft;
	int hdcListRight;
	int	hdcListTL;
	int hdcListTR;
	int hdcListBL;
	int hdcListBR;
} HDC_LIST_CONTROL2;

typedef struct _COMBO_ITEM2
{	
	char		text[256];
	int			Sel;
	int			Norm;
	float		fValue;
} COMBO_ITEM2;

typedef struct _COMBO_CONTROL2
{
	int			comboID;
	int			x;
	int			y;
	int			Width;
	int			WidthR;
	int			Hight;
	int			ListHight;
	int			ListMaxHight;
	int			ListMaxHightR;
	int			Selected;
	int			CSelected;
	int			SSelected;
	int			OSelected;
	COMBO_ITEM2	*pItem;
	int			CounfOfItems;
	int			CounfOfItemsL;
	RECT		coEditRect;
	RECT		coDownRect;
	RECT		coListRect;
	RECT		coLUp;
	RECT		coLDown;
	RECT		coLMov;
	RECT		coLMovA;
	char		bEdit;
	char		bList;
	char		bOpen;
	int			*pBDC;
	int			xmstart;
	int			xm;
	int			xmp;
	char		bIn;
} COMBO_CONTROL2;

typedef struct _COMBO_DROP_CONTROL2
{
	int			dropID;
	RECT		coLUp;
	RECT		coLDown;
	int			x;
	int			y;
	int			Selected;
	COMBO_ITEM2	*pItem;
	int			CounfOfItems;
} COMBO_DROP_CONTROL2;

typedef struct _BUTTON_CONTROL2
{
	int			x;
	int			y;
	int			type;
	RECT		Rect;
	int			dc;
	int			ButtonID;
	char		bActivated;
} BUTTON_CONTROL2;

typedef struct _CHECKBOX_CONTROL2
{
	int			x;
	int			y;
	RECT		Rect;
	RECT		RectFull;
	int			iDC;
	int			checkID;
	char		bChecked;
	char		bChange;
} CHECKBOX_CONTROL2;

typedef struct _PROGRES_CONTROL2
{
	RECT		rectMover;
	RECT		rectProgres;
	int			progID;
	int			min;
	int			max;
	int			pos;
	int			cor;
	int			x;
	int			y;
	int			bDC;
	char		bIn;
	char		bChange;
	char		bExclusive;
	RECT		RectFull;
	int			iDC;
} PROGRES_CONTROL2;

typedef struct _LIST_VIEW_CONTROL2
{
	int			listID;
	int			dx;
	int			listnum;
	int			x;
	int			y;
	int			mpmin;
	int			mpmax;
	int			mpos;
	RECT		rectList;
	RECT		rectUp;
	RECT		rectDown;
	RECT		rectMoverA;
	RECT		rectMover;
	int			bDCn;
	int			bDCs;
	int			bDCm;
	char		bIn;
	char		bInE;
	int			Selected;
	int			Pushc;
	DWORD		pTime;
	int			cSelected;
	char		bClck;
	char		bDblClck;
	int			*piValue;
	WCHAR		**pwText;
	char		**cText;
	int			cClckSel;
	int			bSelection;
} LIST_VIEW_CONTROL2;

typedef struct _LIST_VIEW2_CONTROL2
{
	int			listID;
	int			dx;
	int			x;
	int			y;
	int			mpmin;
	int			mpmax;
	int			mpos;
	RECT		rectList;
	RECT		rectUp;
	RECT		rectDown;
	RECT		rectMoverA;
	RECT		rectMover;
	int			bDC;
	int			bDCm;
	int			iViewHeight;
	char		bIn;
	char		bInE;
} LIST_VIEW2_CONTROL2;

typedef struct _CONTROL_EDIT2
{
	int			bDC;
	//int			bDCb;
	int			x;
	int			y;
	WCHAR		wtext[64];
	int			lchar;
	int			editID;
	RECT		rect;
	char		bActive;
	char		bcActive;
	DWORD		pTime;
	int			tx;
} CONTROL_EDIT2;

typedef struct _CONTROL_LIST_ITEM2
{
	COMBO_CONTROL2		*p_combo;
	BUTTON_CONTROL2		*p_button;
	CHECKBOX_CONTROL2	*p_check;
	PROGRES_CONTROL2	*p_prog;
	LIST_VIEW_CONTROL2	*p_list;
	LIST_VIEW2_CONTROL2	*p_list_v;
	CONTROL_EDIT2		*p_edit;
	COMBO_DROP_CONTROL2	*p_combod;
	int					iTab;
	char				bActive;
	char				bFocus;
	char				bDisabled;
} CONTROL_LIST_ITEM2;

	int co2_Load_Graphic(int combo_var);
	int co2_Release_Graphic(void);

	COMBO_CONTROL2 *co2_Create_Combo(int hdc, int x, int y, int maxlisthight, int id);
	int co2_Combo_Add_String(COMBO_CONTROL2 *p_co, char *text);
	int co2_Combo_Add_StringWC(COMBO_CONTROL2 *p_co, char *text);
	int co2_Combo_Set_Params(COMBO_CONTROL2 *p_co, int itemnum);
	int co2_Combo_Set_Sel(int hdc, COMBO_CONTROL2 *p_co, int i);
	int co2_Combo_Get_Sel_Not_Opend(CONTROL_LIST_ITEM2 *p_list, int lsize, int id);
	int co2_Combo_Get_Sel(CONTROL_LIST_ITEM2 *p_list, int lsize, int id);
	void co2_Release_Combo(COMBO_CONTROL2 *p_co);

	BUTTON_CONTROL2 *co2_Create_Button(int hdc, int x, int y, int type, char *text, int isection, int buttonID);
	int co2_Is_Button_Activated(CONTROL_LIST_ITEM2 *p_list, int lsize, int id);
	void co2_Release_Button(BUTTON_CONTROL2 *p_bu);
	
	CHECKBOX_CONTROL2 *co2_Create_CheckBox(int hdc, int x, int y, char *text, int isection, int checkID);
	int co2_Check_Get_State(CONTROL_LIST_ITEM2 *p_list, int lsize, int id);
	int co2_Check_Set_State(CHECKBOX_CONTROL2 *p_ch, int hdc, int state, char bDraw);
	void co2_Release_CheckBox(CHECKBOX_CONTROL2 *p_ch);

	PROGRES_CONTROL2 *co2_Create_Progres(int hdc, int x, int y, int min, int max, int progID);
	int co2_Progres_Set(PROGRES_CONTROL2 *p_pr, int hdc, int i);
	int co2_Progres_Get(CONTROL_LIST_ITEM2 *p_list, int lsize, int id);
	int co2_Progres_Changed(CONTROL_LIST_ITEM2 *p_list, int lsize, int id);
	void co2_Release_Progres(PROGRES_CONTROL2 *p_pr);

	int co2_Set_Text(int hdc, int x, int y, char *text, int isection);
	int co2_Set_Text_Center(int hdc, char *text, int isection, RECT r);
	int co2_Set_Text_Formated_Rect(int hdc, int x, int y, char *text, int isection, RECT *pR);
	int co2_Set_Text_Right(int hdc, char *text, int isection, int x, int y);

	LIST_VIEW_CONTROL2 *co2_Create_List(int hdc, int x, int y, int width, int hight, int id, int numofitems, char bSelection);	
	int co2_List_Add_String(LIST_VIEW_CONTROL2 *p_li, int index, int x, char *text, int iValue, char bSelected);
	int co2_List_Add_StringWC(LIST_VIEW_CONTROL2 *p_li, int index, int x, char *text, int iValue, char bSelected);
	int co2_List_Add_StringWC2(LIST_VIEW_CONTROL2 *p_li, int index, int x, WCHAR *wc, char *text, int iValue, char bSelected);
	int co2_List_Redraw(int hdc, LIST_VIEW_CONTROL2 *p_li, int y);
	int co2_List_Get_Dbclck(CONTROL_LIST_ITEM2 *p_list, int lsize, int id, LIST_VIEW_CONTROL2 **p_li);
	int co2_List_Get_Clck(CONTROL_LIST_ITEM2 *p_list, int lsize, int id, LIST_VIEW_CONTROL2 **p_li);
	int co2_List_Delete_Item(LIST_VIEW_CONTROL2 *p_li, char **cValue);

	int co2_List_Get_Value(CONTROL_LIST_ITEM2 *p_list, int lsize, int id, int index);
	void co2_Release_List(LIST_VIEW_CONTROL2 *p_li);

	CONTROL_EDIT2 *co2_Create_Edit(int hdc, int x, int y, int editID);
	WCHAR *co2_Edit_Get_Text(CONTROL_EDIT2 *p_ed);
	void co2_Edit_Set_Text(CONTROL_EDIT2 *p_ed, WCHAR *cText, char bRedraw, int hdc, int xcor, int ycor);

	COMBO_DROP_CONTROL2 *co2_Create_Combo_Drop(int hdc, int x, int y, int id);
	int co2_Combo_Drop_Add_String(COMBO_DROP_CONTROL2 *p_co, char *text, float fValue);
	int co2_Combo_Drop_Set_Sel(int hdc, COMBO_DROP_CONTROL2 *p_co, int i);
	int co2_Combo_Drop_Set_String(int hdc, COMBO_DROP_CONTROL2 *p_co, int xcor, int ycor);
	int co2_Combo_Drop_Get_Sel(CONTROL_LIST_ITEM2 *p_list, int lsize, int id, float *pf);

	void co2_Handle_Controls(CONTROL_LIST_ITEM2 *p_list, int lsize, int x, int y, int hdc, int xcor, int ycor);
	void co2_Handle_Release(CONTROL_LIST_ITEM2 *p_list, int lsize);

	void co2_Frame_Draw(HDC hdc, int x, int y, int xr, int yr);

	HDC co2_CreateDC(HDC hdc, int x, int y, HDC_INFO2 *pdcinfo);
	int	co2_Rect_Hit(RECT rect, int x, int y);

	LIST_VIEW2_CONTROL2 *co2_Create_List_View(int hdc, int x, int y, int width, int hight, int id, int iViewHeight);
	void co2_Release_List_View(LIST_VIEW2_CONTROL2 *p_li);
	int co2_Get_List_View_DC(LIST_VIEW2_CONTROL2 *p_li);
	int co2_List_View_Redraw(int hdc, LIST_VIEW2_CONTROL2 *p_li, int y);
	void co2_Set_List_View_List_Pos(LIST_VIEW2_CONTROL2 *p_li, int y, int hdc, int xcor, int  ycor);

	void co2_Release_Combo_Drop(COMBO_DROP_CONTROL2 *p_co);

	//int co2_Set_TextWC(int hdc, int x, int y, char *text, int isection);

	void co2_Check_Disable(int hdc, int xcor, int ycor, CONTROL_LIST_ITEM2 *p_list, int lsize, int id);
	void co2_Check_Enable(int hdc, int xcor, int ycor, CONTROL_LIST_ITEM2 *p_list, int lsize, int id);
	int co2_Check_Get_State_Change(CONTROL_LIST_ITEM2 *p_list, int lsize, int id);

	void co2_Draw_Disable(int hdc, int xcor, int ycor, int x, int y, int dx, int dy);

	void co2_Progres_Disable(int hdc, int xcor, int ycor, CONTROL_LIST_ITEM2 *p_list, int lsize, int id, char bSave, int shdc);
	void co2_Progres_Enable(int hdc, int xcor, int ycor, CONTROL_LIST_ITEM2 *p_list, int lsize, int id);

	void co_delete(char *cfile);

#endif
