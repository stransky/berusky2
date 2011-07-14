#ifndef __LOAD_
#define __LOAD_

#include "Object.h"

        // nahraje level
int lsi_Load_Level(char *p_Level_Name, LEVELINFO * p_Level);

        // release levelu
void lsi_Release_Level(LEVELINFO * p_Level);

        // pripoji k predmetum materialy
void lsi_Bind_Materials2Items(LEVELINFO * p_Level);

        //void lsi_Make_Screenshot(HDC hdc, char *pFile);

void lsi_Save_Level(WCHAR * pwc_Level_Name, LEVELINFO * p_Level);
int lsi_Load_Saved_Level(char *p_Level_Name, LEVELINFO * p_Level);

void lsi_Load_Level_Script(LEVELINFO * p_Level, char *cFile);

int lsi_Get_Save_Info(char *p_Level_Name, int *pActLevel, int *pActScene);

void lsi_Get_Dir_Name(char *cText, char *cLevel);
int lsi_Save_Exist(WCHAR * wName, char *cFile);

int lsi_Create_Level_Raw(char *p_Level_Name, BUNKA_LEVELU_DISK ** b_l_d,
  int *size);

#endif
