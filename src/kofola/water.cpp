#include "game_logic.h"
#include "water.h"
#include "Berusky3d_kofola_interface.h"

		WATER_STRUCT	wsWater;
extern	LEVELINFO		Level;

int w_Check_Oclussion(int x, int y, int z)
{
	long real;
	int xs = x-1;
	int ys = y-1;
	int i, j, c = 0;

	if(ys < 0 || ys + 2 > Level.Size[1] ||
	   xs < 0 || xs + 2 > Level.Size[0])
	   return 1;

	for(i=xs;i<xs+3;i++)	
		for(j=ys;j<ys+3;j++)
		{
			gl_Logical2Real(i, j, z+1, &real, &Level);

			if(Level.Level[real])
				if(Level.Level[real]->p_Object->Class == 2)
					c++;
		}

	if(c > 1)
		return 1;
	else
		return 0;
}

int w_Check_Oclussion_Vertical(int x, int y, int z)
{
	long real;
	int ys = y-1;
	int j, c = 0;

	if(ys < 0 || ys + 3 > Level.Size[1])
	   return 1;

	for(j=ys;j<ys+3;j++)
	{
		gl_Logical2Real(x, j, z+1, &real, &Level);

		if(Level.Level[real])
			if(Level.Level[real]->p_Object->Class == 2)
				c++;
	}

	if(c > 1)
		return 1;
	else
		return 0;
}

int w_Check_Oclussion_Horizontal(int x, int y, int z)
{
	long real;
	int xs = x-1;
	int i, c = 0;

	if(xs < 0 || xs + 3 > Level.Size[0])
	   return 1;

	for(i=xs;i<xs+3;i++)	
	{
		gl_Logical2Real(i, y, z+1, &real, &Level);

		if(Level.Level[real])
			if(Level.Level[real]->p_Object->Class == 2)
				c++;
	}

	if(c > 1)
		return 1;
	else
		return 0;
}

int	w_Find_Top(int x, int y, int z)
{
	long real;
	int i;

	for(i=y;i<Level.Size[1];i++)
	{
		gl_Logical2Real(x, i, z+1, &real, &Level);

		if(Level.Level[real])
			if(Level.Level[real]->p_Object->Class == 2 && w_Check_Oclussion(x, i, z))
				return i-1;
	}

	return Level.Size[1] - 1;
}

int	w_Find_Bottom(int x, int y, int z)
{
	long real;
	int i;

	for(i=y;i>=0;i--)
	{
		gl_Logical2Real(x, i, z+1, &real, &Level);

		if(Level.Level[real])
			if(Level.Level[real]->p_Object->Class == 2 && w_Check_Oclussion(x, i, z))
				return i+1;
	}

	return 0;
}

int	w_Find_Left(int x, int y, int z)
{
	long real;
	int i;

	for(i=x;i>=0;i--)
	{
		gl_Logical2Real(i, y, z+1, &real, &Level);

		if(Level.Level[real])
			if(Level.Level[real]->p_Object->Class == 2 && w_Check_Oclussion(i, y, z))
				return i+1;
	}

	return 0;
}

int	w_Find_Right(int x, int y, int z)
{
	long real;
	int i;

	for(i=x;i<Level.Size[0];i++)
	{
		gl_Logical2Real(i, y, z+1, &real, &Level);

		if(Level.Level[real])
			if(Level.Level[real]->p_Object->Class == 2 && w_Check_Oclussion(i, y, z))
				return i-1;
	}

	return Level.Size[0] - 1;
}

float w_Count_Size(WATER_BOUNDARY *pBoundary)
{
	return (pBoundary->Top - pBoundary->Bottom) * (pBoundary->Right - pBoundary->Left);
}

float w_Count_Aspect_Ratio(WATER_BOUNDARY *pBoundary)
{
	float a,b;

	a = pBoundary->Top - pBoundary->Bottom;
	b = pBoundary->Right - pBoundary->Left;

	if(a > b)
		return b / a;
	else
		return a / b;
}

