
#include "math3d.hpp"

///>====================================----------------------------
///
///	i m p l e m e n t a c j a
///
///>====================================----------------------------

void CVector4::Normalize()
{
	if (!IsZero()) {
		register double len = sqrt(1.0/(x*x + y*y + z*z + w*w));
		x = (float)(x * len);
		y = (float)(y * len);
		z = (float)(z * len);
		w = (float)(w * len);
	} else
		Zeros();
}

ostream& operator<<(ostream &_out, CVector4 &_vec)
{
	_out<<"["<<_vec.x<<", "<<_vec.y<<", "<<_vec.z<<", "<<_vec.w<<"]";
	return _out;
}
