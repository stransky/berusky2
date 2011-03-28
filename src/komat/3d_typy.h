/*
  Univerzalni 3d typy
*/

#ifndef  __3D_TYPY__
#define  __3D_TYPY__


typedef struct
{
  float x,y,z;
} BOD,VEKTOR,VX,POZICE_BODU;

typedef struct
{
  float u,v;
} BODUV;

typedef struct _ROVINA {
  float x,y,z,e;
} ROVINA;

typedef struct _ROVINAD {
  double x,y,z,e;
} ROVINAD;

typedef struct
{
  float x,y,z,w;
} WBOD;

typedef struct
{
  dword x,y;
} BOD_2D;

typedef struct
{
  int x,y,z;
} BOD3D,QBOD;

typedef struct _BARVA_RGBA {
  
  float r,g,b,a;

} BARVA_RGBA,BODRGBA,RGBA;

typedef struct _BARVA_RGB {
  
  float r,g,b;

} BARVA_RGB,BODRGB,RGB;

typedef word FACE;

typedef struct _RGB_M {

float r,g,b;

} RGB_M;

typedef void (* END_FUNKCE)(int param, int param2, void *p_param);

#endif
