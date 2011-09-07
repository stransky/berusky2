/*
  h pro dir imput
*/

#ifndef __DI_H
#define __DI_H

#ifdef __cplusplus
extern "C"
{
#endif

#define  POCET_KLAVES SDLK_LAST+3

/* Definice klaves v pole_key */
#define  K_ZAV_L    K_BRACKET_L
#define  K_ZAV_P    K_BRACKET_R
#define  K_STREDNIK K_SEMICOL
#define  K_UVOZ     K_QUOTE
#define  K_SHFT_L   SDLK_LSHIFT
#define  K_SHFT_P   SDLK_RSHIFT
#define  K_CARKA    K_COMMA
#define  K_TECKA    K_PERIOD
#define  K_PTRSCR   SDLK_PRINT
#define  K_CAPSLOCK SDLK_CAPSLOCK
#define  K_NUMLOCK  SDLK_NUMLOCK
#define  K_SCRLOCK  SDLK_SCROLLOCK
#define  K_NAHORU   K_UP
#define  K_K_MINUS  SDLK_KP_MINUS
#define  K_DOLEVA   K_LEFT
#define  K_K_5      KP_5
#define  K_DOPRAVA  K_RIGHT
#define  K_K_PLUS   SDLK_KP_PLUS
#define  K_DOLU     K_DOWN
#define  K_INS      K_INSERT

#define  K_SHIFT    SDLK_LAST
#define  K_CTRL     SDLK_LAST+1
#define  K_ALT      SDLK_LAST+2


  extern int key[POCET_KLAVES];
  extern int key_pressed;

  typedef struct _MYSI_INFO
  {

    int x_min, x_max;
    int y_min, y_max;
    int x_res, y_res;

    int x, y;
    int dx, dy;

    int t1, t2;
    int dt1, dt2;
    int ot1, ot2;
    int d1, d2;

  } MYSI_INFO;

#define stisk_mys() (spracuj_spravy(TRUE) && !(mi.t1 || mi.t2))
#define nuluj_mys() { mi.t1 = mi.t2 = 0; mi.dx = mi.dy = 0; mi.dt1 = mi.dt2 = 0; }
#define nuluj_dif() { mi.dx = mi.dy = 0; }

  extern MYSI_INFO mi;

  int zapni_kurzor(void);
  int vypni_kurzor(void);

#ifdef __cplusplus
}
#endif

#endif
