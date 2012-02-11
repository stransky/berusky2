#include <math.h>

//------------------------------------------------------------------------------------------------
// Calculate lungth of vector
//------------------------------------------------------------------------------------------------
float Vector_Length(float *Vector)
{
	return (float)sqrt(Vector[0]*Vector[0] + Vector[1]*Vector[1] + Vector[2]*Vector[2]);
}

//------------------------------------------------------------------------------------------------
// Normalize vector
//------------------------------------------------------------------------------------------------
float Normalize_Vector(float *Vector)
{

	float size = Vector_Length(Vector);

	if(size == 0) return 0.0f;

	Vector[0] /= size;
	Vector[1] /= size;
	Vector[2] /= size;
 
	return size;
}

//------------------------------------------------------------------------------------------------
// Add two vectors
//------------------------------------------------------------------------------------------------
void Add_Vector(float *Vector1, float *Vector2, float *vResult)
{
	vResult[0] = Vector1[0] + Vector2[0];
	vResult[1] = Vector1[1] + Vector2[1];
	vResult[2] = Vector1[2] + Vector2[2];
}

//------------------------------------------------------------------------------------------------
// Sub two vectors
//------------------------------------------------------------------------------------------------
void Sub_Vector(float *Vector1, float *Vector2, float *vResult)
{
	vResult[0] = Vector1[0] - Vector2[0];
	vResult[1] = Vector1[1] - Vector2[1];
	vResult[2] = Vector1[2] - Vector2[2];
}

//------------------------------------------------------------------------------------------------
// Multiple vector with constant
//------------------------------------------------------------------------------------------------
void Multiple_Vector(float *Vector, float Const)
{
	Vector[0] *= Const;
	Vector[1] *= Const;
	Vector[2] *= Const;
}

//------------------------------------------------------------------------------------------------
// Sub two vectors
//------------------------------------------------------------------------------------------------
void Make_Vector(float *Vector1, float *Vector2, float *vResult)
{
	vResult[0] = Vector2[0] - Vector1[0];
	vResult[1] = Vector2[1] - Vector1[1];
	vResult[2] = Vector2[2] - Vector1[2];
}