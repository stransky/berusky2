/*
  Vsechny include pro 3D
*/

#ifndef  __3D_ALL__
#define  __3D_ALL__

#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>

//#define  GL_GLEXT_PROTOTYPES

#include "glext.h"
#include "wglext.h"

#include <stdio.h>
#include <io.h>
#include <direct.h>
#include <math.h>
#include <assert.h>
#include <float.h>

#include <il\il.h>

#include "mmalloc.h"

#include "zlib.h"
#include "zlib_io.h"

#define  MAX_JMENO       50

#include "wzakl_typ.h"

void kprintf(byte log, byte *p_text,...);
void kprintfl(byte log, byte *p_text,...);
void ddw(byte *p_text,...);

#include "..\\kofola\\level_environment.h"
#include "..\\Kofola\\apak.h"

#include "kfile.h"

#include "3d_typy.h"
#include "texture.h"
#include "ddx.h"
#include "gglext.h"
#include "di.h"
#include "matrix.h"
#include "color.h"
#include "mat3d.h"
#include "strip.h"
#include "load.h"
#include "load_chunks.h"
#include "load_out.h"
#include "vektor.h"
#include "kdtree.h"
#include "keyframe.h"
#include "3dmath.h"
#include "3dproject.h"
#include "quat.h"
#include "3dinterface.h"
#include "bmp.h"
#include "obb.h"

#endif