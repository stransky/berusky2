/*
  rutiny pro obsluhu mysi a klavesnice
*/

#include "3d_all.h"

int  key[POCET_KLAVES];
int  key_pressed;
MYSI_INFO mi;

int zapni_kurzor(void)
{
 ShowCursor(TRUE);
 return(TRUE);
}

int vypni_kurzor(void)
{
 ShowCursor(FALSE);
 return(TRUE);
}