void w_Add_Boundary(int Top, int Left, int Bottom, int Right, int Height)
{
	float			fTop, fLeft, fBottom, fRight, fHeight;
	BOD				b;
	WATER_BOUNDARY *pBoundary = wsWater.pFirst;

	if(Left == -1)
		return;

	kom_get_fyz_souradnice(Left,0, Top, &b);
	fTop = b.z + 1;
	fLeft = b.x - 1;

	kom_get_fyz_souradnice(Right, Height, Bottom, &b);
	fBottom = b.z - 1;
	fRight = b.x + 1;
	fHeight = b.y;

	while(pBoundary)
	{
		if(pBoundary->Top == fTop &&
		   pBoundary->Left == fLeft &&
		   pBoundary->Bottom == fBottom &&
		   pBoundary->Right == fRight &&
		   pBoundary->Height == fHeight)
			return;

		pBoundary = pBoundary->pNext;
	}

	pBoundary = (WATER_BOUNDARY *) malloc(sizeof(WATER_BOUNDARY));

	
	pBoundary->Top = fTop;
	pBoundary->Left = fLeft;
	pBoundary->Bottom = fBottom;
	pBoundary->Right = fRight;
	
	pBoundary->Height = fHeight;
	pBoundary->pNext = NULL;

	pBoundary->Size = w_Count_Size(pBoundary);
	pBoundary->AspectRatio = w_Count_Aspect_Ratio(pBoundary);

	wsWater.iCount++;

	if(!wsWater.pFirst)
	{
		wsWater.pFirst = pBoundary;
		wsWater.pLast = pBoundary;
		return;
	}

	wsWater.pLast->pNext = pBoundary;
	wsWater.pLast = pBoundary;
}

void w_Construct_Boundary(int x, int y, int z)
{
	int Top, 
		Left = -1, 
		Bottom, 
		Right = Level.Size[0] + 1,
		nLeft,
		nRight;
	int	i;

	Top = w_Find_Top(x, y, z);
	Bottom = w_Find_Bottom(x, y, z);

	for(i=Bottom;i<=Top;i++)
	{
		nLeft = w_Find_Left(x, i, z);
		nRight = w_Find_Right(x, i, z);

		if(nLeft > Left)
			Left = nLeft;

		if(nRight < Right)
			Right = nRight;
	}

	w_Add_Boundary(Top, Left, Bottom, Right, z);
}

void w_Create_Water_Boundaries(void)
{
	long real;
	int i, j, k;

	wsWater.iCount = 0;
	wsWater.pFirst = NULL;
	wsWater.pLast = NULL;

	for(i=0;i<Level.Size[2];i+=2)
		for(j=0;j<Level.Size[0];j++)
			for(k=0;k<Level.Size[1];k++)
			{
				gl_Logical2Real(j, k, i, &real, &Level);

				if(Level.Level[real])
					if(Level.Level[real]->p_Object->Class == 12)
						w_Construct_Boundary(j, k, i);
			}

	return;
}

void w_Release_Water_Boundaries(void)
{
	WATER_BOUNDARY *pBoundary = wsWater.pFirst;
	WATER_BOUNDARY *pLBoundary;

	while(pBoundary)
	{
		pLBoundary = pBoundary;
		pBoundary = pBoundary->pNext;

		kprintf(1, "free((void *) pLBoundary)");

		free((void *) pLBoundary);
	}

	wsWater.iCount = 0;
	wsWater.pFirst = NULL;
	wsWater.pLast = NULL;
}

