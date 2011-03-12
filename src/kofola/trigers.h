#ifndef __TRIGERS_
#define __TRIGERS_

#include "Object.h"
#include "grammar.h"

typedef struct
{
	COMMAND	command[256];		//prikazy
	int		lastcommand;		//posledni prikaz
	int		actualcommnad;		//aktualne provadeny prekaz
} GAME_TRIGER;

typedef struct
{
	char			StrTable[256][64];	//tabulka retezcu
	int				LastStr;			//posledni string
	GAME_TRIGER		*pTriger;			//ukazatel na pole trigru
	int				sizeofT;			//pocet polozek v poli
} TRIGER_STRUCTURE;

	char trig_Load_Trigers(char *pLevel, char *pFile, TRIGER_STRUCTURE *pTStruct, GRAMMAR *pGr);
	char trig_Load_Triger(char *pFile, GAME_TRIGER *pTriger, GRAMMAR *pGr, TRIGER_STRUCTURE *pTStruct);
	char trig_Check_ifPos_Trigers(TRIGER_STRUCTURE *pTStruct, LEVELINFO *pLevel);
	int trig_Find_Triger(TRIGER_STRUCTURE *pTStruct, LEVELINFO *pLevel);
	void trig_Do_Triger_Command(COMMAND *pCommand, int *flag);
	void trig_Parse_Line(char *pLine, COMMAND *pCommand, GAME_TRIGER *pTriger, GRAMMAR *pGr, TRIGER_STRUCTURE *pTStruct);
	void trig_Parse_LineU(WCHAR *pLine, COMMAND *pCommand, GAME_TRIGER *pTriger, GRAMMAR *pGr, TRIGER_STRUCTURE *pTStruct);

#endif
