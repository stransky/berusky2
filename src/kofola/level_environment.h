#ifndef __LEVEL_ENVIRONMENT_H__
#define __LEVEL_ENVIRONMENT_H__

typedef struct
{
  char bUsed;
  int iWeather;
  unsigned int uiDensity;
  unsigned int uiIntensity;
  float fmaxDivergence;
  float fDist;
  float fSky;
  int iReserved[10];
} WEATHERSTRUCT;

typedef struct
{
  char bUsed;
  int EffectID;
  float fRadius;
  unsigned int uiDensity;
  unsigned int uiIntensity;
  float fSky;
  float fLowPlane;
  int iReserved[10];
} NATUREEFFECTSTRUCT;

typedef struct
{
  NATUREEFFECTSTRUCT NatureEffect[10];
  WEATHERSTRUCT Weather[3];
  char cCam[32];
  char cEnv[32];
  char cAnimEff[32];
  int Reserved[92];
} LEVEL_ENVIRONMENT;

#endif
