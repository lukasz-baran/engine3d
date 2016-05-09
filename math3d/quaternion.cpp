
#include "math3d.hpp"

///>====================================----------------------------
///
///	i m p l e m e n t a c j a
///
///>====================================----------------------------

void CQuaternion::FromAxisAngle(const CVector3 &axis, const float angle)
{
	float		angle_2 = 0.5f * angle;
	float		_sin = (float)sin(angle_2);
	this->CQuaternion::CQuaternion((float)cos(angle_2), _sin*axis.x, _sin*axis.y, _sin*axis.z);
}

////////////////////////////////////////////////////////////////////

CQuaternion Exp(const CQuaternion &quat)
{
	CQuaternion		out;
	float			ang = sqrtf(quat.x*quat.x + quat.y*quat.y + quat.z*quat.z);
	out.w = cosf(ang);
	if (ang < MATH3D_EPS)
		ang = 1.0f;
	else
		ang = sinf(ang)/ang;
	out.x = quat.x*ang;
	out.y = quat.y*ang;
	out.z = quat.z*ang;
	return out;
}

////////////////////////////////////////////////////////////////////

void SLERP(CQuaternion &a, CQuaternion &b, float t, CQuaternion &dest)
{
	float		fcos = Dot(a, b);
	float		fAngle = acosf(fcos);
	float		c1, c2;
	CQuaternion	q1, q2;

	if (fAngle < MATH3D_EPS) {
		c1 = 1.0f - t;
		c2 = t;
	} else {
		float finvsin=1.0f/sinf(fAngle);
		c1=sinf((1.0f-t)*fAngle)*finvsin;
		c2=sinf(t*fAngle)*finvsin;
	}
	q1 = a * c1;
	q2 = b * c2;
	dest = q1 + q2;
	dest = dest.Unit();

/*	QuatMul(c1,a,&q1);
	QuatMul(c2,b,&q2);
	QuatAdd(&q1,&q2,dest);
	QuatUnit(dest,dest);*/
};

ostream& operator<<(ostream &_out, CQuaternion &_vec)
{
	_out << "[" << _vec.w << ", " << _vec.x << ", " <<
		_vec.y << ", " << _vec.z << "]";
	return _out;
}
