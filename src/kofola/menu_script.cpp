#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "menu_script.h"
#include "menu_def.h"
#include "Apak.h"
#include "3d_all.h"

extern APAK_HANDLE *pDataArchive;

//------------------------------------------------------------------------------------------------
// najde prvni platny znak
//------------------------------------------------------------------------------------------------
int Find_First_Valid_Char(int start, char *text)
{
  int l = strlen(text);
  int i;

  for (i = start; i < l; i++)
    if (text[i] != 32 && text[i] != '(' && text[i] != 9 && text[i] != ',') {
      if (text[i] != ')')
        return i;
      else
        return -1;
    }

  return -1;
}

int Find_First_Valid_CharU(int start, WCHAR * text)
{
  int l = wcslen(text);
  int i;

  for (i = start; i < l; i++)
    if (text[i] != MAKEWORD(32, 0) &&
      text[i] != MAKEWORD('(', 0) &&
      text[i] != MAKEWORD(9, 0) && text[i] != MAKEWORD(',', 0)) {
      if (text[i] != MAKEWORD(')', 0)) {
        if (i > 0)
          if (text[i - 1] == MAKEWORD('\\', 0))
            continue;
        return i;
      }
      else
        return -1;
    }

  return -1;
}

//------------------------------------------------------------------------------------------------
// najde prvni platny oddelovac
//------------------------------------------------------------------------------------------------
int Find_First_Separator(int start, char *text)
{
  int l = strlen(text);
  int i;

  for (i = start; i < l; i++)
    if (text[i] == 32 ||
      text[i] == '(' ||
      text[i] == 9 ||
      text[i] == ')' || text[i] == ',' || text[i] == 10 || text[i] == 13)
      return i;

  return -1;
}

int Find_First_SeparatorU(int start, WCHAR * text)
{
  int l = wcslen(text);
  int i;

  for (i = start; i < l; i++)
    if (text[i] == MAKEWORD(32, 0) ||
      text[i] == MAKEWORD('(', 0) ||
      text[i] == MAKEWORD(9, 0) ||
      text[i] == MAKEWORD(')', 0) ||
      text[i] == MAKEWORD(',', 0) ||
      text[i] == MAKEWORD(10, 0) || text[i] == MAKEWORD(13, 0)) {
      if (i > 0)
        if (text[i - 1] == MAKEWORD('\\', 0))
          continue;

      return i;
    }

  return -1;
}

//------------------------------------------------------------------------------------------------
// najde prvni platny vyraz
//------------------------------------------------------------------------------------------------
int Find_Next_Expresion(char *p_Command, int start, char *p_Expresion)
{
  int first, last;

  if (start == -1)
    return -1;

  if (p_Command[start] == 10 || p_Command[start] == 13)
    return -1;

  first = Find_First_Valid_Char(start, p_Command);
  if (first == -1)
    return -1;
  last = Find_First_Separator(first, p_Command);
  if (last == -1)
    return -1;

  strncpy(p_Expresion, &p_Command[first], last - first);
  p_Expresion[last - first] = '\0';

  return last;
}

int Find_Next_ExpresionU(WCHAR * p_Command, int start, WCHAR * p_Expresion)
{
  int first, last;

  if (start == -1)
    return -1;

  if (p_Command[start] == 10 || p_Command[start] == 13)
    return -1;

  first = Find_First_Valid_CharU(start, p_Command);
  if (first == -1)
    return -1;
  last = Find_First_SeparatorU(first, p_Command);
  if (last == -1)
    return -1;

  wcsncpy(p_Expresion, &p_Command[first], last - first);
  p_Expresion[last - first] = '\0';

  return last;
}

