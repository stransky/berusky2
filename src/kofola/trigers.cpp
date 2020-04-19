#include <stdio.h>
#include <stdlib.h>
#include "3d_all.h"
#include "Berusky3d_kofola_interface.h"
#include "trigers.h"
#include "menu_script.h"
#include "game_logic.h"

extern char pGDataDir[MAX_FILENAME];

int Find_Next_ExpresionU(WCHAR * p_Command, int start, WCHAR * p_Expresion);
int trig_Commandtoi(char *pCommand, GRAMMAR * pGr, int *iMask)
{
  int i;

  for (i = 0; i < pGr->LastMask; i++)
    if (!strcmp(pCommand, pGr->Mask[i].cCommand)) {
      *iMask = i;
      return pGr->Mask[i].iCommand;
    }

  return -1;
}

void trig_Parse_Line(char *pLine, COMMAND * pCommand, GAME_TRIGER * pTriger,
  GRAMMAR * pGr, TRIGER_STRUCTURE * pTStruct)
{
  char Expression[64];
  int cursor = 0, par = 0, iMask;
//  int cursoru = 1;
  float fValue;

  cursor = Find_Next_Expresion(pLine, cursor, Expression);
  strcpy(pCommand->cCommand, Expression);
  pCommand->iCommand = trig_Commandtoi(Expression, pGr, &iMask);

  if (pCommand->iCommand == -1)
    return;

  while (cursor != -1) {
    cursor = Find_Next_Expresion(pLine, cursor, Expression);

//    cursoru = cursor + 1;

    if (cursor != -1) {
      switch (pGr->Mask[iMask].Parametr[par].Type) {
          //int
        case 0:
          pCommand->Parametr[par].Type = 0;
          pCommand->Parametr[par].Value = atoi(Expression);
          break;
          //float
        case 1:
          pCommand->Parametr[par].Type = 1;
          fValue = (float) atof(Expression);
          memcpy((void *) &pCommand->Parametr[par].Value, (void *) &fValue,
            sizeof(float));
          break;
          //string
        case 2:
          pCommand->Parametr[par].Type = 2;
          pCommand->Parametr[par].Value = pTStruct->LastStr;
          strcpy(pTStruct->StrTable[pTStruct->LastStr], Expression);
          pTStruct->LastStr++;
          break;
      }
      par++;
    }
  }

  pCommand->LastParam = par;
  pTriger->lastcommand++;
}

void Uni2Char(WCHAR * cUni, char *cText, int ctsize)
{
  int cu = wcslen(cUni), i;

  ZeroMemory(cText, ctsize);

  for (i = 0; i < cu; i++)
    cText[i] = (char) cUni[i];
}

void trig_Parse_LineU(WCHAR * pLine, COMMAND * pCommand,
                      GAME_TRIGER * pTriger, GRAMMAR * pGr, 
                      TRIGER_STRUCTURE * pTStruct)
{
  WCHAR Expression[64];
  char tmp[64];
  int cursor = 0, par = 0, iMask;
  float fValue;

  cursor = Find_Next_ExpresionU(pLine, cursor, Expression);
  if (cursor == -1)
    return;
  Uni2Char(Expression, pCommand->cCommand, 64);

  pCommand->iCommand = trig_Commandtoi(pCommand->cCommand, pGr, &iMask);

  if (pCommand->iCommand == -1)
    return;

  while (cursor != -1) {
    cursor = Find_Next_ExpresionU(pLine, cursor, Expression);

    if (cursor != -1) {
      switch (pGr->Mask[iMask].Parametr[par].Type) {
          //int
        case 0:
          pCommand->Parametr[par].Type = 0;
          Uni2Char(Expression, tmp, 64);
          pCommand->Parametr[par].Value = atoi(tmp);
          break;
          //float
        case 1:
          pCommand->Parametr[par].Type = 1;
          Uni2Char(Expression, tmp, 64);
          fValue = (float) atof(tmp);
          memcpy((void *) &pCommand->Parametr[par].Value, (void *) &fValue,
            sizeof(float));
          break;
          //string
        case 2:
          pCommand->Parametr[par].Type = 2;
          pCommand->Parametr[par].Value = pTStruct->LastStr;
          wcscpy((WCHAR *) pTStruct->StrTable[pTStruct->LastStr], Expression);
          pTStruct->LastStr++;
          break;
          //Uni Code
        case 3:
          {
            pCommand->Parametr[par].Type = 3;
            ZeroMemory((void *) &pCommand->Parametr[par].Value,
              sizeof(DWORD));

            if (Expression[0] == MAKEWORD('\\', 0))
              memcpy((void *) &pCommand->Parametr[par].Value,
                (void *) &Expression[1], 1 * sizeof(WCHAR));
            else
              memcpy((void *) &pCommand->Parametr[par].Value,
                (void *) Expression, 1 * sizeof(WCHAR));
          }
          break;
      }
      par++;
    }
  }

  pCommand->LastParam = par;
  pTriger->lastcommand++;
}

