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

#include <sys/time.h>

#include "3d_all.h"
#include "ini.h"

#include "3d_all.h"

#include "Object.h"

#include "Berusky_universal.h"
#include "Berusky3d_castice.h"
#include "Berusky3d.h"
#include "Berusky3d_ini.h"
#include "Berusky3d_load.h"
#include "Berusky3d_render.h"
#include "Berusky3d_animace.h"
#include "Berusky3d_kofola_interface.h"
#include "Berusky3d_light.h"
#include "Berusky3d_kamery.h"

#include "Berusky3d_kofola2d.h"

char cCheckMusicExeption = 0;

void Sleep(int ms)
{
  SDL_Delay(ms);
}

int ftoi(float t)
{
  return ((int) roundf(t));
}

static int timeGetTimeSet = 0;

void timeGetTimeInit(void)
{
  timeGetTimeSet = 1;
}

unsigned int timeGetTime(void)
{
  assert(timeGetTimeSet);
  return (current_time_get());
}

//-----------------------------------------------------------------------------
// ddx2 interface
//-----------------------------------------------------------------------------
extern APAK_HANDLE *pBmpArchive;
DeviceHandle ddxDevice;
DeviceHandle ddxCursorDevice;
int i_CursorDDX = 0;
int i_CursorDDXChange = TRUE;
int bDrawCursor = 0;
int ddxInitDone = FALSE;
int bFlip;
static long bLastGameState = 1;
extern MOUSE_INFO dim;
int start_x = 0;
int start_y = 0;

#define CURSOR_DEVICE_DX 75
#define CURSOR_DEVICE_DY 75

//-----------------------------------------------------------------------------
// Name: ddxDrawCursor()
// Desc: Draw cursor.
//-----------------------------------------------------------------------------
void ddxDrawCursor(void)
{
  if(bDrawCursor) {
    ddx2DrawCursor(i_CursorDDXChange ? i_CursorDDX : NO_SURFACE,
                   dim.x_orig, dim.y_orig,
                   CURSOR_DEVICE_DX, CURSOR_DEVICE_DY,
                   TRANSCOLOR);
    if(i_CursorDDXChange)
      i_CursorDDXChange = FALSE;
  }
}

//-----------------------------------------------------------------------------
// Name: DisplayFrame()
// Desc: Blts a the sprites to the back buffer, then flips the 
//       back buffer onto the primary buffer.
//-----------------------------------------------------------------------------
int DisplayFrame()
{
  ddxPublish();
  return(TRUE);
}

void ddxSetCursorSurface(int iSurface)
{
  i_CursorDDX = iSurface;
  i_CursorDDXChange = TRUE;
}

int ddxInit(void)
{
  if (!ddxInitDone) {
    ddx2Init(10000, RGB(255, 0, 255));

    ddxSetCursor(1);
    ddxSetCursorSurface(0);
    ddxSetFlip(1);

    // Create cursor device
    ddxCursorDevice = ddx2DeviceCreate(TRUE, 32, TRUE);
    ddx2DeviceSetActive(ddxCursorDevice);
    ddx2DeviceSetBackBufferSize(CURSOR_DEVICE_DX, CURSOR_DEVICE_DY);
    ddx2DeviceSetBackBufferRect(0, 0, CURSOR_DEVICE_DX, CURSOR_DEVICE_DY);
    ddx2DeviceSetTextRenderRec(0, 0, CURSOR_DEVICE_DX, CURSOR_DEVICE_DY);
    ddx2DeviceSetScreenRec(0, 0, CURSOR_DEVICE_DX, CURSOR_DEVICE_DY);
    ddx2DeviceSetRender(TRUE);
  
    // Create screen device
    ddxDevice = ddx2DeviceCreate(TRUE, 32);
    ddx2DeviceSetActive(ddxDevice);

    ddx2DeviceSetBackBufferSize(1024, 768);
    ddx2DeviceSetBackBufferRect(0, 0, 1024, 768);
    ddx2DeviceSetTextRenderRec(0, 0, 1024, 768);
          
    ddx2DeviceSetScreenRec(0, 0, hwconf.xres, hwconf.yres);
    ddx2DeviceSetRender(TRUE);
  
    ddxInitDone = TRUE;
  }
  return (TRUE);
}

void ddxRelease(void)
{
  if (ddxInitDone) {
    ddx2DeviceRemove(ddxCursorDevice);
    ddx2DeviceRemove(ddxDevice);
    ddx2Release();
    ddxInitDone = FALSE;
  }
}

