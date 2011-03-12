/*
  Vsechny include pro 3D
*/

#ifndef  __3D_ALL__
#define  __3D_ALL__

#include <GL/gl.h>
#include <GL/glu.h>

//#define  GL_GLEXT_PROTOTYPES

#include "glext.h"

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <float.h>
#include <limits.h>

#include "mmalloc.h"

#include "zlib.h"
#include "zlib_io.h"

#define  MAX_JMENO       50

#define  TRUE   (1==1)
#define  FALSE  (1!=1)

typedef unsigned char   byte;
typedef unsigned short  word;
typedef unsigned int    dword;
typedef unsigned int    COLORREF;
typedef dword           DWORD;
typedef float           FLOAT;
typedef unsigned short  WCHAR;
typedef void *          HWND;
typedef void *          HDC;
typedef unsigned int    UINT;
typedef int             BOOL;


typedef struct _RECT {
  int left;
  int top;
  int right;
  int bottom;
} RECT, *PRECT;

inline void ZeroMemory(void *mem, int size)
{
  memset(mem,0,size);
}

typedef void AUDIO_DATA;

#define MAX_BYTE 0xff
#define MAX_PATH PATH_MAX

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

void kprintf(char log, char *p_text,...);
void kprintfl(char log, char *p_text,...);
void ddw(char *p_text,...);

#include "level_environment.h"
#include "Apak.h"

#include "kfile.h"

#include "3d_typy.h"
#include "Texture.h"
#include "Ddx.h"
#include "gglext.h"
#include "Di.h"
#include "Matrix.h"
#include "Color.h"
#include "Mat3d.h"
#include "strip.h"
#include "Load.h"
#include "Load_chunks.h"
#include "Load_out.h"
#include "Vektor.h"
#include "Kdtree.h"
#include "Keyframe.h"
#include "3dmath.h"
#include "3dproject.h"
#include "Quat.h"
#include "3dinterface.h"
#include "Bmp.h"
#include "obb.h"

#endif
