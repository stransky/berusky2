/*
  h pro dir imput
*/

#ifndef __DI_H
#define __DI_H

#ifdef __cplusplus
extern "C" {
#endif

#define  POCET_KLAVES    0xff

/* Definice klaves v pole_key */
#define  K_ESC     1
#define  K_1       2
#define  K_2       3
#define  K_3       4
#define  K_4       5
#define  K_5       6
#define  K_6       7
#define  K_7       8
#define  K_8       9
#define  K_9       10
#define  K_0       11
#define  K_MINUS   12
#define  K_PLUS    13
#define  K_BKSP    14
#define  K_TAB     15
#define  K_Q       16
#define  K_W       17
#define  K_E       18
#define  K_R       19
#define  K_T       20
#define  K_Y       21
#define  K_U       22
#define  K_I       23
#define  K_O       24
#define  K_P       25
#define  K_ZAV_L   26
#define  K_ZAV_P   27
#define  K_ENTER   28
#define  K_CTRL    29
#define  K_A       30
#define  K_S       31
#define  K_D       32
#define  K_F       33
#define  K_G       34
#define  K_H       35
#define  K_J       36
#define  K_K       37
#define  K_L       38
#define  K_STREDNIK 39
#define  K_UVOZ    40
#define  K_TILDA   41
#define  K_SHFT_L  42
#define  K_SHIFT   42
#define  K_BACKSLASH 43
#define  K_Z       44
#define  K_X       45
#define  K_C       46
#define  K_V       47
#define  K_B       48
#define  K_N       49
#define  K_M       50
#define  K_CARKA   51
#define  K_TECKA   52
#define  K_SLASH   53
#define  K_SHFT_P  54
#define  K_PTRSCR  55
#define  K_ALT     56
#define  K_SPACE   57
#define  K_CAPSLOCK 58
#define  K_F1      59
#define  K_F2      60
#define  K_F3      61
#define  K_F4      62
#define  K_F5      63
#define  K_F6      64
#define  K_F7      65
#define  K_F8      66
#define  K_F9      67
#define  K_F10     68
#define  K_F11     87
#define  K_F12     88
#define  K_NUMLOCK 69
#define  K_SCRLOCK 70
#define  K_HOME    71
#define  K_NAHORU  72
#define  K_PGUP    73
#define  K_K_MINUS 74
#define  K_DOLEVA  75
#define  K_K_5     76
#define  K_DOPRAVA 77
#define  K_K_PLUS  78
#define  K_END     79
#define  K_DOLU    80
#define  K_PGDN    81
#define  K_INS     82
#define  K_DEL     83

extern   byte    _a_klavesa;
extern   byte    key[POCET_KLAVES];
extern	 TCHAR	 key_pressed;

typedef struct _MYSI_INFO {

  int x_min, x_max;
  int y_min, y_max;
  int x_res, y_res;

  int x, y;  
  int dx,dy;

  int t1,t2;
  int dt1,dt2;
  int ot1,ot2;
  int d1,d2;

} MYSI_INFO;

#define stisk_mys() (spracuj_spravy(TRUE) && !(mi.t1 || mi.t2))
#define nuluj_mys() { mi.t1 = mi.t2 = 0; mi.dx = mi.dy = 0; mi.dt1 = mi.dt2 = 0; }
#define nuluj_dif() { mi.dx = mi.dy = 0; }

extern MYSI_INFO mi;

int  zapni_kurzor(void);
int  vypni_kurzor(void);

#ifdef __cplusplus
}
#endif

#endif

