//------------------------------------------------------------------------------------------------
// 0.0.1
//------------------------------------------------------------------------------------------------
#include <math.h>

//------------------------------------------------------------------------------------------------
// Make normal 3D vector (axe Y is static)
//------------------------------------------------------------------------------------------------
void Normal_Vector(float *Vector, float *vResult)
{
  vResult[0] = -1 * Vector[2];
  vResult[1] = Vector[1];
  vResult[2] = Vector[0];
}

//------------------------------------------------------------------------------------------------
// Calculate inverse vector
//------------------------------------------------------------------------------------------------
void Inverse_Vector(float *Vector, float *vResult, unsigned int uiDimension)
{
  unsigned int i;

  for (i = 0; i < uiDimension; i++)
    vResult[i] = Vector[i] * -1;
}


//------------------------------------------------------------------------------------------------
// Calculate length of 3D vector
//------------------------------------------------------------------------------------------------
float Vector_Length(float *Vector)
{
  return (float) sqrt(Vector[0] * Vector[0] + Vector[1] * Vector[1] +
    Vector[2] * Vector[2]);
}

//------------------------------------------------------------------------------------------------
// Normalize 3D vector
//------------------------------------------------------------------------------------------------
float Normalize_Vector(float *Vector)
{

  float size = Vector_Length(Vector);

  if (size == 0)
    return 0.0f;

  Vector[0] /= size;
  Vector[1] /= size;
  Vector[2] /= size;

  return size;
}

//------------------------------------------------------------------------------------------------
// Add two 3D vectors
//------------------------------------------------------------------------------------------------
void Add_Vector(float *Vector1, float *Vector2, float *vResult)
{
  vResult[0] = Vector1[0] + Vector2[0];
  vResult[1] = Vector1[1] + Vector2[1];
  vResult[2] = Vector1[2] + Vector2[2];
}

//------------------------------------------------------------------------------------------------
// Sub two 3D vectors
//------------------------------------------------------------------------------------------------
void Sub_Vector(float *Vector1, float *Vector2, float *vResult)
{
  vResult[0] = Vector1[0] - Vector2[0];
  vResult[1] = Vector1[1] - Vector2[1];
  vResult[2] = Vector1[2] - Vector2[2];
}

//------------------------------------------------------------------------------------------------
// Multiple 3D vector with constant
//------------------------------------------------------------------------------------------------
void Multiple_Vector(float *Vector, float Const)
{
  Vector[0] *= Const;
  Vector[1] *= Const;
  Vector[2] *= Const;
}

//------------------------------------------------------------------------------------------------
// Make 3D vector
//------------------------------------------------------------------------------------------------
void Make_Vector(float *Point1, float *Point2, float *vResult)
{
  vResult[0] = Point2[0] - Point1[0];
  vResult[1] = Point2[1] - Point1[1];
  vResult[2] = Point2[2] - Point1[2];
}
