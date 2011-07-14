#ifndef __2D_GRAPHIC_
#define __2D_GRAPHIC_

typedef struct
{
  //TODO
  // HANDLE       hRegion;
  char bSave;
} VIDEORAM;

typedef struct
{
  // TODO
  // BITMAP       bitmap;
  // HBITMAP old_hbitmap;
  // HBITMAP hbitmap;
  // HDC          bitmapDC;
  char bLoad;
} BITMAPSTRUCT;

typedef struct
{
  int bm_count;
  char bm_dir[256];
  BITMAPSTRUCT *bitmap;

  // TODO
  // HWND                 hWnd;
  // HDC                          hDC;
  float ProgressStatus;
  int ProgressBmp;
  float ProgressPlus;
} _2D_DATA;

typedef struct
{
  RECT rect;
  int iLayer;
  char bUsed;
} DRAW_RECT;

#define DRAW_RECT_NUM	40

typedef struct
{
  DRAW_RECT rect[DRAW_RECT_NUM];
  int rlast;
} RECT_LINE;

        // init 2d
int _2d_Init(void);

        // 2d release
void _2d_Release(void);

        // load bitmap
int _2d_Load_Bitmap(char *p_File_Name);
int _2d_APAK_Load_Bitmap(char *p_File_Name, APAK_HANDLE * pHandle);

        // release bitmap
int _2d_Release_Bitmap(int pointer);

        // load list of bitmaps
int _2d_Load_List(char *p_File_Name);
int _2d_APAK_Load_List(char *p_File_Name);
void _2d_Draw_Load_Screen(void);

void _2d_Init_Load_Progress_Bar(int iNumOfItems);
void _2d_Add_Progress(float fPercent);
void _2d_Draw_Progress(int x, int y);
void _2d_Release_Progress(void);
void _2d_Blackness(void);

        //int _2d_Find_Free_Surface(void);

void _2d_Add_RectItem(RECT_LINE * p_rl, RECT rect, int iLayer);
void _2d_Add_RectItem_IfNPresent(RECT_LINE * p_rl, RECT rect, int iLayer);
int _2d_Is_InRectLine(RECT * rline, RECT * p_r, int size);

void _2d_Fill_Rect(RECT rect, COLORREF color);
void _2d_Clear_RectLine(RECT_LINE * p_rl);

/*
	bool BitBltU(
  //HDC hdcDest, // handle to destination DC
  int nXDest,  // x-coord of destination upper-left corner
  int nYDest,  // y-coord of destination upper-left corner
  int nWidth,  // width of destination rectangle
  int nHeight, // height of destination rectangle
  //HDC hdcSrc,  // handle to source DC
  int nXSrc,   // x-coordinate of source upper-left corner
  int nYSrc,   // y-coordinate of source upper-left corner
  DWORD dwRop  // raster operation code
);
*/

        //typedef BOOL (WINAPI* LPTRANSPARENTBLT)(HDC, int, int, int, int, HDC, int, int, int, int, UINT); 

int LoadTransparentBlt(void);
void UnloadTransparentBlt(void);

#endif