void ddxPublish(void)
{
  int i, x = 0;

  ddxDrawCursor();

  if (!rline.rlast)
    ddx2SetRect(NULL, 0);
  else {
    ZeroMemory(rDrawRect, sizeof(RECT) * DRAW_RECT_NUM);
    for (i = 0; i < rline.rlast; i++) {
      if (!_2d_Is_InRectLine(rDrawRect, &rline.rect[i].rect, x)) {
        memcpy(&rDrawRect[x], &rline.rect[i].rect, sizeof(RECT));
        x++;
      }
    }
    ddx2SetRect(rDrawRect, x);
  }

  spracuj_spravy(0);
  ddx2RenderujVse(p_ber);

  flip();

  _2d_Clear_RectLine(&rline);
}

int ddxFindFreeSurface(void)
{
  return (ddx2FindFreeSurface());
}

int ddxReleaseBitmap(int iSurface)
{
  return (ddx2ReleaseBitmap(iSurface));
}

int ddxLoadList(char *pFileName, int bProgress)
{
  if (bProgress) {
    ddxSetCursor(0);
    ddxSetFlip(0);
  }

  char bmp_dir[MAX_PATH];
  GetPrivateProfileString("game", "bitmap_dir", "", bmp_dir, MAX_PATH, (const char *) ini_file);
  if(!bmp_dir[0])
    kerror(TRUE, "Unable to read bitmap_dir from ini file '%s'",ini_file);

  kprintf(1, "Kofola: - Load bitmap pro herni menu");
  ddx2LoadList(pFileName, pBmpArchive, bmp_dir);

  if (bProgress) {
    ddxSetCursor(1);
    ddxSetFlip(1);
  }

  return (1);
}

int ddxLoadBitmap(char *pFileName, APAK_HANDLE * pHandle)
{
  return (ddx2LoadBitmap(pFileName, pHandle));
}

int ddxCreateSurface(int x, int y, int idx)
{
  return (ddx2CreateSurface(x, y, idx));
}

void ddxDrawSurfaceColorKey(int iSurface, int *com, int layer, COLORREF color)
{
  ddx2DrawSurfaceColorKey(iSurface, com, layer, color);
}

void ddxDrawDisplayColorKey(int *com, int layer, COLORREF color)
{
  ddx2DrawDisplayColorKey(com, layer, color);
}

void ddxDrawDisplay(int *com, int layer)
{
  ddx2DrawDisplay(com, layer);
}

void ddxDrawSurface(int iSurface, int *com, int layer)
{
  ddx2DrawSurface(iSurface, com, layer);
}

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
  )
{
  bool ret = ddx2TransparentBlt(dcDestSurface,
    nXOriginDest,
    nYOriginDest,
    nWidthDest,
    nHeightDest,
    dcSrcSurface,
    nXOriginSrc,
    nYOriginSrc,
    crTransparent);

  return (ret);
}

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
  )
{
  bool ret = ddx2TransparentBltDisplay(nXOriginDest,
    nYOriginDest,
    nWidthDest,
    nHeightDest,
    dcSrcSurface,
    nXOriginSrc,
    nYOriginSrc,
    nWidthSrc,
    nHeightSrc,
    crTransparent);

  return (ret);
}

BOOL ddxBitBlt(int dcDestSurface,       // handle to Dest DC
  int nXOriginDest,             // x-coord of destination upper-left corner
  int nYOriginDest,             // y-coord of destination upper-left corner
  int nWidthDest,               // width of destination rectangle
  int nHeightDest,              // height of destination rectangle
  int dcSrcSurface,             // handle to source DC
  int nXOriginSrc,              // x-coord of source upper-left corner
  int nYOriginSrc               // y-coord of source upper-left corner
  )
{
  return ddx2BitBlt(dcDestSurface,
    nXOriginDest,
    nYOriginDest,
    nWidthDest, nHeightDest, dcSrcSurface, nXOriginSrc, nYOriginSrc);
}

BOOL ddxBitBltDisplay(int nXOriginDest, // x-coord of destination upper-left corner
  int nYOriginDest,             // y-coord of destination upper-left corner
  int nWidthDest,               // width of destination rectangle
  int nHeightDest,              // height of destination rectangle
  int dcSrcSurface,             // handle to source DC
  int nXOriginSrc,              // x-coord of source upper-left corner
  int nYOriginSrc               // y-coord of source upper-left corner
  )
{
  int ret = ddx2BitBltDisplay(nXOriginDest,
    nYOriginDest,
    nWidthDest,
    nHeightDest,
    dcSrcSurface,
    nXOriginSrc,
    nYOriginSrc);

  return (ret);
}

