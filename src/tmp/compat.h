#include "Apak.h"

typedef struct
{
  int tmp;
} AUDIO_DATA;


void Sleep(int ms);
int ftoi(float t);

char * itoa(int base, char *buf, int d);

unsigned int timeGetTime(void);

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
void ddxAddRectItem(void *p_rl, RECT rect, int iLayer);
int  ddxStretchBltDisplay(RECT *rDest, int iSurface, RECT *rSource);
int  ddxStretchBlt(int iSDest, RECT *rDest, int iSSource, RECT *rSource);

int ddxUpdateMouse(void);

void ddxSetFlip(char bSwitch);
void ddxSetCursor(char bSwitch);
void ddxResizeCursorBack(int iSurface);
void ddxSetCursorSurface(int iSurface);

BOOL ddxRestore(int *p_CompositDC, int *p_FontDC, int *p_BackDC, int *p_iCompositDC, int *p_iFontDC, int *p_iBackDC, 
        char *p_cBrutalRestart, AUDIO_DATA *p_ad);

void ddxSaveSurface(int idx);

void ddxCleareSurfaceColorDisplay(COLORREF color);

int ddxGetMode(void);

int ogg_playing(void);

char * strlwr(char *cFile);

void dbgprintf(char *p_tmp, ...);

#define MAKEWORD(a, b)      ((unsigned short)(((unsigned char)(a)) | ((unsigned short)((unsigned char)(b))) << 8))

int MultiByteToWideChar(
  int CodePage,
  int dwFlags,
  char * lpMultiByteStr,
  int cbMultiByte,
  WCHAR * lpWideCharStr,
  int cchWideChar
);

// Audio interface
// Init
void ap_Init(AUDIO_DATA *p_ad);

// Release
void ap_Release(AUDIO_DATA *p_ad);

// loades play list
int ap_Load_Play_List(char *p_File_Name, AUDIO_DATA *p_ad);

// Releases play list
void ap_Release_Play_List(AUDIO_DATA *p_ad);

// playes ogg song
int ap_Play_Song(int Index, char Random, AUDIO_DATA *p_ad);
int ap_Setup_and_Play_Song(int Index, char Random, AUDIO_DATA *p_ad);

// stops ogg song
void ap_Stop_Song(AUDIO_DATA *p_ad);

// Plays sound
int ap_Play_Sound(int Type, char Relative, char bAmbient, float *p_Pos, int Wave_Index, void *p_eax, AUDIO_DATA *p_ad);

// Releases materail list
void ap_Release_Material_List(AUDIO_DATA *p_ad);

// loades material list
int ap_Load_Material_List(char *p_File_Name, AUDIO_DATA *p_ad);

// count environment
void ap_Count_Environment(AUDIO_DATA *p_ad, void *p_Level);

// load environment
int ap_Load_Environment(char *p_Env_Name, void *p_Level, AUDIO_DATA *p_ad);

int ap_Load_Sound_List(AUDIO_DATA *p_ad, char *cFile, int iStart);

void ShowCursor(bool state);
void SetCursor(void *tmp);

char * strupr(char * string);

GLAPI void APIENTRY glArrayObjectATI (GLenum t1, GLint t2, GLenum t3, GLsizei t4, GLuint t5, GLuint t6);
GLAPI GLuint APIENTRY glNewObjectBufferATI (GLsizei, const GLvoid *, GLenum);
GLAPI GLboolean APIENTRY glIsObjectBufferATI (GLuint);
GLAPI void APIENTRY glUpdateObjectBufferATI (GLuint, GLuint, GLsizei, const GLvoid *, GLenum);
GLAPI void APIENTRY glGetObjectBufferfvATI (GLuint, GLenum, GLfloat *);
GLAPI void APIENTRY glGetObjectBufferivATI (GLuint, GLenum, GLint *);
GLAPI void APIENTRY glFreeObjectBufferATI (GLuint);
GLAPI void APIENTRY glArrayObjectATI (GLenum, GLint, GLenum, GLsizei, GLuint, GLuint);
GLAPI void APIENTRY glGetArrayObjectfvATI (GLenum, GLenum, GLfloat *);
GLAPI void APIENTRY glGetArrayObjectivATI (GLenum, GLenum, GLint *);
GLAPI void APIENTRY glVariantArrayObjectATI (GLuint, GLenum, GLsizei, GLuint, GLuint);
GLAPI void APIENTRY glGetVariantArrayObjectfvATI (GLuint, GLenum, GLfloat *);
GLAPI void APIENTRY glGetVariantArrayObjectivATI (GLuint, GLenum, GLint *);

GLAPI void APIENTRY glElementPointerATI (GLenum, const GLvoid *);
GLAPI void APIENTRY glDrawElementArrayATI (GLenum, GLsizei);
GLAPI void APIENTRY glDrawRangeElementArrayATI (GLenum, GLuint, GLuint, GLsizei);


void GetPrivateProfileString(
  char * lpAppName,
  char * lpKeyName,
  char * lpDefault,
  char * lpReturnedString,
  int nSize,
  char * lpFileName
);

int WritePrivateProfileString(
  char * lpAppName,
  char * lpKeyName,
  char * lpString,
  char * lpFileName
);

int GetPrivateProfileInt(
  char * lpAppName,
  char * lpKeyName,
  int nDefault,
  char * lpFileName
);
