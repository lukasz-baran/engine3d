
#include "math3d.hpp"

///>====================================----------------------------
///
///	i m p l e m e n t a c j a
///
///>====================================----------------------------

void CVector3::CalcNormal(const CVector3 &v1, const CVector3 &v2, const CVector3 &v3)
{
	/// taken from Acid's helium engine:
	float	ux = v2.x - v1.x, vx = v3.x - v1.x;
	float	uy = v2.y - v1.y, vy = v3.y - v1.y;
	float	uz = v2.z - v1.z, vz = v3.z - v1.z;
	x = uy*vz - uz*vy;
	y = uz*vx - ux*vz;
	z = ux*vy - uy*vx;
	Normalize();
}

void CVector3::Normalize()
{
	if (!IsZero()) {
		register double len = sqrt(1.0/(x*x + y*y + z*z));
		x = (float)(x * len);
		y = (float)(y * len);
		z = (float)(z * len);
	} else
		Zeros();
}

ostream& operator<<(ostream &_out, CVector3 &_vec)
{
	_out << "[" << _vec.x << ", " << _vec.y << ", " << _vec.z << "]";
	return _out;
}
