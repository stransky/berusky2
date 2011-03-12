#include "..\\komat\\mss_on.h"
#include "square_cut.h"

void OhodnotVrchol(int x, int y, SQUARE *pOblast, OHODNOCENI *pOhodnot)
{
	if( x < pOblast->left)
		pOhodnot->horizontal = -1;
	else
		if( x > pOblast->right)
			pOhodnot->horizontal = 1;
		else
			pOhodnot->horizontal = 0;

	if( y < pOblast->top)
		pOhodnot->vertikal = -1;
	else
		if( y > pOblast->bottom)
			pOhodnot->vertikal = 1;
		else
			pOhodnot->vertikal = 0;

	return;
}

void KorigujSouracniciX(int *x, SQUARE *pOblast, OHODNOCENI *pOhodnot)
{
	if(pOhodnot->horizontal == -1)
		*x = pOblast->left;
	else
		if(pOhodnot->horizontal == 1)
			*x = pOblast->right;
}

void KorigujSouracniciY(int *y, SQUARE *pOblast, OHODNOCENI *pOhodnot)
{
	if(pOhodnot->vertikal == -1)
		*y = pOblast->top;
	else
		if(pOhodnot->vertikal == 1)
			*y = pOblast->bottom;
}

void OrezCtverec(SQUARE *pCtverec, SQUARE *pOblast)
{
	OHODNOCENI TopLeft;
	OHODNOCENI BottomRight;

	OhodnotVrchol(pCtverec->left, pCtverec->top, pOblast, &TopLeft);
	OhodnotVrchol(pCtverec->right, pCtverec->bottom, pOblast, &BottomRight);

	KorigujSouracniciX(&pCtverec->left, pOblast, &TopLeft);
	KorigujSouracniciX(&pCtverec->right, pOblast, &BottomRight);
	KorigujSouracniciY(&pCtverec->top, pOblast, &TopLeft);
	KorigujSouracniciY(&pCtverec->bottom, pOblast, &BottomRight);
}

char OhodnotCtverec(SQUARE *pCtverec, SQUARE *pOblast)
{
	OHODNOCENI TopLeft;
	OHODNOCENI BottomRight;
	OHODNOCENI TopRight;
	OHODNOCENI BottomLeft;
	
	OhodnotVrchol(pCtverec->left, pCtverec->top, pOblast, &TopLeft);
	OhodnotVrchol(pCtverec->right, pCtverec->top, pOblast, &TopRight);
	OhodnotVrchol(pCtverec->right, pCtverec->bottom, pOblast, &BottomRight);
	OhodnotVrchol(pCtverec->left, pCtverec->bottom, pOblast, &BottomLeft);

	//jeden a vice vrcholu v oblasti
	if((!TopLeft.horizontal && !TopLeft.vertikal) ||
	   (!BottomRight.horizontal && !BottomRight.vertikal) ||
	   (!TopRight.horizontal && !TopRight.vertikal) ||
	   (!BottomLeft.horizontal && !BottomLeft.vertikal))
			return 1;
	else
	//horizontalni protnuti oblasti
	if(!TopLeft.vertikal && !TopRight.vertikal &&
	   !BottomRight.vertikal && !BottomLeft.vertikal &&
		TopLeft.horizontal == -1 && BottomLeft.horizontal == -1 &&
		TopRight.horizontal == 1 && BottomRight.horizontal == 1)
			return 1;
	else
	//vertikalni protnuti oblasti
	if(TopLeft.vertikal == -1 && TopRight.vertikal == -1 &&
	   BottomRight.vertikal == 1&& BottomLeft.vertikal == 1&&
	   !TopLeft.horizontal && !BottomLeft.horizontal &&
	   !TopRight.horizontal && !BottomRight.horizontal) 
			return 1;
	else
	//oblast je ve ctverci
	if(TopLeft.horizontal == -1 && TopLeft.vertikal == -1 &&
	   TopRight.horizontal == 1 && TopRight.vertikal == -1 &&
	   BottomLeft.horizontal == -1 && BottomLeft.vertikal == 1 &&
	   BottomRight.horizontal == 1 && BottomRight.vertikal == 1)
			return 1;
	else
	//oblast prekryva kurzor zhora
	if(TopLeft.horizontal == -1 && TopLeft.vertikal == -1 &&
	   TopRight.horizontal == 1 && TopRight.vertikal == -1 &&
	   BottomLeft.horizontal == -1 && !BottomLeft.vertikal &&
	   BottomRight.horizontal == 1 && !BottomRight.vertikal)
			return 1;
	else
	//oblast prekryva kurzor ze spoda
	if(TopLeft.horizontal == -1 && !TopLeft.vertikal &&
	   TopRight.horizontal == 1 && !TopRight.vertikal &&
	   BottomLeft.horizontal == -1 && BottomLeft.vertikal == 1 &&
	   BottomRight.horizontal == 1 && BottomRight.vertikal == 1)
			return 1;
	else
	//oblast prekryva kurzor z leva
	if(TopLeft.horizontal == -1 && TopLeft.vertikal == -1 &&
	   !TopRight.horizontal && TopRight.vertikal == -1 &&
	   BottomLeft.horizontal == -1 && BottomLeft.vertikal == 1 &&
	   !BottomRight.horizontal && BottomRight.vertikal == 1)
			return 1;
	else
	//oblast prekryva kurzor z prava
	if(!TopLeft.horizontal && TopLeft.vertikal == -1 &&
	   TopRight.horizontal == 1 && TopRight.vertikal == -1 &&
	   !BottomLeft.horizontal && BottomLeft.vertikal == 1 &&
	   BottomRight.horizontal == 1 && BottomRight.vertikal == 1)
			return 1;

		return 0;
}