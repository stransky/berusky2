#include "3d_all.h"
#include "ini.h"

void Sleep(int ms)
{
}

int ftoi(float t)
{
  return((int)t);
}

unsigned int timeGetTime(void)
{
}

//-----------------------------------------------------------------------------
// Name: DisplayFrame()
// Desc: Blts a the sprites to the back buffer, then flips the 
//       back buffer onto the primary buffer.
//-----------------------------------------------------------------------------
int DisplayFrame()
{
}

int ddxFindFreeSurface(void)
{
  return(0);
}

int ddxReleaseBitmap(int iSurface)
{
}

int ddxLoadList(char *pFileName, int bProgress)
{
}

int ddxLoadBitmap(char *pFileName, APAK_HANDLE *pHandle)
{
}

int ddxCreateSurface(int x, int y, int idx)
{
}

void ddxDrawSurfaceColorKey(int iSurface, int *com, int layer, COLORREF color)
{
}
void ddxDrawDisplayColorKey(int *com, int layer, COLORREF color)
{
}
void ddxDrawDisplay(int *com, int layer)
{
}
void ddxDrawSurface(int iSurface, int *com, int layer)
{
}

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
  )
{
}

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
  )
{
}

BOOL ddxBitBlt(
    int dcDestSurface,  // handle to Dest DC
    int nXOriginDest,   // x-coord of destination upper-left corner
    int nYOriginDest,   // y-coord of destination upper-left corner
    int nWidthDest,     // width of destination rectangle
    int nHeightDest,    // height of destination rectangle
    int dcSrcSurface,   // handle to source DC
    int nXOriginSrc,    // x-coord of source upper-left corner
    int nYOriginSrc     // y-coord of source upper-left corner
  )
{
}

BOOL ddxBitBltDisplay(
    int nXOriginDest,   // x-coord of destination upper-left corner
    int nYOriginDest,   // y-coord of destination upper-left corner
    int nWidthDest,     // width of destination rectangle
    int nHeightDest,    // height of destination rectangle
    int dcSrcSurface,   // handle to source DC
    int nXOriginSrc,    // x-coord of source upper-left corner
    int nYOriginSrc     // y-coord of source upper-left corner
  )
{
}

int ddxGetWidth(int iSurface)
{
}
int ddxGetHight(int iSurface)
{
}

void ddxCleareSurface(int iSurface)
{
}
void ddxCleareSurfaceColor(int iSurface, COLORREF color)
{
}
void ddxFillRect(int iSurface, RECT *rect, COLORREF color)
{
}
void ddxFillRectDisplay(RECT *rect, COLORREF color)
{
}
void ddxAddRectItem(void *p_rl, RECT rect, int iLayer)
{
}
int  ddxStretchBltDisplay(RECT *rDest, int iSurface, RECT *rSource)
{
}
int  ddxStretchBlt(int iSDest, RECT *rDest, int iSSource, RECT *rSource)
{
}

int ddxUpdateMouse(void)
{
}

void ddxSetFlip(char bSwitch)
{
}
void ddxSetCursor(char bSwitch)
{
}
void ddxResizeCursorBack(int iSurface)
{
}
void ddxSetCursorSurface(int iSurface)
{
}

BOOL ddxRestore(int *p_CompositDC, int *p_FontDC, int *p_BackDC, int *p_iCompositDC, int *p_iFontDC, int *p_iBackDC, 
        char *p_cBrutalRestart, AUDIO_DATA *p_ad)
{
}

void ddxSaveSurface(int idx)
{
}

void ddxCleareSurfaceColorDisplay(COLORREF color)
{
}

int ddxGetMode(void)
{
}

int ogg_playing(void)
{
}

char * strlwr(char *cFile)
{
}

void dbgprintf(char *p_tmp, ...)
{
}

int MultiByteToWideChar(
  int CodePage,
  int dwFlags,
  char * lpMultiByteStr,
  int cbMultiByte,
  WCHAR * lpWideCharStr,
  int cchWideChar
)
{
}

// Audio interface
// Init
void ap_Init(AUDIO_DATA *p_ad)
{
}

// Release
void ap_Release(AUDIO_DATA *p_ad)
{
}

// loades play list
int ap_Load_Play_List(char *p_File_Name, AUDIO_DATA *p_ad)
{
}

// Releases play list
void ap_Release_Play_List(AUDIO_DATA *p_ad)
{
}

// playes ogg song
int ap_Play_Song(int Index, char Random, AUDIO_DATA *p_ad)
{
}
int ap_Setup_and_Play_Song(int Index, char Random, AUDIO_DATA *p_ad)
{
}

// stops ogg song
void ap_Stop_Song(AUDIO_DATA *p_ad)
{
}