void w_Choose_Random_Boundary(WATER_BOUNDARY *pBoundary, float fx, float fy, float fz)
{
	int				iBoundary;
	int				iCount = 0;
	WATER_BOUNDARY *pTBoundary = wsWater.pFirst;

	while(pTBoundary)
	{
		if(fx >= pTBoundary->Left && fx <= pTBoundary->Right &&
		   fy >= pTBoundary->Bottom && fy <= pTBoundary->Top &&
		   fz >= pTBoundary->Height - 0.25f && fz <= pTBoundary->Height + 0.25f)
			iCount++;

		pTBoundary = pTBoundary->pNext;
	}

	if(!iCount)
		return;

	iBoundary = rand()%iCount;

	iCount = 0;
	pTBoundary = wsWater.pFirst;

	while(pTBoundary)
	{
		if(iCount == iBoundary)
		{
			pBoundary->Bottom = pTBoundary->Bottom;
			pBoundary->Top = pTBoundary->Top;
			pBoundary->Right = pTBoundary->Right;
			pBoundary->Left = pTBoundary->Left;
			return;
		}

		iCount++;
		pTBoundary = pTBoundary->pNext;
	}
}

void w_Choose_Max_Size_Boundary(WATER_BOUNDARY *pBoundary, float fx, float fy, float fz)
{
	int				iBoundary;
	int				iCount = 0;
	float			max_size = 0;
	WATER_BOUNDARY *pTBoundary = wsWater.pFirst;

	while(pTBoundary)
	{
		if(fx >= pTBoundary->Left && fx <= pTBoundary->Right &&
		   fy >= pTBoundary->Bottom && fy <= pTBoundary->Top &&
		   fz >= pTBoundary->Height - 0.25f && fz <= pTBoundary->Height + 0.25f &&
		   pTBoundary->Size > max_size)
		{
			iBoundary = iCount;
			max_size = pTBoundary->Size;
		}

		iCount++;
		pTBoundary = pTBoundary->pNext;
	}

	if(!iCount)
		return;

	iCount = 0;
	pTBoundary = wsWater.pFirst;

	while(pTBoundary)
	{
		if(iCount == iBoundary)
		{
			pBoundary->Bottom = pTBoundary->Bottom;
			pBoundary->Top = pTBoundary->Top;
			pBoundary->Right = pTBoundary->Right;
			pBoundary->Left = pTBoundary->Left;
			return;
		}

		iCount++;
		pTBoundary = pTBoundary->pNext;
	}
}

void w_Choose_Size_Heuristics_Boundary(WATER_BOUNDARY *pBoundary, float fx, float fy, float fz)
{
	int				iBoundary = -1;
	int				iCount = 0;
	float			max_size = 0;
	WATER_BOUNDARY *pTBoundary = wsWater.pFirst;

	while(pTBoundary)
	{
		if(fx >= pTBoundary->Left && fx <= pTBoundary->Right &&
		   fy >= pTBoundary->Bottom && fy <= pTBoundary->Top &&
		   fz >= pTBoundary->Height - 0.25f && fz <= pTBoundary->Height + 0.25f &&
		   pTBoundary->Size * pTBoundary->AspectRatio > max_size)
		{
			iBoundary = iCount;
			max_size = pTBoundary->Size * pTBoundary->AspectRatio;
		}

		iCount++;
		pTBoundary = pTBoundary->pNext;
	}

	if(!iCount)
		return;

	iCount = 0;
	pTBoundary = wsWater.pFirst;

	while(pTBoundary)
	{
		if(iCount == iBoundary)
		{
			pBoundary->Bottom = pTBoundary->Bottom;
			pBoundary->Top = pTBoundary->Top;
			pBoundary->Right = pTBoundary->Right;
			pBoundary->Left = pTBoundary->Left;
			return;
		}

		iCount++;
		pTBoundary = pTBoundary->pNext;
	}
}

void w_Choose_Best_Boundary(WATER_BOUNDARY *pBoundary, float fx, float fy, float fz, int Method)
{
	switch(Method)
	{
		case WATER_BOUNDARY_METHOD_RANDOM:
			w_Choose_Random_Boundary(pBoundary, fx, fy, fz);
			break;
		case WATER_BOUNDARY_METHOD_MAX_SIZE:
			w_Choose_Max_Size_Boundary(pBoundary, fx, fy, fz);
			break;
		case WATER_BOUNDARY_METHOD_SIZE_HEURISTICS:
			w_Choose_Size_Heuristics_Boundary(pBoundary, fx, fy, fz);
			break;
	}
}
