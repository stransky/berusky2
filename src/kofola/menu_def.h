#ifndef __MENU_DEF_
#define __MENU_DEF_

#include <SDL/SDL.h>

#define CLIST_ITEMC 55
#define TAB_NUM		4
#define TAB_X		90
#define TAB_Y		88
#define TAB_XRES	850
#define TAB_YRES	650
#define MENU_ANIMATION_CORECTION 1

typedef struct _CMD_LINE
{
  int iParam[6];
  char cParam[2][256];
  int iAnim[200][18];
  SDL_TimerID uiTimerID;
  int iLastfrm;
  long iCounter;
  int iLayer;
  char bActive;
  char bEndActivate[2];
  struct _CMD_LINE *pCmdLine;
} CMD_LINE;

typedef struct _LIST_ITEM_
{
  char text[256];
  long timespamp;
} LIST_ITEM_;

int FillStringList(char *cmask, LIST_ITEM_ ** list, int *isize);

#endif
