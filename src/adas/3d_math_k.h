#ifndef __3D_MATHEMATIC_
#define __3D_MATHEMATIC_

#ifdef __cplusplus
	extern "C" {
#endif
 
	// Calculate lungth of vector
	float Vector_Length(float *Vector);

	// Normalize vector
	float Normalize_Vector(float *Vector);

	// Add two vectors
	void Add_Vector(float *Vector1, float *Vector2, float *vResult);
	
	// Sub two vectors
	void Sub_Vector(float *Vector1, float *Vector2, float *vResult);

	// Multiple vector with constant
	void Multiple_Vector(float *Vector, float Const);

	// Make vectors
	void Make_Vector(float *Vector1, float *Vector2, float *vResult);

#ifdef __cplusplus
	}
#endif

#endif
