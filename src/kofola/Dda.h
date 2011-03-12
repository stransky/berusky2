//------------------------------------------------------------------------------------------------
// 1.0.0
//------------------------------------------------------------------------------------------------

#ifndef __DDA_
#define __DDA_

typedef struct
{
	int		x,y,x1,x2,y1,y2;
	float	plus;
	float	realy,realx;
	int		main_axe;
} DDA;

	// prvni krok dda primky
	void dda_First(DDA *dda);

	// dalsi krok dda primky
	int  dda_Next(DDA *dda);

	// krok zpet
	int dda_Prev(DDA *dda);

#endif