#ifndef __SQUARE_CUT_
#define __SQUARE_CUT_

typedef struct
{
	int top;
	int	bottom;
	int left;
	int right;
} SQUARE;

typedef struct
{
	char vertikal;
	char horizontal;
} OHODNOCENI;

void OrezCtverec(SQUARE *pCtverec, SQUARE *pOblast);
char OhodnotCtverec(SQUARE *pCtverec, SQUARE *pOblast);

#endif