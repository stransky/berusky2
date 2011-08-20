#include "Apak.h"

#define  MAX_JMENO       50

#define  TRUE   (1==1)
#define  FALSE  (1!=1)

typedef unsigned char byte;
typedef unsigned char BYTE;
typedef unsigned short word;
typedef unsigned short WORD;
typedef unsigned int dword;
typedef unsigned int COLORREF;
typedef long LONG;
typedef dword DWORD;
typedef float FLOAT;
typedef wchar_t WCHAR;
typedef void *HWND;
typedef void *HDC;
typedef unsigned int UINT;
typedef int BOOL;
typedef void *HINSTANCE;
typedef VECT2DF BODUV;
typedef VECT3DF BOD;
typedef VECT4DF WBOD;
typedef RGBF BODRGB;
typedef RGBAF BODRGBA;
typedef RGBF RGB_M;
typedef RGBAF BARVA_RGBA;
typedef PLANE ROVINA;

//typedef MATERIAL_TEXT   EDIT_TEXT;
//typedef SURFACE_SW      bitmapa;
typedef word FACE;

typedef struct _ROVINAD
{
  double x, y, z, e;
} ROVINAD;

typedef struct _RECT
{
  int left;
  int top;
  int right;
  int bottom;
} RECT, *PRECT;

#include "kfile.h"
#include "Bmp.h"
#include "Texture.h"
#include "Matrix.h"
#include "Mat3d.h"
#include "Vektor.h"
#include "3dinterface.h"
#include "3dproject.h"
#include "3dmath.h"
#include "obbo.h"
#include "Quat.h"
#include "Keyframe.h"
#include "Load.h"
#include "Load_out.h"
#include "Di.h"
#include "Ddx.h"
#include "Load_chunks.h"
#include "Kdtree.h"
#include "2D_graphic.h"

typedef void (*END_FUNKCE) (int param, int param2, void *p_param);

inline void ZeroMemory(void *mem, int size)
{
  memset(mem, 0, size);
}

// default filename size
#define MAX_PATH 255

typedef struct _MOUSE_INFO
{
  int x_res, y_res;

  int x, y;
  int rx, ry;
  int dx, dy;

  int t1, t2;
  int dt1, dt2;
  int d1, d2;
  int tf1, tf2;
  int lt1, lt2;

} MOUSE_INFO;

#define RGB(r,g,b) int_rgb(r,g,b)
#define CP_ACP     0
#define TRANSCOLOR int_rgb(255, 0, 255)
#define HDC2DD     -1

void kprintf(char log, const char *p_text, ...);
void kprintfl(char log, const char *p_text, ...);
void ddw(char *p_text, ...);

extern char ini_file[MAX_FILENAME];

typedef struct
{
  int tmp;
} AUDIO_DATA;

void Sleep(int ms);

//char *itoa(int base, char *buf, int d);
char *itoa(int d, char *buf, int base);

void timeGetTimeInit(void);
unsigned int timeGetTime(void);

//-----------------------------------------------------------------------------
// ddx2 interface
//-----------------------------------------------------------------------------
extern RECT_LINE rline;
extern RECT rDrawRect[DRAW_RECT_NUM];


//------------------------------------------------------------------------------------------------
// Draw all
//------------------------------------------------------------------------------------------------
void ddxPublish(void);

//-----------------------------------------------------------------------------
// Name: DisplayFrame()
// Desc: Blts a the sprites to the back buffer, then flips the 
//       back buffer onto the primary buffer.
//-----------------------------------------------------------------------------
int DisplayFrame();

//------------------------------------------------------------------------------------------------
// init 2d
//------------------------------------------------------------------------------------------------
int ddxInit(void);

//------------------------------------------------------------------------------------------------
// 2d release
//------------------------------------------------------------------------------------------------
void ddxRelease(void);

//------------------------------------------------------------------------------------------------
// finds first free index
//------------------------------------------------------------------------------------------------
int ddxFindFreeSurface(void);

//------------------------------------------------------------------------------------------------
// release bitmap
//------------------------------------------------------------------------------------------------
int ddxReleaseBitmap(int iSurface);