char trig_Load_Triger(char *pFile, GAME_TRIGER * pTriger, GRAMMAR * pGr,
  TRIGER_STRUCTURE * pTStruct)
{
  FILE *file;
  char text[MAX_FILENAME];

  pTriger->lastcommand = 0;

  construct_path(text, MAX_FILENAME, 2, pGDataDir, pFile);
  file = fopen(text, "r");

  if (!file)
    return 0;

  while (!feof(file)) {
    if(fgets(text, 256, file)) {
      trig_Parse_Line(text, &pTriger->command[pTriger->lastcommand], pTriger, 
                      pGr, pTStruct);
      text[0] = '\0';
    }
  }

  fclose(file);
  return 1;
}

char trig_Load_Trigers(char *pLevel, char *pFile, TRIGER_STRUCTURE * pTStruct,
                       GRAMMAR * pGr)
{
  FILE *file;
  char text[MAX_FILENAME];
  int i;

  pTStruct->pTriger = NULL;
  pTStruct->sizeofT = 0;

  if (chdir(GAME_LEVEL_DIR))
    return 0;

  strcpy(text, pLevel);
  text[strlen(pLevel) - 4] = '\0'; //TODO - newline?
  if (chdir(text))
    return 0;

  file = fopen(pFile, "r");
  if (!file)
    return 0;

  if (chdir(DATA_DIR) ||
      fgets(text, MAX_FILENAME, file) == NULL) {
    fclose(file);
    return 0;
  }

  pTStruct->sizeofT = atoi(text);

  pTStruct->pTriger =
    (GAME_TRIGER *) malloc(pTStruct->sizeofT * sizeof(GAME_TRIGER));

  if (!pTStruct->pTriger) {
    pTStruct->sizeofT = 0;
    fclose(file);
    return 0;
  }

  pTStruct->LastStr = 0;

  for (i = 0; i < pTStruct->sizeofT; i++) {
    if (fgets(text, MAX_FILENAME, file) == NULL) {
      pTStruct->pTriger = NULL;
      pTStruct->sizeofT = 0;
      fclose(file);
      return 0;
    }
    newline_cut(text);
    if (!trig_Load_Triger(text, &pTStruct->pTriger[i], pGr, pTStruct))
      kprintf(1, "Unable to load triger %s", text);
  }

  fclose(file);
  return 1;
}

char trig_cmp(int i, char *c)
{
  int n;

  if (c[0] == '?')
    return 1;
  else {
    n = atoi(c);
    if (n == i)
      return 1;
    else
      return 0;
  }
}

char trig_Check_ifPos_Trigers(TRIGER_STRUCTURE * pTStruct, LEVELINFO * pLevel)
{
  int i, r;
  COMMAND *pCom;

  for (i = 0; i < pTStruct->sizeofT; i++)
    if (pTStruct->pTriger[i].command[0].iCommand == 1) {
      pCom = &pTStruct->pTriger[i].command[0];

      gl_Logical2Real(pCom->Parametr[2].Value, pCom->Parametr[3].Value,
        pCom->Parametr[4].Value, &r, pLevel);

      if (pLevel->Level[r])
        if (trig_cmp(pLevel->Level[r]->p_Object->Class,
            pTStruct->StrTable[pCom->Parametr[0].Value]) &&
          trig_cmp(pLevel->Level[r]->p_Object->SubClass,
            pTStruct->StrTable[pCom->Parametr[1].Value]))
          return 1;
    }

  return 0;
}

int trig_Find_Triger(TRIGER_STRUCTURE * pTStruct, LEVELINFO * pLevel)
{
  int i, r;
  COMMAND *pCom;

  for (i = 0; i < pTStruct->sizeofT; i++)
    switch (pTStruct->pTriger[i].command[0].iCommand) {
      case 1:
        {
          pCom = &pTStruct->pTriger[i].command[0];

          gl_Logical2Real(pCom->Parametr[2].Value, pCom->Parametr[3].Value,
            pCom->Parametr[4].Value, &r, pLevel);

          if (pLevel->Level[r])
            if (trig_cmp(pLevel->Level[r]->p_Object->Class,
                pTStruct->StrTable[pCom->Parametr[0].Value]) &&
              trig_cmp(pLevel->Level[r]->p_Object->SubClass,
                pTStruct->StrTable[pCom->Parametr[1].Value]))
              return i;
        }
        break;
    }

  return -1;
}

void trig_Do_MoveCameraTo(COMMAND * pCommand, int *p_flag)
{
  BOD p_p, p_t;

  memcpy((void *) &p_p.x, (void *) &pCommand->Parametr[0].Value,
    sizeof(float));
  memcpy((void *) &p_p.y, (void *) &pCommand->Parametr[1].Value,
    sizeof(float));
  memcpy((void *) &p_p.z, (void *) &pCommand->Parametr[2].Value,
    sizeof(float));
  memcpy((void *) &p_t.x, (void *) &pCommand->Parametr[3].Value,
    sizeof(float));
  memcpy((void *) &p_t.y, (void *) &pCommand->Parametr[4].Value,
    sizeof(float));
  memcpy((void *) &p_t.z, (void *) &pCommand->Parametr[5].Value,
    sizeof(float));

  kam_3ds_anim(&p_p, &p_t, 0.0f, p_flag, 0, 10, 1.0f);
}

void trig_Do_Triger_Command(COMMAND * pCommand, int *p_flag)
{
  switch (pCommand->iCommand) {
    case 2:
      trig_Do_MoveCameraTo(pCommand, p_flag);
      break;
  }
}
