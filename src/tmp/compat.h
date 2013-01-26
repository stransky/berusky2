/*
 *        .þÛÛþ þ    þ þÛÛþ.     þ    þ þÛÛÛþ.  þÛÛÛþ .þÛÛþ. þ    þ
 *       .þ   Û Ûþ.  Û Û   þ.    Û    Û Û    þ  Û.    Û.   Û Ûþ.  Û
 *       Û    Û Û Û  Û Û    Û    Û   þ. Û.   Û  Û     Û    Û Û Û  Û
 *     .þþÛÛÛÛþ Û  Û Û þÛÛÛÛþþ.  þþÛÛ.  þþÛÛþ.  þÛ    Û    Û Û  Û Û
 *    .Û      Û Û  .þÛ Û      Û. Û   Û  Û    Û  Û.    þ.   Û Û  .þÛ
 *    þ.      þ þ    þ þ      .þ þ   .þ þ    .þ þÛÛÛþ .þÛÛþ. þ    þ
 *
 * Berusky (C) AnakreoN
 * Martin Stransky <stransky@anakreon.cz> 
 * Michal Simonik <simonik@anakreon.cz> 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef __COMPAT_H__
#define __COMPAT_H__

#include "compat_mini.h"
#include "Apak.h"

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

typedef void (*END_FUNKCE) (size_ptr param, size_ptr param2, size_ptr param3);

#define RGB(r,g,b) int_rgb(r,g,b)
#define TRANSCOLOR int_rgb(255, 0, 255)

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
int ddxLoadBitmap(char *pFileName, APAK_HANDLE * pHandle);

//------------------------------------------------------------------------------------------------
// Create Surface
//------------------------------------------------------------------------------------------------
int ddxCreateSurface(int x, int y, int idx);

//------------------------------------------------------------------------------------------------
// nakresli obrazek
//------------------------------------------------------------------------------------------------
void ddxDrawSurfaceColorKey(int iSurface, int *com, int layer, COLORREF color);
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

bool ddxRestore(AUDIO_DATA * p_ad);

void ddxSaveSurface(int idx);

void ddxCleareSurfaceColorDisplay(COLORREF color);

int ddxGetMode(void);

void InitDirectDraw(void);
void FreeDirectDraw(void);

char MenuCheckBossExit(void);

char *strlwr(char *cFile);

void dbgprintf(char *p_tmp, ...);

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

char * apak_dir_correction(char *dir);

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

extern char cCheckMusicExeption;

void alut_test(int ini);

#endif //__COMPAT_H__