//------------------------------------------------------------------------------------------------
// load list of bitmaps from APAK
//------------------------------------------------------------------------------------------------
int ddxLoadList(char *pFileName, int bProgress);

//------------------------------------------------------------------------------------------------
// load bitmap from APAK
//------------------------------------------------------------------------------------------------
int ddxLoadBitmap(const char *pFileName, APAK_HANDLE * pHandle);

//------------------------------------------------------------------------------------------------
// Create Surface
//------------------------------------------------------------------------------------------------
int ddxCreateSurface(int x, int y, int idx);

//------------------------------------------------------------------------------------------------
// nakresli obrazek
//------------------------------------------------------------------------------------------------
void ddxDrawSurfaceColorKey(int iSurface, int *com, int layer,
  COLORREF color);
void ddxDrawDisplayColorKey(int *com, int layer, COLORREF color);
void ddxDrawDisplay(int *com, int layer);
void ddxDrawSurface(int iSurface, int *com, int layer);

BOOL ddxTransparentBlt(int dcDestSurface,       // handle to Dest DC
  int nXOriginDest,             // x-coord of destination upper-left corner
  int nYOriginDest,             // y-coord of destination upper-left corner
  int nWidthDest,               // width of destination rectangle
  int nHeightDest,              // height of destination rectangle
  int dcSrcSurface,             // handle to source DC
  int nXOriginSrc,              // x-coord of source upper-left corner
  int nYOriginSrc,              // y-coord of source upper-left corner
  int nWidthSrc,                // width of source rectangle
  int nHeightSrc,               // height of source rectangle
  UINT crTransparent            // color to make transparent
  );

BOOL ddxTransparentBltDisplay(int nXOriginDest, // x-coord of destination upper-left corner
  int nYOriginDest,             // y-coord of destination upper-left corner
  int nWidthDest,               // width of destination rectangle
  int nHeightDest,              // height of destination rectangle
  int dcSrcSurface,             // handle to source DC
  int nXOriginSrc,              // x-coord of source upper-left corner
  int nYOriginSrc,              // y-coord of source upper-left corner
  int nWidthSrc,                // width of source rectangle
  int nHeightSrc,               // height of source rectangle
  UINT crTransparent            // color to make transparent
  );

BOOL ddxBitBlt(int dcDestSurface,       // handle to Dest DC
  int nXOriginDest,             // x-coord of destination upper-left corner
  int nYOriginDest,             // y-coord of destination upper-left corner
  int nWidthDest,               // width of destination rectangle
  int nHeightDest,              // height of destination rectangle
  int dcSrcSurface,             // handle to source DC
  int nXOriginSrc,              // x-coord of source upper-left corner
  int nYOriginSrc               // y-coord of source upper-left corner
  );

BOOL ddxBitBltDisplay(int nXOriginDest, // x-coord of destination upper-left corner
  int nYOriginDest,             // y-coord of destination upper-left corner
  int nWidthDest,               // width of destination rectangle
  int nHeightDest,              // height of destination rectangle
  int dcSrcSurface,             // handle to source DC
  int nXOriginSrc,              // x-coord of source upper-left corner
  int nYOriginSrc               // y-coord of source upper-left corner
  );

int ddxGetWidth(int iSurface);
int ddxGetHight(int iSurface);

void ddxCleareSurface(int iSurface);
void ddxCleareSurfaceColor(int iSurface, COLORREF color);
void ddxFillRect(int iSurface, RECT * rect, COLORREF color);
void ddxFillRectDisplay(RECT * rect, COLORREF color);
void ddxAddRectItem(void *p_rl, RECT rect, int iLayer);
int ddxStretchBltDisplay(RECT * rDest, int iSurface, RECT * rSource);
int ddxStretchBlt(int iSDest, RECT * rDest, int iSSource, RECT * rSource);

int ddxUpdateMouse(void);

void ddxSetFlip(char bSwitch);
void ddxSetCursor(char bSwitch);
void ddxResizeCursorBack(int iSurface);
void ddxSetCursorSurface(int iSurface);