int ddxGetWidth(int iSurface)
{
  return (ddx2GetWidth(iSurface));
}

int ddxGetHight(int iSurface)
{
  return (ddx2GetHeight(iSurface));
}

void ddxCleareSurface(int iSurface)
{
  ddx2CleareSurface(iSurface);
}

void ddxCleareSurfaceColor(int iSurface, COLORREF color)
{
  ddx2CleareSurfaceColor(iSurface, color);
}

void ddxFillRect(int iSurface, RECT * rect, COLORREF color)
{
  ddx2FillRect(iSurface, rect, color);
}

void ddxFillRectDisplay(RECT * rect, COLORREF color)
{
  ddx2FillRect(DDX2_BACK_BUFFER, rect, color);
}

void ddxAddRectItem(void *p_rl, RECT rect, int iLayer)
{
  ddx2AddRectItem((RECT_LINE *) p_rl, rect, iLayer);
}

int ddxStretchBltDisplay(RECT * rDest, int iSurface, RECT * rSource)
{  
  return(ddx2BitBltStretch(DDX2_BACK_BUFFER,
                           rDest->left, rDest->top,
                           rDest->right - rDest->left,
                           rDest->bottom - rDest->top,
                           iSurface,
                           rSource->left, rSource->top,
                           rSource->right - rSource->left,
                           rSource->bottom - rSource->top));
}

int ddxStretchBlt(int iSDest, RECT * rDest, int iSSource, RECT * rSource)
{
  assert(0); // not implemented
  return(TRUE);
}

int ddxUpdateMouse(void)
{
  float s_factor[] = { (float) 1024 / (float) hwconf.xres, 
                       (float) 768 / (float) hwconf.yres };
  spracuj_spravy(1);
                     
  dim.x_orig = mi.x;
  dim.y_orig = mi.y;

  dim.dx = (int)ceil(mi.dx * s_factor[0]);
  dim.dy = (int)ceil(mi.dy * s_factor[1]);

  dim.x = (int)ceil(mi.x * s_factor[0]);
  dim.y = (int)ceil(mi.y * s_factor[1]);

  dim.rx = dim.x - start_x;
  dim.ry = dim.y - start_y;

  dim.t1 = dim.dt1 = mi.t1;
  dim.t2 = dim.dt2 = mi.t2;

  if (!dim.t1)
    dim.lt1 = 0;

  if (!dim.t2)
    dim.lt2 = 0;

  if (dim.t1 && dim.lt1)
    dim.t1 = 0;

  if (dim.t2 && dim.lt2)
    dim.t2 = 0;

  if (!dim.lt1 && dim.t1) {
    dim.lt1 = 1;
    dim.tf1 = 1;
  }

  if (!dim.lt2 && dim.t2) {
    dim.lt2 = 1;
    dim.tf2 = 1;
  }
  
  return TRUE;
}

void ddxSetFlip(char bSwitch)
{
  bFlip = bSwitch;
}

void ddxSetCursor(char bSwitch)
{
  bDrawCursor = bSwitch;
  ddx2DrawCursorSetDraw(bSwitch);
}

void ddxResizeCursorBack(int iSurface)
{
  // We don't need this on Linux
}

void ddxRestore(AUDIO_DATA * p_ad)
{
  static dword dwLastMenuMusicCheck = 0;

	if(karmin_aktivni && timeGetTime() - dwLastMenuMusicCheck > 20000)
	{
		if(!ogg_playing() && !cCheckMusicExeption)
			ap_Play_Song(0, 0, p_ad);

		dwLastMenuMusicCheck = timeGetTime();
	}

  if (bLastGameState != karmin_aktivni) {
    bLastGameState = karmin_aktivni;

    // Game is restored
    if (bLastGameState) {
			if(!ogg_playing())
				ap_Play_Song(0, 0, p_ad);
    }
    else {
      adas_Release_Source(-1, ALL_TYPES, UNDEFINED_VALUE);
      adas_Release_Source(ALL_SOUND_SOURCES, ALL_TYPES,UNDEFINED_VALUE); 
      ap_Stop_Song(p_ad);
    }
  }

  if (MenuCheckBossExit()) {
    gl_Kofola_End(1);
  }
}

void ddxSaveSurface(int idx)
{
  // TODO
  assert(0);
}

void ddxCleareSurfaceColorDisplay(COLORREF color)
{
  ddx2CleareSurfaceColor(DDX2_BACK_BUFFER, color);
  RECT r = {0,0,ddx2GetWidth(DDX2_BACK_BUFFER),ddx2GetHeight(DDX2_BACK_BUFFER)};
  ddx2AddRectItem(&rline, r);
}

