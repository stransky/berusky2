#ifndef __MENU_SCRIPT_
#define __MENU_SCRIPT_

#include "menu_def.h"

#define COM_UNKNOWN			-1
#define COM_NOCOMMAND		0
#define COM_DRAW			1
#define COM_FLIP			2
#define COM_ONABOVE			3
#define COM_ONCLICK			4
#define COM_RANDOMANIMATION	5
#define COM_RUNLEVEL		6
#define COM_COMICS			7
#define COM_BINDSOUND		8
#define COM_LOADTEXTURES	9
#define COM_INITOPENGL		10
#define COM_RELEASEOPENGL	11
#define COM_RUNANIMATION	12
#define COM_BINDEXITANIMATION	13
#define COM_BINDANIMATION	14
#define COM_CREATEBUTTON	15
#define COM_SETRECT			16

int Find_Next_Expresion(char *p_Command, int start, char *p_Expresion);

void Parse_Line(FILE * file, int *result, int res_size, char *comfile1,
  char *comfile2);
void Parse_AnimLine(FILE * file, int *result, int res_size);
void Parse_ScenarioLine(FILE * file, int *result, int res_size,
  char *comfile1, char *comfile2);
void Parse_LineT(char *ftext, int *result, int res_size, char *comfile1,
  char *comfile2);

void LoadMenuScript(const char *p_File_Name, CMD_LINE * res, int *lastcmd);
void LoadAnimationMenuScript(CMD_LINE * res, int i, int *lastanm);

#endif