// Plays sound
int ap_Play_Sound(int Type, char Relative, char bAmbient, float *p_Pos, int Wave_Index, void *p_eax, AUDIO_DATA *p_ad)
{
}

// Releases materail list
void ap_Release_Material_List(AUDIO_DATA *p_ad)
{
}

// loades material list
int ap_Load_Material_List(char *p_File_Name, AUDIO_DATA *p_ad)
{
}

// count environment
void ap_Count_Environment(AUDIO_DATA *p_ad, void *p_Level)
{
}

// load environment
int ap_Load_Environment(char *p_Env_Name, void *p_Level, AUDIO_DATA *p_ad)
{
}

int ap_Load_Sound_List(AUDIO_DATA *p_ad, char *cFile, int iStart)
{
}

void ShowCursor(bool state)
{
}

void SetCursor(void *tmp)
{
}

char * strupr(char * string)
{
}
/*
GLAPI GLuint APIENTRY glNewObjectBufferATI (GLsizei t1, const GLvoid *t2, GLenum t3)
{
}
GLAPI GLboolean APIENTRY glIsObjectBufferATI (GLuint t1)
{
}
GLAPI void APIENTRY glUpdateObjectBufferATI (GLuint t1, GLuint t2, GLsizei t3, const GLvoid *t4, GLenum t5)
{
}
GLAPI void APIENTRY glGetObjectBufferfvATI (GLuint t1, GLenum t2, GLfloat * t3)
{
}
GLAPI void APIENTRY glGetObjectBufferivATI (GLuint t1, GLenum t2, GLint * t3)
{
}
GLAPI void APIENTRY glFreeObjectBufferATI (GLuint t)
{
}
GLAPI void APIENTRY glArrayObjectATI (GLenum t1, GLint t2, GLenum t3, GLsizei t4, GLuint t5, GLuint t6)
{
}
GLAPI void APIENTRY glGetArrayObjectfvATI (GLenum t, GLenum t1, GLfloat * t2)
{
}
GLAPI void APIENTRY glGetArrayObjectivATI (GLenum t, GLenum t1, GLint * t2)
{
}
GLAPI void APIENTRY glVariantArrayObjectATI (GLuint t, GLenum t1, GLsizei t2, GLuint t3, GLuint t4)
{
}
GLAPI void APIENTRY glGetVariantArrayObjectfvATI (GLuint t, GLenum t1, GLfloat * t2)
{
}
GLAPI void APIENTRY glGetVariantArrayObjectivATI (GLuint t, GLenum t2, GLint * t3)
{
}

GLAPI void APIENTRY glElementPointerATI (GLenum t, const GLvoid * t1)
{
}
GLAPI void APIENTRY glDrawElementArrayATI (GLenum t, GLsizei t1)
{
}
GLAPI void APIENTRY glDrawRangeElementArrayATI (GLenum t1, GLuint t2, GLuint t3, GLsizei t4)
{
}
*/

void GetPrivateProfileString(
  char * lpAppName, // section name
  char * lpKeyName, // key name
  char * lpDefault, // default key value
  char * lpReturnedString, 
  int nSize,
  char * lpFileName // ini file name
)
{

}

int WritePrivateProfileString(
  char * lpAppName,
  char * lpKeyName,
  char * lpString,
  char * lpFileName
)
{

}

int GetPrivateProfileInt(
  char * lpAppName,
  char * lpKeyName,
  int nDefault,
  char * lpFileName
)
{

}

bitmapa * bmp_vyrob(int x, int y)
{
}
bitmapa * bmp_nahraj(char *p_file, int flip)
{
}
bitmapa * bmp_nahraj_pack(char *p_file, APAK_HANDLE *pAHandle)
{
}
int       bmp_uloz(char *p_file, bitmapa *p_bmp)
{
}
int       bmp_uloz_pack(FFILE f, bitmapa *p_bmp)
{
}
void      bmp_zrus(bitmapa **p_bmp)
{
}
bitmapa * bmp_kopituj(bitmapa *p_bmp)
{
}
void      bmp_kopiruj(bitmapa *p_src, bitmapa *p_desc)
{
}
void      bmp_cti_rozmery(bitmapa *p_bmp, int *p_x, int *p_y)
{
}
int       bmp_velikost(bitmapa *p_bmp)
{
}
void      bmp_putpixel(bitmapa *p_bmp, int x, int y, dword barva)
{
}
void      bmp_smaz(bitmapa *p_text, dword barva)
{
}

int  txt_vyrob_2D_texturu(int x, int y, int filtr, int format)
{
}
int  txt_zrus_2D_texturu(int *p_text)
{
}
