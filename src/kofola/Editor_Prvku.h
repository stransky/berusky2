#ifndef __EDITOR_PRVKU_
#define __EDITOR_PRVKU_

#include "object.h"

extern OBJECTDESC *Object;
extern SECONDDATADESC *SecondData;

OBJECTDESC *ko_Nahraj_herni_prvky(void);
void ko_Uvolni_herni_prvky(void);
int ko_GetLayer(int Guid);

#endif
