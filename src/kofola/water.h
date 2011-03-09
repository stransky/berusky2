#ifndef __WATER_
#define __WATER_

#define WATER_BOUNDARY_METHOD_RANDOM			1
#define WATER_BOUNDARY_METHOD_MAX_SIZE			2
#define WATER_BOUNDARY_METHOD_SIZE_HEURISTICS	3

typedef struct _WATER_BOUNDARY
{
	float					Top;
	float					Bottom;
	float					Left;
	float					Right;
	float					Height;
	float					AspectRatio;
	float					Size;
	struct _WATER_BOUNDARY	*pNext;
} WATER_BOUNDARY;

typedef struct
{
	WATER_BOUNDARY	*pFirst;
	WATER_BOUNDARY	*pLast;
	int				iCount;
} WATER_STRUCT;

	void w_Create_Water_Boundaries(void);
	void w_Release_Water_Boundaries(void);
	void w_Choose_Best_Boundary(WATER_BOUNDARY *pBoundary, float fx, float fy, float fz, int Method);

#endif