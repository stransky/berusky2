#ifndef _2DDX_H_
#define _2DDX_H_

//------------------------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------------------------
#include <ddraw.h>
#include "audio_plug-in.h"

#ifdef __cplusplus
	extern "C" {
#endif

typedef struct _MOUSE_INFO {

  int x_min, x_max;
  int y_min, y_max;
  int x_res, y_res;

  int x, y;
  int rx, ry;
  int dx,dy;

  int t1,t2;
  int dt1,dt2;
  int d1,d2;
  int tf1, tf2;
  int lt1, lt2;

} MOUSE_INFO;

	#define HDC2DD		-1
	#define TRANSCOLOR	RGB(255, 0, 255)

	//-----------------------------------------------------------------------------
	// Name: InitDirectDraw()
	// Desc: Create the DirectDraw object, and init the surfaces
	//-----------------------------------------------------------------------------
	HRESULT InitDirectDraw( HWND hWnd , int x, int y, int bpp);

	//-----------------------------------------------------------------------------
	// Name: FreeDirectDraw()
	// Desc: Release all the DirectDraw objects
	//-----------------------------------------------------------------------------
	VOID FreeDirectDraw();

	//-----------------------------------------------------------------------------
	// Name: DisplayFrame()
	// Desc: Blts a the sprites to the back buffer, then flips the 
	//       back buffer onto the primary buffer.
	//-----------------------------------------------------------------------------
	HRESULT DisplayFrame();
	HRESULT DisplayFramePure();

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
	int ddxLoadBitmap(char *pFileName, APAK_HANDLE *pHandle);

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

	BOOL ddxTransparentBlt(
			int dcDestSurface,  // handle to Dest DC
			int nXOriginDest,   // x-coord of destination upper-left corner
			int nYOriginDest,   // y-coord of destination upper-left corner
			int nWidthDest,     // width of destination rectangle
			int nHeightDest,    // height of destination rectangle
			int dcSrcSurface,   // handle to source DC
			int nXOriginSrc,    // x-coord of source upper-left corner
			int nYOriginSrc,    // y-coord of source upper-left corner
			int nWidthSrc,      // width of source rectangle
			int nHeightSrc,     // height of source rectangle
			UINT crTransparent  // color to make transparent
		);

	BOOL ddxTransparentBltDisplay(
			int nXOriginDest,   // x-coord of destination upper-left corner
			int nYOriginDest,   // y-coord of destination upper-left corner
			int nWidthDest,     // width of destination rectangle
			int nHeightDest,    // height of destination rectangle
			int dcSrcSurface,   // handle to source DC
			int nXOriginSrc,    // x-coord of source upper-left corner
			int nYOriginSrc,    // y-coord of source upper-left corner
			int nWidthSrc,      // width of source rectangle
			int nHeightSrc,     // height of source rectangle
			UINT crTransparent  // color to make transparent
		);

	BOOL ddxBitBlt(
			int dcDestSurface,  // handle to Dest DC
			int nXOriginDest,   // x-coord of destination upper-left corner
			int nYOriginDest,   // y-coord of destination upper-left corner
			int nWidthDest,     // width of destination rectangle
			int nHeightDest,    // height of destination rectangle
			int dcSrcSurface,   // handle to source DC
			int nXOriginSrc,    // x-coord of source upper-left corner
			int nYOriginSrc     // y-coord of source upper-left corner
		);

	BOOL ddxBitBltDisplay(
			int nXOriginDest,   // x-coord of destination upper-left corner
			int nYOriginDest,   // y-coord of destination upper-left corner
			int nWidthDest,     // width of destination rectangle
			int nHeightDest,    // height of destination rectangle
			int dcSrcSurface,   // handle to source DC
			int nXOriginSrc,    // x-coord of source upper-left corner
			int nYOriginSrc     // y-coord of source upper-left corner
		);

	int ddxGetWidth(int iSurface);
	int ddxGetHight(int iSurface);

	void ddxCleareSurface(int iSurface);
	void ddxCleareSurfaceColor(int iSurface, COLORREF color);
	void ddxFillRect(int iSurface, RECT *rect, COLORREF color);
	void ddxFillRectDisplay(RECT *rect, COLORREF color);
	void ddxAddRectItem(RECT_LINE *p_rl, RECT rect, int iLayer);
	HRESULT ddxStretchBltDisplay(RECT *rDest, int iSurface, RECT *rSource);
	HRESULT ddxStretchBlt(int iSDest, RECT *rDest, int iSSource, RECT *rSource);

	HRESULT ddxUpdateMouse(VOID);
	
	void ddxSetFlip(char bSwitch);
	void ddxSetCursor(char bSwitch);
	void ddxResizeCursorBack(int iSurface);
	void ddxSetCursorSurface(int iSurface);

	BOOL ddxRestore(int *p_CompositDC, int *p_FontDC, int *p_BackDC, int *p_iCompositDC, int *p_iFontDC, int *p_iBackDC, 
					char *p_cBrutalRestart, AUDIO_DATA *p_ad);

	void ddxSaveSurface(int idx);
	
	void ddxCleareSurfaceColorDisplay(COLORREF color);

	int ddxGetMode(void);

#ifdef __cplusplus
	}
#endif

#endif