//------------------------------------------------------------------------------------------------
// prevede string na prikaz
//------------------------------------------------------------------------------------------------
int String2Command(char *string)
{
  if (string == NULL)
    return COM_NOCOMMAND;

  if (string[0] == '\0')
    return COM_NOCOMMAND;

  if (!strcmp(string, "Draw"))
    return COM_DRAW;

  if (!strcmp(string, "Flip"))
    return COM_FLIP;

  if (!strcmp(string, "OnAbove"))
    return COM_ONABOVE;

  if (!strcmp(string, "OnClick"))
    return COM_ONCLICK;

  if (!strcmp(string, "RandomAnimation"))
    return COM_RANDOMANIMATION;

  if (!strcmp(string, "RunLevel"))
    return COM_RUNLEVEL;

  if (!strcmp(string, "Comics"))
    return COM_COMICS;

  if (!strcmp(string, "BindSound"))
    return COM_BINDSOUND;

  if (!strcmp(string, "LoadTextures"))
    return COM_LOADTEXTURES;

  if (!strcmp(string, "InitOpenGL"))
    return COM_INITOPENGL;

  if (!strcmp(string, "ReleaseOpenGL"))
    return COM_RELEASEOPENGL;

  if (!strcmp(string, "RunAnimation"))
    return COM_RUNANIMATION;

  if (!strcmp(string, "BindExitAnimation"))
    return COM_BINDEXITANIMATION;

  if (!strcmp(string, "BindAnimation"))
    return COM_BINDANIMATION;

  if (!strcmp(string, "CreateButton"))
    return COM_CREATEBUTTON;

  if (!strcmp(string, "SetRect"))
    return COM_SETRECT;

  return COM_UNKNOWN;
}

//------------------------------------------------------------------------------------------------
// naparsuje radek
//------------------------------------------------------------------------------------------------
void Parse_Line(FILE * file, int *result, int res_size, char *comfile1,
  char *comfile2)
{
  char text[256], expression[256];
  int p = 0, r = 1;
  int i;

  agets(text, 256, file);

/*	if(feof(file))
		return;*/

  for (i = 0; i < res_size; i++)
    result[i] = -1;

  strcpy(comfile1, "");
  strcpy(comfile2, "");

  p = Find_Next_Expresion(text, p, expression);

  if (p != -1)
    result[0] = String2Command(expression);
  else
    result[0] = COM_NOCOMMAND;

  if (result[0] == COM_NOCOMMAND ||
    result[0] == COM_UNKNOWN || result[0] == COM_FLIP) {
    result[0] = -1;
    return;
  }

  while (p != -1) {
    p = Find_Next_Expresion(text, p, expression);

    switch (result[0]) {
      case COM_DRAW:
        if ((p != -1) && (r < res_size))
          result[r] = atoi(expression);
        break;
      case COM_SETRECT:
      case COM_BINDSOUND:
      case COM_RANDOMANIMATION:
      case COM_ONABOVE:
      case COM_ONCLICK:
      case COM_BINDEXITANIMATION:
      case COM_BINDANIMATION:
        if (p != -1) {
          if (r == 5)
            strcpy(comfile1, expression);
          else if (r == 6)
            strcpy(comfile2, expression);
          else if (r < res_size)
            result[r] = atoi(expression);
        }
        break;
      case COM_RUNANIMATION:
        if (p != -1) {
          if (r == 1)
            strcpy(comfile1, expression);
          else if (r == 2)
            strcpy(comfile2, expression);
        }
        break;
      case COM_CREATEBUTTON:
        if (p != -1) {
          if (r == 1 || r == 2)
            result[r] = atoi(expression);
          else if (r == 3)
            strcpy(comfile1, expression);
          else if (r == 4)
            strcpy(comfile2, expression);
        }
        break;
    }

    r++;
  }
}