// Return always "windowed" mode
int ddxGetMode(void)
{
  return (TRUE);
}

void InitDirectDraw(void)
{
  bInMenu = 1;
}

void FreeDirectDraw()
{
  bInMenu = 0;
}

char *strlwr(char *cFile)
{
  char *tmp = cFile;

  while (*tmp) {
    *tmp = tolower(*tmp);
    tmp++;
  }
  return (cFile);
}

void dbgprintf(char *p_tmp, ...)
{
}

int MultiByteToWideChar(int CodePage, int dwFlags, char *lpMultiByteStr,
                        int cbMultiByte, WCHAR * lpWideCharStr, int cchWideChar)
{
  int ret = mbstowcs(lpWideCharStr, lpMultiByteStr, cchWideChar);
  assert(ret && ret < cchWideChar);
  return (ret);
}

int WideCharToMultiByte(int CodePage, int dwFlags, wchar_t * lpWideCharStr,
                        int cchWideChar, char *lpMultiByteStr, int cbMultiByte, 
                        char *lpDefaultChar, int *lpUsedDefaultChar)
{
  return (wcstombs(lpMultiByteStr, lpWideCharStr, cbMultiByte));
}

void ShowCursor(bool state)
{
  SDL_ShowCursor(state ? SDL_ENABLE : SDL_DISABLE);
}

void SetCursor(void *tmp)
{  
  // WinAPI - we don't need it
}

char *strupr(char *string)
{
  // TODO
  assert(0);
}

void GetPrivateProfileString(const char *lpAppName,     // section name
  const char *lpKeyName,        // key name
  const char *lpDefault,        // default key value
  char *lpReturnedString, 
  int nSize, 
  const char *lpFileName     // ini file name
  )
{ 
  ini_read_string_section(lpFileName, lpAppName, lpKeyName, 
                          lpReturnedString, nSize, lpDefault);
  path_correction(lpReturnedString, nSize);
}

int WritePrivateProfileString(const char *lpAppName,    // section name
  const char *lpKeyName, const char *lpString, const char *lpFileName)
{
  return(ini_write_string_section(lpFileName, lpAppName, lpKeyName, lpString));
}

int GetPrivateProfileInt(const char *lpAppName, // section name
  const char *lpKeyName, int nDefault, const char *lpFileName)
{
  return(ini_read_int_section(lpFileName, lpAppName, lpKeyName, nDefault));
}

void wchar_windows_to_linux(word * p_in, int str_len, wchar_t * p_out)
{
  int i;

  for (i = 0; i < str_len; i++)
    *p_out++ = *p_in++;
}

void wchar_linux_to_windows(wchar_t * p_in, int str_len, word * p_out)
{
  int i;

  for (i = 0; i < str_len; i++)
    *p_out++ = *p_in++;
}

// in place replacement
wchar_t *wchar_windows_to_linux(word * p_in, int bytes_in_len)
{
  wchar_t *p_tmp = (wchar_t *) mmalloc(bytes_in_len * 2);
  wchar_windows_to_linux(p_in, bytes_in_len/2, p_tmp);
  return (p_tmp);
}

/* type definition for the "new" timer callback function */
Uint32 callback(Uint32 interval, void *param)
{
  TIMERPROC func = (TIMERPROC) param;

  func(NULL, 0, 0, interval);
  return (interval);
}

TIMER_ID SetTimer(HWND hWnd, TIMER_ID nIDEvent, UINT uElapse,
  TIMERPROC lpTimerFunc)
{
  return (SDL_AddTimer(uElapse, callback, (void *) lpTimerFunc));
}

UINT KillTimer(HWND hWnd, TIMER_ID uIDEvent)
{
  return (SDL_RemoveTimer(uIDEvent));
}

char current_working_dir[PATH_MAX];
char current_working_dir_file[PATH_MAX];

void working_dir_init(void)
{
  getcwd(current_working_dir, PATH_MAX);
}

char *working_file_get(const char *p_file)
{
  return (return_path_ext(p_file, current_working_dir,
      current_working_dir_file, PATH_MAX));
}

char *working_file_get(const char *p_file, char *p_target, int size)
{
  return (return_path_ext(p_file, current_working_dir, p_target, size));
}

void working_file_translate(char *p_file, int size)
{
  return_path_ext(p_file, current_working_dir, current_working_dir_file, PATH_MAX);
  assert(size <= PATH_MAX);
  strcpy(p_file, current_working_dir_file);
}

void GetFileSize(FILE *f, dword *size)
{
  *size = file_size_get(f);
}
