//------------------------------------------------------------------------------------------------
// version 0.0.2
//------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "3d_all.h"
#include "Berusky_universal.h"
#include "2D_graphic.h"
#include "Berusky3d_ini.h"

//------------------------------------------------------------------------------------------------
// Rectangles
//------------------------------------------------------------------------------------------------
void _2d_Clear_RectLine(RECT_LINE * p_rl)
{
  memset(p_rl, 0, sizeof(RECT_LINE));
  p_rl->rlast = 0;
}

int _2d_Is_InRectLine(RECT * rline, RECT * p_r, int size)
{
  int i;

  for (i = 0; i < size; i++) {
    if (rline[i].bottom == p_r->bottom &&
        rline[i].left == p_r->left &&
        rline[i].right == p_r->right && 
        rline[i].top == p_r->top)
    {
      return 1;
    }
  }

  return 0;
}

void _2d_Add_RectItem_IfNPresent(RECT_LINE * p_rl, RECT rect, int iLayer)
{
  int i;

  for (i = 0; i < p_rl->rlast; i++)
    if (p_rl->rect[i].rect.bottom == rect.bottom &&
      p_rl->rect[i].rect.left == rect.left &&
      p_rl->rect[i].rect.right == rect.right &&
      p_rl->rect[i].rect.top == rect.top)
      return;

  if (p_rl->rlast < DRAW_RECT_NUM) {
    p_rl->rect[p_rl->rlast].rect = rect;
    p_rl->rect[p_rl->rlast].iLayer = iLayer;
    p_rl->rect[p_rl->rlast].bUsed = 1;

    p_rl->rlast++;
  }
}

void _2d_Add_RectItem(RECT_LINE * p_rl, RECT rect, int iLayer)
{
  if (p_rl->rlast < DRAW_RECT_NUM) {
    p_rl->rect[p_rl->rlast].rect = rect;
    p_rl->rect[p_rl->rlast].iLayer = iLayer;
    p_rl->rect[p_rl->rlast].bUsed = 1;

    p_rl->rlast++;
  }
}