void Parse_LineT(char *ftext, int *result, int res_size, char *comfile1,
  char *comfile2)
{
  char text[256], expression[256];
  int p = 0, r = 1;

  //fgets(text, 256, file);
  strcpy(text, ftext);

  p = Find_Next_Expresion(text, p, expression);

  if (p != -1)
    result[0] = String2Command(expression);
  else
    result[0] = COM_NOCOMMAND;

  if (result[0] == COM_NOCOMMAND ||
    result[0] == COM_UNKNOWN || result[0] == COM_FLIP)
    return;

  while (p != -1) {
    p = Find_Next_Expresion(text, p, expression);

    switch (result[0]) {
      case COM_DRAW:
        if ((p != -1) && (r < res_size))
          result[r] = atoi(expression);
        break;
      case COM_SETRECT:
      case COM_BINDSOUND:
      case COM_RANDOMANIMATION:
      case COM_ONABOVE:
      case COM_ONCLICK:
      case COM_BINDEXITANIMATION:
      case COM_BINDANIMATION:
        if (p != -1) {
          if (r == 5)
            strcpy(comfile1, expression);
          else if (r == 6)
            strcpy(comfile2, expression);
          else if (r < res_size)
            result[r] = atoi(expression);
        }
        break;
      case COM_RUNANIMATION:
        if (p != -1) {
          if (r == 1)
            strcpy(comfile1, expression);
          else if (r == 2)
            strcpy(comfile2, expression);
        }
        break;
      case COM_CREATEBUTTON:
        if (p != -1) {
          if (r == 1 || r == 2)
            result[r] = atoi(expression);
          else if (r == 3)
            strcpy(comfile1, expression);
          else if (r == 4)
            strcpy(comfile2, expression);
        }
        break;
    }

    r++;
  }
}

void Parse_RandomLine(char *expression, int *result, int res_size)
{
  char text[32];
  int c = 11, i = 0;
  int done = 0;

  while (!done) {
    result[c] = atoi(&expression[i]);

    i += strlen(itoa(result[c], text, 10));

    if (expression[i] != ';' || i >= (int) strlen(expression))
      break;
    i++;
    c++;

    if (c >= res_size - 1)
      break;
  }

  c++;

  for (i = c; c < res_size; c++)
    result[i] = -1;

  result[17] = c - 12;
}

//------------------------------------------------------------------------------------------------
// naparsuje radek animace
//------------------------------------------------------------------------------------------------
void Parse_AnimLine(FILE * file, int *result, int res_size)
{
  char text[256], expression[256];
  int p = 0, r = 0, i;

  for (i = 0; i < res_size; i++)
    result[i] = -1;

  agets(text, 256, file);

  while ((p != -1) && (r < res_size)) {
    p = Find_Next_Expresion(text, p, expression);

    /*if(p != -1)
       result[r] = atoi(expression); */

    if (p == -1)
      break;

    if (r < 11)
      result[r] = atoi(expression);
    else if (expression[0] == '?')
      Parse_RandomLine(&expression[1], result, res_size);

    r++;
  }
}

//------------------------------------------------------------------------------------------------
// naparsuje radek skriptu hry
//------------------------------------------------------------------------------------------------
void Parse_ScenarioLine(FILE * file, int *result, int res_size,
  char *comfile1, char *comfile2)
{
  char text[256], expression[256];
  int p = 0, r = 1;

  fgets(text, 256, file);

  p = Find_Next_Expresion(text, p, expression);

  if (p != -1)
    result[0] = String2Command(expression);
  else
    result[0] = COM_NOCOMMAND;

  while (p != -1) {
    p = Find_Next_Expresion(text, p, expression);

    switch (r) {
      case 1:
        strcpy(comfile1, expression);
        break;
      case 2:
        strcpy(comfile2, expression);
        break;
    }

    r++;
  }
}

void LoadMenuScript(const char *p_File_Name, CMD_LINE * res, int *lastcmd)
{
  FILE *file;

  pDataArchive->pActualNode = pDataArchive->pRootNode->pNextNode;

  file = aopen(pDataArchive, p_File_Name, "r");
  if (file) {
    while (!aeof(file)) {
      Parse_Line(file, res[*lastcmd].iParam, 6, res[*lastcmd].cParam[0],
        res[*lastcmd].cParam[1]);
      res[*lastcmd].uiTimerID = 0;
      res[*lastcmd].iLastfrm = 1;
      res[*lastcmd].iCounter = 0;
      (*lastcmd)++;
    }

    aclose(file);
  }

  (*lastcmd)--;
}

void LoadAnimationMenuScript(CMD_LINE * res, int i, int *lastanm)
{
  FILE *file;

  file = aopen(pDataArchive, res[i].cParam[0], "r");

  if (file) {
    while (!aeof(file)) {
      Parse_AnimLine(file, res[i].iAnim[*lastanm], 18);
      (*lastanm)++;
    }

    aclose(file);
  }
}
