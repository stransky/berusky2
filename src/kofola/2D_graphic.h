#ifndef __2D_GRAPHIC_
#define __2D_GRAPHIC_

typedef struct
{
  RECT rect;
  int iLayer;
  char bUsed;
} DRAW_RECT;

#define DRAW_RECT_NUM	40

typedef struct
{
  DRAW_RECT rect[DRAW_RECT_NUM];
  int rlast;
} RECT_LINE;

void _2d_Add_RectItem(RECT_LINE * p_rl, RECT rect, int iLayer);
void _2d_Add_RectItem_IfNPresent(RECT_LINE * p_rl, RECT rect, int iLayer);
int  _2d_Is_InRectLine(RECT * rline, RECT * p_r, int size);
void _2d_Clear_RectLine(RECT_LINE * p_rl);

#endif
