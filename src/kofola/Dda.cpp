//------------------------------------------------------------------------------------------------
// 1.0.0
//------------------------------------------------------------------------------------------------
#include <math.h>
#include <stdlib.h>
#include "Dda.h"

//------------------------------------------------------------------------------------------------
// prvni krok dda primky
//------------------------------------------------------------------------------------------------
void dda_First(DDA * dda)
{
//      int             p[2];

  if (abs(dda->y1 - dda->y2) < abs(dda->x1 - dda->x2)) {
/*		if (dda->x1 > dda->x2)
		{
			p[0]=dda->x2;
			p[1]=dda->y2;
			dda->x2=dda->x1;
			dda->y2=dda->y1;
			dda->x1=p[0];
			dda->y1=p[1];
		}*/

    dda->y = dda->y1;
    dda->plus = (float) abs(dda->y1 - dda->y2) / (float) (dda->x2 - dda->x1);
    dda->realy = (float) dda->y1;
    dda->x = dda->x1;
    dda->main_axe = 0;
    //dda->main_axe = dda->x2 - dda->x1;
  }
  else {
/*		if (dda->y1 > dda->y2)
		{
			p[0]=dda->x2;
			p[1]=dda->y2;
			dda->x2=dda->x1;
			dda->y2=dda->y1;
			dda->x1=p[0];
			dda->y1=p[1];
		}*/

    dda->x = dda->x1;
    dda->plus = (float) abs(dda->x1 - dda->x2) / (float) (dda->y2 - dda->y1);
    dda->realx = (float) dda->x1;
    dda->y = dda->y1;
    //dda->main_axe = dda->y2 - dda->y1;
    dda->main_axe = 0;
  }
}

//------------------------------------------------------------------------------------------------
// dalsi krok dda primky
//------------------------------------------------------------------------------------------------
int dda_Next(DDA * dda)
{
  if (abs(dda->y1 - dda->y2) < abs(dda->x1 - dda->x2)) {
    if (dda->x == dda->x2)
      return 0;

    if (dda->x1 < dda->x2)
      dda->x++;
    else
      dda->x--;

    if (dda->y1 < dda->y2)
      dda->realy = dda->realy + dda->plus;
    else
      dda->realy = dda->realy - dda->plus;

    dda->y = (int) floor(dda->realy);
  }
  else {
    if (dda->y == dda->y2)
      return 0;

    if (dda->y1 < dda->y2)
      dda->y++;
    else
      dda->y--;

    if (dda->x1 < dda->x2)
      dda->realx = dda->realx + dda->plus;
    else
      dda->realx = dda->realx - dda->plus;

    dda->x = (int) floor(dda->realx);
  }

  dda->main_axe++;
  return 1;
}

//------------------------------------------------------------------------------------------------
// dalsi krok dda primky
//------------------------------------------------------------------------------------------------
int dda_Prev(DDA * dda)
{
  if (abs(dda->y1 - dda->y2) < abs(dda->x1 - dda->x2)) {
    if (dda->x == dda->x1)
      return 0;

    if (dda->x1 < dda->x2)
      dda->x--;
    else
      dda->x++;

    if (dda->y1 < dda->y2)
      dda->realy = dda->realy - dda->plus;
    else
      dda->realy = dda->realy + dda->plus;

    dda->y = (int) floor(dda->realy);
  }
  else {
    if (dda->y == dda->y1)
      return 0;

    if (dda->y1 < dda->y2)
      dda->y--;
    else
      dda->y++;

    if (dda->x1 < dda->x2)
      dda->realx = dda->realx - dda->plus;
    else
      dda->realx = dda->realx + dda->plus;

    dda->x = (int) floor(dda->realx);
  }

  dda->main_axe--;
  return 1;
}
