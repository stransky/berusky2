#ifndef __GRAMMAR_
#define __GRAMMAR_


typedef struct
{
	DWORD	Value;
	char	Type;
} VALUE;

typedef struct
{
	int		iCommand;
	char	cCommand[64];
	VALUE	Parametr[16];
	int		LastParam;
} COMMAND;

typedef struct
{
	COMMAND		Mask[256];
	int			LastMask;
} GRAMMAR;

	char gr_Load_Grammar(char *pFile, GRAMMAR *pGr);
	void gr_Add_Mask(char *pMask, GRAMMAR *pGr);

#endif