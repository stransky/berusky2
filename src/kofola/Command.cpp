//------------------------------------------------------------------------------------------------
// 0.0.1
//------------------------------------------------------------------------------------------------
#include "3d_all.h"
#include "Berusky3d_kofola_interface.h"
#include "2D_graphic.h"

extern _2D_DATA _2dd;
extern RECT_LINE rline;

//------------------------------------------------------------------------------------------------
// nakresli obrazek
//------------------------------------------------------------------------------------------------
void menucommand_Draw(HDC hdc, int *com, int layer)
{
/*	RECT r;

	r.left = com[2];
	r.top = com[3];
	r.right = _2dd.bitmap[com[1]].bitmap.bmWidth;
	r.bottom = _2dd.bitmap[com[1]].bitmap.bmHeight;

	BitBlt(hdc,r.left,r.top,
		   r.right,
		   r.bottom,
		   _2dd.bitmap[com[1]].bitmapDC,0,0,SRCCOPY);

	_2d_Add_RectItem(&rline, r, layer);*/
}

void menucommand_DrawT(HDC hdc, int *com)
{
/*	TransparentBltU(hdc,com[2],com[3],
				  _2dd.bitmap[com[1]].bitmap.bmWidth,
				  _2dd.bitmap[com[1]].bitmap.bmHeight,
				  _2dd.bitmap[com[1]].bitmapDC,0,0,
				  _2dd.bitmap[com[1]].bitmap.bmWidth, _2dd.bitmap[com[1]].bitmap.bmHeight, RGB(255, 0, 255));*/
}