BOOL ddxRestore(AUDIO_DATA * p_ad);

void ddxSaveSurface(int idx);

void ddxCleareSurfaceColorDisplay(COLORREF color);

int ddxGetMode(void);

void InitDirectDraw(HWND hWnd, int x, int y, int bpp);
void FreeDirectDraw();

char MenuCheckBossExit(void);

int ogg_playing(void);

char *strlwr(char *cFile);

void dbgprintf(char *p_tmp, ...);

#define MAKEWORD(a, b)      ((unsigned short)(((unsigned char)(a)) | ((unsigned short)((unsigned char)(b))) << 8))

#define CP_ACP 0

int MultiByteToWideChar(int CodePage,
  int dwFlags,
  char *lpMultiByteStr,
  int cbMultiByte, WCHAR * lpWideCharStr, int cchWideChar);

#define WC_COMPOSITECHECK 0
#define WC_DEFAULTCHAR    0

int WideCharToMultiByte(int CodePage,
  int dwFlags,
  wchar_t * lpWideCharStr,
  int cchWideChar,
  char *lpMultiByteStr,
  int cbMultiByte, char *lpDefaultChar, int *lpUsedDefaultChar);

// Audio interface
// Init
void ap_Init(AUDIO_DATA * p_ad);

// Release
void ap_Release(AUDIO_DATA * p_ad);

// loades play list
int ap_Load_Play_List(char *p_File_Name, AUDIO_DATA * p_ad);

// Releases play list
void ap_Release_Play_List(AUDIO_DATA * p_ad);

// playes ogg song
int ap_Play_Song(int Index, char Random, AUDIO_DATA * p_ad);
int ap_Setup_and_Play_Song(int Index, char Random, AUDIO_DATA * p_ad);

// stops ogg song
void ap_Stop_Song(AUDIO_DATA * p_ad);

// Plays sound
int ap_Play_Sound(int Type, char Relative, char bAmbient, float *p_Pos,
  int Wave_Index, void *p_eax, AUDIO_DATA * p_ad);

// Releases materail list
void ap_Release_Material_List(AUDIO_DATA * p_ad);

// loades material list
int ap_Load_Material_List(char *p_File_Name, AUDIO_DATA * p_ad);

// count environment
void ap_Count_Environment(AUDIO_DATA * p_ad, void *p_Level);

// load environment
int ap_Load_Environment(char *p_Env_Name, void *p_Level, AUDIO_DATA * p_ad);

int ap_Load_Sound_List(AUDIO_DATA * p_ad, char *cFile, int iStart);

void ShowCursor(bool state);
void SetCursor(void *tmp);

char *strupr(char *string);

void GetPrivateProfileString(const char *lpAppName,
  const char *lpKeyName,
  const char *lpDefault,
  char *lpReturnedString, int nSize, const char *lpFileName);

int WritePrivateProfileString(const char *lpAppName,
  const char *lpKeyName, const char *lpString, const char *lpFileName);

int GetPrivateProfileInt(const char *lpAppName,
  const char *lpKeyName, int nDefault, const char *lpFileName);

inline void null_free(void **p_mem)
{
  if ((*p_mem) != NULL) {
    free(*p_mem);
    *p_mem = NULL;
  }
}

void apak_dir_correction(char *dir);

wchar_t *wchar_windows_to_linux(word * p_in, int bytes_in_len);
void wchar_windows_to_linux(word * p_in, int str_len, wchar_t * p_out);
void wchar_linux_to_windows(wchar_t * p_in, int str_len, word * p_out);


typedef SDL_TimerID TIMER_ID;

typedef void (*TIMERPROC) (HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime);
TIMER_ID SetTimer(HWND hWnd, TIMER_ID nIDEvent, UINT uElapse, TIMERPROC lpTimerFunc);
UINT KillTimer(HWND hWnd, TIMER_ID uIDEvent);

void working_dir_init(void);
void working_file_translate(char *p_file, int size);
char *working_file_get(const char *p_file);
char *working_file_get(const char *p_file, char *p_target, int size);
