
#ifndef __math3d_hpp__
#define __math3d_hpp__

#define		MATH3D_PI	3.14159265358979324f
#define		MATH3D_PI_2	(MATH3D_PI/2)
#define		MATH3D_2PI	(MATH3D_PI*2)
#define		MATH3D_EPS	0.001f
#define		MATH3D_EPSILON	(1e-8)


#define		TO_RADIANS(x)	((x)*MATH3D_PI/180.0f)
#define		MATH3D_SQR(x)	((x)*(x))

#include <math.h>
#include <memory.h>
#include <iostream.h>

class CVector3;
class CVector4;
class CMatrix4x4;
class CQuaternion;

#include "vector3.hpp"
#include "vector4.hpp"
#include "quaternion.hpp"
#include "matrix4x4.hpp"

#endif
